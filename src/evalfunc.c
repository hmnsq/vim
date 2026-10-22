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
#تعریف_کړه USING_FLOAT_STUFF

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

#که_تعریف_شوی VMS
# include <float.h>
#د_که_پای

ساكن فراغ f_and(typval_T *argvars, typval_T *rettv);
#که_تعریف_شوی FEAT_BEVAL
ساكن فراغ f_balloon_gettext(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_balloon_show(typval_T *argvars, typval_T *rettv);
# إذا defined(FEAT_BEVAL_غړی)
ساكن فراغ f_balloon_وېشل(typval_T *argvars, typval_T *rettv);
# endif
#د_که_پای
ساكن فراغ f_base64_encode(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_base64_decode(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_bindtextdomain(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_byte2line(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_call(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_changenr(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_char2nr(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_charcol(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_ستون(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_confirm(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_copy(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_cursor(typval_T *argsvars, typval_T *rettv);
#که_تعریف_شوی MSWIN
ساكن فراغ f_debugbreak(typval_T *argvars, typval_T *rettv);
#د_که_پای
ساكن فراغ f_deepcopy(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_did_filetype(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_echoraw(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_تشه(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_environ(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_err_teapot(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_escape(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_eval(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_eventhandler(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_execute(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_exists_compiled(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_expand(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_expandcmd(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_feedkeys(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_fnameescape(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_foreground(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_funcref(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_function(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_garbagecollect(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_get(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getbgcolor(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getcellpixels(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getchangelist(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getcharpos(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getcharsearch(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getcurpos(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getcursorcharpos(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getenv(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getfontname(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getinfo(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getjumplist(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getpid(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getpos(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getreg(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getreginfo(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getregion(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getregionpos(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_getregtype(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_gettagstack(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_gettext(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_haslocaldir(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_hlID(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_hlexists(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_hostname(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_id(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_شاخص(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_indexof(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_ننوت(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_inputdialog(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_inputlist(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_inputrestore(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_inputsave(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_inputsecret(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_مداخله(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_invert(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_islocked(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_توکي(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_keytrans(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_وروستی_بفر_nr(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_libcall(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_libcallnr(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_کرښه(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_line2byte(typval_T *argvars, typval_T *rettv);
#که_تعریف_شوی FEAT_LUA
ساكن فراغ f_luaeval(typval_T *argvars, typval_T *rettv);
#د_که_پای
ساكن فراغ f_سمون(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_matchbufline(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_matchend(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_matchlist(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_matchstr(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_matchstrlist(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_matchstrpos(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_max(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_min(typval_T *argvars, typval_T *rettv);
#که_تعریف_شوی FEAT_MZSCHEME
ساكن فراغ f_mzeval(typval_T *argvars, typval_T *rettv);
#د_که_پای
ساكن فراغ f_nextnonblank(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_ngettext(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_nr2char(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_or(typval_T *argvars, typval_T *rettv);
#که_تعریف_شوی FEAT_PERL
ساكن فراغ f_perleval(typval_T *argvars, typval_T *rettv);
#د_که_پای
ساكن فراغ f_prevnonblank(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_printf(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_pum_getpos(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_pumvisible(typval_T *argvars, typval_T *rettv);
#که_تعریف_شوی FEAT_PYTHON3
ساكن فراغ f_py3eval(typval_T *argvars, typval_T *rettv);
#د_که_پای
#که_تعریف_شوی FEAT_PYTHON
ساكن فراغ f_pyeval(typval_T *argvars, typval_T *rettv);
#د_که_پای
#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
ساكن فراغ f_pyxeval(typval_T *argvars, typval_T *rettv);
#د_که_پای
ساكن فراغ f_test_srand_تخم(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_rand(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_range(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_reg_executing(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_reg_recording(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_rename(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_repeat(typval_T *argvars, typval_T *rettv);
#که_تعریف_شوی FEAT_RUBY
ساكن فراغ f_rubyeval(typval_T *argvars, typval_T *rettv);
#د_که_پای
ساكن فراغ f_screenattr(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_screenchar(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_screenchars(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_screencol(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_screenrow(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_screenstring(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_پلټنه(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_searchdecl(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_searchpair(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_searchpairpos(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_searchpos(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_setcharpos(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_setcharsearch(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_setcursorcharpos(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_setenv(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_setfperm(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_setpos(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_setreg(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_settagstack(typval_T *argvars, typval_T *rettv);
#که_تعریف_شوی FEAT_CRYPT
ساكن فراغ f_sha256(typval_T *argvars, typval_T *rettv);
#د_که_پای
ساكن فراغ f_shellescape(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_shiftwidth(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_soundfold(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_spellbadword(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_spellsuggest(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_وېشل(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_srand(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_submatch(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_substitute(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_swapfilelist(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_swapinfo(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_swapname(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_synID(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_synIDattr(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_synIDtrans(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_synstack(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_synconcealed(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_tabpagebuflist(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_taglist(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_tagfiles(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_type(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_virtcol(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_visualmode(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_wildmenumode(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_windowsversion(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_wordcount(typval_T *argvars, typval_T *rettv);
ساكن فراغ f_xor(typval_T *argvars, typval_T *rettv);


/*
 * Functions that check the argument type of a builtin function.
 * Each function returns FAIL and gives an error message if the type is wrong.
 */

// Context passed to an arg_ function.
تعريفنوع بنية {
    صحيح		arg_شمېر;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    صحيح		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
تعريفنوع صحيح (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    ساكن صحيح
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    أرجع need_type(actual, expected, 0,
	    context->arg_idx - context->arg_شمېر, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    ساكن صحيح
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    إذا (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_شمېر, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	أرجع FAIL;
    أرجع arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    صحيح
arg_type_modifiable(type_T *type, صحيح arg_idx)
{
    حرف *tofree;

    إذا ((type->tt_flags & TTFLAG_CONST) == 0)
	أرجع OK;
    semsg(_(e_دلیل_nr_trying_منزل_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    أرجع FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    ساكن صحيح
arg_any(type_T *type ناکارول_شوی,
	type_T *decl_type ناکارول_شوی,
	argcontext_T *context ناکارول_شوی)
{
    أرجع OK;
}

/*
 * Check "type" is a float or a number.
 */
    ساكن صحيح
arg_float_or_nr(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_شمېره
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_شمېره, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a number.
 */
    ساكن صحيح
arg_شمېره(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    أرجع check_arg_type(&t_شمېره, type, context);
}

/*
 * Check "type" is an object.
 */
    ساكن صحيح
arg_object(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    ساكن صحيح
arg_dict_any(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    أرجع check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    ساكن صحيح
arg_list_any(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    أرجع check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    ساكن صحيح
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type ناکارول_شوی,
	argcontext_T *context)
{
    أرجع check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    ساكن صحيح
arg_list_شمېره(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    أرجع check_arg_type(&t_list_شمېره, type, context);
}

/*
 * Check "type" is a list of strings.
 */
    ساكن صحيح
arg_list_string(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    أرجع check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    ساكن صحيح
arg_tuple_any(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    أرجع check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    ساكن صحيح
arg_string(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    أرجع check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    ساكن صحيح
arg_blob(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    أرجع check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    ساكن صحيح
arg_bool(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    أرجع check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    ساكن صحيح
arg_bool_or_nr(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_شمېره
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_شمېره, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    ساكن صحيح
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	أرجع OK;
    أرجع check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    ساكن صحيح
arg_list_or_blob(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    ساكن صحيح
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    إذا (arg_list_or_blob(type, decl_type, context) == FAIL)
	أرجع FAIL;
    أرجع arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    ساكن صحيح
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    أرجع FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    ساكن صحيح
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a string or a number
 */
    ساكن صحيح
arg_string_or_nr(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_شمېره
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    ساكن صحيح
arg_بفر(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_شمېره
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    ساكن صحيح
arg_بفر_or_dict_any(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_شمېره
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    ساكن صحيح
arg_lnum(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_شمېره
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    ساكن صحيح
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	أرجع OK;
    إذا (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	أرجع FAIL;
    }
    إذا (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	أرجع OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    ساكن صحيح
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    ساكن صحيح
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    ساكن صحيح
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    ساكن صحيح
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    ساكن صحيح
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type ناکارول_شوی,
	argcontext_T *context)
{
    إذا (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    ساكن صحيح
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    إذا (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	أرجع FAIL;
    أرجع arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    ساكن صحيح
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    ساكن صحيح
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    إذا (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	أرجع FAIL;
    أرجع arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    ساكن صحيح
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	أرجع OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_دلیل_nr),
						 context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    ساكن صحيح
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = عدم;
    type_T *(دلیلونه[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, عدم, عدم, دلیلونه};

    إذا (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	إذا (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	وإلا
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    وإلا إذا (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    وإلا إذا (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_شمېره;

    دلیلونه[0] = عدم;
    دلیلونه[1] = &t_unknown;
    إذا (type->tt_argcount != -1)
    {
	إذا (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_شمېره_د_arguments));
	    أرجع FAIL;
	}
	إذا (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	وإلا
	{
	    إذا (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		دلیلونه[0] = &t_شمېره;
	    وإلا إذا (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		دلیلونه[0] = &t_string;
	    إذا (دلیلونه[0] != عدم)
		دلیلونه[1] = expected_member;
	}
    }

    إذا (!type_any_or_unknown(type->tt_member) || دلیلونه[0] != عدم)
    {
	where_T where = WHERE_INIT;

	إذا (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == عدم
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	وإلا إذا (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	وإلا // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	إذا (دلیلونه[0] == عدم)
	    دلیلونه[0] = &t_unknown;
	إذا (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_شاخص = 2;
	where.wt_ډول = WT_دلیل;
	أرجع check_type(&t_func_exp, type, TRUE, where);
    }
    أرجع OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    ساكن صحيح
arg_filter_func(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	أرجع OK;

    إذا (type->tt_type == VAR_FUNC)
	أرجع check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_دلیل_nr), 2);
    أرجع FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    ساكن صحيح
arg_map_func(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	أرجع OK;

    إذا (type->tt_type == VAR_FUNC)
	أرجع check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_دلیل_nr), 2);
    أرجع FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    ساكن صحيح
arg_foreach_func(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	أرجع OK;

    إذا (type->tt_type == VAR_FUNC)
	أرجع check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_دلیل_nr), 2);
    أرجع FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    ساكن صحيح
arg_ترتیبول_how(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	أرجع OK;

    إذا (type->tt_type == VAR_FUNC)
    {
	type_T *(دلیلونه[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_شمېره, عدم, دلیلونه};

	إذا (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    دلیلونه[0] = context->arg_types[0].type_curr->tt_member;
	وإلا
	    دلیلونه[0] = &t_unknown;
	إذا (!type_any_or_unknown(type->tt_member) || دلیلونه[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    دلیلونه[1] = دلیلونه[0];
	    إذا (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_شاخص = 2;
	    where.wt_ډول = WT_دلیل;
	    أرجع check_type(&t_func_exp, type, TRUE, where);
	}

	أرجع OK;
    }
    semsg(_(e_string_or_function_required_for_دلیل_nr), 2);
    أرجع FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    ساكن صحيح
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_شمېره
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    ساكن صحيح
varargs_class(type_T *type ناکارول_شوی,
	      type_T *decl_type ناکارول_شوی,
	      argcontext_T *context)
{
    لكل (صحيح i = context->arg_idx; i < context->arg_شمېر; ++i)
    {
	type2_T *types = &context->arg_types[i];
	إذا (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_دلیل_nr), i + 1);
	    أرجع FAIL;
	}
    }
    أرجع OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    ساكن صحيح
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    ساكن صحيح
arg_سرچپه(type_T *type, type_T *decl_type, argcontext_T *context)
{
    إذا (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	أرجع FAIL;
    أرجع arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    ساكن صحيح
arg_دنده(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    أرجع check_arg_type(&t_دنده, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    ساكن صحيح
arg_chan_or_دنده(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_دنده
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    ساكن صحيح
arg_same_as_prev(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_decl;

    أرجع check_arg_type(prev_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    ساكن صحيح
arg_same_struct_as_prev(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;

    إذا (prev_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	أرجع check_arg_type(prev_type, type, context);
    أرجع OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    ساكن صحيح
arg_توکی_د_prev(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    إذا (prev_type->tt_type == VAR_LIST)
	expected = prev_type->tt_member;
    وإلا إذا (prev_type->tt_type == VAR_BLOB)
	expected = &t_شمېره;
    وإلا
	// probably VAR_ANY, can't check
	أرجع OK;

    أرجع check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    ساكن صحيح
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_شمېره
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    ساكن صحيح
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	أرجع OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    ساكن صحيح
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *لومړی_type = context->arg_types[context->arg_idx - 2].type_curr;

    إذا (لومړی_type->tt_type == VAR_LIST || لومړی_type->tt_type == VAR_BLOB)
	أرجع arg_شمېره(type, decl_type, context);
    إذا (لومړی_type->tt_type == VAR_DICT)
	أرجع arg_string(type, decl_type, context);
    أرجع OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    ساكن صحيح
arg_get1(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	أرجع OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    ساكن صحيح
arg_len1(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_شمېره
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	أرجع OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    ساكن صحيح
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *لومړی_type = context->arg_types[context->arg_idx - 1].type_curr;

    إذا (لومړی_type->tt_type == VAR_LIST || لومړی_type->tt_type == VAR_BLOB)
	أرجع arg_شمېره(type, decl_type, context);
    إذا (لومړی_type->tt_type == VAR_DICT)
	أرجع arg_string_or_nr(type, decl_type, context);
    أرجع OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    ساكن صحيح
arg_repeat1(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_شمېره
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	أرجع OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    ساكن صحيح
arg_slice1(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	أرجع OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    ساكن صحيح
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type ناکارول_شوی,
    argcontext_T	*context)
{
    إذا (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	أرجع OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_دلیل_nr),
							 context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    ساكن صحيح
arg_cursor1(type_T *type, type_T *decl_type ناکارول_شوی, argcontext_T *context)
{
    إذا (type->tt_type == VAR_شمېره
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	أرجع OK;

    arg_type_mismatch(&t_شمېره, type, context->arg_idx + 1);
    أرجع FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
ساكن argcheck_T arg1_blob[] = {arg_blob};
ساكن argcheck_T arg1_bool[] = {arg_bool};
ساكن argcheck_T arg1_بفر[] = {arg_بفر};
ساكن argcheck_T arg1_بفر_or_dict_any[] = {arg_بفر_or_dict_any};
ساكن argcheck_T arg1_chan_or_دنده[] = {arg_chan_or_دنده};
ساكن argcheck_T arg1_dict_any[] = {arg_dict_any};
ساكن argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
ساكن argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
ساكن argcheck_T arg1_دنده[] = {arg_دنده};
ساكن argcheck_T arg1_list_any[] = {arg_list_any};
ساكن argcheck_T arg1_tuple_any[] = {arg_tuple_any};
ساكن argcheck_T arg1_list_شمېره[] = {arg_list_شمېره};
ساكن argcheck_T arg1_سرچپه[] = {arg_سرچپه};
ساكن argcheck_T arg1_list_or_tuple_or_dict[] = {arg_list_or_tuple_or_dict};
ساكن argcheck_T arg1_list_string[] = {arg_list_string};
ساكن argcheck_T arg1_list_tuple_dict_blob_or_string[] = {arg_list_tuple_dict_blob_or_string};
ساكن argcheck_T arg1_lnum[] = {arg_lnum};
ساكن argcheck_T arg1_شمېره[] = {arg_شمېره};
ساكن argcheck_T arg1_string[] = {arg_string};
ساكن argcheck_T arg1_string_or_list_any[] = {arg_string_or_list_any};
ساكن argcheck_T arg1_string_or_list_string[] = {arg_string_or_list_string};
ساكن argcheck_T arg1_string_or_nr[] = {arg_string_or_nr};
ساكن argcheck_T arg1_string_or_blob[] = {arg_string_or_blob};
ساكن argcheck_T arg2_بفر_any[] = {arg_بفر, arg_any};
ساكن argcheck_T arg2_بفر_bool[] = {arg_بفر, arg_bool};
ساكن argcheck_T arg2_بفر_list_any[] = {arg_بفر, arg_list_any};
ساكن argcheck_T arg2_بفر_lnum[] = {arg_بفر, arg_lnum};
ساكن argcheck_T arg2_بفر_شمېره[] = {arg_بفر, arg_شمېره};
ساكن argcheck_T arg2_بفر_string[] = {arg_بفر, arg_string};
ساكن argcheck_T arg2_chan_or_دنده_dict[] = {arg_chan_or_دنده, arg_dict_any};
ساكن argcheck_T arg2_chan_or_دنده_string[] = {arg_chan_or_دنده, arg_string};
ساكن argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
ساكن argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
ساكن argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
ساكن argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
ساكن argcheck_T arg2_دنده_dict[] = {arg_دنده, arg_dict_any};
ساكن argcheck_T arg2_دنده_string_or_شمېره[] = {arg_دنده, arg_string_or_nr};
ساكن argcheck_T arg2_list_any_شمېره[] = {arg_list_any, arg_شمېره};
ساكن argcheck_T arg2_list_شمېره[] = {arg_list_شمېره, arg_list_شمېره};
ساكن argcheck_T arg2_list_شمېره_bool[] = {arg_list_شمېره, arg_bool};
ساكن argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
ساكن argcheck_T arg2_listblobmod_توکی[] = {arg_list_or_blob_mod, arg_توکی_د_prev};
ساكن argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
ساكن argcheck_T arg2_lnum_شمېره[] = {arg_lnum, arg_شمېره};
ساكن argcheck_T arg2_شمېره[] = {arg_شمېره, arg_شمېره};
ساكن argcheck_T arg2_شمېره_any[] = {arg_شمېره, arg_any};
ساكن argcheck_T arg2_شمېره_bool[] = {arg_شمېره, arg_bool};
ساكن argcheck_T arg2_شمېره_dict_any[] = {arg_شمېره, arg_dict_any};
ساكن argcheck_T arg2_شمېره_list[] = {arg_شمېره, arg_list_any};
ساكن argcheck_T arg2_شمېره_string[] = {arg_شمېره, arg_string};
ساكن argcheck_T arg2_شمېره_بفر[] = {arg_شمېره, arg_بفر};
ساكن argcheck_T arg2_شمېره_string_or_list[] = {arg_شمېره, arg_string_or_list_any};
ساكن argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
ساكن argcheck_T arg2_string[] = {arg_string, arg_string};
ساكن argcheck_T arg2_string_any[] = {arg_string, arg_any};
ساكن argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
ساكن argcheck_T arg2_string_chan_or_دنده[] = {arg_string, arg_chan_or_دنده};
ساكن argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
ساكن argcheck_T arg2_string_list_شمېره[] = {arg_string, arg_list_شمېره};
ساكن argcheck_T arg2_string_شمېره[] = {arg_string, arg_شمېره};
ساكن argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
ساكن argcheck_T arg2_string_or_list_شمېره[] = {arg_string_or_list_any, arg_شمېره};
ساكن argcheck_T arg2_string_string_or_شمېره[] = {arg_string, arg_string_or_nr};
ساكن argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
ساكن argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
ساكن argcheck_T arg3_any_بفر_bool_or_dict[] = {
			      arg_any, arg_بفر, arg_bool_or_dict_any};
ساكن argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
ساكن argcheck_T arg3_بفر_lnum_lnum[] = {arg_بفر, arg_lnum, arg_lnum};
ساكن argcheck_T arg3_بفر_شمېره_شمېره[] = {arg_بفر, arg_شمېره, arg_شمېره};
ساكن argcheck_T arg3_بفر_string_any[] = {arg_بفر, arg_string, arg_any};
ساكن argcheck_T arg3_بفر_string_dict[] = {arg_بفر, arg_string, arg_dict_any};
ساكن argcheck_T arg3_dict_شمېره_شمېره[] = {arg_dict_any, arg_شمېره, arg_شمېره};
ساكن argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
ساكن argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
ساكن argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
ساكن argcheck_T arg3_lnum_شمېره_bool[] = {arg_lnum, arg_شمېره, arg_bool};
ساكن argcheck_T arg3_شمېره[] = {arg_شمېره, arg_شمېره, arg_شمېره};
ساكن argcheck_T arg3_شمېره_any_dict[] = {arg_شمېره, arg_any, arg_dict_any};
ساكن argcheck_T arg3_شمېره_شمېره_dict[] = {arg_شمېره, arg_شمېره, arg_dict_any};
ساكن argcheck_T arg3_شمېره_string_any[] = {arg_شمېره, arg_string, arg_any};
ساكن argcheck_T arg3_شمېره_string_بفر[] = {arg_شمېره, arg_string, arg_بفر};
ساكن argcheck_T arg3_شمېره_string_string[] = {arg_شمېره, arg_string, arg_string};
ساكن argcheck_T arg3_string[] = {arg_string, arg_string, arg_string};
ساكن argcheck_T arg3_string_any_dict[] = {arg_string, arg_any, arg_dict_any};
ساكن argcheck_T arg3_string_any_string[] = {arg_string, arg_any, arg_string};
ساكن argcheck_T arg3_string_bool_bool[] = {arg_string, arg_bool, arg_bool};
ساكن argcheck_T arg3_string_شمېره_bool[] = {arg_string, arg_شمېره, arg_bool};
ساكن argcheck_T arg3_string_شمېره_شمېره[] = {arg_string, arg_شمېره, arg_شمېره};
ساكن argcheck_T arg3_string_or_dict_bool_dict[] = {arg_string_or_dict_any, arg_bool, arg_dict_any};
ساكن argcheck_T arg3_string_or_list_bool_شمېره[] = {arg_string_or_list_any, arg_bool, arg_شمېره};
ساكن argcheck_T arg3_string_string_bool[] = {arg_string, arg_string, arg_bool};
ساكن argcheck_T arg3_string_string_dict[] = {arg_string, arg_string, arg_dict_any};
ساكن argcheck_T arg3_string_string_شمېره[] = {arg_string, arg_string, arg_شمېره};
ساكن argcheck_T arg4_شمېره_شمېره_string_any[] = {arg_شمېره, arg_شمېره, arg_string, arg_any};
ساكن argcheck_T arg4_string_string_any_string[] = {arg_string, arg_string, arg_any, arg_string};
ساكن argcheck_T arg4_string_string_شمېره_string[] = {arg_string, arg_string, arg_شمېره, arg_string};
ساكن argcheck_T arg4_string_شمېره_bool_bool[] = {arg_string, arg_شمېره, arg_bool, arg_bool};
/* Function specific argument types (not covered by the above) */
ساكن argcheck_T arg15_assert_fails[] = {arg_string_or_nr, arg_string_or_list_any, arg_any, arg_شمېره, arg_string};
ساكن argcheck_T arg34_assert_inrange[] = {arg_float_or_nr, arg_float_or_nr, arg_float_or_nr, arg_string};
ساكن argcheck_T arg4_browse[] = {arg_bool, arg_string, arg_string, arg_string};
ساكن argcheck_T arg23_chanexpr[] = {arg_chan_or_دنده, arg_any, arg_dict_any};
ساكن argcheck_T arg23_chanraw[] = {arg_chan_or_دنده, arg_string_or_blob, arg_dict_any};
ساكن argcheck_T arg24_شمېر[] = {arg_string_list_tuple_or_dict, arg_any, arg_bool, arg_شمېره};
ساكن argcheck_T arg13_cursor[] = {arg_cursor1, arg_شمېره, arg_شمېره};
ساكن argcheck_T arg12_deepcopy[] = {arg_any, arg_bool};
ساكن argcheck_T arg12_execute[] = {arg_string_or_list_string, arg_string};
ساكن argcheck_T arg12_getchar[] = {arg_bool_or_nr, arg_dict_any};
ساكن argcheck_T arg23_extend[] = {arg_list_or_dict_or_blob_mod, arg_same_as_prev, arg_extend3};
ساكن argcheck_T arg23_extendnew[] = {arg_list_or_dict_or_blob, arg_same_struct_as_prev, arg_extend3};
ساكن argcheck_T arg23_get[] = {arg_get1, arg_string_or_nr, arg_any};
ساكن argcheck_T arg14_glob[] = {arg_string, arg_bool, arg_bool, arg_bool};
ساكن argcheck_T arg25_globpath[] = {arg_string, arg_string, arg_bool, arg_bool, arg_bool};
ساكن argcheck_T arg24_شاخص[] = {arg_list_or_tuple_or_blob, arg_توکی_د_prev, arg_شمېره, arg_bool};
ساكن argcheck_T arg23_شاخص[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
ساكن argcheck_T arg23_ورننباسه[] = {arg_list_or_blob, arg_توکی_د_prev, arg_شمېره};
ساكن argcheck_T arg1_len[] = {arg_len1};
ساكن argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
ساكن argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
ساكن argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
ساكن argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
ساكن argcheck_T arg2_instanceof[] = {arg_object, varargs_class, عدم };
ساكن argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
ساكن argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
ساكن argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_شمېره, arg_شمېره, arg_dict_any};
ساكن argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_شمېره, arg_شمېره, arg_dict_any};
ساكن argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
ساكن argcheck_T arg45_matchbufline[] = {arg_بفر, arg_string, arg_lnum, arg_lnum, arg_dict_any};
ساكن argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
ساكن argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
ساكن argcheck_T arg24_remote_بیان[] = {arg_string, arg_string, arg_string, arg_شمېره};
ساكن argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_شمېره};
ساكن argcheck_T arg2_repeat[] = {arg_repeat1, arg_شمېره};
ساكن argcheck_T arg15_پلټنه[] = {arg_string, arg_string, arg_شمېره, arg_شمېره, arg_string_or_func};
ساكن argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_شمېره, arg_شمېره};
ساكن argcheck_T arg3_setbufline[] = {arg_بفر, arg_lnum, arg_str_or_nr_or_list};
ساكن argcheck_T arg2_setline[] = {arg_lnum, arg_any};
ساكن argcheck_T arg24_setloclist[] = {arg_شمېره, arg_list_any, arg_string, arg_dict_any};
ساكن argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
ساكن argcheck_T arg23_settagstack[] = {arg_شمېره, arg_dict_any, arg_string};
ساكن argcheck_T arg02_sign_getplaced[] = {arg_بفر, arg_dict_any};
ساكن argcheck_T arg45_sign_ځای_پرځای_کړه[] = {arg_شمېره, arg_string, arg_string, arg_بفر, arg_dict_any};
ساكن argcheck_T arg23_ټوټه[] = {arg_slice1, arg_شمېره, arg_شمېره};
ساكن argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_ترتیبول_how, arg_dict_any};
ساكن argcheck_T arg24_strpart[] = {arg_string, arg_شمېره, arg_شمېره, arg_bool};
ساكن argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
ساكن argcheck_T arg23_win_execute[] = {arg_شمېره, arg_string_or_list_string, arg_string};
ساكن argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
ساكن argcheck_T arg24_سمون_func[] = {arg_string_or_list_any, arg_string, arg_شمېره, arg_شمېره};

// Can be used by functions called through "f_retfunc" to create new types.
ساكن garray_T *current_type_gap = عدم;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    ساكن type_T *
ret_void(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_void;
}
    ساكن type_T *
ret_any(صحيح	argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_any;
}
    ساكن type_T *
ret_bool(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_bool;
}
    ساكن type_T *
ret_شمېره_bool(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_شمېره_bool;
}
    ساكن type_T *
ret_شمېره(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_شمېره;
}
    ساكن type_T *
ret_float(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_float;
}
    ساكن type_T *
ret_string(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_string;
}
    ساكن type_T *
ret_list_any(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_list_any;
}
    ساكن type_T *
ret_list_شمېره(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    أرجع &t_list_شمېره;
}
    ساكن type_T *
ret_list_string(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    أرجع &t_list_string;
}
    ساكن type_T *
ret_list_dict_any(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    أرجع &t_list_dict_any;
}
    ساكن type_T *
ret_list_توکي(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    أرجع &t_list_list_any;
}
    ساكن type_T *
ret_list_list_any(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    أرجع &t_list_list_any;
}
    ساكن type_T *
ret_list_string_توکي(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    أرجع &t_list_list_string;
}
    ساكن type_T *
ret_list_regionpos(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    أرجع &t_list_list_list_شمېره;
}
    ساكن type_T *
ret_tuple_any(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_tuple_any;
}
    ساكن type_T *
ret_dict_any(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_dict_any;
}
    ساكن type_T *
ret_دنده_info(صحيح argcount,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type)
{
    إذا (argcount == 0)
    {
	*decl_type = &t_list_any;
	أرجع &t_list_دنده;
    }
    أرجع &t_dict_any;
}
    ساكن type_T *
ret_dict_شمېره(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_dict_شمېره;
}
    ساكن type_T *
ret_dict_string(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_dict_string;
}
    ساكن type_T *
ret_blob(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_blob;
}
    ساكن type_T *
ret_func_any(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_func_any;
}
    ساكن type_T *
ret_func_unknown(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_func_unknown;
}
    ساكن type_T *
ret_channel(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_channel;
}
    ساكن type_T *
ret_دنده(صحيح argcount ناکارول_شوی,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    أرجع &t_دنده;
}
    ساكن type_T *
ret_لومړی_arg(صحيح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    إذا (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	أرجع argtypes[0].type_curr;
    }
    أرجع &t_void;
}
    ساكن type_T *
ret_ټوټه(صحيح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    إذا (argcount > 0)
    {
	إذا (argtypes[0].type_decl != عدم)
	{
	    بدل (argtypes[0].type_decl->tt_type)
	    {
		حالة VAR_STRING: *decl_type = &t_string; اكسر;
		حالة VAR_BLOB: *decl_type = &t_blob; اكسر;
		حالة VAR_LIST: *decl_type = &t_list_any; اكسر;
		حالة VAR_TUPLE: *decl_type = &t_tuple_any; اكسر;
		افتراضي: اكسر;
	    }
	}
	أرجع argtypes[0].type_curr;
    }
    أرجع &t_void;
}
    ساكن type_T *
ret_copy(صحيح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    إذا (argcount > 0)
    {
	إذا (argtypes[0].type_decl != عدم)
	{
	    إذا (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    وإلا إذا (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    وإلا
		*decl_type = argtypes[0].type_decl;
	}
	إذا (argtypes[0].type_curr != عدم)
	{
	    إذا (argtypes[0].type_curr->tt_type == VAR_LIST)
		أرجع &t_list_any;
	    وإلا إذا (argtypes[0].type_curr->tt_type == VAR_DICT)
		أرجع &t_dict_any;
	}
	أرجع argtypes[0].type_curr;
    }
    أرجع &t_void;
}
    ساكن type_T *
ret_extend(صحيح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    إذا (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	// if the second argument has a different current type then the current
	// type is "any"
	إذا (argcount > 1 && !equal_type(argtypes[0].type_curr,
						     argtypes[1].type_curr, 0))
	{
	    إذا (argtypes[0].type_curr->tt_type == VAR_LIST)
		أرجع &t_list_any;
	    إذا (argtypes[0].type_curr->tt_type == VAR_DICT)
		أرجع &t_dict_any;
	    إذا (argtypes[0].type_curr->tt_type == VAR_BLOB)
		أرجع &t_blob;
	}
	أرجع argtypes[0].type_curr;
    }
    أرجع &t_void;
}
    ساكن type_T *
ret_repeat(صحيح argcount,
	type2_T *argtypes,
	type_T	**decl_type ناکارول_شوی)
{
    إذا (argcount == 0)
	أرجع &t_any;
    إذا (argtypes[0].type_curr == &t_شمېره)
	أرجع &t_string;
    أرجع argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    ساكن type_T *
ret_لومړی_cont(صحيح argcount,
	type2_T *argtypes,
	type_T	**decl_type ناکارول_شوی)
{
    إذا (argcount > 0)
    {
	إذا (argtypes[0].type_curr->tt_type == VAR_LIST)
	    أرجع &t_list_any;
	إذا (argtypes[0].type_curr->tt_type == VAR_DICT)
	    أرجع &t_dict_any;
	إذا (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    أرجع argtypes[0].type_curr;
    }
    أرجع &t_any;
}
// for get(): the type of the item when a missing item has the same type
    ساكن type_T *
ret_get(صحيح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    إذا (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*توکی = عدم;

	إذا (t->tt_type == VAR_BLOB)
	    توکی = &t_شمېره;
	وإلا إذا (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    توکی = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	إذا (توکی != عدم && (argcount == 3
			? equal_type(توکی, argtypes[2].type_curr, 0)
			: توکی->tt_type == VAR_شمېره))
	{
	    إذا (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    أرجع توکی;
	}
    }
    أرجع &t_any;
}
// for getline()
    ساكن type_T *
ret_getline(صحيح argcount,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    إذا (argcount == 1)
	أرجع &t_string;
    *decl_type = &t_list_any;
    أرجع &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    ساكن type_T *
ret_string_or_any_1(صحيح argcount,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    إذا (argcount < 1)
	أرجع &t_string;
    أرجع &t_any;
}
    ساكن type_T *
ret_string_or_any_2(صحيح argcount,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    إذا (argcount < 2)
	أرجع &t_string;
    أرجع &t_any;
}
    ساكن type_T *
ret_string_or_any_3(صحيح argcount,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    إذا (argcount < 3)
	أرجع &t_string;
    أرجع &t_any;
}
    ساكن type_T *
ret_string_or_any_4(صحيح argcount,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    إذا (argcount < 4)
	أرجع &t_string;
    أرجع &t_any;
}
// for abs()
    ساكن type_T *
ret_abs(صحيح argcount,
	type2_T *argtypes,
	type_T	**decl_type ناکارول_شوی)
{
    إذا (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	أرجع &t_float;
    أرجع &t_شمېره;
}
// for sign_define()
    ساكن type_T *
ret_sign_define(صحيح argcount,
	type2_T *argtypes,
	type_T	**decl_type ناکارول_شوی)
{
    إذا (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	أرجع &t_list_شمېره;
    أرجع &t_شمېره;
}
// for values(): list of member of first argument
    ساكن type_T *
ret_list_member(صحيح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    إذا (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	إذا (current_type_gap != عدم
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	وإلا
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	إذا (current_type_gap != عدم
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    أرجع get_list_type(t->tt_member, current_type_gap);
    }
    أرجع &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    ساكن type_T *
ret_list_or_dict_0(صحيح argcount,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type)
{
    إذا (argcount > 0)
	أرجع &t_dict_any;
    *decl_type = &t_list_any;
    أرجع &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    ساكن type_T *
ret_list_or_dict_1(صحيح argcount,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type)
{
    إذا (argcount > 1)
	أرجع &t_dict_any;
    *decl_type = &t_list_any;
    أرجع &t_list_dict_any;
}

    ساكن type_T *
ret_argv(صحيح argcount,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type)
{
    // argv() returns list of strings
    إذا (argcount == 0)
    {
	*decl_type = &t_list_any;
	أرجع &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    أرجع &t_any;
}

    ساكن type_T *
ret_remove(صحيح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    إذا (argcount > 0)
    {
	إذا (argtypes[0].type_curr->tt_type == VAR_LIST
		|| argtypes[0].type_curr->tt_type == VAR_DICT)
	{
	    إذا (argcount == 3)
	    {
		*decl_type = argtypes[0].type_decl;
		أرجع argtypes[0].type_curr;
	    }
	    إذا (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    أرجع argtypes[0].type_curr->tt_member;
	}
	إذا (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    أرجع &t_شمېره;
    }
    أرجع &t_any;
}

    ساكن type_T *
ret_max_min(صحيح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    إذا (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	إذا (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	وإلا
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	إذا (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	وإلا
	    t = &t_any;
	أرجع t;
    }
    أرجع &t_any;
}

    ساكن type_T *
ret_getreg(صحيح argcount,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    إذا (argcount == 3)
    {
	*decl_type = &t_list_any;
	أرجع &t_list_string;
    }
    أرجع &t_string;
}

    ساكن type_T *
ret_virtcol(صحيح argcount,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    إذا (argcount > 1)
    {
	*decl_type = &t_list_any;
	أرجع &t_list_شمېره;
    }
    أرجع &t_شمېره;
}

    ساكن type_T *
ret_maparg(صحيح argcount,
	type2_T *argtypes ناکارول_شوی,
	type_T	**decl_type ناکارول_شوی)
{
    // Assume that if the fourth argument is passed it's non-zero
    إذا (argcount == 4)
	أرجع &t_dict_any;
    أرجع &t_string;
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
تعريفنوع بنية
{
    حرف	*f_name;	// function name
    حرف	f_min_argc;	// minimal number of arguments
    حرف	f_max_argc;	// maximal number of arguments
    حرف	f_argtype;	// for method: FEARG_ values; bits FE_
    argcheck_T	*f_argcheck;	// list of functions to check argument types;
				// use "arg_any" (not NULL) to accept an
				// argument of any type
    حرف	*f_argnames;	// names of the arguments as in the help,
				// separated by ", "; an argument without a
				// name is empty, NULL when there is none
    type_T	*(*f_retfunc)(صحيح argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    فراغ	(*f_func)(typval_T *دلیلونه, typval_T *rvar);
				// implementation of function
} funcentry_T;

// Set f_max_argc to VARGS for varargs.
#تعریف_کړه VARGS    CHAR_MAX

// values for f_argtype; zero means it cannot be used as a method
#تعریف_کړه FEARG_1	    0x01    // base is the first argument
#تعریف_کړه FEARG_2     0x02    // base is the second argument
#تعریف_کړه FEARG_3     0x03    // base is the third argument
#تعریف_کړه FEARG_4     0x04    // base is the fourth argument
#تعریف_کړه FEARG_ماسک  0x0F    // bits in f_argtype used as argument index
#تعریف_کړه FE_X	    0x10    // builtin accepts a non-value (class, typealias)

#إذا defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#وإلا
# define MATH_FUNC(name) عدم
#د_که_پای
#که_تعریف_شوی FEAT_TIMERS
# define TIMER_FUNC(name) name
#وإلا
# define TIMER_FUNC(name) عدم
#د_که_پای
#که_تعریف_شوی FEAT_دنده_CHANNEL
# define دنده_FUNC(name) name
#وإلا
# define دنده_FUNC(name) عدم
#د_که_پای
#که_تعریف_شوی FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#وإلا
# define PROP_FUNC(name) عدم
#د_که_پای
#که_تعریف_شوی FEAT_SIGNS
# define SIGN_FUNC(name) name
#وإلا
# define SIGN_FUNC(name) عدم
#د_که_پای
#که_تعریف_شوی FEAT_SOUND
# define SOUND_FUNC(name) name
#وإلا
# define SOUND_FUNC(name) عدم
#د_که_پای
#که_تعریف_شوی FEAT_TERMINAL
# define غړی_FUNC(name) name
#وإلا
# define غړی_FUNC(name) عدم
#د_که_پای
#که_تعریف_شوی FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#وإلا
# define TABPANEL_FUNC(name) عدم
#د_که_پای
#که_تعریف_شوی FEAT_IMAGE
# define IMAGE_FUNC(name) name
#وإلا
# define IMAGE_FUNC(name) عدم
#د_که_پای

ساكن ثابت funcentry_T global_functions[] =
{
    {"abs",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_abs,	    f_abs},
    {"acos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_acos},
    {"add",		2, 2, FEARG_1,	    arg2_listblobmod_توکی,
			"object, expr",
			ret_لومړی_arg,	    f_جمع_کړه},
    {"and",		2, 2, FEARG_1,	    arg2_شمېره,
			"expr1, expr2",
			ret_شمېره,	    f_and},
    {"append",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_شمېره_bool,    f_append},
    {"appendbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_شمېره_bool,    f_appendbufline},
    {"argc",		0, 1, 0,	    arg1_شمېره,
			"winid",
			ret_شمېره,	    f_argc},
    {"argidx",		0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_argidx},
    {"arglistid",	0, 2, 0,	    arg2_شمېره,
			"winnr, tabnr",
			ret_شمېره,	    f_arglistid},
    {"argv",		0, 2, 0,	    arg2_شمېره,
			"nr, winid",
			ret_argv,	    f_argv},
    {"asin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_asin},
    {"assert_beeps",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_شمېره_bool,    f_assert_beeps},
    {"assert_equal",	2, 3, FEARG_2,	    عدم,
			"expected, actual, msg",
			ret_شمېره_bool,    f_assert_equal},
    {"assert_equalfile", 2, 3, FEARG_1,	    arg3_string,
			"fname_one, fname_two, msg",
			ret_شمېره_bool,    f_assert_equalfile},
    {"assert_exception", 1, 2, 0,	    arg2_string,
			"error, msg",
			ret_شمېره_bool,    f_assert_exception},
    {"assert_fails",	1, 5, FEARG_1,	    arg15_assert_fails,
			"cmd, error, msg, lnum, context",
			ret_شمېره_bool,    f_assert_fails},
    {"assert_false",	1, 2, FEARG_1,	    عدم,
			"actual, msg",
			ret_شمېره_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_شمېره_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_شمېره_bool,    f_assert_سمون},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_شمېره_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    عدم,
			"expected, actual, msg",
			ret_شمېره_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_شمېره_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_شمېره_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    عدم,
			"actual, msg",
			ret_شمېره_bool,    f_assert_true},
    {"atan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_atan},
    {"atan2",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"expr1, expr2",
			ret_float,	    f_atan2},
    {"autocmd_add",	1, 1, FEARG_1,	    arg1_list_any,
			"acmds",
			ret_bool,	    f_autocmd_جمع_کړه},
    {"autocmd_delete",	1, 1, FEARG_1,	    arg1_list_any,
			"acmds",
			ret_bool,	    f_autocmd_delete},
    {"autocmd_get",	0, 1, FEARG_1,	    arg1_dict_any,
			"opts",
			ret_list_dict_any,  f_autocmd_get},
    {"balloon_gettext",	0, 0, 0,	    عدم,
			عدم,
			ret_string,
#که_تعریف_شوی FEAT_BEVAL
	    f_balloon_gettext
#وإلا
	    عدم
#د_که_پای
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#که_تعریف_شوی FEAT_BEVAL
	    f_balloon_show
#وإلا
	    عدم
#د_که_پای
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#إذا defined(FEAT_BEVAL_غړی)
	    f_balloon_وېشل
#وإلا
	    عدم
#د_که_پای
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
			ret_list_شمېره,    f_blob2list},
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
			ret_شمېره,	    f_bufadd},
    {"bufexists",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_شمېره_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_بفر,	// obsolete
			"buf",
			ret_شمېره_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_بفر,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_بفر,	// obsolete
			"buf",
			ret_شمېره,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_شمېره_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_شمېره_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_بفر_bool,
			"buf, create",
			ret_شمېره,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_شمېره,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_شمېره,	    f_bufwinnr},
    {"byte2line",	1, 1, FEARG_1,	    arg1_شمېره,
			"byte",
			ret_شمېره,	    f_byte2line},
    {"byteidx",		2, 3, FEARG_1,	    arg3_string_شمېره_bool,
			"expr, nr, utf16",
			ret_شمېره,	    f_byteidx},
    {"byteidxcomp",	2, 3, FEARG_1,	    arg3_string_شمېره_bool,
			"expr, nr, utf16",
			ret_شمېره,	    f_byteidxcomp},
    {"call",		2, 3, FEARG_1,	    arg3_any_list_dict,
			"func, arglist, dict",
			ret_any,	    f_call},
    {"ceil",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ceil},
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_دنده,
			"handle",
			ret_شمېره_bool,    دنده_FUNC(f_ch_canread)},
    {"ch_close",	1, 1, FEARG_1,	    arg1_chan_or_دنده,
			"handle",
			ret_void,	    دنده_FUNC(f_ch_close)},
    {"ch_close_in",	1, 1, FEARG_1,	    arg1_chan_or_دنده,
			"handle",
			ret_void,	    دنده_FUNC(f_ch_close_دننه)},
    {"ch_evalexpr",	2, 3, FEARG_1,	    arg23_chanexpr,
			"handle, expr, options",
			ret_any,	    دنده_FUNC(f_ch_evalexpr)},
    {"ch_evalraw",	2, 3, FEARG_1,	    arg23_chanraw,
			"handle, string, options",
			ret_any,	    دنده_FUNC(f_ch_evalraw)},
    {"ch_getbufnr",	2, 2, FEARG_1,	    arg2_chan_or_دنده_string,
			"handle, what",
			ret_شمېره,	    دنده_FUNC(f_ch_getbufnr)},
    {"ch_getjob",	1, 1, FEARG_1,	    arg1_chan_or_دنده,
			"channel",
			ret_دنده,	    دنده_FUNC(f_ch_getjob)},
    {"ch_info",		1, 1, FEARG_1,	    arg1_chan_or_دنده,
			"handle",
			ret_dict_any,	    دنده_FUNC(f_ch_info)},
    {"ch_listen",	1, 2, FEARG_1,	    arg2_string_dict,
			"address, options",
			ret_channel,	    دنده_FUNC(f_ch_listen)},
    {"ch_log",		1, 2, FEARG_1,	    arg2_string_chan_or_دنده,
			"msg, handle",
			ret_void,	    f_ch_log},
    {"ch_logfile",	1, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_void,	    f_ch_logfile},
    {"ch_open",		1, 2, FEARG_1,	    arg2_string_dict,
			"address, options",
			ret_channel,	    دنده_FUNC(f_ch_open)},
    {"ch_read",		1, 2, FEARG_1,	    arg2_chan_or_دنده_dict,
			"handle, options",
			ret_string,	    دنده_FUNC(f_ch_read)},
    {"ch_readblob",	1, 2, FEARG_1,	    arg2_chan_or_دنده_dict,
			"handle, options",
			ret_blob,	    دنده_FUNC(f_ch_readblob)},
    {"ch_readraw",	1, 2, FEARG_1,	    arg2_chan_or_دنده_dict,
			"handle, options",
			ret_string,	    دنده_FUNC(f_ch_readraw)},
    {"ch_sendexpr",	2, 3, FEARG_1,	    arg23_chanexpr,
			"handle, expr, options",
			ret_any,	    دنده_FUNC(f_ch_sendexpr)},
    {"ch_sendraw",	2, 3, FEARG_1,	    arg23_chanraw,
			"handle, expr, options",
			ret_void,	    دنده_FUNC(f_ch_sendraw)},
    {"ch_setoptions",	2, 2, FEARG_1,	    arg2_chan_or_دنده_dict,
			"handle, options",
			ret_void,	    دنده_FUNC(f_ch_setoptions)},
    {"ch_status",	1, 2, FEARG_1,	    arg2_chan_or_دنده_dict,
			"handle, options",
			ret_string,	    دنده_FUNC(f_ch_حالت)},
    {"changenr",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_changenr},
    {"char2nr",		1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_شمېره,	    f_char2nr},
    {"charclass",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_شمېره,	    f_charclass},
    {"charcol",		1, 2, FEARG_1,	    arg2_string_or_list_شمېره,
			"expr, winid",
			ret_شمېره,	    f_charcol},
    {"charidx",		2, 4, FEARG_1,	    arg4_string_شمېره_bool_bool,
			"string, idx, countcc, utf16",
			ret_شمېره,	    f_charidx},
    {"chdir",		1, 2, FEARG_1,	    arg2_string,
			"dir, scope",
			ret_string,	    f_chdir},
    {"cindent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_شمېره,	    f_cindent},
    {"clearmatches",	0, 1, FEARG_1,	    arg1_شمېره,
			"win",
			ret_void,	    f_clearmatches},
    {"cmdcomplete_info",0, 0, 0,	    عدم,
			عدم,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_شمېره,
			"expr, winid",
			ret_شمېره,	    f_ستون},
    {"complete",	2, 2, FEARG_2,	    arg2_شمېره_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_شمېره,	    f_complete_جمع_کړه},
    {"complete_check",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_شمېره_string,
			"msg, choices, default, type",
			ret_شمېره,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    عدم,
			"expr",
			ret_copy,	    f_copy},
    {"cos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cos},
    {"cosh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cosh},
    {"count",		2, 4, FEARG_1,	    arg24_شمېر,
			"comp, expr, ic, start",
			ret_شمېره,	    f_شمېر},
    {"cscope_connection",0,3, 0,	    arg3_شمېره_string_string,
			"num, dbpath, prepend",
			ret_شمېره,	    f_cscope_connection},
    {"cursor",		1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_شمېره,	    f_cursor},
    {"debugbreak",	1, 1, FEARG_1,	    arg1_شمېره,
			"pid",
			ret_شمېره,
#که_تعریف_شوی MSWIN
	    f_debugbreak
#وإلا
	    عدم
#د_که_پای
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_شمېره_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_بفر_lnum_lnum,
			"buf, first, last",
			ret_شمېره_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره_bool,    f_did_filetype},
    {"diff",		2, 3, FEARG_1,	    arg3_diff,
			"fromlist, tolist, options",
			ret_string_or_any_3,  f_diff},
    {"diff_filler",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_شمېره,	    f_diff_filler},
    {"diff_hlID",	2, 2, FEARG_1,	    arg2_lnum_شمېره,
			"lnum, col",
			ret_شمېره,	    f_diff_hlID},
    {"digraph_get",	1, 1, FEARG_1,	    arg1_string,
			"chars",
			ret_string,	    f_digraph_get},
    {"digraph_getlist",	0, 1, FEARG_1,	    arg1_bool,
			"listall",
			ret_list_string_توکي, f_digraph_getlist},
    {"digraph_set",	2, 2, FEARG_1,	    arg2_string,
			"chars, digraph",
			ret_bool,	f_digraph_سېټ},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    عدم,
			"expr",
			ret_شمېره_bool,    f_تشه},
    {"environ",		0, 0, 0,	    عدم,
			عدم,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    عدم,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره_bool,    f_eventhandler},
    {"executable",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_شمېره,	    f_executable},
    {"execute",		1, 2, FEARG_1,	    arg12_execute,
			"command, silent",
			ret_string,	    f_execute},
    {"exepath",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_exepath},
    {"exists",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_شمېره_bool,    f_exists},
    {"exists_compiled",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_شمېره_bool,    f_exists_compiled},
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
			ret_لومړی_cont,	    f_extendnew},
    {"feedkeys",	1, 2, FEARG_1,	    arg2_string,
			"string, mode",
			ret_void,	    f_feedkeys},
    {"file_readable",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"file",
			ret_شمېره_bool,    f_filereadable},
    {"filecopy",	2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_شمېره_bool,    f_filecopy},
    {"filereadable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_شمېره_bool,    f_filereadable},
    {"filewritable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_شمېره,	    f_filewritable},
    {"filter",		2, 2, FEARG_1,	    arg2_filter,
			"expr1, expr2",
			ret_لومړی_arg,	    f_filter},
    {"finddir",		1, 3, FEARG_1,	    arg3_string_string_شمېره,
			"name, path, count",
			ret_string_or_any_3,	    f_finddir},
    {"findfile",	1, 3, FEARG_1,	    arg3_string_string_شمېره,
			"name, path, count",
			ret_string_or_any_3,	    f_findfile},
    {"flatten",		1, 2, FEARG_1,	    arg2_list_any_شمېره,
			"list, maxdepth",
			ret_list_any,	    f_flatten},
    {"flattennew",	1, 2, FEARG_1,	    arg2_list_any_شمېره,
			"list, maxdepth",
			ret_list_any,	    f_flattennew},
    {"float2nr",	1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_شمېره,	    f_float2nr},
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
    {"foldclosed",	1, 2, FEARG_1,	    arg2_lnum_شمېره,
			"lnum, winid",
			ret_شمېره,	    f_foldclosed},
    {"foldclosedend",	1, 2, FEARG_1,	    arg2_lnum_شمېره,
			"lnum, winid",
			ret_شمېره,	    f_foldclosedend},
    {"foldlevel",	1, 2, FEARG_1,	    arg2_lnum_شمېره,
			"lnum, winid",
			ret_شمېره,	    f_foldlevel},
    {"foldtext",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_شمېره,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_لومړی_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    عدم,
			عدم,
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
    {"getbgcolor",	0, 0, 0,	    عدم,
			عدم,
			ret_list_any,	    f_getbgcolor},
    {"getbufinfo",	0, 1, FEARG_1,	    arg1_بفر_or_dict_any,
			"buf",
			ret_list_dict_any,  f_getbufinfo},
    {"getbufline",	2, 3, FEARG_1,	    arg3_بفر_lnum_lnum,
			"buf, lnum, end",
			ret_list_string,    f_getbufline},
    {"getbufoneline",	2, 2, FEARG_1,	    arg2_بفر_lnum,
			"buf, lnum",
			ret_string,	    f_getbufoneline},
    {"getbufvar",	2, 3, FEARG_1,	    arg3_بفر_string_any,
			"buf, varname, def",
			ret_any,	    f_getbufvar},
    {"getcellpixels",	0, 0, 0,	    عدم,
			عدم,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    عدم,
			عدم,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_شمېره,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    عدم,
			عدم,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_getcmdwintype},
    {"getcompletion",	2, 3, FEARG_1,	    arg3_string_string_bool,
			"pat, type, filtered",
			ret_list_string,    f_getcompletion},
    {"getcompletiontype", 1, 1, FEARG_1,    arg1_string,
			"pat",
			ret_string,	    f_getcompletiontype},
    {"getcurpos",	0, 1, FEARG_1,	    arg1_شمېره,
			"winid",
			ret_list_شمېره,    f_getcurpos},
    {"getcursorcharpos", 0, 1, FEARG_1,	    arg1_شمېره,
			"winid",
			ret_list_شمېره,    f_getcursorcharpos},
    {"getcwd",		0, 2, FEARG_1,	    arg2_شمېره,
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
			ret_شمېره,	    f_getfsize},
    {"getftime",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_شمېره,	    f_getftime},
    {"getftype",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getftype},
    {"getimstatus",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره_bool,    f_getimstatus},
    {"getinfo",		2, 3, FEARG_2,	    arg3_string_string_dict,
			"kind, name, opts",
			ret_dict_any,	    f_getinfo},
    {"getjumplist",	0, 2, FEARG_1,	    arg2_شمېره,
			"winnr, tabnr",
			ret_list_any,	    f_getjumplist},
    {"getline",		1, 2, FEARG_1,	    arg2_lnum,
			"lnum, end",
			ret_getline,	    f_getline},
    {"getloclist",	1, 2, 0,	    arg2_شمېره_dict_any,
			"nr, what",
			ret_list_or_dict_1, f_getloclist},
    {"getmarklist",	0, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_شمېره,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    عدم,
			عدم,
			ret_dict_شمېره,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_getpid},
    {"getpos",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_شمېره,    f_getpos},
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
    {"getstacktrace",	0, 0, 0,	    عدم,
			عدم,
			ret_list_dict_any,  f_getstacktrace},
    {"gettabinfo",	0, 1, FEARG_1,	    arg1_شمېره,
			"tabnr",
			ret_list_dict_any,  f_gettabinfo},
    {"gettabvar",	2, 3, FEARG_1,	    arg3_شمېره_string_any,
			"tabnr, varname, def",
			ret_any,	    f_gettabvar},
    {"gettabwinvar",	3, 4, FEARG_1,	    arg4_شمېره_شمېره_string_any,
			"tabnr, winnr, varname, def",
			ret_any,	    f_gettabwinvar},
    {"gettagstack",	0, 1, FEARG_1,	    arg1_شمېره,
			"winnr",
			ret_dict_any,	    f_gettagstack},
    {"gettext",		1, 2, FEARG_1,	    arg2_string,
			"text, package",
			ret_string,	    f_gettext},
    {"getwininfo",	0, 1, FEARG_1,	    arg1_شمېره,
			"winid",
			ret_list_dict_any,  f_getwininfo},
    {"getwinpos",	0, 1, FEARG_1,	    arg1_شمېره,
			"timeout",
			ret_list_شمېره,    f_getwinpos},
    {"getwinposx",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_getwinposy},
    {"getwinvar",	2, 3, FEARG_1,	    arg3_شمېره_string_any,
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
			ret_شمېره_bool,    f_has},
    {"has_key",		2, 2, FEARG_1,	    arg2_dict_any_string_or_nr,
			"dict, key",
			ret_شمېره_bool,    f_has_کلی},
    {"haslocaldir",	0, 2, FEARG_1,	    arg2_شمېره,
			"winnr, tabnr",
			ret_شمېره,	    f_haslocaldir},
    {"hasmapto",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"what, mode, abbr",
			ret_شمېره_bool,    f_hasmapto},
    {"highlightID",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_شمېره,	    f_hlID},
    {"highlight_exists",1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_شمېره_bool,    f_hlexists},
    {"histadd",		2, 2, FEARG_2,	    arg2_string,
			"history, item",
			ret_شمېره_bool,    f_histadd},
    {"histdel",		1, 2, FEARG_1,	    arg2_string_string_or_شمېره,
			"history, item",
			ret_شمېره_bool,    f_histdel},
    {"histget",		1, 2, FEARG_1,	    arg2_string_شمېره,
			"history, index",
			ret_string,	    f_histget},
    {"histnr",		1, 1, FEARG_1,	    arg1_string,
			"history",
			ret_شمېره,	    f_histnr},
    {"hlID",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_شمېره,	    f_hlID},
    {"hlexists",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_شمېره_bool,    f_hlexists},
    {"hlget",		0, 2, FEARG_1,	    arg2_string_bool,
			"name, resolve",
			ret_list_dict_any,  f_hlget},
    {"hlset",		1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_شمېره_bool,    f_hlset},
    {"hostname",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    عدم,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_شمېره,	    IMAGE_FUNC(f_image_جمع_کړه)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_شمېره,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_شمېره,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_شمېره,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_شاخص,
			"object, expr, start, ic",
			ret_شمېره,	    f_شاخص},
    {"indexof",		2, 3, FEARG_1,	    arg23_شاخص,
			"object, expr, opts",
			ret_شمېره,	    f_indexof},
    {"input",		1, 3, FEARG_1,	    arg3_string,
			"prompt, text, completion",
			ret_string,	    f_ننوت},
    {"inputdialog",	1, 3, FEARG_1,	    arg3_string,
			"prompt, text, cancelreturn",
			ret_string,	    f_inputdialog},
    {"inputlist",	1, 1, FEARG_1,	    arg1_list_string,
			"textlist",
			ret_شمېره,	    f_inputlist},
    {"inputrestore",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_ورننباسه,
			"object, item, idx",
			ret_لومړی_arg,	    f_ورننباسه},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    عدم,
			عدم,
			ret_void,	    f_مداخله},
    {"invert",		1, 1, FEARG_1,	    arg1_شمېره,
			"expr",
			ret_شمېره,	    f_invert},
    {"isabsolutepath",	1, 1, FEARG_1,	    arg1_string,
			"path",
			ret_شمېره_bool,    f_isabsolutepath},
    {"isdirectory",	1, 1, FEARG_1,	    arg1_string,
			"directory",
			ret_شمېره_bool,    f_isdirectory},
    {"isinf",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_شمېره,	    MATH_FUNC(f_isinf)},
    {"islocked",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_شمېره_bool,    f_islocked},
    {"isnan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_شمېره_bool,    MATH_FUNC(f_isnan)},
    {"items",		1, 1, FEARG_1,	    arg1_list_tuple_dict_blob_or_string,
			"expr",
			ret_list_توکي,	    f_توکي},
    {"job_getchannel",	1, 1, FEARG_1,	    arg1_دنده,
			"job",
			ret_channel,	    دنده_FUNC(f_دنده_getchannel)},
    {"job_info",	0, 1, FEARG_1,	    arg1_دنده,
			"job",
			ret_دنده_info,	    دنده_FUNC(f_دنده_info)},
    {"job_setoptions",	2, 2, FEARG_1,	    arg2_دنده_dict,
			"job, options",
			ret_void,	    دنده_FUNC(f_دنده_setoptions)},
    {"job_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"command, options",
			ret_دنده,	    دنده_FUNC(f_دنده_پیل)},
    {"job_status",	1, 1, FEARG_1,	    arg1_دنده,
			"job",
			ret_string,	    دنده_FUNC(f_دنده_حالت)},
    {"job_stop",	1, 2, FEARG_1,	    arg2_دنده_string_or_شمېره,
			"job, how",
			ret_شمېره_bool,    دنده_FUNC(f_دنده_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    عدم,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    عدم,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_کلیګانې},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    عدم,	// obsolete
			عدم,
			ret_شمېره,	    f_وروستی_بفر_nr},
    {"len",		1, 1, FEARG_1,	    arg1_len,
			"expr",
			ret_شمېره,	    f_len},
    {"libcall",		3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_string,	    f_libcall},
    {"libcallnr",	3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_شمېره,	    f_libcallnr},
    {"line",		1, 2, FEARG_1,	    arg2_string_شمېره,
			"expr, winid",
			ret_شمېره,	    f_کرښه},
    {"line2byte",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_شمېره,	    f_line2byte},
    {"lispindent",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_شمېره,	    f_lispindent},
    {"list2blob",	1, 1, FEARG_1,	    arg1_list_شمېره,
			"list",
			ret_blob,	    f_list2blob},
    {"list2str",	1, 2, FEARG_1,	    arg2_list_شمېره_bool,
			"list, utf8",
			ret_string,	    f_list2str},
    {"list2tuple",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_tuple_any,	    f_list2tuple},
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_بفر_bool_or_dict,
			"callback, buf, options",
			ret_شمېره,	    f_listener_جمع_کړه},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_شمېره,
			"id",
			ret_شمېره_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_localtime},
    {"log",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_log},
    {"log10",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_log10},
    {"luaeval",		1, 2, FEARG_1,	    arg2_string_any,
			"expr1, expr2",
			ret_any,
#که_تعریف_شوی FEAT_LUA
		f_luaeval
#وإلا
		عدم
#د_که_پای
			},
    {"map",		2, 2, FEARG_1,	    arg2_map,
			"expr1, expr2",
			ret_لومړی_cont,	    f_map},
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
			ret_لومړی_cont,	    f_mapnew},
    {"mapset",		1, 3, FEARG_1,	    arg3_string_or_dict_bool_dict,
			"mode, abbr, dict",
			ret_void,	    f_mapset},
    {"match",		2, 4, FEARG_1,	    arg24_سمون_func,
			"expr, pat, start, count",
			ret_شمېره,	    f_سمون},
    {"matchadd",	2, 5, FEARG_1,	    arg25_matchadd,
			"group, pattern, priority, id, dict",
			ret_شمېره,	    f_matchadd},
    {"matchaddpos",	2, 5, FEARG_1,	    arg25_matchaddpos,
			"group, pos, priority, id, dict",
			ret_شمېره,	    f_matchaddpos},
    {"matcharg",	1, 1, FEARG_1,	    arg1_شمېره,
			"nr",
			ret_list_string,    f_matcharg},
    {"matchbufline",	4, 5, FEARG_1,	    arg45_matchbufline,
			"buf, pat, lnum, end, dict",
			ret_list_any,	    f_matchbufline},
    {"matchdelete",	1, 2, FEARG_1,	    arg2_شمېره,
			"id, win",
			ret_شمېره_bool,    f_matchdelete},
    {"matchend",	2, 4, FEARG_1,	    arg24_سمون_func,
			"expr, pat, start, count",
			ret_شمېره,	    f_matchend},
    {"matchfuzzy",	2, 3, FEARG_1,	    arg3_list_string_dict,
			"list, str, dict",
			ret_list_any,	    f_matchfuzzy},
    {"matchfuzzypos",	2, 3, FEARG_1,	    arg3_list_string_dict,
			"list, str, dict",
			ret_list_list_any,  f_matchfuzzypos},
    {"matchlist",	2, 4, FEARG_1,	    arg24_سمون_func,
			"expr, pat, start, count",
			ret_list_string,    f_matchlist},
    {"matchstr",	2, 4, FEARG_1,	    arg24_سمون_func,
			"expr, pat, start, count",
			ret_string,	    f_matchstr},
    {"matchstrlist",	2, 3, FEARG_1,	    arg23_matchstrlist,
			"list, pat, dict",
			ret_list_any,	    f_matchstrlist},
    {"matchstrpos",	2, 4, FEARG_1,	    arg24_سمون_func,
			"expr, pat, start, count",
			ret_list_any,	    f_matchstrpos},
    {"max",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_max},
    {"menu_info",	1, 2, FEARG_1,	    arg2_string,
			"name, mode",
			ret_dict_any,
#که_تعریف_شوی FEAT_MENU
	    f_menu_info
#وإلا
	    عدم
#د_که_پای
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_شمېره,
			"name, flags, prot",
			ret_شمېره_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_ډېرپېښ},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#که_تعریف_شوی FEAT_MZSCHEME
	    f_mzeval
#وإلا
	    عدم
#د_که_پای
			},
    {"nextnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_شمېره,	    f_nextnonblank},
    {"ngettext",	3, 4, FEARG_3,	    arg4_string_string_شمېره_string,
			"single, plural, number, domain",
			ret_string,	    f_ngettext},
    {"nr2char",		1, 2, FEARG_1,	    arg2_شمېره_bool,
			"expr, utf8",
			ret_string,	    f_nr2char},
    {"or",		2, 2, FEARG_1,	    arg2_شمېره,
			"expr1, expr2",
			ret_شمېره,	    f_or},
    {"pathshorten",	1, 2, FEARG_1,	    arg2_string_شمېره,
			"path, len",
			ret_string,	    f_pathshorten},
    {"perleval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#که_تعریف_شوی FEAT_PERL
	    f_perleval
#وإلا
	    عدم
#د_که_پای
			},
    {"popup_atcursor",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_شمېره,	    PROP_FUNC(f_popup_atcursor)},
    {"popup_beval",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_شمېره,	    PROP_FUNC(f_popup_beval)},
    {"popup_clear",	0, 1, 0,	    arg1_bool,
			"force",
			ret_void,	    PROP_FUNC(f_popup_clear)},
    {"popup_close",	1, 2, FEARG_1,	    arg2_شمېره_any,
			"id, result",
			ret_void,	    PROP_FUNC(f_popup_close)},
    {"popup_create",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_شمېره,	    PROP_FUNC(f_popup_create)},
    {"popup_dialog",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_شمېره,	    PROP_FUNC(f_popup_dialog)},
    {"popup_filter_menu", 2, 2, 0,	    arg2_شمېره_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_menu)},
    {"popup_filter_yesno", 2, 2, 0,	    arg2_شمېره_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_yesno)},
    {"popup_findecho",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    PROP_FUNC(f_popup_findpreview)},
    {"popup_getoptions", 1, 1, FEARG_1,	    arg1_شمېره,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getoptions)},
    {"popup_getpos",	1, 1, FEARG_1,	    arg1_شمېره,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getpos)},
    {"popup_hide",	1, 1, FEARG_1,	    arg1_شمېره,
			"id",
			ret_void,	    PROP_FUNC(f_popup_hide)},
    {"popup_list",	0, 0, 0,	    عدم,
			عدم,
			ret_list_شمېره,    PROP_FUNC(f_popup_list)},
    {"popup_locate",	2, 2, 0,	    arg2_شمېره,
			"row, col",
			ret_شمېره,	    PROP_FUNC(f_popup_locate)},
    {"popup_menu",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_شمېره,	    PROP_FUNC(f_popup_menu)},
    {"popup_move",	2, 2, FEARG_1,	    arg2_شمېره_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_move)},
    {"popup_notification", 2, 2, FEARG_1,   arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_شمېره,	    PROP_FUNC(f_popup_notification)},
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_شمېره_بفر,
			"id, buf",
			ret_شمېره_bool,    PROP_FUNC(f_popup_setbuf)},
    {"popup_setoptions", 2, 2, FEARG_1,	    arg2_شمېره_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_setoptions)},
    {"popup_settext",	2, 2, FEARG_1,	    arg2_شمېره_string_or_list,
			"id, text",
			ret_void,	    PROP_FUNC(f_popup_settext)},
    {"popup_show",	1, 1, FEARG_1,	    arg1_شمېره,
			"id",
			ret_شمېره,	    PROP_FUNC(f_popup_show)},
    {"pow",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"x, y",
			ret_float,	    f_pow},
    {"preinserted",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_شمېره,	    f_prevnonblank},
    {"printf",		1, 19, FEARG_2,	    arg119_printf,
			"fmt, expr1",
			ret_string,	    f_printf},
    {"prompt_getprompt", 1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_string,	    دنده_FUNC(f_prompt_getprompt)},
    {"prompt_setcallback", 2, 2, FEARG_1,   arg2_بفر_any,
			"buf, expr",
			ret_void,	    دنده_FUNC(f_prompt_setcallback)},
    {"prompt_setinterrupt", 2, 2, FEARG_1,  arg2_بفر_any,
			"buf, expr",
			ret_void,	    دنده_FUNC(f_prompt_setinterrupt)},
    {"prompt_setprompt", 2, 2, FEARG_1,	    arg2_بفر_string,
			"buf, text",
			ret_void,	    دنده_FUNC(f_prompt_setprompt)},
    {"prop_add",	3, 3, FEARG_1,	    arg3_شمېره_شمېره_dict,
			"lnum, col, props",
			ret_شمېره,	    PROP_FUNC(f_prop_جمع_کړه)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_جمع_کړه_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_شمېره_شمېره_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_پیدا_کړه)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_شمېره_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_شمېره_شمېره,
			"props, lnum, lnum_end",
			ret_شمېره,	    PROP_FUNC(f_prop_remove)},
    {"prop_type_add",	2, 2, FEARG_1,	    arg2_string_dict,
			"name, props",
			ret_void,	    PROP_FUNC(f_prop_type_جمع_کړه)},
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
    {"pum_getpos",	0, 0, 0,	    عدم,
			عدم,
			ret_dict_شمېره,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#که_تعریف_شوی FEAT_PYTHON3
	    f_py3eval
#وإلا
	    عدم
#د_که_پای
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#که_تعریف_شوی FEAT_PYTHON
	    f_pyeval
#وإلا
	    عدم
#د_که_پای
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#وإلا
	    عدم
#د_که_پای
			},
    {"rand",		0, 1, FEARG_1,	    arg1_list_شمېره,
			"expr",
			ret_شمېره,	    f_rand},
    {"range",		1, 3, FEARG_1,	    arg3_شمېره,
			"expr, max, stride",
			ret_list_شمېره,    f_range},
    {"readblob",	1, 3, FEARG_1,	    arg3_string_شمېره_شمېره,
			"fname, offset, size",
			ret_blob,	    f_readblob},
    {"readdir",		1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_string,    f_readdir},
    {"readdirex",	1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_dict_any,  f_readdirex},
    {"readfile",	1, 3, FEARG_1,	    arg3_string_string_شمېره,
			"fname, type, max",
			ret_list_string,    f_readfile},
    {"redraw_listener_add", 1, 1, FEARG_1,  arg1_dict_any,
			"opts",
			ret_شمېره,	    f_redraw_listener_جمع_کړه},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_شمېره,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_reg_recording},
    {"reltime",		0, 2, FEARG_1,	    arg2_list_شمېره,
			"start, end",
			ret_list_شمېره,    f_reltime},
    {"reltimefloat",	1, 1, FEARG_1,	    arg1_list_شمېره,
			"time",
			ret_float,	    f_reltimefloat},
    {"reltimestr",	1, 1, FEARG_1,	    arg1_list_شمېره,
			"time",
			ret_string,	    f_reltimestr},
    {"remote_expr",	2, 4, FEARG_1,	    arg24_remote_بیان,
			"server, string, idvar, timeout",
			ret_string,	    f_remote_بیان},
    {"remote_foreground", 1, 1, FEARG_1,    arg1_string,
			"server",
			ret_void,	    f_remote_foreground},
    {"remote_peek",	1, 2, FEARG_1,	    arg2_string,
			"serverid, retvar",
			ret_شمېره,	    f_remote_peek},
    {"remote_read",	1, 2, FEARG_1,	    arg2_string_شمېره,
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
			ret_شمېره_bool,    f_rename},
    {"repeat",		2, 2, FEARG_1,	    arg2_repeat,
			"expr, count",
			ret_repeat,	    f_repeat},
    {"resolve",		1, 1, FEARG_1,	    arg1_string,
			"filename",
			ret_string,	    f_resolve},
    {"reverse",		1, 1, FEARG_1,	    arg1_سرچپه,
			"object",
			ret_لومړی_arg,	    f_سرچپه},
    {"round",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_پړاو},
    {"rubyeval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#که_تعریف_شوی FEAT_RUBY
	    f_rubyeval
#وإلا
	    عدم
#د_که_پای
			},
    {"screenattr",	2, 2, FEARG_1,	    arg2_شمېره,
			"row, col",
			ret_شمېره,	    f_screenattr},
    {"screenchar",	2, 2, FEARG_1,	    arg2_شمېره,
			"row, col",
			ret_شمېره,	    f_screenchar},
    {"screenchars",	2, 2, FEARG_1,	    arg2_شمېره,
			"row, col",
			ret_list_شمېره,    f_screenchars},
    {"screencol",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_شمېره,
			"winid, lnum, col",
			ret_dict_شمېره,    f_screenpos},
    {"screenrow",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_screenrow},
    {"screenstring",	2, 2, FEARG_1,	    arg2_شمېره,
			"row, col",
			ret_string,	    f_screenstring},
    {"search",		1, 5, FEARG_1,	    arg15_پلټنه,
			"pattern, flags, stopline, timeout, skip",
			ret_شمېره,	    f_پلټنه},
    {"searchcount",	0, 1, FEARG_1,	    arg1_dict_any,
			"options",
			ret_dict_شمېره,    f_searchcount},
    {"searchdecl",	1, 3, FEARG_1,	    arg3_string_bool_bool,
			"name, global, thisblock",
			ret_شمېره_bool,    f_searchdecl},
    {"searchpair",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_شمېره,	    f_searchpair},
    {"searchpairpos",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_list_شمېره,    f_searchpairpos},
    {"searchpos",	1, 5, FEARG_1,	    arg15_پلټنه,
			"pattern, flags, stopline, timeout, skip",
			ret_list_شمېره,    f_searchpos},
    {"server2client",	2, 2, FEARG_1,	    arg2_string,
			"clientid, string",
			ret_شمېره_bool,    f_server2client},
    {"serverlist",	0, 1, 0,	    arg1_dict_any,
			"dict",
			ret_string_or_any_1,	    f_serverlist},
    {"setbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_شمېره_bool,    f_setbufline},
    {"setbufvar",	3, 3, FEARG_3,	    arg3_بفر_string_any,
			"buf, varname, val",
			ret_void,	    f_setbufvar},
    {"setcellwidths",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_void,	    f_setcellwidths},
    {"setcharpos",	2, 2, FEARG_2,	    arg2_string_list_شمېره,
			"expr, list",
			ret_شمېره_bool,    f_setcharpos},
    {"setcharsearch",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_setcharsearch},
    {"setcmdline",	1, 2, FEARG_1,	    arg2_string_شمېره,
			"str, pos",
			ret_شمېره_bool,    f_setcmdline},
    {"setcmdpos",	1, 1, FEARG_1,	    arg1_شمېره,
			"pos",
			ret_شمېره_bool,    f_setcmdpos},
    {"setcursorcharpos", 1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_شمېره_bool,    f_setcursorcharpos},
    {"setenv",		2, 2, FEARG_2,	    arg2_string_any,
			"name, val",
			ret_void,	    f_setenv},
    {"setfperm",	2, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_شمېره_bool,    f_setfperm},
    {"setline",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_شمېره_bool,    f_setline},
    {"setloclist",	2, 4, FEARG_2,	    arg24_setloclist,
			"nr, list, action, what",
			ret_شمېره_bool,    f_setloclist},
    {"setmatches",	1, 2, FEARG_1,	    arg2_list_any_شمېره,
			"list, win",
			ret_شمېره_bool,    f_setmatches},
    {"setpos",		2, 2, FEARG_2,	    arg2_string_list_شمېره,
			"expr, list",
			ret_شمېره_bool,    f_setpos},
    {"setqflist",	1, 3, FEARG_1,	    arg13_setqflist,
			"list, action, what",
			ret_شمېره_bool,    f_setqflist},
    {"setreg",		2, 3, FEARG_2,	    arg3_string_any_string,
			"regname, value, options",
			ret_شمېره_bool,    f_setreg},
    {"settabvar",	3, 3, FEARG_3,	    arg3_شمېره_string_any,
			"tabnr, varname, val",
			ret_void,	    f_settabvar},
    {"settabwinvar",	4, 4, FEARG_4,	    arg4_شمېره_شمېره_string_any,
			"tabnr, winnr, varname, val",
			ret_void,	    f_settabwinvar},
    {"settagstack",	2, 3, FEARG_2,	    arg23_settagstack,
			"nr, dict, action",
			ret_شمېره_bool,    f_settagstack},
    {"setwinvar",	3, 3, FEARG_3,	    arg3_شمېره_string_any,
			"winnr, varname, val",
			ret_void,	    f_setwinvar},
    {"sha256",		1, 1, FEARG_1,	    arg1_string_or_blob,
			"expr",
			ret_string,
#که_تعریف_شوی FEAT_CRYPT
	    f_sha256
#وإلا
	    عدم
#د_که_پای
			},
    {"shellescape",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, special",
			ret_string,	    f_shellescape},
    {"shiftwidth",	0, 1, FEARG_1,	    arg1_شمېره,
			"col",
			ret_شمېره,	    f_shiftwidth},
    {"sign_define",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"name, dict",
			ret_sign_define,	    SIGN_FUNC(f_sign_define)},
    {"sign_getdefined",	0, 1, FEARG_1,	    arg1_string,
			"name",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getdefined)},
    {"sign_getplaced",	0, 2, FEARG_1,	    arg02_sign_getplaced,
			"buf, dict",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getplaced)},
    {"sign_jump",	3, 3, FEARG_1,	    arg3_شمېره_string_بفر,
			"id, group, buf",
			ret_شمېره,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_ځای_پرځای_کړه,
			"id, group, name, buf, dict",
			ret_شمېره,	    SIGN_FUNC(f_sign_ځای_پرځای_کړه)},
    {"sign_placelist",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_شمېره,    SIGN_FUNC(f_sign_placelist)},
    {"sign_undefine",	0, 1, FEARG_1,	    arg1_string_or_list_string,
			"name",
			ret_شمېره_bool,    SIGN_FUNC(f_sign_undefine)},
    {"sign_unplace",	1, 2, FEARG_1,	    arg2_string_dict,
			"group, dict",
			ret_شمېره_bool,    SIGN_FUNC(f_sign_unplace)},
    {"sign_unplacelist", 1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_شمېره,    SIGN_FUNC(f_sign_unplacelist)},
    {"simplify",	1, 1, FEARG_1,	    arg1_string,
			"filename",
			ret_string,	    f_simplify},
    {"sin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sin},
    {"sinh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sinh},
    {"slice",		2, 3, FEARG_1,	    arg23_ټوټه,
			"expr, start, end",
			ret_ټوټه,	    f_ټوټه},
    {"sort",		1, 3, FEARG_1,	    arg13_sortuniq,
			"list, how, dict",
			ret_لومړی_arg,	    f_ترتیبول},
    {"sound_clear",	0, 0, 0,	    عدم,
			عدم,
			ret_void,	    SOUND_FUNC(f_sound_clear)},
    {"sound_playevent",	1, 2, FEARG_1,	    arg2_string_any,
			"name, callback",
			ret_شمېره,	    SOUND_FUNC(f_sound_playevent)},
    {"sound_playfile",	1, 2, FEARG_1,	    arg2_string_any,
			"path, callback",
			ret_شمېره,	    SOUND_FUNC(f_sound_playfile)},
    {"sound_stop",	1, 1, FEARG_1,	    arg1_شمېره,
			"id",
			ret_void,	    SOUND_FUNC(f_sound_stop)},
    {"soundfold",	1, 1, FEARG_1,	    arg1_string,
			"word",
			ret_string,	    f_soundfold},
    {"spellbadword",	0, 1, FEARG_1,	    arg1_string,
			"sentence",
			ret_list_string,    f_spellbadword},
    {"spellsuggest",	1, 3, FEARG_1,	    arg3_string_شمېره_bool,
			"word, max, capital",
			ret_list_string,    f_spellsuggest},
    {"split",		1, 3, FEARG_1,	    arg3_string_string_bool,
			"string, pattern, keepempty",
			ret_list_string,    f_وېشل},
    {"sqrt",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sqrt},
    {"srand",		0, 1, FEARG_1,	    arg1_شمېره,
			"expr",
			ret_list_شمېره,    f_srand},
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
			ret_list_شمېره,    f_str2list},
    {"str2nr",		1, 3, FEARG_1,	    arg3_string_شمېره_bool,
			"string, base, quoted",
			ret_شمېره,	    f_str2nr},
    {"strcharlen",	1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_شمېره,	    f_strcharlen},
    {"strcharpart",	2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, skipcc",
			ret_string,	    f_strcharpart},
    {"strchars",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, skipcc",
			ret_شمېره,	    f_strchars},
    {"strdisplaywidth",	1, 2, FEARG_1,	    arg2_string_شمېره,
			"string, col",
			ret_شمېره,	    f_strdisplaywidth},
    {"strftime",	1, 2, FEARG_1,	    arg2_string_شمېره,
			"format, time",
			ret_string,
#که_تعریف_شوی HAVE_STRFTIME
	    f_strftime
#وإلا
	    عدم
#د_که_پای
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_شمېره,
			"str, index",
			ret_شمېره,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_شمېره,
			"haystack, needle, start",
			ret_شمېره,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, عدم,
			"expr",
			ret_string,	    f_string},
    {"strlen",		1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_شمېره,	    f_strlen},
    {"strpart",		2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, chars",
			ret_string,	    f_strpart},
    {"strptime",	2, 2, FEARG_1,	    arg2_string,
			"format, timestring",
			ret_شمېره,
#که_تعریف_شوی HAVE_STRPTIME
	    f_strptime
#وإلا
	    عدم
#د_که_پای
			},
    {"strridx",		2, 3, FEARG_1,	    arg3_string_string_شمېره,
			"haystack, needle, start",
			ret_شمېره,	    f_strridx},
    {"strtrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_strtrans},
    {"strutf16len",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, countcc",
			ret_شمېره,	    f_strutf16len},
    {"strwidth",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_شمېره,	    f_strwidth},
    {"submatch",	1, 2, FEARG_1,	    arg2_شمېره_bool,
			"nr, list",
			ret_string_or_any_2,	    f_submatch},
    {"substitute",	4, 4, FEARG_1,	    arg4_string_string_any_string,
			"string, pat, sub, flags",
			ret_string,	    f_substitute},
    {"swapfilelist",	0, 0, 0,	    عدم,
			عدم,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_string,	    f_swapname},
    {"synID",		3, 3, 0,	    arg3_lnum_شمېره_bool,
			"lnum, col, trans",
			ret_شمېره,	    f_synID},
    {"synIDattr",	2, 3, FEARG_1,	    arg3_شمېره_string_string,
			"synID, what, mode",
			ret_string,	    f_synIDattr},
    {"synIDtrans",	1, 1, FEARG_1,	    arg1_شمېره,
			"synID",
			ret_شمېره,	    f_synIDtrans},
    {"synconcealed",	2, 2, 0,	    arg2_lnum_شمېره,
			"lnum, col",
			ret_list_any,	    f_synconcealed},
    {"synstack",	2, 2, 0,	    arg2_lnum_شمېره,
			"lnum, col",
			ret_list_شمېره,    f_synstack},
    {"system",		1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_string,	    f_system},
    {"systemlist",	1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_list_string,    f_systemlist},
    {"tabpagebuflist",	0, 1, FEARG_1,	    arg1_شمېره,
			"arg",
			ret_list_شمېره,    f_tabpagebuflist},
    {"tabpagenr",	0, 1, 0,	    arg1_string,
			"arg",
			ret_شمېره,	    f_tabpagenr},
    {"tabpagewinnr",	1, 2, FEARG_1,	    arg2_شمېره_string,
			"tabarg, arg",
			ret_شمېره,	    f_tabpagewinnr},
    {"tabpanel_getinfo", 0, 0, 0,	    عدم,
			عدم,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_شمېره_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    عدم,
			عدم,
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
    {"tempname",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_شمېره,	    غړی_FUNC(f_غړی_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_شمېره,	    غړی_FUNC(f_غړی_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_بفر_string_dict,
			"buf, filename, options",
			ret_void,	    غړی_FUNC(f_غړی_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_بفر,
			"buf",
			ret_شمېره,	    غړی_FUNC(f_غړی_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_بفر,
			"buf",
			ret_list_string,
#إذا defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_غړی_getansicolors
#وإلا
	    عدم
#د_که_پای
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_شمېره_string,
			"attr, what",
			ret_شمېره,	    غړی_FUNC(f_غړی_getattr)},
    {"term_getcursor",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_list_any,	    غړی_FUNC(f_غړی_getcursor)},
    {"term_getjob",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_دنده,	    غړی_FUNC(f_غړی_getjob)},
    {"term_getline",	2, 2, FEARG_1,	    arg2_بفر_lnum,
			"buf, row",
			ret_string,	    غړی_FUNC(f_غړی_getline)},
    {"term_getscrolled", 1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_شمېره,	    غړی_FUNC(f_غړی_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_list_شمېره,    غړی_FUNC(f_غړی_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_string,	    غړی_FUNC(f_غړی_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_string,	    غړی_FUNC(f_غړی_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_بفر_bool,
			"buf, input",
			ret_string,	    غړی_FUNC(f_غړی_gettty)},
    {"term_list",	0, 0, 0,	    عدم,
			عدم,
			ret_list_شمېره,    غړی_FUNC(f_غړی_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_بفر_lnum,
			"buf, row",
			ret_list_dict_any,  غړی_FUNC(f_غړی_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_بفر_string,
			"buf, keys",
			ret_void,	    غړی_FUNC(f_غړی_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_بفر_list_any,
			"buf, colors",
			ret_void,
#إذا defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_غړی_setansicolors
#وإلا
	    عدم
#د_که_پای
			},
    {"term_setapi",	2, 2, FEARG_1,	    arg2_بفر_string,
			"buf, expr",
			ret_void,	    غړی_FUNC(f_غړی_setapi)},
    {"term_setkill",	2, 2, FEARG_1,	    arg2_بفر_string,
			"buf, how",
			ret_void,	    غړی_FUNC(f_غړی_setkill)},
    {"term_setrestore",	2, 2, FEARG_1,	    arg2_بفر_string,
			"buf, command",
			ret_void,	    غړی_FUNC(f_غړی_setrestore)},
    {"term_setsize",	3, 3, FEARG_1,	    arg3_بفر_شمېره_شمېره,
			"buf, rows, cols",
			ret_void,	    غړی_FUNC(f_غړی_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_شمېره,	    غړی_FUNC(f_غړی_پیل)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_بفر_شمېره,
			"buf, time",
			ret_void,	    غړی_FUNC(f_غړی_wait)},
    {"terminalprops",	0, 0, 0,	    عدم,
			عدم,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_شمېره,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    عدم,
			عدم,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    عدم,
			عدم,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   عدم,
			عدم,
			ret_void,	    f_test_garbagecollect_soon},
    {"test_getvalue",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_شمېره,	    f_test_getvalue},
    {"test_gui_event",	2, 2, FEARG_1,	    arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_gui_event},
    {"test_ignore_error", 1, 1, FEARG_1,    arg1_string,
			"expr",
			ret_void,	    f_test_ignore_error},
    {"test_mswin_event", 2, 2, FEARG_1,     arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_mswin_event},
    {"test_null_blob",	0, 0, 0,	    عدم,
			عدم,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    عدم,
			عدم,
			ret_channel,	    دنده_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    عدم,
			عدم,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    عدم,
			عدم,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    عدم,
			عدم,
			ret_دنده,	    دنده_FUNC(f_test_null_دنده)},
    {"test_null_list",	0, 0, 0,	    عدم,
			عدم,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    عدم,
			عدم,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    عدم,
			عدم,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_سېټ},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_شمېره,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, عدم,
			"expr",
			ret_شمېره,	    f_test_refcount},
    {"test_setmouse",	2, 2, 0,	    arg2_شمېره,
			"row, col",
			ret_void,	    f_test_setmouse},
    {"test_settime",	1, 1, FEARG_1,	    arg1_شمېره,
			"expr",
			ret_void,	    f_test_settime},
    {"test_srand_seed",	0, 1, FEARG_1,	    arg1_شمېره,
			"seed",
			ret_void,	    f_test_srand_تخم},
    {"test_unknown",	0, 0, 0,	    عدم,
			عدم,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    عدم,
			عدم,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_شمېره,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_شمېره_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_شمېره_any_dict,
			"time, callback, options",
			ret_شمېره,	    TIMER_FUNC(f_timer_پیل)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_شمېره,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    عدم,
			عدم,
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
    {"trim",		1, 3, FEARG_1,	    arg3_string_string_شمېره,
			"text, mask, dir",
			ret_string,	    f_trim},
    {"trunc",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_trunc},
    {"tuple2list",	1, 1, FEARG_1,	    arg1_tuple_any,
			"tuple",
			ret_list_any,	    f_tuple2list},
    {"type",		1, 1, FEARG_1|FE_X, عدم,
			"expr",
			ret_شمېره,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, عدم,
			"expr",
			ret_string,	    f_typename},
    {"undofile",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_string,	    f_undofile},
    {"undotree",	0, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_dict_any,	    f_undotree},
    {"uniq",		1, 3, FEARG_1,	    arg13_sortuniq,
			"list, func, dict",
			ret_لومړی_arg,	    f_uniq},
    {"uri_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_uridecode},
    {"uri_encode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_uriencode},
    {"utf16idx",	2, 4, FEARG_1,	    arg4_string_شمېره_bool_bool,
			"string, idx, countcc, charidx",
			ret_شمېره,	    f_utf16idx},
    {"values",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_member,    f_ارزښتونه},
    {"virtcol",		1, 3, FEARG_1,	    arg3_string_or_list_bool_شمېره,
			"expr, list, winid",
			ret_virtcol,	    f_virtcol},
    {"virtcol2col",	3, 3, FEARG_1,	    arg3_شمېره,
			"winid, lnum, col",
			ret_شمېره,	    f_virtcol2col},
    {"visualmode",	0, 1, 0,	    arg1_bool,
			"expr",
			ret_string,	    f_visualmode},
    {"wildmenumode",	0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    عدم,
			عدم,
			ret_void,	    f_wildtrigger},
    {"win_execute",	2, 3, FEARG_2,	    arg23_win_execute,
			"id, command, silent",
			ret_string,	    f_win_execute},
    {"win_findbuf",	1, 1, FEARG_1,	    arg1_شمېره,
			"bufnr",
			ret_list_شمېره,    f_win_findbuf},
    {"win_getid",	0, 2, FEARG_1,	    arg2_شمېره,
			"win, tab",
			ret_شمېره,	    f_win_getid},
    {"win_gettype",	0, 1, FEARG_1,	    arg1_شمېره,
			"nr",
			ret_string,	    f_win_gettype},
    {"win_gotoid",	1, 1, FEARG_1,	    arg1_شمېره,
			"expr",
			ret_شمېره_bool,    f_win_gotoid},
    {"win_id2tabwin",	1, 1, FEARG_1,	    arg1_شمېره,
			"expr",
			ret_list_شمېره,    f_win_id2tabwin},
    {"win_id2win",	1, 1, FEARG_1,	    arg1_شمېره,
			"expr",
			ret_شمېره,	    f_win_id2win},
    {"win_move_separator", 2, 2, FEARG_1,   arg2_شمېره,
			"nr, offset",
			ret_شمېره_bool,    f_win_move_separator},
    {"win_move_statusline", 2, 2, FEARG_1,  arg2_شمېره,
			"nr, offset",
			ret_شمېره_bool,    f_win_move_statusline},
    {"win_screenpos",	1, 1, FEARG_1,	    arg1_شمېره,
			"nr",
			ret_list_شمېره,    f_win_screenpos},
    {"win_splitmove",   2, 3, FEARG_1,	    arg3_شمېره_شمېره_dict,
			"nr, target, options",
			ret_شمېره_bool,    f_win_splitmove},
    {"winbufnr",	1, 1, FEARG_1,	    arg1_شمېره,
			"nr",
			ret_شمېره,	    f_winbufnr},
    {"wincol",		0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_شمېره,
			"nr",
			ret_شمېره,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_شمېره,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    عدم,
			عدم,
			ret_شمېره,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_شمېره,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    عدم,
			عدم,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    عدم,
			عدم,
			ret_dict_شمېره,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_شمېره,
			"nr",
			ret_شمېره,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    عدم,
			عدم,
			ret_dict_شمېره,    f_wordcount},
    {"writefile",	2, 3, FEARG_1,	    arg23_writefile,
			"object, fname, flags",
			ret_شمېره_bool,    f_writefile},
    {"xor",		2, 2, FEARG_1,	    arg2_شمېره,
			"expr1, expr2",
			ret_شمېره,	    f_xor},
};

/*
 * Return true if specified function allows a type as an argument.
 */
    ساكن صحيح
func_allows_type(صحيح idx)
{
    أرجع (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, صحيح idx)
{
    ساكن صحيح	intidx = -1;
    char_u	*name;

    إذا (idx == 0)
	intidx = -1;
    إذا (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	إذا (name != عدم)
	{
	    إذا (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_نمونه, 2) == 0)
		أرجع cat_مختاړی_varname('g', name);
	    أرجع name;
	}
    }
    إذا (++intidx < (صحيح)ARRAY_اوږدتیا(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	إذا (global_functions[intidx].f_func == عدم)
	    أرجع (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	إذا (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	أرجع IObuff;
    }

    أرجع عدم;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_بیان_name(expand_T *xp, صحيح idx)
{
    ساكن صحيح	intidx = -1;
    char_u	*name;

    إذا (idx == 0)
	intidx = -1;
    إذا (intidx < 0)
    {
	name = get_function_name(xp, idx);
	إذا (name != عدم)
	    أرجع name;
    }
    أرجع get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    ساكن صحيح
پیدا_کړه_دننی_func_opt(char_u *name, صحيح implemented)
{
    صحيح		لومړی = 0;
    صحيح		وروستی;
    صحيح		cmp;
    صحيح		x;

    وروستی = (صحيح)ARRAY_اوږدتیا(global_functions) - 1;

    // Find the function name in the table. Binary search.
    طالما (لومړی <= وروستی)
    {
	x = لومړی + ((بینښه)(وروستی - لومړی) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	إذا (cmp < 0)
	    وروستی = x - 1;
	وإلا إذا (cmp > 0)
	    لومړی = x + 1;
	وإلا إذا (implemented && global_functions[x].f_func == عدم)
	    اكسر;
	وإلا
	    أرجع x;
    }
    أرجع -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    صحيح
پیدا_کړه_دننی_func(char_u *name)
{
    أرجع پیدا_کړه_دننی_func_opt(name, TRUE);
}

    صحيح
has_دننی_func(char_u *name)
{
    أرجع پیدا_کړه_دننی_func_opt(name, TRUE) >= 0;
}

    ساكن صحيح
has_دننی_func_name(char_u *name)
{
    أرجع پیدا_کړه_دننی_func_opt(name, FALSE) >= 0;
}

    حرف *
دننی_func_name(صحيح idx)
{
    أرجع global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    صحيح
دننی_func_check_arg_types(
	type2_T	*types,
	صحيح	idx,
	صحيح	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    إذا (!(func_allows_type(idx)))
    {
	لكل (صحيح i = 0; i < argcount; ++i)
	    إذا (check_type_is_ارزښت(types[i].type_curr) == FAIL)
		أرجع FAIL;
    }

    ثابت argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    إذا (argchecks == عدم)
	أرجع OK;

    argcontext_T context;

    context.arg_شمېر = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    لكل (صحيح i = 0; i < argcount && argchecks[i] != عدم; ++i)
    {
	context.arg_idx = i;
	إذا (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    فراغ
دننی_func_get_argcount(صحيح idx, صحيح *argcount, صحيح *min_argcount)
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
دننی_func_ret_type(
	صحيح	    idx,
	صحيح	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = عدم;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    إذا (*decl_type == عدم)
	*decl_type = ret;
    current_type_gap = عدم;
    أرجع ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    صحيح
دننی_func_is_map(صحيح idx)
{
    أرجع global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    صحيح
check_دننی_func(صحيح idx, صحيح argcount)
{
    funcerror_T	    res;
    حرف	    *name;

    إذا (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    وإلا إذا (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    وإلا
	أرجع global_functions[idx].f_argtype & FEARG_ماسک;

    name = دننی_func_name(idx);
    إذا (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    وإلا
	semsg(_(e_not_enough_arguments_for_function_str), name);
    أرجع -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    ساكن صحيح
check_دلیلونه_for_type(صحيح idx, صحيح argcount, typval_T *argvars)
{
    إذا (!func_allows_type(idx))
    {
	لكل (صحيح i = 0; i < argcount; ++i)
	    إذا (check_typval_is_ارزښت(&argvars[i]) == FAIL)
		أرجع FAIL;
    }
    أرجع OK;
}

    funcerror_T
call_دننی_func(
	char_u	    *name,
	صحيح	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    صحيح i;

    i = پیدا_کړه_دننی_func(name);
    إذا (i < 0)
	أرجع FCERR_UNKNOWN;
    إذا (argcount < global_functions[i].f_min_argc)
	أرجع FCERR_TOOFEW;
    إذا (argcount > global_functions[i].f_max_argc)
	أرجع FCERR_TOOMANY;
    إذا (check_دلیلونه_for_type(i, argcount, argvars) == FAIL)
	أرجع FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    إذا (دننه_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    أرجع FCERR_NONE;
}

    فراغ
call_دننی_func_by_idx(
	صحيح	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    إذا (دننه_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_دننی_لاره(
	char_u	    *name,
	صحيح	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    صحيح		fi;
    typval_T	argv[MAX_FUNC_دلیلونه + 1];

    fi = پیدا_کړه_دننی_func(name);
    إذا (fi < 0)
	أرجع FCERR_UNKNOWN;
    إذا ((global_functions[fi].f_argtype & FEARG_ماسک) == 0)
	أرجع FCERR_NOTMETHOD;
    إذا (argcount + 1 < global_functions[fi].f_min_argc)
	أرجع FCERR_TOOFEW;
    إذا (argcount + 1 > global_functions[fi].f_max_argc)
	أرجع FCERR_TOOMANY;
    إذا (check_دلیلونه_for_type(fi, argcount, argvars) == FAIL)
	أرجع FCERR_OTHER;

    إذا ((global_functions[fi].f_argtype & FEARG_ماسک) == FEARG_2)
    {
	إذا (argcount < 1)
	    أرجع FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	لكل (صحيح i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    وإلا إذا ((global_functions[fi].f_argtype & FEARG_ماسک) == FEARG_3)
    {
	إذا (argcount < 2)
	    أرجع FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	لكل (صحيح i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    وإلا إذا ((global_functions[fi].f_argtype & FEARG_ماسک) == FEARG_4)
    {
	إذا (argcount < 3)
	    أرجع FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	لكل (صحيح i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    وإلا
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	لكل (صحيح i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    إذا (check_دلیلونه_for_type(fi, argcount + 1, argv) == FAIL)
	أرجع FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    أرجع FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    صحيح
non_zero_arg(typval_T *argvars)
{
    أرجع ((argvars[0].v_type == VAR_شمېره
		&& argvars[0].vval.v_شمېره != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_شمېره == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != عدم
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    ساكن فراغ
f_and(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->vval.v_شمېره = tv_get_شمېره_chk(&argvars[0], عدم)
					& tv_get_شمېره_chk(&argvars[1], عدم);
}

/*
 * "balloon_show()" function
 */
#که_تعریف_شوی FEAT_BEVAL
    ساكن فراغ
f_balloon_gettext(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    إذا (balloonEval == عدم)
	أرجع;

    إذا (balloonEval->پیغام == عدم)
	rettv->vval.v_string = عدم;
    وإلا
	rettv->vval.v_string = vim_strsave(balloonEval->پیغام);
}

    ساكن فراغ
f_balloon_show(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    إذا (balloonEval == عدم)
	أرجع;

    إذا (دننه_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.دننه_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, عدم,
		l == عدم || l->lv_len == 0 ? عدم : l);
    }
    وإلا
    {
	char_u *mesg;

	إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    أرجع;

	mesg = tv_get_string_chk(&argvars[0]);
	إذا (mesg != عدم)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? عدم : mesg, عدم);
    }
}

# إذا defined(FEAT_BEVAL_غړی)
    ساكن فراغ
f_balloon_وېشل(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    إذا (rettv_list_alloc(rettv) != OK)
	أرجع;

    char_u *پیغام;

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;
    پیغام = tv_get_string_chk(&argvars[0]);
    إذا (پیغام != عدم)
    {
	pumitem_T	*array;
	صحيح		اندازه = وېشل_پیغام(پیغام, &array);

	// Skip the first and last item, they are always empty.
	لكل (صحيح i = 1; i < اندازه - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_متن, -1);
	طالما (اندازه > 0)
	    vim_free(array[--اندازه].pum_متن);
	vim_free(array);
    }
}
# endif
#د_که_پای

/*
 * "base64_decode(string)" function
 */
    ساكن فراغ
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    إذا (check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (rettv_blob_alloc(rettv) == FAIL)
	أرجع;

    char_u *str = tv_get_string_chk(&argvars[0]);
    إذا (str != عدم)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    ساكن فراغ
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    إذا (check_for_blob_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    blob_T *blob = argvars->vval.v_blob;
    إذا (blob != عدم)
	rettv->vval.v_string =
	    base64_encode(blob->bv_ga.ga_معلومات, blob->bv_ga.ga_len);
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
    إذا (buf == عدم)
	semsg(_(e_invalid_بفر_name_str), tv_get_string(arg));
    أرجع buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    ساكن فراغ
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_شمېره = VVAL_TRUE;

    إذا (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	أرجع;

    إذا (strcmp((ثابت حرف *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[0]));
    وإلا
    {
	إذا (bindtextdomain((ثابت حرف *)argvars[0].vval.v_string, (ثابت حرف *)argvars[1].vval.v_string) == عدم)
	{
	    do_outofmem_پیغام((اوږد)0);
	    rettv->vval.v_شمېره = VVAL_FALSE;
	}
    }

    أرجع;
}

/*
 * "byte2line(byte)" function
 */
    ساكن فراغ
f_byte2line(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
#که_نه_تعریف_شوی FEAT_BYTEOFF
    rettv->vval.v_شمېره = -1;
#وإلا
    اوږد	boff = 0;

    إذا (دننه_vim9script() && check_for_شمېره_arg(argvars, 0) == FAIL)
	أرجع;

    boff = tv_get_شمېره(&argvars[0]) - 1;  // boff gets -1 on type error
    إذا (boff < 0)
	rettv->vval.v_شمېره = -1;
    وإلا
	rettv->vval.v_شمېره = ml_پیدا_کړه_کرښه_or_دننه_واټن(curbuf,
							  (linenr_T)0, &boff);
#د_که_پای
}

/*
 * "call(func, arglist [, dict])" function
 */
    ساكن فراغ
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = عدم;
    dict_T	*selfdict = عدم;
    char_u	*tofree = عدم;

    إذا (دننه_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    إذا (check_for_list_arg(argvars, 1) == FAIL)
	أرجع;
    إذا (argvars[1].vval.v_list == عدم)
	أرجع;

    إذا (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    وإلا إذا (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    وإلا
	func = tv_get_string(&argvars[0]);
    إذا (func == عدم || *func == NUL)
	أرجع;		// type error, empty name or null function

    إذا (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, عدم, FALSE, TFN_INT|TFN_QUIET);
	إذا (tofree == عدم)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    أرجع;
	}
	إذا (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_دلیلونه(tofree, STRLEN(tofree),
									&p);
	    إذا (s != عدم)
	    {
		vim_free(tofree);
		tofree = s;
	    }
	}
	func = tofree;
    }

    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	إذا (check_for_dict_arg(argvars, 2) == FAIL)
	    اذهب بشپړ;

	selfdict = argvars[2].vval.v_dict;
    }

    (فراغ)func_call(func, &argvars[1], partial, selfdict, rettv);

بشپړ:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    ساكن فراغ
f_changenr(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    rettv->vval.v_شمېره = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    ساكن فراغ
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (has_mbyte)
    {
	صحيح	utf8 = 0;

	إذا (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (صحيح)tv_get_bool_chk(&argvars[1], عدم);

	إذا (utf8)
	    rettv->vval.v_شمېره = utf_ptr2char(tv_get_string(&argvars[0]));
	وإلا
	    rettv->vval.v_شمېره = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    وإلا
	rettv->vval.v_شمېره = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    ساكن فراغ
get_ستون(typval_T *argvars, typval_T *rettv, صحيح charcol)
{
    colnr_T	ستون = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    صحيح		winchanged = FALSE;

    إذا (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_شمېره_arg(argvars, 1) == FAIL)
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((صحيح)tv_get_شمېره(&argvars[1]), &tp);
	إذا (wp == عدم || tp == عدم)
	    أرجع;

	إذا (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    أرجع;

	check_cursor();
	winchanged = TRUE;
    }

    صحيح fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    إذا (fp != عدم && fnum == curbuf->b_fnum)
    {
	إذا (fp->ستون == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    إذا (fp->lnum <= curbuf->b_ml.ml_کرښه_شمېر)
		ستون = ml_get_len(fp->lnum) + 1;
	    وإلا
		ستون = MAXCOL;
	}
	وإلا
	{
	    ستون = fp->ستون + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    إذا (virtual_فعال() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		إذا (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    صحيح		l;

		    إذا (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			ستون += l;
		}
	    }
	}
    }
    rettv->vval.v_شمېره = ستون;

    إذا (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    ساكن فراغ
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_ستون(argvars, rettv, TRUE);
}

    win_T *
get_optional_کړکۍ(typval_T *argvars, صحيح idx)
{
    win_T   *win = curwin;

    إذا (argvars[idx].v_type == VAR_UNKNOWN)
	أرجع curwin;

    win = پیدا_کړه_win_by_nr_or_id(&argvars[idx]);
    إذا (win == عدم)
    {
	emsg(_(e_invalid_کړکۍ_شمېره));
	أرجع عدم;
    }
    أرجع win;
}

/*
 * "col(string)" function
 */
    ساكن فراغ
f_ستون(typval_T *argvars, typval_T *rettv)
{
    get_ستون(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    ساكن فراغ
f_confirm(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*پیغام_2;
    char_u	*buttons = عدم;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    صحيح		def = 1;
    صحيح		type = VIM_GENERIC;
    char_u	*typestr;
    صحيح		error = FALSE;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_شمېره_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	أرجع;

    پیغام_2 = tv_get_string_chk(&argvars[0]);
    إذا (پیغام_2 == عدم)
	error = TRUE;
    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	إذا (buttons == عدم)
	    error = TRUE;
	إذا (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (صحيح)tv_get_شمېره_chk(&argvars[2], &error);
	    إذا (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		إذا (typestr == عدم)
		    error = TRUE;
		وإلا
		{
		    بدل (TOUPPER_ASC(*typestr))
		    {
			حالة 'E': type = VIM_ERROR; اكسر;
			حالة 'Q': type = VIM_QUESTION; اكسر;
			حالة 'I': type = VIM_INFO; اكسر;
			حالة 'W': type = VIM_WARNING; اكسر;
			حالة 'G': type = VIM_GENERIC; اكسر;
		    }
		}
	    }
	}
    }

    إذا (buttons == عدم || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    إذا (!error)
	rettv->vval.v_شمېره = do_dialog(type, عدم, پیغام_2, buttons,
							    def, عدم, FALSE);
#د_که_پای
}

/*
 * "copy()" function
 */
    ساكن فراغ
f_copy(typval_T *argvars, typval_T *rettv)
{
    توکی_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ساكن فراغ
سېټ_cursorpos(typval_T *argvars, typval_T *rettv, صحيح charcol)
{
    اوږد	lnum, ستون;
    اوږد	coladd = 0;
    صحيح		سېټ_curswant = TRUE;

    إذا (دننه_vim9script()
	    && (check_for_string_or_شمېره_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_شمېره_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_شمېره_arg(argvars, 2) == FAIL)))
	أرجع;

    rettv->vval.v_شمېره = -1;
    إذا (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	إذا (list2fpos(argvars, &pos, عدم, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_دلیل));
	    أرجع;
	}
	lnum = pos.lnum;
	ستون = pos.ستون;
	coladd = pos.coladd;
	إذا (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    سېټ_curswant = FALSE;
	}
    }
    وإلا إذا ((argvars[0].v_type == VAR_شمېره
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_شمېره
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	إذا (lnum < 0)
	    semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[0]));
	وإلا إذا (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	ستون = (اوږد)tv_get_شمېره_chk(&argvars[1], عدم);
	إذا (charcol)
	    ستون = buf_charidx_منزل_byteidx(curbuf, lnum, ستون) + 1;
	إذا (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (اوږد)tv_get_شمېره_chk(&argvars[2], عدم);
    }
    وإلا
    {
	emsg(_(e_invalid_دلیل));
	أرجع;
    }
    إذا (lnum < 0 || ستون < 0 || coladd < 0)
	أرجع;		// type error; errmsg already given
    إذا (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    إذا (ستون != MAXCOL && --ستون < 0)
	ستون = 0;
    curwin->w_cursor.ستون = ستون;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    إذا (has_mbyte)
	mb_adjust_cursor();

    curwin->w_سېټ_curswant = سېټ_curswant;
    rettv->vval.v_شمېره = 0;
}

/*
 * "cursor(lnum, col)" function, or
 * "cursor(list)"
 *
 * Moves the cursor to the specified line and column.
 * Returns 0 when the position could be set, -1 otherwise.
 */
    ساكن فراغ
f_cursor(typval_T *argvars, typval_T *rettv)
{
    سېټ_cursorpos(argvars, rettv, FALSE);
}

#که_تعریف_شوی MSWIN
/*
 * "debugbreak()" function
 */
    ساكن فراغ
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    صحيح		pid;

    rettv->vval.v_شمېره = FAIL;
    إذا (دننه_vim9script() && check_for_شمېره_arg(argvars, 0) == FAIL)
	أرجع;

    pid = (صحيح)tv_get_شمېره(&argvars[0]);
    إذا (pid == 0)
    {
	emsg(_(e_invalid_دلیل));
	أرجع;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    إذا (hProcess == عدم)
	أرجع;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_شمېره = OK;
}
#د_که_پای

/*
 * "deepcopy()" function
 */
    ساكن فراغ
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    إذا (check_for_opt_bool_arg(argvars, 1) == FAIL)
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], عدم);

    توکی_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    ساكن فراغ
f_did_filetype(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
    rettv->vval.v_شمېره = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    ساكن فراغ
f_echoraw(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    char_u *str;

    إذا (check_secure())
	أرجع;

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    str = tv_get_string_chk(&argvars[0]);
    إذا (str != عدم && *str != NUL)
    {
	بهر_str(str);
	بهر_flush();
    }
}

/*
 * "empty({expr})" function
 */
    ساكن فراغ
f_تشه(typval_T *argvars, typval_T *rettv)
{
    صحيح		n = FALSE;

    بدل (argvars[0].v_type)
    {
	حالة VAR_STRING:
	حالة VAR_FUNC:
	    n = argvars[0].vval.v_string == عدم
					  || *argvars[0].vval.v_string == NUL;
	    اكسر;
	حالة VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == عدم;
	    اكسر;
	حالة VAR_شمېره:
	    n = argvars[0].vval.v_شمېره == 0;
	    اكسر;
	حالة VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    اكسر;
	حالة VAR_LIST:
	    n = argvars[0].vval.v_list == عدم
					|| argvars[0].vval.v_list->lv_len == 0;
	    اكسر;
	حالة VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    اكسر;
	حالة VAR_DICT:
	    n = argvars[0].vval.v_dict == عدم
			|| argvars[0].vval.v_dict->dv_hashtab.ht_کارول_شوی == 0;
	    اكسر;
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	    n = argvars[0].vval.v_شمېره != VVAL_TRUE;
	    اكسر;
	حالة VAR_CLASS:
	    n = argvars[0].vval.v_class != عدم;
	    اكسر;
	حالة VAR_OBJECT:
	    n = object_تشه(argvars[0].vval.v_object);
	    اكسر;

	حالة VAR_BLOB:
	    n = argvars[0].vval.v_blob == عدم
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    اكسر;

	حالة VAR_دنده:
#که_تعریف_شوی FEAT_دنده_CHANNEL
	    n = argvars[0].vval.v_دنده == عدم
			   || argvars[0].vval.v_دنده->jv_حالت != دنده_STARTED;
	    اكسر;
#د_که_پای
	حالة VAR_CHANNEL:
#که_تعریف_شوی FEAT_دنده_CHANNEL
	    n = argvars[0].vval.v_channel == عدم
			       || !channel_is_open(argvars[0].vval.v_channel);
	    اكسر;
#د_که_پای
	حالة VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == عدم
		|| argvars[0].vval.v_typealias->ta_name == عدم
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    اكسر;

	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_INSTR:
	    دننی_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    اكسر;
    }

    rettv->vval.v_شمېره = n;
}

/*
 * "environ()" function
 */
    ساكن فراغ
f_environ(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
#إذا !defined(AMIGA)
    صحيح			i = 0;
    char_u		*ثبت, *ارزښت;
# إذا defined (MSWIN)
#  إذا !defined(_UCRT)
    خارجي wchar_t	**_wenviron;
#  endif
# وإلا
    خارجي حرف		**environ;
# endif

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;

# ifdef MSWIN
    إذا (*_wenviron == عدم)
	أرجع;
# وإلا
    إذا (*environ == عدم)
	أرجع;
# endif

    لكل (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	إذا ((p = (short_u *)_wenviron[i]) == عدم)
	    أرجع;
	ثبت = utf16_منزل_enc(p, عدم);
# وإلا
	إذا ((ثبت = (char_u *)environ[i]) == عدم)
	    أرجع;
	ثبت = vim_strsave(ثبت);
# endif
	إذا (ثبت == عدم) // out of memory
	    أرجع;
	إذا ((ارزښت = vim_strchr(ثبت, '=')) == عدم)
	{
	    vim_free(ثبت);
	    استمر;
	}
	*ارزښت++ = NUL;
	dict_جمع_کړه_string(rettv->vval.v_dict, (حرف *)ثبت, ارزښت);
	vim_free(ثبت);
    }
#د_که_پای
}

/*
 * "err_teapot()" function
 */
    ساكن فراغ
f_err_teapot(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	إذا (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    إذا (*skipwhite(s) == NUL)
		أرجع;
	}

	صحيح err = FALSE;
	صحيح do_503 = eval_بیان_منزل_bool(&argvars[0], &err);
	إذا (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_شته));
	    أرجع;
	}
    }

    emsg(_(e_im_a_teapot));
}

/*
 * "escape({string}, {chars})" function
 */
    ساكن فراغ
f_escape(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->vval.v_string = vim_strsave_escaped(tv_get_string(&argvars[0]),
					 tv_get_string_buf(&argvars[1], buf));
    rettv->v_type = VAR_STRING;
}

/*
 * "eval()" function
 */
    ساكن فراغ
f_eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*s, *p;

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    s = tv_get_string_chk(&argvars[0]);
    إذا (s != عدم)
	s = skipwhite(s);

    p = s;
    إذا (s == عدم || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	إذا (p != عدم && !aborting())
	    semsg(_(e_invalid_افاده_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_شمېره;
	rettv->vval.v_شمېره = 0;
    }
    وإلا إذا (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    ساكن فراغ
f_eventhandler(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    rettv->vval.v_شمېره = vgetc_busy || ننوت_busy;
}

ساكن garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    فراغ
execute_redir_str(char_u *ارزښت, صحيح ارزښت_len)
{
    صحيح		len;

    إذا (ارزښت_len == -1)
	len = (صحيح)STRLEN(ارزښت);	// Append the entire string
    وإلا
	len = ارزښت_len;		// Append only "value_len" characters
    إذا (ga_grow(&redir_execute_ga, len) == FAIL)
	أرجع;

    mch_memmove((حرف *)redir_execute_ga.ga_معلومات
	    + redir_execute_ga.ga_len, ارزښت, len);
    redir_execute_ga.ga_len += len;
}

#إذا defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    ساكن char_u *
get_str_کرښه(
    صحيح	    c ناکارول_شوی,
    فراغ    *cookie,
    صحيح	    indent ناکارول_شوی,
    getline_opt_T options ناکارول_شوی)
{
    char_u	*پیل = *(char_u **)cookie;
    char_u	*کرښه;
    char_u	*p;

    p = پیل;
    إذا (p == عدم || *p == NUL)
	أرجع عدم;
    p = vim_strchr(p, '\n');
    إذا (p == عدم)
	کرښه = vim_strsave(پیل);
    وإلا
    {
	کرښه = vim_strnsave(پیل, p - پیل);
	p++;
    }

    *(char_u **)cookie = p;
    أرجع کرښه;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    فراغ
execute_cmds_from_string(char_u *str)
{
    do_cmdline(عدم, get_str_کرښه, (فراغ *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#د_که_پای

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_کرښه(
    صحيح	    c ناکارول_شوی,
    فراغ    *cookie,
    صحيح	    indent ناکارول_شوی,
    getline_opt_T options ناکارول_شوی)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *توکی = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    إذا (توکی == عدم)
	أرجع عدم;
    s = tv_get_string_buf_chk(&توکی->li_tv, buf);
    *p = توکی->li_بل;
    أرجع s == عدم ? عدم : vim_strsave(s);
}

/*
 * "execute()" function
 */
    فراغ
execute_common(typval_T *argvars, typval_T *rettv, صحيح arg_off)
{
    char_u	*cmd = عدم;
    list_T	*list = عدم;
    صحيح		save_پیغام_silent = پیغام_silent;
    صحيح		save_emsg_silent = emsg_silent;
    صحيح		save_emsg_noredir = emsg_noredir;
    صحيح		save_redir_execute = redir_execute;
    صحيح		save_redir_off = redir_off;
    garray_T	save_ga;
    صحيح		save_پیغام_ستون = پیغام_ستون;
    صحيح		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    صحيح		echo_output = FALSE;

    rettv->vval.v_string = عدم;
    rettv->v_type = VAR_STRING;

    إذا (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	إذا (list == عدم || list->lv_len == 0)
	    // empty list, no commands, empty output
	    أرجع;
	++list->lv_refcount;
    }
    وإلا إذا (argvars[arg_off].v_type == VAR_دنده
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_ارزښت_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	أرجع;
    }
    وإلا
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	إذا (cmd == عدم)
	    أرجع;
    }

    إذا (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      دننه_vim9script());

	إذا (s == عدم)
	    أرجع;
	إذا (*s == NUL)
	    echo_output = TRUE;
	إذا (STRNCMP(s, "silent", 6) == 0)
	    ++پیغام_silent;
	إذا (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    وإلا
	++پیغام_silent;

    إذا (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, حجم(حرف), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    إذا (!echo_output)
	پیغام_ستون = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    إذا (cmd != عدم)
	do_cmdline_cmd(cmd);
    وإلا
    {
	listitem_T	*توکی;

	CHECK_LIST_MATERIALIZE(list);
	توکی = list->lv_لومړی;
	do_cmdline(عدم, get_list_کرښه, (فراغ *)&توکی,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    إذا (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((حرف *)redir_execute_ga.ga_معلومات)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_معلومات;
    }
    وإلا
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = عدم;
    }
    پیغام_silent = save_پیغام_silent;
    emsg_silent = save_emsg_silent;
    emsg_noredir = save_emsg_noredir;

    redir_execute = save_redir_execute;
    إذا (redir_execute)
	redir_execute_ga = save_ga;
    redir_off = save_redir_off;

    // "silent reg" or "silent echo x" leaves msg_col somewhere in the line.
    إذا (echo_output)
	// When not working silently: put it in column zero.  A following
	// "echon" will overwrite the message, unavoidably.
	پیغام_ستون = 0;
    وإلا
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	پیغام_ستون = save_پیغام_ستون;
}

/*
 * "execute()" function
 */
    ساكن فراغ
f_execute(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    فراغ
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحيح		n = FALSE;

    إذا (دننه_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	أرجع;

    p = tv_get_string(&argvars[0]);
    إذا (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	إذا (mch_getenv(p + 1) != عدم)
	    n = TRUE;
	وإلا
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    إذا (p != عدم && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    وإلا إذا (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, عدم, TRUE) == OK);
	إذا (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    وإلا إذا (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    وإلا إذا (*p == '?')			// internal function only
    {
	n = has_دننی_func_name(p + 1);
    }
    وإلا إذا (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    وإلا إذا (*p == '#')
    {
	إذا (p[1] == '#')
	    n = autocmd_supported(p + 2);
	وإلا
	    n = au_exists(p + 1);
    }
    وإلا				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_شمېره = n;
}

    ساكن فراغ
f_exists_compiled(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
    emsg(_(e_exists_compiled_can_only_be_کارول_شوی_دننه_def_function));
}

/*
 * "expand()" function
 */
    ساكن فراغ
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    size_t	len;
    صحيح		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    صحيح		error = FALSE;
    char_u	*پایله;
#که_تعریف_شوی BACKSLASH_دننه_FILENAME
    char_u	*p_csl_save = p_csl;
#د_که_پای

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	أرجع;

#که_تعریف_شوی BACKSLASH_دننه_FILENAME
    // avoid using 'completeslash' here
    p_csl = تشه_option;
#د_که_پای

    rettv->v_type = VAR_STRING;
    إذا (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_سېټ(rettv, عدم);

    s = tv_get_string(&argvars[0]);
    إذا (*s == '%' || *s == '#' || *s == '<')
    {
	حرف	*errormsg = عدم;

	إذا (p_verbose == 0)
	    ++emsg_off;
	پایله = eval_vars(s, s, &len, عدم, &errormsg, عدم, FALSE);
	إذا (p_verbose == 0)
	    --emsg_off;
	وإلا إذا (errormsg != عدم)
	    emsg(errormsg);
	إذا (rettv->v_type == VAR_LIST)
	{
	    إذا (rettv_list_alloc(rettv) == OK && پایله != عدم)
		list_append_string(rettv->vval.v_list, پایله, -1);
	    vim_free(پایله);
	}
	وإلا
	    rettv->vval.v_string = پایله;
    }
    وإلا
    {
	// When the optional second argument is non-zero, don't remove matches
	// for 'wildignore' and don't put matches for 'suffixes' at the end.
	إذا (argvars[1].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	إذا (!error)
	{
	    ExpandInit(&xpc);
	    xpc.xp_context = EXPAND_FILES;
	    إذا (p_wic)
		options += WILD_ICASE;
	    إذا (rettv->v_type == VAR_STRING)
		rettv->vval.v_string = ExpandOne(&xpc, s, عدم,
							   options, WILD_ALL);
	    وإلا إذا (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, عدم, options, WILD_ALL_KEEP);
		لكل (صحيح i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	وإلا
	    rettv->vval.v_string = عدم;
    }
#که_تعریف_شوی BACKSLASH_دننه_FILENAME
    p_csl = p_csl_save;
#د_که_پای
}

/*
 * "expandcmd()" function
 * Expand all the special characters in a command string.
 */
    ساكن فراغ
f_expandcmd(typval_T *argvars, typval_T *rettv)
{
    exarg_T	eap;
    char_u	*cmdstr;
    حرف	*errormsg = عدم;
    صحيح		emsgoff = TRUE;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_ډګر(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = عدم;
    eap.cmdidx = CMD_USER;

    إذا (emsgoff)
	++emsg_off;
    إذا (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	إذا (!emsgoff && errormsg != عدم && *errormsg != NUL)
	    emsg(errormsg);
    إذا (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    ساكن فراغ
f_feedkeys(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    صحيح		remap = TRUE;
    صحيح		ورننباسه = FALSE;
    char_u	*کلیګانې, *flags;
    char_u	nbuf[NUMBUFLEN];
    صحيح		typed = FALSE;
    صحيح		execute = FALSE;
    صحيح		context = FALSE;
    صحيح		dangerous = FALSE;
    صحيح		lowlevel = FALSE;

    // This is not allowed in the sandbox.  If the commands would still be
    // executed in the sandbox it would be OK, but it probably happens later,
    // when "sandbox" is no longer set.
    إذا (check_secure())
	أرجع;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    کلیګانې = tv_get_string(&argvars[0]);

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	لكل ( ; *flags != NUL; ++flags)
	{
	    بدل (*flags)
	    {
		حالة 'n': remap = FALSE; اكسر;
		حالة 'm': remap = TRUE; اكسر;
		حالة 't': typed = TRUE; اكسر;
		حالة 'i': ورننباسه = TRUE; اكسر;
		حالة 'x': execute = TRUE; اكسر;
		حالة 'c': context = TRUE; اكسر;
		حالة '!': dangerous = TRUE; اكسر;
		حالة 'L': lowlevel = TRUE; اكسر;
	    }
	}
    }

    إذا (*کلیګانې != NUL || execute)
    {
	إذا (lowlevel
#که_تعریف_شوی FEAT_VTP
		&& (!is_غړی_win32()
		    || (کلیګانې[0] == 3 && ctrl_c_interrupts && typed))
#د_که_پای
	   )
	{
#که_تعریف_شوی USE_ننوت_BUF
	    ch_log(عدم, "feedkeys() lowlevel: %s", کلیګانې);

	    صحيح len = (صحيح)STRLEN(کلیګانې);
	    لكل (صحيح idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		إذا (کلیګانې[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		جمع_کړه_منزل_ننوت_buf(کلیګانې + idx, 1);
	    }
#وإلا
	    emsg(_(e_lowlevel_ننوت_not_supported));
#د_که_پای
	}
	وإلا
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *کلیګانې_esc = vim_strsave_escape_csi(کلیګانې);
	    إذا (کلیګانې_esc == عدم)
		أرجع;

	    ch_log(عدم, "feedkeys(%s): %s", typed ? "typed" : "", کلیګانې);

	    ins_typebuf(کلیګانې_esc, (remap ? REMAP_YES : REMAP_NONE),
				   ورننباسه ? 0 : typebuf.tb_len, !typed, FALSE);
	    إذا (vgetc_busy
#که_تعریف_شوی FEAT_TIMERS
		    || timer_busy
#د_که_پای
		    || ننوت_busy)
		typebuf_was_filled = TRUE;

	    vim_free(کلیګانې_esc);
	}

	إذا (execute)
	{
	    صحيح		save_پیغام_scroll = پیغام_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    پیغام_scroll = FALSE;

	    ch_log(عدم, "feedkeys() executing");

	    إذا (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_نسخه = 0;
	    }

	    إذا (!dangerous)
	    {
		++ex_نورمال_busy;
		++دننه_feedkeys;
	    }
	    ++allow_osc_کلی;
	    exec_نورمال(TRUE, lowlevel, TRUE);
	    --allow_osc_کلی;
	    إذا (!dangerous)
	    {
		--ex_نورمال_busy;
		--دننه_feedkeys;
	    }

	    پیغام_scroll |= save_پیغام_scroll;

	    إذا (context)
		current_sctx = save_sctx;
	}
    }
}

/*
 * "fnameescape({string})" function
 */
    ساكن فراغ
f_fnameescape(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    ساكن فراغ
f_foreground(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
#که_تعریف_شوی FEAT_GUI
    إذا (gui.دننه_use)
    {
	gui_mch_سېټ_foreground();
	أرجع;
    }
#د_که_پای
#إذا defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_سېټ_foreground();
#د_که_پای
}

/*
 * "function()" function
 * "funcref()" function
 */
    ساكن فراغ
common_function(typval_T *argvars, typval_T *rettv, صحيح is_funcref)
{
    char_u	*s;
    char_u	*name;
    صحيح		use_string = FALSE;
    partial_T   *arg_pt = عدم;
    char_u	*trans_name = عدم;
    صحيح		is_global = FALSE;
    char_u	*پیل_bracket = عدم;

    إذا (دننه_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	أرجع;

    إذا (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    وإلا إذا (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != عدم)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    وإلا
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    إذا (s == عدم)
    {
	semsg(_(e_invalid_دلیل_str), "NULL");
	أرجع;
    }

    إذا ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == عدم) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, عدم);
	إذا (*name == '<')
	{
	    // generic function
	    پیل_bracket = name;
	    إذا (skip_generic_func_type_دلیلونه(&name) == FAIL)
		اذهب theend;
	}
	إذا (*name != NUL)
	    s = عدم;
    }

    إذا (s == عدم || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == عدم))
	semsg(_(e_invalid_دلیل_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    وإلا إذا (trans_name != عدم && (is_funcref
			 ? پیدا_کړه_func(trans_name, is_global) == عدم
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    وإلا
    {
	صحيح	dict_idx = 0;
	صحيح	arg_idx = 0;
	list_T	*list = عدم;

	إذا (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	وإلا إذا (trans_name != عدم && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	وإلا
	    name = vim_strsave(s);

	إذا (argvars[1].v_type != VAR_UNKNOWN)
	{
	    إذا (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    وإلا إذا (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    وإلا
		// function(name, [args])
		arg_idx = 1;
	    إذا (dict_idx > 0)
	    {
		إذا (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    اذهب theend;
		}
		إذا (argvars[dict_idx].vval.v_dict == عدم)
		    dict_idx = 0;
	    }
	    إذا (arg_idx > 0)
	    {
		إذا (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_دوهم_دلیل_د_function_must_be_list_or_dict));
		    vim_free(name);
		    اذهب theend;
		}
		list = argvars[arg_idx].vval.v_list;
		إذا (list == عدم || list->lv_len == 0)
		    arg_idx = 0;
		وإلا إذا (list->lv_len > MAX_FUNC_دلیلونه)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    اذهب theend;
		}
	    }
	}
	إذا (dict_idx > 0 || arg_idx > 0 || arg_pt != عدم || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    إذا (pt == عدم)
		vim_free(name);
	    وإلا
	    {
		إذا (arg_idx > 0 || (arg_pt != عدم && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    صحيح		i = 0;
		    صحيح		arg_len = 0;
		    صحيح		lv_len = 0;

		    إذا (arg_pt != عدم)
			arg_len = arg_pt->pt_argc;
		    إذا (list != عدم)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    إذا (pt->pt_argv == عدم)
		    {
			vim_free(pt);
			vim_free(name);
			اذهب theend;
		    }
		    لكل (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    إذا (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_توکي(list, li)
			    copy_tv(&li->li_tv, &pt->pt_argv[i++]);
		    }
		}

		// For "function(dict.func, [], dict)" and "func" is a partial
		// use "dict".  That is backwards compatible.
		إذا (dict_idx > 0)
		{
		    // The dict is bound explicitly, pt_auto is FALSE.
		    pt->pt_dict = argvars[dict_idx].vval.v_dict;
		    ++pt->pt_dict->dv_refcount;
		}
		وإلا إذا (arg_pt != عدم)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    إذا (pt->pt_dict != عدم)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    إذا (pt->pt_obj != عدم)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		إذا (arg_pt != عدم && arg_pt->pt_func != عدم)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		وإلا إذا (is_funcref)
		{
		    pt->pt_func = پیدا_کړه_func(trans_name, is_global);
		    إذا (IS_GENERIC_FUNC(pt->pt_func) && پیل_bracket != عدم)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&پیل_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		وإلا
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		إذا (arg_pt != عدم)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	وإلا
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    إذا (پیل_bracket == عدم)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    وإلا
	    {
		// generic function
		size_t len = STRLEN(name) + STRLEN(پیل_bracket);
		rettv->vval.v_string = alloc(len + 1);
		إذا (rettv->vval.v_string != عدم)
		{
		    STRCPY(rettv->vval.v_string, name);
		    STRCAT(rettv->vval.v_string, پیل_bracket);
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
    ساكن فراغ
f_funcref(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, TRUE);
}

/*
 * "function()" function
 */
    ساكن فراغ
f_function(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, FALSE);
}

/*
 * "garbagecollect()" function
 */
    ساكن فراغ
f_garbagecollect(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    إذا (دننه_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	أرجع;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    إذا (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
	garbage_collect_at_exit = TRUE;
}

/*
 * "get()" function
 */
    ساكن فراغ
f_get(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    list_T	*l;
    tuple_T	*tuple;
    dictitem_T	*di;
    dict_T	*d;
    typval_T	*tv = عدم;
    صحيح		what_is_dict = FALSE;

    إذا (argvars[0].v_type == VAR_BLOB)
    {
	صحيح error = FALSE;
	صحيح idx = tv_get_شمېره_chk(&argvars[1], &error);

	إذا (!error)
	{
	    rettv->v_type = VAR_شمېره;
	    إذا (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    إذا (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_شمېره = -1;
	    وإلا
	    {
		rettv->vval.v_شمېره = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    وإلا إذا (argvars[0].v_type == VAR_LIST)
    {
	إذا ((l = argvars[0].vval.v_list) != عدم)
	{
	    صحيح		error = FALSE;

	    li = list_پیدا_کړه(l, (اوږد)tv_get_شمېره_chk(&argvars[1], &error));
	    إذا (!error && li != عدم)
		tv = &li->li_tv;
	}
    }
    وإلا إذا (argvars[0].v_type == VAR_TUPLE)
    {
	إذا ((tuple = argvars[0].vval.v_tuple) != عدم)
	{
	    صحيح		error = FALSE;
	    اوږد	idx;

	    idx = (اوږد)tv_get_شمېره_chk(&argvars[1], &error);
	    إذا (!error)
		tv = tuple_پیدا_کړه(tuple, idx);
	}
    }
    وإلا إذا (argvars[0].v_type == VAR_DICT)
    {
	إذا ((d = argvars[0].vval.v_dict) != عدم)
	{
	    di = dict_پیدا_کړه(d, tv_get_string(&argvars[1]), -1);
	    إذا (di != عدم)
		tv = &di->di_tv;
	}
    }
    وإلا إذا (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	إذا (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	وإلا
	{
	    CLEAR_ډګر(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	إذا (pt != عدم)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    إذا (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		إذا (name == عدم)
		    rettv->vval.v_string = عدم;
		وإلا
		{
		    إذا (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    إذا (*what == 'n' && pt->pt_name == عدم
							&& pt->pt_func != عدم)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    وإلا إذا (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		إذا (pt->pt_dict != عدم)
		    rettv_dict_سېټ(rettv, pt->pt_dict);
	    }
	    وإلا إذا (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		إذا (rettv_list_alloc(rettv) == OK)
		{
		    صحيح i;

		    لكل (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    وإلا إذا (STRCMP(what, "arity") == 0)
	    {
		صحيح required = 0, optional = 0, varargs = FALSE;
		char_u *name = partial_name(pt);

		get_func_arity(name, &required, &optional, &varargs);

		rettv->v_type = VAR_DICT;
		إذا (rettv_dict_alloc(rettv) == OK)
		{
		    dict_T *dict = rettv->vval.v_dict;

		    // Take into account the arguments of the partial, if any.
		    // Note that it is possible to supply more arguments than the function
		    // accepts.
		    إذا (pt->pt_argc >= required + optional)
			required = optional = 0;
		    وإلا إذا (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    وإلا
			required -= pt->pt_argc;

		    dict_جمع_کړه_شمېره(dict, "required", required);
		    dict_جمع_کړه_شمېره(dict, "optional", optional);
		    dict_جمع_کړه_bool(dict, "varargs", varargs);
		}
	    }
	    وإلا
		semsg(_(e_invalid_دلیل_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    إذا (!what_is_dict)
		أرجع;
	}
    }
    وإلا
	semsg(_(e_دلیل_د_str_must_be_list_tuple_dictionary_or_blob), "get()");

    إذا (tv == عدم)
    {
	إذا (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    وإلا
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
    ساكن فراغ
f_getbgcolor(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	إذا (gui.دننه_use)
	    try_hl = true;
# endif
# ifdef FEAT_TERMGUICOLORS
	إذا (p_tgc)
	    try_hl = true;
# endif
	إذا (try_hl)
	{
	    guicolor_T	fg, bg;
	    صحيح		hl_id = syn_name2id((char_u *)"Normal");

	    إذا (hl_id > 0)
	    {
		(فراغ)syn_id2colors(hl_id, &fg, &bg);
		إذا (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_شمېره(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_شمېره(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_شمېره(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    أرجع;
		}
	    }
	}
    }
#د_که_پای
#که_تعریف_شوی FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	إذا (غړی_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_شمېره(rettv->vval.v_list, (varnumber_T)r);
	    list_append_شمېره(rettv->vval.v_list, (varnumber_T)g);
	    list_append_شمېره(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#د_که_پای
}

/*
 * "getcellpixels()" function
 */
    ساكن فراغ
f_getcellpixels(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

#إذا defined(FEAT_GUI)
    إذا (gui.دننه_use)
    {
	// success pixel size and no gui.
	list_append_شمېره(rettv->vval.v_list, (varnumber_T)gui.char_پلنوالی);
	list_append_شمېره(rettv->vval.v_list, (varnumber_T)gui.char_لوړوالی);
    }
    وإلا
#د_که_پای
    {
#إذا defined(UNIX) || defined(MSWIN)
	list_append_شمېره(rettv->vval.v_list, (varnumber_T)cell_پلنوالی);
	list_append_شمېره(rettv->vval.v_list, (varnumber_T)cell_لوړوالی);
#وإلا
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_شمېره(rettv->vval.v_list, (varnumber_T)-1);
	list_append_شمېره(rettv->vval.v_list, (varnumber_T)-1);
#د_که_پای
    }

}

/*
 * "getchangelist()" function
 */
    ساكن فراغ
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    صحيح		i;
    list_T	*l;
    dict_T	*d;
    صحيح		changelistindex;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (دننه_vim9script() && check_for_opt_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    وإلا
	buf = tv_get_buf_from_arg(&argvars[0]);
    إذا (buf == عدم)
	أرجع;

    l = list_alloc();
    إذا (l == عدم)
	أرجع;
    إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	أرجع;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    إذا (buf == curwin->w_بفر)
    {
	changelistindex = curwin->w_changelistidx;
    }
    وإلا
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    إذا (wip->wi_win == curwin)
		اكسر;
	changelistindex = wip != عدم ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_شمېره(rettv->vval.v_list, (varnumber_T)changelistindex);

    لكل (i = 0; i < buf->b_changelistlen; ++i)
    {
	إذا (buf->b_changelist[i].lnum == 0)
	    استمر;
	إذا ((d = dict_alloc()) == عدم)
	    أرجع;
	إذا (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    أرجع;
	}
	dict_جمع_کړه_شمېره(d, "lnum", (اوږد)buf->b_changelist[i].lnum);
	dict_جمع_کړه_شمېره(d, "col", (اوږد)buf->b_changelist[i].ستون);
	dict_جمع_کړه_شمېره(d, "coladd", (اوږد)buf->b_changelist[i].coladd);
    }
}

    ساكن فراغ
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    صحيح		getcurpos,
    صحيح		charcol)
{
    pos_T	*fp = عدم;
    pos_T	pos;
    win_T	*wp = curwin;
    list_T	*l;
    صحيح		fnum = -1;

    إذا (rettv_list_alloc(rettv) == OK)
    {
	l = rettv->vval.v_list;
	إذا (getcurpos)
	{
	    إذا (argvars[0].v_type != VAR_UNKNOWN)
	    {
		wp = پیدا_کړه_win_by_nr_or_id(&argvars[0]);
		إذا (wp != عدم)
		    fp = &wp->w_cursor;
	    }
	    وإلا
		fp = &curwin->w_cursor;
	    إذا (fp != عدم && charcol)
	    {
		pos = *fp;
		pos.ستون =
		    buf_byteidx_منزل_charidx(wp->w_بفر, pos.lnum, pos.ستون);
		fp = &pos;
	    }
	}
	وإلا
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	إذا (fnum != -1)
	    list_append_شمېره(l, (varnumber_T)fnum);
	وإلا
	    list_append_شمېره(l, (varnumber_T)0);
	list_append_شمېره(l, (fp != عدم) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_شمېره(l, (fp != عدم)
		     ? (varnumber_T)(fp->ستون == MAXCOL ? MAXCOL : fp->ستون + 1)
							    : (varnumber_T)0);
	list_append_شمېره(l, (fp != عدم) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	إذا (getcurpos)
	{
	    صحيح	    save_سېټ_curswant = curwin->w_سېټ_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    إذا (wp == curwin)
		update_curswant();
	    list_append_شمېره(l, wp == عدم ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    إذا (wp == curwin && save_سېټ_curswant)
	    {
		curwin->w_سېټ_curswant = save_سېټ_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    وإلا
	rettv->vval.v_شمېره = FALSE;
}

/*
 * "getcharpos()" function
 */
    ساكن فراغ
f_getcharpos(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    ساكن فراغ
f_getcharsearch(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    إذا (rettv_dict_alloc(rettv) != OK)
	أرجع;

    dict_T *dict = rettv->vval.v_dict;

    dict_جمع_کړه_string(dict, "char", وروستی_csearch());
    dict_جمع_کړه_شمېره(dict, "forward", وروستی_csearch_forward());
    dict_جمع_کړه_شمېره(dict, "until", وروستی_csearch_until());
}

/*
 * "getenv()" function
 */
    ساكن فراغ
f_getenv(typval_T *argvars, typval_T *rettv)
{
    صحيح	    mustfree = FALSE;
    char_u  *p;

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    إذا (p == عدم)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_شمېره = VVAL_NULL;
	أرجع;
    }
    إذا (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    ساكن فراغ
f_getfontname(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (دننه_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	أرجع;

#که_تعریف_شوی FEAT_GUI
    إذا (gui.دننه_use)
    {
	GuiFont font;
	char_u	*name = عدم;

	إذا (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	وإلا
	{
	    name = tv_get_string(&argvars[0]);
	    إذا (STRCMP(name, "*") == 0)	    // don't use font dialog
		أرجع;
	    font = gui_mch_get_font(name, FALSE);
	    إذا (font == NOFONT)
		أرجع;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	إذا (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#د_که_پای
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
ساكن بنية
{
    argcheck_T	func;
    حرف	*types;	    // separated by commas
} argcheck_types[] =
{
    {arg_any,			"any"},
    {arg_blob,			"blob"},
    {arg_bool,			"bool,number"},
    {arg_bool_or_dict_any,	"bool,number,dict<any>"},
    {arg_bool_or_nr,		"bool,number"},
    {arg_بفر,		"string,number"},
    {arg_بفر_or_dict_any,	"string,number,dict<any>"},
    {arg_chan_or_دنده,		"channel,job"},
    {arg_cursor1,		"number,string,list<any>"},
    {arg_dict_any,		"dict<any>"},
    {arg_dict_any_or_string,	"dict<any>,string"},
    {arg_extend3,		"number,string"},
    {arg_filter_func,		"string,func"},
    {arg_float_or_nr,		"float,number"},
    {arg_foreach_func,		"string,func"},
    {arg_get1,			"blob,list<any>,tuple<any>,dict<any>,func"},
    {arg_توکی_د_prev,		"any"},
    {arg_دنده,			"job"},
    {arg_len1,
	    "string,number,blob,list<any>,tuple<any>,dict<any>,object<any>"},
    {arg_list_any,		"list<any>"},
    {arg_list_any_mod,		"list<any>"},
    {arg_list_شمېره,		"list<number>"},
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
    {arg_شمېره,		"number"},
    {arg_object,		"object<any>"},
    {arg_remove2,		"number,string"},
    {arg_repeat1,		"string,number,blob,list<any>,tuple<any>"},
    {arg_سرچپه,		"list<any>,tuple<any>,blob,string"},
    {arg_same_as_prev,		"any"},
    {arg_same_struct_as_prev,	"any"},
    {arg_slice1,		"list<any>,tuple<any>,blob,string"},
    {arg_ترتیبول_how,		"string,func"},
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
    ساكن list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    حرف	*types = "any";

    إذا (l == عدم)
	أرجع عدم;
    لكل (صحيح i = 0; i < (صحيح)ARRAY_اوږدتیا(argcheck_types); ++i)
	إذا (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    اكسر;
	}
    لكل (حرف *p = types; ; )
    {
	حرف	*پای_ټکی = (حرف *)vim_strchr((char_u *)p, ',');
	صحيح	len = پای_ټکی == عدم ? (صحيح)STRLEN(p) : (صحيح)(پای_ټکی - p);

	إذا (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    أرجع عدم;
	}
	إذا (پای_ټکی == عدم)
	    اكسر;
	p = پای_ټکی + 1;
    }
    أرجع l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    ساكن صحيح
getinfo_argtypes(
	list_T			*l,
	ثابت funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    صحيح		argcount = 0;

    إذا (l != عدم)
	FOR_ALL_LIST_توکي(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    إذا (s == عدم)
		أرجع -1;
	    إذا (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_دلیلونه)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		أرجع -1;
	    }
	    type = parse_type(&p, type_gap, عدم, عدم, TRUE);
	    إذا (type == عدم)
		أرجع -1;
	    إذا (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		أرجع -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    لكل ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_دلیلونه; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    أرجع argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    ساكن حرف *
builtin_ret_type_name(
	صحيح	    idx,
	صحيح	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	حرف	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = دننی_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    أرجع type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
تعريفنوع تعداد {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_ډول_T;

ساكن بنية
{
    حرف		*name;
    getinfo_ډول_T	ډول;
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
    ساكن فراغ
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_ډول_T	ډول;
    صحيح			ki;
    صحيح			idx;
    ثابت funcentry_T	*fe;
    dict_T		*d;
    list_T		*دلیلونه;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_دلیلونه];
    list_T		*argtypes_list = عدم;
    صحيح			argcount;
    صحيح			vim9 = -1;	// -1: the context of the caller
    حرف		*ret_name;
    حرف		*tofree;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    إذا (دننه_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;
    إذا (check_for_opt_dict_arg(argvars, 2) == FAIL)
	أرجع;

    kindname = tv_get_string(&argvars[0]);
    لكل (ki = 0; ki < (صحيح)ARRAY_اوږدتیا(getinfo_kinds); ++ki)
	إذا (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    اكسر;
    إذا (ki == (صحيح)ARRAY_اوږدتیا(getinfo_kinds))
    {
	semsg(_(e_invalid_دلیل_str), kindname);
	أرجع;
    }
    ډول = getinfo_kinds[ki].ډول;
    name = tv_get_string_buf(&argvars[1], namebuf);

    إذا (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != عدم)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	صحيح	    todo = (صحيح)opts->dv_hashtab.ht_کارول_شوی;

	FOR_ALL_HASHTAB_توکي(&opts->dv_hashtab, hi, todo)
	{
	    إذا (HASHITEM_تشه(hi))
		استمر;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    إذا (STRCMP(di->di_کلی, "argtypes") == 0 && ډول == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    وإلا إذا (STRCMP(di->di_کلی, "vim9") == 0 && ډول == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    وإلا
	    {
		semsg(_(e_invalid_دلیل_str), di->di_کلی);
		أرجع;
	    }
	}
    }
    إذا (ډول == GI_VIMVAR)
    {
	إذا (STRNCMP(name, "v:", 2) == 0)
	    (فراغ)vim_var_info(name + 2, rettv->vval.v_dict);
	أرجع;
    }
    إذا (ډول == GI_COMMAND)
    {
	(فراغ)ex_command_info(name, vim9, rettv->vval.v_dict);
	أرجع;
    }
    إذا (ډول == GI_OPTION)
    {
	(فراغ)option_info(name, rettv->vval.v_dict);
	أرجع;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = پیدا_کړه_دننی_func_opt(name, FALSE);
    إذا (idx < 0)
    {
	(فراغ)user_func_info(name, rettv->vval.v_dict);
	أرجع;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, حجم(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    إذا (argcount < 0)
    {
	clear_type_list(&type_gap);
	أرجع;
    }
    d = rettv->vval.v_dict;
    dict_جمع_کړه_string(d, "name", (char_u *)fe->f_name);
    dict_جمع_کړه_string(d, "kind", (char_u *)"builtin");
    dict_جمع_کړه_bool(d, "available", fe->f_func != عدم);
    dict_جمع_کړه_شمېره(d, "minargs", fe->f_min_argc);
    dict_جمع_کړه_شمېره(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_جمع_کړه_شمېره(d, "method", fe->f_argtype & FEARG_ماسک);

    دلیلونه = list_alloc();
    إذا (دلیلونه == عدم || dict_جمع_کړه_list(d, "args", دلیلونه) == FAIL)
    {
	list_unref(دلیلونه);
	clear_type_list(&type_gap);
	أرجع;
    }
    argnames = (char_u *)fe->f_argnames;
    لكل (صحيح i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = عدم;
	صحيح	    argnamelen = 0;

	إذا (fe->f_argcheck == عدم)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    إذا (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		اكسر;
	    check = arg_any;
	}
	وإلا
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    إذا ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == عدم)
		اكسر;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	إذا (argnames != عدم)
	{
	    char_u  *پای_ټکی = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = پای_ټکی == عدم ? (صحيح)STRLEN(argnames)
						     : (صحيح)(پای_ټکی - argnames);
	    argnames = پای_ټکی == عدم ? عدم : skipwhite(پای_ټکی + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	إذا (arg == عدم || types == عدم
		|| dict_جمع_کړه_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    أرجع;
	}
	إذا ((argnamelen > 0
		    && dict_جمع_کړه_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(دلیلونه, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    أرجع;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    إذا (argtypes_list == عدم
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	حرف	*tofree_max;
	حرف	*max_name;

	لكل (صحيح i = argcount; i < fe->f_max_argc && i < MAX_FUNC_دلیلونه; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	إذا (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = عدم;
	    ret_name = "any";
	}
	vim_free(tofree_max);
    }
    dict_جمع_کړه_string(d, "returns", (char_u *)ret_name);
    vim_free(tofree);
    clear_type_list(&type_gap);
}

/*
 * "getjumplist()" function
 */
    ساكن فراغ
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    صحيح		i;
    list_T	*l;
    dict_T	*d;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (دننه_vim9script()
	    && (check_for_opt_شمېره_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_شمېره_arg(argvars, 1) == FAIL)))
	أرجع;

    wp = پیدا_کړه_tabwin(&argvars[0], &argvars[1], عدم);
    إذا (wp == عدم)
	أرجع;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    إذا (l == عدم)
	أرجع;
    إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	أرجع;
    }

    list_append_شمېره(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    لكل (i = 0; i < wp->w_jumplistlen; ++i)
    {
	إذا (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    استمر;
	إذا ((d = dict_alloc()) == عدم)
	    أرجع;
	إذا (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    أرجع;
	}
	dict_جمع_کړه_شمېره(d, "lnum", (اوږد)wp->w_jumplist[i].fmark.mark.lnum);
	dict_جمع_کړه_شمېره(d, "col", (اوږد)wp->w_jumplist[i].fmark.mark.ستون);
	dict_جمع_کړه_شمېره(d, "coladd", (اوږد)wp->w_jumplist[i].fmark.mark.coladd);
	dict_جمع_کړه_شمېره(d, "bufnr", (اوږد)wp->w_jumplist[i].fmark.fnum);
	إذا (wp->w_jumplist[i].fname != عدم)
	    dict_جمع_کړه_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    ساكن فراغ
f_getpid(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    rettv->vval.v_شمېره = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    ساكن فراغ
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script() && check_for_opt_شمېره_arg(argvars, 0) == FAIL)
	أرجع;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    ساكن فراغ
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script() && check_for_opt_شمېره_arg(argvars, 0) == FAIL)
	أرجع;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    ساكن فراغ
f_getpos(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    ساكن صحيح
بلاک_def2str(بنية بلاک_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    إذا (ret->string == عدم)
    {
	ret->اوږدتیا = 0;
	أرجع FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->اوږدتیا = bd->startspaces;
    mch_memmove(ret->string + ret->اوږدتیا, bd->textstart, bd->textlen);
    ret->اوږدتیا += bd->textlen;
    vim_memset(ret->string + ret->اوږدتیا, ' ', bd->endspaces);
    ret->اوږدتیا += bd->endspaces;
    ret->string[ret->اوږدتیا] = NUL;

    أرجع OK;
}

    ساكن صحيح
getregionpos(
    typval_T	*argvars,
    typval_T	*rettv,
    pos_T	*p1,
    pos_T	*p2,
    صحيح		*inclusive,
    صحيح		*region_type,
    oparg_T	*oap)
{
    صحيح		fnum1 = -1, fnum2 = -1;
    char_u	*type;
    buf_T	*findbuf;
    char_u	default_type[] = "v";
    صحيح		بلاک_پلنوالی = 0;
    صحيح		is_select_exclusive;
    صحيح		l;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع FAIL;

    إذا (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	أرجع FAIL;

    إذا (list2fpos(&argvars[0], p1, &fnum1, عدم, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, عدم, FALSE) != OK
	    || fnum1 != fnum2)
	أرجع FAIL;

    إذا (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	إذا (type == عدم)
	    type = default_type;
    }
    وإلا
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    إذا (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    وإلا إذا (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    وإلا إذا (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	إذا (*p != NUL && ((بلاک_پلنوالی = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_ارزښت_for_دلیل_str_str), "type", type);
	    أرجع FAIL;
	}
	*region_type = MBLOCK;
    }
    وإلا
    {
	semsg(_(e_invalid_ارزښت_for_دلیل_str_str), "type", type);
	أرجع FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    إذا (findbuf == عدم || findbuf->b_ml.ml_mfp == عدم)
    {
	emsg(_(e_بفر_is_not_loaded));
	أرجع FAIL;
    }

    إذا (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_کرښه_شمېر)
    {
	semsg(_(e_invalid_کرښه_شمېره_nr), p1->lnum);
	أرجع FAIL;
    }
    إذا (p1->ستون == MAXCOL)
	p1->ستون = ml_get_buf_len(findbuf, p1->lnum) + 1;
    وإلا إذا (p1->ستون < 1 || p1->ستون > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_ستنه_شمېره_nr), p1->ستون);
	أرجع FAIL;
    }

    إذا (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_کرښه_شمېر)
    {
	semsg(_(e_invalid_کرښه_شمېره_nr), p2->lnum);
	أرجع FAIL;
    }
    إذا (p2->ستون == MAXCOL)
	p2->ستون = ml_get_buf_len(findbuf, p2->lnum) + 1;
    وإلا إذا (p2->ستون < 1 || p2->ستون > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_ستنه_شمېره_nr), p2->ستون);
	أرجع FAIL;
    }

    curbuf = findbuf;
    curwin->w_بفر = curbuf;
    virtual_op = virtual_فعال();

    // NOTE: Adjustment is needed.
    p1->ستون--;
    p2->ستون--;

    إذا (!LT_POS(*p1, *p2))
    {
	// swap position
	pos_T p;

	p = *p1;
	*p1 = *p2;
	*p2 = p;
    }

    إذا (*region_type == MCHAR)
    {
	// Handle 'selection' == "exclusive".
	إذا (is_select_exclusive && !EQUAL_POS(*p1, *p2))
	    // When backing up to previous line, inclusive becomes false.
	    *inclusive = !unadjust_for_sel_inner(p2);
	// If p2 is on NUL (end of line), inclusive becomes false.
	إذا (*inclusive && !virtual_op && *ml_get_pos(p2) == NUL)
	    *inclusive = FALSE;
    }
    وإلا إذا (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#که_تعریف_شوی FEAT_LINEBREAK
	صحيح	lbr_saved = reset_lbr();
#د_که_پای

	getvvcol(curwin, p1, &sc1, عدم, &ec1, 0);
	getvvcol(curwin, p2, &sc2, عدم, &ec2, 0);

#که_تعریف_شوی FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#د_که_پای

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->پیل = *p1;
	oap->پای_ټکی = *p2;
	oap->پیل_vcol = MIN(sc1, sc2);
	إذا (بلاک_پلنوالی > 0)
	    oap->پای_ټکی_vcol = oap->پیل_vcol + بلاک_پلنوالی - 1;
	وإلا إذا (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->پای_ټکی_vcol = sc2 - 1;
	وإلا
	    oap->پای_ټکی_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    إذا (l > 1)
	p2->ستون += l - 1;

    أرجع OK;
}

/*
 * "getregion()" function
 */
    ساكن فراغ
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    صحيح			inclusive = TRUE;
    صحيح			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    صحيح			save_virtual;
    string_T		akt = {عدم, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    إذا (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	أرجع;

    لكل (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	صحيح ret = OK;

	إذا (region_type == MBLOCK)
	{
	    بنية بلاک_def	bd;

	    بلاک_prep(&oa, &bd, lnum, FALSE);
	    بلاک_def2str(&bd, &akt);
	}
	وإلا إذا (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.اوږدتیا = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.اوږدتیا);
	    إذا (akt.string == عدم)
		akt.اوږدتیا = 0;
	    وإلا
		akt.اوږدتیا = s.اوږدتیا;
	}
	وإلا
	{
	    بنية بلاک_def	bd;

	    charwise_بلاک_prep(p1, p2, &bd, lnum, inclusive);
	    بلاک_def2str(&bd, &akt);
	}

	إذا (akt.string == عدم)
	{
	    clear_tv(rettv);
	    (فراغ)rettv_list_alloc(rettv);
	    اكسر;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (صحيح)akt.اوږدتیا);
	vim_free(akt.string);

	إذا (ret == FAIL)
	{
	    clear_tv(rettv);
	    (فراغ)rettv_list_alloc(rettv);
	    اكسر;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_بفر = curbuf;
    virtual_op = save_virtual;
}

    ساكن فراغ
جمع_کړه_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    إذا (l1 == عدم)
	أرجع;

    إذا (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	أرجع;
    }

    l2 = list_alloc();
    إذا (l2 == عدم)
	أرجع;

    إذا (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	أرجع;
    }

    l3 = list_alloc();
    إذا (l3 == عدم)
	أرجع;

    إذا (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	أرجع;
    }

    list_append_شمېره(l2, curbuf->b_fnum);
    list_append_شمېره(l2, p1.lnum);
    list_append_شمېره(l2, p1.ستون);
    list_append_شمېره(l2, p1.coladd);

    list_append_شمېره(l3, curbuf->b_fnum);
    list_append_شمېره(l3, p2.lnum);
    list_append_شمېره(l3, p2.ستون);
    list_append_شمېره(l3, p2.coladd);
}

/*
 * Compute the positions of the region segment on line "lnum".
 * "ret_p1" is set to the start position of the segment and "ret_p2" to its
 * end position.
 */
    ساكن فراغ
getregionpos_کرښه(
    linenr_T	lnum,
    pos_T	p1,
    pos_T	p2,
    صحيح		inclusive,
    صحيح		region_type,
    oparg_T	*oap,
    صحيح		allow_eol,
    pos_T	*ret_p1,
    pos_T	*ret_p2)
{
    char_u	*کرښه = ml_get(lnum);
    colnr_T	کرښه_len = ml_get_len(lnum);

    إذا (region_type == MLINE)
    {
	ret_p1->ستون = 1;
	ret_p1->coladd = 0;
	ret_p2->ستون = MAXCOL;
	ret_p2->coladd = 0;
    }
    وإلا
    {
	بنية بلاک_def	bd;

	إذا (region_type == MBLOCK)
	    بلاک_prep(oap, &bd, lnum, FALSE);
	وإلا
	    charwise_بلاک_prep(p1, p2, &bd, lnum, inclusive);

	إذا (bd.is_oneChar)  // selection entirely inside one char
	{
	    إذا (region_type == MBLOCK)
	    {
		ret_p1->ستون = mb_prevptr(کرښه, bd.textstart) - کرښه + 1;
		ret_p1->coladd = bd.پیل_char_vcols
					   - (bd.پیل_vcol - oap->پیل_vcol);
	    }
	    وإلا
	    {
		ret_p1->ستون = p1.ستون + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	وإلا إذا (region_type == MBLOCK && oap->پیل_vcol > bd.پیل_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->ستون = MAXCOL;
	    ret_p1->coladd = oap->پیل_vcol - bd.پیل_vcol;
	    bd.is_oneChar = TRUE;
	}
	وإلا إذا (bd.startspaces > 0)
	{
	    ret_p1->ستون = mb_prevptr(کرښه, bd.textstart) - کرښه + 1;
	    ret_p1->coladd = bd.پیل_char_vcols - bd.startspaces;
	}
	وإلا
	{
	    ret_p1->ستون = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	إذا (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->ستون = ret_p1->ستون;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	وإلا إذا (bd.endspaces > 0)
	{
	    ret_p2->ستون = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	وإلا
	{
	    ret_p2->ستون = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    إذا (!allow_eol && ret_p1->ستون > کرښه_len)
    {
	ret_p1->ستون = 0;
	ret_p1->coladd = 0;
    }
    وإلا إذا (ret_p1->ستون > کرښه_len + 1)
	ret_p1->ستون = کرښه_len + 1;

    إذا (!allow_eol && ret_p2->ستون > کرښه_len)
    {
	ret_p2->ستون = ret_p1->ستون == 0 ? 0 : کرښه_len;
	ret_p2->coladd = 0;
    }
    وإلا إذا (ret_p2->ستون > کرښه_len + 1)
	ret_p2->ستون = کرښه_len + 1;

    ret_p1->lnum = lnum;
    ret_p2->lnum = lnum;
}

/*
 * "getregionpos()" function
 */
    ساكن فراغ
f_getregionpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	p1, p2;
    صحيح		inclusive = TRUE;
    صحيح		region_type = -1;
    صحيح		allow_eol = FALSE;
    صحيح		bounds_only = FALSE;
    oparg_T	oa;
    صحيح		lnum;

    buf_T	*save_curbuf;
    صحيح		save_virtual;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    إذا (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	أرجع;

    إذا (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    إذا (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	پیل_pos, پای_ټکی_pos;

	getregionpos_کرښه(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &پیل_pos, &پای_ټکی_pos);
	إذا (p2.lnum != p1.lnum)
	{
	    pos_T	ناکارول_شوی_2;

	    getregionpos_کرښه(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &ناکارول_شوی_2, &پای_ټکی_pos);
	}
	جمع_کړه_regionpos_range(rettv, پیل_pos, پای_ټکی_pos);
    }
    وإلا
    {
	لكل (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_کرښه(lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &ret_p1, &ret_p2);
	    جمع_کړه_regionpos_range(rettv, ret_p1, ret_p2);
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_بفر = curbuf;
    virtual_op = save_virtual;
}

/*
 * Common between getreg(), getreginfo() and getregtype(): get the register
 * name from the first argument.
 * Returns zero on error.
 */
    ساكن صحيح
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	إذا (strregname != عدم && دننه_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_ثبتوونکی_name_must_be_one_char_str), strregname);
	    strregname = عدم;
	}
	إذا (strregname == عدم)	    // type error; errmsg already given
	    أرجع 0;
    }
    وإلا
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    أرجع *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    ساكن فراغ
f_getreg(typval_T *argvars, typval_T *rettv)
{
    صحيح		regname;
    صحيح		arg2 = FALSE;
    صحيح		return_list = FALSE;

    إذا (دننه_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	أرجع;

    regname = getreg_get_regname(argvars);
    إذا (regname == 0)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	صحيح		error = FALSE;

	arg2 = (صحيح)tv_get_bool_chk(&argvars[1], &error);

	إذا (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (صحيح)tv_get_bool_chk(&argvars[2], &error);
	إذا (error)
	    أرجع;
    }

    إذا (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_بیان_SRC : 0) | GREG_LIST);
	إذا (rettv->vval.v_list == عدم)
	    (فراغ)rettv_list_alloc(rettv);
	وإلا
	    ++rettv->vval.v_list->lv_refcount;
    }
    وإلا
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = get_reg_contents(regname,
						    arg2 ? GREG_بیان_SRC : 0);
    }
}

/*
 * "getregtype()" function
 */
    ساكن فراغ
f_getregtype(typval_T *argvars, typval_T *rettv)
{
    صحيح		regname;
    char_u	buf[NUMBUFLEN + 2];
    اوږد	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (دننه_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	أرجع;

    regname = getreg_get_regname(argvars);
    إذا (regname == 0)
	أرجع;

    buf[0] = NUL;
    buf[1] = NUL;
    بدل (get_reg_type(regname, &reglen))
    {
	حالة MLINE: buf[0] = 'V'; اكسر;
	حالة MCHAR: buf[0] = 'v'; اكسر;
	حالة MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((حرف *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		اكسر;
    }
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "gettagstack()" function
 */
    ساكن فراغ
f_gettagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;			// default is current window

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;

    إذا (دننه_vim9script() && check_for_opt_شمېره_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = پیدا_کړه_win_by_nr_or_id(&argvars[0]);
	إذا (wp == عدم)
	    أرجع;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    ساكن فراغ
f_gettext(typval_T *argvars, typval_T *rettv)
{
#إذا defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    حرف *prev = عدم;
#د_که_پای

    إذا (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	أرجع;

    rettv->v_type = VAR_STRING;

    إذا (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != عدم &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#إذا defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ثابت حرف *)argvars[1].vval.v_string, (حرف *)p_enc);
#د_که_پای

#إذا defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((ثابت حرف *)argvars[1].vval.v_string, (ثابت حرف *)argvars[0].vval.v_string));
#وإلا
	textdomain((ثابت حرف *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#د_که_پای

#إذا defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	إذا (prev != عدم)
	    bind_textdomain_codeset((ثابت حرف *)argvars[1].vval.v_string, prev);
#د_که_پای
    }
    وإلا
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
}

// for VIM_VERSION_ defines
#ضمن "version.h"

/*
 * "has()" function
 */
    فراغ
f_has(typval_T *argvars, typval_T *rettv)
{
    صحيح		i;
    char_u	*name;
    صحيح		x = FALSE;
    صحيح		n = FALSE;
    تعريفنوع بنية {
	حرف *name;
	لنډ present;
    } has_توکی_T;
    ساكن has_توکی_T has_list[] =
    {
	{"amiga",
#که_تعریف_شوی AMIGA
		1
#وإلا
		0
#د_که_پای
		},
	{"android",
#که_تعریف_شوی __ANDROID__
		1
#وإلا
		0
#د_که_پای
		},
	{"arp",
#إذا defined(AMIGA) && defined(FEAT_ARP)
		1
#وإلا
		0
#د_که_پای
		},
	{"haiku",
#که_تعریف_شوی __HAIKU__
		1
#وإلا
		0
#د_که_پای
		},
	{"bsd",
#إذا defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#وإلا
		0
#د_که_پای
		},
	{"hpux",
#که_تعریف_شوی hpux
		1
#وإلا
		0
#د_که_پای
		},
	{"hurd",
#که_تعریف_شوی __GNU__
		1
#وإلا
		0
#د_که_پای
		},
	{"linux",
#که_تعریف_شوی __linux__
		1
#وإلا
		0
#د_که_پای
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#که_تعریف_شوی MACOS_X
		1
#وإلا
		0
#د_که_پای
		},
	{"osx",		// Mac OS X
#که_تعریف_شوی MACOS_X
		1
#وإلا
		0
#د_که_پای
		},
	{"macunix",	// Mac OS X, with the darwin feature
#إذا defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#وإلا
		0
#د_که_پای
		},
	{"osxdarwin",	// synonym for macunix
#إذا defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#وإلا
		0
#د_که_پای
		},
	{"qnx",
#که_تعریف_شوی __QNX__
		1
#وإلا
		0
#د_که_پای
		},
	{"sun",
#که_تعریف_شوی SUN_SYSTEM
		1
#وإلا
		0
#د_که_پای
		},
	{"termux",
#که_تعریف_شوی __TERMUX__
		1
#وإلا
		0
#د_که_پای
		},
	{"unix",
#که_تعریف_شوی UNIX
		1
#وإلا
		0
#د_که_پای
		},
	{"vms",
#که_تعریف_شوی VMS
		1
#وإلا
		0
#د_که_پای
		},
	{"win32",
#که_تعریف_شوی MSWIN
		1
#وإلا
		0
#د_که_پای
		},
	{"win32unix",
#که_تعریف_شوی WIN32UNIX
		1
#وإلا
		0
#د_که_پای
		},
	{"win64",
#که_تعریف_شوی _WIN64
		1
#وإلا
		0
#د_که_پای
		},
	{"ebcdic", 0 },
	{"fname_case",
#که_نه_تعریف_شوی CASE_INSENSITIVE_FILENAME
		1
#وإلا
		0
#د_که_پای
		},
	{"acl",
#که_تعریف_شوی HAVE_ACL
		1
#وإلا
		0
#د_که_پای
		},
	{"arabic",
#که_تعریف_شوی FEAT_ARABIC
		1
#وإلا
		0
#د_که_پای
		},
	{"autocmd", 1},
	{"autochdir",
#که_تعریف_شوی FEAT_AUTOCHDIR
		1
#وإلا
		0
#د_که_پای
		},
	{"autoservername",
#که_تعریف_شوی FEAT_AUTOSERVERNAME
		1
#وإلا
		0
#د_که_پای
		},
	{"socketserver",
#که_تعریف_شوی FEAT_SOCKETSERVER
		1
#وإلا
		0
#د_که_پای
		},
	{"balloon_eval",
#که_تعریف_شوی FEAT_BEVAL_GUI
		1
#وإلا
		0
#د_که_پای
		},
	{"balloon_multiline",
#که_تعریف_شوی FEAT_BEVAL_GUI
		1
#وإلا
		0
#د_که_پای
		},
	{"balloon_eval_term",
#که_تعریف_شوی FEAT_BEVAL_غړی
		1
#وإلا
		0
#د_که_پای
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#إذا defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#وإلا
		0
#د_که_پای
		},
	{"byte_offset",
#که_تعریف_شوی FEAT_BYTEOFF
		1
#وإلا
		0
#د_که_پای
		},
	{"channel",
#که_تعریف_شوی FEAT_دنده_CHANNEL
		1
#وإلا
		0
#د_که_پای
		},
	{"cindent", 1},
	{"clientserver",
#که_تعریف_شوی FEAT_CLIENTSERVER
		1
#وإلا
		0
#د_که_پای
		},
	{"clipboard",
#که_تعریف_شوی FEAT_CLIPBOARD
		1
#وإلا
		0
#د_که_پای
		},
	{"clipboard_provider",
#که_تعریف_شوی FEAT_CLIPBOARD_PROVIDER
		1
#وإلا
		0
#د_که_پای
		},
	{"cmdline_compl", 1},
	{"cmdline_hist", 1},
	{"cmdwin", 1},
	{"comments", 1},
	{"conceal",
#که_تعریف_شوی FEAT_CONCEAL
		1
#وإلا
		0
#د_که_پای
		},
	{"cryptv",
#که_تعریف_شوی FEAT_CRYPT
		1
#وإلا
		0
#د_که_پای
		},
	{"crypt-blowfish",
#که_تعریف_شوی FEAT_CRYPT
		1
#وإلا
		0
#د_که_پای
		},
	{"crypt-blowfish2",
#که_تعریف_شوی FEAT_CRYPT
		1
#وإلا
		0
#د_که_پای
		},
	{"cscope",
#که_تعریف_شوی FEAT_CSCOPE
		1
#وإلا
		0
#د_که_پای
		},
	{"cursorbind", 1},
	{"cursorshape",
#که_تعریف_شوی CURSOR_SHAPE
		1
#وإلا
		0
#د_که_پای
		},
	{"debug",
#که_تعریف_شوی DEBUG
		1
#وإلا
		0
#د_که_پای
		},
	{"dialog_con",
#که_تعریف_شوی FEAT_CON_DIALOG
		1
#وإلا
		0
#د_که_پای
		},
	{"dialog_con_gui",
#إذا defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#وإلا
		0
#د_که_پای
		},
	{"dialog_gui",
#که_تعریف_شوی FEAT_GUI_DIALOG
		1
#وإلا
		0
#د_که_پای
		},
	{"diff",
#که_تعریف_شوی FEAT_DIFF
		1
#وإلا
		0
#د_که_پای
		},
	{"digraphs",
#که_تعریف_شوی FEAT_DIGRAPHS
		1
#وإلا
		0
#د_که_پای
		},
	{"directx",
#که_تعریف_شوی FEAT_DIRECTX
		1
#وإلا
		0
#د_که_پای
		},
	{"dnd",
#که_تعریف_شوی FEAT_DND
		1
#وإلا
		0
#د_که_پای
		},
	{"drop_file",
#که_تعریف_شوی HAVE_DROP_FILE
		1
#وإلا
		0
#د_که_پای
		},
	{"emacs_tags",
#که_تعریف_شوی FEAT_EMACS_نښانونه
		1
#وإلا
		0
#د_که_پای
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#که_تعریف_شوی FEAT_پلټنه_EXTRA
		1
#وإلا
		0
#د_که_پای
		},
	{"file_in_path", 1},
	{"filterpipe",
#إذا defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#وإلا
		0
#د_که_پای
		},
	{"find_in_path",
#که_تعریف_شوی FEAT_پیدا_کړه_ID
		1
#وإلا
		0
#د_که_پای
		},
	{"float", 1},
	{"folding",
#که_تعریف_شوی FEAT_FOLDING
		1
#وإلا
		0
#د_که_پای
		},
	{"footer", 0},
	{"fork",
#إذا !defined(USE_SYSTEM) && defined(UNIX)
		1
#وإلا
		0
#د_که_پای
		},
	{"gettext",
#که_تعریف_شوی FEAT_GETTEXT
		1
#وإلا
		0
#د_که_پای
		},
	{"gui",
#که_تعریف_شوی FEAT_GUI
		1
#وإلا
		0
#د_که_پای
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#که_تعریف_شوی FEAT_GUI_GTK
		1
#وإلا
		0
#د_که_پای
		},
	{"gui_gtk2",
#إذا defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#وإلا
		0
#د_که_پای
		},
	{"gui_gtk3",
#إذا defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#وإلا
		0
#د_که_پای
		},
	{"gui_gtk4",
#إذا defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#وإلا
		0
#د_که_پای
		},
	{"gui_gnome",
#که_تعریف_شوی FEAT_GUI_GNOME
		1
#وإلا
		0
#د_که_پای
		},
	{"gui_haiku",
#که_تعریف_شوی FEAT_GUI_HAIKU
		1
#وإلا
		0
#د_که_پای
		},
	{"gui_mac", 0},
	{"gui_motif",
#که_تعریف_شوی FEAT_GUI_MOTIF
		1
#وإلا
		0
#د_که_پای
		},
	{"gui_photon",
#که_تعریف_شوی FEAT_GUI_PHOTON
		1
#وإلا
		0
#د_که_پای
		},
	{"gui_win32",
#که_تعریف_شوی FEAT_GUI_MSWIN
		1
#وإلا
		0
#د_که_پای
		},
	{"iconv",
#إذا defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#وإلا
		0
#د_که_پای
		},
	{"image",
#که_تعریف_شوی FEAT_IMAGE
		1
#وإلا
		0
#د_که_پای
		},
	{"image_popup",
#که_تعریف_شوی FEAT_IMAGE_POPUP
		1
#وإلا
		0
#د_که_پای
		},
	{"insert_expand", 1},
	{"ipv6",
#که_تعریف_شوی FEAT_IPV6
		1
#وإلا
		0
#د_که_پای
	},
	{"job",
#که_تعریف_شوی FEAT_دنده_CHANNEL
		1
#وإلا
		0
#د_که_پای
		},
	{"jumplist", 1},
	{"keymap",
#که_تعریف_شوی FEAT_KEYMAP
		1
#وإلا
		0
#د_که_پای
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#که_تعریف_شوی FEAT_LANGMAP
		1
#وإلا
		0
#د_که_پای
		},
	{"libcall",
#که_تعریف_شوی FEAT_LIBCALL
		1
#وإلا
		0
#د_که_پای
		},
	{"linebreak",
#که_تعریف_شوی FEAT_LINEBREAK
		1
#وإلا
		0
#د_که_پای
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#إذا defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#وإلا
		0
#د_که_پای
		},
	{"menu",
#که_تعریف_شوی FEAT_MENU
		1
#وإلا
		0
#د_که_پای
		},
	{"mksession",
#که_تعریف_شوی FEAT_SESSION
		1
#وإلا
		0
#د_که_پای
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#که_تعریف_شوی FEAT_MOUSESHAPE
		1
#وإلا
		0
#د_که_پای
		},
	{"mouse_dec",
#إذا (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#وإلا
		0
#د_که_پای
		},
	{"mouse_gpm",
#إذا (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#وإلا
		0
#د_که_پای
		},
	{"mouse_jsbterm",
#إذا (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#وإلا
		0
#د_که_پای
		},
	{"mouse_netterm",
#إذا (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#وإلا
		0
#د_که_پای
		},
	{"mouse_pterm",
#إذا (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#وإلا
		0
#د_که_پای
		},
	{"mouse_sgr",
#إذا (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#وإلا
		0
#د_که_پای
		},
	{"mouse_sysmouse",
#إذا (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#وإلا
		0
#د_که_پای
		},
	{"mouse_urxvt",
#إذا (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#وإلا
		0
#د_که_پای
		},
	{"mouse_xterm",
#إذا (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#وإلا
		0
#د_که_پای
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#که_تعریف_شوی FEAT_MBYTE_IME
		1
#وإلا
		0
#د_که_پای
		},
	{"multi_lang",
#که_تعریف_شوی FEAT_MULTI_LANG
		1
#وإلا
		0
#د_که_پای
		},
	{"mzscheme",
#إذا defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#وإلا
		0
#د_که_پای
		},
	{"nanotime",
#که_تعریف_شوی ST_MTIM_NSEC
		1
#وإلا
		0
#د_که_پای
	},
	{"num64", 1},
	{"ole",
#که_تعریف_شوی FEAT_OLE
		1
#وإلا
		0
#د_که_پای
		},
	{"packages",
#که_تعریف_شوی FEAT_EVAL
		1
#وإلا
		0
#د_که_پای
		},
	{"path_extra", 1},
	{"perl",
#إذا defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#وإلا
		0
#د_که_پای
		},
	{"persistent_undo",
#که_تعریف_شوی FEAT_PERSISTENT_UNDO
		1
#وإلا
		0
#د_که_پای
		},
	{"python_compiled",
#إذا defined(FEAT_PYTHON)
		1
#وإلا
		0
#د_که_پای
		},
	{"python_dynamic",
#إذا defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#وإلا
		0
#د_که_پای
		},
	{"python",
#إذا defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#وإلا
		0
#د_که_پای
		},
	{"pythonx",
#إذا (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#وإلا
		0
#د_که_پای
		},
	{"python3_compiled",
#إذا defined(FEAT_PYTHON3)
		1
#وإلا
		0
#د_که_پای
		},
	{"python3_dynamic",
#إذا defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#وإلا
		0
#د_که_پای
		},
	{"python3_stable",
#إذا defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#وإلا
		0
#د_که_پای
		},
	{"python3",
#إذا defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#وإلا
		0
#د_که_پای
		},
	{"popupwin",
#که_تعریف_شوی FEAT_PROP_POPUP
		1
#وإلا
		0
#د_که_پای
		},
	{"postscript",
#که_تعریف_شوی FEAT_POSTSCRIPT
		1
#وإلا
		0
#د_که_پای
		},
	{"pango",
#که_تعریف_شوی FEAT_PRINT_PANGO
		1
#وإلا
		0
#د_که_پای
		},
	{"printer",
#که_تعریف_شوی FEAT_PRINTER
		1
#وإلا
		0
#د_که_پای
		},
	{"profile",
#که_تعریف_شوی FEAT_PROFILE
		1
#وإلا
		0
#د_که_پای
		},
	{"prof_nsec",
#که_تعریف_شوی PROF_NSEC
		1
#وإلا
		0
#د_که_پای
		},
	{"reltime",
#که_تعریف_شوی FEAT_RELTIME
		1
#وإلا
		0
#د_که_پای
		},
	{"quickfix",
#که_تعریف_شوی FEAT_QUICKFIX
		1
#وإلا
		0
#د_که_پای
		},
	{"rightleft",
#که_تعریف_شوی FEAT_RIGHTLEFT
		1
#وإلا
		0
#د_که_پای
		},
	{"ruby",
#إذا defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#وإلا
		0
#د_که_پای
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#که_تعریف_شوی FEAT_SIGNS
		1
#وإلا
		0
#د_که_پای
		},
	{"smartindent", 1},
	{"startuptime",
#که_تعریف_شوی STARTUPTIME
		1
#وإلا
		0
#د_که_پای
		},
	{"statusline",
#که_تعریف_شوی FEAT_STL_OPT
		1
#وإلا
		0
#د_که_پای
		},
	{"statusline_click",
#که_تعریف_شوی FEAT_STL_OPT
		1
#وإلا
		0
#د_که_پای
		},
	{"netbeans_intg",
#که_تعریف_شوی FEAT_NETBEANS_INTG
		1
#وإلا
		0
#د_که_پای
		},
	{"sodium",
#إذا defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#وإلا
		0
#د_که_پای
		},
	{"sound",
#که_تعریف_شوی FEAT_SOUND
		1
#وإلا
		0
#د_که_پای
		},
	{"spell",
#که_تعریف_شوی FEAT_SPELL
		1
#وإلا
		0
#د_که_پای
		},
	{"syntax",
#که_تعریف_شوی FEAT_SYN_HL
		1
#وإلا
		0
#د_که_پای
		},
	{"system",
#إذا defined(USE_SYSTEM) || !defined(UNIX)
		1
#وإلا
		0
#د_که_پای
		},
	{"tabpanel",
#إذا defined(FEAT_TABPANEL)
		1,
#وإلا
		0,
#د_که_پای
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#إذا defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#وإلا
		0
#د_که_پای
		},
	{"termguicolors",
#که_تعریف_شوی FEAT_TERMGUICOLORS
		1
#وإلا
		0
#د_که_پای
		},
	{"terminal",
#إذا defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#وإلا
		0
#د_که_پای
		},
	{"terminfo",
#که_تعریف_شوی TERMINFO
		1
#وإلا
		0
#د_که_پای
		},
	{"termresponse",
#که_تعریف_شوی FEAT_TERMRESPONSE
		1
#وإلا
		0
#د_که_پای
		},
	{"textobjects", 1},
	{"textprop",
#که_تعریف_شوی FEAT_PROP_POPUP
		1
#وإلا
		0
#د_که_پای
		},
	{"tgetent",
#که_تعریف_شوی HAVE_TGETENT
		1
#وإلا
		0
#د_که_پای
		},
	{"timers",
#که_تعریف_شوی FEAT_TIMERS
		1
#وإلا
		0
#د_که_پای
		},
	{"title", 1},
	{"toolbar",
#که_تعریف_شوی FEAT_TOOLBAR
		1
#وإلا
		0
#د_که_پای
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#که_تعریف_شوی FEAT_VARTABS
		1
#وإلا
		0
#د_که_پای
		},
	{"vertsplit", 1},
	{"viminfo",
#که_تعریف_شوی FEAT_VIMINFO
		1
#وإلا
		0
#د_که_پای
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
#که_تعریف_شوی FEAT_VTP
		1
#وإلا
		0
#د_که_پای
		},
	{"wayland",
#که_تعریف_شوی FEAT_WAYLAND
		1
#وإلا
		0
#د_که_پای
		},
	{"wayland_clipboard",
#که_تعریف_شوی FEAT_WAYLAND_CLIPBOARD
		1
#وإلا
		0
#د_که_پای
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#که_تعریف_شوی FEAT_WAK
		1
#وإلا
		0
#د_که_پای
		},
	{"writebackup",
#که_تعریف_شوی FEAT_WRITEBACKUP
		1
#وإلا
		0
#د_که_پای
		},
	{"xattr",
#که_تعریف_شوی FEAT_XATTR
		1
#وإلا
		0
#د_که_پای
		},
	{"xim",
#که_تعریف_شوی FEAT_XIM
		1
#وإلا
		0
#د_که_پای
		},
	{"xfontset",
#که_تعریف_شوی FEAT_XFONTSET
		1
#وإلا
		0
#د_که_پای
		},
	{"xpm",
#إذا defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#وإلا
		0
#د_که_پای
		},
	{"xpm_w32",	// for backward compatibility
#که_تعریف_شوی FEAT_XPM_W32
		1
#وإلا
		0
#د_که_پای
		},
	{"xsmp",
#که_تعریف_شوی USE_XSMP
		1
#وإلا
		0
#د_که_پای
		},
	{"xsmp_interact",
#که_تعریف_شوی USE_XSMP_INTERACT
		1
#وإلا
		0
#د_که_پای
		},
	{"xterm_clipboard",
#که_تعریف_شوی FEAT_XCLIPBOARD
		1
#وإلا
		0
#د_که_پای
		},
	{"xterm_save",
#که_تعریف_شوی FEAT_XTERM_SAVE
		1
#وإلا
		0
#د_که_پای
		},
	{"X11",
#إذا defined(UNIX) && defined(FEAT_X11)
		1
#وإلا
		0
#د_که_پای
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#إذا defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#وإلا
		0
#د_که_پای
		},
	{عدم, 0}
    };

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    name = tv_get_string(&argvars[0]);

    // Fast-path: check features not in has_list[] first to avoid the full
    // linear scan for very common queries like has('patch-...').
    إذا (STRNICMP(name, "patch", 5) == 0)
    {
	x = TRUE;
	إذا (name[5] == '-'
		&& STRLEN(name) >= 11
		&& (name[6] >= '1' && name[6] <= '9'))
	{
	    حرف	*پای_ټکی;
	    صحيح	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (صحيح)strtoul((حرف *)name + 6, &پای_ټکی, 10);
	    إذا (*پای_ټکی == '.' && vim_isdigit(پای_ټکی[1])
		    && پای_ټکی[2] == '.' && vim_isdigit(پای_ټکی[3]))
	    {
		minor = atoi(پای_ټکی + 1);

		// Expect "patch-9.9.01234".
		n = (major < VIM_نسخه_MAJOR
		     || (major == VIM_نسخه_MAJOR
			 && (minor < VIM_نسخه_MINOR
			     || (minor == VIM_نسخه_MINOR
				 && has_patch(atoi(پای_ټکی + 3))))));
	    }
	}
	وإلا إذا (خوندي_isdigit(name[5]))
	    n = has_patch(atoi((حرف *)name + 5));
    }
    وإلا إذا (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    وإلا إذا (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_ننوت_isatty();
    }
    وإلا إذا (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    وإلا إذا (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    وإلا إذا (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#که_تعریف_شوی FEAT_GUI
	n = (gui.دننه_use || gui.starting);
#د_که_پای
    }
    وإلا إذا (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#إذا defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.دننه_use;	// gui_mch_browse() works when GUI is running
#د_که_پای
    }
    وإلا إذا (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#که_تعریف_شوی FEAT_SYN_HL
	n = syntax_present(curwin);
#د_که_پای
    }
    وإلا إذا (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#که_تعریف_شوی FEAT_VTP
	n = is_غړی_win32() && has_vtp_working();
#د_که_پای
    }
    وإلا إذا (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#که_تعریف_شوی FEAT_NETBEANS_INTG
	n = netbeans_فعال();
#د_که_پای
    }
    وإلا إذا (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#که_تعریف_شوی FEAT_MOUSE_GPM
	n = gpm_enabled();
#د_که_پای
    }
    وإلا إذا (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#إذا defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#د_که_پای
    }
    وإلا إذا (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#که_تعریف_شوی FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_ستوری.شته;
#د_که_پای
    }
    وإلا إذا (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#که_تعریف_شوی FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# إذا defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	إذا (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	وإلا
	    n = FALSE;
# وإلا
	n = FALSE;
# endif
#د_که_پای
    }

    // Look up in has_list[] only if not already handled above.
    إذا (x == FALSE)
    {
	لكل (i = 0; has_list[i].name != عدم; ++i)
	    إذا (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		اكسر;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	إذا (x == TRUE && n == FALSE)
	{
	    إذا (0)
	    {
		// intentionally empty
	    }
#که_تعریف_شوی VIMDLL
	    وإلا إذا (STRICMP(name, "filterpipe") == 0)
		n = gui.دننه_use || gui.starting;
#د_که_پای
#إذا defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    وإلا إذا (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#د_که_پای
#که_تعریف_شوی DYNAMIC_LUA
	    وإلا إذا (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#د_که_پای
#که_تعریف_شوی DYNAMIC_MZSCHEME
	    وإلا إذا (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#د_که_پای
#که_تعریف_شوی DYNAMIC_PERL
	    وإلا إذا (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#د_که_پای
#که_تعریف_شوی DYNAMIC_PYTHON
	    وإلا إذا (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#د_که_پای
#که_تعریف_شوی DYNAMIC_PYTHON3
	    وإلا إذا (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#د_که_پای
#إذا defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    وإلا إذا (STRICMP(name, "pythonx") == 0)
	    {
# إذا defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		إذا (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		وإلا إذا (p_pyx == 3)
		    n = python3_enabled(FALSE);
		وإلا إذا (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#د_که_پای
#که_تعریف_شوی DYNAMIC_RUBY
	    وإلا إذا (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#د_که_پای
#که_تعریف_شوی DYNAMIC_TCL
	    وإلا إذا (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#د_که_پای
#که_تعریف_شوی DYNAMIC_SODIUM
	    وإلا إذا (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#د_که_پای
#إذا defined(FEAT_TERMINAL) && defined(MSWIN)
	    وإلا إذا (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#د_که_پای
#که_تعریف_شوی DYNAMIC_GPM
	    وإلا إذا (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_شته();
#د_که_پای
	}
    }

    إذا (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_شمېره = x;
    وإلا
	// return whether feature is enabled
	rettv->vval.v_شمېره = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    صحيح
dynamic_feature(char_u *feature)
{
    أرجع (feature == عدم
#إذا defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.دننه_use)
#د_که_پای
#که_تعریف_شوی VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#د_که_پای
#إذا defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.دننه_use)
#د_که_پای
#إذا defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    || STRICMP(feature, "iconv") == 0
#د_که_پای
#که_تعریف_شوی DYNAMIC_LUA
	    || STRICMP(feature, "lua") == 0
#د_که_پای
#که_تعریف_شوی FEAT_MOUSE_GPM
	    || (STRICMP(feature, "mouse_gpm_enabled") == 0 && !gpm_enabled())
#د_که_پای
#که_تعریف_شوی DYNAMIC_MZSCHEME
	    || STRICMP(feature, "mzscheme") == 0
#د_که_پای
#که_تعریف_شوی FEAT_NETBEANS_INTG
	    || STRICMP(feature, "netbeans_enabled") == 0
#د_که_پای
#که_تعریف_شوی DYNAMIC_PERL
	    || STRICMP(feature, "perl") == 0
#د_که_پای
#که_تعریف_شوی DYNAMIC_PYTHON
	    || STRICMP(feature, "python") == 0
#د_که_پای
#که_تعریف_شوی DYNAMIC_PYTHON3
	    || STRICMP(feature, "python3") == 0
#د_که_پای
#إذا defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    || STRICMP(feature, "pythonx") == 0
#د_که_پای
#که_تعریف_شوی DYNAMIC_RUBY
	    || STRICMP(feature, "ruby") == 0
#د_که_پای
#که_تعریف_شوی FEAT_SYN_HL
	    || STRICMP(feature, "syntax_items") == 0
#د_که_پای
#که_تعریف_شوی DYNAMIC_TCL
	    || STRICMP(feature, "tcl") == 0
#د_که_پای
	    // once "starting" is zero it will stay that way
	    || (STRICMP(feature, "vim_starting") == 0 && starting != 0)
	    || STRICMP(feature, "multi_byte_encoding") == 0
#إذا defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#د_که_پای
	    );
}

/*
 * "haslocaldir()" function
 */
    ساكن فراغ
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = عدم;
    win_T	*wp = عدم;

    إذا (دننه_vim9script()
	    && (check_for_opt_شمېره_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_شمېره_arg(argvars, 1) == FAIL)))
	أرجع;

    wp = پیدا_کړه_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    إذا (wp != عدم && wp->w_localdir != عدم)
	rettv->vval.v_شمېره = 1;
    وإلا إذا (tp != عدم && tp->tp_localdir != عدم)
	rettv->vval.v_شمېره = 2;
    وإلا
	rettv->vval.v_شمېره = 0;
}

/*
 * "highlightID(name)" function
 */
    ساكن فراغ
f_hlID(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_شمېره = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    ساكن فراغ
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_شمېره = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    ساكن فراغ
f_hostname(typval_T *argvars ناکارول_شوی, typval_T *rettv)
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
#که_تعریف_شوی VMS // VMS does not have defined uintptr_t
# إذا defined(HAVE_NO_LONG_LONG)
تعريفنوع بینښه صحيح uintptr_t;
# وإلا
تعريفنوع بینښه اوږد اوږد uintptr_t;
# endif
#د_که_پای // VMS

    ساكن فراغ
f_id(typval_T *argvars, typval_T *rettv)
{
    حرف    numbuf[NUMBUFLEN];
    حرف    *p = numbuf;

    بدل (argvars[0].v_type)
    {
	حالة VAR_LIST:
	حالة VAR_TUPLE:
	حالة VAR_DICT:
	حالة VAR_OBJECT:
	حالة VAR_دنده:
	حالة VAR_CHANNEL:
	حالة VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    إذا (argvars[0].vval.v_object != عدم)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(فراغ *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		لكل (; p < numbuf + حجم(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	افتراضي:
	    اكسر;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    ساكن فراغ
شاخص_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    صحيح		پیل = 0;
    صحيح		error = FALSE;
    صحيح		ic = FALSE;

    b = argvars[0].vval.v_blob;
    إذا (b == عدم)
	أرجع;

    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	پیل = tv_get_شمېره_chk(&argvars[2], &error);
	إذا (error)
	    أرجع;
    }

    إذا (پیل < 0)
    {
	پیل = blob_len(b) + پیل;
	إذا (پیل < 0)
	    پیل = 0;
    }

    لكل (صحيح idx = پیل; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_شمېره;
	tv.vval.v_شمېره = blob_get(b, idx);
	إذا (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_شمېره = idx;
	    أرجع;
	}
    }
}

/*
 * index() function for a tuple
 */
    ساكن فراغ
شاخص_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    صحيح		ic = FALSE;
    صحيح		error = FALSE;

    إذا (tuple == عدم)
	أرجع;

    صحيح	پیل_idx = 0;
    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	پیل_idx = tv_get_شمېره_chk(&argvars[2], &error);
	إذا (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (صحيح)tv_get_bool_chk(&argvars[3], &error);
	إذا (error)
	    أرجع;
    }

    rettv->vval.v_شمېره = شاخص_tuple(tuple, &argvars[1], پیل_idx, ic);
}

/*
 * index() function for a list
 */
    ساكن فراغ
شاخص_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*توکی;
    اوږد	idx = 0;
    صحيح		ic = FALSE;
    صحيح		error = FALSE;

    l = argvars[0].vval.v_list;
    إذا (l == عدم)
	أرجع;

    CHECK_LIST_MATERIALIZE(l);
    توکی = l->lv_لومړی;
    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	توکی = list_پیدا_کړه(l, (اوږد)tv_get_شمېره_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	إذا (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (صحيح)tv_get_bool_chk(&argvars[3], &error);
	إذا (error)
	    توکی = عدم;
    }

    لكل ( ; توکی != عدم; توکی = توکی->li_بل, ++idx)
	إذا (tv_equal(&توکی->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_شمېره = idx;
	    اكسر;
	}
}

/*
 * "index()" function
 */
    ساكن فراغ
f_شاخص(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_شمېره = -1;

    إذا (دننه_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_شمېره_arg(argvars, 1) == FAIL)
		|| check_for_opt_شمېره_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	أرجع;

    إذا (argvars[0].v_type == VAR_BLOB)
	شاخص_func_blob(argvars, rettv);
    وإلا إذا (argvars[0].v_type == VAR_TUPLE)
	شاخص_func_tuple(argvars, rettv);
    وإلا إذا (argvars[0].v_type == VAR_LIST)
	شاخص_func_list(argvars, rettv);
    وإلا
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    صحيح
indexof_eval_بیان(typval_T *بیان)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	وموندل;
    صحيح		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_کلی);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    إذا (eval_بیان_typval(بیان, FALSE, argv, 2, عدم, &newtv) == FAIL)
	أرجع FALSE;

    وموندل = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    أرجع error ? FALSE : وموندل;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    ساكن صحيح
indexof_blob(blob_T *b, اوږد startidx, typval_T *بیان)
{
    اوږد	idx = 0;

    إذا (b == عدم)
	أرجع -1;

    إذا (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	إذا (startidx < 0)
	    startidx = 0;
    }

    سېټ_vim_var_type(VV_کلی, VAR_شمېره);
    سېټ_vim_var_type(VV_VAL, VAR_شمېره);

    صحيح		called_emsg_پیل = called_emsg;
    لكل (idx = startidx; idx < blob_len(b); ++idx)
    {
	سېټ_vim_var_nr(VV_کلی, idx);
	سېټ_vim_var_nr(VV_VAL, blob_get(b, idx));

	إذا (indexof_eval_بیان(بیان))
	    أرجع idx;

	إذا (called_emsg != called_emsg_پیل)
	    أرجع -1;
    }

    أرجع -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ساكن صحيح
indexof_list(list_T *l, اوږد startidx, typval_T *بیان)
{
    listitem_T	*توکی;
    اوږد	idx = 0;
    صحيح		وموندل;

    إذا (l == عدم)
	أرجع -1;

    CHECK_LIST_MATERIALIZE(l);

    إذا (startidx == 0)
	توکی = l->lv_لومړی;
    وإلا
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	توکی = list_پیدا_کړه(l, startidx);
	إذا (توکی != عدم)
	    idx = l->lv_u.mat.lv_idx;
    }

    سېټ_vim_var_type(VV_کلی, VAR_شمېره);

    صحيح		called_emsg_پیل = called_emsg;
    لكل ( ; توکی != عدم; توکی = توکی->li_بل, ++idx)
    {
	سېټ_vim_var_nr(VV_کلی, idx);
	copy_tv(&توکی->li_tv, get_vim_var_tv(VV_VAL));

	وموندل = indexof_eval_بیان(بیان);
	clear_tv(get_vim_var_tv(VV_VAL));

	إذا (وموندل)
	    أرجع idx;

	إذا (called_emsg != called_emsg_پیل)
	    أرجع -1;
    }

    أرجع -1;
}

/*
 * "indexof()" function
 */
    ساكن فراغ
f_indexof(typval_T *argvars, typval_T *rettv)
{
    اوږد	startidx = 0;
    typval_T	save_val;
    typval_T	save_کلی;
    صحيح		save_did_emsg;

    rettv->vval.v_شمېره = -1;

    إذا (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	أرجع;

    إذا ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == عدم
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == عدم))
	أرجع;

    إذا (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_شمېره_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_کلی, &save_کلی);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    إذا (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_شمېره = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    وإلا إذا (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_شمېره = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    وإلا
	rettv->vval.v_شمېره = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_کلی, &save_کلی);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

ساكن صحيح inputsecret_flag = 0;

/*
 * "input()" function
 *     Also handles inputsecret() when inputsecret is set.
 */
    ساكن فراغ
f_ننوت(typval_T *argvars, typval_T *rettv)
{
    get_user_ننوت(argvars, rettv, FALSE, inputsecret_flag);
}

/*
 * "inputdialog()" function
 */
    ساكن فراغ
f_inputdialog(typval_T *argvars, typval_T *rettv)
{
#إذا defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    إذا (gui.دننه_use && vim_strchr(p_go, GO_CONDIALOG) == عدم)
    {
	char_u	*پیغام_2;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	إذا (دننه_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    أرجع;

	پیغام_2 = tv_get_string_chk(&argvars[0]);
	إذا (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != عدم)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	وإلا
	    IObuff[0] = NUL;
	إذا (پیغام_2 != عدم && defstr != عدم
		&& do_dialog(VIM_QUESTION, عدم, پیغام_2,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	وإلا
	{
	    إذا (پیغام_2 != عدم && defstr != عدم
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    وإلا
		rettv->vval.v_string = عدم;
	}
	rettv->v_type = VAR_STRING;
    }
    وإلا
#د_که_پای
	get_user_ننوت(argvars, rettv, TRUE, inputsecret_flag);
}

/*
 * "inputlist()" function
 */
    ساكن فراغ
f_inputlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    صحيح		selected;
    صحيح		mouse_کارول_شوی;

#که_تعریف_شوی NO_CONSOLE_ننوت
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    إذا (no_console_ننوت() && !is_not_a_غړی())
	أرجع;
#د_که_پای
    إذا (دننه_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == عدم)
    {
	semsg(_(e_دلیل_د_str_must_be_list), "inputlist()");
	أرجع;
    }

    پیغام_پیل();
    پیغام_لیکه = لیکې - 1;	// for when 'cmdheight' > 1
    کرښې_کیڼ = لیکې;	// avoid more prompt
    پیغام_scroll = TRUE;
    پیغام_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_توکي(l, li)
    {
	پیغام_puts((حرف *)tv_get_string(&li->li_tv));
	پیغام_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_شمېره(&mouse_کارول_شوی);
    إذا (mouse_کارول_شوی)
	selected -= کرښې_کیڼ;

    rettv->vval.v_شمېره = selected;
}

ساكن garray_T	    ga_userinput = {0, 0, حجم(tasave_T), 4, عدم};

/*
 * "inputrestore()" function
 */
    ساكن فراغ
f_inputrestore(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    إذا (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_معلومات)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    وإلا إذا (p_verbose > 1)
    {
	verb_پیغام(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_شمېره = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    ساكن فراغ
f_inputsave(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    إذا (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_معلومات)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    وإلا
	rettv->vval.v_شمېره = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    ساكن فراغ
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    ++cmdline_ستوری;
    ++inputsecret_flag;
    f_ننوت(argvars, rettv);
    --cmdline_ستوری;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    ساكن فراغ
f_مداخله(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    ساكن فراغ
f_invert(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script() && check_for_شمېره_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_شمېره = ~tv_get_شمېره_chk(&argvars[0], عدم);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    ساكن فراغ
free_lval_ریښه(lval_ریښه_T *ریښه)
{
    إذا (ریښه->lr_tv != عدم)
	free_tv(ریښه->lr_tv);
    class_unref(ریښه->lr_cl_exec);
    ریښه->lr_tv = عدم;
    ریښه->lr_cl_exec = عدم;
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

    ساكن صحيح
fix_variable_reference_lval_ریښه(lval_ریښه_T *ریښه, char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    صحيح وموندل_member = FALSE;
    إذا (ریښه->lr_tv->v_type == VAR_OBJECT)
    {
	إذا (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    وموندل_member = TRUE;
    }
    إذا (!وموندل_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*پای_ټکی;
	لكل (پای_ټکی = name; ASCII_ISALNUM(*پای_ټکی) || *پای_ټکی == '_'; ++پای_ټکی)
	    ;
	صحيح idx = class_member_idx(ریښه->lr_cl_exec, name, پای_ټکی - name);
	إذا (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(ریښه->lr_tv);
	    copy_tv(&ریښه->lr_cl_exec->class_members_tv[idx], ریښه->lr_tv);
	    وموندل_member = TRUE;
	}
    }
    إذا (!وموندل_member)
    {
	free_tv(ریښه->lr_tv);
	ریښه->lr_tv = عدم;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    أرجع OK;
}

/*
 * "islocked()" function
 */
    ساكن فراغ
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*پای_ټکی;
    dictitem_T	*di;

    rettv->vval.v_شمېره = -1;

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    char_u *name = tv_get_string(&argvars[0]);
#که_تعریف_شوی LOG_LOCKVAR
    ch_log(عدم, "LKVAR: f_islocked(): name: %s", name);
#د_که_پای

    lval_ریښه_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_ریښه_T *ریښه = عدم;

    // Set up lval_root if executing in a method.
    إذا (fill_exec_lval_ریښه(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	إذا (fix_variable_reference_lval_ریښه(&aroot, name) == OK)
	    ریښه = &aroot;
    }

    lval_ریښه_T	*lval_ریښه_save = lval_ریښه;
    lval_ریښه = ریښه;
    پای_ټکی = get_lval(name, عدم, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_پیل);
    lval_ریښه = lval_ریښه_save;

    إذا (پای_ټکی != عدم && lv.ll_name != عدم)
    {
	إذا (*پای_ټکی != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_پای_ټکی
		   ? e_invalid_دلیل_str : e_trailing_characters_str), پای_ټکی);
	}
	وإلا
	{
	    إذا (lv.ll_tv == عدم)
	    {
		di = پیدا_کړه_var(lv.ll_name, عدم, TRUE);
		إذا (di != عدم)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_شمېره = ((di->di_flags & DI_FLAGS_قلف)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    وإلا إذا (lv.ll_is_ریښه)
	    {
		rettv->vval.v_شمېره = tv_islocked(lv.ll_tv);
	    }
	    وإلا إذا (lv.ll_object != عدم)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_شمېره = tv_islocked(tv);
#که_تعریف_شوی LOG_LOCKVAR
		ch_log(عدم, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#د_که_پای
	    }
	    وإلا إذا (lv.ll_class != عدم)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_شمېره = tv_islocked(tv);
#که_تعریف_شوی LOG_LOCKVAR
		ch_log(عدم, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#د_که_پای
	    }
	    وإلا إذا (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    وإلا إذا (lv.ll_newkey != عدم)
		semsg(_(e_کلی_not_present_دننه_dictionary_str), lv.ll_newkey);
	    وإلا إذا (lv.ll_list != عدم)
		// List item.
		rettv->vval.v_شمېره = tv_islocked(&lv.ll_li->li_tv);
	    وإلا إذا (lv.ll_tuple != عدم)
		// Tuple item.
		rettv->vval.v_شمېره = tv_islocked(lv.ll_tv);
	    وإلا
		// Dictionary item.
		rettv->vval.v_شمېره = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    إذا (ریښه != عدم)
	free_lval_ریښه(ریښه);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    ساكن فراغ
f_توکي(typval_T *argvars, typval_T *rettv)
{
    إذا (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    وإلا إذا (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    وإلا إذا (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    وإلا إذا (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    وإلا إذا (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    وإلا
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_دلیل_nr), 1);
}

/*
 * "keytrans()" function
 */
    ساكن فراغ
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    إذا (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == عدم)
	أرجع;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    ساكن فراغ
f_وروستی_بفر_nr(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    صحيح		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	إذا (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_شمېره = n;
}

/*
 * "len()" function
 */
    فراغ
f_len(typval_T *argvars, typval_T *rettv)
{
    بدل (argvars[0].v_type)
    {
	حالة VAR_STRING:
	حالة VAR_شمېره:
	    rettv->vval.v_شمېره = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    اكسر;
	حالة VAR_BLOB:
	    rettv->vval.v_شمېره = blob_len(argvars[0].vval.v_blob);
	    اكسر;
	حالة VAR_LIST:
	    rettv->vval.v_شمېره = list_len(argvars[0].vval.v_list);
	    اكسر;
	حالة VAR_TUPLE:
	    rettv->vval.v_شمېره = tuple_len(argvars[0].vval.v_tuple);
	    اكسر;
	حالة VAR_DICT:
	    rettv->vval.v_شمېره = dict_len(argvars[0].vval.v_dict);
	    اكسر;
	حالة VAR_OBJECT:
	    rettv->vval.v_شمېره = object_len(argvars[0].vval.v_object);
	    اكسر;
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	حالة VAR_FLOAT:
	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	حالة VAR_دنده:
	حالة VAR_CHANNEL:
	حالة VAR_INSTR:
	حالة VAR_CLASS:
	حالة VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    اكسر;
    }
}

    ساكن فراغ
libcall_common(typval_T *argvars ناکارول_شوی, typval_T *rettv, صحيح type)
{
#که_تعریف_شوی FEAT_LIBCALL
    char_u		*string_دننه;
    char_u		**string_پایله;
    صحيح			nr_پایله;
#د_که_پای

    rettv->v_type = type;
    إذا (type != VAR_شمېره)
	rettv->vval.v_string = عدم;

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_شمېره_arg(argvars, 2) == FAIL))
	أرجع;

#که_تعریف_شوی FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    إذا (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_دننه = عدم;
	إذا (argvars[2].v_type == VAR_STRING)
	    string_دننه = argvars[2].vval.v_string;
	إذا (type == VAR_شمېره)
	{
	    string_پایله = عدم;
	}
	وإلا
	{
	    rettv->vval.v_string = عدم;
	    string_پایله = &rettv->vval.v_string;
	}
	إذا (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_دننه,
			     argvars[2].vval.v_شمېره,
			     string_پایله,
			     &nr_پایله) == OK
		&& type == VAR_شمېره)
	    rettv->vval.v_شمېره = nr_پایله;
    }
#د_که_پای
}

/*
 * "libcall()" function
 */
    ساكن فراغ
f_libcall(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_STRING);
}

/*
 * "libcallnr()" function
 */
    ساكن فراغ
f_libcallnr(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_شمېره);
}

/*
 * "line(string, [winid])" function
 */
    ساكن فراغ
f_کرښه(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum = 0;
    pos_T	*fp = عدم;
    صحيح		fnum;
    صحيح		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (صحيح)tv_get_شمېره(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	إذا (wp != عدم && tp != عدم)
	{
	    إذا (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		إذا (*p_spk != 'c'
#که_تعریف_شوی FEAT_DIFF
		|| (curwin->w_p_diff && switchwin.sw_curwin->w_p_diff)
#د_که_پای
		)
		    skip_update_topline = TRUE;
		check_cursor();
		fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	    }
	    skip_update_topline = FALSE;
	    restore_win_noblock(&switchwin, TRUE);
	}
    }
    وإلا
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    إذا (fp != عدم)
	lnum = fp->lnum;
    rettv->vval.v_شمېره = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    ساكن فراغ
f_line2byte(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
#که_نه_تعریف_شوی FEAT_BYTEOFF
    rettv->vval.v_شمېره = -1;
#وإلا
    linenr_T	lnum;

    إذا (دننه_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	أرجع;

    lnum = tv_get_lnum(argvars);
    إذا (lnum < 1 || lnum > curbuf->b_ml.ml_کرښه_شمېر + 1)
	rettv->vval.v_شمېره = -1;
    وإلا
	rettv->vval.v_شمېره = ml_پیدا_کړه_کرښه_or_دننه_واټن(curbuf, lnum, عدم);
    إذا (rettv->vval.v_شمېره >= 0)
	++rettv->vval.v_شمېره;
#د_که_پای
}

#که_تعریف_شوی FEAT_LUA
/*
 * "luaeval()" function
 */
    ساكن فراغ
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#د_که_پای

تعريفنوع تعداد
{
    سمون_پای_ټکی_2,	    // matchend()
    سمون_سمون,    // match()
    سمون_STR,	    // matchstr()
    سمون_LIST,	    // matchlist()
    سمون_POS	    // matchstrpos()
} matchtype_T;

    ساكن فراغ
پیدا_کړه_some_سمون(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = عدم;
    اوږد	len = 0;
    char_u	*بیان = عدم;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    اوږد	پیل = 0;
    اوږد	nth = 1;
    colnr_T	startcol = 0;
    صحيح		سمون = 0;
    list_T	*l = عدم;
    listitem_T	*li = عدم;
    اوږد	idx = 0;
    صحيح		prev_قلف = 0;
    char_u	*tofree = عدم;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = تشه_option;

    rettv->vval.v_شمېره = -1;
    إذا (type == سمون_LIST || type == سمون_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	إذا (rettv_list_alloc(rettv) == FAIL)
	    اذهب theend;
	إذا (type == سمون_POS
		&& (list_append_string(rettv->vval.v_list,
					    (char_u *)"", 0) == FAIL
		    || list_append_شمېره(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_شمېره(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_شمېره(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL))
	{
		list_free(rettv->vval.v_list);
		rettv->vval.v_list = عدم;
		اذهب theend;
	}
    }
    وإلا إذا (type == سمون_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = عدم;
    }

    إذا (دننه_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_شمېره_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_شمېره_arg(argvars, 3) == FAIL)))
	اذهب theend;

    إذا (argvars[0].v_type == VAR_LIST)
    {
	إذا ((l = argvars[0].vval.v_list) == عدم)
	    اذهب theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_لومړی;
    }
    وإلا
    {
	بیان = str = tv_get_string(&argvars[0]);
	len = (اوږد)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    إذا (pat == عدم)
	اذهب theend;

    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	صحيح	    error = FALSE;

	پیل = (اوږد)tv_get_شمېره_chk(&argvars[2], &error);
	إذا (error)
	    اذهب theend;
	إذا (l != عدم)
	{
	    li = list_پیدا_کړه(l, پیل);
	    إذا (li == عدم)
		اذهب theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	وإلا
	{
	    إذا (پیل < 0)
		پیل = 0;
	    إذا (پیل > len)
		اذهب theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    إذا (argvars[3].v_type != VAR_UNKNOWN)
		startcol = پیل;
	    وإلا
	    {
		str += پیل;
		len -= پیل;
	    }
	}

	إذا (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (اوږد)tv_get_شمېره_chk(&argvars[3], &error);
	إذا (error)
	    اذهب theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    إذا (regmatch.regprog != عدم)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	إذا (l != عدم)
	{
	    prev_قلف = l->lv_قلف;
	    إذا (l->lv_قلف == 0)
		l->lv_قلف = VAR_LOCKED;
	}

	لكل (;;)
	{
	    إذا (l != عدم)
	    {
		إذا (li == عدم)
		{
		    سمون = FALSE;
		    اكسر;
		}
		vim_free(tofree);
		بیان = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		إذا (str == عدم)
		    اكسر;
	    }

	    سمون = vim_regexec_nl(&regmatch, str, startcol);

	    إذا (سمون && --nth <= 0)
		اكسر;
	    إذا (l == عدم && !سمون)
		اكسر;

	    // Advance to just after the match.
	    إذا (l != عدم)
	    {
		li = li->li_بل;
		++idx;
	    }
	    وإلا
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		إذا (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    سمون = FALSE;
		    اكسر;
		}
	    }
	}

	إذا (سمون)
	{
	    إذا (type == سمون_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_لومړی;
		listitem_T *li2 = li1->li_بل;
		listitem_T *li3 = li2->li_بل;
		listitem_T *li4 = li3->li_بل;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_شمېره =
				      (varnumber_T)(regmatch.startp[0] - بیان);
		li4->li_tv.vval.v_شمېره =
					(varnumber_T)(regmatch.endp[0] - بیان);
		إذا (l != عدم)
		    li2->li_tv.vval.v_شمېره = (varnumber_T)idx;
	    }
	    وإلا إذا (type == سمون_LIST)
	    {
		صحيح i;

		// return list with matched string and submatches
		لكل (i = 0; i < NSUBEXP; ++i)
		{
		    إذا (regmatch.endp[i] == عدم)
		    {
			إذا (list_append_string(rettv->vval.v_list,
							    عدم, 0) == FAIL)
			    اكسر;
		    }
		    وإلا إذا (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(صحيح)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			اكسر;
		}
	    }
	    وإلا إذا (type == سمون_STR)
	    {
		// return matched string
		إذا (l != عدم)
		    copy_tv(&li->li_tv, rettv);
		وإلا
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    وإلا إذا (l != عدم)
		rettv->vval.v_شمېره = idx;
	    وإلا
	    {
		إذا (type != سمون_پای_ټکی_2)
		    rettv->vval.v_شمېره =
				      (varnumber_T)(regmatch.startp[0] - str);
		وإلا
		    rettv->vval.v_شمېره =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_شمېره += (varnumber_T)(str - بیان);
	    }
	}
	إذا (l != عدم)
	    l->lv_قلف = prev_قلف;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    إذا (type == سمون_POS && l == عدم && rettv->vval.v_list != عدم)
	// matchstrpos() without a list: drop the second item.
	listitem_remove(rettv->vval.v_list,
				       rettv->vval.v_list->lv_لومړی->li_بل);
    vim_free(tofree);
    p_cpo = save_cpo;
}

/*
 * Return all the matches in string "str" for pattern "rmp".
 * The matches are returned in the List "mlist".
 * If "submatches" is TRUE, then submatch information is also returned.
 * "matchbuf" is TRUE when called for matchbufline().
 */
    ساكن صحيح
get_matches_دننه_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    صحيح		idx,
    صحيح		submatches,
    صحيح		matchbuf)
{
    اوږد	len = (اوږد)STRLEN(str);
    صحيح		سمون = 0;
    colnr_T	startidx = 0;

    لكل (;;)
    {
	سمون = vim_regexec_nl(rmp, str, startidx);
	إذا (!سمون)
	    اكسر;

	dict_T *d = dict_alloc();
	إذا (d == عدم)
	    أرجع FAIL;
	إذا (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    أرجع FAIL;
	}

	إذا (dict_جمع_کړه_شمېره(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    أرجع FAIL;

	إذا (dict_جمع_کړه_شمېره(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    أرجع FAIL;

	إذا (dict_جمع_کړه_string_len(d, "text", rmp->startp[0],
		    (صحيح)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    أرجع FAIL;

	إذا (submatches)
	{
	    list_T *sml = list_alloc();
	    إذا (sml == عدم)
		أرجع FAIL;

	    إذا (dict_جمع_کړه_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		أرجع FAIL;
	    }

	    // return a list with the submatches
	    لكل (صحيح i = 1; i < NSUBEXP; ++i)
	    {
		إذا (rmp->endp[i] == عدم)
		{
		    إذا (list_append_string(sml, (char_u *)"", 0) == FAIL)
			أرجع FAIL;
		}
		وإلا إذا (list_append_string(sml, rmp->startp[i],
			    (صحيح)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    أرجع FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	إذا (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    اكسر;
    }

    أرجع OK;
}

/*
 * "matchbufline()" function
 */
    ساكن فراغ
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = عدم;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_شمېره = -1;
    إذا (rettv_list_alloc(rettv) != OK)
	أرجع;
    retlist = rettv->vval.v_list;

    إذا (check_for_بفر_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	أرجع;

    صحيح prev_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
    {
	إذا (did_emsg == prev_did_emsg)
	    semsg(_(e_invalid_بفر_name_str), tv_get_string(&argvars[0]));
	أرجع;
    }
    إذا (buf->b_ml.ml_mfp == عدم)
    {
	emsg(_(e_بفر_is_not_loaded));
	أرجع;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    صحيح		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    إذا (did_emsg > did_emsg_before)
	أرجع;
    إذا (slnum < 1)
    {
	semsg(_(e_invalid_ارزښت_for_دلیل_str), "lnum");
	أرجع;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    إذا (did_emsg > did_emsg_before)
	أرجع;
    إذا (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_ارزښت_for_دلیل_str), "end_lnum");
	أرجع;
    }

    إذا (elnum > buf->b_ml.ml_کرښه_شمېر)
	elnum = buf->b_ml.ml_کرښه_شمېر;

    صحيح		submatches = FALSE;
    إذا (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	إذا (d != عدم)
	{
	    dictitem_T *di = dict_پیدا_کړه(d, (char_u *)"submatches", -1);
	    إذا (di != عدم)
	    {
		إذا (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_ارزښت_for_دلیل_str), "submatches");
		    أرجع;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = تشه_option;

    regmatch.regprog = eval_regcomp(pat);
    إذا (regmatch.regprog == عدم)
	اذهب theend;
    regmatch.rm_ic = p_ic;

    طالما (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	إذا (get_matches_دننه_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    اذهب cleanup;
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
    ساكن فراغ
f_سمون(typval_T *argvars, typval_T *rettv)
{
    پیدا_کړه_some_سمون(argvars, rettv, سمون_سمون);
}

/*
 * "matchend()" function
 */
    ساكن فراغ
f_matchend(typval_T *argvars, typval_T *rettv)
{
    پیدا_کړه_some_سمون(argvars, rettv, سمون_پای_ټکی_2);
}

/*
 * "matchlist()" function
 */
    ساكن فراغ
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    پیدا_کړه_some_سمون(argvars, rettv, سمون_LIST);
}

/*
 * "matchstr()" function
 */
    ساكن فراغ
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    پیدا_کړه_some_سمون(argvars, rettv, سمون_STR);
}

/*
 * "matchstrlist()" function
 */
    ساكن فراغ
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = عدم;
    char_u	*save_cpo;
    list_T	*l = عدم;
    listitem_T	*li = عدم;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_شمېره = -1;
    إذا (rettv_list_alloc(rettv) != OK)
	أرجع;
    retlist = rettv->vval.v_list;

    إذا (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	أرجع;

    إذا ((l = argvars[0].vval.v_list) == عدم)
	أرجع;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    إذا (pat == عدم)
	أرجع;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = تشه_option;

    regmatch.regprog = eval_regcomp(pat);
    إذا (regmatch.regprog == عدم)
	اذهب theend;
    regmatch.rm_ic = p_ic;

    صحيح		submatches = FALSE;
    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	إذا (d != عدم)
	{
	    dictitem_T *di = dict_پیدا_کړه(d, (char_u *)"submatches", -1);
	    إذا (di != عدم)
	    {
		إذا (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_ارزښت_for_دلیل_str), "submatches");
		    اذهب cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    صحيح idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_توکي(l, li)
    {
	إذا (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != عدم)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    إذا (get_matches_دننه_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		اذهب cleanup;
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
    ساكن فراغ
f_matchstrpos(typval_T *argvars, typval_T *rettv)
{
    پیدا_کړه_some_سمون(argvars, rettv, سمون_POS);
}

    ساكن فراغ
max_min(typval_T *argvars, typval_T *rettv, صحيح domax)
{
    varnumber_T	n = 0;

    إذا (دننه_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_شمېره = 0;

    إذا (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = عدم;

	l = argvars[0].vval.v_list;
	إذا (l != عدم && l->lv_len > 0)
	{
	    إذا (l->lv_لومړی == &range_list_توکی)
	    {
		إذا ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_پیل;
		وإلا
		    n = l->lv_u.nonmat.lv_پیل + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_شمېره = n;
	    }
	    وإلا
	    {
		FOR_ALL_LIST_توکي(l, li)
		{
		    إذا (tv == عدم)
			tv = &li->li_tv;
		    وإلا
		    {
			صحيح res;
			إذا (typval_compare2(&li->li_tv, tv,
				domax ? بیان_GREATER : بیان_SMALLER, FALSE, &res) == FAIL)
			    أرجع;
			إذا (res == OK)
			    tv = &li->li_tv;
		    }
		}

		إذا (tv != عدم)
		    copy_tv(tv, rettv);
	    }
	}
    }
    وإلا إذا (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	إذا (tv != عدم)
	    copy_tv(tv, rettv);
    }
    وإلا إذا (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	صحيح		todo;
	typval_T	*tv = عدم;

	d = argvars[0].vval.v_dict;
	إذا (d != عدم)
	{
	    todo = (صحيح)d->dv_hashtab.ht_کارول_شوی;
	    FOR_ALL_HASHTAB_توکي(&d->dv_hashtab, hi, todo)
	    {
		إذا (!HASHITEM_تشه(hi))
		{
		    --todo;
		    إذا (tv == عدم)
			tv = &HI2DI(hi)->di_tv;
		    وإلا
		    {
			صحيح res;
			إذا (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? بیان_GREATER : بیان_SMALLER, FALSE, &res) == FAIL)
			    أرجع;
			إذا (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	إذا (tv != عدم)
	    copy_tv(tv, rettv);
    }
    وإلا
	semsg(_(e_دلیل_د_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
}

/*
 * "max()" function
 */
    ساكن فراغ
f_max(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, TRUE);
}

/*
 * "min()" function
 */
    ساكن فراغ
f_min(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, FALSE);
}

#إذا defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    ساكن فراغ
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    فراغ
mzscheme_call_vim(char_u *name, typval_T *دلیلونه, typval_T *rettv)
{
    typval_T argvars[3];

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = name;
    copy_tv(دلیلونه, &argvars[1]);
    argvars[2].v_type = VAR_UNKNOWN;
    f_call(argvars, rettv);
    clear_tv(&argvars[1]);
}
#د_که_پای

/*
 * "nextnonblank()" function
 */
    ساكن فراغ
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    إذا (دننه_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	أرجع;

    لكل (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	إذا (lnum < 0 || lnum > curbuf->b_ml.ml_کرښه_شمېر)
	{
	    lnum = 0;
	    اكسر;
	}
	إذا (*skipwhite(ml_get(lnum)) != NUL)
	    اكسر;
    }
    rettv->vval.v_شمېره = lnum;
}


/*
 * "ngettext()" function
 */
    ساكن فراغ
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#إذا defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    حرف *prev = عدم;
#د_که_پای

    إذا (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_شمېره_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	أرجع;

    rettv->v_type = VAR_STRING;

    إذا (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != عدم &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#إذا defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ثابت حرف *)argvars[3].vval.v_string, (حرف *)p_enc);
#د_که_پای

#إذا defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((ثابت حرف *)argvars[3].vval.v_string, (ثابت حرف *)argvars[0].vval.v_string, (ثابت حرف *)argvars[1].vval.v_string, (صحيح)argvars[2].vval.v_شمېره));
#وإلا
	textdomain((ثابت حرف *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ثابت حرف *)argvars[0].vval.v_string, (ثابت حرف *)argvars[1].vval.v_string, argvars[2].vval.v_شمېره));
	textdomain(VIMPACKAGE);
#د_که_پای

#إذا defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	إذا (prev != عدم)
	    bind_textdomain_codeset((ثابت حرف *)argvars[3].vval.v_string, prev);
#د_که_پای
    }
    وإلا
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ثابت حرف *)argvars[0].vval.v_string, (ثابت حرف *)argvars[1].vval.v_string, argvars[2].vval.v_شمېره));
}


/*
 * "nr2char()" function
 */
    ساكن فراغ
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (has_mbyte)
    {
	صحيح	utf8 = 0;

	إذا (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (صحيح)tv_get_bool_chk(&argvars[1], عدم);
	إذا (utf8)
	    buf[utf_char2bytes((صحيح)tv_get_شمېره(&argvars[0]), buf)] = NUL;
	وإلا
	    buf[(*mb_char2bytes)((صحيح)tv_get_شمېره(&argvars[0]), buf)] = NUL;
    }
    وإلا
    {
	buf[0] = (char_u)tv_get_شمېره(&argvars[0]);
	buf[1] = NUL;
    }
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "or(expr, expr)" function
 */
    ساكن فراغ
f_or(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->vval.v_شمېره = tv_get_شمېره_chk(&argvars[0], عدم)
					| tv_get_شمېره_chk(&argvars[1], عدم);
}

#که_تعریف_شوی FEAT_PERL
/*
 * "perleval()" function
 */
    ساكن فراغ
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#د_که_پای

/*
 * "prevnonblank()" function
 */
    ساكن فراغ
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    إذا (دننه_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	أرجع;

    lnum = tv_get_lnum(argvars);
    إذا (lnum < 1 || lnum > curbuf->b_ml.ml_کرښه_شمېر)
	lnum = 0;
    وإلا
	طالما (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
	    --lnum;
    rettv->vval.v_شمېره = lnum;
}

// This dummy va_list is here because:
// - passing a NULL pointer doesn't work when va_list isn't a pointer
// - locally in the function results in a "used before set" warning
// - using va_start() to initialize it gives "function with fixed args" error
ساكن va_list	ap;

/*
 * "printf()" function
 */
    ساكن فراغ
f_printf(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    صحيح		len;
    char_u	*s;
    صحيح		saved_did_emsg = did_emsg;
    حرف	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (دننه_vim9script() && check_for_string_or_شمېره_arg(argvars, 0) == FAIL)
	أرجع;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (حرف *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(عدم, 0, fmt, ap, argvars + 1);
    إذا (!did_emsg)
    {
	s = alloc(len + 1);
	إذا (s != عدم)
	{
	    rettv->vval.v_string = s;
	    (فراغ)vim_vsnprintf_typval((حرف *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    ساكن فراغ
f_pum_getpos(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    pum_سېټ_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    ساكن فراغ
f_pumvisible(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
    إذا (pum_visible())
	rettv->vval.v_شمېره = 1;
}

#که_تعریف_شوی FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    ساكن فراغ
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (p_pyx == 0)
	p_pyx = 3;

    إذا (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    وإلا إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	أرجع;
    }
    وإلا
    {
	locals = عدم;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#د_که_پای

#که_تعریف_شوی FEAT_PYTHON
/*
 * "pyeval()" function
 */
    ساكن فراغ
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (دننه_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	أرجع;

    إذا (p_pyx == 0)
	p_pyx = 2;

    إذا (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    وإلا إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	أرجع;
    }
    وإلا
    {
	locals = عدم;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#د_که_پای

#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    ساكن فراغ
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

# إذا defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    إذا (p_pyx == 2)
	f_pyeval(argvars, rettv);
    وإلا
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#د_که_پای

ساكن UINT32_T srand_تخم_for_testing = 0;
ساكن صحيح	srand_تخم_for_testing_is_کارول_شوی = FALSE;

    ساكن فراغ
f_test_srand_تخم(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    إذا (دننه_vim9script() && check_for_opt_شمېره_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_UNKNOWN)
	srand_تخم_for_testing_is_کارول_شوی = FALSE;
    وإلا
    {
	srand_تخم_for_testing = (UINT32_T)tv_get_شمېره(&argvars[0]);
	srand_تخم_for_testing_is_کارول_شوی = TRUE;
    }
}

    ساكن فراغ
init_srand(UINT32_T *x)
{
    بنية {
	اتحاد {
	    UINT32_T شمېره;
	    char_u   بایټونه[حجم(UINT32_T)];
	} contents;
    } buf;

    إذا (srand_تخم_for_testing_is_کارول_شوی)
    {
	*x = srand_تخم_for_testing;
	أرجع;
    }

    إذا (mch_get_random(buf.contents.بایټونه, حجم(buf.contents.بایټونه)) == OK)
    {
	*x = buf.contents.شمېره;
	أرجع;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#إذا defined(FEAT_SODIUM)
    إذا (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    وإلا
#د_که_پای
    {
#إذا defined(FEAT_RELTIME)
	proftime_T res;
	profile_پیل(&res);
# إذا defined(MSWIN)
	*x = (UINT32_T)res.ټیټ_Part;
# وإلا
	*x = (UINT32_T)res.tv_fsec;
# endif
#وإلا
	*x = vim_وخت();
#د_که_پای
	*x ^= mch_get_pid();
    }
}

#تعریف_کړه ROTL(x, k) (((x) << (k)) | ((x) >> (32 - (k))))
#تعریف_کړه SPLITMIX32(x, z) ( \
    (z) = ((x) += 0x9e3779b9), \
    (z) = ((z) ^ ((z) >> 16)) * 0x85ebca6b, \
    (z) = ((z) ^ ((z) >> 13)) * 0xc2b2ae35, \
    (z) ^ ((z) >> 16) \
    )
#تعریف_کړه SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w) \
    پایله = ROTL((y) * 5, 7) * 9; \
    t = (y) << 9; \
    (z) ^= (x); \
    (w) ^= (y); \
    (y) ^= (z), (x) ^= (w); \
    (z) ^= t; \
    (w) = ROTL(w, 11);

/*
 * "rand()" function
 */
    ساكن فراغ
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = عدم;
    ساكن UINT32_T	gx, gy, gz, gw;
    ساكن صحيح	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, پایله;

    إذا (دننه_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_UNKNOWN)
    {
	// When no argument is given use the global seed list.
	إذا (initialized == FALSE)
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
    وإلا إذا (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	إذا (l == عدم || list_len(l) != 4)
	    اذهب theend;

	lx = list_پیدا_کړه(l, 0L);
	ly = list_پیدا_کړه(l, 1L);
	lz = list_پیدا_کړه(l, 2L);
	lw = list_پیدا_کړه(l, 3L);
	إذا (lx == عدم || ly == عدم || lz == عدم || lw == عدم)
	    اذهب theend;
	إذا (lx->li_tv.v_type != VAR_شمېره
		|| ly->li_tv.v_type != VAR_شمېره
		|| lz->li_tv.v_type != VAR_شمېره
		|| lw->li_tv.v_type != VAR_شمېره)
	    اذهب theend;
	x = (UINT32_T)lx->li_tv.vval.v_شمېره;
	y = (UINT32_T)ly->li_tv.vval.v_شمېره;
	z = (UINT32_T)lz->li_tv.vval.v_شمېره;
	w = (UINT32_T)lw->li_tv.vval.v_شمېره;

	SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w);

	lx->li_tv.vval.v_شمېره = (varnumber_T)x;
	ly->li_tv.vval.v_شمېره = (varnumber_T)y;
	lz->li_tv.vval.v_شمېره = (varnumber_T)z;
	lw->li_tv.vval.v_شمېره = (varnumber_T)w;
    }
    وإلا
	اذهب theend;

    rettv->v_type = VAR_شمېره;
    rettv->vval.v_شمېره = (varnumber_T)پایله;
    أرجع;

theend:
    semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[0]));
    rettv->v_type = VAR_شمېره;
    rettv->vval.v_شمېره = -1;
}

/*
 * "srand()" function
 */
    ساكن فراغ
f_srand(typval_T *argvars, typval_T *rettv)
{
    UINT32_T x = 0, z;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (دننه_vim9script() && check_for_opt_شمېره_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    وإلا
    {
	صحيح	    error = FALSE;

	x = (UINT32_T)tv_get_شمېره_chk(&argvars[0], &error);
	إذا (error)
	    أرجع;
    }

    list_append_شمېره(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_شمېره(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_شمېره(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_شمېره(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#تعریف_لغوه ROTL
#تعریف_لغوه SPLITMIX32
#تعریف_لغوه SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    ساكن فراغ
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	پیل;
    varnumber_T	پای_ټکی;
    varnumber_T	stride = 1;
    صحيح		error = FALSE;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_opt_شمېره_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_شمېره_arg(argvars, 2) == FAIL)))
	أرجع;

    پیل = tv_get_شمېره_chk(&argvars[0], &error);
    إذا (argvars[1].v_type == VAR_UNKNOWN)
    {
	پای_ټکی = پیل - 1;
	پیل = 0;
    }
    وإلا
    {
	پای_ټکی = tv_get_شمېره_chk(&argvars[1], &error);
	إذا (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_شمېره_chk(&argvars[2], &error);
    }

    إذا (error)
	أرجع;		// type error; errmsg already given
    إذا (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	أرجع;
    }

    // The stride is stored in "lv_stride", which is an int.
    إذا (stride < INT_MIN || stride > لوی_عدد)
    {
	حرف	buf[NUMBUFLEN];

	vim_snprintf(buf, حجم(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	أرجع;
    }

    uvarnumber_T	len;

    إذا (stride > 0 ? پای_ټکی < پیل : پای_ټکی > پیل)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	شا = stride > 0
				  ? (uvarnumber_T)پیل - (uvarnumber_T)پای_ټکی
				  : (uvarnumber_T)پای_ټکی - (uvarnumber_T)پیل;

	إذا (شا > 1)
	{
	    emsg(_(e_پیل_past_پای_ټکی));
	    أرجع;
	}
	len = 0;
    }
    وإلا
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	پراختیا = stride > 0
				  ? (uvarnumber_T)پای_ټکی - (uvarnumber_T)پیل
				  : (uvarnumber_T)پیل - (uvarnumber_T)پای_ټکی;
	uvarnumber_T	شمېر = پراختیا / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	إذا (شمېر >= (uvarnumber_T)لوی_عدد)
	{
	    حرف	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, حجم(buf), "%llu",
			      شمېر < UVARNUM_MAX ? شمېر + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    أرجع;
	}
	len = شمېر + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_لومړی = &range_list_توکی;
    list->lv_u.nonmat.lv_پیل = پیل;
    list->lv_u.nonmat.lv_پای_ټکی = پای_ټکی;
    list->lv_u.nonmat.lv_stride = (صحيح)stride;
    list->lv_len = (صحيح)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    فراغ
range_list_materialize(list_T *list)
{
    varnumber_T پیل = list->lv_u.nonmat.lv_پیل;
    varnumber_T پای_ټکی = list->lv_u.nonmat.lv_پای_ټکی;
    صحيح		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_لومړی = عدم;
    list->lv_u.mat.lv_وروستی = عدم;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_توکی = عدم;
    لكل (i = پیل; stride > 0 ? i <= پای_ټکی : i >= پای_ټکی; i += stride)
    {
	إذا (list_append_شمېره(list, i) == FAIL)
	    اكسر;
	إذا (list->lv_قلف & VAR_توکي_LOCKED)
	    list->lv_u.mat.lv_وروستی->li_tv.v_قلف = VAR_LOCKED;
    }
    list->lv_قلف &= ~VAR_توکي_LOCKED;
}

/*
 * "getreginfo()" function
 */
    ساكن فراغ
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    صحيح		regname;
    char_u	buf[NUMBUFLEN + 2];
    size_t	buflen;
    اوږد	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    إذا (دننه_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	أرجع;

    regname = getreg_get_regname(argvars);
    إذا (regname == 0)
	أرجع;

    إذا (regname == '@')
	regname = '"';

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_بیان_SRC | GREG_LIST);
    إذا (list == عدم)
	أرجع;
    إذا (dict_جمع_کړه_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    بدل (get_reg_type(regname, &reglen))
    {
	حالة MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    اكسر;
	حالة MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    اكسر;
	حالة MBLOCK:
	    buflen = vim_snprintf_safelen((حرف *)buf, حجم(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    اكسر;
	افتراضي:
	    buf[0] = NUL;
	    buflen = 0;
	    اكسر;
    }
    (فراغ)dict_جمع_کړه_string_len(dict, (حرف *)"regtype", buf, (صحيح)buflen);

    buf[0] = get_ثبتوونکی_name(get_unname_ثبتوونکی());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    إذا (regname == '"')
	(فراغ)dict_جمع_کړه_string_len(dict, (حرف *)"points_to", buf, (صحيح)buflen);
    وإلا
    {
	dictitem_T	*توکی = dictitem_alloc((char_u *)"isunnamed");

	إذا (توکی != عدم)
	{
	    توکی->di_tv.v_type = VAR_BOOL;
	    توکی->di_tv.vval.v_شمېره = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (فراغ)dict_جمع_کړه(dict, توکی);
	}
    }
}

    ساكن فراغ
return_ثبتوونکی(صحيح regname, typval_T *rettv)
{
    char_u buf[2] = {NUL, NUL};

    buf[0] = (char_u)regname;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(buf, (buf[0] == NUL) ? 0 : 1);
}

/*
 * "reg_executing()" function
 */
    ساكن فراغ
f_reg_executing(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    return_ثبتوونکی(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    ساكن فراغ
f_reg_recording(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    return_ثبتوونکی(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    ساكن فراغ
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_شمېره = -1;
    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->vval.v_شمېره = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    ساكن فراغ
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    صحيح slen = list_len(l);

    إذا (rettv_list_alloc(rettv) == FAIL
	    || l == عدم
	    || n <= 0
	    || slen == 0)
	أرجع;

    إذا (check_repeat_شمېر(slen, n) == FAIL)
	أرجع;

    طالما (n-- > 0)
	إذا (list_extend(rettv->vval.v_list, l, عدم) == FAIL)
	    اكسر;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    ساكن فراغ
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    صحيح		slen;
    صحيح		len;
    صحيح		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    إذا (rettv_blob_alloc(rettv) == FAIL
	    || blob == عدم
	    || n <= 0)
	أرجع;

    slen = blob->bv_ga.ga_len;
    إذا (slen <= 0)
	أرجع;
    إذا (check_repeat_شمېر(slen, n) == FAIL)
	أرجع;
    len = slen * (صحيح)n;

    إذا (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	أرجع;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    لكل (i = 0; i < slen; ++i)
	إذا (blob_get(blob, i) != 0)
	    اكسر;

    إذا (i == slen)
	// No need to copy since all bytes are already zero
	أرجع;

    لكل (i = 0; i < (صحيح)n; ++i)
	blob_سېټ_range(rettv->vval.v_blob,
		(اوږد)i * slen, ((اوږد)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    ساكن فراغ
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    size_t	slen;
    صحيح		len;
    char_u	*r;
    صحيح		بشپړ;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    slen = STRLEN(p);
    إذا (slen == 0 || n <= 0)
	أرجع;
    إذا (check_repeat_شمېر((varnumber_T)slen, n) == FAIL)
	أرجع;
    len = (صحيح)slen * (صحيح)n;

    r = alloc((size_t)len + 1);
    إذا (r == عدم)
	أرجع;

    mch_memmove(r, p, slen);
    بشپړ = (صحيح)slen;
    طالما (بشپړ < len)
    {
	صحيح copy_len = بشپړ;

	إذا (copy_len > len - بشپړ)
	    copy_len = len - بشپړ;
	mch_memmove(r + بشپړ, r, (size_t)copy_len);
	بشپړ += copy_len;
    }
    r[len] = NUL;

    rettv->vval.v_string = r;
}

/*
 * "repeat()" function
 */
    ساكن فراغ
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    إذا (دننه_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

    n = tv_get_شمېره(&argvars[1]);
    إذا (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    وإلا إذا (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    وإلا إذا (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    وإلا
	repeat_string(&argvars[0], n, rettv);
}

#تعریف_کړه SP_NOMOVE	0x01	    // don't move cursor
#تعریف_کړه SP_REPEAT	0x02	    // repeat to find outer pair
#تعریف_کړه SP_RETCOUNT	0x04	    // return matchcount
#تعریف_کړه SP_SETPCMARK	0x08	    // set previous context mark
#تعریف_کړه SP_پیل	0x10	    // accept match at start position
#تعریف_کړه SP_SUBPAT	0x20	    // return nr of matching sub-pattern
#تعریف_کړه SP_پای_ټکی		0x40	    // leave cursor at end of match
#تعریف_کړه SP_ستنه	0x80	    // start at cursor column

/*
 * Get flags for a search function.
 * Possibly sets "p_ws".
 * Returns BACKWARD, FORWARD or zero (for an error).
 */
    ساكن صحيح
get_پلټنه_arg(typval_T *varp, صحيح *flagsp)
{
    صحيح		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    صحيح		ماسک;

    إذا (varp->v_type == VAR_UNKNOWN)
	أرجع FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    إذا (flags == عدم)
	أرجع 0;		// type error; errmsg already given
    طالما (*flags != NUL)
    {
	بدل (*flags)
	{
	    حالة 'b': dir = BACKWARD; اكسر;
	    حالة 'w': p_ws = TRUE; اكسر;
	    حالة 'W': p_ws = FALSE; اكسر;
	    افتراضي:  ماسک = 0;
		      إذا (flagsp != عدم)
			  بدل (*flags)
			  {
			      حالة 'c': ماسک = SP_پیل; اكسر;
			      حالة 'e': ماسک = SP_پای_ټکی; اكسر;
			      حالة 'm': ماسک = SP_RETCOUNT; اكسر;
			      حالة 'n': ماسک = SP_NOMOVE; اكسر;
			      حالة 'p': ماسک = SP_SUBPAT; اكسر;
			      حالة 'r': ماسک = SP_REPEAT; اكسر;
			      حالة 's': ماسک = SP_SETPCMARK; اكسر;
			      حالة 'z': ماسک = SP_ستنه; اكسر;
			  }
		      إذا (ماسک == 0)
		      {
			  semsg(_(e_invalid_دلیل_str), flags);
			  dir = 0;
		      }
		      وإلا
			  *flagsp |= ماسک;
	}
	إذا (dir == 0)
	    اكسر;
	++flags;
    }
    أرجع dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    ساكن صحيح
پلټنه_cmn(typval_T *argvars, pos_T *سمون_pos, صحيح *flagsp)
{
    صحيح		flags;
    char_u	*pat;
    size_t	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    صحيح		save_p_ws = p_ws;
    صحيح		dir;
    صحيح		retval = 0;	// default: FAIL
    اوږد	lnum_stop = 0;
#که_تعریف_شوی FEAT_RELTIME
    اوږد	وخت_حد = 0;
#د_که_پای
    صحيح		options = پلټنه_KEEP;
    صحيح		subpatnum;
    searchit_arg_T sia;
    صحيح		use_skip = FALSE;
    pos_T	firstpos;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_شمېره_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_شمېره_arg(argvars, 3) == FAIL)))))
	اذهب theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_پلټنه_arg(&argvars[1], flagsp);	// may set p_ws
    إذا (dir == 0)
	اذهب theend;
    flags = *flagsp;
    إذا (flags & SP_پیل)
	options |= پلټنه_پیل;
    إذا (flags & SP_پای_ټکی)
	options |= پلټنه_پای_ټکی;
    إذا (flags & SP_ستنه)
	options |= پلټنه_ستون;

    // Optional arguments: line number to stop searching, timeout and skip.
    إذا (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (اوږد)tv_get_شمېره_chk(&argvars[2], عدم);
	إذا (lnum_stop < 0)
	    اذهب theend;
	إذا (argvars[3].v_type != VAR_UNKNOWN)
	{
#که_تعریف_شوی FEAT_RELTIME
	    وخت_حد = (اوږد)tv_get_شمېره_chk(&argvars[3], عدم);
	    إذا (وخت_حد < 0)
		اذهب theend;
#د_که_پای
	    use_skip = eval_بیان_valid_arg(&argvars[4]);
	}
    }

    /*
     * This function does not accept SP_REPEAT and SP_RETCOUNT flags.
     * Check to make sure only those flags are set.
     * Also, Only the SP_NOMOVE or the SP_SETPCMARK flag can be set. Both
     * flags cannot be set. Check for that condition also.
     */
    إذا (((flags & (SP_REPEAT | SP_RETCOUNT)) != 0)
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[1]));
	اذهب theend;
    }

    pos = save_cursor = curwin->w_cursor;
    CLEAR_ډګر(firstpos);
    CLEAR_ډګر(sia);
    sia.sa_stop_lnum = (linenr_T)lnum_stop;
#که_تعریف_شوی FEAT_RELTIME
    sia.sa_tm = وخت_حد;
#د_که_پای

    patlen = STRLEN(pat);

    // Repeat until {skip} returns FALSE.
    لكل (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, عدم, dir, pat, patlen, 1L,
						     options, RE_پلټنه, &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	إذا (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	إذا (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    اكسر;
	إذا (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    صحيح	    do_skip;
	    صحيح	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_بیان_منزل_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    إذا (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		اكسر;
	    }
	    إذا (!do_skip)
		اكسر;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~پلټنه_پیل;
    }

    إذا (subpatnum != FAIL)
    {
	إذا (flags & SP_SUBPAT)
	    retval = subpatnum;
	وإلا
	    retval = pos.lnum;
	إذا (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	إذا (سمون_pos != عدم)
	{
	    // Store the match cursor position
	    سمون_pos->lnum = pos.lnum;
	    سمون_pos->ستون = pos.ستون + 1;
	}
	// "/$" will put the cursor after the end of the line, may need to
	// correct that here
	check_cursor();
    }

    // If 'n' flag is used: restore cursor position.
    إذا (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    وإلا
	curwin->w_سېټ_curswant = true;
theend:
    p_ws = save_p_ws;

    أرجع retval;
}

#که_تعریف_شوی FEAT_RUBY
/*
 * "rubyeval()" function
 */
    ساكن فراغ
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#د_که_پای

/*
 * "screenattr()" function
 */
    ساكن فراغ
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    صحيح		لیکه;
    صحيح		ستون;
    صحيح		c;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

    لیکه = (صحيح)tv_get_شمېره_chk(&argvars[0], عدم) - 1;
    ستون = (صحيح)tv_get_شمېره_chk(&argvars[1], عدم) - 1;
    إذا (لیکه < 0 || لیکه >= screen_لیکې
	    || ستون < 0 || ستون >= screen_Columns)
	c = -1;
    وإلا
	c = ScreenAttrs[کرښه_دننه_واټن[لیکه] + ستون];
    rettv->vval.v_شمېره = c;
}

/*
 * "screenchar()" function
 */
    ساكن فراغ
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    صحيح		لیکه;
    صحيح		ستون;
    صحيح		c;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

    لیکه = (صحيح)tv_get_شمېره_chk(&argvars[0], عدم) - 1;
    ستون = (صحيح)tv_get_شمېره_chk(&argvars[1], عدم) - 1;
    إذا (لیکه < 0 || لیکه >= screen_لیکې || ستون < 0 || ستون >= screen_Columns)
	c = -1;
    وإلا
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(لیکه, ستون, buf, عدم);
	c = (*mb_ptr2char)(buf);
    }
    rettv->vval.v_شمېره = c;
}

/*
 * "screenchars()" function
 */
    ساكن فراغ
f_screenchars(typval_T *argvars, typval_T *rettv)
{
    صحيح		لیکه;
    صحيح		ستون;
    صحيح		c;
    صحيح		i;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

    لیکه = (صحيح)tv_get_شمېره_chk(&argvars[0], عدم) - 1;
    ستون = (صحيح)tv_get_شمېره_chk(&argvars[1], عدم) - 1;
    إذا (لیکه < 0 || لیکه >= screen_لیکې || ستون < 0 || ستون >= screen_Columns)
	أرجع;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(لیکه, ستون, buf, عدم);
    صحيح pcc[MAX_MCO];
    إذا (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    وإلا
	c = (*mb_ptr2char)(buf);
    list_append_شمېره(rettv->vval.v_list, (varnumber_T)c);

    إذا (enc_utf8)
	لكل (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_شمېره(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    ساكن فراغ
f_screencol(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    rettv->vval.v_شمېره = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    ساكن فراغ
f_screenrow(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    rettv->vval.v_شمېره = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    ساكن فراغ
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    صحيح		لیکه;
    صحيح		ستون;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = عدم;
    rettv->v_type = VAR_STRING;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

    لیکه = (صحيح)tv_get_شمېره_chk(&argvars[0], عدم) - 1;
    ستون = (صحيح)tv_get_شمېره_chk(&argvars[1], عدم) - 1;
    إذا (لیکه < 0 || لیکه >= screen_لیکې || ستون < 0 || ستون >= screen_Columns)
	أرجع;

    screen_getbytes(لیکه, ستون, buf, عدم);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    ساكن فراغ
f_پلټنه(typval_T *argvars, typval_T *rettv)
{
    صحيح		flags = 0;

    rettv->vval.v_شمېره = پلټنه_cmn(argvars, عدم, &flags);
}

/*
 * "searchdecl()" function
 */
    ساكن فراغ
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    صحيح		locally = TRUE;
    صحيح		thisblock = FALSE;
    صحيح		error = FALSE;
    char_u	*name;

    rettv->vval.v_شمېره = 1;	// default: FAIL

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	أرجع;

    name = tv_get_string_chk(&argvars[0]);
    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(صحيح)tv_get_bool_chk(&argvars[1], &error);
	إذا (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (صحيح)tv_get_bool_chk(&argvars[2], &error);
    }
    إذا (!error && name != عدم)
	rettv->vval.v_شمېره = پیدا_کړه_decl(name, (صحيح)STRLEN(name),
				     locally, thisblock, پلټنه_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    ساكن صحيح
searchpair_cmn(typval_T *argvars, pos_T *سمون_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    صحيح		save_p_ws = p_ws;
    صحيح		dir;
    صحيح		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    صحيح		retval = 0;		// default: FAIL
    اوږد	lnum_stop = 0;
    اوږد	وخت_حد = 0;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_شمېره_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_شمېره_arg(argvars, 6) == FAIL)))))
	اذهب theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    إذا (spat == عدم || mpat == عدم || epat == عدم)
	اذهب theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_پلټنه_arg(&argvars[3], &flags); // may set p_ws
    إذا (dir == 0)
	اذهب theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    إذا ((flags & (SP_پای_ټکی | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[3]));
	اذهب theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    إذا (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    إذا (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = عدم;
    وإلا
    {
	// Type is checked later.
	skip = &argvars[4];

	إذا (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (اوږد)tv_get_شمېره_chk(&argvars[5], عدم);
	    إذا (lnum_stop < 0)
	    {
		semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[5]));
		اذهب theend;
	    }
#که_تعریف_شوی FEAT_RELTIME
	    إذا (argvars[6].v_type != VAR_UNKNOWN)
	    {
		وخت_حد = (اوږد)tv_get_شمېره_chk(&argvars[6], عدم);
		إذا (وخت_حد < 0)
		{
		    semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[6]));
		    اذهب theend;
		}
	    }
#د_که_پای
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    سمون_pos, lnum_stop, وخت_حد);

theend:
    p_ws = save_p_ws;

    أرجع retval;
}

/*
 * "searchpair()" function
 */
    ساكن فراغ
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_شمېره = searchpair_cmn(argvars, عدم);
}

/*
 * "searchpairpos()" function
 */
    ساكن فراغ
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	سمون_pos;
    صحيح		lnum = 0;
    صحيح		ستون = 0;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (searchpair_cmn(argvars, &سمون_pos) > 0)
    {
	lnum = سمون_pos.lnum;
	ستون = سمون_pos.ستون;
    }

    list_append_شمېره(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_شمېره(rettv->vval.v_list, (varnumber_T)ستون);
}

/*
 * Search for a start/middle/end thing.
 * Used by searchpair(), see its documentation for the details.
 * Returns 0 or -1 for no match,
 */
    اوږد
do_searchpair(
    char_u	*spat,	    // start pattern
    char_u	*mpat,	    // middle pattern
    char_u	*epat,	    // end pattern
    صحيح		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    صحيح		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*سمون_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    اوږد	وخت_حد ناکارول_شوی) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = عدم, *pat3 = عدم;
    size_t	patlen;
    size_t	spatlen;
    size_t	epatlen;
    size_t	pat2size;
    size_t	pat2len;
    size_t	pat3size;
    size_t	pat3len;
    اوږد	retval = 0;
    pos_T	pos;
    pos_T	firstpos;
    pos_T	foundpos;
    pos_T	save_cursor;
    pos_T	save_pos;
    صحيح		n;
    صحيح		r;
    صحيح		nest = 1;
    صحيح		use_skip = FALSE;
    صحيح		err;
    صحيح		options = پلټنه_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = تشه_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    إذا (pat2 == عدم)
	اذهب theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    إذا (pat3 == عدم)
	اذهب theend;
    pat2len = vim_snprintf((حرف *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    إذا (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    وإلا
	pat3len = vim_snprintf((حرف *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    إذا (flags & SP_پیل)
	options |= پلټنه_پیل;

    إذا (skip != عدم)
	use_skip = eval_بیان_valid_arg(skip);

#که_تعریف_شوی FEAT_RELTIME
    إذا (وخت_حد > 0)
	init_regexp_timeout(وخت_حد);
#د_که_پای
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    لكل (;;)
    {
	searchit_arg_T sia;

	CLEAR_ډګر(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, عدم, dir, pat, patlen, 1L,
						     options, RE_پلټنه, &sia);
	إذا (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    اكسر;

	إذا (firstpos.lnum == 0)
	    firstpos = pos;
	إذا (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    إذا (dir == BACKWARD)
		decl(&pos);
	    وإلا
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~پلټنه_پیل;

	// If the skip pattern matches, ignore this match.
	إذا (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_بیان_منزل_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    إذا (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		اكسر;
	    }
	    إذا (r)
		استمر;
	}

	إذا ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	وإلا
	{
	    // Found end when searching forward or start when searching
	    // backward: end of (nested) pair; or found middle in outer pair.
	    إذا (--nest == 1)
		pat = pat3;	// outer level, search for middle
	}

	إذا (nest == 0)
	{
	    // Found the match: return matchcount or line number.
	    إذا (flags & SP_RETCOUNT)
		++retval;
	    وإلا
		retval = pos.lnum;
	    إذا (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    إذا (!(flags & SP_REPEAT))
		اكسر;
	    nest = 1;	    // search for next unmatched
	}
    }

    إذا (سمون_pos != عدم)
    {
	// Store the match cursor position
	سمون_pos->lnum = curwin->w_cursor.lnum;
	سمون_pos->ستون = curwin->w_cursor.ستون + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    إذا ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#که_تعریف_شوی FEAT_RELTIME
    إذا (وخت_حد > 0)
	disable_regexp_timeout();
#د_که_پای
    vim_free(pat2);
    vim_free(pat3);
    إذا (p_cpo == تشه_option)
	p_cpo = save_cpo;
    وإلا
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	إذا (*p_cpo == NUL)
	    سېټ_option_ارزښت_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    أرجع retval;
}

/*
 * "searchpos()" function
 */
    ساكن فراغ
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	سمون_pos;
    صحيح		lnum = 0;
    صحيح		ستون = 0;
    صحيح		n;
    صحيح		flags = 0;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    n = پلټنه_cmn(argvars, &سمون_pos, &flags);
    إذا (n > 0)
    {
	lnum = سمون_pos.lnum;
	ستون = سمون_pos.ستون;
    }

    list_append_شمېره(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_شمېره(rettv->vval.v_list, (varnumber_T)ستون);
    إذا (flags & SP_SUBPAT)
	list_append_شمېره(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ساكن فراغ
سېټ_position(typval_T *argvars, typval_T *rettv, صحيح charpos)
{
    pos_T	pos;
    صحيح		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_شمېره = -1;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	أرجع;

    name = tv_get_string_chk(argvars);
    إذا (name == عدم)
	أرجع;

    إذا (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	أرجع;

    إذا (pos.ستون != MAXCOL && --pos.ستون < 0)
	pos.ستون = 0;
    إذا ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	إذا (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_سېټ_curswant = false;
	}
	check_cursor();
	rettv->vval.v_شمېره = 0;
    }
    وإلا إذا (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	إذا (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_شمېره = 0;
    }
    وإلا
	emsg(_(e_invalid_دلیل));
}
/*
 * "setcharpos()" function
 */
    ساكن فراغ
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    سېټ_position(argvars, rettv, TRUE);
}

    ساكن فراغ
f_setcharsearch(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    إذا (check_for_dict_arg(argvars, 0) == FAIL)
	أرجع;

    إذا ((d = argvars[0].vval.v_dict) == عدم)
	أرجع;

    csearch = dict_get_string(d, "char", FALSE);
    إذا (csearch != عدم)
    {
	إذا (enc_utf8)
	{
	    صحيح pcc[MAX_MCO];
	    صحيح c = utfc_ptr2char(csearch, pcc);

	    سېټ_وروستی_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	وإلا
	    سېټ_وروستی_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_پیدا_کړه(d, (char_u *)"forward", -1);
    إذا (di != عدم)
	سېټ_csearch_direction((صحيح)tv_get_شمېره(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_پیدا_کړه(d, (char_u *)"until", -1);
    إذا (di != عدم)
	سېټ_csearch_until(!!tv_get_شمېره(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    ساكن فراغ
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    سېټ_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    ساكن فراغ
f_setenv(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    إذا (check_restricted() || check_secure())
	أرجع;

    name = tv_get_string_buf(&argvars[0], namebuf);
    إذا (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_شمېره == VVAL_NULL)
	vim_unsetenv_ext(name);
    وإلا
	vim_setenv_ext(name, tv_get_string_buf(&argvars[1], valbuf));
}

/*
 * "setfperm({fname}, {mode})" function
 */
    ساكن فراغ
f_setfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	modebuf[NUMBUFLEN];
    char_u	*ډېرپېښ_str;
    صحيح		i;
    صحيح		ماسک;
    صحيح		ډېرپېښ = 0;

    rettv->vval.v_شمېره = 0;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    fname = tv_get_string_chk(&argvars[0]);
    إذا (fname == عدم)
	أرجع;
    ډېرپېښ_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    إذا (ډېرپېښ_str == عدم)
	أرجع;
    إذا (STRLEN(ډېرپېښ_str) != 9)
    {
	semsg(_(e_invalid_دلیل_str), ډېرپېښ_str);
	أرجع;
    }

    ماسک = 1;
    لكل (i = 8; i >= 0; --i)
    {
	إذا (ډېرپېښ_str[i] != '-')
	    ډېرپېښ |= ماسک;
	ماسک = ماسک << 1;
    }
    rettv->vval.v_شمېره = mch_setperm(fname, ډېرپېښ) == OK;
}

/*
 * "setpos()" function
 */
    ساكن فراغ
f_setpos(typval_T *argvars, typval_T *rettv)
{
    سېټ_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    صحيح
get_yank_type(char_u **pp, char_u *yank_type, اوږد *بلاک_len)
{
    char_u *stropt = *pp;
    بدل (*stropt)
    {
	حالة 'v': حالة 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    اكسر;
	حالة 'V': حالة 'l':	// line-wise selection
	    *yank_type = MLINE;
	    اكسر;
	حالة 'b': حالة Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    إذا (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*بلاک_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    اكسر;
	افتراضي:
	    أرجع FAIL;
    }
    *pp = stropt;
    أرجع OK;
}

/*
 * "setreg()" function
 */
    ساكن فراغ
f_setreg(typval_T *argvars, typval_T *rettv)
{
    صحيح		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    صحيح		append;
    char_u	yank_type;
    اوږد	بلاک_len;
    typval_T	*regcontents;
    صحيح		pointreg;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	أرجع;

    pointreg = 0;
    regcontents = عدم;
    بلاک_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_شمېره = 1;		// FAIL is default

    إذا (strregname == عدم)
	أرجع;		// type error; errmsg already given
    إذا (دننه_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_ثبتوونکی_name_must_be_one_char_str), strregname);
	أرجع;
    }
    regname = *strregname;
    إذا (regname == 0 || regname == '@')
	regname = '"';

    إذا (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	إذا (d == عدم || d->dv_hashtab.ht_کارول_شوی == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {عدم, عدم};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    أرجع;
	}

	di = dict_پیدا_کړه(d, (char_u *)"regcontents", -1);
	إذا (di != عدم)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	إذا (stropt != عدم)
	{
	    صحيح ret = get_yank_type(&stropt, &yank_type, &بلاک_len);

	    إذا (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_ارزښت_for_دلیل_str), "value");
		أرجع;
	    }
	}

	إذا (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    إذا (stropt != عدم)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	وإلا إذا (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    وإلا
	regcontents = &argvars[1];

    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	إذا (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    أرجع;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	إذا (stropt == عدم)
	    أرجع;		// type error
	لكل (; *stropt != NUL; ++stropt)
	    بدل (*stropt)
	    {
		حالة 'a': حالة 'A':	// append
		    append = TRUE;
		    اكسر;
		افتراضي:
		    get_yank_type(&stropt, &yank_type, &بلاک_len);
	    }
    }

    إذا (regcontents && regcontents->v_type == VAR_LIST)
    {
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	list_T		*ll = regcontents->vval.v_list;
	listitem_T	*li;
	صحيح		len;

	// If the list is NULL handle like an empty list.
	len = ll == عدم ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	إذا (lstval == عدم)
	    أرجع;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	إذا (ll != عدم)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_توکي(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		إذا (strval == عدم)
		    اذهب free_lstval;
		إذا (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    إذا (strval == عدم)
			اذهب free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = عدم;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, بلاک_len);
free_lstval:
	طالما (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    وإلا إذا (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	إذا (strval == عدم)
	    أرجع;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, بلاک_len);
    }
    إذا (pointreg != 0)
	get_yank_ثبتوونکی(pointreg, TRUE);

    rettv->vval.v_شمېره = 0;
}

/*
 * "settagstack()" function
 */
    ساكن فراغ
f_settagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    dict_T	*d;
    صحيح		action = 'r';

    rettv->vval.v_شمېره = -1;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	أرجع;

    // first argument: window number or id
    wp = پیدا_کړه_win_by_nr_or_id(&argvars[0]);
    إذا (wp == عدم)
	أرجع;

    // second argument: dict with items to set in the tag stack
    إذا (check_for_dict_arg(argvars, 1) == FAIL)
	أرجع;
    d = argvars[1].vval.v_dict;
    إذا (d == عدم)
	أرجع;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    إذا (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    وإلا إذا (check_for_string_arg(argvars, 2) == FAIL)
	أرجع;
    وإلا
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	إذا (actstr == عدم)
	    أرجع;
	إذا ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	وإلا
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    أرجع;
	}
    }

    إذا (سېټ_tagstack(wp, d, action) == OK)
	rettv->vval.v_شمېره = 0;
}

#که_تعریف_شوی FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    ساكن فراغ
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحيح		len;

    إذا (دننه_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != عدم ? (char_u *)blob->bv_ga.ga_معلومات : (char_u *)"";
	len = blob != عدم ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_بایټونه(p, len, عدم, 0));
    }
    وإلا
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_بایټونه(p, (صحيح)STRLEN(p), عدم, 0));
    }
}
#د_که_پای // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    ساكن فراغ
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    صحيح do_special;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    ساكن فراغ
f_shiftwidth(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    rettv->vval.v_شمېره = 0;

    إذا (دننه_vim9script() && check_for_opt_شمېره_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	اوږد	ستون;

	ستون = (اوږد)tv_get_شمېره_chk(argvars, عدم);
	إذا (ستون < 0)
	    أرجع;	// type error; errmsg already given
#که_تعریف_شوی FEAT_VARTABS
	rettv->vval.v_شمېره = get_sw_ارزښت_ستون(curbuf, ستون, FALSE);
	أرجع;
#د_که_پای
    }

    rettv->vval.v_شمېره = get_sw_ارزښت(curbuf);
}

/*
 * "soundfold({word})" function
 */
    ساكن فراغ
f_soundfold(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#که_تعریف_شوی FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#وإلا
    rettv->vval.v_string = vim_strsave(s);
#د_که_پای
}

/*
 * "spellbadword()" function
 */
    ساكن فراغ
f_spellbadword(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    char_u	*کلمه = (char_u *)"";
    hlf_T	attr = HLF_شمېر;
    صحيح		len = 0;
#که_تعریف_شوی FEAT_SPELL
    صحيح		wo_spell_save = curwin->w_p_spell;

    إذا (دننه_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    إذا (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	أرجع;
    }
#د_که_پای

    إذا (rettv_list_alloc(rettv) == FAIL)
    {
#که_تعریف_شوی FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#د_که_پای
	أرجع;
    }

#که_تعریف_شوی FEAT_SPELL
    إذا (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_منزل(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	إذا (len != 0)
	{
	    کلمه = ml_get_cursor();
	    curwin->w_سېټ_curswant = true;
	}
    }
    وإلا إذا (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	صحيح	capcol = -1;

	إذا (str != عدم)
	{
	    // Check the argument for spelling.
	    طالما (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		إذا (attr != HLF_شمېر)
		{
		    کلمه = str;
		    اكسر;
		}
		str += len;
		capcol -= len;
		len = 0;
	    }
	}
    }
    curwin->w_p_spell = wo_spell_save;
#د_که_پای

    list_append_string(rettv->vval.v_list, کلمه, len);
    بدل (attr)
    {
    حالة HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	اكسر;

    حالة HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	اكسر;

    حالة HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	اكسر;

    حالة HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	اكسر;

    افتراضي:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	اكسر;
    }
}

/*
 * "spellsuggest()" function
 */
    ساكن فراغ
f_spellsuggest(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
#که_تعریف_شوی FEAT_SPELL
    char_u	*str;
    صحيح		typeerr = FALSE;
    صحيح		maxcount;
    garray_T	ga;
    صحيح		i;
    listitem_T	*li;
    صحيح		need_capital = FALSE;
    صحيح		wo_spell_save = curwin->w_p_spell;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_شمېره_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	أرجع;

    إذا (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    إذا (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	أرجع;
    }
#د_که_پای

    إذا (rettv_list_alloc(rettv) == FAIL)
    {
#که_تعریف_شوی FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#د_که_پای
	أرجع;
    }

#که_تعریف_شوی FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (صحيح)tv_get_شمېره_chk(&argvars[1], &typeerr);
	إذا (maxcount <= 0)
	    أرجع;
	إذا (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (صحيح)tv_get_bool_chk(&argvars[2], &typeerr);
	    إذا (typeerr)
		أرجع;
	}
    }
    وإلا
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    لكل (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_معلومات)[i];

	li = listitem_alloc();
	إذا (li == عدم)
	    vim_free(str);
	وإلا
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_قلف = 0;
	    li->li_tv.vval.v_string = str;
	    list_append(rettv->vval.v_list, li);
	}
    }
    ga_clear(&ga);
    curwin->w_p_spell = wo_spell_save;
#د_که_پای
}

    ساكن فراغ
f_وېشل(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*پای_ټکی;
    char_u	*pat = عدم;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    صحيح		سمون;
    colnr_T	ستون = 0;
    صحيح		keepempty = FALSE;
    صحيح		typeerr = FALSE;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	أرجع;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = تشه_option;

    str = tv_get_string(&argvars[0]);
    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	إذا (pat == عدم)
	    typeerr = TRUE;
	إذا (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (صحيح)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    إذا (pat == عدم || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    إذا (rettv_list_alloc(rettv) == FAIL)
	اذهب theend;
    إذا (typeerr)
	اذهب theend;

    regmatch.regprog = eval_regcomp(pat);
    إذا (regmatch.regprog != عدم)
    {
	regmatch.rm_ic = FALSE;
	طالما (*str != NUL || keepempty)
	{
	    إذا (*str == NUL)
		سمون = FALSE;	// empty item at the end
	    وإلا
		سمون = vim_regexec_nl(&regmatch, str, ستون);
	    إذا (سمون)
		پای_ټکی = regmatch.startp[0];
	    وإلا
		پای_ټکی = str + STRLEN(str);
	    إذا (keepempty || پای_ټکی > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && سمون && پای_ټکی < regmatch.endp[0]))
	    {
		إذا (list_append_string(rettv->vval.v_list, str,
						    (صحيح)(پای_ټکی - str)) == FAIL)
		    اكسر;
	    }
	    إذا (!سمون)
		اكسر;
	    // Advance to just after the match.
	    إذا (regmatch.endp[0] > str)
		ستون = 0;
	    وإلا
		// Don't get stuck at the same match.
		ستون = (*mb_ptr2len)(regmatch.endp[0]);
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
    ساكن فراغ
f_submatch(typval_T *argvars, typval_T *rettv)
{
    صحيح		error = FALSE;
    صحيح		no;
    صحيح		retList = 0;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    no = (صحيح)tv_get_شمېره_chk(&argvars[0], &error);
    إذا (error)
	أرجع;
    إذا (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_شمېره_nr), no);
	أرجع;
    }
    إذا (argvars[1].v_type != VAR_UNKNOWN)
	retList = (صحيح)tv_get_bool_chk(&argvars[1], &error);
    إذا (error)
	أرجع;

    إذا (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    وإلا
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = reg_submatch_list(no);
    }
}

/*
 * "substitute()" function
 */
    ساكن فراغ
f_substitute(typval_T *argvars, typval_T *rettv)
{
    char_u	patbuf[NUMBUFLEN];
    char_u	subbuf[NUMBUFLEN];
    char_u	flagsbuf[NUMBUFLEN];
    char_u	*str;
    char_u	*pat;
    char_u	*sub = عدم;
    typval_T	*بیان = عدم;
    char_u	*flg;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	أرجع;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    إذا (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	بیان = &argvars[2];
    وإلا
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    إذا (str == عدم || pat == عدم || (sub == عدم && بیان == عدم)
								|| flg == عدم)
	rettv->vval.v_string = عدم;
    وإلا
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, بیان, flg, عدم);
}

/*
 * "swapfilelist()" function
 */
    ساكن فراغ
f_swapfilelist(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;
    recover_نومونه(عدم, FALSE, rettv->vval.v_list, 0, عدم);
}

/*
 * "swapinfo(swap_filename)" function
 */
    ساكن فراغ
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (rettv_dict_alloc(rettv) == OK)
	get_b0_dict(tv_get_string(argvars), rettv->vval.v_dict);
}

/*
 * "swapname(expr)" function
 */
    ساكن فراغ
f_swapname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    إذا (دننه_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم || buf->b_ml.ml_mfp == عدم
					|| buf->b_ml.ml_mfp->mf_fname == عدم)
	rettv->vval.v_string = عدم;
    وإلا
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    ساكن فراغ
f_synID(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    صحيح		id = 0;
#که_تعریف_شوی FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	ستون;
    صحيح		trans;
    صحيح		transerr = FALSE;

    إذا (دننه_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	أرجع;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    ستون = (linenr_T)tv_get_شمېره(&argvars[1]) - 1;	// -1 on type error
    trans = (صحيح)tv_get_bool_chk(&argvars[2], &transerr);

    إذا (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_کرښه_شمېر
	    && ستون >= 0 && ستون < (اوږد)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, ستون, trans, عدم, FALSE);
#د_که_پای

    rettv->vval.v_شمېره = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    ساكن فراغ
f_synIDattr(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    char_u	*p = عدم;
#که_تعریف_شوی FEAT_SYN_HL
    صحيح		id;
    char_u	*what;
    char_u	*ډېرپېښ;
    char_u	modebuf[NUMBUFLEN];
    صحيح		modec;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	أرجع;

    id = (صحيح)tv_get_شمېره(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	ډېرپېښ = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(ډېرپېښ[0]);
	إذا (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    وإلا
    {
# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	إذا (USE_24_بټ)
	    modec = 'g';
	وإلا
# endif
	    إذا (t_colors > 1)
		modec = 'c';
	    وإلا
		modec = 't';
    }

    بدل (TOLOWER_ASC(what[0]))
    {
	حالة 'b':
		إذا (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		وإلا					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		اكسر;

	حالة 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		اكسر;

	حالة 'i':
		إذا (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		وإلا					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		اكسر;

	حالة 'n':
		إذا (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		وإلا					// name
		    p = get_highlight_name_ext(عدم, id - 1, FALSE);
		اكسر;

	حالة 'r':					//سرچپه
		p = highlight_has_attr(id, HL_INVERSE, modec);
		اكسر;

	حالة 's':
		إذا (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		وإلا إذا (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		وإلا					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		اكسر;

	حالة 'u':
		إذا (STRLEN(what) >= 9)
		{
		    إذا (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    وإلا إذا (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    وإلا إذا (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    وإلا إذا (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    وإلا
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		وإلا
							// ul
		    p = highlight_color(id, what, modec);
		اكسر;
    }

    إذا (p != عدم)
	p = vim_strsave(p);
#د_که_پای
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    ساكن فراغ
f_synIDtrans(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    صحيح		id;

#که_تعریف_شوی FEAT_SYN_HL
    إذا (دننه_vim9script() && check_for_شمېره_arg(argvars, 0) == FAIL)
	أرجع;

    id = (صحيح)tv_get_شمېره(&argvars[0]);

    إذا (id > 0)
	id = syn_get_final_id(id);
    وإلا
#د_که_پای
	id = 0;

    rettv->vval.v_شمېره = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    ساكن فراغ
f_synconcealed(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
#إذا defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	ستون;
    صحيح		syntax_flags = 0;
    صحيح		cchar;
    صحيح		matchid = 0;
    char_u	str[NUMBUFLEN];
#د_که_پای

    rettv_list_سېټ(rettv, عدم);

    إذا (دننه_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

#إذا defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    ستون = (colnr_T)tv_get_شمېره(&argvars[1]) - 1;	// -1 on type error

    CLEAR_ډګر(str);

    إذا (rettv_list_alloc(rettv) == OK)
    {
	إذا (lnum >= 1 && lnum <= curbuf->b_ml.ml_کرښه_شمېر
	    && ستون >= 0 && ستون <= (اوږد)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (فراغ)syn_get_id(curwin, lnum, ستون, FALSE, عدم, FALSE);
	    syntax_flags = get_syntax_info(&matchid);

	    // get the conceal character
	    إذا ((syntax_flags & HL_CONCEAL) && curwin->w_p_cole < 3)
	    {
		cchar = syn_get_sub_char();
		إذا (cchar == NUL && curwin->w_p_cole == 1)
		    cchar = (curwin->w_lcs_chars.conceal == NUL) ? ' '
					: curwin->w_lcs_chars.conceal;
		إذا (cchar != NUL)
		{
		    إذا (has_mbyte)
			(*mb_char2bytes)(cchar, str);
		    وإلا
			str[0] = cchar;
		}
	    }
	}

	list_append_شمېره(rettv->vval.v_list,
					    (syntax_flags & HL_CONCEAL) != 0);
	// -1 to auto-determine strlen
	list_append_string(rettv->vval.v_list, str, -1);
	list_append_شمېره(rettv->vval.v_list, matchid);
    }
#د_که_پای
}

/*
 * "synstack(lnum, col)" function
 */
    ساكن فراغ
f_synstack(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
#که_تعریف_شوی FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	ستون;
    صحيح		i;
    صحيح		id;
#د_که_پای

    rettv_list_سېټ(rettv, عدم);

    إذا (دننه_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

#که_تعریف_شوی FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    ستون = (colnr_T)tv_get_شمېره(&argvars[1]) - 1;	// -1 on type error

    إذا (lnum >= 1 && lnum <= curbuf->b_ml.ml_کرښه_شمېر
	    && ستون >= 0 && ستون <= (اوږد)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(فراغ)syn_get_id(curwin, lnum, ستون, FALSE, عدم, TRUE);
	لكل (i = 0; ; ++i)
	{
	    id = syn_get_ډېری_توکی(i);
	    إذا (id < 0)
		اكسر;
	    إذا (list_append_شمېره(rettv->vval.v_list, id) == FAIL)
		اكسر;
	}
    }
#د_که_پای
}

/*
 * "tabpagebuflist()" function
 */
    ساكن فراغ
f_tabpagebuflist(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
    tabpage_T	*tp;
    win_T	*wp = عدم;

    إذا (دننه_vim9script() && check_for_opt_شمېره_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    وإلا
    {
	tp = پیدا_کړه_tabpage((صحيح)tv_get_شمېره(&argvars[0]));
	إذا (tp != عدم)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    إذا (wp != عدم && rettv_list_alloc(rettv) == OK)
    {
	لكل (; wp != عدم; wp = wp->w_بل)
	    إذا (list_append_شمېره(rettv->vval.v_list,
						wp->w_بفر->b_fnum) == FAIL)
		اكسر;
    }
}

/*
 * "tagfiles()" function
 */
    ساكن فراغ
f_tagfiles(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    صحيح		لومړی;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;
    fname = alloc(MAXPATHL);
    إذا (fname == عدم)
	أرجع;

    لكل (لومړی = TRUE; ; لومړی = FALSE)
	إذا (get_tagfname(&tn, لومړی, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    اكسر;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    ساكن فراغ
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = عدم;
    char_u  *نښان_نمونه;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    نښان_نمونه = tv_get_string(&argvars[0]);

    rettv->vval.v_شمېره = FALSE;
    إذا (*نښان_نمونه == NUL)
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    إذا (rettv_list_alloc(rettv) == OK)
	(فراغ)get_نښانونه(rettv->vval.v_list, نښان_نمونه, fname);
}

/*
 * "type(expr)" function
 */
    ساكن فراغ
f_type(typval_T *argvars, typval_T *rettv)
{
    صحيح n = -1;

    بدل (argvars[0].v_type)
    {
	حالة VAR_شمېره:  n = VAR_TYPE_شمېره; اكسر;
	حالة VAR_STRING:  n = VAR_TYPE_STRING; اكسر;
	حالة VAR_PARTIAL:
	حالة VAR_FUNC:    n = VAR_TYPE_FUNC; اكسر;
	حالة VAR_LIST:    n = VAR_TYPE_LIST; اكسر;
	حالة VAR_TUPLE:   n = VAR_TYPE_TUPLE; اكسر;
	حالة VAR_DICT:    n = VAR_TYPE_DICT; اكسر;
	حالة VAR_FLOAT:   n = VAR_TYPE_FLOAT; اكسر;
	حالة VAR_BOOL:	  n = VAR_TYPE_BOOL; اكسر;
	حالة VAR_SPECIAL: n = VAR_TYPE_NONE; اكسر;
	حالة VAR_دنده:     n = VAR_TYPE_دنده; اكسر;
	حالة VAR_CHANNEL: n = VAR_TYPE_CHANNEL; اكسر;
	حالة VAR_BLOB:    n = VAR_TYPE_BLOB; اكسر;
	حالة VAR_INSTR:   n = VAR_TYPE_INSTR; اكسر;
	حالة VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; اكسر;
	حالة VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		إذا (cl != عدم && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		وإلا
		    n = VAR_TYPE_CLASS;
		اكسر;
	    }
	حالة VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		إذا (obj == عدم)
		    n = VAR_TYPE_OBJECT;
		وإلا
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    إذا (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    وإلا
			n = VAR_TYPE_OBJECT;
		}
		اكسر;
	    }
	حالة VAR_VOID:
	    emsg(_(e_cannot_use_void_ارزښت));
	    اكسر;
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	    دننی_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    اكسر;
    }
    rettv->vval.v_شمېره = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    ساكن فراغ
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_پیل = 0;
    colnr_T	vcol_پای_ټکی = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    صحيح		winchanged = FALSE;
    صحيح		len;

    إذا (دننه_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_شمېره_arg(argvars, 2) == FAIL))))
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((صحيح)tv_get_شمېره(&argvars[2]), &tp);
	إذا (wp == عدم || tp == عدم)
	    اذهب theend;

	إذا (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    اذهب theend;

	check_cursor();
	winchanged = TRUE;
    }

    صحيح fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    إذا (fp != عدم && fp->lnum <= curbuf->b_ml.ml_کرښه_شمېر
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	إذا (fp->ستون < 0)
	    fp->ستون = 0;
	وإلا
	{
	    len = (صحيح)ml_get_len(fp->lnum);
	    إذا (fp->ستون > len)
		fp->ستون = len;
	}
	getvvcol(curwin, fp, &vcol_پیل, عدم, &vcol_پای_ټکی, 0);
	++vcol_پیل;
	++vcol_پای_ټکی;
    }

theend:
    إذا (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	إذا (rettv_list_alloc(rettv) == OK)
	{
	    list_append_شمېره(rettv->vval.v_list, vcol_پیل);
	    list_append_شمېره(rettv->vval.v_list, vcol_پای_ټکی);
	}
	وإلا
	    rettv->vval.v_شمېره = 0;
    }
    وإلا
	rettv->vval.v_شمېره = vcol_پای_ټکی;

    إذا (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    ساكن فراغ
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    إذا (دننه_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_ډېرپېښ_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    إذا (non_zero_arg(&argvars[0]))
	curbuf->b_visual_ډېرپېښ_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    ساكن فراغ
f_wildmenumode(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
    إذا (wild_menu_showing || ((State & ډېرپېښ_CMDLINE) && cmdline_pum_فعال()))
	rettv->vval.v_شمېره = 1;
}

/*
 * "windowsversion()" function
 */
    ساكن فراغ
f_windowsversion(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windows_نسخه);
}

/*
 * "wordcount()" function
 */
    ساكن فراغ
f_wordcount(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    ساكن فراغ
f_xor(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->vval.v_شمېره = tv_get_شمېره_chk(&argvars[0], عدم)
					^ tv_get_شمېره_chk(&argvars[1], عدم);
}

#د_که_پای // FEAT_EVAL
