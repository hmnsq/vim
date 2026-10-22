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

#అన్తర్భావ "vim.h"

#అన్తర్భావ "if_mzsch.h"

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#ఒకవేళ defined(FEAT_MZSCHEME)

/*
 * scheme_register_tls_space is only available on 32-bit Windows until
 * racket-6.3.  See
 * http://docs.racket-lang.org/inside/im_memoryalloc.html?q=scheme_register_tls_space
 */
#ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_సంస్కరణ_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#అయితే_ముగింపు

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#ఒకవేళ defined(MZ_PRECISE_GC) && MZSCHEME_సంస్కరణ_MAJOR >= 400 \
    || MZSCHEME_సంస్కరణ_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#అయితే_ముగింపు

// Base data structures
#నిర్వచించు SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_బఫరు_type)
#నిర్వచించు SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_కిటికీ_type)

రకనిర్వచనం నిర్మాణం
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_బఫరు;

#నిర్వచించు INVALID_బఫరు_విలువ ((buf_T *)(-1))

రకనిర్వచనం నిర్మాణం
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_కిటికీ;

#నిర్వచించు INVALID_కిటికీ_విలువ ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
రకనిర్వచనం నిర్మాణం
{
    Scheme_Closed_Prim	*prim;
    అక్షరం	*name;
    పూర్ణాంకం		mina;	// arity information
    పూర్ణాంకం		maxa;
} Vim_Prim;

రకనిర్వచనం నిర్మాణం
{
    అక్షరం	    *name;
    Scheme_Object   *పోర్టు;
} పోర్టు_Info;

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
#నిర్వచితమైతే HAVE_SANDBOX
స్థిర Scheme_Object *sandbox_file_guard(పూర్ణాంకం, Scheme_Object **);
స్థిర Scheme_Object *sandbox_జాలం_guard(పూర్ణాంకం, Scheme_Object **);
స్థిర శూన్య sandbox_check(శూన్య);
#అయితే_ముగింపు
//  Buffer-related commands
స్థిర Scheme_Object *బఫరు_new(buf_T *buf);
స్థిర Scheme_Object *get_బఫరు_by_అంకె(శూన్య *, పూర్ణాంకం, Scheme_Object **);
స్థిర vim_mz_బఫరు *get_vim_curr_బఫరు(శూన్య);

//  Window-related commands
స్థిర Scheme_Object *కిటికీ_new(win_T *win);
స్థిర vim_mz_కిటికీ *get_vim_curr_కిటికీ(శూన్య);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
స్థిర పూర్ణాంకం vim_error_check(శూన్య);
స్థిర పూర్ణాంకం do_mzscheme_command(exarg_T *, శూన్య *, Scheme_Closed_Prim *what);
స్థిర పూర్ణాంకం startup_mzscheme(శూన్య);
స్థిర అక్షరం *string_గమ్యం_పంక్తి(Scheme_Object *obj);
#ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#లేకపోతే
# define OUTPUT_LEN_TYPE దీర్ఘ
#అయితే_ముగింపు
స్థిర శూన్య do_output(అక్షరం *mesg, OUTPUT_LEN_TYPE len);
స్థిర శూన్య do_printf(అక్షరం *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
స్థిర శూన్య do_flush(శూన్య);
స్థిర Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
స్థిర Scheme_Object *వెలికితీయి_exn_సందేశం(Scheme_Object *v);
స్థిర Scheme_Object *do_eval(శూన్య *, పూర్ణాంకం noargc, Scheme_Object **noargv);
స్థిర Scheme_Object *do_ఎక్కించు(శూన్య *, పూర్ణాంకం noargc, Scheme_Object **noargv);
స్థిర శూన్య రిజిస్టరు_vim_exn(శూన్య);
స్థిర vim_mz_బఫరు *get_బఫరు_arg(స్థిరరాశి అక్షరం *fname, పూర్ణాంకం argnum,
	పూర్ణాంకం argc, Scheme_Object **argv);
స్థిర vim_mz_కిటికీ *get_కిటికీ_arg(స్థిరరాశి అక్షరం *fname, పూర్ణాంకం argnum,
	పూర్ణాంకం argc, Scheme_Object **argv);
స్థిర పూర్ణాంకం పంక్తి_లోపల_range(linenr_T, buf_T *);
స్థిర శూన్య check_పంక్తి_range(linenr_T, buf_T *);
స్థిర శూన్య mz_fix_cursor(పూర్ణాంకం lo, పూర్ణాంకం hi, పూర్ణాంకం extra);

స్థిర పూర్ణాంకం eval_with_exn_handling(శూన్య *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
స్థిర శూన్య make_modules(శూన్య);
స్థిర శూన్య init_exn_catching_apply(శూన్య);
స్థిర పూర్ణాంకం mzscheme_env_main(Scheme_Env *env, పూర్ణాంకం argc, అక్షరం **argv);
స్థిర పూర్ణాంకం mzscheme_init(శూన్య);
#నిర్వచితమైతే FEAT_EVAL
స్థిర Scheme_Object *vim_గమ్యం_mzscheme(typval_T *vim_విలువ);
స్థిర Scheme_Object *vim_గమ్యం_mzscheme_impl(typval_T *vim_విలువ, పూర్ణాంకం లోతు,
	Scheme_Hash_పట్టిక *సందర్శించిన);
స్థిర పూర్ణాంకం mzscheme_గమ్యం_vim(Scheme_Object *obj, typval_T *tv);
స్థిర పూర్ణాంకం mzscheme_గమ్యం_vim_impl(Scheme_Object *obj, typval_T *tv, పూర్ణాంకం లోతు,
	Scheme_Hash_పట్టిక *సందర్శించిన);
స్థిర Scheme_Object *vim_funcref(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv);
#అయితే_ముగింపు

#నిర్వచితమైతే MZ_PRECISE_GC
స్థిర పూర్ణాంకం బఫరు_కొలత_proc(శూన్య *obj వాడని)
{
    తిరిగి gc_బైట్లు_గమ్యం_పదాలు(పరిమాణం(vim_mz_బఫరు));
}
స్థిర పూర్ణాంకం బఫరు_mark_proc(శూన్య *obj)
{
    తిరిగి బఫరు_కొలత_proc(obj);
}
స్థిర పూర్ణాంకం బఫరు_fixup_proc(శూన్య *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    తిరిగి బఫరు_కొలత_proc(obj);
}
స్థిర పూర్ణాంకం కిటికీ_కొలత_proc(శూన్య *obj వాడని)
{
    తిరిగి gc_బైట్లు_గమ్యం_పదాలు(పరిమాణం(vim_mz_కిటికీ));
}
స్థిర పూర్ణాంకం కిటికీ_mark_proc(శూన్య *obj)
{
    తిరిగి కిటికీ_కొలత_proc(obj);
}
స్థిర పూర్ణాంకం కిటికీ_fixup_proc(శూన్య *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    తిరిగి కిటికీ_కొలత_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define కిటికీ_REF(win) *(vim_mz_కిటికీ **)((win)->w_mzscheme_ref)
# define బఫరు_REF(buf) *(vim_mz_బఫరు **)((buf)->b_mzscheme_ref)
#లేకపోతే
# define కిటికీ_REF(win) (vim_mz_కిటికీ *)((win)->w_mzscheme_ref)
# define బఫరు_REF(buf) (vim_mz_బఫరు *)((buf)->b_mzscheme_ref)
#అయితే_ముగింపు

#ఒకవేళ defined(DYNAMIC_MZSCHEME)
స్థిర Scheme_Object *dll_scheme_eof;
స్థిర Scheme_Object *dll_scheme_false;
స్థిర Scheme_Object *dll_scheme_void;
స్థిర Scheme_Object *dll_scheme_null;
స్థిర Scheme_Object *dll_scheme_true;

స్థిర Scheme_Thread **dll_scheme_current_thread_ptr;

స్థిర శూన్య (**dll_scheme_console_printf_ptr)(అక్షరం *str, ...);
స్థిర శూన్య (**dll_scheme_console_output_ptr)(అక్షరం *str, OUTPUT_LEN_TYPE len);
స్థిర శూన్య (**dll_scheme_notify_multithread_ptr)(పూర్ణాంకం on);

స్థిర శూన్య *(*dll_GC_malloc)(size_t కొలత_లోపల_బైట్లు);
స్థిర శూన్య *(*dll_GC_malloc_atomic)(size_t కొలత_లోపల_బైట్లు);
స్థిర Scheme_Env *(*dll_scheme_basic_env)(శూన్య);
స్థిర శూన్య (*dll_scheme_check_దారాలు)(శూన్య);
స్థిర శూన్య (*dll_scheme_రిజిస్టరు_static)(శూన్య *ptr, దీర్ఘ కొలత);
స్థిర శూన్య (*dll_scheme_సమితి_దొంతర_పునాది)(శూన్య *పునాది, పూర్ణాంకం no_auto_statics);
స్థిర శూన్య (*dll_scheme_కూడు_global)(స్థిరరాశి అక్షరం *name, Scheme_Object *val,
	Scheme_Env *env);
స్థిర శూన్య (*dll_scheme_కూడు_global_చిహ్నం)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
స్థిర Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, పూర్ణాంకం అంకె_rands,
	Scheme_Object **rands);
స్థిర Scheme_Object *(*dll_scheme_builtin_విలువ)(స్థిరరాశి అక్షరం *name);
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 299
స్థిర Scheme_Object *(*dll_scheme_byte_string_గమ్యం_char_string)(Scheme_Object *s);
స్థిర Scheme_Object *(*dll_scheme_make_మార్గం)(స్థిరరాశి అక్షరం *chars);
# endif
స్థిర శూన్య (*dll_scheme_close_ఆదానం_పోర్టు)(Scheme_Object *పోర్టు);
స్థిర శూన్య (*dll_scheme_లెక్క_పంక్తులు)(Scheme_Object *పోర్టు);
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR < 360
స్థిర Scheme_Object *(*dll_scheme_current_continuation_marks)(శూన్య);
# లేకపోతే
స్థిర Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_ట్యాగ్);
# endif
స్థిర శూన్య (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *పోర్టు);
స్థిర అక్షరం *(*dll_scheme_display_గమ్యం_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
స్థిర పూర్ణాంకం (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
స్థిర Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	పూర్ణాంకం అంకె_rands_2, Scheme_Object **rands, పూర్ణాంకం val);
స్థిర శూన్య (*dll_scheme_dont_gc_ptr)(శూన్య *p);
స్థిర Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
స్థిర Scheme_Object *(*dll_scheme_eval_string)(స్థిరరాశి అక్షరం *str,
	Scheme_Env *env);
స్థిర Scheme_Object *(*dll_scheme_eval_string_all)(స్థిరరాశి అక్షరం *str,
	Scheme_Env *env, పూర్ణాంకం all);
స్థిర శూన్య (*dll_scheme_ముగింపు_primitive_module)(Scheme_Env *env);
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR < 299
స్థిర అక్షరం *(*dll_scheme_format)(అక్షరం *format, పూర్ణాంకం flen, పూర్ణాంకం argc,
	Scheme_Object **argv, దీర్ఘ *rlen);
# లేకపోతే
స్థిర అక్షరం *(*dll_scheme_format_utf8)(అక్షరం *format, పూర్ణాంకం flen, పూర్ణాంకం argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
స్థిర Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, పూర్ణాంకం pos);
# endif
స్థిర శూన్య (*dll_scheme_gc_ptr_ok)(శూన్య *p);
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR < 299
స్థిర అక్షరం *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	దీర్ఘ *len);
# లేకపోతే
స్థిర అక్షరం *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
స్థిర Scheme_Object *(*dll_scheme_intern_చిహ్నం)(స్థిరరాశి అక్షరం *name);
స్థిర Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *చిహ్నం,
	Scheme_Env *env);
స్థిర Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, శూన్య *దత్తాంశం, స్థిరరాశి అక్షరం *name, mzshort mina,
     mzshort maxa);
స్థిర Scheme_Object *(*dll_scheme_make_integer_విలువ)(దీర్ఘ i);
స్థిర Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
స్థిర Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	స్థిరరాశి అక్షరం *name, mzshort mina, mzshort maxa);
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR < 299
స్థిర Scheme_Object *(*dll_scheme_make_string)(స్థిరరాశి అక్షరం *chars);
స్థిర Scheme_Object *(*dll_scheme_make_string_output_పోర్టు)();
# లేకపోతే
స్థిర Scheme_Object *(*dll_scheme_make_byte_string)(స్థిరరాశి అక్షరం *chars);
స్థిర Scheme_Object *(*dll_scheme_make_byte_string_output_పోర్టు)();
# endif
స్థిర Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	పూర్ణాంకం argc, Scheme_Object **argv);
స్థిర Scheme_Object **(*dll_scheme_make_struct_పేర్లు)(Scheme_Object *పునాది,
	Scheme_Object *క్షేత్రం_పేర్లు, పూర్ణాంకం flags, పూర్ణాంకం *లెక్క_బహిర్గతం);
స్థిర Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *పునాది,
	Scheme_Object *మాతృ, Scheme_Object *inspector, పూర్ణాంకం అంకె_fields,
	పూర్ణాంకం అంకె_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
స్థిర Scheme_Object **(*dll_scheme_make_struct_విలువలు)(
	Scheme_Object *struct_type, Scheme_Object **పేర్లు, పూర్ణాంకం లెక్క,
	పూర్ణాంకం flags);
స్థిర Scheme_Type (*dll_scheme_make_type)(స్థిరరాశి అక్షరం *name);
స్థిర Scheme_Object *(*dll_scheme_make_సదిశ)(పూర్ణాంకం కొలత,
	Scheme_Object *fill);
స్థిర శూన్య *(*dll_scheme_malloc_fail_ok)(శూన్య *(*f)(size_t), size_t);
స్థిర Scheme_Object *(*dll_scheme_open_ఆదానం_file)(స్థిరరాశి అక్షరం *name,
	స్థిరరాశి అక్షరం *who);
