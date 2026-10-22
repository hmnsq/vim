/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * MzScheme interface by Sergey Khorev <sergey.khorev@gmail.com>
 * Based on work by Brent Fulgham <bfulgham@debian.org>
 * (Based on lots of help from Matthew Flatt)
 *
 * This consists of six parts:
 * 1. MzScheme interpreter main program
 * 2. Routines that handle the external interface between MzScheme and
 *    Vim.
 * 3. MzScheme input/output handlers: writes output via [e]msg().
 * 4. Implementation of the Vim Features for MzScheme
 * 5. Vim Window-related Manipulation Functions.
 * 6. Vim Buffer-related Manipulation Functions
 *
 * NOTES
 * 1. Memory, allocated with scheme_malloc*, need not to be freed explicitly,
 *    garbage collector will do it self
 * 2. Requires at least NORMAL features. I can't imagine why one may want
 *    to build with TINY features but with MzScheme interface.
 * 3. I don't use K&R-style functions. Anyways, MzScheme headers are ANSI.
 */

#അന്തര്ഭാവ "vim.h"

#അന്തര്ഭാവ "if_mzsch.h"

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#എങ്കിൽ defined(FEAT_MZSCHEME)

/*
 * scheme_register_tls_space is only available on 32-bit Windows until
 * racket-6.3.  See
 * http://docs.racket-lang.org/inside/im_memoryalloc.html?q=scheme_register_tls_space
 */
#എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_VERSION_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#endif

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#എങ്കിൽ defined(MZ_PRECISE_GC) && MZSCHEME_VERSION_MAJOR >= 400 \
    || MZSCHEME_VERSION_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#endif

// Base data structures
#define SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_buffer_type)
#define SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_window_type)

തരനിർവചനം ഘടന
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_buffer;

#define INVALID_BUFFER_മൂല്യം ((buf_T *)(-1))

തരനിർവചനം ഘടന
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_window;

#define INVALID_WINDOW_മൂല്യം ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
തരനിർവചനം ഘടന
{
    Scheme_Closed_Prim	*prim;
    അക്ഷരം	*name;
    പൂർണ്ണസംഖ്യ		mina;	// arity information
    പൂർണ്ണസംഖ്യ		maxa;
} Vim_Prim;

തരനിർവചനം ഘടന
{
    അക്ഷരം	    *name;
    Scheme_Object   *port;
} Port_Info;

/*
 *========================================================================
 *  Vim-Control Commands
 *========================================================================
 */
/*
 *========================================================================
 *  Utility functions for the vim/mzscheme interface
 *========================================================================
 */
#ifdef HAVE_SANDBOX
സ്ഥിതം Scheme_Object *sandbox_file_guard(പൂർണ്ണസംഖ്യ, Scheme_Object **);
സ്ഥിതം Scheme_Object *sandbox_network_guard(പൂർണ്ണസംഖ്യ, Scheme_Object **);
സ്ഥിതം ശൂന്യം sandbox_check(ശൂന്യം);
#endif
//  Buffer-related commands
സ്ഥിതം Scheme_Object *buffer_new(buf_T *buf);
സ്ഥിതം Scheme_Object *get_buffer_by_അക്കം(ശൂന്യം *, പൂർണ്ണസംഖ്യ, Scheme_Object **);
സ്ഥിതം vim_mz_buffer *get_vim_curr_buffer(ശൂന്യം);

//  Window-related commands
സ്ഥിതം Scheme_Object *window_new(win_T *win);
സ്ഥിതം vim_mz_window *get_vim_curr_window(ശൂന്യം);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ vim_error_check(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ do_mzscheme_command(exarg_T *, ശൂന്യം *, Scheme_Closed_Prim *what);
സ്ഥിതം പൂർണ്ണസംഖ്യ startup_mzscheme(ശൂന്യം);
സ്ഥിതം അക്ഷരം *string_to_വരി(Scheme_Object *obj);
#എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#അല്ലെങ്കിൽ
# define OUTPUT_LEN_TYPE long
#endif
സ്ഥിതം ശൂന്യം do_output(അക്ഷരം *mesg, OUTPUT_LEN_TYPE len);
സ്ഥിതം ശൂന്യം do_printf(അക്ഷരം *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
സ്ഥിതം ശൂന്യം do_flush(ശൂന്യം);
സ്ഥിതം Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
സ്ഥിതം Scheme_Object *extract_exn_message(Scheme_Object *v);
സ്ഥിതം Scheme_Object *do_eval(ശൂന്യം *, പൂർണ്ണസംഖ്യ noargc, Scheme_Object **noargv);
സ്ഥിതം Scheme_Object *do_load(ശൂന്യം *, പൂർണ്ണസംഖ്യ noargc, Scheme_Object **noargv);
സ്ഥിതം ശൂന്യം register_vim_exn(ശൂന്യം);
സ്ഥിതം vim_mz_buffer *get_buffer_arg(സ്ഥിരം അക്ഷരം *fname, പൂർണ്ണസംഖ്യ argnum,
	പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv);
സ്ഥിതം vim_mz_window *get_window_arg(സ്ഥിരം അക്ഷരം *fname, പൂർണ്ണസംഖ്യ argnum,
	പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv);
സ്ഥിതം പൂർണ്ണസംഖ്യ വരി_in_range(linenr_T, buf_T *);
സ്ഥിതം ശൂന്യം check_വരി_range(linenr_T, buf_T *);
സ്ഥിതം ശൂന്യം mz_fix_cursor(പൂർണ്ണസംഖ്യ lo, പൂർണ്ണസംഖ്യ hi, പൂർണ്ണസംഖ്യ extra);

സ്ഥിതം പൂർണ്ണസംഖ്യ eval_with_exn_handling(ശൂന്യം *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
സ്ഥിതം ശൂന്യം make_modules(ശൂന്യം);
സ്ഥിതം ശൂന്യം init_exn_catching_apply(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ mzscheme_env_main(Scheme_Env *env, പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv);
സ്ഥിതം പൂർണ്ണസംഖ്യ mzscheme_init(ശൂന്യം);
#ifdef FEAT_EVAL
സ്ഥിതം Scheme_Object *vim_to_mzscheme(typval_T *vim_മൂല്യം);
സ്ഥിതം Scheme_Object *vim_to_mzscheme_impl(typval_T *vim_മൂല്യം, പൂർണ്ണസംഖ്യ depth,
	Scheme_Hash_Table *visited);
സ്ഥിതം പൂർണ്ണസംഖ്യ mzscheme_to_vim(Scheme_Object *obj, typval_T *tv);
സ്ഥിതം പൂർണ്ണസംഖ്യ mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, പൂർണ്ണസംഖ്യ depth,
	Scheme_Hash_Table *visited);
സ്ഥിതം Scheme_Object *vim_funcref(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv);
#endif

#ifdef MZ_PRECISE_GC
സ്ഥിതം പൂർണ്ണസംഖ്യ buffer_size_proc(ശൂന്യം *obj UNUSED)
{
    തിരികെ gc_BYTES_TO_വാക്കുകൾ(വലുപ്പം(vim_mz_buffer));
}
സ്ഥിതം പൂർണ്ണസംഖ്യ buffer_mark_proc(ശൂന്യം *obj)
{
    തിരികെ buffer_size_proc(obj);
}
സ്ഥിതം പൂർണ്ണസംഖ്യ buffer_fixup_proc(ശൂന്യം *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    തിരികെ buffer_size_proc(obj);
}
സ്ഥിതം പൂർണ്ണസംഖ്യ window_size_proc(ശൂന്യം *obj UNUSED)
{
    തിരികെ gc_BYTES_TO_വാക്കുകൾ(വലുപ്പം(vim_mz_window));
}
സ്ഥിതം പൂർണ്ണസംഖ്യ window_mark_proc(ശൂന്യം *obj)
{
    തിരികെ window_size_proc(obj);
}
സ്ഥിതം പൂർണ്ണസംഖ്യ window_fixup_proc(ശൂന്യം *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    തിരികെ window_size_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define WINDOW_REF(win) *(vim_mz_window **)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) *(vim_mz_buffer **)((buf)->b_mzscheme_ref)
#അല്ലെങ്കിൽ
# define WINDOW_REF(win) (vim_mz_window *)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) (vim_mz_buffer *)((buf)->b_mzscheme_ref)
#endif

#എങ്കിൽ defined(DYNAMIC_MZSCHEME)
സ്ഥിതം Scheme_Object *dll_scheme_eof;
സ്ഥിതം Scheme_Object *dll_scheme_false;
സ്ഥിതം Scheme_Object *dll_scheme_void;
സ്ഥിതം Scheme_Object *dll_scheme_null;
സ്ഥിതം Scheme_Object *dll_scheme_true;

സ്ഥിതം Scheme_Thread **dll_scheme_current_thread_ptr;

സ്ഥിതം ശൂന്യം (**dll_scheme_console_printf_ptr)(അക്ഷരം *str, ...);
സ്ഥിതം ശൂന്യം (**dll_scheme_console_output_ptr)(അക്ഷരം *str, OUTPUT_LEN_TYPE len);
സ്ഥിതം ശൂന്യം (**dll_scheme_notify_multithread_ptr)(പൂർണ്ണസംഖ്യ on);

സ്ഥിതം ശൂന്യം *(*dll_GC_malloc)(size_t size_in_bytes);
സ്ഥിതം ശൂന്യം *(*dll_GC_malloc_atomic)(size_t size_in_bytes);
സ്ഥിതം Scheme_Env *(*dll_scheme_basic_env)(ശൂന്യം);
സ്ഥിതം ശൂന്യം (*dll_scheme_check_threads)(ശൂന്യം);
സ്ഥിതം ശൂന്യം (*dll_scheme_register_static)(ശൂന്യം *ptr, long size);
സ്ഥിതം ശൂന്യം (*dll_scheme_set_അട്ടി_base)(ശൂന്യം *base, പൂർണ്ണസംഖ്യ no_auto_statics);
സ്ഥിതം ശൂന്യം (*dll_scheme_add_global)(സ്ഥിരം അക്ഷരം *name, Scheme_Object *val,
	Scheme_Env *env);
സ്ഥിതം ശൂന്യം (*dll_scheme_add_global_ചിഹ്നം)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
സ്ഥിതം Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, പൂർണ്ണസംഖ്യ അക്കം_rands,
	Scheme_Object **rands);
സ്ഥിതം Scheme_Object *(*dll_scheme_builtin_മൂല്യം)(സ്ഥിരം അക്ഷരം *name);
# എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 299
സ്ഥിതം Scheme_Object *(*dll_scheme_byte_string_to_char_string)(Scheme_Object *s);
സ്ഥിതം Scheme_Object *(*dll_scheme_make_path)(സ്ഥിരം അക്ഷരം *chars);
# endif
സ്ഥിതം ശൂന്യം (*dll_scheme_close_നിവേശം_port)(Scheme_Object *port);
സ്ഥിതം ശൂന്യം (*dll_scheme_എണ്ണം_വരികൾ)(Scheme_Object *port);
# എങ്കിൽ MZSCHEME_VERSION_MAJOR < 360
സ്ഥിതം Scheme_Object *(*dll_scheme_current_continuation_marks)(ശൂന്യം);
# അല്ലെങ്കിൽ
സ്ഥിതം Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_tag);
# endif
സ്ഥിതം ശൂന്യം (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *port);
സ്ഥിതം അക്ഷരം *(*dll_scheme_display_to_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
സ്ഥിതം Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	പൂർണ്ണസംഖ്യ അക്കം_rands_2, Scheme_Object **rands, പൂർണ്ണസംഖ്യ val);
സ്ഥിതം ശൂന്യം (*dll_scheme_dont_gc_ptr)(ശൂന്യം *p);
സ്ഥിതം Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
സ്ഥിതം Scheme_Object *(*dll_scheme_eval_string)(സ്ഥിരം അക്ഷരം *str,
	Scheme_Env *env);
സ്ഥിതം Scheme_Object *(*dll_scheme_eval_string_all)(സ്ഥിരം അക്ഷരം *str,
	Scheme_Env *env, പൂർണ്ണസംഖ്യ all);
സ്ഥിതം ശൂന്യം (*dll_scheme_finish_primitive_module)(Scheme_Env *env);
# എങ്കിൽ MZSCHEME_VERSION_MAJOR < 299
സ്ഥിതം അക്ഷരം *(*dll_scheme_format)(അക്ഷരം *format, പൂർണ്ണസംഖ്യ flen, പൂർണ്ണസംഖ്യ argc,
	Scheme_Object **argv, long *rlen);
# അല്ലെങ്കിൽ
സ്ഥിതം അക്ഷരം *(*dll_scheme_format_utf8)(അക്ഷരം *format, പൂർണ്ണസംഖ്യ flen, പൂർണ്ണസംഖ്യ argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
സ്ഥിതം Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, പൂർണ്ണസംഖ്യ pos);
# endif
സ്ഥിതം ശൂന്യം (*dll_scheme_gc_ptr_ok)(ശൂന്യം *p);
# എങ്കിൽ MZSCHEME_VERSION_MAJOR < 299
സ്ഥിതം അക്ഷരം *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	long *len);
# അല്ലെങ്കിൽ
സ്ഥിതം അക്ഷരം *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
സ്ഥിതം Scheme_Object *(*dll_scheme_intern_ചിഹ്നം)(സ്ഥിരം അക്ഷരം *name);
സ്ഥിതം Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *ചിഹ്നം,
	Scheme_Env *env);
