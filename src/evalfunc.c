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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

நிலையான வெறுமை f_and(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_BEVAL
நிலையான வெறுமை f_balloon_gettext(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_balloon_show(typval_T *argvars, typval_T *rettv);
# எனில் defined(FEAT_BEVAL_TERM)
நிலையான வெறுமை f_balloon_split(typval_T *argvars, typval_T *rettv);
# endif
#endif
நிலையான வெறுமை f_base64_encode(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_base64_decode(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_bindtextdomain(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_byte2line(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_call(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_changenr(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_char2nr(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_charcol(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_col(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_confirm(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_copy(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_cursor(typval_T *argsvars, typval_T *rettv);
#ifdef MSWIN
நிலையான வெறுமை f_debugbreak(typval_T *argvars, typval_T *rettv);
#endif
நிலையான வெறுமை f_deepcopy(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_did_filetype(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_echoraw(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_empty(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_environ(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_err_teapot(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_escape(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_eval(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_eventhandler(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_execute(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_exists_compiled(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_expand(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_expandcmd(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_feedkeys(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_fnameescape(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_foreground(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_funcref(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_function(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_garbagecollect(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_get(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getbgcolor(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getcellpixels(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getchangelist(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getcharpos(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getcharsearch(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getcurpos(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getcursorcharpos(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getenv(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getfontname(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getinfo(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getjumplist(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getpid(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getpos(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getreg(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getreginfo(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getregion(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getregionpos(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_getregtype(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_gettagstack(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_gettext(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_haslocaldir(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_hlID(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_hlexists(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_hostname(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_id(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_index(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_indexof(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_உள்ளீடு(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_inputdialog(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_inputlist(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_inputrestore(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_inputsave(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_inputsecret(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_interrupt(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_invert(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_islocked(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_பொருட்கள்(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_keytrans(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_last_buffer_nr(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_libcall(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_libcallnr(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_வரி(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_line2byte(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_LUA
நிலையான வெறுமை f_luaeval(typval_T *argvars, typval_T *rettv);
#endif
நிலையான வெறுமை f_match(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_matchbufline(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_matchend(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_matchlist(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_matchstr(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_matchstrlist(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_matchstrpos(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_max(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_min(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_MZSCHEME
நிலையான வெறுமை f_mzeval(typval_T *argvars, typval_T *rettv);
#endif
நிலையான வெறுமை f_nextnonblank(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_ngettext(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_nr2char(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_or(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PERL
நிலையான வெறுமை f_perleval(typval_T *argvars, typval_T *rettv);
#endif
நிலையான வெறுமை f_prevnonblank(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_printf(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_pum_getpos(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_pumvisible(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PYTHON3
நிலையான வெறுமை f_py3eval(typval_T *argvars, typval_T *rettv);
#endif
#ifdef FEAT_PYTHON
நிலையான வெறுமை f_pyeval(typval_T *argvars, typval_T *rettv);
#endif
#எனில் defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
நிலையான வெறுமை f_pyxeval(typval_T *argvars, typval_T *rettv);
#endif
நிலையான வெறுமை f_test_srand_seed(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_rand(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_range(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_reg_executing(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_reg_recording(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_rename(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_repeat(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_RUBY
நிலையான வெறுமை f_rubyeval(typval_T *argvars, typval_T *rettv);
#endif
நிலையான வெறுமை f_screenattr(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_screenchar(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_screenchars(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_screencol(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_screenrow(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_screenstring(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_தேடல்(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_searchdecl(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_searchpair(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_searchpairpos(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_searchpos(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_setcharpos(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_setcharsearch(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_setcursorcharpos(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_setenv(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_setfperm(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_setpos(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_setreg(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_settagstack(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_CRYPT
நிலையான வெறுமை f_sha256(typval_T *argvars, typval_T *rettv);
#endif
நிலையான வெறுமை f_shellescape(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_shiftwidth(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_soundfold(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_spellbadword(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_spellsuggest(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_split(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_srand(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_submatch(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_substitute(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_swapfilelist(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_swapinfo(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_swapname(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_synID(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_synIDattr(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_synIDtrans(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_synstack(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_synconcealed(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_tabpagebuflist(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_taglist(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_tagfiles(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_type(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_virtcol(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_visualmode(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_wildmenumode(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_windowsversion(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_wordcount(typval_T *argvars, typval_T *rettv);
நிலையான வெறுமை f_xor(typval_T *argvars, typval_T *rettv);


/*
 * Functions that check the argument type of a builtin function.
 * Each function returns FAIL and gives an error message if the type is wrong.
 */

// Context passed to an arg_ function.
வகைவரை கட்டமைப்பு {
    முழு		arg_மொத்தம்;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    முழு		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
வகைவரை முழு (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    நிலையான முழு
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    திரும்பு need_type(actual, expected, 0,
	    context->arg_idx - context->arg_மொத்தம், context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    நிலையான முழு
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    எனில் (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_மொத்தம், context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	திரும்பு FAIL;
    திரும்பு arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    முழு
arg_type_modifiable(type_T *type, முழு arg_idx)
{
    எழுத்து *tofree;

    எனில் ((type->tt_flags & TTFLAG_CONST) == 0)
	திரும்பு OK;
    semsg(_(e_argument_nr_trying_to_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    திரும்பு FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    நிலையான முழு
arg_any(type_T *type UNUSED,
	type_T *decl_type UNUSED,
	argcontext_T *context UNUSED)
{
    திரும்பு OK;
}

/*
 * Check "type" is a float or a number.
 */
    நிலையான முழு
arg_float_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_எண்
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_எண், type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a number.
 */
    நிலையான முழு
arg_எண்(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    திரும்பு check_arg_type(&t_எண், type, context);
}

/*
 * Check "type" is an object.
 */
    நிலையான முழு
arg_object(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    நிலையான முழு
arg_dict_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    திரும்பு check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    நிலையான முழு
arg_list_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    திரும்பு check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    நிலையான முழு
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    திரும்பு check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    நிலையான முழு
arg_list_எண்(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    திரும்பு check_arg_type(&t_list_எண், type, context);
}

/*
 * Check "type" is a list of strings.
 */
    நிலையான முழு
arg_list_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    திரும்பு check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    நிலையான முழு
arg_tuple_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    திரும்பு check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    நிலையான முழு
arg_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    திரும்பு check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    நிலையான முழு
arg_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    திரும்பு check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    நிலையான முழு
arg_bool(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    திரும்பு check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    நிலையான முழு
arg_bool_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_எண்
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_எண், type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    நிலையான முழு
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	திரும்பு OK;
    திரும்பு check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    நிலையான முழு
arg_list_or_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    நிலையான முழு
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    எனில் (arg_list_or_blob(type, decl_type, context) == FAIL)
	திரும்பு FAIL;
    திரும்பு arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    நிலையான முழு
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    திரும்பு FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    நிலையான முழு
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a string or a number
 */
    நிலையான முழு
arg_string_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_எண்
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    நிலையான முழு
arg_buffer(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_எண்
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    நிலையான முழு
arg_buffer_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_எண்
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    நிலையான முழு
arg_lnum(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_எண்
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    நிலையான முழு
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	திரும்பு OK;
    எனில் (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	திரும்பு FAIL;
    }
    எனில் (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	திரும்பு OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    நிலையான முழு
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    நிலையான முழு
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    நிலையான முழு
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    நிலையான முழு
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    நிலையான முழு
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    எனில் (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    நிலையான முழு
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    எனில் (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	திரும்பு FAIL;
    திரும்பு arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    நிலையான முழு
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    நிலையான முழு
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    எனில் (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	திரும்பு FAIL;
    திரும்பு arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    நிலையான முழு
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	திரும்பு OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr),
						 context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    நிலையான முழு
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = வெற்று;
    type_T *(args[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, வெற்று, வெற்று, args};

    எனில் (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	எனில் (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	இல்லையெனில்
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    இல்லையெனில் எனில் (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    இல்லையெனில் எனில் (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_எண்;

    args[0] = வெற்று;
    args[1] = &t_unknown;
    எனில் (type->tt_argcount != -1)
    {
	எனில் (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_எண்_of_arguments));
	    திரும்பு FAIL;
	}
	எனில் (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	இல்லையெனில்
	{
	    எனில் (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		args[0] = &t_எண்;
	    இல்லையெனில் எனில் (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		args[0] = &t_string;
	    எனில் (args[0] != வெற்று)
		args[1] = expected_member;
	}
    }

    எனில் (!type_any_or_unknown(type->tt_member) || args[0] != வெற்று)
    {
	where_T where = WHERE_INIT;

	எனில் (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == வெற்று
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	இல்லையெனில் எனில் (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	இல்லையெனில் // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	எனில் (args[0] == வெற்று)
	    args[0] = &t_unknown;
	எனில் (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_index = 2;
	where.wt_kind = WT_ARGUMENT;
	திரும்பு check_type(&t_func_exp, type, TRUE, where);
    }
    திரும்பு OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    நிலையான முழு
arg_filter_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	திரும்பு OK;

    எனில் (type->tt_type == VAR_FUNC)
	திரும்பு check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    திரும்பு FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    நிலையான முழு
arg_map_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	திரும்பு OK;

    எனில் (type->tt_type == VAR_FUNC)
	திரும்பு check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    திரும்பு FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    நிலையான முழு
arg_foreach_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	திரும்பு OK;

    எனில் (type->tt_type == VAR_FUNC)
	திரும்பு check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    திரும்பு FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    நிலையான முழு
arg_sort_how(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	திரும்பு OK;

    எனில் (type->tt_type == VAR_FUNC)
    {
	type_T *(args[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_எண், வெற்று, args};

	எனில் (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    args[0] = context->arg_types[0].type_curr->tt_member;
	இல்லையெனில்
	    args[0] = &t_unknown;
	எனில் (!type_any_or_unknown(type->tt_member) || args[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    args[1] = args[0];
	    எனில் (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_index = 2;
	    where.wt_kind = WT_ARGUMENT;
	    திரும்பு check_type(&t_func_exp, type, TRUE, where);
	}

	திரும்பு OK;
    }
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    திரும்பு FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    நிலையான முழு
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_எண்
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    நிலையான முழு
varargs_class(type_T *type UNUSED,
	      type_T *decl_type UNUSED,
	      argcontext_T *context)
{
    க்கு (முழு i = context->arg_idx; i < context->arg_மொத்தம்; ++i)
    {
	type2_T *types = &context->arg_types[i];
	எனில் (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    திரும்பு FAIL;
	}
    }
    திரும்பு OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    நிலையான முழு
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    நிலையான முழு
arg_reverse(type_T *type, type_T *decl_type, argcontext_T *context)
{
    எனில் (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	திரும்பு FAIL;
    திரும்பு arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    நிலையான முழு
arg_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    திரும்பு check_arg_type(&t_job, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    நிலையான முழு
arg_chan_or_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_JOB
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    நிலையான முழு
arg_same_as_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_decl;

    திரும்பு check_arg_type(prev_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    நிலையான முழு
arg_same_struct_as_prev(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;

    எனில் (prev_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	திரும்பு check_arg_type(prev_type, type, context);
    திரும்பு OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    நிலையான முழு
arg_item_of_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    எனில் (prev_type->tt_type == VAR_LIST)
	expected = prev_type->tt_member;
    இல்லையெனில் எனில் (prev_type->tt_type == VAR_BLOB)
	expected = &t_எண்;
    இல்லையெனில்
	// probably VAR_ANY, can't check
	திரும்பு OK;

    திரும்பு check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    நிலையான முழு
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_எண்
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    நிலையான முழு
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	திரும்பு OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    நிலையான முழு
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 2].type_curr;

    எனில் (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	திரும்பு arg_எண்(type, decl_type, context);
    எனில் (first_type->tt_type == VAR_DICT)
	திரும்பு arg_string(type, decl_type, context);
    திரும்பு OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    நிலையான முழு
arg_get1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	திரும்பு OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    நிலையான முழு
arg_len1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_எண்
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	திரும்பு OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    நிலையான முழு
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 1].type_curr;

    எனில் (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	திரும்பு arg_எண்(type, decl_type, context);
    எனில் (first_type->tt_type == VAR_DICT)
	திரும்பு arg_string_or_nr(type, decl_type, context);
    திரும்பு OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    நிலையான முழு
arg_repeat1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_எண்
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	திரும்பு OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    நிலையான முழு
arg_slice1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	திரும்பு OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    நிலையான முழு
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    எனில் (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	திரும்பு OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr),
							 context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    நிலையான முழு
arg_cursor1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    எனில் (type->tt_type == VAR_எண்
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	திரும்பு OK;

    arg_type_mismatch(&t_எண், type, context->arg_idx + 1);
    திரும்பு FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
நிலையான argcheck_T arg1_blob[] = {arg_blob};
நிலையான argcheck_T arg1_bool[] = {arg_bool};
நிலையான argcheck_T arg1_buffer[] = {arg_buffer};
நிலையான argcheck_T arg1_buffer_or_dict_any[] = {arg_buffer_or_dict_any};
நிலையான argcheck_T arg1_chan_or_job[] = {arg_chan_or_job};
நிலையான argcheck_T arg1_dict_any[] = {arg_dict_any};
நிலையான argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
நிலையான argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
நிலையான argcheck_T arg1_job[] = {arg_job};
நிலையான argcheck_T arg1_list_any[] = {arg_list_any};
நிலையான argcheck_T arg1_tuple_any[] = {arg_tuple_any};
நிலையான argcheck_T arg1_list_எண்[] = {arg_list_எண்};
நிலையான argcheck_T arg1_reverse[] = {arg_reverse};
நிலையான argcheck_T arg1_list_or_tuple_or_dict[] = {arg_list_or_tuple_or_dict};
நிலையான argcheck_T arg1_list_string[] = {arg_list_string};
நிலையான argcheck_T arg1_list_tuple_dict_blob_or_string[] = {arg_list_tuple_dict_blob_or_string};
நிலையான argcheck_T arg1_lnum[] = {arg_lnum};
நிலையான argcheck_T arg1_எண்[] = {arg_எண்};
நிலையான argcheck_T arg1_string[] = {arg_string};
நிலையான argcheck_T arg1_string_or_list_any[] = {arg_string_or_list_any};
நிலையான argcheck_T arg1_string_or_list_string[] = {arg_string_or_list_string};
நிலையான argcheck_T arg1_string_or_nr[] = {arg_string_or_nr};
நிலையான argcheck_T arg1_string_or_blob[] = {arg_string_or_blob};
நிலையான argcheck_T arg2_buffer_any[] = {arg_buffer, arg_any};
நிலையான argcheck_T arg2_buffer_bool[] = {arg_buffer, arg_bool};
நிலையான argcheck_T arg2_buffer_list_any[] = {arg_buffer, arg_list_any};
நிலையான argcheck_T arg2_buffer_lnum[] = {arg_buffer, arg_lnum};
நிலையான argcheck_T arg2_buffer_எண்[] = {arg_buffer, arg_எண்};
நிலையான argcheck_T arg2_buffer_string[] = {arg_buffer, arg_string};
நிலையான argcheck_T arg2_chan_or_job_dict[] = {arg_chan_or_job, arg_dict_any};
நிலையான argcheck_T arg2_chan_or_job_string[] = {arg_chan_or_job, arg_string};
நிலையான argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
நிலையான argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
நிலையான argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
நிலையான argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
நிலையான argcheck_T arg2_job_dict[] = {arg_job, arg_dict_any};
நிலையான argcheck_T arg2_job_string_or_எண்[] = {arg_job, arg_string_or_nr};
நிலையான argcheck_T arg2_list_any_எண்[] = {arg_list_any, arg_எண்};
நிலையான argcheck_T arg2_list_எண்[] = {arg_list_எண், arg_list_எண்};
நிலையான argcheck_T arg2_list_எண்_bool[] = {arg_list_எண், arg_bool};
நிலையான argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
நிலையான argcheck_T arg2_listblobmod_item[] = {arg_list_or_blob_mod, arg_item_of_prev};
நிலையான argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
நிலையான argcheck_T arg2_lnum_எண்[] = {arg_lnum, arg_எண்};
நிலையான argcheck_T arg2_எண்[] = {arg_எண், arg_எண்};
நிலையான argcheck_T arg2_எண்_any[] = {arg_எண், arg_any};
நிலையான argcheck_T arg2_எண்_bool[] = {arg_எண், arg_bool};
நிலையான argcheck_T arg2_எண்_dict_any[] = {arg_எண், arg_dict_any};
நிலையான argcheck_T arg2_எண்_list[] = {arg_எண், arg_list_any};
நிலையான argcheck_T arg2_எண்_string[] = {arg_எண், arg_string};
நிலையான argcheck_T arg2_எண்_buffer[] = {arg_எண், arg_buffer};
நிலையான argcheck_T arg2_எண்_string_or_list[] = {arg_எண், arg_string_or_list_any};
நிலையான argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
நிலையான argcheck_T arg2_string[] = {arg_string, arg_string};
நிலையான argcheck_T arg2_string_any[] = {arg_string, arg_any};
நிலையான argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
நிலையான argcheck_T arg2_string_chan_or_job[] = {arg_string, arg_chan_or_job};
நிலையான argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
நிலையான argcheck_T arg2_string_list_எண்[] = {arg_string, arg_list_எண்};
நிலையான argcheck_T arg2_string_எண்[] = {arg_string, arg_எண்};
நிலையான argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
நிலையான argcheck_T arg2_string_or_list_எண்[] = {arg_string_or_list_any, arg_எண்};
நிலையான argcheck_T arg2_string_string_or_எண்[] = {arg_string, arg_string_or_nr};
நிலையான argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
நிலையான argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
நிலையான argcheck_T arg3_any_buffer_bool_or_dict[] = {
			      arg_any, arg_buffer, arg_bool_or_dict_any};
நிலையான argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
நிலையான argcheck_T arg3_buffer_lnum_lnum[] = {arg_buffer, arg_lnum, arg_lnum};
நிலையான argcheck_T arg3_buffer_எண்_எண்[] = {arg_buffer, arg_எண், arg_எண்};
நிலையான argcheck_T arg3_buffer_string_any[] = {arg_buffer, arg_string, arg_any};
நிலையான argcheck_T arg3_buffer_string_dict[] = {arg_buffer, arg_string, arg_dict_any};
நிலையான argcheck_T arg3_dict_எண்_எண்[] = {arg_dict_any, arg_எண், arg_எண்};
நிலையான argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
நிலையான argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
நிலையான argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
நிலையான argcheck_T arg3_lnum_எண்_bool[] = {arg_lnum, arg_எண், arg_bool};
நிலையான argcheck_T arg3_எண்[] = {arg_எண், arg_எண், arg_எண்};
நிலையான argcheck_T arg3_எண்_any_dict[] = {arg_எண், arg_any, arg_dict_any};
நிலையான argcheck_T arg3_எண்_எண்_dict[] = {arg_எண், arg_எண், arg_dict_any};
நிலையான argcheck_T arg3_எண்_string_any[] = {arg_எண், arg_string, arg_any};
நிலையான argcheck_T arg3_எண்_string_buffer[] = {arg_எண், arg_string, arg_buffer};
நிலையான argcheck_T arg3_எண்_string_string[] = {arg_எண், arg_string, arg_string};
நிலையான argcheck_T arg3_string[] = {arg_string, arg_string, arg_string};
நிலையான argcheck_T arg3_string_any_dict[] = {arg_string, arg_any, arg_dict_any};
நிலையான argcheck_T arg3_string_any_string[] = {arg_string, arg_any, arg_string};
நிலையான argcheck_T arg3_string_bool_bool[] = {arg_string, arg_bool, arg_bool};
நிலையான argcheck_T arg3_string_எண்_bool[] = {arg_string, arg_எண், arg_bool};
நிலையான argcheck_T arg3_string_எண்_எண்[] = {arg_string, arg_எண், arg_எண்};
நிலையான argcheck_T arg3_string_or_dict_bool_dict[] = {arg_string_or_dict_any, arg_bool, arg_dict_any};
நிலையான argcheck_T arg3_string_or_list_bool_எண்[] = {arg_string_or_list_any, arg_bool, arg_எண்};
நிலையான argcheck_T arg3_string_string_bool[] = {arg_string, arg_string, arg_bool};
நிலையான argcheck_T arg3_string_string_dict[] = {arg_string, arg_string, arg_dict_any};
நிலையான argcheck_T arg3_string_string_எண்[] = {arg_string, arg_string, arg_எண்};
நிலையான argcheck_T arg4_எண்_எண்_string_any[] = {arg_எண், arg_எண், arg_string, arg_any};
நிலையான argcheck_T arg4_string_string_any_string[] = {arg_string, arg_string, arg_any, arg_string};
நிலையான argcheck_T arg4_string_string_எண்_string[] = {arg_string, arg_string, arg_எண், arg_string};
நிலையான argcheck_T arg4_string_எண்_bool_bool[] = {arg_string, arg_எண், arg_bool, arg_bool};
/* Function specific argument types (not covered by the above) */
நிலையான argcheck_T arg15_assert_fails[] = {arg_string_or_nr, arg_string_or_list_any, arg_any, arg_எண், arg_string};
நிலையான argcheck_T arg34_assert_inrange[] = {arg_float_or_nr, arg_float_or_nr, arg_float_or_nr, arg_string};
நிலையான argcheck_T arg4_browse[] = {arg_bool, arg_string, arg_string, arg_string};
நிலையான argcheck_T arg23_chanexpr[] = {arg_chan_or_job, arg_any, arg_dict_any};
நிலையான argcheck_T arg23_chanraw[] = {arg_chan_or_job, arg_string_or_blob, arg_dict_any};
நிலையான argcheck_T arg24_மொத்தம்[] = {arg_string_list_tuple_or_dict, arg_any, arg_bool, arg_எண்};
நிலையான argcheck_T arg13_cursor[] = {arg_cursor1, arg_எண், arg_எண்};
நிலையான argcheck_T arg12_deepcopy[] = {arg_any, arg_bool};
நிலையான argcheck_T arg12_execute[] = {arg_string_or_list_string, arg_string};
நிலையான argcheck_T arg12_getchar[] = {arg_bool_or_nr, arg_dict_any};
நிலையான argcheck_T arg23_extend[] = {arg_list_or_dict_or_blob_mod, arg_same_as_prev, arg_extend3};
நிலையான argcheck_T arg23_extendnew[] = {arg_list_or_dict_or_blob, arg_same_struct_as_prev, arg_extend3};
நிலையான argcheck_T arg23_get[] = {arg_get1, arg_string_or_nr, arg_any};
நிலையான argcheck_T arg14_glob[] = {arg_string, arg_bool, arg_bool, arg_bool};
நிலையான argcheck_T arg25_globpath[] = {arg_string, arg_string, arg_bool, arg_bool, arg_bool};
நிலையான argcheck_T arg24_index[] = {arg_list_or_tuple_or_blob, arg_item_of_prev, arg_எண், arg_bool};
நிலையான argcheck_T arg23_index[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
நிலையான argcheck_T arg23_செருகு[] = {arg_list_or_blob, arg_item_of_prev, arg_எண்};
நிலையான argcheck_T arg1_len[] = {arg_len1};
நிலையான argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
நிலையான argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
நிலையான argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
நிலையான argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
நிலையான argcheck_T arg2_instanceof[] = {arg_object, varargs_class, வெற்று };
நிலையான argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
நிலையான argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
நிலையான argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_எண், arg_எண், arg_dict_any};
நிலையான argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_எண், arg_எண், arg_dict_any};
நிலையான argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
நிலையான argcheck_T arg45_matchbufline[] = {arg_buffer, arg_string, arg_lnum, arg_lnum, arg_dict_any};
நிலையான argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
நிலையான argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
நிலையான argcheck_T arg24_remote_கோவை[] = {arg_string, arg_string, arg_string, arg_எண்};
நிலையான argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_எண்};
நிலையான argcheck_T arg2_repeat[] = {arg_repeat1, arg_எண்};
நிலையான argcheck_T arg15_தேடல்[] = {arg_string, arg_string, arg_எண், arg_எண், arg_string_or_func};
நிலையான argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_எண், arg_எண்};
நிலையான argcheck_T arg3_setbufline[] = {arg_buffer, arg_lnum, arg_str_or_nr_or_list};
நிலையான argcheck_T arg2_setline[] = {arg_lnum, arg_any};
நிலையான argcheck_T arg24_setloclist[] = {arg_எண், arg_list_any, arg_string, arg_dict_any};
நிலையான argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
நிலையான argcheck_T arg23_settagstack[] = {arg_எண், arg_dict_any, arg_string};
நிலையான argcheck_T arg02_sign_getplaced[] = {arg_buffer, arg_dict_any};
நிலையான argcheck_T arg45_sign_place[] = {arg_எண், arg_string, arg_string, arg_buffer, arg_dict_any};
நிலையான argcheck_T arg23_slice[] = {arg_slice1, arg_எண், arg_எண்};
நிலையான argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_sort_how, arg_dict_any};
நிலையான argcheck_T arg24_strpart[] = {arg_string, arg_எண், arg_எண், arg_bool};
நிலையான argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
நிலையான argcheck_T arg23_win_execute[] = {arg_எண், arg_string_or_list_string, arg_string};
நிலையான argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
நிலையான argcheck_T arg24_match_func[] = {arg_string_or_list_any, arg_string, arg_எண், arg_எண்};

// Can be used by functions called through "f_retfunc" to create new types.
நிலையான garray_T *current_type_gap = வெற்று;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    நிலையான type_T *
ret_void(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_void;
}
    நிலையான type_T *
ret_any(முழு	argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_any;
}
    நிலையான type_T *
ret_bool(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_bool;
}
    நிலையான type_T *
ret_எண்_bool(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_எண்_bool;
}
    நிலையான type_T *
ret_எண்(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_எண்;
}
    நிலையான type_T *
ret_float(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_float;
}
    நிலையான type_T *
ret_string(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_string;
}
    நிலையான type_T *
ret_list_any(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_list_any;
}
    நிலையான type_T *
ret_list_எண்(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    திரும்பு &t_list_எண்;
}
    நிலையான type_T *
ret_list_string(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    திரும்பு &t_list_string;
}
    நிலையான type_T *
ret_list_dict_any(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    திரும்பு &t_list_dict_any;
}
    நிலையான type_T *
ret_list_பொருட்கள்(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    திரும்பு &t_list_list_any;
}
    நிலையான type_T *
ret_list_list_any(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    திரும்பு &t_list_list_any;
}
    நிலையான type_T *
ret_list_string_பொருட்கள்(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    திரும்பு &t_list_list_string;
}
    நிலையான type_T *
ret_list_regionpos(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    திரும்பு &t_list_list_list_எண்;
}
    நிலையான type_T *
ret_tuple_any(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_tuple_any;
}
    நிலையான type_T *
ret_dict_any(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_dict_any;
}
    நிலையான type_T *
ret_job_info(முழு argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    எனில் (argcount == 0)
    {
	*decl_type = &t_list_any;
	திரும்பு &t_list_job;
    }
    திரும்பு &t_dict_any;
}
    நிலையான type_T *
ret_dict_எண்(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_dict_எண்;
}
    நிலையான type_T *
ret_dict_string(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_dict_string;
}
    நிலையான type_T *
ret_blob(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_blob;
}
    நிலையான type_T *
ret_func_any(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_func_any;
}
    நிலையான type_T *
ret_func_unknown(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_func_unknown;
}
    நிலையான type_T *
ret_channel(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_channel;
}
    நிலையான type_T *
ret_job(முழு argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    திரும்பு &t_job;
}
    நிலையான type_T *
ret_first_arg(முழு argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    எனில் (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	திரும்பு argtypes[0].type_curr;
    }
    திரும்பு &t_void;
}
    நிலையான type_T *
ret_slice(முழு argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    எனில் (argcount > 0)
    {
	எனில் (argtypes[0].type_decl != வெற்று)
	{
	    தேர்வு (argtypes[0].type_decl->tt_type)
	    {
		நிலை VAR_STRING: *decl_type = &t_string; நிறுத்து;
		நிலை VAR_BLOB: *decl_type = &t_blob; நிறுத்து;
		நிலை VAR_LIST: *decl_type = &t_list_any; நிறுத்து;
		நிலை VAR_TUPLE: *decl_type = &t_tuple_any; நிறுத்து;
		இயல்பு: நிறுத்து;
	    }
	}
	திரும்பு argtypes[0].type_curr;
    }
    திரும்பு &t_void;
}
    நிலையான type_T *
ret_copy(முழு argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    எனில் (argcount > 0)
    {
	எனில் (argtypes[0].type_decl != வெற்று)
	{
	    எனில் (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    இல்லையெனில் எனில் (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    இல்லையெனில்
		*decl_type = argtypes[0].type_decl;
	}
	எனில் (argtypes[0].type_curr != வெற்று)
	{
	    எனில் (argtypes[0].type_curr->tt_type == VAR_LIST)
		திரும்பு &t_list_any;
	    இல்லையெனில் எனில் (argtypes[0].type_curr->tt_type == VAR_DICT)
		திரும்பு &t_dict_any;
	}
	திரும்பு argtypes[0].type_curr;
    }
    திரும்பு &t_void;
}
    நிலையான type_T *
ret_extend(முழு argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    எனில் (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	// if the second argument has a different current type then the current
	// type is "any"
	எனில் (argcount > 1 && !equal_type(argtypes[0].type_curr,
						     argtypes[1].type_curr, 0))
	{
	    எனில் (argtypes[0].type_curr->tt_type == VAR_LIST)
		திரும்பு &t_list_any;
	    எனில் (argtypes[0].type_curr->tt_type == VAR_DICT)
		திரும்பு &t_dict_any;
	    எனில் (argtypes[0].type_curr->tt_type == VAR_BLOB)
		திரும்பு &t_blob;
	}
	திரும்பு argtypes[0].type_curr;
    }
    திரும்பு &t_void;
}
    நிலையான type_T *
ret_repeat(முழு argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    எனில் (argcount == 0)
	திரும்பு &t_any;
    எனில் (argtypes[0].type_curr == &t_எண்)
	திரும்பு &t_string;
    திரும்பு argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    நிலையான type_T *
ret_first_cont(முழு argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    எனில் (argcount > 0)
    {
	எனில் (argtypes[0].type_curr->tt_type == VAR_LIST)
	    திரும்பு &t_list_any;
	எனில் (argtypes[0].type_curr->tt_type == VAR_DICT)
	    திரும்பு &t_dict_any;
	எனில் (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    திரும்பு argtypes[0].type_curr;
    }
    திரும்பு &t_any;
}
// for get(): the type of the item when a missing item has the same type
    நிலையான type_T *
ret_get(முழு argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    எனில் (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*item = வெற்று;

	எனில் (t->tt_type == VAR_BLOB)
	    item = &t_எண்;
	இல்லையெனில் எனில் (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    item = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	எனில் (item != வெற்று && (argcount == 3
			? equal_type(item, argtypes[2].type_curr, 0)
			: item->tt_type == VAR_எண்))
	{
	    எனில் (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    திரும்பு item;
	}
    }
    திரும்பு &t_any;
}
// for getline()
    நிலையான type_T *
ret_getline(முழு argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    எனில் (argcount == 1)
	திரும்பு &t_string;
    *decl_type = &t_list_any;
    திரும்பு &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    நிலையான type_T *
ret_string_or_any_1(முழு argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    எனில் (argcount < 1)
	திரும்பு &t_string;
    திரும்பு &t_any;
}
    நிலையான type_T *
ret_string_or_any_2(முழு argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    எனில் (argcount < 2)
	திரும்பு &t_string;
    திரும்பு &t_any;
}
    நிலையான type_T *
ret_string_or_any_3(முழு argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    எனில் (argcount < 3)
	திரும்பு &t_string;
    திரும்பு &t_any;
}
    நிலையான type_T *
ret_string_or_any_4(முழு argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    எனில் (argcount < 4)
	திரும்பு &t_string;
    திரும்பு &t_any;
}
// for abs()
    நிலையான type_T *
ret_abs(முழு argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    எனில் (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	திரும்பு &t_float;
    திரும்பு &t_எண்;
}
// for sign_define()
    நிலையான type_T *
ret_sign_define(முழு argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    எனில் (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	திரும்பு &t_list_எண்;
    திரும்பு &t_எண்;
}
// for values(): list of member of first argument
    நிலையான type_T *
ret_list_member(முழு argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    எனில் (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	எனில் (current_type_gap != வெற்று
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	இல்லையெனில்
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	எனில் (current_type_gap != வெற்று
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    திரும்பு get_list_type(t->tt_member, current_type_gap);
    }
    திரும்பு &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    நிலையான type_T *
ret_list_or_dict_0(முழு argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    எனில் (argcount > 0)
	திரும்பு &t_dict_any;
    *decl_type = &t_list_any;
    திரும்பு &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    நிலையான type_T *
ret_list_or_dict_1(முழு argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    எனில் (argcount > 1)
	திரும்பு &t_dict_any;
    *decl_type = &t_list_any;
    திரும்பு &t_list_dict_any;
}

    நிலையான type_T *
ret_argv(முழு argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // argv() returns list of strings
    எனில் (argcount == 0)
    {
	*decl_type = &t_list_any;
	திரும்பு &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    திரும்பு &t_any;
}

    நிலையான type_T *
ret_remove(முழு argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    எனில் (argcount > 0)
    {
	எனில் (argtypes[0].type_curr->tt_type == VAR_LIST
		|| argtypes[0].type_curr->tt_type == VAR_DICT)
	{
	    எனில் (argcount == 3)
	    {
		*decl_type = argtypes[0].type_decl;
		திரும்பு argtypes[0].type_curr;
	    }
	    எனில் (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    திரும்பு argtypes[0].type_curr->tt_member;
	}
	எனில் (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    திரும்பு &t_எண்;
    }
    திரும்பு &t_any;
}

    நிலையான type_T *
ret_max_min(முழு argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    எனில் (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	எனில் (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	இல்லையெனில்
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	எனில் (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	இல்லையெனில்
	    t = &t_any;
	திரும்பு t;
    }
    திரும்பு &t_any;
}

    நிலையான type_T *
ret_getreg(முழு argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    எனில் (argcount == 3)
    {
	*decl_type = &t_list_any;
	திரும்பு &t_list_string;
    }
    திரும்பு &t_string;
}

    நிலையான type_T *
ret_virtcol(முழு argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    எனில் (argcount > 1)
    {
	*decl_type = &t_list_any;
	திரும்பு &t_list_எண்;
    }
    திரும்பு &t_எண்;
}

    நிலையான type_T *
ret_maparg(முழு argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    // Assume that if the fourth argument is passed it's non-zero
    எனில் (argcount == 4)
	திரும்பு &t_dict_any;
    திரும்பு &t_string;
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
வகைவரை கட்டமைப்பு
{
    எழுத்து	*f_name;	// function name
    எழுத்து	f_min_argc;	// minimal number of arguments
    எழுத்து	f_max_argc;	// maximal number of arguments
    எழுத்து	f_argtype;	// for method: FEARG_ values; bits FE_
    argcheck_T	*f_argcheck;	// list of functions to check argument types;
				// use "arg_any" (not NULL) to accept an
				// argument of any type
    எழுத்து	*f_argnames;	// names of the arguments as in the help,
				// separated by ", "; an argument without a
				// name is empty, NULL when there is none
    type_T	*(*f_retfunc)(முழு argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    வெறுமை	(*f_func)(typval_T *args, typval_T *rvar);
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

#எனில் defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#இல்லையெனில்
# define MATH_FUNC(name) வெற்று
#endif
#ifdef FEAT_TIMERS
# define TIMER_FUNC(name) name
#இல்லையெனில்
# define TIMER_FUNC(name) வெற்று
#endif
#ifdef FEAT_JOB_CHANNEL
# define JOB_FUNC(name) name
#இல்லையெனில்
# define JOB_FUNC(name) வெற்று
#endif
#ifdef FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#இல்லையெனில்
# define PROP_FUNC(name) வெற்று
#endif
#ifdef FEAT_SIGNS
# define SIGN_FUNC(name) name
#இல்லையெனில்
# define SIGN_FUNC(name) வெற்று
#endif
#ifdef FEAT_SOUND
# define SOUND_FUNC(name) name
#இல்லையெனில்
# define SOUND_FUNC(name) வெற்று
#endif
#ifdef FEAT_TERMINAL
# define TERM_FUNC(name) name
#இல்லையெனில்
# define TERM_FUNC(name) வெற்று
#endif
#ifdef FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#இல்லையெனில்
# define TABPANEL_FUNC(name) வெற்று
#endif
#ifdef FEAT_IMAGE
# define IMAGE_FUNC(name) name
#இல்லையெனில்
# define IMAGE_FUNC(name) வெற்று
#endif

நிலையான மாறிலி funcentry_T global_functions[] =
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
    {"and",		2, 2, FEARG_1,	    arg2_எண்,
			"expr1, expr2",
			ret_எண்,	    f_and},
    {"append",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_எண்_bool,    f_append},
    {"appendbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_எண்_bool,    f_appendbufline},
    {"argc",		0, 1, 0,	    arg1_எண்,
			"winid",
			ret_எண்,	    f_argc},
    {"argidx",		0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_argidx},
    {"arglistid",	0, 2, 0,	    arg2_எண்,
			"winnr, tabnr",
			ret_எண்,	    f_arglistid},
    {"argv",		0, 2, 0,	    arg2_எண்,
			"nr, winid",
			ret_argv,	    f_argv},
    {"asin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_asin},
    {"assert_beeps",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_எண்_bool,    f_assert_beeps},
    {"assert_equal",	2, 3, FEARG_2,	    வெற்று,
			"expected, actual, msg",
			ret_எண்_bool,    f_assert_equal},
    {"assert_equalfile", 2, 3, FEARG_1,	    arg3_string,
			"fname_one, fname_two, msg",
			ret_எண்_bool,    f_assert_equalfile},
    {"assert_exception", 1, 2, 0,	    arg2_string,
			"error, msg",
			ret_எண்_bool,    f_assert_exception},
    {"assert_fails",	1, 5, FEARG_1,	    arg15_assert_fails,
			"cmd, error, msg, lnum, context",
			ret_எண்_bool,    f_assert_fails},
    {"assert_false",	1, 2, FEARG_1,	    வெற்று,
			"actual, msg",
			ret_எண்_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_எண்_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_எண்_bool,    f_assert_match},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_எண்_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    வெற்று,
			"expected, actual, msg",
			ret_எண்_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_எண்_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_எண்_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    வெற்று,
			"actual, msg",
			ret_எண்_bool,    f_assert_true},
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
    {"balloon_gettext",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,
#ifdef FEAT_BEVAL
	    f_balloon_gettext
#இல்லையெனில்
	    வெற்று
#endif
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#ifdef FEAT_BEVAL
	    f_balloon_show
#இல்லையெனில்
	    வெற்று
#endif
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#எனில் defined(FEAT_BEVAL_TERM)
	    f_balloon_split
#இல்லையெனில்
	    வெற்று
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
			ret_list_எண்,    f_blob2list},
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
			ret_எண்,	    f_bufadd},
    {"bufexists",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_எண்_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_எண்_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_எண்,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_எண்_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_எண்_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, create",
			ret_எண்,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_எண்,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_எண்,	    f_bufwinnr},
    {"byte2line",	1, 1, FEARG_1,	    arg1_எண்,
			"byte",
			ret_எண்,	    f_byte2line},
    {"byteidx",		2, 3, FEARG_1,	    arg3_string_எண்_bool,
			"expr, nr, utf16",
			ret_எண்,	    f_byteidx},
    {"byteidxcomp",	2, 3, FEARG_1,	    arg3_string_எண்_bool,
			"expr, nr, utf16",
			ret_எண்,	    f_byteidxcomp},
    {"call",		2, 3, FEARG_1,	    arg3_any_list_dict,
			"func, arglist, dict",
			ret_any,	    f_call},
    {"ceil",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ceil},
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_job,
			"handle",
			ret_எண்_bool,    JOB_FUNC(f_ch_canread)},
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
			ret_எண்,	    JOB_FUNC(f_ch_getbufnr)},
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
    {"changenr",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_changenr},
    {"char2nr",		1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_எண்,	    f_char2nr},
    {"charclass",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_எண்,	    f_charclass},
    {"charcol",		1, 2, FEARG_1,	    arg2_string_or_list_எண்,
			"expr, winid",
			ret_எண்,	    f_charcol},
    {"charidx",		2, 4, FEARG_1,	    arg4_string_எண்_bool_bool,
			"string, idx, countcc, utf16",
			ret_எண்,	    f_charidx},
    {"chdir",		1, 2, FEARG_1,	    arg2_string,
			"dir, scope",
			ret_string,	    f_chdir},
    {"cindent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_எண்,	    f_cindent},
    {"clearmatches",	0, 1, FEARG_1,	    arg1_எண்,
			"win",
			ret_void,	    f_clearmatches},
    {"cmdcomplete_info",0, 0, 0,	    வெற்று,
			வெற்று,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_எண்,
			"expr, winid",
			ret_எண்,	    f_col},
    {"complete",	2, 2, FEARG_2,	    arg2_எண்_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_எண்,	    f_complete_add},
    {"complete_check",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_எண்_string,
			"msg, choices, default, type",
			ret_எண்,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    வெற்று,
			"expr",
			ret_copy,	    f_copy},
    {"cos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cos},
    {"cosh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cosh},
    {"count",		2, 4, FEARG_1,	    arg24_மொத்தம்,
			"comp, expr, ic, start",
			ret_எண்,	    f_மொத்தம்},
    {"cscope_connection",0,3, 0,	    arg3_எண்_string_string,
			"num, dbpath, prepend",
			ret_எண்,	    f_cscope_connection},
    {"cursor",		1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_எண்,	    f_cursor},
    {"debugbreak",	1, 1, FEARG_1,	    arg1_எண்,
			"pid",
			ret_எண்,
#ifdef MSWIN
	    f_debugbreak
#இல்லையெனில்
	    வெற்று
#endif
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_எண்_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_buffer_lnum_lnum,
			"buf, first, last",
			ret_எண்_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்_bool,    f_did_filetype},
    {"diff",		2, 3, FEARG_1,	    arg3_diff,
			"fromlist, tolist, options",
			ret_string_or_any_3,  f_diff},
    {"diff_filler",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_எண்,	    f_diff_filler},
    {"diff_hlID",	2, 2, FEARG_1,	    arg2_lnum_எண்,
			"lnum, col",
			ret_எண்,	    f_diff_hlID},
    {"digraph_get",	1, 1, FEARG_1,	    arg1_string,
			"chars",
			ret_string,	    f_digraph_get},
    {"digraph_getlist",	0, 1, FEARG_1,	    arg1_bool,
			"listall",
			ret_list_string_பொருட்கள், f_digraph_getlist},
    {"digraph_set",	2, 2, FEARG_1,	    arg2_string,
			"chars, digraph",
			ret_bool,	f_digraph_set},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    வெற்று,
			"expr",
			ret_எண்_bool,    f_empty},
    {"environ",		0, 0, 0,	    வெற்று,
			வெற்று,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    வெற்று,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்_bool,    f_eventhandler},
    {"executable",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_எண்,	    f_executable},
    {"execute",		1, 2, FEARG_1,	    arg12_execute,
			"command, silent",
			ret_string,	    f_execute},
    {"exepath",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_exepath},
    {"exists",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_எண்_bool,    f_exists},
    {"exists_compiled",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_எண்_bool,    f_exists_compiled},
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
			ret_எண்_bool,    f_filereadable},
    {"filecopy",	2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_எண்_bool,    f_filecopy},
    {"filereadable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_எண்_bool,    f_filereadable},
    {"filewritable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_எண்,	    f_filewritable},
    {"filter",		2, 2, FEARG_1,	    arg2_filter,
			"expr1, expr2",
			ret_first_arg,	    f_filter},
    {"finddir",		1, 3, FEARG_1,	    arg3_string_string_எண்,
			"name, path, count",
			ret_string_or_any_3,	    f_finddir},
    {"findfile",	1, 3, FEARG_1,	    arg3_string_string_எண்,
			"name, path, count",
			ret_string_or_any_3,	    f_findfile},
    {"flatten",		1, 2, FEARG_1,	    arg2_list_any_எண்,
			"list, maxdepth",
			ret_list_any,	    f_flatten},
    {"flattennew",	1, 2, FEARG_1,	    arg2_list_any_எண்,
			"list, maxdepth",
			ret_list_any,	    f_flattennew},
    {"float2nr",	1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_எண்,	    f_float2nr},
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
    {"foldclosed",	1, 2, FEARG_1,	    arg2_lnum_எண்,
			"lnum, winid",
			ret_எண்,	    f_foldclosed},
    {"foldclosedend",	1, 2, FEARG_1,	    arg2_lnum_எண்,
			"lnum, winid",
			ret_எண்,	    f_foldclosedend},
    {"foldlevel",	1, 2, FEARG_1,	    arg2_lnum_எண்,
			"lnum, winid",
			ret_எண்,	    f_foldlevel},
    {"foldtext",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_எண்,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_first_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    வெற்று,
			வெற்று,
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
    {"getbgcolor",	0, 0, 0,	    வெற்று,
			வெற்று,
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
    {"getcellpixels",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_எண்,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_getcmdwintype},
    {"getcompletion",	2, 3, FEARG_1,	    arg3_string_string_bool,
			"pat, type, filtered",
			ret_list_string,    f_getcompletion},
    {"getcompletiontype", 1, 1, FEARG_1,    arg1_string,
			"pat",
			ret_string,	    f_getcompletiontype},
    {"getcurpos",	0, 1, FEARG_1,	    arg1_எண்,
			"winid",
			ret_list_எண்,    f_getcurpos},
    {"getcursorcharpos", 0, 1, FEARG_1,	    arg1_எண்,
			"winid",
			ret_list_எண்,    f_getcursorcharpos},
    {"getcwd",		0, 2, FEARG_1,	    arg2_எண்,
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
			ret_எண்,	    f_getfsize},
    {"getftime",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_எண்,	    f_getftime},
    {"getftype",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getftype},
    {"getimstatus",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்_bool,    f_getimstatus},
    {"getinfo",		2, 3, FEARG_2,	    arg3_string_string_dict,
			"kind, name, opts",
			ret_dict_any,	    f_getinfo},
    {"getjumplist",	0, 2, FEARG_1,	    arg2_எண்,
			"winnr, tabnr",
			ret_list_any,	    f_getjumplist},
    {"getline",		1, 2, FEARG_1,	    arg2_lnum,
			"lnum, end",
			ret_getline,	    f_getline},
    {"getloclist",	1, 2, 0,	    arg2_எண்_dict_any,
			"nr, what",
			ret_list_or_dict_1, f_getloclist},
    {"getmarklist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_எண்,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_dict_எண்,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_getpid},
    {"getpos",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_எண்,    f_getpos},
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
    {"getstacktrace",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_list_dict_any,  f_getstacktrace},
    {"gettabinfo",	0, 1, FEARG_1,	    arg1_எண்,
			"tabnr",
			ret_list_dict_any,  f_gettabinfo},
    {"gettabvar",	2, 3, FEARG_1,	    arg3_எண்_string_any,
			"tabnr, varname, def",
			ret_any,	    f_gettabvar},
    {"gettabwinvar",	3, 4, FEARG_1,	    arg4_எண்_எண்_string_any,
			"tabnr, winnr, varname, def",
			ret_any,	    f_gettabwinvar},
    {"gettagstack",	0, 1, FEARG_1,	    arg1_எண்,
			"winnr",
			ret_dict_any,	    f_gettagstack},
    {"gettext",		1, 2, FEARG_1,	    arg2_string,
			"text, package",
			ret_string,	    f_gettext},
    {"getwininfo",	0, 1, FEARG_1,	    arg1_எண்,
			"winid",
			ret_list_dict_any,  f_getwininfo},
    {"getwinpos",	0, 1, FEARG_1,	    arg1_எண்,
			"timeout",
			ret_list_எண்,    f_getwinpos},
    {"getwinposx",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_getwinposy},
    {"getwinvar",	2, 3, FEARG_1,	    arg3_எண்_string_any,
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
			ret_எண்_bool,    f_has},
    {"has_key",		2, 2, FEARG_1,	    arg2_dict_any_string_or_nr,
			"dict, key",
			ret_எண்_bool,    f_has_சாவி},
    {"haslocaldir",	0, 2, FEARG_1,	    arg2_எண்,
			"winnr, tabnr",
			ret_எண்,	    f_haslocaldir},
    {"hasmapto",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"what, mode, abbr",
			ret_எண்_bool,    f_hasmapto},
    {"highlightID",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_எண்,	    f_hlID},
    {"highlight_exists",1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_எண்_bool,    f_hlexists},
    {"histadd",		2, 2, FEARG_2,	    arg2_string,
			"history, item",
			ret_எண்_bool,    f_histadd},
    {"histdel",		1, 2, FEARG_1,	    arg2_string_string_or_எண்,
			"history, item",
			ret_எண்_bool,    f_histdel},
    {"histget",		1, 2, FEARG_1,	    arg2_string_எண்,
			"history, index",
			ret_string,	    f_histget},
    {"histnr",		1, 1, FEARG_1,	    arg1_string,
			"history",
			ret_எண்,	    f_histnr},
    {"hlID",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_எண்,	    f_hlID},
    {"hlexists",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_எண்_bool,    f_hlexists},
    {"hlget",		0, 2, FEARG_1,	    arg2_string_bool,
			"name, resolve",
			ret_list_dict_any,  f_hlget},
    {"hlset",		1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_எண்_bool,    f_hlset},
    {"hostname",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    வெற்று,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_எண்,	    IMAGE_FUNC(f_image_add)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_எண்,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_எண்,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_எண்,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_index,
			"object, expr, start, ic",
			ret_எண்,	    f_index},
    {"indexof",		2, 3, FEARG_1,	    arg23_index,
			"object, expr, opts",
			ret_எண்,	    f_indexof},
    {"input",		1, 3, FEARG_1,	    arg3_string,
			"prompt, text, completion",
			ret_string,	    f_உள்ளீடு},
    {"inputdialog",	1, 3, FEARG_1,	    arg3_string,
			"prompt, text, cancelreturn",
			ret_string,	    f_inputdialog},
    {"inputlist",	1, 1, FEARG_1,	    arg1_list_string,
			"textlist",
			ret_எண்,	    f_inputlist},
    {"inputrestore",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_செருகு,
			"object, item, idx",
			ret_first_arg,	    f_செருகு},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_void,	    f_interrupt},
    {"invert",		1, 1, FEARG_1,	    arg1_எண்,
			"expr",
			ret_எண்,	    f_invert},
    {"isabsolutepath",	1, 1, FEARG_1,	    arg1_string,
			"path",
			ret_எண்_bool,    f_isabsolutepath},
    {"isdirectory",	1, 1, FEARG_1,	    arg1_string,
			"directory",
			ret_எண்_bool,    f_isdirectory},
    {"isinf",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_எண்,	    MATH_FUNC(f_isinf)},
    {"islocked",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_எண்_bool,    f_islocked},
    {"isnan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_எண்_bool,    MATH_FUNC(f_isnan)},
    {"items",		1, 1, FEARG_1,	    arg1_list_tuple_dict_blob_or_string,
			"expr",
			ret_list_பொருட்கள்,	    f_பொருட்கள்},
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
    {"job_stop",	1, 2, FEARG_1,	    arg2_job_string_or_எண்,
			"job, how",
			ret_எண்_bool,    JOB_FUNC(f_job_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    வெற்று,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    வெற்று,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_சாவிகள்},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    வெற்று,	// obsolete
			வெற்று,
			ret_எண்,	    f_last_buffer_nr},
    {"len",		1, 1, FEARG_1,	    arg1_len,
			"expr",
			ret_எண்,	    f_len},
    {"libcall",		3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_string,	    f_libcall},
    {"libcallnr",	3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_எண்,	    f_libcallnr},
    {"line",		1, 2, FEARG_1,	    arg2_string_எண்,
			"expr, winid",
			ret_எண்,	    f_வரி},
    {"line2byte",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_எண்,	    f_line2byte},
    {"lispindent",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_எண்,	    f_lispindent},
    {"list2blob",	1, 1, FEARG_1,	    arg1_list_எண்,
			"list",
			ret_blob,	    f_list2blob},
    {"list2str",	1, 2, FEARG_1,	    arg2_list_எண்_bool,
			"list, utf8",
			ret_string,	    f_list2str},
    {"list2tuple",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_tuple_any,	    f_list2tuple},
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_buffer_bool_or_dict,
			"callback, buf, options",
			ret_எண்,	    f_listener_add},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_எண்,
			"id",
			ret_எண்_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_localtime},
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
#இல்லையெனில்
		வெற்று
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
			ret_எண்,	    f_match},
    {"matchadd",	2, 5, FEARG_1,	    arg25_matchadd,
			"group, pattern, priority, id, dict",
			ret_எண்,	    f_matchadd},
    {"matchaddpos",	2, 5, FEARG_1,	    arg25_matchaddpos,
			"group, pos, priority, id, dict",
			ret_எண்,	    f_matchaddpos},
    {"matcharg",	1, 1, FEARG_1,	    arg1_எண்,
			"nr",
			ret_list_string,    f_matcharg},
    {"matchbufline",	4, 5, FEARG_1,	    arg45_matchbufline,
			"buf, pat, lnum, end, dict",
			ret_list_any,	    f_matchbufline},
    {"matchdelete",	1, 2, FEARG_1,	    arg2_எண்,
			"id, win",
			ret_எண்_bool,    f_matchdelete},
    {"matchend",	2, 4, FEARG_1,	    arg24_match_func,
			"expr, pat, start, count",
			ret_எண்,	    f_matchend},
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
#இல்லையெனில்
	    வெற்று
#endif
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_எண்,
			"name, flags, prot",
			ret_எண்_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_mode},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_MZSCHEME
	    f_mzeval
#இல்லையெனில்
	    வெற்று
#endif
			},
    {"nextnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_எண்,	    f_nextnonblank},
    {"ngettext",	3, 4, FEARG_3,	    arg4_string_string_எண்_string,
			"single, plural, number, domain",
			ret_string,	    f_ngettext},
    {"nr2char",		1, 2, FEARG_1,	    arg2_எண்_bool,
			"expr, utf8",
			ret_string,	    f_nr2char},
    {"or",		2, 2, FEARG_1,	    arg2_எண்,
			"expr1, expr2",
			ret_எண்,	    f_or},
    {"pathshorten",	1, 2, FEARG_1,	    arg2_string_எண்,
			"path, len",
			ret_string,	    f_pathshorten},
    {"perleval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_PERL
	    f_perleval
#இல்லையெனில்
	    வெற்று
#endif
			},
    {"popup_atcursor",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_எண்,	    PROP_FUNC(f_popup_atcursor)},
    {"popup_beval",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_எண்,	    PROP_FUNC(f_popup_beval)},
    {"popup_clear",	0, 1, 0,	    arg1_bool,
			"force",
			ret_void,	    PROP_FUNC(f_popup_clear)},
    {"popup_close",	1, 2, FEARG_1,	    arg2_எண்_any,
			"id, result",
			ret_void,	    PROP_FUNC(f_popup_close)},
    {"popup_create",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_எண்,	    PROP_FUNC(f_popup_create)},
    {"popup_dialog",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_எண்,	    PROP_FUNC(f_popup_dialog)},
    {"popup_filter_menu", 2, 2, 0,	    arg2_எண்_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_menu)},
    {"popup_filter_yesno", 2, 2, 0,	    arg2_எண்_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_yesno)},
    {"popup_findecho",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    PROP_FUNC(f_popup_findpreview)},
    {"popup_getoptions", 1, 1, FEARG_1,	    arg1_எண்,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getoptions)},
    {"popup_getpos",	1, 1, FEARG_1,	    arg1_எண்,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getpos)},
    {"popup_hide",	1, 1, FEARG_1,	    arg1_எண்,
			"id",
			ret_void,	    PROP_FUNC(f_popup_hide)},
    {"popup_list",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_list_எண்,    PROP_FUNC(f_popup_list)},
    {"popup_locate",	2, 2, 0,	    arg2_எண்,
			"row, col",
			ret_எண்,	    PROP_FUNC(f_popup_locate)},
    {"popup_menu",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_எண்,	    PROP_FUNC(f_popup_menu)},
    {"popup_move",	2, 2, FEARG_1,	    arg2_எண்_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_move)},
    {"popup_notification", 2, 2, FEARG_1,   arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_எண்,	    PROP_FUNC(f_popup_notification)},
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_எண்_buffer,
			"id, buf",
			ret_எண்_bool,    PROP_FUNC(f_popup_setbuf)},
    {"popup_setoptions", 2, 2, FEARG_1,	    arg2_எண்_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_setoptions)},
    {"popup_settext",	2, 2, FEARG_1,	    arg2_எண்_string_or_list,
			"id, text",
			ret_void,	    PROP_FUNC(f_popup_settext)},
    {"popup_show",	1, 1, FEARG_1,	    arg1_எண்,
			"id",
			ret_எண்,	    PROP_FUNC(f_popup_show)},
    {"pow",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"x, y",
			ret_float,	    f_pow},
    {"preinserted",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_எண்,	    f_prevnonblank},
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
    {"prop_add",	3, 3, FEARG_1,	    arg3_எண்_எண்_dict,
			"lnum, col, props",
			ret_எண்,	    PROP_FUNC(f_prop_add)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_add_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_எண்_எண்_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_find)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_எண்_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_எண்_எண்,
			"props, lnum, lnum_end",
			ret_எண்,	    PROP_FUNC(f_prop_remove)},
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
    {"pum_getpos",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_dict_எண்,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON3
	    f_py3eval
#இல்லையெனில்
	    வெற்று
#endif
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON
	    f_pyeval
#இல்லையெனில்
	    வெற்று
#endif
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#எனில் defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#இல்லையெனில்
	    வெற்று
#endif
			},
    {"rand",		0, 1, FEARG_1,	    arg1_list_எண்,
			"expr",
			ret_எண்,	    f_rand},
    {"range",		1, 3, FEARG_1,	    arg3_எண்,
			"expr, max, stride",
			ret_list_எண்,    f_range},
    {"readblob",	1, 3, FEARG_1,	    arg3_string_எண்_எண்,
			"fname, offset, size",
			ret_blob,	    f_readblob},
    {"readdir",		1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_string,    f_readdir},
    {"readdirex",	1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_dict_any,  f_readdirex},
    {"readfile",	1, 3, FEARG_1,	    arg3_string_string_எண்,
			"fname, type, max",
			ret_list_string,    f_readfile},
    {"redraw_listener_add", 1, 1, FEARG_1,  arg1_dict_any,
			"opts",
			ret_எண்,	    f_redraw_listener_add},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_எண்,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_reg_recording},
    {"reltime",		0, 2, FEARG_1,	    arg2_list_எண்,
			"start, end",
			ret_list_எண்,    f_reltime},
    {"reltimefloat",	1, 1, FEARG_1,	    arg1_list_எண்,
			"time",
			ret_float,	    f_reltimefloat},
    {"reltimestr",	1, 1, FEARG_1,	    arg1_list_எண்,
			"time",
			ret_string,	    f_reltimestr},
    {"remote_expr",	2, 4, FEARG_1,	    arg24_remote_கோவை,
			"server, string, idvar, timeout",
			ret_string,	    f_remote_கோவை},
    {"remote_foreground", 1, 1, FEARG_1,    arg1_string,
			"server",
			ret_void,	    f_remote_foreground},
    {"remote_peek",	1, 2, FEARG_1,	    arg2_string,
			"serverid, retvar",
			ret_எண்,	    f_remote_peek},
    {"remote_read",	1, 2, FEARG_1,	    arg2_string_எண்,
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
			ret_எண்_bool,    f_rename},
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
#இல்லையெனில்
	    வெற்று
#endif
			},
    {"screenattr",	2, 2, FEARG_1,	    arg2_எண்,
			"row, col",
			ret_எண்,	    f_screenattr},
    {"screenchar",	2, 2, FEARG_1,	    arg2_எண்,
			"row, col",
			ret_எண்,	    f_screenchar},
    {"screenchars",	2, 2, FEARG_1,	    arg2_எண்,
			"row, col",
			ret_list_எண்,    f_screenchars},
    {"screencol",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_எண்,
			"winid, lnum, col",
			ret_dict_எண்,    f_screenpos},
    {"screenrow",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_screenrow},
    {"screenstring",	2, 2, FEARG_1,	    arg2_எண்,
			"row, col",
			ret_string,	    f_screenstring},
    {"search",		1, 5, FEARG_1,	    arg15_தேடல்,
			"pattern, flags, stopline, timeout, skip",
			ret_எண்,	    f_தேடல்},
    {"searchcount",	0, 1, FEARG_1,	    arg1_dict_any,
			"options",
			ret_dict_எண்,    f_searchcount},
    {"searchdecl",	1, 3, FEARG_1,	    arg3_string_bool_bool,
			"name, global, thisblock",
			ret_எண்_bool,    f_searchdecl},
    {"searchpair",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_எண்,	    f_searchpair},
    {"searchpairpos",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_list_எண்,    f_searchpairpos},
    {"searchpos",	1, 5, FEARG_1,	    arg15_தேடல்,
			"pattern, flags, stopline, timeout, skip",
			ret_list_எண்,    f_searchpos},
    {"server2client",	2, 2, FEARG_1,	    arg2_string,
			"clientid, string",
			ret_எண்_bool,    f_server2client},
    {"serverlist",	0, 1, 0,	    arg1_dict_any,
			"dict",
			ret_string_or_any_1,	    f_serverlist},
    {"setbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_எண்_bool,    f_setbufline},
    {"setbufvar",	3, 3, FEARG_3,	    arg3_buffer_string_any,
			"buf, varname, val",
			ret_void,	    f_setbufvar},
    {"setcellwidths",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_void,	    f_setcellwidths},
    {"setcharpos",	2, 2, FEARG_2,	    arg2_string_list_எண்,
			"expr, list",
			ret_எண்_bool,    f_setcharpos},
    {"setcharsearch",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_setcharsearch},
    {"setcmdline",	1, 2, FEARG_1,	    arg2_string_எண்,
			"str, pos",
			ret_எண்_bool,    f_setcmdline},
    {"setcmdpos",	1, 1, FEARG_1,	    arg1_எண்,
			"pos",
			ret_எண்_bool,    f_setcmdpos},
    {"setcursorcharpos", 1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_எண்_bool,    f_setcursorcharpos},
    {"setenv",		2, 2, FEARG_2,	    arg2_string_any,
			"name, val",
			ret_void,	    f_setenv},
    {"setfperm",	2, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_எண்_bool,    f_setfperm},
    {"setline",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_எண்_bool,    f_setline},
    {"setloclist",	2, 4, FEARG_2,	    arg24_setloclist,
			"nr, list, action, what",
			ret_எண்_bool,    f_setloclist},
    {"setmatches",	1, 2, FEARG_1,	    arg2_list_any_எண்,
			"list, win",
			ret_எண்_bool,    f_setmatches},
    {"setpos",		2, 2, FEARG_2,	    arg2_string_list_எண்,
			"expr, list",
			ret_எண்_bool,    f_setpos},
    {"setqflist",	1, 3, FEARG_1,	    arg13_setqflist,
			"list, action, what",
			ret_எண்_bool,    f_setqflist},
    {"setreg",		2, 3, FEARG_2,	    arg3_string_any_string,
			"regname, value, options",
			ret_எண்_bool,    f_setreg},
    {"settabvar",	3, 3, FEARG_3,	    arg3_எண்_string_any,
			"tabnr, varname, val",
			ret_void,	    f_settabvar},
    {"settabwinvar",	4, 4, FEARG_4,	    arg4_எண்_எண்_string_any,
			"tabnr, winnr, varname, val",
			ret_void,	    f_settabwinvar},
    {"settagstack",	2, 3, FEARG_2,	    arg23_settagstack,
			"nr, dict, action",
			ret_எண்_bool,    f_settagstack},
    {"setwinvar",	3, 3, FEARG_3,	    arg3_எண்_string_any,
			"winnr, varname, val",
			ret_void,	    f_setwinvar},
    {"sha256",		1, 1, FEARG_1,	    arg1_string_or_blob,
			"expr",
			ret_string,
#ifdef FEAT_CRYPT
	    f_sha256
#இல்லையெனில்
	    வெற்று
#endif
			},
    {"shellescape",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, special",
			ret_string,	    f_shellescape},
    {"shiftwidth",	0, 1, FEARG_1,	    arg1_எண்,
			"col",
			ret_எண்,	    f_shiftwidth},
    {"sign_define",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"name, dict",
			ret_sign_define,	    SIGN_FUNC(f_sign_define)},
    {"sign_getdefined",	0, 1, FEARG_1,	    arg1_string,
			"name",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getdefined)},
    {"sign_getplaced",	0, 2, FEARG_1,	    arg02_sign_getplaced,
			"buf, dict",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getplaced)},
    {"sign_jump",	3, 3, FEARG_1,	    arg3_எண்_string_buffer,
			"id, group, buf",
			ret_எண்,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_place,
			"id, group, name, buf, dict",
			ret_எண்,	    SIGN_FUNC(f_sign_place)},
    {"sign_placelist",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_எண்,    SIGN_FUNC(f_sign_placelist)},
    {"sign_undefine",	0, 1, FEARG_1,	    arg1_string_or_list_string,
			"name",
			ret_எண்_bool,    SIGN_FUNC(f_sign_undefine)},
    {"sign_unplace",	1, 2, FEARG_1,	    arg2_string_dict,
			"group, dict",
			ret_எண்_bool,    SIGN_FUNC(f_sign_unplace)},
    {"sign_unplacelist", 1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_எண்,    SIGN_FUNC(f_sign_unplacelist)},
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
    {"sound_clear",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_void,	    SOUND_FUNC(f_sound_clear)},
    {"sound_playevent",	1, 2, FEARG_1,	    arg2_string_any,
			"name, callback",
			ret_எண்,	    SOUND_FUNC(f_sound_playevent)},
    {"sound_playfile",	1, 2, FEARG_1,	    arg2_string_any,
			"path, callback",
			ret_எண்,	    SOUND_FUNC(f_sound_playfile)},
    {"sound_stop",	1, 1, FEARG_1,	    arg1_எண்,
			"id",
			ret_void,	    SOUND_FUNC(f_sound_stop)},
    {"soundfold",	1, 1, FEARG_1,	    arg1_string,
			"word",
			ret_string,	    f_soundfold},
    {"spellbadword",	0, 1, FEARG_1,	    arg1_string,
			"sentence",
			ret_list_string,    f_spellbadword},
    {"spellsuggest",	1, 3, FEARG_1,	    arg3_string_எண்_bool,
			"word, max, capital",
			ret_list_string,    f_spellsuggest},
    {"split",		1, 3, FEARG_1,	    arg3_string_string_bool,
			"string, pattern, keepempty",
			ret_list_string,    f_split},
    {"sqrt",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sqrt},
    {"srand",		0, 1, FEARG_1,	    arg1_எண்,
			"expr",
			ret_list_எண்,    f_srand},
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
			ret_list_எண்,    f_str2list},
    {"str2nr",		1, 3, FEARG_1,	    arg3_string_எண்_bool,
			"string, base, quoted",
			ret_எண்,	    f_str2nr},
    {"strcharlen",	1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_எண்,	    f_strcharlen},
    {"strcharpart",	2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, skipcc",
			ret_string,	    f_strcharpart},
    {"strchars",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, skipcc",
			ret_எண்,	    f_strchars},
    {"strdisplaywidth",	1, 2, FEARG_1,	    arg2_string_எண்,
			"string, col",
			ret_எண்,	    f_strdisplaywidth},
    {"strftime",	1, 2, FEARG_1,	    arg2_string_எண்,
			"format, time",
			ret_string,
#ifdef HAVE_STRFTIME
	    f_strftime
#இல்லையெனில்
	    வெற்று
#endif
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_எண்,
			"str, index",
			ret_எண்,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_எண்,
			"haystack, needle, start",
			ret_எண்,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, வெற்று,
			"expr",
			ret_string,	    f_string},
    {"strlen",		1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_எண்,	    f_strlen},
    {"strpart",		2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, chars",
			ret_string,	    f_strpart},
    {"strptime",	2, 2, FEARG_1,	    arg2_string,
			"format, timestring",
			ret_எண்,
#ifdef HAVE_STRPTIME
	    f_strptime
#இல்லையெனில்
	    வெற்று
#endif
			},
    {"strridx",		2, 3, FEARG_1,	    arg3_string_string_எண்,
			"haystack, needle, start",
			ret_எண்,	    f_strridx},
    {"strtrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_strtrans},
    {"strutf16len",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, countcc",
			ret_எண்,	    f_strutf16len},
    {"strwidth",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_எண்,	    f_strwidth},
    {"submatch",	1, 2, FEARG_1,	    arg2_எண்_bool,
			"nr, list",
			ret_string_or_any_2,	    f_submatch},
    {"substitute",	4, 4, FEARG_1,	    arg4_string_string_any_string,
			"string, pat, sub, flags",
			ret_string,	    f_substitute},
    {"swapfilelist",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_swapname},
    {"synID",		3, 3, 0,	    arg3_lnum_எண்_bool,
			"lnum, col, trans",
			ret_எண்,	    f_synID},
    {"synIDattr",	2, 3, FEARG_1,	    arg3_எண்_string_string,
			"synID, what, mode",
			ret_string,	    f_synIDattr},
    {"synIDtrans",	1, 1, FEARG_1,	    arg1_எண்,
			"synID",
			ret_எண்,	    f_synIDtrans},
    {"synconcealed",	2, 2, 0,	    arg2_lnum_எண்,
			"lnum, col",
			ret_list_any,	    f_synconcealed},
    {"synstack",	2, 2, 0,	    arg2_lnum_எண்,
			"lnum, col",
			ret_list_எண்,    f_synstack},
    {"system",		1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_string,	    f_system},
    {"systemlist",	1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_list_string,    f_systemlist},
    {"tabpagebuflist",	0, 1, FEARG_1,	    arg1_எண்,
			"arg",
			ret_list_எண்,    f_tabpagebuflist},
    {"tabpagenr",	0, 1, 0,	    arg1_string,
			"arg",
			ret_எண்,	    f_tabpagenr},
    {"tabpagewinnr",	1, 2, FEARG_1,	    arg2_எண்_string,
			"tabarg, arg",
			ret_எண்,	    f_tabpagewinnr},
    {"tabpanel_getinfo", 0, 0, 0,	    வெற்று,
			வெற்று,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_எண்_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    வெற்று,
			வெற்று,
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
    {"tempname",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_எண்,	    TERM_FUNC(f_term_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_எண்,	    TERM_FUNC(f_term_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_buffer_string_dict,
			"buf, filename, options",
			ret_void,	    TERM_FUNC(f_term_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_buffer,
			"buf",
			ret_எண்,	    TERM_FUNC(f_term_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_buffer,
			"buf",
			ret_list_string,
#எனில் defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_getansicolors
#இல்லையெனில்
	    வெற்று
#endif
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_எண்_string,
			"attr, what",
			ret_எண்,	    TERM_FUNC(f_term_getattr)},
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
			ret_எண்,	    TERM_FUNC(f_term_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_எண்,    TERM_FUNC(f_term_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, input",
			ret_string,	    TERM_FUNC(f_term_gettty)},
    {"term_list",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_list_எண்,    TERM_FUNC(f_term_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_buffer_lnum,
			"buf, row",
			ret_list_dict_any,  TERM_FUNC(f_term_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_buffer_string,
			"buf, keys",
			ret_void,	    TERM_FUNC(f_term_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_buffer_list_any,
			"buf, colors",
			ret_void,
#எனில் defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_setansicolors
#இல்லையெனில்
	    வெற்று
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
    {"term_setsize",	3, 3, FEARG_1,	    arg3_buffer_எண்_எண்,
			"buf, rows, cols",
			ret_void,	    TERM_FUNC(f_term_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_எண்,	    TERM_FUNC(f_term_start)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_buffer_எண்,
			"buf, time",
			ret_void,	    TERM_FUNC(f_term_wait)},
    {"terminalprops",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_எண்,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    வெற்று,
			வெற்று,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   வெற்று,
			வெற்று,
			ret_void,	    f_test_garbagecollect_soon},
    {"test_getvalue",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_எண்,	    f_test_getvalue},
    {"test_gui_event",	2, 2, FEARG_1,	    arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_gui_event},
    {"test_ignore_error", 1, 1, FEARG_1,    arg1_string,
			"expr",
			ret_void,	    f_test_ignore_error},
    {"test_mswin_event", 2, 2, FEARG_1,     arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_mswin_event},
    {"test_null_blob",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    வெற்று,
			வெற்று,
			ret_channel,	    JOB_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    வெற்று,
			வெற்று,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_job,	    JOB_FUNC(f_test_null_job)},
    {"test_null_list",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    வெற்று,
			வெற்று,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_set},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_எண்,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, வெற்று,
			"expr",
			ret_எண்,	    f_test_refcount},
    {"test_setmouse",	2, 2, 0,	    arg2_எண்,
			"row, col",
			ret_void,	    f_test_setmouse},
    {"test_settime",	1, 1, FEARG_1,	    arg1_எண்,
			"expr",
			ret_void,	    f_test_settime},
    {"test_srand_seed",	0, 1, FEARG_1,	    arg1_எண்,
			"seed",
			ret_void,	    f_test_srand_seed},
    {"test_unknown",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_எண்,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_எண்_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_எண்_any_dict,
			"time, callback, options",
			ret_எண்,	    TIMER_FUNC(f_timer_start)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_எண்,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    வெற்று,
			வெற்று,
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
    {"trim",		1, 3, FEARG_1,	    arg3_string_string_எண்,
			"text, mask, dir",
			ret_string,	    f_trim},
    {"trunc",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_trunc},
    {"tuple2list",	1, 1, FEARG_1,	    arg1_tuple_any,
			"tuple",
			ret_list_any,	    f_tuple2list},
    {"type",		1, 1, FEARG_1|FE_X, வெற்று,
			"expr",
			ret_எண்,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, வெற்று,
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
    {"utf16idx",	2, 4, FEARG_1,	    arg4_string_எண்_bool_bool,
			"string, idx, countcc, charidx",
			ret_எண்,	    f_utf16idx},
    {"values",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_member,    f_மதிப்புகள்},
    {"virtcol",		1, 3, FEARG_1,	    arg3_string_or_list_bool_எண்,
			"expr, list, winid",
			ret_virtcol,	    f_virtcol},
    {"virtcol2col",	3, 3, FEARG_1,	    arg3_எண்,
			"winid, lnum, col",
			ret_எண்,	    f_virtcol2col},
    {"visualmode",	0, 1, 0,	    arg1_bool,
			"expr",
			ret_string,	    f_visualmode},
    {"wildmenumode",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_void,	    f_wildtrigger},
    {"win_execute",	2, 3, FEARG_2,	    arg23_win_execute,
			"id, command, silent",
			ret_string,	    f_win_execute},
    {"win_findbuf",	1, 1, FEARG_1,	    arg1_எண்,
			"bufnr",
			ret_list_எண்,    f_win_findbuf},
    {"win_getid",	0, 2, FEARG_1,	    arg2_எண்,
			"win, tab",
			ret_எண்,	    f_win_getid},
    {"win_gettype",	0, 1, FEARG_1,	    arg1_எண்,
			"nr",
			ret_string,	    f_win_gettype},
    {"win_gotoid",	1, 1, FEARG_1,	    arg1_எண்,
			"expr",
			ret_எண்_bool,    f_win_gotoid},
    {"win_id2tabwin",	1, 1, FEARG_1,	    arg1_எண்,
			"expr",
			ret_list_எண்,    f_win_id2tabwin},
    {"win_id2win",	1, 1, FEARG_1,	    arg1_எண்,
			"expr",
			ret_எண்,	    f_win_id2win},
    {"win_move_separator", 2, 2, FEARG_1,   arg2_எண்,
			"nr, offset",
			ret_எண்_bool,    f_win_move_separator},
    {"win_move_statusline", 2, 2, FEARG_1,  arg2_எண்,
			"nr, offset",
			ret_எண்_bool,    f_win_move_statusline},
    {"win_screenpos",	1, 1, FEARG_1,	    arg1_எண்,
			"nr",
			ret_list_எண்,    f_win_screenpos},
    {"win_splitmove",   2, 3, FEARG_1,	    arg3_எண்_எண்_dict,
			"nr, target, options",
			ret_எண்_bool,    f_win_splitmove},
    {"winbufnr",	1, 1, FEARG_1,	    arg1_எண்,
			"nr",
			ret_எண்,	    f_winbufnr},
    {"wincol",		0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_எண்,
			"nr",
			ret_எண்,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_எண்,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    வெற்று,
			வெற்று,
			ret_எண்,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_எண்,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_dict_எண்,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_எண்,
			"nr",
			ret_எண்,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    வெற்று,
			வெற்று,
			ret_dict_எண்,    f_wordcount},
    {"writefile",	2, 3, FEARG_1,	    arg23_writefile,
			"object, fname, flags",
			ret_எண்_bool,    f_writefile},
    {"xor",		2, 2, FEARG_1,	    arg2_எண்,
			"expr1, expr2",
			ret_எண்,	    f_xor},
};

/*
 * Return true if specified function allows a type as an argument.
 */
    நிலையான முழு
func_allows_type(முழு idx)
{
    திரும்பு (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, முழு idx)
{
    நிலையான முழு	intidx = -1;
    char_u	*name;

    எனில் (idx == 0)
	intidx = -1;
    எனில் (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	எனில் (name != வெற்று)
	{
	    எனில் (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_pattern, 2) == 0)
		திரும்பு cat_prefix_varname('g', name);
	    திரும்பு name;
	}
    }
    எனில் (++intidx < (முழு)ARRAY_LENGTH(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	எனில் (global_functions[intidx].f_func == வெற்று)
	    திரும்பு (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	எனில் (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	திரும்பு IObuff;
    }

    திரும்பு வெற்று;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_கோவை_name(expand_T *xp, முழு idx)
{
    நிலையான முழு	intidx = -1;
    char_u	*name;

    எனில் (idx == 0)
	intidx = -1;
    எனில் (intidx < 0)
    {
	name = get_function_name(xp, idx);
	எனில் (name != வெற்று)
	    திரும்பு name;
    }
    திரும்பு get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    நிலையான முழு
find_internal_func_opt(char_u *name, முழு implemented)
{
    முழு		first = 0;
    முழு		last;
    முழு		cmp;
    முழு		x;

    last = (முழு)ARRAY_LENGTH(global_functions) - 1;

    // Find the function name in the table. Binary search.
    போது (first <= last)
    {
	x = first + ((unsigned)(last - first) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	எனில் (cmp < 0)
	    last = x - 1;
	இல்லையெனில் எனில் (cmp > 0)
	    first = x + 1;
	இல்லையெனில் எனில் (implemented && global_functions[x].f_func == வெற்று)
	    நிறுத்து;
	இல்லையெனில்
	    திரும்பு x;
    }
    திரும்பு -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    முழு
find_internal_func(char_u *name)
{
    திரும்பு find_internal_func_opt(name, TRUE);
}

    முழு
has_internal_func(char_u *name)
{
    திரும்பு find_internal_func_opt(name, TRUE) >= 0;
}

    நிலையான முழு
has_internal_func_name(char_u *name)
{
    திரும்பு find_internal_func_opt(name, FALSE) >= 0;
}

    எழுத்து *
internal_func_name(முழு idx)
{
    திரும்பு global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    முழு
internal_func_check_arg_types(
	type2_T	*types,
	முழு	idx,
	முழு	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    எனில் (!(func_allows_type(idx)))
    {
	க்கு (முழு i = 0; i < argcount; ++i)
	    எனில் (check_type_is_மதிப்பு(types[i].type_curr) == FAIL)
		திரும்பு FAIL;
    }

    மாறிலி argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    எனில் (argchecks == வெற்று)
	திரும்பு OK;

    argcontext_T context;

    context.arg_மொத்தம் = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    க்கு (முழு i = 0; i < argcount && argchecks[i] != வெற்று; ++i)
    {
	context.arg_idx = i;
	எனில் (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    வெறுமை
internal_func_get_argcount(முழு idx, முழு *argcount, முழு *min_argcount)
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
	முழு	    idx,
	முழு	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = வெற்று;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    எனில் (*decl_type == வெற்று)
	*decl_type = ret;
    current_type_gap = வெற்று;
    திரும்பு ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    முழு
internal_func_is_map(முழு idx)
{
    திரும்பு global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    முழு
check_internal_func(முழு idx, முழு argcount)
{
    funcerror_T	    res;
    எழுத்து	    *name;

    எனில் (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    இல்லையெனில் எனில் (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    இல்லையெனில்
	திரும்பு global_functions[idx].f_argtype & FEARG_MASK;

    name = internal_func_name(idx);
    எனில் (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    இல்லையெனில்
	semsg(_(e_not_enough_arguments_for_function_str), name);
    திரும்பு -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    நிலையான முழு
check_args_for_type(முழு idx, முழு argcount, typval_T *argvars)
{
    எனில் (!func_allows_type(idx))
    {
	க்கு (முழு i = 0; i < argcount; ++i)
	    எனில் (check_typval_is_மதிப்பு(&argvars[i]) == FAIL)
		திரும்பு FAIL;
    }
    திரும்பு OK;
}

    funcerror_T
call_internal_func(
	char_u	    *name,
	முழு	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    முழு i;

    i = find_internal_func(name);
    எனில் (i < 0)
	திரும்பு FCERR_UNKNOWN;
    எனில் (argcount < global_functions[i].f_min_argc)
	திரும்பு FCERR_TOOFEW;
    எனில் (argcount > global_functions[i].f_max_argc)
	திரும்பு FCERR_TOOMANY;
    எனில் (check_args_for_type(i, argcount, argvars) == FAIL)
	திரும்பு FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    எனில் (in_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    திரும்பு FCERR_NONE;
}

    வெறுமை
call_internal_func_by_idx(
	முழு	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    எனில் (in_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_internal_method(
	char_u	    *name,
	முழு	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    முழு		fi;
    typval_T	argv[MAX_FUNC_ARGS + 1];

    fi = find_internal_func(name);
    எனில் (fi < 0)
	திரும்பு FCERR_UNKNOWN;
    எனில் ((global_functions[fi].f_argtype & FEARG_MASK) == 0)
	திரும்பு FCERR_NOTMETHOD;
    எனில் (argcount + 1 < global_functions[fi].f_min_argc)
	திரும்பு FCERR_TOOFEW;
    எனில் (argcount + 1 > global_functions[fi].f_max_argc)
	திரும்பு FCERR_TOOMANY;
    எனில் (check_args_for_type(fi, argcount, argvars) == FAIL)
	திரும்பு FCERR_OTHER;

    எனில் ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_2)
    {
	எனில் (argcount < 1)
	    திரும்பு FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	க்கு (முழு i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    இல்லையெனில் எனில் ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_3)
    {
	எனில் (argcount < 2)
	    திரும்பு FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	க்கு (முழு i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    இல்லையெனில் எனில் ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_4)
    {
	எனில் (argcount < 3)
	    திரும்பு FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	க்கு (முழு i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    இல்லையெனில்
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	க்கு (முழு i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    எனில் (check_args_for_type(fi, argcount + 1, argv) == FAIL)
	திரும்பு FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    திரும்பு FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    முழு
non_zero_arg(typval_T *argvars)
{
    திரும்பு ((argvars[0].v_type == VAR_எண்
		&& argvars[0].vval.v_எண் != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_எண் == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != வெற்று
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    நிலையான வெறுமை
f_and(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    rettv->vval.v_எண் = tv_get_எண்_chk(&argvars[0], வெற்று)
					& tv_get_எண்_chk(&argvars[1], வெற்று);
}

/*
 * "balloon_show()" function
 */
#ifdef FEAT_BEVAL
    நிலையான வெறுமை
f_balloon_gettext(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    எனில் (balloonEval == வெற்று)
	திரும்பு;

    எனில் (balloonEval->செய்தி == வெற்று)
	rettv->vval.v_string = வெற்று;
    இல்லையெனில்
	rettv->vval.v_string = vim_strsave(balloonEval->செய்தி);
}

    நிலையான வெறுமை
f_balloon_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (balloonEval == வெற்று)
	திரும்பு;

    எனில் (in_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, வெற்று,
		l == வெற்று || l->lv_len == 0 ? வெற்று : l);
    }
    இல்லையெனில்
    {
	char_u *mesg;

	எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    திரும்பு;

	mesg = tv_get_string_chk(&argvars[0]);
	எனில் (mesg != வெற்று)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? வெற்று : mesg, வெற்று);
    }
}

# எனில் defined(FEAT_BEVAL_TERM)
    நிலையான வெறுமை
f_balloon_split(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (rettv_list_alloc(rettv) != OK)
	திரும்பு;

    char_u *செய்தி;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;
    செய்தி = tv_get_string_chk(&argvars[0]);
    எனில் (செய்தி != வெற்று)
    {
	pumitem_T	*array;
	முழு		size = split_message(செய்தி, &array);

	// Skip the first and last item, they are always empty.
	க்கு (முழு i = 1; i < size - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_text, -1);
	போது (size > 0)
	    vim_free(array[--size].pum_text);
	vim_free(array);
    }
}
# endif
#endif

/*
 * "base64_decode(string)" function
 */
    நிலையான வெறுமை
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    எனில் (check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (rettv_blob_alloc(rettv) == FAIL)
	திரும்பு;

    char_u *str = tv_get_string_chk(&argvars[0]);
    எனில் (str != வெற்று)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    நிலையான வெறுமை
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    எனில் (check_for_blob_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    blob_T *blob = argvars->vval.v_blob;
    எனில் (blob != வெற்று)
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
    எனில் (buf == வெற்று)
	semsg(_(e_invalid_buffer_name_str), tv_get_string(arg));
    திரும்பு buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    நிலையான வெறுமை
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_எண் = VVAL_TRUE;

    எனில் (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	திரும்பு;

    எனில் (strcmp((மாறிலி எழுத்து *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    இல்லையெனில்
    {
	எனில் (bindtextdomain((மாறிலி எழுத்து *)argvars[0].vval.v_string, (மாறிலி எழுத்து *)argvars[1].vval.v_string) == வெற்று)
	{
	    do_outofmem_செய்தி((long)0);
	    rettv->vval.v_எண் = VVAL_FALSE;
	}
    }

    திரும்பு;
}

/*
 * "byte2line(byte)" function
 */
    நிலையான வெறுமை
f_byte2line(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_எண் = -1;
#இல்லையெனில்
    long	boff = 0;

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    boff = tv_get_எண்(&argvars[0]) - 1;  // boff gets -1 on type error
    எனில் (boff < 0)
	rettv->vval.v_எண் = -1;
    இல்லையெனில்
	rettv->vval.v_எண் = ml_find_வரி_or_offset(curbuf,
							  (linenr_T)0, &boff);
#endif
}

/*
 * "call(func, arglist [, dict])" function
 */
    நிலையான வெறுமை
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = வெற்று;
    dict_T	*selfdict = வெற்று;
    char_u	*tofree = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	திரும்பு;

    எனில் (check_for_list_arg(argvars, 1) == FAIL)
	திரும்பு;
    எனில் (argvars[1].vval.v_list == வெற்று)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    இல்லையெனில்
	func = tv_get_string(&argvars[0]);
    எனில் (func == வெற்று || *func == NUL)
	திரும்பு;		// type error, empty name or null function

    எனில் (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, வெற்று, FALSE, TFN_INT|TFN_QUIET);
	எனில் (tofree == வெற்று)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    திரும்பு;
	}
	எனில் (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_args(tofree, STRLEN(tofree),
									&p);
	    எனில் (s != வெற்று)
	    {
		vim_free(tofree);
		tofree = s;
	    }
	}
	func = tofree;
    }

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	எனில் (check_for_dict_arg(argvars, 2) == FAIL)
	    செல் done;

	selfdict = argvars[2].vval.v_dict;
    }

    (வெறுமை)func_call(func, &argvars[1], partial, selfdict, rettv);

done:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    நிலையான வெறுமை
f_changenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_எண் = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    நிலையான வெறுமை
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (has_mbyte)
    {
	முழு	utf8 = 0;

	எனில் (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (முழு)tv_get_bool_chk(&argvars[1], வெற்று);

	எனில் (utf8)
	    rettv->vval.v_எண் = utf_ptr2char(tv_get_string(&argvars[0]));
	இல்லையெனில்
	    rettv->vval.v_எண் = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    இல்லையெனில்
	rettv->vval.v_எண் = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    நிலையான வெறுமை
get_col(typval_T *argvars, typval_T *rettv, முழு charcol)
{
    colnr_T	col = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    முழு		winchanged = FALSE;

    எனில் (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_எண்_arg(argvars, 1) == FAIL)
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((முழு)tv_get_எண்(&argvars[1]), &tp);
	எனில் (wp == வெற்று || tp == வெற்று)
	    திரும்பு;

	எனில் (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    திரும்பு;

	check_cursor();
	winchanged = TRUE;
    }

    முழு fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    எனில் (fp != வெற்று && fnum == curbuf->b_fnum)
    {
	எனில் (fp->col == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    எனில் (fp->lnum <= curbuf->b_ml.ml_வரி_மொத்தம்)
		col = ml_get_len(fp->lnum) + 1;
	    இல்லையெனில்
		col = MAXCOL;
	}
	இல்லையெனில்
	{
	    col = fp->col + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    எனில் (virtual_active() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		எனில் (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    முழு		l;

		    எனில் (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			col += l;
		}
	    }
	}
    }
    rettv->vval.v_எண் = col;

    எனில் (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    நிலையான வெறுமை
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, TRUE);
}

    win_T *
get_optional_window(typval_T *argvars, முழு idx)
{
    win_T   *win = curwin;

    எனில் (argvars[idx].v_type == VAR_UNKNOWN)
	திரும்பு curwin;

    win = find_win_by_nr_or_id(&argvars[idx]);
    எனில் (win == வெற்று)
    {
	emsg(_(e_invalid_window_எண்));
	திரும்பு வெற்று;
    }
    திரும்பு win;
}

/*
 * "col(string)" function
 */
    நிலையான வெறுமை
f_col(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    நிலையான வெறுமை
f_confirm(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*message;
    char_u	*buttons = வெற்று;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    முழு		def = 1;
    முழு		type = VIM_GENERIC;
    char_u	*typestr;
    முழு		error = FALSE;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_எண்_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	திரும்பு;

    message = tv_get_string_chk(&argvars[0]);
    எனில் (message == வெற்று)
	error = TRUE;
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	எனில் (buttons == வெற்று)
	    error = TRUE;
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (முழு)tv_get_எண்_chk(&argvars[2], &error);
	    எனில் (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		எனில் (typestr == வெற்று)
		    error = TRUE;
		இல்லையெனில்
		{
		    தேர்வு (TOUPPER_ASC(*typestr))
		    {
			நிலை 'E': type = VIM_ERROR; நிறுத்து;
			நிலை 'Q': type = VIM_QUESTION; நிறுத்து;
			நிலை 'I': type = VIM_INFO; நிறுத்து;
			நிலை 'W': type = VIM_WARNING; நிறுத்து;
			நிலை 'G': type = VIM_GENERIC; நிறுத்து;
		    }
		}
	    }
	}
    }

    எனில் (buttons == வெற்று || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    எனில் (!error)
	rettv->vval.v_எண் = do_dialog(type, வெற்று, message, buttons,
							    def, வெற்று, FALSE);
#endif
}

/*
 * "copy()" function
 */
    நிலையான வெறுமை
f_copy(typval_T *argvars, typval_T *rettv)
{
    item_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    நிலையான வெறுமை
set_cursorpos(typval_T *argvars, typval_T *rettv, முழு charcol)
{
    long	lnum, col;
    long	coladd = 0;
    முழு		set_curswant = TRUE;

    எனில் (in_vim9script()
	    && (check_for_string_or_எண்_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 2) == FAIL)))
	திரும்பு;

    rettv->vval.v_எண் = -1;
    எனில் (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	எனில் (list2fpos(argvars, &pos, வெற்று, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_argument));
	    திரும்பு;
	}
	lnum = pos.lnum;
	col = pos.col;
	coladd = pos.coladd;
	எனில் (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    set_curswant = FALSE;
	}
    }
    இல்லையெனில் எனில் ((argvars[0].v_type == VAR_எண்
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_எண்
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	எனில் (lnum < 0)
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
	இல்லையெனில் எனில் (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	col = (long)tv_get_எண்_chk(&argvars[1], வெற்று);
	எனில் (charcol)
	    col = buf_charidx_to_byteidx(curbuf, lnum, col) + 1;
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (long)tv_get_எண்_chk(&argvars[2], வெற்று);
    }
    இல்லையெனில்
    {
	emsg(_(e_invalid_argument));
	திரும்பு;
    }
    எனில் (lnum < 0 || col < 0 || coladd < 0)
	திரும்பு;		// type error; errmsg already given
    எனில் (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    எனில் (col != MAXCOL && --col < 0)
	col = 0;
    curwin->w_cursor.col = col;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    எனில் (has_mbyte)
	mb_adjust_cursor();

    curwin->w_set_curswant = set_curswant;
    rettv->vval.v_எண் = 0;
}

/*
 * "cursor(lnum, col)" function, or
 * "cursor(list)"
 *
 * Moves the cursor to the specified line and column.
 * Returns 0 when the position could be set, -1 otherwise.
 */
    நிலையான வெறுமை
f_cursor(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, FALSE);
}

#ifdef MSWIN
/*
 * "debugbreak()" function
 */
    நிலையான வெறுமை
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    முழு		pid;

    rettv->vval.v_எண் = FAIL;
    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    pid = (முழு)tv_get_எண்(&argvars[0]);
    எனில் (pid == 0)
    {
	emsg(_(e_invalid_argument));
	திரும்பு;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    எனில் (hProcess == வெற்று)
	திரும்பு;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_எண் = OK;
}
#endif

/*
 * "deepcopy()" function
 */
    நிலையான வெறுமை
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    எனில் (check_for_opt_bool_arg(argvars, 1) == FAIL)
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], வெற்று);

    item_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    நிலையான வெறுமை
f_did_filetype(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->vval.v_எண் = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    நிலையான வெறுமை
f_echoraw(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *str;

    எனில் (check_secure())
	திரும்பு;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    str = tv_get_string_chk(&argvars[0]);
    எனில் (str != வெற்று && *str != NUL)
    {
	out_str(str);
	out_flush();
    }
}

/*
 * "empty({expr})" function
 */
    நிலையான வெறுமை
f_empty(typval_T *argvars, typval_T *rettv)
{
    முழு		n = FALSE;

    தேர்வு (argvars[0].v_type)
    {
	நிலை VAR_STRING:
	நிலை VAR_FUNC:
	    n = argvars[0].vval.v_string == வெற்று
					  || *argvars[0].vval.v_string == NUL;
	    நிறுத்து;
	நிலை VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == வெற்று;
	    நிறுத்து;
	நிலை VAR_எண்:
	    n = argvars[0].vval.v_எண் == 0;
	    நிறுத்து;
	நிலை VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    நிறுத்து;
	நிலை VAR_LIST:
	    n = argvars[0].vval.v_list == வெற்று
					|| argvars[0].vval.v_list->lv_len == 0;
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    நிறுத்து;
	நிலை VAR_DICT:
	    n = argvars[0].vval.v_dict == வெற்று
			|| argvars[0].vval.v_dict->dv_hashtab.ht_used == 0;
	    நிறுத்து;
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	    n = argvars[0].vval.v_எண் != VVAL_TRUE;
	    நிறுத்து;
	நிலை VAR_CLASS:
	    n = argvars[0].vval.v_class != வெற்று;
	    நிறுத்து;
	நிலை VAR_OBJECT:
	    n = object_empty(argvars[0].vval.v_object);
	    நிறுத்து;

	நிலை VAR_BLOB:
	    n = argvars[0].vval.v_blob == வெற்று
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    நிறுத்து;

	நிலை VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_job == வெற்று
			   || argvars[0].vval.v_job->jv_status != JOB_STARTED;
	    நிறுத்து;
#endif
	நிலை VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_channel == வெற்று
			       || !channel_is_open(argvars[0].vval.v_channel);
	    நிறுத்து;
#endif
	நிலை VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == வெற்று
		|| argvars[0].vval.v_typealias->ta_name == வெற்று
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    நிறுத்து;

	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	நிலை VAR_INSTR:
	    internal_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    நிறுத்து;
    }

    rettv->vval.v_எண் = n;
}

/*
 * "environ()" function
 */
    நிலையான வெறுமை
f_environ(typval_T *argvars UNUSED, typval_T *rettv)
{
#எனில் !defined(AMIGA)
    முழு			i = 0;
    char_u		*entry, *மதிப்பு;
# எனில் defined (MSWIN)
#  எனில் !defined(_UCRT)
    வெளி wchar_t	**_wenviron;
#  endif
# இல்லையெனில்
    வெளி எழுத்து		**environ;
# endif

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;

# ifdef MSWIN
    எனில் (*_wenviron == வெற்று)
	திரும்பு;
# இல்லையெனில்
    எனில் (*environ == வெற்று)
	திரும்பு;
# endif

    க்கு (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	எனில் ((p = (short_u *)_wenviron[i]) == வெற்று)
	    திரும்பு;
	entry = utf16_to_enc(p, வெற்று);
# இல்லையெனில்
	எனில் ((entry = (char_u *)environ[i]) == வெற்று)
	    திரும்பு;
	entry = vim_strsave(entry);
# endif
	எனில் (entry == வெற்று) // out of memory
	    திரும்பு;
	எனில் ((மதிப்பு = vim_strchr(entry, '=')) == வெற்று)
	{
	    vim_free(entry);
	    தொடர்;
	}
	*மதிப்பு++ = NUL;
	dict_add_string(rettv->vval.v_dict, (எழுத்து *)entry, மதிப்பு);
	vim_free(entry);
    }
#endif
}

/*
 * "err_teapot()" function
 */
    நிலையான வெறுமை
f_err_teapot(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	எனில் (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    எனில் (*skipwhite(s) == NUL)
		திரும்பு;
	}

	முழு err = FALSE;
	முழு do_503 = eval_கோவை_to_bool(&argvars[0], &err);
	எனில் (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_available));
	    திரும்பு;
	}
    }

    emsg(_(e_im_a_teapot));
}

/*
 * "escape({string}, {chars})" function
 */
    நிலையான வெறுமை
f_escape(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    rettv->vval.v_string = vim_strsave_escaped(tv_get_string(&argvars[0]),
					 tv_get_string_buf(&argvars[1], buf));
    rettv->v_type = VAR_STRING;
}

/*
 * "eval()" function
 */
    நிலையான வெறுமை
f_eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*s, *p;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    s = tv_get_string_chk(&argvars[0]);
    எனில் (s != வெற்று)
	s = skipwhite(s);

    p = s;
    எனில் (s == வெற்று || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	எனில் (p != வெற்று && !aborting())
	    semsg(_(e_invalid_expression_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_எண்;
	rettv->vval.v_எண் = 0;
    }
    இல்லையெனில் எனில் (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    நிலையான வெறுமை
f_eventhandler(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_எண் = vgetc_busy || உள்ளீடு_busy;
}

நிலையான garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    வெறுமை
execute_redir_str(char_u *மதிப்பு, முழு மதிப்பு_len)
{
    முழு		len;

    எனில் (மதிப்பு_len == -1)
	len = (முழு)STRLEN(மதிப்பு);	// Append the entire string
    இல்லையெனில்
	len = மதிப்பு_len;		// Append only "value_len" characters
    எனில் (ga_grow(&redir_execute_ga, len) == FAIL)
	திரும்பு;

    mch_memmove((எழுத்து *)redir_execute_ga.ga_data
	    + redir_execute_ga.ga_len, மதிப்பு, len);
    redir_execute_ga.ga_len += len;
}

#எனில் defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    நிலையான char_u *
get_str_வரி(
    முழு	    c UNUSED,
    வெறுமை    *cookie,
    முழு	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    char_u	*start = *(char_u **)cookie;
    char_u	*வரி;
    char_u	*p;

    p = start;
    எனில் (p == வெற்று || *p == NUL)
	திரும்பு வெற்று;
    p = vim_strchr(p, '\n');
    எனில் (p == வெற்று)
	வரி = vim_strsave(start);
    இல்லையெனில்
    {
	வரி = vim_strnsave(start, p - start);
	p++;
    }

    *(char_u **)cookie = p;
    திரும்பு வரி;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    வெறுமை
execute_cmds_from_string(char_u *str)
{
    do_cmdline(வெற்று, get_str_வரி, (வெறுமை *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#endif

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_வரி(
    முழு	    c UNUSED,
    வெறுமை    *cookie,
    முழு	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *item = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    எனில் (item == வெற்று)
	திரும்பு வெற்று;
    s = tv_get_string_buf_chk(&item->li_tv, buf);
    *p = item->li_next;
    திரும்பு s == வெற்று ? வெற்று : vim_strsave(s);
}

/*
 * "execute()" function
 */
    வெறுமை
execute_common(typval_T *argvars, typval_T *rettv, முழு arg_off)
{
    char_u	*cmd = வெற்று;
    list_T	*list = வெற்று;
    முழு		save_செய்தி_silent = செய்தி_silent;
    முழு		save_emsg_silent = emsg_silent;
    முழு		save_emsg_noredir = emsg_noredir;
    முழு		save_redir_execute = redir_execute;
    முழு		save_redir_off = redir_off;
    garray_T	save_ga;
    முழு		save_செய்தி_col = செய்தி_col;
    முழு		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    முழு		echo_output = FALSE;

    rettv->vval.v_string = வெற்று;
    rettv->v_type = VAR_STRING;

    எனில் (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	எனில் (list == வெற்று || list->lv_len == 0)
	    // empty list, no commands, empty output
	    திரும்பு;
	++list->lv_refcount;
    }
    இல்லையெனில் எனில் (argvars[arg_off].v_type == VAR_JOB
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_மதிப்பு_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	திரும்பு;
    }
    இல்லையெனில்
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	எனில் (cmd == வெற்று)
	    திரும்பு;
    }

    எனில் (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      in_vim9script());

	எனில் (s == வெற்று)
	    திரும்பு;
	எனில் (*s == NUL)
	    echo_output = TRUE;
	எனில் (STRNCMP(s, "silent", 6) == 0)
	    ++செய்தி_silent;
	எனில் (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    இல்லையெனில்
	++செய்தி_silent;

    எனில் (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, அளவு(எழுத்து), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    எனில் (!echo_output)
	செய்தி_col = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    எனில் (cmd != வெற்று)
	do_cmdline_cmd(cmd);
    இல்லையெனில்
    {
	listitem_T	*item;

	CHECK_LIST_MATERIALIZE(list);
	item = list->lv_first;
	do_cmdline(வெற்று, get_list_வரி, (வெறுமை *)&item,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    எனில் (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((எழுத்து *)redir_execute_ga.ga_data)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_data;
    }
    இல்லையெனில்
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = வெற்று;
    }
    செய்தி_silent = save_செய்தி_silent;
    emsg_silent = save_emsg_silent;
    emsg_noredir = save_emsg_noredir;

    redir_execute = save_redir_execute;
    எனில் (redir_execute)
	redir_execute_ga = save_ga;
    redir_off = save_redir_off;

    // "silent reg" or "silent echo x" leaves msg_col somewhere in the line.
    எனில் (echo_output)
	// When not working silently: put it in column zero.  A following
	// "echon" will overwrite the message, unavoidably.
	செய்தி_col = 0;
    இல்லையெனில்
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	செய்தி_col = save_செய்தி_col;
}

/*
 * "execute()" function
 */
    நிலையான வெறுமை
f_execute(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    வெறுமை
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    முழு		n = FALSE;

    எனில் (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    p = tv_get_string(&argvars[0]);
    எனில் (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	எனில் (mch_getenv(p + 1) != வெற்று)
	    n = TRUE;
	இல்லையெனில்
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    எனில் (p != வெற்று && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    இல்லையெனில் எனில் (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, வெற்று, TRUE) == OK);
	எனில் (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    இல்லையெனில் எனில் (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    இல்லையெனில் எனில் (*p == '?')			// internal function only
    {
	n = has_internal_func_name(p + 1);
    }
    இல்லையெனில் எனில் (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    இல்லையெனில் எனில் (*p == '#')
    {
	எனில் (p[1] == '#')
	    n = autocmd_supported(p + 2);
	இல்லையெனில்
	    n = au_exists(p + 1);
    }
    இல்லையெனில்				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_எண் = n;
}

    நிலையான வெறுமை
f_exists_compiled(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    emsg(_(e_exists_compiled_can_only_be_used_in_def_function));
}

/*
 * "expand()" function
 */
    நிலையான வெறுமை
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    size_t	len;
    முழு		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    முழு		error = FALSE;
    char_u	*result;
#ifdef BACKSLASH_IN_FILENAME
    char_u	*p_csl_save = p_csl;
#endif

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	திரும்பு;

#ifdef BACKSLASH_IN_FILENAME
    // avoid using 'completeslash' here
    p_csl = empty_option;
#endif

    rettv->v_type = VAR_STRING;
    எனில் (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_set(rettv, வெற்று);

    s = tv_get_string(&argvars[0]);
    எனில் (*s == '%' || *s == '#' || *s == '<')
    {
	எழுத்து	*errormsg = வெற்று;

	எனில் (p_verbose == 0)
	    ++emsg_off;
	result = eval_vars(s, s, &len, வெற்று, &errormsg, வெற்று, FALSE);
	எனில் (p_verbose == 0)
	    --emsg_off;
	இல்லையெனில் எனில் (errormsg != வெற்று)
	    emsg(errormsg);
	எனில் (rettv->v_type == VAR_LIST)
	{
	    எனில் (rettv_list_alloc(rettv) == OK && result != வெற்று)
		list_append_string(rettv->vval.v_list, result, -1);
	    vim_free(result);
	}
	இல்லையெனில்
	    rettv->vval.v_string = result;
    }
    இல்லையெனில்
    {
	// When the optional second argument is non-zero, don't remove matches
	// for 'wildignore' and don't put matches for 'suffixes' at the end.
	எனில் (argvars[1].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	எனில் (!error)
	{
	    ExpandInit(&xpc);
	    xpc.xp_context = EXPAND_FILES;
	    எனில் (p_wic)
		options += WILD_ICASE;
	    எனில் (rettv->v_type == VAR_STRING)
		rettv->vval.v_string = ExpandOne(&xpc, s, வெற்று,
							   options, WILD_ALL);
	    இல்லையெனில் எனில் (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, வெற்று, options, WILD_ALL_KEEP);
		க்கு (முழு i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	இல்லையெனில்
	    rettv->vval.v_string = வெற்று;
    }
#ifdef BACKSLASH_IN_FILENAME
    p_csl = p_csl_save;
#endif
}

/*
 * "expandcmd()" function
 * Expand all the special characters in a command string.
 */
    நிலையான வெறுமை
f_expandcmd(typval_T *argvars, typval_T *rettv)
{
    exarg_T	eap;
    char_u	*cmdstr;
    எழுத்து	*errormsg = வெற்று;
    முழு		emsgoff = TRUE;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_FIELD(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = வெற்று;
    eap.cmdidx = CMD_USER;

    எனில் (emsgoff)
	++emsg_off;
    எனில் (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	எனில் (!emsgoff && errormsg != வெற்று && *errormsg != NUL)
	    emsg(errormsg);
    எனில் (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    நிலையான வெறுமை
f_feedkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    முழு		remap = TRUE;
    முழு		செருகு = FALSE;
    char_u	*சாவிகள், *flags;
    char_u	nbuf[NUMBUFLEN];
    முழு		typed = FALSE;
    முழு		execute = FALSE;
    முழு		context = FALSE;
    முழு		dangerous = FALSE;
    முழு		lowlevel = FALSE;

    // This is not allowed in the sandbox.  If the commands would still be
    // executed in the sandbox it would be OK, but it probably happens later,
    // when "sandbox" is no longer set.
    எனில் (check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    சாவிகள் = tv_get_string(&argvars[0]);

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	க்கு ( ; *flags != NUL; ++flags)
	{
	    தேர்வு (*flags)
	    {
		நிலை 'n': remap = FALSE; நிறுத்து;
		நிலை 'm': remap = TRUE; நிறுத்து;
		நிலை 't': typed = TRUE; நிறுத்து;
		நிலை 'i': செருகு = TRUE; நிறுத்து;
		நிலை 'x': execute = TRUE; நிறுத்து;
		நிலை 'c': context = TRUE; நிறுத்து;
		நிலை '!': dangerous = TRUE; நிறுத்து;
		நிலை 'L': lowlevel = TRUE; நிறுத்து;
	    }
	}
    }

    எனில் (*சாவிகள் != NUL || execute)
    {
	எனில் (lowlevel
#ifdef FEAT_VTP
		&& (!is_term_win32()
		    || (சாவிகள்[0] == 3 && ctrl_c_interrupts && typed))
#endif
	   )
	{
#ifdef USE_உள்ளீடு_BUF
	    ch_log(வெற்று, "feedkeys() lowlevel: %s", சாவிகள்);

	    முழு len = (முழு)STRLEN(சாவிகள்);
	    க்கு (முழு idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		எனில் (சாவிகள்[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		add_to_உள்ளீடு_buf(சாவிகள் + idx, 1);
	    }
#இல்லையெனில்
	    emsg(_(e_lowlevel_உள்ளீடு_not_supported));
#endif
	}
	இல்லையெனில்
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *சாவிகள்_esc = vim_strsave_escape_csi(சாவிகள்);
	    எனில் (சாவிகள்_esc == வெற்று)
		திரும்பு;

	    ch_log(வெற்று, "feedkeys(%s): %s", typed ? "typed" : "", சாவிகள்);

	    ins_typebuf(சாவிகள்_esc, (remap ? REMAP_YES : REMAP_NONE),
				   செருகு ? 0 : typebuf.tb_len, !typed, FALSE);
	    எனில் (vgetc_busy
#ifdef FEAT_TIMERS
		    || timer_busy
#endif
		    || உள்ளீடு_busy)
		typebuf_was_filled = TRUE;

	    vim_free(சாவிகள்_esc);
	}

	எனில் (execute)
	{
	    முழு		save_செய்தி_scroll = செய்தி_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    செய்தி_scroll = FALSE;

	    ch_log(வெற்று, "feedkeys() executing");

	    எனில் (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_version = 0;
	    }

	    எனில் (!dangerous)
	    {
		++ex_normal_busy;
		++in_feedkeys;
	    }
	    ++allow_osc_சாவி;
	    exec_normal(TRUE, lowlevel, TRUE);
	    --allow_osc_சாவி;
	    எனில் (!dangerous)
	    {
		--ex_normal_busy;
		--in_feedkeys;
	    }

	    செய்தி_scroll |= save_செய்தி_scroll;

	    எனில் (context)
		current_sctx = save_sctx;
	}
    }
}

/*
 * "fnameescape({string})" function
 */
    நிலையான வெறுமை
f_fnameescape(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    நிலையான வெறுமை
f_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    எனில் (gui.in_use)
    {
	gui_mch_set_foreground();
	திரும்பு;
    }
#endif
#எனில் defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_set_foreground();
#endif
}

/*
 * "function()" function
 * "funcref()" function
 */
    நிலையான வெறுமை
common_function(typval_T *argvars, typval_T *rettv, முழு is_funcref)
{
    char_u	*s;
    char_u	*name;
    முழு		use_string = FALSE;
    partial_T   *arg_pt = வெற்று;
    char_u	*trans_name = வெற்று;
    முழு		is_global = FALSE;
    char_u	*start_bracket = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != வெற்று)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    இல்லையெனில்
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    எனில் (s == வெற்று)
    {
	semsg(_(e_invalid_argument_str), "NULL");
	திரும்பு;
    }

    எனில் ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == வெற்று) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, வெற்று);
	எனில் (*name == '<')
	{
	    // generic function
	    start_bracket = name;
	    எனில் (skip_generic_func_type_args(&name) == FAIL)
		செல் theend;
	}
	எனில் (*name != NUL)
	    s = வெற்று;
    }

    எனில் (s == வெற்று || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == வெற்று))
	semsg(_(e_invalid_argument_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    இல்லையெனில் எனில் (trans_name != வெற்று && (is_funcref
			 ? find_func(trans_name, is_global) == வெற்று
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    இல்லையெனில்
    {
	முழு	dict_idx = 0;
	முழு	arg_idx = 0;
	list_T	*list = வெற்று;

	எனில் (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	இல்லையெனில் எனில் (trans_name != வெற்று && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	இல்லையெனில்
	    name = vim_strsave(s);

	எனில் (argvars[1].v_type != VAR_UNKNOWN)
	{
	    எனில் (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    இல்லையெனில் எனில் (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    இல்லையெனில்
		// function(name, [args])
		arg_idx = 1;
	    எனில் (dict_idx > 0)
	    {
		எனில் (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    செல் theend;
		}
		எனில் (argvars[dict_idx].vval.v_dict == வெற்று)
		    dict_idx = 0;
	    }
	    எனில் (arg_idx > 0)
	    {
		எனில் (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_second_argument_of_function_must_be_list_or_dict));
		    vim_free(name);
		    செல் theend;
		}
		list = argvars[arg_idx].vval.v_list;
		எனில் (list == வெற்று || list->lv_len == 0)
		    arg_idx = 0;
		இல்லையெனில் எனில் (list->lv_len > MAX_FUNC_ARGS)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    செல் theend;
		}
	    }
	}
	எனில் (dict_idx > 0 || arg_idx > 0 || arg_pt != வெற்று || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    எனில் (pt == வெற்று)
		vim_free(name);
	    இல்லையெனில்
	    {
		எனில் (arg_idx > 0 || (arg_pt != வெற்று && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    முழு		i = 0;
		    முழு		arg_len = 0;
		    முழு		lv_len = 0;

		    எனில் (arg_pt != வெற்று)
			arg_len = arg_pt->pt_argc;
		    எனில் (list != வெற்று)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    எனில் (pt->pt_argv == வெற்று)
		    {
			vim_free(pt);
			vim_free(name);
			செல் theend;
		    }
		    க்கு (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    எனில் (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_பொருட்கள்(list, li)
			    copy_tv(&li->li_tv, &pt->pt_argv[i++]);
		    }
		}

		// For "function(dict.func, [], dict)" and "func" is a partial
		// use "dict".  That is backwards compatible.
		எனில் (dict_idx > 0)
		{
		    // The dict is bound explicitly, pt_auto is FALSE.
		    pt->pt_dict = argvars[dict_idx].vval.v_dict;
		    ++pt->pt_dict->dv_refcount;
		}
		இல்லையெனில் எனில் (arg_pt != வெற்று)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    எனில் (pt->pt_dict != வெற்று)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    எனில் (pt->pt_obj != வெற்று)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		எனில் (arg_pt != வெற்று && arg_pt->pt_func != வெற்று)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		இல்லையெனில் எனில் (is_funcref)
		{
		    pt->pt_func = find_func(trans_name, is_global);
		    எனில் (IS_GENERIC_FUNC(pt->pt_func) && start_bracket != வெற்று)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&start_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		இல்லையெனில்
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		எனில் (arg_pt != வெற்று)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	இல்லையெனில்
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    எனில் (start_bracket == வெற்று)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    இல்லையெனில்
	    {
		// generic function
		size_t len = STRLEN(name) + STRLEN(start_bracket);
		rettv->vval.v_string = alloc(len + 1);
		எனில் (rettv->vval.v_string != வெற்று)
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
    நிலையான வெறுமை
f_funcref(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, TRUE);
}

/*
 * "function()" function
 */
    நிலையான வெறுமை
f_function(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, FALSE);
}

/*
 * "garbagecollect()" function
 */
    நிலையான வெறுமை
f_garbagecollect(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	திரும்பு;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    எனில் (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
	garbage_collect_at_exit = TRUE;
}

/*
 * "get()" function
 */
    நிலையான வெறுமை
f_get(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    list_T	*l;
    tuple_T	*tuple;
    dictitem_T	*di;
    dict_T	*d;
    typval_T	*tv = வெற்று;
    முழு		what_is_dict = FALSE;

    எனில் (argvars[0].v_type == VAR_BLOB)
    {
	முழு error = FALSE;
	முழு idx = tv_get_எண்_chk(&argvars[1], &error);

	எனில் (!error)
	{
	    rettv->v_type = VAR_எண்;
	    எனில் (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    எனில் (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_எண் = -1;
	    இல்லையெனில்
	    {
		rettv->vval.v_எண் = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_LIST)
    {
	எனில் ((l = argvars[0].vval.v_list) != வெற்று)
	{
	    முழு		error = FALSE;

	    li = list_find(l, (long)tv_get_எண்_chk(&argvars[1], &error));
	    எனில் (!error && li != வெற்று)
		tv = &li->li_tv;
	}
    }
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_TUPLE)
    {
	எனில் ((tuple = argvars[0].vval.v_tuple) != வெற்று)
	{
	    முழு		error = FALSE;
	    long	idx;

	    idx = (long)tv_get_எண்_chk(&argvars[1], &error);
	    எனில் (!error)
		tv = tuple_find(tuple, idx);
	}
    }
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_DICT)
    {
	எனில் ((d = argvars[0].vval.v_dict) != வெற்று)
	{
	    di = dict_find(d, tv_get_string(&argvars[1]), -1);
	    எனில் (di != வெற்று)
		tv = &di->di_tv;
	}
    }
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	எனில் (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	இல்லையெனில்
	{
	    CLEAR_FIELD(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	எனில் (pt != வெற்று)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    எனில் (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		எனில் (name == வெற்று)
		    rettv->vval.v_string = வெற்று;
		இல்லையெனில்
		{
		    எனில் (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    எனில் (*what == 'n' && pt->pt_name == வெற்று
							&& pt->pt_func != வெற்று)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		எனில் (pt->pt_dict != வெற்று)
		    rettv_dict_set(rettv, pt->pt_dict);
	    }
	    இல்லையெனில் எனில் (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		எனில் (rettv_list_alloc(rettv) == OK)
		{
		    முழு i;

		    க்கு (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(what, "arity") == 0)
	    {
		முழு required = 0, optional = 0, varargs = FALSE;
		char_u *name = partial_name(pt);

		get_func_arity(name, &required, &optional, &varargs);

		rettv->v_type = VAR_DICT;
		எனில் (rettv_dict_alloc(rettv) == OK)
		{
		    dict_T *dict = rettv->vval.v_dict;

		    // Take into account the arguments of the partial, if any.
		    // Note that it is possible to supply more arguments than the function
		    // accepts.
		    எனில் (pt->pt_argc >= required + optional)
			required = optional = 0;
		    இல்லையெனில் எனில் (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    இல்லையெனில்
			required -= pt->pt_argc;

		    dict_add_எண்(dict, "required", required);
		    dict_add_எண்(dict, "optional", optional);
		    dict_add_bool(dict, "varargs", varargs);
		}
	    }
	    இல்லையெனில்
		semsg(_(e_invalid_argument_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    எனில் (!what_is_dict)
		திரும்பு;
	}
    }
    இல்லையெனில்
	semsg(_(e_argument_of_str_must_be_list_tuple_dictionary_or_blob), "get()");

    எனில் (tv == வெற்று)
    {
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    இல்லையெனில்
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
    நிலையான வெறுமை
f_getbgcolor(typval_T *argvars UNUSED, typval_T *rettv)
{
    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	எனில் (gui.in_use)
	    try_hl = true;
# endif
# ifdef FEAT_TERMGUICOLORS
	எனில் (p_tgc)
	    try_hl = true;
# endif
	எனில் (try_hl)
	{
	    guicolor_T	fg, bg;
	    முழு		hl_id = syn_name2id((char_u *)"Normal");

	    எனில் (hl_id > 0)
	    {
		(வெறுமை)syn_id2colors(hl_id, &fg, &bg);
		எனில் (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_எண்(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_எண்(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_எண்(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    திரும்பு;
		}
	    }
	}
    }
#endif
#ifdef FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	எனில் (term_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_எண்(rettv->vval.v_list, (varnumber_T)r);
	    list_append_எண்(rettv->vval.v_list, (varnumber_T)g);
	    list_append_எண்(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#endif
}

/*
 * "getcellpixels()" function
 */
    நிலையான வெறுமை
f_getcellpixels(typval_T *argvars UNUSED, typval_T *rettv)
{
    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

#எனில் defined(FEAT_GUI)
    எனில் (gui.in_use)
    {
	// success pixel size and no gui.
	list_append_எண்(rettv->vval.v_list, (varnumber_T)gui.char_width);
	list_append_எண்(rettv->vval.v_list, (varnumber_T)gui.char_height);
    }
    இல்லையெனில்
#endif
    {
#எனில் defined(UNIX) || defined(MSWIN)
	list_append_எண்(rettv->vval.v_list, (varnumber_T)cell_width);
	list_append_எண்(rettv->vval.v_list, (varnumber_T)cell_height);
#இல்லையெனில்
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_எண்(rettv->vval.v_list, (varnumber_T)-1);
	list_append_எண்(rettv->vval.v_list, (varnumber_T)-1);
#endif
    }

}

/*
 * "getchangelist()" function
 */
    நிலையான வெறுமை
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    முழு		i;
    list_T	*l;
    dict_T	*d;
    முழு		changelistindex;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    இல்லையெனில்
	buf = tv_get_buf_from_arg(&argvars[0]);
    எனில் (buf == வெற்று)
	திரும்பு;

    l = list_alloc();
    எனில் (l == வெற்று)
	திரும்பு;
    எனில் (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	திரும்பு;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    எனில் (buf == curwin->w_buffer)
    {
	changelistindex = curwin->w_changelistidx;
    }
    இல்லையெனில்
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    எனில் (wip->wi_win == curwin)
		நிறுத்து;
	changelistindex = wip != வெற்று ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_எண்(rettv->vval.v_list, (varnumber_T)changelistindex);

    க்கு (i = 0; i < buf->b_changelistlen; ++i)
    {
	எனில் (buf->b_changelist[i].lnum == 0)
	    தொடர்;
	எனில் ((d = dict_alloc()) == வெற்று)
	    திரும்பு;
	எனில் (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    திரும்பு;
	}
	dict_add_எண்(d, "lnum", (long)buf->b_changelist[i].lnum);
	dict_add_எண்(d, "col", (long)buf->b_changelist[i].col);
	dict_add_எண்(d, "coladd", (long)buf->b_changelist[i].coladd);
    }
}

    நிலையான வெறுமை
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    முழு		getcurpos,
    முழு		charcol)
{
    pos_T	*fp = வெற்று;
    pos_T	pos;
    win_T	*wp = curwin;
    list_T	*l;
    முழு		fnum = -1;

    எனில் (rettv_list_alloc(rettv) == OK)
    {
	l = rettv->vval.v_list;
	எனில் (getcurpos)
	{
	    எனில் (argvars[0].v_type != VAR_UNKNOWN)
	    {
		wp = find_win_by_nr_or_id(&argvars[0]);
		எனில் (wp != வெற்று)
		    fp = &wp->w_cursor;
	    }
	    இல்லையெனில்
		fp = &curwin->w_cursor;
	    எனில் (fp != வெற்று && charcol)
	    {
		pos = *fp;
		pos.col =
		    buf_byteidx_to_charidx(wp->w_buffer, pos.lnum, pos.col);
		fp = &pos;
	    }
	}
	இல்லையெனில்
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	எனில் (fnum != -1)
	    list_append_எண்(l, (varnumber_T)fnum);
	இல்லையெனில்
	    list_append_எண்(l, (varnumber_T)0);
	list_append_எண்(l, (fp != வெற்று) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_எண்(l, (fp != வெற்று)
		     ? (varnumber_T)(fp->col == MAXCOL ? MAXCOL : fp->col + 1)
							    : (varnumber_T)0);
	list_append_எண்(l, (fp != வெற்று) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	எனில் (getcurpos)
	{
	    முழு	    save_set_curswant = curwin->w_set_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    எனில் (wp == curwin)
		update_curswant();
	    list_append_எண்(l, wp == வெற்று ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    எனில் (wp == curwin && save_set_curswant)
	    {
		curwin->w_set_curswant = save_set_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    இல்லையெனில்
	rettv->vval.v_எண் = FALSE;
}

/*
 * "getcharpos()" function
 */
    நிலையான வெறுமை
f_getcharpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    நிலையான வெறுமை
f_getcharsearch(typval_T *argvars UNUSED, typval_T *rettv)
{
    எனில் (rettv_dict_alloc(rettv) != OK)
	திரும்பு;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_string(dict, "char", last_csearch());
    dict_add_எண்(dict, "forward", last_csearch_forward());
    dict_add_எண்(dict, "until", last_csearch_until());
}

/*
 * "getenv()" function
 */
    நிலையான வெறுமை
f_getenv(typval_T *argvars, typval_T *rettv)
{
    முழு	    mustfree = FALSE;
    char_u  *p;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    எனில் (p == வெற்று)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_எண் = VVAL_NULL;
	திரும்பு;
    }
    எனில் (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    நிலையான வெறுமை
f_getfontname(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	திரும்பு;

#ifdef FEAT_GUI
    எனில் (gui.in_use)
    {
	GuiFont font;
	char_u	*name = வெற்று;

	எனில் (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	இல்லையெனில்
	{
	    name = tv_get_string(&argvars[0]);
	    எனில் (STRCMP(name, "*") == 0)	    // don't use font dialog
		திரும்பு;
	    font = gui_mch_get_font(name, FALSE);
	    எனில் (font == NOFONT)
		திரும்பு;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	எனில் (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#endif
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
நிலையான கட்டமைப்பு
{
    argcheck_T	func;
    எழுத்து	*types;	    // separated by commas
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
    {arg_list_எண்,		"list<number>"},
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
    {arg_எண்,		"number"},
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
    நிலையான list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    எழுத்து	*types = "any";

    எனில் (l == வெற்று)
	திரும்பு வெற்று;
    க்கு (முழு i = 0; i < (முழு)ARRAY_LENGTH(argcheck_types); ++i)
	எனில் (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    நிறுத்து;
	}
    க்கு (எழுத்து *p = types; ; )
    {
	எழுத்து	*end = (எழுத்து *)vim_strchr((char_u *)p, ',');
	முழு	len = end == வெற்று ? (முழு)STRLEN(p) : (முழு)(end - p);

	எனில் (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    திரும்பு வெற்று;
	}
	எனில் (end == வெற்று)
	    நிறுத்து;
	p = end + 1;
    }
    திரும்பு l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    நிலையான முழு
getinfo_argtypes(
	list_T			*l,
	மாறிலி funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    முழு		argcount = 0;

    எனில் (l != வெற்று)
	FOR_ALL_LIST_பொருட்கள்(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    எனில் (s == வெற்று)
		திரும்பு -1;
	    எனில் (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_ARGS)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		திரும்பு -1;
	    }
	    type = parse_type(&p, type_gap, வெற்று, வெற்று, TRUE);
	    எனில் (type == வெற்று)
		திரும்பு -1;
	    எனில் (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		திரும்பு -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    க்கு ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_ARGS; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    திரும்பு argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    நிலையான எழுத்து *
builtin_ret_type_name(
	முழு	    idx,
	முழு	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	எழுத்து	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = internal_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    திரும்பு type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
வகைவரை எண்ணிக்கை {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_kind_T;

நிலையான கட்டமைப்பு
{
    எழுத்து		*name;
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
    நிலையான வெறுமை
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_kind_T	kind;
    முழு			ki;
    முழு			idx;
    மாறிலி funcentry_T	*fe;
    dict_T		*d;
    list_T		*args;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_ARGS];
    list_T		*argtypes_list = வெற்று;
    முழு			argcount;
    முழு			vim9 = -1;	// -1: the context of the caller
    எழுத்து		*ret_name;
    எழுத்து		*tofree;

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;
    எனில் (in_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;
    எனில் (check_for_opt_dict_arg(argvars, 2) == FAIL)
	திரும்பு;

    kindname = tv_get_string(&argvars[0]);
    க்கு (ki = 0; ki < (முழு)ARRAY_LENGTH(getinfo_kinds); ++ki)
	எனில் (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    நிறுத்து;
    எனில் (ki == (முழு)ARRAY_LENGTH(getinfo_kinds))
    {
	semsg(_(e_invalid_argument_str), kindname);
	திரும்பு;
    }
    kind = getinfo_kinds[ki].kind;
    name = tv_get_string_buf(&argvars[1], namebuf);

    எனில் (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != வெற்று)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	முழு	    todo = (முழு)opts->dv_hashtab.ht_used;

	FOR_ALL_HASHTAB_பொருட்கள்(&opts->dv_hashtab, hi, todo)
	{
	    எனில் (HASHITEM_EMPTY(hi))
		தொடர்;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    எனில் (STRCMP(di->di_சாவி, "argtypes") == 0 && kind == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    இல்லையெனில் எனில் (STRCMP(di->di_சாவி, "vim9") == 0 && kind == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    இல்லையெனில்
	    {
		semsg(_(e_invalid_argument_str), di->di_சாவி);
		திரும்பு;
	    }
	}
    }
    எனில் (kind == GI_VIMVAR)
    {
	எனில் (STRNCMP(name, "v:", 2) == 0)
	    (வெறுமை)vim_var_info(name + 2, rettv->vval.v_dict);
	திரும்பு;
    }
    எனில் (kind == GI_COMMAND)
    {
	(வெறுமை)ex_command_info(name, vim9, rettv->vval.v_dict);
	திரும்பு;
    }
    எனில் (kind == GI_OPTION)
    {
	(வெறுமை)option_info(name, rettv->vval.v_dict);
	திரும்பு;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = find_internal_func_opt(name, FALSE);
    எனில் (idx < 0)
    {
	(வெறுமை)user_func_info(name, rettv->vval.v_dict);
	திரும்பு;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, அளவு(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    எனில் (argcount < 0)
    {
	clear_type_list(&type_gap);
	திரும்பு;
    }
    d = rettv->vval.v_dict;
    dict_add_string(d, "name", (char_u *)fe->f_name);
    dict_add_string(d, "kind", (char_u *)"builtin");
    dict_add_bool(d, "available", fe->f_func != வெற்று);
    dict_add_எண்(d, "minargs", fe->f_min_argc);
    dict_add_எண்(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_add_எண்(d, "method", fe->f_argtype & FEARG_MASK);

    args = list_alloc();
    எனில் (args == வெற்று || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	clear_type_list(&type_gap);
	திரும்பு;
    }
    argnames = (char_u *)fe->f_argnames;
    க்கு (முழு i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = வெற்று;
	முழு	    argnamelen = 0;

	எனில் (fe->f_argcheck == வெற்று)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    எனில் (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		நிறுத்து;
	    check = arg_any;
	}
	இல்லையெனில்
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    எனில் ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == வெற்று)
		நிறுத்து;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	எனில் (argnames != வெற்று)
	{
	    char_u  *end = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = end == வெற்று ? (முழு)STRLEN(argnames)
						     : (முழு)(end - argnames);
	    argnames = end == வெற்று ? வெற்று : skipwhite(end + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	எனில் (arg == வெற்று || types == வெற்று
		|| dict_add_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    திரும்பு;
	}
	எனில் ((argnamelen > 0
		    && dict_add_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    திரும்பு;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    எனில் (argtypes_list == வெற்று
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	எழுத்து	*tofree_max;
	எழுத்து	*max_name;

	க்கு (முழு i = argcount; i < fe->f_max_argc && i < MAX_FUNC_ARGS; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	எனில் (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = வெற்று;
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
    நிலையான வெறுமை
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    முழு		i;
    list_T	*l;
    dict_T	*d;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_opt_எண்_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 1) == FAIL)))
	திரும்பு;

    wp = find_tabwin(&argvars[0], &argvars[1], வெற்று);
    எனில் (wp == வெற்று)
	திரும்பு;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    எனில் (l == வெற்று)
	திரும்பு;
    எனில் (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	திரும்பு;
    }

    list_append_எண்(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    க்கு (i = 0; i < wp->w_jumplistlen; ++i)
    {
	எனில் (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    தொடர்;
	எனில் ((d = dict_alloc()) == வெற்று)
	    திரும்பு;
	எனில் (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    திரும்பு;
	}
	dict_add_எண்(d, "lnum", (long)wp->w_jumplist[i].fmark.mark.lnum);
	dict_add_எண்(d, "col", (long)wp->w_jumplist[i].fmark.mark.col);
	dict_add_எண்(d, "coladd", (long)wp->w_jumplist[i].fmark.mark.coladd);
	dict_add_எண்(d, "bufnr", (long)wp->w_jumplist[i].fmark.fnum);
	எனில் (wp->w_jumplist[i].fname != வெற்று)
	    dict_add_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    நிலையான வெறுமை
f_getpid(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_எண் = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    நிலையான வெறுமை
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    நிலையான வெறுமை
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    நிலையான வெறுமை
f_getpos(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    நிலையான முழு
block_def2str(கட்டமைப்பு block_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    எனில் (ret->string == வெற்று)
    {
	ret->length = 0;
	திரும்பு FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->length = bd->startspaces;
    mch_memmove(ret->string + ret->length, bd->textstart, bd->textlen);
    ret->length += bd->textlen;
    vim_memset(ret->string + ret->length, ' ', bd->endspaces);
    ret->length += bd->endspaces;
    ret->string[ret->length] = NUL;

    திரும்பு OK;
}

    நிலையான முழு
getregionpos(
    typval_T	*argvars,
    typval_T	*rettv,
    pos_T	*p1,
    pos_T	*p2,
    முழு		*inclusive,
    முழு		*region_type,
    oparg_T	*oap)
{
    முழு		fnum1 = -1, fnum2 = -1;
    char_u	*type;
    buf_T	*findbuf;
    char_u	default_type[] = "v";
    முழு		block_width = 0;
    முழு		is_select_exclusive;
    முழு		l;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு FAIL;

    எனில் (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	திரும்பு FAIL;

    எனில் (list2fpos(&argvars[0], p1, &fnum1, வெற்று, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, வெற்று, FALSE) != OK
	    || fnum1 != fnum2)
	திரும்பு FAIL;

    எனில் (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	எனில் (type == வெற்று)
	    type = default_type;
    }
    இல்லையெனில்
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    எனில் (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    இல்லையெனில் எனில் (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    இல்லையெனில் எனில் (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	எனில் (*p != NUL && ((block_width = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "type", type);
	    திரும்பு FAIL;
	}
	*region_type = MBLOCK;
    }
    இல்லையெனில்
    {
	semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "type", type);
	திரும்பு FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    எனில் (findbuf == வெற்று || findbuf->b_ml.ml_mfp == வெற்று)
    {
	emsg(_(e_buffer_is_not_loaded));
	திரும்பு FAIL;
    }

    எனில் (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_வரி_மொத்தம்)
    {
	semsg(_(e_invalid_வரி_எண்_nr), p1->lnum);
	திரும்பு FAIL;
    }
    எனில் (p1->col == MAXCOL)
	p1->col = ml_get_buf_len(findbuf, p1->lnum) + 1;
    இல்லையெனில் எனில் (p1->col < 1 || p1->col > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_column_எண்_nr), p1->col);
	திரும்பு FAIL;
    }

    எனில் (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_வரி_மொத்தம்)
    {
	semsg(_(e_invalid_வரி_எண்_nr), p2->lnum);
	திரும்பு FAIL;
    }
    எனில் (p2->col == MAXCOL)
	p2->col = ml_get_buf_len(findbuf, p2->lnum) + 1;
    இல்லையெனில் எனில் (p2->col < 1 || p2->col > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_column_எண்_nr), p2->col);
	திரும்பு FAIL;
    }

    curbuf = findbuf;
    curwin->w_buffer = curbuf;
    virtual_op = virtual_active();

    // NOTE: Adjustment is needed.
    p1->col--;
    p2->col--;

    எனில் (!LT_POS(*p1, *p2))
    {
	// swap position
	pos_T p;

	p = *p1;
	*p1 = *p2;
	*p2 = p;
    }

    எனில் (*region_type == MCHAR)
    {
	// Handle 'selection' == "exclusive".
	எனில் (is_select_exclusive && !EQUAL_POS(*p1, *p2))
	    // When backing up to previous line, inclusive becomes false.
	    *inclusive = !unadjust_for_sel_inner(p2);
	// If p2 is on NUL (end of line), inclusive becomes false.
	எனில் (*inclusive && !virtual_op && *ml_get_pos(p2) == NUL)
	    *inclusive = FALSE;
    }
    இல்லையெனில் எனில் (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#ifdef FEAT_LINEBREAK
	முழு	lbr_saved = reset_lbr();
#endif

	getvvcol(curwin, p1, &sc1, வெற்று, &ec1, 0);
	getvvcol(curwin, p2, &sc2, வெற்று, &ec2, 0);

#ifdef FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#endif

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->start = *p1;
	oap->end = *p2;
	oap->start_vcol = MIN(sc1, sc2);
	எனில் (block_width > 0)
	    oap->end_vcol = oap->start_vcol + block_width - 1;
	இல்லையெனில் எனில் (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->end_vcol = sc2 - 1;
	இல்லையெனில்
	    oap->end_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    எனில் (l > 1)
	p2->col += l - 1;

    திரும்பு OK;
}

/*
 * "getregion()" function
 */
    நிலையான வெறுமை
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    முழு			inclusive = TRUE;
    முழு			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    முழு			save_virtual;
    string_T		akt = {வெற்று, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    எனில் (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	திரும்பு;

    க்கு (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	முழு ret = OK;

	எனில் (region_type == MBLOCK)
	{
	    கட்டமைப்பு block_def	bd;

	    block_prep(&oa, &bd, lnum, FALSE);
	    block_def2str(&bd, &akt);
	}
	இல்லையெனில் எனில் (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.length = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.length);
	    எனில் (akt.string == வெற்று)
		akt.length = 0;
	    இல்லையெனில்
		akt.length = s.length;
	}
	இல்லையெனில்
	{
	    கட்டமைப்பு block_def	bd;

	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);
	    block_def2str(&bd, &akt);
	}

	எனில் (akt.string == வெற்று)
	{
	    clear_tv(rettv);
	    (வெறுமை)rettv_list_alloc(rettv);
	    நிறுத்து;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (முழு)akt.length);
	vim_free(akt.string);

	எனில் (ret == FAIL)
	{
	    clear_tv(rettv);
	    (வெறுமை)rettv_list_alloc(rettv);
	    நிறுத்து;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_buffer = curbuf;
    virtual_op = save_virtual;
}

    நிலையான வெறுமை
add_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    எனில் (l1 == வெற்று)
	திரும்பு;

    எனில் (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	திரும்பு;
    }

    l2 = list_alloc();
    எனில் (l2 == வெற்று)
	திரும்பு;

    எனில் (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	திரும்பு;
    }

    l3 = list_alloc();
    எனில் (l3 == வெற்று)
	திரும்பு;

    எனில் (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	திரும்பு;
    }

    list_append_எண்(l2, curbuf->b_fnum);
    list_append_எண்(l2, p1.lnum);
    list_append_எண்(l2, p1.col);
    list_append_எண்(l2, p1.coladd);

    list_append_எண்(l3, curbuf->b_fnum);
    list_append_எண்(l3, p2.lnum);
    list_append_எண்(l3, p2.col);
    list_append_எண்(l3, p2.coladd);
}

/*
 * Compute the positions of the region segment on line "lnum".
 * "ret_p1" is set to the start position of the segment and "ret_p2" to its
 * end position.
 */
    நிலையான வெறுமை
getregionpos_வரி(
    linenr_T	lnum,
    pos_T	p1,
    pos_T	p2,
    முழு		inclusive,
    முழு		region_type,
    oparg_T	*oap,
    முழு		allow_eol,
    pos_T	*ret_p1,
    pos_T	*ret_p2)
{
    char_u	*வரி = ml_get(lnum);
    colnr_T	வரி_len = ml_get_len(lnum);

    எனில் (region_type == MLINE)
    {
	ret_p1->col = 1;
	ret_p1->coladd = 0;
	ret_p2->col = MAXCOL;
	ret_p2->coladd = 0;
    }
    இல்லையெனில்
    {
	கட்டமைப்பு block_def	bd;

	எனில் (region_type == MBLOCK)
	    block_prep(oap, &bd, lnum, FALSE);
	இல்லையெனில்
	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);

	எனில் (bd.is_oneChar)  // selection entirely inside one char
	{
	    எனில் (region_type == MBLOCK)
	    {
		ret_p1->col = mb_prevptr(வரி, bd.textstart) - வரி + 1;
		ret_p1->coladd = bd.start_char_vcols
					   - (bd.start_vcol - oap->start_vcol);
	    }
	    இல்லையெனில்
	    {
		ret_p1->col = p1.col + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	இல்லையெனில் எனில் (region_type == MBLOCK && oap->start_vcol > bd.start_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->col = MAXCOL;
	    ret_p1->coladd = oap->start_vcol - bd.start_vcol;
	    bd.is_oneChar = TRUE;
	}
	இல்லையெனில் எனில் (bd.startspaces > 0)
	{
	    ret_p1->col = mb_prevptr(வரி, bd.textstart) - வரி + 1;
	    ret_p1->coladd = bd.start_char_vcols - bd.startspaces;
	}
	இல்லையெனில்
	{
	    ret_p1->col = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	எனில் (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->col = ret_p1->col;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	இல்லையெனில் எனில் (bd.endspaces > 0)
	{
	    ret_p2->col = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	இல்லையெனில்
	{
	    ret_p2->col = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    எனில் (!allow_eol && ret_p1->col > வரி_len)
    {
	ret_p1->col = 0;
	ret_p1->coladd = 0;
    }
    இல்லையெனில் எனில் (ret_p1->col > வரி_len + 1)
	ret_p1->col = வரி_len + 1;

    எனில் (!allow_eol && ret_p2->col > வரி_len)
    {
	ret_p2->col = ret_p1->col == 0 ? 0 : வரி_len;
	ret_p2->coladd = 0;
    }
    இல்லையெனில் எனில் (ret_p2->col > வரி_len + 1)
	ret_p2->col = வரி_len + 1;

    ret_p1->lnum = lnum;
    ret_p2->lnum = lnum;
}

/*
 * "getregionpos()" function
 */
    நிலையான வெறுமை
f_getregionpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	p1, p2;
    முழு		inclusive = TRUE;
    முழு		region_type = -1;
    முழு		allow_eol = FALSE;
    முழு		bounds_only = FALSE;
    oparg_T	oa;
    முழு		lnum;

    buf_T	*save_curbuf;
    முழு		save_virtual;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    எனில் (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	திரும்பு;

    எனில் (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    எனில் (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	start_pos, end_pos;

	getregionpos_வரி(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &start_pos, &end_pos);
	எனில் (p2.lnum != p1.lnum)
	{
	    pos_T	unused;

	    getregionpos_வரி(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &unused, &end_pos);
	}
	add_regionpos_range(rettv, start_pos, end_pos);
    }
    இல்லையெனில்
    {
	க்கு (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_வரி(lnum, p1, p2, inclusive, region_type, &oa,
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
    நிலையான முழு
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	எனில் (strregname != வெற்று && in_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_register_name_must_be_one_char_str), strregname);
	    strregname = வெற்று;
	}
	எனில் (strregname == வெற்று)	    // type error; errmsg already given
	    திரும்பு 0;
    }
    இல்லையெனில்
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    திரும்பு *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    நிலையான வெறுமை
f_getreg(typval_T *argvars, typval_T *rettv)
{
    முழு		regname;
    முழு		arg2 = FALSE;
    முழு		return_list = FALSE;

    எனில் (in_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	திரும்பு;

    regname = getreg_get_regname(argvars);
    எனில் (regname == 0)
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	முழு		error = FALSE;

	arg2 = (முழு)tv_get_bool_chk(&argvars[1], &error);

	எனில் (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (முழு)tv_get_bool_chk(&argvars[2], &error);
	எனில் (error)
	    திரும்பு;
    }

    எனில் (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_கோவை_SRC : 0) | GREG_LIST);
	எனில் (rettv->vval.v_list == வெற்று)
	    (வெறுமை)rettv_list_alloc(rettv);
	இல்லையெனில்
	    ++rettv->vval.v_list->lv_refcount;
    }
    இல்லையெனில்
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = get_reg_contents(regname,
						    arg2 ? GREG_கோவை_SRC : 0);
    }
}

/*
 * "getregtype()" function
 */
    நிலையான வெறுமை
f_getregtype(typval_T *argvars, typval_T *rettv)
{
    முழு		regname;
    char_u	buf[NUMBUFLEN + 2];
    long	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    regname = getreg_get_regname(argvars);
    எனில் (regname == 0)
	திரும்பு;

    buf[0] = NUL;
    buf[1] = NUL;
    தேர்வு (get_reg_type(regname, &reglen))
    {
	நிலை MLINE: buf[0] = 'V'; நிறுத்து;
	நிலை MCHAR: buf[0] = 'v'; நிறுத்து;
	நிலை MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((எழுத்து *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		நிறுத்து;
    }
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "gettagstack()" function
 */
    நிலையான வெறுமை
f_gettagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;			// default is current window

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	எனில் (wp == வெற்று)
	    திரும்பு;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    நிலையான வெறுமை
f_gettext(typval_T *argvars, typval_T *rettv)
{
#எனில் defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    எழுத்து *prev = வெற்று;
#endif

    எனில் (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_STRING;

    எனில் (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != வெற்று &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#எனில் defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((மாறிலி எழுத்து *)argvars[1].vval.v_string, (எழுத்து *)p_enc);
#endif

#எனில் defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((மாறிலி எழுத்து *)argvars[1].vval.v_string, (மாறிலி எழுத்து *)argvars[0].vval.v_string));
#இல்லையெனில்
	textdomain((மாறிலி எழுத்து *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#endif

#எனில் defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	எனில் (prev != வெற்று)
	    bind_textdomain_codeset((மாறிலி எழுத்து *)argvars[1].vval.v_string, prev);
#endif
    }
    இல்லையெனில்
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
}

// for VIM_VERSION_ defines
#அந்தர்பாவ "version.h"

/*
 * "has()" function
 */
    வெறுமை
f_has(typval_T *argvars, typval_T *rettv)
{
    முழு		i;
    char_u	*name;
    முழு		x = FALSE;
    முழு		n = FALSE;
    வகைவரை கட்டமைப்பு {
	எழுத்து *name;
	short present;
    } has_item_T;
    நிலையான has_item_T has_list[] =
    {
	{"amiga",
#ifdef AMIGA
		1
#இல்லையெனில்
		0
#endif
		},
	{"android",
#ifdef __ANDROID__
		1
#இல்லையெனில்
		0
#endif
		},
	{"arp",
#எனில் defined(AMIGA) && defined(FEAT_ARP)
		1
#இல்லையெனில்
		0
#endif
		},
	{"haiku",
#ifdef __HAIKU__
		1
#இல்லையெனில்
		0
#endif
		},
	{"bsd",
#எனில் defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#இல்லையெனில்
		0
#endif
		},
	{"hpux",
#ifdef hpux
		1
#இல்லையெனில்
		0
#endif
		},
	{"hurd",
#ifdef __GNU__
		1
#இல்லையெனில்
		0
#endif
		},
	{"linux",
#ifdef __linux__
		1
#இல்லையெனில்
		0
#endif
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#ifdef MACOS_X
		1
#இல்லையெனில்
		0
#endif
		},
	{"osx",		// Mac OS X
#ifdef MACOS_X
		1
#இல்லையெனில்
		0
#endif
		},
	{"macunix",	// Mac OS X, with the darwin feature
#எனில் defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#இல்லையெனில்
		0
#endif
		},
	{"osxdarwin",	// synonym for macunix
#எனில் defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#இல்லையெனில்
		0
#endif
		},
	{"qnx",
#ifdef __QNX__
		1
#இல்லையெனில்
		0
#endif
		},
	{"sun",
#ifdef SUN_SYSTEM
		1
#இல்லையெனில்
		0
#endif
		},
	{"termux",
#ifdef __TERMUX__
		1
#இல்லையெனில்
		0
#endif
		},
	{"unix",
#ifdef UNIX
		1
#இல்லையெனில்
		0
#endif
		},
	{"vms",
#ifdef VMS
		1
#இல்லையெனில்
		0
#endif
		},
	{"win32",
#ifdef MSWIN
		1
#இல்லையெனில்
		0
#endif
		},
	{"win32unix",
#ifdef WIN32UNIX
		1
#இல்லையெனில்
		0
#endif
		},
	{"win64",
#ifdef _WIN64
		1
#இல்லையெனில்
		0
#endif
		},
	{"ebcdic", 0 },
	{"fname_case",
#ifndef CASE_INSENSITIVE_FILENAME
		1
#இல்லையெனில்
		0
#endif
		},
	{"acl",
#ifdef HAVE_ACL
		1
#இல்லையெனில்
		0
#endif
		},
	{"arabic",
#ifdef FEAT_ARABIC
		1
#இல்லையெனில்
		0
#endif
		},
	{"autocmd", 1},
	{"autochdir",
#ifdef FEAT_AUTOCHDIR
		1
#இல்லையெனில்
		0
#endif
		},
	{"autoservername",
#ifdef FEAT_AUTOSERVERNAME
		1
#இல்லையெனில்
		0
#endif
		},
	{"socketserver",
#ifdef FEAT_SOCKETSERVER
		1
#இல்லையெனில்
		0
#endif
		},
	{"balloon_eval",
#ifdef FEAT_BEVAL_GUI
		1
#இல்லையெனில்
		0
#endif
		},
	{"balloon_multiline",
#ifdef FEAT_BEVAL_GUI
		1
#இல்லையெனில்
		0
#endif
		},
	{"balloon_eval_term",
#ifdef FEAT_BEVAL_TERM
		1
#இல்லையெனில்
		0
#endif
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#எனில் defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#இல்லையெனில்
		0
#endif
		},
	{"byte_offset",
#ifdef FEAT_BYTEOFF
		1
#இல்லையெனில்
		0
#endif
		},
	{"channel",
#ifdef FEAT_JOB_CHANNEL
		1
#இல்லையெனில்
		0
#endif
		},
	{"cindent", 1},
	{"clientserver",
#ifdef FEAT_CLIENTSERVER
		1
#இல்லையெனில்
		0
#endif
		},
	{"clipboard",
#ifdef FEAT_CLIPBOARD
		1
#இல்லையெனில்
		0
#endif
		},
	{"clipboard_provider",
#ifdef FEAT_CLIPBOARD_PROVIDER
		1
#இல்லையெனில்
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
#இல்லையெனில்
		0
#endif
		},
	{"cryptv",
#ifdef FEAT_CRYPT
		1
#இல்லையெனில்
		0
#endif
		},
	{"crypt-blowfish",
#ifdef FEAT_CRYPT
		1
#இல்லையெனில்
		0
#endif
		},
	{"crypt-blowfish2",
#ifdef FEAT_CRYPT
		1
#இல்லையெனில்
		0
#endif
		},
	{"cscope",
#ifdef FEAT_CSCOPE
		1
#இல்லையெனில்
		0
#endif
		},
	{"cursorbind", 1},
	{"cursorshape",
#ifdef CURSOR_SHAPE
		1
#இல்லையெனில்
		0
#endif
		},
	{"debug",
#ifdef DEBUG
		1
#இல்லையெனில்
		0
#endif
		},
	{"dialog_con",
#ifdef FEAT_CON_DIALOG
		1
#இல்லையெனில்
		0
#endif
		},
	{"dialog_con_gui",
#எனில் defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#இல்லையெனில்
		0
#endif
		},
	{"dialog_gui",
#ifdef FEAT_GUI_DIALOG
		1
#இல்லையெனில்
		0
#endif
		},
	{"diff",
#ifdef FEAT_DIFF
		1
#இல்லையெனில்
		0
#endif
		},
	{"digraphs",
#ifdef FEAT_DIGRAPHS
		1
#இல்லையெனில்
		0
#endif
		},
	{"directx",
#ifdef FEAT_DIRECTX
		1
#இல்லையெனில்
		0
#endif
		},
	{"dnd",
#ifdef FEAT_DND
		1
#இல்லையெனில்
		0
#endif
		},
	{"drop_file",
#ifdef HAVE_DROP_FILE
		1
#இல்லையெனில்
		0
#endif
		},
	{"emacs_tags",
#ifdef FEAT_EMACS_TAGS
		1
#இல்லையெனில்
		0
#endif
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#ifdef FEAT_தேடல்_EXTRA
		1
#இல்லையெனில்
		0
#endif
		},
	{"file_in_path", 1},
	{"filterpipe",
#எனில் defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#இல்லையெனில்
		0
#endif
		},
	{"find_in_path",
#ifdef FEAT_FIND_ID
		1
#இல்லையெனில்
		0
#endif
		},
	{"float", 1},
	{"folding",
#ifdef FEAT_FOLDING
		1
#இல்லையெனில்
		0
#endif
		},
	{"footer", 0},
	{"fork",
#எனில் !defined(USE_SYSTEM) && defined(UNIX)
		1
#இல்லையெனில்
		0
#endif
		},
	{"gettext",
#ifdef FEAT_GETTEXT
		1
#இல்லையெனில்
		0
#endif
		},
	{"gui",
#ifdef FEAT_GUI
		1
#இல்லையெனில்
		0
#endif
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#ifdef FEAT_GUI_GTK
		1
#இல்லையெனில்
		0
#endif
		},
	{"gui_gtk2",
#எனில் defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#இல்லையெனில்
		0
#endif
		},
	{"gui_gtk3",
#எனில் defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#இல்லையெனில்
		0
#endif
		},
	{"gui_gtk4",
#எனில் defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#இல்லையெனில்
		0
#endif
		},
	{"gui_gnome",
#ifdef FEAT_GUI_GNOME
		1
#இல்லையெனில்
		0
#endif
		},
	{"gui_haiku",
#ifdef FEAT_GUI_HAIKU
		1
#இல்லையெனில்
		0
#endif
		},
	{"gui_mac", 0},
	{"gui_motif",
#ifdef FEAT_GUI_MOTIF
		1
#இல்லையெனில்
		0
#endif
		},
	{"gui_photon",
#ifdef FEAT_GUI_PHOTON
		1
#இல்லையெனில்
		0
#endif
		},
	{"gui_win32",
#ifdef FEAT_GUI_MSWIN
		1
#இல்லையெனில்
		0
#endif
		},
	{"iconv",
#எனில் defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#இல்லையெனில்
		0
#endif
		},
	{"image",
#ifdef FEAT_IMAGE
		1
#இல்லையெனில்
		0
#endif
		},
	{"image_popup",
#ifdef FEAT_IMAGE_POPUP
		1
#இல்லையெனில்
		0
#endif
		},
	{"insert_expand", 1},
	{"ipv6",
#ifdef FEAT_IPV6
		1
#இல்லையெனில்
		0
#endif
	},
	{"job",
#ifdef FEAT_JOB_CHANNEL
		1
#இல்லையெனில்
		0
#endif
		},
	{"jumplist", 1},
	{"keymap",
#ifdef FEAT_KEYMAP
		1
#இல்லையெனில்
		0
#endif
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#ifdef FEAT_LANGMAP
		1
#இல்லையெனில்
		0
#endif
		},
	{"libcall",
#ifdef FEAT_LIBCALL
		1
#இல்லையெனில்
		0
#endif
		},
	{"linebreak",
#ifdef FEAT_LINEBREAK
		1
#இல்லையெனில்
		0
#endif
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#எனில் defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#இல்லையெனில்
		0
#endif
		},
	{"menu",
#ifdef FEAT_MENU
		1
#இல்லையெனில்
		0
#endif
		},
	{"mksession",
#ifdef FEAT_SESSION
		1
#இல்லையெனில்
		0
#endif
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#ifdef FEAT_MOUSESHAPE
		1
#இல்லையெனில்
		0
#endif
		},
	{"mouse_dec",
#எனில் (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#இல்லையெனில்
		0
#endif
		},
	{"mouse_gpm",
#எனில் (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#இல்லையெனில்
		0
#endif
		},
	{"mouse_jsbterm",
#எனில் (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#இல்லையெனில்
		0
#endif
		},
	{"mouse_netterm",
#எனில் (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#இல்லையெனில்
		0
#endif
		},
	{"mouse_pterm",
#எனில் (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#இல்லையெனில்
		0
#endif
		},
	{"mouse_sgr",
#எனில் (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#இல்லையெனில்
		0
#endif
		},
	{"mouse_sysmouse",
#எனில் (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#இல்லையெனில்
		0
#endif
		},
	{"mouse_urxvt",
#எனில் (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#இல்லையெனில்
		0
#endif
		},
	{"mouse_xterm",
#எனில் (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#இல்லையெனில்
		0
#endif
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#ifdef FEAT_MBYTE_IME
		1
#இல்லையெனில்
		0
#endif
		},
	{"multi_lang",
#ifdef FEAT_MULTI_LANG
		1
#இல்லையெனில்
		0
#endif
		},
	{"mzscheme",
#எனில் defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#இல்லையெனில்
		0
#endif
		},
	{"nanotime",
#ifdef ST_MTIM_NSEC
		1
#இல்லையெனில்
		0
#endif
	},
	{"num64", 1},
	{"ole",
#ifdef FEAT_OLE
		1
#இல்லையெனில்
		0
#endif
		},
	{"packages",
#ifdef FEAT_EVAL
		1
#இல்லையெனில்
		0
#endif
		},
	{"path_extra", 1},
	{"perl",
#எனில் defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#இல்லையெனில்
		0
#endif
		},
	{"persistent_undo",
#ifdef FEAT_PERSISTENT_UNDO
		1
#இல்லையெனில்
		0
#endif
		},
	{"python_compiled",
#எனில் defined(FEAT_PYTHON)
		1
#இல்லையெனில்
		0
#endif
		},
	{"python_dynamic",
#எனில் defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#இல்லையெனில்
		0
#endif
		},
	{"python",
#எனில் defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#இல்லையெனில்
		0
#endif
		},
	{"pythonx",
#எனில் (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#இல்லையெனில்
		0
#endif
		},
	{"python3_compiled",
#எனில் defined(FEAT_PYTHON3)
		1
#இல்லையெனில்
		0
#endif
		},
	{"python3_dynamic",
#எனில் defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#இல்லையெனில்
		0
#endif
		},
	{"python3_stable",
#எனில் defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#இல்லையெனில்
		0
#endif
		},
	{"python3",
#எனில் defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#இல்லையெனில்
		0
#endif
		},
	{"popupwin",
#ifdef FEAT_PROP_POPUP
		1
#இல்லையெனில்
		0
#endif
		},
	{"postscript",
#ifdef FEAT_POSTSCRIPT
		1
#இல்லையெனில்
		0
#endif
		},
	{"pango",
#ifdef FEAT_PRINT_PANGO
		1
#இல்லையெனில்
		0
#endif
		},
	{"printer",
#ifdef FEAT_PRINTER
		1
#இல்லையெனில்
		0
#endif
		},
	{"profile",
#ifdef FEAT_PROFILE
		1
#இல்லையெனில்
		0
#endif
		},
	{"prof_nsec",
#ifdef PROF_NSEC
		1
#இல்லையெனில்
		0
#endif
		},
	{"reltime",
#ifdef FEAT_RELTIME
		1
#இல்லையெனில்
		0
#endif
		},
	{"quickfix",
#ifdef FEAT_QUICKFIX
		1
#இல்லையெனில்
		0
#endif
		},
	{"rightleft",
#ifdef FEAT_RIGHTLEFT
		1
#இல்லையெனில்
		0
#endif
		},
	{"ruby",
#எனில் defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#இல்லையெனில்
		0
#endif
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#ifdef FEAT_SIGNS
		1
#இல்லையெனில்
		0
#endif
		},
	{"smartindent", 1},
	{"startuptime",
#ifdef STARTUPTIME
		1
#இல்லையெனில்
		0
#endif
		},
	{"statusline",
#ifdef FEAT_STL_OPT
		1
#இல்லையெனில்
		0
#endif
		},
	{"statusline_click",
#ifdef FEAT_STL_OPT
		1
#இல்லையெனில்
		0
#endif
		},
	{"netbeans_intg",
#ifdef FEAT_NETBEANS_INTG
		1
#இல்லையெனில்
		0
#endif
		},
	{"sodium",
#எனில் defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#இல்லையெனில்
		0
#endif
		},
	{"sound",
#ifdef FEAT_SOUND
		1
#இல்லையெனில்
		0
#endif
		},
	{"spell",
#ifdef FEAT_SPELL
		1
#இல்லையெனில்
		0
#endif
		},
	{"syntax",
#ifdef FEAT_SYN_HL
		1
#இல்லையெனில்
		0
#endif
		},
	{"system",
#எனில் defined(USE_SYSTEM) || !defined(UNIX)
		1
#இல்லையெனில்
		0
#endif
		},
	{"tabpanel",
#எனில் defined(FEAT_TABPANEL)
		1,
#இல்லையெனில்
		0,
#endif
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#எனில் defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#இல்லையெனில்
		0
#endif
		},
	{"termguicolors",
#ifdef FEAT_TERMGUICOLORS
		1
#இல்லையெனில்
		0
#endif
		},
	{"terminal",
#எனில் defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#இல்லையெனில்
		0
#endif
		},
	{"terminfo",
#ifdef TERMINFO
		1
#இல்லையெனில்
		0
#endif
		},
	{"termresponse",
#ifdef FEAT_TERMRESPONSE
		1
#இல்லையெனில்
		0
#endif
		},
	{"textobjects", 1},
	{"textprop",
#ifdef FEAT_PROP_POPUP
		1
#இல்லையெனில்
		0
#endif
		},
	{"tgetent",
#ifdef HAVE_TGETENT
		1
#இல்லையெனில்
		0
#endif
		},
	{"timers",
#ifdef FEAT_TIMERS
		1
#இல்லையெனில்
		0
#endif
		},
	{"title", 1},
	{"toolbar",
#ifdef FEAT_TOOLBAR
		1
#இல்லையெனில்
		0
#endif
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#ifdef FEAT_VARTABS
		1
#இல்லையெனில்
		0
#endif
		},
	{"vertsplit", 1},
	{"viminfo",
#ifdef FEAT_VIMINFO
		1
#இல்லையெனில்
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
#இல்லையெனில்
		0
#endif
		},
	{"wayland",
#ifdef FEAT_WAYLAND
		1
#இல்லையெனில்
		0
#endif
		},
	{"wayland_clipboard",
#ifdef FEAT_WAYLAND_CLIPBOARD
		1
#இல்லையெனில்
		0
#endif
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#ifdef FEAT_WAK
		1
#இல்லையெனில்
		0
#endif
		},
	{"writebackup",
#ifdef FEAT_WRITEBACKUP
		1
#இல்லையெனில்
		0
#endif
		},
	{"xattr",
#ifdef FEAT_XATTR
		1
#இல்லையெனில்
		0
#endif
		},
	{"xim",
#ifdef FEAT_XIM
		1
#இல்லையெனில்
		0
#endif
		},
	{"xfontset",
#ifdef FEAT_XFONTSET
		1
#இல்லையெனில்
		0
#endif
		},
	{"xpm",
#எனில் defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#இல்லையெனில்
		0
#endif
		},
	{"xpm_w32",	// for backward compatibility
#ifdef FEAT_XPM_W32
		1
#இல்லையெனில்
		0
#endif
		},
	{"xsmp",
#ifdef USE_XSMP
		1
#இல்லையெனில்
		0
#endif
		},
	{"xsmp_interact",
#ifdef USE_XSMP_INTERACT
		1
#இல்லையெனில்
		0
#endif
		},
	{"xterm_clipboard",
#ifdef FEAT_XCLIPBOARD
		1
#இல்லையெனில்
		0
#endif
		},
	{"xterm_save",
#ifdef FEAT_XTERM_SAVE
		1
#இல்லையெனில்
		0
#endif
		},
	{"X11",
#எனில் defined(UNIX) && defined(FEAT_X11)
		1
#இல்லையெனில்
		0
#endif
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#எனில் defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#இல்லையெனில்
		0
#endif
		},
	{வெற்று, 0}
    };

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	திரும்பு;

    name = tv_get_string(&argvars[0]);

    // Fast-path: check features not in has_list[] first to avoid the full
    // linear scan for very common queries like has('patch-...').
    எனில் (STRNICMP(name, "patch", 5) == 0)
    {
	x = TRUE;
	எனில் (name[5] == '-'
		&& STRLEN(name) >= 11
		&& (name[6] >= '1' && name[6] <= '9'))
	{
	    எழுத்து	*end;
	    முழு	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (முழு)strtoul((எழுத்து *)name + 6, &end, 10);
	    எனில் (*end == '.' && vim_isdigit(end[1])
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
	இல்லையெனில் எனில் (SAFE_isdigit(name[5]))
	    n = has_patch(atoi((எழுத்து *)name + 5));
    }
    இல்லையெனில் எனில் (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    இல்லையெனில் எனில் (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_உள்ளீடு_isatty();
    }
    இல்லையெனில் எனில் (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    இல்லையெனில் எனில் (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    இல்லையெனில் எனில் (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#ifdef FEAT_GUI
	n = (gui.in_use || gui.starting);
#endif
    }
    இல்லையெனில் எனில் (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#எனில் defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.in_use;	// gui_mch_browse() works when GUI is running
#endif
    }
    இல்லையெனில் எனில் (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#ifdef FEAT_SYN_HL
	n = syntax_present(curwin);
#endif
    }
    இல்லையெனில் எனில் (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#ifdef FEAT_VTP
	n = is_term_win32() && has_vtp_working();
#endif
    }
    இல்லையெனில் எனில் (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_NETBEANS_INTG
	n = netbeans_active();
#endif
    }
    இல்லையெனில் எனில் (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_MOUSE_GPM
	n = gpm_enabled();
#endif
    }
    இல்லையெனில் எனில் (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#எனில் defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#endif
    }
    இல்லையெனில் எனில் (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_star.available;
#endif
    }
    இல்லையெனில் எனில் (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# எனில் defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	எனில் (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	இல்லையெனில்
	    n = FALSE;
# இல்லையெனில்
	n = FALSE;
# endif
#endif
    }

    // Look up in has_list[] only if not already handled above.
    எனில் (x == FALSE)
    {
	க்கு (i = 0; has_list[i].name != வெற்று; ++i)
	    எனில் (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		நிறுத்து;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	எனில் (x == TRUE && n == FALSE)
	{
	    எனில் (0)
	    {
		// intentionally empty
	    }
#ifdef VIMDLL
	    இல்லையெனில் எனில் (STRICMP(name, "filterpipe") == 0)
		n = gui.in_use || gui.starting;
#endif
#எனில் defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    இல்லையெனில் எனில் (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#endif
#ifdef DYNAMIC_LUA
	    இல்லையெனில் எனில் (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#endif
#ifdef DYNAMIC_MZSCHEME
	    இல்லையெனில் எனில் (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#endif
#ifdef DYNAMIC_PERL
	    இல்லையெனில் எனில் (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON
	    இல்லையெனில் எனில் (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON3
	    இல்லையெனில் எனில் (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#endif
#எனில் defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    இல்லையெனில் எனில் (STRICMP(name, "pythonx") == 0)
	    {
# எனில் defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		எனில் (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		இல்லையெனில் எனில் (p_pyx == 3)
		    n = python3_enabled(FALSE);
		இல்லையெனில் எனில் (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#endif
#ifdef DYNAMIC_RUBY
	    இல்லையெனில் எனில் (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#endif
#ifdef DYNAMIC_TCL
	    இல்லையெனில் எனில் (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#endif
#ifdef DYNAMIC_SODIUM
	    இல்லையெனில் எனில் (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#endif
#எனில் defined(FEAT_TERMINAL) && defined(MSWIN)
	    இல்லையெனில் எனில் (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#endif
#ifdef DYNAMIC_GPM
	    இல்லையெனில் எனில் (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_available();
#endif
	}
    }

    எனில் (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_எண் = x;
    இல்லையெனில்
	// return whether feature is enabled
	rettv->vval.v_எண் = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    முழு
dynamic_feature(char_u *feature)
{
    திரும்பு (feature == வெற்று
#எனில் defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.in_use)
#endif
#ifdef VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#endif
#எனில் defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.in_use)
#endif
#எனில் defined(USE_ICONV) && defined(DYNAMIC_ICONV)
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
#எனில் defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
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
#எனில் defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#endif
	    );
}

/*
 * "haslocaldir()" function
 */
    நிலையான வெறுமை
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = வெற்று;
    win_T	*wp = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_opt_எண்_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 1) == FAIL)))
	திரும்பு;

    wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    எனில் (wp != வெற்று && wp->w_localdir != வெற்று)
	rettv->vval.v_எண் = 1;
    இல்லையெனில் எனில் (tp != வெற்று && tp->tp_localdir != வெற்று)
	rettv->vval.v_எண் = 2;
    இல்லையெனில்
	rettv->vval.v_எண் = 0;
}

/*
 * "highlightID(name)" function
 */
    நிலையான வெறுமை
f_hlID(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    நிலையான வெறுமை
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    நிலையான வெறுமை
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
# எனில் defined(HAVE_NO_LONG_LONG)
வகைவரை unsigned முழு uintptr_t;
# இல்லையெனில்
வகைவரை unsigned long long uintptr_t;
# endif
#endif // VMS

    நிலையான வெறுமை
f_id(typval_T *argvars, typval_T *rettv)
{
    எழுத்து    numbuf[NUMBUFLEN];
    எழுத்து    *p = numbuf;

    தேர்வு (argvars[0].v_type)
    {
	நிலை VAR_LIST:
	நிலை VAR_TUPLE:
	நிலை VAR_DICT:
	நிலை VAR_OBJECT:
	நிலை VAR_JOB:
	நிலை VAR_CHANNEL:
	நிலை VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    எனில் (argvars[0].vval.v_object != வெற்று)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(வெறுமை *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		க்கு (; p < numbuf + அளவு(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	இயல்பு:
	    நிறுத்து;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    நிலையான வெறுமை
index_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    முழு		start = 0;
    முழு		error = FALSE;
    முழு		ic = FALSE;

    b = argvars[0].vval.v_blob;
    எனில் (b == வெற்று)
	திரும்பு;

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	start = tv_get_எண்_chk(&argvars[2], &error);
	எனில் (error)
	    திரும்பு;
    }

    எனில் (start < 0)
    {
	start = blob_len(b) + start;
	எனில் (start < 0)
	    start = 0;
    }

    க்கு (முழு idx = start; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_எண்;
	tv.vval.v_எண் = blob_get(b, idx);
	எனில் (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_எண் = idx;
	    திரும்பு;
	}
    }
}

/*
 * index() function for a tuple
 */
    நிலையான வெறுமை
index_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    முழு		ic = FALSE;
    முழு		error = FALSE;

    எனில் (tuple == வெற்று)
	திரும்பு;

    முழு	start_idx = 0;
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	start_idx = tv_get_எண்_chk(&argvars[2], &error);
	எனில் (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (முழு)tv_get_bool_chk(&argvars[3], &error);
	எனில் (error)
	    திரும்பு;
    }

    rettv->vval.v_எண் = index_tuple(tuple, &argvars[1], start_idx, ic);
}

/*
 * index() function for a list
 */
    நிலையான வெறுமை
index_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*item;
    long	idx = 0;
    முழு		ic = FALSE;
    முழு		error = FALSE;

    l = argvars[0].vval.v_list;
    எனில் (l == வெற்று)
	திரும்பு;

    CHECK_LIST_MATERIALIZE(l);
    item = l->lv_first;
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, (long)tv_get_எண்_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	எனில் (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (முழு)tv_get_bool_chk(&argvars[3], &error);
	எனில் (error)
	    item = வெற்று;
    }

    க்கு ( ; item != வெற்று; item = item->li_next, ++idx)
	எனில் (tv_equal(&item->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_எண் = idx;
	    நிறுத்து;
	}
}

/*
 * "index()" function
 */
    நிலையான வெறுமை
f_index(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_எண்_arg(argvars, 1) == FAIL)
		|| check_for_opt_எண்_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_BLOB)
	index_func_blob(argvars, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_TUPLE)
	index_func_tuple(argvars, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_LIST)
	index_func_list(argvars, rettv);
    இல்லையெனில்
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    முழு
indexof_eval_கோவை(typval_T *கோவை)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	found;
    முழு		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_சாவி);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    எனில் (eval_கோவை_typval(கோவை, FALSE, argv, 2, வெற்று, &newtv) == FAIL)
	திரும்பு FALSE;

    found = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    திரும்பு error ? FALSE : found;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    நிலையான முழு
indexof_blob(blob_T *b, long startidx, typval_T *கோவை)
{
    long	idx = 0;

    எனில் (b == வெற்று)
	திரும்பு -1;

    எனில் (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	எனில் (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_சாவி, VAR_எண்);
    set_vim_var_type(VV_VAL, VAR_எண்);

    முழு		called_emsg_start = called_emsg;
    க்கு (idx = startidx; idx < blob_len(b); ++idx)
    {
	set_vim_var_nr(VV_சாவி, idx);
	set_vim_var_nr(VV_VAL, blob_get(b, idx));

	எனில் (indexof_eval_கோவை(கோவை))
	    திரும்பு idx;

	எனில் (called_emsg != called_emsg_start)
	    திரும்பு -1;
    }

    திரும்பு -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    நிலையான முழு
indexof_list(list_T *l, long startidx, typval_T *கோவை)
{
    listitem_T	*item;
    long	idx = 0;
    முழு		found;

    எனில் (l == வெற்று)
	திரும்பு -1;

    CHECK_LIST_MATERIALIZE(l);

    எனில் (startidx == 0)
	item = l->lv_first;
    இல்லையெனில்
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, startidx);
	எனில் (item != வெற்று)
	    idx = l->lv_u.mat.lv_idx;
    }

    set_vim_var_type(VV_சாவி, VAR_எண்);

    முழு		called_emsg_start = called_emsg;
    க்கு ( ; item != வெற்று; item = item->li_next, ++idx)
    {
	set_vim_var_nr(VV_சாவி, idx);
	copy_tv(&item->li_tv, get_vim_var_tv(VV_VAL));

	found = indexof_eval_கோவை(கோவை);
	clear_tv(get_vim_var_tv(VV_VAL));

	எனில் (found)
	    திரும்பு idx;

	எனில் (called_emsg != called_emsg_start)
	    திரும்பு -1;
    }

    திரும்பு -1;
}

/*
 * "indexof()" function
 */
    நிலையான வெறுமை
f_indexof(typval_T *argvars, typval_T *rettv)
{
    long	startidx = 0;
    typval_T	save_val;
    typval_T	save_சாவி;
    முழு		save_did_emsg;

    rettv->vval.v_எண் = -1;

    எனில் (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	திரும்பு;

    எனில் ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == வெற்று
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == வெற்று))
	திரும்பு;

    எனில் (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_எண்_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_சாவி, &save_சாவி);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    எனில் (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_எண் = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_எண் = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    இல்லையெனில்
	rettv->vval.v_எண் = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_சாவி, &save_சாவி);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

நிலையான முழு inputsecret_flag = 0;

/*
 * "input()" function
 *     Also handles inputsecret() when inputsecret is set.
 */
    நிலையான வெறுமை
f_உள்ளீடு(typval_T *argvars, typval_T *rettv)
{
    get_user_உள்ளீடு(argvars, rettv, FALSE, inputsecret_flag);
}

/*
 * "inputdialog()" function
 */
    நிலையான வெறுமை
f_inputdialog(typval_T *argvars, typval_T *rettv)
{
#எனில் defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    எனில் (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == வெற்று)
    {
	char_u	*message;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	எனில் (in_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    திரும்பு;

	message = tv_get_string_chk(&argvars[0]);
	எனில் (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != வெற்று)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	இல்லையெனில்
	    IObuff[0] = NUL;
	எனில் (message != வெற்று && defstr != வெற்று
		&& do_dialog(VIM_QUESTION, வெற்று, message,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	இல்லையெனில்
	{
	    எனில் (message != வெற்று && defstr != வெற்று
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    இல்லையெனில்
		rettv->vval.v_string = வெற்று;
	}
	rettv->v_type = VAR_STRING;
    }
    இல்லையெனில்
#endif
	get_user_உள்ளீடு(argvars, rettv, TRUE, inputsecret_flag);
}

/*
 * "inputlist()" function
 */
    நிலையான வெறுமை
f_inputlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    முழு		selected;
    முழு		mouse_used;

#ifdef NO_CONSOLE_உள்ளீடு
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    எனில் (no_console_உள்ளீடு() && !is_not_a_term())
	திரும்பு;
#endif
    எனில் (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == வெற்று)
    {
	semsg(_(e_argument_of_str_must_be_list), "inputlist()");
	திரும்பு;
    }

    செய்தி_start();
    செய்தி_row = Rows - 1;	// for when 'cmdheight' > 1
    வரிகள்_இடது = Rows;	// avoid more prompt
    செய்தி_scroll = TRUE;
    செய்தி_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	செய்தி_puts((எழுத்து *)tv_get_string(&li->li_tv));
	செய்தி_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_எண்(&mouse_used);
    எனில் (mouse_used)
	selected -= வரிகள்_இடது;

    rettv->vval.v_எண் = selected;
}

நிலையான garray_T	    ga_userinput = {0, 0, அளவு(tasave_T), 4, வெற்று};

/*
 * "inputrestore()" function
 */
    நிலையான வெறுமை
f_inputrestore(typval_T *argvars UNUSED, typval_T *rettv)
{
    எனில் (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_data)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    இல்லையெனில் எனில் (p_verbose > 1)
    {
	verb_செய்தி(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_எண் = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    நிலையான வெறுமை
f_inputsave(typval_T *argvars UNUSED, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    எனில் (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_data)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    இல்லையெனில்
	rettv->vval.v_எண் = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    நிலையான வெறுமை
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    ++cmdline_star;
    ++inputsecret_flag;
    f_உள்ளீடு(argvars, rettv);
    --cmdline_star;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    நிலையான வெறுமை
f_interrupt(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    நிலையான வெறுமை
f_invert(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = ~tv_get_எண்_chk(&argvars[0], வெற்று);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    நிலையான வெறுமை
free_lval_வேர்(lval_வேர்_T *வேர்)
{
    எனில் (வேர்->lr_tv != வெற்று)
	free_tv(வேர்->lr_tv);
    class_unref(வேர்->lr_cl_exec);
    வேர்->lr_tv = வெற்று;
    வேர்->lr_cl_exec = வெற்று;
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

    நிலையான முழு
fix_variable_reference_lval_வேர்(lval_வேர்_T *வேர், char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    முழு found_member = FALSE;
    எனில் (வேர்->lr_tv->v_type == VAR_OBJECT)
    {
	எனில் (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    found_member = TRUE;
    }
    எனில் (!found_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*end;
	க்கு (end = name; ASCII_ISALNUM(*end) || *end == '_'; ++end)
	    ;
	முழு idx = class_member_idx(வேர்->lr_cl_exec, name, end - name);
	எனில் (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(வேர்->lr_tv);
	    copy_tv(&வேர்->lr_cl_exec->class_members_tv[idx], வேர்->lr_tv);
	    found_member = TRUE;
	}
    }
    எனில் (!found_member)
    {
	free_tv(வேர்->lr_tv);
	வேர்->lr_tv = வெற்று;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    திரும்பு OK;
}

/*
 * "islocked()" function
 */
    நிலையான வெறுமை
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*end;
    dictitem_T	*di;

    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    char_u *name = tv_get_string(&argvars[0]);
#ifdef LOG_LOCKVAR
    ch_log(வெற்று, "LKVAR: f_islocked(): name: %s", name);
#endif

    lval_வேர்_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_வேர்_T *வேர் = வெற்று;

    // Set up lval_root if executing in a method.
    எனில் (fill_exec_lval_வேர்(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	எனில் (fix_variable_reference_lval_வேர்(&aroot, name) == OK)
	    வேர் = &aroot;
    }

    lval_வேர்_T	*lval_வேர்_save = lval_வேர்;
    lval_வேர் = வேர்;
    end = get_lval(name, வெற்று, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_START);
    lval_வேர் = lval_வேர்_save;

    எனில் (end != வெற்று && lv.ll_name != வெற்று)
    {
	எனில் (*end != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_end
		   ? e_invalid_argument_str : e_trailing_characters_str), end);
	}
	இல்லையெனில்
	{
	    எனில் (lv.ll_tv == வெற்று)
	    {
		di = find_var(lv.ll_name, வெற்று, TRUE);
		எனில் (di != வெற்று)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_எண் = ((di->di_flags & DI_FLAGS_LOCK)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    இல்லையெனில் எனில் (lv.ll_is_வேர்)
	    {
		rettv->vval.v_எண் = tv_islocked(lv.ll_tv);
	    }
	    இல்லையெனில் எனில் (lv.ll_object != வெற்று)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_எண் = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(வெற்று, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#endif
	    }
	    இல்லையெனில் எனில் (lv.ll_class != வெற்று)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_எண் = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(வெற்று, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#endif
	    }
	    இல்லையெனில் எனில் (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    இல்லையெனில் எனில் (lv.ll_newkey != வெற்று)
		semsg(_(e_சாவி_not_present_in_dictionary_str), lv.ll_newkey);
	    இல்லையெனில் எனில் (lv.ll_list != வெற்று)
		// List item.
		rettv->vval.v_எண் = tv_islocked(&lv.ll_li->li_tv);
	    இல்லையெனில் எனில் (lv.ll_tuple != வெற்று)
		// Tuple item.
		rettv->vval.v_எண் = tv_islocked(lv.ll_tv);
	    இல்லையெனில்
		// Dictionary item.
		rettv->vval.v_எண் = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    எனில் (வேர் != வெற்று)
	free_lval_வேர்(வேர்);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    நிலையான வெறுமை
f_பொருட்கள்(typval_T *argvars, typval_T *rettv)
{
    எனில் (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    இல்லையெனில்
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), 1);
}

/*
 * "keytrans()" function
 */
    நிலையான வெறுமை
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    எனில் (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == வெற்று)
	திரும்பு;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    நிலையான வெறுமை
f_last_buffer_nr(typval_T *argvars UNUSED, typval_T *rettv)
{
    முழு		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	எனில் (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_எண் = n;
}

/*
 * "len()" function
 */
    வெறுமை
f_len(typval_T *argvars, typval_T *rettv)
{
    தேர்வு (argvars[0].v_type)
    {
	நிலை VAR_STRING:
	நிலை VAR_எண்:
	    rettv->vval.v_எண் = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    நிறுத்து;
	நிலை VAR_BLOB:
	    rettv->vval.v_எண் = blob_len(argvars[0].vval.v_blob);
	    நிறுத்து;
	நிலை VAR_LIST:
	    rettv->vval.v_எண் = list_len(argvars[0].vval.v_list);
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    rettv->vval.v_எண் = tuple_len(argvars[0].vval.v_tuple);
	    நிறுத்து;
	நிலை VAR_DICT:
	    rettv->vval.v_எண் = dict_len(argvars[0].vval.v_dict);
	    நிறுத்து;
	நிலை VAR_OBJECT:
	    rettv->vval.v_எண் = object_len(argvars[0].vval.v_object);
	    நிறுத்து;
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	நிலை VAR_FLOAT:
	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL:
	நிலை VAR_JOB:
	நிலை VAR_CHANNEL:
	நிலை VAR_INSTR:
	நிலை VAR_CLASS:
	நிலை VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    நிறுத்து;
    }
}

    நிலையான வெறுமை
libcall_common(typval_T *argvars UNUSED, typval_T *rettv, முழு type)
{
#ifdef FEAT_LIBCALL
    char_u		*string_in;
    char_u		**string_result;
    முழு			nr_result;
#endif

    rettv->v_type = type;
    எனில் (type != VAR_எண்)
	rettv->vval.v_string = வெற்று;

    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_எண்_arg(argvars, 2) == FAIL))
	திரும்பு;

#ifdef FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    எனில் (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_in = வெற்று;
	எனில் (argvars[2].v_type == VAR_STRING)
	    string_in = argvars[2].vval.v_string;
	எனில் (type == VAR_எண்)
	{
	    string_result = வெற்று;
	}
	இல்லையெனில்
	{
	    rettv->vval.v_string = வெற்று;
	    string_result = &rettv->vval.v_string;
	}
	எனில் (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_in,
			     argvars[2].vval.v_எண்,
			     string_result,
			     &nr_result) == OK
		&& type == VAR_எண்)
	    rettv->vval.v_எண் = nr_result;
    }
#endif
}

/*
 * "libcall()" function
 */
    நிலையான வெறுமை
f_libcall(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_STRING);
}

/*
 * "libcallnr()" function
 */
    நிலையான வெறுமை
f_libcallnr(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_எண்);
}

/*
 * "line(string, [winid])" function
 */
    நிலையான வெறுமை
f_வரி(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum = 0;
    pos_T	*fp = வெற்று;
    முழு		fnum;
    முழு		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (முழு)tv_get_எண்(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	எனில் (wp != வெற்று && tp != வெற்று)
	{
	    எனில் (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		எனில் (*p_spk != 'c'
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
    இல்லையெனில்
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    எனில் (fp != வெற்று)
	lnum = fp->lnum;
    rettv->vval.v_எண் = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    நிலையான வெறுமை
f_line2byte(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_எண் = -1;
#இல்லையெனில்
    linenr_T	lnum;

    எனில் (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	திரும்பு;

    lnum = tv_get_lnum(argvars);
    எனில் (lnum < 1 || lnum > curbuf->b_ml.ml_வரி_மொத்தம் + 1)
	rettv->vval.v_எண் = -1;
    இல்லையெனில்
	rettv->vval.v_எண் = ml_find_வரி_or_offset(curbuf, lnum, வெற்று);
    எனில் (rettv->vval.v_எண் >= 0)
	++rettv->vval.v_எண்;
#endif
}

#ifdef FEAT_LUA
/*
 * "luaeval()" function
 */
    நிலையான வெறுமை
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#endif

வகைவரை எண்ணிக்கை
{
    MATCH_END,	    // matchend()
    MATCH_MATCH,    // match()
    MATCH_STR,	    // matchstr()
    MATCH_LIST,	    // matchlist()
    MATCH_POS	    // matchstrpos()
} matchtype_T;

    நிலையான வெறுமை
find_some_match(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = வெற்று;
    long	len = 0;
    char_u	*கோவை = வெற்று;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    long	start = 0;
    long	nth = 1;
    colnr_T	startcol = 0;
    முழு		match = 0;
    list_T	*l = வெற்று;
    listitem_T	*li = வெற்று;
    long	idx = 0;
    முழு		prev_lock = 0;
    char_u	*tofree = வெற்று;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    rettv->vval.v_எண் = -1;
    எனில் (type == MATCH_LIST || type == MATCH_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	எனில் (rettv_list_alloc(rettv) == FAIL)
	    செல் theend;
	எனில் (type == MATCH_POS
		&& (list_append_string(rettv->vval.v_list,
					    (char_u *)"", 0) == FAIL
		    || list_append_எண்(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_எண்(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_எண்(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL))
	{
		list_free(rettv->vval.v_list);
		rettv->vval.v_list = வெற்று;
		செல் theend;
	}
    }
    இல்லையெனில் எனில் (type == MATCH_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = வெற்று;
    }

    எனில் (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_எண்_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 3) == FAIL)))
	செல் theend;

    எனில் (argvars[0].v_type == VAR_LIST)
    {
	எனில் ((l = argvars[0].vval.v_list) == வெற்று)
	    செல் theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    இல்லையெனில்
    {
	கோவை = str = tv_get_string(&argvars[0]);
	len = (long)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    எனில் (pat == வெற்று)
	செல் theend;

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	முழு	    error = FALSE;

	start = (long)tv_get_எண்_chk(&argvars[2], &error);
	எனில் (error)
	    செல் theend;
	எனில் (l != வெற்று)
	{
	    li = list_find(l, start);
	    எனில் (li == வெற்று)
		செல் theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	இல்லையெனில்
	{
	    எனில் (start < 0)
		start = 0;
	    எனில் (start > len)
		செல் theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    எனில் (argvars[3].v_type != VAR_UNKNOWN)
		startcol = start;
	    இல்லையெனில்
	    {
		str += start;
		len -= start;
	    }
	}

	எனில் (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (long)tv_get_எண்_chk(&argvars[3], &error);
	எனில் (error)
	    செல் theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    எனில் (regmatch.regprog != வெற்று)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	எனில் (l != வெற்று)
	{
	    prev_lock = l->lv_lock;
	    எனில் (l->lv_lock == 0)
		l->lv_lock = VAR_LOCKED;
	}

	க்கு (;;)
	{
	    எனில் (l != வெற்று)
	    {
		எனில் (li == வெற்று)
		{
		    match = FALSE;
		    நிறுத்து;
		}
		vim_free(tofree);
		கோவை = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		எனில் (str == வெற்று)
		    நிறுத்து;
	    }

	    match = vim_regexec_nl(&regmatch, str, startcol);

	    எனில் (match && --nth <= 0)
		நிறுத்து;
	    எனில் (l == வெற்று && !match)
		நிறுத்து;

	    // Advance to just after the match.
	    எனில் (l != வெற்று)
	    {
		li = li->li_next;
		++idx;
	    }
	    இல்லையெனில்
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		எனில் (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    match = FALSE;
		    நிறுத்து;
		}
	    }
	}

	எனில் (match)
	{
	    எனில் (type == MATCH_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_first;
		listitem_T *li2 = li1->li_next;
		listitem_T *li3 = li2->li_next;
		listitem_T *li4 = li3->li_next;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_எண் =
				      (varnumber_T)(regmatch.startp[0] - கோவை);
		li4->li_tv.vval.v_எண் =
					(varnumber_T)(regmatch.endp[0] - கோவை);
		எனில் (l != வெற்று)
		    li2->li_tv.vval.v_எண் = (varnumber_T)idx;
	    }
	    இல்லையெனில் எனில் (type == MATCH_LIST)
	    {
		முழு i;

		// return list with matched string and submatches
		க்கு (i = 0; i < NSUBEXP; ++i)
		{
		    எனில் (regmatch.endp[i] == வெற்று)
		    {
			எனில் (list_append_string(rettv->vval.v_list,
							    வெற்று, 0) == FAIL)
			    நிறுத்து;
		    }
		    இல்லையெனில் எனில் (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(முழு)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			நிறுத்து;
		}
	    }
	    இல்லையெனில் எனில் (type == MATCH_STR)
	    {
		// return matched string
		எனில் (l != வெற்று)
		    copy_tv(&li->li_tv, rettv);
		இல்லையெனில்
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    இல்லையெனில் எனில் (l != வெற்று)
		rettv->vval.v_எண் = idx;
	    இல்லையெனில்
	    {
		எனில் (type != MATCH_END)
		    rettv->vval.v_எண் =
				      (varnumber_T)(regmatch.startp[0] - str);
		இல்லையெனில்
		    rettv->vval.v_எண் =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_எண் += (varnumber_T)(str - கோவை);
	    }
	}
	எனில் (l != வெற்று)
	    l->lv_lock = prev_lock;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    எனில் (type == MATCH_POS && l == வெற்று && rettv->vval.v_list != வெற்று)
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
    நிலையான முழு
get_matches_in_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    முழு		idx,
    முழு		submatches,
    முழு		matchbuf)
{
    long	len = (long)STRLEN(str);
    முழு		match = 0;
    colnr_T	startidx = 0;

    க்கு (;;)
    {
	match = vim_regexec_nl(rmp, str, startidx);
	எனில் (!match)
	    நிறுத்து;

	dict_T *d = dict_alloc();
	எனில் (d == வெற்று)
	    திரும்பு FAIL;
	எனில் (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    திரும்பு FAIL;
	}

	எனில் (dict_add_எண்(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    திரும்பு FAIL;

	எனில் (dict_add_எண்(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    திரும்பு FAIL;

	எனில் (dict_add_string_len(d, "text", rmp->startp[0],
		    (முழு)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    திரும்பு FAIL;

	எனில் (submatches)
	{
	    list_T *sml = list_alloc();
	    எனில் (sml == வெற்று)
		திரும்பு FAIL;

	    எனில் (dict_add_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		திரும்பு FAIL;
	    }

	    // return a list with the submatches
	    க்கு (முழு i = 1; i < NSUBEXP; ++i)
	    {
		எனில் (rmp->endp[i] == வெற்று)
		{
		    எனில் (list_append_string(sml, (char_u *)"", 0) == FAIL)
			திரும்பு FAIL;
		}
		இல்லையெனில் எனில் (list_append_string(sml, rmp->startp[i],
			    (முழு)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    திரும்பு FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	எனில் (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    நிறுத்து;
    }

    திரும்பு OK;
}

/*
 * "matchbufline()" function
 */
    நிலையான வெறுமை
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = வெற்று;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_எண் = -1;
    எனில் (rettv_list_alloc(rettv) != OK)
	திரும்பு;
    retlist = rettv->vval.v_list;

    எனில் (check_for_buffer_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	திரும்பு;

    முழு prev_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    எனில் (buf == வெற்று)
    {
	எனில் (did_emsg == prev_did_emsg)
	    semsg(_(e_invalid_buffer_name_str), tv_get_string(&argvars[0]));
	திரும்பு;
    }
    எனில் (buf->b_ml.ml_mfp == வெற்று)
    {
	emsg(_(e_buffer_is_not_loaded));
	திரும்பு;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    முழு		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    எனில் (did_emsg > did_emsg_before)
	திரும்பு;
    எனில் (slnum < 1)
    {
	semsg(_(e_invalid_மதிப்பு_for_argument_str), "lnum");
	திரும்பு;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    எனில் (did_emsg > did_emsg_before)
	திரும்பு;
    எனில் (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_மதிப்பு_for_argument_str), "end_lnum");
	திரும்பு;
    }

    எனில் (elnum > buf->b_ml.ml_வரி_மொத்தம்)
	elnum = buf->b_ml.ml_வரி_மொத்தம்;

    முழு		submatches = FALSE;
    எனில் (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	எனில் (d != வெற்று)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    எனில் (di != வெற்று)
	    {
		எனில் (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "submatches");
		    திரும்பு;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    எனில் (regmatch.regprog == வெற்று)
	செல் theend;
    regmatch.rm_ic = p_ic;

    போது (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	எனில் (get_matches_in_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    செல் cleanup;
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
    நிலையான வெறுமை
f_match(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_MATCH);
}

/*
 * "matchend()" function
 */
    நிலையான வெறுமை
f_matchend(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_END);
}

/*
 * "matchlist()" function
 */
    நிலையான வெறுமை
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_LIST);
}

/*
 * "matchstr()" function
 */
    நிலையான வெறுமை
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_STR);
}

/*
 * "matchstrlist()" function
 */
    நிலையான வெறுமை
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = வெற்று;
    char_u	*save_cpo;
    list_T	*l = வெற்று;
    listitem_T	*li = வெற்று;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_எண் = -1;
    எனில் (rettv_list_alloc(rettv) != OK)
	திரும்பு;
    retlist = rettv->vval.v_list;

    எனில் (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	திரும்பு;

    எனில் ((l = argvars[0].vval.v_list) == வெற்று)
	திரும்பு;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    எனில் (pat == வெற்று)
	திரும்பு;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    எனில் (regmatch.regprog == வெற்று)
	செல் theend;
    regmatch.rm_ic = p_ic;

    முழு		submatches = FALSE;
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	எனில் (d != வெற்று)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    எனில் (di != வெற்று)
	    {
		எனில் (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_மதிப்பு_for_argument_str), "submatches");
		    செல் cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    முழு idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	எனில் (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != வெற்று)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    எனில் (get_matches_in_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		செல் cleanup;
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
    நிலையான வெறுமை
f_matchstrpos(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_POS);
}

    நிலையான வெறுமை
max_min(typval_T *argvars, typval_T *rettv, முழு domax)
{
    varnumber_T	n = 0;

    எனில் (in_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = 0;

    எனில் (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = வெற்று;

	l = argvars[0].vval.v_list;
	எனில் (l != வெற்று && l->lv_len > 0)
	{
	    எனில் (l->lv_first == &range_list_item)
	    {
		எனில் ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_start;
		இல்லையெனில்
		    n = l->lv_u.nonmat.lv_start + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_எண் = n;
	    }
	    இல்லையெனில்
	    {
		FOR_ALL_LIST_பொருட்கள்(l, li)
		{
		    எனில் (tv == வெற்று)
			tv = &li->li_tv;
		    இல்லையெனில்
		    {
			முழு res;
			எனில் (typval_compare2(&li->li_tv, tv,
				domax ? கோவை_GREATER : கோவை_SMALLER, FALSE, &res) == FAIL)
			    திரும்பு;
			எனில் (res == OK)
			    tv = &li->li_tv;
		    }
		}

		எனில் (tv != வெற்று)
		    copy_tv(tv, rettv);
	    }
	}
    }
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	எனில் (tv != வெற்று)
	    copy_tv(tv, rettv);
    }
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	முழு		todo;
	typval_T	*tv = வெற்று;

	d = argvars[0].vval.v_dict;
	எனில் (d != வெற்று)
	{
	    todo = (முழு)d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_பொருட்கள்(&d->dv_hashtab, hi, todo)
	    {
		எனில் (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    எனில் (tv == வெற்று)
			tv = &HI2DI(hi)->di_tv;
		    இல்லையெனில்
		    {
			முழு res;
			எனில் (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? கோவை_GREATER : கோவை_SMALLER, FALSE, &res) == FAIL)
			    திரும்பு;
			எனில் (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	எனில் (tv != வெற்று)
	    copy_tv(tv, rettv);
    }
    இல்லையெனில்
	semsg(_(e_argument_of_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
}

/*
 * "max()" function
 */
    நிலையான வெறுமை
f_max(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, TRUE);
}

/*
 * "min()" function
 */
    நிலையான வெறுமை
f_min(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, FALSE);
}

#எனில் defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    நிலையான வெறுமை
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    வெறுமை
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
    நிலையான வெறுமை
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    எனில் (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	திரும்பு;

    க்கு (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	எனில் (lnum < 0 || lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	{
	    lnum = 0;
	    நிறுத்து;
	}
	எனில் (*skipwhite(ml_get(lnum)) != NUL)
	    நிறுத்து;
    }
    rettv->vval.v_எண் = lnum;
}


/*
 * "ngettext()" function
 */
    நிலையான வெறுமை
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#எனில் defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    எழுத்து *prev = வெற்று;
#endif

    எனில் (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_எண்_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_STRING;

    எனில் (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != வெற்று &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#எனில் defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((மாறிலி எழுத்து *)argvars[3].vval.v_string, (எழுத்து *)p_enc);
#endif

#எனில் defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((மாறிலி எழுத்து *)argvars[3].vval.v_string, (மாறிலி எழுத்து *)argvars[0].vval.v_string, (மாறிலி எழுத்து *)argvars[1].vval.v_string, (முழு)argvars[2].vval.v_எண்));
#இல்லையெனில்
	textdomain((மாறிலி எழுத்து *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((மாறிலி எழுத்து *)argvars[0].vval.v_string, (மாறிலி எழுத்து *)argvars[1].vval.v_string, argvars[2].vval.v_எண்));
	textdomain(VIMPACKAGE);
#endif

#எனில் defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	எனில் (prev != வெற்று)
	    bind_textdomain_codeset((மாறிலி எழுத்து *)argvars[3].vval.v_string, prev);
#endif
    }
    இல்லையெனில்
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((மாறிலி எழுத்து *)argvars[0].vval.v_string, (மாறிலி எழுத்து *)argvars[1].vval.v_string, argvars[2].vval.v_எண்));
}


/*
 * "nr2char()" function
 */
    நிலையான வெறுமை
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (has_mbyte)
    {
	முழு	utf8 = 0;

	எனில் (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (முழு)tv_get_bool_chk(&argvars[1], வெற்று);
	எனில் (utf8)
	    buf[utf_char2bytes((முழு)tv_get_எண்(&argvars[0]), buf)] = NUL;
	இல்லையெனில்
	    buf[(*mb_char2bytes)((முழு)tv_get_எண்(&argvars[0]), buf)] = NUL;
    }
    இல்லையெனில்
    {
	buf[0] = (char_u)tv_get_எண்(&argvars[0]);
	buf[1] = NUL;
    }
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "or(expr, expr)" function
 */
    நிலையான வெறுமை
f_or(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    rettv->vval.v_எண் = tv_get_எண்_chk(&argvars[0], வெற்று)
					| tv_get_எண்_chk(&argvars[1], வெற்று);
}

#ifdef FEAT_PERL
/*
 * "perleval()" function
 */
    நிலையான வெறுமை
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#endif

/*
 * "prevnonblank()" function
 */
    நிலையான வெறுமை
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    எனில் (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	திரும்பு;

    lnum = tv_get_lnum(argvars);
    எனில் (lnum < 1 || lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	lnum = 0;
    இல்லையெனில்
	போது (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
	    --lnum;
    rettv->vval.v_எண் = lnum;
}

// This dummy va_list is here because:
// - passing a NULL pointer doesn't work when va_list isn't a pointer
// - locally in the function results in a "used before set" warning
// - using va_start() to initialize it gives "function with fixed args" error
நிலையான va_list	ap;

/*
 * "printf()" function
 */
    நிலையான வெறுமை
f_printf(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    முழு		len;
    char_u	*s;
    முழு		saved_did_emsg = did_emsg;
    எழுத்து	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script() && check_for_string_or_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (எழுத்து *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(வெற்று, 0, fmt, ap, argvars + 1);
    எனில் (!did_emsg)
    {
	s = alloc(len + 1);
	எனில் (s != வெற்று)
	{
	    rettv->vval.v_string = s;
	    (வெறுமை)vim_vsnprintf_typval((எழுத்து *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    நிலையான வெறுமை
f_pum_getpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;
    pum_set_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    நிலையான வெறுமை
f_pumvisible(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    எனில் (pum_visible())
	rettv->vval.v_எண் = 1;
}

#ifdef FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    நிலையான வெறுமை
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (p_pyx == 0)
	p_pyx = 3;

    எனில் (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    இல்லையெனில் எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	திரும்பு;
    }
    இல்லையெனில்
    {
	locals = வெற்று;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#endif

#ifdef FEAT_PYTHON
/*
 * "pyeval()" function
 */
    நிலையான வெறுமை
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	திரும்பு;

    எனில் (p_pyx == 0)
	p_pyx = 2;

    எனில் (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    இல்லையெனில் எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	திரும்பு;
    }
    இல்லையெனில்
    {
	locals = வெற்று;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#endif

#எனில் defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    நிலையான வெறுமை
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

# எனில் defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    எனில் (p_pyx == 2)
	f_pyeval(argvars, rettv);
    இல்லையெனில்
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#endif

நிலையான UINT32_T srand_seed_for_testing = 0;
நிலையான முழு	srand_seed_for_testing_is_used = FALSE;

    நிலையான வெறுமை
f_test_srand_seed(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_UNKNOWN)
	srand_seed_for_testing_is_used = FALSE;
    இல்லையெனில்
    {
	srand_seed_for_testing = (UINT32_T)tv_get_எண்(&argvars[0]);
	srand_seed_for_testing_is_used = TRUE;
    }
}

    நிலையான வெறுமை
init_srand(UINT32_T *x)
{
    கட்டமைப்பு {
	ஒன்றிப்பு {
	    UINT32_T எண்;
	    char_u   bytes[அளவு(UINT32_T)];
	} contents;
    } buf;

    எனில் (srand_seed_for_testing_is_used)
    {
	*x = srand_seed_for_testing;
	திரும்பு;
    }

    எனில் (mch_get_random(buf.contents.bytes, அளவு(buf.contents.bytes)) == OK)
    {
	*x = buf.contents.எண்;
	திரும்பு;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#எனில் defined(FEAT_SODIUM)
    எனில் (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    இல்லையெனில்
#endif
    {
#எனில் defined(FEAT_RELTIME)
	proftime_T res;
	profile_start(&res);
# எனில் defined(MSWIN)
	*x = (UINT32_T)res.கீழ்_Part;
# இல்லையெனில்
	*x = (UINT32_T)res.tv_fsec;
# endif
#இல்லையெனில்
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
    நிலையான வெறுமை
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = வெற்று;
    நிலையான UINT32_T	gx, gy, gz, gw;
    நிலையான முழு	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, result;

    எனில் (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_UNKNOWN)
    {
	// When no argument is given use the global seed list.
	எனில் (initialized == FALSE)
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
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	எனில் (l == வெற்று || list_len(l) != 4)
	    செல் theend;

	lx = list_find(l, 0L);
	ly = list_find(l, 1L);
	lz = list_find(l, 2L);
	lw = list_find(l, 3L);
	எனில் (lx == வெற்று || ly == வெற்று || lz == வெற்று || lw == வெற்று)
	    செல் theend;
	எனில் (lx->li_tv.v_type != VAR_எண்
		|| ly->li_tv.v_type != VAR_எண்
		|| lz->li_tv.v_type != VAR_எண்
		|| lw->li_tv.v_type != VAR_எண்)
	    செல் theend;
	x = (UINT32_T)lx->li_tv.vval.v_எண்;
	y = (UINT32_T)ly->li_tv.vval.v_எண்;
	z = (UINT32_T)lz->li_tv.vval.v_எண்;
	w = (UINT32_T)lw->li_tv.vval.v_எண்;

	SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w);

	lx->li_tv.vval.v_எண் = (varnumber_T)x;
	ly->li_tv.vval.v_எண் = (varnumber_T)y;
	lz->li_tv.vval.v_எண் = (varnumber_T)z;
	lw->li_tv.vval.v_எண் = (varnumber_T)w;
    }
    இல்லையெனில்
	செல் theend;

    rettv->v_type = VAR_எண்;
    rettv->vval.v_எண் = (varnumber_T)result;
    திரும்பு;

theend:
    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    rettv->v_type = VAR_எண்;
    rettv->vval.v_எண் = -1;
}

/*
 * "srand()" function
 */
    நிலையான வெறுமை
f_srand(typval_T *argvars, typval_T *rettv)
{
    UINT32_T x = 0, z;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    இல்லையெனில்
    {
	முழு	    error = FALSE;

	x = (UINT32_T)tv_get_எண்_chk(&argvars[0], &error);
	எனில் (error)
	    திரும்பு;
    }

    list_append_எண்(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_எண்(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_எண்(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_எண்(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#undef ROTL
#undef SPLITMIX32
#undef SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    நிலையான வெறுமை
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	start;
    varnumber_T	end;
    varnumber_T	stride = 1;
    முழு		error = FALSE;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 2) == FAIL)))
	திரும்பு;

    start = tv_get_எண்_chk(&argvars[0], &error);
    எனில் (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = start - 1;
	start = 0;
    }
    இல்லையெனில்
    {
	end = tv_get_எண்_chk(&argvars[1], &error);
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_எண்_chk(&argvars[2], &error);
    }

    எனில் (error)
	திரும்பு;		// type error; errmsg already given
    எனில் (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	திரும்பு;
    }

    // The stride is stored in "lv_stride", which is an int.
    எனில் (stride < INT_MIN || stride > INT_MAX)
    {
	எழுத்து	buf[NUMBUFLEN];

	vim_snprintf(buf, அளவு(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	திரும்பு;
    }

    uvarnumber_T	len;

    எனில் (stride > 0 ? end < start : end > start)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	back = stride > 0
				  ? (uvarnumber_T)start - (uvarnumber_T)end
				  : (uvarnumber_T)end - (uvarnumber_T)start;

	எனில் (back > 1)
	{
	    emsg(_(e_start_past_end));
	    திரும்பு;
	}
	len = 0;
    }
    இல்லையெனில்
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	span = stride > 0
				  ? (uvarnumber_T)end - (uvarnumber_T)start
				  : (uvarnumber_T)start - (uvarnumber_T)end;
	uvarnumber_T	மொத்தம் = span / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	எனில் (மொத்தம் >= (uvarnumber_T)INT_MAX)
	{
	    எழுத்து	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, அளவு(buf), "%llu",
			      மொத்தம் < UVARNUM_MAX ? மொத்தம் + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    திரும்பு;
	}
	len = மொத்தம் + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_first = &range_list_item;
    list->lv_u.nonmat.lv_start = start;
    list->lv_u.nonmat.lv_end = end;
    list->lv_u.nonmat.lv_stride = (முழு)stride;
    list->lv_len = (முழு)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    வெறுமை
range_list_materialize(list_T *list)
{
    varnumber_T start = list->lv_u.nonmat.lv_start;
    varnumber_T end = list->lv_u.nonmat.lv_end;
    முழு		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_first = வெற்று;
    list->lv_u.mat.lv_last = வெற்று;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_item = வெற்று;
    க்கு (i = start; stride > 0 ? i <= end : i >= end; i += stride)
    {
	எனில் (list_append_எண்(list, i) == FAIL)
	    நிறுத்து;
	எனில் (list->lv_lock & VAR_பொருட்கள்_LOCKED)
	    list->lv_u.mat.lv_last->li_tv.v_lock = VAR_LOCKED;
    }
    list->lv_lock &= ~VAR_பொருட்கள்_LOCKED;
}

/*
 * "getreginfo()" function
 */
    நிலையான வெறுமை
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    முழு		regname;
    char_u	buf[NUMBUFLEN + 2];
    size_t	buflen;
    long	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    எனில் (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    regname = getreg_get_regname(argvars);
    எனில் (regname == 0)
	திரும்பு;

    எனில் (regname == '@')
	regname = '"';

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_கோவை_SRC | GREG_LIST);
    எனில் (list == வெற்று)
	திரும்பு;
    எனில் (dict_add_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    தேர்வு (get_reg_type(regname, &reglen))
    {
	நிலை MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    நிறுத்து;
	நிலை MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    நிறுத்து;
	நிலை MBLOCK:
	    buflen = vim_snprintf_safelen((எழுத்து *)buf, அளவு(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    நிறுத்து;
	இயல்பு:
	    buf[0] = NUL;
	    buflen = 0;
	    நிறுத்து;
    }
    (வெறுமை)dict_add_string_len(dict, (எழுத்து *)"regtype", buf, (முழு)buflen);

    buf[0] = get_register_name(get_unname_register());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    எனில் (regname == '"')
	(வெறுமை)dict_add_string_len(dict, (எழுத்து *)"points_to", buf, (முழு)buflen);
    இல்லையெனில்
    {
	dictitem_T	*item = dictitem_alloc((char_u *)"isunnamed");

	எனில் (item != வெற்று)
	{
	    item->di_tv.v_type = VAR_BOOL;
	    item->di_tv.vval.v_எண் = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (வெறுமை)dict_add(dict, item);
	}
    }
}

    நிலையான வெறுமை
return_register(முழு regname, typval_T *rettv)
{
    char_u buf[2] = {NUL, NUL};

    buf[0] = (char_u)regname;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(buf, (buf[0] == NUL) ? 0 : 1);
}

/*
 * "reg_executing()" function
 */
    நிலையான வெறுமை
f_reg_executing(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    நிலையான வெறுமை
f_reg_recording(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    நிலையான வெறுமை
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_எண் = -1;
    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    rettv->vval.v_எண் = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    நிலையான வெறுமை
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    முழு slen = list_len(l);

    எனில் (rettv_list_alloc(rettv) == FAIL
	    || l == வெற்று
	    || n <= 0
	    || slen == 0)
	திரும்பு;

    எனில் (check_repeat_மொத்தம்(slen, n) == FAIL)
	திரும்பு;

    போது (n-- > 0)
	எனில் (list_extend(rettv->vval.v_list, l, வெற்று) == FAIL)
	    நிறுத்து;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    நிலையான வெறுமை
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    முழு		slen;
    முழு		len;
    முழு		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    எனில் (rettv_blob_alloc(rettv) == FAIL
	    || blob == வெற்று
	    || n <= 0)
	திரும்பு;

    slen = blob->bv_ga.ga_len;
    எனில் (slen <= 0)
	திரும்பு;
    எனில் (check_repeat_மொத்தம்(slen, n) == FAIL)
	திரும்பு;
    len = slen * (முழு)n;

    எனில் (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	திரும்பு;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    க்கு (i = 0; i < slen; ++i)
	எனில் (blob_get(blob, i) != 0)
	    நிறுத்து;

    எனில் (i == slen)
	// No need to copy since all bytes are already zero
	திரும்பு;

    க்கு (i = 0; i < (முழு)n; ++i)
	blob_set_range(rettv->vval.v_blob,
		(long)i * slen, ((long)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    நிலையான வெறுமை
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    size_t	slen;
    முழு		len;
    char_u	*r;
    முழு		done;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    slen = STRLEN(p);
    எனில் (slen == 0 || n <= 0)
	திரும்பு;
    எனில் (check_repeat_மொத்தம்((varnumber_T)slen, n) == FAIL)
	திரும்பு;
    len = (முழு)slen * (முழு)n;

    r = alloc((size_t)len + 1);
    எனில் (r == வெற்று)
	திரும்பு;

    mch_memmove(r, p, slen);
    done = (முழு)slen;
    போது (done < len)
    {
	முழு copy_len = done;

	எனில் (copy_len > len - done)
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
    நிலையான வெறுமை
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    எனில் (in_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    n = tv_get_எண்(&argvars[1]);
    எனில் (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    இல்லையெனில்
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
    நிலையான முழு
get_தேடல்_arg(typval_T *varp, முழு *flagsp)
{
    முழு		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    முழு		mask;

    எனில் (varp->v_type == VAR_UNKNOWN)
	திரும்பு FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    எனில் (flags == வெற்று)
	திரும்பு 0;		// type error; errmsg already given
    போது (*flags != NUL)
    {
	தேர்வு (*flags)
	{
	    நிலை 'b': dir = BACKWARD; நிறுத்து;
	    நிலை 'w': p_ws = TRUE; நிறுத்து;
	    நிலை 'W': p_ws = FALSE; நிறுத்து;
	    இயல்பு:  mask = 0;
		      எனில் (flagsp != வெற்று)
			  தேர்வு (*flags)
			  {
			      நிலை 'c': mask = SP_START; நிறுத்து;
			      நிலை 'e': mask = SP_END; நிறுத்து;
			      நிலை 'm': mask = SP_RETCOUNT; நிறுத்து;
			      நிலை 'n': mask = SP_NOMOVE; நிறுத்து;
			      நிலை 'p': mask = SP_SUBPAT; நிறுத்து;
			      நிலை 'r': mask = SP_REPEAT; நிறுத்து;
			      நிலை 's': mask = SP_SETPCMARK; நிறுத்து;
			      நிலை 'z': mask = SP_COLUMN; நிறுத்து;
			  }
		      எனில் (mask == 0)
		      {
			  semsg(_(e_invalid_argument_str), flags);
			  dir = 0;
		      }
		      இல்லையெனில்
			  *flagsp |= mask;
	}
	எனில் (dir == 0)
	    நிறுத்து;
	++flags;
    }
    திரும்பு dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    நிலையான முழு
தேடல்_cmn(typval_T *argvars, pos_T *match_pos, முழு *flagsp)
{
    முழு		flags;
    char_u	*pat;
    size_t	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    முழு		save_p_ws = p_ws;
    முழு		dir;
    முழு		retval = 0;	// default: FAIL
    long	lnum_stop = 0;
#ifdef FEAT_RELTIME
    long	time_limit = 0;
#endif
    முழு		options = தேடல்_KEEP;
    முழு		subpatnum;
    searchit_arg_T sia;
    முழு		use_skip = FALSE;
    pos_T	firstpos;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_எண்_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_எண்_arg(argvars, 3) == FAIL)))))
	செல் theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_தேடல்_arg(&argvars[1], flagsp);	// may set p_ws
    எனில் (dir == 0)
	செல் theend;
    flags = *flagsp;
    எனில் (flags & SP_START)
	options |= தேடல்_START;
    எனில் (flags & SP_END)
	options |= தேடல்_END;
    எனில் (flags & SP_COLUMN)
	options |= தேடல்_COL;

    // Optional arguments: line number to stop searching, timeout and skip.
    எனில் (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (long)tv_get_எண்_chk(&argvars[2], வெற்று);
	எனில் (lnum_stop < 0)
	    செல் theend;
	எனில் (argvars[3].v_type != VAR_UNKNOWN)
	{
#ifdef FEAT_RELTIME
	    time_limit = (long)tv_get_எண்_chk(&argvars[3], வெற்று);
	    எனில் (time_limit < 0)
		செல் theend;
#endif
	    use_skip = eval_கோவை_valid_arg(&argvars[4]);
	}
    }

    /*
     * This function does not accept SP_REPEAT and SP_RETCOUNT flags.
     * Check to make sure only those flags are set.
     * Also, Only the SP_NOMOVE or the SP_SETPCMARK flag can be set. Both
     * flags cannot be set. Check for that condition also.
     */
    எனில் (((flags & (SP_REPEAT | SP_RETCOUNT)) != 0)
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	செல் theend;
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
    க்கு (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, வெற்று, dir, pat, patlen, 1L,
						     options, RE_தேடல், &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	எனில் (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	எனில் (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    நிறுத்து;
	எனில் (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    முழு	    do_skip;
	    முழு	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_கோவை_to_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    எனில் (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		நிறுத்து;
	    }
	    எனில் (!do_skip)
		நிறுத்து;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~தேடல்_START;
    }

    எனில் (subpatnum != FAIL)
    {
	எனில் (flags & SP_SUBPAT)
	    retval = subpatnum;
	இல்லையெனில்
	    retval = pos.lnum;
	எனில் (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	எனில் (match_pos != வெற்று)
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
    எனில் (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    இல்லையெனில்
	curwin->w_set_curswant = true;
theend:
    p_ws = save_p_ws;

    திரும்பு retval;
}

#ifdef FEAT_RUBY
/*
 * "rubyeval()" function
 */
    நிலையான வெறுமை
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#endif

/*
 * "screenattr()" function
 */
    நிலையான வெறுமை
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    முழு		row;
    முழு		col;
    முழு		c;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    row = (முழு)tv_get_எண்_chk(&argvars[0], வெற்று) - 1;
    col = (முழு)tv_get_எண்_chk(&argvars[1], வெற்று) - 1;
    எனில் (row < 0 || row >= screen_Rows
	    || col < 0 || col >= screen_Columns)
	c = -1;
    இல்லையெனில்
	c = ScreenAttrs[வரி_Offset[row] + col];
    rettv->vval.v_எண் = c;
}

/*
 * "screenchar()" function
 */
    நிலையான வெறுமை
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    முழு		row;
    முழு		col;
    முழு		c;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    row = (முழு)tv_get_எண்_chk(&argvars[0], வெற்று) - 1;
    col = (முழு)tv_get_எண்_chk(&argvars[1], வெற்று) - 1;
    எனில் (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	c = -1;
    இல்லையெனில்
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(row, col, buf, வெற்று);
	c = (*mb_ptr2char)(buf);
    }
    rettv->vval.v_எண் = c;
}

/*
 * "screenchars()" function
 */
    நிலையான வெறுமை
f_screenchars(typval_T *argvars, typval_T *rettv)
{
    முழு		row;
    முழு		col;
    முழு		c;
    முழு		i;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    row = (முழு)tv_get_எண்_chk(&argvars[0], வெற்று) - 1;
    col = (முழு)tv_get_எண்_chk(&argvars[1], வெற்று) - 1;
    எனில் (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	திரும்பு;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(row, col, buf, வெற்று);
    முழு pcc[MAX_MCO];
    எனில் (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    இல்லையெனில்
	c = (*mb_ptr2char)(buf);
    list_append_எண்(rettv->vval.v_list, (varnumber_T)c);

    எனில் (enc_utf8)
	க்கு (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_எண்(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    நிலையான வெறுமை
f_screencol(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_எண் = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    நிலையான வெறுமை
f_screenrow(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_எண் = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    நிலையான வெறுமை
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    முழு		row;
    முழு		col;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = வெற்று;
    rettv->v_type = VAR_STRING;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    row = (முழு)tv_get_எண்_chk(&argvars[0], வெற்று) - 1;
    col = (முழு)tv_get_எண்_chk(&argvars[1], வெற்று) - 1;
    எனில் (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	திரும்பு;

    screen_getbytes(row, col, buf, வெற்று);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    நிலையான வெறுமை
f_தேடல்(typval_T *argvars, typval_T *rettv)
{
    முழு		flags = 0;

    rettv->vval.v_எண் = தேடல்_cmn(argvars, வெற்று, &flags);
}

/*
 * "searchdecl()" function
 */
    நிலையான வெறுமை
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    முழு		locally = TRUE;
    முழு		thisblock = FALSE;
    முழு		error = FALSE;
    char_u	*name;

    rettv->vval.v_எண் = 1;	// default: FAIL

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	திரும்பு;

    name = tv_get_string_chk(&argvars[0]);
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(முழு)tv_get_bool_chk(&argvars[1], &error);
	எனில் (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (முழு)tv_get_bool_chk(&argvars[2], &error);
    }
    எனில் (!error && name != வெற்று)
	rettv->vval.v_எண் = find_decl(name, (முழு)STRLEN(name),
				     locally, thisblock, தேடல்_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    நிலையான முழு
searchpair_cmn(typval_T *argvars, pos_T *match_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    முழு		save_p_ws = p_ws;
    முழு		dir;
    முழு		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    முழு		retval = 0;		// default: FAIL
    long	lnum_stop = 0;
    long	time_limit = 0;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_எண்_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_எண்_arg(argvars, 6) == FAIL)))))
	செல் theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    எனில் (spat == வெற்று || mpat == வெற்று || epat == வெற்று)
	செல் theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_தேடல்_arg(&argvars[3], &flags); // may set p_ws
    எனில் (dir == 0)
	செல் theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    எனில் ((flags & (SP_END | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[3]));
	செல் theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    எனில் (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    எனில் (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = வெற்று;
    இல்லையெனில்
    {
	// Type is checked later.
	skip = &argvars[4];

	எனில் (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (long)tv_get_எண்_chk(&argvars[5], வெற்று);
	    எனில் (lnum_stop < 0)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[5]));
		செல் theend;
	    }
#ifdef FEAT_RELTIME
	    எனில் (argvars[6].v_type != VAR_UNKNOWN)
	    {
		time_limit = (long)tv_get_எண்_chk(&argvars[6], வெற்று);
		எனில் (time_limit < 0)
		{
		    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[6]));
		    செல் theend;
		}
	    }
#endif
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    match_pos, lnum_stop, time_limit);

theend:
    p_ws = save_p_ws;

    திரும்பு retval;
}

/*
 * "searchpair()" function
 */
    நிலையான வெறுமை
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_எண் = searchpair_cmn(argvars, வெற்று);
}

/*
 * "searchpairpos()" function
 */
    நிலையான வெறுமை
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    முழு		lnum = 0;
    முழு		col = 0;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (searchpair_cmn(argvars, &match_pos) > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_எண்(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_எண்(rettv->vval.v_list, (varnumber_T)col);
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
    முழு		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    முழு		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*match_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    long	time_limit UNUSED) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = வெற்று, *pat3 = வெற்று;
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
    முழு		n;
    முழு		r;
    முழு		nest = 1;
    முழு		use_skip = FALSE;
    முழு		err;
    முழு		options = தேடல்_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    எனில் (pat2 == வெற்று)
	செல் theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    எனில் (pat3 == வெற்று)
	செல் theend;
    pat2len = vim_snprintf((எழுத்து *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    எனில் (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    இல்லையெனில்
	pat3len = vim_snprintf((எழுத்து *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    எனில் (flags & SP_START)
	options |= தேடல்_START;

    எனில் (skip != வெற்று)
	use_skip = eval_கோவை_valid_arg(skip);

#ifdef FEAT_RELTIME
    எனில் (time_limit > 0)
	init_regexp_timeout(time_limit);
#endif
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    க்கு (;;)
    {
	searchit_arg_T sia;

	CLEAR_FIELD(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, வெற்று, dir, pat, patlen, 1L,
						     options, RE_தேடல், &sia);
	எனில் (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    நிறுத்து;

	எனில் (firstpos.lnum == 0)
	    firstpos = pos;
	எனில் (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    எனில் (dir == BACKWARD)
		decl(&pos);
	    இல்லையெனில்
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~தேடல்_START;

	// If the skip pattern matches, ignore this match.
	எனில் (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_கோவை_to_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    எனில் (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		நிறுத்து;
	    }
	    எனில் (r)
		தொடர்;
	}

	எனில் ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	இல்லையெனில்
	{
	    // Found end when searching forward or start when searching
	    // backward: end of (nested) pair; or found middle in outer pair.
	    எனில் (--nest == 1)
		pat = pat3;	// outer level, search for middle
	}

	எனில் (nest == 0)
	{
	    // Found the match: return matchcount or line number.
	    எனில் (flags & SP_RETCOUNT)
		++retval;
	    இல்லையெனில்
		retval = pos.lnum;
	    எனில் (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    எனில் (!(flags & SP_REPEAT))
		நிறுத்து;
	    nest = 1;	    // search for next unmatched
	}
    }

    எனில் (match_pos != வெற்று)
    {
	// Store the match cursor position
	match_pos->lnum = curwin->w_cursor.lnum;
	match_pos->col = curwin->w_cursor.col + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    எனில் ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#ifdef FEAT_RELTIME
    எனில் (time_limit > 0)
	disable_regexp_timeout();
#endif
    vim_free(pat2);
    vim_free(pat3);
    எனில் (p_cpo == empty_option)
	p_cpo = save_cpo;
    இல்லையெனில்
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	எனில் (*p_cpo == NUL)
	    set_option_மதிப்பு_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    திரும்பு retval;
}

/*
 * "searchpos()" function
 */
    நிலையான வெறுமை
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    முழு		lnum = 0;
    முழு		col = 0;
    முழு		n;
    முழு		flags = 0;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    n = தேடல்_cmn(argvars, &match_pos, &flags);
    எனில் (n > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_எண்(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_எண்(rettv->vval.v_list, (varnumber_T)col);
    எனில் (flags & SP_SUBPAT)
	list_append_எண்(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    நிலையான வெறுமை
set_position(typval_T *argvars, typval_T *rettv, முழு charpos)
{
    pos_T	pos;
    முழு		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	திரும்பு;

    name = tv_get_string_chk(argvars);
    எனில் (name == வெற்று)
	திரும்பு;

    எனில் (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	திரும்பு;

    எனில் (pos.col != MAXCOL && --pos.col < 0)
	pos.col = 0;
    எனில் ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	எனில் (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_set_curswant = false;
	}
	check_cursor();
	rettv->vval.v_எண் = 0;
    }
    இல்லையெனில் எனில் (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	எனில் (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_எண் = 0;
    }
    இல்லையெனில்
	emsg(_(e_invalid_argument));
}
/*
 * "setcharpos()" function
 */
    நிலையான வெறுமை
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, TRUE);
}

    நிலையான வெறுமை
f_setcharsearch(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    எனில் (check_for_dict_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் ((d = argvars[0].vval.v_dict) == வெற்று)
	திரும்பு;

    csearch = dict_get_string(d, "char", FALSE);
    எனில் (csearch != வெற்று)
    {
	எனில் (enc_utf8)
	{
	    முழு pcc[MAX_MCO];
	    முழு c = utfc_ptr2char(csearch, pcc);

	    set_last_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	இல்லையெனில்
	    set_last_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_find(d, (char_u *)"forward", -1);
    எனில் (di != வெற்று)
	set_csearch_direction((முழு)tv_get_எண்(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_find(d, (char_u *)"until", -1);
    எனில் (di != வெற்று)
	set_csearch_until(!!tv_get_எண்(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    நிலையான வெறுமை
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    நிலையான வெறுமை
f_setenv(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    எனில் (check_restricted() || check_secure())
	திரும்பு;

    name = tv_get_string_buf(&argvars[0], namebuf);
    எனில் (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_எண் == VVAL_NULL)
	vim_unsetenv_ext(name);
    இல்லையெனில்
	vim_setenv_ext(name, tv_get_string_buf(&argvars[1], valbuf));
}

/*
 * "setfperm({fname}, {mode})" function
 */
    நிலையான வெறுமை
f_setfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	modebuf[NUMBUFLEN];
    char_u	*mode_str;
    முழு		i;
    முழு		mask;
    முழு		mode = 0;

    rettv->vval.v_எண் = 0;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    fname = tv_get_string_chk(&argvars[0]);
    எனில் (fname == வெற்று)
	திரும்பு;
    mode_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    எனில் (mode_str == வெற்று)
	திரும்பு;
    எனில் (STRLEN(mode_str) != 9)
    {
	semsg(_(e_invalid_argument_str), mode_str);
	திரும்பு;
    }

    mask = 1;
    க்கு (i = 8; i >= 0; --i)
    {
	எனில் (mode_str[i] != '-')
	    mode |= mask;
	mask = mask << 1;
    }
    rettv->vval.v_எண் = mch_setperm(fname, mode) == OK;
}

/*
 * "setpos()" function
 */
    நிலையான வெறுமை
f_setpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    முழு
get_yank_type(char_u **pp, char_u *yank_type, long *block_len)
{
    char_u *stropt = *pp;
    தேர்வு (*stropt)
    {
	நிலை 'v': நிலை 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    நிறுத்து;
	நிலை 'V': நிலை 'l':	// line-wise selection
	    *yank_type = MLINE;
	    நிறுத்து;
	நிலை 'b': நிலை Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    எனில் (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*block_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    நிறுத்து;
	இயல்பு:
	    திரும்பு FAIL;
    }
    *pp = stropt;
    திரும்பு OK;
}

/*
 * "setreg()" function
 */
    நிலையான வெறுமை
f_setreg(typval_T *argvars, typval_T *rettv)
{
    முழு		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    முழு		append;
    char_u	yank_type;
    long	block_len;
    typval_T	*regcontents;
    முழு		pointreg;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	திரும்பு;

    pointreg = 0;
    regcontents = வெற்று;
    block_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_எண் = 1;		// FAIL is default

    எனில் (strregname == வெற்று)
	திரும்பு;		// type error; errmsg already given
    எனில் (in_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_register_name_must_be_one_char_str), strregname);
	திரும்பு;
    }
    regname = *strregname;
    எனில் (regname == 0 || regname == '@')
	regname = '"';

    எனில் (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	எனில் (d == வெற்று || d->dv_hashtab.ht_used == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {வெற்று, வெற்று};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    திரும்பு;
	}

	di = dict_find(d, (char_u *)"regcontents", -1);
	எனில் (di != வெற்று)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	எனில் (stropt != வெற்று)
	{
	    முழு ret = get_yank_type(&stropt, &yank_type, &block_len);

	    எனில் (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_மதிப்பு_for_argument_str), "value");
		திரும்பு;
	    }
	}

	எனில் (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    எனில் (stropt != வெற்று)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	இல்லையெனில் எனில் (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    இல்லையெனில்
	regcontents = &argvars[1];

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	எனில் (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    திரும்பு;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	எனில் (stropt == வெற்று)
	    திரும்பு;		// type error
	க்கு (; *stropt != NUL; ++stropt)
	    தேர்வு (*stropt)
	    {
		நிலை 'a': நிலை 'A':	// append
		    append = TRUE;
		    நிறுத்து;
		இயல்பு:
		    get_yank_type(&stropt, &yank_type, &block_len);
	    }
    }

    எனில் (regcontents && regcontents->v_type == VAR_LIST)
    {
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	list_T		*ll = regcontents->vval.v_list;
	listitem_T	*li;
	முழு		len;

	// If the list is NULL handle like an empty list.
	len = ll == வெற்று ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	எனில் (lstval == வெற்று)
	    திரும்பு;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	எனில் (ll != வெற்று)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_பொருட்கள்(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		எனில் (strval == வெற்று)
		    செல் free_lstval;
		எனில் (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    எனில் (strval == வெற்று)
			செல் free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = வெற்று;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, block_len);
free_lstval:
	போது (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    இல்லையெனில் எனில் (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	எனில் (strval == வெற்று)
	    திரும்பு;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, block_len);
    }
    எனில் (pointreg != 0)
	get_yank_register(pointreg, TRUE);

    rettv->vval.v_எண் = 0;
}

/*
 * "settagstack()" function
 */
    நிலையான வெறுமை
f_settagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    dict_T	*d;
    முழு		action = 'r';

    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	திரும்பு;

    // first argument: window number or id
    wp = find_win_by_nr_or_id(&argvars[0]);
    எனில் (wp == வெற்று)
	திரும்பு;

    // second argument: dict with items to set in the tag stack
    எனில் (check_for_dict_arg(argvars, 1) == FAIL)
	திரும்பு;
    d = argvars[1].vval.v_dict;
    எனில் (d == வெற்று)
	திரும்பு;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    எனில் (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    இல்லையெனில் எனில் (check_for_string_arg(argvars, 2) == FAIL)
	திரும்பு;
    இல்லையெனில்
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	எனில் (actstr == வெற்று)
	    திரும்பு;
	எனில் ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	இல்லையெனில்
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    திரும்பு;
	}
    }

    எனில் (set_tagstack(wp, d, action) == OK)
	rettv->vval.v_எண் = 0;
}

#ifdef FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    நிலையான வெறுமை
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    முழு		len;

    எனில் (in_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != வெற்று ? (char_u *)blob->bv_ga.ga_data : (char_u *)"";
	len = blob != வெற்று ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_bytes(p, len, வெற்று, 0));
    }
    இல்லையெனில்
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_bytes(p, (முழு)STRLEN(p), வெற்று, 0));
    }
}
#endif // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    நிலையான வெறுமை
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    முழு do_special;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	திரும்பு;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    நிலையான வெறுமை
f_shiftwidth(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_எண் = 0;

    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	long	col;

	col = (long)tv_get_எண்_chk(argvars, வெற்று);
	எனில் (col < 0)
	    திரும்பு;	// type error; errmsg already given
#ifdef FEAT_VARTABS
	rettv->vval.v_எண் = get_sw_மதிப்பு_col(curbuf, col, FALSE);
	திரும்பு;
#endif
    }

    rettv->vval.v_எண் = get_sw_மதிப்பு(curbuf);
}

/*
 * "soundfold({word})" function
 */
    நிலையான வெறுமை
f_soundfold(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#ifdef FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#இல்லையெனில்
    rettv->vval.v_string = vim_strsave(s);
#endif
}

/*
 * "spellbadword()" function
 */
    நிலையான வெறுமை
f_spellbadword(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*word = (char_u *)"";
    hlf_T	attr = HLF_மொத்தம்;
    முழு		len = 0;
#ifdef FEAT_SPELL
    முழு		wo_spell_save = curwin->w_p_spell;

    எனில் (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    எனில் (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	திரும்பு;
    }
#endif

    எனில் (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	திரும்பு;
    }

#ifdef FEAT_SPELL
    எனில் (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_to(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	எனில் (len != 0)
	{
	    word = ml_get_cursor();
	    curwin->w_set_curswant = true;
	}
    }
    இல்லையெனில் எனில் (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	முழு	capcol = -1;

	எனில் (str != வெற்று)
	{
	    // Check the argument for spelling.
	    போது (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		எனில் (attr != HLF_மொத்தம்)
		{
		    word = str;
		    நிறுத்து;
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
    தேர்வு (attr)
    {
    நிலை HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	நிறுத்து;

    நிலை HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	நிறுத்து;

    நிலை HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	நிறுத்து;

    நிலை HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	நிறுத்து;

    இயல்பு:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	நிறுத்து;
    }
}

/*
 * "spellsuggest()" function
 */
    நிலையான வெறுமை
f_spellsuggest(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SPELL
    char_u	*str;
    முழு		typeerr = FALSE;
    முழு		maxcount;
    garray_T	ga;
    முழு		i;
    listitem_T	*li;
    முழு		need_capital = FALSE;
    முழு		wo_spell_save = curwin->w_p_spell;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	திரும்பு;

    எனில் (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    எனில் (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	திரும்பு;
    }
#endif

    எனில் (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	திரும்பு;
    }

#ifdef FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (முழு)tv_get_எண்_chk(&argvars[1], &typeerr);
	எனில் (maxcount <= 0)
	    திரும்பு;
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (முழு)tv_get_bool_chk(&argvars[2], &typeerr);
	    எனில் (typeerr)
		திரும்பு;
	}
    }
    இல்லையெனில்
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    க்கு (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_data)[i];

	li = listitem_alloc();
	எனில் (li == வெற்று)
	    vim_free(str);
	இல்லையெனில்
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

    நிலையான வெறுமை
f_split(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*end;
    char_u	*pat = வெற்று;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    முழு		match;
    colnr_T	col = 0;
    முழு		keepempty = FALSE;
    முழு		typeerr = FALSE;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	திரும்பு;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    str = tv_get_string(&argvars[0]);
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	எனில் (pat == வெற்று)
	    typeerr = TRUE;
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (முழு)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    எனில் (pat == வெற்று || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    எனில் (rettv_list_alloc(rettv) == FAIL)
	செல் theend;
    எனில் (typeerr)
	செல் theend;

    regmatch.regprog = eval_regcomp(pat);
    எனில் (regmatch.regprog != வெற்று)
    {
	regmatch.rm_ic = FALSE;
	போது (*str != NUL || keepempty)
	{
	    எனில் (*str == NUL)
		match = FALSE;	// empty item at the end
	    இல்லையெனில்
		match = vim_regexec_nl(&regmatch, str, col);
	    எனில் (match)
		end = regmatch.startp[0];
	    இல்லையெனில்
		end = str + STRLEN(str);
	    எனில் (keepempty || end > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && match && end < regmatch.endp[0]))
	    {
		எனில் (list_append_string(rettv->vval.v_list, str,
						    (முழு)(end - str)) == FAIL)
		    நிறுத்து;
	    }
	    எனில் (!match)
		நிறுத்து;
	    // Advance to just after the match.
	    எனில் (regmatch.endp[0] > str)
		col = 0;
	    இல்லையெனில்
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
    நிலையான வெறுமை
f_submatch(typval_T *argvars, typval_T *rettv)
{
    முழு		error = FALSE;
    முழு		no;
    முழு		retList = 0;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	திரும்பு;

    no = (முழு)tv_get_எண்_chk(&argvars[0], &error);
    எனில் (error)
	திரும்பு;
    எனில் (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_எண்_nr), no);
	திரும்பு;
    }
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
	retList = (முழு)tv_get_bool_chk(&argvars[1], &error);
    எனில் (error)
	திரும்பு;

    எனில் (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    இல்லையெனில்
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = reg_submatch_list(no);
    }
}

/*
 * "substitute()" function
 */
    நிலையான வெறுமை
f_substitute(typval_T *argvars, typval_T *rettv)
{
    char_u	patbuf[NUMBUFLEN];
    char_u	subbuf[NUMBUFLEN];
    char_u	flagsbuf[NUMBUFLEN];
    char_u	*str;
    char_u	*pat;
    char_u	*sub = வெற்று;
    typval_T	*கோவை = வெற்று;
    char_u	*flg;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	திரும்பு;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    எனில் (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	கோவை = &argvars[2];
    இல்லையெனில்
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    எனில் (str == வெற்று || pat == வெற்று || (sub == வெற்று && கோவை == வெற்று)
								|| flg == வெற்று)
	rettv->vval.v_string = வெற்று;
    இல்லையெனில்
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, கோவை, flg, வெற்று);
}

/*
 * "swapfilelist()" function
 */
    நிலையான வெறுமை
f_swapfilelist(typval_T *argvars UNUSED, typval_T *rettv)
{
    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;
    recover_names(வெற்று, FALSE, rettv->vval.v_list, 0, வெற்று);
}

/*
 * "swapinfo(swap_filename)" function
 */
    நிலையான வெறுமை
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (rettv_dict_alloc(rettv) == OK)
	get_b0_dict(tv_get_string(argvars), rettv->vval.v_dict);
}

/*
 * "swapname(expr)" function
 */
    நிலையான வெறுமை
f_swapname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = tv_get_buf(&argvars[0], FALSE);
    எனில் (buf == வெற்று || buf->b_ml.ml_mfp == வெற்று
					|| buf->b_ml.ml_mfp->mf_fname == வெற்று)
	rettv->vval.v_string = வெற்று;
    இல்லையெனில்
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    நிலையான வெறுமை
f_synID(typval_T *argvars UNUSED, typval_T *rettv)
{
    முழு		id = 0;
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    முழு		trans;
    முழு		transerr = FALSE;

    எனில் (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	திரும்பு;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (linenr_T)tv_get_எண்(&argvars[1]) - 1;	// -1 on type error
    trans = (முழு)tv_get_bool_chk(&argvars[2], &transerr);

    எனில் (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_வரி_மொத்தம்
	    && col >= 0 && col < (long)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, col, trans, வெற்று, FALSE);
#endif

    rettv->vval.v_எண் = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    நிலையான வெறுமை
f_synIDattr(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*p = வெற்று;
#ifdef FEAT_SYN_HL
    முழு		id;
    char_u	*what;
    char_u	*mode;
    char_u	modebuf[NUMBUFLEN];
    முழு		modec;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	திரும்பு;

    id = (முழு)tv_get_எண்(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	mode = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(mode[0]);
	எனில் (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    இல்லையெனில்
    {
# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	எனில் (USE_24BIT)
	    modec = 'g';
	இல்லையெனில்
# endif
	    எனில் (t_colors > 1)
		modec = 'c';
	    இல்லையெனில்
		modec = 't';
    }

    தேர்வு (TOLOWER_ASC(what[0]))
    {
	நிலை 'b':
		எனில் (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		இல்லையெனில்					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		நிறுத்து;

	நிலை 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		நிறுத்து;

	நிலை 'i':
		எனில் (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		இல்லையெனில்					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		நிறுத்து;

	நிலை 'n':
		எனில் (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		இல்லையெனில்					// name
		    p = get_highlight_name_ext(வெற்று, id - 1, FALSE);
		நிறுத்து;

	நிலை 'r':					// reverse
		p = highlight_has_attr(id, HL_INVERSE, modec);
		நிறுத்து;

	நிலை 's':
		எனில் (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		இல்லையெனில் எனில் (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		இல்லையெனில்					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		நிறுத்து;

	நிலை 'u':
		எனில் (STRLEN(what) >= 9)
		{
		    எனில் (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    இல்லையெனில் எனில் (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    இல்லையெனில் எனில் (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    இல்லையெனில் எனில் (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    இல்லையெனில்
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		இல்லையெனில்
							// ul
		    p = highlight_color(id, what, modec);
		நிறுத்து;
    }

    எனில் (p != வெற்று)
	p = vim_strsave(p);
#endif
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    நிலையான வெறுமை
f_synIDtrans(typval_T *argvars UNUSED, typval_T *rettv)
{
    முழு		id;

#ifdef FEAT_SYN_HL
    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    id = (முழு)tv_get_எண்(&argvars[0]);

    எனில் (id > 0)
	id = syn_get_final_id(id);
    இல்லையெனில்
#endif
	id = 0;

    rettv->vval.v_எண் = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    நிலையான வெறுமை
f_synconcealed(typval_T *argvars UNUSED, typval_T *rettv)
{
#எனில் defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	col;
    முழு		syntax_flags = 0;
    முழு		cchar;
    முழு		matchid = 0;
    char_u	str[NUMBUFLEN];
#endif

    rettv_list_set(rettv, வெற்று);

    எனில் (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

#எனில் defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_எண்(&argvars[1]) - 1;	// -1 on type error

    CLEAR_FIELD(str);

    எனில் (rettv_list_alloc(rettv) == OK)
    {
	எனில் (lnum >= 1 && lnum <= curbuf->b_ml.ml_வரி_மொத்தம்
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (வெறுமை)syn_get_id(curwin, lnum, col, FALSE, வெற்று, FALSE);
	    syntax_flags = get_syntax_info(&matchid);

	    // get the conceal character
	    எனில் ((syntax_flags & HL_CONCEAL) && curwin->w_p_cole < 3)
	    {
		cchar = syn_get_sub_char();
		எனில் (cchar == NUL && curwin->w_p_cole == 1)
		    cchar = (curwin->w_lcs_chars.conceal == NUL) ? ' '
					: curwin->w_lcs_chars.conceal;
		எனில் (cchar != NUL)
		{
		    எனில் (has_mbyte)
			(*mb_char2bytes)(cchar, str);
		    இல்லையெனில்
			str[0] = cchar;
		}
	    }
	}

	list_append_எண்(rettv->vval.v_list,
					    (syntax_flags & HL_CONCEAL) != 0);
	// -1 to auto-determine strlen
	list_append_string(rettv->vval.v_list, str, -1);
	list_append_எண்(rettv->vval.v_list, matchid);
    }
#endif
}

/*
 * "synstack(lnum, col)" function
 */
    நிலையான வெறுமை
f_synstack(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    முழு		i;
    முழு		id;
#endif

    rettv_list_set(rettv, வெற்று);

    எனில் (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

#ifdef FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_எண்(&argvars[1]) - 1;	// -1 on type error

    எனில் (lnum >= 1 && lnum <= curbuf->b_ml.ml_வரி_மொத்தம்
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(வெறுமை)syn_get_id(curwin, lnum, col, FALSE, வெற்று, TRUE);
	க்கு (i = 0; ; ++i)
	{
	    id = syn_get_அடுக்கு_item(i);
	    எனில் (id < 0)
		நிறுத்து;
	    எனில் (list_append_எண்(rettv->vval.v_list, id) == FAIL)
		நிறுத்து;
	}
    }
#endif
}

/*
 * "tabpagebuflist()" function
 */
    நிலையான வெறுமை
f_tabpagebuflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp = வெற்று;

    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    இல்லையெனில்
    {
	tp = find_tabpage((முழு)tv_get_எண்(&argvars[0]));
	எனில் (tp != வெற்று)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    எனில் (wp != வெற்று && rettv_list_alloc(rettv) == OK)
    {
	க்கு (; wp != வெற்று; wp = wp->w_next)
	    எனில் (list_append_எண்(rettv->vval.v_list,
						wp->w_buffer->b_fnum) == FAIL)
		நிறுத்து;
    }
}

/*
 * "tagfiles()" function
 */
    நிலையான வெறுமை
f_tagfiles(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    முழு		first;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;
    fname = alloc(MAXPATHL);
    எனில் (fname == வெற்று)
	திரும்பு;

    க்கு (first = TRUE; ; first = FALSE)
	எனில் (get_tagfname(&tn, first, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    நிறுத்து;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    நிலையான வெறுமை
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = வெற்று;
    char_u  *tag_pattern;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    tag_pattern = tv_get_string(&argvars[0]);

    rettv->vval.v_எண் = FALSE;
    எனில் (*tag_pattern == NUL)
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    எனில் (rettv_list_alloc(rettv) == OK)
	(வெறுமை)get_tags(rettv->vval.v_list, tag_pattern, fname);
}

/*
 * "type(expr)" function
 */
    நிலையான வெறுமை
f_type(typval_T *argvars, typval_T *rettv)
{
    முழு n = -1;

    தேர்வு (argvars[0].v_type)
    {
	நிலை VAR_எண்:  n = VAR_TYPE_எண்; நிறுத்து;
	நிலை VAR_STRING:  n = VAR_TYPE_STRING; நிறுத்து;
	நிலை VAR_PARTIAL:
	நிலை VAR_FUNC:    n = VAR_TYPE_FUNC; நிறுத்து;
	நிலை VAR_LIST:    n = VAR_TYPE_LIST; நிறுத்து;
	நிலை VAR_TUPLE:   n = VAR_TYPE_TUPLE; நிறுத்து;
	நிலை VAR_DICT:    n = VAR_TYPE_DICT; நிறுத்து;
	நிலை VAR_FLOAT:   n = VAR_TYPE_FLOAT; நிறுத்து;
	நிலை VAR_BOOL:	  n = VAR_TYPE_BOOL; நிறுத்து;
	நிலை VAR_SPECIAL: n = VAR_TYPE_NONE; நிறுத்து;
	நிலை VAR_JOB:     n = VAR_TYPE_JOB; நிறுத்து;
	நிலை VAR_CHANNEL: n = VAR_TYPE_CHANNEL; நிறுத்து;
	நிலை VAR_BLOB:    n = VAR_TYPE_BLOB; நிறுத்து;
	நிலை VAR_INSTR:   n = VAR_TYPE_INSTR; நிறுத்து;
	நிலை VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; நிறுத்து;
	நிலை VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		எனில் (cl != வெற்று && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		இல்லையெனில்
		    n = VAR_TYPE_CLASS;
		நிறுத்து;
	    }
	நிலை VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		எனில் (obj == வெற்று)
		    n = VAR_TYPE_OBJECT;
		இல்லையெனில்
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    எனில் (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    இல்லையெனில்
			n = VAR_TYPE_OBJECT;
		}
		நிறுத்து;
	    }
	நிலை VAR_VOID:
	    emsg(_(e_cannot_use_void_மதிப்பு));
	    நிறுத்து;
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	    internal_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    நிறுத்து;
    }
    rettv->vval.v_எண் = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    நிலையான வெறுமை
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_start = 0;
    colnr_T	vcol_end = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    முழு		winchanged = FALSE;
    முழு		len;

    எனில் (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_எண்_arg(argvars, 2) == FAIL))))
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((முழு)tv_get_எண்(&argvars[2]), &tp);
	எனில் (wp == வெற்று || tp == வெற்று)
	    செல் theend;

	எனில் (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    செல் theend;

	check_cursor();
	winchanged = TRUE;
    }

    முழு fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    எனில் (fp != வெற்று && fp->lnum <= curbuf->b_ml.ml_வரி_மொத்தம்
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	எனில் (fp->col < 0)
	    fp->col = 0;
	இல்லையெனில்
	{
	    len = (முழு)ml_get_len(fp->lnum);
	    எனில் (fp->col > len)
		fp->col = len;
	}
	getvvcol(curwin, fp, &vcol_start, வெற்று, &vcol_end, 0);
	++vcol_start;
	++vcol_end;
    }

theend:
    எனில் (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	எனில் (rettv_list_alloc(rettv) == OK)
	{
	    list_append_எண்(rettv->vval.v_list, vcol_start);
	    list_append_எண்(rettv->vval.v_list, vcol_end);
	}
	இல்லையெனில்
	    rettv->vval.v_எண் = 0;
    }
    இல்லையெனில்
	rettv->vval.v_எண் = vcol_end;

    எனில் (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    நிலையான வெறுமை
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    எனில் (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_mode_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    எனில் (non_zero_arg(&argvars[0]))
	curbuf->b_visual_mode_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    நிலையான வெறுமை
f_wildmenumode(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    எனில் (wild_menu_showing || ((State & MODE_CMDLINE) && cmdline_pum_active()))
	rettv->vval.v_எண் = 1;
}

/*
 * "windowsversion()" function
 */
    நிலையான வெறுமை
f_windowsversion(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windowsVersion);
}

/*
 * "wordcount()" function
 */
    நிலையான வெறுமை
f_wordcount(typval_T *argvars UNUSED, typval_T *rettv)
{
    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    நிலையான வெறுமை
f_xor(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    rettv->vval.v_எண் = tv_get_எண்_chk(&argvars[0], வெற்று)
					^ tv_get_எண்_chk(&argvars[1], வெற்று);
}

#endif // FEAT_EVAL
