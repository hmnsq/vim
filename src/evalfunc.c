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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

સ્થિર શૂન્ય f_and(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_BEVAL
સ્થિર શૂન્ય f_balloon_gettext(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_balloon_show(typval_T *argvars, typval_T *rettv);
# જો defined(FEAT_BEVAL_TERM)
સ્થિર શૂન્ય f_balloon_split(typval_T *argvars, typval_T *rettv);
# endif
#endif
સ્થિર શૂન્ય f_base64_encode(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_base64_decode(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_bindtextdomain(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_byte2line(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_call(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_changenr(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_char2nr(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_charcol(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_col(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_confirm(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_copy(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_cursor(typval_T *argsvars, typval_T *rettv);
#ifdef MSWIN
સ્થિર શૂન્ય f_debugbreak(typval_T *argvars, typval_T *rettv);
#endif
સ્થિર શૂન્ય f_deepcopy(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_did_filetype(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_echoraw(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_empty(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_environ(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_err_teapot(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_escape(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_eval(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_eventhandler(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_execute(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_exists_compiled(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_expand(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_expandcmd(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_feedkeys(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_fnameescape(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_foreground(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_funcref(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_function(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_garbagecollect(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_get(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getbgcolor(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getcellpixels(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getchangelist(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getcharpos(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getcharsearch(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getcurpos(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getcursorcharpos(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getenv(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getfontname(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getinfo(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getjumplist(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getpid(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getpos(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getreg(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getreginfo(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getregion(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getregionpos(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_getregtype(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_gettagstack(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_gettext(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_haslocaldir(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_hlID(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_hlexists(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_hostname(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_id(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_index(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_indexof(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_આગત(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_inputdialog(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_inputlist(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_inputrestore(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_inputsave(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_inputsecret(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_interrupt(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_invert(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_islocked(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_વસ્તુઓ(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_keytrans(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_last_buffer_nr(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_libcall(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_libcallnr(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_લીટી(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_line2byte(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_LUA
સ્થિર શૂન્ય f_luaeval(typval_T *argvars, typval_T *rettv);
#endif
સ્થિર શૂન્ય f_match(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_matchbufline(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_matchend(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_matchlist(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_matchstr(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_matchstrlist(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_matchstrpos(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_max(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_min(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_MZSCHEME
સ્થિર શૂન્ય f_mzeval(typval_T *argvars, typval_T *rettv);
#endif
સ્થિર શૂન્ય f_nextnonblank(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_ngettext(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_nr2char(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_or(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PERL
સ્થિર શૂન્ય f_perleval(typval_T *argvars, typval_T *rettv);
#endif
સ્થિર શૂન્ય f_prevnonblank(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_printf(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_pum_getpos(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_pumvisible(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PYTHON3
સ્થિર શૂન્ય f_py3eval(typval_T *argvars, typval_T *rettv);
#endif
#ifdef FEAT_PYTHON
સ્થિર શૂન્ય f_pyeval(typval_T *argvars, typval_T *rettv);
#endif
#જો defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
સ્થિર શૂન્ય f_pyxeval(typval_T *argvars, typval_T *rettv);
#endif
સ્થિર શૂન્ય f_test_srand_seed(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_rand(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_range(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_reg_executing(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_reg_recording(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_rename(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_repeat(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_RUBY
સ્થિર શૂન્ય f_rubyeval(typval_T *argvars, typval_T *rettv);
#endif
સ્થિર શૂન્ય f_screenattr(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_screenchar(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_screenchars(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_screencol(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_screenrow(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_screenstring(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_શોધ(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_searchdecl(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_searchpair(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_searchpairpos(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_searchpos(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_setcharpos(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_setcharsearch(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_setcursorcharpos(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_setenv(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_setfperm(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_setpos(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_setreg(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_settagstack(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_CRYPT
સ્થિર શૂન્ય f_sha256(typval_T *argvars, typval_T *rettv);
#endif
સ્થિર શૂન્ય f_shellescape(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_shiftwidth(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_soundfold(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_spellbadword(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_spellsuggest(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_split(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_srand(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_submatch(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_substitute(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_swapfilelist(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_swapinfo(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_swapname(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_synID(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_synIDattr(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_synIDtrans(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_synstack(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_synconcealed(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_tabpagebuflist(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_taglist(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_tagfiles(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_type(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_virtcol(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_visualmode(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_wildmenumode(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_windowsversion(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_wordcount(typval_T *argvars, typval_T *rettv);
સ્થિર શૂન્ય f_xor(typval_T *argvars, typval_T *rettv);


/*
 * Functions that check the argument type of a builtin function.
 * Each function returns FAIL and gives an error message if the type is wrong.
 */

// Context passed to an arg_ function.
પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક		arg_ગણતરી;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    પૂર્ણાંક		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
પ્રકારવ્યાખ્યા પૂર્ણાંક (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    સ્થિર પૂર્ણાંક
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    પાછા need_type(actual, expected, 0,
	    context->arg_idx - context->arg_ગણતરી, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    સ્થિર પૂર્ણાંક
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    જો (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_ગણતરી, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	પાછા FAIL;
    પાછા arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    પૂર્ણાંક
arg_type_modifiable(type_T *type, પૂર્ણાંક arg_idx)
{
    અક્ષર *tofree;

    જો ((type->tt_flags & TTFLAG_CONST) == 0)
	પાછા OK;
    semsg(_(e_argument_nr_trying_to_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    પાછા FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    સ્થિર પૂર્ણાંક
arg_any(type_T *type UNUSED,
	type_T *decl_type UNUSED,
	argcontext_T *context UNUSED)
{
    પાછા OK;
}

/*
 * Check "type" is a float or a number.
 */
    સ્થિર પૂર્ણાંક
arg_float_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_સંખ્યા
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_સંખ્યા, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a number.
 */
    સ્થિર પૂર્ણાંક
arg_સંખ્યા(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    પાછા check_arg_type(&t_સંખ્યા, type, context);
}

/*
 * Check "type" is an object.
 */
    સ્થિર પૂર્ણાંક
arg_object(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    સ્થિર પૂર્ણાંક
arg_dict_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    પાછા check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    સ્થિર પૂર્ણાંક
arg_list_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    પાછા check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    સ્થિર પૂર્ણાંક
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    પાછા check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    સ્થિર પૂર્ણાંક
arg_list_સંખ્યા(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    પાછા check_arg_type(&t_list_સંખ્યા, type, context);
}

/*
 * Check "type" is a list of strings.
 */
    સ્થિર પૂર્ણાંક
arg_list_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    પાછા check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    સ્થિર પૂર્ણાંક
arg_tuple_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    પાછા check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    સ્થિર પૂર્ણાંક
arg_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    પાછા check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    સ્થિર પૂર્ણાંક
arg_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    પાછા check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    સ્થિર પૂર્ણાંક
arg_bool(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    પાછા check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    સ્થિર પૂર્ણાંક
arg_bool_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_સંખ્યા
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_સંખ્યા, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    સ્થિર પૂર્ણાંક
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	પાછા OK;
    પાછા check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    સ્થિર પૂર્ણાંક
arg_list_or_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    સ્થિર પૂર્ણાંક
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    જો (arg_list_or_blob(type, decl_type, context) == FAIL)
	પાછા FAIL;
    પાછા arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    સ્થિર પૂર્ણાંક
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    પાછા FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    સ્થિર પૂર્ણાંક
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a string or a number
 */
    સ્થિર પૂર્ણાંક
arg_string_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_સંખ્યા
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    સ્થિર પૂર્ણાંક
arg_buffer(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_સંખ્યા
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    સ્થિર પૂર્ણાંક
arg_buffer_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_સંખ્યા
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    સ્થિર પૂર્ણાંક
arg_lnum(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_સંખ્યા
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    સ્થિર પૂર્ણાંક
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	પાછા OK;
    જો (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	પાછા FAIL;
    }
    જો (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	પાછા OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    સ્થિર પૂર્ણાંક
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    સ્થિર પૂર્ણાંક
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    સ્થિર પૂર્ણાંક
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    સ્થિર પૂર્ણાંક
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    સ્થિર પૂર્ણાંક
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    જો (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    સ્થિર પૂર્ણાંક
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    જો (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	પાછા FAIL;
    પાછા arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    સ્થિર પૂર્ણાંક
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    સ્થિર પૂર્ણાંક
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    જો (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	પાછા FAIL;
    પાછા arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    સ્થિર પૂર્ણાંક
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	પાછા OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr),
						 context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    સ્થિર પૂર્ણાંક
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = ખાલી;
    type_T *(args[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, ખાલી, ખાલી, args};

    જો (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	જો (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	નહીંતર
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    નહીંતર જો (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    નહીંતર જો (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_સંખ્યા;

    args[0] = ખાલી;
    args[1] = &t_unknown;
    જો (type->tt_argcount != -1)
    {
	જો (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_સંખ્યા_of_arguments));
	    પાછા FAIL;
	}
	જો (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	નહીંતર
	{
	    જો (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		args[0] = &t_સંખ્યા;
	    નહીંતર જો (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		args[0] = &t_string;
	    જો (args[0] != ખાલી)
		args[1] = expected_member;
	}
    }

    જો (!type_any_or_unknown(type->tt_member) || args[0] != ખાલી)
    {
	where_T where = WHERE_INIT;

	જો (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == ખાલી
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	નહીંતર જો (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	નહીંતર // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	જો (args[0] == ખાલી)
	    args[0] = &t_unknown;
	જો (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_index = 2;
	where.wt_kind = WT_ARGUMENT;
	પાછા check_type(&t_func_exp, type, TRUE, where);
    }
    પાછા OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    સ્થિર પૂર્ણાંક
arg_filter_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	પાછા OK;

    જો (type->tt_type == VAR_FUNC)
	પાછા check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    પાછા FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    સ્થિર પૂર્ણાંક
arg_map_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	પાછા OK;

    જો (type->tt_type == VAR_FUNC)
	પાછા check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    પાછા FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    સ્થિર પૂર્ણાંક
arg_foreach_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	પાછા OK;

    જો (type->tt_type == VAR_FUNC)
	પાછા check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    પાછા FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    સ્થિર પૂર્ણાંક
arg_sort_how(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	પાછા OK;

    જો (type->tt_type == VAR_FUNC)
    {
	type_T *(args[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_સંખ્યા, ખાલી, args};

	જો (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    args[0] = context->arg_types[0].type_curr->tt_member;
	નહીંતર
	    args[0] = &t_unknown;
	જો (!type_any_or_unknown(type->tt_member) || args[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    args[1] = args[0];
	    જો (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_index = 2;
	    where.wt_kind = WT_ARGUMENT;
	    પાછા check_type(&t_func_exp, type, TRUE, where);
	}

	પાછા OK;
    }
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    પાછા FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    સ્થિર પૂર્ણાંક
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_સંખ્યા
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    સ્થિર પૂર્ણાંક
varargs_class(type_T *type UNUSED,
	      type_T *decl_type UNUSED,
	      argcontext_T *context)
{
    માટે (પૂર્ણાંક i = context->arg_idx; i < context->arg_ગણતરી; ++i)
    {
	type2_T *types = &context->arg_types[i];
	જો (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    પાછા FAIL;
	}
    }
    પાછા OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    સ્થિર પૂર્ણાંક
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    સ્થિર પૂર્ણાંક
arg_reverse(type_T *type, type_T *decl_type, argcontext_T *context)
{
    જો (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	પાછા FAIL;
    પાછા arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    સ્થિર પૂર્ણાંક
arg_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    પાછા check_arg_type(&t_job, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    સ્થિર પૂર્ણાંક
arg_chan_or_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_JOB
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    સ્થિર પૂર્ણાંક
arg_same_as_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_decl;

    પાછા check_arg_type(prev_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    સ્થિર પૂર્ણાંક
arg_same_struct_as_prev(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;

    જો (prev_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	પાછા check_arg_type(prev_type, type, context);
    પાછા OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    સ્થિર પૂર્ણાંક
arg_item_of_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    જો (prev_type->tt_type == VAR_LIST)
	expected = prev_type->tt_member;
    નહીંતર જો (prev_type->tt_type == VAR_BLOB)
	expected = &t_સંખ્યા;
    નહીંતર
	// probably VAR_ANY, can't check
	પાછા OK;

    પાછા check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    સ્થિર પૂર્ણાંક
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_સંખ્યા
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    સ્થિર પૂર્ણાંક
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	પાછા OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    સ્થિર પૂર્ણાંક
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 2].type_curr;

    જો (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	પાછા arg_સંખ્યા(type, decl_type, context);
    જો (first_type->tt_type == VAR_DICT)
	પાછા arg_string(type, decl_type, context);
    પાછા OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    સ્થિર પૂર્ણાંક
arg_get1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	પાછા OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    સ્થિર પૂર્ણાંક
arg_len1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_સંખ્યા
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	પાછા OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    સ્થિર પૂર્ણાંક
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 1].type_curr;

    જો (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	પાછા arg_સંખ્યા(type, decl_type, context);
    જો (first_type->tt_type == VAR_DICT)
	પાછા arg_string_or_nr(type, decl_type, context);
    પાછા OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    સ્થિર પૂર્ણાંક
arg_repeat1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_સંખ્યા
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	પાછા OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    સ્થિર પૂર્ણાંક
arg_slice1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	પાછા OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    સ્થિર પૂર્ણાંક
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    જો (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	પાછા OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr),
							 context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    સ્થિર પૂર્ણાંક
arg_cursor1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    જો (type->tt_type == VAR_સંખ્યા
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	પાછા OK;

    arg_type_mismatch(&t_સંખ્યા, type, context->arg_idx + 1);
    પાછા FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
સ્થિર argcheck_T arg1_blob[] = {arg_blob};
સ્થિર argcheck_T arg1_bool[] = {arg_bool};
સ્થિર argcheck_T arg1_buffer[] = {arg_buffer};
સ્થિર argcheck_T arg1_buffer_or_dict_any[] = {arg_buffer_or_dict_any};
સ્થિર argcheck_T arg1_chan_or_job[] = {arg_chan_or_job};
સ્થિર argcheck_T arg1_dict_any[] = {arg_dict_any};
સ્થિર argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
સ્થિર argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
સ્થિર argcheck_T arg1_job[] = {arg_job};
સ્થિર argcheck_T arg1_list_any[] = {arg_list_any};
સ્થિર argcheck_T arg1_tuple_any[] = {arg_tuple_any};
સ્થિર argcheck_T arg1_list_સંખ્યા[] = {arg_list_સંખ્યા};
સ્થિર argcheck_T arg1_reverse[] = {arg_reverse};
સ્થિર argcheck_T arg1_list_or_tuple_or_dict[] = {arg_list_or_tuple_or_dict};
સ્થિર argcheck_T arg1_list_string[] = {arg_list_string};
સ્થિર argcheck_T arg1_list_tuple_dict_blob_or_string[] = {arg_list_tuple_dict_blob_or_string};
સ્થિર argcheck_T arg1_lnum[] = {arg_lnum};
સ્થિર argcheck_T arg1_સંખ્યા[] = {arg_સંખ્યા};
સ્થિર argcheck_T arg1_string[] = {arg_string};
સ્થિર argcheck_T arg1_string_or_list_any[] = {arg_string_or_list_any};
સ્થિર argcheck_T arg1_string_or_list_string[] = {arg_string_or_list_string};
સ્થિર argcheck_T arg1_string_or_nr[] = {arg_string_or_nr};
સ્થિર argcheck_T arg1_string_or_blob[] = {arg_string_or_blob};
સ્થિર argcheck_T arg2_buffer_any[] = {arg_buffer, arg_any};
સ્થિર argcheck_T arg2_buffer_bool[] = {arg_buffer, arg_bool};
સ્થિર argcheck_T arg2_buffer_list_any[] = {arg_buffer, arg_list_any};
સ્થિર argcheck_T arg2_buffer_lnum[] = {arg_buffer, arg_lnum};
સ્થિર argcheck_T arg2_buffer_સંખ્યા[] = {arg_buffer, arg_સંખ્યા};
સ્થિર argcheck_T arg2_buffer_string[] = {arg_buffer, arg_string};
સ્થિર argcheck_T arg2_chan_or_job_dict[] = {arg_chan_or_job, arg_dict_any};
સ્થિર argcheck_T arg2_chan_or_job_string[] = {arg_chan_or_job, arg_string};
સ્થિર argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
સ્થિર argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
સ્થિર argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
સ્થિર argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
સ્થિર argcheck_T arg2_job_dict[] = {arg_job, arg_dict_any};
સ્થિર argcheck_T arg2_job_string_or_સંખ્યા[] = {arg_job, arg_string_or_nr};
સ્થિર argcheck_T arg2_list_any_સંખ્યા[] = {arg_list_any, arg_સંખ્યા};
સ્થિર argcheck_T arg2_list_સંખ્યા[] = {arg_list_સંખ્યા, arg_list_સંખ્યા};
સ્થિર argcheck_T arg2_list_સંખ્યા_bool[] = {arg_list_સંખ્યા, arg_bool};
સ્થિર argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
સ્થિર argcheck_T arg2_listblobmod_item[] = {arg_list_or_blob_mod, arg_item_of_prev};
સ્થિર argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
સ્થિર argcheck_T arg2_lnum_સંખ્યા[] = {arg_lnum, arg_સંખ્યા};
સ્થિર argcheck_T arg2_સંખ્યા[] = {arg_સંખ્યા, arg_સંખ્યા};
સ્થિર argcheck_T arg2_સંખ્યા_any[] = {arg_સંખ્યા, arg_any};
સ્થિર argcheck_T arg2_સંખ્યા_bool[] = {arg_સંખ્યા, arg_bool};
સ્થિર argcheck_T arg2_સંખ્યા_dict_any[] = {arg_સંખ્યા, arg_dict_any};
સ્થિર argcheck_T arg2_સંખ્યા_list[] = {arg_સંખ્યા, arg_list_any};
સ્થિર argcheck_T arg2_સંખ્યા_string[] = {arg_સંખ્યા, arg_string};
સ્થિર argcheck_T arg2_સંખ્યા_buffer[] = {arg_સંખ્યા, arg_buffer};
સ્થિર argcheck_T arg2_સંખ્યા_string_or_list[] = {arg_સંખ્યા, arg_string_or_list_any};
સ્થિર argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
સ્થિર argcheck_T arg2_string[] = {arg_string, arg_string};
સ્થિર argcheck_T arg2_string_any[] = {arg_string, arg_any};
સ્થિર argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
સ્થિર argcheck_T arg2_string_chan_or_job[] = {arg_string, arg_chan_or_job};
સ્થિર argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
સ્થિર argcheck_T arg2_string_list_સંખ્યા[] = {arg_string, arg_list_સંખ્યા};
સ્થિર argcheck_T arg2_string_સંખ્યા[] = {arg_string, arg_સંખ્યા};
સ્થિર argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
સ્થિર argcheck_T arg2_string_or_list_સંખ્યા[] = {arg_string_or_list_any, arg_સંખ્યા};
સ્થિર argcheck_T arg2_string_string_or_સંખ્યા[] = {arg_string, arg_string_or_nr};
સ્થિર argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
સ્થિર argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
સ્થિર argcheck_T arg3_any_buffer_bool_or_dict[] = {
			      arg_any, arg_buffer, arg_bool_or_dict_any};
સ્થિર argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
સ્થિર argcheck_T arg3_buffer_lnum_lnum[] = {arg_buffer, arg_lnum, arg_lnum};
સ્થિર argcheck_T arg3_buffer_સંખ્યા_સંખ્યા[] = {arg_buffer, arg_સંખ્યા, arg_સંખ્યા};
સ્થિર argcheck_T arg3_buffer_string_any[] = {arg_buffer, arg_string, arg_any};
સ્થિર argcheck_T arg3_buffer_string_dict[] = {arg_buffer, arg_string, arg_dict_any};
સ્થિર argcheck_T arg3_dict_સંખ્યા_સંખ્યા[] = {arg_dict_any, arg_સંખ્યા, arg_સંખ્યા};
સ્થિર argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
સ્થિર argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
સ્થિર argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
સ્થિર argcheck_T arg3_lnum_સંખ્યા_bool[] = {arg_lnum, arg_સંખ્યા, arg_bool};
સ્થિર argcheck_T arg3_સંખ્યા[] = {arg_સંખ્યા, arg_સંખ્યા, arg_સંખ્યા};
સ્થિર argcheck_T arg3_સંખ્યા_any_dict[] = {arg_સંખ્યા, arg_any, arg_dict_any};
સ્થિર argcheck_T arg3_સંખ્યા_સંખ્યા_dict[] = {arg_સંખ્યા, arg_સંખ્યા, arg_dict_any};
સ્થિર argcheck_T arg3_સંખ્યા_string_any[] = {arg_સંખ્યા, arg_string, arg_any};
સ્થિર argcheck_T arg3_સંખ્યા_string_buffer[] = {arg_સંખ્યા, arg_string, arg_buffer};
સ્થિર argcheck_T arg3_સંખ્યા_string_string[] = {arg_સંખ્યા, arg_string, arg_string};
સ્થિર argcheck_T arg3_string[] = {arg_string, arg_string, arg_string};
સ્થિર argcheck_T arg3_string_any_dict[] = {arg_string, arg_any, arg_dict_any};
સ્થિર argcheck_T arg3_string_any_string[] = {arg_string, arg_any, arg_string};
સ્થિર argcheck_T arg3_string_bool_bool[] = {arg_string, arg_bool, arg_bool};
સ્થિર argcheck_T arg3_string_સંખ્યા_bool[] = {arg_string, arg_સંખ્યા, arg_bool};
સ્થિર argcheck_T arg3_string_સંખ્યા_સંખ્યા[] = {arg_string, arg_સંખ્યા, arg_સંખ્યા};
સ્થિર argcheck_T arg3_string_or_dict_bool_dict[] = {arg_string_or_dict_any, arg_bool, arg_dict_any};
સ્થિર argcheck_T arg3_string_or_list_bool_સંખ્યા[] = {arg_string_or_list_any, arg_bool, arg_સંખ્યા};
સ્થિર argcheck_T arg3_string_string_bool[] = {arg_string, arg_string, arg_bool};
સ્થિર argcheck_T arg3_string_string_dict[] = {arg_string, arg_string, arg_dict_any};
સ્થિર argcheck_T arg3_string_string_સંખ્યા[] = {arg_string, arg_string, arg_સંખ્યા};
સ્થિર argcheck_T arg4_સંખ્યા_સંખ્યા_string_any[] = {arg_સંખ્યા, arg_સંખ્યા, arg_string, arg_any};
સ્થિર argcheck_T arg4_string_string_any_string[] = {arg_string, arg_string, arg_any, arg_string};
સ્થિર argcheck_T arg4_string_string_સંખ્યા_string[] = {arg_string, arg_string, arg_સંખ્યા, arg_string};
સ્થિર argcheck_T arg4_string_સંખ્યા_bool_bool[] = {arg_string, arg_સંખ્યા, arg_bool, arg_bool};
/* Function specific argument types (not covered by the above) */
સ્થિર argcheck_T arg15_assert_fails[] = {arg_string_or_nr, arg_string_or_list_any, arg_any, arg_સંખ્યા, arg_string};
સ્થિર argcheck_T arg34_assert_inrange[] = {arg_float_or_nr, arg_float_or_nr, arg_float_or_nr, arg_string};
સ્થિર argcheck_T arg4_browse[] = {arg_bool, arg_string, arg_string, arg_string};
સ્થિર argcheck_T arg23_chanexpr[] = {arg_chan_or_job, arg_any, arg_dict_any};
સ્થિર argcheck_T arg23_chanraw[] = {arg_chan_or_job, arg_string_or_blob, arg_dict_any};
સ્થિર argcheck_T arg24_ગણતરી[] = {arg_string_list_tuple_or_dict, arg_any, arg_bool, arg_સંખ્યા};
સ્થિર argcheck_T arg13_cursor[] = {arg_cursor1, arg_સંખ્યા, arg_સંખ્યા};
સ્થિર argcheck_T arg12_deepcopy[] = {arg_any, arg_bool};
સ્થિર argcheck_T arg12_execute[] = {arg_string_or_list_string, arg_string};
સ્થિર argcheck_T arg12_getchar[] = {arg_bool_or_nr, arg_dict_any};
સ્થિર argcheck_T arg23_extend[] = {arg_list_or_dict_or_blob_mod, arg_same_as_prev, arg_extend3};
સ્થિર argcheck_T arg23_extendnew[] = {arg_list_or_dict_or_blob, arg_same_struct_as_prev, arg_extend3};
સ્થિર argcheck_T arg23_get[] = {arg_get1, arg_string_or_nr, arg_any};
સ્થિર argcheck_T arg14_glob[] = {arg_string, arg_bool, arg_bool, arg_bool};
સ્થિર argcheck_T arg25_globpath[] = {arg_string, arg_string, arg_bool, arg_bool, arg_bool};
સ્થિર argcheck_T arg24_index[] = {arg_list_or_tuple_or_blob, arg_item_of_prev, arg_સંખ્યા, arg_bool};
સ્થિર argcheck_T arg23_index[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
સ્થિર argcheck_T arg23_ઉમેરો[] = {arg_list_or_blob, arg_item_of_prev, arg_સંખ્યા};
સ્થિર argcheck_T arg1_len[] = {arg_len1};
સ્થિર argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
સ્થિર argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
સ્થિર argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
સ્થિર argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
સ્થિર argcheck_T arg2_instanceof[] = {arg_object, varargs_class, ખાલી };
સ્થિર argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
સ્થિર argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
સ્થિર argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_સંખ્યા, arg_સંખ્યા, arg_dict_any};
સ્થિર argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_સંખ્યા, arg_સંખ્યા, arg_dict_any};
સ્થિર argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
સ્થિર argcheck_T arg45_matchbufline[] = {arg_buffer, arg_string, arg_lnum, arg_lnum, arg_dict_any};
સ્થિર argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
સ્થિર argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
સ્થિર argcheck_T arg24_remote_પદાવલિ[] = {arg_string, arg_string, arg_string, arg_સંખ્યા};
સ્થિર argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_સંખ્યા};
સ્થિર argcheck_T arg2_repeat[] = {arg_repeat1, arg_સંખ્યા};
સ્થિર argcheck_T arg15_શોધ[] = {arg_string, arg_string, arg_સંખ્યા, arg_સંખ્યા, arg_string_or_func};
સ્થિર argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_સંખ્યા, arg_સંખ્યા};
સ્થિર argcheck_T arg3_setbufline[] = {arg_buffer, arg_lnum, arg_str_or_nr_or_list};
સ્થિર argcheck_T arg2_setline[] = {arg_lnum, arg_any};
સ્થિર argcheck_T arg24_setloclist[] = {arg_સંખ્યા, arg_list_any, arg_string, arg_dict_any};
સ્થિર argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
સ્થિર argcheck_T arg23_settagstack[] = {arg_સંખ્યા, arg_dict_any, arg_string};
સ્થિર argcheck_T arg02_sign_getplaced[] = {arg_buffer, arg_dict_any};
સ્થિર argcheck_T arg45_sign_place[] = {arg_સંખ્યા, arg_string, arg_string, arg_buffer, arg_dict_any};
સ્થિર argcheck_T arg23_slice[] = {arg_slice1, arg_સંખ્યા, arg_સંખ્યા};
સ્થિર argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_sort_how, arg_dict_any};
સ્થિર argcheck_T arg24_strpart[] = {arg_string, arg_સંખ્યા, arg_સંખ્યા, arg_bool};
સ્થિર argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
સ્થિર argcheck_T arg23_win_execute[] = {arg_સંખ્યા, arg_string_or_list_string, arg_string};
સ્થિર argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
સ્થિર argcheck_T arg24_match_func[] = {arg_string_or_list_any, arg_string, arg_સંખ્યા, arg_સંખ્યા};

// Can be used by functions called through "f_retfunc" to create new types.
સ્થિર garray_T *current_type_gap = ખાલી;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    સ્થિર type_T *
ret_void(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_void;
}
    સ્થિર type_T *
ret_any(પૂર્ણાંક	argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_any;
}
    સ્થિર type_T *
ret_bool(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_bool;
}
    સ્થિર type_T *
ret_સંખ્યા_bool(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_સંખ્યા_bool;
}
    સ્થિર type_T *
ret_સંખ્યા(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_સંખ્યા;
}
    સ્થિર type_T *
ret_float(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_float;
}
    સ્થિર type_T *
ret_string(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_string;
}
    સ્થિર type_T *
ret_list_any(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_list_any;
}
    સ્થિર type_T *
ret_list_સંખ્યા(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    પાછા &t_list_સંખ્યા;
}
    સ્થિર type_T *
ret_list_string(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    પાછા &t_list_string;
}
    સ્થિર type_T *
ret_list_dict_any(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    પાછા &t_list_dict_any;
}
    સ્થિર type_T *
ret_list_વસ્તુઓ(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    પાછા &t_list_list_any;
}
    સ્થિર type_T *
ret_list_list_any(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    પાછા &t_list_list_any;
}
    સ્થિર type_T *
ret_list_string_વસ્તુઓ(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    પાછા &t_list_list_string;
}
    સ્થિર type_T *
ret_list_regionpos(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    પાછા &t_list_list_list_સંખ્યા;
}
    સ્થિર type_T *
ret_tuple_any(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_tuple_any;
}
    સ્થિર type_T *
ret_dict_any(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_dict_any;
}
    સ્થિર type_T *
ret_job_info(પૂર્ણાંક argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    જો (argcount == 0)
    {
	*decl_type = &t_list_any;
	પાછા &t_list_job;
    }
    પાછા &t_dict_any;
}
    સ્થિર type_T *
ret_dict_સંખ્યા(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_dict_સંખ્યા;
}
    સ્થિર type_T *
ret_dict_string(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_dict_string;
}
    સ્થિર type_T *
ret_blob(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_blob;
}
    સ્થિર type_T *
ret_func_any(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_func_any;
}
    સ્થિર type_T *
ret_func_unknown(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_func_unknown;
}
    સ્થિર type_T *
ret_channel(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_channel;
}
    સ્થિર type_T *
ret_job(પૂર્ણાંક argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    પાછા &t_job;
}
    સ્થિર type_T *
ret_first_arg(પૂર્ણાંક argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    જો (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	પાછા argtypes[0].type_curr;
    }
    પાછા &t_void;
}
    સ્થિર type_T *
ret_slice(પૂર્ણાંક argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    જો (argcount > 0)
    {
	જો (argtypes[0].type_decl != ખાલી)
	{
	    પસંદગી (argtypes[0].type_decl->tt_type)
	    {
		સ્થિતિ VAR_STRING: *decl_type = &t_string; તોડો;
		સ્થિતિ VAR_BLOB: *decl_type = &t_blob; તોડો;
		સ્થિતિ VAR_LIST: *decl_type = &t_list_any; તોડો;
		સ્થિતિ VAR_TUPLE: *decl_type = &t_tuple_any; તોડો;
		મૂળભૂત: તોડો;
	    }
	}
	પાછા argtypes[0].type_curr;
    }
    પાછા &t_void;
}
    સ્થિર type_T *
ret_copy(પૂર્ણાંક argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    જો (argcount > 0)
    {
	જો (argtypes[0].type_decl != ખાલી)
	{
	    જો (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    નહીંતર જો (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    નહીંતર
		*decl_type = argtypes[0].type_decl;
	}
	જો (argtypes[0].type_curr != ખાલી)
	{
	    જો (argtypes[0].type_curr->tt_type == VAR_LIST)
		પાછા &t_list_any;
	    નહીંતર જો (argtypes[0].type_curr->tt_type == VAR_DICT)
		પાછા &t_dict_any;
	}
	પાછા argtypes[0].type_curr;
    }
    પાછા &t_void;
}
    સ્થિર type_T *
ret_extend(પૂર્ણાંક argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    જો (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	// if the second argument has a different current type then the current
	// type is "any"
	જો (argcount > 1 && !equal_type(argtypes[0].type_curr,
						     argtypes[1].type_curr, 0))
	{
	    જો (argtypes[0].type_curr->tt_type == VAR_LIST)
		પાછા &t_list_any;
	    જો (argtypes[0].type_curr->tt_type == VAR_DICT)
		પાછા &t_dict_any;
	    જો (argtypes[0].type_curr->tt_type == VAR_BLOB)
		પાછા &t_blob;
	}
	પાછા argtypes[0].type_curr;
    }
    પાછા &t_void;
}
    સ્થિર type_T *
ret_repeat(પૂર્ણાંક argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    જો (argcount == 0)
	પાછા &t_any;
    જો (argtypes[0].type_curr == &t_સંખ્યા)
	પાછા &t_string;
    પાછા argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    સ્થિર type_T *
ret_first_cont(પૂર્ણાંક argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    જો (argcount > 0)
    {
	જો (argtypes[0].type_curr->tt_type == VAR_LIST)
	    પાછા &t_list_any;
	જો (argtypes[0].type_curr->tt_type == VAR_DICT)
	    પાછા &t_dict_any;
	જો (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    પાછા argtypes[0].type_curr;
    }
    પાછા &t_any;
}
// for get(): the type of the item when a missing item has the same type
    સ્થિર type_T *
ret_get(પૂર્ણાંક argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    જો (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*item = ખાલી;

	જો (t->tt_type == VAR_BLOB)
	    item = &t_સંખ્યા;
	નહીંતર જો (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    item = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	જો (item != ખાલી && (argcount == 3
			? equal_type(item, argtypes[2].type_curr, 0)
			: item->tt_type == VAR_સંખ્યા))
	{
	    જો (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    પાછા item;
	}
    }
    પાછા &t_any;
}
// for getline()
    સ્થિર type_T *
ret_getline(પૂર્ણાંક argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    જો (argcount == 1)
	પાછા &t_string;
    *decl_type = &t_list_any;
    પાછા &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    સ્થિર type_T *
ret_string_or_any_1(પૂર્ણાંક argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    જો (argcount < 1)
	પાછા &t_string;
    પાછા &t_any;
}
    સ્થિર type_T *
ret_string_or_any_2(પૂર્ણાંક argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    જો (argcount < 2)
	પાછા &t_string;
    પાછા &t_any;
}
    સ્થિર type_T *
ret_string_or_any_3(પૂર્ણાંક argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    જો (argcount < 3)
	પાછા &t_string;
    પાછા &t_any;
}
    સ્થિર type_T *
ret_string_or_any_4(પૂર્ણાંક argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    જો (argcount < 4)
	પાછા &t_string;
    પાછા &t_any;
}
// for abs()
    સ્થિર type_T *
ret_abs(પૂર્ણાંક argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    જો (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	પાછા &t_float;
    પાછા &t_સંખ્યા;
}
// for sign_define()
    સ્થિર type_T *
ret_sign_define(પૂર્ણાંક argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    જો (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	પાછા &t_list_સંખ્યા;
    પાછા &t_સંખ્યા;
}
// for values(): list of member of first argument
    સ્થિર type_T *
ret_list_member(પૂર્ણાંક argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    જો (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	જો (current_type_gap != ખાલી
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	નહીંતર
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	જો (current_type_gap != ખાલી
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    પાછા get_list_type(t->tt_member, current_type_gap);
    }
    પાછા &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    સ્થિર type_T *
ret_list_or_dict_0(પૂર્ણાંક argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    જો (argcount > 0)
	પાછા &t_dict_any;
    *decl_type = &t_list_any;
    પાછા &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    સ્થિર type_T *
ret_list_or_dict_1(પૂર્ણાંક argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    જો (argcount > 1)
	પાછા &t_dict_any;
    *decl_type = &t_list_any;
    પાછા &t_list_dict_any;
}

    સ્થિર type_T *
ret_argv(પૂર્ણાંક argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // argv() returns list of strings
    જો (argcount == 0)
    {
	*decl_type = &t_list_any;
	પાછા &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    પાછા &t_any;
}

    સ્થિર type_T *
ret_remove(પૂર્ણાંક argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    જો (argcount > 0)
    {
	જો (argtypes[0].type_curr->tt_type == VAR_LIST
		|| argtypes[0].type_curr->tt_type == VAR_DICT)
	{
	    જો (argcount == 3)
	    {
		*decl_type = argtypes[0].type_decl;
		પાછા argtypes[0].type_curr;
	    }
	    જો (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    પાછા argtypes[0].type_curr->tt_member;
	}
	જો (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    પાછા &t_સંખ્યા;
    }
    પાછા &t_any;
}

    સ્થિર type_T *
ret_max_min(પૂર્ણાંક argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    જો (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	જો (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	નહીંતર
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	જો (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	નહીંતર
	    t = &t_any;
	પાછા t;
    }
    પાછા &t_any;
}

    સ્થિર type_T *
ret_getreg(પૂર્ણાંક argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    જો (argcount == 3)
    {
	*decl_type = &t_list_any;
	પાછા &t_list_string;
    }
    પાછા &t_string;
}

    સ્થિર type_T *
ret_virtcol(પૂર્ણાંક argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    જો (argcount > 1)
    {
	*decl_type = &t_list_any;
	પાછા &t_list_સંખ્યા;
    }
    પાછા &t_સંખ્યા;
}

    સ્થિર type_T *
ret_maparg(પૂર્ણાંક argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    // Assume that if the fourth argument is passed it's non-zero
    જો (argcount == 4)
	પાછા &t_dict_any;
    પાછા &t_string;
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
પ્રકારવ્યાખ્યા સંરચના
{
    અક્ષર	*f_name;	// function name
    અક્ષર	f_min_argc;	// minimal number of arguments
    અક્ષર	f_max_argc;	// maximal number of arguments
    અક્ષર	f_argtype;	// for method: FEARG_ values; bits FE_
    argcheck_T	*f_argcheck;	// list of functions to check argument types;
				// use "arg_any" (not NULL) to accept an
				// argument of any type
    અક્ષર	*f_argnames;	// names of the arguments as in the help,
				// separated by ", "; an argument without a
				// name is empty, NULL when there is none
    type_T	*(*f_retfunc)(પૂર્ણાંક argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    શૂન્ય	(*f_func)(typval_T *args, typval_T *rvar);
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

#જો defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#નહીંતર
# define MATH_FUNC(name) ખાલી
#endif
#ifdef FEAT_TIMERS
# define TIMER_FUNC(name) name
#નહીંતર
# define TIMER_FUNC(name) ખાલી
#endif
#ifdef FEAT_JOB_CHANNEL
# define JOB_FUNC(name) name
#નહીંતર
# define JOB_FUNC(name) ખાલી
#endif
#ifdef FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#નહીંતર
# define PROP_FUNC(name) ખાલી
#endif
#ifdef FEAT_SIGNS
# define SIGN_FUNC(name) name
#નહીંતર
# define SIGN_FUNC(name) ખાલી
#endif
#ifdef FEAT_SOUND
# define SOUND_FUNC(name) name
#નહીંતર
# define SOUND_FUNC(name) ખાલી
#endif
#ifdef FEAT_TERMINAL
# define TERM_FUNC(name) name
#નહીંતર
# define TERM_FUNC(name) ખાલી
#endif
#ifdef FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#નહીંતર
# define TABPANEL_FUNC(name) ખાલી
#endif
#ifdef FEAT_IMAGE
# define IMAGE_FUNC(name) name
#નહીંતર
# define IMAGE_FUNC(name) ખાલી
#endif

સ્થિર અચલ funcentry_T global_functions[] =
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
    {"and",		2, 2, FEARG_1,	    arg2_સંખ્યા,
			"expr1, expr2",
			ret_સંખ્યા,	    f_and},
    {"append",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_સંખ્યા_bool,    f_append},
    {"appendbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_સંખ્યા_bool,    f_appendbufline},
    {"argc",		0, 1, 0,	    arg1_સંખ્યા,
			"winid",
			ret_સંખ્યા,	    f_argc},
    {"argidx",		0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_argidx},
    {"arglistid",	0, 2, 0,	    arg2_સંખ્યા,
			"winnr, tabnr",
			ret_સંખ્યા,	    f_arglistid},
    {"argv",		0, 2, 0,	    arg2_સંખ્યા,
			"nr, winid",
			ret_argv,	    f_argv},
    {"asin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_asin},
    {"assert_beeps",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_સંખ્યા_bool,    f_assert_beeps},
    {"assert_equal",	2, 3, FEARG_2,	    ખાલી,
			"expected, actual, msg",
			ret_સંખ્યા_bool,    f_assert_equal},
    {"assert_equalfile", 2, 3, FEARG_1,	    arg3_string,
			"fname_one, fname_two, msg",
			ret_સંખ્યા_bool,    f_assert_equalfile},
    {"assert_exception", 1, 2, 0,	    arg2_string,
			"error, msg",
			ret_સંખ્યા_bool,    f_assert_exception},
    {"assert_fails",	1, 5, FEARG_1,	    arg15_assert_fails,
			"cmd, error, msg, lnum, context",
			ret_સંખ્યા_bool,    f_assert_fails},
    {"assert_false",	1, 2, FEARG_1,	    ખાલી,
			"actual, msg",
			ret_સંખ્યા_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_સંખ્યા_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_સંખ્યા_bool,    f_assert_match},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_સંખ્યા_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    ખાલી,
			"expected, actual, msg",
			ret_સંખ્યા_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_સંખ્યા_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_સંખ્યા_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    ખાલી,
			"actual, msg",
			ret_સંખ્યા_bool,    f_assert_true},
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
    {"balloon_gettext",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,
#ifdef FEAT_BEVAL
	    f_balloon_gettext
#નહીંતર
	    ખાલી
#endif
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#ifdef FEAT_BEVAL
	    f_balloon_show
#નહીંતર
	    ખાલી
#endif
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#જો defined(FEAT_BEVAL_TERM)
	    f_balloon_split
#નહીંતર
	    ખાલી
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
			ret_list_સંખ્યા,    f_blob2list},
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
			ret_સંખ્યા,	    f_bufadd},
    {"bufexists",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_સંખ્યા_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_સંખ્યા_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_સંખ્યા,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_સંખ્યા_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_સંખ્યા_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, create",
			ret_સંખ્યા,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_સંખ્યા,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_સંખ્યા,	    f_bufwinnr},
    {"byte2line",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"byte",
			ret_સંખ્યા,	    f_byte2line},
    {"byteidx",		2, 3, FEARG_1,	    arg3_string_સંખ્યા_bool,
			"expr, nr, utf16",
			ret_સંખ્યા,	    f_byteidx},
    {"byteidxcomp",	2, 3, FEARG_1,	    arg3_string_સંખ્યા_bool,
			"expr, nr, utf16",
			ret_સંખ્યા,	    f_byteidxcomp},
    {"call",		2, 3, FEARG_1,	    arg3_any_list_dict,
			"func, arglist, dict",
			ret_any,	    f_call},
    {"ceil",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ceil},
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_job,
			"handle",
			ret_સંખ્યા_bool,    JOB_FUNC(f_ch_canread)},
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
			ret_સંખ્યા,	    JOB_FUNC(f_ch_getbufnr)},
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
    {"changenr",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_changenr},
    {"char2nr",		1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_સંખ્યા,	    f_char2nr},
    {"charclass",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_સંખ્યા,	    f_charclass},
    {"charcol",		1, 2, FEARG_1,	    arg2_string_or_list_સંખ્યા,
			"expr, winid",
			ret_સંખ્યા,	    f_charcol},
    {"charidx",		2, 4, FEARG_1,	    arg4_string_સંખ્યા_bool_bool,
			"string, idx, countcc, utf16",
			ret_સંખ્યા,	    f_charidx},
    {"chdir",		1, 2, FEARG_1,	    arg2_string,
			"dir, scope",
			ret_string,	    f_chdir},
    {"cindent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_સંખ્યા,	    f_cindent},
    {"clearmatches",	0, 1, FEARG_1,	    arg1_સંખ્યા,
			"win",
			ret_void,	    f_clearmatches},
    {"cmdcomplete_info",0, 0, 0,	    ખાલી,
			ખાલી,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_સંખ્યા,
			"expr, winid",
			ret_સંખ્યા,	    f_col},
    {"complete",	2, 2, FEARG_2,	    arg2_સંખ્યા_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_સંખ્યા,	    f_complete_add},
    {"complete_check",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_સંખ્યા_string,
			"msg, choices, default, type",
			ret_સંખ્યા,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    ખાલી,
			"expr",
			ret_copy,	    f_copy},
    {"cos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cos},
    {"cosh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cosh},
    {"count",		2, 4, FEARG_1,	    arg24_ગણતરી,
			"comp, expr, ic, start",
			ret_સંખ્યા,	    f_ગણતરી},
    {"cscope_connection",0,3, 0,	    arg3_સંખ્યા_string_string,
			"num, dbpath, prepend",
			ret_સંખ્યા,	    f_cscope_connection},
    {"cursor",		1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_સંખ્યા,	    f_cursor},
    {"debugbreak",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"pid",
			ret_સંખ્યા,
#ifdef MSWIN
	    f_debugbreak
#નહીંતર
	    ખાલી
#endif
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_સંખ્યા_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_buffer_lnum_lnum,
			"buf, first, last",
			ret_સંખ્યા_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા_bool,    f_did_filetype},
    {"diff",		2, 3, FEARG_1,	    arg3_diff,
			"fromlist, tolist, options",
			ret_string_or_any_3,  f_diff},
    {"diff_filler",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_સંખ્યા,	    f_diff_filler},
    {"diff_hlID",	2, 2, FEARG_1,	    arg2_lnum_સંખ્યા,
			"lnum, col",
			ret_સંખ્યા,	    f_diff_hlID},
    {"digraph_get",	1, 1, FEARG_1,	    arg1_string,
			"chars",
			ret_string,	    f_digraph_get},
    {"digraph_getlist",	0, 1, FEARG_1,	    arg1_bool,
			"listall",
			ret_list_string_વસ્તુઓ, f_digraph_getlist},
    {"digraph_set",	2, 2, FEARG_1,	    arg2_string,
			"chars, digraph",
			ret_bool,	f_digraph_set},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    ખાલી,
			"expr",
			ret_સંખ્યા_bool,    f_empty},
    {"environ",		0, 0, 0,	    ખાલી,
			ખાલી,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    ખાલી,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા_bool,    f_eventhandler},
    {"executable",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_સંખ્યા,	    f_executable},
    {"execute",		1, 2, FEARG_1,	    arg12_execute,
			"command, silent",
			ret_string,	    f_execute},
    {"exepath",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_exepath},
    {"exists",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_સંખ્યા_bool,    f_exists},
    {"exists_compiled",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_સંખ્યા_bool,    f_exists_compiled},
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
			ret_સંખ્યા_bool,    f_filereadable},
    {"filecopy",	2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_સંખ્યા_bool,    f_filecopy},
    {"filereadable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_સંખ્યા_bool,    f_filereadable},
    {"filewritable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_સંખ્યા,	    f_filewritable},
    {"filter",		2, 2, FEARG_1,	    arg2_filter,
			"expr1, expr2",
			ret_first_arg,	    f_filter},
    {"finddir",		1, 3, FEARG_1,	    arg3_string_string_સંખ્યા,
			"name, path, count",
			ret_string_or_any_3,	    f_finddir},
    {"findfile",	1, 3, FEARG_1,	    arg3_string_string_સંખ્યા,
			"name, path, count",
			ret_string_or_any_3,	    f_findfile},
    {"flatten",		1, 2, FEARG_1,	    arg2_list_any_સંખ્યા,
			"list, maxdepth",
			ret_list_any,	    f_flatten},
    {"flattennew",	1, 2, FEARG_1,	    arg2_list_any_સંખ્યા,
			"list, maxdepth",
			ret_list_any,	    f_flattennew},
    {"float2nr",	1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_સંખ્યા,	    f_float2nr},
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
    {"foldclosed",	1, 2, FEARG_1,	    arg2_lnum_સંખ્યા,
			"lnum, winid",
			ret_સંખ્યા,	    f_foldclosed},
    {"foldclosedend",	1, 2, FEARG_1,	    arg2_lnum_સંખ્યા,
			"lnum, winid",
			ret_સંખ્યા,	    f_foldclosedend},
    {"foldlevel",	1, 2, FEARG_1,	    arg2_lnum_સંખ્યા,
			"lnum, winid",
			ret_સંખ્યા,	    f_foldlevel},
    {"foldtext",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_સંખ્યા,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_first_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    ખાલી,
			ખાલી,
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
    {"getbgcolor",	0, 0, 0,	    ખાલી,
			ખાલી,
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
    {"getcellpixels",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_સંખ્યા,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_getcmdwintype},
    {"getcompletion",	2, 3, FEARG_1,	    arg3_string_string_bool,
			"pat, type, filtered",
			ret_list_string,    f_getcompletion},
    {"getcompletiontype", 1, 1, FEARG_1,    arg1_string,
			"pat",
			ret_string,	    f_getcompletiontype},
    {"getcurpos",	0, 1, FEARG_1,	    arg1_સંખ્યા,
			"winid",
			ret_list_સંખ્યા,    f_getcurpos},
    {"getcursorcharpos", 0, 1, FEARG_1,	    arg1_સંખ્યા,
			"winid",
			ret_list_સંખ્યા,    f_getcursorcharpos},
    {"getcwd",		0, 2, FEARG_1,	    arg2_સંખ્યા,
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
			ret_સંખ્યા,	    f_getfsize},
    {"getftime",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_સંખ્યા,	    f_getftime},
    {"getftype",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getftype},
    {"getimstatus",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા_bool,    f_getimstatus},
    {"getinfo",		2, 3, FEARG_2,	    arg3_string_string_dict,
			"kind, name, opts",
			ret_dict_any,	    f_getinfo},
    {"getjumplist",	0, 2, FEARG_1,	    arg2_સંખ્યા,
			"winnr, tabnr",
			ret_list_any,	    f_getjumplist},
    {"getline",		1, 2, FEARG_1,	    arg2_lnum,
			"lnum, end",
			ret_getline,	    f_getline},
    {"getloclist",	1, 2, 0,	    arg2_સંખ્યા_dict_any,
			"nr, what",
			ret_list_or_dict_1, f_getloclist},
    {"getmarklist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_સંખ્યા,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_dict_સંખ્યા,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_getpid},
    {"getpos",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_સંખ્યા,    f_getpos},
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
    {"getstacktrace",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_list_dict_any,  f_getstacktrace},
    {"gettabinfo",	0, 1, FEARG_1,	    arg1_સંખ્યા,
			"tabnr",
			ret_list_dict_any,  f_gettabinfo},
    {"gettabvar",	2, 3, FEARG_1,	    arg3_સંખ્યા_string_any,
			"tabnr, varname, def",
			ret_any,	    f_gettabvar},
    {"gettabwinvar",	3, 4, FEARG_1,	    arg4_સંખ્યા_સંખ્યા_string_any,
			"tabnr, winnr, varname, def",
			ret_any,	    f_gettabwinvar},
    {"gettagstack",	0, 1, FEARG_1,	    arg1_સંખ્યા,
			"winnr",
			ret_dict_any,	    f_gettagstack},
    {"gettext",		1, 2, FEARG_1,	    arg2_string,
			"text, package",
			ret_string,	    f_gettext},
    {"getwininfo",	0, 1, FEARG_1,	    arg1_સંખ્યા,
			"winid",
			ret_list_dict_any,  f_getwininfo},
    {"getwinpos",	0, 1, FEARG_1,	    arg1_સંખ્યા,
			"timeout",
			ret_list_સંખ્યા,    f_getwinpos},
    {"getwinposx",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_getwinposy},
    {"getwinvar",	2, 3, FEARG_1,	    arg3_સંખ્યા_string_any,
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
			ret_સંખ્યા_bool,    f_has},
    {"has_key",		2, 2, FEARG_1,	    arg2_dict_any_string_or_nr,
			"dict, key",
			ret_સંખ્યા_bool,    f_has_ચાવી},
    {"haslocaldir",	0, 2, FEARG_1,	    arg2_સંખ્યા,
			"winnr, tabnr",
			ret_સંખ્યા,	    f_haslocaldir},
    {"hasmapto",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"what, mode, abbr",
			ret_સંખ્યા_bool,    f_hasmapto},
    {"highlightID",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_સંખ્યા,	    f_hlID},
    {"highlight_exists",1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_સંખ્યા_bool,    f_hlexists},
    {"histadd",		2, 2, FEARG_2,	    arg2_string,
			"history, item",
			ret_સંખ્યા_bool,    f_histadd},
    {"histdel",		1, 2, FEARG_1,	    arg2_string_string_or_સંખ્યા,
			"history, item",
			ret_સંખ્યા_bool,    f_histdel},
    {"histget",		1, 2, FEARG_1,	    arg2_string_સંખ્યા,
			"history, index",
			ret_string,	    f_histget},
    {"histnr",		1, 1, FEARG_1,	    arg1_string,
			"history",
			ret_સંખ્યા,	    f_histnr},
    {"hlID",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_સંખ્યા,	    f_hlID},
    {"hlexists",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_સંખ્યા_bool,    f_hlexists},
    {"hlget",		0, 2, FEARG_1,	    arg2_string_bool,
			"name, resolve",
			ret_list_dict_any,  f_hlget},
    {"hlset",		1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_સંખ્યા_bool,    f_hlset},
    {"hostname",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    ખાલી,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_સંખ્યા,	    IMAGE_FUNC(f_image_add)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_સંખ્યા,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_સંખ્યા,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_index,
			"object, expr, start, ic",
			ret_સંખ્યા,	    f_index},
    {"indexof",		2, 3, FEARG_1,	    arg23_index,
			"object, expr, opts",
			ret_સંખ્યા,	    f_indexof},
    {"input",		1, 3, FEARG_1,	    arg3_string,
			"prompt, text, completion",
			ret_string,	    f_આગત},
    {"inputdialog",	1, 3, FEARG_1,	    arg3_string,
			"prompt, text, cancelreturn",
			ret_string,	    f_inputdialog},
    {"inputlist",	1, 1, FEARG_1,	    arg1_list_string,
			"textlist",
			ret_સંખ્યા,	    f_inputlist},
    {"inputrestore",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_ઉમેરો,
			"object, item, idx",
			ret_first_arg,	    f_ઉમેરો},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_void,	    f_interrupt},
    {"invert",		1, 1, FEARG_1,	    arg1_સંખ્યા,
			"expr",
			ret_સંખ્યા,	    f_invert},
    {"isabsolutepath",	1, 1, FEARG_1,	    arg1_string,
			"path",
			ret_સંખ્યા_bool,    f_isabsolutepath},
    {"isdirectory",	1, 1, FEARG_1,	    arg1_string,
			"directory",
			ret_સંખ્યા_bool,    f_isdirectory},
    {"isinf",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_સંખ્યા,	    MATH_FUNC(f_isinf)},
    {"islocked",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_સંખ્યા_bool,    f_islocked},
    {"isnan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_સંખ્યા_bool,    MATH_FUNC(f_isnan)},
    {"items",		1, 1, FEARG_1,	    arg1_list_tuple_dict_blob_or_string,
			"expr",
			ret_list_વસ્તુઓ,	    f_વસ્તુઓ},
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
    {"job_stop",	1, 2, FEARG_1,	    arg2_job_string_or_સંખ્યા,
			"job, how",
			ret_સંખ્યા_bool,    JOB_FUNC(f_job_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    ખાલી,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    ખાલી,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_ચાવીઓ},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    ખાલી,	// obsolete
			ખાલી,
			ret_સંખ્યા,	    f_last_buffer_nr},
    {"len",		1, 1, FEARG_1,	    arg1_len,
			"expr",
			ret_સંખ્યા,	    f_len},
    {"libcall",		3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_string,	    f_libcall},
    {"libcallnr",	3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_સંખ્યા,	    f_libcallnr},
    {"line",		1, 2, FEARG_1,	    arg2_string_સંખ્યા,
			"expr, winid",
			ret_સંખ્યા,	    f_લીટી},
    {"line2byte",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_સંખ્યા,	    f_line2byte},
    {"lispindent",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_સંખ્યા,	    f_lispindent},
    {"list2blob",	1, 1, FEARG_1,	    arg1_list_સંખ્યા,
			"list",
			ret_blob,	    f_list2blob},
    {"list2str",	1, 2, FEARG_1,	    arg2_list_સંખ્યા_bool,
			"list, utf8",
			ret_string,	    f_list2str},
    {"list2tuple",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_tuple_any,	    f_list2tuple},
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_buffer_bool_or_dict,
			"callback, buf, options",
			ret_સંખ્યા,	    f_listener_add},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"id",
			ret_સંખ્યા_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_localtime},
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
#નહીંતર
		ખાલી
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
			ret_સંખ્યા,	    f_match},
    {"matchadd",	2, 5, FEARG_1,	    arg25_matchadd,
			"group, pattern, priority, id, dict",
			ret_સંખ્યા,	    f_matchadd},
    {"matchaddpos",	2, 5, FEARG_1,	    arg25_matchaddpos,
			"group, pos, priority, id, dict",
			ret_સંખ્યા,	    f_matchaddpos},
    {"matcharg",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"nr",
			ret_list_string,    f_matcharg},
    {"matchbufline",	4, 5, FEARG_1,	    arg45_matchbufline,
			"buf, pat, lnum, end, dict",
			ret_list_any,	    f_matchbufline},
    {"matchdelete",	1, 2, FEARG_1,	    arg2_સંખ્યા,
			"id, win",
			ret_સંખ્યા_bool,    f_matchdelete},
    {"matchend",	2, 4, FEARG_1,	    arg24_match_func,
			"expr, pat, start, count",
			ret_સંખ્યા,	    f_matchend},
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
#નહીંતર
	    ખાલી
#endif
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_સંખ્યા,
			"name, flags, prot",
			ret_સંખ્યા_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_mode},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_MZSCHEME
	    f_mzeval
#નહીંતર
	    ખાલી
#endif
			},
    {"nextnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_સંખ્યા,	    f_nextnonblank},
    {"ngettext",	3, 4, FEARG_3,	    arg4_string_string_સંખ્યા_string,
			"single, plural, number, domain",
			ret_string,	    f_ngettext},
    {"nr2char",		1, 2, FEARG_1,	    arg2_સંખ્યા_bool,
			"expr, utf8",
			ret_string,	    f_nr2char},
    {"or",		2, 2, FEARG_1,	    arg2_સંખ્યા,
			"expr1, expr2",
			ret_સંખ્યા,	    f_or},
    {"pathshorten",	1, 2, FEARG_1,	    arg2_string_સંખ્યા,
			"path, len",
			ret_string,	    f_pathshorten},
    {"perleval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_PERL
	    f_perleval
#નહીંતર
	    ખાલી
#endif
			},
    {"popup_atcursor",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_સંખ્યા,	    PROP_FUNC(f_popup_atcursor)},
    {"popup_beval",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_સંખ્યા,	    PROP_FUNC(f_popup_beval)},
    {"popup_clear",	0, 1, 0,	    arg1_bool,
			"force",
			ret_void,	    PROP_FUNC(f_popup_clear)},
    {"popup_close",	1, 2, FEARG_1,	    arg2_સંખ્યા_any,
			"id, result",
			ret_void,	    PROP_FUNC(f_popup_close)},
    {"popup_create",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_સંખ્યા,	    PROP_FUNC(f_popup_create)},
    {"popup_dialog",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_સંખ્યા,	    PROP_FUNC(f_popup_dialog)},
    {"popup_filter_menu", 2, 2, 0,	    arg2_સંખ્યા_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_menu)},
    {"popup_filter_yesno", 2, 2, 0,	    arg2_સંખ્યા_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_yesno)},
    {"popup_findecho",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    PROP_FUNC(f_popup_findpreview)},
    {"popup_getoptions", 1, 1, FEARG_1,	    arg1_સંખ્યા,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getoptions)},
    {"popup_getpos",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getpos)},
    {"popup_hide",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"id",
			ret_void,	    PROP_FUNC(f_popup_hide)},
    {"popup_list",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_list_સંખ્યા,    PROP_FUNC(f_popup_list)},
    {"popup_locate",	2, 2, 0,	    arg2_સંખ્યા,
			"row, col",
			ret_સંખ્યા,	    PROP_FUNC(f_popup_locate)},
    {"popup_menu",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_સંખ્યા,	    PROP_FUNC(f_popup_menu)},
    {"popup_move",	2, 2, FEARG_1,	    arg2_સંખ્યા_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_move)},
    {"popup_notification", 2, 2, FEARG_1,   arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_સંખ્યા,	    PROP_FUNC(f_popup_notification)},
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_સંખ્યા_buffer,
			"id, buf",
			ret_સંખ્યા_bool,    PROP_FUNC(f_popup_setbuf)},
    {"popup_setoptions", 2, 2, FEARG_1,	    arg2_સંખ્યા_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_setoptions)},
    {"popup_settext",	2, 2, FEARG_1,	    arg2_સંખ્યા_string_or_list,
			"id, text",
			ret_void,	    PROP_FUNC(f_popup_settext)},
    {"popup_show",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"id",
			ret_સંખ્યા,	    PROP_FUNC(f_popup_show)},
    {"pow",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"x, y",
			ret_float,	    f_pow},
    {"preinserted",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_સંખ્યા,	    f_prevnonblank},
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
    {"prop_add",	3, 3, FEARG_1,	    arg3_સંખ્યા_સંખ્યા_dict,
			"lnum, col, props",
			ret_સંખ્યા,	    PROP_FUNC(f_prop_add)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_add_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_સંખ્યા_સંખ્યા_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_find)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_સંખ્યા_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_સંખ્યા_સંખ્યા,
			"props, lnum, lnum_end",
			ret_સંખ્યા,	    PROP_FUNC(f_prop_remove)},
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
    {"pum_getpos",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_dict_સંખ્યા,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON3
	    f_py3eval
#નહીંતર
	    ખાલી
#endif
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON
	    f_pyeval
#નહીંતર
	    ખાલી
#endif
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#જો defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#નહીંતર
	    ખાલી
#endif
			},
    {"rand",		0, 1, FEARG_1,	    arg1_list_સંખ્યા,
			"expr",
			ret_સંખ્યા,	    f_rand},
    {"range",		1, 3, FEARG_1,	    arg3_સંખ્યા,
			"expr, max, stride",
			ret_list_સંખ્યા,    f_range},
    {"readblob",	1, 3, FEARG_1,	    arg3_string_સંખ્યા_સંખ્યા,
			"fname, offset, size",
			ret_blob,	    f_readblob},
    {"readdir",		1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_string,    f_readdir},
    {"readdirex",	1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_dict_any,  f_readdirex},
    {"readfile",	1, 3, FEARG_1,	    arg3_string_string_સંખ્યા,
			"fname, type, max",
			ret_list_string,    f_readfile},
    {"redraw_listener_add", 1, 1, FEARG_1,  arg1_dict_any,
			"opts",
			ret_સંખ્યા,	    f_redraw_listener_add},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_સંખ્યા,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_reg_recording},
    {"reltime",		0, 2, FEARG_1,	    arg2_list_સંખ્યા,
			"start, end",
			ret_list_સંખ્યા,    f_reltime},
    {"reltimefloat",	1, 1, FEARG_1,	    arg1_list_સંખ્યા,
			"time",
			ret_float,	    f_reltimefloat},
    {"reltimestr",	1, 1, FEARG_1,	    arg1_list_સંખ્યા,
			"time",
			ret_string,	    f_reltimestr},
    {"remote_expr",	2, 4, FEARG_1,	    arg24_remote_પદાવલિ,
			"server, string, idvar, timeout",
			ret_string,	    f_remote_પદાવલિ},
    {"remote_foreground", 1, 1, FEARG_1,    arg1_string,
			"server",
			ret_void,	    f_remote_foreground},
    {"remote_peek",	1, 2, FEARG_1,	    arg2_string,
			"serverid, retvar",
			ret_સંખ્યા,	    f_remote_peek},
    {"remote_read",	1, 2, FEARG_1,	    arg2_string_સંખ્યા,
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
			ret_સંખ્યા_bool,    f_rename},
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
#નહીંતર
	    ખાલી
#endif
			},
    {"screenattr",	2, 2, FEARG_1,	    arg2_સંખ્યા,
			"row, col",
			ret_સંખ્યા,	    f_screenattr},
    {"screenchar",	2, 2, FEARG_1,	    arg2_સંખ્યા,
			"row, col",
			ret_સંખ્યા,	    f_screenchar},
    {"screenchars",	2, 2, FEARG_1,	    arg2_સંખ્યા,
			"row, col",
			ret_list_સંખ્યા,    f_screenchars},
    {"screencol",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_સંખ્યા,
			"winid, lnum, col",
			ret_dict_સંખ્યા,    f_screenpos},
    {"screenrow",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_screenrow},
    {"screenstring",	2, 2, FEARG_1,	    arg2_સંખ્યા,
			"row, col",
			ret_string,	    f_screenstring},
    {"search",		1, 5, FEARG_1,	    arg15_શોધ,
			"pattern, flags, stopline, timeout, skip",
			ret_સંખ્યા,	    f_શોધ},
    {"searchcount",	0, 1, FEARG_1,	    arg1_dict_any,
			"options",
			ret_dict_સંખ્યા,    f_searchcount},
    {"searchdecl",	1, 3, FEARG_1,	    arg3_string_bool_bool,
			"name, global, thisblock",
			ret_સંખ્યા_bool,    f_searchdecl},
    {"searchpair",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_સંખ્યા,	    f_searchpair},
    {"searchpairpos",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_list_સંખ્યા,    f_searchpairpos},
    {"searchpos",	1, 5, FEARG_1,	    arg15_શોધ,
			"pattern, flags, stopline, timeout, skip",
			ret_list_સંખ્યા,    f_searchpos},
    {"server2client",	2, 2, FEARG_1,	    arg2_string,
			"clientid, string",
			ret_સંખ્યા_bool,    f_server2client},
    {"serverlist",	0, 1, 0,	    arg1_dict_any,
			"dict",
			ret_string_or_any_1,	    f_serverlist},
    {"setbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_સંખ્યા_bool,    f_setbufline},
    {"setbufvar",	3, 3, FEARG_3,	    arg3_buffer_string_any,
			"buf, varname, val",
			ret_void,	    f_setbufvar},
    {"setcellwidths",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_void,	    f_setcellwidths},
    {"setcharpos",	2, 2, FEARG_2,	    arg2_string_list_સંખ્યા,
			"expr, list",
			ret_સંખ્યા_bool,    f_setcharpos},
    {"setcharsearch",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_setcharsearch},
    {"setcmdline",	1, 2, FEARG_1,	    arg2_string_સંખ્યા,
			"str, pos",
			ret_સંખ્યા_bool,    f_setcmdline},
    {"setcmdpos",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"pos",
			ret_સંખ્યા_bool,    f_setcmdpos},
    {"setcursorcharpos", 1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_સંખ્યા_bool,    f_setcursorcharpos},
    {"setenv",		2, 2, FEARG_2,	    arg2_string_any,
			"name, val",
			ret_void,	    f_setenv},
    {"setfperm",	2, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_સંખ્યા_bool,    f_setfperm},
    {"setline",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_સંખ્યા_bool,    f_setline},
    {"setloclist",	2, 4, FEARG_2,	    arg24_setloclist,
			"nr, list, action, what",
			ret_સંખ્યા_bool,    f_setloclist},
    {"setmatches",	1, 2, FEARG_1,	    arg2_list_any_સંખ્યા,
			"list, win",
			ret_સંખ્યા_bool,    f_setmatches},
    {"setpos",		2, 2, FEARG_2,	    arg2_string_list_સંખ્યા,
			"expr, list",
			ret_સંખ્યા_bool,    f_setpos},
    {"setqflist",	1, 3, FEARG_1,	    arg13_setqflist,
			"list, action, what",
			ret_સંખ્યા_bool,    f_setqflist},
    {"setreg",		2, 3, FEARG_2,	    arg3_string_any_string,
			"regname, value, options",
			ret_સંખ્યા_bool,    f_setreg},
    {"settabvar",	3, 3, FEARG_3,	    arg3_સંખ્યા_string_any,
			"tabnr, varname, val",
			ret_void,	    f_settabvar},
    {"settabwinvar",	4, 4, FEARG_4,	    arg4_સંખ્યા_સંખ્યા_string_any,
			"tabnr, winnr, varname, val",
			ret_void,	    f_settabwinvar},
    {"settagstack",	2, 3, FEARG_2,	    arg23_settagstack,
			"nr, dict, action",
			ret_સંખ્યા_bool,    f_settagstack},
    {"setwinvar",	3, 3, FEARG_3,	    arg3_સંખ્યા_string_any,
			"winnr, varname, val",
			ret_void,	    f_setwinvar},
    {"sha256",		1, 1, FEARG_1,	    arg1_string_or_blob,
			"expr",
			ret_string,
#ifdef FEAT_CRYPT
	    f_sha256
#નહીંતર
	    ખાલી
#endif
			},
    {"shellescape",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, special",
			ret_string,	    f_shellescape},
    {"shiftwidth",	0, 1, FEARG_1,	    arg1_સંખ્યા,
			"col",
			ret_સંખ્યા,	    f_shiftwidth},
    {"sign_define",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"name, dict",
			ret_sign_define,	    SIGN_FUNC(f_sign_define)},
    {"sign_getdefined",	0, 1, FEARG_1,	    arg1_string,
			"name",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getdefined)},
    {"sign_getplaced",	0, 2, FEARG_1,	    arg02_sign_getplaced,
			"buf, dict",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getplaced)},
    {"sign_jump",	3, 3, FEARG_1,	    arg3_સંખ્યા_string_buffer,
			"id, group, buf",
			ret_સંખ્યા,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_place,
			"id, group, name, buf, dict",
			ret_સંખ્યા,	    SIGN_FUNC(f_sign_place)},
    {"sign_placelist",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_સંખ્યા,    SIGN_FUNC(f_sign_placelist)},
    {"sign_undefine",	0, 1, FEARG_1,	    arg1_string_or_list_string,
			"name",
			ret_સંખ્યા_bool,    SIGN_FUNC(f_sign_undefine)},
    {"sign_unplace",	1, 2, FEARG_1,	    arg2_string_dict,
			"group, dict",
			ret_સંખ્યા_bool,    SIGN_FUNC(f_sign_unplace)},
    {"sign_unplacelist", 1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_સંખ્યા,    SIGN_FUNC(f_sign_unplacelist)},
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
    {"sound_clear",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_void,	    SOUND_FUNC(f_sound_clear)},
    {"sound_playevent",	1, 2, FEARG_1,	    arg2_string_any,
			"name, callback",
			ret_સંખ્યા,	    SOUND_FUNC(f_sound_playevent)},
    {"sound_playfile",	1, 2, FEARG_1,	    arg2_string_any,
			"path, callback",
			ret_સંખ્યા,	    SOUND_FUNC(f_sound_playfile)},
    {"sound_stop",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"id",
			ret_void,	    SOUND_FUNC(f_sound_stop)},
    {"soundfold",	1, 1, FEARG_1,	    arg1_string,
			"word",
			ret_string,	    f_soundfold},
    {"spellbadword",	0, 1, FEARG_1,	    arg1_string,
			"sentence",
			ret_list_string,    f_spellbadword},
    {"spellsuggest",	1, 3, FEARG_1,	    arg3_string_સંખ્યા_bool,
			"word, max, capital",
			ret_list_string,    f_spellsuggest},
    {"split",		1, 3, FEARG_1,	    arg3_string_string_bool,
			"string, pattern, keepempty",
			ret_list_string,    f_split},
    {"sqrt",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sqrt},
    {"srand",		0, 1, FEARG_1,	    arg1_સંખ્યા,
			"expr",
			ret_list_સંખ્યા,    f_srand},
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
			ret_list_સંખ્યા,    f_str2list},
    {"str2nr",		1, 3, FEARG_1,	    arg3_string_સંખ્યા_bool,
			"string, base, quoted",
			ret_સંખ્યા,	    f_str2nr},
    {"strcharlen",	1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_સંખ્યા,	    f_strcharlen},
    {"strcharpart",	2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, skipcc",
			ret_string,	    f_strcharpart},
    {"strchars",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, skipcc",
			ret_સંખ્યા,	    f_strchars},
    {"strdisplaywidth",	1, 2, FEARG_1,	    arg2_string_સંખ્યા,
			"string, col",
			ret_સંખ્યા,	    f_strdisplaywidth},
    {"strftime",	1, 2, FEARG_1,	    arg2_string_સંખ્યા,
			"format, time",
			ret_string,
#ifdef HAVE_STRFTIME
	    f_strftime
#નહીંતર
	    ખાલી
#endif
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_સંખ્યા,
			"str, index",
			ret_સંખ્યા,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_સંખ્યા,
			"haystack, needle, start",
			ret_સંખ્યા,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, ખાલી,
			"expr",
			ret_string,	    f_string},
    {"strlen",		1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_સંખ્યા,	    f_strlen},
    {"strpart",		2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, chars",
			ret_string,	    f_strpart},
    {"strptime",	2, 2, FEARG_1,	    arg2_string,
			"format, timestring",
			ret_સંખ્યા,
#ifdef HAVE_STRPTIME
	    f_strptime
#નહીંતર
	    ખાલી
#endif
			},
    {"strridx",		2, 3, FEARG_1,	    arg3_string_string_સંખ્યા,
			"haystack, needle, start",
			ret_સંખ્યા,	    f_strridx},
    {"strtrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_strtrans},
    {"strutf16len",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, countcc",
			ret_સંખ્યા,	    f_strutf16len},
    {"strwidth",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_સંખ્યા,	    f_strwidth},
    {"submatch",	1, 2, FEARG_1,	    arg2_સંખ્યા_bool,
			"nr, list",
			ret_string_or_any_2,	    f_submatch},
    {"substitute",	4, 4, FEARG_1,	    arg4_string_string_any_string,
			"string, pat, sub, flags",
			ret_string,	    f_substitute},
    {"swapfilelist",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_swapname},
    {"synID",		3, 3, 0,	    arg3_lnum_સંખ્યા_bool,
			"lnum, col, trans",
			ret_સંખ્યા,	    f_synID},
    {"synIDattr",	2, 3, FEARG_1,	    arg3_સંખ્યા_string_string,
			"synID, what, mode",
			ret_string,	    f_synIDattr},
    {"synIDtrans",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"synID",
			ret_સંખ્યા,	    f_synIDtrans},
    {"synconcealed",	2, 2, 0,	    arg2_lnum_સંખ્યા,
			"lnum, col",
			ret_list_any,	    f_synconcealed},
    {"synstack",	2, 2, 0,	    arg2_lnum_સંખ્યા,
			"lnum, col",
			ret_list_સંખ્યા,    f_synstack},
    {"system",		1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_string,	    f_system},
    {"systemlist",	1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_list_string,    f_systemlist},
    {"tabpagebuflist",	0, 1, FEARG_1,	    arg1_સંખ્યા,
			"arg",
			ret_list_સંખ્યા,    f_tabpagebuflist},
    {"tabpagenr",	0, 1, 0,	    arg1_string,
			"arg",
			ret_સંખ્યા,	    f_tabpagenr},
    {"tabpagewinnr",	1, 2, FEARG_1,	    arg2_સંખ્યા_string,
			"tabarg, arg",
			ret_સંખ્યા,	    f_tabpagewinnr},
    {"tabpanel_getinfo", 0, 0, 0,	    ખાલી,
			ખાલી,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_સંખ્યા_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    ખાલી,
			ખાલી,
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
    {"tempname",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_સંખ્યા,	    TERM_FUNC(f_term_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_સંખ્યા,	    TERM_FUNC(f_term_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_buffer_string_dict,
			"buf, filename, options",
			ret_void,	    TERM_FUNC(f_term_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_buffer,
			"buf",
			ret_સંખ્યા,	    TERM_FUNC(f_term_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_buffer,
			"buf",
			ret_list_string,
#જો defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_getansicolors
#નહીંતર
	    ખાલી
#endif
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_સંખ્યા_string,
			"attr, what",
			ret_સંખ્યા,	    TERM_FUNC(f_term_getattr)},
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
			ret_સંખ્યા,	    TERM_FUNC(f_term_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_સંખ્યા,    TERM_FUNC(f_term_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, input",
			ret_string,	    TERM_FUNC(f_term_gettty)},
    {"term_list",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_list_સંખ્યા,    TERM_FUNC(f_term_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_buffer_lnum,
			"buf, row",
			ret_list_dict_any,  TERM_FUNC(f_term_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_buffer_string,
			"buf, keys",
			ret_void,	    TERM_FUNC(f_term_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_buffer_list_any,
			"buf, colors",
			ret_void,
#જો defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_setansicolors
#નહીંતર
	    ખાલી
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
    {"term_setsize",	3, 3, FEARG_1,	    arg3_buffer_સંખ્યા_સંખ્યા,
			"buf, rows, cols",
			ret_void,	    TERM_FUNC(f_term_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_સંખ્યા,	    TERM_FUNC(f_term_start)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_buffer_સંખ્યા,
			"buf, time",
			ret_void,	    TERM_FUNC(f_term_wait)},
    {"terminalprops",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_સંખ્યા,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    ખાલી,
			ખાલી,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   ખાલી,
			ખાલી,
			ret_void,	    f_test_garbagecollect_soon},
    {"test_getvalue",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_સંખ્યા,	    f_test_getvalue},
    {"test_gui_event",	2, 2, FEARG_1,	    arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_gui_event},
    {"test_ignore_error", 1, 1, FEARG_1,    arg1_string,
			"expr",
			ret_void,	    f_test_ignore_error},
    {"test_mswin_event", 2, 2, FEARG_1,     arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_mswin_event},
    {"test_null_blob",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    ખાલી,
			ખાલી,
			ret_channel,	    JOB_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    ખાલી,
			ખાલી,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_job,	    JOB_FUNC(f_test_null_job)},
    {"test_null_list",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    ખાલી,
			ખાલી,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_set},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_સંખ્યા,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, ખાલી,
			"expr",
			ret_સંખ્યા,	    f_test_refcount},
    {"test_setmouse",	2, 2, 0,	    arg2_સંખ્યા,
			"row, col",
			ret_void,	    f_test_setmouse},
    {"test_settime",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"expr",
			ret_void,	    f_test_settime},
    {"test_srand_seed",	0, 1, FEARG_1,	    arg1_સંખ્યા,
			"seed",
			ret_void,	    f_test_srand_seed},
    {"test_unknown",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_સંખ્યા,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_સંખ્યા_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_સંખ્યા_any_dict,
			"time, callback, options",
			ret_સંખ્યા,	    TIMER_FUNC(f_timer_start)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    ખાલી,
			ખાલી,
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
    {"trim",		1, 3, FEARG_1,	    arg3_string_string_સંખ્યા,
			"text, mask, dir",
			ret_string,	    f_trim},
    {"trunc",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_trunc},
    {"tuple2list",	1, 1, FEARG_1,	    arg1_tuple_any,
			"tuple",
			ret_list_any,	    f_tuple2list},
    {"type",		1, 1, FEARG_1|FE_X, ખાલી,
			"expr",
			ret_સંખ્યા,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, ખાલી,
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
    {"utf16idx",	2, 4, FEARG_1,	    arg4_string_સંખ્યા_bool_bool,
			"string, idx, countcc, charidx",
			ret_સંખ્યા,	    f_utf16idx},
    {"values",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_member,    f_કિંમતો},
    {"virtcol",		1, 3, FEARG_1,	    arg3_string_or_list_bool_સંખ્યા,
			"expr, list, winid",
			ret_virtcol,	    f_virtcol},
    {"virtcol2col",	3, 3, FEARG_1,	    arg3_સંખ્યા,
			"winid, lnum, col",
			ret_સંખ્યા,	    f_virtcol2col},
    {"visualmode",	0, 1, 0,	    arg1_bool,
			"expr",
			ret_string,	    f_visualmode},
    {"wildmenumode",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_void,	    f_wildtrigger},
    {"win_execute",	2, 3, FEARG_2,	    arg23_win_execute,
			"id, command, silent",
			ret_string,	    f_win_execute},
    {"win_findbuf",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"bufnr",
			ret_list_સંખ્યા,    f_win_findbuf},
    {"win_getid",	0, 2, FEARG_1,	    arg2_સંખ્યા,
			"win, tab",
			ret_સંખ્યા,	    f_win_getid},
    {"win_gettype",	0, 1, FEARG_1,	    arg1_સંખ્યા,
			"nr",
			ret_string,	    f_win_gettype},
    {"win_gotoid",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"expr",
			ret_સંખ્યા_bool,    f_win_gotoid},
    {"win_id2tabwin",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"expr",
			ret_list_સંખ્યા,    f_win_id2tabwin},
    {"win_id2win",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"expr",
			ret_સંખ્યા,	    f_win_id2win},
    {"win_move_separator", 2, 2, FEARG_1,   arg2_સંખ્યા,
			"nr, offset",
			ret_સંખ્યા_bool,    f_win_move_separator},
    {"win_move_statusline", 2, 2, FEARG_1,  arg2_સંખ્યા,
			"nr, offset",
			ret_સંખ્યા_bool,    f_win_move_statusline},
    {"win_screenpos",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"nr",
			ret_list_સંખ્યા,    f_win_screenpos},
    {"win_splitmove",   2, 3, FEARG_1,	    arg3_સંખ્યા_સંખ્યા_dict,
			"nr, target, options",
			ret_સંખ્યા_bool,    f_win_splitmove},
    {"winbufnr",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"nr",
			ret_સંખ્યા,	    f_winbufnr},
    {"wincol",		0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"nr",
			ret_સંખ્યા,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_સંખ્યા,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    ખાલી,
			ખાલી,
			ret_સંખ્યા,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_સંખ્યા,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_dict_સંખ્યા,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_સંખ્યા,
			"nr",
			ret_સંખ્યા,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    ખાલી,
			ખાલી,
			ret_dict_સંખ્યા,    f_wordcount},
    {"writefile",	2, 3, FEARG_1,	    arg23_writefile,
			"object, fname, flags",
			ret_સંખ્યા_bool,    f_writefile},
    {"xor",		2, 2, FEARG_1,	    arg2_સંખ્યા,
			"expr1, expr2",
			ret_સંખ્યા,	    f_xor},
};

/*
 * Return true if specified function allows a type as an argument.
 */
    સ્થિર પૂર્ણાંક
func_allows_type(પૂર્ણાંક idx)
{
    પાછા (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, પૂર્ણાંક idx)
{
    સ્થિર પૂર્ણાંક	intidx = -1;
    char_u	*name;

    જો (idx == 0)
	intidx = -1;
    જો (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	જો (name != ખાલી)
	{
	    જો (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_pattern, 2) == 0)
		પાછા cat_prefix_varname('g', name);
	    પાછા name;
	}
    }
    જો (++intidx < (પૂર્ણાંક)ARRAY_LENGTH(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	જો (global_functions[intidx].f_func == ખાલી)
	    પાછા (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	જો (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	પાછા IObuff;
    }

    પાછા ખાલી;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_પદાવલિ_name(expand_T *xp, પૂર્ણાંક idx)
{
    સ્થિર પૂર્ણાંક	intidx = -1;
    char_u	*name;

    જો (idx == 0)
	intidx = -1;
    જો (intidx < 0)
    {
	name = get_function_name(xp, idx);
	જો (name != ખાલી)
	    પાછા name;
    }
    પાછા get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    સ્થિર પૂર્ણાંક
find_internal_func_opt(char_u *name, પૂર્ણાંક implemented)
{
    પૂર્ણાંક		first = 0;
    પૂર્ણાંક		last;
    પૂર્ણાંક		cmp;
    પૂર્ણાંક		x;

    last = (પૂર્ણાંક)ARRAY_LENGTH(global_functions) - 1;

    // Find the function name in the table. Binary search.
    જ્યારે (first <= last)
    {
	x = first + ((unsigned)(last - first) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	જો (cmp < 0)
	    last = x - 1;
	નહીંતર જો (cmp > 0)
	    first = x + 1;
	નહીંતર જો (implemented && global_functions[x].f_func == ખાલી)
	    તોડો;
	નહીંતર
	    પાછા x;
    }
    પાછા -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    પૂર્ણાંક
find_internal_func(char_u *name)
{
    પાછા find_internal_func_opt(name, TRUE);
}

    પૂર્ણાંક
has_internal_func(char_u *name)
{
    પાછા find_internal_func_opt(name, TRUE) >= 0;
}

    સ્થિર પૂર્ણાંક
has_internal_func_name(char_u *name)
{
    પાછા find_internal_func_opt(name, FALSE) >= 0;
}

    અક્ષર *
internal_func_name(પૂર્ણાંક idx)
{
    પાછા global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    પૂર્ણાંક
internal_func_check_arg_types(
	type2_T	*types,
	પૂર્ણાંક	idx,
	પૂર્ણાંક	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    જો (!(func_allows_type(idx)))
    {
	માટે (પૂર્ણાંક i = 0; i < argcount; ++i)
	    જો (check_type_is_કિંમત(types[i].type_curr) == FAIL)
		પાછા FAIL;
    }

    અચલ argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    જો (argchecks == ખાલી)
	પાછા OK;

    argcontext_T context;

    context.arg_ગણતરી = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    માટે (પૂર્ણાંક i = 0; i < argcount && argchecks[i] != ખાલી; ++i)
    {
	context.arg_idx = i;
	જો (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    શૂન્ય
internal_func_get_argcount(પૂર્ણાંક idx, પૂર્ણાંક *argcount, પૂર્ણાંક *min_argcount)
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
	પૂર્ણાંક	    idx,
	પૂર્ણાંક	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = ખાલી;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    જો (*decl_type == ખાલી)
	*decl_type = ret;
    current_type_gap = ખાલી;
    પાછા ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    પૂર્ણાંક
internal_func_is_map(પૂર્ણાંક idx)
{
    પાછા global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    પૂર્ણાંક
check_internal_func(પૂર્ણાંક idx, પૂર્ણાંક argcount)
{
    funcerror_T	    res;
    અક્ષર	    *name;

    જો (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    નહીંતર જો (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    નહીંતર
	પાછા global_functions[idx].f_argtype & FEARG_MASK;

    name = internal_func_name(idx);
    જો (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    નહીંતર
	semsg(_(e_not_enough_arguments_for_function_str), name);
    પાછા -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    સ્થિર પૂર્ણાંક
check_args_for_type(પૂર્ણાંક idx, પૂર્ણાંક argcount, typval_T *argvars)
{
    જો (!func_allows_type(idx))
    {
	માટે (પૂર્ણાંક i = 0; i < argcount; ++i)
	    જો (check_typval_is_કિંમત(&argvars[i]) == FAIL)
		પાછા FAIL;
    }
    પાછા OK;
}

    funcerror_T
call_internal_func(
	char_u	    *name,
	પૂર્ણાંક	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    પૂર્ણાંક i;

    i = find_internal_func(name);
    જો (i < 0)
	પાછા FCERR_UNKNOWN;
    જો (argcount < global_functions[i].f_min_argc)
	પાછા FCERR_TOOFEW;
    જો (argcount > global_functions[i].f_max_argc)
	પાછા FCERR_TOOMANY;
    જો (check_args_for_type(i, argcount, argvars) == FAIL)
	પાછા FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    જો (in_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    પાછા FCERR_NONE;
}

    શૂન્ય
call_internal_func_by_idx(
	પૂર્ણાંક	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    જો (in_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_internal_method(
	char_u	    *name,
	પૂર્ણાંક	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    પૂર્ણાંક		fi;
    typval_T	argv[MAX_FUNC_ARGS + 1];

    fi = find_internal_func(name);
    જો (fi < 0)
	પાછા FCERR_UNKNOWN;
    જો ((global_functions[fi].f_argtype & FEARG_MASK) == 0)
	પાછા FCERR_NOTMETHOD;
    જો (argcount + 1 < global_functions[fi].f_min_argc)
	પાછા FCERR_TOOFEW;
    જો (argcount + 1 > global_functions[fi].f_max_argc)
	પાછા FCERR_TOOMANY;
    જો (check_args_for_type(fi, argcount, argvars) == FAIL)
	પાછા FCERR_OTHER;

    જો ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_2)
    {
	જો (argcount < 1)
	    પાછા FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	માટે (પૂર્ણાંક i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    નહીંતર જો ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_3)
    {
	જો (argcount < 2)
	    પાછા FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	માટે (પૂર્ણાંક i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    નહીંતર જો ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_4)
    {
	જો (argcount < 3)
	    પાછા FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	માટે (પૂર્ણાંક i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    નહીંતર
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	માટે (પૂર્ણાંક i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    જો (check_args_for_type(fi, argcount + 1, argv) == FAIL)
	પાછા FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    પાછા FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    પૂર્ણાંક
non_zero_arg(typval_T *argvars)
{
    પાછા ((argvars[0].v_type == VAR_સંખ્યા
		&& argvars[0].vval.v_સંખ્યા != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_સંખ્યા == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != ખાલી
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    સ્થિર શૂન્ય
f_and(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    rettv->vval.v_સંખ્યા = tv_get_સંખ્યા_chk(&argvars[0], ખાલી)
					& tv_get_સંખ્યા_chk(&argvars[1], ખાલી);
}

/*
 * "balloon_show()" function
 */
#ifdef FEAT_BEVAL
    સ્થિર શૂન્ય
f_balloon_gettext(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    જો (balloonEval == ખાલી)
	પાછા;

    જો (balloonEval->સંદેશ == ખાલી)
	rettv->vval.v_string = ખાલી;
    નહીંતર
	rettv->vval.v_string = vim_strsave(balloonEval->સંદેશ);
}

    સ્થિર શૂન્ય
f_balloon_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (balloonEval == ખાલી)
	પાછા;

    જો (in_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, ખાલી,
		l == ખાલી || l->lv_len == 0 ? ખાલી : l);
    }
    નહીંતર
    {
	char_u *mesg;

	જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    પાછા;

	mesg = tv_get_string_chk(&argvars[0]);
	જો (mesg != ખાલી)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? ખાલી : mesg, ખાલી);
    }
}

# જો defined(FEAT_BEVAL_TERM)
    સ્થિર શૂન્ય
f_balloon_split(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (rettv_list_alloc(rettv) != OK)
	પાછા;

    char_u *સંદેશ;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;
    સંદેશ = tv_get_string_chk(&argvars[0]);
    જો (સંદેશ != ખાલી)
    {
	pumitem_T	*array;
	પૂર્ણાંક		size = split_message(સંદેશ, &array);

	// Skip the first and last item, they are always empty.
	માટે (પૂર્ણાંક i = 1; i < size - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_text, -1);
	જ્યારે (size > 0)
	    vim_free(array[--size].pum_text);
	vim_free(array);
    }
}
# endif
#endif

/*
 * "base64_decode(string)" function
 */
    સ્થિર શૂન્ય
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    જો (check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    જો (rettv_blob_alloc(rettv) == FAIL)
	પાછા;

    char_u *str = tv_get_string_chk(&argvars[0]);
    જો (str != ખાલી)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    સ્થિર શૂન્ય
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    જો (check_for_blob_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    blob_T *blob = argvars->vval.v_blob;
    જો (blob != ખાલી)
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
    જો (buf == ખાલી)
	semsg(_(e_invalid_buffer_name_str), tv_get_string(arg));
    પાછા buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    સ્થિર શૂન્ય
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_સંખ્યા = VVAL_TRUE;

    જો (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	પાછા;

    જો (strcmp((અચલ અક્ષર *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    નહીંતર
    {
	જો (bindtextdomain((અચલ અક્ષર *)argvars[0].vval.v_string, (અચલ અક્ષર *)argvars[1].vval.v_string) == ખાલી)
	{
	    do_outofmem_સંદેશ((long)0);
	    rettv->vval.v_સંખ્યા = VVAL_FALSE;
	}
    }

    પાછા;
}

/*
 * "byte2line(byte)" function
 */
    સ્થિર શૂન્ય
f_byte2line(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_સંખ્યા = -1;
#નહીંતર
    long	boff = 0;

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    boff = tv_get_સંખ્યા(&argvars[0]) - 1;  // boff gets -1 on type error
    જો (boff < 0)
	rettv->vval.v_સંખ્યા = -1;
    નહીંતર
	rettv->vval.v_સંખ્યા = ml_find_લીટી_or_offset(curbuf,
							  (linenr_T)0, &boff);
#endif
}

/*
 * "call(func, arglist [, dict])" function
 */
    સ્થિર શૂન્ય
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = ખાલી;
    dict_T	*selfdict = ખાલી;
    char_u	*tofree = ખાલી;

    જો (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	પાછા;

    જો (check_for_list_arg(argvars, 1) == FAIL)
	પાછા;
    જો (argvars[1].vval.v_list == ખાલી)
	પાછા;

    જો (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    નહીંતર જો (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    નહીંતર
	func = tv_get_string(&argvars[0]);
    જો (func == ખાલી || *func == NUL)
	પાછા;		// type error, empty name or null function

    જો (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, ખાલી, FALSE, TFN_INT|TFN_QUIET);
	જો (tofree == ખાલી)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    પાછા;
	}
	જો (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_args(tofree, STRLEN(tofree),
									&p);
	    જો (s != ખાલી)
	    {
		vim_free(tofree);
		tofree = s;
	    }
	}
	func = tofree;
    }

    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	જો (check_for_dict_arg(argvars, 2) == FAIL)
	    જાઓ done;

	selfdict = argvars[2].vval.v_dict;
    }

    (શૂન્ય)func_call(func, &argvars[1], partial, selfdict, rettv);

done:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    સ્થિર શૂન્ય
f_changenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    સ્થિર શૂન્ય
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	પાછા;

    જો (has_mbyte)
    {
	પૂર્ણાંક	utf8 = 0;

	જો (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (પૂર્ણાંક)tv_get_bool_chk(&argvars[1], ખાલી);

	જો (utf8)
	    rettv->vval.v_સંખ્યા = utf_ptr2char(tv_get_string(&argvars[0]));
	નહીંતર
	    rettv->vval.v_સંખ્યા = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    નહીંતર
	rettv->vval.v_સંખ્યા = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    સ્થિર શૂન્ય
get_col(typval_T *argvars, typval_T *rettv, પૂર્ણાંક charcol)
{
    colnr_T	col = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    પૂર્ણાંક		winchanged = FALSE;

    જો (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL)
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((પૂર્ણાંક)tv_get_સંખ્યા(&argvars[1]), &tp);
	જો (wp == ખાલી || tp == ખાલી)
	    પાછા;

	જો (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    પાછા;

	check_cursor();
	winchanged = TRUE;
    }

    પૂર્ણાંક fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    જો (fp != ખાલી && fnum == curbuf->b_fnum)
    {
	જો (fp->col == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    જો (fp->lnum <= curbuf->b_ml.ml_લીટી_ગણતરી)
		col = ml_get_len(fp->lnum) + 1;
	    નહીંતર
		col = MAXCOL;
	}
	નહીંતર
	{
	    col = fp->col + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    જો (virtual_active() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		જો (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    પૂર્ણાંક		l;

		    જો (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			col += l;
		}
	    }
	}
    }
    rettv->vval.v_સંખ્યા = col;

    જો (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    સ્થિર શૂન્ય
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, TRUE);
}

    win_T *
get_optional_window(typval_T *argvars, પૂર્ણાંક idx)
{
    win_T   *win = curwin;

    જો (argvars[idx].v_type == VAR_UNKNOWN)
	પાછા curwin;

    win = find_win_by_nr_or_id(&argvars[idx]);
    જો (win == ખાલી)
    {
	emsg(_(e_invalid_window_સંખ્યા));
	પાછા ખાલી;
    }
    પાછા win;
}

/*
 * "col(string)" function
 */
    સ્થિર શૂન્ય
f_col(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    સ્થિર શૂન્ય
f_confirm(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*message;
    char_u	*buttons = ખાલી;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    પૂર્ણાંક		def = 1;
    પૂર્ણાંક		type = VIM_GENERIC;
    char_u	*typestr;
    પૂર્ણાંક		error = FALSE;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	પાછા;

    message = tv_get_string_chk(&argvars[0]);
    જો (message == ખાલી)
	error = TRUE;
    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	જો (buttons == ખાલી)
	    error = TRUE;
	જો (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[2], &error);
	    જો (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		જો (typestr == ખાલી)
		    error = TRUE;
		નહીંતર
		{
		    પસંદગી (TOUPPER_ASC(*typestr))
		    {
			સ્થિતિ 'E': type = VIM_ERROR; તોડો;
			સ્થિતિ 'Q': type = VIM_QUESTION; તોડો;
			સ્થિતિ 'I': type = VIM_INFO; તોડો;
			સ્થિતિ 'W': type = VIM_WARNING; તોડો;
			સ્થિતિ 'G': type = VIM_GENERIC; તોડો;
		    }
		}
	    }
	}
    }

    જો (buttons == ખાલી || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    જો (!error)
	rettv->vval.v_સંખ્યા = do_dialog(type, ખાલી, message, buttons,
							    def, ખાલી, FALSE);
#endif
}

/*
 * "copy()" function
 */
    સ્થિર શૂન્ય
f_copy(typval_T *argvars, typval_T *rettv)
{
    item_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    સ્થિર શૂન્ય
set_cursorpos(typval_T *argvars, typval_T *rettv, પૂર્ણાંક charcol)
{
    long	lnum, col;
    long	coladd = 0;
    પૂર્ણાંક		set_curswant = TRUE;

    જો (in_vim9script()
	    && (check_for_string_or_સંખ્યા_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL)))
	પાછા;

    rettv->vval.v_સંખ્યા = -1;
    જો (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	જો (list2fpos(argvars, &pos, ખાલી, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_argument));
	    પાછા;
	}
	lnum = pos.lnum;
	col = pos.col;
	coladd = pos.coladd;
	જો (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    set_curswant = FALSE;
	}
    }
    નહીંતર જો ((argvars[0].v_type == VAR_સંખ્યા
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_સંખ્યા
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	જો (lnum < 0)
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
	નહીંતર જો (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	col = (long)tv_get_સંખ્યા_chk(&argvars[1], ખાલી);
	જો (charcol)
	    col = buf_charidx_to_byteidx(curbuf, lnum, col) + 1;
	જો (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (long)tv_get_સંખ્યા_chk(&argvars[2], ખાલી);
    }
    નહીંતર
    {
	emsg(_(e_invalid_argument));
	પાછા;
    }
    જો (lnum < 0 || col < 0 || coladd < 0)
	પાછા;		// type error; errmsg already given
    જો (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    જો (col != MAXCOL && --col < 0)
	col = 0;
    curwin->w_cursor.col = col;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    જો (has_mbyte)
	mb_adjust_cursor();

    curwin->w_set_curswant = set_curswant;
    rettv->vval.v_સંખ્યા = 0;
}

/*
 * "cursor(lnum, col)" function, or
 * "cursor(list)"
 *
 * Moves the cursor to the specified line and column.
 * Returns 0 when the position could be set, -1 otherwise.
 */
    સ્થિર શૂન્ય
f_cursor(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, FALSE);
}

#ifdef MSWIN
/*
 * "debugbreak()" function
 */
    સ્થિર શૂન્ય
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		pid;

    rettv->vval.v_સંખ્યા = FAIL;
    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    pid = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]);
    જો (pid == 0)
    {
	emsg(_(e_invalid_argument));
	પાછા;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    જો (hProcess == ખાલી)
	પાછા;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_સંખ્યા = OK;
}
#endif

/*
 * "deepcopy()" function
 */
    સ્થિર શૂન્ય
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    જો (check_for_opt_bool_arg(argvars, 1) == FAIL)
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], ખાલી);

    item_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    સ્થિર શૂન્ય
f_did_filetype(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->vval.v_સંખ્યા = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    સ્થિર શૂન્ય
f_echoraw(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *str;

    જો (check_secure())
	પાછા;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    str = tv_get_string_chk(&argvars[0]);
    જો (str != ખાલી && *str != NUL)
    {
	out_str(str);
	out_flush();
    }
}

/*
 * "empty({expr})" function
 */
    સ્થિર શૂન્ય
f_empty(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		n = FALSE;

    પસંદગી (argvars[0].v_type)
    {
	સ્થિતિ VAR_STRING:
	સ્થિતિ VAR_FUNC:
	    n = argvars[0].vval.v_string == ખાલી
					  || *argvars[0].vval.v_string == NUL;
	    તોડો;
	સ્થિતિ VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == ખાલી;
	    તોડો;
	સ્થિતિ VAR_સંખ્યા:
	    n = argvars[0].vval.v_સંખ્યા == 0;
	    તોડો;
	સ્થિતિ VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    તોડો;
	સ્થિતિ VAR_LIST:
	    n = argvars[0].vval.v_list == ખાલી
					|| argvars[0].vval.v_list->lv_len == 0;
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    તોડો;
	સ્થિતિ VAR_DICT:
	    n = argvars[0].vval.v_dict == ખાલી
			|| argvars[0].vval.v_dict->dv_hashtab.ht_used == 0;
	    તોડો;
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	    n = argvars[0].vval.v_સંખ્યા != VVAL_TRUE;
	    તોડો;
	સ્થિતિ VAR_CLASS:
	    n = argvars[0].vval.v_class != ખાલી;
	    તોડો;
	સ્થિતિ VAR_OBJECT:
	    n = object_empty(argvars[0].vval.v_object);
	    તોડો;

	સ્થિતિ VAR_BLOB:
	    n = argvars[0].vval.v_blob == ખાલી
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    તોડો;

	સ્થિતિ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_job == ખાલી
			   || argvars[0].vval.v_job->jv_status != JOB_STARTED;
	    તોડો;
#endif
	સ્થિતિ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_channel == ખાલી
			       || !channel_is_open(argvars[0].vval.v_channel);
	    તોડો;
#endif
	સ્થિતિ VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == ખાલી
		|| argvars[0].vval.v_typealias->ta_name == ખાલી
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    તોડો;

	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	સ્થિતિ VAR_INSTR:
	    internal_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    તોડો;
    }

    rettv->vval.v_સંખ્યા = n;
}

/*
 * "environ()" function
 */
    સ્થિર શૂન્ય
f_environ(typval_T *argvars UNUSED, typval_T *rettv)
{
#જો !defined(AMIGA)
    પૂર્ણાંક			i = 0;
    char_u		*entry, *કિંમત;
# જો defined (MSWIN)
#  જો !defined(_UCRT)
    બાહ્ય wchar_t	**_wenviron;
#  endif
# નહીંતર
    બાહ્ય અક્ષર		**environ;
# endif

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;

# ifdef MSWIN
    જો (*_wenviron == ખાલી)
	પાછા;
# નહીંતર
    જો (*environ == ખાલી)
	પાછા;
# endif

    માટે (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	જો ((p = (short_u *)_wenviron[i]) == ખાલી)
	    પાછા;
	entry = utf16_to_enc(p, ખાલી);
# નહીંતર
	જો ((entry = (char_u *)environ[i]) == ખાલી)
	    પાછા;
	entry = vim_strsave(entry);
# endif
	જો (entry == ખાલી) // out of memory
	    પાછા;
	જો ((કિંમત = vim_strchr(entry, '=')) == ખાલી)
	{
	    vim_free(entry);
	    ચાલુ;
	}
	*કિંમત++ = NUL;
	dict_add_string(rettv->vval.v_dict, (અક્ષર *)entry, કિંમત);
	vim_free(entry);
    }
#endif
}

/*
 * "err_teapot()" function
 */
    સ્થિર શૂન્ય
f_err_teapot(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	જો (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    જો (*skipwhite(s) == NUL)
		પાછા;
	}

	પૂર્ણાંક err = FALSE;
	પૂર્ણાંક do_503 = eval_પદાવલિ_to_bool(&argvars[0], &err);
	જો (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_available));
	    પાછા;
	}
    }

    emsg(_(e_im_a_teapot));
}

/*
 * "escape({string}, {chars})" function
 */
    સ્થિર શૂન્ય
f_escape(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    rettv->vval.v_string = vim_strsave_escaped(tv_get_string(&argvars[0]),
					 tv_get_string_buf(&argvars[1], buf));
    rettv->v_type = VAR_STRING;
}

/*
 * "eval()" function
 */
    સ્થિર શૂન્ય
f_eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*s, *p;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    s = tv_get_string_chk(&argvars[0]);
    જો (s != ખાલી)
	s = skipwhite(s);

    p = s;
    જો (s == ખાલી || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	જો (p != ખાલી && !aborting())
	    semsg(_(e_invalid_expression_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_સંખ્યા;
	rettv->vval.v_સંખ્યા = 0;
    }
    નહીંતર જો (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    સ્થિર શૂન્ય
f_eventhandler(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = vgetc_busy || આગત_busy;
}

સ્થિર garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    શૂન્ય
execute_redir_str(char_u *કિંમત, પૂર્ણાંક કિંમત_len)
{
    પૂર્ણાંક		len;

    જો (કિંમત_len == -1)
	len = (પૂર્ણાંક)STRLEN(કિંમત);	// Append the entire string
    નહીંતર
	len = કિંમત_len;		// Append only "value_len" characters
    જો (ga_grow(&redir_execute_ga, len) == FAIL)
	પાછા;

    mch_memmove((અક્ષર *)redir_execute_ga.ga_data
	    + redir_execute_ga.ga_len, કિંમત, len);
    redir_execute_ga.ga_len += len;
}

#જો defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    સ્થિર char_u *
get_str_લીટી(
    પૂર્ણાંક	    c UNUSED,
    શૂન્ય    *cookie,
    પૂર્ણાંક	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    char_u	*start = *(char_u **)cookie;
    char_u	*લીટી;
    char_u	*p;

    p = start;
    જો (p == ખાલી || *p == NUL)
	પાછા ખાલી;
    p = vim_strchr(p, '\n');
    જો (p == ખાલી)
	લીટી = vim_strsave(start);
    નહીંતર
    {
	લીટી = vim_strnsave(start, p - start);
	p++;
    }

    *(char_u **)cookie = p;
    પાછા લીટી;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    શૂન્ય
execute_cmds_from_string(char_u *str)
{
    do_cmdline(ખાલી, get_str_લીટી, (શૂન્ય *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#endif

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_લીટી(
    પૂર્ણાંક	    c UNUSED,
    શૂન્ય    *cookie,
    પૂર્ણાંક	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *item = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    જો (item == ખાલી)
	પાછા ખાલી;
    s = tv_get_string_buf_chk(&item->li_tv, buf);
    *p = item->li_next;
    પાછા s == ખાલી ? ખાલી : vim_strsave(s);
}

/*
 * "execute()" function
 */
    શૂન્ય
execute_common(typval_T *argvars, typval_T *rettv, પૂર્ણાંક arg_off)
{
    char_u	*cmd = ખાલી;
    list_T	*list = ખાલી;
    પૂર્ણાંક		save_સંદેશ_silent = સંદેશ_silent;
    પૂર્ણાંક		save_emsg_silent = emsg_silent;
    પૂર્ણાંક		save_emsg_noredir = emsg_noredir;
    પૂર્ણાંક		save_redir_execute = redir_execute;
    પૂર્ણાંક		save_redir_off = redir_off;
    garray_T	save_ga;
    પૂર્ણાંક		save_સંદેશ_col = સંદેશ_col;
    પૂર્ણાંક		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    પૂર્ણાંક		echo_output = FALSE;

    rettv->vval.v_string = ખાલી;
    rettv->v_type = VAR_STRING;

    જો (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	જો (list == ખાલી || list->lv_len == 0)
	    // empty list, no commands, empty output
	    પાછા;
	++list->lv_refcount;
    }
    નહીંતર જો (argvars[arg_off].v_type == VAR_JOB
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_કિંમત_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	પાછા;
    }
    નહીંતર
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	જો (cmd == ખાલી)
	    પાછા;
    }

    જો (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      in_vim9script());

	જો (s == ખાલી)
	    પાછા;
	જો (*s == NUL)
	    echo_output = TRUE;
	જો (STRNCMP(s, "silent", 6) == 0)
	    ++સંદેશ_silent;
	જો (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    નહીંતર
	++સંદેશ_silent;

    જો (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, કદ(અક્ષર), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    જો (!echo_output)
	સંદેશ_col = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    જો (cmd != ખાલી)
	do_cmdline_cmd(cmd);
    નહીંતર
    {
	listitem_T	*item;

	CHECK_LIST_MATERIALIZE(list);
	item = list->lv_first;
	do_cmdline(ખાલી, get_list_લીટી, (શૂન્ય *)&item,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    જો (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((અક્ષર *)redir_execute_ga.ga_data)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_data;
    }
    નહીંતર
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = ખાલી;
    }
    સંદેશ_silent = save_સંદેશ_silent;
    emsg_silent = save_emsg_silent;
    emsg_noredir = save_emsg_noredir;

    redir_execute = save_redir_execute;
    જો (redir_execute)
	redir_execute_ga = save_ga;
    redir_off = save_redir_off;

    // "silent reg" or "silent echo x" leaves msg_col somewhere in the line.
    જો (echo_output)
	// When not working silently: put it in column zero.  A following
	// "echon" will overwrite the message, unavoidably.
	સંદેશ_col = 0;
    નહીંતર
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	સંદેશ_col = save_સંદેશ_col;
}

/*
 * "execute()" function
 */
    સ્થિર શૂન્ય
f_execute(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	પાછા;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    શૂન્ય
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    પૂર્ણાંક		n = FALSE;

    જો (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	પાછા;

    p = tv_get_string(&argvars[0]);
    જો (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	જો (mch_getenv(p + 1) != ખાલી)
	    n = TRUE;
	નહીંતર
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    જો (p != ખાલી && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    નહીંતર જો (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, ખાલી, TRUE) == OK);
	જો (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    નહીંતર જો (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    નહીંતર જો (*p == '?')			// internal function only
    {
	n = has_internal_func_name(p + 1);
    }
    નહીંતર જો (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    નહીંતર જો (*p == '#')
    {
	જો (p[1] == '#')
	    n = autocmd_supported(p + 2);
	નહીંતર
	    n = au_exists(p + 1);
    }
    નહીંતર				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_સંખ્યા = n;
}

    સ્થિર શૂન્ય
f_exists_compiled(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    emsg(_(e_exists_compiled_can_only_be_used_in_def_function));
}

/*
 * "expand()" function
 */
    સ્થિર શૂન્ય
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    size_t	len;
    પૂર્ણાંક		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    પૂર્ણાંક		error = FALSE;
    char_u	*result;
#ifdef BACKSLASH_IN_FILENAME
    char_u	*p_csl_save = p_csl;
#endif

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	પાછા;

#ifdef BACKSLASH_IN_FILENAME
    // avoid using 'completeslash' here
    p_csl = empty_option;
#endif

    rettv->v_type = VAR_STRING;
    જો (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_set(rettv, ખાલી);

    s = tv_get_string(&argvars[0]);
    જો (*s == '%' || *s == '#' || *s == '<')
    {
	અક્ષર	*errormsg = ખાલી;

	જો (p_verbose == 0)
	    ++emsg_off;
	result = eval_vars(s, s, &len, ખાલી, &errormsg, ખાલી, FALSE);
	જો (p_verbose == 0)
	    --emsg_off;
	નહીંતર જો (errormsg != ખાલી)
	    emsg(errormsg);
	જો (rettv->v_type == VAR_LIST)
	{
	    જો (rettv_list_alloc(rettv) == OK && result != ખાલી)
		list_append_string(rettv->vval.v_list, result, -1);
	    vim_free(result);
	}
	નહીંતર
	    rettv->vval.v_string = result;
    }
    નહીંતર
    {
	// When the optional second argument is non-zero, don't remove matches
	// for 'wildignore' and don't put matches for 'suffixes' at the end.
	જો (argvars[1].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	જો (!error)
	{
	    ExpandInit(&xpc);
	    xpc.xp_context = EXPAND_FILES;
	    જો (p_wic)
		options += WILD_ICASE;
	    જો (rettv->v_type == VAR_STRING)
		rettv->vval.v_string = ExpandOne(&xpc, s, ખાલી,
							   options, WILD_ALL);
	    નહીંતર જો (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, ખાલી, options, WILD_ALL_KEEP);
		માટે (પૂર્ણાંક i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	નહીંતર
	    rettv->vval.v_string = ખાલી;
    }
#ifdef BACKSLASH_IN_FILENAME
    p_csl = p_csl_save;
#endif
}

/*
 * "expandcmd()" function
 * Expand all the special characters in a command string.
 */
    સ્થિર શૂન્ય
f_expandcmd(typval_T *argvars, typval_T *rettv)
{
    exarg_T	eap;
    char_u	*cmdstr;
    અક્ષર	*errormsg = ખાલી;
    પૂર્ણાંક		emsgoff = TRUE;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	પાછા;

    જો (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_FIELD(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = ખાલી;
    eap.cmdidx = CMD_USER;

    જો (emsgoff)
	++emsg_off;
    જો (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	જો (!emsgoff && errormsg != ખાલી && *errormsg != NUL)
	    emsg(errormsg);
    જો (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    સ્થિર શૂન્ય
f_feedkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    પૂર્ણાંક		remap = TRUE;
    પૂર્ણાંક		ઉમેરો = FALSE;
    char_u	*ચાવીઓ, *flags;
    char_u	nbuf[NUMBUFLEN];
    પૂર્ણાંક		typed = FALSE;
    પૂર્ણાંક		execute = FALSE;
    પૂર્ણાંક		context = FALSE;
    પૂર્ણાંક		dangerous = FALSE;
    પૂર્ણાંક		lowlevel = FALSE;

    // This is not allowed in the sandbox.  If the commands would still be
    // executed in the sandbox it would be OK, but it probably happens later,
    // when "sandbox" is no longer set.
    જો (check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	પાછા;

    ચાવીઓ = tv_get_string(&argvars[0]);

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	માટે ( ; *flags != NUL; ++flags)
	{
	    પસંદગી (*flags)
	    {
		સ્થિતિ 'n': remap = FALSE; તોડો;
		સ્થિતિ 'm': remap = TRUE; તોડો;
		સ્થિતિ 't': typed = TRUE; તોડો;
		સ્થિતિ 'i': ઉમેરો = TRUE; તોડો;
		સ્થિતિ 'x': execute = TRUE; તોડો;
		સ્થિતિ 'c': context = TRUE; તોડો;
		સ્થિતિ '!': dangerous = TRUE; તોડો;
		સ્થિતિ 'L': lowlevel = TRUE; તોડો;
	    }
	}
    }

    જો (*ચાવીઓ != NUL || execute)
    {
	જો (lowlevel
#ifdef FEAT_VTP
		&& (!is_term_win32()
		    || (ચાવીઓ[0] == 3 && ctrl_c_interrupts && typed))
#endif
	   )
	{
#ifdef USE_આગત_BUF
	    ch_log(ખાલી, "feedkeys() lowlevel: %s", ચાવીઓ);

	    પૂર્ણાંક len = (પૂર્ણાંક)STRLEN(ચાવીઓ);
	    માટે (પૂર્ણાંક idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		જો (ચાવીઓ[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		add_to_આગત_buf(ચાવીઓ + idx, 1);
	    }
#નહીંતર
	    emsg(_(e_lowlevel_આગત_not_supported));
#endif
	}
	નહીંતર
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *ચાવીઓ_esc = vim_strsave_escape_csi(ચાવીઓ);
	    જો (ચાવીઓ_esc == ખાલી)
		પાછા;

	    ch_log(ખાલી, "feedkeys(%s): %s", typed ? "typed" : "", ચાવીઓ);

	    ins_typebuf(ચાવીઓ_esc, (remap ? REMAP_YES : REMAP_NONE),
				   ઉમેરો ? 0 : typebuf.tb_len, !typed, FALSE);
	    જો (vgetc_busy
#ifdef FEAT_TIMERS
		    || timer_busy
#endif
		    || આગત_busy)
		typebuf_was_filled = TRUE;

	    vim_free(ચાવીઓ_esc);
	}

	જો (execute)
	{
	    પૂર્ણાંક		save_સંદેશ_scroll = સંદેશ_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    સંદેશ_scroll = FALSE;

	    ch_log(ખાલી, "feedkeys() executing");

	    જો (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_version = 0;
	    }

	    જો (!dangerous)
	    {
		++ex_normal_busy;
		++in_feedkeys;
	    }
	    ++allow_osc_ચાવી;
	    exec_normal(TRUE, lowlevel, TRUE);
	    --allow_osc_ચાવી;
	    જો (!dangerous)
	    {
		--ex_normal_busy;
		--in_feedkeys;
	    }

	    સંદેશ_scroll |= save_સંદેશ_scroll;

	    જો (context)
		current_sctx = save_sctx;
	}
    }
}

/*
 * "fnameescape({string})" function
 */
    સ્થિર શૂન્ય
f_fnameescape(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    સ્થિર શૂન્ય
f_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    જો (gui.in_use)
    {
	gui_mch_set_foreground();
	પાછા;
    }
#endif
#જો defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_set_foreground();
#endif
}

/*
 * "function()" function
 * "funcref()" function
 */
    સ્થિર શૂન્ય
common_function(typval_T *argvars, typval_T *rettv, પૂર્ણાંક is_funcref)
{
    char_u	*s;
    char_u	*name;
    પૂર્ણાંક		use_string = FALSE;
    partial_T   *arg_pt = ખાલી;
    char_u	*trans_name = ખાલી;
    પૂર્ણાંક		is_global = FALSE;
    char_u	*start_bracket = ખાલી;

    જો (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	પાછા;

    જો (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    નહીંતર જો (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != ખાલી)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    નહીંતર
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    જો (s == ખાલી)
    {
	semsg(_(e_invalid_argument_str), "NULL");
	પાછા;
    }

    જો ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == ખાલી) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, ખાલી);
	જો (*name == '<')
	{
	    // generic function
	    start_bracket = name;
	    જો (skip_generic_func_type_args(&name) == FAIL)
		જાઓ theend;
	}
	જો (*name != NUL)
	    s = ખાલી;
    }

    જો (s == ખાલી || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == ખાલી))
	semsg(_(e_invalid_argument_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    નહીંતર જો (trans_name != ખાલી && (is_funcref
			 ? find_func(trans_name, is_global) == ખાલી
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    નહીંતર
    {
	પૂર્ણાંક	dict_idx = 0;
	પૂર્ણાંક	arg_idx = 0;
	list_T	*list = ખાલી;

	જો (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	નહીંતર જો (trans_name != ખાલી && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	નહીંતર
	    name = vim_strsave(s);

	જો (argvars[1].v_type != VAR_UNKNOWN)
	{
	    જો (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    નહીંતર જો (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    નહીંતર
		// function(name, [args])
		arg_idx = 1;
	    જો (dict_idx > 0)
	    {
		જો (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    જાઓ theend;
		}
		જો (argvars[dict_idx].vval.v_dict == ખાલી)
		    dict_idx = 0;
	    }
	    જો (arg_idx > 0)
	    {
		જો (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_second_argument_of_function_must_be_list_or_dict));
		    vim_free(name);
		    જાઓ theend;
		}
		list = argvars[arg_idx].vval.v_list;
		જો (list == ખાલી || list->lv_len == 0)
		    arg_idx = 0;
		નહીંતર જો (list->lv_len > MAX_FUNC_ARGS)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    જાઓ theend;
		}
	    }
	}
	જો (dict_idx > 0 || arg_idx > 0 || arg_pt != ખાલી || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    જો (pt == ખાલી)
		vim_free(name);
	    નહીંતર
	    {
		જો (arg_idx > 0 || (arg_pt != ખાલી && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    પૂર્ણાંક		i = 0;
		    પૂર્ણાંક		arg_len = 0;
		    પૂર્ણાંક		lv_len = 0;

		    જો (arg_pt != ખાલી)
			arg_len = arg_pt->pt_argc;
		    જો (list != ખાલી)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    જો (pt->pt_argv == ખાલી)
		    {
			vim_free(pt);
			vim_free(name);
			જાઓ theend;
		    }
		    માટે (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    જો (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_વસ્તુઓ(list, li)
			    copy_tv(&li->li_tv, &pt->pt_argv[i++]);
		    }
		}

		// For "function(dict.func, [], dict)" and "func" is a partial
		// use "dict".  That is backwards compatible.
		જો (dict_idx > 0)
		{
		    // The dict is bound explicitly, pt_auto is FALSE.
		    pt->pt_dict = argvars[dict_idx].vval.v_dict;
		    ++pt->pt_dict->dv_refcount;
		}
		નહીંતર જો (arg_pt != ખાલી)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    જો (pt->pt_dict != ખાલી)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    જો (pt->pt_obj != ખાલી)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		જો (arg_pt != ખાલી && arg_pt->pt_func != ખાલી)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		નહીંતર જો (is_funcref)
		{
		    pt->pt_func = find_func(trans_name, is_global);
		    જો (IS_GENERIC_FUNC(pt->pt_func) && start_bracket != ખાલી)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&start_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		નહીંતર
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		જો (arg_pt != ખાલી)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	નહીંતર
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    જો (start_bracket == ખાલી)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    નહીંતર
	    {
		// generic function
		size_t len = STRLEN(name) + STRLEN(start_bracket);
		rettv->vval.v_string = alloc(len + 1);
		જો (rettv->vval.v_string != ખાલી)
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
    સ્થિર શૂન્ય
f_funcref(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, TRUE);
}

/*
 * "function()" function
 */
    સ્થિર શૂન્ય
f_function(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, FALSE);
}

/*
 * "garbagecollect()" function
 */
    સ્થિર શૂન્ય
f_garbagecollect(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	પાછા;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    જો (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
	garbage_collect_at_exit = TRUE;
}

/*
 * "get()" function
 */
    સ્થિર શૂન્ય
f_get(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    list_T	*l;
    tuple_T	*tuple;
    dictitem_T	*di;
    dict_T	*d;
    typval_T	*tv = ખાલી;
    પૂર્ણાંક		what_is_dict = FALSE;

    જો (argvars[0].v_type == VAR_BLOB)
    {
	પૂર્ણાંક error = FALSE;
	પૂર્ણાંક idx = tv_get_સંખ્યા_chk(&argvars[1], &error);

	જો (!error)
	{
	    rettv->v_type = VAR_સંખ્યા;
	    જો (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    જો (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_સંખ્યા = -1;
	    નહીંતર
	    {
		rettv->vval.v_સંખ્યા = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    નહીંતર જો (argvars[0].v_type == VAR_LIST)
    {
	જો ((l = argvars[0].vval.v_list) != ખાલી)
	{
	    પૂર્ણાંક		error = FALSE;

	    li = list_find(l, (long)tv_get_સંખ્યા_chk(&argvars[1], &error));
	    જો (!error && li != ખાલી)
		tv = &li->li_tv;
	}
    }
    નહીંતર જો (argvars[0].v_type == VAR_TUPLE)
    {
	જો ((tuple = argvars[0].vval.v_tuple) != ખાલી)
	{
	    પૂર્ણાંક		error = FALSE;
	    long	idx;

	    idx = (long)tv_get_સંખ્યા_chk(&argvars[1], &error);
	    જો (!error)
		tv = tuple_find(tuple, idx);
	}
    }
    નહીંતર જો (argvars[0].v_type == VAR_DICT)
    {
	જો ((d = argvars[0].vval.v_dict) != ખાલી)
	{
	    di = dict_find(d, tv_get_string(&argvars[1]), -1);
	    જો (di != ખાલી)
		tv = &di->di_tv;
	}
    }
    નહીંતર જો (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	જો (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	નહીંતર
	{
	    CLEAR_FIELD(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	જો (pt != ખાલી)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    જો (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		જો (name == ખાલી)
		    rettv->vval.v_string = ખાલી;
		નહીંતર
		{
		    જો (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    જો (*what == 'n' && pt->pt_name == ખાલી
							&& pt->pt_func != ખાલી)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    નહીંતર જો (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		જો (pt->pt_dict != ખાલી)
		    rettv_dict_set(rettv, pt->pt_dict);
	    }
	    નહીંતર જો (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		જો (rettv_list_alloc(rettv) == OK)
		{
		    પૂર્ણાંક i;

		    માટે (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    નહીંતર જો (STRCMP(what, "arity") == 0)
	    {
		પૂર્ણાંક required = 0, optional = 0, varargs = FALSE;
		char_u *name = partial_name(pt);

		get_func_arity(name, &required, &optional, &varargs);

		rettv->v_type = VAR_DICT;
		જો (rettv_dict_alloc(rettv) == OK)
		{
		    dict_T *dict = rettv->vval.v_dict;

		    // Take into account the arguments of the partial, if any.
		    // Note that it is possible to supply more arguments than the function
		    // accepts.
		    જો (pt->pt_argc >= required + optional)
			required = optional = 0;
		    નહીંતર જો (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    નહીંતર
			required -= pt->pt_argc;

		    dict_add_સંખ્યા(dict, "required", required);
		    dict_add_સંખ્યા(dict, "optional", optional);
		    dict_add_bool(dict, "varargs", varargs);
		}
	    }
	    નહીંતર
		semsg(_(e_invalid_argument_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    જો (!what_is_dict)
		પાછા;
	}
    }
    નહીંતર
	semsg(_(e_argument_of_str_must_be_list_tuple_dictionary_or_blob), "get()");

    જો (tv == ખાલી)
    {
	જો (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    નહીંતર
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
    સ્થિર શૂન્ય
f_getbgcolor(typval_T *argvars UNUSED, typval_T *rettv)
{
    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	જો (gui.in_use)
	    try_hl = true;
# endif
# ifdef FEAT_TERMGUICOLORS
	જો (p_tgc)
	    try_hl = true;
# endif
	જો (try_hl)
	{
	    guicolor_T	fg, bg;
	    પૂર્ણાંક		hl_id = syn_name2id((char_u *)"Normal");

	    જો (hl_id > 0)
	    {
		(શૂન્ય)syn_id2colors(hl_id, &fg, &bg);
		જો (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_સંખ્યા(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_સંખ્યા(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_સંખ્યા(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    પાછા;
		}
	    }
	}
    }
#endif
#ifdef FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	જો (term_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)r);
	    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)g);
	    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#endif
}

/*
 * "getcellpixels()" function
 */
    સ્થિર શૂન્ય
f_getcellpixels(typval_T *argvars UNUSED, typval_T *rettv)
{
    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

#જો defined(FEAT_GUI)
    જો (gui.in_use)
    {
	// success pixel size and no gui.
	list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)gui.char_width);
	list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)gui.char_height);
    }
    નહીંતર
#endif
    {
#જો defined(UNIX) || defined(MSWIN)
	list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)cell_width);
	list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)cell_height);
#નહીંતર
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)-1);
	list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)-1);
#endif
    }

}

/*
 * "getchangelist()" function
 */
    સ્થિર શૂન્ય
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    પૂર્ણાંક		i;
    list_T	*l;
    dict_T	*d;
    પૂર્ણાંક		changelistindex;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    નહીંતર
	buf = tv_get_buf_from_arg(&argvars[0]);
    જો (buf == ખાલી)
	પાછા;

    l = list_alloc();
    જો (l == ખાલી)
	પાછા;
    જો (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	પાછા;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    જો (buf == curwin->w_buffer)
    {
	changelistindex = curwin->w_changelistidx;
    }
    નહીંતર
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    જો (wip->wi_win == curwin)
		તોડો;
	changelistindex = wip != ખાલી ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)changelistindex);

    માટે (i = 0; i < buf->b_changelistlen; ++i)
    {
	જો (buf->b_changelist[i].lnum == 0)
	    ચાલુ;
	જો ((d = dict_alloc()) == ખાલી)
	    પાછા;
	જો (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    પાછા;
	}
	dict_add_સંખ્યા(d, "lnum", (long)buf->b_changelist[i].lnum);
	dict_add_સંખ્યા(d, "col", (long)buf->b_changelist[i].col);
	dict_add_સંખ્યા(d, "coladd", (long)buf->b_changelist[i].coladd);
    }
}

    સ્થિર શૂન્ય
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    પૂર્ણાંક		getcurpos,
    પૂર્ણાંક		charcol)
{
    pos_T	*fp = ખાલી;
    pos_T	pos;
    win_T	*wp = curwin;
    list_T	*l;
    પૂર્ણાંક		fnum = -1;

    જો (rettv_list_alloc(rettv) == OK)
    {
	l = rettv->vval.v_list;
	જો (getcurpos)
	{
	    જો (argvars[0].v_type != VAR_UNKNOWN)
	    {
		wp = find_win_by_nr_or_id(&argvars[0]);
		જો (wp != ખાલી)
		    fp = &wp->w_cursor;
	    }
	    નહીંતર
		fp = &curwin->w_cursor;
	    જો (fp != ખાલી && charcol)
	    {
		pos = *fp;
		pos.col =
		    buf_byteidx_to_charidx(wp->w_buffer, pos.lnum, pos.col);
		fp = &pos;
	    }
	}
	નહીંતર
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	જો (fnum != -1)
	    list_append_સંખ્યા(l, (varnumber_T)fnum);
	નહીંતર
	    list_append_સંખ્યા(l, (varnumber_T)0);
	list_append_સંખ્યા(l, (fp != ખાલી) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_સંખ્યા(l, (fp != ખાલી)
		     ? (varnumber_T)(fp->col == MAXCOL ? MAXCOL : fp->col + 1)
							    : (varnumber_T)0);
	list_append_સંખ્યા(l, (fp != ખાલી) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	જો (getcurpos)
	{
	    પૂર્ણાંક	    save_set_curswant = curwin->w_set_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    જો (wp == curwin)
		update_curswant();
	    list_append_સંખ્યા(l, wp == ખાલી ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    જો (wp == curwin && save_set_curswant)
	    {
		curwin->w_set_curswant = save_set_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    નહીંતર
	rettv->vval.v_સંખ્યા = FALSE;
}

/*
 * "getcharpos()" function
 */
    સ્થિર શૂન્ય
f_getcharpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    સ્થિર શૂન્ય
f_getcharsearch(typval_T *argvars UNUSED, typval_T *rettv)
{
    જો (rettv_dict_alloc(rettv) != OK)
	પાછા;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_string(dict, "char", last_csearch());
    dict_add_સંખ્યા(dict, "forward", last_csearch_forward());
    dict_add_સંખ્યા(dict, "until", last_csearch_until());
}

/*
 * "getenv()" function
 */
    સ્થિર શૂન્ય
f_getenv(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક	    mustfree = FALSE;
    char_u  *p;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    જો (p == ખાલી)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_સંખ્યા = VVAL_NULL;
	પાછા;
    }
    જો (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    સ્થિર શૂન્ય
f_getfontname(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	પાછા;

#ifdef FEAT_GUI
    જો (gui.in_use)
    {
	GuiFont font;
	char_u	*name = ખાલી;

	જો (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	નહીંતર
	{
	    name = tv_get_string(&argvars[0]);
	    જો (STRCMP(name, "*") == 0)	    // don't use font dialog
		પાછા;
	    font = gui_mch_get_font(name, FALSE);
	    જો (font == NOFONT)
		પાછા;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	જો (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#endif
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
સ્થિર સંરચના
{
    argcheck_T	func;
    અક્ષર	*types;	    // separated by commas
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
    {arg_list_સંખ્યા,		"list<number>"},
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
    {arg_સંખ્યા,		"number"},
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
    સ્થિર list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    અક્ષર	*types = "any";

    જો (l == ખાલી)
	પાછા ખાલી;
    માટે (પૂર્ણાંક i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(argcheck_types); ++i)
	જો (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    તોડો;
	}
    માટે (અક્ષર *p = types; ; )
    {
	અક્ષર	*end = (અક્ષર *)vim_strchr((char_u *)p, ',');
	પૂર્ણાંક	len = end == ખાલી ? (પૂર્ણાંક)STRLEN(p) : (પૂર્ણાંક)(end - p);

	જો (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    પાછા ખાલી;
	}
	જો (end == ખાલી)
	    તોડો;
	p = end + 1;
    }
    પાછા l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    સ્થિર પૂર્ણાંક
getinfo_argtypes(
	list_T			*l,
	અચલ funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    પૂર્ણાંક		argcount = 0;

    જો (l != ખાલી)
	FOR_ALL_LIST_વસ્તુઓ(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    જો (s == ખાલી)
		પાછા -1;
	    જો (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_ARGS)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		પાછા -1;
	    }
	    type = parse_type(&p, type_gap, ખાલી, ખાલી, TRUE);
	    જો (type == ખાલી)
		પાછા -1;
	    જો (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		પાછા -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    માટે ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_ARGS; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    પાછા argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    સ્થિર અક્ષર *
builtin_ret_type_name(
	પૂર્ણાંક	    idx,
	પૂર્ણાંક	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	અક્ષર	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = internal_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    પાછા type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
પ્રકારવ્યાખ્યા ગણના {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_kind_T;

સ્થિર સંરચના
{
    અક્ષર		*name;
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
    સ્થિર શૂન્ય
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_kind_T	kind;
    પૂર્ણાંક			ki;
    પૂર્ણાંક			idx;
    અચલ funcentry_T	*fe;
    dict_T		*d;
    list_T		*args;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_ARGS];
    list_T		*argtypes_list = ખાલી;
    પૂર્ણાંક			argcount;
    પૂર્ણાંક			vim9 = -1;	// -1: the context of the caller
    અક્ષર		*ret_name;
    અક્ષર		*tofree;

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;
    જો (in_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;
    જો (check_for_opt_dict_arg(argvars, 2) == FAIL)
	પાછા;

    kindname = tv_get_string(&argvars[0]);
    માટે (ki = 0; ki < (પૂર્ણાંક)ARRAY_LENGTH(getinfo_kinds); ++ki)
	જો (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    તોડો;
    જો (ki == (પૂર્ણાંક)ARRAY_LENGTH(getinfo_kinds))
    {
	semsg(_(e_invalid_argument_str), kindname);
	પાછા;
    }
    kind = getinfo_kinds[ki].kind;
    name = tv_get_string_buf(&argvars[1], namebuf);

    જો (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != ખાલી)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	પૂર્ણાંક	    todo = (પૂર્ણાંક)opts->dv_hashtab.ht_used;

	FOR_ALL_HASHTAB_વસ્તુઓ(&opts->dv_hashtab, hi, todo)
	{
	    જો (HASHITEM_EMPTY(hi))
		ચાલુ;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    જો (STRCMP(di->di_ચાવી, "argtypes") == 0 && kind == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    નહીંતર જો (STRCMP(di->di_ચાવી, "vim9") == 0 && kind == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    નહીંતર
	    {
		semsg(_(e_invalid_argument_str), di->di_ચાવી);
		પાછા;
	    }
	}
    }
    જો (kind == GI_VIMVAR)
    {
	જો (STRNCMP(name, "v:", 2) == 0)
	    (શૂન્ય)vim_var_info(name + 2, rettv->vval.v_dict);
	પાછા;
    }
    જો (kind == GI_COMMAND)
    {
	(શૂન્ય)ex_command_info(name, vim9, rettv->vval.v_dict);
	પાછા;
    }
    જો (kind == GI_OPTION)
    {
	(શૂન્ય)option_info(name, rettv->vval.v_dict);
	પાછા;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = find_internal_func_opt(name, FALSE);
    જો (idx < 0)
    {
	(શૂન્ય)user_func_info(name, rettv->vval.v_dict);
	પાછા;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, કદ(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    જો (argcount < 0)
    {
	clear_type_list(&type_gap);
	પાછા;
    }
    d = rettv->vval.v_dict;
    dict_add_string(d, "name", (char_u *)fe->f_name);
    dict_add_string(d, "kind", (char_u *)"builtin");
    dict_add_bool(d, "available", fe->f_func != ખાલી);
    dict_add_સંખ્યા(d, "minargs", fe->f_min_argc);
    dict_add_સંખ્યા(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_add_સંખ્યા(d, "method", fe->f_argtype & FEARG_MASK);

    args = list_alloc();
    જો (args == ખાલી || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	clear_type_list(&type_gap);
	પાછા;
    }
    argnames = (char_u *)fe->f_argnames;
    માટે (પૂર્ણાંક i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = ખાલી;
	પૂર્ણાંક	    argnamelen = 0;

	જો (fe->f_argcheck == ખાલી)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    જો (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		તોડો;
	    check = arg_any;
	}
	નહીંતર
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    જો ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == ખાલી)
		તોડો;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	જો (argnames != ખાલી)
	{
	    char_u  *end = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = end == ખાલી ? (પૂર્ણાંક)STRLEN(argnames)
						     : (પૂર્ણાંક)(end - argnames);
	    argnames = end == ખાલી ? ખાલી : skipwhite(end + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	જો (arg == ખાલી || types == ખાલી
		|| dict_add_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    પાછા;
	}
	જો ((argnamelen > 0
		    && dict_add_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    પાછા;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    જો (argtypes_list == ખાલી
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	અક્ષર	*tofree_max;
	અક્ષર	*max_name;

	માટે (પૂર્ણાંક i = argcount; i < fe->f_max_argc && i < MAX_FUNC_ARGS; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	જો (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = ખાલી;
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
    સ્થિર શૂન્ય
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    પૂર્ણાંક		i;
    list_T	*l;
    dict_T	*d;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script()
	    && (check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL)))
	પાછા;

    wp = find_tabwin(&argvars[0], &argvars[1], ખાલી);
    જો (wp == ખાલી)
	પાછા;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    જો (l == ખાલી)
	પાછા;
    જો (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	પાછા;
    }

    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    માટે (i = 0; i < wp->w_jumplistlen; ++i)
    {
	જો (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    ચાલુ;
	જો ((d = dict_alloc()) == ખાલી)
	    પાછા;
	જો (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    પાછા;
	}
	dict_add_સંખ્યા(d, "lnum", (long)wp->w_jumplist[i].fmark.mark.lnum);
	dict_add_સંખ્યા(d, "col", (long)wp->w_jumplist[i].fmark.mark.col);
	dict_add_સંખ્યા(d, "coladd", (long)wp->w_jumplist[i].fmark.mark.coladd);
	dict_add_સંખ્યા(d, "bufnr", (long)wp->w_jumplist[i].fmark.fnum);
	જો (wp->w_jumplist[i].fname != ખાલી)
	    dict_add_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    સ્થિર શૂન્ય
f_getpid(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    સ્થિર શૂન્ય
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    સ્થિર શૂન્ય
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    સ્થિર શૂન્ય
f_getpos(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    સ્થિર પૂર્ણાંક
block_def2str(સંરચના block_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    જો (ret->string == ખાલી)
    {
	ret->length = 0;
	પાછા FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->length = bd->startspaces;
    mch_memmove(ret->string + ret->length, bd->textstart, bd->textlen);
    ret->length += bd->textlen;
    vim_memset(ret->string + ret->length, ' ', bd->endspaces);
    ret->length += bd->endspaces;
    ret->string[ret->length] = NUL;

    પાછા OK;
}

    સ્થિર પૂર્ણાંક
getregionpos(
    typval_T	*argvars,
    typval_T	*rettv,
    pos_T	*p1,
    pos_T	*p2,
    પૂર્ણાંક		*inclusive,
    પૂર્ણાંક		*region_type,
    oparg_T	*oap)
{
    પૂર્ણાંક		fnum1 = -1, fnum2 = -1;
    char_u	*type;
    buf_T	*findbuf;
    char_u	default_type[] = "v";
    પૂર્ણાંક		block_width = 0;
    પૂર્ણાંક		is_select_exclusive;
    પૂર્ણાંક		l;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા FAIL;

    જો (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	પાછા FAIL;

    જો (list2fpos(&argvars[0], p1, &fnum1, ખાલી, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, ખાલી, FALSE) != OK
	    || fnum1 != fnum2)
	પાછા FAIL;

    જો (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	જો (type == ખાલી)
	    type = default_type;
    }
    નહીંતર
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    જો (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    નહીંતર જો (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    નહીંતર જો (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	જો (*p != NUL && ((block_width = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_કિંમત_for_argument_str_str), "type", type);
	    પાછા FAIL;
	}
	*region_type = MBLOCK;
    }
    નહીંતર
    {
	semsg(_(e_invalid_કિંમત_for_argument_str_str), "type", type);
	પાછા FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    જો (findbuf == ખાલી || findbuf->b_ml.ml_mfp == ખાલી)
    {
	emsg(_(e_buffer_is_not_loaded));
	પાછા FAIL;
    }

    જો (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_લીટી_ગણતરી)
    {
	semsg(_(e_invalid_લીટી_સંખ્યા_nr), p1->lnum);
	પાછા FAIL;
    }
    જો (p1->col == MAXCOL)
	p1->col = ml_get_buf_len(findbuf, p1->lnum) + 1;
    નહીંતર જો (p1->col < 1 || p1->col > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_column_સંખ્યા_nr), p1->col);
	પાછા FAIL;
    }

    જો (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_લીટી_ગણતરી)
    {
	semsg(_(e_invalid_લીટી_સંખ્યા_nr), p2->lnum);
	પાછા FAIL;
    }
    જો (p2->col == MAXCOL)
	p2->col = ml_get_buf_len(findbuf, p2->lnum) + 1;
    નહીંતર જો (p2->col < 1 || p2->col > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_column_સંખ્યા_nr), p2->col);
	પાછા FAIL;
    }

    curbuf = findbuf;
    curwin->w_buffer = curbuf;
    virtual_op = virtual_active();

    // NOTE: Adjustment is needed.
    p1->col--;
    p2->col--;

    જો (!LT_POS(*p1, *p2))
    {
	// swap position
	pos_T p;

	p = *p1;
	*p1 = *p2;
	*p2 = p;
    }

    જો (*region_type == MCHAR)
    {
	// Handle 'selection' == "exclusive".
	જો (is_select_exclusive && !EQUAL_POS(*p1, *p2))
	    // When backing up to previous line, inclusive becomes false.
	    *inclusive = !unadjust_for_sel_inner(p2);
	// If p2 is on NUL (end of line), inclusive becomes false.
	જો (*inclusive && !virtual_op && *ml_get_pos(p2) == NUL)
	    *inclusive = FALSE;
    }
    નહીંતર જો (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#ifdef FEAT_LINEBREAK
	પૂર્ણાંક	lbr_saved = reset_lbr();
#endif

	getvvcol(curwin, p1, &sc1, ખાલી, &ec1, 0);
	getvvcol(curwin, p2, &sc2, ખાલી, &ec2, 0);

#ifdef FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#endif

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->start = *p1;
	oap->end = *p2;
	oap->start_vcol = MIN(sc1, sc2);
	જો (block_width > 0)
	    oap->end_vcol = oap->start_vcol + block_width - 1;
	નહીંતર જો (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->end_vcol = sc2 - 1;
	નહીંતર
	    oap->end_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    જો (l > 1)
	p2->col += l - 1;

    પાછા OK;
}

/*
 * "getregion()" function
 */
    સ્થિર શૂન્ય
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    પૂર્ણાંક			inclusive = TRUE;
    પૂર્ણાંક			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    પૂર્ણાંક			save_virtual;
    string_T		akt = {ખાલી, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    જો (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	પાછા;

    માટે (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	પૂર્ણાંક ret = OK;

	જો (region_type == MBLOCK)
	{
	    સંરચના block_def	bd;

	    block_prep(&oa, &bd, lnum, FALSE);
	    block_def2str(&bd, &akt);
	}
	નહીંતર જો (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.length = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.length);
	    જો (akt.string == ખાલી)
		akt.length = 0;
	    નહીંતર
		akt.length = s.length;
	}
	નહીંતર
	{
	    સંરચના block_def	bd;

	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);
	    block_def2str(&bd, &akt);
	}

	જો (akt.string == ખાલી)
	{
	    clear_tv(rettv);
	    (શૂન્ય)rettv_list_alloc(rettv);
	    તોડો;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (પૂર્ણાંક)akt.length);
	vim_free(akt.string);

	જો (ret == FAIL)
	{
	    clear_tv(rettv);
	    (શૂન્ય)rettv_list_alloc(rettv);
	    તોડો;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_buffer = curbuf;
    virtual_op = save_virtual;
}

    સ્થિર શૂન્ય
add_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    જો (l1 == ખાલી)
	પાછા;

    જો (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	પાછા;
    }

    l2 = list_alloc();
    જો (l2 == ખાલી)
	પાછા;

    જો (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	પાછા;
    }

    l3 = list_alloc();
    જો (l3 == ખાલી)
	પાછા;

    જો (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	પાછા;
    }

    list_append_સંખ્યા(l2, curbuf->b_fnum);
    list_append_સંખ્યા(l2, p1.lnum);
    list_append_સંખ્યા(l2, p1.col);
    list_append_સંખ્યા(l2, p1.coladd);

    list_append_સંખ્યા(l3, curbuf->b_fnum);
    list_append_સંખ્યા(l3, p2.lnum);
    list_append_સંખ્યા(l3, p2.col);
    list_append_સંખ્યા(l3, p2.coladd);
}

/*
 * Compute the positions of the region segment on line "lnum".
 * "ret_p1" is set to the start position of the segment and "ret_p2" to its
 * end position.
 */
    સ્થિર શૂન્ય
getregionpos_લીટી(
    linenr_T	lnum,
    pos_T	p1,
    pos_T	p2,
    પૂર્ણાંક		inclusive,
    પૂર્ણાંક		region_type,
    oparg_T	*oap,
    પૂર્ણાંક		allow_eol,
    pos_T	*ret_p1,
    pos_T	*ret_p2)
{
    char_u	*લીટી = ml_get(lnum);
    colnr_T	લીટી_len = ml_get_len(lnum);

    જો (region_type == MLINE)
    {
	ret_p1->col = 1;
	ret_p1->coladd = 0;
	ret_p2->col = MAXCOL;
	ret_p2->coladd = 0;
    }
    નહીંતર
    {
	સંરચના block_def	bd;

	જો (region_type == MBLOCK)
	    block_prep(oap, &bd, lnum, FALSE);
	નહીંતર
	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);

	જો (bd.is_oneChar)  // selection entirely inside one char
	{
	    જો (region_type == MBLOCK)
	    {
		ret_p1->col = mb_prevptr(લીટી, bd.textstart) - લીટી + 1;
		ret_p1->coladd = bd.start_char_vcols
					   - (bd.start_vcol - oap->start_vcol);
	    }
	    નહીંતર
	    {
		ret_p1->col = p1.col + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	નહીંતર જો (region_type == MBLOCK && oap->start_vcol > bd.start_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->col = MAXCOL;
	    ret_p1->coladd = oap->start_vcol - bd.start_vcol;
	    bd.is_oneChar = TRUE;
	}
	નહીંતર જો (bd.startspaces > 0)
	{
	    ret_p1->col = mb_prevptr(લીટી, bd.textstart) - લીટી + 1;
	    ret_p1->coladd = bd.start_char_vcols - bd.startspaces;
	}
	નહીંતર
	{
	    ret_p1->col = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	જો (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->col = ret_p1->col;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	નહીંતર જો (bd.endspaces > 0)
	{
	    ret_p2->col = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	નહીંતર
	{
	    ret_p2->col = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    જો (!allow_eol && ret_p1->col > લીટી_len)
    {
	ret_p1->col = 0;
	ret_p1->coladd = 0;
    }
    નહીંતર જો (ret_p1->col > લીટી_len + 1)
	ret_p1->col = લીટી_len + 1;

    જો (!allow_eol && ret_p2->col > લીટી_len)
    {
	ret_p2->col = ret_p1->col == 0 ? 0 : લીટી_len;
	ret_p2->coladd = 0;
    }
    નહીંતર જો (ret_p2->col > લીટી_len + 1)
	ret_p2->col = લીટી_len + 1;

    ret_p1->lnum = lnum;
    ret_p2->lnum = lnum;
}

/*
 * "getregionpos()" function
 */
    સ્થિર શૂન્ય
f_getregionpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	p1, p2;
    પૂર્ણાંક		inclusive = TRUE;
    પૂર્ણાંક		region_type = -1;
    પૂર્ણાંક		allow_eol = FALSE;
    પૂર્ણાંક		bounds_only = FALSE;
    oparg_T	oa;
    પૂર્ણાંક		lnum;

    buf_T	*save_curbuf;
    પૂર્ણાંક		save_virtual;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    જો (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	પાછા;

    જો (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    જો (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	start_pos, end_pos;

	getregionpos_લીટી(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &start_pos, &end_pos);
	જો (p2.lnum != p1.lnum)
	{
	    pos_T	unused;

	    getregionpos_લીટી(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &unused, &end_pos);
	}
	add_regionpos_range(rettv, start_pos, end_pos);
    }
    નહીંતર
    {
	માટે (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_લીટી(lnum, p1, p2, inclusive, region_type, &oa,
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
    સ્થિર પૂર્ણાંક
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	જો (strregname != ખાલી && in_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_register_name_must_be_one_char_str), strregname);
	    strregname = ખાલી;
	}
	જો (strregname == ખાલી)	    // type error; errmsg already given
	    પાછા 0;
    }
    નહીંતર
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    પાછા *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    સ્થિર શૂન્ય
f_getreg(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		regname;
    પૂર્ણાંક		arg2 = FALSE;
    પૂર્ણાંક		return_list = FALSE;

    જો (in_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	પાછા;

    regname = getreg_get_regname(argvars);
    જો (regname == 0)
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	પૂર્ણાંક		error = FALSE;

	arg2 = (પૂર્ણાંક)tv_get_bool_chk(&argvars[1], &error);

	જો (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (પૂર્ણાંક)tv_get_bool_chk(&argvars[2], &error);
	જો (error)
	    પાછા;
    }

    જો (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_પદાવલિ_SRC : 0) | GREG_LIST);
	જો (rettv->vval.v_list == ખાલી)
	    (શૂન્ય)rettv_list_alloc(rettv);
	નહીંતર
	    ++rettv->vval.v_list->lv_refcount;
    }
    નહીંતર
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = get_reg_contents(regname,
						    arg2 ? GREG_પદાવલિ_SRC : 0);
    }
}

/*
 * "getregtype()" function
 */
    સ્થિર શૂન્ય
f_getregtype(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		regname;
    char_u	buf[NUMBUFLEN + 2];
    long	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	પાછા;

    regname = getreg_get_regname(argvars);
    જો (regname == 0)
	પાછા;

    buf[0] = NUL;
    buf[1] = NUL;
    પસંદગી (get_reg_type(regname, &reglen))
    {
	સ્થિતિ MLINE: buf[0] = 'V'; તોડો;
	સ્થિતિ MCHAR: buf[0] = 'v'; તોડો;
	સ્થિતિ MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((અક્ષર *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		તોડો;
    }
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "gettagstack()" function
 */
    સ્થિર શૂન્ય
f_gettagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;			// default is current window

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	જો (wp == ખાલી)
	    પાછા;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    સ્થિર શૂન્ય
f_gettext(typval_T *argvars, typval_T *rettv)
{
#જો defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    અક્ષર *prev = ખાલી;
#endif

    જો (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	પાછા;

    rettv->v_type = VAR_STRING;

    જો (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != ખાલી &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#જો defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((અચલ અક્ષર *)argvars[1].vval.v_string, (અક્ષર *)p_enc);
#endif

#જો defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((અચલ અક્ષર *)argvars[1].vval.v_string, (અચલ અક્ષર *)argvars[0].vval.v_string));
#નહીંતર
	textdomain((અચલ અક્ષર *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#endif

#જો defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	જો (prev != ખાલી)
	    bind_textdomain_codeset((અચલ અક્ષર *)argvars[1].vval.v_string, prev);
#endif
    }
    નહીંતર
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
}

// for VIM_VERSION_ defines
#અન્તર્ભાવ "version.h"

/*
 * "has()" function
 */
    શૂન્ય
f_has(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		i;
    char_u	*name;
    પૂર્ણાંક		x = FALSE;
    પૂર્ણાંક		n = FALSE;
    પ્રકારવ્યાખ્યા સંરચના {
	અક્ષર *name;
	short present;
    } has_item_T;
    સ્થિર has_item_T has_list[] =
    {
	{"amiga",
#ifdef AMIGA
		1
#નહીંતર
		0
#endif
		},
	{"android",
#ifdef __ANDROID__
		1
#નહીંતર
		0
#endif
		},
	{"arp",
#જો defined(AMIGA) && defined(FEAT_ARP)
		1
#નહીંતર
		0
#endif
		},
	{"haiku",
#ifdef __HAIKU__
		1
#નહીંતર
		0
#endif
		},
	{"bsd",
#જો defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#નહીંતર
		0
#endif
		},
	{"hpux",
#ifdef hpux
		1
#નહીંતર
		0
#endif
		},
	{"hurd",
#ifdef __GNU__
		1
#નહીંતર
		0
#endif
		},
	{"linux",
#ifdef __linux__
		1
#નહીંતર
		0
#endif
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#ifdef MACOS_X
		1
#નહીંતર
		0
#endif
		},
	{"osx",		// Mac OS X
#ifdef MACOS_X
		1
#નહીંતર
		0
#endif
		},
	{"macunix",	// Mac OS X, with the darwin feature
#જો defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#નહીંતર
		0
#endif
		},
	{"osxdarwin",	// synonym for macunix
#જો defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#નહીંતર
		0
#endif
		},
	{"qnx",
#ifdef __QNX__
		1
#નહીંતર
		0
#endif
		},
	{"sun",
#ifdef SUN_SYSTEM
		1
#નહીંતર
		0
#endif
		},
	{"termux",
#ifdef __TERMUX__
		1
#નહીંતર
		0
#endif
		},
	{"unix",
#ifdef UNIX
		1
#નહીંતર
		0
#endif
		},
	{"vms",
#ifdef VMS
		1
#નહીંતર
		0
#endif
		},
	{"win32",
#ifdef MSWIN
		1
#નહીંતર
		0
#endif
		},
	{"win32unix",
#ifdef WIN32UNIX
		1
#નહીંતર
		0
#endif
		},
	{"win64",
#ifdef _WIN64
		1
#નહીંતર
		0
#endif
		},
	{"ebcdic", 0 },
	{"fname_case",
#ifndef CASE_INSENSITIVE_FILENAME
		1
#નહીંતર
		0
#endif
		},
	{"acl",
#ifdef HAVE_ACL
		1
#નહીંતર
		0
#endif
		},
	{"arabic",
#ifdef FEAT_ARABIC
		1
#નહીંતર
		0
#endif
		},
	{"autocmd", 1},
	{"autochdir",
#ifdef FEAT_AUTOCHDIR
		1
#નહીંતર
		0
#endif
		},
	{"autoservername",
#ifdef FEAT_AUTOSERVERNAME
		1
#નહીંતર
		0
#endif
		},
	{"socketserver",
#ifdef FEAT_SOCKETSERVER
		1
#નહીંતર
		0
#endif
		},
	{"balloon_eval",
#ifdef FEAT_BEVAL_GUI
		1
#નહીંતર
		0
#endif
		},
	{"balloon_multiline",
#ifdef FEAT_BEVAL_GUI
		1
#નહીંતર
		0
#endif
		},
	{"balloon_eval_term",
#ifdef FEAT_BEVAL_TERM
		1
#નહીંતર
		0
#endif
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#જો defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#નહીંતર
		0
#endif
		},
	{"byte_offset",
#ifdef FEAT_BYTEOFF
		1
#નહીંતર
		0
#endif
		},
	{"channel",
#ifdef FEAT_JOB_CHANNEL
		1
#નહીંતર
		0
#endif
		},
	{"cindent", 1},
	{"clientserver",
#ifdef FEAT_CLIENTSERVER
		1
#નહીંતર
		0
#endif
		},
	{"clipboard",
#ifdef FEAT_CLIPBOARD
		1
#નહીંતર
		0
#endif
		},
	{"clipboard_provider",
#ifdef FEAT_CLIPBOARD_PROVIDER
		1
#નહીંતર
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
#નહીંતર
		0
#endif
		},
	{"cryptv",
#ifdef FEAT_CRYPT
		1
#નહીંતર
		0
#endif
		},
	{"crypt-blowfish",
#ifdef FEAT_CRYPT
		1
#નહીંતર
		0
#endif
		},
	{"crypt-blowfish2",
#ifdef FEAT_CRYPT
		1
#નહીંતર
		0
#endif
		},
	{"cscope",
#ifdef FEAT_CSCOPE
		1
#નહીંતર
		0
#endif
		},
	{"cursorbind", 1},
	{"cursorshape",
#ifdef CURSOR_SHAPE
		1
#નહીંતર
		0
#endif
		},
	{"debug",
#ifdef DEBUG
		1
#નહીંતર
		0
#endif
		},
	{"dialog_con",
#ifdef FEAT_CON_DIALOG
		1
#નહીંતર
		0
#endif
		},
	{"dialog_con_gui",
#જો defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#નહીંતર
		0
#endif
		},
	{"dialog_gui",
#ifdef FEAT_GUI_DIALOG
		1
#નહીંતર
		0
#endif
		},
	{"diff",
#ifdef FEAT_DIFF
		1
#નહીંતર
		0
#endif
		},
	{"digraphs",
#ifdef FEAT_DIGRAPHS
		1
#નહીંતર
		0
#endif
		},
	{"directx",
#ifdef FEAT_DIRECTX
		1
#નહીંતર
		0
#endif
		},
	{"dnd",
#ifdef FEAT_DND
		1
#નહીંતર
		0
#endif
		},
	{"drop_file",
#ifdef HAVE_DROP_FILE
		1
#નહીંતર
		0
#endif
		},
	{"emacs_tags",
#ifdef FEAT_EMACS_TAGS
		1
#નહીંતર
		0
#endif
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#ifdef FEAT_શોધ_EXTRA
		1
#નહીંતર
		0
#endif
		},
	{"file_in_path", 1},
	{"filterpipe",
#જો defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#નહીંતર
		0
#endif
		},
	{"find_in_path",
#ifdef FEAT_FIND_ID
		1
#નહીંતર
		0
#endif
		},
	{"float", 1},
	{"folding",
#ifdef FEAT_FOLDING
		1
#નહીંતર
		0
#endif
		},
	{"footer", 0},
	{"fork",
#જો !defined(USE_SYSTEM) && defined(UNIX)
		1
#નહીંતર
		0
#endif
		},
	{"gettext",
#ifdef FEAT_GETTEXT
		1
#નહીંતર
		0
#endif
		},
	{"gui",
#ifdef FEAT_GUI
		1
#નહીંતર
		0
#endif
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#ifdef FEAT_GUI_GTK
		1
#નહીંતર
		0
#endif
		},
	{"gui_gtk2",
#જો defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#નહીંતર
		0
#endif
		},
	{"gui_gtk3",
#જો defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#નહીંતર
		0
#endif
		},
	{"gui_gtk4",
#જો defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#નહીંતર
		0
#endif
		},
	{"gui_gnome",
#ifdef FEAT_GUI_GNOME
		1
#નહીંતર
		0
#endif
		},
	{"gui_haiku",
#ifdef FEAT_GUI_HAIKU
		1
#નહીંતર
		0
#endif
		},
	{"gui_mac", 0},
	{"gui_motif",
#ifdef FEAT_GUI_MOTIF
		1
#નહીંતર
		0
#endif
		},
	{"gui_photon",
#ifdef FEAT_GUI_PHOTON
		1
#નહીંતર
		0
#endif
		},
	{"gui_win32",
#ifdef FEAT_GUI_MSWIN
		1
#નહીંતર
		0
#endif
		},
	{"iconv",
#જો defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#નહીંતર
		0
#endif
		},
	{"image",
#ifdef FEAT_IMAGE
		1
#નહીંતર
		0
#endif
		},
	{"image_popup",
#ifdef FEAT_IMAGE_POPUP
		1
#નહીંતર
		0
#endif
		},
	{"insert_expand", 1},
	{"ipv6",
#ifdef FEAT_IPV6
		1
#નહીંતર
		0
#endif
	},
	{"job",
#ifdef FEAT_JOB_CHANNEL
		1
#નહીંતર
		0
#endif
		},
	{"jumplist", 1},
	{"keymap",
#ifdef FEAT_KEYMAP
		1
#નહીંતર
		0
#endif
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#ifdef FEAT_LANGMAP
		1
#નહીંતર
		0
#endif
		},
	{"libcall",
#ifdef FEAT_LIBCALL
		1
#નહીંતર
		0
#endif
		},
	{"linebreak",
#ifdef FEAT_LINEBREAK
		1
#નહીંતર
		0
#endif
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#જો defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#નહીંતર
		0
#endif
		},
	{"menu",
#ifdef FEAT_MENU
		1
#નહીંતર
		0
#endif
		},
	{"mksession",
#ifdef FEAT_SESSION
		1
#નહીંતર
		0
#endif
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#ifdef FEAT_MOUSESHAPE
		1
#નહીંતર
		0
#endif
		},
	{"mouse_dec",
#જો (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#નહીંતર
		0
#endif
		},
	{"mouse_gpm",
#જો (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#નહીંતર
		0
#endif
		},
	{"mouse_jsbterm",
#જો (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#નહીંતર
		0
#endif
		},
	{"mouse_netterm",
#જો (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#નહીંતર
		0
#endif
		},
	{"mouse_pterm",
#જો (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#નહીંતર
		0
#endif
		},
	{"mouse_sgr",
#જો (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#નહીંતર
		0
#endif
		},
	{"mouse_sysmouse",
#જો (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#નહીંતર
		0
#endif
		},
	{"mouse_urxvt",
#જો (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#નહીંતર
		0
#endif
		},
	{"mouse_xterm",
#જો (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#નહીંતર
		0
#endif
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#ifdef FEAT_MBYTE_IME
		1
#નહીંતર
		0
#endif
		},
	{"multi_lang",
#ifdef FEAT_MULTI_LANG
		1
#નહીંતર
		0
#endif
		},
	{"mzscheme",
#જો defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#નહીંતર
		0
#endif
		},
	{"nanotime",
#ifdef ST_MTIM_NSEC
		1
#નહીંતર
		0
#endif
	},
	{"num64", 1},
	{"ole",
#ifdef FEAT_OLE
		1
#નહીંતર
		0
#endif
		},
	{"packages",
#ifdef FEAT_EVAL
		1
#નહીંતર
		0
#endif
		},
	{"path_extra", 1},
	{"perl",
#જો defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#નહીંતર
		0
#endif
		},
	{"persistent_undo",
#ifdef FEAT_PERSISTENT_UNDO
		1
#નહીંતર
		0
#endif
		},
	{"python_compiled",
#જો defined(FEAT_PYTHON)
		1
#નહીંતર
		0
#endif
		},
	{"python_dynamic",
#જો defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#નહીંતર
		0
#endif
		},
	{"python",
#જો defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#નહીંતર
		0
#endif
		},
	{"pythonx",
#જો (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#નહીંતર
		0
#endif
		},
	{"python3_compiled",
#જો defined(FEAT_PYTHON3)
		1
#નહીંતર
		0
#endif
		},
	{"python3_dynamic",
#જો defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#નહીંતર
		0
#endif
		},
	{"python3_stable",
#જો defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#નહીંતર
		0
#endif
		},
	{"python3",
#જો defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#નહીંતર
		0
#endif
		},
	{"popupwin",
#ifdef FEAT_PROP_POPUP
		1
#નહીંતર
		0
#endif
		},
	{"postscript",
#ifdef FEAT_POSTSCRIPT
		1
#નહીંતર
		0
#endif
		},
	{"pango",
#ifdef FEAT_PRINT_PANGO
		1
#નહીંતર
		0
#endif
		},
	{"printer",
#ifdef FEAT_PRINTER
		1
#નહીંતર
		0
#endif
		},
	{"profile",
#ifdef FEAT_PROFILE
		1
#નહીંતર
		0
#endif
		},
	{"prof_nsec",
#ifdef PROF_NSEC
		1
#નહીંતર
		0
#endif
		},
	{"reltime",
#ifdef FEAT_RELTIME
		1
#નહીંતર
		0
#endif
		},
	{"quickfix",
#ifdef FEAT_QUICKFIX
		1
#નહીંતર
		0
#endif
		},
	{"rightleft",
#ifdef FEAT_RIGHTLEFT
		1
#નહીંતર
		0
#endif
		},
	{"ruby",
#જો defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#નહીંતર
		0
#endif
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#ifdef FEAT_SIGNS
		1
#નહીંતર
		0
#endif
		},
	{"smartindent", 1},
	{"startuptime",
#ifdef STARTUPTIME
		1
#નહીંતર
		0
#endif
		},
	{"statusline",
#ifdef FEAT_STL_OPT
		1
#નહીંતર
		0
#endif
		},
	{"statusline_click",
#ifdef FEAT_STL_OPT
		1
#નહીંતર
		0
#endif
		},
	{"netbeans_intg",
#ifdef FEAT_NETBEANS_INTG
		1
#નહીંતર
		0
#endif
		},
	{"sodium",
#જો defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#નહીંતર
		0
#endif
		},
	{"sound",
#ifdef FEAT_SOUND
		1
#નહીંતર
		0
#endif
		},
	{"spell",
#ifdef FEAT_SPELL
		1
#નહીંતર
		0
#endif
		},
	{"syntax",
#ifdef FEAT_SYN_HL
		1
#નહીંતર
		0
#endif
		},
	{"system",
#જો defined(USE_SYSTEM) || !defined(UNIX)
		1
#નહીંતર
		0
#endif
		},
	{"tabpanel",
#જો defined(FEAT_TABPANEL)
		1,
#નહીંતર
		0,
#endif
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#જો defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#નહીંતર
		0
#endif
		},
	{"termguicolors",
#ifdef FEAT_TERMGUICOLORS
		1
#નહીંતર
		0
#endif
		},
	{"terminal",
#જો defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#નહીંતર
		0
#endif
		},
	{"terminfo",
#ifdef TERMINFO
		1
#નહીંતર
		0
#endif
		},
	{"termresponse",
#ifdef FEAT_TERMRESPONSE
		1
#નહીંતર
		0
#endif
		},
	{"textobjects", 1},
	{"textprop",
#ifdef FEAT_PROP_POPUP
		1
#નહીંતર
		0
#endif
		},
	{"tgetent",
#ifdef HAVE_TGETENT
		1
#નહીંતર
		0
#endif
		},
	{"timers",
#ifdef FEAT_TIMERS
		1
#નહીંતર
		0
#endif
		},
	{"title", 1},
	{"toolbar",
#ifdef FEAT_TOOLBAR
		1
#નહીંતર
		0
#endif
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#ifdef FEAT_VARTABS
		1
#નહીંતર
		0
#endif
		},
	{"vertsplit", 1},
	{"viminfo",
#ifdef FEAT_VIMINFO
		1
#નહીંતર
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
#નહીંતર
		0
#endif
		},
	{"wayland",
#ifdef FEAT_WAYLAND
		1
#નહીંતર
		0
#endif
		},
	{"wayland_clipboard",
#ifdef FEAT_WAYLAND_CLIPBOARD
		1
#નહીંતર
		0
#endif
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#ifdef FEAT_WAK
		1
#નહીંતર
		0
#endif
		},
	{"writebackup",
#ifdef FEAT_WRITEBACKUP
		1
#નહીંતર
		0
#endif
		},
	{"xattr",
#ifdef FEAT_XATTR
		1
#નહીંતર
		0
#endif
		},
	{"xim",
#ifdef FEAT_XIM
		1
#નહીંતર
		0
#endif
		},
	{"xfontset",
#ifdef FEAT_XFONTSET
		1
#નહીંતર
		0
#endif
		},
	{"xpm",
#જો defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#નહીંતર
		0
#endif
		},
	{"xpm_w32",	// for backward compatibility
#ifdef FEAT_XPM_W32
		1
#નહીંતર
		0
#endif
		},
	{"xsmp",
#ifdef USE_XSMP
		1
#નહીંતર
		0
#endif
		},
	{"xsmp_interact",
#ifdef USE_XSMP_INTERACT
		1
#નહીંતર
		0
#endif
		},
	{"xterm_clipboard",
#ifdef FEAT_XCLIPBOARD
		1
#નહીંતર
		0
#endif
		},
	{"xterm_save",
#ifdef FEAT_XTERM_SAVE
		1
#નહીંતર
		0
#endif
		},
	{"X11",
#જો defined(UNIX) && defined(FEAT_X11)
		1
#નહીંતર
		0
#endif
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#જો defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#નહીંતર
		0
#endif
		},
	{ખાલી, 0}
    };

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	પાછા;

    name = tv_get_string(&argvars[0]);

    // Fast-path: check features not in has_list[] first to avoid the full
    // linear scan for very common queries like has('patch-...').
    જો (STRNICMP(name, "patch", 5) == 0)
    {
	x = TRUE;
	જો (name[5] == '-'
		&& STRLEN(name) >= 11
		&& (name[6] >= '1' && name[6] <= '9'))
	{
	    અક્ષર	*end;
	    પૂર્ણાંક	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (પૂર્ણાંક)strtoul((અક્ષર *)name + 6, &end, 10);
	    જો (*end == '.' && vim_isdigit(end[1])
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
	નહીંતર જો (SAFE_isdigit(name[5]))
	    n = has_patch(atoi((અક્ષર *)name + 5));
    }
    નહીંતર જો (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    નહીંતર જો (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_આગત_isatty();
    }
    નહીંતર જો (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    નહીંતર જો (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    નહીંતર જો (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#ifdef FEAT_GUI
	n = (gui.in_use || gui.starting);
#endif
    }
    નહીંતર જો (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#જો defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.in_use;	// gui_mch_browse() works when GUI is running
#endif
    }
    નહીંતર જો (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#ifdef FEAT_SYN_HL
	n = syntax_present(curwin);
#endif
    }
    નહીંતર જો (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#ifdef FEAT_VTP
	n = is_term_win32() && has_vtp_working();
#endif
    }
    નહીંતર જો (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_NETBEANS_INTG
	n = netbeans_active();
#endif
    }
    નહીંતર જો (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_MOUSE_GPM
	n = gpm_enabled();
#endif
    }
    નહીંતર જો (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#જો defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#endif
    }
    નહીંતર જો (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_star.available;
#endif
    }
    નહીંતર જો (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# જો defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	જો (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	નહીંતર
	    n = FALSE;
# નહીંતર
	n = FALSE;
# endif
#endif
    }

    // Look up in has_list[] only if not already handled above.
    જો (x == FALSE)
    {
	માટે (i = 0; has_list[i].name != ખાલી; ++i)
	    જો (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		તોડો;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	જો (x == TRUE && n == FALSE)
	{
	    જો (0)
	    {
		// intentionally empty
	    }
#ifdef VIMDLL
	    નહીંતર જો (STRICMP(name, "filterpipe") == 0)
		n = gui.in_use || gui.starting;
#endif
#જો defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    નહીંતર જો (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#endif
#ifdef DYNAMIC_LUA
	    નહીંતર જો (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#endif
#ifdef DYNAMIC_MZSCHEME
	    નહીંતર જો (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#endif
#ifdef DYNAMIC_PERL
	    નહીંતર જો (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON
	    નહીંતર જો (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON3
	    નહીંતર જો (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#endif
#જો defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    નહીંતર જો (STRICMP(name, "pythonx") == 0)
	    {
# જો defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		જો (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		નહીંતર જો (p_pyx == 3)
		    n = python3_enabled(FALSE);
		નહીંતર જો (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#endif
#ifdef DYNAMIC_RUBY
	    નહીંતર જો (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#endif
#ifdef DYNAMIC_TCL
	    નહીંતર જો (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#endif
#ifdef DYNAMIC_SODIUM
	    નહીંતર જો (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#endif
#જો defined(FEAT_TERMINAL) && defined(MSWIN)
	    નહીંતર જો (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#endif
#ifdef DYNAMIC_GPM
	    નહીંતર જો (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_available();
#endif
	}
    }

    જો (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_સંખ્યા = x;
    નહીંતર
	// return whether feature is enabled
	rettv->vval.v_સંખ્યા = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    પૂર્ણાંક
dynamic_feature(char_u *feature)
{
    પાછા (feature == ખાલી
#જો defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.in_use)
#endif
#ifdef VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#endif
#જો defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.in_use)
#endif
#જો defined(USE_ICONV) && defined(DYNAMIC_ICONV)
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
#જો defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
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
#જો defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#endif
	    );
}

/*
 * "haslocaldir()" function
 */
    સ્થિર શૂન્ય
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = ખાલી;
    win_T	*wp = ખાલી;

    જો (in_vim9script()
	    && (check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL)))
	પાછા;

    wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    જો (wp != ખાલી && wp->w_localdir != ખાલી)
	rettv->vval.v_સંખ્યા = 1;
    નહીંતર જો (tp != ખાલી && tp->tp_localdir != ખાલી)
	rettv->vval.v_સંખ્યા = 2;
    નહીંતર
	rettv->vval.v_સંખ્યા = 0;
}

/*
 * "highlightID(name)" function
 */
    સ્થિર શૂન્ય
f_hlID(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    સ્થિર શૂન્ય
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    સ્થિર શૂન્ય
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
# જો defined(HAVE_NO_LONG_LONG)
પ્રકારવ્યાખ્યા unsigned પૂર્ણાંક uintptr_t;
# નહીંતર
પ્રકારવ્યાખ્યા unsigned long long uintptr_t;
# endif
#endif // VMS

    સ્થિર શૂન્ય
f_id(typval_T *argvars, typval_T *rettv)
{
    અક્ષર    numbuf[NUMBUFLEN];
    અક્ષર    *p = numbuf;

    પસંદગી (argvars[0].v_type)
    {
	સ્થિતિ VAR_LIST:
	સ્થિતિ VAR_TUPLE:
	સ્થિતિ VAR_DICT:
	સ્થિતિ VAR_OBJECT:
	સ્થિતિ VAR_JOB:
	સ્થિતિ VAR_CHANNEL:
	સ્થિતિ VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    જો (argvars[0].vval.v_object != ખાલી)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(શૂન્ય *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		માટે (; p < numbuf + કદ(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	મૂળભૂત:
	    તોડો;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    સ્થિર શૂન્ય
index_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    પૂર્ણાંક		start = 0;
    પૂર્ણાંક		error = FALSE;
    પૂર્ણાંક		ic = FALSE;

    b = argvars[0].vval.v_blob;
    જો (b == ખાલી)
	પાછા;

    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	start = tv_get_સંખ્યા_chk(&argvars[2], &error);
	જો (error)
	    પાછા;
    }

    જો (start < 0)
    {
	start = blob_len(b) + start;
	જો (start < 0)
	    start = 0;
    }

    માટે (પૂર્ણાંક idx = start; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_સંખ્યા;
	tv.vval.v_સંખ્યા = blob_get(b, idx);
	જો (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_સંખ્યા = idx;
	    પાછા;
	}
    }
}

/*
 * index() function for a tuple
 */
    સ્થિર શૂન્ય
index_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    પૂર્ણાંક		ic = FALSE;
    પૂર્ણાંક		error = FALSE;

    જો (tuple == ખાલી)
	પાછા;

    પૂર્ણાંક	start_idx = 0;
    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	start_idx = tv_get_સંખ્યા_chk(&argvars[2], &error);
	જો (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (પૂર્ણાંક)tv_get_bool_chk(&argvars[3], &error);
	જો (error)
	    પાછા;
    }

    rettv->vval.v_સંખ્યા = index_tuple(tuple, &argvars[1], start_idx, ic);
}

/*
 * index() function for a list
 */
    સ્થિર શૂન્ય
index_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*item;
    long	idx = 0;
    પૂર્ણાંક		ic = FALSE;
    પૂર્ણાંક		error = FALSE;

    l = argvars[0].vval.v_list;
    જો (l == ખાલી)
	પાછા;

    CHECK_LIST_MATERIALIZE(l);
    item = l->lv_first;
    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, (long)tv_get_સંખ્યા_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	જો (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (પૂર્ણાંક)tv_get_bool_chk(&argvars[3], &error);
	જો (error)
	    item = ખાલી;
    }

    માટે ( ; item != ખાલી; item = item->li_next, ++idx)
	જો (tv_equal(&item->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_સંખ્યા = idx;
	    તોડો;
	}
}

/*
 * "index()" function
 */
    સ્થિર શૂન્ય
f_index(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_સંખ્યા_arg(argvars, 1) == FAIL)
		|| check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	પાછા;

    જો (argvars[0].v_type == VAR_BLOB)
	index_func_blob(argvars, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_TUPLE)
	index_func_tuple(argvars, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_LIST)
	index_func_list(argvars, rettv);
    નહીંતર
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    પૂર્ણાંક
indexof_eval_પદાવલિ(typval_T *પદાવલિ)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	found;
    પૂર્ણાંક		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_ચાવી);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    જો (eval_પદાવલિ_typval(પદાવલિ, FALSE, argv, 2, ખાલી, &newtv) == FAIL)
	પાછા FALSE;

    found = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    પાછા error ? FALSE : found;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    સ્થિર પૂર્ણાંક
indexof_blob(blob_T *b, long startidx, typval_T *પદાવલિ)
{
    long	idx = 0;

    જો (b == ખાલી)
	પાછા -1;

    જો (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	જો (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_ચાવી, VAR_સંખ્યા);
    set_vim_var_type(VV_VAL, VAR_સંખ્યા);

    પૂર્ણાંક		called_emsg_start = called_emsg;
    માટે (idx = startidx; idx < blob_len(b); ++idx)
    {
	set_vim_var_nr(VV_ચાવી, idx);
	set_vim_var_nr(VV_VAL, blob_get(b, idx));

	જો (indexof_eval_પદાવલિ(પદાવલિ))
	    પાછા idx;

	જો (called_emsg != called_emsg_start)
	    પાછા -1;
    }

    પાછા -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    સ્થિર પૂર્ણાંક
indexof_list(list_T *l, long startidx, typval_T *પદાવલિ)
{
    listitem_T	*item;
    long	idx = 0;
    પૂર્ણાંક		found;

    જો (l == ખાલી)
	પાછા -1;

    CHECK_LIST_MATERIALIZE(l);

    જો (startidx == 0)
	item = l->lv_first;
    નહીંતર
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, startidx);
	જો (item != ખાલી)
	    idx = l->lv_u.mat.lv_idx;
    }

    set_vim_var_type(VV_ચાવી, VAR_સંખ્યા);

    પૂર્ણાંક		called_emsg_start = called_emsg;
    માટે ( ; item != ખાલી; item = item->li_next, ++idx)
    {
	set_vim_var_nr(VV_ચાવી, idx);
	copy_tv(&item->li_tv, get_vim_var_tv(VV_VAL));

	found = indexof_eval_પદાવલિ(પદાવલિ);
	clear_tv(get_vim_var_tv(VV_VAL));

	જો (found)
	    પાછા idx;

	જો (called_emsg != called_emsg_start)
	    પાછા -1;
    }

    પાછા -1;
}

/*
 * "indexof()" function
 */
    સ્થિર શૂન્ય
f_indexof(typval_T *argvars, typval_T *rettv)
{
    long	startidx = 0;
    typval_T	save_val;
    typval_T	save_ચાવી;
    પૂર્ણાંક		save_did_emsg;

    rettv->vval.v_સંખ્યા = -1;

    જો (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	પાછા;

    જો ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == ખાલી
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == ખાલી))
	પાછા;

    જો (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_સંખ્યા_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_ચાવી, &save_ચાવી);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    જો (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_સંખ્યા = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    નહીંતર જો (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_સંખ્યા = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    નહીંતર
	rettv->vval.v_સંખ્યા = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_ચાવી, &save_ચાવી);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

સ્થિર પૂર્ણાંક inputsecret_flag = 0;

/*
 * "input()" function
 *     Also handles inputsecret() when inputsecret is set.
 */
    સ્થિર શૂન્ય
f_આગત(typval_T *argvars, typval_T *rettv)
{
    get_user_આગત(argvars, rettv, FALSE, inputsecret_flag);
}

/*
 * "inputdialog()" function
 */
    સ્થિર શૂન્ય
f_inputdialog(typval_T *argvars, typval_T *rettv)
{
#જો defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    જો (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == ખાલી)
    {
	char_u	*message;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	જો (in_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    પાછા;

	message = tv_get_string_chk(&argvars[0]);
	જો (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != ખાલી)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	નહીંતર
	    IObuff[0] = NUL;
	જો (message != ખાલી && defstr != ખાલી
		&& do_dialog(VIM_QUESTION, ખાલી, message,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	નહીંતર
	{
	    જો (message != ખાલી && defstr != ખાલી
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    નહીંતર
		rettv->vval.v_string = ખાલી;
	}
	rettv->v_type = VAR_STRING;
    }
    નહીંતર
#endif
	get_user_આગત(argvars, rettv, TRUE, inputsecret_flag);
}

/*
 * "inputlist()" function
 */
    સ્થિર શૂન્ય
f_inputlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    પૂર્ણાંક		selected;
    પૂર્ણાંક		mouse_used;

#ifdef NO_CONSOLE_આગત
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    જો (no_console_આગત() && !is_not_a_term())
	પાછા;
#endif
    જો (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ખાલી)
    {
	semsg(_(e_argument_of_str_must_be_list), "inputlist()");
	પાછા;
    }

    સંદેશ_start();
    સંદેશ_row = Rows - 1;	// for when 'cmdheight' > 1
    લીટીઓ_ડાબું = Rows;	// avoid more prompt
    સંદેશ_scroll = TRUE;
    સંદેશ_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	સંદેશ_puts((અક્ષર *)tv_get_string(&li->li_tv));
	સંદેશ_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_સંખ્યા(&mouse_used);
    જો (mouse_used)
	selected -= લીટીઓ_ડાબું;

    rettv->vval.v_સંખ્યા = selected;
}

સ્થિર garray_T	    ga_userinput = {0, 0, કદ(tasave_T), 4, ખાલી};

/*
 * "inputrestore()" function
 */
    સ્થિર શૂન્ય
f_inputrestore(typval_T *argvars UNUSED, typval_T *rettv)
{
    જો (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_data)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    નહીંતર જો (p_verbose > 1)
    {
	verb_સંદેશ(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_સંખ્યા = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    સ્થિર શૂન્ય
f_inputsave(typval_T *argvars UNUSED, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    જો (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_data)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    નહીંતર
	rettv->vval.v_સંખ્યા = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    સ્થિર શૂન્ય
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	પાછા;

    ++cmdline_star;
    ++inputsecret_flag;
    f_આગત(argvars, rettv);
    --cmdline_star;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    સ્થિર શૂન્ય
f_interrupt(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    સ્થિર શૂન્ય
f_invert(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = ~tv_get_સંખ્યા_chk(&argvars[0], ખાલી);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    સ્થિર શૂન્ય
free_lval_મૂળ(lval_મૂળ_T *મૂળ)
{
    જો (મૂળ->lr_tv != ખાલી)
	free_tv(મૂળ->lr_tv);
    class_unref(મૂળ->lr_cl_exec);
    મૂળ->lr_tv = ખાલી;
    મૂળ->lr_cl_exec = ખાલી;
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

    સ્થિર પૂર્ણાંક
fix_variable_reference_lval_મૂળ(lval_મૂળ_T *મૂળ, char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    પૂર્ણાંક found_member = FALSE;
    જો (મૂળ->lr_tv->v_type == VAR_OBJECT)
    {
	જો (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    found_member = TRUE;
    }
    જો (!found_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*end;
	માટે (end = name; ASCII_ISALNUM(*end) || *end == '_'; ++end)
	    ;
	પૂર્ણાંક idx = class_member_idx(મૂળ->lr_cl_exec, name, end - name);
	જો (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(મૂળ->lr_tv);
	    copy_tv(&મૂળ->lr_cl_exec->class_members_tv[idx], મૂળ->lr_tv);
	    found_member = TRUE;
	}
    }
    જો (!found_member)
    {
	free_tv(મૂળ->lr_tv);
	મૂળ->lr_tv = ખાલી;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    પાછા OK;
}

/*
 * "islocked()" function
 */
    સ્થિર શૂન્ય
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*end;
    dictitem_T	*di;

    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    char_u *name = tv_get_string(&argvars[0]);
#ifdef LOG_LOCKVAR
    ch_log(ખાલી, "LKVAR: f_islocked(): name: %s", name);
#endif

    lval_મૂળ_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_મૂળ_T *મૂળ = ખાલી;

    // Set up lval_root if executing in a method.
    જો (fill_exec_lval_મૂળ(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	જો (fix_variable_reference_lval_મૂળ(&aroot, name) == OK)
	    મૂળ = &aroot;
    }

    lval_મૂળ_T	*lval_મૂળ_save = lval_મૂળ;
    lval_મૂળ = મૂળ;
    end = get_lval(name, ખાલી, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_START);
    lval_મૂળ = lval_મૂળ_save;

    જો (end != ખાલી && lv.ll_name != ખાલી)
    {
	જો (*end != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_end
		   ? e_invalid_argument_str : e_trailing_characters_str), end);
	}
	નહીંતર
	{
	    જો (lv.ll_tv == ખાલી)
	    {
		di = find_var(lv.ll_name, ખાલી, TRUE);
		જો (di != ખાલી)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_સંખ્યા = ((di->di_flags & DI_FLAGS_LOCK)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    નહીંતર જો (lv.ll_is_મૂળ)
	    {
		rettv->vval.v_સંખ્યા = tv_islocked(lv.ll_tv);
	    }
	    નહીંતર જો (lv.ll_object != ખાલી)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_સંખ્યા = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(ખાલી, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#endif
	    }
	    નહીંતર જો (lv.ll_class != ખાલી)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_સંખ્યા = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(ખાલી, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#endif
	    }
	    નહીંતર જો (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    નહીંતર જો (lv.ll_newkey != ખાલી)
		semsg(_(e_ચાવી_not_present_in_dictionary_str), lv.ll_newkey);
	    નહીંતર જો (lv.ll_list != ખાલી)
		// List item.
		rettv->vval.v_સંખ્યા = tv_islocked(&lv.ll_li->li_tv);
	    નહીંતર જો (lv.ll_tuple != ખાલી)
		// Tuple item.
		rettv->vval.v_સંખ્યા = tv_islocked(lv.ll_tv);
	    નહીંતર
		// Dictionary item.
		rettv->vval.v_સંખ્યા = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    જો (મૂળ != ખાલી)
	free_lval_મૂળ(મૂળ);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    સ્થિર શૂન્ય
f_વસ્તુઓ(typval_T *argvars, typval_T *rettv)
{
    જો (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    નહીંતર
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), 1);
}

/*
 * "keytrans()" function
 */
    સ્થિર શૂન્ય
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    જો (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == ખાલી)
	પાછા;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    સ્થિર શૂન્ય
f_last_buffer_nr(typval_T *argvars UNUSED, typval_T *rettv)
{
    પૂર્ણાંક		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	જો (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_સંખ્યા = n;
}

/*
 * "len()" function
 */
    શૂન્ય
f_len(typval_T *argvars, typval_T *rettv)
{
    પસંદગી (argvars[0].v_type)
    {
	સ્થિતિ VAR_STRING:
	સ્થિતિ VAR_સંખ્યા:
	    rettv->vval.v_સંખ્યા = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    તોડો;
	સ્થિતિ VAR_BLOB:
	    rettv->vval.v_સંખ્યા = blob_len(argvars[0].vval.v_blob);
	    તોડો;
	સ્થિતિ VAR_LIST:
	    rettv->vval.v_સંખ્યા = list_len(argvars[0].vval.v_list);
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    rettv->vval.v_સંખ્યા = tuple_len(argvars[0].vval.v_tuple);
	    તોડો;
	સ્થિતિ VAR_DICT:
	    rettv->vval.v_સંખ્યા = dict_len(argvars[0].vval.v_dict);
	    તોડો;
	સ્થિતિ VAR_OBJECT:
	    rettv->vval.v_સંખ્યા = object_len(argvars[0].vval.v_object);
	    તોડો;
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	સ્થિતિ VAR_FLOAT:
	સ્થિતિ VAR_FUNC:
	સ્થિતિ VAR_PARTIAL:
	સ્થિતિ VAR_JOB:
	સ્થિતિ VAR_CHANNEL:
	સ્થિતિ VAR_INSTR:
	સ્થિતિ VAR_CLASS:
	સ્થિતિ VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    તોડો;
    }
}

    સ્થિર શૂન્ય
libcall_common(typval_T *argvars UNUSED, typval_T *rettv, પૂર્ણાંક type)
{
#ifdef FEAT_LIBCALL
    char_u		*string_in;
    char_u		**string_result;
    પૂર્ણાંક			nr_result;
#endif

    rettv->v_type = type;
    જો (type != VAR_સંખ્યા)
	rettv->vval.v_string = ખાલી;

    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_સંખ્યા_arg(argvars, 2) == FAIL))
	પાછા;

#ifdef FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    જો (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_in = ખાલી;
	જો (argvars[2].v_type == VAR_STRING)
	    string_in = argvars[2].vval.v_string;
	જો (type == VAR_સંખ્યા)
	{
	    string_result = ખાલી;
	}
	નહીંતર
	{
	    rettv->vval.v_string = ખાલી;
	    string_result = &rettv->vval.v_string;
	}
	જો (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_in,
			     argvars[2].vval.v_સંખ્યા,
			     string_result,
			     &nr_result) == OK
		&& type == VAR_સંખ્યા)
	    rettv->vval.v_સંખ્યા = nr_result;
    }
#endif
}

/*
 * "libcall()" function
 */
    સ્થિર શૂન્ય
f_libcall(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_STRING);
}

/*
 * "libcallnr()" function
 */
    સ્થિર શૂન્ય
f_libcallnr(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_સંખ્યા);
}

/*
 * "line(string, [winid])" function
 */
    સ્થિર શૂન્ય
f_લીટી(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum = 0;
    pos_T	*fp = ખાલી;
    પૂર્ણાંક		fnum;
    પૂર્ણાંક		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	જો (wp != ખાલી && tp != ખાલી)
	{
	    જો (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		જો (*p_spk != 'c'
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
    નહીંતર
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    જો (fp != ખાલી)
	lnum = fp->lnum;
    rettv->vval.v_સંખ્યા = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    સ્થિર શૂન્ય
f_line2byte(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_સંખ્યા = -1;
#નહીંતર
    linenr_T	lnum;

    જો (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	પાછા;

    lnum = tv_get_lnum(argvars);
    જો (lnum < 1 || lnum > curbuf->b_ml.ml_લીટી_ગણતરી + 1)
	rettv->vval.v_સંખ્યા = -1;
    નહીંતર
	rettv->vval.v_સંખ્યા = ml_find_લીટી_or_offset(curbuf, lnum, ખાલી);
    જો (rettv->vval.v_સંખ્યા >= 0)
	++rettv->vval.v_સંખ્યા;
#endif
}

#ifdef FEAT_LUA
/*
 * "luaeval()" function
 */
    સ્થિર શૂન્ય
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#endif

પ્રકારવ્યાખ્યા ગણના
{
    MATCH_END,	    // matchend()
    MATCH_MATCH,    // match()
    MATCH_STR,	    // matchstr()
    MATCH_LIST,	    // matchlist()
    MATCH_POS	    // matchstrpos()
} matchtype_T;

    સ્થિર શૂન્ય
find_some_match(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = ખાલી;
    long	len = 0;
    char_u	*પદાવલિ = ખાલી;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    long	start = 0;
    long	nth = 1;
    colnr_T	startcol = 0;
    પૂર્ણાંક		match = 0;
    list_T	*l = ખાલી;
    listitem_T	*li = ખાલી;
    long	idx = 0;
    પૂર્ણાંક		prev_lock = 0;
    char_u	*tofree = ખાલી;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    rettv->vval.v_સંખ્યા = -1;
    જો (type == MATCH_LIST || type == MATCH_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	જો (rettv_list_alloc(rettv) == FAIL)
	    જાઓ theend;
	જો (type == MATCH_POS
		&& (list_append_string(rettv->vval.v_list,
					    (char_u *)"", 0) == FAIL
		    || list_append_સંખ્યા(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_સંખ્યા(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_સંખ્યા(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL))
	{
		list_free(rettv->vval.v_list);
		rettv->vval.v_list = ખાલી;
		જાઓ theend;
	}
    }
    નહીંતર જો (type == MATCH_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ખાલી;
    }

    જો (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 3) == FAIL)))
	જાઓ theend;

    જો (argvars[0].v_type == VAR_LIST)
    {
	જો ((l = argvars[0].vval.v_list) == ખાલી)
	    જાઓ theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    નહીંતર
    {
	પદાવલિ = str = tv_get_string(&argvars[0]);
	len = (long)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    જો (pat == ખાલી)
	જાઓ theend;

    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	પૂર્ણાંક	    error = FALSE;

	start = (long)tv_get_સંખ્યા_chk(&argvars[2], &error);
	જો (error)
	    જાઓ theend;
	જો (l != ખાલી)
	{
	    li = list_find(l, start);
	    જો (li == ખાલી)
		જાઓ theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	નહીંતર
	{
	    જો (start < 0)
		start = 0;
	    જો (start > len)
		જાઓ theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    જો (argvars[3].v_type != VAR_UNKNOWN)
		startcol = start;
	    નહીંતર
	    {
		str += start;
		len -= start;
	    }
	}

	જો (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (long)tv_get_સંખ્યા_chk(&argvars[3], &error);
	જો (error)
	    જાઓ theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    જો (regmatch.regprog != ખાલી)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	જો (l != ખાલી)
	{
	    prev_lock = l->lv_lock;
	    જો (l->lv_lock == 0)
		l->lv_lock = VAR_LOCKED;
	}

	માટે (;;)
	{
	    જો (l != ખાલી)
	    {
		જો (li == ખાલી)
		{
		    match = FALSE;
		    તોડો;
		}
		vim_free(tofree);
		પદાવલિ = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		જો (str == ખાલી)
		    તોડો;
	    }

	    match = vim_regexec_nl(&regmatch, str, startcol);

	    જો (match && --nth <= 0)
		તોડો;
	    જો (l == ખાલી && !match)
		તોડો;

	    // Advance to just after the match.
	    જો (l != ખાલી)
	    {
		li = li->li_next;
		++idx;
	    }
	    નહીંતર
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		જો (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    match = FALSE;
		    તોડો;
		}
	    }
	}

	જો (match)
	{
	    જો (type == MATCH_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_first;
		listitem_T *li2 = li1->li_next;
		listitem_T *li3 = li2->li_next;
		listitem_T *li4 = li3->li_next;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_સંખ્યા =
				      (varnumber_T)(regmatch.startp[0] - પદાવલિ);
		li4->li_tv.vval.v_સંખ્યા =
					(varnumber_T)(regmatch.endp[0] - પદાવલિ);
		જો (l != ખાલી)
		    li2->li_tv.vval.v_સંખ્યા = (varnumber_T)idx;
	    }
	    નહીંતર જો (type == MATCH_LIST)
	    {
		પૂર્ણાંક i;

		// return list with matched string and submatches
		માટે (i = 0; i < NSUBEXP; ++i)
		{
		    જો (regmatch.endp[i] == ખાલી)
		    {
			જો (list_append_string(rettv->vval.v_list,
							    ખાલી, 0) == FAIL)
			    તોડો;
		    }
		    નહીંતર જો (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(પૂર્ણાંક)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			તોડો;
		}
	    }
	    નહીંતર જો (type == MATCH_STR)
	    {
		// return matched string
		જો (l != ખાલી)
		    copy_tv(&li->li_tv, rettv);
		નહીંતર
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    નહીંતર જો (l != ખાલી)
		rettv->vval.v_સંખ્યા = idx;
	    નહીંતર
	    {
		જો (type != MATCH_END)
		    rettv->vval.v_સંખ્યા =
				      (varnumber_T)(regmatch.startp[0] - str);
		નહીંતર
		    rettv->vval.v_સંખ્યા =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_સંખ્યા += (varnumber_T)(str - પદાવલિ);
	    }
	}
	જો (l != ખાલી)
	    l->lv_lock = prev_lock;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    જો (type == MATCH_POS && l == ખાલી && rettv->vval.v_list != ખાલી)
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
    સ્થિર પૂર્ણાંક
get_matches_in_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    પૂર્ણાંક		idx,
    પૂર્ણાંક		submatches,
    પૂર્ણાંક		matchbuf)
{
    long	len = (long)STRLEN(str);
    પૂર્ણાંક		match = 0;
    colnr_T	startidx = 0;

    માટે (;;)
    {
	match = vim_regexec_nl(rmp, str, startidx);
	જો (!match)
	    તોડો;

	dict_T *d = dict_alloc();
	જો (d == ખાલી)
	    પાછા FAIL;
	જો (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    પાછા FAIL;
	}

	જો (dict_add_સંખ્યા(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    પાછા FAIL;

	જો (dict_add_સંખ્યા(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    પાછા FAIL;

	જો (dict_add_string_len(d, "text", rmp->startp[0],
		    (પૂર્ણાંક)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    પાછા FAIL;

	જો (submatches)
	{
	    list_T *sml = list_alloc();
	    જો (sml == ખાલી)
		પાછા FAIL;

	    જો (dict_add_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		પાછા FAIL;
	    }

	    // return a list with the submatches
	    માટે (પૂર્ણાંક i = 1; i < NSUBEXP; ++i)
	    {
		જો (rmp->endp[i] == ખાલી)
		{
		    જો (list_append_string(sml, (char_u *)"", 0) == FAIL)
			પાછા FAIL;
		}
		નહીંતર જો (list_append_string(sml, rmp->startp[i],
			    (પૂર્ણાંક)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    પાછા FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	જો (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    તોડો;
    }

    પાછા OK;
}

/*
 * "matchbufline()" function
 */
    સ્થિર શૂન્ય
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ખાલી;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_સંખ્યા = -1;
    જો (rettv_list_alloc(rettv) != OK)
	પાછા;
    retlist = rettv->vval.v_list;

    જો (check_for_buffer_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	પાછા;

    પૂર્ણાંક prev_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    જો (buf == ખાલી)
    {
	જો (did_emsg == prev_did_emsg)
	    semsg(_(e_invalid_buffer_name_str), tv_get_string(&argvars[0]));
	પાછા;
    }
    જો (buf->b_ml.ml_mfp == ખાલી)
    {
	emsg(_(e_buffer_is_not_loaded));
	પાછા;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    પૂર્ણાંક		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    જો (did_emsg > did_emsg_before)
	પાછા;
    જો (slnum < 1)
    {
	semsg(_(e_invalid_કિંમત_for_argument_str), "lnum");
	પાછા;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    જો (did_emsg > did_emsg_before)
	પાછા;
    જો (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_કિંમત_for_argument_str), "end_lnum");
	પાછા;
    }

    જો (elnum > buf->b_ml.ml_લીટી_ગણતરી)
	elnum = buf->b_ml.ml_લીટી_ગણતરી;

    પૂર્ણાંક		submatches = FALSE;
    જો (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	જો (d != ખાલી)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    જો (di != ખાલી)
	    {
		જો (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "submatches");
		    પાછા;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    જો (regmatch.regprog == ખાલી)
	જાઓ theend;
    regmatch.rm_ic = p_ic;

    જ્યારે (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	જો (get_matches_in_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    જાઓ cleanup;
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
    સ્થિર શૂન્ય
f_match(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_MATCH);
}

/*
 * "matchend()" function
 */
    સ્થિર શૂન્ય
f_matchend(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_END);
}

/*
 * "matchlist()" function
 */
    સ્થિર શૂન્ય
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_LIST);
}

/*
 * "matchstr()" function
 */
    સ્થિર શૂન્ય
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_STR);
}

/*
 * "matchstrlist()" function
 */
    સ્થિર શૂન્ય
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ખાલી;
    char_u	*save_cpo;
    list_T	*l = ખાલી;
    listitem_T	*li = ખાલી;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_સંખ્યા = -1;
    જો (rettv_list_alloc(rettv) != OK)
	પાછા;
    retlist = rettv->vval.v_list;

    જો (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	પાછા;

    જો ((l = argvars[0].vval.v_list) == ખાલી)
	પાછા;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    જો (pat == ખાલી)
	પાછા;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    જો (regmatch.regprog == ખાલી)
	જાઓ theend;
    regmatch.rm_ic = p_ic;

    પૂર્ણાંક		submatches = FALSE;
    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	જો (d != ખાલી)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    જો (di != ખાલી)
	    {
		જો (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_કિંમત_for_argument_str), "submatches");
		    જાઓ cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    પૂર્ણાંક idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	જો (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != ખાલી)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    જો (get_matches_in_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		જાઓ cleanup;
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
    સ્થિર શૂન્ય
f_matchstrpos(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_POS);
}

    સ્થિર શૂન્ય
max_min(typval_T *argvars, typval_T *rettv, પૂર્ણાંક domax)
{
    varnumber_T	n = 0;

    જો (in_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = 0;

    જો (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = ખાલી;

	l = argvars[0].vval.v_list;
	જો (l != ખાલી && l->lv_len > 0)
	{
	    જો (l->lv_first == &range_list_item)
	    {
		જો ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_start;
		નહીંતર
		    n = l->lv_u.nonmat.lv_start + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_સંખ્યા = n;
	    }
	    નહીંતર
	    {
		FOR_ALL_LIST_વસ્તુઓ(l, li)
		{
		    જો (tv == ખાલી)
			tv = &li->li_tv;
		    નહીંતર
		    {
			પૂર્ણાંક res;
			જો (typval_compare2(&li->li_tv, tv,
				domax ? પદાવલિ_GREATER : પદાવલિ_SMALLER, FALSE, &res) == FAIL)
			    પાછા;
			જો (res == OK)
			    tv = &li->li_tv;
		    }
		}

		જો (tv != ખાલી)
		    copy_tv(tv, rettv);
	    }
	}
    }
    નહીંતર જો (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	જો (tv != ખાલી)
	    copy_tv(tv, rettv);
    }
    નહીંતર જો (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	પૂર્ણાંક		todo;
	typval_T	*tv = ખાલી;

	d = argvars[0].vval.v_dict;
	જો (d != ખાલી)
	{
	    todo = (પૂર્ણાંક)d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_વસ્તુઓ(&d->dv_hashtab, hi, todo)
	    {
		જો (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    જો (tv == ખાલી)
			tv = &HI2DI(hi)->di_tv;
		    નહીંતર
		    {
			પૂર્ણાંક res;
			જો (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? પદાવલિ_GREATER : પદાવલિ_SMALLER, FALSE, &res) == FAIL)
			    પાછા;
			જો (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	જો (tv != ખાલી)
	    copy_tv(tv, rettv);
    }
    નહીંતર
	semsg(_(e_argument_of_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
}

/*
 * "max()" function
 */
    સ્થિર શૂન્ય
f_max(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, TRUE);
}

/*
 * "min()" function
 */
    સ્થિર શૂન્ય
f_min(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, FALSE);
}

#જો defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    સ્થિર શૂન્ય
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    શૂન્ય
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
    સ્થિર શૂન્ય
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    જો (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	પાછા;

    માટે (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	જો (lnum < 0 || lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	{
	    lnum = 0;
	    તોડો;
	}
	જો (*skipwhite(ml_get(lnum)) != NUL)
	    તોડો;
    }
    rettv->vval.v_સંખ્યા = lnum;
}


/*
 * "ngettext()" function
 */
    સ્થિર શૂન્ય
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#જો defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    અક્ષર *prev = ખાલી;
#endif

    જો (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_સંખ્યા_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	પાછા;

    rettv->v_type = VAR_STRING;

    જો (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != ખાલી &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#જો defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((અચલ અક્ષર *)argvars[3].vval.v_string, (અક્ષર *)p_enc);
#endif

#જો defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((અચલ અક્ષર *)argvars[3].vval.v_string, (અચલ અક્ષર *)argvars[0].vval.v_string, (અચલ અક્ષર *)argvars[1].vval.v_string, (પૂર્ણાંક)argvars[2].vval.v_સંખ્યા));
#નહીંતર
	textdomain((અચલ અક્ષર *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((અચલ અક્ષર *)argvars[0].vval.v_string, (અચલ અક્ષર *)argvars[1].vval.v_string, argvars[2].vval.v_સંખ્યા));
	textdomain(VIMPACKAGE);
#endif

#જો defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	જો (prev != ખાલી)
	    bind_textdomain_codeset((અચલ અક્ષર *)argvars[3].vval.v_string, prev);
#endif
    }
    નહીંતર
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((અચલ અક્ષર *)argvars[0].vval.v_string, (અચલ અક્ષર *)argvars[1].vval.v_string, argvars[2].vval.v_સંખ્યા));
}


/*
 * "nr2char()" function
 */
    સ્થિર શૂન્ય
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	પાછા;

    જો (has_mbyte)
    {
	પૂર્ણાંક	utf8 = 0;

	જો (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (પૂર્ણાંક)tv_get_bool_chk(&argvars[1], ખાલી);
	જો (utf8)
	    buf[utf_char2bytes((પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]), buf)] = NUL;
	નહીંતર
	    buf[(*mb_char2bytes)((પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]), buf)] = NUL;
    }
    નહીંતર
    {
	buf[0] = (char_u)tv_get_સંખ્યા(&argvars[0]);
	buf[1] = NUL;
    }
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "or(expr, expr)" function
 */
    સ્થિર શૂન્ય
f_or(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    rettv->vval.v_સંખ્યા = tv_get_સંખ્યા_chk(&argvars[0], ખાલી)
					| tv_get_સંખ્યા_chk(&argvars[1], ખાલી);
}

#ifdef FEAT_PERL
/*
 * "perleval()" function
 */
    સ્થિર શૂન્ય
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#endif

/*
 * "prevnonblank()" function
 */
    સ્થિર શૂન્ય
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    જો (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	પાછા;

    lnum = tv_get_lnum(argvars);
    જો (lnum < 1 || lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	lnum = 0;
    નહીંતર
	જ્યારે (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
	    --lnum;
    rettv->vval.v_સંખ્યા = lnum;
}

// This dummy va_list is here because:
// - passing a NULL pointer doesn't work when va_list isn't a pointer
// - locally in the function results in a "used before set" warning
// - using va_start() to initialize it gives "function with fixed args" error
સ્થિર va_list	ap;

/*
 * "printf()" function
 */
    સ્થિર શૂન્ય
f_printf(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    પૂર્ણાંક		len;
    char_u	*s;
    પૂર્ણાંક		saved_did_emsg = did_emsg;
    અક્ષર	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script() && check_for_string_or_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (અક્ષર *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(ખાલી, 0, fmt, ap, argvars + 1);
    જો (!did_emsg)
    {
	s = alloc(len + 1);
	જો (s != ખાલી)
	{
	    rettv->vval.v_string = s;
	    (શૂન્ય)vim_vsnprintf_typval((અક્ષર *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    સ્થિર શૂન્ય
f_pum_getpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;
    pum_set_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    સ્થિર શૂન્ય
f_pumvisible(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    જો (pum_visible())
	rettv->vval.v_સંખ્યા = 1;
}

#ifdef FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    સ્થિર શૂન્ય
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	પાછા;

    જો (p_pyx == 0)
	p_pyx = 3;

    જો (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    નહીંતર જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	પાછા;
    }
    નહીંતર
    {
	locals = ખાલી;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#endif

#ifdef FEAT_PYTHON
/*
 * "pyeval()" function
 */
    સ્થિર શૂન્ય
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	પાછા;

    જો (p_pyx == 0)
	p_pyx = 2;

    જો (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    નહીંતર જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	પાછા;
    }
    નહીંતર
    {
	locals = ખાલી;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#endif

#જો defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    સ્થિર શૂન્ય
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	પાછા;

# જો defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    જો (p_pyx == 2)
	f_pyeval(argvars, rettv);
    નહીંતર
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#endif

સ્થિર UINT32_T srand_seed_for_testing = 0;
સ્થિર પૂર્ણાંક	srand_seed_for_testing_is_used = FALSE;

    સ્થિર શૂન્ય
f_test_srand_seed(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type == VAR_UNKNOWN)
	srand_seed_for_testing_is_used = FALSE;
    નહીંતર
    {
	srand_seed_for_testing = (UINT32_T)tv_get_સંખ્યા(&argvars[0]);
	srand_seed_for_testing_is_used = TRUE;
    }
}

    સ્થિર શૂન્ય
init_srand(UINT32_T *x)
{
    સંરચના {
	સંઘ {
	    UINT32_T સંખ્યા;
	    char_u   bytes[કદ(UINT32_T)];
	} contents;
    } buf;

    જો (srand_seed_for_testing_is_used)
    {
	*x = srand_seed_for_testing;
	પાછા;
    }

    જો (mch_get_random(buf.contents.bytes, કદ(buf.contents.bytes)) == OK)
    {
	*x = buf.contents.સંખ્યા;
	પાછા;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#જો defined(FEAT_SODIUM)
    જો (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    નહીંતર
#endif
    {
#જો defined(FEAT_RELTIME)
	proftime_T res;
	profile_start(&res);
# જો defined(MSWIN)
	*x = (UINT32_T)res.નીચલું_Part;
# નહીંતર
	*x = (UINT32_T)res.tv_fsec;
# endif
#નહીંતર
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
    સ્થિર શૂન્ય
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = ખાલી;
    સ્થિર UINT32_T	gx, gy, gz, gw;
    સ્થિર પૂર્ણાંક	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, result;

    જો (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type == VAR_UNKNOWN)
    {
	// When no argument is given use the global seed list.
	જો (initialized == FALSE)
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
    નહીંતર જો (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	જો (l == ખાલી || list_len(l) != 4)
	    જાઓ theend;

	lx = list_find(l, 0L);
	ly = list_find(l, 1L);
	lz = list_find(l, 2L);
	lw = list_find(l, 3L);
	જો (lx == ખાલી || ly == ખાલી || lz == ખાલી || lw == ખાલી)
	    જાઓ theend;
	જો (lx->li_tv.v_type != VAR_સંખ્યા
		|| ly->li_tv.v_type != VAR_સંખ્યા
		|| lz->li_tv.v_type != VAR_સંખ્યા
		|| lw->li_tv.v_type != VAR_સંખ્યા)
	    જાઓ theend;
	x = (UINT32_T)lx->li_tv.vval.v_સંખ્યા;
	y = (UINT32_T)ly->li_tv.vval.v_સંખ્યા;
	z = (UINT32_T)lz->li_tv.vval.v_સંખ્યા;
	w = (UINT32_T)lw->li_tv.vval.v_સંખ્યા;

	SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w);

	lx->li_tv.vval.v_સંખ્યા = (varnumber_T)x;
	ly->li_tv.vval.v_સંખ્યા = (varnumber_T)y;
	lz->li_tv.vval.v_સંખ્યા = (varnumber_T)z;
	lw->li_tv.vval.v_સંખ્યા = (varnumber_T)w;
    }
    નહીંતર
	જાઓ theend;

    rettv->v_type = VAR_સંખ્યા;
    rettv->vval.v_સંખ્યા = (varnumber_T)result;
    પાછા;

theend:
    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    rettv->v_type = VAR_સંખ્યા;
    rettv->vval.v_સંખ્યા = -1;
}

/*
 * "srand()" function
 */
    સ્થિર શૂન્ય
f_srand(typval_T *argvars, typval_T *rettv)
{
    UINT32_T x = 0, z;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    નહીંતર
    {
	પૂર્ણાંક	    error = FALSE;

	x = (UINT32_T)tv_get_સંખ્યા_chk(&argvars[0], &error);
	જો (error)
	    પાછા;
    }

    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#undef ROTL
#undef SPLITMIX32
#undef SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    સ્થિર શૂન્ય
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	start;
    varnumber_T	end;
    varnumber_T	stride = 1;
    પૂર્ણાંક		error = FALSE;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL)))
	પાછા;

    start = tv_get_સંખ્યા_chk(&argvars[0], &error);
    જો (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = start - 1;
	start = 0;
    }
    નહીંતર
    {
	end = tv_get_સંખ્યા_chk(&argvars[1], &error);
	જો (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_સંખ્યા_chk(&argvars[2], &error);
    }

    જો (error)
	પાછા;		// type error; errmsg already given
    જો (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	પાછા;
    }

    // The stride is stored in "lv_stride", which is an int.
    જો (stride < INT_MIN || stride > INT_MAX)
    {
	અક્ષર	buf[NUMBUFLEN];

	vim_snprintf(buf, કદ(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	પાછા;
    }

    uvarnumber_T	len;

    જો (stride > 0 ? end < start : end > start)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	back = stride > 0
				  ? (uvarnumber_T)start - (uvarnumber_T)end
				  : (uvarnumber_T)end - (uvarnumber_T)start;

	જો (back > 1)
	{
	    emsg(_(e_start_past_end));
	    પાછા;
	}
	len = 0;
    }
    નહીંતર
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	span = stride > 0
				  ? (uvarnumber_T)end - (uvarnumber_T)start
				  : (uvarnumber_T)start - (uvarnumber_T)end;
	uvarnumber_T	ગણતરી = span / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	જો (ગણતરી >= (uvarnumber_T)INT_MAX)
	{
	    અક્ષર	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, કદ(buf), "%llu",
			      ગણતરી < UVARNUM_MAX ? ગણતરી + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    પાછા;
	}
	len = ગણતરી + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_first = &range_list_item;
    list->lv_u.nonmat.lv_start = start;
    list->lv_u.nonmat.lv_end = end;
    list->lv_u.nonmat.lv_stride = (પૂર્ણાંક)stride;
    list->lv_len = (પૂર્ણાંક)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    શૂન્ય
range_list_materialize(list_T *list)
{
    varnumber_T start = list->lv_u.nonmat.lv_start;
    varnumber_T end = list->lv_u.nonmat.lv_end;
    પૂર્ણાંક		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_first = ખાલી;
    list->lv_u.mat.lv_last = ખાલી;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_item = ખાલી;
    માટે (i = start; stride > 0 ? i <= end : i >= end; i += stride)
    {
	જો (list_append_સંખ્યા(list, i) == FAIL)
	    તોડો;
	જો (list->lv_lock & VAR_વસ્તુઓ_LOCKED)
	    list->lv_u.mat.lv_last->li_tv.v_lock = VAR_LOCKED;
    }
    list->lv_lock &= ~VAR_વસ્તુઓ_LOCKED;
}

/*
 * "getreginfo()" function
 */
    સ્થિર શૂન્ય
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		regname;
    char_u	buf[NUMBUFLEN + 2];
    size_t	buflen;
    long	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    જો (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	પાછા;

    regname = getreg_get_regname(argvars);
    જો (regname == 0)
	પાછા;

    જો (regname == '@')
	regname = '"';

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_પદાવલિ_SRC | GREG_LIST);
    જો (list == ખાલી)
	પાછા;
    જો (dict_add_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    પસંદગી (get_reg_type(regname, &reglen))
    {
	સ્થિતિ MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    તોડો;
	સ્થિતિ MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    તોડો;
	સ્થિતિ MBLOCK:
	    buflen = vim_snprintf_safelen((અક્ષર *)buf, કદ(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    તોડો;
	મૂળભૂત:
	    buf[0] = NUL;
	    buflen = 0;
	    તોડો;
    }
    (શૂન્ય)dict_add_string_len(dict, (અક્ષર *)"regtype", buf, (પૂર્ણાંક)buflen);

    buf[0] = get_register_name(get_unname_register());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    જો (regname == '"')
	(શૂન્ય)dict_add_string_len(dict, (અક્ષર *)"points_to", buf, (પૂર્ણાંક)buflen);
    નહીંતર
    {
	dictitem_T	*item = dictitem_alloc((char_u *)"isunnamed");

	જો (item != ખાલી)
	{
	    item->di_tv.v_type = VAR_BOOL;
	    item->di_tv.vval.v_સંખ્યા = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (શૂન્ય)dict_add(dict, item);
	}
    }
}

    સ્થિર શૂન્ય
return_register(પૂર્ણાંક regname, typval_T *rettv)
{
    char_u buf[2] = {NUL, NUL};

    buf[0] = (char_u)regname;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(buf, (buf[0] == NUL) ? 0 : 1);
}

/*
 * "reg_executing()" function
 */
    સ્થિર શૂન્ય
f_reg_executing(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    સ્થિર શૂન્ય
f_reg_recording(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    સ્થિર શૂન્ય
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_સંખ્યા = -1;
    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    rettv->vval.v_સંખ્યા = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    સ્થિર શૂન્ય
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    પૂર્ણાંક slen = list_len(l);

    જો (rettv_list_alloc(rettv) == FAIL
	    || l == ખાલી
	    || n <= 0
	    || slen == 0)
	પાછા;

    જો (check_repeat_ગણતરી(slen, n) == FAIL)
	પાછા;

    જ્યારે (n-- > 0)
	જો (list_extend(rettv->vval.v_list, l, ખાલી) == FAIL)
	    તોડો;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    સ્થિર શૂન્ય
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    પૂર્ણાંક		slen;
    પૂર્ણાંક		len;
    પૂર્ણાંક		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    જો (rettv_blob_alloc(rettv) == FAIL
	    || blob == ખાલી
	    || n <= 0)
	પાછા;

    slen = blob->bv_ga.ga_len;
    જો (slen <= 0)
	પાછા;
    જો (check_repeat_ગણતરી(slen, n) == FAIL)
	પાછા;
    len = slen * (પૂર્ણાંક)n;

    જો (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	પાછા;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    માટે (i = 0; i < slen; ++i)
	જો (blob_get(blob, i) != 0)
	    તોડો;

    જો (i == slen)
	// No need to copy since all bytes are already zero
	પાછા;

    માટે (i = 0; i < (પૂર્ણાંક)n; ++i)
	blob_set_range(rettv->vval.v_blob,
		(long)i * slen, ((long)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    સ્થિર શૂન્ય
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    size_t	slen;
    પૂર્ણાંક		len;
    char_u	*r;
    પૂર્ણાંક		done;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    slen = STRLEN(p);
    જો (slen == 0 || n <= 0)
	પાછા;
    જો (check_repeat_ગણતરી((varnumber_T)slen, n) == FAIL)
	પાછા;
    len = (પૂર્ણાંક)slen * (પૂર્ણાંક)n;

    r = alloc((size_t)len + 1);
    જો (r == ખાલી)
	પાછા;

    mch_memmove(r, p, slen);
    done = (પૂર્ણાંક)slen;
    જ્યારે (done < len)
    {
	પૂર્ણાંક copy_len = done;

	જો (copy_len > len - done)
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
    સ્થિર શૂન્ય
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    જો (in_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    n = tv_get_સંખ્યા(&argvars[1]);
    જો (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    નહીંતર
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
    સ્થિર પૂર્ણાંક
get_શોધ_arg(typval_T *varp, પૂર્ણાંક *flagsp)
{
    પૂર્ણાંક		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    પૂર્ણાંક		mask;

    જો (varp->v_type == VAR_UNKNOWN)
	પાછા FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    જો (flags == ખાલી)
	પાછા 0;		// type error; errmsg already given
    જ્યારે (*flags != NUL)
    {
	પસંદગી (*flags)
	{
	    સ્થિતિ 'b': dir = BACKWARD; તોડો;
	    સ્થિતિ 'w': p_ws = TRUE; તોડો;
	    સ્થિતિ 'W': p_ws = FALSE; તોડો;
	    મૂળભૂત:  mask = 0;
		      જો (flagsp != ખાલી)
			  પસંદગી (*flags)
			  {
			      સ્થિતિ 'c': mask = SP_START; તોડો;
			      સ્થિતિ 'e': mask = SP_END; તોડો;
			      સ્થિતિ 'm': mask = SP_RETCOUNT; તોડો;
			      સ્થિતિ 'n': mask = SP_NOMOVE; તોડો;
			      સ્થિતિ 'p': mask = SP_SUBPAT; તોડો;
			      સ્થિતિ 'r': mask = SP_REPEAT; તોડો;
			      સ્થિતિ 's': mask = SP_SETPCMARK; તોડો;
			      સ્થિતિ 'z': mask = SP_COLUMN; તોડો;
			  }
		      જો (mask == 0)
		      {
			  semsg(_(e_invalid_argument_str), flags);
			  dir = 0;
		      }
		      નહીંતર
			  *flagsp |= mask;
	}
	જો (dir == 0)
	    તોડો;
	++flags;
    }
    પાછા dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    સ્થિર પૂર્ણાંક
શોધ_cmn(typval_T *argvars, pos_T *match_pos, પૂર્ણાંક *flagsp)
{
    પૂર્ણાંક		flags;
    char_u	*pat;
    size_t	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    પૂર્ણાંક		save_p_ws = p_ws;
    પૂર્ણાંક		dir;
    પૂર્ણાંક		retval = 0;	// default: FAIL
    long	lnum_stop = 0;
#ifdef FEAT_RELTIME
    long	time_limit = 0;
#endif
    પૂર્ણાંક		options = શોધ_KEEP;
    પૂર્ણાંક		subpatnum;
    searchit_arg_T sia;
    પૂર્ણાંક		use_skip = FALSE;
    pos_T	firstpos;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_સંખ્યા_arg(argvars, 3) == FAIL)))))
	જાઓ theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_શોધ_arg(&argvars[1], flagsp);	// may set p_ws
    જો (dir == 0)
	જાઓ theend;
    flags = *flagsp;
    જો (flags & SP_START)
	options |= શોધ_START;
    જો (flags & SP_END)
	options |= શોધ_END;
    જો (flags & SP_COLUMN)
	options |= શોધ_COL;

    // Optional arguments: line number to stop searching, timeout and skip.
    જો (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (long)tv_get_સંખ્યા_chk(&argvars[2], ખાલી);
	જો (lnum_stop < 0)
	    જાઓ theend;
	જો (argvars[3].v_type != VAR_UNKNOWN)
	{
#ifdef FEAT_RELTIME
	    time_limit = (long)tv_get_સંખ્યા_chk(&argvars[3], ખાલી);
	    જો (time_limit < 0)
		જાઓ theend;
#endif
	    use_skip = eval_પદાવલિ_valid_arg(&argvars[4]);
	}
    }

    /*
     * This function does not accept SP_REPEAT and SP_RETCOUNT flags.
     * Check to make sure only those flags are set.
     * Also, Only the SP_NOMOVE or the SP_SETPCMARK flag can be set. Both
     * flags cannot be set. Check for that condition also.
     */
    જો (((flags & (SP_REPEAT | SP_RETCOUNT)) != 0)
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	જાઓ theend;
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
    માટે (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, ખાલી, dir, pat, patlen, 1L,
						     options, RE_શોધ, &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	જો (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	જો (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    તોડો;
	જો (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    પૂર્ણાંક	    do_skip;
	    પૂર્ણાંક	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_પદાવલિ_to_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    જો (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		તોડો;
	    }
	    જો (!do_skip)
		તોડો;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~શોધ_START;
    }

    જો (subpatnum != FAIL)
    {
	જો (flags & SP_SUBPAT)
	    retval = subpatnum;
	નહીંતર
	    retval = pos.lnum;
	જો (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	જો (match_pos != ખાલી)
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
    જો (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    નહીંતર
	curwin->w_set_curswant = true;
theend:
    p_ws = save_p_ws;

    પાછા retval;
}

#ifdef FEAT_RUBY
/*
 * "rubyeval()" function
 */
    સ્થિર શૂન્ય
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#endif

/*
 * "screenattr()" function
 */
    સ્થિર શૂન્ય
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		row;
    પૂર્ણાંક		col;
    પૂર્ણાંક		c;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    row = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[0], ખાલી) - 1;
    col = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[1], ખાલી) - 1;
    જો (row < 0 || row >= screen_Rows
	    || col < 0 || col >= screen_Columns)
	c = -1;
    નહીંતર
	c = ScreenAttrs[લીટી_Offset[row] + col];
    rettv->vval.v_સંખ્યા = c;
}

/*
 * "screenchar()" function
 */
    સ્થિર શૂન્ય
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		row;
    પૂર્ણાંક		col;
    પૂર્ણાંક		c;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    row = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[0], ખાલી) - 1;
    col = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[1], ખાલી) - 1;
    જો (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	c = -1;
    નહીંતર
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(row, col, buf, ખાલી);
	c = (*mb_ptr2char)(buf);
    }
    rettv->vval.v_સંખ્યા = c;
}

/*
 * "screenchars()" function
 */
    સ્થિર શૂન્ય
f_screenchars(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		row;
    પૂર્ણાંક		col;
    પૂર્ણાંક		c;
    પૂર્ણાંક		i;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    row = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[0], ખાલી) - 1;
    col = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[1], ખાલી) - 1;
    જો (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	પાછા;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(row, col, buf, ખાલી);
    પૂર્ણાંક pcc[MAX_MCO];
    જો (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    નહીંતર
	c = (*mb_ptr2char)(buf);
    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)c);

    જો (enc_utf8)
	માટે (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    સ્થિર શૂન્ય
f_screencol(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    સ્થિર શૂન્ય
f_screenrow(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    સ્થિર શૂન્ય
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		row;
    પૂર્ણાંક		col;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = ખાલી;
    rettv->v_type = VAR_STRING;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    row = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[0], ખાલી) - 1;
    col = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[1], ખાલી) - 1;
    જો (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	પાછા;

    screen_getbytes(row, col, buf, ખાલી);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    સ્થિર શૂન્ય
f_શોધ(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		flags = 0;

    rettv->vval.v_સંખ્યા = શોધ_cmn(argvars, ખાલી, &flags);
}

/*
 * "searchdecl()" function
 */
    સ્થિર શૂન્ય
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		locally = TRUE;
    પૂર્ણાંક		thisblock = FALSE;
    પૂર્ણાંક		error = FALSE;
    char_u	*name;

    rettv->vval.v_સંખ્યા = 1;	// default: FAIL

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	પાછા;

    name = tv_get_string_chk(&argvars[0]);
    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(પૂર્ણાંક)tv_get_bool_chk(&argvars[1], &error);
	જો (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (પૂર્ણાંક)tv_get_bool_chk(&argvars[2], &error);
    }
    જો (!error && name != ખાલી)
	rettv->vval.v_સંખ્યા = find_decl(name, (પૂર્ણાંક)STRLEN(name),
				     locally, thisblock, શોધ_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    સ્થિર પૂર્ણાંક
searchpair_cmn(typval_T *argvars, pos_T *match_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    પૂર્ણાંક		save_p_ws = p_ws;
    પૂર્ણાંક		dir;
    પૂર્ણાંક		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    પૂર્ણાંક		retval = 0;		// default: FAIL
    long	lnum_stop = 0;
    long	time_limit = 0;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_સંખ્યા_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_સંખ્યા_arg(argvars, 6) == FAIL)))))
	જાઓ theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    જો (spat == ખાલી || mpat == ખાલી || epat == ખાલી)
	જાઓ theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_શોધ_arg(&argvars[3], &flags); // may set p_ws
    જો (dir == 0)
	જાઓ theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    જો ((flags & (SP_END | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[3]));
	જાઓ theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    જો (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    જો (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = ખાલી;
    નહીંતર
    {
	// Type is checked later.
	skip = &argvars[4];

	જો (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (long)tv_get_સંખ્યા_chk(&argvars[5], ખાલી);
	    જો (lnum_stop < 0)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[5]));
		જાઓ theend;
	    }
#ifdef FEAT_RELTIME
	    જો (argvars[6].v_type != VAR_UNKNOWN)
	    {
		time_limit = (long)tv_get_સંખ્યા_chk(&argvars[6], ખાલી);
		જો (time_limit < 0)
		{
		    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[6]));
		    જાઓ theend;
		}
	    }
#endif
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    match_pos, lnum_stop, time_limit);

theend:
    p_ws = save_p_ws;

    પાછા retval;
}

/*
 * "searchpair()" function
 */
    સ્થિર શૂન્ય
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = searchpair_cmn(argvars, ખાલી);
}

/*
 * "searchpairpos()" function
 */
    સ્થિર શૂન્ય
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    પૂર્ણાંક		lnum = 0;
    પૂર્ણાંક		col = 0;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (searchpair_cmn(argvars, &match_pos) > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)col);
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
    પૂર્ણાંક		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    પૂર્ણાંક		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*match_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    long	time_limit UNUSED) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = ખાલી, *pat3 = ખાલી;
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
    પૂર્ણાંક		n;
    પૂર્ણાંક		r;
    પૂર્ણાંક		nest = 1;
    પૂર્ણાંક		use_skip = FALSE;
    પૂર્ણાંક		err;
    પૂર્ણાંક		options = શોધ_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    જો (pat2 == ખાલી)
	જાઓ theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    જો (pat3 == ખાલી)
	જાઓ theend;
    pat2len = vim_snprintf((અક્ષર *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    જો (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    નહીંતર
	pat3len = vim_snprintf((અક્ષર *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    જો (flags & SP_START)
	options |= શોધ_START;

    જો (skip != ખાલી)
	use_skip = eval_પદાવલિ_valid_arg(skip);

#ifdef FEAT_RELTIME
    જો (time_limit > 0)
	init_regexp_timeout(time_limit);
#endif
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    માટે (;;)
    {
	searchit_arg_T sia;

	CLEAR_FIELD(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, ખાલી, dir, pat, patlen, 1L,
						     options, RE_શોધ, &sia);
	જો (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    તોડો;

	જો (firstpos.lnum == 0)
	    firstpos = pos;
	જો (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    જો (dir == BACKWARD)
		decl(&pos);
	    નહીંતર
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~શોધ_START;

	// If the skip pattern matches, ignore this match.
	જો (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_પદાવલિ_to_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    જો (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		તોડો;
	    }
	    જો (r)
		ચાલુ;
	}

	જો ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	નહીંતર
	{
	    // Found end when searching forward or start when searching
	    // backward: end of (nested) pair; or found middle in outer pair.
	    જો (--nest == 1)
		pat = pat3;	// outer level, search for middle
	}

	જો (nest == 0)
	{
	    // Found the match: return matchcount or line number.
	    જો (flags & SP_RETCOUNT)
		++retval;
	    નહીંતર
		retval = pos.lnum;
	    જો (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    જો (!(flags & SP_REPEAT))
		તોડો;
	    nest = 1;	    // search for next unmatched
	}
    }

    જો (match_pos != ખાલી)
    {
	// Store the match cursor position
	match_pos->lnum = curwin->w_cursor.lnum;
	match_pos->col = curwin->w_cursor.col + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    જો ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#ifdef FEAT_RELTIME
    જો (time_limit > 0)
	disable_regexp_timeout();
#endif
    vim_free(pat2);
    vim_free(pat3);
    જો (p_cpo == empty_option)
	p_cpo = save_cpo;
    નહીંતર
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	જો (*p_cpo == NUL)
	    set_option_કિંમત_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    પાછા retval;
}

/*
 * "searchpos()" function
 */
    સ્થિર શૂન્ય
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    પૂર્ણાંક		lnum = 0;
    પૂર્ણાંક		col = 0;
    પૂર્ણાંક		n;
    પૂર્ણાંક		flags = 0;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    n = શોધ_cmn(argvars, &match_pos, &flags);
    જો (n > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)col);
    જો (flags & SP_SUBPAT)
	list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    સ્થિર શૂન્ય
set_position(typval_T *argvars, typval_T *rettv, પૂર્ણાંક charpos)
{
    pos_T	pos;
    પૂર્ણાંક		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	પાછા;

    name = tv_get_string_chk(argvars);
    જો (name == ખાલી)
	પાછા;

    જો (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	પાછા;

    જો (pos.col != MAXCOL && --pos.col < 0)
	pos.col = 0;
    જો ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	જો (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_set_curswant = false;
	}
	check_cursor();
	rettv->vval.v_સંખ્યા = 0;
    }
    નહીંતર જો (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	જો (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_સંખ્યા = 0;
    }
    નહીંતર
	emsg(_(e_invalid_argument));
}
/*
 * "setcharpos()" function
 */
    સ્થિર શૂન્ય
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, TRUE);
}

    સ્થિર શૂન્ય
f_setcharsearch(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    જો (check_for_dict_arg(argvars, 0) == FAIL)
	પાછા;

    જો ((d = argvars[0].vval.v_dict) == ખાલી)
	પાછા;

    csearch = dict_get_string(d, "char", FALSE);
    જો (csearch != ખાલી)
    {
	જો (enc_utf8)
	{
	    પૂર્ણાંક pcc[MAX_MCO];
	    પૂર્ણાંક c = utfc_ptr2char(csearch, pcc);

	    set_last_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	નહીંતર
	    set_last_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_find(d, (char_u *)"forward", -1);
    જો (di != ખાલી)
	set_csearch_direction((પૂર્ણાંક)tv_get_સંખ્યા(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_find(d, (char_u *)"until", -1);
    જો (di != ખાલી)
	set_csearch_until(!!tv_get_સંખ્યા(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    સ્થિર શૂન્ય
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    સ્થિર શૂન્ય
f_setenv(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    જો (check_restricted() || check_secure())
	પાછા;

    name = tv_get_string_buf(&argvars[0], namebuf);
    જો (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_સંખ્યા == VVAL_NULL)
	vim_unsetenv_ext(name);
    નહીંતર
	vim_setenv_ext(name, tv_get_string_buf(&argvars[1], valbuf));
}

/*
 * "setfperm({fname}, {mode})" function
 */
    સ્થિર શૂન્ય
f_setfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	modebuf[NUMBUFLEN];
    char_u	*mode_str;
    પૂર્ણાંક		i;
    પૂર્ણાંક		mask;
    પૂર્ણાંક		mode = 0;

    rettv->vval.v_સંખ્યા = 0;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    fname = tv_get_string_chk(&argvars[0]);
    જો (fname == ખાલી)
	પાછા;
    mode_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    જો (mode_str == ખાલી)
	પાછા;
    જો (STRLEN(mode_str) != 9)
    {
	semsg(_(e_invalid_argument_str), mode_str);
	પાછા;
    }

    mask = 1;
    માટે (i = 8; i >= 0; --i)
    {
	જો (mode_str[i] != '-')
	    mode |= mask;
	mask = mask << 1;
    }
    rettv->vval.v_સંખ્યા = mch_setperm(fname, mode) == OK;
}

/*
 * "setpos()" function
 */
    સ્થિર શૂન્ય
f_setpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    પૂર્ણાંક
get_yank_type(char_u **pp, char_u *yank_type, long *block_len)
{
    char_u *stropt = *pp;
    પસંદગી (*stropt)
    {
	સ્થિતિ 'v': સ્થિતિ 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    તોડો;
	સ્થિતિ 'V': સ્થિતિ 'l':	// line-wise selection
	    *yank_type = MLINE;
	    તોડો;
	સ્થિતિ 'b': સ્થિતિ Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    જો (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*block_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    તોડો;
	મૂળભૂત:
	    પાછા FAIL;
    }
    *pp = stropt;
    પાછા OK;
}

/*
 * "setreg()" function
 */
    સ્થિર શૂન્ય
f_setreg(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    પૂર્ણાંક		append;
    char_u	yank_type;
    long	block_len;
    typval_T	*regcontents;
    પૂર્ણાંક		pointreg;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	પાછા;

    pointreg = 0;
    regcontents = ખાલી;
    block_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_સંખ્યા = 1;		// FAIL is default

    જો (strregname == ખાલી)
	પાછા;		// type error; errmsg already given
    જો (in_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_register_name_must_be_one_char_str), strregname);
	પાછા;
    }
    regname = *strregname;
    જો (regname == 0 || regname == '@')
	regname = '"';

    જો (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	જો (d == ખાલી || d->dv_hashtab.ht_used == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {ખાલી, ખાલી};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    પાછા;
	}

	di = dict_find(d, (char_u *)"regcontents", -1);
	જો (di != ખાલી)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	જો (stropt != ખાલી)
	{
	    પૂર્ણાંક ret = get_yank_type(&stropt, &yank_type, &block_len);

	    જો (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_કિંમત_for_argument_str), "value");
		પાછા;
	    }
	}

	જો (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    જો (stropt != ખાલી)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	નહીંતર જો (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    નહીંતર
	regcontents = &argvars[1];

    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	જો (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    પાછા;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	જો (stropt == ખાલી)
	    પાછા;		// type error
	માટે (; *stropt != NUL; ++stropt)
	    પસંદગી (*stropt)
	    {
		સ્થિતિ 'a': સ્થિતિ 'A':	// append
		    append = TRUE;
		    તોડો;
		મૂળભૂત:
		    get_yank_type(&stropt, &yank_type, &block_len);
	    }
    }

    જો (regcontents && regcontents->v_type == VAR_LIST)
    {
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	list_T		*ll = regcontents->vval.v_list;
	listitem_T	*li;
	પૂર્ણાંક		len;

	// If the list is NULL handle like an empty list.
	len = ll == ખાલી ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	જો (lstval == ખાલી)
	    પાછા;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	જો (ll != ખાલી)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_વસ્તુઓ(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		જો (strval == ખાલી)
		    જાઓ free_lstval;
		જો (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    જો (strval == ખાલી)
			જાઓ free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ખાલી;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, block_len);
free_lstval:
	જ્યારે (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    નહીંતર જો (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	જો (strval == ખાલી)
	    પાછા;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, block_len);
    }
    જો (pointreg != 0)
	get_yank_register(pointreg, TRUE);

    rettv->vval.v_સંખ્યા = 0;
}

/*
 * "settagstack()" function
 */
    સ્થિર શૂન્ય
f_settagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    dict_T	*d;
    પૂર્ણાંક		action = 'r';

    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	પાછા;

    // first argument: window number or id
    wp = find_win_by_nr_or_id(&argvars[0]);
    જો (wp == ખાલી)
	પાછા;

    // second argument: dict with items to set in the tag stack
    જો (check_for_dict_arg(argvars, 1) == FAIL)
	પાછા;
    d = argvars[1].vval.v_dict;
    જો (d == ખાલી)
	પાછા;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    જો (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    નહીંતર જો (check_for_string_arg(argvars, 2) == FAIL)
	પાછા;
    નહીંતર
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	જો (actstr == ખાલી)
	    પાછા;
	જો ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	નહીંતર
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    પાછા;
	}
    }

    જો (set_tagstack(wp, d, action) == OK)
	rettv->vval.v_સંખ્યા = 0;
}

#ifdef FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    સ્થિર શૂન્ય
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    પૂર્ણાંક		len;

    જો (in_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != ખાલી ? (char_u *)blob->bv_ga.ga_data : (char_u *)"";
	len = blob != ખાલી ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_bytes(p, len, ખાલી, 0));
    }
    નહીંતર
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_bytes(p, (પૂર્ણાંક)STRLEN(p), ખાલી, 0));
    }
}
#endif // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    સ્થિર શૂન્ય
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક do_special;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	પાછા;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    સ્થિર શૂન્ય
f_shiftwidth(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = 0;

    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	long	col;

	col = (long)tv_get_સંખ્યા_chk(argvars, ખાલી);
	જો (col < 0)
	    પાછા;	// type error; errmsg already given
#ifdef FEAT_VARTABS
	rettv->vval.v_સંખ્યા = get_sw_કિંમત_col(curbuf, col, FALSE);
	પાછા;
#endif
    }

    rettv->vval.v_સંખ્યા = get_sw_કિંમત(curbuf);
}

/*
 * "soundfold({word})" function
 */
    સ્થિર શૂન્ય
f_soundfold(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#ifdef FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#નહીંતર
    rettv->vval.v_string = vim_strsave(s);
#endif
}

/*
 * "spellbadword()" function
 */
    સ્થિર શૂન્ય
f_spellbadword(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*word = (char_u *)"";
    hlf_T	attr = HLF_ગણતરી;
    પૂર્ણાંક		len = 0;
#ifdef FEAT_SPELL
    પૂર્ણાંક		wo_spell_save = curwin->w_p_spell;

    જો (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	પાછા;

    જો (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    જો (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	પાછા;
    }
#endif

    જો (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	પાછા;
    }

#ifdef FEAT_SPELL
    જો (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_to(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	જો (len != 0)
	{
	    word = ml_get_cursor();
	    curwin->w_set_curswant = true;
	}
    }
    નહીંતર જો (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	પૂર્ણાંક	capcol = -1;

	જો (str != ખાલી)
	{
	    // Check the argument for spelling.
	    જ્યારે (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		જો (attr != HLF_ગણતરી)
		{
		    word = str;
		    તોડો;
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
    પસંદગી (attr)
    {
    સ્થિતિ HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	તોડો;

    સ્થિતિ HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	તોડો;

    સ્થિતિ HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	તોડો;

    સ્થિતિ HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	તોડો;

    મૂળભૂત:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	તોડો;
    }
}

/*
 * "spellsuggest()" function
 */
    સ્થિર શૂન્ય
f_spellsuggest(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SPELL
    char_u	*str;
    પૂર્ણાંક		typeerr = FALSE;
    પૂર્ણાંક		maxcount;
    garray_T	ga;
    પૂર્ણાંક		i;
    listitem_T	*li;
    પૂર્ણાંક		need_capital = FALSE;
    પૂર્ણાંક		wo_spell_save = curwin->w_p_spell;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	પાછા;

    જો (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    જો (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	પાછા;
    }
#endif

    જો (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	પાછા;
    }

#ifdef FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[1], &typeerr);
	જો (maxcount <= 0)
	    પાછા;
	જો (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (પૂર્ણાંક)tv_get_bool_chk(&argvars[2], &typeerr);
	    જો (typeerr)
		પાછા;
	}
    }
    નહીંતર
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    માટે (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_data)[i];

	li = listitem_alloc();
	જો (li == ખાલી)
	    vim_free(str);
	નહીંતર
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

    સ્થિર શૂન્ય
f_split(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*end;
    char_u	*pat = ખાલી;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    પૂર્ણાંક		match;
    colnr_T	col = 0;
    પૂર્ણાંક		keepempty = FALSE;
    પૂર્ણાંક		typeerr = FALSE;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	પાછા;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    str = tv_get_string(&argvars[0]);
    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	જો (pat == ખાલી)
	    typeerr = TRUE;
	જો (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (પૂર્ણાંક)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    જો (pat == ખાલી || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    જો (rettv_list_alloc(rettv) == FAIL)
	જાઓ theend;
    જો (typeerr)
	જાઓ theend;

    regmatch.regprog = eval_regcomp(pat);
    જો (regmatch.regprog != ખાલી)
    {
	regmatch.rm_ic = FALSE;
	જ્યારે (*str != NUL || keepempty)
	{
	    જો (*str == NUL)
		match = FALSE;	// empty item at the end
	    નહીંતર
		match = vim_regexec_nl(&regmatch, str, col);
	    જો (match)
		end = regmatch.startp[0];
	    નહીંતર
		end = str + STRLEN(str);
	    જો (keepempty || end > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && match && end < regmatch.endp[0]))
	    {
		જો (list_append_string(rettv->vval.v_list, str,
						    (પૂર્ણાંક)(end - str)) == FAIL)
		    તોડો;
	    }
	    જો (!match)
		તોડો;
	    // Advance to just after the match.
	    જો (regmatch.endp[0] > str)
		col = 0;
	    નહીંતર
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
    સ્થિર શૂન્ય
f_submatch(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		error = FALSE;
    પૂર્ણાંક		no;
    પૂર્ણાંક		retList = 0;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	પાછા;

    no = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[0], &error);
    જો (error)
	પાછા;
    જો (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_સંખ્યા_nr), no);
	પાછા;
    }
    જો (argvars[1].v_type != VAR_UNKNOWN)
	retList = (પૂર્ણાંક)tv_get_bool_chk(&argvars[1], &error);
    જો (error)
	પાછા;

    જો (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    નહીંતર
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = reg_submatch_list(no);
    }
}

/*
 * "substitute()" function
 */
    સ્થિર શૂન્ય
f_substitute(typval_T *argvars, typval_T *rettv)
{
    char_u	patbuf[NUMBUFLEN];
    char_u	subbuf[NUMBUFLEN];
    char_u	flagsbuf[NUMBUFLEN];
    char_u	*str;
    char_u	*pat;
    char_u	*sub = ખાલી;
    typval_T	*પદાવલિ = ખાલી;
    char_u	*flg;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	પાછા;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    જો (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	પદાવલિ = &argvars[2];
    નહીંતર
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    જો (str == ખાલી || pat == ખાલી || (sub == ખાલી && પદાવલિ == ખાલી)
								|| flg == ખાલી)
	rettv->vval.v_string = ખાલી;
    નહીંતર
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, પદાવલિ, flg, ખાલી);
}

/*
 * "swapfilelist()" function
 */
    સ્થિર શૂન્ય
f_swapfilelist(typval_T *argvars UNUSED, typval_T *rettv)
{
    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;
    recover_names(ખાલી, FALSE, rettv->vval.v_list, 0, ખાલી);
}

/*
 * "swapinfo(swap_filename)" function
 */
    સ્થિર શૂન્ય
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    જો (rettv_dict_alloc(rettv) == OK)
	get_b0_dict(tv_get_string(argvars), rettv->vval.v_dict);
}

/*
 * "swapname(expr)" function
 */
    સ્થિર શૂન્ય
f_swapname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = tv_get_buf(&argvars[0], FALSE);
    જો (buf == ખાલી || buf->b_ml.ml_mfp == ખાલી
					|| buf->b_ml.ml_mfp->mf_fname == ખાલી)
	rettv->vval.v_string = ખાલી;
    નહીંતર
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    સ્થિર શૂન્ય
f_synID(typval_T *argvars UNUSED, typval_T *rettv)
{
    પૂર્ણાંક		id = 0;
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    પૂર્ણાંક		trans;
    પૂર્ણાંક		transerr = FALSE;

    જો (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	પાછા;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (linenr_T)tv_get_સંખ્યા(&argvars[1]) - 1;	// -1 on type error
    trans = (પૂર્ણાંક)tv_get_bool_chk(&argvars[2], &transerr);

    જો (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_લીટી_ગણતરી
	    && col >= 0 && col < (long)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, col, trans, ખાલી, FALSE);
#endif

    rettv->vval.v_સંખ્યા = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    સ્થિર શૂન્ય
f_synIDattr(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*p = ખાલી;
#ifdef FEAT_SYN_HL
    પૂર્ણાંક		id;
    char_u	*what;
    char_u	*mode;
    char_u	modebuf[NUMBUFLEN];
    પૂર્ણાંક		modec;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	પાછા;

    id = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	mode = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(mode[0]);
	જો (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    નહીંતર
    {
# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	જો (USE_24BIT)
	    modec = 'g';
	નહીંતર
# endif
	    જો (t_colors > 1)
		modec = 'c';
	    નહીંતર
		modec = 't';
    }

    પસંદગી (TOLOWER_ASC(what[0]))
    {
	સ્થિતિ 'b':
		જો (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		નહીંતર					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		તોડો;

	સ્થિતિ 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		તોડો;

	સ્થિતિ 'i':
		જો (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		નહીંતર					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		તોડો;

	સ્થિતિ 'n':
		જો (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		નહીંતર					// name
		    p = get_highlight_name_ext(ખાલી, id - 1, FALSE);
		તોડો;

	સ્થિતિ 'r':					// reverse
		p = highlight_has_attr(id, HL_INVERSE, modec);
		તોડો;

	સ્થિતિ 's':
		જો (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		નહીંતર જો (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		નહીંતર					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		તોડો;

	સ્થિતિ 'u':
		જો (STRLEN(what) >= 9)
		{
		    જો (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    નહીંતર જો (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    નહીંતર જો (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    નહીંતર જો (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    નહીંતર
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		નહીંતર
							// ul
		    p = highlight_color(id, what, modec);
		તોડો;
    }

    જો (p != ખાલી)
	p = vim_strsave(p);
#endif
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    સ્થિર શૂન્ય
f_synIDtrans(typval_T *argvars UNUSED, typval_T *rettv)
{
    પૂર્ણાંક		id;

#ifdef FEAT_SYN_HL
    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    id = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]);

    જો (id > 0)
	id = syn_get_final_id(id);
    નહીંતર
#endif
	id = 0;

    rettv->vval.v_સંખ્યા = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    સ્થિર શૂન્ય
f_synconcealed(typval_T *argvars UNUSED, typval_T *rettv)
{
#જો defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	col;
    પૂર્ણાંક		syntax_flags = 0;
    પૂર્ણાંક		cchar;
    પૂર્ણાંક		matchid = 0;
    char_u	str[NUMBUFLEN];
#endif

    rettv_list_set(rettv, ખાલી);

    જો (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

#જો defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_સંખ્યા(&argvars[1]) - 1;	// -1 on type error

    CLEAR_FIELD(str);

    જો (rettv_list_alloc(rettv) == OK)
    {
	જો (lnum >= 1 && lnum <= curbuf->b_ml.ml_લીટી_ગણતરી
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (શૂન્ય)syn_get_id(curwin, lnum, col, FALSE, ખાલી, FALSE);
	    syntax_flags = get_syntax_info(&matchid);

	    // get the conceal character
	    જો ((syntax_flags & HL_CONCEAL) && curwin->w_p_cole < 3)
	    {
		cchar = syn_get_sub_char();
		જો (cchar == NUL && curwin->w_p_cole == 1)
		    cchar = (curwin->w_lcs_chars.conceal == NUL) ? ' '
					: curwin->w_lcs_chars.conceal;
		જો (cchar != NUL)
		{
		    જો (has_mbyte)
			(*mb_char2bytes)(cchar, str);
		    નહીંતર
			str[0] = cchar;
		}
	    }
	}

	list_append_સંખ્યા(rettv->vval.v_list,
					    (syntax_flags & HL_CONCEAL) != 0);
	// -1 to auto-determine strlen
	list_append_string(rettv->vval.v_list, str, -1);
	list_append_સંખ્યા(rettv->vval.v_list, matchid);
    }
#endif
}

/*
 * "synstack(lnum, col)" function
 */
    સ્થિર શૂન્ય
f_synstack(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    પૂર્ણાંક		i;
    પૂર્ણાંક		id;
#endif

    rettv_list_set(rettv, ખાલી);

    જો (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

#ifdef FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_સંખ્યા(&argvars[1]) - 1;	// -1 on type error

    જો (lnum >= 1 && lnum <= curbuf->b_ml.ml_લીટી_ગણતરી
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(શૂન્ય)syn_get_id(curwin, lnum, col, FALSE, ખાલી, TRUE);
	માટે (i = 0; ; ++i)
	{
	    id = syn_get_ઢગલો_item(i);
	    જો (id < 0)
		તોડો;
	    જો (list_append_સંખ્યા(rettv->vval.v_list, id) == FAIL)
		તોડો;
	}
    }
#endif
}

/*
 * "tabpagebuflist()" function
 */
    સ્થિર શૂન્ય
f_tabpagebuflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp = ખાલી;

    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    નહીંતર
    {
	tp = find_tabpage((પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]));
	જો (tp != ખાલી)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    જો (wp != ખાલી && rettv_list_alloc(rettv) == OK)
    {
	માટે (; wp != ખાલી; wp = wp->w_next)
	    જો (list_append_સંખ્યા(rettv->vval.v_list,
						wp->w_buffer->b_fnum) == FAIL)
		તોડો;
    }
}

/*
 * "tagfiles()" function
 */
    સ્થિર શૂન્ય
f_tagfiles(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    પૂર્ણાંક		first;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;
    fname = alloc(MAXPATHL);
    જો (fname == ખાલી)
	પાછા;

    માટે (first = TRUE; ; first = FALSE)
	જો (get_tagfname(&tn, first, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    તોડો;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    સ્થિર શૂન્ય
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = ખાલી;
    char_u  *tag_pattern;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	પાછા;

    tag_pattern = tv_get_string(&argvars[0]);

    rettv->vval.v_સંખ્યા = FALSE;
    જો (*tag_pattern == NUL)
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    જો (rettv_list_alloc(rettv) == OK)
	(શૂન્ય)get_tags(rettv->vval.v_list, tag_pattern, fname);
}

/*
 * "type(expr)" function
 */
    સ્થિર શૂન્ય
f_type(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક n = -1;

    પસંદગી (argvars[0].v_type)
    {
	સ્થિતિ VAR_સંખ્યા:  n = VAR_TYPE_સંખ્યા; તોડો;
	સ્થિતિ VAR_STRING:  n = VAR_TYPE_STRING; તોડો;
	સ્થિતિ VAR_PARTIAL:
	સ્થિતિ VAR_FUNC:    n = VAR_TYPE_FUNC; તોડો;
	સ્થિતિ VAR_LIST:    n = VAR_TYPE_LIST; તોડો;
	સ્થિતિ VAR_TUPLE:   n = VAR_TYPE_TUPLE; તોડો;
	સ્થિતિ VAR_DICT:    n = VAR_TYPE_DICT; તોડો;
	સ્થિતિ VAR_FLOAT:   n = VAR_TYPE_FLOAT; તોડો;
	સ્થિતિ VAR_BOOL:	  n = VAR_TYPE_BOOL; તોડો;
	સ્થિતિ VAR_SPECIAL: n = VAR_TYPE_NONE; તોડો;
	સ્થિતિ VAR_JOB:     n = VAR_TYPE_JOB; તોડો;
	સ્થિતિ VAR_CHANNEL: n = VAR_TYPE_CHANNEL; તોડો;
	સ્થિતિ VAR_BLOB:    n = VAR_TYPE_BLOB; તોડો;
	સ્થિતિ VAR_INSTR:   n = VAR_TYPE_INSTR; તોડો;
	સ્થિતિ VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; તોડો;
	સ્થિતિ VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		જો (cl != ખાલી && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		નહીંતર
		    n = VAR_TYPE_CLASS;
		તોડો;
	    }
	સ્થિતિ VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		જો (obj == ખાલી)
		    n = VAR_TYPE_OBJECT;
		નહીંતર
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    જો (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    નહીંતર
			n = VAR_TYPE_OBJECT;
		}
		તોડો;
	    }
	સ્થિતિ VAR_VOID:
	    emsg(_(e_cannot_use_void_કિંમત));
	    તોડો;
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	    internal_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    તોડો;
    }
    rettv->vval.v_સંખ્યા = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    સ્થિર શૂન્ય
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_start = 0;
    colnr_T	vcol_end = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    પૂર્ણાંક		winchanged = FALSE;
    પૂર્ણાંક		len;

    જો (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL))))
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((પૂર્ણાંક)tv_get_સંખ્યા(&argvars[2]), &tp);
	જો (wp == ખાલી || tp == ખાલી)
	    જાઓ theend;

	જો (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    જાઓ theend;

	check_cursor();
	winchanged = TRUE;
    }

    પૂર્ણાંક fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    જો (fp != ખાલી && fp->lnum <= curbuf->b_ml.ml_લીટી_ગણતરી
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	જો (fp->col < 0)
	    fp->col = 0;
	નહીંતર
	{
	    len = (પૂર્ણાંક)ml_get_len(fp->lnum);
	    જો (fp->col > len)
		fp->col = len;
	}
	getvvcol(curwin, fp, &vcol_start, ખાલી, &vcol_end, 0);
	++vcol_start;
	++vcol_end;
    }

theend:
    જો (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	જો (rettv_list_alloc(rettv) == OK)
	{
	    list_append_સંખ્યા(rettv->vval.v_list, vcol_start);
	    list_append_સંખ્યા(rettv->vval.v_list, vcol_end);
	}
	નહીંતર
	    rettv->vval.v_સંખ્યા = 0;
    }
    નહીંતર
	rettv->vval.v_સંખ્યા = vcol_end;

    જો (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    સ્થિર શૂન્ય
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    જો (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_mode_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    જો (non_zero_arg(&argvars[0]))
	curbuf->b_visual_mode_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    સ્થિર શૂન્ય
f_wildmenumode(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    જો (wild_menu_showing || ((State & MODE_CMDLINE) && cmdline_pum_active()))
	rettv->vval.v_સંખ્યા = 1;
}

/*
 * "windowsversion()" function
 */
    સ્થિર શૂન્ય
f_windowsversion(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windowsVersion);
}

/*
 * "wordcount()" function
 */
    સ્થિર શૂન્ય
f_wordcount(typval_T *argvars UNUSED, typval_T *rettv)
{
    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    સ્થિર શૂન્ય
f_xor(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    rettv->vval.v_સંખ્યા = tv_get_સંખ્યા_chk(&argvars[0], ખાલી)
					^ tv_get_સંખ્યા_chk(&argvars[1], ખાલી);
}

#endif // FEAT_EVAL