സ്ഥിതം Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, ശൂന്യം *data, സ്ഥിരം അക്ഷരം *name, mzshort mina,
     mzshort maxa);
സ്ഥിതം Scheme_Object *(*dll_scheme_make_integer_മൂല്യം)(long i);
സ്ഥിതം Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
സ്ഥിതം Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	സ്ഥിരം അക്ഷരം *name, mzshort mina, mzshort maxa);
# എങ്കിൽ MZSCHEME_VERSION_MAJOR < 299
സ്ഥിതം Scheme_Object *(*dll_scheme_make_string)(സ്ഥിരം അക്ഷരം *chars);
സ്ഥിതം Scheme_Object *(*dll_scheme_make_string_output_port)();
# അല്ലെങ്കിൽ
സ്ഥിതം Scheme_Object *(*dll_scheme_make_byte_string)(സ്ഥിരം അക്ഷരം *chars);
സ്ഥിതം Scheme_Object *(*dll_scheme_make_byte_string_output_port)();
# endif
സ്ഥിതം Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv);
സ്ഥിതം Scheme_Object **(*dll_scheme_make_struct_names)(Scheme_Object *base,
	Scheme_Object *field_names, പൂർണ്ണസംഖ്യ flags, പൂർണ്ണസംഖ്യ *എണ്ണം_out);
സ്ഥിതം Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *base,
	Scheme_Object *parent, Scheme_Object *inspector, പൂർണ്ണസംഖ്യ അക്കം_fields,
	പൂർണ്ണസംഖ്യ അക്കം_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
സ്ഥിതം Scheme_Object **(*dll_scheme_make_struct_മൂല്യങ്ങൾ)(
	Scheme_Object *struct_type, Scheme_Object **names, പൂർണ്ണസംഖ്യ എണ്ണം,
	പൂർണ്ണസംഖ്യ flags);
സ്ഥിതം Scheme_Type (*dll_scheme_make_type)(സ്ഥിരം അക്ഷരം *name);
സ്ഥിതം Scheme_Object *(*dll_scheme_make_vector)(പൂർണ്ണസംഖ്യ size,
	Scheme_Object *fill);
സ്ഥിതം ശൂന്യം *(*dll_scheme_malloc_fail_ok)(ശൂന്യം *(*f)(size_t), size_t);
സ്ഥിതം Scheme_Object *(*dll_scheme_open_നിവേശം_file)(സ്ഥിരം അക്ഷരം *name,
	സ്ഥിരം അക്ഷരം *who);
സ്ഥിതം Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_scheme_proper_list_length)(Scheme_Object *list);
സ്ഥിതം ശൂന്യം (*dll_scheme_raise)(Scheme_Object *exn);
സ്ഥിതം Scheme_Object *(*dll_scheme_read)(Scheme_Object *port);
സ്ഥിതം ശൂന്യം (*dll_scheme_signal_error)(സ്ഥിരം അക്ഷരം *സന്ദേശം, ...);
സ്ഥിതം ശൂന്യം (*dll_scheme_wrong_type)(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *expected,
	പൂർണ്ണസംഖ്യ which, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv);
# എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 299
സ്ഥിതം ശൂന്യം (*dll_scheme_set_param)(Scheme_Config *c, പൂർണ്ണസംഖ്യ pos,
	Scheme_Object *o);
സ്ഥിതം Scheme_Config *(*dll_scheme_current_config)(ശൂന്യം);
സ്ഥിതം Scheme_Object *(*dll_scheme_char_string_to_byte_string)
    (Scheme_Object *s);
സ്ഥിതം Scheme_Object *(*dll_scheme_char_string_to_path)
    (Scheme_Object *s);
സ്ഥിതം ശൂന്യം *(*dll_scheme_set_collects_path)(Scheme_Object *p);
# endif
സ്ഥിതം Scheme_Hash_Table *(*dll_scheme_make_hash_table)(പൂർണ്ണസംഖ്യ type);
സ്ഥിതം ശൂന്യം (*dll_scheme_hash_set)(Scheme_Hash_Table *table,
	Scheme_Object *താക്കോൽ, Scheme_Object *മൂല്യം);
സ്ഥിതം Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_Table *table,
	Scheme_Object *താക്കോൽ);
സ്ഥിതം Scheme_Object *(*dll_scheme_make_double)(ഇരട്ട d);
സ്ഥിതം Scheme_Object *(*dll_scheme_make_sized_byte_string)(അക്ഷരം *chars,
	long len, പൂർണ്ണസംഖ്യ copy);
സ്ഥിതം Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
സ്ഥിതം Scheme_Object *(*dll_scheme_dynamic_wind)(ശൂന്യം (*pre)(ശൂന്യം *), Scheme_Object *(* അസ്ഥിര act)(ശൂന്യം *), ശൂന്യം (* അസ്ഥിര post)(ശൂന്യം *), Scheme_Object *(*jmp_handler)(ശൂന്യം *), ശൂന്യം * അസ്ഥിര data);
# ifdef MZ_PRECISE_GC
സ്ഥിതം ശൂന്യം *(*dll_GC_malloc_one_tagged)(size_t size_in_bytes);
സ്ഥിതം ശൂന്യം (*dll_GC_register_traversers)(short tag, Size_Proc size, Mark_Proc mark, Fixup_Proc fixup, പൂർണ്ണസംഖ്യ is_constant_size, പൂർണ്ണസംഖ്യ is_atomic);
# endif
# എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 400
സ്ഥിതം ശൂന്യം (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
സ്ഥിതം ശൂന്യം **(*dll_scheme_malloc_immobile_box)(ശൂന്യം *p);
സ്ഥിതം ശൂന്യം (*dll_scheme_free_immobile_box)(ശൂന്യം **b);
# endif
# എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_scheme_main_setup)(പൂർണ്ണസംഖ്യ no_auto_statics, Scheme_Env_Main _main, പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv);
#   എങ്കിൽ defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
സ്ഥിതം ശൂന്യം (*dll_scheme_register_tls_space)(ശൂന്യം *tls_space, പൂർണ്ണസംഖ്യ _tls_index);
#   endif
#  endif
#  എങ്കിൽ defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
സ്ഥിതം Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(ശൂന്യം);
#  endif
# endif
# എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 600
സ്ഥിതം ശൂന്യം (*dll_scheme_embedded_load)(intptr_t len, സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ predefined);
സ്ഥിതം ശൂന്യം (*dll_scheme_register_embedded_load)(intptr_t len, സ്ഥിരം അക്ഷരം *s);
സ്ഥിതം ശൂന്യം (*dll_scheme_set_config_path)(Scheme_Object *p);
# endif

# എങ്കിൽ defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  എങ്കിൽ !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
#   define scheme_current_thread (*dll_scheme_current_thread_ptr)
#  endif
#  define scheme_console_printf (*dll_scheme_console_printf_ptr)
#  define scheme_console_output (*dll_scheme_console_output_ptr)
#  define scheme_notify_multithread (*dll_scheme_notify_multithread_ptr)

// and functions in a usual way
#  define GC_malloc dll_GC_malloc
#  define GC_malloc_atomic dll_GC_malloc_atomic

#  define scheme_add_global dll_scheme_add_global
#  define scheme_add_global_ചിഹ്നം dll_scheme_add_global_ചിഹ്നം
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_മൂല്യം dll_scheme_builtin_മൂല്യം
#  എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 299
#   define scheme_byte_string_to_char_string dll_scheme_byte_string_to_char_string
#   define scheme_make_path dll_scheme_make_path
#  endif
#  define scheme_check_threads dll_scheme_check_threads
#  define scheme_close_നിവേശം_port dll_scheme_close_നിവേശം_port
#  define scheme_എണ്ണം_വരികൾ dll_scheme_എണ്ണം_വരികൾ
#  define scheme_current_continuation_marks \
    dll_scheme_current_continuation_marks
#  define scheme_display dll_scheme_display
#  define scheme_display_to_string dll_scheme_display_to_string
#  define scheme_do_eval dll_scheme_do_eval
#  define scheme_dont_gc_ptr dll_scheme_dont_gc_ptr
#  define scheme_eq dll_scheme_eq
#  define scheme_eval dll_scheme_eval
#  define scheme_eval_string dll_scheme_eval_string
#  define scheme_eval_string_all dll_scheme_eval_string_all
#  define scheme_finish_primitive_module dll_scheme_finish_primitive_module
#  എങ്കിൽ MZSCHEME_VERSION_MAJOR < 299
#   define scheme_format dll_scheme_format
#  അല്ലെങ്കിൽ
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  എങ്കിൽ MZSCHEME_VERSION_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  അല്ലെങ്കിൽ
#   define scheme_get_sized_byte_string_output \
    dll_scheme_get_sized_byte_string_output
#   define scheme_get_param dll_scheme_get_param
#  endif
#  define scheme_intern_ചിഹ്നം dll_scheme_intern_ചിഹ്നം
#  define scheme_lookup_global dll_scheme_lookup_global
#  define scheme_make_closed_prim_w_arity dll_scheme_make_closed_prim_w_arity
#  define scheme_make_integer_മൂല്യം dll_scheme_make_integer_മൂല്യം
#  define scheme_make_pair dll_scheme_make_pair
#  define scheme_make_prim_w_arity dll_scheme_make_prim_w_arity
#  എങ്കിൽ MZSCHEME_VERSION_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_port dll_scheme_make_string_output_port
#  അല്ലെങ്കിൽ
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_port \
    dll_scheme_make_byte_string_output_port
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_names dll_scheme_make_struct_names
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_മൂല്യങ്ങൾ dll_scheme_make_struct_മൂല്യങ്ങൾ
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_vector dll_scheme_make_vector
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_നിവേശം_file dll_scheme_open_നിവേശം_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_length dll_scheme_proper_list_length
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_register_static dll_scheme_register_static
#  define scheme_set_അട്ടി_base dll_scheme_set_അട്ടി_base
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 299
#   define scheme_set_param dll_scheme_set_param
#   define scheme_current_config dll_scheme_current_config
#   define scheme_char_string_to_byte_string \
    dll_scheme_char_string_to_byte_string
#   define scheme_char_string_to_path \
    dll_scheme_char_string_to_path
#   define scheme_set_collects_path dll_scheme_set_collects_path
#  endif
#  define scheme_make_hash_table dll_scheme_make_hash_table
#  define scheme_hash_set dll_scheme_hash_set
#  define scheme_hash_get dll_scheme_hash_get
#  define scheme_make_double dll_scheme_make_double
#  define scheme_make_sized_byte_string dll_scheme_make_sized_byte_string
#  define scheme_namespace_require dll_scheme_namespace_require
#  define scheme_dynamic_wind dll_scheme_dynamic_wind
#  ifdef MZ_PRECISE_GC
#   define GC_malloc_one_tagged dll_GC_malloc_one_tagged
#   define GC_register_traversers dll_GC_register_traversers
#  endif
#  എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_setup dll_scheme_main_setup
#    എങ്കിൽ defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
#     define scheme_register_tls_space dll_scheme_register_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 600
#   define scheme_embedded_load dll_scheme_embedded_load
#   define scheme_register_embedded_load dll_scheme_register_embedded_load
#   define scheme_set_config_path dll_scheme_set_config_path
#  endif

#  ifndef PROTO
#   എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 500
#    എങ്കിൽ defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(ശൂന്യം)
{
    തിരികെ dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

തരനിർവചനം ഘടന
{
    അക്ഷരം    *name;
    ശൂന്യം    **ptr;
} Thunk_Info;

സ്ഥിതം Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (ശൂന്യം **)&dll_GC_malloc},
    {"GC_malloc_atomic", (ശൂന്യം **)&dll_GC_malloc_atomic},
    {ശൂന്യ, ശൂന്യ}};

സ്ഥിതം Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (ശൂന്യം **)&dll_scheme_eof},
    {"scheme_false", (ശൂന്യം **)&dll_scheme_false},
    {"scheme_void", (ശൂന്യം **)&dll_scheme_void},
    {"scheme_null", (ശൂന്യം **)&dll_scheme_null},
    {"scheme_true", (ശൂന്യം **)&dll_scheme_true},
