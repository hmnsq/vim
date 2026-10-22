/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * evalfunc.c: Builtin functions
 */
#define USING_FLOAT_STUFF

#include "vim.h"

#もし defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

static void f_and(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_BEVAL
static void f_balloon_gettext(typval_T *argvars, typval_T *rettv);
static void f_balloon_show(typval_T *argvars, typval_T *rettv);
# もし defined(FEAT_BEVAL_TERM)
static void f_balloon_split(typval_T *argvars, typval_T *rettv);
# endif
#endif
static void f_base64_encode(typval_T *argvars, typval_T *rettv);
static void f_base64_decode(typval_T *argvars, typval_T *rettv);
static void f_bindtextdomain(typval_T *argvars, typval_T *rettv);
static void f_byte2line(typval_T *argvars, typval_T *rettv);
static void f_call(typval_T *argvars, typval_T *rettv);
static void f_changenr(typval_T *argvars, typval_T *rettv);
static void f_char2nr(typval_T *argvars, typval_T *rettv);
static void f_charcol(typval_T *argvars, typval_T *rettv);
static void f_col(typval_T *argvars, typval_T *rettv);
static void f_confirm(typval_T *argvars, typval_T *rettv);
static void f_copy(typval_T *argvars, typval_T *rettv);
static void f_cursor(typval_T *argsvars, typval_T *rettv);
#ifdef MSWIN
static void f_debugbreak(typval_T *argvars, typval_T *rettv);
#endif
static void f_deepcopy(typval_T *argvars, typval_T *rettv);
static void f_did_filetype(typval_T *argvars, typval_T *rettv);
static void f_echoraw(typval_T *argvars, typval_T *rettv);
static void f_empty(typval_T *argvars, typval_T *rettv);
static void f_environ(typval_T *argvars, typval_T *rettv);
static void f_err_teapot(typval_T *argvars, typval_T *rettv);
static void f_escape(typval_T *argvars, typval_T *rettv);
static void f_eval(typval_T *argvars, typval_T *rettv);
static void f_eventhandler(typval_T *argvars, typval_T *rettv);
static void f_execute(typval_T *argvars, typval_T *rettv);
static void f_exists_compiled(typval_T *argvars, typval_T *rettv);
static void f_expand(typval_T *argvars, typval_T *rettv);
static void f_expandcmd(typval_T *argvars, typval_T *rettv);
static void f_feedkeys(typval_T *argvars, typval_T *rettv);
static void f_fnameescape(typval_T *argvars, typval_T *rettv);
static void f_foreground(typval_T *argvars, typval_T *rettv);
static void f_funcref(typval_T *argvars, typval_T *rettv);
static void f_function(typval_T *argvars, typval_T *rettv);
static void f_garbagecollect(typval_T *argvars, typval_T *rettv);
static void f_get(typval_T *argvars, typval_T *rettv);
static void f_getbgcolor(typval_T *argvars, typval_T *rettv);
static void f_getcellpixels(typval_T *argvars, typval_T *rettv);
static void f_getchangelist(typval_T *argvars, typval_T *rettv);
static void f_getcharpos(typval_T *argvars, typval_T *rettv);
static void f_getcharsearch(typval_T *argvars, typval_T *rettv);
static void f_getcurpos(typval_T *argvars, typval_T *rettv);
static void f_getcursorcharpos(typval_T *argvars, typval_T *rettv);
static void f_getenv(typval_T *argvars, typval_T *rettv);
static void f_getfontname(typval_T *argvars, typval_T *rettv);
static void f_getinfo(typval_T *argvars, typval_T *rettv);
static void f_getjumplist(typval_T *argvars, typval_T *rettv);
static void f_getpid(typval_T *argvars, typval_T *rettv);
static void f_getpos(typval_T *argvars, typval_T *rettv);
static void f_getreg(typval_T *argvars, typval_T *rettv);
static void f_getreginfo(typval_T *argvars, typval_T *rettv);
static void f_getregion(typval_T *argvars, typval_T *rettv);
static void f_getregionpos(typval_T *argvars, typval_T *rettv);
static void f_getregtype(typval_T *argvars, typval_T *rettv);
static void f_gettagstack(typval_T *argvars, typval_T *rettv);
static void f_gettext(typval_T *argvars, typval_T *rettv);
static void f_haslocaldir(typval_T *argvars, typval_T *rettv);
static void f_hlID(typval_T *argvars, typval_T *rettv);
static void f_hlexists(typval_T *argvars, typval_T *rettv);
static void f_hostname(typval_T *argvars, typval_T *rettv);
static void f_id(typval_T *argvars, typval_T *rettv);
static void f_index(typval_T *argvars, typval_T *rettv);
static void f_indexof(typval_T *argvars, typval_T *rettv);
static void f_input(typval_T *argvars, typval_T *rettv);
static void f_inputdialog(typval_T *argvars, typval_T *rettv);
static void f_inputlist(typval_T *argvars, typval_T *rettv);
static void f_inputrestore(typval_T *argvars, typval_T *rettv);
static void f_inputsave(typval_T *argvars, typval_T *rettv);
static void f_inputsecret(typval_T *argvars, typval_T *rettv);
static void f_interrupt(typval_T *argvars, typval_T *rettv);
static void f_invert(typval_T *argvars, typval_T *rettv);
static void f_islocked(typval_T *argvars, typval_T *rettv);
static void f_items(typval_T *argvars, typval_T *rettv);
static void f_keytrans(typval_T *argvars, typval_T *rettv);
static void f_last_buffer_nr(typval_T *argvars, typval_T *rettv);
static void f_libcall(typval_T *argvars, typval_T *rettv);
static void f_libcallnr(typval_T *argvars, typval_T *rettv);
static void f_line(typval_T *argvars, typval_T *rettv);
static void f_line2byte(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_LUA
static void f_luaeval(typval_T *argvars, typval_T *rettv);
#endif
static void f_match(typval_T *argvars, typval_T *rettv);
static void f_matchbufline(typval_T *argvars, typval_T *rettv);
static void f_matchend(typval_T *argvars, typval_T *rettv);
static void f_matchlist(typval_T *argvars, typval_T *rettv);
static void f_matchstr(typval_T *argvars, typval_T *rettv);
static void f_matchstrlist(typval_T *argvars, typval_T *rettv);
static void f_matchstrpos(typval_T *argvars, typval_T *rettv);
static void f_max(typval_T *argvars, typval_T *rettv);
static void f_min(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_MZSCHEME
static void f_mzeval(typval_T *argvars, typval_T *rettv);
#endif
static void f_nextnonblank(typval_T *argvars, typval_T *rettv);
static void f_ngettext(typval_T *argvars, typval_T *rettv);
static void f_nr2char(typval_T *argvars, typval_T *rettv);
static void f_or(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PERL
static void f_perleval(typval_T *argvars, typval_T *rettv);
#endif
static void f_prevnonblank(typval_T *argvars, typval_T *rettv);
static void f_printf(typval_T *argvars, typval_T *rettv);
static void f_pum_getpos(typval_T *argvars, typval_T *rettv);
static void f_pumvisible(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PYTHON3
static void f_py3eval(typval_T *argvars, typval_T *rettv);
#endif
#ifdef FEAT_PYTHON
static void f_pyeval(typval_T *argvars, typval_T *rettv);
#endif
#もし defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
static void f_pyxeval(typval_T *argvars, typval_T *rettv);
#endif
static void f_test_srand_seed(typval_T *argvars, typval_T *rettv);
static void f_rand(typval_T *argvars, typval_T *rettv);
static void f_range(typval_T *argvars, typval_T *rettv);
static void f_reg_executing(typval_T *argvars, typval_T *rettv);
static void f_reg_recording(typval_T *argvars, typval_T *rettv);
static void f_rename(typval_T *argvars, typval_T *rettv);
static void f_repeat(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_RUBY
static void f_rubyeval(typval_T *argvars, typval_T *rettv);
#endif
static void f_screenattr(typval_T *argvars, typval_T *rettv);
static void f_screenchar(typval_T *argvars, typval_T *rettv);
static void f_screenchars(typval_T *argvars, typval_T *rettv);
static void f_screencol(typval_T *argvars, typval_T *rettv);
static void f_screenrow(typval_T *argvars, typval_T *rettv);
static void f_screenstring(typval_T *argvars, typval_T *rettv);
static void f_search(typval_T *argvars, typval_T *rettv);
static void f_searchdecl(typval_T *argvars, typval_T *rettv);
static void f_searchpair(typval_T *argvars, typval_T *rettv);
static void f_searchpairpos(typval_T *argvars, typval_T *rettv);
static void f_searchpos(typval_T *argvars, typval_T *rettv);
static void f_setcharpos(typval_T *argvars, typval_T *rettv);
static void f_setcharsearch(typval_T *argvars, typval_T *rettv);
static void f_setcursorcharpos(typval_T *argvars, typval_T *rettv);
static void f_setenv(typval_T *argvars, typval_T *rettv);
static void f_setfperm(typval_T *argvars, typval_T *rettv);
static void f_setpos(typval_T *argvars, typval_T *rettv);
static void f_setreg(typval_T *argvars, typval_T *rettv);
static void f_settagstack(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_CRYPT
static void f_sha256(typval_T *argvars, typval_T *rettv);
#endif
static void f_shellescape(typval_T *argvars, typval_T *rettv);
static void f_shiftwidth(typval_T *argvars, typval_T *rettv);
static void f_soundfold(typval_T *argvars, typval_T *rettv);
static void f_spellbadword(typval_T *argvars, typval_T *rettv);
static void f_spellsuggest(typval_T *argvars, typval_T *rettv);
static void f_split(typval_T *argvars, typval_T *rettv);
static void f_srand(typval_T *argvars, typval_T *rettv);
static void f_submatch(typval_T *argvars, typval_T *rettv);
static void f_substitute(typval_T *argvars, typval_T *rettv);
static void f_swapfilelist(typval_T *argvars, typval_T *rettv);
static void f_swapinfo(typval_T *argvars, typval_T *rettv);
static void f_swapname(typval_T *argvars, typval_T *rettv);
static void f_synID(typval_T *argvars, typval_T *rettv);
static void f_synIDattr(typval_T *argvars, typval_T *rettv);
static void f_synIDtrans(typval_T *argvars, typval_T *rettv);
static void f_synstack(typval_T *argvars, typval_T *rettv);
static void f_synconcealed(typval_T *argvars, typval_T *rettv);
static void f_tabpagebuflist(typval_T *argvars, typval_T *rettv);
static void f_taglist(typval_T *argvars, typval_T *rettv);
static void f_tagfiles(typval_T *argvars, typval_T *rettv);
static void f_type(typval_T *argvars, typval_T *rettv);
static void f_virtcol(typval_T *argvars, typval_T *rettv);
static void f_visualmode(typval_T *argvars, typval_T *rettv);
static void f_wildmenumode(typval_T *argvars, typval_T *rettv);
static void f_windowsversion(typval_T *argvars, typval_T *rettv);
static void f_wordcount(typval_T *argvars, typval_T *rettv);
static void f_xor(typval_T *argvars, typval_T *rettv);


/*
 * Functions that check the argument type of a builtin function.
 * Each function returns FAIL and gives an error message if the type is wrong.
 */

// Context passed to an arg_ function.
typedef struct {
    整数		arg_count;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    整数		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
typedef 整数 (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    static 整数
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    戻る need_type(actual, expected, 0,
	    context->arg_idx - context->arg_count, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    static 整数
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    もし (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_count, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	戻る FAIL;
    戻る arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    整数
arg_type_modifiable(type_T *type, 整数 arg_idx)
{
    char *tofree;

    もし ((type->tt_flags & TTFLAG_CONST) == 0)
	戻る OK;
    semsg(_(e_argument_nr_trying_to_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    戻る FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    static 整数
arg_any(type_T *type UNUSED,
	type_T *decl_type UNUSED,
	argcontext_T *context UNUSED)
{
    戻る OK;
}

/*
 * Check "type" is a float or a number.
 */
    static 整数
arg_float_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_NUMBER
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_number, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a number.
 */
    static 整数
arg_number(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    戻る check_arg_type(&t_number, type, context);
}

/*
 * Check "type" is an object.
 */
    static 整数
arg_object(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    static 整数
arg_dict_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    戻る check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    static 整数
arg_list_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    戻る check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    static 整数
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    戻る check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    static 整数
arg_list_number(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    戻る check_arg_type(&t_list_number, type, context);
}

/*
 * Check "type" is a list of strings.
 */
    static 整数
arg_list_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    戻る check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    static 整数
arg_tuple_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    戻る check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    static 整数
arg_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    戻る check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    static 整数
arg_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    戻る check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    static 整数
arg_bool(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    戻る check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    static 整数
arg_bool_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_NUMBER
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_number, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    static 整数
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	戻る OK;
    戻る check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    static 整数
arg_list_or_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    static 整数
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    もし (arg_list_or_blob(type, decl_type, context) == FAIL)
	戻る FAIL;
    戻る arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    static 整数
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    戻る FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    static 整数
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a string or a number
 */
    static 整数
arg_string_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_NUMBER
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    static 整数
arg_buffer(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_NUMBER
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    static 整数
arg_buffer_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_NUMBER
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    static 整数
arg_lnum(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_NUMBER
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    static 整数
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	戻る OK;
    もし (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	戻る FAIL;
    }
    もし (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	戻る OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    static 整数
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    static 整数
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    static 整数
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    static 整数
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    static 整数
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    もし (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    static 整数
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    もし (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	戻る FAIL;
    戻る arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    static 整数
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    static 整数
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    もし (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	戻る FAIL;
    戻る arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    static 整数
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	戻る OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr),
						 context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    static 整数
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = NULL;
    type_T *(args[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, NULL, NULL, args};

    もし (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	もし (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	否则
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    否则 もし (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    否则 もし (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_number;

    args[0] = NULL;
    args[1] = &t_unknown;
    もし (type->tt_argcount != -1)
    {
	もし (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_number_of_arguments));
	    戻る FAIL;
	}
	もし (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	否则
	{
	    もし (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		args[0] = &t_number;
	    否则 もし (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		args[0] = &t_string;
	    もし (args[0] != NULL)
		args[1] = expected_member;
	}
    }

    もし (!type_any_or_unknown(type->tt_member) || args[0] != NULL)
    {
	where_T where = WHERE_INIT;

	もし (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == NULL
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	否则 もし (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	否则 // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	もし (args[0] == NULL)
	    args[0] = &t_unknown;
	もし (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_index = 2;
	where.wt_kind = WT_ARGUMENT;
	戻る check_type(&t_func_exp, type, TRUE, where);
    }
    戻る OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    static 整数
arg_filter_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	戻る OK;

    もし (type->tt_type == VAR_FUNC)
	戻る check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    戻る FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    static 整数
arg_map_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	戻る OK;

    もし (type->tt_type == VAR_FUNC)
	戻る check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    戻る FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    static 整数
arg_foreach_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	戻る OK;

    もし (type->tt_type == VAR_FUNC)
	戻る check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    戻る FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    static 整数
arg_sort_how(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	戻る OK;

    もし (type->tt_type == VAR_FUNC)
    {
	type_T *(args[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_number, NULL, args};

	もし (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    args[0] = context->arg_types[0].type_curr->tt_member;
	否则
	    args[0] = &t_unknown;
	もし (!type_any_or_unknown(type->tt_member) || args[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    args[1] = args[0];
	    もし (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_index = 2;
	    where.wt_kind = WT_ARGUMENT;
	    戻る check_type(&t_func_exp, type, TRUE, where);
	}

	戻る OK;
    }
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    戻る FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    static 整数
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_NUMBER
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    static 整数
varargs_class(type_T *type UNUSED,
	      type_T *decl_type UNUSED,
	      argcontext_T *context)
{
    ために (整数 i = context->arg_idx; i < context->arg_count; ++i)
    {
	type2_T *types = &context->arg_types[i];
	もし (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    戻る FAIL;
	}
    }
    戻る OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    static 整数
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    static 整数
arg_reverse(type_T *type, type_T *decl_type, argcontext_T *context)
{
    もし (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	戻る FAIL;
    戻る arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    static 整数
arg_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    戻る check_arg_type(&t_job, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    static 整数
arg_chan_or_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_JOB
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    static 整数
arg_same_as_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_decl;

    戻る check_arg_type(prev_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    static 整数
arg_same_struct_as_prev(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;

    もし (prev_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	戻る check_arg_type(prev_type, type, context);
    戻る OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    static 整数
arg_item_of_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    もし (prev_type->tt_type == VAR_LIST)
	expected = prev_type->tt_member;
    否则 もし (prev_type->tt_type == VAR_BLOB)
	expected = &t_number;
    否则
	// probably VAR_ANY, can't check
	戻る OK;

    戻る check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    static 整数
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_NUMBER
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    static 整数
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	戻る OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    static 整数
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 2].type_curr;

    もし (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	戻る arg_number(type, decl_type, context);
    もし (first_type->tt_type == VAR_DICT)
	戻る arg_string(type, decl_type, context);
    戻る OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    static 整数
arg_get1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	戻る OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    static 整数
arg_len1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_NUMBER
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	戻る OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    static 整数
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 1].type_curr;

    もし (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	戻る arg_number(type, decl_type, context);
    もし (first_type->tt_type == VAR_DICT)
	戻る arg_string_or_nr(type, decl_type, context);
    戻る OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    static 整数
arg_repeat1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_NUMBER
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	戻る OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    static 整数
arg_slice1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	戻る OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    static 整数
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    もし (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	戻る OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr),
							 context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    static 整数
arg_cursor1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    もし (type->tt_type == VAR_NUMBER
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	戻る OK;

    arg_type_mismatch(&t_number, type, context->arg_idx + 1);
    戻る FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
static argcheck_T arg1_blob[] = {arg_blob};
static argcheck_T arg1_bool[] = {arg_bool};
static argcheck_T arg1_buffer[] = {arg_buffer};
static argcheck_T arg1_buffer_or_dict_any[] = {arg_buffer_or_dict_any};
static argcheck_T arg1_chan_or_job[] = {arg_chan_or_job};
static argcheck_T arg1_dict_any[] = {arg_dict_any};
static argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
static argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
static argcheck_T arg1_job[] = {arg_job};
static argcheck_T arg1_list_any[] = {arg_list_any};
static argcheck_T arg1_tuple_any[] = {arg_tuple_any};
static argcheck_T arg1_list_number[] = {arg_list_number};
static argcheck_T arg1_reverse[] = {arg_reverse};
static argcheck_T arg1_list_or_tuple_or_dict[] = {arg_list_or_tuple_or_dict};
static argcheck_T arg1_list_string[] = {arg_list_string};
static argcheck_T arg1_list_tuple_dict_blob_or_string[] = {arg_list_tuple_dict_blob_or_string};
static argcheck_T arg1_lnum[] = {arg_lnum};
static argcheck_T arg1_number[] = {arg_number};
static argcheck_T arg1_string[] = {arg_string};
static argcheck_T arg1_string_or_list_any[] = {arg_string_or_list_any};
static argcheck_T arg1_string_or_list_string[] = {arg_string_or_list_string};
static argcheck_T arg1_string_or_nr[] = {arg_string_or_nr};
static argcheck_T arg1_string_or_blob[] = {arg_string_or_blob};
static argcheck_T arg2_buffer_any[] = {arg_buffer, arg_any};
static argcheck_T arg2_buffer_bool[] = {arg_buffer, arg_bool};
static argcheck_T arg2_buffer_list_any[] = {arg_buffer, arg_list_any};
static argcheck_T arg2_buffer_lnum[] = {arg_buffer, arg_lnum};
static argcheck_T arg2_buffer_number[] = {arg_buffer, arg_number};
static argcheck_T arg2_buffer_string[] = {arg_buffer, arg_string};
static argcheck_T arg2_chan_or_job_dict[] = {arg_chan_or_job, arg_dict_any};
static argcheck_T arg2_chan_or_job_string[] = {arg_chan_or_job, arg_string};
static argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
static argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
static argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
static argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
static argcheck_T arg2_job_dict[] = {arg_job, arg_dict_any};
static argcheck_T arg2_job_string_or_number[] = {arg_job, arg_string_or_nr};
static argcheck_T arg2_list_any_number[] = {arg_list_any, arg_number};
static argcheck_T arg2_list_number[] = {arg_list_number, arg_list_number};
static argcheck_T arg2_list_number_bool[] = {arg_list_number, arg_bool};
static argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
static argcheck_T arg2_listblobmod_item[] = {arg_list_or_blob_mod, arg_item_of_prev};
static argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
static argcheck_T arg2_lnum_number[] = {arg_lnum, arg_number};
static argcheck_T arg2_number[] = {arg_number, arg_number};
static argcheck_T arg2_number_any[] = {arg_number, arg_any};
static argcheck_T arg2_number_bool[] = {arg_number, arg_bool};
static argcheck_T arg2_number_dict_any[] = {arg_number, arg_dict_any};
static argcheck_T arg2_number_list[] = {arg_number, arg_list_any};
static argcheck_T arg2_number_string[] = {arg_number, arg_string};
static argcheck_T arg2_number_buffer[] = {arg_number, arg_buffer};
static argcheck_T arg2_number_string_or_list[] = {arg_number, arg_string_or_list_any};
static argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
static argcheck_T arg2_string[] = {arg_string, arg_string};
static argcheck_T arg2_string_any[] = {arg_string, arg_any};
static argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
static argcheck_T arg2_string_chan_or_job[] = {arg_string, arg_chan_or_job};
static argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
static argcheck_T arg2_string_list_number[] = {arg_string, arg_list_number};
static argcheck_T arg2_string_number[] = {arg_string, arg_number};
static argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
static argcheck_T arg2_string_or_list_number[] = {arg_string_or_list_any, arg_number};
static argcheck_T arg2_string_string_or_number[] = {arg_string, arg_string_or_nr};
static argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
static argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
static argcheck_T arg3_any_buffer_bool_or_dict[] = {
			      arg_any, arg_buffer, arg_bool_or_dict_any};
static argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
static argcheck_T arg3_buffer_lnum_lnum[] = {arg_buffer, arg_lnum, arg_lnum};
static argcheck_T arg3_buffer_number_number[] = {arg_buffer, arg_number, arg_number};
static argcheck_T arg3_buffer_string_any[] = {arg_buffer, arg_string, arg_any};
static argcheck_T arg3_buffer_string_dict[] = {arg_buffer, arg_string, arg_dict_any};
static argcheck_T arg3_dict_number_number[] = {arg_dict_any, arg_number, arg_number};
static argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
static argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
static argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
static argcheck_T arg3_lnum_number_bool[] = {arg_lnum, arg_number, arg_bool};
static argcheck_T arg3_number[] = {arg_number, arg_number, arg_number};
static argcheck_T arg3_number_any_dict[] = {arg_number, arg_any, arg_dict_any};
static argcheck_T arg3_number_number_dict[] = {arg_number, arg_number, arg_dict_any};
static argcheck_T arg3_number_string_any[] = {arg_number, arg_string, arg_any};
static argcheck_T arg3_number_string_buffer[] = {arg_number, arg_string, arg_buffer};
static argcheck_T arg3_number_string_string[] = {arg_number, arg_string, arg_string};
static argcheck_T arg3_string[] = {arg_string, arg_string, arg_string};
static argcheck_T arg3_string_any_dict[] = {arg_string, arg_any, arg_dict_any};
static argcheck_T arg3_string_any_string[] = {arg_string, arg_any, arg_string};
static argcheck_T arg3_string_bool_bool[] = {arg_string, arg_bool, arg_bool};
static argcheck_T arg3_string_number_bool[] = {arg_string, arg_number, arg_bool};
static argcheck_T arg3_string_number_number[] = {arg_string, arg_number, arg_number};
static argcheck_T arg3_string_or_dict_bool_dict[] = {arg_string_or_dict_any, arg_bool, arg_dict_any};
static argcheck_T arg3_string_or_list_bool_number[] = {arg_string_or_list_any, arg_bool, arg_number};
static argcheck_T arg3_string_string_bool[] = {arg_string, arg_string, arg_bool};
static argcheck_T arg3_string_string_dict[] = {arg_string, arg_string, arg_dict_any};
static argcheck_T arg3_string_string_number[] = {arg_string, arg_string, arg_number};
static argcheck_T arg4_number_number_string_any[] = {arg_number, arg_number, arg_string, arg_any};
static argcheck_T arg4_string_string_any_string[] = {arg_string, arg_string, arg_any, arg_string};
static argcheck_T arg4_string_string_number_string[] = {arg_string, arg_string, arg_number, arg_string};
static argcheck_T arg4_string_number_bool_bool[] = {arg_string, arg_number, arg_bool, arg_bool};
/* Function specific argument types (not covered by the above) */
static argcheck_T arg15_assert_fails[] = {arg_string_or_nr, arg_string_or_list_any, arg_any, arg_number, arg_string};
static argcheck_T arg34_assert_inrange[] = {arg_float_or_nr, arg_float_or_nr, arg_float_or_nr, arg_string};
static argcheck_T arg4_browse[] = {arg_bool, arg_string, arg_string, arg_string};
static argcheck_T arg23_chanexpr[] = {arg_chan_or_job, arg_any, arg_dict_any};
static argcheck_T arg23_chanraw[] = {arg_chan_or_job, arg_string_or_blob, arg_dict_any};
static argcheck_T arg24_count[] = {arg_string_list_tuple_or_dict, arg_any, arg_bool, arg_number};
static argcheck_T arg13_cursor[] = {arg_cursor1, arg_number, arg_number};
static argcheck_T arg12_deepcopy[] = {arg_any, arg_bool};
static argcheck_T arg12_execute[] = {arg_string_or_list_string, arg_string};
static argcheck_T arg12_getchar[] = {arg_bool_or_nr, arg_dict_any};
static argcheck_T arg23_extend[] = {arg_list_or_dict_or_blob_mod, arg_same_as_prev, arg_extend3};
static argcheck_T arg23_extendnew[] = {arg_list_or_dict_or_blob, arg_same_struct_as_prev, arg_extend3};
static argcheck_T arg23_get[] = {arg_get1, arg_string_or_nr, arg_any};
static argcheck_T arg14_glob[] = {arg_string, arg_bool, arg_bool, arg_bool};
static argcheck_T arg25_globpath[] = {arg_string, arg_string, arg_bool, arg_bool, arg_bool};
static argcheck_T arg24_index[] = {arg_list_or_tuple_or_blob, arg_item_of_prev, arg_number, arg_bool};
static argcheck_T arg23_index[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
static argcheck_T arg23_insert[] = {arg_list_or_blob, arg_item_of_prev, arg_number};
static argcheck_T arg1_len[] = {arg_len1};
static argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
static argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
static argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
static argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
static argcheck_T arg2_instanceof[] = {arg_object, varargs_class, NULL };
static argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
static argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
static argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_number, arg_number, arg_dict_any};
static argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_number, arg_number, arg_dict_any};
static argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
static argcheck_T arg45_matchbufline[] = {arg_buffer, arg_string, arg_lnum, arg_lnum, arg_dict_any};
static argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
static argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
static argcheck_T arg24_remote_expr[] = {arg_string, arg_string, arg_string, arg_number};
static argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_number};
static argcheck_T arg2_repeat[] = {arg_repeat1, arg_number};
static argcheck_T arg15_search[] = {arg_string, arg_string, arg_number, arg_number, arg_string_or_func};
static argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_number, arg_number};
static argcheck_T arg3_setbufline[] = {arg_buffer, arg_lnum, arg_str_or_nr_or_list};
static argcheck_T arg2_setline[] = {arg_lnum, arg_any};
static argcheck_T arg24_setloclist[] = {arg_number, arg_list_any, arg_string, arg_dict_any};
static argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
static argcheck_T arg23_settagstack[] = {arg_number, arg_dict_any, arg_string};
static argcheck_T arg02_sign_getplaced[] = {arg_buffer, arg_dict_any};
static argcheck_T arg45_sign_place[] = {arg_number, arg_string, arg_string, arg_buffer, arg_dict_any};
static argcheck_T arg23_slice[] = {arg_slice1, arg_number, arg_number};
static argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_sort_how, arg_dict_any};
static argcheck_T arg24_strpart[] = {arg_string, arg_number, arg_number, arg_bool};
static argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
static argcheck_T arg23_win_execute[] = {arg_number, arg_string_or_list_string, arg_string};
static argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
static argcheck_T arg24_match_func[] = {arg_string_or_list_any, arg_string, arg_number, arg_number};

// Can be used by functions called through "f_retfunc" to create new types.
static garray_T *current_type_gap = NULL;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    static type_T *
ret_void(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_void;
}
    static type_T *
ret_any(整数	argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_any;
}
    static type_T *
ret_bool(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_bool;
}
    static type_T *
ret_number_bool(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_number_bool;
}
    static type_T *
ret_number(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_number;
}
    static type_T *
ret_float(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_float;
}
    static type_T *
ret_string(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_string;
}
    static type_T *
ret_list_any(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_list_any;
}
    static type_T *
ret_list_number(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    戻る &t_list_number;
}
    static type_T *
ret_list_string(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    戻る &t_list_string;
}
    static type_T *
ret_list_dict_any(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    戻る &t_list_dict_any;
}
    static type_T *
ret_list_items(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    戻る &t_list_list_any;
}
    static type_T *
ret_list_list_any(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    戻る &t_list_list_any;
}
    static type_T *
ret_list_string_items(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    戻る &t_list_list_string;
}
    static type_T *
ret_list_regionpos(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    戻る &t_list_list_list_number;
}
    static type_T *
ret_tuple_any(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_tuple_any;
}
    static type_T *
ret_dict_any(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_dict_any;
}
    static type_T *
ret_job_info(整数 argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    もし (argcount == 0)
    {
	*decl_type = &t_list_any;
	戻る &t_list_job;
    }
    戻る &t_dict_any;
}
    static type_T *
ret_dict_number(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_dict_number;
}
    static type_T *
ret_dict_string(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_dict_string;
}
    static type_T *
ret_blob(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_blob;
}
    static type_T *
ret_func_any(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_func_any;
}
    static type_T *
ret_func_unknown(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_func_unknown;
}
    static type_T *
ret_channel(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_channel;
}
    static type_T *
ret_job(整数 argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    戻る &t_job;
}
    static type_T *
ret_first_arg(整数 argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    もし (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	戻る argtypes[0].type_curr;
    }
    戻る &t_void;
}
    static type_T *
ret_slice(整数 argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    もし (argcount > 0)
    {
	もし (argtypes[0].type_decl != NULL)
	{
	    switch (argtypes[0].type_decl->tt_type)
	    {
		case VAR_STRING: *decl_type = &t_string; break;
		case VAR_BLOB: *decl_type = &t_blob; break;
		case VAR_LIST: *decl_type = &t_list_any; break;
		case VAR_TUPLE: *decl_type = &t_tuple_any; break;
		default: break;
	    }
	}
	戻る argtypes[0].type_curr;
    }
    戻る &t_void;
}
    static type_T *
ret_copy(整数 argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    もし (argcount > 0)
    {
	もし (argtypes[0].type_decl != NULL)
	{
	    もし (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    否则 もし (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    否则
		*decl_type = argtypes[0].type_decl;
	}
	もし (argtypes[0].type_curr != NULL)
	{
	    もし (argtypes[0].type_curr->tt_type == VAR_LIST)
		戻る &t_list_any;
	    否则 もし (argtypes[0].type_curr->tt_type == VAR_DICT)
		戻る &t_dict_any;
	}
	戻る argtypes[0].type_curr;
    }
    戻る &t_void;
}
    static type_T *
ret_extend(整数 argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    もし (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	// if the second argument has a different current type then the current
	// type is "any"
	もし (argcount > 1 && !equal_type(argtypes[0].type_curr,
						     argtypes[1].type_curr, 0))
	{
	    もし (argtypes[0].type_curr->tt_type == VAR_LIST)
		戻る &t_list_any;
	    もし (argtypes[0].type_curr->tt_type == VAR_DICT)
		戻る &t_dict_any;
	    もし (argtypes[0].type_curr->tt_type == VAR_BLOB)
		戻る &t_blob;
	}
	戻る argtypes[0].type_curr;
    }
    戻る &t_void;
}
    static type_T *
ret_repeat(整数 argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    もし (argcount == 0)
	戻る &t_any;
    もし (argtypes[0].type_curr == &t_number)
	戻る &t_string;
    戻る argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    static type_T *
ret_first_cont(整数 argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    もし (argcount > 0)
    {
	もし (argtypes[0].type_curr->tt_type == VAR_LIST)
	    戻る &t_list_any;
	もし (argtypes[0].type_curr->tt_type == VAR_DICT)
	    戻る &t_dict_any;
	もし (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    戻る argtypes[0].type_curr;
    }
    戻る &t_any;
}
// for get(): the type of the item when a missing item has the same type
    static type_T *
ret_get(整数 argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    もし (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*item = NULL;

	もし (t->tt_type == VAR_BLOB)
	    item = &t_number;
	否则 もし (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    item = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	もし (item != NULL && (argcount == 3
			? equal_type(item, argtypes[2].type_curr, 0)
			: item->tt_type == VAR_NUMBER))
	{
	    もし (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    戻る item;
	}
    }
    戻る &t_any;
}
// for getline()
    static type_T *
ret_getline(整数 argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    もし (argcount == 1)
	戻る &t_string;
    *decl_type = &t_list_any;
    戻る &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    static type_T *
ret_string_or_any_1(整数 argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    もし (argcount < 1)
	戻る &t_string;
    戻る &t_any;
}
    static type_T *
ret_string_or_any_2(整数 argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    もし (argcount < 2)
	戻る &t_string;
    戻る &t_any;
}
    static type_T *
ret_string_or_any_3(整数 argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    もし (argcount < 3)
	戻る &t_string;
    戻る &t_any;
}
    static type_T *
ret_string_or_any_4(整数 argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    もし (argcount < 4)
	戻る &t_string;
    戻る &t_any;
}
// for abs()
    static type_T *
ret_abs(整数 argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    もし (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	戻る &t_float;
    戻る &t_number;
}
// for sign_define()
    static type_T *
ret_sign_define(整数 argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    もし (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	戻る &t_list_number;
    戻る &t_number;
}
// for values(): list of member of first argument
    static type_T *
ret_list_member(整数 argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    もし (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	もし (current_type_gap != NULL
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	否则
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	もし (current_type_gap != NULL
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    戻る get_list_type(t->tt_member, current_type_gap);
    }
    戻る &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    static type_T *
ret_list_or_dict_0(整数 argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    もし (argcount > 0)
	戻る &t_dict_any;
    *decl_type = &t_list_any;
    戻る &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    static type_T *
ret_list_or_dict_1(整数 argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    もし (argcount > 1)
	戻る &t_dict_any;
    *decl_type = &t_list_any;
    戻る &t_list_dict_any;
}

    static type_T *
ret_argv(整数 argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // argv() returns list of strings
    もし (argcount == 0)
    {
	*decl_type = &t_list_any;
	戻る &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    戻る &t_any;
}

    static type_T *
ret_remove(整数 argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    もし (argcount > 0)
    {
	もし (argtypes[0].type_curr->tt_type == VAR_LIST
		|| argtypes[0].type_curr->tt_type == VAR_DICT)
	{
	    もし (argcount == 3)
	    {
		*decl_type = argtypes[0].type_decl;
		戻る argtypes[0].type_curr;
	    }
	    もし (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    戻る argtypes[0].type_curr->tt_member;
	}
	もし (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    戻る &t_number;
    }
    戻る &t_any;
}

    static type_T *
ret_max_min(整数 argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    もし (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	もし (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	否则
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	もし (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	否则
	    t = &t_any;
	戻る t;
    }
    戻る &t_any;
}

    static type_T *
ret_getreg(整数 argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    もし (argcount == 3)
    {
	*decl_type = &t_list_any;
	戻る &t_list_string;
    }
    戻る &t_string;
}

    static type_T *
ret_virtcol(整数 argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    もし (argcount > 1)
    {
	*decl_type = &t_list_any;
	戻る &t_list_number;
    }
    戻る &t_number;
}

    static type_T *
ret_maparg(整数 argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    // Assume that if the fourth argument is passed it's non-zero
    もし (argcount == 4)
	戻る &t_dict_any;
    戻る &t_string;
}

/*
 * Array with names and number of arguments of all internal functions
 * MUST BE KEPT SORTED IN strcmp() ORDER FOR BINARY SEARCH!
 *
 * The builtin function may be varargs. In that case
 *	- f_max_argc == VARGS
 *	- For varargs, f_argcheck must be NULL terminated. The last non-null
 *	  entry in f_argcheck should validate all the remaining args.
 */
typedef struct
{
    char	*f_name;	// function name
    char	f_min_argc;	// minimal number of arguments
    char	f_max_argc;	// maximal number of arguments
    char	f_argtype;	// for method: FEARG_ values; bits FE_
    argcheck_T	*f_argcheck;	// list of functions to check argument types;
				// use "arg_any" (not NULL) to accept an
				// argument of any type
    char	*f_argnames;	// names of the arguments as in the help,
				// separated by ", "; an argument without a
				// name is empty, NULL when there is none
    type_T	*(*f_retfunc)(整数 argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    void	(*f_func)(typval_T *args, typval_T *rvar);
				// implementation of function
} funcentry_T;

// Set f_max_argc to VARGS for varargs.
#define VARGS    CHAR_MAX

// values for f_argtype; zero means it cannot be used as a method
#define FEARG_1	    0x01    // base is the first argument
#define FEARG_2     0x02    // base is the second argument
#define FEARG_3     0x03    // base is the third argument
#define FEARG_4     0x04    // base is the fourth argument
#define FEARG_MASK  0x0F    // bits in f_argtype used as argument index
#define FE_X	    0x10    // builtin accepts a non-value (class, typealias)

#もし defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#否则
# define MATH_FUNC(name) NULL
#endif
#ifdef FEAT_TIMERS
# define TIMER_FUNC(name) name
#否则
# define TIMER_FUNC(name) NULL
#endif
#ifdef FEAT_JOB_CHANNEL
# define JOB_FUNC(name) name
#否则
# define JOB_FUNC(name) NULL
#endif
#ifdef FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#否则
# define PROP_FUNC(name) NULL
#endif
#ifdef FEAT_SIGNS
# define SIGN_FUNC(name) name
#否则
# define SIGN_FUNC(name) NULL
#endif
#ifdef FEAT_SOUND
# define SOUND_FUNC(name) name
#否则
# define SOUND_FUNC(name) NULL
#endif
#ifdef FEAT_TERMINAL
# define TERM_FUNC(name) name
#否则
# define TERM_FUNC(name) NULL
#endif
#ifdef FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#否则
# define TABPANEL_FUNC(name) NULL
#endif
#ifdef FEAT_IMAGE
# define IMAGE_FUNC(name) name
#否则
# define IMAGE_FUNC(name) NULL
#endif

static const funcentry_T global_functions[] =
{
    {"abs",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_abs,	    f_abs},
    {"acos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_acos},
    {"add",		2, 2, FEARG_1,	    arg2_listblobmod_item,
			"object, expr",
			ret_first_arg,	    f_add},
    {"and",		2, 2, FEARG_1,	    arg2_number,
			"expr1, expr2",
			ret_number,	    f_and},
    {"append",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_number_bool,    f_append},
    {"appendbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_number_bool,    f_appendbufline},
    {"argc",		0, 1, 0,	    arg1_number,
			"winid",
			ret_number,	    f_argc},
    {"argidx",		0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_argidx},
    {"arglistid",	0, 2, 0,	    arg2_number,
			"winnr, tabnr",
			ret_number,	    f_arglistid},
    {"argv",		0, 2, 0,	    arg2_number,
			"nr, winid",
			ret_argv,	    f_argv},
    {"asin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_asin},
    {"assert_beeps",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_number_bool,    f_assert_beeps},
    {"assert_equal",	2, 3, FEARG_2,	    NULL,
			"expected, actual, msg",
			ret_number_bool,    f_assert_equal},
    {"assert_equalfile", 2, 3, FEARG_1,	    arg3_string,
			"fname_one, fname_two, msg",
			ret_number_bool,    f_assert_equalfile},
    {"assert_exception", 1, 2, 0,	    arg2_string,
			"error, msg",
			ret_number_bool,    f_assert_exception},
    {"assert_fails",	1, 5, FEARG_1,	    arg15_assert_fails,
			"cmd, error, msg, lnum, context",
			ret_number_bool,    f_assert_fails},
    {"assert_false",	1, 2, FEARG_1,	    NULL,
			"actual, msg",
			ret_number_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_number_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_number_bool,    f_assert_match},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_number_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    NULL,
			"expected, actual, msg",
			ret_number_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_number_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_number_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    NULL,
			"actual, msg",
			ret_number_bool,    f_assert_true},
    {"atan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_atan},
    {"atan2",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"expr1, expr2",
			ret_float,	    f_atan2},
    {"autocmd_add",	1, 1, FEARG_1,	    arg1_list_any,
			"acmds",
			ret_bool,	    f_autocmd_add},
    {"autocmd_delete",	1, 1, FEARG_1,	    arg1_list_any,
			"acmds",
			ret_bool,	    f_autocmd_delete},
    {"autocmd_get",	0, 1, FEARG_1,	    arg1_dict_any,
			"opts",
			ret_list_dict_any,  f_autocmd_get},
    {"balloon_gettext",	0, 0, 0,	    NULL,
			NULL,
			ret_string,
#ifdef FEAT_BEVAL
	    f_balloon_gettext
#否则
	    NULL
#endif
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#ifdef FEAT_BEVAL
	    f_balloon_show
#否则
	    NULL
#endif
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#もし defined(FEAT_BEVAL_TERM)
	    f_balloon_split
#否则
	    NULL
#endif
			},
    {"base64_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_blob,	    f_base64_decode},
    {"base64_encode",	1, 1, FEARG_1,	    arg1_blob,
			"blob",
			ret_string,	    f_base64_encode},
    {"bindtextdomain",	2, 2, 0,	    arg2_string,
			"package, path",
			ret_bool,	    f_bindtextdomain},
    {"blob2list",	1, 1, FEARG_1,	    arg1_blob,
			"blob",
			ret_list_number,    f_blob2list},
    {"blob2str",	1, 2, FEARG_1,	    arg2_blob_dict,
			"blob, options",
			ret_list_string,    f_blob2str},
    {"browse",		4, 4, 0,	    arg4_browse,
			"save, title, initdir, default",
			ret_string,	    f_browse},
    {"browsedir",	2, 2, 0,	    arg2_string,
			"title, initdir",
			ret_string,	    f_browsedir},
    {"bufadd",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_number,	    f_bufadd},
    {"bufexists",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_number_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_number_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_number,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_number_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_number_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, create",
			ret_number,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_number,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_number,	    f_bufwinnr},
    {"byte2line",	1, 1, FEARG_1,	    arg1_number,
			"byte",
			ret_number,	    f_byte2line},
    {"byteidx",		2, 3, FEARG_1,	    arg3_string_number_bool,
			"expr, nr, utf16",
			ret_number,	    f_byteidx},
    {"byteidxcomp",	2, 3, FEARG_1,	    arg3_string_number_bool,
			"expr, nr, utf16",
			ret_number,	    f_byteidxcomp},
    {"call",		2, 3, FEARG_1,	    arg3_any_list_dict,
			"func, arglist, dict",
			ret_any,	    f_call},
    {"ceil",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ceil},
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_job,
			"handle",
			ret_number_bool,    JOB_FUNC(f_ch_canread)},
    {"ch_close",	1, 1, FEARG_1,	    arg1_chan_or_job,
			"handle",
			ret_void,	    JOB_FUNC(f_ch_close)},
    {"ch_close_in",	1, 1, FEARG_1,	    arg1_chan_or_job,
			"handle",
			ret_void,	    JOB_FUNC(f_ch_close_in)},
    {"ch_evalexpr",	2, 3, FEARG_1,	    arg23_chanexpr,
			"handle, expr, options",
			ret_any,	    JOB_FUNC(f_ch_evalexpr)},
    {"ch_evalraw",	2, 3, FEARG_1,	    arg23_chanraw,
			"handle, string, options",
			ret_any,	    JOB_FUNC(f_ch_evalraw)},
    {"ch_getbufnr",	2, 2, FEARG_1,	    arg2_chan_or_job_string,
			"handle, what",
			ret_number,	    JOB_FUNC(f_ch_getbufnr)},
    {"ch_getjob",	1, 1, FEARG_1,	    arg1_chan_or_job,
			"channel",
			ret_job,	    JOB_FUNC(f_ch_getjob)},
    {"ch_info",		1, 1, FEARG_1,	    arg1_chan_or_job,
			"handle",
			ret_dict_any,	    JOB_FUNC(f_ch_info)},
    {"ch_listen",	1, 2, FEARG_1,	    arg2_string_dict,
			"address, options",
			ret_channel,	    JOB_FUNC(f_ch_listen)},
    {"ch_log",		1, 2, FEARG_1,	    arg2_string_chan_or_job,
			"msg, handle",
			ret_void,	    f_ch_log},
    {"ch_logfile",	1, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_void,	    f_ch_logfile},
    {"ch_open",		1, 2, FEARG_1,	    arg2_string_dict,
			"address, options",
			ret_channel,	    JOB_FUNC(f_ch_open)},
    {"ch_read",		1, 2, FEARG_1,	    arg2_chan_or_job_dict,
			"handle, options",
			ret_string,	    JOB_FUNC(f_ch_read)},
    {"ch_readblob",	1, 2, FEARG_1,	    arg2_chan_or_job_dict,
			"handle, options",
			ret_blob,	    JOB_FUNC(f_ch_readblob)},
    {"ch_readraw",	1, 2, FEARG_1,	    arg2_chan_or_job_dict,
			"handle, options",
			ret_string,	    JOB_FUNC(f_ch_readraw)},
    {"ch_sendexpr",	2, 3, FEARG_1,	    arg23_chanexpr,
			"handle, expr, options",
			ret_any,	    JOB_FUNC(f_ch_sendexpr)},
    {"ch_sendraw",	2, 3, FEARG_1,	    arg23_chanraw,
			"handle, expr, options",
			ret_void,	    JOB_FUNC(f_ch_sendraw)},
    {"ch_setoptions",	2, 2, FEARG_1,	    arg2_chan_or_job_dict,
			"handle, options",
			ret_void,	    JOB_FUNC(f_ch_setoptions)},
    {"ch_status",	1, 2, FEARG_1,	    arg2_chan_or_job_dict,
			"handle, options",
			ret_string,	    JOB_FUNC(f_ch_status)},
    {"changenr",	0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_changenr},
    {"char2nr",		1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_number,	    f_char2nr},
    {"charclass",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_number,	    f_charclass},
    {"charcol",		1, 2, FEARG_1,	    arg2_string_or_list_number,
			"expr, winid",
			ret_number,	    f_charcol},
    {"charidx",		2, 4, FEARG_1,	    arg4_string_number_bool_bool,
			"string, idx, countcc, utf16",
			ret_number,	    f_charidx},
    {"chdir",		1, 2, FEARG_1,	    arg2_string,
			"dir, scope",
			ret_string,	    f_chdir},
    {"cindent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_number,	    f_cindent},
    {"clearmatches",	0, 1, FEARG_1,	    arg1_number,
			"win",
			ret_void,	    f_clearmatches},
    {"cmdcomplete_info",0, 0, 0,	    NULL,
			NULL,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_number,
			"expr, winid",
			ret_number,	    f_col},
    {"complete",	2, 2, FEARG_2,	    arg2_number_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_number,	    f_complete_add},
    {"complete_check",	0, 0, 0,	    NULL,
			NULL,
			ret_number_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_number_string,
			"msg, choices, default, type",
			ret_number,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    NULL,
			"expr",
			ret_copy,	    f_copy},
    {"cos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cos},
    {"cosh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cosh},
    {"count",		2, 4, FEARG_1,	    arg24_count,
			"comp, expr, ic, start",
			ret_number,	    f_count},
    {"cscope_connection",0,3, 0,	    arg3_number_string_string,
			"num, dbpath, prepend",
			ret_number,	    f_cscope_connection},
    {"cursor",		1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_number,	    f_cursor},
    {"debugbreak",	1, 1, FEARG_1,	    arg1_number,
			"pid",
			ret_number,
#ifdef MSWIN
	    f_debugbreak
#否则
	    NULL
#endif
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_number_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_buffer_lnum_lnum,
			"buf, first, last",
			ret_number_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    NULL,
			NULL,
			ret_number_bool,    f_did_filetype},
    {"diff",		2, 3, FEARG_1,	    arg3_diff,
			"fromlist, tolist, options",
			ret_string_or_any_3,  f_diff},
    {"diff_filler",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_number,	    f_diff_filler},
    {"diff_hlID",	2, 2, FEARG_1,	    arg2_lnum_number,
			"lnum, col",
			ret_number,	    f_diff_hlID},
    {"digraph_get",	1, 1, FEARG_1,	    arg1_string,
			"chars",
			ret_string,	    f_digraph_get},
    {"digraph_getlist",	0, 1, FEARG_1,	    arg1_bool,
			"listall",
			ret_list_string_items, f_digraph_getlist},
    {"digraph_set",	2, 2, FEARG_1,	    arg2_string,
			"chars, digraph",
			ret_bool,	f_digraph_set},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    NULL,
			"expr",
			ret_number_bool,    f_empty},
    {"environ",		0, 0, 0,	    NULL,
			NULL,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    NULL,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    NULL,
			NULL,
			ret_number_bool,    f_eventhandler},
    {"executable",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_number,	    f_executable},
    {"execute",		1, 2, FEARG_1,	    arg12_execute,
			"command, silent",
			ret_string,	    f_execute},
    {"exepath",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_exepath},
    {"exists",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_number_bool,    f_exists},
    {"exists_compiled",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_number_bool,    f_exists_compiled},
    {"exp",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_exp},
    {"expand",		1, 3, FEARG_1,	    arg3_string_bool_bool,
			"string, nosuf, list",
			ret_string_or_any_3,	    f_expand},
    {"expandcmd",	1, 2, FEARG_1,	    arg2_string_dict,
			"string, options",
			ret_string,	    f_expandcmd},
    {"extend",		2, 3, FEARG_1,	    arg23_extend,
			"expr1, expr2, expr3",
			ret_extend,	    f_extend},
    {"extendnew",	2, 3, FEARG_1,	    arg23_extendnew,
			"expr1, expr2, expr3",
			ret_first_cont,	    f_extendnew},
    {"feedkeys",	1, 2, FEARG_1,	    arg2_string,
			"string, mode",
			ret_void,	    f_feedkeys},
    {"file_readable",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"file",
			ret_number_bool,    f_filereadable},
    {"filecopy",	2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_number_bool,    f_filecopy},
    {"filereadable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_number_bool,    f_filereadable},
    {"filewritable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_number,	    f_filewritable},
    {"filter",		2, 2, FEARG_1,	    arg2_filter,
			"expr1, expr2",
			ret_first_arg,	    f_filter},
    {"finddir",		1, 3, FEARG_1,	    arg3_string_string_number,
			"name, path, count",
			ret_string_or_any_3,	    f_finddir},
    {"findfile",	1, 3, FEARG_1,	    arg3_string_string_number,
			"name, path, count",
			ret_string_or_any_3,	    f_findfile},
    {"flatten",		1, 2, FEARG_1,	    arg2_list_any_number,
			"list, maxdepth",
			ret_list_any,	    f_flatten},
    {"flattennew",	1, 2, FEARG_1,	    arg2_list_any_number,
			"list, maxdepth",
			ret_list_any,	    f_flattennew},
    {"float2nr",	1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_number,	    f_float2nr},
    {"floor",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_floor},
    {"fmod",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"expr1, expr2",
			ret_float,	    f_fmod},
    {"fnameescape",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_fnameescape},
    {"fnamemodify",	2, 2, FEARG_1,	    arg2_string,
			"fname, mods",
			ret_string,	    f_fnamemodify},
    {"foldclosed",	1, 2, FEARG_1,	    arg2_lnum_number,
			"lnum, winid",
			ret_number,	    f_foldclosed},
    {"foldclosedend",	1, 2, FEARG_1,	    arg2_lnum_number,
			"lnum, winid",
			ret_number,	    f_foldclosedend},
    {"foldlevel",	1, 2, FEARG_1,	    arg2_lnum_number,
			"lnum, winid",
			ret_number,	    f_foldlevel},
    {"foldtext",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_number,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_first_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    NULL,
			NULL,
			ret_void,	    f_foreground},
    {"fullcommand",	1, 2, FEARG_1,	    arg2_string_bool,
			"name, vim9",
			ret_string,	    f_fullcommand},
    {"funcref",		1, 3, FEARG_1,	    arg3_any_list_dict,
			"name, arglist, dict",
			ret_func_unknown,   f_funcref},
    {"function",	1, 3, FEARG_1,	    arg3_any_list_dict,
			"name, arglist, dict",
			ret_func_unknown,   f_function},
    {"garbagecollect",	0, 1, 0,	    arg1_bool,
			"atexit",
			ret_void,	    f_garbagecollect},
    {"get",		2, 3, FEARG_1,	    arg23_get,
			"list, idx, default",
			ret_get,	    f_get},
    {"getbgcolor",	0, 0, 0,	    NULL,
			NULL,
			ret_list_any,	    f_getbgcolor},
    {"getbufinfo",	0, 1, FEARG_1,	    arg1_buffer_or_dict_any,
			"buf",
			ret_list_dict_any,  f_getbufinfo},
    {"getbufline",	2, 3, FEARG_1,	    arg3_buffer_lnum_lnum,
			"buf, lnum, end",
			ret_list_string,    f_getbufline},
    {"getbufoneline",	2, 2, FEARG_1,	    arg2_buffer_lnum,
			"buf, lnum",
			ret_string,	    f_getbufoneline},
    {"getbufvar",	2, 3, FEARG_1,	    arg3_buffer_string_any,
			"buf, varname, def",
			ret_any,	    f_getbufvar},
    {"getcellpixels",	0, 0, 0,	    NULL,
			NULL,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    NULL,
			NULL,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_number,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    NULL,
			NULL,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_getcmdwintype},
    {"getcompletion",	2, 3, FEARG_1,	    arg3_string_string_bool,
			"pat, type, filtered",
			ret_list_string,    f_getcompletion},
    {"getcompletiontype", 1, 1, FEARG_1,    arg1_string,
			"pat",
			ret_string,	    f_getcompletiontype},
    {"getcurpos",	0, 1, FEARG_1,	    arg1_number,
			"winid",
			ret_list_number,    f_getcurpos},
    {"getcursorcharpos", 0, 1, FEARG_1,	    arg1_number,
			"winid",
			ret_list_number,    f_getcursorcharpos},
    {"getcwd",		0, 2, FEARG_1,	    arg2_number,
			"winnr, tabnr",
			ret_string,	    f_getcwd},
    {"getenv",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_any,	    f_getenv},
    {"getfontname",	0, 1, 0,	    arg1_string,
			"name",
			ret_string,	    f_getfontname},
    {"getfperm",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getfperm},
    {"getfsize",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_number,	    f_getfsize},
    {"getftime",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_number,	    f_getftime},
    {"getftype",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getftype},
    {"getimstatus",	0, 0, 0,	    NULL,
			NULL,
			ret_number_bool,    f_getimstatus},
    {"getinfo",		2, 3, FEARG_2,	    arg3_string_string_dict,
			"kind, name, opts",
			ret_dict_any,	    f_getinfo},
    {"getjumplist",	0, 2, FEARG_1,	    arg2_number,
			"winnr, tabnr",
			ret_list_any,	    f_getjumplist},
    {"getline",		1, 2, FEARG_1,	    arg2_lnum,
			"lnum, end",
			ret_getline,	    f_getline},
    {"getloclist",	1, 2, 0,	    arg2_number_dict_any,
			"nr, what",
			ret_list_or_dict_1, f_getloclist},
    {"getmarklist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_number,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    NULL,
			NULL,
			ret_dict_number,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_getpid},
    {"getpos",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_number,    f_getpos},
    {"getqflist",	0, 1, 0,	    arg1_dict_any,
			"what",
			ret_list_or_dict_0, f_getqflist},
    {"getreg",		0, 3, FEARG_1,	    arg3_string_bool_bool,
			"regname, , list",
			ret_getreg,	    f_getreg},
    {"getreginfo",	0, 1, FEARG_1,	    arg1_string,
			"regname",
			ret_dict_any,	    f_getreginfo},
    {"getregion",	2, 3, FEARG_1,	    arg3_list_list_dict,
			"pos1, pos2, opts",
			ret_list_string,    f_getregion},
    {"getregionpos",	2, 3, FEARG_1,      arg3_list_list_dict,
			"pos1, pos2, opts",
			ret_list_regionpos, f_getregionpos},
    {"getregtype",	0, 1, FEARG_1,	    arg1_string,
			"regname",
			ret_string,	    f_getregtype},
    {"getscriptinfo",	0, 1, 0,	    arg1_dict_any,
			"opts",
			ret_list_dict_any,  f_getscriptinfo},
    {"getstacktrace",	0, 0, 0,	    NULL,
			NULL,
			ret_list_dict_any,  f_getstacktrace},
    {"gettabinfo",	0, 1, FEARG_1,	    arg1_number,
			"tabnr",
			ret_list_dict_any,  f_gettabinfo},
    {"gettabvar",	2, 3, FEARG_1,	    arg3_number_string_any,
			"tabnr, varname, def",
			ret_any,	    f_gettabvar},
    {"gettabwinvar",	3, 4, FEARG_1,	    arg4_number_number_string_any,
			"tabnr, winnr, varname, def",
			ret_any,	    f_gettabwinvar},
    {"gettagstack",	0, 1, FEARG_1,	    arg1_number,
			"winnr",
			ret_dict_any,	    f_gettagstack},
    {"gettext",		1, 2, FEARG_1,	    arg2_string,
			"text, package",
			ret_string,	    f_gettext},
    {"getwininfo",	0, 1, FEARG_1,	    arg1_number,
			"winid",
			ret_list_dict_any,  f_getwininfo},
    {"getwinpos",	0, 1, FEARG_1,	    arg1_number,
			"timeout",
			ret_list_number,    f_getwinpos},
    {"getwinposx",	0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_getwinposy},
    {"getwinvar",	2, 3, FEARG_1,	    arg3_number_string_any,
			"winnr, varname, def",
			ret_any,	    f_getwinvar},
    {"glob",		1, 4, FEARG_1,	    arg14_glob,
			"expr, nosuf, list, alllinks",
			ret_string_or_any_3,	    f_glob},
    {"glob2regpat",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_glob2regpat},
    {"globpath",	2, 5, FEARG_2,	    arg25_globpath,
			"path, expr, nosuf, list, alllinks",
			ret_string_or_any_4,	    f_globpath},
    {"has",		1, 2, 0,	    arg2_string_bool,
			"feature, check",
			ret_number_bool,    f_has},
    {"has_key",		2, 2, FEARG_1,	    arg2_dict_any_string_or_nr,
			"dict, key",
			ret_number_bool,    f_has_key},
    {"haslocaldir",	0, 2, FEARG_1,	    arg2_number,
			"winnr, tabnr",
			ret_number,	    f_haslocaldir},
    {"hasmapto",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"what, mode, abbr",
			ret_number_bool,    f_hasmapto},
    {"highlightID",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_number,	    f_hlID},
    {"highlight_exists",1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_number_bool,    f_hlexists},
    {"histadd",		2, 2, FEARG_2,	    arg2_string,
			"history, item",
			ret_number_bool,    f_histadd},
    {"histdel",		1, 2, FEARG_1,	    arg2_string_string_or_number,
			"history, item",
			ret_number_bool,    f_histdel},
    {"histget",		1, 2, FEARG_1,	    arg2_string_number,
			"history, index",
			ret_string,	    f_histget},
    {"histnr",		1, 1, FEARG_1,	    arg1_string,
			"history",
			ret_number,	    f_histnr},
    {"hlID",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_number,	    f_hlID},
    {"hlexists",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_number_bool,    f_hlexists},
    {"hlget",		0, 2, FEARG_1,	    arg2_string_bool,
			"name, resolve",
			ret_list_dict_any,  f_hlget},
    {"hlset",		1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_number_bool,    f_hlset},
    {"hostname",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    NULL,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_number,	    IMAGE_FUNC(f_image_add)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_number,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_number,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_number,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_index,
			"object, expr, start, ic",
			ret_number,	    f_index},
    {"indexof",		2, 3, FEARG_1,	    arg23_index,
			"object, expr, opts",
			ret_number,	    f_indexof},
    {"input",		1, 3, FEARG_1,	    arg3_string,
			"prompt, text, completion",
			ret_string,	    f_input},
    {"inputdialog",	1, 3, FEARG_1,	    arg3_string,
			"prompt, text, cancelreturn",
			ret_string,	    f_inputdialog},
    {"inputlist",	1, 1, FEARG_1,	    arg1_list_string,
			"textlist",
			ret_number,	    f_inputlist},
    {"inputrestore",	0, 0, 0,	    NULL,
			NULL,
			ret_number_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    NULL,
			NULL,
			ret_number_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_insert,
			"object, item, idx",
			ret_first_arg,	    f_insert},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    NULL,
			NULL,
			ret_void,	    f_interrupt},
    {"invert",		1, 1, FEARG_1,	    arg1_number,
			"expr",
			ret_number,	    f_invert},
    {"isabsolutepath",	1, 1, FEARG_1,	    arg1_string,
			"path",
			ret_number_bool,    f_isabsolutepath},
    {"isdirectory",	1, 1, FEARG_1,	    arg1_string,
			"directory",
			ret_number_bool,    f_isdirectory},
    {"isinf",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_number,	    MATH_FUNC(f_isinf)},
    {"islocked",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_number_bool,    f_islocked},
    {"isnan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_number_bool,    MATH_FUNC(f_isnan)},
    {"items",		1, 1, FEARG_1,	    arg1_list_tuple_dict_blob_or_string,
			"expr",
			ret_list_items,	    f_items},
    {"job_getchannel",	1, 1, FEARG_1,	    arg1_job,
			"job",
			ret_channel,	    JOB_FUNC(f_job_getchannel)},
    {"job_info",	0, 1, FEARG_1,	    arg1_job,
			"job",
			ret_job_info,	    JOB_FUNC(f_job_info)},
    {"job_setoptions",	2, 2, FEARG_1,	    arg2_job_dict,
			"job, options",
			ret_void,	    JOB_FUNC(f_job_setoptions)},
    {"job_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"command, options",
			ret_job,	    JOB_FUNC(f_job_start)},
    {"job_status",	1, 1, FEARG_1,	    arg1_job,
			"job",
			ret_string,	    JOB_FUNC(f_job_status)},
    {"job_stop",	1, 2, FEARG_1,	    arg2_job_string_or_number,
			"job, how",
			ret_number_bool,    JOB_FUNC(f_job_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    NULL,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    NULL,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_keys},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    NULL,	// obsolete
			NULL,
			ret_number,	    f_last_buffer_nr},
    {"len",		1, 1, FEARG_1,	    arg1_len,
			"expr",
			ret_number,	    f_len},
    {"libcall",		3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_string,	    f_libcall},
    {"libcallnr",	3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_number,	    f_libcallnr},
    {"line",		1, 2, FEARG_1,	    arg2_string_number,
			"expr, winid",
			ret_number,	    f_line},
    {"line2byte",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_number,	    f_line2byte},
    {"lispindent",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_number,	    f_lispindent},
    {"list2blob",	1, 1, FEARG_1,	    arg1_list_number,
			"list",
			ret_blob,	    f_list2blob},
    {"list2str",	1, 2, FEARG_1,	    arg2_list_number_bool,
			"list, utf8",
			ret_string,	    f_list2str},
    {"list2tuple",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_tuple_any,	    f_list2tuple},
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_buffer_bool_or_dict,
			"callback, buf, options",
			ret_number,	    f_listener_add},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_number,
			"id",
			ret_number_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_localtime},
    {"log",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_log},
    {"log10",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_log10},
    {"luaeval",		1, 2, FEARG_1,	    arg2_string_any,
			"expr1, expr2",
			ret_any,
#ifdef FEAT_LUA
		f_luaeval
#否则
		NULL
#endif
			},
    {"map",		2, 2, FEARG_1,	    arg2_map,
			"expr1, expr2",
			ret_first_cont,	    f_map},
    {"maparg",		1, 4, FEARG_1,	    arg14_maparg,
			"name, mode, abbr, dict",
			ret_maparg,	    f_maparg},
    {"mapcheck",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"name, mode, abbr",
			ret_string,	    f_mapcheck},
    {"maplist",		0, 1, 0,	    arg1_bool,
			"abbr",
			ret_list_dict_any,  f_maplist},
    {"mapnew",		2, 2, FEARG_1,	    arg2_mapnew,
			"expr1, expr2",
			ret_first_cont,	    f_mapnew},
    {"mapset",		1, 3, FEARG_1,	    arg3_string_or_dict_bool_dict,
			"mode, abbr, dict",
			ret_void,	    f_mapset},
    {"match",		2, 4, FEARG_1,	    arg24_match_func,
			"expr, pat, start, count",
			ret_number,	    f_match},
    {"matchadd",	2, 5, FEARG_1,	    arg25_matchadd,
			"group, pattern, priority, id, dict",
			ret_number,	    f_matchadd},
    {"matchaddpos",	2, 5, FEARG_1,	    arg25_matchaddpos,
			"group, pos, priority, id, dict",
			ret_number,	    f_matchaddpos},
    {"matcharg",	1, 1, FEARG_1,	    arg1_number,
			"nr",
			ret_list_string,    f_matcharg},
    {"matchbufline",	4, 5, FEARG_1,	    arg45_matchbufline,
			"buf, pat, lnum, end, dict",
			ret_list_any,	    f_matchbufline},
    {"matchdelete",	1, 2, FEARG_1,	    arg2_number,
			"id, win",
			ret_number_bool,    f_matchdelete},
    {"matchend",	2, 4, FEARG_1,	    arg24_match_func,
			"expr, pat, start, count",
			ret_number,	    f_matchend},
    {"matchfuzzy",	2, 3, FEARG_1,	    arg3_list_string_dict,
			"list, str, dict",
			ret_list_any,	    f_matchfuzzy},
    {"matchfuzzypos",	2, 3, FEARG_1,	    arg3_list_string_dict,
			"list, str, dict",
			ret_list_list_any,  f_matchfuzzypos},
    {"matchlist",	2, 4, FEARG_1,	    arg24_match_func,
			"expr, pat, start, count",
			ret_list_string,    f_matchlist},
    {"matchstr",	2, 4, FEARG_1,	    arg24_match_func,
			"expr, pat, start, count",
			ret_string,	    f_matchstr},
    {"matchstrlist",	2, 3, FEARG_1,	    arg23_matchstrlist,
			"list, pat, dict",
			ret_list_any,	    f_matchstrlist},
    {"matchstrpos",	2, 4, FEARG_1,	    arg24_match_func,
			"expr, pat, start, count",
			ret_list_any,	    f_matchstrpos},
    {"max",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_max},
    {"menu_info",	1, 2, FEARG_1,	    arg2_string,
			"name, mode",
			ret_dict_any,
#ifdef FEAT_MENU
	    f_menu_info
#否则
	    NULL
#endif
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_number,
			"name, flags, prot",
			ret_number_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_mode},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_MZSCHEME
	    f_mzeval
#否则
	    NULL
#endif
			},
    {"nextnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_number,	    f_nextnonblank},
    {"ngettext",	3, 4, FEARG_3,	    arg4_string_string_number_string,
			"single, plural, number, domain",
			ret_string,	    f_ngettext},
    {"nr2char",		1, 2, FEARG_1,	    arg2_number_bool,
			"expr, utf8",
			ret_string,	    f_nr2char},
    {"or",		2, 2, FEARG_1,	    arg2_number,
			"expr1, expr2",
			ret_number,	    f_or},
    {"pathshorten",	1, 2, FEARG_1,	    arg2_string_number,
			"path, len",
			ret_string,	    f_pathshorten},
    {"perleval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_PERL
	    f_perleval
#否则
	    NULL
#endif
			},
    {"popup_atcursor",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_number,	    PROP_FUNC(f_popup_atcursor)},
    {"popup_beval",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_number,	    PROP_FUNC(f_popup_beval)},
    {"popup_clear",	0, 1, 0,	    arg1_bool,
			"force",
			ret_void,	    PROP_FUNC(f_popup_clear)},
    {"popup_close",	1, 2, FEARG_1,	    arg2_number_any,
			"id, result",
			ret_void,	    PROP_FUNC(f_popup_close)},
    {"popup_create",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_number,	    PROP_FUNC(f_popup_create)},
    {"popup_dialog",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_number,	    PROP_FUNC(f_popup_dialog)},
    {"popup_filter_menu", 2, 2, 0,	    arg2_number_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_menu)},
    {"popup_filter_yesno", 2, 2, 0,	    arg2_number_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_yesno)},
    {"popup_findecho",	0, 0, 0,	    NULL,
			NULL,
			ret_number,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    NULL,
			NULL,
			ret_number,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    NULL,
			NULL,
			ret_number,	    PROP_FUNC(f_popup_findpreview)},
    {"popup_getoptions", 1, 1, FEARG_1,	    arg1_number,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getoptions)},
    {"popup_getpos",	1, 1, FEARG_1,	    arg1_number,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getpos)},
    {"popup_hide",	1, 1, FEARG_1,	    arg1_number,
			"id",
			ret_void,	    PROP_FUNC(f_popup_hide)},
    {"popup_list",	0, 0, 0,	    NULL,
			NULL,
			ret_list_number,    PROP_FUNC(f_popup_list)},
    {"popup_locate",	2, 2, 0,	    arg2_number,
			"row, col",
			ret_number,	    PROP_FUNC(f_popup_locate)},
    {"popup_menu",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_number,	    PROP_FUNC(f_popup_menu)},
    {"popup_move",	2, 2, FEARG_1,	    arg2_number_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_move)},
    {"popup_notification", 2, 2, FEARG_1,   arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_number,	    PROP_FUNC(f_popup_notification)},
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_number_buffer,
			"id, buf",
			ret_number_bool,    PROP_FUNC(f_popup_setbuf)},
    {"popup_setoptions", 2, 2, FEARG_1,	    arg2_number_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_setoptions)},
    {"popup_settext",	2, 2, FEARG_1,	    arg2_number_string_or_list,
			"id, text",
			ret_void,	    PROP_FUNC(f_popup_settext)},
    {"popup_show",	1, 1, FEARG_1,	    arg1_number,
			"id",
			ret_number,	    PROP_FUNC(f_popup_show)},
    {"pow",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"x, y",
			ret_float,	    f_pow},
    {"preinserted",	0, 0, 0,	    NULL,
			NULL,
			ret_number_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_number,	    f_prevnonblank},
    {"printf",		1, 19, FEARG_2,	    arg119_printf,
			"fmt, expr1",
			ret_string,	    f_printf},
    {"prompt_getprompt", 1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    JOB_FUNC(f_prompt_getprompt)},
    {"prompt_setcallback", 2, 2, FEARG_1,   arg2_buffer_any,
			"buf, expr",
			ret_void,	    JOB_FUNC(f_prompt_setcallback)},
    {"prompt_setinterrupt", 2, 2, FEARG_1,  arg2_buffer_any,
			"buf, expr",
			ret_void,	    JOB_FUNC(f_prompt_setinterrupt)},
    {"prompt_setprompt", 2, 2, FEARG_1,	    arg2_buffer_string,
			"buf, text",
			ret_void,	    JOB_FUNC(f_prompt_setprompt)},
    {"prop_add",	3, 3, FEARG_1,	    arg3_number_number_dict,
			"lnum, col, props",
			ret_number,	    PROP_FUNC(f_prop_add)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_add_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_number_number_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_find)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_number_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_number_number,
			"props, lnum, lnum_end",
			ret_number,	    PROP_FUNC(f_prop_remove)},
    {"prop_type_add",	2, 2, FEARG_1,	    arg2_string_dict,
			"name, props",
			ret_void,	    PROP_FUNC(f_prop_type_add)},
    {"prop_type_change", 2, 2, FEARG_1,	    arg2_string_dict,
			"name, props",
			ret_void,	    PROP_FUNC(f_prop_type_change)},
    {"prop_type_delete", 1, 2, FEARG_1,	    arg2_string_dict,
			"name, props",
			ret_void,	    PROP_FUNC(f_prop_type_delete)},
    {"prop_type_get",	1, 2, FEARG_1,	    arg2_string_dict,
			"name, props",
			ret_dict_any,	    PROP_FUNC(f_prop_type_get)},
    {"prop_type_list",	0, 1, FEARG_1,	    arg1_dict_any,
			"props",
			ret_list_string,    PROP_FUNC(f_prop_type_list)},
    {"pum_getpos",	0, 0, 0,	    NULL,
			NULL,
			ret_dict_number,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    NULL,
			NULL,
			ret_number_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON3
	    f_py3eval
#否则
	    NULL
#endif
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON
	    f_pyeval
#否则
	    NULL
#endif
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#もし defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#否则
	    NULL
#endif
			},
    {"rand",		0, 1, FEARG_1,	    arg1_list_number,
			"expr",
			ret_number,	    f_rand},
    {"range",		1, 3, FEARG_1,	    arg3_number,
			"expr, max, stride",
			ret_list_number,    f_range},
    {"readblob",	1, 3, FEARG_1,	    arg3_string_number_number,
			"fname, offset, size",
			ret_blob,	    f_readblob},
    {"readdir",		1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_string,    f_readdir},
    {"readdirex",	1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_dict_any,  f_readdirex},
    {"readfile",	1, 3, FEARG_1,	    arg3_string_string_number,
			"fname, type, max",
			ret_list_string,    f_readfile},
    {"redraw_listener_add", 1, 1, FEARG_1,  arg1_dict_any,
			"opts",
			ret_number,	    f_redraw_listener_add},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_number,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_reg_recording},
    {"reltime",		0, 2, FEARG_1,	    arg2_list_number,
			"start, end",
			ret_list_number,    f_reltime},
    {"reltimefloat",	1, 1, FEARG_1,	    arg1_list_number,
			"time",
			ret_float,	    f_reltimefloat},
    {"reltimestr",	1, 1, FEARG_1,	    arg1_list_number,
			"time",
			ret_string,	    f_reltimestr},
    {"remote_expr",	2, 4, FEARG_1,	    arg24_remote_expr,
			"server, string, idvar, timeout",
			ret_string,	    f_remote_expr},
    {"remote_foreground", 1, 1, FEARG_1,    arg1_string,
			"server",
			ret_void,	    f_remote_foreground},
    {"remote_peek",	1, 2, FEARG_1,	    arg2_string,
			"serverid, retvar",
			ret_number,	    f_remote_peek},
    {"remote_read",	1, 2, FEARG_1,	    arg2_string_number,
			"serverid, timeout",
			ret_string,	    f_remote_read},
    {"remote_send",	2, 3, FEARG_1,	    arg3_string,
			"server, string, idvar",
			ret_string,	    f_remote_send},
    {"remote_startserver", 1, 1, FEARG_1,   arg1_string,
			"name",
			ret_void,	    f_remote_startserver},
    {"remove",		2, 3, FEARG_1,	    arg23_remove,
			"list, idx, end",
			ret_remove,	    f_remove},
    {"rename",		2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_number_bool,    f_rename},
    {"repeat",		2, 2, FEARG_1,	    arg2_repeat,
			"expr, count",
			ret_repeat,	    f_repeat},
    {"resolve",		1, 1, FEARG_1,	    arg1_string,
			"filename",
			ret_string,	    f_resolve},
    {"reverse",		1, 1, FEARG_1,	    arg1_reverse,
			"object",
			ret_first_arg,	    f_reverse},
    {"round",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_round},
    {"rubyeval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_RUBY
	    f_rubyeval
#否则
	    NULL
#endif
			},
    {"screenattr",	2, 2, FEARG_1,	    arg2_number,
			"row, col",
			ret_number,	    f_screenattr},
    {"screenchar",	2, 2, FEARG_1,	    arg2_number,
			"row, col",
			ret_number,	    f_screenchar},
    {"screenchars",	2, 2, FEARG_1,	    arg2_number,
			"row, col",
			ret_list_number,    f_screenchars},
    {"screencol",	0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_number,
			"winid, lnum, col",
			ret_dict_number,    f_screenpos},
    {"screenrow",	0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_screenrow},
    {"screenstring",	2, 2, FEARG_1,	    arg2_number,
			"row, col",
			ret_string,	    f_screenstring},
    {"search",		1, 5, FEARG_1,	    arg15_search,
			"pattern, flags, stopline, timeout, skip",
			ret_number,	    f_search},
    {"searchcount",	0, 1, FEARG_1,	    arg1_dict_any,
			"options",
			ret_dict_number,    f_searchcount},
    {"searchdecl",	1, 3, FEARG_1,	    arg3_string_bool_bool,
			"name, global, thisblock",
			ret_number_bool,    f_searchdecl},
    {"searchpair",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_number,	    f_searchpair},
    {"searchpairpos",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_list_number,    f_searchpairpos},
    {"searchpos",	1, 5, FEARG_1,	    arg15_search,
			"pattern, flags, stopline, timeout, skip",
			ret_list_number,    f_searchpos},
    {"server2client",	2, 2, FEARG_1,	    arg2_string,
			"clientid, string",
			ret_number_bool,    f_server2client},
    {"serverlist",	0, 1, 0,	    arg1_dict_any,
			"dict",
			ret_string_or_any_1,	    f_serverlist},
    {"setbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_number_bool,    f_setbufline},
    {"setbufvar",	3, 3, FEARG_3,	    arg3_buffer_string_any,
			"buf, varname, val",
			ret_void,	    f_setbufvar},
    {"setcellwidths",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_void,	    f_setcellwidths},
    {"setcharpos",	2, 2, FEARG_2,	    arg2_string_list_number,
			"expr, list",
			ret_number_bool,    f_setcharpos},
    {"setcharsearch",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_setcharsearch},
    {"setcmdline",	1, 2, FEARG_1,	    arg2_string_number,
			"str, pos",
			ret_number_bool,    f_setcmdline},
    {"setcmdpos",	1, 1, FEARG_1,	    arg1_number,
			"pos",
			ret_number_bool,    f_setcmdpos},
    {"setcursorcharpos", 1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_number_bool,    f_setcursorcharpos},
    {"setenv",		2, 2, FEARG_2,	    arg2_string_any,
			"name, val",
			ret_void,	    f_setenv},
    {"setfperm",	2, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_number_bool,    f_setfperm},
    {"setline",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_number_bool,    f_setline},
    {"setloclist",	2, 4, FEARG_2,	    arg24_setloclist,
			"nr, list, action, what",
			ret_number_bool,    f_setloclist},
    {"setmatches",	1, 2, FEARG_1,	    arg2_list_any_number,
			"list, win",
			ret_number_bool,    f_setmatches},
    {"setpos",		2, 2, FEARG_2,	    arg2_string_list_number,
			"expr, list",
			ret_number_bool,    f_setpos},
    {"setqflist",	1, 3, FEARG_1,	    arg13_setqflist,
			"list, action, what",
			ret_number_bool,    f_setqflist},
    {"setreg",		2, 3, FEARG_2,	    arg3_string_any_string,
			"regname, value, options",
			ret_number_bool,    f_setreg},
    {"settabvar",	3, 3, FEARG_3,	    arg3_number_string_any,
			"tabnr, varname, val",
			ret_void,	    f_settabvar},
    {"settabwinvar",	4, 4, FEARG_4,	    arg4_number_number_string_any,
			"tabnr, winnr, varname, val",
			ret_void,	    f_settabwinvar},
    {"settagstack",	2, 3, FEARG_2,	    arg23_settagstack,
			"nr, dict, action",
			ret_number_bool,    f_settagstack},
    {"setwinvar",	3, 3, FEARG_3,	    arg3_number_string_any,
			"winnr, varname, val",
			ret_void,	    f_setwinvar},
    {"sha256",		1, 1, FEARG_1,	    arg1_string_or_blob,
			"expr",
			ret_string,
#ifdef FEAT_CRYPT
	    f_sha256
#否则
	    NULL
#endif
			},
    {"shellescape",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, special",
			ret_string,	    f_shellescape},
    {"shiftwidth",	0, 1, FEARG_1,	    arg1_number,
			"col",
			ret_number,	    f_shiftwidth},
    {"sign_define",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"name, dict",
			ret_sign_define,	    SIGN_FUNC(f_sign_define)},
    {"sign_getdefined",	0, 1, FEARG_1,	    arg1_string,
			"name",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getdefined)},
    {"sign_getplaced",	0, 2, FEARG_1,	    arg02_sign_getplaced,
			"buf, dict",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getplaced)},
    {"sign_jump",	3, 3, FEARG_1,	    arg3_number_string_buffer,
			"id, group, buf",
			ret_number,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_place,
			"id, group, name, buf, dict",
			ret_number,	    SIGN_FUNC(f_sign_place)},
    {"sign_placelist",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_number,    SIGN_FUNC(f_sign_placelist)},
    {"sign_undefine",	0, 1, FEARG_1,	    arg1_string_or_list_string,
			"name",
			ret_number_bool,    SIGN_FUNC(f_sign_undefine)},
    {"sign_unplace",	1, 2, FEARG_1,	    arg2_string_dict,
			"group, dict",
			ret_number_bool,    SIGN_FUNC(f_sign_unplace)},
    {"sign_unplacelist", 1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_number,    SIGN_FUNC(f_sign_unplacelist)},
    {"simplify",	1, 1, FEARG_1,	    arg1_string,
			"filename",
			ret_string,	    f_simplify},
    {"sin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sin},
    {"sinh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sinh},
    {"slice",		2, 3, FEARG_1,	    arg23_slice,
			"expr, start, end",
			ret_slice,	    f_slice},
    {"sort",		1, 3, FEARG_1,	    arg13_sortuniq,
			"list, how, dict",
			ret_first_arg,	    f_sort},
    {"sound_clear",	0, 0, 0,	    NULL,
			NULL,
			ret_void,	    SOUND_FUNC(f_sound_clear)},
    {"sound_playevent",	1, 2, FEARG_1,	    arg2_string_any,
			"name, callback",
			ret_number,	    SOUND_FUNC(f_sound_playevent)},
    {"sound_playfile",	1, 2, FEARG_1,	    arg2_string_any,
			"path, callback",
			ret_number,	    SOUND_FUNC(f_sound_playfile)},
    {"sound_stop",	1, 1, FEARG_1,	    arg1_number,
			"id",
			ret_void,	    SOUND_FUNC(f_sound_stop)},
    {"soundfold",	1, 1, FEARG_1,	    arg1_string,
			"word",
			ret_string,	    f_soundfold},
    {"spellbadword",	0, 1, FEARG_1,	    arg1_string,
			"sentence",
			ret_list_string,    f_spellbadword},
    {"spellsuggest",	1, 3, FEARG_1,	    arg3_string_number_bool,
			"word, max, capital",
			ret_list_string,    f_spellsuggest},
    {"split",		1, 3, FEARG_1,	    arg3_string_string_bool,
			"string, pattern, keepempty",
			ret_list_string,    f_split},
    {"sqrt",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sqrt},
    {"srand",		0, 1, FEARG_1,	    arg1_number,
			"expr",
			ret_list_number,    f_srand},
    {"state",		0, 1, FEARG_1,	    arg1_string,
			"what",
			ret_string,	    f_state},
    {"str2blob",	1, 2, FEARG_1,	    arg2_list_string_dict,
			"list, options",
			ret_blob,	    f_str2blob},
    {"str2float",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, quoted",
			ret_float,	    f_str2float},
    {"str2list",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_list_number,    f_str2list},
    {"str2nr",		1, 3, FEARG_1,	    arg3_string_number_bool,
			"string, base, quoted",
			ret_number,	    f_str2nr},
    {"strcharlen",	1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_number,	    f_strcharlen},
    {"strcharpart",	2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, skipcc",
			ret_string,	    f_strcharpart},
    {"strchars",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, skipcc",
			ret_number,	    f_strchars},
    {"strdisplaywidth",	1, 2, FEARG_1,	    arg2_string_number,
			"string, col",
			ret_number,	    f_strdisplaywidth},
    {"strftime",	1, 2, FEARG_1,	    arg2_string_number,
			"format, time",
			ret_string,
#ifdef HAVE_STRFTIME
	    f_strftime
#否则
	    NULL
#endif
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_number,
			"str, index",
			ret_number,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_number,
			"haystack, needle, start",
			ret_number,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, NULL,
			"expr",
			ret_string,	    f_string},
    {"strlen",		1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_number,	    f_strlen},
    {"strpart",		2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, chars",
			ret_string,	    f_strpart},
    {"strptime",	2, 2, FEARG_1,	    arg2_string,
			"format, timestring",
			ret_number,
#ifdef HAVE_STRPTIME
	    f_strptime
#否则
	    NULL
#endif
			},
    {"strridx",		2, 3, FEARG_1,	    arg3_string_string_number,
			"haystack, needle, start",
			ret_number,	    f_strridx},
    {"strtrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_strtrans},
    {"strutf16len",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, countcc",
			ret_number,	    f_strutf16len},
    {"strwidth",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_number,	    f_strwidth},
    {"submatch",	1, 2, FEARG_1,	    arg2_number_bool,
			"nr, list",
			ret_string_or_any_2,	    f_submatch},
    {"substitute",	4, 4, FEARG_1,	    arg4_string_string_any_string,
			"string, pat, sub, flags",
			ret_string,	    f_substitute},
    {"swapfilelist",	0, 0, 0,	    NULL,
			NULL,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_swapname},
    {"synID",		3, 3, 0,	    arg3_lnum_number_bool,
			"lnum, col, trans",
			ret_number,	    f_synID},
    {"synIDattr",	2, 3, FEARG_1,	    arg3_number_string_string,
			"synID, what, mode",
			ret_string,	    f_synIDattr},
    {"synIDtrans",	1, 1, FEARG_1,	    arg1_number,
			"synID",
			ret_number,	    f_synIDtrans},
    {"synconcealed",	2, 2, 0,	    arg2_lnum_number,
			"lnum, col",
			ret_list_any,	    f_synconcealed},
    {"synstack",	2, 2, 0,	    arg2_lnum_number,
			"lnum, col",
			ret_list_number,    f_synstack},
    {"system",		1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_string,	    f_system},
    {"systemlist",	1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_list_string,    f_systemlist},
    {"tabpagebuflist",	0, 1, FEARG_1,	    arg1_number,
			"arg",
			ret_list_number,    f_tabpagebuflist},
    {"tabpagenr",	0, 1, 0,	    arg1_string,
			"arg",
			ret_number,	    f_tabpagenr},
    {"tabpagewinnr",	1, 2, FEARG_1,	    arg2_number_string,
			"tabarg, arg",
			ret_number,	    f_tabpagewinnr},
    {"tabpanel_getinfo", 0, 0, 0,	    NULL,
			NULL,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_number_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    NULL,
			NULL,
			ret_list_string,    f_tagfiles},
    {"taglist",		1, 2, FEARG_1,	    arg2_string,
			"expr, filename",
			ret_list_dict_any,  f_taglist},
    {"tan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_tan},
    {"tanh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_tanh},
    {"tempname",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_number,	    TERM_FUNC(f_term_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_number,	    TERM_FUNC(f_term_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_buffer_string_dict,
			"buf, filename, options",
			ret_void,	    TERM_FUNC(f_term_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_buffer,
			"buf",
			ret_number,	    TERM_FUNC(f_term_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_buffer,
			"buf",
			ret_list_string,
#もし defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_getansicolors
#否则
	    NULL
#endif
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_number_string,
			"attr, what",
			ret_number,	    TERM_FUNC(f_term_getattr)},
    {"term_getcursor",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_any,	    TERM_FUNC(f_term_getcursor)},
    {"term_getjob",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_job,	    TERM_FUNC(f_term_getjob)},
    {"term_getline",	2, 2, FEARG_1,	    arg2_buffer_lnum,
			"buf, row",
			ret_string,	    TERM_FUNC(f_term_getline)},
    {"term_getscrolled", 1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_number,	    TERM_FUNC(f_term_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_number,    TERM_FUNC(f_term_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, input",
			ret_string,	    TERM_FUNC(f_term_gettty)},
    {"term_list",	0, 0, 0,	    NULL,
			NULL,
			ret_list_number,    TERM_FUNC(f_term_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_buffer_lnum,
			"buf, row",
			ret_list_dict_any,  TERM_FUNC(f_term_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_buffer_string,
			"buf, keys",
			ret_void,	    TERM_FUNC(f_term_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_buffer_list_any,
			"buf, colors",
			ret_void,
#もし defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_setansicolors
#否则
	    NULL
#endif
			},
    {"term_setapi",	2, 2, FEARG_1,	    arg2_buffer_string,
			"buf, expr",
			ret_void,	    TERM_FUNC(f_term_setapi)},
    {"term_setkill",	2, 2, FEARG_1,	    arg2_buffer_string,
			"buf, how",
			ret_void,	    TERM_FUNC(f_term_setkill)},
    {"term_setrestore",	2, 2, FEARG_1,	    arg2_buffer_string,
			"buf, command",
			ret_void,	    TERM_FUNC(f_term_setrestore)},
    {"term_setsize",	3, 3, FEARG_1,	    arg3_buffer_number_number,
			"buf, rows, cols",
			ret_void,	    TERM_FUNC(f_term_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_number,	    TERM_FUNC(f_term_start)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_buffer_number,
			"buf, time",
			ret_void,	    TERM_FUNC(f_term_wait)},
    {"terminalprops",	0, 0, 0,	    NULL,
			NULL,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_number,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    NULL,
			NULL,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    NULL,
			NULL,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   NULL,
			NULL,
			ret_void,	    f_test_garbagecollect_soon},
    {"test_getvalue",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_number,	    f_test_getvalue},
    {"test_gui_event",	2, 2, FEARG_1,	    arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_gui_event},
    {"test_ignore_error", 1, 1, FEARG_1,    arg1_string,
			"expr",
			ret_void,	    f_test_ignore_error},
    {"test_mswin_event", 2, 2, FEARG_1,     arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_mswin_event},
    {"test_null_blob",	0, 0, 0,	    NULL,
			NULL,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    NULL,
			NULL,
			ret_channel,	    JOB_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    NULL,
			NULL,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    NULL,
			NULL,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    NULL,
			NULL,
			ret_job,	    JOB_FUNC(f_test_null_job)},
    {"test_null_list",	0, 0, 0,	    NULL,
			NULL,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    NULL,
			NULL,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    NULL,
			NULL,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_set},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_number,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, NULL,
			"expr",
			ret_number,	    f_test_refcount},
    {"test_setmouse",	2, 2, 0,	    arg2_number,
			"row, col",
			ret_void,	    f_test_setmouse},
    {"test_settime",	1, 1, FEARG_1,	    arg1_number,
			"expr",
			ret_void,	    f_test_settime},
    {"test_srand_seed",	0, 1, FEARG_1,	    arg1_number,
			"seed",
			ret_void,	    f_test_srand_seed},
    {"test_unknown",	0, 0, 0,	    NULL,
			NULL,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    NULL,
			NULL,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_number,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_number_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_number_any_dict,
			"time, callback, options",
			ret_number,	    TIMER_FUNC(f_timer_start)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_number,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    NULL,
			NULL,
			ret_void,	    TIMER_FUNC(f_timer_stopall)},
    {"tolower",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_tolower},
    {"toupper",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_toupper},
    {"tr",		3, 3, FEARG_1,	    arg3_string,
			"src, fromstr, tostr",
			ret_string,	    f_tr},
    {"trim",		1, 3, FEARG_1,	    arg3_string_string_number,
			"text, mask, dir",
			ret_string,	    f_trim},
    {"trunc",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_trunc},
    {"tuple2list",	1, 1, FEARG_1,	    arg1_tuple_any,
			"tuple",
			ret_list_any,	    f_tuple2list},
    {"type",		1, 1, FEARG_1|FE_X, NULL,
			"expr",
			ret_number,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, NULL,
			"expr",
			ret_string,	    f_typename},
    {"undofile",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_string,	    f_undofile},
    {"undotree",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_dict_any,	    f_undotree},
    {"uniq",		1, 3, FEARG_1,	    arg13_sortuniq,
			"list, func, dict",
			ret_first_arg,	    f_uniq},
    {"uri_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_uridecode},
    {"uri_encode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_uriencode},
    {"utf16idx",	2, 4, FEARG_1,	    arg4_string_number_bool_bool,
			"string, idx, countcc, charidx",
			ret_number,	    f_utf16idx},
    {"values",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_member,    f_values},
    {"virtcol",		1, 3, FEARG_1,	    arg3_string_or_list_bool_number,
			"expr, list, winid",
			ret_virtcol,	    f_virtcol},
    {"virtcol2col",	3, 3, FEARG_1,	    arg3_number,
			"winid, lnum, col",
			ret_number,	    f_virtcol2col},
    {"visualmode",	0, 1, 0,	    arg1_bool,
			"expr",
			ret_string,	    f_visualmode},
    {"wildmenumode",	0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    NULL,
			NULL,
			ret_void,	    f_wildtrigger},
    {"win_execute",	2, 3, FEARG_2,	    arg23_win_execute,
			"id, command, silent",
			ret_string,	    f_win_execute},
    {"win_findbuf",	1, 1, FEARG_1,	    arg1_number,
			"bufnr",
			ret_list_number,    f_win_findbuf},
    {"win_getid",	0, 2, FEARG_1,	    arg2_number,
			"win, tab",
			ret_number,	    f_win_getid},
    {"win_gettype",	0, 1, FEARG_1,	    arg1_number,
			"nr",
			ret_string,	    f_win_gettype},
    {"win_gotoid",	1, 1, FEARG_1,	    arg1_number,
			"expr",
			ret_number_bool,    f_win_gotoid},
    {"win_id2tabwin",	1, 1, FEARG_1,	    arg1_number,
			"expr",
			ret_list_number,    f_win_id2tabwin},
    {"win_id2win",	1, 1, FEARG_1,	    arg1_number,
			"expr",
			ret_number,	    f_win_id2win},
    {"win_move_separator", 2, 2, FEARG_1,   arg2_number,
			"nr, offset",
			ret_number_bool,    f_win_move_separator},
    {"win_move_statusline", 2, 2, FEARG_1,  arg2_number,
			"nr, offset",
			ret_number_bool,    f_win_move_statusline},
    {"win_screenpos",	1, 1, FEARG_1,	    arg1_number,
			"nr",
			ret_list_number,    f_win_screenpos},
    {"win_splitmove",   2, 3, FEARG_1,	    arg3_number_number_dict,
			"nr, target, options",
			ret_number_bool,    f_win_splitmove},
    {"winbufnr",	1, 1, FEARG_1,	    arg1_number,
			"nr",
			ret_number,	    f_winbufnr},
    {"wincol",		0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_number,
			"nr",
			ret_number,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_number,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    NULL,
			NULL,
			ret_number,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_number,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    NULL,
			NULL,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    NULL,
			NULL,
			ret_dict_number,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_number,
			"nr",
			ret_number,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    NULL,
			NULL,
			ret_dict_number,    f_wordcount},
    {"writefile",	2, 3, FEARG_1,	    arg23_writefile,
			"object, fname, flags",
			ret_number_bool,    f_writefile},
    {"xor",		2, 2, FEARG_1,	    arg2_number,
			"expr1, expr2",
			ret_number,	    f_xor},
};

/*
 * Return true if specified function allows a type as an argument.
 */
    static 整数
func_allows_type(整数 idx)
{
    戻る (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, 整数 idx)
{
    static 整数	intidx = -1;
    char_u	*name;

    もし (idx == 0)
	intidx = -1;
    もし (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	もし (name != NULL)
	{
	    もし (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_pattern, 2) == 0)
		戻る cat_prefix_varname('g', name);
	    戻る name;
	}
    }
    もし (++intidx < (整数)ARRAY_LENGTH(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	もし (global_functions[intidx].f_func == NULL)
	    戻る (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	もし (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	戻る IObuff;
    }

    戻る NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_expr_name(expand_T *xp, 整数 idx)
{
    static 整数	intidx = -1;
    char_u	*name;

    もし (idx == 0)
	intidx = -1;
    もし (intidx < 0)
    {
	name = get_function_name(xp, idx);
	もし (name != NULL)
	    戻る name;
    }
    戻る get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    static 整数
find_internal_func_opt(char_u *name, 整数 implemented)
{
    整数		first = 0;
    整数		last;
    整数		cmp;
    整数		x;

    last = (整数)ARRAY_LENGTH(global_functions) - 1;

    // Find the function name in the table. Binary search.
    ながら (first <= last)
    {
	x = first + ((unsigned)(last - first) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	もし (cmp < 0)
	    last = x - 1;
	否则 もし (cmp > 0)
	    first = x + 1;
	否则 もし (implemented && global_functions[x].f_func == NULL)
	    break;
	否则
	    戻る x;
    }
    戻る -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    整数
find_internal_func(char_u *name)
{
    戻る find_internal_func_opt(name, TRUE);
}

    整数
has_internal_func(char_u *name)
{
    戻る find_internal_func_opt(name, TRUE) >= 0;
}

    static 整数
has_internal_func_name(char_u *name)
{
    戻る find_internal_func_opt(name, FALSE) >= 0;
}

    char *
internal_func_name(整数 idx)
{
    戻る global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    整数
internal_func_check_arg_types(
	type2_T	*types,
	整数	idx,
	整数	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    もし (!(func_allows_type(idx)))
    {
	ために (整数 i = 0; i < argcount; ++i)
	    もし (check_type_is_value(types[i].type_curr) == FAIL)
		戻る FAIL;
    }

    const argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    もし (argchecks == NULL)
	戻る OK;

    argcontext_T context;

    context.arg_count = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    ために (整数 i = 0; i < argcount && argchecks[i] != NULL; ++i)
    {
	context.arg_idx = i;
	もし (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    戻る FAIL;
    }
    戻る OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    void
internal_func_get_argcount(整数 idx, 整数 *argcount, 整数 *min_argcount)
{
    *argcount = global_functions[idx].f_max_argc;
    *min_argcount = global_functions[idx].f_min_argc;
}

/*
 * Call the "f_retfunc" function to obtain the return type of function "idx".
 * "decl_type" is set to the declared type.
 * "argtypes" is the list of argument types or NULL when there are no
 * arguments.
 * "argcount" may be less than the actual count when only getting the type.
 */
    type_T *
internal_func_ret_type(
	整数	    idx,
	整数	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = NULL;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    もし (*decl_type == NULL)
	*decl_type = ret;
    current_type_gap = NULL;
    戻る ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    整数
internal_func_is_map(整数 idx)
{
    戻る global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    整数
check_internal_func(整数 idx, 整数 argcount)
{
    funcerror_T	    res;
    char	    *name;

    もし (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    否则 もし (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    否则
	戻る global_functions[idx].f_argtype & FEARG_MASK;

    name = internal_func_name(idx);
    もし (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    否则
	semsg(_(e_not_enough_arguments_for_function_str), name);
    戻る -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    static 整数
check_args_for_type(整数 idx, 整数 argcount, typval_T *argvars)
{
    もし (!func_allows_type(idx))
    {
	ために (整数 i = 0; i < argcount; ++i)
	    もし (check_typval_is_value(&argvars[i]) == FAIL)
		戻る FAIL;
    }
    戻る OK;
}

    funcerror_T
call_internal_func(
	char_u	    *name,
	整数	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    整数 i;

    i = find_internal_func(name);
    もし (i < 0)
	戻る FCERR_UNKNOWN;
    もし (argcount < global_functions[i].f_min_argc)
	戻る FCERR_TOOFEW;
    もし (argcount > global_functions[i].f_max_argc)
	戻る FCERR_TOOMANY;
    もし (check_args_for_type(i, argcount, argvars) == FAIL)
	戻る FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    もし (in_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    戻る FCERR_NONE;
}

    void
call_internal_func_by_idx(
	整数	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    もし (in_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_internal_method(
	char_u	    *name,
	整数	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    整数		fi;
    typval_T	argv[MAX_FUNC_ARGS + 1];

    fi = find_internal_func(name);
    もし (fi < 0)
	戻る FCERR_UNKNOWN;
    もし ((global_functions[fi].f_argtype & FEARG_MASK) == 0)
	戻る FCERR_NOTMETHOD;
    もし (argcount + 1 < global_functions[fi].f_min_argc)
	戻る FCERR_TOOFEW;
    もし (argcount + 1 > global_functions[fi].f_max_argc)
	戻る FCERR_TOOMANY;
    もし (check_args_for_type(fi, argcount, argvars) == FAIL)
	戻る FCERR_OTHER;

    もし ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_2)
    {
	もし (argcount < 1)
	    戻る FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	ために (整数 i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    否则 もし ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_3)
    {
	もし (argcount < 2)
	    戻る FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	ために (整数 i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    否则 もし ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_4)
    {
	もし (argcount < 3)
	    戻る FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	ために (整数 i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    否则
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	ために (整数 i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    もし (check_args_for_type(fi, argcount + 1, argv) == FAIL)
	戻る FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    戻る FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    整数
non_zero_arg(typval_T *argvars)
{
    戻る ((argvars[0].v_type == VAR_NUMBER
		&& argvars[0].vval.v_number != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_number == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != NULL
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    static void
f_and(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	戻る;

    rettv->vval.v_number = tv_get_number_chk(&argvars[0], NULL)
					& tv_get_number_chk(&argvars[1], NULL);
}

/*
 * "balloon_show()" function
 */
#ifdef FEAT_BEVAL
    static void
f_balloon_gettext(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    もし (balloonEval == NULL)
	戻る;

    もし (balloonEval->msg == NULL)
	rettv->vval.v_string = NULL;
    否则
	rettv->vval.v_string = vim_strsave(balloonEval->msg);
}

    static void
f_balloon_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    もし (balloonEval == NULL)
	戻る;

    もし (in_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, NULL,
		l == NULL || l->lv_len == 0 ? NULL : l);
    }
    否则
    {
	char_u *mesg;

	もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    戻る;

	mesg = tv_get_string_chk(&argvars[0]);
	もし (mesg != NULL)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? NULL : mesg, NULL);
    }
}

# もし defined(FEAT_BEVAL_TERM)
    static void
f_balloon_split(typval_T *argvars, typval_T *rettv UNUSED)
{
    もし (rettv_list_alloc(rettv) != OK)
	戻る;

    char_u *msg;

    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;
    msg = tv_get_string_chk(&argvars[0]);
    もし (msg != NULL)
    {
	pumitem_T	*array;
	整数		size = split_message(msg, &array);

	// Skip the first and last item, they are always empty.
	ために (整数 i = 1; i < size - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_text, -1);
	ながら (size > 0)
	    vim_free(array[--size].pum_text);
	vim_free(array);
    }
}
# endif
#endif

/*
 * "base64_decode(string)" function
 */
    static void
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    もし (check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    もし (rettv_blob_alloc(rettv) == FAIL)
	戻る;

    char_u *str = tv_get_string_chk(&argvars[0]);
    もし (str != NULL)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    static void
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    もし (check_for_blob_arg(argvars, 0) == FAIL)
	戻る;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    blob_T *blob = argvars->vval.v_blob;
    もし (blob != NULL)
	rettv->vval.v_string =
	    base64_encode(blob->bv_ga.ga_data, blob->bv_ga.ga_len);
}

/*
 * Get the buffer from "arg" and give an error and return NULL if it is not
 * valid.
 */
    buf_T *
get_buf_arg(typval_T *arg)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(arg, FALSE);
    --emsg_off;
    もし (buf == NULL)
	semsg(_(e_invalid_buffer_name_str), tv_get_string(arg));
    戻る buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    static void
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = VVAL_TRUE;

    もし (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	戻る;

    もし (strcmp((const char *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    否则
    {
	もし (bindtextdomain((const char *)argvars[0].vval.v_string, (const char *)argvars[1].vval.v_string) == NULL)
	{
	    do_outofmem_msg((long)0);
	    rettv->vval.v_number = VVAL_FALSE;
	}
    }

    戻る;
}

/*
 * "byte2line(byte)" function
 */
    static void
f_byte2line(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_number = -1;
#否则
    long	boff = 0;

    もし (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	戻る;

    boff = tv_get_number(&argvars[0]) - 1;  // boff gets -1 on type error
    もし (boff < 0)
	rettv->vval.v_number = -1;
    否则
	rettv->vval.v_number = ml_find_line_or_offset(curbuf,
							  (linenr_T)0, &boff);
#endif
}

/*
 * "call(func, arglist [, dict])" function
 */
    static void
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = NULL;
    dict_T	*selfdict = NULL;
    char_u	*tofree = NULL;

    もし (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	戻る;

    もし (check_for_list_arg(argvars, 1) == FAIL)
	戻る;
    もし (argvars[1].vval.v_list == NULL)
	戻る;

    もし (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    否则 もし (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    否则
	func = tv_get_string(&argvars[0]);
    もし (func == NULL || *func == NUL)
	戻る;		// type error, empty name or null function

    もし (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, NULL, FALSE, TFN_INT|TFN_QUIET);
	もし (tofree == NULL)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    戻る;
	}
	もし (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_args(tofree, STRLEN(tofree),
									&p);
	    もし (s != NULL)
	    {
		vim_free(tofree);
		tofree = s;
	    }
	}
	func = tofree;
    }

    もし (argvars[2].v_type != VAR_UNKNOWN)
    {
	もし (check_for_dict_arg(argvars, 2) == FAIL)
	    goto done;

	selfdict = argvars[2].vval.v_dict;
    }

    (void)func_call(func, &argvars[1], partial, selfdict, rettv);

done:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    static void
f_changenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_number = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    static void
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	戻る;

    もし (has_mbyte)
    {
	整数	utf8 = 0;

	もし (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (整数)tv_get_bool_chk(&argvars[1], NULL);

	もし (utf8)
	    rettv->vval.v_number = utf_ptr2char(tv_get_string(&argvars[0]));
	否则
	    rettv->vval.v_number = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    否则
	rettv->vval.v_number = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    static void
get_col(typval_T *argvars, typval_T *rettv, 整数 charcol)
{
    colnr_T	col = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    整数		winchanged = FALSE;

    もし (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_number_arg(argvars, 1) == FAIL)
	戻る;

    もし (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((整数)tv_get_number(&argvars[1]), &tp);
	もし (wp == NULL || tp == NULL)
	    戻る;

	もし (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    戻る;

	check_cursor();
	winchanged = TRUE;
    }

    整数 fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    もし (fp != NULL && fnum == curbuf->b_fnum)
    {
	もし (fp->col == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    もし (fp->lnum <= curbuf->b_ml.ml_line_count)
		col = ml_get_len(fp->lnum) + 1;
	    否则
		col = MAXCOL;
	}
	否则
	{
	    col = fp->col + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    もし (virtual_active() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		もし (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    整数		l;

		    もし (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			col += l;
		}
	    }
	}
    }
    rettv->vval.v_number = col;

    もし (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    static void
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, TRUE);
}

    win_T *
get_optional_window(typval_T *argvars, 整数 idx)
{
    win_T   *win = curwin;

    もし (argvars[idx].v_type == VAR_UNKNOWN)
	戻る curwin;

    win = find_win_by_nr_or_id(&argvars[idx]);
    もし (win == NULL)
    {
	emsg(_(e_invalid_window_number));
	戻る NULL;
    }
    戻る win;
}

/*
 * "col(string)" function
 */
    static void
f_col(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    static void
f_confirm(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#もし defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*message;
    char_u	*buttons = NULL;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    整数		def = 1;
    整数		type = VIM_GENERIC;
    char_u	*typestr;
    整数		error = FALSE;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_number_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	戻る;

    message = tv_get_string_chk(&argvars[0]);
    もし (message == NULL)
	error = TRUE;
    もし (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	もし (buttons == NULL)
	    error = TRUE;
	もし (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (整数)tv_get_number_chk(&argvars[2], &error);
	    もし (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		もし (typestr == NULL)
		    error = TRUE;
		否则
		{
		    switch (TOUPPER_ASC(*typestr))
		    {
			case 'E': type = VIM_ERROR; break;
			case 'Q': type = VIM_QUESTION; break;
			case 'I': type = VIM_INFO; break;
			case 'W': type = VIM_WARNING; break;
			case 'G': type = VIM_GENERIC; break;
		    }
		}
	    }
	}
    }

    もし (buttons == NULL || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    もし (!error)
	rettv->vval.v_number = do_dialog(type, NULL, message, buttons,
							    def, NULL, FALSE);
#endif
}

/*
 * "copy()" function
 */
    static void
f_copy(typval_T *argvars, typval_T *rettv)
{
    item_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    static void
set_cursorpos(typval_T *argvars, typval_T *rettv, 整数 charcol)
{
    long	lnum, col;
    long	coladd = 0;
    整数		set_curswant = TRUE;

    もし (in_vim9script()
	    && (check_for_string_or_number_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	戻る;

    rettv->vval.v_number = -1;
    もし (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	もし (list2fpos(argvars, &pos, NULL, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_argument));
	    戻る;
	}
	lnum = pos.lnum;
	col = pos.col;
	coladd = pos.coladd;
	もし (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    set_curswant = FALSE;
	}
    }
    否则 もし ((argvars[0].v_type == VAR_NUMBER
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_NUMBER
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	もし (lnum < 0)
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
	否则 もし (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	col = (long)tv_get_number_chk(&argvars[1], NULL);
	もし (charcol)
	    col = buf_charidx_to_byteidx(curbuf, lnum, col) + 1;
	もし (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (long)tv_get_number_chk(&argvars[2], NULL);
    }
    否则
    {
	emsg(_(e_invalid_argument));
	戻る;
    }
    もし (lnum < 0 || col < 0 || coladd < 0)
	戻る;		// type error; errmsg already given
    もし (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    もし (col != MAXCOL && --col < 0)
	col = 0;
    curwin->w_cursor.col = col;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    もし (has_mbyte)
	mb_adjust_cursor();

    curwin->w_set_curswant = set_curswant;
    rettv->vval.v_number = 0;
}

/*
 * "cursor(lnum, col)" function, or
 * "cursor(list)"
 *
 * Moves the cursor to the specified line and column.
 * Returns 0 when the position could be set, -1 otherwise.
 */
    static void
f_cursor(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, FALSE);
}

#ifdef MSWIN
/*
 * "debugbreak()" function
 */
    static void
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    整数		pid;

    rettv->vval.v_number = FAIL;
    もし (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	戻る;

    pid = (整数)tv_get_number(&argvars[0]);
    もし (pid == 0)
    {
	emsg(_(e_invalid_argument));
	戻る;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    もし (hProcess == NULL)
	戻る;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_number = OK;
}
#endif

/*
 * "deepcopy()" function
 */
    static void
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    もし (check_for_opt_bool_arg(argvars, 1) == FAIL)
	戻る;

    もし (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], NULL);

    item_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    static void
f_did_filetype(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->vval.v_number = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    static void
f_echoraw(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *str;

    もし (check_secure())
	戻る;

    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    str = tv_get_string_chk(&argvars[0]);
    もし (str != NULL && *str != NUL)
    {
	out_str(str);
	out_flush();
    }
}

/*
 * "empty({expr})" function
 */
    static void
f_empty(typval_T *argvars, typval_T *rettv)
{
    整数		n = FALSE;

    switch (argvars[0].v_type)
    {
	case VAR_STRING:
	case VAR_FUNC:
	    n = argvars[0].vval.v_string == NULL
					  || *argvars[0].vval.v_string == NUL;
	    break;
	case VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == NULL;
	    break;
	case VAR_NUMBER:
	    n = argvars[0].vval.v_number == 0;
	    break;
	case VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    break;
	case VAR_LIST:
	    n = argvars[0].vval.v_list == NULL
					|| argvars[0].vval.v_list->lv_len == 0;
	    break;
	case VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    break;
	case VAR_DICT:
	    n = argvars[0].vval.v_dict == NULL
			|| argvars[0].vval.v_dict->dv_hashtab.ht_used == 0;
	    break;
	case VAR_BOOL:
	case VAR_SPECIAL:
	    n = argvars[0].vval.v_number != VVAL_TRUE;
	    break;
	case VAR_CLASS:
	    n = argvars[0].vval.v_class != NULL;
	    break;
	case VAR_OBJECT:
	    n = object_empty(argvars[0].vval.v_object);
	    break;

	case VAR_BLOB:
	    n = argvars[0].vval.v_blob == NULL
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    break;

	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_job == NULL
			   || argvars[0].vval.v_job->jv_status != JOB_STARTED;
	    break;
#endif
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_channel == NULL
			       || !channel_is_open(argvars[0].vval.v_channel);
	    break;
#endif
	case VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == NULL
		|| argvars[0].vval.v_typealias->ta_name == NULL
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    break;

	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	case VAR_INSTR:
	    internal_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    break;
    }

    rettv->vval.v_number = n;
}

/*
 * "environ()" function
 */
    static void
f_environ(typval_T *argvars UNUSED, typval_T *rettv)
{
#もし !defined(AMIGA)
    整数			i = 0;
    char_u		*entry, *value;
# もし defined (MSWIN)
#  もし !defined(_UCRT)
    extern wchar_t	**_wenviron;
#  endif
# 否则
    extern char		**environ;
# endif

    もし (rettv_dict_alloc(rettv) == FAIL)
	戻る;

# ifdef MSWIN
    もし (*_wenviron == NULL)
	戻る;
# 否则
    もし (*environ == NULL)
	戻る;
# endif

    ために (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	もし ((p = (short_u *)_wenviron[i]) == NULL)
	    戻る;
	entry = utf16_to_enc(p, NULL);
# 否则
	もし ((entry = (char_u *)environ[i]) == NULL)
	    戻る;
	entry = vim_strsave(entry);
# endif
	もし (entry == NULL) // out of memory
	    戻る;
	もし ((value = vim_strchr(entry, '=')) == NULL)
	{
	    vim_free(entry);
	    continue;
	}
	*value++ = NUL;
	dict_add_string(rettv->vval.v_dict, (char *)entry, value);
	vim_free(entry);
    }
#endif
}

/*
 * "err_teapot()" function
 */
    static void
f_err_teapot(typval_T *argvars, typval_T *rettv UNUSED)
{
    もし (argvars[0].v_type != VAR_UNKNOWN)
    {
	もし (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    もし (*skipwhite(s) == NUL)
		戻る;
	}

	整数 err = FALSE;
	整数 do_503 = eval_expr_to_bool(&argvars[0], &err);
	もし (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_available));
	    戻る;
	}
    }

    emsg(_(e_im_a_teapot));
}

/*
 * "escape({string}, {chars})" function
 */
    static void
f_escape(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	戻る;

    rettv->vval.v_string = vim_strsave_escaped(tv_get_string(&argvars[0]),
					 tv_get_string_buf(&argvars[1], buf));
    rettv->v_type = VAR_STRING;
}

/*
 * "eval()" function
 */
    static void
f_eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*s, *p;

    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    s = tv_get_string_chk(&argvars[0]);
    もし (s != NULL)
	s = skipwhite(s);

    p = s;
    もし (s == NULL || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	もし (p != NULL && !aborting())
	    semsg(_(e_invalid_expression_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_NUMBER;
	rettv->vval.v_number = 0;
    }
    否则 もし (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    static void
f_eventhandler(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_number = vgetc_busy || input_busy;
}

static garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    void
execute_redir_str(char_u *value, 整数 value_len)
{
    整数		len;

    もし (value_len == -1)
	len = (整数)STRLEN(value);	// Append the entire string
    否则
	len = value_len;		// Append only "value_len" characters
    もし (ga_grow(&redir_execute_ga, len) == FAIL)
	戻る;

    mch_memmove((char *)redir_execute_ga.ga_data
	    + redir_execute_ga.ga_len, value, len);
    redir_execute_ga.ga_len += len;
}

#もし defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    static char_u *
get_str_line(
    整数	    c UNUSED,
    void    *cookie,
    整数	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    char_u	*start = *(char_u **)cookie;
    char_u	*line;
    char_u	*p;

    p = start;
    もし (p == NULL || *p == NUL)
	戻る NULL;
    p = vim_strchr(p, '\n');
    もし (p == NULL)
	line = vim_strsave(start);
    否则
    {
	line = vim_strnsave(start, p - start);
	p++;
    }

    *(char_u **)cookie = p;
    戻る line;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    void
execute_cmds_from_string(char_u *str)
{
    do_cmdline(NULL, get_str_line, (void *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#endif

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_line(
    整数	    c UNUSED,
    void    *cookie,
    整数	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *item = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    もし (item == NULL)
	戻る NULL;
    s = tv_get_string_buf_chk(&item->li_tv, buf);
    *p = item->li_next;
    戻る s == NULL ? NULL : vim_strsave(s);
}

/*
 * "execute()" function
 */
    void
execute_common(typval_T *argvars, typval_T *rettv, 整数 arg_off)
{
    char_u	*cmd = NULL;
    list_T	*list = NULL;
    整数		save_msg_silent = msg_silent;
    整数		save_emsg_silent = emsg_silent;
    整数		save_emsg_noredir = emsg_noredir;
    整数		save_redir_execute = redir_execute;
    整数		save_redir_off = redir_off;
    garray_T	save_ga;
    整数		save_msg_col = msg_col;
    整数		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    整数		echo_output = FALSE;

    rettv->vval.v_string = NULL;
    rettv->v_type = VAR_STRING;

    もし (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	もし (list == NULL || list->lv_len == 0)
	    // empty list, no commands, empty output
	    戻る;
	++list->lv_refcount;
    }
    否则 もし (argvars[arg_off].v_type == VAR_JOB
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_value_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	戻る;
    }
    否则
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	もし (cmd == NULL)
	    戻る;
    }

    もし (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      in_vim9script());

	もし (s == NULL)
	    戻る;
	もし (*s == NUL)
	    echo_output = TRUE;
	もし (STRNCMP(s, "silent", 6) == 0)
	    ++msg_silent;
	もし (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    否则
	++msg_silent;

    もし (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, sizeof(char), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    もし (!echo_output)
	msg_col = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    もし (cmd != NULL)
	do_cmdline_cmd(cmd);
    否则
    {
	listitem_T	*item;

	CHECK_LIST_MATERIALIZE(list);
	item = list->lv_first;
	do_cmdline(NULL, get_list_line, (void *)&item,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    もし (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((char *)redir_execute_ga.ga_data)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_data;
    }
    否则
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = NULL;
    }
    msg_silent = save_msg_silent;
    emsg_silent = save_emsg_silent;
    emsg_noredir = save_emsg_noredir;

    redir_execute = save_redir_execute;
    もし (redir_execute)
	redir_execute_ga = save_ga;
    redir_off = save_redir_off;

    // "silent reg" or "silent echo x" leaves msg_col somewhere in the line.
    もし (echo_output)
	// When not working silently: put it in column zero.  A following
	// "echon" will overwrite the message, unavoidably.
	msg_col = 0;
    否则
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	msg_col = save_msg_col;
}

/*
 * "execute()" function
 */
    static void
f_execute(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	戻る;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    void
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    整数		n = FALSE;

    もし (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	戻る;

    p = tv_get_string(&argvars[0]);
    もし (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	もし (mch_getenv(p + 1) != NULL)
	    n = TRUE;
	否则
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    もし (p != NULL && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    否则 もし (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, NULL, TRUE) == OK);
	もし (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    否则 もし (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    否则 もし (*p == '?')			// internal function only
    {
	n = has_internal_func_name(p + 1);
    }
    否则 もし (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    否则 もし (*p == '#')
    {
	もし (p[1] == '#')
	    n = autocmd_supported(p + 2);
	否则
	    n = au_exists(p + 1);
    }
    否则				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_number = n;
}

    static void
f_exists_compiled(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    emsg(_(e_exists_compiled_can_only_be_used_in_def_function));
}

/*
 * "expand()" function
 */
    static void
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    size_t	len;
    整数		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    整数		error = FALSE;
    char_u	*result;
#ifdef BACKSLASH_IN_FILENAME
    char_u	*p_csl_save = p_csl;
#endif

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	戻る;

#ifdef BACKSLASH_IN_FILENAME
    // avoid using 'completeslash' here
    p_csl = empty_option;
#endif

    rettv->v_type = VAR_STRING;
    もし (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_set(rettv, NULL);

    s = tv_get_string(&argvars[0]);
    もし (*s == '%' || *s == '#' || *s == '<')
    {
	char	*errormsg = NULL;

	もし (p_verbose == 0)
	    ++emsg_off;
	result = eval_vars(s, s, &len, NULL, &errormsg, NULL, FALSE);
	もし (p_verbose == 0)
	    --emsg_off;
	否则 もし (errormsg != NULL)
	    emsg(errormsg);
	もし (rettv->v_type == VAR_LIST)
	{
	    もし (rettv_list_alloc(rettv) == OK && result != NULL)
		list_append_string(rettv->vval.v_list, result, -1);
	    vim_free(result);
	}
	否则
	    rettv->vval.v_string = result;
    }
    否则
    {
	// When the optional second argument is non-zero, don't remove matches
	// for 'wildignore' and don't put matches for 'suffixes' at the end.
	もし (argvars[1].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	もし (!error)
	{
	    ExpandInit(&xpc);
	    xpc.xp_context = EXPAND_FILES;
	    もし (p_wic)
		options += WILD_ICASE;
	    もし (rettv->v_type == VAR_STRING)
		rettv->vval.v_string = ExpandOne(&xpc, s, NULL,
							   options, WILD_ALL);
	    否则 もし (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, NULL, options, WILD_ALL_KEEP);
		ために (整数 i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	否则
	    rettv->vval.v_string = NULL;
    }
#ifdef BACKSLASH_IN_FILENAME
    p_csl = p_csl_save;
#endif
}

/*
 * "expandcmd()" function
 * Expand all the special characters in a command string.
 */
    static void
f_expandcmd(typval_T *argvars, typval_T *rettv)
{
    exarg_T	eap;
    char_u	*cmdstr;
    char	*errormsg = NULL;
    整数		emsgoff = TRUE;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	戻る;

    もし (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_FIELD(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = NULL;
    eap.cmdidx = CMD_USER;

    もし (emsgoff)
	++emsg_off;
    もし (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	もし (!emsgoff && errormsg != NULL && *errormsg != NUL)
	    emsg(errormsg);
    もし (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    static void
f_feedkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    整数		remap = TRUE;
    整数		insert = FALSE;
    char_u	*keys, *flags;
    char_u	nbuf[NUMBUFLEN];
    整数		typed = FALSE;
    整数		execute = FALSE;
    整数		context = FALSE;
    整数		dangerous = FALSE;
    整数		lowlevel = FALSE;

    // This is not allowed in the sandbox.  If the commands would still be
    // executed in the sandbox it would be OK, but it probably happens later,
    // when "sandbox" is no longer set.
    もし (check_secure())
	戻る;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	戻る;

    keys = tv_get_string(&argvars[0]);

    もし (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	ために ( ; *flags != NUL; ++flags)
	{
	    switch (*flags)
	    {
		case 'n': remap = FALSE; break;
		case 'm': remap = TRUE; break;
		case 't': typed = TRUE; break;
		case 'i': insert = TRUE; break;
		case 'x': execute = TRUE; break;
		case 'c': context = TRUE; break;
		case '!': dangerous = TRUE; break;
		case 'L': lowlevel = TRUE; break;
	    }
	}
    }

    もし (*keys != NUL || execute)
    {
	もし (lowlevel
#ifdef FEAT_VTP
		&& (!is_term_win32()
		    || (keys[0] == 3 && ctrl_c_interrupts && typed))
#endif
	   )
	{
#ifdef USE_INPUT_BUF
	    ch_log(NULL, "feedkeys() lowlevel: %s", keys);

	    整数 len = (整数)STRLEN(keys);
	    ために (整数 idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		もし (keys[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		add_to_input_buf(keys + idx, 1);
	    }
#否则
	    emsg(_(e_lowlevel_input_not_supported));
#endif
	}
	否则
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *keys_esc = vim_strsave_escape_csi(keys);
	    もし (keys_esc == NULL)
		戻る;

	    ch_log(NULL, "feedkeys(%s): %s", typed ? "typed" : "", keys);

	    ins_typebuf(keys_esc, (remap ? REMAP_YES : REMAP_NONE),
				   insert ? 0 : typebuf.tb_len, !typed, FALSE);
	    もし (vgetc_busy
#ifdef FEAT_TIMERS
		    || timer_busy
#endif
		    || input_busy)
		typebuf_was_filled = TRUE;

	    vim_free(keys_esc);
	}

	もし (execute)
	{
	    整数		save_msg_scroll = msg_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    msg_scroll = FALSE;

	    ch_log(NULL, "feedkeys() executing");

	    もし (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_version = 0;
	    }

	    もし (!dangerous)
	    {
		++ex_normal_busy;
		++in_feedkeys;
	    }
	    ++allow_osc_key;
	    exec_normal(TRUE, lowlevel, TRUE);
	    --allow_osc_key;
	    もし (!dangerous)
	    {
		--ex_normal_busy;
		--in_feedkeys;
	    }

	    msg_scroll |= save_msg_scroll;

	    もし (context)
		current_sctx = save_sctx;
	}
    }
}

/*
 * "fnameescape({string})" function
 */
    static void
f_fnameescape(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    static void
f_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    もし (gui.in_use)
    {
	gui_mch_set_foreground();
	戻る;
    }
#endif
#もし defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_set_foreground();
#endif
}

/*
 * "function()" function
 * "funcref()" function
 */
    static void
common_function(typval_T *argvars, typval_T *rettv, 整数 is_funcref)
{
    char_u	*s;
    char_u	*name;
    整数		use_string = FALSE;
    partial_T   *arg_pt = NULL;
    char_u	*trans_name = NULL;
    整数		is_global = FALSE;
    char_u	*start_bracket = NULL;

    もし (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	戻る;

    もし (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    否则 もし (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != NULL)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    否则
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    もし (s == NULL)
    {
	semsg(_(e_invalid_argument_str), "NULL");
	戻る;
    }

    もし ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == NULL) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, NULL);
	もし (*name == '<')
	{
	    // generic function
	    start_bracket = name;
	    もし (skip_generic_func_type_args(&name) == FAIL)
		goto theend;
	}
	もし (*name != NUL)
	    s = NULL;
    }

    もし (s == NULL || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == NULL))
	semsg(_(e_invalid_argument_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    否则 もし (trans_name != NULL && (is_funcref
			 ? find_func(trans_name, is_global) == NULL
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    否则
    {
	整数	dict_idx = 0;
	整数	arg_idx = 0;
	list_T	*list = NULL;

	もし (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	否则 もし (trans_name != NULL && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	否则
	    name = vim_strsave(s);

	もし (argvars[1].v_type != VAR_UNKNOWN)
	{
	    もし (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    否则 もし (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    否则
		// function(name, [args])
		arg_idx = 1;
	    もし (dict_idx > 0)
	    {
		もし (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    goto theend;
		}
		もし (argvars[dict_idx].vval.v_dict == NULL)
		    dict_idx = 0;
	    }
	    もし (arg_idx > 0)
	    {
		もし (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_second_argument_of_function_must_be_list_or_dict));
		    vim_free(name);
		    goto theend;
		}
		list = argvars[arg_idx].vval.v_list;
		もし (list == NULL || list->lv_len == 0)
		    arg_idx = 0;
		否则 もし (list->lv_len > MAX_FUNC_ARGS)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    goto theend;
		}
	    }
	}
	もし (dict_idx > 0 || arg_idx > 0 || arg_pt != NULL || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    もし (pt == NULL)
		vim_free(name);
	    否则
	    {
		もし (arg_idx > 0 || (arg_pt != NULL && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    整数		i = 0;
		    整数		arg_len = 0;
		    整数		lv_len = 0;

		    もし (arg_pt != NULL)
			arg_len = arg_pt->pt_argc;
		    もし (list != NULL)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    もし (pt->pt_argv == NULL)
		    {
			vim_free(pt);
			vim_free(name);
			goto theend;
		    }
		    ために (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    もし (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_ITEMS(list, li)
			    copy_tv(&li->li_tv, &pt->pt_argv[i++]);
		    }
		}

		// For "function(dict.func, [], dict)" and "func" is a partial
		// use "dict".  That is backwards compatible.
		もし (dict_idx > 0)
		{
		    // The dict is bound explicitly, pt_auto is FALSE.
		    pt->pt_dict = argvars[dict_idx].vval.v_dict;
		    ++pt->pt_dict->dv_refcount;
		}
		否则 もし (arg_pt != NULL)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    もし (pt->pt_dict != NULL)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    もし (pt->pt_obj != NULL)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		もし (arg_pt != NULL && arg_pt->pt_func != NULL)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		否则 もし (is_funcref)
		{
		    pt->pt_func = find_func(trans_name, is_global);
		    もし (IS_GENERIC_FUNC(pt->pt_func) && start_bracket != NULL)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&start_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		否则
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		もし (arg_pt != NULL)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	否则
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    もし (start_bracket == NULL)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    否则
	    {
		// generic function
		size_t len = STRLEN(name) + STRLEN(start_bracket);
		rettv->vval.v_string = alloc(len + 1);
		もし (rettv->vval.v_string != NULL)
		{
		    STRCPY(rettv->vval.v_string, name);
		    STRCAT(rettv->vval.v_string, start_bracket);
		}
		vim_free(name);
	    }
	}
    }
theend:
    vim_free(trans_name);
}

/*
 * "funcref()" function
 */
    static void
f_funcref(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, TRUE);
}

/*
 * "function()" function
 */
    static void
f_function(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, FALSE);
}

/*
 * "garbagecollect()" function
 */
    static void
f_garbagecollect(typval_T *argvars, typval_T *rettv UNUSED)
{
    もし (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	戻る;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    もし (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
	garbage_collect_at_exit = TRUE;
}

/*
 * "get()" function
 */
    static void
f_get(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    list_T	*l;
    tuple_T	*tuple;
    dictitem_T	*di;
    dict_T	*d;
    typval_T	*tv = NULL;
    整数		what_is_dict = FALSE;

    もし (argvars[0].v_type == VAR_BLOB)
    {
	整数 error = FALSE;
	整数 idx = tv_get_number_chk(&argvars[1], &error);

	もし (!error)
	{
	    rettv->v_type = VAR_NUMBER;
	    もし (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    もし (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_number = -1;
	    否则
	    {
		rettv->vval.v_number = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    否则 もし (argvars[0].v_type == VAR_LIST)
    {
	もし ((l = argvars[0].vval.v_list) != NULL)
	{
	    整数		error = FALSE;

	    li = list_find(l, (long)tv_get_number_chk(&argvars[1], &error));
	    もし (!error && li != NULL)
		tv = &li->li_tv;
	}
    }
    否则 もし (argvars[0].v_type == VAR_TUPLE)
    {
	もし ((tuple = argvars[0].vval.v_tuple) != NULL)
	{
	    整数		error = FALSE;
	    long	idx;

	    idx = (long)tv_get_number_chk(&argvars[1], &error);
	    もし (!error)
		tv = tuple_find(tuple, idx);
	}
    }
    否则 もし (argvars[0].v_type == VAR_DICT)
    {
	もし ((d = argvars[0].vval.v_dict) != NULL)
	{
	    di = dict_find(d, tv_get_string(&argvars[1]), -1);
	    もし (di != NULL)
		tv = &di->di_tv;
	}
    }
    否则 もし (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	もし (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	否则
	{
	    CLEAR_FIELD(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	もし (pt != NULL)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    もし (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		もし (name == NULL)
		    rettv->vval.v_string = NULL;
		否则
		{
		    もし (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    もし (*what == 'n' && pt->pt_name == NULL
							&& pt->pt_func != NULL)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    否则 もし (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		もし (pt->pt_dict != NULL)
		    rettv_dict_set(rettv, pt->pt_dict);
	    }
	    否则 もし (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		もし (rettv_list_alloc(rettv) == OK)
		{
		    整数 i;

		    ために (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    否则 もし (STRCMP(what, "arity") == 0)
	    {
		整数 required = 0, optional = 0, varargs = FALSE;
		char_u *name = partial_name(pt);

		get_func_arity(name, &required, &optional, &varargs);

		rettv->v_type = VAR_DICT;
		もし (rettv_dict_alloc(rettv) == OK)
		{
		    dict_T *dict = rettv->vval.v_dict;

		    // Take into account the arguments of the partial, if any.
		    // Note that it is possible to supply more arguments than the function
		    // accepts.
		    もし (pt->pt_argc >= required + optional)
			required = optional = 0;
		    否则 もし (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    否则
			required -= pt->pt_argc;

		    dict_add_number(dict, "required", required);
		    dict_add_number(dict, "optional", optional);
		    dict_add_bool(dict, "varargs", varargs);
		}
	    }
	    否则
		semsg(_(e_invalid_argument_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    もし (!what_is_dict)
		戻る;
	}
    }
    否则
	semsg(_(e_argument_of_str_must_be_list_tuple_dictionary_or_blob), "get()");

    もし (tv == NULL)
    {
	もし (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    否则
	copy_tv(tv, rettv);
}

/*
 * "getbgcolor()" function
 *
 * Returns a list [r, g, b] (0..255) describing the current background
 * colour: in GUI mode the "Normal" highlight group's bg, in terminal
 * mode the value reported by the OSC 11 response (see v:termrbgresp).
 * Returns an empty list when no value is available, e.g. before the
 * terminal has answered the query, or when "Normal" has no bg colour.
 */
    static void
f_getbgcolor(typval_T *argvars UNUSED, typval_T *rettv)
{
    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

#もし defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	もし (gui.in_use)
	    try_hl = true;
# endif
# ifdef FEAT_TERMGUICOLORS
	もし (p_tgc)
	    try_hl = true;
# endif
	もし (try_hl)
	{
	    guicolor_T	fg, bg;
	    整数		hl_id = syn_name2id((char_u *)"Normal");

	    もし (hl_id > 0)
	    {
		(void)syn_id2colors(hl_id, &fg, &bg);
		もし (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_number(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_number(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_number(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    戻る;
		}
	    }
	}
    }
#endif
#ifdef FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	もし (term_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_number(rettv->vval.v_list, (varnumber_T)r);
	    list_append_number(rettv->vval.v_list, (varnumber_T)g);
	    list_append_number(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#endif
}

/*
 * "getcellpixels()" function
 */
    static void
f_getcellpixels(typval_T *argvars UNUSED, typval_T *rettv)
{
    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

#もし defined(FEAT_GUI)
    もし (gui.in_use)
    {
	// success pixel size and no gui.
	list_append_number(rettv->vval.v_list, (varnumber_T)gui.char_width);
	list_append_number(rettv->vval.v_list, (varnumber_T)gui.char_height);
    }
    否则
#endif
    {
#もし defined(UNIX) || defined(MSWIN)
	list_append_number(rettv->vval.v_list, (varnumber_T)cell_width);
	list_append_number(rettv->vval.v_list, (varnumber_T)cell_height);
#否则
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_number(rettv->vval.v_list, (varnumber_T)-1);
	list_append_number(rettv->vval.v_list, (varnumber_T)-1);
#endif
    }

}

/*
 * "getchangelist()" function
 */
    static void
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    整数		i;
    list_T	*l;
    dict_T	*d;
    整数		changelistindex;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    もし (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    否则
	buf = tv_get_buf_from_arg(&argvars[0]);
    もし (buf == NULL)
	戻る;

    l = list_alloc();
    もし (l == NULL)
	戻る;
    もし (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	戻る;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    もし (buf == curwin->w_buffer)
    {
	changelistindex = curwin->w_changelistidx;
    }
    否则
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    もし (wip->wi_win == curwin)
		break;
	changelistindex = wip != NULL ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_number(rettv->vval.v_list, (varnumber_T)changelistindex);

    ために (i = 0; i < buf->b_changelistlen; ++i)
    {
	もし (buf->b_changelist[i].lnum == 0)
	    continue;
	もし ((d = dict_alloc()) == NULL)
	    戻る;
	もし (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    戻る;
	}
	dict_add_number(d, "lnum", (long)buf->b_changelist[i].lnum);
	dict_add_number(d, "col", (long)buf->b_changelist[i].col);
	dict_add_number(d, "coladd", (long)buf->b_changelist[i].coladd);
    }
}

    static void
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    整数		getcurpos,
    整数		charcol)
{
    pos_T	*fp = NULL;
    pos_T	pos;
    win_T	*wp = curwin;
    list_T	*l;
    整数		fnum = -1;

    もし (rettv_list_alloc(rettv) == OK)
    {
	l = rettv->vval.v_list;
	もし (getcurpos)
	{
	    もし (argvars[0].v_type != VAR_UNKNOWN)
	    {
		wp = find_win_by_nr_or_id(&argvars[0]);
		もし (wp != NULL)
		    fp = &wp->w_cursor;
	    }
	    否则
		fp = &curwin->w_cursor;
	    もし (fp != NULL && charcol)
	    {
		pos = *fp;
		pos.col =
		    buf_byteidx_to_charidx(wp->w_buffer, pos.lnum, pos.col);
		fp = &pos;
	    }
	}
	否则
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	もし (fnum != -1)
	    list_append_number(l, (varnumber_T)fnum);
	否则
	    list_append_number(l, (varnumber_T)0);
	list_append_number(l, (fp != NULL) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_number(l, (fp != NULL)
		     ? (varnumber_T)(fp->col == MAXCOL ? MAXCOL : fp->col + 1)
							    : (varnumber_T)0);
	list_append_number(l, (fp != NULL) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	もし (getcurpos)
	{
	    整数	    save_set_curswant = curwin->w_set_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    もし (wp == curwin)
		update_curswant();
	    list_append_number(l, wp == NULL ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    もし (wp == curwin && save_set_curswant)
	    {
		curwin->w_set_curswant = save_set_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    否则
	rettv->vval.v_number = FALSE;
}

/*
 * "getcharpos()" function
 */
    static void
f_getcharpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    static void
f_getcharsearch(typval_T *argvars UNUSED, typval_T *rettv)
{
    もし (rettv_dict_alloc(rettv) != OK)
	戻る;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_string(dict, "char", last_csearch());
    dict_add_number(dict, "forward", last_csearch_forward());
    dict_add_number(dict, "until", last_csearch_until());
}

/*
 * "getenv()" function
 */
    static void
f_getenv(typval_T *argvars, typval_T *rettv)
{
    整数	    mustfree = FALSE;
    char_u  *p;

    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    もし (p == NULL)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_number = VVAL_NULL;
	戻る;
    }
    もし (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    static void
f_getfontname(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    もし (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	戻る;

#ifdef FEAT_GUI
    もし (gui.in_use)
    {
	GuiFont font;
	char_u	*name = NULL;

	もし (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	否则
	{
	    name = tv_get_string(&argvars[0]);
	    もし (STRCMP(name, "*") == 0)	    // don't use font dialog
		戻る;
	    font = gui_mch_get_font(name, FALSE);
	    もし (font == NOFONT)
		戻る;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	もし (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#endif
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
static struct
{
    argcheck_T	func;
    char	*types;	    // separated by commas
} argcheck_types[] =
{
    {arg_any,			"any"},
    {arg_blob,			"blob"},
    {arg_bool,			"bool,number"},
    {arg_bool_or_dict_any,	"bool,number,dict<any>"},
    {arg_bool_or_nr,		"bool,number"},
    {arg_buffer,		"string,number"},
    {arg_buffer_or_dict_any,	"string,number,dict<any>"},
    {arg_chan_or_job,		"channel,job"},
    {arg_cursor1,		"number,string,list<any>"},
    {arg_dict_any,		"dict<any>"},
    {arg_dict_any_or_string,	"dict<any>,string"},
    {arg_extend3,		"number,string"},
    {arg_filter_func,		"string,func"},
    {arg_float_or_nr,		"float,number"},
    {arg_foreach_func,		"string,func"},
    {arg_get1,			"blob,list<any>,tuple<any>,dict<any>,func"},
    {arg_item_of_prev,		"any"},
    {arg_job,			"job"},
    {arg_len1,
	    "string,number,blob,list<any>,tuple<any>,dict<any>,object<any>"},
    {arg_list_any,		"list<any>"},
    {arg_list_any_mod,		"list<any>"},
    {arg_list_number,		"list<number>"},
    {arg_list_or_blob,		"list<any>,blob"},
    {arg_list_or_blob_mod,	"list<any>,blob"},
    {arg_list_or_dict_or_blob,	"list<any>,dict<any>,blob"},
    {arg_list_or_dict_or_blob_mod, "list<any>,dict<any>,blob"},
    {arg_list_or_dict_or_blob_or_string, "list<any>,dict<any>,blob,string"},
    {arg_list_or_dict_or_blob_or_string_mod,
					"list<any>,dict<any>,blob,string"},
    {arg_list_or_tuple,		"list<any>,tuple<any>"},
    {arg_list_or_tuple_or_blob,	"list<any>,tuple<any>,blob"},
    {arg_list_or_tuple_or_dict,	"list<any>,tuple<any>,dict<any>"},
    {arg_list_string,		"list<string>"},
    {arg_list_tuple_dict_blob_or_string,
				"list<any>,tuple<any>,dict<any>,blob,string"},
    {arg_lnum,			"string,number"},
    {arg_map_func,		"string,func"},
    {arg_number,		"number"},
    {arg_object,		"object<any>"},
    {arg_remove2,		"number,string"},
    {arg_repeat1,		"string,number,blob,list<any>,tuple<any>"},
    {arg_reverse,		"list<any>,tuple<any>,blob,string"},
    {arg_same_as_prev,		"any"},
    {arg_same_struct_as_prev,	"any"},
    {arg_slice1,		"list<any>,tuple<any>,blob,string"},
    {arg_sort_how,		"string,func"},
    {arg_str_or_nr_or_list,	"string,number,list<any>"},
    {arg_string,		"string"},
    {arg_string_list_tuple_or_blob, "string,list<any>,tuple<any>,blob"},
    {arg_string_list_tuple_or_dict, "string,list<any>,tuple<any>,dict<any>"},
    {arg_string_or_blob,	"string,blob"},
    {arg_string_or_dict_any,	"string,dict<any>"},
    {arg_string_or_func,	"string,func,bool,number"},
    {arg_string_or_list_any,	"string,list<any>"},
    {arg_string_or_list_string,	"string,list<string>"},
    {arg_string_or_nr,		"string,number"},
    {arg_tuple_any,		"tuple<any>"},
    {varargs_class,		"class"},
};

/*
 * The types argument check "check" accepts as a List of strings, or NULL when
 * out of memory.
 */
    static list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    char	*types = "any";

    もし (l == NULL)
	戻る NULL;
    ために (整数 i = 0; i < (整数)ARRAY_LENGTH(argcheck_types); ++i)
	もし (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    break;
	}
    ために (char *p = types; ; )
    {
	char	*end = (char *)vim_strchr((char_u *)p, ',');
	整数	len = end == NULL ? (整数)STRLEN(p) : (整数)(end - p);

	もし (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    戻る NULL;
	}
	もし (end == NULL)
	    break;
	p = end + 1;
    }
    戻る l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    static 整数
getinfo_argtypes(
	list_T			*l,
	const funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    整数		argcount = 0;

    もし (l != NULL)
	FOR_ALL_LIST_ITEMS(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    もし (s == NULL)
		戻る -1;
	    もし (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_ARGS)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		戻る -1;
	    }
	    type = parse_type(&p, type_gap, NULL, NULL, TRUE);
	    もし (type == NULL)
		戻る -1;
	    もし (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		戻る -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    ために ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_ARGS; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    戻る argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    static char *
builtin_ret_type_name(
	整数	    idx,
	整数	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	char	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = internal_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    戻る type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
typedef enum {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_kind_T;

static struct
{
    char		*name;
    getinfo_kind_T	kind;
} getinfo_kinds[] =
{
    {"command",		GI_COMMAND},
    {"function",	GI_FUNCTION},
    {"option",		GI_OPTION},
    {"vimvar",		GI_VIMVAR},
};

/*
 * "getinfo()" function
 */
    static void
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_kind_T	kind;
    整数			ki;
    整数			idx;
    const funcentry_T	*fe;
    dict_T		*d;
    list_T		*args;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_ARGS];
    list_T		*argtypes_list = NULL;
    整数			argcount;
    整数			vim9 = -1;	// -1: the context of the caller
    char		*ret_name;
    char		*tofree;

    もし (rettv_dict_alloc(rettv) == FAIL)
	戻る;
    もし (in_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	戻る;
    もし (check_for_opt_dict_arg(argvars, 2) == FAIL)
	戻る;

    kindname = tv_get_string(&argvars[0]);
    ために (ki = 0; ki < (整数)ARRAY_LENGTH(getinfo_kinds); ++ki)
	もし (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    break;
    もし (ki == (整数)ARRAY_LENGTH(getinfo_kinds))
    {
	semsg(_(e_invalid_argument_str), kindname);
	戻る;
    }
    kind = getinfo_kinds[ki].kind;
    name = tv_get_string_buf(&argvars[1], namebuf);

    もし (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != NULL)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	整数	    todo = (整数)opts->dv_hashtab.ht_used;

	FOR_ALL_HASHTAB_ITEMS(&opts->dv_hashtab, hi, todo)
	{
	    もし (HASHITEM_EMPTY(hi))
		continue;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    もし (STRCMP(di->di_key, "argtypes") == 0 && kind == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    否则 もし (STRCMP(di->di_key, "vim9") == 0 && kind == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    否则
	    {
		semsg(_(e_invalid_argument_str), di->di_key);
		戻る;
	    }
	}
    }
    もし (kind == GI_VIMVAR)
    {
	もし (STRNCMP(name, "v:", 2) == 0)
	    (void)vim_var_info(name + 2, rettv->vval.v_dict);
	戻る;
    }
    もし (kind == GI_COMMAND)
    {
	(void)ex_command_info(name, vim9, rettv->vval.v_dict);
	戻る;
    }
    もし (kind == GI_OPTION)
    {
	(void)option_info(name, rettv->vval.v_dict);
	戻る;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = find_internal_func_opt(name, FALSE);
    もし (idx < 0)
    {
	(void)user_func_info(name, rettv->vval.v_dict);
	戻る;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, sizeof(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    もし (argcount < 0)
    {
	clear_type_list(&type_gap);
	戻る;
    }
    d = rettv->vval.v_dict;
    dict_add_string(d, "name", (char_u *)fe->f_name);
    dict_add_string(d, "kind", (char_u *)"builtin");
    dict_add_bool(d, "available", fe->f_func != NULL);
    dict_add_number(d, "minargs", fe->f_min_argc);
    dict_add_number(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_add_number(d, "method", fe->f_argtype & FEARG_MASK);

    args = list_alloc();
    もし (args == NULL || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	clear_type_list(&type_gap);
	戻る;
    }
    argnames = (char_u *)fe->f_argnames;
    ために (整数 i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = NULL;
	整数	    argnamelen = 0;

	もし (fe->f_argcheck == NULL)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    もし (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		break;
	    check = arg_any;
	}
	否则
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    もし ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == NULL)
		break;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	もし (argnames != NULL)
	{
	    char_u  *end = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = end == NULL ? (整数)STRLEN(argnames)
						     : (整数)(end - argnames);
	    argnames = end == NULL ? NULL : skipwhite(end + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	もし (arg == NULL || types == NULL
		|| dict_add_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    戻る;
	}
	もし ((argnamelen > 0
		    && dict_add_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    戻る;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    もし (argtypes_list == NULL
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	char	*tofree_max;
	char	*max_name;

	ために (整数 i = argcount; i < fe->f_max_argc && i < MAX_FUNC_ARGS; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	もし (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = NULL;
	    ret_name = "any";
	}
	vim_free(tofree_max);
    }
    dict_add_string(d, "returns", (char_u *)ret_name);
    vim_free(tofree);
    clear_type_list(&type_gap);
}

/*
 * "getjumplist()" function
 */
    static void
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    整数		i;
    list_T	*l;
    dict_T	*d;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    もし (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	戻る;

    wp = find_tabwin(&argvars[0], &argvars[1], NULL);
    もし (wp == NULL)
	戻る;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    もし (l == NULL)
	戻る;
    もし (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	戻る;
    }

    list_append_number(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    ために (i = 0; i < wp->w_jumplistlen; ++i)
    {
	もし (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    continue;
	もし ((d = dict_alloc()) == NULL)
	    戻る;
	もし (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    戻る;
	}
	dict_add_number(d, "lnum", (long)wp->w_jumplist[i].fmark.mark.lnum);
	dict_add_number(d, "col", (long)wp->w_jumplist[i].fmark.mark.col);
	dict_add_number(d, "coladd", (long)wp->w_jumplist[i].fmark.mark.coladd);
	dict_add_number(d, "bufnr", (long)wp->w_jumplist[i].fmark.fnum);
	もし (wp->w_jumplist[i].fname != NULL)
	    dict_add_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    static void
f_getpid(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_number = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    static void
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	戻る;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    static void
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	戻る;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    static void
f_getpos(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    static 整数
block_def2str(struct block_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    もし (ret->string == NULL)
    {
	ret->length = 0;
	戻る FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->length = bd->startspaces;
    mch_memmove(ret->string + ret->length, bd->textstart, bd->textlen);
    ret->length += bd->textlen;
    vim_memset(ret->string + ret->length, ' ', bd->endspaces);
    ret->length += bd->endspaces;
    ret->string[ret->length] = NUL;

    戻る OK;
}

    static 整数
getregionpos(
    typval_T	*argvars,
    typval_T	*rettv,
    pos_T	*p1,
    pos_T	*p2,
    整数		*inclusive,
    整数		*region_type,
    oparg_T	*oap)
{
    整数		fnum1 = -1, fnum2 = -1;
    char_u	*type;
    buf_T	*findbuf;
    char_u	default_type[] = "v";
    整数		block_width = 0;
    整数		is_select_exclusive;
    整数		l;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る FAIL;

    もし (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	戻る FAIL;

    もし (list2fpos(&argvars[0], p1, &fnum1, NULL, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, NULL, FALSE) != OK
	    || fnum1 != fnum2)
	戻る FAIL;

    もし (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	もし (type == NULL)
	    type = default_type;
    }
    否则
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    もし (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    否则 もし (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    否则 もし (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	もし (*p != NUL && ((block_width = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_value_for_argument_str_str), "type", type);
	    戻る FAIL;
	}
	*region_type = MBLOCK;
    }
    否则
    {
	semsg(_(e_invalid_value_for_argument_str_str), "type", type);
	戻る FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    もし (findbuf == NULL || findbuf->b_ml.ml_mfp == NULL)
    {
	emsg(_(e_buffer_is_not_loaded));
	戻る FAIL;
    }

    もし (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_line_count)
    {
	semsg(_(e_invalid_line_number_nr), p1->lnum);
	戻る FAIL;
    }
    もし (p1->col == MAXCOL)
	p1->col = ml_get_buf_len(findbuf, p1->lnum) + 1;
    否则 もし (p1->col < 1 || p1->col > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_column_number_nr), p1->col);
	戻る FAIL;
    }

    もし (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_line_count)
    {
	semsg(_(e_invalid_line_number_nr), p2->lnum);
	戻る FAIL;
    }
    もし (p2->col == MAXCOL)
	p2->col = ml_get_buf_len(findbuf, p2->lnum) + 1;
    否则 もし (p2->col < 1 || p2->col > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_column_number_nr), p2->col);
	戻る FAIL;
    }

    curbuf = findbuf;
    curwin->w_buffer = curbuf;
    virtual_op = virtual_active();

    // NOTE: Adjustment is needed.
    p1->col--;
    p2->col--;

    もし (!LT_POS(*p1, *p2))
    {
	// swap position
	pos_T p;

	p = *p1;
	*p1 = *p2;
	*p2 = p;
    }

    もし (*region_type == MCHAR)
    {
	// Handle 'selection' == "exclusive".
	もし (is_select_exclusive && !EQUAL_POS(*p1, *p2))
	    // When backing up to previous line, inclusive becomes false.
	    *inclusive = !unadjust_for_sel_inner(p2);
	// If p2 is on NUL (end of line), inclusive becomes false.
	もし (*inclusive && !virtual_op && *ml_get_pos(p2) == NUL)
	    *inclusive = FALSE;
    }
    否则 もし (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#ifdef FEAT_LINEBREAK
	整数	lbr_saved = reset_lbr();
#endif

	getvvcol(curwin, p1, &sc1, NULL, &ec1, 0);
	getvvcol(curwin, p2, &sc2, NULL, &ec2, 0);

#ifdef FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#endif

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->start = *p1;
	oap->end = *p2;
	oap->start_vcol = MIN(sc1, sc2);
	もし (block_width > 0)
	    oap->end_vcol = oap->start_vcol + block_width - 1;
	否则 もし (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->end_vcol = sc2 - 1;
	否则
	    oap->end_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    もし (l > 1)
	p2->col += l - 1;

    戻る OK;
}

/*
 * "getregion()" function
 */
    static void
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    整数			inclusive = TRUE;
    整数			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    整数			save_virtual;
    string_T		akt = {NULL, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    もし (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	戻る;

    ために (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	整数 ret = OK;

	もし (region_type == MBLOCK)
	{
	    struct block_def	bd;

	    block_prep(&oa, &bd, lnum, FALSE);
	    block_def2str(&bd, &akt);
	}
	否则 もし (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.length = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.length);
	    もし (akt.string == NULL)
		akt.length = 0;
	    否则
		akt.length = s.length;
	}
	否则
	{
	    struct block_def	bd;

	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);
	    block_def2str(&bd, &akt);
	}

	もし (akt.string == NULL)
	{
	    clear_tv(rettv);
	    (void)rettv_list_alloc(rettv);
	    break;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (整数)akt.length);
	vim_free(akt.string);

	もし (ret == FAIL)
	{
	    clear_tv(rettv);
	    (void)rettv_list_alloc(rettv);
	    break;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_buffer = curbuf;
    virtual_op = save_virtual;
}

    static void
add_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    もし (l1 == NULL)
	戻る;

    もし (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	戻る;
    }

    l2 = list_alloc();
    もし (l2 == NULL)
	戻る;

    もし (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	戻る;
    }

    l3 = list_alloc();
    もし (l3 == NULL)
	戻る;

    もし (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	戻る;
    }

    list_append_number(l2, curbuf->b_fnum);
    list_append_number(l2, p1.lnum);
    list_append_number(l2, p1.col);
    list_append_number(l2, p1.coladd);

    list_append_number(l3, curbuf->b_fnum);
    list_append_number(l3, p2.lnum);
    list_append_number(l3, p2.col);
    list_append_number(l3, p2.coladd);
}

/*
 * Compute the positions of the region segment on line "lnum".
 * "ret_p1" is set to the start position of the segment and "ret_p2" to its
 * end position.
 */
    static void
getregionpos_line(
    linenr_T	lnum,
    pos_T	p1,
    pos_T	p2,
    整数		inclusive,
    整数		region_type,
    oparg_T	*oap,
    整数		allow_eol,
    pos_T	*ret_p1,
    pos_T	*ret_p2)
{
    char_u	*line = ml_get(lnum);
    colnr_T	line_len = ml_get_len(lnum);

    もし (region_type == MLINE)
    {
	ret_p1->col = 1;
	ret_p1->coladd = 0;
	ret_p2->col = MAXCOL;
	ret_p2->coladd = 0;
    }
    否则
    {
	struct block_def	bd;

	もし (region_type == MBLOCK)
	    block_prep(oap, &bd, lnum, FALSE);
	否则
	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);

	もし (bd.is_oneChar)  // selection entirely inside one char
	{
	    もし (region_type == MBLOCK)
	    {
		ret_p1->col = mb_prevptr(line, bd.textstart) - line + 1;
		ret_p1->coladd = bd.start_char_vcols
					   - (bd.start_vcol - oap->start_vcol);
	    }
	    否则
	    {
		ret_p1->col = p1.col + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	否则 もし (region_type == MBLOCK && oap->start_vcol > bd.start_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->col = MAXCOL;
	    ret_p1->coladd = oap->start_vcol - bd.start_vcol;
	    bd.is_oneChar = TRUE;
	}
	否则 もし (bd.startspaces > 0)
	{
	    ret_p1->col = mb_prevptr(line, bd.textstart) - line + 1;
	    ret_p1->coladd = bd.start_char_vcols - bd.startspaces;
	}
	否则
	{
	    ret_p1->col = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	もし (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->col = ret_p1->col;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	否则 もし (bd.endspaces > 0)
	{
	    ret_p2->col = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	否则
	{
	    ret_p2->col = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    もし (!allow_eol && ret_p1->col > line_len)
    {
	ret_p1->col = 0;
	ret_p1->coladd = 0;
    }
    否则 もし (ret_p1->col > line_len + 1)
	ret_p1->col = line_len + 1;

    もし (!allow_eol && ret_p2->col > line_len)
    {
	ret_p2->col = ret_p1->col == 0 ? 0 : line_len;
	ret_p2->coladd = 0;
    }
    否则 もし (ret_p2->col > line_len + 1)
	ret_p2->col = line_len + 1;

    ret_p1->lnum = lnum;
    ret_p2->lnum = lnum;
}

/*
 * "getregionpos()" function
 */
    static void
f_getregionpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	p1, p2;
    整数		inclusive = TRUE;
    整数		region_type = -1;
    整数		allow_eol = FALSE;
    整数		bounds_only = FALSE;
    oparg_T	oa;
    整数		lnum;

    buf_T	*save_curbuf;
    整数		save_virtual;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    もし (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	戻る;

    もし (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    もし (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	start_pos, end_pos;

	getregionpos_line(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &start_pos, &end_pos);
	もし (p2.lnum != p1.lnum)
	{
	    pos_T	unused;

	    getregionpos_line(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &unused, &end_pos);
	}
	add_regionpos_range(rettv, start_pos, end_pos);
    }
    否则
    {
	ために (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_line(lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &ret_p1, &ret_p2);
	    add_regionpos_range(rettv, ret_p1, ret_p2);
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_buffer = curbuf;
    virtual_op = save_virtual;
}

/*
 * Common between getreg(), getreginfo() and getregtype(): get the register
 * name from the first argument.
 * Returns zero on error.
 */
    static 整数
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    もし (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	もし (strregname != NULL && in_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_register_name_must_be_one_char_str), strregname);
	    strregname = NULL;
	}
	もし (strregname == NULL)	    // type error; errmsg already given
	    戻る 0;
    }
    否则
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    戻る *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    static void
f_getreg(typval_T *argvars, typval_T *rettv)
{
    整数		regname;
    整数		arg2 = FALSE;
    整数		return_list = FALSE;

    もし (in_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	戻る;

    regname = getreg_get_regname(argvars);
    もし (regname == 0)
	戻る;

    もし (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	整数		error = FALSE;

	arg2 = (整数)tv_get_bool_chk(&argvars[1], &error);

	もし (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (整数)tv_get_bool_chk(&argvars[2], &error);
	もし (error)
	    戻る;
    }

    もし (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_EXPR_SRC : 0) | GREG_LIST);
	もし (rettv->vval.v_list == NULL)
	    (void)rettv_list_alloc(rettv);
	否则
	    ++rettv->vval.v_list->lv_refcount;
    }
    否则
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = get_reg_contents(regname,
						    arg2 ? GREG_EXPR_SRC : 0);
    }
}

/*
 * "getregtype()" function
 */
    static void
f_getregtype(typval_T *argvars, typval_T *rettv)
{
    整数		regname;
    char_u	buf[NUMBUFLEN + 2];
    long	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    もし (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	戻る;

    regname = getreg_get_regname(argvars);
    もし (regname == 0)
	戻る;

    buf[0] = NUL;
    buf[1] = NUL;
    switch (get_reg_type(regname, &reglen))
    {
	case MLINE: buf[0] = 'V'; break;
	case MCHAR: buf[0] = 'v'; break;
	case MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((char *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		break;
    }
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "gettagstack()" function
 */
    static void
f_gettagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;			// default is current window

    もし (rettv_dict_alloc(rettv) == FAIL)
	戻る;

    もし (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	もし (wp == NULL)
	    戻る;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    static void
f_gettext(typval_T *argvars, typval_T *rettv)
{
#もし defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    char *prev = NULL;
#endif

    もし (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	戻る;

    rettv->v_type = VAR_STRING;

    もし (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != NULL &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#もし defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((const char *)argvars[1].vval.v_string, (char *)p_enc);
#endif

#もし defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((const char *)argvars[1].vval.v_string, (const char *)argvars[0].vval.v_string));
#否则
	textdomain((const char *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#endif

#もし defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	もし (prev != NULL)
	    bind_textdomain_codeset((const char *)argvars[1].vval.v_string, prev);
#endif
    }
    否则
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
}

// for VIM_VERSION_ defines
#include "version.h"

/*
 * "has()" function
 */
    void
f_has(typval_T *argvars, typval_T *rettv)
{
    整数		i;
    char_u	*name;
    整数		x = FALSE;
    整数		n = FALSE;
    typedef struct {
	char *name;
	short present;
    } has_item_T;
    static has_item_T has_list[] =
    {
	{"amiga",
#ifdef AMIGA
		1
#否则
		0
#endif
		},
	{"android",
#ifdef __ANDROID__
		1
#否则
		0
#endif
		},
	{"arp",
#もし defined(AMIGA) && defined(FEAT_ARP)
		1
#否则
		0
#endif
		},
	{"haiku",
#ifdef __HAIKU__
		1
#否则
		0
#endif
		},
	{"bsd",
#もし defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#否则
		0
#endif
		},
	{"hpux",
#ifdef hpux
		1
#否则
		0
#endif
		},
	{"hurd",
#ifdef __GNU__
		1
#否则
		0
#endif
		},
	{"linux",
#ifdef __linux__
		1
#否则
		0
#endif
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#ifdef MACOS_X
		1
#否则
		0
#endif
		},
	{"osx",		// Mac OS X
#ifdef MACOS_X
		1
#否则
		0
#endif
		},
	{"macunix",	// Mac OS X, with the darwin feature
#もし defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#否则
		0
#endif
		},
	{"osxdarwin",	// synonym for macunix
#もし defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#否则
		0
#endif
		},
	{"qnx",
#ifdef __QNX__
		1
#否则
		0
#endif
		},
	{"sun",
#ifdef SUN_SYSTEM
		1
#否则
		0
#endif
		},
	{"termux",
#ifdef __TERMUX__
		1
#否则
		0
#endif
		},
	{"unix",
#ifdef UNIX
		1
#否则
		0
#endif
		},
	{"vms",
#ifdef VMS
		1
#否则
		0
#endif
		},
	{"win32",
#ifdef MSWIN
		1
#否则
		0
#endif
		},
	{"win32unix",
#ifdef WIN32UNIX
		1
#否则
		0
#endif
		},
	{"win64",
#ifdef _WIN64
		1
#否则
		0
#endif
		},
	{"ebcdic", 0 },
	{"fname_case",
#ifndef CASE_INSENSITIVE_FILENAME
		1
#否则
		0
#endif
		},
	{"acl",
#ifdef HAVE_ACL
		1
#否则
		0
#endif
		},
	{"arabic",
#ifdef FEAT_ARABIC
		1
#否则
		0
#endif
		},
	{"autocmd", 1},
	{"autochdir",
#ifdef FEAT_AUTOCHDIR
		1
#否则
		0
#endif
		},
	{"autoservername",
#ifdef FEAT_AUTOSERVERNAME
		1
#否则
		0
#endif
		},
	{"socketserver",
#ifdef FEAT_SOCKETSERVER
		1
#否则
		0
#endif
		},
	{"balloon_eval",
#ifdef FEAT_BEVAL_GUI
		1
#否则
		0
#endif
		},
	{"balloon_multiline",
#ifdef FEAT_BEVAL_GUI
		1
#否则
		0
#endif
		},
	{"balloon_eval_term",
#ifdef FEAT_BEVAL_TERM
		1
#否则
		0
#endif
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#もし defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#否则
		0
#endif
		},
	{"byte_offset",
#ifdef FEAT_BYTEOFF
		1
#否则
		0
#endif
		},
	{"channel",
#ifdef FEAT_JOB_CHANNEL
		1
#否则
		0
#endif
		},
	{"cindent", 1},
	{"clientserver",
#ifdef FEAT_CLIENTSERVER
		1
#否则
		0
#endif
		},
	{"clipboard",
#ifdef FEAT_CLIPBOARD
		1
#否则
		0
#endif
		},
	{"clipboard_provider",
#ifdef FEAT_CLIPBOARD_PROVIDER
		1
#否则
		0
#endif
		},
	{"cmdline_compl", 1},
	{"cmdline_hist", 1},
	{"cmdwin", 1},
	{"comments", 1},
	{"conceal",
#ifdef FEAT_CONCEAL
		1
#否则
		0
#endif
		},
	{"cryptv",
#ifdef FEAT_CRYPT
		1
#否则
		0
#endif
		},
	{"crypt-blowfish",
#ifdef FEAT_CRYPT
		1
#否则
		0
#endif
		},
	{"crypt-blowfish2",
#ifdef FEAT_CRYPT
		1
#否则
		0
#endif
		},
	{"cscope",
#ifdef FEAT_CSCOPE
		1
#否则
		0
#endif
		},
	{"cursorbind", 1},
	{"cursorshape",
#ifdef CURSOR_SHAPE
		1
#否则
		0
#endif
		},
	{"debug",
#ifdef DEBUG
		1
#否则
		0
#endif
		},
	{"dialog_con",
#ifdef FEAT_CON_DIALOG
		1
#否则
		0
#endif
		},
	{"dialog_con_gui",
#もし defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#否则
		0
#endif
		},
	{"dialog_gui",
#ifdef FEAT_GUI_DIALOG
		1
#否则
		0
#endif
		},
	{"diff",
#ifdef FEAT_DIFF
		1
#否则
		0
#endif
		},
	{"digraphs",
#ifdef FEAT_DIGRAPHS
		1
#否则
		0
#endif
		},
	{"directx",
#ifdef FEAT_DIRECTX
		1
#否则
		0
#endif
		},
	{"dnd",
#ifdef FEAT_DND
		1
#否则
		0
#endif
		},
	{"drop_file",
#ifdef HAVE_DROP_FILE
		1
#否则
		0
#endif
		},
	{"emacs_tags",
#ifdef FEAT_EMACS_TAGS
		1
#否则
		0
#endif
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#ifdef FEAT_SEARCH_EXTRA
		1
#否则
		0
#endif
		},
	{"file_in_path", 1},
	{"filterpipe",
#もし defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#否则
		0
#endif
		},
	{"find_in_path",
#ifdef FEAT_FIND_ID
		1
#否则
		0
#endif
		},
	{"float", 1},
	{"folding",
#ifdef FEAT_FOLDING
		1
#否则
		0
#endif
		},
	{"footer", 0},
	{"fork",
#もし !defined(USE_SYSTEM) && defined(UNIX)
		1
#否则
		0
#endif
		},
	{"gettext",
#ifdef FEAT_GETTEXT
		1
#否则
		0
#endif
		},
	{"gui",
#ifdef FEAT_GUI
		1
#否则
		0
#endif
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#ifdef FEAT_GUI_GTK
		1
#否则
		0
#endif
		},
	{"gui_gtk2",
#もし defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#否则
		0
#endif
		},
	{"gui_gtk3",
#もし defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#否则
		0
#endif
		},
	{"gui_gtk4",
#もし defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#否则
		0
#endif
		},
	{"gui_gnome",
#ifdef FEAT_GUI_GNOME
		1
#否则
		0
#endif
		},
	{"gui_haiku",
#ifdef FEAT_GUI_HAIKU
		1
#否则
		0
#endif
		},
	{"gui_mac", 0},
	{"gui_motif",
#ifdef FEAT_GUI_MOTIF
		1
#否则
		0
#endif
		},
	{"gui_photon",
#ifdef FEAT_GUI_PHOTON
		1
#否则
		0
#endif
		},
	{"gui_win32",
#ifdef FEAT_GUI_MSWIN
		1
#否则
		0
#endif
		},
	{"iconv",
#もし defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#否则
		0
#endif
		},
	{"image",
#ifdef FEAT_IMAGE
		1
#否则
		0
#endif
		},
	{"image_popup",
#ifdef FEAT_IMAGE_POPUP
		1
#否则
		0
#endif
		},
	{"insert_expand", 1},
	{"ipv6",
#ifdef FEAT_IPV6
		1
#否则
		0
#endif
	},
	{"job",
#ifdef FEAT_JOB_CHANNEL
		1
#否则
		0
#endif
		},
	{"jumplist", 1},
	{"keymap",
#ifdef FEAT_KEYMAP
		1
#否则
		0
#endif
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#ifdef FEAT_LANGMAP
		1
#否则
		0
#endif
		},
	{"libcall",
#ifdef FEAT_LIBCALL
		1
#否则
		0
#endif
		},
	{"linebreak",
#ifdef FEAT_LINEBREAK
		1
#否则
		0
#endif
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#もし defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#否则
		0
#endif
		},
	{"menu",
#ifdef FEAT_MENU
		1
#否则
		0
#endif
		},
	{"mksession",
#ifdef FEAT_SESSION
		1
#否则
		0
#endif
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#ifdef FEAT_MOUSESHAPE
		1
#否则
		0
#endif
		},
	{"mouse_dec",
#もし (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#否则
		0
#endif
		},
	{"mouse_gpm",
#もし (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#否则
		0
#endif
		},
	{"mouse_jsbterm",
#もし (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#否则
		0
#endif
		},
	{"mouse_netterm",
#もし (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#否则
		0
#endif
		},
	{"mouse_pterm",
#もし (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#否则
		0
#endif
		},
	{"mouse_sgr",
#もし (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#否则
		0
#endif
		},
	{"mouse_sysmouse",
#もし (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#否则
		0
#endif
		},
	{"mouse_urxvt",
#もし (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#否则
		0
#endif
		},
	{"mouse_xterm",
#もし (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#否则
		0
#endif
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#ifdef FEAT_MBYTE_IME
		1
#否则
		0
#endif
		},
	{"multi_lang",
#ifdef FEAT_MULTI_LANG
		1
#否则
		0
#endif
		},
	{"mzscheme",
#もし defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#否则
		0
#endif
		},
	{"nanotime",
#ifdef ST_MTIM_NSEC
		1
#否则
		0
#endif
	},
	{"num64", 1},
	{"ole",
#ifdef FEAT_OLE
		1
#否则
		0
#endif
		},
	{"packages",
#ifdef FEAT_EVAL
		1
#否则
		0
#endif
		},
	{"path_extra", 1},
	{"perl",
#もし defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#否则
		0
#endif
		},
	{"persistent_undo",
#ifdef FEAT_PERSISTENT_UNDO
		1
#否则
		0
#endif
		},
	{"python_compiled",
#もし defined(FEAT_PYTHON)
		1
#否则
		0
#endif
		},
	{"python_dynamic",
#もし defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#否则
		0
#endif
		},
	{"python",
#もし defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#否则
		0
#endif
		},
	{"pythonx",
#もし (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#否则
		0
#endif
		},
	{"python3_compiled",
#もし defined(FEAT_PYTHON3)
		1
#否则
		0
#endif
		},
	{"python3_dynamic",
#もし defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#否则
		0
#endif
		},
	{"python3_stable",
#もし defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#否则
		0
#endif
		},
	{"python3",
#もし defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#否则
		0
#endif
		},
	{"popupwin",
#ifdef FEAT_PROP_POPUP
		1
#否则
		0
#endif
		},
	{"postscript",
#ifdef FEAT_POSTSCRIPT
		1
#否则
		0
#endif
		},
	{"pango",
#ifdef FEAT_PRINT_PANGO
		1
#否则
		0
#endif
		},
	{"printer",
#ifdef FEAT_PRINTER
		1
#否则
		0
#endif
		},
	{"profile",
#ifdef FEAT_PROFILE
		1
#否则
		0
#endif
		},
	{"prof_nsec",
#ifdef PROF_NSEC
		1
#否则
		0
#endif
		},
	{"reltime",
#ifdef FEAT_RELTIME
		1
#否则
		0
#endif
		},
	{"quickfix",
#ifdef FEAT_QUICKFIX
		1
#否则
		0
#endif
		},
	{"rightleft",
#ifdef FEAT_RIGHTLEFT
		1
#否则
		0
#endif
		},
	{"ruby",
#もし defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#否则
		0
#endif
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#ifdef FEAT_SIGNS
		1
#否则
		0
#endif
		},
	{"smartindent", 1},
	{"startuptime",
#ifdef STARTUPTIME
		1
#否则
		0
#endif
		},
	{"statusline",
#ifdef FEAT_STL_OPT
		1
#否则
		0
#endif
		},
	{"statusline_click",
#ifdef FEAT_STL_OPT
		1
#否则
		0
#endif
		},
	{"netbeans_intg",
#ifdef FEAT_NETBEANS_INTG
		1
#否则
		0
#endif
		},
	{"sodium",
#もし defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#否则
		0
#endif
		},
	{"sound",
#ifdef FEAT_SOUND
		1
#否则
		0
#endif
		},
	{"spell",
#ifdef FEAT_SPELL
		1
#否则
		0
#endif
		},
	{"syntax",
#ifdef FEAT_SYN_HL
		1
#否则
		0
#endif
		},
	{"system",
#もし defined(USE_SYSTEM) || !defined(UNIX)
		1
#否则
		0
#endif
		},
	{"tabpanel",
#もし defined(FEAT_TABPANEL)
		1,
#否则
		0,
#endif
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#もし defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#否则
		0
#endif
		},
	{"termguicolors",
#ifdef FEAT_TERMGUICOLORS
		1
#否则
		0
#endif
		},
	{"terminal",
#もし defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#否则
		0
#endif
		},
	{"terminfo",
#ifdef TERMINFO
		1
#否则
		0
#endif
		},
	{"termresponse",
#ifdef FEAT_TERMRESPONSE
		1
#否则
		0
#endif
		},
	{"textobjects", 1},
	{"textprop",
#ifdef FEAT_PROP_POPUP
		1
#否则
		0
#endif
		},
	{"tgetent",
#ifdef HAVE_TGETENT
		1
#否则
		0
#endif
		},
	{"timers",
#ifdef FEAT_TIMERS
		1
#否则
		0
#endif
		},
	{"title", 1},
	{"toolbar",
#ifdef FEAT_TOOLBAR
		1
#否则
		0
#endif
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#ifdef FEAT_VARTABS
		1
#否则
		0
#endif
		},
	{"vertsplit", 1},
	{"viminfo",
#ifdef FEAT_VIMINFO
		1
#否则
		0
#endif
		},
	{"vim9script", 1},
	{"vimscript-1", 1},
	{"vimscript-2", 1},
	{"vimscript-3", 1},
	{"vimscript-4", 1},
	{"virtualedit", 1},
	{"visual", 1},
	{"visualextra", 1},
	{"vreplace", 1},
	{"vtp",
#ifdef FEAT_VTP
		1
#否则
		0
#endif
		},
	{"wayland",
#ifdef FEAT_WAYLAND
		1
#否则
		0
#endif
		},
	{"wayland_clipboard",
#ifdef FEAT_WAYLAND_CLIPBOARD
		1
#否则
		0
#endif
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#ifdef FEAT_WAK
		1
#否则
		0
#endif
		},
	{"writebackup",
#ifdef FEAT_WRITEBACKUP
		1
#否则
		0
#endif
		},
	{"xattr",
#ifdef FEAT_XATTR
		1
#否则
		0
#endif
		},
	{"xim",
#ifdef FEAT_XIM
		1
#否则
		0
#endif
		},
	{"xfontset",
#ifdef FEAT_XFONTSET
		1
#否则
		0
#endif
		},
	{"xpm",
#もし defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#否则
		0
#endif
		},
	{"xpm_w32",	// for backward compatibility
#ifdef FEAT_XPM_W32
		1
#否则
		0
#endif
		},
	{"xsmp",
#ifdef USE_XSMP
		1
#否则
		0
#endif
		},
	{"xsmp_interact",
#ifdef USE_XSMP_INTERACT
		1
#否则
		0
#endif
		},
	{"xterm_clipboard",
#ifdef FEAT_XCLIPBOARD
		1
#否则
		0
#endif
		},
	{"xterm_save",
#ifdef FEAT_XTERM_SAVE
		1
#否则
		0
#endif
		},
	{"X11",
#もし defined(UNIX) && defined(FEAT_X11)
		1
#否则
		0
#endif
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#もし defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#否则
		0
#endif
		},
	{NULL, 0}
    };

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	戻る;

    name = tv_get_string(&argvars[0]);

    // Fast-path: check features not in has_list[] first to avoid the full
    // linear scan for very common queries like has('patch-...').
    もし (STRNICMP(name, "patch", 5) == 0)
    {
	x = TRUE;
	もし (name[5] == '-'
		&& STRLEN(name) >= 11
		&& (name[6] >= '1' && name[6] <= '9'))
	{
	    char	*end;
	    整数	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (整数)strtoul((char *)name + 6, &end, 10);
	    もし (*end == '.' && vim_isdigit(end[1])
		    && end[2] == '.' && vim_isdigit(end[3]))
	    {
		minor = atoi(end + 1);

		// Expect "patch-9.9.01234".
		n = (major < VIM_VERSION_MAJOR
		     || (major == VIM_VERSION_MAJOR
			 && (minor < VIM_VERSION_MINOR
			     || (minor == VIM_VERSION_MINOR
				 && has_patch(atoi(end + 3))))));
	    }
	}
	否则 もし (SAFE_isdigit(name[5]))
	    n = has_patch(atoi((char *)name + 5));
    }
    否则 もし (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    否则 もし (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_input_isatty();
    }
    否则 もし (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    否则 もし (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    否则 もし (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#ifdef FEAT_GUI
	n = (gui.in_use || gui.starting);
#endif
    }
    否则 もし (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#もし defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.in_use;	// gui_mch_browse() works when GUI is running
#endif
    }
    否则 もし (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#ifdef FEAT_SYN_HL
	n = syntax_present(curwin);
#endif
    }
    否则 もし (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#ifdef FEAT_VTP
	n = is_term_win32() && has_vtp_working();
#endif
    }
    否则 もし (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_NETBEANS_INTG
	n = netbeans_active();
#endif
    }
    否则 もし (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_MOUSE_GPM
	n = gpm_enabled();
#endif
    }
    否则 もし (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#もし defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#endif
    }
    否则 もし (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_star.available;
#endif
    }
    否则 もし (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# もし defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	もし (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	否则
	    n = FALSE;
# 否则
	n = FALSE;
# endif
#endif
    }

    // Look up in has_list[] only if not already handled above.
    もし (x == FALSE)
    {
	ために (i = 0; has_list[i].name != NULL; ++i)
	    もし (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		break;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	もし (x == TRUE && n == FALSE)
	{
	    もし (0)
	    {
		// intentionally empty
	    }
#ifdef VIMDLL
	    否则 もし (STRICMP(name, "filterpipe") == 0)
		n = gui.in_use || gui.starting;
#endif
#もし defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    否则 もし (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#endif
#ifdef DYNAMIC_LUA
	    否则 もし (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#endif
#ifdef DYNAMIC_MZSCHEME
	    否则 もし (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#endif
#ifdef DYNAMIC_PERL
	    否则 もし (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON
	    否则 もし (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON3
	    否则 もし (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#endif
#もし defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    否则 もし (STRICMP(name, "pythonx") == 0)
	    {
# もし defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		もし (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		否则 もし (p_pyx == 3)
		    n = python3_enabled(FALSE);
		否则 もし (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#endif
#ifdef DYNAMIC_RUBY
	    否则 もし (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#endif
#ifdef DYNAMIC_TCL
	    否则 もし (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#endif
#ifdef DYNAMIC_SODIUM
	    否则 もし (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#endif
#もし defined(FEAT_TERMINAL) && defined(MSWIN)
	    否则 もし (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#endif
#ifdef DYNAMIC_GPM
	    否则 もし (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_available();
#endif
	}
    }

    もし (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_number = x;
    否则
	// return whether feature is enabled
	rettv->vval.v_number = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    整数
dynamic_feature(char_u *feature)
{
    戻る (feature == NULL
#もし defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.in_use)
#endif
#ifdef VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#endif
#もし defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.in_use)
#endif
#もし defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    || STRICMP(feature, "iconv") == 0
#endif
#ifdef DYNAMIC_LUA
	    || STRICMP(feature, "lua") == 0
#endif
#ifdef FEAT_MOUSE_GPM
	    || (STRICMP(feature, "mouse_gpm_enabled") == 0 && !gpm_enabled())
#endif
#ifdef DYNAMIC_MZSCHEME
	    || STRICMP(feature, "mzscheme") == 0
#endif
#ifdef FEAT_NETBEANS_INTG
	    || STRICMP(feature, "netbeans_enabled") == 0
#endif
#ifdef DYNAMIC_PERL
	    || STRICMP(feature, "perl") == 0
#endif
#ifdef DYNAMIC_PYTHON
	    || STRICMP(feature, "python") == 0
#endif
#ifdef DYNAMIC_PYTHON3
	    || STRICMP(feature, "python3") == 0
#endif
#もし defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    || STRICMP(feature, "pythonx") == 0
#endif
#ifdef DYNAMIC_RUBY
	    || STRICMP(feature, "ruby") == 0
#endif
#ifdef FEAT_SYN_HL
	    || STRICMP(feature, "syntax_items") == 0
#endif
#ifdef DYNAMIC_TCL
	    || STRICMP(feature, "tcl") == 0
#endif
	    // once "starting" is zero it will stay that way
	    || (STRICMP(feature, "vim_starting") == 0 && starting != 0)
	    || STRICMP(feature, "multi_byte_encoding") == 0
#もし defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#endif
	    );
}

/*
 * "haslocaldir()" function
 */
    static void
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = NULL;
    win_T	*wp = NULL;

    もし (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	戻る;

    wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    もし (wp != NULL && wp->w_localdir != NULL)
	rettv->vval.v_number = 1;
    否则 もし (tp != NULL && tp->tp_localdir != NULL)
	rettv->vval.v_number = 2;
    否则
	rettv->vval.v_number = 0;
}

/*
 * "highlightID(name)" function
 */
    static void
f_hlID(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    rettv->vval.v_number = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    static void
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    rettv->vval.v_number = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    static void
f_hostname(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u hostname[256];

    mch_get_host_name(hostname, 256);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(hostname);
}

/*
 * "id()" function
 * Identity. Return address of item as a hex string, %p format.
 * Currently only valid for object/container types.
 * Return empty string if not an object.
 */
#ifdef VMS // VMS does not have defined uintptr_t
# もし defined(HAVE_NO_LONG_LONG)
typedef unsigned 整数 uintptr_t;
# 否则
typedef unsigned long long uintptr_t;
# endif
#endif // VMS

    static void
f_id(typval_T *argvars, typval_T *rettv)
{
    char    numbuf[NUMBUFLEN];
    char    *p = numbuf;

    switch (argvars[0].v_type)
    {
	case VAR_LIST:
	case VAR_TUPLE:
	case VAR_DICT:
	case VAR_OBJECT:
	case VAR_JOB:
	case VAR_CHANNEL:
	case VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    もし (argvars[0].vval.v_object != NULL)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(void *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		ために (; p < numbuf + sizeof(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	default:
	    break;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    static void
index_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    整数		start = 0;
    整数		error = FALSE;
    整数		ic = FALSE;

    b = argvars[0].vval.v_blob;
    もし (b == NULL)
	戻る;

    もし (argvars[2].v_type != VAR_UNKNOWN)
    {
	start = tv_get_number_chk(&argvars[2], &error);
	もし (error)
	    戻る;
    }

    もし (start < 0)
    {
	start = blob_len(b) + start;
	もし (start < 0)
	    start = 0;
    }

    ために (整数 idx = start; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_NUMBER;
	tv.vval.v_number = blob_get(b, idx);
	もし (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_number = idx;
	    戻る;
	}
    }
}

/*
 * index() function for a tuple
 */
    static void
index_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    整数		ic = FALSE;
    整数		error = FALSE;

    もし (tuple == NULL)
	戻る;

    整数	start_idx = 0;
    もし (argvars[2].v_type != VAR_UNKNOWN)
    {
	start_idx = tv_get_number_chk(&argvars[2], &error);
	もし (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (整数)tv_get_bool_chk(&argvars[3], &error);
	もし (error)
	    戻る;
    }

    rettv->vval.v_number = index_tuple(tuple, &argvars[1], start_idx, ic);
}

/*
 * index() function for a list
 */
    static void
index_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*item;
    long	idx = 0;
    整数		ic = FALSE;
    整数		error = FALSE;

    l = argvars[0].vval.v_list;
    もし (l == NULL)
	戻る;

    CHECK_LIST_MATERIALIZE(l);
    item = l->lv_first;
    もし (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, (long)tv_get_number_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	もし (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (整数)tv_get_bool_chk(&argvars[3], &error);
	もし (error)
	    item = NULL;
    }

    ために ( ; item != NULL; item = item->li_next, ++idx)
	もし (tv_equal(&item->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_number = idx;
	    break;
	}
}

/*
 * "index()" function
 */
    static void
f_index(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = -1;

    もし (in_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_number_arg(argvars, 1) == FAIL)
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	戻る;

    もし (argvars[0].v_type == VAR_BLOB)
	index_func_blob(argvars, rettv);
    否则 もし (argvars[0].v_type == VAR_TUPLE)
	index_func_tuple(argvars, rettv);
    否则 もし (argvars[0].v_type == VAR_LIST)
	index_func_list(argvars, rettv);
    否则
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    整数
indexof_eval_expr(typval_T *expr)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	found;
    整数		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_KEY);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    もし (eval_expr_typval(expr, FALSE, argv, 2, NULL, &newtv) == FAIL)
	戻る FALSE;

    found = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    戻る error ? FALSE : found;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    static 整数
indexof_blob(blob_T *b, long startidx, typval_T *expr)
{
    long	idx = 0;

    もし (b == NULL)
	戻る -1;

    もし (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	もし (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_KEY, VAR_NUMBER);
    set_vim_var_type(VV_VAL, VAR_NUMBER);

    整数		called_emsg_start = called_emsg;
    ために (idx = startidx; idx < blob_len(b); ++idx)
    {
	set_vim_var_nr(VV_KEY, idx);
	set_vim_var_nr(VV_VAL, blob_get(b, idx));

	もし (indexof_eval_expr(expr))
	    戻る idx;

	もし (called_emsg != called_emsg_start)
	    戻る -1;
    }

    戻る -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    static 整数
indexof_list(list_T *l, long startidx, typval_T *expr)
{
    listitem_T	*item;
    long	idx = 0;
    整数		found;

    もし (l == NULL)
	戻る -1;

    CHECK_LIST_MATERIALIZE(l);

    もし (startidx == 0)
	item = l->lv_first;
    否则
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, startidx);
	もし (item != NULL)
	    idx = l->lv_u.mat.lv_idx;
    }

    set_vim_var_type(VV_KEY, VAR_NUMBER);

    整数		called_emsg_start = called_emsg;
    ために ( ; item != NULL; item = item->li_next, ++idx)
    {
	set_vim_var_nr(VV_KEY, idx);
	copy_tv(&item->li_tv, get_vim_var_tv(VV_VAL));

	found = indexof_eval_expr(expr);
	clear_tv(get_vim_var_tv(VV_VAL));

	もし (found)
	    戻る idx;

	もし (called_emsg != called_emsg_start)
	    戻る -1;
    }

    戻る -1;
}

/*
 * "indexof()" function
 */
    static void
f_indexof(typval_T *argvars, typval_T *rettv)
{
    long	startidx = 0;
    typval_T	save_val;
    typval_T	save_key;
    整数		save_did_emsg;

    rettv->vval.v_number = -1;

    もし (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	戻る;

    もし ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == NULL
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == NULL))
	戻る;

    もし (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_number_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_KEY, &save_key);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    もし (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_number = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    否则 もし (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_number = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    否则
	rettv->vval.v_number = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_KEY, &save_key);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

static 整数 inputsecret_flag = 0;

/*
 * "input()" function
 *     Also handles inputsecret() when inputsecret is set.
 */
    static void
f_input(typval_T *argvars, typval_T *rettv)
{
    get_user_input(argvars, rettv, FALSE, inputsecret_flag);
}

/*
 * "inputdialog()" function
 */
    static void
f_inputdialog(typval_T *argvars, typval_T *rettv)
{
#もし defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    もし (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == NULL)
    {
	char_u	*message;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	もし (in_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    戻る;

	message = tv_get_string_chk(&argvars[0]);
	もし (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != NULL)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	否则
	    IObuff[0] = NUL;
	もし (message != NULL && defstr != NULL
		&& do_dialog(VIM_QUESTION, NULL, message,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	否则
	{
	    もし (message != NULL && defstr != NULL
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    否则
		rettv->vval.v_string = NULL;
	}
	rettv->v_type = VAR_STRING;
    }
    否则
#endif
	get_user_input(argvars, rettv, TRUE, inputsecret_flag);
}

/*
 * "inputlist()" function
 */
    static void
f_inputlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    整数		selected;
    整数		mouse_used;

#ifdef NO_CONSOLE_INPUT
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    もし (no_console_input() && !is_not_a_term())
	戻る;
#endif
    もし (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == NULL)
    {
	semsg(_(e_argument_of_str_must_be_list), "inputlist()");
	戻る;
    }

    msg_start();
    msg_row = Rows - 1;	// for when 'cmdheight' > 1
    lines_left = Rows;	// avoid more prompt
    msg_scroll = TRUE;
    msg_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	msg_puts((char *)tv_get_string(&li->li_tv));
	msg_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_number(&mouse_used);
    もし (mouse_used)
	selected -= lines_left;

    rettv->vval.v_number = selected;
}

static garray_T	    ga_userinput = {0, 0, sizeof(tasave_T), 4, NULL};

/*
 * "inputrestore()" function
 */
    static void
f_inputrestore(typval_T *argvars UNUSED, typval_T *rettv)
{
    もし (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_data)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    否则 もし (p_verbose > 1)
    {
	verb_msg(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_number = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    static void
f_inputsave(typval_T *argvars UNUSED, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    もし (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_data)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    否则
	rettv->vval.v_number = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    static void
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	戻る;

    ++cmdline_star;
    ++inputsecret_flag;
    f_input(argvars, rettv);
    --cmdline_star;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    static void
f_interrupt(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    static void
f_invert(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	戻る;

    rettv->vval.v_number = ~tv_get_number_chk(&argvars[0], NULL);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    static void
free_lval_root(lval_root_T *root)
{
    もし (root->lr_tv != NULL)
	free_tv(root->lr_tv);
    class_unref(root->lr_cl_exec);
    root->lr_tv = NULL;
    root->lr_cl_exec = NULL;
}

/*
 * This is used if executing in a method, the argument string is a
 * variable/item expr/reference. It may start with a potential class/object
 * variable.
 *
 * Adjust "root" as needed; lr_tv may be changed or freed.
 *
 * Always returns OK.
 * Free resources and return FAIL if the root should not be used. Otherwise OK.
 */

    static 整数
fix_variable_reference_lval_root(lval_root_T *root, char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    整数 found_member = FALSE;
    もし (root->lr_tv->v_type == VAR_OBJECT)
    {
	もし (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    found_member = TRUE;
    }
    もし (!found_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*end;
	ために (end = name; ASCII_ISALNUM(*end) || *end == '_'; ++end)
	    ;
	整数 idx = class_member_idx(root->lr_cl_exec, name, end - name);
	もし (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(root->lr_tv);
	    copy_tv(&root->lr_cl_exec->class_members_tv[idx], root->lr_tv);
	    found_member = TRUE;
	}
    }
    もし (!found_member)
    {
	free_tv(root->lr_tv);
	root->lr_tv = NULL;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    戻る OK;
}

/*
 * "islocked()" function
 */
    static void
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*end;
    dictitem_T	*di;

    rettv->vval.v_number = -1;

    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    char_u *name = tv_get_string(&argvars[0]);
#ifdef LOG_LOCKVAR
    ch_log(NULL, "LKVAR: f_islocked(): name: %s", name);
#endif

    lval_root_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_root_T *root = NULL;

    // Set up lval_root if executing in a method.
    もし (fill_exec_lval_root(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	もし (fix_variable_reference_lval_root(&aroot, name) == OK)
	    root = &aroot;
    }

    lval_root_T	*lval_root_save = lval_root;
    lval_root = root;
    end = get_lval(name, NULL, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_START);
    lval_root = lval_root_save;

    もし (end != NULL && lv.ll_name != NULL)
    {
	もし (*end != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_end
		   ? e_invalid_argument_str : e_trailing_characters_str), end);
	}
	否则
	{
	    もし (lv.ll_tv == NULL)
	    {
		di = find_var(lv.ll_name, NULL, TRUE);
		もし (di != NULL)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_number = ((di->di_flags & DI_FLAGS_LOCK)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    否则 もし (lv.ll_is_root)
	    {
		rettv->vval.v_number = tv_islocked(lv.ll_tv);
	    }
	    否则 もし (lv.ll_object != NULL)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_number = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(NULL, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#endif
	    }
	    否则 もし (lv.ll_class != NULL)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_number = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(NULL, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#endif
	    }
	    否则 もし (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    否则 もし (lv.ll_newkey != NULL)
		semsg(_(e_key_not_present_in_dictionary_str), lv.ll_newkey);
	    否则 もし (lv.ll_list != NULL)
		// List item.
		rettv->vval.v_number = tv_islocked(&lv.ll_li->li_tv);
	    否则 もし (lv.ll_tuple != NULL)
		// Tuple item.
		rettv->vval.v_number = tv_islocked(lv.ll_tv);
	    否则
		// Dictionary item.
		rettv->vval.v_number = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    もし (root != NULL)
	free_lval_root(root);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    static void
f_items(typval_T *argvars, typval_T *rettv)
{
    もし (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    否则 もし (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    否则 もし (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    否则 もし (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    否则 もし (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    否则
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), 1);
}

/*
 * "keytrans()" function
 */
    static void
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    もし (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == NULL)
	戻る;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    static void
f_last_buffer_nr(typval_T *argvars UNUSED, typval_T *rettv)
{
    整数		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	もし (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_number = n;
}

/*
 * "len()" function
 */
    void
f_len(typval_T *argvars, typval_T *rettv)
{
    switch (argvars[0].v_type)
    {
	case VAR_STRING:
	case VAR_NUMBER:
	    rettv->vval.v_number = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    break;
	case VAR_BLOB:
	    rettv->vval.v_number = blob_len(argvars[0].vval.v_blob);
	    break;
	case VAR_LIST:
	    rettv->vval.v_number = list_len(argvars[0].vval.v_list);
	    break;
	case VAR_TUPLE:
	    rettv->vval.v_number = tuple_len(argvars[0].vval.v_tuple);
	    break;
	case VAR_DICT:
	    rettv->vval.v_number = dict_len(argvars[0].vval.v_dict);
	    break;
	case VAR_OBJECT:
	    rettv->vval.v_number = object_len(argvars[0].vval.v_object);
	    break;
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	case VAR_BOOL:
	case VAR_SPECIAL:
	case VAR_FLOAT:
	case VAR_FUNC:
	case VAR_PARTIAL:
	case VAR_JOB:
	case VAR_CHANNEL:
	case VAR_INSTR:
	case VAR_CLASS:
	case VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    break;
    }
}

    static void
libcall_common(typval_T *argvars UNUSED, typval_T *rettv, 整数 type)
{
#ifdef FEAT_LIBCALL
    char_u		*string_in;
    char_u		**string_result;
    整数			nr_result;
#endif

    rettv->v_type = type;
    もし (type != VAR_NUMBER)
	rettv->vval.v_string = NULL;

    もし (check_restricted() || check_secure())
	戻る;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_number_arg(argvars, 2) == FAIL))
	戻る;

#ifdef FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    もし (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_in = NULL;
	もし (argvars[2].v_type == VAR_STRING)
	    string_in = argvars[2].vval.v_string;
	もし (type == VAR_NUMBER)
	{
	    string_result = NULL;
	}
	否则
	{
	    rettv->vval.v_string = NULL;
	    string_result = &rettv->vval.v_string;
	}
	もし (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_in,
			     argvars[2].vval.v_number,
			     string_result,
			     &nr_result) == OK
		&& type == VAR_NUMBER)
	    rettv->vval.v_number = nr_result;
    }
#endif
}

/*
 * "libcall()" function
 */
    static void
f_libcall(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_STRING);
}

/*
 * "libcallnr()" function
 */
    static void
f_libcallnr(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_NUMBER);
}

/*
 * "line(string, [winid])" function
 */
    static void
f_line(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum = 0;
    pos_T	*fp = NULL;
    整数		fnum;
    整数		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	戻る;

    もし (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (整数)tv_get_number(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	もし (wp != NULL && tp != NULL)
	{
	    もし (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		もし (*p_spk != 'c'
#ifdef FEAT_DIFF
		|| (curwin->w_p_diff && switchwin.sw_curwin->w_p_diff)
#endif
		)
		    skip_update_topline = TRUE;
		check_cursor();
		fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	    }
	    skip_update_topline = FALSE;
	    restore_win_noblock(&switchwin, TRUE);
	}
    }
    否则
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    もし (fp != NULL)
	lnum = fp->lnum;
    rettv->vval.v_number = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    static void
f_line2byte(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_number = -1;
#否则
    linenr_T	lnum;

    もし (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	戻る;

    lnum = tv_get_lnum(argvars);
    もし (lnum < 1 || lnum > curbuf->b_ml.ml_line_count + 1)
	rettv->vval.v_number = -1;
    否则
	rettv->vval.v_number = ml_find_line_or_offset(curbuf, lnum, NULL);
    もし (rettv->vval.v_number >= 0)
	++rettv->vval.v_number;
#endif
}

#ifdef FEAT_LUA
/*
 * "luaeval()" function
 */
    static void
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    もし (check_restricted() || check_secure())
	戻る;

    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#endif

typedef enum
{
    MATCH_END,	    // matchend()
    MATCH_MATCH,    // match()
    MATCH_STR,	    // matchstr()
    MATCH_LIST,	    // matchlist()
    MATCH_POS	    // matchstrpos()
} matchtype_T;

    static void
find_some_match(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = NULL;
    long	len = 0;
    char_u	*expr = NULL;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    long	start = 0;
    long	nth = 1;
    colnr_T	startcol = 0;
    整数		match = 0;
    list_T	*l = NULL;
    listitem_T	*li = NULL;
    long	idx = 0;
    整数		prev_lock = 0;
    char_u	*tofree = NULL;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    rettv->vval.v_number = -1;
    もし (type == MATCH_LIST || type == MATCH_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	もし (rettv_list_alloc(rettv) == FAIL)
	    goto theend;
	もし (type == MATCH_POS
		&& (list_append_string(rettv->vval.v_list,
					    (char_u *)"", 0) == FAIL
		    || list_append_number(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_number(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_number(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL))
	{
		list_free(rettv->vval.v_list);
		rettv->vval.v_list = NULL;
		goto theend;
	}
    }
    否则 もし (type == MATCH_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = NULL;
    }

    もし (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 3) == FAIL)))
	goto theend;

    もし (argvars[0].v_type == VAR_LIST)
    {
	もし ((l = argvars[0].vval.v_list) == NULL)
	    goto theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    否则
    {
	expr = str = tv_get_string(&argvars[0]);
	len = (long)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    もし (pat == NULL)
	goto theend;

    もし (argvars[2].v_type != VAR_UNKNOWN)
    {
	整数	    error = FALSE;

	start = (long)tv_get_number_chk(&argvars[2], &error);
	もし (error)
	    goto theend;
	もし (l != NULL)
	{
	    li = list_find(l, start);
	    もし (li == NULL)
		goto theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	否则
	{
	    もし (start < 0)
		start = 0;
	    もし (start > len)
		goto theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    もし (argvars[3].v_type != VAR_UNKNOWN)
		startcol = start;
	    否则
	    {
		str += start;
		len -= start;
	    }
	}

	もし (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (long)tv_get_number_chk(&argvars[3], &error);
	もし (error)
	    goto theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    もし (regmatch.regprog != NULL)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	もし (l != NULL)
	{
	    prev_lock = l->lv_lock;
	    もし (l->lv_lock == 0)
		l->lv_lock = VAR_LOCKED;
	}

	ために (;;)
	{
	    もし (l != NULL)
	    {
		もし (li == NULL)
		{
		    match = FALSE;
		    break;
		}
		vim_free(tofree);
		expr = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		もし (str == NULL)
		    break;
	    }

	    match = vim_regexec_nl(&regmatch, str, startcol);

	    もし (match && --nth <= 0)
		break;
	    もし (l == NULL && !match)
		break;

	    // Advance to just after the match.
	    もし (l != NULL)
	    {
		li = li->li_next;
		++idx;
	    }
	    否则
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		もし (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    match = FALSE;
		    break;
		}
	    }
	}

	もし (match)
	{
	    もし (type == MATCH_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_first;
		listitem_T *li2 = li1->li_next;
		listitem_T *li3 = li2->li_next;
		listitem_T *li4 = li3->li_next;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_number =
				      (varnumber_T)(regmatch.startp[0] - expr);
		li4->li_tv.vval.v_number =
					(varnumber_T)(regmatch.endp[0] - expr);
		もし (l != NULL)
		    li2->li_tv.vval.v_number = (varnumber_T)idx;
	    }
	    否则 もし (type == MATCH_LIST)
	    {
		整数 i;

		// return list with matched string and submatches
		ために (i = 0; i < NSUBEXP; ++i)
		{
		    もし (regmatch.endp[i] == NULL)
		    {
			もし (list_append_string(rettv->vval.v_list,
							    NULL, 0) == FAIL)
			    break;
		    }
		    否则 もし (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(整数)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			break;
		}
	    }
	    否则 もし (type == MATCH_STR)
	    {
		// return matched string
		もし (l != NULL)
		    copy_tv(&li->li_tv, rettv);
		否则
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    否则 もし (l != NULL)
		rettv->vval.v_number = idx;
	    否则
	    {
		もし (type != MATCH_END)
		    rettv->vval.v_number =
				      (varnumber_T)(regmatch.startp[0] - str);
		否则
		    rettv->vval.v_number =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_number += (varnumber_T)(str - expr);
	    }
	}
	もし (l != NULL)
	    l->lv_lock = prev_lock;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    もし (type == MATCH_POS && l == NULL && rettv->vval.v_list != NULL)
	// matchstrpos() without a list: drop the second item.
	listitem_remove(rettv->vval.v_list,
				       rettv->vval.v_list->lv_first->li_next);
    vim_free(tofree);
    p_cpo = save_cpo;
}

/*
 * Return all the matches in string "str" for pattern "rmp".
 * The matches are returned in the List "mlist".
 * If "submatches" is TRUE, then submatch information is also returned.
 * "matchbuf" is TRUE when called for matchbufline().
 */
    static 整数
get_matches_in_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    整数		idx,
    整数		submatches,
    整数		matchbuf)
{
    long	len = (long)STRLEN(str);
    整数		match = 0;
    colnr_T	startidx = 0;

    ために (;;)
    {
	match = vim_regexec_nl(rmp, str, startidx);
	もし (!match)
	    break;

	dict_T *d = dict_alloc();
	もし (d == NULL)
	    戻る FAIL;
	もし (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    戻る FAIL;
	}

	もし (dict_add_number(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    戻る FAIL;

	もし (dict_add_number(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    戻る FAIL;

	もし (dict_add_string_len(d, "text", rmp->startp[0],
		    (整数)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    戻る FAIL;

	もし (submatches)
	{
	    list_T *sml = list_alloc();
	    もし (sml == NULL)
		戻る FAIL;

	    もし (dict_add_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		戻る FAIL;
	    }

	    // return a list with the submatches
	    ために (整数 i = 1; i < NSUBEXP; ++i)
	    {
		もし (rmp->endp[i] == NULL)
		{
		    もし (list_append_string(sml, (char_u *)"", 0) == FAIL)
			戻る FAIL;
		}
		否则 もし (list_append_string(sml, rmp->startp[i],
			    (整数)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    戻る FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	もし (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    break;
    }

    戻る OK;
}

/*
 * "matchbufline()" function
 */
    static void
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = NULL;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_number = -1;
    もし (rettv_list_alloc(rettv) != OK)
	戻る;
    retlist = rettv->vval.v_list;

    もし (check_for_buffer_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	戻る;

    整数 prev_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    もし (buf == NULL)
    {
	もし (did_emsg == prev_did_emsg)
	    semsg(_(e_invalid_buffer_name_str), tv_get_string(&argvars[0]));
	戻る;
    }
    もし (buf->b_ml.ml_mfp == NULL)
    {
	emsg(_(e_buffer_is_not_loaded));
	戻る;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    整数		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    もし (did_emsg > did_emsg_before)
	戻る;
    もし (slnum < 1)
    {
	semsg(_(e_invalid_value_for_argument_str), "lnum");
	戻る;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    もし (did_emsg > did_emsg_before)
	戻る;
    もし (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_value_for_argument_str), "end_lnum");
	戻る;
    }

    もし (elnum > buf->b_ml.ml_line_count)
	elnum = buf->b_ml.ml_line_count;

    整数		submatches = FALSE;
    もし (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	もし (d != NULL)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    もし (di != NULL)
	    {
		もし (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "submatches");
		    戻る;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    もし (regmatch.regprog == NULL)
	goto theend;
    regmatch.rm_ic = p_ic;

    ながら (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	もし (get_matches_in_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    goto cleanup;
	slnum++;
    }

cleanup:
    eval_regfree(pat, regmatch.regprog);

theend:
    p_cpo = save_cpo;
}

/*
 * "match()" function
 */
    static void
f_match(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_MATCH);
}

/*
 * "matchend()" function
 */
    static void
f_matchend(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_END);
}

/*
 * "matchlist()" function
 */
    static void
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_LIST);
}

/*
 * "matchstr()" function
 */
    static void
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_STR);
}

/*
 * "matchstrlist()" function
 */
    static void
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = NULL;
    char_u	*save_cpo;
    list_T	*l = NULL;
    listitem_T	*li = NULL;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_number = -1;
    もし (rettv_list_alloc(rettv) != OK)
	戻る;
    retlist = rettv->vval.v_list;

    もし (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	戻る;

    もし ((l = argvars[0].vval.v_list) == NULL)
	戻る;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    もし (pat == NULL)
	戻る;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    もし (regmatch.regprog == NULL)
	goto theend;
    regmatch.rm_ic = p_ic;

    整数		submatches = FALSE;
    もし (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	もし (d != NULL)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    もし (di != NULL)
	    {
		もし (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_value_for_argument_str), "submatches");
		    goto cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    整数 idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	もし (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != NULL)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    もし (get_matches_in_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		goto cleanup;
	}
	idx++;
    }

cleanup:
    eval_regfree(pat, regmatch.regprog);

theend:
    p_cpo = save_cpo;
}

/*
 * "matchstrpos()" function
 */
    static void
f_matchstrpos(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_POS);
}

    static void
max_min(typval_T *argvars, typval_T *rettv, 整数 domax)
{
    varnumber_T	n = 0;

    もし (in_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	戻る;

    rettv->vval.v_number = 0;

    もし (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = NULL;

	l = argvars[0].vval.v_list;
	もし (l != NULL && l->lv_len > 0)
	{
	    もし (l->lv_first == &range_list_item)
	    {
		もし ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_start;
		否则
		    n = l->lv_u.nonmat.lv_start + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_number = n;
	    }
	    否则
	    {
		FOR_ALL_LIST_ITEMS(l, li)
		{
		    もし (tv == NULL)
			tv = &li->li_tv;
		    否则
		    {
			整数 res;
			もし (typval_compare2(&li->li_tv, tv,
				domax ? EXPR_GREATER : EXPR_SMALLER, FALSE, &res) == FAIL)
			    戻る;
			もし (res == OK)
			    tv = &li->li_tv;
		    }
		}

		もし (tv != NULL)
		    copy_tv(tv, rettv);
	    }
	}
    }
    否则 もし (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	もし (tv != NULL)
	    copy_tv(tv, rettv);
    }
    否则 もし (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	整数		todo;
	typval_T	*tv = NULL;

	d = argvars[0].vval.v_dict;
	もし (d != NULL)
	{
	    todo = (整数)d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
	    {
		もし (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    もし (tv == NULL)
			tv = &HI2DI(hi)->di_tv;
		    否则
		    {
			整数 res;
			もし (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? EXPR_GREATER : EXPR_SMALLER, FALSE, &res) == FAIL)
			    戻る;
			もし (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	もし (tv != NULL)
	    copy_tv(tv, rettv);
    }
    否则
	semsg(_(e_argument_of_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
}

/*
 * "max()" function
 */
    static void
f_max(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, TRUE);
}

/*
 * "min()" function
 */
    static void
f_min(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, FALSE);
}

#もし defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    static void
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    もし (check_restricted() || check_secure())
	戻る;

    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    void
mzscheme_call_vim(char_u *name, typval_T *args, typval_T *rettv)
{
    typval_T argvars[3];

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = name;
    copy_tv(args, &argvars[1]);
    argvars[2].v_type = VAR_UNKNOWN;
    f_call(argvars, rettv);
    clear_tv(&argvars[1]);
}
#endif

/*
 * "nextnonblank()" function
 */
    static void
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    もし (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	戻る;

    ために (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	もし (lnum < 0 || lnum > curbuf->b_ml.ml_line_count)
	{
	    lnum = 0;
	    break;
	}
	もし (*skipwhite(ml_get(lnum)) != NUL)
	    break;
    }
    rettv->vval.v_number = lnum;
}


/*
 * "ngettext()" function
 */
    static void
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#もし defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    char *prev = NULL;
#endif

    もし (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_number_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	戻る;

    rettv->v_type = VAR_STRING;

    もし (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != NULL &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#もし defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((const char *)argvars[3].vval.v_string, (char *)p_enc);
#endif

#もし defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((const char *)argvars[3].vval.v_string, (const char *)argvars[0].vval.v_string, (const char *)argvars[1].vval.v_string, (整数)argvars[2].vval.v_number));
#否则
	textdomain((const char *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((const char *)argvars[0].vval.v_string, (const char *)argvars[1].vval.v_string, argvars[2].vval.v_number));
	textdomain(VIMPACKAGE);
#endif

#もし defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	もし (prev != NULL)
	    bind_textdomain_codeset((const char *)argvars[3].vval.v_string, prev);
#endif
    }
    否则
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((const char *)argvars[0].vval.v_string, (const char *)argvars[1].vval.v_string, argvars[2].vval.v_number));
}


/*
 * "nr2char()" function
 */
    static void
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	戻る;

    もし (has_mbyte)
    {
	整数	utf8 = 0;

	もし (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (整数)tv_get_bool_chk(&argvars[1], NULL);
	もし (utf8)
	    buf[utf_char2bytes((整数)tv_get_number(&argvars[0]), buf)] = NUL;
	否则
	    buf[(*mb_char2bytes)((整数)tv_get_number(&argvars[0]), buf)] = NUL;
    }
    否则
    {
	buf[0] = (char_u)tv_get_number(&argvars[0]);
	buf[1] = NUL;
    }
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "or(expr, expr)" function
 */
    static void
f_or(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	戻る;

    rettv->vval.v_number = tv_get_number_chk(&argvars[0], NULL)
					| tv_get_number_chk(&argvars[1], NULL);
}

#ifdef FEAT_PERL
/*
 * "perleval()" function
 */
    static void
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    もし (check_restricted() || check_secure())
	戻る;

    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#endif

/*
 * "prevnonblank()" function
 */
    static void
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    もし (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	戻る;

    lnum = tv_get_lnum(argvars);
    もし (lnum < 1 || lnum > curbuf->b_ml.ml_line_count)
	lnum = 0;
    否则
	ながら (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
	    --lnum;
    rettv->vval.v_number = lnum;
}

// This dummy va_list is here because:
// - passing a NULL pointer doesn't work when va_list isn't a pointer
// - locally in the function results in a "used before set" warning
// - using va_start() to initialize it gives "function with fixed args" error
static va_list	ap;

/*
 * "printf()" function
 */
    static void
f_printf(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    整数		len;
    char_u	*s;
    整数		saved_did_emsg = did_emsg;
    char	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    もし (in_vim9script() && check_for_string_or_number_arg(argvars, 0) == FAIL)
	戻る;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (char *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(NULL, 0, fmt, ap, argvars + 1);
    もし (!did_emsg)
    {
	s = alloc(len + 1);
	もし (s != NULL)
	{
	    rettv->vval.v_string = s;
	    (void)vim_vsnprintf_typval((char *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    static void
f_pum_getpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    もし (rettv_dict_alloc(rettv) == FAIL)
	戻る;
    pum_set_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    static void
f_pumvisible(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    もし (pum_visible())
	rettv->vval.v_number = 1;
}

#ifdef FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    static void
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    もし (check_restricted() || check_secure())
	戻る;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	戻る;

    もし (p_pyx == 0)
	p_pyx = 3;

    もし (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    否则 もし (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	戻る;
    }
    否则
    {
	locals = NULL;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#endif

#ifdef FEAT_PYTHON
/*
 * "pyeval()" function
 */
    static void
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    もし (check_restricted() || check_secure())
	戻る;

    もし (in_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	戻る;

    もし (p_pyx == 0)
	p_pyx = 2;

    もし (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    否则 もし (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	戻る;
    }
    否则
    {
	locals = NULL;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#endif

#もし defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    static void
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    もし (check_restricted() || check_secure())
	戻る;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	戻る;

# もし defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    もし (p_pyx == 2)
	f_pyeval(argvars, rettv);
    否则
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#endif

static UINT32_T srand_seed_for_testing = 0;
static 整数	srand_seed_for_testing_is_used = FALSE;

    static void
f_test_srand_seed(typval_T *argvars, typval_T *rettv UNUSED)
{
    もし (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type == VAR_UNKNOWN)
	srand_seed_for_testing_is_used = FALSE;
    否则
    {
	srand_seed_for_testing = (UINT32_T)tv_get_number(&argvars[0]);
	srand_seed_for_testing_is_used = TRUE;
    }
}

    static void
init_srand(UINT32_T *x)
{
    struct {
	union {
	    UINT32_T number;
	    char_u   bytes[sizeof(UINT32_T)];
	} contents;
    } buf;

    もし (srand_seed_for_testing_is_used)
    {
	*x = srand_seed_for_testing;
	戻る;
    }

    もし (mch_get_random(buf.contents.bytes, sizeof(buf.contents.bytes)) == OK)
    {
	*x = buf.contents.number;
	戻る;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#もし defined(FEAT_SODIUM)
    もし (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    否则
#endif
    {
#もし defined(FEAT_RELTIME)
	proftime_T res;
	profile_start(&res);
# もし defined(MSWIN)
	*x = (UINT32_T)res.LowPart;
# 否则
	*x = (UINT32_T)res.tv_fsec;
# endif
#否则
	*x = vim_time();
#endif
	*x ^= mch_get_pid();
    }
}

#define ROTL(x, k) (((x) << (k)) | ((x) >> (32 - (k))))
#define SPLITMIX32(x, z) ( \
    (z) = ((x) += 0x9e3779b9), \
    (z) = ((z) ^ ((z) >> 16)) * 0x85ebca6b, \
    (z) = ((z) ^ ((z) >> 13)) * 0xc2b2ae35, \
    (z) ^ ((z) >> 16) \
    )
#define SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w) \
    result = ROTL((y) * 5, 7) * 9; \
    t = (y) << 9; \
    (z) ^= (x); \
    (w) ^= (y); \
    (y) ^= (z), (x) ^= (w); \
    (z) ^= t; \
    (w) = ROTL(w, 11);

/*
 * "rand()" function
 */
    static void
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = NULL;
    static UINT32_T	gx, gy, gz, gw;
    static 整数	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, result;

    もし (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type == VAR_UNKNOWN)
    {
	// When no argument is given use the global seed list.
	もし (initialized == FALSE)
	{
	    // Initialize the global seed list.
	    init_srand(&x);

	    gx = SPLITMIX32(x, z);
	    gy = SPLITMIX32(x, z);
	    gz = SPLITMIX32(x, z);
	    gw = SPLITMIX32(x, z);
	    initialized = TRUE;
	}

	SHUFFLE_XOSHIRO128STARSTAR(gx, gy, gz, gw);
    }
    否则 もし (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	もし (l == NULL || list_len(l) != 4)
	    goto theend;

	lx = list_find(l, 0L);
	ly = list_find(l, 1L);
	lz = list_find(l, 2L);
	lw = list_find(l, 3L);
	もし (lx == NULL || ly == NULL || lz == NULL || lw == NULL)
	    goto theend;
	もし (lx->li_tv.v_type != VAR_NUMBER
		|| ly->li_tv.v_type != VAR_NUMBER
		|| lz->li_tv.v_type != VAR_NUMBER
		|| lw->li_tv.v_type != VAR_NUMBER)
	    goto theend;
	x = (UINT32_T)lx->li_tv.vval.v_number;
	y = (UINT32_T)ly->li_tv.vval.v_number;
	z = (UINT32_T)lz->li_tv.vval.v_number;
	w = (UINT32_T)lw->li_tv.vval.v_number;

	SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w);

	lx->li_tv.vval.v_number = (varnumber_T)x;
	ly->li_tv.vval.v_number = (varnumber_T)y;
	lz->li_tv.vval.v_number = (varnumber_T)z;
	lw->li_tv.vval.v_number = (varnumber_T)w;
    }
    否则
	goto theend;

    rettv->v_type = VAR_NUMBER;
    rettv->vval.v_number = (varnumber_T)result;
    戻る;

theend:
    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    rettv->v_type = VAR_NUMBER;
    rettv->vval.v_number = -1;
}

/*
 * "srand()" function
 */
    static void
f_srand(typval_T *argvars, typval_T *rettv)
{
    UINT32_T x = 0, z;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    もし (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    否则
    {
	整数	    error = FALSE;

	x = (UINT32_T)tv_get_number_chk(&argvars[0], &error);
	もし (error)
	    戻る;
    }

    list_append_number(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_number(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_number(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_number(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#undef ROTL
#undef SPLITMIX32
#undef SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    static void
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	start;
    varnumber_T	end;
    varnumber_T	stride = 1;
    整数		error = FALSE;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	戻る;

    start = tv_get_number_chk(&argvars[0], &error);
    もし (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = start - 1;
	start = 0;
    }
    否则
    {
	end = tv_get_number_chk(&argvars[1], &error);
	もし (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_number_chk(&argvars[2], &error);
    }

    もし (error)
	戻る;		// type error; errmsg already given
    もし (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	戻る;
    }

    // The stride is stored in "lv_stride", which is an int.
    もし (stride < INT_MIN || stride > INT_MAX)
    {
	char	buf[NUMBUFLEN];

	vim_snprintf(buf, sizeof(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	戻る;
    }

    uvarnumber_T	len;

    もし (stride > 0 ? end < start : end > start)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	back = stride > 0
				  ? (uvarnumber_T)start - (uvarnumber_T)end
				  : (uvarnumber_T)end - (uvarnumber_T)start;

	もし (back > 1)
	{
	    emsg(_(e_start_past_end));
	    戻る;
	}
	len = 0;
    }
    否则
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	span = stride > 0
				  ? (uvarnumber_T)end - (uvarnumber_T)start
				  : (uvarnumber_T)start - (uvarnumber_T)end;
	uvarnumber_T	count = span / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	もし (count >= (uvarnumber_T)INT_MAX)
	{
	    char	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, sizeof(buf), "%llu",
			      count < UVARNUM_MAX ? count + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    戻る;
	}
	len = count + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_first = &range_list_item;
    list->lv_u.nonmat.lv_start = start;
    list->lv_u.nonmat.lv_end = end;
    list->lv_u.nonmat.lv_stride = (整数)stride;
    list->lv_len = (整数)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    void
range_list_materialize(list_T *list)
{
    varnumber_T start = list->lv_u.nonmat.lv_start;
    varnumber_T end = list->lv_u.nonmat.lv_end;
    整数		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_first = NULL;
    list->lv_u.mat.lv_last = NULL;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_item = NULL;
    ために (i = start; stride > 0 ? i <= end : i >= end; i += stride)
    {
	もし (list_append_number(list, i) == FAIL)
	    break;
	もし (list->lv_lock & VAR_ITEMS_LOCKED)
	    list->lv_u.mat.lv_last->li_tv.v_lock = VAR_LOCKED;
    }
    list->lv_lock &= ~VAR_ITEMS_LOCKED;
}

/*
 * "getreginfo()" function
 */
    static void
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    整数		regname;
    char_u	buf[NUMBUFLEN + 2];
    size_t	buflen;
    long	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    もし (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	戻る;

    regname = getreg_get_regname(argvars);
    もし (regname == 0)
	戻る;

    もし (regname == '@')
	regname = '"';

    もし (rettv_dict_alloc(rettv) == FAIL)
	戻る;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_EXPR_SRC | GREG_LIST);
    もし (list == NULL)
	戻る;
    もし (dict_add_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    switch (get_reg_type(regname, &reglen))
    {
	case MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    break;
	case MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    break;
	case MBLOCK:
	    buflen = vim_snprintf_safelen((char *)buf, sizeof(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    break;
	default:
	    buf[0] = NUL;
	    buflen = 0;
	    break;
    }
    (void)dict_add_string_len(dict, (char *)"regtype", buf, (整数)buflen);

    buf[0] = get_register_name(get_unname_register());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    もし (regname == '"')
	(void)dict_add_string_len(dict, (char *)"points_to", buf, (整数)buflen);
    否则
    {
	dictitem_T	*item = dictitem_alloc((char_u *)"isunnamed");

	もし (item != NULL)
	{
	    item->di_tv.v_type = VAR_BOOL;
	    item->di_tv.vval.v_number = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (void)dict_add(dict, item);
	}
    }
}

    static void
return_register(整数 regname, typval_T *rettv)
{
    char_u buf[2] = {NUL, NUL};

    buf[0] = (char_u)regname;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(buf, (buf[0] == NUL) ? 0 : 1);
}

/*
 * "reg_executing()" function
 */
    static void
f_reg_executing(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    static void
f_reg_recording(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    static void
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = -1;
    もし (check_restricted() || check_secure())
	戻る;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	戻る;

    rettv->vval.v_number = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    static void
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    整数 slen = list_len(l);

    もし (rettv_list_alloc(rettv) == FAIL
	    || l == NULL
	    || n <= 0
	    || slen == 0)
	戻る;

    もし (check_repeat_count(slen, n) == FAIL)
	戻る;

    ながら (n-- > 0)
	もし (list_extend(rettv->vval.v_list, l, NULL) == FAIL)
	    break;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    static void
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    整数		slen;
    整数		len;
    整数		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    もし (rettv_blob_alloc(rettv) == FAIL
	    || blob == NULL
	    || n <= 0)
	戻る;

    slen = blob->bv_ga.ga_len;
    もし (slen <= 0)
	戻る;
    もし (check_repeat_count(slen, n) == FAIL)
	戻る;
    len = slen * (整数)n;

    もし (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	戻る;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    ために (i = 0; i < slen; ++i)
	もし (blob_get(blob, i) != 0)
	    break;

    もし (i == slen)
	// No need to copy since all bytes are already zero
	戻る;

    ために (i = 0; i < (整数)n; ++i)
	blob_set_range(rettv->vval.v_blob,
		(long)i * slen, ((long)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    static void
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    size_t	slen;
    整数		len;
    char_u	*r;
    整数		done;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    slen = STRLEN(p);
    もし (slen == 0 || n <= 0)
	戻る;
    もし (check_repeat_count((varnumber_T)slen, n) == FAIL)
	戻る;
    len = (整数)slen * (整数)n;

    r = alloc((size_t)len + 1);
    もし (r == NULL)
	戻る;

    mch_memmove(r, p, slen);
    done = (整数)slen;
    ながら (done < len)
    {
	整数 copy_len = done;

	もし (copy_len > len - done)
	    copy_len = len - done;
	mch_memmove(r + done, r, (size_t)copy_len);
	done += copy_len;
    }
    r[len] = NUL;

    rettv->vval.v_string = r;
}

/*
 * "repeat()" function
 */
    static void
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    もし (in_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	戻る;

    n = tv_get_number(&argvars[1]);
    もし (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    否则 もし (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    否则 もし (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    否则
	repeat_string(&argvars[0], n, rettv);
}

#define SP_NOMOVE	0x01	    // don't move cursor
#define SP_REPEAT	0x02	    // repeat to find outer pair
#define SP_RETCOUNT	0x04	    // return matchcount
#define SP_SETPCMARK	0x08	    // set previous context mark
#define SP_START	0x10	    // accept match at start position
#define SP_SUBPAT	0x20	    // return nr of matching sub-pattern
#define SP_END		0x40	    // leave cursor at end of match
#define SP_COLUMN	0x80	    // start at cursor column

/*
 * Get flags for a search function.
 * Possibly sets "p_ws".
 * Returns BACKWARD, FORWARD or zero (for an error).
 */
    static 整数
get_search_arg(typval_T *varp, 整数 *flagsp)
{
    整数		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    整数		mask;

    もし (varp->v_type == VAR_UNKNOWN)
	戻る FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    もし (flags == NULL)
	戻る 0;		// type error; errmsg already given
    ながら (*flags != NUL)
    {
	switch (*flags)
	{
	    case 'b': dir = BACKWARD; break;
	    case 'w': p_ws = TRUE; break;
	    case 'W': p_ws = FALSE; break;
	    default:  mask = 0;
		      もし (flagsp != NULL)
			  switch (*flags)
			  {
			      case 'c': mask = SP_START; break;
			      case 'e': mask = SP_END; break;
			      case 'm': mask = SP_RETCOUNT; break;
			      case 'n': mask = SP_NOMOVE; break;
			      case 'p': mask = SP_SUBPAT; break;
			      case 'r': mask = SP_REPEAT; break;
			      case 's': mask = SP_SETPCMARK; break;
			      case 'z': mask = SP_COLUMN; break;
			  }
		      もし (mask == 0)
		      {
			  semsg(_(e_invalid_argument_str), flags);
			  dir = 0;
		      }
		      否则
			  *flagsp |= mask;
	}
	もし (dir == 0)
	    break;
	++flags;
    }
    戻る dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    static 整数
search_cmn(typval_T *argvars, pos_T *match_pos, 整数 *flagsp)
{
    整数		flags;
    char_u	*pat;
    size_t	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    整数		save_p_ws = p_ws;
    整数		dir;
    整数		retval = 0;	// default: FAIL
    long	lnum_stop = 0;
#ifdef FEAT_RELTIME
    long	time_limit = 0;
#endif
    整数		options = SEARCH_KEEP;
    整数		subpatnum;
    searchit_arg_T sia;
    整数		use_skip = FALSE;
    pos_T	firstpos;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_number_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_number_arg(argvars, 3) == FAIL)))))
	goto theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_search_arg(&argvars[1], flagsp);	// may set p_ws
    もし (dir == 0)
	goto theend;
    flags = *flagsp;
    もし (flags & SP_START)
	options |= SEARCH_START;
    もし (flags & SP_END)
	options |= SEARCH_END;
    もし (flags & SP_COLUMN)
	options |= SEARCH_COL;

    // Optional arguments: line number to stop searching, timeout and skip.
    もし (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (long)tv_get_number_chk(&argvars[2], NULL);
	もし (lnum_stop < 0)
	    goto theend;
	もし (argvars[3].v_type != VAR_UNKNOWN)
	{
#ifdef FEAT_RELTIME
	    time_limit = (long)tv_get_number_chk(&argvars[3], NULL);
	    もし (time_limit < 0)
		goto theend;
#endif
	    use_skip = eval_expr_valid_arg(&argvars[4]);
	}
    }

    /*
     * This function does not accept SP_REPEAT and SP_RETCOUNT flags.
     * Check to make sure only those flags are set.
     * Also, Only the SP_NOMOVE or the SP_SETPCMARK flag can be set. Both
     * flags cannot be set. Check for that condition also.
     */
    もし (((flags & (SP_REPEAT | SP_RETCOUNT)) != 0)
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	goto theend;
    }

    pos = save_cursor = curwin->w_cursor;
    CLEAR_FIELD(firstpos);
    CLEAR_FIELD(sia);
    sia.sa_stop_lnum = (linenr_T)lnum_stop;
#ifdef FEAT_RELTIME
    sia.sa_tm = time_limit;
#endif

    patlen = STRLEN(pat);

    // Repeat until {skip} returns FALSE.
    ために (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, NULL, dir, pat, patlen, 1L,
						     options, RE_SEARCH, &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	もし (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	もし (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    break;
	もし (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    整数	    do_skip;
	    整数	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_expr_to_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    もし (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		break;
	    }
	    もし (!do_skip)
		break;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~SEARCH_START;
    }

    もし (subpatnum != FAIL)
    {
	もし (flags & SP_SUBPAT)
	    retval = subpatnum;
	否则
	    retval = pos.lnum;
	もし (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	もし (match_pos != NULL)
	{
	    // Store the match cursor position
	    match_pos->lnum = pos.lnum;
	    match_pos->col = pos.col + 1;
	}
	// "/$" will put the cursor after the end of the line, may need to
	// correct that here
	check_cursor();
    }

    // If 'n' flag is used: restore cursor position.
    もし (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    否则
	curwin->w_set_curswant = true;
theend:
    p_ws = save_p_ws;

    戻る retval;
}

#ifdef FEAT_RUBY
/*
 * "rubyeval()" function
 */
    static void
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    もし (check_restricted() || check_secure())
	戻る;

    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#endif

/*
 * "screenattr()" function
 */
    static void
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    整数		row;
    整数		col;
    整数		c;

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	戻る;

    row = (整数)tv_get_number_chk(&argvars[0], NULL) - 1;
    col = (整数)tv_get_number_chk(&argvars[1], NULL) - 1;
    もし (row < 0 || row >= screen_Rows
	    || col < 0 || col >= screen_Columns)
	c = -1;
    否则
	c = ScreenAttrs[LineOffset[row] + col];
    rettv->vval.v_number = c;
}

/*
 * "screenchar()" function
 */
    static void
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    整数		row;
    整数		col;
    整数		c;

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	戻る;

    row = (整数)tv_get_number_chk(&argvars[0], NULL) - 1;
    col = (整数)tv_get_number_chk(&argvars[1], NULL) - 1;
    もし (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	c = -1;
    否则
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(row, col, buf, NULL);
	c = (*mb_ptr2char)(buf);
    }
    rettv->vval.v_number = c;
}

/*
 * "screenchars()" function
 */
    static void
f_screenchars(typval_T *argvars, typval_T *rettv)
{
    整数		row;
    整数		col;
    整数		c;
    整数		i;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	戻る;

    row = (整数)tv_get_number_chk(&argvars[0], NULL) - 1;
    col = (整数)tv_get_number_chk(&argvars[1], NULL) - 1;
    もし (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	戻る;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(row, col, buf, NULL);
    整数 pcc[MAX_MCO];
    もし (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    否则
	c = (*mb_ptr2char)(buf);
    list_append_number(rettv->vval.v_list, (varnumber_T)c);

    もし (enc_utf8)
	ために (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_number(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    static void
f_screencol(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_number = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    static void
f_screenrow(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_number = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    static void
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    整数		row;
    整数		col;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = NULL;
    rettv->v_type = VAR_STRING;

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	戻る;

    row = (整数)tv_get_number_chk(&argvars[0], NULL) - 1;
    col = (整数)tv_get_number_chk(&argvars[1], NULL) - 1;
    もし (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	戻る;

    screen_getbytes(row, col, buf, NULL);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    static void
f_search(typval_T *argvars, typval_T *rettv)
{
    整数		flags = 0;

    rettv->vval.v_number = search_cmn(argvars, NULL, &flags);
}

/*
 * "searchdecl()" function
 */
    static void
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    整数		locally = TRUE;
    整数		thisblock = FALSE;
    整数		error = FALSE;
    char_u	*name;

    rettv->vval.v_number = 1;	// default: FAIL

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	戻る;

    name = tv_get_string_chk(&argvars[0]);
    もし (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(整数)tv_get_bool_chk(&argvars[1], &error);
	もし (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (整数)tv_get_bool_chk(&argvars[2], &error);
    }
    もし (!error && name != NULL)
	rettv->vval.v_number = find_decl(name, (整数)STRLEN(name),
				     locally, thisblock, SEARCH_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    static 整数
searchpair_cmn(typval_T *argvars, pos_T *match_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    整数		save_p_ws = p_ws;
    整数		dir;
    整数		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    整数		retval = 0;		// default: FAIL
    long	lnum_stop = 0;
    long	time_limit = 0;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_number_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_number_arg(argvars, 6) == FAIL)))))
	goto theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    もし (spat == NULL || mpat == NULL || epat == NULL)
	goto theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_search_arg(&argvars[3], &flags); // may set p_ws
    もし (dir == 0)
	goto theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    もし ((flags & (SP_END | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[3]));
	goto theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    もし (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    もし (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = NULL;
    否则
    {
	// Type is checked later.
	skip = &argvars[4];

	もし (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (long)tv_get_number_chk(&argvars[5], NULL);
	    もし (lnum_stop < 0)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[5]));
		goto theend;
	    }
#ifdef FEAT_RELTIME
	    もし (argvars[6].v_type != VAR_UNKNOWN)
	    {
		time_limit = (long)tv_get_number_chk(&argvars[6], NULL);
		もし (time_limit < 0)
		{
		    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[6]));
		    goto theend;
		}
	    }
#endif
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    match_pos, lnum_stop, time_limit);

theend:
    p_ws = save_p_ws;

    戻る retval;
}

/*
 * "searchpair()" function
 */
    static void
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = searchpair_cmn(argvars, NULL);
}

/*
 * "searchpairpos()" function
 */
    static void
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    整数		lnum = 0;
    整数		col = 0;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    もし (searchpair_cmn(argvars, &match_pos) > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_number(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_number(rettv->vval.v_list, (varnumber_T)col);
}

/*
 * Search for a start/middle/end thing.
 * Used by searchpair(), see its documentation for the details.
 * Returns 0 or -1 for no match,
 */
    long
do_searchpair(
    char_u	*spat,	    // start pattern
    char_u	*mpat,	    // middle pattern
    char_u	*epat,	    // end pattern
    整数		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    整数		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*match_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    long	time_limit UNUSED) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = NULL, *pat3 = NULL;
    size_t	patlen;
    size_t	spatlen;
    size_t	epatlen;
    size_t	pat2size;
    size_t	pat2len;
    size_t	pat3size;
    size_t	pat3len;
    long	retval = 0;
    pos_T	pos;
    pos_T	firstpos;
    pos_T	foundpos;
    pos_T	save_cursor;
    pos_T	save_pos;
    整数		n;
    整数		r;
    整数		nest = 1;
    整数		use_skip = FALSE;
    整数		err;
    整数		options = SEARCH_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    もし (pat2 == NULL)
	goto theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    もし (pat3 == NULL)
	goto theend;
    pat2len = vim_snprintf((char *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    もし (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    否则
	pat3len = vim_snprintf((char *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    もし (flags & SP_START)
	options |= SEARCH_START;

    もし (skip != NULL)
	use_skip = eval_expr_valid_arg(skip);

#ifdef FEAT_RELTIME
    もし (time_limit > 0)
	init_regexp_timeout(time_limit);
#endif
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    ために (;;)
    {
	searchit_arg_T sia;

	CLEAR_FIELD(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, NULL, dir, pat, patlen, 1L,
						     options, RE_SEARCH, &sia);
	もし (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    break;

	もし (firstpos.lnum == 0)
	    firstpos = pos;
	もし (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    もし (dir == BACKWARD)
		decl(&pos);
	    否则
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~SEARCH_START;

	// If the skip pattern matches, ignore this match.
	もし (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_expr_to_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    もし (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		break;
	    }
	    もし (r)
		continue;
	}

	もし ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	否则
	{
	    // Found end when searching forward or start when searching
	    // backward: end of (nested) pair; or found middle in outer pair.
	    もし (--nest == 1)
		pat = pat3;	// outer level, search for middle
	}

	もし (nest == 0)
	{
	    // Found the match: return matchcount or line number.
	    もし (flags & SP_RETCOUNT)
		++retval;
	    否则
		retval = pos.lnum;
	    もし (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    もし (!(flags & SP_REPEAT))
		break;
	    nest = 1;	    // search for next unmatched
	}
    }

    もし (match_pos != NULL)
    {
	// Store the match cursor position
	match_pos->lnum = curwin->w_cursor.lnum;
	match_pos->col = curwin->w_cursor.col + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    もし ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#ifdef FEAT_RELTIME
    もし (time_limit > 0)
	disable_regexp_timeout();
#endif
    vim_free(pat2);
    vim_free(pat3);
    もし (p_cpo == empty_option)
	p_cpo = save_cpo;
    否则
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	もし (*p_cpo == NUL)
	    set_option_value_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    戻る retval;
}

/*
 * "searchpos()" function
 */
    static void
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    整数		lnum = 0;
    整数		col = 0;
    整数		n;
    整数		flags = 0;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    n = search_cmn(argvars, &match_pos, &flags);
    もし (n > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_number(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_number(rettv->vval.v_list, (varnumber_T)col);
    もし (flags & SP_SUBPAT)
	list_append_number(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    static void
set_position(typval_T *argvars, typval_T *rettv, 整数 charpos)
{
    pos_T	pos;
    整数		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_number = -1;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	戻る;

    name = tv_get_string_chk(argvars);
    もし (name == NULL)
	戻る;

    もし (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	戻る;

    もし (pos.col != MAXCOL && --pos.col < 0)
	pos.col = 0;
    もし ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	もし (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_set_curswant = false;
	}
	check_cursor();
	rettv->vval.v_number = 0;
    }
    否则 もし (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	もし (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_number = 0;
    }
    否则
	emsg(_(e_invalid_argument));
}
/*
 * "setcharpos()" function
 */
    static void
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, TRUE);
}

    static void
f_setcharsearch(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    もし (check_for_dict_arg(argvars, 0) == FAIL)
	戻る;

    もし ((d = argvars[0].vval.v_dict) == NULL)
	戻る;

    csearch = dict_get_string(d, "char", FALSE);
    もし (csearch != NULL)
    {
	もし (enc_utf8)
	{
	    整数 pcc[MAX_MCO];
	    整数 c = utfc_ptr2char(csearch, pcc);

	    set_last_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	否则
	    set_last_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_find(d, (char_u *)"forward", -1);
    もし (di != NULL)
	set_csearch_direction((整数)tv_get_number(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_find(d, (char_u *)"until", -1);
    もし (di != NULL)
	set_csearch_until(!!tv_get_number(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    static void
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    static void
f_setenv(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    もし (check_restricted() || check_secure())
	戻る;

    name = tv_get_string_buf(&argvars[0], namebuf);
    もし (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_number == VVAL_NULL)
	vim_unsetenv_ext(name);
    否则
	vim_setenv_ext(name, tv_get_string_buf(&argvars[1], valbuf));
}

/*
 * "setfperm({fname}, {mode})" function
 */
    static void
f_setfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	modebuf[NUMBUFLEN];
    char_u	*mode_str;
    整数		i;
    整数		mask;
    整数		mode = 0;

    rettv->vval.v_number = 0;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	戻る;

    fname = tv_get_string_chk(&argvars[0]);
    もし (fname == NULL)
	戻る;
    mode_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    もし (mode_str == NULL)
	戻る;
    もし (STRLEN(mode_str) != 9)
    {
	semsg(_(e_invalid_argument_str), mode_str);
	戻る;
    }

    mask = 1;
    ために (i = 8; i >= 0; --i)
    {
	もし (mode_str[i] != '-')
	    mode |= mask;
	mask = mask << 1;
    }
    rettv->vval.v_number = mch_setperm(fname, mode) == OK;
}

/*
 * "setpos()" function
 */
    static void
f_setpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    整数
get_yank_type(char_u **pp, char_u *yank_type, long *block_len)
{
    char_u *stropt = *pp;
    switch (*stropt)
    {
	case 'v': case 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    break;
	case 'V': case 'l':	// line-wise selection
	    *yank_type = MLINE;
	    break;
	case 'b': case Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    もし (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*block_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    break;
	default:
	    戻る FAIL;
    }
    *pp = stropt;
    戻る OK;
}

/*
 * "setreg()" function
 */
    static void
f_setreg(typval_T *argvars, typval_T *rettv)
{
    整数		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    整数		append;
    char_u	yank_type;
    long	block_len;
    typval_T	*regcontents;
    整数		pointreg;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	戻る;

    pointreg = 0;
    regcontents = NULL;
    block_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_number = 1;		// FAIL is default

    もし (strregname == NULL)
	戻る;		// type error; errmsg already given
    もし (in_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_register_name_must_be_one_char_str), strregname);
	戻る;
    }
    regname = *strregname;
    もし (regname == 0 || regname == '@')
	regname = '"';

    もし (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	もし (d == NULL || d->dv_hashtab.ht_used == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {NULL, NULL};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    戻る;
	}

	di = dict_find(d, (char_u *)"regcontents", -1);
	もし (di != NULL)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	もし (stropt != NULL)
	{
	    整数 ret = get_yank_type(&stropt, &yank_type, &block_len);

	    もし (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_value_for_argument_str), "value");
		戻る;
	    }
	}

	もし (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    もし (stropt != NULL)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	否则 もし (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    否则
	regcontents = &argvars[1];

    もし (argvars[2].v_type != VAR_UNKNOWN)
    {
	もし (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    戻る;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	もし (stropt == NULL)
	    戻る;		// type error
	ために (; *stropt != NUL; ++stropt)
	    switch (*stropt)
	    {
		case 'a': case 'A':	// append
		    append = TRUE;
		    break;
		default:
		    get_yank_type(&stropt, &yank_type, &block_len);
	    }
    }

    もし (regcontents && regcontents->v_type == VAR_LIST)
    {
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	list_T		*ll = regcontents->vval.v_list;
	listitem_T	*li;
	整数		len;

	// If the list is NULL handle like an empty list.
	len = ll == NULL ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	もし (lstval == NULL)
	    戻る;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	もし (ll != NULL)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_ITEMS(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		もし (strval == NULL)
		    goto free_lstval;
		もし (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    もし (strval == NULL)
			goto free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = NULL;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, block_len);
free_lstval:
	ながら (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    否则 もし (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	もし (strval == NULL)
	    戻る;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, block_len);
    }
    もし (pointreg != 0)
	get_yank_register(pointreg, TRUE);

    rettv->vval.v_number = 0;
}

/*
 * "settagstack()" function
 */
    static void
f_settagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    dict_T	*d;
    整数		action = 'r';

    rettv->vval.v_number = -1;

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	戻る;

    // first argument: window number or id
    wp = find_win_by_nr_or_id(&argvars[0]);
    もし (wp == NULL)
	戻る;

    // second argument: dict with items to set in the tag stack
    もし (check_for_dict_arg(argvars, 1) == FAIL)
	戻る;
    d = argvars[1].vval.v_dict;
    もし (d == NULL)
	戻る;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    もし (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    否则 もし (check_for_string_arg(argvars, 2) == FAIL)
	戻る;
    否则
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	もし (actstr == NULL)
	    戻る;
	もし ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	否则
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    戻る;
	}
    }

    もし (set_tagstack(wp, d, action) == OK)
	rettv->vval.v_number = 0;
}

#ifdef FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    static void
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    整数		len;

    もし (in_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	戻る;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    もし (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != NULL ? (char_u *)blob->bv_ga.ga_data : (char_u *)"";
	len = blob != NULL ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_bytes(p, len, NULL, 0));
    }
    否则
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_bytes(p, (整数)STRLEN(p), NULL, 0));
    }
}
#endif // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    static void
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    整数 do_special;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	戻る;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    static void
f_shiftwidth(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_number = 0;

    もし (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type != VAR_UNKNOWN)
    {
	long	col;

	col = (long)tv_get_number_chk(argvars, NULL);
	もし (col < 0)
	    戻る;	// type error; errmsg already given
#ifdef FEAT_VARTABS
	rettv->vval.v_number = get_sw_value_col(curbuf, col, FALSE);
	戻る;
#endif
    }

    rettv->vval.v_number = get_sw_value(curbuf);
}

/*
 * "soundfold({word})" function
 */
    static void
f_soundfold(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#ifdef FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#否则
    rettv->vval.v_string = vim_strsave(s);
#endif
}

/*
 * "spellbadword()" function
 */
    static void
f_spellbadword(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*word = (char_u *)"";
    hlf_T	attr = HLF_COUNT;
    整数		len = 0;
#ifdef FEAT_SPELL
    整数		wo_spell_save = curwin->w_p_spell;

    もし (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	戻る;

    もし (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    もし (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	戻る;
    }
#endif

    もし (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	戻る;
    }

#ifdef FEAT_SPELL
    もし (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_to(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	もし (len != 0)
	{
	    word = ml_get_cursor();
	    curwin->w_set_curswant = true;
	}
    }
    否则 もし (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	整数	capcol = -1;

	もし (str != NULL)
	{
	    // Check the argument for spelling.
	    ながら (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		もし (attr != HLF_COUNT)
		{
		    word = str;
		    break;
		}
		str += len;
		capcol -= len;
		len = 0;
	    }
	}
    }
    curwin->w_p_spell = wo_spell_save;
#endif

    list_append_string(rettv->vval.v_list, word, len);
    switch (attr)
    {
    case HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	break;

    case HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	break;

    case HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	break;

    case HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	break;

    default:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	break;
    }
}

/*
 * "spellsuggest()" function
 */
    static void
f_spellsuggest(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SPELL
    char_u	*str;
    整数		typeerr = FALSE;
    整数		maxcount;
    garray_T	ga;
    整数		i;
    listitem_T	*li;
    整数		need_capital = FALSE;
    整数		wo_spell_save = curwin->w_p_spell;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	戻る;

    もし (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    もし (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	戻る;
    }
#endif

    もし (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	戻る;
    }

#ifdef FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    もし (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (整数)tv_get_number_chk(&argvars[1], &typeerr);
	もし (maxcount <= 0)
	    戻る;
	もし (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (整数)tv_get_bool_chk(&argvars[2], &typeerr);
	    もし (typeerr)
		戻る;
	}
    }
    否则
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    ために (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_data)[i];

	li = listitem_alloc();
	もし (li == NULL)
	    vim_free(str);
	否则
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_lock = 0;
	    li->li_tv.vval.v_string = str;
	    list_append(rettv->vval.v_list, li);
	}
    }
    ga_clear(&ga);
    curwin->w_p_spell = wo_spell_save;
#endif
}

    static void
f_split(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*end;
    char_u	*pat = NULL;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    整数		match;
    colnr_T	col = 0;
    整数		keepempty = FALSE;
    整数		typeerr = FALSE;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	戻る;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    str = tv_get_string(&argvars[0]);
    もし (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	もし (pat == NULL)
	    typeerr = TRUE;
	もし (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (整数)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    もし (pat == NULL || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    もし (rettv_list_alloc(rettv) == FAIL)
	goto theend;
    もし (typeerr)
	goto theend;

    regmatch.regprog = eval_regcomp(pat);
    もし (regmatch.regprog != NULL)
    {
	regmatch.rm_ic = FALSE;
	ながら (*str != NUL || keepempty)
	{
	    もし (*str == NUL)
		match = FALSE;	// empty item at the end
	    否则
		match = vim_regexec_nl(&regmatch, str, col);
	    もし (match)
		end = regmatch.startp[0];
	    否则
		end = str + STRLEN(str);
	    もし (keepempty || end > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && match && end < regmatch.endp[0]))
	    {
		もし (list_append_string(rettv->vval.v_list, str,
						    (整数)(end - str)) == FAIL)
		    break;
	    }
	    もし (!match)
		break;
	    // Advance to just after the match.
	    もし (regmatch.endp[0] > str)
		col = 0;
	    否则
		// Don't get stuck at the same match.
		col = (*mb_ptr2len)(regmatch.endp[0]);
	    str = regmatch.endp[0];
	}

	eval_regfree(pat, regmatch.regprog);
    }

theend:
    p_cpo = save_cpo;
}

/*
 * "submatch()" function
 */
    static void
f_submatch(typval_T *argvars, typval_T *rettv)
{
    整数		error = FALSE;
    整数		no;
    整数		retList = 0;

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	戻る;

    no = (整数)tv_get_number_chk(&argvars[0], &error);
    もし (error)
	戻る;
    もし (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_number_nr), no);
	戻る;
    }
    もし (argvars[1].v_type != VAR_UNKNOWN)
	retList = (整数)tv_get_bool_chk(&argvars[1], &error);
    もし (error)
	戻る;

    もし (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    否则
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = reg_submatch_list(no);
    }
}

/*
 * "substitute()" function
 */
    static void
f_substitute(typval_T *argvars, typval_T *rettv)
{
    char_u	patbuf[NUMBUFLEN];
    char_u	subbuf[NUMBUFLEN];
    char_u	flagsbuf[NUMBUFLEN];
    char_u	*str;
    char_u	*pat;
    char_u	*sub = NULL;
    typval_T	*expr = NULL;
    char_u	*flg;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	戻る;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    もし (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	expr = &argvars[2];
    否则
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    もし (str == NULL || pat == NULL || (sub == NULL && expr == NULL)
								|| flg == NULL)
	rettv->vval.v_string = NULL;
    否则
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, expr, flg, NULL);
}

/*
 * "swapfilelist()" function
 */
    static void
f_swapfilelist(typval_T *argvars UNUSED, typval_T *rettv)
{
    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;
    recover_names(NULL, FALSE, rettv->vval.v_list, 0, NULL);
}

/*
 * "swapinfo(swap_filename)" function
 */
    static void
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    もし (rettv_dict_alloc(rettv) == OK)
	get_b0_dict(tv_get_string(argvars), rettv->vval.v_dict);
}

/*
 * "swapname(expr)" function
 */
    static void
f_swapname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    もし (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	戻る;

    buf = tv_get_buf(&argvars[0], FALSE);
    もし (buf == NULL || buf->b_ml.ml_mfp == NULL
					|| buf->b_ml.ml_mfp->mf_fname == NULL)
	rettv->vval.v_string = NULL;
    否则
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    static void
f_synID(typval_T *argvars UNUSED, typval_T *rettv)
{
    整数		id = 0;
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    整数		trans;
    整数		transerr = FALSE;

    もし (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	戻る;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (linenr_T)tv_get_number(&argvars[1]) - 1;	// -1 on type error
    trans = (整数)tv_get_bool_chk(&argvars[2], &transerr);

    もし (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count
	    && col >= 0 && col < (long)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, col, trans, NULL, FALSE);
#endif

    rettv->vval.v_number = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    static void
f_synIDattr(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*p = NULL;
#ifdef FEAT_SYN_HL
    整数		id;
    char_u	*what;
    char_u	*mode;
    char_u	modebuf[NUMBUFLEN];
    整数		modec;

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	戻る;

    id = (整数)tv_get_number(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    もし (argvars[2].v_type != VAR_UNKNOWN)
    {
	mode = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(mode[0]);
	もし (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    否则
    {
# もし defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	もし (USE_24BIT)
	    modec = 'g';
	否则
# endif
	    もし (t_colors > 1)
		modec = 'c';
	    否则
		modec = 't';
    }

    switch (TOLOWER_ASC(what[0]))
    {
	case 'b':
		もし (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		否则					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		break;

	case 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		break;

	case 'i':
		もし (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		否则					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		break;

	case 'n':
		もし (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		否则					// name
		    p = get_highlight_name_ext(NULL, id - 1, FALSE);
		break;

	case 'r':					// reverse
		p = highlight_has_attr(id, HL_INVERSE, modec);
		break;

	case 's':
		もし (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		否则 もし (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		否则					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		break;

	case 'u':
		もし (STRLEN(what) >= 9)
		{
		    もし (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    否则 もし (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    否则 もし (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    否则 もし (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    否则
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		否则
							// ul
		    p = highlight_color(id, what, modec);
		break;
    }

    もし (p != NULL)
	p = vim_strsave(p);
#endif
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    static void
f_synIDtrans(typval_T *argvars UNUSED, typval_T *rettv)
{
    整数		id;

#ifdef FEAT_SYN_HL
    もし (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	戻る;

    id = (整数)tv_get_number(&argvars[0]);

    もし (id > 0)
	id = syn_get_final_id(id);
    否则
#endif
	id = 0;

    rettv->vval.v_number = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    static void
f_synconcealed(typval_T *argvars UNUSED, typval_T *rettv)
{
#もし defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	col;
    整数		syntax_flags = 0;
    整数		cchar;
    整数		matchid = 0;
    char_u	str[NUMBUFLEN];
#endif

    rettv_list_set(rettv, NULL);

    もし (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	戻る;

#もし defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_number(&argvars[1]) - 1;	// -1 on type error

    CLEAR_FIELD(str);

    もし (rettv_list_alloc(rettv) == OK)
    {
	もし (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (void)syn_get_id(curwin, lnum, col, FALSE, NULL, FALSE);
	    syntax_flags = get_syntax_info(&matchid);

	    // get the conceal character
	    もし ((syntax_flags & HL_CONCEAL) && curwin->w_p_cole < 3)
	    {
		cchar = syn_get_sub_char();
		もし (cchar == NUL && curwin->w_p_cole == 1)
		    cchar = (curwin->w_lcs_chars.conceal == NUL) ? ' '
					: curwin->w_lcs_chars.conceal;
		もし (cchar != NUL)
		{
		    もし (has_mbyte)
			(*mb_char2bytes)(cchar, str);
		    否则
			str[0] = cchar;
		}
	    }
	}

	list_append_number(rettv->vval.v_list,
					    (syntax_flags & HL_CONCEAL) != 0);
	// -1 to auto-determine strlen
	list_append_string(rettv->vval.v_list, str, -1);
	list_append_number(rettv->vval.v_list, matchid);
    }
#endif
}

/*
 * "synstack(lnum, col)" function
 */
    static void
f_synstack(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    整数		i;
    整数		id;
#endif

    rettv_list_set(rettv, NULL);

    もし (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	戻る;

#ifdef FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_number(&argvars[1]) - 1;	// -1 on type error

    もし (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(void)syn_get_id(curwin, lnum, col, FALSE, NULL, TRUE);
	ために (i = 0; ; ++i)
	{
	    id = syn_get_stack_item(i);
	    もし (id < 0)
		break;
	    もし (list_append_number(rettv->vval.v_list, id) == FAIL)
		break;
	}
    }
#endif
}

/*
 * "tabpagebuflist()" function
 */
    static void
f_tabpagebuflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp = NULL;

    もし (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    否则
    {
	tp = find_tabpage((整数)tv_get_number(&argvars[0]));
	もし (tp != NULL)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    もし (wp != NULL && rettv_list_alloc(rettv) == OK)
    {
	ために (; wp != NULL; wp = wp->w_next)
	    もし (list_append_number(rettv->vval.v_list,
						wp->w_buffer->b_fnum) == FAIL)
		break;
    }
}

/*
 * "tagfiles()" function
 */
    static void
f_tagfiles(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    整数		first;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;
    fname = alloc(MAXPATHL);
    もし (fname == NULL)
	戻る;

    ために (first = TRUE; ; first = FALSE)
	もし (get_tagfname(&tn, first, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    break;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    static void
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = NULL;
    char_u  *tag_pattern;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	戻る;

    tag_pattern = tv_get_string(&argvars[0]);

    rettv->vval.v_number = FALSE;
    もし (*tag_pattern == NUL)
	戻る;

    もし (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    もし (rettv_list_alloc(rettv) == OK)
	(void)get_tags(rettv->vval.v_list, tag_pattern, fname);
}

/*
 * "type(expr)" function
 */
    static void
f_type(typval_T *argvars, typval_T *rettv)
{
    整数 n = -1;

    switch (argvars[0].v_type)
    {
	case VAR_NUMBER:  n = VAR_TYPE_NUMBER; break;
	case VAR_STRING:  n = VAR_TYPE_STRING; break;
	case VAR_PARTIAL:
	case VAR_FUNC:    n = VAR_TYPE_FUNC; break;
	case VAR_LIST:    n = VAR_TYPE_LIST; break;
	case VAR_TUPLE:   n = VAR_TYPE_TUPLE; break;
	case VAR_DICT:    n = VAR_TYPE_DICT; break;
	case VAR_FLOAT:   n = VAR_TYPE_FLOAT; break;
	case VAR_BOOL:	  n = VAR_TYPE_BOOL; break;
	case VAR_SPECIAL: n = VAR_TYPE_NONE; break;
	case VAR_JOB:     n = VAR_TYPE_JOB; break;
	case VAR_CHANNEL: n = VAR_TYPE_CHANNEL; break;
	case VAR_BLOB:    n = VAR_TYPE_BLOB; break;
	case VAR_INSTR:   n = VAR_TYPE_INSTR; break;
	case VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; break;
	case VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		もし (cl != NULL && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		否则
		    n = VAR_TYPE_CLASS;
		break;
	    }
	case VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		もし (obj == NULL)
		    n = VAR_TYPE_OBJECT;
		否则
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    もし (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    否则
			n = VAR_TYPE_OBJECT;
		}
		break;
	    }
	case VAR_VOID:
	    emsg(_(e_cannot_use_void_value));
	    break;
	case VAR_UNKNOWN:
	case VAR_ANY:
	    internal_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    break;
    }
    rettv->vval.v_number = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    static void
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_start = 0;
    colnr_T	vcol_end = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    整数		winchanged = FALSE;
    整数		len;

    もし (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_number_arg(argvars, 2) == FAIL))))
	戻る;

    もし (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((整数)tv_get_number(&argvars[2]), &tp);
	もし (wp == NULL || tp == NULL)
	    goto theend;

	もし (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    goto theend;

	check_cursor();
	winchanged = TRUE;
    }

    整数 fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    もし (fp != NULL && fp->lnum <= curbuf->b_ml.ml_line_count
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	もし (fp->col < 0)
	    fp->col = 0;
	否则
	{
	    len = (整数)ml_get_len(fp->lnum);
	    もし (fp->col > len)
		fp->col = len;
	}
	getvvcol(curwin, fp, &vcol_start, NULL, &vcol_end, 0);
	++vcol_start;
	++vcol_end;
    }

theend:
    もし (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	もし (rettv_list_alloc(rettv) == OK)
	{
	    list_append_number(rettv->vval.v_list, vcol_start);
	    list_append_number(rettv->vval.v_list, vcol_end);
	}
	否则
	    rettv->vval.v_number = 0;
    }
    否则
	rettv->vval.v_number = vcol_end;

    もし (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    static void
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    もし (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	戻る;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_mode_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    もし (non_zero_arg(&argvars[0]))
	curbuf->b_visual_mode_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    static void
f_wildmenumode(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    もし (wild_menu_showing || ((State & MODE_CMDLINE) && cmdline_pum_active()))
	rettv->vval.v_number = 1;
}

/*
 * "windowsversion()" function
 */
    static void
f_windowsversion(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windowsVersion);
}

/*
 * "wordcount()" function
 */
    static void
f_wordcount(typval_T *argvars UNUSED, typval_T *rettv)
{
    もし (rettv_dict_alloc(rettv) == FAIL)
	戻る;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    static void
f_xor(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	戻る;

    rettv->vval.v_number = tv_get_number_chk(&argvars[0], NULL)
					^ tv_get_number_chk(&argvars[1], NULL);
}

#endif // FEAT_EVAL