స్థిర Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
స్థిర పూర్ణాంకం (*dll_scheme_proper_list_నిడివి)(Scheme_Object *list);
స్థిర శూన్య (*dll_scheme_raise)(Scheme_Object *exn);
స్థిర Scheme_Object *(*dll_scheme_read)(Scheme_Object *పోర్టు);
స్థిర శూన్య (*dll_scheme_signal_error)(స్థిరరాశి అక్షరం *సందేశం, ...);
స్థిర శూన్య (*dll_scheme_wrong_type)(స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *expected,
	పూర్ణాంకం which, పూర్ణాంకం argc, Scheme_Object **argv);
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 299
స్థిర శూన్య (*dll_scheme_సమితి_param)(Scheme_Config *c, పూర్ణాంకం pos,
	Scheme_Object *o);
స్థిర Scheme_Config *(*dll_scheme_current_config)(శూన్య);
స్థిర Scheme_Object *(*dll_scheme_char_string_గమ్యం_byte_string)
    (Scheme_Object *s);
స్థిర Scheme_Object *(*dll_scheme_char_string_గమ్యం_మార్గం)
    (Scheme_Object *s);
స్థిర శూన్య *(*dll_scheme_సమితి_collects_మార్గం)(Scheme_Object *p);
# endif
స్థిర Scheme_Hash_పట్టిక *(*dll_scheme_make_hash_పట్టిక)(పూర్ణాంకం type);
స్థిర శూన్య (*dll_scheme_hash_సమితి)(Scheme_Hash_పట్టిక *పట్టిక,
	Scheme_Object *కీలకం, Scheme_Object *విలువ);
స్థిర Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_పట్టిక *పట్టిక,
	Scheme_Object *కీలకం);
స్థిర Scheme_Object *(*dll_scheme_make_double)(ద్విగుణ d);
స్థిర Scheme_Object *(*dll_scheme_make_sized_byte_string)(అక్షరం *chars,
	దీర్ఘ len, పూర్ణాంకం copy);
స్థిర Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
స్థిర Scheme_Object *(*dll_scheme_dynamic_wind)(శూన్య (*pre)(శూన్య *), Scheme_Object *(* అస్థిర act)(శూన్య *), శూన్య (* అస్థిర post)(శూన్య *), Scheme_Object *(*jmp_handler)(శూన్య *), శూన్య * అస్థిర దత్తాంశం);
# ifdef MZ_PRECISE_GC
స్థిర శూన్య *(*dll_GC_malloc_one_tagged)(size_t కొలత_లోపల_బైట్లు);
స్థిర శూన్య (*dll_GC_రిజిస్టరు_traversers)(పొట్టి ట్యాగ్, కొలత_Proc కొలత, Mark_Proc mark, Fixup_Proc fixup, పూర్ణాంకం is_constant_కొలత, పూర్ణాంకం is_atomic);
# endif
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 400
స్థిర శూన్య (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
స్థిర శూన్య **(*dll_scheme_malloc_immobile_box)(శూన్య *p);
స్థిర శూన్య (*dll_scheme_free_immobile_box)(శూన్య **b);
# endif
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
స్థిర పూర్ణాంకం (*dll_scheme_main_అమరిక)(పూర్ణాంకం no_auto_statics, Scheme_Env_Main _main, పూర్ణాంకం argc, అక్షరం **argv);
#   ఒకవేళ defined(IMPLEMENT_THREAD_LOCAL_ద్వారా_WIN_TLS) || MZSCHEME_సంస్కరణ_MAJOR >= 603
స్థిర శూన్య (*dll_scheme_రిజిస్టరు_tls_space)(శూన్య *tls_space, పూర్ణాంకం tls_సూచీ);
#   endif
#  endif
#  ఒకవేళ defined(IMPLEMENT_THREAD_LOCAL_ద్వారా_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_ద్వారా_PROC)
స్థిర Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(శూన్య);
#  endif
# endif
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 600
స్థిర శూన్య (*dll_scheme_embedded_ఎక్కించు)(intptr_t len, స్థిరరాశి అక్షరం *s, పూర్ణాంకం predefined);
స్థిర శూన్య (*dll_scheme_రిజిస్టరు_embedded_ఎక్కించు)(intptr_t len, స్థిరరాశి అక్షరం *s);
స్థిర శూన్య (*dll_scheme_సమితి_config_మార్గం)(Scheme_Object *p);
# endif

# ఒకవేళ defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  ఒకవేళ !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_పట్టిక)
#   define scheme_current_thread (*dll_scheme_current_thread_ptr)
#  endif
#  define scheme_console_printf (*dll_scheme_console_printf_ptr)
#  define scheme_console_output (*dll_scheme_console_output_ptr)
#  define scheme_notify_multithread (*dll_scheme_notify_multithread_ptr)

// and functions in a usual way
#  define GC_malloc dll_GC_malloc
#  define GC_malloc_atomic dll_GC_malloc_atomic

#  define scheme_కూడు_global dll_scheme_కూడు_global
#  define scheme_కూడు_global_చిహ్నం dll_scheme_కూడు_global_చిహ్నం
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_విలువ dll_scheme_builtin_విలువ
#  ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 299
#   define scheme_byte_string_గమ్యం_char_string dll_scheme_byte_string_గమ్యం_char_string
#   define scheme_make_మార్గం dll_scheme_make_మార్గం
#  endif
#  define scheme_check_దారాలు dll_scheme_check_దారాలు
#  define scheme_close_ఆదానం_పోర్టు dll_scheme_close_ఆదానం_పోర్టు
#  define scheme_లెక్క_పంక్తులు dll_scheme_లెక్క_పంక్తులు
#  define scheme_current_continuation_marks \
    dll_scheme_current_continuation_marks
#  define scheme_display dll_scheme_display
#  define scheme_display_గమ్యం_string dll_scheme_display_గమ్యం_string
#  define scheme_do_eval dll_scheme_do_eval
#  define scheme_dont_gc_ptr dll_scheme_dont_gc_ptr
#  define scheme_eq dll_scheme_eq
#  define scheme_eval dll_scheme_eval
#  define scheme_eval_string dll_scheme_eval_string
#  define scheme_eval_string_all dll_scheme_eval_string_all
#  define scheme_ముగింపు_primitive_module dll_scheme_ముగింపు_primitive_module
#  ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR < 299
#   define scheme_format dll_scheme_format
#  లేకపోతే
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  లేకపోతే
#   define scheme_get_sized_byte_string_output \
    dll_scheme_get_sized_byte_string_output
#   define scheme_get_param dll_scheme_get_param
#  endif
#  define scheme_intern_చిహ్నం dll_scheme_intern_చిహ్నం
#  define scheme_lookup_global dll_scheme_lookup_global
#  define scheme_make_closed_prim_w_arity dll_scheme_make_closed_prim_w_arity
#  define scheme_make_integer_విలువ dll_scheme_make_integer_విలువ
#  define scheme_make_pair dll_scheme_make_pair
#  define scheme_make_prim_w_arity dll_scheme_make_prim_w_arity
#  ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_పోర్టు dll_scheme_make_string_output_పోర్టు
#  లేకపోతే
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_పోర్టు \
    dll_scheme_make_byte_string_output_పోర్టు
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_పేర్లు dll_scheme_make_struct_పేర్లు
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_విలువలు dll_scheme_make_struct_విలువలు
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_సదిశ dll_scheme_make_సదిశ
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_ఆదానం_file dll_scheme_open_ఆదానం_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_నిడివి dll_scheme_proper_list_నిడివి
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_రిజిస్టరు_static dll_scheme_రిజిస్టరు_static
#  define scheme_సమితి_దొంతర_పునాది dll_scheme_సమితి_దొంతర_పునాది
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 299
#   define scheme_సమితి_param dll_scheme_సమితి_param
#   define scheme_current_config dll_scheme_current_config
#   define scheme_char_string_గమ్యం_byte_string \
    dll_scheme_char_string_గమ్యం_byte_string
#   define scheme_char_string_గమ్యం_మార్గం \
    dll_scheme_char_string_గమ్యం_మార్గం
#   define scheme_సమితి_collects_మార్గం dll_scheme_సమితి_collects_మార్గం
#  endif
#  define scheme_make_hash_పట్టిక dll_scheme_make_hash_పట్టిక
#  define scheme_hash_సమితి dll_scheme_hash_సమితి
#  define scheme_hash_get dll_scheme_hash_get
#  define scheme_make_double dll_scheme_make_double
#  define scheme_make_sized_byte_string dll_scheme_make_sized_byte_string
#  define scheme_namespace_require dll_scheme_namespace_require
#  define scheme_dynamic_wind dll_scheme_dynamic_wind
#  ifdef MZ_PRECISE_GC
#   define GC_malloc_one_tagged dll_GC_malloc_one_tagged
#   define GC_రిజిస్టరు_traversers dll_GC_రిజిస్టరు_traversers
#  endif
#  ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_అమరిక dll_scheme_main_అమరిక
#    ఒకవేళ defined(IMPLEMENT_THREAD_LOCAL_ద్వారా_WIN_TLS) || MZSCHEME_సంస్కరణ_MAJOR >= 603
#     define scheme_రిజిస్టరు_tls_space dll_scheme_రిజిస్టరు_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 600
#   define scheme_embedded_ఎక్కించు dll_scheme_embedded_ఎక్కించు
#   define scheme_రిజిస్టరు_embedded_ఎక్కించు dll_scheme_రిజిస్టరు_embedded_ఎక్కించు
#   define scheme_సమితి_config_మార్గం dll_scheme_సమితి_config_మార్గం
#  endif

#  ifndef PROTO
#   ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 500
#    ఒకవేళ defined(IMPLEMENT_THREAD_LOCAL_ద్వారా_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_ద్వారా_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(శూన్య)
{
    తిరిగి dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

రకనిర్వచనం నిర్మాణం
{
    అక్షరం    *name;
    శూన్య    **ptr;
} Thunk_Info;

స్థిర Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (శూన్య **)&dll_GC_malloc},
    {"GC_malloc_atomic", (శూన్య **)&dll_GC_malloc_atomic},
    {ఖాళీ, ఖాళీ}};

స్థిర Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (శూన్య **)&dll_scheme_eof},
    {"scheme_false", (శూన్య **)&dll_scheme_false},
    {"scheme_void", (శూన్య **)&dll_scheme_void},
    {"scheme_null", (శూన్య **)&dll_scheme_null},
    {"scheme_true", (శూన్య **)&dll_scheme_true},
