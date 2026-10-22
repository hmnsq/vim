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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

স্থির শূন্য f_and(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_BEVAL
স্থির শূন্য f_balloon_gettext(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_balloon_show(typval_T *argvars, typval_T *rettv);
# যদি defined(FEAT_BEVAL_TERM)
স্থির শূন্য f_balloon_split(typval_T *argvars, typval_T *rettv);
# endif
#endif
স্থির শূন্য f_base64_encode(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_base64_decode(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_bindtextdomain(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_byte2line(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_call(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_changenr(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_char2nr(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_charcol(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_col(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_confirm(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_copy(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_cursor(typval_T *argsvars, typval_T *rettv);
#ifdef MSWIN
স্থির শূন্য f_debugbreak(typval_T *argvars, typval_T *rettv);
#endif
স্থির শূন্য f_deepcopy(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_did_filetype(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_echoraw(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_empty(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_environ(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_err_teapot(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_escape(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_eval(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_eventhandler(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_execute(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_exists_compiled(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_expand(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_expandcmd(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_feedkeys(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_fnameescape(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_foreground(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_funcref(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_function(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_garbagecollect(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_get(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getbgcolor(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getcellpixels(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getchangelist(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getcharpos(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getcharsearch(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getcurpos(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getcursorcharpos(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getenv(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getfontname(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getinfo(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getjumplist(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getpid(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getpos(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getreg(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getreginfo(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getregion(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getregionpos(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_getregtype(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_gettagstack(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_gettext(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_haslocaldir(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_hlID(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_hlexists(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_hostname(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_id(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_সূচক(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_indexof(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_প্রবেশ(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_inputdialog(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_inputlist(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_inputrestore(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_inputsave(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_inputsecret(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_interrupt(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_invert(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_islocked(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_বস্তুসমূহ(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_keytrans(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_last_বাফার_nr(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_libcall(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_libcallnr(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_সারি(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_line2byte(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_LUA
স্থির শূন্য f_luaeval(typval_T *argvars, typval_T *rettv);
#endif
স্থির শূন্য f_match(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_matchbufline(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_matchend(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_matchlist(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_matchstr(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_matchstrlist(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_matchstrpos(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_max(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_min(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_MZSCHEME
স্থির শূন্য f_mzeval(typval_T *argvars, typval_T *rettv);
#endif
স্থির শূন্য f_nextnonblank(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_ngettext(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_nr2char(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_or(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PERL
স্থির শূন্য f_perleval(typval_T *argvars, typval_T *rettv);
#endif
স্থির শূন্য f_prevnonblank(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_printf(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_pum_getpos(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_pumvisible(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PYTHON3
স্থির শূন্য f_py3eval(typval_T *argvars, typval_T *rettv);
#endif
#ifdef FEAT_PYTHON
স্থির শূন্য f_pyeval(typval_T *argvars, typval_T *rettv);
#endif
#যদি defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
স্থির শূন্য f_pyxeval(typval_T *argvars, typval_T *rettv);
#endif
স্থির শূন্য f_test_srand_seed(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_rand(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_range(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_reg_executing(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_reg_recording(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_rename(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_repeat(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_RUBY
স্থির শূন্য f_rubyeval(typval_T *argvars, typval_T *rettv);
#endif
স্থির শূন্য f_screenattr(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_screenchar(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_screenchars(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_screencol(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_screenrow(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_screenstring(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_অনুসন্ধান(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_searchdecl(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_searchpair(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_searchpairpos(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_searchpos(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_setcharpos(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_setcharsearch(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_setcursorcharpos(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_setenv(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_setfperm(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_setpos(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_setreg(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_settagstack(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_CRYPT
স্থির শূন্য f_sha256(typval_T *argvars, typval_T *rettv);
#endif
স্থির শূন্য f_shellescape(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_shiftwidth(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_soundfold(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_spellbadword(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_spellsuggest(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_split(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_srand(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_submatch(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_substitute(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_swapfilelist(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_swapinfo(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_swapname(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_synID(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_synIDattr(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_synIDtrans(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_synstack(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_synconcealed(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_tabpagebuflist(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_taglist(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_tagfiles(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_type(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_virtcol(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_visualmode(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_wildmenumode(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_windowsversion(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_wordcount(typval_T *argvars, typval_T *rettv);
স্থির শূন্য f_xor(typval_T *argvars, typval_T *rettv);


/*
 * Functions that check the argument type of a builtin function.
 * Each function returns FAIL and gives an error message if the type is wrong.
 */

// Context passed to an arg_ function.
প্রকারলেখ কাঠামো {
    পূর্ণসংখ্যা		arg_গুনতি;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    পূর্ণসংখ্যা		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
প্রকারলেখ পূর্ণসংখ্যা (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    স্থির পূর্ণসংখ্যা
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    ফেরত need_type(actual, expected, 0,
	    context->arg_idx - context->arg_গুনতি, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    স্থির পূর্ণসংখ্যা
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    যদি (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_গুনতি, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	ফেরত FAIL;
    ফেরত arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    পূর্ণসংখ্যা
arg_type_modifiable(type_T *type, পূর্ণসংখ্যা arg_idx)
{
    অক্ষর *tofree;

    যদি ((type->tt_flags & TTFLAG_CONST) == 0)
	ফেরত OK;
    semsg(_(e_argument_nr_trying_to_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    ফেরত FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    স্থির পূর্ণসংখ্যা
arg_any(type_T *type UNUSED,
	type_T *decl_type UNUSED,
	argcontext_T *context UNUSED)
{
    ফেরত OK;
}

/*
 * Check "type" is a float or a number.
 */
    স্থির পূর্ণসংখ্যা
arg_float_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_সংখ্যা
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_সংখ্যা, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a number.
 */
    স্থির পূর্ণসংখ্যা
arg_সংখ্যা(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ফেরত check_arg_type(&t_সংখ্যা, type, context);
}

/*
 * Check "type" is an object.
 */
    স্থির পূর্ণসংখ্যা
arg_object(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    স্থির পূর্ণসংখ্যা
arg_dict_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ফেরত check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    স্থির পূর্ণসংখ্যা
arg_list_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ফেরত check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    স্থির পূর্ণসংখ্যা
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    ফেরত check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    স্থির পূর্ণসংখ্যা
arg_list_সংখ্যা(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ফেরত check_arg_type(&t_list_সংখ্যা, type, context);
}

/*
 * Check "type" is a list of strings.
 */
    স্থির পূর্ণসংখ্যা
arg_list_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ফেরত check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    স্থির পূর্ণসংখ্যা
arg_tuple_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ফেরত check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    স্থির পূর্ণসংখ্যা
arg_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ফেরত check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    স্থির পূর্ণসংখ্যা
arg_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ফেরত check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    স্থির পূর্ণসংখ্যা
arg_bool(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ফেরত check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    স্থির পূর্ণসংখ্যা
arg_bool_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_সংখ্যা
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_সংখ্যা, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    স্থির পূর্ণসংখ্যা
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	ফেরত OK;
    ফেরত check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    স্থির পূর্ণসংখ্যা
arg_list_or_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    স্থির পূর্ণসংখ্যা
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    যদি (arg_list_or_blob(type, decl_type, context) == FAIL)
	ফেরত FAIL;
    ফেরত arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    স্থির পূর্ণসংখ্যা
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ফেরত FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    স্থির পূর্ণসংখ্যা
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a string or a number
 */
    স্থির পূর্ণসংখ্যা
arg_string_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_সংখ্যা
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    স্থির পূর্ণসংখ্যা
arg_বাফার(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_সংখ্যা
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    স্থির পূর্ণসংখ্যা
arg_বাফার_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_সংখ্যা
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    স্থির পূর্ণসংখ্যা
arg_lnum(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_সংখ্যা
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    স্থির পূর্ণসংখ্যা
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ফেরত OK;
    যদি (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	ফেরত FAIL;
    }
    যদি (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	ফেরত OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    স্থির পূর্ণসংখ্যা
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    স্থির পূর্ণসংখ্যা
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    স্থির পূর্ণসংখ্যা
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    স্থির পূর্ণসংখ্যা
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    স্থির পূর্ণসংখ্যা
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    যদি (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    স্থির পূর্ণসংখ্যা
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    যদি (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	ফেরত FAIL;
    ফেরত arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    স্থির পূর্ণসংখ্যা
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    স্থির পূর্ণসংখ্যা
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    যদি (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	ফেরত FAIL;
    ফেরত arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    স্থির পূর্ণসংখ্যা
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ফেরত OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr),
						 context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    স্থির পূর্ণসংখ্যা
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = ফাঁকা;
    type_T *(args[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, ফাঁকা, ফাঁকা, args};

    যদি (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	যদি (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	অন্যথা
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    অন্যথা যদি (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    অন্যথা যদি (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_সংখ্যা;

    args[0] = ফাঁকা;
    args[1] = &t_unknown;
    যদি (type->tt_argcount != -1)
    {
	যদি (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_সংখ্যা_of_arguments));
	    ফেরত FAIL;
	}
	যদি (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	অন্যথা
	{
	    যদি (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		args[0] = &t_সংখ্যা;
	    অন্যথা যদি (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		args[0] = &t_string;
	    যদি (args[0] != ফাঁকা)
		args[1] = expected_member;
	}
    }

    যদি (!type_any_or_unknown(type->tt_member) || args[0] != ফাঁকা)
    {
	where_T where = WHERE_INIT;

	যদি (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == ফাঁকা
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	অন্যথা যদি (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	অন্যথা // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	যদি (args[0] == ফাঁকা)
	    args[0] = &t_unknown;
	যদি (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_সূচক = 2;
	where.wt_kind = WT_ARGUMENT;
	ফেরত check_type(&t_func_exp, type, TRUE, where);
    }
    ফেরত OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    স্থির পূর্ণসংখ্যা
arg_filter_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ফেরত OK;

    যদি (type->tt_type == VAR_FUNC)
	ফেরত check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    ফেরত FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    স্থির পূর্ণসংখ্যা
arg_map_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ফেরত OK;

    যদি (type->tt_type == VAR_FUNC)
	ফেরত check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    ফেরত FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    স্থির পূর্ণসংখ্যা
arg_foreach_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ফেরত OK;

    যদি (type->tt_type == VAR_FUNC)
	ফেরত check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    ফেরত FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    স্থির পূর্ণসংখ্যা
arg_sort_how(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ফেরত OK;

    যদি (type->tt_type == VAR_FUNC)
    {
	type_T *(args[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_সংখ্যা, ফাঁকা, args};

	যদি (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    args[0] = context->arg_types[0].type_curr->tt_member;
	অন্যথা
	    args[0] = &t_unknown;
	যদি (!type_any_or_unknown(type->tt_member) || args[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    args[1] = args[0];
	    যদি (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_সূচক = 2;
	    where.wt_kind = WT_ARGUMENT;
	    ফেরত check_type(&t_func_exp, type, TRUE, where);
	}

	ফেরত OK;
    }
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    ফেরত FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    স্থির পূর্ণসংখ্যা
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_সংখ্যা
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    স্থির পূর্ণসংখ্যা
varargs_class(type_T *type UNUSED,
	      type_T *decl_type UNUSED,
	      argcontext_T *context)
{
    জন্য (পূর্ণসংখ্যা i = context->arg_idx; i < context->arg_গুনতি; ++i)
    {
	type2_T *types = &context->arg_types[i];
	যদি (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    ফেরত FAIL;
	}
    }
    ফেরত OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    স্থির পূর্ণসংখ্যা
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    স্থির পূর্ণসংখ্যা
arg_reverse(type_T *type, type_T *decl_type, argcontext_T *context)
{
    যদি (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	ফেরত FAIL;
    ফেরত arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    স্থির পূর্ণসংখ্যা
arg_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ফেরত check_arg_type(&t_job, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    স্থির পূর্ণসংখ্যা
arg_chan_or_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_JOB
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    স্থির পূর্ণসংখ্যা
arg_same_as_পূর্ব(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *পূর্ব_type = context->arg_types[context->arg_idx - 1].type_decl;

    ফেরত check_arg_type(পূর্ব_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    স্থির পূর্ণসংখ্যা
arg_same_struct_as_পূর্ব(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    type_T *পূর্ব_type = context->arg_types[context->arg_idx - 1].type_curr;

    যদি (পূর্ব_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	ফেরত check_arg_type(পূর্ব_type, type, context);
    ফেরত OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    স্থির পূর্ণসংখ্যা
arg_item_of_পূর্ব(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *পূর্ব_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    যদি (পূর্ব_type->tt_type == VAR_LIST)
	expected = পূর্ব_type->tt_member;
    অন্যথা যদি (পূর্ব_type->tt_type == VAR_BLOB)
	expected = &t_সংখ্যা;
    অন্যথা
	// probably VAR_ANY, can't check
	ফেরত OK;

    ফেরত check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    স্থির পূর্ণসংখ্যা
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_সংখ্যা
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    স্থির পূর্ণসংখ্যা
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ফেরত OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    স্থির পূর্ণসংখ্যা
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 2].type_curr;

    যদি (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	ফেরত arg_সংখ্যা(type, decl_type, context);
    যদি (first_type->tt_type == VAR_DICT)
	ফেরত arg_string(type, decl_type, context);
    ফেরত OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    স্থির পূর্ণসংখ্যা
arg_get1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ফেরত OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    স্থির পূর্ণসংখ্যা
arg_len1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_সংখ্যা
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	ফেরত OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    স্থির পূর্ণসংখ্যা
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 1].type_curr;

    যদি (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	ফেরত arg_সংখ্যা(type, decl_type, context);
    যদি (first_type->tt_type == VAR_DICT)
	ফেরত arg_string_or_nr(type, decl_type, context);
    ফেরত OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    স্থির পূর্ণসংখ্যা
arg_repeat1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_সংখ্যা
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	ফেরত OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    স্থির পূর্ণসংখ্যা
arg_slice1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ফেরত OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    স্থির পূর্ণসংখ্যা
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    যদি (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ফেরত OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr),
							 context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    স্থির পূর্ণসংখ্যা
arg_cursor1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    যদি (type->tt_type == VAR_সংখ্যা
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ফেরত OK;

    arg_type_mismatch(&t_সংখ্যা, type, context->arg_idx + 1);
    ফেরত FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
স্থির argcheck_T arg1_blob[] = {arg_blob};
স্থির argcheck_T arg1_bool[] = {arg_bool};
স্থির argcheck_T arg1_বাফার[] = {arg_বাফার};
স্থির argcheck_T arg1_বাফার_or_dict_any[] = {arg_বাফার_or_dict_any};
স্থির argcheck_T arg1_chan_or_job[] = {arg_chan_or_job};
স্থির argcheck_T arg1_dict_any[] = {arg_dict_any};
স্থির argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
স্থির argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
স্থির argcheck_T arg1_job[] = {arg_job};
স্থির argcheck_T arg1_list_any[] = {arg_list_any};
স্থির argcheck_T arg1_tuple_any[] = {arg_tuple_any};
স্থির argcheck_T arg1_list_সংখ্যা[] = {arg_list_সংখ্যা};
স্থির argcheck_T arg1_reverse[] = {arg_reverse};
স্থির argcheck_T arg1_list_or_tuple_or_dict[] = {arg_list_or_tuple_or_dict};
স্থির argcheck_T arg1_list_string[] = {arg_list_string};
স্থির argcheck_T arg1_list_tuple_dict_blob_or_string[] = {arg_list_tuple_dict_blob_or_string};
স্থির argcheck_T arg1_lnum[] = {arg_lnum};
স্থির argcheck_T arg1_সংখ্যা[] = {arg_সংখ্যা};
স্থির argcheck_T arg1_string[] = {arg_string};
স্থির argcheck_T arg1_string_or_list_any[] = {arg_string_or_list_any};
স্থির argcheck_T arg1_string_or_list_string[] = {arg_string_or_list_string};
স্থির argcheck_T arg1_string_or_nr[] = {arg_string_or_nr};
স্থির argcheck_T arg1_string_or_blob[] = {arg_string_or_blob};
স্থির argcheck_T arg2_বাফার_any[] = {arg_বাফার, arg_any};
স্থির argcheck_T arg2_বাফার_bool[] = {arg_বাফার, arg_bool};
স্থির argcheck_T arg2_বাফার_list_any[] = {arg_বাফার, arg_list_any};
স্থির argcheck_T arg2_বাফার_lnum[] = {arg_বাফার, arg_lnum};
স্থির argcheck_T arg2_বাফার_সংখ্যা[] = {arg_বাফার, arg_সংখ্যা};
স্থির argcheck_T arg2_বাফার_string[] = {arg_বাফার, arg_string};
স্থির argcheck_T arg2_chan_or_job_dict[] = {arg_chan_or_job, arg_dict_any};
স্থির argcheck_T arg2_chan_or_job_string[] = {arg_chan_or_job, arg_string};
স্থির argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
স্থির argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
স্থির argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
স্থির argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
স্থির argcheck_T arg2_job_dict[] = {arg_job, arg_dict_any};
স্থির argcheck_T arg2_job_string_or_সংখ্যা[] = {arg_job, arg_string_or_nr};
স্থির argcheck_T arg2_list_any_সংখ্যা[] = {arg_list_any, arg_সংখ্যা};
স্থির argcheck_T arg2_list_সংখ্যা[] = {arg_list_সংখ্যা, arg_list_সংখ্যা};
স্থির argcheck_T arg2_list_সংখ্যা_bool[] = {arg_list_সংখ্যা, arg_bool};
স্থির argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
স্থির argcheck_T arg2_listblobmod_item[] = {arg_list_or_blob_mod, arg_item_of_পূর্ব};
স্থির argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
স্থির argcheck_T arg2_lnum_সংখ্যা[] = {arg_lnum, arg_সংখ্যা};
স্থির argcheck_T arg2_সংখ্যা[] = {arg_সংখ্যা, arg_সংখ্যা};
স্থির argcheck_T arg2_সংখ্যা_any[] = {arg_সংখ্যা, arg_any};
স্থির argcheck_T arg2_সংখ্যা_bool[] = {arg_সংখ্যা, arg_bool};
স্থির argcheck_T arg2_সংখ্যা_dict_any[] = {arg_সংখ্যা, arg_dict_any};
স্থির argcheck_T arg2_সংখ্যা_list[] = {arg_সংখ্যা, arg_list_any};
স্থির argcheck_T arg2_সংখ্যা_string[] = {arg_সংখ্যা, arg_string};
স্থির argcheck_T arg2_সংখ্যা_বাফার[] = {arg_সংখ্যা, arg_বাফার};
স্থির argcheck_T arg2_সংখ্যা_string_or_list[] = {arg_সংখ্যা, arg_string_or_list_any};
স্থির argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
স্থির argcheck_T arg2_string[] = {arg_string, arg_string};
স্থির argcheck_T arg2_string_any[] = {arg_string, arg_any};
স্থির argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
স্থির argcheck_T arg2_string_chan_or_job[] = {arg_string, arg_chan_or_job};
স্থির argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
স্থির argcheck_T arg2_string_list_সংখ্যা[] = {arg_string, arg_list_সংখ্যা};
স্থির argcheck_T arg2_string_সংখ্যা[] = {arg_string, arg_সংখ্যা};
স্থির argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
স্থির argcheck_T arg2_string_or_list_সংখ্যা[] = {arg_string_or_list_any, arg_সংখ্যা};
স্থির argcheck_T arg2_string_string_or_সংখ্যা[] = {arg_string, arg_string_or_nr};
স্থির argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
স্থির argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
স্থির argcheck_T arg3_any_বাফার_bool_or_dict[] = {
			      arg_any, arg_বাফার, arg_bool_or_dict_any};
স্থির argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
স্থির argcheck_T arg3_বাফার_lnum_lnum[] = {arg_বাফার, arg_lnum, arg_lnum};
স্থির argcheck_T arg3_বাফার_সংখ্যা_সংখ্যা[] = {arg_বাফার, arg_সংখ্যা, arg_সংখ্যা};
স্থির argcheck_T arg3_বাফার_string_any[] = {arg_বাফার, arg_string, arg_any};
স্থির argcheck_T arg3_বাফার_string_dict[] = {arg_বাফার, arg_string, arg_dict_any};
স্থির argcheck_T arg3_dict_সংখ্যা_সংখ্যা[] = {arg_dict_any, arg_সংখ্যা, arg_সংখ্যা};
স্থির argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
স্থির argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
স্থির argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
স্থির argcheck_T arg3_lnum_সংখ্যা_bool[] = {arg_lnum, arg_সংখ্যা, arg_bool};
স্থির argcheck_T arg3_সংখ্যা[] = {arg_সংখ্যা, arg_সংখ্যা, arg_সংখ্যা};
স্থির argcheck_T arg3_সংখ্যা_any_dict[] = {arg_সংখ্যা, arg_any, arg_dict_any};
স্থির argcheck_T arg3_সংখ্যা_সংখ্যা_dict[] = {arg_সংখ্যা, arg_সংখ্যা, arg_dict_any};
স্থির argcheck_T arg3_সংখ্যা_string_any[] = {arg_সংখ্যা, arg_string, arg_any};
স্থির argcheck_T arg3_সংখ্যা_string_বাফার[] = {arg_সংখ্যা, arg_string, arg_বাফার};
স্থির argcheck_T arg3_সংখ্যা_string_string[] = {arg_সংখ্যা, arg_string, arg_string};
স্থির argcheck_T arg3_string[] = {arg_string, arg_string, arg_string};
স্থির argcheck_T arg3_string_any_dict[] = {arg_string, arg_any, arg_dict_any};
স্থির argcheck_T arg3_string_any_string[] = {arg_string, arg_any, arg_string};
স্থির argcheck_T arg3_string_bool_bool[] = {arg_string, arg_bool, arg_bool};
স্থির argcheck_T arg3_string_সংখ্যা_bool[] = {arg_string, arg_সংখ্যা, arg_bool};
স্থির argcheck_T arg3_string_সংখ্যা_সংখ্যা[] = {arg_string, arg_সংখ্যা, arg_সংখ্যা};
স্থির argcheck_T arg3_string_or_dict_bool_dict[] = {arg_string_or_dict_any, arg_bool, arg_dict_any};
স্থির argcheck_T arg3_string_or_list_bool_সংখ্যা[] = {arg_string_or_list_any, arg_bool, arg_সংখ্যা};
স্থির argcheck_T arg3_string_string_bool[] = {arg_string, arg_string, arg_bool};
স্থির argcheck_T arg3_string_string_dict[] = {arg_string, arg_string, arg_dict_any};
স্থির argcheck_T arg3_string_string_সংখ্যা[] = {arg_string, arg_string, arg_সংখ্যা};
স্থির argcheck_T arg4_সংখ্যা_সংখ্যা_string_any[] = {arg_সংখ্যা, arg_সংখ্যা, arg_string, arg_any};
স্থির argcheck_T arg4_string_string_any_string[] = {arg_string, arg_string, arg_any, arg_string};
স্থির argcheck_T arg4_string_string_সংখ্যা_string[] = {arg_string, arg_string, arg_সংখ্যা, arg_string};
স্থির argcheck_T arg4_string_সংখ্যা_bool_bool[] = {arg_string, arg_সংখ্যা, arg_bool, arg_bool};
/* Function specific argument types (not covered by the above) */
স্থির argcheck_T arg15_assert_fails[] = {arg_string_or_nr, arg_string_or_list_any, arg_any, arg_সংখ্যা, arg_string};
স্থির argcheck_T arg34_assert_inrange[] = {arg_float_or_nr, arg_float_or_nr, arg_float_or_nr, arg_string};
স্থির argcheck_T arg4_browse[] = {arg_bool, arg_string, arg_string, arg_string};
স্থির argcheck_T arg23_chanexpr[] = {arg_chan_or_job, arg_any, arg_dict_any};
স্থির argcheck_T arg23_chanraw[] = {arg_chan_or_job, arg_string_or_blob, arg_dict_any};
স্থির argcheck_T arg24_গুনতি[] = {arg_string_list_tuple_or_dict, arg_any, arg_bool, arg_সংখ্যা};
স্থির argcheck_T arg13_cursor[] = {arg_cursor1, arg_সংখ্যা, arg_সংখ্যা};
স্থির argcheck_T arg12_deepcopy[] = {arg_any, arg_bool};
স্থির argcheck_T arg12_execute[] = {arg_string_or_list_string, arg_string};
স্থির argcheck_T arg12_getchar[] = {arg_bool_or_nr, arg_dict_any};
স্থির argcheck_T arg23_extend[] = {arg_list_or_dict_or_blob_mod, arg_same_as_পূর্ব, arg_extend3};
স্থির argcheck_T arg23_extendnew[] = {arg_list_or_dict_or_blob, arg_same_struct_as_পূর্ব, arg_extend3};
স্থির argcheck_T arg23_get[] = {arg_get1, arg_string_or_nr, arg_any};
স্থির argcheck_T arg14_glob[] = {arg_string, arg_bool, arg_bool, arg_bool};
স্থির argcheck_T arg25_globpath[] = {arg_string, arg_string, arg_bool, arg_bool, arg_bool};
স্থির argcheck_T arg24_সূচক[] = {arg_list_or_tuple_or_blob, arg_item_of_পূর্ব, arg_সংখ্যা, arg_bool};
স্থির argcheck_T arg23_সূচক[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
স্থির argcheck_T arg23_ঢোকাও[] = {arg_list_or_blob, arg_item_of_পূর্ব, arg_সংখ্যা};
স্থির argcheck_T arg1_len[] = {arg_len1};
স্থির argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
স্থির argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
স্থির argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
স্থির argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
স্থির argcheck_T arg2_instanceof[] = {arg_object, varargs_class, ফাঁকা };
স্থির argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
স্থির argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
স্থির argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_সংখ্যা, arg_সংখ্যা, arg_dict_any};
স্থির argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_সংখ্যা, arg_সংখ্যা, arg_dict_any};
স্থির argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
স্থির argcheck_T arg45_matchbufline[] = {arg_বাফার, arg_string, arg_lnum, arg_lnum, arg_dict_any};
স্থির argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
স্থির argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
স্থির argcheck_T arg24_remote_রাশিমালা[] = {arg_string, arg_string, arg_string, arg_সংখ্যা};
স্থির argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_সংখ্যা};
স্থির argcheck_T arg2_repeat[] = {arg_repeat1, arg_সংখ্যা};
স্থির argcheck_T arg15_অনুসন্ধান[] = {arg_string, arg_string, arg_সংখ্যা, arg_সংখ্যা, arg_string_or_func};
স্থির argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_সংখ্যা, arg_সংখ্যা};
স্থির argcheck_T arg3_setbufline[] = {arg_বাফার, arg_lnum, arg_str_or_nr_or_list};
স্থির argcheck_T arg2_setline[] = {arg_lnum, arg_any};
স্থির argcheck_T arg24_setloclist[] = {arg_সংখ্যা, arg_list_any, arg_string, arg_dict_any};
স্থির argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
স্থির argcheck_T arg23_settagstack[] = {arg_সংখ্যা, arg_dict_any, arg_string};
স্থির argcheck_T arg02_sign_getplaced[] = {arg_বাফার, arg_dict_any};
স্থির argcheck_T arg45_sign_place[] = {arg_সংখ্যা, arg_string, arg_string, arg_বাফার, arg_dict_any};
স্থির argcheck_T arg23_slice[] = {arg_slice1, arg_সংখ্যা, arg_সংখ্যা};
স্থির argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_sort_how, arg_dict_any};
স্থির argcheck_T arg24_strpart[] = {arg_string, arg_সংখ্যা, arg_সংখ্যা, arg_bool};
স্থির argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
স্থির argcheck_T arg23_win_execute[] = {arg_সংখ্যা, arg_string_or_list_string, arg_string};
স্থির argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
স্থির argcheck_T arg24_match_func[] = {arg_string_or_list_any, arg_string, arg_সংখ্যা, arg_সংখ্যা};

// Can be used by functions called through "f_retfunc" to create new types.
স্থির garray_T *current_type_gap = ফাঁকা;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    স্থির type_T *
ret_void(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_void;
}
    স্থির type_T *
ret_any(পূর্ণসংখ্যা	argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_any;
}
    স্থির type_T *
ret_bool(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_bool;
}
    স্থির type_T *
ret_সংখ্যা_bool(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_সংখ্যা_bool;
}
    স্থির type_T *
ret_সংখ্যা(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_সংখ্যা;
}
    স্থির type_T *
ret_float(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_float;
}
    স্থির type_T *
ret_string(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_string;
}
    স্থির type_T *
ret_list_any(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_list_any;
}
    স্থির type_T *
ret_list_সংখ্যা(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ফেরত &t_list_সংখ্যা;
}
    স্থির type_T *
ret_list_string(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ফেরত &t_list_string;
}
    স্থির type_T *
ret_list_dict_any(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ফেরত &t_list_dict_any;
}
    স্থির type_T *
ret_list_বস্তুসমূহ(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ফেরত &t_list_list_any;
}
    স্থির type_T *
ret_list_list_any(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ফেরত &t_list_list_any;
}
    স্থির type_T *
ret_list_string_বস্তুসমূহ(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ফেরত &t_list_list_string;
}
    স্থির type_T *
ret_list_regionpos(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ফেরত &t_list_list_list_সংখ্যা;
}
    স্থির type_T *
ret_tuple_any(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_tuple_any;
}
    স্থির type_T *
ret_dict_any(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_dict_any;
}
    স্থির type_T *
ret_job_info(পূর্ণসংখ্যা argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    যদি (argcount == 0)
    {
	*decl_type = &t_list_any;
	ফেরত &t_list_job;
    }
    ফেরত &t_dict_any;
}
    স্থির type_T *
ret_dict_সংখ্যা(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_dict_সংখ্যা;
}
    স্থির type_T *
ret_dict_string(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_dict_string;
}
    স্থির type_T *
ret_blob(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_blob;
}
    স্থির type_T *
ret_func_any(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_func_any;
}
    স্থির type_T *
ret_func_unknown(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_func_unknown;
}
    স্থির type_T *
ret_channel(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_channel;
}
    স্থির type_T *
ret_job(পূর্ণসংখ্যা argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ফেরত &t_job;
}
    স্থির type_T *
ret_first_arg(পূর্ণসংখ্যা argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    যদি (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	ফেরত argtypes[0].type_curr;
    }
    ফেরত &t_void;
}
    স্থির type_T *
ret_slice(পূর্ণসংখ্যা argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    যদি (argcount > 0)
    {
	যদি (argtypes[0].type_decl != ফাঁকা)
	{
	    নির্বাচন (argtypes[0].type_decl->tt_type)
	    {
		ক্ষেত্র VAR_STRING: *decl_type = &t_string; বিরতি;
		ক্ষেত্র VAR_BLOB: *decl_type = &t_blob; বিরতি;
		ক্ষেত্র VAR_LIST: *decl_type = &t_list_any; বিরতি;
		ক্ষেত্র VAR_TUPLE: *decl_type = &t_tuple_any; বিরতি;
		পূর্বনির্ধারিত: বিরতি;
	    }
	}
	ফেরত argtypes[0].type_curr;
    }
    ফেরত &t_void;
}
    স্থির type_T *
ret_copy(পূর্ণসংখ্যা argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    যদি (argcount > 0)
    {
	যদি (argtypes[0].type_decl != ফাঁকা)
	{
	    যদি (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    অন্যথা যদি (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    অন্যথা
		*decl_type = argtypes[0].type_decl;
	}
	যদি (argtypes[0].type_curr != ফাঁকা)
	{
	    যদি (argtypes[0].type_curr->tt_type == VAR_LIST)
		ফেরত &t_list_any;
	    অন্যথা যদি (argtypes[0].type_curr->tt_type == VAR_DICT)
		ফেরত &t_dict_any;
	}
	ফেরত argtypes[0].type_curr;
    }
    ফেরত &t_void;
}
    স্থির type_T *
ret_extend(পূর্ণসংখ্যা argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    যদি (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	// if the second argument has a different current type then the current
	// type is "any"
	যদি (argcount > 1 && !equal_type(argtypes[0].type_curr,
						     argtypes[1].type_curr, 0))
	{
	    যদি (argtypes[0].type_curr->tt_type == VAR_LIST)
		ফেরত &t_list_any;
	    যদি (argtypes[0].type_curr->tt_type == VAR_DICT)
		ফেরত &t_dict_any;
	    যদি (argtypes[0].type_curr->tt_type == VAR_BLOB)
		ফেরত &t_blob;
	}
	ফেরত argtypes[0].type_curr;
    }
    ফেরত &t_void;
}
    স্থির type_T *
ret_repeat(পূর্ণসংখ্যা argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    যদি (argcount == 0)
	ফেরত &t_any;
    যদি (argtypes[0].type_curr == &t_সংখ্যা)
	ফেরত &t_string;
    ফেরত argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    স্থির type_T *
ret_first_cont(পূর্ণসংখ্যা argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    যদি (argcount > 0)
    {
	যদি (argtypes[0].type_curr->tt_type == VAR_LIST)
	    ফেরত &t_list_any;
	যদি (argtypes[0].type_curr->tt_type == VAR_DICT)
	    ফেরত &t_dict_any;
	যদি (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    ফেরত argtypes[0].type_curr;
    }
    ফেরত &t_any;
}
// for get(): the type of the item when a missing item has the same type
    স্থির type_T *
ret_get(পূর্ণসংখ্যা argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    যদি (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*item = ফাঁকা;

	যদি (t->tt_type == VAR_BLOB)
	    item = &t_সংখ্যা;
	অন্যথা যদি (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    item = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	যদি (item != ফাঁকা && (argcount == 3
			? equal_type(item, argtypes[2].type_curr, 0)
			: item->tt_type == VAR_সংখ্যা))
	{
	    যদি (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    ফেরত item;
	}
    }
    ফেরত &t_any;
}
// for getline()
    স্থির type_T *
ret_getline(পূর্ণসংখ্যা argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    যদি (argcount == 1)
	ফেরত &t_string;
    *decl_type = &t_list_any;
    ফেরত &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    স্থির type_T *
ret_string_or_any_1(পূর্ণসংখ্যা argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    যদি (argcount < 1)
	ফেরত &t_string;
    ফেরত &t_any;
}
    স্থির type_T *
ret_string_or_any_2(পূর্ণসংখ্যা argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    যদি (argcount < 2)
	ফেরত &t_string;
    ফেরত &t_any;
}
    স্থির type_T *
ret_string_or_any_3(পূর্ণসংখ্যা argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    যদি (argcount < 3)
	ফেরত &t_string;
    ফেরত &t_any;
}
    স্থির type_T *
ret_string_or_any_4(পূর্ণসংখ্যা argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    যদি (argcount < 4)
	ফেরত &t_string;
    ফেরত &t_any;
}
// for abs()
    স্থির type_T *
ret_abs(পূর্ণসংখ্যা argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    যদি (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	ফেরত &t_float;
    ফেরত &t_সংখ্যা;
}
// for sign_define()
    স্থির type_T *
ret_sign_define(পূর্ণসংখ্যা argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    যদি (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	ফেরত &t_list_সংখ্যা;
    ফেরত &t_সংখ্যা;
}
// for values(): list of member of first argument
    স্থির type_T *
ret_list_member(পূর্ণসংখ্যা argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    যদি (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	যদি (current_type_gap != ফাঁকা
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	অন্যথা
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	যদি (current_type_gap != ফাঁকা
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    ফেরত get_list_type(t->tt_member, current_type_gap);
    }
    ফেরত &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    স্থির type_T *
ret_list_or_dict_0(পূর্ণসংখ্যা argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    যদি (argcount > 0)
	ফেরত &t_dict_any;
    *decl_type = &t_list_any;
    ফেরত &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    স্থির type_T *
ret_list_or_dict_1(পূর্ণসংখ্যা argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    যদি (argcount > 1)
	ফেরত &t_dict_any;
    *decl_type = &t_list_any;
    ফেরত &t_list_dict_any;
}

    স্থির type_T *
ret_argv(পূর্ণসংখ্যা argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // argv() returns list of strings
    যদি (argcount == 0)
    {
	*decl_type = &t_list_any;
	ফেরত &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    ফেরত &t_any;
}

    স্থির type_T *
ret_remove(পূর্ণসংখ্যা argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    যদি (argcount > 0)
    {
	যদি (argtypes[0].type_curr->tt_type == VAR_LIST
		|| argtypes[0].type_curr->tt_type == VAR_DICT)
	{
	    যদি (argcount == 3)
	    {
		*decl_type = argtypes[0].type_decl;
		ফেরত argtypes[0].type_curr;
	    }
	    যদি (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    ফেরত argtypes[0].type_curr->tt_member;
	}
	যদি (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    ফেরত &t_সংখ্যা;
    }
    ফেরত &t_any;
}

    স্থির type_T *
ret_max_min(পূর্ণসংখ্যা argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    যদি (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	যদি (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	অন্যথা
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	যদি (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	অন্যথা
	    t = &t_any;
	ফেরত t;
    }
    ফেরত &t_any;
}

    স্থির type_T *
ret_getreg(পূর্ণসংখ্যা argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    যদি (argcount == 3)
    {
	*decl_type = &t_list_any;
	ফেরত &t_list_string;
    }
    ফেরত &t_string;
}

    স্থির type_T *
ret_virtcol(পূর্ণসংখ্যা argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    যদি (argcount > 1)
    {
	*decl_type = &t_list_any;
	ফেরত &t_list_সংখ্যা;
    }
    ফেরত &t_সংখ্যা;
}

    স্থির type_T *
ret_maparg(পূর্ণসংখ্যা argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    // Assume that if the fourth argument is passed it's non-zero
    যদি (argcount == 4)
	ফেরত &t_dict_any;
    ফেরত &t_string;
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
প্রকারলেখ কাঠামো
{
    অক্ষর	*f_name;	// function name
    অক্ষর	f_min_argc;	// minimal number of arguments
    অক্ষর	f_max_argc;	// maximal number of arguments
    অক্ষর	f_argtype;	// for method: FEARG_ values; bits FE_
    argcheck_T	*f_argcheck;	// list of functions to check argument types;
				// use "arg_any" (not NULL) to accept an
				// argument of any type
    অক্ষর	*f_argnames;	// names of the arguments as in the help,
				// separated by ", "; an argument without a
				// name is empty, NULL when there is none
    type_T	*(*f_retfunc)(পূর্ণসংখ্যা argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    শূন্য	(*f_func)(typval_T *args, typval_T *rvar);
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

#যদি defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#অন্যথা
# define MATH_FUNC(name) ফাঁকা
#endif
#ifdef FEAT_TIMERS
# define TIMER_FUNC(name) name
#অন্যথা
# define TIMER_FUNC(name) ফাঁকা
#endif
#ifdef FEAT_JOB_CHANNEL
# define JOB_FUNC(name) name
#অন্যথা
# define JOB_FUNC(name) ফাঁকা
#endif
#ifdef FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#অন্যথা
# define PROP_FUNC(name) ফাঁকা
#endif
#ifdef FEAT_SIGNS
# define SIGN_FUNC(name) name
#অন্যথা
# define SIGN_FUNC(name) ফাঁকা
#endif
#ifdef FEAT_SOUND
# define SOUND_FUNC(name) name
#অন্যথা
# define SOUND_FUNC(name) ফাঁকা
#endif
#ifdef FEAT_TERMINAL
# define TERM_FUNC(name) name
#অন্যথা
# define TERM_FUNC(name) ফাঁকা
#endif
#ifdef FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#অন্যথা
# define TABPANEL_FUNC(name) ফাঁকা
#endif
#ifdef FEAT_IMAGE
# define IMAGE_FUNC(name) name
#অন্যথা
# define IMAGE_FUNC(name) ফাঁকা
#endif

স্থির ধ্রুবক funcentry_T global_functions[] =
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
    {"and",		2, 2, FEARG_1,	    arg2_সংখ্যা,
			"expr1, expr2",
			ret_সংখ্যা,	    f_and},
    {"append",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_সংখ্যা_bool,    f_append},
    {"appendbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_সংখ্যা_bool,    f_appendbufline},
    {"argc",		0, 1, 0,	    arg1_সংখ্যা,
			"winid",
			ret_সংখ্যা,	    f_argc},
    {"argidx",		0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_argidx},
    {"arglistid",	0, 2, 0,	    arg2_সংখ্যা,
			"winnr, tabnr",
			ret_সংখ্যা,	    f_arglistid},
    {"argv",		0, 2, 0,	    arg2_সংখ্যা,
			"nr, winid",
			ret_argv,	    f_argv},
    {"asin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_asin},
    {"assert_beeps",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_সংখ্যা_bool,    f_assert_beeps},
    {"assert_equal",	2, 3, FEARG_2,	    ফাঁকা,
			"expected, actual, msg",
			ret_সংখ্যা_bool,    f_assert_equal},
    {"assert_equalfile", 2, 3, FEARG_1,	    arg3_string,
			"fname_one, fname_two, msg",
			ret_সংখ্যা_bool,    f_assert_equalfile},
    {"assert_exception", 1, 2, 0,	    arg2_string,
			"error, msg",
			ret_সংখ্যা_bool,    f_assert_exception},
    {"assert_fails",	1, 5, FEARG_1,	    arg15_assert_fails,
			"cmd, error, msg, lnum, context",
			ret_সংখ্যা_bool,    f_assert_fails},
    {"assert_false",	1, 2, FEARG_1,	    ফাঁকা,
			"actual, msg",
			ret_সংখ্যা_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_সংখ্যা_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_সংখ্যা_bool,    f_assert_match},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_সংখ্যা_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    ফাঁকা,
			"expected, actual, msg",
			ret_সংখ্যা_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_সংখ্যা_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_সংখ্যা_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    ফাঁকা,
			"actual, msg",
			ret_সংখ্যা_bool,    f_assert_true},
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
    {"balloon_gettext",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,
#ifdef FEAT_BEVAL
	    f_balloon_gettext
#অন্যথা
	    ফাঁকা
#endif
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#ifdef FEAT_BEVAL
	    f_balloon_show
#অন্যথা
	    ফাঁকা
#endif
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#যদি defined(FEAT_BEVAL_TERM)
	    f_balloon_split
#অন্যথা
	    ফাঁকা
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
			ret_list_সংখ্যা,    f_blob2list},
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
			ret_সংখ্যা,	    f_bufadd},
    {"bufexists",	1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_সংখ্যা_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_বাফার,	// obsolete
			"buf",
			ret_সংখ্যা_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_বাফার,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_বাফার,	// obsolete
			"buf",
			ret_সংখ্যা,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_সংখ্যা_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_সংখ্যা_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_বাফার_bool,
			"buf, create",
			ret_সংখ্যা,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_সংখ্যা,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_সংখ্যা,	    f_bufwinnr},
    {"byte2line",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"byte",
			ret_সংখ্যা,	    f_byte2line},
    {"byteidx",		2, 3, FEARG_1,	    arg3_string_সংখ্যা_bool,
			"expr, nr, utf16",
			ret_সংখ্যা,	    f_byteidx},
    {"byteidxcomp",	2, 3, FEARG_1,	    arg3_string_সংখ্যা_bool,
			"expr, nr, utf16",
			ret_সংখ্যা,	    f_byteidxcomp},
    {"call",		2, 3, FEARG_1,	    arg3_any_list_dict,
			"func, arglist, dict",
			ret_any,	    f_call},
    {"ceil",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ceil},
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_job,
			"handle",
			ret_সংখ্যা_bool,    JOB_FUNC(f_ch_canread)},
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
			ret_সংখ্যা,	    JOB_FUNC(f_ch_getbufnr)},
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
    {"changenr",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_changenr},
    {"char2nr",		1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_সংখ্যা,	    f_char2nr},
    {"charclass",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_সংখ্যা,	    f_charclass},
    {"charcol",		1, 2, FEARG_1,	    arg2_string_or_list_সংখ্যা,
			"expr, winid",
			ret_সংখ্যা,	    f_charcol},
    {"charidx",		2, 4, FEARG_1,	    arg4_string_সংখ্যা_bool_bool,
			"string, idx, countcc, utf16",
			ret_সংখ্যা,	    f_charidx},
    {"chdir",		1, 2, FEARG_1,	    arg2_string,
			"dir, scope",
			ret_string,	    f_chdir},
    {"cindent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_সংখ্যা,	    f_cindent},
    {"clearmatches",	0, 1, FEARG_1,	    arg1_সংখ্যা,
			"win",
			ret_void,	    f_clearmatches},
    {"cmdcomplete_info",0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_সংখ্যা,
			"expr, winid",
			ret_সংখ্যা,	    f_col},
    {"complete",	2, 2, FEARG_2,	    arg2_সংখ্যা_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_সংখ্যা,	    f_complete_add},
    {"complete_check",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_সংখ্যা_string,
			"msg, choices, default, type",
			ret_সংখ্যা,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    ফাঁকা,
			"expr",
			ret_copy,	    f_copy},
    {"cos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cos},
    {"cosh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cosh},
    {"count",		2, 4, FEARG_1,	    arg24_গুনতি,
			"comp, expr, ic, start",
			ret_সংখ্যা,	    f_গুনতি},
    {"cscope_connection",0,3, 0,	    arg3_সংখ্যা_string_string,
			"num, dbpath, prepend",
			ret_সংখ্যা,	    f_cscope_connection},
    {"cursor",		1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_সংখ্যা,	    f_cursor},
    {"debugbreak",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"pid",
			ret_সংখ্যা,
#ifdef MSWIN
	    f_debugbreak
#অন্যথা
	    ফাঁকা
#endif
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_সংখ্যা_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_বাফার_lnum_lnum,
			"buf, first, last",
			ret_সংখ্যা_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা_bool,    f_did_filetype},
    {"diff",		2, 3, FEARG_1,	    arg3_diff,
			"fromlist, tolist, options",
			ret_string_or_any_3,  f_diff},
    {"diff_filler",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_সংখ্যা,	    f_diff_filler},
    {"diff_hlID",	2, 2, FEARG_1,	    arg2_lnum_সংখ্যা,
			"lnum, col",
			ret_সংখ্যা,	    f_diff_hlID},
    {"digraph_get",	1, 1, FEARG_1,	    arg1_string,
			"chars",
			ret_string,	    f_digraph_get},
    {"digraph_getlist",	0, 1, FEARG_1,	    arg1_bool,
			"listall",
			ret_list_string_বস্তুসমূহ, f_digraph_getlist},
    {"digraph_set",	2, 2, FEARG_1,	    arg2_string,
			"chars, digraph",
			ret_bool,	f_digraph_set},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    ফাঁকা,
			"expr",
			ret_সংখ্যা_bool,    f_empty},
    {"environ",		0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    ফাঁকা,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা_bool,    f_eventhandler},
    {"executable",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_সংখ্যা,	    f_executable},
    {"execute",		1, 2, FEARG_1,	    arg12_execute,
			"command, silent",
			ret_string,	    f_execute},
    {"exepath",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_exepath},
    {"exists",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_সংখ্যা_bool,    f_exists},
    {"exists_compiled",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_সংখ্যা_bool,    f_exists_compiled},
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
			ret_সংখ্যা_bool,    f_filereadable},
    {"filecopy",	2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_সংখ্যা_bool,    f_filecopy},
    {"filereadable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_সংখ্যা_bool,    f_filereadable},
    {"filewritable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_সংখ্যা,	    f_filewritable},
    {"filter",		2, 2, FEARG_1,	    arg2_filter,
			"expr1, expr2",
			ret_first_arg,	    f_filter},
    {"finddir",		1, 3, FEARG_1,	    arg3_string_string_সংখ্যা,
			"name, path, count",
			ret_string_or_any_3,	    f_finddir},
    {"findfile",	1, 3, FEARG_1,	    arg3_string_string_সংখ্যা,
			"name, path, count",
			ret_string_or_any_3,	    f_findfile},
    {"flatten",		1, 2, FEARG_1,	    arg2_list_any_সংখ্যা,
			"list, maxdepth",
			ret_list_any,	    f_flatten},
    {"flattennew",	1, 2, FEARG_1,	    arg2_list_any_সংখ্যা,
			"list, maxdepth",
			ret_list_any,	    f_flattennew},
    {"float2nr",	1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_সংখ্যা,	    f_float2nr},
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
    {"foldclosed",	1, 2, FEARG_1,	    arg2_lnum_সংখ্যা,
			"lnum, winid",
			ret_সংখ্যা,	    f_foldclosed},
    {"foldclosedend",	1, 2, FEARG_1,	    arg2_lnum_সংখ্যা,
			"lnum, winid",
			ret_সংখ্যা,	    f_foldclosedend},
    {"foldlevel",	1, 2, FEARG_1,	    arg2_lnum_সংখ্যা,
			"lnum, winid",
			ret_সংখ্যা,	    f_foldlevel},
    {"foldtext",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_সংখ্যা,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_first_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
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
    {"getbgcolor",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_list_any,	    f_getbgcolor},
    {"getbufinfo",	0, 1, FEARG_1,	    arg1_বাফার_or_dict_any,
			"buf",
			ret_list_dict_any,  f_getbufinfo},
    {"getbufline",	2, 3, FEARG_1,	    arg3_বাফার_lnum_lnum,
			"buf, lnum, end",
			ret_list_string,    f_getbufline},
    {"getbufoneline",	2, 2, FEARG_1,	    arg2_বাফার_lnum,
			"buf, lnum",
			ret_string,	    f_getbufoneline},
    {"getbufvar",	2, 3, FEARG_1,	    arg3_বাফার_string_any,
			"buf, varname, def",
			ret_any,	    f_getbufvar},
    {"getcellpixels",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_সংখ্যা,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_getcmdwintype},
    {"getcompletion",	2, 3, FEARG_1,	    arg3_string_string_bool,
			"pat, type, filtered",
			ret_list_string,    f_getcompletion},
    {"getcompletiontype", 1, 1, FEARG_1,    arg1_string,
			"pat",
			ret_string,	    f_getcompletiontype},
    {"getcurpos",	0, 1, FEARG_1,	    arg1_সংখ্যা,
			"winid",
			ret_list_সংখ্যা,    f_getcurpos},
    {"getcursorcharpos", 0, 1, FEARG_1,	    arg1_সংখ্যা,
			"winid",
			ret_list_সংখ্যা,    f_getcursorcharpos},
    {"getcwd",		0, 2, FEARG_1,	    arg2_সংখ্যা,
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
			ret_সংখ্যা,	    f_getfsize},
    {"getftime",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_সংখ্যা,	    f_getftime},
    {"getftype",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getftype},
    {"getimstatus",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা_bool,    f_getimstatus},
    {"getinfo",		2, 3, FEARG_2,	    arg3_string_string_dict,
			"kind, name, opts",
			ret_dict_any,	    f_getinfo},
    {"getjumplist",	0, 2, FEARG_1,	    arg2_সংখ্যা,
			"winnr, tabnr",
			ret_list_any,	    f_getjumplist},
    {"getline",		1, 2, FEARG_1,	    arg2_lnum,
			"lnum, end",
			ret_getline,	    f_getline},
    {"getloclist",	1, 2, 0,	    arg2_সংখ্যা_dict_any,
			"nr, what",
			ret_list_or_dict_1, f_getloclist},
    {"getmarklist",	0, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_সংখ্যা,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_dict_সংখ্যা,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_getpid},
    {"getpos",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_সংখ্যা,    f_getpos},
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
    {"getstacktrace",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_list_dict_any,  f_getstacktrace},
    {"gettabinfo",	0, 1, FEARG_1,	    arg1_সংখ্যা,
			"tabnr",
			ret_list_dict_any,  f_gettabinfo},
    {"gettabvar",	2, 3, FEARG_1,	    arg3_সংখ্যা_string_any,
			"tabnr, varname, def",
			ret_any,	    f_gettabvar},
    {"gettabwinvar",	3, 4, FEARG_1,	    arg4_সংখ্যা_সংখ্যা_string_any,
			"tabnr, winnr, varname, def",
			ret_any,	    f_gettabwinvar},
    {"gettagstack",	0, 1, FEARG_1,	    arg1_সংখ্যা,
			"winnr",
			ret_dict_any,	    f_gettagstack},
    {"gettext",		1, 2, FEARG_1,	    arg2_string,
			"text, package",
			ret_string,	    f_gettext},
    {"getwininfo",	0, 1, FEARG_1,	    arg1_সংখ্যা,
			"winid",
			ret_list_dict_any,  f_getwininfo},
    {"getwinpos",	0, 1, FEARG_1,	    arg1_সংখ্যা,
			"timeout",
			ret_list_সংখ্যা,    f_getwinpos},
    {"getwinposx",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_getwinposy},
    {"getwinvar",	2, 3, FEARG_1,	    arg3_সংখ্যা_string_any,
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
			ret_সংখ্যা_bool,    f_has},
    {"has_key",		2, 2, FEARG_1,	    arg2_dict_any_string_or_nr,
			"dict, key",
			ret_সংখ্যা_bool,    f_has_চাবি},
    {"haslocaldir",	0, 2, FEARG_1,	    arg2_সংখ্যা,
			"winnr, tabnr",
			ret_সংখ্যা,	    f_haslocaldir},
    {"hasmapto",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"what, mode, abbr",
			ret_সংখ্যা_bool,    f_hasmapto},
    {"highlightID",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_সংখ্যা,	    f_hlID},
    {"highlight_exists",1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_সংখ্যা_bool,    f_hlexists},
    {"histadd",		2, 2, FEARG_2,	    arg2_string,
			"history, item",
			ret_সংখ্যা_bool,    f_histadd},
    {"histdel",		1, 2, FEARG_1,	    arg2_string_string_or_সংখ্যা,
			"history, item",
			ret_সংখ্যা_bool,    f_histdel},
    {"histget",		1, 2, FEARG_1,	    arg2_string_সংখ্যা,
			"history, index",
			ret_string,	    f_histget},
    {"histnr",		1, 1, FEARG_1,	    arg1_string,
			"history",
			ret_সংখ্যা,	    f_histnr},
    {"hlID",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_সংখ্যা,	    f_hlID},
    {"hlexists",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_সংখ্যা_bool,    f_hlexists},
    {"hlget",		0, 2, FEARG_1,	    arg2_string_bool,
			"name, resolve",
			ret_list_dict_any,  f_hlget},
    {"hlset",		1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_সংখ্যা_bool,    f_hlset},
    {"hostname",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    ফাঁকা,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_সংখ্যা,	    IMAGE_FUNC(f_image_add)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_সংখ্যা,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_সংখ্যা,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_সূচক,
			"object, expr, start, ic",
			ret_সংখ্যা,	    f_সূচক},
    {"indexof",		2, 3, FEARG_1,	    arg23_সূচক,
			"object, expr, opts",
			ret_সংখ্যা,	    f_indexof},
    {"input",		1, 3, FEARG_1,	    arg3_string,
			"prompt, text, completion",
			ret_string,	    f_প্রবেশ},
    {"inputdialog",	1, 3, FEARG_1,	    arg3_string,
			"prompt, text, cancelreturn",
			ret_string,	    f_inputdialog},
    {"inputlist",	1, 1, FEARG_1,	    arg1_list_string,
			"textlist",
			ret_সংখ্যা,	    f_inputlist},
    {"inputrestore",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_ঢোকাও,
			"object, item, idx",
			ret_first_arg,	    f_ঢোকাও},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_void,	    f_interrupt},
    {"invert",		1, 1, FEARG_1,	    arg1_সংখ্যা,
			"expr",
			ret_সংখ্যা,	    f_invert},
    {"isabsolutepath",	1, 1, FEARG_1,	    arg1_string,
			"path",
			ret_সংখ্যা_bool,    f_isabsolutepath},
    {"isdirectory",	1, 1, FEARG_1,	    arg1_string,
			"directory",
			ret_সংখ্যা_bool,    f_isdirectory},
    {"isinf",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_সংখ্যা,	    MATH_FUNC(f_isinf)},
    {"islocked",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_সংখ্যা_bool,    f_islocked},
    {"isnan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_সংখ্যা_bool,    MATH_FUNC(f_isnan)},
    {"items",		1, 1, FEARG_1,	    arg1_list_tuple_dict_blob_or_string,
			"expr",
			ret_list_বস্তুসমূহ,	    f_বস্তুসমূহ},
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
    {"job_stop",	1, 2, FEARG_1,	    arg2_job_string_or_সংখ্যা,
			"job, how",
			ret_সংখ্যা_bool,    JOB_FUNC(f_job_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    ফাঁকা,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    ফাঁকা,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_চাবিগুলি},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    ফাঁকা,	// obsolete
			ফাঁকা,
			ret_সংখ্যা,	    f_last_বাফার_nr},
    {"len",		1, 1, FEARG_1,	    arg1_len,
			"expr",
			ret_সংখ্যা,	    f_len},
    {"libcall",		3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_string,	    f_libcall},
    {"libcallnr",	3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_সংখ্যা,	    f_libcallnr},
    {"line",		1, 2, FEARG_1,	    arg2_string_সংখ্যা,
			"expr, winid",
			ret_সংখ্যা,	    f_সারি},
    {"line2byte",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_সংখ্যা,	    f_line2byte},
    {"lispindent",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_সংখ্যা,	    f_lispindent},
    {"list2blob",	1, 1, FEARG_1,	    arg1_list_সংখ্যা,
			"list",
			ret_blob,	    f_list2blob},
    {"list2str",	1, 2, FEARG_1,	    arg2_list_সংখ্যা_bool,
			"list, utf8",
			ret_string,	    f_list2str},
    {"list2tuple",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_tuple_any,	    f_list2tuple},
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_বাফার_bool_or_dict,
			"callback, buf, options",
			ret_সংখ্যা,	    f_listener_add},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"id",
			ret_সংখ্যা_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_localtime},
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
#অন্যথা
		ফাঁকা
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
			ret_সংখ্যা,	    f_match},
    {"matchadd",	2, 5, FEARG_1,	    arg25_matchadd,
			"group, pattern, priority, id, dict",
			ret_সংখ্যা,	    f_matchadd},
    {"matchaddpos",	2, 5, FEARG_1,	    arg25_matchaddpos,
			"group, pos, priority, id, dict",
			ret_সংখ্যা,	    f_matchaddpos},
    {"matcharg",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"nr",
			ret_list_string,    f_matcharg},
    {"matchbufline",	4, 5, FEARG_1,	    arg45_matchbufline,
			"buf, pat, lnum, end, dict",
			ret_list_any,	    f_matchbufline},
    {"matchdelete",	1, 2, FEARG_1,	    arg2_সংখ্যা,
			"id, win",
			ret_সংখ্যা_bool,    f_matchdelete},
    {"matchend",	2, 4, FEARG_1,	    arg24_match_func,
			"expr, pat, start, count",
			ret_সংখ্যা,	    f_matchend},
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
#অন্যথা
	    ফাঁকা
#endif
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_সংখ্যা,
			"name, flags, prot",
			ret_সংখ্যা_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_mode},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_MZSCHEME
	    f_mzeval
#অন্যথা
	    ফাঁকা
#endif
			},
    {"nextnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_সংখ্যা,	    f_nextnonblank},
    {"ngettext",	3, 4, FEARG_3,	    arg4_string_string_সংখ্যা_string,
			"single, plural, number, domain",
			ret_string,	    f_ngettext},
    {"nr2char",		1, 2, FEARG_1,	    arg2_সংখ্যা_bool,
			"expr, utf8",
			ret_string,	    f_nr2char},
    {"or",		2, 2, FEARG_1,	    arg2_সংখ্যা,
			"expr1, expr2",
			ret_সংখ্যা,	    f_or},
    {"pathshorten",	1, 2, FEARG_1,	    arg2_string_সংখ্যা,
			"path, len",
			ret_string,	    f_pathshorten},
    {"perleval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_PERL
	    f_perleval
#অন্যথা
	    ফাঁকা
#endif
			},
    {"popup_atcursor",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_সংখ্যা,	    PROP_FUNC(f_popup_atcursor)},
    {"popup_beval",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_সংখ্যা,	    PROP_FUNC(f_popup_beval)},
    {"popup_clear",	0, 1, 0,	    arg1_bool,
			"force",
			ret_void,	    PROP_FUNC(f_popup_clear)},
    {"popup_close",	1, 2, FEARG_1,	    arg2_সংখ্যা_any,
			"id, result",
			ret_void,	    PROP_FUNC(f_popup_close)},
    {"popup_create",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_সংখ্যা,	    PROP_FUNC(f_popup_create)},
    {"popup_dialog",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_সংখ্যা,	    PROP_FUNC(f_popup_dialog)},
    {"popup_filter_menu", 2, 2, 0,	    arg2_সংখ্যা_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_menu)},
    {"popup_filter_yesno", 2, 2, 0,	    arg2_সংখ্যা_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_yesno)},
    {"popup_findecho",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    PROP_FUNC(f_popup_findpreview)},
    {"popup_getoptions", 1, 1, FEARG_1,	    arg1_সংখ্যা,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getoptions)},
    {"popup_getpos",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getpos)},
    {"popup_hide",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"id",
			ret_void,	    PROP_FUNC(f_popup_hide)},
    {"popup_list",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_list_সংখ্যা,    PROP_FUNC(f_popup_list)},
    {"popup_locate",	2, 2, 0,	    arg2_সংখ্যা,
			"row, col",
			ret_সংখ্যা,	    PROP_FUNC(f_popup_locate)},
    {"popup_menu",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_সংখ্যা,	    PROP_FUNC(f_popup_menu)},
    {"popup_move",	2, 2, FEARG_1,	    arg2_সংখ্যা_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_move)},
    {"popup_notification", 2, 2, FEARG_1,   arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_সংখ্যা,	    PROP_FUNC(f_popup_notification)},
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_সংখ্যা_বাফার,
			"id, buf",
			ret_সংখ্যা_bool,    PROP_FUNC(f_popup_setbuf)},
    {"popup_setoptions", 2, 2, FEARG_1,	    arg2_সংখ্যা_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_setoptions)},
    {"popup_settext",	2, 2, FEARG_1,	    arg2_সংখ্যা_string_or_list,
			"id, text",
			ret_void,	    PROP_FUNC(f_popup_settext)},
    {"popup_show",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"id",
			ret_সংখ্যা,	    PROP_FUNC(f_popup_show)},
    {"pow",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"x, y",
			ret_float,	    f_pow},
    {"preinserted",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_সংখ্যা,	    f_prevnonblank},
    {"printf",		1, 19, FEARG_2,	    arg119_printf,
			"fmt, expr1",
			ret_string,	    f_printf},
    {"prompt_getprompt", 1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_string,	    JOB_FUNC(f_prompt_getprompt)},
    {"prompt_setcallback", 2, 2, FEARG_1,   arg2_বাফার_any,
			"buf, expr",
			ret_void,	    JOB_FUNC(f_prompt_setcallback)},
    {"prompt_setinterrupt", 2, 2, FEARG_1,  arg2_বাফার_any,
			"buf, expr",
			ret_void,	    JOB_FUNC(f_prompt_setinterrupt)},
    {"prompt_setprompt", 2, 2, FEARG_1,	    arg2_বাফার_string,
			"buf, text",
			ret_void,	    JOB_FUNC(f_prompt_setprompt)},
    {"prop_add",	3, 3, FEARG_1,	    arg3_সংখ্যা_সংখ্যা_dict,
			"lnum, col, props",
			ret_সংখ্যা,	    PROP_FUNC(f_prop_add)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_add_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_সংখ্যা_সংখ্যা_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_find)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_সংখ্যা_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_সংখ্যা_সংখ্যা,
			"props, lnum, lnum_end",
			ret_সংখ্যা,	    PROP_FUNC(f_prop_remove)},
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
    {"pum_getpos",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_dict_সংখ্যা,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON3
	    f_py3eval
#অন্যথা
	    ফাঁকা
#endif
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON
	    f_pyeval
#অন্যথা
	    ফাঁকা
#endif
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#যদি defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#অন্যথা
	    ফাঁকা
#endif
			},
    {"rand",		0, 1, FEARG_1,	    arg1_list_সংখ্যা,
			"expr",
			ret_সংখ্যা,	    f_rand},
    {"range",		1, 3, FEARG_1,	    arg3_সংখ্যা,
			"expr, max, stride",
			ret_list_সংখ্যা,    f_range},
    {"readblob",	1, 3, FEARG_1,	    arg3_string_সংখ্যা_সংখ্যা,
			"fname, offset, size",
			ret_blob,	    f_readblob},
    {"readdir",		1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_string,    f_readdir},
    {"readdirex",	1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_dict_any,  f_readdirex},
    {"readfile",	1, 3, FEARG_1,	    arg3_string_string_সংখ্যা,
			"fname, type, max",
			ret_list_string,    f_readfile},
    {"redraw_listener_add", 1, 1, FEARG_1,  arg1_dict_any,
			"opts",
			ret_সংখ্যা,	    f_redraw_listener_add},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_সংখ্যা,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_reg_recording},
    {"reltime",		0, 2, FEARG_1,	    arg2_list_সংখ্যা,
			"start, end",
			ret_list_সংখ্যা,    f_reltime},
    {"reltimefloat",	1, 1, FEARG_1,	    arg1_list_সংখ্যা,
			"time",
			ret_float,	    f_reltimefloat},
    {"reltimestr",	1, 1, FEARG_1,	    arg1_list_সংখ্যা,
			"time",
			ret_string,	    f_reltimestr},
    {"remote_expr",	2, 4, FEARG_1,	    arg24_remote_রাশিমালা,
			"server, string, idvar, timeout",
			ret_string,	    f_remote_রাশিমালা},
    {"remote_foreground", 1, 1, FEARG_1,    arg1_string,
			"server",
			ret_void,	    f_remote_foreground},
    {"remote_peek",	1, 2, FEARG_1,	    arg2_string,
			"serverid, retvar",
			ret_সংখ্যা,	    f_remote_peek},
    {"remote_read",	1, 2, FEARG_1,	    arg2_string_সংখ্যা,
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
			ret_সংখ্যা_bool,    f_rename},
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
#অন্যথা
	    ফাঁকা
#endif
			},
    {"screenattr",	2, 2, FEARG_1,	    arg2_সংখ্যা,
			"row, col",
			ret_সংখ্যা,	    f_screenattr},
    {"screenchar",	2, 2, FEARG_1,	    arg2_সংখ্যা,
			"row, col",
			ret_সংখ্যা,	    f_screenchar},
    {"screenchars",	2, 2, FEARG_1,	    arg2_সংখ্যা,
			"row, col",
			ret_list_সংখ্যা,    f_screenchars},
    {"screencol",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_সংখ্যা,
			"winid, lnum, col",
			ret_dict_সংখ্যা,    f_screenpos},
    {"screenrow",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_screenrow},
    {"screenstring",	2, 2, FEARG_1,	    arg2_সংখ্যা,
			"row, col",
			ret_string,	    f_screenstring},
    {"search",		1, 5, FEARG_1,	    arg15_অনুসন্ধান,
			"pattern, flags, stopline, timeout, skip",
			ret_সংখ্যা,	    f_অনুসন্ধান},
    {"searchcount",	0, 1, FEARG_1,	    arg1_dict_any,
			"options",
			ret_dict_সংখ্যা,    f_searchcount},
    {"searchdecl",	1, 3, FEARG_1,	    arg3_string_bool_bool,
			"name, global, thisblock",
			ret_সংখ্যা_bool,    f_searchdecl},
    {"searchpair",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_সংখ্যা,	    f_searchpair},
    {"searchpairpos",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_list_সংখ্যা,    f_searchpairpos},
    {"searchpos",	1, 5, FEARG_1,	    arg15_অনুসন্ধান,
			"pattern, flags, stopline, timeout, skip",
			ret_list_সংখ্যা,    f_searchpos},
    {"server2client",	2, 2, FEARG_1,	    arg2_string,
			"clientid, string",
			ret_সংখ্যা_bool,    f_server2client},
    {"serverlist",	0, 1, 0,	    arg1_dict_any,
			"dict",
			ret_string_or_any_1,	    f_serverlist},
    {"setbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_সংখ্যা_bool,    f_setbufline},
    {"setbufvar",	3, 3, FEARG_3,	    arg3_বাফার_string_any,
			"buf, varname, val",
			ret_void,	    f_setbufvar},
    {"setcellwidths",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_void,	    f_setcellwidths},
    {"setcharpos",	2, 2, FEARG_2,	    arg2_string_list_সংখ্যা,
			"expr, list",
			ret_সংখ্যা_bool,    f_setcharpos},
    {"setcharsearch",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_setcharsearch},
    {"setcmdline",	1, 2, FEARG_1,	    arg2_string_সংখ্যা,
			"str, pos",
			ret_সংখ্যা_bool,    f_setcmdline},
    {"setcmdpos",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"pos",
			ret_সংখ্যা_bool,    f_setcmdpos},
    {"setcursorcharpos", 1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_সংখ্যা_bool,    f_setcursorcharpos},
    {"setenv",		2, 2, FEARG_2,	    arg2_string_any,
			"name, val",
			ret_void,	    f_setenv},
    {"setfperm",	2, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_সংখ্যা_bool,    f_setfperm},
    {"setline",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_সংখ্যা_bool,    f_setline},
    {"setloclist",	2, 4, FEARG_2,	    arg24_setloclist,
			"nr, list, action, what",
			ret_সংখ্যা_bool,    f_setloclist},
    {"setmatches",	1, 2, FEARG_1,	    arg2_list_any_সংখ্যা,
			"list, win",
			ret_সংখ্যা_bool,    f_setmatches},
    {"setpos",		2, 2, FEARG_2,	    arg2_string_list_সংখ্যা,
			"expr, list",
			ret_সংখ্যা_bool,    f_setpos},
    {"setqflist",	1, 3, FEARG_1,	    arg13_setqflist,
			"list, action, what",
			ret_সংখ্যা_bool,    f_setqflist},
    {"setreg",		2, 3, FEARG_2,	    arg3_string_any_string,
			"regname, value, options",
			ret_সংখ্যা_bool,    f_setreg},
    {"settabvar",	3, 3, FEARG_3,	    arg3_সংখ্যা_string_any,
			"tabnr, varname, val",
			ret_void,	    f_settabvar},
    {"settabwinvar",	4, 4, FEARG_4,	    arg4_সংখ্যা_সংখ্যা_string_any,
			"tabnr, winnr, varname, val",
			ret_void,	    f_settabwinvar},
    {"settagstack",	2, 3, FEARG_2,	    arg23_settagstack,
			"nr, dict, action",
			ret_সংখ্যা_bool,    f_settagstack},
    {"setwinvar",	3, 3, FEARG_3,	    arg3_সংখ্যা_string_any,
			"winnr, varname, val",
			ret_void,	    f_setwinvar},
    {"sha256",		1, 1, FEARG_1,	    arg1_string_or_blob,
			"expr",
			ret_string,
#ifdef FEAT_CRYPT
	    f_sha256
#অন্যথা
	    ফাঁকা
#endif
			},
    {"shellescape",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, special",
			ret_string,	    f_shellescape},
    {"shiftwidth",	0, 1, FEARG_1,	    arg1_সংখ্যা,
			"col",
			ret_সংখ্যা,	    f_shiftwidth},
    {"sign_define",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"name, dict",
			ret_sign_define,	    SIGN_FUNC(f_sign_define)},
    {"sign_getdefined",	0, 1, FEARG_1,	    arg1_string,
			"name",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getdefined)},
    {"sign_getplaced",	0, 2, FEARG_1,	    arg02_sign_getplaced,
			"buf, dict",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getplaced)},
    {"sign_jump",	3, 3, FEARG_1,	    arg3_সংখ্যা_string_বাফার,
			"id, group, buf",
			ret_সংখ্যা,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_place,
			"id, group, name, buf, dict",
			ret_সংখ্যা,	    SIGN_FUNC(f_sign_place)},
    {"sign_placelist",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_সংখ্যা,    SIGN_FUNC(f_sign_placelist)},
    {"sign_undefine",	0, 1, FEARG_1,	    arg1_string_or_list_string,
			"name",
			ret_সংখ্যা_bool,    SIGN_FUNC(f_sign_undefine)},
    {"sign_unplace",	1, 2, FEARG_1,	    arg2_string_dict,
			"group, dict",
			ret_সংখ্যা_bool,    SIGN_FUNC(f_sign_unplace)},
    {"sign_unplacelist", 1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_সংখ্যা,    SIGN_FUNC(f_sign_unplacelist)},
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
    {"sound_clear",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_void,	    SOUND_FUNC(f_sound_clear)},
    {"sound_playevent",	1, 2, FEARG_1,	    arg2_string_any,
			"name, callback",
			ret_সংখ্যা,	    SOUND_FUNC(f_sound_playevent)},
    {"sound_playfile",	1, 2, FEARG_1,	    arg2_string_any,
			"path, callback",
			ret_সংখ্যা,	    SOUND_FUNC(f_sound_playfile)},
    {"sound_stop",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"id",
			ret_void,	    SOUND_FUNC(f_sound_stop)},
    {"soundfold",	1, 1, FEARG_1,	    arg1_string,
			"word",
			ret_string,	    f_soundfold},
    {"spellbadword",	0, 1, FEARG_1,	    arg1_string,
			"sentence",
			ret_list_string,    f_spellbadword},
    {"spellsuggest",	1, 3, FEARG_1,	    arg3_string_সংখ্যা_bool,
			"word, max, capital",
			ret_list_string,    f_spellsuggest},
    {"split",		1, 3, FEARG_1,	    arg3_string_string_bool,
			"string, pattern, keepempty",
			ret_list_string,    f_split},
    {"sqrt",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sqrt},
    {"srand",		0, 1, FEARG_1,	    arg1_সংখ্যা,
			"expr",
			ret_list_সংখ্যা,    f_srand},
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
			ret_list_সংখ্যা,    f_str2list},
    {"str2nr",		1, 3, FEARG_1,	    arg3_string_সংখ্যা_bool,
			"string, base, quoted",
			ret_সংখ্যা,	    f_str2nr},
    {"strcharlen",	1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_সংখ্যা,	    f_strcharlen},
    {"strcharpart",	2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, skipcc",
			ret_string,	    f_strcharpart},
    {"strchars",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, skipcc",
			ret_সংখ্যা,	    f_strchars},
    {"strdisplaywidth",	1, 2, FEARG_1,	    arg2_string_সংখ্যা,
			"string, col",
			ret_সংখ্যা,	    f_strdisplaywidth},
    {"strftime",	1, 2, FEARG_1,	    arg2_string_সংখ্যা,
			"format, time",
			ret_string,
#ifdef HAVE_STRFTIME
	    f_strftime
#অন্যথা
	    ফাঁকা
#endif
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_সংখ্যা,
			"str, index",
			ret_সংখ্যা,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_সংখ্যা,
			"haystack, needle, start",
			ret_সংখ্যা,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, ফাঁকা,
			"expr",
			ret_string,	    f_string},
    {"strlen",		1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_সংখ্যা,	    f_strlen},
    {"strpart",		2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, chars",
			ret_string,	    f_strpart},
    {"strptime",	2, 2, FEARG_1,	    arg2_string,
			"format, timestring",
			ret_সংখ্যা,
#ifdef HAVE_STRPTIME
	    f_strptime
#অন্যথা
	    ফাঁকা
#endif
			},
    {"strridx",		2, 3, FEARG_1,	    arg3_string_string_সংখ্যা,
			"haystack, needle, start",
			ret_সংখ্যা,	    f_strridx},
    {"strtrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_strtrans},
    {"strutf16len",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, countcc",
			ret_সংখ্যা,	    f_strutf16len},
    {"strwidth",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_সংখ্যা,	    f_strwidth},
    {"submatch",	1, 2, FEARG_1,	    arg2_সংখ্যা_bool,
			"nr, list",
			ret_string_or_any_2,	    f_submatch},
    {"substitute",	4, 4, FEARG_1,	    arg4_string_string_any_string,
			"string, pat, sub, flags",
			ret_string,	    f_substitute},
    {"swapfilelist",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_string,	    f_swapname},
    {"synID",		3, 3, 0,	    arg3_lnum_সংখ্যা_bool,
			"lnum, col, trans",
			ret_সংখ্যা,	    f_synID},
    {"synIDattr",	2, 3, FEARG_1,	    arg3_সংখ্যা_string_string,
			"synID, what, mode",
			ret_string,	    f_synIDattr},
    {"synIDtrans",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"synID",
			ret_সংখ্যা,	    f_synIDtrans},
    {"synconcealed",	2, 2, 0,	    arg2_lnum_সংখ্যা,
			"lnum, col",
			ret_list_any,	    f_synconcealed},
    {"synstack",	2, 2, 0,	    arg2_lnum_সংখ্যা,
			"lnum, col",
			ret_list_সংখ্যা,    f_synstack},
    {"system",		1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_string,	    f_system},
    {"systemlist",	1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_list_string,    f_systemlist},
    {"tabpagebuflist",	0, 1, FEARG_1,	    arg1_সংখ্যা,
			"arg",
			ret_list_সংখ্যা,    f_tabpagebuflist},
    {"tabpagenr",	0, 1, 0,	    arg1_string,
			"arg",
			ret_সংখ্যা,	    f_tabpagenr},
    {"tabpagewinnr",	1, 2, FEARG_1,	    arg2_সংখ্যা_string,
			"tabarg, arg",
			ret_সংখ্যা,	    f_tabpagewinnr},
    {"tabpanel_getinfo", 0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_সংখ্যা_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
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
    {"tempname",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_সংখ্যা,	    TERM_FUNC(f_term_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_সংখ্যা,	    TERM_FUNC(f_term_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_বাফার_string_dict,
			"buf, filename, options",
			ret_void,	    TERM_FUNC(f_term_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_বাফার,
			"buf",
			ret_সংখ্যা,	    TERM_FUNC(f_term_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_বাফার,
			"buf",
			ret_list_string,
#যদি defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_getansicolors
#অন্যথা
	    ফাঁকা
#endif
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_সংখ্যা_string,
			"attr, what",
			ret_সংখ্যা,	    TERM_FUNC(f_term_getattr)},
    {"term_getcursor",	1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_list_any,	    TERM_FUNC(f_term_getcursor)},
    {"term_getjob",	1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_job,	    TERM_FUNC(f_term_getjob)},
    {"term_getline",	2, 2, FEARG_1,	    arg2_বাফার_lnum,
			"buf, row",
			ret_string,	    TERM_FUNC(f_term_getline)},
    {"term_getscrolled", 1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_সংখ্যা,	    TERM_FUNC(f_term_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_list_সংখ্যা,    TERM_FUNC(f_term_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_string,	    TERM_FUNC(f_term_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_বাফার,
			"buf",
			ret_string,	    TERM_FUNC(f_term_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_বাফার_bool,
			"buf, input",
			ret_string,	    TERM_FUNC(f_term_gettty)},
    {"term_list",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_list_সংখ্যা,    TERM_FUNC(f_term_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_বাফার_lnum,
			"buf, row",
			ret_list_dict_any,  TERM_FUNC(f_term_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_বাফার_string,
			"buf, keys",
			ret_void,	    TERM_FUNC(f_term_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_বাফার_list_any,
			"buf, colors",
			ret_void,
#যদি defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_setansicolors
#অন্যথা
	    ফাঁকা
#endif
			},
    {"term_setapi",	2, 2, FEARG_1,	    arg2_বাফার_string,
			"buf, expr",
			ret_void,	    TERM_FUNC(f_term_setapi)},
    {"term_setkill",	2, 2, FEARG_1,	    arg2_বাফার_string,
			"buf, how",
			ret_void,	    TERM_FUNC(f_term_setkill)},
    {"term_setrestore",	2, 2, FEARG_1,	    arg2_বাফার_string,
			"buf, command",
			ret_void,	    TERM_FUNC(f_term_setrestore)},
    {"term_setsize",	3, 3, FEARG_1,	    arg3_বাফার_সংখ্যা_সংখ্যা,
			"buf, rows, cols",
			ret_void,	    TERM_FUNC(f_term_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_সংখ্যা,	    TERM_FUNC(f_term_start)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_বাফার_সংখ্যা,
			"buf, time",
			ret_void,	    TERM_FUNC(f_term_wait)},
    {"terminalprops",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_সংখ্যা,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    ফাঁকা,
			ফাঁকা,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   ফাঁকা,
			ফাঁকা,
			ret_void,	    f_test_garbagecollect_soon},
    {"test_getvalue",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_সংখ্যা,	    f_test_getvalue},
    {"test_gui_event",	2, 2, FEARG_1,	    arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_gui_event},
    {"test_ignore_error", 1, 1, FEARG_1,    arg1_string,
			"expr",
			ret_void,	    f_test_ignore_error},
    {"test_mswin_event", 2, 2, FEARG_1,     arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_mswin_event},
    {"test_null_blob",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_channel,	    JOB_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_job,	    JOB_FUNC(f_test_null_job)},
    {"test_null_list",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_set},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_সংখ্যা,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, ফাঁকা,
			"expr",
			ret_সংখ্যা,	    f_test_refcount},
    {"test_setmouse",	2, 2, 0,	    arg2_সংখ্যা,
			"row, col",
			ret_void,	    f_test_setmouse},
    {"test_settime",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"expr",
			ret_void,	    f_test_settime},
    {"test_srand_seed",	0, 1, FEARG_1,	    arg1_সংখ্যা,
			"seed",
			ret_void,	    f_test_srand_seed},
    {"test_unknown",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_সংখ্যা,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_সংখ্যা_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_সংখ্যা_any_dict,
			"time, callback, options",
			ret_সংখ্যা,	    TIMER_FUNC(f_timer_start)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
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
    {"trim",		1, 3, FEARG_1,	    arg3_string_string_সংখ্যা,
			"text, mask, dir",
			ret_string,	    f_trim},
    {"trunc",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_trunc},
    {"tuple2list",	1, 1, FEARG_1,	    arg1_tuple_any,
			"tuple",
			ret_list_any,	    f_tuple2list},
    {"type",		1, 1, FEARG_1|FE_X, ফাঁকা,
			"expr",
			ret_সংখ্যা,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, ফাঁকা,
			"expr",
			ret_string,	    f_typename},
    {"undofile",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_string,	    f_undofile},
    {"undotree",	0, 1, FEARG_1,	    arg1_বাফার,
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
    {"utf16idx",	2, 4, FEARG_1,	    arg4_string_সংখ্যা_bool_bool,
			"string, idx, countcc, charidx",
			ret_সংখ্যা,	    f_utf16idx},
    {"values",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_member,    f_মানসমূহ},
    {"virtcol",		1, 3, FEARG_1,	    arg3_string_or_list_bool_সংখ্যা,
			"expr, list, winid",
			ret_virtcol,	    f_virtcol},
    {"virtcol2col",	3, 3, FEARG_1,	    arg3_সংখ্যা,
			"winid, lnum, col",
			ret_সংখ্যা,	    f_virtcol2col},
    {"visualmode",	0, 1, 0,	    arg1_bool,
			"expr",
			ret_string,	    f_visualmode},
    {"wildmenumode",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_void,	    f_wildtrigger},
    {"win_execute",	2, 3, FEARG_2,	    arg23_win_execute,
			"id, command, silent",
			ret_string,	    f_win_execute},
    {"win_findbuf",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"bufnr",
			ret_list_সংখ্যা,    f_win_findbuf},
    {"win_getid",	0, 2, FEARG_1,	    arg2_সংখ্যা,
			"win, tab",
			ret_সংখ্যা,	    f_win_getid},
    {"win_gettype",	0, 1, FEARG_1,	    arg1_সংখ্যা,
			"nr",
			ret_string,	    f_win_gettype},
    {"win_gotoid",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"expr",
			ret_সংখ্যা_bool,    f_win_gotoid},
    {"win_id2tabwin",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"expr",
			ret_list_সংখ্যা,    f_win_id2tabwin},
    {"win_id2win",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"expr",
			ret_সংখ্যা,	    f_win_id2win},
    {"win_move_separator", 2, 2, FEARG_1,   arg2_সংখ্যা,
			"nr, offset",
			ret_সংখ্যা_bool,    f_win_move_separator},
    {"win_move_statusline", 2, 2, FEARG_1,  arg2_সংখ্যা,
			"nr, offset",
			ret_সংখ্যা_bool,    f_win_move_statusline},
    {"win_screenpos",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"nr",
			ret_list_সংখ্যা,    f_win_screenpos},
    {"win_splitmove",   2, 3, FEARG_1,	    arg3_সংখ্যা_সংখ্যা_dict,
			"nr, target, options",
			ret_সংখ্যা_bool,    f_win_splitmove},
    {"winbufnr",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"nr",
			ret_সংখ্যা,	    f_winbufnr},
    {"wincol",		0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"nr",
			ret_সংখ্যা,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_সংখ্যা,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_সংখ্যা,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_সংখ্যা,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_dict_সংখ্যা,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_সংখ্যা,
			"nr",
			ret_সংখ্যা,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    ফাঁকা,
			ফাঁকা,
			ret_dict_সংখ্যা,    f_wordcount},
    {"writefile",	2, 3, FEARG_1,	    arg23_writefile,
			"object, fname, flags",
			ret_সংখ্যা_bool,    f_writefile},
    {"xor",		2, 2, FEARG_1,	    arg2_সংখ্যা,
			"expr1, expr2",
			ret_সংখ্যা,	    f_xor},
};

/*
 * Return true if specified function allows a type as an argument.
 */
    স্থির পূর্ণসংখ্যা
func_allows_type(পূর্ণসংখ্যা idx)
{
    ফেরত (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, পূর্ণসংখ্যা idx)
{
    স্থির পূর্ণসংখ্যা	intidx = -1;
    char_u	*name;

    যদি (idx == 0)
	intidx = -1;
    যদি (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	যদি (name != ফাঁকা)
	{
	    যদি (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_pattern, 2) == 0)
		ফেরত cat_prefix_varname('g', name);
	    ফেরত name;
	}
    }
    যদি (++intidx < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	যদি (global_functions[intidx].f_func == ফাঁকা)
	    ফেরত (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	যদি (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	ফেরত IObuff;
    }

    ফেরত ফাঁকা;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_রাশিমালা_name(expand_T *xp, পূর্ণসংখ্যা idx)
{
    স্থির পূর্ণসংখ্যা	intidx = -1;
    char_u	*name;

    যদি (idx == 0)
	intidx = -1;
    যদি (intidx < 0)
    {
	name = get_function_name(xp, idx);
	যদি (name != ফাঁকা)
	    ফেরত name;
    }
    ফেরত get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    স্থির পূর্ণসংখ্যা
find_internal_func_opt(char_u *name, পূর্ণসংখ্যা implemented)
{
    পূর্ণসংখ্যা		first = 0;
    পূর্ণসংখ্যা		last;
    পূর্ণসংখ্যা		cmp;
    পূর্ণসংখ্যা		x;

    last = (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(global_functions) - 1;

    // Find the function name in the table. Binary search.
    যখন (first <= last)
    {
	x = first + ((unsigned)(last - first) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	যদি (cmp < 0)
	    last = x - 1;
	অন্যথা যদি (cmp > 0)
	    first = x + 1;
	অন্যথা যদি (implemented && global_functions[x].f_func == ফাঁকা)
	    বিরতি;
	অন্যথা
	    ফেরত x;
    }
    ফেরত -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    পূর্ণসংখ্যা
find_internal_func(char_u *name)
{
    ফেরত find_internal_func_opt(name, TRUE);
}

    পূর্ণসংখ্যা
has_internal_func(char_u *name)
{
    ফেরত find_internal_func_opt(name, TRUE) >= 0;
}

    স্থির পূর্ণসংখ্যা
has_internal_func_name(char_u *name)
{
    ফেরত find_internal_func_opt(name, FALSE) >= 0;
}

    অক্ষর *
internal_func_name(পূর্ণসংখ্যা idx)
{
    ফেরত global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    পূর্ণসংখ্যা
internal_func_check_arg_types(
	type2_T	*types,
	পূর্ণসংখ্যা	idx,
	পূর্ণসংখ্যা	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    যদি (!(func_allows_type(idx)))
    {
	জন্য (পূর্ণসংখ্যা i = 0; i < argcount; ++i)
	    যদি (check_type_is_মান(types[i].type_curr) == FAIL)
		ফেরত FAIL;
    }

    ধ্রুবক argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    যদি (argchecks == ফাঁকা)
	ফেরত OK;

    argcontext_T context;

    context.arg_গুনতি = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    জন্য (পূর্ণসংখ্যা i = 0; i < argcount && argchecks[i] != ফাঁকা; ++i)
    {
	context.arg_idx = i;
	যদি (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    শূন্য
internal_func_get_argcount(পূর্ণসংখ্যা idx, পূর্ণসংখ্যা *argcount, পূর্ণসংখ্যা *min_argcount)
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
	পূর্ণসংখ্যা	    idx,
	পূর্ণসংখ্যা	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = ফাঁকা;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    যদি (*decl_type == ফাঁকা)
	*decl_type = ret;
    current_type_gap = ফাঁকা;
    ফেরত ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    পূর্ণসংখ্যা
internal_func_is_map(পূর্ণসংখ্যা idx)
{
    ফেরত global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    পূর্ণসংখ্যা
check_internal_func(পূর্ণসংখ্যা idx, পূর্ণসংখ্যা argcount)
{
    funcerror_T	    res;
    অক্ষর	    *name;

    যদি (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    অন্যথা যদি (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    অন্যথা
	ফেরত global_functions[idx].f_argtype & FEARG_MASK;

    name = internal_func_name(idx);
    যদি (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    অন্যথা
	semsg(_(e_not_enough_arguments_for_function_str), name);
    ফেরত -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    স্থির পূর্ণসংখ্যা
check_args_for_type(পূর্ণসংখ্যা idx, পূর্ণসংখ্যা argcount, typval_T *argvars)
{
    যদি (!func_allows_type(idx))
    {
	জন্য (পূর্ণসংখ্যা i = 0; i < argcount; ++i)
	    যদি (check_typval_is_মান(&argvars[i]) == FAIL)
		ফেরত FAIL;
    }
    ফেরত OK;
}

    funcerror_T
call_internal_func(
	char_u	    *name,
	পূর্ণসংখ্যা	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    পূর্ণসংখ্যা i;

    i = find_internal_func(name);
    যদি (i < 0)
	ফেরত FCERR_UNKNOWN;
    যদি (argcount < global_functions[i].f_min_argc)
	ফেরত FCERR_TOOFEW;
    যদি (argcount > global_functions[i].f_max_argc)
	ফেরত FCERR_TOOMANY;
    যদি (check_args_for_type(i, argcount, argvars) == FAIL)
	ফেরত FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    যদি (in_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    ফেরত FCERR_NONE;
}

    শূন্য
call_internal_func_by_idx(
	পূর্ণসংখ্যা	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    যদি (in_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_internal_method(
	char_u	    *name,
	পূর্ণসংখ্যা	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    পূর্ণসংখ্যা		fi;
    typval_T	argv[MAX_FUNC_ARGS + 1];

    fi = find_internal_func(name);
    যদি (fi < 0)
	ফেরত FCERR_UNKNOWN;
    যদি ((global_functions[fi].f_argtype & FEARG_MASK) == 0)
	ফেরত FCERR_NOTMETHOD;
    যদি (argcount + 1 < global_functions[fi].f_min_argc)
	ফেরত FCERR_TOOFEW;
    যদি (argcount + 1 > global_functions[fi].f_max_argc)
	ফেরত FCERR_TOOMANY;
    যদি (check_args_for_type(fi, argcount, argvars) == FAIL)
	ফেরত FCERR_OTHER;

    যদি ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_2)
    {
	যদি (argcount < 1)
	    ফেরত FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	জন্য (পূর্ণসংখ্যা i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    অন্যথা যদি ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_3)
    {
	যদি (argcount < 2)
	    ফেরত FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	জন্য (পূর্ণসংখ্যা i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    অন্যথা যদি ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_4)
    {
	যদি (argcount < 3)
	    ফেরত FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	জন্য (পূর্ণসংখ্যা i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    অন্যথা
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	জন্য (পূর্ণসংখ্যা i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    যদি (check_args_for_type(fi, argcount + 1, argv) == FAIL)
	ফেরত FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    ফেরত FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    পূর্ণসংখ্যা
non_zero_arg(typval_T *argvars)
{
    ফেরত ((argvars[0].v_type == VAR_সংখ্যা
		&& argvars[0].vval.v_সংখ্যা != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_সংখ্যা == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != ফাঁকা
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    স্থির শূন্য
f_and(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    rettv->vval.v_সংখ্যা = tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা)
					& tv_get_সংখ্যা_chk(&argvars[1], ফাঁকা);
}

/*
 * "balloon_show()" function
 */
#ifdef FEAT_BEVAL
    স্থির শূন্য
f_balloon_gettext(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    যদি (balloonEval == ফাঁকা)
	ফেরত;

    যদি (balloonEval->বার্তা == ফাঁকা)
	rettv->vval.v_string = ফাঁকা;
    অন্যথা
	rettv->vval.v_string = vim_strsave(balloonEval->বার্তা);
}

    স্থির শূন্য
f_balloon_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (balloonEval == ফাঁকা)
	ফেরত;

    যদি (in_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, ফাঁকা,
		l == ফাঁকা || l->lv_len == 0 ? ফাঁকা : l);
    }
    অন্যথা
    {
	char_u *mesg;

	যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    ফেরত;

	mesg = tv_get_string_chk(&argvars[0]);
	যদি (mesg != ফাঁকা)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? ফাঁকা : mesg, ফাঁকা);
    }
}

# যদি defined(FEAT_BEVAL_TERM)
    স্থির শূন্য
f_balloon_split(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (rettv_list_alloc(rettv) != OK)
	ফেরত;

    char_u *বার্তা;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;
    বার্তা = tv_get_string_chk(&argvars[0]);
    যদি (বার্তা != ফাঁকা)
    {
	pumitem_T	*array;
	পূর্ণসংখ্যা		আকার_2 = split_message(বার্তা, &array);

	// Skip the first and last item, they are always empty.
	জন্য (পূর্ণসংখ্যা i = 1; i < আকার_2 - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_text, -1);
	যখন (আকার_2 > 0)
	    vim_free(array[--আকার_2].pum_text);
	vim_free(array);
    }
}
# endif
#endif

/*
 * "base64_decode(string)" function
 */
    স্থির শূন্য
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    যদি (check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (rettv_blob_alloc(rettv) == FAIL)
	ফেরত;

    char_u *str = tv_get_string_chk(&argvars[0]);
    যদি (str != ফাঁকা)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    স্থির শূন্য
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    যদি (check_for_blob_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    blob_T *blob = argvars->vval.v_blob;
    যদি (blob != ফাঁকা)
	rettv->vval.v_string =
	    base64_encode(blob->bv_ga.ga_তথ্য, blob->bv_ga.ga_len);
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
    যদি (buf == ফাঁকা)
	semsg(_(e_invalid_বাফার_name_str), tv_get_string(arg));
    ফেরত buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    স্থির শূন্য
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_সংখ্যা = VVAL_TRUE;

    যদি (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	ফেরত;

    যদি (strcmp((ধ্রুবক অক্ষর *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    অন্যথা
    {
	যদি (bindtextdomain((ধ্রুবক অক্ষর *)argvars[0].vval.v_string, (ধ্রুবক অক্ষর *)argvars[1].vval.v_string) == ফাঁকা)
	{
	    do_outofmem_বার্তা((long)0);
	    rettv->vval.v_সংখ্যা = VVAL_FALSE;
	}
    }

    ফেরত;
}

/*
 * "byte2line(byte)" function
 */
    স্থির শূন্য
f_byte2line(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_সংখ্যা = -1;
#অন্যথা
    long	boff = 0;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    boff = tv_get_সংখ্যা(&argvars[0]) - 1;  // boff gets -1 on type error
    যদি (boff < 0)
	rettv->vval.v_সংখ্যা = -1;
    অন্যথা
	rettv->vval.v_সংখ্যা = ml_find_সারি_or_offset(curbuf,
							  (linenr_T)0, &boff);
#endif
}

/*
 * "call(func, arglist [, dict])" function
 */
    স্থির শূন্য
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = ফাঁকা;
    dict_T	*selfdict = ফাঁকা;
    char_u	*tofree = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ফেরত;

    যদি (check_for_list_arg(argvars, 1) == FAIL)
	ফেরত;
    যদি (argvars[1].vval.v_list == ফাঁকা)
	ফেরত;

    যদি (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    অন্যথা যদি (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    অন্যথা
	func = tv_get_string(&argvars[0]);
    যদি (func == ফাঁকা || *func == NUL)
	ফেরত;		// type error, empty name or null function

    যদি (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, ফাঁকা, FALSE, TFN_INT|TFN_QUIET);
	যদি (tofree == ফাঁকা)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    ফেরত;
	}
	যদি (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_args(tofree, STRLEN(tofree),
									&p);
	    যদি (s != ফাঁকা)
	    {
		vim_free(tofree);
		tofree = s;
	    }
	}
	func = tofree;
    }

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	যদি (check_for_dict_arg(argvars, 2) == FAIL)
	    যাও done;

	selfdict = argvars[2].vval.v_dict;
    }

    (শূন্য)func_call(func, &argvars[1], partial, selfdict, rettv);

done:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    স্থির শূন্য
f_changenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    স্থির শূন্য
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (has_mbyte)
    {
	পূর্ণসংখ্যা	utf8 = 0;

	যদি (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[1], ফাঁকা);

	যদি (utf8)
	    rettv->vval.v_সংখ্যা = utf_ptr2char(tv_get_string(&argvars[0]));
	অন্যথা
	    rettv->vval.v_সংখ্যা = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    অন্যথা
	rettv->vval.v_সংখ্যা = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    স্থির শূন্য
get_col(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা charcol)
{
    colnr_T	col = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    পূর্ণসংখ্যা		winchanged = FALSE;

    যদি (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL)
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[1]), &tp);
	যদি (wp == ফাঁকা || tp == ফাঁকা)
	    ফেরত;

	যদি (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    ফেরত;

	check_cursor();
	winchanged = TRUE;
    }

    পূর্ণসংখ্যা fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    যদি (fp != ফাঁকা && fnum == curbuf->b_fnum)
    {
	যদি (fp->col == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    যদি (fp->lnum <= curbuf->b_ml.ml_সারি_গুনতি)
		col = ml_get_len(fp->lnum) + 1;
	    অন্যথা
		col = MAXCOL;
	}
	অন্যথা
	{
	    col = fp->col + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    যদি (virtual_active() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		যদি (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    পূর্ণসংখ্যা		l;

		    যদি (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			col += l;
		}
	    }
	}
    }
    rettv->vval.v_সংখ্যা = col;

    যদি (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    স্থির শূন্য
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, TRUE);
}

    win_T *
get_optional_window(typval_T *argvars, পূর্ণসংখ্যা idx)
{
    win_T   *win = curwin;

    যদি (argvars[idx].v_type == VAR_UNKNOWN)
	ফেরত curwin;

    win = find_win_by_nr_or_id(&argvars[idx]);
    যদি (win == ফাঁকা)
    {
	emsg(_(e_invalid_window_সংখ্যা));
	ফেরত ফাঁকা;
    }
    ফেরত win;
}

/*
 * "col(string)" function
 */
    স্থির শূন্য
f_col(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    স্থির শূন্য
f_confirm(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*message;
    char_u	*buttons = ফাঁকা;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    পূর্ণসংখ্যা		def = 1;
    পূর্ণসংখ্যা		type = VIM_GENERIC;
    char_u	*typestr;
    পূর্ণসংখ্যা		error = FALSE;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	ফেরত;

    message = tv_get_string_chk(&argvars[0]);
    যদি (message == ফাঁকা)
	error = TRUE;
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	যদি (buttons == ফাঁকা)
	    error = TRUE;
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[2], &error);
	    যদি (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		যদি (typestr == ফাঁকা)
		    error = TRUE;
		অন্যথা
		{
		    নির্বাচন (TOUPPER_ASC(*typestr))
		    {
			ক্ষেত্র 'E': type = VIM_ERROR; বিরতি;
			ক্ষেত্র 'Q': type = VIM_QUESTION; বিরতি;
			ক্ষেত্র 'I': type = VIM_INFO; বিরতি;
			ক্ষেত্র 'W': type = VIM_WARNING; বিরতি;
			ক্ষেত্র 'G': type = VIM_GENERIC; বিরতি;
		    }
		}
	    }
	}
    }

    যদি (buttons == ফাঁকা || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    যদি (!error)
	rettv->vval.v_সংখ্যা = do_dialog(type, ফাঁকা, message, buttons,
							    def, ফাঁকা, FALSE);
#endif
}

/*
 * "copy()" function
 */
    স্থির শূন্য
f_copy(typval_T *argvars, typval_T *rettv)
{
    item_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    স্থির শূন্য
set_cursorpos(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা charcol)
{
    long	lnum, col;
    long	coladd = 0;
    পূর্ণসংখ্যা		set_curswant = TRUE;

    যদি (in_vim9script()
	    && (check_for_string_or_সংখ্যা_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL)))
	ফেরত;

    rettv->vval.v_সংখ্যা = -1;
    যদি (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	যদি (list2fpos(argvars, &pos, ফাঁকা, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_argument));
	    ফেরত;
	}
	lnum = pos.lnum;
	col = pos.col;
	coladd = pos.coladd;
	যদি (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    set_curswant = FALSE;
	}
    }
    অন্যথা যদি ((argvars[0].v_type == VAR_সংখ্যা
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_সংখ্যা
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	যদি (lnum < 0)
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
	অন্যথা যদি (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	col = (long)tv_get_সংখ্যা_chk(&argvars[1], ফাঁকা);
	যদি (charcol)
	    col = buf_charidx_to_byteidx(curbuf, lnum, col) + 1;
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (long)tv_get_সংখ্যা_chk(&argvars[2], ফাঁকা);
    }
    অন্যথা
    {
	emsg(_(e_invalid_argument));
	ফেরত;
    }
    যদি (lnum < 0 || col < 0 || coladd < 0)
	ফেরত;		// type error; errmsg already given
    যদি (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    যদি (col != MAXCOL && --col < 0)
	col = 0;
    curwin->w_cursor.col = col;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    যদি (has_mbyte)
	mb_adjust_cursor();

    curwin->w_set_curswant = set_curswant;
    rettv->vval.v_সংখ্যা = 0;
}

/*
 * "cursor(lnum, col)" function, or
 * "cursor(list)"
 *
 * Moves the cursor to the specified line and column.
 * Returns 0 when the position could be set, -1 otherwise.
 */
    স্থির শূন্য
f_cursor(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, FALSE);
}

#ifdef MSWIN
/*
 * "debugbreak()" function
 */
    স্থির শূন্য
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		pid;

    rettv->vval.v_সংখ্যা = FAIL;
    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    pid = (পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[0]);
    যদি (pid == 0)
    {
	emsg(_(e_invalid_argument));
	ফেরত;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    যদি (hProcess == ফাঁকা)
	ফেরত;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_সংখ্যা = OK;
}
#endif

/*
 * "deepcopy()" function
 */
    স্থির শূন্য
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    যদি (check_for_opt_bool_arg(argvars, 1) == FAIL)
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], ফাঁকা);

    item_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    স্থির শূন্য
f_did_filetype(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->vval.v_সংখ্যা = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    স্থির শূন্য
f_echoraw(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *str;

    যদি (check_secure())
	ফেরত;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    str = tv_get_string_chk(&argvars[0]);
    যদি (str != ফাঁকা && *str != NUL)
    {
	out_str(str);
	out_flush();
    }
}

/*
 * "empty({expr})" function
 */
    স্থির শূন্য
f_empty(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		n = FALSE;

    নির্বাচন (argvars[0].v_type)
    {
	ক্ষেত্র VAR_STRING:
	ক্ষেত্র VAR_FUNC:
	    n = argvars[0].vval.v_string == ফাঁকা
					  || *argvars[0].vval.v_string == NUL;
	    বিরতি;
	ক্ষেত্র VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_সংখ্যা:
	    n = argvars[0].vval.v_সংখ্যা == 0;
	    বিরতি;
	ক্ষেত্র VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    বিরতি;
	ক্ষেত্র VAR_LIST:
	    n = argvars[0].vval.v_list == ফাঁকা
					|| argvars[0].vval.v_list->lv_len == 0;
	    বিরতি;
	ক্ষেত্র VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    বিরতি;
	ক্ষেত্র VAR_DICT:
	    n = argvars[0].vval.v_dict == ফাঁকা
			|| argvars[0].vval.v_dict->dv_hashtab.ht_used == 0;
	    বিরতি;
	ক্ষেত্র VAR_BOOL:
	ক্ষেত্র VAR_SPECIAL:
	    n = argvars[0].vval.v_সংখ্যা != VVAL_TRUE;
	    বিরতি;
	ক্ষেত্র VAR_CLASS:
	    n = argvars[0].vval.v_class != ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_OBJECT:
	    n = object_empty(argvars[0].vval.v_object);
	    বিরতি;

	ক্ষেত্র VAR_BLOB:
	    n = argvars[0].vval.v_blob == ফাঁকা
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    বিরতি;

	ক্ষেত্র VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_job == ফাঁকা
			   || argvars[0].vval.v_job->jv_status != JOB_STARTED;
	    বিরতি;
#endif
	ক্ষেত্র VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_channel == ফাঁকা
			       || !channel_is_open(argvars[0].vval.v_channel);
	    বিরতি;
#endif
	ক্ষেত্র VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == ফাঁকা
		|| argvars[0].vval.v_typealias->ta_name == ফাঁকা
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    বিরতি;

	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_VOID:
	ক্ষেত্র VAR_INSTR:
	    internal_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    বিরতি;
    }

    rettv->vval.v_সংখ্যা = n;
}

/*
 * "environ()" function
 */
    স্থির শূন্য
f_environ(typval_T *argvars UNUSED, typval_T *rettv)
{
#যদি !defined(AMIGA)
    পূর্ণসংখ্যা			i = 0;
    char_u		*entry, *মান;
# যদি defined (MSWIN)
#  যদি !defined(_UCRT)
    বাহ্যিক wchar_t	**_wenviron;
#  endif
# অন্যথা
    বাহ্যিক অক্ষর		**environ;
# endif

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;

# ifdef MSWIN
    যদি (*_wenviron == ফাঁকা)
	ফেরত;
# অন্যথা
    যদি (*environ == ফাঁকা)
	ফেরত;
# endif

    জন্য (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	যদি ((p = (short_u *)_wenviron[i]) == ফাঁকা)
	    ফেরত;
	entry = utf16_to_enc(p, ফাঁকা);
# অন্যথা
	যদি ((entry = (char_u *)environ[i]) == ফাঁকা)
	    ফেরত;
	entry = vim_strsave(entry);
# endif
	যদি (entry == ফাঁকা) // out of memory
	    ফেরত;
	যদি ((মান = vim_strchr(entry, '=')) == ফাঁকা)
	{
	    vim_free(entry);
	    চালু;
	}
	*মান++ = NUL;
	dict_add_string(rettv->vval.v_dict, (অক্ষর *)entry, মান);
	vim_free(entry);
    }
#endif
}

/*
 * "err_teapot()" function
 */
    স্থির শূন্য
f_err_teapot(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	যদি (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    যদি (*skipwhite(s) == NUL)
		ফেরত;
	}

	পূর্ণসংখ্যা err = FALSE;
	পূর্ণসংখ্যা do_503 = eval_রাশিমালা_to_bool(&argvars[0], &err);
	যদি (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_available));
	    ফেরত;
	}
    }

    emsg(_(e_im_a_teapot));
}

/*
 * "escape({string}, {chars})" function
 */
    স্থির শূন্য
f_escape(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    rettv->vval.v_string = vim_strsave_escaped(tv_get_string(&argvars[0]),
					 tv_get_string_buf(&argvars[1], buf));
    rettv->v_type = VAR_STRING;
}

/*
 * "eval()" function
 */
    স্থির শূন্য
f_eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*s, *p;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    s = tv_get_string_chk(&argvars[0]);
    যদি (s != ফাঁকা)
	s = skipwhite(s);

    p = s;
    যদি (s == ফাঁকা || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	যদি (p != ফাঁকা && !aborting())
	    semsg(_(e_invalid_expression_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_সংখ্যা;
	rettv->vval.v_সংখ্যা = 0;
    }
    অন্যথা যদি (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    স্থির শূন্য
f_eventhandler(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = vgetc_busy || প্রবেশ_busy;
}

স্থির garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    শূন্য
execute_redir_str(char_u *মান, পূর্ণসংখ্যা মান_len)
{
    পূর্ণসংখ্যা		len;

    যদি (মান_len == -1)
	len = (পূর্ণসংখ্যা)STRLEN(মান);	// Append the entire string
    অন্যথা
	len = মান_len;		// Append only "value_len" characters
    যদি (ga_grow(&redir_execute_ga, len) == FAIL)
	ফেরত;

    mch_memmove((অক্ষর *)redir_execute_ga.ga_তথ্য
	    + redir_execute_ga.ga_len, মান, len);
    redir_execute_ga.ga_len += len;
}

#যদি defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    স্থির char_u *
get_str_সারি(
    পূর্ণসংখ্যা	    c UNUSED,
    শূন্য    *cookie,
    পূর্ণসংখ্যা	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    char_u	*start = *(char_u **)cookie;
    char_u	*সারি;
    char_u	*p;

    p = start;
    যদি (p == ফাঁকা || *p == NUL)
	ফেরত ফাঁকা;
    p = vim_strchr(p, '\n');
    যদি (p == ফাঁকা)
	সারি = vim_strsave(start);
    অন্যথা
    {
	সারি = vim_strnsave(start, p - start);
	p++;
    }

    *(char_u **)cookie = p;
    ফেরত সারি;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    শূন্য
execute_cmds_from_string(char_u *str)
{
    do_cmdline(ফাঁকা, get_str_সারি, (শূন্য *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#endif

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_সারি(
    পূর্ণসংখ্যা	    c UNUSED,
    শূন্য    *cookie,
    পূর্ণসংখ্যা	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *item = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    যদি (item == ফাঁকা)
	ফেরত ফাঁকা;
    s = tv_get_string_buf_chk(&item->li_tv, buf);
    *p = item->li_পরবর্তী;
    ফেরত s == ফাঁকা ? ফাঁকা : vim_strsave(s);
}

/*
 * "execute()" function
 */
    শূন্য
execute_common(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা arg_off)
{
    char_u	*cmd = ফাঁকা;
    list_T	*list = ফাঁকা;
    পূর্ণসংখ্যা		save_বার্তা_silent = বার্তা_silent;
    পূর্ণসংখ্যা		save_emsg_silent = emsg_silent;
    পূর্ণসংখ্যা		save_emsg_noredir = emsg_noredir;
    পূর্ণসংখ্যা		save_redir_execute = redir_execute;
    পূর্ণসংখ্যা		save_redir_off = redir_off;
    garray_T	save_ga;
    পূর্ণসংখ্যা		save_বার্তা_col = বার্তা_col;
    পূর্ণসংখ্যা		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    পূর্ণসংখ্যা		echo_output = FALSE;

    rettv->vval.v_string = ফাঁকা;
    rettv->v_type = VAR_STRING;

    যদি (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	যদি (list == ফাঁকা || list->lv_len == 0)
	    // empty list, no commands, empty output
	    ফেরত;
	++list->lv_refcount;
    }
    অন্যথা যদি (argvars[arg_off].v_type == VAR_JOB
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_মান_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	ফেরত;
    }
    অন্যথা
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	যদি (cmd == ফাঁকা)
	    ফেরত;
    }

    যদি (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      in_vim9script());

	যদি (s == ফাঁকা)
	    ফেরত;
	যদি (*s == NUL)
	    echo_output = TRUE;
	যদি (STRNCMP(s, "silent", 6) == 0)
	    ++বার্তা_silent;
	যদি (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    অন্যথা
	++বার্তা_silent;

    যদি (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, আকার(অক্ষর), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    যদি (!echo_output)
	বার্তা_col = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    যদি (cmd != ফাঁকা)
	do_cmdline_cmd(cmd);
    অন্যথা
    {
	listitem_T	*item;

	CHECK_LIST_MATERIALIZE(list);
	item = list->lv_first;
	do_cmdline(ফাঁকা, get_list_সারি, (শূন্য *)&item,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    যদি (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((অক্ষর *)redir_execute_ga.ga_তথ্য)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_তথ্য;
    }
    অন্যথা
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = ফাঁকা;
    }
    বার্তা_silent = save_বার্তা_silent;
    emsg_silent = save_emsg_silent;
    emsg_noredir = save_emsg_noredir;

    redir_execute = save_redir_execute;
    যদি (redir_execute)
	redir_execute_ga = save_ga;
    redir_off = save_redir_off;

    // "silent reg" or "silent echo x" leaves msg_col somewhere in the line.
    যদি (echo_output)
	// When not working silently: put it in column zero.  A following
	// "echon" will overwrite the message, unavoidably.
	বার্তা_col = 0;
    অন্যথা
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	বার্তা_col = save_বার্তা_col;
}

/*
 * "execute()" function
 */
    স্থির শূন্য
f_execute(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ফেরত;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    শূন্য
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    পূর্ণসংখ্যা		n = FALSE;

    যদি (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ফেরত;

    p = tv_get_string(&argvars[0]);
    যদি (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	যদি (mch_getenv(p + 1) != ফাঁকা)
	    n = TRUE;
	অন্যথা
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    যদি (p != ফাঁকা && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    অন্যথা যদি (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, ফাঁকা, TRUE) == OK);
	যদি (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    অন্যথা যদি (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    অন্যথা যদি (*p == '?')			// internal function only
    {
	n = has_internal_func_name(p + 1);
    }
    অন্যথা যদি (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    অন্যথা যদি (*p == '#')
    {
	যদি (p[1] == '#')
	    n = autocmd_supported(p + 2);
	অন্যথা
	    n = au_exists(p + 1);
    }
    অন্যথা				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_সংখ্যা = n;
}

    স্থির শূন্য
f_exists_compiled(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    emsg(_(e_exists_compiled_can_only_be_used_in_def_function));
}

/*
 * "expand()" function
 */
    স্থির শূন্য
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    size_t	len;
    পূর্ণসংখ্যা		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    পূর্ণসংখ্যা		error = FALSE;
    char_u	*ফলাফল;
#ifdef BACKSLASH_IN_FILENAME
    char_u	*p_csl_save = p_csl;
#endif

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ফেরত;

#ifdef BACKSLASH_IN_FILENAME
    // avoid using 'completeslash' here
    p_csl = empty_option;
#endif

    rettv->v_type = VAR_STRING;
    যদি (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_set(rettv, ফাঁকা);

    s = tv_get_string(&argvars[0]);
    যদি (*s == '%' || *s == '#' || *s == '<')
    {
	অক্ষর	*errormsg = ফাঁকা;

	যদি (p_verbose == 0)
	    ++emsg_off;
	ফলাফল = eval_vars(s, s, &len, ফাঁকা, &errormsg, ফাঁকা, FALSE);
	যদি (p_verbose == 0)
	    --emsg_off;
	অন্যথা যদি (errormsg != ফাঁকা)
	    emsg(errormsg);
	যদি (rettv->v_type == VAR_LIST)
	{
	    যদি (rettv_list_alloc(rettv) == OK && ফলাফল != ফাঁকা)
		list_append_string(rettv->vval.v_list, ফলাফল, -1);
	    vim_free(ফলাফল);
	}
	অন্যথা
	    rettv->vval.v_string = ফলাফল;
    }
    অন্যথা
    {
	// When the optional second argument is non-zero, don't remove matches
	// for 'wildignore' and don't put matches for 'suffixes' at the end.
	যদি (argvars[1].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	যদি (!error)
	{
	    ExpandInit(&xpc);
	    xpc.xp_context = EXPAND_FILES;
	    যদি (p_wic)
		options += WILD_ICASE;
	    যদি (rettv->v_type == VAR_STRING)
		rettv->vval.v_string = ExpandOne(&xpc, s, ফাঁকা,
							   options, WILD_ALL);
	    অন্যথা যদি (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, ফাঁকা, options, WILD_ALL_KEEP);
		জন্য (পূর্ণসংখ্যা i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	অন্যথা
	    rettv->vval.v_string = ফাঁকা;
    }
#ifdef BACKSLASH_IN_FILENAME
    p_csl = p_csl_save;
#endif
}

/*
 * "expandcmd()" function
 * Expand all the special characters in a command string.
 */
    স্থির শূন্য
f_expandcmd(typval_T *argvars, typval_T *rettv)
{
    exarg_T	eap;
    char_u	*cmdstr;
    অক্ষর	*errormsg = ফাঁকা;
    পূর্ণসংখ্যা		emsgoff = TRUE;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_FIELD(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = ফাঁকা;
    eap.cmdidx = CMD_USER;

    যদি (emsgoff)
	++emsg_off;
    যদি (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	যদি (!emsgoff && errormsg != ফাঁকা && *errormsg != NUL)
	    emsg(errormsg);
    যদি (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    স্থির শূন্য
f_feedkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    পূর্ণসংখ্যা		remap = TRUE;
    পূর্ণসংখ্যা		ঢোকাও = FALSE;
    char_u	*চাবিগুলি, *flags;
    char_u	nbuf[NUMBUFLEN];
    পূর্ণসংখ্যা		typed = FALSE;
    পূর্ণসংখ্যা		execute = FALSE;
    পূর্ণসংখ্যা		context = FALSE;
    পূর্ণসংখ্যা		dangerous = FALSE;
    পূর্ণসংখ্যা		lowlevel = FALSE;

    // This is not allowed in the sandbox.  If the commands would still be
    // executed in the sandbox it would be OK, but it probably happens later,
    // when "sandbox" is no longer set.
    যদি (check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ফেরত;

    চাবিগুলি = tv_get_string(&argvars[0]);

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	জন্য ( ; *flags != NUL; ++flags)
	{
	    নির্বাচন (*flags)
	    {
		ক্ষেত্র 'n': remap = FALSE; বিরতি;
		ক্ষেত্র 'm': remap = TRUE; বিরতি;
		ক্ষেত্র 't': typed = TRUE; বিরতি;
		ক্ষেত্র 'i': ঢোকাও = TRUE; বিরতি;
		ক্ষেত্র 'x': execute = TRUE; বিরতি;
		ক্ষেত্র 'c': context = TRUE; বিরতি;
		ক্ষেত্র '!': dangerous = TRUE; বিরতি;
		ক্ষেত্র 'L': lowlevel = TRUE; বিরতি;
	    }
	}
    }

    যদি (*চাবিগুলি != NUL || execute)
    {
	যদি (lowlevel
#ifdef FEAT_VTP
		&& (!is_term_win32()
		    || (চাবিগুলি[0] == 3 && ctrl_c_interrupts && typed))
#endif
	   )
	{
#ifdef USE_প্রবেশ_BUF
	    ch_log(ফাঁকা, "feedkeys() lowlevel: %s", চাবিগুলি);

	    পূর্ণসংখ্যা len = (পূর্ণসংখ্যা)STRLEN(চাবিগুলি);
	    জন্য (পূর্ণসংখ্যা idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		যদি (চাবিগুলি[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		add_to_প্রবেশ_buf(চাবিগুলি + idx, 1);
	    }
#অন্যথা
	    emsg(_(e_lowlevel_প্রবেশ_not_supported));
#endif
	}
	অন্যথা
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *চাবিগুলি_esc = vim_strsave_escape_csi(চাবিগুলি);
	    যদি (চাবিগুলি_esc == ফাঁকা)
		ফেরত;

	    ch_log(ফাঁকা, "feedkeys(%s): %s", typed ? "typed" : "", চাবিগুলি);

	    ins_typebuf(চাবিগুলি_esc, (remap ? REMAP_YES : REMAP_NONE),
				   ঢোকাও ? 0 : typebuf.tb_len, !typed, FALSE);
	    যদি (vgetc_busy
#ifdef FEAT_TIMERS
		    || timer_busy
#endif
		    || প্রবেশ_busy)
		typebuf_was_filled = TRUE;

	    vim_free(চাবিগুলি_esc);
	}

	যদি (execute)
	{
	    পূর্ণসংখ্যা		save_বার্তা_scroll = বার্তা_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    বার্তা_scroll = FALSE;

	    ch_log(ফাঁকা, "feedkeys() executing");

	    যদি (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_version = 0;
	    }

	    যদি (!dangerous)
	    {
		++ex_normal_busy;
		++in_feedkeys;
	    }
	    ++allow_osc_চাবি;
	    exec_normal(TRUE, lowlevel, TRUE);
	    --allow_osc_চাবি;
	    যদি (!dangerous)
	    {
		--ex_normal_busy;
		--in_feedkeys;
	    }

	    বার্তা_scroll |= save_বার্তা_scroll;

	    যদি (context)
		current_sctx = save_sctx;
	}
    }
}

/*
 * "fnameescape({string})" function
 */
    স্থির শূন্য
f_fnameescape(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    স্থির শূন্য
f_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	gui_mch_set_foreground();
	ফেরত;
    }
#endif
#যদি defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_set_foreground();
#endif
}

/*
 * "function()" function
 * "funcref()" function
 */
    স্থির শূন্য
common_function(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা is_funcref)
{
    char_u	*s;
    char_u	*name;
    পূর্ণসংখ্যা		use_string = FALSE;
    partial_T   *arg_pt = ফাঁকা;
    char_u	*trans_name = ফাঁকা;
    পূর্ণসংখ্যা		is_global = FALSE;
    char_u	*start_bracket = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ফেরত;

    যদি (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    অন্যথা যদি (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != ফাঁকা)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    অন্যথা
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    যদি (s == ফাঁকা)
    {
	semsg(_(e_invalid_argument_str), "NULL");
	ফেরত;
    }

    যদি ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == ফাঁকা) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, ফাঁকা);
	যদি (*name == '<')
	{
	    // generic function
	    start_bracket = name;
	    যদি (skip_generic_func_type_args(&name) == FAIL)
		যাও theend;
	}
	যদি (*name != NUL)
	    s = ফাঁকা;
    }

    যদি (s == ফাঁকা || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == ফাঁকা))
	semsg(_(e_invalid_argument_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    অন্যথা যদি (trans_name != ফাঁকা && (is_funcref
			 ? find_func(trans_name, is_global) == ফাঁকা
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    অন্যথা
    {
	পূর্ণসংখ্যা	dict_idx = 0;
	পূর্ণসংখ্যা	arg_idx = 0;
	list_T	*list = ফাঁকা;

	যদি (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	অন্যথা যদি (trans_name != ফাঁকা && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	অন্যথা
	    name = vim_strsave(s);

	যদি (argvars[1].v_type != VAR_UNKNOWN)
	{
	    যদি (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    অন্যথা যদি (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    অন্যথা
		// function(name, [args])
		arg_idx = 1;
	    যদি (dict_idx > 0)
	    {
		যদি (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    যাও theend;
		}
		যদি (argvars[dict_idx].vval.v_dict == ফাঁকা)
		    dict_idx = 0;
	    }
	    যদি (arg_idx > 0)
	    {
		যদি (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_second_argument_of_function_must_be_list_or_dict));
		    vim_free(name);
		    যাও theend;
		}
		list = argvars[arg_idx].vval.v_list;
		যদি (list == ফাঁকা || list->lv_len == 0)
		    arg_idx = 0;
		অন্যথা যদি (list->lv_len > MAX_FUNC_ARGS)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    যাও theend;
		}
	    }
	}
	যদি (dict_idx > 0 || arg_idx > 0 || arg_pt != ফাঁকা || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    যদি (pt == ফাঁকা)
		vim_free(name);
	    অন্যথা
	    {
		যদি (arg_idx > 0 || (arg_pt != ফাঁকা && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    পূর্ণসংখ্যা		i = 0;
		    পূর্ণসংখ্যা		arg_len = 0;
		    পূর্ণসংখ্যা		lv_len = 0;

		    যদি (arg_pt != ফাঁকা)
			arg_len = arg_pt->pt_argc;
		    যদি (list != ফাঁকা)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    যদি (pt->pt_argv == ফাঁকা)
		    {
			vim_free(pt);
			vim_free(name);
			যাও theend;
		    }
		    জন্য (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    যদি (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_বস্তুসমূহ(list, li)
			    copy_tv(&li->li_tv, &pt->pt_argv[i++]);
		    }
		}

		// For "function(dict.func, [], dict)" and "func" is a partial
		// use "dict".  That is backwards compatible.
		যদি (dict_idx > 0)
		{
		    // The dict is bound explicitly, pt_auto is FALSE.
		    pt->pt_dict = argvars[dict_idx].vval.v_dict;
		    ++pt->pt_dict->dv_refcount;
		}
		অন্যথা যদি (arg_pt != ফাঁকা)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    যদি (pt->pt_dict != ফাঁকা)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    যদি (pt->pt_obj != ফাঁকা)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		যদি (arg_pt != ফাঁকা && arg_pt->pt_func != ফাঁকা)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		অন্যথা যদি (is_funcref)
		{
		    pt->pt_func = find_func(trans_name, is_global);
		    যদি (IS_GENERIC_FUNC(pt->pt_func) && start_bracket != ফাঁকা)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&start_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		অন্যথা
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		যদি (arg_pt != ফাঁকা)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	অন্যথা
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    যদি (start_bracket == ফাঁকা)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    অন্যথা
	    {
		// generic function
		size_t len = STRLEN(name) + STRLEN(start_bracket);
		rettv->vval.v_string = alloc(len + 1);
		যদি (rettv->vval.v_string != ফাঁকা)
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
    স্থির শূন্য
f_funcref(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, TRUE);
}

/*
 * "function()" function
 */
    স্থির শূন্য
f_function(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, FALSE);
}

/*
 * "garbagecollect()" function
 */
    স্থির শূন্য
f_garbagecollect(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ফেরত;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    যদি (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
	garbage_collect_at_exit = TRUE;
}

/*
 * "get()" function
 */
    স্থির শূন্য
f_get(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    list_T	*l;
    tuple_T	*tuple;
    dictitem_T	*di;
    dict_T	*d;
    typval_T	*tv = ফাঁকা;
    পূর্ণসংখ্যা		what_is_dict = FALSE;

    যদি (argvars[0].v_type == VAR_BLOB)
    {
	পূর্ণসংখ্যা error = FALSE;
	পূর্ণসংখ্যা idx = tv_get_সংখ্যা_chk(&argvars[1], &error);

	যদি (!error)
	{
	    rettv->v_type = VAR_সংখ্যা;
	    যদি (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    যদি (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_সংখ্যা = -1;
	    অন্যথা
	    {
		rettv->vval.v_সংখ্যা = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    অন্যথা যদি (argvars[0].v_type == VAR_LIST)
    {
	যদি ((l = argvars[0].vval.v_list) != ফাঁকা)
	{
	    পূর্ণসংখ্যা		error = FALSE;

	    li = list_find(l, (long)tv_get_সংখ্যা_chk(&argvars[1], &error));
	    যদি (!error && li != ফাঁকা)
		tv = &li->li_tv;
	}
    }
    অন্যথা যদি (argvars[0].v_type == VAR_TUPLE)
    {
	যদি ((tuple = argvars[0].vval.v_tuple) != ফাঁকা)
	{
	    পূর্ণসংখ্যা		error = FALSE;
	    long	idx;

	    idx = (long)tv_get_সংখ্যা_chk(&argvars[1], &error);
	    যদি (!error)
		tv = tuple_find(tuple, idx);
	}
    }
    অন্যথা যদি (argvars[0].v_type == VAR_DICT)
    {
	যদি ((d = argvars[0].vval.v_dict) != ফাঁকা)
	{
	    di = dict_find(d, tv_get_string(&argvars[1]), -1);
	    যদি (di != ফাঁকা)
		tv = &di->di_tv;
	}
    }
    অন্যথা যদি (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	যদি (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	অন্যথা
	{
	    CLEAR_FIELD(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	যদি (pt != ফাঁকা)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    যদি (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		যদি (name == ফাঁকা)
		    rettv->vval.v_string = ফাঁকা;
		অন্যথা
		{
		    যদি (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    যদি (*what == 'n' && pt->pt_name == ফাঁকা
							&& pt->pt_func != ফাঁকা)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    অন্যথা যদি (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		যদি (pt->pt_dict != ফাঁকা)
		    rettv_dict_set(rettv, pt->pt_dict);
	    }
	    অন্যথা যদি (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		যদি (rettv_list_alloc(rettv) == OK)
		{
		    পূর্ণসংখ্যা i;

		    জন্য (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    অন্যথা যদি (STRCMP(what, "arity") == 0)
	    {
		পূর্ণসংখ্যা required = 0, optional = 0, varargs = FALSE;
		char_u *name = partial_name(pt);

		get_func_arity(name, &required, &optional, &varargs);

		rettv->v_type = VAR_DICT;
		যদি (rettv_dict_alloc(rettv) == OK)
		{
		    dict_T *dict = rettv->vval.v_dict;

		    // Take into account the arguments of the partial, if any.
		    // Note that it is possible to supply more arguments than the function
		    // accepts.
		    যদি (pt->pt_argc >= required + optional)
			required = optional = 0;
		    অন্যথা যদি (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    অন্যথা
			required -= pt->pt_argc;

		    dict_add_সংখ্যা(dict, "required", required);
		    dict_add_সংখ্যা(dict, "optional", optional);
		    dict_add_bool(dict, "varargs", varargs);
		}
	    }
	    অন্যথা
		semsg(_(e_invalid_argument_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    যদি (!what_is_dict)
		ফেরত;
	}
    }
    অন্যথা
	semsg(_(e_argument_of_str_must_be_list_tuple_dictionary_or_blob), "get()");

    যদি (tv == ফাঁকা)
    {
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    অন্যথা
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
    স্থির শূন্য
f_getbgcolor(typval_T *argvars UNUSED, typval_T *rettv)
{
    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	যদি (gui.in_use)
	    try_hl = true;
# endif
# ifdef FEAT_TERMGUICOLORS
	যদি (p_tgc)
	    try_hl = true;
# endif
	যদি (try_hl)
	{
	    guicolor_T	fg, bg;
	    পূর্ণসংখ্যা		hl_id = syn_name2id((char_u *)"Normal");

	    যদি (hl_id > 0)
	    {
		(শূন্য)syn_id2colors(hl_id, &fg, &bg);
		যদি (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_সংখ্যা(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_সংখ্যা(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_সংখ্যা(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    ফেরত;
		}
	    }
	}
    }
#endif
#ifdef FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	যদি (term_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)r);
	    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)g);
	    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#endif
}

/*
 * "getcellpixels()" function
 */
    স্থির শূন্য
f_getcellpixels(typval_T *argvars UNUSED, typval_T *rettv)
{
    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

#যদি defined(FEAT_GUI)
    যদি (gui.in_use)
    {
	// success pixel size and no gui.
	list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)gui.char_width);
	list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)gui.char_height);
    }
    অন্যথা
#endif
    {
#যদি defined(UNIX) || defined(MSWIN)
	list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)cell_width);
	list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)cell_height);
#অন্যথা
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)-1);
	list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)-1);
#endif
    }

}

/*
 * "getchangelist()" function
 */
    স্থির শূন্য
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    পূর্ণসংখ্যা		i;
    list_T	*l;
    dict_T	*d;
    পূর্ণসংখ্যা		changelistindex;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script() && check_for_opt_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    অন্যথা
	buf = tv_get_buf_from_arg(&argvars[0]);
    যদি (buf == ফাঁকা)
	ফেরত;

    l = list_alloc();
    যদি (l == ফাঁকা)
	ফেরত;
    যদি (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	ফেরত;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    যদি (buf == curwin->w_বাফার)
    {
	changelistindex = curwin->w_changelistidx;
    }
    অন্যথা
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    যদি (wip->wi_win == curwin)
		বিরতি;
	changelistindex = wip != ফাঁকা ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)changelistindex);

    জন্য (i = 0; i < buf->b_changelistlen; ++i)
    {
	যদি (buf->b_changelist[i].lnum == 0)
	    চালু;
	যদি ((d = dict_alloc()) == ফাঁকা)
	    ফেরত;
	যদি (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ফেরত;
	}
	dict_add_সংখ্যা(d, "lnum", (long)buf->b_changelist[i].lnum);
	dict_add_সংখ্যা(d, "col", (long)buf->b_changelist[i].col);
	dict_add_সংখ্যা(d, "coladd", (long)buf->b_changelist[i].coladd);
    }
}

    স্থির শূন্য
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    পূর্ণসংখ্যা		getcurpos,
    পূর্ণসংখ্যা		charcol)
{
    pos_T	*fp = ফাঁকা;
    pos_T	pos;
    win_T	*wp = curwin;
    list_T	*l;
    পূর্ণসংখ্যা		fnum = -1;

    যদি (rettv_list_alloc(rettv) == OK)
    {
	l = rettv->vval.v_list;
	যদি (getcurpos)
	{
	    যদি (argvars[0].v_type != VAR_UNKNOWN)
	    {
		wp = find_win_by_nr_or_id(&argvars[0]);
		যদি (wp != ফাঁকা)
		    fp = &wp->w_cursor;
	    }
	    অন্যথা
		fp = &curwin->w_cursor;
	    যদি (fp != ফাঁকা && charcol)
	    {
		pos = *fp;
		pos.col =
		    buf_byteidx_to_charidx(wp->w_বাফার, pos.lnum, pos.col);
		fp = &pos;
	    }
	}
	অন্যথা
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	যদি (fnum != -1)
	    list_append_সংখ্যা(l, (varnumber_T)fnum);
	অন্যথা
	    list_append_সংখ্যা(l, (varnumber_T)0);
	list_append_সংখ্যা(l, (fp != ফাঁকা) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_সংখ্যা(l, (fp != ফাঁকা)
		     ? (varnumber_T)(fp->col == MAXCOL ? MAXCOL : fp->col + 1)
							    : (varnumber_T)0);
	list_append_সংখ্যা(l, (fp != ফাঁকা) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	যদি (getcurpos)
	{
	    পূর্ণসংখ্যা	    save_set_curswant = curwin->w_set_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    যদি (wp == curwin)
		update_curswant();
	    list_append_সংখ্যা(l, wp == ফাঁকা ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    যদি (wp == curwin && save_set_curswant)
	    {
		curwin->w_set_curswant = save_set_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    অন্যথা
	rettv->vval.v_সংখ্যা = FALSE;
}

/*
 * "getcharpos()" function
 */
    স্থির শূন্য
f_getcharpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    স্থির শূন্য
f_getcharsearch(typval_T *argvars UNUSED, typval_T *rettv)
{
    যদি (rettv_dict_alloc(rettv) != OK)
	ফেরত;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_string(dict, "char", last_csearch());
    dict_add_সংখ্যা(dict, "forward", last_csearch_forward());
    dict_add_সংখ্যা(dict, "until", last_csearch_until());
}

/*
 * "getenv()" function
 */
    স্থির শূন্য
f_getenv(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা	    mustfree = FALSE;
    char_u  *p;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    যদি (p == ফাঁকা)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_সংখ্যা = VVAL_NULL;
	ফেরত;
    }
    যদি (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    স্থির শূন্য
f_getfontname(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ফেরত;

#ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	GuiFont font;
	char_u	*name = ফাঁকা;

	যদি (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	অন্যথা
	{
	    name = tv_get_string(&argvars[0]);
	    যদি (STRCMP(name, "*") == 0)	    // don't use font dialog
		ফেরত;
	    font = gui_mch_get_font(name, FALSE);
	    যদি (font == NOFONT)
		ফেরত;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	যদি (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#endif
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
স্থির কাঠামো
{
    argcheck_T	func;
    অক্ষর	*types;	    // separated by commas
} argcheck_types[] =
{
    {arg_any,			"any"},
    {arg_blob,			"blob"},
    {arg_bool,			"bool,number"},
    {arg_bool_or_dict_any,	"bool,number,dict<any>"},
    {arg_bool_or_nr,		"bool,number"},
    {arg_বাফার,		"string,number"},
    {arg_বাফার_or_dict_any,	"string,number,dict<any>"},
    {arg_chan_or_job,		"channel,job"},
    {arg_cursor1,		"number,string,list<any>"},
    {arg_dict_any,		"dict<any>"},
    {arg_dict_any_or_string,	"dict<any>,string"},
    {arg_extend3,		"number,string"},
    {arg_filter_func,		"string,func"},
    {arg_float_or_nr,		"float,number"},
    {arg_foreach_func,		"string,func"},
    {arg_get1,			"blob,list<any>,tuple<any>,dict<any>,func"},
    {arg_item_of_পূর্ব,		"any"},
    {arg_job,			"job"},
    {arg_len1,
	    "string,number,blob,list<any>,tuple<any>,dict<any>,object<any>"},
    {arg_list_any,		"list<any>"},
    {arg_list_any_mod,		"list<any>"},
    {arg_list_সংখ্যা,		"list<number>"},
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
    {arg_সংখ্যা,		"number"},
    {arg_object,		"object<any>"},
    {arg_remove2,		"number,string"},
    {arg_repeat1,		"string,number,blob,list<any>,tuple<any>"},
    {arg_reverse,		"list<any>,tuple<any>,blob,string"},
    {arg_same_as_পূর্ব,		"any"},
    {arg_same_struct_as_পূর্ব,	"any"},
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
    স্থির list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    অক্ষর	*types = "any";

    যদি (l == ফাঁকা)
	ফেরত ফাঁকা;
    জন্য (পূর্ণসংখ্যা i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(argcheck_types); ++i)
	যদি (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    বিরতি;
	}
    জন্য (অক্ষর *p = types; ; )
    {
	অক্ষর	*end = (অক্ষর *)vim_strchr((char_u *)p, ',');
	পূর্ণসংখ্যা	len = end == ফাঁকা ? (পূর্ণসংখ্যা)STRLEN(p) : (পূর্ণসংখ্যা)(end - p);

	যদি (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    ফেরত ফাঁকা;
	}
	যদি (end == ফাঁকা)
	    বিরতি;
	p = end + 1;
    }
    ফেরত l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    স্থির পূর্ণসংখ্যা
getinfo_argtypes(
	list_T			*l,
	ধ্রুবক funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    পূর্ণসংখ্যা		argcount = 0;

    যদি (l != ফাঁকা)
	FOR_ALL_LIST_বস্তুসমূহ(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    যদি (s == ফাঁকা)
		ফেরত -1;
	    যদি (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_ARGS)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		ফেরত -1;
	    }
	    type = parse_type(&p, type_gap, ফাঁকা, ফাঁকা, TRUE);
	    যদি (type == ফাঁকা)
		ফেরত -1;
	    যদি (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		ফেরত -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    জন্য ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_ARGS; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    ফেরত argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    স্থির অক্ষর *
builtin_ret_type_name(
	পূর্ণসংখ্যা	    idx,
	পূর্ণসংখ্যা	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	অক্ষর	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = internal_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    ফেরত type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
প্রকারলেখ গণনা {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_kind_T;

স্থির কাঠামো
{
    অক্ষর		*name;
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
    স্থির শূন্য
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_kind_T	kind;
    পূর্ণসংখ্যা			ki;
    পূর্ণসংখ্যা			idx;
    ধ্রুবক funcentry_T	*fe;
    dict_T		*d;
    list_T		*args;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_ARGS];
    list_T		*argtypes_list = ফাঁকা;
    পূর্ণসংখ্যা			argcount;
    পূর্ণসংখ্যা			vim9 = -1;	// -1: the context of the caller
    অক্ষর		*ret_name;
    অক্ষর		*tofree;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;
    যদি (in_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;
    যদি (check_for_opt_dict_arg(argvars, 2) == FAIL)
	ফেরত;

    kindname = tv_get_string(&argvars[0]);
    জন্য (ki = 0; ki < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(getinfo_kinds); ++ki)
	যদি (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    বিরতি;
    যদি (ki == (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(getinfo_kinds))
    {
	semsg(_(e_invalid_argument_str), kindname);
	ফেরত;
    }
    kind = getinfo_kinds[ki].kind;
    name = tv_get_string_buf(&argvars[1], namebuf);

    যদি (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != ফাঁকা)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	পূর্ণসংখ্যা	    todo = (পূর্ণসংখ্যা)opts->dv_hashtab.ht_used;

	FOR_ALL_HASHTAB_বস্তুসমূহ(&opts->dv_hashtab, hi, todo)
	{
	    যদি (HASHITEM_EMPTY(hi))
		চালু;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    যদি (STRCMP(di->di_চাবি, "argtypes") == 0 && kind == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    অন্যথা যদি (STRCMP(di->di_চাবি, "vim9") == 0 && kind == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    অন্যথা
	    {
		semsg(_(e_invalid_argument_str), di->di_চাবি);
		ফেরত;
	    }
	}
    }
    যদি (kind == GI_VIMVAR)
    {
	যদি (STRNCMP(name, "v:", 2) == 0)
	    (শূন্য)vim_var_info(name + 2, rettv->vval.v_dict);
	ফেরত;
    }
    যদি (kind == GI_COMMAND)
    {
	(শূন্য)ex_command_info(name, vim9, rettv->vval.v_dict);
	ফেরত;
    }
    যদি (kind == GI_OPTION)
    {
	(শূন্য)option_info(name, rettv->vval.v_dict);
	ফেরত;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = find_internal_func_opt(name, FALSE);
    যদি (idx < 0)
    {
	(শূন্য)user_func_info(name, rettv->vval.v_dict);
	ফেরত;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, আকার(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    যদি (argcount < 0)
    {
	clear_type_list(&type_gap);
	ফেরত;
    }
    d = rettv->vval.v_dict;
    dict_add_string(d, "name", (char_u *)fe->f_name);
    dict_add_string(d, "kind", (char_u *)"builtin");
    dict_add_bool(d, "available", fe->f_func != ফাঁকা);
    dict_add_সংখ্যা(d, "minargs", fe->f_min_argc);
    dict_add_সংখ্যা(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_add_সংখ্যা(d, "method", fe->f_argtype & FEARG_MASK);

    args = list_alloc();
    যদি (args == ফাঁকা || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	clear_type_list(&type_gap);
	ফেরত;
    }
    argnames = (char_u *)fe->f_argnames;
    জন্য (পূর্ণসংখ্যা i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = ফাঁকা;
	পূর্ণসংখ্যা	    argnamelen = 0;

	যদি (fe->f_argcheck == ফাঁকা)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    যদি (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		বিরতি;
	    check = arg_any;
	}
	অন্যথা
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    যদি ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == ফাঁকা)
		বিরতি;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	যদি (argnames != ফাঁকা)
	{
	    char_u  *end = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = end == ফাঁকা ? (পূর্ণসংখ্যা)STRLEN(argnames)
						     : (পূর্ণসংখ্যা)(end - argnames);
	    argnames = end == ফাঁকা ? ফাঁকা : skipwhite(end + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	যদি (arg == ফাঁকা || types == ফাঁকা
		|| dict_add_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    ফেরত;
	}
	যদি ((argnamelen > 0
		    && dict_add_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    ফেরত;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    যদি (argtypes_list == ফাঁকা
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	অক্ষর	*tofree_max;
	অক্ষর	*max_name;

	জন্য (পূর্ণসংখ্যা i = argcount; i < fe->f_max_argc && i < MAX_FUNC_ARGS; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	যদি (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = ফাঁকা;
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
    স্থির শূন্য
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    পূর্ণসংখ্যা		i;
    list_T	*l;
    dict_T	*d;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL)))
	ফেরত;

    wp = find_tabwin(&argvars[0], &argvars[1], ফাঁকা);
    যদি (wp == ফাঁকা)
	ফেরত;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    যদি (l == ফাঁকা)
	ফেরত;
    যদি (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	ফেরত;
    }

    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    জন্য (i = 0; i < wp->w_jumplistlen; ++i)
    {
	যদি (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    চালু;
	যদি ((d = dict_alloc()) == ফাঁকা)
	    ফেরত;
	যদি (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ফেরত;
	}
	dict_add_সংখ্যা(d, "lnum", (long)wp->w_jumplist[i].fmark.mark.lnum);
	dict_add_সংখ্যা(d, "col", (long)wp->w_jumplist[i].fmark.mark.col);
	dict_add_সংখ্যা(d, "coladd", (long)wp->w_jumplist[i].fmark.mark.coladd);
	dict_add_সংখ্যা(d, "bufnr", (long)wp->w_jumplist[i].fmark.fnum);
	যদি (wp->w_jumplist[i].fname != ফাঁকা)
	    dict_add_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    স্থির শূন্য
f_getpid(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    স্থির শূন্য
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    স্থির শূন্য
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    স্থির শূন্য
f_getpos(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    স্থির পূর্ণসংখ্যা
block_def2str(কাঠামো block_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    যদি (ret->string == ফাঁকা)
    {
	ret->দৈর্ঘ্য = 0;
	ফেরত FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->দৈর্ঘ্য = bd->startspaces;
    mch_memmove(ret->string + ret->দৈর্ঘ্য, bd->textstart, bd->textlen);
    ret->দৈর্ঘ্য += bd->textlen;
    vim_memset(ret->string + ret->দৈর্ঘ্য, ' ', bd->endspaces);
    ret->দৈর্ঘ্য += bd->endspaces;
    ret->string[ret->দৈর্ঘ্য] = NUL;

    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
getregionpos(
    typval_T	*argvars,
    typval_T	*rettv,
    pos_T	*p1,
    pos_T	*p2,
    পূর্ণসংখ্যা		*inclusive,
    পূর্ণসংখ্যা		*region_type,
    oparg_T	*oap)
{
    পূর্ণসংখ্যা		fnum1 = -1, fnum2 = -1;
    char_u	*type;
    buf_T	*findbuf;
    char_u	default_type[] = "v";
    পূর্ণসংখ্যা		block_width = 0;
    পূর্ণসংখ্যা		is_select_exclusive;
    পূর্ণসংখ্যা		l;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত FAIL;

    যদি (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ফেরত FAIL;

    যদি (list2fpos(&argvars[0], p1, &fnum1, ফাঁকা, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, ফাঁকা, FALSE) != OK
	    || fnum1 != fnum2)
	ফেরত FAIL;

    যদি (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	যদি (type == ফাঁকা)
	    type = default_type;
    }
    অন্যথা
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    যদি (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    অন্যথা যদি (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    অন্যথা যদি (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	যদি (*p != NUL && ((block_width = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_মান_for_argument_str_str), "type", type);
	    ফেরত FAIL;
	}
	*region_type = MBLOCK;
    }
    অন্যথা
    {
	semsg(_(e_invalid_মান_for_argument_str_str), "type", type);
	ফেরত FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    যদি (findbuf == ফাঁকা || findbuf->b_ml.ml_mfp == ফাঁকা)
    {
	emsg(_(e_বাফার_is_not_loaded));
	ফেরত FAIL;
    }

    যদি (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_সারি_গুনতি)
    {
	semsg(_(e_invalid_সারি_সংখ্যা_nr), p1->lnum);
	ফেরত FAIL;
    }
    যদি (p1->col == MAXCOL)
	p1->col = ml_get_buf_len(findbuf, p1->lnum) + 1;
    অন্যথা যদি (p1->col < 1 || p1->col > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_column_সংখ্যা_nr), p1->col);
	ফেরত FAIL;
    }

    যদি (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_সারি_গুনতি)
    {
	semsg(_(e_invalid_সারি_সংখ্যা_nr), p2->lnum);
	ফেরত FAIL;
    }
    যদি (p2->col == MAXCOL)
	p2->col = ml_get_buf_len(findbuf, p2->lnum) + 1;
    অন্যথা যদি (p2->col < 1 || p2->col > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_column_সংখ্যা_nr), p2->col);
	ফেরত FAIL;
    }

    curbuf = findbuf;
    curwin->w_বাফার = curbuf;
    virtual_op = virtual_active();

    // NOTE: Adjustment is needed.
    p1->col--;
    p2->col--;

    যদি (!LT_POS(*p1, *p2))
    {
	// swap position
	pos_T p;

	p = *p1;
	*p1 = *p2;
	*p2 = p;
    }

    যদি (*region_type == MCHAR)
    {
	// Handle 'selection' == "exclusive".
	যদি (is_select_exclusive && !EQUAL_POS(*p1, *p2))
	    // When backing up to previous line, inclusive becomes false.
	    *inclusive = !unadjust_for_sel_inner(p2);
	// If p2 is on NUL (end of line), inclusive becomes false.
	যদি (*inclusive && !virtual_op && *ml_get_pos(p2) == NUL)
	    *inclusive = FALSE;
    }
    অন্যথা যদি (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#ifdef FEAT_LINEBREAK
	পূর্ণসংখ্যা	lbr_saved = reset_lbr();
#endif

	getvvcol(curwin, p1, &sc1, ফাঁকা, &ec1, 0);
	getvvcol(curwin, p2, &sc2, ফাঁকা, &ec2, 0);

#ifdef FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#endif

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->start = *p1;
	oap->end = *p2;
	oap->start_vcol = MIN(sc1, sc2);
	যদি (block_width > 0)
	    oap->end_vcol = oap->start_vcol + block_width - 1;
	অন্যথা যদি (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->end_vcol = sc2 - 1;
	অন্যথা
	    oap->end_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    যদি (l > 1)
	p2->col += l - 1;

    ফেরত OK;
}

/*
 * "getregion()" function
 */
    স্থির শূন্য
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    পূর্ণসংখ্যা			inclusive = TRUE;
    পূর্ণসংখ্যা			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    পূর্ণসংখ্যা			save_virtual;
    string_T		akt = {ফাঁকা, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    যদি (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	ফেরত;

    জন্য (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	পূর্ণসংখ্যা ret = OK;

	যদি (region_type == MBLOCK)
	{
	    কাঠামো block_def	bd;

	    block_prep(&oa, &bd, lnum, FALSE);
	    block_def2str(&bd, &akt);
	}
	অন্যথা যদি (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.দৈর্ঘ্য = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.দৈর্ঘ্য);
	    যদি (akt.string == ফাঁকা)
		akt.দৈর্ঘ্য = 0;
	    অন্যথা
		akt.দৈর্ঘ্য = s.দৈর্ঘ্য;
	}
	অন্যথা
	{
	    কাঠামো block_def	bd;

	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);
	    block_def2str(&bd, &akt);
	}

	যদি (akt.string == ফাঁকা)
	{
	    clear_tv(rettv);
	    (শূন্য)rettv_list_alloc(rettv);
	    বিরতি;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (পূর্ণসংখ্যা)akt.দৈর্ঘ্য);
	vim_free(akt.string);

	যদি (ret == FAIL)
	{
	    clear_tv(rettv);
	    (শূন্য)rettv_list_alloc(rettv);
	    বিরতি;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_বাফার = curbuf;
    virtual_op = save_virtual;
}

    স্থির শূন্য
add_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    যদি (l1 == ফাঁকা)
	ফেরত;

    যদি (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	ফেরত;
    }

    l2 = list_alloc();
    যদি (l2 == ফাঁকা)
	ফেরত;

    যদি (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	ফেরত;
    }

    l3 = list_alloc();
    যদি (l3 == ফাঁকা)
	ফেরত;

    যদি (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	ফেরত;
    }

    list_append_সংখ্যা(l2, curbuf->b_fnum);
    list_append_সংখ্যা(l2, p1.lnum);
    list_append_সংখ্যা(l2, p1.col);
    list_append_সংখ্যা(l2, p1.coladd);

    list_append_সংখ্যা(l3, curbuf->b_fnum);
    list_append_সংখ্যা(l3, p2.lnum);
    list_append_সংখ্যা(l3, p2.col);
    list_append_সংখ্যা(l3, p2.coladd);
}

/*
 * Compute the positions of the region segment on line "lnum".
 * "ret_p1" is set to the start position of the segment and "ret_p2" to its
 * end position.
 */
    স্থির শূন্য
getregionpos_সারি(
    linenr_T	lnum,
    pos_T	p1,
    pos_T	p2,
    পূর্ণসংখ্যা		inclusive,
    পূর্ণসংখ্যা		region_type,
    oparg_T	*oap,
    পূর্ণসংখ্যা		allow_eol,
    pos_T	*ret_p1,
    pos_T	*ret_p2)
{
    char_u	*সারি = ml_get(lnum);
    colnr_T	সারি_len = ml_get_len(lnum);

    যদি (region_type == MLINE)
    {
	ret_p1->col = 1;
	ret_p1->coladd = 0;
	ret_p2->col = MAXCOL;
	ret_p2->coladd = 0;
    }
    অন্যথা
    {
	কাঠামো block_def	bd;

	যদি (region_type == MBLOCK)
	    block_prep(oap, &bd, lnum, FALSE);
	অন্যথা
	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);

	যদি (bd.is_oneChar)  // selection entirely inside one char
	{
	    যদি (region_type == MBLOCK)
	    {
		ret_p1->col = mb_prevptr(সারি, bd.textstart) - সারি + 1;
		ret_p1->coladd = bd.start_char_vcols
					   - (bd.start_vcol - oap->start_vcol);
	    }
	    অন্যথা
	    {
		ret_p1->col = p1.col + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	অন্যথা যদি (region_type == MBLOCK && oap->start_vcol > bd.start_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->col = MAXCOL;
	    ret_p1->coladd = oap->start_vcol - bd.start_vcol;
	    bd.is_oneChar = TRUE;
	}
	অন্যথা যদি (bd.startspaces > 0)
	{
	    ret_p1->col = mb_prevptr(সারি, bd.textstart) - সারি + 1;
	    ret_p1->coladd = bd.start_char_vcols - bd.startspaces;
	}
	অন্যথা
	{
	    ret_p1->col = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	যদি (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->col = ret_p1->col;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	অন্যথা যদি (bd.endspaces > 0)
	{
	    ret_p2->col = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	অন্যথা
	{
	    ret_p2->col = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    যদি (!allow_eol && ret_p1->col > সারি_len)
    {
	ret_p1->col = 0;
	ret_p1->coladd = 0;
    }
    অন্যথা যদি (ret_p1->col > সারি_len + 1)
	ret_p1->col = সারি_len + 1;

    যদি (!allow_eol && ret_p2->col > সারি_len)
    {
	ret_p2->col = ret_p1->col == 0 ? 0 : সারি_len;
	ret_p2->coladd = 0;
    }
    অন্যথা যদি (ret_p2->col > সারি_len + 1)
	ret_p2->col = সারি_len + 1;

    ret_p1->lnum = lnum;
    ret_p2->lnum = lnum;
}

/*
 * "getregionpos()" function
 */
    স্থির শূন্য
f_getregionpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	p1, p2;
    পূর্ণসংখ্যা		inclusive = TRUE;
    পূর্ণসংখ্যা		region_type = -1;
    পূর্ণসংখ্যা		allow_eol = FALSE;
    পূর্ণসংখ্যা		bounds_only = FALSE;
    oparg_T	oa;
    পূর্ণসংখ্যা		lnum;

    buf_T	*save_curbuf;
    পূর্ণসংখ্যা		save_virtual;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    যদি (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	ফেরত;

    যদি (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    যদি (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	start_pos, end_pos;

	getregionpos_সারি(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &start_pos, &end_pos);
	যদি (p2.lnum != p1.lnum)
	{
	    pos_T	unused;

	    getregionpos_সারি(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &unused, &end_pos);
	}
	add_regionpos_range(rettv, start_pos, end_pos);
    }
    অন্যথা
    {
	জন্য (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_সারি(lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &ret_p1, &ret_p2);
	    add_regionpos_range(rettv, ret_p1, ret_p2);
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_বাফার = curbuf;
    virtual_op = save_virtual;
}

/*
 * Common between getreg(), getreginfo() and getregtype(): get the register
 * name from the first argument.
 * Returns zero on error.
 */
    স্থির পূর্ণসংখ্যা
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	যদি (strregname != ফাঁকা && in_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_register_name_must_be_one_char_str), strregname);
	    strregname = ফাঁকা;
	}
	যদি (strregname == ফাঁকা)	    // type error; errmsg already given
	    ফেরত 0;
    }
    অন্যথা
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    ফেরত *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    স্থির শূন্য
f_getreg(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		regname;
    পূর্ণসংখ্যা		arg2 = FALSE;
    পূর্ণসংখ্যা		return_list = FALSE;

    যদি (in_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	ফেরত;

    regname = getreg_get_regname(argvars);
    যদি (regname == 0)
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	পূর্ণসংখ্যা		error = FALSE;

	arg2 = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[1], &error);

	যদি (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[2], &error);
	যদি (error)
	    ফেরত;
    }

    যদি (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_রাশিমালা_SRC : 0) | GREG_LIST);
	যদি (rettv->vval.v_list == ফাঁকা)
	    (শূন্য)rettv_list_alloc(rettv);
	অন্যথা
	    ++rettv->vval.v_list->lv_refcount;
    }
    অন্যথা
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = get_reg_contents(regname,
						    arg2 ? GREG_রাশিমালা_SRC : 0);
    }
}

/*
 * "getregtype()" function
 */
    স্থির শূন্য
f_getregtype(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		regname;
    char_u	buf[NUMBUFLEN + 2];
    long	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ফেরত;

    regname = getreg_get_regname(argvars);
    যদি (regname == 0)
	ফেরত;

    buf[0] = NUL;
    buf[1] = NUL;
    নির্বাচন (get_reg_type(regname, &reglen))
    {
	ক্ষেত্র MLINE: buf[0] = 'V'; বিরতি;
	ক্ষেত্র MCHAR: buf[0] = 'v'; বিরতি;
	ক্ষেত্র MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((অক্ষর *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		বিরতি;
    }
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "gettagstack()" function
 */
    স্থির শূন্য
f_gettagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;			// default is current window

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	যদি (wp == ফাঁকা)
	    ফেরত;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    স্থির শূন্য
f_gettext(typval_T *argvars, typval_T *rettv)
{
#যদি defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    অক্ষর *পূর্ব = ফাঁকা;
#endif

    যদি (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	ফেরত;

    rettv->v_type = VAR_STRING;

    যদি (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != ফাঁকা &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#যদি defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	পূর্ব = bind_textdomain_codeset((ধ্রুবক অক্ষর *)argvars[1].vval.v_string, (অক্ষর *)p_enc);
#endif

#যদি defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((ধ্রুবক অক্ষর *)argvars[1].vval.v_string, (ধ্রুবক অক্ষর *)argvars[0].vval.v_string));
#অন্যথা
	textdomain((ধ্রুবক অক্ষর *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#endif

#যদি defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	যদি (পূর্ব != ফাঁকা)
	    bind_textdomain_codeset((ধ্রুবক অক্ষর *)argvars[1].vval.v_string, পূর্ব);
#endif
    }
    অন্যথা
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
}

// for VIM_VERSION_ defines
#অন্তর্ভাব "version.h"

/*
 * "has()" function
 */
    শূন্য
f_has(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		i;
    char_u	*name;
    পূর্ণসংখ্যা		x = FALSE;
    পূর্ণসংখ্যা		n = FALSE;
    প্রকারলেখ কাঠামো {
	অক্ষর *name;
	short present;
    } has_item_T;
    স্থির has_item_T has_list[] =
    {
	{"amiga",
#ifdef AMIGA
		1
#অন্যথা
		0
#endif
		},
	{"android",
#ifdef __ANDROID__
		1
#অন্যথা
		0
#endif
		},
	{"arp",
#যদি defined(AMIGA) && defined(FEAT_ARP)
		1
#অন্যথা
		0
#endif
		},
	{"haiku",
#ifdef __HAIKU__
		1
#অন্যথা
		0
#endif
		},
	{"bsd",
#যদি defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#অন্যথা
		0
#endif
		},
	{"hpux",
#ifdef hpux
		1
#অন্যথা
		0
#endif
		},
	{"hurd",
#ifdef __GNU__
		1
#অন্যথা
		0
#endif
		},
	{"linux",
#ifdef __linux__
		1
#অন্যথা
		0
#endif
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#ifdef MACOS_X
		1
#অন্যথা
		0
#endif
		},
	{"osx",		// Mac OS X
#ifdef MACOS_X
		1
#অন্যথা
		0
#endif
		},
	{"macunix",	// Mac OS X, with the darwin feature
#যদি defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#অন্যথা
		0
#endif
		},
	{"osxdarwin",	// synonym for macunix
#যদি defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#অন্যথা
		0
#endif
		},
	{"qnx",
#ifdef __QNX__
		1
#অন্যথা
		0
#endif
		},
	{"sun",
#ifdef SUN_SYSTEM
		1
#অন্যথা
		0
#endif
		},
	{"termux",
#ifdef __TERMUX__
		1
#অন্যথা
		0
#endif
		},
	{"unix",
#ifdef UNIX
		1
#অন্যথা
		0
#endif
		},
	{"vms",
#ifdef VMS
		1
#অন্যথা
		0
#endif
		},
	{"win32",
#ifdef MSWIN
		1
#অন্যথা
		0
#endif
		},
	{"win32unix",
#ifdef WIN32UNIX
		1
#অন্যথা
		0
#endif
		},
	{"win64",
#ifdef _WIN64
		1
#অন্যথা
		0
#endif
		},
	{"ebcdic", 0 },
	{"fname_case",
#ifndef CASE_INSENSITIVE_FILENAME
		1
#অন্যথা
		0
#endif
		},
	{"acl",
#ifdef HAVE_ACL
		1
#অন্যথা
		0
#endif
		},
	{"arabic",
#ifdef FEAT_ARABIC
		1
#অন্যথা
		0
#endif
		},
	{"autocmd", 1},
	{"autochdir",
#ifdef FEAT_AUTOCHDIR
		1
#অন্যথা
		0
#endif
		},
	{"autoservername",
#ifdef FEAT_AUTOSERVERNAME
		1
#অন্যথা
		0
#endif
		},
	{"socketserver",
#ifdef FEAT_SOCKETSERVER
		1
#অন্যথা
		0
#endif
		},
	{"balloon_eval",
#ifdef FEAT_BEVAL_GUI
		1
#অন্যথা
		0
#endif
		},
	{"balloon_multiline",
#ifdef FEAT_BEVAL_GUI
		1
#অন্যথা
		0
#endif
		},
	{"balloon_eval_term",
#ifdef FEAT_BEVAL_TERM
		1
#অন্যথা
		0
#endif
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#যদি defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#অন্যথা
		0
#endif
		},
	{"byte_offset",
#ifdef FEAT_BYTEOFF
		1
#অন্যথা
		0
#endif
		},
	{"channel",
#ifdef FEAT_JOB_CHANNEL
		1
#অন্যথা
		0
#endif
		},
	{"cindent", 1},
	{"clientserver",
#ifdef FEAT_CLIENTSERVER
		1
#অন্যথা
		0
#endif
		},
	{"clipboard",
#ifdef FEAT_CLIPBOARD
		1
#অন্যথা
		0
#endif
		},
	{"clipboard_provider",
#ifdef FEAT_CLIPBOARD_PROVIDER
		1
#অন্যথা
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
#অন্যথা
		0
#endif
		},
	{"cryptv",
#ifdef FEAT_CRYPT
		1
#অন্যথা
		0
#endif
		},
	{"crypt-blowfish",
#ifdef FEAT_CRYPT
		1
#অন্যথা
		0
#endif
		},
	{"crypt-blowfish2",
#ifdef FEAT_CRYPT
		1
#অন্যথা
		0
#endif
		},
	{"cscope",
#ifdef FEAT_CSCOPE
		1
#অন্যথা
		0
#endif
		},
	{"cursorbind", 1},
	{"cursorshape",
#ifdef CURSOR_SHAPE
		1
#অন্যথা
		0
#endif
		},
	{"debug",
#ifdef DEBUG
		1
#অন্যথা
		0
#endif
		},
	{"dialog_con",
#ifdef FEAT_CON_DIALOG
		1
#অন্যথা
		0
#endif
		},
	{"dialog_con_gui",
#যদি defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#অন্যথা
		0
#endif
		},
	{"dialog_gui",
#ifdef FEAT_GUI_DIALOG
		1
#অন্যথা
		0
#endif
		},
	{"diff",
#ifdef FEAT_DIFF
		1
#অন্যথা
		0
#endif
		},
	{"digraphs",
#ifdef FEAT_DIGRAPHS
		1
#অন্যথা
		0
#endif
		},
	{"directx",
#ifdef FEAT_DIRECTX
		1
#অন্যথা
		0
#endif
		},
	{"dnd",
#ifdef FEAT_DND
		1
#অন্যথা
		0
#endif
		},
	{"drop_file",
#ifdef HAVE_DROP_FILE
		1
#অন্যথা
		0
#endif
		},
	{"emacs_tags",
#ifdef FEAT_EMACS_TAGS
		1
#অন্যথা
		0
#endif
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#ifdef FEAT_অনুসন্ধান_EXTRA
		1
#অন্যথা
		0
#endif
		},
	{"file_in_path", 1},
	{"filterpipe",
#যদি defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#অন্যথা
		0
#endif
		},
	{"find_in_path",
#ifdef FEAT_FIND_ID
		1
#অন্যথা
		0
#endif
		},
	{"float", 1},
	{"folding",
#ifdef FEAT_FOLDING
		1
#অন্যথা
		0
#endif
		},
	{"footer", 0},
	{"fork",
#যদি !defined(USE_SYSTEM) && defined(UNIX)
		1
#অন্যথা
		0
#endif
		},
	{"gettext",
#ifdef FEAT_GETTEXT
		1
#অন্যথা
		0
#endif
		},
	{"gui",
#ifdef FEAT_GUI
		1
#অন্যথা
		0
#endif
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#ifdef FEAT_GUI_GTK
		1
#অন্যথা
		0
#endif
		},
	{"gui_gtk2",
#যদি defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#অন্যথা
		0
#endif
		},
	{"gui_gtk3",
#যদি defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#অন্যথা
		0
#endif
		},
	{"gui_gtk4",
#যদি defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#অন্যথা
		0
#endif
		},
	{"gui_gnome",
#ifdef FEAT_GUI_GNOME
		1
#অন্যথা
		0
#endif
		},
	{"gui_haiku",
#ifdef FEAT_GUI_HAIKU
		1
#অন্যথা
		0
#endif
		},
	{"gui_mac", 0},
	{"gui_motif",
#ifdef FEAT_GUI_MOTIF
		1
#অন্যথা
		0
#endif
		},
	{"gui_photon",
#ifdef FEAT_GUI_PHOTON
		1
#অন্যথা
		0
#endif
		},
	{"gui_win32",
#ifdef FEAT_GUI_MSWIN
		1
#অন্যথা
		0
#endif
		},
	{"iconv",
#যদি defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#অন্যথা
		0
#endif
		},
	{"image",
#ifdef FEAT_IMAGE
		1
#অন্যথা
		0
#endif
		},
	{"image_popup",
#ifdef FEAT_IMAGE_POPUP
		1
#অন্যথা
		0
#endif
		},
	{"insert_expand", 1},
	{"ipv6",
#ifdef FEAT_IPV6
		1
#অন্যথা
		0
#endif
	},
	{"job",
#ifdef FEAT_JOB_CHANNEL
		1
#অন্যথা
		0
#endif
		},
	{"jumplist", 1},
	{"keymap",
#ifdef FEAT_KEYMAP
		1
#অন্যথা
		0
#endif
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#ifdef FEAT_LANGMAP
		1
#অন্যথা
		0
#endif
		},
	{"libcall",
#ifdef FEAT_LIBCALL
		1
#অন্যথা
		0
#endif
		},
	{"linebreak",
#ifdef FEAT_LINEBREAK
		1
#অন্যথা
		0
#endif
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#যদি defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#অন্যথা
		0
#endif
		},
	{"menu",
#ifdef FEAT_MENU
		1
#অন্যথা
		0
#endif
		},
	{"mksession",
#ifdef FEAT_SESSION
		1
#অন্যথা
		0
#endif
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#ifdef FEAT_MOUSESHAPE
		1
#অন্যথা
		0
#endif
		},
	{"mouse_dec",
#যদি (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#অন্যথা
		0
#endif
		},
	{"mouse_gpm",
#যদি (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#অন্যথা
		0
#endif
		},
	{"mouse_jsbterm",
#যদি (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#অন্যথা
		0
#endif
		},
	{"mouse_netterm",
#যদি (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#অন্যথা
		0
#endif
		},
	{"mouse_pterm",
#যদি (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#অন্যথা
		0
#endif
		},
	{"mouse_sgr",
#যদি (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#অন্যথা
		0
#endif
		},
	{"mouse_sysmouse",
#যদি (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#অন্যথা
		0
#endif
		},
	{"mouse_urxvt",
#যদি (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#অন্যথা
		0
#endif
		},
	{"mouse_xterm",
#যদি (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#অন্যথা
		0
#endif
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#ifdef FEAT_MBYTE_IME
		1
#অন্যথা
		0
#endif
		},
	{"multi_lang",
#ifdef FEAT_MULTI_LANG
		1
#অন্যথা
		0
#endif
		},
	{"mzscheme",
#যদি defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#অন্যথা
		0
#endif
		},
	{"nanotime",
#ifdef ST_MTIM_NSEC
		1
#অন্যথা
		0
#endif
	},
	{"num64", 1},
	{"ole",
#ifdef FEAT_OLE
		1
#অন্যথা
		0
#endif
		},
	{"packages",
#ifdef FEAT_EVAL
		1
#অন্যথা
		0
#endif
		},
	{"path_extra", 1},
	{"perl",
#যদি defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#অন্যথা
		0
#endif
		},
	{"persistent_undo",
#ifdef FEAT_PERSISTENT_UNDO
		1
#অন্যথা
		0
#endif
		},
	{"python_compiled",
#যদি defined(FEAT_PYTHON)
		1
#অন্যথা
		0
#endif
		},
	{"python_dynamic",
#যদি defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#অন্যথা
		0
#endif
		},
	{"python",
#যদি defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#অন্যথা
		0
#endif
		},
	{"pythonx",
#যদি (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#অন্যথা
		0
#endif
		},
	{"python3_compiled",
#যদি defined(FEAT_PYTHON3)
		1
#অন্যথা
		0
#endif
		},
	{"python3_dynamic",
#যদি defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#অন্যথা
		0
#endif
		},
	{"python3_stable",
#যদি defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#অন্যথা
		0
#endif
		},
	{"python3",
#যদি defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#অন্যথা
		0
#endif
		},
	{"popupwin",
#ifdef FEAT_PROP_POPUP
		1
#অন্যথা
		0
#endif
		},
	{"postscript",
#ifdef FEAT_POSTSCRIPT
		1
#অন্যথা
		0
#endif
		},
	{"pango",
#ifdef FEAT_PRINT_PANGO
		1
#অন্যথা
		0
#endif
		},
	{"printer",
#ifdef FEAT_PRINTER
		1
#অন্যথা
		0
#endif
		},
	{"profile",
#ifdef FEAT_PROFILE
		1
#অন্যথা
		0
#endif
		},
	{"prof_nsec",
#ifdef PROF_NSEC
		1
#অন্যথা
		0
#endif
		},
	{"reltime",
#ifdef FEAT_RELTIME
		1
#অন্যথা
		0
#endif
		},
	{"quickfix",
#ifdef FEAT_QUICKFIX
		1
#অন্যথা
		0
#endif
		},
	{"rightleft",
#ifdef FEAT_RIGHTLEFT
		1
#অন্যথা
		0
#endif
		},
	{"ruby",
#যদি defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#অন্যথা
		0
#endif
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#ifdef FEAT_SIGNS
		1
#অন্যথা
		0
#endif
		},
	{"smartindent", 1},
	{"startuptime",
#ifdef STARTUPTIME
		1
#অন্যথা
		0
#endif
		},
	{"statusline",
#ifdef FEAT_STL_OPT
		1
#অন্যথা
		0
#endif
		},
	{"statusline_click",
#ifdef FEAT_STL_OPT
		1
#অন্যথা
		0
#endif
		},
	{"netbeans_intg",
#ifdef FEAT_NETBEANS_INTG
		1
#অন্যথা
		0
#endif
		},
	{"sodium",
#যদি defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#অন্যথা
		0
#endif
		},
	{"sound",
#ifdef FEAT_SOUND
		1
#অন্যথা
		0
#endif
		},
	{"spell",
#ifdef FEAT_SPELL
		1
#অন্যথা
		0
#endif
		},
	{"syntax",
#ifdef FEAT_SYN_HL
		1
#অন্যথা
		0
#endif
		},
	{"system",
#যদি defined(USE_SYSTEM) || !defined(UNIX)
		1
#অন্যথা
		0
#endif
		},
	{"tabpanel",
#যদি defined(FEAT_TABPANEL)
		1,
#অন্যথা
		0,
#endif
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#যদি defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#অন্যথা
		0
#endif
		},
	{"termguicolors",
#ifdef FEAT_TERMGUICOLORS
		1
#অন্যথা
		0
#endif
		},
	{"terminal",
#যদি defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#অন্যথা
		0
#endif
		},
	{"terminfo",
#ifdef TERMINFO
		1
#অন্যথা
		0
#endif
		},
	{"termresponse",
#ifdef FEAT_TERMRESPONSE
		1
#অন্যথা
		0
#endif
		},
	{"textobjects", 1},
	{"textprop",
#ifdef FEAT_PROP_POPUP
		1
#অন্যথা
		0
#endif
		},
	{"tgetent",
#ifdef HAVE_TGETENT
		1
#অন্যথা
		0
#endif
		},
	{"timers",
#ifdef FEAT_TIMERS
		1
#অন্যথা
		0
#endif
		},
	{"title", 1},
	{"toolbar",
#ifdef FEAT_TOOLBAR
		1
#অন্যথা
		0
#endif
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#ifdef FEAT_VARTABS
		1
#অন্যথা
		0
#endif
		},
	{"vertsplit", 1},
	{"viminfo",
#ifdef FEAT_VIMINFO
		1
#অন্যথা
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
#অন্যথা
		0
#endif
		},
	{"wayland",
#ifdef FEAT_WAYLAND
		1
#অন্যথা
		0
#endif
		},
	{"wayland_clipboard",
#ifdef FEAT_WAYLAND_CLIPBOARD
		1
#অন্যথা
		0
#endif
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#ifdef FEAT_WAK
		1
#অন্যথা
		0
#endif
		},
	{"writebackup",
#ifdef FEAT_WRITEBACKUP
		1
#অন্যথা
		0
#endif
		},
	{"xattr",
#ifdef FEAT_XATTR
		1
#অন্যথা
		0
#endif
		},
	{"xim",
#ifdef FEAT_XIM
		1
#অন্যথা
		0
#endif
		},
	{"xfontset",
#ifdef FEAT_XFONTSET
		1
#অন্যথা
		0
#endif
		},
	{"xpm",
#যদি defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#অন্যথা
		0
#endif
		},
	{"xpm_w32",	// for backward compatibility
#ifdef FEAT_XPM_W32
		1
#অন্যথা
		0
#endif
		},
	{"xsmp",
#ifdef USE_XSMP
		1
#অন্যথা
		0
#endif
		},
	{"xsmp_interact",
#ifdef USE_XSMP_INTERACT
		1
#অন্যথা
		0
#endif
		},
	{"xterm_clipboard",
#ifdef FEAT_XCLIPBOARD
		1
#অন্যথা
		0
#endif
		},
	{"xterm_save",
#ifdef FEAT_XTERM_SAVE
		1
#অন্যথা
		0
#endif
		},
	{"X11",
#যদি defined(UNIX) && defined(FEAT_X11)
		1
#অন্যথা
		0
#endif
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#যদি defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#অন্যথা
		0
#endif
		},
	{ফাঁকা, 0}
    };

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ফেরত;

    name = tv_get_string(&argvars[0]);

    // Fast-path: check features not in has_list[] first to avoid the full
    // linear scan for very common queries like has('patch-...').
    যদি (STRNICMP(name, "patch", 5) == 0)
    {
	x = TRUE;
	যদি (name[5] == '-'
		&& STRLEN(name) >= 11
		&& (name[6] >= '1' && name[6] <= '9'))
	{
	    অক্ষর	*end;
	    পূর্ণসংখ্যা	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (পূর্ণসংখ্যা)strtoul((অক্ষর *)name + 6, &end, 10);
	    যদি (*end == '.' && vim_isdigit(end[1])
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
	অন্যথা যদি (SAFE_isdigit(name[5]))
	    n = has_patch(atoi((অক্ষর *)name + 5));
    }
    অন্যথা যদি (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    অন্যথা যদি (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_প্রবেশ_isatty();
    }
    অন্যথা যদি (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    অন্যথা যদি (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    অন্যথা যদি (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#ifdef FEAT_GUI
	n = (gui.in_use || gui.starting);
#endif
    }
    অন্যথা যদি (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#যদি defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.in_use;	// gui_mch_browse() works when GUI is running
#endif
    }
    অন্যথা যদি (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#ifdef FEAT_SYN_HL
	n = syntax_present(curwin);
#endif
    }
    অন্যথা যদি (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#ifdef FEAT_VTP
	n = is_term_win32() && has_vtp_working();
#endif
    }
    অন্যথা যদি (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_NETBEANS_INTG
	n = netbeans_active();
#endif
    }
    অন্যথা যদি (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_MOUSE_GPM
	n = gpm_enabled();
#endif
    }
    অন্যথা যদি (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#যদি defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#endif
    }
    অন্যথা যদি (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_star.available;
#endif
    }
    অন্যথা যদি (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# যদি defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	যদি (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	অন্যথা
	    n = FALSE;
# অন্যথা
	n = FALSE;
# endif
#endif
    }

    // Look up in has_list[] only if not already handled above.
    যদি (x == FALSE)
    {
	জন্য (i = 0; has_list[i].name != ফাঁকা; ++i)
	    যদি (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		বিরতি;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	যদি (x == TRUE && n == FALSE)
	{
	    যদি (0)
	    {
		// intentionally empty
	    }
#ifdef VIMDLL
	    অন্যথা যদি (STRICMP(name, "filterpipe") == 0)
		n = gui.in_use || gui.starting;
#endif
#যদি defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    অন্যথা যদি (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#endif
#ifdef DYNAMIC_LUA
	    অন্যথা যদি (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#endif
#ifdef DYNAMIC_MZSCHEME
	    অন্যথা যদি (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#endif
#ifdef DYNAMIC_PERL
	    অন্যথা যদি (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON
	    অন্যথা যদি (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON3
	    অন্যথা যদি (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#endif
#যদি defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    অন্যথা যদি (STRICMP(name, "pythonx") == 0)
	    {
# যদি defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		যদি (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		অন্যথা যদি (p_pyx == 3)
		    n = python3_enabled(FALSE);
		অন্যথা যদি (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#endif
#ifdef DYNAMIC_RUBY
	    অন্যথা যদি (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#endif
#ifdef DYNAMIC_TCL
	    অন্যথা যদি (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#endif
#ifdef DYNAMIC_SODIUM
	    অন্যথা যদি (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#endif
#যদি defined(FEAT_TERMINAL) && defined(MSWIN)
	    অন্যথা যদি (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#endif
#ifdef DYNAMIC_GPM
	    অন্যথা যদি (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_available();
#endif
	}
    }

    যদি (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_সংখ্যা = x;
    অন্যথা
	// return whether feature is enabled
	rettv->vval.v_সংখ্যা = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    পূর্ণসংখ্যা
dynamic_feature(char_u *feature)
{
    ফেরত (feature == ফাঁকা
#যদি defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.in_use)
#endif
#ifdef VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#endif
#যদি defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.in_use)
#endif
#যদি defined(USE_ICONV) && defined(DYNAMIC_ICONV)
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
#যদি defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
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
#যদি defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#endif
	    );
}

/*
 * "haslocaldir()" function
 */
    স্থির শূন্য
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = ফাঁকা;
    win_T	*wp = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL)))
	ফেরত;

    wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    যদি (wp != ফাঁকা && wp->w_localdir != ফাঁকা)
	rettv->vval.v_সংখ্যা = 1;
    অন্যথা যদি (tp != ফাঁকা && tp->tp_localdir != ফাঁকা)
	rettv->vval.v_সংখ্যা = 2;
    অন্যথা
	rettv->vval.v_সংখ্যা = 0;
}

/*
 * "highlightID(name)" function
 */
    স্থির শূন্য
f_hlID(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->vval.v_সংখ্যা = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    স্থির শূন্য
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->vval.v_সংখ্যা = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    স্থির শূন্য
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
# যদি defined(HAVE_NO_LONG_LONG)
প্রকারলেখ unsigned পূর্ণসংখ্যা uintptr_t;
# অন্যথা
প্রকারলেখ unsigned long long uintptr_t;
# endif
#endif // VMS

    স্থির শূন্য
f_id(typval_T *argvars, typval_T *rettv)
{
    অক্ষর    numbuf[NUMBUFLEN];
    অক্ষর    *p = numbuf;

    নির্বাচন (argvars[0].v_type)
    {
	ক্ষেত্র VAR_LIST:
	ক্ষেত্র VAR_TUPLE:
	ক্ষেত্র VAR_DICT:
	ক্ষেত্র VAR_OBJECT:
	ক্ষেত্র VAR_JOB:
	ক্ষেত্র VAR_CHANNEL:
	ক্ষেত্র VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    যদি (argvars[0].vval.v_object != ফাঁকা)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(শূন্য *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		জন্য (; p < numbuf + আকার(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	পূর্বনির্ধারিত:
	    বিরতি;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    স্থির শূন্য
সূচক_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    পূর্ণসংখ্যা		start = 0;
    পূর্ণসংখ্যা		error = FALSE;
    পূর্ণসংখ্যা		ic = FALSE;

    b = argvars[0].vval.v_blob;
    যদি (b == ফাঁকা)
	ফেরত;

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	start = tv_get_সংখ্যা_chk(&argvars[2], &error);
	যদি (error)
	    ফেরত;
    }

    যদি (start < 0)
    {
	start = blob_len(b) + start;
	যদি (start < 0)
	    start = 0;
    }

    জন্য (পূর্ণসংখ্যা idx = start; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_সংখ্যা;
	tv.vval.v_সংখ্যা = blob_get(b, idx);
	যদি (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_সংখ্যা = idx;
	    ফেরত;
	}
    }
}

/*
 * index() function for a tuple
 */
    স্থির শূন্য
সূচক_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    পূর্ণসংখ্যা		ic = FALSE;
    পূর্ণসংখ্যা		error = FALSE;

    যদি (tuple == ফাঁকা)
	ফেরত;

    পূর্ণসংখ্যা	start_idx = 0;
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	start_idx = tv_get_সংখ্যা_chk(&argvars[2], &error);
	যদি (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[3], &error);
	যদি (error)
	    ফেরত;
    }

    rettv->vval.v_সংখ্যা = সূচক_tuple(tuple, &argvars[1], start_idx, ic);
}

/*
 * index() function for a list
 */
    স্থির শূন্য
সূচক_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*item;
    long	idx = 0;
    পূর্ণসংখ্যা		ic = FALSE;
    পূর্ণসংখ্যা		error = FALSE;

    l = argvars[0].vval.v_list;
    যদি (l == ফাঁকা)
	ফেরত;

    CHECK_LIST_MATERIALIZE(l);
    item = l->lv_first;
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, (long)tv_get_সংখ্যা_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	যদি (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[3], &error);
	যদি (error)
	    item = ফাঁকা;
    }

    জন্য ( ; item != ফাঁকা; item = item->li_পরবর্তী, ++idx)
	যদি (tv_equal(&item->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_সংখ্যা = idx;
	    বিরতি;
	}
}

/*
 * "index()" function
 */
    স্থির শূন্য
f_সূচক(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_সংখ্যা_arg(argvars, 1) == FAIL)
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ফেরত;

    যদি (argvars[0].v_type == VAR_BLOB)
	সূচক_func_blob(argvars, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_TUPLE)
	সূচক_func_tuple(argvars, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_LIST)
	সূচক_func_list(argvars, rettv);
    অন্যথা
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    পূর্ণসংখ্যা
indexof_eval_রাশিমালা(typval_T *রাশিমালা)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	found;
    পূর্ণসংখ্যা		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_চাবি);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    যদি (eval_রাশিমালা_typval(রাশিমালা, FALSE, argv, 2, ফাঁকা, &newtv) == FAIL)
	ফেরত FALSE;

    found = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    ফেরত error ? FALSE : found;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    স্থির পূর্ণসংখ্যা
indexof_blob(blob_T *b, long startidx, typval_T *রাশিমালা)
{
    long	idx = 0;

    যদি (b == ফাঁকা)
	ফেরত -1;

    যদি (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	যদি (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_চাবি, VAR_সংখ্যা);
    set_vim_var_type(VV_VAL, VAR_সংখ্যা);

    পূর্ণসংখ্যা		called_emsg_start = called_emsg;
    জন্য (idx = startidx; idx < blob_len(b); ++idx)
    {
	set_vim_var_nr(VV_চাবি, idx);
	set_vim_var_nr(VV_VAL, blob_get(b, idx));

	যদি (indexof_eval_রাশিমালা(রাশিমালা))
	    ফেরত idx;

	যদি (called_emsg != called_emsg_start)
	    ফেরত -1;
    }

    ফেরত -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    স্থির পূর্ণসংখ্যা
indexof_list(list_T *l, long startidx, typval_T *রাশিমালা)
{
    listitem_T	*item;
    long	idx = 0;
    পূর্ণসংখ্যা		found;

    যদি (l == ফাঁকা)
	ফেরত -1;

    CHECK_LIST_MATERIALIZE(l);

    যদি (startidx == 0)
	item = l->lv_first;
    অন্যথা
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, startidx);
	যদি (item != ফাঁকা)
	    idx = l->lv_u.mat.lv_idx;
    }

    set_vim_var_type(VV_চাবি, VAR_সংখ্যা);

    পূর্ণসংখ্যা		called_emsg_start = called_emsg;
    জন্য ( ; item != ফাঁকা; item = item->li_পরবর্তী, ++idx)
    {
	set_vim_var_nr(VV_চাবি, idx);
	copy_tv(&item->li_tv, get_vim_var_tv(VV_VAL));

	found = indexof_eval_রাশিমালা(রাশিমালা);
	clear_tv(get_vim_var_tv(VV_VAL));

	যদি (found)
	    ফেরত idx;

	যদি (called_emsg != called_emsg_start)
	    ফেরত -1;
    }

    ফেরত -1;
}

/*
 * "indexof()" function
 */
    স্থির শূন্য
f_indexof(typval_T *argvars, typval_T *rettv)
{
    long	startidx = 0;
    typval_T	save_val;
    typval_T	save_চাবি;
    পূর্ণসংখ্যা		save_did_emsg;

    rettv->vval.v_সংখ্যা = -1;

    যদি (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ফেরত;

    যদি ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == ফাঁকা
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == ফাঁকা))
	ফেরত;

    যদি (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_সংখ্যা_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_চাবি, &save_চাবি);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    যদি (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_সংখ্যা = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    অন্যথা যদি (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_সংখ্যা = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    অন্যথা
	rettv->vval.v_সংখ্যা = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_চাবি, &save_চাবি);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

স্থির পূর্ণসংখ্যা inputsecret_flag = 0;

/*
 * "input()" function
 *     Also handles inputsecret() when inputsecret is set.
 */
    স্থির শূন্য
f_প্রবেশ(typval_T *argvars, typval_T *rettv)
{
    get_user_প্রবেশ(argvars, rettv, FALSE, inputsecret_flag);
}

/*
 * "inputdialog()" function
 */
    স্থির শূন্য
f_inputdialog(typval_T *argvars, typval_T *rettv)
{
#যদি defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    যদি (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == ফাঁকা)
    {
	char_u	*message;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	যদি (in_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    ফেরত;

	message = tv_get_string_chk(&argvars[0]);
	যদি (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != ফাঁকা)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	অন্যথা
	    IObuff[0] = NUL;
	যদি (message != ফাঁকা && defstr != ফাঁকা
		&& do_dialog(VIM_QUESTION, ফাঁকা, message,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	অন্যথা
	{
	    যদি (message != ফাঁকা && defstr != ফাঁকা
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    অন্যথা
		rettv->vval.v_string = ফাঁকা;
	}
	rettv->v_type = VAR_STRING;
    }
    অন্যথা
#endif
	get_user_প্রবেশ(argvars, rettv, TRUE, inputsecret_flag);
}

/*
 * "inputlist()" function
 */
    স্থির শূন্য
f_inputlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    পূর্ণসংখ্যা		selected;
    পূর্ণসংখ্যা		mouse_used;

#ifdef NO_CONSOLE_প্রবেশ
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    যদি (no_console_প্রবেশ() && !is_not_a_term())
	ফেরত;
#endif
    যদি (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ফাঁকা)
    {
	semsg(_(e_argument_of_str_must_be_list), "inputlist()");
	ফেরত;
    }

    বার্তা_start();
    বার্তা_row = Rows - 1;	// for when 'cmdheight' > 1
    সারিগুলি_বাম = Rows;	// avoid more prompt
    বার্তা_scroll = TRUE;
    বার্তা_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	বার্তা_puts((অক্ষর *)tv_get_string(&li->li_tv));
	বার্তা_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_সংখ্যা(&mouse_used);
    যদি (mouse_used)
	selected -= সারিগুলি_বাম;

    rettv->vval.v_সংখ্যা = selected;
}

স্থির garray_T	    ga_userinput = {0, 0, আকার(tasave_T), 4, ফাঁকা};

/*
 * "inputrestore()" function
 */
    স্থির শূন্য
f_inputrestore(typval_T *argvars UNUSED, typval_T *rettv)
{
    যদি (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_তথ্য)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    অন্যথা যদি (p_verbose > 1)
    {
	verb_বার্তা(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_সংখ্যা = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    স্থির শূন্য
f_inputsave(typval_T *argvars UNUSED, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    যদি (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_তথ্য)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    অন্যথা
	rettv->vval.v_সংখ্যা = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    স্থির শূন্য
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ফেরত;

    ++cmdline_star;
    ++inputsecret_flag;
    f_প্রবেশ(argvars, rettv);
    --cmdline_star;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    স্থির শূন্য
f_interrupt(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    স্থির শূন্য
f_invert(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->vval.v_সংখ্যা = ~tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    স্থির শূন্য
free_lval_মূল(lval_মূল_T *মূল)
{
    যদি (মূল->lr_tv != ফাঁকা)
	free_tv(মূল->lr_tv);
    class_unref(মূল->lr_cl_exec);
    মূল->lr_tv = ফাঁকা;
    মূল->lr_cl_exec = ফাঁকা;
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

    স্থির পূর্ণসংখ্যা
fix_variable_reference_lval_মূল(lval_মূল_T *মূল, char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    পূর্ণসংখ্যা found_member = FALSE;
    যদি (মূল->lr_tv->v_type == VAR_OBJECT)
    {
	যদি (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    found_member = TRUE;
    }
    যদি (!found_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*end;
	জন্য (end = name; ASCII_ISALNUM(*end) || *end == '_'; ++end)
	    ;
	পূর্ণসংখ্যা idx = class_member_idx(মূল->lr_cl_exec, name, end - name);
	যদি (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(মূল->lr_tv);
	    copy_tv(&মূল->lr_cl_exec->class_members_tv[idx], মূল->lr_tv);
	    found_member = TRUE;
	}
    }
    যদি (!found_member)
    {
	free_tv(মূল->lr_tv);
	মূল->lr_tv = ফাঁকা;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    ফেরত OK;
}

/*
 * "islocked()" function
 */
    স্থির শূন্য
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*end;
    dictitem_T	*di;

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    char_u *name = tv_get_string(&argvars[0]);
#ifdef LOG_LOCKVAR
    ch_log(ফাঁকা, "LKVAR: f_islocked(): name: %s", name);
#endif

    lval_মূল_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_মূল_T *মূল = ফাঁকা;

    // Set up lval_root if executing in a method.
    যদি (fill_exec_lval_মূল(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	যদি (fix_variable_reference_lval_মূল(&aroot, name) == OK)
	    মূল = &aroot;
    }

    lval_মূল_T	*lval_মূল_save = lval_মূল;
    lval_মূল = মূল;
    end = get_lval(name, ফাঁকা, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_START);
    lval_মূল = lval_মূল_save;

    যদি (end != ফাঁকা && lv.ll_name != ফাঁকা)
    {
	যদি (*end != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_end
		   ? e_invalid_argument_str : e_trailing_characters_str), end);
	}
	অন্যথা
	{
	    যদি (lv.ll_tv == ফাঁকা)
	    {
		di = find_var(lv.ll_name, ফাঁকা, TRUE);
		যদি (di != ফাঁকা)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_সংখ্যা = ((di->di_flags & DI_FLAGS_LOCK)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    অন্যথা যদি (lv.ll_is_মূল)
	    {
		rettv->vval.v_সংখ্যা = tv_islocked(lv.ll_tv);
	    }
	    অন্যথা যদি (lv.ll_object != ফাঁকা)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_সংখ্যা = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(ফাঁকা, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#endif
	    }
	    অন্যথা যদি (lv.ll_class != ফাঁকা)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_সংখ্যা = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(ফাঁকা, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#endif
	    }
	    অন্যথা যদি (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    অন্যথা যদি (lv.ll_newkey != ফাঁকা)
		semsg(_(e_চাবি_not_present_in_dictionary_str), lv.ll_newkey);
	    অন্যথা যদি (lv.ll_list != ফাঁকা)
		// List item.
		rettv->vval.v_সংখ্যা = tv_islocked(&lv.ll_li->li_tv);
	    অন্যথা যদি (lv.ll_tuple != ফাঁকা)
		// Tuple item.
		rettv->vval.v_সংখ্যা = tv_islocked(lv.ll_tv);
	    অন্যথা
		// Dictionary item.
		rettv->vval.v_সংখ্যা = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    যদি (মূল != ফাঁকা)
	free_lval_মূল(মূল);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    স্থির শূন্য
f_বস্তুসমূহ(typval_T *argvars, typval_T *rettv)
{
    যদি (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    অন্যথা
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), 1);
}

/*
 * "keytrans()" function
 */
    স্থির শূন্য
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    যদি (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == ফাঁকা)
	ফেরত;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    স্থির শূন্য
f_last_বাফার_nr(typval_T *argvars UNUSED, typval_T *rettv)
{
    পূর্ণসংখ্যা		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	যদি (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_সংখ্যা = n;
}

/*
 * "len()" function
 */
    শূন্য
f_len(typval_T *argvars, typval_T *rettv)
{
    নির্বাচন (argvars[0].v_type)
    {
	ক্ষেত্র VAR_STRING:
	ক্ষেত্র VAR_সংখ্যা:
	    rettv->vval.v_সংখ্যা = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    বিরতি;
	ক্ষেত্র VAR_BLOB:
	    rettv->vval.v_সংখ্যা = blob_len(argvars[0].vval.v_blob);
	    বিরতি;
	ক্ষেত্র VAR_LIST:
	    rettv->vval.v_সংখ্যা = list_len(argvars[0].vval.v_list);
	    বিরতি;
	ক্ষেত্র VAR_TUPLE:
	    rettv->vval.v_সংখ্যা = tuple_len(argvars[0].vval.v_tuple);
	    বিরতি;
	ক্ষেত্র VAR_DICT:
	    rettv->vval.v_সংখ্যা = dict_len(argvars[0].vval.v_dict);
	    বিরতি;
	ক্ষেত্র VAR_OBJECT:
	    rettv->vval.v_সংখ্যা = object_len(argvars[0].vval.v_object);
	    বিরতি;
	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_VOID:
	ক্ষেত্র VAR_BOOL:
	ক্ষেত্র VAR_SPECIAL:
	ক্ষেত্র VAR_FLOAT:
	ক্ষেত্র VAR_FUNC:
	ক্ষেত্র VAR_PARTIAL:
	ক্ষেত্র VAR_JOB:
	ক্ষেত্র VAR_CHANNEL:
	ক্ষেত্র VAR_INSTR:
	ক্ষেত্র VAR_CLASS:
	ক্ষেত্র VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    বিরতি;
    }
}

    স্থির শূন্য
libcall_common(typval_T *argvars UNUSED, typval_T *rettv, পূর্ণসংখ্যা type)
{
#ifdef FEAT_LIBCALL
    char_u		*string_in;
    char_u		**string_ফলাফল;
    পূর্ণসংখ্যা			nr_ফলাফল;
#endif

    rettv->v_type = type;
    যদি (type != VAR_সংখ্যা)
	rettv->vval.v_string = ফাঁকা;

    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_সংখ্যা_arg(argvars, 2) == FAIL))
	ফেরত;

#ifdef FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    যদি (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_in = ফাঁকা;
	যদি (argvars[2].v_type == VAR_STRING)
	    string_in = argvars[2].vval.v_string;
	যদি (type == VAR_সংখ্যা)
	{
	    string_ফলাফল = ফাঁকা;
	}
	অন্যথা
	{
	    rettv->vval.v_string = ফাঁকা;
	    string_ফলাফল = &rettv->vval.v_string;
	}
	যদি (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_in,
			     argvars[2].vval.v_সংখ্যা,
			     string_ফলাফল,
			     &nr_ফলাফল) == OK
		&& type == VAR_সংখ্যা)
	    rettv->vval.v_সংখ্যা = nr_ফলাফল;
    }
#endif
}

/*
 * "libcall()" function
 */
    স্থির শূন্য
f_libcall(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_STRING);
}

/*
 * "libcallnr()" function
 */
    স্থির শূন্য
f_libcallnr(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_সংখ্যা);
}

/*
 * "line(string, [winid])" function
 */
    স্থির শূন্য
f_সারি(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum = 0;
    pos_T	*fp = ফাঁকা;
    পূর্ণসংখ্যা		fnum;
    পূর্ণসংখ্যা		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	যদি (wp != ফাঁকা && tp != ফাঁকা)
	{
	    যদি (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		যদি (*p_spk != 'c'
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
    অন্যথা
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    যদি (fp != ফাঁকা)
	lnum = fp->lnum;
    rettv->vval.v_সংখ্যা = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    স্থির শূন্য
f_line2byte(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_সংখ্যা = -1;
#অন্যথা
    linenr_T	lnum;

    যদি (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ফেরত;

    lnum = tv_get_lnum(argvars);
    যদি (lnum < 1 || lnum > curbuf->b_ml.ml_সারি_গুনতি + 1)
	rettv->vval.v_সংখ্যা = -1;
    অন্যথা
	rettv->vval.v_সংখ্যা = ml_find_সারি_or_offset(curbuf, lnum, ফাঁকা);
    যদি (rettv->vval.v_সংখ্যা >= 0)
	++rettv->vval.v_সংখ্যা;
#endif
}

#ifdef FEAT_LUA
/*
 * "luaeval()" function
 */
    স্থির শূন্য
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#endif

প্রকারলেখ গণনা
{
    MATCH_END,	    // matchend()
    MATCH_MATCH,    // match()
    MATCH_STR,	    // matchstr()
    MATCH_LIST,	    // matchlist()
    MATCH_POS	    // matchstrpos()
} matchtype_T;

    স্থির শূন্য
find_some_match(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = ফাঁকা;
    long	len = 0;
    char_u	*রাশিমালা = ফাঁকা;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    long	start = 0;
    long	nth = 1;
    colnr_T	startcol = 0;
    পূর্ণসংখ্যা		match = 0;
    list_T	*l = ফাঁকা;
    listitem_T	*li = ফাঁকা;
    long	idx = 0;
    পূর্ণসংখ্যা		পূর্ব_lock = 0;
    char_u	*tofree = ফাঁকা;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    rettv->vval.v_সংখ্যা = -1;
    যদি (type == MATCH_LIST || type == MATCH_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	যদি (rettv_list_alloc(rettv) == FAIL)
	    যাও theend;
	যদি (type == MATCH_POS
		&& (list_append_string(rettv->vval.v_list,
					    (char_u *)"", 0) == FAIL
		    || list_append_সংখ্যা(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_সংখ্যা(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_সংখ্যা(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL))
	{
		list_free(rettv->vval.v_list);
		rettv->vval.v_list = ফাঁকা;
		যাও theend;
	}
    }
    অন্যথা যদি (type == MATCH_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ফাঁকা;
    }

    যদি (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 3) == FAIL)))
	যাও theend;

    যদি (argvars[0].v_type == VAR_LIST)
    {
	যদি ((l = argvars[0].vval.v_list) == ফাঁকা)
	    যাও theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    অন্যথা
    {
	রাশিমালা = str = tv_get_string(&argvars[0]);
	len = (long)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    যদি (pat == ফাঁকা)
	যাও theend;

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	পূর্ণসংখ্যা	    error = FALSE;

	start = (long)tv_get_সংখ্যা_chk(&argvars[2], &error);
	যদি (error)
	    যাও theend;
	যদি (l != ফাঁকা)
	{
	    li = list_find(l, start);
	    যদি (li == ফাঁকা)
		যাও theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	অন্যথা
	{
	    যদি (start < 0)
		start = 0;
	    যদি (start > len)
		যাও theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    যদি (argvars[3].v_type != VAR_UNKNOWN)
		startcol = start;
	    অন্যথা
	    {
		str += start;
		len -= start;
	    }
	}

	যদি (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (long)tv_get_সংখ্যা_chk(&argvars[3], &error);
	যদি (error)
	    যাও theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    যদি (regmatch.regprog != ফাঁকা)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	যদি (l != ফাঁকা)
	{
	    পূর্ব_lock = l->lv_lock;
	    যদি (l->lv_lock == 0)
		l->lv_lock = VAR_LOCKED;
	}

	জন্য (;;)
	{
	    যদি (l != ফাঁকা)
	    {
		যদি (li == ফাঁকা)
		{
		    match = FALSE;
		    বিরতি;
		}
		vim_free(tofree);
		রাশিমালা = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		যদি (str == ফাঁকা)
		    বিরতি;
	    }

	    match = vim_regexec_nl(&regmatch, str, startcol);

	    যদি (match && --nth <= 0)
		বিরতি;
	    যদি (l == ফাঁকা && !match)
		বিরতি;

	    // Advance to just after the match.
	    যদি (l != ফাঁকা)
	    {
		li = li->li_পরবর্তী;
		++idx;
	    }
	    অন্যথা
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		যদি (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    match = FALSE;
		    বিরতি;
		}
	    }
	}

	যদি (match)
	{
	    যদি (type == MATCH_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_first;
		listitem_T *li2 = li1->li_পরবর্তী;
		listitem_T *li3 = li2->li_পরবর্তী;
		listitem_T *li4 = li3->li_পরবর্তী;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_সংখ্যা =
				      (varnumber_T)(regmatch.startp[0] - রাশিমালা);
		li4->li_tv.vval.v_সংখ্যা =
					(varnumber_T)(regmatch.endp[0] - রাশিমালা);
		যদি (l != ফাঁকা)
		    li2->li_tv.vval.v_সংখ্যা = (varnumber_T)idx;
	    }
	    অন্যথা যদি (type == MATCH_LIST)
	    {
		পূর্ণসংখ্যা i;

		// return list with matched string and submatches
		জন্য (i = 0; i < NSUBEXP; ++i)
		{
		    যদি (regmatch.endp[i] == ফাঁকা)
		    {
			যদি (list_append_string(rettv->vval.v_list,
							    ফাঁকা, 0) == FAIL)
			    বিরতি;
		    }
		    অন্যথা যদি (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(পূর্ণসংখ্যা)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			বিরতি;
		}
	    }
	    অন্যথা যদি (type == MATCH_STR)
	    {
		// return matched string
		যদি (l != ফাঁকা)
		    copy_tv(&li->li_tv, rettv);
		অন্যথা
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    অন্যথা যদি (l != ফাঁকা)
		rettv->vval.v_সংখ্যা = idx;
	    অন্যথা
	    {
		যদি (type != MATCH_END)
		    rettv->vval.v_সংখ্যা =
				      (varnumber_T)(regmatch.startp[0] - str);
		অন্যথা
		    rettv->vval.v_সংখ্যা =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_সংখ্যা += (varnumber_T)(str - রাশিমালা);
	    }
	}
	যদি (l != ফাঁকা)
	    l->lv_lock = পূর্ব_lock;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    যদি (type == MATCH_POS && l == ফাঁকা && rettv->vval.v_list != ফাঁকা)
	// matchstrpos() without a list: drop the second item.
	listitem_remove(rettv->vval.v_list,
				       rettv->vval.v_list->lv_first->li_পরবর্তী);
    vim_free(tofree);
    p_cpo = save_cpo;
}

/*
 * Return all the matches in string "str" for pattern "rmp".
 * The matches are returned in the List "mlist".
 * If "submatches" is TRUE, then submatch information is also returned.
 * "matchbuf" is TRUE when called for matchbufline().
 */
    স্থির পূর্ণসংখ্যা
get_matches_in_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    পূর্ণসংখ্যা		idx,
    পূর্ণসংখ্যা		submatches,
    পূর্ণসংখ্যা		matchbuf)
{
    long	len = (long)STRLEN(str);
    পূর্ণসংখ্যা		match = 0;
    colnr_T	startidx = 0;

    জন্য (;;)
    {
	match = vim_regexec_nl(rmp, str, startidx);
	যদি (!match)
	    বিরতি;

	dict_T *d = dict_alloc();
	যদি (d == ফাঁকা)
	    ফেরত FAIL;
	যদি (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    ফেরত FAIL;
	}

	যদি (dict_add_সংখ্যা(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    ফেরত FAIL;

	যদি (dict_add_সংখ্যা(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    ফেরত FAIL;

	যদি (dict_add_string_len(d, "text", rmp->startp[0],
		    (পূর্ণসংখ্যা)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    ফেরত FAIL;

	যদি (submatches)
	{
	    list_T *sml = list_alloc();
	    যদি (sml == ফাঁকা)
		ফেরত FAIL;

	    যদি (dict_add_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		ফেরত FAIL;
	    }

	    // return a list with the submatches
	    জন্য (পূর্ণসংখ্যা i = 1; i < NSUBEXP; ++i)
	    {
		যদি (rmp->endp[i] == ফাঁকা)
		{
		    যদি (list_append_string(sml, (char_u *)"", 0) == FAIL)
			ফেরত FAIL;
		}
		অন্যথা যদি (list_append_string(sml, rmp->startp[i],
			    (পূর্ণসংখ্যা)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    ফেরত FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	যদি (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    বিরতি;
    }

    ফেরত OK;
}

/*
 * "matchbufline()" function
 */
    স্থির শূন্য
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ফাঁকা;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_সংখ্যা = -1;
    যদি (rettv_list_alloc(rettv) != OK)
	ফেরত;
    retlist = rettv->vval.v_list;

    যদি (check_for_বাফার_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	ফেরত;

    পূর্ণসংখ্যা পূর্ব_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    যদি (buf == ফাঁকা)
    {
	যদি (did_emsg == পূর্ব_did_emsg)
	    semsg(_(e_invalid_বাফার_name_str), tv_get_string(&argvars[0]));
	ফেরত;
    }
    যদি (buf->b_ml.ml_mfp == ফাঁকা)
    {
	emsg(_(e_বাফার_is_not_loaded));
	ফেরত;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    পূর্ণসংখ্যা		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    যদি (did_emsg > did_emsg_before)
	ফেরত;
    যদি (slnum < 1)
    {
	semsg(_(e_invalid_মান_for_argument_str), "lnum");
	ফেরত;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    যদি (did_emsg > did_emsg_before)
	ফেরত;
    যদি (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_মান_for_argument_str), "end_lnum");
	ফেরত;
    }

    যদি (elnum > buf->b_ml.ml_সারি_গুনতি)
	elnum = buf->b_ml.ml_সারি_গুনতি;

    পূর্ণসংখ্যা		submatches = FALSE;
    যদি (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	যদি (d != ফাঁকা)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    যদি (di != ফাঁকা)
	    {
		যদি (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "submatches");
		    ফেরত;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    যদি (regmatch.regprog == ফাঁকা)
	যাও theend;
    regmatch.rm_ic = p_ic;

    যখন (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	যদি (get_matches_in_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    যাও cleanup;
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
    স্থির শূন্য
f_match(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_MATCH);
}

/*
 * "matchend()" function
 */
    স্থির শূন্য
f_matchend(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_END);
}

/*
 * "matchlist()" function
 */
    স্থির শূন্য
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_LIST);
}

/*
 * "matchstr()" function
 */
    স্থির শূন্য
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_STR);
}

/*
 * "matchstrlist()" function
 */
    স্থির শূন্য
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ফাঁকা;
    char_u	*save_cpo;
    list_T	*l = ফাঁকা;
    listitem_T	*li = ফাঁকা;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_সংখ্যা = -1;
    যদি (rettv_list_alloc(rettv) != OK)
	ফেরত;
    retlist = rettv->vval.v_list;

    যদি (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ফেরত;

    যদি ((l = argvars[0].vval.v_list) == ফাঁকা)
	ফেরত;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    যদি (pat == ফাঁকা)
	ফেরত;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    যদি (regmatch.regprog == ফাঁকা)
	যাও theend;
    regmatch.rm_ic = p_ic;

    পূর্ণসংখ্যা		submatches = FALSE;
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	যদি (d != ফাঁকা)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    যদি (di != ফাঁকা)
	    {
		যদি (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_মান_for_argument_str), "submatches");
		    যাও cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    পূর্ণসংখ্যা idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	যদি (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != ফাঁকা)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    যদি (get_matches_in_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		যাও cleanup;
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
    স্থির শূন্য
f_matchstrpos(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_POS);
}

    স্থির শূন্য
max_min(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা domax)
{
    varnumber_T	n = 0;

    যদি (in_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->vval.v_সংখ্যা = 0;

    যদি (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = ফাঁকা;

	l = argvars[0].vval.v_list;
	যদি (l != ফাঁকা && l->lv_len > 0)
	{
	    যদি (l->lv_first == &range_list_item)
	    {
		যদি ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_start;
		অন্যথা
		    n = l->lv_u.nonmat.lv_start + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_সংখ্যা = n;
	    }
	    অন্যথা
	    {
		FOR_ALL_LIST_বস্তুসমূহ(l, li)
		{
		    যদি (tv == ফাঁকা)
			tv = &li->li_tv;
		    অন্যথা
		    {
			পূর্ণসংখ্যা res;
			যদি (typval_compare2(&li->li_tv, tv,
				domax ? রাশিমালা_GREATER : রাশিমালা_SMALLER, FALSE, &res) == FAIL)
			    ফেরত;
			যদি (res == OK)
			    tv = &li->li_tv;
		    }
		}

		যদি (tv != ফাঁকা)
		    copy_tv(tv, rettv);
	    }
	}
    }
    অন্যথা যদি (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	যদি (tv != ফাঁকা)
	    copy_tv(tv, rettv);
    }
    অন্যথা যদি (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	পূর্ণসংখ্যা		todo;
	typval_T	*tv = ফাঁকা;

	d = argvars[0].vval.v_dict;
	যদি (d != ফাঁকা)
	{
	    todo = (পূর্ণসংখ্যা)d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_বস্তুসমূহ(&d->dv_hashtab, hi, todo)
	    {
		যদি (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    যদি (tv == ফাঁকা)
			tv = &HI2DI(hi)->di_tv;
		    অন্যথা
		    {
			পূর্ণসংখ্যা res;
			যদি (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? রাশিমালা_GREATER : রাশিমালা_SMALLER, FALSE, &res) == FAIL)
			    ফেরত;
			যদি (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	যদি (tv != ফাঁকা)
	    copy_tv(tv, rettv);
    }
    অন্যথা
	semsg(_(e_argument_of_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
}

/*
 * "max()" function
 */
    স্থির শূন্য
f_max(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, TRUE);
}

/*
 * "min()" function
 */
    স্থির শূন্য
f_min(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, FALSE);
}

#যদি defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    স্থির শূন্য
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    শূন্য
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
    স্থির শূন্য
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    যদি (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ফেরত;

    জন্য (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	যদি (lnum < 0 || lnum > curbuf->b_ml.ml_সারি_গুনতি)
	{
	    lnum = 0;
	    বিরতি;
	}
	যদি (*skipwhite(ml_get(lnum)) != NUL)
	    বিরতি;
    }
    rettv->vval.v_সংখ্যা = lnum;
}


/*
 * "ngettext()" function
 */
    স্থির শূন্য
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#যদি defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    অক্ষর *পূর্ব = ফাঁকা;
#endif

    যদি (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_সংখ্যা_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	ফেরত;

    rettv->v_type = VAR_STRING;

    যদি (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != ফাঁকা &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#যদি defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	পূর্ব = bind_textdomain_codeset((ধ্রুবক অক্ষর *)argvars[3].vval.v_string, (অক্ষর *)p_enc);
#endif

#যদি defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((ধ্রুবক অক্ষর *)argvars[3].vval.v_string, (ধ্রুবক অক্ষর *)argvars[0].vval.v_string, (ধ্রুবক অক্ষর *)argvars[1].vval.v_string, (পূর্ণসংখ্যা)argvars[2].vval.v_সংখ্যা));
#অন্যথা
	textdomain((ধ্রুবক অক্ষর *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ধ্রুবক অক্ষর *)argvars[0].vval.v_string, (ধ্রুবক অক্ষর *)argvars[1].vval.v_string, argvars[2].vval.v_সংখ্যা));
	textdomain(VIMPACKAGE);
#endif

#যদি defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	যদি (পূর্ব != ফাঁকা)
	    bind_textdomain_codeset((ধ্রুবক অক্ষর *)argvars[3].vval.v_string, পূর্ব);
#endif
    }
    অন্যথা
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ধ্রুবক অক্ষর *)argvars[0].vval.v_string, (ধ্রুবক অক্ষর *)argvars[1].vval.v_string, argvars[2].vval.v_সংখ্যা));
}


/*
 * "nr2char()" function
 */
    স্থির শূন্য
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (has_mbyte)
    {
	পূর্ণসংখ্যা	utf8 = 0;

	যদি (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[1], ফাঁকা);
	যদি (utf8)
	    buf[utf_char2bytes((পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[0]), buf)] = NUL;
	অন্যথা
	    buf[(*mb_char2bytes)((পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[0]), buf)] = NUL;
    }
    অন্যথা
    {
	buf[0] = (char_u)tv_get_সংখ্যা(&argvars[0]);
	buf[1] = NUL;
    }
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "or(expr, expr)" function
 */
    স্থির শূন্য
f_or(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    rettv->vval.v_সংখ্যা = tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা)
					| tv_get_সংখ্যা_chk(&argvars[1], ফাঁকা);
}

#ifdef FEAT_PERL
/*
 * "perleval()" function
 */
    স্থির শূন্য
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#endif

/*
 * "prevnonblank()" function
 */
    স্থির শূন্য
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    যদি (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ফেরত;

    lnum = tv_get_lnum(argvars);
    যদি (lnum < 1 || lnum > curbuf->b_ml.ml_সারি_গুনতি)
	lnum = 0;
    অন্যথা
	যখন (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
	    --lnum;
    rettv->vval.v_সংখ্যা = lnum;
}

// This dummy va_list is here because:
// - passing a NULL pointer doesn't work when va_list isn't a pointer
// - locally in the function results in a "used before set" warning
// - using va_start() to initialize it gives "function with fixed args" error
স্থির va_list	ap;

/*
 * "printf()" function
 */
    স্থির শূন্য
f_printf(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    পূর্ণসংখ্যা		len;
    char_u	*s;
    পূর্ণসংখ্যা		saved_did_emsg = did_emsg;
    অক্ষর	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script() && check_for_string_or_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (অক্ষর *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(ফাঁকা, 0, fmt, ap, argvars + 1);
    যদি (!did_emsg)
    {
	s = alloc(len + 1);
	যদি (s != ফাঁকা)
	{
	    rettv->vval.v_string = s;
	    (শূন্য)vim_vsnprintf_typval((অক্ষর *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    স্থির শূন্য
f_pum_getpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;
    pum_set_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    স্থির শূন্য
f_pumvisible(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    যদি (pum_visible())
	rettv->vval.v_সংখ্যা = 1;
}

#ifdef FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    স্থির শূন্য
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (p_pyx == 0)
	p_pyx = 3;

    যদি (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    অন্যথা যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	ফেরত;
    }
    অন্যথা
    {
	locals = ফাঁকা;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#endif

#ifdef FEAT_PYTHON
/*
 * "pyeval()" function
 */
    স্থির শূন্য
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	ফেরত;

    যদি (p_pyx == 0)
	p_pyx = 2;

    যদি (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    অন্যথা যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	ফেরত;
    }
    অন্যথা
    {
	locals = ফাঁকা;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#endif

#যদি defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    স্থির শূন্য
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ফেরত;

# যদি defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    যদি (p_pyx == 2)
	f_pyeval(argvars, rettv);
    অন্যথা
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#endif

স্থির UINT32_T srand_seed_for_testing = 0;
স্থির পূর্ণসংখ্যা	srand_seed_for_testing_is_used = FALSE;

    স্থির শূন্য
f_test_srand_seed(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
	srand_seed_for_testing_is_used = FALSE;
    অন্যথা
    {
	srand_seed_for_testing = (UINT32_T)tv_get_সংখ্যা(&argvars[0]);
	srand_seed_for_testing_is_used = TRUE;
    }
}

    স্থির শূন্য
init_srand(UINT32_T *x)
{
    কাঠামো {
	সংঘ {
	    UINT32_T সংখ্যা;
	    char_u   bytes[আকার(UINT32_T)];
	} contents;
    } buf;

    যদি (srand_seed_for_testing_is_used)
    {
	*x = srand_seed_for_testing;
	ফেরত;
    }

    যদি (mch_get_random(buf.contents.bytes, আকার(buf.contents.bytes)) == OK)
    {
	*x = buf.contents.সংখ্যা;
	ফেরত;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#যদি defined(FEAT_SODIUM)
    যদি (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    অন্যথা
#endif
    {
#যদি defined(FEAT_RELTIME)
	proftime_T res;
	profile_start(&res);
# যদি defined(MSWIN)
	*x = (UINT32_T)res.নিচের_Part;
# অন্যথা
	*x = (UINT32_T)res.tv_fsec;
# endif
#অন্যথা
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
    ফলাফল = ROTL((y) * 5, 7) * 9; \
    t = (y) << 9; \
    (z) ^= (x); \
    (w) ^= (y); \
    (y) ^= (z), (x) ^= (w); \
    (z) ^= t; \
    (w) = ROTL(w, 11);

/*
 * "rand()" function
 */
    স্থির শূন্য
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = ফাঁকা;
    স্থির UINT32_T	gx, gy, gz, gw;
    স্থির পূর্ণসংখ্যা	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, ফলাফল;

    যদি (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
    {
	// When no argument is given use the global seed list.
	যদি (initialized == FALSE)
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
    অন্যথা যদি (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	যদি (l == ফাঁকা || list_len(l) != 4)
	    যাও theend;

	lx = list_find(l, 0L);
	ly = list_find(l, 1L);
	lz = list_find(l, 2L);
	lw = list_find(l, 3L);
	যদি (lx == ফাঁকা || ly == ফাঁকা || lz == ফাঁকা || lw == ফাঁকা)
	    যাও theend;
	যদি (lx->li_tv.v_type != VAR_সংখ্যা
		|| ly->li_tv.v_type != VAR_সংখ্যা
		|| lz->li_tv.v_type != VAR_সংখ্যা
		|| lw->li_tv.v_type != VAR_সংখ্যা)
	    যাও theend;
	x = (UINT32_T)lx->li_tv.vval.v_সংখ্যা;
	y = (UINT32_T)ly->li_tv.vval.v_সংখ্যা;
	z = (UINT32_T)lz->li_tv.vval.v_সংখ্যা;
	w = (UINT32_T)lw->li_tv.vval.v_সংখ্যা;

	SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w);

	lx->li_tv.vval.v_সংখ্যা = (varnumber_T)x;
	ly->li_tv.vval.v_সংখ্যা = (varnumber_T)y;
	lz->li_tv.vval.v_সংখ্যা = (varnumber_T)z;
	lw->li_tv.vval.v_সংখ্যা = (varnumber_T)w;
    }
    অন্যথা
	যাও theend;

    rettv->v_type = VAR_সংখ্যা;
    rettv->vval.v_সংখ্যা = (varnumber_T)ফলাফল;
    ফেরত;

theend:
    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    rettv->v_type = VAR_সংখ্যা;
    rettv->vval.v_সংখ্যা = -1;
}

/*
 * "srand()" function
 */
    স্থির শূন্য
f_srand(typval_T *argvars, typval_T *rettv)
{
    UINT32_T x = 0, z;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    অন্যথা
    {
	পূর্ণসংখ্যা	    error = FALSE;

	x = (UINT32_T)tv_get_সংখ্যা_chk(&argvars[0], &error);
	যদি (error)
	    ফেরত;
    }

    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#undef ROTL
#undef SPLITMIX32
#undef SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    স্থির শূন্য
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	start;
    varnumber_T	end;
    varnumber_T	stride = 1;
    পূর্ণসংখ্যা		error = FALSE;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL)))
	ফেরত;

    start = tv_get_সংখ্যা_chk(&argvars[0], &error);
    যদি (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = start - 1;
	start = 0;
    }
    অন্যথা
    {
	end = tv_get_সংখ্যা_chk(&argvars[1], &error);
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_সংখ্যা_chk(&argvars[2], &error);
    }

    যদি (error)
	ফেরত;		// type error; errmsg already given
    যদি (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	ফেরত;
    }

    // The stride is stored in "lv_stride", which is an int.
    যদি (stride < INT_MIN || stride > INT_MAX)
    {
	অক্ষর	buf[NUMBUFLEN];

	vim_snprintf(buf, আকার(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	ফেরত;
    }

    uvarnumber_T	len;

    যদি (stride > 0 ? end < start : end > start)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	back = stride > 0
				  ? (uvarnumber_T)start - (uvarnumber_T)end
				  : (uvarnumber_T)end - (uvarnumber_T)start;

	যদি (back > 1)
	{
	    emsg(_(e_start_past_end));
	    ফেরত;
	}
	len = 0;
    }
    অন্যথা
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	span = stride > 0
				  ? (uvarnumber_T)end - (uvarnumber_T)start
				  : (uvarnumber_T)start - (uvarnumber_T)end;
	uvarnumber_T	গুনতি = span / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	যদি (গুনতি >= (uvarnumber_T)INT_MAX)
	{
	    অক্ষর	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, আকার(buf), "%llu",
			      গুনতি < UVARNUM_MAX ? গুনতি + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    ফেরত;
	}
	len = গুনতি + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_first = &range_list_item;
    list->lv_u.nonmat.lv_start = start;
    list->lv_u.nonmat.lv_end = end;
    list->lv_u.nonmat.lv_stride = (পূর্ণসংখ্যা)stride;
    list->lv_len = (পূর্ণসংখ্যা)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    শূন্য
range_list_materialize(list_T *list)
{
    varnumber_T start = list->lv_u.nonmat.lv_start;
    varnumber_T end = list->lv_u.nonmat.lv_end;
    পূর্ণসংখ্যা		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_first = ফাঁকা;
    list->lv_u.mat.lv_last = ফাঁকা;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_item = ফাঁকা;
    জন্য (i = start; stride > 0 ? i <= end : i >= end; i += stride)
    {
	যদি (list_append_সংখ্যা(list, i) == FAIL)
	    বিরতি;
	যদি (list->lv_lock & VAR_বস্তুসমূহ_LOCKED)
	    list->lv_u.mat.lv_last->li_tv.v_lock = VAR_LOCKED;
    }
    list->lv_lock &= ~VAR_বস্তুসমূহ_LOCKED;
}

/*
 * "getreginfo()" function
 */
    স্থির শূন্য
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		regname;
    char_u	buf[NUMBUFLEN + 2];
    size_t	buflen;
    long	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    যদি (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ফেরত;

    regname = getreg_get_regname(argvars);
    যদি (regname == 0)
	ফেরত;

    যদি (regname == '@')
	regname = '"';

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_রাশিমালা_SRC | GREG_LIST);
    যদি (list == ফাঁকা)
	ফেরত;
    যদি (dict_add_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    নির্বাচন (get_reg_type(regname, &reglen))
    {
	ক্ষেত্র MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    বিরতি;
	ক্ষেত্র MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    বিরতি;
	ক্ষেত্র MBLOCK:
	    buflen = vim_snprintf_safelen((অক্ষর *)buf, আকার(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    বিরতি;
	পূর্বনির্ধারিত:
	    buf[0] = NUL;
	    buflen = 0;
	    বিরতি;
    }
    (শূন্য)dict_add_string_len(dict, (অক্ষর *)"regtype", buf, (পূর্ণসংখ্যা)buflen);

    buf[0] = get_register_name(get_unname_register());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    যদি (regname == '"')
	(শূন্য)dict_add_string_len(dict, (অক্ষর *)"points_to", buf, (পূর্ণসংখ্যা)buflen);
    অন্যথা
    {
	dictitem_T	*item = dictitem_alloc((char_u *)"isunnamed");

	যদি (item != ফাঁকা)
	{
	    item->di_tv.v_type = VAR_BOOL;
	    item->di_tv.vval.v_সংখ্যা = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (শূন্য)dict_add(dict, item);
	}
    }
}

    স্থির শূন্য
return_register(পূর্ণসংখ্যা regname, typval_T *rettv)
{
    char_u buf[2] = {NUL, NUL};

    buf[0] = (char_u)regname;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(buf, (buf[0] == NUL) ? 0 : 1);
}

/*
 * "reg_executing()" function
 */
    স্থির শূন্য
f_reg_executing(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    স্থির শূন্য
f_reg_recording(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    স্থির শূন্য
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_সংখ্যা = -1;
    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    rettv->vval.v_সংখ্যা = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    স্থির শূন্য
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    পূর্ণসংখ্যা slen = list_len(l);

    যদি (rettv_list_alloc(rettv) == FAIL
	    || l == ফাঁকা
	    || n <= 0
	    || slen == 0)
	ফেরত;

    যদি (check_repeat_গুনতি(slen, n) == FAIL)
	ফেরত;

    যখন (n-- > 0)
	যদি (list_extend(rettv->vval.v_list, l, ফাঁকা) == FAIL)
	    বিরতি;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    স্থির শূন্য
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    পূর্ণসংখ্যা		slen;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    যদি (rettv_blob_alloc(rettv) == FAIL
	    || blob == ফাঁকা
	    || n <= 0)
	ফেরত;

    slen = blob->bv_ga.ga_len;
    যদি (slen <= 0)
	ফেরত;
    যদি (check_repeat_গুনতি(slen, n) == FAIL)
	ফেরত;
    len = slen * (পূর্ণসংখ্যা)n;

    যদি (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	ফেরত;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    জন্য (i = 0; i < slen; ++i)
	যদি (blob_get(blob, i) != 0)
	    বিরতি;

    যদি (i == slen)
	// No need to copy since all bytes are already zero
	ফেরত;

    জন্য (i = 0; i < (পূর্ণসংখ্যা)n; ++i)
	blob_set_range(rettv->vval.v_blob,
		(long)i * slen, ((long)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    স্থির শূন্য
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    size_t	slen;
    পূর্ণসংখ্যা		len;
    char_u	*r;
    পূর্ণসংখ্যা		done;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    slen = STRLEN(p);
    যদি (slen == 0 || n <= 0)
	ফেরত;
    যদি (check_repeat_গুনতি((varnumber_T)slen, n) == FAIL)
	ফেরত;
    len = (পূর্ণসংখ্যা)slen * (পূর্ণসংখ্যা)n;

    r = alloc((size_t)len + 1);
    যদি (r == ফাঁকা)
	ফেরত;

    mch_memmove(r, p, slen);
    done = (পূর্ণসংখ্যা)slen;
    যখন (done < len)
    {
	পূর্ণসংখ্যা copy_len = done;

	যদি (copy_len > len - done)
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
    স্থির শূন্য
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    যদি (in_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    n = tv_get_সংখ্যা(&argvars[1]);
    যদি (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    অন্যথা
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
    স্থির পূর্ণসংখ্যা
get_অনুসন্ধান_arg(typval_T *varp, পূর্ণসংখ্যা *flagsp)
{
    পূর্ণসংখ্যা		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    পূর্ণসংখ্যা		mask;

    যদি (varp->v_type == VAR_UNKNOWN)
	ফেরত FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    যদি (flags == ফাঁকা)
	ফেরত 0;		// type error; errmsg already given
    যখন (*flags != NUL)
    {
	নির্বাচন (*flags)
	{
	    ক্ষেত্র 'b': dir = BACKWARD; বিরতি;
	    ক্ষেত্র 'w': p_ws = TRUE; বিরতি;
	    ক্ষেত্র 'W': p_ws = FALSE; বিরতি;
	    পূর্বনির্ধারিত:  mask = 0;
		      যদি (flagsp != ফাঁকা)
			  নির্বাচন (*flags)
			  {
			      ক্ষেত্র 'c': mask = SP_START; বিরতি;
			      ক্ষেত্র 'e': mask = SP_END; বিরতি;
			      ক্ষেত্র 'm': mask = SP_RETCOUNT; বিরতি;
			      ক্ষেত্র 'n': mask = SP_NOMOVE; বিরতি;
			      ক্ষেত্র 'p': mask = SP_SUBPAT; বিরতি;
			      ক্ষেত্র 'r': mask = SP_REPEAT; বিরতি;
			      ক্ষেত্র 's': mask = SP_SETPCMARK; বিরতি;
			      ক্ষেত্র 'z': mask = SP_COLUMN; বিরতি;
			  }
		      যদি (mask == 0)
		      {
			  semsg(_(e_invalid_argument_str), flags);
			  dir = 0;
		      }
		      অন্যথা
			  *flagsp |= mask;
	}
	যদি (dir == 0)
	    বিরতি;
	++flags;
    }
    ফেরত dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    স্থির পূর্ণসংখ্যা
অনুসন্ধান_cmn(typval_T *argvars, pos_T *match_pos, পূর্ণসংখ্যা *flagsp)
{
    পূর্ণসংখ্যা		flags;
    char_u	*pat;
    size_t	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    পূর্ণসংখ্যা		save_p_ws = p_ws;
    পূর্ণসংখ্যা		dir;
    পূর্ণসংখ্যা		retval = 0;	// default: FAIL
    long	lnum_stop = 0;
#ifdef FEAT_RELTIME
    long	time_limit = 0;
#endif
    পূর্ণসংখ্যা		options = অনুসন্ধান_KEEP;
    পূর্ণসংখ্যা		subpatnum;
    searchit_arg_T sia;
    পূর্ণসংখ্যা		use_skip = FALSE;
    pos_T	firstpos;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_সংখ্যা_arg(argvars, 3) == FAIL)))))
	যাও theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_অনুসন্ধান_arg(&argvars[1], flagsp);	// may set p_ws
    যদি (dir == 0)
	যাও theend;
    flags = *flagsp;
    যদি (flags & SP_START)
	options |= অনুসন্ধান_START;
    যদি (flags & SP_END)
	options |= অনুসন্ধান_END;
    যদি (flags & SP_COLUMN)
	options |= অনুসন্ধান_COL;

    // Optional arguments: line number to stop searching, timeout and skip.
    যদি (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (long)tv_get_সংখ্যা_chk(&argvars[2], ফাঁকা);
	যদি (lnum_stop < 0)
	    যাও theend;
	যদি (argvars[3].v_type != VAR_UNKNOWN)
	{
#ifdef FEAT_RELTIME
	    time_limit = (long)tv_get_সংখ্যা_chk(&argvars[3], ফাঁকা);
	    যদি (time_limit < 0)
		যাও theend;
#endif
	    use_skip = eval_রাশিমালা_valid_arg(&argvars[4]);
	}
    }

    /*
     * This function does not accept SP_REPEAT and SP_RETCOUNT flags.
     * Check to make sure only those flags are set.
     * Also, Only the SP_NOMOVE or the SP_SETPCMARK flag can be set. Both
     * flags cannot be set. Check for that condition also.
     */
    যদি (((flags & (SP_REPEAT | SP_RETCOUNT)) != 0)
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	যাও theend;
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
    জন্য (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, ফাঁকা, dir, pat, patlen, 1L,
						     options, RE_অনুসন্ধান, &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	যদি (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	যদি (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    বিরতি;
	যদি (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    পূর্ণসংখ্যা	    do_skip;
	    পূর্ণসংখ্যা	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_রাশিমালা_to_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    যদি (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		বিরতি;
	    }
	    যদি (!do_skip)
		বিরতি;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~অনুসন্ধান_START;
    }

    যদি (subpatnum != FAIL)
    {
	যদি (flags & SP_SUBPAT)
	    retval = subpatnum;
	অন্যথা
	    retval = pos.lnum;
	যদি (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	যদি (match_pos != ফাঁকা)
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
    যদি (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    অন্যথা
	curwin->w_set_curswant = true;
theend:
    p_ws = save_p_ws;

    ফেরত retval;
}

#ifdef FEAT_RUBY
/*
 * "rubyeval()" function
 */
    স্থির শূন্য
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#endif

/*
 * "screenattr()" function
 */
    স্থির শূন্য
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		row;
    পূর্ণসংখ্যা		col;
    পূর্ণসংখ্যা		c;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    row = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা) - 1;
    col = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[1], ফাঁকা) - 1;
    যদি (row < 0 || row >= screen_Rows
	    || col < 0 || col >= screen_Columns)
	c = -1;
    অন্যথা
	c = ScreenAttrs[সারি_Offset[row] + col];
    rettv->vval.v_সংখ্যা = c;
}

/*
 * "screenchar()" function
 */
    স্থির শূন্য
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		row;
    পূর্ণসংখ্যা		col;
    পূর্ণসংখ্যা		c;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    row = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা) - 1;
    col = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[1], ফাঁকা) - 1;
    যদি (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	c = -1;
    অন্যথা
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(row, col, buf, ফাঁকা);
	c = (*mb_ptr2char)(buf);
    }
    rettv->vval.v_সংখ্যা = c;
}

/*
 * "screenchars()" function
 */
    স্থির শূন্য
f_screenchars(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		row;
    পূর্ণসংখ্যা		col;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		i;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    row = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা) - 1;
    col = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[1], ফাঁকা) - 1;
    যদি (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	ফেরত;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(row, col, buf, ফাঁকা);
    পূর্ণসংখ্যা pcc[MAX_MCO];
    যদি (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    অন্যথা
	c = (*mb_ptr2char)(buf);
    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)c);

    যদি (enc_utf8)
	জন্য (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    স্থির শূন্য
f_screencol(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    স্থির শূন্য
f_screenrow(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    স্থির শূন্য
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		row;
    পূর্ণসংখ্যা		col;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = ফাঁকা;
    rettv->v_type = VAR_STRING;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    row = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা) - 1;
    col = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[1], ফাঁকা) - 1;
    যদি (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	ফেরত;

    screen_getbytes(row, col, buf, ফাঁকা);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    স্থির শূন্য
f_অনুসন্ধান(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		flags = 0;

    rettv->vval.v_সংখ্যা = অনুসন্ধান_cmn(argvars, ফাঁকা, &flags);
}

/*
 * "searchdecl()" function
 */
    স্থির শূন্য
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		locally = TRUE;
    পূর্ণসংখ্যা		thisblock = FALSE;
    পূর্ণসংখ্যা		error = FALSE;
    char_u	*name;

    rettv->vval.v_সংখ্যা = 1;	// default: FAIL

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ফেরত;

    name = tv_get_string_chk(&argvars[0]);
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[1], &error);
	যদি (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[2], &error);
    }
    যদি (!error && name != ফাঁকা)
	rettv->vval.v_সংখ্যা = find_decl(name, (পূর্ণসংখ্যা)STRLEN(name),
				     locally, thisblock, অনুসন্ধান_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    স্থির পূর্ণসংখ্যা
searchpair_cmn(typval_T *argvars, pos_T *match_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    পূর্ণসংখ্যা		save_p_ws = p_ws;
    পূর্ণসংখ্যা		dir;
    পূর্ণসংখ্যা		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    পূর্ণসংখ্যা		retval = 0;		// default: FAIL
    long	lnum_stop = 0;
    long	time_limit = 0;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_সংখ্যা_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_সংখ্যা_arg(argvars, 6) == FAIL)))))
	যাও theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    যদি (spat == ফাঁকা || mpat == ফাঁকা || epat == ফাঁকা)
	যাও theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_অনুসন্ধান_arg(&argvars[3], &flags); // may set p_ws
    যদি (dir == 0)
	যাও theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    যদি ((flags & (SP_END | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[3]));
	যাও theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    যদি (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    যদি (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = ফাঁকা;
    অন্যথা
    {
	// Type is checked later.
	skip = &argvars[4];

	যদি (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (long)tv_get_সংখ্যা_chk(&argvars[5], ফাঁকা);
	    যদি (lnum_stop < 0)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[5]));
		যাও theend;
	    }
#ifdef FEAT_RELTIME
	    যদি (argvars[6].v_type != VAR_UNKNOWN)
	    {
		time_limit = (long)tv_get_সংখ্যা_chk(&argvars[6], ফাঁকা);
		যদি (time_limit < 0)
		{
		    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[6]));
		    যাও theend;
		}
	    }
#endif
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    match_pos, lnum_stop, time_limit);

theend:
    p_ws = save_p_ws;

    ফেরত retval;
}

/*
 * "searchpair()" function
 */
    স্থির শূন্য
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = searchpair_cmn(argvars, ফাঁকা);
}

/*
 * "searchpairpos()" function
 */
    স্থির শূন্য
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    পূর্ণসংখ্যা		lnum = 0;
    পূর্ণসংখ্যা		col = 0;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (searchpair_cmn(argvars, &match_pos) > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)col);
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
    পূর্ণসংখ্যা		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    পূর্ণসংখ্যা		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*match_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    long	time_limit UNUSED) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = ফাঁকা, *pat3 = ফাঁকা;
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
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		r;
    পূর্ণসংখ্যা		nest = 1;
    পূর্ণসংখ্যা		use_skip = FALSE;
    পূর্ণসংখ্যা		err;
    পূর্ণসংখ্যা		options = অনুসন্ধান_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    যদি (pat2 == ফাঁকা)
	যাও theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    যদি (pat3 == ফাঁকা)
	যাও theend;
    pat2len = vim_snprintf((অক্ষর *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    যদি (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    অন্যথা
	pat3len = vim_snprintf((অক্ষর *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    যদি (flags & SP_START)
	options |= অনুসন্ধান_START;

    যদি (skip != ফাঁকা)
	use_skip = eval_রাশিমালা_valid_arg(skip);

#ifdef FEAT_RELTIME
    যদি (time_limit > 0)
	init_regexp_timeout(time_limit);
#endif
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    জন্য (;;)
    {
	searchit_arg_T sia;

	CLEAR_FIELD(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, ফাঁকা, dir, pat, patlen, 1L,
						     options, RE_অনুসন্ধান, &sia);
	যদি (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    বিরতি;

	যদি (firstpos.lnum == 0)
	    firstpos = pos;
	যদি (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    যদি (dir == BACKWARD)
		decl(&pos);
	    অন্যথা
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~অনুসন্ধান_START;

	// If the skip pattern matches, ignore this match.
	যদি (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_রাশিমালা_to_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    যদি (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		বিরতি;
	    }
	    যদি (r)
		চালু;
	}

	যদি ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	অন্যথা
	{
	    // Found end when searching forward or start when searching
	    // backward: end of (nested) pair; or found middle in outer pair.
	    যদি (--nest == 1)
		pat = pat3;	// outer level, search for middle
	}

	যদি (nest == 0)
	{
	    // Found the match: return matchcount or line number.
	    যদি (flags & SP_RETCOUNT)
		++retval;
	    অন্যথা
		retval = pos.lnum;
	    যদি (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    যদি (!(flags & SP_REPEAT))
		বিরতি;
	    nest = 1;	    // search for next unmatched
	}
    }

    যদি (match_pos != ফাঁকা)
    {
	// Store the match cursor position
	match_pos->lnum = curwin->w_cursor.lnum;
	match_pos->col = curwin->w_cursor.col + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    যদি ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#ifdef FEAT_RELTIME
    যদি (time_limit > 0)
	disable_regexp_timeout();
#endif
    vim_free(pat2);
    vim_free(pat3);
    যদি (p_cpo == empty_option)
	p_cpo = save_cpo;
    অন্যথা
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	যদি (*p_cpo == NUL)
	    set_option_মান_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    ফেরত retval;
}

/*
 * "searchpos()" function
 */
    স্থির শূন্য
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    পূর্ণসংখ্যা		lnum = 0;
    পূর্ণসংখ্যা		col = 0;
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		flags = 0;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    n = অনুসন্ধান_cmn(argvars, &match_pos, &flags);
    যদি (n > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)col);
    যদি (flags & SP_SUBPAT)
	list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    স্থির শূন্য
set_position(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা charpos)
{
    pos_T	pos;
    পূর্ণসংখ্যা		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ফেরত;

    name = tv_get_string_chk(argvars);
    যদি (name == ফাঁকা)
	ফেরত;

    যদি (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	ফেরত;

    যদি (pos.col != MAXCOL && --pos.col < 0)
	pos.col = 0;
    যদি ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	যদি (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_set_curswant = false;
	}
	check_cursor();
	rettv->vval.v_সংখ্যা = 0;
    }
    অন্যথা যদি (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	যদি (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_সংখ্যা = 0;
    }
    অন্যথা
	emsg(_(e_invalid_argument));
}
/*
 * "setcharpos()" function
 */
    স্থির শূন্য
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, TRUE);
}

    স্থির শূন্য
f_setcharsearch(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    যদি (check_for_dict_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি ((d = argvars[0].vval.v_dict) == ফাঁকা)
	ফেরত;

    csearch = dict_get_string(d, "char", FALSE);
    যদি (csearch != ফাঁকা)
    {
	যদি (enc_utf8)
	{
	    পূর্ণসংখ্যা pcc[MAX_MCO];
	    পূর্ণসংখ্যা c = utfc_ptr2char(csearch, pcc);

	    set_last_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	অন্যথা
	    set_last_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_find(d, (char_u *)"forward", -1);
    যদি (di != ফাঁকা)
	set_csearch_direction((পূর্ণসংখ্যা)tv_get_সংখ্যা(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_find(d, (char_u *)"until", -1);
    যদি (di != ফাঁকা)
	set_csearch_until(!!tv_get_সংখ্যা(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    স্থির শূন্য
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    স্থির শূন্য
f_setenv(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    যদি (check_restricted() || check_secure())
	ফেরত;

    name = tv_get_string_buf(&argvars[0], namebuf);
    যদি (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_সংখ্যা == VVAL_NULL)
	vim_unsetenv_ext(name);
    অন্যথা
	vim_setenv_ext(name, tv_get_string_buf(&argvars[1], valbuf));
}

/*
 * "setfperm({fname}, {mode})" function
 */
    স্থির শূন্য
f_setfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	modebuf[NUMBUFLEN];
    char_u	*mode_str;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		mask;
    পূর্ণসংখ্যা		mode = 0;

    rettv->vval.v_সংখ্যা = 0;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    fname = tv_get_string_chk(&argvars[0]);
    যদি (fname == ফাঁকা)
	ফেরত;
    mode_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    যদি (mode_str == ফাঁকা)
	ফেরত;
    যদি (STRLEN(mode_str) != 9)
    {
	semsg(_(e_invalid_argument_str), mode_str);
	ফেরত;
    }

    mask = 1;
    জন্য (i = 8; i >= 0; --i)
    {
	যদি (mode_str[i] != '-')
	    mode |= mask;
	mask = mask << 1;
    }
    rettv->vval.v_সংখ্যা = mch_setperm(fname, mode) == OK;
}

/*
 * "setpos()" function
 */
    স্থির শূন্য
f_setpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    পূর্ণসংখ্যা
get_yank_type(char_u **pp, char_u *yank_type, long *block_len)
{
    char_u *stropt = *pp;
    নির্বাচন (*stropt)
    {
	ক্ষেত্র 'v': ক্ষেত্র 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    বিরতি;
	ক্ষেত্র 'V': ক্ষেত্র 'l':	// line-wise selection
	    *yank_type = MLINE;
	    বিরতি;
	ক্ষেত্র 'b': ক্ষেত্র Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    যদি (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*block_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    বিরতি;
	পূর্বনির্ধারিত:
	    ফেরত FAIL;
    }
    *pp = stropt;
    ফেরত OK;
}

/*
 * "setreg()" function
 */
    স্থির শূন্য
f_setreg(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    পূর্ণসংখ্যা		append;
    char_u	yank_type;
    long	block_len;
    typval_T	*regcontents;
    পূর্ণসংখ্যা		pointreg;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ফেরত;

    pointreg = 0;
    regcontents = ফাঁকা;
    block_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_সংখ্যা = 1;		// FAIL is default

    যদি (strregname == ফাঁকা)
	ফেরত;		// type error; errmsg already given
    যদি (in_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_register_name_must_be_one_char_str), strregname);
	ফেরত;
    }
    regname = *strregname;
    যদি (regname == 0 || regname == '@')
	regname = '"';

    যদি (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	যদি (d == ফাঁকা || d->dv_hashtab.ht_used == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {ফাঁকা, ফাঁকা};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    ফেরত;
	}

	di = dict_find(d, (char_u *)"regcontents", -1);
	যদি (di != ফাঁকা)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	যদি (stropt != ফাঁকা)
	{
	    পূর্ণসংখ্যা ret = get_yank_type(&stropt, &yank_type, &block_len);

	    যদি (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_মান_for_argument_str), "value");
		ফেরত;
	    }
	}

	যদি (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    যদি (stropt != ফাঁকা)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	অন্যথা যদি (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    অন্যথা
	regcontents = &argvars[1];

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	যদি (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    ফেরত;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	যদি (stropt == ফাঁকা)
	    ফেরত;		// type error
	জন্য (; *stropt != NUL; ++stropt)
	    নির্বাচন (*stropt)
	    {
		ক্ষেত্র 'a': ক্ষেত্র 'A':	// append
		    append = TRUE;
		    বিরতি;
		পূর্বনির্ধারিত:
		    get_yank_type(&stropt, &yank_type, &block_len);
	    }
    }

    যদি (regcontents && regcontents->v_type == VAR_LIST)
    {
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	list_T		*ll = regcontents->vval.v_list;
	listitem_T	*li;
	পূর্ণসংখ্যা		len;

	// If the list is NULL handle like an empty list.
	len = ll == ফাঁকা ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	যদি (lstval == ফাঁকা)
	    ফেরত;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	যদি (ll != ফাঁকা)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_বস্তুসমূহ(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		যদি (strval == ফাঁকা)
		    যাও free_lstval;
		যদি (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    যদি (strval == ফাঁকা)
			যাও free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ফাঁকা;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, block_len);
free_lstval:
	যখন (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    অন্যথা যদি (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	যদি (strval == ফাঁকা)
	    ফেরত;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, block_len);
    }
    যদি (pointreg != 0)
	get_yank_register(pointreg, TRUE);

    rettv->vval.v_সংখ্যা = 0;
}

/*
 * "settagstack()" function
 */
    স্থির শূন্য
f_settagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    dict_T	*d;
    পূর্ণসংখ্যা		action = 'r';

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ফেরত;

    // first argument: window number or id
    wp = find_win_by_nr_or_id(&argvars[0]);
    যদি (wp == ফাঁকা)
	ফেরত;

    // second argument: dict with items to set in the tag stack
    যদি (check_for_dict_arg(argvars, 1) == FAIL)
	ফেরত;
    d = argvars[1].vval.v_dict;
    যদি (d == ফাঁকা)
	ফেরত;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    যদি (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    অন্যথা যদি (check_for_string_arg(argvars, 2) == FAIL)
	ফেরত;
    অন্যথা
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	যদি (actstr == ফাঁকা)
	    ফেরত;
	যদি ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	অন্যথা
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    ফেরত;
	}
    }

    যদি (set_tagstack(wp, d, action) == OK)
	rettv->vval.v_সংখ্যা = 0;
}

#ifdef FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    স্থির শূন্য
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    পূর্ণসংখ্যা		len;

    যদি (in_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != ফাঁকা ? (char_u *)blob->bv_ga.ga_তথ্য : (char_u *)"";
	len = blob != ফাঁকা ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_bytes(p, len, ফাঁকা, 0));
    }
    অন্যথা
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_bytes(p, (পূর্ণসংখ্যা)STRLEN(p), ফাঁকা, 0));
    }
}
#endif // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    স্থির শূন্য
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা do_special;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ফেরত;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    স্থির শূন্য
f_shiftwidth(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = 0;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	long	col;

	col = (long)tv_get_সংখ্যা_chk(argvars, ফাঁকা);
	যদি (col < 0)
	    ফেরত;	// type error; errmsg already given
#ifdef FEAT_VARTABS
	rettv->vval.v_সংখ্যা = get_sw_মান_col(curbuf, col, FALSE);
	ফেরত;
#endif
    }

    rettv->vval.v_সংখ্যা = get_sw_মান(curbuf);
}

/*
 * "soundfold({word})" function
 */
    স্থির শূন্য
f_soundfold(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#ifdef FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#অন্যথা
    rettv->vval.v_string = vim_strsave(s);
#endif
}

/*
 * "spellbadword()" function
 */
    স্থির শূন্য
f_spellbadword(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*word = (char_u *)"";
    hlf_T	attr = HLF_গুনতি;
    পূর্ণসংখ্যা		len = 0;
#ifdef FEAT_SPELL
    পূর্ণসংখ্যা		wo_spell_save = curwin->w_p_spell;

    যদি (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    যদি (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	ফেরত;
    }
#endif

    যদি (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	ফেরত;
    }

#ifdef FEAT_SPELL
    যদি (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_to(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	যদি (len != 0)
	{
	    word = ml_get_cursor();
	    curwin->w_set_curswant = true;
	}
    }
    অন্যথা যদি (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	পূর্ণসংখ্যা	capcol = -1;

	যদি (str != ফাঁকা)
	{
	    // Check the argument for spelling.
	    যখন (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		যদি (attr != HLF_গুনতি)
		{
		    word = str;
		    বিরতি;
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
    নির্বাচন (attr)
    {
    ক্ষেত্র HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	বিরতি;

    ক্ষেত্র HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	বিরতি;

    ক্ষেত্র HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	বিরতি;

    ক্ষেত্র HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	বিরতি;

    পূর্বনির্ধারিত:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	বিরতি;
    }
}

/*
 * "spellsuggest()" function
 */
    স্থির শূন্য
f_spellsuggest(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SPELL
    char_u	*str;
    পূর্ণসংখ্যা		typeerr = FALSE;
    পূর্ণসংখ্যা		maxcount;
    garray_T	ga;
    পূর্ণসংখ্যা		i;
    listitem_T	*li;
    পূর্ণসংখ্যা		need_capital = FALSE;
    পূর্ণসংখ্যা		wo_spell_save = curwin->w_p_spell;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ফেরত;

    যদি (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    যদি (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	ফেরত;
    }
#endif

    যদি (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	ফেরত;
    }

#ifdef FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[1], &typeerr);
	যদি (maxcount <= 0)
	    ফেরত;
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[2], &typeerr);
	    যদি (typeerr)
		ফেরত;
	}
    }
    অন্যথা
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    জন্য (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_তথ্য)[i];

	li = listitem_alloc();
	যদি (li == ফাঁকা)
	    vim_free(str);
	অন্যথা
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

    স্থির শূন্য
f_split(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*end;
    char_u	*pat = ফাঁকা;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    পূর্ণসংখ্যা		match;
    colnr_T	col = 0;
    পূর্ণসংখ্যা		keepempty = FALSE;
    পূর্ণসংখ্যা		typeerr = FALSE;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ফেরত;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    str = tv_get_string(&argvars[0]);
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	যদি (pat == ফাঁকা)
	    typeerr = TRUE;
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    যদি (pat == ফাঁকা || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    যদি (rettv_list_alloc(rettv) == FAIL)
	যাও theend;
    যদি (typeerr)
	যাও theend;

    regmatch.regprog = eval_regcomp(pat);
    যদি (regmatch.regprog != ফাঁকা)
    {
	regmatch.rm_ic = FALSE;
	যখন (*str != NUL || keepempty)
	{
	    যদি (*str == NUL)
		match = FALSE;	// empty item at the end
	    অন্যথা
		match = vim_regexec_nl(&regmatch, str, col);
	    যদি (match)
		end = regmatch.startp[0];
	    অন্যথা
		end = str + STRLEN(str);
	    যদি (keepempty || end > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && match && end < regmatch.endp[0]))
	    {
		যদি (list_append_string(rettv->vval.v_list, str,
						    (পূর্ণসংখ্যা)(end - str)) == FAIL)
		    বিরতি;
	    }
	    যদি (!match)
		বিরতি;
	    // Advance to just after the match.
	    যদি (regmatch.endp[0] > str)
		col = 0;
	    অন্যথা
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
    স্থির শূন্য
f_submatch(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		error = FALSE;
    পূর্ণসংখ্যা		no;
    পূর্ণসংখ্যা		retList = 0;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ফেরত;

    no = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[0], &error);
    যদি (error)
	ফেরত;
    যদি (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_সংখ্যা_nr), no);
	ফেরত;
    }
    যদি (argvars[1].v_type != VAR_UNKNOWN)
	retList = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[1], &error);
    যদি (error)
	ফেরত;

    যদি (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    অন্যথা
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = reg_submatch_list(no);
    }
}

/*
 * "substitute()" function
 */
    স্থির শূন্য
f_substitute(typval_T *argvars, typval_T *rettv)
{
    char_u	patbuf[NUMBUFLEN];
    char_u	subbuf[NUMBUFLEN];
    char_u	flagsbuf[NUMBUFLEN];
    char_u	*str;
    char_u	*pat;
    char_u	*sub = ফাঁকা;
    typval_T	*রাশিমালা = ফাঁকা;
    char_u	*flg;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	ফেরত;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    যদি (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	রাশিমালা = &argvars[2];
    অন্যথা
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    যদি (str == ফাঁকা || pat == ফাঁকা || (sub == ফাঁকা && রাশিমালা == ফাঁকা)
								|| flg == ফাঁকা)
	rettv->vval.v_string = ফাঁকা;
    অন্যথা
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, রাশিমালা, flg, ফাঁকা);
}

/*
 * "swapfilelist()" function
 */
    স্থির শূন্য
f_swapfilelist(typval_T *argvars UNUSED, typval_T *rettv)
{
    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;
    recover_names(ফাঁকা, FALSE, rettv->vval.v_list, 0, ফাঁকা);
}

/*
 * "swapinfo(swap_filename)" function
 */
    স্থির শূন্য
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (rettv_dict_alloc(rettv) == OK)
	get_b0_dict(tv_get_string(argvars), rettv->vval.v_dict);
}

/*
 * "swapname(expr)" function
 */
    স্থির শূন্য
f_swapname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    যদি (in_vim9script() && check_for_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    buf = tv_get_buf(&argvars[0], FALSE);
    যদি (buf == ফাঁকা || buf->b_ml.ml_mfp == ফাঁকা
					|| buf->b_ml.ml_mfp->mf_fname == ফাঁকা)
	rettv->vval.v_string = ফাঁকা;
    অন্যথা
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    স্থির শূন্য
f_synID(typval_T *argvars UNUSED, typval_T *rettv)
{
    পূর্ণসংখ্যা		id = 0;
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    পূর্ণসংখ্যা		trans;
    পূর্ণসংখ্যা		transerr = FALSE;

    যদি (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	ফেরত;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (linenr_T)tv_get_সংখ্যা(&argvars[1]) - 1;	// -1 on type error
    trans = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[2], &transerr);

    যদি (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_সারি_গুনতি
	    && col >= 0 && col < (long)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, col, trans, ফাঁকা, FALSE);
#endif

    rettv->vval.v_সংখ্যা = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    স্থির শূন্য
f_synIDattr(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*p = ফাঁকা;
#ifdef FEAT_SYN_HL
    পূর্ণসংখ্যা		id;
    char_u	*what;
    char_u	*mode;
    char_u	modebuf[NUMBUFLEN];
    পূর্ণসংখ্যা		modec;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ফেরত;

    id = (পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	mode = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(mode[0]);
	যদি (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    অন্যথা
    {
# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	যদি (USE_24BIT)
	    modec = 'g';
	অন্যথা
# endif
	    যদি (t_colors > 1)
		modec = 'c';
	    অন্যথা
		modec = 't';
    }

    নির্বাচন (TOLOWER_ASC(what[0]))
    {
	ক্ষেত্র 'b':
		যদি (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		অন্যথা					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		বিরতি;

	ক্ষেত্র 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		বিরতি;

	ক্ষেত্র 'i':
		যদি (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		অন্যথা					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		বিরতি;

	ক্ষেত্র 'n':
		যদি (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		অন্যথা					// name
		    p = get_highlight_name_ext(ফাঁকা, id - 1, FALSE);
		বিরতি;

	ক্ষেত্র 'r':					// reverse
		p = highlight_has_attr(id, HL_INVERSE, modec);
		বিরতি;

	ক্ষেত্র 's':
		যদি (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		অন্যথা যদি (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		অন্যথা					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		বিরতি;

	ক্ষেত্র 'u':
		যদি (STRLEN(what) >= 9)
		{
		    যদি (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    অন্যথা যদি (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    অন্যথা যদি (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    অন্যথা যদি (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    অন্যথা
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		অন্যথা
							// ul
		    p = highlight_color(id, what, modec);
		বিরতি;
    }

    যদি (p != ফাঁকা)
	p = vim_strsave(p);
#endif
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    স্থির শূন্য
f_synIDtrans(typval_T *argvars UNUSED, typval_T *rettv)
{
    পূর্ণসংখ্যা		id;

#ifdef FEAT_SYN_HL
    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    id = (পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[0]);

    যদি (id > 0)
	id = syn_get_final_id(id);
    অন্যথা
#endif
	id = 0;

    rettv->vval.v_সংখ্যা = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    স্থির শূন্য
f_synconcealed(typval_T *argvars UNUSED, typval_T *rettv)
{
#যদি defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	col;
    পূর্ণসংখ্যা		syntax_flags = 0;
    পূর্ণসংখ্যা		cchar;
    পূর্ণসংখ্যা		matchid = 0;
    char_u	str[NUMBUFLEN];
#endif

    rettv_list_set(rettv, ফাঁকা);

    যদি (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

#যদি defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_সংখ্যা(&argvars[1]) - 1;	// -1 on type error

    CLEAR_FIELD(str);

    যদি (rettv_list_alloc(rettv) == OK)
    {
	যদি (lnum >= 1 && lnum <= curbuf->b_ml.ml_সারি_গুনতি
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (শূন্য)syn_get_id(curwin, lnum, col, FALSE, ফাঁকা, FALSE);
	    syntax_flags = get_syntax_info(&matchid);

	    // get the conceal character
	    যদি ((syntax_flags & HL_CONCEAL) && curwin->w_p_cole < 3)
	    {
		cchar = syn_get_sub_char();
		যদি (cchar == NUL && curwin->w_p_cole == 1)
		    cchar = (curwin->w_lcs_chars.conceal == NUL) ? ' '
					: curwin->w_lcs_chars.conceal;
		যদি (cchar != NUL)
		{
		    যদি (has_mbyte)
			(*mb_char2bytes)(cchar, str);
		    অন্যথা
			str[0] = cchar;
		}
	    }
	}

	list_append_সংখ্যা(rettv->vval.v_list,
					    (syntax_flags & HL_CONCEAL) != 0);
	// -1 to auto-determine strlen
	list_append_string(rettv->vval.v_list, str, -1);
	list_append_সংখ্যা(rettv->vval.v_list, matchid);
    }
#endif
}

/*
 * "synstack(lnum, col)" function
 */
    স্থির শূন্য
f_synstack(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		id;
#endif

    rettv_list_set(rettv, ফাঁকা);

    যদি (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

#ifdef FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_সংখ্যা(&argvars[1]) - 1;	// -1 on type error

    যদি (lnum >= 1 && lnum <= curbuf->b_ml.ml_সারি_গুনতি
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(শূন্য)syn_get_id(curwin, lnum, col, FALSE, ফাঁকা, TRUE);
	জন্য (i = 0; ; ++i)
	{
	    id = syn_get_স্তূপ_item(i);
	    যদি (id < 0)
		বিরতি;
	    যদি (list_append_সংখ্যা(rettv->vval.v_list, id) == FAIL)
		বিরতি;
	}
    }
#endif
}

/*
 * "tabpagebuflist()" function
 */
    স্থির শূন্য
f_tabpagebuflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp = ফাঁকা;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    অন্যথা
    {
	tp = find_tabpage((পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[0]));
	যদি (tp != ফাঁকা)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    যদি (wp != ফাঁকা && rettv_list_alloc(rettv) == OK)
    {
	জন্য (; wp != ফাঁকা; wp = wp->w_পরবর্তী)
	    যদি (list_append_সংখ্যা(rettv->vval.v_list,
						wp->w_বাফার->b_fnum) == FAIL)
		বিরতি;
    }
}

/*
 * "tagfiles()" function
 */
    স্থির শূন্য
f_tagfiles(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    পূর্ণসংখ্যা		first;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;
    fname = alloc(MAXPATHL);
    যদি (fname == ফাঁকা)
	ফেরত;

    জন্য (first = TRUE; ; first = FALSE)
	যদি (get_tagfname(&tn, first, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    বিরতি;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    স্থির শূন্য
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = ফাঁকা;
    char_u  *tag_pattern;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ফেরত;

    tag_pattern = tv_get_string(&argvars[0]);

    rettv->vval.v_সংখ্যা = FALSE;
    যদি (*tag_pattern == NUL)
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    যদি (rettv_list_alloc(rettv) == OK)
	(শূন্য)get_tags(rettv->vval.v_list, tag_pattern, fname);
}

/*
 * "type(expr)" function
 */
    স্থির শূন্য
f_type(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা n = -1;

    নির্বাচন (argvars[0].v_type)
    {
	ক্ষেত্র VAR_সংখ্যা:  n = VAR_TYPE_সংখ্যা; বিরতি;
	ক্ষেত্র VAR_STRING:  n = VAR_TYPE_STRING; বিরতি;
	ক্ষেত্র VAR_PARTIAL:
	ক্ষেত্র VAR_FUNC:    n = VAR_TYPE_FUNC; বিরতি;
	ক্ষেত্র VAR_LIST:    n = VAR_TYPE_LIST; বিরতি;
	ক্ষেত্র VAR_TUPLE:   n = VAR_TYPE_TUPLE; বিরতি;
	ক্ষেত্র VAR_DICT:    n = VAR_TYPE_DICT; বিরতি;
	ক্ষেত্র VAR_FLOAT:   n = VAR_TYPE_FLOAT; বিরতি;
	ক্ষেত্র VAR_BOOL:	  n = VAR_TYPE_BOOL; বিরতি;
	ক্ষেত্র VAR_SPECIAL: n = VAR_TYPE_NONE; বিরতি;
	ক্ষেত্র VAR_JOB:     n = VAR_TYPE_JOB; বিরতি;
	ক্ষেত্র VAR_CHANNEL: n = VAR_TYPE_CHANNEL; বিরতি;
	ক্ষেত্র VAR_BLOB:    n = VAR_TYPE_BLOB; বিরতি;
	ক্ষেত্র VAR_INSTR:   n = VAR_TYPE_INSTR; বিরতি;
	ক্ষেত্র VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; বিরতি;
	ক্ষেত্র VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		যদি (cl != ফাঁকা && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		অন্যথা
		    n = VAR_TYPE_CLASS;
		বিরতি;
	    }
	ক্ষেত্র VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		যদি (obj == ফাঁকা)
		    n = VAR_TYPE_OBJECT;
		অন্যথা
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    যদি (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    অন্যথা
			n = VAR_TYPE_OBJECT;
		}
		বিরতি;
	    }
	ক্ষেত্র VAR_VOID:
	    emsg(_(e_cannot_use_void_মান));
	    বিরতি;
	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	    internal_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    বিরতি;
    }
    rettv->vval.v_সংখ্যা = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    স্থির শূন্য
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_start = 0;
    colnr_T	vcol_end = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    পূর্ণসংখ্যা		winchanged = FALSE;
    পূর্ণসংখ্যা		len;

    যদি (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL))))
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[2]), &tp);
	যদি (wp == ফাঁকা || tp == ফাঁকা)
	    যাও theend;

	যদি (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    যাও theend;

	check_cursor();
	winchanged = TRUE;
    }

    পূর্ণসংখ্যা fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    যদি (fp != ফাঁকা && fp->lnum <= curbuf->b_ml.ml_সারি_গুনতি
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	যদি (fp->col < 0)
	    fp->col = 0;
	অন্যথা
	{
	    len = (পূর্ণসংখ্যা)ml_get_len(fp->lnum);
	    যদি (fp->col > len)
		fp->col = len;
	}
	getvvcol(curwin, fp, &vcol_start, ফাঁকা, &vcol_end, 0);
	++vcol_start;
	++vcol_end;
    }

theend:
    যদি (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	যদি (rettv_list_alloc(rettv) == OK)
	{
	    list_append_সংখ্যা(rettv->vval.v_list, vcol_start);
	    list_append_সংখ্যা(rettv->vval.v_list, vcol_end);
	}
	অন্যথা
	    rettv->vval.v_সংখ্যা = 0;
    }
    অন্যথা
	rettv->vval.v_সংখ্যা = vcol_end;

    যদি (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    স্থির শূন্য
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    যদি (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_mode_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    যদি (non_zero_arg(&argvars[0]))
	curbuf->b_visual_mode_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    স্থির শূন্য
f_wildmenumode(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    যদি (wild_menu_showing || ((State & MODE_CMDLINE) && cmdline_pum_active()))
	rettv->vval.v_সংখ্যা = 1;
}

/*
 * "windowsversion()" function
 */
    স্থির শূন্য
f_windowsversion(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windowsVersion);
}

/*
 * "wordcount()" function
 */
    স্থির শূন্য
f_wordcount(typval_T *argvars UNUSED, typval_T *rettv)
{
    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    স্থির শূন্য
f_xor(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    rettv->vval.v_সংখ্যা = tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা)
					^ tv_get_সংখ্যা_chk(&argvars[1], ফাঁকা);
}

#endif // FEAT_EVAL
