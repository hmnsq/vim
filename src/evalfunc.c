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
#నిర్వచించు USING_FLOAT_STUFF

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

#నిర్వచితమైతే VMS
# include <float.h>
#అయితే_ముగింపు

స్థిర శూన్య f_and(typval_T *argvars, typval_T *rettv);
#నిర్వచితమైతే FEAT_BEVAL
స్థిర శూన్య f_balloon_gettext(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_balloon_show(typval_T *argvars, typval_T *rettv);
# ఒకవేళ defined(FEAT_BEVAL_పదం)
స్థిర శూన్య f_balloon_విభజించు(typval_T *argvars, typval_T *rettv);
# endif
#అయితే_ముగింపు
స్థిర శూన్య f_base64_encode(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_base64_decode(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_bindtextdomain(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_byte2line(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_call(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_changenr(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_char2nr(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_charcol(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_నిలువు(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_confirm(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_copy(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_cursor(typval_T *argsvars, typval_T *rettv);
#నిర్వచితమైతే MSWIN
స్థిర శూన్య f_debugbreak(typval_T *argvars, typval_T *rettv);
#అయితే_ముగింపు
స్థిర శూన్య f_deepcopy(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_did_filetype(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_echoraw(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_ఖాళీలు(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_environ(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_err_teapot(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_escape(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_eval(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_eventhandler(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_execute(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_exists_compiled(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_expand(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_expandcmd(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_feedkeys(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_fnameescape(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_foreground(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_funcref(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_function(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_garbagecollect(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_get(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getbgcolor(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getcellpixels(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getchangelist(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getcharpos(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getcharsearch(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getcurpos(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getcursorcharpos(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getenv(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getfontname(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getinfo(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getjumplist(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getpid(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getpos(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getreg(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getreginfo(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getregion(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getregionpos(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_getregtype(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_gettagstack(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_gettext(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_haslocaldir(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_hlID(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_hlexists(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_hostname(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_id(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_సూచీ(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_indexof(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_ఆదానం(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_inputdialog(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_inputlist(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_inputrestore(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_inputsave(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_inputsecret(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_అంతరాయం(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_invert(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_islocked(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_వస్తువులు(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_keytrans(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_చివరి_బఫరు_nr(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_libcall(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_libcallnr(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_పంక్తి(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_line2byte(typval_T *argvars, typval_T *rettv);
#నిర్వచితమైతే FEAT_LUA
స్థిర శూన్య f_luaeval(typval_T *argvars, typval_T *rettv);
#అయితే_ముగింపు
స్థిర శూన్య f_సరిపోలిక(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_matchbufline(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_matchend(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_matchlist(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_matchstr(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_matchstrlist(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_matchstrpos(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_max(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_min(typval_T *argvars, typval_T *rettv);
#నిర్వచితమైతే FEAT_MZSCHEME
స్థిర శూన్య f_mzeval(typval_T *argvars, typval_T *rettv);
#అయితే_ముగింపు
స్థిర శూన్య f_nextnonblank(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_ngettext(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_nr2char(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_or(typval_T *argvars, typval_T *rettv);
#నిర్వచితమైతే FEAT_PERL
స్థిర శూన్య f_perleval(typval_T *argvars, typval_T *rettv);
#అయితే_ముగింపు
స్థిర శూన్య f_prevnonblank(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_printf(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_pum_getpos(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_pumvisible(typval_T *argvars, typval_T *rettv);
#నిర్వచితమైతే FEAT_PYTHON3
స్థిర శూన్య f_py3eval(typval_T *argvars, typval_T *rettv);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PYTHON
స్థిర శూన్య f_pyeval(typval_T *argvars, typval_T *rettv);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
స్థిర శూన్య f_pyxeval(typval_T *argvars, typval_T *rettv);
#అయితే_ముగింపు
స్థిర శూన్య f_test_srand_విత్తనం(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_rand(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_range(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_reg_executing(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_reg_recording(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_rename(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_repeat(typval_T *argvars, typval_T *rettv);
#నిర్వచితమైతే FEAT_RUBY
స్థిర శూన్య f_rubyeval(typval_T *argvars, typval_T *rettv);
#అయితే_ముగింపు
స్థిర శూన్య f_screenattr(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_screenchar(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_screenchars(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_screencol(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_screenrow(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_screenstring(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_శోధన(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_searchdecl(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_searchpair(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_searchpairpos(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_searchpos(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_setcharpos(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_setcharsearch(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_setcursorcharpos(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_setenv(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_setfperm(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_setpos(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_setreg(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_settagstack(typval_T *argvars, typval_T *rettv);
#నిర్వచితమైతే FEAT_CRYPT
స్థిర శూన్య f_sha256(typval_T *argvars, typval_T *rettv);
#అయితే_ముగింపు
స్థిర శూన్య f_shellescape(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_shiftwidth(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_soundfold(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_spellbadword(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_spellsuggest(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_విభజించు(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_srand(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_submatch(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_substitute(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_swapfilelist(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_swapinfo(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_swapname(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_synID(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_synIDattr(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_synIDtrans(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_synstack(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_synconcealed(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_tabpagebuflist(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_taglist(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_tagfiles(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_type(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_virtcol(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_visualmode(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_wildmenumode(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_windowsversion(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_wordcount(typval_T *argvars, typval_T *rettv);
స్థిర శూన్య f_xor(typval_T *argvars, typval_T *rettv);


/*
 * Functions that check the argument type of a builtin function.
 * Each function returns FAIL and gives an error message if the type is wrong.
 */

// Context passed to an arg_ function.
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		arg_లెక్క;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    పూర్ణాంకం		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
రకనిర్వచనం పూర్ణాంకం (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    స్థిర పూర్ణాంకం
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    తిరిగి need_type(actual, expected, 0,
	    context->arg_idx - context->arg_లెక్క, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    స్థిర పూర్ణాంకం
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    ఒకవేళ (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_లెక్క, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	తిరిగి FAIL;
    తిరిగి arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    పూర్ణాంకం
arg_type_modifiable(type_T *type, పూర్ణాంకం arg_idx)
{
    అక్షరం *tofree;

    ఒకవేళ ((type->tt_flags & TTFLAG_CONST) == 0)
	తిరిగి OK;
    semsg(_(e_వాదం_nr_trying_గమ్యం_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    తిరిగి FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    స్థిర పూర్ణాంకం
arg_any(type_T *type వాడని,
	type_T *decl_type వాడని,
	argcontext_T *context వాడని)
{
    తిరిగి OK;
}

/*
 * Check "type" is a float or a number.
 */
    స్థిర పూర్ణాంకం
arg_float_or_nr(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_సంఖ్య
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_సంఖ్య, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a number.
 */
    స్థిర పూర్ణాంకం
arg_సంఖ్య(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    తిరిగి check_arg_type(&t_సంఖ్య, type, context);
}

/*
 * Check "type" is an object.
 */
    స్థిర పూర్ణాంకం
arg_object(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    స్థిర పూర్ణాంకం
arg_dict_any(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    తిరిగి check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    స్థిర పూర్ణాంకం
arg_list_any(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    తిరిగి check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    స్థిర పూర్ణాంకం
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type వాడని,
	argcontext_T *context)
{
    తిరిగి check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    స్థిర పూర్ణాంకం
arg_list_సంఖ్య(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    తిరిగి check_arg_type(&t_list_సంఖ్య, type, context);
}

/*
 * Check "type" is a list of strings.
 */
    స్థిర పూర్ణాంకం
arg_list_string(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    తిరిగి check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    స్థిర పూర్ణాంకం
arg_tuple_any(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    తిరిగి check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    స్థిర పూర్ణాంకం
arg_string(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    తిరిగి check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    స్థిర పూర్ణాంకం
arg_blob(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    తిరిగి check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    స్థిర పూర్ణాంకం
arg_bool(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    తిరిగి check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    స్థిర పూర్ణాంకం
arg_bool_or_nr(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_సంఖ్య
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_సంఖ్య, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    స్థిర పూర్ణాంకం
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	తిరిగి OK;
    తిరిగి check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    స్థిర పూర్ణాంకం
arg_list_or_blob(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    స్థిర పూర్ణాంకం
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ఒకవేళ (arg_list_or_blob(type, decl_type, context) == FAIL)
	తిరిగి FAIL;
    తిరిగి arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    స్థిర పూర్ణాంకం
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    తిరిగి FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    స్థిర పూర్ణాంకం
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a string or a number
 */
    స్థిర పూర్ణాంకం
arg_string_or_nr(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_సంఖ్య
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    స్థిర పూర్ణాంకం
arg_బఫరు(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_సంఖ్య
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    స్థిర పూర్ణాంకం
arg_బఫరు_or_dict_any(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_సంఖ్య
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    స్థిర పూర్ణాంకం
arg_lnum(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_సంఖ్య
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    స్థిర పూర్ణాంకం
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	తిరిగి OK;
    ఒకవేళ (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	తిరిగి FAIL;
    }
    ఒకవేళ (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	తిరిగి OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    స్థిర పూర్ణాంకం
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    స్థిర పూర్ణాంకం
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    స్థిర పూర్ణాంకం
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    స్థిర పూర్ణాంకం
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    స్థిర పూర్ణాంకం
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type వాడని,
	argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    స్థిర పూర్ణాంకం
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ఒకవేళ (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	తిరిగి FAIL;
    తిరిగి arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    స్థిర పూర్ణాంకం
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    స్థిర పూర్ణాంకం
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ఒకవేళ (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	తిరిగి FAIL;
    తిరిగి arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    స్థిర పూర్ణాంకం
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	తిరిగి OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_వాదం_nr),
						 context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    స్థిర పూర్ణాంకం
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = ఖాళీ;
    type_T *(వాదనలు[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, ఖాళీ, ఖాళీ, వాదనలు};

    ఒకవేళ (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	ఒకవేళ (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	లేకపోతే
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    లేకపోతే ఒకవేళ (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    లేకపోతే ఒకవేళ (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_సంఖ్య;

    వాదనలు[0] = ఖాళీ;
    వాదనలు[1] = &t_unknown;
    ఒకవేళ (type->tt_argcount != -1)
    {
	ఒకవేళ (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_సంఖ్య_యొక్క_arguments));
	    తిరిగి FAIL;
	}
	ఒకవేళ (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	లేకపోతే
	{
	    ఒకవేళ (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		వాదనలు[0] = &t_సంఖ్య;
	    లేకపోతే ఒకవేళ (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		వాదనలు[0] = &t_string;
	    ఒకవేళ (వాదనలు[0] != ఖాళీ)
		వాదనలు[1] = expected_member;
	}
    }

    ఒకవేళ (!type_any_or_unknown(type->tt_member) || వాదనలు[0] != ఖాళీ)
    {
	where_T where = WHERE_INIT;

	ఒకవేళ (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == ఖాళీ
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	లేకపోతే ఒకవేళ (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	లేకపోతే // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	ఒకవేళ (వాదనలు[0] == ఖాళీ)
	    వాదనలు[0] = &t_unknown;
	ఒకవేళ (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_సూచీ = 2;
	where.wt_రకం = WT_వాదం;
	తిరిగి check_type(&t_func_exp, type, TRUE, where);
    }
    తిరిగి OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    స్థిర పూర్ణాంకం
arg_filter_func(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	తిరిగి OK;

    ఒకవేళ (type->tt_type == VAR_FUNC)
	తిరిగి check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_వాదం_nr), 2);
    తిరిగి FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    స్థిర పూర్ణాంకం
arg_map_func(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	తిరిగి OK;

    ఒకవేళ (type->tt_type == VAR_FUNC)
	తిరిగి check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_వాదం_nr), 2);
    తిరిగి FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    స్థిర పూర్ణాంకం
arg_foreach_func(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	తిరిగి OK;

    ఒకవేళ (type->tt_type == VAR_FUNC)
	తిరిగి check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_వాదం_nr), 2);
    తిరిగి FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    స్థిర పూర్ణాంకం
arg_క్రమీకరించు_how(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	తిరిగి OK;

    ఒకవేళ (type->tt_type == VAR_FUNC)
    {
	type_T *(వాదనలు[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_సంఖ్య, ఖాళీ, వాదనలు};

	ఒకవేళ (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    వాదనలు[0] = context->arg_types[0].type_curr->tt_member;
	లేకపోతే
	    వాదనలు[0] = &t_unknown;
	ఒకవేళ (!type_any_or_unknown(type->tt_member) || వాదనలు[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    వాదనలు[1] = వాదనలు[0];
	    ఒకవేళ (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_సూచీ = 2;
	    where.wt_రకం = WT_వాదం;
	    తిరిగి check_type(&t_func_exp, type, TRUE, where);
	}

	తిరిగి OK;
    }
    semsg(_(e_string_or_function_required_for_వాదం_nr), 2);
    తిరిగి FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    స్థిర పూర్ణాంకం
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_సంఖ్య
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    స్థిర పూర్ణాంకం
varargs_class(type_T *type వాడని,
	      type_T *decl_type వాడని,
	      argcontext_T *context)
{
    కొరకు (పూర్ణాంకం i = context->arg_idx; i < context->arg_లెక్క; ++i)
    {
	type2_T *types = &context->arg_types[i];
	ఒకవేళ (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_వాదం_nr), i + 1);
	    తిరిగి FAIL;
	}
    }
    తిరిగి OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    స్థిర పూర్ణాంకం
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    స్థిర పూర్ణాంకం
arg_తిరగేయి(type_T *type, type_T *decl_type, argcontext_T *context)
{
    ఒకవేళ (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	తిరిగి FAIL;
    తిరిగి arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    స్థిర పూర్ణాంకం
arg_పని(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    తిరిగి check_arg_type(&t_పని, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    స్థిర పూర్ణాంకం
arg_chan_or_పని(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_పని
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    స్థిర పూర్ణాంకం
arg_same_as_prev(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_decl;

    తిరిగి check_arg_type(prev_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    స్థిర పూర్ణాంకం
arg_same_struct_as_prev(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;

    ఒకవేళ (prev_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	తిరిగి check_arg_type(prev_type, type, context);
    తిరిగి OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    స్థిర పూర్ణాంకం
arg_అంశం_యొక్క_prev(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    ఒకవేళ (prev_type->tt_type == VAR_LIST)
	expected = prev_type->tt_member;
    లేకపోతే ఒకవేళ (prev_type->tt_type == VAR_BLOB)
	expected = &t_సంఖ్య;
    లేకపోతే
	// probably VAR_ANY, can't check
	తిరిగి OK;

    తిరిగి check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    స్థిర పూర్ణాంకం
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_సంఖ్య
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    స్థిర పూర్ణాంకం
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	తిరిగి OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    స్థిర పూర్ణాంకం
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *మొదటి_type = context->arg_types[context->arg_idx - 2].type_curr;

    ఒకవేళ (మొదటి_type->tt_type == VAR_LIST || మొదటి_type->tt_type == VAR_BLOB)
	తిరిగి arg_సంఖ్య(type, decl_type, context);
    ఒకవేళ (మొదటి_type->tt_type == VAR_DICT)
	తిరిగి arg_string(type, decl_type, context);
    తిరిగి OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    స్థిర పూర్ణాంకం
arg_get1(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	తిరిగి OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    స్థిర పూర్ణాంకం
arg_len1(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_సంఖ్య
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	తిరిగి OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    స్థిర పూర్ణాంకం
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *మొదటి_type = context->arg_types[context->arg_idx - 1].type_curr;

    ఒకవేళ (మొదటి_type->tt_type == VAR_LIST || మొదటి_type->tt_type == VAR_BLOB)
	తిరిగి arg_సంఖ్య(type, decl_type, context);
    ఒకవేళ (మొదటి_type->tt_type == VAR_DICT)
	తిరిగి arg_string_or_nr(type, decl_type, context);
    తిరిగి OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    స్థిర పూర్ణాంకం
arg_repeat1(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_సంఖ్య
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	తిరిగి OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    స్థిర పూర్ణాంకం
arg_slice1(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	తిరిగి OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    స్థిర పూర్ణాంకం
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type వాడని,
    argcontext_T	*context)
{
    ఒకవేళ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	తిరిగి OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_వాదం_nr),
							 context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    స్థిర పూర్ణాంకం
arg_cursor1(type_T *type, type_T *decl_type వాడని, argcontext_T *context)
{
    ఒకవేళ (type->tt_type == VAR_సంఖ్య
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	తిరిగి OK;

    arg_type_mismatch(&t_సంఖ్య, type, context->arg_idx + 1);
    తిరిగి FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
స్థిర argcheck_T arg1_blob[] = {arg_blob};
స్థిర argcheck_T arg1_bool[] = {arg_bool};
స్థిర argcheck_T arg1_బఫరు[] = {arg_బఫరు};
స్థిర argcheck_T arg1_బఫరు_or_dict_any[] = {arg_బఫరు_or_dict_any};
స్థిర argcheck_T arg1_chan_or_పని[] = {arg_chan_or_పని};
స్థిర argcheck_T arg1_dict_any[] = {arg_dict_any};
స్థిర argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
స్థిర argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
స్థిర argcheck_T arg1_పని[] = {arg_పని};
స్థిర argcheck_T arg1_list_any[] = {arg_list_any};
స్థిర argcheck_T arg1_tuple_any[] = {arg_tuple_any};
స్థిర argcheck_T arg1_list_సంఖ్య[] = {arg_list_సంఖ్య};
స్థిర argcheck_T arg1_తిరగేయి[] = {arg_తిరగేయి};
స్థిర argcheck_T arg1_list_or_tuple_or_dict[] = {arg_list_or_tuple_or_dict};
స్థిర argcheck_T arg1_list_string[] = {arg_list_string};
స్థిర argcheck_T arg1_list_tuple_dict_blob_or_string[] = {arg_list_tuple_dict_blob_or_string};
స్థిర argcheck_T arg1_lnum[] = {arg_lnum};
స్థిర argcheck_T arg1_సంఖ్య[] = {arg_సంఖ్య};
స్థిర argcheck_T arg1_string[] = {arg_string};
స్థిర argcheck_T arg1_string_or_list_any[] = {arg_string_or_list_any};
స్థిర argcheck_T arg1_string_or_list_string[] = {arg_string_or_list_string};
స్థిర argcheck_T arg1_string_or_nr[] = {arg_string_or_nr};
స్థిర argcheck_T arg1_string_or_blob[] = {arg_string_or_blob};
స్థిర argcheck_T arg2_బఫరు_any[] = {arg_బఫరు, arg_any};
స్థిర argcheck_T arg2_బఫరు_bool[] = {arg_బఫరు, arg_bool};
స్థిర argcheck_T arg2_బఫరు_list_any[] = {arg_బఫరు, arg_list_any};
స్థిర argcheck_T arg2_బఫరు_lnum[] = {arg_బఫరు, arg_lnum};
స్థిర argcheck_T arg2_బఫరు_సంఖ్య[] = {arg_బఫరు, arg_సంఖ్య};
స్థిర argcheck_T arg2_బఫరు_string[] = {arg_బఫరు, arg_string};
స్థిర argcheck_T arg2_chan_or_పని_dict[] = {arg_chan_or_పని, arg_dict_any};
స్థిర argcheck_T arg2_chan_or_పని_string[] = {arg_chan_or_పని, arg_string};
స్థిర argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
స్థిర argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
స్థిర argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
స్థిర argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
స్థిర argcheck_T arg2_పని_dict[] = {arg_పని, arg_dict_any};
స్థిర argcheck_T arg2_పని_string_or_సంఖ్య[] = {arg_పని, arg_string_or_nr};
స్థిర argcheck_T arg2_list_any_సంఖ్య[] = {arg_list_any, arg_సంఖ్య};
స్థిర argcheck_T arg2_list_సంఖ్య[] = {arg_list_సంఖ్య, arg_list_సంఖ్య};
స్థిర argcheck_T arg2_list_సంఖ్య_bool[] = {arg_list_సంఖ్య, arg_bool};
స్థిర argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
స్థిర argcheck_T arg2_listblobmod_అంశం[] = {arg_list_or_blob_mod, arg_అంశం_యొక్క_prev};
స్థిర argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
స్థిర argcheck_T arg2_lnum_సంఖ్య[] = {arg_lnum, arg_సంఖ్య};
స్థిర argcheck_T arg2_సంఖ్య[] = {arg_సంఖ్య, arg_సంఖ్య};
స్థిర argcheck_T arg2_సంఖ్య_any[] = {arg_సంఖ్య, arg_any};
స్థిర argcheck_T arg2_సంఖ్య_bool[] = {arg_సంఖ్య, arg_bool};
స్థిర argcheck_T arg2_సంఖ్య_dict_any[] = {arg_సంఖ్య, arg_dict_any};
స్థిర argcheck_T arg2_సంఖ్య_list[] = {arg_సంఖ్య, arg_list_any};
స్థిర argcheck_T arg2_సంఖ్య_string[] = {arg_సంఖ్య, arg_string};
స్థిర argcheck_T arg2_సంఖ్య_బఫరు[] = {arg_సంఖ్య, arg_బఫరు};
స్థిర argcheck_T arg2_సంఖ్య_string_or_list[] = {arg_సంఖ్య, arg_string_or_list_any};
స్థిర argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
స్థిర argcheck_T arg2_string[] = {arg_string, arg_string};
స్థిర argcheck_T arg2_string_any[] = {arg_string, arg_any};
స్థిర argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
స్థిర argcheck_T arg2_string_chan_or_పని[] = {arg_string, arg_chan_or_పని};
స్థిర argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
స్థిర argcheck_T arg2_string_list_సంఖ్య[] = {arg_string, arg_list_సంఖ్య};
స్థిర argcheck_T arg2_string_సంఖ్య[] = {arg_string, arg_సంఖ్య};
స్థిర argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
స్థిర argcheck_T arg2_string_or_list_సంఖ్య[] = {arg_string_or_list_any, arg_సంఖ్య};
స్థిర argcheck_T arg2_string_string_or_సంఖ్య[] = {arg_string, arg_string_or_nr};
స్థిర argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
స్థిర argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
స్థిర argcheck_T arg3_any_బఫరు_bool_or_dict[] = {
			      arg_any, arg_బఫరు, arg_bool_or_dict_any};
స్థిర argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
స్థిర argcheck_T arg3_బఫరు_lnum_lnum[] = {arg_బఫరు, arg_lnum, arg_lnum};
స్థిర argcheck_T arg3_బఫరు_సంఖ్య_సంఖ్య[] = {arg_బఫరు, arg_సంఖ్య, arg_సంఖ్య};
స్థిర argcheck_T arg3_బఫరు_string_any[] = {arg_బఫరు, arg_string, arg_any};
స్థిర argcheck_T arg3_బఫరు_string_dict[] = {arg_బఫరు, arg_string, arg_dict_any};
స్థిర argcheck_T arg3_dict_సంఖ్య_సంఖ్య[] = {arg_dict_any, arg_సంఖ్య, arg_సంఖ్య};
స్థిర argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
స్థిర argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
స్థిర argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
స్థిర argcheck_T arg3_lnum_సంఖ్య_bool[] = {arg_lnum, arg_సంఖ్య, arg_bool};
స్థిర argcheck_T arg3_సంఖ్య[] = {arg_సంఖ్య, arg_సంఖ్య, arg_సంఖ్య};
స్థిర argcheck_T arg3_సంఖ్య_any_dict[] = {arg_సంఖ్య, arg_any, arg_dict_any};
స్థిర argcheck_T arg3_సంఖ్య_సంఖ్య_dict[] = {arg_సంఖ్య, arg_సంఖ్య, arg_dict_any};
స్థిర argcheck_T arg3_సంఖ్య_string_any[] = {arg_సంఖ్య, arg_string, arg_any};
స్థిర argcheck_T arg3_సంఖ్య_string_బఫరు[] = {arg_సంఖ్య, arg_string, arg_బఫరు};
స్థిర argcheck_T arg3_సంఖ్య_string_string[] = {arg_సంఖ్య, arg_string, arg_string};
స్థిర argcheck_T arg3_string[] = {arg_string, arg_string, arg_string};
స్థిర argcheck_T arg3_string_any_dict[] = {arg_string, arg_any, arg_dict_any};
స్థిర argcheck_T arg3_string_any_string[] = {arg_string, arg_any, arg_string};
స్థిర argcheck_T arg3_string_bool_bool[] = {arg_string, arg_bool, arg_bool};
స్థిర argcheck_T arg3_string_సంఖ్య_bool[] = {arg_string, arg_సంఖ్య, arg_bool};
స్థిర argcheck_T arg3_string_సంఖ్య_సంఖ్య[] = {arg_string, arg_సంఖ్య, arg_సంఖ్య};
స్థిర argcheck_T arg3_string_or_dict_bool_dict[] = {arg_string_or_dict_any, arg_bool, arg_dict_any};
స్థిర argcheck_T arg3_string_or_list_bool_సంఖ్య[] = {arg_string_or_list_any, arg_bool, arg_సంఖ్య};
స్థిర argcheck_T arg3_string_string_bool[] = {arg_string, arg_string, arg_bool};
స్థిర argcheck_T arg3_string_string_dict[] = {arg_string, arg_string, arg_dict_any};
స్థిర argcheck_T arg3_string_string_సంఖ్య[] = {arg_string, arg_string, arg_సంఖ్య};
స్థిర argcheck_T arg4_సంఖ్య_సంఖ్య_string_any[] = {arg_సంఖ్య, arg_సంఖ్య, arg_string, arg_any};
స్థిర argcheck_T arg4_string_string_any_string[] = {arg_string, arg_string, arg_any, arg_string};
స్థిర argcheck_T arg4_string_string_సంఖ్య_string[] = {arg_string, arg_string, arg_సంఖ్య, arg_string};
స్థిర argcheck_T arg4_string_సంఖ్య_bool_bool[] = {arg_string, arg_సంఖ్య, arg_bool, arg_bool};
/* Function specific argument types (not covered by the above) */
స్థిర argcheck_T arg15_assert_fails[] = {arg_string_or_nr, arg_string_or_list_any, arg_any, arg_సంఖ్య, arg_string};
స్థిర argcheck_T arg34_assert_inrange[] = {arg_float_or_nr, arg_float_or_nr, arg_float_or_nr, arg_string};
స్థిర argcheck_T arg4_browse[] = {arg_bool, arg_string, arg_string, arg_string};
స్థిర argcheck_T arg23_chanexpr[] = {arg_chan_or_పని, arg_any, arg_dict_any};
స్థిర argcheck_T arg23_chanraw[] = {arg_chan_or_పని, arg_string_or_blob, arg_dict_any};
స్థిర argcheck_T arg24_లెక్క[] = {arg_string_list_tuple_or_dict, arg_any, arg_bool, arg_సంఖ్య};
స్థిర argcheck_T arg13_cursor[] = {arg_cursor1, arg_సంఖ్య, arg_సంఖ్య};
స్థిర argcheck_T arg12_deepcopy[] = {arg_any, arg_bool};
స్థిర argcheck_T arg12_execute[] = {arg_string_or_list_string, arg_string};
స్థిర argcheck_T arg12_getchar[] = {arg_bool_or_nr, arg_dict_any};
స్థిర argcheck_T arg23_extend[] = {arg_list_or_dict_or_blob_mod, arg_same_as_prev, arg_extend3};
స్థిర argcheck_T arg23_extendnew[] = {arg_list_or_dict_or_blob, arg_same_struct_as_prev, arg_extend3};
స్థిర argcheck_T arg23_get[] = {arg_get1, arg_string_or_nr, arg_any};
స్థిర argcheck_T arg14_glob[] = {arg_string, arg_bool, arg_bool, arg_bool};
స్థిర argcheck_T arg25_globpath[] = {arg_string, arg_string, arg_bool, arg_bool, arg_bool};
స్థిర argcheck_T arg24_సూచీ[] = {arg_list_or_tuple_or_blob, arg_అంశం_యొక్క_prev, arg_సంఖ్య, arg_bool};
స్థిర argcheck_T arg23_సూచీ[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
స్థిర argcheck_T arg23_చొప్పించు[] = {arg_list_or_blob, arg_అంశం_యొక్క_prev, arg_సంఖ్య};
స్థిర argcheck_T arg1_len[] = {arg_len1};
స్థిర argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
స్థిర argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
స్థిర argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
స్థిర argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
స్థిర argcheck_T arg2_instanceof[] = {arg_object, varargs_class, ఖాళీ };
స్థిర argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
స్థిర argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
స్థిర argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_సంఖ్య, arg_సంఖ్య, arg_dict_any};
స్థిర argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_సంఖ్య, arg_సంఖ్య, arg_dict_any};
స్థిర argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
స్థిర argcheck_T arg45_matchbufline[] = {arg_బఫరు, arg_string, arg_lnum, arg_lnum, arg_dict_any};
స్థిర argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
స్థిర argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
స్థిర argcheck_T arg24_remote_సమాసం[] = {arg_string, arg_string, arg_string, arg_సంఖ్య};
స్థిర argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_సంఖ్య};
స్థిర argcheck_T arg2_repeat[] = {arg_repeat1, arg_సంఖ్య};
స్థిర argcheck_T arg15_శోధన[] = {arg_string, arg_string, arg_సంఖ్య, arg_సంఖ్య, arg_string_or_func};
స్థిర argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_సంఖ్య, arg_సంఖ్య};
స్థిర argcheck_T arg3_setbufline[] = {arg_బఫరు, arg_lnum, arg_str_or_nr_or_list};
స్థిర argcheck_T arg2_setline[] = {arg_lnum, arg_any};
స్థిర argcheck_T arg24_setloclist[] = {arg_సంఖ్య, arg_list_any, arg_string, arg_dict_any};
స్థిర argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
స్థిర argcheck_T arg23_settagstack[] = {arg_సంఖ్య, arg_dict_any, arg_string};
స్థిర argcheck_T arg02_sign_getplaced[] = {arg_బఫరు, arg_dict_any};
స్థిర argcheck_T arg45_sign_ఉంచు[] = {arg_సంఖ్య, arg_string, arg_string, arg_బఫరు, arg_dict_any};
స్థిర argcheck_T arg23_ముక్క[] = {arg_slice1, arg_సంఖ్య, arg_సంఖ్య};
స్థిర argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_క్రమీకరించు_how, arg_dict_any};
స్థిర argcheck_T arg24_strpart[] = {arg_string, arg_సంఖ్య, arg_సంఖ్య, arg_bool};
స్థిర argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
స్థిర argcheck_T arg23_win_execute[] = {arg_సంఖ్య, arg_string_or_list_string, arg_string};
స్థిర argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
స్థిర argcheck_T arg24_సరిపోలిక_func[] = {arg_string_or_list_any, arg_string, arg_సంఖ్య, arg_సంఖ్య};

// Can be used by functions called through "f_retfunc" to create new types.
స్థిర garray_T *current_type_gap = ఖాళీ;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    స్థిర type_T *
ret_void(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_void;
}
    స్థిర type_T *
ret_any(పూర్ణాంకం	argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_any;
}
    స్థిర type_T *
ret_bool(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_bool;
}
    స్థిర type_T *
ret_సంఖ్య_bool(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_సంఖ్య_bool;
}
    స్థిర type_T *
ret_సంఖ్య(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_సంఖ్య;
}
    స్థిర type_T *
ret_float(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_float;
}
    స్థిర type_T *
ret_string(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_string;
}
    స్థిర type_T *
ret_list_any(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_list_any;
}
    స్థిర type_T *
ret_list_సంఖ్య(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    తిరిగి &t_list_సంఖ్య;
}
    స్థిర type_T *
ret_list_string(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    తిరిగి &t_list_string;
}
    స్థిర type_T *
ret_list_dict_any(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    తిరిగి &t_list_dict_any;
}
    స్థిర type_T *
ret_list_వస్తువులు(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    తిరిగి &t_list_list_any;
}
    స్థిర type_T *
ret_list_list_any(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    తిరిగి &t_list_list_any;
}
    స్థిర type_T *
ret_list_string_వస్తువులు(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    తిరిగి &t_list_list_string;
}
    స్థిర type_T *
ret_list_regionpos(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    తిరిగి &t_list_list_list_సంఖ్య;
}
    స్థిర type_T *
ret_tuple_any(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_tuple_any;
}
    స్థిర type_T *
ret_dict_any(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_dict_any;
}
    స్థిర type_T *
ret_పని_info(పూర్ణాంకం argcount,
	type2_T *argtypes వాడని,
	type_T	**decl_type)
{
    ఒకవేళ (argcount == 0)
    {
	*decl_type = &t_list_any;
	తిరిగి &t_list_పని;
    }
    తిరిగి &t_dict_any;
}
    స్థిర type_T *
ret_dict_సంఖ్య(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_dict_సంఖ్య;
}
    స్థిర type_T *
ret_dict_string(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_dict_string;
}
    స్థిర type_T *
ret_blob(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_blob;
}
    స్థిర type_T *
ret_func_any(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_func_any;
}
    స్థిర type_T *
ret_func_unknown(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_func_unknown;
}
    స్థిర type_T *
ret_channel(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_channel;
}
    స్థిర type_T *
ret_పని(పూర్ణాంకం argcount వాడని,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    తిరిగి &t_పని;
}
    స్థిర type_T *
ret_మొదటి_arg(పూర్ణాంకం argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ఒకవేళ (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	తిరిగి argtypes[0].type_curr;
    }
    తిరిగి &t_void;
}
    స్థిర type_T *
ret_ముక్క(పూర్ణాంకం argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ఒకవేళ (argcount > 0)
    {
	ఒకవేళ (argtypes[0].type_decl != ఖాళీ)
	{
	    ఎంపిక (argtypes[0].type_decl->tt_type)
	    {
		స్థితి VAR_STRING: *decl_type = &t_string; ఆపు;
		స్థితి VAR_BLOB: *decl_type = &t_blob; ఆపు;
		స్థితి VAR_LIST: *decl_type = &t_list_any; ఆపు;
		స్థితి VAR_TUPLE: *decl_type = &t_tuple_any; ఆపు;
		అప్రమేయ: ఆపు;
	    }
	}
	తిరిగి argtypes[0].type_curr;
    }
    తిరిగి &t_void;
}
    స్థిర type_T *
ret_copy(పూర్ణాంకం argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ఒకవేళ (argcount > 0)
    {
	ఒకవేళ (argtypes[0].type_decl != ఖాళీ)
	{
	    ఒకవేళ (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    లేకపోతే ఒకవేళ (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    లేకపోతే
		*decl_type = argtypes[0].type_decl;
	}
	ఒకవేళ (argtypes[0].type_curr != ఖాళీ)
	{
	    ఒకవేళ (argtypes[0].type_curr->tt_type == VAR_LIST)
		తిరిగి &t_list_any;
	    లేకపోతే ఒకవేళ (argtypes[0].type_curr->tt_type == VAR_DICT)
		తిరిగి &t_dict_any;
	}
	తిరిగి argtypes[0].type_curr;
    }
    తిరిగి &t_void;
}
    స్థిర type_T *
ret_extend(పూర్ణాంకం argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ఒకవేళ (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	// if the second argument has a different current type then the current
	// type is "any"
	ఒకవేళ (argcount > 1 && !equal_type(argtypes[0].type_curr,
						     argtypes[1].type_curr, 0))
	{
	    ఒకవేళ (argtypes[0].type_curr->tt_type == VAR_LIST)
		తిరిగి &t_list_any;
	    ఒకవేళ (argtypes[0].type_curr->tt_type == VAR_DICT)
		తిరిగి &t_dict_any;
	    ఒకవేళ (argtypes[0].type_curr->tt_type == VAR_BLOB)
		తిరిగి &t_blob;
	}
	తిరిగి argtypes[0].type_curr;
    }
    తిరిగి &t_void;
}
    స్థిర type_T *
ret_repeat(పూర్ణాంకం argcount,
	type2_T *argtypes,
	type_T	**decl_type వాడని)
{
    ఒకవేళ (argcount == 0)
	తిరిగి &t_any;
    ఒకవేళ (argtypes[0].type_curr == &t_సంఖ్య)
	తిరిగి &t_string;
    తిరిగి argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    స్థిర type_T *
ret_మొదటి_cont(పూర్ణాంకం argcount,
	type2_T *argtypes,
	type_T	**decl_type వాడని)
{
    ఒకవేళ (argcount > 0)
    {
	ఒకవేళ (argtypes[0].type_curr->tt_type == VAR_LIST)
	    తిరిగి &t_list_any;
	ఒకవేళ (argtypes[0].type_curr->tt_type == VAR_DICT)
	    తిరిగి &t_dict_any;
	ఒకవేళ (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    తిరిగి argtypes[0].type_curr;
    }
    తిరిగి &t_any;
}
// for get(): the type of the item when a missing item has the same type
    స్థిర type_T *
ret_get(పూర్ణాంకం argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ఒకవేళ (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*అంశం = ఖాళీ;

	ఒకవేళ (t->tt_type == VAR_BLOB)
	    అంశం = &t_సంఖ్య;
	లేకపోతే ఒకవేళ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    అంశం = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	ఒకవేళ (అంశం != ఖాళీ && (argcount == 3
			? equal_type(అంశం, argtypes[2].type_curr, 0)
			: అంశం->tt_type == VAR_సంఖ్య))
	{
	    ఒకవేళ (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    తిరిగి అంశం;
	}
    }
    తిరిగి &t_any;
}
// for getline()
    స్థిర type_T *
ret_getline(పూర్ణాంకం argcount,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    ఒకవేళ (argcount == 1)
	తిరిగి &t_string;
    *decl_type = &t_list_any;
    తిరిగి &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    స్థిర type_T *
ret_string_or_any_1(పూర్ణాంకం argcount,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    ఒకవేళ (argcount < 1)
	తిరిగి &t_string;
    తిరిగి &t_any;
}
    స్థిర type_T *
ret_string_or_any_2(పూర్ణాంకం argcount,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    ఒకవేళ (argcount < 2)
	తిరిగి &t_string;
    తిరిగి &t_any;
}
    స్థిర type_T *
ret_string_or_any_3(పూర్ణాంకం argcount,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    ఒకవేళ (argcount < 3)
	తిరిగి &t_string;
    తిరిగి &t_any;
}
    స్థిర type_T *
ret_string_or_any_4(పూర్ణాంకం argcount,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    ఒకవేళ (argcount < 4)
	తిరిగి &t_string;
    తిరిగి &t_any;
}
// for abs()
    స్థిర type_T *
ret_abs(పూర్ణాంకం argcount,
	type2_T *argtypes,
	type_T	**decl_type వాడని)
{
    ఒకవేళ (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	తిరిగి &t_float;
    తిరిగి &t_సంఖ్య;
}
// for sign_define()
    స్థిర type_T *
ret_sign_define(పూర్ణాంకం argcount,
	type2_T *argtypes,
	type_T	**decl_type వాడని)
{
    ఒకవేళ (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	తిరిగి &t_list_సంఖ్య;
    తిరిగి &t_సంఖ్య;
}
// for values(): list of member of first argument
    స్థిర type_T *
ret_list_member(పూర్ణాంకం argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ఒకవేళ (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	ఒకవేళ (current_type_gap != ఖాళీ
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	లేకపోతే
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	ఒకవేళ (current_type_gap != ఖాళీ
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    తిరిగి get_list_type(t->tt_member, current_type_gap);
    }
    తిరిగి &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    స్థిర type_T *
ret_list_or_dict_0(పూర్ణాంకం argcount,
	type2_T *argtypes వాడని,
	type_T	**decl_type)
{
    ఒకవేళ (argcount > 0)
	తిరిగి &t_dict_any;
    *decl_type = &t_list_any;
    తిరిగి &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    స్థిర type_T *
ret_list_or_dict_1(పూర్ణాంకం argcount,
	type2_T *argtypes వాడని,
	type_T	**decl_type)
{
    ఒకవేళ (argcount > 1)
	తిరిగి &t_dict_any;
    *decl_type = &t_list_any;
    తిరిగి &t_list_dict_any;
}

    స్థిర type_T *
ret_argv(పూర్ణాంకం argcount,
	type2_T *argtypes వాడని,
	type_T	**decl_type)
{
    // argv() returns list of strings
    ఒకవేళ (argcount == 0)
    {
	*decl_type = &t_list_any;
	తిరిగి &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    తిరిగి &t_any;
}

    స్థిర type_T *
ret_remove(పూర్ణాంకం argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ఒకవేళ (argcount > 0)
    {
	ఒకవేళ (argtypes[0].type_curr->tt_type == VAR_LIST
		|| argtypes[0].type_curr->tt_type == VAR_DICT)
	{
	    ఒకవేళ (argcount == 3)
	    {
		*decl_type = argtypes[0].type_decl;
		తిరిగి argtypes[0].type_curr;
	    }
	    ఒకవేళ (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    తిరిగి argtypes[0].type_curr->tt_member;
	}
	ఒకవేళ (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    తిరిగి &t_సంఖ్య;
    }
    తిరిగి &t_any;
}

    స్థిర type_T *
ret_max_min(పూర్ణాంకం argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ఒకవేళ (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	ఒకవేళ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	లేకపోతే
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	ఒకవేళ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	లేకపోతే
	    t = &t_any;
	తిరిగి t;
    }
    తిరిగి &t_any;
}

    స్థిర type_T *
ret_getreg(పూర్ణాంకం argcount,
	type2_T *argtypes వాడని,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    ఒకవేళ (argcount == 3)
    {
	*decl_type = &t_list_any;
	తిరిగి &t_list_string;
    }
    తిరిగి &t_string;
}

    స్థిర type_T *
ret_virtcol(పూర్ణాంకం argcount,
	type2_T *argtypes వాడని,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    ఒకవేళ (argcount > 1)
    {
	*decl_type = &t_list_any;
	తిరిగి &t_list_సంఖ్య;
    }
    తిరిగి &t_సంఖ్య;
}

    స్థిర type_T *
ret_maparg(పూర్ణాంకం argcount,
	type2_T *argtypes వాడని,
	type_T	**decl_type వాడని)
{
    // Assume that if the fourth argument is passed it's non-zero
    ఒకవేళ (argcount == 4)
	తిరిగి &t_dict_any;
    తిరిగి &t_string;
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
రకనిర్వచనం నిర్మాణం
{
    అక్షరం	*f_name;	// function name
    అక్షరం	f_min_argc;	// minimal number of arguments
    అక్షరం	f_max_argc;	// maximal number of arguments
    అక్షరం	f_argtype;	// for method: FEARG_ values; bits FE_
    argcheck_T	*f_argcheck;	// list of functions to check argument types;
				// use "arg_any" (not NULL) to accept an
				// argument of any type
    అక్షరం	*f_argnames;	// names of the arguments as in the help,
				// separated by ", "; an argument without a
				// name is empty, NULL when there is none
    type_T	*(*f_retfunc)(పూర్ణాంకం argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    శూన్య	(*f_func)(typval_T *వాదనలు, typval_T *rvar);
				// implementation of function
} funcentry_T;

// Set f_max_argc to VARGS for varargs.
#నిర్వచించు VARGS    CHAR_MAX

// values for f_argtype; zero means it cannot be used as a method
#నిర్వచించు FEARG_1	    0x01    // base is the first argument
#నిర్వచించు FEARG_2     0x02    // base is the second argument
#నిర్వచించు FEARG_3     0x03    // base is the third argument
#నిర్వచించు FEARG_4     0x04    // base is the fourth argument
#నిర్వచించు FEARG_ముసుగు  0x0F    // bits in f_argtype used as argument index
#నిర్వచించు FE_X	    0x10    // builtin accepts a non-value (class, typealias)

#ఒకవేళ defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#లేకపోతే
# define MATH_FUNC(name) ఖాళీ
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TIMERS
# define TIMER_FUNC(name) name
#లేకపోతే
# define TIMER_FUNC(name) ఖాళీ
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
# define పని_FUNC(name) name
#లేకపోతే
# define పని_FUNC(name) ఖాళీ
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#లేకపోతే
# define PROP_FUNC(name) ఖాళీ
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
# define SIGN_FUNC(name) name
#లేకపోతే
# define SIGN_FUNC(name) ఖాళీ
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SOUND
# define SOUND_FUNC(name) name
#లేకపోతే
# define SOUND_FUNC(name) ఖాళీ
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
# define పదం_FUNC(name) name
#లేకపోతే
# define పదం_FUNC(name) ఖాళీ
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#లేకపోతే
# define TABPANEL_FUNC(name) ఖాళీ
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_IMAGE
# define IMAGE_FUNC(name) name
#లేకపోతే
# define IMAGE_FUNC(name) ఖాళీ
#అయితే_ముగింపు

స్థిర స్థిరరాశి funcentry_T global_functions[] =
{
    {"abs",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_abs,	    f_abs},
    {"acos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_acos},
    {"add",		2, 2, FEARG_1,	    arg2_listblobmod_అంశం,
			"object, expr",
			ret_మొదటి_arg,	    f_కూడు},
    {"and",		2, 2, FEARG_1,	    arg2_సంఖ్య,
			"expr1, expr2",
			ret_సంఖ్య,	    f_and},
    {"append",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_సంఖ్య_bool,    f_append},
    {"appendbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_సంఖ్య_bool,    f_appendbufline},
    {"argc",		0, 1, 0,	    arg1_సంఖ్య,
			"winid",
			ret_సంఖ్య,	    f_argc},
    {"argidx",		0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_argidx},
    {"arglistid",	0, 2, 0,	    arg2_సంఖ్య,
			"winnr, tabnr",
			ret_సంఖ్య,	    f_arglistid},
    {"argv",		0, 2, 0,	    arg2_సంఖ్య,
			"nr, winid",
			ret_argv,	    f_argv},
    {"asin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_asin},
    {"assert_beeps",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_సంఖ్య_bool,    f_assert_beeps},
    {"assert_equal",	2, 3, FEARG_2,	    ఖాళీ,
			"expected, actual, msg",
			ret_సంఖ్య_bool,    f_assert_equal},
    {"assert_equalfile", 2, 3, FEARG_1,	    arg3_string,
			"fname_one, fname_two, msg",
			ret_సంఖ్య_bool,    f_assert_equalfile},
    {"assert_exception", 1, 2, 0,	    arg2_string,
			"error, msg",
			ret_సంఖ్య_bool,    f_assert_exception},
    {"assert_fails",	1, 5, FEARG_1,	    arg15_assert_fails,
			"cmd, error, msg, lnum, context",
			ret_సంఖ్య_bool,    f_assert_fails},
    {"assert_false",	1, 2, FEARG_1,	    ఖాళీ,
			"actual, msg",
			ret_సంఖ్య_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_సంఖ్య_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_సంఖ్య_bool,    f_assert_సరిపోలిక},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_సంఖ్య_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    ఖాళీ,
			"expected, actual, msg",
			ret_సంఖ్య_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_సంఖ్య_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_సంఖ్య_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    ఖాళీ,
			"actual, msg",
			ret_సంఖ్య_bool,    f_assert_true},
    {"atan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_atan},
    {"atan2",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"expr1, expr2",
			ret_float,	    f_atan2},
    {"autocmd_add",	1, 1, FEARG_1,	    arg1_list_any,
			"acmds",
			ret_bool,	    f_autocmd_కూడు},
    {"autocmd_delete",	1, 1, FEARG_1,	    arg1_list_any,
			"acmds",
			ret_bool,	    f_autocmd_delete},
    {"autocmd_get",	0, 1, FEARG_1,	    arg1_dict_any,
			"opts",
			ret_list_dict_any,  f_autocmd_get},
    {"balloon_gettext",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,
#నిర్వచితమైతే FEAT_BEVAL
	    f_balloon_gettext
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#నిర్వచితమైతే FEAT_BEVAL
	    f_balloon_show
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#ఒకవేళ defined(FEAT_BEVAL_పదం)
	    f_balloon_విభజించు
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
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
			ret_list_సంఖ్య,    f_blob2list},
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
			ret_సంఖ్య,	    f_bufadd},
    {"bufexists",	1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_సంఖ్య_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_బఫరు,	// obsolete
			"buf",
			ret_సంఖ్య_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_బఫరు,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_బఫరు,	// obsolete
			"buf",
			ret_సంఖ్య,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_సంఖ్య_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_సంఖ్య_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_బఫరు_bool,
			"buf, create",
			ret_సంఖ్య,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_సంఖ్య,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_సంఖ్య,	    f_bufwinnr},
    {"byte2line",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"byte",
			ret_సంఖ్య,	    f_byte2line},
    {"byteidx",		2, 3, FEARG_1,	    arg3_string_సంఖ్య_bool,
			"expr, nr, utf16",
			ret_సంఖ్య,	    f_byteidx},
    {"byteidxcomp",	2, 3, FEARG_1,	    arg3_string_సంఖ్య_bool,
			"expr, nr, utf16",
			ret_సంఖ్య,	    f_byteidxcomp},
    {"call",		2, 3, FEARG_1,	    arg3_any_list_dict,
			"func, arglist, dict",
			ret_any,	    f_call},
    {"ceil",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ceil},
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_పని,
			"handle",
			ret_సంఖ్య_bool,    పని_FUNC(f_ch_canread)},
    {"ch_close",	1, 1, FEARG_1,	    arg1_chan_or_పని,
			"handle",
			ret_void,	    పని_FUNC(f_ch_close)},
    {"ch_close_in",	1, 1, FEARG_1,	    arg1_chan_or_పని,
			"handle",
			ret_void,	    పని_FUNC(f_ch_close_లోపల)},
    {"ch_evalexpr",	2, 3, FEARG_1,	    arg23_chanexpr,
			"handle, expr, options",
			ret_any,	    పని_FUNC(f_ch_evalexpr)},
    {"ch_evalraw",	2, 3, FEARG_1,	    arg23_chanraw,
			"handle, string, options",
			ret_any,	    పని_FUNC(f_ch_evalraw)},
    {"ch_getbufnr",	2, 2, FEARG_1,	    arg2_chan_or_పని_string,
			"handle, what",
			ret_సంఖ్య,	    పని_FUNC(f_ch_getbufnr)},
    {"ch_getjob",	1, 1, FEARG_1,	    arg1_chan_or_పని,
			"channel",
			ret_పని,	    పని_FUNC(f_ch_getjob)},
    {"ch_info",		1, 1, FEARG_1,	    arg1_chan_or_పని,
			"handle",
			ret_dict_any,	    పని_FUNC(f_ch_info)},
    {"ch_listen",	1, 2, FEARG_1,	    arg2_string_dict,
			"address, options",
			ret_channel,	    పని_FUNC(f_ch_listen)},
    {"ch_log",		1, 2, FEARG_1,	    arg2_string_chan_or_పని,
			"msg, handle",
			ret_void,	    f_ch_log},
    {"ch_logfile",	1, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_void,	    f_ch_logfile},
    {"ch_open",		1, 2, FEARG_1,	    arg2_string_dict,
			"address, options",
			ret_channel,	    పని_FUNC(f_ch_open)},
    {"ch_read",		1, 2, FEARG_1,	    arg2_chan_or_పని_dict,
			"handle, options",
			ret_string,	    పని_FUNC(f_ch_read)},
    {"ch_readblob",	1, 2, FEARG_1,	    arg2_chan_or_పని_dict,
			"handle, options",
			ret_blob,	    పని_FUNC(f_ch_readblob)},
    {"ch_readraw",	1, 2, FEARG_1,	    arg2_chan_or_పని_dict,
			"handle, options",
			ret_string,	    పని_FUNC(f_ch_readraw)},
    {"ch_sendexpr",	2, 3, FEARG_1,	    arg23_chanexpr,
			"handle, expr, options",
			ret_any,	    పని_FUNC(f_ch_sendexpr)},
    {"ch_sendraw",	2, 3, FEARG_1,	    arg23_chanraw,
			"handle, expr, options",
			ret_void,	    పని_FUNC(f_ch_sendraw)},
    {"ch_setoptions",	2, 2, FEARG_1,	    arg2_chan_or_పని_dict,
			"handle, options",
			ret_void,	    పని_FUNC(f_ch_setoptions)},
    {"ch_status",	1, 2, FEARG_1,	    arg2_chan_or_పని_dict,
			"handle, options",
			ret_string,	    పని_FUNC(f_ch_స్థితి)},
    {"changenr",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_changenr},
    {"char2nr",		1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_సంఖ్య,	    f_char2nr},
    {"charclass",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_సంఖ్య,	    f_charclass},
    {"charcol",		1, 2, FEARG_1,	    arg2_string_or_list_సంఖ్య,
			"expr, winid",
			ret_సంఖ్య,	    f_charcol},
    {"charidx",		2, 4, FEARG_1,	    arg4_string_సంఖ్య_bool_bool,
			"string, idx, countcc, utf16",
			ret_సంఖ్య,	    f_charidx},
    {"chdir",		1, 2, FEARG_1,	    arg2_string,
			"dir, scope",
			ret_string,	    f_chdir},
    {"cindent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_సంఖ్య,	    f_cindent},
    {"clearmatches",	0, 1, FEARG_1,	    arg1_సంఖ్య,
			"win",
			ret_void,	    f_clearmatches},
    {"cmdcomplete_info",0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_సంఖ్య,
			"expr, winid",
			ret_సంఖ్య,	    f_నిలువు},
    {"complete",	2, 2, FEARG_2,	    arg2_సంఖ్య_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_సంఖ్య,	    f_complete_కూడు},
    {"complete_check",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_సంఖ్య_string,
			"msg, choices, default, type",
			ret_సంఖ్య,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    ఖాళీ,
			"expr",
			ret_copy,	    f_copy},
    {"cos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cos},
    {"cosh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cosh},
    {"count",		2, 4, FEARG_1,	    arg24_లెక్క,
			"comp, expr, ic, start",
			ret_సంఖ్య,	    f_లెక్క},
    {"cscope_connection",0,3, 0,	    arg3_సంఖ్య_string_string,
			"num, dbpath, prepend",
			ret_సంఖ్య,	    f_cscope_connection},
    {"cursor",		1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_సంఖ్య,	    f_cursor},
    {"debugbreak",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"pid",
			ret_సంఖ్య,
#నిర్వచితమైతే MSWIN
	    f_debugbreak
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_సంఖ్య_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_బఫరు_lnum_lnum,
			"buf, first, last",
			ret_సంఖ్య_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య_bool,    f_did_filetype},
    {"diff",		2, 3, FEARG_1,	    arg3_diff,
			"fromlist, tolist, options",
			ret_string_or_any_3,  f_diff},
    {"diff_filler",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_సంఖ్య,	    f_diff_filler},
    {"diff_hlID",	2, 2, FEARG_1,	    arg2_lnum_సంఖ్య,
			"lnum, col",
			ret_సంఖ్య,	    f_diff_hlID},
    {"digraph_get",	1, 1, FEARG_1,	    arg1_string,
			"chars",
			ret_string,	    f_digraph_get},
    {"digraph_getlist",	0, 1, FEARG_1,	    arg1_bool,
			"listall",
			ret_list_string_వస్తువులు, f_digraph_getlist},
    {"digraph_set",	2, 2, FEARG_1,	    arg2_string,
			"chars, digraph",
			ret_bool,	f_digraph_సమితి},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    ఖాళీ,
			"expr",
			ret_సంఖ్య_bool,    f_ఖాళీలు},
    {"environ",		0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    ఖాళీ,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య_bool,    f_eventhandler},
    {"executable",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_సంఖ్య,	    f_executable},
    {"execute",		1, 2, FEARG_1,	    arg12_execute,
			"command, silent",
			ret_string,	    f_execute},
    {"exepath",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_exepath},
    {"exists",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_సంఖ్య_bool,    f_exists},
    {"exists_compiled",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_సంఖ్య_bool,    f_exists_compiled},
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
			ret_మొదటి_cont,	    f_extendnew},
    {"feedkeys",	1, 2, FEARG_1,	    arg2_string,
			"string, mode",
			ret_void,	    f_feedkeys},
    {"file_readable",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"file",
			ret_సంఖ్య_bool,    f_filereadable},
    {"filecopy",	2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_సంఖ్య_bool,    f_filecopy},
    {"filereadable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_సంఖ్య_bool,    f_filereadable},
    {"filewritable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_సంఖ్య,	    f_filewritable},
    {"filter",		2, 2, FEARG_1,	    arg2_filter,
			"expr1, expr2",
			ret_మొదటి_arg,	    f_filter},
    {"finddir",		1, 3, FEARG_1,	    arg3_string_string_సంఖ్య,
			"name, path, count",
			ret_string_or_any_3,	    f_finddir},
    {"findfile",	1, 3, FEARG_1,	    arg3_string_string_సంఖ్య,
			"name, path, count",
			ret_string_or_any_3,	    f_findfile},
    {"flatten",		1, 2, FEARG_1,	    arg2_list_any_సంఖ్య,
			"list, maxdepth",
			ret_list_any,	    f_flatten},
    {"flattennew",	1, 2, FEARG_1,	    arg2_list_any_సంఖ్య,
			"list, maxdepth",
			ret_list_any,	    f_flattennew},
    {"float2nr",	1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_సంఖ్య,	    f_float2nr},
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
    {"foldclosed",	1, 2, FEARG_1,	    arg2_lnum_సంఖ్య,
			"lnum, winid",
			ret_సంఖ్య,	    f_foldclosed},
    {"foldclosedend",	1, 2, FEARG_1,	    arg2_lnum_సంఖ్య,
			"lnum, winid",
			ret_సంఖ్య,	    f_foldclosedend},
    {"foldlevel",	1, 2, FEARG_1,	    arg2_lnum_సంఖ్య,
			"lnum, winid",
			ret_సంఖ్య,	    f_foldlevel},
    {"foldtext",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_సంఖ్య,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_మొదటి_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
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
    {"getbgcolor",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_list_any,	    f_getbgcolor},
    {"getbufinfo",	0, 1, FEARG_1,	    arg1_బఫరు_or_dict_any,
			"buf",
			ret_list_dict_any,  f_getbufinfo},
    {"getbufline",	2, 3, FEARG_1,	    arg3_బఫరు_lnum_lnum,
			"buf, lnum, end",
			ret_list_string,    f_getbufline},
    {"getbufoneline",	2, 2, FEARG_1,	    arg2_బఫరు_lnum,
			"buf, lnum",
			ret_string,	    f_getbufoneline},
    {"getbufvar",	2, 3, FEARG_1,	    arg3_బఫరు_string_any,
			"buf, varname, def",
			ret_any,	    f_getbufvar},
    {"getcellpixels",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_సంఖ్య,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_getcmdwintype},
    {"getcompletion",	2, 3, FEARG_1,	    arg3_string_string_bool,
			"pat, type, filtered",
			ret_list_string,    f_getcompletion},
    {"getcompletiontype", 1, 1, FEARG_1,    arg1_string,
			"pat",
			ret_string,	    f_getcompletiontype},
    {"getcurpos",	0, 1, FEARG_1,	    arg1_సంఖ్య,
			"winid",
			ret_list_సంఖ్య,    f_getcurpos},
    {"getcursorcharpos", 0, 1, FEARG_1,	    arg1_సంఖ్య,
			"winid",
			ret_list_సంఖ్య,    f_getcursorcharpos},
    {"getcwd",		0, 2, FEARG_1,	    arg2_సంఖ్య,
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
			ret_సంఖ్య,	    f_getfsize},
    {"getftime",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_సంఖ్య,	    f_getftime},
    {"getftype",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getftype},
    {"getimstatus",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య_bool,    f_getimstatus},
    {"getinfo",		2, 3, FEARG_2,	    arg3_string_string_dict,
			"kind, name, opts",
			ret_dict_any,	    f_getinfo},
    {"getjumplist",	0, 2, FEARG_1,	    arg2_సంఖ్య,
			"winnr, tabnr",
			ret_list_any,	    f_getjumplist},
    {"getline",		1, 2, FEARG_1,	    arg2_lnum,
			"lnum, end",
			ret_getline,	    f_getline},
    {"getloclist",	1, 2, 0,	    arg2_సంఖ్య_dict_any,
			"nr, what",
			ret_list_or_dict_1, f_getloclist},
    {"getmarklist",	0, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_సంఖ్య,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_dict_సంఖ్య,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_getpid},
    {"getpos",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_సంఖ్య,    f_getpos},
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
    {"getstacktrace",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_list_dict_any,  f_getstacktrace},
    {"gettabinfo",	0, 1, FEARG_1,	    arg1_సంఖ్య,
			"tabnr",
			ret_list_dict_any,  f_gettabinfo},
    {"gettabvar",	2, 3, FEARG_1,	    arg3_సంఖ్య_string_any,
			"tabnr, varname, def",
			ret_any,	    f_gettabvar},
    {"gettabwinvar",	3, 4, FEARG_1,	    arg4_సంఖ్య_సంఖ్య_string_any,
			"tabnr, winnr, varname, def",
			ret_any,	    f_gettabwinvar},
    {"gettagstack",	0, 1, FEARG_1,	    arg1_సంఖ్య,
			"winnr",
			ret_dict_any,	    f_gettagstack},
    {"gettext",		1, 2, FEARG_1,	    arg2_string,
			"text, package",
			ret_string,	    f_gettext},
    {"getwininfo",	0, 1, FEARG_1,	    arg1_సంఖ్య,
			"winid",
			ret_list_dict_any,  f_getwininfo},
    {"getwinpos",	0, 1, FEARG_1,	    arg1_సంఖ్య,
			"timeout",
			ret_list_సంఖ్య,    f_getwinpos},
    {"getwinposx",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_getwinposy},
    {"getwinvar",	2, 3, FEARG_1,	    arg3_సంఖ్య_string_any,
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
			ret_సంఖ్య_bool,    f_has},
    {"has_key",		2, 2, FEARG_1,	    arg2_dict_any_string_or_nr,
			"dict, key",
			ret_సంఖ్య_bool,    f_has_కీలకం},
    {"haslocaldir",	0, 2, FEARG_1,	    arg2_సంఖ్య,
			"winnr, tabnr",
			ret_సంఖ్య,	    f_haslocaldir},
    {"hasmapto",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"what, mode, abbr",
			ret_సంఖ్య_bool,    f_hasmapto},
    {"highlightID",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_సంఖ్య,	    f_hlID},
    {"highlight_exists",1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_సంఖ్య_bool,    f_hlexists},
    {"histadd",		2, 2, FEARG_2,	    arg2_string,
			"history, item",
			ret_సంఖ్య_bool,    f_histadd},
    {"histdel",		1, 2, FEARG_1,	    arg2_string_string_or_సంఖ్య,
			"history, item",
			ret_సంఖ్య_bool,    f_histdel},
    {"histget",		1, 2, FEARG_1,	    arg2_string_సంఖ్య,
			"history, index",
			ret_string,	    f_histget},
    {"histnr",		1, 1, FEARG_1,	    arg1_string,
			"history",
			ret_సంఖ్య,	    f_histnr},
    {"hlID",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_సంఖ్య,	    f_hlID},
    {"hlexists",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_సంఖ్య_bool,    f_hlexists},
    {"hlget",		0, 2, FEARG_1,	    arg2_string_bool,
			"name, resolve",
			ret_list_dict_any,  f_hlget},
    {"hlset",		1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_సంఖ్య_bool,    f_hlset},
    {"hostname",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    ఖాళీ,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_సంఖ్య,	    IMAGE_FUNC(f_image_కూడు)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_సంఖ్య,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_సంఖ్య,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_సూచీ,
			"object, expr, start, ic",
			ret_సంఖ్య,	    f_సూచీ},
    {"indexof",		2, 3, FEARG_1,	    arg23_సూచీ,
			"object, expr, opts",
			ret_సంఖ్య,	    f_indexof},
    {"input",		1, 3, FEARG_1,	    arg3_string,
			"prompt, text, completion",
			ret_string,	    f_ఆదానం},
    {"inputdialog",	1, 3, FEARG_1,	    arg3_string,
			"prompt, text, cancelreturn",
			ret_string,	    f_inputdialog},
    {"inputlist",	1, 1, FEARG_1,	    arg1_list_string,
			"textlist",
			ret_సంఖ్య,	    f_inputlist},
    {"inputrestore",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_చొప్పించు,
			"object, item, idx",
			ret_మొదటి_arg,	    f_చొప్పించు},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_void,	    f_అంతరాయం},
    {"invert",		1, 1, FEARG_1,	    arg1_సంఖ్య,
			"expr",
			ret_సంఖ్య,	    f_invert},
    {"isabsolutepath",	1, 1, FEARG_1,	    arg1_string,
			"path",
			ret_సంఖ్య_bool,    f_isabsolutepath},
    {"isdirectory",	1, 1, FEARG_1,	    arg1_string,
			"directory",
			ret_సంఖ్య_bool,    f_isdirectory},
    {"isinf",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_సంఖ్య,	    MATH_FUNC(f_isinf)},
    {"islocked",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_సంఖ్య_bool,    f_islocked},
    {"isnan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_సంఖ్య_bool,    MATH_FUNC(f_isnan)},
    {"items",		1, 1, FEARG_1,	    arg1_list_tuple_dict_blob_or_string,
			"expr",
			ret_list_వస్తువులు,	    f_వస్తువులు},
    {"job_getchannel",	1, 1, FEARG_1,	    arg1_పని,
			"job",
			ret_channel,	    పని_FUNC(f_పని_getchannel)},
    {"job_info",	0, 1, FEARG_1,	    arg1_పని,
			"job",
			ret_పని_info,	    పని_FUNC(f_పని_info)},
    {"job_setoptions",	2, 2, FEARG_1,	    arg2_పని_dict,
			"job, options",
			ret_void,	    పని_FUNC(f_పని_setoptions)},
    {"job_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"command, options",
			ret_పని,	    పని_FUNC(f_పని_ప్రారంభం)},
    {"job_status",	1, 1, FEARG_1,	    arg1_పని,
			"job",
			ret_string,	    పని_FUNC(f_పని_స్థితి)},
    {"job_stop",	1, 2, FEARG_1,	    arg2_పని_string_or_సంఖ్య,
			"job, how",
			ret_సంఖ్య_bool,    పని_FUNC(f_పని_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    ఖాళీ,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    ఖాళీ,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_కీలకాలు},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    ఖాళీ,	// obsolete
			ఖాళీ,
			ret_సంఖ్య,	    f_చివరి_బఫరు_nr},
    {"len",		1, 1, FEARG_1,	    arg1_len,
			"expr",
			ret_సంఖ్య,	    f_len},
    {"libcall",		3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_string,	    f_libcall},
    {"libcallnr",	3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_సంఖ్య,	    f_libcallnr},
    {"line",		1, 2, FEARG_1,	    arg2_string_సంఖ్య,
			"expr, winid",
			ret_సంఖ్య,	    f_పంక్తి},
    {"line2byte",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_సంఖ్య,	    f_line2byte},
    {"lispindent",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_సంఖ్య,	    f_lispindent},
    {"list2blob",	1, 1, FEARG_1,	    arg1_list_సంఖ్య,
			"list",
			ret_blob,	    f_list2blob},
    {"list2str",	1, 2, FEARG_1,	    arg2_list_సంఖ్య_bool,
			"list, utf8",
			ret_string,	    f_list2str},
    {"list2tuple",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_tuple_any,	    f_list2tuple},
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_బఫరు_bool_or_dict,
			"callback, buf, options",
			ret_సంఖ్య,	    f_listener_కూడు},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"id",
			ret_సంఖ్య_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_localtime},
    {"log",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_log},
    {"log10",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_log10},
    {"luaeval",		1, 2, FEARG_1,	    arg2_string_any,
			"expr1, expr2",
			ret_any,
#నిర్వచితమైతే FEAT_LUA
		f_luaeval
#లేకపోతే
		ఖాళీ
#అయితే_ముగింపు
			},
    {"map",		2, 2, FEARG_1,	    arg2_map,
			"expr1, expr2",
			ret_మొదటి_cont,	    f_map},
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
			ret_మొదటి_cont,	    f_mapnew},
    {"mapset",		1, 3, FEARG_1,	    arg3_string_or_dict_bool_dict,
			"mode, abbr, dict",
			ret_void,	    f_mapset},
    {"match",		2, 4, FEARG_1,	    arg24_సరిపోలిక_func,
			"expr, pat, start, count",
			ret_సంఖ్య,	    f_సరిపోలిక},
    {"matchadd",	2, 5, FEARG_1,	    arg25_matchadd,
			"group, pattern, priority, id, dict",
			ret_సంఖ్య,	    f_matchadd},
    {"matchaddpos",	2, 5, FEARG_1,	    arg25_matchaddpos,
			"group, pos, priority, id, dict",
			ret_సంఖ్య,	    f_matchaddpos},
    {"matcharg",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"nr",
			ret_list_string,    f_matcharg},
    {"matchbufline",	4, 5, FEARG_1,	    arg45_matchbufline,
			"buf, pat, lnum, end, dict",
			ret_list_any,	    f_matchbufline},
    {"matchdelete",	1, 2, FEARG_1,	    arg2_సంఖ్య,
			"id, win",
			ret_సంఖ్య_bool,    f_matchdelete},
    {"matchend",	2, 4, FEARG_1,	    arg24_సరిపోలిక_func,
			"expr, pat, start, count",
			ret_సంఖ్య,	    f_matchend},
    {"matchfuzzy",	2, 3, FEARG_1,	    arg3_list_string_dict,
			"list, str, dict",
			ret_list_any,	    f_matchfuzzy},
    {"matchfuzzypos",	2, 3, FEARG_1,	    arg3_list_string_dict,
			"list, str, dict",
			ret_list_list_any,  f_matchfuzzypos},
    {"matchlist",	2, 4, FEARG_1,	    arg24_సరిపోలిక_func,
			"expr, pat, start, count",
			ret_list_string,    f_matchlist},
    {"matchstr",	2, 4, FEARG_1,	    arg24_సరిపోలిక_func,
			"expr, pat, start, count",
			ret_string,	    f_matchstr},
    {"matchstrlist",	2, 3, FEARG_1,	    arg23_matchstrlist,
			"list, pat, dict",
			ret_list_any,	    f_matchstrlist},
    {"matchstrpos",	2, 4, FEARG_1,	    arg24_సరిపోలిక_func,
			"expr, pat, start, count",
			ret_list_any,	    f_matchstrpos},
    {"max",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_max},
    {"menu_info",	1, 2, FEARG_1,	    arg2_string,
			"name, mode",
			ret_dict_any,
#నిర్వచితమైతే FEAT_MENU
	    f_menu_info
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_సంఖ్య,
			"name, flags, prot",
			ret_సంఖ్య_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_బహుళకం},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#నిర్వచితమైతే FEAT_MZSCHEME
	    f_mzeval
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"nextnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_సంఖ్య,	    f_nextnonblank},
    {"ngettext",	3, 4, FEARG_3,	    arg4_string_string_సంఖ్య_string,
			"single, plural, number, domain",
			ret_string,	    f_ngettext},
    {"nr2char",		1, 2, FEARG_1,	    arg2_సంఖ్య_bool,
			"expr, utf8",
			ret_string,	    f_nr2char},
    {"or",		2, 2, FEARG_1,	    arg2_సంఖ్య,
			"expr1, expr2",
			ret_సంఖ్య,	    f_or},
    {"pathshorten",	1, 2, FEARG_1,	    arg2_string_సంఖ్య,
			"path, len",
			ret_string,	    f_pathshorten},
    {"perleval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#నిర్వచితమైతే FEAT_PERL
	    f_perleval
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"popup_atcursor",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_సంఖ్య,	    PROP_FUNC(f_popup_atcursor)},
    {"popup_beval",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_సంఖ్య,	    PROP_FUNC(f_popup_beval)},
    {"popup_clear",	0, 1, 0,	    arg1_bool,
			"force",
			ret_void,	    PROP_FUNC(f_popup_clear)},
    {"popup_close",	1, 2, FEARG_1,	    arg2_సంఖ్య_any,
			"id, result",
			ret_void,	    PROP_FUNC(f_popup_close)},
    {"popup_create",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_సంఖ్య,	    PROP_FUNC(f_popup_create)},
    {"popup_dialog",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_సంఖ్య,	    PROP_FUNC(f_popup_dialog)},
    {"popup_filter_menu", 2, 2, 0,	    arg2_సంఖ్య_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_menu)},
    {"popup_filter_yesno", 2, 2, 0,	    arg2_సంఖ్య_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_yesno)},
    {"popup_findecho",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    PROP_FUNC(f_popup_findpreview)},
    {"popup_getoptions", 1, 1, FEARG_1,	    arg1_సంఖ్య,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getoptions)},
    {"popup_getpos",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getpos)},
    {"popup_hide",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"id",
			ret_void,	    PROP_FUNC(f_popup_hide)},
    {"popup_list",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_list_సంఖ్య,    PROP_FUNC(f_popup_list)},
    {"popup_locate",	2, 2, 0,	    arg2_సంఖ్య,
			"row, col",
			ret_సంఖ్య,	    PROP_FUNC(f_popup_locate)},
    {"popup_menu",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_సంఖ్య,	    PROP_FUNC(f_popup_menu)},
    {"popup_move",	2, 2, FEARG_1,	    arg2_సంఖ్య_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_move)},
    {"popup_notification", 2, 2, FEARG_1,   arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_సంఖ్య,	    PROP_FUNC(f_popup_notification)},
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_సంఖ్య_బఫరు,
			"id, buf",
			ret_సంఖ్య_bool,    PROP_FUNC(f_popup_setbuf)},
    {"popup_setoptions", 2, 2, FEARG_1,	    arg2_సంఖ్య_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_setoptions)},
    {"popup_settext",	2, 2, FEARG_1,	    arg2_సంఖ్య_string_or_list,
			"id, text",
			ret_void,	    PROP_FUNC(f_popup_settext)},
    {"popup_show",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"id",
			ret_సంఖ్య,	    PROP_FUNC(f_popup_show)},
    {"pow",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"x, y",
			ret_float,	    f_pow},
    {"preinserted",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_సంఖ్య,	    f_prevnonblank},
    {"printf",		1, 19, FEARG_2,	    arg119_printf,
			"fmt, expr1",
			ret_string,	    f_printf},
    {"prompt_getprompt", 1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_string,	    పని_FUNC(f_prompt_getprompt)},
    {"prompt_setcallback", 2, 2, FEARG_1,   arg2_బఫరు_any,
			"buf, expr",
			ret_void,	    పని_FUNC(f_prompt_setcallback)},
    {"prompt_setinterrupt", 2, 2, FEARG_1,  arg2_బఫరు_any,
			"buf, expr",
			ret_void,	    పని_FUNC(f_prompt_setinterrupt)},
    {"prompt_setprompt", 2, 2, FEARG_1,	    arg2_బఫరు_string,
			"buf, text",
			ret_void,	    పని_FUNC(f_prompt_setprompt)},
    {"prop_add",	3, 3, FEARG_1,	    arg3_సంఖ్య_సంఖ్య_dict,
			"lnum, col, props",
			ret_సంఖ్య,	    PROP_FUNC(f_prop_కూడు)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_కూడు_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_సంఖ్య_సంఖ్య_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_కనుగొను)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_సంఖ్య_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_సంఖ్య_సంఖ్య,
			"props, lnum, lnum_end",
			ret_సంఖ్య,	    PROP_FUNC(f_prop_remove)},
    {"prop_type_add",	2, 2, FEARG_1,	    arg2_string_dict,
			"name, props",
			ret_void,	    PROP_FUNC(f_prop_type_కూడు)},
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
    {"pum_getpos",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_dict_సంఖ్య,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#నిర్వచితమైతే FEAT_PYTHON3
	    f_py3eval
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#నిర్వచితమైతే FEAT_PYTHON
	    f_pyeval
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"rand",		0, 1, FEARG_1,	    arg1_list_సంఖ్య,
			"expr",
			ret_సంఖ్య,	    f_rand},
    {"range",		1, 3, FEARG_1,	    arg3_సంఖ్య,
			"expr, max, stride",
			ret_list_సంఖ్య,    f_range},
    {"readblob",	1, 3, FEARG_1,	    arg3_string_సంఖ్య_సంఖ్య,
			"fname, offset, size",
			ret_blob,	    f_readblob},
    {"readdir",		1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_string,    f_readdir},
    {"readdirex",	1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_dict_any,  f_readdirex},
    {"readfile",	1, 3, FEARG_1,	    arg3_string_string_సంఖ్య,
			"fname, type, max",
			ret_list_string,    f_readfile},
    {"redraw_listener_add", 1, 1, FEARG_1,  arg1_dict_any,
			"opts",
			ret_సంఖ్య,	    f_redraw_listener_కూడు},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_సంఖ్య,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_reg_recording},
    {"reltime",		0, 2, FEARG_1,	    arg2_list_సంఖ్య,
			"start, end",
			ret_list_సంఖ్య,    f_reltime},
    {"reltimefloat",	1, 1, FEARG_1,	    arg1_list_సంఖ్య,
			"time",
			ret_float,	    f_reltimefloat},
    {"reltimestr",	1, 1, FEARG_1,	    arg1_list_సంఖ్య,
			"time",
			ret_string,	    f_reltimestr},
    {"remote_expr",	2, 4, FEARG_1,	    arg24_remote_సమాసం,
			"server, string, idvar, timeout",
			ret_string,	    f_remote_సమాసం},
    {"remote_foreground", 1, 1, FEARG_1,    arg1_string,
			"server",
			ret_void,	    f_remote_foreground},
    {"remote_peek",	1, 2, FEARG_1,	    arg2_string,
			"serverid, retvar",
			ret_సంఖ్య,	    f_remote_peek},
    {"remote_read",	1, 2, FEARG_1,	    arg2_string_సంఖ్య,
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
			ret_సంఖ్య_bool,    f_rename},
    {"repeat",		2, 2, FEARG_1,	    arg2_repeat,
			"expr, count",
			ret_repeat,	    f_repeat},
    {"resolve",		1, 1, FEARG_1,	    arg1_string,
			"filename",
			ret_string,	    f_resolve},
    {"reverse",		1, 1, FEARG_1,	    arg1_తిరగేయి,
			"object",
			ret_మొదటి_arg,	    f_తిరగేయి},
    {"round",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_విడత},
    {"rubyeval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#నిర్వచితమైతే FEAT_RUBY
	    f_rubyeval
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"screenattr",	2, 2, FEARG_1,	    arg2_సంఖ్య,
			"row, col",
			ret_సంఖ్య,	    f_screenattr},
    {"screenchar",	2, 2, FEARG_1,	    arg2_సంఖ్య,
			"row, col",
			ret_సంఖ్య,	    f_screenchar},
    {"screenchars",	2, 2, FEARG_1,	    arg2_సంఖ్య,
			"row, col",
			ret_list_సంఖ్య,    f_screenchars},
    {"screencol",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_సంఖ్య,
			"winid, lnum, col",
			ret_dict_సంఖ్య,    f_screenpos},
    {"screenrow",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_screenrow},
    {"screenstring",	2, 2, FEARG_1,	    arg2_సంఖ్య,
			"row, col",
			ret_string,	    f_screenstring},
    {"search",		1, 5, FEARG_1,	    arg15_శోధన,
			"pattern, flags, stopline, timeout, skip",
			ret_సంఖ్య,	    f_శోధన},
    {"searchcount",	0, 1, FEARG_1,	    arg1_dict_any,
			"options",
			ret_dict_సంఖ్య,    f_searchcount},
    {"searchdecl",	1, 3, FEARG_1,	    arg3_string_bool_bool,
			"name, global, thisblock",
			ret_సంఖ్య_bool,    f_searchdecl},
    {"searchpair",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_సంఖ్య,	    f_searchpair},
    {"searchpairpos",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_list_సంఖ్య,    f_searchpairpos},
    {"searchpos",	1, 5, FEARG_1,	    arg15_శోధన,
			"pattern, flags, stopline, timeout, skip",
			ret_list_సంఖ్య,    f_searchpos},
    {"server2client",	2, 2, FEARG_1,	    arg2_string,
			"clientid, string",
			ret_సంఖ్య_bool,    f_server2client},
    {"serverlist",	0, 1, 0,	    arg1_dict_any,
			"dict",
			ret_string_or_any_1,	    f_serverlist},
    {"setbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_సంఖ్య_bool,    f_setbufline},
    {"setbufvar",	3, 3, FEARG_3,	    arg3_బఫరు_string_any,
			"buf, varname, val",
			ret_void,	    f_setbufvar},
    {"setcellwidths",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_void,	    f_setcellwidths},
    {"setcharpos",	2, 2, FEARG_2,	    arg2_string_list_సంఖ్య,
			"expr, list",
			ret_సంఖ్య_bool,    f_setcharpos},
    {"setcharsearch",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_setcharsearch},
    {"setcmdline",	1, 2, FEARG_1,	    arg2_string_సంఖ్య,
			"str, pos",
			ret_సంఖ్య_bool,    f_setcmdline},
    {"setcmdpos",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"pos",
			ret_సంఖ్య_bool,    f_setcmdpos},
    {"setcursorcharpos", 1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_సంఖ్య_bool,    f_setcursorcharpos},
    {"setenv",		2, 2, FEARG_2,	    arg2_string_any,
			"name, val",
			ret_void,	    f_setenv},
    {"setfperm",	2, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_సంఖ్య_bool,    f_setfperm},
    {"setline",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_సంఖ్య_bool,    f_setline},
    {"setloclist",	2, 4, FEARG_2,	    arg24_setloclist,
			"nr, list, action, what",
			ret_సంఖ్య_bool,    f_setloclist},
    {"setmatches",	1, 2, FEARG_1,	    arg2_list_any_సంఖ్య,
			"list, win",
			ret_సంఖ్య_bool,    f_setmatches},
    {"setpos",		2, 2, FEARG_2,	    arg2_string_list_సంఖ్య,
			"expr, list",
			ret_సంఖ్య_bool,    f_setpos},
    {"setqflist",	1, 3, FEARG_1,	    arg13_setqflist,
			"list, action, what",
			ret_సంఖ్య_bool,    f_setqflist},
    {"setreg",		2, 3, FEARG_2,	    arg3_string_any_string,
			"regname, value, options",
			ret_సంఖ్య_bool,    f_setreg},
    {"settabvar",	3, 3, FEARG_3,	    arg3_సంఖ్య_string_any,
			"tabnr, varname, val",
			ret_void,	    f_settabvar},
    {"settabwinvar",	4, 4, FEARG_4,	    arg4_సంఖ్య_సంఖ్య_string_any,
			"tabnr, winnr, varname, val",
			ret_void,	    f_settabwinvar},
    {"settagstack",	2, 3, FEARG_2,	    arg23_settagstack,
			"nr, dict, action",
			ret_సంఖ్య_bool,    f_settagstack},
    {"setwinvar",	3, 3, FEARG_3,	    arg3_సంఖ్య_string_any,
			"winnr, varname, val",
			ret_void,	    f_setwinvar},
    {"sha256",		1, 1, FEARG_1,	    arg1_string_or_blob,
			"expr",
			ret_string,
#నిర్వచితమైతే FEAT_CRYPT
	    f_sha256
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"shellescape",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, special",
			ret_string,	    f_shellescape},
    {"shiftwidth",	0, 1, FEARG_1,	    arg1_సంఖ్య,
			"col",
			ret_సంఖ్య,	    f_shiftwidth},
    {"sign_define",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"name, dict",
			ret_sign_define,	    SIGN_FUNC(f_sign_define)},
    {"sign_getdefined",	0, 1, FEARG_1,	    arg1_string,
			"name",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getdefined)},
    {"sign_getplaced",	0, 2, FEARG_1,	    arg02_sign_getplaced,
			"buf, dict",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getplaced)},
    {"sign_jump",	3, 3, FEARG_1,	    arg3_సంఖ్య_string_బఫరు,
			"id, group, buf",
			ret_సంఖ్య,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_ఉంచు,
			"id, group, name, buf, dict",
			ret_సంఖ్య,	    SIGN_FUNC(f_sign_ఉంచు)},
    {"sign_placelist",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_సంఖ్య,    SIGN_FUNC(f_sign_placelist)},
    {"sign_undefine",	0, 1, FEARG_1,	    arg1_string_or_list_string,
			"name",
			ret_సంఖ్య_bool,    SIGN_FUNC(f_sign_undefine)},
    {"sign_unplace",	1, 2, FEARG_1,	    arg2_string_dict,
			"group, dict",
			ret_సంఖ్య_bool,    SIGN_FUNC(f_sign_unplace)},
    {"sign_unplacelist", 1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_సంఖ్య,    SIGN_FUNC(f_sign_unplacelist)},
    {"simplify",	1, 1, FEARG_1,	    arg1_string,
			"filename",
			ret_string,	    f_simplify},
    {"sin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sin},
    {"sinh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sinh},
    {"slice",		2, 3, FEARG_1,	    arg23_ముక్క,
			"expr, start, end",
			ret_ముక్క,	    f_ముక్క},
    {"sort",		1, 3, FEARG_1,	    arg13_sortuniq,
			"list, how, dict",
			ret_మొదటి_arg,	    f_క్రమీకరించు},
    {"sound_clear",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_void,	    SOUND_FUNC(f_sound_clear)},
    {"sound_playevent",	1, 2, FEARG_1,	    arg2_string_any,
			"name, callback",
			ret_సంఖ్య,	    SOUND_FUNC(f_sound_playevent)},
    {"sound_playfile",	1, 2, FEARG_1,	    arg2_string_any,
			"path, callback",
			ret_సంఖ్య,	    SOUND_FUNC(f_sound_playfile)},
    {"sound_stop",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"id",
			ret_void,	    SOUND_FUNC(f_sound_stop)},
    {"soundfold",	1, 1, FEARG_1,	    arg1_string,
			"word",
			ret_string,	    f_soundfold},
    {"spellbadword",	0, 1, FEARG_1,	    arg1_string,
			"sentence",
			ret_list_string,    f_spellbadword},
    {"spellsuggest",	1, 3, FEARG_1,	    arg3_string_సంఖ్య_bool,
			"word, max, capital",
			ret_list_string,    f_spellsuggest},
    {"split",		1, 3, FEARG_1,	    arg3_string_string_bool,
			"string, pattern, keepempty",
			ret_list_string,    f_విభజించు},
    {"sqrt",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sqrt},
    {"srand",		0, 1, FEARG_1,	    arg1_సంఖ్య,
			"expr",
			ret_list_సంఖ్య,    f_srand},
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
			ret_list_సంఖ్య,    f_str2list},
    {"str2nr",		1, 3, FEARG_1,	    arg3_string_సంఖ్య_bool,
			"string, base, quoted",
			ret_సంఖ్య,	    f_str2nr},
    {"strcharlen",	1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_సంఖ్య,	    f_strcharlen},
    {"strcharpart",	2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, skipcc",
			ret_string,	    f_strcharpart},
    {"strchars",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, skipcc",
			ret_సంఖ్య,	    f_strchars},
    {"strdisplaywidth",	1, 2, FEARG_1,	    arg2_string_సంఖ్య,
			"string, col",
			ret_సంఖ్య,	    f_strdisplaywidth},
    {"strftime",	1, 2, FEARG_1,	    arg2_string_సంఖ్య,
			"format, time",
			ret_string,
#నిర్వచితమైతే HAVE_STRFTIME
	    f_strftime
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_సంఖ్య,
			"str, index",
			ret_సంఖ్య,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_సంఖ్య,
			"haystack, needle, start",
			ret_సంఖ్య,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, ఖాళీ,
			"expr",
			ret_string,	    f_string},
    {"strlen",		1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_సంఖ్య,	    f_strlen},
    {"strpart",		2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, chars",
			ret_string,	    f_strpart},
    {"strptime",	2, 2, FEARG_1,	    arg2_string,
			"format, timestring",
			ret_సంఖ్య,
#నిర్వచితమైతే HAVE_STRPTIME
	    f_strptime
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"strridx",		2, 3, FEARG_1,	    arg3_string_string_సంఖ్య,
			"haystack, needle, start",
			ret_సంఖ్య,	    f_strridx},
    {"strtrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_strtrans},
    {"strutf16len",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, countcc",
			ret_సంఖ్య,	    f_strutf16len},
    {"strwidth",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_సంఖ్య,	    f_strwidth},
    {"submatch",	1, 2, FEARG_1,	    arg2_సంఖ్య_bool,
			"nr, list",
			ret_string_or_any_2,	    f_submatch},
    {"substitute",	4, 4, FEARG_1,	    arg4_string_string_any_string,
			"string, pat, sub, flags",
			ret_string,	    f_substitute},
    {"swapfilelist",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_string,	    f_swapname},
    {"synID",		3, 3, 0,	    arg3_lnum_సంఖ్య_bool,
			"lnum, col, trans",
			ret_సంఖ్య,	    f_synID},
    {"synIDattr",	2, 3, FEARG_1,	    arg3_సంఖ్య_string_string,
			"synID, what, mode",
			ret_string,	    f_synIDattr},
    {"synIDtrans",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"synID",
			ret_సంఖ్య,	    f_synIDtrans},
    {"synconcealed",	2, 2, 0,	    arg2_lnum_సంఖ్య,
			"lnum, col",
			ret_list_any,	    f_synconcealed},
    {"synstack",	2, 2, 0,	    arg2_lnum_సంఖ్య,
			"lnum, col",
			ret_list_సంఖ్య,    f_synstack},
    {"system",		1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_string,	    f_system},
    {"systemlist",	1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_list_string,    f_systemlist},
    {"tabpagebuflist",	0, 1, FEARG_1,	    arg1_సంఖ్య,
			"arg",
			ret_list_సంఖ్య,    f_tabpagebuflist},
    {"tabpagenr",	0, 1, 0,	    arg1_string,
			"arg",
			ret_సంఖ్య,	    f_tabpagenr},
    {"tabpagewinnr",	1, 2, FEARG_1,	    arg2_సంఖ్య_string,
			"tabarg, arg",
			ret_సంఖ్య,	    f_tabpagewinnr},
    {"tabpanel_getinfo", 0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_సంఖ్య_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
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
    {"tempname",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_సంఖ్య,	    పదం_FUNC(f_పదం_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_సంఖ్య,	    పదం_FUNC(f_పదం_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_బఫరు_string_dict,
			"buf, filename, options",
			ret_void,	    పదం_FUNC(f_పదం_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_బఫరు,
			"buf",
			ret_సంఖ్య,	    పదం_FUNC(f_పదం_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_బఫరు,
			"buf",
			ret_list_string,
#ఒకవేళ defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_పదం_getansicolors
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_సంఖ్య_string,
			"attr, what",
			ret_సంఖ్య,	    పదం_FUNC(f_పదం_getattr)},
    {"term_getcursor",	1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_list_any,	    పదం_FUNC(f_పదం_getcursor)},
    {"term_getjob",	1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_పని,	    పదం_FUNC(f_పదం_getjob)},
    {"term_getline",	2, 2, FEARG_1,	    arg2_బఫరు_lnum,
			"buf, row",
			ret_string,	    పదం_FUNC(f_పదం_getline)},
    {"term_getscrolled", 1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_సంఖ్య,	    పదం_FUNC(f_పదం_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_list_సంఖ్య,    పదం_FUNC(f_పదం_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_string,	    పదం_FUNC(f_పదం_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_string,	    పదం_FUNC(f_పదం_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_బఫరు_bool,
			"buf, input",
			ret_string,	    పదం_FUNC(f_పదం_gettty)},
    {"term_list",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_list_సంఖ్య,    పదం_FUNC(f_పదం_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_బఫరు_lnum,
			"buf, row",
			ret_list_dict_any,  పదం_FUNC(f_పదం_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_బఫరు_string,
			"buf, keys",
			ret_void,	    పదం_FUNC(f_పదం_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_బఫరు_list_any,
			"buf, colors",
			ret_void,
#ఒకవేళ defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_పదం_setansicolors
#లేకపోతే
	    ఖాళీ
#అయితే_ముగింపు
			},
    {"term_setapi",	2, 2, FEARG_1,	    arg2_బఫరు_string,
			"buf, expr",
			ret_void,	    పదం_FUNC(f_పదం_setapi)},
    {"term_setkill",	2, 2, FEARG_1,	    arg2_బఫరు_string,
			"buf, how",
			ret_void,	    పదం_FUNC(f_పదం_setkill)},
    {"term_setrestore",	2, 2, FEARG_1,	    arg2_బఫరు_string,
			"buf, command",
			ret_void,	    పదం_FUNC(f_పదం_setrestore)},
    {"term_setsize",	3, 3, FEARG_1,	    arg3_బఫరు_సంఖ్య_సంఖ్య,
			"buf, rows, cols",
			ret_void,	    పదం_FUNC(f_పదం_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_సంఖ్య,	    పదం_FUNC(f_పదం_ప్రారంభం)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_బఫరు_సంఖ్య,
			"buf, time",
			ret_void,	    పదం_FUNC(f_పదం_wait)},
    {"terminalprops",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_సంఖ్య,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    ఖాళీ,
			ఖాళీ,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   ఖాళీ,
			ఖాళీ,
			ret_void,	    f_test_garbagecollect_soon},
    {"test_getvalue",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_సంఖ్య,	    f_test_getvalue},
    {"test_gui_event",	2, 2, FEARG_1,	    arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_gui_event},
    {"test_ignore_error", 1, 1, FEARG_1,    arg1_string,
			"expr",
			ret_void,	    f_test_ignore_error},
    {"test_mswin_event", 2, 2, FEARG_1,     arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_mswin_event},
    {"test_null_blob",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_channel,	    పని_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_పని,	    పని_FUNC(f_test_null_పని)},
    {"test_null_list",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_సమితి},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_సంఖ్య,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, ఖాళీ,
			"expr",
			ret_సంఖ్య,	    f_test_refcount},
    {"test_setmouse",	2, 2, 0,	    arg2_సంఖ్య,
			"row, col",
			ret_void,	    f_test_setmouse},
    {"test_settime",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"expr",
			ret_void,	    f_test_settime},
    {"test_srand_seed",	0, 1, FEARG_1,	    arg1_సంఖ్య,
			"seed",
			ret_void,	    f_test_srand_విత్తనం},
    {"test_unknown",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_సంఖ్య,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_సంఖ్య_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_సంఖ్య_any_dict,
			"time, callback, options",
			ret_సంఖ్య,	    TIMER_FUNC(f_timer_ప్రారంభం)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
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
    {"trim",		1, 3, FEARG_1,	    arg3_string_string_సంఖ్య,
			"text, mask, dir",
			ret_string,	    f_trim},
    {"trunc",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_trunc},
    {"tuple2list",	1, 1, FEARG_1,	    arg1_tuple_any,
			"tuple",
			ret_list_any,	    f_tuple2list},
    {"type",		1, 1, FEARG_1|FE_X, ఖాళీ,
			"expr",
			ret_సంఖ్య,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, ఖాళీ,
			"expr",
			ret_string,	    f_typename},
    {"undofile",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_string,	    f_undofile},
    {"undotree",	0, 1, FEARG_1,	    arg1_బఫరు,
			"buf",
			ret_dict_any,	    f_undotree},
    {"uniq",		1, 3, FEARG_1,	    arg13_sortuniq,
			"list, func, dict",
			ret_మొదటి_arg,	    f_uniq},
    {"uri_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_uridecode},
    {"uri_encode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_uriencode},
    {"utf16idx",	2, 4, FEARG_1,	    arg4_string_సంఖ్య_bool_bool,
			"string, idx, countcc, charidx",
			ret_సంఖ్య,	    f_utf16idx},
    {"values",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_member,    f_విలువలు},
    {"virtcol",		1, 3, FEARG_1,	    arg3_string_or_list_bool_సంఖ్య,
			"expr, list, winid",
			ret_virtcol,	    f_virtcol},
    {"virtcol2col",	3, 3, FEARG_1,	    arg3_సంఖ్య,
			"winid, lnum, col",
			ret_సంఖ్య,	    f_virtcol2col},
    {"visualmode",	0, 1, 0,	    arg1_bool,
			"expr",
			ret_string,	    f_visualmode},
    {"wildmenumode",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_void,	    f_wildtrigger},
    {"win_execute",	2, 3, FEARG_2,	    arg23_win_execute,
			"id, command, silent",
			ret_string,	    f_win_execute},
    {"win_findbuf",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"bufnr",
			ret_list_సంఖ్య,    f_win_findbuf},
    {"win_getid",	0, 2, FEARG_1,	    arg2_సంఖ్య,
			"win, tab",
			ret_సంఖ్య,	    f_win_getid},
    {"win_gettype",	0, 1, FEARG_1,	    arg1_సంఖ్య,
			"nr",
			ret_string,	    f_win_gettype},
    {"win_gotoid",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"expr",
			ret_సంఖ్య_bool,    f_win_gotoid},
    {"win_id2tabwin",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"expr",
			ret_list_సంఖ్య,    f_win_id2tabwin},
    {"win_id2win",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"expr",
			ret_సంఖ్య,	    f_win_id2win},
    {"win_move_separator", 2, 2, FEARG_1,   arg2_సంఖ్య,
			"nr, offset",
			ret_సంఖ్య_bool,    f_win_move_separator},
    {"win_move_statusline", 2, 2, FEARG_1,  arg2_సంఖ్య,
			"nr, offset",
			ret_సంఖ్య_bool,    f_win_move_statusline},
    {"win_screenpos",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"nr",
			ret_list_సంఖ్య,    f_win_screenpos},
    {"win_splitmove",   2, 3, FEARG_1,	    arg3_సంఖ్య_సంఖ్య_dict,
			"nr, target, options",
			ret_సంఖ్య_bool,    f_win_splitmove},
    {"winbufnr",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"nr",
			ret_సంఖ్య,	    f_winbufnr},
    {"wincol",		0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"nr",
			ret_సంఖ్య,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_సంఖ్య,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_సంఖ్య,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_సంఖ్య,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_dict_సంఖ్య,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_సంఖ్య,
			"nr",
			ret_సంఖ్య,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    ఖాళీ,
			ఖాళీ,
			ret_dict_సంఖ్య,    f_wordcount},
    {"writefile",	2, 3, FEARG_1,	    arg23_writefile,
			"object, fname, flags",
			ret_సంఖ్య_bool,    f_writefile},
    {"xor",		2, 2, FEARG_1,	    arg2_సంఖ్య,
			"expr1, expr2",
			ret_సంఖ్య,	    f_xor},
};

/*
 * Return true if specified function allows a type as an argument.
 */
    స్థిర పూర్ణాంకం
func_allows_type(పూర్ణాంకం idx)
{
    తిరిగి (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, పూర్ణాంకం idx)
{
    స్థిర పూర్ణాంకం	intidx = -1;
    char_u	*name;

    ఒకవేళ (idx == 0)
	intidx = -1;
    ఒకవేళ (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	ఒకవేళ (name != ఖాళీ)
	{
	    ఒకవేళ (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_నమూనా, 2) == 0)
		తిరిగి cat_ఉపసర్గ_varname('g', name);
	    తిరిగి name;
	}
    }
    ఒకవేళ (++intidx < (పూర్ణాంకం)ARRAY_నిడివి(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	ఒకవేళ (global_functions[intidx].f_func == ఖాళీ)
	    తిరిగి (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	ఒకవేళ (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	తిరిగి IObuff;
    }

    తిరిగి ఖాళీ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_సమాసం_name(expand_T *xp, పూర్ణాంకం idx)
{
    స్థిర పూర్ణాంకం	intidx = -1;
    char_u	*name;

    ఒకవేళ (idx == 0)
	intidx = -1;
    ఒకవేళ (intidx < 0)
    {
	name = get_function_name(xp, idx);
	ఒకవేళ (name != ఖాళీ)
	    తిరిగి name;
    }
    తిరిగి get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    స్థిర పూర్ణాంకం
కనుగొను_అంతర్గత_func_opt(char_u *name, పూర్ణాంకం implemented)
{
    పూర్ణాంకం		మొదటి = 0;
    పూర్ణాంకం		చివరి;
    పూర్ణాంకం		cmp;
    పూర్ణాంకం		x;

    చివరి = (పూర్ణాంకం)ARRAY_నిడివి(global_functions) - 1;

    // Find the function name in the table. Binary search.
    అయితే (మొదటి <= చివరి)
    {
	x = మొదటి + ((గుర్తులేని)(చివరి - మొదటి) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	ఒకవేళ (cmp < 0)
	    చివరి = x - 1;
	లేకపోతే ఒకవేళ (cmp > 0)
	    మొదటి = x + 1;
	లేకపోతే ఒకవేళ (implemented && global_functions[x].f_func == ఖాళీ)
	    ఆపు;
	లేకపోతే
	    తిరిగి x;
    }
    తిరిగి -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    పూర్ణాంకం
కనుగొను_అంతర్గత_func(char_u *name)
{
    తిరిగి కనుగొను_అంతర్గత_func_opt(name, TRUE);
}

    పూర్ణాంకం
has_అంతర్గత_func(char_u *name)
{
    తిరిగి కనుగొను_అంతర్గత_func_opt(name, TRUE) >= 0;
}

    స్థిర పూర్ణాంకం
has_అంతర్గత_func_name(char_u *name)
{
    తిరిగి కనుగొను_అంతర్గత_func_opt(name, FALSE) >= 0;
}

    అక్షరం *
అంతర్గత_func_name(పూర్ణాంకం idx)
{
    తిరిగి global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    పూర్ణాంకం
అంతర్గత_func_check_arg_types(
	type2_T	*types,
	పూర్ణాంకం	idx,
	పూర్ణాంకం	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    ఒకవేళ (!(func_allows_type(idx)))
    {
	కొరకు (పూర్ణాంకం i = 0; i < argcount; ++i)
	    ఒకవేళ (check_type_is_విలువ(types[i].type_curr) == FAIL)
		తిరిగి FAIL;
    }

    స్థిరరాశి argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    ఒకవేళ (argchecks == ఖాళీ)
	తిరిగి OK;

    argcontext_T context;

    context.arg_లెక్క = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    కొరకు (పూర్ణాంకం i = 0; i < argcount && argchecks[i] != ఖాళీ; ++i)
    {
	context.arg_idx = i;
	ఒకవేళ (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    శూన్య
అంతర్గత_func_get_argcount(పూర్ణాంకం idx, పూర్ణాంకం *argcount, పూర్ణాంకం *min_argcount)
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
అంతర్గత_func_ret_type(
	పూర్ణాంకం	    idx,
	పూర్ణాంకం	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = ఖాళీ;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    ఒకవేళ (*decl_type == ఖాళీ)
	*decl_type = ret;
    current_type_gap = ఖాళీ;
    తిరిగి ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    పూర్ణాంకం
అంతర్గత_func_is_map(పూర్ణాంకం idx)
{
    తిరిగి global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    పూర్ణాంకం
check_అంతర్గత_func(పూర్ణాంకం idx, పూర్ణాంకం argcount)
{
    funcerror_T	    res;
    అక్షరం	    *name;

    ఒకవేళ (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    లేకపోతే ఒకవేళ (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    లేకపోతే
	తిరిగి global_functions[idx].f_argtype & FEARG_ముసుగు;

    name = అంతర్గత_func_name(idx);
    ఒకవేళ (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    లేకపోతే
	semsg(_(e_not_enough_arguments_for_function_str), name);
    తిరిగి -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    స్థిర పూర్ణాంకం
check_వాదనలు_for_type(పూర్ణాంకం idx, పూర్ణాంకం argcount, typval_T *argvars)
{
    ఒకవేళ (!func_allows_type(idx))
    {
	కొరకు (పూర్ణాంకం i = 0; i < argcount; ++i)
	    ఒకవేళ (check_typval_is_విలువ(&argvars[i]) == FAIL)
		తిరిగి FAIL;
    }
    తిరిగి OK;
}

    funcerror_T
call_అంతర్గత_func(
	char_u	    *name,
	పూర్ణాంకం	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    పూర్ణాంకం i;

    i = కనుగొను_అంతర్గత_func(name);
    ఒకవేళ (i < 0)
	తిరిగి FCERR_UNKNOWN;
    ఒకవేళ (argcount < global_functions[i].f_min_argc)
	తిరిగి FCERR_TOOFEW;
    ఒకవేళ (argcount > global_functions[i].f_max_argc)
	తిరిగి FCERR_TOOMANY;
    ఒకవేళ (check_వాదనలు_for_type(i, argcount, argvars) == FAIL)
	తిరిగి FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    ఒకవేళ (లోపల_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    తిరిగి FCERR_NONE;
}

    శూన్య
call_అంతర్గత_func_by_idx(
	పూర్ణాంకం	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    ఒకవేళ (లోపల_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_అంతర్గత_పద్ధతి(
	char_u	    *name,
	పూర్ణాంకం	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    పూర్ణాంకం		fi;
    typval_T	argv[MAX_FUNC_వాదనలు + 1];

    fi = కనుగొను_అంతర్గత_func(name);
    ఒకవేళ (fi < 0)
	తిరిగి FCERR_UNKNOWN;
    ఒకవేళ ((global_functions[fi].f_argtype & FEARG_ముసుగు) == 0)
	తిరిగి FCERR_NOTMETHOD;
    ఒకవేళ (argcount + 1 < global_functions[fi].f_min_argc)
	తిరిగి FCERR_TOOFEW;
    ఒకవేళ (argcount + 1 > global_functions[fi].f_max_argc)
	తిరిగి FCERR_TOOMANY;
    ఒకవేళ (check_వాదనలు_for_type(fi, argcount, argvars) == FAIL)
	తిరిగి FCERR_OTHER;

    ఒకవేళ ((global_functions[fi].f_argtype & FEARG_ముసుగు) == FEARG_2)
    {
	ఒకవేళ (argcount < 1)
	    తిరిగి FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	కొరకు (పూర్ణాంకం i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    లేకపోతే ఒకవేళ ((global_functions[fi].f_argtype & FEARG_ముసుగు) == FEARG_3)
    {
	ఒకవేళ (argcount < 2)
	    తిరిగి FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	కొరకు (పూర్ణాంకం i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    లేకపోతే ఒకవేళ ((global_functions[fi].f_argtype & FEARG_ముసుగు) == FEARG_4)
    {
	ఒకవేళ (argcount < 3)
	    తిరిగి FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	కొరకు (పూర్ణాంకం i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    లేకపోతే
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	కొరకు (పూర్ణాంకం i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    ఒకవేళ (check_వాదనలు_for_type(fi, argcount + 1, argv) == FAIL)
	తిరిగి FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    తిరిగి FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    పూర్ణాంకం
non_zero_arg(typval_T *argvars)
{
    తిరిగి ((argvars[0].v_type == VAR_సంఖ్య
		&& argvars[0].vval.v_సంఖ్య != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_సంఖ్య == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != ఖాళీ
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    స్థిర శూన్య
f_and(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    rettv->vval.v_సంఖ్య = tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ)
					& tv_get_సంఖ్య_chk(&argvars[1], ఖాళీ);
}

/*
 * "balloon_show()" function
 */
#నిర్వచితమైతే FEAT_BEVAL
    స్థిర శూన్య
f_balloon_gettext(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    ఒకవేళ (balloonEval == ఖాళీ)
	తిరిగి;

    ఒకవేళ (balloonEval->సందేశం == ఖాళీ)
	rettv->vval.v_string = ఖాళీ;
    లేకపోతే
	rettv->vval.v_string = vim_strsave(balloonEval->సందేశం);
}

    స్థిర శూన్య
f_balloon_show(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (balloonEval == ఖాళీ)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.లోపల_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, ఖాళీ,
		l == ఖాళీ || l->lv_len == 0 ? ఖాళీ : l);
    }
    లేకపోతే
    {
	char_u *mesg;

	ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    తిరిగి;

	mesg = tv_get_string_chk(&argvars[0]);
	ఒకవేళ (mesg != ఖాళీ)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? ఖాళీ : mesg, ఖాళీ);
    }
}

# ఒకవేళ defined(FEAT_BEVAL_పదం)
    స్థిర శూన్య
f_balloon_విభజించు(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (rettv_list_alloc(rettv) != OK)
	తిరిగి;

    char_u *సందేశం;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;
    సందేశం = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (సందేశం != ఖాళీ)
    {
	pumitem_T	*array;
	పూర్ణాంకం		కొలత = విభజించు_సందేశం(సందేశం, &array);

	// Skip the first and last item, they are always empty.
	కొరకు (పూర్ణాంకం i = 1; i < కొలత - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_పాఠ్యం, -1);
	అయితే (కొలత > 0)
	    vim_free(array[--కొలత].pum_పాఠ్యం);
	vim_free(array);
    }
}
# endif
#అయితే_ముగింపు

/*
 * "base64_decode(string)" function
 */
    స్థిర శూన్య
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (rettv_blob_alloc(rettv) == FAIL)
	తిరిగి;

    char_u *str = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (str != ఖాళీ)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    స్థిర శూన్య
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (check_for_blob_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    blob_T *blob = argvars->vval.v_blob;
    ఒకవేళ (blob != ఖాళీ)
	rettv->vval.v_string =
	    base64_encode(blob->bv_ga.ga_దత్తాంశం, blob->bv_ga.ga_len);
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
    ఒకవేళ (buf == ఖాళీ)
	semsg(_(e_invalid_బఫరు_name_str), tv_get_string(arg));
    తిరిగి buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    స్థిర శూన్య
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_సంఖ్య = VVAL_TRUE;

    ఒకవేళ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	తిరిగి;

    ఒకవేళ (strcmp((స్థిరరాశి అక్షరం *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_వాదం_str), tv_get_string(&argvars[0]));
    లేకపోతే
    {
	ఒకవేళ (bindtextdomain((స్థిరరాశి అక్షరం *)argvars[0].vval.v_string, (స్థిరరాశి అక్షరం *)argvars[1].vval.v_string) == ఖాళీ)
	{
	    do_outofmem_సందేశం((దీర్ఘ)0);
	    rettv->vval.v_సంఖ్య = VVAL_FALSE;
	}
    }

    తిరిగి;
}

/*
 * "byte2line(byte)" function
 */
    స్థిర శూన్య
f_byte2line(typval_T *argvars వాడని, typval_T *rettv)
{
#నిర్వచితం_కాకపోతే FEAT_BYTEOFF
    rettv->vval.v_సంఖ్య = -1;
#లేకపోతే
    దీర్ఘ	boff = 0;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    boff = tv_get_సంఖ్య(&argvars[0]) - 1;  // boff gets -1 on type error
    ఒకవేళ (boff < 0)
	rettv->vval.v_సంఖ్య = -1;
    లేకపోతే
	rettv->vval.v_సంఖ్య = ml_కనుగొను_పంక్తి_or_స్థానభ్రంశం(curbuf,
							  (linenr_T)0, &boff);
#అయితే_ముగింపు
}

/*
 * "call(func, arglist [, dict])" function
 */
    స్థిర శూన్య
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = ఖాళీ;
    dict_T	*selfdict = ఖాళీ;
    char_u	*tofree = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	తిరిగి;

    ఒకవేళ (check_for_list_arg(argvars, 1) == FAIL)
	తిరిగి;
    ఒకవేళ (argvars[1].vval.v_list == ఖాళీ)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    లేకపోతే
	func = tv_get_string(&argvars[0]);
    ఒకవేళ (func == ఖాళీ || *func == NUL)
	తిరిగి;		// type error, empty name or null function

    ఒకవేళ (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, ఖాళీ, FALSE, TFN_INT|TFN_QUIET);
	ఒకవేళ (tofree == ఖాళీ)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    తిరిగి;
	}
	ఒకవేళ (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_వాదనలు(tofree, STRLEN(tofree),
									&p);
	    ఒకవేళ (s != ఖాళీ)
	    {
		vim_free(tofree);
		tofree = s;
	    }
	}
	func = tofree;
    }

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ఒకవేళ (check_for_dict_arg(argvars, 2) == FAIL)
	    వెళ్ళు పూర్తి;

	selfdict = argvars[2].vval.v_dict;
    }

    (శూన్య)func_call(func, &argvars[1], partial, selfdict, rettv);

పూర్తి:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    స్థిర శూన్య
f_changenr(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    స్థిర శూన్య
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (has_mbyte)
    {
	పూర్ణాంకం	utf8 = 0;

	ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (పూర్ణాంకం)tv_get_bool_chk(&argvars[1], ఖాళీ);

	ఒకవేళ (utf8)
	    rettv->vval.v_సంఖ్య = utf_ptr2char(tv_get_string(&argvars[0]));
	లేకపోతే
	    rettv->vval.v_సంఖ్య = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    లేకపోతే
	rettv->vval.v_సంఖ్య = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    స్థిర శూన్య
get_నిలువు(typval_T *argvars, typval_T *rettv, పూర్ణాంకం charcol)
{
    colnr_T	నిలువు = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    పూర్ణాంకం		winchanged = FALSE;

    ఒకవేళ (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((పూర్ణాంకం)tv_get_సంఖ్య(&argvars[1]), &tp);
	ఒకవేళ (wp == ఖాళీ || tp == ఖాళీ)
	    తిరిగి;

	ఒకవేళ (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    తిరిగి;

	check_cursor();
	winchanged = TRUE;
    }

    పూర్ణాంకం fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    ఒకవేళ (fp != ఖాళీ && fnum == curbuf->b_fnum)
    {
	ఒకవేళ (fp->నిలువు == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    ఒకవేళ (fp->lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క)
		నిలువు = ml_get_len(fp->lnum) + 1;
	    లేకపోతే
		నిలువు = MAXCOL;
	}
	లేకపోతే
	{
	    నిలువు = fp->నిలువు + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    ఒకవేళ (virtual_చురుకైన() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		ఒకవేళ (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    పూర్ణాంకం		l;

		    ఒకవేళ (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			నిలువు += l;
		}
	    }
	}
    }
    rettv->vval.v_సంఖ్య = నిలువు;

    ఒకవేళ (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    స్థిర శూన్య
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_నిలువు(argvars, rettv, TRUE);
}

    win_T *
get_optional_కిటికీ(typval_T *argvars, పూర్ణాంకం idx)
{
    win_T   *win = curwin;

    ఒకవేళ (argvars[idx].v_type == VAR_UNKNOWN)
	తిరిగి curwin;

    win = కనుగొను_win_by_nr_or_id(&argvars[idx]);
    ఒకవేళ (win == ఖాళీ)
    {
	emsg(_(e_invalid_కిటికీ_సంఖ్య));
	తిరిగి ఖాళీ;
    }
    తిరిగి win;
}

/*
 * "col(string)" function
 */
    స్థిర శూన్య
f_నిలువు(typval_T *argvars, typval_T *rettv)
{
    get_నిలువు(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    స్థిర శూన్య
f_confirm(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*సందేశం_2;
    char_u	*buttons = ఖాళీ;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    పూర్ణాంకం		def = 1;
    పూర్ణాంకం		type = VIM_GENERIC;
    char_u	*typestr;
    పూర్ణాంకం		error = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	తిరిగి;

    సందేశం_2 = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (సందేశం_2 == ఖాళీ)
	error = TRUE;
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	ఒకవేళ (buttons == ఖాళీ)
	    error = TRUE;
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[2], &error);
	    ఒకవేళ (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		ఒకవేళ (typestr == ఖాళీ)
		    error = TRUE;
		లేకపోతే
		{
		    ఎంపిక (TOUPPER_ASC(*typestr))
		    {
			స్థితి 'E': type = VIM_ERROR; ఆపు;
			స్థితి 'Q': type = VIM_QUESTION; ఆపు;
			స్థితి 'I': type = VIM_INFO; ఆపు;
			స్థితి 'W': type = VIM_WARNING; ఆపు;
			స్థితి 'G': type = VIM_GENERIC; ఆపు;
		    }
		}
	    }
	}
    }

    ఒకవేళ (buttons == ఖాళీ || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    ఒకవేళ (!error)
	rettv->vval.v_సంఖ్య = do_dialog(type, ఖాళీ, సందేశం_2, buttons,
							    def, ఖాళీ, FALSE);
#అయితే_ముగింపు
}

/*
 * "copy()" function
 */
    స్థిర శూన్య
f_copy(typval_T *argvars, typval_T *rettv)
{
    అంశం_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    స్థిర శూన్య
సమితి_cursorpos(typval_T *argvars, typval_T *rettv, పూర్ణాంకం charcol)
{
    దీర్ఘ	lnum, నిలువు;
    దీర్ఘ	coladd = 0;
    పూర్ణాంకం		సమితి_curswant = TRUE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_or_సంఖ్య_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL)))
	తిరిగి;

    rettv->vval.v_సంఖ్య = -1;
    ఒకవేళ (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	ఒకవేళ (list2fpos(argvars, &pos, ఖాళీ, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_వాదం));
	    తిరిగి;
	}
	lnum = pos.lnum;
	నిలువు = pos.నిలువు;
	coladd = pos.coladd;
	ఒకవేళ (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    సమితి_curswant = FALSE;
	}
    }
    లేకపోతే ఒకవేళ ((argvars[0].v_type == VAR_సంఖ్య
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_సంఖ్య
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	ఒకవేళ (lnum < 0)
	    semsg(_(e_invalid_వాదం_str), tv_get_string(&argvars[0]));
	లేకపోతే ఒకవేళ (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	నిలువు = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[1], ఖాళీ);
	ఒకవేళ (charcol)
	    నిలువు = buf_charidx_గమ్యం_byteidx(curbuf, lnum, నిలువు) + 1;
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[2], ఖాళీ);
    }
    లేకపోతే
    {
	emsg(_(e_invalid_వాదం));
	తిరిగి;
    }
    ఒకవేళ (lnum < 0 || నిలువు < 0 || coladd < 0)
	తిరిగి;		// type error; errmsg already given
    ఒకవేళ (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    ఒకవేళ (నిలువు != MAXCOL && --నిలువు < 0)
	నిలువు = 0;
    curwin->w_cursor.నిలువు = నిలువు;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    ఒకవేళ (has_mbyte)
	mb_adjust_cursor();

    curwin->w_సమితి_curswant = సమితి_curswant;
    rettv->vval.v_సంఖ్య = 0;
}

/*
 * "cursor(lnum, col)" function, or
 * "cursor(list)"
 *
 * Moves the cursor to the specified line and column.
 * Returns 0 when the position could be set, -1 otherwise.
 */
    స్థిర శూన్య
f_cursor(typval_T *argvars, typval_T *rettv)
{
    సమితి_cursorpos(argvars, rettv, FALSE);
}

#నిర్వచితమైతే MSWIN
/*
 * "debugbreak()" function
 */
    స్థిర శూన్య
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		pid;

    rettv->vval.v_సంఖ్య = FAIL;
    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    pid = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]);
    ఒకవేళ (pid == 0)
    {
	emsg(_(e_invalid_వాదం));
	తిరిగి;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    ఒకవేళ (hProcess == ఖాళీ)
	తిరిగి;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_సంఖ్య = OK;
}
#అయితే_ముగింపు

/*
 * "deepcopy()" function
 */
    స్థిర శూన్య
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    ఒకవేళ (check_for_opt_bool_arg(argvars, 1) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], ఖాళీ);

    అంశం_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    స్థిర శూన్య
f_did_filetype(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    rettv->vval.v_సంఖ్య = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    స్థిర శూన్య
f_echoraw(typval_T *argvars, typval_T *rettv వాడని)
{
    char_u *str;

    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    str = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (str != ఖాళీ && *str != NUL)
    {
	బహిర్గతం_str(str);
	బహిర్గతం_flush();
    }
}

/*
 * "empty({expr})" function
 */
    స్థిర శూన్య
f_ఖాళీలు(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		n = FALSE;

    ఎంపిక (argvars[0].v_type)
    {
	స్థితి VAR_STRING:
	స్థితి VAR_FUNC:
	    n = argvars[0].vval.v_string == ఖాళీ
					  || *argvars[0].vval.v_string == NUL;
	    ఆపు;
	స్థితి VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == ఖాళీ;
	    ఆపు;
	స్థితి VAR_సంఖ్య:
	    n = argvars[0].vval.v_సంఖ్య == 0;
	    ఆపు;
	స్థితి VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    ఆపు;
	స్థితి VAR_LIST:
	    n = argvars[0].vval.v_list == ఖాళీ
					|| argvars[0].vval.v_list->lv_len == 0;
	    ఆపు;
	స్థితి VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    ఆపు;
	స్థితి VAR_DICT:
	    n = argvars[0].vval.v_dict == ఖాళీ
			|| argvars[0].vval.v_dict->dv_hashtab.ht_వాడిన == 0;
	    ఆపు;
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	    n = argvars[0].vval.v_సంఖ్య != VVAL_TRUE;
	    ఆపు;
	స్థితి VAR_CLASS:
	    n = argvars[0].vval.v_class != ఖాళీ;
	    ఆపు;
	స్థితి VAR_OBJECT:
	    n = object_ఖాళీలు(argvars[0].vval.v_object);
	    ఆపు;

	స్థితి VAR_BLOB:
	    n = argvars[0].vval.v_blob == ఖాళీ
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    ఆపు;

	స్థితి VAR_పని:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    n = argvars[0].vval.v_పని == ఖాళీ
			   || argvars[0].vval.v_పని->jv_స్థితి != పని_STARTED;
	    ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_CHANNEL:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    n = argvars[0].vval.v_channel == ఖాళీ
			       || !channel_is_open(argvars[0].vval.v_channel);
	    ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == ఖాళీ
		|| argvars[0].vval.v_typealias->ta_name == ఖాళీ
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    ఆపు;

	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	స్థితి VAR_INSTR:
	    అంతర్గత_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    ఆపు;
    }

    rettv->vval.v_సంఖ్య = n;
}

/*
 * "environ()" function
 */
    స్థిర శూన్య
f_environ(typval_T *argvars వాడని, typval_T *rettv)
{
#ఒకవేళ !defined(AMIGA)
    పూర్ణాంకం			i = 0;
    char_u		*నమోదు_2, *విలువ;
# ఒకవేళ defined (MSWIN)
#  ఒకవేళ !defined(_UCRT)
    బాహ్య wchar_t	**_wenviron;
#  endif
# లేకపోతే
    బాహ్య అక్షరం		**environ;
# endif

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;

# ifdef MSWIN
    ఒకవేళ (*_wenviron == ఖాళీ)
	తిరిగి;
# లేకపోతే
    ఒకవేళ (*environ == ఖాళీ)
	తిరిగి;
# endif

    కొరకు (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	ఒకవేళ ((p = (short_u *)_wenviron[i]) == ఖాళీ)
	    తిరిగి;
	నమోదు_2 = utf16_గమ్యం_enc(p, ఖాళీ);
# లేకపోతే
	ఒకవేళ ((నమోదు_2 = (char_u *)environ[i]) == ఖాళీ)
	    తిరిగి;
	నమోదు_2 = vim_strsave(నమోదు_2);
# endif
	ఒకవేళ (నమోదు_2 == ఖాళీ) // out of memory
	    తిరిగి;
	ఒకవేళ ((విలువ = vim_strchr(నమోదు_2, '=')) == ఖాళీ)
	{
	    vim_free(నమోదు_2);
	    కొనసాగు;
	}
	*విలువ++ = NUL;
	dict_కూడు_string(rettv->vval.v_dict, (అక్షరం *)నమోదు_2, విలువ);
	vim_free(నమోదు_2);
    }
#అయితే_ముగింపు
}

/*
 * "err_teapot()" function
 */
    స్థిర శూన్య
f_err_teapot(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ఒకవేళ (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    ఒకవేళ (*skipwhite(s) == NUL)
		తిరిగి;
	}

	పూర్ణాంకం err = FALSE;
	పూర్ణాంకం do_503 = eval_సమాసం_గమ్యం_bool(&argvars[0], &err);
	ఒకవేళ (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_అందుబాటు));
	    తిరిగి;
	}
    }

    emsg(_(e_im_a_teapot));
}

/*
 * "escape({string}, {chars})" function
 */
    స్థిర శూన్య
f_escape(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    rettv->vval.v_string = vim_strsave_escaped(tv_get_string(&argvars[0]),
					 tv_get_string_buf(&argvars[1], buf));
    rettv->v_type = VAR_STRING;
}

/*
 * "eval()" function
 */
    స్థిర శూన్య
f_eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*s, *p;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    s = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (s != ఖాళీ)
	s = skipwhite(s);

    p = s;
    ఒకవేళ (s == ఖాళీ || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	ఒకవేళ (p != ఖాళీ && !aborting())
	    semsg(_(e_invalid_వ్యక్తీకరణ_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_సంఖ్య;
	rettv->vval.v_సంఖ్య = 0;
    }
    లేకపోతే ఒకవేళ (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    స్థిర శూన్య
f_eventhandler(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = vgetc_busy || ఆదానం_busy;
}

స్థిర garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    శూన్య
execute_redir_str(char_u *విలువ, పూర్ణాంకం విలువ_len)
{
    పూర్ణాంకం		len;

    ఒకవేళ (విలువ_len == -1)
	len = (పూర్ణాంకం)STRLEN(విలువ);	// Append the entire string
    లేకపోతే
	len = విలువ_len;		// Append only "value_len" characters
    ఒకవేళ (ga_grow(&redir_execute_ga, len) == FAIL)
	తిరిగి;

    mch_memmove((అక్షరం *)redir_execute_ga.ga_దత్తాంశం
	    + redir_execute_ga.ga_len, విలువ, len);
    redir_execute_ga.ga_len += len;
}

#ఒకవేళ defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    స్థిర char_u *
get_str_పంక్తి(
    పూర్ణాంకం	    c వాడని,
    శూన్య    *cookie,
    పూర్ణాంకం	    indent వాడని,
    getline_opt_T options వాడని)
{
    char_u	*ప్రారంభం = *(char_u **)cookie;
    char_u	*పంక్తి;
    char_u	*p;

    p = ప్రారంభం;
    ఒకవేళ (p == ఖాళీ || *p == NUL)
	తిరిగి ఖాళీ;
    p = vim_strchr(p, '\n');
    ఒకవేళ (p == ఖాళీ)
	పంక్తి = vim_strsave(ప్రారంభం);
    లేకపోతే
    {
	పంక్తి = vim_strnsave(ప్రారంభం, p - ప్రారంభం);
	p++;
    }

    *(char_u **)cookie = p;
    తిరిగి పంక్తి;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    శూన్య
execute_cmds_from_string(char_u *str)
{
    do_cmdline(ఖాళీ, get_str_పంక్తి, (శూన్య *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#అయితే_ముగింపు

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_పంక్తి(
    పూర్ణాంకం	    c వాడని,
    శూన్య    *cookie,
    పూర్ణాంకం	    indent వాడని,
    getline_opt_T options వాడని)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *అంశం = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి ఖాళీ;
    s = tv_get_string_buf_chk(&అంశం->li_tv, buf);
    *p = అంశం->li_తదుపరి;
    తిరిగి s == ఖాళీ ? ఖాళీ : vim_strsave(s);
}

/*
 * "execute()" function
 */
    శూన్య
execute_common(typval_T *argvars, typval_T *rettv, పూర్ణాంకం arg_off)
{
    char_u	*cmd = ఖాళీ;
    list_T	*list = ఖాళీ;
    పూర్ణాంకం		save_సందేశం_silent = సందేశం_silent;
    పూర్ణాంకం		save_emsg_silent = emsg_silent;
    పూర్ణాంకం		save_emsg_noredir = emsg_noredir;
    పూర్ణాంకం		save_redir_execute = redir_execute;
    పూర్ణాంకం		save_redir_off = redir_off;
    garray_T	save_ga;
    పూర్ణాంకం		save_సందేశం_నిలువు = సందేశం_నిలువు;
    పూర్ణాంకం		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    పూర్ణాంకం		echo_output = FALSE;

    rettv->vval.v_string = ఖాళీ;
    rettv->v_type = VAR_STRING;

    ఒకవేళ (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	ఒకవేళ (list == ఖాళీ || list->lv_len == 0)
	    // empty list, no commands, empty output
	    తిరిగి;
	++list->lv_refcount;
    }
    లేకపోతే ఒకవేళ (argvars[arg_off].v_type == VAR_పని
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_విలువ_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	తిరిగి;
    }
    లేకపోతే
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	ఒకవేళ (cmd == ఖాళీ)
	    తిరిగి;
    }

    ఒకవేళ (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      లోపల_vim9script());

	ఒకవేళ (s == ఖాళీ)
	    తిరిగి;
	ఒకవేళ (*s == NUL)
	    echo_output = TRUE;
	ఒకవేళ (STRNCMP(s, "silent", 6) == 0)
	    ++సందేశం_silent;
	ఒకవేళ (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    లేకపోతే
	++సందేశం_silent;

    ఒకవేళ (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, పరిమాణం(అక్షరం), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    ఒకవేళ (!echo_output)
	సందేశం_నిలువు = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    ఒకవేళ (cmd != ఖాళీ)
	do_cmdline_cmd(cmd);
    లేకపోతే
    {
	listitem_T	*అంశం;

	CHECK_LIST_MATERIALIZE(list);
	అంశం = list->lv_మొదటి;
	do_cmdline(ఖాళీ, get_list_పంక్తి, (శూన్య *)&అంశం,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    ఒకవేళ (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((అక్షరం *)redir_execute_ga.ga_దత్తాంశం)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_దత్తాంశం;
    }
    లేకపోతే
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = ఖాళీ;
    }
    సందేశం_silent = save_సందేశం_silent;
    emsg_silent = save_emsg_silent;
    emsg_noredir = save_emsg_noredir;

    redir_execute = save_redir_execute;
    ఒకవేళ (redir_execute)
	redir_execute_ga = save_ga;
    redir_off = save_redir_off;

    // "silent reg" or "silent echo x" leaves msg_col somewhere in the line.
    ఒకవేళ (echo_output)
	// When not working silently: put it in column zero.  A following
	// "echon" will overwrite the message, unavoidably.
	సందేశం_నిలువు = 0;
    లేకపోతే
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	సందేశం_నిలువు = save_సందేశం_నిలువు;
}

/*
 * "execute()" function
 */
    స్థిర శూన్య
f_execute(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    శూన్య
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    పూర్ణాంకం		n = FALSE;

    ఒకవేళ (లోపల_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    p = tv_get_string(&argvars[0]);
    ఒకవేళ (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	ఒకవేళ (mch_getenv(p + 1) != ఖాళీ)
	    n = TRUE;
	లేకపోతే
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    ఒకవేళ (p != ఖాళీ && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    లేకపోతే ఒకవేళ (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, ఖాళీ, TRUE) == OK);
	ఒకవేళ (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    లేకపోతే ఒకవేళ (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    లేకపోతే ఒకవేళ (*p == '?')			// internal function only
    {
	n = has_అంతర్గత_func_name(p + 1);
    }
    లేకపోతే ఒకవేళ (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    లేకపోతే ఒకవేళ (*p == '#')
    {
	ఒకవేళ (p[1] == '#')
	    n = autocmd_supported(p + 2);
	లేకపోతే
	    n = au_exists(p + 1);
    }
    లేకపోతే				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_సంఖ్య = n;
}

    స్థిర శూన్య
f_exists_compiled(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    emsg(_(e_exists_compiled_can_only_be_వాడిన_లోపల_def_function));
}

/*
 * "expand()" function
 */
    స్థిర శూన్య
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    size_t	len;
    పూర్ణాంకం		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    పూర్ణాంకం		error = FALSE;
    char_u	*ఫలితం;
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    char_u	*p_csl_save = p_csl;
#అయితే_ముగింపు

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	తిరిగి;

#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    // avoid using 'completeslash' here
    p_csl = ఖాళీలు_option;
#అయితే_ముగింపు

    rettv->v_type = VAR_STRING;
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_సమితి(rettv, ఖాళీ);

    s = tv_get_string(&argvars[0]);
    ఒకవేళ (*s == '%' || *s == '#' || *s == '<')
    {
	అక్షరం	*errormsg = ఖాళీ;

	ఒకవేళ (p_verbose == 0)
	    ++emsg_off;
	ఫలితం = eval_vars(s, s, &len, ఖాళీ, &errormsg, ఖాళీ, FALSE);
	ఒకవేళ (p_verbose == 0)
	    --emsg_off;
	లేకపోతే ఒకవేళ (errormsg != ఖాళీ)
	    emsg(errormsg);
	ఒకవేళ (rettv->v_type == VAR_LIST)
	{
	    ఒకవేళ (rettv_list_alloc(rettv) == OK && ఫలితం != ఖాళీ)
		list_append_string(rettv->vval.v_list, ఫలితం, -1);
	    vim_free(ఫలితం);
	}
	లేకపోతే
	    rettv->vval.v_string = ఫలితం;
    }
    లేకపోతే
    {
	// When the optional second argument is non-zero, don't remove matches
	// for 'wildignore' and don't put matches for 'suffixes' at the end.
	ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	ఒకవేళ (!error)
	{
	    ExpandInit(&xpc);
	    xpc.xp_context = EXPAND_FILES;
	    ఒకవేళ (p_wic)
		options += WILD_ICASE;
	    ఒకవేళ (rettv->v_type == VAR_STRING)
		rettv->vval.v_string = ExpandOne(&xpc, s, ఖాళీ,
							   options, WILD_ALL);
	    లేకపోతే ఒకవేళ (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, ఖాళీ, options, WILD_ALL_KEEP);
		కొరకు (పూర్ణాంకం i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	లేకపోతే
	    rettv->vval.v_string = ఖాళీ;
    }
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    p_csl = p_csl_save;
#అయితే_ముగింపు
}

/*
 * "expandcmd()" function
 * Expand all the special characters in a command string.
 */
    స్థిర శూన్య
f_expandcmd(typval_T *argvars, typval_T *rettv)
{
    exarg_T	eap;
    char_u	*cmdstr;
    అక్షరం	*errormsg = ఖాళీ;
    పూర్ణాంకం		emsgoff = TRUE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_క్షేత్రం(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = ఖాళీ;
    eap.cmdidx = CMD_USER;

    ఒకవేళ (emsgoff)
	++emsg_off;
    ఒకవేళ (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	ఒకవేళ (!emsgoff && errormsg != ఖాళీ && *errormsg != NUL)
	    emsg(errormsg);
    ఒకవేళ (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    స్థిర శూన్య
f_feedkeys(typval_T *argvars, typval_T *rettv వాడని)
{
    పూర్ణాంకం		remap = TRUE;
    పూర్ణాంకం		చొప్పించు = FALSE;
    char_u	*కీలకాలు, *flags;
    char_u	nbuf[NUMBUFLEN];
    పూర్ణాంకం		typed = FALSE;
    పూర్ణాంకం		execute = FALSE;
    పూర్ణాంకం		context = FALSE;
    పూర్ణాంకం		dangerous = FALSE;
    పూర్ణాంకం		lowlevel = FALSE;

    // This is not allowed in the sandbox.  If the commands would still be
    // executed in the sandbox it would be OK, but it probably happens later,
    // when "sandbox" is no longer set.
    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    కీలకాలు = tv_get_string(&argvars[0]);

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	కొరకు ( ; *flags != NUL; ++flags)
	{
	    ఎంపిక (*flags)
	    {
		స్థితి 'n': remap = FALSE; ఆపు;
		స్థితి 'm': remap = TRUE; ఆపు;
		స్థితి 't': typed = TRUE; ఆపు;
		స్థితి 'i': చొప్పించు = TRUE; ఆపు;
		స్థితి 'x': execute = TRUE; ఆపు;
		స్థితి 'c': context = TRUE; ఆపు;
		స్థితి '!': dangerous = TRUE; ఆపు;
		స్థితి 'L': lowlevel = TRUE; ఆపు;
	    }
	}
    }

    ఒకవేళ (*కీలకాలు != NUL || execute)
    {
	ఒకవేళ (lowlevel
#నిర్వచితమైతే FEAT_VTP
		&& (!is_పదం_win32()
		    || (కీలకాలు[0] == 3 && ctrl_c_interrupts && typed))
#అయితే_ముగింపు
	   )
	{
#నిర్వచితమైతే USE_ఆదానం_BUF
	    ch_log(ఖాళీ, "feedkeys() lowlevel: %s", కీలకాలు);

	    పూర్ణాంకం len = (పూర్ణాంకం)STRLEN(కీలకాలు);
	    కొరకు (పూర్ణాంకం idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		ఒకవేళ (కీలకాలు[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		కూడు_గమ్యం_ఆదానం_buf(కీలకాలు + idx, 1);
	    }
#లేకపోతే
	    emsg(_(e_lowlevel_ఆదానం_not_supported));
#అయితే_ముగింపు
	}
	లేకపోతే
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *కీలకాలు_esc = vim_strsave_escape_csi(కీలకాలు);
	    ఒకవేళ (కీలకాలు_esc == ఖాళీ)
		తిరిగి;

	    ch_log(ఖాళీ, "feedkeys(%s): %s", typed ? "typed" : "", కీలకాలు);

	    ins_typebuf(కీలకాలు_esc, (remap ? REMAP_YES : REMAP_NONE),
				   చొప్పించు ? 0 : typebuf.tb_len, !typed, FALSE);
	    ఒకవేళ (vgetc_busy
#నిర్వచితమైతే FEAT_TIMERS
		    || timer_busy
#అయితే_ముగింపు
		    || ఆదానం_busy)
		typebuf_was_filled = TRUE;

	    vim_free(కీలకాలు_esc);
	}

	ఒకవేళ (execute)
	{
	    పూర్ణాంకం		save_సందేశం_scroll = సందేశం_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    సందేశం_scroll = FALSE;

	    ch_log(ఖాళీ, "feedkeys() executing");

	    ఒకవేళ (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_సంస్కరణ = 0;
	    }

	    ఒకవేళ (!dangerous)
	    {
		++ex_సాధారణ_busy;
		++లోపల_feedkeys;
	    }
	    ++allow_osc_కీలకం;
	    exec_సాధారణ(TRUE, lowlevel, TRUE);
	    --allow_osc_కీలకం;
	    ఒకవేళ (!dangerous)
	    {
		--ex_సాధారణ_busy;
		--లోపల_feedkeys;
	    }

	    సందేశం_scroll |= save_సందేశం_scroll;

	    ఒకవేళ (context)
		current_sctx = save_sctx;
	}
    }
}

/*
 * "fnameescape({string})" function
 */
    స్థిర శూన్య
f_fnameescape(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    స్థిర శూన్య
f_foreground(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	gui_mch_సమితి_foreground();
	తిరిగి;
    }
#అయితే_ముగింపు
#ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_సమితి_foreground();
#అయితే_ముగింపు
}

/*
 * "function()" function
 * "funcref()" function
 */
    స్థిర శూన్య
common_function(typval_T *argvars, typval_T *rettv, పూర్ణాంకం is_funcref)
{
    char_u	*s;
    char_u	*name;
    పూర్ణాంకం		use_string = FALSE;
    partial_T   *arg_pt = ఖాళీ;
    char_u	*trans_name = ఖాళీ;
    పూర్ణాంకం		is_global = FALSE;
    char_u	*ప్రారంభం_bracket = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != ఖాళీ)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    లేకపోతే
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    ఒకవేళ (s == ఖాళీ)
    {
	semsg(_(e_invalid_వాదం_str), "NULL");
	తిరిగి;
    }

    ఒకవేళ ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == ఖాళీ) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, ఖాళీ);
	ఒకవేళ (*name == '<')
	{
	    // generic function
	    ప్రారంభం_bracket = name;
	    ఒకవేళ (skip_generic_func_type_వాదనలు(&name) == FAIL)
		వెళ్ళు theend;
	}
	ఒకవేళ (*name != NUL)
	    s = ఖాళీ;
    }

    ఒకవేళ (s == ఖాళీ || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == ఖాళీ))
	semsg(_(e_invalid_వాదం_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    లేకపోతే ఒకవేళ (trans_name != ఖాళీ && (is_funcref
			 ? కనుగొను_func(trans_name, is_global) == ఖాళీ
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    లేకపోతే
    {
	పూర్ణాంకం	dict_idx = 0;
	పూర్ణాంకం	arg_idx = 0;
	list_T	*list = ఖాళీ;

	ఒకవేళ (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	లేకపోతే ఒకవేళ (trans_name != ఖాళీ && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	లేకపోతే
	    name = vim_strsave(s);

	ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    లేకపోతే ఒకవేళ (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    లేకపోతే
		// function(name, [args])
		arg_idx = 1;
	    ఒకవేళ (dict_idx > 0)
	    {
		ఒకవేళ (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    వెళ్ళు theend;
		}
		ఒకవేళ (argvars[dict_idx].vval.v_dict == ఖాళీ)
		    dict_idx = 0;
	    }
	    ఒకవేళ (arg_idx > 0)
	    {
		ఒకవేళ (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_రెండవ_వాదం_యొక్క_function_must_be_list_or_dict));
		    vim_free(name);
		    వెళ్ళు theend;
		}
		list = argvars[arg_idx].vval.v_list;
		ఒకవేళ (list == ఖాళీ || list->lv_len == 0)
		    arg_idx = 0;
		లేకపోతే ఒకవేళ (list->lv_len > MAX_FUNC_వాదనలు)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    వెళ్ళు theend;
		}
	    }
	}
	ఒకవేళ (dict_idx > 0 || arg_idx > 0 || arg_pt != ఖాళీ || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    ఒకవేళ (pt == ఖాళీ)
		vim_free(name);
	    లేకపోతే
	    {
		ఒకవేళ (arg_idx > 0 || (arg_pt != ఖాళీ && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    పూర్ణాంకం		i = 0;
		    పూర్ణాంకం		arg_len = 0;
		    పూర్ణాంకం		lv_len = 0;

		    ఒకవేళ (arg_pt != ఖాళీ)
			arg_len = arg_pt->pt_argc;
		    ఒకవేళ (list != ఖాళీ)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    ఒకవేళ (pt->pt_argv == ఖాళీ)
		    {
			vim_free(pt);
			vim_free(name);
			వెళ్ళు theend;
		    }
		    కొరకు (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    ఒకవేళ (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_వస్తువులు(list, li)
			    copy_tv(&li->li_tv, &pt->pt_argv[i++]);
		    }
		}

		// For "function(dict.func, [], dict)" and "func" is a partial
		// use "dict".  That is backwards compatible.
		ఒకవేళ (dict_idx > 0)
		{
		    // The dict is bound explicitly, pt_auto is FALSE.
		    pt->pt_dict = argvars[dict_idx].vval.v_dict;
		    ++pt->pt_dict->dv_refcount;
		}
		లేకపోతే ఒకవేళ (arg_pt != ఖాళీ)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    ఒకవేళ (pt->pt_dict != ఖాళీ)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    ఒకవేళ (pt->pt_obj != ఖాళీ)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		ఒకవేళ (arg_pt != ఖాళీ && arg_pt->pt_func != ఖాళీ)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		లేకపోతే ఒకవేళ (is_funcref)
		{
		    pt->pt_func = కనుగొను_func(trans_name, is_global);
		    ఒకవేళ (IS_GENERIC_FUNC(pt->pt_func) && ప్రారంభం_bracket != ఖాళీ)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&ప్రారంభం_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		లేకపోతే
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		ఒకవేళ (arg_pt != ఖాళీ)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	లేకపోతే
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    ఒకవేళ (ప్రారంభం_bracket == ఖాళీ)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    లేకపోతే
	    {
		// generic function
		size_t len = STRLEN(name) + STRLEN(ప్రారంభం_bracket);
		rettv->vval.v_string = alloc(len + 1);
		ఒకవేళ (rettv->vval.v_string != ఖాళీ)
		{
		    STRCPY(rettv->vval.v_string, name);
		    STRCAT(rettv->vval.v_string, ప్రారంభం_bracket);
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
    స్థిర శూన్య
f_funcref(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, TRUE);
}

/*
 * "function()" function
 */
    స్థిర శూన్య
f_function(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, FALSE);
}

/*
 * "garbagecollect()" function
 */
    స్థిర శూన్య
f_garbagecollect(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (లోపల_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	తిరిగి;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
	garbage_collect_at_exit = TRUE;
}

/*
 * "get()" function
 */
    స్థిర శూన్య
f_get(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    list_T	*l;
    tuple_T	*tuple;
    dictitem_T	*di;
    dict_T	*d;
    typval_T	*tv = ఖాళీ;
    పూర్ణాంకం		what_is_dict = FALSE;

    ఒకవేళ (argvars[0].v_type == VAR_BLOB)
    {
	పూర్ణాంకం error = FALSE;
	పూర్ణాంకం idx = tv_get_సంఖ్య_chk(&argvars[1], &error);

	ఒకవేళ (!error)
	{
	    rettv->v_type = VAR_సంఖ్య;
	    ఒకవేళ (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    ఒకవేళ (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_సంఖ్య = -1;
	    లేకపోతే
	    {
		rettv->vval.v_సంఖ్య = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_LIST)
    {
	ఒకవేళ ((l = argvars[0].vval.v_list) != ఖాళీ)
	{
	    పూర్ణాంకం		error = FALSE;

	    li = list_కనుగొను(l, (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[1], &error));
	    ఒకవేళ (!error && li != ఖాళీ)
		tv = &li->li_tv;
	}
    }
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_TUPLE)
    {
	ఒకవేళ ((tuple = argvars[0].vval.v_tuple) != ఖాళీ)
	{
	    పూర్ణాంకం		error = FALSE;
	    దీర్ఘ	idx;

	    idx = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[1], &error);
	    ఒకవేళ (!error)
		tv = tuple_కనుగొను(tuple, idx);
	}
    }
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_DICT)
    {
	ఒకవేళ ((d = argvars[0].vval.v_dict) != ఖాళీ)
	{
	    di = dict_కనుగొను(d, tv_get_string(&argvars[1]), -1);
	    ఒకవేళ (di != ఖాళీ)
		tv = &di->di_tv;
	}
    }
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	ఒకవేళ (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	లేకపోతే
	{
	    CLEAR_క్షేత్రం(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	ఒకవేళ (pt != ఖాళీ)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    ఒకవేళ (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		ఒకవేళ (name == ఖాళీ)
		    rettv->vval.v_string = ఖాళీ;
		లేకపోతే
		{
		    ఒకవేళ (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    ఒకవేళ (*what == 'n' && pt->pt_name == ఖాళీ
							&& pt->pt_func != ఖాళీ)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		ఒకవేళ (pt->pt_dict != ఖాళీ)
		    rettv_dict_సమితి(rettv, pt->pt_dict);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		ఒకవేళ (rettv_list_alloc(rettv) == OK)
		{
		    పూర్ణాంకం i;

		    కొరకు (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(what, "arity") == 0)
	    {
		పూర్ణాంకం required = 0, optional = 0, varargs = FALSE;
		char_u *name = partial_name(pt);

		get_func_arity(name, &required, &optional, &varargs);

		rettv->v_type = VAR_DICT;
		ఒకవేళ (rettv_dict_alloc(rettv) == OK)
		{
		    dict_T *dict = rettv->vval.v_dict;

		    // Take into account the arguments of the partial, if any.
		    // Note that it is possible to supply more arguments than the function
		    // accepts.
		    ఒకవేళ (pt->pt_argc >= required + optional)
			required = optional = 0;
		    లేకపోతే ఒకవేళ (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    లేకపోతే
			required -= pt->pt_argc;

		    dict_కూడు_సంఖ్య(dict, "required", required);
		    dict_కూడు_సంఖ్య(dict, "optional", optional);
		    dict_కూడు_bool(dict, "varargs", varargs);
		}
	    }
	    లేకపోతే
		semsg(_(e_invalid_వాదం_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    ఒకవేళ (!what_is_dict)
		తిరిగి;
	}
    }
    లేకపోతే
	semsg(_(e_వాదం_యొక్క_str_must_be_list_tuple_dictionary_or_blob), "get()");

    ఒకవేళ (tv == ఖాళీ)
    {
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    లేకపోతే
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
    స్థిర శూన్య
f_getbgcolor(typval_T *argvars వాడని, typval_T *rettv)
{
    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	ఒకవేళ (gui.లోపల_use)
	    try_hl = true;
# endif
# ifdef FEAT_TERMGUICOLORS
	ఒకవేళ (p_tgc)
	    try_hl = true;
# endif
	ఒకవేళ (try_hl)
	{
	    guicolor_T	fg, bg;
	    పూర్ణాంకం		hl_id = syn_name2id((char_u *)"Normal");

	    ఒకవేళ (hl_id > 0)
	    {
		(శూన్య)syn_id2colors(hl_id, &fg, &bg);
		ఒకవేళ (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_సంఖ్య(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_సంఖ్య(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_సంఖ్య(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    తిరిగి;
		}
	    }
	}
    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	ఒకవేళ (పదం_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)r);
	    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)g);
	    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#అయితే_ముగింపు
}

/*
 * "getcellpixels()" function
 */
    స్థిర శూన్య
f_getcellpixels(typval_T *argvars వాడని, typval_T *rettv)
{
    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

#ఒకవేళ defined(FEAT_GUI)
    ఒకవేళ (gui.లోపల_use)
    {
	// success pixel size and no gui.
	list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)gui.char_వెడల్పు);
	list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)gui.char_ఎత్తు);
    }
    లేకపోతే
#అయితే_ముగింపు
    {
#ఒకవేళ defined(UNIX) || defined(MSWIN)
	list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)cell_వెడల్పు);
	list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)cell_ఎత్తు);
#లేకపోతే
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)-1);
	list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)-1);
#అయితే_ముగింపు
    }

}

/*
 * "getchangelist()" function
 */
    స్థిర శూన్య
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    పూర్ణాంకం		i;
    list_T	*l;
    dict_T	*d;
    పూర్ణాంకం		changelistindex;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    లేకపోతే
	buf = tv_get_buf_from_arg(&argvars[0]);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    l = list_alloc();
    ఒకవేళ (l == ఖాళీ)
	తిరిగి;
    ఒకవేళ (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	తిరిగి;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    ఒకవేళ (buf == curwin->w_బఫరు)
    {
	changelistindex = curwin->w_changelistidx;
    }
    లేకపోతే
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    ఒకవేళ (wip->wi_win == curwin)
		ఆపు;
	changelistindex = wip != ఖాళీ ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)changelistindex);

    కొరకు (i = 0; i < buf->b_changelistlen; ++i)
    {
	ఒకవేళ (buf->b_changelist[i].lnum == 0)
	    కొనసాగు;
	ఒకవేళ ((d = dict_alloc()) == ఖాళీ)
	    తిరిగి;
	ఒకవేళ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    తిరిగి;
	}
	dict_కూడు_సంఖ్య(d, "lnum", (దీర్ఘ)buf->b_changelist[i].lnum);
	dict_కూడు_సంఖ్య(d, "col", (దీర్ఘ)buf->b_changelist[i].నిలువు);
	dict_కూడు_సంఖ్య(d, "coladd", (దీర్ఘ)buf->b_changelist[i].coladd);
    }
}

    స్థిర శూన్య
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    పూర్ణాంకం		getcurpos,
    పూర్ణాంకం		charcol)
{
    pos_T	*fp = ఖాళీ;
    pos_T	pos;
    win_T	*wp = curwin;
    list_T	*l;
    పూర్ణాంకం		fnum = -1;

    ఒకవేళ (rettv_list_alloc(rettv) == OK)
    {
	l = rettv->vval.v_list;
	ఒకవేళ (getcurpos)
	{
	    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
	    {
		wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
		ఒకవేళ (wp != ఖాళీ)
		    fp = &wp->w_cursor;
	    }
	    లేకపోతే
		fp = &curwin->w_cursor;
	    ఒకవేళ (fp != ఖాళీ && charcol)
	    {
		pos = *fp;
		pos.నిలువు =
		    buf_byteidx_గమ్యం_charidx(wp->w_బఫరు, pos.lnum, pos.నిలువు);
		fp = &pos;
	    }
	}
	లేకపోతే
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	ఒకవేళ (fnum != -1)
	    list_append_సంఖ్య(l, (varnumber_T)fnum);
	లేకపోతే
	    list_append_సంఖ్య(l, (varnumber_T)0);
	list_append_సంఖ్య(l, (fp != ఖాళీ) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_సంఖ్య(l, (fp != ఖాళీ)
		     ? (varnumber_T)(fp->నిలువు == MAXCOL ? MAXCOL : fp->నిలువు + 1)
							    : (varnumber_T)0);
	list_append_సంఖ్య(l, (fp != ఖాళీ) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	ఒకవేళ (getcurpos)
	{
	    పూర్ణాంకం	    save_సమితి_curswant = curwin->w_సమితి_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    ఒకవేళ (wp == curwin)
		update_curswant();
	    list_append_సంఖ్య(l, wp == ఖాళీ ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    ఒకవేళ (wp == curwin && save_సమితి_curswant)
	    {
		curwin->w_సమితి_curswant = save_సమితి_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    లేకపోతే
	rettv->vval.v_సంఖ్య = FALSE;
}

/*
 * "getcharpos()" function
 */
    స్థిర శూన్య
f_getcharpos(typval_T *argvars వాడని, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    స్థిర శూన్య
f_getcharsearch(typval_T *argvars వాడని, typval_T *rettv)
{
    ఒకవేళ (rettv_dict_alloc(rettv) != OK)
	తిరిగి;

    dict_T *dict = rettv->vval.v_dict;

    dict_కూడు_string(dict, "char", చివరి_csearch());
    dict_కూడు_సంఖ్య(dict, "forward", చివరి_csearch_forward());
    dict_కూడు_సంఖ్య(dict, "until", చివరి_csearch_until());
}

/*
 * "getenv()" function
 */
    స్థిర శూన్య
f_getenv(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం	    mustfree = FALSE;
    char_u  *p;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    ఒకవేళ (p == ఖాళీ)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_సంఖ్య = VVAL_NULL;
	తిరిగి;
    }
    ఒకవేళ (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    స్థిర శూన్య
f_getfontname(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	తిరిగి;

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	GuiFont font;
	char_u	*name = ఖాళీ;

	ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	లేకపోతే
	{
	    name = tv_get_string(&argvars[0]);
	    ఒకవేళ (STRCMP(name, "*") == 0)	    // don't use font dialog
		తిరిగి;
	    font = gui_mch_get_font(name, FALSE);
	    ఒకవేళ (font == NOFONT)
		తిరిగి;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#అయితే_ముగింపు
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
స్థిర నిర్మాణం
{
    argcheck_T	func;
    అక్షరం	*types;	    // separated by commas
} argcheck_types[] =
{
    {arg_any,			"any"},
    {arg_blob,			"blob"},
    {arg_bool,			"bool,number"},
    {arg_bool_or_dict_any,	"bool,number,dict<any>"},
    {arg_bool_or_nr,		"bool,number"},
    {arg_బఫరు,		"string,number"},
    {arg_బఫరు_or_dict_any,	"string,number,dict<any>"},
    {arg_chan_or_పని,		"channel,job"},
    {arg_cursor1,		"number,string,list<any>"},
    {arg_dict_any,		"dict<any>"},
    {arg_dict_any_or_string,	"dict<any>,string"},
    {arg_extend3,		"number,string"},
    {arg_filter_func,		"string,func"},
    {arg_float_or_nr,		"float,number"},
    {arg_foreach_func,		"string,func"},
    {arg_get1,			"blob,list<any>,tuple<any>,dict<any>,func"},
    {arg_అంశం_యొక్క_prev,		"any"},
    {arg_పని,			"job"},
    {arg_len1,
	    "string,number,blob,list<any>,tuple<any>,dict<any>,object<any>"},
    {arg_list_any,		"list<any>"},
    {arg_list_any_mod,		"list<any>"},
    {arg_list_సంఖ్య,		"list<number>"},
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
    {arg_సంఖ్య,		"number"},
    {arg_object,		"object<any>"},
    {arg_remove2,		"number,string"},
    {arg_repeat1,		"string,number,blob,list<any>,tuple<any>"},
    {arg_తిరగేయి,		"list<any>,tuple<any>,blob,string"},
    {arg_same_as_prev,		"any"},
    {arg_same_struct_as_prev,	"any"},
    {arg_slice1,		"list<any>,tuple<any>,blob,string"},
    {arg_క్రమీకరించు_how,		"string,func"},
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
    స్థిర list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    అక్షరం	*types = "any";

    ఒకవేళ (l == ఖాళీ)
	తిరిగి ఖాళీ;
    కొరకు (పూర్ణాంకం i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(argcheck_types); ++i)
	ఒకవేళ (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    ఆపు;
	}
    కొరకు (అక్షరం *p = types; ; )
    {
	అక్షరం	*చివర = (అక్షరం *)vim_strchr((char_u *)p, ',');
	పూర్ణాంకం	len = చివర == ఖాళీ ? (పూర్ణాంకం)STRLEN(p) : (పూర్ణాంకం)(చివర - p);

	ఒకవేళ (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    తిరిగి ఖాళీ;
	}
	ఒకవేళ (చివర == ఖాళీ)
	    ఆపు;
	p = చివర + 1;
    }
    తిరిగి l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    స్థిర పూర్ణాంకం
getinfo_argtypes(
	list_T			*l,
	స్థిరరాశి funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    పూర్ణాంకం		argcount = 0;

    ఒకవేళ (l != ఖాళీ)
	FOR_ALL_LIST_వస్తువులు(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    ఒకవేళ (s == ఖాళీ)
		తిరిగి -1;
	    ఒకవేళ (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_వాదనలు)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		తిరిగి -1;
	    }
	    type = parse_type(&p, type_gap, ఖాళీ, ఖాళీ, TRUE);
	    ఒకవేళ (type == ఖాళీ)
		తిరిగి -1;
	    ఒకవేళ (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		తిరిగి -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    కొరకు ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_వాదనలు; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    తిరిగి argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    స్థిర అక్షరం *
builtin_ret_type_name(
	పూర్ణాంకం	    idx,
	పూర్ణాంకం	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	అక్షరం	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = అంతర్గత_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    తిరిగి type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
రకనిర్వచనం గణన {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_రకం_T;

స్థిర నిర్మాణం
{
    అక్షరం		*name;
    getinfo_రకం_T	రకం;
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
    స్థిర శూన్య
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_రకం_T	రకం;
    పూర్ణాంకం			ki;
    పూర్ణాంకం			idx;
    స్థిరరాశి funcentry_T	*fe;
    dict_T		*d;
    list_T		*వాదనలు;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_వాదనలు];
    list_T		*argtypes_list = ఖాళీ;
    పూర్ణాంకం			argcount;
    పూర్ణాంకం			vim9 = -1;	// -1: the context of the caller
    అక్షరం		*ret_name;
    అక్షరం		*tofree;

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;
    ఒకవేళ (లోపల_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;
    ఒకవేళ (check_for_opt_dict_arg(argvars, 2) == FAIL)
	తిరిగి;

    kindname = tv_get_string(&argvars[0]);
    కొరకు (ki = 0; ki < (పూర్ణాంకం)ARRAY_నిడివి(getinfo_kinds); ++ki)
	ఒకవేళ (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    ఆపు;
    ఒకవేళ (ki == (పూర్ణాంకం)ARRAY_నిడివి(getinfo_kinds))
    {
	semsg(_(e_invalid_వాదం_str), kindname);
	తిరిగి;
    }
    రకం = getinfo_kinds[ki].రకం;
    name = tv_get_string_buf(&argvars[1], namebuf);

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != ఖాళీ)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	పూర్ణాంకం	    todo = (పూర్ణాంకం)opts->dv_hashtab.ht_వాడిన;

	FOR_ALL_HASHTAB_వస్తువులు(&opts->dv_hashtab, hi, todo)
	{
	    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
		కొనసాగు;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    ఒకవేళ (STRCMP(di->di_కీలకం, "argtypes") == 0 && రకం == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    లేకపోతే ఒకవేళ (STRCMP(di->di_కీలకం, "vim9") == 0 && రకం == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    లేకపోతే
	    {
		semsg(_(e_invalid_వాదం_str), di->di_కీలకం);
		తిరిగి;
	    }
	}
    }
    ఒకవేళ (రకం == GI_VIMVAR)
    {
	ఒకవేళ (STRNCMP(name, "v:", 2) == 0)
	    (శూన్య)vim_var_info(name + 2, rettv->vval.v_dict);
	తిరిగి;
    }
    ఒకవేళ (రకం == GI_COMMAND)
    {
	(శూన్య)ex_command_info(name, vim9, rettv->vval.v_dict);
	తిరిగి;
    }
    ఒకవేళ (రకం == GI_OPTION)
    {
	(శూన్య)option_info(name, rettv->vval.v_dict);
	తిరిగి;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = కనుగొను_అంతర్గత_func_opt(name, FALSE);
    ఒకవేళ (idx < 0)
    {
	(శూన్య)user_func_info(name, rettv->vval.v_dict);
	తిరిగి;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, పరిమాణం(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    ఒకవేళ (argcount < 0)
    {
	clear_type_list(&type_gap);
	తిరిగి;
    }
    d = rettv->vval.v_dict;
    dict_కూడు_string(d, "name", (char_u *)fe->f_name);
    dict_కూడు_string(d, "kind", (char_u *)"builtin");
    dict_కూడు_bool(d, "available", fe->f_func != ఖాళీ);
    dict_కూడు_సంఖ్య(d, "minargs", fe->f_min_argc);
    dict_కూడు_సంఖ్య(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_కూడు_సంఖ్య(d, "method", fe->f_argtype & FEARG_ముసుగు);

    వాదనలు = list_alloc();
    ఒకవేళ (వాదనలు == ఖాళీ || dict_కూడు_list(d, "args", వాదనలు) == FAIL)
    {
	list_unref(వాదనలు);
	clear_type_list(&type_gap);
	తిరిగి;
    }
    argnames = (char_u *)fe->f_argnames;
    కొరకు (పూర్ణాంకం i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = ఖాళీ;
	పూర్ణాంకం	    argnamelen = 0;

	ఒకవేళ (fe->f_argcheck == ఖాళీ)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    ఒకవేళ (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		ఆపు;
	    check = arg_any;
	}
	లేకపోతే
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    ఒకవేళ ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == ఖాళీ)
		ఆపు;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	ఒకవేళ (argnames != ఖాళీ)
	{
	    char_u  *చివర = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = చివర == ఖాళీ ? (పూర్ణాంకం)STRLEN(argnames)
						     : (పూర్ణాంకం)(చివర - argnames);
	    argnames = చివర == ఖాళీ ? ఖాళీ : skipwhite(చివర + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	ఒకవేళ (arg == ఖాళీ || types == ఖాళీ
		|| dict_కూడు_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    తిరిగి;
	}
	ఒకవేళ ((argnamelen > 0
		    && dict_కూడు_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(వాదనలు, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    తిరిగి;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    ఒకవేళ (argtypes_list == ఖాళీ
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	అక్షరం	*tofree_max;
	అక్షరం	*max_name;

	కొరకు (పూర్ణాంకం i = argcount; i < fe->f_max_argc && i < MAX_FUNC_వాదనలు; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	ఒకవేళ (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = ఖాళీ;
	    ret_name = "any";
	}
	vim_free(tofree_max);
    }
    dict_కూడు_string(d, "returns", (char_u *)ret_name);
    vim_free(tofree);
    clear_type_list(&type_gap);
}

/*
 * "getjumplist()" function
 */
    స్థిర శూన్య
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    పూర్ణాంకం		i;
    list_T	*l;
    dict_T	*d;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL)))
	తిరిగి;

    wp = కనుగొను_tabwin(&argvars[0], &argvars[1], ఖాళీ);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    ఒకవేళ (l == ఖాళీ)
	తిరిగి;
    ఒకవేళ (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	తిరిగి;
    }

    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    కొరకు (i = 0; i < wp->w_jumplistlen; ++i)
    {
	ఒకవేళ (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    కొనసాగు;
	ఒకవేళ ((d = dict_alloc()) == ఖాళీ)
	    తిరిగి;
	ఒకవేళ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    తిరిగి;
	}
	dict_కూడు_సంఖ్య(d, "lnum", (దీర్ఘ)wp->w_jumplist[i].fmark.mark.lnum);
	dict_కూడు_సంఖ్య(d, "col", (దీర్ఘ)wp->w_jumplist[i].fmark.mark.నిలువు);
	dict_కూడు_సంఖ్య(d, "coladd", (దీర్ఘ)wp->w_jumplist[i].fmark.mark.coladd);
	dict_కూడు_సంఖ్య(d, "bufnr", (దీర్ఘ)wp->w_jumplist[i].fmark.fnum);
	ఒకవేళ (wp->w_jumplist[i].fname != ఖాళీ)
	    dict_కూడు_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    స్థిర శూన్య
f_getpid(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    స్థిర శూన్య
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    స్థిర శూన్య
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    స్థిర శూన్య
f_getpos(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    స్థిర పూర్ణాంకం
ఖండం_def2str(నిర్మాణం ఖండం_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    ఒకవేళ (ret->string == ఖాళీ)
    {
	ret->నిడివి = 0;
	తిరిగి FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->నిడివి = bd->startspaces;
    mch_memmove(ret->string + ret->నిడివి, bd->textstart, bd->textlen);
    ret->నిడివి += bd->textlen;
    vim_memset(ret->string + ret->నిడివి, ' ', bd->endspaces);
    ret->నిడివి += bd->endspaces;
    ret->string[ret->నిడివి] = NUL;

    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
getregionpos(
    typval_T	*argvars,
    typval_T	*rettv,
    pos_T	*p1,
    pos_T	*p2,
    పూర్ణాంకం		*inclusive,
    పూర్ణాంకం		*region_type,
    oparg_T	*oap)
{
    పూర్ణాంకం		fnum1 = -1, fnum2 = -1;
    char_u	*type;
    buf_T	*findbuf;
    char_u	default_type[] = "v";
    పూర్ణాంకం		ఖండం_వెడల్పు = 0;
    పూర్ణాంకం		is_select_exclusive;
    పూర్ణాంకం		l;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (list2fpos(&argvars[0], p1, &fnum1, ఖాళీ, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, ఖాళీ, FALSE) != OK
	    || fnum1 != fnum2)
	తిరిగి FAIL;

    ఒకవేళ (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	ఒకవేళ (type == ఖాళీ)
	    type = default_type;
    }
    లేకపోతే
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    ఒకవేళ (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    లేకపోతే ఒకవేళ (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    లేకపోతే ఒకవేళ (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	ఒకవేళ (*p != NUL && ((ఖండం_వెడల్పు = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_విలువ_for_వాదం_str_str), "type", type);
	    తిరిగి FAIL;
	}
	*region_type = MBLOCK;
    }
    లేకపోతే
    {
	semsg(_(e_invalid_విలువ_for_వాదం_str_str), "type", type);
	తిరిగి FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    ఒకవేళ (findbuf == ఖాళీ || findbuf->b_ml.ml_mfp == ఖాళీ)
    {
	emsg(_(e_బఫరు_is_not_loaded));
	తిరిగి FAIL;
    }

    ఒకవేళ (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	semsg(_(e_invalid_పంక్తి_సంఖ్య_nr), p1->lnum);
	తిరిగి FAIL;
    }
    ఒకవేళ (p1->నిలువు == MAXCOL)
	p1->నిలువు = ml_get_buf_len(findbuf, p1->lnum) + 1;
    లేకపోతే ఒకవేళ (p1->నిలువు < 1 || p1->నిలువు > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_స్తంభం_సంఖ్య_nr), p1->నిలువు);
	తిరిగి FAIL;
    }

    ఒకవేళ (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	semsg(_(e_invalid_పంక్తి_సంఖ్య_nr), p2->lnum);
	తిరిగి FAIL;
    }
    ఒకవేళ (p2->నిలువు == MAXCOL)
	p2->నిలువు = ml_get_buf_len(findbuf, p2->lnum) + 1;
    లేకపోతే ఒకవేళ (p2->నిలువు < 1 || p2->నిలువు > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_స్తంభం_సంఖ్య_nr), p2->నిలువు);
	తిరిగి FAIL;
    }

    curbuf = findbuf;
    curwin->w_బఫరు = curbuf;
    virtual_op = virtual_చురుకైన();

    // NOTE: Adjustment is needed.
    p1->నిలువు--;
    p2->నిలువు--;

    ఒకవేళ (!LT_POS(*p1, *p2))
    {
	// swap position
	pos_T p;

	p = *p1;
	*p1 = *p2;
	*p2 = p;
    }

    ఒకవేళ (*region_type == MCHAR)
    {
	// Handle 'selection' == "exclusive".
	ఒకవేళ (is_select_exclusive && !EQUAL_POS(*p1, *p2))
	    // When backing up to previous line, inclusive becomes false.
	    *inclusive = !unadjust_for_sel_inner(p2);
	// If p2 is on NUL (end of line), inclusive becomes false.
	ఒకవేళ (*inclusive && !virtual_op && *ml_get_pos(p2) == NUL)
	    *inclusive = FALSE;
    }
    లేకపోతే ఒకవేళ (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#నిర్వచితమైతే FEAT_LINEBREAK
	పూర్ణాంకం	lbr_saved = reset_lbr();
#అయితే_ముగింపు

	getvvcol(curwin, p1, &sc1, ఖాళీ, &ec1, 0);
	getvvcol(curwin, p2, &sc2, ఖాళీ, &ec2, 0);

#నిర్వచితమైతే FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#అయితే_ముగింపు

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->ప్రారంభం = *p1;
	oap->చివర = *p2;
	oap->ప్రారంభం_vcol = MIN(sc1, sc2);
	ఒకవేళ (ఖండం_వెడల్పు > 0)
	    oap->చివర_vcol = oap->ప్రారంభం_vcol + ఖండం_వెడల్పు - 1;
	లేకపోతే ఒకవేళ (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->చివర_vcol = sc2 - 1;
	లేకపోతే
	    oap->చివర_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    ఒకవేళ (l > 1)
	p2->నిలువు += l - 1;

    తిరిగి OK;
}

/*
 * "getregion()" function
 */
    స్థిర శూన్య
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    పూర్ణాంకం			inclusive = TRUE;
    పూర్ణాంకం			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    పూర్ణాంకం			save_virtual;
    string_T		akt = {ఖాళీ, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    ఒకవేళ (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	తిరిగి;

    కొరకు (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	పూర్ణాంకం ret = OK;

	ఒకవేళ (region_type == MBLOCK)
	{
	    నిర్మాణం ఖండం_def	bd;

	    ఖండం_prep(&oa, &bd, lnum, FALSE);
	    ఖండం_def2str(&bd, &akt);
	}
	లేకపోతే ఒకవేళ (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.నిడివి = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.నిడివి);
	    ఒకవేళ (akt.string == ఖాళీ)
		akt.నిడివి = 0;
	    లేకపోతే
		akt.నిడివి = s.నిడివి;
	}
	లేకపోతే
	{
	    నిర్మాణం ఖండం_def	bd;

	    charwise_ఖండం_prep(p1, p2, &bd, lnum, inclusive);
	    ఖండం_def2str(&bd, &akt);
	}

	ఒకవేళ (akt.string == ఖాళీ)
	{
	    clear_tv(rettv);
	    (శూన్య)rettv_list_alloc(rettv);
	    ఆపు;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (పూర్ణాంకం)akt.నిడివి);
	vim_free(akt.string);

	ఒకవేళ (ret == FAIL)
	{
	    clear_tv(rettv);
	    (శూన్య)rettv_list_alloc(rettv);
	    ఆపు;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_బఫరు = curbuf;
    virtual_op = save_virtual;
}

    స్థిర శూన్య
కూడు_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    ఒకవేళ (l1 == ఖాళీ)
	తిరిగి;

    ఒకవేళ (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	తిరిగి;
    }

    l2 = list_alloc();
    ఒకవేళ (l2 == ఖాళీ)
	తిరిగి;

    ఒకవేళ (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	తిరిగి;
    }

    l3 = list_alloc();
    ఒకవేళ (l3 == ఖాళీ)
	తిరిగి;

    ఒకవేళ (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	తిరిగి;
    }

    list_append_సంఖ్య(l2, curbuf->b_fnum);
    list_append_సంఖ్య(l2, p1.lnum);
    list_append_సంఖ్య(l2, p1.నిలువు);
    list_append_సంఖ్య(l2, p1.coladd);

    list_append_సంఖ్య(l3, curbuf->b_fnum);
    list_append_సంఖ్య(l3, p2.lnum);
    list_append_సంఖ్య(l3, p2.నిలువు);
    list_append_సంఖ్య(l3, p2.coladd);
}

/*
 * Compute the positions of the region segment on line "lnum".
 * "ret_p1" is set to the start position of the segment and "ret_p2" to its
 * end position.
 */
    స్థిర శూన్య
getregionpos_పంక్తి(
    linenr_T	lnum,
    pos_T	p1,
    pos_T	p2,
    పూర్ణాంకం		inclusive,
    పూర్ణాంకం		region_type,
    oparg_T	*oap,
    పూర్ణాంకం		allow_eol,
    pos_T	*ret_p1,
    pos_T	*ret_p2)
{
    char_u	*పంక్తి = ml_get(lnum);
    colnr_T	పంక్తి_len = ml_get_len(lnum);

    ఒకవేళ (region_type == MLINE)
    {
	ret_p1->నిలువు = 1;
	ret_p1->coladd = 0;
	ret_p2->నిలువు = MAXCOL;
	ret_p2->coladd = 0;
    }
    లేకపోతే
    {
	నిర్మాణం ఖండం_def	bd;

	ఒకవేళ (region_type == MBLOCK)
	    ఖండం_prep(oap, &bd, lnum, FALSE);
	లేకపోతే
	    charwise_ఖండం_prep(p1, p2, &bd, lnum, inclusive);

	ఒకవేళ (bd.is_oneChar)  // selection entirely inside one char
	{
	    ఒకవేళ (region_type == MBLOCK)
	    {
		ret_p1->నిలువు = mb_prevptr(పంక్తి, bd.textstart) - పంక్తి + 1;
		ret_p1->coladd = bd.ప్రారంభం_char_vcols
					   - (bd.ప్రారంభం_vcol - oap->ప్రారంభం_vcol);
	    }
	    లేకపోతే
	    {
		ret_p1->నిలువు = p1.నిలువు + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	లేకపోతే ఒకవేళ (region_type == MBLOCK && oap->ప్రారంభం_vcol > bd.ప్రారంభం_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->నిలువు = MAXCOL;
	    ret_p1->coladd = oap->ప్రారంభం_vcol - bd.ప్రారంభం_vcol;
	    bd.is_oneChar = TRUE;
	}
	లేకపోతే ఒకవేళ (bd.startspaces > 0)
	{
	    ret_p1->నిలువు = mb_prevptr(పంక్తి, bd.textstart) - పంక్తి + 1;
	    ret_p1->coladd = bd.ప్రారంభం_char_vcols - bd.startspaces;
	}
	లేకపోతే
	{
	    ret_p1->నిలువు = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	ఒకవేళ (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->నిలువు = ret_p1->నిలువు;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	లేకపోతే ఒకవేళ (bd.endspaces > 0)
	{
	    ret_p2->నిలువు = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	లేకపోతే
	{
	    ret_p2->నిలువు = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    ఒకవేళ (!allow_eol && ret_p1->నిలువు > పంక్తి_len)
    {
	ret_p1->నిలువు = 0;
	ret_p1->coladd = 0;
    }
    లేకపోతే ఒకవేళ (ret_p1->నిలువు > పంక్తి_len + 1)
	ret_p1->నిలువు = పంక్తి_len + 1;

    ఒకవేళ (!allow_eol && ret_p2->నిలువు > పంక్తి_len)
    {
	ret_p2->నిలువు = ret_p1->నిలువు == 0 ? 0 : పంక్తి_len;
	ret_p2->coladd = 0;
    }
    లేకపోతే ఒకవేళ (ret_p2->నిలువు > పంక్తి_len + 1)
	ret_p2->నిలువు = పంక్తి_len + 1;

    ret_p1->lnum = lnum;
    ret_p2->lnum = lnum;
}

/*
 * "getregionpos()" function
 */
    స్థిర శూన్య
f_getregionpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	p1, p2;
    పూర్ణాంకం		inclusive = TRUE;
    పూర్ణాంకం		region_type = -1;
    పూర్ణాంకం		allow_eol = FALSE;
    పూర్ణాంకం		bounds_only = FALSE;
    oparg_T	oa;
    పూర్ణాంకం		lnum;

    buf_T	*save_curbuf;
    పూర్ణాంకం		save_virtual;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    ఒకవేళ (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    ఒకవేళ (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	ప్రారంభం_pos, చివర_pos;

	getregionpos_పంక్తి(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &ప్రారంభం_pos, &చివర_pos);
	ఒకవేళ (p2.lnum != p1.lnum)
	{
	    pos_T	వాడని_2;

	    getregionpos_పంక్తి(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &వాడని_2, &చివర_pos);
	}
	కూడు_regionpos_range(rettv, ప్రారంభం_pos, చివర_pos);
    }
    లేకపోతే
    {
	కొరకు (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_పంక్తి(lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &ret_p1, &ret_p2);
	    కూడు_regionpos_range(rettv, ret_p1, ret_p2);
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_బఫరు = curbuf;
    virtual_op = save_virtual;
}

/*
 * Common between getreg(), getreginfo() and getregtype(): get the register
 * name from the first argument.
 * Returns zero on error.
 */
    స్థిర పూర్ణాంకం
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	ఒకవేళ (strregname != ఖాళీ && లోపల_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_రిజిస్టరు_name_must_be_one_char_str), strregname);
	    strregname = ఖాళీ;
	}
	ఒకవేళ (strregname == ఖాళీ)	    // type error; errmsg already given
	    తిరిగి 0;
    }
    లేకపోతే
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    తిరిగి *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    స్థిర శూన్య
f_getreg(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		regname;
    పూర్ణాంకం		arg2 = FALSE;
    పూర్ణాంకం		return_list = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	తిరిగి;

    regname = getreg_get_regname(argvars);
    ఒకవేళ (regname == 0)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	పూర్ణాంకం		error = FALSE;

	arg2 = (పూర్ణాంకం)tv_get_bool_chk(&argvars[1], &error);

	ఒకవేళ (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (పూర్ణాంకం)tv_get_bool_chk(&argvars[2], &error);
	ఒకవేళ (error)
	    తిరిగి;
    }

    ఒకవేళ (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_సమాసం_SRC : 0) | GREG_LIST);
	ఒకవేళ (rettv->vval.v_list == ఖాళీ)
	    (శూన్య)rettv_list_alloc(rettv);
	లేకపోతే
	    ++rettv->vval.v_list->lv_refcount;
    }
    లేకపోతే
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = get_reg_contents(regname,
						    arg2 ? GREG_సమాసం_SRC : 0);
    }
}

/*
 * "getregtype()" function
 */
    స్థిర శూన్య
f_getregtype(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		regname;
    char_u	buf[NUMBUFLEN + 2];
    దీర్ఘ	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    regname = getreg_get_regname(argvars);
    ఒకవేళ (regname == 0)
	తిరిగి;

    buf[0] = NUL;
    buf[1] = NUL;
    ఎంపిక (get_reg_type(regname, &reglen))
    {
	స్థితి MLINE: buf[0] = 'V'; ఆపు;
	స్థితి MCHAR: buf[0] = 'v'; ఆపు;
	స్థితి MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((అక్షరం *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		ఆపు;
    }
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "gettagstack()" function
 */
    స్థిర శూన్య
f_gettagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;			// default is current window

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
	ఒకవేళ (wp == ఖాళీ)
	    తిరిగి;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    స్థిర శూన్య
f_gettext(typval_T *argvars, typval_T *rettv)
{
#ఒకవేళ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    అక్షరం *prev = ఖాళీ;
#అయితే_ముగింపు

    ఒకవేళ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_STRING;

    ఒకవేళ (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != ఖాళీ &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#ఒకవేళ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((స్థిరరాశి అక్షరం *)argvars[1].vval.v_string, (అక్షరం *)p_enc);
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((స్థిరరాశి అక్షరం *)argvars[1].vval.v_string, (స్థిరరాశి అక్షరం *)argvars[0].vval.v_string));
#లేకపోతే
	textdomain((స్థిరరాశి అక్షరం *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	ఒకవేళ (prev != ఖాళీ)
	    bind_textdomain_codeset((స్థిరరాశి అక్షరం *)argvars[1].vval.v_string, prev);
#అయితే_ముగింపు
    }
    లేకపోతే
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
}

// for VIM_VERSION_ defines
#అన్తర్భావ "version.h"

/*
 * "has()" function
 */
    శూన్య
f_has(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		i;
    char_u	*name;
    పూర్ణాంకం		x = FALSE;
    పూర్ణాంకం		n = FALSE;
    రకనిర్వచనం నిర్మాణం {
	అక్షరం *name;
	పొట్టి present;
    } has_అంశం_T;
    స్థిర has_అంశం_T has_list[] =
    {
	{"amiga",
#నిర్వచితమైతే AMIGA
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"android",
#నిర్వచితమైతే __ANDROID__
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"arp",
#ఒకవేళ defined(AMIGA) && defined(FEAT_ARP)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"haiku",
#నిర్వచితమైతే __HAIKU__
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"bsd",
#ఒకవేళ defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"hpux",
#నిర్వచితమైతే hpux
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"hurd",
#నిర్వచితమైతే __GNU__
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"linux",
#నిర్వచితమైతే __linux__
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#నిర్వచితమైతే MACOS_X
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"osx",		// Mac OS X
#నిర్వచితమైతే MACOS_X
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"macunix",	// Mac OS X, with the darwin feature
#ఒకవేళ defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"osxdarwin",	// synonym for macunix
#ఒకవేళ defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"qnx",
#నిర్వచితమైతే __QNX__
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"sun",
#నిర్వచితమైతే SUN_SYSTEM
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"termux",
#నిర్వచితమైతే __TERMUX__
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"unix",
#నిర్వచితమైతే UNIX
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"vms",
#నిర్వచితమైతే VMS
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"win32",
#నిర్వచితమైతే MSWIN
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"win32unix",
#నిర్వచితమైతే WIN32UNIX
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"win64",
#నిర్వచితమైతే _WIN64
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"ebcdic", 0 },
	{"fname_case",
#నిర్వచితం_కాకపోతే CASE_INSENSITIVE_FILENAME
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"acl",
#నిర్వచితమైతే HAVE_ACL
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"arabic",
#నిర్వచితమైతే FEAT_ARABIC
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"autocmd", 1},
	{"autochdir",
#నిర్వచితమైతే FEAT_AUTOCHDIR
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"autoservername",
#నిర్వచితమైతే FEAT_AUTOSERVERNAME
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"socketserver",
#నిర్వచితమైతే FEAT_SOCKETSERVER
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"balloon_eval",
#నిర్వచితమైతే FEAT_BEVAL_GUI
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"balloon_multiline",
#నిర్వచితమైతే FEAT_BEVAL_GUI
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"balloon_eval_term",
#నిర్వచితమైతే FEAT_BEVAL_పదం
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#ఒకవేళ defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"byte_offset",
#నిర్వచితమైతే FEAT_BYTEOFF
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"channel",
#నిర్వచితమైతే FEAT_పని_CHANNEL
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"cindent", 1},
	{"clientserver",
#నిర్వచితమైతే FEAT_CLIENTSERVER
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"clipboard",
#నిర్వచితమైతే FEAT_CLIPBOARD
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"clipboard_provider",
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"cmdline_compl", 1},
	{"cmdline_hist", 1},
	{"cmdwin", 1},
	{"comments", 1},
	{"conceal",
#నిర్వచితమైతే FEAT_CONCEAL
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"cryptv",
#నిర్వచితమైతే FEAT_CRYPT
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"crypt-blowfish",
#నిర్వచితమైతే FEAT_CRYPT
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"crypt-blowfish2",
#నిర్వచితమైతే FEAT_CRYPT
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"cscope",
#నిర్వచితమైతే FEAT_CSCOPE
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"cursorbind", 1},
	{"cursorshape",
#నిర్వచితమైతే CURSOR_SHAPE
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"debug",
#నిర్వచితమైతే DEBUG
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"dialog_con",
#నిర్వచితమైతే FEAT_CON_DIALOG
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"dialog_con_gui",
#ఒకవేళ defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"dialog_gui",
#నిర్వచితమైతే FEAT_GUI_DIALOG
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"diff",
#నిర్వచితమైతే FEAT_DIFF
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"digraphs",
#నిర్వచితమైతే FEAT_DIGRAPHS
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"directx",
#నిర్వచితమైతే FEAT_DIRECTX
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"dnd",
#నిర్వచితమైతే FEAT_DND
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"drop_file",
#నిర్వచితమైతే HAVE_DROP_FILE
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"emacs_tags",
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#నిర్వచితమైతే FEAT_శోధన_EXTRA
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"file_in_path", 1},
	{"filterpipe",
#ఒకవేళ defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"find_in_path",
#నిర్వచితమైతే FEAT_కనుగొను_ID
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"float", 1},
	{"folding",
#నిర్వచితమైతే FEAT_FOLDING
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"footer", 0},
	{"fork",
#ఒకవేళ !defined(USE_SYSTEM) && defined(UNIX)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"gettext",
#నిర్వచితమైతే FEAT_GETTEXT
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"gui",
#నిర్వచితమైతే FEAT_GUI
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#నిర్వచితమైతే FEAT_GUI_GTK
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"gui_gtk2",
#ఒకవేళ defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"gui_gtk3",
#ఒకవేళ defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"gui_gtk4",
#ఒకవేళ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"gui_gnome",
#నిర్వచితమైతే FEAT_GUI_GNOME
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"gui_haiku",
#నిర్వచితమైతే FEAT_GUI_HAIKU
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"gui_mac", 0},
	{"gui_motif",
#నిర్వచితమైతే FEAT_GUI_MOTIF
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"gui_photon",
#నిర్వచితమైతే FEAT_GUI_PHOTON
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"gui_win32",
#నిర్వచితమైతే FEAT_GUI_MSWIN
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"iconv",
#ఒకవేళ defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"image",
#నిర్వచితమైతే FEAT_IMAGE
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"image_popup",
#నిర్వచితమైతే FEAT_IMAGE_POPUP
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"insert_expand", 1},
	{"ipv6",
#నిర్వచితమైతే FEAT_IPV6
		1
#లేకపోతే
		0
#అయితే_ముగింపు
	},
	{"job",
#నిర్వచితమైతే FEAT_పని_CHANNEL
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"jumplist", 1},
	{"keymap",
#నిర్వచితమైతే FEAT_KEYMAP
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#నిర్వచితమైతే FEAT_LANGMAP
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"libcall",
#నిర్వచితమైతే FEAT_LIBCALL
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"linebreak",
#నిర్వచితమైతే FEAT_LINEBREAK
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#ఒకవేళ defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"menu",
#నిర్వచితమైతే FEAT_MENU
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"mksession",
#నిర్వచితమైతే FEAT_SESSION
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#నిర్వచితమైతే FEAT_MOUSESHAPE
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"mouse_dec",
#ఒకవేళ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"mouse_gpm",
#ఒకవేళ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"mouse_jsbterm",
#ఒకవేళ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"mouse_netterm",
#ఒకవేళ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"mouse_pterm",
#ఒకవేళ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"mouse_sgr",
#ఒకవేళ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"mouse_sysmouse",
#ఒకవేళ (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"mouse_urxvt",
#ఒకవేళ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"mouse_xterm",
#ఒకవేళ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#నిర్వచితమైతే FEAT_MBYTE_IME
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"multi_lang",
#నిర్వచితమైతే FEAT_MULTI_LANG
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"mzscheme",
#ఒకవేళ defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"nanotime",
#నిర్వచితమైతే ST_MTIM_NSEC
		1
#లేకపోతే
		0
#అయితే_ముగింపు
	},
	{"num64", 1},
	{"ole",
#నిర్వచితమైతే FEAT_OLE
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"packages",
#నిర్వచితమైతే FEAT_EVAL
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"path_extra", 1},
	{"perl",
#ఒకవేళ defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"persistent_undo",
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"python_compiled",
#ఒకవేళ defined(FEAT_PYTHON)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"python_dynamic",
#ఒకవేళ defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"python",
#ఒకవేళ defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"pythonx",
#ఒకవేళ (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"python3_compiled",
#ఒకవేళ defined(FEAT_PYTHON3)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"python3_dynamic",
#ఒకవేళ defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"python3_stable",
#ఒకవేళ defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"python3",
#ఒకవేళ defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"popupwin",
#నిర్వచితమైతే FEAT_PROP_POPUP
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"postscript",
#నిర్వచితమైతే FEAT_POSTSCRIPT
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"pango",
#నిర్వచితమైతే FEAT_PRINT_PANGO
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"printer",
#నిర్వచితమైతే FEAT_PRINTER
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"profile",
#నిర్వచితమైతే FEAT_PROFILE
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"prof_nsec",
#నిర్వచితమైతే PROF_NSEC
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"reltime",
#నిర్వచితమైతే FEAT_RELTIME
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"quickfix",
#నిర్వచితమైతే FEAT_QUICKFIX
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"rightleft",
#నిర్వచితమైతే FEAT_RIGHTLEFT
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"ruby",
#ఒకవేళ defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#నిర్వచితమైతే FEAT_SIGNS
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"smartindent", 1},
	{"startuptime",
#నిర్వచితమైతే STARTUPTIME
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"statusline",
#నిర్వచితమైతే FEAT_STL_OPT
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"statusline_click",
#నిర్వచితమైతే FEAT_STL_OPT
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"netbeans_intg",
#నిర్వచితమైతే FEAT_NETBEANS_INTG
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"sodium",
#ఒకవేళ defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"sound",
#నిర్వచితమైతే FEAT_SOUND
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"spell",
#నిర్వచితమైతే FEAT_SPELL
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"syntax",
#నిర్వచితమైతే FEAT_SYN_HL
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"system",
#ఒకవేళ defined(USE_SYSTEM) || !defined(UNIX)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"tabpanel",
#ఒకవేళ defined(FEAT_TABPANEL)
		1,
#లేకపోతే
		0,
#అయితే_ముగింపు
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#ఒకవేళ defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"termguicolors",
#నిర్వచితమైతే FEAT_TERMGUICOLORS
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"terminal",
#ఒకవేళ defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"terminfo",
#నిర్వచితమైతే TERMINFO
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"termresponse",
#నిర్వచితమైతే FEAT_TERMRESPONSE
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"textobjects", 1},
	{"textprop",
#నిర్వచితమైతే FEAT_PROP_POPUP
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"tgetent",
#నిర్వచితమైతే HAVE_TGETENT
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"timers",
#నిర్వచితమైతే FEAT_TIMERS
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"title", 1},
	{"toolbar",
#నిర్వచితమైతే FEAT_TOOLBAR
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#నిర్వచితమైతే FEAT_VARTABS
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"vertsplit", 1},
	{"viminfo",
#నిర్వచితమైతే FEAT_VIMINFO
		1
#లేకపోతే
		0
#అయితే_ముగింపు
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
#నిర్వచితమైతే FEAT_VTP
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"wayland",
#నిర్వచితమైతే FEAT_WAYLAND
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"wayland_clipboard",
#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#నిర్వచితమైతే FEAT_WAK
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"writebackup",
#నిర్వచితమైతే FEAT_WRITEBACKUP
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"xattr",
#నిర్వచితమైతే FEAT_XATTR
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"xim",
#నిర్వచితమైతే FEAT_XIM
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"xfontset",
#నిర్వచితమైతే FEAT_XFONTSET
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"xpm",
#ఒకవేళ defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"xpm_w32",	// for backward compatibility
#నిర్వచితమైతే FEAT_XPM_W32
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"xsmp",
#నిర్వచితమైతే USE_XSMP
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"xsmp_interact",
#నిర్వచితమైతే USE_XSMP_INTERACT
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"xterm_clipboard",
#నిర్వచితమైతే FEAT_XCLIPBOARD
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"xterm_save",
#నిర్వచితమైతే FEAT_XTERM_SAVE
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{"X11",
#ఒకవేళ defined(UNIX) && defined(FEAT_X11)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#ఒకవేళ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#లేకపోతే
		0
#అయితే_ముగింపు
		},
	{ఖాళీ, 0}
    };

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	తిరిగి;

    name = tv_get_string(&argvars[0]);

    // Fast-path: check features not in has_list[] first to avoid the full
    // linear scan for very common queries like has('patch-...').
    ఒకవేళ (STRNICMP(name, "patch", 5) == 0)
    {
	x = TRUE;
	ఒకవేళ (name[5] == '-'
		&& STRLEN(name) >= 11
		&& (name[6] >= '1' && name[6] <= '9'))
	{
	    అక్షరం	*చివర;
	    పూర్ణాంకం	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (పూర్ణాంకం)strtoul((అక్షరం *)name + 6, &చివర, 10);
	    ఒకవేళ (*చివర == '.' && vim_isdigit(చివర[1])
		    && చివర[2] == '.' && vim_isdigit(చివర[3]))
	    {
		minor = atoi(చివర + 1);

		// Expect "patch-9.9.01234".
		n = (major < VIM_సంస్కరణ_MAJOR
		     || (major == VIM_సంస్కరణ_MAJOR
			 && (minor < VIM_సంస్కరణ_MINOR
			     || (minor == VIM_సంస్కరణ_MINOR
				 && has_patch(atoi(చివర + 3))))));
	    }
	}
	లేకపోతే ఒకవేళ (సురక్షిత_isdigit(name[5]))
	    n = has_patch(atoi((అక్షరం *)name + 5));
    }
    లేకపోతే ఒకవేళ (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    లేకపోతే ఒకవేళ (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_ఆదానం_isatty();
    }
    లేకపోతే ఒకవేళ (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    లేకపోతే ఒకవేళ (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    లేకపోతే ఒకవేళ (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#నిర్వచితమైతే FEAT_GUI
	n = (gui.లోపల_use || gui.starting);
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#ఒకవేళ defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.లోపల_use;	// gui_mch_browse() works when GUI is running
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#నిర్వచితమైతే FEAT_SYN_HL
	n = syntax_present(curwin);
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#నిర్వచితమైతే FEAT_VTP
	n = is_పదం_win32() && has_vtp_working();
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	n = netbeans_చురుకైన();
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#నిర్వచితమైతే FEAT_MOUSE_GPM
	n = gpm_enabled();
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#ఒకవేళ defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#నిర్వచితమైతే FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_నక్షత్రం.అందుబాటు;
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#నిర్వచితమైతే FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# ఒకవేళ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	ఒకవేళ (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	లేకపోతే
	    n = FALSE;
# లేకపోతే
	n = FALSE;
# endif
#అయితే_ముగింపు
    }

    // Look up in has_list[] only if not already handled above.
    ఒకవేళ (x == FALSE)
    {
	కొరకు (i = 0; has_list[i].name != ఖాళీ; ++i)
	    ఒకవేళ (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		ఆపు;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	ఒకవేళ (x == TRUE && n == FALSE)
	{
	    ఒకవేళ (0)
	    {
		// intentionally empty
	    }
#నిర్వచితమైతే VIMDLL
	    లేకపోతే ఒకవేళ (STRICMP(name, "filterpipe") == 0)
		n = gui.లోపల_use || gui.starting;
#అయితే_ముగింపు
#ఒకవేళ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    లేకపోతే ఒకవేళ (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_LUA
	    లేకపోతే ఒకవేళ (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_MZSCHEME
	    లేకపోతే ఒకవేళ (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_PERL
	    లేకపోతే ఒకవేళ (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_PYTHON
	    లేకపోతే ఒకవేళ (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_PYTHON3
	    లేకపోతే ఒకవేళ (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#అయితే_ముగింపు
#ఒకవేళ defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    లేకపోతే ఒకవేళ (STRICMP(name, "pythonx") == 0)
	    {
# ఒకవేళ defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		ఒకవేళ (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		లేకపోతే ఒకవేళ (p_pyx == 3)
		    n = python3_enabled(FALSE);
		లేకపోతే ఒకవేళ (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_RUBY
	    లేకపోతే ఒకవేళ (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_TCL
	    లేకపోతే ఒకవేళ (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_SODIUM
	    లేకపోతే ఒకవేళ (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_TERMINAL) && defined(MSWIN)
	    లేకపోతే ఒకవేళ (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_GPM
	    లేకపోతే ఒకవేళ (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_అందుబాటు();
#అయితే_ముగింపు
	}
    }

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_సంఖ్య = x;
    లేకపోతే
	// return whether feature is enabled
	rettv->vval.v_సంఖ్య = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    పూర్ణాంకం
dynamic_feature(char_u *feature)
{
    తిరిగి (feature == ఖాళీ
#ఒకవేళ defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.లోపల_use)
#అయితే_ముగింపు
#నిర్వచితమైతే VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.లోపల_use)
#అయితే_ముగింపు
#ఒకవేళ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    || STRICMP(feature, "iconv") == 0
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_LUA
	    || STRICMP(feature, "lua") == 0
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_GPM
	    || (STRICMP(feature, "mouse_gpm_enabled") == 0 && !gpm_enabled())
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_MZSCHEME
	    || STRICMP(feature, "mzscheme") == 0
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	    || STRICMP(feature, "netbeans_enabled") == 0
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_PERL
	    || STRICMP(feature, "perl") == 0
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_PYTHON
	    || STRICMP(feature, "python") == 0
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_PYTHON3
	    || STRICMP(feature, "python3") == 0
#అయితే_ముగింపు
#ఒకవేళ defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    || STRICMP(feature, "pythonx") == 0
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_RUBY
	    || STRICMP(feature, "ruby") == 0
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
	    || STRICMP(feature, "syntax_items") == 0
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_TCL
	    || STRICMP(feature, "tcl") == 0
#అయితే_ముగింపు
	    // once "starting" is zero it will stay that way
	    || (STRICMP(feature, "vim_starting") == 0 && starting != 0)
	    || STRICMP(feature, "multi_byte_encoding") == 0
#ఒకవేళ defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#అయితే_ముగింపు
	    );
}

/*
 * "haslocaldir()" function
 */
    స్థిర శూన్య
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = ఖాళీ;
    win_T	*wp = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL)))
	తిరిగి;

    wp = కనుగొను_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    ఒకవేళ (wp != ఖాళీ && wp->w_localdir != ఖాళీ)
	rettv->vval.v_సంఖ్య = 1;
    లేకపోతే ఒకవేళ (tp != ఖాళీ && tp->tp_localdir != ఖాళీ)
	rettv->vval.v_సంఖ్య = 2;
    లేకపోతే
	rettv->vval.v_సంఖ్య = 0;
}

/*
 * "highlightID(name)" function
 */
    స్థిర శూన్య
f_hlID(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    స్థిర శూన్య
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    స్థిర శూన్య
f_hostname(typval_T *argvars వాడని, typval_T *rettv)
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
#నిర్వచితమైతే VMS // VMS does not have defined uintptr_t
# ఒకవేళ defined(HAVE_NO_LONG_LONG)
రకనిర్వచనం గుర్తులేని పూర్ణాంకం uintptr_t;
# లేకపోతే
రకనిర్వచనం గుర్తులేని దీర్ఘ దీర్ఘ uintptr_t;
# endif
#అయితే_ముగింపు // VMS

    స్థిర శూన్య
f_id(typval_T *argvars, typval_T *rettv)
{
    అక్షరం    numbuf[NUMBUFLEN];
    అక్షరం    *p = numbuf;

    ఎంపిక (argvars[0].v_type)
    {
	స్థితి VAR_LIST:
	స్థితి VAR_TUPLE:
	స్థితి VAR_DICT:
	స్థితి VAR_OBJECT:
	స్థితి VAR_పని:
	స్థితి VAR_CHANNEL:
	స్థితి VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    ఒకవేళ (argvars[0].vval.v_object != ఖాళీ)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(శూన్య *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		కొరకు (; p < numbuf + పరిమాణం(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	అప్రమేయ:
	    ఆపు;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    స్థిర శూన్య
సూచీ_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    పూర్ణాంకం		ప్రారంభం = 0;
    పూర్ణాంకం		error = FALSE;
    పూర్ణాంకం		ic = FALSE;

    b = argvars[0].vval.v_blob;
    ఒకవేళ (b == ఖాళీ)
	తిరిగి;

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ప్రారంభం = tv_get_సంఖ్య_chk(&argvars[2], &error);
	ఒకవేళ (error)
	    తిరిగి;
    }

    ఒకవేళ (ప్రారంభం < 0)
    {
	ప్రారంభం = blob_len(b) + ప్రారంభం;
	ఒకవేళ (ప్రారంభం < 0)
	    ప్రారంభం = 0;
    }

    కొరకు (పూర్ణాంకం idx = ప్రారంభం; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_సంఖ్య;
	tv.vval.v_సంఖ్య = blob_get(b, idx);
	ఒకవేళ (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_సంఖ్య = idx;
	    తిరిగి;
	}
    }
}

/*
 * index() function for a tuple
 */
    స్థిర శూన్య
సూచీ_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    పూర్ణాంకం		ic = FALSE;
    పూర్ణాంకం		error = FALSE;

    ఒకవేళ (tuple == ఖాళీ)
	తిరిగి;

    పూర్ణాంకం	ప్రారంభం_idx = 0;
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ప్రారంభం_idx = tv_get_సంఖ్య_chk(&argvars[2], &error);
	ఒకవేళ (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (పూర్ణాంకం)tv_get_bool_chk(&argvars[3], &error);
	ఒకవేళ (error)
	    తిరిగి;
    }

    rettv->vval.v_సంఖ్య = సూచీ_tuple(tuple, &argvars[1], ప్రారంభం_idx, ic);
}

/*
 * index() function for a list
 */
    స్థిర శూన్య
సూచీ_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*అంశం;
    దీర్ఘ	idx = 0;
    పూర్ణాంకం		ic = FALSE;
    పూర్ణాంకం		error = FALSE;

    l = argvars[0].vval.v_list;
    ఒకవేళ (l == ఖాళీ)
	తిరిగి;

    CHECK_LIST_MATERIALIZE(l);
    అంశం = l->lv_మొదటి;
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	అంశం = list_కనుగొను(l, (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	ఒకవేళ (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (పూర్ణాంకం)tv_get_bool_chk(&argvars[3], &error);
	ఒకవేళ (error)
	    అంశం = ఖాళీ;
    }

    కొరకు ( ; అంశం != ఖాళీ; అంశం = అంశం->li_తదుపరి, ++idx)
	ఒకవేళ (tv_equal(&అంశం->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_సంఖ్య = idx;
	    ఆపు;
	}
}

/*
 * "index()" function
 */
    స్థిర శూన్య
f_సూచీ(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_సంఖ్య_arg(argvars, 1) == FAIL)
		|| check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_BLOB)
	సూచీ_func_blob(argvars, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_TUPLE)
	సూచీ_func_tuple(argvars, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_LIST)
	సూచీ_func_list(argvars, rettv);
    లేకపోతే
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    పూర్ణాంకం
indexof_eval_సమాసం(typval_T *సమాసం)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	దొరికింది;
    పూర్ణాంకం		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_కీలకం);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    ఒకవేళ (eval_సమాసం_typval(సమాసం, FALSE, argv, 2, ఖాళీ, &newtv) == FAIL)
	తిరిగి FALSE;

    దొరికింది = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    తిరిగి error ? FALSE : దొరికింది;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    స్థిర పూర్ణాంకం
indexof_blob(blob_T *b, దీర్ఘ startidx, typval_T *సమాసం)
{
    దీర్ఘ	idx = 0;

    ఒకవేళ (b == ఖాళీ)
	తిరిగి -1;

    ఒకవేళ (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	ఒకవేళ (startidx < 0)
	    startidx = 0;
    }

    సమితి_vim_var_type(VV_కీలకం, VAR_సంఖ్య);
    సమితి_vim_var_type(VV_VAL, VAR_సంఖ్య);

    పూర్ణాంకం		called_emsg_ప్రారంభం = called_emsg;
    కొరకు (idx = startidx; idx < blob_len(b); ++idx)
    {
	సమితి_vim_var_nr(VV_కీలకం, idx);
	సమితి_vim_var_nr(VV_VAL, blob_get(b, idx));

	ఒకవేళ (indexof_eval_సమాసం(సమాసం))
	    తిరిగి idx;

	ఒకవేళ (called_emsg != called_emsg_ప్రారంభం)
	    తిరిగి -1;
    }

    తిరిగి -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    స్థిర పూర్ణాంకం
indexof_list(list_T *l, దీర్ఘ startidx, typval_T *సమాసం)
{
    listitem_T	*అంశం;
    దీర్ఘ	idx = 0;
    పూర్ణాంకం		దొరికింది;

    ఒకవేళ (l == ఖాళీ)
	తిరిగి -1;

    CHECK_LIST_MATERIALIZE(l);

    ఒకవేళ (startidx == 0)
	అంశం = l->lv_మొదటి;
    లేకపోతే
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	అంశం = list_కనుగొను(l, startidx);
	ఒకవేళ (అంశం != ఖాళీ)
	    idx = l->lv_u.mat.lv_idx;
    }

    సమితి_vim_var_type(VV_కీలకం, VAR_సంఖ్య);

    పూర్ణాంకం		called_emsg_ప్రారంభం = called_emsg;
    కొరకు ( ; అంశం != ఖాళీ; అంశం = అంశం->li_తదుపరి, ++idx)
    {
	సమితి_vim_var_nr(VV_కీలకం, idx);
	copy_tv(&అంశం->li_tv, get_vim_var_tv(VV_VAL));

	దొరికింది = indexof_eval_సమాసం(సమాసం);
	clear_tv(get_vim_var_tv(VV_VAL));

	ఒకవేళ (దొరికింది)
	    తిరిగి idx;

	ఒకవేళ (called_emsg != called_emsg_ప్రారంభం)
	    తిరిగి -1;
    }

    తిరిగి -1;
}

/*
 * "indexof()" function
 */
    స్థిర శూన్య
f_indexof(typval_T *argvars, typval_T *rettv)
{
    దీర్ఘ	startidx = 0;
    typval_T	save_val;
    typval_T	save_కీలకం;
    పూర్ణాంకం		save_did_emsg;

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	తిరిగి;

    ఒకవేళ ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == ఖాళీ
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == ఖాళీ))
	తిరిగి;

    ఒకవేళ (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_సంఖ్య_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_కీలకం, &save_కీలకం);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    ఒకవేళ (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_సంఖ్య = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_సంఖ్య = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    లేకపోతే
	rettv->vval.v_సంఖ్య = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_కీలకం, &save_కీలకం);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

స్థిర పూర్ణాంకం inputsecret_flag = 0;

/*
 * "input()" function
 *     Also handles inputsecret() when inputsecret is set.
 */
    స్థిర శూన్య
f_ఆదానం(typval_T *argvars, typval_T *rettv)
{
    get_user_ఆదానం(argvars, rettv, FALSE, inputsecret_flag);
}

/*
 * "inputdialog()" function
 */
    స్థిర శూన్య
f_inputdialog(typval_T *argvars, typval_T *rettv)
{
#ఒకవేళ defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    ఒకవేళ (gui.లోపల_use && vim_strchr(p_go, GO_CONDIALOG) == ఖాళీ)
    {
	char_u	*సందేశం_2;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	ఒకవేళ (లోపల_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    తిరిగి;

	సందేశం_2 = tv_get_string_chk(&argvars[0]);
	ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != ఖాళీ)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	లేకపోతే
	    IObuff[0] = NUL;
	ఒకవేళ (సందేశం_2 != ఖాళీ && defstr != ఖాళీ
		&& do_dialog(VIM_QUESTION, ఖాళీ, సందేశం_2,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	లేకపోతే
	{
	    ఒకవేళ (సందేశం_2 != ఖాళీ && defstr != ఖాళీ
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    లేకపోతే
		rettv->vval.v_string = ఖాళీ;
	}
	rettv->v_type = VAR_STRING;
    }
    లేకపోతే
#అయితే_ముగింపు
	get_user_ఆదానం(argvars, rettv, TRUE, inputsecret_flag);
}

/*
 * "inputlist()" function
 */
    స్థిర శూన్య
f_inputlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    పూర్ణాంకం		selected;
    పూర్ణాంకం		mouse_వాడిన;

#నిర్వచితమైతే NO_CONSOLE_ఆదానం
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    ఒకవేళ (no_console_ఆదానం() && !is_not_a_పదం())
	తిరిగి;
#అయితే_ముగింపు
    ఒకవేళ (లోపల_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ఖాళీ)
    {
	semsg(_(e_వాదం_యొక్క_str_must_be_list), "inputlist()");
	తిరిగి;
    }

    సందేశం_ప్రారంభం();
    సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;	// for when 'cmdheight' > 1
    పంక్తులు_ఎడమ = అడ్డువరుసలు;	// avoid more prompt
    సందేశం_scroll = TRUE;
    సందేశం_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	సందేశం_puts((అక్షరం *)tv_get_string(&li->li_tv));
	సందేశం_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_సంఖ్య(&mouse_వాడిన);
    ఒకవేళ (mouse_వాడిన)
	selected -= పంక్తులు_ఎడమ;

    rettv->vval.v_సంఖ్య = selected;
}

స్థిర garray_T	    ga_userinput = {0, 0, పరిమాణం(tasave_T), 4, ఖాళీ};

/*
 * "inputrestore()" function
 */
    స్థిర శూన్య
f_inputrestore(typval_T *argvars వాడని, typval_T *rettv)
{
    ఒకవేళ (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_దత్తాంశం)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    లేకపోతే ఒకవేళ (p_verbose > 1)
    {
	verb_సందేశం(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_సంఖ్య = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    స్థిర శూన్య
f_inputsave(typval_T *argvars వాడని, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    ఒకవేళ (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_దత్తాంశం)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    లేకపోతే
	rettv->vval.v_సంఖ్య = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    స్థిర శూన్య
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    ++cmdline_నక్షత్రం;
    ++inputsecret_flag;
    f_ఆదానం(argvars, rettv);
    --cmdline_నక్షత్రం;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    స్థిర శూన్య
f_అంతరాయం(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    స్థిర శూన్య
f_invert(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = ~tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    స్థిర శూన్య
free_lval_మూలం(lval_మూలం_T *మూలం)
{
    ఒకవేళ (మూలం->lr_tv != ఖాళీ)
	free_tv(మూలం->lr_tv);
    class_unref(మూలం->lr_cl_exec);
    మూలం->lr_tv = ఖాళీ;
    మూలం->lr_cl_exec = ఖాళీ;
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

    స్థిర పూర్ణాంకం
fix_variable_reference_lval_మూలం(lval_మూలం_T *మూలం, char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    పూర్ణాంకం దొరికింది_member = FALSE;
    ఒకవేళ (మూలం->lr_tv->v_type == VAR_OBJECT)
    {
	ఒకవేళ (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    దొరికింది_member = TRUE;
    }
    ఒకవేళ (!దొరికింది_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*చివర;
	కొరకు (చివర = name; ASCII_ISALNUM(*చివర) || *చివర == '_'; ++చివర)
	    ;
	పూర్ణాంకం idx = class_member_idx(మూలం->lr_cl_exec, name, చివర - name);
	ఒకవేళ (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(మూలం->lr_tv);
	    copy_tv(&మూలం->lr_cl_exec->class_members_tv[idx], మూలం->lr_tv);
	    దొరికింది_member = TRUE;
	}
    }
    ఒకవేళ (!దొరికింది_member)
    {
	free_tv(మూలం->lr_tv);
	మూలం->lr_tv = ఖాళీ;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    తిరిగి OK;
}

/*
 * "islocked()" function
 */
    స్థిర శూన్య
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*చివర;
    dictitem_T	*di;

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    char_u *name = tv_get_string(&argvars[0]);
#నిర్వచితమైతే LOG_LOCKVAR
    ch_log(ఖాళీ, "LKVAR: f_islocked(): name: %s", name);
#అయితే_ముగింపు

    lval_మూలం_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_మూలం_T *మూలం = ఖాళీ;

    // Set up lval_root if executing in a method.
    ఒకవేళ (fill_exec_lval_మూలం(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	ఒకవేళ (fix_variable_reference_lval_మూలం(&aroot, name) == OK)
	    మూలం = &aroot;
    }

    lval_మూలం_T	*lval_మూలం_save = lval_మూలం;
    lval_మూలం = మూలం;
    చివర = get_lval(name, ఖాళీ, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_ప్రారంభం);
    lval_మూలం = lval_మూలం_save;

    ఒకవేళ (చివర != ఖాళీ && lv.ll_name != ఖాళీ)
    {
	ఒకవేళ (*చివర != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_చివర
		   ? e_invalid_వాదం_str : e_trailing_characters_str), చివర);
	}
	లేకపోతే
	{
	    ఒకవేళ (lv.ll_tv == ఖాళీ)
	    {
		di = కనుగొను_var(lv.ll_name, ఖాళీ, TRUE);
		ఒకవేళ (di != ఖాళీ)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_సంఖ్య = ((di->di_flags & DI_FLAGS_తాళం)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    లేకపోతే ఒకవేళ (lv.ll_is_మూలం)
	    {
		rettv->vval.v_సంఖ్య = tv_islocked(lv.ll_tv);
	    }
	    లేకపోతే ఒకవేళ (lv.ll_object != ఖాళీ)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_సంఖ్య = tv_islocked(tv);
#నిర్వచితమైతే LOG_LOCKVAR
		ch_log(ఖాళీ, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#అయితే_ముగింపు
	    }
	    లేకపోతే ఒకవేళ (lv.ll_class != ఖాళీ)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_సంఖ్య = tv_islocked(tv);
#నిర్వచితమైతే LOG_LOCKVAR
		ch_log(ఖాళీ, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#అయితే_ముగింపు
	    }
	    లేకపోతే ఒకవేళ (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    లేకపోతే ఒకవేళ (lv.ll_newkey != ఖాళీ)
		semsg(_(e_కీలకం_not_present_లోపల_dictionary_str), lv.ll_newkey);
	    లేకపోతే ఒకవేళ (lv.ll_list != ఖాళీ)
		// List item.
		rettv->vval.v_సంఖ్య = tv_islocked(&lv.ll_li->li_tv);
	    లేకపోతే ఒకవేళ (lv.ll_tuple != ఖాళీ)
		// Tuple item.
		rettv->vval.v_సంఖ్య = tv_islocked(lv.ll_tv);
	    లేకపోతే
		// Dictionary item.
		rettv->vval.v_సంఖ్య = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    ఒకవేళ (మూలం != ఖాళీ)
	free_lval_మూలం(మూలం);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    స్థిర శూన్య
f_వస్తువులు(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    లేకపోతే
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_వాదం_nr), 1);
}

/*
 * "keytrans()" function
 */
    స్థిర శూన్య
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == ఖాళీ)
	తిరిగి;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    స్థిర శూన్య
f_చివరి_బఫరు_nr(typval_T *argvars వాడని, typval_T *rettv)
{
    పూర్ణాంకం		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ఒకవేళ (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_సంఖ్య = n;
}

/*
 * "len()" function
 */
    శూన్య
f_len(typval_T *argvars, typval_T *rettv)
{
    ఎంపిక (argvars[0].v_type)
    {
	స్థితి VAR_STRING:
	స్థితి VAR_సంఖ్య:
	    rettv->vval.v_సంఖ్య = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    ఆపు;
	స్థితి VAR_BLOB:
	    rettv->vval.v_సంఖ్య = blob_len(argvars[0].vval.v_blob);
	    ఆపు;
	స్థితి VAR_LIST:
	    rettv->vval.v_సంఖ్య = list_len(argvars[0].vval.v_list);
	    ఆపు;
	స్థితి VAR_TUPLE:
	    rettv->vval.v_సంఖ్య = tuple_len(argvars[0].vval.v_tuple);
	    ఆపు;
	స్థితి VAR_DICT:
	    rettv->vval.v_సంఖ్య = dict_len(argvars[0].vval.v_dict);
	    ఆపు;
	స్థితి VAR_OBJECT:
	    rettv->vval.v_సంఖ్య = object_len(argvars[0].vval.v_object);
	    ఆపు;
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	స్థితి VAR_FLOAT:
	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL:
	స్థితి VAR_పని:
	స్థితి VAR_CHANNEL:
	స్థితి VAR_INSTR:
	స్థితి VAR_CLASS:
	స్థితి VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    ఆపు;
    }
}

    స్థిర శూన్య
libcall_common(typval_T *argvars వాడని, typval_T *rettv, పూర్ణాంకం type)
{
#నిర్వచితమైతే FEAT_LIBCALL
    char_u		*string_లోపల;
    char_u		**string_ఫలితం;
    పూర్ణాంకం			nr_ఫలితం;
#అయితే_ముగింపు

    rettv->v_type = type;
    ఒకవేళ (type != VAR_సంఖ్య)
	rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_సంఖ్య_arg(argvars, 2) == FAIL))
	తిరిగి;

#నిర్వచితమైతే FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    ఒకవేళ (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_లోపల = ఖాళీ;
	ఒకవేళ (argvars[2].v_type == VAR_STRING)
	    string_లోపల = argvars[2].vval.v_string;
	ఒకవేళ (type == VAR_సంఖ్య)
	{
	    string_ఫలితం = ఖాళీ;
	}
	లేకపోతే
	{
	    rettv->vval.v_string = ఖాళీ;
	    string_ఫలితం = &rettv->vval.v_string;
	}
	ఒకవేళ (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_లోపల,
			     argvars[2].vval.v_సంఖ్య,
			     string_ఫలితం,
			     &nr_ఫలితం) == OK
		&& type == VAR_సంఖ్య)
	    rettv->vval.v_సంఖ్య = nr_ఫలితం;
    }
#అయితే_ముగింపు
}

/*
 * "libcall()" function
 */
    స్థిర శూన్య
f_libcall(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_STRING);
}

/*
 * "libcallnr()" function
 */
    స్థిర శూన్య
f_libcallnr(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_సంఖ్య);
}

/*
 * "line(string, [winid])" function
 */
    స్థిర శూన్య
f_పంక్తి(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum = 0;
    pos_T	*fp = ఖాళీ;
    పూర్ణాంకం		fnum;
    పూర్ణాంకం		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	ఒకవేళ (wp != ఖాళీ && tp != ఖాళీ)
	{
	    ఒకవేళ (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		ఒకవేళ (*p_spk != 'c'
#నిర్వచితమైతే FEAT_DIFF
		|| (curwin->w_p_diff && switchwin.sw_curwin->w_p_diff)
#అయితే_ముగింపు
		)
		    skip_update_topline = TRUE;
		check_cursor();
		fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	    }
	    skip_update_topline = FALSE;
	    restore_win_noblock(&switchwin, TRUE);
	}
    }
    లేకపోతే
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    ఒకవేళ (fp != ఖాళీ)
	lnum = fp->lnum;
    rettv->vval.v_సంఖ్య = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    స్థిర శూన్య
f_line2byte(typval_T *argvars వాడని, typval_T *rettv)
{
#నిర్వచితం_కాకపోతే FEAT_BYTEOFF
    rettv->vval.v_సంఖ్య = -1;
#లేకపోతే
    linenr_T	lnum;

    ఒకవేళ (లోపల_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	తిరిగి;

    lnum = tv_get_lnum(argvars);
    ఒకవేళ (lnum < 1 || lnum > curbuf->b_ml.ml_పంక్తి_లెక్క + 1)
	rettv->vval.v_సంఖ్య = -1;
    లేకపోతే
	rettv->vval.v_సంఖ్య = ml_కనుగొను_పంక్తి_or_స్థానభ్రంశం(curbuf, lnum, ఖాళీ);
    ఒకవేళ (rettv->vval.v_సంఖ్య >= 0)
	++rettv->vval.v_సంఖ్య;
#అయితే_ముగింపు
}

#నిర్వచితమైతే FEAT_LUA
/*
 * "luaeval()" function
 */
    స్థిర శూన్య
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#అయితే_ముగింపు

రకనిర్వచనం గణన
{
    సరిపోలిక_చివర_2,	    // matchend()
    సరిపోలిక_సరిపోలిక,    // match()
    సరిపోలిక_STR,	    // matchstr()
    సరిపోలిక_LIST,	    // matchlist()
    సరిపోలిక_POS	    // matchstrpos()
} matchtype_T;

    స్థిర శూన్య
కనుగొను_some_సరిపోలిక(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = ఖాళీ;
    దీర్ఘ	len = 0;
    char_u	*సమాసం = ఖాళీ;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    దీర్ఘ	ప్రారంభం = 0;
    దీర్ఘ	nth = 1;
    colnr_T	startcol = 0;
    పూర్ణాంకం		సరిపోలిక = 0;
    list_T	*l = ఖాళీ;
    listitem_T	*li = ఖాళీ;
    దీర్ఘ	idx = 0;
    పూర్ణాంకం		prev_తాళం = 0;
    char_u	*tofree = ఖాళీ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ఖాళీలు_option;

    rettv->vval.v_సంఖ్య = -1;
    ఒకవేళ (type == సరిపోలిక_LIST || type == సరిపోలిక_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	    వెళ్ళు theend;
	ఒకవేళ (type == సరిపోలిక_POS
		&& (list_append_string(rettv->vval.v_list,
					    (char_u *)"", 0) == FAIL
		    || list_append_సంఖ్య(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_సంఖ్య(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_సంఖ్య(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL))
	{
		list_free(rettv->vval.v_list);
		rettv->vval.v_list = ఖాళీ;
		వెళ్ళు theend;
	}
    }
    లేకపోతే ఒకవేళ (type == సరిపోలిక_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ఖాళీ;
    }

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 3) == FAIL)))
	వెళ్ళు theend;

    ఒకవేళ (argvars[0].v_type == VAR_LIST)
    {
	ఒకవేళ ((l = argvars[0].vval.v_list) == ఖాళీ)
	    వెళ్ళు theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_మొదటి;
    }
    లేకపోతే
    {
	సమాసం = str = tv_get_string(&argvars[0]);
	len = (దీర్ఘ)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    ఒకవేళ (pat == ఖాళీ)
	వెళ్ళు theend;

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	పూర్ణాంకం	    error = FALSE;

	ప్రారంభం = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[2], &error);
	ఒకవేళ (error)
	    వెళ్ళు theend;
	ఒకవేళ (l != ఖాళీ)
	{
	    li = list_కనుగొను(l, ప్రారంభం);
	    ఒకవేళ (li == ఖాళీ)
		వెళ్ళు theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	లేకపోతే
	{
	    ఒకవేళ (ప్రారంభం < 0)
		ప్రారంభం = 0;
	    ఒకవేళ (ప్రారంభం > len)
		వెళ్ళు theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    ఒకవేళ (argvars[3].v_type != VAR_UNKNOWN)
		startcol = ప్రారంభం;
	    లేకపోతే
	    {
		str += ప్రారంభం;
		len -= ప్రారంభం;
	    }
	}

	ఒకవేళ (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[3], &error);
	ఒకవేళ (error)
	    వెళ్ళు theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    ఒకవేళ (regmatch.regprog != ఖాళీ)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	ఒకవేళ (l != ఖాళీ)
	{
	    prev_తాళం = l->lv_తాళం;
	    ఒకవేళ (l->lv_తాళం == 0)
		l->lv_తాళం = VAR_LOCKED;
	}

	కొరకు (;;)
	{
	    ఒకవేళ (l != ఖాళీ)
	    {
		ఒకవేళ (li == ఖాళీ)
		{
		    సరిపోలిక = FALSE;
		    ఆపు;
		}
		vim_free(tofree);
		సమాసం = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		ఒకవేళ (str == ఖాళీ)
		    ఆపు;
	    }

	    సరిపోలిక = vim_regexec_nl(&regmatch, str, startcol);

	    ఒకవేళ (సరిపోలిక && --nth <= 0)
		ఆపు;
	    ఒకవేళ (l == ఖాళీ && !సరిపోలిక)
		ఆపు;

	    // Advance to just after the match.
	    ఒకవేళ (l != ఖాళీ)
	    {
		li = li->li_తదుపరి;
		++idx;
	    }
	    లేకపోతే
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		ఒకవేళ (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    సరిపోలిక = FALSE;
		    ఆపు;
		}
	    }
	}

	ఒకవేళ (సరిపోలిక)
	{
	    ఒకవేళ (type == సరిపోలిక_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_మొదటి;
		listitem_T *li2 = li1->li_తదుపరి;
		listitem_T *li3 = li2->li_తదుపరి;
		listitem_T *li4 = li3->li_తదుపరి;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_సంఖ్య =
				      (varnumber_T)(regmatch.startp[0] - సమాసం);
		li4->li_tv.vval.v_సంఖ్య =
					(varnumber_T)(regmatch.endp[0] - సమాసం);
		ఒకవేళ (l != ఖాళీ)
		    li2->li_tv.vval.v_సంఖ్య = (varnumber_T)idx;
	    }
	    లేకపోతే ఒకవేళ (type == సరిపోలిక_LIST)
	    {
		పూర్ణాంకం i;

		// return list with matched string and submatches
		కొరకు (i = 0; i < NSUBEXP; ++i)
		{
		    ఒకవేళ (regmatch.endp[i] == ఖాళీ)
		    {
			ఒకవేళ (list_append_string(rettv->vval.v_list,
							    ఖాళీ, 0) == FAIL)
			    ఆపు;
		    }
		    లేకపోతే ఒకవేళ (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(పూర్ణాంకం)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			ఆపు;
		}
	    }
	    లేకపోతే ఒకవేళ (type == సరిపోలిక_STR)
	    {
		// return matched string
		ఒకవేళ (l != ఖాళీ)
		    copy_tv(&li->li_tv, rettv);
		లేకపోతే
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    లేకపోతే ఒకవేళ (l != ఖాళీ)
		rettv->vval.v_సంఖ్య = idx;
	    లేకపోతే
	    {
		ఒకవేళ (type != సరిపోలిక_చివర_2)
		    rettv->vval.v_సంఖ్య =
				      (varnumber_T)(regmatch.startp[0] - str);
		లేకపోతే
		    rettv->vval.v_సంఖ్య =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_సంఖ్య += (varnumber_T)(str - సమాసం);
	    }
	}
	ఒకవేళ (l != ఖాళీ)
	    l->lv_తాళం = prev_తాళం;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    ఒకవేళ (type == సరిపోలిక_POS && l == ఖాళీ && rettv->vval.v_list != ఖాళీ)
	// matchstrpos() without a list: drop the second item.
	listitem_remove(rettv->vval.v_list,
				       rettv->vval.v_list->lv_మొదటి->li_తదుపరి);
    vim_free(tofree);
    p_cpo = save_cpo;
}

/*
 * Return all the matches in string "str" for pattern "rmp".
 * The matches are returned in the List "mlist".
 * If "submatches" is TRUE, then submatch information is also returned.
 * "matchbuf" is TRUE when called for matchbufline().
 */
    స్థిర పూర్ణాంకం
get_matches_లోపల_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    పూర్ణాంకం		idx,
    పూర్ణాంకం		submatches,
    పూర్ణాంకం		matchbuf)
{
    దీర్ఘ	len = (దీర్ఘ)STRLEN(str);
    పూర్ణాంకం		సరిపోలిక = 0;
    colnr_T	startidx = 0;

    కొరకు (;;)
    {
	సరిపోలిక = vim_regexec_nl(rmp, str, startidx);
	ఒకవేళ (!సరిపోలిక)
	    ఆపు;

	dict_T *d = dict_alloc();
	ఒకవేళ (d == ఖాళీ)
	    తిరిగి FAIL;
	ఒకవేళ (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    తిరిగి FAIL;
	}

	ఒకవేళ (dict_కూడు_సంఖ్య(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (dict_కూడు_సంఖ్య(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (dict_కూడు_string_len(d, "text", rmp->startp[0],
		    (పూర్ణాంకం)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (submatches)
	{
	    list_T *sml = list_alloc();
	    ఒకవేళ (sml == ఖాళీ)
		తిరిగి FAIL;

	    ఒకవేళ (dict_కూడు_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		తిరిగి FAIL;
	    }

	    // return a list with the submatches
	    కొరకు (పూర్ణాంకం i = 1; i < NSUBEXP; ++i)
	    {
		ఒకవేళ (rmp->endp[i] == ఖాళీ)
		{
		    ఒకవేళ (list_append_string(sml, (char_u *)"", 0) == FAIL)
			తిరిగి FAIL;
		}
		లేకపోతే ఒకవేళ (list_append_string(sml, rmp->startp[i],
			    (పూర్ణాంకం)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    తిరిగి FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	ఒకవేళ (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    ఆపు;
    }

    తిరిగి OK;
}

/*
 * "matchbufline()" function
 */
    స్థిర శూన్య
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ఖాళీ;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_సంఖ్య = -1;
    ఒకవేళ (rettv_list_alloc(rettv) != OK)
	తిరిగి;
    retlist = rettv->vval.v_list;

    ఒకవేళ (check_for_బఫరు_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	తిరిగి;

    పూర్ణాంకం prev_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    ఒకవేళ (buf == ఖాళీ)
    {
	ఒకవేళ (did_emsg == prev_did_emsg)
	    semsg(_(e_invalid_బఫరు_name_str), tv_get_string(&argvars[0]));
	తిరిగి;
    }
    ఒకవేళ (buf->b_ml.ml_mfp == ఖాళీ)
    {
	emsg(_(e_బఫరు_is_not_loaded));
	తిరిగి;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    పూర్ణాంకం		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    ఒకవేళ (did_emsg > did_emsg_before)
	తిరిగి;
    ఒకవేళ (slnum < 1)
    {
	semsg(_(e_invalid_విలువ_for_వాదం_str), "lnum");
	తిరిగి;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    ఒకవేళ (did_emsg > did_emsg_before)
	తిరిగి;
    ఒకవేళ (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_విలువ_for_వాదం_str), "end_lnum");
	తిరిగి;
    }

    ఒకవేళ (elnum > buf->b_ml.ml_పంక్తి_లెక్క)
	elnum = buf->b_ml.ml_పంక్తి_లెక్క;

    పూర్ణాంకం		submatches = FALSE;
    ఒకవేళ (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	ఒకవేళ (d != ఖాళీ)
	{
	    dictitem_T *di = dict_కనుగొను(d, (char_u *)"submatches", -1);
	    ఒకవేళ (di != ఖాళీ)
	    {
		ఒకవేళ (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "submatches");
		    తిరిగి;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ఖాళీలు_option;

    regmatch.regprog = eval_regcomp(pat);
    ఒకవేళ (regmatch.regprog == ఖాళీ)
	వెళ్ళు theend;
    regmatch.rm_ic = p_ic;

    అయితే (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	ఒకవేళ (get_matches_లోపల_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    వెళ్ళు cleanup;
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
    స్థిర శూన్య
f_సరిపోలిక(typval_T *argvars, typval_T *rettv)
{
    కనుగొను_some_సరిపోలిక(argvars, rettv, సరిపోలిక_సరిపోలిక);
}

/*
 * "matchend()" function
 */
    స్థిర శూన్య
f_matchend(typval_T *argvars, typval_T *rettv)
{
    కనుగొను_some_సరిపోలిక(argvars, rettv, సరిపోలిక_చివర_2);
}

/*
 * "matchlist()" function
 */
    స్థిర శూన్య
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    కనుగొను_some_సరిపోలిక(argvars, rettv, సరిపోలిక_LIST);
}

/*
 * "matchstr()" function
 */
    స్థిర శూన్య
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    కనుగొను_some_సరిపోలిక(argvars, rettv, సరిపోలిక_STR);
}

/*
 * "matchstrlist()" function
 */
    స్థిర శూన్య
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ఖాళీ;
    char_u	*save_cpo;
    list_T	*l = ఖాళీ;
    listitem_T	*li = ఖాళీ;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_సంఖ్య = -1;
    ఒకవేళ (rettv_list_alloc(rettv) != OK)
	తిరిగి;
    retlist = rettv->vval.v_list;

    ఒకవేళ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	తిరిగి;

    ఒకవేళ ((l = argvars[0].vval.v_list) == ఖాళీ)
	తిరిగి;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    ఒకవేళ (pat == ఖాళీ)
	తిరిగి;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ఖాళీలు_option;

    regmatch.regprog = eval_regcomp(pat);
    ఒకవేళ (regmatch.regprog == ఖాళీ)
	వెళ్ళు theend;
    regmatch.rm_ic = p_ic;

    పూర్ణాంకం		submatches = FALSE;
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	ఒకవేళ (d != ఖాళీ)
	{
	    dictitem_T *di = dict_కనుగొను(d, (char_u *)"submatches", -1);
	    ఒకవేళ (di != ఖాళీ)
	    {
		ఒకవేళ (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_విలువ_for_వాదం_str), "submatches");
		    వెళ్ళు cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    పూర్ణాంకం idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	ఒకవేళ (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != ఖాళీ)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    ఒకవేళ (get_matches_లోపల_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		వెళ్ళు cleanup;
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
    స్థిర శూన్య
f_matchstrpos(typval_T *argvars, typval_T *rettv)
{
    కనుగొను_some_సరిపోలిక(argvars, rettv, సరిపోలిక_POS);
}

    స్థిర శూన్య
max_min(typval_T *argvars, typval_T *rettv, పూర్ణాంకం domax)
{
    varnumber_T	n = 0;

    ఒకవేళ (లోపల_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = 0;

    ఒకవేళ (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = ఖాళీ;

	l = argvars[0].vval.v_list;
	ఒకవేళ (l != ఖాళీ && l->lv_len > 0)
	{
	    ఒకవేళ (l->lv_మొదటి == &range_list_అంశం)
	    {
		ఒకవేళ ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_ప్రారంభం;
		లేకపోతే
		    n = l->lv_u.nonmat.lv_ప్రారంభం + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_సంఖ్య = n;
	    }
	    లేకపోతే
	    {
		FOR_ALL_LIST_వస్తువులు(l, li)
		{
		    ఒకవేళ (tv == ఖాళీ)
			tv = &li->li_tv;
		    లేకపోతే
		    {
			పూర్ణాంకం res;
			ఒకవేళ (typval_compare2(&li->li_tv, tv,
				domax ? సమాసం_GREATER : సమాసం_SMALLER, FALSE, &res) == FAIL)
			    తిరిగి;
			ఒకవేళ (res == OK)
			    tv = &li->li_tv;
		    }
		}

		ఒకవేళ (tv != ఖాళీ)
		    copy_tv(tv, rettv);
	    }
	}
    }
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	ఒకవేళ (tv != ఖాళీ)
	    copy_tv(tv, rettv);
    }
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	పూర్ణాంకం		todo;
	typval_T	*tv = ఖాళీ;

	d = argvars[0].vval.v_dict;
	ఒకవేళ (d != ఖాళీ)
	{
	    todo = (పూర్ణాంకం)d->dv_hashtab.ht_వాడిన;
	    FOR_ALL_HASHTAB_వస్తువులు(&d->dv_hashtab, hi, todo)
	    {
		ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		{
		    --todo;
		    ఒకవేళ (tv == ఖాళీ)
			tv = &HI2DI(hi)->di_tv;
		    లేకపోతే
		    {
			పూర్ణాంకం res;
			ఒకవేళ (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? సమాసం_GREATER : సమాసం_SMALLER, FALSE, &res) == FAIL)
			    తిరిగి;
			ఒకవేళ (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	ఒకవేళ (tv != ఖాళీ)
	    copy_tv(tv, rettv);
    }
    లేకపోతే
	semsg(_(e_వాదం_యొక్క_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
}

/*
 * "max()" function
 */
    స్థిర శూన్య
f_max(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, TRUE);
}

/*
 * "min()" function
 */
    స్థిర శూన్య
f_min(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, FALSE);
}

#ఒకవేళ defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    స్థిర శూన్య
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    శూన్య
mzscheme_call_vim(char_u *name, typval_T *వాదనలు, typval_T *rettv)
{
    typval_T argvars[3];

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = name;
    copy_tv(వాదనలు, &argvars[1]);
    argvars[2].v_type = VAR_UNKNOWN;
    f_call(argvars, rettv);
    clear_tv(&argvars[1]);
}
#అయితే_ముగింపు

/*
 * "nextnonblank()" function
 */
    స్థిర శూన్య
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ఒకవేళ (లోపల_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	తిరిగి;

    కొరకు (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	ఒకవేళ (lnum < 0 || lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    lnum = 0;
	    ఆపు;
	}
	ఒకవేళ (*skipwhite(ml_get(lnum)) != NUL)
	    ఆపు;
    }
    rettv->vval.v_సంఖ్య = lnum;
}


/*
 * "ngettext()" function
 */
    స్థిర శూన్య
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#ఒకవేళ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    అక్షరం *prev = ఖాళీ;
#అయితే_ముగింపు

    ఒకవేళ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_సంఖ్య_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_STRING;

    ఒకవేళ (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != ఖాళీ &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#ఒకవేళ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((స్థిరరాశి అక్షరం *)argvars[3].vval.v_string, (అక్షరం *)p_enc);
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((స్థిరరాశి అక్షరం *)argvars[3].vval.v_string, (స్థిరరాశి అక్షరం *)argvars[0].vval.v_string, (స్థిరరాశి అక్షరం *)argvars[1].vval.v_string, (పూర్ణాంకం)argvars[2].vval.v_సంఖ్య));
#లేకపోతే
	textdomain((స్థిరరాశి అక్షరం *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((స్థిరరాశి అక్షరం *)argvars[0].vval.v_string, (స్థిరరాశి అక్షరం *)argvars[1].vval.v_string, argvars[2].vval.v_సంఖ్య));
	textdomain(VIMPACKAGE);
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	ఒకవేళ (prev != ఖాళీ)
	    bind_textdomain_codeset((స్థిరరాశి అక్షరం *)argvars[3].vval.v_string, prev);
#అయితే_ముగింపు
    }
    లేకపోతే
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((స్థిరరాశి అక్షరం *)argvars[0].vval.v_string, (స్థిరరాశి అక్షరం *)argvars[1].vval.v_string, argvars[2].vval.v_సంఖ్య));
}


/*
 * "nr2char()" function
 */
    స్థిర శూన్య
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (has_mbyte)
    {
	పూర్ణాంకం	utf8 = 0;

	ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (పూర్ణాంకం)tv_get_bool_chk(&argvars[1], ఖాళీ);
	ఒకవేళ (utf8)
	    buf[utf_char2bytes((పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]), buf)] = NUL;
	లేకపోతే
	    buf[(*mb_char2bytes)((పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]), buf)] = NUL;
    }
    లేకపోతే
    {
	buf[0] = (char_u)tv_get_సంఖ్య(&argvars[0]);
	buf[1] = NUL;
    }
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "or(expr, expr)" function
 */
    స్థిర శూన్య
f_or(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    rettv->vval.v_సంఖ్య = tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ)
					| tv_get_సంఖ్య_chk(&argvars[1], ఖాళీ);
}

#నిర్వచితమైతే FEAT_PERL
/*
 * "perleval()" function
 */
    స్థిర శూన్య
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#అయితే_ముగింపు

/*
 * "prevnonblank()" function
 */
    స్థిర శూన్య
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ఒకవేళ (లోపల_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	తిరిగి;

    lnum = tv_get_lnum(argvars);
    ఒకవేళ (lnum < 1 || lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	lnum = 0;
    లేకపోతే
	అయితే (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
	    --lnum;
    rettv->vval.v_సంఖ్య = lnum;
}

// This dummy va_list is here because:
// - passing a NULL pointer doesn't work when va_list isn't a pointer
// - locally in the function results in a "used before set" warning
// - using va_start() to initialize it gives "function with fixed args" error
స్థిర va_list	ap;

/*
 * "printf()" function
 */
    స్థిర శూన్య
f_printf(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    పూర్ణాంకం		len;
    char_u	*s;
    పూర్ణాంకం		saved_did_emsg = did_emsg;
    అక్షరం	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script() && check_for_string_or_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (అక్షరం *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(ఖాళీ, 0, fmt, ap, argvars + 1);
    ఒకవేళ (!did_emsg)
    {
	s = alloc(len + 1);
	ఒకవేళ (s != ఖాళీ)
	{
	    rettv->vval.v_string = s;
	    (శూన్య)vim_vsnprintf_typval((అక్షరం *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    స్థిర శూన్య
f_pum_getpos(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;
    pum_సమితి_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    స్థిర శూన్య
f_pumvisible(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    ఒకవేళ (pum_visible())
	rettv->vval.v_సంఖ్య = 1;
}

#నిర్వచితమైతే FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    స్థిర శూన్య
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (p_pyx == 0)
	p_pyx = 3;

    ఒకవేళ (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    లేకపోతే ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	తిరిగి;
    }
    లేకపోతే
    {
	locals = ఖాళీ;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PYTHON
/*
 * "pyeval()" function
 */
    స్థిర శూన్య
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	తిరిగి;

    ఒకవేళ (p_pyx == 0)
	p_pyx = 2;

    ఒకవేళ (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    లేకపోతే ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	తిరిగి;
    }
    లేకపోతే
    {
	locals = ఖాళీ;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    స్థిర శూన్య
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

# ఒకవేళ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ఒకవేళ (p_pyx == 2)
	f_pyeval(argvars, rettv);
    లేకపోతే
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#అయితే_ముగింపు

స్థిర UINT32_T srand_విత్తనం_for_testing = 0;
స్థిర పూర్ణాంకం	srand_విత్తనం_for_testing_is_వాడిన = FALSE;

    స్థిర శూన్య
f_test_srand_విత్తనం(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
	srand_విత్తనం_for_testing_is_వాడిన = FALSE;
    లేకపోతే
    {
	srand_విత్తనం_for_testing = (UINT32_T)tv_get_సంఖ్య(&argvars[0]);
	srand_విత్తనం_for_testing_is_వాడిన = TRUE;
    }
}

    స్థిర శూన్య
init_srand(UINT32_T *x)
{
    నిర్మాణం {
	సంఘం {
	    UINT32_T సంఖ్య;
	    char_u   బైట్లు[పరిమాణం(UINT32_T)];
	} contents;
    } buf;

    ఒకవేళ (srand_విత్తనం_for_testing_is_వాడిన)
    {
	*x = srand_విత్తనం_for_testing;
	తిరిగి;
    }

    ఒకవేళ (mch_get_random(buf.contents.బైట్లు, పరిమాణం(buf.contents.బైట్లు)) == OK)
    {
	*x = buf.contents.సంఖ్య;
	తిరిగి;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#ఒకవేళ defined(FEAT_SODIUM)
    ఒకవేళ (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    లేకపోతే
#అయితే_ముగింపు
    {
#ఒకవేళ defined(FEAT_RELTIME)
	proftime_T res;
	profile_ప్రారంభం(&res);
# ఒకవేళ defined(MSWIN)
	*x = (UINT32_T)res.దిగువ_Part;
# లేకపోతే
	*x = (UINT32_T)res.tv_fsec;
# endif
#లేకపోతే
	*x = vim_సమయం();
#అయితే_ముగింపు
	*x ^= mch_get_pid();
    }
}

#నిర్వచించు ROTL(x, k) (((x) << (k)) | ((x) >> (32 - (k))))
#నిర్వచించు SPLITMIX32(x, z) ( \
    (z) = ((x) += 0x9e3779b9), \
    (z) = ((z) ^ ((z) >> 16)) * 0x85ebca6b, \
    (z) = ((z) ^ ((z) >> 13)) * 0xc2b2ae35, \
    (z) ^ ((z) >> 16) \
    )
#నిర్వచించు SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w) \
    ఫలితం = ROTL((y) * 5, 7) * 9; \
    t = (y) << 9; \
    (z) ^= (x); \
    (w) ^= (y); \
    (y) ^= (z), (x) ^= (w); \
    (z) ^= t; \
    (w) = ROTL(w, 11);

/*
 * "rand()" function
 */
    స్థిర శూన్య
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = ఖాళీ;
    స్థిర UINT32_T	gx, gy, gz, gw;
    స్థిర పూర్ణాంకం	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, ఫలితం;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// When no argument is given use the global seed list.
	ఒకవేళ (initialized == FALSE)
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
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	ఒకవేళ (l == ఖాళీ || list_len(l) != 4)
	    వెళ్ళు theend;

	lx = list_కనుగొను(l, 0L);
	ly = list_కనుగొను(l, 1L);
	lz = list_కనుగొను(l, 2L);
	lw = list_కనుగొను(l, 3L);
	ఒకవేళ (lx == ఖాళీ || ly == ఖాళీ || lz == ఖాళీ || lw == ఖాళీ)
	    వెళ్ళు theend;
	ఒకవేళ (lx->li_tv.v_type != VAR_సంఖ్య
		|| ly->li_tv.v_type != VAR_సంఖ్య
		|| lz->li_tv.v_type != VAR_సంఖ్య
		|| lw->li_tv.v_type != VAR_సంఖ్య)
	    వెళ్ళు theend;
	x = (UINT32_T)lx->li_tv.vval.v_సంఖ్య;
	y = (UINT32_T)ly->li_tv.vval.v_సంఖ్య;
	z = (UINT32_T)lz->li_tv.vval.v_సంఖ్య;
	w = (UINT32_T)lw->li_tv.vval.v_సంఖ్య;

	SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w);

	lx->li_tv.vval.v_సంఖ్య = (varnumber_T)x;
	ly->li_tv.vval.v_సంఖ్య = (varnumber_T)y;
	lz->li_tv.vval.v_సంఖ్య = (varnumber_T)z;
	lw->li_tv.vval.v_సంఖ్య = (varnumber_T)w;
    }
    లేకపోతే
	వెళ్ళు theend;

    rettv->v_type = VAR_సంఖ్య;
    rettv->vval.v_సంఖ్య = (varnumber_T)ఫలితం;
    తిరిగి;

theend:
    semsg(_(e_invalid_వాదం_str), tv_get_string(&argvars[0]));
    rettv->v_type = VAR_సంఖ్య;
    rettv->vval.v_సంఖ్య = -1;
}

/*
 * "srand()" function
 */
    స్థిర శూన్య
f_srand(typval_T *argvars, typval_T *rettv)
{
    UINT32_T x = 0, z;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    లేకపోతే
    {
	పూర్ణాంకం	    error = FALSE;

	x = (UINT32_T)tv_get_సంఖ్య_chk(&argvars[0], &error);
	ఒకవేళ (error)
	    తిరిగి;
    }

    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#నిర్వచనం_రద్దు ROTL
#నిర్వచనం_రద్దు SPLITMIX32
#నిర్వచనం_రద్దు SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    స్థిర శూన్య
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	ప్రారంభం;
    varnumber_T	చివర;
    varnumber_T	stride = 1;
    పూర్ణాంకం		error = FALSE;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL)))
	తిరిగి;

    ప్రారంభం = tv_get_సంఖ్య_chk(&argvars[0], &error);
    ఒకవేళ (argvars[1].v_type == VAR_UNKNOWN)
    {
	చివర = ప్రారంభం - 1;
	ప్రారంభం = 0;
    }
    లేకపోతే
    {
	చివర = tv_get_సంఖ్య_chk(&argvars[1], &error);
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_సంఖ్య_chk(&argvars[2], &error);
    }

    ఒకవేళ (error)
	తిరిగి;		// type error; errmsg already given
    ఒకవేళ (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	తిరిగి;
    }

    // The stride is stored in "lv_stride", which is an int.
    ఒకవేళ (stride < INT_MIN || stride > గరిష్ఠ_పూర్ణాంకం)
    {
	అక్షరం	buf[NUMBUFLEN];

	vim_snprintf(buf, పరిమాణం(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	తిరిగి;
    }

    uvarnumber_T	len;

    ఒకవేళ (stride > 0 ? చివర < ప్రారంభం : చివర > ప్రారంభం)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	వెనుక = stride > 0
				  ? (uvarnumber_T)ప్రారంభం - (uvarnumber_T)చివర
				  : (uvarnumber_T)చివర - (uvarnumber_T)ప్రారంభం;

	ఒకవేళ (వెనుక > 1)
	{
	    emsg(_(e_ప్రారంభం_past_చివర));
	    తిరిగి;
	}
	len = 0;
    }
    లేకపోతే
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	పరిధి = stride > 0
				  ? (uvarnumber_T)చివర - (uvarnumber_T)ప్రారంభం
				  : (uvarnumber_T)ప్రారంభం - (uvarnumber_T)చివర;
	uvarnumber_T	లెక్క = పరిధి / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	ఒకవేళ (లెక్క >= (uvarnumber_T)గరిష్ఠ_పూర్ణాంకం)
	{
	    అక్షరం	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, పరిమాణం(buf), "%llu",
			      లెక్క < UVARNUM_MAX ? లెక్క + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    తిరిగి;
	}
	len = లెక్క + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_మొదటి = &range_list_అంశం;
    list->lv_u.nonmat.lv_ప్రారంభం = ప్రారంభం;
    list->lv_u.nonmat.lv_చివర = చివర;
    list->lv_u.nonmat.lv_stride = (పూర్ణాంకం)stride;
    list->lv_len = (పూర్ణాంకం)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    శూన్య
range_list_materialize(list_T *list)
{
    varnumber_T ప్రారంభం = list->lv_u.nonmat.lv_ప్రారంభం;
    varnumber_T చివర = list->lv_u.nonmat.lv_చివర;
    పూర్ణాంకం		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_మొదటి = ఖాళీ;
    list->lv_u.mat.lv_చివరి = ఖాళీ;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_అంశం = ఖాళీ;
    కొరకు (i = ప్రారంభం; stride > 0 ? i <= చివర : i >= చివర; i += stride)
    {
	ఒకవేళ (list_append_సంఖ్య(list, i) == FAIL)
	    ఆపు;
	ఒకవేళ (list->lv_తాళం & VAR_వస్తువులు_LOCKED)
	    list->lv_u.mat.lv_చివరి->li_tv.v_తాళం = VAR_LOCKED;
    }
    list->lv_తాళం &= ~VAR_వస్తువులు_LOCKED;
}

/*
 * "getreginfo()" function
 */
    స్థిర శూన్య
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		regname;
    char_u	buf[NUMBUFLEN + 2];
    size_t	buflen;
    దీర్ఘ	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    regname = getreg_get_regname(argvars);
    ఒకవేళ (regname == 0)
	తిరిగి;

    ఒకవేళ (regname == '@')
	regname = '"';

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_సమాసం_SRC | GREG_LIST);
    ఒకవేళ (list == ఖాళీ)
	తిరిగి;
    ఒకవేళ (dict_కూడు_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    ఎంపిక (get_reg_type(regname, &reglen))
    {
	స్థితి MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    ఆపు;
	స్థితి MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    ఆపు;
	స్థితి MBLOCK:
	    buflen = vim_snprintf_safelen((అక్షరం *)buf, పరిమాణం(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    ఆపు;
	అప్రమేయ:
	    buf[0] = NUL;
	    buflen = 0;
	    ఆపు;
    }
    (శూన్య)dict_కూడు_string_len(dict, (అక్షరం *)"regtype", buf, (పూర్ణాంకం)buflen);

    buf[0] = get_రిజిస్టరు_name(get_unname_రిజిస్టరు());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    ఒకవేళ (regname == '"')
	(శూన్య)dict_కూడు_string_len(dict, (అక్షరం *)"points_to", buf, (పూర్ణాంకం)buflen);
    లేకపోతే
    {
	dictitem_T	*అంశం = dictitem_alloc((char_u *)"isunnamed");

	ఒకవేళ (అంశం != ఖాళీ)
	{
	    అంశం->di_tv.v_type = VAR_BOOL;
	    అంశం->di_tv.vval.v_సంఖ్య = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (శూన్య)dict_కూడు(dict, అంశం);
	}
    }
}

    స్థిర శూన్య
return_రిజిస్టరు(పూర్ణాంకం regname, typval_T *rettv)
{
    char_u buf[2] = {NUL, NUL};

    buf[0] = (char_u)regname;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(buf, (buf[0] == NUL) ? 0 : 1);
}

/*
 * "reg_executing()" function
 */
    స్థిర శూన్య
f_reg_executing(typval_T *argvars వాడని, typval_T *rettv)
{
    return_రిజిస్టరు(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    స్థిర శూన్య
f_reg_recording(typval_T *argvars వాడని, typval_T *rettv)
{
    return_రిజిస్టరు(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    స్థిర శూన్య
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_సంఖ్య = -1;
    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    rettv->vval.v_సంఖ్య = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    స్థిర శూన్య
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    పూర్ణాంకం slen = list_len(l);

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL
	    || l == ఖాళీ
	    || n <= 0
	    || slen == 0)
	తిరిగి;

    ఒకవేళ (check_repeat_లెక్క(slen, n) == FAIL)
	తిరిగి;

    అయితే (n-- > 0)
	ఒకవేళ (list_extend(rettv->vval.v_list, l, ఖాళీ) == FAIL)
	    ఆపు;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    స్థిర శూన్య
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    పూర్ణాంకం		slen;
    పూర్ణాంకం		len;
    పూర్ణాంకం		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    ఒకవేళ (rettv_blob_alloc(rettv) == FAIL
	    || blob == ఖాళీ
	    || n <= 0)
	తిరిగి;

    slen = blob->bv_ga.ga_len;
    ఒకవేళ (slen <= 0)
	తిరిగి;
    ఒకవేళ (check_repeat_లెక్క(slen, n) == FAIL)
	తిరిగి;
    len = slen * (పూర్ణాంకం)n;

    ఒకవేళ (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	తిరిగి;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    కొరకు (i = 0; i < slen; ++i)
	ఒకవేళ (blob_get(blob, i) != 0)
	    ఆపు;

    ఒకవేళ (i == slen)
	// No need to copy since all bytes are already zero
	తిరిగి;

    కొరకు (i = 0; i < (పూర్ణాంకం)n; ++i)
	blob_సమితి_range(rettv->vval.v_blob,
		(దీర్ఘ)i * slen, ((దీర్ఘ)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    స్థిర శూన్య
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    size_t	slen;
    పూర్ణాంకం		len;
    char_u	*r;
    పూర్ణాంకం		పూర్తి;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    slen = STRLEN(p);
    ఒకవేళ (slen == 0 || n <= 0)
	తిరిగి;
    ఒకవేళ (check_repeat_లెక్క((varnumber_T)slen, n) == FAIL)
	తిరిగి;
    len = (పూర్ణాంకం)slen * (పూర్ణాంకం)n;

    r = alloc((size_t)len + 1);
    ఒకవేళ (r == ఖాళీ)
	తిరిగి;

    mch_memmove(r, p, slen);
    పూర్తి = (పూర్ణాంకం)slen;
    అయితే (పూర్తి < len)
    {
	పూర్ణాంకం copy_len = పూర్తి;

	ఒకవేళ (copy_len > len - పూర్తి)
	    copy_len = len - పూర్తి;
	mch_memmove(r + పూర్తి, r, (size_t)copy_len);
	పూర్తి += copy_len;
    }
    r[len] = NUL;

    rettv->vval.v_string = r;
}

/*
 * "repeat()" function
 */
    స్థిర శూన్య
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    n = tv_get_సంఖ్య(&argvars[1]);
    ఒకవేళ (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    లేకపోతే
	repeat_string(&argvars[0], n, rettv);
}

#నిర్వచించు SP_NOMOVE	0x01	    // don't move cursor
#నిర్వచించు SP_REPEAT	0x02	    // repeat to find outer pair
#నిర్వచించు SP_RETCOUNT	0x04	    // return matchcount
#నిర్వచించు SP_SETPCMARK	0x08	    // set previous context mark
#నిర్వచించు SP_ప్రారంభం	0x10	    // accept match at start position
#నిర్వచించు SP_SUBPAT	0x20	    // return nr of matching sub-pattern
#నిర్వచించు SP_చివర		0x40	    // leave cursor at end of match
#నిర్వచించు SP_స్తంభం	0x80	    // start at cursor column

/*
 * Get flags for a search function.
 * Possibly sets "p_ws".
 * Returns BACKWARD, FORWARD or zero (for an error).
 */
    స్థిర పూర్ణాంకం
get_శోధన_arg(typval_T *varp, పూర్ణాంకం *flagsp)
{
    పూర్ణాంకం		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    పూర్ణాంకం		ముసుగు;

    ఒకవేళ (varp->v_type == VAR_UNKNOWN)
	తిరిగి FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    ఒకవేళ (flags == ఖాళీ)
	తిరిగి 0;		// type error; errmsg already given
    అయితే (*flags != NUL)
    {
	ఎంపిక (*flags)
	{
	    స్థితి 'b': dir = BACKWARD; ఆపు;
	    స్థితి 'w': p_ws = TRUE; ఆపు;
	    స్థితి 'W': p_ws = FALSE; ఆపు;
	    అప్రమేయ:  ముసుగు = 0;
		      ఒకవేళ (flagsp != ఖాళీ)
			  ఎంపిక (*flags)
			  {
			      స్థితి 'c': ముసుగు = SP_ప్రారంభం; ఆపు;
			      స్థితి 'e': ముసుగు = SP_చివర; ఆపు;
			      స్థితి 'm': ముసుగు = SP_RETCOUNT; ఆపు;
			      స్థితి 'n': ముసుగు = SP_NOMOVE; ఆపు;
			      స్థితి 'p': ముసుగు = SP_SUBPAT; ఆపు;
			      స్థితి 'r': ముసుగు = SP_REPEAT; ఆపు;
			      స్థితి 's': ముసుగు = SP_SETPCMARK; ఆపు;
			      స్థితి 'z': ముసుగు = SP_స్తంభం; ఆపు;
			  }
		      ఒకవేళ (ముసుగు == 0)
		      {
			  semsg(_(e_invalid_వాదం_str), flags);
			  dir = 0;
		      }
		      లేకపోతే
			  *flagsp |= ముసుగు;
	}
	ఒకవేళ (dir == 0)
	    ఆపు;
	++flags;
    }
    తిరిగి dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    స్థిర పూర్ణాంకం
శోధన_cmn(typval_T *argvars, pos_T *సరిపోలిక_pos, పూర్ణాంకం *flagsp)
{
    పూర్ణాంకం		flags;
    char_u	*pat;
    size_t	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    పూర్ణాంకం		save_p_ws = p_ws;
    పూర్ణాంకం		dir;
    పూర్ణాంకం		retval = 0;	// default: FAIL
    దీర్ఘ	lnum_stop = 0;
#నిర్వచితమైతే FEAT_RELTIME
    దీర్ఘ	సమయం_హద్దు = 0;
#అయితే_ముగింపు
    పూర్ణాంకం		options = శోధన_KEEP;
    పూర్ణాంకం		subpatnum;
    searchit_arg_T sia;
    పూర్ణాంకం		use_skip = FALSE;
    pos_T	firstpos;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_సంఖ్య_arg(argvars, 3) == FAIL)))))
	వెళ్ళు theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_శోధన_arg(&argvars[1], flagsp);	// may set p_ws
    ఒకవేళ (dir == 0)
	వెళ్ళు theend;
    flags = *flagsp;
    ఒకవేళ (flags & SP_ప్రారంభం)
	options |= శోధన_ప్రారంభం;
    ఒకవేళ (flags & SP_చివర)
	options |= శోధన_చివర;
    ఒకవేళ (flags & SP_స్తంభం)
	options |= శోధన_నిలువు;

    // Optional arguments: line number to stop searching, timeout and skip.
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[2], ఖాళీ);
	ఒకవేళ (lnum_stop < 0)
	    వెళ్ళు theend;
	ఒకవేళ (argvars[3].v_type != VAR_UNKNOWN)
	{
#నిర్వచితమైతే FEAT_RELTIME
	    సమయం_హద్దు = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[3], ఖాళీ);
	    ఒకవేళ (సమయం_హద్దు < 0)
		వెళ్ళు theend;
#అయితే_ముగింపు
	    use_skip = eval_సమాసం_valid_arg(&argvars[4]);
	}
    }

    /*
     * This function does not accept SP_REPEAT and SP_RETCOUNT flags.
     * Check to make sure only those flags are set.
     * Also, Only the SP_NOMOVE or the SP_SETPCMARK flag can be set. Both
     * flags cannot be set. Check for that condition also.
     */
    ఒకవేళ (((flags & (SP_REPEAT | SP_RETCOUNT)) != 0)
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_వాదం_str), tv_get_string(&argvars[1]));
	వెళ్ళు theend;
    }

    pos = save_cursor = curwin->w_cursor;
    CLEAR_క్షేత్రం(firstpos);
    CLEAR_క్షేత్రం(sia);
    sia.sa_stop_lnum = (linenr_T)lnum_stop;
#నిర్వచితమైతే FEAT_RELTIME
    sia.sa_tm = సమయం_హద్దు;
#అయితే_ముగింపు

    patlen = STRLEN(pat);

    // Repeat until {skip} returns FALSE.
    కొరకు (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, ఖాళీ, dir, pat, patlen, 1L,
						     options, RE_శోధన, &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	ఒకవేళ (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	ఒకవేళ (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    ఆపు;
	ఒకవేళ (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    పూర్ణాంకం	    do_skip;
	    పూర్ణాంకం	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_సమాసం_గమ్యం_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    ఒకవేళ (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		ఆపు;
	    }
	    ఒకవేళ (!do_skip)
		ఆపు;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~శోధన_ప్రారంభం;
    }

    ఒకవేళ (subpatnum != FAIL)
    {
	ఒకవేళ (flags & SP_SUBPAT)
	    retval = subpatnum;
	లేకపోతే
	    retval = pos.lnum;
	ఒకవేళ (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	ఒకవేళ (సరిపోలిక_pos != ఖాళీ)
	{
	    // Store the match cursor position
	    సరిపోలిక_pos->lnum = pos.lnum;
	    సరిపోలిక_pos->నిలువు = pos.నిలువు + 1;
	}
	// "/$" will put the cursor after the end of the line, may need to
	// correct that here
	check_cursor();
    }

    // If 'n' flag is used: restore cursor position.
    ఒకవేళ (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    లేకపోతే
	curwin->w_సమితి_curswant = true;
theend:
    p_ws = save_p_ws;

    తిరిగి retval;
}

#నిర్వచితమైతే FEAT_RUBY
/*
 * "rubyeval()" function
 */
    స్థిర శూన్య
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#అయితే_ముగింపు

/*
 * "screenattr()" function
 */
    స్థిర శూన్య
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		అడ్డువరుస;
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		c;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    అడ్డువరుస = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ) - 1;
    నిలువు = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[1], ఖాళీ) - 1;
    ఒకవేళ (అడ్డువరుస < 0 || అడ్డువరుస >= screen_అడ్డువరుసలు
	    || నిలువు < 0 || నిలువు >= screen_Columns)
	c = -1;
    లేకపోతే
	c = ScreenAttrs[పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + నిలువు];
    rettv->vval.v_సంఖ్య = c;
}

/*
 * "screenchar()" function
 */
    స్థిర శూన్య
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		అడ్డువరుస;
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		c;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    అడ్డువరుస = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ) - 1;
    నిలువు = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[1], ఖాళీ) - 1;
    ఒకవేళ (అడ్డువరుస < 0 || అడ్డువరుస >= screen_అడ్డువరుసలు || నిలువు < 0 || నిలువు >= screen_Columns)
	c = -1;
    లేకపోతే
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(అడ్డువరుస, నిలువు, buf, ఖాళీ);
	c = (*mb_ptr2char)(buf);
    }
    rettv->vval.v_సంఖ్య = c;
}

/*
 * "screenchars()" function
 */
    స్థిర శూన్య
f_screenchars(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		అడ్డువరుస;
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		c;
    పూర్ణాంకం		i;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    అడ్డువరుస = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ) - 1;
    నిలువు = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[1], ఖాళీ) - 1;
    ఒకవేళ (అడ్డువరుస < 0 || అడ్డువరుస >= screen_అడ్డువరుసలు || నిలువు < 0 || నిలువు >= screen_Columns)
	తిరిగి;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(అడ్డువరుస, నిలువు, buf, ఖాళీ);
    పూర్ణాంకం pcc[MAX_MCO];
    ఒకవేళ (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    లేకపోతే
	c = (*mb_ptr2char)(buf);
    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)c);

    ఒకవేళ (enc_utf8)
	కొరకు (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    స్థిర శూన్య
f_screencol(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    స్థిర శూన్య
f_screenrow(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    స్థిర శూన్య
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		అడ్డువరుస;
    పూర్ణాంకం		నిలువు;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = ఖాళీ;
    rettv->v_type = VAR_STRING;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    అడ్డువరుస = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ) - 1;
    నిలువు = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[1], ఖాళీ) - 1;
    ఒకవేళ (అడ్డువరుస < 0 || అడ్డువరుస >= screen_అడ్డువరుసలు || నిలువు < 0 || నిలువు >= screen_Columns)
	తిరిగి;

    screen_getbytes(అడ్డువరుస, నిలువు, buf, ఖాళీ);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    స్థిర శూన్య
f_శోధన(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		flags = 0;

    rettv->vval.v_సంఖ్య = శోధన_cmn(argvars, ఖాళీ, &flags);
}

/*
 * "searchdecl()" function
 */
    స్థిర శూన్య
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		locally = TRUE;
    పూర్ణాంకం		thisblock = FALSE;
    పూర్ణాంకం		error = FALSE;
    char_u	*name;

    rettv->vval.v_సంఖ్య = 1;	// default: FAIL

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	తిరిగి;

    name = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(పూర్ణాంకం)tv_get_bool_chk(&argvars[1], &error);
	ఒకవేళ (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (పూర్ణాంకం)tv_get_bool_chk(&argvars[2], &error);
    }
    ఒకవేళ (!error && name != ఖాళీ)
	rettv->vval.v_సంఖ్య = కనుగొను_decl(name, (పూర్ణాంకం)STRLEN(name),
				     locally, thisblock, శోధన_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    స్థిర పూర్ణాంకం
searchpair_cmn(typval_T *argvars, pos_T *సరిపోలిక_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    పూర్ణాంకం		save_p_ws = p_ws;
    పూర్ణాంకం		dir;
    పూర్ణాంకం		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    పూర్ణాంకం		retval = 0;		// default: FAIL
    దీర్ఘ	lnum_stop = 0;
    దీర్ఘ	సమయం_హద్దు = 0;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_సంఖ్య_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_సంఖ్య_arg(argvars, 6) == FAIL)))))
	వెళ్ళు theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    ఒకవేళ (spat == ఖాళీ || mpat == ఖాళీ || epat == ఖాళీ)
	వెళ్ళు theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_శోధన_arg(&argvars[3], &flags); // may set p_ws
    ఒకవేళ (dir == 0)
	వెళ్ళు theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    ఒకవేళ ((flags & (SP_చివర | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_వాదం_str), tv_get_string(&argvars[3]));
	వెళ్ళు theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    ఒకవేళ (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    ఒకవేళ (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = ఖాళీ;
    లేకపోతే
    {
	// Type is checked later.
	skip = &argvars[4];

	ఒకవేళ (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[5], ఖాళీ);
	    ఒకవేళ (lnum_stop < 0)
	    {
		semsg(_(e_invalid_వాదం_str), tv_get_string(&argvars[5]));
		వెళ్ళు theend;
	    }
#నిర్వచితమైతే FEAT_RELTIME
	    ఒకవేళ (argvars[6].v_type != VAR_UNKNOWN)
	    {
		సమయం_హద్దు = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[6], ఖాళీ);
		ఒకవేళ (సమయం_హద్దు < 0)
		{
		    semsg(_(e_invalid_వాదం_str), tv_get_string(&argvars[6]));
		    వెళ్ళు theend;
		}
	    }
#అయితే_ముగింపు
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    సరిపోలిక_pos, lnum_stop, సమయం_హద్దు);

theend:
    p_ws = save_p_ws;

    తిరిగి retval;
}

/*
 * "searchpair()" function
 */
    స్థిర శూన్య
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = searchpair_cmn(argvars, ఖాళీ);
}

/*
 * "searchpairpos()" function
 */
    స్థిర శూన్య
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	సరిపోలిక_pos;
    పూర్ణాంకం		lnum = 0;
    పూర్ణాంకం		నిలువు = 0;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (searchpair_cmn(argvars, &సరిపోలిక_pos) > 0)
    {
	lnum = సరిపోలిక_pos.lnum;
	నిలువు = సరిపోలిక_pos.నిలువు;
    }

    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)నిలువు);
}

/*
 * Search for a start/middle/end thing.
 * Used by searchpair(), see its documentation for the details.
 * Returns 0 or -1 for no match,
 */
    దీర్ఘ
do_searchpair(
    char_u	*spat,	    // start pattern
    char_u	*mpat,	    // middle pattern
    char_u	*epat,	    // end pattern
    పూర్ణాంకం		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    పూర్ణాంకం		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*సరిపోలిక_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    దీర్ఘ	సమయం_హద్దు వాడని) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = ఖాళీ, *pat3 = ఖాళీ;
    size_t	patlen;
    size_t	spatlen;
    size_t	epatlen;
    size_t	pat2size;
    size_t	pat2len;
    size_t	pat3size;
    size_t	pat3len;
    దీర్ఘ	retval = 0;
    pos_T	pos;
    pos_T	firstpos;
    pos_T	foundpos;
    pos_T	save_cursor;
    pos_T	save_pos;
    పూర్ణాంకం		n;
    పూర్ణాంకం		r;
    పూర్ణాంకం		nest = 1;
    పూర్ణాంకం		use_skip = FALSE;
    పూర్ణాంకం		err;
    పూర్ణాంకం		options = శోధన_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ఖాళీలు_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    ఒకవేళ (pat2 == ఖాళీ)
	వెళ్ళు theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    ఒకవేళ (pat3 == ఖాళీ)
	వెళ్ళు theend;
    pat2len = vim_snprintf((అక్షరం *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    ఒకవేళ (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    లేకపోతే
	pat3len = vim_snprintf((అక్షరం *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    ఒకవేళ (flags & SP_ప్రారంభం)
	options |= శోధన_ప్రారంభం;

    ఒకవేళ (skip != ఖాళీ)
	use_skip = eval_సమాసం_valid_arg(skip);

#నిర్వచితమైతే FEAT_RELTIME
    ఒకవేళ (సమయం_హద్దు > 0)
	init_regexp_timeout(సమయం_హద్దు);
#అయితే_ముగింపు
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    కొరకు (;;)
    {
	searchit_arg_T sia;

	CLEAR_క్షేత్రం(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, ఖాళీ, dir, pat, patlen, 1L,
						     options, RE_శోధన, &sia);
	ఒకవేళ (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    ఆపు;

	ఒకవేళ (firstpos.lnum == 0)
	    firstpos = pos;
	ఒకవేళ (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    ఒకవేళ (dir == BACKWARD)
		decl(&pos);
	    లేకపోతే
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~శోధన_ప్రారంభం;

	// If the skip pattern matches, ignore this match.
	ఒకవేళ (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_సమాసం_గమ్యం_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    ఒకవేళ (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		ఆపు;
	    }
	    ఒకవేళ (r)
		కొనసాగు;
	}

	ఒకవేళ ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	లేకపోతే
	{
	    // Found end when searching forward or start when searching
	    // backward: end of (nested) pair; or found middle in outer pair.
	    ఒకవేళ (--nest == 1)
		pat = pat3;	// outer level, search for middle
	}

	ఒకవేళ (nest == 0)
	{
	    // Found the match: return matchcount or line number.
	    ఒకవేళ (flags & SP_RETCOUNT)
		++retval;
	    లేకపోతే
		retval = pos.lnum;
	    ఒకవేళ (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    ఒకవేళ (!(flags & SP_REPEAT))
		ఆపు;
	    nest = 1;	    // search for next unmatched
	}
    }

    ఒకవేళ (సరిపోలిక_pos != ఖాళీ)
    {
	// Store the match cursor position
	సరిపోలిక_pos->lnum = curwin->w_cursor.lnum;
	సరిపోలిక_pos->నిలువు = curwin->w_cursor.నిలువు + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    ఒకవేళ ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#నిర్వచితమైతే FEAT_RELTIME
    ఒకవేళ (సమయం_హద్దు > 0)
	disable_regexp_timeout();
#అయితే_ముగింపు
    vim_free(pat2);
    vim_free(pat3);
    ఒకవేళ (p_cpo == ఖాళీలు_option)
	p_cpo = save_cpo;
    లేకపోతే
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	ఒకవేళ (*p_cpo == NUL)
	    సమితి_option_విలువ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    తిరిగి retval;
}

/*
 * "searchpos()" function
 */
    స్థిర శూన్య
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	సరిపోలిక_pos;
    పూర్ణాంకం		lnum = 0;
    పూర్ణాంకం		నిలువు = 0;
    పూర్ణాంకం		n;
    పూర్ణాంకం		flags = 0;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    n = శోధన_cmn(argvars, &సరిపోలిక_pos, &flags);
    ఒకవేళ (n > 0)
    {
	lnum = సరిపోలిక_pos.lnum;
	నిలువు = సరిపోలిక_pos.నిలువు;
    }

    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)నిలువు);
    ఒకవేళ (flags & SP_SUBPAT)
	list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    స్థిర శూన్య
సమితి_position(typval_T *argvars, typval_T *rettv, పూర్ణాంకం charpos)
{
    pos_T	pos;
    పూర్ణాంకం		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	తిరిగి;

    name = tv_get_string_chk(argvars);
    ఒకవేళ (name == ఖాళీ)
	తిరిగి;

    ఒకవేళ (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	తిరిగి;

    ఒకవేళ (pos.నిలువు != MAXCOL && --pos.నిలువు < 0)
	pos.నిలువు = 0;
    ఒకవేళ ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	ఒకవేళ (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_సమితి_curswant = false;
	}
	check_cursor();
	rettv->vval.v_సంఖ్య = 0;
    }
    లేకపోతే ఒకవేళ (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	ఒకవేళ (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_సంఖ్య = 0;
    }
    లేకపోతే
	emsg(_(e_invalid_వాదం));
}
/*
 * "setcharpos()" function
 */
    స్థిర శూన్య
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    సమితి_position(argvars, rettv, TRUE);
}

    స్థిర శూన్య
f_setcharsearch(typval_T *argvars, typval_T *rettv వాడని)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    ఒకవేళ (check_for_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ ((d = argvars[0].vval.v_dict) == ఖాళీ)
	తిరిగి;

    csearch = dict_get_string(d, "char", FALSE);
    ఒకవేళ (csearch != ఖాళీ)
    {
	ఒకవేళ (enc_utf8)
	{
	    పూర్ణాంకం pcc[MAX_MCO];
	    పూర్ణాంకం c = utfc_ptr2char(csearch, pcc);

	    సమితి_చివరి_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	లేకపోతే
	    సమితి_చివరి_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_కనుగొను(d, (char_u *)"forward", -1);
    ఒకవేళ (di != ఖాళీ)
	సమితి_csearch_direction((పూర్ణాంకం)tv_get_సంఖ్య(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_కనుగొను(d, (char_u *)"until", -1);
    ఒకవేళ (di != ఖాళీ)
	సమితి_csearch_until(!!tv_get_సంఖ్య(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    స్థిర శూన్య
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    సమితి_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    స్థిర శూన్య
f_setenv(typval_T *argvars, typval_T *rettv వాడని)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    name = tv_get_string_buf(&argvars[0], namebuf);
    ఒకవేళ (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_సంఖ్య == VVAL_NULL)
	vim_unsetenv_ext(name);
    లేకపోతే
	vim_setenv_ext(name, tv_get_string_buf(&argvars[1], valbuf));
}

/*
 * "setfperm({fname}, {mode})" function
 */
    స్థిర శూన్య
f_setfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	modebuf[NUMBUFLEN];
    char_u	*బహుళకం_str;
    పూర్ణాంకం		i;
    పూర్ణాంకం		ముసుగు;
    పూర్ణాంకం		బహుళకం = 0;

    rettv->vval.v_సంఖ్య = 0;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    fname = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి;
    బహుళకం_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    ఒకవేళ (బహుళకం_str == ఖాళీ)
	తిరిగి;
    ఒకవేళ (STRLEN(బహుళకం_str) != 9)
    {
	semsg(_(e_invalid_వాదం_str), బహుళకం_str);
	తిరిగి;
    }

    ముసుగు = 1;
    కొరకు (i = 8; i >= 0; --i)
    {
	ఒకవేళ (బహుళకం_str[i] != '-')
	    బహుళకం |= ముసుగు;
	ముసుగు = ముసుగు << 1;
    }
    rettv->vval.v_సంఖ్య = mch_setperm(fname, బహుళకం) == OK;
}

/*
 * "setpos()" function
 */
    స్థిర శూన్య
f_setpos(typval_T *argvars, typval_T *rettv)
{
    సమితి_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    పూర్ణాంకం
get_yank_type(char_u **pp, char_u *yank_type, దీర్ఘ *ఖండం_len)
{
    char_u *stropt = *pp;
    ఎంపిక (*stropt)
    {
	స్థితి 'v': స్థితి 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    ఆపు;
	స్థితి 'V': స్థితి 'l':	// line-wise selection
	    *yank_type = MLINE;
	    ఆపు;
	స్థితి 'b': స్థితి Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    ఒకవేళ (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*ఖండం_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    ఆపు;
	అప్రమేయ:
	    తిరిగి FAIL;
    }
    *pp = stropt;
    తిరిగి OK;
}

/*
 * "setreg()" function
 */
    స్థిర శూన్య
f_setreg(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    పూర్ణాంకం		append;
    char_u	yank_type;
    దీర్ఘ	ఖండం_len;
    typval_T	*regcontents;
    పూర్ణాంకం		pointreg;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	తిరిగి;

    pointreg = 0;
    regcontents = ఖాళీ;
    ఖండం_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_సంఖ్య = 1;		// FAIL is default

    ఒకవేళ (strregname == ఖాళీ)
	తిరిగి;		// type error; errmsg already given
    ఒకవేళ (లోపల_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_రిజిస్టరు_name_must_be_one_char_str), strregname);
	తిరిగి;
    }
    regname = *strregname;
    ఒకవేళ (regname == 0 || regname == '@')
	regname = '"';

    ఒకవేళ (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	ఒకవేళ (d == ఖాళీ || d->dv_hashtab.ht_వాడిన == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {ఖాళీ, ఖాళీ};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    తిరిగి;
	}

	di = dict_కనుగొను(d, (char_u *)"regcontents", -1);
	ఒకవేళ (di != ఖాళీ)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	ఒకవేళ (stropt != ఖాళీ)
	{
	    పూర్ణాంకం ret = get_yank_type(&stropt, &yank_type, &ఖండం_len);

	    ఒకవేళ (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_విలువ_for_వాదం_str), "value");
		తిరిగి;
	    }
	}

	ఒకవేళ (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    ఒకవేళ (stropt != ఖాళీ)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	లేకపోతే ఒకవేళ (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    లేకపోతే
	regcontents = &argvars[1];

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ఒకవేళ (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    తిరిగి;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	ఒకవేళ (stropt == ఖాళీ)
	    తిరిగి;		// type error
	కొరకు (; *stropt != NUL; ++stropt)
	    ఎంపిక (*stropt)
	    {
		స్థితి 'a': స్థితి 'A':	// append
		    append = TRUE;
		    ఆపు;
		అప్రమేయ:
		    get_yank_type(&stropt, &yank_type, &ఖండం_len);
	    }
    }

    ఒకవేళ (regcontents && regcontents->v_type == VAR_LIST)
    {
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	list_T		*ll = regcontents->vval.v_list;
	listitem_T	*li;
	పూర్ణాంకం		len;

	// If the list is NULL handle like an empty list.
	len = ll == ఖాళీ ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	ఒకవేళ (lstval == ఖాళీ)
	    తిరిగి;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	ఒకవేళ (ll != ఖాళీ)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_వస్తువులు(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		ఒకవేళ (strval == ఖాళీ)
		    వెళ్ళు free_lstval;
		ఒకవేళ (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    ఒకవేళ (strval == ఖాళీ)
			వెళ్ళు free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ఖాళీ;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, ఖండం_len);
free_lstval:
	అయితే (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    లేకపోతే ఒకవేళ (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	ఒకవేళ (strval == ఖాళీ)
	    తిరిగి;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, ఖండం_len);
    }
    ఒకవేళ (pointreg != 0)
	get_yank_రిజిస్టరు(pointreg, TRUE);

    rettv->vval.v_సంఖ్య = 0;
}

/*
 * "settagstack()" function
 */
    స్థిర శూన్య
f_settagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    dict_T	*d;
    పూర్ణాంకం		action = 'r';

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	తిరిగి;

    // first argument: window number or id
    wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;

    // second argument: dict with items to set in the tag stack
    ఒకవేళ (check_for_dict_arg(argvars, 1) == FAIL)
	తిరిగి;
    d = argvars[1].vval.v_dict;
    ఒకవేళ (d == ఖాళీ)
	తిరిగి;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    ఒకవేళ (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    లేకపోతే ఒకవేళ (check_for_string_arg(argvars, 2) == FAIL)
	తిరిగి;
    లేకపోతే
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	ఒకవేళ (actstr == ఖాళీ)
	    తిరిగి;
	ఒకవేళ ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	లేకపోతే
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    తిరిగి;
	}
    }

    ఒకవేళ (సమితి_tagstack(wp, d, action) == OK)
	rettv->vval.v_సంఖ్య = 0;
}

#నిర్వచితమైతే FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    స్థిర శూన్య
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    పూర్ణాంకం		len;

    ఒకవేళ (లోపల_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != ఖాళీ ? (char_u *)blob->bv_ga.ga_దత్తాంశం : (char_u *)"";
	len = blob != ఖాళీ ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_బైట్లు(p, len, ఖాళీ, 0));
    }
    లేకపోతే
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_బైట్లు(p, (పూర్ణాంకం)STRLEN(p), ఖాళీ, 0));
    }
}
#అయితే_ముగింపు // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    స్థిర శూన్య
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం do_special;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	తిరిగి;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    స్థిర శూన్య
f_shiftwidth(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = 0;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	దీర్ఘ	నిలువు;

	నిలువు = (దీర్ఘ)tv_get_సంఖ్య_chk(argvars, ఖాళీ);
	ఒకవేళ (నిలువు < 0)
	    తిరిగి;	// type error; errmsg already given
#నిర్వచితమైతే FEAT_VARTABS
	rettv->vval.v_సంఖ్య = get_sw_విలువ_నిలువు(curbuf, నిలువు, FALSE);
	తిరిగి;
#అయితే_ముగింపు
    }

    rettv->vval.v_సంఖ్య = get_sw_విలువ(curbuf);
}

/*
 * "soundfold({word})" function
 */
    స్థిర శూన్య
f_soundfold(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#నిర్వచితమైతే FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#లేకపోతే
    rettv->vval.v_string = vim_strsave(s);
#అయితే_ముగింపు
}

/*
 * "spellbadword()" function
 */
    స్థిర శూన్య
f_spellbadword(typval_T *argvars వాడని, typval_T *rettv)
{
    char_u	*మాట = (char_u *)"";
    hlf_T	attr = HLF_లెక్క;
    పూర్ణాంకం		len = 0;
#నిర్వచితమైతే FEAT_SPELL
    పూర్ణాంకం		wo_spell_save = curwin->w_p_spell;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    ఒకవేళ (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	తిరిగి;
    }
#అయితే_ముగింపు

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
    {
#నిర్వచితమైతే FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#అయితే_ముగింపు
	తిరిగి;
    }

#నిర్వచితమైతే FEAT_SPELL
    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_గమ్యం(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	ఒకవేళ (len != 0)
	{
	    మాట = ml_get_cursor();
	    curwin->w_సమితి_curswant = true;
	}
    }
    లేకపోతే ఒకవేళ (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	పూర్ణాంకం	capcol = -1;

	ఒకవేళ (str != ఖాళీ)
	{
	    // Check the argument for spelling.
	    అయితే (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		ఒకవేళ (attr != HLF_లెక్క)
		{
		    మాట = str;
		    ఆపు;
		}
		str += len;
		capcol -= len;
		len = 0;
	    }
	}
    }
    curwin->w_p_spell = wo_spell_save;
#అయితే_ముగింపు

    list_append_string(rettv->vval.v_list, మాట, len);
    ఎంపిక (attr)
    {
    స్థితి HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	ఆపు;

    స్థితి HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	ఆపు;

    స్థితి HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	ఆపు;

    స్థితి HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	ఆపు;

    అప్రమేయ:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	ఆపు;
    }
}

/*
 * "spellsuggest()" function
 */
    స్థిర శూన్య
f_spellsuggest(typval_T *argvars వాడని, typval_T *rettv)
{
#నిర్వచితమైతే FEAT_SPELL
    char_u	*str;
    పూర్ణాంకం		typeerr = FALSE;
    పూర్ణాంకం		maxcount;
    garray_T	ga;
    పూర్ణాంకం		i;
    listitem_T	*li;
    పూర్ణాంకం		need_capital = FALSE;
    పూర్ణాంకం		wo_spell_save = curwin->w_p_spell;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	తిరిగి;

    ఒకవేళ (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    ఒకవేళ (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	తిరిగి;
    }
#అయితే_ముగింపు

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
    {
#నిర్వచితమైతే FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#అయితే_ముగింపు
	తిరిగి;
    }

#నిర్వచితమైతే FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[1], &typeerr);
	ఒకవేళ (maxcount <= 0)
	    తిరిగి;
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (పూర్ణాంకం)tv_get_bool_chk(&argvars[2], &typeerr);
	    ఒకవేళ (typeerr)
		తిరిగి;
	}
    }
    లేకపోతే
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    కొరకు (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_దత్తాంశం)[i];

	li = listitem_alloc();
	ఒకవేళ (li == ఖాళీ)
	    vim_free(str);
	లేకపోతే
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_తాళం = 0;
	    li->li_tv.vval.v_string = str;
	    list_append(rettv->vval.v_list, li);
	}
    }
    ga_clear(&ga);
    curwin->w_p_spell = wo_spell_save;
#అయితే_ముగింపు
}

    స్థిర శూన్య
f_విభజించు(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*చివర;
    char_u	*pat = ఖాళీ;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    పూర్ణాంకం		సరిపోలిక;
    colnr_T	నిలువు = 0;
    పూర్ణాంకం		keepempty = FALSE;
    పూర్ణాంకం		typeerr = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	తిరిగి;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ఖాళీలు_option;

    str = tv_get_string(&argvars[0]);
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	ఒకవేళ (pat == ఖాళీ)
	    typeerr = TRUE;
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (పూర్ణాంకం)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    ఒకవేళ (pat == ఖాళీ || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	వెళ్ళు theend;
    ఒకవేళ (typeerr)
	వెళ్ళు theend;

    regmatch.regprog = eval_regcomp(pat);
    ఒకవేళ (regmatch.regprog != ఖాళీ)
    {
	regmatch.rm_ic = FALSE;
	అయితే (*str != NUL || keepempty)
	{
	    ఒకవేళ (*str == NUL)
		సరిపోలిక = FALSE;	// empty item at the end
	    లేకపోతే
		సరిపోలిక = vim_regexec_nl(&regmatch, str, నిలువు);
	    ఒకవేళ (సరిపోలిక)
		చివర = regmatch.startp[0];
	    లేకపోతే
		చివర = str + STRLEN(str);
	    ఒకవేళ (keepempty || చివర > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && సరిపోలిక && చివర < regmatch.endp[0]))
	    {
		ఒకవేళ (list_append_string(rettv->vval.v_list, str,
						    (పూర్ణాంకం)(చివర - str)) == FAIL)
		    ఆపు;
	    }
	    ఒకవేళ (!సరిపోలిక)
		ఆపు;
	    // Advance to just after the match.
	    ఒకవేళ (regmatch.endp[0] > str)
		నిలువు = 0;
	    లేకపోతే
		// Don't get stuck at the same match.
		నిలువు = (*mb_ptr2len)(regmatch.endp[0]);
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
    స్థిర శూన్య
f_submatch(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		error = FALSE;
    పూర్ణాంకం		no;
    పూర్ణాంకం		retList = 0;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	తిరిగి;

    no = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[0], &error);
    ఒకవేళ (error)
	తిరిగి;
    ఒకవేళ (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_సంఖ్య_nr), no);
	తిరిగి;
    }
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	retList = (పూర్ణాంకం)tv_get_bool_chk(&argvars[1], &error);
    ఒకవేళ (error)
	తిరిగి;

    ఒకవేళ (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    లేకపోతే
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = reg_submatch_list(no);
    }
}

/*
 * "substitute()" function
 */
    స్థిర శూన్య
f_substitute(typval_T *argvars, typval_T *rettv)
{
    char_u	patbuf[NUMBUFLEN];
    char_u	subbuf[NUMBUFLEN];
    char_u	flagsbuf[NUMBUFLEN];
    char_u	*str;
    char_u	*pat;
    char_u	*sub = ఖాళీ;
    typval_T	*సమాసం = ఖాళీ;
    char_u	*flg;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	తిరిగి;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    ఒకవేళ (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	సమాసం = &argvars[2];
    లేకపోతే
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    ఒకవేళ (str == ఖాళీ || pat == ఖాళీ || (sub == ఖాళీ && సమాసం == ఖాళీ)
								|| flg == ఖాళీ)
	rettv->vval.v_string = ఖాళీ;
    లేకపోతే
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, సమాసం, flg, ఖాళీ);
}

/*
 * "swapfilelist()" function
 */
    స్థిర శూన్య
f_swapfilelist(typval_T *argvars వాడని, typval_T *rettv)
{
    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;
    recover_పేర్లు(ఖాళీ, FALSE, rettv->vval.v_list, 0, ఖాళీ);
}

/*
 * "swapinfo(swap_filename)" function
 */
    స్థిర శూన్య
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (rettv_dict_alloc(rettv) == OK)
	get_b0_dict(tv_get_string(argvars), rettv->vval.v_dict);
}

/*
 * "swapname(expr)" function
 */
    స్థిర శూన్య
f_swapname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = tv_get_buf(&argvars[0], FALSE);
    ఒకవేళ (buf == ఖాళీ || buf->b_ml.ml_mfp == ఖాళీ
					|| buf->b_ml.ml_mfp->mf_fname == ఖాళీ)
	rettv->vval.v_string = ఖాళీ;
    లేకపోతే
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    స్థిర శూన్య
f_synID(typval_T *argvars వాడని, typval_T *rettv)
{
    పూర్ణాంకం		id = 0;
#నిర్వచితమైతే FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	నిలువు;
    పూర్ణాంకం		trans;
    పూర్ణాంకం		transerr = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	తిరిగి;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    నిలువు = (linenr_T)tv_get_సంఖ్య(&argvars[1]) - 1;	// -1 on type error
    trans = (పూర్ణాంకం)tv_get_bool_chk(&argvars[2], &transerr);

    ఒకవేళ (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క
	    && నిలువు >= 0 && నిలువు < (దీర్ఘ)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, నిలువు, trans, ఖాళీ, FALSE);
#అయితే_ముగింపు

    rettv->vval.v_సంఖ్య = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    స్థిర శూన్య
f_synIDattr(typval_T *argvars వాడని, typval_T *rettv)
{
    char_u	*p = ఖాళీ;
#నిర్వచితమైతే FEAT_SYN_HL
    పూర్ణాంకం		id;
    char_u	*what;
    char_u	*బహుళకం;
    char_u	modebuf[NUMBUFLEN];
    పూర్ణాంకం		modec;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	తిరిగి;

    id = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	బహుళకం = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(బహుళకం[0]);
	ఒకవేళ (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    లేకపోతే
    {
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	ఒకవేళ (USE_24_బిట్టు)
	    modec = 'g';
	లేకపోతే
# endif
	    ఒకవేళ (t_colors > 1)
		modec = 'c';
	    లేకపోతే
		modec = 't';
    }

    ఎంపిక (TOLOWER_ASC(what[0]))
    {
	స్థితి 'b':
		ఒకవేళ (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		లేకపోతే					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		ఆపు;

	స్థితి 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		ఆపు;

	స్థితి 'i':
		ఒకవేళ (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		లేకపోతే					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		ఆపు;

	స్థితి 'n':
		ఒకవేళ (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		లేకపోతే					// name
		    p = get_highlight_name_ext(ఖాళీ, id - 1, FALSE);
		ఆపు;

	స్థితి 'r':					//తిరగేయి
		p = highlight_has_attr(id, HL_INVERSE, modec);
		ఆపు;

	స్థితి 's':
		ఒకవేళ (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		లేకపోతే ఒకవేళ (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		లేకపోతే					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		ఆపు;

	స్థితి 'u':
		ఒకవేళ (STRLEN(what) >= 9)
		{
		    ఒకవేళ (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    లేకపోతే ఒకవేళ (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    లేకపోతే ఒకవేళ (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    లేకపోతే ఒకవేళ (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    లేకపోతే
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		లేకపోతే
							// ul
		    p = highlight_color(id, what, modec);
		ఆపు;
    }

    ఒకవేళ (p != ఖాళీ)
	p = vim_strsave(p);
#అయితే_ముగింపు
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    స్థిర శూన్య
f_synIDtrans(typval_T *argvars వాడని, typval_T *rettv)
{
    పూర్ణాంకం		id;

#నిర్వచితమైతే FEAT_SYN_HL
    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    id = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]);

    ఒకవేళ (id > 0)
	id = syn_get_final_id(id);
    లేకపోతే
#అయితే_ముగింపు
	id = 0;

    rettv->vval.v_సంఖ్య = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    స్థిర శూన్య
f_synconcealed(typval_T *argvars వాడని, typval_T *rettv)
{
#ఒకవేళ defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	నిలువు;
    పూర్ణాంకం		syntax_flags = 0;
    పూర్ణాంకం		cchar;
    పూర్ణాంకం		matchid = 0;
    char_u	str[NUMBUFLEN];
#అయితే_ముగింపు

    rettv_list_సమితి(rettv, ఖాళీ);

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

#ఒకవేళ defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    నిలువు = (colnr_T)tv_get_సంఖ్య(&argvars[1]) - 1;	// -1 on type error

    CLEAR_క్షేత్రం(str);

    ఒకవేళ (rettv_list_alloc(rettv) == OK)
    {
	ఒకవేళ (lnum >= 1 && lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క
	    && నిలువు >= 0 && నిలువు <= (దీర్ఘ)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (శూన్య)syn_get_id(curwin, lnum, నిలువు, FALSE, ఖాళీ, FALSE);
	    syntax_flags = get_syntax_info(&matchid);

	    // get the conceal character
	    ఒకవేళ ((syntax_flags & HL_CONCEAL) && curwin->w_p_cole < 3)
	    {
		cchar = syn_get_sub_char();
		ఒకవేళ (cchar == NUL && curwin->w_p_cole == 1)
		    cchar = (curwin->w_lcs_chars.conceal == NUL) ? ' '
					: curwin->w_lcs_chars.conceal;
		ఒకవేళ (cchar != NUL)
		{
		    ఒకవేళ (has_mbyte)
			(*mb_char2bytes)(cchar, str);
		    లేకపోతే
			str[0] = cchar;
		}
	    }
	}

	list_append_సంఖ్య(rettv->vval.v_list,
					    (syntax_flags & HL_CONCEAL) != 0);
	// -1 to auto-determine strlen
	list_append_string(rettv->vval.v_list, str, -1);
	list_append_సంఖ్య(rettv->vval.v_list, matchid);
    }
#అయితే_ముగింపు
}

/*
 * "synstack(lnum, col)" function
 */
    స్థిర శూన్య
f_synstack(typval_T *argvars వాడని, typval_T *rettv)
{
#నిర్వచితమైతే FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	నిలువు;
    పూర్ణాంకం		i;
    పూర్ణాంకం		id;
#అయితే_ముగింపు

    rettv_list_సమితి(rettv, ఖాళీ);

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

#నిర్వచితమైతే FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    నిలువు = (colnr_T)tv_get_సంఖ్య(&argvars[1]) - 1;	// -1 on type error

    ఒకవేళ (lnum >= 1 && lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క
	    && నిలువు >= 0 && నిలువు <= (దీర్ఘ)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(శూన్య)syn_get_id(curwin, lnum, నిలువు, FALSE, ఖాళీ, TRUE);
	కొరకు (i = 0; ; ++i)
	{
	    id = syn_get_దొంతర_అంశం(i);
	    ఒకవేళ (id < 0)
		ఆపు;
	    ఒకవేళ (list_append_సంఖ్య(rettv->vval.v_list, id) == FAIL)
		ఆపు;
	}
    }
#అయితే_ముగింపు
}

/*
 * "tabpagebuflist()" function
 */
    స్థిర శూన్య
f_tabpagebuflist(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    tabpage_T	*tp;
    win_T	*wp = ఖాళీ;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    లేకపోతే
    {
	tp = కనుగొను_tabpage((పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]));
	ఒకవేళ (tp != ఖాళీ)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    ఒకవేళ (wp != ఖాళీ && rettv_list_alloc(rettv) == OK)
    {
	కొరకు (; wp != ఖాళీ; wp = wp->w_తదుపరి)
	    ఒకవేళ (list_append_సంఖ్య(rettv->vval.v_list,
						wp->w_బఫరు->b_fnum) == FAIL)
		ఆపు;
    }
}

/*
 * "tagfiles()" function
 */
    స్థిర శూన్య
f_tagfiles(typval_T *argvars వాడని, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    పూర్ణాంకం		మొదటి;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;
    fname = alloc(MAXPATHL);
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి;

    కొరకు (మొదటి = TRUE; ; మొదటి = FALSE)
	ఒకవేళ (get_tagfname(&tn, మొదటి, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    ఆపు;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    స్థిర శూన్య
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = ఖాళీ;
    char_u  *ట్యాగ్_నమూనా;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    ట్యాగ్_నమూనా = tv_get_string(&argvars[0]);

    rettv->vval.v_సంఖ్య = FALSE;
    ఒకవేళ (*ట్యాగ్_నమూనా == NUL)
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    ఒకవేళ (rettv_list_alloc(rettv) == OK)
	(శూన్య)get_ట్యాగులు(rettv->vval.v_list, ట్యాగ్_నమూనా, fname);
}

/*
 * "type(expr)" function
 */
    స్థిర శూన్య
f_type(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం n = -1;

    ఎంపిక (argvars[0].v_type)
    {
	స్థితి VAR_సంఖ్య:  n = VAR_TYPE_సంఖ్య; ఆపు;
	స్థితి VAR_STRING:  n = VAR_TYPE_STRING; ఆపు;
	స్థితి VAR_PARTIAL:
	స్థితి VAR_FUNC:    n = VAR_TYPE_FUNC; ఆపు;
	స్థితి VAR_LIST:    n = VAR_TYPE_LIST; ఆపు;
	స్థితి VAR_TUPLE:   n = VAR_TYPE_TUPLE; ఆపు;
	స్థితి VAR_DICT:    n = VAR_TYPE_DICT; ఆపు;
	స్థితి VAR_FLOAT:   n = VAR_TYPE_FLOAT; ఆపు;
	స్థితి VAR_BOOL:	  n = VAR_TYPE_BOOL; ఆపు;
	స్థితి VAR_SPECIAL: n = VAR_TYPE_NONE; ఆపు;
	స్థితి VAR_పని:     n = VAR_TYPE_పని; ఆపు;
	స్థితి VAR_CHANNEL: n = VAR_TYPE_CHANNEL; ఆపు;
	స్థితి VAR_BLOB:    n = VAR_TYPE_BLOB; ఆపు;
	స్థితి VAR_INSTR:   n = VAR_TYPE_INSTR; ఆపు;
	స్థితి VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; ఆపు;
	స్థితి VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		ఒకవేళ (cl != ఖాళీ && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		లేకపోతే
		    n = VAR_TYPE_CLASS;
		ఆపు;
	    }
	స్థితి VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		ఒకవేళ (obj == ఖాళీ)
		    n = VAR_TYPE_OBJECT;
		లేకపోతే
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    ఒకవేళ (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    లేకపోతే
			n = VAR_TYPE_OBJECT;
		}
		ఆపు;
	    }
	స్థితి VAR_VOID:
	    emsg(_(e_cannot_use_void_విలువ));
	    ఆపు;
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	    అంతర్గత_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    ఆపు;
    }
    rettv->vval.v_సంఖ్య = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    స్థిర శూన్య
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_ప్రారంభం = 0;
    colnr_T	vcol_చివర = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    పూర్ణాంకం		winchanged = FALSE;
    పూర్ణాంకం		len;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL))))
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((పూర్ణాంకం)tv_get_సంఖ్య(&argvars[2]), &tp);
	ఒకవేళ (wp == ఖాళీ || tp == ఖాళీ)
	    వెళ్ళు theend;

	ఒకవేళ (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    వెళ్ళు theend;

	check_cursor();
	winchanged = TRUE;
    }

    పూర్ణాంకం fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    ఒకవేళ (fp != ఖాళీ && fp->lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	ఒకవేళ (fp->నిలువు < 0)
	    fp->నిలువు = 0;
	లేకపోతే
	{
	    len = (పూర్ణాంకం)ml_get_len(fp->lnum);
	    ఒకవేళ (fp->నిలువు > len)
		fp->నిలువు = len;
	}
	getvvcol(curwin, fp, &vcol_ప్రారంభం, ఖాళీ, &vcol_చివర, 0);
	++vcol_ప్రారంభం;
	++vcol_చివర;
    }

theend:
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	ఒకవేళ (rettv_list_alloc(rettv) == OK)
	{
	    list_append_సంఖ్య(rettv->vval.v_list, vcol_ప్రారంభం);
	    list_append_సంఖ్య(rettv->vval.v_list, vcol_చివర);
	}
	లేకపోతే
	    rettv->vval.v_సంఖ్య = 0;
    }
    లేకపోతే
	rettv->vval.v_సంఖ్య = vcol_చివర;

    ఒకవేళ (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    స్థిర శూన్య
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    ఒకవేళ (లోపల_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_బహుళకం_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    ఒకవేళ (non_zero_arg(&argvars[0]))
	curbuf->b_visual_బహుళకం_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    స్థిర శూన్య
f_wildmenumode(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    ఒకవేళ (wild_menu_showing || ((State & బహుళకం_CMDLINE) && cmdline_pum_చురుకైన()))
	rettv->vval.v_సంఖ్య = 1;
}

/*
 * "windowsversion()" function
 */
    స్థిర శూన్య
f_windowsversion(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windows_సంస్కరణ);
}

/*
 * "wordcount()" function
 */
    స్థిర శూన్య
f_wordcount(typval_T *argvars వాడని, typval_T *rettv)
{
    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    స్థిర శూన్య
f_xor(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    rettv->vval.v_సంఖ్య = tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ)
					^ tv_get_సంఖ్య_chk(&argvars[1], ఖాళీ);
}

#అయితే_ముగింపు // FEAT_EVAL
