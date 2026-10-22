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
#تعریف_کرو USING_FLOAT_STUFF

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

#اگر_تعریف_شدہ VMS
# include <float.h>
#اگر_ختم

ساکن خالی f_and(typval_T *argvars, typval_T *rettv);
#اگر_تعریف_شدہ FEAT_BEVAL
ساکن خالی f_balloon_gettext(typval_T *argvars, typval_T *rettv);
ساکن خالی f_balloon_show(typval_T *argvars, typval_T *rettv);
# اگر defined(FEAT_BEVAL_رکن)
ساکن خالی f_balloon_بانٹو(typval_T *argvars, typval_T *rettv);
# endif
#اگر_ختم
ساکن خالی f_base64_encode(typval_T *argvars, typval_T *rettv);
ساکن خالی f_base64_decode(typval_T *argvars, typval_T *rettv);
ساکن خالی f_bindtextdomain(typval_T *argvars, typval_T *rettv);
ساکن خالی f_byte2line(typval_T *argvars, typval_T *rettv);
ساکن خالی f_call(typval_T *argvars, typval_T *rettv);
ساکن خالی f_changenr(typval_T *argvars, typval_T *rettv);
ساکن خالی f_char2nr(typval_T *argvars, typval_T *rettv);
ساکن خالی f_charcol(typval_T *argvars, typval_T *rettv);
ساکن خالی f_ستون(typval_T *argvars, typval_T *rettv);
ساکن خالی f_confirm(typval_T *argvars, typval_T *rettv);
ساکن خالی f_copy(typval_T *argvars, typval_T *rettv);
ساکن خالی f_cursor(typval_T *argsvars, typval_T *rettv);
#اگر_تعریف_شدہ MSWIN
ساکن خالی f_debugbreak(typval_T *argvars, typval_T *rettv);
#اگر_ختم
ساکن خالی f_deepcopy(typval_T *argvars, typval_T *rettv);
ساکن خالی f_did_filetype(typval_T *argvars, typval_T *rettv);
ساکن خالی f_echoraw(typval_T *argvars, typval_T *rettv);
ساکن خالی f_خالی_خانے(typval_T *argvars, typval_T *rettv);
ساکن خالی f_environ(typval_T *argvars, typval_T *rettv);
ساکن خالی f_err_teapot(typval_T *argvars, typval_T *rettv);
ساکن خالی f_escape(typval_T *argvars, typval_T *rettv);
ساکن خالی f_eval(typval_T *argvars, typval_T *rettv);
ساکن خالی f_eventhandler(typval_T *argvars, typval_T *rettv);
ساکن خالی f_execute(typval_T *argvars, typval_T *rettv);
ساکن خالی f_exists_compiled(typval_T *argvars, typval_T *rettv);
ساکن خالی f_expand(typval_T *argvars, typval_T *rettv);
ساکن خالی f_expandcmd(typval_T *argvars, typval_T *rettv);
ساکن خالی f_feedkeys(typval_T *argvars, typval_T *rettv);
ساکن خالی f_fnameescape(typval_T *argvars, typval_T *rettv);
ساکن خالی f_foreground(typval_T *argvars, typval_T *rettv);
ساکن خالی f_funcref(typval_T *argvars, typval_T *rettv);
ساکن خالی f_function(typval_T *argvars, typval_T *rettv);
ساکن خالی f_garbagecollect(typval_T *argvars, typval_T *rettv);
ساکن خالی f_get(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getbgcolor(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getcellpixels(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getchangelist(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getcharpos(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getcharsearch(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getcurpos(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getcursorcharpos(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getenv(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getfontname(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getinfo(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getjumplist(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getpid(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getpos(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getreg(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getreginfo(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getregion(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getregionpos(typval_T *argvars, typval_T *rettv);
ساکن خالی f_getregtype(typval_T *argvars, typval_T *rettv);
ساکن خالی f_gettagstack(typval_T *argvars, typval_T *rettv);
ساکن خالی f_gettext(typval_T *argvars, typval_T *rettv);
ساکن خالی f_haslocaldir(typval_T *argvars, typval_T *rettv);
ساکن خالی f_hlID(typval_T *argvars, typval_T *rettv);
ساکن خالی f_hlexists(typval_T *argvars, typval_T *rettv);
ساکن خالی f_hostname(typval_T *argvars, typval_T *rettv);
ساکن خالی f_id(typval_T *argvars, typval_T *rettv);
ساکن خالی f_اشاریہ(typval_T *argvars, typval_T *rettv);
ساکن خالی f_indexof(typval_T *argvars, typval_T *rettv);
ساکن خالی f_داخلہ(typval_T *argvars, typval_T *rettv);
ساکن خالی f_inputdialog(typval_T *argvars, typval_T *rettv);
ساکن خالی f_inputlist(typval_T *argvars, typval_T *rettv);
ساکن خالی f_inputrestore(typval_T *argvars, typval_T *rettv);
ساکن خالی f_inputsave(typval_T *argvars, typval_T *rettv);
ساکن خالی f_inputsecret(typval_T *argvars, typval_T *rettv);
ساکن خالی f_مداخلت(typval_T *argvars, typval_T *rettv);
ساکن خالی f_invert(typval_T *argvars, typval_T *rettv);
ساکن خالی f_islocked(typval_T *argvars, typval_T *rettv);
ساکن خالی f_اشیاء(typval_T *argvars, typval_T *rettv);
ساکن خالی f_keytrans(typval_T *argvars, typval_T *rettv);
ساکن خالی f_آخری_بفر_nr(typval_T *argvars, typval_T *rettv);
ساکن خالی f_libcall(typval_T *argvars, typval_T *rettv);
ساکن خالی f_libcallnr(typval_T *argvars, typval_T *rettv);
ساکن خالی f_سطر(typval_T *argvars, typval_T *rettv);
ساکن خالی f_line2byte(typval_T *argvars, typval_T *rettv);
#اگر_تعریف_شدہ FEAT_LUA
ساکن خالی f_luaeval(typval_T *argvars, typval_T *rettv);
#اگر_ختم
ساکن خالی f_مطابقت(typval_T *argvars, typval_T *rettv);
ساکن خالی f_matchbufline(typval_T *argvars, typval_T *rettv);
ساکن خالی f_matchend(typval_T *argvars, typval_T *rettv);
ساکن خالی f_matchlist(typval_T *argvars, typval_T *rettv);
ساکن خالی f_matchstr(typval_T *argvars, typval_T *rettv);
ساکن خالی f_matchstrlist(typval_T *argvars, typval_T *rettv);
ساکن خالی f_matchstrpos(typval_T *argvars, typval_T *rettv);
ساکن خالی f_max(typval_T *argvars, typval_T *rettv);
ساکن خالی f_min(typval_T *argvars, typval_T *rettv);
#اگر_تعریف_شدہ FEAT_MZSCHEME
ساکن خالی f_mzeval(typval_T *argvars, typval_T *rettv);
#اگر_ختم
ساکن خالی f_nextnonblank(typval_T *argvars, typval_T *rettv);
ساکن خالی f_ngettext(typval_T *argvars, typval_T *rettv);
ساکن خالی f_nr2char(typval_T *argvars, typval_T *rettv);
ساکن خالی f_or(typval_T *argvars, typval_T *rettv);
#اگر_تعریف_شدہ FEAT_PERL
ساکن خالی f_perleval(typval_T *argvars, typval_T *rettv);
#اگر_ختم
ساکن خالی f_prevnonblank(typval_T *argvars, typval_T *rettv);
ساکن خالی f_printf(typval_T *argvars, typval_T *rettv);
ساکن خالی f_pum_getpos(typval_T *argvars, typval_T *rettv);
ساکن خالی f_pumvisible(typval_T *argvars, typval_T *rettv);
#اگر_تعریف_شدہ FEAT_PYTHON3
ساکن خالی f_py3eval(typval_T *argvars, typval_T *rettv);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PYTHON
ساکن خالی f_pyeval(typval_T *argvars, typval_T *rettv);
#اگر_ختم
#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
ساکن خالی f_pyxeval(typval_T *argvars, typval_T *rettv);
#اگر_ختم
ساکن خالی f_test_srand_بیج(typval_T *argvars, typval_T *rettv);
ساکن خالی f_rand(typval_T *argvars, typval_T *rettv);
ساکن خالی f_range(typval_T *argvars, typval_T *rettv);
ساکن خالی f_reg_executing(typval_T *argvars, typval_T *rettv);
ساکن خالی f_reg_recording(typval_T *argvars, typval_T *rettv);
ساکن خالی f_rename(typval_T *argvars, typval_T *rettv);
ساکن خالی f_repeat(typval_T *argvars, typval_T *rettv);
#اگر_تعریف_شدہ FEAT_RUBY
ساکن خالی f_rubyeval(typval_T *argvars, typval_T *rettv);
#اگر_ختم
ساکن خالی f_screenattr(typval_T *argvars, typval_T *rettv);
ساکن خالی f_screenchar(typval_T *argvars, typval_T *rettv);
ساکن خالی f_screenchars(typval_T *argvars, typval_T *rettv);
ساکن خالی f_screencol(typval_T *argvars, typval_T *rettv);
ساکن خالی f_screenrow(typval_T *argvars, typval_T *rettv);
ساکن خالی f_screenstring(typval_T *argvars, typval_T *rettv);
ساکن خالی f_تلاش(typval_T *argvars, typval_T *rettv);
ساکن خالی f_searchdecl(typval_T *argvars, typval_T *rettv);
ساکن خالی f_searchpair(typval_T *argvars, typval_T *rettv);
ساکن خالی f_searchpairpos(typval_T *argvars, typval_T *rettv);
ساکن خالی f_searchpos(typval_T *argvars, typval_T *rettv);
ساکن خالی f_setcharpos(typval_T *argvars, typval_T *rettv);
ساکن خالی f_setcharsearch(typval_T *argvars, typval_T *rettv);
ساکن خالی f_setcursorcharpos(typval_T *argvars, typval_T *rettv);
ساکن خالی f_setenv(typval_T *argvars, typval_T *rettv);
ساکن خالی f_setfperm(typval_T *argvars, typval_T *rettv);
ساکن خالی f_setpos(typval_T *argvars, typval_T *rettv);
ساکن خالی f_setreg(typval_T *argvars, typval_T *rettv);
ساکن خالی f_settagstack(typval_T *argvars, typval_T *rettv);
#اگر_تعریف_شدہ FEAT_CRYPT
ساکن خالی f_sha256(typval_T *argvars, typval_T *rettv);
#اگر_ختم
ساکن خالی f_shellescape(typval_T *argvars, typval_T *rettv);
ساکن خالی f_shiftwidth(typval_T *argvars, typval_T *rettv);
ساکن خالی f_soundfold(typval_T *argvars, typval_T *rettv);
ساکن خالی f_spellbadword(typval_T *argvars, typval_T *rettv);
ساکن خالی f_spellsuggest(typval_T *argvars, typval_T *rettv);
ساکن خالی f_بانٹو(typval_T *argvars, typval_T *rettv);
ساکن خالی f_srand(typval_T *argvars, typval_T *rettv);
ساکن خالی f_submatch(typval_T *argvars, typval_T *rettv);
ساکن خالی f_substitute(typval_T *argvars, typval_T *rettv);
ساکن خالی f_swapfilelist(typval_T *argvars, typval_T *rettv);
ساکن خالی f_swapinfo(typval_T *argvars, typval_T *rettv);
ساکن خالی f_swapname(typval_T *argvars, typval_T *rettv);
ساکن خالی f_synID(typval_T *argvars, typval_T *rettv);
ساکن خالی f_synIDattr(typval_T *argvars, typval_T *rettv);
ساکن خالی f_synIDtrans(typval_T *argvars, typval_T *rettv);
ساکن خالی f_synstack(typval_T *argvars, typval_T *rettv);
ساکن خالی f_synconcealed(typval_T *argvars, typval_T *rettv);
ساکن خالی f_tabpagebuflist(typval_T *argvars, typval_T *rettv);
ساکن خالی f_taglist(typval_T *argvars, typval_T *rettv);
ساکن خالی f_tagfiles(typval_T *argvars, typval_T *rettv);
ساکن خالی f_type(typval_T *argvars, typval_T *rettv);
ساکن خالی f_virtcol(typval_T *argvars, typval_T *rettv);
ساکن خالی f_visualmode(typval_T *argvars, typval_T *rettv);
ساکن خالی f_wildmenumode(typval_T *argvars, typval_T *rettv);
ساکن خالی f_windowsversion(typval_T *argvars, typval_T *rettv);
ساکن خالی f_wordcount(typval_T *argvars, typval_T *rettv);
ساکن خالی f_xor(typval_T *argvars, typval_T *rettv);


/*
 * Functions that check the argument type of a builtin function.
 * Each function returns FAIL and gives an error message if the type is wrong.
 */

// Context passed to an arg_ function.
قسمتعریف ساخت {
    صحیح		arg_گنتی;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    صحیح		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
قسمتعریف صحیح (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    ساکن صحیح
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    واپس need_type(actual, expected, 0,
	    context->arg_idx - context->arg_گنتی, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    ساکن صحیح
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    اگر (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_گنتی, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	واپس FAIL;
    واپس arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    صحیح
arg_type_modifiable(type_T *type, صحیح arg_idx)
{
    حرف *tofree;

    اگر ((type->tt_flags & TTFLAG_CONST) == 0)
	واپس OK;
    semsg(_(e_دلیل_nr_trying_مقصد_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    واپس FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    ساکن صحیح
arg_any(type_T *type غیر_مستعمل,
	type_T *decl_type غیر_مستعمل,
	argcontext_T *context غیر_مستعمل)
{
    واپس OK;
}

/*
 * Check "type" is a float or a number.
 */
    ساکن صحیح
arg_float_or_nr(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_نمبر
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_نمبر, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a number.
 */
    ساکن صحیح
arg_نمبر(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    واپس check_arg_type(&t_نمبر, type, context);
}

/*
 * Check "type" is an object.
 */
    ساکن صحیح
arg_object(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    ساکن صحیح
arg_dict_any(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    واپس check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    ساکن صحیح
arg_list_any(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    واپس check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    ساکن صحیح
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type غیر_مستعمل,
	argcontext_T *context)
{
    واپس check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    ساکن صحیح
arg_list_نمبر(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    واپس check_arg_type(&t_list_نمبر, type, context);
}

/*
 * Check "type" is a list of strings.
 */
    ساکن صحیح
arg_list_string(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    واپس check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    ساکن صحیح
arg_tuple_any(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    واپس check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    ساکن صحیح
arg_string(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    واپس check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    ساکن صحیح
arg_blob(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    واپس check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    ساکن صحیح
arg_bool(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    واپس check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    ساکن صحیح
arg_bool_or_nr(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_نمبر
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_نمبر, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    ساکن صحیح
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	واپس OK;
    واپس check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    ساکن صحیح
arg_list_or_blob(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    ساکن صحیح
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    اگر (arg_list_or_blob(type, decl_type, context) == FAIL)
	واپس FAIL;
    واپس arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    ساکن صحیح
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    واپس FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    ساکن صحیح
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a string or a number
 */
    ساکن صحیح
arg_string_or_nr(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_نمبر
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    ساکن صحیح
arg_بفر(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_نمبر
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    ساکن صحیح
arg_بفر_or_dict_any(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_نمبر
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    ساکن صحیح
arg_lnum(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_نمبر
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    ساکن صحیح
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	واپس OK;
    اگر (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	واپس FAIL;
    }
    اگر (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	واپس OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    ساکن صحیح
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    ساکن صحیح
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    ساکن صحیح
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    ساکن صحیح
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    ساکن صحیح
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type غیر_مستعمل,
	argcontext_T *context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    ساکن صحیح
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    اگر (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	واپس FAIL;
    واپس arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    ساکن صحیح
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    ساکن صحیح
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    اگر (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	واپس FAIL;
    واپس arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    ساکن صحیح
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	واپس OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_دلیل_nr),
						 context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    ساکن صحیح
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = تہی;
    type_T *(دلائل[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, تہی, تہی, دلائل};

    اگر (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	اگر (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	ورنہ
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    ورنہ اگر (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    ورنہ اگر (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_نمبر;

    دلائل[0] = تہی;
    دلائل[1] = &t_unknown;
    اگر (type->tt_argcount != -1)
    {
	اگر (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_نمبر_کا_arguments));
	    واپس FAIL;
	}
	اگر (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	ورنہ
	{
	    اگر (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		دلائل[0] = &t_نمبر;
	    ورنہ اگر (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		دلائل[0] = &t_string;
	    اگر (دلائل[0] != تہی)
		دلائل[1] = expected_member;
	}
    }

    اگر (!type_any_or_unknown(type->tt_member) || دلائل[0] != تہی)
    {
	where_T where = WHERE_INIT;

	اگر (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == تہی
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	ورنہ اگر (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	ورنہ // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	اگر (دلائل[0] == تہی)
	    دلائل[0] = &t_unknown;
	اگر (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_اشاریہ = 2;
	where.wt_قسم = WT_دلیل;
	واپس check_type(&t_func_exp, type, TRUE, where);
    }
    واپس OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    ساکن صحیح
arg_filter_func(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	واپس OK;

    اگر (type->tt_type == VAR_FUNC)
	واپس check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_دلیل_nr), 2);
    واپس FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    ساکن صحیح
arg_map_func(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	واپس OK;

    اگر (type->tt_type == VAR_FUNC)
	واپس check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_دلیل_nr), 2);
    واپس FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    ساکن صحیح
arg_foreach_func(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	واپس OK;

    اگر (type->tt_type == VAR_FUNC)
	واپس check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_دلیل_nr), 2);
    واپس FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    ساکن صحیح
arg_ترتیب_دو_how(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	واپس OK;

    اگر (type->tt_type == VAR_FUNC)
    {
	type_T *(دلائل[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_نمبر, تہی, دلائل};

	اگر (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    دلائل[0] = context->arg_types[0].type_curr->tt_member;
	ورنہ
	    دلائل[0] = &t_unknown;
	اگر (!type_any_or_unknown(type->tt_member) || دلائل[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    دلائل[1] = دلائل[0];
	    اگر (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_اشاریہ = 2;
	    where.wt_قسم = WT_دلیل;
	    واپس check_type(&t_func_exp, type, TRUE, where);
	}

	واپس OK;
    }
    semsg(_(e_string_or_function_required_for_دلیل_nr), 2);
    واپس FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    ساکن صحیح
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_نمبر
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    ساکن صحیح
varargs_class(type_T *type غیر_مستعمل,
	      type_T *decl_type غیر_مستعمل,
	      argcontext_T *context)
{
    برائے (صحیح i = context->arg_idx; i < context->arg_گنتی; ++i)
    {
	type2_T *types = &context->arg_types[i];
	اگر (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_دلیل_nr), i + 1);
	    واپس FAIL;
	}
    }
    واپس OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    ساکن صحیح
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    ساکن صحیح
arg_الٹ(type_T *type, type_T *decl_type, argcontext_T *context)
{
    اگر (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	واپس FAIL;
    واپس arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    ساکن صحیح
arg_کام(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    واپس check_arg_type(&t_کام, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    ساکن صحیح
arg_chan_or_کام(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_کام
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    ساکن صحیح
arg_same_as_پچھلا(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    type_T *پچھلا_type = context->arg_types[context->arg_idx - 1].type_decl;

    واپس check_arg_type(پچھلا_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    ساکن صحیح
arg_same_struct_as_پچھلا(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    type_T *پچھلا_type = context->arg_types[context->arg_idx - 1].type_curr;

    اگر (پچھلا_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	واپس check_arg_type(پچھلا_type, type, context);
    واپس OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    ساکن صحیح
arg_شے_کا_پچھلا(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    type_T *پچھلا_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    اگر (پچھلا_type->tt_type == VAR_LIST)
	expected = پچھلا_type->tt_member;
    ورنہ اگر (پچھلا_type->tt_type == VAR_BLOB)
	expected = &t_نمبر;
    ورنہ
	// probably VAR_ANY, can't check
	واپس OK;

    واپس check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    ساکن صحیح
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_نمبر
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    ساکن صحیح
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	واپس OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    ساکن صحیح
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *پہلا_type = context->arg_types[context->arg_idx - 2].type_curr;

    اگر (پہلا_type->tt_type == VAR_LIST || پہلا_type->tt_type == VAR_BLOB)
	واپس arg_نمبر(type, decl_type, context);
    اگر (پہلا_type->tt_type == VAR_DICT)
	واپس arg_string(type, decl_type, context);
    واپس OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    ساکن صحیح
arg_get1(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	واپس OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    ساکن صحیح
arg_len1(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_نمبر
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	واپس OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    ساکن صحیح
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *پہلا_type = context->arg_types[context->arg_idx - 1].type_curr;

    اگر (پہلا_type->tt_type == VAR_LIST || پہلا_type->tt_type == VAR_BLOB)
	واپس arg_نمبر(type, decl_type, context);
    اگر (پہلا_type->tt_type == VAR_DICT)
	واپس arg_string_or_nr(type, decl_type, context);
    واپس OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    ساکن صحیح
arg_repeat1(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_نمبر
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	واپس OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    ساکن صحیح
arg_slice1(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	واپس OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    ساکن صحیح
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type غیر_مستعمل,
    argcontext_T	*context)
{
    اگر (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	واپس OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_دلیل_nr),
							 context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    ساکن صحیح
arg_cursor1(type_T *type, type_T *decl_type غیر_مستعمل, argcontext_T *context)
{
    اگر (type->tt_type == VAR_نمبر
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	واپس OK;

    arg_type_mismatch(&t_نمبر, type, context->arg_idx + 1);
    واپس FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
ساکن argcheck_T arg1_blob[] = {arg_blob};
ساکن argcheck_T arg1_bool[] = {arg_bool};
ساکن argcheck_T arg1_بفر[] = {arg_بفر};
ساکن argcheck_T arg1_بفر_or_dict_any[] = {arg_بفر_or_dict_any};
ساکن argcheck_T arg1_chan_or_کام[] = {arg_chan_or_کام};
ساکن argcheck_T arg1_dict_any[] = {arg_dict_any};
ساکن argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
ساکن argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
ساکن argcheck_T arg1_کام[] = {arg_کام};
ساکن argcheck_T arg1_list_any[] = {arg_list_any};
ساکن argcheck_T arg1_tuple_any[] = {arg_tuple_any};
ساکن argcheck_T arg1_list_نمبر[] = {arg_list_نمبر};
ساکن argcheck_T arg1_الٹ[] = {arg_الٹ};
ساکن argcheck_T arg1_list_or_tuple_or_dict[] = {arg_list_or_tuple_or_dict};
ساکن argcheck_T arg1_list_string[] = {arg_list_string};
ساکن argcheck_T arg1_list_tuple_dict_blob_or_string[] = {arg_list_tuple_dict_blob_or_string};
ساکن argcheck_T arg1_lnum[] = {arg_lnum};
ساکن argcheck_T arg1_نمبر[] = {arg_نمبر};
ساکن argcheck_T arg1_string[] = {arg_string};
ساکن argcheck_T arg1_string_or_list_any[] = {arg_string_or_list_any};
ساکن argcheck_T arg1_string_or_list_string[] = {arg_string_or_list_string};
ساکن argcheck_T arg1_string_or_nr[] = {arg_string_or_nr};
ساکن argcheck_T arg1_string_or_blob[] = {arg_string_or_blob};
ساکن argcheck_T arg2_بفر_any[] = {arg_بفر, arg_any};
ساکن argcheck_T arg2_بفر_bool[] = {arg_بفر, arg_bool};
ساکن argcheck_T arg2_بفر_list_any[] = {arg_بفر, arg_list_any};
ساکن argcheck_T arg2_بفر_lnum[] = {arg_بفر, arg_lnum};
ساکن argcheck_T arg2_بفر_نمبر[] = {arg_بفر, arg_نمبر};
ساکن argcheck_T arg2_بفر_string[] = {arg_بفر, arg_string};
ساکن argcheck_T arg2_chan_or_کام_dict[] = {arg_chan_or_کام, arg_dict_any};
ساکن argcheck_T arg2_chan_or_کام_string[] = {arg_chan_or_کام, arg_string};
ساکن argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
ساکن argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
ساکن argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
ساکن argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
ساکن argcheck_T arg2_کام_dict[] = {arg_کام, arg_dict_any};
ساکن argcheck_T arg2_کام_string_or_نمبر[] = {arg_کام, arg_string_or_nr};
ساکن argcheck_T arg2_list_any_نمبر[] = {arg_list_any, arg_نمبر};
ساکن argcheck_T arg2_list_نمبر[] = {arg_list_نمبر, arg_list_نمبر};
ساکن argcheck_T arg2_list_نمبر_bool[] = {arg_list_نمبر, arg_bool};
ساکن argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
ساکن argcheck_T arg2_listblobmod_شے[] = {arg_list_or_blob_mod, arg_شے_کا_پچھلا};
ساکن argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
ساکن argcheck_T arg2_lnum_نمبر[] = {arg_lnum, arg_نمبر};
ساکن argcheck_T arg2_نمبر[] = {arg_نمبر, arg_نمبر};
ساکن argcheck_T arg2_نمبر_any[] = {arg_نمبر, arg_any};
ساکن argcheck_T arg2_نمبر_bool[] = {arg_نمبر, arg_bool};
ساکن argcheck_T arg2_نمبر_dict_any[] = {arg_نمبر, arg_dict_any};
ساکن argcheck_T arg2_نمبر_list[] = {arg_نمبر, arg_list_any};
ساکن argcheck_T arg2_نمبر_string[] = {arg_نمبر, arg_string};
ساکن argcheck_T arg2_نمبر_بفر[] = {arg_نمبر, arg_بفر};
ساکن argcheck_T arg2_نمبر_string_or_list[] = {arg_نمبر, arg_string_or_list_any};
ساکن argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
ساکن argcheck_T arg2_string[] = {arg_string, arg_string};
ساکن argcheck_T arg2_string_any[] = {arg_string, arg_any};
ساکن argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
ساکن argcheck_T arg2_string_chan_or_کام[] = {arg_string, arg_chan_or_کام};
ساکن argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
ساکن argcheck_T arg2_string_list_نمبر[] = {arg_string, arg_list_نمبر};
ساکن argcheck_T arg2_string_نمبر[] = {arg_string, arg_نمبر};
ساکن argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
ساکن argcheck_T arg2_string_or_list_نمبر[] = {arg_string_or_list_any, arg_نمبر};
ساکن argcheck_T arg2_string_string_or_نمبر[] = {arg_string, arg_string_or_nr};
ساکن argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
ساکن argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
ساکن argcheck_T arg3_any_بفر_bool_or_dict[] = {
			      arg_any, arg_بفر, arg_bool_or_dict_any};
ساکن argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
ساکن argcheck_T arg3_بفر_lnum_lnum[] = {arg_بفر, arg_lnum, arg_lnum};
ساکن argcheck_T arg3_بفر_نمبر_نمبر[] = {arg_بفر, arg_نمبر, arg_نمبر};
ساکن argcheck_T arg3_بفر_string_any[] = {arg_بفر, arg_string, arg_any};
ساکن argcheck_T arg3_بفر_string_dict[] = {arg_بفر, arg_string, arg_dict_any};
ساکن argcheck_T arg3_dict_نمبر_نمبر[] = {arg_dict_any, arg_نمبر, arg_نمبر};
ساکن argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
ساکن argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
ساکن argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
ساکن argcheck_T arg3_lnum_نمبر_bool[] = {arg_lnum, arg_نمبر, arg_bool};
ساکن argcheck_T arg3_نمبر[] = {arg_نمبر, arg_نمبر, arg_نمبر};
ساکن argcheck_T arg3_نمبر_any_dict[] = {arg_نمبر, arg_any, arg_dict_any};
ساکن argcheck_T arg3_نمبر_نمبر_dict[] = {arg_نمبر, arg_نمبر, arg_dict_any};
ساکن argcheck_T arg3_نمبر_string_any[] = {arg_نمبر, arg_string, arg_any};
ساکن argcheck_T arg3_نمبر_string_بفر[] = {arg_نمبر, arg_string, arg_بفر};
ساکن argcheck_T arg3_نمبر_string_string[] = {arg_نمبر, arg_string, arg_string};
ساکن argcheck_T arg3_string[] = {arg_string, arg_string, arg_string};
ساکن argcheck_T arg3_string_any_dict[] = {arg_string, arg_any, arg_dict_any};
ساکن argcheck_T arg3_string_any_string[] = {arg_string, arg_any, arg_string};
ساکن argcheck_T arg3_string_bool_bool[] = {arg_string, arg_bool, arg_bool};
ساکن argcheck_T arg3_string_نمبر_bool[] = {arg_string, arg_نمبر, arg_bool};
ساکن argcheck_T arg3_string_نمبر_نمبر[] = {arg_string, arg_نمبر, arg_نمبر};
ساکن argcheck_T arg3_string_or_dict_bool_dict[] = {arg_string_or_dict_any, arg_bool, arg_dict_any};
ساکن argcheck_T arg3_string_or_list_bool_نمبر[] = {arg_string_or_list_any, arg_bool, arg_نمبر};
ساکن argcheck_T arg3_string_string_bool[] = {arg_string, arg_string, arg_bool};
ساکن argcheck_T arg3_string_string_dict[] = {arg_string, arg_string, arg_dict_any};
ساکن argcheck_T arg3_string_string_نمبر[] = {arg_string, arg_string, arg_نمبر};
ساکن argcheck_T arg4_نمبر_نمبر_string_any[] = {arg_نمبر, arg_نمبر, arg_string, arg_any};
ساکن argcheck_T arg4_string_string_any_string[] = {arg_string, arg_string, arg_any, arg_string};
ساکن argcheck_T arg4_string_string_نمبر_string[] = {arg_string, arg_string, arg_نمبر, arg_string};
ساکن argcheck_T arg4_string_نمبر_bool_bool[] = {arg_string, arg_نمبر, arg_bool, arg_bool};
/* Function specific argument types (not covered by the above) */
ساکن argcheck_T arg15_assert_fails[] = {arg_string_or_nr, arg_string_or_list_any, arg_any, arg_نمبر, arg_string};
ساکن argcheck_T arg34_assert_inrange[] = {arg_float_or_nr, arg_float_or_nr, arg_float_or_nr, arg_string};
ساکن argcheck_T arg4_browse[] = {arg_bool, arg_string, arg_string, arg_string};
ساکن argcheck_T arg23_chanexpr[] = {arg_chan_or_کام, arg_any, arg_dict_any};
ساکن argcheck_T arg23_chanraw[] = {arg_chan_or_کام, arg_string_or_blob, arg_dict_any};
ساکن argcheck_T arg24_گنتی[] = {arg_string_list_tuple_or_dict, arg_any, arg_bool, arg_نمبر};
ساکن argcheck_T arg13_cursor[] = {arg_cursor1, arg_نمبر, arg_نمبر};
ساکن argcheck_T arg12_deepcopy[] = {arg_any, arg_bool};
ساکن argcheck_T arg12_execute[] = {arg_string_or_list_string, arg_string};
ساکن argcheck_T arg12_getchar[] = {arg_bool_or_nr, arg_dict_any};
ساکن argcheck_T arg23_extend[] = {arg_list_or_dict_or_blob_mod, arg_same_as_پچھلا, arg_extend3};
ساکن argcheck_T arg23_extendnew[] = {arg_list_or_dict_or_blob, arg_same_struct_as_پچھلا, arg_extend3};
ساکن argcheck_T arg23_get[] = {arg_get1, arg_string_or_nr, arg_any};
ساکن argcheck_T arg14_glob[] = {arg_string, arg_bool, arg_bool, arg_bool};
ساکن argcheck_T arg25_globpath[] = {arg_string, arg_string, arg_bool, arg_bool, arg_bool};
ساکن argcheck_T arg24_اشاریہ[] = {arg_list_or_tuple_or_blob, arg_شے_کا_پچھلا, arg_نمبر, arg_bool};
ساکن argcheck_T arg23_اشاریہ[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
ساکن argcheck_T arg23_داخل_کرو[] = {arg_list_or_blob, arg_شے_کا_پچھلا, arg_نمبر};
ساکن argcheck_T arg1_len[] = {arg_len1};
ساکن argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
ساکن argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
ساکن argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
ساکن argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
ساکن argcheck_T arg2_instanceof[] = {arg_object, varargs_class, تہی };
ساکن argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
ساکن argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
ساکن argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_نمبر, arg_نمبر, arg_dict_any};
ساکن argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_نمبر, arg_نمبر, arg_dict_any};
ساکن argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
ساکن argcheck_T arg45_matchbufline[] = {arg_بفر, arg_string, arg_lnum, arg_lnum, arg_dict_any};
ساکن argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
ساکن argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
ساکن argcheck_T arg24_remote_اظہاریہ[] = {arg_string, arg_string, arg_string, arg_نمبر};
ساکن argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_نمبر};
ساکن argcheck_T arg2_repeat[] = {arg_repeat1, arg_نمبر};
ساکن argcheck_T arg15_تلاش[] = {arg_string, arg_string, arg_نمبر, arg_نمبر, arg_string_or_func};
ساکن argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_نمبر, arg_نمبر};
ساکن argcheck_T arg3_setbufline[] = {arg_بفر, arg_lnum, arg_str_or_nr_or_list};
ساکن argcheck_T arg2_setline[] = {arg_lnum, arg_any};
ساکن argcheck_T arg24_setloclist[] = {arg_نمبر, arg_list_any, arg_string, arg_dict_any};
ساکن argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
ساکن argcheck_T arg23_settagstack[] = {arg_نمبر, arg_dict_any, arg_string};
ساکن argcheck_T arg02_sign_getplaced[] = {arg_بفر, arg_dict_any};
ساکن argcheck_T arg45_sign_رکھو[] = {arg_نمبر, arg_string, arg_string, arg_بفر, arg_dict_any};
ساکن argcheck_T arg23_قاش[] = {arg_slice1, arg_نمبر, arg_نمبر};
ساکن argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_ترتیب_دو_how, arg_dict_any};
ساکن argcheck_T arg24_strpart[] = {arg_string, arg_نمبر, arg_نمبر, arg_bool};
ساکن argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
ساکن argcheck_T arg23_win_execute[] = {arg_نمبر, arg_string_or_list_string, arg_string};
ساکن argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
ساکن argcheck_T arg24_مطابقت_func[] = {arg_string_or_list_any, arg_string, arg_نمبر, arg_نمبر};

// Can be used by functions called through "f_retfunc" to create new types.
ساکن garray_T *current_type_gap = تہی;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    ساکن type_T *
ret_void(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_void;
}
    ساکن type_T *
ret_any(صحیح	argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_any;
}
    ساکن type_T *
ret_bool(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_bool;
}
    ساکن type_T *
ret_نمبر_bool(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_نمبر_bool;
}
    ساکن type_T *
ret_نمبر(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_نمبر;
}
    ساکن type_T *
ret_float(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_float;
}
    ساکن type_T *
ret_string(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_string;
}
    ساکن type_T *
ret_list_any(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_list_any;
}
    ساکن type_T *
ret_list_نمبر(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    واپس &t_list_نمبر;
}
    ساکن type_T *
ret_list_string(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    واپس &t_list_string;
}
    ساکن type_T *
ret_list_dict_any(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    واپس &t_list_dict_any;
}
    ساکن type_T *
ret_list_اشیاء(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    واپس &t_list_list_any;
}
    ساکن type_T *
ret_list_list_any(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    واپس &t_list_list_any;
}
    ساکن type_T *
ret_list_string_اشیاء(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    واپس &t_list_list_string;
}
    ساکن type_T *
ret_list_regionpos(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    واپس &t_list_list_list_نمبر;
}
    ساکن type_T *
ret_tuple_any(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_tuple_any;
}
    ساکن type_T *
ret_dict_any(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_dict_any;
}
    ساکن type_T *
ret_کام_info(صحیح argcount,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type)
{
    اگر (argcount == 0)
    {
	*decl_type = &t_list_any;
	واپس &t_list_کام;
    }
    واپس &t_dict_any;
}
    ساکن type_T *
ret_dict_نمبر(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_dict_نمبر;
}
    ساکن type_T *
ret_dict_string(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_dict_string;
}
    ساکن type_T *
ret_blob(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_blob;
}
    ساکن type_T *
ret_func_any(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_func_any;
}
    ساکن type_T *
ret_func_unknown(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_func_unknown;
}
    ساکن type_T *
ret_channel(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_channel;
}
    ساکن type_T *
ret_کام(صحیح argcount غیر_مستعمل,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    واپس &t_کام;
}
    ساکن type_T *
ret_پہلا_arg(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	واپس argtypes[0].type_curr;
    }
    واپس &t_void;
}
    ساکن type_T *
ret_قاش(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	اگر (argtypes[0].type_decl != تہی)
	{
	    انتخاب (argtypes[0].type_decl->tt_type)
	    {
		حالت VAR_STRING: *decl_type = &t_string; توڑو;
		حالت VAR_BLOB: *decl_type = &t_blob; توڑو;
		حالت VAR_LIST: *decl_type = &t_list_any; توڑو;
		حالت VAR_TUPLE: *decl_type = &t_tuple_any; توڑو;
		طےشدہ: توڑو;
	    }
	}
	واپس argtypes[0].type_curr;
    }
    واپس &t_void;
}
    ساکن type_T *
ret_copy(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	اگر (argtypes[0].type_decl != تہی)
	{
	    اگر (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    ورنہ اگر (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    ورنہ
		*decl_type = argtypes[0].type_decl;
	}
	اگر (argtypes[0].type_curr != تہی)
	{
	    اگر (argtypes[0].type_curr->tt_type == VAR_LIST)
		واپس &t_list_any;
	    ورنہ اگر (argtypes[0].type_curr->tt_type == VAR_DICT)
		واپس &t_dict_any;
	}
	واپس argtypes[0].type_curr;
    }
    واپس &t_void;
}
    ساکن type_T *
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
		واپس &t_list_any;
	    اگر (argtypes[0].type_curr->tt_type == VAR_DICT)
		واپس &t_dict_any;
	    اگر (argtypes[0].type_curr->tt_type == VAR_BLOB)
		واپس &t_blob;
	}
	واپس argtypes[0].type_curr;
    }
    واپس &t_void;
}
    ساکن type_T *
ret_repeat(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type غیر_مستعمل)
{
    اگر (argcount == 0)
	واپس &t_any;
    اگر (argtypes[0].type_curr == &t_نمبر)
	واپس &t_string;
    واپس argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    ساکن type_T *
ret_پہلا_cont(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type غیر_مستعمل)
{
    اگر (argcount > 0)
    {
	اگر (argtypes[0].type_curr->tt_type == VAR_LIST)
	    واپس &t_list_any;
	اگر (argtypes[0].type_curr->tt_type == VAR_DICT)
	    واپس &t_dict_any;
	اگر (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    واپس argtypes[0].type_curr;
    }
    واپس &t_any;
}
// for get(): the type of the item when a missing item has the same type
    ساکن type_T *
ret_get(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*شے = تہی;

	اگر (t->tt_type == VAR_BLOB)
	    شے = &t_نمبر;
	ورنہ اگر (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    شے = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	اگر (شے != تہی && (argcount == 3
			? equal_type(شے, argtypes[2].type_curr, 0)
			: شے->tt_type == VAR_نمبر))
	{
	    اگر (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    واپس شے;
	}
    }
    واپس &t_any;
}
// for getline()
    ساکن type_T *
ret_getline(صحیح argcount,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    اگر (argcount == 1)
	واپس &t_string;
    *decl_type = &t_list_any;
    واپس &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    ساکن type_T *
ret_string_or_any_1(صحیح argcount,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    اگر (argcount < 1)
	واپس &t_string;
    واپس &t_any;
}
    ساکن type_T *
ret_string_or_any_2(صحیح argcount,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    اگر (argcount < 2)
	واپس &t_string;
    واپس &t_any;
}
    ساکن type_T *
ret_string_or_any_3(صحیح argcount,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    اگر (argcount < 3)
	واپس &t_string;
    واپس &t_any;
}
    ساکن type_T *
ret_string_or_any_4(صحیح argcount,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    اگر (argcount < 4)
	واپس &t_string;
    واپس &t_any;
}
// for abs()
    ساکن type_T *
ret_abs(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type غیر_مستعمل)
{
    اگر (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	واپس &t_float;
    واپس &t_نمبر;
}
// for sign_define()
    ساکن type_T *
ret_sign_define(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type غیر_مستعمل)
{
    اگر (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	واپس &t_list_نمبر;
    واپس &t_نمبر;
}
// for values(): list of member of first argument
    ساکن type_T *
ret_list_member(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	اگر (current_type_gap != تہی
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	ورنہ
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	اگر (current_type_gap != تہی
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    واپس get_list_type(t->tt_member, current_type_gap);
    }
    واپس &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    ساکن type_T *
ret_list_or_dict_0(صحیح argcount,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type)
{
    اگر (argcount > 0)
	واپس &t_dict_any;
    *decl_type = &t_list_any;
    واپس &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    ساکن type_T *
ret_list_or_dict_1(صحیح argcount,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type)
{
    اگر (argcount > 1)
	واپس &t_dict_any;
    *decl_type = &t_list_any;
    واپس &t_list_dict_any;
}

    ساکن type_T *
ret_argv(صحیح argcount,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type)
{
    // argv() returns list of strings
    اگر (argcount == 0)
    {
	*decl_type = &t_list_any;
	واپس &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    واپس &t_any;
}

    ساکن type_T *
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
		واپس argtypes[0].type_curr;
	    }
	    اگر (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    واپس argtypes[0].type_curr->tt_member;
	}
	اگر (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    واپس &t_نمبر;
    }
    واپس &t_any;
}

    ساکن type_T *
ret_max_min(صحیح argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    اگر (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	اگر (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	ورنہ
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	اگر (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	ورنہ
	    t = &t_any;
	واپس t;
    }
    واپس &t_any;
}

    ساکن type_T *
ret_getreg(صحیح argcount,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    اگر (argcount == 3)
    {
	*decl_type = &t_list_any;
	واپس &t_list_string;
    }
    واپس &t_string;
}

    ساکن type_T *
ret_virtcol(صحیح argcount,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    اگر (argcount > 1)
    {
	*decl_type = &t_list_any;
	واپس &t_list_نمبر;
    }
    واپس &t_نمبر;
}

    ساکن type_T *
ret_maparg(صحیح argcount,
	type2_T *argtypes غیر_مستعمل,
	type_T	**decl_type غیر_مستعمل)
{
    // Assume that if the fourth argument is passed it's non-zero
    اگر (argcount == 4)
	واپس &t_dict_any;
    واپس &t_string;
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
قسمتعریف ساخت
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
    type_T	*(*f_retfunc)(صحیح argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    خالی	(*f_func)(typval_T *دلائل, typval_T *rvar);
				// implementation of function
} funcentry_T;

// Set f_max_argc to VARGS for varargs.
#تعریف_کرو VARGS    CHAR_MAX

// values for f_argtype; zero means it cannot be used as a method
#تعریف_کرو FEARG_1	    0x01    // base is the first argument
#تعریف_کرو FEARG_2     0x02    // base is the second argument
#تعریف_کرو FEARG_3     0x03    // base is the third argument
#تعریف_کرو FEARG_4     0x04    // base is the fourth argument
#تعریف_کرو FEARG_نقاب  0x0F    // bits in f_argtype used as argument index
#تعریف_کرو FE_X	    0x10    // builtin accepts a non-value (class, typealias)

#اگر defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#ورنہ
# define MATH_FUNC(name) تہی
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TIMERS
# define TIMER_FUNC(name) name
#ورنہ
# define TIMER_FUNC(name) تہی
#اگر_ختم
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
# define کام_FUNC(name) name
#ورنہ
# define کام_FUNC(name) تہی
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#ورنہ
# define PROP_FUNC(name) تہی
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
# define SIGN_FUNC(name) name
#ورنہ
# define SIGN_FUNC(name) تہی
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SOUND
# define SOUND_FUNC(name) name
#ورنہ
# define SOUND_FUNC(name) تہی
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
# define رکن_FUNC(name) name
#ورنہ
# define رکن_FUNC(name) تہی
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#ورنہ
# define TABPANEL_FUNC(name) تہی
#اگر_ختم
#اگر_تعریف_شدہ FEAT_IMAGE
# define IMAGE_FUNC(name) name
#ورنہ
# define IMAGE_FUNC(name) تہی
#اگر_ختم

ساکن ثابت funcentry_T global_functions[] =
{
    {"abs",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_abs,	    f_abs},
    {"acos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_acos},
    {"add",		2, 2, FEARG_1,	    arg2_listblobmod_شے,
			"object, expr",
			ret_پہلا_arg,	    f_جوڑو},
    {"and",		2, 2, FEARG_1,	    arg2_نمبر,
			"expr1, expr2",
			ret_نمبر,	    f_and},
    {"append",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_نمبر_bool,    f_append},
    {"appendbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_نمبر_bool,    f_appendbufline},
    {"argc",		0, 1, 0,	    arg1_نمبر,
			"winid",
			ret_نمبر,	    f_argc},
    {"argidx",		0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_argidx},
    {"arglistid",	0, 2, 0,	    arg2_نمبر,
			"winnr, tabnr",
			ret_نمبر,	    f_arglistid},
    {"argv",		0, 2, 0,	    arg2_نمبر,
			"nr, winid",
			ret_argv,	    f_argv},
    {"asin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_asin},
    {"assert_beeps",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_نمبر_bool,    f_assert_beeps},
    {"assert_equal",	2, 3, FEARG_2,	    تہی,
			"expected, actual, msg",
			ret_نمبر_bool,    f_assert_equal},
    {"assert_equalfile", 2, 3, FEARG_1,	    arg3_string,
			"fname_one, fname_two, msg",
			ret_نمبر_bool,    f_assert_equalfile},
    {"assert_exception", 1, 2, 0,	    arg2_string,
			"error, msg",
			ret_نمبر_bool,    f_assert_exception},
    {"assert_fails",	1, 5, FEARG_1,	    arg15_assert_fails,
			"cmd, error, msg, lnum, context",
			ret_نمبر_bool,    f_assert_fails},
    {"assert_false",	1, 2, FEARG_1,	    تہی,
			"actual, msg",
			ret_نمبر_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_نمبر_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_نمبر_bool,    f_assert_مطابقت},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_نمبر_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    تہی,
			"expected, actual, msg",
			ret_نمبر_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_نمبر_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_نمبر_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    تہی,
			"actual, msg",
			ret_نمبر_bool,    f_assert_true},
    {"atan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_atan},
    {"atan2",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"expr1, expr2",
			ret_float,	    f_atan2},
    {"autocmd_add",	1, 1, FEARG_1,	    arg1_list_any,
			"acmds",
			ret_bool,	    f_autocmd_جوڑو},
    {"autocmd_delete",	1, 1, FEARG_1,	    arg1_list_any,
			"acmds",
			ret_bool,	    f_autocmd_delete},
    {"autocmd_get",	0, 1, FEARG_1,	    arg1_dict_any,
			"opts",
			ret_list_dict_any,  f_autocmd_get},
    {"balloon_gettext",	0, 0, 0,	    تہی,
			تہی,
			ret_string,
#اگر_تعریف_شدہ FEAT_BEVAL
	    f_balloon_gettext
#ورنہ
	    تہی
#اگر_ختم
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#اگر_تعریف_شدہ FEAT_BEVAL
	    f_balloon_show
#ورنہ
	    تہی
#اگر_ختم
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#اگر defined(FEAT_BEVAL_رکن)
	    f_balloon_بانٹو
#ورنہ
	    تہی
#اگر_ختم
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
			ret_list_نمبر,    f_blob2list},
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
			ret_نمبر,	    f_bufadd},
    {"bufexists",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_نمبر_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_بفر,	// obsolete
			"buf",
			ret_نمبر_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_بفر,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_بفر,	// obsolete
			"buf",
			ret_نمبر,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_نمبر_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_نمبر_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_بفر_bool,
			"buf, create",
			ret_نمبر,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_نمبر,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_نمبر,	    f_bufwinnr},
    {"byte2line",	1, 1, FEARG_1,	    arg1_نمبر,
			"byte",
			ret_نمبر,	    f_byte2line},
    {"byteidx",		2, 3, FEARG_1,	    arg3_string_نمبر_bool,
			"expr, nr, utf16",
			ret_نمبر,	    f_byteidx},
    {"byteidxcomp",	2, 3, FEARG_1,	    arg3_string_نمبر_bool,
			"expr, nr, utf16",
			ret_نمبر,	    f_byteidxcomp},
    {"call",		2, 3, FEARG_1,	    arg3_any_list_dict,
			"func, arglist, dict",
			ret_any,	    f_call},
    {"ceil",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ceil},
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_کام,
			"handle",
			ret_نمبر_bool,    کام_FUNC(f_ch_canread)},
    {"ch_close",	1, 1, FEARG_1,	    arg1_chan_or_کام,
			"handle",
			ret_void,	    کام_FUNC(f_ch_close)},
    {"ch_close_in",	1, 1, FEARG_1,	    arg1_chan_or_کام,
			"handle",
			ret_void,	    کام_FUNC(f_ch_close_اندر)},
    {"ch_evalexpr",	2, 3, FEARG_1,	    arg23_chanexpr,
			"handle, expr, options",
			ret_any,	    کام_FUNC(f_ch_evalexpr)},
    {"ch_evalraw",	2, 3, FEARG_1,	    arg23_chanraw,
			"handle, string, options",
			ret_any,	    کام_FUNC(f_ch_evalraw)},
    {"ch_getbufnr",	2, 2, FEARG_1,	    arg2_chan_or_کام_string,
			"handle, what",
			ret_نمبر,	    کام_FUNC(f_ch_getbufnr)},
    {"ch_getjob",	1, 1, FEARG_1,	    arg1_chan_or_کام,
			"channel",
			ret_کام,	    کام_FUNC(f_ch_getjob)},
    {"ch_info",		1, 1, FEARG_1,	    arg1_chan_or_کام,
			"handle",
			ret_dict_any,	    کام_FUNC(f_ch_info)},
    {"ch_listen",	1, 2, FEARG_1,	    arg2_string_dict,
			"address, options",
			ret_channel,	    کام_FUNC(f_ch_listen)},
    {"ch_log",		1, 2, FEARG_1,	    arg2_string_chan_or_کام,
			"msg, handle",
			ret_void,	    f_ch_log},
    {"ch_logfile",	1, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_void,	    f_ch_logfile},
    {"ch_open",		1, 2, FEARG_1,	    arg2_string_dict,
			"address, options",
			ret_channel,	    کام_FUNC(f_ch_open)},
    {"ch_read",		1, 2, FEARG_1,	    arg2_chan_or_کام_dict,
			"handle, options",
			ret_string,	    کام_FUNC(f_ch_read)},
    {"ch_readblob",	1, 2, FEARG_1,	    arg2_chan_or_کام_dict,
			"handle, options",
			ret_blob,	    کام_FUNC(f_ch_readblob)},
    {"ch_readraw",	1, 2, FEARG_1,	    arg2_chan_or_کام_dict,
			"handle, options",
			ret_string,	    کام_FUNC(f_ch_readraw)},
    {"ch_sendexpr",	2, 3, FEARG_1,	    arg23_chanexpr,
			"handle, expr, options",
			ret_any,	    کام_FUNC(f_ch_sendexpr)},
    {"ch_sendraw",	2, 3, FEARG_1,	    arg23_chanraw,
			"handle, expr, options",
			ret_void,	    کام_FUNC(f_ch_sendraw)},
    {"ch_setoptions",	2, 2, FEARG_1,	    arg2_chan_or_کام_dict,
			"handle, options",
			ret_void,	    کام_FUNC(f_ch_setoptions)},
    {"ch_status",	1, 2, FEARG_1,	    arg2_chan_or_کام_dict,
			"handle, options",
			ret_string,	    کام_FUNC(f_ch_حالت)},
    {"changenr",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_changenr},
    {"char2nr",		1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_نمبر,	    f_char2nr},
    {"charclass",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_نمبر,	    f_charclass},
    {"charcol",		1, 2, FEARG_1,	    arg2_string_or_list_نمبر,
			"expr, winid",
			ret_نمبر,	    f_charcol},
    {"charidx",		2, 4, FEARG_1,	    arg4_string_نمبر_bool_bool,
			"string, idx, countcc, utf16",
			ret_نمبر,	    f_charidx},
    {"chdir",		1, 2, FEARG_1,	    arg2_string,
			"dir, scope",
			ret_string,	    f_chdir},
    {"cindent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_نمبر,	    f_cindent},
    {"clearmatches",	0, 1, FEARG_1,	    arg1_نمبر,
			"win",
			ret_void,	    f_clearmatches},
    {"cmdcomplete_info",0, 0, 0,	    تہی,
			تہی,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_نمبر,
			"expr, winid",
			ret_نمبر,	    f_ستون},
    {"complete",	2, 2, FEARG_2,	    arg2_نمبر_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_نمبر,	    f_complete_جوڑو},
    {"complete_check",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_نمبر_string,
			"msg, choices, default, type",
			ret_نمبر,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    تہی,
			"expr",
			ret_copy,	    f_copy},
    {"cos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cos},
    {"cosh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cosh},
    {"count",		2, 4, FEARG_1,	    arg24_گنتی,
			"comp, expr, ic, start",
			ret_نمبر,	    f_گنتی},
    {"cscope_connection",0,3, 0,	    arg3_نمبر_string_string,
			"num, dbpath, prepend",
			ret_نمبر,	    f_cscope_connection},
    {"cursor",		1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_نمبر,	    f_cursor},
    {"debugbreak",	1, 1, FEARG_1,	    arg1_نمبر,
			"pid",
			ret_نمبر,
#اگر_تعریف_شدہ MSWIN
	    f_debugbreak
#ورنہ
	    تہی
#اگر_ختم
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_نمبر_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_بفر_lnum_lnum,
			"buf, first, last",
			ret_نمبر_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر_bool,    f_did_filetype},
    {"diff",		2, 3, FEARG_1,	    arg3_diff,
			"fromlist, tolist, options",
			ret_string_or_any_3,  f_diff},
    {"diff_filler",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_نمبر,	    f_diff_filler},
    {"diff_hlID",	2, 2, FEARG_1,	    arg2_lnum_نمبر,
			"lnum, col",
			ret_نمبر,	    f_diff_hlID},
    {"digraph_get",	1, 1, FEARG_1,	    arg1_string,
			"chars",
			ret_string,	    f_digraph_get},
    {"digraph_getlist",	0, 1, FEARG_1,	    arg1_bool,
			"listall",
			ret_list_string_اشیاء, f_digraph_getlist},
    {"digraph_set",	2, 2, FEARG_1,	    arg2_string,
			"chars, digraph",
			ret_bool,	f_digraph_سیٹ},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    تہی,
			"expr",
			ret_نمبر_bool,    f_خالی_خانے},
    {"environ",		0, 0, 0,	    تہی,
			تہی,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    تہی,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر_bool,    f_eventhandler},
    {"executable",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_نمبر,	    f_executable},
    {"execute",		1, 2, FEARG_1,	    arg12_execute,
			"command, silent",
			ret_string,	    f_execute},
    {"exepath",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_exepath},
    {"exists",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_نمبر_bool,    f_exists},
    {"exists_compiled",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_نمبر_bool,    f_exists_compiled},
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
			ret_پہلا_cont,	    f_extendnew},
    {"feedkeys",	1, 2, FEARG_1,	    arg2_string,
			"string, mode",
			ret_void,	    f_feedkeys},
    {"file_readable",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"file",
			ret_نمبر_bool,    f_filereadable},
    {"filecopy",	2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_نمبر_bool,    f_filecopy},
    {"filereadable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_نمبر_bool,    f_filereadable},
    {"filewritable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_نمبر,	    f_filewritable},
    {"filter",		2, 2, FEARG_1,	    arg2_filter,
			"expr1, expr2",
			ret_پہلا_arg,	    f_filter},
    {"finddir",		1, 3, FEARG_1,	    arg3_string_string_نمبر,
			"name, path, count",
			ret_string_or_any_3,	    f_finddir},
    {"findfile",	1, 3, FEARG_1,	    arg3_string_string_نمبر,
			"name, path, count",
			ret_string_or_any_3,	    f_findfile},
    {"flatten",		1, 2, FEARG_1,	    arg2_list_any_نمبر,
			"list, maxdepth",
			ret_list_any,	    f_flatten},
    {"flattennew",	1, 2, FEARG_1,	    arg2_list_any_نمبر,
			"list, maxdepth",
			ret_list_any,	    f_flattennew},
    {"float2nr",	1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_نمبر,	    f_float2nr},
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
    {"foldclosed",	1, 2, FEARG_1,	    arg2_lnum_نمبر,
			"lnum, winid",
			ret_نمبر,	    f_foldclosed},
    {"foldclosedend",	1, 2, FEARG_1,	    arg2_lnum_نمبر,
			"lnum, winid",
			ret_نمبر,	    f_foldclosedend},
    {"foldlevel",	1, 2, FEARG_1,	    arg2_lnum_نمبر,
			"lnum, winid",
			ret_نمبر,	    f_foldlevel},
    {"foldtext",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_نمبر,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_پہلا_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    تہی,
			تہی,
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
    {"getbgcolor",	0, 0, 0,	    تہی,
			تہی,
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
    {"getcellpixels",	0, 0, 0,	    تہی,
			تہی,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    تہی,
			تہی,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_نمبر,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    تہی,
			تہی,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_getcmdwintype},
    {"getcompletion",	2, 3, FEARG_1,	    arg3_string_string_bool,
			"pat, type, filtered",
			ret_list_string,    f_getcompletion},
    {"getcompletiontype", 1, 1, FEARG_1,    arg1_string,
			"pat",
			ret_string,	    f_getcompletiontype},
    {"getcurpos",	0, 1, FEARG_1,	    arg1_نمبر,
			"winid",
			ret_list_نمبر,    f_getcurpos},
    {"getcursorcharpos", 0, 1, FEARG_1,	    arg1_نمبر,
			"winid",
			ret_list_نمبر,    f_getcursorcharpos},
    {"getcwd",		0, 2, FEARG_1,	    arg2_نمبر,
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
			ret_نمبر,	    f_getfsize},
    {"getftime",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_نمبر,	    f_getftime},
    {"getftype",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getftype},
    {"getimstatus",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر_bool,    f_getimstatus},
    {"getinfo",		2, 3, FEARG_2,	    arg3_string_string_dict,
			"kind, name, opts",
			ret_dict_any,	    f_getinfo},
    {"getjumplist",	0, 2, FEARG_1,	    arg2_نمبر,
			"winnr, tabnr",
			ret_list_any,	    f_getjumplist},
    {"getline",		1, 2, FEARG_1,	    arg2_lnum,
			"lnum, end",
			ret_getline,	    f_getline},
    {"getloclist",	1, 2, 0,	    arg2_نمبر_dict_any,
			"nr, what",
			ret_list_or_dict_1, f_getloclist},
    {"getmarklist",	0, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_نمبر,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    تہی,
			تہی,
			ret_dict_نمبر,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_getpid},
    {"getpos",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_نمبر,    f_getpos},
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
    {"getstacktrace",	0, 0, 0,	    تہی,
			تہی,
			ret_list_dict_any,  f_getstacktrace},
    {"gettabinfo",	0, 1, FEARG_1,	    arg1_نمبر,
			"tabnr",
			ret_list_dict_any,  f_gettabinfo},
    {"gettabvar",	2, 3, FEARG_1,	    arg3_نمبر_string_any,
			"tabnr, varname, def",
			ret_any,	    f_gettabvar},
    {"gettabwinvar",	3, 4, FEARG_1,	    arg4_نمبر_نمبر_string_any,
			"tabnr, winnr, varname, def",
			ret_any,	    f_gettabwinvar},
    {"gettagstack",	0, 1, FEARG_1,	    arg1_نمبر,
			"winnr",
			ret_dict_any,	    f_gettagstack},
    {"gettext",		1, 2, FEARG_1,	    arg2_string,
			"text, package",
			ret_string,	    f_gettext},
    {"getwininfo",	0, 1, FEARG_1,	    arg1_نمبر,
			"winid",
			ret_list_dict_any,  f_getwininfo},
    {"getwinpos",	0, 1, FEARG_1,	    arg1_نمبر,
			"timeout",
			ret_list_نمبر,    f_getwinpos},
    {"getwinposx",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_getwinposy},
    {"getwinvar",	2, 3, FEARG_1,	    arg3_نمبر_string_any,
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
			ret_نمبر_bool,    f_has},
    {"has_key",		2, 2, FEARG_1,	    arg2_dict_any_string_or_nr,
			"dict, key",
			ret_نمبر_bool,    f_has_کلید},
    {"haslocaldir",	0, 2, FEARG_1,	    arg2_نمبر,
			"winnr, tabnr",
			ret_نمبر,	    f_haslocaldir},
    {"hasmapto",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"what, mode, abbr",
			ret_نمبر_bool,    f_hasmapto},
    {"highlightID",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_نمبر,	    f_hlID},
    {"highlight_exists",1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_نمبر_bool,    f_hlexists},
    {"histadd",		2, 2, FEARG_2,	    arg2_string,
			"history, item",
			ret_نمبر_bool,    f_histadd},
    {"histdel",		1, 2, FEARG_1,	    arg2_string_string_or_نمبر,
			"history, item",
			ret_نمبر_bool,    f_histdel},
    {"histget",		1, 2, FEARG_1,	    arg2_string_نمبر,
			"history, index",
			ret_string,	    f_histget},
    {"histnr",		1, 1, FEARG_1,	    arg1_string,
			"history",
			ret_نمبر,	    f_histnr},
    {"hlID",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_نمبر,	    f_hlID},
    {"hlexists",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_نمبر_bool,    f_hlexists},
    {"hlget",		0, 2, FEARG_1,	    arg2_string_bool,
			"name, resolve",
			ret_list_dict_any,  f_hlget},
    {"hlset",		1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_نمبر_bool,    f_hlset},
    {"hostname",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    تہی,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_نمبر,	    IMAGE_FUNC(f_image_جوڑو)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_نمبر,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_نمبر,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_نمبر,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_اشاریہ,
			"object, expr, start, ic",
			ret_نمبر,	    f_اشاریہ},
    {"indexof",		2, 3, FEARG_1,	    arg23_اشاریہ,
			"object, expr, opts",
			ret_نمبر,	    f_indexof},
    {"input",		1, 3, FEARG_1,	    arg3_string,
			"prompt, text, completion",
			ret_string,	    f_داخلہ},
    {"inputdialog",	1, 3, FEARG_1,	    arg3_string,
			"prompt, text, cancelreturn",
			ret_string,	    f_inputdialog},
    {"inputlist",	1, 1, FEARG_1,	    arg1_list_string,
			"textlist",
			ret_نمبر,	    f_inputlist},
    {"inputrestore",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_داخل_کرو,
			"object, item, idx",
			ret_پہلا_arg,	    f_داخل_کرو},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    تہی,
			تہی,
			ret_void,	    f_مداخلت},
    {"invert",		1, 1, FEARG_1,	    arg1_نمبر,
			"expr",
			ret_نمبر,	    f_invert},
    {"isabsolutepath",	1, 1, FEARG_1,	    arg1_string,
			"path",
			ret_نمبر_bool,    f_isabsolutepath},
    {"isdirectory",	1, 1, FEARG_1,	    arg1_string,
			"directory",
			ret_نمبر_bool,    f_isdirectory},
    {"isinf",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_نمبر,	    MATH_FUNC(f_isinf)},
    {"islocked",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_نمبر_bool,    f_islocked},
    {"isnan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_نمبر_bool,    MATH_FUNC(f_isnan)},
    {"items",		1, 1, FEARG_1,	    arg1_list_tuple_dict_blob_or_string,
			"expr",
			ret_list_اشیاء,	    f_اشیاء},
    {"job_getchannel",	1, 1, FEARG_1,	    arg1_کام,
			"job",
			ret_channel,	    کام_FUNC(f_کام_getchannel)},
    {"job_info",	0, 1, FEARG_1,	    arg1_کام,
			"job",
			ret_کام_info,	    کام_FUNC(f_کام_info)},
    {"job_setoptions",	2, 2, FEARG_1,	    arg2_کام_dict,
			"job, options",
			ret_void,	    کام_FUNC(f_کام_setoptions)},
    {"job_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"command, options",
			ret_کام,	    کام_FUNC(f_کام_آغاز)},
    {"job_status",	1, 1, FEARG_1,	    arg1_کام,
			"job",
			ret_string,	    کام_FUNC(f_کام_حالت)},
    {"job_stop",	1, 2, FEARG_1,	    arg2_کام_string_or_نمبر,
			"job, how",
			ret_نمبر_bool,    کام_FUNC(f_کام_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    تہی,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    تہی,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_کلیدیں},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    تہی,	// obsolete
			تہی,
			ret_نمبر,	    f_آخری_بفر_nr},
    {"len",		1, 1, FEARG_1,	    arg1_len,
			"expr",
			ret_نمبر,	    f_len},
    {"libcall",		3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_string,	    f_libcall},
    {"libcallnr",	3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_نمبر,	    f_libcallnr},
    {"line",		1, 2, FEARG_1,	    arg2_string_نمبر,
			"expr, winid",
			ret_نمبر,	    f_سطر},
    {"line2byte",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_نمبر,	    f_line2byte},
    {"lispindent",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_نمبر,	    f_lispindent},
    {"list2blob",	1, 1, FEARG_1,	    arg1_list_نمبر,
			"list",
			ret_blob,	    f_list2blob},
    {"list2str",	1, 2, FEARG_1,	    arg2_list_نمبر_bool,
			"list, utf8",
			ret_string,	    f_list2str},
    {"list2tuple",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_tuple_any,	    f_list2tuple},
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_بفر_bool_or_dict,
			"callback, buf, options",
			ret_نمبر,	    f_listener_جوڑو},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_نمبر,
			"id",
			ret_نمبر_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_localtime},
    {"log",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_log},
    {"log10",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_log10},
    {"luaeval",		1, 2, FEARG_1,	    arg2_string_any,
			"expr1, expr2",
			ret_any,
#اگر_تعریف_شدہ FEAT_LUA
		f_luaeval
#ورنہ
		تہی
#اگر_ختم
			},
    {"map",		2, 2, FEARG_1,	    arg2_map,
			"expr1, expr2",
			ret_پہلا_cont,	    f_map},
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
			ret_پہلا_cont,	    f_mapnew},
    {"mapset",		1, 3, FEARG_1,	    arg3_string_or_dict_bool_dict,
			"mode, abbr, dict",
			ret_void,	    f_mapset},
    {"match",		2, 4, FEARG_1,	    arg24_مطابقت_func,
			"expr, pat, start, count",
			ret_نمبر,	    f_مطابقت},
    {"matchadd",	2, 5, FEARG_1,	    arg25_matchadd,
			"group, pattern, priority, id, dict",
			ret_نمبر,	    f_matchadd},
    {"matchaddpos",	2, 5, FEARG_1,	    arg25_matchaddpos,
			"group, pos, priority, id, dict",
			ret_نمبر,	    f_matchaddpos},
    {"matcharg",	1, 1, FEARG_1,	    arg1_نمبر,
			"nr",
			ret_list_string,    f_matcharg},
    {"matchbufline",	4, 5, FEARG_1,	    arg45_matchbufline,
			"buf, pat, lnum, end, dict",
			ret_list_any,	    f_matchbufline},
    {"matchdelete",	1, 2, FEARG_1,	    arg2_نمبر,
			"id, win",
			ret_نمبر_bool,    f_matchdelete},
    {"matchend",	2, 4, FEARG_1,	    arg24_مطابقت_func,
			"expr, pat, start, count",
			ret_نمبر,	    f_matchend},
    {"matchfuzzy",	2, 3, FEARG_1,	    arg3_list_string_dict,
			"list, str, dict",
			ret_list_any,	    f_matchfuzzy},
    {"matchfuzzypos",	2, 3, FEARG_1,	    arg3_list_string_dict,
			"list, str, dict",
			ret_list_list_any,  f_matchfuzzypos},
    {"matchlist",	2, 4, FEARG_1,	    arg24_مطابقت_func,
			"expr, pat, start, count",
			ret_list_string,    f_matchlist},
    {"matchstr",	2, 4, FEARG_1,	    arg24_مطابقت_func,
			"expr, pat, start, count",
			ret_string,	    f_matchstr},
    {"matchstrlist",	2, 3, FEARG_1,	    arg23_matchstrlist,
			"list, pat, dict",
			ret_list_any,	    f_matchstrlist},
    {"matchstrpos",	2, 4, FEARG_1,	    arg24_مطابقت_func,
			"expr, pat, start, count",
			ret_list_any,	    f_matchstrpos},
    {"max",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_max},
    {"menu_info",	1, 2, FEARG_1,	    arg2_string,
			"name, mode",
			ret_dict_any,
#اگر_تعریف_شدہ FEAT_MENU
	    f_menu_info
#ورنہ
	    تہی
#اگر_ختم
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_نمبر,
			"name, flags, prot",
			ret_نمبر_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_منوال},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#اگر_تعریف_شدہ FEAT_MZSCHEME
	    f_mzeval
#ورنہ
	    تہی
#اگر_ختم
			},
    {"nextnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_نمبر,	    f_nextnonblank},
    {"ngettext",	3, 4, FEARG_3,	    arg4_string_string_نمبر_string,
			"single, plural, number, domain",
			ret_string,	    f_ngettext},
    {"nr2char",		1, 2, FEARG_1,	    arg2_نمبر_bool,
			"expr, utf8",
			ret_string,	    f_nr2char},
    {"or",		2, 2, FEARG_1,	    arg2_نمبر,
			"expr1, expr2",
			ret_نمبر,	    f_or},
    {"pathshorten",	1, 2, FEARG_1,	    arg2_string_نمبر,
			"path, len",
			ret_string,	    f_pathshorten},
    {"perleval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#اگر_تعریف_شدہ FEAT_PERL
	    f_perleval
#ورنہ
	    تہی
#اگر_ختم
			},
    {"popup_atcursor",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_نمبر,	    PROP_FUNC(f_popup_atcursor)},
    {"popup_beval",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_نمبر,	    PROP_FUNC(f_popup_beval)},
    {"popup_clear",	0, 1, 0,	    arg1_bool,
			"force",
			ret_void,	    PROP_FUNC(f_popup_clear)},
    {"popup_close",	1, 2, FEARG_1,	    arg2_نمبر_any,
			"id, result",
			ret_void,	    PROP_FUNC(f_popup_close)},
    {"popup_create",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_نمبر,	    PROP_FUNC(f_popup_create)},
    {"popup_dialog",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_نمبر,	    PROP_FUNC(f_popup_dialog)},
    {"popup_filter_menu", 2, 2, 0,	    arg2_نمبر_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_menu)},
    {"popup_filter_yesno", 2, 2, 0,	    arg2_نمبر_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_yesno)},
    {"popup_findecho",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    PROP_FUNC(f_popup_findpreview)},
    {"popup_getoptions", 1, 1, FEARG_1,	    arg1_نمبر,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getoptions)},
    {"popup_getpos",	1, 1, FEARG_1,	    arg1_نمبر,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getpos)},
    {"popup_hide",	1, 1, FEARG_1,	    arg1_نمبر,
			"id",
			ret_void,	    PROP_FUNC(f_popup_hide)},
    {"popup_list",	0, 0, 0,	    تہی,
			تہی,
			ret_list_نمبر,    PROP_FUNC(f_popup_list)},
    {"popup_locate",	2, 2, 0,	    arg2_نمبر,
			"row, col",
			ret_نمبر,	    PROP_FUNC(f_popup_locate)},
    {"popup_menu",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_نمبر,	    PROP_FUNC(f_popup_menu)},
    {"popup_move",	2, 2, FEARG_1,	    arg2_نمبر_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_move)},
    {"popup_notification", 2, 2, FEARG_1,   arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_نمبر,	    PROP_FUNC(f_popup_notification)},
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_نمبر_بفر,
			"id, buf",
			ret_نمبر_bool,    PROP_FUNC(f_popup_setbuf)},
    {"popup_setoptions", 2, 2, FEARG_1,	    arg2_نمبر_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_setoptions)},
    {"popup_settext",	2, 2, FEARG_1,	    arg2_نمبر_string_or_list,
			"id, text",
			ret_void,	    PROP_FUNC(f_popup_settext)},
    {"popup_show",	1, 1, FEARG_1,	    arg1_نمبر,
			"id",
			ret_نمبر,	    PROP_FUNC(f_popup_show)},
    {"pow",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"x, y",
			ret_float,	    f_pow},
    {"preinserted",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_نمبر,	    f_prevnonblank},
    {"printf",		1, 19, FEARG_2,	    arg119_printf,
			"fmt, expr1",
			ret_string,	    f_printf},
    {"prompt_getprompt", 1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_string,	    کام_FUNC(f_prompt_getprompt)},
    {"prompt_setcallback", 2, 2, FEARG_1,   arg2_بفر_any,
			"buf, expr",
			ret_void,	    کام_FUNC(f_prompt_setcallback)},
    {"prompt_setinterrupt", 2, 2, FEARG_1,  arg2_بفر_any,
			"buf, expr",
			ret_void,	    کام_FUNC(f_prompt_setinterrupt)},
    {"prompt_setprompt", 2, 2, FEARG_1,	    arg2_بفر_string,
			"buf, text",
			ret_void,	    کام_FUNC(f_prompt_setprompt)},
    {"prop_add",	3, 3, FEARG_1,	    arg3_نمبر_نمبر_dict,
			"lnum, col, props",
			ret_نمبر,	    PROP_FUNC(f_prop_جوڑو)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_جوڑو_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_نمبر_نمبر_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_ڈھونڈو)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_نمبر_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_نمبر_نمبر,
			"props, lnum, lnum_end",
			ret_نمبر,	    PROP_FUNC(f_prop_remove)},
    {"prop_type_add",	2, 2, FEARG_1,	    arg2_string_dict,
			"name, props",
			ret_void,	    PROP_FUNC(f_prop_type_جوڑو)},
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
    {"pum_getpos",	0, 0, 0,	    تہی,
			تہی,
			ret_dict_نمبر,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#اگر_تعریف_شدہ FEAT_PYTHON3
	    f_py3eval
#ورنہ
	    تہی
#اگر_ختم
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#اگر_تعریف_شدہ FEAT_PYTHON
	    f_pyeval
#ورنہ
	    تہی
#اگر_ختم
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#ورنہ
	    تہی
#اگر_ختم
			},
    {"rand",		0, 1, FEARG_1,	    arg1_list_نمبر,
			"expr",
			ret_نمبر,	    f_rand},
    {"range",		1, 3, FEARG_1,	    arg3_نمبر,
			"expr, max, stride",
			ret_list_نمبر,    f_range},
    {"readblob",	1, 3, FEARG_1,	    arg3_string_نمبر_نمبر,
			"fname, offset, size",
			ret_blob,	    f_readblob},
    {"readdir",		1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_string,    f_readdir},
    {"readdirex",	1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_dict_any,  f_readdirex},
    {"readfile",	1, 3, FEARG_1,	    arg3_string_string_نمبر,
			"fname, type, max",
			ret_list_string,    f_readfile},
    {"redraw_listener_add", 1, 1, FEARG_1,  arg1_dict_any,
			"opts",
			ret_نمبر,	    f_redraw_listener_جوڑو},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_نمبر,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_reg_recording},
    {"reltime",		0, 2, FEARG_1,	    arg2_list_نمبر,
			"start, end",
			ret_list_نمبر,    f_reltime},
    {"reltimefloat",	1, 1, FEARG_1,	    arg1_list_نمبر,
			"time",
			ret_float,	    f_reltimefloat},
    {"reltimestr",	1, 1, FEARG_1,	    arg1_list_نمبر,
			"time",
			ret_string,	    f_reltimestr},
    {"remote_expr",	2, 4, FEARG_1,	    arg24_remote_اظہاریہ,
			"server, string, idvar, timeout",
			ret_string,	    f_remote_اظہاریہ},
    {"remote_foreground", 1, 1, FEARG_1,    arg1_string,
			"server",
			ret_void,	    f_remote_foreground},
    {"remote_peek",	1, 2, FEARG_1,	    arg2_string,
			"serverid, retvar",
			ret_نمبر,	    f_remote_peek},
    {"remote_read",	1, 2, FEARG_1,	    arg2_string_نمبر,
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
			ret_نمبر_bool,    f_rename},
    {"repeat",		2, 2, FEARG_1,	    arg2_repeat,
			"expr, count",
			ret_repeat,	    f_repeat},
    {"resolve",		1, 1, FEARG_1,	    arg1_string,
			"filename",
			ret_string,	    f_resolve},
    {"reverse",		1, 1, FEARG_1,	    arg1_الٹ,
			"object",
			ret_پہلا_arg,	    f_الٹ},
    {"round",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_باری},
    {"rubyeval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#اگر_تعریف_شدہ FEAT_RUBY
	    f_rubyeval
#ورنہ
	    تہی
#اگر_ختم
			},
    {"screenattr",	2, 2, FEARG_1,	    arg2_نمبر,
			"row, col",
			ret_نمبر,	    f_screenattr},
    {"screenchar",	2, 2, FEARG_1,	    arg2_نمبر,
			"row, col",
			ret_نمبر,	    f_screenchar},
    {"screenchars",	2, 2, FEARG_1,	    arg2_نمبر,
			"row, col",
			ret_list_نمبر,    f_screenchars},
    {"screencol",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_نمبر,
			"winid, lnum, col",
			ret_dict_نمبر,    f_screenpos},
    {"screenrow",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_screenrow},
    {"screenstring",	2, 2, FEARG_1,	    arg2_نمبر,
			"row, col",
			ret_string,	    f_screenstring},
    {"search",		1, 5, FEARG_1,	    arg15_تلاش,
			"pattern, flags, stopline, timeout, skip",
			ret_نمبر,	    f_تلاش},
    {"searchcount",	0, 1, FEARG_1,	    arg1_dict_any,
			"options",
			ret_dict_نمبر,    f_searchcount},
    {"searchdecl",	1, 3, FEARG_1,	    arg3_string_bool_bool,
			"name, global, thisblock",
			ret_نمبر_bool,    f_searchdecl},
    {"searchpair",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_نمبر,	    f_searchpair},
    {"searchpairpos",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_list_نمبر,    f_searchpairpos},
    {"searchpos",	1, 5, FEARG_1,	    arg15_تلاش,
			"pattern, flags, stopline, timeout, skip",
			ret_list_نمبر,    f_searchpos},
    {"server2client",	2, 2, FEARG_1,	    arg2_string,
			"clientid, string",
			ret_نمبر_bool,    f_server2client},
    {"serverlist",	0, 1, 0,	    arg1_dict_any,
			"dict",
			ret_string_or_any_1,	    f_serverlist},
    {"setbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_نمبر_bool,    f_setbufline},
    {"setbufvar",	3, 3, FEARG_3,	    arg3_بفر_string_any,
			"buf, varname, val",
			ret_void,	    f_setbufvar},
    {"setcellwidths",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_void,	    f_setcellwidths},
    {"setcharpos",	2, 2, FEARG_2,	    arg2_string_list_نمبر,
			"expr, list",
			ret_نمبر_bool,    f_setcharpos},
    {"setcharsearch",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_setcharsearch},
    {"setcmdline",	1, 2, FEARG_1,	    arg2_string_نمبر,
			"str, pos",
			ret_نمبر_bool,    f_setcmdline},
    {"setcmdpos",	1, 1, FEARG_1,	    arg1_نمبر,
			"pos",
			ret_نمبر_bool,    f_setcmdpos},
    {"setcursorcharpos", 1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_نمبر_bool,    f_setcursorcharpos},
    {"setenv",		2, 2, FEARG_2,	    arg2_string_any,
			"name, val",
			ret_void,	    f_setenv},
    {"setfperm",	2, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_نمبر_bool,    f_setfperm},
    {"setline",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_نمبر_bool,    f_setline},
    {"setloclist",	2, 4, FEARG_2,	    arg24_setloclist,
			"nr, list, action, what",
			ret_نمبر_bool,    f_setloclist},
    {"setmatches",	1, 2, FEARG_1,	    arg2_list_any_نمبر,
			"list, win",
			ret_نمبر_bool,    f_setmatches},
    {"setpos",		2, 2, FEARG_2,	    arg2_string_list_نمبر,
			"expr, list",
			ret_نمبر_bool,    f_setpos},
    {"setqflist",	1, 3, FEARG_1,	    arg13_setqflist,
			"list, action, what",
			ret_نمبر_bool,    f_setqflist},
    {"setreg",		2, 3, FEARG_2,	    arg3_string_any_string,
			"regname, value, options",
			ret_نمبر_bool,    f_setreg},
    {"settabvar",	3, 3, FEARG_3,	    arg3_نمبر_string_any,
			"tabnr, varname, val",
			ret_void,	    f_settabvar},
    {"settabwinvar",	4, 4, FEARG_4,	    arg4_نمبر_نمبر_string_any,
			"tabnr, winnr, varname, val",
			ret_void,	    f_settabwinvar},
    {"settagstack",	2, 3, FEARG_2,	    arg23_settagstack,
			"nr, dict, action",
			ret_نمبر_bool,    f_settagstack},
    {"setwinvar",	3, 3, FEARG_3,	    arg3_نمبر_string_any,
			"winnr, varname, val",
			ret_void,	    f_setwinvar},
    {"sha256",		1, 1, FEARG_1,	    arg1_string_or_blob,
			"expr",
			ret_string,
#اگر_تعریف_شدہ FEAT_CRYPT
	    f_sha256
#ورنہ
	    تہی
#اگر_ختم
			},
    {"shellescape",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, special",
			ret_string,	    f_shellescape},
    {"shiftwidth",	0, 1, FEARG_1,	    arg1_نمبر,
			"col",
			ret_نمبر,	    f_shiftwidth},
    {"sign_define",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"name, dict",
			ret_sign_define,	    SIGN_FUNC(f_sign_define)},
    {"sign_getdefined",	0, 1, FEARG_1,	    arg1_string,
			"name",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getdefined)},
    {"sign_getplaced",	0, 2, FEARG_1,	    arg02_sign_getplaced,
			"buf, dict",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getplaced)},
    {"sign_jump",	3, 3, FEARG_1,	    arg3_نمبر_string_بفر,
			"id, group, buf",
			ret_نمبر,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_رکھو,
			"id, group, name, buf, dict",
			ret_نمبر,	    SIGN_FUNC(f_sign_رکھو)},
    {"sign_placelist",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_نمبر,    SIGN_FUNC(f_sign_placelist)},
    {"sign_undefine",	0, 1, FEARG_1,	    arg1_string_or_list_string,
			"name",
			ret_نمبر_bool,    SIGN_FUNC(f_sign_undefine)},
    {"sign_unplace",	1, 2, FEARG_1,	    arg2_string_dict,
			"group, dict",
			ret_نمبر_bool,    SIGN_FUNC(f_sign_unplace)},
    {"sign_unplacelist", 1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_نمبر,    SIGN_FUNC(f_sign_unplacelist)},
    {"simplify",	1, 1, FEARG_1,	    arg1_string,
			"filename",
			ret_string,	    f_simplify},
    {"sin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sin},
    {"sinh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sinh},
    {"slice",		2, 3, FEARG_1,	    arg23_قاش,
			"expr, start, end",
			ret_قاش,	    f_قاش},
    {"sort",		1, 3, FEARG_1,	    arg13_sortuniq,
			"list, how, dict",
			ret_پہلا_arg,	    f_ترتیب_دو},
    {"sound_clear",	0, 0, 0,	    تہی,
			تہی,
			ret_void,	    SOUND_FUNC(f_sound_clear)},
    {"sound_playevent",	1, 2, FEARG_1,	    arg2_string_any,
			"name, callback",
			ret_نمبر,	    SOUND_FUNC(f_sound_playevent)},
    {"sound_playfile",	1, 2, FEARG_1,	    arg2_string_any,
			"path, callback",
			ret_نمبر,	    SOUND_FUNC(f_sound_playfile)},
    {"sound_stop",	1, 1, FEARG_1,	    arg1_نمبر,
			"id",
			ret_void,	    SOUND_FUNC(f_sound_stop)},
    {"soundfold",	1, 1, FEARG_1,	    arg1_string,
			"word",
			ret_string,	    f_soundfold},
    {"spellbadword",	0, 1, FEARG_1,	    arg1_string,
			"sentence",
			ret_list_string,    f_spellbadword},
    {"spellsuggest",	1, 3, FEARG_1,	    arg3_string_نمبر_bool,
			"word, max, capital",
			ret_list_string,    f_spellsuggest},
    {"split",		1, 3, FEARG_1,	    arg3_string_string_bool,
			"string, pattern, keepempty",
			ret_list_string,    f_بانٹو},
    {"sqrt",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sqrt},
    {"srand",		0, 1, FEARG_1,	    arg1_نمبر,
			"expr",
			ret_list_نمبر,    f_srand},
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
			ret_list_نمبر,    f_str2list},
    {"str2nr",		1, 3, FEARG_1,	    arg3_string_نمبر_bool,
			"string, base, quoted",
			ret_نمبر,	    f_str2nr},
    {"strcharlen",	1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_نمبر,	    f_strcharlen},
    {"strcharpart",	2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, skipcc",
			ret_string,	    f_strcharpart},
    {"strchars",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, skipcc",
			ret_نمبر,	    f_strchars},
    {"strdisplaywidth",	1, 2, FEARG_1,	    arg2_string_نمبر,
			"string, col",
			ret_نمبر,	    f_strdisplaywidth},
    {"strftime",	1, 2, FEARG_1,	    arg2_string_نمبر,
			"format, time",
			ret_string,
#اگر_تعریف_شدہ HAVE_STRFTIME
	    f_strftime
#ورنہ
	    تہی
#اگر_ختم
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_نمبر,
			"str, index",
			ret_نمبر,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_نمبر,
			"haystack, needle, start",
			ret_نمبر,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, تہی,
			"expr",
			ret_string,	    f_string},
    {"strlen",		1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_نمبر,	    f_strlen},
    {"strpart",		2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, chars",
			ret_string,	    f_strpart},
    {"strptime",	2, 2, FEARG_1,	    arg2_string,
			"format, timestring",
			ret_نمبر,
#اگر_تعریف_شدہ HAVE_STRPTIME
	    f_strptime
#ورنہ
	    تہی
#اگر_ختم
			},
    {"strridx",		2, 3, FEARG_1,	    arg3_string_string_نمبر,
			"haystack, needle, start",
			ret_نمبر,	    f_strridx},
    {"strtrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_strtrans},
    {"strutf16len",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, countcc",
			ret_نمبر,	    f_strutf16len},
    {"strwidth",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_نمبر,	    f_strwidth},
    {"submatch",	1, 2, FEARG_1,	    arg2_نمبر_bool,
			"nr, list",
			ret_string_or_any_2,	    f_submatch},
    {"substitute",	4, 4, FEARG_1,	    arg4_string_string_any_string,
			"string, pat, sub, flags",
			ret_string,	    f_substitute},
    {"swapfilelist",	0, 0, 0,	    تہی,
			تہی,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_string,	    f_swapname},
    {"synID",		3, 3, 0,	    arg3_lnum_نمبر_bool,
			"lnum, col, trans",
			ret_نمبر,	    f_synID},
    {"synIDattr",	2, 3, FEARG_1,	    arg3_نمبر_string_string,
			"synID, what, mode",
			ret_string,	    f_synIDattr},
    {"synIDtrans",	1, 1, FEARG_1,	    arg1_نمبر,
			"synID",
			ret_نمبر,	    f_synIDtrans},
    {"synconcealed",	2, 2, 0,	    arg2_lnum_نمبر,
			"lnum, col",
			ret_list_any,	    f_synconcealed},
    {"synstack",	2, 2, 0,	    arg2_lnum_نمبر,
			"lnum, col",
			ret_list_نمبر,    f_synstack},
    {"system",		1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_string,	    f_system},
    {"systemlist",	1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_list_string,    f_systemlist},
    {"tabpagebuflist",	0, 1, FEARG_1,	    arg1_نمبر,
			"arg",
			ret_list_نمبر,    f_tabpagebuflist},
    {"tabpagenr",	0, 1, 0,	    arg1_string,
			"arg",
			ret_نمبر,	    f_tabpagenr},
    {"tabpagewinnr",	1, 2, FEARG_1,	    arg2_نمبر_string,
			"tabarg, arg",
			ret_نمبر,	    f_tabpagewinnr},
    {"tabpanel_getinfo", 0, 0, 0,	    تہی,
			تہی,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_نمبر_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    تہی,
			تہی,
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
    {"tempname",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_نمبر,	    رکن_FUNC(f_رکن_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_نمبر,	    رکن_FUNC(f_رکن_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_بفر_string_dict,
			"buf, filename, options",
			ret_void,	    رکن_FUNC(f_رکن_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_بفر,
			"buf",
			ret_نمبر,	    رکن_FUNC(f_رکن_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_بفر,
			"buf",
			ret_list_string,
#اگر defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_رکن_getansicolors
#ورنہ
	    تہی
#اگر_ختم
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_نمبر_string,
			"attr, what",
			ret_نمبر,	    رکن_FUNC(f_رکن_getattr)},
    {"term_getcursor",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_list_any,	    رکن_FUNC(f_رکن_getcursor)},
    {"term_getjob",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_کام,	    رکن_FUNC(f_رکن_getjob)},
    {"term_getline",	2, 2, FEARG_1,	    arg2_بفر_lnum,
			"buf, row",
			ret_string,	    رکن_FUNC(f_رکن_getline)},
    {"term_getscrolled", 1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_نمبر,	    رکن_FUNC(f_رکن_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_list_نمبر,    رکن_FUNC(f_رکن_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_string,	    رکن_FUNC(f_رکن_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_بفر,
			"buf",
			ret_string,	    رکن_FUNC(f_رکن_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_بفر_bool,
			"buf, input",
			ret_string,	    رکن_FUNC(f_رکن_gettty)},
    {"term_list",	0, 0, 0,	    تہی,
			تہی,
			ret_list_نمبر,    رکن_FUNC(f_رکن_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_بفر_lnum,
			"buf, row",
			ret_list_dict_any,  رکن_FUNC(f_رکن_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_بفر_string,
			"buf, keys",
			ret_void,	    رکن_FUNC(f_رکن_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_بفر_list_any,
			"buf, colors",
			ret_void,
#اگر defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_رکن_setansicolors
#ورنہ
	    تہی
#اگر_ختم
			},
    {"term_setapi",	2, 2, FEARG_1,	    arg2_بفر_string,
			"buf, expr",
			ret_void,	    رکن_FUNC(f_رکن_setapi)},
    {"term_setkill",	2, 2, FEARG_1,	    arg2_بفر_string,
			"buf, how",
			ret_void,	    رکن_FUNC(f_رکن_setkill)},
    {"term_setrestore",	2, 2, FEARG_1,	    arg2_بفر_string,
			"buf, command",
			ret_void,	    رکن_FUNC(f_رکن_setrestore)},
    {"term_setsize",	3, 3, FEARG_1,	    arg3_بفر_نمبر_نمبر,
			"buf, rows, cols",
			ret_void,	    رکن_FUNC(f_رکن_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_نمبر,	    رکن_FUNC(f_رکن_آغاز)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_بفر_نمبر,
			"buf, time",
			ret_void,	    رکن_FUNC(f_رکن_wait)},
    {"terminalprops",	0, 0, 0,	    تہی,
			تہی,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_نمبر,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    تہی,
			تہی,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    تہی,
			تہی,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   تہی,
			تہی,
			ret_void,	    f_test_garbagecollect_soon},
    {"test_getvalue",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_نمبر,	    f_test_getvalue},
    {"test_gui_event",	2, 2, FEARG_1,	    arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_gui_event},
    {"test_ignore_error", 1, 1, FEARG_1,    arg1_string,
			"expr",
			ret_void,	    f_test_ignore_error},
    {"test_mswin_event", 2, 2, FEARG_1,     arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_mswin_event},
    {"test_null_blob",	0, 0, 0,	    تہی,
			تہی,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    تہی,
			تہی,
			ret_channel,	    کام_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    تہی,
			تہی,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    تہی,
			تہی,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    تہی,
			تہی,
			ret_کام,	    کام_FUNC(f_test_null_کام)},
    {"test_null_list",	0, 0, 0,	    تہی,
			تہی,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    تہی,
			تہی,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    تہی,
			تہی,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_سیٹ},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_نمبر,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, تہی,
			"expr",
			ret_نمبر,	    f_test_refcount},
    {"test_setmouse",	2, 2, 0,	    arg2_نمبر,
			"row, col",
			ret_void,	    f_test_setmouse},
    {"test_settime",	1, 1, FEARG_1,	    arg1_نمبر,
			"expr",
			ret_void,	    f_test_settime},
    {"test_srand_seed",	0, 1, FEARG_1,	    arg1_نمبر,
			"seed",
			ret_void,	    f_test_srand_بیج},
    {"test_unknown",	0, 0, 0,	    تہی,
			تہی,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    تہی,
			تہی,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_نمبر,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_نمبر_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_نمبر_any_dict,
			"time, callback, options",
			ret_نمبر,	    TIMER_FUNC(f_timer_آغاز)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_نمبر,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    تہی,
			تہی,
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
    {"trim",		1, 3, FEARG_1,	    arg3_string_string_نمبر,
			"text, mask, dir",
			ret_string,	    f_trim},
    {"trunc",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_trunc},
    {"tuple2list",	1, 1, FEARG_1,	    arg1_tuple_any,
			"tuple",
			ret_list_any,	    f_tuple2list},
    {"type",		1, 1, FEARG_1|FE_X, تہی,
			"expr",
			ret_نمبر,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, تہی,
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
			ret_پہلا_arg,	    f_uniq},
    {"uri_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_uridecode},
    {"uri_encode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_uriencode},
    {"utf16idx",	2, 4, FEARG_1,	    arg4_string_نمبر_bool_bool,
			"string, idx, countcc, charidx",
			ret_نمبر,	    f_utf16idx},
    {"values",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_member,    f_اقدار},
    {"virtcol",		1, 3, FEARG_1,	    arg3_string_or_list_bool_نمبر,
			"expr, list, winid",
			ret_virtcol,	    f_virtcol},
    {"virtcol2col",	3, 3, FEARG_1,	    arg3_نمبر,
			"winid, lnum, col",
			ret_نمبر,	    f_virtcol2col},
    {"visualmode",	0, 1, 0,	    arg1_bool,
			"expr",
			ret_string,	    f_visualmode},
    {"wildmenumode",	0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    تہی,
			تہی,
			ret_void,	    f_wildtrigger},
    {"win_execute",	2, 3, FEARG_2,	    arg23_win_execute,
			"id, command, silent",
			ret_string,	    f_win_execute},
    {"win_findbuf",	1, 1, FEARG_1,	    arg1_نمبر,
			"bufnr",
			ret_list_نمبر,    f_win_findbuf},
    {"win_getid",	0, 2, FEARG_1,	    arg2_نمبر,
			"win, tab",
			ret_نمبر,	    f_win_getid},
    {"win_gettype",	0, 1, FEARG_1,	    arg1_نمبر,
			"nr",
			ret_string,	    f_win_gettype},
    {"win_gotoid",	1, 1, FEARG_1,	    arg1_نمبر,
			"expr",
			ret_نمبر_bool,    f_win_gotoid},
    {"win_id2tabwin",	1, 1, FEARG_1,	    arg1_نمبر,
			"expr",
			ret_list_نمبر,    f_win_id2tabwin},
    {"win_id2win",	1, 1, FEARG_1,	    arg1_نمبر,
			"expr",
			ret_نمبر,	    f_win_id2win},
    {"win_move_separator", 2, 2, FEARG_1,   arg2_نمبر,
			"nr, offset",
			ret_نمبر_bool,    f_win_move_separator},
    {"win_move_statusline", 2, 2, FEARG_1,  arg2_نمبر,
			"nr, offset",
			ret_نمبر_bool,    f_win_move_statusline},
    {"win_screenpos",	1, 1, FEARG_1,	    arg1_نمبر,
			"nr",
			ret_list_نمبر,    f_win_screenpos},
    {"win_splitmove",   2, 3, FEARG_1,	    arg3_نمبر_نمبر_dict,
			"nr, target, options",
			ret_نمبر_bool,    f_win_splitmove},
    {"winbufnr",	1, 1, FEARG_1,	    arg1_نمبر,
			"nr",
			ret_نمبر,	    f_winbufnr},
    {"wincol",		0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_نمبر,
			"nr",
			ret_نمبر,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_نمبر,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    تہی,
			تہی,
			ret_نمبر,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_نمبر,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    تہی,
			تہی,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    تہی,
			تہی,
			ret_dict_نمبر,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_نمبر,
			"nr",
			ret_نمبر,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    تہی,
			تہی,
			ret_dict_نمبر,    f_wordcount},
    {"writefile",	2, 3, FEARG_1,	    arg23_writefile,
			"object, fname, flags",
			ret_نمبر_bool,    f_writefile},
    {"xor",		2, 2, FEARG_1,	    arg2_نمبر,
			"expr1, expr2",
			ret_نمبر,	    f_xor},
};

/*
 * Return true if specified function allows a type as an argument.
 */
    ساکن صحیح
func_allows_type(صحیح idx)
{
    واپس (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, صحیح idx)
{
    ساکن صحیح	intidx = -1;
    char_u	*name;

    اگر (idx == 0)
	intidx = -1;
    اگر (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	اگر (name != تہی)
	{
	    اگر (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_نمونہ, 2) == 0)
		واپس cat_سابقہ_varname('g', name);
	    واپس name;
	}
    }
    اگر (++intidx < (صحیح)ARRAY_لمبائی(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	اگر (global_functions[intidx].f_func == تہی)
	    واپس (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	اگر (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	واپس IObuff;
    }

    واپس تہی;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_اظہاریہ_name(expand_T *xp, صحیح idx)
{
    ساکن صحیح	intidx = -1;
    char_u	*name;

    اگر (idx == 0)
	intidx = -1;
    اگر (intidx < 0)
    {
	name = get_function_name(xp, idx);
	اگر (name != تہی)
	    واپس name;
    }
    واپس get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    ساکن صحیح
ڈھونڈو_اندرونی_func_opt(char_u *name, صحیح implemented)
{
    صحیح		پہلا = 0;
    صحیح		آخری;
    صحیح		cmp;
    صحیح		x;

    آخری = (صحیح)ARRAY_لمبائی(global_functions) - 1;

    // Find the function name in the table. Binary search.
    جب (پہلا <= آخری)
    {
	x = پہلا + ((غیرعلامتی)(آخری - پہلا) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	اگر (cmp < 0)
	    آخری = x - 1;
	ورنہ اگر (cmp > 0)
	    پہلا = x + 1;
	ورنہ اگر (implemented && global_functions[x].f_func == تہی)
	    توڑو;
	ورنہ
	    واپس x;
    }
    واپس -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    صحیح
ڈھونڈو_اندرونی_func(char_u *name)
{
    واپس ڈھونڈو_اندرونی_func_opt(name, TRUE);
}

    صحیح
has_اندرونی_func(char_u *name)
{
    واپس ڈھونڈو_اندرونی_func_opt(name, TRUE) >= 0;
}

    ساکن صحیح
has_اندرونی_func_name(char_u *name)
{
    واپس ڈھونڈو_اندرونی_func_opt(name, FALSE) >= 0;
}

    حرف *
اندرونی_func_name(صحیح idx)
{
    واپس global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    صحیح
اندرونی_func_check_arg_types(
	type2_T	*types,
	صحیح	idx,
	صحیح	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    اگر (!(func_allows_type(idx)))
    {
	برائے (صحیح i = 0; i < argcount; ++i)
	    اگر (check_type_is_قدر(types[i].type_curr) == FAIL)
		واپس FAIL;
    }

    ثابت argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    اگر (argchecks == تہی)
	واپس OK;

    argcontext_T context;

    context.arg_گنتی = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    برائے (صحیح i = 0; i < argcount && argchecks[i] != تہی; ++i)
    {
	context.arg_idx = i;
	اگر (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    واپس FAIL;
    }
    واپس OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    خالی
اندرونی_func_get_argcount(صحیح idx, صحیح *argcount, صحیح *min_argcount)
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
اندرونی_func_ret_type(
	صحیح	    idx,
	صحیح	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = تہی;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    اگر (*decl_type == تہی)
	*decl_type = ret;
    current_type_gap = تہی;
    واپس ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    صحیح
اندرونی_func_is_map(صحیح idx)
{
    واپس global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    صحیح
check_اندرونی_func(صحیح idx, صحیح argcount)
{
    funcerror_T	    res;
    حرف	    *name;

    اگر (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    ورنہ اگر (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    ورنہ
	واپس global_functions[idx].f_argtype & FEARG_نقاب;

    name = اندرونی_func_name(idx);
    اگر (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    ورنہ
	semsg(_(e_not_enough_arguments_for_function_str), name);
    واپس -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    ساکن صحیح
check_دلائل_for_type(صحیح idx, صحیح argcount, typval_T *argvars)
{
    اگر (!func_allows_type(idx))
    {
	برائے (صحیح i = 0; i < argcount; ++i)
	    اگر (check_typval_is_قدر(&argvars[i]) == FAIL)
		واپس FAIL;
    }
    واپس OK;
}

    funcerror_T
call_اندرونی_func(
	char_u	    *name,
	صحیح	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    صحیح i;

    i = ڈھونڈو_اندرونی_func(name);
    اگر (i < 0)
	واپس FCERR_UNKNOWN;
    اگر (argcount < global_functions[i].f_min_argc)
	واپس FCERR_TOOFEW;
    اگر (argcount > global_functions[i].f_max_argc)
	واپس FCERR_TOOMANY;
    اگر (check_دلائل_for_type(i, argcount, argvars) == FAIL)
	واپس FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    اگر (اندر_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    واپس FCERR_NONE;
}

    خالی
call_اندرونی_func_by_idx(
	صحیح	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    اگر (اندر_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_اندرونی_طریقہ(
	char_u	    *name,
	صحیح	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    صحیح		fi;
    typval_T	argv[MAX_FUNC_دلائل + 1];

    fi = ڈھونڈو_اندرونی_func(name);
    اگر (fi < 0)
	واپس FCERR_UNKNOWN;
    اگر ((global_functions[fi].f_argtype & FEARG_نقاب) == 0)
	واپس FCERR_NOTMETHOD;
    اگر (argcount + 1 < global_functions[fi].f_min_argc)
	واپس FCERR_TOOFEW;
    اگر (argcount + 1 > global_functions[fi].f_max_argc)
	واپس FCERR_TOOMANY;
    اگر (check_دلائل_for_type(fi, argcount, argvars) == FAIL)
	واپس FCERR_OTHER;

    اگر ((global_functions[fi].f_argtype & FEARG_نقاب) == FEARG_2)
    {
	اگر (argcount < 1)
	    واپس FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	برائے (صحیح i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ورنہ اگر ((global_functions[fi].f_argtype & FEARG_نقاب) == FEARG_3)
    {
	اگر (argcount < 2)
	    واپس FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	برائے (صحیح i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ورنہ اگر ((global_functions[fi].f_argtype & FEARG_نقاب) == FEARG_4)
    {
	اگر (argcount < 3)
	    واپس FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	برائے (صحیح i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ورنہ
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	برائے (صحیح i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    اگر (check_دلائل_for_type(fi, argcount + 1, argv) == FAIL)
	واپس FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    واپس FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    صحیح
non_zero_arg(typval_T *argvars)
{
    واپس ((argvars[0].v_type == VAR_نمبر
		&& argvars[0].vval.v_نمبر != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_نمبر == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != تہی
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    ساکن خالی
f_and(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    rettv->vval.v_نمبر = tv_get_نمبر_chk(&argvars[0], تہی)
					& tv_get_نمبر_chk(&argvars[1], تہی);
}

/*
 * "balloon_show()" function
 */
#اگر_تعریف_شدہ FEAT_BEVAL
    ساکن خالی
f_balloon_gettext(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    اگر (balloonEval == تہی)
	واپس;

    اگر (balloonEval->پیغام == تہی)
	rettv->vval.v_string = تہی;
    ورنہ
	rettv->vval.v_string = vim_strsave(balloonEval->پیغام);
}

    ساکن خالی
f_balloon_show(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (balloonEval == تہی)
	واپس;

    اگر (اندر_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.اندر_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, تہی,
		l == تہی || l->lv_len == 0 ? تہی : l);
    }
    ورنہ
    {
	char_u *mesg;

	اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    واپس;

	mesg = tv_get_string_chk(&argvars[0]);
	اگر (mesg != تہی)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? تہی : mesg, تہی);
    }
}

# اگر defined(FEAT_BEVAL_رکن)
    ساکن خالی
f_balloon_بانٹو(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (rettv_list_alloc(rettv) != OK)
	واپس;

    char_u *پیغام;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;
    پیغام = tv_get_string_chk(&argvars[0]);
    اگر (پیغام != تہی)
    {
	pumitem_T	*array;
	صحیح		سائز = بانٹو_پیغام(پیغام, &array);

	// Skip the first and last item, they are always empty.
	برائے (صحیح i = 1; i < سائز - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_متن, -1);
	جب (سائز > 0)
	    vim_free(array[--سائز].pum_متن);
	vim_free(array);
    }
}
# endif
#اگر_ختم

/*
 * "base64_decode(string)" function
 */
    ساکن خالی
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    اگر (check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    اگر (rettv_blob_alloc(rettv) == FAIL)
	واپس;

    char_u *str = tv_get_string_chk(&argvars[0]);
    اگر (str != تہی)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    ساکن خالی
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    اگر (check_for_blob_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    blob_T *blob = argvars->vval.v_blob;
    اگر (blob != تہی)
	rettv->vval.v_string =
	    base64_encode(blob->bv_ga.ga_ڈیٹا, blob->bv_ga.ga_len);
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
    اگر (buf == تہی)
	semsg(_(e_invalid_بفر_name_str), tv_get_string(arg));
    واپس buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    ساکن خالی
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_نمبر = VVAL_TRUE;

    اگر (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	واپس;

    اگر (strcmp((ثابت حرف *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[0]));
    ورنہ
    {
	اگر (bindtextdomain((ثابت حرف *)argvars[0].vval.v_string, (ثابت حرف *)argvars[1].vval.v_string) == تہی)
	{
	    do_outofmem_پیغام((طویل)0);
	    rettv->vval.v_نمبر = VVAL_FALSE;
	}
    }

    واپس;
}

/*
 * "byte2line(byte)" function
 */
    ساکن خالی
f_byte2line(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
#اگر_غیر_تعریف_شدہ FEAT_BYTEOFF
    rettv->vval.v_نمبر = -1;
#ورنہ
    طویل	boff = 0;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    boff = tv_get_نمبر(&argvars[0]) - 1;  // boff gets -1 on type error
    اگر (boff < 0)
	rettv->vval.v_نمبر = -1;
    ورنہ
	rettv->vval.v_نمبر = ml_ڈھونڈو_سطر_or_انحراف(curbuf,
							  (linenr_T)0, &boff);
#اگر_ختم
}

/*
 * "call(func, arglist [, dict])" function
 */
    ساکن خالی
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = تہی;
    dict_T	*selfdict = تہی;
    char_u	*tofree = تہی;

    اگر (اندر_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	واپس;

    اگر (check_for_list_arg(argvars, 1) == FAIL)
	واپس;
    اگر (argvars[1].vval.v_list == تہی)
	واپس;

    اگر (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    ورنہ اگر (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    ورنہ
	func = tv_get_string(&argvars[0]);
    اگر (func == تہی || *func == NUL)
	واپس;		// type error, empty name or null function

    اگر (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, تہی, FALSE, TFN_INT|TFN_QUIET);
	اگر (tofree == تہی)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    واپس;
	}
	اگر (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_دلائل(tofree, STRLEN(tofree),
									&p);
	    اگر (s != تہی)
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
	    جاؤ مکمل;

	selfdict = argvars[2].vval.v_dict;
    }

    (خالی)func_call(func, &argvars[1], partial, selfdict, rettv);

مکمل:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    ساکن خالی
f_changenr(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->vval.v_نمبر = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    ساکن خالی
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	واپس;

    اگر (has_mbyte)
    {
	صحیح	utf8 = 0;

	اگر (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (صحیح)tv_get_bool_chk(&argvars[1], تہی);

	اگر (utf8)
	    rettv->vval.v_نمبر = utf_ptr2char(tv_get_string(&argvars[0]));
	ورنہ
	    rettv->vval.v_نمبر = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    ورنہ
	rettv->vval.v_نمبر = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    ساکن خالی
get_ستون(typval_T *argvars, typval_T *rettv, صحیح charcol)
{
    colnr_T	ستون = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    صحیح		winchanged = FALSE;

    اگر (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_نمبر_arg(argvars, 1) == FAIL)
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((صحیح)tv_get_نمبر(&argvars[1]), &tp);
	اگر (wp == تہی || tp == تہی)
	    واپس;

	اگر (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    واپس;

	check_cursor();
	winchanged = TRUE;
    }

    صحیح fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    اگر (fp != تہی && fnum == curbuf->b_fnum)
    {
	اگر (fp->ستون == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    اگر (fp->lnum <= curbuf->b_ml.ml_سطر_گنتی)
		ستون = ml_get_len(fp->lnum) + 1;
	    ورنہ
		ستون = MAXCOL;
	}
	ورنہ
	{
	    ستون = fp->ستون + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    اگر (virtual_فعال() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		اگر (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    صحیح		l;

		    اگر (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			ستون += l;
		}
	    }
	}
    }
    rettv->vval.v_نمبر = ستون;

    اگر (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    ساکن خالی
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_ستون(argvars, rettv, TRUE);
}

    win_T *
get_optional_کھڑکی(typval_T *argvars, صحیح idx)
{
    win_T   *win = curwin;

    اگر (argvars[idx].v_type == VAR_UNKNOWN)
	واپس curwin;

    win = ڈھونڈو_win_by_nr_or_id(&argvars[idx]);
    اگر (win == تہی)
    {
	emsg(_(e_invalid_کھڑکی_نمبر));
	واپس تہی;
    }
    واپس win;
}

/*
 * "col(string)" function
 */
    ساکن خالی
f_ستون(typval_T *argvars, typval_T *rettv)
{
    get_ستون(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    ساکن خالی
f_confirm(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*پیغام_2;
    char_u	*buttons = تہی;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    صحیح		def = 1;
    صحیح		type = VIM_GENERIC;
    char_u	*typestr;
    صحیح		error = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_نمبر_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	واپس;

    پیغام_2 = tv_get_string_chk(&argvars[0]);
    اگر (پیغام_2 == تہی)
	error = TRUE;
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	اگر (buttons == تہی)
	    error = TRUE;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (صحیح)tv_get_نمبر_chk(&argvars[2], &error);
	    اگر (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		اگر (typestr == تہی)
		    error = TRUE;
		ورنہ
		{
		    انتخاب (TOUPPER_ASC(*typestr))
		    {
			حالت 'E': type = VIM_ERROR; توڑو;
			حالت 'Q': type = VIM_QUESTION; توڑو;
			حالت 'I': type = VIM_INFO; توڑو;
			حالت 'W': type = VIM_WARNING; توڑو;
			حالت 'G': type = VIM_GENERIC; توڑو;
		    }
		}
	    }
	}
    }

    اگر (buttons == تہی || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    اگر (!error)
	rettv->vval.v_نمبر = do_dialog(type, تہی, پیغام_2, buttons,
							    def, تہی, FALSE);
#اگر_ختم
}

/*
 * "copy()" function
 */
    ساکن خالی
f_copy(typval_T *argvars, typval_T *rettv)
{
    شے_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ساکن خالی
سیٹ_cursorpos(typval_T *argvars, typval_T *rettv, صحیح charcol)
{
    طویل	lnum, ستون;
    طویل	coladd = 0;
    صحیح		سیٹ_curswant = TRUE;

    اگر (اندر_vim9script()
	    && (check_for_string_or_نمبر_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 2) == FAIL)))
	واپس;

    rettv->vval.v_نمبر = -1;
    اگر (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	اگر (list2fpos(argvars, &pos, تہی, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_دلیل));
	    واپس;
	}
	lnum = pos.lnum;
	ستون = pos.ستون;
	coladd = pos.coladd;
	اگر (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    سیٹ_curswant = FALSE;
	}
    }
    ورنہ اگر ((argvars[0].v_type == VAR_نمبر
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_نمبر
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	اگر (lnum < 0)
	    semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[0]));
	ورنہ اگر (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	ستون = (طویل)tv_get_نمبر_chk(&argvars[1], تہی);
	اگر (charcol)
	    ستون = buf_charidx_مقصد_byteidx(curbuf, lnum, ستون) + 1;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (طویل)tv_get_نمبر_chk(&argvars[2], تہی);
    }
    ورنہ
    {
	emsg(_(e_invalid_دلیل));
	واپس;
    }
    اگر (lnum < 0 || ستون < 0 || coladd < 0)
	واپس;		// type error; errmsg already given
    اگر (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    اگر (ستون != MAXCOL && --ستون < 0)
	ستون = 0;
    curwin->w_cursor.ستون = ستون;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    اگر (has_mbyte)
	mb_adjust_cursor();

    curwin->w_سیٹ_curswant = سیٹ_curswant;
    rettv->vval.v_نمبر = 0;
}

/*
 * "cursor(lnum, col)" function, or
 * "cursor(list)"
 *
 * Moves the cursor to the specified line and column.
 * Returns 0 when the position could be set, -1 otherwise.
 */
    ساکن خالی
f_cursor(typval_T *argvars, typval_T *rettv)
{
    سیٹ_cursorpos(argvars, rettv, FALSE);
}

#اگر_تعریف_شدہ MSWIN
/*
 * "debugbreak()" function
 */
    ساکن خالی
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    صحیح		pid;

    rettv->vval.v_نمبر = FAIL;
    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    pid = (صحیح)tv_get_نمبر(&argvars[0]);
    اگر (pid == 0)
    {
	emsg(_(e_invalid_دلیل));
	واپس;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    اگر (hProcess == تہی)
	واپس;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_نمبر = OK;
}
#اگر_ختم

/*
 * "deepcopy()" function
 */
    ساکن خالی
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    اگر (check_for_opt_bool_arg(argvars, 1) == FAIL)
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], تہی);

    شے_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    ساکن خالی
f_did_filetype(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    rettv->vval.v_نمبر = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    ساکن خالی
f_echoraw(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    char_u *str;

    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    str = tv_get_string_chk(&argvars[0]);
    اگر (str != تہی && *str != NUL)
    {
	برآمد_str(str);
	برآمد_flush();
    }
}

/*
 * "empty({expr})" function
 */
    ساکن خالی
f_خالی_خانے(typval_T *argvars, typval_T *rettv)
{
    صحیح		n = FALSE;

    انتخاب (argvars[0].v_type)
    {
	حالت VAR_STRING:
	حالت VAR_FUNC:
	    n = argvars[0].vval.v_string == تہی
					  || *argvars[0].vval.v_string == NUL;
	    توڑو;
	حالت VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == تہی;
	    توڑو;
	حالت VAR_نمبر:
	    n = argvars[0].vval.v_نمبر == 0;
	    توڑو;
	حالت VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    توڑو;
	حالت VAR_LIST:
	    n = argvars[0].vval.v_list == تہی
					|| argvars[0].vval.v_list->lv_len == 0;
	    توڑو;
	حالت VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    توڑو;
	حالت VAR_DICT:
	    n = argvars[0].vval.v_dict == تہی
			|| argvars[0].vval.v_dict->dv_hashtab.ht_مستعمل == 0;
	    توڑو;
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    n = argvars[0].vval.v_نمبر != VVAL_TRUE;
	    توڑو;
	حالت VAR_CLASS:
	    n = argvars[0].vval.v_class != تہی;
	    توڑو;
	حالت VAR_OBJECT:
	    n = object_خالی_خانے(argvars[0].vval.v_object);
	    توڑو;

	حالت VAR_BLOB:
	    n = argvars[0].vval.v_blob == تہی
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    توڑو;

	حالت VAR_کام:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    n = argvars[0].vval.v_کام == تہی
			   || argvars[0].vval.v_کام->jv_حالت != کام_STARTED;
	    توڑو;
#اگر_ختم
	حالت VAR_CHANNEL:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    n = argvars[0].vval.v_channel == تہی
			       || !channel_is_open(argvars[0].vval.v_channel);
	    توڑو;
#اگر_ختم
	حالت VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == تہی
		|| argvars[0].vval.v_typealias->ta_name == تہی
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    توڑو;

	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_INSTR:
	    اندرونی_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    توڑو;
    }

    rettv->vval.v_نمبر = n;
}

/*
 * "environ()" function
 */
    ساکن خالی
f_environ(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
#اگر !defined(AMIGA)
    صحیح			i = 0;
    char_u		*اندراج_2, *قدر;
# اگر defined (MSWIN)
#  اگر !defined(_UCRT)
    خارجی wchar_t	**_wenviron;
#  endif
# ورنہ
    خارجی حرف		**environ;
# endif

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;

# ifdef MSWIN
    اگر (*_wenviron == تہی)
	واپس;
# ورنہ
    اگر (*environ == تہی)
	واپس;
# endif

    برائے (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	اگر ((p = (short_u *)_wenviron[i]) == تہی)
	    واپس;
	اندراج_2 = utf16_مقصد_enc(p, تہی);
# ورنہ
	اگر ((اندراج_2 = (char_u *)environ[i]) == تہی)
	    واپس;
	اندراج_2 = vim_strsave(اندراج_2);
# endif
	اگر (اندراج_2 == تہی) // out of memory
	    واپس;
	اگر ((قدر = vim_strchr(اندراج_2, '=')) == تہی)
	{
	    vim_free(اندراج_2);
	    جاری;
	}
	*قدر++ = NUL;
	dict_جوڑو_string(rettv->vval.v_dict, (حرف *)اندراج_2, قدر);
	vim_free(اندراج_2);
    }
#اگر_ختم
}

/*
 * "err_teapot()" function
 */
    ساکن خالی
f_err_teapot(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	اگر (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    اگر (*skipwhite(s) == NUL)
		واپس;
	}

	صحیح err = FALSE;
	صحیح do_503 = eval_اظہاریہ_مقصد_bool(&argvars[0], &err);
	اگر (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_دستیاب));
	    واپس;
	}
    }

    emsg(_(e_im_a_teapot));
}

/*
 * "escape({string}, {chars})" function
 */
    ساکن خالی
f_escape(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    rettv->vval.v_string = vim_strsave_escaped(tv_get_string(&argvars[0]),
					 tv_get_string_buf(&argvars[1], buf));
    rettv->v_type = VAR_STRING;
}

/*
 * "eval()" function
 */
    ساکن خالی
f_eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*s, *p;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    s = tv_get_string_chk(&argvars[0]);
    اگر (s != تہی)
	s = skipwhite(s);

    p = s;
    اگر (s == تہی || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	اگر (p != تہی && !aborting())
	    semsg(_(e_invalid_عبارت_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_نمبر;
	rettv->vval.v_نمبر = 0;
    }
    ورنہ اگر (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    ساکن خالی
f_eventhandler(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->vval.v_نمبر = vgetc_busy || داخلہ_busy;
}

ساکن garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    خالی
execute_redir_str(char_u *قدر, صحیح قدر_len)
{
    صحیح		len;

    اگر (قدر_len == -1)
	len = (صحیح)STRLEN(قدر);	// Append the entire string
    ورنہ
	len = قدر_len;		// Append only "value_len" characters
    اگر (ga_grow(&redir_execute_ga, len) == FAIL)
	واپس;

    mch_memmove((حرف *)redir_execute_ga.ga_ڈیٹا
	    + redir_execute_ga.ga_len, قدر, len);
    redir_execute_ga.ga_len += len;
}

#اگر defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    ساکن char_u *
get_str_سطر(
    صحیح	    c غیر_مستعمل,
    خالی    *cookie,
    صحیح	    indent غیر_مستعمل,
    getline_opt_T options غیر_مستعمل)
{
    char_u	*آغاز = *(char_u **)cookie;
    char_u	*سطر;
    char_u	*p;

    p = آغاز;
    اگر (p == تہی || *p == NUL)
	واپس تہی;
    p = vim_strchr(p, '\n');
    اگر (p == تہی)
	سطر = vim_strsave(آغاز);
    ورنہ
    {
	سطر = vim_strnsave(آغاز, p - آغاز);
	p++;
    }

    *(char_u **)cookie = p;
    واپس سطر;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    خالی
execute_cmds_from_string(char_u *str)
{
    do_cmdline(تہی, get_str_سطر, (خالی *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#اگر_ختم

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_سطر(
    صحیح	    c غیر_مستعمل,
    خالی    *cookie,
    صحیح	    indent غیر_مستعمل,
    getline_opt_T options غیر_مستعمل)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *شے = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    اگر (شے == تہی)
	واپس تہی;
    s = tv_get_string_buf_chk(&شے->li_tv, buf);
    *p = شے->li_اگلا;
    واپس s == تہی ? تہی : vim_strsave(s);
}

/*
 * "execute()" function
 */
    خالی
execute_common(typval_T *argvars, typval_T *rettv, صحیح arg_off)
{
    char_u	*cmd = تہی;
    list_T	*list = تہی;
    صحیح		save_پیغام_silent = پیغام_silent;
    صحیح		save_emsg_silent = emsg_silent;
    صحیح		save_emsg_noredir = emsg_noredir;
    صحیح		save_redir_execute = redir_execute;
    صحیح		save_redir_off = redir_off;
    garray_T	save_ga;
    صحیح		save_پیغام_ستون = پیغام_ستون;
    صحیح		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    صحیح		echo_output = FALSE;

    rettv->vval.v_string = تہی;
    rettv->v_type = VAR_STRING;

    اگر (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	اگر (list == تہی || list->lv_len == 0)
	    // empty list, no commands, empty output
	    واپس;
	++list->lv_refcount;
    }
    ورنہ اگر (argvars[arg_off].v_type == VAR_کام
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_قدر_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	واپس;
    }
    ورنہ
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	اگر (cmd == تہی)
	    واپس;
    }

    اگر (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      اندر_vim9script());

	اگر (s == تہی)
	    واپس;
	اگر (*s == NUL)
	    echo_output = TRUE;
	اگر (STRNCMP(s, "silent", 6) == 0)
	    ++پیغام_silent;
	اگر (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    ورنہ
	++پیغام_silent;

    اگر (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, حجم(حرف), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    اگر (!echo_output)
	پیغام_ستون = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    اگر (cmd != تہی)
	do_cmdline_cmd(cmd);
    ورنہ
    {
	listitem_T	*شے;

	CHECK_LIST_MATERIALIZE(list);
	شے = list->lv_پہلا;
	do_cmdline(تہی, get_list_سطر, (خالی *)&شے,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    اگر (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((حرف *)redir_execute_ga.ga_ڈیٹا)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_ڈیٹا;
    }
    ورنہ
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = تہی;
    }
    پیغام_silent = save_پیغام_silent;
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
	پیغام_ستون = 0;
    ورنہ
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	پیغام_ستون = save_پیغام_ستون;
}

/*
 * "execute()" function
 */
    ساکن خالی
f_execute(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	واپس;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    خالی
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحیح		n = FALSE;

    اگر (اندر_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	واپس;

    p = tv_get_string(&argvars[0]);
    اگر (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	اگر (mch_getenv(p + 1) != تہی)
	    n = TRUE;
	ورنہ
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    اگر (p != تہی && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    ورنہ اگر (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, تہی, TRUE) == OK);
	اگر (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    ورنہ اگر (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    ورنہ اگر (*p == '?')			// internal function only
    {
	n = has_اندرونی_func_name(p + 1);
    }
    ورنہ اگر (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    ورنہ اگر (*p == '#')
    {
	اگر (p[1] == '#')
	    n = autocmd_supported(p + 2);
	ورنہ
	    n = au_exists(p + 1);
    }
    ورنہ				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_نمبر = n;
}

    ساکن خالی
f_exists_compiled(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    emsg(_(e_exists_compiled_can_only_be_مستعمل_اندر_def_function));
}

/*
 * "expand()" function
 */
    ساکن خالی
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    size_t	len;
    صحیح		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    صحیح		error = FALSE;
    char_u	*نتیجہ;
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    char_u	*p_csl_save = p_csl;
#اگر_ختم

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	واپس;

#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    // avoid using 'completeslash' here
    p_csl = خالی_خانے_option;
#اگر_ختم

    rettv->v_type = VAR_STRING;
    اگر (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_سیٹ(rettv, تہی);

    s = tv_get_string(&argvars[0]);
    اگر (*s == '%' || *s == '#' || *s == '<')
    {
	حرف	*errormsg = تہی;

	اگر (p_verbose == 0)
	    ++emsg_off;
	نتیجہ = eval_vars(s, s, &len, تہی, &errormsg, تہی, FALSE);
	اگر (p_verbose == 0)
	    --emsg_off;
	ورنہ اگر (errormsg != تہی)
	    emsg(errormsg);
	اگر (rettv->v_type == VAR_LIST)
	{
	    اگر (rettv_list_alloc(rettv) == OK && نتیجہ != تہی)
		list_append_string(rettv->vval.v_list, نتیجہ, -1);
	    vim_free(نتیجہ);
	}
	ورنہ
	    rettv->vval.v_string = نتیجہ;
    }
    ورنہ
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
		rettv->vval.v_string = ExpandOne(&xpc, s, تہی,
							   options, WILD_ALL);
	    ورنہ اگر (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, تہی, options, WILD_ALL_KEEP);
		برائے (صحیح i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	ورنہ
	    rettv->vval.v_string = تہی;
    }
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    p_csl = p_csl_save;
#اگر_ختم
}

/*
 * "expandcmd()" function
 * Expand all the special characters in a command string.
 */
    ساکن خالی
f_expandcmd(typval_T *argvars, typval_T *rettv)
{
    exarg_T	eap;
    char_u	*cmdstr;
    حرف	*errormsg = تہی;
    صحیح		emsgoff = TRUE;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

    اگر (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_میدان(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = تہی;
    eap.cmdidx = CMD_USER;

    اگر (emsgoff)
	++emsg_off;
    اگر (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	اگر (!emsgoff && errormsg != تہی && *errormsg != NUL)
	    emsg(errormsg);
    اگر (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    ساکن خالی
f_feedkeys(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    صحیح		remap = TRUE;
    صحیح		داخل_کرو = FALSE;
    char_u	*کلیدیں, *flags;
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
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	واپس;

    کلیدیں = tv_get_string(&argvars[0]);

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	برائے ( ; *flags != NUL; ++flags)
	{
	    انتخاب (*flags)
	    {
		حالت 'n': remap = FALSE; توڑو;
		حالت 'm': remap = TRUE; توڑو;
		حالت 't': typed = TRUE; توڑو;
		حالت 'i': داخل_کرو = TRUE; توڑو;
		حالت 'x': execute = TRUE; توڑو;
		حالت 'c': context = TRUE; توڑو;
		حالت '!': dangerous = TRUE; توڑو;
		حالت 'L': lowlevel = TRUE; توڑو;
	    }
	}
    }

    اگر (*کلیدیں != NUL || execute)
    {
	اگر (lowlevel
#اگر_تعریف_شدہ FEAT_VTP
		&& (!is_رکن_win32()
		    || (کلیدیں[0] == 3 && ctrl_c_interrupts && typed))
#اگر_ختم
	   )
	{
#اگر_تعریف_شدہ USE_داخلہ_BUF
	    ch_log(تہی, "feedkeys() lowlevel: %s", کلیدیں);

	    صحیح len = (صحیح)STRLEN(کلیدیں);
	    برائے (صحیح idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		اگر (کلیدیں[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		جوڑو_مقصد_داخلہ_buf(کلیدیں + idx, 1);
	    }
#ورنہ
	    emsg(_(e_lowlevel_داخلہ_not_supported));
#اگر_ختم
	}
	ورنہ
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *کلیدیں_esc = vim_strsave_escape_csi(کلیدیں);
	    اگر (کلیدیں_esc == تہی)
		واپس;

	    ch_log(تہی, "feedkeys(%s): %s", typed ? "typed" : "", کلیدیں);

	    ins_typebuf(کلیدیں_esc, (remap ? REMAP_YES : REMAP_NONE),
				   داخل_کرو ? 0 : typebuf.tb_len, !typed, FALSE);
	    اگر (vgetc_busy
#اگر_تعریف_شدہ FEAT_TIMERS
		    || timer_busy
#اگر_ختم
		    || داخلہ_busy)
		typebuf_was_filled = TRUE;

	    vim_free(کلیدیں_esc);
	}

	اگر (execute)
	{
	    صحیح		save_پیغام_scroll = پیغام_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    پیغام_scroll = FALSE;

	    ch_log(تہی, "feedkeys() executing");

	    اگر (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_نسخہ = 0;
	    }

	    اگر (!dangerous)
	    {
		++ex_معمول_busy;
		++اندر_feedkeys;
	    }
	    ++allow_osc_کلید;
	    exec_معمول(TRUE, lowlevel, TRUE);
	    --allow_osc_کلید;
	    اگر (!dangerous)
	    {
		--ex_معمول_busy;
		--اندر_feedkeys;
	    }

	    پیغام_scroll |= save_پیغام_scroll;

	    اگر (context)
		current_sctx = save_sctx;
	}
    }
}

/*
 * "fnameescape({string})" function
 */
    ساکن خالی
f_fnameescape(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    ساکن خالی
f_foreground(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
    {
	gui_mch_سیٹ_foreground();
	واپس;
    }
#اگر_ختم
#اگر defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_سیٹ_foreground();
#اگر_ختم
}

/*
 * "function()" function
 * "funcref()" function
 */
    ساکن خالی
common_function(typval_T *argvars, typval_T *rettv, صحیح is_funcref)
{
    char_u	*s;
    char_u	*name;
    صحیح		use_string = FALSE;
    partial_T   *arg_pt = تہی;
    char_u	*trans_name = تہی;
    صحیح		is_global = FALSE;
    char_u	*آغاز_bracket = تہی;

    اگر (اندر_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	واپس;

    اگر (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    ورنہ اگر (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != تہی)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    ورنہ
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    اگر (s == تہی)
    {
	semsg(_(e_invalid_دلیل_str), "NULL");
	واپس;
    }

    اگر ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == تہی) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, تہی);
	اگر (*name == '<')
	{
	    // generic function
	    آغاز_bracket = name;
	    اگر (skip_generic_func_type_دلائل(&name) == FAIL)
		جاؤ theend;
	}
	اگر (*name != NUL)
	    s = تہی;
    }

    اگر (s == تہی || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == تہی))
	semsg(_(e_invalid_دلیل_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    ورنہ اگر (trans_name != تہی && (is_funcref
			 ? ڈھونڈو_func(trans_name, is_global) == تہی
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    ورنہ
    {
	صحیح	dict_idx = 0;
	صحیح	arg_idx = 0;
	list_T	*list = تہی;

	اگر (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	ورنہ اگر (trans_name != تہی && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	ورنہ
	    name = vim_strsave(s);

	اگر (argvars[1].v_type != VAR_UNKNOWN)
	{
	    اگر (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    ورنہ اگر (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    ورنہ
		// function(name, [args])
		arg_idx = 1;
	    اگر (dict_idx > 0)
	    {
		اگر (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    جاؤ theend;
		}
		اگر (argvars[dict_idx].vval.v_dict == تہی)
		    dict_idx = 0;
	    }
	    اگر (arg_idx > 0)
	    {
		اگر (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_دوسرا_دلیل_کا_function_must_be_list_or_dict));
		    vim_free(name);
		    جاؤ theend;
		}
		list = argvars[arg_idx].vval.v_list;
		اگر (list == تہی || list->lv_len == 0)
		    arg_idx = 0;
		ورنہ اگر (list->lv_len > MAX_FUNC_دلائل)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    جاؤ theend;
		}
	    }
	}
	اگر (dict_idx > 0 || arg_idx > 0 || arg_pt != تہی || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    اگر (pt == تہی)
		vim_free(name);
	    ورنہ
	    {
		اگر (arg_idx > 0 || (arg_pt != تہی && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    صحیح		i = 0;
		    صحیح		arg_len = 0;
		    صحیح		lv_len = 0;

		    اگر (arg_pt != تہی)
			arg_len = arg_pt->pt_argc;
		    اگر (list != تہی)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    اگر (pt->pt_argv == تہی)
		    {
			vim_free(pt);
			vim_free(name);
			جاؤ theend;
		    }
		    برائے (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    اگر (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_اشیاء(list, li)
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
		ورنہ اگر (arg_pt != تہی)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    اگر (pt->pt_dict != تہی)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    اگر (pt->pt_obj != تہی)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		اگر (arg_pt != تہی && arg_pt->pt_func != تہی)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		ورنہ اگر (is_funcref)
		{
		    pt->pt_func = ڈھونڈو_func(trans_name, is_global);
		    اگر (IS_GENERIC_FUNC(pt->pt_func) && آغاز_bracket != تہی)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&آغاز_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		ورنہ
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		اگر (arg_pt != تہی)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	ورنہ
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    اگر (آغاز_bracket == تہی)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    ورنہ
	    {
		// generic function
		size_t len = STRLEN(name) + STRLEN(آغاز_bracket);
		rettv->vval.v_string = alloc(len + 1);
		اگر (rettv->vval.v_string != تہی)
		{
		    STRCPY(rettv->vval.v_string, name);
		    STRCAT(rettv->vval.v_string, آغاز_bracket);
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
    ساکن خالی
f_funcref(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, TRUE);
}

/*
 * "function()" function
 */
    ساکن خالی
f_function(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, FALSE);
}

/*
 * "garbagecollect()" function
 */
    ساکن خالی
f_garbagecollect(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (اندر_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	واپس;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    اگر (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
	garbage_collect_at_exit = TRUE;
}

/*
 * "get()" function
 */
    ساکن خالی
f_get(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    list_T	*l;
    tuple_T	*tuple;
    dictitem_T	*di;
    dict_T	*d;
    typval_T	*tv = تہی;
    صحیح		what_is_dict = FALSE;

    اگر (argvars[0].v_type == VAR_BLOB)
    {
	صحیح error = FALSE;
	صحیح idx = tv_get_نمبر_chk(&argvars[1], &error);

	اگر (!error)
	{
	    rettv->v_type = VAR_نمبر;
	    اگر (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    اگر (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_نمبر = -1;
	    ورنہ
	    {
		rettv->vval.v_نمبر = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    ورنہ اگر (argvars[0].v_type == VAR_LIST)
    {
	اگر ((l = argvars[0].vval.v_list) != تہی)
	{
	    صحیح		error = FALSE;

	    li = list_ڈھونڈو(l, (طویل)tv_get_نمبر_chk(&argvars[1], &error));
	    اگر (!error && li != تہی)
		tv = &li->li_tv;
	}
    }
    ورنہ اگر (argvars[0].v_type == VAR_TUPLE)
    {
	اگر ((tuple = argvars[0].vval.v_tuple) != تہی)
	{
	    صحیح		error = FALSE;
	    طویل	idx;

	    idx = (طویل)tv_get_نمبر_chk(&argvars[1], &error);
	    اگر (!error)
		tv = tuple_ڈھونڈو(tuple, idx);
	}
    }
    ورنہ اگر (argvars[0].v_type == VAR_DICT)
    {
	اگر ((d = argvars[0].vval.v_dict) != تہی)
	{
	    di = dict_ڈھونڈو(d, tv_get_string(&argvars[1]), -1);
	    اگر (di != تہی)
		tv = &di->di_tv;
	}
    }
    ورنہ اگر (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	اگر (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	ورنہ
	{
	    CLEAR_میدان(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	اگر (pt != تہی)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    اگر (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		اگر (name == تہی)
		    rettv->vval.v_string = تہی;
		ورنہ
		{
		    اگر (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    اگر (*what == 'n' && pt->pt_name == تہی
							&& pt->pt_func != تہی)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    ورنہ اگر (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		اگر (pt->pt_dict != تہی)
		    rettv_dict_سیٹ(rettv, pt->pt_dict);
	    }
	    ورنہ اگر (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		اگر (rettv_list_alloc(rettv) == OK)
		{
		    صحیح i;

		    برائے (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    ورنہ اگر (STRCMP(what, "arity") == 0)
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
		    ورنہ اگر (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    ورنہ
			required -= pt->pt_argc;

		    dict_جوڑو_نمبر(dict, "required", required);
		    dict_جوڑو_نمبر(dict, "optional", optional);
		    dict_جوڑو_bool(dict, "varargs", varargs);
		}
	    }
	    ورنہ
		semsg(_(e_invalid_دلیل_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    اگر (!what_is_dict)
		واپس;
	}
    }
    ورنہ
	semsg(_(e_دلیل_کا_str_must_be_list_tuple_dictionary_or_blob), "get()");

    اگر (tv == تہی)
    {
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    ورنہ
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
    ساکن خالی
f_getbgcolor(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	اگر (gui.اندر_use)
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
		(خالی)syn_id2colors(hl_id, &fg, &bg);
		اگر (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_نمبر(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_نمبر(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_نمبر(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    واپس;
		}
	    }
	}
    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	اگر (رکن_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_نمبر(rettv->vval.v_list, (varnumber_T)r);
	    list_append_نمبر(rettv->vval.v_list, (varnumber_T)g);
	    list_append_نمبر(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#اگر_ختم
}

/*
 * "getcellpixels()" function
 */
    ساکن خالی
f_getcellpixels(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

#اگر defined(FEAT_GUI)
    اگر (gui.اندر_use)
    {
	// success pixel size and no gui.
	list_append_نمبر(rettv->vval.v_list, (varnumber_T)gui.char_چوڑائی);
	list_append_نمبر(rettv->vval.v_list, (varnumber_T)gui.char_اونچائی);
    }
    ورنہ
#اگر_ختم
    {
#اگر defined(UNIX) || defined(MSWIN)
	list_append_نمبر(rettv->vval.v_list, (varnumber_T)cell_چوڑائی);
	list_append_نمبر(rettv->vval.v_list, (varnumber_T)cell_اونچائی);
#ورنہ
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_نمبر(rettv->vval.v_list, (varnumber_T)-1);
	list_append_نمبر(rettv->vval.v_list, (varnumber_T)-1);
#اگر_ختم
    }

}

/*
 * "getchangelist()" function
 */
    ساکن خالی
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    صحیح		i;
    list_T	*l;
    dict_T	*d;
    صحیح		changelistindex;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_opt_بفر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    ورنہ
	buf = tv_get_buf_from_arg(&argvars[0]);
    اگر (buf == تہی)
	واپس;

    l = list_alloc();
    اگر (l == تہی)
	واپس;
    اگر (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	واپس;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    اگر (buf == curwin->w_بفر)
    {
	changelistindex = curwin->w_changelistidx;
    }
    ورنہ
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    اگر (wip->wi_win == curwin)
		توڑو;
	changelistindex = wip != تہی ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_نمبر(rettv->vval.v_list, (varnumber_T)changelistindex);

    برائے (i = 0; i < buf->b_changelistlen; ++i)
    {
	اگر (buf->b_changelist[i].lnum == 0)
	    جاری;
	اگر ((d = dict_alloc()) == تہی)
	    واپس;
	اگر (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    واپس;
	}
	dict_جوڑو_نمبر(d, "lnum", (طویل)buf->b_changelist[i].lnum);
	dict_جوڑو_نمبر(d, "col", (طویل)buf->b_changelist[i].ستون);
	dict_جوڑو_نمبر(d, "coladd", (طویل)buf->b_changelist[i].coladd);
    }
}

    ساکن خالی
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    صحیح		getcurpos,
    صحیح		charcol)
{
    pos_T	*fp = تہی;
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
		wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
		اگر (wp != تہی)
		    fp = &wp->w_cursor;
	    }
	    ورنہ
		fp = &curwin->w_cursor;
	    اگر (fp != تہی && charcol)
	    {
		pos = *fp;
		pos.ستون =
		    buf_byteidx_مقصد_charidx(wp->w_بفر, pos.lnum, pos.ستون);
		fp = &pos;
	    }
	}
	ورنہ
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	اگر (fnum != -1)
	    list_append_نمبر(l, (varnumber_T)fnum);
	ورنہ
	    list_append_نمبر(l, (varnumber_T)0);
	list_append_نمبر(l, (fp != تہی) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_نمبر(l, (fp != تہی)
		     ? (varnumber_T)(fp->ستون == MAXCOL ? MAXCOL : fp->ستون + 1)
							    : (varnumber_T)0);
	list_append_نمبر(l, (fp != تہی) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	اگر (getcurpos)
	{
	    صحیح	    save_سیٹ_curswant = curwin->w_سیٹ_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    اگر (wp == curwin)
		update_curswant();
	    list_append_نمبر(l, wp == تہی ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    اگر (wp == curwin && save_سیٹ_curswant)
	    {
		curwin->w_سیٹ_curswant = save_سیٹ_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    ورنہ
	rettv->vval.v_نمبر = FALSE;
}

/*
 * "getcharpos()" function
 */
    ساکن خالی
f_getcharpos(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    ساکن خالی
f_getcharsearch(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    اگر (rettv_dict_alloc(rettv) != OK)
	واپس;

    dict_T *dict = rettv->vval.v_dict;

    dict_جوڑو_string(dict, "char", آخری_csearch());
    dict_جوڑو_نمبر(dict, "forward", آخری_csearch_forward());
    dict_جوڑو_نمبر(dict, "until", آخری_csearch_until());
}

/*
 * "getenv()" function
 */
    ساکن خالی
f_getenv(typval_T *argvars, typval_T *rettv)
{
    صحیح	    mustfree = FALSE;
    char_u  *p;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    اگر (p == تہی)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_نمبر = VVAL_NULL;
	واپس;
    }
    اگر (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    ساکن خالی
f_getfontname(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	واپس;

#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
    {
	GuiFont font;
	char_u	*name = تہی;

	اگر (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	ورنہ
	{
	    name = tv_get_string(&argvars[0]);
	    اگر (STRCMP(name, "*") == 0)	    // don't use font dialog
		واپس;
	    font = gui_mch_get_font(name, FALSE);
	    اگر (font == NOFONT)
		واپس;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	اگر (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#اگر_ختم
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
ساکن ساخت
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
    {arg_chan_or_کام,		"channel,job"},
    {arg_cursor1,		"number,string,list<any>"},
    {arg_dict_any,		"dict<any>"},
    {arg_dict_any_or_string,	"dict<any>,string"},
    {arg_extend3,		"number,string"},
    {arg_filter_func,		"string,func"},
    {arg_float_or_nr,		"float,number"},
    {arg_foreach_func,		"string,func"},
    {arg_get1,			"blob,list<any>,tuple<any>,dict<any>,func"},
    {arg_شے_کا_پچھلا,		"any"},
    {arg_کام,			"job"},
    {arg_len1,
	    "string,number,blob,list<any>,tuple<any>,dict<any>,object<any>"},
    {arg_list_any,		"list<any>"},
    {arg_list_any_mod,		"list<any>"},
    {arg_list_نمبر,		"list<number>"},
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
    {arg_نمبر,		"number"},
    {arg_object,		"object<any>"},
    {arg_remove2,		"number,string"},
    {arg_repeat1,		"string,number,blob,list<any>,tuple<any>"},
    {arg_الٹ,		"list<any>,tuple<any>,blob,string"},
    {arg_same_as_پچھلا,		"any"},
    {arg_same_struct_as_پچھلا,	"any"},
    {arg_slice1,		"list<any>,tuple<any>,blob,string"},
    {arg_ترتیب_دو_how,		"string,func"},
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
    ساکن list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    حرف	*types = "any";

    اگر (l == تہی)
	واپس تہی;
    برائے (صحیح i = 0; i < (صحیح)ARRAY_لمبائی(argcheck_types); ++i)
	اگر (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    توڑو;
	}
    برائے (حرف *p = types; ; )
    {
	حرف	*انتہا = (حرف *)vim_strchr((char_u *)p, ',');
	صحیح	len = انتہا == تہی ? (صحیح)STRLEN(p) : (صحیح)(انتہا - p);

	اگر (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    واپس تہی;
	}
	اگر (انتہا == تہی)
	    توڑو;
	p = انتہا + 1;
    }
    واپس l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    ساکن صحیح
getinfo_argtypes(
	list_T			*l,
	ثابت funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    صحیح		argcount = 0;

    اگر (l != تہی)
	FOR_ALL_LIST_اشیاء(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    اگر (s == تہی)
		واپس -1;
	    اگر (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_دلائل)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		واپس -1;
	    }
	    type = parse_type(&p, type_gap, تہی, تہی, TRUE);
	    اگر (type == تہی)
		واپس -1;
	    اگر (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		واپس -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    برائے ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_دلائل; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    واپس argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    ساکن حرف *
builtin_ret_type_name(
	صحیح	    idx,
	صحیح	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	حرف	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = اندرونی_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    واپس type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
قسمتعریف شمار {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_قسم_T;

ساکن ساخت
{
    حرف		*name;
    getinfo_قسم_T	قسم;
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
    ساکن خالی
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_قسم_T	قسم;
    صحیح			ki;
    صحیح			idx;
    ثابت funcentry_T	*fe;
    dict_T		*d;
    list_T		*دلائل;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_دلائل];
    list_T		*argtypes_list = تہی;
    صحیح			argcount;
    صحیح			vim9 = -1;	// -1: the context of the caller
    حرف		*ret_name;
    حرف		*tofree;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;
    اگر (اندر_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;
    اگر (check_for_opt_dict_arg(argvars, 2) == FAIL)
	واپس;

    kindname = tv_get_string(&argvars[0]);
    برائے (ki = 0; ki < (صحیح)ARRAY_لمبائی(getinfo_kinds); ++ki)
	اگر (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    توڑو;
    اگر (ki == (صحیح)ARRAY_لمبائی(getinfo_kinds))
    {
	semsg(_(e_invalid_دلیل_str), kindname);
	واپس;
    }
    قسم = getinfo_kinds[ki].قسم;
    name = tv_get_string_buf(&argvars[1], namebuf);

    اگر (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != تہی)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	صحیح	    todo = (صحیح)opts->dv_hashtab.ht_مستعمل;

	FOR_ALL_HASHTAB_اشیاء(&opts->dv_hashtab, hi, todo)
	{
	    اگر (HASHITEM_خالی_خانے(hi))
		جاری;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    اگر (STRCMP(di->di_کلید, "argtypes") == 0 && قسم == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    ورنہ اگر (STRCMP(di->di_کلید, "vim9") == 0 && قسم == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    ورنہ
	    {
		semsg(_(e_invalid_دلیل_str), di->di_کلید);
		واپس;
	    }
	}
    }
    اگر (قسم == GI_VIMVAR)
    {
	اگر (STRNCMP(name, "v:", 2) == 0)
	    (خالی)vim_var_info(name + 2, rettv->vval.v_dict);
	واپس;
    }
    اگر (قسم == GI_COMMAND)
    {
	(خالی)ex_command_info(name, vim9, rettv->vval.v_dict);
	واپس;
    }
    اگر (قسم == GI_OPTION)
    {
	(خالی)option_info(name, rettv->vval.v_dict);
	واپس;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = ڈھونڈو_اندرونی_func_opt(name, FALSE);
    اگر (idx < 0)
    {
	(خالی)user_func_info(name, rettv->vval.v_dict);
	واپس;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, حجم(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    اگر (argcount < 0)
    {
	clear_type_list(&type_gap);
	واپس;
    }
    d = rettv->vval.v_dict;
    dict_جوڑو_string(d, "name", (char_u *)fe->f_name);
    dict_جوڑو_string(d, "kind", (char_u *)"builtin");
    dict_جوڑو_bool(d, "available", fe->f_func != تہی);
    dict_جوڑو_نمبر(d, "minargs", fe->f_min_argc);
    dict_جوڑو_نمبر(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_جوڑو_نمبر(d, "method", fe->f_argtype & FEARG_نقاب);

    دلائل = list_alloc();
    اگر (دلائل == تہی || dict_جوڑو_list(d, "args", دلائل) == FAIL)
    {
	list_unref(دلائل);
	clear_type_list(&type_gap);
	واپس;
    }
    argnames = (char_u *)fe->f_argnames;
    برائے (صحیح i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = تہی;
	صحیح	    argnamelen = 0;

	اگر (fe->f_argcheck == تہی)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    اگر (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		توڑو;
	    check = arg_any;
	}
	ورنہ
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    اگر ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == تہی)
		توڑو;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	اگر (argnames != تہی)
	{
	    char_u  *انتہا = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = انتہا == تہی ? (صحیح)STRLEN(argnames)
						     : (صحیح)(انتہا - argnames);
	    argnames = انتہا == تہی ? تہی : skipwhite(انتہا + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	اگر (arg == تہی || types == تہی
		|| dict_جوڑو_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    واپس;
	}
	اگر ((argnamelen > 0
		    && dict_جوڑو_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(دلائل, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    واپس;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    اگر (argtypes_list == تہی
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	حرف	*tofree_max;
	حرف	*max_name;

	برائے (صحیح i = argcount; i < fe->f_max_argc && i < MAX_FUNC_دلائل; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	اگر (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = تہی;
	    ret_name = "any";
	}
	vim_free(tofree_max);
    }
    dict_جوڑو_string(d, "returns", (char_u *)ret_name);
    vim_free(tofree);
    clear_type_list(&type_gap);
}

/*
 * "getjumplist()" function
 */
    ساکن خالی
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    صحیح		i;
    list_T	*l;
    dict_T	*d;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_opt_نمبر_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 1) == FAIL)))
	واپس;

    wp = ڈھونڈو_tabwin(&argvars[0], &argvars[1], تہی);
    اگر (wp == تہی)
	واپس;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    اگر (l == تہی)
	واپس;
    اگر (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	واپس;
    }

    list_append_نمبر(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    برائے (i = 0; i < wp->w_jumplistlen; ++i)
    {
	اگر (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    جاری;
	اگر ((d = dict_alloc()) == تہی)
	    واپس;
	اگر (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    واپس;
	}
	dict_جوڑو_نمبر(d, "lnum", (طویل)wp->w_jumplist[i].fmark.mark.lnum);
	dict_جوڑو_نمبر(d, "col", (طویل)wp->w_jumplist[i].fmark.mark.ستون);
	dict_جوڑو_نمبر(d, "coladd", (طویل)wp->w_jumplist[i].fmark.mark.coladd);
	dict_جوڑو_نمبر(d, "bufnr", (طویل)wp->w_jumplist[i].fmark.fnum);
	اگر (wp->w_jumplist[i].fname != تہی)
	    dict_جوڑو_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    ساکن خالی
f_getpid(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->vval.v_نمبر = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    ساکن خالی
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    ساکن خالی
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    ساکن خالی
f_getpos(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    ساکن صحیح
بلاک_def2str(ساخت بلاک_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    اگر (ret->string == تہی)
    {
	ret->لمبائی = 0;
	واپس FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->لمبائی = bd->startspaces;
    mch_memmove(ret->string + ret->لمبائی, bd->textstart, bd->textlen);
    ret->لمبائی += bd->textlen;
    vim_memset(ret->string + ret->لمبائی, ' ', bd->endspaces);
    ret->لمبائی += bd->endspaces;
    ret->string[ret->لمبائی] = NUL;

    واپس OK;
}

    ساکن صحیح
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
    صحیح		بلاک_چوڑائی = 0;
    صحیح		is_select_exclusive;
    صحیح		l;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس FAIL;

    اگر (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	واپس FAIL;

    اگر (list2fpos(&argvars[0], p1, &fnum1, تہی, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, تہی, FALSE) != OK
	    || fnum1 != fnum2)
	واپس FAIL;

    اگر (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	اگر (type == تہی)
	    type = default_type;
    }
    ورنہ
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    اگر (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    ورنہ اگر (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    ورنہ اگر (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	اگر (*p != NUL && ((بلاک_چوڑائی = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str_str), "type", type);
	    واپس FAIL;
	}
	*region_type = MBLOCK;
    }
    ورنہ
    {
	semsg(_(e_invalid_قدر_for_دلیل_str_str), "type", type);
	واپس FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    اگر (findbuf == تہی || findbuf->b_ml.ml_mfp == تہی)
    {
	emsg(_(e_بفر_is_not_loaded));
	واپس FAIL;
    }

    اگر (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_سطر_گنتی)
    {
	semsg(_(e_invalid_سطر_نمبر_nr), p1->lnum);
	واپس FAIL;
    }
    اگر (p1->ستون == MAXCOL)
	p1->ستون = ml_get_buf_len(findbuf, p1->lnum) + 1;
    ورنہ اگر (p1->ستون < 1 || p1->ستون > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_عمود_نمبر_nr), p1->ستون);
	واپس FAIL;
    }

    اگر (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_سطر_گنتی)
    {
	semsg(_(e_invalid_سطر_نمبر_nr), p2->lnum);
	واپس FAIL;
    }
    اگر (p2->ستون == MAXCOL)
	p2->ستون = ml_get_buf_len(findbuf, p2->lnum) + 1;
    ورنہ اگر (p2->ستون < 1 || p2->ستون > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_عمود_نمبر_nr), p2->ستون);
	واپس FAIL;
    }

    curbuf = findbuf;
    curwin->w_بفر = curbuf;
    virtual_op = virtual_فعال();

    // NOTE: Adjustment is needed.
    p1->ستون--;
    p2->ستون--;

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
    ورنہ اگر (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#اگر_تعریف_شدہ FEAT_LINEBREAK
	صحیح	lbr_saved = reset_lbr();
#اگر_ختم

	getvvcol(curwin, p1, &sc1, تہی, &ec1, 0);
	getvvcol(curwin, p2, &sc2, تہی, &ec2, 0);

#اگر_تعریف_شدہ FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#اگر_ختم

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->آغاز = *p1;
	oap->انتہا = *p2;
	oap->آغاز_vcol = MIN(sc1, sc2);
	اگر (بلاک_چوڑائی > 0)
	    oap->انتہا_vcol = oap->آغاز_vcol + بلاک_چوڑائی - 1;
	ورنہ اگر (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->انتہا_vcol = sc2 - 1;
	ورنہ
	    oap->انتہا_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    اگر (l > 1)
	p2->ستون += l - 1;

    واپس OK;
}

/*
 * "getregion()" function
 */
    ساکن خالی
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    صحیح			inclusive = TRUE;
    صحیح			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    صحیح			save_virtual;
    string_T		akt = {تہی, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    اگر (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	واپس;

    برائے (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	صحیح ret = OK;

	اگر (region_type == MBLOCK)
	{
	    ساخت بلاک_def	bd;

	    بلاک_prep(&oa, &bd, lnum, FALSE);
	    بلاک_def2str(&bd, &akt);
	}
	ورنہ اگر (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.لمبائی = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.لمبائی);
	    اگر (akt.string == تہی)
		akt.لمبائی = 0;
	    ورنہ
		akt.لمبائی = s.لمبائی;
	}
	ورنہ
	{
	    ساخت بلاک_def	bd;

	    charwise_بلاک_prep(p1, p2, &bd, lnum, inclusive);
	    بلاک_def2str(&bd, &akt);
	}

	اگر (akt.string == تہی)
	{
	    clear_tv(rettv);
	    (خالی)rettv_list_alloc(rettv);
	    توڑو;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (صحیح)akt.لمبائی);
	vim_free(akt.string);

	اگر (ret == FAIL)
	{
	    clear_tv(rettv);
	    (خالی)rettv_list_alloc(rettv);
	    توڑو;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_بفر = curbuf;
    virtual_op = save_virtual;
}

    ساکن خالی
جوڑو_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    اگر (l1 == تہی)
	واپس;

    اگر (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	واپس;
    }

    l2 = list_alloc();
    اگر (l2 == تہی)
	واپس;

    اگر (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	واپس;
    }

    l3 = list_alloc();
    اگر (l3 == تہی)
	واپس;

    اگر (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	واپس;
    }

    list_append_نمبر(l2, curbuf->b_fnum);
    list_append_نمبر(l2, p1.lnum);
    list_append_نمبر(l2, p1.ستون);
    list_append_نمبر(l2, p1.coladd);

    list_append_نمبر(l3, curbuf->b_fnum);
    list_append_نمبر(l3, p2.lnum);
    list_append_نمبر(l3, p2.ستون);
    list_append_نمبر(l3, p2.coladd);
}

/*
 * Compute the positions of the region segment on line "lnum".
 * "ret_p1" is set to the start position of the segment and "ret_p2" to its
 * end position.
 */
    ساکن خالی
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
	ret_p1->ستون = 1;
	ret_p1->coladd = 0;
	ret_p2->ستون = MAXCOL;
	ret_p2->coladd = 0;
    }
    ورنہ
    {
	ساخت بلاک_def	bd;

	اگر (region_type == MBLOCK)
	    بلاک_prep(oap, &bd, lnum, FALSE);
	ورنہ
	    charwise_بلاک_prep(p1, p2, &bd, lnum, inclusive);

	اگر (bd.is_oneChar)  // selection entirely inside one char
	{
	    اگر (region_type == MBLOCK)
	    {
		ret_p1->ستون = mb_prevptr(سطر, bd.textstart) - سطر + 1;
		ret_p1->coladd = bd.آغاز_char_vcols
					   - (bd.آغاز_vcol - oap->آغاز_vcol);
	    }
	    ورنہ
	    {
		ret_p1->ستون = p1.ستون + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	ورنہ اگر (region_type == MBLOCK && oap->آغاز_vcol > bd.آغاز_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->ستون = MAXCOL;
	    ret_p1->coladd = oap->آغاز_vcol - bd.آغاز_vcol;
	    bd.is_oneChar = TRUE;
	}
	ورنہ اگر (bd.startspaces > 0)
	{
	    ret_p1->ستون = mb_prevptr(سطر, bd.textstart) - سطر + 1;
	    ret_p1->coladd = bd.آغاز_char_vcols - bd.startspaces;
	}
	ورنہ
	{
	    ret_p1->ستون = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	اگر (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->ستون = ret_p1->ستون;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	ورنہ اگر (bd.endspaces > 0)
	{
	    ret_p2->ستون = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	ورنہ
	{
	    ret_p2->ستون = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    اگر (!allow_eol && ret_p1->ستون > سطر_len)
    {
	ret_p1->ستون = 0;
	ret_p1->coladd = 0;
    }
    ورنہ اگر (ret_p1->ستون > سطر_len + 1)
	ret_p1->ستون = سطر_len + 1;

    اگر (!allow_eol && ret_p2->ستون > سطر_len)
    {
	ret_p2->ستون = ret_p1->ستون == 0 ? 0 : سطر_len;
	ret_p2->coladd = 0;
    }
    ورنہ اگر (ret_p2->ستون > سطر_len + 1)
	ret_p2->ستون = سطر_len + 1;

    ret_p1->lnum = lnum;
    ret_p2->lnum = lnum;
}

/*
 * "getregionpos()" function
 */
    ساکن خالی
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
	واپس;

    اگر (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    اگر (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	آغاز_pos, انتہا_pos;

	getregionpos_سطر(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &آغاز_pos, &انتہا_pos);
	اگر (p2.lnum != p1.lnum)
	{
	    pos_T	غیر_مستعمل_2;

	    getregionpos_سطر(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &غیر_مستعمل_2, &انتہا_pos);
	}
	جوڑو_regionpos_range(rettv, آغاز_pos, انتہا_pos);
    }
    ورنہ
    {
	برائے (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_سطر(lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &ret_p1, &ret_p2);
	    جوڑو_regionpos_range(rettv, ret_p1, ret_p2);
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
    ساکن صحیح
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	اگر (strregname != تہی && اندر_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_رجسٹر_name_must_be_one_char_str), strregname);
	    strregname = تہی;
	}
	اگر (strregname == تہی)	    // type error; errmsg already given
	    واپس 0;
    }
    ورنہ
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    واپس *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    ساکن خالی
f_getreg(typval_T *argvars, typval_T *rettv)
{
    صحیح		regname;
    صحیح		arg2 = FALSE;
    صحیح		return_list = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	واپس;

    regname = getreg_get_regname(argvars);
    اگر (regname == 0)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	صحیح		error = FALSE;

	arg2 = (صحیح)tv_get_bool_chk(&argvars[1], &error);

	اگر (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (صحیح)tv_get_bool_chk(&argvars[2], &error);
	اگر (error)
	    واپس;
    }

    اگر (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_اظہاریہ_SRC : 0) | GREG_LIST);
	اگر (rettv->vval.v_list == تہی)
	    (خالی)rettv_list_alloc(rettv);
	ورنہ
	    ++rettv->vval.v_list->lv_refcount;
    }
    ورنہ
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = get_reg_contents(regname,
						    arg2 ? GREG_اظہاریہ_SRC : 0);
    }
}

/*
 * "getregtype()" function
 */
    ساکن خالی
f_getregtype(typval_T *argvars, typval_T *rettv)
{
    صحیح		regname;
    char_u	buf[NUMBUFLEN + 2];
    طویل	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	واپس;

    regname = getreg_get_regname(argvars);
    اگر (regname == 0)
	واپس;

    buf[0] = NUL;
    buf[1] = NUL;
    انتخاب (get_reg_type(regname, &reglen))
    {
	حالت MLINE: buf[0] = 'V'; توڑو;
	حالت MCHAR: buf[0] = 'v'; توڑو;
	حالت MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((حرف *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		توڑو;
    }
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "gettagstack()" function
 */
    ساکن خالی
f_gettagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;			// default is current window

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
	اگر (wp == تہی)
	    واپس;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    ساکن خالی
f_gettext(typval_T *argvars, typval_T *rettv)
{
#اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    حرف *پچھلا = تہی;
#اگر_ختم

    اگر (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	واپس;

    rettv->v_type = VAR_STRING;

    اگر (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != تہی &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	پچھلا = bind_textdomain_codeset((ثابت حرف *)argvars[1].vval.v_string, (حرف *)p_enc);
#اگر_ختم

#اگر defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((ثابت حرف *)argvars[1].vval.v_string, (ثابت حرف *)argvars[0].vval.v_string));
#ورنہ
	textdomain((ثابت حرف *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#اگر_ختم

#اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	اگر (پچھلا != تہی)
	    bind_textdomain_codeset((ثابت حرف *)argvars[1].vval.v_string, پچھلا);
#اگر_ختم
    }
    ورنہ
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
}

// for VIM_VERSION_ defines
#شامل "version.h"

/*
 * "has()" function
 */
    خالی
f_has(typval_T *argvars, typval_T *rettv)
{
    صحیح		i;
    char_u	*name;
    صحیح		x = FALSE;
    صحیح		n = FALSE;
    قسمتعریف ساخت {
	حرف *name;
	مختصر present;
    } has_شے_T;
    ساکن has_شے_T has_list[] =
    {
	{"amiga",
#اگر_تعریف_شدہ AMIGA
		1
#ورنہ
		0
#اگر_ختم
		},
	{"android",
#اگر_تعریف_شدہ __ANDROID__
		1
#ورنہ
		0
#اگر_ختم
		},
	{"arp",
#اگر defined(AMIGA) && defined(FEAT_ARP)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"haiku",
#اگر_تعریف_شدہ __HAIKU__
		1
#ورنہ
		0
#اگر_ختم
		},
	{"bsd",
#اگر defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"hpux",
#اگر_تعریف_شدہ hpux
		1
#ورنہ
		0
#اگر_ختم
		},
	{"hurd",
#اگر_تعریف_شدہ __GNU__
		1
#ورنہ
		0
#اگر_ختم
		},
	{"linux",
#اگر_تعریف_شدہ __linux__
		1
#ورنہ
		0
#اگر_ختم
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#اگر_تعریف_شدہ MACOS_X
		1
#ورنہ
		0
#اگر_ختم
		},
	{"osx",		// Mac OS X
#اگر_تعریف_شدہ MACOS_X
		1
#ورنہ
		0
#اگر_ختم
		},
	{"macunix",	// Mac OS X, with the darwin feature
#اگر defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"osxdarwin",	// synonym for macunix
#اگر defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"qnx",
#اگر_تعریف_شدہ __QNX__
		1
#ورنہ
		0
#اگر_ختم
		},
	{"sun",
#اگر_تعریف_شدہ SUN_SYSTEM
		1
#ورنہ
		0
#اگر_ختم
		},
	{"termux",
#اگر_تعریف_شدہ __TERMUX__
		1
#ورنہ
		0
#اگر_ختم
		},
	{"unix",
#اگر_تعریف_شدہ UNIX
		1
#ورنہ
		0
#اگر_ختم
		},
	{"vms",
#اگر_تعریف_شدہ VMS
		1
#ورنہ
		0
#اگر_ختم
		},
	{"win32",
#اگر_تعریف_شدہ MSWIN
		1
#ورنہ
		0
#اگر_ختم
		},
	{"win32unix",
#اگر_تعریف_شدہ WIN32UNIX
		1
#ورنہ
		0
#اگر_ختم
		},
	{"win64",
#اگر_تعریف_شدہ _WIN64
		1
#ورنہ
		0
#اگر_ختم
		},
	{"ebcdic", 0 },
	{"fname_case",
#اگر_غیر_تعریف_شدہ CASE_INSENSITIVE_FILENAME
		1
#ورنہ
		0
#اگر_ختم
		},
	{"acl",
#اگر_تعریف_شدہ HAVE_ACL
		1
#ورنہ
		0
#اگر_ختم
		},
	{"arabic",
#اگر_تعریف_شدہ FEAT_ARABIC
		1
#ورنہ
		0
#اگر_ختم
		},
	{"autocmd", 1},
	{"autochdir",
#اگر_تعریف_شدہ FEAT_AUTOCHDIR
		1
#ورنہ
		0
#اگر_ختم
		},
	{"autoservername",
#اگر_تعریف_شدہ FEAT_AUTOSERVERNAME
		1
#ورنہ
		0
#اگر_ختم
		},
	{"socketserver",
#اگر_تعریف_شدہ FEAT_SOCKETSERVER
		1
#ورنہ
		0
#اگر_ختم
		},
	{"balloon_eval",
#اگر_تعریف_شدہ FEAT_BEVAL_GUI
		1
#ورنہ
		0
#اگر_ختم
		},
	{"balloon_multiline",
#اگر_تعریف_شدہ FEAT_BEVAL_GUI
		1
#ورنہ
		0
#اگر_ختم
		},
	{"balloon_eval_term",
#اگر_تعریف_شدہ FEAT_BEVAL_رکن
		1
#ورنہ
		0
#اگر_ختم
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#اگر defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#ورنہ
		0
#اگر_ختم
		},
	{"byte_offset",
#اگر_تعریف_شدہ FEAT_BYTEOFF
		1
#ورنہ
		0
#اگر_ختم
		},
	{"channel",
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
		1
#ورنہ
		0
#اگر_ختم
		},
	{"cindent", 1},
	{"clientserver",
#اگر_تعریف_شدہ FEAT_CLIENTSERVER
		1
#ورنہ
		0
#اگر_ختم
		},
	{"clipboard",
#اگر_تعریف_شدہ FEAT_CLIPBOARD
		1
#ورنہ
		0
#اگر_ختم
		},
	{"clipboard_provider",
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
		1
#ورنہ
		0
#اگر_ختم
		},
	{"cmdline_compl", 1},
	{"cmdline_hist", 1},
	{"cmdwin", 1},
	{"comments", 1},
	{"conceal",
#اگر_تعریف_شدہ FEAT_CONCEAL
		1
#ورنہ
		0
#اگر_ختم
		},
	{"cryptv",
#اگر_تعریف_شدہ FEAT_CRYPT
		1
#ورنہ
		0
#اگر_ختم
		},
	{"crypt-blowfish",
#اگر_تعریف_شدہ FEAT_CRYPT
		1
#ورنہ
		0
#اگر_ختم
		},
	{"crypt-blowfish2",
#اگر_تعریف_شدہ FEAT_CRYPT
		1
#ورنہ
		0
#اگر_ختم
		},
	{"cscope",
#اگر_تعریف_شدہ FEAT_CSCOPE
		1
#ورنہ
		0
#اگر_ختم
		},
	{"cursorbind", 1},
	{"cursorshape",
#اگر_تعریف_شدہ CURSOR_SHAPE
		1
#ورنہ
		0
#اگر_ختم
		},
	{"debug",
#اگر_تعریف_شدہ DEBUG
		1
#ورنہ
		0
#اگر_ختم
		},
	{"dialog_con",
#اگر_تعریف_شدہ FEAT_CON_DIALOG
		1
#ورنہ
		0
#اگر_ختم
		},
	{"dialog_con_gui",
#اگر defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"dialog_gui",
#اگر_تعریف_شدہ FEAT_GUI_DIALOG
		1
#ورنہ
		0
#اگر_ختم
		},
	{"diff",
#اگر_تعریف_شدہ FEAT_DIFF
		1
#ورنہ
		0
#اگر_ختم
		},
	{"digraphs",
#اگر_تعریف_شدہ FEAT_DIGRAPHS
		1
#ورنہ
		0
#اگر_ختم
		},
	{"directx",
#اگر_تعریف_شدہ FEAT_DIRECTX
		1
#ورنہ
		0
#اگر_ختم
		},
	{"dnd",
#اگر_تعریف_شدہ FEAT_DND
		1
#ورنہ
		0
#اگر_ختم
		},
	{"drop_file",
#اگر_تعریف_شدہ HAVE_DROP_FILE
		1
#ورنہ
		0
#اگر_ختم
		},
	{"emacs_tags",
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
		1
#ورنہ
		0
#اگر_ختم
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
		1
#ورنہ
		0
#اگر_ختم
		},
	{"file_in_path", 1},
	{"filterpipe",
#اگر defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"find_in_path",
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
		1
#ورنہ
		0
#اگر_ختم
		},
	{"float", 1},
	{"folding",
#اگر_تعریف_شدہ FEAT_FOLDING
		1
#ورنہ
		0
#اگر_ختم
		},
	{"footer", 0},
	{"fork",
#اگر !defined(USE_SYSTEM) && defined(UNIX)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"gettext",
#اگر_تعریف_شدہ FEAT_GETTEXT
		1
#ورنہ
		0
#اگر_ختم
		},
	{"gui",
#اگر_تعریف_شدہ FEAT_GUI
		1
#ورنہ
		0
#اگر_ختم
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#اگر_تعریف_شدہ FEAT_GUI_GTK
		1
#ورنہ
		0
#اگر_ختم
		},
	{"gui_gtk2",
#اگر defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"gui_gtk3",
#اگر defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"gui_gtk4",
#اگر defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"gui_gnome",
#اگر_تعریف_شدہ FEAT_GUI_GNOME
		1
#ورنہ
		0
#اگر_ختم
		},
	{"gui_haiku",
#اگر_تعریف_شدہ FEAT_GUI_HAIKU
		1
#ورنہ
		0
#اگر_ختم
		},
	{"gui_mac", 0},
	{"gui_motif",
#اگر_تعریف_شدہ FEAT_GUI_MOTIF
		1
#ورنہ
		0
#اگر_ختم
		},
	{"gui_photon",
#اگر_تعریف_شدہ FEAT_GUI_PHOTON
		1
#ورنہ
		0
#اگر_ختم
		},
	{"gui_win32",
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
		1
#ورنہ
		0
#اگر_ختم
		},
	{"iconv",
#اگر defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"image",
#اگر_تعریف_شدہ FEAT_IMAGE
		1
#ورنہ
		0
#اگر_ختم
		},
	{"image_popup",
#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
		1
#ورنہ
		0
#اگر_ختم
		},
	{"insert_expand", 1},
	{"ipv6",
#اگر_تعریف_شدہ FEAT_IPV6
		1
#ورنہ
		0
#اگر_ختم
	},
	{"job",
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
		1
#ورنہ
		0
#اگر_ختم
		},
	{"jumplist", 1},
	{"keymap",
#اگر_تعریف_شدہ FEAT_KEYMAP
		1
#ورنہ
		0
#اگر_ختم
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#اگر_تعریف_شدہ FEAT_LANGMAP
		1
#ورنہ
		0
#اگر_ختم
		},
	{"libcall",
#اگر_تعریف_شدہ FEAT_LIBCALL
		1
#ورنہ
		0
#اگر_ختم
		},
	{"linebreak",
#اگر_تعریف_شدہ FEAT_LINEBREAK
		1
#ورنہ
		0
#اگر_ختم
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#اگر defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"menu",
#اگر_تعریف_شدہ FEAT_MENU
		1
#ورنہ
		0
#اگر_ختم
		},
	{"mksession",
#اگر_تعریف_شدہ FEAT_SESSION
		1
#ورنہ
		0
#اگر_ختم
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
		1
#ورنہ
		0
#اگر_ختم
		},
	{"mouse_dec",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"mouse_gpm",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"mouse_jsbterm",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"mouse_netterm",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"mouse_pterm",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"mouse_sgr",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"mouse_sysmouse",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"mouse_urxvt",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"mouse_xterm",
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#اگر_تعریف_شدہ FEAT_MBYTE_IME
		1
#ورنہ
		0
#اگر_ختم
		},
	{"multi_lang",
#اگر_تعریف_شدہ FEAT_MULTI_LANG
		1
#ورنہ
		0
#اگر_ختم
		},
	{"mzscheme",
#اگر defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"nanotime",
#اگر_تعریف_شدہ ST_MTIM_NSEC
		1
#ورنہ
		0
#اگر_ختم
	},
	{"num64", 1},
	{"ole",
#اگر_تعریف_شدہ FEAT_OLE
		1
#ورنہ
		0
#اگر_ختم
		},
	{"packages",
#اگر_تعریف_شدہ FEAT_EVAL
		1
#ورنہ
		0
#اگر_ختم
		},
	{"path_extra", 1},
	{"perl",
#اگر defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"persistent_undo",
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
		1
#ورنہ
		0
#اگر_ختم
		},
	{"python_compiled",
#اگر defined(FEAT_PYTHON)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"python_dynamic",
#اگر defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"python",
#اگر defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"pythonx",
#اگر (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#ورنہ
		0
#اگر_ختم
		},
	{"python3_compiled",
#اگر defined(FEAT_PYTHON3)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"python3_dynamic",
#اگر defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"python3_stable",
#اگر defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"python3",
#اگر defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"popupwin",
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		1
#ورنہ
		0
#اگر_ختم
		},
	{"postscript",
#اگر_تعریف_شدہ FEAT_POSTSCRIPT
		1
#ورنہ
		0
#اگر_ختم
		},
	{"pango",
#اگر_تعریف_شدہ FEAT_PRINT_PANGO
		1
#ورنہ
		0
#اگر_ختم
		},
	{"printer",
#اگر_تعریف_شدہ FEAT_PRINTER
		1
#ورنہ
		0
#اگر_ختم
		},
	{"profile",
#اگر_تعریف_شدہ FEAT_PROFILE
		1
#ورنہ
		0
#اگر_ختم
		},
	{"prof_nsec",
#اگر_تعریف_شدہ PROF_NSEC
		1
#ورنہ
		0
#اگر_ختم
		},
	{"reltime",
#اگر_تعریف_شدہ FEAT_RELTIME
		1
#ورنہ
		0
#اگر_ختم
		},
	{"quickfix",
#اگر_تعریف_شدہ FEAT_QUICKFIX
		1
#ورنہ
		0
#اگر_ختم
		},
	{"rightleft",
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		1
#ورنہ
		0
#اگر_ختم
		},
	{"ruby",
#اگر defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#اگر_تعریف_شدہ FEAT_SIGNS
		1
#ورنہ
		0
#اگر_ختم
		},
	{"smartindent", 1},
	{"startuptime",
#اگر_تعریف_شدہ STARTUPTIME
		1
#ورنہ
		0
#اگر_ختم
		},
	{"statusline",
#اگر_تعریف_شدہ FEAT_STL_OPT
		1
#ورنہ
		0
#اگر_ختم
		},
	{"statusline_click",
#اگر_تعریف_شدہ FEAT_STL_OPT
		1
#ورنہ
		0
#اگر_ختم
		},
	{"netbeans_intg",
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
		1
#ورنہ
		0
#اگر_ختم
		},
	{"sodium",
#اگر defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"sound",
#اگر_تعریف_شدہ FEAT_SOUND
		1
#ورنہ
		0
#اگر_ختم
		},
	{"spell",
#اگر_تعریف_شدہ FEAT_SPELL
		1
#ورنہ
		0
#اگر_ختم
		},
	{"syntax",
#اگر_تعریف_شدہ FEAT_SYN_HL
		1
#ورنہ
		0
#اگر_ختم
		},
	{"system",
#اگر defined(USE_SYSTEM) || !defined(UNIX)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"tabpanel",
#اگر defined(FEAT_TABPANEL)
		1,
#ورنہ
		0,
#اگر_ختم
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#اگر defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"termguicolors",
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
		1
#ورنہ
		0
#اگر_ختم
		},
	{"terminal",
#اگر defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"terminfo",
#اگر_تعریف_شدہ TERMINFO
		1
#ورنہ
		0
#اگر_ختم
		},
	{"termresponse",
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
		1
#ورنہ
		0
#اگر_ختم
		},
	{"textobjects", 1},
	{"textprop",
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		1
#ورنہ
		0
#اگر_ختم
		},
	{"tgetent",
#اگر_تعریف_شدہ HAVE_TGETENT
		1
#ورنہ
		0
#اگر_ختم
		},
	{"timers",
#اگر_تعریف_شدہ FEAT_TIMERS
		1
#ورنہ
		0
#اگر_ختم
		},
	{"title", 1},
	{"toolbar",
#اگر_تعریف_شدہ FEAT_TOOLBAR
		1
#ورنہ
		0
#اگر_ختم
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#اگر_تعریف_شدہ FEAT_VARTABS
		1
#ورنہ
		0
#اگر_ختم
		},
	{"vertsplit", 1},
	{"viminfo",
#اگر_تعریف_شدہ FEAT_VIMINFO
		1
#ورنہ
		0
#اگر_ختم
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
#اگر_تعریف_شدہ FEAT_VTP
		1
#ورنہ
		0
#اگر_ختم
		},
	{"wayland",
#اگر_تعریف_شدہ FEAT_WAYLAND
		1
#ورنہ
		0
#اگر_ختم
		},
	{"wayland_clipboard",
#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD
		1
#ورنہ
		0
#اگر_ختم
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#اگر_تعریف_شدہ FEAT_WAK
		1
#ورنہ
		0
#اگر_ختم
		},
	{"writebackup",
#اگر_تعریف_شدہ FEAT_WRITEBACKUP
		1
#ورنہ
		0
#اگر_ختم
		},
	{"xattr",
#اگر_تعریف_شدہ FEAT_XATTR
		1
#ورنہ
		0
#اگر_ختم
		},
	{"xim",
#اگر_تعریف_شدہ FEAT_XIM
		1
#ورنہ
		0
#اگر_ختم
		},
	{"xfontset",
#اگر_تعریف_شدہ FEAT_XFONTSET
		1
#ورنہ
		0
#اگر_ختم
		},
	{"xpm",
#اگر defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#ورنہ
		0
#اگر_ختم
		},
	{"xpm_w32",	// for backward compatibility
#اگر_تعریف_شدہ FEAT_XPM_W32
		1
#ورنہ
		0
#اگر_ختم
		},
	{"xsmp",
#اگر_تعریف_شدہ USE_XSMP
		1
#ورنہ
		0
#اگر_ختم
		},
	{"xsmp_interact",
#اگر_تعریف_شدہ USE_XSMP_INTERACT
		1
#ورنہ
		0
#اگر_ختم
		},
	{"xterm_clipboard",
#اگر_تعریف_شدہ FEAT_XCLIPBOARD
		1
#ورنہ
		0
#اگر_ختم
		},
	{"xterm_save",
#اگر_تعریف_شدہ FEAT_XTERM_SAVE
		1
#ورنہ
		0
#اگر_ختم
		},
	{"X11",
#اگر defined(UNIX) && defined(FEAT_X11)
		1
#ورنہ
		0
#اگر_ختم
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#ورنہ
		0
#اگر_ختم
		},
	{تہی, 0}
    };

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	واپس;

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
	    حرف	*انتہا;
	    صحیح	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (صحیح)strtoul((حرف *)name + 6, &انتہا, 10);
	    اگر (*انتہا == '.' && vim_isdigit(انتہا[1])
		    && انتہا[2] == '.' && vim_isdigit(انتہا[3]))
	    {
		minor = atoi(انتہا + 1);

		// Expect "patch-9.9.01234".
		n = (major < VIM_نسخہ_MAJOR
		     || (major == VIM_نسخہ_MAJOR
			 && (minor < VIM_نسخہ_MINOR
			     || (minor == VIM_نسخہ_MINOR
				 && has_patch(atoi(انتہا + 3))))));
	    }
	}
	ورنہ اگر (محفوظ_isdigit(name[5]))
	    n = has_patch(atoi((حرف *)name + 5));
    }
    ورنہ اگر (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    ورنہ اگر (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_داخلہ_isatty();
    }
    ورنہ اگر (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    ورنہ اگر (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    ورنہ اگر (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#اگر_تعریف_شدہ FEAT_GUI
	n = (gui.اندر_use || gui.starting);
#اگر_ختم
    }
    ورنہ اگر (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#اگر defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.اندر_use;	// gui_mch_browse() works when GUI is running
#اگر_ختم
    }
    ورنہ اگر (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#اگر_تعریف_شدہ FEAT_SYN_HL
	n = syntax_present(curwin);
#اگر_ختم
    }
    ورنہ اگر (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#اگر_تعریف_شدہ FEAT_VTP
	n = is_رکن_win32() && has_vtp_working();
#اگر_ختم
    }
    ورنہ اگر (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	n = netbeans_فعال();
#اگر_ختم
    }
    ورنہ اگر (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#اگر_تعریف_شدہ FEAT_MOUSE_GPM
	n = gpm_enabled();
#اگر_ختم
    }
    ورنہ اگر (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#اگر defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#اگر_ختم
    }
    ورنہ اگر (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#اگر_تعریف_شدہ FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_ستارہ.دستیاب;
#اگر_ختم
    }
    ورنہ اگر (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#اگر_تعریف_شدہ FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# اگر defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	اگر (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	ورنہ
	    n = FALSE;
# ورنہ
	n = FALSE;
# endif
#اگر_ختم
    }

    // Look up in has_list[] only if not already handled above.
    اگر (x == FALSE)
    {
	برائے (i = 0; has_list[i].name != تہی; ++i)
	    اگر (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		توڑو;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	اگر (x == TRUE && n == FALSE)
	{
	    اگر (0)
	    {
		// intentionally empty
	    }
#اگر_تعریف_شدہ VIMDLL
	    ورنہ اگر (STRICMP(name, "filterpipe") == 0)
		n = gui.اندر_use || gui.starting;
#اگر_ختم
#اگر defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    ورنہ اگر (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_LUA
	    ورنہ اگر (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_MZSCHEME
	    ورنہ اگر (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_PERL
	    ورنہ اگر (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_PYTHON
	    ورنہ اگر (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_PYTHON3
	    ورنہ اگر (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#اگر_ختم
#اگر defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    ورنہ اگر (STRICMP(name, "pythonx") == 0)
	    {
# اگر defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		اگر (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		ورنہ اگر (p_pyx == 3)
		    n = python3_enabled(FALSE);
		ورنہ اگر (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_RUBY
	    ورنہ اگر (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_TCL
	    ورنہ اگر (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_SODIUM
	    ورنہ اگر (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#اگر_ختم
#اگر defined(FEAT_TERMINAL) && defined(MSWIN)
	    ورنہ اگر (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_GPM
	    ورنہ اگر (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_دستیاب();
#اگر_ختم
	}
    }

    اگر (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_نمبر = x;
    ورنہ
	// return whether feature is enabled
	rettv->vval.v_نمبر = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    صحیح
dynamic_feature(char_u *feature)
{
    واپس (feature == تہی
#اگر defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.اندر_use)
#اگر_ختم
#اگر_تعریف_شدہ VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#اگر_ختم
#اگر defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.اندر_use)
#اگر_ختم
#اگر defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    || STRICMP(feature, "iconv") == 0
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_LUA
	    || STRICMP(feature, "lua") == 0
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_GPM
	    || (STRICMP(feature, "mouse_gpm_enabled") == 0 && !gpm_enabled())
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_MZSCHEME
	    || STRICMP(feature, "mzscheme") == 0
#اگر_ختم
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	    || STRICMP(feature, "netbeans_enabled") == 0
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_PERL
	    || STRICMP(feature, "perl") == 0
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_PYTHON
	    || STRICMP(feature, "python") == 0
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_PYTHON3
	    || STRICMP(feature, "python3") == 0
#اگر_ختم
#اگر defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    || STRICMP(feature, "pythonx") == 0
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_RUBY
	    || STRICMP(feature, "ruby") == 0
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
	    || STRICMP(feature, "syntax_items") == 0
#اگر_ختم
#اگر_تعریف_شدہ DYNAMIC_TCL
	    || STRICMP(feature, "tcl") == 0
#اگر_ختم
	    // once "starting" is zero it will stay that way
	    || (STRICMP(feature, "vim_starting") == 0 && starting != 0)
	    || STRICMP(feature, "multi_byte_encoding") == 0
#اگر defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#اگر_ختم
	    );
}

/*
 * "haslocaldir()" function
 */
    ساکن خالی
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = تہی;
    win_T	*wp = تہی;

    اگر (اندر_vim9script()
	    && (check_for_opt_نمبر_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 1) == FAIL)))
	واپس;

    wp = ڈھونڈو_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    اگر (wp != تہی && wp->w_localdir != تہی)
	rettv->vval.v_نمبر = 1;
    ورنہ اگر (tp != تہی && tp->tp_localdir != تہی)
	rettv->vval.v_نمبر = 2;
    ورنہ
	rettv->vval.v_نمبر = 0;
}

/*
 * "highlightID(name)" function
 */
    ساکن خالی
f_hlID(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    ساکن خالی
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    ساکن خالی
f_hostname(typval_T *argvars غیر_مستعمل, typval_T *rettv)
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
#اگر_تعریف_شدہ VMS // VMS does not have defined uintptr_t
# اگر defined(HAVE_NO_LONG_LONG)
قسمتعریف غیرعلامتی صحیح uintptr_t;
# ورنہ
قسمتعریف غیرعلامتی طویل طویل uintptr_t;
# endif
#اگر_ختم // VMS

    ساکن خالی
f_id(typval_T *argvars, typval_T *rettv)
{
    حرف    numbuf[NUMBUFLEN];
    حرف    *p = numbuf;

    انتخاب (argvars[0].v_type)
    {
	حالت VAR_LIST:
	حالت VAR_TUPLE:
	حالت VAR_DICT:
	حالت VAR_OBJECT:
	حالت VAR_کام:
	حالت VAR_CHANNEL:
	حالت VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    اگر (argvars[0].vval.v_object != تہی)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(خالی *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		برائے (; p < numbuf + حجم(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	طےشدہ:
	    توڑو;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    ساکن خالی
اشاریہ_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    صحیح		آغاز = 0;
    صحیح		error = FALSE;
    صحیح		ic = FALSE;

    b = argvars[0].vval.v_blob;
    اگر (b == تہی)
	واپس;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	آغاز = tv_get_نمبر_chk(&argvars[2], &error);
	اگر (error)
	    واپس;
    }

    اگر (آغاز < 0)
    {
	آغاز = blob_len(b) + آغاز;
	اگر (آغاز < 0)
	    آغاز = 0;
    }

    برائے (صحیح idx = آغاز; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_نمبر;
	tv.vval.v_نمبر = blob_get(b, idx);
	اگر (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_نمبر = idx;
	    واپس;
	}
    }
}

/*
 * index() function for a tuple
 */
    ساکن خالی
اشاریہ_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    صحیح		ic = FALSE;
    صحیح		error = FALSE;

    اگر (tuple == تہی)
	واپس;

    صحیح	آغاز_idx = 0;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	آغاز_idx = tv_get_نمبر_chk(&argvars[2], &error);
	اگر (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (صحیح)tv_get_bool_chk(&argvars[3], &error);
	اگر (error)
	    واپس;
    }

    rettv->vval.v_نمبر = اشاریہ_tuple(tuple, &argvars[1], آغاز_idx, ic);
}

/*
 * index() function for a list
 */
    ساکن خالی
اشاریہ_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*شے;
    طویل	idx = 0;
    صحیح		ic = FALSE;
    صحیح		error = FALSE;

    l = argvars[0].vval.v_list;
    اگر (l == تہی)
	واپس;

    CHECK_LIST_MATERIALIZE(l);
    شے = l->lv_پہلا;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	شے = list_ڈھونڈو(l, (طویل)tv_get_نمبر_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	اگر (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (صحیح)tv_get_bool_chk(&argvars[3], &error);
	اگر (error)
	    شے = تہی;
    }

    برائے ( ; شے != تہی; شے = شے->li_اگلا, ++idx)
	اگر (tv_equal(&شے->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_نمبر = idx;
	    توڑو;
	}
}

/*
 * "index()" function
 */
    ساکن خالی
f_اشاریہ(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_نمبر_arg(argvars, 1) == FAIL)
		|| check_for_opt_نمبر_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	واپس;

    اگر (argvars[0].v_type == VAR_BLOB)
	اشاریہ_func_blob(argvars, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_TUPLE)
	اشاریہ_func_tuple(argvars, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_LIST)
	اشاریہ_func_list(argvars, rettv);
    ورنہ
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    صحیح
indexof_eval_اظہاریہ(typval_T *اظہاریہ)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	ملا;
    صحیح		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_کلید);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    اگر (eval_اظہاریہ_typval(اظہاریہ, FALSE, argv, 2, تہی, &newtv) == FAIL)
	واپس FALSE;

    ملا = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    واپس error ? FALSE : ملا;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    ساکن صحیح
indexof_blob(blob_T *b, طویل startidx, typval_T *اظہاریہ)
{
    طویل	idx = 0;

    اگر (b == تہی)
	واپس -1;

    اگر (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	اگر (startidx < 0)
	    startidx = 0;
    }

    سیٹ_vim_var_type(VV_کلید, VAR_نمبر);
    سیٹ_vim_var_type(VV_VAL, VAR_نمبر);

    صحیح		called_emsg_آغاز = called_emsg;
    برائے (idx = startidx; idx < blob_len(b); ++idx)
    {
	سیٹ_vim_var_nr(VV_کلید, idx);
	سیٹ_vim_var_nr(VV_VAL, blob_get(b, idx));

	اگر (indexof_eval_اظہاریہ(اظہاریہ))
	    واپس idx;

	اگر (called_emsg != called_emsg_آغاز)
	    واپس -1;
    }

    واپس -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ساکن صحیح
indexof_list(list_T *l, طویل startidx, typval_T *اظہاریہ)
{
    listitem_T	*شے;
    طویل	idx = 0;
    صحیح		ملا;

    اگر (l == تہی)
	واپس -1;

    CHECK_LIST_MATERIALIZE(l);

    اگر (startidx == 0)
	شے = l->lv_پہلا;
    ورنہ
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	شے = list_ڈھونڈو(l, startidx);
	اگر (شے != تہی)
	    idx = l->lv_u.mat.lv_idx;
    }

    سیٹ_vim_var_type(VV_کلید, VAR_نمبر);

    صحیح		called_emsg_آغاز = called_emsg;
    برائے ( ; شے != تہی; شے = شے->li_اگلا, ++idx)
    {
	سیٹ_vim_var_nr(VV_کلید, idx);
	copy_tv(&شے->li_tv, get_vim_var_tv(VV_VAL));

	ملا = indexof_eval_اظہاریہ(اظہاریہ);
	clear_tv(get_vim_var_tv(VV_VAL));

	اگر (ملا)
	    واپس idx;

	اگر (called_emsg != called_emsg_آغاز)
	    واپس -1;
    }

    واپس -1;
}

/*
 * "indexof()" function
 */
    ساکن خالی
f_indexof(typval_T *argvars, typval_T *rettv)
{
    طویل	startidx = 0;
    typval_T	save_val;
    typval_T	save_کلید;
    صحیح		save_did_emsg;

    rettv->vval.v_نمبر = -1;

    اگر (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	واپس;

    اگر ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == تہی
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == تہی))
	واپس;

    اگر (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_نمبر_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_کلید, &save_کلید);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    اگر (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_نمبر = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    ورنہ اگر (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_نمبر = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    ورنہ
	rettv->vval.v_نمبر = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_کلید, &save_کلید);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

ساکن صحیح inputsecret_flag = 0;

/*
 * "input()" function
 *     Also handles inputsecret() when inputsecret is set.
 */
    ساکن خالی
f_داخلہ(typval_T *argvars, typval_T *rettv)
{
    get_user_داخلہ(argvars, rettv, FALSE, inputsecret_flag);
}

/*
 * "inputdialog()" function
 */
    ساکن خالی
f_inputdialog(typval_T *argvars, typval_T *rettv)
{
#اگر defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    اگر (gui.اندر_use && vim_strchr(p_go, GO_CONDIALOG) == تہی)
    {
	char_u	*پیغام_2;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	اگر (اندر_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    واپس;

	پیغام_2 = tv_get_string_chk(&argvars[0]);
	اگر (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != تہی)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	ورنہ
	    IObuff[0] = NUL;
	اگر (پیغام_2 != تہی && defstr != تہی
		&& do_dialog(VIM_QUESTION, تہی, پیغام_2,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	ورنہ
	{
	    اگر (پیغام_2 != تہی && defstr != تہی
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    ورنہ
		rettv->vval.v_string = تہی;
	}
	rettv->v_type = VAR_STRING;
    }
    ورنہ
#اگر_ختم
	get_user_داخلہ(argvars, rettv, TRUE, inputsecret_flag);
}

/*
 * "inputlist()" function
 */
    ساکن خالی
f_inputlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    صحیح		selected;
    صحیح		mouse_مستعمل;

#اگر_تعریف_شدہ NO_CONSOLE_داخلہ
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    اگر (no_console_داخلہ() && !is_not_a_رکن())
	واپس;
#اگر_ختم
    اگر (اندر_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == تہی)
    {
	semsg(_(e_دلیل_کا_str_must_be_list), "inputlist()");
	واپس;
    }

    پیغام_آغاز();
    پیغام_صف = صفیں - 1;	// for when 'cmdheight' > 1
    سطریں_بایاں = صفیں;	// avoid more prompt
    پیغام_scroll = TRUE;
    پیغام_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_اشیاء(l, li)
    {
	پیغام_puts((حرف *)tv_get_string(&li->li_tv));
	پیغام_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_نمبر(&mouse_مستعمل);
    اگر (mouse_مستعمل)
	selected -= سطریں_بایاں;

    rettv->vval.v_نمبر = selected;
}

ساکن garray_T	    ga_userinput = {0, 0, حجم(tasave_T), 4, تہی};

/*
 * "inputrestore()" function
 */
    ساکن خالی
f_inputrestore(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    اگر (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_ڈیٹا)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    ورنہ اگر (p_verbose > 1)
    {
	verb_پیغام(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_نمبر = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    ساکن خالی
f_inputsave(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    اگر (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_ڈیٹا)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    ورنہ
	rettv->vval.v_نمبر = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    ساکن خالی
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	واپس;

    ++cmdline_ستارہ;
    ++inputsecret_flag;
    f_داخلہ(argvars, rettv);
    --cmdline_ستارہ;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    ساکن خالی
f_مداخلت(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    ساکن خالی
f_invert(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = ~tv_get_نمبر_chk(&argvars[0], تہی);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    ساکن خالی
free_lval_جڑ(lval_جڑ_T *جڑ)
{
    اگر (جڑ->lr_tv != تہی)
	free_tv(جڑ->lr_tv);
    class_unref(جڑ->lr_cl_exec);
    جڑ->lr_tv = تہی;
    جڑ->lr_cl_exec = تہی;
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

    ساکن صحیح
fix_variable_reference_lval_جڑ(lval_جڑ_T *جڑ, char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    صحیح ملا_member = FALSE;
    اگر (جڑ->lr_tv->v_type == VAR_OBJECT)
    {
	اگر (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    ملا_member = TRUE;
    }
    اگر (!ملا_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*انتہا;
	برائے (انتہا = name; ASCII_ISALNUM(*انتہا) || *انتہا == '_'; ++انتہا)
	    ;
	صحیح idx = class_member_idx(جڑ->lr_cl_exec, name, انتہا - name);
	اگر (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(جڑ->lr_tv);
	    copy_tv(&جڑ->lr_cl_exec->class_members_tv[idx], جڑ->lr_tv);
	    ملا_member = TRUE;
	}
    }
    اگر (!ملا_member)
    {
	free_tv(جڑ->lr_tv);
	جڑ->lr_tv = تہی;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    واپس OK;
}

/*
 * "islocked()" function
 */
    ساکن خالی
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*انتہا;
    dictitem_T	*di;

    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    char_u *name = tv_get_string(&argvars[0]);
#اگر_تعریف_شدہ LOG_LOCKVAR
    ch_log(تہی, "LKVAR: f_islocked(): name: %s", name);
#اگر_ختم

    lval_جڑ_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_جڑ_T *جڑ = تہی;

    // Set up lval_root if executing in a method.
    اگر (fill_exec_lval_جڑ(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	اگر (fix_variable_reference_lval_جڑ(&aroot, name) == OK)
	    جڑ = &aroot;
    }

    lval_جڑ_T	*lval_جڑ_save = lval_جڑ;
    lval_جڑ = جڑ;
    انتہا = get_lval(name, تہی, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_آغاز);
    lval_جڑ = lval_جڑ_save;

    اگر (انتہا != تہی && lv.ll_name != تہی)
    {
	اگر (*انتہا != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_انتہا
		   ? e_invalid_دلیل_str : e_trailing_characters_str), انتہا);
	}
	ورنہ
	{
	    اگر (lv.ll_tv == تہی)
	    {
		di = ڈھونڈو_var(lv.ll_name, تہی, TRUE);
		اگر (di != تہی)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_نمبر = ((di->di_flags & DI_FLAGS_تالا)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    ورنہ اگر (lv.ll_is_جڑ)
	    {
		rettv->vval.v_نمبر = tv_islocked(lv.ll_tv);
	    }
	    ورنہ اگر (lv.ll_object != تہی)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_نمبر = tv_islocked(tv);
#اگر_تعریف_شدہ LOG_LOCKVAR
		ch_log(تہی, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#اگر_ختم
	    }
	    ورنہ اگر (lv.ll_class != تہی)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_نمبر = tv_islocked(tv);
#اگر_تعریف_شدہ LOG_LOCKVAR
		ch_log(تہی, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#اگر_ختم
	    }
	    ورنہ اگر (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    ورنہ اگر (lv.ll_newkey != تہی)
		semsg(_(e_کلید_not_present_اندر_dictionary_str), lv.ll_newkey);
	    ورنہ اگر (lv.ll_list != تہی)
		// List item.
		rettv->vval.v_نمبر = tv_islocked(&lv.ll_li->li_tv);
	    ورنہ اگر (lv.ll_tuple != تہی)
		// Tuple item.
		rettv->vval.v_نمبر = tv_islocked(lv.ll_tv);
	    ورنہ
		// Dictionary item.
		rettv->vval.v_نمبر = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    اگر (جڑ != تہی)
	free_lval_جڑ(جڑ);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    ساکن خالی
f_اشیاء(typval_T *argvars, typval_T *rettv)
{
    اگر (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    ورنہ
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_دلیل_nr), 1);
}

/*
 * "keytrans()" function
 */
    ساکن خالی
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    اگر (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == تہی)
	واپس;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    ساکن خالی
f_آخری_بفر_nr(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    صحیح		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	اگر (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_نمبر = n;
}

/*
 * "len()" function
 */
    خالی
f_len(typval_T *argvars, typval_T *rettv)
{
    انتخاب (argvars[0].v_type)
    {
	حالت VAR_STRING:
	حالت VAR_نمبر:
	    rettv->vval.v_نمبر = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    توڑو;
	حالت VAR_BLOB:
	    rettv->vval.v_نمبر = blob_len(argvars[0].vval.v_blob);
	    توڑو;
	حالت VAR_LIST:
	    rettv->vval.v_نمبر = list_len(argvars[0].vval.v_list);
	    توڑو;
	حالت VAR_TUPLE:
	    rettv->vval.v_نمبر = tuple_len(argvars[0].vval.v_tuple);
	    توڑو;
	حالت VAR_DICT:
	    rettv->vval.v_نمبر = dict_len(argvars[0].vval.v_dict);
	    توڑو;
	حالت VAR_OBJECT:
	    rettv->vval.v_نمبر = object_len(argvars[0].vval.v_object);
	    توڑو;
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	حالت VAR_FLOAT:
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	حالت VAR_کام:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_CLASS:
	حالت VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    توڑو;
    }
}

    ساکن خالی
libcall_common(typval_T *argvars غیر_مستعمل, typval_T *rettv, صحیح type)
{
#اگر_تعریف_شدہ FEAT_LIBCALL
    char_u		*string_اندر;
    char_u		**string_نتیجہ;
    صحیح			nr_نتیجہ;
#اگر_ختم

    rettv->v_type = type;
    اگر (type != VAR_نمبر)
	rettv->vval.v_string = تہی;

    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_نمبر_arg(argvars, 2) == FAIL))
	واپس;

#اگر_تعریف_شدہ FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    اگر (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_اندر = تہی;
	اگر (argvars[2].v_type == VAR_STRING)
	    string_اندر = argvars[2].vval.v_string;
	اگر (type == VAR_نمبر)
	{
	    string_نتیجہ = تہی;
	}
	ورنہ
	{
	    rettv->vval.v_string = تہی;
	    string_نتیجہ = &rettv->vval.v_string;
	}
	اگر (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_اندر,
			     argvars[2].vval.v_نمبر,
			     string_نتیجہ,
			     &nr_نتیجہ) == OK
		&& type == VAR_نمبر)
	    rettv->vval.v_نمبر = nr_نتیجہ;
    }
#اگر_ختم
}

/*
 * "libcall()" function
 */
    ساکن خالی
f_libcall(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_STRING);
}

/*
 * "libcallnr()" function
 */
    ساکن خالی
f_libcallnr(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_نمبر);
}

/*
 * "line(string, [winid])" function
 */
    ساکن خالی
f_سطر(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum = 0;
    pos_T	*fp = تہی;
    صحیح		fnum;
    صحیح		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (صحیح)tv_get_نمبر(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	اگر (wp != تہی && tp != تہی)
	{
	    اگر (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		اگر (*p_spk != 'c'
#اگر_تعریف_شدہ FEAT_DIFF
		|| (curwin->w_p_diff && switchwin.sw_curwin->w_p_diff)
#اگر_ختم
		)
		    skip_update_topline = TRUE;
		check_cursor();
		fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	    }
	    skip_update_topline = FALSE;
	    restore_win_noblock(&switchwin, TRUE);
	}
    }
    ورنہ
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    اگر (fp != تہی)
	lnum = fp->lnum;
    rettv->vval.v_نمبر = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    ساکن خالی
f_line2byte(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
#اگر_غیر_تعریف_شدہ FEAT_BYTEOFF
    rettv->vval.v_نمبر = -1;
#ورنہ
    linenr_T	lnum;

    اگر (اندر_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	واپس;

    lnum = tv_get_lnum(argvars);
    اگر (lnum < 1 || lnum > curbuf->b_ml.ml_سطر_گنتی + 1)
	rettv->vval.v_نمبر = -1;
    ورنہ
	rettv->vval.v_نمبر = ml_ڈھونڈو_سطر_or_انحراف(curbuf, lnum, تہی);
    اگر (rettv->vval.v_نمبر >= 0)
	++rettv->vval.v_نمبر;
#اگر_ختم
}

#اگر_تعریف_شدہ FEAT_LUA
/*
 * "luaeval()" function
 */
    ساکن خالی
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#اگر_ختم

قسمتعریف شمار
{
    مطابقت_انتہا_2,	    // matchend()
    مطابقت_مطابقت,    // match()
    مطابقت_STR,	    // matchstr()
    مطابقت_LIST,	    // matchlist()
    مطابقت_POS	    // matchstrpos()
} matchtype_T;

    ساکن خالی
ڈھونڈو_some_مطابقت(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = تہی;
    طویل	len = 0;
    char_u	*اظہاریہ = تہی;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    طویل	آغاز = 0;
    طویل	nth = 1;
    colnr_T	startcol = 0;
    صحیح		مطابقت = 0;
    list_T	*l = تہی;
    listitem_T	*li = تہی;
    طویل	idx = 0;
    صحیح		پچھلا_تالا = 0;
    char_u	*tofree = تہی;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = خالی_خانے_option;

    rettv->vval.v_نمبر = -1;
    اگر (type == مطابقت_LIST || type == مطابقت_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	اگر (rettv_list_alloc(rettv) == FAIL)
	    جاؤ theend;
	اگر (type == مطابقت_POS
		&& (list_append_string(rettv->vval.v_list,
					    (char_u *)"", 0) == FAIL
		    || list_append_نمبر(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_نمبر(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_نمبر(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL))
	{
		list_free(rettv->vval.v_list);
		rettv->vval.v_list = تہی;
		جاؤ theend;
	}
    }
    ورنہ اگر (type == مطابقت_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = تہی;
    }

    اگر (اندر_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 3) == FAIL)))
	جاؤ theend;

    اگر (argvars[0].v_type == VAR_LIST)
    {
	اگر ((l = argvars[0].vval.v_list) == تہی)
	    جاؤ theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_پہلا;
    }
    ورنہ
    {
	اظہاریہ = str = tv_get_string(&argvars[0]);
	len = (طویل)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    اگر (pat == تہی)
	جاؤ theend;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	صحیح	    error = FALSE;

	آغاز = (طویل)tv_get_نمبر_chk(&argvars[2], &error);
	اگر (error)
	    جاؤ theend;
	اگر (l != تہی)
	{
	    li = list_ڈھونڈو(l, آغاز);
	    اگر (li == تہی)
		جاؤ theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	ورنہ
	{
	    اگر (آغاز < 0)
		آغاز = 0;
	    اگر (آغاز > len)
		جاؤ theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    اگر (argvars[3].v_type != VAR_UNKNOWN)
		startcol = آغاز;
	    ورنہ
	    {
		str += آغاز;
		len -= آغاز;
	    }
	}

	اگر (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (طویل)tv_get_نمبر_chk(&argvars[3], &error);
	اگر (error)
	    جاؤ theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    اگر (regmatch.regprog != تہی)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	اگر (l != تہی)
	{
	    پچھلا_تالا = l->lv_تالا;
	    اگر (l->lv_تالا == 0)
		l->lv_تالا = VAR_LOCKED;
	}

	برائے (;;)
	{
	    اگر (l != تہی)
	    {
		اگر (li == تہی)
		{
		    مطابقت = FALSE;
		    توڑو;
		}
		vim_free(tofree);
		اظہاریہ = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		اگر (str == تہی)
		    توڑو;
	    }

	    مطابقت = vim_regexec_nl(&regmatch, str, startcol);

	    اگر (مطابقت && --nth <= 0)
		توڑو;
	    اگر (l == تہی && !مطابقت)
		توڑو;

	    // Advance to just after the match.
	    اگر (l != تہی)
	    {
		li = li->li_اگلا;
		++idx;
	    }
	    ورنہ
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		اگر (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    مطابقت = FALSE;
		    توڑو;
		}
	    }
	}

	اگر (مطابقت)
	{
	    اگر (type == مطابقت_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_پہلا;
		listitem_T *li2 = li1->li_اگلا;
		listitem_T *li3 = li2->li_اگلا;
		listitem_T *li4 = li3->li_اگلا;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_نمبر =
				      (varnumber_T)(regmatch.startp[0] - اظہاریہ);
		li4->li_tv.vval.v_نمبر =
					(varnumber_T)(regmatch.endp[0] - اظہاریہ);
		اگر (l != تہی)
		    li2->li_tv.vval.v_نمبر = (varnumber_T)idx;
	    }
	    ورنہ اگر (type == مطابقت_LIST)
	    {
		صحیح i;

		// return list with matched string and submatches
		برائے (i = 0; i < NSUBEXP; ++i)
		{
		    اگر (regmatch.endp[i] == تہی)
		    {
			اگر (list_append_string(rettv->vval.v_list,
							    تہی, 0) == FAIL)
			    توڑو;
		    }
		    ورنہ اگر (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(صحیح)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			توڑو;
		}
	    }
	    ورنہ اگر (type == مطابقت_STR)
	    {
		// return matched string
		اگر (l != تہی)
		    copy_tv(&li->li_tv, rettv);
		ورنہ
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    ورنہ اگر (l != تہی)
		rettv->vval.v_نمبر = idx;
	    ورنہ
	    {
		اگر (type != مطابقت_انتہا_2)
		    rettv->vval.v_نمبر =
				      (varnumber_T)(regmatch.startp[0] - str);
		ورنہ
		    rettv->vval.v_نمبر =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_نمبر += (varnumber_T)(str - اظہاریہ);
	    }
	}
	اگر (l != تہی)
	    l->lv_تالا = پچھلا_تالا;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    اگر (type == مطابقت_POS && l == تہی && rettv->vval.v_list != تہی)
	// matchstrpos() without a list: drop the second item.
	listitem_remove(rettv->vval.v_list,
				       rettv->vval.v_list->lv_پہلا->li_اگلا);
    vim_free(tofree);
    p_cpo = save_cpo;
}

/*
 * Return all the matches in string "str" for pattern "rmp".
 * The matches are returned in the List "mlist".
 * If "submatches" is TRUE, then submatch information is also returned.
 * "matchbuf" is TRUE when called for matchbufline().
 */
    ساکن صحیح
get_matches_اندر_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    صحیح		idx,
    صحیح		submatches,
    صحیح		matchbuf)
{
    طویل	len = (طویل)STRLEN(str);
    صحیح		مطابقت = 0;
    colnr_T	startidx = 0;

    برائے (;;)
    {
	مطابقت = vim_regexec_nl(rmp, str, startidx);
	اگر (!مطابقت)
	    توڑو;

	dict_T *d = dict_alloc();
	اگر (d == تہی)
	    واپس FAIL;
	اگر (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    واپس FAIL;
	}

	اگر (dict_جوڑو_نمبر(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    واپس FAIL;

	اگر (dict_جوڑو_نمبر(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    واپس FAIL;

	اگر (dict_جوڑو_string_len(d, "text", rmp->startp[0],
		    (صحیح)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    واپس FAIL;

	اگر (submatches)
	{
	    list_T *sml = list_alloc();
	    اگر (sml == تہی)
		واپس FAIL;

	    اگر (dict_جوڑو_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		واپس FAIL;
	    }

	    // return a list with the submatches
	    برائے (صحیح i = 1; i < NSUBEXP; ++i)
	    {
		اگر (rmp->endp[i] == تہی)
		{
		    اگر (list_append_string(sml, (char_u *)"", 0) == FAIL)
			واپس FAIL;
		}
		ورنہ اگر (list_append_string(sml, rmp->startp[i],
			    (صحیح)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    واپس FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	اگر (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    توڑو;
    }

    واپس OK;
}

/*
 * "matchbufline()" function
 */
    ساکن خالی
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = تہی;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_نمبر = -1;
    اگر (rettv_list_alloc(rettv) != OK)
	واپس;
    retlist = rettv->vval.v_list;

    اگر (check_for_بفر_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	واپس;

    صحیح پچھلا_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    اگر (buf == تہی)
    {
	اگر (did_emsg == پچھلا_did_emsg)
	    semsg(_(e_invalid_بفر_name_str), tv_get_string(&argvars[0]));
	واپس;
    }
    اگر (buf->b_ml.ml_mfp == تہی)
    {
	emsg(_(e_بفر_is_not_loaded));
	واپس;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    صحیح		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    اگر (did_emsg > did_emsg_before)
	واپس;
    اگر (slnum < 1)
    {
	semsg(_(e_invalid_قدر_for_دلیل_str), "lnum");
	واپس;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    اگر (did_emsg > did_emsg_before)
	واپس;
    اگر (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_قدر_for_دلیل_str), "end_lnum");
	واپس;
    }

    اگر (elnum > buf->b_ml.ml_سطر_گنتی)
	elnum = buf->b_ml.ml_سطر_گنتی;

    صحیح		submatches = FALSE;
    اگر (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	اگر (d != تہی)
	{
	    dictitem_T *di = dict_ڈھونڈو(d, (char_u *)"submatches", -1);
	    اگر (di != تہی)
	    {
		اگر (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "submatches");
		    واپس;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = خالی_خانے_option;

    regmatch.regprog = eval_regcomp(pat);
    اگر (regmatch.regprog == تہی)
	جاؤ theend;
    regmatch.rm_ic = p_ic;

    جب (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	اگر (get_matches_اندر_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    جاؤ cleanup;
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
    ساکن خالی
f_مطابقت(typval_T *argvars, typval_T *rettv)
{
    ڈھونڈو_some_مطابقت(argvars, rettv, مطابقت_مطابقت);
}

/*
 * "matchend()" function
 */
    ساکن خالی
f_matchend(typval_T *argvars, typval_T *rettv)
{
    ڈھونڈو_some_مطابقت(argvars, rettv, مطابقت_انتہا_2);
}

/*
 * "matchlist()" function
 */
    ساکن خالی
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    ڈھونڈو_some_مطابقت(argvars, rettv, مطابقت_LIST);
}

/*
 * "matchstr()" function
 */
    ساکن خالی
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    ڈھونڈو_some_مطابقت(argvars, rettv, مطابقت_STR);
}

/*
 * "matchstrlist()" function
 */
    ساکن خالی
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = تہی;
    char_u	*save_cpo;
    list_T	*l = تہی;
    listitem_T	*li = تہی;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_نمبر = -1;
    اگر (rettv_list_alloc(rettv) != OK)
	واپس;
    retlist = rettv->vval.v_list;

    اگر (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	واپس;

    اگر ((l = argvars[0].vval.v_list) == تہی)
	واپس;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    اگر (pat == تہی)
	واپس;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = خالی_خانے_option;

    regmatch.regprog = eval_regcomp(pat);
    اگر (regmatch.regprog == تہی)
	جاؤ theend;
    regmatch.rm_ic = p_ic;

    صحیح		submatches = FALSE;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	اگر (d != تہی)
	{
	    dictitem_T *di = dict_ڈھونڈو(d, (char_u *)"submatches", -1);
	    اگر (di != تہی)
	    {
		اگر (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_قدر_for_دلیل_str), "submatches");
		    جاؤ cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    صحیح idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_اشیاء(l, li)
    {
	اگر (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != تہی)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    اگر (get_matches_اندر_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		جاؤ cleanup;
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
    ساکن خالی
f_matchstrpos(typval_T *argvars, typval_T *rettv)
{
    ڈھونڈو_some_مطابقت(argvars, rettv, مطابقت_POS);
}

    ساکن خالی
max_min(typval_T *argvars, typval_T *rettv, صحیح domax)
{
    varnumber_T	n = 0;

    اگر (اندر_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = 0;

    اگر (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = تہی;

	l = argvars[0].vval.v_list;
	اگر (l != تہی && l->lv_len > 0)
	{
	    اگر (l->lv_پہلا == &range_list_شے)
	    {
		اگر ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_آغاز;
		ورنہ
		    n = l->lv_u.nonmat.lv_آغاز + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_نمبر = n;
	    }
	    ورنہ
	    {
		FOR_ALL_LIST_اشیاء(l, li)
		{
		    اگر (tv == تہی)
			tv = &li->li_tv;
		    ورنہ
		    {
			صحیح res;
			اگر (typval_compare2(&li->li_tv, tv,
				domax ? اظہاریہ_GREATER : اظہاریہ_SMALLER, FALSE, &res) == FAIL)
			    واپس;
			اگر (res == OK)
			    tv = &li->li_tv;
		    }
		}

		اگر (tv != تہی)
		    copy_tv(tv, rettv);
	    }
	}
    }
    ورنہ اگر (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	اگر (tv != تہی)
	    copy_tv(tv, rettv);
    }
    ورنہ اگر (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	صحیح		todo;
	typval_T	*tv = تہی;

	d = argvars[0].vval.v_dict;
	اگر (d != تہی)
	{
	    todo = (صحیح)d->dv_hashtab.ht_مستعمل;
	    FOR_ALL_HASHTAB_اشیاء(&d->dv_hashtab, hi, todo)
	    {
		اگر (!HASHITEM_خالی_خانے(hi))
		{
		    --todo;
		    اگر (tv == تہی)
			tv = &HI2DI(hi)->di_tv;
		    ورنہ
		    {
			صحیح res;
			اگر (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? اظہاریہ_GREATER : اظہاریہ_SMALLER, FALSE, &res) == FAIL)
			    واپس;
			اگر (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	اگر (tv != تہی)
	    copy_tv(tv, rettv);
    }
    ورنہ
	semsg(_(e_دلیل_کا_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
}

/*
 * "max()" function
 */
    ساکن خالی
f_max(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, TRUE);
}

/*
 * "min()" function
 */
    ساکن خالی
f_min(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, FALSE);
}

#اگر defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    ساکن خالی
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    خالی
mzscheme_call_vim(char_u *name, typval_T *دلائل, typval_T *rettv)
{
    typval_T argvars[3];

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = name;
    copy_tv(دلائل, &argvars[1]);
    argvars[2].v_type = VAR_UNKNOWN;
    f_call(argvars, rettv);
    clear_tv(&argvars[1]);
}
#اگر_ختم

/*
 * "nextnonblank()" function
 */
    ساکن خالی
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    اگر (اندر_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	واپس;

    برائے (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	اگر (lnum < 0 || lnum > curbuf->b_ml.ml_سطر_گنتی)
	{
	    lnum = 0;
	    توڑو;
	}
	اگر (*skipwhite(ml_get(lnum)) != NUL)
	    توڑو;
    }
    rettv->vval.v_نمبر = lnum;
}


/*
 * "ngettext()" function
 */
    ساکن خالی
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    حرف *پچھلا = تہی;
#اگر_ختم

    اگر (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_نمبر_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	واپس;

    rettv->v_type = VAR_STRING;

    اگر (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != تہی &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	پچھلا = bind_textdomain_codeset((ثابت حرف *)argvars[3].vval.v_string, (حرف *)p_enc);
#اگر_ختم

#اگر defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((ثابت حرف *)argvars[3].vval.v_string, (ثابت حرف *)argvars[0].vval.v_string, (ثابت حرف *)argvars[1].vval.v_string, (صحیح)argvars[2].vval.v_نمبر));
#ورنہ
	textdomain((ثابت حرف *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ثابت حرف *)argvars[0].vval.v_string, (ثابت حرف *)argvars[1].vval.v_string, argvars[2].vval.v_نمبر));
	textdomain(VIMPACKAGE);
#اگر_ختم

#اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	اگر (پچھلا != تہی)
	    bind_textdomain_codeset((ثابت حرف *)argvars[3].vval.v_string, پچھلا);
#اگر_ختم
    }
    ورنہ
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ثابت حرف *)argvars[0].vval.v_string, (ثابت حرف *)argvars[1].vval.v_string, argvars[2].vval.v_نمبر));
}


/*
 * "nr2char()" function
 */
    ساکن خالی
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	واپس;

    اگر (has_mbyte)
    {
	صحیح	utf8 = 0;

	اگر (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (صحیح)tv_get_bool_chk(&argvars[1], تہی);
	اگر (utf8)
	    buf[utf_char2bytes((صحیح)tv_get_نمبر(&argvars[0]), buf)] = NUL;
	ورنہ
	    buf[(*mb_char2bytes)((صحیح)tv_get_نمبر(&argvars[0]), buf)] = NUL;
    }
    ورنہ
    {
	buf[0] = (char_u)tv_get_نمبر(&argvars[0]);
	buf[1] = NUL;
    }
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "or(expr, expr)" function
 */
    ساکن خالی
f_or(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    rettv->vval.v_نمبر = tv_get_نمبر_chk(&argvars[0], تہی)
					| tv_get_نمبر_chk(&argvars[1], تہی);
}

#اگر_تعریف_شدہ FEAT_PERL
/*
 * "perleval()" function
 */
    ساکن خالی
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#اگر_ختم

/*
 * "prevnonblank()" function
 */
    ساکن خالی
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    اگر (اندر_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	واپس;

    lnum = tv_get_lnum(argvars);
    اگر (lnum < 1 || lnum > curbuf->b_ml.ml_سطر_گنتی)
	lnum = 0;
    ورنہ
	جب (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
	    --lnum;
    rettv->vval.v_نمبر = lnum;
}

// This dummy va_list is here because:
// - passing a NULL pointer doesn't work when va_list isn't a pointer
// - locally in the function results in a "used before set" warning
// - using va_start() to initialize it gives "function with fixed args" error
ساکن va_list	ap;

/*
 * "printf()" function
 */
    ساکن خالی
f_printf(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    صحیح		len;
    char_u	*s;
    صحیح		saved_did_emsg = did_emsg;
    حرف	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script() && check_for_string_or_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (حرف *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(تہی, 0, fmt, ap, argvars + 1);
    اگر (!did_emsg)
    {
	s = alloc(len + 1);
	اگر (s != تہی)
	{
	    rettv->vval.v_string = s;
	    (خالی)vim_vsnprintf_typval((حرف *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    ساکن خالی
f_pum_getpos(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;
    pum_سیٹ_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    ساکن خالی
f_pumvisible(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    اگر (pum_visible())
	rettv->vval.v_نمبر = 1;
}

#اگر_تعریف_شدہ FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    ساکن خالی
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

    اگر (p_pyx == 0)
	p_pyx = 3;

    اگر (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    ورنہ اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	واپس;
    }
    ورنہ
    {
	locals = تہی;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PYTHON
/*
 * "pyeval()" function
 */
    ساکن خالی
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	واپس;

    اگر (p_pyx == 0)
	p_pyx = 2;

    اگر (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    ورنہ اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	واپس;
    }
    ورنہ
    {
	locals = تہی;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#اگر_ختم

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    ساکن خالی
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

# اگر defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    اگر (p_pyx == 2)
	f_pyeval(argvars, rettv);
    ورنہ
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#اگر_ختم

ساکن UINT32_T srand_بیج_for_testing = 0;
ساکن صحیح	srand_بیج_for_testing_is_مستعمل = FALSE;

    ساکن خالی
f_test_srand_بیج(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
	srand_بیج_for_testing_is_مستعمل = FALSE;
    ورنہ
    {
	srand_بیج_for_testing = (UINT32_T)tv_get_نمبر(&argvars[0]);
	srand_بیج_for_testing_is_مستعمل = TRUE;
    }
}

    ساکن خالی
init_srand(UINT32_T *x)
{
    ساخت {
	اتحاد {
	    UINT32_T نمبر;
	    char_u   بائٹیں[حجم(UINT32_T)];
	} contents;
    } buf;

    اگر (srand_بیج_for_testing_is_مستعمل)
    {
	*x = srand_بیج_for_testing;
	واپس;
    }

    اگر (mch_get_random(buf.contents.بائٹیں, حجم(buf.contents.بائٹیں)) == OK)
    {
	*x = buf.contents.نمبر;
	واپس;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#اگر defined(FEAT_SODIUM)
    اگر (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    ورنہ
#اگر_ختم
    {
#اگر defined(FEAT_RELTIME)
	proftime_T res;
	profile_آغاز(&res);
# اگر defined(MSWIN)
	*x = (UINT32_T)res.کم_Part;
# ورنہ
	*x = (UINT32_T)res.tv_fsec;
# endif
#ورنہ
	*x = vim_وقت();
#اگر_ختم
	*x ^= mch_get_pid();
    }
}

#تعریف_کرو ROTL(x, k) (((x) << (k)) | ((x) >> (32 - (k))))
#تعریف_کرو SPLITMIX32(x, z) ( \
    (z) = ((x) += 0x9e3779b9), \
    (z) = ((z) ^ ((z) >> 16)) * 0x85ebca6b, \
    (z) = ((z) ^ ((z) >> 13)) * 0xc2b2ae35, \
    (z) ^ ((z) >> 16) \
    )
#تعریف_کرو SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w) \
    نتیجہ = ROTL((y) * 5, 7) * 9; \
    t = (y) << 9; \
    (z) ^= (x); \
    (w) ^= (y); \
    (y) ^= (z), (x) ^= (w); \
    (z) ^= t; \
    (w) = ROTL(w, 11);

/*
 * "rand()" function
 */
    ساکن خالی
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = تہی;
    ساکن UINT32_T	gx, gy, gz, gw;
    ساکن صحیح	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, نتیجہ;

    اگر (اندر_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	واپس;

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
    ورنہ اگر (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	اگر (l == تہی || list_len(l) != 4)
	    جاؤ theend;

	lx = list_ڈھونڈو(l, 0L);
	ly = list_ڈھونڈو(l, 1L);
	lz = list_ڈھونڈو(l, 2L);
	lw = list_ڈھونڈو(l, 3L);
	اگر (lx == تہی || ly == تہی || lz == تہی || lw == تہی)
	    جاؤ theend;
	اگر (lx->li_tv.v_type != VAR_نمبر
		|| ly->li_tv.v_type != VAR_نمبر
		|| lz->li_tv.v_type != VAR_نمبر
		|| lw->li_tv.v_type != VAR_نمبر)
	    جاؤ theend;
	x = (UINT32_T)lx->li_tv.vval.v_نمبر;
	y = (UINT32_T)ly->li_tv.vval.v_نمبر;
	z = (UINT32_T)lz->li_tv.vval.v_نمبر;
	w = (UINT32_T)lw->li_tv.vval.v_نمبر;

	SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w);

	lx->li_tv.vval.v_نمبر = (varnumber_T)x;
	ly->li_tv.vval.v_نمبر = (varnumber_T)y;
	lz->li_tv.vval.v_نمبر = (varnumber_T)z;
	lw->li_tv.vval.v_نمبر = (varnumber_T)w;
    }
    ورنہ
	جاؤ theend;

    rettv->v_type = VAR_نمبر;
    rettv->vval.v_نمبر = (varnumber_T)نتیجہ;
    واپس;

theend:
    semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[0]));
    rettv->v_type = VAR_نمبر;
    rettv->vval.v_نمبر = -1;
}

/*
 * "srand()" function
 */
    ساکن خالی
f_srand(typval_T *argvars, typval_T *rettv)
{
    UINT32_T x = 0, z;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    ورنہ
    {
	صحیح	    error = FALSE;

	x = (UINT32_T)tv_get_نمبر_chk(&argvars[0], &error);
	اگر (error)
	    واپس;
    }

    list_append_نمبر(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_نمبر(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_نمبر(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_نمبر(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#تعریف_منسوخ ROTL
#تعریف_منسوخ SPLITMIX32
#تعریف_منسوخ SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    ساکن خالی
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	آغاز;
    varnumber_T	انتہا;
    varnumber_T	stride = 1;
    صحیح		error = FALSE;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 2) == FAIL)))
	واپس;

    آغاز = tv_get_نمبر_chk(&argvars[0], &error);
    اگر (argvars[1].v_type == VAR_UNKNOWN)
    {
	انتہا = آغاز - 1;
	آغاز = 0;
    }
    ورنہ
    {
	انتہا = tv_get_نمبر_chk(&argvars[1], &error);
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_نمبر_chk(&argvars[2], &error);
    }

    اگر (error)
	واپس;		// type error; errmsg already given
    اگر (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	واپس;
    }

    // The stride is stored in "lv_stride", which is an int.
    اگر (stride < INT_MIN || stride > زیادہ_سے_زیادہ_عدد)
    {
	حرف	buf[NUMBUFLEN];

	vim_snprintf(buf, حجم(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	واپس;
    }

    uvarnumber_T	len;

    اگر (stride > 0 ? انتہا < آغاز : انتہا > آغاز)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	عقب = stride > 0
				  ? (uvarnumber_T)آغاز - (uvarnumber_T)انتہا
				  : (uvarnumber_T)انتہا - (uvarnumber_T)آغاز;

	اگر (عقب > 1)
	{
	    emsg(_(e_آغاز_past_انتہا));
	    واپس;
	}
	len = 0;
    }
    ورنہ
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	وسعت = stride > 0
				  ? (uvarnumber_T)انتہا - (uvarnumber_T)آغاز
				  : (uvarnumber_T)آغاز - (uvarnumber_T)انتہا;
	uvarnumber_T	گنتی = وسعت / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	اگر (گنتی >= (uvarnumber_T)زیادہ_سے_زیادہ_عدد)
	{
	    حرف	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, حجم(buf), "%llu",
			      گنتی < UVARNUM_MAX ? گنتی + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    واپس;
	}
	len = گنتی + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_پہلا = &range_list_شے;
    list->lv_u.nonmat.lv_آغاز = آغاز;
    list->lv_u.nonmat.lv_انتہا = انتہا;
    list->lv_u.nonmat.lv_stride = (صحیح)stride;
    list->lv_len = (صحیح)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    خالی
range_list_materialize(list_T *list)
{
    varnumber_T آغاز = list->lv_u.nonmat.lv_آغاز;
    varnumber_T انتہا = list->lv_u.nonmat.lv_انتہا;
    صحیح		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_پہلا = تہی;
    list->lv_u.mat.lv_آخری = تہی;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_شے = تہی;
    برائے (i = آغاز; stride > 0 ? i <= انتہا : i >= انتہا; i += stride)
    {
	اگر (list_append_نمبر(list, i) == FAIL)
	    توڑو;
	اگر (list->lv_تالا & VAR_اشیاء_LOCKED)
	    list->lv_u.mat.lv_آخری->li_tv.v_تالا = VAR_LOCKED;
    }
    list->lv_تالا &= ~VAR_اشیاء_LOCKED;
}

/*
 * "getreginfo()" function
 */
    ساکن خالی
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    صحیح		regname;
    char_u	buf[NUMBUFLEN + 2];
    size_t	buflen;
    طویل	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    اگر (اندر_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	واپس;

    regname = getreg_get_regname(argvars);
    اگر (regname == 0)
	واپس;

    اگر (regname == '@')
	regname = '"';

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_اظہاریہ_SRC | GREG_LIST);
    اگر (list == تہی)
	واپس;
    اگر (dict_جوڑو_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    انتخاب (get_reg_type(regname, &reglen))
    {
	حالت MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    توڑو;
	حالت MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    توڑو;
	حالت MBLOCK:
	    buflen = vim_snprintf_safelen((حرف *)buf, حجم(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    توڑو;
	طےشدہ:
	    buf[0] = NUL;
	    buflen = 0;
	    توڑو;
    }
    (خالی)dict_جوڑو_string_len(dict, (حرف *)"regtype", buf, (صحیح)buflen);

    buf[0] = get_رجسٹر_name(get_unname_رجسٹر());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    اگر (regname == '"')
	(خالی)dict_جوڑو_string_len(dict, (حرف *)"points_to", buf, (صحیح)buflen);
    ورنہ
    {
	dictitem_T	*شے = dictitem_alloc((char_u *)"isunnamed");

	اگر (شے != تہی)
	{
	    شے->di_tv.v_type = VAR_BOOL;
	    شے->di_tv.vval.v_نمبر = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (خالی)dict_جوڑو(dict, شے);
	}
    }
}

    ساکن خالی
return_رجسٹر(صحیح regname, typval_T *rettv)
{
    char_u buf[2] = {NUL, NUL};

    buf[0] = (char_u)regname;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(buf, (buf[0] == NUL) ? 0 : 1);
}

/*
 * "reg_executing()" function
 */
    ساکن خالی
f_reg_executing(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    return_رجسٹر(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    ساکن خالی
f_reg_recording(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    return_رجسٹر(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    ساکن خالی
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_نمبر = -1;
    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    rettv->vval.v_نمبر = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    ساکن خالی
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    صحیح slen = list_len(l);

    اگر (rettv_list_alloc(rettv) == FAIL
	    || l == تہی
	    || n <= 0
	    || slen == 0)
	واپس;

    اگر (check_repeat_گنتی(slen, n) == FAIL)
	واپس;

    جب (n-- > 0)
	اگر (list_extend(rettv->vval.v_list, l, تہی) == FAIL)
	    توڑو;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    ساکن خالی
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    صحیح		slen;
    صحیح		len;
    صحیح		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    اگر (rettv_blob_alloc(rettv) == FAIL
	    || blob == تہی
	    || n <= 0)
	واپس;

    slen = blob->bv_ga.ga_len;
    اگر (slen <= 0)
	واپس;
    اگر (check_repeat_گنتی(slen, n) == FAIL)
	واپس;
    len = slen * (صحیح)n;

    اگر (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	واپس;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    برائے (i = 0; i < slen; ++i)
	اگر (blob_get(blob, i) != 0)
	    توڑو;

    اگر (i == slen)
	// No need to copy since all bytes are already zero
	واپس;

    برائے (i = 0; i < (صحیح)n; ++i)
	blob_سیٹ_range(rettv->vval.v_blob,
		(طویل)i * slen, ((طویل)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    ساکن خالی
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    size_t	slen;
    صحیح		len;
    char_u	*r;
    صحیح		مکمل;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    slen = STRLEN(p);
    اگر (slen == 0 || n <= 0)
	واپس;
    اگر (check_repeat_گنتی((varnumber_T)slen, n) == FAIL)
	واپس;
    len = (صحیح)slen * (صحیح)n;

    r = alloc((size_t)len + 1);
    اگر (r == تہی)
	واپس;

    mch_memmove(r, p, slen);
    مکمل = (صحیح)slen;
    جب (مکمل < len)
    {
	صحیح copy_len = مکمل;

	اگر (copy_len > len - مکمل)
	    copy_len = len - مکمل;
	mch_memmove(r + مکمل, r, (size_t)copy_len);
	مکمل += copy_len;
    }
    r[len] = NUL;

    rettv->vval.v_string = r;
}

/*
 * "repeat()" function
 */
    ساکن خالی
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    اگر (اندر_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    n = tv_get_نمبر(&argvars[1]);
    اگر (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    ورنہ
	repeat_string(&argvars[0], n, rettv);
}

#تعریف_کرو SP_NOMOVE	0x01	    // don't move cursor
#تعریف_کرو SP_REPEAT	0x02	    // repeat to find outer pair
#تعریف_کرو SP_RETCOUNT	0x04	    // return matchcount
#تعریف_کرو SP_SETPCMARK	0x08	    // set previous context mark
#تعریف_کرو SP_آغاز	0x10	    // accept match at start position
#تعریف_کرو SP_SUBPAT	0x20	    // return nr of matching sub-pattern
#تعریف_کرو SP_انتہا		0x40	    // leave cursor at end of match
#تعریف_کرو SP_عمود	0x80	    // start at cursor column

/*
 * Get flags for a search function.
 * Possibly sets "p_ws".
 * Returns BACKWARD, FORWARD or zero (for an error).
 */
    ساکن صحیح
get_تلاش_arg(typval_T *varp, صحیح *flagsp)
{
    صحیح		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    صحیح		نقاب;

    اگر (varp->v_type == VAR_UNKNOWN)
	واپس FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    اگر (flags == تہی)
	واپس 0;		// type error; errmsg already given
    جب (*flags != NUL)
    {
	انتخاب (*flags)
	{
	    حالت 'b': dir = BACKWARD; توڑو;
	    حالت 'w': p_ws = TRUE; توڑو;
	    حالت 'W': p_ws = FALSE; توڑو;
	    طےشدہ:  نقاب = 0;
		      اگر (flagsp != تہی)
			  انتخاب (*flags)
			  {
			      حالت 'c': نقاب = SP_آغاز; توڑو;
			      حالت 'e': نقاب = SP_انتہا; توڑو;
			      حالت 'm': نقاب = SP_RETCOUNT; توڑو;
			      حالت 'n': نقاب = SP_NOMOVE; توڑو;
			      حالت 'p': نقاب = SP_SUBPAT; توڑو;
			      حالت 'r': نقاب = SP_REPEAT; توڑو;
			      حالت 's': نقاب = SP_SETPCMARK; توڑو;
			      حالت 'z': نقاب = SP_عمود; توڑو;
			  }
		      اگر (نقاب == 0)
		      {
			  semsg(_(e_invalid_دلیل_str), flags);
			  dir = 0;
		      }
		      ورنہ
			  *flagsp |= نقاب;
	}
	اگر (dir == 0)
	    توڑو;
	++flags;
    }
    واپس dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    ساکن صحیح
تلاش_cmn(typval_T *argvars, pos_T *مطابقت_pos, صحیح *flagsp)
{
    صحیح		flags;
    char_u	*pat;
    size_t	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    صحیح		save_p_ws = p_ws;
    صحیح		dir;
    صحیح		retval = 0;	// default: FAIL
    طویل	lnum_stop = 0;
#اگر_تعریف_شدہ FEAT_RELTIME
    طویل	وقت_حد_بندی = 0;
#اگر_ختم
    صحیح		options = تلاش_KEEP;
    صحیح		subpatnum;
    searchit_arg_T sia;
    صحیح		use_skip = FALSE;
    pos_T	firstpos;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_نمبر_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_نمبر_arg(argvars, 3) == FAIL)))))
	جاؤ theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_تلاش_arg(&argvars[1], flagsp);	// may set p_ws
    اگر (dir == 0)
	جاؤ theend;
    flags = *flagsp;
    اگر (flags & SP_آغاز)
	options |= تلاش_آغاز;
    اگر (flags & SP_انتہا)
	options |= تلاش_انتہا;
    اگر (flags & SP_عمود)
	options |= تلاش_ستون;

    // Optional arguments: line number to stop searching, timeout and skip.
    اگر (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (طویل)tv_get_نمبر_chk(&argvars[2], تہی);
	اگر (lnum_stop < 0)
	    جاؤ theend;
	اگر (argvars[3].v_type != VAR_UNKNOWN)
	{
#اگر_تعریف_شدہ FEAT_RELTIME
	    وقت_حد_بندی = (طویل)tv_get_نمبر_chk(&argvars[3], تہی);
	    اگر (وقت_حد_بندی < 0)
		جاؤ theend;
#اگر_ختم
	    use_skip = eval_اظہاریہ_valid_arg(&argvars[4]);
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
	semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[1]));
	جاؤ theend;
    }

    pos = save_cursor = curwin->w_cursor;
    CLEAR_میدان(firstpos);
    CLEAR_میدان(sia);
    sia.sa_stop_lnum = (linenr_T)lnum_stop;
#اگر_تعریف_شدہ FEAT_RELTIME
    sia.sa_tm = وقت_حد_بندی;
#اگر_ختم

    patlen = STRLEN(pat);

    // Repeat until {skip} returns FALSE.
    برائے (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, تہی, dir, pat, patlen, 1L,
						     options, RE_تلاش, &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	اگر (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	اگر (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    توڑو;
	اگر (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    صحیح	    do_skip;
	    صحیح	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_اظہاریہ_مقصد_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    اگر (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		توڑو;
	    }
	    اگر (!do_skip)
		توڑو;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~تلاش_آغاز;
    }

    اگر (subpatnum != FAIL)
    {
	اگر (flags & SP_SUBPAT)
	    retval = subpatnum;
	ورنہ
	    retval = pos.lnum;
	اگر (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	اگر (مطابقت_pos != تہی)
	{
	    // Store the match cursor position
	    مطابقت_pos->lnum = pos.lnum;
	    مطابقت_pos->ستون = pos.ستون + 1;
	}
	// "/$" will put the cursor after the end of the line, may need to
	// correct that here
	check_cursor();
    }

    // If 'n' flag is used: restore cursor position.
    اگر (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    ورنہ
	curwin->w_سیٹ_curswant = true;
theend:
    p_ws = save_p_ws;

    واپس retval;
}

#اگر_تعریف_شدہ FEAT_RUBY
/*
 * "rubyeval()" function
 */
    ساکن خالی
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#اگر_ختم

/*
 * "screenattr()" function
 */
    ساکن خالی
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    صحیح		صف;
    صحیح		ستون;
    صحیح		c;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    صف = (صحیح)tv_get_نمبر_chk(&argvars[0], تہی) - 1;
    ستون = (صحیح)tv_get_نمبر_chk(&argvars[1], تہی) - 1;
    اگر (صف < 0 || صف >= screen_صفیں
	    || ستون < 0 || ستون >= screen_Columns)
	c = -1;
    ورنہ
	c = ScreenAttrs[سطر_انحراف[صف] + ستون];
    rettv->vval.v_نمبر = c;
}

/*
 * "screenchar()" function
 */
    ساکن خالی
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    صحیح		صف;
    صحیح		ستون;
    صحیح		c;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    صف = (صحیح)tv_get_نمبر_chk(&argvars[0], تہی) - 1;
    ستون = (صحیح)tv_get_نمبر_chk(&argvars[1], تہی) - 1;
    اگر (صف < 0 || صف >= screen_صفیں || ستون < 0 || ستون >= screen_Columns)
	c = -1;
    ورنہ
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(صف, ستون, buf, تہی);
	c = (*mb_ptr2char)(buf);
    }
    rettv->vval.v_نمبر = c;
}

/*
 * "screenchars()" function
 */
    ساکن خالی
f_screenchars(typval_T *argvars, typval_T *rettv)
{
    صحیح		صف;
    صحیح		ستون;
    صحیح		c;
    صحیح		i;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    صف = (صحیح)tv_get_نمبر_chk(&argvars[0], تہی) - 1;
    ستون = (صحیح)tv_get_نمبر_chk(&argvars[1], تہی) - 1;
    اگر (صف < 0 || صف >= screen_صفیں || ستون < 0 || ستون >= screen_Columns)
	واپس;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(صف, ستون, buf, تہی);
    صحیح pcc[MAX_MCO];
    اگر (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    ورنہ
	c = (*mb_ptr2char)(buf);
    list_append_نمبر(rettv->vval.v_list, (varnumber_T)c);

    اگر (enc_utf8)
	برائے (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_نمبر(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    ساکن خالی
f_screencol(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->vval.v_نمبر = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    ساکن خالی
f_screenrow(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->vval.v_نمبر = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    ساکن خالی
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    صحیح		صف;
    صحیح		ستون;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = تہی;
    rettv->v_type = VAR_STRING;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    صف = (صحیح)tv_get_نمبر_chk(&argvars[0], تہی) - 1;
    ستون = (صحیح)tv_get_نمبر_chk(&argvars[1], تہی) - 1;
    اگر (صف < 0 || صف >= screen_صفیں || ستون < 0 || ستون >= screen_Columns)
	واپس;

    screen_getbytes(صف, ستون, buf, تہی);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    ساکن خالی
f_تلاش(typval_T *argvars, typval_T *rettv)
{
    صحیح		flags = 0;

    rettv->vval.v_نمبر = تلاش_cmn(argvars, تہی, &flags);
}

/*
 * "searchdecl()" function
 */
    ساکن خالی
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    صحیح		locally = TRUE;
    صحیح		thisblock = FALSE;
    صحیح		error = FALSE;
    char_u	*name;

    rettv->vval.v_نمبر = 1;	// default: FAIL

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	واپس;

    name = tv_get_string_chk(&argvars[0]);
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(صحیح)tv_get_bool_chk(&argvars[1], &error);
	اگر (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (صحیح)tv_get_bool_chk(&argvars[2], &error);
    }
    اگر (!error && name != تہی)
	rettv->vval.v_نمبر = ڈھونڈو_decl(name, (صحیح)STRLEN(name),
				     locally, thisblock, تلاش_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    ساکن صحیح
searchpair_cmn(typval_T *argvars, pos_T *مطابقت_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    صحیح		save_p_ws = p_ws;
    صحیح		dir;
    صحیح		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    صحیح		retval = 0;		// default: FAIL
    طویل	lnum_stop = 0;
    طویل	وقت_حد_بندی = 0;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_نمبر_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_نمبر_arg(argvars, 6) == FAIL)))))
	جاؤ theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    اگر (spat == تہی || mpat == تہی || epat == تہی)
	جاؤ theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_تلاش_arg(&argvars[3], &flags); // may set p_ws
    اگر (dir == 0)
	جاؤ theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    اگر ((flags & (SP_انتہا | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[3]));
	جاؤ theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    اگر (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    اگر (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = تہی;
    ورنہ
    {
	// Type is checked later.
	skip = &argvars[4];

	اگر (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (طویل)tv_get_نمبر_chk(&argvars[5], تہی);
	    اگر (lnum_stop < 0)
	    {
		semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[5]));
		جاؤ theend;
	    }
#اگر_تعریف_شدہ FEAT_RELTIME
	    اگر (argvars[6].v_type != VAR_UNKNOWN)
	    {
		وقت_حد_بندی = (طویل)tv_get_نمبر_chk(&argvars[6], تہی);
		اگر (وقت_حد_بندی < 0)
		{
		    semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[6]));
		    جاؤ theend;
		}
	    }
#اگر_ختم
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    مطابقت_pos, lnum_stop, وقت_حد_بندی);

theend:
    p_ws = save_p_ws;

    واپس retval;
}

/*
 * "searchpair()" function
 */
    ساکن خالی
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_نمبر = searchpair_cmn(argvars, تہی);
}

/*
 * "searchpairpos()" function
 */
    ساکن خالی
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	مطابقت_pos;
    صحیح		lnum = 0;
    صحیح		ستون = 0;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (searchpair_cmn(argvars, &مطابقت_pos) > 0)
    {
	lnum = مطابقت_pos.lnum;
	ستون = مطابقت_pos.ستون;
    }

    list_append_نمبر(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_نمبر(rettv->vval.v_list, (varnumber_T)ستون);
}

/*
 * Search for a start/middle/end thing.
 * Used by searchpair(), see its documentation for the details.
 * Returns 0 or -1 for no match,
 */
    طویل
do_searchpair(
    char_u	*spat,	    // start pattern
    char_u	*mpat,	    // middle pattern
    char_u	*epat,	    // end pattern
    صحیح		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    صحیح		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*مطابقت_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    طویل	وقت_حد_بندی غیر_مستعمل) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = تہی, *pat3 = تہی;
    size_t	patlen;
    size_t	spatlen;
    size_t	epatlen;
    size_t	pat2size;
    size_t	pat2len;
    size_t	pat3size;
    size_t	pat3len;
    طویل	retval = 0;
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
    صحیح		options = تلاش_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = خالی_خانے_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    اگر (pat2 == تہی)
	جاؤ theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    اگر (pat3 == تہی)
	جاؤ theend;
    pat2len = vim_snprintf((حرف *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    اگر (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    ورنہ
	pat3len = vim_snprintf((حرف *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    اگر (flags & SP_آغاز)
	options |= تلاش_آغاز;

    اگر (skip != تہی)
	use_skip = eval_اظہاریہ_valid_arg(skip);

#اگر_تعریف_شدہ FEAT_RELTIME
    اگر (وقت_حد_بندی > 0)
	init_regexp_timeout(وقت_حد_بندی);
#اگر_ختم
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    برائے (;;)
    {
	searchit_arg_T sia;

	CLEAR_میدان(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, تہی, dir, pat, patlen, 1L,
						     options, RE_تلاش, &sia);
	اگر (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    توڑو;

	اگر (firstpos.lnum == 0)
	    firstpos = pos;
	اگر (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    اگر (dir == BACKWARD)
		decl(&pos);
	    ورنہ
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~تلاش_آغاز;

	// If the skip pattern matches, ignore this match.
	اگر (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_اظہاریہ_مقصد_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    اگر (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		توڑو;
	    }
	    اگر (r)
		جاری;
	}

	اگر ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	ورنہ
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
	    ورنہ
		retval = pos.lnum;
	    اگر (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    اگر (!(flags & SP_REPEAT))
		توڑو;
	    nest = 1;	    // search for next unmatched
	}
    }

    اگر (مطابقت_pos != تہی)
    {
	// Store the match cursor position
	مطابقت_pos->lnum = curwin->w_cursor.lnum;
	مطابقت_pos->ستون = curwin->w_cursor.ستون + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    اگر ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#اگر_تعریف_شدہ FEAT_RELTIME
    اگر (وقت_حد_بندی > 0)
	disable_regexp_timeout();
#اگر_ختم
    vim_free(pat2);
    vim_free(pat3);
    اگر (p_cpo == خالی_خانے_option)
	p_cpo = save_cpo;
    ورنہ
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	اگر (*p_cpo == NUL)
	    سیٹ_option_قدر_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    واپس retval;
}

/*
 * "searchpos()" function
 */
    ساکن خالی
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	مطابقت_pos;
    صحیح		lnum = 0;
    صحیح		ستون = 0;
    صحیح		n;
    صحیح		flags = 0;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    n = تلاش_cmn(argvars, &مطابقت_pos, &flags);
    اگر (n > 0)
    {
	lnum = مطابقت_pos.lnum;
	ستون = مطابقت_pos.ستون;
    }

    list_append_نمبر(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_نمبر(rettv->vval.v_list, (varnumber_T)ستون);
    اگر (flags & SP_SUBPAT)
	list_append_نمبر(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ساکن خالی
سیٹ_position(typval_T *argvars, typval_T *rettv, صحیح charpos)
{
    pos_T	pos;
    صحیح		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	واپس;

    name = tv_get_string_chk(argvars);
    اگر (name == تہی)
	واپس;

    اگر (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	واپس;

    اگر (pos.ستون != MAXCOL && --pos.ستون < 0)
	pos.ستون = 0;
    اگر ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	اگر (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_سیٹ_curswant = false;
	}
	check_cursor();
	rettv->vval.v_نمبر = 0;
    }
    ورنہ اگر (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	اگر (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_نمبر = 0;
    }
    ورنہ
	emsg(_(e_invalid_دلیل));
}
/*
 * "setcharpos()" function
 */
    ساکن خالی
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    سیٹ_position(argvars, rettv, TRUE);
}

    ساکن خالی
f_setcharsearch(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    اگر (check_for_dict_arg(argvars, 0) == FAIL)
	واپس;

    اگر ((d = argvars[0].vval.v_dict) == تہی)
	واپس;

    csearch = dict_get_string(d, "char", FALSE);
    اگر (csearch != تہی)
    {
	اگر (enc_utf8)
	{
	    صحیح pcc[MAX_MCO];
	    صحیح c = utfc_ptr2char(csearch, pcc);

	    سیٹ_آخری_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	ورنہ
	    سیٹ_آخری_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_ڈھونڈو(d, (char_u *)"forward", -1);
    اگر (di != تہی)
	سیٹ_csearch_direction((صحیح)tv_get_نمبر(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_ڈھونڈو(d, (char_u *)"until", -1);
    اگر (di != تہی)
	سیٹ_csearch_until(!!tv_get_نمبر(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    ساکن خالی
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    سیٹ_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    ساکن خالی
f_setenv(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    اگر (check_restricted() || check_secure())
	واپس;

    name = tv_get_string_buf(&argvars[0], namebuf);
    اگر (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_نمبر == VVAL_NULL)
	vim_unsetenv_ext(name);
    ورنہ
	vim_setenv_ext(name, tv_get_string_buf(&argvars[1], valbuf));
}

/*
 * "setfperm({fname}, {mode})" function
 */
    ساکن خالی
f_setfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	modebuf[NUMBUFLEN];
    char_u	*منوال_str;
    صحیح		i;
    صحیح		نقاب;
    صحیح		منوال = 0;

    rettv->vval.v_نمبر = 0;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    fname = tv_get_string_chk(&argvars[0]);
    اگر (fname == تہی)
	واپس;
    منوال_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    اگر (منوال_str == تہی)
	واپس;
    اگر (STRLEN(منوال_str) != 9)
    {
	semsg(_(e_invalid_دلیل_str), منوال_str);
	واپس;
    }

    نقاب = 1;
    برائے (i = 8; i >= 0; --i)
    {
	اگر (منوال_str[i] != '-')
	    منوال |= نقاب;
	نقاب = نقاب << 1;
    }
    rettv->vval.v_نمبر = mch_setperm(fname, منوال) == OK;
}

/*
 * "setpos()" function
 */
    ساکن خالی
f_setpos(typval_T *argvars, typval_T *rettv)
{
    سیٹ_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    صحیح
get_yank_type(char_u **pp, char_u *yank_type, طویل *بلاک_len)
{
    char_u *stropt = *pp;
    انتخاب (*stropt)
    {
	حالت 'v': حالت 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    توڑو;
	حالت 'V': حالت 'l':	// line-wise selection
	    *yank_type = MLINE;
	    توڑو;
	حالت 'b': حالت Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    اگر (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*بلاک_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    توڑو;
	طےشدہ:
	    واپس FAIL;
    }
    *pp = stropt;
    واپس OK;
}

/*
 * "setreg()" function
 */
    ساکن خالی
f_setreg(typval_T *argvars, typval_T *rettv)
{
    صحیح		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    صحیح		append;
    char_u	yank_type;
    طویل	بلاک_len;
    typval_T	*regcontents;
    صحیح		pointreg;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	واپس;

    pointreg = 0;
    regcontents = تہی;
    بلاک_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_نمبر = 1;		// FAIL is default

    اگر (strregname == تہی)
	واپس;		// type error; errmsg already given
    اگر (اندر_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_رجسٹر_name_must_be_one_char_str), strregname);
	واپس;
    }
    regname = *strregname;
    اگر (regname == 0 || regname == '@')
	regname = '"';

    اگر (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	اگر (d == تہی || d->dv_hashtab.ht_مستعمل == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {تہی, تہی};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    واپس;
	}

	di = dict_ڈھونڈو(d, (char_u *)"regcontents", -1);
	اگر (di != تہی)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	اگر (stropt != تہی)
	{
	    صحیح ret = get_yank_type(&stropt, &yank_type, &بلاک_len);

	    اگر (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_قدر_for_دلیل_str), "value");
		واپس;
	    }
	}

	اگر (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    اگر (stropt != تہی)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	ورنہ اگر (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    ورنہ
	regcontents = &argvars[1];

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	اگر (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    واپس;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	اگر (stropt == تہی)
	    واپس;		// type error
	برائے (; *stropt != NUL; ++stropt)
	    انتخاب (*stropt)
	    {
		حالت 'a': حالت 'A':	// append
		    append = TRUE;
		    توڑو;
		طےشدہ:
		    get_yank_type(&stropt, &yank_type, &بلاک_len);
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
	len = ll == تہی ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	اگر (lstval == تہی)
	    واپس;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	اگر (ll != تہی)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_اشیاء(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		اگر (strval == تہی)
		    جاؤ free_lstval;
		اگر (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    اگر (strval == تہی)
			جاؤ free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = تہی;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, بلاک_len);
free_lstval:
	جب (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    ورنہ اگر (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	اگر (strval == تہی)
	    واپس;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, بلاک_len);
    }
    اگر (pointreg != 0)
	get_yank_رجسٹر(pointreg, TRUE);

    rettv->vval.v_نمبر = 0;
}

/*
 * "settagstack()" function
 */
    ساکن خالی
f_settagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    dict_T	*d;
    صحیح		action = 'r';

    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	واپس;

    // first argument: window number or id
    wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
    اگر (wp == تہی)
	واپس;

    // second argument: dict with items to set in the tag stack
    اگر (check_for_dict_arg(argvars, 1) == FAIL)
	واپس;
    d = argvars[1].vval.v_dict;
    اگر (d == تہی)
	واپس;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    اگر (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    ورنہ اگر (check_for_string_arg(argvars, 2) == FAIL)
	واپس;
    ورنہ
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	اگر (actstr == تہی)
	    واپس;
	اگر ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	ورنہ
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    واپس;
	}
    }

    اگر (سیٹ_tagstack(wp, d, action) == OK)
	rettv->vval.v_نمبر = 0;
}

#اگر_تعریف_شدہ FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    ساکن خالی
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحیح		len;

    اگر (اندر_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != تہی ? (char_u *)blob->bv_ga.ga_ڈیٹا : (char_u *)"";
	len = blob != تہی ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_بائٹیں(p, len, تہی, 0));
    }
    ورنہ
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_بائٹیں(p, (صحیح)STRLEN(p), تہی, 0));
    }
}
#اگر_ختم // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    ساکن خالی
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    صحیح do_special;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	واپس;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    ساکن خالی
f_shiftwidth(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->vval.v_نمبر = 0;

    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	طویل	ستون;

	ستون = (طویل)tv_get_نمبر_chk(argvars, تہی);
	اگر (ستون < 0)
	    واپس;	// type error; errmsg already given
#اگر_تعریف_شدہ FEAT_VARTABS
	rettv->vval.v_نمبر = get_sw_قدر_ستون(curbuf, ستون, FALSE);
	واپس;
#اگر_ختم
    }

    rettv->vval.v_نمبر = get_sw_قدر(curbuf);
}

/*
 * "soundfold({word})" function
 */
    ساکن خالی
f_soundfold(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#اگر_تعریف_شدہ FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#ورنہ
    rettv->vval.v_string = vim_strsave(s);
#اگر_ختم
}

/*
 * "spellbadword()" function
 */
    ساکن خالی
f_spellbadword(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    char_u	*لفظ = (char_u *)"";
    hlf_T	attr = HLF_گنتی;
    صحیح		len = 0;
#اگر_تعریف_شدہ FEAT_SPELL
    صحیح		wo_spell_save = curwin->w_p_spell;

    اگر (اندر_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	واپس;

    اگر (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    اگر (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	واپس;
    }
#اگر_ختم

    اگر (rettv_list_alloc(rettv) == FAIL)
    {
#اگر_تعریف_شدہ FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#اگر_ختم
	واپس;
    }

#اگر_تعریف_شدہ FEAT_SPELL
    اگر (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_مقصد(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	اگر (len != 0)
	{
	    لفظ = ml_get_cursor();
	    curwin->w_سیٹ_curswant = true;
	}
    }
    ورنہ اگر (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	صحیح	capcol = -1;

	اگر (str != تہی)
	{
	    // Check the argument for spelling.
	    جب (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		اگر (attr != HLF_گنتی)
		{
		    لفظ = str;
		    توڑو;
		}
		str += len;
		capcol -= len;
		len = 0;
	    }
	}
    }
    curwin->w_p_spell = wo_spell_save;
#اگر_ختم

    list_append_string(rettv->vval.v_list, لفظ, len);
    انتخاب (attr)
    {
    حالت HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	توڑو;

    حالت HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	توڑو;

    حالت HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	توڑو;

    حالت HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	توڑو;

    طےشدہ:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	توڑو;
    }
}

/*
 * "spellsuggest()" function
 */
    ساکن خالی
f_spellsuggest(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
#اگر_تعریف_شدہ FEAT_SPELL
    char_u	*str;
    صحیح		typeerr = FALSE;
    صحیح		maxcount;
    garray_T	ga;
    صحیح		i;
    listitem_T	*li;
    صحیح		need_capital = FALSE;
    صحیح		wo_spell_save = curwin->w_p_spell;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	واپس;

    اگر (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    اگر (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	واپس;
    }
#اگر_ختم

    اگر (rettv_list_alloc(rettv) == FAIL)
    {
#اگر_تعریف_شدہ FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#اگر_ختم
	واپس;
    }

#اگر_تعریف_شدہ FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (صحیح)tv_get_نمبر_chk(&argvars[1], &typeerr);
	اگر (maxcount <= 0)
	    واپس;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (صحیح)tv_get_bool_chk(&argvars[2], &typeerr);
	    اگر (typeerr)
		واپس;
	}
    }
    ورنہ
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    برائے (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_ڈیٹا)[i];

	li = listitem_alloc();
	اگر (li == تہی)
	    vim_free(str);
	ورنہ
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_تالا = 0;
	    li->li_tv.vval.v_string = str;
	    list_append(rettv->vval.v_list, li);
	}
    }
    ga_clear(&ga);
    curwin->w_p_spell = wo_spell_save;
#اگر_ختم
}

    ساکن خالی
f_بانٹو(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*انتہا;
    char_u	*pat = تہی;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    صحیح		مطابقت;
    colnr_T	ستون = 0;
    صحیح		keepempty = FALSE;
    صحیح		typeerr = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	واپس;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = خالی_خانے_option;

    str = tv_get_string(&argvars[0]);
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	اگر (pat == تہی)
	    typeerr = TRUE;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (صحیح)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    اگر (pat == تہی || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    اگر (rettv_list_alloc(rettv) == FAIL)
	جاؤ theend;
    اگر (typeerr)
	جاؤ theend;

    regmatch.regprog = eval_regcomp(pat);
    اگر (regmatch.regprog != تہی)
    {
	regmatch.rm_ic = FALSE;
	جب (*str != NUL || keepempty)
	{
	    اگر (*str == NUL)
		مطابقت = FALSE;	// empty item at the end
	    ورنہ
		مطابقت = vim_regexec_nl(&regmatch, str, ستون);
	    اگر (مطابقت)
		انتہا = regmatch.startp[0];
	    ورنہ
		انتہا = str + STRLEN(str);
	    اگر (keepempty || انتہا > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && مطابقت && انتہا < regmatch.endp[0]))
	    {
		اگر (list_append_string(rettv->vval.v_list, str,
						    (صحیح)(انتہا - str)) == FAIL)
		    توڑو;
	    }
	    اگر (!مطابقت)
		توڑو;
	    // Advance to just after the match.
	    اگر (regmatch.endp[0] > str)
		ستون = 0;
	    ورنہ
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
    ساکن خالی
f_submatch(typval_T *argvars, typval_T *rettv)
{
    صحیح		error = FALSE;
    صحیح		no;
    صحیح		retList = 0;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	واپس;

    no = (صحیح)tv_get_نمبر_chk(&argvars[0], &error);
    اگر (error)
	واپس;
    اگر (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_نمبر_nr), no);
	واپس;
    }
    اگر (argvars[1].v_type != VAR_UNKNOWN)
	retList = (صحیح)tv_get_bool_chk(&argvars[1], &error);
    اگر (error)
	واپس;

    اگر (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    ورنہ
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = reg_submatch_list(no);
    }
}

/*
 * "substitute()" function
 */
    ساکن خالی
f_substitute(typval_T *argvars, typval_T *rettv)
{
    char_u	patbuf[NUMBUFLEN];
    char_u	subbuf[NUMBUFLEN];
    char_u	flagsbuf[NUMBUFLEN];
    char_u	*str;
    char_u	*pat;
    char_u	*sub = تہی;
    typval_T	*اظہاریہ = تہی;
    char_u	*flg;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	واپس;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    اگر (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	اظہاریہ = &argvars[2];
    ورنہ
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    اگر (str == تہی || pat == تہی || (sub == تہی && اظہاریہ == تہی)
								|| flg == تہی)
	rettv->vval.v_string = تہی;
    ورنہ
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, اظہاریہ, flg, تہی);
}

/*
 * "swapfilelist()" function
 */
    ساکن خالی
f_swapfilelist(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;
    recover_نام(تہی, FALSE, rettv->vval.v_list, 0, تہی);
}

/*
 * "swapinfo(swap_filename)" function
 */
    ساکن خالی
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    اگر (rettv_dict_alloc(rettv) == OK)
	get_b0_dict(tv_get_string(argvars), rettv->vval.v_dict);
}

/*
 * "swapname(expr)" function
 */
    ساکن خالی
f_swapname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = tv_get_buf(&argvars[0], FALSE);
    اگر (buf == تہی || buf->b_ml.ml_mfp == تہی
					|| buf->b_ml.ml_mfp->mf_fname == تہی)
	rettv->vval.v_string = تہی;
    ورنہ
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    ساکن خالی
f_synID(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    صحیح		id = 0;
#اگر_تعریف_شدہ FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	ستون;
    صحیح		trans;
    صحیح		transerr = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	واپس;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    ستون = (linenr_T)tv_get_نمبر(&argvars[1]) - 1;	// -1 on type error
    trans = (صحیح)tv_get_bool_chk(&argvars[2], &transerr);

    اگر (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_گنتی
	    && ستون >= 0 && ستون < (طویل)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, ستون, trans, تہی, FALSE);
#اگر_ختم

    rettv->vval.v_نمبر = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    ساکن خالی
f_synIDattr(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    char_u	*p = تہی;
#اگر_تعریف_شدہ FEAT_SYN_HL
    صحیح		id;
    char_u	*what;
    char_u	*منوال;
    char_u	modebuf[NUMBUFLEN];
    صحیح		modec;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	واپس;

    id = (صحیح)tv_get_نمبر(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	منوال = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(منوال[0]);
	اگر (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    ورنہ
    {
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	اگر (USE_24_ثنائی_ہندسہ)
	    modec = 'g';
	ورنہ
# endif
	    اگر (t_colors > 1)
		modec = 'c';
	    ورنہ
		modec = 't';
    }

    انتخاب (TOLOWER_ASC(what[0]))
    {
	حالت 'b':
		اگر (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		ورنہ					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		توڑو;

	حالت 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		توڑو;

	حالت 'i':
		اگر (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		ورنہ					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		توڑو;

	حالت 'n':
		اگر (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		ورنہ					// name
		    p = get_highlight_name_ext(تہی, id - 1, FALSE);
		توڑو;

	حالت 'r':					//الٹ
		p = highlight_has_attr(id, HL_INVERSE, modec);
		توڑو;

	حالت 's':
		اگر (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		ورنہ اگر (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		ورنہ					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		توڑو;

	حالت 'u':
		اگر (STRLEN(what) >= 9)
		{
		    اگر (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    ورنہ اگر (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    ورنہ اگر (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    ورنہ اگر (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    ورنہ
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		ورنہ
							// ul
		    p = highlight_color(id, what, modec);
		توڑو;
    }

    اگر (p != تہی)
	p = vim_strsave(p);
#اگر_ختم
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    ساکن خالی
f_synIDtrans(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    صحیح		id;

#اگر_تعریف_شدہ FEAT_SYN_HL
    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    id = (صحیح)tv_get_نمبر(&argvars[0]);

    اگر (id > 0)
	id = syn_get_final_id(id);
    ورنہ
#اگر_ختم
	id = 0;

    rettv->vval.v_نمبر = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    ساکن خالی
f_synconcealed(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
#اگر defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	ستون;
    صحیح		syntax_flags = 0;
    صحیح		cchar;
    صحیح		matchid = 0;
    char_u	str[NUMBUFLEN];
#اگر_ختم

    rettv_list_سیٹ(rettv, تہی);

    اگر (اندر_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

#اگر defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    ستون = (colnr_T)tv_get_نمبر(&argvars[1]) - 1;	// -1 on type error

    CLEAR_میدان(str);

    اگر (rettv_list_alloc(rettv) == OK)
    {
	اگر (lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_گنتی
	    && ستون >= 0 && ستون <= (طویل)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (خالی)syn_get_id(curwin, lnum, ستون, FALSE, تہی, FALSE);
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
		    ورنہ
			str[0] = cchar;
		}
	    }
	}

	list_append_نمبر(rettv->vval.v_list,
					    (syntax_flags & HL_CONCEAL) != 0);
	// -1 to auto-determine strlen
	list_append_string(rettv->vval.v_list, str, -1);
	list_append_نمبر(rettv->vval.v_list, matchid);
    }
#اگر_ختم
}

/*
 * "synstack(lnum, col)" function
 */
    ساکن خالی
f_synstack(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
#اگر_تعریف_شدہ FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	ستون;
    صحیح		i;
    صحیح		id;
#اگر_ختم

    rettv_list_سیٹ(rettv, تہی);

    اگر (اندر_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

#اگر_تعریف_شدہ FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    ستون = (colnr_T)tv_get_نمبر(&argvars[1]) - 1;	// -1 on type error

    اگر (lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_گنتی
	    && ستون >= 0 && ستون <= (طویل)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(خالی)syn_get_id(curwin, lnum, ستون, FALSE, تہی, TRUE);
	برائے (i = 0; ; ++i)
	{
	    id = syn_get_ڈھیر_شے(i);
	    اگر (id < 0)
		توڑو;
	    اگر (list_append_نمبر(rettv->vval.v_list, id) == FAIL)
		توڑو;
	}
    }
#اگر_ختم
}

/*
 * "tabpagebuflist()" function
 */
    ساکن خالی
f_tabpagebuflist(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    tabpage_T	*tp;
    win_T	*wp = تہی;

    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    ورنہ
    {
	tp = ڈھونڈو_tabpage((صحیح)tv_get_نمبر(&argvars[0]));
	اگر (tp != تہی)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    اگر (wp != تہی && rettv_list_alloc(rettv) == OK)
    {
	برائے (; wp != تہی; wp = wp->w_اگلا)
	    اگر (list_append_نمبر(rettv->vval.v_list,
						wp->w_بفر->b_fnum) == FAIL)
		توڑو;
    }
}

/*
 * "tagfiles()" function
 */
    ساکن خالی
f_tagfiles(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    صحیح		پہلا;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;
    fname = alloc(MAXPATHL);
    اگر (fname == تہی)
	واپس;

    برائے (پہلا = TRUE; ; پہلا = FALSE)
	اگر (get_tagfname(&tn, پہلا, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    توڑو;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    ساکن خالی
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = تہی;
    char_u  *لیبل_نمونہ;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	واپس;

    لیبل_نمونہ = tv_get_string(&argvars[0]);

    rettv->vval.v_نمبر = FALSE;
    اگر (*لیبل_نمونہ == NUL)
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    اگر (rettv_list_alloc(rettv) == OK)
	(خالی)get_لیبل_فہرست(rettv->vval.v_list, لیبل_نمونہ, fname);
}

/*
 * "type(expr)" function
 */
    ساکن خالی
f_type(typval_T *argvars, typval_T *rettv)
{
    صحیح n = -1;

    انتخاب (argvars[0].v_type)
    {
	حالت VAR_نمبر:  n = VAR_TYPE_نمبر; توڑو;
	حالت VAR_STRING:  n = VAR_TYPE_STRING; توڑو;
	حالت VAR_PARTIAL:
	حالت VAR_FUNC:    n = VAR_TYPE_FUNC; توڑو;
	حالت VAR_LIST:    n = VAR_TYPE_LIST; توڑو;
	حالت VAR_TUPLE:   n = VAR_TYPE_TUPLE; توڑو;
	حالت VAR_DICT:    n = VAR_TYPE_DICT; توڑو;
	حالت VAR_FLOAT:   n = VAR_TYPE_FLOAT; توڑو;
	حالت VAR_BOOL:	  n = VAR_TYPE_BOOL; توڑو;
	حالت VAR_SPECIAL: n = VAR_TYPE_NONE; توڑو;
	حالت VAR_کام:     n = VAR_TYPE_کام; توڑو;
	حالت VAR_CHANNEL: n = VAR_TYPE_CHANNEL; توڑو;
	حالت VAR_BLOB:    n = VAR_TYPE_BLOB; توڑو;
	حالت VAR_INSTR:   n = VAR_TYPE_INSTR; توڑو;
	حالت VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; توڑو;
	حالت VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		اگر (cl != تہی && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		ورنہ
		    n = VAR_TYPE_CLASS;
		توڑو;
	    }
	حالت VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		اگر (obj == تہی)
		    n = VAR_TYPE_OBJECT;
		ورنہ
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    اگر (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    ورنہ
			n = VAR_TYPE_OBJECT;
		}
		توڑو;
	    }
	حالت VAR_VOID:
	    emsg(_(e_cannot_use_void_قدر));
	    توڑو;
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	    اندرونی_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    توڑو;
    }
    rettv->vval.v_نمبر = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    ساکن خالی
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_آغاز = 0;
    colnr_T	vcol_انتہا = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    صحیح		winchanged = FALSE;
    صحیح		len;

    اگر (اندر_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_نمبر_arg(argvars, 2) == FAIL))))
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((صحیح)tv_get_نمبر(&argvars[2]), &tp);
	اگر (wp == تہی || tp == تہی)
	    جاؤ theend;

	اگر (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    جاؤ theend;

	check_cursor();
	winchanged = TRUE;
    }

    صحیح fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    اگر (fp != تہی && fp->lnum <= curbuf->b_ml.ml_سطر_گنتی
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	اگر (fp->ستون < 0)
	    fp->ستون = 0;
	ورنہ
	{
	    len = (صحیح)ml_get_len(fp->lnum);
	    اگر (fp->ستون > len)
		fp->ستون = len;
	}
	getvvcol(curwin, fp, &vcol_آغاز, تہی, &vcol_انتہا, 0);
	++vcol_آغاز;
	++vcol_انتہا;
    }

theend:
    اگر (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	اگر (rettv_list_alloc(rettv) == OK)
	{
	    list_append_نمبر(rettv->vval.v_list, vcol_آغاز);
	    list_append_نمبر(rettv->vval.v_list, vcol_انتہا);
	}
	ورنہ
	    rettv->vval.v_نمبر = 0;
    }
    ورنہ
	rettv->vval.v_نمبر = vcol_انتہا;

    اگر (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    ساکن خالی
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    اگر (اندر_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_منوال_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    اگر (non_zero_arg(&argvars[0]))
	curbuf->b_visual_منوال_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    ساکن خالی
f_wildmenumode(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    اگر (wild_menu_showing || ((State & منوال_CMDLINE) && cmdline_pum_فعال()))
	rettv->vval.v_نمبر = 1;
}

/*
 * "windowsversion()" function
 */
    ساکن خالی
f_windowsversion(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windows_نسخہ);
}

/*
 * "wordcount()" function
 */
    ساکن خالی
f_wordcount(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    ساکن خالی
f_xor(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    rettv->vval.v_نمبر = tv_get_نمبر_chk(&argvars[0], تہی)
					^ tv_get_نمبر_chk(&argvars[1], تہی);
}

#اگر_ختم // FEAT_EVAL
