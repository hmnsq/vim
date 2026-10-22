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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

स्थिर शून्य f_and(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_BEVAL
स्थिर शून्य f_balloon_gettext(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_balloon_show(typval_T *argvars, typval_T *rettv);
# यदि defined(FEAT_BEVAL_TERM)
स्थिर शून्य f_balloon_split(typval_T *argvars, typval_T *rettv);
# endif
#endif
स्थिर शून्य f_base64_encode(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_base64_decode(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_bindtextdomain(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_byte2line(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_call(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_changenr(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_char2nr(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_charcol(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_col(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_confirm(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_copy(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_cursor(typval_T *argsvars, typval_T *rettv);
#ifdef MSWIN
स्थिर शून्य f_debugbreak(typval_T *argvars, typval_T *rettv);
#endif
स्थिर शून्य f_deepcopy(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_did_filetype(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_echoraw(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_empty(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_environ(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_err_teapot(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_escape(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_eval(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_eventhandler(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_execute(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_exists_compiled(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_expand(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_expandcmd(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_feedkeys(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_fnameescape(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_foreground(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_funcref(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_function(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_garbagecollect(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_get(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getbgcolor(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getcellpixels(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getchangelist(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getcharpos(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getcharsearch(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getcurpos(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getcursorcharpos(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getenv(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getfontname(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getinfo(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getjumplist(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getpid(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getpos(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getreg(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getreginfo(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getregion(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getregionpos(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_getregtype(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_gettagstack(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_gettext(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_haslocaldir(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_hlID(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_hlexists(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_hostname(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_id(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_index(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_indexof(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_निवेश(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_inputdialog(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_inputlist(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_inputrestore(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_inputsave(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_inputsecret(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_interrupt(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_invert(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_islocked(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_वस्तुएँ(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_keytrans(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_last_buffer_nr(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_libcall(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_libcallnr(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_पंक्ति(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_line2byte(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_LUA
स्थिर शून्य f_luaeval(typval_T *argvars, typval_T *rettv);
#endif
स्थिर शून्य f_match(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_matchbufline(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_matchend(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_matchlist(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_matchstr(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_matchstrlist(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_matchstrpos(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_max(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_min(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_MZSCHEME
स्थिर शून्य f_mzeval(typval_T *argvars, typval_T *rettv);
#endif
स्थिर शून्य f_nextnonblank(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_ngettext(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_nr2char(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_or(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PERL
स्थिर शून्य f_perleval(typval_T *argvars, typval_T *rettv);
#endif
स्थिर शून्य f_prevnonblank(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_printf(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_pum_getpos(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_pumvisible(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PYTHON3
स्थिर शून्य f_py3eval(typval_T *argvars, typval_T *rettv);
#endif
#ifdef FEAT_PYTHON
स्थिर शून्य f_pyeval(typval_T *argvars, typval_T *rettv);
#endif
#यदि defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
स्थिर शून्य f_pyxeval(typval_T *argvars, typval_T *rettv);
#endif
स्थिर शून्य f_test_srand_seed(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_rand(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_range(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_reg_executing(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_reg_recording(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_rename(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_repeat(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_RUBY
स्थिर शून्य f_rubyeval(typval_T *argvars, typval_T *rettv);
#endif
स्थिर शून्य f_screenattr(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_screenchar(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_screenchars(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_screencol(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_screenrow(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_screenstring(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_खोज(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_searchdecl(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_searchpair(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_searchpairpos(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_searchpos(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_setcharpos(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_setcharsearch(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_setcursorcharpos(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_setenv(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_setfperm(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_setpos(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_setreg(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_settagstack(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_CRYPT
स्थिर शून्य f_sha256(typval_T *argvars, typval_T *rettv);
#endif
स्थिर शून्य f_shellescape(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_shiftwidth(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_soundfold(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_spellbadword(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_spellsuggest(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_split(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_srand(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_submatch(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_substitute(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_swapfilelist(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_swapinfo(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_swapname(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_synID(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_synIDattr(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_synIDtrans(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_synstack(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_synconcealed(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_tabpagebuflist(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_taglist(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_tagfiles(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_type(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_virtcol(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_visualmode(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_wildmenumode(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_windowsversion(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_wordcount(typval_T *argvars, typval_T *rettv);
स्थिर शून्य f_xor(typval_T *argvars, typval_T *rettv);


/*
 * Functions that check the argument type of a builtin function.
 * Each function returns FAIL and gives an error message if the type is wrong.
 */

// Context passed to an arg_ function.
प्रकारनिर्देश संरचना {
    पूर्णाङ्क		arg_गिनती;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    पूर्णाङ्क		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
प्रकारनिर्देश पूर्णाङ्क (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    स्थिर पूर्णाङ्क
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    प्रत्यागम need_type(actual, expected, 0,
	    context->arg_idx - context->arg_गिनती, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    स्थिर पूर्णाङ्क
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    यदि (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_गिनती, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	प्रत्यागम FAIL;
    प्रत्यागम arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    पूर्णाङ्क
arg_type_modifiable(type_T *type, पूर्णाङ्क arg_idx)
{
    वर्ण *tofree;

    यदि ((type->tt_flags & TTFLAG_CONST) == 0)
	प्रत्यागम OK;
    semsg(_(e_argument_nr_trying_to_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    प्रत्यागम FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    स्थिर पूर्णाङ्क
arg_any(type_T *type UNUSED,
	type_T *decl_type UNUSED,
	argcontext_T *context UNUSED)
{
    प्रत्यागम OK;
}

/*
 * Check "type" is a float or a number.
 */
    स्थिर पूर्णाङ्क
arg_float_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_संख्या
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_संख्या, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a number.
 */
    स्थिर पूर्णाङ्क
arg_संख्या(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    प्रत्यागम check_arg_type(&t_संख्या, type, context);
}

/*
 * Check "type" is an object.
 */
    स्थिर पूर्णाङ्क
arg_object(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    स्थिर पूर्णाङ्क
arg_dict_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    प्रत्यागम check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    स्थिर पूर्णाङ्क
arg_list_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    प्रत्यागम check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    स्थिर पूर्णाङ्क
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    प्रत्यागम check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    स्थिर पूर्णाङ्क
arg_list_संख्या(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    प्रत्यागम check_arg_type(&t_list_संख्या, type, context);
}

/*
 * Check "type" is a list of strings.
 */
    स्थिर पूर्णाङ्क
arg_list_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    प्रत्यागम check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    स्थिर पूर्णाङ्क
arg_tuple_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    प्रत्यागम check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    स्थिर पूर्णाङ्क
arg_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    प्रत्यागम check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    स्थिर पूर्णाङ्क
arg_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    प्रत्यागम check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    स्थिर पूर्णाङ्क
arg_bool(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    प्रत्यागम check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    स्थिर पूर्णाङ्क
arg_bool_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_संख्या
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_संख्या, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    स्थिर पूर्णाङ्क
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	प्रत्यागम OK;
    प्रत्यागम check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    स्थिर पूर्णाङ्क
arg_list_or_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    स्थिर पूर्णाङ्क
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    यदि (arg_list_or_blob(type, decl_type, context) == FAIL)
	प्रत्यागम FAIL;
    प्रत्यागम arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    स्थिर पूर्णाङ्क
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    स्थिर पूर्णाङ्क
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a string or a number
 */
    स्थिर पूर्णाङ्क
arg_string_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_संख्या
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    स्थिर पूर्णाङ्क
arg_buffer(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_संख्या
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    स्थिर पूर्णाङ्क
arg_buffer_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_संख्या
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    स्थिर पूर्णाङ्क
arg_lnum(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_संख्या
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    स्थिर पूर्णाङ्क
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    यदि (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	प्रत्यागम FAIL;
    }
    यदि (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	प्रत्यागम OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    स्थिर पूर्णाङ्क
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    स्थिर पूर्णाङ्क
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    स्थिर पूर्णाङ्क
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    स्थिर पूर्णाङ्क
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    स्थिर पूर्णाङ्क
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    यदि (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    स्थिर पूर्णाङ्क
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    यदि (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	प्रत्यागम FAIL;
    प्रत्यागम arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    स्थिर पूर्णाङ्क
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    स्थिर पूर्णाङ्क
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    यदि (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	प्रत्यागम FAIL;
    प्रत्यागम arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    स्थिर पूर्णाङ्क
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	प्रत्यागम OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr),
						 context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    स्थिर पूर्णाङ्क
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = रिक्त;
    type_T *(args[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, रिक्त, रिक्त, args};

    यदि (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	यदि (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	अन्यथा
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    अन्यथा यदि (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    अन्यथा यदि (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_संख्या;

    args[0] = रिक्त;
    args[1] = &t_unknown;
    यदि (type->tt_argcount != -1)
    {
	यदि (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_संख्या_of_arguments));
	    प्रत्यागम FAIL;
	}
	यदि (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	अन्यथा
	{
	    यदि (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		args[0] = &t_संख्या;
	    अन्यथा यदि (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		args[0] = &t_string;
	    यदि (args[0] != रिक्त)
		args[1] = expected_member;
	}
    }

    यदि (!type_any_or_unknown(type->tt_member) || args[0] != रिक्त)
    {
	where_T where = WHERE_INIT;

	यदि (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == रिक्त
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	अन्यथा यदि (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	अन्यथा // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	यदि (args[0] == रिक्त)
	    args[0] = &t_unknown;
	यदि (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_index = 2;
	where.wt_kind = WT_ARGUMENT;
	प्रत्यागम check_type(&t_func_exp, type, TRUE, where);
    }
    प्रत्यागम OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    स्थिर पूर्णाङ्क
arg_filter_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	प्रत्यागम OK;

    यदि (type->tt_type == VAR_FUNC)
	प्रत्यागम check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    प्रत्यागम FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    स्थिर पूर्णाङ्क
arg_map_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	प्रत्यागम OK;

    यदि (type->tt_type == VAR_FUNC)
	प्रत्यागम check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    प्रत्यागम FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    स्थिर पूर्णाङ्क
arg_foreach_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	प्रत्यागम OK;

    यदि (type->tt_type == VAR_FUNC)
	प्रत्यागम check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    प्रत्यागम FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    स्थिर पूर्णाङ्क
arg_sort_how(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	प्रत्यागम OK;

    यदि (type->tt_type == VAR_FUNC)
    {
	type_T *(args[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_संख्या, रिक्त, args};

	यदि (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    args[0] = context->arg_types[0].type_curr->tt_member;
	अन्यथा
	    args[0] = &t_unknown;
	यदि (!type_any_or_unknown(type->tt_member) || args[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    args[1] = args[0];
	    यदि (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_index = 2;
	    where.wt_kind = WT_ARGUMENT;
	    प्रत्यागम check_type(&t_func_exp, type, TRUE, where);
	}

	प्रत्यागम OK;
    }
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    प्रत्यागम FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    स्थिर पूर्णाङ्क
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_संख्या
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    स्थिर पूर्णाङ्क
varargs_class(type_T *type UNUSED,
	      type_T *decl_type UNUSED,
	      argcontext_T *context)
{
    प्रति (पूर्णाङ्क i = context->arg_idx; i < context->arg_गिनती; ++i)
    {
	type2_T *types = &context->arg_types[i];
	यदि (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    प्रत्यागम FAIL;
	}
    }
    प्रत्यागम OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    स्थिर पूर्णाङ्क
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    स्थिर पूर्णाङ्क
arg_reverse(type_T *type, type_T *decl_type, argcontext_T *context)
{
    यदि (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	प्रत्यागम FAIL;
    प्रत्यागम arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    स्थिर पूर्णाङ्क
arg_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    प्रत्यागम check_arg_type(&t_job, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    स्थिर पूर्णाङ्क
arg_chan_or_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_JOB
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    स्थिर पूर्णाङ्क
arg_same_as_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_decl;

    प्रत्यागम check_arg_type(prev_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    स्थिर पूर्णाङ्क
arg_same_struct_as_prev(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;

    यदि (prev_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	प्रत्यागम check_arg_type(prev_type, type, context);
    प्रत्यागम OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    स्थिर पूर्णाङ्क
arg_item_of_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    यदि (prev_type->tt_type == VAR_LIST)
	expected = prev_type->tt_member;
    अन्यथा यदि (prev_type->tt_type == VAR_BLOB)
	expected = &t_संख्या;
    अन्यथा
	// probably VAR_ANY, can't check
	प्रत्यागम OK;

    प्रत्यागम check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    स्थिर पूर्णाङ्क
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_संख्या
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    स्थिर पूर्णाङ्क
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	प्रत्यागम OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    स्थिर पूर्णाङ्क
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 2].type_curr;

    यदि (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	प्रत्यागम arg_संख्या(type, decl_type, context);
    यदि (first_type->tt_type == VAR_DICT)
	प्रत्यागम arg_string(type, decl_type, context);
    प्रत्यागम OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    स्थिर पूर्णाङ्क
arg_get1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	प्रत्यागम OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    स्थिर पूर्णाङ्क
arg_len1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_संख्या
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	प्रत्यागम OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    स्थिर पूर्णाङ्क
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 1].type_curr;

    यदि (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	प्रत्यागम arg_संख्या(type, decl_type, context);
    यदि (first_type->tt_type == VAR_DICT)
	प्रत्यागम arg_string_or_nr(type, decl_type, context);
    प्रत्यागम OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    स्थिर पूर्णाङ्क
arg_repeat1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_संख्या
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	प्रत्यागम OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    स्थिर पूर्णाङ्क
arg_slice1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	प्रत्यागम OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    स्थिर पूर्णाङ्क
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    यदि (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	प्रत्यागम OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr),
							 context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    स्थिर पूर्णाङ्क
arg_cursor1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    यदि (type->tt_type == VAR_संख्या
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	प्रत्यागम OK;

    arg_type_mismatch(&t_संख्या, type, context->arg_idx + 1);
    प्रत्यागम FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
स्थिर argcheck_T arg1_blob[] = {arg_blob};
स्थिर argcheck_T arg1_bool[] = {arg_bool};
स्थिर argcheck_T arg1_buffer[] = {arg_buffer};
स्थिर argcheck_T arg1_buffer_or_dict_any[] = {arg_buffer_or_dict_any};
स्थिर argcheck_T arg1_chan_or_job[] = {arg_chan_or_job};
स्थिर argcheck_T arg1_dict_any[] = {arg_dict_any};
स्थिर argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
स्थिर argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
स्थिर argcheck_T arg1_job[] = {arg_job};
स्थिर argcheck_T arg1_list_any[] = {arg_list_any};
स्थिर argcheck_T arg1_tuple_any[] = {arg_tuple_any};
स्थिर argcheck_T arg1_list_संख्या[] = {arg_list_संख्या};
स्थिर argcheck_T arg1_reverse[] = {arg_reverse};
स्थिर argcheck_T arg1_list_or_tuple_or_dict[] = {arg_list_or_tuple_or_dict};
स्थिर argcheck_T arg1_list_string[] = {arg_list_string};
स्थिर argcheck_T arg1_list_tuple_dict_blob_or_string[] = {arg_list_tuple_dict_blob_or_string};
स्थिर argcheck_T arg1_lnum[] = {arg_lnum};
स्थिर argcheck_T arg1_संख्या[] = {arg_संख्या};
स्थिर argcheck_T arg1_string[] = {arg_string};
स्थिर argcheck_T arg1_string_or_list_any[] = {arg_string_or_list_any};
स्थिर argcheck_T arg1_string_or_list_string[] = {arg_string_or_list_string};
स्थिर argcheck_T arg1_string_or_nr[] = {arg_string_or_nr};
स्थिर argcheck_T arg1_string_or_blob[] = {arg_string_or_blob};
स्थिर argcheck_T arg2_buffer_any[] = {arg_buffer, arg_any};
स्थिर argcheck_T arg2_buffer_bool[] = {arg_buffer, arg_bool};
स्थिर argcheck_T arg2_buffer_list_any[] = {arg_buffer, arg_list_any};
स्थिर argcheck_T arg2_buffer_lnum[] = {arg_buffer, arg_lnum};
स्थिर argcheck_T arg2_buffer_संख्या[] = {arg_buffer, arg_संख्या};
स्थिर argcheck_T arg2_buffer_string[] = {arg_buffer, arg_string};
स्थिर argcheck_T arg2_chan_or_job_dict[] = {arg_chan_or_job, arg_dict_any};
स्थिर argcheck_T arg2_chan_or_job_string[] = {arg_chan_or_job, arg_string};
स्थिर argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
स्थिर argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
स्थिर argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
स्थिर argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
स्थिर argcheck_T arg2_job_dict[] = {arg_job, arg_dict_any};
स्थिर argcheck_T arg2_job_string_or_संख्या[] = {arg_job, arg_string_or_nr};
स्थिर argcheck_T arg2_list_any_संख्या[] = {arg_list_any, arg_संख्या};
स्थिर argcheck_T arg2_list_संख्या[] = {arg_list_संख्या, arg_list_संख्या};
स्थिर argcheck_T arg2_list_संख्या_bool[] = {arg_list_संख्या, arg_bool};
स्थिर argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
स्थिर argcheck_T arg2_listblobmod_item[] = {arg_list_or_blob_mod, arg_item_of_prev};
स्थिर argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
स्थिर argcheck_T arg2_lnum_संख्या[] = {arg_lnum, arg_संख्या};
स्थिर argcheck_T arg2_संख्या[] = {arg_संख्या, arg_संख्या};
स्थिर argcheck_T arg2_संख्या_any[] = {arg_संख्या, arg_any};
स्थिर argcheck_T arg2_संख्या_bool[] = {arg_संख्या, arg_bool};
स्थिर argcheck_T arg2_संख्या_dict_any[] = {arg_संख्या, arg_dict_any};
स्थिर argcheck_T arg2_संख्या_list[] = {arg_संख्या, arg_list_any};
स्थिर argcheck_T arg2_संख्या_string[] = {arg_संख्या, arg_string};
स्थिर argcheck_T arg2_संख्या_buffer[] = {arg_संख्या, arg_buffer};
स्थिर argcheck_T arg2_संख्या_string_or_list[] = {arg_संख्या, arg_string_or_list_any};
स्थिर argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
स्थिर argcheck_T arg2_string[] = {arg_string, arg_string};
स्थिर argcheck_T arg2_string_any[] = {arg_string, arg_any};
स्थिर argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
स्थिर argcheck_T arg2_string_chan_or_job[] = {arg_string, arg_chan_or_job};
स्थिर argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
स्थिर argcheck_T arg2_string_list_संख्या[] = {arg_string, arg_list_संख्या};
स्थिर argcheck_T arg2_string_संख्या[] = {arg_string, arg_संख्या};
स्थिर argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
स्थिर argcheck_T arg2_string_or_list_संख्या[] = {arg_string_or_list_any, arg_संख्या};
स्थिर argcheck_T arg2_string_string_or_संख्या[] = {arg_string, arg_string_or_nr};
स्थिर argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
स्थिर argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
स्थिर argcheck_T arg3_any_buffer_bool_or_dict[] = {
			      arg_any, arg_buffer, arg_bool_or_dict_any};
स्थिर argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
स्थिर argcheck_T arg3_buffer_lnum_lnum[] = {arg_buffer, arg_lnum, arg_lnum};
स्थिर argcheck_T arg3_buffer_संख्या_संख्या[] = {arg_buffer, arg_संख्या, arg_संख्या};
स्थिर argcheck_T arg3_buffer_string_any[] = {arg_buffer, arg_string, arg_any};
स्थिर argcheck_T arg3_buffer_string_dict[] = {arg_buffer, arg_string, arg_dict_any};
स्थिर argcheck_T arg3_dict_संख्या_संख्या[] = {arg_dict_any, arg_संख्या, arg_संख्या};
स्थिर argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
स्थिर argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
स्थिर argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
स्थिर argcheck_T arg3_lnum_संख्या_bool[] = {arg_lnum, arg_संख्या, arg_bool};
स्थिर argcheck_T arg3_संख्या[] = {arg_संख्या, arg_संख्या, arg_संख्या};
स्थिर argcheck_T arg3_संख्या_any_dict[] = {arg_संख्या, arg_any, arg_dict_any};
स्थिर argcheck_T arg3_संख्या_संख्या_dict[] = {arg_संख्या, arg_संख्या, arg_dict_any};
स्थिर argcheck_T arg3_संख्या_string_any[] = {arg_संख्या, arg_string, arg_any};
स्थिर argcheck_T arg3_संख्या_string_buffer[] = {arg_संख्या, arg_string, arg_buffer};
स्थिर argcheck_T arg3_संख्या_string_string[] = {arg_संख्या, arg_string, arg_string};
स्थिर argcheck_T arg3_string[] = {arg_string, arg_string, arg_string};
स्थिर argcheck_T arg3_string_any_dict[] = {arg_string, arg_any, arg_dict_any};
स्थिर argcheck_T arg3_string_any_string[] = {arg_string, arg_any, arg_string};
स्थिर argcheck_T arg3_string_bool_bool[] = {arg_string, arg_bool, arg_bool};
स्थिर argcheck_T arg3_string_संख्या_bool[] = {arg_string, arg_संख्या, arg_bool};
स्थिर argcheck_T arg3_string_संख्या_संख्या[] = {arg_string, arg_संख्या, arg_संख्या};
स्थिर argcheck_T arg3_string_or_dict_bool_dict[] = {arg_string_or_dict_any, arg_bool, arg_dict_any};
स्थिर argcheck_T arg3_string_or_list_bool_संख्या[] = {arg_string_or_list_any, arg_bool, arg_संख्या};
स्थिर argcheck_T arg3_string_string_bool[] = {arg_string, arg_string, arg_bool};
स्थिर argcheck_T arg3_string_string_dict[] = {arg_string, arg_string, arg_dict_any};
स्थिर argcheck_T arg3_string_string_संख्या[] = {arg_string, arg_string, arg_संख्या};
स्थिर argcheck_T arg4_संख्या_संख्या_string_any[] = {arg_संख्या, arg_संख्या, arg_string, arg_any};
स्थिर argcheck_T arg4_string_string_any_string[] = {arg_string, arg_string, arg_any, arg_string};
स्थिर argcheck_T arg4_string_string_संख्या_string[] = {arg_string, arg_string, arg_संख्या, arg_string};
स्थिर argcheck_T arg4_string_संख्या_bool_bool[] = {arg_string, arg_संख्या, arg_bool, arg_bool};
/* Function specific argument types (not covered by the above) */
स्थिर argcheck_T arg15_assert_fails[] = {arg_string_or_nr, arg_string_or_list_any, arg_any, arg_संख्या, arg_string};
स्थिर argcheck_T arg34_assert_inrange[] = {arg_float_or_nr, arg_float_or_nr, arg_float_or_nr, arg_string};
स्थिर argcheck_T arg4_browse[] = {arg_bool, arg_string, arg_string, arg_string};
स्थिर argcheck_T arg23_chanexpr[] = {arg_chan_or_job, arg_any, arg_dict_any};
स्थिर argcheck_T arg23_chanraw[] = {arg_chan_or_job, arg_string_or_blob, arg_dict_any};
स्थिर argcheck_T arg24_गिनती[] = {arg_string_list_tuple_or_dict, arg_any, arg_bool, arg_संख्या};
स्थिर argcheck_T arg13_cursor[] = {arg_cursor1, arg_संख्या, arg_संख्या};
स्थिर argcheck_T arg12_deepcopy[] = {arg_any, arg_bool};
स्थिर argcheck_T arg12_execute[] = {arg_string_or_list_string, arg_string};
स्थिर argcheck_T arg12_getchar[] = {arg_bool_or_nr, arg_dict_any};
स्थिर argcheck_T arg23_extend[] = {arg_list_or_dict_or_blob_mod, arg_same_as_prev, arg_extend3};
स्थिर argcheck_T arg23_extendnew[] = {arg_list_or_dict_or_blob, arg_same_struct_as_prev, arg_extend3};
स्थिर argcheck_T arg23_get[] = {arg_get1, arg_string_or_nr, arg_any};
स्थिर argcheck_T arg14_glob[] = {arg_string, arg_bool, arg_bool, arg_bool};
स्थिर argcheck_T arg25_globpath[] = {arg_string, arg_string, arg_bool, arg_bool, arg_bool};
स्थिर argcheck_T arg24_index[] = {arg_list_or_tuple_or_blob, arg_item_of_prev, arg_संख्या, arg_bool};
स्थिर argcheck_T arg23_index[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
स्थिर argcheck_T arg23_जोड़ो[] = {arg_list_or_blob, arg_item_of_prev, arg_संख्या};
स्थिर argcheck_T arg1_len[] = {arg_len1};
स्थिर argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
स्थिर argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
स्थिर argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
स्थिर argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
स्थिर argcheck_T arg2_instanceof[] = {arg_object, varargs_class, रिक्त };
स्थिर argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
स्थिर argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
स्थिर argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_संख्या, arg_संख्या, arg_dict_any};
स्थिर argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_संख्या, arg_संख्या, arg_dict_any};
स्थिर argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
स्थिर argcheck_T arg45_matchbufline[] = {arg_buffer, arg_string, arg_lnum, arg_lnum, arg_dict_any};
स्थिर argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
स्थिर argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
स्थिर argcheck_T arg24_remote_व्यंजक[] = {arg_string, arg_string, arg_string, arg_संख्या};
स्थिर argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_संख्या};
स्थिर argcheck_T arg2_repeat[] = {arg_repeat1, arg_संख्या};
स्थिर argcheck_T arg15_खोज[] = {arg_string, arg_string, arg_संख्या, arg_संख्या, arg_string_or_func};
स्थिर argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_संख्या, arg_संख्या};
स्थिर argcheck_T arg3_setbufline[] = {arg_buffer, arg_lnum, arg_str_or_nr_or_list};
स्थिर argcheck_T arg2_setline[] = {arg_lnum, arg_any};
स्थिर argcheck_T arg24_setloclist[] = {arg_संख्या, arg_list_any, arg_string, arg_dict_any};
स्थिर argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
स्थिर argcheck_T arg23_settagstack[] = {arg_संख्या, arg_dict_any, arg_string};
स्थिर argcheck_T arg02_sign_getplaced[] = {arg_buffer, arg_dict_any};
स्थिर argcheck_T arg45_sign_place[] = {arg_संख्या, arg_string, arg_string, arg_buffer, arg_dict_any};
स्थिर argcheck_T arg23_slice[] = {arg_slice1, arg_संख्या, arg_संख्या};
स्थिर argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_sort_how, arg_dict_any};
स्थिर argcheck_T arg24_strpart[] = {arg_string, arg_संख्या, arg_संख्या, arg_bool};
स्थिर argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
स्थिर argcheck_T arg23_win_execute[] = {arg_संख्या, arg_string_or_list_string, arg_string};
स्थिर argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
स्थिर argcheck_T arg24_match_func[] = {arg_string_or_list_any, arg_string, arg_संख्या, arg_संख्या};

// Can be used by functions called through "f_retfunc" to create new types.
स्थिर garray_T *current_type_gap = रिक्त;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    स्थिर type_T *
ret_void(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_void;
}
    स्थिर type_T *
ret_any(पूर्णाङ्क	argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_any;
}
    स्थिर type_T *
ret_bool(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_bool;
}
    स्थिर type_T *
ret_संख्या_bool(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_संख्या_bool;
}
    स्थिर type_T *
ret_संख्या(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_संख्या;
}
    स्थिर type_T *
ret_float(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_float;
}
    स्थिर type_T *
ret_string(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_string;
}
    स्थिर type_T *
ret_list_any(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_list_any;
}
    स्थिर type_T *
ret_list_संख्या(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    प्रत्यागम &t_list_संख्या;
}
    स्थिर type_T *
ret_list_string(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    प्रत्यागम &t_list_string;
}
    स्थिर type_T *
ret_list_dict_any(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    प्रत्यागम &t_list_dict_any;
}
    स्थिर type_T *
ret_list_वस्तुएँ(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    प्रत्यागम &t_list_list_any;
}
    स्थिर type_T *
ret_list_list_any(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    प्रत्यागम &t_list_list_any;
}
    स्थिर type_T *
ret_list_string_वस्तुएँ(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    प्रत्यागम &t_list_list_string;
}
    स्थिर type_T *
ret_list_regionpos(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    प्रत्यागम &t_list_list_list_संख्या;
}
    स्थिर type_T *
ret_tuple_any(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_tuple_any;
}
    स्थिर type_T *
ret_dict_any(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_dict_any;
}
    स्थिर type_T *
ret_job_info(पूर्णाङ्क argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    यदि (argcount == 0)
    {
	*decl_type = &t_list_any;
	प्रत्यागम &t_list_job;
    }
    प्रत्यागम &t_dict_any;
}
    स्थिर type_T *
ret_dict_संख्या(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_dict_संख्या;
}
    स्थिर type_T *
ret_dict_string(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_dict_string;
}
    स्थिर type_T *
ret_blob(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_blob;
}
    स्थिर type_T *
ret_func_any(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_func_any;
}
    स्थिर type_T *
ret_func_unknown(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_func_unknown;
}
    स्थिर type_T *
ret_channel(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_channel;
}
    स्थिर type_T *
ret_job(पूर्णाङ्क argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    प्रत्यागम &t_job;
}
    स्थिर type_T *
ret_first_arg(पूर्णाङ्क argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    यदि (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	प्रत्यागम argtypes[0].type_curr;
    }
    प्रत्यागम &t_void;
}
    स्थिर type_T *
ret_slice(पूर्णाङ्क argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    यदि (argcount > 0)
    {
	यदि (argtypes[0].type_decl != रिक्त)
	{
	    विकल्प (argtypes[0].type_decl->tt_type)
	    {
		पक्ष VAR_STRING: *decl_type = &t_string; भङ्ग;
		पक्ष VAR_BLOB: *decl_type = &t_blob; भङ्ग;
		पक्ष VAR_LIST: *decl_type = &t_list_any; भङ्ग;
		पक्ष VAR_TUPLE: *decl_type = &t_tuple_any; भङ्ग;
		स्वभाव: भङ्ग;
	    }
	}
	प्रत्यागम argtypes[0].type_curr;
    }
    प्रत्यागम &t_void;
}
    स्थिर type_T *
ret_copy(पूर्णाङ्क argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    यदि (argcount > 0)
    {
	यदि (argtypes[0].type_decl != रिक्त)
	{
	    यदि (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    अन्यथा यदि (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    अन्यथा
		*decl_type = argtypes[0].type_decl;
	}
	यदि (argtypes[0].type_curr != रिक्त)
	{
	    यदि (argtypes[0].type_curr->tt_type == VAR_LIST)
		प्रत्यागम &t_list_any;
	    अन्यथा यदि (argtypes[0].type_curr->tt_type == VAR_DICT)
		प्रत्यागम &t_dict_any;
	}
	प्रत्यागम argtypes[0].type_curr;
    }
    प्रत्यागम &t_void;
}
    स्थिर type_T *
ret_extend(पूर्णाङ्क argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    यदि (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	// if the second argument has a different current type then the current
	// type is "any"
	यदि (argcount > 1 && !equal_type(argtypes[0].type_curr,
						     argtypes[1].type_curr, 0))
	{
	    यदि (argtypes[0].type_curr->tt_type == VAR_LIST)
		प्रत्यागम &t_list_any;
	    यदि (argtypes[0].type_curr->tt_type == VAR_DICT)
		प्रत्यागम &t_dict_any;
	    यदि (argtypes[0].type_curr->tt_type == VAR_BLOB)
		प्रत्यागम &t_blob;
	}
	प्रत्यागम argtypes[0].type_curr;
    }
    प्रत्यागम &t_void;
}
    स्थिर type_T *
ret_repeat(पूर्णाङ्क argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    यदि (argcount == 0)
	प्रत्यागम &t_any;
    यदि (argtypes[0].type_curr == &t_संख्या)
	प्रत्यागम &t_string;
    प्रत्यागम argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    स्थिर type_T *
ret_first_cont(पूर्णाङ्क argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    यदि (argcount > 0)
    {
	यदि (argtypes[0].type_curr->tt_type == VAR_LIST)
	    प्रत्यागम &t_list_any;
	यदि (argtypes[0].type_curr->tt_type == VAR_DICT)
	    प्रत्यागम &t_dict_any;
	यदि (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    प्रत्यागम argtypes[0].type_curr;
    }
    प्रत्यागम &t_any;
}
// for get(): the type of the item when a missing item has the same type
    स्थिर type_T *
ret_get(पूर्णाङ्क argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    यदि (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*item = रिक्त;

	यदि (t->tt_type == VAR_BLOB)
	    item = &t_संख्या;
	अन्यथा यदि (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    item = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	यदि (item != रिक्त && (argcount == 3
			? equal_type(item, argtypes[2].type_curr, 0)
			: item->tt_type == VAR_संख्या))
	{
	    यदि (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    प्रत्यागम item;
	}
    }
    प्रत्यागम &t_any;
}
// for getline()
    स्थिर type_T *
ret_getline(पूर्णाङ्क argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    यदि (argcount == 1)
	प्रत्यागम &t_string;
    *decl_type = &t_list_any;
    प्रत्यागम &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    स्थिर type_T *
ret_string_or_any_1(पूर्णाङ्क argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    यदि (argcount < 1)
	प्रत्यागम &t_string;
    प्रत्यागम &t_any;
}
    स्थिर type_T *
ret_string_or_any_2(पूर्णाङ्क argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    यदि (argcount < 2)
	प्रत्यागम &t_string;
    प्रत्यागम &t_any;
}
    स्थिर type_T *
ret_string_or_any_3(पूर्णाङ्क argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    यदि (argcount < 3)
	प्रत्यागम &t_string;
    प्रत्यागम &t_any;
}
    स्थिर type_T *
ret_string_or_any_4(पूर्णाङ्क argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    यदि (argcount < 4)
	प्रत्यागम &t_string;
    प्रत्यागम &t_any;
}
// for abs()
    स्थिर type_T *
ret_abs(पूर्णाङ्क argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    यदि (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	प्रत्यागम &t_float;
    प्रत्यागम &t_संख्या;
}
// for sign_define()
    स्थिर type_T *
ret_sign_define(पूर्णाङ्क argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    यदि (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	प्रत्यागम &t_list_संख्या;
    प्रत्यागम &t_संख्या;
}
// for values(): list of member of first argument
    स्थिर type_T *
ret_list_member(पूर्णाङ्क argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    यदि (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	यदि (current_type_gap != रिक्त
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	अन्यथा
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	यदि (current_type_gap != रिक्त
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    प्रत्यागम get_list_type(t->tt_member, current_type_gap);
    }
    प्रत्यागम &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    स्थिर type_T *
ret_list_or_dict_0(पूर्णाङ्क argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    यदि (argcount > 0)
	प्रत्यागम &t_dict_any;
    *decl_type = &t_list_any;
    प्रत्यागम &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    स्थिर type_T *
ret_list_or_dict_1(पूर्णाङ्क argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    यदि (argcount > 1)
	प्रत्यागम &t_dict_any;
    *decl_type = &t_list_any;
    प्रत्यागम &t_list_dict_any;
}

    स्थिर type_T *
ret_argv(पूर्णाङ्क argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // argv() returns list of strings
    यदि (argcount == 0)
    {
	*decl_type = &t_list_any;
	प्रत्यागम &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    प्रत्यागम &t_any;
}

    स्थिर type_T *
ret_remove(पूर्णाङ्क argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    यदि (argcount > 0)
    {
	यदि (argtypes[0].type_curr->tt_type == VAR_LIST
		|| argtypes[0].type_curr->tt_type == VAR_DICT)
	{
	    यदि (argcount == 3)
	    {
		*decl_type = argtypes[0].type_decl;
		प्रत्यागम argtypes[0].type_curr;
	    }
	    यदि (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    प्रत्यागम argtypes[0].type_curr->tt_member;
	}
	यदि (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    प्रत्यागम &t_संख्या;
    }
    प्रत्यागम &t_any;
}

    स्थिर type_T *
ret_max_min(पूर्णाङ्क argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    यदि (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	यदि (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	अन्यथा
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	यदि (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	अन्यथा
	    t = &t_any;
	प्रत्यागम t;
    }
    प्रत्यागम &t_any;
}

    स्थिर type_T *
ret_getreg(पूर्णाङ्क argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    यदि (argcount == 3)
    {
	*decl_type = &t_list_any;
	प्रत्यागम &t_list_string;
    }
    प्रत्यागम &t_string;
}

    स्थिर type_T *
ret_virtcol(पूर्णाङ्क argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    यदि (argcount > 1)
    {
	*decl_type = &t_list_any;
	प्रत्यागम &t_list_संख्या;
    }
    प्रत्यागम &t_संख्या;
}

    स्थिर type_T *
ret_maparg(पूर्णाङ्क argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    // Assume that if the fourth argument is passed it's non-zero
    यदि (argcount == 4)
	प्रत्यागम &t_dict_any;
    प्रत्यागम &t_string;
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
प्रकारनिर्देश संरचना
{
    वर्ण	*f_name;	// function name
    वर्ण	f_min_argc;	// minimal number of arguments
    वर्ण	f_max_argc;	// maximal number of arguments
    वर्ण	f_argtype;	// for method: FEARG_ values; bits FE_
    argcheck_T	*f_argcheck;	// list of functions to check argument types;
				// use "arg_any" (not NULL) to accept an
				// argument of any type
    वर्ण	*f_argnames;	// names of the arguments as in the help,
				// separated by ", "; an argument without a
				// name is empty, NULL when there is none
    type_T	*(*f_retfunc)(पूर्णाङ्क argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    शून्य	(*f_func)(typval_T *args, typval_T *rvar);
				// implementation of function
} funcentry_T;

// Set f_max_argc to VARGS for varargs.
#define VARGS    अक्षर_उच्च

// values for f_argtype; zero means it cannot be used as a method
#define FEARG_1	    0x01    // base is the first argument
#define FEARG_2     0x02    // base is the second argument
#define FEARG_3     0x03    // base is the third argument
#define FEARG_4     0x04    // base is the fourth argument
#define FEARG_MASK  0x0F    // bits in f_argtype used as argument index
#define FE_X	    0x10    // builtin accepts a non-value (class, typealias)

#यदि defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#अन्यथा
# define MATH_FUNC(name) रिक्त
#endif
#ifdef FEAT_TIMERS
# define TIMER_FUNC(name) name
#अन्यथा
# define TIMER_FUNC(name) रिक्त
#endif
#ifdef FEAT_JOB_CHANNEL
# define JOB_FUNC(name) name
#अन्यथा
# define JOB_FUNC(name) रिक्त
#endif
#ifdef FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#अन्यथा
# define PROP_FUNC(name) रिक्त
#endif
#ifdef FEAT_SIGNS
# define SIGN_FUNC(name) name
#अन्यथा
# define SIGN_FUNC(name) रिक्त
#endif
#ifdef FEAT_SOUND
# define SOUND_FUNC(name) name
#अन्यथा
# define SOUND_FUNC(name) रिक्त
#endif
#ifdef FEAT_TERMINAL
# define TERM_FUNC(name) name
#अन्यथा
# define TERM_FUNC(name) रिक्त
#endif
#ifdef FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#अन्यथा
# define TABPANEL_FUNC(name) रिक्त
#endif
#ifdef FEAT_IMAGE
# define IMAGE_FUNC(name) name
#अन्यथा
# define IMAGE_FUNC(name) रिक्त
#endif

स्थिर ध्रुव funcentry_T global_functions[] =
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
    {"and",		2, 2, FEARG_1,	    arg2_संख्या,
			"expr1, expr2",
			ret_संख्या,	    f_and},
    {"append",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_संख्या_bool,    f_append},
    {"appendbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_संख्या_bool,    f_appendbufline},
    {"argc",		0, 1, 0,	    arg1_संख्या,
			"winid",
			ret_संख्या,	    f_argc},
    {"argidx",		0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_argidx},
    {"arglistid",	0, 2, 0,	    arg2_संख्या,
			"winnr, tabnr",
			ret_संख्या,	    f_arglistid},
    {"argv",		0, 2, 0,	    arg2_संख्या,
			"nr, winid",
			ret_argv,	    f_argv},
    {"asin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_asin},
    {"assert_beeps",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_संख्या_bool,    f_assert_beeps},
    {"assert_equal",	2, 3, FEARG_2,	    रिक्त,
			"expected, actual, msg",
			ret_संख्या_bool,    f_assert_equal},
    {"assert_equalfile", 2, 3, FEARG_1,	    arg3_string,
			"fname_one, fname_two, msg",
			ret_संख्या_bool,    f_assert_equalfile},
    {"assert_exception", 1, 2, 0,	    arg2_string,
			"error, msg",
			ret_संख्या_bool,    f_assert_exception},
    {"assert_fails",	1, 5, FEARG_1,	    arg15_assert_fails,
			"cmd, error, msg, lnum, context",
			ret_संख्या_bool,    f_assert_fails},
    {"assert_false",	1, 2, FEARG_1,	    रिक्त,
			"actual, msg",
			ret_संख्या_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_संख्या_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_संख्या_bool,    f_assert_match},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_संख्या_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    रिक्त,
			"expected, actual, msg",
			ret_संख्या_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_संख्या_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_संख्या_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    रिक्त,
			"actual, msg",
			ret_संख्या_bool,    f_assert_true},
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
    {"balloon_gettext",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,
#ifdef FEAT_BEVAL
	    f_balloon_gettext
#अन्यथा
	    रिक्त
#endif
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#ifdef FEAT_BEVAL
	    f_balloon_show
#अन्यथा
	    रिक्त
#endif
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#यदि defined(FEAT_BEVAL_TERM)
	    f_balloon_split
#अन्यथा
	    रिक्त
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
			ret_list_संख्या,    f_blob2list},
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
			ret_संख्या,	    f_bufadd},
    {"bufexists",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_संख्या_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_संख्या_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_संख्या,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_संख्या_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_संख्या_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, create",
			ret_संख्या,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_संख्या,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_संख्या,	    f_bufwinnr},
    {"byte2line",	1, 1, FEARG_1,	    arg1_संख्या,
			"byte",
			ret_संख्या,	    f_byte2line},
    {"byteidx",		2, 3, FEARG_1,	    arg3_string_संख्या_bool,
			"expr, nr, utf16",
			ret_संख्या,	    f_byteidx},
    {"byteidxcomp",	2, 3, FEARG_1,	    arg3_string_संख्या_bool,
			"expr, nr, utf16",
			ret_संख्या,	    f_byteidxcomp},
    {"call",		2, 3, FEARG_1,	    arg3_any_list_dict,
			"func, arglist, dict",
			ret_any,	    f_call},
    {"ceil",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ceil},
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_job,
			"handle",
			ret_संख्या_bool,    JOB_FUNC(f_ch_canread)},
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
			ret_संख्या,	    JOB_FUNC(f_ch_getbufnr)},
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
    {"changenr",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_changenr},
    {"char2nr",		1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_संख्या,	    f_char2nr},
    {"charclass",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_संख्या,	    f_charclass},
    {"charcol",		1, 2, FEARG_1,	    arg2_string_or_list_संख्या,
			"expr, winid",
			ret_संख्या,	    f_charcol},
    {"charidx",		2, 4, FEARG_1,	    arg4_string_संख्या_bool_bool,
			"string, idx, countcc, utf16",
			ret_संख्या,	    f_charidx},
    {"chdir",		1, 2, FEARG_1,	    arg2_string,
			"dir, scope",
			ret_string,	    f_chdir},
    {"cindent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_संख्या,	    f_cindent},
    {"clearmatches",	0, 1, FEARG_1,	    arg1_संख्या,
			"win",
			ret_void,	    f_clearmatches},
    {"cmdcomplete_info",0, 0, 0,	    रिक्त,
			रिक्त,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_संख्या,
			"expr, winid",
			ret_संख्या,	    f_col},
    {"complete",	2, 2, FEARG_2,	    arg2_संख्या_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_संख्या,	    f_complete_add},
    {"complete_check",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_संख्या_string,
			"msg, choices, default, type",
			ret_संख्या,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    रिक्त,
			"expr",
			ret_copy,	    f_copy},
    {"cos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cos},
    {"cosh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cosh},
    {"count",		2, 4, FEARG_1,	    arg24_गिनती,
			"comp, expr, ic, start",
			ret_संख्या,	    f_गिनती},
    {"cscope_connection",0,3, 0,	    arg3_संख्या_string_string,
			"num, dbpath, prepend",
			ret_संख्या,	    f_cscope_connection},
    {"cursor",		1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_संख्या,	    f_cursor},
    {"debugbreak",	1, 1, FEARG_1,	    arg1_संख्या,
			"pid",
			ret_संख्या,
#ifdef MSWIN
	    f_debugbreak
#अन्यथा
	    रिक्त
#endif
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_संख्या_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_buffer_lnum_lnum,
			"buf, first, last",
			ret_संख्या_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या_bool,    f_did_filetype},
    {"diff",		2, 3, FEARG_1,	    arg3_diff,
			"fromlist, tolist, options",
			ret_string_or_any_3,  f_diff},
    {"diff_filler",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_संख्या,	    f_diff_filler},
    {"diff_hlID",	2, 2, FEARG_1,	    arg2_lnum_संख्या,
			"lnum, col",
			ret_संख्या,	    f_diff_hlID},
    {"digraph_get",	1, 1, FEARG_1,	    arg1_string,
			"chars",
			ret_string,	    f_digraph_get},
    {"digraph_getlist",	0, 1, FEARG_1,	    arg1_bool,
			"listall",
			ret_list_string_वस्तुएँ, f_digraph_getlist},
    {"digraph_set",	2, 2, FEARG_1,	    arg2_string,
			"chars, digraph",
			ret_bool,	f_digraph_set},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    रिक्त,
			"expr",
			ret_संख्या_bool,    f_empty},
    {"environ",		0, 0, 0,	    रिक्त,
			रिक्त,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    रिक्त,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या_bool,    f_eventhandler},
    {"executable",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_संख्या,	    f_executable},
    {"execute",		1, 2, FEARG_1,	    arg12_execute,
			"command, silent",
			ret_string,	    f_execute},
    {"exepath",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_exepath},
    {"exists",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_संख्या_bool,    f_exists},
    {"exists_compiled",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_संख्या_bool,    f_exists_compiled},
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
			ret_संख्या_bool,    f_filereadable},
    {"filecopy",	2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_संख्या_bool,    f_filecopy},
    {"filereadable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_संख्या_bool,    f_filereadable},
    {"filewritable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_संख्या,	    f_filewritable},
    {"filter",		2, 2, FEARG_1,	    arg2_filter,
			"expr1, expr2",
			ret_first_arg,	    f_filter},
    {"finddir",		1, 3, FEARG_1,	    arg3_string_string_संख्या,
			"name, path, count",
			ret_string_or_any_3,	    f_finddir},
    {"findfile",	1, 3, FEARG_1,	    arg3_string_string_संख्या,
			"name, path, count",
			ret_string_or_any_3,	    f_findfile},
    {"flatten",		1, 2, FEARG_1,	    arg2_list_any_संख्या,
			"list, maxdepth",
			ret_list_any,	    f_flatten},
    {"flattennew",	1, 2, FEARG_1,	    arg2_list_any_संख्या,
			"list, maxdepth",
			ret_list_any,	    f_flattennew},
    {"float2nr",	1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_संख्या,	    f_float2nr},
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
    {"foldclosed",	1, 2, FEARG_1,	    arg2_lnum_संख्या,
			"lnum, winid",
			ret_संख्या,	    f_foldclosed},
    {"foldclosedend",	1, 2, FEARG_1,	    arg2_lnum_संख्या,
			"lnum, winid",
			ret_संख्या,	    f_foldclosedend},
    {"foldlevel",	1, 2, FEARG_1,	    arg2_lnum_संख्या,
			"lnum, winid",
			ret_संख्या,	    f_foldlevel},
    {"foldtext",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_संख्या,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_first_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    रिक्त,
			रिक्त,
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
    {"getbgcolor",	0, 0, 0,	    रिक्त,
			रिक्त,
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
    {"getcellpixels",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_संख्या,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_getcmdwintype},
    {"getcompletion",	2, 3, FEARG_1,	    arg3_string_string_bool,
			"pat, type, filtered",
			ret_list_string,    f_getcompletion},
    {"getcompletiontype", 1, 1, FEARG_1,    arg1_string,
			"pat",
			ret_string,	    f_getcompletiontype},
    {"getcurpos",	0, 1, FEARG_1,	    arg1_संख्या,
			"winid",
			ret_list_संख्या,    f_getcurpos},
    {"getcursorcharpos", 0, 1, FEARG_1,	    arg1_संख्या,
			"winid",
			ret_list_संख्या,    f_getcursorcharpos},
    {"getcwd",		0, 2, FEARG_1,	    arg2_संख्या,
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
			ret_संख्या,	    f_getfsize},
    {"getftime",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_संख्या,	    f_getftime},
    {"getftype",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getftype},
    {"getimstatus",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या_bool,    f_getimstatus},
    {"getinfo",		2, 3, FEARG_2,	    arg3_string_string_dict,
			"kind, name, opts",
			ret_dict_any,	    f_getinfo},
    {"getjumplist",	0, 2, FEARG_1,	    arg2_संख्या,
			"winnr, tabnr",
			ret_list_any,	    f_getjumplist},
    {"getline",		1, 2, FEARG_1,	    arg2_lnum,
			"lnum, end",
			ret_getline,	    f_getline},
    {"getloclist",	1, 2, 0,	    arg2_संख्या_dict_any,
			"nr, what",
			ret_list_or_dict_1, f_getloclist},
    {"getmarklist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_संख्या,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_dict_संख्या,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_getpid},
    {"getpos",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_संख्या,    f_getpos},
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
    {"getstacktrace",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_list_dict_any,  f_getstacktrace},
    {"gettabinfo",	0, 1, FEARG_1,	    arg1_संख्या,
			"tabnr",
			ret_list_dict_any,  f_gettabinfo},
    {"gettabvar",	2, 3, FEARG_1,	    arg3_संख्या_string_any,
			"tabnr, varname, def",
			ret_any,	    f_gettabvar},
    {"gettabwinvar",	3, 4, FEARG_1,	    arg4_संख्या_संख्या_string_any,
			"tabnr, winnr, varname, def",
			ret_any,	    f_gettabwinvar},
    {"gettagstack",	0, 1, FEARG_1,	    arg1_संख्या,
			"winnr",
			ret_dict_any,	    f_gettagstack},
    {"gettext",		1, 2, FEARG_1,	    arg2_string,
			"text, package",
			ret_string,	    f_gettext},
    {"getwininfo",	0, 1, FEARG_1,	    arg1_संख्या,
			"winid",
			ret_list_dict_any,  f_getwininfo},
    {"getwinpos",	0, 1, FEARG_1,	    arg1_संख्या,
			"timeout",
			ret_list_संख्या,    f_getwinpos},
    {"getwinposx",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_getwinposy},
    {"getwinvar",	2, 3, FEARG_1,	    arg3_संख्या_string_any,
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
			ret_संख्या_bool,    f_has},
    {"has_key",		2, 2, FEARG_1,	    arg2_dict_any_string_or_nr,
			"dict, key",
			ret_संख्या_bool,    f_has_कुंजी},
    {"haslocaldir",	0, 2, FEARG_1,	    arg2_संख्या,
			"winnr, tabnr",
			ret_संख्या,	    f_haslocaldir},
    {"hasmapto",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"what, mode, abbr",
			ret_संख्या_bool,    f_hasmapto},
    {"highlightID",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_संख्या,	    f_hlID},
    {"highlight_exists",1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_संख्या_bool,    f_hlexists},
    {"histadd",		2, 2, FEARG_2,	    arg2_string,
			"history, item",
			ret_संख्या_bool,    f_histadd},
    {"histdel",		1, 2, FEARG_1,	    arg2_string_string_or_संख्या,
			"history, item",
			ret_संख्या_bool,    f_histdel},
    {"histget",		1, 2, FEARG_1,	    arg2_string_संख्या,
			"history, index",
			ret_string,	    f_histget},
    {"histnr",		1, 1, FEARG_1,	    arg1_string,
			"history",
			ret_संख्या,	    f_histnr},
    {"hlID",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_संख्या,	    f_hlID},
    {"hlexists",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_संख्या_bool,    f_hlexists},
    {"hlget",		0, 2, FEARG_1,	    arg2_string_bool,
			"name, resolve",
			ret_list_dict_any,  f_hlget},
    {"hlset",		1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_संख्या_bool,    f_hlset},
    {"hostname",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    रिक्त,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_संख्या,	    IMAGE_FUNC(f_image_add)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_संख्या,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_संख्या,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_संख्या,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_index,
			"object, expr, start, ic",
			ret_संख्या,	    f_index},
    {"indexof",		2, 3, FEARG_1,	    arg23_index,
			"object, expr, opts",
			ret_संख्या,	    f_indexof},
    {"input",		1, 3, FEARG_1,	    arg3_string,
			"prompt, text, completion",
			ret_string,	    f_निवेश},
    {"inputdialog",	1, 3, FEARG_1,	    arg3_string,
			"prompt, text, cancelreturn",
			ret_string,	    f_inputdialog},
    {"inputlist",	1, 1, FEARG_1,	    arg1_list_string,
			"textlist",
			ret_संख्या,	    f_inputlist},
    {"inputrestore",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_जोड़ो,
			"object, item, idx",
			ret_first_arg,	    f_जोड़ो},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_void,	    f_interrupt},
    {"invert",		1, 1, FEARG_1,	    arg1_संख्या,
			"expr",
			ret_संख्या,	    f_invert},
    {"isabsolutepath",	1, 1, FEARG_1,	    arg1_string,
			"path",
			ret_संख्या_bool,    f_isabsolutepath},
    {"isdirectory",	1, 1, FEARG_1,	    arg1_string,
			"directory",
			ret_संख्या_bool,    f_isdirectory},
    {"isinf",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_संख्या,	    MATH_FUNC(f_isinf)},
    {"islocked",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_संख्या_bool,    f_islocked},
    {"isnan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_संख्या_bool,    MATH_FUNC(f_isnan)},
    {"items",		1, 1, FEARG_1,	    arg1_list_tuple_dict_blob_or_string,
			"expr",
			ret_list_वस्तुएँ,	    f_वस्तुएँ},
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
    {"job_stop",	1, 2, FEARG_1,	    arg2_job_string_or_संख्या,
			"job, how",
			ret_संख्या_bool,    JOB_FUNC(f_job_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    रिक्त,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    रिक्त,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_कुंजियाँ},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    रिक्त,	// obsolete
			रिक्त,
			ret_संख्या,	    f_last_buffer_nr},
    {"len",		1, 1, FEARG_1,	    arg1_len,
			"expr",
			ret_संख्या,	    f_len},
    {"libcall",		3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_string,	    f_libcall},
    {"libcallnr",	3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_संख्या,	    f_libcallnr},
    {"line",		1, 2, FEARG_1,	    arg2_string_संख्या,
			"expr, winid",
			ret_संख्या,	    f_पंक्ति},
    {"line2byte",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_संख्या,	    f_line2byte},
    {"lispindent",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_संख्या,	    f_lispindent},
    {"list2blob",	1, 1, FEARG_1,	    arg1_list_संख्या,
			"list",
			ret_blob,	    f_list2blob},
    {"list2str",	1, 2, FEARG_1,	    arg2_list_संख्या_bool,
			"list, utf8",
			ret_string,	    f_list2str},
    {"list2tuple",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_tuple_any,	    f_list2tuple},
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_buffer_bool_or_dict,
			"callback, buf, options",
			ret_संख्या,	    f_listener_add},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_संख्या,
			"id",
			ret_संख्या_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_localtime},
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
#अन्यथा
		रिक्त
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
			ret_संख्या,	    f_match},
    {"matchadd",	2, 5, FEARG_1,	    arg25_matchadd,
			"group, pattern, priority, id, dict",
			ret_संख्या,	    f_matchadd},
    {"matchaddpos",	2, 5, FEARG_1,	    arg25_matchaddpos,
			"group, pos, priority, id, dict",
			ret_संख्या,	    f_matchaddpos},
    {"matcharg",	1, 1, FEARG_1,	    arg1_संख्या,
			"nr",
			ret_list_string,    f_matcharg},
    {"matchbufline",	4, 5, FEARG_1,	    arg45_matchbufline,
			"buf, pat, lnum, end, dict",
			ret_list_any,	    f_matchbufline},
    {"matchdelete",	1, 2, FEARG_1,	    arg2_संख्या,
			"id, win",
			ret_संख्या_bool,    f_matchdelete},
    {"matchend",	2, 4, FEARG_1,	    arg24_match_func,
			"expr, pat, start, count",
			ret_संख्या,	    f_matchend},
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
#अन्यथा
	    रिक्त
#endif
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_संख्या,
			"name, flags, prot",
			ret_संख्या_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_mode},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_MZSCHEME
	    f_mzeval
#अन्यथा
	    रिक्त
#endif
			},
    {"nextnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_संख्या,	    f_nextnonblank},
    {"ngettext",	3, 4, FEARG_3,	    arg4_string_string_संख्या_string,
			"single, plural, number, domain",
			ret_string,	    f_ngettext},
    {"nr2char",		1, 2, FEARG_1,	    arg2_संख्या_bool,
			"expr, utf8",
			ret_string,	    f_nr2char},
    {"or",		2, 2, FEARG_1,	    arg2_संख्या,
			"expr1, expr2",
			ret_संख्या,	    f_or},
    {"pathshorten",	1, 2, FEARG_1,	    arg2_string_संख्या,
			"path, len",
			ret_string,	    f_pathshorten},
    {"perleval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_PERL
	    f_perleval
#अन्यथा
	    रिक्त
#endif
			},
    {"popup_atcursor",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_संख्या,	    PROP_FUNC(f_popup_atcursor)},
    {"popup_beval",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_संख्या,	    PROP_FUNC(f_popup_beval)},
    {"popup_clear",	0, 1, 0,	    arg1_bool,
			"force",
			ret_void,	    PROP_FUNC(f_popup_clear)},
    {"popup_close",	1, 2, FEARG_1,	    arg2_संख्या_any,
			"id, result",
			ret_void,	    PROP_FUNC(f_popup_close)},
    {"popup_create",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_संख्या,	    PROP_FUNC(f_popup_create)},
    {"popup_dialog",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_संख्या,	    PROP_FUNC(f_popup_dialog)},
    {"popup_filter_menu", 2, 2, 0,	    arg2_संख्या_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_menu)},
    {"popup_filter_yesno", 2, 2, 0,	    arg2_संख्या_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_yesno)},
    {"popup_findecho",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    PROP_FUNC(f_popup_findpreview)},
    {"popup_getoptions", 1, 1, FEARG_1,	    arg1_संख्या,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getoptions)},
    {"popup_getpos",	1, 1, FEARG_1,	    arg1_संख्या,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getpos)},
    {"popup_hide",	1, 1, FEARG_1,	    arg1_संख्या,
			"id",
			ret_void,	    PROP_FUNC(f_popup_hide)},
    {"popup_list",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_list_संख्या,    PROP_FUNC(f_popup_list)},
    {"popup_locate",	2, 2, 0,	    arg2_संख्या,
			"row, col",
			ret_संख्या,	    PROP_FUNC(f_popup_locate)},
    {"popup_menu",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_संख्या,	    PROP_FUNC(f_popup_menu)},
    {"popup_move",	2, 2, FEARG_1,	    arg2_संख्या_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_move)},
    {"popup_notification", 2, 2, FEARG_1,   arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_संख्या,	    PROP_FUNC(f_popup_notification)},
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_संख्या_buffer,
			"id, buf",
			ret_संख्या_bool,    PROP_FUNC(f_popup_setbuf)},
    {"popup_setoptions", 2, 2, FEARG_1,	    arg2_संख्या_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_setoptions)},
    {"popup_settext",	2, 2, FEARG_1,	    arg2_संख्या_string_or_list,
			"id, text",
			ret_void,	    PROP_FUNC(f_popup_settext)},
    {"popup_show",	1, 1, FEARG_1,	    arg1_संख्या,
			"id",
			ret_संख्या,	    PROP_FUNC(f_popup_show)},
    {"pow",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"x, y",
			ret_float,	    f_pow},
    {"preinserted",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_संख्या,	    f_prevnonblank},
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
    {"prop_add",	3, 3, FEARG_1,	    arg3_संख्या_संख्या_dict,
			"lnum, col, props",
			ret_संख्या,	    PROP_FUNC(f_prop_add)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_add_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_संख्या_संख्या_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_find)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_संख्या_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_संख्या_संख्या,
			"props, lnum, lnum_end",
			ret_संख्या,	    PROP_FUNC(f_prop_remove)},
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
    {"pum_getpos",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_dict_संख्या,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON3
	    f_py3eval
#अन्यथा
	    रिक्त
#endif
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON
	    f_pyeval
#अन्यथा
	    रिक्त
#endif
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#यदि defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#अन्यथा
	    रिक्त
#endif
			},
    {"rand",		0, 1, FEARG_1,	    arg1_list_संख्या,
			"expr",
			ret_संख्या,	    f_rand},
    {"range",		1, 3, FEARG_1,	    arg3_संख्या,
			"expr, max, stride",
			ret_list_संख्या,    f_range},
    {"readblob",	1, 3, FEARG_1,	    arg3_string_संख्या_संख्या,
			"fname, offset, size",
			ret_blob,	    f_readblob},
    {"readdir",		1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_string,    f_readdir},
    {"readdirex",	1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_dict_any,  f_readdirex},
    {"readfile",	1, 3, FEARG_1,	    arg3_string_string_संख्या,
			"fname, type, max",
			ret_list_string,    f_readfile},
    {"redraw_listener_add", 1, 1, FEARG_1,  arg1_dict_any,
			"opts",
			ret_संख्या,	    f_redraw_listener_add},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_संख्या,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_reg_recording},
    {"reltime",		0, 2, FEARG_1,	    arg2_list_संख्या,
			"start, end",
			ret_list_संख्या,    f_reltime},
    {"reltimefloat",	1, 1, FEARG_1,	    arg1_list_संख्या,
			"time",
			ret_float,	    f_reltimefloat},
    {"reltimestr",	1, 1, FEARG_1,	    arg1_list_संख्या,
			"time",
			ret_string,	    f_reltimestr},
    {"remote_expr",	2, 4, FEARG_1,	    arg24_remote_व्यंजक,
			"server, string, idvar, timeout",
			ret_string,	    f_remote_व्यंजक},
    {"remote_foreground", 1, 1, FEARG_1,    arg1_string,
			"server",
			ret_void,	    f_remote_foreground},
    {"remote_peek",	1, 2, FEARG_1,	    arg2_string,
			"serverid, retvar",
			ret_संख्या,	    f_remote_peek},
    {"remote_read",	1, 2, FEARG_1,	    arg2_string_संख्या,
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
			ret_संख्या_bool,    f_rename},
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
#अन्यथा
	    रिक्त
#endif
			},
    {"screenattr",	2, 2, FEARG_1,	    arg2_संख्या,
			"row, col",
			ret_संख्या,	    f_screenattr},
    {"screenchar",	2, 2, FEARG_1,	    arg2_संख्या,
			"row, col",
			ret_संख्या,	    f_screenchar},
    {"screenchars",	2, 2, FEARG_1,	    arg2_संख्या,
			"row, col",
			ret_list_संख्या,    f_screenchars},
    {"screencol",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_संख्या,
			"winid, lnum, col",
			ret_dict_संख्या,    f_screenpos},
    {"screenrow",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_screenrow},
    {"screenstring",	2, 2, FEARG_1,	    arg2_संख्या,
			"row, col",
			ret_string,	    f_screenstring},
    {"search",		1, 5, FEARG_1,	    arg15_खोज,
			"pattern, flags, stopline, timeout, skip",
			ret_संख्या,	    f_खोज},
    {"searchcount",	0, 1, FEARG_1,	    arg1_dict_any,
			"options",
			ret_dict_संख्या,    f_searchcount},
    {"searchdecl",	1, 3, FEARG_1,	    arg3_string_bool_bool,
			"name, global, thisblock",
			ret_संख्या_bool,    f_searchdecl},
    {"searchpair",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_संख्या,	    f_searchpair},
    {"searchpairpos",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_list_संख्या,    f_searchpairpos},
    {"searchpos",	1, 5, FEARG_1,	    arg15_खोज,
			"pattern, flags, stopline, timeout, skip",
			ret_list_संख्या,    f_searchpos},
    {"server2client",	2, 2, FEARG_1,	    arg2_string,
			"clientid, string",
			ret_संख्या_bool,    f_server2client},
    {"serverlist",	0, 1, 0,	    arg1_dict_any,
			"dict",
			ret_string_or_any_1,	    f_serverlist},
    {"setbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_संख्या_bool,    f_setbufline},
    {"setbufvar",	3, 3, FEARG_3,	    arg3_buffer_string_any,
			"buf, varname, val",
			ret_void,	    f_setbufvar},
    {"setcellwidths",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_void,	    f_setcellwidths},
    {"setcharpos",	2, 2, FEARG_2,	    arg2_string_list_संख्या,
			"expr, list",
			ret_संख्या_bool,    f_setcharpos},
    {"setcharsearch",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_setcharsearch},
    {"setcmdline",	1, 2, FEARG_1,	    arg2_string_संख्या,
			"str, pos",
			ret_संख्या_bool,    f_setcmdline},
    {"setcmdpos",	1, 1, FEARG_1,	    arg1_संख्या,
			"pos",
			ret_संख्या_bool,    f_setcmdpos},
    {"setcursorcharpos", 1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_संख्या_bool,    f_setcursorcharpos},
    {"setenv",		2, 2, FEARG_2,	    arg2_string_any,
			"name, val",
			ret_void,	    f_setenv},
    {"setfperm",	2, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_संख्या_bool,    f_setfperm},
    {"setline",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_संख्या_bool,    f_setline},
    {"setloclist",	2, 4, FEARG_2,	    arg24_setloclist,
			"nr, list, action, what",
			ret_संख्या_bool,    f_setloclist},
    {"setmatches",	1, 2, FEARG_1,	    arg2_list_any_संख्या,
			"list, win",
			ret_संख्या_bool,    f_setmatches},
    {"setpos",		2, 2, FEARG_2,	    arg2_string_list_संख्या,
			"expr, list",
			ret_संख्या_bool,    f_setpos},
    {"setqflist",	1, 3, FEARG_1,	    arg13_setqflist,
			"list, action, what",
			ret_संख्या_bool,    f_setqflist},
    {"setreg",		2, 3, FEARG_2,	    arg3_string_any_string,
			"regname, value, options",
			ret_संख्या_bool,    f_setreg},
    {"settabvar",	3, 3, FEARG_3,	    arg3_संख्या_string_any,
			"tabnr, varname, val",
			ret_void,	    f_settabvar},
    {"settabwinvar",	4, 4, FEARG_4,	    arg4_संख्या_संख्या_string_any,
			"tabnr, winnr, varname, val",
			ret_void,	    f_settabwinvar},
    {"settagstack",	2, 3, FEARG_2,	    arg23_settagstack,
			"nr, dict, action",
			ret_संख्या_bool,    f_settagstack},
    {"setwinvar",	3, 3, FEARG_3,	    arg3_संख्या_string_any,
			"winnr, varname, val",
			ret_void,	    f_setwinvar},
    {"sha256",		1, 1, FEARG_1,	    arg1_string_or_blob,
			"expr",
			ret_string,
#ifdef FEAT_CRYPT
	    f_sha256
#अन्यथा
	    रिक्त
#endif
			},
    {"shellescape",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, special",
			ret_string,	    f_shellescape},
    {"shiftwidth",	0, 1, FEARG_1,	    arg1_संख्या,
			"col",
			ret_संख्या,	    f_shiftwidth},
    {"sign_define",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"name, dict",
			ret_sign_define,	    SIGN_FUNC(f_sign_define)},
    {"sign_getdefined",	0, 1, FEARG_1,	    arg1_string,
			"name",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getdefined)},
    {"sign_getplaced",	0, 2, FEARG_1,	    arg02_sign_getplaced,
			"buf, dict",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getplaced)},
    {"sign_jump",	3, 3, FEARG_1,	    arg3_संख्या_string_buffer,
			"id, group, buf",
			ret_संख्या,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_place,
			"id, group, name, buf, dict",
			ret_संख्या,	    SIGN_FUNC(f_sign_place)},
    {"sign_placelist",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_संख्या,    SIGN_FUNC(f_sign_placelist)},
    {"sign_undefine",	0, 1, FEARG_1,	    arg1_string_or_list_string,
			"name",
			ret_संख्या_bool,    SIGN_FUNC(f_sign_undefine)},
    {"sign_unplace",	1, 2, FEARG_1,	    arg2_string_dict,
			"group, dict",
			ret_संख्या_bool,    SIGN_FUNC(f_sign_unplace)},
    {"sign_unplacelist", 1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_संख्या,    SIGN_FUNC(f_sign_unplacelist)},
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
    {"sound_clear",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_void,	    SOUND_FUNC(f_sound_clear)},
    {"sound_playevent",	1, 2, FEARG_1,	    arg2_string_any,
			"name, callback",
			ret_संख्या,	    SOUND_FUNC(f_sound_playevent)},
    {"sound_playfile",	1, 2, FEARG_1,	    arg2_string_any,
			"path, callback",
			ret_संख्या,	    SOUND_FUNC(f_sound_playfile)},
    {"sound_stop",	1, 1, FEARG_1,	    arg1_संख्या,
			"id",
			ret_void,	    SOUND_FUNC(f_sound_stop)},
    {"soundfold",	1, 1, FEARG_1,	    arg1_string,
			"word",
			ret_string,	    f_soundfold},
    {"spellbadword",	0, 1, FEARG_1,	    arg1_string,
			"sentence",
			ret_list_string,    f_spellbadword},
    {"spellsuggest",	1, 3, FEARG_1,	    arg3_string_संख्या_bool,
			"word, max, capital",
			ret_list_string,    f_spellsuggest},
    {"split",		1, 3, FEARG_1,	    arg3_string_string_bool,
			"string, pattern, keepempty",
			ret_list_string,    f_split},
    {"sqrt",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sqrt},
    {"srand",		0, 1, FEARG_1,	    arg1_संख्या,
			"expr",
			ret_list_संख्या,    f_srand},
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
			ret_list_संख्या,    f_str2list},
    {"str2nr",		1, 3, FEARG_1,	    arg3_string_संख्या_bool,
			"string, base, quoted",
			ret_संख्या,	    f_str2nr},
    {"strcharlen",	1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_संख्या,	    f_strcharlen},
    {"strcharpart",	2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, skipcc",
			ret_string,	    f_strcharpart},
    {"strchars",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, skipcc",
			ret_संख्या,	    f_strchars},
    {"strdisplaywidth",	1, 2, FEARG_1,	    arg2_string_संख्या,
			"string, col",
			ret_संख्या,	    f_strdisplaywidth},
    {"strftime",	1, 2, FEARG_1,	    arg2_string_संख्या,
			"format, time",
			ret_string,
#ifdef HAVE_STRFTIME
	    f_strftime
#अन्यथा
	    रिक्त
#endif
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_संख्या,
			"str, index",
			ret_संख्या,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_संख्या,
			"haystack, needle, start",
			ret_संख्या,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, रिक्त,
			"expr",
			ret_string,	    f_string},
    {"strlen",		1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_संख्या,	    f_strlen},
    {"strpart",		2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, chars",
			ret_string,	    f_strpart},
    {"strptime",	2, 2, FEARG_1,	    arg2_string,
			"format, timestring",
			ret_संख्या,
#ifdef HAVE_STRPTIME
	    f_strptime
#अन्यथा
	    रिक्त
#endif
			},
    {"strridx",		2, 3, FEARG_1,	    arg3_string_string_संख्या,
			"haystack, needle, start",
			ret_संख्या,	    f_strridx},
    {"strtrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_strtrans},
    {"strutf16len",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, countcc",
			ret_संख्या,	    f_strutf16len},
    {"strwidth",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_संख्या,	    f_strwidth},
    {"submatch",	1, 2, FEARG_1,	    arg2_संख्या_bool,
			"nr, list",
			ret_string_or_any_2,	    f_submatch},
    {"substitute",	4, 4, FEARG_1,	    arg4_string_string_any_string,
			"string, pat, sub, flags",
			ret_string,	    f_substitute},
    {"swapfilelist",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_swapname},
    {"synID",		3, 3, 0,	    arg3_lnum_संख्या_bool,
			"lnum, col, trans",
			ret_संख्या,	    f_synID},
    {"synIDattr",	2, 3, FEARG_1,	    arg3_संख्या_string_string,
			"synID, what, mode",
			ret_string,	    f_synIDattr},
    {"synIDtrans",	1, 1, FEARG_1,	    arg1_संख्या,
			"synID",
			ret_संख्या,	    f_synIDtrans},
    {"synconcealed",	2, 2, 0,	    arg2_lnum_संख्या,
			"lnum, col",
			ret_list_any,	    f_synconcealed},
    {"synstack",	2, 2, 0,	    arg2_lnum_संख्या,
			"lnum, col",
			ret_list_संख्या,    f_synstack},
    {"system",		1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_string,	    f_system},
    {"systemlist",	1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_list_string,    f_systemlist},
    {"tabpagebuflist",	0, 1, FEARG_1,	    arg1_संख्या,
			"arg",
			ret_list_संख्या,    f_tabpagebuflist},
    {"tabpagenr",	0, 1, 0,	    arg1_string,
			"arg",
			ret_संख्या,	    f_tabpagenr},
    {"tabpagewinnr",	1, 2, FEARG_1,	    arg2_संख्या_string,
			"tabarg, arg",
			ret_संख्या,	    f_tabpagewinnr},
    {"tabpanel_getinfo", 0, 0, 0,	    रिक्त,
			रिक्त,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_संख्या_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    रिक्त,
			रिक्त,
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
    {"tempname",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_संख्या,	    TERM_FUNC(f_term_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_संख्या,	    TERM_FUNC(f_term_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_buffer_string_dict,
			"buf, filename, options",
			ret_void,	    TERM_FUNC(f_term_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_buffer,
			"buf",
			ret_संख्या,	    TERM_FUNC(f_term_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_buffer,
			"buf",
			ret_list_string,
#यदि defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_getansicolors
#अन्यथा
	    रिक्त
#endif
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_संख्या_string,
			"attr, what",
			ret_संख्या,	    TERM_FUNC(f_term_getattr)},
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
			ret_संख्या,	    TERM_FUNC(f_term_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_संख्या,    TERM_FUNC(f_term_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, input",
			ret_string,	    TERM_FUNC(f_term_gettty)},
    {"term_list",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_list_संख्या,    TERM_FUNC(f_term_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_buffer_lnum,
			"buf, row",
			ret_list_dict_any,  TERM_FUNC(f_term_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_buffer_string,
			"buf, keys",
			ret_void,	    TERM_FUNC(f_term_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_buffer_list_any,
			"buf, colors",
			ret_void,
#यदि defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_setansicolors
#अन्यथा
	    रिक्त
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
    {"term_setsize",	3, 3, FEARG_1,	    arg3_buffer_संख्या_संख्या,
			"buf, rows, cols",
			ret_void,	    TERM_FUNC(f_term_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_संख्या,	    TERM_FUNC(f_term_start)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_buffer_संख्या,
			"buf, time",
			ret_void,	    TERM_FUNC(f_term_wait)},
    {"terminalprops",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_संख्या,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    रिक्त,
			रिक्त,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   रिक्त,
			रिक्त,
			ret_void,	    f_test_garbagecollect_soon},
    {"test_getvalue",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_संख्या,	    f_test_getvalue},
    {"test_gui_event",	2, 2, FEARG_1,	    arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_gui_event},
    {"test_ignore_error", 1, 1, FEARG_1,    arg1_string,
			"expr",
			ret_void,	    f_test_ignore_error},
    {"test_mswin_event", 2, 2, FEARG_1,     arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_mswin_event},
    {"test_null_blob",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    रिक्त,
			रिक्त,
			ret_channel,	    JOB_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    रिक्त,
			रिक्त,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_job,	    JOB_FUNC(f_test_null_job)},
    {"test_null_list",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    रिक्त,
			रिक्त,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_set},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_संख्या,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, रिक्त,
			"expr",
			ret_संख्या,	    f_test_refcount},
    {"test_setmouse",	2, 2, 0,	    arg2_संख्या,
			"row, col",
			ret_void,	    f_test_setmouse},
    {"test_settime",	1, 1, FEARG_1,	    arg1_संख्या,
			"expr",
			ret_void,	    f_test_settime},
    {"test_srand_seed",	0, 1, FEARG_1,	    arg1_संख्या,
			"seed",
			ret_void,	    f_test_srand_seed},
    {"test_unknown",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_संख्या,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_संख्या_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_संख्या_any_dict,
			"time, callback, options",
			ret_संख्या,	    TIMER_FUNC(f_timer_start)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_संख्या,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    रिक्त,
			रिक्त,
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
    {"trim",		1, 3, FEARG_1,	    arg3_string_string_संख्या,
			"text, mask, dir",
			ret_string,	    f_trim},
    {"trunc",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_trunc},
    {"tuple2list",	1, 1, FEARG_1,	    arg1_tuple_any,
			"tuple",
			ret_list_any,	    f_tuple2list},
    {"type",		1, 1, FEARG_1|FE_X, रिक्त,
			"expr",
			ret_संख्या,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, रिक्त,
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
    {"utf16idx",	2, 4, FEARG_1,	    arg4_string_संख्या_bool_bool,
			"string, idx, countcc, charidx",
			ret_संख्या,	    f_utf16idx},
    {"values",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_member,    f_मूल्यावली},
    {"virtcol",		1, 3, FEARG_1,	    arg3_string_or_list_bool_संख्या,
			"expr, list, winid",
			ret_virtcol,	    f_virtcol},
    {"virtcol2col",	3, 3, FEARG_1,	    arg3_संख्या,
			"winid, lnum, col",
			ret_संख्या,	    f_virtcol2col},
    {"visualmode",	0, 1, 0,	    arg1_bool,
			"expr",
			ret_string,	    f_visualmode},
    {"wildmenumode",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_void,	    f_wildtrigger},
    {"win_execute",	2, 3, FEARG_2,	    arg23_win_execute,
			"id, command, silent",
			ret_string,	    f_win_execute},
    {"win_findbuf",	1, 1, FEARG_1,	    arg1_संख्या,
			"bufnr",
			ret_list_संख्या,    f_win_findbuf},
    {"win_getid",	0, 2, FEARG_1,	    arg2_संख्या,
			"win, tab",
			ret_संख्या,	    f_win_getid},
    {"win_gettype",	0, 1, FEARG_1,	    arg1_संख्या,
			"nr",
			ret_string,	    f_win_gettype},
    {"win_gotoid",	1, 1, FEARG_1,	    arg1_संख्या,
			"expr",
			ret_संख्या_bool,    f_win_gotoid},
    {"win_id2tabwin",	1, 1, FEARG_1,	    arg1_संख्या,
			"expr",
			ret_list_संख्या,    f_win_id2tabwin},
    {"win_id2win",	1, 1, FEARG_1,	    arg1_संख्या,
			"expr",
			ret_संख्या,	    f_win_id2win},
    {"win_move_separator", 2, 2, FEARG_1,   arg2_संख्या,
			"nr, offset",
			ret_संख्या_bool,    f_win_move_separator},
    {"win_move_statusline", 2, 2, FEARG_1,  arg2_संख्या,
			"nr, offset",
			ret_संख्या_bool,    f_win_move_statusline},
    {"win_screenpos",	1, 1, FEARG_1,	    arg1_संख्या,
			"nr",
			ret_list_संख्या,    f_win_screenpos},
    {"win_splitmove",   2, 3, FEARG_1,	    arg3_संख्या_संख्या_dict,
			"nr, target, options",
			ret_संख्या_bool,    f_win_splitmove},
    {"winbufnr",	1, 1, FEARG_1,	    arg1_संख्या,
			"nr",
			ret_संख्या,	    f_winbufnr},
    {"wincol",		0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_संख्या,
			"nr",
			ret_संख्या,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_संख्या,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    रिक्त,
			रिक्त,
			ret_संख्या,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_संख्या,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_dict_संख्या,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_संख्या,
			"nr",
			ret_संख्या,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    रिक्त,
			रिक्त,
			ret_dict_संख्या,    f_wordcount},
    {"writefile",	2, 3, FEARG_1,	    arg23_writefile,
			"object, fname, flags",
			ret_संख्या_bool,    f_writefile},
    {"xor",		2, 2, FEARG_1,	    arg2_संख्या,
			"expr1, expr2",
			ret_संख्या,	    f_xor},
};

/*
 * Return true if specified function allows a type as an argument.
 */
    स्थिर पूर्णाङ्क
func_allows_type(पूर्णाङ्क idx)
{
    प्रत्यागम (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, पूर्णाङ्क idx)
{
    स्थिर पूर्णाङ्क	intidx = -1;
    char_u	*name;

    यदि (idx == 0)
	intidx = -1;
    यदि (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	यदि (name != रिक्त)
	{
	    यदि (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_pattern, 2) == 0)
		प्रत्यागम cat_prefix_varname('g', name);
	    प्रत्यागम name;
	}
    }
    यदि (++intidx < (पूर्णाङ्क)ARRAY_LENGTH(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	यदि (global_functions[intidx].f_func == रिक्त)
	    प्रत्यागम (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	यदि (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	प्रत्यागम IObuff;
    }

    प्रत्यागम रिक्त;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_व्यंजक_name(expand_T *xp, पूर्णाङ्क idx)
{
    स्थिर पूर्णाङ्क	intidx = -1;
    char_u	*name;

    यदि (idx == 0)
	intidx = -1;
    यदि (intidx < 0)
    {
	name = get_function_name(xp, idx);
	यदि (name != रिक्त)
	    प्रत्यागम name;
    }
    प्रत्यागम get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    स्थिर पूर्णाङ्क
find_internal_func_opt(char_u *name, पूर्णाङ्क implemented)
{
    पूर्णाङ्क		first = 0;
    पूर्णाङ्क		last;
    पूर्णाङ्क		cmp;
    पूर्णाङ्क		x;

    last = (पूर्णाङ्क)ARRAY_LENGTH(global_functions) - 1;

    // Find the function name in the table. Binary search.
    यावत् (first <= last)
    {
	x = first + ((अचिन्हित)(last - first) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	यदि (cmp < 0)
	    last = x - 1;
	अन्यथा यदि (cmp > 0)
	    first = x + 1;
	अन्यथा यदि (implemented && global_functions[x].f_func == रिक्त)
	    भङ्ग;
	अन्यथा
	    प्रत्यागम x;
    }
    प्रत्यागम -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    पूर्णाङ्क
find_internal_func(char_u *name)
{
    प्रत्यागम find_internal_func_opt(name, TRUE);
}

    पूर्णाङ्क
has_internal_func(char_u *name)
{
    प्रत्यागम find_internal_func_opt(name, TRUE) >= 0;
}

    स्थिर पूर्णाङ्क
has_internal_func_name(char_u *name)
{
    प्रत्यागम find_internal_func_opt(name, FALSE) >= 0;
}

    वर्ण *
internal_func_name(पूर्णाङ्क idx)
{
    प्रत्यागम global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    पूर्णाङ्क
internal_func_check_arg_types(
	type2_T	*types,
	पूर्णाङ्क	idx,
	पूर्णाङ्क	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    यदि (!(func_allows_type(idx)))
    {
	प्रति (पूर्णाङ्क i = 0; i < argcount; ++i)
	    यदि (check_type_is_मूल्य(types[i].type_curr) == FAIL)
		प्रत्यागम FAIL;
    }

    ध्रुव argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    यदि (argchecks == रिक्त)
	प्रत्यागम OK;

    argcontext_T context;

    context.arg_गिनती = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    प्रति (पूर्णाङ्क i = 0; i < argcount && argchecks[i] != रिक्त; ++i)
    {
	context.arg_idx = i;
	यदि (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    शून्य
internal_func_get_argcount(पूर्णाङ्क idx, पूर्णाङ्क *argcount, पूर्णाङ्क *min_argcount)
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
	पूर्णाङ्क	    idx,
	पूर्णाङ्क	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = रिक्त;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    यदि (*decl_type == रिक्त)
	*decl_type = ret;
    current_type_gap = रिक्त;
    प्रत्यागम ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    पूर्णाङ्क
internal_func_is_map(पूर्णाङ्क idx)
{
    प्रत्यागम global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    पूर्णाङ्क
check_internal_func(पूर्णाङ्क idx, पूर्णाङ्क argcount)
{
    funcerror_T	    res;
    वर्ण	    *name;

    यदि (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    अन्यथा यदि (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    अन्यथा
	प्रत्यागम global_functions[idx].f_argtype & FEARG_MASK;

    name = internal_func_name(idx);
    यदि (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    अन्यथा
	semsg(_(e_not_enough_arguments_for_function_str), name);
    प्रत्यागम -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    स्थिर पूर्णाङ्क
check_args_for_type(पूर्णाङ्क idx, पूर्णाङ्क argcount, typval_T *argvars)
{
    यदि (!func_allows_type(idx))
    {
	प्रति (पूर्णाङ्क i = 0; i < argcount; ++i)
	    यदि (check_typval_is_मूल्य(&argvars[i]) == FAIL)
		प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

    funcerror_T
call_internal_func(
	char_u	    *name,
	पूर्णाङ्क	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    पूर्णाङ्क i;

    i = find_internal_func(name);
    यदि (i < 0)
	प्रत्यागम FCERR_UNKNOWN;
    यदि (argcount < global_functions[i].f_min_argc)
	प्रत्यागम FCERR_TOOFEW;
    यदि (argcount > global_functions[i].f_max_argc)
	प्रत्यागम FCERR_TOOMANY;
    यदि (check_args_for_type(i, argcount, argvars) == FAIL)
	प्रत्यागम FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    यदि (in_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    प्रत्यागम FCERR_NONE;
}

    शून्य
call_internal_func_by_idx(
	पूर्णाङ्क	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    यदि (in_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_internal_method(
	char_u	    *name,
	पूर्णाङ्क	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    पूर्णाङ्क		fi;
    typval_T	argv[MAX_FUNC_ARGS + 1];

    fi = find_internal_func(name);
    यदि (fi < 0)
	प्रत्यागम FCERR_UNKNOWN;
    यदि ((global_functions[fi].f_argtype & FEARG_MASK) == 0)
	प्रत्यागम FCERR_NOTMETHOD;
    यदि (argcount + 1 < global_functions[fi].f_min_argc)
	प्रत्यागम FCERR_TOOFEW;
    यदि (argcount + 1 > global_functions[fi].f_max_argc)
	प्रत्यागम FCERR_TOOMANY;
    यदि (check_args_for_type(fi, argcount, argvars) == FAIL)
	प्रत्यागम FCERR_OTHER;

    यदि ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_2)
    {
	यदि (argcount < 1)
	    प्रत्यागम FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	प्रति (पूर्णाङ्क i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    अन्यथा यदि ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_3)
    {
	यदि (argcount < 2)
	    प्रत्यागम FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	प्रति (पूर्णाङ्क i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    अन्यथा यदि ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_4)
    {
	यदि (argcount < 3)
	    प्रत्यागम FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	प्रति (पूर्णाङ्क i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    अन्यथा
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	प्रति (पूर्णाङ्क i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    यदि (check_args_for_type(fi, argcount + 1, argv) == FAIL)
	प्रत्यागम FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    प्रत्यागम FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    पूर्णाङ्क
non_zero_arg(typval_T *argvars)
{
    प्रत्यागम ((argvars[0].v_type == VAR_संख्या
		&& argvars[0].vval.v_संख्या != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_संख्या == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != रिक्त
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    स्थिर शून्य
f_and(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    rettv->vval.v_संख्या = tv_get_संख्या_chk(&argvars[0], रिक्त)
					& tv_get_संख्या_chk(&argvars[1], रिक्त);
}

/*
 * "balloon_show()" function
 */
#ifdef FEAT_BEVAL
    स्थिर शून्य
f_balloon_gettext(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    यदि (balloonEval == रिक्त)
	प्रत्यागम;

    यदि (balloonEval->संदेश == रिक्त)
	rettv->vval.v_string = रिक्त;
    अन्यथा
	rettv->vval.v_string = vim_strsave(balloonEval->संदेश);
}

    स्थिर शून्य
f_balloon_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    यदि (balloonEval == रिक्त)
	प्रत्यागम;

    यदि (in_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, रिक्त,
		l == रिक्त || l->lv_len == 0 ? रिक्त : l);
    }
    अन्यथा
    {
	char_u *mesg;

	यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    प्रत्यागम;

	mesg = tv_get_string_chk(&argvars[0]);
	यदि (mesg != रिक्त)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? रिक्त : mesg, रिक्त);
    }
}

# यदि defined(FEAT_BEVAL_TERM)
    स्थिर शून्य
f_balloon_split(typval_T *argvars, typval_T *rettv UNUSED)
{
    यदि (rettv_list_alloc(rettv) != OK)
	प्रत्यागम;

    char_u *संदेश;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;
    संदेश = tv_get_string_chk(&argvars[0]);
    यदि (संदेश != रिक्त)
    {
	pumitem_T	*array;
	पूर्णाङ्क		size = split_message(संदेश, &array);

	// Skip the first and last item, they are always empty.
	प्रति (पूर्णाङ्क i = 1; i < size - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_text, -1);
	यावत् (size > 0)
	    vim_free(array[--size].pum_text);
	vim_free(array);
    }
}
# endif
#endif

/*
 * "base64_decode(string)" function
 */
    स्थिर शून्य
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    यदि (check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (rettv_blob_alloc(rettv) == FAIL)
	प्रत्यागम;

    char_u *str = tv_get_string_chk(&argvars[0]);
    यदि (str != रिक्त)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    स्थिर शून्य
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    यदि (check_for_blob_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    blob_T *blob = argvars->vval.v_blob;
    यदि (blob != रिक्त)
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
    यदि (buf == रिक्त)
	semsg(_(e_invalid_buffer_name_str), tv_get_string(arg));
    प्रत्यागम buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    स्थिर शून्य
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_संख्या = VVAL_TRUE;

    यदि (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	प्रत्यागम;

    यदि (म_भेद((ध्रुव वर्ण *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    अन्यथा
    {
	यदि (bindtextdomain((ध्रुव वर्ण *)argvars[0].vval.v_string, (ध्रुव वर्ण *)argvars[1].vval.v_string) == रिक्त)
	{
	    do_outofmem_संदेश((दीर्घ)0);
	    rettv->vval.v_संख्या = VVAL_FALSE;
	}
    }

    प्रत्यागम;
}

/*
 * "byte2line(byte)" function
 */
    स्थिर शून्य
f_byte2line(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_संख्या = -1;
#अन्यथा
    दीर्घ	boff = 0;

    यदि (in_vim9script() && check_for_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    boff = tv_get_संख्या(&argvars[0]) - 1;  // boff gets -1 on type error
    यदि (boff < 0)
	rettv->vval.v_संख्या = -1;
    अन्यथा
	rettv->vval.v_संख्या = ml_find_पंक्ति_or_offset(curbuf,
							  (linenr_T)0, &boff);
#endif
}

/*
 * "call(func, arglist [, dict])" function
 */
    स्थिर शून्य
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = रिक्त;
    dict_T	*selfdict = रिक्त;
    char_u	*tofree = रिक्त;

    यदि (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    यदि (check_for_list_arg(argvars, 1) == FAIL)
	प्रत्यागम;
    यदि (argvars[1].vval.v_list == रिक्त)
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    अन्यथा यदि (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    अन्यथा
	func = tv_get_string(&argvars[0]);
    यदि (func == रिक्त || *func == NUL)
	प्रत्यागम;		// type error, empty name or null function

    यदि (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, रिक्त, FALSE, TFN_INT|TFN_QUIET);
	यदि (tofree == रिक्त)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    प्रत्यागम;
	}
	यदि (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_args(tofree, STRLEN(tofree),
									&p);
	    यदि (s != रिक्त)
	    {
		vim_free(tofree);
		tofree = s;
	    }
	}
	func = tofree;
    }

    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	यदि (check_for_dict_arg(argvars, 2) == FAIL)
	    गच्छ done;

	selfdict = argvars[2].vval.v_dict;
    }

    (शून्य)func_call(func, &argvars[1], partial, selfdict, rettv);

done:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    स्थिर शून्य
f_changenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_संख्या = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    स्थिर शून्य
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    यदि (has_mbyte)
    {
	पूर्णाङ्क	utf8 = 0;

	यदि (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (पूर्णाङ्क)tv_get_bool_chk(&argvars[1], रिक्त);

	यदि (utf8)
	    rettv->vval.v_संख्या = utf_ptr2char(tv_get_string(&argvars[0]));
	अन्यथा
	    rettv->vval.v_संख्या = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    अन्यथा
	rettv->vval.v_संख्या = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    स्थिर शून्य
get_col(typval_T *argvars, typval_T *rettv, पूर्णाङ्क charcol)
{
    colnr_T	col = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    पूर्णाङ्क		winchanged = FALSE;

    यदि (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_संख्या_arg(argvars, 1) == FAIL)
	प्रत्यागम;

    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((पूर्णाङ्क)tv_get_संख्या(&argvars[1]), &tp);
	यदि (wp == रिक्त || tp == रिक्त)
	    प्रत्यागम;

	यदि (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    प्रत्यागम;

	check_cursor();
	winchanged = TRUE;
    }

    पूर्णाङ्क fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    यदि (fp != रिक्त && fnum == curbuf->b_fnum)
    {
	यदि (fp->col == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    यदि (fp->lnum <= curbuf->b_ml.ml_पंक्ति_गिनती)
		col = ml_get_len(fp->lnum) + 1;
	    अन्यथा
		col = MAXCOL;
	}
	अन्यथा
	{
	    col = fp->col + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    यदि (virtual_active() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		यदि (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    पूर्णाङ्क		l;

		    यदि (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			col += l;
		}
	    }
	}
    }
    rettv->vval.v_संख्या = col;

    यदि (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    स्थिर शून्य
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, TRUE);
}

    win_T *
get_optional_window(typval_T *argvars, पूर्णाङ्क idx)
{
    win_T   *win = curwin;

    यदि (argvars[idx].v_type == VAR_UNKNOWN)
	प्रत्यागम curwin;

    win = find_win_by_nr_or_id(&argvars[idx]);
    यदि (win == रिक्त)
    {
	emsg(_(e_invalid_window_संख्या));
	प्रत्यागम रिक्त;
    }
    प्रत्यागम win;
}

/*
 * "col(string)" function
 */
    स्थिर शून्य
f_col(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    स्थिर शून्य
f_confirm(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#यदि defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*message;
    char_u	*buttons = रिक्त;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    पूर्णाङ्क		def = 1;
    पूर्णाङ्क		type = VIM_GENERIC;
    char_u	*typestr;
    पूर्णाङ्क		error = FALSE;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_संख्या_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	प्रत्यागम;

    message = tv_get_string_chk(&argvars[0]);
    यदि (message == रिक्त)
	error = TRUE;
    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	यदि (buttons == रिक्त)
	    error = TRUE;
	यदि (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (पूर्णाङ्क)tv_get_संख्या_chk(&argvars[2], &error);
	    यदि (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		यदि (typestr == रिक्त)
		    error = TRUE;
		अन्यथा
		{
		    विकल्प (TOUPPER_ASC(*typestr))
		    {
			पक्ष 'E': type = VIM_ERROR; भङ्ग;
			पक्ष 'Q': type = VIM_QUESTION; भङ्ग;
			पक्ष 'I': type = VIM_INFO; भङ्ग;
			पक्ष 'W': type = VIM_WARNING; भङ्ग;
			पक्ष 'G': type = VIM_GENERIC; भङ्ग;
		    }
		}
	    }
	}
    }

    यदि (buttons == रिक्त || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    यदि (!error)
	rettv->vval.v_संख्या = do_dialog(type, रिक्त, message, buttons,
							    def, रिक्त, FALSE);
#endif
}

/*
 * "copy()" function
 */
    स्थिर शून्य
f_copy(typval_T *argvars, typval_T *rettv)
{
    item_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    स्थिर शून्य
set_cursorpos(typval_T *argvars, typval_T *rettv, पूर्णाङ्क charcol)
{
    दीर्घ	lnum, col;
    दीर्घ	coladd = 0;
    पूर्णाङ्क		set_curswant = TRUE;

    यदि (in_vim9script()
	    && (check_for_string_or_संख्या_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 2) == FAIL)))
	प्रत्यागम;

    rettv->vval.v_संख्या = -1;
    यदि (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	यदि (list2fpos(argvars, &pos, रिक्त, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_argument));
	    प्रत्यागम;
	}
	lnum = pos.lnum;
	col = pos.col;
	coladd = pos.coladd;
	यदि (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    set_curswant = FALSE;
	}
    }
    अन्यथा यदि ((argvars[0].v_type == VAR_संख्या
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_संख्या
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	यदि (lnum < 0)
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
	अन्यथा यदि (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	col = (दीर्घ)tv_get_संख्या_chk(&argvars[1], रिक्त);
	यदि (charcol)
	    col = buf_charidx_to_byteidx(curbuf, lnum, col) + 1;
	यदि (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (दीर्घ)tv_get_संख्या_chk(&argvars[2], रिक्त);
    }
    अन्यथा
    {
	emsg(_(e_invalid_argument));
	प्रत्यागम;
    }
    यदि (lnum < 0 || col < 0 || coladd < 0)
	प्रत्यागम;		// type error; errmsg already given
    यदि (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    यदि (col != MAXCOL && --col < 0)
	col = 0;
    curwin->w_cursor.col = col;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    यदि (has_mbyte)
	mb_adjust_cursor();

    curwin->w_set_curswant = set_curswant;
    rettv->vval.v_संख्या = 0;
}

/*
 * "cursor(lnum, col)" function, or
 * "cursor(list)"
 *
 * Moves the cursor to the specified line and column.
 * Returns 0 when the position could be set, -1 otherwise.
 */
    स्थिर शून्य
f_cursor(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, FALSE);
}

#ifdef MSWIN
/*
 * "debugbreak()" function
 */
    स्थिर शून्य
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		pid;

    rettv->vval.v_संख्या = FAIL;
    यदि (in_vim9script() && check_for_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    pid = (पूर्णाङ्क)tv_get_संख्या(&argvars[0]);
    यदि (pid == 0)
    {
	emsg(_(e_invalid_argument));
	प्रत्यागम;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    यदि (hProcess == रिक्त)
	प्रत्यागम;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_संख्या = OK;
}
#endif

/*
 * "deepcopy()" function
 */
    स्थिर शून्य
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    यदि (check_for_opt_bool_arg(argvars, 1) == FAIL)
	प्रत्यागम;

    यदि (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], रिक्त);

    item_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    स्थिर शून्य
f_did_filetype(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->vval.v_संख्या = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    स्थिर शून्य
f_echoraw(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *str;

    यदि (check_secure())
	प्रत्यागम;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    str = tv_get_string_chk(&argvars[0]);
    यदि (str != रिक्त && *str != NUL)
    {
	out_str(str);
	out_flush();
    }
}

/*
 * "empty({expr})" function
 */
    स्थिर शून्य
f_empty(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		n = FALSE;

    विकल्प (argvars[0].v_type)
    {
	पक्ष VAR_STRING:
	पक्ष VAR_FUNC:
	    n = argvars[0].vval.v_string == रिक्त
					  || *argvars[0].vval.v_string == NUL;
	    भङ्ग;
	पक्ष VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == रिक्त;
	    भङ्ग;
	पक्ष VAR_संख्या:
	    n = argvars[0].vval.v_संख्या == 0;
	    भङ्ग;
	पक्ष VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    भङ्ग;
	पक्ष VAR_LIST:
	    n = argvars[0].vval.v_list == रिक्त
					|| argvars[0].vval.v_list->lv_len == 0;
	    भङ्ग;
	पक्ष VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    भङ्ग;
	पक्ष VAR_DICT:
	    n = argvars[0].vval.v_dict == रिक्त
			|| argvars[0].vval.v_dict->dv_hashtab.ht_used == 0;
	    भङ्ग;
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	    n = argvars[0].vval.v_संख्या != VVAL_TRUE;
	    भङ्ग;
	पक्ष VAR_CLASS:
	    n = argvars[0].vval.v_class != रिक्त;
	    भङ्ग;
	पक्ष VAR_OBJECT:
	    n = object_empty(argvars[0].vval.v_object);
	    भङ्ग;

	पक्ष VAR_BLOB:
	    n = argvars[0].vval.v_blob == रिक्त
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    भङ्ग;

	पक्ष VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_job == रिक्त
			   || argvars[0].vval.v_job->jv_status != JOB_STARTED;
	    भङ्ग;
#endif
	पक्ष VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_channel == रिक्त
			       || !channel_is_open(argvars[0].vval.v_channel);
	    भङ्ग;
#endif
	पक्ष VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == रिक्त
		|| argvars[0].vval.v_typealias->ta_name == रिक्त
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    भङ्ग;

	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	पक्ष VAR_INSTR:
	    internal_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    भङ्ग;
    }

    rettv->vval.v_संख्या = n;
}

/*
 * "environ()" function
 */
    स्थिर शून्य
f_environ(typval_T *argvars UNUSED, typval_T *rettv)
{
#यदि !defined(AMIGA)
    पूर्णाङ्क			i = 0;
    char_u		*entry, *मूल्य;
# यदि defined (MSWIN)
#  यदि !defined(_UCRT)
    बाह्य ब_अक्षर_प्रकार	**_wenviron;
#  endif
# अन्यथा
    बाह्य वर्ण		**environ;
# endif

    यदि (rettv_dict_alloc(rettv) == FAIL)
	प्रत्यागम;

# ifdef MSWIN
    यदि (*_wenviron == रिक्त)
	प्रत्यागम;
# अन्यथा
    यदि (*environ == रिक्त)
	प्रत्यागम;
# endif

    प्रति (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	यदि ((p = (short_u *)_wenviron[i]) == रिक्त)
	    प्रत्यागम;
	entry = utf16_to_enc(p, रिक्त);
# अन्यथा
	यदि ((entry = (char_u *)environ[i]) == रिक्त)
	    प्रत्यागम;
	entry = vim_strsave(entry);
# endif
	यदि (entry == रिक्त) // out of memory
	    प्रत्यागम;
	यदि ((मूल्य = vim_strchr(entry, '=')) == रिक्त)
	{
	    vim_free(entry);
	    अनुवर्त;
	}
	*मूल्य++ = NUL;
	dict_add_string(rettv->vval.v_dict, (वर्ण *)entry, मूल्य);
	vim_free(entry);
    }
#endif
}

/*
 * "err_teapot()" function
 */
    स्थिर शून्य
f_err_teapot(typval_T *argvars, typval_T *rettv UNUSED)
{
    यदि (argvars[0].v_type != VAR_UNKNOWN)
    {
	यदि (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    यदि (*skipwhite(s) == NUL)
		प्रत्यागम;
	}

	पूर्णाङ्क err = FALSE;
	पूर्णाङ्क do_503 = eval_व्यंजक_to_bool(&argvars[0], &err);
	यदि (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_available));
	    प्रत्यागम;
	}
    }

    emsg(_(e_im_a_teapot));
}

/*
 * "escape({string}, {chars})" function
 */
    स्थिर शून्य
f_escape(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    rettv->vval.v_string = vim_strsave_escaped(tv_get_string(&argvars[0]),
					 tv_get_string_buf(&argvars[1], buf));
    rettv->v_type = VAR_STRING;
}

/*
 * "eval()" function
 */
    स्थिर शून्य
f_eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*s, *p;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    s = tv_get_string_chk(&argvars[0]);
    यदि (s != रिक्त)
	s = skipwhite(s);

    p = s;
    यदि (s == रिक्त || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	यदि (p != रिक्त && !aborting())
	    semsg(_(e_invalid_expression_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_संख्या;
	rettv->vval.v_संख्या = 0;
    }
    अन्यथा यदि (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    स्थिर शून्य
f_eventhandler(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_संख्या = vgetc_busy || निवेश_busy;
}

स्थिर garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    शून्य
execute_redir_str(char_u *मूल्य, पूर्णाङ्क मूल्य_len)
{
    पूर्णाङ्क		len;

    यदि (मूल्य_len == -1)
	len = (पूर्णाङ्क)STRLEN(मूल्य);	// Append the entire string
    अन्यथा
	len = मूल्य_len;		// Append only "value_len" characters
    यदि (ga_grow(&redir_execute_ga, len) == FAIL)
	प्रत्यागम;

    mch_memmove((वर्ण *)redir_execute_ga.ga_data
	    + redir_execute_ga.ga_len, मूल्य, len);
    redir_execute_ga.ga_len += len;
}

#यदि defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    स्थिर char_u *
get_str_पंक्ति(
    पूर्णाङ्क	    c UNUSED,
    शून्य    *cookie,
    पूर्णाङ्क	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    char_u	*start = *(char_u **)cookie;
    char_u	*पंक्ति;
    char_u	*p;

    p = start;
    यदि (p == रिक्त || *p == NUL)
	प्रत्यागम रिक्त;
    p = vim_strchr(p, '\n');
    यदि (p == रिक्त)
	पंक्ति = vim_strsave(start);
    अन्यथा
    {
	पंक्ति = vim_strnsave(start, p - start);
	p++;
    }

    *(char_u **)cookie = p;
    प्रत्यागम पंक्ति;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    शून्य
execute_cmds_from_string(char_u *str)
{
    do_cmdline(रिक्त, get_str_पंक्ति, (शून्य *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#endif

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_पंक्ति(
    पूर्णाङ्क	    c UNUSED,
    शून्य    *cookie,
    पूर्णाङ्क	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *item = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    यदि (item == रिक्त)
	प्रत्यागम रिक्त;
    s = tv_get_string_buf_chk(&item->li_tv, buf);
    *p = item->li_next;
    प्रत्यागम s == रिक्त ? रिक्त : vim_strsave(s);
}

/*
 * "execute()" function
 */
    शून्य
execute_common(typval_T *argvars, typval_T *rettv, पूर्णाङ्क arg_off)
{
    char_u	*cmd = रिक्त;
    list_T	*list = रिक्त;
    पूर्णाङ्क		save_संदेश_silent = संदेश_silent;
    पूर्णाङ्क		save_emsg_silent = emsg_silent;
    पूर्णाङ्क		save_emsg_noredir = emsg_noredir;
    पूर्णाङ्क		save_redir_execute = redir_execute;
    पूर्णाङ्क		save_redir_off = redir_off;
    garray_T	save_ga;
    पूर्णाङ्क		save_संदेश_col = संदेश_col;
    पूर्णाङ्क		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    पूर्णाङ्क		echo_output = FALSE;

    rettv->vval.v_string = रिक्त;
    rettv->v_type = VAR_STRING;

    यदि (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	यदि (list == रिक्त || list->lv_len == 0)
	    // empty list, no commands, empty output
	    प्रत्यागम;
	++list->lv_refcount;
    }
    अन्यथा यदि (argvars[arg_off].v_type == VAR_JOB
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_मूल्य_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	प्रत्यागम;
    }
    अन्यथा
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	यदि (cmd == रिक्त)
	    प्रत्यागम;
    }

    यदि (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      in_vim9script());

	यदि (s == रिक्त)
	    प्रत्यागम;
	यदि (*s == NUL)
	    echo_output = TRUE;
	यदि (STRNCMP(s, "silent", 6) == 0)
	    ++संदेश_silent;
	यदि (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    अन्यथा
	++संदेश_silent;

    यदि (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, परिमाण(वर्ण), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    यदि (!echo_output)
	संदेश_col = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    यदि (cmd != रिक्त)
	do_cmdline_cmd(cmd);
    अन्यथा
    {
	listitem_T	*item;

	CHECK_LIST_MATERIALIZE(list);
	item = list->lv_first;
	do_cmdline(रिक्त, get_list_पंक्ति, (शून्य *)&item,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    यदि (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((वर्ण *)redir_execute_ga.ga_data)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_data;
    }
    अन्यथा
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = रिक्त;
    }
    संदेश_silent = save_संदेश_silent;
    emsg_silent = save_emsg_silent;
    emsg_noredir = save_emsg_noredir;

    redir_execute = save_redir_execute;
    यदि (redir_execute)
	redir_execute_ga = save_ga;
    redir_off = save_redir_off;

    // "silent reg" or "silent echo x" leaves msg_col somewhere in the line.
    यदि (echo_output)
	// When not working silently: put it in column zero.  A following
	// "echon" will overwrite the message, unavoidably.
	संदेश_col = 0;
    अन्यथा
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	संदेश_col = save_संदेश_col;
}

/*
 * "execute()" function
 */
    स्थिर शून्य
f_execute(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    शून्य
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    पूर्णाङ्क		n = FALSE;

    यदि (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    p = tv_get_string(&argvars[0]);
    यदि (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	यदि (mch_getenv(p + 1) != रिक्त)
	    n = TRUE;
	अन्यथा
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    यदि (p != रिक्त && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    अन्यथा यदि (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, रिक्त, TRUE) == OK);
	यदि (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    अन्यथा यदि (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    अन्यथा यदि (*p == '?')			// internal function only
    {
	n = has_internal_func_name(p + 1);
    }
    अन्यथा यदि (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    अन्यथा यदि (*p == '#')
    {
	यदि (p[1] == '#')
	    n = autocmd_supported(p + 2);
	अन्यथा
	    n = au_exists(p + 1);
    }
    अन्यथा				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_संख्या = n;
}

    स्थिर शून्य
f_exists_compiled(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    emsg(_(e_exists_compiled_can_only_be_used_in_def_function));
}

/*
 * "expand()" function
 */
    स्थिर शून्य
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    माप_प्रकार	len;
    पूर्णाङ्क		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    पूर्णाङ्क		error = FALSE;
    char_u	*result;
#ifdef BACKSLASH_IN_FILENAME
    char_u	*p_csl_save = p_csl;
#endif

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	प्रत्यागम;

#ifdef BACKSLASH_IN_FILENAME
    // avoid using 'completeslash' here
    p_csl = empty_option;
#endif

    rettv->v_type = VAR_STRING;
    यदि (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_set(rettv, रिक्त);

    s = tv_get_string(&argvars[0]);
    यदि (*s == '%' || *s == '#' || *s == '<')
    {
	वर्ण	*errormsg = रिक्त;

	यदि (p_verbose == 0)
	    ++emsg_off;
	result = eval_vars(s, s, &len, रिक्त, &errormsg, रिक्त, FALSE);
	यदि (p_verbose == 0)
	    --emsg_off;
	अन्यथा यदि (errormsg != रिक्त)
	    emsg(errormsg);
	यदि (rettv->v_type == VAR_LIST)
	{
	    यदि (rettv_list_alloc(rettv) == OK && result != रिक्त)
		list_append_string(rettv->vval.v_list, result, -1);
	    vim_free(result);
	}
	अन्यथा
	    rettv->vval.v_string = result;
    }
    अन्यथा
    {
	// When the optional second argument is non-zero, don't remove matches
	// for 'wildignore' and don't put matches for 'suffixes' at the end.
	यदि (argvars[1].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	यदि (!error)
	{
	    ExpandInit(&xpc);
	    xpc.xp_context = EXPAND_FILES;
	    यदि (p_wic)
		options += WILD_ICASE;
	    यदि (rettv->v_type == VAR_STRING)
		rettv->vval.v_string = ExpandOne(&xpc, s, रिक्त,
							   options, WILD_ALL);
	    अन्यथा यदि (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, रिक्त, options, WILD_ALL_KEEP);
		प्रति (पूर्णाङ्क i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	अन्यथा
	    rettv->vval.v_string = रिक्त;
    }
#ifdef BACKSLASH_IN_FILENAME
    p_csl = p_csl_save;
#endif
}

/*
 * "expandcmd()" function
 * Expand all the special characters in a command string.
 */
    स्थिर शून्य
f_expandcmd(typval_T *argvars, typval_T *rettv)
{
    exarg_T	eap;
    char_u	*cmdstr;
    वर्ण	*errormsg = रिक्त;
    पूर्णाङ्क		emsgoff = TRUE;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    यदि (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_FIELD(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = रिक्त;
    eap.cmdidx = CMD_USER;

    यदि (emsgoff)
	++emsg_off;
    यदि (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	यदि (!emsgoff && errormsg != रिक्त && *errormsg != NUL)
	    emsg(errormsg);
    यदि (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    स्थिर शून्य
f_feedkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    पूर्णाङ्क		remap = TRUE;
    पूर्णाङ्क		जोड़ो = FALSE;
    char_u	*कुंजियाँ, *flags;
    char_u	nbuf[NUMBUFLEN];
    पूर्णाङ्क		typed = FALSE;
    पूर्णाङ्क		execute = FALSE;
    पूर्णाङ्क		context = FALSE;
    पूर्णाङ्क		dangerous = FALSE;
    पूर्णाङ्क		lowlevel = FALSE;

    // This is not allowed in the sandbox.  If the commands would still be
    // executed in the sandbox it would be OK, but it probably happens later,
    // when "sandbox" is no longer set.
    यदि (check_secure())
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    कुंजियाँ = tv_get_string(&argvars[0]);

    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	प्रति ( ; *flags != NUL; ++flags)
	{
	    विकल्प (*flags)
	    {
		पक्ष 'n': remap = FALSE; भङ्ग;
		पक्ष 'm': remap = TRUE; भङ्ग;
		पक्ष 't': typed = TRUE; भङ्ग;
		पक्ष 'i': जोड़ो = TRUE; भङ्ग;
		पक्ष 'x': execute = TRUE; भङ्ग;
		पक्ष 'c': context = TRUE; भङ्ग;
		पक्ष '!': dangerous = TRUE; भङ्ग;
		पक्ष 'L': lowlevel = TRUE; भङ्ग;
	    }
	}
    }

    यदि (*कुंजियाँ != NUL || execute)
    {
	यदि (lowlevel
#ifdef FEAT_VTP
		&& (!is_term_win32()
		    || (कुंजियाँ[0] == 3 && ctrl_c_interrupts && typed))
#endif
	   )
	{
#ifdef USE_निवेश_BUF
	    ch_log(रिक्त, "feedkeys() lowlevel: %s", कुंजियाँ);

	    पूर्णाङ्क len = (पूर्णाङ्क)STRLEN(कुंजियाँ);
	    प्रति (पूर्णाङ्क idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		यदि (कुंजियाँ[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		add_to_निवेश_buf(कुंजियाँ + idx, 1);
	    }
#अन्यथा
	    emsg(_(e_lowlevel_निवेश_not_supported));
#endif
	}
	अन्यथा
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *कुंजियाँ_esc = vim_strsave_escape_csi(कुंजियाँ);
	    यदि (कुंजियाँ_esc == रिक्त)
		प्रत्यागम;

	    ch_log(रिक्त, "feedkeys(%s): %s", typed ? "typed" : "", कुंजियाँ);

	    ins_typebuf(कुंजियाँ_esc, (remap ? REMAP_YES : REMAP_NONE),
				   जोड़ो ? 0 : typebuf.tb_len, !typed, FALSE);
	    यदि (vgetc_busy
#ifdef FEAT_TIMERS
		    || timer_busy
#endif
		    || निवेश_busy)
		typebuf_was_filled = TRUE;

	    vim_free(कुंजियाँ_esc);
	}

	यदि (execute)
	{
	    पूर्णाङ्क		save_संदेश_scroll = संदेश_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    संदेश_scroll = FALSE;

	    ch_log(रिक्त, "feedkeys() executing");

	    यदि (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_version = 0;
	    }

	    यदि (!dangerous)
	    {
		++ex_normal_busy;
		++in_feedkeys;
	    }
	    ++allow_osc_कुंजी;
	    exec_normal(TRUE, lowlevel, TRUE);
	    --allow_osc_कुंजी;
	    यदि (!dangerous)
	    {
		--ex_normal_busy;
		--in_feedkeys;
	    }

	    संदेश_scroll |= save_संदेश_scroll;

	    यदि (context)
		current_sctx = save_sctx;
	}
    }
}

/*
 * "fnameescape({string})" function
 */
    स्थिर शून्य
f_fnameescape(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    स्थिर शून्य
f_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    यदि (gui.in_use)
    {
	gui_mch_set_foreground();
	प्रत्यागम;
    }
#endif
#यदि defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_set_foreground();
#endif
}

/*
 * "function()" function
 * "funcref()" function
 */
    स्थिर शून्य
common_function(typval_T *argvars, typval_T *rettv, पूर्णाङ्क is_funcref)
{
    char_u	*s;
    char_u	*name;
    पूर्णाङ्क		use_string = FALSE;
    partial_T   *arg_pt = रिक्त;
    char_u	*trans_name = रिक्त;
    पूर्णाङ्क		is_global = FALSE;
    char_u	*start_bracket = रिक्त;

    यदि (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    अन्यथा यदि (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != रिक्त)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    अन्यथा
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    यदि (s == रिक्त)
    {
	semsg(_(e_invalid_argument_str), "NULL");
	प्रत्यागम;
    }

    यदि ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == रिक्त) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, रिक्त);
	यदि (*name == '<')
	{
	    // generic function
	    start_bracket = name;
	    यदि (skip_generic_func_type_args(&name) == FAIL)
		गच्छ theend;
	}
	यदि (*name != NUL)
	    s = रिक्त;
    }

    यदि (s == रिक्त || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == रिक्त))
	semsg(_(e_invalid_argument_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    अन्यथा यदि (trans_name != रिक्त && (is_funcref
			 ? find_func(trans_name, is_global) == रिक्त
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    अन्यथा
    {
	पूर्णाङ्क	dict_idx = 0;
	पूर्णाङ्क	arg_idx = 0;
	list_T	*list = रिक्त;

	यदि (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	अन्यथा यदि (trans_name != रिक्त && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	अन्यथा
	    name = vim_strsave(s);

	यदि (argvars[1].v_type != VAR_UNKNOWN)
	{
	    यदि (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    अन्यथा यदि (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    अन्यथा
		// function(name, [args])
		arg_idx = 1;
	    यदि (dict_idx > 0)
	    {
		यदि (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    गच्छ theend;
		}
		यदि (argvars[dict_idx].vval.v_dict == रिक्त)
		    dict_idx = 0;
	    }
	    यदि (arg_idx > 0)
	    {
		यदि (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_second_argument_of_function_must_be_list_or_dict));
		    vim_free(name);
		    गच्छ theend;
		}
		list = argvars[arg_idx].vval.v_list;
		यदि (list == रिक्त || list->lv_len == 0)
		    arg_idx = 0;
		अन्यथा यदि (list->lv_len > MAX_FUNC_ARGS)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    गच्छ theend;
		}
	    }
	}
	यदि (dict_idx > 0 || arg_idx > 0 || arg_pt != रिक्त || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    यदि (pt == रिक्त)
		vim_free(name);
	    अन्यथा
	    {
		यदि (arg_idx > 0 || (arg_pt != रिक्त && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    पूर्णाङ्क		i = 0;
		    पूर्णाङ्क		arg_len = 0;
		    पूर्णाङ्क		lv_len = 0;

		    यदि (arg_pt != रिक्त)
			arg_len = arg_pt->pt_argc;
		    यदि (list != रिक्त)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    यदि (pt->pt_argv == रिक्त)
		    {
			vim_free(pt);
			vim_free(name);
			गच्छ theend;
		    }
		    प्रति (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    यदि (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_वस्तुएँ(list, li)
			    copy_tv(&li->li_tv, &pt->pt_argv[i++]);
		    }
		}

		// For "function(dict.func, [], dict)" and "func" is a partial
		// use "dict".  That is backwards compatible.
		यदि (dict_idx > 0)
		{
		    // The dict is bound explicitly, pt_auto is FALSE.
		    pt->pt_dict = argvars[dict_idx].vval.v_dict;
		    ++pt->pt_dict->dv_refcount;
		}
		अन्यथा यदि (arg_pt != रिक्त)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    यदि (pt->pt_dict != रिक्त)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    यदि (pt->pt_obj != रिक्त)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		यदि (arg_pt != रिक्त && arg_pt->pt_func != रिक्त)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		अन्यथा यदि (is_funcref)
		{
		    pt->pt_func = find_func(trans_name, is_global);
		    यदि (IS_GENERIC_FUNC(pt->pt_func) && start_bracket != रिक्त)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&start_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		अन्यथा
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		यदि (arg_pt != रिक्त)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	अन्यथा
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    यदि (start_bracket == रिक्त)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    अन्यथा
	    {
		// generic function
		माप_प्रकार len = STRLEN(name) + STRLEN(start_bracket);
		rettv->vval.v_string = alloc(len + 1);
		यदि (rettv->vval.v_string != रिक्त)
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
    स्थिर शून्य
f_funcref(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, TRUE);
}

/*
 * "function()" function
 */
    स्थिर शून्य
f_function(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, FALSE);
}

/*
 * "garbagecollect()" function
 */
    स्थिर शून्य
f_garbagecollect(typval_T *argvars, typval_T *rettv UNUSED)
{
    यदि (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    यदि (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
	garbage_collect_at_exit = TRUE;
}

/*
 * "get()" function
 */
    स्थिर शून्य
f_get(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    list_T	*l;
    tuple_T	*tuple;
    dictitem_T	*di;
    dict_T	*d;
    typval_T	*tv = रिक्त;
    पूर्णाङ्क		what_is_dict = FALSE;

    यदि (argvars[0].v_type == VAR_BLOB)
    {
	पूर्णाङ्क error = FALSE;
	पूर्णाङ्क idx = tv_get_संख्या_chk(&argvars[1], &error);

	यदि (!error)
	{
	    rettv->v_type = VAR_संख्या;
	    यदि (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    यदि (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_संख्या = -1;
	    अन्यथा
	    {
		rettv->vval.v_संख्या = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    अन्यथा यदि (argvars[0].v_type == VAR_LIST)
    {
	यदि ((l = argvars[0].vval.v_list) != रिक्त)
	{
	    पूर्णाङ्क		error = FALSE;

	    li = list_find(l, (दीर्घ)tv_get_संख्या_chk(&argvars[1], &error));
	    यदि (!error && li != रिक्त)
		tv = &li->li_tv;
	}
    }
    अन्यथा यदि (argvars[0].v_type == VAR_TUPLE)
    {
	यदि ((tuple = argvars[0].vval.v_tuple) != रिक्त)
	{
	    पूर्णाङ्क		error = FALSE;
	    दीर्घ	idx;

	    idx = (दीर्घ)tv_get_संख्या_chk(&argvars[1], &error);
	    यदि (!error)
		tv = tuple_find(tuple, idx);
	}
    }
    अन्यथा यदि (argvars[0].v_type == VAR_DICT)
    {
	यदि ((d = argvars[0].vval.v_dict) != रिक्त)
	{
	    di = dict_find(d, tv_get_string(&argvars[1]), -1);
	    यदि (di != रिक्त)
		tv = &di->di_tv;
	}
    }
    अन्यथा यदि (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	यदि (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	अन्यथा
	{
	    CLEAR_FIELD(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	यदि (pt != रिक्त)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    यदि (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		यदि (name == रिक्त)
		    rettv->vval.v_string = रिक्त;
		अन्यथा
		{
		    यदि (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    यदि (*what == 'n' && pt->pt_name == रिक्त
							&& pt->pt_func != रिक्त)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    अन्यथा यदि (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		यदि (pt->pt_dict != रिक्त)
		    rettv_dict_set(rettv, pt->pt_dict);
	    }
	    अन्यथा यदि (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		यदि (rettv_list_alloc(rettv) == OK)
		{
		    पूर्णाङ्क i;

		    प्रति (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    अन्यथा यदि (STRCMP(what, "arity") == 0)
	    {
		पूर्णाङ्क required = 0, optional = 0, varargs = FALSE;
		char_u *name = partial_name(pt);

		get_func_arity(name, &required, &optional, &varargs);

		rettv->v_type = VAR_DICT;
		यदि (rettv_dict_alloc(rettv) == OK)
		{
		    dict_T *dict = rettv->vval.v_dict;

		    // Take into account the arguments of the partial, if any.
		    // Note that it is possible to supply more arguments than the function
		    // accepts.
		    यदि (pt->pt_argc >= required + optional)
			required = optional = 0;
		    अन्यथा यदि (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    अन्यथा
			required -= pt->pt_argc;

		    dict_add_संख्या(dict, "required", required);
		    dict_add_संख्या(dict, "optional", optional);
		    dict_add_bool(dict, "varargs", varargs);
		}
	    }
	    अन्यथा
		semsg(_(e_invalid_argument_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    यदि (!what_is_dict)
		प्रत्यागम;
	}
    }
    अन्यथा
	semsg(_(e_argument_of_str_must_be_list_tuple_dictionary_or_blob), "get()");

    यदि (tv == रिक्त)
    {
	यदि (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    अन्यथा
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
    स्थिर शून्य
f_getbgcolor(typval_T *argvars UNUSED, typval_T *rettv)
{
    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

#यदि defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	यदि (gui.in_use)
	    try_hl = true;
# endif
# ifdef FEAT_TERMGUICOLORS
	यदि (p_tgc)
	    try_hl = true;
# endif
	यदि (try_hl)
	{
	    guicolor_T	fg, bg;
	    पूर्णाङ्क		hl_id = syn_name2id((char_u *)"Normal");

	    यदि (hl_id > 0)
	    {
		(शून्य)syn_id2colors(hl_id, &fg, &bg);
		यदि (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_संख्या(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_संख्या(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_संख्या(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    प्रत्यागम;
		}
	    }
	}
    }
#endif
#ifdef FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	यदि (term_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_संख्या(rettv->vval.v_list, (varnumber_T)r);
	    list_append_संख्या(rettv->vval.v_list, (varnumber_T)g);
	    list_append_संख्या(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#endif
}

/*
 * "getcellpixels()" function
 */
    स्थिर शून्य
f_getcellpixels(typval_T *argvars UNUSED, typval_T *rettv)
{
    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

#यदि defined(FEAT_GUI)
    यदि (gui.in_use)
    {
	// success pixel size and no gui.
	list_append_संख्या(rettv->vval.v_list, (varnumber_T)gui.char_width);
	list_append_संख्या(rettv->vval.v_list, (varnumber_T)gui.char_height);
    }
    अन्यथा
#endif
    {
#यदि defined(UNIX) || defined(MSWIN)
	list_append_संख्या(rettv->vval.v_list, (varnumber_T)cell_width);
	list_append_संख्या(rettv->vval.v_list, (varnumber_T)cell_height);
#अन्यथा
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_संख्या(rettv->vval.v_list, (varnumber_T)-1);
	list_append_संख्या(rettv->vval.v_list, (varnumber_T)-1);
#endif
    }

}

/*
 * "getchangelist()" function
 */
    स्थिर शून्य
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    पूर्णाङ्क		i;
    list_T	*l;
    dict_T	*d;
    पूर्णाङ्क		changelistindex;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    अन्यथा
	buf = tv_get_buf_from_arg(&argvars[0]);
    यदि (buf == रिक्त)
	प्रत्यागम;

    l = list_alloc();
    यदि (l == रिक्त)
	प्रत्यागम;
    यदि (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	प्रत्यागम;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    यदि (buf == curwin->w_buffer)
    {
	changelistindex = curwin->w_changelistidx;
    }
    अन्यथा
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    यदि (wip->wi_win == curwin)
		भङ्ग;
	changelistindex = wip != रिक्त ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_संख्या(rettv->vval.v_list, (varnumber_T)changelistindex);

    प्रति (i = 0; i < buf->b_changelistlen; ++i)
    {
	यदि (buf->b_changelist[i].lnum == 0)
	    अनुवर्त;
	यदि ((d = dict_alloc()) == रिक्त)
	    प्रत्यागम;
	यदि (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    प्रत्यागम;
	}
	dict_add_संख्या(d, "lnum", (दीर्घ)buf->b_changelist[i].lnum);
	dict_add_संख्या(d, "col", (दीर्घ)buf->b_changelist[i].col);
	dict_add_संख्या(d, "coladd", (दीर्घ)buf->b_changelist[i].coladd);
    }
}

    स्थिर शून्य
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    पूर्णाङ्क		getcurpos,
    पूर्णाङ्क		charcol)
{
    pos_T	*fp = रिक्त;
    pos_T	pos;
    win_T	*wp = curwin;
    list_T	*l;
    पूर्णाङ्क		fnum = -1;

    यदि (rettv_list_alloc(rettv) == OK)
    {
	l = rettv->vval.v_list;
	यदि (getcurpos)
	{
	    यदि (argvars[0].v_type != VAR_UNKNOWN)
	    {
		wp = find_win_by_nr_or_id(&argvars[0]);
		यदि (wp != रिक्त)
		    fp = &wp->w_cursor;
	    }
	    अन्यथा
		fp = &curwin->w_cursor;
	    यदि (fp != रिक्त && charcol)
	    {
		pos = *fp;
		pos.col =
		    buf_byteidx_to_charidx(wp->w_buffer, pos.lnum, pos.col);
		fp = &pos;
	    }
	}
	अन्यथा
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	यदि (fnum != -1)
	    list_append_संख्या(l, (varnumber_T)fnum);
	अन्यथा
	    list_append_संख्या(l, (varnumber_T)0);
	list_append_संख्या(l, (fp != रिक्त) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_संख्या(l, (fp != रिक्त)
		     ? (varnumber_T)(fp->col == MAXCOL ? MAXCOL : fp->col + 1)
							    : (varnumber_T)0);
	list_append_संख्या(l, (fp != रिक्त) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	यदि (getcurpos)
	{
	    पूर्णाङ्क	    save_set_curswant = curwin->w_set_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    यदि (wp == curwin)
		update_curswant();
	    list_append_संख्या(l, wp == रिक्त ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    यदि (wp == curwin && save_set_curswant)
	    {
		curwin->w_set_curswant = save_set_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    अन्यथा
	rettv->vval.v_संख्या = FALSE;
}

/*
 * "getcharpos()" function
 */
    स्थिर शून्य
f_getcharpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    स्थिर शून्य
f_getcharsearch(typval_T *argvars UNUSED, typval_T *rettv)
{
    यदि (rettv_dict_alloc(rettv) != OK)
	प्रत्यागम;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_string(dict, "char", last_csearch());
    dict_add_संख्या(dict, "forward", last_csearch_forward());
    dict_add_संख्या(dict, "until", last_csearch_until());
}

/*
 * "getenv()" function
 */
    स्थिर शून्य
f_getenv(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क	    mustfree = FALSE;
    char_u  *p;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    यदि (p == रिक्त)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_संख्या = VVAL_NULL;
	प्रत्यागम;
    }
    यदि (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    स्थिर शून्य
f_getfontname(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    यदि (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

#ifdef FEAT_GUI
    यदि (gui.in_use)
    {
	GuiFont font;
	char_u	*name = रिक्त;

	यदि (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	अन्यथा
	{
	    name = tv_get_string(&argvars[0]);
	    यदि (STRCMP(name, "*") == 0)	    // don't use font dialog
		प्रत्यागम;
	    font = gui_mch_get_font(name, FALSE);
	    यदि (font == NOFONT)
		प्रत्यागम;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	यदि (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#endif
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
स्थिर संरचना
{
    argcheck_T	func;
    वर्ण	*types;	    // separated by commas
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
    {arg_list_संख्या,		"list<number>"},
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
    {arg_संख्या,		"number"},
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
    स्थिर list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    वर्ण	*types = "any";

    यदि (l == रिक्त)
	प्रत्यागम रिक्त;
    प्रति (पूर्णाङ्क i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(argcheck_types); ++i)
	यदि (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    भङ्ग;
	}
    प्रति (वर्ण *p = types; ; )
    {
	वर्ण	*end = (वर्ण *)vim_strchr((char_u *)p, ',');
	पूर्णाङ्क	len = end == रिक्त ? (पूर्णाङ्क)STRLEN(p) : (पूर्णाङ्क)(end - p);

	यदि (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    प्रत्यागम रिक्त;
	}
	यदि (end == रिक्त)
	    भङ्ग;
	p = end + 1;
    }
    प्रत्यागम l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    स्थिर पूर्णाङ्क
getinfo_argtypes(
	list_T			*l,
	ध्रुव funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    पूर्णाङ्क		argcount = 0;

    यदि (l != रिक्त)
	FOR_ALL_LIST_वस्तुएँ(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    यदि (s == रिक्त)
		प्रत्यागम -1;
	    यदि (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_ARGS)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		प्रत्यागम -1;
	    }
	    type = parse_type(&p, type_gap, रिक्त, रिक्त, TRUE);
	    यदि (type == रिक्त)
		प्रत्यागम -1;
	    यदि (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		प्रत्यागम -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    प्रति ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_ARGS; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    प्रत्यागम argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    स्थिर वर्ण *
builtin_ret_type_name(
	पूर्णाङ्क	    idx,
	पूर्णाङ्क	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	वर्ण	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = internal_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    प्रत्यागम type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
प्रकारनिर्देश गणना {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_kind_T;

स्थिर संरचना
{
    वर्ण		*name;
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
    स्थिर शून्य
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_kind_T	kind;
    पूर्णाङ्क			ki;
    पूर्णाङ्क			idx;
    ध्रुव funcentry_T	*fe;
    dict_T		*d;
    list_T		*args;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_ARGS];
    list_T		*argtypes_list = रिक्त;
    पूर्णाङ्क			argcount;
    पूर्णाङ्क			vim9 = -1;	// -1: the context of the caller
    वर्ण		*ret_name;
    वर्ण		*tofree;

    यदि (rettv_dict_alloc(rettv) == FAIL)
	प्रत्यागम;
    यदि (in_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;
    यदि (check_for_opt_dict_arg(argvars, 2) == FAIL)
	प्रत्यागम;

    kindname = tv_get_string(&argvars[0]);
    प्रति (ki = 0; ki < (पूर्णाङ्क)ARRAY_LENGTH(getinfo_kinds); ++ki)
	यदि (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    भङ्ग;
    यदि (ki == (पूर्णाङ्क)ARRAY_LENGTH(getinfo_kinds))
    {
	semsg(_(e_invalid_argument_str), kindname);
	प्रत्यागम;
    }
    kind = getinfo_kinds[ki].kind;
    name = tv_get_string_buf(&argvars[1], namebuf);

    यदि (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != रिक्त)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	पूर्णाङ्क	    todo = (पूर्णाङ्क)opts->dv_hashtab.ht_used;

	FOR_ALL_HASHTAB_वस्तुएँ(&opts->dv_hashtab, hi, todo)
	{
	    यदि (HASHITEM_EMPTY(hi))
		अनुवर्त;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    यदि (STRCMP(di->di_कुंजी, "argtypes") == 0 && kind == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    अन्यथा यदि (STRCMP(di->di_कुंजी, "vim9") == 0 && kind == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    अन्यथा
	    {
		semsg(_(e_invalid_argument_str), di->di_कुंजी);
		प्रत्यागम;
	    }
	}
    }
    यदि (kind == GI_VIMVAR)
    {
	यदि (STRNCMP(name, "v:", 2) == 0)
	    (शून्य)vim_var_info(name + 2, rettv->vval.v_dict);
	प्रत्यागम;
    }
    यदि (kind == GI_COMMAND)
    {
	(शून्य)ex_command_info(name, vim9, rettv->vval.v_dict);
	प्रत्यागम;
    }
    यदि (kind == GI_OPTION)
    {
	(शून्य)option_info(name, rettv->vval.v_dict);
	प्रत्यागम;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = find_internal_func_opt(name, FALSE);
    यदि (idx < 0)
    {
	(शून्य)user_func_info(name, rettv->vval.v_dict);
	प्रत्यागम;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, परिमाण(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    यदि (argcount < 0)
    {
	clear_type_list(&type_gap);
	प्रत्यागम;
    }
    d = rettv->vval.v_dict;
    dict_add_string(d, "name", (char_u *)fe->f_name);
    dict_add_string(d, "kind", (char_u *)"builtin");
    dict_add_bool(d, "available", fe->f_func != रिक्त);
    dict_add_संख्या(d, "minargs", fe->f_min_argc);
    dict_add_संख्या(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_add_संख्या(d, "method", fe->f_argtype & FEARG_MASK);

    args = list_alloc();
    यदि (args == रिक्त || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	clear_type_list(&type_gap);
	प्रत्यागम;
    }
    argnames = (char_u *)fe->f_argnames;
    प्रति (पूर्णाङ्क i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = रिक्त;
	पूर्णाङ्क	    argnamelen = 0;

	यदि (fe->f_argcheck == रिक्त)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    यदि (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		भङ्ग;
	    check = arg_any;
	}
	अन्यथा
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    यदि ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == रिक्त)
		भङ्ग;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	यदि (argnames != रिक्त)
	{
	    char_u  *end = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = end == रिक्त ? (पूर्णाङ्क)STRLEN(argnames)
						     : (पूर्णाङ्क)(end - argnames);
	    argnames = end == रिक्त ? रिक्त : skipwhite(end + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	यदि (arg == रिक्त || types == रिक्त
		|| dict_add_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    प्रत्यागम;
	}
	यदि ((argnamelen > 0
		    && dict_add_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    प्रत्यागम;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    यदि (argtypes_list == रिक्त
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	वर्ण	*tofree_max;
	वर्ण	*max_name;

	प्रति (पूर्णाङ्क i = argcount; i < fe->f_max_argc && i < MAX_FUNC_ARGS; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	यदि (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = रिक्त;
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
    स्थिर शून्य
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    पूर्णाङ्क		i;
    list_T	*l;
    dict_T	*d;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_opt_संख्या_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 1) == FAIL)))
	प्रत्यागम;

    wp = find_tabwin(&argvars[0], &argvars[1], रिक्त);
    यदि (wp == रिक्त)
	प्रत्यागम;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    यदि (l == रिक्त)
	प्रत्यागम;
    यदि (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	प्रत्यागम;
    }

    list_append_संख्या(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    प्रति (i = 0; i < wp->w_jumplistlen; ++i)
    {
	यदि (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    अनुवर्त;
	यदि ((d = dict_alloc()) == रिक्त)
	    प्रत्यागम;
	यदि (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    प्रत्यागम;
	}
	dict_add_संख्या(d, "lnum", (दीर्घ)wp->w_jumplist[i].fmark.mark.lnum);
	dict_add_संख्या(d, "col", (दीर्घ)wp->w_jumplist[i].fmark.mark.col);
	dict_add_संख्या(d, "coladd", (दीर्घ)wp->w_jumplist[i].fmark.mark.coladd);
	dict_add_संख्या(d, "bufnr", (दीर्घ)wp->w_jumplist[i].fmark.fnum);
	यदि (wp->w_jumplist[i].fname != रिक्त)
	    dict_add_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    स्थिर शून्य
f_getpid(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_संख्या = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    स्थिर शून्य
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_opt_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    स्थिर शून्य
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_opt_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    स्थिर शून्य
f_getpos(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    स्थिर पूर्णाङ्क
block_def2str(संरचना block_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    यदि (ret->string == रिक्त)
    {
	ret->length = 0;
	प्रत्यागम FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->length = bd->startspaces;
    mch_memmove(ret->string + ret->length, bd->textstart, bd->textlen);
    ret->length += bd->textlen;
    vim_memset(ret->string + ret->length, ' ', bd->endspaces);
    ret->length += bd->endspaces;
    ret->string[ret->length] = NUL;

    प्रत्यागम OK;
}

    स्थिर पूर्णाङ्क
getregionpos(
    typval_T	*argvars,
    typval_T	*rettv,
    pos_T	*p1,
    pos_T	*p2,
    पूर्णाङ्क		*inclusive,
    पूर्णाङ्क		*region_type,
    oparg_T	*oap)
{
    पूर्णाङ्क		fnum1 = -1, fnum2 = -1;
    char_u	*type;
    buf_T	*findbuf;
    char_u	default_type[] = "v";
    पूर्णाङ्क		block_width = 0;
    पूर्णाङ्क		is_select_exclusive;
    पूर्णाङ्क		l;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम FAIL;

    यदि (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	प्रत्यागम FAIL;

    यदि (list2fpos(&argvars[0], p1, &fnum1, रिक्त, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, रिक्त, FALSE) != OK
	    || fnum1 != fnum2)
	प्रत्यागम FAIL;

    यदि (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	यदि (type == रिक्त)
	    type = default_type;
    }
    अन्यथा
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    यदि (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    अन्यथा यदि (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    अन्यथा यदि (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	यदि (*p != NUL && ((block_width = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str_str), "type", type);
	    प्रत्यागम FAIL;
	}
	*region_type = MBLOCK;
    }
    अन्यथा
    {
	semsg(_(e_invalid_मूल्य_for_argument_str_str), "type", type);
	प्रत्यागम FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    यदि (findbuf == रिक्त || findbuf->b_ml.ml_mfp == रिक्त)
    {
	emsg(_(e_buffer_is_not_loaded));
	प्रत्यागम FAIL;
    }

    यदि (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_पंक्ति_गिनती)
    {
	semsg(_(e_invalid_पंक्ति_संख्या_nr), p1->lnum);
	प्रत्यागम FAIL;
    }
    यदि (p1->col == MAXCOL)
	p1->col = ml_get_buf_len(findbuf, p1->lnum) + 1;
    अन्यथा यदि (p1->col < 1 || p1->col > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_column_संख्या_nr), p1->col);
	प्रत्यागम FAIL;
    }

    यदि (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_पंक्ति_गिनती)
    {
	semsg(_(e_invalid_पंक्ति_संख्या_nr), p2->lnum);
	प्रत्यागम FAIL;
    }
    यदि (p2->col == MAXCOL)
	p2->col = ml_get_buf_len(findbuf, p2->lnum) + 1;
    अन्यथा यदि (p2->col < 1 || p2->col > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_column_संख्या_nr), p2->col);
	प्रत्यागम FAIL;
    }

    curbuf = findbuf;
    curwin->w_buffer = curbuf;
    virtual_op = virtual_active();

    // NOTE: Adjustment is needed.
    p1->col--;
    p2->col--;

    यदि (!LT_POS(*p1, *p2))
    {
	// swap position
	pos_T p;

	p = *p1;
	*p1 = *p2;
	*p2 = p;
    }

    यदि (*region_type == MCHAR)
    {
	// Handle 'selection' == "exclusive".
	यदि (is_select_exclusive && !EQUAL_POS(*p1, *p2))
	    // When backing up to previous line, inclusive becomes false.
	    *inclusive = !unadjust_for_sel_inner(p2);
	// If p2 is on NUL (end of line), inclusive becomes false.
	यदि (*inclusive && !virtual_op && *ml_get_pos(p2) == NUL)
	    *inclusive = FALSE;
    }
    अन्यथा यदि (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#ifdef FEAT_LINEBREAK
	पूर्णाङ्क	lbr_saved = reset_lbr();
#endif

	getvvcol(curwin, p1, &sc1, रिक्त, &ec1, 0);
	getvvcol(curwin, p2, &sc2, रिक्त, &ec2, 0);

#ifdef FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#endif

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->start = *p1;
	oap->end = *p2;
	oap->start_vcol = MIN(sc1, sc2);
	यदि (block_width > 0)
	    oap->end_vcol = oap->start_vcol + block_width - 1;
	अन्यथा यदि (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->end_vcol = sc2 - 1;
	अन्यथा
	    oap->end_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    यदि (l > 1)
	p2->col += l - 1;

    प्रत्यागम OK;
}

/*
 * "getregion()" function
 */
    स्थिर शून्य
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    पूर्णाङ्क			inclusive = TRUE;
    पूर्णाङ्क			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    पूर्णाङ्क			save_virtual;
    string_T		akt = {रिक्त, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    यदि (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	प्रत्यागम;

    प्रति (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	पूर्णाङ्क ret = OK;

	यदि (region_type == MBLOCK)
	{
	    संरचना block_def	bd;

	    block_prep(&oa, &bd, lnum, FALSE);
	    block_def2str(&bd, &akt);
	}
	अन्यथा यदि (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.length = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.length);
	    यदि (akt.string == रिक्त)
		akt.length = 0;
	    अन्यथा
		akt.length = s.length;
	}
	अन्यथा
	{
	    संरचना block_def	bd;

	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);
	    block_def2str(&bd, &akt);
	}

	यदि (akt.string == रिक्त)
	{
	    clear_tv(rettv);
	    (शून्य)rettv_list_alloc(rettv);
	    भङ्ग;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (पूर्णाङ्क)akt.length);
	vim_free(akt.string);

	यदि (ret == FAIL)
	{
	    clear_tv(rettv);
	    (शून्य)rettv_list_alloc(rettv);
	    भङ्ग;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_buffer = curbuf;
    virtual_op = save_virtual;
}

    स्थिर शून्य
add_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    यदि (l1 == रिक्त)
	प्रत्यागम;

    यदि (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	प्रत्यागम;
    }

    l2 = list_alloc();
    यदि (l2 == रिक्त)
	प्रत्यागम;

    यदि (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	प्रत्यागम;
    }

    l3 = list_alloc();
    यदि (l3 == रिक्त)
	प्रत्यागम;

    यदि (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	प्रत्यागम;
    }

    list_append_संख्या(l2, curbuf->b_fnum);
    list_append_संख्या(l2, p1.lnum);
    list_append_संख्या(l2, p1.col);
    list_append_संख्या(l2, p1.coladd);

    list_append_संख्या(l3, curbuf->b_fnum);
    list_append_संख्या(l3, p2.lnum);
    list_append_संख्या(l3, p2.col);
    list_append_संख्या(l3, p2.coladd);
}

/*
 * Compute the positions of the region segment on line "lnum".
 * "ret_p1" is set to the start position of the segment and "ret_p2" to its
 * end position.
 */
    स्थिर शून्य
getregionpos_पंक्ति(
    linenr_T	lnum,
    pos_T	p1,
    pos_T	p2,
    पूर्णाङ्क		inclusive,
    पूर्णाङ्क		region_type,
    oparg_T	*oap,
    पूर्णाङ्क		allow_eol,
    pos_T	*ret_p1,
    pos_T	*ret_p2)
{
    char_u	*पंक्ति = ml_get(lnum);
    colnr_T	पंक्ति_len = ml_get_len(lnum);

    यदि (region_type == MLINE)
    {
	ret_p1->col = 1;
	ret_p1->coladd = 0;
	ret_p2->col = MAXCOL;
	ret_p2->coladd = 0;
    }
    अन्यथा
    {
	संरचना block_def	bd;

	यदि (region_type == MBLOCK)
	    block_prep(oap, &bd, lnum, FALSE);
	अन्यथा
	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);

	यदि (bd.is_oneChar)  // selection entirely inside one char
	{
	    यदि (region_type == MBLOCK)
	    {
		ret_p1->col = mb_prevptr(पंक्ति, bd.textstart) - पंक्ति + 1;
		ret_p1->coladd = bd.start_char_vcols
					   - (bd.start_vcol - oap->start_vcol);
	    }
	    अन्यथा
	    {
		ret_p1->col = p1.col + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	अन्यथा यदि (region_type == MBLOCK && oap->start_vcol > bd.start_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->col = MAXCOL;
	    ret_p1->coladd = oap->start_vcol - bd.start_vcol;
	    bd.is_oneChar = TRUE;
	}
	अन्यथा यदि (bd.startspaces > 0)
	{
	    ret_p1->col = mb_prevptr(पंक्ति, bd.textstart) - पंक्ति + 1;
	    ret_p1->coladd = bd.start_char_vcols - bd.startspaces;
	}
	अन्यथा
	{
	    ret_p1->col = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	यदि (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->col = ret_p1->col;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	अन्यथा यदि (bd.endspaces > 0)
	{
	    ret_p2->col = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	अन्यथा
	{
	    ret_p2->col = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    यदि (!allow_eol && ret_p1->col > पंक्ति_len)
    {
	ret_p1->col = 0;
	ret_p1->coladd = 0;
    }
    अन्यथा यदि (ret_p1->col > पंक्ति_len + 1)
	ret_p1->col = पंक्ति_len + 1;

    यदि (!allow_eol && ret_p2->col > पंक्ति_len)
    {
	ret_p2->col = ret_p1->col == 0 ? 0 : पंक्ति_len;
	ret_p2->coladd = 0;
    }
    अन्यथा यदि (ret_p2->col > पंक्ति_len + 1)
	ret_p2->col = पंक्ति_len + 1;

    ret_p1->lnum = lnum;
    ret_p2->lnum = lnum;
}

/*
 * "getregionpos()" function
 */
    स्थिर शून्य
f_getregionpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	p1, p2;
    पूर्णाङ्क		inclusive = TRUE;
    पूर्णाङ्क		region_type = -1;
    पूर्णाङ्क		allow_eol = FALSE;
    पूर्णाङ्क		bounds_only = FALSE;
    oparg_T	oa;
    पूर्णाङ्क		lnum;

    buf_T	*save_curbuf;
    पूर्णाङ्क		save_virtual;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    यदि (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	प्रत्यागम;

    यदि (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    यदि (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	start_pos, end_pos;

	getregionpos_पंक्ति(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &start_pos, &end_pos);
	यदि (p2.lnum != p1.lnum)
	{
	    pos_T	unused;

	    getregionpos_पंक्ति(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &unused, &end_pos);
	}
	add_regionpos_range(rettv, start_pos, end_pos);
    }
    अन्यथा
    {
	प्रति (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_पंक्ति(lnum, p1, p2, inclusive, region_type, &oa,
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
    स्थिर पूर्णाङ्क
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    यदि (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	यदि (strregname != रिक्त && in_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_register_name_must_be_one_char_str), strregname);
	    strregname = रिक्त;
	}
	यदि (strregname == रिक्त)	    // type error; errmsg already given
	    प्रत्यागम 0;
    }
    अन्यथा
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    प्रत्यागम *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    स्थिर शून्य
f_getreg(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		regname;
    पूर्णाङ्क		arg2 = FALSE;
    पूर्णाङ्क		return_list = FALSE;

    यदि (in_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	प्रत्यागम;

    regname = getreg_get_regname(argvars);
    यदि (regname == 0)
	प्रत्यागम;

    यदि (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	पूर्णाङ्क		error = FALSE;

	arg2 = (पूर्णाङ्क)tv_get_bool_chk(&argvars[1], &error);

	यदि (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (पूर्णाङ्क)tv_get_bool_chk(&argvars[2], &error);
	यदि (error)
	    प्रत्यागम;
    }

    यदि (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_व्यंजक_SRC : 0) | GREG_LIST);
	यदि (rettv->vval.v_list == रिक्त)
	    (शून्य)rettv_list_alloc(rettv);
	अन्यथा
	    ++rettv->vval.v_list->lv_refcount;
    }
    अन्यथा
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = get_reg_contents(regname,
						    arg2 ? GREG_व्यंजक_SRC : 0);
    }
}

/*
 * "getregtype()" function
 */
    स्थिर शून्य
f_getregtype(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		regname;
    char_u	buf[NUMBUFLEN + 2];
    दीर्घ	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    यदि (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    regname = getreg_get_regname(argvars);
    यदि (regname == 0)
	प्रत्यागम;

    buf[0] = NUL;
    buf[1] = NUL;
    विकल्प (get_reg_type(regname, &reglen))
    {
	पक्ष MLINE: buf[0] = 'V'; भङ्ग;
	पक्ष MCHAR: buf[0] = 'v'; भङ्ग;
	पक्ष MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((वर्ण *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		भङ्ग;
    }
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "gettagstack()" function
 */
    स्थिर शून्य
f_gettagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;			// default is current window

    यदि (rettv_dict_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script() && check_for_opt_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	यदि (wp == रिक्त)
	    प्रत्यागम;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    स्थिर शून्य
f_gettext(typval_T *argvars, typval_T *rettv)
{
#यदि defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    वर्ण *prev = रिक्त;
#endif

    यदि (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_STRING;

    यदि (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != रिक्त &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#यदि defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ध्रुव वर्ण *)argvars[1].vval.v_string, (वर्ण *)p_enc);
#endif

#यदि defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((ध्रुव वर्ण *)argvars[1].vval.v_string, (ध्रुव वर्ण *)argvars[0].vval.v_string));
#अन्यथा
	textdomain((ध्रुव वर्ण *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#endif

#यदि defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	यदि (prev != रिक्त)
	    bind_textdomain_codeset((ध्रुव वर्ण *)argvars[1].vval.v_string, prev);
#endif
    }
    अन्यथा
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
}

// for VIM_VERSION_ defines
#अन्तर्भाव "version.h"

/*
 * "has()" function
 */
    शून्य
f_has(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		i;
    char_u	*name;
    पूर्णाङ्क		x = FALSE;
    पूर्णाङ्क		n = FALSE;
    प्रकारनिर्देश संरचना {
	वर्ण *name;
	लघु present;
    } has_item_T;
    स्थिर has_item_T has_list[] =
    {
	{"amiga",
#ifdef AMIGA
		1
#अन्यथा
		0
#endif
		},
	{"android",
#ifdef __ANDROID__
		1
#अन्यथा
		0
#endif
		},
	{"arp",
#यदि defined(AMIGA) && defined(FEAT_ARP)
		1
#अन्यथा
		0
#endif
		},
	{"haiku",
#ifdef __HAIKU__
		1
#अन्यथा
		0
#endif
		},
	{"bsd",
#यदि defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#अन्यथा
		0
#endif
		},
	{"hpux",
#ifdef hpux
		1
#अन्यथा
		0
#endif
		},
	{"hurd",
#ifdef __GNU__
		1
#अन्यथा
		0
#endif
		},
	{"linux",
#ifdef __linux__
		1
#अन्यथा
		0
#endif
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#ifdef MACOS_X
		1
#अन्यथा
		0
#endif
		},
	{"osx",		// Mac OS X
#ifdef MACOS_X
		1
#अन्यथा
		0
#endif
		},
	{"macunix",	// Mac OS X, with the darwin feature
#यदि defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#अन्यथा
		0
#endif
		},
	{"osxdarwin",	// synonym for macunix
#यदि defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#अन्यथा
		0
#endif
		},
	{"qnx",
#ifdef __QNX__
		1
#अन्यथा
		0
#endif
		},
	{"sun",
#ifdef SUN_SYSTEM
		1
#अन्यथा
		0
#endif
		},
	{"termux",
#ifdef __TERMUX__
		1
#अन्यथा
		0
#endif
		},
	{"unix",
#ifdef UNIX
		1
#अन्यथा
		0
#endif
		},
	{"vms",
#ifdef VMS
		1
#अन्यथा
		0
#endif
		},
	{"win32",
#ifdef MSWIN
		1
#अन्यथा
		0
#endif
		},
	{"win32unix",
#ifdef WIN32UNIX
		1
#अन्यथा
		0
#endif
		},
	{"win64",
#ifdef _WIN64
		1
#अन्यथा
		0
#endif
		},
	{"ebcdic", 0 },
	{"fname_case",
#ifndef CASE_INSENSITIVE_FILENAME
		1
#अन्यथा
		0
#endif
		},
	{"acl",
#ifdef HAVE_ACL
		1
#अन्यथा
		0
#endif
		},
	{"arabic",
#ifdef FEAT_ARABIC
		1
#अन्यथा
		0
#endif
		},
	{"autocmd", 1},
	{"autochdir",
#ifdef FEAT_AUTOCHDIR
		1
#अन्यथा
		0
#endif
		},
	{"autoservername",
#ifdef FEAT_AUTOSERVERNAME
		1
#अन्यथा
		0
#endif
		},
	{"socketserver",
#ifdef FEAT_SOCKETSERVER
		1
#अन्यथा
		0
#endif
		},
	{"balloon_eval",
#ifdef FEAT_BEVAL_GUI
		1
#अन्यथा
		0
#endif
		},
	{"balloon_multiline",
#ifdef FEAT_BEVAL_GUI
		1
#अन्यथा
		0
#endif
		},
	{"balloon_eval_term",
#ifdef FEAT_BEVAL_TERM
		1
#अन्यथा
		0
#endif
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#यदि defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#अन्यथा
		0
#endif
		},
	{"byte_offset",
#ifdef FEAT_BYTEOFF
		1
#अन्यथा
		0
#endif
		},
	{"channel",
#ifdef FEAT_JOB_CHANNEL
		1
#अन्यथा
		0
#endif
		},
	{"cindent", 1},
	{"clientserver",
#ifdef FEAT_CLIENTSERVER
		1
#अन्यथा
		0
#endif
		},
	{"clipboard",
#ifdef FEAT_CLIPBOARD
		1
#अन्यथा
		0
#endif
		},
	{"clipboard_provider",
#ifdef FEAT_CLIPBOARD_PROVIDER
		1
#अन्यथा
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
#अन्यथा
		0
#endif
		},
	{"cryptv",
#ifdef FEAT_CRYPT
		1
#अन्यथा
		0
#endif
		},
	{"crypt-blowfish",
#ifdef FEAT_CRYPT
		1
#अन्यथा
		0
#endif
		},
	{"crypt-blowfish2",
#ifdef FEAT_CRYPT
		1
#अन्यथा
		0
#endif
		},
	{"cscope",
#ifdef FEAT_CSCOPE
		1
#अन्यथा
		0
#endif
		},
	{"cursorbind", 1},
	{"cursorshape",
#ifdef CURSOR_SHAPE
		1
#अन्यथा
		0
#endif
		},
	{"debug",
#ifdef DEBUG
		1
#अन्यथा
		0
#endif
		},
	{"dialog_con",
#ifdef FEAT_CON_DIALOG
		1
#अन्यथा
		0
#endif
		},
	{"dialog_con_gui",
#यदि defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#अन्यथा
		0
#endif
		},
	{"dialog_gui",
#ifdef FEAT_GUI_DIALOG
		1
#अन्यथा
		0
#endif
		},
	{"diff",
#ifdef FEAT_DIFF
		1
#अन्यथा
		0
#endif
		},
	{"digraphs",
#ifdef FEAT_DIGRAPHS
		1
#अन्यथा
		0
#endif
		},
	{"directx",
#ifdef FEAT_DIRECTX
		1
#अन्यथा
		0
#endif
		},
	{"dnd",
#ifdef FEAT_DND
		1
#अन्यथा
		0
#endif
		},
	{"drop_file",
#ifdef HAVE_DROP_FILE
		1
#अन्यथा
		0
#endif
		},
	{"emacs_tags",
#ifdef FEAT_EMACS_TAGS
		1
#अन्यथा
		0
#endif
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#ifdef FEAT_खोज_EXTRA
		1
#अन्यथा
		0
#endif
		},
	{"file_in_path", 1},
	{"filterpipe",
#यदि defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#अन्यथा
		0
#endif
		},
	{"find_in_path",
#ifdef FEAT_FIND_ID
		1
#अन्यथा
		0
#endif
		},
	{"float", 1},
	{"folding",
#ifdef FEAT_FOLDING
		1
#अन्यथा
		0
#endif
		},
	{"footer", 0},
	{"fork",
#यदि !defined(USE_SYSTEM) && defined(UNIX)
		1
#अन्यथा
		0
#endif
		},
	{"gettext",
#ifdef FEAT_GETTEXT
		1
#अन्यथा
		0
#endif
		},
	{"gui",
#ifdef FEAT_GUI
		1
#अन्यथा
		0
#endif
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#ifdef FEAT_GUI_GTK
		1
#अन्यथा
		0
#endif
		},
	{"gui_gtk2",
#यदि defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#अन्यथा
		0
#endif
		},
	{"gui_gtk3",
#यदि defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#अन्यथा
		0
#endif
		},
	{"gui_gtk4",
#यदि defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#अन्यथा
		0
#endif
		},
	{"gui_gnome",
#ifdef FEAT_GUI_GNOME
		1
#अन्यथा
		0
#endif
		},
	{"gui_haiku",
#ifdef FEAT_GUI_HAIKU
		1
#अन्यथा
		0
#endif
		},
	{"gui_mac", 0},
	{"gui_motif",
#ifdef FEAT_GUI_MOTIF
		1
#अन्यथा
		0
#endif
		},
	{"gui_photon",
#ifdef FEAT_GUI_PHOTON
		1
#अन्यथा
		0
#endif
		},
	{"gui_win32",
#ifdef FEAT_GUI_MSWIN
		1
#अन्यथा
		0
#endif
		},
	{"iconv",
#यदि defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#अन्यथा
		0
#endif
		},
	{"image",
#ifdef FEAT_IMAGE
		1
#अन्यथा
		0
#endif
		},
	{"image_popup",
#ifdef FEAT_IMAGE_POPUP
		1
#अन्यथा
		0
#endif
		},
	{"insert_expand", 1},
	{"ipv6",
#ifdef FEAT_IPV6
		1
#अन्यथा
		0
#endif
	},
	{"job",
#ifdef FEAT_JOB_CHANNEL
		1
#अन्यथा
		0
#endif
		},
	{"jumplist", 1},
	{"keymap",
#ifdef FEAT_KEYMAP
		1
#अन्यथा
		0
#endif
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#ifdef FEAT_LANGMAP
		1
#अन्यथा
		0
#endif
		},
	{"libcall",
#ifdef FEAT_LIBCALL
		1
#अन्यथा
		0
#endif
		},
	{"linebreak",
#ifdef FEAT_LINEBREAK
		1
#अन्यथा
		0
#endif
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#यदि defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#अन्यथा
		0
#endif
		},
	{"menu",
#ifdef FEAT_MENU
		1
#अन्यथा
		0
#endif
		},
	{"mksession",
#ifdef FEAT_SESSION
		1
#अन्यथा
		0
#endif
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#ifdef FEAT_MOUSESHAPE
		1
#अन्यथा
		0
#endif
		},
	{"mouse_dec",
#यदि (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#अन्यथा
		0
#endif
		},
	{"mouse_gpm",
#यदि (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#अन्यथा
		0
#endif
		},
	{"mouse_jsbterm",
#यदि (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#अन्यथा
		0
#endif
		},
	{"mouse_netterm",
#यदि (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#अन्यथा
		0
#endif
		},
	{"mouse_pterm",
#यदि (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#अन्यथा
		0
#endif
		},
	{"mouse_sgr",
#यदि (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#अन्यथा
		0
#endif
		},
	{"mouse_sysmouse",
#यदि (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#अन्यथा
		0
#endif
		},
	{"mouse_urxvt",
#यदि (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#अन्यथा
		0
#endif
		},
	{"mouse_xterm",
#यदि (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#अन्यथा
		0
#endif
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#ifdef FEAT_MBYTE_IME
		1
#अन्यथा
		0
#endif
		},
	{"multi_lang",
#ifdef FEAT_MULTI_LANG
		1
#अन्यथा
		0
#endif
		},
	{"mzscheme",
#यदि defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#अन्यथा
		0
#endif
		},
	{"nanotime",
#ifdef ST_MTIM_NSEC
		1
#अन्यथा
		0
#endif
	},
	{"num64", 1},
	{"ole",
#ifdef FEAT_OLE
		1
#अन्यथा
		0
#endif
		},
	{"packages",
#ifdef FEAT_EVAL
		1
#अन्यथा
		0
#endif
		},
	{"path_extra", 1},
	{"perl",
#यदि defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#अन्यथा
		0
#endif
		},
	{"persistent_undo",
#ifdef FEAT_PERSISTENT_UNDO
		1
#अन्यथा
		0
#endif
		},
	{"python_compiled",
#यदि defined(FEAT_PYTHON)
		1
#अन्यथा
		0
#endif
		},
	{"python_dynamic",
#यदि defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#अन्यथा
		0
#endif
		},
	{"python",
#यदि defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#अन्यथा
		0
#endif
		},
	{"pythonx",
#यदि (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#अन्यथा
		0
#endif
		},
	{"python3_compiled",
#यदि defined(FEAT_PYTHON3)
		1
#अन्यथा
		0
#endif
		},
	{"python3_dynamic",
#यदि defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#अन्यथा
		0
#endif
		},
	{"python3_stable",
#यदि defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#अन्यथा
		0
#endif
		},
	{"python3",
#यदि defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#अन्यथा
		0
#endif
		},
	{"popupwin",
#ifdef FEAT_PROP_POPUP
		1
#अन्यथा
		0
#endif
		},
	{"postscript",
#ifdef FEAT_POSTSCRIPT
		1
#अन्यथा
		0
#endif
		},
	{"pango",
#ifdef FEAT_PRINT_PANGO
		1
#अन्यथा
		0
#endif
		},
	{"printer",
#ifdef FEAT_PRINTER
		1
#अन्यथा
		0
#endif
		},
	{"profile",
#ifdef FEAT_PROFILE
		1
#अन्यथा
		0
#endif
		},
	{"prof_nsec",
#ifdef PROF_NSEC
		1
#अन्यथा
		0
#endif
		},
	{"reltime",
#ifdef FEAT_RELTIME
		1
#अन्यथा
		0
#endif
		},
	{"quickfix",
#ifdef FEAT_QUICKFIX
		1
#अन्यथा
		0
#endif
		},
	{"rightleft",
#ifdef FEAT_RIGHTLEFT
		1
#अन्यथा
		0
#endif
		},
	{"ruby",
#यदि defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#अन्यथा
		0
#endif
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#ifdef FEAT_SIGNS
		1
#अन्यथा
		0
#endif
		},
	{"smartindent", 1},
	{"startuptime",
#ifdef STARTUPTIME
		1
#अन्यथा
		0
#endif
		},
	{"statusline",
#ifdef FEAT_STL_OPT
		1
#अन्यथा
		0
#endif
		},
	{"statusline_click",
#ifdef FEAT_STL_OPT
		1
#अन्यथा
		0
#endif
		},
	{"netbeans_intg",
#ifdef FEAT_NETBEANS_INTG
		1
#अन्यथा
		0
#endif
		},
	{"sodium",
#यदि defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#अन्यथा
		0
#endif
		},
	{"sound",
#ifdef FEAT_SOUND
		1
#अन्यथा
		0
#endif
		},
	{"spell",
#ifdef FEAT_SPELL
		1
#अन्यथा
		0
#endif
		},
	{"syntax",
#ifdef FEAT_SYN_HL
		1
#अन्यथा
		0
#endif
		},
	{"system",
#यदि defined(USE_SYSTEM) || !defined(UNIX)
		1
#अन्यथा
		0
#endif
		},
	{"tabpanel",
#यदि defined(FEAT_TABPANEL)
		1,
#अन्यथा
		0,
#endif
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#यदि defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#अन्यथा
		0
#endif
		},
	{"termguicolors",
#ifdef FEAT_TERMGUICOLORS
		1
#अन्यथा
		0
#endif
		},
	{"terminal",
#यदि defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#अन्यथा
		0
#endif
		},
	{"terminfo",
#ifdef TERMINFO
		1
#अन्यथा
		0
#endif
		},
	{"termresponse",
#ifdef FEAT_TERMRESPONSE
		1
#अन्यथा
		0
#endif
		},
	{"textobjects", 1},
	{"textprop",
#ifdef FEAT_PROP_POPUP
		1
#अन्यथा
		0
#endif
		},
	{"tgetent",
#ifdef HAVE_TGETENT
		1
#अन्यथा
		0
#endif
		},
	{"timers",
#ifdef FEAT_TIMERS
		1
#अन्यथा
		0
#endif
		},
	{"title", 1},
	{"toolbar",
#ifdef FEAT_TOOLBAR
		1
#अन्यथा
		0
#endif
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#ifdef FEAT_VARTABS
		1
#अन्यथा
		0
#endif
		},
	{"vertsplit", 1},
	{"viminfo",
#ifdef FEAT_VIMINFO
		1
#अन्यथा
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
#अन्यथा
		0
#endif
		},
	{"wayland",
#ifdef FEAT_WAYLAND
		1
#अन्यथा
		0
#endif
		},
	{"wayland_clipboard",
#ifdef FEAT_WAYLAND_CLIPBOARD
		1
#अन्यथा
		0
#endif
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#ifdef FEAT_WAK
		1
#अन्यथा
		0
#endif
		},
	{"writebackup",
#ifdef FEAT_WRITEBACKUP
		1
#अन्यथा
		0
#endif
		},
	{"xattr",
#ifdef FEAT_XATTR
		1
#अन्यथा
		0
#endif
		},
	{"xim",
#ifdef FEAT_XIM
		1
#अन्यथा
		0
#endif
		},
	{"xfontset",
#ifdef FEAT_XFONTSET
		1
#अन्यथा
		0
#endif
		},
	{"xpm",
#यदि defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#अन्यथा
		0
#endif
		},
	{"xpm_w32",	// for backward compatibility
#ifdef FEAT_XPM_W32
		1
#अन्यथा
		0
#endif
		},
	{"xsmp",
#ifdef USE_XSMP
		1
#अन्यथा
		0
#endif
		},
	{"xsmp_interact",
#ifdef USE_XSMP_INTERACT
		1
#अन्यथा
		0
#endif
		},
	{"xterm_clipboard",
#ifdef FEAT_XCLIPBOARD
		1
#अन्यथा
		0
#endif
		},
	{"xterm_save",
#ifdef FEAT_XTERM_SAVE
		1
#अन्यथा
		0
#endif
		},
	{"X11",
#यदि defined(UNIX) && defined(FEAT_X11)
		1
#अन्यथा
		0
#endif
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#यदि defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#अन्यथा
		0
#endif
		},
	{रिक्त, 0}
    };

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    name = tv_get_string(&argvars[0]);

    // Fast-path: check features not in has_list[] first to avoid the full
    // linear scan for very common queries like has('patch-...').
    यदि (STRNICMP(name, "patch", 5) == 0)
    {
	x = TRUE;
	यदि (name[5] == '-'
		&& STRLEN(name) >= 11
		&& (name[6] >= '1' && name[6] <= '9'))
	{
	    वर्ण	*end;
	    पूर्णाङ्क	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (पूर्णाङ्क)म_से_अदीर्घ((वर्ण *)name + 6, &end, 10);
	    यदि (*end == '.' && vim_isdigit(end[1])
		    && end[2] == '.' && vim_isdigit(end[3]))
	    {
		minor = म_से_प(end + 1);

		// Expect "patch-9.9.01234".
		n = (major < VIM_VERSION_MAJOR
		     || (major == VIM_VERSION_MAJOR
			 && (minor < VIM_VERSION_MINOR
			     || (minor == VIM_VERSION_MINOR
				 && has_patch(म_से_प(end + 3))))));
	    }
	}
	अन्यथा यदि (SAFE_isdigit(name[5]))
	    n = has_patch(म_से_प((वर्ण *)name + 5));
    }
    अन्यथा यदि (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    अन्यथा यदि (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_निवेश_isatty();
    }
    अन्यथा यदि (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    अन्यथा यदि (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    अन्यथा यदि (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#ifdef FEAT_GUI
	n = (gui.in_use || gui.starting);
#endif
    }
    अन्यथा यदि (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#यदि defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.in_use;	// gui_mch_browse() works when GUI is running
#endif
    }
    अन्यथा यदि (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#ifdef FEAT_SYN_HL
	n = syntax_present(curwin);
#endif
    }
    अन्यथा यदि (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#ifdef FEAT_VTP
	n = is_term_win32() && has_vtp_working();
#endif
    }
    अन्यथा यदि (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_NETBEANS_INTG
	n = netbeans_active();
#endif
    }
    अन्यथा यदि (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_MOUSE_GPM
	n = gpm_enabled();
#endif
    }
    अन्यथा यदि (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#यदि defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#endif
    }
    अन्यथा यदि (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_star.available;
#endif
    }
    अन्यथा यदि (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# यदि defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	यदि (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	अन्यथा
	    n = FALSE;
# अन्यथा
	n = FALSE;
# endif
#endif
    }

    // Look up in has_list[] only if not already handled above.
    यदि (x == FALSE)
    {
	प्रति (i = 0; has_list[i].name != रिक्त; ++i)
	    यदि (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		भङ्ग;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	यदि (x == TRUE && n == FALSE)
	{
	    यदि (0)
	    {
		// intentionally empty
	    }
#ifdef VIMDLL
	    अन्यथा यदि (STRICMP(name, "filterpipe") == 0)
		n = gui.in_use || gui.starting;
#endif
#यदि defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    अन्यथा यदि (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#endif
#ifdef DYNAMIC_LUA
	    अन्यथा यदि (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#endif
#ifdef DYNAMIC_MZSCHEME
	    अन्यथा यदि (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#endif
#ifdef DYNAMIC_PERL
	    अन्यथा यदि (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON
	    अन्यथा यदि (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON3
	    अन्यथा यदि (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#endif
#यदि defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    अन्यथा यदि (STRICMP(name, "pythonx") == 0)
	    {
# यदि defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		यदि (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		अन्यथा यदि (p_pyx == 3)
		    n = python3_enabled(FALSE);
		अन्यथा यदि (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#endif
#ifdef DYNAMIC_RUBY
	    अन्यथा यदि (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#endif
#ifdef DYNAMIC_TCL
	    अन्यथा यदि (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#endif
#ifdef DYNAMIC_SODIUM
	    अन्यथा यदि (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#endif
#यदि defined(FEAT_TERMINAL) && defined(MSWIN)
	    अन्यथा यदि (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#endif
#ifdef DYNAMIC_GPM
	    अन्यथा यदि (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_available();
#endif
	}
    }

    यदि (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_संख्या = x;
    अन्यथा
	// return whether feature is enabled
	rettv->vval.v_संख्या = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    पूर्णाङ्क
dynamic_feature(char_u *feature)
{
    प्रत्यागम (feature == रिक्त
#यदि defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.in_use)
#endif
#ifdef VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#endif
#यदि defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.in_use)
#endif
#यदि defined(USE_ICONV) && defined(DYNAMIC_ICONV)
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
#यदि defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
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
#यदि defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#endif
	    );
}

/*
 * "haslocaldir()" function
 */
    स्थिर शून्य
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = रिक्त;
    win_T	*wp = रिक्त;

    यदि (in_vim9script()
	    && (check_for_opt_संख्या_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 1) == FAIL)))
	प्रत्यागम;

    wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    यदि (wp != रिक्त && wp->w_localdir != रिक्त)
	rettv->vval.v_संख्या = 1;
    अन्यथा यदि (tp != रिक्त && tp->tp_localdir != रिक्त)
	rettv->vval.v_संख्या = 2;
    अन्यथा
	rettv->vval.v_संख्या = 0;
}

/*
 * "highlightID(name)" function
 */
    स्थिर शून्य
f_hlID(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->vval.v_संख्या = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    स्थिर शून्य
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->vval.v_संख्या = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    स्थिर शून्य
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
# यदि defined(HAVE_NO_LONG_LONG)
प्रकारनिर्देश अचिन्हित पूर्णाङ्क uintptr_t;
# अन्यथा
प्रकारनिर्देश अचिन्हित दीर्घ दीर्घ uintptr_t;
# endif
#endif // VMS

    स्थिर शून्य
f_id(typval_T *argvars, typval_T *rettv)
{
    वर्ण    numbuf[NUMBUFLEN];
    वर्ण    *p = numbuf;

    विकल्प (argvars[0].v_type)
    {
	पक्ष VAR_LIST:
	पक्ष VAR_TUPLE:
	पक्ष VAR_DICT:
	पक्ष VAR_OBJECT:
	पक्ष VAR_JOB:
	पक्ष VAR_CHANNEL:
	पक्ष VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    यदि (argvars[0].vval.v_object != रिक्त)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(शून्य *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		प्रति (; p < numbuf + परिमाण(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	स्वभाव:
	    भङ्ग;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    स्थिर शून्य
index_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    पूर्णाङ्क		start = 0;
    पूर्णाङ्क		error = FALSE;
    पूर्णाङ्क		ic = FALSE;

    b = argvars[0].vval.v_blob;
    यदि (b == रिक्त)
	प्रत्यागम;

    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	start = tv_get_संख्या_chk(&argvars[2], &error);
	यदि (error)
	    प्रत्यागम;
    }

    यदि (start < 0)
    {
	start = blob_len(b) + start;
	यदि (start < 0)
	    start = 0;
    }

    प्रति (पूर्णाङ्क idx = start; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_संख्या;
	tv.vval.v_संख्या = blob_get(b, idx);
	यदि (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_संख्या = idx;
	    प्रत्यागम;
	}
    }
}

/*
 * index() function for a tuple
 */
    स्थिर शून्य
index_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    पूर्णाङ्क		ic = FALSE;
    पूर्णाङ्क		error = FALSE;

    यदि (tuple == रिक्त)
	प्रत्यागम;

    पूर्णाङ्क	start_idx = 0;
    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	start_idx = tv_get_संख्या_chk(&argvars[2], &error);
	यदि (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (पूर्णाङ्क)tv_get_bool_chk(&argvars[3], &error);
	यदि (error)
	    प्रत्यागम;
    }

    rettv->vval.v_संख्या = index_tuple(tuple, &argvars[1], start_idx, ic);
}

/*
 * index() function for a list
 */
    स्थिर शून्य
index_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*item;
    दीर्घ	idx = 0;
    पूर्णाङ्क		ic = FALSE;
    पूर्णाङ्क		error = FALSE;

    l = argvars[0].vval.v_list;
    यदि (l == रिक्त)
	प्रत्यागम;

    CHECK_LIST_MATERIALIZE(l);
    item = l->lv_first;
    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, (दीर्घ)tv_get_संख्या_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	यदि (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (पूर्णाङ्क)tv_get_bool_chk(&argvars[3], &error);
	यदि (error)
	    item = रिक्त;
    }

    प्रति ( ; item != रिक्त; item = item->li_next, ++idx)
	यदि (tv_equal(&item->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_संख्या = idx;
	    भङ्ग;
	}
}

/*
 * "index()" function
 */
    स्थिर शून्य
f_index(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_संख्या = -1;

    यदि (in_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_संख्या_arg(argvars, 1) == FAIL)
		|| check_for_opt_संख्या_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_BLOB)
	index_func_blob(argvars, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_TUPLE)
	index_func_tuple(argvars, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_LIST)
	index_func_list(argvars, rettv);
    अन्यथा
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    पूर्णाङ्क
indexof_eval_व्यंजक(typval_T *व्यंजक)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	found;
    पूर्णाङ्क		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_कुंजी);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    यदि (eval_व्यंजक_typval(व्यंजक, FALSE, argv, 2, रिक्त, &newtv) == FAIL)
	प्रत्यागम FALSE;

    found = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    प्रत्यागम error ? FALSE : found;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    स्थिर पूर्णाङ्क
indexof_blob(blob_T *b, दीर्घ startidx, typval_T *व्यंजक)
{
    दीर्घ	idx = 0;

    यदि (b == रिक्त)
	प्रत्यागम -1;

    यदि (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	यदि (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_कुंजी, VAR_संख्या);
    set_vim_var_type(VV_VAL, VAR_संख्या);

    पूर्णाङ्क		called_emsg_start = called_emsg;
    प्रति (idx = startidx; idx < blob_len(b); ++idx)
    {
	set_vim_var_nr(VV_कुंजी, idx);
	set_vim_var_nr(VV_VAL, blob_get(b, idx));

	यदि (indexof_eval_व्यंजक(व्यंजक))
	    प्रत्यागम idx;

	यदि (called_emsg != called_emsg_start)
	    प्रत्यागम -1;
    }

    प्रत्यागम -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    स्थिर पूर्णाङ्क
indexof_list(list_T *l, दीर्घ startidx, typval_T *व्यंजक)
{
    listitem_T	*item;
    दीर्घ	idx = 0;
    पूर्णाङ्क		found;

    यदि (l == रिक्त)
	प्रत्यागम -1;

    CHECK_LIST_MATERIALIZE(l);

    यदि (startidx == 0)
	item = l->lv_first;
    अन्यथा
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, startidx);
	यदि (item != रिक्त)
	    idx = l->lv_u.mat.lv_idx;
    }

    set_vim_var_type(VV_कुंजी, VAR_संख्या);

    पूर्णाङ्क		called_emsg_start = called_emsg;
    प्रति ( ; item != रिक्त; item = item->li_next, ++idx)
    {
	set_vim_var_nr(VV_कुंजी, idx);
	copy_tv(&item->li_tv, get_vim_var_tv(VV_VAL));

	found = indexof_eval_व्यंजक(व्यंजक);
	clear_tv(get_vim_var_tv(VV_VAL));

	यदि (found)
	    प्रत्यागम idx;

	यदि (called_emsg != called_emsg_start)
	    प्रत्यागम -1;
    }

    प्रत्यागम -1;
}

/*
 * "indexof()" function
 */
    स्थिर शून्य
f_indexof(typval_T *argvars, typval_T *rettv)
{
    दीर्घ	startidx = 0;
    typval_T	save_val;
    typval_T	save_कुंजी;
    पूर्णाङ्क		save_did_emsg;

    rettv->vval.v_संख्या = -1;

    यदि (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	प्रत्यागम;

    यदि ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == रिक्त
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == रिक्त))
	प्रत्यागम;

    यदि (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_संख्या_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_कुंजी, &save_कुंजी);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    यदि (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_संख्या = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    अन्यथा यदि (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_संख्या = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    अन्यथा
	rettv->vval.v_संख्या = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_कुंजी, &save_कुंजी);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

स्थिर पूर्णाङ्क inputsecret_flag = 0;

/*
 * "input()" function
 *     Also handles inputsecret() when inputsecret is set.
 */
    स्थिर शून्य
f_निवेश(typval_T *argvars, typval_T *rettv)
{
    get_user_निवेश(argvars, rettv, FALSE, inputsecret_flag);
}

/*
 * "inputdialog()" function
 */
    स्थिर शून्य
f_inputdialog(typval_T *argvars, typval_T *rettv)
{
#यदि defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    यदि (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == रिक्त)
    {
	char_u	*message;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	यदि (in_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    प्रत्यागम;

	message = tv_get_string_chk(&argvars[0]);
	यदि (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != रिक्त)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	अन्यथा
	    IObuff[0] = NUL;
	यदि (message != रिक्त && defstr != रिक्त
		&& do_dialog(VIM_QUESTION, रिक्त, message,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	अन्यथा
	{
	    यदि (message != रिक्त && defstr != रिक्त
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    अन्यथा
		rettv->vval.v_string = रिक्त;
	}
	rettv->v_type = VAR_STRING;
    }
    अन्यथा
#endif
	get_user_निवेश(argvars, rettv, TRUE, inputsecret_flag);
}

/*
 * "inputlist()" function
 */
    स्थिर शून्य
f_inputlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    पूर्णाङ्क		selected;
    पूर्णाङ्क		mouse_used;

#ifdef NO_CONSOLE_निवेश
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    यदि (no_console_निवेश() && !is_not_a_term())
	प्रत्यागम;
#endif
    यदि (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == रिक्त)
    {
	semsg(_(e_argument_of_str_must_be_list), "inputlist()");
	प्रत्यागम;
    }

    संदेश_start();
    संदेश_row = Rows - 1;	// for when 'cmdheight' > 1
    पंक्तियाँ_बायाँ = Rows;	// avoid more prompt
    संदेश_scroll = TRUE;
    संदेश_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_वस्तुएँ(l, li)
    {
	संदेश_puts((वर्ण *)tv_get_string(&li->li_tv));
	संदेश_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_संख्या(&mouse_used);
    यदि (mouse_used)
	selected -= पंक्तियाँ_बायाँ;

    rettv->vval.v_संख्या = selected;
}

स्थिर garray_T	    ga_userinput = {0, 0, परिमाण(tasave_T), 4, रिक्त};

/*
 * "inputrestore()" function
 */
    स्थिर शून्य
f_inputrestore(typval_T *argvars UNUSED, typval_T *rettv)
{
    यदि (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_data)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    अन्यथा यदि (p_verbose > 1)
    {
	verb_संदेश(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_संख्या = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    स्थिर शून्य
f_inputsave(typval_T *argvars UNUSED, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    यदि (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_data)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    अन्यथा
	rettv->vval.v_संख्या = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    स्थिर शून्य
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    ++cmdline_star;
    ++inputsecret_flag;
    f_निवेश(argvars, rettv);
    --cmdline_star;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    स्थिर शून्य
f_interrupt(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    स्थिर शून्य
f_invert(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->vval.v_संख्या = ~tv_get_संख्या_chk(&argvars[0], रिक्त);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    स्थिर शून्य
free_lval_मूल(lval_मूल_T *मूल)
{
    यदि (मूल->lr_tv != रिक्त)
	free_tv(मूल->lr_tv);
    class_unref(मूल->lr_cl_exec);
    मूल->lr_tv = रिक्त;
    मूल->lr_cl_exec = रिक्त;
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

    स्थिर पूर्णाङ्क
fix_variable_reference_lval_मूल(lval_मूल_T *मूल, char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    पूर्णाङ्क found_member = FALSE;
    यदि (मूल->lr_tv->v_type == VAR_OBJECT)
    {
	यदि (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    found_member = TRUE;
    }
    यदि (!found_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*end;
	प्रति (end = name; ASCII_ISALNUM(*end) || *end == '_'; ++end)
	    ;
	पूर्णाङ्क idx = class_member_idx(मूल->lr_cl_exec, name, end - name);
	यदि (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(मूल->lr_tv);
	    copy_tv(&मूल->lr_cl_exec->class_members_tv[idx], मूल->lr_tv);
	    found_member = TRUE;
	}
    }
    यदि (!found_member)
    {
	free_tv(मूल->lr_tv);
	मूल->lr_tv = रिक्त;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    प्रत्यागम OK;
}

/*
 * "islocked()" function
 */
    स्थिर शून्य
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*end;
    dictitem_T	*di;

    rettv->vval.v_संख्या = -1;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    char_u *name = tv_get_string(&argvars[0]);
#ifdef LOG_LOCKVAR
    ch_log(रिक्त, "LKVAR: f_islocked(): name: %s", name);
#endif

    lval_मूल_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_मूल_T *मूल = रिक्त;

    // Set up lval_root if executing in a method.
    यदि (fill_exec_lval_मूल(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	यदि (fix_variable_reference_lval_मूल(&aroot, name) == OK)
	    मूल = &aroot;
    }

    lval_मूल_T	*lval_मूल_save = lval_मूल;
    lval_मूल = मूल;
    end = get_lval(name, रिक्त, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_START);
    lval_मूल = lval_मूल_save;

    यदि (end != रिक्त && lv.ll_name != रिक्त)
    {
	यदि (*end != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_end
		   ? e_invalid_argument_str : e_trailing_characters_str), end);
	}
	अन्यथा
	{
	    यदि (lv.ll_tv == रिक्त)
	    {
		di = find_var(lv.ll_name, रिक्त, TRUE);
		यदि (di != रिक्त)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_संख्या = ((di->di_flags & DI_FLAGS_LOCK)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    अन्यथा यदि (lv.ll_is_मूल)
	    {
		rettv->vval.v_संख्या = tv_islocked(lv.ll_tv);
	    }
	    अन्यथा यदि (lv.ll_object != रिक्त)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_संख्या = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(रिक्त, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#endif
	    }
	    अन्यथा यदि (lv.ll_class != रिक्त)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_संख्या = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(रिक्त, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#endif
	    }
	    अन्यथा यदि (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    अन्यथा यदि (lv.ll_newkey != रिक्त)
		semsg(_(e_कुंजी_not_present_in_dictionary_str), lv.ll_newkey);
	    अन्यथा यदि (lv.ll_list != रिक्त)
		// List item.
		rettv->vval.v_संख्या = tv_islocked(&lv.ll_li->li_tv);
	    अन्यथा यदि (lv.ll_tuple != रिक्त)
		// Tuple item.
		rettv->vval.v_संख्या = tv_islocked(lv.ll_tv);
	    अन्यथा
		// Dictionary item.
		rettv->vval.v_संख्या = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    यदि (मूल != रिक्त)
	free_lval_मूल(मूल);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    स्थिर शून्य
f_वस्तुएँ(typval_T *argvars, typval_T *rettv)
{
    यदि (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    अन्यथा
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), 1);
}

/*
 * "keytrans()" function
 */
    स्थिर शून्य
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    यदि (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == रिक्त)
	प्रत्यागम;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    स्थिर शून्य
f_last_buffer_nr(typval_T *argvars UNUSED, typval_T *rettv)
{
    पूर्णाङ्क		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	यदि (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_संख्या = n;
}

/*
 * "len()" function
 */
    शून्य
f_len(typval_T *argvars, typval_T *rettv)
{
    विकल्प (argvars[0].v_type)
    {
	पक्ष VAR_STRING:
	पक्ष VAR_संख्या:
	    rettv->vval.v_संख्या = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    भङ्ग;
	पक्ष VAR_BLOB:
	    rettv->vval.v_संख्या = blob_len(argvars[0].vval.v_blob);
	    भङ्ग;
	पक्ष VAR_LIST:
	    rettv->vval.v_संख्या = list_len(argvars[0].vval.v_list);
	    भङ्ग;
	पक्ष VAR_TUPLE:
	    rettv->vval.v_संख्या = tuple_len(argvars[0].vval.v_tuple);
	    भङ्ग;
	पक्ष VAR_DICT:
	    rettv->vval.v_संख्या = dict_len(argvars[0].vval.v_dict);
	    भङ्ग;
	पक्ष VAR_OBJECT:
	    rettv->vval.v_संख्या = object_len(argvars[0].vval.v_object);
	    भङ्ग;
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	पक्ष VAR_FLOAT:
	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL:
	पक्ष VAR_JOB:
	पक्ष VAR_CHANNEL:
	पक्ष VAR_INSTR:
	पक्ष VAR_CLASS:
	पक्ष VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    भङ्ग;
    }
}

    स्थिर शून्य
libcall_common(typval_T *argvars UNUSED, typval_T *rettv, पूर्णाङ्क type)
{
#ifdef FEAT_LIBCALL
    char_u		*string_in;
    char_u		**string_result;
    पूर्णाङ्क			nr_result;
#endif

    rettv->v_type = type;
    यदि (type != VAR_संख्या)
	rettv->vval.v_string = रिक्त;

    यदि (check_restricted() || check_secure())
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_संख्या_arg(argvars, 2) == FAIL))
	प्रत्यागम;

#ifdef FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    यदि (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_in = रिक्त;
	यदि (argvars[2].v_type == VAR_STRING)
	    string_in = argvars[2].vval.v_string;
	यदि (type == VAR_संख्या)
	{
	    string_result = रिक्त;
	}
	अन्यथा
	{
	    rettv->vval.v_string = रिक्त;
	    string_result = &rettv->vval.v_string;
	}
	यदि (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_in,
			     argvars[2].vval.v_संख्या,
			     string_result,
			     &nr_result) == OK
		&& type == VAR_संख्या)
	    rettv->vval.v_संख्या = nr_result;
    }
#endif
}

/*
 * "libcall()" function
 */
    स्थिर शून्य
f_libcall(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_STRING);
}

/*
 * "libcallnr()" function
 */
    स्थिर शून्य
f_libcallnr(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_संख्या);
}

/*
 * "line(string, [winid])" function
 */
    स्थिर शून्य
f_पंक्ति(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum = 0;
    pos_T	*fp = रिक्त;
    पूर्णाङ्क		fnum;
    पूर्णाङ्क		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (पूर्णाङ्क)tv_get_संख्या(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	यदि (wp != रिक्त && tp != रिक्त)
	{
	    यदि (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		यदि (*p_spk != 'c'
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
    अन्यथा
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    यदि (fp != रिक्त)
	lnum = fp->lnum;
    rettv->vval.v_संख्या = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    स्थिर शून्य
f_line2byte(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_संख्या = -1;
#अन्यथा
    linenr_T	lnum;

    यदि (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    lnum = tv_get_lnum(argvars);
    यदि (lnum < 1 || lnum > curbuf->b_ml.ml_पंक्ति_गिनती + 1)
	rettv->vval.v_संख्या = -1;
    अन्यथा
	rettv->vval.v_संख्या = ml_find_पंक्ति_or_offset(curbuf, lnum, रिक्त);
    यदि (rettv->vval.v_संख्या >= 0)
	++rettv->vval.v_संख्या;
#endif
}

#ifdef FEAT_LUA
/*
 * "luaeval()" function
 */
    स्थिर शून्य
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    यदि (check_restricted() || check_secure())
	प्रत्यागम;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#endif

प्रकारनिर्देश गणना
{
    MATCH_END,	    // matchend()
    MATCH_MATCH,    // match()
    MATCH_STR,	    // matchstr()
    MATCH_LIST,	    // matchlist()
    MATCH_POS	    // matchstrpos()
} matchtype_T;

    स्थिर शून्य
find_some_match(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = रिक्त;
    दीर्घ	len = 0;
    char_u	*व्यंजक = रिक्त;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    दीर्घ	start = 0;
    दीर्घ	nth = 1;
    colnr_T	startcol = 0;
    पूर्णाङ्क		match = 0;
    list_T	*l = रिक्त;
    listitem_T	*li = रिक्त;
    दीर्घ	idx = 0;
    पूर्णाङ्क		prev_lock = 0;
    char_u	*tofree = रिक्त;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    rettv->vval.v_संख्या = -1;
    यदि (type == MATCH_LIST || type == MATCH_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	यदि (rettv_list_alloc(rettv) == FAIL)
	    गच्छ theend;
	यदि (type == MATCH_POS
		&& (list_append_string(rettv->vval.v_list,
					    (char_u *)"", 0) == FAIL
		    || list_append_संख्या(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_संख्या(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_संख्या(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL))
	{
		list_free(rettv->vval.v_list);
		rettv->vval.v_list = रिक्त;
		गच्छ theend;
	}
    }
    अन्यथा यदि (type == MATCH_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = रिक्त;
    }

    यदि (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 3) == FAIL)))
	गच्छ theend;

    यदि (argvars[0].v_type == VAR_LIST)
    {
	यदि ((l = argvars[0].vval.v_list) == रिक्त)
	    गच्छ theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    अन्यथा
    {
	व्यंजक = str = tv_get_string(&argvars[0]);
	len = (दीर्घ)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    यदि (pat == रिक्त)
	गच्छ theend;

    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	पूर्णाङ्क	    error = FALSE;

	start = (दीर्घ)tv_get_संख्या_chk(&argvars[2], &error);
	यदि (error)
	    गच्छ theend;
	यदि (l != रिक्त)
	{
	    li = list_find(l, start);
	    यदि (li == रिक्त)
		गच्छ theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	अन्यथा
	{
	    यदि (start < 0)
		start = 0;
	    यदि (start > len)
		गच्छ theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    यदि (argvars[3].v_type != VAR_UNKNOWN)
		startcol = start;
	    अन्यथा
	    {
		str += start;
		len -= start;
	    }
	}

	यदि (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (दीर्घ)tv_get_संख्या_chk(&argvars[3], &error);
	यदि (error)
	    गच्छ theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    यदि (regmatch.regprog != रिक्त)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	यदि (l != रिक्त)
	{
	    prev_lock = l->lv_lock;
	    यदि (l->lv_lock == 0)
		l->lv_lock = VAR_LOCKED;
	}

	प्रति (;;)
	{
	    यदि (l != रिक्त)
	    {
		यदि (li == रिक्त)
		{
		    match = FALSE;
		    भङ्ग;
		}
		vim_free(tofree);
		व्यंजक = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		यदि (str == रिक्त)
		    भङ्ग;
	    }

	    match = vim_regexec_nl(&regmatch, str, startcol);

	    यदि (match && --nth <= 0)
		भङ्ग;
	    यदि (l == रिक्त && !match)
		भङ्ग;

	    // Advance to just after the match.
	    यदि (l != रिक्त)
	    {
		li = li->li_next;
		++idx;
	    }
	    अन्यथा
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		यदि (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    match = FALSE;
		    भङ्ग;
		}
	    }
	}

	यदि (match)
	{
	    यदि (type == MATCH_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_first;
		listitem_T *li2 = li1->li_next;
		listitem_T *li3 = li2->li_next;
		listitem_T *li4 = li3->li_next;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_संख्या =
				      (varnumber_T)(regmatch.startp[0] - व्यंजक);
		li4->li_tv.vval.v_संख्या =
					(varnumber_T)(regmatch.endp[0] - व्यंजक);
		यदि (l != रिक्त)
		    li2->li_tv.vval.v_संख्या = (varnumber_T)idx;
	    }
	    अन्यथा यदि (type == MATCH_LIST)
	    {
		पूर्णाङ्क i;

		// return list with matched string and submatches
		प्रति (i = 0; i < NSUBEXP; ++i)
		{
		    यदि (regmatch.endp[i] == रिक्त)
		    {
			यदि (list_append_string(rettv->vval.v_list,
							    रिक्त, 0) == FAIL)
			    भङ्ग;
		    }
		    अन्यथा यदि (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(पूर्णाङ्क)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			भङ्ग;
		}
	    }
	    अन्यथा यदि (type == MATCH_STR)
	    {
		// return matched string
		यदि (l != रिक्त)
		    copy_tv(&li->li_tv, rettv);
		अन्यथा
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    अन्यथा यदि (l != रिक्त)
		rettv->vval.v_संख्या = idx;
	    अन्यथा
	    {
		यदि (type != MATCH_END)
		    rettv->vval.v_संख्या =
				      (varnumber_T)(regmatch.startp[0] - str);
		अन्यथा
		    rettv->vval.v_संख्या =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_संख्या += (varnumber_T)(str - व्यंजक);
	    }
	}
	यदि (l != रिक्त)
	    l->lv_lock = prev_lock;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    यदि (type == MATCH_POS && l == रिक्त && rettv->vval.v_list != रिक्त)
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
    स्थिर पूर्णाङ्क
get_matches_in_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    पूर्णाङ्क		idx,
    पूर्णाङ्क		submatches,
    पूर्णाङ्क		matchbuf)
{
    दीर्घ	len = (दीर्घ)STRLEN(str);
    पूर्णाङ्क		match = 0;
    colnr_T	startidx = 0;

    प्रति (;;)
    {
	match = vim_regexec_nl(rmp, str, startidx);
	यदि (!match)
	    भङ्ग;

	dict_T *d = dict_alloc();
	यदि (d == रिक्त)
	    प्रत्यागम FAIL;
	यदि (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    प्रत्यागम FAIL;
	}

	यदि (dict_add_संख्या(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    प्रत्यागम FAIL;

	यदि (dict_add_संख्या(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    प्रत्यागम FAIL;

	यदि (dict_add_string_len(d, "text", rmp->startp[0],
		    (पूर्णाङ्क)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    प्रत्यागम FAIL;

	यदि (submatches)
	{
	    list_T *sml = list_alloc();
	    यदि (sml == रिक्त)
		प्रत्यागम FAIL;

	    यदि (dict_add_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		प्रत्यागम FAIL;
	    }

	    // return a list with the submatches
	    प्रति (पूर्णाङ्क i = 1; i < NSUBEXP; ++i)
	    {
		यदि (rmp->endp[i] == रिक्त)
		{
		    यदि (list_append_string(sml, (char_u *)"", 0) == FAIL)
			प्रत्यागम FAIL;
		}
		अन्यथा यदि (list_append_string(sml, rmp->startp[i],
			    (पूर्णाङ्क)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    प्रत्यागम FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	यदि (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    भङ्ग;
    }

    प्रत्यागम OK;
}

/*
 * "matchbufline()" function
 */
    स्थिर शून्य
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = रिक्त;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_संख्या = -1;
    यदि (rettv_list_alloc(rettv) != OK)
	प्रत्यागम;
    retlist = rettv->vval.v_list;

    यदि (check_for_buffer_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	प्रत्यागम;

    पूर्णाङ्क prev_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    यदि (buf == रिक्त)
    {
	यदि (did_emsg == prev_did_emsg)
	    semsg(_(e_invalid_buffer_name_str), tv_get_string(&argvars[0]));
	प्रत्यागम;
    }
    यदि (buf->b_ml.ml_mfp == रिक्त)
    {
	emsg(_(e_buffer_is_not_loaded));
	प्रत्यागम;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    पूर्णाङ्क		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    यदि (did_emsg > did_emsg_before)
	प्रत्यागम;
    यदि (slnum < 1)
    {
	semsg(_(e_invalid_मूल्य_for_argument_str), "lnum");
	प्रत्यागम;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    यदि (did_emsg > did_emsg_before)
	प्रत्यागम;
    यदि (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_मूल्य_for_argument_str), "end_lnum");
	प्रत्यागम;
    }

    यदि (elnum > buf->b_ml.ml_पंक्ति_गिनती)
	elnum = buf->b_ml.ml_पंक्ति_गिनती;

    पूर्णाङ्क		submatches = FALSE;
    यदि (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	यदि (d != रिक्त)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    यदि (di != रिक्त)
	    {
		यदि (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "submatches");
		    प्रत्यागम;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    यदि (regmatch.regprog == रिक्त)
	गच्छ theend;
    regmatch.rm_ic = p_ic;

    यावत् (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	यदि (get_matches_in_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    गच्छ cleanup;
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
    स्थिर शून्य
f_match(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_MATCH);
}

/*
 * "matchend()" function
 */
    स्थिर शून्य
f_matchend(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_END);
}

/*
 * "matchlist()" function
 */
    स्थिर शून्य
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_LIST);
}

/*
 * "matchstr()" function
 */
    स्थिर शून्य
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_STR);
}

/*
 * "matchstrlist()" function
 */
    स्थिर शून्य
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = रिक्त;
    char_u	*save_cpo;
    list_T	*l = रिक्त;
    listitem_T	*li = रिक्त;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_संख्या = -1;
    यदि (rettv_list_alloc(rettv) != OK)
	प्रत्यागम;
    retlist = rettv->vval.v_list;

    यदि (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	प्रत्यागम;

    यदि ((l = argvars[0].vval.v_list) == रिक्त)
	प्रत्यागम;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    यदि (pat == रिक्त)
	प्रत्यागम;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    यदि (regmatch.regprog == रिक्त)
	गच्छ theend;
    regmatch.rm_ic = p_ic;

    पूर्णाङ्क		submatches = FALSE;
    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	यदि (d != रिक्त)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    यदि (di != रिक्त)
	    {
		यदि (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_मूल्य_for_argument_str), "submatches");
		    गच्छ cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    पूर्णाङ्क idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_वस्तुएँ(l, li)
    {
	यदि (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != रिक्त)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    यदि (get_matches_in_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		गच्छ cleanup;
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
    स्थिर शून्य
f_matchstrpos(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_POS);
}

    स्थिर शून्य
max_min(typval_T *argvars, typval_T *rettv, पूर्णाङ्क domax)
{
    varnumber_T	n = 0;

    यदि (in_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->vval.v_संख्या = 0;

    यदि (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = रिक्त;

	l = argvars[0].vval.v_list;
	यदि (l != रिक्त && l->lv_len > 0)
	{
	    यदि (l->lv_first == &range_list_item)
	    {
		यदि ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_start;
		अन्यथा
		    n = l->lv_u.nonmat.lv_start + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_संख्या = n;
	    }
	    अन्यथा
	    {
		FOR_ALL_LIST_वस्तुएँ(l, li)
		{
		    यदि (tv == रिक्त)
			tv = &li->li_tv;
		    अन्यथा
		    {
			पूर्णाङ्क res;
			यदि (typval_compare2(&li->li_tv, tv,
				domax ? व्यंजक_GREATER : व्यंजक_SMALLER, FALSE, &res) == FAIL)
			    प्रत्यागम;
			यदि (res == OK)
			    tv = &li->li_tv;
		    }
		}

		यदि (tv != रिक्त)
		    copy_tv(tv, rettv);
	    }
	}
    }
    अन्यथा यदि (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	यदि (tv != रिक्त)
	    copy_tv(tv, rettv);
    }
    अन्यथा यदि (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	पूर्णाङ्क		todo;
	typval_T	*tv = रिक्त;

	d = argvars[0].vval.v_dict;
	यदि (d != रिक्त)
	{
	    todo = (पूर्णाङ्क)d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_वस्तुएँ(&d->dv_hashtab, hi, todo)
	    {
		यदि (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    यदि (tv == रिक्त)
			tv = &HI2DI(hi)->di_tv;
		    अन्यथा
		    {
			पूर्णाङ्क res;
			यदि (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? व्यंजक_GREATER : व्यंजक_SMALLER, FALSE, &res) == FAIL)
			    प्रत्यागम;
			यदि (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	यदि (tv != रिक्त)
	    copy_tv(tv, rettv);
    }
    अन्यथा
	semsg(_(e_argument_of_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
}

/*
 * "max()" function
 */
    स्थिर शून्य
f_max(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, TRUE);
}

/*
 * "min()" function
 */
    स्थिर शून्य
f_min(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, FALSE);
}

#यदि defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    स्थिर शून्य
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    यदि (check_restricted() || check_secure())
	प्रत्यागम;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    शून्य
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
    स्थिर शून्य
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    यदि (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    प्रति (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	यदि (lnum < 0 || lnum > curbuf->b_ml.ml_पंक्ति_गिनती)
	{
	    lnum = 0;
	    भङ्ग;
	}
	यदि (*skipwhite(ml_get(lnum)) != NUL)
	    भङ्ग;
    }
    rettv->vval.v_संख्या = lnum;
}


/*
 * "ngettext()" function
 */
    स्थिर शून्य
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#यदि defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    वर्ण *prev = रिक्त;
#endif

    यदि (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_संख्या_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_STRING;

    यदि (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != रिक्त &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#यदि defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ध्रुव वर्ण *)argvars[3].vval.v_string, (वर्ण *)p_enc);
#endif

#यदि defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((ध्रुव वर्ण *)argvars[3].vval.v_string, (ध्रुव वर्ण *)argvars[0].vval.v_string, (ध्रुव वर्ण *)argvars[1].vval.v_string, (पूर्णाङ्क)argvars[2].vval.v_संख्या));
#अन्यथा
	textdomain((ध्रुव वर्ण *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ध्रुव वर्ण *)argvars[0].vval.v_string, (ध्रुव वर्ण *)argvars[1].vval.v_string, argvars[2].vval.v_संख्या));
	textdomain(VIMPACKAGE);
#endif

#यदि defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	यदि (prev != रिक्त)
	    bind_textdomain_codeset((ध्रुव वर्ण *)argvars[3].vval.v_string, prev);
#endif
    }
    अन्यथा
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ध्रुव वर्ण *)argvars[0].vval.v_string, (ध्रुव वर्ण *)argvars[1].vval.v_string, argvars[2].vval.v_संख्या));
}


/*
 * "nr2char()" function
 */
    स्थिर शून्य
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    यदि (has_mbyte)
    {
	पूर्णाङ्क	utf8 = 0;

	यदि (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (पूर्णाङ्क)tv_get_bool_chk(&argvars[1], रिक्त);
	यदि (utf8)
	    buf[utf_char2bytes((पूर्णाङ्क)tv_get_संख्या(&argvars[0]), buf)] = NUL;
	अन्यथा
	    buf[(*mb_char2bytes)((पूर्णाङ्क)tv_get_संख्या(&argvars[0]), buf)] = NUL;
    }
    अन्यथा
    {
	buf[0] = (char_u)tv_get_संख्या(&argvars[0]);
	buf[1] = NUL;
    }
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "or(expr, expr)" function
 */
    स्थिर शून्य
f_or(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    rettv->vval.v_संख्या = tv_get_संख्या_chk(&argvars[0], रिक्त)
					| tv_get_संख्या_chk(&argvars[1], रिक्त);
}

#ifdef FEAT_PERL
/*
 * "perleval()" function
 */
    स्थिर शून्य
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    यदि (check_restricted() || check_secure())
	प्रत्यागम;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#endif

/*
 * "prevnonblank()" function
 */
    स्थिर शून्य
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    यदि (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    lnum = tv_get_lnum(argvars);
    यदि (lnum < 1 || lnum > curbuf->b_ml.ml_पंक्ति_गिनती)
	lnum = 0;
    अन्यथा
	यावत् (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
	    --lnum;
    rettv->vval.v_संख्या = lnum;
}

// This dummy va_list is here because:
// - passing a NULL pointer doesn't work when va_list isn't a pointer
// - locally in the function results in a "used before set" warning
// - using va_start() to initialize it gives "function with fixed args" error
स्थिर बहु_सूची	ap;

/*
 * "printf()" function
 */
    स्थिर शून्य
f_printf(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    पूर्णाङ्क		len;
    char_u	*s;
    पूर्णाङ्क		saved_did_emsg = did_emsg;
    वर्ण	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    यदि (in_vim9script() && check_for_string_or_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (वर्ण *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(रिक्त, 0, fmt, ap, argvars + 1);
    यदि (!did_emsg)
    {
	s = alloc(len + 1);
	यदि (s != रिक्त)
	{
	    rettv->vval.v_string = s;
	    (शून्य)vim_vsnprintf_typval((वर्ण *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    स्थिर शून्य
f_pum_getpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    यदि (rettv_dict_alloc(rettv) == FAIL)
	प्रत्यागम;
    pum_set_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    स्थिर शून्य
f_pumvisible(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    यदि (pum_visible())
	rettv->vval.v_संख्या = 1;
}

#ifdef FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    स्थिर शून्य
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    यदि (check_restricted() || check_secure())
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    यदि (p_pyx == 0)
	p_pyx = 3;

    यदि (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    अन्यथा यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	प्रत्यागम;
    }
    अन्यथा
    {
	locals = रिक्त;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#endif

#ifdef FEAT_PYTHON
/*
 * "pyeval()" function
 */
    स्थिर शून्य
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    यदि (check_restricted() || check_secure())
	प्रत्यागम;

    यदि (in_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	प्रत्यागम;

    यदि (p_pyx == 0)
	p_pyx = 2;

    यदि (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    अन्यथा यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	प्रत्यागम;
    }
    अन्यथा
    {
	locals = रिक्त;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#endif

#यदि defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    स्थिर शून्य
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    यदि (check_restricted() || check_secure())
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

# यदि defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    यदि (p_pyx == 2)
	f_pyeval(argvars, rettv);
    अन्यथा
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#endif

स्थिर UINT32_T srand_seed_for_testing = 0;
स्थिर पूर्णाङ्क	srand_seed_for_testing_is_used = FALSE;

    स्थिर शून्य
f_test_srand_seed(typval_T *argvars, typval_T *rettv UNUSED)
{
    यदि (in_vim9script() && check_for_opt_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_UNKNOWN)
	srand_seed_for_testing_is_used = FALSE;
    अन्यथा
    {
	srand_seed_for_testing = (UINT32_T)tv_get_संख्या(&argvars[0]);
	srand_seed_for_testing_is_used = TRUE;
    }
}

    स्थिर शून्य
init_srand(UINT32_T *x)
{
    संरचना {
	सङ्घ {
	    UINT32_T संख्या;
	    char_u   bytes[परिमाण(UINT32_T)];
	} contents;
    } buf;

    यदि (srand_seed_for_testing_is_used)
    {
	*x = srand_seed_for_testing;
	प्रत्यागम;
    }

    यदि (mch_get_random(buf.contents.bytes, परिमाण(buf.contents.bytes)) == OK)
    {
	*x = buf.contents.संख्या;
	प्रत्यागम;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#यदि defined(FEAT_SODIUM)
    यदि (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    अन्यथा
#endif
    {
#यदि defined(FEAT_RELTIME)
	proftime_T res;
	profile_start(&res);
# यदि defined(MSWIN)
	*x = (UINT32_T)res.निचला_Part;
# अन्यथा
	*x = (UINT32_T)res.tv_fsec;
# endif
#अन्यथा
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
    स्थिर शून्य
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = रिक्त;
    स्थिर UINT32_T	gx, gy, gz, gw;
    स्थिर पूर्णाङ्क	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, result;

    यदि (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_UNKNOWN)
    {
	// When no argument is given use the global seed list.
	यदि (initialized == FALSE)
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
    अन्यथा यदि (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	यदि (l == रिक्त || list_len(l) != 4)
	    गच्छ theend;

	lx = list_find(l, 0L);
	ly = list_find(l, 1L);
	lz = list_find(l, 2L);
	lw = list_find(l, 3L);
	यदि (lx == रिक्त || ly == रिक्त || lz == रिक्त || lw == रिक्त)
	    गच्छ theend;
	यदि (lx->li_tv.v_type != VAR_संख्या
		|| ly->li_tv.v_type != VAR_संख्या
		|| lz->li_tv.v_type != VAR_संख्या
		|| lw->li_tv.v_type != VAR_संख्या)
	    गच्छ theend;
	x = (UINT32_T)lx->li_tv.vval.v_संख्या;
	y = (UINT32_T)ly->li_tv.vval.v_संख्या;
	z = (UINT32_T)lz->li_tv.vval.v_संख्या;
	w = (UINT32_T)lw->li_tv.vval.v_संख्या;

	SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w);

	lx->li_tv.vval.v_संख्या = (varnumber_T)x;
	ly->li_tv.vval.v_संख्या = (varnumber_T)y;
	lz->li_tv.vval.v_संख्या = (varnumber_T)z;
	lw->li_tv.vval.v_संख्या = (varnumber_T)w;
    }
    अन्यथा
	गच्छ theend;

    rettv->v_type = VAR_संख्या;
    rettv->vval.v_संख्या = (varnumber_T)result;
    प्रत्यागम;

theend:
    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    rettv->v_type = VAR_संख्या;
    rettv->vval.v_संख्या = -1;
}

/*
 * "srand()" function
 */
    स्थिर शून्य
f_srand(typval_T *argvars, typval_T *rettv)
{
    UINT32_T x = 0, z;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script() && check_for_opt_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    अन्यथा
    {
	पूर्णाङ्क	    error = FALSE;

	x = (UINT32_T)tv_get_संख्या_chk(&argvars[0], &error);
	यदि (error)
	    प्रत्यागम;
    }

    list_append_संख्या(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_संख्या(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_संख्या(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_संख्या(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#undef ROTL
#undef SPLITMIX32
#undef SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    स्थिर शून्य
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	start;
    varnumber_T	end;
    varnumber_T	stride = 1;
    पूर्णाङ्क		error = FALSE;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 2) == FAIL)))
	प्रत्यागम;

    start = tv_get_संख्या_chk(&argvars[0], &error);
    यदि (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = start - 1;
	start = 0;
    }
    अन्यथा
    {
	end = tv_get_संख्या_chk(&argvars[1], &error);
	यदि (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_संख्या_chk(&argvars[2], &error);
    }

    यदि (error)
	प्रत्यागम;		// type error; errmsg already given
    यदि (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	प्रत्यागम;
    }

    // The stride is stored in "lv_stride", which is an int.
    यदि (stride < पूर्णांक_न्यून || stride > पूर्णांक_उच्च)
    {
	वर्ण	buf[NUMBUFLEN];

	vim_snprintf(buf, परिमाण(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	प्रत्यागम;
    }

    uvarnumber_T	len;

    यदि (stride > 0 ? end < start : end > start)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	back = stride > 0
				  ? (uvarnumber_T)start - (uvarnumber_T)end
				  : (uvarnumber_T)end - (uvarnumber_T)start;

	यदि (back > 1)
	{
	    emsg(_(e_start_past_end));
	    प्रत्यागम;
	}
	len = 0;
    }
    अन्यथा
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	span = stride > 0
				  ? (uvarnumber_T)end - (uvarnumber_T)start
				  : (uvarnumber_T)start - (uvarnumber_T)end;
	uvarnumber_T	गिनती = span / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	यदि (गिनती >= (uvarnumber_T)पूर्णांक_उच्च)
	{
	    वर्ण	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, परिमाण(buf), "%llu",
			      गिनती < UVARNUM_MAX ? गिनती + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    प्रत्यागम;
	}
	len = गिनती + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_first = &range_list_item;
    list->lv_u.nonmat.lv_start = start;
    list->lv_u.nonmat.lv_end = end;
    list->lv_u.nonmat.lv_stride = (पूर्णाङ्क)stride;
    list->lv_len = (पूर्णाङ्क)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    शून्य
range_list_materialize(list_T *list)
{
    varnumber_T start = list->lv_u.nonmat.lv_start;
    varnumber_T end = list->lv_u.nonmat.lv_end;
    पूर्णाङ्क		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_first = रिक्त;
    list->lv_u.mat.lv_last = रिक्त;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_item = रिक्त;
    प्रति (i = start; stride > 0 ? i <= end : i >= end; i += stride)
    {
	यदि (list_append_संख्या(list, i) == FAIL)
	    भङ्ग;
	यदि (list->lv_lock & VAR_वस्तुएँ_LOCKED)
	    list->lv_u.mat.lv_last->li_tv.v_lock = VAR_LOCKED;
    }
    list->lv_lock &= ~VAR_वस्तुएँ_LOCKED;
}

/*
 * "getreginfo()" function
 */
    स्थिर शून्य
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		regname;
    char_u	buf[NUMBUFLEN + 2];
    माप_प्रकार	buflen;
    दीर्घ	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    यदि (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    regname = getreg_get_regname(argvars);
    यदि (regname == 0)
	प्रत्यागम;

    यदि (regname == '@')
	regname = '"';

    यदि (rettv_dict_alloc(rettv) == FAIL)
	प्रत्यागम;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_व्यंजक_SRC | GREG_LIST);
    यदि (list == रिक्त)
	प्रत्यागम;
    यदि (dict_add_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    विकल्प (get_reg_type(regname, &reglen))
    {
	पक्ष MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    भङ्ग;
	पक्ष MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    भङ्ग;
	पक्ष MBLOCK:
	    buflen = vim_snprintf_safelen((वर्ण *)buf, परिमाण(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    भङ्ग;
	स्वभाव:
	    buf[0] = NUL;
	    buflen = 0;
	    भङ्ग;
    }
    (शून्य)dict_add_string_len(dict, (वर्ण *)"regtype", buf, (पूर्णाङ्क)buflen);

    buf[0] = get_register_name(get_unname_register());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    यदि (regname == '"')
	(शून्य)dict_add_string_len(dict, (वर्ण *)"points_to", buf, (पूर्णाङ्क)buflen);
    अन्यथा
    {
	dictitem_T	*item = dictitem_alloc((char_u *)"isunnamed");

	यदि (item != रिक्त)
	{
	    item->di_tv.v_type = VAR_BOOL;
	    item->di_tv.vval.v_संख्या = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (शून्य)dict_add(dict, item);
	}
    }
}

    स्थिर शून्य
return_register(पूर्णाङ्क regname, typval_T *rettv)
{
    char_u buf[2] = {NUL, NUL};

    buf[0] = (char_u)regname;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(buf, (buf[0] == NUL) ? 0 : 1);
}

/*
 * "reg_executing()" function
 */
    स्थिर शून्य
f_reg_executing(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    स्थिर शून्य
f_reg_recording(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    स्थिर शून्य
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_संख्या = -1;
    यदि (check_restricted() || check_secure())
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    rettv->vval.v_संख्या = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    स्थिर शून्य
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    पूर्णाङ्क slen = list_len(l);

    यदि (rettv_list_alloc(rettv) == FAIL
	    || l == रिक्त
	    || n <= 0
	    || slen == 0)
	प्रत्यागम;

    यदि (check_repeat_गिनती(slen, n) == FAIL)
	प्रत्यागम;

    यावत् (n-- > 0)
	यदि (list_extend(rettv->vval.v_list, l, रिक्त) == FAIL)
	    भङ्ग;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    स्थिर शून्य
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    पूर्णाङ्क		slen;
    पूर्णाङ्क		len;
    पूर्णाङ्क		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    यदि (rettv_blob_alloc(rettv) == FAIL
	    || blob == रिक्त
	    || n <= 0)
	प्रत्यागम;

    slen = blob->bv_ga.ga_len;
    यदि (slen <= 0)
	प्रत्यागम;
    यदि (check_repeat_गिनती(slen, n) == FAIL)
	प्रत्यागम;
    len = slen * (पूर्णाङ्क)n;

    यदि (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	प्रत्यागम;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    प्रति (i = 0; i < slen; ++i)
	यदि (blob_get(blob, i) != 0)
	    भङ्ग;

    यदि (i == slen)
	// No need to copy since all bytes are already zero
	प्रत्यागम;

    प्रति (i = 0; i < (पूर्णाङ्क)n; ++i)
	blob_set_range(rettv->vval.v_blob,
		(दीर्घ)i * slen, ((दीर्घ)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    स्थिर शून्य
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    माप_प्रकार	slen;
    पूर्णाङ्क		len;
    char_u	*r;
    पूर्णाङ्क		done;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    slen = STRLEN(p);
    यदि (slen == 0 || n <= 0)
	प्रत्यागम;
    यदि (check_repeat_गिनती((varnumber_T)slen, n) == FAIL)
	प्रत्यागम;
    len = (पूर्णाङ्क)slen * (पूर्णाङ्क)n;

    r = alloc((माप_प्रकार)len + 1);
    यदि (r == रिक्त)
	प्रत्यागम;

    mch_memmove(r, p, slen);
    done = (पूर्णाङ्क)slen;
    यावत् (done < len)
    {
	पूर्णाङ्क copy_len = done;

	यदि (copy_len > len - done)
	    copy_len = len - done;
	mch_memmove(r + done, r, (माप_प्रकार)copy_len);
	done += copy_len;
    }
    r[len] = NUL;

    rettv->vval.v_string = r;
}

/*
 * "repeat()" function
 */
    स्थिर शून्य
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    यदि (in_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    n = tv_get_संख्या(&argvars[1]);
    यदि (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    अन्यथा
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
    स्थिर पूर्णाङ्क
get_खोज_arg(typval_T *varp, पूर्णाङ्क *flagsp)
{
    पूर्णाङ्क		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    पूर्णाङ्क		mask;

    यदि (varp->v_type == VAR_UNKNOWN)
	प्रत्यागम FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    यदि (flags == रिक्त)
	प्रत्यागम 0;		// type error; errmsg already given
    यावत् (*flags != NUL)
    {
	विकल्प (*flags)
	{
	    पक्ष 'b': dir = BACKWARD; भङ्ग;
	    पक्ष 'w': p_ws = TRUE; भङ्ग;
	    पक्ष 'W': p_ws = FALSE; भङ्ग;
	    स्वभाव:  mask = 0;
		      यदि (flagsp != रिक्त)
			  विकल्प (*flags)
			  {
			      पक्ष 'c': mask = SP_START; भङ्ग;
			      पक्ष 'e': mask = SP_END; भङ्ग;
			      पक्ष 'm': mask = SP_RETCOUNT; भङ्ग;
			      पक्ष 'n': mask = SP_NOMOVE; भङ्ग;
			      पक्ष 'p': mask = SP_SUBPAT; भङ्ग;
			      पक्ष 'r': mask = SP_REPEAT; भङ्ग;
			      पक्ष 's': mask = SP_SETPCMARK; भङ्ग;
			      पक्ष 'z': mask = SP_COLUMN; भङ्ग;
			  }
		      यदि (mask == 0)
		      {
			  semsg(_(e_invalid_argument_str), flags);
			  dir = 0;
		      }
		      अन्यथा
			  *flagsp |= mask;
	}
	यदि (dir == 0)
	    भङ्ग;
	++flags;
    }
    प्रत्यागम dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    स्थिर पूर्णाङ्क
खोज_cmn(typval_T *argvars, pos_T *match_pos, पूर्णाङ्क *flagsp)
{
    पूर्णाङ्क		flags;
    char_u	*pat;
    माप_प्रकार	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    पूर्णाङ्क		save_p_ws = p_ws;
    पूर्णाङ्क		dir;
    पूर्णाङ्क		retval = 0;	// default: FAIL
    दीर्घ	lnum_stop = 0;
#ifdef FEAT_RELTIME
    दीर्घ	time_limit = 0;
#endif
    पूर्णाङ्क		options = खोज_KEEP;
    पूर्णाङ्क		subpatnum;
    searchit_arg_T sia;
    पूर्णाङ्क		use_skip = FALSE;
    pos_T	firstpos;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_संख्या_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_संख्या_arg(argvars, 3) == FAIL)))))
	गच्छ theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_खोज_arg(&argvars[1], flagsp);	// may set p_ws
    यदि (dir == 0)
	गच्छ theend;
    flags = *flagsp;
    यदि (flags & SP_START)
	options |= खोज_START;
    यदि (flags & SP_END)
	options |= खोज_END;
    यदि (flags & SP_COLUMN)
	options |= खोज_COL;

    // Optional arguments: line number to stop searching, timeout and skip.
    यदि (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (दीर्घ)tv_get_संख्या_chk(&argvars[2], रिक्त);
	यदि (lnum_stop < 0)
	    गच्छ theend;
	यदि (argvars[3].v_type != VAR_UNKNOWN)
	{
#ifdef FEAT_RELTIME
	    time_limit = (दीर्घ)tv_get_संख्या_chk(&argvars[3], रिक्त);
	    यदि (time_limit < 0)
		गच्छ theend;
#endif
	    use_skip = eval_व्यंजक_valid_arg(&argvars[4]);
	}
    }

    /*
     * This function does not accept SP_REPEAT and SP_RETCOUNT flags.
     * Check to make sure only those flags are set.
     * Also, Only the SP_NOMOVE or the SP_SETPCMARK flag can be set. Both
     * flags cannot be set. Check for that condition also.
     */
    यदि (((flags & (SP_REPEAT | SP_RETCOUNT)) != 0)
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	गच्छ theend;
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
    प्रति (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, रिक्त, dir, pat, patlen, 1L,
						     options, RE_खोज, &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	यदि (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	यदि (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    भङ्ग;
	यदि (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    पूर्णाङ्क	    do_skip;
	    पूर्णाङ्क	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_व्यंजक_to_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    यदि (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		भङ्ग;
	    }
	    यदि (!do_skip)
		भङ्ग;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~खोज_START;
    }

    यदि (subpatnum != FAIL)
    {
	यदि (flags & SP_SUBPAT)
	    retval = subpatnum;
	अन्यथा
	    retval = pos.lnum;
	यदि (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	यदि (match_pos != रिक्त)
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
    यदि (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    अन्यथा
	curwin->w_set_curswant = true;
theend:
    p_ws = save_p_ws;

    प्रत्यागम retval;
}

#ifdef FEAT_RUBY
/*
 * "rubyeval()" function
 */
    स्थिर शून्य
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    यदि (check_restricted() || check_secure())
	प्रत्यागम;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#endif

/*
 * "screenattr()" function
 */
    स्थिर शून्य
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		row;
    पूर्णाङ्क		col;
    पूर्णाङ्क		c;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    row = (पूर्णाङ्क)tv_get_संख्या_chk(&argvars[0], रिक्त) - 1;
    col = (पूर्णाङ्क)tv_get_संख्या_chk(&argvars[1], रिक्त) - 1;
    यदि (row < 0 || row >= screen_Rows
	    || col < 0 || col >= screen_Columns)
	c = -1;
    अन्यथा
	c = ScreenAttrs[पंक्ति_Offset[row] + col];
    rettv->vval.v_संख्या = c;
}

/*
 * "screenchar()" function
 */
    स्थिर शून्य
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		row;
    पूर्णाङ्क		col;
    पूर्णाङ्क		c;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    row = (पूर्णाङ्क)tv_get_संख्या_chk(&argvars[0], रिक्त) - 1;
    col = (पूर्णाङ्क)tv_get_संख्या_chk(&argvars[1], रिक्त) - 1;
    यदि (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	c = -1;
    अन्यथा
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(row, col, buf, रिक्त);
	c = (*mb_ptr2char)(buf);
    }
    rettv->vval.v_संख्या = c;
}

/*
 * "screenchars()" function
 */
    स्थिर शून्य
f_screenchars(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		row;
    पूर्णाङ्क		col;
    पूर्णाङ्क		c;
    पूर्णाङ्क		i;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    row = (पूर्णाङ्क)tv_get_संख्या_chk(&argvars[0], रिक्त) - 1;
    col = (पूर्णाङ्क)tv_get_संख्या_chk(&argvars[1], रिक्त) - 1;
    यदि (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	प्रत्यागम;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(row, col, buf, रिक्त);
    पूर्णाङ्क pcc[MAX_MCO];
    यदि (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    अन्यथा
	c = (*mb_ptr2char)(buf);
    list_append_संख्या(rettv->vval.v_list, (varnumber_T)c);

    यदि (enc_utf8)
	प्रति (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_संख्या(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    स्थिर शून्य
f_screencol(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_संख्या = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    स्थिर शून्य
f_screenrow(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_संख्या = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    स्थिर शून्य
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		row;
    पूर्णाङ्क		col;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = रिक्त;
    rettv->v_type = VAR_STRING;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    row = (पूर्णाङ्क)tv_get_संख्या_chk(&argvars[0], रिक्त) - 1;
    col = (पूर्णाङ्क)tv_get_संख्या_chk(&argvars[1], रिक्त) - 1;
    यदि (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	प्रत्यागम;

    screen_getbytes(row, col, buf, रिक्त);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    स्थिर शून्य
f_खोज(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		flags = 0;

    rettv->vval.v_संख्या = खोज_cmn(argvars, रिक्त, &flags);
}

/*
 * "searchdecl()" function
 */
    स्थिर शून्य
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		locally = TRUE;
    पूर्णाङ्क		thisblock = FALSE;
    पूर्णाङ्क		error = FALSE;
    char_u	*name;

    rettv->vval.v_संख्या = 1;	// default: FAIL

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	प्रत्यागम;

    name = tv_get_string_chk(&argvars[0]);
    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(पूर्णाङ्क)tv_get_bool_chk(&argvars[1], &error);
	यदि (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (पूर्णाङ्क)tv_get_bool_chk(&argvars[2], &error);
    }
    यदि (!error && name != रिक्त)
	rettv->vval.v_संख्या = find_decl(name, (पूर्णाङ्क)STRLEN(name),
				     locally, thisblock, खोज_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    स्थिर पूर्णाङ्क
searchpair_cmn(typval_T *argvars, pos_T *match_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    पूर्णाङ्क		save_p_ws = p_ws;
    पूर्णाङ्क		dir;
    पूर्णाङ्क		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    पूर्णाङ्क		retval = 0;		// default: FAIL
    दीर्घ	lnum_stop = 0;
    दीर्घ	time_limit = 0;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_संख्या_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_संख्या_arg(argvars, 6) == FAIL)))))
	गच्छ theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    यदि (spat == रिक्त || mpat == रिक्त || epat == रिक्त)
	गच्छ theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_खोज_arg(&argvars[3], &flags); // may set p_ws
    यदि (dir == 0)
	गच्छ theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    यदि ((flags & (SP_END | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[3]));
	गच्छ theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    यदि (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    यदि (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = रिक्त;
    अन्यथा
    {
	// Type is checked later.
	skip = &argvars[4];

	यदि (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (दीर्घ)tv_get_संख्या_chk(&argvars[5], रिक्त);
	    यदि (lnum_stop < 0)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[5]));
		गच्छ theend;
	    }
#ifdef FEAT_RELTIME
	    यदि (argvars[6].v_type != VAR_UNKNOWN)
	    {
		time_limit = (दीर्घ)tv_get_संख्या_chk(&argvars[6], रिक्त);
		यदि (time_limit < 0)
		{
		    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[6]));
		    गच्छ theend;
		}
	    }
#endif
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    match_pos, lnum_stop, time_limit);

theend:
    p_ws = save_p_ws;

    प्रत्यागम retval;
}

/*
 * "searchpair()" function
 */
    स्थिर शून्य
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_संख्या = searchpair_cmn(argvars, रिक्त);
}

/*
 * "searchpairpos()" function
 */
    स्थिर शून्य
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    पूर्णाङ्क		lnum = 0;
    पूर्णाङ्क		col = 0;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (searchpair_cmn(argvars, &match_pos) > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_संख्या(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_संख्या(rettv->vval.v_list, (varnumber_T)col);
}

/*
 * Search for a start/middle/end thing.
 * Used by searchpair(), see its documentation for the details.
 * Returns 0 or -1 for no match,
 */
    दीर्घ
do_searchpair(
    char_u	*spat,	    // start pattern
    char_u	*mpat,	    // middle pattern
    char_u	*epat,	    // end pattern
    पूर्णाङ्क		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    पूर्णाङ्क		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*match_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    दीर्घ	time_limit UNUSED) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = रिक्त, *pat3 = रिक्त;
    माप_प्रकार	patlen;
    माप_प्रकार	spatlen;
    माप_प्रकार	epatlen;
    माप_प्रकार	pat2size;
    माप_प्रकार	pat2len;
    माप_प्रकार	pat3size;
    माप_प्रकार	pat3len;
    दीर्घ	retval = 0;
    pos_T	pos;
    pos_T	firstpos;
    pos_T	foundpos;
    pos_T	save_cursor;
    pos_T	save_pos;
    पूर्णाङ्क		n;
    पूर्णाङ्क		r;
    पूर्णाङ्क		nest = 1;
    पूर्णाङ्क		use_skip = FALSE;
    पूर्णाङ्क		err;
    पूर्णाङ्क		options = खोज_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    यदि (pat2 == रिक्त)
	गच्छ theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    यदि (pat3 == रिक्त)
	गच्छ theend;
    pat2len = vim_snprintf((वर्ण *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    यदि (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    अन्यथा
	pat3len = vim_snprintf((वर्ण *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    यदि (flags & SP_START)
	options |= खोज_START;

    यदि (skip != रिक्त)
	use_skip = eval_व्यंजक_valid_arg(skip);

#ifdef FEAT_RELTIME
    यदि (time_limit > 0)
	init_regexp_timeout(time_limit);
#endif
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    प्रति (;;)
    {
	searchit_arg_T sia;

	CLEAR_FIELD(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, रिक्त, dir, pat, patlen, 1L,
						     options, RE_खोज, &sia);
	यदि (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    भङ्ग;

	यदि (firstpos.lnum == 0)
	    firstpos = pos;
	यदि (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    यदि (dir == BACKWARD)
		decl(&pos);
	    अन्यथा
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~खोज_START;

	// If the skip pattern matches, ignore this match.
	यदि (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_व्यंजक_to_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    यदि (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		भङ्ग;
	    }
	    यदि (r)
		अनुवर्त;
	}

	यदि ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	अन्यथा
	{
	    // Found end when searching forward or start when searching
	    // backward: end of (nested) pair; or found middle in outer pair.
	    यदि (--nest == 1)
		pat = pat3;	// outer level, search for middle
	}

	यदि (nest == 0)
	{
	    // Found the match: return matchcount or line number.
	    यदि (flags & SP_RETCOUNT)
		++retval;
	    अन्यथा
		retval = pos.lnum;
	    यदि (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    यदि (!(flags & SP_REPEAT))
		भङ्ग;
	    nest = 1;	    // search for next unmatched
	}
    }

    यदि (match_pos != रिक्त)
    {
	// Store the match cursor position
	match_pos->lnum = curwin->w_cursor.lnum;
	match_pos->col = curwin->w_cursor.col + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    यदि ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#ifdef FEAT_RELTIME
    यदि (time_limit > 0)
	disable_regexp_timeout();
#endif
    vim_free(pat2);
    vim_free(pat3);
    यदि (p_cpo == empty_option)
	p_cpo = save_cpo;
    अन्यथा
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	यदि (*p_cpo == NUL)
	    set_option_मूल्य_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    प्रत्यागम retval;
}

/*
 * "searchpos()" function
 */
    स्थिर शून्य
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    पूर्णाङ्क		lnum = 0;
    पूर्णाङ्क		col = 0;
    पूर्णाङ्क		n;
    पूर्णाङ्क		flags = 0;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    n = खोज_cmn(argvars, &match_pos, &flags);
    यदि (n > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_संख्या(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_संख्या(rettv->vval.v_list, (varnumber_T)col);
    यदि (flags & SP_SUBPAT)
	list_append_संख्या(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    स्थिर शून्य
set_position(typval_T *argvars, typval_T *rettv, पूर्णाङ्क charpos)
{
    pos_T	pos;
    पूर्णाङ्क		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_संख्या = -1;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    name = tv_get_string_chk(argvars);
    यदि (name == रिक्त)
	प्रत्यागम;

    यदि (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	प्रत्यागम;

    यदि (pos.col != MAXCOL && --pos.col < 0)
	pos.col = 0;
    यदि ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	यदि (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_set_curswant = false;
	}
	check_cursor();
	rettv->vval.v_संख्या = 0;
    }
    अन्यथा यदि (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	यदि (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_संख्या = 0;
    }
    अन्यथा
	emsg(_(e_invalid_argument));
}
/*
 * "setcharpos()" function
 */
    स्थिर शून्य
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, TRUE);
}

    स्थिर शून्य
f_setcharsearch(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    यदि (check_for_dict_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि ((d = argvars[0].vval.v_dict) == रिक्त)
	प्रत्यागम;

    csearch = dict_get_string(d, "char", FALSE);
    यदि (csearch != रिक्त)
    {
	यदि (enc_utf8)
	{
	    पूर्णाङ्क pcc[MAX_MCO];
	    पूर्णाङ्क c = utfc_ptr2char(csearch, pcc);

	    set_last_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	अन्यथा
	    set_last_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_find(d, (char_u *)"forward", -1);
    यदि (di != रिक्त)
	set_csearch_direction((पूर्णाङ्क)tv_get_संख्या(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_find(d, (char_u *)"until", -1);
    यदि (di != रिक्त)
	set_csearch_until(!!tv_get_संख्या(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    स्थिर शून्य
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    स्थिर शून्य
f_setenv(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    यदि (check_restricted() || check_secure())
	प्रत्यागम;

    name = tv_get_string_buf(&argvars[0], namebuf);
    यदि (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_संख्या == VVAL_NULL)
	vim_unsetenv_ext(name);
    अन्यथा
	vim_setenv_ext(name, tv_get_string_buf(&argvars[1], valbuf));
}

/*
 * "setfperm({fname}, {mode})" function
 */
    स्थिर शून्य
f_setfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	modebuf[NUMBUFLEN];
    char_u	*mode_str;
    पूर्णाङ्क		i;
    पूर्णाङ्क		mask;
    पूर्णाङ्क		mode = 0;

    rettv->vval.v_संख्या = 0;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    fname = tv_get_string_chk(&argvars[0]);
    यदि (fname == रिक्त)
	प्रत्यागम;
    mode_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    यदि (mode_str == रिक्त)
	प्रत्यागम;
    यदि (STRLEN(mode_str) != 9)
    {
	semsg(_(e_invalid_argument_str), mode_str);
	प्रत्यागम;
    }

    mask = 1;
    प्रति (i = 8; i >= 0; --i)
    {
	यदि (mode_str[i] != '-')
	    mode |= mask;
	mask = mask << 1;
    }
    rettv->vval.v_संख्या = mch_setperm(fname, mode) == OK;
}

/*
 * "setpos()" function
 */
    स्थिर शून्य
f_setpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    पूर्णाङ्क
get_yank_type(char_u **pp, char_u *yank_type, दीर्घ *block_len)
{
    char_u *stropt = *pp;
    विकल्प (*stropt)
    {
	पक्ष 'v': पक्ष 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    भङ्ग;
	पक्ष 'V': पक्ष 'l':	// line-wise selection
	    *yank_type = MLINE;
	    भङ्ग;
	पक्ष 'b': पक्ष Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    यदि (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*block_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    भङ्ग;
	स्वभाव:
	    प्रत्यागम FAIL;
    }
    *pp = stropt;
    प्रत्यागम OK;
}

/*
 * "setreg()" function
 */
    स्थिर शून्य
f_setreg(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    पूर्णाङ्क		append;
    char_u	yank_type;
    दीर्घ	block_len;
    typval_T	*regcontents;
    पूर्णाङ्क		pointreg;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    pointreg = 0;
    regcontents = रिक्त;
    block_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_संख्या = 1;		// FAIL is default

    यदि (strregname == रिक्त)
	प्रत्यागम;		// type error; errmsg already given
    यदि (in_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_register_name_must_be_one_char_str), strregname);
	प्रत्यागम;
    }
    regname = *strregname;
    यदि (regname == 0 || regname == '@')
	regname = '"';

    यदि (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	यदि (d == रिक्त || d->dv_hashtab.ht_used == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {रिक्त, रिक्त};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    प्रत्यागम;
	}

	di = dict_find(d, (char_u *)"regcontents", -1);
	यदि (di != रिक्त)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	यदि (stropt != रिक्त)
	{
	    पूर्णाङ्क ret = get_yank_type(&stropt, &yank_type, &block_len);

	    यदि (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_मूल्य_for_argument_str), "value");
		प्रत्यागम;
	    }
	}

	यदि (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    यदि (stropt != रिक्त)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	अन्यथा यदि (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    अन्यथा
	regcontents = &argvars[1];

    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	यदि (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    प्रत्यागम;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	यदि (stropt == रिक्त)
	    प्रत्यागम;		// type error
	प्रति (; *stropt != NUL; ++stropt)
	    विकल्प (*stropt)
	    {
		पक्ष 'a': पक्ष 'A':	// append
		    append = TRUE;
		    भङ्ग;
		स्वभाव:
		    get_yank_type(&stropt, &yank_type, &block_len);
	    }
    }

    यदि (regcontents && regcontents->v_type == VAR_LIST)
    {
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	list_T		*ll = regcontents->vval.v_list;
	listitem_T	*li;
	पूर्णाङ्क		len;

	// If the list is NULL handle like an empty list.
	len = ll == रिक्त ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	यदि (lstval == रिक्त)
	    प्रत्यागम;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	यदि (ll != रिक्त)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_वस्तुएँ(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		यदि (strval == रिक्त)
		    गच्छ free_lstval;
		यदि (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    यदि (strval == रिक्त)
			गच्छ free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = रिक्त;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, block_len);
free_lstval:
	यावत् (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    अन्यथा यदि (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	यदि (strval == रिक्त)
	    प्रत्यागम;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, block_len);
    }
    यदि (pointreg != 0)
	get_yank_register(pointreg, TRUE);

    rettv->vval.v_संख्या = 0;
}

/*
 * "settagstack()" function
 */
    स्थिर शून्य
f_settagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    dict_T	*d;
    पूर्णाङ्क		action = 'r';

    rettv->vval.v_संख्या = -1;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    // first argument: window number or id
    wp = find_win_by_nr_or_id(&argvars[0]);
    यदि (wp == रिक्त)
	प्रत्यागम;

    // second argument: dict with items to set in the tag stack
    यदि (check_for_dict_arg(argvars, 1) == FAIL)
	प्रत्यागम;
    d = argvars[1].vval.v_dict;
    यदि (d == रिक्त)
	प्रत्यागम;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    यदि (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    अन्यथा यदि (check_for_string_arg(argvars, 2) == FAIL)
	प्रत्यागम;
    अन्यथा
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	यदि (actstr == रिक्त)
	    प्रत्यागम;
	यदि ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	अन्यथा
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    प्रत्यागम;
	}
    }

    यदि (set_tagstack(wp, d, action) == OK)
	rettv->vval.v_संख्या = 0;
}

#ifdef FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    स्थिर शून्य
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    पूर्णाङ्क		len;

    यदि (in_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    यदि (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != रिक्त ? (char_u *)blob->bv_ga.ga_data : (char_u *)"";
	len = blob != रिक्त ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_bytes(p, len, रिक्त, 0));
    }
    अन्यथा
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_bytes(p, (पूर्णाङ्क)STRLEN(p), रिक्त, 0));
    }
}
#endif // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    स्थिर शून्य
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क do_special;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    स्थिर शून्य
f_shiftwidth(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_संख्या = 0;

    यदि (in_vim9script() && check_for_opt_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type != VAR_UNKNOWN)
    {
	दीर्घ	col;

	col = (दीर्घ)tv_get_संख्या_chk(argvars, रिक्त);
	यदि (col < 0)
	    प्रत्यागम;	// type error; errmsg already given
#ifdef FEAT_VARTABS
	rettv->vval.v_संख्या = get_sw_मूल्य_col(curbuf, col, FALSE);
	प्रत्यागम;
#endif
    }

    rettv->vval.v_संख्या = get_sw_मूल्य(curbuf);
}

/*
 * "soundfold({word})" function
 */
    स्थिर शून्य
f_soundfold(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#ifdef FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#अन्यथा
    rettv->vval.v_string = vim_strsave(s);
#endif
}

/*
 * "spellbadword()" function
 */
    स्थिर शून्य
f_spellbadword(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*word = (char_u *)"";
    hlf_T	attr = HLF_गिनती;
    पूर्णाङ्क		len = 0;
#ifdef FEAT_SPELL
    पूर्णाङ्क		wo_spell_save = curwin->w_p_spell;

    यदि (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    यदि (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	प्रत्यागम;
    }
#endif

    यदि (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	प्रत्यागम;
    }

#ifdef FEAT_SPELL
    यदि (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_to(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	यदि (len != 0)
	{
	    word = ml_get_cursor();
	    curwin->w_set_curswant = true;
	}
    }
    अन्यथा यदि (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	पूर्णाङ्क	capcol = -1;

	यदि (str != रिक्त)
	{
	    // Check the argument for spelling.
	    यावत् (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		यदि (attr != HLF_गिनती)
		{
		    word = str;
		    भङ्ग;
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
    विकल्प (attr)
    {
    पक्ष HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	भङ्ग;

    पक्ष HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	भङ्ग;

    पक्ष HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	भङ्ग;

    पक्ष HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	भङ्ग;

    स्वभाव:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	भङ्ग;
    }
}

/*
 * "spellsuggest()" function
 */
    स्थिर शून्य
f_spellsuggest(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SPELL
    char_u	*str;
    पूर्णाङ्क		typeerr = FALSE;
    पूर्णाङ्क		maxcount;
    garray_T	ga;
    पूर्णाङ्क		i;
    listitem_T	*li;
    पूर्णाङ्क		need_capital = FALSE;
    पूर्णाङ्क		wo_spell_save = curwin->w_p_spell;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	प्रत्यागम;

    यदि (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    यदि (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	प्रत्यागम;
    }
#endif

    यदि (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	प्रत्यागम;
    }

#ifdef FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (पूर्णाङ्क)tv_get_संख्या_chk(&argvars[1], &typeerr);
	यदि (maxcount <= 0)
	    प्रत्यागम;
	यदि (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (पूर्णाङ्क)tv_get_bool_chk(&argvars[2], &typeerr);
	    यदि (typeerr)
		प्रत्यागम;
	}
    }
    अन्यथा
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    प्रति (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_data)[i];

	li = listitem_alloc();
	यदि (li == रिक्त)
	    vim_free(str);
	अन्यथा
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

    स्थिर शून्य
f_split(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*end;
    char_u	*pat = रिक्त;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    पूर्णाङ्क		match;
    colnr_T	col = 0;
    पूर्णाङ्क		keepempty = FALSE;
    पूर्णाङ्क		typeerr = FALSE;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	प्रत्यागम;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    str = tv_get_string(&argvars[0]);
    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	यदि (pat == रिक्त)
	    typeerr = TRUE;
	यदि (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (पूर्णाङ्क)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    यदि (pat == रिक्त || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    यदि (rettv_list_alloc(rettv) == FAIL)
	गच्छ theend;
    यदि (typeerr)
	गच्छ theend;

    regmatch.regprog = eval_regcomp(pat);
    यदि (regmatch.regprog != रिक्त)
    {
	regmatch.rm_ic = FALSE;
	यावत् (*str != NUL || keepempty)
	{
	    यदि (*str == NUL)
		match = FALSE;	// empty item at the end
	    अन्यथा
		match = vim_regexec_nl(&regmatch, str, col);
	    यदि (match)
		end = regmatch.startp[0];
	    अन्यथा
		end = str + STRLEN(str);
	    यदि (keepempty || end > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && match && end < regmatch.endp[0]))
	    {
		यदि (list_append_string(rettv->vval.v_list, str,
						    (पूर्णाङ्क)(end - str)) == FAIL)
		    भङ्ग;
	    }
	    यदि (!match)
		भङ्ग;
	    // Advance to just after the match.
	    यदि (regmatch.endp[0] > str)
		col = 0;
	    अन्यथा
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
    स्थिर शून्य
f_submatch(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		error = FALSE;
    पूर्णाङ्क		no;
    पूर्णाङ्क		retList = 0;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    no = (पूर्णाङ्क)tv_get_संख्या_chk(&argvars[0], &error);
    यदि (error)
	प्रत्यागम;
    यदि (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_संख्या_nr), no);
	प्रत्यागम;
    }
    यदि (argvars[1].v_type != VAR_UNKNOWN)
	retList = (पूर्णाङ्क)tv_get_bool_chk(&argvars[1], &error);
    यदि (error)
	प्रत्यागम;

    यदि (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    अन्यथा
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = reg_submatch_list(no);
    }
}

/*
 * "substitute()" function
 */
    स्थिर शून्य
f_substitute(typval_T *argvars, typval_T *rettv)
{
    char_u	patbuf[NUMBUFLEN];
    char_u	subbuf[NUMBUFLEN];
    char_u	flagsbuf[NUMBUFLEN];
    char_u	*str;
    char_u	*pat;
    char_u	*sub = रिक्त;
    typval_T	*व्यंजक = रिक्त;
    char_u	*flg;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	प्रत्यागम;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    यदि (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	व्यंजक = &argvars[2];
    अन्यथा
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    यदि (str == रिक्त || pat == रिक्त || (sub == रिक्त && व्यंजक == रिक्त)
								|| flg == रिक्त)
	rettv->vval.v_string = रिक्त;
    अन्यथा
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, व्यंजक, flg, रिक्त);
}

/*
 * "swapfilelist()" function
 */
    स्थिर शून्य
f_swapfilelist(typval_T *argvars UNUSED, typval_T *rettv)
{
    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;
    recover_names(रिक्त, FALSE, rettv->vval.v_list, 0, रिक्त);
}

/*
 * "swapinfo(swap_filename)" function
 */
    स्थिर शून्य
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (rettv_dict_alloc(rettv) == OK)
	get_b0_dict(tv_get_string(argvars), rettv->vval.v_dict);
}

/*
 * "swapname(expr)" function
 */
    स्थिर शून्य
f_swapname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    यदि (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    buf = tv_get_buf(&argvars[0], FALSE);
    यदि (buf == रिक्त || buf->b_ml.ml_mfp == रिक्त
					|| buf->b_ml.ml_mfp->mf_fname == रिक्त)
	rettv->vval.v_string = रिक्त;
    अन्यथा
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    स्थिर शून्य
f_synID(typval_T *argvars UNUSED, typval_T *rettv)
{
    पूर्णाङ्क		id = 0;
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    पूर्णाङ्क		trans;
    पूर्णाङ्क		transerr = FALSE;

    यदि (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (linenr_T)tv_get_संख्या(&argvars[1]) - 1;	// -1 on type error
    trans = (पूर्णाङ्क)tv_get_bool_chk(&argvars[2], &transerr);

    यदि (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_पंक्ति_गिनती
	    && col >= 0 && col < (दीर्घ)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, col, trans, रिक्त, FALSE);
#endif

    rettv->vval.v_संख्या = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    स्थिर शून्य
f_synIDattr(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*p = रिक्त;
#ifdef FEAT_SYN_HL
    पूर्णाङ्क		id;
    char_u	*what;
    char_u	*mode;
    char_u	modebuf[NUMBUFLEN];
    पूर्णाङ्क		modec;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    id = (पूर्णाङ्क)tv_get_संख्या(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	mode = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(mode[0]);
	यदि (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    अन्यथा
    {
# यदि defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	यदि (USE_24BIT)
	    modec = 'g';
	अन्यथा
# endif
	    यदि (t_colors > 1)
		modec = 'c';
	    अन्यथा
		modec = 't';
    }

    विकल्प (TOLOWER_ASC(what[0]))
    {
	पक्ष 'b':
		यदि (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		अन्यथा					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		भङ्ग;

	पक्ष 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		भङ्ग;

	पक्ष 'i':
		यदि (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		अन्यथा					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		भङ्ग;

	पक्ष 'n':
		यदि (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		अन्यथा					// name
		    p = get_highlight_name_ext(रिक्त, id - 1, FALSE);
		भङ्ग;

	पक्ष 'r':					// reverse
		p = highlight_has_attr(id, HL_INVERSE, modec);
		भङ्ग;

	पक्ष 's':
		यदि (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		अन्यथा यदि (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		अन्यथा					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		भङ्ग;

	पक्ष 'u':
		यदि (STRLEN(what) >= 9)
		{
		    यदि (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    अन्यथा यदि (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    अन्यथा यदि (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    अन्यथा यदि (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    अन्यथा
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		अन्यथा
							// ul
		    p = highlight_color(id, what, modec);
		भङ्ग;
    }

    यदि (p != रिक्त)
	p = vim_strsave(p);
#endif
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    स्थिर शून्य
f_synIDtrans(typval_T *argvars UNUSED, typval_T *rettv)
{
    पूर्णाङ्क		id;

#ifdef FEAT_SYN_HL
    यदि (in_vim9script() && check_for_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    id = (पूर्णाङ्क)tv_get_संख्या(&argvars[0]);

    यदि (id > 0)
	id = syn_get_final_id(id);
    अन्यथा
#endif
	id = 0;

    rettv->vval.v_संख्या = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    स्थिर शून्य
f_synconcealed(typval_T *argvars UNUSED, typval_T *rettv)
{
#यदि defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	col;
    पूर्णाङ्क		syntax_flags = 0;
    पूर्णाङ्क		cchar;
    पूर्णाङ्क		matchid = 0;
    char_u	str[NUMBUFLEN];
#endif

    rettv_list_set(rettv, रिक्त);

    यदि (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

#यदि defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_संख्या(&argvars[1]) - 1;	// -1 on type error

    CLEAR_FIELD(str);

    यदि (rettv_list_alloc(rettv) == OK)
    {
	यदि (lnum >= 1 && lnum <= curbuf->b_ml.ml_पंक्ति_गिनती
	    && col >= 0 && col <= (दीर्घ)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (शून्य)syn_get_id(curwin, lnum, col, FALSE, रिक्त, FALSE);
	    syntax_flags = get_syntax_info(&matchid);

	    // get the conceal character
	    यदि ((syntax_flags & HL_CONCEAL) && curwin->w_p_cole < 3)
	    {
		cchar = syn_get_sub_char();
		यदि (cchar == NUL && curwin->w_p_cole == 1)
		    cchar = (curwin->w_lcs_chars.conceal == NUL) ? ' '
					: curwin->w_lcs_chars.conceal;
		यदि (cchar != NUL)
		{
		    यदि (has_mbyte)
			(*mb_char2bytes)(cchar, str);
		    अन्यथा
			str[0] = cchar;
		}
	    }
	}

	list_append_संख्या(rettv->vval.v_list,
					    (syntax_flags & HL_CONCEAL) != 0);
	// -1 to auto-determine strlen
	list_append_string(rettv->vval.v_list, str, -1);
	list_append_संख्या(rettv->vval.v_list, matchid);
    }
#endif
}

/*
 * "synstack(lnum, col)" function
 */
    स्थिर शून्य
f_synstack(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    पूर्णाङ्क		i;
    पूर्णाङ्क		id;
#endif

    rettv_list_set(rettv, रिक्त);

    यदि (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

#ifdef FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_संख्या(&argvars[1]) - 1;	// -1 on type error

    यदि (lnum >= 1 && lnum <= curbuf->b_ml.ml_पंक्ति_गिनती
	    && col >= 0 && col <= (दीर्घ)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(शून्य)syn_get_id(curwin, lnum, col, FALSE, रिक्त, TRUE);
	प्रति (i = 0; ; ++i)
	{
	    id = syn_get_ढेर_item(i);
	    यदि (id < 0)
		भङ्ग;
	    यदि (list_append_संख्या(rettv->vval.v_list, id) == FAIL)
		भङ्ग;
	}
    }
#endif
}

/*
 * "tabpagebuflist()" function
 */
    स्थिर शून्य
f_tabpagebuflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp = रिक्त;

    यदि (in_vim9script() && check_for_opt_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    अन्यथा
    {
	tp = find_tabpage((पूर्णाङ्क)tv_get_संख्या(&argvars[0]));
	यदि (tp != रिक्त)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    यदि (wp != रिक्त && rettv_list_alloc(rettv) == OK)
    {
	प्रति (; wp != रिक्त; wp = wp->w_next)
	    यदि (list_append_संख्या(rettv->vval.v_list,
						wp->w_buffer->b_fnum) == FAIL)
		भङ्ग;
    }
}

/*
 * "tagfiles()" function
 */
    स्थिर शून्य
f_tagfiles(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    पूर्णाङ्क		first;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;
    fname = alloc(MAXPATHL);
    यदि (fname == रिक्त)
	प्रत्यागम;

    प्रति (first = TRUE; ; first = FALSE)
	यदि (get_tagfname(&tn, first, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    भङ्ग;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    स्थिर शून्य
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = रिक्त;
    char_u  *tag_pattern;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    tag_pattern = tv_get_string(&argvars[0]);

    rettv->vval.v_संख्या = FALSE;
    यदि (*tag_pattern == NUL)
	प्रत्यागम;

    यदि (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    यदि (rettv_list_alloc(rettv) == OK)
	(शून्य)get_tags(rettv->vval.v_list, tag_pattern, fname);
}

/*
 * "type(expr)" function
 */
    स्थिर शून्य
f_type(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क n = -1;

    विकल्प (argvars[0].v_type)
    {
	पक्ष VAR_संख्या:  n = VAR_TYPE_संख्या; भङ्ग;
	पक्ष VAR_STRING:  n = VAR_TYPE_STRING; भङ्ग;
	पक्ष VAR_PARTIAL:
	पक्ष VAR_FUNC:    n = VAR_TYPE_FUNC; भङ्ग;
	पक्ष VAR_LIST:    n = VAR_TYPE_LIST; भङ्ग;
	पक्ष VAR_TUPLE:   n = VAR_TYPE_TUPLE; भङ्ग;
	पक्ष VAR_DICT:    n = VAR_TYPE_DICT; भङ्ग;
	पक्ष VAR_FLOAT:   n = VAR_TYPE_FLOAT; भङ्ग;
	पक्ष VAR_BOOL:	  n = VAR_TYPE_BOOL; भङ्ग;
	पक्ष VAR_SPECIAL: n = VAR_TYPE_NONE; भङ्ग;
	पक्ष VAR_JOB:     n = VAR_TYPE_JOB; भङ्ग;
	पक्ष VAR_CHANNEL: n = VAR_TYPE_CHANNEL; भङ्ग;
	पक्ष VAR_BLOB:    n = VAR_TYPE_BLOB; भङ्ग;
	पक्ष VAR_INSTR:   n = VAR_TYPE_INSTR; भङ्ग;
	पक्ष VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; भङ्ग;
	पक्ष VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		यदि (cl != रिक्त && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		अन्यथा
		    n = VAR_TYPE_CLASS;
		भङ्ग;
	    }
	पक्ष VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		यदि (obj == रिक्त)
		    n = VAR_TYPE_OBJECT;
		अन्यथा
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    यदि (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    अन्यथा
			n = VAR_TYPE_OBJECT;
		}
		भङ्ग;
	    }
	पक्ष VAR_VOID:
	    emsg(_(e_cannot_use_void_मूल्य));
	    भङ्ग;
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	    internal_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    भङ्ग;
    }
    rettv->vval.v_संख्या = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    स्थिर शून्य
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_start = 0;
    colnr_T	vcol_end = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    पूर्णाङ्क		winchanged = FALSE;
    पूर्णाङ्क		len;

    यदि (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_संख्या_arg(argvars, 2) == FAIL))))
	प्रत्यागम;

    यदि (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((पूर्णाङ्क)tv_get_संख्या(&argvars[2]), &tp);
	यदि (wp == रिक्त || tp == रिक्त)
	    गच्छ theend;

	यदि (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    गच्छ theend;

	check_cursor();
	winchanged = TRUE;
    }

    पूर्णाङ्क fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    यदि (fp != रिक्त && fp->lnum <= curbuf->b_ml.ml_पंक्ति_गिनती
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	यदि (fp->col < 0)
	    fp->col = 0;
	अन्यथा
	{
	    len = (पूर्णाङ्क)ml_get_len(fp->lnum);
	    यदि (fp->col > len)
		fp->col = len;
	}
	getvvcol(curwin, fp, &vcol_start, रिक्त, &vcol_end, 0);
	++vcol_start;
	++vcol_end;
    }

theend:
    यदि (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	यदि (rettv_list_alloc(rettv) == OK)
	{
	    list_append_संख्या(rettv->vval.v_list, vcol_start);
	    list_append_संख्या(rettv->vval.v_list, vcol_end);
	}
	अन्यथा
	    rettv->vval.v_संख्या = 0;
    }
    अन्यथा
	rettv->vval.v_संख्या = vcol_end;

    यदि (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    स्थिर शून्य
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    यदि (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_mode_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    यदि (non_zero_arg(&argvars[0]))
	curbuf->b_visual_mode_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    स्थिर शून्य
f_wildmenumode(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    यदि (wild_menu_showing || ((State & MODE_CMDLINE) && cmdline_pum_active()))
	rettv->vval.v_संख्या = 1;
}

/*
 * "windowsversion()" function
 */
    स्थिर शून्य
f_windowsversion(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windowsVersion);
}

/*
 * "wordcount()" function
 */
    स्थिर शून्य
f_wordcount(typval_T *argvars UNUSED, typval_T *rettv)
{
    यदि (rettv_dict_alloc(rettv) == FAIL)
	प्रत्यागम;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    स्थिर शून्य
f_xor(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    rettv->vval.v_संख्या = tv_get_संख्या_chk(&argvars[0], रिक्त)
					^ tv_get_संख्या_chk(&argvars[1], रिक्त);
}

#endif // FEAT_EVAL