# എങ്കിൽ !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
    {"scheme_current_thread", (ശൂന്യം **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (ശൂന്യം **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (ശൂന്യം **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(ശൂന്യം **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (ശൂന്യം **)&dll_scheme_add_global},
    {"scheme_add_global_symbol", (ശൂന്യം **)&dll_scheme_add_global_ചിഹ്നം},
    {"scheme_apply", (ശൂന്യം **)&dll_scheme_apply},
    {"scheme_basic_env", (ശൂന്യം **)&dll_scheme_basic_env},
# എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (ശൂന്യം **)&dll_scheme_byte_string_to_char_string},
    {"scheme_make_path", (ശൂന്യം **)&dll_scheme_make_path},
# endif
    {"scheme_builtin_value", (ശൂന്യം **)&dll_scheme_builtin_മൂല്യം},
    {"scheme_check_threads", (ശൂന്യം **)&dll_scheme_check_threads},
    {"scheme_close_input_port", (ശൂന്യം **)&dll_scheme_close_നിവേശം_port},
    {"scheme_count_lines", (ശൂന്യം **)&dll_scheme_എണ്ണം_വരികൾ},
    {"scheme_current_continuation_marks",
	(ശൂന്യം **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (ശൂന്യം **)&dll_scheme_display},
    {"scheme_display_to_string", (ശൂന്യം **)&dll_scheme_display_to_string},
    {"scheme_do_eval", (ശൂന്യം **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (ശൂന്യം **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (ശൂന്യം **)&dll_scheme_eq},
    {"scheme_eval", (ശൂന്യം **)&dll_scheme_eval},
    {"scheme_eval_string", (ശൂന്യം **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (ശൂന്യം **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(ശൂന്യം **)&dll_scheme_finish_primitive_module},
# എങ്കിൽ MZSCHEME_VERSION_MAJOR < 299
    {"scheme_format", (ശൂന്യം **)&dll_scheme_format},
# അല്ലെങ്കിൽ
    {"scheme_format_utf8", (ശൂന്യം **)&dll_scheme_format_utf8},
    {"scheme_get_param", (ശൂന്യം **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (ശൂന്യം **)&dll_scheme_gc_ptr_ok},
# എങ്കിൽ MZSCHEME_VERSION_MAJOR < 299
    {"scheme_get_sized_string_output",
	(ശൂന്യം **)&dll_scheme_get_sized_string_output},
# അല്ലെങ്കിൽ
    {"scheme_get_sized_byte_string_output",
	(ശൂന്യം **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (ശൂന്യം **)&dll_scheme_intern_ചിഹ്നം},
    {"scheme_lookup_global", (ശൂന്യം **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(ശൂന്യം **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (ശൂന്യം **)&dll_scheme_make_integer_മൂല്യം},
    {"scheme_make_pair", (ശൂന്യം **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (ശൂന്യം **)&dll_scheme_make_prim_w_arity},
# എങ്കിൽ MZSCHEME_VERSION_MAJOR < 299
    {"scheme_make_string", (ശൂന്യം **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(ശൂന്യം **)&dll_scheme_make_string_output_port},
# അല്ലെങ്കിൽ
    {"scheme_make_byte_string", (ശൂന്യം **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(ശൂന്യം **)&dll_scheme_make_byte_string_output_port},
# endif
    {"scheme_make_struct_instance",
	(ശൂന്യം **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (ശൂന്യം **)&dll_scheme_make_struct_names},
    {"scheme_make_struct_type", (ശൂന്യം **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (ശൂന്യം **)&dll_scheme_make_struct_മൂല്യങ്ങൾ},
    {"scheme_make_type", (ശൂന്യം **)&dll_scheme_make_type},
    {"scheme_make_vector", (ശൂന്യം **)&dll_scheme_make_vector},
    {"scheme_malloc_fail_ok", (ശൂന്യം **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (ശൂന്യം **)&dll_scheme_open_നിവേശം_file},
    {"scheme_primitive_module", (ശൂന്യം **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (ശൂന്യം **)&dll_scheme_proper_list_length},
    {"scheme_raise", (ശൂന്യം **)&dll_scheme_raise},
    {"scheme_read", (ശൂന്യം **)&dll_scheme_read},
    {"scheme_register_static", (ശൂന്യം **)&dll_scheme_register_static},
    {"scheme_set_stack_base", (ശൂന്യം **)&dll_scheme_set_അട്ടി_base},
    {"scheme_signal_error", (ശൂന്യം **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (ശൂന്യം **)&dll_scheme_wrong_type},
# എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_set_param", (ശൂന്യം **)&dll_scheme_set_param},
    {"scheme_current_config", (ശൂന്യം **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(ശൂന്യം **)&dll_scheme_char_string_to_byte_string},
    {"scheme_char_string_to_path", (ശൂന്യം **)&dll_scheme_char_string_to_path},
    {"scheme_set_collects_path", (ശൂന്യം **)&dll_scheme_set_collects_path},
# endif
    {"scheme_make_hash_table", (ശൂന്യം **)&dll_scheme_make_hash_table},
    {"scheme_hash_set", (ശൂന്യം **)&dll_scheme_hash_set},
    {"scheme_hash_get", (ശൂന്യം **)&dll_scheme_hash_get},
    {"scheme_make_double", (ശൂന്യം **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (ശൂന്യം **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (ശൂന്യം **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (ശൂന്യം **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (ശൂന്യം **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (ശൂന്യം **)&dll_GC_register_traversers},
# endif
# എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (ശൂന്യം **)&dll_scheme_main_setup},
#   എങ്കിൽ defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
    {"scheme_register_tls_space", (ശൂന്യം **)&dll_scheme_register_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (ശൂന്യം **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (ശൂന്യം **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (ശൂന്യം **)&dll_scheme_free_immobile_box},
# endif
# എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 500
#  എങ്കിൽ defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
    {"scheme_external_get_thread_local_variables", (ശൂന്യം **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 600
    {"scheme_embedded_load", (ശൂന്യം **)&dll_scheme_embedded_load},
    {"scheme_register_embedded_load", (ശൂന്യം **)&dll_scheme_register_embedded_load},
    {"scheme_set_config_path", (ശൂന്യം **)&dll_scheme_set_config_path},
# endif
    {ശൂന്യ, ശൂന്യ}};

സ്ഥിതം HINSTANCE hMzGC = 0;
സ്ഥിതം HINSTANCE hMzSch = 0;

സ്ഥിതം ശൂന്യം dynamic_mzscheme_end(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ mzscheme_runtime_link_init(അക്ഷരം *sch_dll, അക്ഷരം *gc_dll,
	പൂർണ്ണസംഖ്യ verbose);

    സ്ഥിതം പൂർണ്ണസംഖ്യ
mzscheme_runtime_link_init(അക്ഷരം *sch_dll, അക്ഷരം *gc_dll, പൂർണ്ണസംഖ്യ verbose)
{
    Thunk_Info *thunk = ശൂന്യ;

    എങ്കിൽ (hMzGC && hMzSch)
	തിരികെ OK;
    hMzSch = vimLoadLib(sch_dll);
    hMzGC = vimLoadLib(gc_dll);

    എങ്കിൽ (!hMzGC)
    {
	എങ്കിൽ (verbose)
	    semsg(_(e_could_not_load_library_str_str), gc_dll, GetWin32Error());
	തിരികെ FAIL;
    }

    എങ്കിൽ (!hMzSch)
    {
	എങ്കിൽ (verbose)
	    semsg(_(e_could_not_load_library_str_str), sch_dll, GetWin32Error());
	തിരികെ FAIL;
    }

    വേണ്ടി (thunk = mzsch_imports; thunk->name; thunk++)
    {
	എങ്കിൽ ((*thunk->ptr =
		    (ശൂന്യം *)GetProcAddress(hMzSch, thunk->name)) == ശൂന്യ)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    എങ്കിൽ (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    തിരികെ FAIL;
	}
    }
    വേണ്ടി (thunk = mzgc_imports; thunk->name; thunk++)
    {
	എങ്കിൽ ((*thunk->ptr =
		    (ശൂന്യം *)GetProcAddress(hMzGC, thunk->name)) == ശൂന്യ)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    എങ്കിൽ (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    തിരികെ FAIL;
	}
    }
    തിരികെ OK;
}

    പൂർണ്ണസംഖ്യ
mzscheme_enabled(പൂർണ്ണസംഖ്യ verbose)
{
    തിരികെ mzscheme_runtime_link_init(
	    (അക്ഷരം *)p_mzschemedll, (അക്ഷരം *)p_mzschemegcdll, verbose) == OK;
}

    സ്ഥിതം ശൂന്യം
dynamic_mzscheme_end(ശൂന്യം)
{
    എങ്കിൽ (hMzSch)
    {
	FreeLibrary(hMzSch);
	hMzSch = 0;
    }
    എങ്കിൽ (hMzGC)
    {
	FreeLibrary(hMzGC);
	hMzGC = 0;
    }
}
#endif // DYNAMIC_MZSCHEME

#എങ്കിൽ MZSCHEME_VERSION_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, അക്കം) GUARANTEE_STRING(proc, അക്കം)
#അല്ലെങ്കിൽ
    സ്ഥിതം Scheme_Object *
guaranteed_byte_string_arg(അക്ഷരം *proc, പൂർണ്ണസംഖ്യ അക്കം, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    എങ്കിൽ (SCHEME_BYTE_STRINGP(argv[അക്കം]))
    {
	തിരികെ argv[അക്കം];
    }
    അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_CHAR_STRINGP(argv[അക്കം]))
    {
	Scheme_Object *tmp = ശൂന്യ;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, argv[അക്കം]);
	MZ_GC_VAR_IN_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_to_byte_string(argv[അക്കം]);
	MZ_GC_UNREG();
	തിരികെ tmp;
    }
    അല്ലെങ്കിൽ
	scheme_wrong_type(proc, "string", അക്കം, argc, argv);
    // unreachable
    തിരികെ scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, അക്കം) guaranteed_byte_string_arg(proc, അക്കം, argc, argv)
#endif

// need to put it here for dynamic stuff to work
#എങ്കിൽ defined(INCLUDE_MZSCHEME_BASE)
# include "mzscheme_base.c"
#endif

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

സ്ഥിതം Scheme_Type mz_buffer_type;
സ്ഥിതം Scheme_Type mz_window_type;

സ്ഥിതം പൂർണ്ണസംഖ്യ initialized = FALSE;
#ifdef DYNAMIC_MZSCHEME
സ്ഥിതം പൂർണ്ണസംഖ്യ disabled = FALSE;
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ load_base_module_failed = FALSE;

// global environment
സ്ഥിതം Scheme_Env    *environment = ശൂന്യ;
// output/error handlers
സ്ഥിതം Scheme_Object *curout = ശൂന്യ;
സ്ഥിതം Scheme_Object *curerr = ശൂന്യ;
// exn:vim exception
സ്ഥിതം Scheme_Object *exn_catching_apply = ശൂന്യ;
സ്ഥിതം Scheme_Object *exn_p = ശൂന്യ;
സ്ഥിതം Scheme_Object *exn_message = ശൂന്യ;
സ്ഥിതം Scheme_Object *vim_exn = ശൂന്യ; // Vim Error exception

#എങ്കിൽ !defined(MZ_PRECISE_GC) || MZSCHEME_VERSION_MAJOR < 400
സ്ഥിതം ശൂന്യം *അട്ടി_base = ശൂന്യ;
#endif

സ്ഥിതം long range_start;
സ്ഥിതം long range_end;

// MzScheme threads scheduling stuff
സ്ഥിതം പൂർണ്ണസംഖ്യ mz_threads_allow = 0;

#എങ്കിൽ defined(FEAT_GUI_MSWIN)
സ്ഥിതം ശൂന്യം CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
സ്ഥിതം UINT timer_id = 0;
#അല്ലെങ്കിലെങ്കിൽ defined(FEAT_GUI_GTK)
സ്ഥിതം gboolean timer_proc(gpointer);
സ്ഥിതം guint timer_id = 0;
#അല്ലെങ്കിലെങ്കിൽ defined(FEAT_GUI_MOTIF)
സ്ഥിതം ശൂന്യം timer_proc(XtPointer, XtIntervalId *);
സ്ഥിതം XtIntervalId timer_id = (XtIntervalId)0;
#endif

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    ശൂന്യം
mzvim_check_threads(ശൂന്യം)
{
    // Last time MzScheme threads were scheduled
    സ്ഥിതം time_t mz_last_time = 0;

    എങ്കിൽ (mz_threads_allow && p_mzq > 0)
    {
	time_t now = time(ശൂന്യ);

	എങ്കിൽ ((now - mz_last_time) * 1000 > p_mzq)
	{
	    mz_last_time = now;
	    scheme_check_threads();
	}
    }
}
#endif

#എങ്കിൽ defined(MZSCHEME_GUI_THREADS)
സ്ഥിതം ശൂന്യം setup_timer(ശൂന്യം);
സ്ഥിതം ശൂന്യം remove_timer(ശൂന്യം);

// timers are presented in GUI only
# എങ്കിൽ defined(FEAT_GUI_MSWIN)
    സ്ഥിതം ശൂന്യം CALLBACK
timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_സന്ദേശം UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime UNUSED)
# elif defined(FEAT_GUI_GTK)
    സ്ഥിതം gboolean
timer_proc(gpointer data UNUSED)
# elif defined(FEAT_GUI_MOTIF)
    സ്ഥിതം ശൂന്യം
timer_proc(XtPointer timed_out UNUSED, XtIntervalId *interval_id UNUSED)
# endif
{
    scheme_check_threads();
# എങ്കിൽ defined(FEAT_GUI_GTK)
    തിരികെ TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    എങ്കിൽ (mz_threads_allow && p_mzq > 0)
	timer_id = XtAppAddTimeOut(app_context, p_mzq,
		timer_proc, ശൂന്യ);
# endif
}

    സ്ഥിതം ശൂന്യം
setup_timer(ശൂന്യം)
{
# എങ്കിൽ defined(FEAT_GUI_MSWIN)
    timer_id = SetTimer(ശൂന്യ, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_add((guint)p_mzq, (GSourceFunc)timer_proc, ശൂന്യ);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = XtAppAddTimeOut(app_context, p_mzq, timer_proc, ശൂന്യ);
# endif
}

    സ്ഥിതം ശൂന്യം
remove_timer(ശൂന്യം)
{
# എങ്കിൽ defined(FEAT_GUI_MSWIN)
    KillTimer(ശൂന്യ, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_source_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    XtRemoveTimeOut(timer_id);
# endif
    timer_id = 0;
}

#endif // MZSCHEME_GUI_THREADS

    അക്ഷരം *
did_set_mzquantum(optset_T *args UNUSED)
{
#എങ്കിൽ defined(MZSCHEME_GUI_THREADS)
    // reset timer
    എങ്കിൽ (timer_id != 0)
	remove_timer();
    എങ്കിൽ (mz_threads_allow && p_mzq > 0 && gui.in_use)
	setup_timer();
#endif
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം ശൂന്യം
notify_multithread(പൂർണ്ണസംഖ്യ on)
{
    mz_threads_allow = on;
#ifdef MZSCHEME_GUI_THREADS
    എങ്കിൽ (on && timer_id == 0 && p_mzq > 0 && gui.in_use)
	setup_timer();
    എങ്കിൽ (!on && timer_id != 0)
	remove_timer();
#endif
}

    ശൂന്യം
mzscheme_end(ശൂന്യം)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#എങ്കിൽ 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_end();
# endif
#endif
}

#എങ്കിൽ HAVE_TLS_SPACE && !defined(VIMDLL)
# എങ്കിൽ defined(_MSC_VER)
സ്ഥിതം __declspec(thread) ശൂന്യം *tls_space;
ബാഹ്യ intptr_t _tls_index;
# elif defined(__MINGW32__)
സ്ഥിതം __thread ശൂന്യം *tls_space;
ബാഹ്യ intptr_t _tls_index;
# അല്ലെങ്കിൽ
സ്ഥിതം THREAD_LOCAL ശൂന്യം *tls_space;
സ്ഥിതം intptr_t _tls_index = 0;
# endif
#endif

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    പൂർണ്ണസംഖ്യ
mzscheme_main(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    argc = 0;
    അക്ഷരം    *argv = ശൂന്യ;

#ifdef DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    എങ്കിൽ (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	തിരികെ vim_main2();
    }
#endif
#ifdef HAVE_TLS_SPACE
# ifdef VIMDLL
    ശൂന്യം **ptls_space;
    intptr_t tls_index;
    ശൂന്യം (*pget_tls_info)(ശൂന്യം ***ptls_space, intptr_t *ptls_index);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (ശൂന്യം *)GetProcAddress(
				GetModuleHandle(ശൂന്യ), "get_tls_info");
    എങ്കിൽ (pget_tls_info == ശൂന്യ)
    {
	disabled = TRUE;
	തിരികെ vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_index);
    scheme_register_tls_space(ptls_space, tls_index);
# അല്ലെങ്കിൽ
    scheme_register_tls_space(&tls_space, _tls_index);
# endif
#endif
#ifdef TRAMPOLINED_MZVIM_STARTUP
    തിരികെ scheme_main_setup(TRUE, mzscheme_env_main, argc, &argv);
#അല്ലെങ്കിൽ
    തിരികെ mzscheme_env_main(ശൂന്യ, argc, &argv);
#endif
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
mzscheme_env_main(Scheme_Env *env, പൂർണ്ണസംഖ്യ argc UNUSED, അക്ഷരം **argv UNUSED)
{
#ifdef TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#അല്ലെങ്കിൽ
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = ശൂന്യ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, dummy);

    അട്ടി_base = &__gc_var_stack__;
# അല്ലെങ്കിൽ
    പൂർണ്ണസംഖ്യ dummy = 0;
    അട്ടി_base = (ശൂന്യം *)&dummy;
# endif
#endif

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    തിരികെ 0;
}

    സ്ഥിതം Scheme_Object*
load_base_module(ശൂന്യം *data)
{
    scheme_namespace_require(scheme_intern_ചിഹ്നം((അക്ഷരം *)data));
    തിരികെ scheme_null;
}

    സ്ഥിതം Scheme_Object *
load_base_module_on_error(ശൂന്യം *data UNUSED)
{
    load_base_module_failed = TRUE;
    തിരികെ scheme_null;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
startup_mzscheme(ശൂന്യം)
{
#ifndef TRAMPOLINED_MZVIM_STARTUP
    scheme_set_അട്ടി_base(അട്ടി_base, 1);
#endif

#ifndef TRAMPOLINED_MZVIM_STARTUP
    // in newer versions of precise GC the initial env has been created
    environment = scheme_basic_env();
#endif

    MZ_REGISTER_STATIC(environment);
    MZ_REGISTER_STATIC(curout);
    MZ_REGISTER_STATIC(curerr);
    MZ_REGISTER_STATIC(exn_catching_apply);
    MZ_REGISTER_STATIC(exn_p);
    MZ_REGISTER_STATIC(exn_message);
    MZ_REGISTER_STATIC(vim_exn);

    MZ_GC_CHECK();

#ifdef INCLUDE_MZSCHEME_BASE
    // invoke function from generated and included mzscheme_base.c
    declare_modules(environment);
#endif

    // setup 'current-library-collection-paths' parameter
#എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 299
    {
	Scheme_Object *coll_path = ശൂന്യ;
	പൂർണ്ണസംഖ്യ mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, coll_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	എങ്കിൽ (s != ശൂന്യ)
	{
	    coll_path = scheme_make_path((അക്ഷരം *)s);
	    MZ_GC_CHECK();
	    എങ്കിൽ (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	എങ്കിൽ (coll_path == ശൂന്യ)
	{
	    coll_path = scheme_make_path(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	എങ്കിൽ (coll_path != ശൂന്യ)
	{
	    scheme_set_collects_path(coll_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#അല്ലെങ്കിൽ
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = ശൂന്യ;
	Scheme_Object	*coll_pair = ശൂന്യ;
	Scheme_Config	*config = ശൂന്യ;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_IN_REG(0, coll_string);
	MZ_GC_VAR_IN_REG(1, coll_pair);
	MZ_GC_VAR_IN_REG(2, config);
	MZ_GC_REG();
	coll_string = scheme_make_byte_string(MZSCHEME_COLLECTS);
	MZ_GC_CHECK();
	coll_pair = scheme_make_pair(coll_string, scheme_null);
	MZ_GC_CHECK();
	config = scheme_current_config();
	MZ_GC_CHECK();
	scheme_set_param(config, MZCONFIG_COLLECTION_PATHS, coll_pair);
	MZ_GC_CHECK();
	MZ_GC_UNREG();
    }
# endif
#endif

#എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 600
    {
	Scheme_Object *config_path = ശൂന്യ;
	പൂർണ്ണസംഖ്യ mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, config_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	എങ്കിൽ (s != ശൂന്യ)
	{
	    config_path = scheme_make_path((അക്ഷരം *)s);
	    MZ_GC_CHECK();
	    എങ്കിൽ (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	എങ്കിൽ (config_path == ശൂന്യ)
	{
	    config_path = scheme_make_path(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	എങ്കിൽ (config_path != ശൂന്യ)
	{
	    scheme_set_config_path(config_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#endif

#എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#endif

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(ശൂന്യ, load_base_module, ശൂന്യ,
				    load_base_module_on_error, "racket/base");
	എങ്കിൽ (load_base_module_failed)
	{
	    load_base_module_failed = FALSE;
	    scheme_dynamic_wind(ശൂന്യ, load_base_module, ശൂന്യ,
				    load_base_module_on_error, "scheme/base");
	    എങ്കിൽ (load_base_module_failed)
		തിരികെ -1;
	}
    }

    register_vim_exn();
    // use new environment to initialise exception handling
    init_exn_catching_apply();

    // redirect output
    scheme_console_output = do_output;
    scheme_console_printf = do_printf;

#ifdef HAVE_SANDBOX
    {
	Scheme_Object	*make_security_guard = ശൂന്യ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, make_security_guard);
	MZ_GC_REG();

# എങ്കിൽ MZSCHEME_VERSION_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_ചിഹ്നം = ശൂന്യ;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, make_security_guard_ചിഹ്നം);
	    MZ_GC_REG();
	    make_security_guard_ചിഹ്നം = scheme_intern_ചിഹ്നം("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_ചിഹ്നം, environment);
	    MZ_GC_UNREG();
	}
# അല്ലെങ്കിൽ
	make_security_guard = scheme_builtin_മൂല്യം("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	എങ്കിൽ (make_security_guard != ശൂന്യ)
	{
	    Scheme_Object   *args[3] = {ശൂന്യ, ശൂന്യ, ശൂന്യ};
	    Scheme_Object   *guard = ശൂന്യ;
	    Scheme_Config   *config = ശൂന്യ;
	    MZ_GC_DECL_REG(5);
	    MZ_GC_ARRAY_VAR_IN_REG(0, args, 3);
	    MZ_GC_VAR_IN_REG(3, guard);
	    MZ_GC_VAR_IN_REG(4, config);
	    MZ_GC_REG();
	    config = scheme_current_config();
	    MZ_GC_CHECK();
	    args[0] = scheme_get_param(config, MZCONFIG_SECURITY_GUARD);
	    MZ_GC_CHECK();
	    args[1] = scheme_make_prim_w_arity(sandbox_file_guard,
		    "sandbox-file-guard", 3, 3);
	    args[2] = scheme_make_prim_w_arity(sandbox_network_guard,
		    "sandbox-network-guard", 4, 4);
	    guard = scheme_apply(make_security_guard, 3, args);
	    MZ_GC_CHECK();
	    scheme_set_param(config, MZCONFIG_SECURITY_GUARD, guard);
	    MZ_GC_CHECK();
	    MZ_GC_UNREG();
	}
	MZ_GC_UNREG();
    }
#endif
    // Create buffer and window types for use in Scheme code
    mz_buffer_type = scheme_make_type("<vim-buffer>");
    MZ_GC_CHECK();
    mz_window_type = scheme_make_type("<vim-window>");
    MZ_GC_CHECK();
#ifdef MZ_PRECISE_GC
    GC_register_traversers(mz_buffer_type,
	    buffer_size_proc, buffer_mark_proc, buffer_fixup_proc,
	    TRUE, TRUE);
    GC_register_traversers(mz_window_type,
	    window_size_proc, window_mark_proc, window_fixup_proc,
	    TRUE, TRUE);
#endif

    make_modules();

    /*
     * setup callback to receive notifications
     * whether thread scheduling is (or not) required
     */
    scheme_notify_multithread = notify_multithread;

    തിരികെ 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mzscheme_init(ശൂന്യം)
{
    എങ്കിൽ (!initialized)
    {
#ifdef DYNAMIC_MZSCHEME
	എങ്കിൽ (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    തിരികെ -1;
	}
#endif
	എങ്കിൽ (load_base_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_base_module_could_not_be_loaded));
	    തിരികെ -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = ശൂന്യ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, config);
	MZ_GC_REG();
	config = scheme_current_config();
	MZ_GC_CHECK();
	// recreate ports each call effectively clearing these ones
	curout = scheme_make_byte_string_output_port();
	MZ_GC_CHECK();
	curerr = scheme_make_byte_string_output_port();
	MZ_GC_CHECK();
	scheme_set_param(config, MZCONFIG_OUTPUT_PORT, curout);
	MZ_GC_CHECK();
	scheme_set_param(config, MZCONFIG_ERROR_PORT, curerr);
	MZ_GC_CHECK();
	MZ_GC_UNREG();
    }

    തിരികെ 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval_with_exn_handling(
    ശൂന്യം		*data,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *മൂല്യം = ശൂന്യ;
    Scheme_Object   *exn = ശൂന്യ;
    Scheme_Object   *prim = ശൂന്യ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, മൂല്യം);
    MZ_GC_VAR_IN_REG(1, exn);
    MZ_GC_VAR_IN_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, data, "mzvim", 0, 0);
    MZ_GC_CHECK();
    മൂല്യം = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    എങ്കിൽ (!മൂല്യം)
    {
	മൂല്യം = extract_exn_message(exn);
	// Got an exn?
	എങ്കിൽ (മൂല്യം)
	{
	    scheme_display(മൂല്യം, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	തിരികെ FAIL;
    }

    എങ്കിൽ (ret != ശൂന്യ)	// if pointer to retval supported give it up
	*ret = മൂല്യം;
    // Print any result, as long as it's not a void
    അല്ലെങ്കിൽ എങ്കിൽ (!SCHEME_VOIDP(മൂല്യം))
    {
	scheme_display(മൂല്യം, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    തിരികെ OK;
}

/*
 * :mzscheme
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
do_mzscheme_command(exarg_T *eap, ശൂന്യം *data, Scheme_Closed_Prim *what)
{
    എങ്കിൽ (mzscheme_init())
	തിരികെ FAIL;

    range_start = eap->line1;
    range_end = eap->line2;

    തിരികെ eval_with_exn_handling(data, what, ശൂന്യ);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    ശൂന്യം
mzscheme_buffer_free(buf_T *buf)
{
    എങ്കിൽ (buf->b_mzscheme_ref == ശൂന്യ)
	തിരികെ;

    vim_mz_buffer *bp = ശൂന്യ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, bp);
    MZ_GC_REG();

    bp = BUFFER_REF(buf);
    bp->buf = INVALID_BUFFER_മൂല്യം;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#അല്ലെങ്കിൽ
    scheme_gc_ptr_ok(bp);
#endif
    buf->b_mzscheme_ref = ശൂന്യ;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    ശൂന്യം
mzscheme_window_free(win_T *win)
{
    എങ്കിൽ (win->w_mzscheme_ref == ശൂന്യ)
	തിരികെ;

    vim_mz_window *wp = ശൂന്യ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, wp);
    MZ_GC_REG();
    wp = WINDOW_REF(win);
    wp->win = INVALID_WINDOW_മൂല്യം;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#അല്ലെങ്കിൽ
    scheme_gc_ptr_ok(wp);
#endif
    win->w_mzscheme_ref = ശൂന്യ;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * ":mzscheme" (or ":mz")
 */
    ശൂന്യം
ex_mzscheme(exarg_T *eap)
{
    char_u	*script;

    script = script_get(eap, eap->arg);
    എങ്കിൽ (eap->skip)
	തിരികെ;

    എങ്കിൽ (script == ശൂന്യ)
	do_mzscheme_command(eap, eap->arg, do_eval);
    അല്ലെങ്കിൽ
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    സ്ഥിതം Scheme_Object *
do_load(ശൂന്യം *data, പൂർണ്ണസംഖ്യ noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    Scheme_Object   *വ്യഞ്ജകം = ശൂന്യ;
    Scheme_Object   *result = ശൂന്യ;
    അക്ഷരം	    *file = ശൂന്യ;
    Port_Info	    *pinfo = (Port_Info *)data;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, വ്യഞ്ജകം);
    MZ_GC_VAR_IN_REG(1, result);
    MZ_GC_VAR_IN_REG(2, file);
    MZ_GC_REG();

    file = (അക്ഷരം *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->port = scheme_open_നിവേശം_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_എണ്ണം_വരികൾ(pinfo->port);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    ആകുമ്പോൾ (!SCHEME_EOFP(വ്യഞ്ജകം = scheme_read(pinfo->port)))
    {
	result = scheme_eval(വ്യഞ്ജകം, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_നിവേശം_port(pinfo->port);
    MZ_GC_CHECK();
    pinfo->port = ശൂന്യ;
    MZ_GC_UNREG();
    തിരികെ result;
}

/*
 * :mzfile
 */
    ശൂന്യം
ex_mzfile(exarg_T *eap)
{
    Port_Info	pinfo = {ശൂന്യ, ശൂന്യ};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, pinfo.port);
    MZ_GC_REG();

    pinfo.name = (അക്ഷരം *)eap->arg;
    എങ്കിൽ (do_mzscheme_command(eap, &pinfo, do_load) != OK
	    && pinfo.port != ശൂന്യ)	// looks like port was not closed
    {
	scheme_close_നിവേശം_port(pinfo.port);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
}


/*
 *========================================================================
 * Exception handling code -- cribbed form the MzScheme sources and
 * Matthew Flatt's "Inside PLT MzScheme" document.
 *========================================================================
 */
    സ്ഥിതം ശൂന്യം
init_exn_catching_apply(ശൂന്യം)
{
    എങ്കിൽ (exn_catching_apply)
	തിരികെ;

    അക്ഷരം *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_മൂല്യം("exn?");
    MZ_GC_CHECK();
    exn_message = scheme_builtin_മൂല്യം("exn-message");
    MZ_GC_CHECK();
}

/*
 * This function applies a thunk, returning the Scheme value if there's
 * no exception, otherwise returning NULL and setting *exn to the raised
 * value (usually an exn structure).
 */
    സ്ഥിതം Scheme_Object *
_apply_thunk_catch_exceptions(Scheme_Object *f, Scheme_Object **exn)
{
    Scheme_Object *v;

    v = _scheme_apply(exn_catching_apply, 1, &f);
    // v is a pair: (cons #t value) or (cons #f exn)

    എങ്കിൽ (SCHEME_TRUEP(SCHEME_CAR(v)))
	തിരികെ SCHEME_CDR(v);
    അല്ലെങ്കിൽ
    {
	*exn = SCHEME_CDR(v);
	തിരികെ ശൂന്യ;
    }
}

    സ്ഥിതം Scheme_Object *
extract_exn_message(Scheme_Object *v)
{
    എങ്കിൽ (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	തിരികെ _scheme_apply(exn_message, 1, &v);
    അല്ലെങ്കിൽ
	തിരികെ ശൂന്യ; // Not an exn structure
}

    സ്ഥിതം Scheme_Object *
do_eval(ശൂന്യം *s, പൂർണ്ണസംഖ്യ noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    തിരികെ scheme_eval_string_all((അക്ഷരം *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    സ്ഥിതം ശൂന്യം
do_intrnl_output(അക്ഷരം *mesg, പൂർണ്ണസംഖ്യ error)
{
    അക്ഷരം *p, *prev;

    prev = mesg;
    p = strchr(prev, '\n');
    ആകുമ്പോൾ (p)
    {
	*p = '\0';
	എങ്കിൽ (error)
	    emsg(prev);
	അല്ലെങ്കിൽ
	    സന്ദേശം(prev);
	prev = p + 1;
	p = strchr(prev, '\n');
    }

    എങ്കിൽ (error)
	emsg(prev);
    അല്ലെങ്കിൽ
	സന്ദേശം(prev);
}

    സ്ഥിതം ശൂന്യം
do_output(അക്ഷരം *mesg, OUTPUT_LEN_TYPE len UNUSED)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    സ്ഥിതം ശൂന്യം
do_err_output(അക്ഷരം *mesg)
{
    do_intrnl_output(mesg, 1);
}

    സ്ഥിതം ശൂന്യം
do_printf(അക്ഷരം *format, ...)
{
    do_intrnl_output(format, 1);
}

    സ്ഥിതം ശൂന്യം
do_flush(ശൂന്യം)
{
    അക്ഷരം *buff;
    OUTPUT_LEN_TYPE length;

    buff = scheme_get_sized_byte_string_output(curerr, &length);
    MZ_GC_CHECK();
    എങ്കിൽ (length)
    {
	do_err_output(buff);
	തിരികെ;
    }

    buff = scheme_get_sized_byte_string_output(curout, &length);
    MZ_GC_CHECK();
    എങ്കിൽ (length)
	do_output(buff, length);
}

/*
 *========================================================================
 *  4. Implementation of the Vim Features for MzScheme
 *========================================================================
 */

/*
 * (command {command-string})
 */
    സ്ഥിതം Scheme_Object *
vim_command(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = ശൂന്യ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_മൂല്യം(cmd), ശൂന്യ, ശൂന്യ, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    തിരികെ scheme_void;
}

/*
 * (eval {expr-string})
 */
    സ്ഥിതം Scheme_Object *
vim_eval(ശൂന്യം *data UNUSED, പൂർണ്ണസംഖ്യ argc UNUSED, Scheme_Object **argv UNUSED)
{
#ifdef FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)data;
    Scheme_Object	*result = ശൂന്യ;
    typval_T		*vim_result;
    Scheme_Object *വ്യഞ്ജകം = ശൂന്യ;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, വ്യഞ്ജകം);
    MZ_GC_REG();
    വ്യഞ്ജകം = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_result = eval_വ്യഞ്ജകം(BYTE_STRING_മൂല്യം(വ്യഞ്ജകം), ശൂന്യ);

    എങ്കിൽ (vim_result == ശൂന്യ)
	raise_vim_exn(_("invalid expression"));

    result = vim_to_mzscheme(vim_result);
    MZ_GC_CHECK();
    free_tv(vim_result);

    MZ_GC_UNREG();
    തിരികെ result;
#അല്ലെങ്കിൽ
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    തിരികെ scheme_false;
#endif
}

/*
 * (range-start)
 */
    സ്ഥിതം Scheme_Object *
get_range_start(
    ശൂന്യം		*data UNUSED,
    പൂർണ്ണസംഖ്യ			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    തിരികെ scheme_make_integer(range_start);
}

/*
 * (range-end)
 */
    സ്ഥിതം Scheme_Object *
get_range_end(ശൂന്യം *data UNUSED, പൂർണ്ണസംഖ്യ argc UNUSED, Scheme_Object **argv UNUSED)
{
    തിരികെ scheme_make_integer(range_end);
}

/*
 * (beep)
 */
    സ്ഥിതം Scheme_Object *
mzscheme_beep(ശൂന്യം *data UNUSED, പൂർണ്ണസംഖ്യ argc UNUSED, Scheme_Object **argv UNUSED)
{
    vim_beep(BO_LANG);
    തിരികെ scheme_void;
}

സ്ഥിതം Scheme_Object *M_global = ശൂന്യ;

/*
 * (get-option {option-name}) [buffer/window]
 */
    സ്ഥിതം Scheme_Object *
get_option(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    long	    മൂല്യം;
    അക്ഷരം	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = ശൂന്യ;
    Scheme_Object   *name = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, rval);
    MZ_GC_VAR_IN_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    എങ്കിൽ (argc > 1)
    {
	എങ്കിൽ (M_global == ശൂന്യ)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_ചിഹ്നം("global");
	    MZ_GC_CHECK();
	}

	എങ്കിൽ (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);

	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	അല്ലെങ്കിൽ
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_മൂല്യം(BYTE_STRING_മൂല്യം(name), &മൂല്യം, (char_u **)&strval,
								  ശൂന്യ, scope);
    curbuf = save_curb;
    curwin = save_curw;

    തിരഞ്ഞെടുപ്പ് (rc)
    {
    അവസ്ഥ gov_bool:
    അവസ്ഥ gov_സംഖ്യ:
	MZ_GC_UNREG();
	തിരികെ scheme_make_integer_മൂല്യം(മൂല്യം);
    അവസ്ഥ gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	തിരികെ rval;
    അവസ്ഥ gov_hidden_bool:
    അവസ്ഥ gov_hidden_സംഖ്യ:
    അവസ്ഥ gov_hidden_string:
	MZ_GC_UNREG();
	raise_vim_exn(_("hidden option"));
	//NOTREACHED
    അവസ്ഥ gov_unknown:
	MZ_GC_UNREG();
	raise_vim_exn(_("unknown option"));
	//NOTREACHED
    }
    // unreachable
    തിരികെ scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    സ്ഥിതം Scheme_Object *
set_option(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    char_u	*command = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = ശൂന്യ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    എങ്കിൽ (argc > 1)
    {
	എങ്കിൽ (M_global == ശൂന്യ)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_ചിഹ്നം("global");
	    MZ_GC_CHECK();
	}

	എങ്കിൽ (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);
	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	അല്ലെങ്കിൽ
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    // do_set can modify cmd, make copy
    command = vim_strsave(BYTE_STRING_മൂല്യം(cmd));
    MZ_GC_UNREG();
    do_set(command, scope);
    vim_free(command);
    update_screen(UPD_NOT_VALID);
    curbuf = save_curb;
    curwin = save_curw;
    raise_if_error();
    തിരികെ scheme_void;
}

/*
 *===========================================================================
 *  5. Vim Window-related Manipulation Functions
 *===========================================================================
 */

/*
 * (curr-win)
 */
    സ്ഥിതം Scheme_Object *
get_curr_win(ശൂന്യം *data UNUSED, പൂർണ്ണസംഖ്യ argc UNUSED, Scheme_Object **argv UNUSED)
{
    തിരികെ (Scheme_Object *)get_vim_curr_window();
}

/*
 * (win-count)
 */
    സ്ഥിതം Scheme_Object *
get_window_എണ്ണം(
    ശൂന്യം		*data UNUSED,
    പൂർണ്ണസംഖ്യ			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    പൂർണ്ണസംഖ്യ	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    തിരികെ scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    സ്ഥിതം Scheme_Object *
get_window_list(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_buffer_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    വേണ്ടി ( ; w != ശൂന്യ; w = w->w_next)
	എങ്കിൽ (w->w_buffer == buf->buf)
	{
	    list = scheme_make_pair(window_new(w), list);
	    MZ_GC_CHECK();
	}

    തിരികെ list;
}

    സ്ഥിതം Scheme_Object *
window_new(win_T *win)
{
    vim_mz_window *self = ശൂന്യ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    എങ്കിൽ (win->w_mzscheme_ref != ശൂന്യ)
	തിരികെ (Scheme_Object *)WINDOW_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, വലുപ്പം(vim_mz_window));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(ശൂന്യ);
#അല്ലെങ്കിൽ
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#endif
    MZ_GC_CHECK();
    WINDOW_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_window_type;

    MZ_GC_UNREG();
    തിരികെ (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    സ്ഥിതം Scheme_Object *
get_window_അക്കം(ശൂന്യം *data UNUSED, പൂർണ്ണസംഖ്യ argc UNUSED, Scheme_Object **argv UNUSED)
{
    പൂർണ്ണസംഖ്യ		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = get_window_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    വേണ്ടി (wp = firstwin; wp != win; wp = wp->w_next)
	++nr;

    തിരികെ scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    സ്ഥിതം Scheme_Object *
get_window_by_അക്കം(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = firstwin;
    പൂർണ്ണസംഖ്യ		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    എങ്കിൽ (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    വേണ്ടി ( ; win != ശൂന്യ; win = win->w_next, --fnum)
	എങ്കിൽ (fnum == 1)	    // to be 1-based
	    തിരികെ window_new(win);

    തിരികെ scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    സ്ഥിതം Scheme_Object *
get_window_buffer(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    തിരികെ buffer_new(win->win->w_buffer);
}

/*
 * (get-win-height [window])
 */
    സ്ഥിതം Scheme_Object *
get_window_height(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    തിരികെ scheme_make_integer(win->win->w_height);
}

/*
 * (set-win-height {height} [window])
 */
    സ്ഥിതം Scheme_Object *
set_window_height(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    പൂർണ്ണസംഖ്യ		    height;

    win = get_window_arg(prim->name, 1, argc, argv);
    height = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    savewin = curwin;
    curwin = win->win;
    win_setheight(height);
    curwin = savewin;

    raise_if_error();
    തിരികെ scheme_void;
}

/*
 * (get-win-width [window])
 */
    സ്ഥിതം Scheme_Object *
get_window_width(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    തിരികെ scheme_make_integer(win->win->w_width);
}

/*
 * (set-win-width {width} [window])
 */
    സ്ഥിതം Scheme_Object *
set_window_width(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    പൂർണ്ണസംഖ്യ		    width = 0;

    win = get_window_arg(prim->name, 1, argc, argv);
    width = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    savewin = curwin;
    curwin = win->win;
    win_setwidth(width);
    curwin = savewin;

    raise_if_error();
    തിരികെ scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    സ്ഥിതം Scheme_Object *
get_cursor(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    pos_T	    pos;

    win = get_window_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    തിരികെ scheme_make_pair(scheme_make_integer_മൂല്യം((long)pos.lnum),
		    scheme_make_integer_മൂല്യം((long)pos.col + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    സ്ഥിതം Scheme_Object *
set_cursor(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    long	    lnum = 0;
    long	    col = 0;

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    win = get_window_arg(prim->name, 1, argc, argv);
    GUARANTEE_PAIR(prim->name, 0);

    എങ്കിൽ (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    col = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_വരി_range(lnum, win->win->w_buffer);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.col = col;
    win->win->w_set_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    തിരികെ scheme_void;
}
/*
 *===========================================================================
 *  6. Vim Buffer-related Manipulation Functions
 *===========================================================================
 */

/*
 * (open-buff {filename})
 */
    സ്ഥിതം Scheme_Object *
mzscheme_open_buffer(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    പൂർണ്ണസംഖ്യ		    അക്കം = 0;
    Scheme_Object   *onum = ശൂന്യ;
    Scheme_Object   *buf = ശൂന്യ;
    Scheme_Object   *fname;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, onum);
    MZ_GC_VAR_IN_REG(1, buf);
    MZ_GC_VAR_IN_REG(2, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    // TODO make open existing file
    അക്കം = buflist_add(BYTE_STRING_മൂല്യം(fname), BLN_LISTED | BLN_CURBUF);

    എങ്കിൽ (അക്കം == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(അക്കം);
    buf = get_buffer_by_അക്കം(data, 1, &onum);
    MZ_GC_UNREG();
    തിരികെ buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    സ്ഥിതം Scheme_Object *
get_buffer_by_അക്കം(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	എങ്കിൽ (buf->b_fnum == fnum)
	    തിരികെ buffer_new(buf);

    തിരികെ scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    സ്ഥിതം Scheme_Object *
get_buffer_by_name(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    Scheme_Object *buffer = ശൂന്യ;
    Scheme_Object *fname = ശൂന്യ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, buffer);
    MZ_GC_VAR_IN_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    buffer = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	എങ്കിൽ (buf->b_ffname == ശൂന്യ || buf->b_sfname == ശൂന്യ)
	    // empty string
	{
	    എങ്കിൽ (BYTE_STRING_മൂല്യം(fname)[0] == NUL)
		buffer = buffer_new(buf);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!fnamecmp(buf->b_ffname, BYTE_STRING_മൂല്യം(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_മൂല്യം(fname)))
	{
	    // either short or long filename matches
	    buffer = buffer_new(buf);
	}
    }

    MZ_GC_UNREG();
    തിരികെ buffer;
}

/*
 * (get-next-buff [buffer])
 */
    സ്ഥിതം Scheme_Object *
get_next_buffer(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    എങ്കിൽ (buf->b_next == ശൂന്യ)
	തിരികെ scheme_false;
    അല്ലെങ്കിൽ
	തിരികെ buffer_new(buf->b_next);
}

/*
 * (get-prev-buff [buffer])
 */
    സ്ഥിതം Scheme_Object *
get_prev_buffer(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    എങ്കിൽ (buf->b_prev == ശൂന്യ)
	തിരികെ scheme_false;
    അല്ലെങ്കിൽ
	തിരികെ buffer_new(buf->b_prev);
}

/*
 * (get-buff-num [buffer])
 */
    സ്ഥിതം Scheme_Object *
get_buffer_അക്കം(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    തിരികെ scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    സ്ഥിതം Scheme_Object *
get_buffer_എണ്ണം(
    ശൂന്യം		*data UNUSED,
    പൂർണ്ണസംഖ്യ			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    buf_T   *b;
    പൂർണ്ണസംഖ്യ	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    തിരികെ scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    സ്ഥിതം Scheme_Object *
get_buffer_name(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    തിരികെ scheme_make_byte_string((അക്ഷരം *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    സ്ഥിതം Scheme_Object *
get_curr_buffer(
    ശൂന്യം		*data UNUSED,
    പൂർണ്ണസംഖ്യ			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    തിരികെ (Scheme_Object *)get_vim_curr_buffer();
}

    സ്ഥിതം Scheme_Object *
buffer_new(buf_T *buf)
{
    vim_mz_buffer *self = ശൂന്യ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    എങ്കിൽ (buf->b_mzscheme_ref)
	തിരികെ (Scheme_Object *)BUFFER_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, വലുപ്പം(vim_mz_buffer));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(ശൂന്യ);
#അല്ലെങ്കിൽ
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#endif
    MZ_GC_CHECK();
    BUFFER_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_buffer_type;

    MZ_GC_UNREG();
    തിരികെ (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    സ്ഥിതം Scheme_Object *
get_buffer_size(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    തിരികെ scheme_make_integer(buf->buf->b_ml.ml_വരി_എണ്ണം);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    സ്ഥിതം Scheme_Object *
get_buffer_വരി(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    പൂർണ്ണസംഖ്യ		    linenr;
    char_u	    *വരി;

    buf = get_buffer_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    വരി = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    തിരികെ scheme_make_byte_string((അക്ഷരം *)വരി);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    സ്ഥിതം Scheme_Object *
get_buffer_വരി_list(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    പൂർണ്ണസംഖ്യ		    i, hi, lo, n;
    Scheme_Object   *list = ശൂന്യ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, list);
    MZ_GC_REG();

    buf = get_buffer_arg(prim->name, 2, argc, argv);
    list = scheme_null;
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    /*
     * Handle some error conditions
     */
    എങ്കിൽ (lo < 0)
	lo = 0;

    എങ്കിൽ (hi < 0)
	hi = 0;
    എങ്കിൽ (hi < lo)
	hi = lo;

    n = hi - lo;

    വേണ്ടി (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (അക്ഷരം *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    തിരികെ list;
}

/*
 * (set-buff-line {linenr} {string/#f} [buffer])
 *
 * Replace a line in the specified buffer. The line number is
 * in Vim format (1-based). The replacement line is given as
 * an MzScheme string object. The object is checked for validity
 * and correct format. An exception is thrown if the values are not
 * the correct format.
 *
 * It returns a Scheme Object that indicates the length of the
 * string changed.
 */
    സ്ഥിതം Scheme_Object *
set_buffer_വരി(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *വരി = ശൂന്യ;
    അക്ഷരം	    *save;
    പൂർണ്ണസംഖ്യ		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, വരി);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    എങ്കിൽ (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    വരി = argv[1];
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    check_വരി_range(n, buf->buf);

    എങ്കിൽ (SCHEME_FALSEP(വരി))
    {
	buf_T	    *savebuf = curbuf;

	curbuf = buf->buf;

	എങ്കിൽ (u_savedel((linenr_T)n, 1L) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	അല്ലെങ്കിൽ എങ്കിൽ (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	എങ്കിൽ (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_വരികൾ_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	തിരികെ scheme_void;
    }
    അല്ലെങ്കിൽ
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_to_വരി(വരി);

	curbuf = buf->buf;

	എങ്കിൽ (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	അല്ലെങ്കിൽ എങ്കിൽ (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	അല്ലെങ്കിൽ
	{
	    vim_free(save);
	    changed_bytes((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	എങ്കിൽ (buf->buf == curwin->w_buffer)
	    check_cursor_col();

	MZ_GC_UNREG();
	raise_if_error();
	തിരികെ scheme_void;
    }
}

    സ്ഥിതം ശൂന്യം
free_array(അക്ഷരം **array)
{
    അക്ഷരം **curr = array;
    ആകുമ്പോൾ (*curr != ശൂന്യ)
	vim_free(*curr++);
    vim_free(array);
}

/*
 * (set-buff-line-list {start} {end} {string-list/#f/null} [buffer])
 *
 * Replace a range of lines in the specified buffer. The line numbers are in
 * Vim format (1-based). The range is from lo up to, but not including, hi.
 * The replacement lines are given as a Scheme list of string objects. The
 * list is checked for validity and correct format.
 *
 * Errors are returned as a value of FAIL. The return value is OK on success.
 * If OK is returned and len_change is not NULL, *len_change is set to the
 * change in the buffer length.
 */
    സ്ഥിതം Scheme_Object *
set_buffer_വരി_list(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = ശൂന്യ;
    Scheme_Object   *വരി_list = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		    i, old_len, new_len, hi, lo;
    long	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, വരി_list);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    എങ്കിൽ (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    വരി_list = argv[2];
    buf = get_buffer_arg(prim->name, 3, argc, argv);
    old_len = hi - lo;
    എങ്കിൽ (old_len < 0) // process inverse values wisely
    {
	i = lo;
	lo = hi;
	hi = i;
	old_len = -old_len;
    }
    extra = 0;

    check_വരി_range(lo, buf->buf);	    // inclusive
    check_വരി_range(hi - 1, buf->buf);	    // exclusive

    എങ്കിൽ (SCHEME_FALSEP(വരി_list) || SCHEME_NULLP(വരി_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	എങ്കിൽ (u_savedel((linenr_T)lo, (long)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	അല്ലെങ്കിൽ
	{
	    വേണ്ടി (i = 0; i < old_len; i++)
		എങ്കിൽ (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    എങ്കിൽ (buf->buf == curwin->w_buffer)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_വരികൾ_mark((linenr_T)lo, (long)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	തിരികെ scheme_void;
    }
    അല്ലെങ്കിൽ
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_length(വരി_list);
	MZ_GC_CHECK();
	എങ്കിൽ (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	അല്ലെങ്കിൽ
	{
	    അക്ഷരം		**array = ശൂന്യ;
	    Scheme_Object   *വരി = ശൂന്യ;
	    Scheme_Object   *rest = ശൂന്യ;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_IN_REG(0, വരി);
	    MZ_GC_VAR_IN_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(അക്ഷരം *, new_len + 1);

	    rest = വരി_list;
	    വേണ്ടി (i = 0; i < new_len; ++i)
	    {
		വരി = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		എങ്കിൽ (!SCHEME_STRINGP(വരി))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_to_വരി(വരി);
	    }

	    curbuf = buf->buf;

	    എങ്കിൽ (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    {
		curbuf = savebuf;
		free_array(array);
		raise_vim_exn(_("cannot save undo information"));
	    }

	    /*
	     * If the size of the range is reducing (ie, new_len < old_len) we
	     * need to delete some old_len. We do this at the start, by
	     * repeatedly deleting line "lo".
	     */
	    വേണ്ടി (i = 0; i < old_len - new_len; ++i)
	    {
		എങ്കിൽ (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot delete line"));
		}
		extra--;
	    }

	    /*
	     * For as long as possible, replace the existing old_len with the
	     * new old_len. This is a more efficient operation, as it requires
	     * less memory allocation and freeing.
	     */
	    വേണ്ടി (i = 0; i < old_len && i < new_len; i++)
		എങ്കിൽ (ml_replace((linenr_T)(lo+i), (char_u *)array[i], TRUE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot replace line"));
		}

	    /*
	     * Now we may need to insert the remaining new_len.  We don't need to
	     * free the string passed back because MzScheme has control of that
	     * memory.
	     */
	    ആകുമ്പോൾ (i < new_len)
	    {
		എങ്കിൽ (ml_append((linenr_T)(lo + i - 1),
			    (char_u *)array[i], 0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}
		++i;
		++extra;
	    }
	    MZ_GC_UNREG();
	    free_array(array);
	}

	/*
	 * Adjust marks. Invalidate any which lie in the
	 * changed range, and move any in the remainder of the buffer.
	 */
	mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (long)MAXLNUM, (long)extra);
	changed_വരികൾ((linenr_T)lo, 0, (linenr_T)hi, (long)extra);

	എങ്കിൽ (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	തിരികെ scheme_void;
    }
}

/*
 * (insert-buff-line-list {linenr} {string/string-list} [buffer])
 *
 * Insert a number of lines into the specified buffer after the specified line.
 * The line number is in Vim format (1-based). The lines to be inserted are
 * given as an MzScheme list of string objects or as a single string. The lines
 * to be added are checked for validity and correct format. Errors are
 * returned as a value of FAIL.  The return value is OK on success.
 * If OK is returned and len_change is not NULL, *len_change
 * is set to the change in the buffer length.
 */
    സ്ഥിതം Scheme_Object *
തിരുകുക_buffer_വരി_list(ശൂന്യം *data, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = ശൂന്യ;
    Scheme_Object   *list = ശൂന്യ;
    അക്ഷരം	    *str = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		    i, n, size;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, list);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    /*
     * First of all, we check the type of the supplied MzScheme object.
     * It must be a string or a list, or the call is in error.
     */
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    list = argv[1];

    എങ്കിൽ (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    എങ്കിൽ (n != 0)	    // 0 can be used in insert
	check_വരി_range(n, buf->buf);
    എങ്കിൽ (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_to_വരി(list);
	curbuf = buf->buf;

	എങ്കിൽ (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	അല്ലെങ്കിൽ എങ്കിൽ (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	അല്ലെങ്കിൽ
	{
	    vim_free(str);
	    appended_വരികൾ_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	തിരികെ scheme_void;
    }

    // List
    size = scheme_proper_list_length(list);
    MZ_GC_CHECK();
    എങ്കിൽ (size < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    അല്ലെങ്കിൽ
    {
	Scheme_Object   *വരി = ശൂന്യ;
	Scheme_Object   *rest = ശൂന്യ;
	അക്ഷരം		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, വരി);
	MZ_GC_VAR_IN_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(അക്ഷരം *, size + 1);

	rest = list;
	വേണ്ടി (i = 0; i < size; ++i)
	{
	    വരി = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_to_വരി(വരി);
	}

	curbuf = buf->buf;

	എങ്കിൽ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	അല്ലെങ്കിൽ
	{
	    വേണ്ടി (i = 0; i < size; ++i)
		എങ്കിൽ (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    എങ്കിൽ (i > 0)
		appended_വരികൾ_mark((linenr_T)n, (long)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    തിരികെ scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    സ്ഥിതം Scheme_Object *
vim_bufferp(ശൂന്യം *data UNUSED, പൂർണ്ണസംഖ്യ argc UNUSED, Scheme_Object **argv)
{
    എങ്കിൽ (SCHEME_VIMBUFFERP(argv[0]))
	തിരികെ scheme_true;
    അല്ലെങ്കിൽ
	തിരികെ scheme_false;
}

/*
 * (win? obj)
 */
    സ്ഥിതം Scheme_Object *
vim_windowp(ശൂന്യം *data UNUSED, പൂർണ്ണസംഖ്യ argc UNUSED, Scheme_Object **argv)
{
    എങ്കിൽ (SCHEME_VIMWINDOWP(argv[0]))
	തിരികെ scheme_true;
    അല്ലെങ്കിൽ
	തിരികെ scheme_false;
}

/*
 * (buff-valid? obj)
 */
    സ്ഥിതം Scheme_Object *
vim_buffer_validp(ശൂന്യം *data UNUSED, പൂർണ്ണസംഖ്യ argc UNUSED, Scheme_Object **argv)
{
    എങ്കിൽ (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_buffer *)argv[0])->buf != INVALID_BUFFER_മൂല്യം)
	തിരികെ scheme_true;
    അല്ലെങ്കിൽ
	തിരികെ scheme_false;
}

/*
 * (win-valid? obj)
 */
    സ്ഥിതം Scheme_Object *
vim_window_validp(ശൂന്യം *data UNUSED, പൂർണ്ണസംഖ്യ argc UNUSED, Scheme_Object **argv)
{
    എങ്കിൽ (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_window *)argv[0])->win != INVALID_WINDOW_മൂല്യം)
	തിരികെ scheme_true;
    അല്ലെങ്കിൽ
	തിരികെ scheme_false;
}

/*
 *===========================================================================
 * Utilities
 *===========================================================================
 */

/*
 * Convert an MzScheme string into a Vim line.
 *
 * All internal nulls are replaced by newline characters.
 * It is an error for the string to contain newline characters.
 *
 * Returns pointer to Vim allocated memory
 */
    സ്ഥിതം അക്ഷരം *
string_to_വരി(Scheme_Object *obj)
{
    അക്ഷരം	*scheme_str = ശൂന്യ;
    അക്ഷരം	*vim_str = ശൂന്യ;
    OUTPUT_LEN_TYPE	len;
    പൂർണ്ണസംഖ്യ		i;

    scheme_str = scheme_display_to_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    എങ്കിൽ (memchr(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    വേണ്ടി (i = 0; i < len; ++i)
    {
	എങ്കിൽ (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	അല്ലെങ്കിൽ
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    തിരികെ vim_str;
}

#ifdef FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    സ്ഥിതം Scheme_Object *
vim_to_mzscheme(typval_T *vim_മൂല്യം)
{
    Scheme_Object	*result = ശൂന്യ;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_Table	*visited = ശൂന്യ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    result = vim_to_mzscheme_impl(vim_മൂല്യം, 1, visited);

    MZ_GC_UNREG();
    തിരികെ result;
}

    സ്ഥിതം Scheme_Object *
vim_to_mzscheme_impl(
    typval_T		*vim_മൂല്യം,
    പൂർണ്ണസംഖ്യ			depth,
    Scheme_Hash_Table	*visited)
{
    Scheme_Object   *result = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		    new_മൂല്യം_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    // Avoid infinite recursion
    എങ്കിൽ (depth > 100)
    {
	MZ_GC_UNREG();
	തിരികെ scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    result = scheme_hash_get(visited, (Scheme_Object *)vim_മൂല്യം);
    MZ_GC_CHECK();
    എങ്കിൽ (result != ശൂന്യ) // found, do nothing
	new_മൂല്യം_2 = FALSE;
    അല്ലെങ്കിൽ എങ്കിൽ (vim_മൂല്യം->v_type == VAR_STRING)
    {
	result = scheme_make_byte_string((അക്ഷരം *)vim_മൂല്യം->vval.v_string);
	MZ_GC_CHECK();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vim_മൂല്യം->v_type == VAR_സംഖ്യ)
    {
	result = scheme_make_integer((long)vim_മൂല്യം->vval.v_സംഖ്യ);
	MZ_GC_CHECK();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vim_മൂല്യം->v_type == VAR_FLOAT)
    {
	result = scheme_make_double((ഇരട്ട)vim_മൂല്യം->vval.v_float);
	MZ_GC_CHECK();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vim_മൂല്യം->v_type == VAR_LIST)
    {
	list_T		*list = vim_മൂല്യം->vval.v_list;
	listitem_T	*curr;

	എങ്കിൽ (list == ശൂന്യ || list->lv_first == ശൂന്യ)
	    result = scheme_null;
	അല്ലെങ്കിൽ
	{
	    Scheme_Object   *obj = ശൂന്യ;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_last;
	    obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
	    result = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    ആകുമ്പോൾ (curr != list->lv_first)
	    {
		curr = curr->li_prev;
		obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
		result = scheme_make_pair(obj, result);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vim_മൂല്യം->v_type == VAR_DICT)
    {
	Scheme_Object	  *താക്കോൽ = ശൂന്യ;
	Scheme_Object	  *obj = ശൂന്യ;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, താക്കോൽ);
	MZ_GC_VAR_IN_REG(1, obj);
	MZ_GC_REG();

	result = (Scheme_Object *)scheme_make_hash_table(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	എങ്കിൽ (vim_മൂല്യം->vval.v_dict != ശൂന്യ)
	{
	    hashtab_T	*ht = &vim_മൂല്യം->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_used;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_ഇനങ്ങൾ(ht, hi, todo)
	    {
		എങ്കിൽ (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_to_mzscheme_impl(&di->di_tv, depth + 1, visited);
		    താക്കോൽ = scheme_make_byte_string((അക്ഷരം *)hi->hi_താക്കോൽ);
		    MZ_GC_CHECK();
		    scheme_hash_set((Scheme_Hash_Table *)result, താക്കോൽ, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vim_മൂല്യം->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = ശൂന്യ;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((അക്ഷരം *)vim_മൂല്യം->vval.v_string);
	MZ_GC_CHECK();
	result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(സ്ഥിരം അക്ഷരം *)BYTE_STRING_മൂല്യം(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vim_മൂല്യം->v_type == VAR_PARTIAL)
    {
	എങ്കിൽ (vim_മൂല്യം->vval.v_partial == ശൂന്യ)
	    result = scheme_null;
	അല്ലെങ്കിൽ
	{
	    Scheme_Object *funcname = ശൂന്യ;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (അക്ഷരം *)partial_name(vim_മൂല്യം->vval.v_partial));
	    MZ_GC_CHECK();
	    result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (സ്ഥിരം അക്ഷരം *)BYTE_STRING_മൂല്യം(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vim_മൂല്യം->v_type == VAR_BOOL || vim_മൂല്യം->v_type == VAR_SPECIAL)
    {
	എങ്കിൽ (vim_മൂല്യം->vval.v_സംഖ്യ <= VVAL_TRUE)
	    result = scheme_make_integer((long)vim_മൂല്യം->vval.v_സംഖ്യ);
	അല്ലെങ്കിൽ
	    result = scheme_null;
	MZ_GC_CHECK();
    }
    അല്ലെങ്കിൽ
    {
	result = scheme_void;
	new_മൂല്യം_2 = FALSE;
    }
    എങ്കിൽ (new_മൂല്യം_2)
    {
	scheme_hash_set(visited, (Scheme_Object *)vim_മൂല്യം, result);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    തിരികെ result;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
mzscheme_to_vim(Scheme_Object *obj, typval_T *tv)
{
    പൂർണ്ണസംഖ്യ i, status;
    Scheme_Hash_Table	*visited = ശൂന്യ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    status = mzscheme_to_vim_impl(obj, tv, 1, visited);
    വേണ്ടി (i = 0; i < visited->size; ++i)
    {
	// free up remembered objects
	എങ്കിൽ (visited->vals[i] != ശൂന്യ)
	    free_tv((typval_T *)visited->vals[i]);
    }

    MZ_GC_UNREG();
    തിരികെ status;
}
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, പൂർണ്ണസംഖ്യ depth,
	Scheme_Hash_Table *visited)
{
    പൂർണ്ണസംഖ്യ		status = OK;
    typval_T	*found;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    MZ_GC_CHECK();
    എങ്കിൽ (depth > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_സംഖ്യ;
	tv->vval.v_സംഖ്യ = 0;
	തിരികെ FAIL;
    }

    found = (typval_T *)scheme_hash_get(visited, obj);
    എങ്കിൽ (found != ശൂന്യ)
	copy_tv(found, tv);
    അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_സംഖ്യ = VVAL_NULL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_സംഖ്യ;
	tv->vval.v_സംഖ്യ = SCHEME_INT_VAL(obj);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_സംഖ്യ = SCHEME_TRUEP(obj);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_മൂല്യം(obj));
    }
# എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 299
    അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = ശൂന്യ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_to_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_മൂല്യം(tmp));
	MZ_GC_UNREG();
    }
# endif
    അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	എങ്കിൽ (list == ശൂന്യ)
	    status = FAIL;
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ		    i;
	    Scheme_Object   *curr = ശൂന്യ;
	    Scheme_Object   *cval = ശൂന്യ;
	    // temporary var to hold current element of vectors and pairs
	    typval_T	    *v;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_IN_REG(0, curr);
	    MZ_GC_VAR_IN_REG(1, cval);
	    MZ_GC_REG();

	    tv->v_type = VAR_LIST;
	    tv->vval.v_list = list;
	    ++list->lv_refcount;

	    v = ALLOC_ONE(typval_T);
	    എങ്കിൽ (v == ശൂന്യ)
		status = FAIL;
	    അല്ലെങ്കിൽ
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *visited_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, visited_tv);
		scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

		എങ്കിൽ (SCHEME_VECTORP(obj))
		{
		    വേണ്ടി (i = 0; i < SCHEME_VEC_SIZE(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			എങ്കിൽ (status == FAIL)
			    നിർത്തുക;
			status = list_append_tv(list, v);
			clear_tv(v);
			എങ്കിൽ (status == FAIL)
			    നിർത്തുക;
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    വേണ്ടി (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			എങ്കിൽ (status == FAIL)
			    നിർത്തുക;
			status = list_append_tv(list, v);
			clear_tv(v);
			എങ്കിൽ (status == FAIL)
			    നിർത്തുക;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    എങ്കിൽ (status == OK && !SCHEME_NULLP(curr))
		    {
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			എങ്കിൽ (status == OK)
			{
			    status = list_append_tv(list, v);
			    clear_tv(v);
			}
		    }
		}
		// nothing to do for scheme_null
		vim_free(v);
	    }
	    MZ_GC_UNREG();
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (SCHEME_HASHTP(obj))
    {
	പൂർണ്ണസംഖ്യ		i;
	dict_T		*dict;
	Scheme_Object   *താക്കോൽ = ശൂന്യ;
	Scheme_Object   *val = ശൂന്യ;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, താക്കോൽ);
	MZ_GC_VAR_IN_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	എങ്കിൽ (dict == ശൂന്യ)
	    status = FAIL;
	അല്ലെങ്കിൽ
	{
	    typval_T    *visited_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, visited_tv);
	    scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

	    വേണ്ടി (i = 0; i < ((Scheme_Hash_Table *)obj)->size; ++i)
	    {
		എങ്കിൽ (((Scheme_Hash_Table *) obj)->vals[i] != ശൂന്യ)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *item = dictitem_alloc((char_u *)string_to_വരി(
				((Scheme_Hash_Table *) obj)->താക്കോലുകൾ[i]));
		    // convert Scheme val to Vim and add it to the dict
		    എങ്കിൽ (mzscheme_to_vim_impl(((Scheme_Hash_Table *) obj)->vals[i],
				    &item->di_tv, depth + 1, visited) == FAIL
			    || dict_add(dict, item) == FAIL)
		    {
			dictitem_free(item);
			status = FAIL;
			നിർത്തുക;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    അല്ലെങ്കിൽ
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_to_വരി(obj);
    }
    MZ_GC_UNREG();
    തിരികെ status;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    സ്ഥിതം Scheme_Object *
vim_funcref(ശൂന്യം *name, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    പൂർണ്ണസംഖ്യ i;
    typval_T args;
    പൂർണ്ണസംഖ്യ status = OK;
    Scheme_Object *result = ശൂന്യ;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_REG();

    result = scheme_void;
    എങ്കിൽ (list == ശൂന്യ)
	status = FAIL;
    അല്ലെങ്കിൽ
    {
	args.v_type = VAR_LIST;
	args.vval.v_list = list;
	++list->lv_refcount;
	വേണ്ടി (i = 0; status == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    എങ്കിൽ (v == ശൂന്യ)
		status = FAIL;
	    അല്ലെങ്കിൽ
	    {
		status = mzscheme_to_vim(argv[i], v);
		എങ്കിൽ (status == OK)
		{
		    status = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	എങ്കിൽ (status == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_മൂല്യം((Scheme_Object *)name), &args, &ret);
	    MZ_GC_CHECK();
	    result = vim_to_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&args);
    MZ_GC_UNREG();
    എങ്കിൽ (status != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    അല്ലെങ്കിൽ
	raise_if_error();
    തിരികെ result;
}

    ശൂന്യം
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = ശൂന്യ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, ret);
    MZ_GC_REG();

    എങ്കിൽ (mzscheme_init())
    {
	MZ_GC_UNREG();
	തിരികെ;
    }

    MZ_GC_CHECK();
    എങ്കിൽ (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_to_vim(ret, rettv);

    MZ_GC_UNREG();
}
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vim_error_check(ശൂന്യം)
{
    തിരികെ (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    സ്ഥിതം ശൂന്യം
register_vim_exn(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	nc = 0;
    പൂർണ്ണസംഖ്യ i;
    Scheme_Object   *struct_exn = ശൂന്യ;
    Scheme_Object   *exn_name = ശൂന്യ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, struct_exn);
    MZ_GC_VAR_IN_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_ചിഹ്നം("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_മൂല്യം("struct:exn");
    MZ_GC_CHECK();

    എങ്കിൽ (vim_exn == ശൂന്യ)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, ശൂന്യ, 0, 0, ശൂന്യ, ശൂന്യ
#എങ്കിൽ MZSCHEME_VERSION_MAJOR >= 299
		, ശൂന്യ
#endif
		);


    {
	Scheme_Object   **tmp = ശൂന്യ;
	Scheme_Object   *exn_names[5] = {ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ};
	Scheme_Object   *exn_മൂല്യങ്ങൾ[5] = {ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_IN_REG(0, exn_names, 5);
	MZ_GC_ARRAY_VAR_IN_REG(3, exn_മൂല്യങ്ങൾ, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_names(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_names, tmp, nc * വലുപ്പം(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_മൂല്യങ്ങൾ(vim_exn, exn_names, nc, 0);
	mch_memmove(exn_മൂല്യങ്ങൾ, tmp, nc * വലുപ്പം(Scheme_Object *));
	MZ_GC_CHECK();

	വേണ്ടി (i = 0; i < nc; i++)
	{
	    scheme_add_global_ചിഹ്നം(exn_names[i],
		    exn_മൂല്യങ്ങൾ[i], environment);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
    MZ_GC_UNREG();
}

/*
 * raise exn:vim, may be with additional info string
 */
    ശൂന്യം
raise_vim_exn(സ്ഥിരം അക്ഷരം *add_info)
{
    അക്ഷരം	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {ശൂന്യ, ശൂന്യ};
    Scheme_Object   *exn = ശൂന്യ;
    Scheme_Object   *byte_string = ശൂന്യ;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_IN_REG(0, argv, 2);
    MZ_GC_VAR_IN_REG(3, exn);
    MZ_GC_VAR_IN_REG(4, byte_string);
    MZ_GC_REG();

    എങ്കിൽ (add_info != ശൂന്യ)
    {
	അക്ഷരം		*c_string = ശൂന്യ;
	Scheme_Object   *info = ശൂന്യ;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_IN_REG(0, c_string);
	MZ_GC_VAR_IN_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(add_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (പൂർണ്ണസംഖ്യ)STRLEN(fmt), 1, &info, ശൂന്യ);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	SCHEME_SET_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    അല്ലെങ്കിൽ
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#എങ്കിൽ MZSCHEME_VERSION_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#അല്ലെങ്കിൽ
    argv[1] = scheme_current_continuation_marks(ശൂന്യ);
    MZ_GC_CHECK();
#endif

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    ശൂന്യം
raise_if_error(ശൂന്യം)
{
    എങ്കിൽ (vim_error_check())
	raise_vim_exn(ശൂന്യ);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    സ്ഥിതം vim_mz_buffer *
get_buffer_arg(സ്ഥിരം അക്ഷരം *fname, പൂർണ്ണസംഖ്യ argnum, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    vim_mz_buffer *b;

    എങ്കിൽ (argc < argnum + 1)
	തിരികെ get_vim_curr_buffer();
    എങ്കിൽ (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_buffer *)argv[argnum];
    (ശൂന്യം)get_valid_buffer(argv[argnum]);
    തിരികെ b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    സ്ഥിതം vim_mz_window *
get_window_arg(സ്ഥിരം അക്ഷരം *fname, പൂർണ്ണസംഖ്യ argnum, പൂർണ്ണസംഖ്യ argc, Scheme_Object **argv)
{
    vim_mz_window *w;

    എങ്കിൽ (argc < argnum + 1)
	തിരികെ get_vim_curr_window();
    w = (vim_mz_window *)argv[argnum];
    എങ്കിൽ (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (ശൂന്യം)get_valid_window(argv[argnum]);
    തിരികെ w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_buffer(ശൂന്യം *obj)
{
    buf_T *buf = ((vim_mz_buffer *)obj)->buf;

    എങ്കിൽ (buf == INVALID_BUFFER_മൂല്യം)
	scheme_signal_error(_("buffer is invalid"));
    തിരികെ buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_window(ശൂന്യം *obj)
{
    win_T *win = ((vim_mz_window *)obj)->win;
    എങ്കിൽ (win == INVALID_WINDOW_മൂല്യം)
	scheme_signal_error(_("window is invalid"));
    തിരികെ win;
}

    പൂർണ്ണസംഖ്യ
mzthreads_allowed(ശൂന്യം)
{
    തിരികെ mz_threads_allow;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
വരി_in_range(linenr_T lnum, buf_T *buf)
{
    തിരികെ (lnum > 0 && lnum <= buf->b_ml.ml_വരി_എണ്ണം);
}

    സ്ഥിതം ശൂന്യം
check_വരി_range(linenr_T lnum, buf_T *buf)
{
    എങ്കിൽ (!വരി_in_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    സ്ഥിതം ശൂന്യം
mz_fix_cursor(പൂർണ്ണസംഖ്യ lo, പൂർണ്ണസംഖ്യ hi, പൂർണ്ണസംഖ്യ extra)
{
    എങ്കിൽ (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	എങ്കിൽ (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	അല്ലെങ്കിൽ
	    check_cursor_col();
	changed_cline_bef_curs();
    }
    invalidate_botline();
}

സ്ഥിതം Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_buffer_വരി, "get-buff-line", 1, 2},
    {set_buffer_വരി, "set-buff-line", 2, 3},
    {get_buffer_വരി_list, "get-buff-line-list", 2, 3},
    {get_buffer_name, "get-buff-name", 0, 1},
    {get_buffer_അക്കം, "get-buff-num", 0, 1},
    {get_buffer_size, "get-buff-size", 0, 1},
    {set_buffer_വരി_list, "set-buff-line-list", 3, 4},
    {തിരുകുക_buffer_വരി_list, "insert-buff-line-list", 2, 3},
    {get_curr_buffer, "curr-buff", 0, 0},
    {get_buffer_എണ്ണം, "buff-count", 0, 0},
    {get_next_buffer, "get-next-buff", 0, 1},
    {get_prev_buffer, "get-prev-buff", 0, 1},
    {mzscheme_open_buffer, "open-buff", 1, 1},
    {get_buffer_by_name, "get-buff-by-name", 1, 1},
    {get_buffer_by_അക്കം, "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_window_എണ്ണം, "win-count", 0, 0},
    {get_window_by_അക്കം, "get-win-by-num", 1, 1},
    {get_window_അക്കം, "get-win-num", 0, 1},
    {get_window_buffer, "get-win-buffer", 0, 1},
    {get_window_height, "get-win-height", 0, 1},
    {set_window_height, "set-win-height", 1, 2},
    {get_window_width, "get-win-width", 0, 1},
    {set_window_width, "set-win-width", 1, 2},
    {get_cursor, "get-cursor", 0, 1},
    {set_cursor, "set-cursor", 1, 2},
    {get_window_list, "get-win-list", 0, 1},
    /*
     * Vim-related commands
     */
    {vim_command, "command", 1, 1},
    {vim_eval, "eval", 1, 1},
    {get_range_start, "range-start", 0, 0},
    {get_range_end, "range-end", 0, 0},
    {mzscheme_beep, "beep", 0, 0},
    {get_option, "get-option", 1, 2},
    {set_option, "set-option", 1, 2},
    /*
     * small utilities
     */
    {vim_bufferp, "buff?", 1, 1},
    {vim_windowp, "win?", 1, 1},
    {vim_buffer_validp, "buff-valid?", 1, 1},
    {vim_window_validp, "win-valid?", 1, 1}
};

/*
 * return MzScheme wrapper for curbuf
 */
    സ്ഥിതം vim_mz_buffer *
get_vim_curr_buffer(ശൂന്യം)
{
    എങ്കിൽ (curbuf->b_mzscheme_ref == ശൂന്യ)
	തിരികെ (vim_mz_buffer *)buffer_new(curbuf);
    അല്ലെങ്കിൽ
	തിരികെ BUFFER_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    സ്ഥിതം vim_mz_window *
get_vim_curr_window(ശൂന്യം)
{
    എങ്കിൽ (curwin->w_mzscheme_ref == ശൂന്യ)
	തിരികെ (vim_mz_window *)window_new(curwin);
    അല്ലെങ്കിൽ
	തിരികെ WINDOW_REF(curwin);
}

    സ്ഥിതം ശൂന്യം
make_modules(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		    i;
    Scheme_Env	    *mod = ശൂന്യ;
    Scheme_Object   *vimext_ചിഹ്നം = ശൂന്യ;
    Scheme_Object   *closed_prim = ശൂന്യ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, mod);
    MZ_GC_VAR_IN_REG(1, vimext_ചിഹ്നം);
    MZ_GC_VAR_IN_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_ചിഹ്നം = scheme_intern_ചിഹ്നം("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_ചിഹ്നം, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(prims); i++)
    {
	Vim_Prim *prim = prims + i;
	closed_prim = scheme_make_closed_prim_w_arity(prim->prim, prim, prim->name,
			    prim->mina, prim->maxa);
	scheme_add_global(prim->name, closed_prim, mod);
	MZ_GC_CHECK();
    }
    scheme_finish_primitive_module(mod);
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

#ifdef HAVE_SANDBOX
സ്ഥിതം Scheme_Object *M_write = ശൂന്യ;
സ്ഥിതം Scheme_Object *M_read = ശൂന്യ;
സ്ഥിതം Scheme_Object *M_execute = ശൂന്യ;
സ്ഥിതം Scheme_Object *M_delete = ശൂന്യ;

    സ്ഥിതം ശൂന്യം
sandbox_check(ശൂന്യം)
{
    എങ്കിൽ (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    സ്ഥിതം Scheme_Object *
sandbox_file_guard(പൂർണ്ണസംഖ്യ argc UNUSED, Scheme_Object **argv)
{
    എങ്കിൽ (!sandbox)
	തിരികെ scheme_void;

    Scheme_Object *requested_access = argv[2];

    എങ്കിൽ (M_write == ശൂന്യ)
    {
	MZ_REGISTER_STATIC(M_write);
	M_write = scheme_intern_ചിഹ്നം("write");
	MZ_GC_CHECK();
    }
    എങ്കിൽ (M_read == ശൂന്യ)
    {
	MZ_REGISTER_STATIC(M_read);
	M_read = scheme_intern_ചിഹ്നം("read");
	MZ_GC_CHECK();
    }
    എങ്കിൽ (M_execute == ശൂന്യ)
    {
	MZ_REGISTER_STATIC(M_execute);
	M_execute = scheme_intern_ചിഹ്നം("execute");
	MZ_GC_CHECK();
    }
    എങ്കിൽ (M_delete == ശൂന്യ)
    {
	MZ_REGISTER_STATIC(M_delete);
	M_delete = scheme_intern_ചിഹ്നം("delete");
	MZ_GC_CHECK();
    }

    ആകുമ്പോൾ (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *item = SCHEME_CAR(requested_access);
	എങ്കിൽ (scheme_eq(item, M_write) || scheme_eq(item, M_read)
		|| scheme_eq(item, M_execute) || scheme_eq(item, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    തിരികെ scheme_void;
}

    സ്ഥിതം Scheme_Object *
sandbox_network_guard(പൂർണ്ണസംഖ്യ argc UNUSED, Scheme_Object **argv UNUSED)
{
    തിരികെ scheme_void;
}
#endif

#endif
