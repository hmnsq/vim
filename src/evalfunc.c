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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

ایستا تهی f_and(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_BEVAL
ایستا تهی f_balloon_gettext(typval_T *argvars, typval_T *rettv);
ایستا تهی f_balloon_show(typval_T *argvars, typval_T *rettv);
# اگر defined(FEAT_BEVAL_TERM)
ایستا تهی f_balloon_split(typval_T *argvars, typval_T *rettv);
# endif
#endif
ایستا تهی f_base64_encode(typval_T *argvars, typval_T *rettv);
ایستا تهی f_base64_decode(typval_T *argvars, typval_T *rettv);
ایستا تهی f_bindtextdomain(typval_T *argvars, typval_T *rettv);
ایستا تهی f_byte2line(typval_T *argvars, typval_T *rettv);
ایستا تهی f_call(typval_T *argvars, typval_T *rettv);
ایستا تهی f_changenr(typval_T *argvars, typval_T *rettv);
ایستا تهی f_char2nr(typval_T *argvars, typval_T *rettv);
ایستا تهی f_charcol(typval_T *argvars, typval_T *rettv);
ایستا تهی f_col(typval_T *argvars, typval_T *rettv);
ایستا تهی f_confirm(typval_T *argvars, typval_T *rettv);
ایستا تهی f_copy(typval_T *argvars, typval_T *rettv);
ایستا تهی f_cursor(typval_T *argsvars, typval_T *rettv);
#ifdef MSWIN
ایستا تهی f_debugbreak(typval_T *argvars, typval_T *rettv);
#endif
ایستا تهی f_deepcopy(typval_T *argvars, typval_T *rettv);
ایستا تهی f_did_filetype(typval_T *argvars, typval_T *rettv);
ایستا تهی f_echoraw(typval_T *argvars, typval_T *rettv);
ایستا تهی f_empty(typval_T *argvars, typval_T *rettv);
ایستا تهی f_environ(typval_T *argvars, typval_T *rettv);
ایستا تهی f_err_teapot(typval_T *argvars, typval_T *rettv);
ایستا تهی f_escape(typval_T *argvars, typval_T *rettv);
ایستا تهی f_eval(typval_T *argvars, typval_T *rettv);
ایستا تهی f_eventhandler(typval_T *argvars, typval_T *rettv);
ایستا تهی f_execute(typval_T *argvars, typval_T *rettv);
ایستا تهی f_exists_compiled(typval_T *argvars, typval_T *rettv);
ایستا تهی f_expand(typval_T *argvars, typval_T *rettv);
ایستا تهی f_expandcmd(typval_T *argvars, typval_T *rettv);
ایستا تهی f_feedkeys(typval_T *argvars, typval_T *rettv);
ایستا تهی f_fnameescape(typval_T *argvars, typval_T *rettv);
ایستا تهی f_foreground(typval_T *argvars, typval_T *rettv);
ایستا تهی f_funcref(typval_T *argvars, typval_T *rettv);
ایستا تهی f_function(typval_T *argvars, typval_T *rettv);
ایستا تهی f_garbagecollect(typval_T *argvars, typval_T *rettv);
ایستا تهی f_get(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getbgcolor(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getcellpixels(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getchangelist(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getcharpos(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getcharsearch(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getcurpos(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getcursorcharpos(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getenv(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getfontname(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getinfo(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getjumplist(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getpid(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getpos(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getreg(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getreginfo(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getregion(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getregionpos(typval_T *argvars, typval_T *rettv);
ایستا تهی f_getregtype(typval_T *argvars, typval_T *rettv);
ایستا تهی f_gettagstack(typval_T *argvars, typval_T *rettv);
ایستا تهی f_gettext(typval_T *argvars, typval_T *rettv);
ایستا تهی f_haslocaldir(typval_T *argvars, typval_T *rettv);
ایستا تهی f_hlID(typval_T *argvars, typval_T *rettv);
ایستا تهی f_hlexists(typval_T *argvars, typval_T *rettv);
ایستا تهی f_hostname(typval_T *argvars, typval_T *rettv);
ایستا تهی f_id(typval_T *argvars, typval_T *rettv);
ایستا تهی f_index(typval_T *argvars, typval_T *rettv);
ایستا تهی f_indexof(typval_T *argvars, typval_T *rettv);
ایستا تهی f_درون_داد(typval_T *argvars, typval_T *rettv);
ایستا تهی f_inputdialog(typval_T *argvars, typval_T *rettv);
ایستا تهی f_inputlist(typval_T *argvars, typval_T *rettv);
ایستا تهی f_inputrestore(typval_T *argvars, typval_T *rettv);
ایستا تهی f_inputsave(typval_T *argvars, typval_T *rettv);
ایستا تهی f_inputsecret(typval_T *argvars, typval_T *rettv);
ایستا تهی f_interrupt(typval_T *argvars, typval_T *rettv);
ایستا تهی f_invert(typval_T *argvars, typval_T *rettv);
ایستا تهی f_islocked(typval_T *argvars, typval_T *rettv);
ایستا تهی f_اقلام(typval_T *argvars, typval_T *rettv);
ایستا تهی f_keytrans(typval_T *argvars, typval_T *rettv);
ایستا تهی f_last_buffer_nr(typval_T *argvars, typval_T *rettv);
ایستا تهی f_libcall(typval_T *argvars, typval_T *rettv);
ایستا تهی f_libcallnr(typval_T *argvars, typval_T *rettv);
ایستا تهی f_سطر(typval_T *argvars, typval_T *rettv);
ایستا تهی f_line2byte(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_LUA
ایستا تهی f_luaeval(typval_T *argvars, typval_T *rettv);
#endif
ایستا تهی f_match(typval_T *argvars, typval_T *rettv);
ایستا تهی f_matchbufline(typval_T *argvars, typval_T *rettv);
ایستا تهی f_matchend(typval_T *argvars, typval_T *rettv);
ایستا تهی f_matchlist(typval_T *argvars, typval_T *rettv);
ایستا تهی f_matchstr(typval_T *argvars, typval_T *rettv);
ایستا تهی f_matchstrlist(typval_T *argvars, typval_T *rettv);
ایستا تهی f_matchstrpos(typval_T *argvars, typval_T *rettv);
ایستا تهی f_max(typval_T *argvars, typval_T *rettv);
ایستا تهی f_min(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_MZSCHEME
ایستا تهی f_mzeval(typval_T *argvars, typval_T *rettv);
#endif
ایستا تهی f_nextnonblank(typval_T *argvars, typval_T *rettv);
ایستا تهی f_ngettext(typval_T *argvars, typval_T *rettv);
ایستا تهی f_nr2char(typval_T *argvars, typval_T *rettv);
ایستا تهی f_or(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PERL
ایستا تهی f_perleval(typval_T *argvars, typval_T *rettv);
#endif
ایستا تهی f_prevnonblank(typval_T *argvars, typval_T *rettv);
ایستا تهی f_printf(typval_T *argvars, typval_T *rettv);
ایستا تهی f_pum_getpos(typval_T *argvars, typval_T *rettv);
ایستا تهی f_pumvisible(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PYTHON3
ایستا تهی f_py3eval(typval_T *argvars, typval_T *rettv);
#endif
#ifdef FEAT_PYTHON
ایستا تهی f_pyeval(typval_T *argvars, typval_T *rettv);
#endif
#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
ایستا تهی f_pyxeval(typval_T *argvars, typval_T *rettv);
#endif
ایستا تهی f_test_srand_seed(typval_T *argvars, typval_T *rettv);
ایستا تهی f_rand(typval_T *argvars, typval_T *rettv);
ایستا تهی f_range(typval_T *argvars, typval_T *rettv);
ایستا تهی f_reg_executing(typval_T *argvars, typval_T *rettv);
ایستا تهی f_reg_recording(typval_T *argvars, typval_T *rettv);
ایستا تهی f_rename(typval_T *argvars, typval_T *rettv);
ایستا تهی f_repeat(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_RUBY
ایستا تهی f_rubyeval(typval_T *argvars, typval_T *rettv);
#endif
ایستا تهی f_screenattr(typval_T *argvars, typval_T *rettv);
ایستا تهی f_screenchar(typval_T *argvars, typval_T *rettv);
ایستا تهی f_screenchars(typval_T *argvars, typval_T *rettv);
ایستا تهی f_screencol(typval_T *argvars, typval_T *rettv);
ایستا تهی f_screenrow(typval_T *argvars, typval_T *rettv);
ایستا تهی f_screenstring(typval_T *argvars, typval_T *rettv);
ایستا تهی f_جستجو(typval_T *argvars, typval_T *rettv);
ایستا تهی f_searchdecl(typval_T *argvars, typval_T *rettv);
ایستا تهی f_searchpair(typval_T *argvars, typval_T *rettv);
ایستا تهی f_searchpairpos(typval_T *argvars, typval_T *rettv);
ایستا تهی f_searchpos(typval_T *argvars, typval_T *rettv);
ایستا تهی f_setcharpos(typval_T *argvars, typval_T *rettv);
ایستا تهی f_setcharsearch(typval_T *argvars, typval_T *rettv);
ایستا تهی f_setcursorcharpos(typval_T *argvars, typval_T *rettv);
ایستا تهی f_setenv(typval_T *argvars, typval_T *rettv);
ایستا تهی f_setfperm(typval_T *argvars, typval_T *rettv);
ایستا تهی f_setpos(typval_T *argvars, typval_T *rettv);
ایستا تهی f_setreg(typval_T *argvars, typval_T *rettv);
ایستا تهی f_settagstack(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_CRYPT
ایستا تهی f_sha256(typval_T *argvars, typval_T *rettv);
#endif
ایستا تهی f_shellescape(typval_T *argvars, typval_T *rettv);
ایستا تهی f_shiftwidth(typval_T *argvars, typval_T *rettv);
ایستا تهی f_soundfold(typval_T *argvars, typval_T *rettv);
ایستا تهی f_spellbadword(typval_T *argvars, typval_T *rettv);
ایستا تهی f_spellsuggest(typval_T *argvars, typval_T *rettv);
ایستا تهی f_split(typval_T *argvars, typval_T *rettv);
ایستا تهی f_srand(typval_T *argvars, typval_T *rettv);
ایستا تهی f_submatch(typval_T *argvars, typval_T *rettv);
ایستا تهی f_substitute(typval_T *argvars, typval_T *rettv);
ایستا تهی f_swapfilelist(typval_T *argvars, typval_T *rettv);
ایستا تهی f_swapinfo(typval_T *argvars, typval_T *rettv);
ایستا تهی f_swapname(typval_T *argvars, typval_T *rettv);
ایستا تهی f_synID(typval_T *argvars, typval_T *rettv);
ایستا تهی f_synIDattr(typval_T *argvars, typval_T *rettv);
ایستا تهی f_synIDtrans(typval_T *argvars, typval_T *rettv);
ایستا تهی f_synstack(typval_T *argvars, typval_T *rettv);
ایستا تهی f_synconcealed(typval_T *argvars, typval_T *rettv);
ایستا تهی f_tabpagebuflist(typval_T *argvars, typval_T *rettv);
ایستا تهی f_taglist(typval_T *argvars, typval_T *rettv);
ایستا تهی f_tagfiles(typval_T *argvars, typval_T *rettv);
ایستا تهی f_type(typval_T *argvars, typval_T *rettv);
ایستا تهی f_virtcol(typval_T *argvars, typval_T *rettv);
ایستا تهی f_visualmode(typval_T *argvars, typval_T *rettv);
ایستا تهی f_wildmenumode(typval_T *argvars, typval_T *rettv);
ایستا تهی f_windowsversion(typval_T *argvars, typval_T *rettv);
ایستا تهی f_wordcount(typval_T *argvars, typval_T *rettv);
ایستا تهی f_xor(typval_T *argvars, typval_T *rettv);


/*
 * Functions that check the argument type of a builtin function.
 * Each function returns FAIL and gives an error message if the type is wrong.
 */

// Context passed to an arg_ function.
تعریفنوع ساختار {
    صحیح		arg_شمار;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    صحیح		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
تعریفنوع صحیح (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    ایستا صحیح
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    بازگشت need_type(actual, expected, 0,
	    context->arg_idx - context->arg_شمار, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    ایستا صحیح
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    اگر (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_شمار, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	بازگشت FAIL;
    بازگشت arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    صحیح
arg_type_modifiable(type_T *type, صحیح arg_idx)
{
    نویسه *tofree;

    اگر ((type->tt_flags & TTFLAG_CONST) == 0)
	بازگشت OK;
    semsg(_(e_argument_nr_trying_to_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    بازگشت FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    ایستا صحیح
arg_any(type_T *type UNUSED,
	type_T *decl_type UNUSED,
	argcontext_T *context UNUSED)
{
    بازگشت OK;
}

/*
 * Check "type" is a float or a number.
 */
    ایستا صحیح
arg_float_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_عدد
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_عدد, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a number.
 */
    ایستا صحیح
arg_عدد(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    بازگشت check_arg_type(&t_عدد, type, context);
}

/*
 * Check "type" is an object.
 */
    ایستا صحیح
arg_object(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    ایستا صحیح
arg_dict_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    بازگشت check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    ایستا صحیح
arg_list_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    بازگشت check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    ایستا صحیح
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    بازگشت check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    ایستا صحیح
arg_list_عدد(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    بازگشت check_arg_type(&t_list_عدد, type, context);
}

/*
 * Check "type" is a list of strings.
 */
    ایستا صحیح
arg_list_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    بازگشت check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    ایستا صحیح
arg_tuple_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    بازگشت check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    ایستا صحیح
arg_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    بازگشت check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    ایستا صحیح
arg_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    بازگشت check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    ایستا صحیح
arg_bool(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    بازگشت check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    ایستا صحیح
arg_bool_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_عدد
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_عدد, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    ایستا صحیح
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	بازگشت OK;
    بازگشت check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    ایستا صحیح
arg_list_or_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    ایستا صحیح
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    اگر (arg_list_or_blob(type, decl_type, context) == FAIL)
	بازگشت FAIL;
    بازگشت arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    ایستا صحیح
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    بازگشت FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    ایستا صحیح
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a string or a number
 */
    ایستا صحیح
arg_string_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_عدد
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    ایستا صحیح
arg_buffer(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_عدد
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    ایستا صحیح
arg_buffer_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_عدد
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    ایستا صحیح
arg_lnum(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_عدد
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    ایستا صحیح
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	بازگشت OK;
    اگر (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	بازگشت FAIL;
    }
    اگر (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	بازگشت OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    ایستا صحیح
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    ایستا صحیح
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    ایستا صحیح
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    ایستا صحیح
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    ایستا صحیح
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    ایستا صحیح
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    اگر (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	بازگشت FAIL;
    بازگشت arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    ایستا صحیح
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    ایستا صحیح
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    اگر (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	بازگشت FAIL;
    بازگشت arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    ایستا صحیح
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	بازگشت OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr),
						 context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    ایستا صحیح
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = پوچ;
    type_T *(args[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, پوچ, پوچ, args};

    اگر (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	اگر (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	وگرنه
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    وگرنه اگر (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    وگرنه اگر (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_عدد;

    args[0] = پوچ;
    args[1] = &t_unknown;
    اگر (type->tt_argcount != -1)
    {
	اگر (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_عدد_of_arguments));
	    بازگشت FAIL;
	}
	اگر (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	وگرنه
	{
	    اگر (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		args[0] = &t_عدد;
	    وگرنه اگر (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		args[0] = &t_string;
	    اگر (args[0] != پوچ)
		args[1] = expected_member;
	}
    }

    اگر (!type_any_or_unknown(type->tt_member) || args[0] != پوچ)
    {
	where_T where = WHERE_INIT;

	اگر (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == پوچ
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	وگرنه اگر (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	وگرنه // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	اگر (args[0] == پوچ)
	    args[0] = &t_unknown;
	اگر (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_index = 2;
	where.wt_kind = WT_ARGUMENT;
	بازگشت check_type(&t_func_exp, type, TRUE, where);
    }
    بازگشت OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    ایستا صحیح
arg_filter_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	بازگشت OK;

    اگر (type->tt_type == VAR_FUNC)
	بازگشت check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    بازگشت FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    ایستا صحیح
arg_map_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	بازگشت OK;

    اگر (type->tt_type == VAR_FUNC)
	بازگشت check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    بازگشت FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    ایستا صحیح
arg_foreach_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	بازگشت OK;

    اگر (type->tt_type == VAR_FUNC)
	بازگشت check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    بازگشت FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    ایستا صحیح
arg_sort_how(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	بازگشت OK;

    اگر (type->tt_type == VAR_FUNC)
    {
	type_T *(args[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_عدد, پوچ, args};

	اگر (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    args[0] = context->arg_types[0].type_curr->tt_member;
	وگرنه
	    args[0] = &t_unknown;
	اگر (!type_any_or_unknown(type->tt_member) || args[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    args[1] = args[0];
	    اگر (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_index = 2;
	    where.wt_kind = WT_ARGUMENT;
	    بازگشت check_type(&t_func_exp, type, TRUE, where);
	}

	بازگشت OK;
    }
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    بازگشت FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    ایستا صحیح
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_عدد
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    ایستا صحیح
varargs_class(type_T *type UNUSED,
	      type_T *decl_type UNUSED,
	      argcontext_T *context)
{
    برای (صحیح i = context->arg_idx; i < context->arg_شمار; ++i)
    {
	type2_T *types = &context->arg_types[i];
	اگر (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    بازگشت FAIL;
	}
    }
    بازگشت OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    ایستا صحیح
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    ایستا صحیح
arg_reverse(type_T *type, type_T *decl_type, argcontext_T *context)
{
    اگر (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	بازگشت FAIL;
    بازگشت arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    ایستا صحیح
arg_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    بازگشت check_arg_type(&t_job, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    ایستا صحیح
arg_chan_or_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_JOB
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    ایستا صحیح
arg_same_as_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_decl;

    بازگشت check_arg_type(prev_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    ایستا صحیح
arg_same_struct_as_prev(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;

    اگر (prev_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	بازگشت check_arg_type(prev_type, type, context);
    بازگشت OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    ایستا صحیح
arg_item_of_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    اگر (prev_type->tt_type == VAR_LIST)
	expected = prev_type->tt_member;
    وگرنه اگر (prev_type->tt_type == VAR_BLOB)
	expected = &t_عدد;
    وگرنه
	// probably VAR_ANY, can't check
	بازگشت OK;

    بازگشت check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    ایستا صحیح
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_عدد
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    ایستا صحیح
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	بازگشت OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    ایستا صحیح
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 2].type_curr;

    اگر (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	بازگشت arg_عدد(type, decl_type, context);
    اگر (first_type->tt_type == VAR_DICT)
	بازگشت arg_string(type, decl_type, context);
    بازگشت OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    ایستا صحیح
arg_get1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	بازگشت OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    ایستا صحیح
arg_len1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_عدد
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	بازگشت OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    ایستا صحیح
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 1].type_curr;

    اگر (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	بازگشت arg_عدد(type, decl_type, context);
    اگر (first_type->tt_type == VAR_DICT)
	بازگشت arg_string_or_nr(type, decl_type, context);
    بازگشت OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    ایستا صحیح
arg_repeat1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_عدد
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	بازگشت OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    ایستا صحیح
arg_slice1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	بازگشت OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    ایستا صحیح
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	بازگشت OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr),
							 context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    ایستا صحیح
arg_cursor1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    اگر (type->tt_type == VAR_عدد
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	بازگشت OK;

    arg_type_mismatch(&t_عدد, type, context->arg_idx + 1);
    بازگشت FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
ایستا argcheck_T arg1_blob[] = {arg_blob};
ایستا argcheck_T arg1_bool[] = {arg_bool};
ایستا argcheck_T arg1_buffer[] = {arg_buffer};
ایستا argcheck_T arg1_buffer_or_dict_any[] = {arg_buffer_or_dict_any};
ایستا argcheck_T arg1_chan_or_job[] = {arg_chan_or_job};
ایستا argcheck_T arg1_dict_any[] = {arg_dict_any};
ایستا argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
ایستا argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
ایستا argcheck_T arg1_job[] = {arg_job};
ایستا argcheck_T arg1_list_any[] = {arg_list_any};
ایستا argcheck_T arg1_tuple_any[] = {arg_tuple_any};
ایستا argcheck_T arg1_list_عدد[] = {arg_list_عدد};
ایستا argcheck_T arg1_reverse[] = {arg_reverse};
ایستا argcheck_T arg1_list_or_tuple_or_dict[] = {arg_list_or_tuple_or_dict};
ایستا argcheck_T arg1_list_string[] = {arg_list_string};
ایستا argcheck_T arg1_list_tuple_dict_blob_or_string[] = {arg_list_tuple_dict_blob_or_string};
ایستا argcheck_T arg1_lnum[] = {arg_lnum};
ایستا argcheck_T arg1_عدد[] = {arg_عدد};
ایستا argcheck_T arg1_string[] = {arg_string};
ایستا argcheck_T arg1_string_or_list_any[] = {arg_string_or_list_any};
ایستا argcheck_T arg1_string_or_list_string[] = {arg_string_or_list_string};
ایستا argcheck_T arg1_string_or_nr[] = {arg_string_or_nr};
ایستا argcheck_T arg1_string_or_blob[] = {arg_string_or_blob};
ایستا argcheck_T arg2_buffer_any[] = {arg_buffer, arg_any};
ایستا argcheck_T arg2_buffer_bool[] = {arg_buffer, arg_bool};
ایستا argcheck_T arg2_buffer_list_any[] = {arg_buffer, arg_list_any};
ایستا argcheck_T arg2_buffer_lnum[] = {arg_buffer, arg_lnum};
ایستا argcheck_T arg2_buffer_عدد[] = {arg_buffer, arg_عدد};
ایستا argcheck_T arg2_buffer_string[] = {arg_buffer, arg_string};
ایستا argcheck_T arg2_chan_or_job_dict[] = {arg_chan_or_job, arg_dict_any};
ایستا argcheck_T arg2_chan_or_job_string[] = {arg_chan_or_job, arg_string};
ایستا argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
ایستا argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
ایستا argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
ایستا argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
ایستا argcheck_T arg2_job_dict[] = {arg_job, arg_dict_any};
ایستا argcheck_T arg2_job_string_or_عدد[] = {arg_job, arg_string_or_nr};
ایستا argcheck_T arg2_list_any_عدد[] = {arg_list_any, arg_عدد};
ایستا argcheck_T arg2_list_عدد[] = {arg_list_عدد, arg_list_عدد};
ایستا argcheck_T arg2_list_عدد_bool[] = {arg_list_عدد, arg_bool};
ایستا argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
ایستا argcheck_T arg2_listblobmod_item[] = {arg_list_or_blob_mod, arg_item_of_prev};
ایستا argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
ایستا argcheck_T arg2_lnum_عدد[] = {arg_lnum, arg_عدد};
ایستا argcheck_T arg2_عدد[] = {arg_عدد, arg_عدد};
ایستا argcheck_T arg2_عدد_any[] = {arg_عدد, arg_any};
ایستا argcheck_T arg2_عدد_bool[] = {arg_عدد, arg_bool};
ایستا argcheck_T arg2_عدد_dict_any[] = {arg_عدد, arg_dict_any};
ایستا argcheck_T arg2_عدد_list[] = {arg_عدد, arg_list_any};
ایستا argcheck_T arg2_عدد_string[] = {arg_عدد, arg_string};
ایستا argcheck_T arg2_عدد_buffer[] = {arg_عدد, arg_buffer};
ایستا argcheck_T arg2_عدد_string_or_list[] = {arg_عدد, arg_string_or_list_any};
ایستا argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
ایستا argcheck_T arg2_string[] = {arg_string, arg_string};
ایستا argcheck_T arg2_string_any[] = {arg_string, arg_any};
ایستا argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
ایستا argcheck_T arg2_string_chan_or_job[] = {arg_string, arg_chan_or_job};
ایستا argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
ایستا argcheck_T arg2_string_list_عدد[] = {arg_string, arg_list_عدد};
ایستا argcheck_T arg2_string_عدد[] = {arg_string, arg_عدد};
ایستا argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
ایستا argcheck_T arg2_string_or_list_عدد[] = {arg_string_or_list_any, arg_عدد};
ایستا argcheck_T arg2_string_string_or_عدد[] = {arg_string, arg_string_or_nr};
ایستا argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
ایستا argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
ایستا argcheck_T arg3_any_buffer_bool_or_dict[] = {
			      arg_any, arg_buffer, arg_bool_or_dict_any};
ایستا argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
ایستا argcheck_T arg3_buffer_lnum_lnum[] = {arg_buffer, arg_lnum, arg_lnum};
ایستا argcheck_T arg3_buffer_عدد_عدد[] = {arg_buffer, arg_عدد, arg_عدد};
ایستا argcheck_T arg3_buffer_string_any[] = {arg_buffer, arg_string, arg_any};
ایستا argcheck_T arg3_buffer_string_dict[] = {arg_buffer, arg_string, arg_dict_any};
ایستا argcheck_T arg3_dict_عدد_عدد[] = {arg_dict_any, arg_عدد, arg_عدد};
ایستا argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
ایستا argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
ایستا argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
ایستا argcheck_T arg3_lnum_عدد_bool[] = {arg_lnum, arg_عدد, arg_bool};
ایستا argcheck_T arg3_عدد[] = {arg_عدد, arg_عدد, arg_عدد};
ایستا argcheck_T arg3_عدد_any_dict[] = {arg_عدد, arg_any, arg_dict_any};
ایستا argcheck_T arg3_عدد_عدد_dict[] = {arg_عدد, arg_عدد, arg_dict_any};
ایستا argcheck_T arg3_عدد_string_any[] = {arg_عدد, arg_string, arg_any};
ایستا argcheck_T arg3_عدد_string_buffer[] = {arg_عدد, arg_string, arg_buffer};
ایستا argcheck_T arg3_عدد_string_string[] = {arg_عدد, arg_string, arg_string};
ایستا argcheck_T arg3_string[] = {arg_string, arg_string, arg_string};
ایستا argcheck_T arg3_string_any_dict[] = {arg_string, arg_any, arg_dict_any};
ایستا argcheck_T arg3_string_any_string[] = {arg_string, arg_any, arg_string};
ایستا argcheck_T arg3_string_bool_bool[] = {arg_string, arg_bool, arg_bool};
ایستا argcheck_T arg3_string_عدد_bool[] = {arg_string, arg_عدد, arg_bool};
ایستا argcheck_T arg3_string_عدد_عدد[] = {arg_string, arg_عدد, arg_عدد};
ایستا argcheck_T arg3_string_or_dict_bool_dict[] = {arg_string_or_dict_any, arg_bool, arg_dict_any};
ایستا argcheck_T arg3_string_or_list_bool_عدد[] = {arg_string_or_list_any, arg_bool, arg_عدد};
ایستا argcheck_T arg3_string_string_bool[] = {arg_string, arg_string, arg_bool};
ایستا argcheck_T arg3_string_string_dict[] = {arg_string, arg_string, arg_dict_any};
ایستا argcheck_T arg3_string_string_عدد[] = {arg_string, arg_string, arg_عدد};
ایستا argcheck_T arg4_عدد_عدد_string_any[] = {arg_عدد, arg_عدد, arg_string, arg_any};
ایستا argcheck_T arg4_string_string_any_string[] = {arg_string, arg_string, arg_any, arg_string};
ایستا argcheck_T arg4_string_string_عدد_string[] = {arg_string, arg_string, arg_عدد, arg_string};
ایستا argcheck_T arg4_string_عدد_bool_bool[] = {arg_string, arg_عدد, arg_bool, arg_bool};
/* Function specific argument types (not covered by the above) */
ایستا argcheck_T arg15_assert_fails[] = {arg_string_or_nr, arg_string_or_list_any, arg_any, arg_عدد, arg_string};
ایستا argcheck_T arg34_assert_inrange[] = {arg_float_or_nr, arg_float_or_nr, arg_float_or_nr, arg_string};
ایستا argcheck_T arg4_browse[] = {arg_bool, arg_string, arg_string, arg_string};
ایستا argcheck_T arg23_chanexpr[] = {arg_chan_or_job, arg_any, arg_dict_any};
ایستا argcheck_T arg23_chanraw[] = {arg_chan_or_job, arg_string_or_blob, arg_dict_any};
ایستا argcheck_T arg24_شمار[] = {arg_string_list_tuple_or_dict, arg_any, arg_bool, arg_عدد};
ایستا argcheck_T arg13_cursor[] = {arg_cursor1, arg_عدد, arg_عدد};
ایستا argcheck_T arg12_deepcopy[] = {arg_any, arg_bool};
ایستا argcheck_T arg12_execute[] = {arg_string_or_list_string, arg_string};
ایستا argcheck_T arg12_getchar[] = {arg_bool_or_nr, arg_dict_any};
ایستا argcheck_T arg23_extend[] = {arg_list_or_dict_or_blob_mod, arg_same_as_prev, arg_extend3};
ایستا argcheck_T arg23_extendnew[] = {arg_list_or_dict_or_blob, arg_same_struct_as_prev, arg_extend3};
ایستا argcheck_T arg23_get[] = {arg_get1, arg_string_or_nr, arg_any};
ایستا argcheck_T arg14_glob[] = {arg_string, arg_bool, arg_bool, arg_bool};
ایستا argcheck_T arg25_globpath[] = {arg_string, arg_string, arg_bool, arg_bool, arg_bool};
ایستا argcheck_T arg24_index[] = {arg_list_or_tuple_or_blob, arg_item_of_prev, arg_عدد, arg_bool};
ایستا argcheck_T arg23_index[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
ایستا argcheck_T arg23_درج[] = {arg_list_or_blob, arg_item_of_prev, arg_عدد};
ایستا argcheck_T arg1_len[] = {arg_len1};
ایستا argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
ایستا argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
ایستا argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
ایستا argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
ایستا argcheck_T arg2_instanceof[] = {arg_object, varargs_class, پوچ };
ایستا argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
ایستا argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
ایستا argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_عدد, arg_عدد, arg_dict_any};
ایستا argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_عدد, arg_عدد, arg_dict_any};
ایستا argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
ایستا argcheck_T arg45_matchbufline[] = {arg_buffer, arg_string, arg_lnum, arg_lnum, arg_dict_any};
ایستا argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
ایستا argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
ایستا argcheck_T arg24_remote_عبارت[] = {arg_string, arg_string, arg_string, arg_عدد};
ایستا argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_عدد};
ایستا argcheck_T arg2_repeat[] = {arg_repeat1, arg_عدد};
ایستا argcheck_T arg15_جستجو[] = {arg_string, arg_string, arg_عدد, arg_عدد, arg_string_or_func};
ایستا argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_عدد, arg_عدد};
ایستا argcheck_T arg3_setbufline[] = {arg_buffer, arg_lnum, arg_str_or_nr_or_list};
ایستا argcheck_T arg2_setline[] = {arg_lnum, arg_any};
ایستا argcheck_T arg24_setloclist[] = {arg_عدد, arg_list_any, arg_string, arg_dict_any};
ایستا argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
ایستا argcheck_T arg23_settagstack[] = {arg_عدد, arg_dict_any, arg_string};
ایستا argcheck_T arg02_sign_getplaced[] = {arg_buffer, arg_dict_any};
ایستا argcheck_T arg45_sign_place[] = {arg_عدد, arg_string, arg_string, arg_buffer, arg_dict_any};
ایستا argcheck_T arg23_slice[] = {arg_slice1, arg_عدد, arg_عدد};
ایستا argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_sort_how, arg_dict_any};
ایستا argcheck_T arg24_strpart[] = {arg_string, arg_عدد, arg_عدد, arg_bool};
ایستا argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
ایستا argcheck_T arg23_win_execute[] = {arg_عدد, arg_string_or_list_string, arg_string};
ایستا argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
ایستا argcheck_T arg24_match_func[] = {arg_string_or_list_any, arg_string, arg_عدد, arg_عدد};

// Can be used by functions called through "f_retfunc" to create new types.
ایستا garray_T *current_type_gap = پوچ;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    ایستا type_T *
ret_void(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_void;
}
    ایستا type_T *
ret_any(صحیح	argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_any;
}
    ایستا type_T *
ret_bool(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_bool;
}
    ایستا type_T *
ret_عدد_bool(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_عدد_bool;
}
    ایستا type_T *
ret_عدد(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_عدد;
}
    ایستا type_T *
ret_float(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_float;
}
    ایستا type_T *
ret_string(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_string;
}
    ایستا type_T *
ret_list_any(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_list_any;
}
    ایستا type_T *
ret_list_عدد(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    بازگشت &t_list_عدد;
}
    ایستا type_T *
ret_list_string(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    بازگشت &t_list_string;
}
    ایستا type_T *
ret_list_dict_any(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    بازگشت &t_list_dict_any;
}
    ایستا type_T *
ret_list_اقلام(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    بازگشت &t_list_list_any;
}
    ایستا type_T *
ret_list_list_any(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    بازگشت &t_list_list_any;
}
    ایستا type_T *
ret_list_string_اقلام(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    بازگشت &t_list_list_string;
}
    ایستا type_T *
ret_list_regionpos(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    بازگشت &t_list_list_list_عدد;
}
    ایستا type_T *
ret_tuple_any(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_tuple_any;
}
    ایستا type_T *
ret_dict_any(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_dict_any;
}
    ایستا type_T *
ret_job_info(صحیح argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    اگر (argcount == 0)
    {
	*decl_type = &t_list_any;
	بازگشت &t_list_job;
    }
    بازگشت &t_dict_any;
}
    ایستا type_T *
ret_dict_عدد(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_dict_عدد;
}
    ایستا type_T *
ret_dict_string(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_dict_string;
}
    ایستا type_T *
ret_blob(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_blob;
}
    ایستا type_T *
ret_func_any(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_func_any;
}
    ایستا type_T *
ret_func_unknown(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_func_unknown;
}
    ایستا type_T *
ret_channel(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_channel;
}
    ایستا type_T *
ret_job(صحیح argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    بازگشت &t_job;
}
    ایستا type_T *
ret_first_arg(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	بازگشت argtypes[0].type_curr;
    }
    بازگشت &t_void;
}
    ایستا type_T *
ret_slice(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	اگر (argtypes[0].type_decl != پوچ)
	{
	    انتخاب (argtypes[0].type_decl->tt_type)
	    {
		حالت VAR_STRING: *decl_type = &t_string; بشکن;
		حالت VAR_BLOB: *decl_type = &t_blob; بشکن;
		حالت VAR_LIST: *decl_type = &t_list_any; بشکن;
		حالت VAR_TUPLE: *decl_type = &t_tuple_any; بشکن;
		پیشفرض: بشکن;
	    }
	}
	بازگشت argtypes[0].type_curr;
    }
    بازگشت &t_void;
}
    ایستا type_T *
ret_copy(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	اگر (argtypes[0].type_decl != پوچ)
	{
	    اگر (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    وگرنه اگر (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    وگرنه
		*decl_type = argtypes[0].type_decl;
	}
	اگر (argtypes[0].type_curr != پوچ)
	{
	    اگر (argtypes[0].type_curr->tt_type == VAR_LIST)
		بازگشت &t_list_any;
	    وگرنه اگر (argtypes[0].type_curr->tt_type == VAR_DICT)
		بازگشت &t_dict_any;
	}
	بازگشت argtypes[0].type_curr;
    }
    بازگشت &t_void;
}
    ایستا type_T *
ret_extend(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	// if the second argument has a different current type then the current
	// type is "any"
	اگر (argcount > 1 && !equal_type(argtypes[0].type_curr,
						     argtypes[1].type_curr, 0))
	{
	    اگر (argtypes[0].type_curr->tt_type == VAR_LIST)
		بازگشت &t_list_any;
	    اگر (argtypes[0].type_curr->tt_type == VAR_DICT)
		بازگشت &t_dict_any;
	    اگر (argtypes[0].type_curr->tt_type == VAR_BLOB)
		بازگشت &t_blob;
	}
	بازگشت argtypes[0].type_curr;
    }
    بازگشت &t_void;
}
    ایستا type_T *
ret_repeat(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    اگر (argcount == 0)
	بازگشت &t_any;
    اگر (argtypes[0].type_curr == &t_عدد)
	بازگشت &t_string;
    بازگشت argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    ایستا type_T *
ret_first_cont(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    اگر (argcount > 0)
    {
	اگر (argtypes[0].type_curr->tt_type == VAR_LIST)
	    بازگشت &t_list_any;
	اگر (argtypes[0].type_curr->tt_type == VAR_DICT)
	    بازگشت &t_dict_any;
	اگر (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    بازگشت argtypes[0].type_curr;
    }
    بازگشت &t_any;
}
// for get(): the type of the item when a missing item has the same type
    ایستا type_T *
ret_get(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*item = پوچ;

	اگر (t->tt_type == VAR_BLOB)
	    item = &t_عدد;
	وگرنه اگر (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    item = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	اگر (item != پوچ && (argcount == 3
			? equal_type(item, argtypes[2].type_curr, 0)
			: item->tt_type == VAR_عدد))
	{
	    اگر (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    بازگشت item;
	}
    }
    بازگشت &t_any;
}
// for getline()
    ایستا type_T *
ret_getline(صحیح argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    اگر (argcount == 1)
	بازگشت &t_string;
    *decl_type = &t_list_any;
    بازگشت &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    ایستا type_T *
ret_string_or_any_1(صحیح argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    اگر (argcount < 1)
	بازگشت &t_string;
    بازگشت &t_any;
}
    ایستا type_T *
ret_string_or_any_2(صحیح argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    اگر (argcount < 2)
	بازگشت &t_string;
    بازگشت &t_any;
}
    ایستا type_T *
ret_string_or_any_3(صحیح argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    اگر (argcount < 3)
	بازگشت &t_string;
    بازگشت &t_any;
}
    ایستا type_T *
ret_string_or_any_4(صحیح argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    اگر (argcount < 4)
	بازگشت &t_string;
    بازگشت &t_any;
}
// for abs()
    ایستا type_T *
ret_abs(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    اگر (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	بازگشت &t_float;
    بازگشت &t_عدد;
}
// for sign_define()
    ایستا type_T *
ret_sign_define(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    اگر (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	بازگشت &t_list_عدد;
    بازگشت &t_عدد;
}
// for values(): list of member of first argument
    ایستا type_T *
ret_list_member(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	اگر (current_type_gap != پوچ
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	وگرنه
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	اگر (current_type_gap != پوچ
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    بازگشت get_list_type(t->tt_member, current_type_gap);
    }
    بازگشت &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    ایستا type_T *
ret_list_or_dict_0(صحیح argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    اگر (argcount > 0)
	بازگشت &t_dict_any;
    *decl_type = &t_list_any;
    بازگشت &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    ایستا type_T *
ret_list_or_dict_1(صحیح argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    اگر (argcount > 1)
	بازگشت &t_dict_any;
    *decl_type = &t_list_any;
    بازگشت &t_list_dict_any;
}

    ایستا type_T *
ret_argv(صحیح argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // argv() returns list of strings
    اگر (argcount == 0)
    {
	*decl_type = &t_list_any;
	بازگشت &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    بازگشت &t_any;
}

    ایستا type_T *
ret_remove(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	اگر (argtypes[0].type_curr->tt_type == VAR_LIST
		|| argtypes[0].type_curr->tt_type == VAR_DICT)
	{
	    اگر (argcount == 3)
	    {
		*decl_type = argtypes[0].type_decl;
		بازگشت argtypes[0].type_curr;
	    }
	    اگر (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    بازگشت argtypes[0].type_curr->tt_member;
	}
	اگر (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    بازگشت &t_عدد;
    }
    بازگشت &t_any;
}

    ایستا type_T *
ret_max_min(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	اگر (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	وگرنه
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	اگر (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	وگرنه
	    t = &t_any;
	بازگشت t;
    }
    بازگشت &t_any;
}

    ایستا type_T *
ret_getreg(صحیح argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    اگر (argcount == 3)
    {
	*decl_type = &t_list_any;
	بازگشت &t_list_string;
    }
    بازگشت &t_string;
}

    ایستا type_T *
ret_virtcol(صحیح argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    اگر (argcount > 1)
    {
	*decl_type = &t_list_any;
	بازگشت &t_list_عدد;
    }
    بازگشت &t_عدد;
}

    ایستا type_T *
ret_maparg(صحیح argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    // Assume that if the fourth argument is passed it's non-zero
    اگر (argcount == 4)
	بازگشت &t_dict_any;
    بازگشت &t_string;
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
تعریفنوع ساختار
{
    نویسه	*f_name;	// function name
    نویسه	f_min_argc;	// minimal number of arguments
    نویسه	f_max_argc;	// maximal number of arguments
    نویسه	f_argtype;	// for method: FEARG_ values; bits FE_
    argcheck_T	*f_argcheck;	// list of functions to check argument types;
				// use "arg_any" (not NULL) to accept an
				// argument of any type
    نویسه	*f_argnames;	// names of the arguments as in the help,
				// separated by ", "; an argument without a
				// name is empty, NULL when there is none
    type_T	*(*f_retfunc)(صحیح argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    تهی	(*f_func)(typval_T *args, typval_T *rvar);
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

#اگر defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#وگرنه
# define MATH_FUNC(name) پوچ
#endif
#ifdef FEAT_TIMERS
# define TIMER_FUNC(name) name
#وگرنه
# define TIMER_FUNC(name) پوچ
#endif
#ifdef FEAT_JOB_CHANNEL
# define JOB_FUNC(name) name
#وگرنه
# define JOB_FUNC(name) پوچ
#endif
#ifdef FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#وگرنه
# define PROP_FUNC(name) پوچ
#endif
#ifdef FEAT_SIGNS
# define SIGN_FUNC(name) name
#وگرنه
# define SIGN_FUNC(name) پوچ
#endif
#ifdef FEAT_SOUND
# define SOUND_FUNC(name) name
#وگرنه
# define SOUND_FUNC(name) پوچ
#endif
#ifdef FEAT_TERMINAL
# define TERM_FUNC(name) name
#وگرنه
# define TERM_FUNC(name) پوچ
#endif
#ifdef FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#وگرنه
# define TABPANEL_FUNC(name) پوچ
#endif
#ifdef FEAT_IMAGE
# define IMAGE_FUNC(name) name
#وگرنه
# define IMAGE_FUNC(name) پوچ
#endif

ایستا ثابت funcentry_T global_functions[] =
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
    {"and",		2, 2, FEARG_1,	    arg2_عدد,
			"expr1, expr2",
			ret_عدد,	    f_and},
    {"append",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_عدد_bool,    f_append},
    {"appendbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_عدد_bool,    f_appendbufline},
    {"argc",		0, 1, 0,	    arg1_عدد,
			"winid",
			ret_عدد,	    f_argc},
    {"argidx",		0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_argidx},
    {"arglistid",	0, 2, 0,	    arg2_عدد,
			"winnr, tabnr",
			ret_عدد,	    f_arglistid},
    {"argv",		0, 2, 0,	    arg2_عدد,
			"nr, winid",
			ret_argv,	    f_argv},
    {"asin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_asin},
    {"assert_beeps",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_عدد_bool,    f_assert_beeps},
    {"assert_equal",	2, 3, FEARG_2,	    پوچ,
			"expected, actual, msg",
			ret_عدد_bool,    f_assert_equal},
    {"assert_equalfile", 2, 3, FEARG_1,	    arg3_string,
			"fname_one, fname_two, msg",
			ret_عدد_bool,    f_assert_equalfile},
    {"assert_exception", 1, 2, 0,	    arg2_string,
			"error, msg",
			ret_عدد_bool,    f_assert_exception},
    {"assert_fails",	1, 5, FEARG_1,	    arg15_assert_fails,
			"cmd, error, msg, lnum, context",
			ret_عدد_bool,    f_assert_fails},
    {"assert_false",	1, 2, FEARG_1,	    پوچ,
			"actual, msg",
			ret_عدد_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_عدد_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_عدد_bool,    f_assert_match},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_عدد_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    پوچ,
			"expected, actual, msg",
			ret_عدد_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_عدد_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_عدد_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    پوچ,
			"actual, msg",
			ret_عدد_bool,    f_assert_true},
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
    {"balloon_gettext",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,
#ifdef FEAT_BEVAL
	    f_balloon_gettext
#وگرنه
	    پوچ
#endif
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#ifdef FEAT_BEVAL
	    f_balloon_show
#وگرنه
	    پوچ
#endif
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#اگر defined(FEAT_BEVAL_TERM)
	    f_balloon_split
#وگرنه
	    پوچ
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
			ret_list_عدد,    f_blob2list},
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
			ret_عدد,	    f_bufadd},
    {"bufexists",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_عدد_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_عدد_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_عدد,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_عدد_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_عدد_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, create",
			ret_عدد,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_عدد,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_عدد,	    f_bufwinnr},
    {"byte2line",	1, 1, FEARG_1,	    arg1_عدد,
			"byte",
			ret_عدد,	    f_byte2line},
    {"byteidx",		2, 3, FEARG_1,	    arg3_string_عدد_bool,
			"expr, nr, utf16",
			ret_عدد,	    f_byteidx},
    {"byteidxcomp",	2, 3, FEARG_1,	    arg3_string_عدد_bool,
			"expr, nr, utf16",
			ret_عدد,	    f_byteidxcomp},
    {"call",		2, 3, FEARG_1,	    arg3_any_list_dict,
			"func, arglist, dict",
			ret_any,	    f_call},
    {"ceil",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ceil},
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_job,
			"handle",
			ret_عدد_bool,    JOB_FUNC(f_ch_canread)},
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
			ret_عدد,	    JOB_FUNC(f_ch_getbufnr)},
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
    {"changenr",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_changenr},
    {"char2nr",		1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_عدد,	    f_char2nr},
    {"charclass",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_عدد,	    f_charclass},
    {"charcol",		1, 2, FEARG_1,	    arg2_string_or_list_عدد,
			"expr, winid",
			ret_عدد,	    f_charcol},
    {"charidx",		2, 4, FEARG_1,	    arg4_string_عدد_bool_bool,
			"string, idx, countcc, utf16",
			ret_عدد,	    f_charidx},
    {"chdir",		1, 2, FEARG_1,	    arg2_string,
			"dir, scope",
			ret_string,	    f_chdir},
    {"cindent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_عدد,	    f_cindent},
    {"clearmatches",	0, 1, FEARG_1,	    arg1_عدد,
			"win",
			ret_void,	    f_clearmatches},
    {"cmdcomplete_info",0, 0, 0,	    پوچ,
			پوچ,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_عدد,
			"expr, winid",
			ret_عدد,	    f_col},
    {"complete",	2, 2, FEARG_2,	    arg2_عدد_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_عدد,	    f_complete_add},
    {"complete_check",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_عدد_string,
			"msg, choices, default, type",
			ret_عدد,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    پوچ,
			"expr",
			ret_copy,	    f_copy},
    {"cos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cos},
    {"cosh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cosh},
    {"count",		2, 4, FEARG_1,	    arg24_شمار,
			"comp, expr, ic, start",
			ret_عدد,	    f_شمار},
    {"cscope_connection",0,3, 0,	    arg3_عدد_string_string,
			"num, dbpath, prepend",
			ret_عدد,	    f_cscope_connection},
    {"cursor",		1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_عدد,	    f_cursor},
    {"debugbreak",	1, 1, FEARG_1,	    arg1_عدد,
			"pid",
			ret_عدد,
#ifdef MSWIN
	    f_debugbreak
#وگرنه
	    پوچ
#endif
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_عدد_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_buffer_lnum_lnum,
			"buf, first, last",
			ret_عدد_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد_bool,    f_did_filetype},
    {"diff",		2, 3, FEARG_1,	    arg3_diff,
			"fromlist, tolist, options",
			ret_string_or_any_3,  f_diff},
    {"diff_filler",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_عدد,	    f_diff_filler},
    {"diff_hlID",	2, 2, FEARG_1,	    arg2_lnum_عدد,
			"lnum, col",
			ret_عدد,	    f_diff_hlID},
    {"digraph_get",	1, 1, FEARG_1,	    arg1_string,
			"chars",
			ret_string,	    f_digraph_get},
    {"digraph_getlist",	0, 1, FEARG_1,	    arg1_bool,
			"listall",
			ret_list_string_اقلام, f_digraph_getlist},
    {"digraph_set",	2, 2, FEARG_1,	    arg2_string,
			"chars, digraph",
			ret_bool,	f_digraph_set},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    پوچ,
			"expr",
			ret_عدد_bool,    f_empty},
    {"environ",		0, 0, 0,	    پوچ,
			پوچ,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    پوچ,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد_bool,    f_eventhandler},
    {"executable",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_عدد,	    f_executable},
    {"execute",		1, 2, FEARG_1,	    arg12_execute,
			"command, silent",
			ret_string,	    f_execute},
    {"exepath",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_exepath},
    {"exists",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_عدد_bool,    f_exists},
    {"exists_compiled",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_عدد_bool,    f_exists_compiled},
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
			ret_عدد_bool,    f_filereadable},
    {"filecopy",	2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_عدد_bool,    f_filecopy},
    {"filereadable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_عدد_bool,    f_filereadable},
    {"filewritable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_عدد,	    f_filewritable},
    {"filter",		2, 2, FEARG_1,	    arg2_filter,
			"expr1, expr2",
			ret_first_arg,	    f_filter},
    {"finddir",		1, 3, FEARG_1,	    arg3_string_string_عدد,
			"name, path, count",
			ret_string_or_any_3,	    f_finddir},
    {"findfile",	1, 3, FEARG_1,	    arg3_string_string_عدد,
			"name, path, count",
			ret_string_or_any_3,	    f_findfile},
    {"flatten",		1, 2, FEARG_1,	    arg2_list_any_عدد,
			"list, maxdepth",
			ret_list_any,	    f_flatten},
    {"flattennew",	1, 2, FEARG_1,	    arg2_list_any_عدد,
			"list, maxdepth",
			ret_list_any,	    f_flattennew},
    {"float2nr",	1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_عدد,	    f_float2nr},
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
    {"foldclosed",	1, 2, FEARG_1,	    arg2_lnum_عدد,
			"lnum, winid",
			ret_عدد,	    f_foldclosed},
    {"foldclosedend",	1, 2, FEARG_1,	    arg2_lnum_عدد,
			"lnum, winid",
			ret_عدد,	    f_foldclosedend},
    {"foldlevel",	1, 2, FEARG_1,	    arg2_lnum_عدد,
			"lnum, winid",
			ret_عدد,	    f_foldlevel},
    {"foldtext",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_عدد,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_first_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    پوچ,
			پوچ,
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
    {"getbgcolor",	0, 0, 0,	    پوچ,
			پوچ,
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
    {"getcellpixels",	0, 0, 0,	    پوچ,
			پوچ,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    پوچ,
			پوچ,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_عدد,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    پوچ,
			پوچ,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_getcmdwintype},
    {"getcompletion",	2, 3, FEARG_1,	    arg3_string_string_bool,
			"pat, type, filtered",
			ret_list_string,    f_getcompletion},
    {"getcompletiontype", 1, 1, FEARG_1,    arg1_string,
			"pat",
			ret_string,	    f_getcompletiontype},
    {"getcurpos",	0, 1, FEARG_1,	    arg1_عدد,
			"winid",
			ret_list_عدد,    f_getcurpos},
    {"getcursorcharpos", 0, 1, FEARG_1,	    arg1_عدد,
			"winid",
			ret_list_عدد,    f_getcursorcharpos},
    {"getcwd",		0, 2, FEARG_1,	    arg2_عدد,
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
			ret_عدد,	    f_getfsize},
    {"getftime",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_عدد,	    f_getftime},
    {"getftype",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getftype},
    {"getimstatus",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد_bool,    f_getimstatus},
    {"getinfo",		2, 3, FEARG_2,	    arg3_string_string_dict,
			"kind, name, opts",
			ret_dict_any,	    f_getinfo},
    {"getjumplist",	0, 2, FEARG_1,	    arg2_عدد,
			"winnr, tabnr",
			ret_list_any,	    f_getjumplist},
    {"getline",		1, 2, FEARG_1,	    arg2_lnum,
			"lnum, end",
			ret_getline,	    f_getline},
    {"getloclist",	1, 2, 0,	    arg2_عدد_dict_any,
			"nr, what",
			ret_list_or_dict_1, f_getloclist},
    {"getmarklist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_عدد,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    پوچ,
			پوچ,
			ret_dict_عدد,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_getpid},
    {"getpos",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_عدد,    f_getpos},
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
    {"getstacktrace",	0, 0, 0,	    پوچ,
			پوچ,
			ret_list_dict_any,  f_getstacktrace},
    {"gettabinfo",	0, 1, FEARG_1,	    arg1_عدد,
			"tabnr",
			ret_list_dict_any,  f_gettabinfo},
    {"gettabvar",	2, 3, FEARG_1,	    arg3_عدد_string_any,
			"tabnr, varname, def",
			ret_any,	    f_gettabvar},
    {"gettabwinvar",	3, 4, FEARG_1,	    arg4_عدد_عدد_string_any,
			"tabnr, winnr, varname, def",
			ret_any,	    f_gettabwinvar},
    {"gettagstack",	0, 1, FEARG_1,	    arg1_عدد,
			"winnr",
			ret_dict_any,	    f_gettagstack},
    {"gettext",		1, 2, FEARG_1,	    arg2_string,
			"text, package",
			ret_string,	    f_gettext},
    {"getwininfo",	0, 1, FEARG_1,	    arg1_عدد,
			"winid",
			ret_list_dict_any,  f_getwininfo},
    {"getwinpos",	0, 1, FEARG_1,	    arg1_عدد,
			"timeout",
			ret_list_عدد,    f_getwinpos},
    {"getwinposx",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_getwinposy},
    {"getwinvar",	2, 3, FEARG_1,	    arg3_عدد_string_any,
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
			ret_عدد_bool,    f_has},
    {"has_key",		2, 2, FEARG_1,	    arg2_dict_any_string_or_nr,
			"dict, key",
			ret_عدد_bool,    f_has_کلید},
    {"haslocaldir",	0, 2, FEARG_1,	    arg2_عدد,
			"winnr, tabnr",
			ret_عدد,	    f_haslocaldir},
    {"hasmapto",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"what, mode, abbr",
			ret_عدد_bool,    f_hasmapto},
    {"highlightID",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_عدد,	    f_hlID},
    {"highlight_exists",1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_عدد_bool,    f_hlexists},
    {"histadd",		2, 2, FEARG_2,	    arg2_string,
			"history, item",
			ret_عدد_bool,    f_histadd},
    {"histdel",		1, 2, FEARG_1,	    arg2_string_string_or_عدد,
			"history, item",
			ret_عدد_bool,    f_histdel},
    {"histget",		1, 2, FEARG_1,	    arg2_string_عدد,
			"history, index",
			ret_string,	    f_histget},
    {"histnr",		1, 1, FEARG_1,	    arg1_string,
			"history",
			ret_عدد,	    f_histnr},
    {"hlID",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_عدد,	    f_hlID},
    {"hlexists",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_عدد_bool,    f_hlexists},
    {"hlget",		0, 2, FEARG_1,	    arg2_string_bool,
			"name, resolve",
			ret_list_dict_any,  f_hlget},
    {"hlset",		1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_عدد_bool,    f_hlset},
    {"hostname",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    پوچ,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_عدد,	    IMAGE_FUNC(f_image_add)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_عدد,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_عدد,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_عدد,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_index,
			"object, expr, start, ic",
			ret_عدد,	    f_index},
    {"indexof",		2, 3, FEARG_1,	    arg23_index,
			"object, expr, opts",
			ret_عدد,	    f_indexof},
    {"input",		1, 3, FEARG_1,	    arg3_string,
			"prompt, text, completion",
			ret_string,	    f_درون_داد},
    {"inputdialog",	1, 3, FEARG_1,	    arg3_string,
			"prompt, text, cancelreturn",
			ret_string,	    f_inputdialog},
    {"inputlist",	1, 1, FEARG_1,	    arg1_list_string,
			"textlist",
			ret_عدد,	    f_inputlist},
    {"inputrestore",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_درج,
			"object, item, idx",
			ret_first_arg,	    f_درج},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    پوچ,
			پوچ,
			ret_void,	    f_interrupt},
    {"invert",		1, 1, FEARG_1,	    arg1_عدد,
			"expr",
			ret_عدد,	    f_invert},
    {"isabsolutepath",	1, 1, FEARG_1,	    arg1_string,
			"path",
			ret_عدد_bool,    f_isabsolutepath},
    {"isdirectory",	1, 1, FEARG_1,	    arg1_string,
			"directory",
			ret_عدد_bool,    f_isdirectory},
    {"isinf",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_عدد,	    MATH_FUNC(f_isinf)},
    {"islocked",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_عدد_bool,    f_islocked},
    {"isnan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_عدد_bool,    MATH_FUNC(f_isnan)},
    {"items",		1, 1, FEARG_1,	    arg1_list_tuple_dict_blob_or_string,
			"expr",
			ret_list_اقلام,	    f_اقلام},
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
    {"job_stop",	1, 2, FEARG_1,	    arg2_job_string_or_عدد,
			"job, how",
			ret_عدد_bool,    JOB_FUNC(f_job_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    پوچ,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    پوچ,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_کلیدها},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    پوچ,	// obsolete
			پوچ,
			ret_عدد,	    f_last_buffer_nr},
    {"len",		1, 1, FEARG_1,	    arg1_len,
			"expr",
			ret_عدد,	    f_len},
    {"libcall",		3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_string,	    f_libcall},
    {"libcallnr",	3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_عدد,	    f_libcallnr},
    {"line",		1, 2, FEARG_1,	    arg2_string_عدد,
			"expr, winid",
			ret_عدد,	    f_سطر},
    {"line2byte",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_عدد,	    f_line2byte},
    {"lispindent",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_عدد,	    f_lispindent},
    {"list2blob",	1, 1, FEARG_1,	    arg1_list_عدد,
			"list",
			ret_blob,	    f_list2blob},
    {"list2str",	1, 2, FEARG_1,	    arg2_list_عدد_bool,
			"list, utf8",
			ret_string,	    f_list2str},
    {"list2tuple",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_tuple_any,	    f_list2tuple},
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_buffer_bool_or_dict,
			"callback, buf, options",
			ret_عدد,	    f_listener_add},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_عدد,
			"id",
			ret_عدد_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_localtime},
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
#وگرنه
		پوچ
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
			ret_عدد,	    f_match},
    {"matchadd",	2, 5, FEARG_1,	    arg25_matchadd,
			"group, pattern, priority, id, dict",
			ret_عدد,	    f_matchadd},
    {"matchaddpos",	2, 5, FEARG_1,	    arg25_matchaddpos,
			"group, pos, priority, id, dict",
			ret_عدد,	    f_matchaddpos},
    {"matcharg",	1, 1, FEARG_1,	    arg1_عدد,
			"nr",
			ret_list_string,    f_matcharg},
    {"matchbufline",	4, 5, FEARG_1,	    arg45_matchbufline,
			"buf, pat, lnum, end, dict",
			ret_list_any,	    f_matchbufline},
    {"matchdelete",	1, 2, FEARG_1,	    arg2_عدد,
			"id, win",
			ret_عدد_bool,    f_matchdelete},
    {"matchend",	2, 4, FEARG_1,	    arg24_match_func,
			"expr, pat, start, count",
			ret_عدد,	    f_matchend},
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
#وگرنه
	    پوچ
#endif
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_عدد,
			"name, flags, prot",
			ret_عدد_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_mode},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_MZSCHEME
	    f_mzeval
#وگرنه
	    پوچ
#endif
			},
    {"nextnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_عدد,	    f_nextnonblank},
    {"ngettext",	3, 4, FEARG_3,	    arg4_string_string_عدد_string,
			"single, plural, number, domain",
			ret_string,	    f_ngettext},
    {"nr2char",		1, 2, FEARG_1,	    arg2_عدد_bool,
			"expr, utf8",
			ret_string,	    f_nr2char},
    {"or",		2, 2, FEARG_1,	    arg2_عدد,
			"expr1, expr2",
			ret_عدد,	    f_or},
    {"pathshorten",	1, 2, FEARG_1,	    arg2_string_عدد,
			"path, len",
			ret_string,	    f_pathshorten},
    {"perleval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_PERL
	    f_perleval
#وگرنه
	    پوچ
#endif
			},
    {"popup_atcursor",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_عدد,	    PROP_FUNC(f_popup_atcursor)},
    {"popup_beval",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_عدد,	    PROP_FUNC(f_popup_beval)},
    {"popup_clear",	0, 1, 0,	    arg1_bool,
			"force",
			ret_void,	    PROP_FUNC(f_popup_clear)},
    {"popup_close",	1, 2, FEARG_1,	    arg2_عدد_any,
			"id, result",
			ret_void,	    PROP_FUNC(f_popup_close)},
    {"popup_create",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_عدد,	    PROP_FUNC(f_popup_create)},
    {"popup_dialog",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_عدد,	    PROP_FUNC(f_popup_dialog)},
    {"popup_filter_menu", 2, 2, 0,	    arg2_عدد_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_menu)},
    {"popup_filter_yesno", 2, 2, 0,	    arg2_عدد_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_yesno)},
    {"popup_findecho",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    PROP_FUNC(f_popup_findpreview)},
    {"popup_getoptions", 1, 1, FEARG_1,	    arg1_عدد,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getoptions)},
    {"popup_getpos",	1, 1, FEARG_1,	    arg1_عدد,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getpos)},
    {"popup_hide",	1, 1, FEARG_1,	    arg1_عدد,
			"id",
			ret_void,	    PROP_FUNC(f_popup_hide)},
    {"popup_list",	0, 0, 0,	    پوچ,
			پوچ,
			ret_list_عدد,    PROP_FUNC(f_popup_list)},
    {"popup_locate",	2, 2, 0,	    arg2_عدد,
			"row, col",
			ret_عدد,	    PROP_FUNC(f_popup_locate)},
    {"popup_menu",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_عدد,	    PROP_FUNC(f_popup_menu)},
    {"popup_move",	2, 2, FEARG_1,	    arg2_عدد_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_move)},
    {"popup_notification", 2, 2, FEARG_1,   arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_عدد,	    PROP_FUNC(f_popup_notification)},
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_عدد_buffer,
			"id, buf",
			ret_عدد_bool,    PROP_FUNC(f_popup_setbuf)},
    {"popup_setoptions", 2, 2, FEARG_1,	    arg2_عدد_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_setoptions)},
    {"popup_settext",	2, 2, FEARG_1,	    arg2_عدد_string_or_list,
			"id, text",
			ret_void,	    PROP_FUNC(f_popup_settext)},
    {"popup_show",	1, 1, FEARG_1,	    arg1_عدد,
			"id",
			ret_عدد,	    PROP_FUNC(f_popup_show)},
    {"pow",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"x, y",
			ret_float,	    f_pow},
    {"preinserted",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_عدد,	    f_prevnonblank},
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
    {"prop_add",	3, 3, FEARG_1,	    arg3_عدد_عدد_dict,
			"lnum, col, props",
			ret_عدد,	    PROP_FUNC(f_prop_add)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_add_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_عدد_عدد_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_find)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_عدد_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_عدد_عدد,
			"props, lnum, lnum_end",
			ret_عدد,	    PROP_FUNC(f_prop_remove)},
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
    {"pum_getpos",	0, 0, 0,	    پوچ,
			پوچ,
			ret_dict_عدد,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON3
	    f_py3eval
#وگرنه
	    پوچ
#endif
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON
	    f_pyeval
#وگرنه
	    پوچ
#endif
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#وگرنه
	    پوچ
#endif
			},
    {"rand",		0, 1, FEARG_1,	    arg1_list_عدد,
			"expr",
			ret_عدد,	    f_rand},
    {"range",		1, 3, FEARG_1,	    arg3_عدد,
			"expr, max, stride",
			ret_list_عدد,    f_range},
    {"readblob",	1, 3, FEARG_1,	    arg3_string_عدد_عدد,
			"fname, offset, size",
			ret_blob,	    f_readblob},
    {"readdir",		1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_string,    f_readdir},
    {"readdirex",	1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_dict_any,  f_readdirex},
    {"readfile",	1, 3, FEARG_1,	    arg3_string_string_عدد,
			"fname, type, max",
			ret_list_string,    f_readfile},
    {"redraw_listener_add", 1, 1, FEARG_1,  arg1_dict_any,
			"opts",
			ret_عدد,	    f_redraw_listener_add},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_عدد,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_reg_recording},
    {"reltime",		0, 2, FEARG_1,	    arg2_list_عدد,
			"start, end",
			ret_list_عدد,    f_reltime},
    {"reltimefloat",	1, 1, FEARG_1,	    arg1_list_عدد,
			"time",
			ret_float,	    f_reltimefloat},
    {"reltimestr",	1, 1, FEARG_1,	    arg1_list_عدد,
			"time",
			ret_string,	    f_reltimestr},
    {"remote_expr",	2, 4, FEARG_1,	    arg24_remote_عبارت,
			"server, string, idvar, timeout",
			ret_string,	    f_remote_عبارت},
    {"remote_foreground", 1, 1, FEARG_1,    arg1_string,
			"server",
			ret_void,	    f_remote_foreground},
    {"remote_peek",	1, 2, FEARG_1,	    arg2_string,
			"serverid, retvar",
			ret_عدد,	    f_remote_peek},
    {"remote_read",	1, 2, FEARG_1,	    arg2_string_عدد,
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
			ret_عدد_bool,    f_rename},
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
#وگرنه
	    پوچ
#endif
			},
    {"screenattr",	2, 2, FEARG_1,	    arg2_عدد,
			"row, col",
			ret_عدد,	    f_screenattr},
    {"screenchar",	2, 2, FEARG_1,	    arg2_عدد,
			"row, col",
			ret_عدد,	    f_screenchar},
    {"screenchars",	2, 2, FEARG_1,	    arg2_عدد,
			"row, col",
			ret_list_عدد,    f_screenchars},
    {"screencol",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_عدد,
			"winid, lnum, col",
			ret_dict_عدد,    f_screenpos},
    {"screenrow",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_screenrow},
    {"screenstring",	2, 2, FEARG_1,	    arg2_عدد,
			"row, col",
			ret_string,	    f_screenstring},
    {"search",		1, 5, FEARG_1,	    arg15_جستجو,
			"pattern, flags, stopline, timeout, skip",
			ret_عدد,	    f_جستجو},
    {"searchcount",	0, 1, FEARG_1,	    arg1_dict_any,
			"options",
			ret_dict_عدد,    f_searchcount},
    {"searchdecl",	1, 3, FEARG_1,	    arg3_string_bool_bool,
			"name, global, thisblock",
			ret_عدد_bool,    f_searchdecl},
    {"searchpair",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_عدد,	    f_searchpair},
    {"searchpairpos",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_list_عدد,    f_searchpairpos},
    {"searchpos",	1, 5, FEARG_1,	    arg15_جستجو,
			"pattern, flags, stopline, timeout, skip",
			ret_list_عدد,    f_searchpos},
    {"server2client",	2, 2, FEARG_1,	    arg2_string,
			"clientid, string",
			ret_عدد_bool,    f_server2client},
    {"serverlist",	0, 1, 0,	    arg1_dict_any,
			"dict",
			ret_string_or_any_1,	    f_serverlist},
    {"setbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_عدد_bool,    f_setbufline},
    {"setbufvar",	3, 3, FEARG_3,	    arg3_buffer_string_any,
			"buf, varname, val",
			ret_void,	    f_setbufvar},
    {"setcellwidths",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_void,	    f_setcellwidths},
    {"setcharpos",	2, 2, FEARG_2,	    arg2_string_list_عدد,
			"expr, list",
			ret_عدد_bool,    f_setcharpos},
    {"setcharsearch",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_setcharsearch},
    {"setcmdline",	1, 2, FEARG_1,	    arg2_string_عدد,
			"str, pos",
			ret_عدد_bool,    f_setcmdline},
    {"setcmdpos",	1, 1, FEARG_1,	    arg1_عدد,
			"pos",
			ret_عدد_bool,    f_setcmdpos},
    {"setcursorcharpos", 1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_عدد_bool,    f_setcursorcharpos},
    {"setenv",		2, 2, FEARG_2,	    arg2_string_any,
			"name, val",
			ret_void,	    f_setenv},
    {"setfperm",	2, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_عدد_bool,    f_setfperm},
    {"setline",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_عدد_bool,    f_setline},
    {"setloclist",	2, 4, FEARG_2,	    arg24_setloclist,
			"nr, list, action, what",
			ret_عدد_bool,    f_setloclist},
    {"setmatches",	1, 2, FEARG_1,	    arg2_list_any_عدد,
			"list, win",
			ret_عدد_bool,    f_setmatches},
    {"setpos",		2, 2, FEARG_2,	    arg2_string_list_عدد,
			"expr, list",
			ret_عدد_bool,    f_setpos},
    {"setqflist",	1, 3, FEARG_1,	    arg13_setqflist,
			"list, action, what",
			ret_عدد_bool,    f_setqflist},
    {"setreg",		2, 3, FEARG_2,	    arg3_string_any_string,
			"regname, value, options",
			ret_عدد_bool,    f_setreg},
    {"settabvar",	3, 3, FEARG_3,	    arg3_عدد_string_any,
			"tabnr, varname, val",
			ret_void,	    f_settabvar},
    {"settabwinvar",	4, 4, FEARG_4,	    arg4_عدد_عدد_string_any,
			"tabnr, winnr, varname, val",
			ret_void,	    f_settabwinvar},
    {"settagstack",	2, 3, FEARG_2,	    arg23_settagstack,
			"nr, dict, action",
			ret_عدد_bool,    f_settagstack},
    {"setwinvar",	3, 3, FEARG_3,	    arg3_عدد_string_any,
			"winnr, varname, val",
			ret_void,	    f_setwinvar},
    {"sha256",		1, 1, FEARG_1,	    arg1_string_or_blob,
			"expr",
			ret_string,
#ifdef FEAT_CRYPT
	    f_sha256
#وگرنه
	    پوچ
#endif
			},
    {"shellescape",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, special",
			ret_string,	    f_shellescape},
    {"shiftwidth",	0, 1, FEARG_1,	    arg1_عدد,
			"col",
			ret_عدد,	    f_shiftwidth},
    {"sign_define",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"name, dict",
			ret_sign_define,	    SIGN_FUNC(f_sign_define)},
    {"sign_getdefined",	0, 1, FEARG_1,	    arg1_string,
			"name",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getdefined)},
    {"sign_getplaced",	0, 2, FEARG_1,	    arg02_sign_getplaced,
			"buf, dict",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getplaced)},
    {"sign_jump",	3, 3, FEARG_1,	    arg3_عدد_string_buffer,
			"id, group, buf",
			ret_عدد,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_place,
			"id, group, name, buf, dict",
			ret_عدد,	    SIGN_FUNC(f_sign_place)},
    {"sign_placelist",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_عدد,    SIGN_FUNC(f_sign_placelist)},
    {"sign_undefine",	0, 1, FEARG_1,	    arg1_string_or_list_string,
			"name",
			ret_عدد_bool,    SIGN_FUNC(f_sign_undefine)},
    {"sign_unplace",	1, 2, FEARG_1,	    arg2_string_dict,
			"group, dict",
			ret_عدد_bool,    SIGN_FUNC(f_sign_unplace)},
    {"sign_unplacelist", 1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_عدد,    SIGN_FUNC(f_sign_unplacelist)},
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
    {"sound_clear",	0, 0, 0,	    پوچ,
			پوچ,
			ret_void,	    SOUND_FUNC(f_sound_clear)},
    {"sound_playevent",	1, 2, FEARG_1,	    arg2_string_any,
			"name, callback",
			ret_عدد,	    SOUND_FUNC(f_sound_playevent)},
    {"sound_playfile",	1, 2, FEARG_1,	    arg2_string_any,
			"path, callback",
			ret_عدد,	    SOUND_FUNC(f_sound_playfile)},
    {"sound_stop",	1, 1, FEARG_1,	    arg1_عدد,
			"id",
			ret_void,	    SOUND_FUNC(f_sound_stop)},
    {"soundfold",	1, 1, FEARG_1,	    arg1_string,
			"word",
			ret_string,	    f_soundfold},
    {"spellbadword",	0, 1, FEARG_1,	    arg1_string,
			"sentence",
			ret_list_string,    f_spellbadword},
    {"spellsuggest",	1, 3, FEARG_1,	    arg3_string_عدد_bool,
			"word, max, capital",
			ret_list_string,    f_spellsuggest},
    {"split",		1, 3, FEARG_1,	    arg3_string_string_bool,
			"string, pattern, keepempty",
			ret_list_string,    f_split},
    {"sqrt",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sqrt},
    {"srand",		0, 1, FEARG_1,	    arg1_عدد,
			"expr",
			ret_list_عدد,    f_srand},
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
			ret_list_عدد,    f_str2list},
    {"str2nr",		1, 3, FEARG_1,	    arg3_string_عدد_bool,
			"string, base, quoted",
			ret_عدد,	    f_str2nr},
    {"strcharlen",	1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_عدد,	    f_strcharlen},
    {"strcharpart",	2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, skipcc",
			ret_string,	    f_strcharpart},
    {"strchars",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, skipcc",
			ret_عدد,	    f_strchars},
    {"strdisplaywidth",	1, 2, FEARG_1,	    arg2_string_عدد,
			"string, col",
			ret_عدد,	    f_strdisplaywidth},
    {"strftime",	1, 2, FEARG_1,	    arg2_string_عدد,
			"format, time",
			ret_string,
#ifdef HAVE_STRFTIME
	    f_strftime
#وگرنه
	    پوچ
#endif
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_عدد,
			"str, index",
			ret_عدد,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_عدد,
			"haystack, needle, start",
			ret_عدد,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, پوچ,
			"expr",
			ret_string,	    f_string},
    {"strlen",		1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_عدد,	    f_strlen},
    {"strpart",		2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, chars",
			ret_string,	    f_strpart},
    {"strptime",	2, 2, FEARG_1,	    arg2_string,
			"format, timestring",
			ret_عدد,
#ifdef HAVE_STRPTIME
	    f_strptime
#وگرنه
	    پوچ
#endif
			},
    {"strridx",		2, 3, FEARG_1,	    arg3_string_string_عدد,
			"haystack, needle, start",
			ret_عدد,	    f_strridx},
    {"strtrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_strtrans},
    {"strutf16len",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, countcc",
			ret_عدد,	    f_strutf16len},
    {"strwidth",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_عدد,	    f_strwidth},
    {"submatch",	1, 2, FEARG_1,	    arg2_عدد_bool,
			"nr, list",
			ret_string_or_any_2,	    f_submatch},
    {"substitute",	4, 4, FEARG_1,	    arg4_string_string_any_string,
			"string, pat, sub, flags",
			ret_string,	    f_substitute},
    {"swapfilelist",	0, 0, 0,	    پوچ,
			پوچ,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_swapname},
    {"synID",		3, 3, 0,	    arg3_lnum_عدد_bool,
			"lnum, col, trans",
			ret_عدد,	    f_synID},
    {"synIDattr",	2, 3, FEARG_1,	    arg3_عدد_string_string,
			"synID, what, mode",
			ret_string,	    f_synIDattr},
    {"synIDtrans",	1, 1, FEARG_1,	    arg1_عدد,
			"synID",
			ret_عدد,	    f_synIDtrans},
    {"synconcealed",	2, 2, 0,	    arg2_lnum_عدد,
			"lnum, col",
			ret_list_any,	    f_synconcealed},
    {"synstack",	2, 2, 0,	    arg2_lnum_عدد,
			"lnum, col",
			ret_list_عدد,    f_synstack},
    {"system",		1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_string,	    f_system},
    {"systemlist",	1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_list_string,    f_systemlist},
    {"tabpagebuflist",	0, 1, FEARG_1,	    arg1_عدد,
			"arg",
			ret_list_عدد,    f_tabpagebuflist},
    {"tabpagenr",	0, 1, 0,	    arg1_string,
			"arg",
			ret_عدد,	    f_tabpagenr},
    {"tabpagewinnr",	1, 2, FEARG_1,	    arg2_عدد_string,
			"tabarg, arg",
			ret_عدد,	    f_tabpagewinnr},
    {"tabpanel_getinfo", 0, 0, 0,	    پوچ,
			پوچ,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_عدد_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    پوچ,
			پوچ,
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
    {"tempname",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_عدد,	    TERM_FUNC(f_term_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_عدد,	    TERM_FUNC(f_term_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_buffer_string_dict,
			"buf, filename, options",
			ret_void,	    TERM_FUNC(f_term_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_buffer,
			"buf",
			ret_عدد,	    TERM_FUNC(f_term_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_buffer,
			"buf",
			ret_list_string,
#اگر defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_getansicolors
#وگرنه
	    پوچ
#endif
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_عدد_string,
			"attr, what",
			ret_عدد,	    TERM_FUNC(f_term_getattr)},
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
			ret_عدد,	    TERM_FUNC(f_term_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_عدد,    TERM_FUNC(f_term_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, input",
			ret_string,	    TERM_FUNC(f_term_gettty)},
    {"term_list",	0, 0, 0,	    پوچ,
			پوچ,
			ret_list_عدد,    TERM_FUNC(f_term_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_buffer_lnum,
			"buf, row",
			ret_list_dict_any,  TERM_FUNC(f_term_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_buffer_string,
			"buf, keys",
			ret_void,	    TERM_FUNC(f_term_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_buffer_list_any,
			"buf, colors",
			ret_void,
#اگر defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_setansicolors
#وگرنه
	    پوچ
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
    {"term_setsize",	3, 3, FEARG_1,	    arg3_buffer_عدد_عدد,
			"buf, rows, cols",
			ret_void,	    TERM_FUNC(f_term_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_عدد,	    TERM_FUNC(f_term_start)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_buffer_عدد,
			"buf, time",
			ret_void,	    TERM_FUNC(f_term_wait)},
    {"terminalprops",	0, 0, 0,	    پوچ,
			پوچ,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_عدد,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    پوچ,
			پوچ,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    پوچ,
			پوچ,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   پوچ,
			پوچ,
			ret_void,	    f_test_garbagecollect_soon},
    {"test_getvalue",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_عدد,	    f_test_getvalue},
    {"test_gui_event",	2, 2, FEARG_1,	    arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_gui_event},
    {"test_ignore_error", 1, 1, FEARG_1,    arg1_string,
			"expr",
			ret_void,	    f_test_ignore_error},
    {"test_mswin_event", 2, 2, FEARG_1,     arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_mswin_event},
    {"test_null_blob",	0, 0, 0,	    پوچ,
			پوچ,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    پوچ,
			پوچ,
			ret_channel,	    JOB_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    پوچ,
			پوچ,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    پوچ,
			پوچ,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    پوچ,
			پوچ,
			ret_job,	    JOB_FUNC(f_test_null_job)},
    {"test_null_list",	0, 0, 0,	    پوچ,
			پوچ,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    پوچ,
			پوچ,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    پوچ,
			پوچ,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_set},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_عدد,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, پوچ,
			"expr",
			ret_عدد,	    f_test_refcount},
    {"test_setmouse",	2, 2, 0,	    arg2_عدد,
			"row, col",
			ret_void,	    f_test_setmouse},
    {"test_settime",	1, 1, FEARG_1,	    arg1_عدد,
			"expr",
			ret_void,	    f_test_settime},
    {"test_srand_seed",	0, 1, FEARG_1,	    arg1_عدد,
			"seed",
			ret_void,	    f_test_srand_seed},
    {"test_unknown",	0, 0, 0,	    پوچ,
			پوچ,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    پوچ,
			پوچ,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_عدد,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_عدد_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_عدد_any_dict,
			"time, callback, options",
			ret_عدد,	    TIMER_FUNC(f_timer_start)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_عدد,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    پوچ,
			پوچ,
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
    {"trim",		1, 3, FEARG_1,	    arg3_string_string_عدد,
			"text, mask, dir",
			ret_string,	    f_trim},
    {"trunc",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_trunc},
    {"tuple2list",	1, 1, FEARG_1,	    arg1_tuple_any,
			"tuple",
			ret_list_any,	    f_tuple2list},
    {"type",		1, 1, FEARG_1|FE_X, پوچ,
			"expr",
			ret_عدد,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, پوچ,
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
    {"utf16idx",	2, 4, FEARG_1,	    arg4_string_عدد_bool_bool,
			"string, idx, countcc, charidx",
			ret_عدد,	    f_utf16idx},
    {"values",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_member,    f_مقادیر},
    {"virtcol",		1, 3, FEARG_1,	    arg3_string_or_list_bool_عدد,
			"expr, list, winid",
			ret_virtcol,	    f_virtcol},
    {"virtcol2col",	3, 3, FEARG_1,	    arg3_عدد,
			"winid, lnum, col",
			ret_عدد,	    f_virtcol2col},
    {"visualmode",	0, 1, 0,	    arg1_bool,
			"expr",
			ret_string,	    f_visualmode},
    {"wildmenumode",	0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    پوچ,
			پوچ,
			ret_void,	    f_wildtrigger},
    {"win_execute",	2, 3, FEARG_2,	    arg23_win_execute,
			"id, command, silent",
			ret_string,	    f_win_execute},
    {"win_findbuf",	1, 1, FEARG_1,	    arg1_عدد,
			"bufnr",
			ret_list_عدد,    f_win_findbuf},
    {"win_getid",	0, 2, FEARG_1,	    arg2_عدد,
			"win, tab",
			ret_عدد,	    f_win_getid},
    {"win_gettype",	0, 1, FEARG_1,	    arg1_عدد,
			"nr",
			ret_string,	    f_win_gettype},
    {"win_gotoid",	1, 1, FEARG_1,	    arg1_عدد,
			"expr",
			ret_عدد_bool,    f_win_gotoid},
    {"win_id2tabwin",	1, 1, FEARG_1,	    arg1_عدد,
			"expr",
			ret_list_عدد,    f_win_id2tabwin},
    {"win_id2win",	1, 1, FEARG_1,	    arg1_عدد,
			"expr",
			ret_عدد,	    f_win_id2win},
    {"win_move_separator", 2, 2, FEARG_1,   arg2_عدد,
			"nr, offset",
			ret_عدد_bool,    f_win_move_separator},
    {"win_move_statusline", 2, 2, FEARG_1,  arg2_عدد,
			"nr, offset",
			ret_عدد_bool,    f_win_move_statusline},
    {"win_screenpos",	1, 1, FEARG_1,	    arg1_عدد,
			"nr",
			ret_list_عدد,    f_win_screenpos},
    {"win_splitmove",   2, 3, FEARG_1,	    arg3_عدد_عدد_dict,
			"nr, target, options",
			ret_عدد_bool,    f_win_splitmove},
    {"winbufnr",	1, 1, FEARG_1,	    arg1_عدد,
			"nr",
			ret_عدد,	    f_winbufnr},
    {"wincol",		0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_عدد,
			"nr",
			ret_عدد,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_عدد,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    پوچ,
			پوچ,
			ret_عدد,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_عدد,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    پوچ,
			پوچ,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    پوچ,
			پوچ,
			ret_dict_عدد,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_عدد,
			"nr",
			ret_عدد,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    پوچ,
			پوچ,
			ret_dict_عدد,    f_wordcount},
    {"writefile",	2, 3, FEARG_1,	    arg23_writefile,
			"object, fname, flags",
			ret_عدد_bool,    f_writefile},
    {"xor",		2, 2, FEARG_1,	    arg2_عدد,
			"expr1, expr2",
			ret_عدد,	    f_xor},
};

/*
 * Return true if specified function allows a type as an argument.
 */
    ایستا صحیح
func_allows_type(صحیح idx)
{
    بازگشت (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, صحیح idx)
{
    ایستا صحیح	intidx = -1;
    char_u	*name;

    اگر (idx == 0)
	intidx = -1;
    اگر (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	اگر (name != پوچ)
	{
	    اگر (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_pattern, 2) == 0)
		بازگشت cat_prefix_varname('g', name);
	    بازگشت name;
	}
    }
    اگر (++intidx < (صحیح)ARRAY_LENGTH(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	اگر (global_functions[intidx].f_func == پوچ)
	    بازگشت (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	اگر (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	بازگشت IObuff;
    }

    بازگشت پوچ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_عبارت_name(expand_T *xp, صحیح idx)
{
    ایستا صحیح	intidx = -1;
    char_u	*name;

    اگر (idx == 0)
	intidx = -1;
    اگر (intidx < 0)
    {
	name = get_function_name(xp, idx);
	اگر (name != پوچ)
	    بازگشت name;
    }
    بازگشت get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    ایستا صحیح
find_internal_func_opt(char_u *name, صحیح implemented)
{
    صحیح		first = 0;
    صحیح		last;
    صحیح		cmp;
    صحیح		x;

    last = (صحیح)ARRAY_LENGTH(global_functions) - 1;

    // Find the function name in the table. Binary search.
    وقتی (first <= last)
    {
	x = first + ((unsigned)(last - first) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	اگر (cmp < 0)
	    last = x - 1;
	وگرنه اگر (cmp > 0)
	    first = x + 1;
	وگرنه اگر (implemented && global_functions[x].f_func == پوچ)
	    بشکن;
	وگرنه
	    بازگشت x;
    }
    بازگشت -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    صحیح
find_internal_func(char_u *name)
{
    بازگشت find_internal_func_opt(name, TRUE);
}

    صحیح
has_internal_func(char_u *name)
{
    بازگشت find_internal_func_opt(name, TRUE) >= 0;
}

    ایستا صحیح
has_internal_func_name(char_u *name)
{
    بازگشت find_internal_func_opt(name, FALSE) >= 0;
}

    نویسه *
internal_func_name(صحیح idx)
{
    بازگشت global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    صحیح
internal_func_check_arg_types(
	type2_T	*types,
	صحیح	idx,
	صحیح	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    اگر (!(func_allows_type(idx)))
    {
	برای (صحیح i = 0; i < argcount; ++i)
	    اگر (check_type_is_مقدار(types[i].type_curr) == FAIL)
		بازگشت FAIL;
    }

    ثابت argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    اگر (argchecks == پوچ)
	بازگشت OK;

    argcontext_T context;

    context.arg_شمار = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    برای (صحیح i = 0; i < argcount && argchecks[i] != پوچ; ++i)
    {
	context.arg_idx = i;
	اگر (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    تهی
internal_func_get_argcount(صحیح idx, صحیح *argcount, صحیح *min_argcount)
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
	صحیح	    idx,
	صحیح	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = پوچ;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    اگر (*decl_type == پوچ)
	*decl_type = ret;
    current_type_gap = پوچ;
    بازگشت ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    صحیح
internal_func_is_map(صحیح idx)
{
    بازگشت global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    صحیح
check_internal_func(صحیح idx, صحیح argcount)
{
    funcerror_T	    res;
    نویسه	    *name;

    اگر (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    وگرنه اگر (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    وگرنه
	بازگشت global_functions[idx].f_argtype & FEARG_MASK;

    name = internal_func_name(idx);
    اگر (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    وگرنه
	semsg(_(e_not_enough_arguments_for_function_str), name);
    بازگشت -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    ایستا صحیح
check_args_for_type(صحیح idx, صحیح argcount, typval_T *argvars)
{
    اگر (!func_allows_type(idx))
    {
	برای (صحیح i = 0; i < argcount; ++i)
	    اگر (check_typval_is_مقدار(&argvars[i]) == FAIL)
		بازگشت FAIL;
    }
    بازگشت OK;
}

    funcerror_T
call_internal_func(
	char_u	    *name,
	صحیح	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    صحیح i;

    i = find_internal_func(name);
    اگر (i < 0)
	بازگشت FCERR_UNKNOWN;
    اگر (argcount < global_functions[i].f_min_argc)
	بازگشت FCERR_TOOFEW;
    اگر (argcount > global_functions[i].f_max_argc)
	بازگشت FCERR_TOOMANY;
    اگر (check_args_for_type(i, argcount, argvars) == FAIL)
	بازگشت FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    اگر (in_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    بازگشت FCERR_NONE;
}

    تهی
call_internal_func_by_idx(
	صحیح	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    اگر (in_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_internal_method(
	char_u	    *name,
	صحیح	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    صحیح		fi;
    typval_T	argv[MAX_FUNC_ARGS + 1];

    fi = find_internal_func(name);
    اگر (fi < 0)
	بازگشت FCERR_UNKNOWN;
    اگر ((global_functions[fi].f_argtype & FEARG_MASK) == 0)
	بازگشت FCERR_NOTMETHOD;
    اگر (argcount + 1 < global_functions[fi].f_min_argc)
	بازگشت FCERR_TOOFEW;
    اگر (argcount + 1 > global_functions[fi].f_max_argc)
	بازگشت FCERR_TOOMANY;
    اگر (check_args_for_type(fi, argcount, argvars) == FAIL)
	بازگشت FCERR_OTHER;

    اگر ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_2)
    {
	اگر (argcount < 1)
	    بازگشت FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	برای (صحیح i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    وگرنه اگر ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_3)
    {
	اگر (argcount < 2)
	    بازگشت FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	برای (صحیح i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    وگرنه اگر ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_4)
    {
	اگر (argcount < 3)
	    بازگشت FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	برای (صحیح i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    وگرنه
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	برای (صحیح i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    اگر (check_args_for_type(fi, argcount + 1, argv) == FAIL)
	بازگشت FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    بازگشت FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    صحیح
non_zero_arg(typval_T *argvars)
{
    بازگشت ((argvars[0].v_type == VAR_عدد
		&& argvars[0].vval.v_عدد != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_عدد == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != پوچ
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    ایستا تهی
f_and(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    rettv->vval.v_عدد = tv_get_عدد_chk(&argvars[0], پوچ)
					& tv_get_عدد_chk(&argvars[1], پوچ);
}

/*
 * "balloon_show()" function
 */
#ifdef FEAT_BEVAL
    ایستا تهی
f_balloon_gettext(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    اگر (balloonEval == پوچ)
	بازگشت;

    اگر (balloonEval->پیام == پوچ)
	rettv->vval.v_string = پوچ;
    وگرنه
	rettv->vval.v_string = vim_strsave(balloonEval->پیام);
}

    ایستا تهی
f_balloon_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    اگر (balloonEval == پوچ)
	بازگشت;

    اگر (in_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, پوچ,
		l == پوچ || l->lv_len == 0 ? پوچ : l);
    }
    وگرنه
    {
	char_u *mesg;

	اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    بازگشت;

	mesg = tv_get_string_chk(&argvars[0]);
	اگر (mesg != پوچ)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? پوچ : mesg, پوچ);
    }
}

# اگر defined(FEAT_BEVAL_TERM)
    ایستا تهی
f_balloon_split(typval_T *argvars, typval_T *rettv UNUSED)
{
    اگر (rettv_list_alloc(rettv) != OK)
	بازگشت;

    char_u *پیام;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;
    پیام = tv_get_string_chk(&argvars[0]);
    اگر (پیام != پوچ)
    {
	pumitem_T	*array;
	صحیح		size = split_message(پیام, &array);

	// Skip the first and last item, they are always empty.
	برای (صحیح i = 1; i < size - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_text, -1);
	وقتی (size > 0)
	    vim_free(array[--size].pum_text);
	vim_free(array);
    }
}
# endif
#endif

/*
 * "base64_decode(string)" function
 */
    ایستا تهی
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    اگر (check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (rettv_blob_alloc(rettv) == FAIL)
	بازگشت;

    char_u *str = tv_get_string_chk(&argvars[0]);
    اگر (str != پوچ)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    ایستا تهی
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    اگر (check_for_blob_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    blob_T *blob = argvars->vval.v_blob;
    اگر (blob != پوچ)
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
    اگر (buf == پوچ)
	semsg(_(e_invalid_buffer_name_str), tv_get_string(arg));
    بازگشت buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    ایستا تهی
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = VVAL_TRUE;

    اگر (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	بازگشت;

    اگر (strcmp((ثابت نویسه *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    وگرنه
    {
	اگر (bindtextdomain((ثابت نویسه *)argvars[0].vval.v_string, (ثابت نویسه *)argvars[1].vval.v_string) == پوچ)
	{
	    do_outofmem_پیام((long)0);
	    rettv->vval.v_عدد = VVAL_FALSE;
	}
    }

    بازگشت;
}

/*
 * "byte2line(byte)" function
 */
    ایستا تهی
f_byte2line(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_عدد = -1;
#وگرنه
    long	boff = 0;

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    boff = tv_get_عدد(&argvars[0]) - 1;  // boff gets -1 on type error
    اگر (boff < 0)
	rettv->vval.v_عدد = -1;
    وگرنه
	rettv->vval.v_عدد = ml_find_سطر_or_offset(curbuf,
							  (linenr_T)0, &boff);
#endif
}

/*
 * "call(func, arglist [, dict])" function
 */
    ایستا تهی
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = پوچ;
    dict_T	*selfdict = پوچ;
    char_u	*tofree = پوچ;

    اگر (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	بازگشت;

    اگر (check_for_list_arg(argvars, 1) == FAIL)
	بازگشت;
    اگر (argvars[1].vval.v_list == پوچ)
	بازگشت;

    اگر (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    وگرنه اگر (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    وگرنه
	func = tv_get_string(&argvars[0]);
    اگر (func == پوچ || *func == NUL)
	بازگشت;		// type error, empty name or null function

    اگر (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, پوچ, FALSE, TFN_INT|TFN_QUIET);
	اگر (tofree == پوچ)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    بازگشت;
	}
	اگر (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_args(tofree, STRLEN(tofree),
									&p);
	    اگر (s != پوچ)
	    {
		vim_free(tofree);
		tofree = s;
	    }
	}
	func = tofree;
    }

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	اگر (check_for_dict_arg(argvars, 2) == FAIL)
	    برو done;

	selfdict = argvars[2].vval.v_dict;
    }

    (تهی)func_call(func, &argvars[1], partial, selfdict, rettv);

done:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    ایستا تهی
f_changenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_عدد = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    ایستا تهی
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (has_mbyte)
    {
	صحیح	utf8 = 0;

	اگر (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (صحیح)tv_get_bool_chk(&argvars[1], پوچ);

	اگر (utf8)
	    rettv->vval.v_عدد = utf_ptr2char(tv_get_string(&argvars[0]));
	وگرنه
	    rettv->vval.v_عدد = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    وگرنه
	rettv->vval.v_عدد = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    ایستا تهی
get_col(typval_T *argvars, typval_T *rettv, صحیح charcol)
{
    colnr_T	col = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    صحیح		winchanged = FALSE;

    اگر (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_عدد_arg(argvars, 1) == FAIL)
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((صحیح)tv_get_عدد(&argvars[1]), &tp);
	اگر (wp == پوچ || tp == پوچ)
	    بازگشت;

	اگر (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    بازگشت;

	check_cursor();
	winchanged = TRUE;
    }

    صحیح fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    اگر (fp != پوچ && fnum == curbuf->b_fnum)
    {
	اگر (fp->col == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    اگر (fp->lnum <= curbuf->b_ml.ml_سطر_شمار)
		col = ml_get_len(fp->lnum) + 1;
	    وگرنه
		col = MAXCOL;
	}
	وگرنه
	{
	    col = fp->col + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    اگر (virtual_active() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		اگر (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    صحیح		l;

		    اگر (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			col += l;
		}
	    }
	}
    }
    rettv->vval.v_عدد = col;

    اگر (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    ایستا تهی
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, TRUE);
}

    win_T *
get_optional_window(typval_T *argvars, صحیح idx)
{
    win_T   *win = curwin;

    اگر (argvars[idx].v_type == VAR_UNKNOWN)
	بازگشت curwin;

    win = find_win_by_nr_or_id(&argvars[idx]);
    اگر (win == پوچ)
    {
	emsg(_(e_invalid_window_عدد));
	بازگشت پوچ;
    }
    بازگشت win;
}

/*
 * "col(string)" function
 */
    ایستا تهی
f_col(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    ایستا تهی
f_confirm(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*message;
    char_u	*buttons = پوچ;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    صحیح		def = 1;
    صحیح		type = VIM_GENERIC;
    char_u	*typestr;
    صحیح		error = FALSE;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_عدد_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	بازگشت;

    message = tv_get_string_chk(&argvars[0]);
    اگر (message == پوچ)
	error = TRUE;
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	اگر (buttons == پوچ)
	    error = TRUE;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (صحیح)tv_get_عدد_chk(&argvars[2], &error);
	    اگر (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		اگر (typestr == پوچ)
		    error = TRUE;
		وگرنه
		{
		    انتخاب (TOUPPER_ASC(*typestr))
		    {
			حالت 'E': type = VIM_ERROR; بشکن;
			حالت 'Q': type = VIM_QUESTION; بشکن;
			حالت 'I': type = VIM_INFO; بشکن;
			حالت 'W': type = VIM_WARNING; بشکن;
			حالت 'G': type = VIM_GENERIC; بشکن;
		    }
		}
	    }
	}
    }

    اگر (buttons == پوچ || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    اگر (!error)
	rettv->vval.v_عدد = do_dialog(type, پوچ, message, buttons,
							    def, پوچ, FALSE);
#endif
}

/*
 * "copy()" function
 */
    ایستا تهی
f_copy(typval_T *argvars, typval_T *rettv)
{
    item_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ایستا تهی
set_cursorpos(typval_T *argvars, typval_T *rettv, صحیح charcol)
{
    long	lnum, col;
    long	coladd = 0;
    صحیح		set_curswant = TRUE;

    اگر (in_vim9script()
	    && (check_for_string_or_عدد_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 2) == FAIL)))
	بازگشت;

    rettv->vval.v_عدد = -1;
    اگر (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	اگر (list2fpos(argvars, &pos, پوچ, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_argument));
	    بازگشت;
	}
	lnum = pos.lnum;
	col = pos.col;
	coladd = pos.coladd;
	اگر (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    set_curswant = FALSE;
	}
    }
    وگرنه اگر ((argvars[0].v_type == VAR_عدد
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_عدد
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	اگر (lnum < 0)
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
	وگرنه اگر (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	col = (long)tv_get_عدد_chk(&argvars[1], پوچ);
	اگر (charcol)
	    col = buf_charidx_to_byteidx(curbuf, lnum, col) + 1;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (long)tv_get_عدد_chk(&argvars[2], پوچ);
    }
    وگرنه
    {
	emsg(_(e_invalid_argument));
	بازگشت;
    }
    اگر (lnum < 0 || col < 0 || coladd < 0)
	بازگشت;		// type error; errmsg already given
    اگر (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    اگر (col != MAXCOL && --col < 0)
	col = 0;
    curwin->w_cursor.col = col;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    اگر (has_mbyte)
	mb_adjust_cursor();

    curwin->w_set_curswant = set_curswant;
    rettv->vval.v_عدد = 0;
}

/*
 * "cursor(lnum, col)" function, or
 * "cursor(list)"
 *
 * Moves the cursor to the specified line and column.
 * Returns 0 when the position could be set, -1 otherwise.
 */
    ایستا تهی
f_cursor(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, FALSE);
}

#ifdef MSWIN
/*
 * "debugbreak()" function
 */
    ایستا تهی
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    صحیح		pid;

    rettv->vval.v_عدد = FAIL;
    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    pid = (صحیح)tv_get_عدد(&argvars[0]);
    اگر (pid == 0)
    {
	emsg(_(e_invalid_argument));
	بازگشت;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    اگر (hProcess == پوچ)
	بازگشت;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_عدد = OK;
}
#endif

/*
 * "deepcopy()" function
 */
    ایستا تهی
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    اگر (check_for_opt_bool_arg(argvars, 1) == FAIL)
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], پوچ);

    item_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    ایستا تهی
f_did_filetype(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->vval.v_عدد = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    ایستا تهی
f_echoraw(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *str;

    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    str = tv_get_string_chk(&argvars[0]);
    اگر (str != پوچ && *str != NUL)
    {
	out_str(str);
	out_flush();
    }
}

/*
 * "empty({expr})" function
 */
    ایستا تهی
f_empty(typval_T *argvars, typval_T *rettv)
{
    صحیح		n = FALSE;

    انتخاب (argvars[0].v_type)
    {
	حالت VAR_STRING:
	حالت VAR_FUNC:
	    n = argvars[0].vval.v_string == پوچ
					  || *argvars[0].vval.v_string == NUL;
	    بشکن;
	حالت VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == پوچ;
	    بشکن;
	حالت VAR_عدد:
	    n = argvars[0].vval.v_عدد == 0;
	    بشکن;
	حالت VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    بشکن;
	حالت VAR_LIST:
	    n = argvars[0].vval.v_list == پوچ
					|| argvars[0].vval.v_list->lv_len == 0;
	    بشکن;
	حالت VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    بشکن;
	حالت VAR_DICT:
	    n = argvars[0].vval.v_dict == پوچ
			|| argvars[0].vval.v_dict->dv_hashtab.ht_used == 0;
	    بشکن;
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    n = argvars[0].vval.v_عدد != VVAL_TRUE;
	    بشکن;
	حالت VAR_CLASS:
	    n = argvars[0].vval.v_class != پوچ;
	    بشکن;
	حالت VAR_OBJECT:
	    n = object_empty(argvars[0].vval.v_object);
	    بشکن;

	حالت VAR_BLOB:
	    n = argvars[0].vval.v_blob == پوچ
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    بشکن;

	حالت VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_job == پوچ
			   || argvars[0].vval.v_job->jv_status != JOB_STARTED;
	    بشکن;
#endif
	حالت VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_channel == پوچ
			       || !channel_is_open(argvars[0].vval.v_channel);
	    بشکن;
#endif
	حالت VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == پوچ
		|| argvars[0].vval.v_typealias->ta_name == پوچ
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    بشکن;

	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_INSTR:
	    internal_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    بشکن;
    }

    rettv->vval.v_عدد = n;
}

/*
 * "environ()" function
 */
    ایستا تهی
f_environ(typval_T *argvars UNUSED, typval_T *rettv)
{
#اگر !defined(AMIGA)
    صحیح			i = 0;
    char_u		*entry, *مقدار;
# اگر defined (MSWIN)
#  اگر !defined(_UCRT)
    بیرونی wchar_t	**_wenviron;
#  endif
# وگرنه
    بیرونی نویسه		**environ;
# endif

    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;

# ifdef MSWIN
    اگر (*_wenviron == پوچ)
	بازگشت;
# وگرنه
    اگر (*environ == پوچ)
	بازگشت;
# endif

    برای (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	اگر ((p = (short_u *)_wenviron[i]) == پوچ)
	    بازگشت;
	entry = utf16_to_enc(p, پوچ);
# وگرنه
	اگر ((entry = (char_u *)environ[i]) == پوچ)
	    بازگشت;
	entry = vim_strsave(entry);
# endif
	اگر (entry == پوچ) // out of memory
	    بازگشت;
	اگر ((مقدار = vim_strchr(entry, '=')) == پوچ)
	{
	    vim_free(entry);
	    ادامه;
	}
	*مقدار++ = NUL;
	dict_add_string(rettv->vval.v_dict, (نویسه *)entry, مقدار);
	vim_free(entry);
    }
#endif
}

/*
 * "err_teapot()" function
 */
    ایستا تهی
f_err_teapot(typval_T *argvars, typval_T *rettv UNUSED)
{
    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	اگر (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    اگر (*skipwhite(s) == NUL)
		بازگشت;
	}

	صحیح err = FALSE;
	صحیح do_503 = eval_عبارت_to_bool(&argvars[0], &err);
	اگر (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_available));
	    بازگشت;
	}
    }

    emsg(_(e_im_a_teapot));
}

/*
 * "escape({string}, {chars})" function
 */
    ایستا تهی
f_escape(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    rettv->vval.v_string = vim_strsave_escaped(tv_get_string(&argvars[0]),
					 tv_get_string_buf(&argvars[1], buf));
    rettv->v_type = VAR_STRING;
}

/*
 * "eval()" function
 */
    ایستا تهی
f_eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*s, *p;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    s = tv_get_string_chk(&argvars[0]);
    اگر (s != پوچ)
	s = skipwhite(s);

    p = s;
    اگر (s == پوچ || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	اگر (p != پوچ && !aborting())
	    semsg(_(e_invalid_expression_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_عدد;
	rettv->vval.v_عدد = 0;
    }
    وگرنه اگر (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    ایستا تهی
f_eventhandler(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_عدد = vgetc_busy || درون_داد_busy;
}

ایستا garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    تهی
execute_redir_str(char_u *مقدار, صحیح مقدار_len)
{
    صحیح		len;

    اگر (مقدار_len == -1)
	len = (صحیح)STRLEN(مقدار);	// Append the entire string
    وگرنه
	len = مقدار_len;		// Append only "value_len" characters
    اگر (ga_grow(&redir_execute_ga, len) == FAIL)
	بازگشت;

    mch_memmove((نویسه *)redir_execute_ga.ga_data
	    + redir_execute_ga.ga_len, مقدار, len);
    redir_execute_ga.ga_len += len;
}

#اگر defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    ایستا char_u *
get_str_سطر(
    صحیح	    c UNUSED,
    تهی    *cookie,
    صحیح	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    char_u	*start = *(char_u **)cookie;
    char_u	*سطر;
    char_u	*p;

    p = start;
    اگر (p == پوچ || *p == NUL)
	بازگشت پوچ;
    p = vim_strchr(p, '\n');
    اگر (p == پوچ)
	سطر = vim_strsave(start);
    وگرنه
    {
	سطر = vim_strnsave(start, p - start);
	p++;
    }

    *(char_u **)cookie = p;
    بازگشت سطر;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    تهی
execute_cmds_from_string(char_u *str)
{
    do_cmdline(پوچ, get_str_سطر, (تهی *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#endif

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_سطر(
    صحیح	    c UNUSED,
    تهی    *cookie,
    صحیح	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *item = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    اگر (item == پوچ)
	بازگشت پوچ;
    s = tv_get_string_buf_chk(&item->li_tv, buf);
    *p = item->li_next;
    بازگشت s == پوچ ? پوچ : vim_strsave(s);
}

/*
 * "execute()" function
 */
    تهی
execute_common(typval_T *argvars, typval_T *rettv, صحیح arg_off)
{
    char_u	*cmd = پوچ;
    list_T	*list = پوچ;
    صحیح		save_پیام_silent = پیام_silent;
    صحیح		save_emsg_silent = emsg_silent;
    صحیح		save_emsg_noredir = emsg_noredir;
    صحیح		save_redir_execute = redir_execute;
    صحیح		save_redir_off = redir_off;
    garray_T	save_ga;
    صحیح		save_پیام_col = پیام_col;
    صحیح		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    صحیح		echo_output = FALSE;

    rettv->vval.v_string = پوچ;
    rettv->v_type = VAR_STRING;

    اگر (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	اگر (list == پوچ || list->lv_len == 0)
	    // empty list, no commands, empty output
	    بازگشت;
	++list->lv_refcount;
    }
    وگرنه اگر (argvars[arg_off].v_type == VAR_JOB
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_مقدار_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	بازگشت;
    }
    وگرنه
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	اگر (cmd == پوچ)
	    بازگشت;
    }

    اگر (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      in_vim9script());

	اگر (s == پوچ)
	    بازگشت;
	اگر (*s == NUL)
	    echo_output = TRUE;
	اگر (STRNCMP(s, "silent", 6) == 0)
	    ++پیام_silent;
	اگر (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    وگرنه
	++پیام_silent;

    اگر (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, اندازه(نویسه), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    اگر (!echo_output)
	پیام_col = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    اگر (cmd != پوچ)
	do_cmdline_cmd(cmd);
    وگرنه
    {
	listitem_T	*item;

	CHECK_LIST_MATERIALIZE(list);
	item = list->lv_first;
	do_cmdline(پوچ, get_list_سطر, (تهی *)&item,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    اگر (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((نویسه *)redir_execute_ga.ga_data)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_data;
    }
    وگرنه
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = پوچ;
    }
    پیام_silent = save_پیام_silent;
    emsg_silent = save_emsg_silent;
    emsg_noredir = save_emsg_noredir;

    redir_execute = save_redir_execute;
    اگر (redir_execute)
	redir_execute_ga = save_ga;
    redir_off = save_redir_off;

    // "silent reg" or "silent echo x" leaves msg_col somewhere in the line.
    اگر (echo_output)
	// When not working silently: put it in column zero.  A following
	// "echon" will overwrite the message, unavoidably.
	پیام_col = 0;
    وگرنه
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	پیام_col = save_پیام_col;
}

/*
 * "execute()" function
 */
    ایستا تهی
f_execute(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	بازگشت;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    تهی
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحیح		n = FALSE;

    اگر (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	بازگشت;

    p = tv_get_string(&argvars[0]);
    اگر (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	اگر (mch_getenv(p + 1) != پوچ)
	    n = TRUE;
	وگرنه
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    اگر (p != پوچ && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    وگرنه اگر (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, پوچ, TRUE) == OK);
	اگر (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    وگرنه اگر (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    وگرنه اگر (*p == '?')			// internal function only
    {
	n = has_internal_func_name(p + 1);
    }
    وگرنه اگر (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    وگرنه اگر (*p == '#')
    {
	اگر (p[1] == '#')
	    n = autocmd_supported(p + 2);
	وگرنه
	    n = au_exists(p + 1);
    }
    وگرنه				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_عدد = n;
}

    ایستا تهی
f_exists_compiled(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    emsg(_(e_exists_compiled_can_only_be_used_in_def_function));
}

/*
 * "expand()" function
 */
    ایستا تهی
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    size_t	len;
    صحیح		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    صحیح		error = FALSE;
    char_u	*result;
#ifdef BACKSLASH_IN_FILENAME
    char_u	*p_csl_save = p_csl;
#endif

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	بازگشت;

#ifdef BACKSLASH_IN_FILENAME
    // avoid using 'completeslash' here
    p_csl = empty_option;
#endif

    rettv->v_type = VAR_STRING;
    اگر (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_set(rettv, پوچ);

    s = tv_get_string(&argvars[0]);
    اگر (*s == '%' || *s == '#' || *s == '<')
    {
	نویسه	*errormsg = پوچ;

	اگر (p_verbose == 0)
	    ++emsg_off;
	result = eval_vars(s, s, &len, پوچ, &errormsg, پوچ, FALSE);
	اگر (p_verbose == 0)
	    --emsg_off;
	وگرنه اگر (errormsg != پوچ)
	    emsg(errormsg);
	اگر (rettv->v_type == VAR_LIST)
	{
	    اگر (rettv_list_alloc(rettv) == OK && result != پوچ)
		list_append_string(rettv->vval.v_list, result, -1);
	    vim_free(result);
	}
	وگرنه
	    rettv->vval.v_string = result;
    }
    وگرنه
    {
	// When the optional second argument is non-zero, don't remove matches
	// for 'wildignore' and don't put matches for 'suffixes' at the end.
	اگر (argvars[1].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	اگر (!error)
	{
	    ExpandInit(&xpc);
	    xpc.xp_context = EXPAND_FILES;
	    اگر (p_wic)
		options += WILD_ICASE;
	    اگر (rettv->v_type == VAR_STRING)
		rettv->vval.v_string = ExpandOne(&xpc, s, پوچ,
							   options, WILD_ALL);
	    وگرنه اگر (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, پوچ, options, WILD_ALL_KEEP);
		برای (صحیح i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	وگرنه
	    rettv->vval.v_string = پوچ;
    }
#ifdef BACKSLASH_IN_FILENAME
    p_csl = p_csl_save;
#endif
}

/*
 * "expandcmd()" function
 * Expand all the special characters in a command string.
 */
    ایستا تهی
f_expandcmd(typval_T *argvars, typval_T *rettv)
{
    exarg_T	eap;
    char_u	*cmdstr;
    نویسه	*errormsg = پوچ;
    صحیح		emsgoff = TRUE;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_FIELD(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = پوچ;
    eap.cmdidx = CMD_USER;

    اگر (emsgoff)
	++emsg_off;
    اگر (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	اگر (!emsgoff && errormsg != پوچ && *errormsg != NUL)
	    emsg(errormsg);
    اگر (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    ایستا تهی
f_feedkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    صحیح		remap = TRUE;
    صحیح		درج = FALSE;
    char_u	*کلیدها, *flags;
    char_u	nbuf[NUMBUFLEN];
    صحیح		typed = FALSE;
    صحیح		execute = FALSE;
    صحیح		context = FALSE;
    صحیح		dangerous = FALSE;
    صحیح		lowlevel = FALSE;

    // This is not allowed in the sandbox.  If the commands would still be
    // executed in the sandbox it would be OK, but it probably happens later,
    // when "sandbox" is no longer set.
    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	بازگشت;

    کلیدها = tv_get_string(&argvars[0]);

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	برای ( ; *flags != NUL; ++flags)
	{
	    انتخاب (*flags)
	    {
		حالت 'n': remap = FALSE; بشکن;
		حالت 'm': remap = TRUE; بشکن;
		حالت 't': typed = TRUE; بشکن;
		حالت 'i': درج = TRUE; بشکن;
		حالت 'x': execute = TRUE; بشکن;
		حالت 'c': context = TRUE; بشکن;
		حالت '!': dangerous = TRUE; بشکن;
		حالت 'L': lowlevel = TRUE; بشکن;
	    }
	}
    }

    اگر (*کلیدها != NUL || execute)
    {
	اگر (lowlevel
#ifdef FEAT_VTP
		&& (!is_term_win32()
		    || (کلیدها[0] == 3 && ctrl_c_interrupts && typed))
#endif
	   )
	{
#ifdef USE_درون_داد_BUF
	    ch_log(پوچ, "feedkeys() lowlevel: %s", کلیدها);

	    صحیح len = (صحیح)STRLEN(کلیدها);
	    برای (صحیح idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		اگر (کلیدها[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		add_to_درون_داد_buf(کلیدها + idx, 1);
	    }
#وگرنه
	    emsg(_(e_lowlevel_درون_داد_not_supported));
#endif
	}
	وگرنه
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *کلیدها_esc = vim_strsave_escape_csi(کلیدها);
	    اگر (کلیدها_esc == پوچ)
		بازگشت;

	    ch_log(پوچ, "feedkeys(%s): %s", typed ? "typed" : "", کلیدها);

	    ins_typebuf(کلیدها_esc, (remap ? REMAP_YES : REMAP_NONE),
				   درج ? 0 : typebuf.tb_len, !typed, FALSE);
	    اگر (vgetc_busy
#ifdef FEAT_TIMERS
		    || timer_busy
#endif
		    || درون_داد_busy)
		typebuf_was_filled = TRUE;

	    vim_free(کلیدها_esc);
	}

	اگر (execute)
	{
	    صحیح		save_پیام_scroll = پیام_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    پیام_scroll = FALSE;

	    ch_log(پوچ, "feedkeys() executing");

	    اگر (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_version = 0;
	    }

	    اگر (!dangerous)
	    {
		++ex_normal_busy;
		++in_feedkeys;
	    }
	    ++allow_osc_کلید;
	    exec_normal(TRUE, lowlevel, TRUE);
	    --allow_osc_کلید;
	    اگر (!dangerous)
	    {
		--ex_normal_busy;
		--in_feedkeys;
	    }

	    پیام_scroll |= save_پیام_scroll;

	    اگر (context)
		current_sctx = save_sctx;
	}
    }
}

/*
 * "fnameescape({string})" function
 */
    ایستا تهی
f_fnameescape(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    ایستا تهی
f_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    اگر (gui.in_use)
    {
	gui_mch_set_foreground();
	بازگشت;
    }
#endif
#اگر defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_set_foreground();
#endif
}

/*
 * "function()" function
 * "funcref()" function
 */
    ایستا تهی
common_function(typval_T *argvars, typval_T *rettv, صحیح is_funcref)
{
    char_u	*s;
    char_u	*name;
    صحیح		use_string = FALSE;
    partial_T   *arg_pt = پوچ;
    char_u	*trans_name = پوچ;
    صحیح		is_global = FALSE;
    char_u	*start_bracket = پوچ;

    اگر (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	بازگشت;

    اگر (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    وگرنه اگر (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != پوچ)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    وگرنه
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    اگر (s == پوچ)
    {
	semsg(_(e_invalid_argument_str), "NULL");
	بازگشت;
    }

    اگر ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == پوچ) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, پوچ);
	اگر (*name == '<')
	{
	    // generic function
	    start_bracket = name;
	    اگر (skip_generic_func_type_args(&name) == FAIL)
		برو theend;
	}
	اگر (*name != NUL)
	    s = پوچ;
    }

    اگر (s == پوچ || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == پوچ))
	semsg(_(e_invalid_argument_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    وگرنه اگر (trans_name != پوچ && (is_funcref
			 ? find_func(trans_name, is_global) == پوچ
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    وگرنه
    {
	صحیح	dict_idx = 0;
	صحیح	arg_idx = 0;
	list_T	*list = پوچ;

	اگر (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	وگرنه اگر (trans_name != پوچ && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	وگرنه
	    name = vim_strsave(s);

	اگر (argvars[1].v_type != VAR_UNKNOWN)
	{
	    اگر (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    وگرنه اگر (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    وگرنه
		// function(name, [args])
		arg_idx = 1;
	    اگر (dict_idx > 0)
	    {
		اگر (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    برو theend;
		}
		اگر (argvars[dict_idx].vval.v_dict == پوچ)
		    dict_idx = 0;
	    }
	    اگر (arg_idx > 0)
	    {
		اگر (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_second_argument_of_function_must_be_list_or_dict));
		    vim_free(name);
		    برو theend;
		}
		list = argvars[arg_idx].vval.v_list;
		اگر (list == پوچ || list->lv_len == 0)
		    arg_idx = 0;
		وگرنه اگر (list->lv_len > MAX_FUNC_ARGS)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    برو theend;
		}
	    }
	}
	اگر (dict_idx > 0 || arg_idx > 0 || arg_pt != پوچ || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    اگر (pt == پوچ)
		vim_free(name);
	    وگرنه
	    {
		اگر (arg_idx > 0 || (arg_pt != پوچ && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    صحیح		i = 0;
		    صحیح		arg_len = 0;
		    صحیح		lv_len = 0;

		    اگر (arg_pt != پوچ)
			arg_len = arg_pt->pt_argc;
		    اگر (list != پوچ)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    اگر (pt->pt_argv == پوچ)
		    {
			vim_free(pt);
			vim_free(name);
			برو theend;
		    }
		    برای (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    اگر (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_اقلام(list, li)
			    copy_tv(&li->li_tv, &pt->pt_argv[i++]);
		    }
		}

		// For "function(dict.func, [], dict)" and "func" is a partial
		// use "dict".  That is backwards compatible.
		اگر (dict_idx > 0)
		{
		    // The dict is bound explicitly, pt_auto is FALSE.
		    pt->pt_dict = argvars[dict_idx].vval.v_dict;
		    ++pt->pt_dict->dv_refcount;
		}
		وگرنه اگر (arg_pt != پوچ)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    اگر (pt->pt_dict != پوچ)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    اگر (pt->pt_obj != پوچ)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		اگر (arg_pt != پوچ && arg_pt->pt_func != پوچ)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		وگرنه اگر (is_funcref)
		{
		    pt->pt_func = find_func(trans_name, is_global);
		    اگر (IS_GENERIC_FUNC(pt->pt_func) && start_bracket != پوچ)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&start_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		وگرنه
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		اگر (arg_pt != پوچ)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	وگرنه
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    اگر (start_bracket == پوچ)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    وگرنه
	    {
		// generic function
		size_t len = STRLEN(name) + STRLEN(start_bracket);
		rettv->vval.v_string = alloc(len + 1);
		اگر (rettv->vval.v_string != پوچ)
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
    ایستا تهی
f_funcref(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, TRUE);
}

/*
 * "function()" function
 */
    ایستا تهی
f_function(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, FALSE);
}

/*
 * "garbagecollect()" function
 */
    ایستا تهی
f_garbagecollect(typval_T *argvars, typval_T *rettv UNUSED)
{
    اگر (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	بازگشت;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    اگر (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
	garbage_collect_at_exit = TRUE;
}

/*
 * "get()" function
 */
    ایستا تهی
f_get(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    list_T	*l;
    tuple_T	*tuple;
    dictitem_T	*di;
    dict_T	*d;
    typval_T	*tv = پوچ;
    صحیح		what_is_dict = FALSE;

    اگر (argvars[0].v_type == VAR_BLOB)
    {
	صحیح error = FALSE;
	صحیح idx = tv_get_عدد_chk(&argvars[1], &error);

	اگر (!error)
	{
	    rettv->v_type = VAR_عدد;
	    اگر (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    اگر (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_عدد = -1;
	    وگرنه
	    {
		rettv->vval.v_عدد = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    وگرنه اگر (argvars[0].v_type == VAR_LIST)
    {
	اگر ((l = argvars[0].vval.v_list) != پوچ)
	{
	    صحیح		error = FALSE;

	    li = list_find(l, (long)tv_get_عدد_chk(&argvars[1], &error));
	    اگر (!error && li != پوچ)
		tv = &li->li_tv;
	}
    }
    وگرنه اگر (argvars[0].v_type == VAR_TUPLE)
    {
	اگر ((tuple = argvars[0].vval.v_tuple) != پوچ)
	{
	    صحیح		error = FALSE;
	    long	idx;

	    idx = (long)tv_get_عدد_chk(&argvars[1], &error);
	    اگر (!error)
		tv = tuple_find(tuple, idx);
	}
    }
    وگرنه اگر (argvars[0].v_type == VAR_DICT)
    {
	اگر ((d = argvars[0].vval.v_dict) != پوچ)
	{
	    di = dict_find(d, tv_get_string(&argvars[1]), -1);
	    اگر (di != پوچ)
		tv = &di->di_tv;
	}
    }
    وگرنه اگر (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	اگر (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	وگرنه
	{
	    CLEAR_FIELD(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	اگر (pt != پوچ)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    اگر (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		اگر (name == پوچ)
		    rettv->vval.v_string = پوچ;
		وگرنه
		{
		    اگر (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    اگر (*what == 'n' && pt->pt_name == پوچ
							&& pt->pt_func != پوچ)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    وگرنه اگر (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		اگر (pt->pt_dict != پوچ)
		    rettv_dict_set(rettv, pt->pt_dict);
	    }
	    وگرنه اگر (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		اگر (rettv_list_alloc(rettv) == OK)
		{
		    صحیح i;

		    برای (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    وگرنه اگر (STRCMP(what, "arity") == 0)
	    {
		صحیح required = 0, optional = 0, varargs = FALSE;
		char_u *name = partial_name(pt);

		get_func_arity(name, &required, &optional, &varargs);

		rettv->v_type = VAR_DICT;
		اگر (rettv_dict_alloc(rettv) == OK)
		{
		    dict_T *dict = rettv->vval.v_dict;

		    // Take into account the arguments of the partial, if any.
		    // Note that it is possible to supply more arguments than the function
		    // accepts.
		    اگر (pt->pt_argc >= required + optional)
			required = optional = 0;
		    وگرنه اگر (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    وگرنه
			required -= pt->pt_argc;

		    dict_add_عدد(dict, "required", required);
		    dict_add_عدد(dict, "optional", optional);
		    dict_add_bool(dict, "varargs", varargs);
		}
	    }
	    وگرنه
		semsg(_(e_invalid_argument_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    اگر (!what_is_dict)
		بازگشت;
	}
    }
    وگرنه
	semsg(_(e_argument_of_str_must_be_list_tuple_dictionary_or_blob), "get()");

    اگر (tv == پوچ)
    {
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    وگرنه
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
    ایستا تهی
f_getbgcolor(typval_T *argvars UNUSED, typval_T *rettv)
{
    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	اگر (gui.in_use)
	    try_hl = true;
# endif
# ifdef FEAT_TERMGUICOLORS
	اگر (p_tgc)
	    try_hl = true;
# endif
	اگر (try_hl)
	{
	    guicolor_T	fg, bg;
	    صحیح		hl_id = syn_name2id((char_u *)"Normal");

	    اگر (hl_id > 0)
	    {
		(تهی)syn_id2colors(hl_id, &fg, &bg);
		اگر (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_عدد(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_عدد(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_عدد(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    بازگشت;
		}
	    }
	}
    }
#endif
#ifdef FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	اگر (term_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_عدد(rettv->vval.v_list, (varnumber_T)r);
	    list_append_عدد(rettv->vval.v_list, (varnumber_T)g);
	    list_append_عدد(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#endif
}

/*
 * "getcellpixels()" function
 */
    ایستا تهی
f_getcellpixels(typval_T *argvars UNUSED, typval_T *rettv)
{
    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

#اگر defined(FEAT_GUI)
    اگر (gui.in_use)
    {
	// success pixel size and no gui.
	list_append_عدد(rettv->vval.v_list, (varnumber_T)gui.char_width);
	list_append_عدد(rettv->vval.v_list, (varnumber_T)gui.char_height);
    }
    وگرنه
#endif
    {
#اگر defined(UNIX) || defined(MSWIN)
	list_append_عدد(rettv->vval.v_list, (varnumber_T)cell_width);
	list_append_عدد(rettv->vval.v_list, (varnumber_T)cell_height);
#وگرنه
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_عدد(rettv->vval.v_list, (varnumber_T)-1);
	list_append_عدد(rettv->vval.v_list, (varnumber_T)-1);
#endif
    }

}

/*
 * "getchangelist()" function
 */
    ایستا تهی
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    صحیح		i;
    list_T	*l;
    dict_T	*d;
    صحیح		changelistindex;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    وگرنه
	buf = tv_get_buf_from_arg(&argvars[0]);
    اگر (buf == پوچ)
	بازگشت;

    l = list_alloc();
    اگر (l == پوچ)
	بازگشت;
    اگر (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	بازگشت;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    اگر (buf == curwin->w_buffer)
    {
	changelistindex = curwin->w_changelistidx;
    }
    وگرنه
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    اگر (wip->wi_win == curwin)
		بشکن;
	changelistindex = wip != پوچ ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_عدد(rettv->vval.v_list, (varnumber_T)changelistindex);

    برای (i = 0; i < buf->b_changelistlen; ++i)
    {
	اگر (buf->b_changelist[i].lnum == 0)
	    ادامه;
	اگر ((d = dict_alloc()) == پوچ)
	    بازگشت;
	اگر (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    بازگشت;
	}
	dict_add_عدد(d, "lnum", (long)buf->b_changelist[i].lnum);
	dict_add_عدد(d, "col", (long)buf->b_changelist[i].col);
	dict_add_عدد(d, "coladd", (long)buf->b_changelist[i].coladd);
    }
}

    ایستا تهی
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    صحیح		getcurpos,
    صحیح		charcol)
{
    pos_T	*fp = پوچ;
    pos_T	pos;
    win_T	*wp = curwin;
    list_T	*l;
    صحیح		fnum = -1;

    اگر (rettv_list_alloc(rettv) == OK)
    {
	l = rettv->vval.v_list;
	اگر (getcurpos)
	{
	    اگر (argvars[0].v_type != VAR_UNKNOWN)
	    {
		wp = find_win_by_nr_or_id(&argvars[0]);
		اگر (wp != پوچ)
		    fp = &wp->w_cursor;
	    }
	    وگرنه
		fp = &curwin->w_cursor;
	    اگر (fp != پوچ && charcol)
	    {
		pos = *fp;
		pos.col =
		    buf_byteidx_to_charidx(wp->w_buffer, pos.lnum, pos.col);
		fp = &pos;
	    }
	}
	وگرنه
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	اگر (fnum != -1)
	    list_append_عدد(l, (varnumber_T)fnum);
	وگرنه
	    list_append_عدد(l, (varnumber_T)0);
	list_append_عدد(l, (fp != پوچ) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_عدد(l, (fp != پوچ)
		     ? (varnumber_T)(fp->col == MAXCOL ? MAXCOL : fp->col + 1)
							    : (varnumber_T)0);
	list_append_عدد(l, (fp != پوچ) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	اگر (getcurpos)
	{
	    صحیح	    save_set_curswant = curwin->w_set_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    اگر (wp == curwin)
		update_curswant();
	    list_append_عدد(l, wp == پوچ ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    اگر (wp == curwin && save_set_curswant)
	    {
		curwin->w_set_curswant = save_set_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    وگرنه
	rettv->vval.v_عدد = FALSE;
}

/*
 * "getcharpos()" function
 */
    ایستا تهی
f_getcharpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    ایستا تهی
f_getcharsearch(typval_T *argvars UNUSED, typval_T *rettv)
{
    اگر (rettv_dict_alloc(rettv) != OK)
	بازگشت;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_string(dict, "char", last_csearch());
    dict_add_عدد(dict, "forward", last_csearch_forward());
    dict_add_عدد(dict, "until", last_csearch_until());
}

/*
 * "getenv()" function
 */
    ایستا تهی
f_getenv(typval_T *argvars, typval_T *rettv)
{
    صحیح	    mustfree = FALSE;
    char_u  *p;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    اگر (p == پوچ)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_عدد = VVAL_NULL;
	بازگشت;
    }
    اگر (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    ایستا تهی
f_getfontname(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	بازگشت;

#ifdef FEAT_GUI
    اگر (gui.in_use)
    {
	GuiFont font;
	char_u	*name = پوچ;

	اگر (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	وگرنه
	{
	    name = tv_get_string(&argvars[0]);
	    اگر (STRCMP(name, "*") == 0)	    // don't use font dialog
		بازگشت;
	    font = gui_mch_get_font(name, FALSE);
	    اگر (font == NOFONT)
		بازگشت;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	اگر (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#endif
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
ایستا ساختار
{
    argcheck_T	func;
    نویسه	*types;	    // separated by commas
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
    {arg_list_عدد,		"list<number>"},
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
    {arg_عدد,		"number"},
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
    ایستا list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    نویسه	*types = "any";

    اگر (l == پوچ)
	بازگشت پوچ;
    برای (صحیح i = 0; i < (صحیح)ARRAY_LENGTH(argcheck_types); ++i)
	اگر (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    بشکن;
	}
    برای (نویسه *p = types; ; )
    {
	نویسه	*end = (نویسه *)vim_strchr((char_u *)p, ',');
	صحیح	len = end == پوچ ? (صحیح)STRLEN(p) : (صحیح)(end - p);

	اگر (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    بازگشت پوچ;
	}
	اگر (end == پوچ)
	    بشکن;
	p = end + 1;
    }
    بازگشت l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    ایستا صحیح
getinfo_argtypes(
	list_T			*l,
	ثابت funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    صحیح		argcount = 0;

    اگر (l != پوچ)
	FOR_ALL_LIST_اقلام(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    اگر (s == پوچ)
		بازگشت -1;
	    اگر (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_ARGS)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		بازگشت -1;
	    }
	    type = parse_type(&p, type_gap, پوچ, پوچ, TRUE);
	    اگر (type == پوچ)
		بازگشت -1;
	    اگر (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		بازگشت -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    برای ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_ARGS; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    بازگشت argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    ایستا نویسه *
builtin_ret_type_name(
	صحیح	    idx,
	صحیح	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	نویسه	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = internal_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    بازگشت type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
تعریفنوع شمارش {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_kind_T;

ایستا ساختار
{
    نویسه		*name;
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
    ایستا تهی
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_kind_T	kind;
    صحیح			ki;
    صحیح			idx;
    ثابت funcentry_T	*fe;
    dict_T		*d;
    list_T		*args;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_ARGS];
    list_T		*argtypes_list = پوچ;
    صحیح			argcount;
    صحیح			vim9 = -1;	// -1: the context of the caller
    نویسه		*ret_name;
    نویسه		*tofree;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;
    اگر (in_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;
    اگر (check_for_opt_dict_arg(argvars, 2) == FAIL)
	بازگشت;

    kindname = tv_get_string(&argvars[0]);
    برای (ki = 0; ki < (صحیح)ARRAY_LENGTH(getinfo_kinds); ++ki)
	اگر (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    بشکن;
    اگر (ki == (صحیح)ARRAY_LENGTH(getinfo_kinds))
    {
	semsg(_(e_invalid_argument_str), kindname);
	بازگشت;
    }
    kind = getinfo_kinds[ki].kind;
    name = tv_get_string_buf(&argvars[1], namebuf);

    اگر (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != پوچ)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	صحیح	    todo = (صحیح)opts->dv_hashtab.ht_used;

	FOR_ALL_HASHTAB_اقلام(&opts->dv_hashtab, hi, todo)
	{
	    اگر (HASHITEM_EMPTY(hi))
		ادامه;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    اگر (STRCMP(di->di_کلید, "argtypes") == 0 && kind == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    وگرنه اگر (STRCMP(di->di_کلید, "vim9") == 0 && kind == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    وگرنه
	    {
		semsg(_(e_invalid_argument_str), di->di_کلید);
		بازگشت;
	    }
	}
    }
    اگر (kind == GI_VIMVAR)
    {
	اگر (STRNCMP(name, "v:", 2) == 0)
	    (تهی)vim_var_info(name + 2, rettv->vval.v_dict);
	بازگشت;
    }
    اگر (kind == GI_COMMAND)
    {
	(تهی)ex_command_info(name, vim9, rettv->vval.v_dict);
	بازگشت;
    }
    اگر (kind == GI_OPTION)
    {
	(تهی)option_info(name, rettv->vval.v_dict);
	بازگشت;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = find_internal_func_opt(name, FALSE);
    اگر (idx < 0)
    {
	(تهی)user_func_info(name, rettv->vval.v_dict);
	بازگشت;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, اندازه(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    اگر (argcount < 0)
    {
	clear_type_list(&type_gap);
	بازگشت;
    }
    d = rettv->vval.v_dict;
    dict_add_string(d, "name", (char_u *)fe->f_name);
    dict_add_string(d, "kind", (char_u *)"builtin");
    dict_add_bool(d, "available", fe->f_func != پوچ);
    dict_add_عدد(d, "minargs", fe->f_min_argc);
    dict_add_عدد(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_add_عدد(d, "method", fe->f_argtype & FEARG_MASK);

    args = list_alloc();
    اگر (args == پوچ || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	clear_type_list(&type_gap);
	بازگشت;
    }
    argnames = (char_u *)fe->f_argnames;
    برای (صحیح i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = پوچ;
	صحیح	    argnamelen = 0;

	اگر (fe->f_argcheck == پوچ)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    اگر (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		بشکن;
	    check = arg_any;
	}
	وگرنه
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    اگر ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == پوچ)
		بشکن;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	اگر (argnames != پوچ)
	{
	    char_u  *end = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = end == پوچ ? (صحیح)STRLEN(argnames)
						     : (صحیح)(end - argnames);
	    argnames = end == پوچ ? پوچ : skipwhite(end + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	اگر (arg == پوچ || types == پوچ
		|| dict_add_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    بازگشت;
	}
	اگر ((argnamelen > 0
		    && dict_add_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    بازگشت;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    اگر (argtypes_list == پوچ
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	نویسه	*tofree_max;
	نویسه	*max_name;

	برای (صحیح i = argcount; i < fe->f_max_argc && i < MAX_FUNC_ARGS; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	اگر (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = پوچ;
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
    ایستا تهی
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    صحیح		i;
    list_T	*l;
    dict_T	*d;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_opt_عدد_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 1) == FAIL)))
	بازگشت;

    wp = find_tabwin(&argvars[0], &argvars[1], پوچ);
    اگر (wp == پوچ)
	بازگشت;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    اگر (l == پوچ)
	بازگشت;
    اگر (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	بازگشت;
    }

    list_append_عدد(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    برای (i = 0; i < wp->w_jumplistlen; ++i)
    {
	اگر (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    ادامه;
	اگر ((d = dict_alloc()) == پوچ)
	    بازگشت;
	اگر (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    بازگشت;
	}
	dict_add_عدد(d, "lnum", (long)wp->w_jumplist[i].fmark.mark.lnum);
	dict_add_عدد(d, "col", (long)wp->w_jumplist[i].fmark.mark.col);
	dict_add_عدد(d, "coladd", (long)wp->w_jumplist[i].fmark.mark.coladd);
	dict_add_عدد(d, "bufnr", (long)wp->w_jumplist[i].fmark.fnum);
	اگر (wp->w_jumplist[i].fname != پوچ)
	    dict_add_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    ایستا تهی
f_getpid(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_عدد = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    ایستا تهی
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    ایستا تهی
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    ایستا تهی
f_getpos(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    ایستا صحیح
block_def2str(ساختار block_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    اگر (ret->string == پوچ)
    {
	ret->length = 0;
	بازگشت FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->length = bd->startspaces;
    mch_memmove(ret->string + ret->length, bd->textstart, bd->textlen);
    ret->length += bd->textlen;
    vim_memset(ret->string + ret->length, ' ', bd->endspaces);
    ret->length += bd->endspaces;
    ret->string[ret->length] = NUL;

    بازگشت OK;
}

    ایستا صحیح
getregionpos(
    typval_T	*argvars,
    typval_T	*rettv,
    pos_T	*p1,
    pos_T	*p2,
    صحیح		*inclusive,
    صحیح		*region_type,
    oparg_T	*oap)
{
    صحیح		fnum1 = -1, fnum2 = -1;
    char_u	*type;
    buf_T	*findbuf;
    char_u	default_type[] = "v";
    صحیح		block_width = 0;
    صحیح		is_select_exclusive;
    صحیح		l;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت FAIL;

    اگر (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	بازگشت FAIL;

    اگر (list2fpos(&argvars[0], p1, &fnum1, پوچ, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, پوچ, FALSE) != OK
	    || fnum1 != fnum2)
	بازگشت FAIL;

    اگر (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	اگر (type == پوچ)
	    type = default_type;
    }
    وگرنه
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    اگر (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    وگرنه اگر (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    وگرنه اگر (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	اگر (*p != NUL && ((block_width = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_مقدار_for_argument_str_str), "type", type);
	    بازگشت FAIL;
	}
	*region_type = MBLOCK;
    }
    وگرنه
    {
	semsg(_(e_invalid_مقدار_for_argument_str_str), "type", type);
	بازگشت FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    اگر (findbuf == پوچ || findbuf->b_ml.ml_mfp == پوچ)
    {
	emsg(_(e_buffer_is_not_loaded));
	بازگشت FAIL;
    }

    اگر (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_سطر_شمار)
    {
	semsg(_(e_invalid_سطر_عدد_nr), p1->lnum);
	بازگشت FAIL;
    }
    اگر (p1->col == MAXCOL)
	p1->col = ml_get_buf_len(findbuf, p1->lnum) + 1;
    وگرنه اگر (p1->col < 1 || p1->col > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_column_عدد_nr), p1->col);
	بازگشت FAIL;
    }

    اگر (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_سطر_شمار)
    {
	semsg(_(e_invalid_سطر_عدد_nr), p2->lnum);
	بازگشت FAIL;
    }
    اگر (p2->col == MAXCOL)
	p2->col = ml_get_buf_len(findbuf, p2->lnum) + 1;
    وگرنه اگر (p2->col < 1 || p2->col > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_column_عدد_nr), p2->col);
	بازگشت FAIL;
    }

    curbuf = findbuf;
    curwin->w_buffer = curbuf;
    virtual_op = virtual_active();

    // NOTE: Adjustment is needed.
    p1->col--;
    p2->col--;

    اگر (!LT_POS(*p1, *p2))
    {
	// swap position
	pos_T p;

	p = *p1;
	*p1 = *p2;
	*p2 = p;
    }

    اگر (*region_type == MCHAR)
    {
	// Handle 'selection' == "exclusive".
	اگر (is_select_exclusive && !EQUAL_POS(*p1, *p2))
	    // When backing up to previous line, inclusive becomes false.
	    *inclusive = !unadjust_for_sel_inner(p2);
	// If p2 is on NUL (end of line), inclusive becomes false.
	اگر (*inclusive && !virtual_op && *ml_get_pos(p2) == NUL)
	    *inclusive = FALSE;
    }
    وگرنه اگر (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#ifdef FEAT_LINEBREAK
	صحیح	lbr_saved = reset_lbr();
#endif

	getvvcol(curwin, p1, &sc1, پوچ, &ec1, 0);
	getvvcol(curwin, p2, &sc2, پوچ, &ec2, 0);

#ifdef FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#endif

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->start = *p1;
	oap->end = *p2;
	oap->start_vcol = MIN(sc1, sc2);
	اگر (block_width > 0)
	    oap->end_vcol = oap->start_vcol + block_width - 1;
	وگرنه اگر (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->end_vcol = sc2 - 1;
	وگرنه
	    oap->end_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    اگر (l > 1)
	p2->col += l - 1;

    بازگشت OK;
}

/*
 * "getregion()" function
 */
    ایستا تهی
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    صحیح			inclusive = TRUE;
    صحیح			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    صحیح			save_virtual;
    string_T		akt = {پوچ, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    اگر (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	بازگشت;

    برای (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	صحیح ret = OK;

	اگر (region_type == MBLOCK)
	{
	    ساختار block_def	bd;

	    block_prep(&oa, &bd, lnum, FALSE);
	    block_def2str(&bd, &akt);
	}
	وگرنه اگر (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.length = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.length);
	    اگر (akt.string == پوچ)
		akt.length = 0;
	    وگرنه
		akt.length = s.length;
	}
	وگرنه
	{
	    ساختار block_def	bd;

	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);
	    block_def2str(&bd, &akt);
	}

	اگر (akt.string == پوچ)
	{
	    clear_tv(rettv);
	    (تهی)rettv_list_alloc(rettv);
	    بشکن;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (صحیح)akt.length);
	vim_free(akt.string);

	اگر (ret == FAIL)
	{
	    clear_tv(rettv);
	    (تهی)rettv_list_alloc(rettv);
	    بشکن;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_buffer = curbuf;
    virtual_op = save_virtual;
}

    ایستا تهی
add_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    اگر (l1 == پوچ)
	بازگشت;

    اگر (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	بازگشت;
    }

    l2 = list_alloc();
    اگر (l2 == پوچ)
	بازگشت;

    اگر (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	بازگشت;
    }

    l3 = list_alloc();
    اگر (l3 == پوچ)
	بازگشت;

    اگر (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	بازگشت;
    }

    list_append_عدد(l2, curbuf->b_fnum);
    list_append_عدد(l2, p1.lnum);
    list_append_عدد(l2, p1.col);
    list_append_عدد(l2, p1.coladd);

    list_append_عدد(l3, curbuf->b_fnum);
    list_append_عدد(l3, p2.lnum);
    list_append_عدد(l3, p2.col);
    list_append_عدد(l3, p2.coladd);
}

/*
 * Compute the positions of the region segment on line "lnum".
 * "ret_p1" is set to the start position of the segment and "ret_p2" to its
 * end position.
 */
    ایستا تهی
getregionpos_سطر(
    linenr_T	lnum,
    pos_T	p1,
    pos_T	p2,
    صحیح		inclusive,
    صحیح		region_type,
    oparg_T	*oap,
    صحیح		allow_eol,
    pos_T	*ret_p1,
    pos_T	*ret_p2)
{
    char_u	*سطر = ml_get(lnum);
    colnr_T	سطر_len = ml_get_len(lnum);

    اگر (region_type == MLINE)
    {
	ret_p1->col = 1;
	ret_p1->coladd = 0;
	ret_p2->col = MAXCOL;
	ret_p2->coladd = 0;
    }
    وگرنه
    {
	ساختار block_def	bd;

	اگر (region_type == MBLOCK)
	    block_prep(oap, &bd, lnum, FALSE);
	وگرنه
	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);

	اگر (bd.is_oneChar)  // selection entirely inside one char
	{
	    اگر (region_type == MBLOCK)
	    {
		ret_p1->col = mb_prevptr(سطر, bd.textstart) - سطر + 1;
		ret_p1->coladd = bd.start_char_vcols
					   - (bd.start_vcol - oap->start_vcol);
	    }
	    وگرنه
	    {
		ret_p1->col = p1.col + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	وگرنه اگر (region_type == MBLOCK && oap->start_vcol > bd.start_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->col = MAXCOL;
	    ret_p1->coladd = oap->start_vcol - bd.start_vcol;
	    bd.is_oneChar = TRUE;
	}
	وگرنه اگر (bd.startspaces > 0)
	{
	    ret_p1->col = mb_prevptr(سطر, bd.textstart) - سطر + 1;
	    ret_p1->coladd = bd.start_char_vcols - bd.startspaces;
	}
	وگرنه
	{
	    ret_p1->col = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	اگر (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->col = ret_p1->col;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	وگرنه اگر (bd.endspaces > 0)
	{
	    ret_p2->col = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	وگرنه
	{
	    ret_p2->col = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    اگر (!allow_eol && ret_p1->col > سطر_len)
    {
	ret_p1->col = 0;
	ret_p1->coladd = 0;
    }
    وگرنه اگر (ret_p1->col > سطر_len + 1)
	ret_p1->col = سطر_len + 1;

    اگر (!allow_eol && ret_p2->col > سطر_len)
    {
	ret_p2->col = ret_p1->col == 0 ? 0 : سطر_len;
	ret_p2->coladd = 0;
    }
    وگرنه اگر (ret_p2->col > سطر_len + 1)
	ret_p2->col = سطر_len + 1;

    ret_p1->lnum = lnum;
    ret_p2->lnum = lnum;
}

/*
 * "getregionpos()" function
 */
    ایستا تهی
f_getregionpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	p1, p2;
    صحیح		inclusive = TRUE;
    صحیح		region_type = -1;
    صحیح		allow_eol = FALSE;
    صحیح		bounds_only = FALSE;
    oparg_T	oa;
    صحیح		lnum;

    buf_T	*save_curbuf;
    صحیح		save_virtual;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    اگر (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	بازگشت;

    اگر (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    اگر (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	start_pos, end_pos;

	getregionpos_سطر(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &start_pos, &end_pos);
	اگر (p2.lnum != p1.lnum)
	{
	    pos_T	unused;

	    getregionpos_سطر(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &unused, &end_pos);
	}
	add_regionpos_range(rettv, start_pos, end_pos);
    }
    وگرنه
    {
	برای (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_سطر(lnum, p1, p2, inclusive, region_type, &oa,
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
    ایستا صحیح
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	اگر (strregname != پوچ && in_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_register_name_must_be_one_char_str), strregname);
	    strregname = پوچ;
	}
	اگر (strregname == پوچ)	    // type error; errmsg already given
	    بازگشت 0;
    }
    وگرنه
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    بازگشت *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    ایستا تهی
f_getreg(typval_T *argvars, typval_T *rettv)
{
    صحیح		regname;
    صحیح		arg2 = FALSE;
    صحیح		return_list = FALSE;

    اگر (in_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	بازگشت;

    regname = getreg_get_regname(argvars);
    اگر (regname == 0)
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	صحیح		error = FALSE;

	arg2 = (صحیح)tv_get_bool_chk(&argvars[1], &error);

	اگر (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (صحیح)tv_get_bool_chk(&argvars[2], &error);
	اگر (error)
	    بازگشت;
    }

    اگر (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_عبارت_SRC : 0) | GREG_LIST);
	اگر (rettv->vval.v_list == پوچ)
	    (تهی)rettv_list_alloc(rettv);
	وگرنه
	    ++rettv->vval.v_list->lv_refcount;
    }
    وگرنه
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = get_reg_contents(regname,
						    arg2 ? GREG_عبارت_SRC : 0);
    }
}

/*
 * "getregtype()" function
 */
    ایستا تهی
f_getregtype(typval_T *argvars, typval_T *rettv)
{
    صحیح		regname;
    char_u	buf[NUMBUFLEN + 2];
    long	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	بازگشت;

    regname = getreg_get_regname(argvars);
    اگر (regname == 0)
	بازگشت;

    buf[0] = NUL;
    buf[1] = NUL;
    انتخاب (get_reg_type(regname, &reglen))
    {
	حالت MLINE: buf[0] = 'V'; بشکن;
	حالت MCHAR: buf[0] = 'v'; بشکن;
	حالت MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((نویسه *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		بشکن;
    }
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "gettagstack()" function
 */
    ایستا تهی
f_gettagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;			// default is current window

    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	اگر (wp == پوچ)
	    بازگشت;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    ایستا تهی
f_gettext(typval_T *argvars, typval_T *rettv)
{
#اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    نویسه *prev = پوچ;
#endif

    اگر (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	بازگشت;

    rettv->v_type = VAR_STRING;

    اگر (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != پوچ &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ثابت نویسه *)argvars[1].vval.v_string, (نویسه *)p_enc);
#endif

#اگر defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((ثابت نویسه *)argvars[1].vval.v_string, (ثابت نویسه *)argvars[0].vval.v_string));
#وگرنه
	textdomain((ثابت نویسه *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#endif

#اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	اگر (prev != پوچ)
	    bind_textdomain_codeset((ثابت نویسه *)argvars[1].vval.v_string, prev);
#endif
    }
    وگرنه
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
}

// for VIM_VERSION_ defines
#شامل "version.h"

/*
 * "has()" function
 */
    تهی
f_has(typval_T *argvars, typval_T *rettv)
{
    صحیح		i;
    char_u	*name;
    صحیح		x = FALSE;
    صحیح		n = FALSE;
    تعریفنوع ساختار {
	نویسه *name;
	short present;
    } has_item_T;
    ایستا has_item_T has_list[] =
    {
	{"amiga",
#ifdef AMIGA
		1
#وگرنه
		0
#endif
		},
	{"android",
#ifdef __ANDROID__
		1
#وگرنه
		0
#endif
		},
	{"arp",
#اگر defined(AMIGA) && defined(FEAT_ARP)
		1
#وگرنه
		0
#endif
		},
	{"haiku",
#ifdef __HAIKU__
		1
#وگرنه
		0
#endif
		},
	{"bsd",
#اگر defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#وگرنه
		0
#endif
		},
	{"hpux",
#ifdef hpux
		1
#وگرنه
		0
#endif
		},
	{"hurd",
#ifdef __GNU__
		1
#وگرنه
		0
#endif
		},
	{"linux",
#ifdef __linux__
		1
#وگرنه
		0
#endif
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#ifdef MACOS_X
		1
#وگرنه
		0
#endif
		},
	{"osx",		// Mac OS X
#ifdef MACOS_X
		1
#وگرنه
		0
#endif
		},
	{"macunix",	// Mac OS X, with the darwin feature
#اگر defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#وگرنه
		0
#endif
		},
	{"osxdarwin",	// synonym for macunix
#اگر defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#وگرنه
		0
#endif
		},
	{"qnx",
#ifdef __QNX__
		1
#وگرنه
		0
#endif
		},
	{"sun",
#ifdef SUN_SYSTEM
		1
#وگرنه
		0
#endif
		},
	{"termux",
#ifdef __TERMUX__
		1
#وگرنه
		0
#endif
		},
	{"unix",
#ifdef UNIX
		1
#وگرنه
		0
#endif
		},
	{"vms",
#ifdef VMS
		1
#وگرنه
		0
#endif
		},
	{"win32",
#ifdef MSWIN
		1
#وگرنه
		0
#endif
		},
	{"win32unix",
#ifdef WIN32UNIX
		1
#وگرنه
		0
#endif
		},
	{"win64",
#ifdef _WIN64
		1
#وگرنه
		0
#endif
		},
	{"ebcdic", 0 },
	{"fname_case",
#ifndef CASE_INSENSITIVE_FILENAME
		1
#وگرنه
		0
#endif
		},
	{"acl",
#ifdef HAVE_ACL
		1
#وگرنه
		0
#endif
		},
	{"arabic",
#ifdef FEAT_ARABIC
		1
#وگرنه
		0
#endif
		},
	{"autocmd", 1},
	{"autochdir",
#ifdef FEAT_AUTOCHDIR
		1
#وگرنه
		0
#endif
		},
	{"autoservername",
#ifdef FEAT_AUTOSERVERNAME
		1
#وگرنه
		0
#endif
		},
	{"socketserver",
#ifdef FEAT_SOCKETSERVER
		1
#وگرنه
		0
#endif
		},
	{"balloon_eval",
#ifdef FEAT_BEVAL_GUI
		1
#وگرنه
		0
#endif
		},
	{"balloon_multiline",
#ifdef FEAT_BEVAL_GUI
		1
#وگرنه
		0
#endif
		},
	{"balloon_eval_term",
#ifdef FEAT_BEVAL_TERM
		1
#وگرنه
		0
#endif
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#اگر defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#وگرنه
		0
#endif
		},
	{"byte_offset",
#ifdef FEAT_BYTEOFF
		1
#وگرنه
		0
#endif
		},
	{"channel",
#ifdef FEAT_JOB_CHANNEL
		1
#وگرنه
		0
#endif
		},
	{"cindent", 1},
	{"clientserver",
#ifdef FEAT_CLIENTSERVER
		1
#وگرنه
		0
#endif
		},
	{"clipboard",
#ifdef FEAT_CLIPBOARD
		1
#وگرنه
		0
#endif
		},
	{"clipboard_provider",
#ifdef FEAT_CLIPBOARD_PROVIDER
		1
#وگرنه
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
#وگرنه
		0
#endif
		},
	{"cryptv",
#ifdef FEAT_CRYPT
		1
#وگرنه
		0
#endif
		},
	{"crypt-blowfish",
#ifdef FEAT_CRYPT
		1
#وگرنه
		0
#endif
		},
	{"crypt-blowfish2",
#ifdef FEAT_CRYPT
		1
#وگرنه
		0
#endif
		},
	{"cscope",
#ifdef FEAT_CSCOPE
		1
#وگرنه
		0
#endif
		},
	{"cursorbind", 1},
	{"cursorshape",
#ifdef CURSOR_SHAPE
		1
#وگرنه
		0
#endif
		},
	{"debug",
#ifdef DEBUG
		1
#وگرنه
		0
#endif
		},
	{"dialog_con",
#ifdef FEAT_CON_DIALOG
		1
#وگرنه
		0
#endif
		},
	{"dialog_con_gui",
#اگر defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#وگرنه
		0
#endif
		},
	{"dialog_gui",
#ifdef FEAT_GUI_DIALOG
		1
#وگرنه
		0
#endif
		},
	{"diff",
#ifdef FEAT_DIFF
		1
#وگرنه
		0
#endif
		},
	{"digraphs",
#ifdef FEAT_DIGRAPHS
		1
#وگرنه
		0
#endif
		},
	{"directx",
#ifdef FEAT_DIRECTX
		1
#وگرنه
		0
#endif
		},
	{"dnd",
#ifdef FEAT_DND
		1
#وگرنه
		0
#endif
		},
	{"drop_file",
#ifdef HAVE_DROP_FILE
		1
#وگرنه
		0
#endif
		},
	{"emacs_tags",
#ifdef FEAT_EMACS_TAGS
		1
#وگرنه
		0
#endif
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#ifdef FEAT_جستجو_EXTRA
		1
#وگرنه
		0
#endif
		},
	{"file_in_path", 1},
	{"filterpipe",
#اگر defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#وگرنه
		0
#endif
		},
	{"find_in_path",
#ifdef FEAT_FIND_ID
		1
#وگرنه
		0
#endif
		},
	{"float", 1},
	{"folding",
#ifdef FEAT_FOLDING
		1
#وگرنه
		0
#endif
		},
	{"footer", 0},
	{"fork",
#اگر !defined(USE_SYSTEM) && defined(UNIX)
		1
#وگرنه
		0
#endif
		},
	{"gettext",
#ifdef FEAT_GETTEXT
		1
#وگرنه
		0
#endif
		},
	{"gui",
#ifdef FEAT_GUI
		1
#وگرنه
		0
#endif
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#ifdef FEAT_GUI_GTK
		1
#وگرنه
		0
#endif
		},
	{"gui_gtk2",
#اگر defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#وگرنه
		0
#endif
		},
	{"gui_gtk3",
#اگر defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#وگرنه
		0
#endif
		},
	{"gui_gtk4",
#اگر defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#وگرنه
		0
#endif
		},
	{"gui_gnome",
#ifdef FEAT_GUI_GNOME
		1
#وگرنه
		0
#endif
		},
	{"gui_haiku",
#ifdef FEAT_GUI_HAIKU
		1
#وگرنه
		0
#endif
		},
	{"gui_mac", 0},
	{"gui_motif",
#ifdef FEAT_GUI_MOTIF
		1
#وگرنه
		0
#endif
		},
	{"gui_photon",
#ifdef FEAT_GUI_PHOTON
		1
#وگرنه
		0
#endif
		},
	{"gui_win32",
#ifdef FEAT_GUI_MSWIN
		1
#وگرنه
		0
#endif
		},
	{"iconv",
#اگر defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#وگرنه
		0
#endif
		},
	{"image",
#ifdef FEAT_IMAGE
		1
#وگرنه
		0
#endif
		},
	{"image_popup",
#ifdef FEAT_IMAGE_POPUP
		1
#وگرنه
		0
#endif
		},
	{"insert_expand", 1},
	{"ipv6",
#ifdef FEAT_IPV6
		1
#وگرنه
		0
#endif
	},
	{"job",
#ifdef FEAT_JOB_CHANNEL
		1
#وگرنه
		0
#endif
		},
	{"jumplist", 1},
	{"keymap",
#ifdef FEAT_KEYMAP
		1
#وگرنه
		0
#endif
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#ifdef FEAT_LANGMAP
		1
#وگرنه
		0
#endif
		},
	{"libcall",
#ifdef FEAT_LIBCALL
		1
#وگرنه
		0
#endif
		},
	{"linebreak",
#ifdef FEAT_LINEBREAK
		1
#وگرنه
		0
#endif
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#اگر defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#وگرنه
		0
#endif
		},
	{"menu",
#ifdef FEAT_MENU
		1
#وگرنه
		0
#endif
		},
	{"mksession",
#ifdef FEAT_SESSION
		1
#وگرنه
		0
#endif
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#ifdef FEAT_MOUSESHAPE
		1
#وگرنه
		0
#endif
		},
	{"mouse_dec",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#وگرنه
		0
#endif
		},
	{"mouse_gpm",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#وگرنه
		0
#endif
		},
	{"mouse_jsbterm",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#وگرنه
		0
#endif
		},
	{"mouse_netterm",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#وگرنه
		0
#endif
		},
	{"mouse_pterm",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#وگرنه
		0
#endif
		},
	{"mouse_sgr",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#وگرنه
		0
#endif
		},
	{"mouse_sysmouse",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#وگرنه
		0
#endif
		},
	{"mouse_urxvt",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#وگرنه
		0
#endif
		},
	{"mouse_xterm",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#وگرنه
		0
#endif
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#ifdef FEAT_MBYTE_IME
		1
#وگرنه
		0
#endif
		},
	{"multi_lang",
#ifdef FEAT_MULTI_LANG
		1
#وگرنه
		0
#endif
		},
	{"mzscheme",
#اگر defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#وگرنه
		0
#endif
		},
	{"nanotime",
#ifdef ST_MTIM_NSEC
		1
#وگرنه
		0
#endif
	},
	{"num64", 1},
	{"ole",
#ifdef FEAT_OLE
		1
#وگرنه
		0
#endif
		},
	{"packages",
#ifdef FEAT_EVAL
		1
#وگرنه
		0
#endif
		},
	{"path_extra", 1},
	{"perl",
#اگر defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#وگرنه
		0
#endif
		},
	{"persistent_undo",
#ifdef FEAT_PERSISTENT_UNDO
		1
#وگرنه
		0
#endif
		},
	{"python_compiled",
#اگر defined(FEAT_PYTHON)
		1
#وگرنه
		0
#endif
		},
	{"python_dynamic",
#اگر defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#وگرنه
		0
#endif
		},
	{"python",
#اگر defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#وگرنه
		0
#endif
		},
	{"pythonx",
#اگر (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#وگرنه
		0
#endif
		},
	{"python3_compiled",
#اگر defined(FEAT_PYTHON3)
		1
#وگرنه
		0
#endif
		},
	{"python3_dynamic",
#اگر defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#وگرنه
		0
#endif
		},
	{"python3_stable",
#اگر defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#وگرنه
		0
#endif
		},
	{"python3",
#اگر defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#وگرنه
		0
#endif
		},
	{"popupwin",
#ifdef FEAT_PROP_POPUP
		1
#وگرنه
		0
#endif
		},
	{"postscript",
#ifdef FEAT_POSTSCRIPT
		1
#وگرنه
		0
#endif
		},
	{"pango",
#ifdef FEAT_PRINT_PANGO
		1
#وگرنه
		0
#endif
		},
	{"printer",
#ifdef FEAT_PRINTER
		1
#وگرنه
		0
#endif
		},
	{"profile",
#ifdef FEAT_PROFILE
		1
#وگرنه
		0
#endif
		},
	{"prof_nsec",
#ifdef PROF_NSEC
		1
#وگرنه
		0
#endif
		},
	{"reltime",
#ifdef FEAT_RELTIME
		1
#وگرنه
		0
#endif
		},
	{"quickfix",
#ifdef FEAT_QUICKFIX
		1
#وگرنه
		0
#endif
		},
	{"rightleft",
#ifdef FEAT_RIGHTLEFT
		1
#وگرنه
		0
#endif
		},
	{"ruby",
#اگر defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#وگرنه
		0
#endif
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#ifdef FEAT_SIGNS
		1
#وگرنه
		0
#endif
		},
	{"smartindent", 1},
	{"startuptime",
#ifdef STARTUPTIME
		1
#وگرنه
		0
#endif
		},
	{"statusline",
#ifdef FEAT_STL_OPT
		1
#وگرنه
		0
#endif
		},
	{"statusline_click",
#ifdef FEAT_STL_OPT
		1
#وگرنه
		0
#endif
		},
	{"netbeans_intg",
#ifdef FEAT_NETBEANS_INTG
		1
#وگرنه
		0
#endif
		},
	{"sodium",
#اگر defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#وگرنه
		0
#endif
		},
	{"sound",
#ifdef FEAT_SOUND
		1
#وگرنه
		0
#endif
		},
	{"spell",
#ifdef FEAT_SPELL
		1
#وگرنه
		0
#endif
		},
	{"syntax",
#ifdef FEAT_SYN_HL
		1
#وگرنه
		0
#endif
		},
	{"system",
#اگر defined(USE_SYSTEM) || !defined(UNIX)
		1
#وگرنه
		0
#endif
		},
	{"tabpanel",
#اگر defined(FEAT_TABPANEL)
		1,
#وگرنه
		0,
#endif
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#اگر defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#وگرنه
		0
#endif
		},
	{"termguicolors",
#ifdef FEAT_TERMGUICOLORS
		1
#وگرنه
		0
#endif
		},
	{"terminal",
#اگر defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#وگرنه
		0
#endif
		},
	{"terminfo",
#ifdef TERMINFO
		1
#وگرنه
		0
#endif
		},
	{"termresponse",
#ifdef FEAT_TERMRESPONSE
		1
#وگرنه
		0
#endif
		},
	{"textobjects", 1},
	{"textprop",
#ifdef FEAT_PROP_POPUP
		1
#وگرنه
		0
#endif
		},
	{"tgetent",
#ifdef HAVE_TGETENT
		1
#وگرنه
		0
#endif
		},
	{"timers",
#ifdef FEAT_TIMERS
		1
#وگرنه
		0
#endif
		},
	{"title", 1},
	{"toolbar",
#ifdef FEAT_TOOLBAR
		1
#وگرنه
		0
#endif
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#ifdef FEAT_VARTABS
		1
#وگرنه
		0
#endif
		},
	{"vertsplit", 1},
	{"viminfo",
#ifdef FEAT_VIMINFO
		1
#وگرنه
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
#وگرنه
		0
#endif
		},
	{"wayland",
#ifdef FEAT_WAYLAND
		1
#وگرنه
		0
#endif
		},
	{"wayland_clipboard",
#ifdef FEAT_WAYLAND_CLIPBOARD
		1
#وگرنه
		0
#endif
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#ifdef FEAT_WAK
		1
#وگرنه
		0
#endif
		},
	{"writebackup",
#ifdef FEAT_WRITEBACKUP
		1
#وگرنه
		0
#endif
		},
	{"xattr",
#ifdef FEAT_XATTR
		1
#وگرنه
		0
#endif
		},
	{"xim",
#ifdef FEAT_XIM
		1
#وگرنه
		0
#endif
		},
	{"xfontset",
#ifdef FEAT_XFONTSET
		1
#وگرنه
		0
#endif
		},
	{"xpm",
#اگر defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#وگرنه
		0
#endif
		},
	{"xpm_w32",	// for backward compatibility
#ifdef FEAT_XPM_W32
		1
#وگرنه
		0
#endif
		},
	{"xsmp",
#ifdef USE_XSMP
		1
#وگرنه
		0
#endif
		},
	{"xsmp_interact",
#ifdef USE_XSMP_INTERACT
		1
#وگرنه
		0
#endif
		},
	{"xterm_clipboard",
#ifdef FEAT_XCLIPBOARD
		1
#وگرنه
		0
#endif
		},
	{"xterm_save",
#ifdef FEAT_XTERM_SAVE
		1
#وگرنه
		0
#endif
		},
	{"X11",
#اگر defined(UNIX) && defined(FEAT_X11)
		1
#وگرنه
		0
#endif
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#وگرنه
		0
#endif
		},
	{پوچ, 0}
    };

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	بازگشت;

    name = tv_get_string(&argvars[0]);

    // Fast-path: check features not in has_list[] first to avoid the full
    // linear scan for very common queries like has('patch-...').
    اگر (STRNICMP(name, "patch", 5) == 0)
    {
	x = TRUE;
	اگر (name[5] == '-'
		&& STRLEN(name) >= 11
		&& (name[6] >= '1' && name[6] <= '9'))
	{
	    نویسه	*end;
	    صحیح	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (صحیح)strtoul((نویسه *)name + 6, &end, 10);
	    اگر (*end == '.' && vim_isdigit(end[1])
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
	وگرنه اگر (SAFE_isdigit(name[5]))
	    n = has_patch(atoi((نویسه *)name + 5));
    }
    وگرنه اگر (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    وگرنه اگر (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_درون_داد_isatty();
    }
    وگرنه اگر (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    وگرنه اگر (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    وگرنه اگر (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#ifdef FEAT_GUI
	n = (gui.in_use || gui.starting);
#endif
    }
    وگرنه اگر (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#اگر defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.in_use;	// gui_mch_browse() works when GUI is running
#endif
    }
    وگرنه اگر (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#ifdef FEAT_SYN_HL
	n = syntax_present(curwin);
#endif
    }
    وگرنه اگر (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#ifdef FEAT_VTP
	n = is_term_win32() && has_vtp_working();
#endif
    }
    وگرنه اگر (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_NETBEANS_INTG
	n = netbeans_active();
#endif
    }
    وگرنه اگر (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_MOUSE_GPM
	n = gpm_enabled();
#endif
    }
    وگرنه اگر (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#اگر defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#endif
    }
    وگرنه اگر (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_star.available;
#endif
    }
    وگرنه اگر (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# اگر defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	اگر (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	وگرنه
	    n = FALSE;
# وگرنه
	n = FALSE;
# endif
#endif
    }

    // Look up in has_list[] only if not already handled above.
    اگر (x == FALSE)
    {
	برای (i = 0; has_list[i].name != پوچ; ++i)
	    اگر (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		بشکن;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	اگر (x == TRUE && n == FALSE)
	{
	    اگر (0)
	    {
		// intentionally empty
	    }
#ifdef VIMDLL
	    وگرنه اگر (STRICMP(name, "filterpipe") == 0)
		n = gui.in_use || gui.starting;
#endif
#اگر defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    وگرنه اگر (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#endif
#ifdef DYNAMIC_LUA
	    وگرنه اگر (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#endif
#ifdef DYNAMIC_MZSCHEME
	    وگرنه اگر (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#endif
#ifdef DYNAMIC_PERL
	    وگرنه اگر (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON
	    وگرنه اگر (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON3
	    وگرنه اگر (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#endif
#اگر defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    وگرنه اگر (STRICMP(name, "pythonx") == 0)
	    {
# اگر defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		اگر (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		وگرنه اگر (p_pyx == 3)
		    n = python3_enabled(FALSE);
		وگرنه اگر (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#endif
#ifdef DYNAMIC_RUBY
	    وگرنه اگر (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#endif
#ifdef DYNAMIC_TCL
	    وگرنه اگر (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#endif
#ifdef DYNAMIC_SODIUM
	    وگرنه اگر (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#endif
#اگر defined(FEAT_TERMINAL) && defined(MSWIN)
	    وگرنه اگر (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#endif
#ifdef DYNAMIC_GPM
	    وگرنه اگر (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_available();
#endif
	}
    }

    اگر (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_عدد = x;
    وگرنه
	// return whether feature is enabled
	rettv->vval.v_عدد = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    صحیح
dynamic_feature(char_u *feature)
{
    بازگشت (feature == پوچ
#اگر defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.in_use)
#endif
#ifdef VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#endif
#اگر defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.in_use)
#endif
#اگر defined(USE_ICONV) && defined(DYNAMIC_ICONV)
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
#اگر defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
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
#اگر defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#endif
	    );
}

/*
 * "haslocaldir()" function
 */
    ایستا تهی
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = پوچ;
    win_T	*wp = پوچ;

    اگر (in_vim9script()
	    && (check_for_opt_عدد_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 1) == FAIL)))
	بازگشت;

    wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    اگر (wp != پوچ && wp->w_localdir != پوچ)
	rettv->vval.v_عدد = 1;
    وگرنه اگر (tp != پوچ && tp->tp_localdir != پوچ)
	rettv->vval.v_عدد = 2;
    وگرنه
	rettv->vval.v_عدد = 0;
}

/*
 * "highlightID(name)" function
 */
    ایستا تهی
f_hlID(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    ایستا تهی
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    ایستا تهی
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
# اگر defined(HAVE_NO_LONG_LONG)
تعریفنوع unsigned صحیح uintptr_t;
# وگرنه
تعریفنوع unsigned long long uintptr_t;
# endif
#endif // VMS

    ایستا تهی
f_id(typval_T *argvars, typval_T *rettv)
{
    نویسه    numbuf[NUMBUFLEN];
    نویسه    *p = numbuf;

    انتخاب (argvars[0].v_type)
    {
	حالت VAR_LIST:
	حالت VAR_TUPLE:
	حالت VAR_DICT:
	حالت VAR_OBJECT:
	حالت VAR_JOB:
	حالت VAR_CHANNEL:
	حالت VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    اگر (argvars[0].vval.v_object != پوچ)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(تهی *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		برای (; p < numbuf + اندازه(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	پیشفرض:
	    بشکن;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    ایستا تهی
index_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    صحیح		start = 0;
    صحیح		error = FALSE;
    صحیح		ic = FALSE;

    b = argvars[0].vval.v_blob;
    اگر (b == پوچ)
	بازگشت;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	start = tv_get_عدد_chk(&argvars[2], &error);
	اگر (error)
	    بازگشت;
    }

    اگر (start < 0)
    {
	start = blob_len(b) + start;
	اگر (start < 0)
	    start = 0;
    }

    برای (صحیح idx = start; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_عدد;
	tv.vval.v_عدد = blob_get(b, idx);
	اگر (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_عدد = idx;
	    بازگشت;
	}
    }
}

/*
 * index() function for a tuple
 */
    ایستا تهی
index_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    صحیح		ic = FALSE;
    صحیح		error = FALSE;

    اگر (tuple == پوچ)
	بازگشت;

    صحیح	start_idx = 0;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	start_idx = tv_get_عدد_chk(&argvars[2], &error);
	اگر (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (صحیح)tv_get_bool_chk(&argvars[3], &error);
	اگر (error)
	    بازگشت;
    }

    rettv->vval.v_عدد = index_tuple(tuple, &argvars[1], start_idx, ic);
}

/*
 * index() function for a list
 */
    ایستا تهی
index_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*item;
    long	idx = 0;
    صحیح		ic = FALSE;
    صحیح		error = FALSE;

    l = argvars[0].vval.v_list;
    اگر (l == پوچ)
	بازگشت;

    CHECK_LIST_MATERIALIZE(l);
    item = l->lv_first;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, (long)tv_get_عدد_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	اگر (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (صحیح)tv_get_bool_chk(&argvars[3], &error);
	اگر (error)
	    item = پوچ;
    }

    برای ( ; item != پوچ; item = item->li_next, ++idx)
	اگر (tv_equal(&item->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_عدد = idx;
	    بشکن;
	}
}

/*
 * "index()" function
 */
    ایستا تهی
f_index(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = -1;

    اگر (in_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_عدد_arg(argvars, 1) == FAIL)
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	بازگشت;

    اگر (argvars[0].v_type == VAR_BLOB)
	index_func_blob(argvars, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_TUPLE)
	index_func_tuple(argvars, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_LIST)
	index_func_list(argvars, rettv);
    وگرنه
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    صحیح
indexof_eval_عبارت(typval_T *عبارت)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	found;
    صحیح		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_کلید);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    اگر (eval_عبارت_typval(عبارت, FALSE, argv, 2, پوچ, &newtv) == FAIL)
	بازگشت FALSE;

    found = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    بازگشت error ? FALSE : found;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    ایستا صحیح
indexof_blob(blob_T *b, long startidx, typval_T *عبارت)
{
    long	idx = 0;

    اگر (b == پوچ)
	بازگشت -1;

    اگر (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	اگر (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_کلید, VAR_عدد);
    set_vim_var_type(VV_VAL, VAR_عدد);

    صحیح		called_emsg_start = called_emsg;
    برای (idx = startidx; idx < blob_len(b); ++idx)
    {
	set_vim_var_nr(VV_کلید, idx);
	set_vim_var_nr(VV_VAL, blob_get(b, idx));

	اگر (indexof_eval_عبارت(عبارت))
	    بازگشت idx;

	اگر (called_emsg != called_emsg_start)
	    بازگشت -1;
    }

    بازگشت -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ایستا صحیح
indexof_list(list_T *l, long startidx, typval_T *عبارت)
{
    listitem_T	*item;
    long	idx = 0;
    صحیح		found;

    اگر (l == پوچ)
	بازگشت -1;

    CHECK_LIST_MATERIALIZE(l);

    اگر (startidx == 0)
	item = l->lv_first;
    وگرنه
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, startidx);
	اگر (item != پوچ)
	    idx = l->lv_u.mat.lv_idx;
    }

    set_vim_var_type(VV_کلید, VAR_عدد);

    صحیح		called_emsg_start = called_emsg;
    برای ( ; item != پوچ; item = item->li_next, ++idx)
    {
	set_vim_var_nr(VV_کلید, idx);
	copy_tv(&item->li_tv, get_vim_var_tv(VV_VAL));

	found = indexof_eval_عبارت(عبارت);
	clear_tv(get_vim_var_tv(VV_VAL));

	اگر (found)
	    بازگشت idx;

	اگر (called_emsg != called_emsg_start)
	    بازگشت -1;
    }

    بازگشت -1;
}

/*
 * "indexof()" function
 */
    ایستا تهی
f_indexof(typval_T *argvars, typval_T *rettv)
{
    long	startidx = 0;
    typval_T	save_val;
    typval_T	save_کلید;
    صحیح		save_did_emsg;

    rettv->vval.v_عدد = -1;

    اگر (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	بازگشت;

    اگر ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == پوچ
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == پوچ))
	بازگشت;

    اگر (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_عدد_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_کلید, &save_کلید);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    اگر (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_عدد = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    وگرنه اگر (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_عدد = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    وگرنه
	rettv->vval.v_عدد = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_کلید, &save_کلید);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

ایستا صحیح inputsecret_flag = 0;

/*
 * "input()" function
 *     Also handles inputsecret() when inputsecret is set.
 */
    ایستا تهی
f_درون_داد(typval_T *argvars, typval_T *rettv)
{
    get_user_درون_داد(argvars, rettv, FALSE, inputsecret_flag);
}

/*
 * "inputdialog()" function
 */
    ایستا تهی
f_inputdialog(typval_T *argvars, typval_T *rettv)
{
#اگر defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    اگر (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == پوچ)
    {
	char_u	*message;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	اگر (in_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    بازگشت;

	message = tv_get_string_chk(&argvars[0]);
	اگر (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != پوچ)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	وگرنه
	    IObuff[0] = NUL;
	اگر (message != پوچ && defstr != پوچ
		&& do_dialog(VIM_QUESTION, پوچ, message,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	وگرنه
	{
	    اگر (message != پوچ && defstr != پوچ
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    وگرنه
		rettv->vval.v_string = پوچ;
	}
	rettv->v_type = VAR_STRING;
    }
    وگرنه
#endif
	get_user_درون_داد(argvars, rettv, TRUE, inputsecret_flag);
}

/*
 * "inputlist()" function
 */
    ایستا تهی
f_inputlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    صحیح		selected;
    صحیح		mouse_used;

#ifdef NO_CONSOLE_درون_داد
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    اگر (no_console_درون_داد() && !is_not_a_term())
	بازگشت;
#endif
    اگر (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == پوچ)
    {
	semsg(_(e_argument_of_str_must_be_list), "inputlist()");
	بازگشت;
    }

    پیام_start();
    پیام_row = Rows - 1;	// for when 'cmdheight' > 1
    سطرها_چپ = Rows;	// avoid more prompt
    پیام_scroll = TRUE;
    پیام_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_اقلام(l, li)
    {
	پیام_puts((نویسه *)tv_get_string(&li->li_tv));
	پیام_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_عدد(&mouse_used);
    اگر (mouse_used)
	selected -= سطرها_چپ;

    rettv->vval.v_عدد = selected;
}

ایستا garray_T	    ga_userinput = {0, 0, اندازه(tasave_T), 4, پوچ};

/*
 * "inputrestore()" function
 */
    ایستا تهی
f_inputrestore(typval_T *argvars UNUSED, typval_T *rettv)
{
    اگر (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_data)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    وگرنه اگر (p_verbose > 1)
    {
	verb_پیام(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_عدد = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    ایستا تهی
f_inputsave(typval_T *argvars UNUSED, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    اگر (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_data)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    وگرنه
	rettv->vval.v_عدد = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    ایستا تهی
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	بازگشت;

    ++cmdline_star;
    ++inputsecret_flag;
    f_درون_داد(argvars, rettv);
    --cmdline_star;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    ایستا تهی
f_interrupt(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    ایستا تهی
f_invert(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = ~tv_get_عدد_chk(&argvars[0], پوچ);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    ایستا تهی
free_lval_ریشه(lval_ریشه_T *ریشه)
{
    اگر (ریشه->lr_tv != پوچ)
	free_tv(ریشه->lr_tv);
    class_unref(ریشه->lr_cl_exec);
    ریشه->lr_tv = پوچ;
    ریشه->lr_cl_exec = پوچ;
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

    ایستا صحیح
fix_variable_reference_lval_ریشه(lval_ریشه_T *ریشه, char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    صحیح found_member = FALSE;
    اگر (ریشه->lr_tv->v_type == VAR_OBJECT)
    {
	اگر (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    found_member = TRUE;
    }
    اگر (!found_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*end;
	برای (end = name; ASCII_ISALNUM(*end) || *end == '_'; ++end)
	    ;
	صحیح idx = class_member_idx(ریشه->lr_cl_exec, name, end - name);
	اگر (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(ریشه->lr_tv);
	    copy_tv(&ریشه->lr_cl_exec->class_members_tv[idx], ریشه->lr_tv);
	    found_member = TRUE;
	}
    }
    اگر (!found_member)
    {
	free_tv(ریشه->lr_tv);
	ریشه->lr_tv = پوچ;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    بازگشت OK;
}

/*
 * "islocked()" function
 */
    ایستا تهی
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*end;
    dictitem_T	*di;

    rettv->vval.v_عدد = -1;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    char_u *name = tv_get_string(&argvars[0]);
#ifdef LOG_LOCKVAR
    ch_log(پوچ, "LKVAR: f_islocked(): name: %s", name);
#endif

    lval_ریشه_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_ریشه_T *ریشه = پوچ;

    // Set up lval_root if executing in a method.
    اگر (fill_exec_lval_ریشه(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	اگر (fix_variable_reference_lval_ریشه(&aroot, name) == OK)
	    ریشه = &aroot;
    }

    lval_ریشه_T	*lval_ریشه_save = lval_ریشه;
    lval_ریشه = ریشه;
    end = get_lval(name, پوچ, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_START);
    lval_ریشه = lval_ریشه_save;

    اگر (end != پوچ && lv.ll_name != پوچ)
    {
	اگر (*end != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_end
		   ? e_invalid_argument_str : e_trailing_characters_str), end);
	}
	وگرنه
	{
	    اگر (lv.ll_tv == پوچ)
	    {
		di = find_var(lv.ll_name, پوچ, TRUE);
		اگر (di != پوچ)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_عدد = ((di->di_flags & DI_FLAGS_LOCK)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    وگرنه اگر (lv.ll_is_ریشه)
	    {
		rettv->vval.v_عدد = tv_islocked(lv.ll_tv);
	    }
	    وگرنه اگر (lv.ll_object != پوچ)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_عدد = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(پوچ, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#endif
	    }
	    وگرنه اگر (lv.ll_class != پوچ)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_عدد = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(پوچ, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#endif
	    }
	    وگرنه اگر (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    وگرنه اگر (lv.ll_newkey != پوچ)
		semsg(_(e_کلید_not_present_in_dictionary_str), lv.ll_newkey);
	    وگرنه اگر (lv.ll_list != پوچ)
		// List item.
		rettv->vval.v_عدد = tv_islocked(&lv.ll_li->li_tv);
	    وگرنه اگر (lv.ll_tuple != پوچ)
		// Tuple item.
		rettv->vval.v_عدد = tv_islocked(lv.ll_tv);
	    وگرنه
		// Dictionary item.
		rettv->vval.v_عدد = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    اگر (ریشه != پوچ)
	free_lval_ریشه(ریشه);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    ایستا تهی
f_اقلام(typval_T *argvars, typval_T *rettv)
{
    اگر (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    وگرنه
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), 1);
}

/*
 * "keytrans()" function
 */
    ایستا تهی
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    اگر (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == پوچ)
	بازگشت;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    ایستا تهی
f_last_buffer_nr(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحیح		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	اگر (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_عدد = n;
}

/*
 * "len()" function
 */
    تهی
f_len(typval_T *argvars, typval_T *rettv)
{
    انتخاب (argvars[0].v_type)
    {
	حالت VAR_STRING:
	حالت VAR_عدد:
	    rettv->vval.v_عدد = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    بشکن;
	حالت VAR_BLOB:
	    rettv->vval.v_عدد = blob_len(argvars[0].vval.v_blob);
	    بشکن;
	حالت VAR_LIST:
	    rettv->vval.v_عدد = list_len(argvars[0].vval.v_list);
	    بشکن;
	حالت VAR_TUPLE:
	    rettv->vval.v_عدد = tuple_len(argvars[0].vval.v_tuple);
	    بشکن;
	حالت VAR_DICT:
	    rettv->vval.v_عدد = dict_len(argvars[0].vval.v_dict);
	    بشکن;
	حالت VAR_OBJECT:
	    rettv->vval.v_عدد = object_len(argvars[0].vval.v_object);
	    بشکن;
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	حالت VAR_FLOAT:
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	حالت VAR_JOB:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_CLASS:
	حالت VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    بشکن;
    }
}

    ایستا تهی
libcall_common(typval_T *argvars UNUSED, typval_T *rettv, صحیح type)
{
#ifdef FEAT_LIBCALL
    char_u		*string_in;
    char_u		**string_result;
    صحیح			nr_result;
#endif

    rettv->v_type = type;
    اگر (type != VAR_عدد)
	rettv->vval.v_string = پوچ;

    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_عدد_arg(argvars, 2) == FAIL))
	بازگشت;

#ifdef FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    اگر (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_in = پوچ;
	اگر (argvars[2].v_type == VAR_STRING)
	    string_in = argvars[2].vval.v_string;
	اگر (type == VAR_عدد)
	{
	    string_result = پوچ;
	}
	وگرنه
	{
	    rettv->vval.v_string = پوچ;
	    string_result = &rettv->vval.v_string;
	}
	اگر (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_in,
			     argvars[2].vval.v_عدد,
			     string_result,
			     &nr_result) == OK
		&& type == VAR_عدد)
	    rettv->vval.v_عدد = nr_result;
    }
#endif
}

/*
 * "libcall()" function
 */
    ایستا تهی
f_libcall(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_STRING);
}

/*
 * "libcallnr()" function
 */
    ایستا تهی
f_libcallnr(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_عدد);
}

/*
 * "line(string, [winid])" function
 */
    ایستا تهی
f_سطر(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum = 0;
    pos_T	*fp = پوچ;
    صحیح		fnum;
    صحیح		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (صحیح)tv_get_عدد(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	اگر (wp != پوچ && tp != پوچ)
	{
	    اگر (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		اگر (*p_spk != 'c'
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
    وگرنه
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    اگر (fp != پوچ)
	lnum = fp->lnum;
    rettv->vval.v_عدد = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    ایستا تهی
f_line2byte(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_عدد = -1;
#وگرنه
    linenr_T	lnum;

    اگر (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	بازگشت;

    lnum = tv_get_lnum(argvars);
    اگر (lnum < 1 || lnum > curbuf->b_ml.ml_سطر_شمار + 1)
	rettv->vval.v_عدد = -1;
    وگرنه
	rettv->vval.v_عدد = ml_find_سطر_or_offset(curbuf, lnum, پوچ);
    اگر (rettv->vval.v_عدد >= 0)
	++rettv->vval.v_عدد;
#endif
}

#ifdef FEAT_LUA
/*
 * "luaeval()" function
 */
    ایستا تهی
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#endif

تعریفنوع شمارش
{
    MATCH_END,	    // matchend()
    MATCH_MATCH,    // match()
    MATCH_STR,	    // matchstr()
    MATCH_LIST,	    // matchlist()
    MATCH_POS	    // matchstrpos()
} matchtype_T;

    ایستا تهی
find_some_match(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = پوچ;
    long	len = 0;
    char_u	*عبارت = پوچ;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    long	start = 0;
    long	nth = 1;
    colnr_T	startcol = 0;
    صحیح		match = 0;
    list_T	*l = پوچ;
    listitem_T	*li = پوچ;
    long	idx = 0;
    صحیح		prev_lock = 0;
    char_u	*tofree = پوچ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    rettv->vval.v_عدد = -1;
    اگر (type == MATCH_LIST || type == MATCH_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	اگر (rettv_list_alloc(rettv) == FAIL)
	    برو theend;
	اگر (type == MATCH_POS
		&& (list_append_string(rettv->vval.v_list,
					    (char_u *)"", 0) == FAIL
		    || list_append_عدد(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_عدد(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_عدد(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL))
	{
		list_free(rettv->vval.v_list);
		rettv->vval.v_list = پوچ;
		برو theend;
	}
    }
    وگرنه اگر (type == MATCH_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = پوچ;
    }

    اگر (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 3) == FAIL)))
	برو theend;

    اگر (argvars[0].v_type == VAR_LIST)
    {
	اگر ((l = argvars[0].vval.v_list) == پوچ)
	    برو theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    وگرنه
    {
	عبارت = str = tv_get_string(&argvars[0]);
	len = (long)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    اگر (pat == پوچ)
	برو theend;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	صحیح	    error = FALSE;

	start = (long)tv_get_عدد_chk(&argvars[2], &error);
	اگر (error)
	    برو theend;
	اگر (l != پوچ)
	{
	    li = list_find(l, start);
	    اگر (li == پوچ)
		برو theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	وگرنه
	{
	    اگر (start < 0)
		start = 0;
	    اگر (start > len)
		برو theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    اگر (argvars[3].v_type != VAR_UNKNOWN)
		startcol = start;
	    وگرنه
	    {
		str += start;
		len -= start;
	    }
	}

	اگر (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (long)tv_get_عدد_chk(&argvars[3], &error);
	اگر (error)
	    برو theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    اگر (regmatch.regprog != پوچ)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	اگر (l != پوچ)
	{
	    prev_lock = l->lv_lock;
	    اگر (l->lv_lock == 0)
		l->lv_lock = VAR_LOCKED;
	}

	برای (;;)
	{
	    اگر (l != پوچ)
	    {
		اگر (li == پوچ)
		{
		    match = FALSE;
		    بشکن;
		}
		vim_free(tofree);
		عبارت = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		اگر (str == پوچ)
		    بشکن;
	    }

	    match = vim_regexec_nl(&regmatch, str, startcol);

	    اگر (match && --nth <= 0)
		بشکن;
	    اگر (l == پوچ && !match)
		بشکن;

	    // Advance to just after the match.
	    اگر (l != پوچ)
	    {
		li = li->li_next;
		++idx;
	    }
	    وگرنه
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		اگر (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    match = FALSE;
		    بشکن;
		}
	    }
	}

	اگر (match)
	{
	    اگر (type == MATCH_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_first;
		listitem_T *li2 = li1->li_next;
		listitem_T *li3 = li2->li_next;
		listitem_T *li4 = li3->li_next;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_عدد =
				      (varnumber_T)(regmatch.startp[0] - عبارت);
		li4->li_tv.vval.v_عدد =
					(varnumber_T)(regmatch.endp[0] - عبارت);
		اگر (l != پوچ)
		    li2->li_tv.vval.v_عدد = (varnumber_T)idx;
	    }
	    وگرنه اگر (type == MATCH_LIST)
	    {
		صحیح i;

		// return list with matched string and submatches
		برای (i = 0; i < NSUBEXP; ++i)
		{
		    اگر (regmatch.endp[i] == پوچ)
		    {
			اگر (list_append_string(rettv->vval.v_list,
							    پوچ, 0) == FAIL)
			    بشکن;
		    }
		    وگرنه اگر (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(صحیح)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			بشکن;
		}
	    }
	    وگرنه اگر (type == MATCH_STR)
	    {
		// return matched string
		اگر (l != پوچ)
		    copy_tv(&li->li_tv, rettv);
		وگرنه
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    وگرنه اگر (l != پوچ)
		rettv->vval.v_عدد = idx;
	    وگرنه
	    {
		اگر (type != MATCH_END)
		    rettv->vval.v_عدد =
				      (varnumber_T)(regmatch.startp[0] - str);
		وگرنه
		    rettv->vval.v_عدد =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_عدد += (varnumber_T)(str - عبارت);
	    }
	}
	اگر (l != پوچ)
	    l->lv_lock = prev_lock;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    اگر (type == MATCH_POS && l == پوچ && rettv->vval.v_list != پوچ)
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
    ایستا صحیح
get_matches_in_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    صحیح		idx,
    صحیح		submatches,
    صحیح		matchbuf)
{
    long	len = (long)STRLEN(str);
    صحیح		match = 0;
    colnr_T	startidx = 0;

    برای (;;)
    {
	match = vim_regexec_nl(rmp, str, startidx);
	اگر (!match)
	    بشکن;

	dict_T *d = dict_alloc();
	اگر (d == پوچ)
	    بازگشت FAIL;
	اگر (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    بازگشت FAIL;
	}

	اگر (dict_add_عدد(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    بازگشت FAIL;

	اگر (dict_add_عدد(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    بازگشت FAIL;

	اگر (dict_add_string_len(d, "text", rmp->startp[0],
		    (صحیح)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    بازگشت FAIL;

	اگر (submatches)
	{
	    list_T *sml = list_alloc();
	    اگر (sml == پوچ)
		بازگشت FAIL;

	    اگر (dict_add_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		بازگشت FAIL;
	    }

	    // return a list with the submatches
	    برای (صحیح i = 1; i < NSUBEXP; ++i)
	    {
		اگر (rmp->endp[i] == پوچ)
		{
		    اگر (list_append_string(sml, (char_u *)"", 0) == FAIL)
			بازگشت FAIL;
		}
		وگرنه اگر (list_append_string(sml, rmp->startp[i],
			    (صحیح)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    بازگشت FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	اگر (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    بشکن;
    }

    بازگشت OK;
}

/*
 * "matchbufline()" function
 */
    ایستا تهی
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = پوچ;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_عدد = -1;
    اگر (rettv_list_alloc(rettv) != OK)
	بازگشت;
    retlist = rettv->vval.v_list;

    اگر (check_for_buffer_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	بازگشت;

    صحیح prev_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    اگر (buf == پوچ)
    {
	اگر (did_emsg == prev_did_emsg)
	    semsg(_(e_invalid_buffer_name_str), tv_get_string(&argvars[0]));
	بازگشت;
    }
    اگر (buf->b_ml.ml_mfp == پوچ)
    {
	emsg(_(e_buffer_is_not_loaded));
	بازگشت;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    صحیح		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    اگر (did_emsg > did_emsg_before)
	بازگشت;
    اگر (slnum < 1)
    {
	semsg(_(e_invalid_مقدار_for_argument_str), "lnum");
	بازگشت;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    اگر (did_emsg > did_emsg_before)
	بازگشت;
    اگر (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_مقدار_for_argument_str), "end_lnum");
	بازگشت;
    }

    اگر (elnum > buf->b_ml.ml_سطر_شمار)
	elnum = buf->b_ml.ml_سطر_شمار;

    صحیح		submatches = FALSE;
    اگر (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	اگر (d != پوچ)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    اگر (di != پوچ)
	    {
		اگر (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "submatches");
		    بازگشت;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    اگر (regmatch.regprog == پوچ)
	برو theend;
    regmatch.rm_ic = p_ic;

    وقتی (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	اگر (get_matches_in_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    برو cleanup;
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
    ایستا تهی
f_match(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_MATCH);
}

/*
 * "matchend()" function
 */
    ایستا تهی
f_matchend(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_END);
}

/*
 * "matchlist()" function
 */
    ایستا تهی
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_LIST);
}

/*
 * "matchstr()" function
 */
    ایستا تهی
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_STR);
}

/*
 * "matchstrlist()" function
 */
    ایستا تهی
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = پوچ;
    char_u	*save_cpo;
    list_T	*l = پوچ;
    listitem_T	*li = پوچ;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_عدد = -1;
    اگر (rettv_list_alloc(rettv) != OK)
	بازگشت;
    retlist = rettv->vval.v_list;

    اگر (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	بازگشت;

    اگر ((l = argvars[0].vval.v_list) == پوچ)
	بازگشت;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    اگر (pat == پوچ)
	بازگشت;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    اگر (regmatch.regprog == پوچ)
	برو theend;
    regmatch.rm_ic = p_ic;

    صحیح		submatches = FALSE;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	اگر (d != پوچ)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    اگر (di != پوچ)
	    {
		اگر (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_مقدار_for_argument_str), "submatches");
		    برو cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    صحیح idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_اقلام(l, li)
    {
	اگر (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != پوچ)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    اگر (get_matches_in_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		برو cleanup;
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
    ایستا تهی
f_matchstrpos(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_POS);
}

    ایستا تهی
max_min(typval_T *argvars, typval_T *rettv, صحیح domax)
{
    varnumber_T	n = 0;

    اگر (in_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = 0;

    اگر (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = پوچ;

	l = argvars[0].vval.v_list;
	اگر (l != پوچ && l->lv_len > 0)
	{
	    اگر (l->lv_first == &range_list_item)
	    {
		اگر ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_start;
		وگرنه
		    n = l->lv_u.nonmat.lv_start + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_عدد = n;
	    }
	    وگرنه
	    {
		FOR_ALL_LIST_اقلام(l, li)
		{
		    اگر (tv == پوچ)
			tv = &li->li_tv;
		    وگرنه
		    {
			صحیح res;
			اگر (typval_compare2(&li->li_tv, tv,
				domax ? عبارت_GREATER : عبارت_SMALLER, FALSE, &res) == FAIL)
			    بازگشت;
			اگر (res == OK)
			    tv = &li->li_tv;
		    }
		}

		اگر (tv != پوچ)
		    copy_tv(tv, rettv);
	    }
	}
    }
    وگرنه اگر (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	اگر (tv != پوچ)
	    copy_tv(tv, rettv);
    }
    وگرنه اگر (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	صحیح		todo;
	typval_T	*tv = پوچ;

	d = argvars[0].vval.v_dict;
	اگر (d != پوچ)
	{
	    todo = (صحیح)d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_اقلام(&d->dv_hashtab, hi, todo)
	    {
		اگر (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    اگر (tv == پوچ)
			tv = &HI2DI(hi)->di_tv;
		    وگرنه
		    {
			صحیح res;
			اگر (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? عبارت_GREATER : عبارت_SMALLER, FALSE, &res) == FAIL)
			    بازگشت;
			اگر (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	اگر (tv != پوچ)
	    copy_tv(tv, rettv);
    }
    وگرنه
	semsg(_(e_argument_of_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
}

/*
 * "max()" function
 */
    ایستا تهی
f_max(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, TRUE);
}

/*
 * "min()" function
 */
    ایستا تهی
f_min(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, FALSE);
}

#اگر defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    ایستا تهی
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    تهی
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
    ایستا تهی
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    اگر (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	بازگشت;

    برای (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	اگر (lnum < 0 || lnum > curbuf->b_ml.ml_سطر_شمار)
	{
	    lnum = 0;
	    بشکن;
	}
	اگر (*skipwhite(ml_get(lnum)) != NUL)
	    بشکن;
    }
    rettv->vval.v_عدد = lnum;
}


/*
 * "ngettext()" function
 */
    ایستا تهی
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    نویسه *prev = پوچ;
#endif

    اگر (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_عدد_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	بازگشت;

    rettv->v_type = VAR_STRING;

    اگر (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != پوچ &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ثابت نویسه *)argvars[3].vval.v_string, (نویسه *)p_enc);
#endif

#اگر defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((ثابت نویسه *)argvars[3].vval.v_string, (ثابت نویسه *)argvars[0].vval.v_string, (ثابت نویسه *)argvars[1].vval.v_string, (صحیح)argvars[2].vval.v_عدد));
#وگرنه
	textdomain((ثابت نویسه *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ثابت نویسه *)argvars[0].vval.v_string, (ثابت نویسه *)argvars[1].vval.v_string, argvars[2].vval.v_عدد));
	textdomain(VIMPACKAGE);
#endif

#اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	اگر (prev != پوچ)
	    bind_textdomain_codeset((ثابت نویسه *)argvars[3].vval.v_string, prev);
#endif
    }
    وگرنه
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ثابت نویسه *)argvars[0].vval.v_string, (ثابت نویسه *)argvars[1].vval.v_string, argvars[2].vval.v_عدد));
}


/*
 * "nr2char()" function
 */
    ایستا تهی
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (has_mbyte)
    {
	صحیح	utf8 = 0;

	اگر (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (صحیح)tv_get_bool_chk(&argvars[1], پوچ);
	اگر (utf8)
	    buf[utf_char2bytes((صحیح)tv_get_عدد(&argvars[0]), buf)] = NUL;
	وگرنه
	    buf[(*mb_char2bytes)((صحیح)tv_get_عدد(&argvars[0]), buf)] = NUL;
    }
    وگرنه
    {
	buf[0] = (char_u)tv_get_عدد(&argvars[0]);
	buf[1] = NUL;
    }
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "or(expr, expr)" function
 */
    ایستا تهی
f_or(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    rettv->vval.v_عدد = tv_get_عدد_chk(&argvars[0], پوچ)
					| tv_get_عدد_chk(&argvars[1], پوچ);
}

#ifdef FEAT_PERL
/*
 * "perleval()" function
 */
    ایستا تهی
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#endif

/*
 * "prevnonblank()" function
 */
    ایستا تهی
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    اگر (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	بازگشت;

    lnum = tv_get_lnum(argvars);
    اگر (lnum < 1 || lnum > curbuf->b_ml.ml_سطر_شمار)
	lnum = 0;
    وگرنه
	وقتی (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
	    --lnum;
    rettv->vval.v_عدد = lnum;
}

// This dummy va_list is here because:
// - passing a NULL pointer doesn't work when va_list isn't a pointer
// - locally in the function results in a "used before set" warning
// - using va_start() to initialize it gives "function with fixed args" error
ایستا va_list	ap;

/*
 * "printf()" function
 */
    ایستا تهی
f_printf(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    صحیح		len;
    char_u	*s;
    صحیح		saved_did_emsg = did_emsg;
    نویسه	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script() && check_for_string_or_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (نویسه *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(پوچ, 0, fmt, ap, argvars + 1);
    اگر (!did_emsg)
    {
	s = alloc(len + 1);
	اگر (s != پوچ)
	{
	    rettv->vval.v_string = s;
	    (تهی)vim_vsnprintf_typval((نویسه *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    ایستا تهی
f_pum_getpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;
    pum_set_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    ایستا تهی
f_pumvisible(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    اگر (pum_visible())
	rettv->vval.v_عدد = 1;
}

#ifdef FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    ایستا تهی
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (p_pyx == 0)
	p_pyx = 3;

    اگر (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    وگرنه اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	بازگشت;
    }
    وگرنه
    {
	locals = پوچ;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#endif

#ifdef FEAT_PYTHON
/*
 * "pyeval()" function
 */
    ایستا تهی
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	بازگشت;

    اگر (p_pyx == 0)
	p_pyx = 2;

    اگر (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    وگرنه اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	بازگشت;
    }
    وگرنه
    {
	locals = پوچ;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#endif

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    ایستا تهی
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

# اگر defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    اگر (p_pyx == 2)
	f_pyeval(argvars, rettv);
    وگرنه
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#endif

ایستا UINT32_T srand_seed_for_testing = 0;
ایستا صحیح	srand_seed_for_testing_is_used = FALSE;

    ایستا تهی
f_test_srand_seed(typval_T *argvars, typval_T *rettv UNUSED)
{
    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
	srand_seed_for_testing_is_used = FALSE;
    وگرنه
    {
	srand_seed_for_testing = (UINT32_T)tv_get_عدد(&argvars[0]);
	srand_seed_for_testing_is_used = TRUE;
    }
}

    ایستا تهی
init_srand(UINT32_T *x)
{
    ساختار {
	اجتماع {
	    UINT32_T عدد;
	    char_u   bytes[اندازه(UINT32_T)];
	} contents;
    } buf;

    اگر (srand_seed_for_testing_is_used)
    {
	*x = srand_seed_for_testing;
	بازگشت;
    }

    اگر (mch_get_random(buf.contents.bytes, اندازه(buf.contents.bytes)) == OK)
    {
	*x = buf.contents.عدد;
	بازگشت;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#اگر defined(FEAT_SODIUM)
    اگر (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    وگرنه
#endif
    {
#اگر defined(FEAT_RELTIME)
	proftime_T res;
	profile_start(&res);
# اگر defined(MSWIN)
	*x = (UINT32_T)res.کم_Part;
# وگرنه
	*x = (UINT32_T)res.tv_fsec;
# endif
#وگرنه
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
    ایستا تهی
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = پوچ;
    ایستا UINT32_T	gx, gy, gz, gw;
    ایستا صحیح	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, result;

    اگر (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
    {
	// When no argument is given use the global seed list.
	اگر (initialized == FALSE)
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
    وگرنه اگر (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	اگر (l == پوچ || list_len(l) != 4)
	    برو theend;

	lx = list_find(l, 0L);
	ly = list_find(l, 1L);
	lz = list_find(l, 2L);
	lw = list_find(l, 3L);
	اگر (lx == پوچ || ly == پوچ || lz == پوچ || lw == پوچ)
	    برو theend;
	اگر (lx->li_tv.v_type != VAR_عدد
		|| ly->li_tv.v_type != VAR_عدد
		|| lz->li_tv.v_type != VAR_عدد
		|| lw->li_tv.v_type != VAR_عدد)
	    برو theend;
	x = (UINT32_T)lx->li_tv.vval.v_عدد;
	y = (UINT32_T)ly->li_tv.vval.v_عدد;
	z = (UINT32_T)lz->li_tv.vval.v_عدد;
	w = (UINT32_T)lw->li_tv.vval.v_عدد;

	SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w);

	lx->li_tv.vval.v_عدد = (varnumber_T)x;
	ly->li_tv.vval.v_عدد = (varnumber_T)y;
	lz->li_tv.vval.v_عدد = (varnumber_T)z;
	lw->li_tv.vval.v_عدد = (varnumber_T)w;
    }
    وگرنه
	برو theend;

    rettv->v_type = VAR_عدد;
    rettv->vval.v_عدد = (varnumber_T)result;
    بازگشت;

theend:
    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    rettv->v_type = VAR_عدد;
    rettv->vval.v_عدد = -1;
}

/*
 * "srand()" function
 */
    ایستا تهی
f_srand(typval_T *argvars, typval_T *rettv)
{
    UINT32_T x = 0, z;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    وگرنه
    {
	صحیح	    error = FALSE;

	x = (UINT32_T)tv_get_عدد_chk(&argvars[0], &error);
	اگر (error)
	    بازگشت;
    }

    list_append_عدد(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_عدد(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_عدد(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_عدد(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#undef ROTL
#undef SPLITMIX32
#undef SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    ایستا تهی
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	start;
    varnumber_T	end;
    varnumber_T	stride = 1;
    صحیح		error = FALSE;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 2) == FAIL)))
	بازگشت;

    start = tv_get_عدد_chk(&argvars[0], &error);
    اگر (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = start - 1;
	start = 0;
    }
    وگرنه
    {
	end = tv_get_عدد_chk(&argvars[1], &error);
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_عدد_chk(&argvars[2], &error);
    }

    اگر (error)
	بازگشت;		// type error; errmsg already given
    اگر (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	بازگشت;
    }

    // The stride is stored in "lv_stride", which is an int.
    اگر (stride < INT_MIN || stride > INT_MAX)
    {
	نویسه	buf[NUMBUFLEN];

	vim_snprintf(buf, اندازه(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	بازگشت;
    }

    uvarnumber_T	len;

    اگر (stride > 0 ? end < start : end > start)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	back = stride > 0
				  ? (uvarnumber_T)start - (uvarnumber_T)end
				  : (uvarnumber_T)end - (uvarnumber_T)start;

	اگر (back > 1)
	{
	    emsg(_(e_start_past_end));
	    بازگشت;
	}
	len = 0;
    }
    وگرنه
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	span = stride > 0
				  ? (uvarnumber_T)end - (uvarnumber_T)start
				  : (uvarnumber_T)start - (uvarnumber_T)end;
	uvarnumber_T	شمار = span / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	اگر (شمار >= (uvarnumber_T)INT_MAX)
	{
	    نویسه	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, اندازه(buf), "%llu",
			      شمار < UVARNUM_MAX ? شمار + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    بازگشت;
	}
	len = شمار + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_first = &range_list_item;
    list->lv_u.nonmat.lv_start = start;
    list->lv_u.nonmat.lv_end = end;
    list->lv_u.nonmat.lv_stride = (صحیح)stride;
    list->lv_len = (صحیح)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    تهی
range_list_materialize(list_T *list)
{
    varnumber_T start = list->lv_u.nonmat.lv_start;
    varnumber_T end = list->lv_u.nonmat.lv_end;
    صحیح		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_first = پوچ;
    list->lv_u.mat.lv_last = پوچ;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_item = پوچ;
    برای (i = start; stride > 0 ? i <= end : i >= end; i += stride)
    {
	اگر (list_append_عدد(list, i) == FAIL)
	    بشکن;
	اگر (list->lv_lock & VAR_اقلام_LOCKED)
	    list->lv_u.mat.lv_last->li_tv.v_lock = VAR_LOCKED;
    }
    list->lv_lock &= ~VAR_اقلام_LOCKED;
}

/*
 * "getreginfo()" function
 */
    ایستا تهی
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    صحیح		regname;
    char_u	buf[NUMBUFLEN + 2];
    size_t	buflen;
    long	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    اگر (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	بازگشت;

    regname = getreg_get_regname(argvars);
    اگر (regname == 0)
	بازگشت;

    اگر (regname == '@')
	regname = '"';

    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_عبارت_SRC | GREG_LIST);
    اگر (list == پوچ)
	بازگشت;
    اگر (dict_add_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    انتخاب (get_reg_type(regname, &reglen))
    {
	حالت MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    بشکن;
	حالت MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    بشکن;
	حالت MBLOCK:
	    buflen = vim_snprintf_safelen((نویسه *)buf, اندازه(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    بشکن;
	پیشفرض:
	    buf[0] = NUL;
	    buflen = 0;
	    بشکن;
    }
    (تهی)dict_add_string_len(dict, (نویسه *)"regtype", buf, (صحیح)buflen);

    buf[0] = get_register_name(get_unname_register());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    اگر (regname == '"')
	(تهی)dict_add_string_len(dict, (نویسه *)"points_to", buf, (صحیح)buflen);
    وگرنه
    {
	dictitem_T	*item = dictitem_alloc((char_u *)"isunnamed");

	اگر (item != پوچ)
	{
	    item->di_tv.v_type = VAR_BOOL;
	    item->di_tv.vval.v_عدد = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (تهی)dict_add(dict, item);
	}
    }
}

    ایستا تهی
return_register(صحیح regname, typval_T *rettv)
{
    char_u buf[2] = {NUL, NUL};

    buf[0] = (char_u)regname;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(buf, (buf[0] == NUL) ? 0 : 1);
}

/*
 * "reg_executing()" function
 */
    ایستا تهی
f_reg_executing(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    ایستا تهی
f_reg_recording(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    ایستا تهی
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_عدد = -1;
    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    rettv->vval.v_عدد = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    ایستا تهی
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    صحیح slen = list_len(l);

    اگر (rettv_list_alloc(rettv) == FAIL
	    || l == پوچ
	    || n <= 0
	    || slen == 0)
	بازگشت;

    اگر (check_repeat_شمار(slen, n) == FAIL)
	بازگشت;

    وقتی (n-- > 0)
	اگر (list_extend(rettv->vval.v_list, l, پوچ) == FAIL)
	    بشکن;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    ایستا تهی
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    صحیح		slen;
    صحیح		len;
    صحیح		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    اگر (rettv_blob_alloc(rettv) == FAIL
	    || blob == پوچ
	    || n <= 0)
	بازگشت;

    slen = blob->bv_ga.ga_len;
    اگر (slen <= 0)
	بازگشت;
    اگر (check_repeat_شمار(slen, n) == FAIL)
	بازگشت;
    len = slen * (صحیح)n;

    اگر (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	بازگشت;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    برای (i = 0; i < slen; ++i)
	اگر (blob_get(blob, i) != 0)
	    بشکن;

    اگر (i == slen)
	// No need to copy since all bytes are already zero
	بازگشت;

    برای (i = 0; i < (صحیح)n; ++i)
	blob_set_range(rettv->vval.v_blob,
		(long)i * slen, ((long)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    ایستا تهی
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    size_t	slen;
    صحیح		len;
    char_u	*r;
    صحیح		done;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    slen = STRLEN(p);
    اگر (slen == 0 || n <= 0)
	بازگشت;
    اگر (check_repeat_شمار((varnumber_T)slen, n) == FAIL)
	بازگشت;
    len = (صحیح)slen * (صحیح)n;

    r = alloc((size_t)len + 1);
    اگر (r == پوچ)
	بازگشت;

    mch_memmove(r, p, slen);
    done = (صحیح)slen;
    وقتی (done < len)
    {
	صحیح copy_len = done;

	اگر (copy_len > len - done)
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
    ایستا تهی
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    اگر (in_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    n = tv_get_عدد(&argvars[1]);
    اگر (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    وگرنه
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
    ایستا صحیح
get_جستجو_arg(typval_T *varp, صحیح *flagsp)
{
    صحیح		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    صحیح		mask;

    اگر (varp->v_type == VAR_UNKNOWN)
	بازگشت FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    اگر (flags == پوچ)
	بازگشت 0;		// type error; errmsg already given
    وقتی (*flags != NUL)
    {
	انتخاب (*flags)
	{
	    حالت 'b': dir = BACKWARD; بشکن;
	    حالت 'w': p_ws = TRUE; بشکن;
	    حالت 'W': p_ws = FALSE; بشکن;
	    پیشفرض:  mask = 0;
		      اگر (flagsp != پوچ)
			  انتخاب (*flags)
			  {
			      حالت 'c': mask = SP_START; بشکن;
			      حالت 'e': mask = SP_END; بشکن;
			      حالت 'm': mask = SP_RETCOUNT; بشکن;
			      حالت 'n': mask = SP_NOMOVE; بشکن;
			      حالت 'p': mask = SP_SUBPAT; بشکن;
			      حالت 'r': mask = SP_REPEAT; بشکن;
			      حالت 's': mask = SP_SETPCMARK; بشکن;
			      حالت 'z': mask = SP_COLUMN; بشکن;
			  }
		      اگر (mask == 0)
		      {
			  semsg(_(e_invalid_argument_str), flags);
			  dir = 0;
		      }
		      وگرنه
			  *flagsp |= mask;
	}
	اگر (dir == 0)
	    بشکن;
	++flags;
    }
    بازگشت dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    ایستا صحیح
جستجو_cmn(typval_T *argvars, pos_T *match_pos, صحیح *flagsp)
{
    صحیح		flags;
    char_u	*pat;
    size_t	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    صحیح		save_p_ws = p_ws;
    صحیح		dir;
    صحیح		retval = 0;	// default: FAIL
    long	lnum_stop = 0;
#ifdef FEAT_RELTIME
    long	time_limit = 0;
#endif
    صحیح		options = جستجو_KEEP;
    صحیح		subpatnum;
    searchit_arg_T sia;
    صحیح		use_skip = FALSE;
    pos_T	firstpos;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_عدد_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_عدد_arg(argvars, 3) == FAIL)))))
	برو theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_جستجو_arg(&argvars[1], flagsp);	// may set p_ws
    اگر (dir == 0)
	برو theend;
    flags = *flagsp;
    اگر (flags & SP_START)
	options |= جستجو_START;
    اگر (flags & SP_END)
	options |= جستجو_END;
    اگر (flags & SP_COLUMN)
	options |= جستجو_COL;

    // Optional arguments: line number to stop searching, timeout and skip.
    اگر (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (long)tv_get_عدد_chk(&argvars[2], پوچ);
	اگر (lnum_stop < 0)
	    برو theend;
	اگر (argvars[3].v_type != VAR_UNKNOWN)
	{
#ifdef FEAT_RELTIME
	    time_limit = (long)tv_get_عدد_chk(&argvars[3], پوچ);
	    اگر (time_limit < 0)
		برو theend;
#endif
	    use_skip = eval_عبارت_valid_arg(&argvars[4]);
	}
    }

    /*
     * This function does not accept SP_REPEAT and SP_RETCOUNT flags.
     * Check to make sure only those flags are set.
     * Also, Only the SP_NOMOVE or the SP_SETPCMARK flag can be set. Both
     * flags cannot be set. Check for that condition also.
     */
    اگر (((flags & (SP_REPEAT | SP_RETCOUNT)) != 0)
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	برو theend;
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
    برای (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, پوچ, dir, pat, patlen, 1L,
						     options, RE_جستجو, &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	اگر (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	اگر (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    بشکن;
	اگر (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    صحیح	    do_skip;
	    صحیح	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_عبارت_to_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    اگر (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		بشکن;
	    }
	    اگر (!do_skip)
		بشکن;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~جستجو_START;
    }

    اگر (subpatnum != FAIL)
    {
	اگر (flags & SP_SUBPAT)
	    retval = subpatnum;
	وگرنه
	    retval = pos.lnum;
	اگر (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	اگر (match_pos != پوچ)
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
    اگر (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    وگرنه
	curwin->w_set_curswant = true;
theend:
    p_ws = save_p_ws;

    بازگشت retval;
}

#ifdef FEAT_RUBY
/*
 * "rubyeval()" function
 */
    ایستا تهی
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#endif

/*
 * "screenattr()" function
 */
    ایستا تهی
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    صحیح		row;
    صحیح		col;
    صحیح		c;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    row = (صحیح)tv_get_عدد_chk(&argvars[0], پوچ) - 1;
    col = (صحیح)tv_get_عدد_chk(&argvars[1], پوچ) - 1;
    اگر (row < 0 || row >= screen_Rows
	    || col < 0 || col >= screen_Columns)
	c = -1;
    وگرنه
	c = ScreenAttrs[سطر_Offset[row] + col];
    rettv->vval.v_عدد = c;
}

/*
 * "screenchar()" function
 */
    ایستا تهی
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    صحیح		row;
    صحیح		col;
    صحیح		c;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    row = (صحیح)tv_get_عدد_chk(&argvars[0], پوچ) - 1;
    col = (صحیح)tv_get_عدد_chk(&argvars[1], پوچ) - 1;
    اگر (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	c = -1;
    وگرنه
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(row, col, buf, پوچ);
	c = (*mb_ptr2char)(buf);
    }
    rettv->vval.v_عدد = c;
}

/*
 * "screenchars()" function
 */
    ایستا تهی
f_screenchars(typval_T *argvars, typval_T *rettv)
{
    صحیح		row;
    صحیح		col;
    صحیح		c;
    صحیح		i;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    row = (صحیح)tv_get_عدد_chk(&argvars[0], پوچ) - 1;
    col = (صحیح)tv_get_عدد_chk(&argvars[1], پوچ) - 1;
    اگر (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	بازگشت;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(row, col, buf, پوچ);
    صحیح pcc[MAX_MCO];
    اگر (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    وگرنه
	c = (*mb_ptr2char)(buf);
    list_append_عدد(rettv->vval.v_list, (varnumber_T)c);

    اگر (enc_utf8)
	برای (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_عدد(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    ایستا تهی
f_screencol(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_عدد = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    ایستا تهی
f_screenrow(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_عدد = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    ایستا تهی
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    صحیح		row;
    صحیح		col;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = پوچ;
    rettv->v_type = VAR_STRING;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    row = (صحیح)tv_get_عدد_chk(&argvars[0], پوچ) - 1;
    col = (صحیح)tv_get_عدد_chk(&argvars[1], پوچ) - 1;
    اگر (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	بازگشت;

    screen_getbytes(row, col, buf, پوچ);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    ایستا تهی
f_جستجو(typval_T *argvars, typval_T *rettv)
{
    صحیح		flags = 0;

    rettv->vval.v_عدد = جستجو_cmn(argvars, پوچ, &flags);
}

/*
 * "searchdecl()" function
 */
    ایستا تهی
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    صحیح		locally = TRUE;
    صحیح		thisblock = FALSE;
    صحیح		error = FALSE;
    char_u	*name;

    rettv->vval.v_عدد = 1;	// default: FAIL

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	بازگشت;

    name = tv_get_string_chk(&argvars[0]);
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(صحیح)tv_get_bool_chk(&argvars[1], &error);
	اگر (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (صحیح)tv_get_bool_chk(&argvars[2], &error);
    }
    اگر (!error && name != پوچ)
	rettv->vval.v_عدد = find_decl(name, (صحیح)STRLEN(name),
				     locally, thisblock, جستجو_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    ایستا صحیح
searchpair_cmn(typval_T *argvars, pos_T *match_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    صحیح		save_p_ws = p_ws;
    صحیح		dir;
    صحیح		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    صحیح		retval = 0;		// default: FAIL
    long	lnum_stop = 0;
    long	time_limit = 0;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_عدد_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_عدد_arg(argvars, 6) == FAIL)))))
	برو theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    اگر (spat == پوچ || mpat == پوچ || epat == پوچ)
	برو theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_جستجو_arg(&argvars[3], &flags); // may set p_ws
    اگر (dir == 0)
	برو theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    اگر ((flags & (SP_END | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[3]));
	برو theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    اگر (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    اگر (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = پوچ;
    وگرنه
    {
	// Type is checked later.
	skip = &argvars[4];

	اگر (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (long)tv_get_عدد_chk(&argvars[5], پوچ);
	    اگر (lnum_stop < 0)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[5]));
		برو theend;
	    }
#ifdef FEAT_RELTIME
	    اگر (argvars[6].v_type != VAR_UNKNOWN)
	    {
		time_limit = (long)tv_get_عدد_chk(&argvars[6], پوچ);
		اگر (time_limit < 0)
		{
		    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[6]));
		    برو theend;
		}
	    }
#endif
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    match_pos, lnum_stop, time_limit);

theend:
    p_ws = save_p_ws;

    بازگشت retval;
}

/*
 * "searchpair()" function
 */
    ایستا تهی
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = searchpair_cmn(argvars, پوچ);
}

/*
 * "searchpairpos()" function
 */
    ایستا تهی
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    صحیح		lnum = 0;
    صحیح		col = 0;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (searchpair_cmn(argvars, &match_pos) > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_عدد(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_عدد(rettv->vval.v_list, (varnumber_T)col);
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
    صحیح		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    صحیح		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*match_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    long	time_limit UNUSED) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = پوچ, *pat3 = پوچ;
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
    صحیح		n;
    صحیح		r;
    صحیح		nest = 1;
    صحیح		use_skip = FALSE;
    صحیح		err;
    صحیح		options = جستجو_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    اگر (pat2 == پوچ)
	برو theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    اگر (pat3 == پوچ)
	برو theend;
    pat2len = vim_snprintf((نویسه *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    اگر (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    وگرنه
	pat3len = vim_snprintf((نویسه *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    اگر (flags & SP_START)
	options |= جستجو_START;

    اگر (skip != پوچ)
	use_skip = eval_عبارت_valid_arg(skip);

#ifdef FEAT_RELTIME
    اگر (time_limit > 0)
	init_regexp_timeout(time_limit);
#endif
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    برای (;;)
    {
	searchit_arg_T sia;

	CLEAR_FIELD(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, پوچ, dir, pat, patlen, 1L,
						     options, RE_جستجو, &sia);
	اگر (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    بشکن;

	اگر (firstpos.lnum == 0)
	    firstpos = pos;
	اگر (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    اگر (dir == BACKWARD)
		decl(&pos);
	    وگرنه
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~جستجو_START;

	// If the skip pattern matches, ignore this match.
	اگر (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_عبارت_to_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    اگر (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		بشکن;
	    }
	    اگر (r)
		ادامه;
	}

	اگر ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	وگرنه
	{
	    // Found end when searching forward or start when searching
	    // backward: end of (nested) pair; or found middle in outer pair.
	    اگر (--nest == 1)
		pat = pat3;	// outer level, search for middle
	}

	اگر (nest == 0)
	{
	    // Found the match: return matchcount or line number.
	    اگر (flags & SP_RETCOUNT)
		++retval;
	    وگرنه
		retval = pos.lnum;
	    اگر (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    اگر (!(flags & SP_REPEAT))
		بشکن;
	    nest = 1;	    // search for next unmatched
	}
    }

    اگر (match_pos != پوچ)
    {
	// Store the match cursor position
	match_pos->lnum = curwin->w_cursor.lnum;
	match_pos->col = curwin->w_cursor.col + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    اگر ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#ifdef FEAT_RELTIME
    اگر (time_limit > 0)
	disable_regexp_timeout();
#endif
    vim_free(pat2);
    vim_free(pat3);
    اگر (p_cpo == empty_option)
	p_cpo = save_cpo;
    وگرنه
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	اگر (*p_cpo == NUL)
	    set_option_مقدار_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    بازگشت retval;
}

/*
 * "searchpos()" function
 */
    ایستا تهی
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    صحیح		lnum = 0;
    صحیح		col = 0;
    صحیح		n;
    صحیح		flags = 0;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    n = جستجو_cmn(argvars, &match_pos, &flags);
    اگر (n > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_عدد(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_عدد(rettv->vval.v_list, (varnumber_T)col);
    اگر (flags & SP_SUBPAT)
	list_append_عدد(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ایستا تهی
set_position(typval_T *argvars, typval_T *rettv, صحیح charpos)
{
    pos_T	pos;
    صحیح		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_عدد = -1;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	بازگشت;

    name = tv_get_string_chk(argvars);
    اگر (name == پوچ)
	بازگشت;

    اگر (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	بازگشت;

    اگر (pos.col != MAXCOL && --pos.col < 0)
	pos.col = 0;
    اگر ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	اگر (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_set_curswant = false;
	}
	check_cursor();
	rettv->vval.v_عدد = 0;
    }
    وگرنه اگر (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	اگر (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_عدد = 0;
    }
    وگرنه
	emsg(_(e_invalid_argument));
}
/*
 * "setcharpos()" function
 */
    ایستا تهی
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, TRUE);
}

    ایستا تهی
f_setcharsearch(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    اگر (check_for_dict_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر ((d = argvars[0].vval.v_dict) == پوچ)
	بازگشت;

    csearch = dict_get_string(d, "char", FALSE);
    اگر (csearch != پوچ)
    {
	اگر (enc_utf8)
	{
	    صحیح pcc[MAX_MCO];
	    صحیح c = utfc_ptr2char(csearch, pcc);

	    set_last_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	وگرنه
	    set_last_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_find(d, (char_u *)"forward", -1);
    اگر (di != پوچ)
	set_csearch_direction((صحیح)tv_get_عدد(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_find(d, (char_u *)"until", -1);
    اگر (di != پوچ)
	set_csearch_until(!!tv_get_عدد(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    ایستا تهی
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    ایستا تهی
f_setenv(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    اگر (check_restricted() || check_secure())
	بازگشت;

    name = tv_get_string_buf(&argvars[0], namebuf);
    اگر (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_عدد == VVAL_NULL)
	vim_unsetenv_ext(name);
    وگرنه
	vim_setenv_ext(name, tv_get_string_buf(&argvars[1], valbuf));
}

/*
 * "setfperm({fname}, {mode})" function
 */
    ایستا تهی
f_setfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	modebuf[NUMBUFLEN];
    char_u	*mode_str;
    صحیح		i;
    صحیح		mask;
    صحیح		mode = 0;

    rettv->vval.v_عدد = 0;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    fname = tv_get_string_chk(&argvars[0]);
    اگر (fname == پوچ)
	بازگشت;
    mode_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    اگر (mode_str == پوچ)
	بازگشت;
    اگر (STRLEN(mode_str) != 9)
    {
	semsg(_(e_invalid_argument_str), mode_str);
	بازگشت;
    }

    mask = 1;
    برای (i = 8; i >= 0; --i)
    {
	اگر (mode_str[i] != '-')
	    mode |= mask;
	mask = mask << 1;
    }
    rettv->vval.v_عدد = mch_setperm(fname, mode) == OK;
}

/*
 * "setpos()" function
 */
    ایستا تهی
f_setpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    صحیح
get_yank_type(char_u **pp, char_u *yank_type, long *block_len)
{
    char_u *stropt = *pp;
    انتخاب (*stropt)
    {
	حالت 'v': حالت 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    بشکن;
	حالت 'V': حالت 'l':	// line-wise selection
	    *yank_type = MLINE;
	    بشکن;
	حالت 'b': حالت Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    اگر (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*block_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    بشکن;
	پیشفرض:
	    بازگشت FAIL;
    }
    *pp = stropt;
    بازگشت OK;
}

/*
 * "setreg()" function
 */
    ایستا تهی
f_setreg(typval_T *argvars, typval_T *rettv)
{
    صحیح		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    صحیح		append;
    char_u	yank_type;
    long	block_len;
    typval_T	*regcontents;
    صحیح		pointreg;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	بازگشت;

    pointreg = 0;
    regcontents = پوچ;
    block_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_عدد = 1;		// FAIL is default

    اگر (strregname == پوچ)
	بازگشت;		// type error; errmsg already given
    اگر (in_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_register_name_must_be_one_char_str), strregname);
	بازگشت;
    }
    regname = *strregname;
    اگر (regname == 0 || regname == '@')
	regname = '"';

    اگر (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	اگر (d == پوچ || d->dv_hashtab.ht_used == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {پوچ, پوچ};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    بازگشت;
	}

	di = dict_find(d, (char_u *)"regcontents", -1);
	اگر (di != پوچ)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	اگر (stropt != پوچ)
	{
	    صحیح ret = get_yank_type(&stropt, &yank_type, &block_len);

	    اگر (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_مقدار_for_argument_str), "value");
		بازگشت;
	    }
	}

	اگر (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    اگر (stropt != پوچ)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	وگرنه اگر (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    وگرنه
	regcontents = &argvars[1];

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	اگر (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    بازگشت;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	اگر (stropt == پوچ)
	    بازگشت;		// type error
	برای (; *stropt != NUL; ++stropt)
	    انتخاب (*stropt)
	    {
		حالت 'a': حالت 'A':	// append
		    append = TRUE;
		    بشکن;
		پیشفرض:
		    get_yank_type(&stropt, &yank_type, &block_len);
	    }
    }

    اگر (regcontents && regcontents->v_type == VAR_LIST)
    {
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	list_T		*ll = regcontents->vval.v_list;
	listitem_T	*li;
	صحیح		len;

	// If the list is NULL handle like an empty list.
	len = ll == پوچ ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	اگر (lstval == پوچ)
	    بازگشت;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	اگر (ll != پوچ)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_اقلام(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		اگر (strval == پوچ)
		    برو free_lstval;
		اگر (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    اگر (strval == پوچ)
			برو free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = پوچ;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, block_len);
free_lstval:
	وقتی (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    وگرنه اگر (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	اگر (strval == پوچ)
	    بازگشت;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, block_len);
    }
    اگر (pointreg != 0)
	get_yank_register(pointreg, TRUE);

    rettv->vval.v_عدد = 0;
}

/*
 * "settagstack()" function
 */
    ایستا تهی
f_settagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    dict_T	*d;
    صحیح		action = 'r';

    rettv->vval.v_عدد = -1;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	بازگشت;

    // first argument: window number or id
    wp = find_win_by_nr_or_id(&argvars[0]);
    اگر (wp == پوچ)
	بازگشت;

    // second argument: dict with items to set in the tag stack
    اگر (check_for_dict_arg(argvars, 1) == FAIL)
	بازگشت;
    d = argvars[1].vval.v_dict;
    اگر (d == پوچ)
	بازگشت;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    اگر (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    وگرنه اگر (check_for_string_arg(argvars, 2) == FAIL)
	بازگشت;
    وگرنه
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	اگر (actstr == پوچ)
	    بازگشت;
	اگر ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	وگرنه
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    بازگشت;
	}
    }

    اگر (set_tagstack(wp, d, action) == OK)
	rettv->vval.v_عدد = 0;
}

#ifdef FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    ایستا تهی
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحیح		len;

    اگر (in_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != پوچ ? (char_u *)blob->bv_ga.ga_data : (char_u *)"";
	len = blob != پوچ ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_bytes(p, len, پوچ, 0));
    }
    وگرنه
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_bytes(p, (صحیح)STRLEN(p), پوچ, 0));
    }
}
#endif // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    ایستا تهی
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    صحیح do_special;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	بازگشت;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    ایستا تهی
f_shiftwidth(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_عدد = 0;

    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	long	col;

	col = (long)tv_get_عدد_chk(argvars, پوچ);
	اگر (col < 0)
	    بازگشت;	// type error; errmsg already given
#ifdef FEAT_VARTABS
	rettv->vval.v_عدد = get_sw_مقدار_col(curbuf, col, FALSE);
	بازگشت;
#endif
    }

    rettv->vval.v_عدد = get_sw_مقدار(curbuf);
}

/*
 * "soundfold({word})" function
 */
    ایستا تهی
f_soundfold(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#ifdef FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#وگرنه
    rettv->vval.v_string = vim_strsave(s);
#endif
}

/*
 * "spellbadword()" function
 */
    ایستا تهی
f_spellbadword(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*word = (char_u *)"";
    hlf_T	attr = HLF_شمار;
    صحیح		len = 0;
#ifdef FEAT_SPELL
    صحیح		wo_spell_save = curwin->w_p_spell;

    اگر (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    اگر (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	بازگشت;
    }
#endif

    اگر (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	بازگشت;
    }

#ifdef FEAT_SPELL
    اگر (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_to(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	اگر (len != 0)
	{
	    word = ml_get_cursor();
	    curwin->w_set_curswant = true;
	}
    }
    وگرنه اگر (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	صحیح	capcol = -1;

	اگر (str != پوچ)
	{
	    // Check the argument for spelling.
	    وقتی (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		اگر (attr != HLF_شمار)
		{
		    word = str;
		    بشکن;
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
    انتخاب (attr)
    {
    حالت HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	بشکن;

    حالت HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	بشکن;

    حالت HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	بشکن;

    حالت HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	بشکن;

    پیشفرض:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	بشکن;
    }
}

/*
 * "spellsuggest()" function
 */
    ایستا تهی
f_spellsuggest(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SPELL
    char_u	*str;
    صحیح		typeerr = FALSE;
    صحیح		maxcount;
    garray_T	ga;
    صحیح		i;
    listitem_T	*li;
    صحیح		need_capital = FALSE;
    صحیح		wo_spell_save = curwin->w_p_spell;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	بازگشت;

    اگر (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    اگر (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	بازگشت;
    }
#endif

    اگر (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	بازگشت;
    }

#ifdef FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (صحیح)tv_get_عدد_chk(&argvars[1], &typeerr);
	اگر (maxcount <= 0)
	    بازگشت;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (صحیح)tv_get_bool_chk(&argvars[2], &typeerr);
	    اگر (typeerr)
		بازگشت;
	}
    }
    وگرنه
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    برای (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_data)[i];

	li = listitem_alloc();
	اگر (li == پوچ)
	    vim_free(str);
	وگرنه
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

    ایستا تهی
f_split(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*end;
    char_u	*pat = پوچ;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    صحیح		match;
    colnr_T	col = 0;
    صحیح		keepempty = FALSE;
    صحیح		typeerr = FALSE;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	بازگشت;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    str = tv_get_string(&argvars[0]);
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	اگر (pat == پوچ)
	    typeerr = TRUE;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (صحیح)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    اگر (pat == پوچ || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    اگر (rettv_list_alloc(rettv) == FAIL)
	برو theend;
    اگر (typeerr)
	برو theend;

    regmatch.regprog = eval_regcomp(pat);
    اگر (regmatch.regprog != پوچ)
    {
	regmatch.rm_ic = FALSE;
	وقتی (*str != NUL || keepempty)
	{
	    اگر (*str == NUL)
		match = FALSE;	// empty item at the end
	    وگرنه
		match = vim_regexec_nl(&regmatch, str, col);
	    اگر (match)
		end = regmatch.startp[0];
	    وگرنه
		end = str + STRLEN(str);
	    اگر (keepempty || end > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && match && end < regmatch.endp[0]))
	    {
		اگر (list_append_string(rettv->vval.v_list, str,
						    (صحیح)(end - str)) == FAIL)
		    بشکن;
	    }
	    اگر (!match)
		بشکن;
	    // Advance to just after the match.
	    اگر (regmatch.endp[0] > str)
		col = 0;
	    وگرنه
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
    ایستا تهی
f_submatch(typval_T *argvars, typval_T *rettv)
{
    صحیح		error = FALSE;
    صحیح		no;
    صحیح		retList = 0;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	بازگشت;

    no = (صحیح)tv_get_عدد_chk(&argvars[0], &error);
    اگر (error)
	بازگشت;
    اگر (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_عدد_nr), no);
	بازگشت;
    }
    اگر (argvars[1].v_type != VAR_UNKNOWN)
	retList = (صحیح)tv_get_bool_chk(&argvars[1], &error);
    اگر (error)
	بازگشت;

    اگر (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    وگرنه
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = reg_submatch_list(no);
    }
}

/*
 * "substitute()" function
 */
    ایستا تهی
f_substitute(typval_T *argvars, typval_T *rettv)
{
    char_u	patbuf[NUMBUFLEN];
    char_u	subbuf[NUMBUFLEN];
    char_u	flagsbuf[NUMBUFLEN];
    char_u	*str;
    char_u	*pat;
    char_u	*sub = پوچ;
    typval_T	*عبارت = پوچ;
    char_u	*flg;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	بازگشت;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    اگر (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	عبارت = &argvars[2];
    وگرنه
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    اگر (str == پوچ || pat == پوچ || (sub == پوچ && عبارت == پوچ)
								|| flg == پوچ)
	rettv->vval.v_string = پوچ;
    وگرنه
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, عبارت, flg, پوچ);
}

/*
 * "swapfilelist()" function
 */
    ایستا تهی
f_swapfilelist(typval_T *argvars UNUSED, typval_T *rettv)
{
    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;
    recover_names(پوچ, FALSE, rettv->vval.v_list, 0, پوچ);
}

/*
 * "swapinfo(swap_filename)" function
 */
    ایستا تهی
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (rettv_dict_alloc(rettv) == OK)
	get_b0_dict(tv_get_string(argvars), rettv->vval.v_dict);
}

/*
 * "swapname(expr)" function
 */
    ایستا تهی
f_swapname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    اگر (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	بازگشت;

    buf = tv_get_buf(&argvars[0], FALSE);
    اگر (buf == پوچ || buf->b_ml.ml_mfp == پوچ
					|| buf->b_ml.ml_mfp->mf_fname == پوچ)
	rettv->vval.v_string = پوچ;
    وگرنه
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    ایستا تهی
f_synID(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحیح		id = 0;
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    صحیح		trans;
    صحیح		transerr = FALSE;

    اگر (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	بازگشت;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (linenr_T)tv_get_عدد(&argvars[1]) - 1;	// -1 on type error
    trans = (صحیح)tv_get_bool_chk(&argvars[2], &transerr);

    اگر (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_شمار
	    && col >= 0 && col < (long)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, col, trans, پوچ, FALSE);
#endif

    rettv->vval.v_عدد = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    ایستا تهی
f_synIDattr(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*p = پوچ;
#ifdef FEAT_SYN_HL
    صحیح		id;
    char_u	*what;
    char_u	*mode;
    char_u	modebuf[NUMBUFLEN];
    صحیح		modec;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	بازگشت;

    id = (صحیح)tv_get_عدد(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	mode = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(mode[0]);
	اگر (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    وگرنه
    {
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	اگر (USE_24BIT)
	    modec = 'g';
	وگرنه
# endif
	    اگر (t_colors > 1)
		modec = 'c';
	    وگرنه
		modec = 't';
    }

    انتخاب (TOLOWER_ASC(what[0]))
    {
	حالت 'b':
		اگر (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		وگرنه					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		بشکن;

	حالت 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		بشکن;

	حالت 'i':
		اگر (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		وگرنه					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		بشکن;

	حالت 'n':
		اگر (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		وگرنه					// name
		    p = get_highlight_name_ext(پوچ, id - 1, FALSE);
		بشکن;

	حالت 'r':					// reverse
		p = highlight_has_attr(id, HL_INVERSE, modec);
		بشکن;

	حالت 's':
		اگر (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		وگرنه اگر (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		وگرنه					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		بشکن;

	حالت 'u':
		اگر (STRLEN(what) >= 9)
		{
		    اگر (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    وگرنه اگر (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    وگرنه اگر (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    وگرنه اگر (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    وگرنه
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		وگرنه
							// ul
		    p = highlight_color(id, what, modec);
		بشکن;
    }

    اگر (p != پوچ)
	p = vim_strsave(p);
#endif
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    ایستا تهی
f_synIDtrans(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحیح		id;

#ifdef FEAT_SYN_HL
    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    id = (صحیح)tv_get_عدد(&argvars[0]);

    اگر (id > 0)
	id = syn_get_final_id(id);
    وگرنه
#endif
	id = 0;

    rettv->vval.v_عدد = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    ایستا تهی
f_synconcealed(typval_T *argvars UNUSED, typval_T *rettv)
{
#اگر defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	col;
    صحیح		syntax_flags = 0;
    صحیح		cchar;
    صحیح		matchid = 0;
    char_u	str[NUMBUFLEN];
#endif

    rettv_list_set(rettv, پوچ);

    اگر (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

#اگر defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_عدد(&argvars[1]) - 1;	// -1 on type error

    CLEAR_FIELD(str);

    اگر (rettv_list_alloc(rettv) == OK)
    {
	اگر (lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_شمار
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (تهی)syn_get_id(curwin, lnum, col, FALSE, پوچ, FALSE);
	    syntax_flags = get_syntax_info(&matchid);

	    // get the conceal character
	    اگر ((syntax_flags & HL_CONCEAL) && curwin->w_p_cole < 3)
	    {
		cchar = syn_get_sub_char();
		اگر (cchar == NUL && curwin->w_p_cole == 1)
		    cchar = (curwin->w_lcs_chars.conceal == NUL) ? ' '
					: curwin->w_lcs_chars.conceal;
		اگر (cchar != NUL)
		{
		    اگر (has_mbyte)
			(*mb_char2bytes)(cchar, str);
		    وگرنه
			str[0] = cchar;
		}
	    }
	}

	list_append_عدد(rettv->vval.v_list,
					    (syntax_flags & HL_CONCEAL) != 0);
	// -1 to auto-determine strlen
	list_append_string(rettv->vval.v_list, str, -1);
	list_append_عدد(rettv->vval.v_list, matchid);
    }
#endif
}

/*
 * "synstack(lnum, col)" function
 */
    ایستا تهی
f_synstack(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    صحیح		i;
    صحیح		id;
#endif

    rettv_list_set(rettv, پوچ);

    اگر (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

#ifdef FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_عدد(&argvars[1]) - 1;	// -1 on type error

    اگر (lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_شمار
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(تهی)syn_get_id(curwin, lnum, col, FALSE, پوچ, TRUE);
	برای (i = 0; ; ++i)
	{
	    id = syn_get_پشته_item(i);
	    اگر (id < 0)
		بشکن;
	    اگر (list_append_عدد(rettv->vval.v_list, id) == FAIL)
		بشکن;
	}
    }
#endif
}

/*
 * "tabpagebuflist()" function
 */
    ایستا تهی
f_tabpagebuflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp = پوچ;

    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    وگرنه
    {
	tp = find_tabpage((صحیح)tv_get_عدد(&argvars[0]));
	اگر (tp != پوچ)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    اگر (wp != پوچ && rettv_list_alloc(rettv) == OK)
    {
	برای (; wp != پوچ; wp = wp->w_next)
	    اگر (list_append_عدد(rettv->vval.v_list,
						wp->w_buffer->b_fnum) == FAIL)
		بشکن;
    }
}

/*
 * "tagfiles()" function
 */
    ایستا تهی
f_tagfiles(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    صحیح		first;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;
    fname = alloc(MAXPATHL);
    اگر (fname == پوچ)
	بازگشت;

    برای (first = TRUE; ; first = FALSE)
	اگر (get_tagfname(&tn, first, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    بشکن;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    ایستا تهی
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = پوچ;
    char_u  *tag_pattern;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	بازگشت;

    tag_pattern = tv_get_string(&argvars[0]);

    rettv->vval.v_عدد = FALSE;
    اگر (*tag_pattern == NUL)
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    اگر (rettv_list_alloc(rettv) == OK)
	(تهی)get_tags(rettv->vval.v_list, tag_pattern, fname);
}

/*
 * "type(expr)" function
 */
    ایستا تهی
f_type(typval_T *argvars, typval_T *rettv)
{
    صحیح n = -1;

    انتخاب (argvars[0].v_type)
    {
	حالت VAR_عدد:  n = VAR_TYPE_عدد; بشکن;
	حالت VAR_STRING:  n = VAR_TYPE_STRING; بشکن;
	حالت VAR_PARTIAL:
	حالت VAR_FUNC:    n = VAR_TYPE_FUNC; بشکن;
	حالت VAR_LIST:    n = VAR_TYPE_LIST; بشکن;
	حالت VAR_TUPLE:   n = VAR_TYPE_TUPLE; بشکن;
	حالت VAR_DICT:    n = VAR_TYPE_DICT; بشکن;
	حالت VAR_FLOAT:   n = VAR_TYPE_FLOAT; بشکن;
	حالت VAR_BOOL:	  n = VAR_TYPE_BOOL; بشکن;
	حالت VAR_SPECIAL: n = VAR_TYPE_NONE; بشکن;
	حالت VAR_JOB:     n = VAR_TYPE_JOB; بشکن;
	حالت VAR_CHANNEL: n = VAR_TYPE_CHANNEL; بشکن;
	حالت VAR_BLOB:    n = VAR_TYPE_BLOB; بشکن;
	حالت VAR_INSTR:   n = VAR_TYPE_INSTR; بشکن;
	حالت VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; بشکن;
	حالت VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		اگر (cl != پوچ && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		وگرنه
		    n = VAR_TYPE_CLASS;
		بشکن;
	    }
	حالت VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		اگر (obj == پوچ)
		    n = VAR_TYPE_OBJECT;
		وگرنه
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    اگر (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    وگرنه
			n = VAR_TYPE_OBJECT;
		}
		بشکن;
	    }
	حالت VAR_VOID:
	    emsg(_(e_cannot_use_void_مقدار));
	    بشکن;
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	    internal_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    بشکن;
    }
    rettv->vval.v_عدد = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    ایستا تهی
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_start = 0;
    colnr_T	vcol_end = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    صحیح		winchanged = FALSE;
    صحیح		len;

    اگر (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_عدد_arg(argvars, 2) == FAIL))))
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((صحیح)tv_get_عدد(&argvars[2]), &tp);
	اگر (wp == پوچ || tp == پوچ)
	    برو theend;

	اگر (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    برو theend;

	check_cursor();
	winchanged = TRUE;
    }

    صحیح fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    اگر (fp != پوچ && fp->lnum <= curbuf->b_ml.ml_سطر_شمار
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	اگر (fp->col < 0)
	    fp->col = 0;
	وگرنه
	{
	    len = (صحیح)ml_get_len(fp->lnum);
	    اگر (fp->col > len)
		fp->col = len;
	}
	getvvcol(curwin, fp, &vcol_start, پوچ, &vcol_end, 0);
	++vcol_start;
	++vcol_end;
    }

theend:
    اگر (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	اگر (rettv_list_alloc(rettv) == OK)
	{
	    list_append_عدد(rettv->vval.v_list, vcol_start);
	    list_append_عدد(rettv->vval.v_list, vcol_end);
	}
	وگرنه
	    rettv->vval.v_عدد = 0;
    }
    وگرنه
	rettv->vval.v_عدد = vcol_end;

    اگر (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    ایستا تهی
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    اگر (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_mode_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    اگر (non_zero_arg(&argvars[0]))
	curbuf->b_visual_mode_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    ایستا تهی
f_wildmenumode(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    اگر (wild_menu_showing || ((State & MODE_CMDLINE) && cmdline_pum_active()))
	rettv->vval.v_عدد = 1;
}

/*
 * "windowsversion()" function
 */
    ایستا تهی
f_windowsversion(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windowsVersion);
}

/*
 * "wordcount()" function
 */
    ایستا تهی
f_wordcount(typval_T *argvars UNUSED, typval_T *rettv)
{
    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    ایستا تهی
f_xor(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    rettv->vval.v_عدد = tv_get_عدد_chk(&argvars[0], پوچ)
					^ tv_get_عدد_chk(&argvars[1], پوچ);
}

#endif // FEAT_EVAL