# ఒకవేళ !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_పట్టిక)
    {"scheme_current_thread", (శూన్య **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (శూన్య **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (శూన్య **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(శూన్య **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (శూన్య **)&dll_scheme_కూడు_global},
    {"scheme_add_global_symbol", (శూన్య **)&dll_scheme_కూడు_global_చిహ్నం},
    {"scheme_apply", (శూన్య **)&dll_scheme_apply},
    {"scheme_basic_env", (శూన్య **)&dll_scheme_basic_env},
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (శూన్య **)&dll_scheme_byte_string_గమ్యం_char_string},
    {"scheme_make_path", (శూన్య **)&dll_scheme_make_మార్గం},
# endif
    {"scheme_builtin_value", (శూన్య **)&dll_scheme_builtin_విలువ},
    {"scheme_check_threads", (శూన్య **)&dll_scheme_check_దారాలు},
    {"scheme_close_input_port", (శూన్య **)&dll_scheme_close_ఆదానం_పోర్టు},
    {"scheme_count_lines", (శూన్య **)&dll_scheme_లెక్క_పంక్తులు},
    {"scheme_current_continuation_marks",
	(శూన్య **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (శూన్య **)&dll_scheme_display},
    {"scheme_display_to_string", (శూన్య **)&dll_scheme_display_గమ్యం_string},
    {"scheme_do_eval", (శూన్య **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (శూన్య **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (శూన్య **)&dll_scheme_eq},
    {"scheme_eval", (శూన్య **)&dll_scheme_eval},
    {"scheme_eval_string", (శూన్య **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (శూన్య **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(శూన్య **)&dll_scheme_ముగింపు_primitive_module},
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR < 299
    {"scheme_format", (శూన్య **)&dll_scheme_format},
# లేకపోతే
    {"scheme_format_utf8", (శూన్య **)&dll_scheme_format_utf8},
    {"scheme_get_param", (శూన్య **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (శూన్య **)&dll_scheme_gc_ptr_ok},
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR < 299
    {"scheme_get_sized_string_output",
	(శూన్య **)&dll_scheme_get_sized_string_output},
# లేకపోతే
    {"scheme_get_sized_byte_string_output",
	(శూన్య **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (శూన్య **)&dll_scheme_intern_చిహ్నం},
    {"scheme_lookup_global", (శూన్య **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(శూన్య **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (శూన్య **)&dll_scheme_make_integer_విలువ},
    {"scheme_make_pair", (శూన్య **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (శూన్య **)&dll_scheme_make_prim_w_arity},
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR < 299
    {"scheme_make_string", (శూన్య **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(శూన్య **)&dll_scheme_make_string_output_పోర్టు},
# లేకపోతే
    {"scheme_make_byte_string", (శూన్య **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(శూన్య **)&dll_scheme_make_byte_string_output_పోర్టు},
# endif
    {"scheme_make_struct_instance",
	(శూన్య **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (శూన్య **)&dll_scheme_make_struct_పేర్లు},
    {"scheme_make_struct_type", (శూన్య **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (శూన్య **)&dll_scheme_make_struct_విలువలు},
    {"scheme_make_type", (శూన్య **)&dll_scheme_make_type},
    {"scheme_make_vector", (శూన్య **)&dll_scheme_make_సదిశ},
    {"scheme_malloc_fail_ok", (శూన్య **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (శూన్య **)&dll_scheme_open_ఆదానం_file},
    {"scheme_primitive_module", (శూన్య **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (శూన్య **)&dll_scheme_proper_list_నిడివి},
    {"scheme_raise", (శూన్య **)&dll_scheme_raise},
    {"scheme_read", (శూన్య **)&dll_scheme_read},
    {"scheme_register_static", (శూన్య **)&dll_scheme_రిజిస్టరు_static},
    {"scheme_set_stack_base", (శూన్య **)&dll_scheme_సమితి_దొంతర_పునాది},
    {"scheme_signal_error", (శూన్య **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (శూన్య **)&dll_scheme_wrong_type},
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 299
    {"scheme_set_param", (శూన్య **)&dll_scheme_సమితి_param},
    {"scheme_current_config", (శూన్య **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(శూన్య **)&dll_scheme_char_string_గమ్యం_byte_string},
    {"scheme_char_string_to_path", (శూన్య **)&dll_scheme_char_string_గమ్యం_మార్గం},
    {"scheme_set_collects_path", (శూన్య **)&dll_scheme_సమితి_collects_మార్గం},
# endif
    {"scheme_make_hash_table", (శూన్య **)&dll_scheme_make_hash_పట్టిక},
    {"scheme_hash_set", (శూన్య **)&dll_scheme_hash_సమితి},
    {"scheme_hash_get", (శూన్య **)&dll_scheme_hash_get},
    {"scheme_make_double", (శూన్య **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (శూన్య **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (శూన్య **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (శూన్య **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (శూన్య **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (శూన్య **)&dll_GC_రిజిస్టరు_traversers},
# endif
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (శూన్య **)&dll_scheme_main_అమరిక},
#   ఒకవేళ defined(IMPLEMENT_THREAD_LOCAL_ద్వారా_WIN_TLS) || MZSCHEME_సంస్కరణ_MAJOR >= 603
    {"scheme_register_tls_space", (శూన్య **)&dll_scheme_రిజిస్టరు_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (శూన్య **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (శూన్య **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (శూన్య **)&dll_scheme_free_immobile_box},
# endif
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 500
#  ఒకవేళ defined(IMPLEMENT_THREAD_LOCAL_ద్వారా_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_ద్వారా_PROC)
    {"scheme_external_get_thread_local_variables", (శూన్య **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 600
    {"scheme_embedded_load", (శూన్య **)&dll_scheme_embedded_ఎక్కించు},
    {"scheme_register_embedded_load", (శూన్య **)&dll_scheme_రిజిస్టరు_embedded_ఎక్కించు},
    {"scheme_set_config_path", (శూన్య **)&dll_scheme_సమితి_config_మార్గం},
# endif
    {ఖాళీ, ఖాళీ}};

స్థిర HINSTANCE hMzGC = 0;
స్థిర HINSTANCE hMzSch = 0;

స్థిర శూన్య dynamic_mzscheme_చివర(శూన్య);
స్థిర పూర్ణాంకం mzscheme_runtime_link_init(అక్షరం *sch_dll, అక్షరం *gc_dll,
	పూర్ణాంకం verbose);

    స్థిర పూర్ణాంకం
mzscheme_runtime_link_init(అక్షరం *sch_dll, అక్షరం *gc_dll, పూర్ణాంకం verbose)
{
    Thunk_Info *thunk = ఖాళీ;

    ఒకవేళ (hMzGC && hMzSch)
	తిరిగి OK;
    hMzSch = vim_ఎక్కించు_Lib(sch_dll);
    hMzGC = vim_ఎక్కించు_Lib(gc_dll);

    ఒకవేళ (!hMzGC)
    {
	ఒకవేళ (verbose)
	    semsg(_(e_could_not_ఎక్కించు_library_str_str), gc_dll, GetWin32Error());
	తిరిగి FAIL;
    }

    ఒకవేళ (!hMzSch)
    {
	ఒకవేళ (verbose)
	    semsg(_(e_could_not_ఎక్కించు_library_str_str), sch_dll, GetWin32Error());
	తిరిగి FAIL;
    }

    కొరకు (thunk = mzsch_imports; thunk->name; thunk++)
    {
	ఒకవేళ ((*thunk->ptr =
		    (శూన్య *)Get_Proc_చిరునామా(hMzSch, thunk->name)) == ఖాళీ)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    ఒకవేళ (verbose)
		semsg(_(e_could_not_ఎక్కించు_library_function_str), thunk->name);
	    తిరిగి FAIL;
	}
    }
    కొరకు (thunk = mzgc_imports; thunk->name; thunk++)
    {
	ఒకవేళ ((*thunk->ptr =
		    (శూన్య *)Get_Proc_చిరునామా(hMzGC, thunk->name)) == ఖాళీ)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    ఒకవేళ (verbose)
		semsg(_(e_could_not_ఎక్కించు_library_function_str), thunk->name);
	    తిరిగి FAIL;
	}
    }
    తిరిగి OK;
}

    పూర్ణాంకం
mzscheme_enabled(పూర్ణాంకం verbose)
{
    తిరిగి mzscheme_runtime_link_init(
	    (అక్షరం *)p_mzschemedll, (అక్షరం *)p_mzschemegcdll, verbose) == OK;
}

    స్థిర శూన్య
dynamic_mzscheme_చివర(శూన్య)
{
    ఒకవేళ (hMzSch)
    {
	FreeLibrary(hMzSch);
	hMzSch = 0;
    }
    ఒకవేళ (hMzGC)
    {
	FreeLibrary(hMzGC);
	hMzGC = 0;
    }
}
#అయితే_ముగింపు // DYNAMIC_MZSCHEME

#ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, అంకె) GUARANTEE_STRING(proc, అంకె)
#లేకపోతే
    స్థిర Scheme_Object *
guaranteed_byte_string_arg(అక్షరం *proc, పూర్ణాంకం అంకె, పూర్ణాంకం argc, Scheme_Object **argv)
{
    ఒకవేళ (SCHEME_BYTE_STRINGP(argv[అంకె]))
    {
	తిరిగి argv[అంకె];
    }
    లేకపోతే ఒకవేళ (SCHEME_CHAR_STRINGP(argv[అంకె]))
    {
	Scheme_Object *tmp = ఖాళీ;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_లోపల_REG(0, argv[అంకె]);
	MZ_GC_VAR_లోపల_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_గమ్యం_byte_string(argv[అంకె]);
	MZ_GC_UNREG();
	తిరిగి tmp;
    }
    లేకపోతే
	scheme_wrong_type(proc, "string", అంకె, argc, argv);
    // unreachable
    తిరిగి scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, అంకె) guaranteed_byte_string_arg(proc, అంకె, argc, argv)
#అయితే_ముగింపు

// need to put it here for dynamic stuff to work
#ఒకవేళ defined(INCLUDE_MZSCHEME_పునాది)
# include "mzscheme_base.c"
#అయితే_ముగింపు

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

స్థిర Scheme_Type mz_బఫరు_type;
స్థిర Scheme_Type mz_కిటికీ_type;

స్థిర పూర్ణాంకం initialized = FALSE;
#నిర్వచితమైతే DYNAMIC_MZSCHEME
స్థిర పూర్ణాంకం disabled = FALSE;
#అయితే_ముగింపు
స్థిర పూర్ణాంకం ఎక్కించు_పునాది_module_failed = FALSE;

// global environment
స్థిర Scheme_Env    *environment = ఖాళీ;
// output/error handlers
స్థిర Scheme_Object *curout = ఖాళీ;
స్థిర Scheme_Object *curerr = ఖాళీ;
// exn:vim exception
స్థిర Scheme_Object *exn_catching_apply = ఖాళీ;
స్థిర Scheme_Object *exn_p = ఖాళీ;
స్థిర Scheme_Object *exn_సందేశం = ఖాళీ;
స్థిర Scheme_Object *vim_exn = ఖాళీ; // Vim Error exception

#ఒకవేళ !defined(MZ_PRECISE_GC) || MZSCHEME_సంస్కరణ_MAJOR < 400
స్థిర శూన్య *దొంతర_పునాది = ఖాళీ;
#అయితే_ముగింపు

స్థిర దీర్ఘ range_ప్రారంభం;
స్థిర దీర్ఘ range_చివర;

// MzScheme threads scheduling stuff
స్థిర పూర్ణాంకం mz_దారాలు_allow = 0;

#ఒకవేళ defined(FEAT_GUI_MSWIN)
స్థిర శూన్య CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
స్థిర UINT timer_id = 0;
#లేదావేళ defined(FEAT_GUI_GTK)
స్థిర gboolean timer_proc(gpointer);
స్థిర guint timer_id = 0;
#లేదావేళ defined(FEAT_GUI_MOTIF)
స్థిర శూన్య timer_proc(Xt_సూచిక, XtIntervalId *);
స్థిర XtIntervalId timer_id = (XtIntervalId)0;
#అయితే_ముగింపు

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    శూన్య
mzvim_check_దారాలు(శూన్య)
{
    // Last time MzScheme threads were scheduled
    స్థిర time_t mz_చివరి_సమయం = 0;

    ఒకవేళ (mz_దారాలు_allow && p_mzq > 0)
    {
	time_t now = సమయం(ఖాళీ);

	ఒకవేళ ((now - mz_చివరి_సమయం) * 1000 > p_mzq)
	{
	    mz_చివరి_సమయం = now;
	    scheme_check_దారాలు();
	}
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(MZSCHEME_GUI_దారాలు)
స్థిర శూన్య అమరిక_timer(శూన్య);
స్థిర శూన్య remove_timer(శూన్య);

// timers are presented in GUI only
# ఒకవేళ defined(FEAT_GUI_MSWIN)
    స్థిర శూన్య CALLBACK
timer_proc(
    HWND	hwnd వాడని,
    UINT	u_సందేశం వాడని,
    UINT_PTR	idEvent వాడని,
    DWORD	dw_సమయం వాడని)
# elif defined(FEAT_GUI_GTK)
    స్థిర gboolean
timer_proc(gpointer దత్తాంశం వాడని)
# elif defined(FEAT_GUI_MOTIF)
    స్థిర శూన్య
timer_proc(Xt_సూచిక timed_బహిర్గతం వాడని, XtIntervalId *interval_id వాడని)
# endif
{
    scheme_check_దారాలు();
# ఒకవేళ defined(FEAT_GUI_GTK)
    తిరిగి TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    ఒకవేళ (mz_దారాలు_allow && p_mzq > 0)
	timer_id = Xt_App_కూడు_సమయం_బహిర్గతం(app_context, p_mzq,
		timer_proc, ఖాళీ);
# endif
}

    స్థిర శూన్య
అమరిక_timer(శూన్య)
{
# ఒకవేళ defined(FEAT_GUI_MSWIN)
    timer_id = సమితి_Timer(ఖాళీ, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_కూడు((guint)p_mzq, (GSourceFunc)timer_proc, ఖాళీ);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = Xt_App_కూడు_సమయం_బహిర్గతం(app_context, p_mzq, timer_proc, ఖాళీ);
# endif
}

    స్థిర శూన్య
remove_timer(శూన్య)
{
# ఒకవేళ defined(FEAT_GUI_MSWIN)
    KillTimer(ఖాళీ, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_ఆధారం_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    Xt_Remove_సమయం_బహిర్గతం(timer_id);
# endif
    timer_id = 0;
}

#అయితే_ముగింపు // MZSCHEME_GUI_THREADS

    అక్షరం *
did_సమితి_mzquantum(optset_T *వాదనలు వాడని)
{
#ఒకవేళ defined(MZSCHEME_GUI_దారాలు)
    // reset timer
    ఒకవేళ (timer_id != 0)
	remove_timer();
    ఒకవేళ (mz_దారాలు_allow && p_mzq > 0 && gui.లోపల_use)
	అమరిక_timer();
#అయితే_ముగింపు
    తిరిగి ఖాళీ;
}

    స్థిర శూన్య
notify_multithread(పూర్ణాంకం on)
{
    mz_దారాలు_allow = on;
#నిర్వచితమైతే MZSCHEME_GUI_దారాలు
    ఒకవేళ (on && timer_id == 0 && p_mzq > 0 && gui.లోపల_use)
	అమరిక_timer();
    ఒకవేళ (!on && timer_id != 0)
	remove_timer();
#అయితే_ముగింపు
}

    శూన్య
mzscheme_చివర(శూన్య)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#ఒకవేళ 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_చివర();
# endif
#అయితే_ముగింపు
}

#ఒకవేళ HAVE_TLS_SPACE && !defined(VIMDLL)
# ఒకవేళ defined(_MSC_VER)
స్థిర __declspec(thread) శూన్య *tls_space;
బాహ్య intptr_t tls_సూచీ;
# elif defined(__MINGW32__)
స్థిర __thread శూన్య *tls_space;
బాహ్య intptr_t tls_సూచీ;
# లేకపోతే
స్థిర THREAD_LOCAL శూన్య *tls_space;
స్థిర intptr_t tls_సూచీ = 0;
# endif
#అయితే_ముగింపు

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    పూర్ణాంకం
mzscheme_main(శూన్య)
{
    పూర్ణాంకం	    argc = 0;
    అక్షరం    *argv = ఖాళీ;

#నిర్వచితమైతే DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    ఒకవేళ (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	తిరిగి vim_main2();
    }
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_TLS_SPACE
# ifdef VIMDLL
    శూన్య **ptls_space;
    intptr_t tls_సూచీ_2;
    శూన్య (*pget_tls_info)(శూన్య ***ptls_space, intptr_t *ptls_సూచీ);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (శూన్య *)Get_Proc_చిరునామా(
				GetModuleHandle(ఖాళీ), "get_tls_info");
    ఒకవేళ (pget_tls_info == ఖాళీ)
    {
	disabled = TRUE;
	తిరిగి vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_సూచీ_2);
    scheme_రిజిస్టరు_tls_space(ptls_space, tls_సూచీ_2);
# లేకపోతే
    scheme_రిజిస్టరు_tls_space(&tls_space, tls_సూచీ);
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే TRAMPOLINED_MZVIM_STARTUP
    తిరిగి scheme_main_అమరిక(TRUE, mzscheme_env_main, argc, &argv);
#లేకపోతే
    తిరిగి mzscheme_env_main(ఖాళీ, argc, &argv);
#అయితే_ముగింపు
}

    స్థిర పూర్ణాంకం
mzscheme_env_main(Scheme_Env *env, పూర్ణాంకం argc వాడని, అక్షరం **argv వాడని)
{
#నిర్వచితమైతే TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#లేకపోతే
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = ఖాళీ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, dummy);

    దొంతర_పునాది = &__gc_var_stack__;
# లేకపోతే
    పూర్ణాంకం dummy = 0;
    దొంతర_పునాది = (శూన్య *)&dummy;
# endif
#అయితే_ముగింపు

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    తిరిగి 0;
}

    స్థిర Scheme_Object*
ఎక్కించు_పునాది_module(శూన్య *దత్తాంశం)
{
    scheme_namespace_require(scheme_intern_చిహ్నం((అక్షరం *)దత్తాంశం));
    తిరిగి scheme_null;
}

    స్థిర Scheme_Object *
ఎక్కించు_పునాది_module_on_error(శూన్య *దత్తాంశం వాడని)
{
    ఎక్కించు_పునాది_module_failed = TRUE;
    తిరిగి scheme_null;
}

    స్థిర పూర్ణాంకం
startup_mzscheme(శూన్య)
{
#నిర్వచితం_కాకపోతే TRAMPOLINED_MZVIM_STARTUP
    scheme_సమితి_దొంతర_పునాది(దొంతర_పునాది, 1);
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే TRAMPOLINED_MZVIM_STARTUP
    // in newer versions of precise GC the initial env has been created
    environment = scheme_basic_env();
#అయితే_ముగింపు

    MZ_రిజిస్టరు_STATIC(environment);
    MZ_రిజిస్టరు_STATIC(curout);
    MZ_రిజిస్టరు_STATIC(curerr);
    MZ_రిజిస్టరు_STATIC(exn_catching_apply);
    MZ_రిజిస్టరు_STATIC(exn_p);
    MZ_రిజిస్టరు_STATIC(exn_సందేశం);
    MZ_రిజిస్టరు_STATIC(vim_exn);

    MZ_GC_CHECK();

#నిర్వచితమైతే INCLUDE_MZSCHEME_పునాది
    // invoke function from generated and included mzscheme_base.c
    declare_modules(environment);
#అయితే_ముగింపు

    // setup 'current-library-collection-paths' parameter
#ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 299
    {
	Scheme_Object *coll_మార్గం = ఖాళీ;
	పూర్ణాంకం mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_లోపల_REG(0, coll_మార్గం);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	ఒకవేళ (s != ఖాళీ)
	{
	    coll_మార్గం = scheme_make_మార్గం((అక్షరం *)s);
	    MZ_GC_CHECK();
	    ఒకవేళ (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	ఒకవేళ (coll_మార్గం == ఖాళీ)
	{
	    coll_మార్గం = scheme_make_మార్గం(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	ఒకవేళ (coll_మార్గం != ఖాళీ)
	{
	    scheme_సమితి_collects_మార్గం(coll_మార్గం);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#లేకపోతే
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = ఖాళీ;
	Scheme_Object	*coll_pair = ఖాళీ;
	Scheme_Config	*config = ఖాళీ;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_లోపల_REG(0, coll_string);
	MZ_GC_VAR_లోపల_REG(1, coll_pair);
	MZ_GC_VAR_లోపల_REG(2, config);
	MZ_GC_REG();
	coll_string = scheme_make_byte_string(MZSCHEME_COLLECTS);
	MZ_GC_CHECK();
	coll_pair = scheme_make_pair(coll_string, scheme_null);
	MZ_GC_CHECK();
	config = scheme_current_config();
	MZ_GC_CHECK();
	scheme_సమితి_param(config, MZCONFIG_COLLECTION_PATHS, coll_pair);
	MZ_GC_CHECK();
	MZ_GC_UNREG();
    }
# endif
#అయితే_ముగింపు

#ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 600
    {
	Scheme_Object *config_మార్గం = ఖాళీ;
	పూర్ణాంకం mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_లోపల_REG(0, config_మార్గం);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	ఒకవేళ (s != ఖాళీ)
	{
	    config_మార్గం = scheme_make_మార్గం((అక్షరం *)s);
	    MZ_GC_CHECK();
	    ఒకవేళ (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	ఒకవేళ (config_మార్గం == ఖాళీ)
	{
	    config_మార్గం = scheme_make_మార్గం(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	ఒకవేళ (config_మార్గం != ఖాళీ)
	{
	    scheme_సమితి_config_మార్గం(config_మార్గం);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#అయితే_ముగింపు

#ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#అయితే_ముగింపు

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(ఖాళీ, ఎక్కించు_పునాది_module, ఖాళీ,
				    ఎక్కించు_పునాది_module_on_error, "racket/base");
	ఒకవేళ (ఎక్కించు_పునాది_module_failed)
	{
	    ఎక్కించు_పునాది_module_failed = FALSE;
	    scheme_dynamic_wind(ఖాళీ, ఎక్కించు_పునాది_module, ఖాళీ,
				    ఎక్కించు_పునాది_module_on_error, "scheme/base");
	    ఒకవేళ (ఎక్కించు_పునాది_module_failed)
		తిరిగి -1;
	}
    }

    రిజిస్టరు_vim_exn();
    // use new environment to initialise exception handling
    init_exn_catching_apply();

    // redirect output
    scheme_console_output = do_output;
    scheme_console_printf = do_printf;

#నిర్వచితమైతే HAVE_SANDBOX
    {
	Scheme_Object	*make_security_guard = ఖాళీ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_లోపల_REG(0, make_security_guard);
	MZ_GC_REG();

# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_చిహ్నం = ఖాళీ;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_లోపల_REG(0, make_security_guard_చిహ్నం);
	    MZ_GC_REG();
	    make_security_guard_చిహ్నం = scheme_intern_చిహ్నం("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_చిహ్నం, environment);
	    MZ_GC_UNREG();
	}
# లేకపోతే
	make_security_guard = scheme_builtin_విలువ("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	ఒకవేళ (make_security_guard != ఖాళీ)
	{
	    Scheme_Object   *వాదనలు[3] = {ఖాళీ, ఖాళీ, ఖాళీ};
	    Scheme_Object   *guard = ఖాళీ;
	    Scheme_Config   *config = ఖాళీ;
	    MZ_GC_DECL_REG(5);
	    MZ_GC_ARRAY_VAR_లోపల_REG(0, వాదనలు, 3);
	    MZ_GC_VAR_లోపల_REG(3, guard);
	    MZ_GC_VAR_లోపల_REG(4, config);
	    MZ_GC_REG();
	    config = scheme_current_config();
	    MZ_GC_CHECK();
	    వాదనలు[0] = scheme_get_param(config, MZCONFIG_SECURITY_GUARD);
	    MZ_GC_CHECK();
	    వాదనలు[1] = scheme_make_prim_w_arity(sandbox_file_guard,
		    "sandbox-file-guard", 3, 3);
	    వాదనలు[2] = scheme_make_prim_w_arity(sandbox_జాలం_guard,
		    "sandbox-network-guard", 4, 4);
	    guard = scheme_apply(make_security_guard, 3, వాదనలు);
	    MZ_GC_CHECK();
	    scheme_సమితి_param(config, MZCONFIG_SECURITY_GUARD, guard);
	    MZ_GC_CHECK();
	    MZ_GC_UNREG();
	}
	MZ_GC_UNREG();
    }
#అయితే_ముగింపు
    // Create buffer and window types for use in Scheme code
    mz_బఫరు_type = scheme_make_type("<vim-buffer>");
    MZ_GC_CHECK();
    mz_కిటికీ_type = scheme_make_type("<vim-window>");
    MZ_GC_CHECK();
#నిర్వచితమైతే MZ_PRECISE_GC
    GC_రిజిస్టరు_traversers(mz_బఫరు_type,
	    బఫరు_కొలత_proc, బఫరు_mark_proc, బఫరు_fixup_proc,
	    TRUE, TRUE);
    GC_రిజిస్టరు_traversers(mz_కిటికీ_type,
	    కిటికీ_కొలత_proc, కిటికీ_mark_proc, కిటికీ_fixup_proc,
	    TRUE, TRUE);
#అయితే_ముగింపు

    make_modules();

    /*
     * setup callback to receive notifications
     * whether thread scheduling is (or not) required
     */
    scheme_notify_multithread = notify_multithread;

    తిరిగి 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    స్థిర పూర్ణాంకం
mzscheme_init(శూన్య)
{
    ఒకవేళ (!initialized)
    {
#నిర్వచితమైతే DYNAMIC_MZSCHEME
	ఒకవేళ (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    తిరిగి -1;
	}
#అయితే_ముగింపు
	ఒకవేళ (ఎక్కించు_పునాది_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_పునాది_module_could_not_be_loaded));
	    తిరిగి -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = ఖాళీ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_లోపల_REG(0, config);
	MZ_GC_REG();
	config = scheme_current_config();
	MZ_GC_CHECK();
	// recreate ports each call effectively clearing these ones
	curout = scheme_make_byte_string_output_పోర్టు();
	MZ_GC_CHECK();
	curerr = scheme_make_byte_string_output_పోర్టు();
	MZ_GC_CHECK();
	scheme_సమితి_param(config, MZCONFIG_OUTPUT_పోర్టు, curout);
	MZ_GC_CHECK();
	scheme_సమితి_param(config, MZCONFIG_ERROR_పోర్టు, curerr);
	MZ_GC_CHECK();
	MZ_GC_UNREG();
    }

    తిరిగి 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    స్థిర పూర్ణాంకం
eval_with_exn_handling(
    శూన్య		*దత్తాంశం,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *విలువ = ఖాళీ;
    Scheme_Object   *exn = ఖాళీ;
    Scheme_Object   *prim = ఖాళీ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_లోపల_REG(0, విలువ);
    MZ_GC_VAR_లోపల_REG(1, exn);
    MZ_GC_VAR_లోపల_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, దత్తాంశం, "mzvim", 0, 0);
    MZ_GC_CHECK();
    విలువ = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    ఒకవేళ (!విలువ)
    {
	విలువ = వెలికితీయి_exn_సందేశం(exn);
	// Got an exn?
	ఒకవేళ (విలువ)
	{
	    scheme_display(విలువ, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	తిరిగి FAIL;
    }

    ఒకవేళ (ret != ఖాళీ)	// if pointer to retval supported give it up
	*ret = విలువ;
    // Print any result, as long as it's not a void
    లేకపోతే ఒకవేళ (!SCHEME_VOIDP(విలువ))
    {
	scheme_display(విలువ, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    తిరిగి OK;
}

/*
 * :mzscheme
 */
    స్థిర పూర్ణాంకం
do_mzscheme_command(exarg_T *eap, శూన్య *దత్తాంశం, Scheme_Closed_Prim *what)
{
    ఒకవేళ (mzscheme_init())
	తిరిగి FAIL;

    range_ప్రారంభం = eap->line1;
    range_చివర = eap->line2;

    తిరిగి eval_with_exn_handling(దత్తాంశం, what, ఖాళీ);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    శూన్య
mzscheme_బఫరు_free(buf_T *buf)
{
    ఒకవేళ (buf->b_mzscheme_ref == ఖాళీ)
	తిరిగి;

    vim_mz_బఫరు *bp = ఖాళీ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, bp);
    MZ_GC_REG();

    bp = బఫరు_REF(buf);
    bp->buf = INVALID_బఫరు_విలువ;
#నిర్వచితమైతే MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#లేకపోతే
    scheme_gc_ptr_ok(bp);
#అయితే_ముగింపు
    buf->b_mzscheme_ref = ఖాళీ;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    శూన్య
mzscheme_కిటికీ_free(win_T *win)
{
    ఒకవేళ (win->w_mzscheme_ref == ఖాళీ)
	తిరిగి;

    vim_mz_కిటికీ *wp = ఖాళీ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, wp);
    MZ_GC_REG();
    wp = కిటికీ_REF(win);
    wp->win = INVALID_కిటికీ_విలువ;
#నిర్వచితమైతే MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#లేకపోతే
    scheme_gc_ptr_ok(wp);
#అయితే_ముగింపు
    win->w_mzscheme_ref = ఖాళీ;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * ":mzscheme" (or ":mz")
 */
    శూన్య
ex_mzscheme(exarg_T *eap)
{
    char_u	*script;

    script = script_get(eap, eap->arg);
    ఒకవేళ (eap->skip)
	తిరిగి;

    ఒకవేళ (script == ఖాళీ)
	do_mzscheme_command(eap, eap->arg, do_eval);
    లేకపోతే
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    స్థిర Scheme_Object *
do_ఎక్కించు(శూన్య *దత్తాంశం, పూర్ణాంకం noargc వాడని, Scheme_Object **noargv వాడని)
{
    Scheme_Object   *సమాసం = ఖాళీ;
    Scheme_Object   *ఫలితం = ఖాళీ;
    అక్షరం	    *file = ఖాళీ;
    పోర్టు_Info	    *pinfo = (పోర్టు_Info *)దత్తాంశం;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_లోపల_REG(0, సమాసం);
    MZ_GC_VAR_లోపల_REG(1, ఫలితం);
    MZ_GC_VAR_లోపల_REG(2, file);
    MZ_GC_REG();

    file = (అక్షరం *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->పోర్టు = scheme_open_ఆదానం_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_లెక్క_పంక్తులు(pinfo->పోర్టు);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    అయితే (!SCHEME_EOFP(సమాసం = scheme_read(pinfo->పోర్టు)))
    {
	ఫలితం = scheme_eval(సమాసం, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_ఆదానం_పోర్టు(pinfo->పోర్టు);
    MZ_GC_CHECK();
    pinfo->పోర్టు = ఖాళీ;
    MZ_GC_UNREG();
    తిరిగి ఫలితం;
}

/*
 * :mzfile
 */
    శూన్య
ex_mzfile(exarg_T *eap)
{
    పోర్టు_Info	pinfo = {ఖాళీ, ఖాళీ};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, pinfo.పోర్టు);
    MZ_GC_REG();

    pinfo.name = (అక్షరం *)eap->arg;
    ఒకవేళ (do_mzscheme_command(eap, &pinfo, do_ఎక్కించు) != OK
	    && pinfo.పోర్టు != ఖాళీ)	// looks like port was not closed
    {
	scheme_close_ఆదానం_పోర్టు(pinfo.పోర్టు);
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
    స్థిర శూన్య
init_exn_catching_apply(శూన్య)
{
    ఒకవేళ (exn_catching_apply)
	తిరిగి;

    అక్షరం *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_విలువ("exn?");
    MZ_GC_CHECK();
    exn_సందేశం = scheme_builtin_విలువ("exn-message");
    MZ_GC_CHECK();
}

/*
 * This function applies a thunk, returning the Scheme value if there's
 * no exception, otherwise returning NULL and setting *exn to the raised
 * value (usually an exn structure).
 */
    స్థిర Scheme_Object *
_apply_thunk_catch_exceptions(Scheme_Object *f, Scheme_Object **exn)
{
    Scheme_Object *v;

    v = _scheme_apply(exn_catching_apply, 1, &f);
    // v is a pair: (cons #t value) or (cons #f exn)

    ఒకవేళ (SCHEME_TRUEP(SCHEME_CAR(v)))
	తిరిగి SCHEME_CDR(v);
    లేకపోతే
    {
	*exn = SCHEME_CDR(v);
	తిరిగి ఖాళీ;
    }
}

    స్థిర Scheme_Object *
వెలికితీయి_exn_సందేశం(Scheme_Object *v)
{
    ఒకవేళ (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	తిరిగి _scheme_apply(exn_సందేశం, 1, &v);
    లేకపోతే
	తిరిగి ఖాళీ; // Not an exn structure
}

    స్థిర Scheme_Object *
do_eval(శూన్య *s, పూర్ణాంకం noargc వాడని, Scheme_Object **noargv వాడని)
{
    తిరిగి scheme_eval_string_all((అక్షరం *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    స్థిర శూన్య
do_intrnl_output(అక్షరం *mesg, పూర్ణాంకం error)
{
    అక్షరం *p, *prev;

    prev = mesg;
    p = strchr(prev, '\n');
    అయితే (p)
    {
	*p = '\0';
	ఒకవేళ (error)
	    emsg(prev);
	లేకపోతే
	    సందేశం(prev);
	prev = p + 1;
	p = strchr(prev, '\n');
    }

    ఒకవేళ (error)
	emsg(prev);
    లేకపోతే
	సందేశం(prev);
}

    స్థిర శూన్య
do_output(అక్షరం *mesg, OUTPUT_LEN_TYPE len వాడని)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    స్థిర శూన్య
do_err_output(అక్షరం *mesg)
{
    do_intrnl_output(mesg, 1);
}

    స్థిర శూన్య
do_printf(అక్షరం *format, ...)
{
    do_intrnl_output(format, 1);
}

    స్థిర శూన్య
do_flush(శూన్య)
{
    అక్షరం *buff;
    OUTPUT_LEN_TYPE నిడివి;

    buff = scheme_get_sized_byte_string_output(curerr, &నిడివి);
    MZ_GC_CHECK();
    ఒకవేళ (నిడివి)
    {
	do_err_output(buff);
	తిరిగి;
    }

    buff = scheme_get_sized_byte_string_output(curout, &నిడివి);
    MZ_GC_CHECK();
    ఒకవేళ (నిడివి)
	do_output(buff, నిడివి);
}

/*
 *========================================================================
 *  4. Implementation of the Vim Features for MzScheme
 *========================================================================
 */

/*
 * (command {command-string})
 */
    స్థిర Scheme_Object *
vim_command(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)దత్తాంశం;
    Scheme_Object *cmd = ఖాళీ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_విలువ(cmd), ఖాళీ, ఖాళీ, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    తిరిగి scheme_void;
}

/*
 * (eval {expr-string})
 */
    స్థిర Scheme_Object *
vim_eval(శూన్య *దత్తాంశం వాడని, పూర్ణాంకం argc వాడని, Scheme_Object **argv వాడని)
{
#నిర్వచితమైతే FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)దత్తాంశం;
    Scheme_Object	*ఫలితం = ఖాళీ;
    typval_T		*vim_ఫలితం;
    Scheme_Object *సమాసం = ఖాళీ;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_లోపల_REG(0, ఫలితం);
    MZ_GC_VAR_లోపల_REG(1, సమాసం);
    MZ_GC_REG();
    సమాసం = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_ఫలితం = eval_సమాసం(BYTE_STRING_విలువ(సమాసం), ఖాళీ);

    ఒకవేళ (vim_ఫలితం == ఖాళీ)
	raise_vim_exn(_("invalid expression"));

    ఫలితం = vim_గమ్యం_mzscheme(vim_ఫలితం);
    MZ_GC_CHECK();
    free_tv(vim_ఫలితం);

    MZ_GC_UNREG();
    తిరిగి ఫలితం;
#లేకపోతే
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    తిరిగి scheme_false;
#అయితే_ముగింపు
}

/*
 * (range-start)
 */
    స్థిర Scheme_Object *
get_range_ప్రారంభం(
    శూన్య		*దత్తాంశం వాడని,
    పూర్ణాంకం			argc వాడని,
    Scheme_Object	**argv వాడని)
{
    తిరిగి scheme_make_integer(range_ప్రారంభం);
}

/*
 * (range-end)
 */
    స్థిర Scheme_Object *
get_range_చివర(శూన్య *దత్తాంశం వాడని, పూర్ణాంకం argc వాడని, Scheme_Object **argv వాడని)
{
    తిరిగి scheme_make_integer(range_చివర);
}

/*
 * (beep)
 */
    స్థిర Scheme_Object *
mzscheme_beep(శూన్య *దత్తాంశం వాడని, పూర్ణాంకం argc వాడని, Scheme_Object **argv వాడని)
{
    vim_beep(BO_LANG);
    తిరిగి scheme_void;
}

స్థిర Scheme_Object *M_global = ఖాళీ;

/*
 * (get-option {option-name}) [buffer/window]
 */
    స్థిర Scheme_Object *
get_option(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    దీర్ఘ	    విలువ;
    అక్షరం	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = ఖాళీ;
    Scheme_Object   *name = ఖాళీ;
    పూర్ణాంకం		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_లోపల_REG(0, rval);
    MZ_GC_VAR_లోపల_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    ఒకవేళ (argc > 1)
    {
	ఒకవేళ (M_global == ఖాళీ)
	{
	    MZ_రిజిస్టరు_STATIC(M_global);
	    M_global = scheme_intern_చిహ్నం("global");
	    MZ_GC_CHECK();
	}

	ఒకవేళ (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	లేకపోతే ఒకవేళ (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_బఫరు(argv[1]);
	    scope = OPT_LOCAL;
	}
	లేకపోతే ఒకవేళ (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_కిటికీ(argv[1]);

	    curwin = win;
	    curbuf = win->w_బఫరు;
	    scope = OPT_LOCAL;
	}
	లేకపోతే
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_విలువ(BYTE_STRING_విలువ(name), &విలువ, (char_u **)&strval,
								  ఖాళీ, scope);
    curbuf = save_curb;
    curwin = save_curw;

    ఎంపిక (rc)
    {
    స్థితి gov_bool:
    స్థితి gov_సంఖ్య:
	MZ_GC_UNREG();
	తిరిగి scheme_make_integer_విలువ(విలువ);
    స్థితి gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	తిరిగి rval;
    స్థితి gov_hidden_bool:
    స్థితి gov_hidden_సంఖ్య:
    స్థితి gov_hidden_string:
	MZ_GC_UNREG();
	raise_vim_exn(_("hidden option"));
	//NOTREACHED
    స్థితి gov_unknown:
	MZ_GC_UNREG();
	raise_vim_exn(_("unknown option"));
	//NOTREACHED
    }
    // unreachable
    తిరిగి scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    స్థిర Scheme_Object *
సమితి_option(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    char_u	*command = ఖాళీ;
    పూర్ణాంకం		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)దత్తాంశం;
    Scheme_Object *cmd = ఖాళీ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    ఒకవేళ (argc > 1)
    {
	ఒకవేళ (M_global == ఖాళీ)
	{
	    MZ_రిజిస్టరు_STATIC(M_global);
	    M_global = scheme_intern_చిహ్నం("global");
	    MZ_GC_CHECK();
	}

	ఒకవేళ (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	లేకపోతే ఒకవేళ (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_బఫరు(argv[1]);
	    scope = OPT_LOCAL;
	}
	లేకపోతే ఒకవేళ (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_కిటికీ(argv[1]);
	    curwin = win;
	    curbuf = win->w_బఫరు;
	    scope = OPT_LOCAL;
	}
	లేకపోతే
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    // do_set can modify cmd, make copy
    command = vim_strsave(BYTE_STRING_విలువ(cmd));
    MZ_GC_UNREG();
    do_సమితి(command, scope);
    vim_free(command);
    update_screen(UPD_NOT_VALID);
    curbuf = save_curb;
    curwin = save_curw;
    raise_if_error();
    తిరిగి scheme_void;
}

/*
 *===========================================================================
 *  5. Vim Window-related Manipulation Functions
 *===========================================================================
 */

/*
 * (curr-win)
 */
    స్థిర Scheme_Object *
get_curr_win(శూన్య *దత్తాంశం వాడని, పూర్ణాంకం argc వాడని, Scheme_Object **argv వాడని)
{
    తిరిగి (Scheme_Object *)get_vim_curr_కిటికీ();
}

/*
 * (win-count)
 */
    స్థిర Scheme_Object *
get_కిటికీ_లెక్క(
    శూన్య		*దత్తాంశం వాడని,
    పూర్ణాంకం			argc వాడని,
    Scheme_Object	**argv వాడని)
{
    పూర్ణాంకం	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    తిరిగి scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    స్థిర Scheme_Object *
get_కిటికీ_list(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_బఫరు   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_బఫరు_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    కొరకు ( ; w != ఖాళీ; w = w->w_తదుపరి)
	ఒకవేళ (w->w_బఫరు == buf->buf)
	{
	    list = scheme_make_pair(కిటికీ_new(w), list);
	    MZ_GC_CHECK();
	}

    తిరిగి list;
}

    స్థిర Scheme_Object *
కిటికీ_new(win_T *win)
{
    vim_mz_కిటికీ *self = ఖాళీ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    ఒకవేళ (win->w_mzscheme_ref != ఖాళీ)
	తిరిగి (Scheme_Object *)కిటికీ_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, పరిమాణం(vim_mz_కిటికీ));
    CLEAR_సూచిక(self);
#నిర్వచితమైతే MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(ఖాళీ);
#లేకపోతే
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#అయితే_ముగింపు
    MZ_GC_CHECK();
    కిటికీ_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_కిటికీ_type;

    MZ_GC_UNREG();
    తిరిగి (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    స్థిర Scheme_Object *
get_కిటికీ_అంకె(శూన్య *దత్తాంశం వాడని, పూర్ణాంకం argc వాడని, Scheme_Object **argv వాడని)
{
    పూర్ణాంకం		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)దత్తాంశం;
    win_T	*win = get_కిటికీ_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    కొరకు (wp = firstwin; wp != win; wp = wp->w_తదుపరి)
	++nr;

    తిరిగి scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    స్థిర Scheme_Object *
get_కిటికీ_by_అంకె(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)దత్తాంశం;
    win_T	*win = firstwin;
    పూర్ణాంకం		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ఒకవేళ (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    కొరకు ( ; win != ఖాళీ; win = win->w_తదుపరి, --fnum)
	ఒకవేళ (fnum == 1)	    // to be 1-based
	    తిరిగి కిటికీ_new(win);

    తిరిగి scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    స్థిర Scheme_Object *
get_కిటికీ_బఫరు(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_కిటికీ   *win = get_కిటికీ_arg(prim->name, 0, argc, argv);

    తిరిగి బఫరు_new(win->win->w_బఫరు);
}

/*
 * (get-win-height [window])
 */
    స్థిర Scheme_Object *
get_కిటికీ_ఎత్తు(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_కిటికీ   *win = get_కిటికీ_arg(prim->name, 0, argc, argv);

    తిరిగి scheme_make_integer(win->win->w_ఎత్తు);
}

/*
 * (set-win-height {height} [window])
 */
    స్థిర Scheme_Object *
సమితి_కిటికీ_ఎత్తు(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_కిటికీ   *win;
    win_T	    *savewin;
    పూర్ణాంకం		    ఎత్తు;

    win = get_కిటికీ_arg(prim->name, 1, argc, argv);
    ఎత్తు = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

    savewin = curwin;
    curwin = win->win;
    win_setheight(ఎత్తు);
    curwin = savewin;

    raise_if_error();
    తిరిగి scheme_void;
}

/*
 * (get-win-width [window])
 */
    స్థిర Scheme_Object *
get_కిటికీ_వెడల్పు(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_కిటికీ   *win = get_కిటికీ_arg(prim->name, 0, argc, argv);

    తిరిగి scheme_make_integer(win->win->w_వెడల్పు);
}

/*
 * (set-win-width {width} [window])
 */
    స్థిర Scheme_Object *
సమితి_కిటికీ_వెడల్పు(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_కిటికీ   *win;
    win_T	    *savewin;
    పూర్ణాంకం		    వెడల్పు = 0;

    win = get_కిటికీ_arg(prim->name, 1, argc, argv);
    వెడల్పు = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

    savewin = curwin;
    curwin = win->win;
    win_setwidth(వెడల్పు);
    curwin = savewin;

    raise_if_error();
    తిరిగి scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    స్థిర Scheme_Object *
get_cursor(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_కిటికీ   *win;
    pos_T	    pos;

    win = get_కిటికీ_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    తిరిగి scheme_make_pair(scheme_make_integer_విలువ((దీర్ఘ)pos.lnum),
		    scheme_make_integer_విలువ((దీర్ఘ)pos.నిలువు + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    స్థిర Scheme_Object *
సమితి_cursor(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_కిటికీ   *win;
    దీర్ఘ	    lnum = 0;
    దీర్ఘ	    నిలువు = 0;

#నిర్వచితమైతే HAVE_SANDBOX
    sandbox_check();
#అయితే_ముగింపు
    win = get_కిటికీ_arg(prim->name, 1, argc, argv);
    GUARANTEE_PAIR(prim->name, 0);

    ఒకవేళ (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    నిలువు = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_పంక్తి_range(lnum, win->win->w_బఫరు);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.నిలువు = నిలువు;
    win->win->w_సమితి_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    తిరిగి scheme_void;
}
/*
 *===========================================================================
 *  6. Vim Buffer-related Manipulation Functions
 *===========================================================================
 */

/*
 * (open-buff {filename})
 */
    స్థిర Scheme_Object *
mzscheme_open_బఫరు(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    పూర్ణాంకం		    అంకె = 0;
    Scheme_Object   *onum = ఖాళీ;
    Scheme_Object   *buf = ఖాళీ;
    Scheme_Object   *fname;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_లోపల_REG(0, onum);
    MZ_GC_VAR_లోపల_REG(1, buf);
    MZ_GC_VAR_లోపల_REG(2, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);

#నిర్వచితమైతే HAVE_SANDBOX
    sandbox_check();
#అయితే_ముగింపు
    // TODO make open existing file
    అంకె = buflist_కూడు(BYTE_STRING_విలువ(fname), BLN_LISTED | BLN_CURBUF);

    ఒకవేళ (అంకె == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(అంకె);
    buf = get_బఫరు_by_అంకె(దత్తాంశం, 1, &onum);
    MZ_GC_UNREG();
    తిరిగి buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    స్థిర Scheme_Object *
get_బఫరు_by_అంకె(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)దత్తాంశం;
    buf_T	*buf;
    పూర్ణాంకం		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	ఒకవేళ (buf->b_fnum == fnum)
	    తిరిగి బఫరు_new(buf);

    తిరిగి scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    స్థిర Scheme_Object *
get_బఫరు_by_name(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)దత్తాంశం;
    buf_T	*buf;
    Scheme_Object *బఫరు = ఖాళీ;
    Scheme_Object *fname = ఖాళీ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_లోపల_REG(0, బఫరు);
    MZ_GC_VAR_లోపల_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    బఫరు = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	ఒకవేళ (buf->b_ffname == ఖాళీ || buf->b_sfname == ఖాళీ)
	    // empty string
	{
	    ఒకవేళ (BYTE_STRING_విలువ(fname)[0] == NUL)
		బఫరు = బఫరు_new(buf);
	}
	లేకపోతే ఒకవేళ (!fnamecmp(buf->b_ffname, BYTE_STRING_విలువ(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_విలువ(fname)))
	{
	    // either short or long filename matches
	    బఫరు = బఫరు_new(buf);
	}
    }

    MZ_GC_UNREG();
    తిరిగి బఫరు;
}

/*
 * (get-next-buff [buffer])
 */
    స్థిర Scheme_Object *
get_తదుపరి_బఫరు(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)దత్తాంశం;
    buf_T	*buf = get_బఫరు_arg(prim->name, 0, argc, argv)->buf;

    ఒకవేళ (buf->b_తదుపరి == ఖాళీ)
	తిరిగి scheme_false;
    లేకపోతే
	తిరిగి బఫరు_new(buf->b_తదుపరి);
}

/*
 * (get-prev-buff [buffer])
 */
    స్థిర Scheme_Object *
get_prev_బఫరు(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)దత్తాంశం;
    buf_T	*buf = get_బఫరు_arg(prim->name, 0, argc, argv)->buf;

    ఒకవేళ (buf->b_prev == ఖాళీ)
	తిరిగి scheme_false;
    లేకపోతే
	తిరిగి బఫరు_new(buf->b_prev);
}

/*
 * (get-buff-num [buffer])
 */
    స్థిర Scheme_Object *
get_బఫరు_అంకె(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_బఫరు   *buf = get_బఫరు_arg(prim->name, 0, argc, argv);

    తిరిగి scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    స్థిర Scheme_Object *
get_బఫరు_లెక్క(
    శూన్య		*దత్తాంశం వాడని,
    పూర్ణాంకం			argc వాడని,
    Scheme_Object	**argv వాడని)
{
    buf_T   *b;
    పూర్ణాంకం	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    తిరిగి scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    స్థిర Scheme_Object *
get_బఫరు_name(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_బఫరు   *buf = get_బఫరు_arg(prim->name, 0, argc, argv);

    తిరిగి scheme_make_byte_string((అక్షరం *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    స్థిర Scheme_Object *
get_curr_బఫరు(
    శూన్య		*దత్తాంశం వాడని,
    పూర్ణాంకం			argc వాడని,
    Scheme_Object	**argv వాడని)
{
    తిరిగి (Scheme_Object *)get_vim_curr_బఫరు();
}

    స్థిర Scheme_Object *
బఫరు_new(buf_T *buf)
{
    vim_mz_బఫరు *self = ఖాళీ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    ఒకవేళ (buf->b_mzscheme_ref)
	తిరిగి (Scheme_Object *)బఫరు_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, పరిమాణం(vim_mz_బఫరు));
    CLEAR_సూచిక(self);
#నిర్వచితమైతే MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(ఖాళీ);
#లేకపోతే
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#అయితే_ముగింపు
    MZ_GC_CHECK();
    బఫరు_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_బఫరు_type;

    MZ_GC_UNREG();
    తిరిగి (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    స్థిర Scheme_Object *
get_బఫరు_కొలత(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_బఫరు   *buf = get_బఫరు_arg(prim->name, 0, argc, argv);

    తిరిగి scheme_make_integer(buf->buf->b_ml.ml_పంక్తి_లెక్క);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    స్థిర Scheme_Object *
get_బఫరు_పంక్తి(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_బఫరు   *buf;
    పూర్ణాంకం		    linenr;
    char_u	    *పంక్తి;

    buf = get_బఫరు_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    పంక్తి = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    తిరిగి scheme_make_byte_string((అక్షరం *)పంక్తి);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    స్థిర Scheme_Object *
get_బఫరు_పంక్తి_list(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_బఫరు   *buf;
    పూర్ణాంకం		    i, hi, lo, n;
    Scheme_Object   *list = ఖాళీ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, list);
    MZ_GC_REG();

    buf = get_బఫరు_arg(prim->name, 2, argc, argv);
    list = scheme_null;
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    /*
     * Handle some error conditions
     */
    ఒకవేళ (lo < 0)
	lo = 0;

    ఒకవేళ (hi < 0)
	hi = 0;
    ఒకవేళ (hi < lo)
	hi = lo;

    n = hi - lo;

    కొరకు (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (అక్షరం *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    తిరిగి list;
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
    స్థిర Scheme_Object *
సమితి_బఫరు_పంక్తి(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_బఫరు   *buf;
    Scheme_Object   *పంక్తి = ఖాళీ;
    అక్షరం	    *save;
    పూర్ణాంకం		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, పంక్తి);
    MZ_GC_REG();

#నిర్వచితమైతే HAVE_SANDBOX
    sandbox_check();
#అయితే_ముగింపు
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ఒకవేళ (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    పంక్తి = argv[1];
    buf = get_బఫరు_arg(prim->name, 2, argc, argv);

    check_పంక్తి_range(n, buf->buf);

    ఒకవేళ (SCHEME_FALSEP(పంక్తి))
    {
	buf_T	    *savebuf = curbuf;

	curbuf = buf->buf;

	ఒకవేళ (u_savedel((linenr_T)n, 1L) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	లేకపోతే ఒకవేళ (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	ఒకవేళ (buf->buf == curwin->w_బఫరు)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_పంక్తులు_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	తిరిగి scheme_void;
    }
    లేకపోతే
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_గమ్యం_పంక్తి(పంక్తి);

	curbuf = buf->buf;

	ఒకవేళ (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	లేకపోతే ఒకవేళ (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	లేకపోతే
	{
	    vim_free(save);
	    మారింది_బైట్లు((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	ఒకవేళ (buf->buf == curwin->w_బఫరు)
	    check_cursor_నిలువు();

	MZ_GC_UNREG();
	raise_if_error();
	తిరిగి scheme_void;
    }
}

    స్థిర శూన్య
free_array(అక్షరం **array)
{
    అక్షరం **curr = array;
    అయితే (*curr != ఖాళీ)
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
    స్థిర Scheme_Object *
సమితి_బఫరు_పంక్తి_list(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_బఫరు   *buf = ఖాళీ;
    Scheme_Object   *పంక్తి_list = ఖాళీ;
    పూర్ణాంకం		    i, old_len, new_len, hi, lo;
    దీర్ఘ	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, పంక్తి_list);
    MZ_GC_REG();

#నిర్వచితమైతే HAVE_SANDBOX
    sandbox_check();
#అయితే_ముగింపు
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    ఒకవేళ (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    పంక్తి_list = argv[2];
    buf = get_బఫరు_arg(prim->name, 3, argc, argv);
    old_len = hi - lo;
    ఒకవేళ (old_len < 0) // process inverse values wisely
    {
	i = lo;
	lo = hi;
	hi = i;
	old_len = -old_len;
    }
    extra = 0;

    check_పంక్తి_range(lo, buf->buf);	    // inclusive
    check_పంక్తి_range(hi - 1, buf->buf);	    // exclusive

    ఒకవేళ (SCHEME_FALSEP(పంక్తి_list) || SCHEME_NULLP(పంక్తి_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	ఒకవేళ (u_savedel((linenr_T)lo, (దీర్ఘ)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	లేకపోతే
	{
	    కొరకు (i = 0; i < old_len; i++)
		ఒకవేళ (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    ఒకవేళ (buf->buf == curwin->w_బఫరు)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_పంక్తులు_mark((linenr_T)lo, (దీర్ఘ)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	తిరిగి scheme_void;
    }
    లేకపోతే
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_నిడివి(పంక్తి_list);
	MZ_GC_CHECK();
	ఒకవేళ (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	లేకపోతే
	{
	    అక్షరం		**array = ఖాళీ;
	    Scheme_Object   *పంక్తి = ఖాళీ;
	    Scheme_Object   *rest = ఖాళీ;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_లోపల_REG(0, పంక్తి);
	    MZ_GC_VAR_లోపల_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(అక్షరం *, new_len + 1);

	    rest = పంక్తి_list;
	    కొరకు (i = 0; i < new_len; ++i)
	    {
		పంక్తి = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		ఒకవేళ (!SCHEME_STRINGP(పంక్తి))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_గమ్యం_పంక్తి(పంక్తి);
	    }

	    curbuf = buf->buf;

	    ఒకవేళ (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
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
	    కొరకు (i = 0; i < old_len - new_len; ++i)
	    {
		ఒకవేళ (ml_delete((linenr_T)lo) == FAIL)
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
	    కొరకు (i = 0; i < old_len && i < new_len; i++)
		ఒకవేళ (ml_replace((linenr_T)(lo+i), (char_u *)array[i], TRUE) == FAIL)
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
	    అయితే (i < new_len)
	    {
		ఒకవేళ (ml_append((linenr_T)(lo + i - 1),
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
						  (దీర్ఘ)MAXLNUM, (దీర్ఘ)extra);
	మారింది_పంక్తులు((linenr_T)lo, 0, (linenr_T)hi, (దీర్ఘ)extra);

	ఒకవేళ (buf->buf == curwin->w_బఫరు)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	తిరిగి scheme_void;
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
    స్థిర Scheme_Object *
చొప్పించు_బఫరు_పంక్తి_list(శూన్య *దత్తాంశం, పూర్ణాంకం argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)దత్తాంశం;
    vim_mz_బఫరు   *buf = ఖాళీ;
    Scheme_Object   *list = ఖాళీ;
    అక్షరం	    *str = ఖాళీ;
    పూర్ణాంకం		    i, n, కొలత;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, list);
    MZ_GC_REG();

#నిర్వచితమైతే HAVE_SANDBOX
    sandbox_check();
#అయితే_ముగింపు
    /*
     * First of all, we check the type of the supplied MzScheme object.
     * It must be a string or a list, or the call is in error.
     */
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    list = argv[1];

    ఒకవేళ (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_బఫరు_arg(prim->name, 2, argc, argv);

    ఒకవేళ (n != 0)	    // 0 can be used in insert
	check_పంక్తి_range(n, buf->buf);
    ఒకవేళ (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_గమ్యం_పంక్తి(list);
	curbuf = buf->buf;

	ఒకవేళ (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	లేకపోతే ఒకవేళ (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	లేకపోతే
	{
	    vim_free(str);
	    appended_పంక్తులు_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	తిరిగి scheme_void;
    }

    // List
    కొలత = scheme_proper_list_నిడివి(list);
    MZ_GC_CHECK();
    ఒకవేళ (కొలత < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    లేకపోతే
    {
	Scheme_Object   *పంక్తి = ఖాళీ;
	Scheme_Object   *rest = ఖాళీ;
	అక్షరం		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_లోపల_REG(0, పంక్తి);
	MZ_GC_VAR_లోపల_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(అక్షరం *, కొలత + 1);

	rest = list;
	కొరకు (i = 0; i < కొలత; ++i)
	{
	    పంక్తి = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_గమ్యం_పంక్తి(పంక్తి);
	}

	curbuf = buf->buf;

	ఒకవేళ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	లేకపోతే
	{
	    కొరకు (i = 0; i < కొలత; ++i)
		ఒకవేళ (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    ఒకవేళ (i > 0)
		appended_పంక్తులు_mark((linenr_T)n, (దీర్ఘ)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    తిరిగి scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    స్థిర Scheme_Object *
vim_bufferp(శూన్య *దత్తాంశం వాడని, పూర్ణాంకం argc వాడని, Scheme_Object **argv)
{
    ఒకవేళ (SCHEME_VIMBUFFERP(argv[0]))
	తిరిగి scheme_true;
    లేకపోతే
	తిరిగి scheme_false;
}

/*
 * (win? obj)
 */
    స్థిర Scheme_Object *
vim_windowp(శూన్య *దత్తాంశం వాడని, పూర్ణాంకం argc వాడని, Scheme_Object **argv)
{
    ఒకవేళ (SCHEME_VIMWINDOWP(argv[0]))
	తిరిగి scheme_true;
    లేకపోతే
	తిరిగి scheme_false;
}

/*
 * (buff-valid? obj)
 */
    స్థిర Scheme_Object *
vim_బఫరు_validp(శూన్య *దత్తాంశం వాడని, పూర్ణాంకం argc వాడని, Scheme_Object **argv)
{
    ఒకవేళ (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_బఫరు *)argv[0])->buf != INVALID_బఫరు_విలువ)
	తిరిగి scheme_true;
    లేకపోతే
	తిరిగి scheme_false;
}

/*
 * (win-valid? obj)
 */
    స్థిర Scheme_Object *
vim_కిటికీ_validp(శూన్య *దత్తాంశం వాడని, పూర్ణాంకం argc వాడని, Scheme_Object **argv)
{
    ఒకవేళ (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_కిటికీ *)argv[0])->win != INVALID_కిటికీ_విలువ)
	తిరిగి scheme_true;
    లేకపోతే
	తిరిగి scheme_false;
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
    స్థిర అక్షరం *
string_గమ్యం_పంక్తి(Scheme_Object *obj)
{
    అక్షరం	*scheme_str = ఖాళీ;
    అక్షరం	*vim_str = ఖాళీ;
    OUTPUT_LEN_TYPE	len;
    పూర్ణాంకం		i;

    scheme_str = scheme_display_గమ్యం_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    ఒకవేళ (memchr(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    కొరకు (i = 0; i < len; ++i)
    {
	ఒకవేళ (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	లేకపోతే
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    తిరిగి vim_str;
}

#నిర్వచితమైతే FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    స్థిర Scheme_Object *
vim_గమ్యం_mzscheme(typval_T *vim_విలువ)
{
    Scheme_Object	*ఫలితం = ఖాళీ;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_పట్టిక	*సందర్శించిన = ఖాళీ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_లోపల_REG(0, ఫలితం);
    MZ_GC_VAR_లోపల_REG(1, సందర్శించిన);
    MZ_GC_REG();

    సందర్శించిన = scheme_make_hash_పట్టిక(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    ఫలితం = vim_గమ్యం_mzscheme_impl(vim_విలువ, 1, సందర్శించిన);

    MZ_GC_UNREG();
    తిరిగి ఫలితం;
}

    స్థిర Scheme_Object *
vim_గమ్యం_mzscheme_impl(
    typval_T		*vim_విలువ,
    పూర్ణాంకం			లోతు,
    Scheme_Hash_పట్టిక	*సందర్శించిన)
{
    Scheme_Object   *ఫలితం = ఖాళీ;
    పూర్ణాంకం		    new_విలువ_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_లోపల_REG(0, ఫలితం);
    MZ_GC_VAR_లోపల_REG(1, సందర్శించిన);
    MZ_GC_REG();

    // Avoid infinite recursion
    ఒకవేళ (లోతు > 100)
    {
	MZ_GC_UNREG();
	తిరిగి scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    ఫలితం = scheme_hash_get(సందర్శించిన, (Scheme_Object *)vim_విలువ);
    MZ_GC_CHECK();
    ఒకవేళ (ఫలితం != ఖాళీ) // found, do nothing
	new_విలువ_2 = FALSE;
    లేకపోతే ఒకవేళ (vim_విలువ->v_type == VAR_STRING)
    {
	ఫలితం = scheme_make_byte_string((అక్షరం *)vim_విలువ->vval.v_string);
	MZ_GC_CHECK();
    }
    లేకపోతే ఒకవేళ (vim_విలువ->v_type == VAR_సంఖ్య)
    {
	ఫలితం = scheme_make_integer((దీర్ఘ)vim_విలువ->vval.v_సంఖ్య);
	MZ_GC_CHECK();
    }
    లేకపోతే ఒకవేళ (vim_విలువ->v_type == VAR_FLOAT)
    {
	ఫలితం = scheme_make_double((ద్విగుణ)vim_విలువ->vval.v_float);
	MZ_GC_CHECK();
    }
    లేకపోతే ఒకవేళ (vim_విలువ->v_type == VAR_LIST)
    {
	list_T		*list = vim_విలువ->vval.v_list;
	listitem_T	*curr;

	ఒకవేళ (list == ఖాళీ || list->lv_మొదటి == ఖాళీ)
	    ఫలితం = scheme_null;
	లేకపోతే
	{
	    Scheme_Object   *obj = ఖాళీ;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_లోపల_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_చివరి;
	    obj = vim_గమ్యం_mzscheme_impl(&curr->li_tv, లోతు + 1, సందర్శించిన);
	    ఫలితం = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    అయితే (curr != list->lv_మొదటి)
	    {
		curr = curr->li_prev;
		obj = vim_గమ్యం_mzscheme_impl(&curr->li_tv, లోతు + 1, సందర్శించిన);
		ఫలితం = scheme_make_pair(obj, ఫలితం);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    లేకపోతే ఒకవేళ (vim_విలువ->v_type == VAR_DICT)
    {
	Scheme_Object	  *కీలకం = ఖాళీ;
	Scheme_Object	  *obj = ఖాళీ;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_లోపల_REG(0, కీలకం);
	MZ_GC_VAR_లోపల_REG(1, obj);
	MZ_GC_REG();

	ఫలితం = (Scheme_Object *)scheme_make_hash_పట్టిక(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	ఒకవేళ (vim_విలువ->vval.v_dict != ఖాళీ)
	{
	    hashtab_T	*ht = &vim_విలువ->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_వాడిన;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
	    {
		ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_గమ్యం_mzscheme_impl(&di->di_tv, లోతు + 1, సందర్శించిన);
		    కీలకం = scheme_make_byte_string((అక్షరం *)hi->hi_కీలకం);
		    MZ_GC_CHECK();
		    scheme_hash_సమితి((Scheme_Hash_పట్టిక *)ఫలితం, కీలకం, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    లేకపోతే ఒకవేళ (vim_విలువ->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = ఖాళీ;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_లోపల_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((అక్షరం *)vim_విలువ->vval.v_string);
	MZ_GC_CHECK();
	ఫలితం = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(స్థిరరాశి అక్షరం *)BYTE_STRING_విలువ(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    లేకపోతే ఒకవేళ (vim_విలువ->v_type == VAR_PARTIAL)
    {
	ఒకవేళ (vim_విలువ->vval.v_partial == ఖాళీ)
	    ఫలితం = scheme_null;
	లేకపోతే
	{
	    Scheme_Object *funcname = ఖాళీ;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_లోపల_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (అక్షరం *)partial_name(vim_విలువ->vval.v_partial));
	    MZ_GC_CHECK();
	    ఫలితం = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (స్థిరరాశి అక్షరం *)BYTE_STRING_విలువ(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    లేకపోతే ఒకవేళ (vim_విలువ->v_type == VAR_BOOL || vim_విలువ->v_type == VAR_SPECIAL)
    {
	ఒకవేళ (vim_విలువ->vval.v_సంఖ్య <= VVAL_TRUE)
	    ఫలితం = scheme_make_integer((దీర్ఘ)vim_విలువ->vval.v_సంఖ్య);
	లేకపోతే
	    ఫలితం = scheme_null;
	MZ_GC_CHECK();
    }
    లేకపోతే
    {
	ఫలితం = scheme_void;
	new_విలువ_2 = FALSE;
    }
    ఒకవేళ (new_విలువ_2)
    {
	scheme_hash_సమితి(సందర్శించిన, (Scheme_Object *)vim_విలువ, ఫలితం);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    తిరిగి ఫలితం;
}

    స్థిర పూర్ణాంకం
mzscheme_గమ్యం_vim(Scheme_Object *obj, typval_T *tv)
{
    పూర్ణాంకం i, స్థితి_2;
    Scheme_Hash_పట్టిక	*సందర్శించిన = ఖాళీ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_లోపల_REG(0, obj);
    MZ_GC_VAR_లోపల_REG(1, సందర్శించిన);
    MZ_GC_REG();

    సందర్శించిన = scheme_make_hash_పట్టిక(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    స్థితి_2 = mzscheme_గమ్యం_vim_impl(obj, tv, 1, సందర్శించిన);
    కొరకు (i = 0; i < సందర్శించిన->కొలత; ++i)
    {
	// free up remembered objects
	ఒకవేళ (సందర్శించిన->vals[i] != ఖాళీ)
	    free_tv((typval_T *)సందర్శించిన->vals[i]);
    }

    MZ_GC_UNREG();
    తిరిగి స్థితి_2;
}
    స్థిర పూర్ణాంకం
mzscheme_గమ్యం_vim_impl(Scheme_Object *obj, typval_T *tv, పూర్ణాంకం లోతు,
	Scheme_Hash_పట్టిక *సందర్శించిన)
{
    పూర్ణాంకం		స్థితి_2 = OK;
    typval_T	*దొరికింది;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_లోపల_REG(0, obj);
    MZ_GC_VAR_లోపల_REG(1, సందర్శించిన);
    MZ_GC_REG();

    MZ_GC_CHECK();
    ఒకవేళ (లోతు > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_సంఖ్య;
	tv->vval.v_సంఖ్య = 0;
	తిరిగి FAIL;
    }

    దొరికింది = (typval_T *)scheme_hash_get(సందర్శించిన, obj);
    ఒకవేళ (దొరికింది != ఖాళీ)
	copy_tv(దొరికింది, tv);
    లేకపోతే ఒకవేళ (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_సంఖ్య = VVAL_NULL;
    }
    లేకపోతే ఒకవేళ (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_సంఖ్య;
	tv->vval.v_సంఖ్య = SCHEME_INT_VAL(obj);
    }
    లేకపోతే ఒకవేళ (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_సంఖ్య = SCHEME_TRUEP(obj);
    }
    లేకపోతే ఒకవేళ (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    లేకపోతే ఒకవేళ (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_విలువ(obj));
    }
# ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 299
    లేకపోతే ఒకవేళ (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = ఖాళీ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_లోపల_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_గమ్యం_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_విలువ(tmp));
	MZ_GC_UNREG();
    }
# endif
    లేకపోతే ఒకవేళ (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	ఒకవేళ (list == ఖాళీ)
	    స్థితి_2 = FAIL;
	లేకపోతే
	{
	    పూర్ణాంకం		    i;
	    Scheme_Object   *curr = ఖాళీ;
	    Scheme_Object   *cval = ఖాళీ;
	    // temporary var to hold current element of vectors and pairs
	    typval_T	    *v;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_లోపల_REG(0, curr);
	    MZ_GC_VAR_లోపల_REG(1, cval);
	    MZ_GC_REG();

	    tv->v_type = VAR_LIST;
	    tv->vval.v_list = list;
	    ++list->lv_refcount;

	    v = ALLOC_ONE(typval_T);
	    ఒకవేళ (v == ఖాళీ)
		స్థితి_2 = FAIL;
	    లేకపోతే
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *సందర్శించిన_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, సందర్శించిన_tv);
		scheme_hash_సమితి(సందర్శించిన, obj, (Scheme_Object *)సందర్శించిన_tv);

		ఒకవేళ (SCHEME_VECTORP(obj))
		{
		    కొరకు (i = 0; i < SCHEME_VEC_కొలత(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			స్థితి_2 = mzscheme_గమ్యం_vim_impl(cval, v, లోతు + 1, సందర్శించిన);
			ఒకవేళ (స్థితి_2 == FAIL)
			    ఆపు;
			స్థితి_2 = list_append_tv(list, v);
			clear_tv(v);
			ఒకవేళ (స్థితి_2 == FAIL)
			    ఆపు;
		    }
		}
		లేకపోతే ఒకవేళ (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    కొరకు (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			స్థితి_2 = mzscheme_గమ్యం_vim_impl(cval, v, లోతు + 1, సందర్శించిన);
			ఒకవేళ (స్థితి_2 == FAIL)
			    ఆపు;
			స్థితి_2 = list_append_tv(list, v);
			clear_tv(v);
			ఒకవేళ (స్థితి_2 == FAIL)
			    ఆపు;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    ఒకవేళ (స్థితి_2 == OK && !SCHEME_NULLP(curr))
		    {
			స్థితి_2 = mzscheme_గమ్యం_vim_impl(cval, v, లోతు + 1, సందర్శించిన);
			ఒకవేళ (స్థితి_2 == OK)
			{
			    స్థితి_2 = list_append_tv(list, v);
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
    లేకపోతే ఒకవేళ (SCHEME_HASHTP(obj))
    {
	పూర్ణాంకం		i;
	dict_T		*dict;
	Scheme_Object   *కీలకం = ఖాళీ;
	Scheme_Object   *val = ఖాళీ;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_లోపల_REG(0, కీలకం);
	MZ_GC_VAR_లోపల_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	ఒకవేళ (dict == ఖాళీ)
	    స్థితి_2 = FAIL;
	లేకపోతే
	{
	    typval_T    *సందర్శించిన_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, సందర్శించిన_tv);
	    scheme_hash_సమితి(సందర్శించిన, obj, (Scheme_Object *)సందర్శించిన_tv);

	    కొరకు (i = 0; i < ((Scheme_Hash_పట్టిక *)obj)->కొలత; ++i)
	    {
		ఒకవేళ (((Scheme_Hash_పట్టిక *) obj)->vals[i] != ఖాళీ)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *అంశం = dictitem_alloc((char_u *)string_గమ్యం_పంక్తి(
				((Scheme_Hash_పట్టిక *) obj)->కీలకాలు[i]));
		    // convert Scheme val to Vim and add it to the dict
		    ఒకవేళ (mzscheme_గమ్యం_vim_impl(((Scheme_Hash_పట్టిక *) obj)->vals[i],
				    &అంశం->di_tv, లోతు + 1, సందర్శించిన) == FAIL
			    || dict_కూడు(dict, అంశం) == FAIL)
		    {
			dictitem_free(అంశం);
			స్థితి_2 = FAIL;
			ఆపు;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    లేకపోతే
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_గమ్యం_పంక్తి(obj);
    }
    MZ_GC_UNREG();
    తిరిగి స్థితి_2;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    స్థిర Scheme_Object *
vim_funcref(శూన్య *name, పూర్ణాంకం argc, Scheme_Object **argv)
{
    పూర్ణాంకం i;
    typval_T వాదనలు;
    పూర్ణాంకం స్థితి_2 = OK;
    Scheme_Object *ఫలితం = ఖాళీ;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, ఫలితం);
    MZ_GC_REG();

    ఫలితం = scheme_void;
    ఒకవేళ (list == ఖాళీ)
	స్థితి_2 = FAIL;
    లేకపోతే
    {
	వాదనలు.v_type = VAR_LIST;
	వాదనలు.vval.v_list = list;
	++list->lv_refcount;
	కొరకు (i = 0; స్థితి_2 == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    ఒకవేళ (v == ఖాళీ)
		స్థితి_2 = FAIL;
	    లేకపోతే
	    {
		స్థితి_2 = mzscheme_గమ్యం_vim(argv[i], v);
		ఒకవేళ (స్థితి_2 == OK)
		{
		    స్థితి_2 = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	ఒకవేళ (స్థితి_2 == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_విలువ((Scheme_Object *)name), &వాదనలు, &ret);
	    MZ_GC_CHECK();
	    ఫలితం = vim_గమ్యం_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&వాదనలు);
    MZ_GC_UNREG();
    ఒకవేళ (స్థితి_2 != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    లేకపోతే
	raise_if_error();
    తిరిగి ఫలితం;
}

    శూన్య
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = ఖాళీ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_లోపల_REG(0, ret);
    MZ_GC_REG();

    ఒకవేళ (mzscheme_init())
    {
	MZ_GC_UNREG();
	తిరిగి;
    }

    MZ_GC_CHECK();
    ఒకవేళ (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_గమ్యం_vim(ret, rettv);

    MZ_GC_UNREG();
}
#అయితే_ముగింపు

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    స్థిర పూర్ణాంకం
vim_error_check(శూన్య)
{
    తిరిగి (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    స్థిర శూన్య
రిజిస్టరు_vim_exn(శూన్య)
{
    పూర్ణాంకం	nc = 0;
    పూర్ణాంకం i;
    Scheme_Object   *struct_exn = ఖాళీ;
    Scheme_Object   *exn_name = ఖాళీ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_లోపల_REG(0, struct_exn);
    MZ_GC_VAR_లోపల_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_చిహ్నం("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_విలువ("struct:exn");
    MZ_GC_CHECK();

    ఒకవేళ (vim_exn == ఖాళీ)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, ఖాళీ, 0, 0, ఖాళీ, ఖాళీ
#ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 299
		, ఖాళీ
#అయితే_ముగింపు
		);


    {
	Scheme_Object   **tmp = ఖాళీ;
	Scheme_Object   *exn_పేర్లు[5] = {ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ};
	Scheme_Object   *exn_విలువలు[5] = {ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_లోపల_REG(0, exn_పేర్లు, 5);
	MZ_GC_ARRAY_VAR_లోపల_REG(3, exn_విలువలు, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_పేర్లు(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_పేర్లు, tmp, nc * పరిమాణం(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_విలువలు(vim_exn, exn_పేర్లు, nc, 0);
	mch_memmove(exn_విలువలు, tmp, nc * పరిమాణం(Scheme_Object *));
	MZ_GC_CHECK();

	కొరకు (i = 0; i < nc; i++)
	{
	    scheme_కూడు_global_చిహ్నం(exn_పేర్లు[i],
		    exn_విలువలు[i], environment);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
    MZ_GC_UNREG();
}

/*
 * raise exn:vim, may be with additional info string
 */
    శూన్య
raise_vim_exn(స్థిరరాశి అక్షరం *కూడు_info)
{
    అక్షరం	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {ఖాళీ, ఖాళీ};
    Scheme_Object   *exn = ఖాళీ;
    Scheme_Object   *byte_string = ఖాళీ;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_లోపల_REG(0, argv, 2);
    MZ_GC_VAR_లోపల_REG(3, exn);
    MZ_GC_VAR_లోపల_REG(4, byte_string);
    MZ_GC_REG();

    ఒకవేళ (కూడు_info != ఖాళీ)
    {
	అక్షరం		*c_string = ఖాళీ;
	Scheme_Object   *info = ఖాళీ;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_లోపల_REG(0, c_string);
	MZ_GC_VAR_లోపల_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(కూడు_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (పూర్ణాంకం)STRLEN(fmt), 1, &info, ఖాళీ);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_గమ్యం_char_string(byte_string);
	SCHEME_సమితి_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    లేకపోతే
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_గమ్యం_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#లేకపోతే
    argv[1] = scheme_current_continuation_marks(ఖాళీ);
    MZ_GC_CHECK();
#అయితే_ముగింపు

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    శూన్య
raise_if_error(శూన్య)
{
    ఒకవేళ (vim_error_check())
	raise_vim_exn(ఖాళీ);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    స్థిర vim_mz_బఫరు *
get_బఫరు_arg(స్థిరరాశి అక్షరం *fname, పూర్ణాంకం argnum, పూర్ణాంకం argc, Scheme_Object **argv)
{
    vim_mz_బఫరు *b;

    ఒకవేళ (argc < argnum + 1)
	తిరిగి get_vim_curr_బఫరు();
    ఒకవేళ (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_బఫరు *)argv[argnum];
    (శూన్య)get_valid_బఫరు(argv[argnum]);
    తిరిగి b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    స్థిర vim_mz_కిటికీ *
get_కిటికీ_arg(స్థిరరాశి అక్షరం *fname, పూర్ణాంకం argnum, పూర్ణాంకం argc, Scheme_Object **argv)
{
    vim_mz_కిటికీ *w;

    ఒకవేళ (argc < argnum + 1)
	తిరిగి get_vim_curr_కిటికీ();
    w = (vim_mz_కిటికీ *)argv[argnum];
    ఒకవేళ (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (శూన్య)get_valid_కిటికీ(argv[argnum]);
    తిరిగి w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_బఫరు(శూన్య *obj)
{
    buf_T *buf = ((vim_mz_బఫరు *)obj)->buf;

    ఒకవేళ (buf == INVALID_బఫరు_విలువ)
	scheme_signal_error(_("buffer is invalid"));
    తిరిగి buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_కిటికీ(శూన్య *obj)
{
    win_T *win = ((vim_mz_కిటికీ *)obj)->win;
    ఒకవేళ (win == INVALID_కిటికీ_విలువ)
	scheme_signal_error(_("window is invalid"));
    తిరిగి win;
}

    పూర్ణాంకం
mzthreads_allowed(శూన్య)
{
    తిరిగి mz_దారాలు_allow;
}

    స్థిర పూర్ణాంకం
పంక్తి_లోపల_range(linenr_T lnum, buf_T *buf)
{
    తిరిగి (lnum > 0 && lnum <= buf->b_ml.ml_పంక్తి_లెక్క);
}

    స్థిర శూన్య
check_పంక్తి_range(linenr_T lnum, buf_T *buf)
{
    ఒకవేళ (!పంక్తి_లోపల_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    స్థిర శూన్య
mz_fix_cursor(పూర్ణాంకం lo, పూర్ణాంకం hi, పూర్ణాంకం extra)
{
    ఒకవేళ (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	ఒకవేళ (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_నిలువు();
	}
	లేకపోతే ఒకవేళ (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	లేకపోతే
	    check_cursor_నిలువు();
	మారింది_cline_bef_curs();
    }
    invalidate_botline();
}

స్థిర Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_బఫరు_పంక్తి, "get-buff-line", 1, 2},
    {సమితి_బఫరు_పంక్తి, "set-buff-line", 2, 3},
    {get_బఫరు_పంక్తి_list, "get-buff-line-list", 2, 3},
    {get_బఫరు_name, "get-buff-name", 0, 1},
    {get_బఫరు_అంకె, "get-buff-num", 0, 1},
    {get_బఫరు_కొలత, "get-buff-size", 0, 1},
    {సమితి_బఫరు_పంక్తి_list, "set-buff-line-list", 3, 4},
    {చొప్పించు_బఫరు_పంక్తి_list, "insert-buff-line-list", 2, 3},
    {get_curr_బఫరు, "curr-buff", 0, 0},
    {get_బఫరు_లెక్క, "buff-count", 0, 0},
    {get_తదుపరి_బఫరు, "get-next-buff", 0, 1},
    {get_prev_బఫరు, "get-prev-buff", 0, 1},
    {mzscheme_open_బఫరు, "open-buff", 1, 1},
    {get_బఫరు_by_name, "get-buff-by-name", 1, 1},
    {get_బఫరు_by_అంకె, "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_కిటికీ_లెక్క, "win-count", 0, 0},
    {get_కిటికీ_by_అంకె, "get-win-by-num", 1, 1},
    {get_కిటికీ_అంకె, "get-win-num", 0, 1},
    {get_కిటికీ_బఫరు, "get-win-buffer", 0, 1},
    {get_కిటికీ_ఎత్తు, "get-win-height", 0, 1},
    {సమితి_కిటికీ_ఎత్తు, "set-win-height", 1, 2},
    {get_కిటికీ_వెడల్పు, "get-win-width", 0, 1},
    {సమితి_కిటికీ_వెడల్పు, "set-win-width", 1, 2},
    {get_cursor, "get-cursor", 0, 1},
    {సమితి_cursor, "set-cursor", 1, 2},
    {get_కిటికీ_list, "get-win-list", 0, 1},
    /*
     * Vim-related commands
     */
    {vim_command, "command", 1, 1},
    {vim_eval, "eval", 1, 1},
    {get_range_ప్రారంభం, "range-start", 0, 0},
    {get_range_చివర, "range-end", 0, 0},
    {mzscheme_beep, "beep", 0, 0},
    {get_option, "get-option", 1, 2},
    {సమితి_option, "set-option", 1, 2},
    /*
     * small utilities
     */
    {vim_bufferp, "buff?", 1, 1},
    {vim_windowp, "win?", 1, 1},
    {vim_బఫరు_validp, "buff-valid?", 1, 1},
    {vim_కిటికీ_validp, "win-valid?", 1, 1}
};

/*
 * return MzScheme wrapper for curbuf
 */
    స్థిర vim_mz_బఫరు *
get_vim_curr_బఫరు(శూన్య)
{
    ఒకవేళ (curbuf->b_mzscheme_ref == ఖాళీ)
	తిరిగి (vim_mz_బఫరు *)బఫరు_new(curbuf);
    లేకపోతే
	తిరిగి బఫరు_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    స్థిర vim_mz_కిటికీ *
get_vim_curr_కిటికీ(శూన్య)
{
    ఒకవేళ (curwin->w_mzscheme_ref == ఖాళీ)
	తిరిగి (vim_mz_కిటికీ *)కిటికీ_new(curwin);
    లేకపోతే
	తిరిగి కిటికీ_REF(curwin);
}

    స్థిర శూన్య
make_modules(శూన్య)
{
    పూర్ణాంకం		    i;
    Scheme_Env	    *mod = ఖాళీ;
    Scheme_Object   *vimext_చిహ్నం = ఖాళీ;
    Scheme_Object   *closed_prim = ఖాళీ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_లోపల_REG(0, mod);
    MZ_GC_VAR_లోపల_REG(1, vimext_చిహ్నం);
    MZ_GC_VAR_లోపల_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_చిహ్నం = scheme_intern_చిహ్నం("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_చిహ్నం, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(prims); i++)
    {
	Vim_Prim *prim = prims + i;
	closed_prim = scheme_make_closed_prim_w_arity(prim->prim, prim, prim->name,
			    prim->mina, prim->maxa);
	scheme_కూడు_global(prim->name, closed_prim, mod);
	MZ_GC_CHECK();
    }
    scheme_ముగింపు_primitive_module(mod);
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

#నిర్వచితమైతే HAVE_SANDBOX
స్థిర Scheme_Object *M_write = ఖాళీ;
స్థిర Scheme_Object *M_read = ఖాళీ;
స్థిర Scheme_Object *M_execute = ఖాళీ;
స్థిర Scheme_Object *M_delete = ఖాళీ;

    స్థిర శూన్య
sandbox_check(శూన్య)
{
    ఒకవేళ (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    స్థిర Scheme_Object *
sandbox_file_guard(పూర్ణాంకం argc వాడని, Scheme_Object **argv)
{
    ఒకవేళ (!sandbox)
	తిరిగి scheme_void;

    Scheme_Object *requested_access = argv[2];

    ఒకవేళ (M_write == ఖాళీ)
    {
	MZ_రిజిస్టరు_STATIC(M_write);
	M_write = scheme_intern_చిహ్నం("write");
	MZ_GC_CHECK();
    }
    ఒకవేళ (M_read == ఖాళీ)
    {
	MZ_రిజిస్టరు_STATIC(M_read);
	M_read = scheme_intern_చిహ్నం("read");
	MZ_GC_CHECK();
    }
    ఒకవేళ (M_execute == ఖాళీ)
    {
	MZ_రిజిస్టరు_STATIC(M_execute);
	M_execute = scheme_intern_చిహ్నం("execute");
	MZ_GC_CHECK();
    }
    ఒకవేళ (M_delete == ఖాళీ)
    {
	MZ_రిజిస్టరు_STATIC(M_delete);
	M_delete = scheme_intern_చిహ్నం("delete");
	MZ_GC_CHECK();
    }

    అయితే (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *అంశం = SCHEME_CAR(requested_access);
	ఒకవేళ (scheme_eq(అంశం, M_write) || scheme_eq(అంశం, M_read)
		|| scheme_eq(అంశం, M_execute) || scheme_eq(అంశం, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    తిరిగి scheme_void;
}

    స్థిర Scheme_Object *
sandbox_జాలం_guard(పూర్ణాంకం argc వాడని, Scheme_Object **argv వాడని)
{
    తిరిగి scheme_void;
}
#అయితే_ముగింపు

#అయితే_ముగింపు
