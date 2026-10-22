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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

സ്ഥിതം ശൂന്യം f_and(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_BEVAL
സ്ഥിതം ശൂന്യം f_balloon_gettext(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_balloon_show(typval_T *argvars, typval_T *rettv);
# എങ്കിൽ defined(FEAT_BEVAL_TERM)
സ്ഥിതം ശൂന്യം f_balloon_split(typval_T *argvars, typval_T *rettv);
# endif
#endif
സ്ഥിതം ശൂന്യം f_base64_encode(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_base64_decode(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_bindtextdomain(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_byte2line(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_call(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_changenr(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_char2nr(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_charcol(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_col(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_confirm(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_copy(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_cursor(typval_T *argsvars, typval_T *rettv);
#ifdef MSWIN
സ്ഥിതം ശൂന്യം f_debugbreak(typval_T *argvars, typval_T *rettv);
#endif
സ്ഥിതം ശൂന്യം f_deepcopy(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_did_filetype(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_echoraw(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_empty(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_environ(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_err_teapot(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_escape(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_eval(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_eventhandler(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_execute(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_exists_compiled(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_expand(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_expandcmd(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_feedkeys(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_fnameescape(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_foreground(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_funcref(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_function(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_garbagecollect(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_get(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getbgcolor(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getcellpixels(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getchangelist(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getcharpos(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getcharsearch(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getcurpos(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getcursorcharpos(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getenv(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getfontname(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getinfo(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getjumplist(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getpid(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getpos(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getreg(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getreginfo(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getregion(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getregionpos(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_getregtype(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_gettagstack(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_gettext(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_haslocaldir(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_hlID(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_hlexists(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_hostname(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_id(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_index(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_indexof(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_നിവേശം(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_inputdialog(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_inputlist(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_inputrestore(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_inputsave(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_inputsecret(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_interrupt(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_invert(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_islocked(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_ഇനങ്ങൾ(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_keytrans(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_last_buffer_nr(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_libcall(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_libcallnr(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_വരി(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_line2byte(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_LUA
സ്ഥിതം ശൂന്യം f_luaeval(typval_T *argvars, typval_T *rettv);
#endif
സ്ഥിതം ശൂന്യം f_match(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_matchbufline(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_matchend(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_matchlist(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_matchstr(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_matchstrlist(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_matchstrpos(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_max(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_min(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_MZSCHEME
സ്ഥിതം ശൂന്യം f_mzeval(typval_T *argvars, typval_T *rettv);
#endif
സ്ഥിതം ശൂന്യം f_nextnonblank(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_ngettext(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_nr2char(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_or(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PERL
സ്ഥിതം ശൂന്യം f_perleval(typval_T *argvars, typval_T *rettv);
#endif
സ്ഥിതം ശൂന്യം f_prevnonblank(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_printf(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_pum_getpos(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_pumvisible(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PYTHON3
സ്ഥിതം ശൂന്യം f_py3eval(typval_T *argvars, typval_T *rettv);
#endif
#ifdef FEAT_PYTHON
സ്ഥിതം ശൂന്യം f_pyeval(typval_T *argvars, typval_T *rettv);
#endif
#എങ്കിൽ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
സ്ഥിതം ശൂന്യം f_pyxeval(typval_T *argvars, typval_T *rettv);
#endif
സ്ഥിതം ശൂന്യം f_test_srand_seed(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_rand(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_range(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_reg_executing(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_reg_recording(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_rename(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_repeat(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_RUBY
സ്ഥിതം ശൂന്യം f_rubyeval(typval_T *argvars, typval_T *rettv);
#endif
സ്ഥിതം ശൂന്യം f_screenattr(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_screenchar(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_screenchars(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_screencol(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_screenrow(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_screenstring(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_തിരയൽ(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_searchdecl(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_searchpair(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_searchpairpos(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_searchpos(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_setcharpos(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_setcharsearch(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_setcursorcharpos(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_setenv(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_setfperm(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_setpos(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_setreg(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_settagstack(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_CRYPT
സ്ഥിതം ശൂന്യം f_sha256(typval_T *argvars, typval_T *rettv);
#endif
സ്ഥിതം ശൂന്യം f_shellescape(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_shiftwidth(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_soundfold(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_spellbadword(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_spellsuggest(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_split(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_srand(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_submatch(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_substitute(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_swapfilelist(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_swapinfo(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_swapname(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_synID(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_synIDattr(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_synIDtrans(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_synstack(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_synconcealed(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_tabpagebuflist(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_taglist(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_tagfiles(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_type(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_virtcol(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_visualmode(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_wildmenumode(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_windowsversion(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_wordcount(typval_T *argvars, typval_T *rettv);
സ്ഥിതം ശൂന്യം f_xor(typval_T *argvars, typval_T *rettv);


/*
 * Functions that check the argument type of a builtin function.
 * Each function returns FAIL and gives an error message if the type is wrong.
 */

// Context passed to an arg_ function.
തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ		arg_എണ്ണം;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    പൂർണ്ണസംഖ്യ		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
തരനിർവചനം പൂർണ്ണസംഖ്യ (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    തിരികെ need_type(actual, expected, 0,
	    context->arg_idx - context->arg_എണ്ണം, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    എങ്കിൽ (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_എണ്ണം, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	തിരികെ FAIL;
    തിരികെ arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    പൂർണ്ണസംഖ്യ
arg_type_modifiable(type_T *type, പൂർണ്ണസംഖ്യ arg_idx)
{
    അക്ഷരം *tofree;

    എങ്കിൽ ((type->tt_flags & TTFLAG_CONST) == 0)
	തിരികെ OK;
    semsg(_(e_argument_nr_trying_to_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    തിരികെ FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_any(type_T *type UNUSED,
	type_T *decl_type UNUSED,
	argcontext_T *context UNUSED)
{
    തിരികെ OK;
}

/*
 * Check "type" is a float or a number.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_float_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_സംഖ്യ
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_സംഖ്യ, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a number.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_സംഖ്യ(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    തിരികെ check_arg_type(&t_സംഖ്യ, type, context);
}

/*
 * Check "type" is an object.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_object(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_dict_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    തിരികെ check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    തിരികെ check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    തിരികെ check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_സംഖ്യ(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    തിരികെ check_arg_type(&t_list_സംഖ്യ, type, context);
}

/*
 * Check "type" is a list of strings.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    തിരികെ check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_tuple_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    തിരികെ check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    തിരികെ check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    തിരികെ check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_bool(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    തിരികെ check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_bool_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_സംഖ്യ
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_സംഖ്യ, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	തിരികെ OK;
    തിരികെ check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_or_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    എങ്കിൽ (arg_list_or_blob(type, decl_type, context) == FAIL)
	തിരികെ FAIL;
    തിരികെ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    തിരികെ FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a string or a number
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_string_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_സംഖ്യ
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_buffer(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_സംഖ്യ
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_buffer_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_സംഖ്യ
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_lnum(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_സംഖ്യ
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	തിരികെ OK;
    എങ്കിൽ (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	തിരികെ FAIL;
    }
    എങ്കിൽ (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	തിരികെ OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    എങ്കിൽ (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	തിരികെ FAIL;
    തിരികെ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    എങ്കിൽ (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	തിരികെ FAIL;
    തിരികെ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	തിരികെ OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr),
						 context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = ശൂന്യ;
    type_T *(args[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, ശൂന്യ, ശൂന്യ, args};

    എങ്കിൽ (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	എങ്കിൽ (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	അല്ലെങ്കിൽ
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    അല്ലെങ്കിൽ എങ്കിൽ (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_സംഖ്യ;

    args[0] = ശൂന്യ;
    args[1] = &t_unknown;
    എങ്കിൽ (type->tt_argcount != -1)
    {
	എങ്കിൽ (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_സംഖ്യ_of_arguments));
	    തിരികെ FAIL;
	}
	എങ്കിൽ (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		args[0] = &t_സംഖ്യ;
	    അല്ലെങ്കിൽ എങ്കിൽ (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		args[0] = &t_string;
	    എങ്കിൽ (args[0] != ശൂന്യ)
		args[1] = expected_member;
	}
    }

    എങ്കിൽ (!type_any_or_unknown(type->tt_member) || args[0] != ശൂന്യ)
    {
	where_T where = WHERE_INIT;

	എങ്കിൽ (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == ശൂന്യ
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	അല്ലെങ്കിൽ എങ്കിൽ (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	അല്ലെങ്കിൽ // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	എങ്കിൽ (args[0] == ശൂന്യ)
	    args[0] = &t_unknown;
	എങ്കിൽ (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_index = 2;
	where.wt_kind = WT_ARGUMENT;
	തിരികെ check_type(&t_func_exp, type, TRUE, where);
    }
    തിരികെ OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_filter_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	തിരികെ OK;

    എങ്കിൽ (type->tt_type == VAR_FUNC)
	തിരികെ check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    തിരികെ FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_map_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	തിരികെ OK;

    എങ്കിൽ (type->tt_type == VAR_FUNC)
	തിരികെ check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    തിരികെ FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_foreach_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	തിരികെ OK;

    എങ്കിൽ (type->tt_type == VAR_FUNC)
	തിരികെ check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    തിരികെ FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_sort_how(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	തിരികെ OK;

    എങ്കിൽ (type->tt_type == VAR_FUNC)
    {
	type_T *(args[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_സംഖ്യ, ശൂന്യ, args};

	എങ്കിൽ (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    args[0] = context->arg_types[0].type_curr->tt_member;
	അല്ലെങ്കിൽ
	    args[0] = &t_unknown;
	എങ്കിൽ (!type_any_or_unknown(type->tt_member) || args[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    args[1] = args[0];
	    എങ്കിൽ (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_index = 2;
	    where.wt_kind = WT_ARGUMENT;
	    തിരികെ check_type(&t_func_exp, type, TRUE, where);
	}

	തിരികെ OK;
    }
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    തിരികെ FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_സംഖ്യ
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
varargs_class(type_T *type UNUSED,
	      type_T *decl_type UNUSED,
	      argcontext_T *context)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = context->arg_idx; i < context->arg_എണ്ണം; ++i)
    {
	type2_T *types = &context->arg_types[i];
	എങ്കിൽ (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    തിരികെ FAIL;
	}
    }
    തിരികെ OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_reverse(type_T *type, type_T *decl_type, argcontext_T *context)
{
    എങ്കിൽ (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	തിരികെ FAIL;
    തിരികെ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    തിരികെ check_arg_type(&t_job, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_chan_or_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_JOB
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_same_as_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_decl;

    തിരികെ check_arg_type(prev_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_same_struct_as_prev(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;

    എങ്കിൽ (prev_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	തിരികെ check_arg_type(prev_type, type, context);
    തിരികെ OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_item_of_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    എങ്കിൽ (prev_type->tt_type == VAR_LIST)
	expected = prev_type->tt_member;
    അല്ലെങ്കിൽ എങ്കിൽ (prev_type->tt_type == VAR_BLOB)
	expected = &t_സംഖ്യ;
    അല്ലെങ്കിൽ
	// probably VAR_ANY, can't check
	തിരികെ OK;

    തിരികെ check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_സംഖ്യ
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	തിരികെ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 2].type_curr;

    എങ്കിൽ (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	തിരികെ arg_സംഖ്യ(type, decl_type, context);
    എങ്കിൽ (first_type->tt_type == VAR_DICT)
	തിരികെ arg_string(type, decl_type, context);
    തിരികെ OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_get1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	തിരികെ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_len1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_സംഖ്യ
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	തിരികെ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 1].type_curr;

    എങ്കിൽ (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	തിരികെ arg_സംഖ്യ(type, decl_type, context);
    എങ്കിൽ (first_type->tt_type == VAR_DICT)
	തിരികെ arg_string_or_nr(type, decl_type, context);
    തിരികെ OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_repeat1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_സംഖ്യ
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	തിരികെ OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_slice1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	തിരികെ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    എങ്കിൽ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	തിരികെ OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr),
							 context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
arg_cursor1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    എങ്കിൽ (type->tt_type == VAR_സംഖ്യ
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	തിരികെ OK;

    arg_type_mismatch(&t_സംഖ്യ, type, context->arg_idx + 1);
    തിരികെ FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
സ്ഥിതം argcheck_T arg1_blob[] = {arg_blob};
സ്ഥിതം argcheck_T arg1_bool[] = {arg_bool};
സ്ഥിതം argcheck_T arg1_buffer[] = {arg_buffer};
സ്ഥിതം argcheck_T arg1_buffer_or_dict_any[] = {arg_buffer_or_dict_any};
സ്ഥിതം argcheck_T arg1_chan_or_job[] = {arg_chan_or_job};
സ്ഥിതം argcheck_T arg1_dict_any[] = {arg_dict_any};
സ്ഥിതം argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
സ്ഥിതം argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
സ്ഥിതം argcheck_T arg1_job[] = {arg_job};
സ്ഥിതം argcheck_T arg1_list_any[] = {arg_list_any};
സ്ഥിതം argcheck_T arg1_tuple_any[] = {arg_tuple_any};
സ്ഥിതം argcheck_T arg1_list_സംഖ്യ[] = {arg_list_സംഖ്യ};
സ്ഥിതം argcheck_T arg1_reverse[] = {arg_reverse};
സ്ഥിതം argcheck_T arg1_list_or_tuple_or_dict[] = {arg_list_or_tuple_or_dict};
സ്ഥിതം argcheck_T arg1_list_string[] = {arg_list_string};
സ്ഥിതം argcheck_T arg1_list_tuple_dict_blob_or_string[] = {arg_list_tuple_dict_blob_or_string};
സ്ഥിതം argcheck_T arg1_lnum[] = {arg_lnum};
സ്ഥിതം argcheck_T arg1_സംഖ്യ[] = {arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg1_string[] = {arg_string};
സ്ഥിതം argcheck_T arg1_string_or_list_any[] = {arg_string_or_list_any};
സ്ഥിതം argcheck_T arg1_string_or_list_string[] = {arg_string_or_list_string};
സ്ഥിതം argcheck_T arg1_string_or_nr[] = {arg_string_or_nr};
സ്ഥിതം argcheck_T arg1_string_or_blob[] = {arg_string_or_blob};
സ്ഥിതം argcheck_T arg2_buffer_any[] = {arg_buffer, arg_any};
സ്ഥിതം argcheck_T arg2_buffer_bool[] = {arg_buffer, arg_bool};
സ്ഥിതം argcheck_T arg2_buffer_list_any[] = {arg_buffer, arg_list_any};
സ്ഥിതം argcheck_T arg2_buffer_lnum[] = {arg_buffer, arg_lnum};
സ്ഥിതം argcheck_T arg2_buffer_സംഖ്യ[] = {arg_buffer, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg2_buffer_string[] = {arg_buffer, arg_string};
സ്ഥിതം argcheck_T arg2_chan_or_job_dict[] = {arg_chan_or_job, arg_dict_any};
സ്ഥിതം argcheck_T arg2_chan_or_job_string[] = {arg_chan_or_job, arg_string};
സ്ഥിതം argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
സ്ഥിതം argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
സ്ഥിതം argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
സ്ഥിതം argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
സ്ഥിതം argcheck_T arg2_job_dict[] = {arg_job, arg_dict_any};
സ്ഥിതം argcheck_T arg2_job_string_or_സംഖ്യ[] = {arg_job, arg_string_or_nr};
സ്ഥിതം argcheck_T arg2_list_any_സംഖ്യ[] = {arg_list_any, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg2_list_സംഖ്യ[] = {arg_list_സംഖ്യ, arg_list_സംഖ്യ};
സ്ഥിതം argcheck_T arg2_list_സംഖ്യ_bool[] = {arg_list_സംഖ്യ, arg_bool};
സ്ഥിതം argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
സ്ഥിതം argcheck_T arg2_listblobmod_item[] = {arg_list_or_blob_mod, arg_item_of_prev};
സ്ഥിതം argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
സ്ഥിതം argcheck_T arg2_lnum_സംഖ്യ[] = {arg_lnum, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg2_സംഖ്യ[] = {arg_സംഖ്യ, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg2_സംഖ്യ_any[] = {arg_സംഖ്യ, arg_any};
സ്ഥിതം argcheck_T arg2_സംഖ്യ_bool[] = {arg_സംഖ്യ, arg_bool};
സ്ഥിതം argcheck_T arg2_സംഖ്യ_dict_any[] = {arg_സംഖ്യ, arg_dict_any};
സ്ഥിതം argcheck_T arg2_സംഖ്യ_list[] = {arg_സംഖ്യ, arg_list_any};
സ്ഥിതം argcheck_T arg2_സംഖ്യ_string[] = {arg_സംഖ്യ, arg_string};
സ്ഥിതം argcheck_T arg2_സംഖ്യ_buffer[] = {arg_സംഖ്യ, arg_buffer};
സ്ഥിതം argcheck_T arg2_സംഖ്യ_string_or_list[] = {arg_സംഖ്യ, arg_string_or_list_any};
സ്ഥിതം argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
സ്ഥിതം argcheck_T arg2_string[] = {arg_string, arg_string};
സ്ഥിതം argcheck_T arg2_string_any[] = {arg_string, arg_any};
സ്ഥിതം argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
സ്ഥിതം argcheck_T arg2_string_chan_or_job[] = {arg_string, arg_chan_or_job};
സ്ഥിതം argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
സ്ഥിതം argcheck_T arg2_string_list_സംഖ്യ[] = {arg_string, arg_list_സംഖ്യ};
സ്ഥിതം argcheck_T arg2_string_സംഖ്യ[] = {arg_string, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
സ്ഥിതം argcheck_T arg2_string_or_list_സംഖ്യ[] = {arg_string_or_list_any, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg2_string_string_or_സംഖ്യ[] = {arg_string, arg_string_or_nr};
സ്ഥിതം argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
സ്ഥിതം argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
സ്ഥിതം argcheck_T arg3_any_buffer_bool_or_dict[] = {
			      arg_any, arg_buffer, arg_bool_or_dict_any};
സ്ഥിതം argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
സ്ഥിതം argcheck_T arg3_buffer_lnum_lnum[] = {arg_buffer, arg_lnum, arg_lnum};
സ്ഥിതം argcheck_T arg3_buffer_സംഖ്യ_സംഖ്യ[] = {arg_buffer, arg_സംഖ്യ, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg3_buffer_string_any[] = {arg_buffer, arg_string, arg_any};
സ്ഥിതം argcheck_T arg3_buffer_string_dict[] = {arg_buffer, arg_string, arg_dict_any};
സ്ഥിതം argcheck_T arg3_dict_സംഖ്യ_സംഖ്യ[] = {arg_dict_any, arg_സംഖ്യ, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
സ്ഥിതം argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
സ്ഥിതം argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
സ്ഥിതം argcheck_T arg3_lnum_സംഖ്യ_bool[] = {arg_lnum, arg_സംഖ്യ, arg_bool};
സ്ഥിതം argcheck_T arg3_സംഖ്യ[] = {arg_സംഖ്യ, arg_സംഖ്യ, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg3_സംഖ്യ_any_dict[] = {arg_സംഖ്യ, arg_any, arg_dict_any};
സ്ഥിതം argcheck_T arg3_സംഖ്യ_സംഖ്യ_dict[] = {arg_സംഖ്യ, arg_സംഖ്യ, arg_dict_any};
സ്ഥിതം argcheck_T arg3_സംഖ്യ_string_any[] = {arg_സംഖ്യ, arg_string, arg_any};
സ്ഥിതം argcheck_T arg3_സംഖ്യ_string_buffer[] = {arg_സംഖ്യ, arg_string, arg_buffer};
സ്ഥിതം argcheck_T arg3_സംഖ്യ_string_string[] = {arg_സംഖ്യ, arg_string, arg_string};
സ്ഥിതം argcheck_T arg3_string[] = {arg_string, arg_string, arg_string};
സ്ഥിതം argcheck_T arg3_string_any_dict[] = {arg_string, arg_any, arg_dict_any};
സ്ഥിതം argcheck_T arg3_string_any_string[] = {arg_string, arg_any, arg_string};
സ്ഥിതം argcheck_T arg3_string_bool_bool[] = {arg_string, arg_bool, arg_bool};
സ്ഥിതം argcheck_T arg3_string_സംഖ്യ_bool[] = {arg_string, arg_സംഖ്യ, arg_bool};
സ്ഥിതം argcheck_T arg3_string_സംഖ്യ_സംഖ്യ[] = {arg_string, arg_സംഖ്യ, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg3_string_or_dict_bool_dict[] = {arg_string_or_dict_any, arg_bool, arg_dict_any};
സ്ഥിതം argcheck_T arg3_string_or_list_bool_സംഖ്യ[] = {arg_string_or_list_any, arg_bool, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg3_string_string_bool[] = {arg_string, arg_string, arg_bool};
സ്ഥിതം argcheck_T arg3_string_string_dict[] = {arg_string, arg_string, arg_dict_any};
സ്ഥിതം argcheck_T arg3_string_string_സംഖ്യ[] = {arg_string, arg_string, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg4_സംഖ്യ_സംഖ്യ_string_any[] = {arg_സംഖ്യ, arg_സംഖ്യ, arg_string, arg_any};
സ്ഥിതം argcheck_T arg4_string_string_any_string[] = {arg_string, arg_string, arg_any, arg_string};
സ്ഥിതം argcheck_T arg4_string_string_സംഖ്യ_string[] = {arg_string, arg_string, arg_സംഖ്യ, arg_string};
സ്ഥിതം argcheck_T arg4_string_സംഖ്യ_bool_bool[] = {arg_string, arg_സംഖ്യ, arg_bool, arg_bool};
/* Function specific argument types (not covered by the above) */
സ്ഥിതം argcheck_T arg15_assert_fails[] = {arg_string_or_nr, arg_string_or_list_any, arg_any, arg_സംഖ്യ, arg_string};
സ്ഥിതം argcheck_T arg34_assert_inrange[] = {arg_float_or_nr, arg_float_or_nr, arg_float_or_nr, arg_string};
സ്ഥിതം argcheck_T arg4_browse[] = {arg_bool, arg_string, arg_string, arg_string};
സ്ഥിതം argcheck_T arg23_chanexpr[] = {arg_chan_or_job, arg_any, arg_dict_any};
സ്ഥിതം argcheck_T arg23_chanraw[] = {arg_chan_or_job, arg_string_or_blob, arg_dict_any};
സ്ഥിതം argcheck_T arg24_എണ്ണം[] = {arg_string_list_tuple_or_dict, arg_any, arg_bool, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg13_cursor[] = {arg_cursor1, arg_സംഖ്യ, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg12_deepcopy[] = {arg_any, arg_bool};
സ്ഥിതം argcheck_T arg12_execute[] = {arg_string_or_list_string, arg_string};
സ്ഥിതം argcheck_T arg12_getchar[] = {arg_bool_or_nr, arg_dict_any};
സ്ഥിതം argcheck_T arg23_extend[] = {arg_list_or_dict_or_blob_mod, arg_same_as_prev, arg_extend3};
സ്ഥിതം argcheck_T arg23_extendnew[] = {arg_list_or_dict_or_blob, arg_same_struct_as_prev, arg_extend3};
സ്ഥിതം argcheck_T arg23_get[] = {arg_get1, arg_string_or_nr, arg_any};
സ്ഥിതം argcheck_T arg14_glob[] = {arg_string, arg_bool, arg_bool, arg_bool};
സ്ഥിതം argcheck_T arg25_globpath[] = {arg_string, arg_string, arg_bool, arg_bool, arg_bool};
സ്ഥിതം argcheck_T arg24_index[] = {arg_list_or_tuple_or_blob, arg_item_of_prev, arg_സംഖ്യ, arg_bool};
സ്ഥിതം argcheck_T arg23_index[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
സ്ഥിതം argcheck_T arg23_തിരുകുക[] = {arg_list_or_blob, arg_item_of_prev, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg1_len[] = {arg_len1};
സ്ഥിതം argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
സ്ഥിതം argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
സ്ഥിതം argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
സ്ഥിതം argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
സ്ഥിതം argcheck_T arg2_instanceof[] = {arg_object, varargs_class, ശൂന്യ };
സ്ഥിതം argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
സ്ഥിതം argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
സ്ഥിതം argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_സംഖ്യ, arg_സംഖ്യ, arg_dict_any};
സ്ഥിതം argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_സംഖ്യ, arg_സംഖ്യ, arg_dict_any};
സ്ഥിതം argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
സ്ഥിതം argcheck_T arg45_matchbufline[] = {arg_buffer, arg_string, arg_lnum, arg_lnum, arg_dict_any};
സ്ഥിതം argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
സ്ഥിതം argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
സ്ഥിതം argcheck_T arg24_remote_വ്യഞ്ജകം[] = {arg_string, arg_string, arg_string, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg2_repeat[] = {arg_repeat1, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg15_തിരയൽ[] = {arg_string, arg_string, arg_സംഖ്യ, arg_സംഖ്യ, arg_string_or_func};
സ്ഥിതം argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_സംഖ്യ, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg3_setbufline[] = {arg_buffer, arg_lnum, arg_str_or_nr_or_list};
സ്ഥിതം argcheck_T arg2_setline[] = {arg_lnum, arg_any};
സ്ഥിതം argcheck_T arg24_setloclist[] = {arg_സംഖ്യ, arg_list_any, arg_string, arg_dict_any};
സ്ഥിതം argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
സ്ഥിതം argcheck_T arg23_settagstack[] = {arg_സംഖ്യ, arg_dict_any, arg_string};
സ്ഥിതം argcheck_T arg02_sign_getplaced[] = {arg_buffer, arg_dict_any};
സ്ഥിതം argcheck_T arg45_sign_place[] = {arg_സംഖ്യ, arg_string, arg_string, arg_buffer, arg_dict_any};
സ്ഥിതം argcheck_T arg23_slice[] = {arg_slice1, arg_സംഖ്യ, arg_സംഖ്യ};
സ്ഥിതം argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_sort_how, arg_dict_any};
സ്ഥിതം argcheck_T arg24_strpart[] = {arg_string, arg_സംഖ്യ, arg_സംഖ്യ, arg_bool};
സ്ഥിതം argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
സ്ഥിതം argcheck_T arg23_win_execute[] = {arg_സംഖ്യ, arg_string_or_list_string, arg_string};
സ്ഥിതം argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
സ്ഥിതം argcheck_T arg24_match_func[] = {arg_string_or_list_any, arg_string, arg_സംഖ്യ, arg_സംഖ്യ};

// Can be used by functions called through "f_retfunc" to create new types.
സ്ഥിതം garray_T *current_type_gap = ശൂന്യ;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    സ്ഥിതം type_T *
ret_void(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_void;
}
    സ്ഥിതം type_T *
ret_any(പൂർണ്ണസംഖ്യ	argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_any;
}
    സ്ഥിതം type_T *
ret_bool(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_bool;
}
    സ്ഥിതം type_T *
ret_സംഖ്യ_bool(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_സംഖ്യ_bool;
}
    സ്ഥിതം type_T *
ret_സംഖ്യ(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_സംഖ്യ;
}
    സ്ഥിതം type_T *
ret_float(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_float;
}
    സ്ഥിതം type_T *
ret_string(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_string;
}
    സ്ഥിതം type_T *
ret_list_any(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_list_any;
}
    സ്ഥിതം type_T *
ret_list_സംഖ്യ(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    തിരികെ &t_list_സംഖ്യ;
}
    സ്ഥിതം type_T *
ret_list_string(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    തിരികെ &t_list_string;
}
    സ്ഥിതം type_T *
ret_list_dict_any(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    തിരികെ &t_list_dict_any;
}
    സ്ഥിതം type_T *
ret_list_ഇനങ്ങൾ(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    തിരികെ &t_list_list_any;
}
    സ്ഥിതം type_T *
ret_list_list_any(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    തിരികെ &t_list_list_any;
}
    സ്ഥിതം type_T *
ret_list_string_ഇനങ്ങൾ(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    തിരികെ &t_list_list_string;
}
    സ്ഥിതം type_T *
ret_list_regionpos(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    തിരികെ &t_list_list_list_സംഖ്യ;
}
    സ്ഥിതം type_T *
ret_tuple_any(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_tuple_any;
}
    സ്ഥിതം type_T *
ret_dict_any(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_dict_any;
}
    സ്ഥിതം type_T *
ret_job_info(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    എങ്കിൽ (argcount == 0)
    {
	*decl_type = &t_list_any;
	തിരികെ &t_list_job;
    }
    തിരികെ &t_dict_any;
}
    സ്ഥിതം type_T *
ret_dict_സംഖ്യ(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_dict_സംഖ്യ;
}
    സ്ഥിതം type_T *
ret_dict_string(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_dict_string;
}
    സ്ഥിതം type_T *
ret_blob(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_blob;
}
    സ്ഥിതം type_T *
ret_func_any(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_func_any;
}
    സ്ഥിതം type_T *
ret_func_unknown(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_func_unknown;
}
    സ്ഥിതം type_T *
ret_channel(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_channel;
}
    സ്ഥിതം type_T *
ret_job(പൂർണ്ണസംഖ്യ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    തിരികെ &t_job;
}
    സ്ഥിതം type_T *
ret_first_arg(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    എങ്കിൽ (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	തിരികെ argtypes[0].type_curr;
    }
    തിരികെ &t_void;
}
    സ്ഥിതം type_T *
ret_slice(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    എങ്കിൽ (argcount > 0)
    {
	എങ്കിൽ (argtypes[0].type_decl != ശൂന്യ)
	{
	    തിരഞ്ഞെടുപ്പ് (argtypes[0].type_decl->tt_type)
	    {
		അവസ്ഥ VAR_STRING: *decl_type = &t_string; നിർത്തുക;
		അവസ്ഥ VAR_BLOB: *decl_type = &t_blob; നിർത്തുക;
		അവസ്ഥ VAR_LIST: *decl_type = &t_list_any; നിർത്തുക;
		അവസ്ഥ VAR_TUPLE: *decl_type = &t_tuple_any; നിർത്തുക;
		സ്ഥിരസ്ഥിതി: നിർത്തുക;
	    }
	}
	തിരികെ argtypes[0].type_curr;
    }
    തിരികെ &t_void;
}
    സ്ഥിതം type_T *
ret_copy(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    എങ്കിൽ (argcount > 0)
    {
	എങ്കിൽ (argtypes[0].type_decl != ശൂന്യ)
	{
	    എങ്കിൽ (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    അല്ലെങ്കിൽ എങ്കിൽ (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    അല്ലെങ്കിൽ
		*decl_type = argtypes[0].type_decl;
	}
	എങ്കിൽ (argtypes[0].type_curr != ശൂന്യ)
	{
	    എങ്കിൽ (argtypes[0].type_curr->tt_type == VAR_LIST)
		തിരികെ &t_list_any;
	    അല്ലെങ്കിൽ എങ്കിൽ (argtypes[0].type_curr->tt_type == VAR_DICT)
		തിരികെ &t_dict_any;
	}
	തിരികെ argtypes[0].type_curr;
    }
    തിരികെ &t_void;
}
    സ്ഥിതം type_T *
ret_extend(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    എങ്കിൽ (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	// if the second argument has a different current type then the current
	// type is "any"
	എങ്കിൽ (argcount > 1 && !equal_type(argtypes[0].type_curr,
						     argtypes[1].type_curr, 0))
	{
	    എങ്കിൽ (argtypes[0].type_curr->tt_type == VAR_LIST)
		തിരികെ &t_list_any;
	    എങ്കിൽ (argtypes[0].type_curr->tt_type == VAR_DICT)
		തിരികെ &t_dict_any;
	    എങ്കിൽ (argtypes[0].type_curr->tt_type == VAR_BLOB)
		തിരികെ &t_blob;
	}
	തിരികെ argtypes[0].type_curr;
    }
    തിരികെ &t_void;
}
    സ്ഥിതം type_T *
ret_repeat(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    എങ്കിൽ (argcount == 0)
	തിരികെ &t_any;
    എങ്കിൽ (argtypes[0].type_curr == &t_സംഖ്യ)
	തിരികെ &t_string;
    തിരികെ argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    സ്ഥിതം type_T *
ret_first_cont(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    എങ്കിൽ (argcount > 0)
    {
	എങ്കിൽ (argtypes[0].type_curr->tt_type == VAR_LIST)
	    തിരികെ &t_list_any;
	എങ്കിൽ (argtypes[0].type_curr->tt_type == VAR_DICT)
	    തിരികെ &t_dict_any;
	എങ്കിൽ (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    തിരികെ argtypes[0].type_curr;
    }
    തിരികെ &t_any;
}
// for get(): the type of the item when a missing item has the same type
    സ്ഥിതം type_T *
ret_get(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    എങ്കിൽ (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*item = ശൂന്യ;

	എങ്കിൽ (t->tt_type == VAR_BLOB)
	    item = &t_സംഖ്യ;
	അല്ലെങ്കിൽ എങ്കിൽ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    item = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	എങ്കിൽ (item != ശൂന്യ && (argcount == 3
			? equal_type(item, argtypes[2].type_curr, 0)
			: item->tt_type == VAR_സംഖ്യ))
	{
	    എങ്കിൽ (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    തിരികെ item;
	}
    }
    തിരികെ &t_any;
}
// for getline()
    സ്ഥിതം type_T *
ret_getline(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    എങ്കിൽ (argcount == 1)
	തിരികെ &t_string;
    *decl_type = &t_list_any;
    തിരികെ &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    സ്ഥിതം type_T *
ret_string_or_any_1(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    എങ്കിൽ (argcount < 1)
	തിരികെ &t_string;
    തിരികെ &t_any;
}
    സ്ഥിതം type_T *
ret_string_or_any_2(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    എങ്കിൽ (argcount < 2)
	തിരികെ &t_string;
    തിരികെ &t_any;
}
    സ്ഥിതം type_T *
ret_string_or_any_3(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    എങ്കിൽ (argcount < 3)
	തിരികെ &t_string;
    തിരികെ &t_any;
}
    സ്ഥിതം type_T *
ret_string_or_any_4(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    എങ്കിൽ (argcount < 4)
	തിരികെ &t_string;
    തിരികെ &t_any;
}
// for abs()
    സ്ഥിതം type_T *
ret_abs(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    എങ്കിൽ (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	തിരികെ &t_float;
    തിരികെ &t_സംഖ്യ;
}
// for sign_define()
    സ്ഥിതം type_T *
ret_sign_define(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    എങ്കിൽ (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	തിരികെ &t_list_സംഖ്യ;
    തിരികെ &t_സംഖ്യ;
}
// for values(): list of member of first argument
    സ്ഥിതം type_T *
ret_list_member(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    എങ്കിൽ (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	എങ്കിൽ (current_type_gap != ശൂന്യ
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	അല്ലെങ്കിൽ
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	എങ്കിൽ (current_type_gap != ശൂന്യ
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    തിരികെ get_list_type(t->tt_member, current_type_gap);
    }
    തിരികെ &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    സ്ഥിതം type_T *
ret_list_or_dict_0(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    എങ്കിൽ (argcount > 0)
	തിരികെ &t_dict_any;
    *decl_type = &t_list_any;
    തിരികെ &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    സ്ഥിതം type_T *
ret_list_or_dict_1(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    എങ്കിൽ (argcount > 1)
	തിരികെ &t_dict_any;
    *decl_type = &t_list_any;
    തിരികെ &t_list_dict_any;
}

    സ്ഥിതം type_T *
ret_argv(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // argv() returns list of strings
    എങ്കിൽ (argcount == 0)
    {
	*decl_type = &t_list_any;
	തിരികെ &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    തിരികെ &t_any;
}

    സ്ഥിതം type_T *
ret_remove(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    എങ്കിൽ (argcount > 0)
    {
	എങ്കിൽ (argtypes[0].type_curr->tt_type == VAR_LIST
		|| argtypes[0].type_curr->tt_type == VAR_DICT)
	{
	    എങ്കിൽ (argcount == 3)
	    {
		*decl_type = argtypes[0].type_decl;
		തിരികെ argtypes[0].type_curr;
	    }
	    എങ്കിൽ (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    തിരികെ argtypes[0].type_curr->tt_member;
	}
	എങ്കിൽ (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    തിരികെ &t_സംഖ്യ;
    }
    തിരികെ &t_any;
}

    സ്ഥിതം type_T *
ret_max_min(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    എങ്കിൽ (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	എങ്കിൽ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	അല്ലെങ്കിൽ
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	എങ്കിൽ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	അല്ലെങ്കിൽ
	    t = &t_any;
	തിരികെ t;
    }
    തിരികെ &t_any;
}

    സ്ഥിതം type_T *
ret_getreg(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    എങ്കിൽ (argcount == 3)
    {
	*decl_type = &t_list_any;
	തിരികെ &t_list_string;
    }
    തിരികെ &t_string;
}

    സ്ഥിതം type_T *
ret_virtcol(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    എങ്കിൽ (argcount > 1)
    {
	*decl_type = &t_list_any;
	തിരികെ &t_list_സംഖ്യ;
    }
    തിരികെ &t_സംഖ്യ;
}

    സ്ഥിതം type_T *
ret_maparg(പൂർണ്ണസംഖ്യ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    // Assume that if the fourth argument is passed it's non-zero
    എങ്കിൽ (argcount == 4)
	തിരികെ &t_dict_any;
    തിരികെ &t_string;
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
തരനിർവചനം ഘടന
{
    അക്ഷരം	*f_name;	// function name
    അക്ഷരം	f_min_argc;	// minimal number of arguments
    അക്ഷരം	f_max_argc;	// maximal number of arguments
    അക്ഷരം	f_argtype;	// for method: FEARG_ values; bits FE_
    argcheck_T	*f_argcheck;	// list of functions to check argument types;
				// use "arg_any" (not NULL) to accept an
				// argument of any type
    അക്ഷരം	*f_argnames;	// names of the arguments as in the help,
				// separated by ", "; an argument without a
				// name is empty, NULL when there is none
    type_T	*(*f_retfunc)(പൂർണ്ണസംഖ്യ argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    ശൂന്യം	(*f_func)(typval_T *args, typval_T *rvar);
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

#എങ്കിൽ defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#അല്ലെങ്കിൽ
# define MATH_FUNC(name) ശൂന്യ
#endif
#ifdef FEAT_TIMERS
# define TIMER_FUNC(name) name
#അല്ലെങ്കിൽ
# define TIMER_FUNC(name) ശൂന്യ
#endif
#ifdef FEAT_JOB_CHANNEL
# define JOB_FUNC(name) name
#അല്ലെങ്കിൽ
# define JOB_FUNC(name) ശൂന്യ
#endif
#ifdef FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#അല്ലെങ്കിൽ
# define PROP_FUNC(name) ശൂന്യ
#endif
#ifdef FEAT_SIGNS
# define SIGN_FUNC(name) name
#അല്ലെങ്കിൽ
# define SIGN_FUNC(name) ശൂന്യ
#endif
#ifdef FEAT_SOUND
# define SOUND_FUNC(name) name
#അല്ലെങ്കിൽ
# define SOUND_FUNC(name) ശൂന്യ
#endif
#ifdef FEAT_TERMINAL
# define TERM_FUNC(name) name
#അല്ലെങ്കിൽ
# define TERM_FUNC(name) ശൂന്യ
#endif
#ifdef FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#അല്ലെങ്കിൽ
# define TABPANEL_FUNC(name) ശൂന്യ
#endif
#ifdef FEAT_IMAGE
# define IMAGE_FUNC(name) name
#അല്ലെങ്കിൽ
# define IMAGE_FUNC(name) ശൂന്യ
#endif

സ്ഥിതം സ്ഥിരം funcentry_T global_functions[] =
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
    {"and",		2, 2, FEARG_1,	    arg2_സംഖ്യ,
			"expr1, expr2",
			ret_സംഖ്യ,	    f_and},
    {"append",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_സംഖ്യ_bool,    f_append},
    {"appendbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_സംഖ്യ_bool,    f_appendbufline},
    {"argc",		0, 1, 0,	    arg1_സംഖ്യ,
			"winid",
			ret_സംഖ്യ,	    f_argc},
    {"argidx",		0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_argidx},
    {"arglistid",	0, 2, 0,	    arg2_സംഖ്യ,
			"winnr, tabnr",
			ret_സംഖ്യ,	    f_arglistid},
    {"argv",		0, 2, 0,	    arg2_സംഖ്യ,
			"nr, winid",
			ret_argv,	    f_argv},
    {"asin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_asin},
    {"assert_beeps",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_സംഖ്യ_bool,    f_assert_beeps},
    {"assert_equal",	2, 3, FEARG_2,	    ശൂന്യ,
			"expected, actual, msg",
			ret_സംഖ്യ_bool,    f_assert_equal},
    {"assert_equalfile", 2, 3, FEARG_1,	    arg3_string,
			"fname_one, fname_two, msg",
			ret_സംഖ്യ_bool,    f_assert_equalfile},
    {"assert_exception", 1, 2, 0,	    arg2_string,
			"error, msg",
			ret_സംഖ്യ_bool,    f_assert_exception},
    {"assert_fails",	1, 5, FEARG_1,	    arg15_assert_fails,
			"cmd, error, msg, lnum, context",
			ret_സംഖ്യ_bool,    f_assert_fails},
    {"assert_false",	1, 2, FEARG_1,	    ശൂന്യ,
			"actual, msg",
			ret_സംഖ്യ_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_സംഖ്യ_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_സംഖ്യ_bool,    f_assert_match},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_സംഖ്യ_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    ശൂന്യ,
			"expected, actual, msg",
			ret_സംഖ്യ_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_സംഖ്യ_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_സംഖ്യ_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    ശൂന്യ,
			"actual, msg",
			ret_സംഖ്യ_bool,    f_assert_true},
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
    {"balloon_gettext",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,
#ifdef FEAT_BEVAL
	    f_balloon_gettext
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#ifdef FEAT_BEVAL
	    f_balloon_show
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#എങ്കിൽ defined(FEAT_BEVAL_TERM)
	    f_balloon_split
#അല്ലെങ്കിൽ
	    ശൂന്യ
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
			ret_list_സംഖ്യ,    f_blob2list},
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
			ret_സംഖ്യ,	    f_bufadd},
    {"bufexists",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_സംഖ്യ_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_സംഖ്യ_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_സംഖ്യ,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_സംഖ്യ_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_സംഖ്യ_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, create",
			ret_സംഖ്യ,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_സംഖ്യ,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_സംഖ്യ,	    f_bufwinnr},
    {"byte2line",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"byte",
			ret_സംഖ്യ,	    f_byte2line},
    {"byteidx",		2, 3, FEARG_1,	    arg3_string_സംഖ്യ_bool,
			"expr, nr, utf16",
			ret_സംഖ്യ,	    f_byteidx},
    {"byteidxcomp",	2, 3, FEARG_1,	    arg3_string_സംഖ്യ_bool,
			"expr, nr, utf16",
			ret_സംഖ്യ,	    f_byteidxcomp},
    {"call",		2, 3, FEARG_1,	    arg3_any_list_dict,
			"func, arglist, dict",
			ret_any,	    f_call},
    {"ceil",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ceil},
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_job,
			"handle",
			ret_സംഖ്യ_bool,    JOB_FUNC(f_ch_canread)},
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
			ret_സംഖ്യ,	    JOB_FUNC(f_ch_getbufnr)},
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
    {"changenr",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_changenr},
    {"char2nr",		1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_സംഖ്യ,	    f_char2nr},
    {"charclass",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_സംഖ്യ,	    f_charclass},
    {"charcol",		1, 2, FEARG_1,	    arg2_string_or_list_സംഖ്യ,
			"expr, winid",
			ret_സംഖ്യ,	    f_charcol},
    {"charidx",		2, 4, FEARG_1,	    arg4_string_സംഖ്യ_bool_bool,
			"string, idx, countcc, utf16",
			ret_സംഖ്യ,	    f_charidx},
    {"chdir",		1, 2, FEARG_1,	    arg2_string,
			"dir, scope",
			ret_string,	    f_chdir},
    {"cindent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_സംഖ്യ,	    f_cindent},
    {"clearmatches",	0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"win",
			ret_void,	    f_clearmatches},
    {"cmdcomplete_info",0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_സംഖ്യ,
			"expr, winid",
			ret_സംഖ്യ,	    f_col},
    {"complete",	2, 2, FEARG_2,	    arg2_സംഖ്യ_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_സംഖ്യ,	    f_complete_add},
    {"complete_check",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_സംഖ്യ_string,
			"msg, choices, default, type",
			ret_സംഖ്യ,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    ശൂന്യ,
			"expr",
			ret_copy,	    f_copy},
    {"cos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cos},
    {"cosh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cosh},
    {"count",		2, 4, FEARG_1,	    arg24_എണ്ണം,
			"comp, expr, ic, start",
			ret_സംഖ്യ,	    f_എണ്ണം},
    {"cscope_connection",0,3, 0,	    arg3_സംഖ്യ_string_string,
			"num, dbpath, prepend",
			ret_സംഖ്യ,	    f_cscope_connection},
    {"cursor",		1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_സംഖ്യ,	    f_cursor},
    {"debugbreak",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"pid",
			ret_സംഖ്യ,
#ifdef MSWIN
	    f_debugbreak
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_സംഖ്യ_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_buffer_lnum_lnum,
			"buf, first, last",
			ret_സംഖ്യ_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ_bool,    f_did_filetype},
    {"diff",		2, 3, FEARG_1,	    arg3_diff,
			"fromlist, tolist, options",
			ret_string_or_any_3,  f_diff},
    {"diff_filler",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_സംഖ്യ,	    f_diff_filler},
    {"diff_hlID",	2, 2, FEARG_1,	    arg2_lnum_സംഖ്യ,
			"lnum, col",
			ret_സംഖ്യ,	    f_diff_hlID},
    {"digraph_get",	1, 1, FEARG_1,	    arg1_string,
			"chars",
			ret_string,	    f_digraph_get},
    {"digraph_getlist",	0, 1, FEARG_1,	    arg1_bool,
			"listall",
			ret_list_string_ഇനങ്ങൾ, f_digraph_getlist},
    {"digraph_set",	2, 2, FEARG_1,	    arg2_string,
			"chars, digraph",
			ret_bool,	f_digraph_set},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    ശൂന്യ,
			"expr",
			ret_സംഖ്യ_bool,    f_empty},
    {"environ",		0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    ശൂന്യ,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ_bool,    f_eventhandler},
    {"executable",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_സംഖ്യ,	    f_executable},
    {"execute",		1, 2, FEARG_1,	    arg12_execute,
			"command, silent",
			ret_string,	    f_execute},
    {"exepath",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_exepath},
    {"exists",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_സംഖ്യ_bool,    f_exists},
    {"exists_compiled",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_സംഖ്യ_bool,    f_exists_compiled},
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
			ret_സംഖ്യ_bool,    f_filereadable},
    {"filecopy",	2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_സംഖ്യ_bool,    f_filecopy},
    {"filereadable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_സംഖ്യ_bool,    f_filereadable},
    {"filewritable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_സംഖ്യ,	    f_filewritable},
    {"filter",		2, 2, FEARG_1,	    arg2_filter,
			"expr1, expr2",
			ret_first_arg,	    f_filter},
    {"finddir",		1, 3, FEARG_1,	    arg3_string_string_സംഖ്യ,
			"name, path, count",
			ret_string_or_any_3,	    f_finddir},
    {"findfile",	1, 3, FEARG_1,	    arg3_string_string_സംഖ്യ,
			"name, path, count",
			ret_string_or_any_3,	    f_findfile},
    {"flatten",		1, 2, FEARG_1,	    arg2_list_any_സംഖ്യ,
			"list, maxdepth",
			ret_list_any,	    f_flatten},
    {"flattennew",	1, 2, FEARG_1,	    arg2_list_any_സംഖ്യ,
			"list, maxdepth",
			ret_list_any,	    f_flattennew},
    {"float2nr",	1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_സംഖ്യ,	    f_float2nr},
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
    {"foldclosed",	1, 2, FEARG_1,	    arg2_lnum_സംഖ്യ,
			"lnum, winid",
			ret_സംഖ്യ,	    f_foldclosed},
    {"foldclosedend",	1, 2, FEARG_1,	    arg2_lnum_സംഖ്യ,
			"lnum, winid",
			ret_സംഖ്യ,	    f_foldclosedend},
    {"foldlevel",	1, 2, FEARG_1,	    arg2_lnum_സംഖ്യ,
			"lnum, winid",
			ret_സംഖ്യ,	    f_foldlevel},
    {"foldtext",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_സംഖ്യ,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_first_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
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
    {"getbgcolor",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
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
    {"getcellpixels",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_സംഖ്യ,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_getcmdwintype},
    {"getcompletion",	2, 3, FEARG_1,	    arg3_string_string_bool,
			"pat, type, filtered",
			ret_list_string,    f_getcompletion},
    {"getcompletiontype", 1, 1, FEARG_1,    arg1_string,
			"pat",
			ret_string,	    f_getcompletiontype},
    {"getcurpos",	0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"winid",
			ret_list_സംഖ്യ,    f_getcurpos},
    {"getcursorcharpos", 0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"winid",
			ret_list_സംഖ്യ,    f_getcursorcharpos},
    {"getcwd",		0, 2, FEARG_1,	    arg2_സംഖ്യ,
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
			ret_സംഖ്യ,	    f_getfsize},
    {"getftime",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_സംഖ്യ,	    f_getftime},
    {"getftype",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getftype},
    {"getimstatus",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ_bool,    f_getimstatus},
    {"getinfo",		2, 3, FEARG_2,	    arg3_string_string_dict,
			"kind, name, opts",
			ret_dict_any,	    f_getinfo},
    {"getjumplist",	0, 2, FEARG_1,	    arg2_സംഖ്യ,
			"winnr, tabnr",
			ret_list_any,	    f_getjumplist},
    {"getline",		1, 2, FEARG_1,	    arg2_lnum,
			"lnum, end",
			ret_getline,	    f_getline},
    {"getloclist",	1, 2, 0,	    arg2_സംഖ്യ_dict_any,
			"nr, what",
			ret_list_or_dict_1, f_getloclist},
    {"getmarklist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_സംഖ്യ,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_dict_സംഖ്യ,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_getpid},
    {"getpos",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_സംഖ്യ,    f_getpos},
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
    {"getstacktrace",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_list_dict_any,  f_getstacktrace},
    {"gettabinfo",	0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"tabnr",
			ret_list_dict_any,  f_gettabinfo},
    {"gettabvar",	2, 3, FEARG_1,	    arg3_സംഖ്യ_string_any,
			"tabnr, varname, def",
			ret_any,	    f_gettabvar},
    {"gettabwinvar",	3, 4, FEARG_1,	    arg4_സംഖ്യ_സംഖ്യ_string_any,
			"tabnr, winnr, varname, def",
			ret_any,	    f_gettabwinvar},
    {"gettagstack",	0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"winnr",
			ret_dict_any,	    f_gettagstack},
    {"gettext",		1, 2, FEARG_1,	    arg2_string,
			"text, package",
			ret_string,	    f_gettext},
    {"getwininfo",	0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"winid",
			ret_list_dict_any,  f_getwininfo},
    {"getwinpos",	0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"timeout",
			ret_list_സംഖ്യ,    f_getwinpos},
    {"getwinposx",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_getwinposy},
    {"getwinvar",	2, 3, FEARG_1,	    arg3_സംഖ്യ_string_any,
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
			ret_സംഖ്യ_bool,    f_has},
    {"has_key",		2, 2, FEARG_1,	    arg2_dict_any_string_or_nr,
			"dict, key",
			ret_സംഖ്യ_bool,    f_has_താക്കോൽ},
    {"haslocaldir",	0, 2, FEARG_1,	    arg2_സംഖ്യ,
			"winnr, tabnr",
			ret_സംഖ്യ,	    f_haslocaldir},
    {"hasmapto",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"what, mode, abbr",
			ret_സംഖ്യ_bool,    f_hasmapto},
    {"highlightID",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_സംഖ്യ,	    f_hlID},
    {"highlight_exists",1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_സംഖ്യ_bool,    f_hlexists},
    {"histadd",		2, 2, FEARG_2,	    arg2_string,
			"history, item",
			ret_സംഖ്യ_bool,    f_histadd},
    {"histdel",		1, 2, FEARG_1,	    arg2_string_string_or_സംഖ്യ,
			"history, item",
			ret_സംഖ്യ_bool,    f_histdel},
    {"histget",		1, 2, FEARG_1,	    arg2_string_സംഖ്യ,
			"history, index",
			ret_string,	    f_histget},
    {"histnr",		1, 1, FEARG_1,	    arg1_string,
			"history",
			ret_സംഖ്യ,	    f_histnr},
    {"hlID",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_സംഖ്യ,	    f_hlID},
    {"hlexists",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_സംഖ്യ_bool,    f_hlexists},
    {"hlget",		0, 2, FEARG_1,	    arg2_string_bool,
			"name, resolve",
			ret_list_dict_any,  f_hlget},
    {"hlset",		1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_സംഖ്യ_bool,    f_hlset},
    {"hostname",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    ശൂന്യ,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_സംഖ്യ,	    IMAGE_FUNC(f_image_add)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_സംഖ്യ,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_index,
			"object, expr, start, ic",
			ret_സംഖ്യ,	    f_index},
    {"indexof",		2, 3, FEARG_1,	    arg23_index,
			"object, expr, opts",
			ret_സംഖ്യ,	    f_indexof},
    {"input",		1, 3, FEARG_1,	    arg3_string,
			"prompt, text, completion",
			ret_string,	    f_നിവേശം},
    {"inputdialog",	1, 3, FEARG_1,	    arg3_string,
			"prompt, text, cancelreturn",
			ret_string,	    f_inputdialog},
    {"inputlist",	1, 1, FEARG_1,	    arg1_list_string,
			"textlist",
			ret_സംഖ്യ,	    f_inputlist},
    {"inputrestore",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_തിരുകുക,
			"object, item, idx",
			ret_first_arg,	    f_തിരുകുക},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_void,	    f_interrupt},
    {"invert",		1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"expr",
			ret_സംഖ്യ,	    f_invert},
    {"isabsolutepath",	1, 1, FEARG_1,	    arg1_string,
			"path",
			ret_സംഖ്യ_bool,    f_isabsolutepath},
    {"isdirectory",	1, 1, FEARG_1,	    arg1_string,
			"directory",
			ret_സംഖ്യ_bool,    f_isdirectory},
    {"isinf",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_സംഖ്യ,	    MATH_FUNC(f_isinf)},
    {"islocked",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_സംഖ്യ_bool,    f_islocked},
    {"isnan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_സംഖ്യ_bool,    MATH_FUNC(f_isnan)},
    {"items",		1, 1, FEARG_1,	    arg1_list_tuple_dict_blob_or_string,
			"expr",
			ret_list_ഇനങ്ങൾ,	    f_ഇനങ്ങൾ},
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
    {"job_stop",	1, 2, FEARG_1,	    arg2_job_string_or_സംഖ്യ,
			"job, how",
			ret_സംഖ്യ_bool,    JOB_FUNC(f_job_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    ശൂന്യ,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    ശൂന്യ,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_താക്കോലുകൾ},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    ശൂന്യ,	// obsolete
			ശൂന്യ,
			ret_സംഖ്യ,	    f_last_buffer_nr},
    {"len",		1, 1, FEARG_1,	    arg1_len,
			"expr",
			ret_സംഖ്യ,	    f_len},
    {"libcall",		3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_string,	    f_libcall},
    {"libcallnr",	3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_സംഖ്യ,	    f_libcallnr},
    {"line",		1, 2, FEARG_1,	    arg2_string_സംഖ്യ,
			"expr, winid",
			ret_സംഖ്യ,	    f_വരി},
    {"line2byte",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_സംഖ്യ,	    f_line2byte},
    {"lispindent",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_സംഖ്യ,	    f_lispindent},
    {"list2blob",	1, 1, FEARG_1,	    arg1_list_സംഖ്യ,
			"list",
			ret_blob,	    f_list2blob},
    {"list2str",	1, 2, FEARG_1,	    arg2_list_സംഖ്യ_bool,
			"list, utf8",
			ret_string,	    f_list2str},
    {"list2tuple",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_tuple_any,	    f_list2tuple},
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_buffer_bool_or_dict,
			"callback, buf, options",
			ret_സംഖ്യ,	    f_listener_add},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"id",
			ret_സംഖ്യ_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_localtime},
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
#അല്ലെങ്കിൽ
		ശൂന്യ
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
			ret_സംഖ്യ,	    f_match},
    {"matchadd",	2, 5, FEARG_1,	    arg25_matchadd,
			"group, pattern, priority, id, dict",
			ret_സംഖ്യ,	    f_matchadd},
    {"matchaddpos",	2, 5, FEARG_1,	    arg25_matchaddpos,
			"group, pos, priority, id, dict",
			ret_സംഖ്യ,	    f_matchaddpos},
    {"matcharg",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"nr",
			ret_list_string,    f_matcharg},
    {"matchbufline",	4, 5, FEARG_1,	    arg45_matchbufline,
			"buf, pat, lnum, end, dict",
			ret_list_any,	    f_matchbufline},
    {"matchdelete",	1, 2, FEARG_1,	    arg2_സംഖ്യ,
			"id, win",
			ret_സംഖ്യ_bool,    f_matchdelete},
    {"matchend",	2, 4, FEARG_1,	    arg24_match_func,
			"expr, pat, start, count",
			ret_സംഖ്യ,	    f_matchend},
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
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_സംഖ്യ,
			"name, flags, prot",
			ret_സംഖ്യ_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_mode},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_MZSCHEME
	    f_mzeval
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
			},
    {"nextnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_സംഖ്യ,	    f_nextnonblank},
    {"ngettext",	3, 4, FEARG_3,	    arg4_string_string_സംഖ്യ_string,
			"single, plural, number, domain",
			ret_string,	    f_ngettext},
    {"nr2char",		1, 2, FEARG_1,	    arg2_സംഖ്യ_bool,
			"expr, utf8",
			ret_string,	    f_nr2char},
    {"or",		2, 2, FEARG_1,	    arg2_സംഖ്യ,
			"expr1, expr2",
			ret_സംഖ്യ,	    f_or},
    {"pathshorten",	1, 2, FEARG_1,	    arg2_string_സംഖ്യ,
			"path, len",
			ret_string,	    f_pathshorten},
    {"perleval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_PERL
	    f_perleval
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
			},
    {"popup_atcursor",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_സംഖ്യ,	    PROP_FUNC(f_popup_atcursor)},
    {"popup_beval",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_സംഖ്യ,	    PROP_FUNC(f_popup_beval)},
    {"popup_clear",	0, 1, 0,	    arg1_bool,
			"force",
			ret_void,	    PROP_FUNC(f_popup_clear)},
    {"popup_close",	1, 2, FEARG_1,	    arg2_സംഖ്യ_any,
			"id, result",
			ret_void,	    PROP_FUNC(f_popup_close)},
    {"popup_create",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_സംഖ്യ,	    PROP_FUNC(f_popup_create)},
    {"popup_dialog",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_സംഖ്യ,	    PROP_FUNC(f_popup_dialog)},
    {"popup_filter_menu", 2, 2, 0,	    arg2_സംഖ്യ_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_menu)},
    {"popup_filter_yesno", 2, 2, 0,	    arg2_സംഖ്യ_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_yesno)},
    {"popup_findecho",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    PROP_FUNC(f_popup_findpreview)},
    {"popup_getoptions", 1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getoptions)},
    {"popup_getpos",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getpos)},
    {"popup_hide",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"id",
			ret_void,	    PROP_FUNC(f_popup_hide)},
    {"popup_list",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_list_സംഖ്യ,    PROP_FUNC(f_popup_list)},
    {"popup_locate",	2, 2, 0,	    arg2_സംഖ്യ,
			"row, col",
			ret_സംഖ്യ,	    PROP_FUNC(f_popup_locate)},
    {"popup_menu",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_സംഖ്യ,	    PROP_FUNC(f_popup_menu)},
    {"popup_move",	2, 2, FEARG_1,	    arg2_സംഖ്യ_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_move)},
    {"popup_notification", 2, 2, FEARG_1,   arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_സംഖ്യ,	    PROP_FUNC(f_popup_notification)},
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_സംഖ്യ_buffer,
			"id, buf",
			ret_സംഖ്യ_bool,    PROP_FUNC(f_popup_setbuf)},
    {"popup_setoptions", 2, 2, FEARG_1,	    arg2_സംഖ്യ_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_setoptions)},
    {"popup_settext",	2, 2, FEARG_1,	    arg2_സംഖ്യ_string_or_list,
			"id, text",
			ret_void,	    PROP_FUNC(f_popup_settext)},
    {"popup_show",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"id",
			ret_സംഖ്യ,	    PROP_FUNC(f_popup_show)},
    {"pow",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"x, y",
			ret_float,	    f_pow},
    {"preinserted",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_സംഖ്യ,	    f_prevnonblank},
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
    {"prop_add",	3, 3, FEARG_1,	    arg3_സംഖ്യ_സംഖ്യ_dict,
			"lnum, col, props",
			ret_സംഖ്യ,	    PROP_FUNC(f_prop_add)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_add_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_സംഖ്യ_സംഖ്യ_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_find)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_സംഖ്യ_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_സംഖ്യ_സംഖ്യ,
			"props, lnum, lnum_end",
			ret_സംഖ്യ,	    PROP_FUNC(f_prop_remove)},
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
    {"pum_getpos",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_dict_സംഖ്യ,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON3
	    f_py3eval
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON
	    f_pyeval
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#എങ്കിൽ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
			},
    {"rand",		0, 1, FEARG_1,	    arg1_list_സംഖ്യ,
			"expr",
			ret_സംഖ്യ,	    f_rand},
    {"range",		1, 3, FEARG_1,	    arg3_സംഖ്യ,
			"expr, max, stride",
			ret_list_സംഖ്യ,    f_range},
    {"readblob",	1, 3, FEARG_1,	    arg3_string_സംഖ്യ_സംഖ്യ,
			"fname, offset, size",
			ret_blob,	    f_readblob},
    {"readdir",		1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_string,    f_readdir},
    {"readdirex",	1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_dict_any,  f_readdirex},
    {"readfile",	1, 3, FEARG_1,	    arg3_string_string_സംഖ്യ,
			"fname, type, max",
			ret_list_string,    f_readfile},
    {"redraw_listener_add", 1, 1, FEARG_1,  arg1_dict_any,
			"opts",
			ret_സംഖ്യ,	    f_redraw_listener_add},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_സംഖ്യ,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_reg_recording},
    {"reltime",		0, 2, FEARG_1,	    arg2_list_സംഖ്യ,
			"start, end",
			ret_list_സംഖ്യ,    f_reltime},
    {"reltimefloat",	1, 1, FEARG_1,	    arg1_list_സംഖ്യ,
			"time",
			ret_float,	    f_reltimefloat},
    {"reltimestr",	1, 1, FEARG_1,	    arg1_list_സംഖ്യ,
			"time",
			ret_string,	    f_reltimestr},
    {"remote_expr",	2, 4, FEARG_1,	    arg24_remote_വ്യഞ്ജകം,
			"server, string, idvar, timeout",
			ret_string,	    f_remote_വ്യഞ്ജകം},
    {"remote_foreground", 1, 1, FEARG_1,    arg1_string,
			"server",
			ret_void,	    f_remote_foreground},
    {"remote_peek",	1, 2, FEARG_1,	    arg2_string,
			"serverid, retvar",
			ret_സംഖ്യ,	    f_remote_peek},
    {"remote_read",	1, 2, FEARG_1,	    arg2_string_സംഖ്യ,
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
			ret_സംഖ്യ_bool,    f_rename},
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
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
			},
    {"screenattr",	2, 2, FEARG_1,	    arg2_സംഖ്യ,
			"row, col",
			ret_സംഖ്യ,	    f_screenattr},
    {"screenchar",	2, 2, FEARG_1,	    arg2_സംഖ്യ,
			"row, col",
			ret_സംഖ്യ,	    f_screenchar},
    {"screenchars",	2, 2, FEARG_1,	    arg2_സംഖ്യ,
			"row, col",
			ret_list_സംഖ്യ,    f_screenchars},
    {"screencol",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_സംഖ്യ,
			"winid, lnum, col",
			ret_dict_സംഖ്യ,    f_screenpos},
    {"screenrow",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_screenrow},
    {"screenstring",	2, 2, FEARG_1,	    arg2_സംഖ്യ,
			"row, col",
			ret_string,	    f_screenstring},
    {"search",		1, 5, FEARG_1,	    arg15_തിരയൽ,
			"pattern, flags, stopline, timeout, skip",
			ret_സംഖ്യ,	    f_തിരയൽ},
    {"searchcount",	0, 1, FEARG_1,	    arg1_dict_any,
			"options",
			ret_dict_സംഖ്യ,    f_searchcount},
    {"searchdecl",	1, 3, FEARG_1,	    arg3_string_bool_bool,
			"name, global, thisblock",
			ret_സംഖ്യ_bool,    f_searchdecl},
    {"searchpair",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_സംഖ്യ,	    f_searchpair},
    {"searchpairpos",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_list_സംഖ്യ,    f_searchpairpos},
    {"searchpos",	1, 5, FEARG_1,	    arg15_തിരയൽ,
			"pattern, flags, stopline, timeout, skip",
			ret_list_സംഖ്യ,    f_searchpos},
    {"server2client",	2, 2, FEARG_1,	    arg2_string,
			"clientid, string",
			ret_സംഖ്യ_bool,    f_server2client},
    {"serverlist",	0, 1, 0,	    arg1_dict_any,
			"dict",
			ret_string_or_any_1,	    f_serverlist},
    {"setbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_സംഖ്യ_bool,    f_setbufline},
    {"setbufvar",	3, 3, FEARG_3,	    arg3_buffer_string_any,
			"buf, varname, val",
			ret_void,	    f_setbufvar},
    {"setcellwidths",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_void,	    f_setcellwidths},
    {"setcharpos",	2, 2, FEARG_2,	    arg2_string_list_സംഖ്യ,
			"expr, list",
			ret_സംഖ്യ_bool,    f_setcharpos},
    {"setcharsearch",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_setcharsearch},
    {"setcmdline",	1, 2, FEARG_1,	    arg2_string_സംഖ്യ,
			"str, pos",
			ret_സംഖ്യ_bool,    f_setcmdline},
    {"setcmdpos",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"pos",
			ret_സംഖ്യ_bool,    f_setcmdpos},
    {"setcursorcharpos", 1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_സംഖ്യ_bool,    f_setcursorcharpos},
    {"setenv",		2, 2, FEARG_2,	    arg2_string_any,
			"name, val",
			ret_void,	    f_setenv},
    {"setfperm",	2, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_സംഖ്യ_bool,    f_setfperm},
    {"setline",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_സംഖ്യ_bool,    f_setline},
    {"setloclist",	2, 4, FEARG_2,	    arg24_setloclist,
			"nr, list, action, what",
			ret_സംഖ്യ_bool,    f_setloclist},
    {"setmatches",	1, 2, FEARG_1,	    arg2_list_any_സംഖ്യ,
			"list, win",
			ret_സംഖ്യ_bool,    f_setmatches},
    {"setpos",		2, 2, FEARG_2,	    arg2_string_list_സംഖ്യ,
			"expr, list",
			ret_സംഖ്യ_bool,    f_setpos},
    {"setqflist",	1, 3, FEARG_1,	    arg13_setqflist,
			"list, action, what",
			ret_സംഖ്യ_bool,    f_setqflist},
    {"setreg",		2, 3, FEARG_2,	    arg3_string_any_string,
			"regname, value, options",
			ret_സംഖ്യ_bool,    f_setreg},
    {"settabvar",	3, 3, FEARG_3,	    arg3_സംഖ്യ_string_any,
			"tabnr, varname, val",
			ret_void,	    f_settabvar},
    {"settabwinvar",	4, 4, FEARG_4,	    arg4_സംഖ്യ_സംഖ്യ_string_any,
			"tabnr, winnr, varname, val",
			ret_void,	    f_settabwinvar},
    {"settagstack",	2, 3, FEARG_2,	    arg23_settagstack,
			"nr, dict, action",
			ret_സംഖ്യ_bool,    f_settagstack},
    {"setwinvar",	3, 3, FEARG_3,	    arg3_സംഖ്യ_string_any,
			"winnr, varname, val",
			ret_void,	    f_setwinvar},
    {"sha256",		1, 1, FEARG_1,	    arg1_string_or_blob,
			"expr",
			ret_string,
#ifdef FEAT_CRYPT
	    f_sha256
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
			},
    {"shellescape",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, special",
			ret_string,	    f_shellescape},
    {"shiftwidth",	0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"col",
			ret_സംഖ്യ,	    f_shiftwidth},
    {"sign_define",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"name, dict",
			ret_sign_define,	    SIGN_FUNC(f_sign_define)},
    {"sign_getdefined",	0, 1, FEARG_1,	    arg1_string,
			"name",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getdefined)},
    {"sign_getplaced",	0, 2, FEARG_1,	    arg02_sign_getplaced,
			"buf, dict",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getplaced)},
    {"sign_jump",	3, 3, FEARG_1,	    arg3_സംഖ്യ_string_buffer,
			"id, group, buf",
			ret_സംഖ്യ,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_place,
			"id, group, name, buf, dict",
			ret_സംഖ്യ,	    SIGN_FUNC(f_sign_place)},
    {"sign_placelist",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_സംഖ്യ,    SIGN_FUNC(f_sign_placelist)},
    {"sign_undefine",	0, 1, FEARG_1,	    arg1_string_or_list_string,
			"name",
			ret_സംഖ്യ_bool,    SIGN_FUNC(f_sign_undefine)},
    {"sign_unplace",	1, 2, FEARG_1,	    arg2_string_dict,
			"group, dict",
			ret_സംഖ്യ_bool,    SIGN_FUNC(f_sign_unplace)},
    {"sign_unplacelist", 1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_സംഖ്യ,    SIGN_FUNC(f_sign_unplacelist)},
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
    {"sound_clear",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_void,	    SOUND_FUNC(f_sound_clear)},
    {"sound_playevent",	1, 2, FEARG_1,	    arg2_string_any,
			"name, callback",
			ret_സംഖ്യ,	    SOUND_FUNC(f_sound_playevent)},
    {"sound_playfile",	1, 2, FEARG_1,	    arg2_string_any,
			"path, callback",
			ret_സംഖ്യ,	    SOUND_FUNC(f_sound_playfile)},
    {"sound_stop",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"id",
			ret_void,	    SOUND_FUNC(f_sound_stop)},
    {"soundfold",	1, 1, FEARG_1,	    arg1_string,
			"word",
			ret_string,	    f_soundfold},
    {"spellbadword",	0, 1, FEARG_1,	    arg1_string,
			"sentence",
			ret_list_string,    f_spellbadword},
    {"spellsuggest",	1, 3, FEARG_1,	    arg3_string_സംഖ്യ_bool,
			"word, max, capital",
			ret_list_string,    f_spellsuggest},
    {"split",		1, 3, FEARG_1,	    arg3_string_string_bool,
			"string, pattern, keepempty",
			ret_list_string,    f_split},
    {"sqrt",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sqrt},
    {"srand",		0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"expr",
			ret_list_സംഖ്യ,    f_srand},
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
			ret_list_സംഖ്യ,    f_str2list},
    {"str2nr",		1, 3, FEARG_1,	    arg3_string_സംഖ്യ_bool,
			"string, base, quoted",
			ret_സംഖ്യ,	    f_str2nr},
    {"strcharlen",	1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_സംഖ്യ,	    f_strcharlen},
    {"strcharpart",	2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, skipcc",
			ret_string,	    f_strcharpart},
    {"strchars",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, skipcc",
			ret_സംഖ്യ,	    f_strchars},
    {"strdisplaywidth",	1, 2, FEARG_1,	    arg2_string_സംഖ്യ,
			"string, col",
			ret_സംഖ്യ,	    f_strdisplaywidth},
    {"strftime",	1, 2, FEARG_1,	    arg2_string_സംഖ്യ,
			"format, time",
			ret_string,
#ifdef HAVE_STRFTIME
	    f_strftime
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_സംഖ്യ,
			"str, index",
			ret_സംഖ്യ,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_സംഖ്യ,
			"haystack, needle, start",
			ret_സംഖ്യ,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, ശൂന്യ,
			"expr",
			ret_string,	    f_string},
    {"strlen",		1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_സംഖ്യ,	    f_strlen},
    {"strpart",		2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, chars",
			ret_string,	    f_strpart},
    {"strptime",	2, 2, FEARG_1,	    arg2_string,
			"format, timestring",
			ret_സംഖ്യ,
#ifdef HAVE_STRPTIME
	    f_strptime
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
			},
    {"strridx",		2, 3, FEARG_1,	    arg3_string_string_സംഖ്യ,
			"haystack, needle, start",
			ret_സംഖ്യ,	    f_strridx},
    {"strtrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_strtrans},
    {"strutf16len",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, countcc",
			ret_സംഖ്യ,	    f_strutf16len},
    {"strwidth",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_സംഖ്യ,	    f_strwidth},
    {"submatch",	1, 2, FEARG_1,	    arg2_സംഖ്യ_bool,
			"nr, list",
			ret_string_or_any_2,	    f_submatch},
    {"substitute",	4, 4, FEARG_1,	    arg4_string_string_any_string,
			"string, pat, sub, flags",
			ret_string,	    f_substitute},
    {"swapfilelist",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_swapname},
    {"synID",		3, 3, 0,	    arg3_lnum_സംഖ്യ_bool,
			"lnum, col, trans",
			ret_സംഖ്യ,	    f_synID},
    {"synIDattr",	2, 3, FEARG_1,	    arg3_സംഖ്യ_string_string,
			"synID, what, mode",
			ret_string,	    f_synIDattr},
    {"synIDtrans",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"synID",
			ret_സംഖ്യ,	    f_synIDtrans},
    {"synconcealed",	2, 2, 0,	    arg2_lnum_സംഖ്യ,
			"lnum, col",
			ret_list_any,	    f_synconcealed},
    {"synstack",	2, 2, 0,	    arg2_lnum_സംഖ്യ,
			"lnum, col",
			ret_list_സംഖ്യ,    f_synstack},
    {"system",		1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_string,	    f_system},
    {"systemlist",	1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_list_string,    f_systemlist},
    {"tabpagebuflist",	0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"arg",
			ret_list_സംഖ്യ,    f_tabpagebuflist},
    {"tabpagenr",	0, 1, 0,	    arg1_string,
			"arg",
			ret_സംഖ്യ,	    f_tabpagenr},
    {"tabpagewinnr",	1, 2, FEARG_1,	    arg2_സംഖ്യ_string,
			"tabarg, arg",
			ret_സംഖ്യ,	    f_tabpagewinnr},
    {"tabpanel_getinfo", 0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_സംഖ്യ_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
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
    {"tempname",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_സംഖ്യ,	    TERM_FUNC(f_term_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_സംഖ്യ,	    TERM_FUNC(f_term_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_buffer_string_dict,
			"buf, filename, options",
			ret_void,	    TERM_FUNC(f_term_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_buffer,
			"buf",
			ret_സംഖ്യ,	    TERM_FUNC(f_term_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_buffer,
			"buf",
			ret_list_string,
#എങ്കിൽ defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_getansicolors
#അല്ലെങ്കിൽ
	    ശൂന്യ
#endif
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_സംഖ്യ_string,
			"attr, what",
			ret_സംഖ്യ,	    TERM_FUNC(f_term_getattr)},
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
			ret_സംഖ്യ,	    TERM_FUNC(f_term_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_സംഖ്യ,    TERM_FUNC(f_term_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, input",
			ret_string,	    TERM_FUNC(f_term_gettty)},
    {"term_list",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_list_സംഖ്യ,    TERM_FUNC(f_term_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_buffer_lnum,
			"buf, row",
			ret_list_dict_any,  TERM_FUNC(f_term_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_buffer_string,
			"buf, keys",
			ret_void,	    TERM_FUNC(f_term_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_buffer_list_any,
			"buf, colors",
			ret_void,
#എങ്കിൽ defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_setansicolors
#അല്ലെങ്കിൽ
	    ശൂന്യ
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
    {"term_setsize",	3, 3, FEARG_1,	    arg3_buffer_സംഖ്യ_സംഖ്യ,
			"buf, rows, cols",
			ret_void,	    TERM_FUNC(f_term_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_സംഖ്യ,	    TERM_FUNC(f_term_start)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_buffer_സംഖ്യ,
			"buf, time",
			ret_void,	    TERM_FUNC(f_term_wait)},
    {"terminalprops",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_സംഖ്യ,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    ശൂന്യ,
			ശൂന്യ,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   ശൂന്യ,
			ശൂന്യ,
			ret_void,	    f_test_garbagecollect_soon},
    {"test_getvalue",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_സംഖ്യ,	    f_test_getvalue},
    {"test_gui_event",	2, 2, FEARG_1,	    arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_gui_event},
    {"test_ignore_error", 1, 1, FEARG_1,    arg1_string,
			"expr",
			ret_void,	    f_test_ignore_error},
    {"test_mswin_event", 2, 2, FEARG_1,     arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_mswin_event},
    {"test_null_blob",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_channel,	    JOB_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_job,	    JOB_FUNC(f_test_null_job)},
    {"test_null_list",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_set},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_സംഖ്യ,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, ശൂന്യ,
			"expr",
			ret_സംഖ്യ,	    f_test_refcount},
    {"test_setmouse",	2, 2, 0,	    arg2_സംഖ്യ,
			"row, col",
			ret_void,	    f_test_setmouse},
    {"test_settime",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"expr",
			ret_void,	    f_test_settime},
    {"test_srand_seed",	0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"seed",
			ret_void,	    f_test_srand_seed},
    {"test_unknown",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_സംഖ്യ_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_സംഖ്യ_any_dict,
			"time, callback, options",
			ret_സംഖ്യ,	    TIMER_FUNC(f_timer_start)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
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
    {"trim",		1, 3, FEARG_1,	    arg3_string_string_സംഖ്യ,
			"text, mask, dir",
			ret_string,	    f_trim},
    {"trunc",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_trunc},
    {"tuple2list",	1, 1, FEARG_1,	    arg1_tuple_any,
			"tuple",
			ret_list_any,	    f_tuple2list},
    {"type",		1, 1, FEARG_1|FE_X, ശൂന്യ,
			"expr",
			ret_സംഖ്യ,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, ശൂന്യ,
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
    {"utf16idx",	2, 4, FEARG_1,	    arg4_string_സംഖ്യ_bool_bool,
			"string, idx, countcc, charidx",
			ret_സംഖ്യ,	    f_utf16idx},
    {"values",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_member,    f_മൂല്യങ്ങൾ},
    {"virtcol",		1, 3, FEARG_1,	    arg3_string_or_list_bool_സംഖ്യ,
			"expr, list, winid",
			ret_virtcol,	    f_virtcol},
    {"virtcol2col",	3, 3, FEARG_1,	    arg3_സംഖ്യ,
			"winid, lnum, col",
			ret_സംഖ്യ,	    f_virtcol2col},
    {"visualmode",	0, 1, 0,	    arg1_bool,
			"expr",
			ret_string,	    f_visualmode},
    {"wildmenumode",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_void,	    f_wildtrigger},
    {"win_execute",	2, 3, FEARG_2,	    arg23_win_execute,
			"id, command, silent",
			ret_string,	    f_win_execute},
    {"win_findbuf",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"bufnr",
			ret_list_സംഖ്യ,    f_win_findbuf},
    {"win_getid",	0, 2, FEARG_1,	    arg2_സംഖ്യ,
			"win, tab",
			ret_സംഖ്യ,	    f_win_getid},
    {"win_gettype",	0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"nr",
			ret_string,	    f_win_gettype},
    {"win_gotoid",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"expr",
			ret_സംഖ്യ_bool,    f_win_gotoid},
    {"win_id2tabwin",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"expr",
			ret_list_സംഖ്യ,    f_win_id2tabwin},
    {"win_id2win",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"expr",
			ret_സംഖ്യ,	    f_win_id2win},
    {"win_move_separator", 2, 2, FEARG_1,   arg2_സംഖ്യ,
			"nr, offset",
			ret_സംഖ്യ_bool,    f_win_move_separator},
    {"win_move_statusline", 2, 2, FEARG_1,  arg2_സംഖ്യ,
			"nr, offset",
			ret_സംഖ്യ_bool,    f_win_move_statusline},
    {"win_screenpos",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"nr",
			ret_list_സംഖ്യ,    f_win_screenpos},
    {"win_splitmove",   2, 3, FEARG_1,	    arg3_സംഖ്യ_സംഖ്യ_dict,
			"nr, target, options",
			ret_സംഖ്യ_bool,    f_win_splitmove},
    {"winbufnr",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"nr",
			ret_സംഖ്യ,	    f_winbufnr},
    {"wincol",		0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"nr",
			ret_സംഖ്യ,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_സംഖ്യ,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_സംഖ്യ,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_സംഖ്യ,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_dict_സംഖ്യ,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_സംഖ്യ,
			"nr",
			ret_സംഖ്യ,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    ശൂന്യ,
			ശൂന്യ,
			ret_dict_സംഖ്യ,    f_wordcount},
    {"writefile",	2, 3, FEARG_1,	    arg23_writefile,
			"object, fname, flags",
			ret_സംഖ്യ_bool,    f_writefile},
    {"xor",		2, 2, FEARG_1,	    arg2_സംഖ്യ,
			"expr1, expr2",
			ret_സംഖ്യ,	    f_xor},
};

/*
 * Return true if specified function allows a type as an argument.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
func_allows_type(പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, പൂർണ്ണസംഖ്യ idx)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	intidx = -1;
    char_u	*name;

    എങ്കിൽ (idx == 0)
	intidx = -1;
    എങ്കിൽ (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	എങ്കിൽ (name != ശൂന്യ)
	{
	    എങ്കിൽ (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_pattern, 2) == 0)
		തിരികെ cat_prefix_varname('g', name);
	    തിരികെ name;
	}
    }
    എങ്കിൽ (++intidx < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	എങ്കിൽ (global_functions[intidx].f_func == ശൂന്യ)
	    തിരികെ (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	എങ്കിൽ (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	തിരികെ IObuff;
    }

    തിരികെ ശൂന്യ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_വ്യഞ്ജകം_name(expand_T *xp, പൂർണ്ണസംഖ്യ idx)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	intidx = -1;
    char_u	*name;

    എങ്കിൽ (idx == 0)
	intidx = -1;
    എങ്കിൽ (intidx < 0)
    {
	name = get_function_name(xp, idx);
	എങ്കിൽ (name != ശൂന്യ)
	    തിരികെ name;
    }
    തിരികെ get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_internal_func_opt(char_u *name, പൂർണ്ണസംഖ്യ implemented)
{
    പൂർണ്ണസംഖ്യ		first = 0;
    പൂർണ്ണസംഖ്യ		last;
    പൂർണ്ണസംഖ്യ		cmp;
    പൂർണ്ണസംഖ്യ		x;

    last = (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(global_functions) - 1;

    // Find the function name in the table. Binary search.
    ആകുമ്പോൾ (first <= last)
    {
	x = first + ((unsigned)(last - first) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	എങ്കിൽ (cmp < 0)
	    last = x - 1;
	അല്ലെങ്കിൽ എങ്കിൽ (cmp > 0)
	    first = x + 1;
	അല്ലെങ്കിൽ എങ്കിൽ (implemented && global_functions[x].f_func == ശൂന്യ)
	    നിർത്തുക;
	അല്ലെങ്കിൽ
	    തിരികെ x;
    }
    തിരികെ -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    പൂർണ്ണസംഖ്യ
find_internal_func(char_u *name)
{
    തിരികെ find_internal_func_opt(name, TRUE);
}

    പൂർണ്ണസംഖ്യ
has_internal_func(char_u *name)
{
    തിരികെ find_internal_func_opt(name, TRUE) >= 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
has_internal_func_name(char_u *name)
{
    തിരികെ find_internal_func_opt(name, FALSE) >= 0;
}

    അക്ഷരം *
internal_func_name(പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    പൂർണ്ണസംഖ്യ
internal_func_check_arg_types(
	type2_T	*types,
	പൂർണ്ണസംഖ്യ	idx,
	പൂർണ്ണസംഖ്യ	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    എങ്കിൽ (!(func_allows_type(idx)))
    {
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < argcount; ++i)
	    എങ്കിൽ (check_type_is_മൂല്യം(types[i].type_curr) == FAIL)
		തിരികെ FAIL;
    }

    സ്ഥിരം argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    എങ്കിൽ (argchecks == ശൂന്യ)
	തിരികെ OK;

    argcontext_T context;

    context.arg_എണ്ണം = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < argcount && argchecks[i] != ശൂന്യ; ++i)
    {
	context.arg_idx = i;
	എങ്കിൽ (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    ശൂന്യം
internal_func_get_argcount(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ *argcount, പൂർണ്ണസംഖ്യ *min_argcount)
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
	പൂർണ്ണസംഖ്യ	    idx,
	പൂർണ്ണസംഖ്യ	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = ശൂന്യ;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    എങ്കിൽ (*decl_type == ശൂന്യ)
	*decl_type = ret;
    current_type_gap = ശൂന്യ;
    തിരികെ ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    പൂർണ്ണസംഖ്യ
internal_func_is_map(പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    പൂർണ്ണസംഖ്യ
check_internal_func(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ argcount)
{
    funcerror_T	    res;
    അക്ഷരം	    *name;

    എങ്കിൽ (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    അല്ലെങ്കിൽ എങ്കിൽ (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    അല്ലെങ്കിൽ
	തിരികെ global_functions[idx].f_argtype & FEARG_MASK;

    name = internal_func_name(idx);
    എങ്കിൽ (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    അല്ലെങ്കിൽ
	semsg(_(e_not_enough_arguments_for_function_str), name);
    തിരികെ -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_args_for_type(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ argcount, typval_T *argvars)
{
    എങ്കിൽ (!func_allows_type(idx))
    {
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < argcount; ++i)
	    എങ്കിൽ (check_typval_is_മൂല്യം(&argvars[i]) == FAIL)
		തിരികെ FAIL;
    }
    തിരികെ OK;
}

    funcerror_T
call_internal_func(
	char_u	    *name,
	പൂർണ്ണസംഖ്യ	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    പൂർണ്ണസംഖ്യ i;

    i = find_internal_func(name);
    എങ്കിൽ (i < 0)
	തിരികെ FCERR_UNKNOWN;
    എങ്കിൽ (argcount < global_functions[i].f_min_argc)
	തിരികെ FCERR_TOOFEW;
    എങ്കിൽ (argcount > global_functions[i].f_max_argc)
	തിരികെ FCERR_TOOMANY;
    എങ്കിൽ (check_args_for_type(i, argcount, argvars) == FAIL)
	തിരികെ FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    എങ്കിൽ (in_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    തിരികെ FCERR_NONE;
}

    ശൂന്യം
call_internal_func_by_idx(
	പൂർണ്ണസംഖ്യ	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    എങ്കിൽ (in_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_internal_method(
	char_u	    *name,
	പൂർണ്ണസംഖ്യ	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    പൂർണ്ണസംഖ്യ		fi;
    typval_T	argv[MAX_FUNC_ARGS + 1];

    fi = find_internal_func(name);
    എങ്കിൽ (fi < 0)
	തിരികെ FCERR_UNKNOWN;
    എങ്കിൽ ((global_functions[fi].f_argtype & FEARG_MASK) == 0)
	തിരികെ FCERR_NOTMETHOD;
    എങ്കിൽ (argcount + 1 < global_functions[fi].f_min_argc)
	തിരികെ FCERR_TOOFEW;
    എങ്കിൽ (argcount + 1 > global_functions[fi].f_max_argc)
	തിരികെ FCERR_TOOMANY;
    എങ്കിൽ (check_args_for_type(fi, argcount, argvars) == FAIL)
	തിരികെ FCERR_OTHER;

    എങ്കിൽ ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_2)
    {
	എങ്കിൽ (argcount < 1)
	    തിരികെ FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_3)
    {
	എങ്കിൽ (argcount < 2)
	    തിരികെ FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_4)
    {
	എങ്കിൽ (argcount < 3)
	    തിരികെ FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    അല്ലെങ്കിൽ
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    എങ്കിൽ (check_args_for_type(fi, argcount + 1, argv) == FAIL)
	തിരികെ FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    തിരികെ FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    പൂർണ്ണസംഖ്യ
non_zero_arg(typval_T *argvars)
{
    തിരികെ ((argvars[0].v_type == VAR_സംഖ്യ
		&& argvars[0].vval.v_സംഖ്യ != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_സംഖ്യ == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != ശൂന്യ
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    സ്ഥിതം ശൂന്യം
f_and(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    rettv->vval.v_സംഖ്യ = tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ)
					& tv_get_സംഖ്യ_chk(&argvars[1], ശൂന്യ);
}

/*
 * "balloon_show()" function
 */
#ifdef FEAT_BEVAL
    സ്ഥിതം ശൂന്യം
f_balloon_gettext(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    എങ്കിൽ (balloonEval == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (balloonEval->സന്ദേശം == ശൂന്യ)
	rettv->vval.v_string = ശൂന്യ;
    അല്ലെങ്കിൽ
	rettv->vval.v_string = vim_strsave(balloonEval->സന്ദേശം);
}

    സ്ഥിതം ശൂന്യം
f_balloon_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (balloonEval == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, ശൂന്യ,
		l == ശൂന്യ || l->lv_len == 0 ? ശൂന്യ : l);
    }
    അല്ലെങ്കിൽ
    {
	char_u *mesg;

	എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    തിരികെ;

	mesg = tv_get_string_chk(&argvars[0]);
	എങ്കിൽ (mesg != ശൂന്യ)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? ശൂന്യ : mesg, ശൂന്യ);
    }
}

# എങ്കിൽ defined(FEAT_BEVAL_TERM)
    സ്ഥിതം ശൂന്യം
f_balloon_split(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (rettv_list_alloc(rettv) != OK)
	തിരികെ;

    char_u *സന്ദേശം;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;
    സന്ദേശം = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (സന്ദേശം != ശൂന്യ)
    {
	pumitem_T	*array;
	പൂർണ്ണസംഖ്യ		size = split_message(സന്ദേശം, &array);

	// Skip the first and last item, they are always empty.
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < size - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_text, -1);
	ആകുമ്പോൾ (size > 0)
	    vim_free(array[--size].pum_text);
	vim_free(array);
    }
}
# endif
#endif

/*
 * "base64_decode(string)" function
 */
    സ്ഥിതം ശൂന്യം
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (rettv_blob_alloc(rettv) == FAIL)
	തിരികെ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (str != ശൂന്യ)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    സ്ഥിതം ശൂന്യം
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (check_for_blob_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    blob_T *blob = argvars->vval.v_blob;
    എങ്കിൽ (blob != ശൂന്യ)
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
    എങ്കിൽ (buf == ശൂന്യ)
	semsg(_(e_invalid_buffer_name_str), tv_get_string(arg));
    തിരികെ buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    സ്ഥിതം ശൂന്യം
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_സംഖ്യ = VVAL_TRUE;

    എങ്കിൽ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	തിരികെ;

    എങ്കിൽ (strcmp((സ്ഥിരം അക്ഷരം *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (bindtextdomain((സ്ഥിരം അക്ഷരം *)argvars[0].vval.v_string, (സ്ഥിരം അക്ഷരം *)argvars[1].vval.v_string) == ശൂന്യ)
	{
	    do_outofmem_സന്ദേശം((long)0);
	    rettv->vval.v_സംഖ്യ = VVAL_FALSE;
	}
    }

    തിരികെ;
}

/*
 * "byte2line(byte)" function
 */
    സ്ഥിതം ശൂന്യം
f_byte2line(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_സംഖ്യ = -1;
#അല്ലെങ്കിൽ
    long	boff = 0;

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    boff = tv_get_സംഖ്യ(&argvars[0]) - 1;  // boff gets -1 on type error
    എങ്കിൽ (boff < 0)
	rettv->vval.v_സംഖ്യ = -1;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = ml_find_വരി_or_offset(curbuf,
							  (linenr_T)0, &boff);
#endif
}

/*
 * "call(func, arglist [, dict])" function
 */
    സ്ഥിതം ശൂന്യം
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = ശൂന്യ;
    dict_T	*selfdict = ശൂന്യ;
    char_u	*tofree = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	തിരികെ;

    എങ്കിൽ (check_for_list_arg(argvars, 1) == FAIL)
	തിരികെ;
    എങ്കിൽ (argvars[1].vval.v_list == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    അല്ലെങ്കിൽ
	func = tv_get_string(&argvars[0]);
    എങ്കിൽ (func == ശൂന്യ || *func == NUL)
	തിരികെ;		// type error, empty name or null function

    എങ്കിൽ (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, ശൂന്യ, FALSE, TFN_INT|TFN_QUIET);
	എങ്കിൽ (tofree == ശൂന്യ)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    തിരികെ;
	}
	എങ്കിൽ (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_args(tofree, STRLEN(tofree),
									&p);
	    എങ്കിൽ (s != ശൂന്യ)
	    {
		vim_free(tofree);
		tofree = s;
	    }
	}
	func = tofree;
    }

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	എങ്കിൽ (check_for_dict_arg(argvars, 2) == FAIL)
	    പോകുക done;

	selfdict = argvars[2].vval.v_dict;
    }

    (ശൂന്യം)func_call(func, &argvars[1], partial, selfdict, rettv);

done:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    സ്ഥിതം ശൂന്യം
f_changenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    സ്ഥിതം ശൂന്യം
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (has_mbyte)
    {
	പൂർണ്ണസംഖ്യ	utf8 = 0;

	എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[1], ശൂന്യ);

	എങ്കിൽ (utf8)
	    rettv->vval.v_സംഖ്യ = utf_ptr2char(tv_get_string(&argvars[0]));
	അല്ലെങ്കിൽ
	    rettv->vval.v_സംഖ്യ = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    സ്ഥിതം ശൂന്യം
get_col(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ charcol)
{
    colnr_T	col = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    പൂർണ്ണസംഖ്യ		winchanged = FALSE;

    എങ്കിൽ (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[1]), &tp);
	എങ്കിൽ (wp == ശൂന്യ || tp == ശൂന്യ)
	    തിരികെ;

	എങ്കിൽ (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    തിരികെ;

	check_cursor();
	winchanged = TRUE;
    }

    പൂർണ്ണസംഖ്യ fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    എങ്കിൽ (fp != ശൂന്യ && fnum == curbuf->b_fnum)
    {
	എങ്കിൽ (fp->col == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    എങ്കിൽ (fp->lnum <= curbuf->b_ml.ml_വരി_എണ്ണം)
		col = ml_get_len(fp->lnum) + 1;
	    അല്ലെങ്കിൽ
		col = MAXCOL;
	}
	അല്ലെങ്കിൽ
	{
	    col = fp->col + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    എങ്കിൽ (virtual_active() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		എങ്കിൽ (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    പൂർണ്ണസംഖ്യ		l;

		    എങ്കിൽ (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			col += l;
		}
	    }
	}
    }
    rettv->vval.v_സംഖ്യ = col;

    എങ്കിൽ (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    സ്ഥിതം ശൂന്യം
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, TRUE);
}

    win_T *
get_optional_window(typval_T *argvars, പൂർണ്ണസംഖ്യ idx)
{
    win_T   *win = curwin;

    എങ്കിൽ (argvars[idx].v_type == VAR_UNKNOWN)
	തിരികെ curwin;

    win = find_win_by_nr_or_id(&argvars[idx]);
    എങ്കിൽ (win == ശൂന്യ)
    {
	emsg(_(e_invalid_window_സംഖ്യ));
	തിരികെ ശൂന്യ;
    }
    തിരികെ win;
}

/*
 * "col(string)" function
 */
    സ്ഥിതം ശൂന്യം
f_col(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    സ്ഥിതം ശൂന്യം
f_confirm(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*message;
    char_u	*buttons = ശൂന്യ;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		def = 1;
    പൂർണ്ണസംഖ്യ		type = VIM_GENERIC;
    char_u	*typestr;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	തിരികെ;

    message = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (message == ശൂന്യ)
	error = TRUE;
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	എങ്കിൽ (buttons == ശൂന്യ)
	    error = TRUE;
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[2], &error);
	    എങ്കിൽ (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		എങ്കിൽ (typestr == ശൂന്യ)
		    error = TRUE;
		അല്ലെങ്കിൽ
		{
		    തിരഞ്ഞെടുപ്പ് (TOUPPER_ASC(*typestr))
		    {
			അവസ്ഥ 'E': type = VIM_ERROR; നിർത്തുക;
			അവസ്ഥ 'Q': type = VIM_QUESTION; നിർത്തുക;
			അവസ്ഥ 'I': type = VIM_INFO; നിർത്തുക;
			അവസ്ഥ 'W': type = VIM_WARNING; നിർത്തുക;
			അവസ്ഥ 'G': type = VIM_GENERIC; നിർത്തുക;
		    }
		}
	    }
	}
    }

    എങ്കിൽ (buttons == ശൂന്യ || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    എങ്കിൽ (!error)
	rettv->vval.v_സംഖ്യ = do_dialog(type, ശൂന്യ, message, buttons,
							    def, ശൂന്യ, FALSE);
#endif
}

/*
 * "copy()" function
 */
    സ്ഥിതം ശൂന്യം
f_copy(typval_T *argvars, typval_T *rettv)
{
    item_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    സ്ഥിതം ശൂന്യം
set_cursorpos(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ charcol)
{
    long	lnum, col;
    long	coladd = 0;
    പൂർണ്ണസംഖ്യ		set_curswant = TRUE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_or_സംഖ്യ_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL)))
	തിരികെ;

    rettv->vval.v_സംഖ്യ = -1;
    എങ്കിൽ (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	എങ്കിൽ (list2fpos(argvars, &pos, ശൂന്യ, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_argument));
	    തിരികെ;
	}
	lnum = pos.lnum;
	col = pos.col;
	coladd = pos.coladd;
	എങ്കിൽ (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    set_curswant = FALSE;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((argvars[0].v_type == VAR_സംഖ്യ
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_സംഖ്യ
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	എങ്കിൽ (lnum < 0)
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
	അല്ലെങ്കിൽ എങ്കിൽ (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	col = (long)tv_get_സംഖ്യ_chk(&argvars[1], ശൂന്യ);
	എങ്കിൽ (charcol)
	    col = buf_charidx_to_byteidx(curbuf, lnum, col) + 1;
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (long)tv_get_സംഖ്യ_chk(&argvars[2], ശൂന്യ);
    }
    അല്ലെങ്കിൽ
    {
	emsg(_(e_invalid_argument));
	തിരികെ;
    }
    എങ്കിൽ (lnum < 0 || col < 0 || coladd < 0)
	തിരികെ;		// type error; errmsg already given
    എങ്കിൽ (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    എങ്കിൽ (col != MAXCOL && --col < 0)
	col = 0;
    curwin->w_cursor.col = col;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    എങ്കിൽ (has_mbyte)
	mb_adjust_cursor();

    curwin->w_set_curswant = set_curswant;
    rettv->vval.v_സംഖ്യ = 0;
}

/*
 * "cursor(lnum, col)" function, or
 * "cursor(list)"
 *
 * Moves the cursor to the specified line and column.
 * Returns 0 when the position could be set, -1 otherwise.
 */
    സ്ഥിതം ശൂന്യം
f_cursor(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, FALSE);
}

#ifdef MSWIN
/*
 * "debugbreak()" function
 */
    സ്ഥിതം ശൂന്യം
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		pid;

    rettv->vval.v_സംഖ്യ = FAIL;
    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    pid = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]);
    എങ്കിൽ (pid == 0)
    {
	emsg(_(e_invalid_argument));
	തിരികെ;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    എങ്കിൽ (hProcess == ശൂന്യ)
	തിരികെ;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_സംഖ്യ = OK;
}
#endif

/*
 * "deepcopy()" function
 */
    സ്ഥിതം ശൂന്യം
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    എങ്കിൽ (check_for_opt_bool_arg(argvars, 1) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], ശൂന്യ);

    item_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    സ്ഥിതം ശൂന്യം
f_did_filetype(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->vval.v_സംഖ്യ = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    സ്ഥിതം ശൂന്യം
f_echoraw(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *str;

    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    str = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (str != ശൂന്യ && *str != NUL)
    {
	out_str(str);
	out_flush();
    }
}

/*
 * "empty({expr})" function
 */
    സ്ഥിതം ശൂന്യം
f_empty(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		n = FALSE;

    തിരഞ്ഞെടുപ്പ് (argvars[0].v_type)
    {
	അവസ്ഥ VAR_STRING:
	അവസ്ഥ VAR_FUNC:
	    n = argvars[0].vval.v_string == ശൂന്യ
					  || *argvars[0].vval.v_string == NUL;
	    നിർത്തുക;
	അവസ്ഥ VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_സംഖ്യ:
	    n = argvars[0].vval.v_സംഖ്യ == 0;
	    നിർത്തുക;
	അവസ്ഥ VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    n = argvars[0].vval.v_list == ശൂന്യ
					|| argvars[0].vval.v_list->lv_len == 0;
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    n = argvars[0].vval.v_dict == ശൂന്യ
			|| argvars[0].vval.v_dict->dv_hashtab.ht_used == 0;
	    നിർത്തുക;
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	    n = argvars[0].vval.v_സംഖ്യ != VVAL_TRUE;
	    നിർത്തുക;
	അവസ്ഥ VAR_CLASS:
	    n = argvars[0].vval.v_class != ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_OBJECT:
	    n = object_empty(argvars[0].vval.v_object);
	    നിർത്തുക;

	അവസ്ഥ VAR_BLOB:
	    n = argvars[0].vval.v_blob == ശൂന്യ
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    നിർത്തുക;

	അവസ്ഥ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_job == ശൂന്യ
			   || argvars[0].vval.v_job->jv_status != JOB_STARTED;
	    നിർത്തുക;
#endif
	അവസ്ഥ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_channel == ശൂന്യ
			       || !channel_is_open(argvars[0].vval.v_channel);
	    നിർത്തുക;
#endif
	അവസ്ഥ VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == ശൂന്യ
		|| argvars[0].vval.v_typealias->ta_name == ശൂന്യ
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    നിർത്തുക;

	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	അവസ്ഥ VAR_INSTR:
	    internal_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    നിർത്തുക;
    }

    rettv->vval.v_സംഖ്യ = n;
}

/*
 * "environ()" function
 */
    സ്ഥിതം ശൂന്യം
f_environ(typval_T *argvars UNUSED, typval_T *rettv)
{
#എങ്കിൽ !defined(AMIGA)
    പൂർണ്ണസംഖ്യ			i = 0;
    char_u		*entry, *മൂല്യം;
# എങ്കിൽ defined (MSWIN)
#  എങ്കിൽ !defined(_UCRT)
    ബാഹ്യ wchar_t	**_wenviron;
#  endif
# അല്ലെങ്കിൽ
    ബാഹ്യ അക്ഷരം		**environ;
# endif

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;

# ifdef MSWIN
    എങ്കിൽ (*_wenviron == ശൂന്യ)
	തിരികെ;
# അല്ലെങ്കിൽ
    എങ്കിൽ (*environ == ശൂന്യ)
	തിരികെ;
# endif

    വേണ്ടി (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	എങ്കിൽ ((p = (short_u *)_wenviron[i]) == ശൂന്യ)
	    തിരികെ;
	entry = utf16_to_enc(p, ശൂന്യ);
# അല്ലെങ്കിൽ
	എങ്കിൽ ((entry = (char_u *)environ[i]) == ശൂന്യ)
	    തിരികെ;
	entry = vim_strsave(entry);
# endif
	എങ്കിൽ (entry == ശൂന്യ) // out of memory
	    തിരികെ;
	എങ്കിൽ ((മൂല്യം = vim_strchr(entry, '=')) == ശൂന്യ)
	{
	    vim_free(entry);
	    തുടരുക;
	}
	*മൂല്യം++ = NUL;
	dict_add_string(rettv->vval.v_dict, (അക്ഷരം *)entry, മൂല്യം);
	vim_free(entry);
    }
#endif
}

/*
 * "err_teapot()" function
 */
    സ്ഥിതം ശൂന്യം
f_err_teapot(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	എങ്കിൽ (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    എങ്കിൽ (*skipwhite(s) == NUL)
		തിരികെ;
	}

	പൂർണ്ണസംഖ്യ err = FALSE;
	പൂർണ്ണസംഖ്യ do_503 = eval_വ്യഞ്ജകം_to_bool(&argvars[0], &err);
	എങ്കിൽ (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_available));
	    തിരികെ;
	}
    }

    emsg(_(e_im_a_teapot));
}

/*
 * "escape({string}, {chars})" function
 */
    സ്ഥിതം ശൂന്യം
f_escape(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    rettv->vval.v_string = vim_strsave_escaped(tv_get_string(&argvars[0]),
					 tv_get_string_buf(&argvars[1], buf));
    rettv->v_type = VAR_STRING;
}

/*
 * "eval()" function
 */
    സ്ഥിതം ശൂന്യം
f_eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*s, *p;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    s = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (s != ശൂന്യ)
	s = skipwhite(s);

    p = s;
    എങ്കിൽ (s == ശൂന്യ || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	എങ്കിൽ (p != ശൂന്യ && !aborting())
	    semsg(_(e_invalid_expression_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_സംഖ്യ;
	rettv->vval.v_സംഖ്യ = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    സ്ഥിതം ശൂന്യം
f_eventhandler(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = vgetc_busy || നിവേശം_busy;
}

സ്ഥിതം garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    ശൂന്യം
execute_redir_str(char_u *മൂല്യം, പൂർണ്ണസംഖ്യ മൂല്യം_len)
{
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ (മൂല്യം_len == -1)
	len = (പൂർണ്ണസംഖ്യ)STRLEN(മൂല്യം);	// Append the entire string
    അല്ലെങ്കിൽ
	len = മൂല്യം_len;		// Append only "value_len" characters
    എങ്കിൽ (ga_grow(&redir_execute_ga, len) == FAIL)
	തിരികെ;

    mch_memmove((അക്ഷരം *)redir_execute_ga.ga_data
	    + redir_execute_ga.ga_len, മൂല്യം, len);
    redir_execute_ga.ga_len += len;
}

#എങ്കിൽ defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    സ്ഥിതം char_u *
get_str_വരി(
    പൂർണ്ണസംഖ്യ	    c UNUSED,
    ശൂന്യം    *cookie,
    പൂർണ്ണസംഖ്യ	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    char_u	*start = *(char_u **)cookie;
    char_u	*വരി;
    char_u	*p;

    p = start;
    എങ്കിൽ (p == ശൂന്യ || *p == NUL)
	തിരികെ ശൂന്യ;
    p = vim_strchr(p, '\n');
    എങ്കിൽ (p == ശൂന്യ)
	വരി = vim_strsave(start);
    അല്ലെങ്കിൽ
    {
	വരി = vim_strnsave(start, p - start);
	p++;
    }

    *(char_u **)cookie = p;
    തിരികെ വരി;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    ശൂന്യം
execute_cmds_from_string(char_u *str)
{
    do_cmdline(ശൂന്യ, get_str_വരി, (ശൂന്യം *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#endif

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_വരി(
    പൂർണ്ണസംഖ്യ	    c UNUSED,
    ശൂന്യം    *cookie,
    പൂർണ്ണസംഖ്യ	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *item = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ ശൂന്യ;
    s = tv_get_string_buf_chk(&item->li_tv, buf);
    *p = item->li_next;
    തിരികെ s == ശൂന്യ ? ശൂന്യ : vim_strsave(s);
}

/*
 * "execute()" function
 */
    ശൂന്യം
execute_common(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ arg_off)
{
    char_u	*cmd = ശൂന്യ;
    list_T	*list = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		save_സന്ദേശം_silent = സന്ദേശം_silent;
    പൂർണ്ണസംഖ്യ		save_emsg_silent = emsg_silent;
    പൂർണ്ണസംഖ്യ		save_emsg_noredir = emsg_noredir;
    പൂർണ്ണസംഖ്യ		save_redir_execute = redir_execute;
    പൂർണ്ണസംഖ്യ		save_redir_off = redir_off;
    garray_T	save_ga;
    പൂർണ്ണസംഖ്യ		save_സന്ദേശം_col = സന്ദേശം_col;
    പൂർണ്ണസംഖ്യ		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    പൂർണ്ണസംഖ്യ		echo_output = FALSE;

    rettv->vval.v_string = ശൂന്യ;
    rettv->v_type = VAR_STRING;

    എങ്കിൽ (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	എങ്കിൽ (list == ശൂന്യ || list->lv_len == 0)
	    // empty list, no commands, empty output
	    തിരികെ;
	++list->lv_refcount;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[arg_off].v_type == VAR_JOB
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_മൂല്യം_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	തിരികെ;
    }
    അല്ലെങ്കിൽ
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	എങ്കിൽ (cmd == ശൂന്യ)
	    തിരികെ;
    }

    എങ്കിൽ (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      in_vim9script());

	എങ്കിൽ (s == ശൂന്യ)
	    തിരികെ;
	എങ്കിൽ (*s == NUL)
	    echo_output = TRUE;
	എങ്കിൽ (STRNCMP(s, "silent", 6) == 0)
	    ++സന്ദേശം_silent;
	എങ്കിൽ (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    അല്ലെങ്കിൽ
	++സന്ദേശം_silent;

    എങ്കിൽ (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, വലുപ്പം(അക്ഷരം), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    എങ്കിൽ (!echo_output)
	സന്ദേശം_col = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    എങ്കിൽ (cmd != ശൂന്യ)
	do_cmdline_cmd(cmd);
    അല്ലെങ്കിൽ
    {
	listitem_T	*item;

	CHECK_LIST_MATERIALIZE(list);
	item = list->lv_first;
	do_cmdline(ശൂന്യ, get_list_വരി, (ശൂന്യം *)&item,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    എങ്കിൽ (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((അക്ഷരം *)redir_execute_ga.ga_data)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_data;
    }
    അല്ലെങ്കിൽ
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = ശൂന്യ;
    }
    സന്ദേശം_silent = save_സന്ദേശം_silent;
    emsg_silent = save_emsg_silent;
    emsg_noredir = save_emsg_noredir;

    redir_execute = save_redir_execute;
    എങ്കിൽ (redir_execute)
	redir_execute_ga = save_ga;
    redir_off = save_redir_off;

    // "silent reg" or "silent echo x" leaves msg_col somewhere in the line.
    എങ്കിൽ (echo_output)
	// When not working silently: put it in column zero.  A following
	// "echon" will overwrite the message, unavoidably.
	സന്ദേശം_col = 0;
    അല്ലെങ്കിൽ
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	സന്ദേശം_col = save_സന്ദേശം_col;
}

/*
 * "execute()" function
 */
    സ്ഥിതം ശൂന്യം
f_execute(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    ശൂന്യം
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		n = FALSE;

    എങ്കിൽ (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    p = tv_get_string(&argvars[0]);
    എങ്കിൽ (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	എങ്കിൽ (mch_getenv(p + 1) != ശൂന്യ)
	    n = TRUE;
	അല്ലെങ്കിൽ
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    എങ്കിൽ (p != ശൂന്യ && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, ശൂന്യ, TRUE) == OK);
	എങ്കിൽ (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*p == '?')			// internal function only
    {
	n = has_internal_func_name(p + 1);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*p == '#')
    {
	എങ്കിൽ (p[1] == '#')
	    n = autocmd_supported(p + 2);
	അല്ലെങ്കിൽ
	    n = au_exists(p + 1);
    }
    അല്ലെങ്കിൽ				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_സംഖ്യ = n;
}

    സ്ഥിതം ശൂന്യം
f_exists_compiled(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    emsg(_(e_exists_compiled_can_only_be_used_in_def_function));
}

/*
 * "expand()" function
 */
    സ്ഥിതം ശൂന്യം
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    size_t	len;
    പൂർണ്ണസംഖ്യ		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    char_u	*result;
#ifdef BACKSLASH_IN_FILENAME
    char_u	*p_csl_save = p_csl;
#endif

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	തിരികെ;

#ifdef BACKSLASH_IN_FILENAME
    // avoid using 'completeslash' here
    p_csl = empty_option;
#endif

    rettv->v_type = VAR_STRING;
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_set(rettv, ശൂന്യ);

    s = tv_get_string(&argvars[0]);
    എങ്കിൽ (*s == '%' || *s == '#' || *s == '<')
    {
	അക്ഷരം	*errormsg = ശൂന്യ;

	എങ്കിൽ (p_verbose == 0)
	    ++emsg_off;
	result = eval_vars(s, s, &len, ശൂന്യ, &errormsg, ശൂന്യ, FALSE);
	എങ്കിൽ (p_verbose == 0)
	    --emsg_off;
	അല്ലെങ്കിൽ എങ്കിൽ (errormsg != ശൂന്യ)
	    emsg(errormsg);
	എങ്കിൽ (rettv->v_type == VAR_LIST)
	{
	    എങ്കിൽ (rettv_list_alloc(rettv) == OK && result != ശൂന്യ)
		list_append_string(rettv->vval.v_list, result, -1);
	    vim_free(result);
	}
	അല്ലെങ്കിൽ
	    rettv->vval.v_string = result;
    }
    അല്ലെങ്കിൽ
    {
	// When the optional second argument is non-zero, don't remove matches
	// for 'wildignore' and don't put matches for 'suffixes' at the end.
	എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	എങ്കിൽ (!error)
	{
	    ExpandInit(&xpc);
	    xpc.xp_context = EXPAND_FILES;
	    എങ്കിൽ (p_wic)
		options += WILD_ICASE;
	    എങ്കിൽ (rettv->v_type == VAR_STRING)
		rettv->vval.v_string = ExpandOne(&xpc, s, ശൂന്യ,
							   options, WILD_ALL);
	    അല്ലെങ്കിൽ എങ്കിൽ (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, ശൂന്യ, options, WILD_ALL_KEEP);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	അല്ലെങ്കിൽ
	    rettv->vval.v_string = ശൂന്യ;
    }
#ifdef BACKSLASH_IN_FILENAME
    p_csl = p_csl_save;
#endif
}

/*
 * "expandcmd()" function
 * Expand all the special characters in a command string.
 */
    സ്ഥിതം ശൂന്യം
f_expandcmd(typval_T *argvars, typval_T *rettv)
{
    exarg_T	eap;
    char_u	*cmdstr;
    അക്ഷരം	*errormsg = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		emsgoff = TRUE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_FIELD(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = ശൂന്യ;
    eap.cmdidx = CMD_USER;

    എങ്കിൽ (emsgoff)
	++emsg_off;
    എങ്കിൽ (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	എങ്കിൽ (!emsgoff && errormsg != ശൂന്യ && *errormsg != NUL)
	    emsg(errormsg);
    എങ്കിൽ (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    സ്ഥിതം ശൂന്യം
f_feedkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    പൂർണ്ണസംഖ്യ		remap = TRUE;
    പൂർണ്ണസംഖ്യ		തിരുകുക = FALSE;
    char_u	*താക്കോലുകൾ, *flags;
    char_u	nbuf[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		typed = FALSE;
    പൂർണ്ണസംഖ്യ		execute = FALSE;
    പൂർണ്ണസംഖ്യ		context = FALSE;
    പൂർണ്ണസംഖ്യ		dangerous = FALSE;
    പൂർണ്ണസംഖ്യ		lowlevel = FALSE;

    // This is not allowed in the sandbox.  If the commands would still be
    // executed in the sandbox it would be OK, but it probably happens later,
    // when "sandbox" is no longer set.
    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    താക്കോലുകൾ = tv_get_string(&argvars[0]);

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	വേണ്ടി ( ; *flags != NUL; ++flags)
	{
	    തിരഞ്ഞെടുപ്പ് (*flags)
	    {
		അവസ്ഥ 'n': remap = FALSE; നിർത്തുക;
		അവസ്ഥ 'm': remap = TRUE; നിർത്തുക;
		അവസ്ഥ 't': typed = TRUE; നിർത്തുക;
		അവസ്ഥ 'i': തിരുകുക = TRUE; നിർത്തുക;
		അവസ്ഥ 'x': execute = TRUE; നിർത്തുക;
		അവസ്ഥ 'c': context = TRUE; നിർത്തുക;
		അവസ്ഥ '!': dangerous = TRUE; നിർത്തുക;
		അവസ്ഥ 'L': lowlevel = TRUE; നിർത്തുക;
	    }
	}
    }

    എങ്കിൽ (*താക്കോലുകൾ != NUL || execute)
    {
	എങ്കിൽ (lowlevel
#ifdef FEAT_VTP
		&& (!is_term_win32()
		    || (താക്കോലുകൾ[0] == 3 && ctrl_c_interrupts && typed))
#endif
	   )
	{
#ifdef USE_നിവേശം_BUF
	    ch_log(ശൂന്യ, "feedkeys() lowlevel: %s", താക്കോലുകൾ);

	    പൂർണ്ണസംഖ്യ len = (പൂർണ്ണസംഖ്യ)STRLEN(താക്കോലുകൾ);
	    വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		എങ്കിൽ (താക്കോലുകൾ[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		add_to_നിവേശം_buf(താക്കോലുകൾ + idx, 1);
	    }
#അല്ലെങ്കിൽ
	    emsg(_(e_lowlevel_നിവേശം_not_supported));
#endif
	}
	അല്ലെങ്കിൽ
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *താക്കോലുകൾ_esc = vim_strsave_escape_csi(താക്കോലുകൾ);
	    എങ്കിൽ (താക്കോലുകൾ_esc == ശൂന്യ)
		തിരികെ;

	    ch_log(ശൂന്യ, "feedkeys(%s): %s", typed ? "typed" : "", താക്കോലുകൾ);

	    ins_typebuf(താക്കോലുകൾ_esc, (remap ? REMAP_YES : REMAP_NONE),
				   തിരുകുക ? 0 : typebuf.tb_len, !typed, FALSE);
	    എങ്കിൽ (vgetc_busy
#ifdef FEAT_TIMERS
		    || timer_busy
#endif
		    || നിവേശം_busy)
		typebuf_was_filled = TRUE;

	    vim_free(താക്കോലുകൾ_esc);
	}

	എങ്കിൽ (execute)
	{
	    പൂർണ്ണസംഖ്യ		save_സന്ദേശം_scroll = സന്ദേശം_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    സന്ദേശം_scroll = FALSE;

	    ch_log(ശൂന്യ, "feedkeys() executing");

	    എങ്കിൽ (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_version = 0;
	    }

	    എങ്കിൽ (!dangerous)
	    {
		++ex_normal_busy;
		++in_feedkeys;
	    }
	    ++allow_osc_താക്കോൽ;
	    exec_normal(TRUE, lowlevel, TRUE);
	    --allow_osc_താക്കോൽ;
	    എങ്കിൽ (!dangerous)
	    {
		--ex_normal_busy;
		--in_feedkeys;
	    }

	    സന്ദേശം_scroll |= save_സന്ദേശം_scroll;

	    എങ്കിൽ (context)
		current_sctx = save_sctx;
	}
    }
}

/*
 * "fnameescape({string})" function
 */
    സ്ഥിതം ശൂന്യം
f_fnameescape(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    സ്ഥിതം ശൂന്യം
f_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
    {
	gui_mch_set_foreground();
	തിരികെ;
    }
#endif
#എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_set_foreground();
#endif
}

/*
 * "function()" function
 * "funcref()" function
 */
    സ്ഥിതം ശൂന്യം
common_function(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ is_funcref)
{
    char_u	*s;
    char_u	*name;
    പൂർണ്ണസംഖ്യ		use_string = FALSE;
    partial_T   *arg_pt = ശൂന്യ;
    char_u	*trans_name = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		is_global = FALSE;
    char_u	*start_bracket = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != ശൂന്യ)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    അല്ലെങ്കിൽ
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    എങ്കിൽ (s == ശൂന്യ)
    {
	semsg(_(e_invalid_argument_str), "NULL");
	തിരികെ;
    }

    എങ്കിൽ ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == ശൂന്യ) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, ശൂന്യ);
	എങ്കിൽ (*name == '<')
	{
	    // generic function
	    start_bracket = name;
	    എങ്കിൽ (skip_generic_func_type_args(&name) == FAIL)
		പോകുക theend;
	}
	എങ്കിൽ (*name != NUL)
	    s = ശൂന്യ;
    }

    എങ്കിൽ (s == ശൂന്യ || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == ശൂന്യ))
	semsg(_(e_invalid_argument_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    അല്ലെങ്കിൽ എങ്കിൽ (trans_name != ശൂന്യ && (is_funcref
			 ? find_func(trans_name, is_global) == ശൂന്യ
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	dict_idx = 0;
	പൂർണ്ണസംഖ്യ	arg_idx = 0;
	list_T	*list = ശൂന്യ;

	എങ്കിൽ (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	അല്ലെങ്കിൽ എങ്കിൽ (trans_name != ശൂന്യ && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	അല്ലെങ്കിൽ
	    name = vim_strsave(s);

	എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    അല്ലെങ്കിൽ
		// function(name, [args])
		arg_idx = 1;
	    എങ്കിൽ (dict_idx > 0)
	    {
		എങ്കിൽ (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    പോകുക theend;
		}
		എങ്കിൽ (argvars[dict_idx].vval.v_dict == ശൂന്യ)
		    dict_idx = 0;
	    }
	    എങ്കിൽ (arg_idx > 0)
	    {
		എങ്കിൽ (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_second_argument_of_function_must_be_list_or_dict));
		    vim_free(name);
		    പോകുക theend;
		}
		list = argvars[arg_idx].vval.v_list;
		എങ്കിൽ (list == ശൂന്യ || list->lv_len == 0)
		    arg_idx = 0;
		അല്ലെങ്കിൽ എങ്കിൽ (list->lv_len > MAX_FUNC_ARGS)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    പോകുക theend;
		}
	    }
	}
	എങ്കിൽ (dict_idx > 0 || arg_idx > 0 || arg_pt != ശൂന്യ || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    എങ്കിൽ (pt == ശൂന്യ)
		vim_free(name);
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (arg_idx > 0 || (arg_pt != ശൂന്യ && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    പൂർണ്ണസംഖ്യ		i = 0;
		    പൂർണ്ണസംഖ്യ		arg_len = 0;
		    പൂർണ്ണസംഖ്യ		lv_len = 0;

		    എങ്കിൽ (arg_pt != ശൂന്യ)
			arg_len = arg_pt->pt_argc;
		    എങ്കിൽ (list != ശൂന്യ)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    എങ്കിൽ (pt->pt_argv == ശൂന്യ)
		    {
			vim_free(pt);
			vim_free(name);
			പോകുക theend;
		    }
		    വേണ്ടി (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    എങ്കിൽ (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_ഇനങ്ങൾ(list, li)
			    copy_tv(&li->li_tv, &pt->pt_argv[i++]);
		    }
		}

		// For "function(dict.func, [], dict)" and "func" is a partial
		// use "dict".  That is backwards compatible.
		എങ്കിൽ (dict_idx > 0)
		{
		    // The dict is bound explicitly, pt_auto is FALSE.
		    pt->pt_dict = argvars[dict_idx].vval.v_dict;
		    ++pt->pt_dict->dv_refcount;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (arg_pt != ശൂന്യ)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    എങ്കിൽ (pt->pt_dict != ശൂന്യ)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    എങ്കിൽ (pt->pt_obj != ശൂന്യ)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		എങ്കിൽ (arg_pt != ശൂന്യ && arg_pt->pt_func != ശൂന്യ)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (is_funcref)
		{
		    pt->pt_func = find_func(trans_name, is_global);
		    എങ്കിൽ (IS_GENERIC_FUNC(pt->pt_func) && start_bracket != ശൂന്യ)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&start_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		അല്ലെങ്കിൽ
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		എങ്കിൽ (arg_pt != ശൂന്യ)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	അല്ലെങ്കിൽ
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    എങ്കിൽ (start_bracket == ശൂന്യ)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    അല്ലെങ്കിൽ
	    {
		// generic function
		size_t len = STRLEN(name) + STRLEN(start_bracket);
		rettv->vval.v_string = alloc(len + 1);
		എങ്കിൽ (rettv->vval.v_string != ശൂന്യ)
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
    സ്ഥിതം ശൂന്യം
f_funcref(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, TRUE);
}

/*
 * "function()" function
 */
    സ്ഥിതം ശൂന്യം
f_function(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, FALSE);
}

/*
 * "garbagecollect()" function
 */
    സ്ഥിതം ശൂന്യം
f_garbagecollect(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	തിരികെ;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
	garbage_collect_at_exit = TRUE;
}

/*
 * "get()" function
 */
    സ്ഥിതം ശൂന്യം
f_get(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    list_T	*l;
    tuple_T	*tuple;
    dictitem_T	*di;
    dict_T	*d;
    typval_T	*tv = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		what_is_dict = FALSE;

    എങ്കിൽ (argvars[0].v_type == VAR_BLOB)
    {
	പൂർണ്ണസംഖ്യ error = FALSE;
	പൂർണ്ണസംഖ്യ idx = tv_get_സംഖ്യ_chk(&argvars[1], &error);

	എങ്കിൽ (!error)
	{
	    rettv->v_type = VAR_സംഖ്യ;
	    എങ്കിൽ (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    എങ്കിൽ (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_സംഖ്യ = -1;
	    അല്ലെങ്കിൽ
	    {
		rettv->vval.v_സംഖ്യ = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_LIST)
    {
	എങ്കിൽ ((l = argvars[0].vval.v_list) != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ		error = FALSE;

	    li = list_find(l, (long)tv_get_സംഖ്യ_chk(&argvars[1], &error));
	    എങ്കിൽ (!error && li != ശൂന്യ)
		tv = &li->li_tv;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_TUPLE)
    {
	എങ്കിൽ ((tuple = argvars[0].vval.v_tuple) != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ		error = FALSE;
	    long	idx;

	    idx = (long)tv_get_സംഖ്യ_chk(&argvars[1], &error);
	    എങ്കിൽ (!error)
		tv = tuple_find(tuple, idx);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_DICT)
    {
	എങ്കിൽ ((d = argvars[0].vval.v_dict) != ശൂന്യ)
	{
	    di = dict_find(d, tv_get_string(&argvars[1]), -1);
	    എങ്കിൽ (di != ശൂന്യ)
		tv = &di->di_tv;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	എങ്കിൽ (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	അല്ലെങ്കിൽ
	{
	    CLEAR_FIELD(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	എങ്കിൽ (pt != ശൂന്യ)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    എങ്കിൽ (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		എങ്കിൽ (name == ശൂന്യ)
		    rettv->vval.v_string = ശൂന്യ;
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    എങ്കിൽ (*what == 'n' && pt->pt_name == ശൂന്യ
							&& pt->pt_func != ശൂന്യ)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		എങ്കിൽ (pt->pt_dict != ശൂന്യ)
		    rettv_dict_set(rettv, pt->pt_dict);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		എങ്കിൽ (rettv_list_alloc(rettv) == OK)
		{
		    പൂർണ്ണസംഖ്യ i;

		    വേണ്ടി (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(what, "arity") == 0)
	    {
		പൂർണ്ണസംഖ്യ required = 0, optional = 0, varargs = FALSE;
		char_u *name = partial_name(pt);

		get_func_arity(name, &required, &optional, &varargs);

		rettv->v_type = VAR_DICT;
		എങ്കിൽ (rettv_dict_alloc(rettv) == OK)
		{
		    dict_T *dict = rettv->vval.v_dict;

		    // Take into account the arguments of the partial, if any.
		    // Note that it is possible to supply more arguments than the function
		    // accepts.
		    എങ്കിൽ (pt->pt_argc >= required + optional)
			required = optional = 0;
		    അല്ലെങ്കിൽ എങ്കിൽ (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    അല്ലെങ്കിൽ
			required -= pt->pt_argc;

		    dict_add_സംഖ്യ(dict, "required", required);
		    dict_add_സംഖ്യ(dict, "optional", optional);
		    dict_add_bool(dict, "varargs", varargs);
		}
	    }
	    അല്ലെങ്കിൽ
		semsg(_(e_invalid_argument_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    എങ്കിൽ (!what_is_dict)
		തിരികെ;
	}
    }
    അല്ലെങ്കിൽ
	semsg(_(e_argument_of_str_must_be_list_tuple_dictionary_or_blob), "get()");

    എങ്കിൽ (tv == ശൂന്യ)
    {
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
f_getbgcolor(typval_T *argvars UNUSED, typval_T *rettv)
{
    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	എങ്കിൽ (gui.in_use)
	    try_hl = true;
# endif
# ifdef FEAT_TERMGUICOLORS
	എങ്കിൽ (p_tgc)
	    try_hl = true;
# endif
	എങ്കിൽ (try_hl)
	{
	    guicolor_T	fg, bg;
	    പൂർണ്ണസംഖ്യ		hl_id = syn_name2id((char_u *)"Normal");

	    എങ്കിൽ (hl_id > 0)
	    {
		(ശൂന്യം)syn_id2colors(hl_id, &fg, &bg);
		എങ്കിൽ (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_സംഖ്യ(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_സംഖ്യ(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_സംഖ്യ(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    തിരികെ;
		}
	    }
	}
    }
#endif
#ifdef FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	എങ്കിൽ (term_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)r);
	    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)g);
	    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#endif
}

/*
 * "getcellpixels()" function
 */
    സ്ഥിതം ശൂന്യം
f_getcellpixels(typval_T *argvars UNUSED, typval_T *rettv)
{
    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

#എങ്കിൽ defined(FEAT_GUI)
    എങ്കിൽ (gui.in_use)
    {
	// success pixel size and no gui.
	list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)gui.char_width);
	list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)gui.char_height);
    }
    അല്ലെങ്കിൽ
#endif
    {
#എങ്കിൽ defined(UNIX) || defined(MSWIN)
	list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)cell_width);
	list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)cell_height);
#അല്ലെങ്കിൽ
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)-1);
	list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)-1);
#endif
    }

}

/*
 * "getchangelist()" function
 */
    സ്ഥിതം ശൂന്യം
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		i;
    list_T	*l;
    dict_T	*d;
    പൂർണ്ണസംഖ്യ		changelistindex;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    അല്ലെങ്കിൽ
	buf = tv_get_buf_from_arg(&argvars[0]);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    l = list_alloc();
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	തിരികെ;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    എങ്കിൽ (buf == curwin->w_buffer)
    {
	changelistindex = curwin->w_changelistidx;
    }
    അല്ലെങ്കിൽ
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    എങ്കിൽ (wip->wi_win == curwin)
		നിർത്തുക;
	changelistindex = wip != ശൂന്യ ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)changelistindex);

    വേണ്ടി (i = 0; i < buf->b_changelistlen; ++i)
    {
	എങ്കിൽ (buf->b_changelist[i].lnum == 0)
	    തുടരുക;
	എങ്കിൽ ((d = dict_alloc()) == ശൂന്യ)
	    തിരികെ;
	എങ്കിൽ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    തിരികെ;
	}
	dict_add_സംഖ്യ(d, "lnum", (long)buf->b_changelist[i].lnum);
	dict_add_സംഖ്യ(d, "col", (long)buf->b_changelist[i].col);
	dict_add_സംഖ്യ(d, "coladd", (long)buf->b_changelist[i].coladd);
    }
}

    സ്ഥിതം ശൂന്യം
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    പൂർണ്ണസംഖ്യ		getcurpos,
    പൂർണ്ണസംഖ്യ		charcol)
{
    pos_T	*fp = ശൂന്യ;
    pos_T	pos;
    win_T	*wp = curwin;
    list_T	*l;
    പൂർണ്ണസംഖ്യ		fnum = -1;

    എങ്കിൽ (rettv_list_alloc(rettv) == OK)
    {
	l = rettv->vval.v_list;
	എങ്കിൽ (getcurpos)
	{
	    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
	    {
		wp = find_win_by_nr_or_id(&argvars[0]);
		എങ്കിൽ (wp != ശൂന്യ)
		    fp = &wp->w_cursor;
	    }
	    അല്ലെങ്കിൽ
		fp = &curwin->w_cursor;
	    എങ്കിൽ (fp != ശൂന്യ && charcol)
	    {
		pos = *fp;
		pos.col =
		    buf_byteidx_to_charidx(wp->w_buffer, pos.lnum, pos.col);
		fp = &pos;
	    }
	}
	അല്ലെങ്കിൽ
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	എങ്കിൽ (fnum != -1)
	    list_append_സംഖ്യ(l, (varnumber_T)fnum);
	അല്ലെങ്കിൽ
	    list_append_സംഖ്യ(l, (varnumber_T)0);
	list_append_സംഖ്യ(l, (fp != ശൂന്യ) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_സംഖ്യ(l, (fp != ശൂന്യ)
		     ? (varnumber_T)(fp->col == MAXCOL ? MAXCOL : fp->col + 1)
							    : (varnumber_T)0);
	list_append_സംഖ്യ(l, (fp != ശൂന്യ) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	എങ്കിൽ (getcurpos)
	{
	    പൂർണ്ണസംഖ്യ	    save_set_curswant = curwin->w_set_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    എങ്കിൽ (wp == curwin)
		update_curswant();
	    list_append_സംഖ്യ(l, wp == ശൂന്യ ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    എങ്കിൽ (wp == curwin && save_set_curswant)
	    {
		curwin->w_set_curswant = save_set_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = FALSE;
}

/*
 * "getcharpos()" function
 */
    സ്ഥിതം ശൂന്യം
f_getcharpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    സ്ഥിതം ശൂന്യം
f_getcharsearch(typval_T *argvars UNUSED, typval_T *rettv)
{
    എങ്കിൽ (rettv_dict_alloc(rettv) != OK)
	തിരികെ;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_string(dict, "char", last_csearch());
    dict_add_സംഖ്യ(dict, "forward", last_csearch_forward());
    dict_add_സംഖ്യ(dict, "until", last_csearch_until());
}

/*
 * "getenv()" function
 */
    സ്ഥിതം ശൂന്യം
f_getenv(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ	    mustfree = FALSE;
    char_u  *p;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    എങ്കിൽ (p == ശൂന്യ)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_സംഖ്യ = VVAL_NULL;
	തിരികെ;
    }
    എങ്കിൽ (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    സ്ഥിതം ശൂന്യം
f_getfontname(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	തിരികെ;

#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
    {
	GuiFont font;
	char_u	*name = ശൂന്യ;

	എങ്കിൽ (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	അല്ലെങ്കിൽ
	{
	    name = tv_get_string(&argvars[0]);
	    എങ്കിൽ (STRCMP(name, "*") == 0)	    // don't use font dialog
		തിരികെ;
	    font = gui_mch_get_font(name, FALSE);
	    എങ്കിൽ (font == NOFONT)
		തിരികെ;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#endif
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
സ്ഥിതം ഘടന
{
    argcheck_T	func;
    അക്ഷരം	*types;	    // separated by commas
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
    {arg_list_സംഖ്യ,		"list<number>"},
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
    {arg_സംഖ്യ,		"number"},
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
    സ്ഥിതം list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    അക്ഷരം	*types = "any";

    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ ശൂന്യ;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(argcheck_types); ++i)
	എങ്കിൽ (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    നിർത്തുക;
	}
    വേണ്ടി (അക്ഷരം *p = types; ; )
    {
	അക്ഷരം	*end = (അക്ഷരം *)vim_strchr((char_u *)p, ',');
	പൂർണ്ണസംഖ്യ	len = end == ശൂന്യ ? (പൂർണ്ണസംഖ്യ)STRLEN(p) : (പൂർണ്ണസംഖ്യ)(end - p);

	എങ്കിൽ (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    തിരികെ ശൂന്യ;
	}
	എങ്കിൽ (end == ശൂന്യ)
	    നിർത്തുക;
	p = end + 1;
    }
    തിരികെ l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
getinfo_argtypes(
	list_T			*l,
	സ്ഥിരം funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ		argcount = 0;

    എങ്കിൽ (l != ശൂന്യ)
	FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    എങ്കിൽ (s == ശൂന്യ)
		തിരികെ -1;
	    എങ്കിൽ (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_ARGS)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		തിരികെ -1;
	    }
	    type = parse_type(&p, type_gap, ശൂന്യ, ശൂന്യ, TRUE);
	    എങ്കിൽ (type == ശൂന്യ)
		തിരികെ -1;
	    എങ്കിൽ (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		തിരികെ -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    വേണ്ടി ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_ARGS; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    തിരികെ argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    സ്ഥിതം അക്ഷരം *
builtin_ret_type_name(
	പൂർണ്ണസംഖ്യ	    idx,
	പൂർണ്ണസംഖ്യ	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	അക്ഷരം	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = internal_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    തിരികെ type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
തരനിർവചനം എണ്ണൽ {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_kind_T;

സ്ഥിതം ഘടന
{
    അക്ഷരം		*name;
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
    സ്ഥിതം ശൂന്യം
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_kind_T	kind;
    പൂർണ്ണസംഖ്യ			ki;
    പൂർണ്ണസംഖ്യ			idx;
    സ്ഥിരം funcentry_T	*fe;
    dict_T		*d;
    list_T		*args;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_ARGS];
    list_T		*argtypes_list = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			argcount;
    പൂർണ്ണസംഖ്യ			vim9 = -1;	// -1: the context of the caller
    അക്ഷരം		*ret_name;
    അക്ഷരം		*tofree;

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;
    എങ്കിൽ (in_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;
    എങ്കിൽ (check_for_opt_dict_arg(argvars, 2) == FAIL)
	തിരികെ;

    kindname = tv_get_string(&argvars[0]);
    വേണ്ടി (ki = 0; ki < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(getinfo_kinds); ++ki)
	എങ്കിൽ (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    നിർത്തുക;
    എങ്കിൽ (ki == (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(getinfo_kinds))
    {
	semsg(_(e_invalid_argument_str), kindname);
	തിരികെ;
    }
    kind = getinfo_kinds[ki].kind;
    name = tv_get_string_buf(&argvars[1], namebuf);

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != ശൂന്യ)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	പൂർണ്ണസംഖ്യ	    todo = (പൂർണ്ണസംഖ്യ)opts->dv_hashtab.ht_used;

	FOR_ALL_HASHTAB_ഇനങ്ങൾ(&opts->dv_hashtab, hi, todo)
	{
	    എങ്കിൽ (HASHITEM_EMPTY(hi))
		തുടരുക;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    എങ്കിൽ (STRCMP(di->di_താക്കോൽ, "argtypes") == 0 && kind == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(di->di_താക്കോൽ, "vim9") == 0 && kind == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    അല്ലെങ്കിൽ
	    {
		semsg(_(e_invalid_argument_str), di->di_താക്കോൽ);
		തിരികെ;
	    }
	}
    }
    എങ്കിൽ (kind == GI_VIMVAR)
    {
	എങ്കിൽ (STRNCMP(name, "v:", 2) == 0)
	    (ശൂന്യം)vim_var_info(name + 2, rettv->vval.v_dict);
	തിരികെ;
    }
    എങ്കിൽ (kind == GI_COMMAND)
    {
	(ശൂന്യം)ex_command_info(name, vim9, rettv->vval.v_dict);
	തിരികെ;
    }
    എങ്കിൽ (kind == GI_OPTION)
    {
	(ശൂന്യം)option_info(name, rettv->vval.v_dict);
	തിരികെ;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = find_internal_func_opt(name, FALSE);
    എങ്കിൽ (idx < 0)
    {
	(ശൂന്യം)user_func_info(name, rettv->vval.v_dict);
	തിരികെ;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, വലുപ്പം(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    എങ്കിൽ (argcount < 0)
    {
	clear_type_list(&type_gap);
	തിരികെ;
    }
    d = rettv->vval.v_dict;
    dict_add_string(d, "name", (char_u *)fe->f_name);
    dict_add_string(d, "kind", (char_u *)"builtin");
    dict_add_bool(d, "available", fe->f_func != ശൂന്യ);
    dict_add_സംഖ്യ(d, "minargs", fe->f_min_argc);
    dict_add_സംഖ്യ(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_add_സംഖ്യ(d, "method", fe->f_argtype & FEARG_MASK);

    args = list_alloc();
    എങ്കിൽ (args == ശൂന്യ || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	clear_type_list(&type_gap);
	തിരികെ;
    }
    argnames = (char_u *)fe->f_argnames;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = ശൂന്യ;
	പൂർണ്ണസംഖ്യ	    argnamelen = 0;

	എങ്കിൽ (fe->f_argcheck == ശൂന്യ)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    എങ്കിൽ (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		നിർത്തുക;
	    check = arg_any;
	}
	അല്ലെങ്കിൽ
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    എങ്കിൽ ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == ശൂന്യ)
		നിർത്തുക;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	എങ്കിൽ (argnames != ശൂന്യ)
	{
	    char_u  *end = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = end == ശൂന്യ ? (പൂർണ്ണസംഖ്യ)STRLEN(argnames)
						     : (പൂർണ്ണസംഖ്യ)(end - argnames);
	    argnames = end == ശൂന്യ ? ശൂന്യ : skipwhite(end + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	എങ്കിൽ (arg == ശൂന്യ || types == ശൂന്യ
		|| dict_add_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    തിരികെ;
	}
	എങ്കിൽ ((argnamelen > 0
		    && dict_add_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    തിരികെ;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    എങ്കിൽ (argtypes_list == ശൂന്യ
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	അക്ഷരം	*tofree_max;
	അക്ഷരം	*max_name;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = argcount; i < fe->f_max_argc && i < MAX_FUNC_ARGS; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	എങ്കിൽ (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = ശൂന്യ;
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
    സ്ഥിതം ശൂന്യം
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		i;
    list_T	*l;
    dict_T	*d;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL)))
	തിരികെ;

    wp = find_tabwin(&argvars[0], &argvars[1], ശൂന്യ);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	തിരികെ;
    }

    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    വേണ്ടി (i = 0; i < wp->w_jumplistlen; ++i)
    {
	എങ്കിൽ (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    തുടരുക;
	എങ്കിൽ ((d = dict_alloc()) == ശൂന്യ)
	    തിരികെ;
	എങ്കിൽ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    തിരികെ;
	}
	dict_add_സംഖ്യ(d, "lnum", (long)wp->w_jumplist[i].fmark.mark.lnum);
	dict_add_സംഖ്യ(d, "col", (long)wp->w_jumplist[i].fmark.mark.col);
	dict_add_സംഖ്യ(d, "coladd", (long)wp->w_jumplist[i].fmark.mark.coladd);
	dict_add_സംഖ്യ(d, "bufnr", (long)wp->w_jumplist[i].fmark.fnum);
	എങ്കിൽ (wp->w_jumplist[i].fname != ശൂന്യ)
	    dict_add_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    സ്ഥിതം ശൂന്യം
f_getpid(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    സ്ഥിതം ശൂന്യം
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    സ്ഥിതം ശൂന്യം
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    സ്ഥിതം ശൂന്യം
f_getpos(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
block_def2str(ഘടന block_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    എങ്കിൽ (ret->string == ശൂന്യ)
    {
	ret->length = 0;
	തിരികെ FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->length = bd->startspaces;
    mch_memmove(ret->string + ret->length, bd->textstart, bd->textlen);
    ret->length += bd->textlen;
    vim_memset(ret->string + ret->length, ' ', bd->endspaces);
    ret->length += bd->endspaces;
    ret->string[ret->length] = NUL;

    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
getregionpos(
    typval_T	*argvars,
    typval_T	*rettv,
    pos_T	*p1,
    pos_T	*p2,
    പൂർണ്ണസംഖ്യ		*inclusive,
    പൂർണ്ണസംഖ്യ		*region_type,
    oparg_T	*oap)
{
    പൂർണ്ണസംഖ്യ		fnum1 = -1, fnum2 = -1;
    char_u	*type;
    buf_T	*findbuf;
    char_u	default_type[] = "v";
    പൂർണ്ണസംഖ്യ		block_width = 0;
    പൂർണ്ണസംഖ്യ		is_select_exclusive;
    പൂർണ്ണസംഖ്യ		l;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (list2fpos(&argvars[0], p1, &fnum1, ശൂന്യ, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, ശൂന്യ, FALSE) != OK
	    || fnum1 != fnum2)
	തിരികെ FAIL;

    എങ്കിൽ (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	എങ്കിൽ (type == ശൂന്യ)
	    type = default_type;
    }
    അല്ലെങ്കിൽ
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    എങ്കിൽ (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    അല്ലെങ്കിൽ എങ്കിൽ (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    അല്ലെങ്കിൽ എങ്കിൽ (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	എങ്കിൽ (*p != NUL && ((block_width = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "type", type);
	    തിരികെ FAIL;
	}
	*region_type = MBLOCK;
    }
    അല്ലെങ്കിൽ
    {
	semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "type", type);
	തിരികെ FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    എങ്കിൽ (findbuf == ശൂന്യ || findbuf->b_ml.ml_mfp == ശൂന്യ)
    {
	emsg(_(e_buffer_is_not_loaded));
	തിരികെ FAIL;
    }

    എങ്കിൽ (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_വരി_എണ്ണം)
    {
	semsg(_(e_invalid_വരി_സംഖ്യ_nr), p1->lnum);
	തിരികെ FAIL;
    }
    എങ്കിൽ (p1->col == MAXCOL)
	p1->col = ml_get_buf_len(findbuf, p1->lnum) + 1;
    അല്ലെങ്കിൽ എങ്കിൽ (p1->col < 1 || p1->col > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_column_സംഖ്യ_nr), p1->col);
	തിരികെ FAIL;
    }

    എങ്കിൽ (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_വരി_എണ്ണം)
    {
	semsg(_(e_invalid_വരി_സംഖ്യ_nr), p2->lnum);
	തിരികെ FAIL;
    }
    എങ്കിൽ (p2->col == MAXCOL)
	p2->col = ml_get_buf_len(findbuf, p2->lnum) + 1;
    അല്ലെങ്കിൽ എങ്കിൽ (p2->col < 1 || p2->col > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_column_സംഖ്യ_nr), p2->col);
	തിരികെ FAIL;
    }

    curbuf = findbuf;
    curwin->w_buffer = curbuf;
    virtual_op = virtual_active();

    // NOTE: Adjustment is needed.
    p1->col--;
    p2->col--;

    എങ്കിൽ (!LT_POS(*p1, *p2))
    {
	// swap position
	pos_T p;

	p = *p1;
	*p1 = *p2;
	*p2 = p;
    }

    എങ്കിൽ (*region_type == MCHAR)
    {
	// Handle 'selection' == "exclusive".
	എങ്കിൽ (is_select_exclusive && !EQUAL_POS(*p1, *p2))
	    // When backing up to previous line, inclusive becomes false.
	    *inclusive = !unadjust_for_sel_inner(p2);
	// If p2 is on NUL (end of line), inclusive becomes false.
	എങ്കിൽ (*inclusive && !virtual_op && *ml_get_pos(p2) == NUL)
	    *inclusive = FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#ifdef FEAT_LINEBREAK
	പൂർണ്ണസംഖ്യ	lbr_saved = reset_lbr();
#endif

	getvvcol(curwin, p1, &sc1, ശൂന്യ, &ec1, 0);
	getvvcol(curwin, p2, &sc2, ശൂന്യ, &ec2, 0);

#ifdef FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#endif

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->start = *p1;
	oap->end = *p2;
	oap->start_vcol = MIN(sc1, sc2);
	എങ്കിൽ (block_width > 0)
	    oap->end_vcol = oap->start_vcol + block_width - 1;
	അല്ലെങ്കിൽ എങ്കിൽ (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->end_vcol = sc2 - 1;
	അല്ലെങ്കിൽ
	    oap->end_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    എങ്കിൽ (l > 1)
	p2->col += l - 1;

    തിരികെ OK;
}

/*
 * "getregion()" function
 */
    സ്ഥിതം ശൂന്യം
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    പൂർണ്ണസംഖ്യ			inclusive = TRUE;
    പൂർണ്ണസംഖ്യ			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    പൂർണ്ണസംഖ്യ			save_virtual;
    string_T		akt = {ശൂന്യ, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    എങ്കിൽ (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	തിരികെ;

    വേണ്ടി (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	പൂർണ്ണസംഖ്യ ret = OK;

	എങ്കിൽ (region_type == MBLOCK)
	{
	    ഘടന block_def	bd;

	    block_prep(&oa, &bd, lnum, FALSE);
	    block_def2str(&bd, &akt);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.length = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.length);
	    എങ്കിൽ (akt.string == ശൂന്യ)
		akt.length = 0;
	    അല്ലെങ്കിൽ
		akt.length = s.length;
	}
	അല്ലെങ്കിൽ
	{
	    ഘടന block_def	bd;

	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);
	    block_def2str(&bd, &akt);
	}

	എങ്കിൽ (akt.string == ശൂന്യ)
	{
	    clear_tv(rettv);
	    (ശൂന്യം)rettv_list_alloc(rettv);
	    നിർത്തുക;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (പൂർണ്ണസംഖ്യ)akt.length);
	vim_free(akt.string);

	എങ്കിൽ (ret == FAIL)
	{
	    clear_tv(rettv);
	    (ശൂന്യം)rettv_list_alloc(rettv);
	    നിർത്തുക;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_buffer = curbuf;
    virtual_op = save_virtual;
}

    സ്ഥിതം ശൂന്യം
add_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    എങ്കിൽ (l1 == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	തിരികെ;
    }

    l2 = list_alloc();
    എങ്കിൽ (l2 == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	തിരികെ;
    }

    l3 = list_alloc();
    എങ്കിൽ (l3 == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	തിരികെ;
    }

    list_append_സംഖ്യ(l2, curbuf->b_fnum);
    list_append_സംഖ്യ(l2, p1.lnum);
    list_append_സംഖ്യ(l2, p1.col);
    list_append_സംഖ്യ(l2, p1.coladd);

    list_append_സംഖ്യ(l3, curbuf->b_fnum);
    list_append_സംഖ്യ(l3, p2.lnum);
    list_append_സംഖ്യ(l3, p2.col);
    list_append_സംഖ്യ(l3, p2.coladd);
}

/*
 * Compute the positions of the region segment on line "lnum".
 * "ret_p1" is set to the start position of the segment and "ret_p2" to its
 * end position.
 */
    സ്ഥിതം ശൂന്യം
getregionpos_വരി(
    linenr_T	lnum,
    pos_T	p1,
    pos_T	p2,
    പൂർണ്ണസംഖ്യ		inclusive,
    പൂർണ്ണസംഖ്യ		region_type,
    oparg_T	*oap,
    പൂർണ്ണസംഖ്യ		allow_eol,
    pos_T	*ret_p1,
    pos_T	*ret_p2)
{
    char_u	*വരി = ml_get(lnum);
    colnr_T	വരി_len = ml_get_len(lnum);

    എങ്കിൽ (region_type == MLINE)
    {
	ret_p1->col = 1;
	ret_p1->coladd = 0;
	ret_p2->col = MAXCOL;
	ret_p2->coladd = 0;
    }
    അല്ലെങ്കിൽ
    {
	ഘടന block_def	bd;

	എങ്കിൽ (region_type == MBLOCK)
	    block_prep(oap, &bd, lnum, FALSE);
	അല്ലെങ്കിൽ
	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);

	എങ്കിൽ (bd.is_oneChar)  // selection entirely inside one char
	{
	    എങ്കിൽ (region_type == MBLOCK)
	    {
		ret_p1->col = mb_prevptr(വരി, bd.textstart) - വരി + 1;
		ret_p1->coladd = bd.start_char_vcols
					   - (bd.start_vcol - oap->start_vcol);
	    }
	    അല്ലെങ്കിൽ
	    {
		ret_p1->col = p1.col + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (region_type == MBLOCK && oap->start_vcol > bd.start_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->col = MAXCOL;
	    ret_p1->coladd = oap->start_vcol - bd.start_vcol;
	    bd.is_oneChar = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (bd.startspaces > 0)
	{
	    ret_p1->col = mb_prevptr(വരി, bd.textstart) - വരി + 1;
	    ret_p1->coladd = bd.start_char_vcols - bd.startspaces;
	}
	അല്ലെങ്കിൽ
	{
	    ret_p1->col = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	എങ്കിൽ (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->col = ret_p1->col;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (bd.endspaces > 0)
	{
	    ret_p2->col = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	അല്ലെങ്കിൽ
	{
	    ret_p2->col = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    എങ്കിൽ (!allow_eol && ret_p1->col > വരി_len)
    {
	ret_p1->col = 0;
	ret_p1->coladd = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ret_p1->col > വരി_len + 1)
	ret_p1->col = വരി_len + 1;

    എങ്കിൽ (!allow_eol && ret_p2->col > വരി_len)
    {
	ret_p2->col = ret_p1->col == 0 ? 0 : വരി_len;
	ret_p2->coladd = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ret_p2->col > വരി_len + 1)
	ret_p2->col = വരി_len + 1;

    ret_p1->lnum = lnum;
    ret_p2->lnum = lnum;
}

/*
 * "getregionpos()" function
 */
    സ്ഥിതം ശൂന്യം
f_getregionpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	p1, p2;
    പൂർണ്ണസംഖ്യ		inclusive = TRUE;
    പൂർണ്ണസംഖ്യ		region_type = -1;
    പൂർണ്ണസംഖ്യ		allow_eol = FALSE;
    പൂർണ്ണസംഖ്യ		bounds_only = FALSE;
    oparg_T	oa;
    പൂർണ്ണസംഖ്യ		lnum;

    buf_T	*save_curbuf;
    പൂർണ്ണസംഖ്യ		save_virtual;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    എങ്കിൽ (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    എങ്കിൽ (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	start_pos, end_pos;

	getregionpos_വരി(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &start_pos, &end_pos);
	എങ്കിൽ (p2.lnum != p1.lnum)
	{
	    pos_T	unused;

	    getregionpos_വരി(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &unused, &end_pos);
	}
	add_regionpos_range(rettv, start_pos, end_pos);
    }
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_വരി(lnum, p1, p2, inclusive, region_type, &oa,
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	എങ്കിൽ (strregname != ശൂന്യ && in_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_register_name_must_be_one_char_str), strregname);
	    strregname = ശൂന്യ;
	}
	എങ്കിൽ (strregname == ശൂന്യ)	    // type error; errmsg already given
	    തിരികെ 0;
    }
    അല്ലെങ്കിൽ
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    തിരികെ *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    സ്ഥിതം ശൂന്യം
f_getreg(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		regname;
    പൂർണ്ണസംഖ്യ		arg2 = FALSE;
    പൂർണ്ണസംഖ്യ		return_list = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	തിരികെ;

    regname = getreg_get_regname(argvars);
    എങ്കിൽ (regname == 0)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	പൂർണ്ണസംഖ്യ		error = FALSE;

	arg2 = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[1], &error);

	എങ്കിൽ (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[2], &error);
	എങ്കിൽ (error)
	    തിരികെ;
    }

    എങ്കിൽ (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_വ്യഞ്ജകം_SRC : 0) | GREG_LIST);
	എങ്കിൽ (rettv->vval.v_list == ശൂന്യ)
	    (ശൂന്യം)rettv_list_alloc(rettv);
	അല്ലെങ്കിൽ
	    ++rettv->vval.v_list->lv_refcount;
    }
    അല്ലെങ്കിൽ
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = get_reg_contents(regname,
						    arg2 ? GREG_വ്യഞ്ജകം_SRC : 0);
    }
}

/*
 * "getregtype()" function
 */
    സ്ഥിതം ശൂന്യം
f_getregtype(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		regname;
    char_u	buf[NUMBUFLEN + 2];
    long	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    regname = getreg_get_regname(argvars);
    എങ്കിൽ (regname == 0)
	തിരികെ;

    buf[0] = NUL;
    buf[1] = NUL;
    തിരഞ്ഞെടുപ്പ് (get_reg_type(regname, &reglen))
    {
	അവസ്ഥ MLINE: buf[0] = 'V'; നിർത്തുക;
	അവസ്ഥ MCHAR: buf[0] = 'v'; നിർത്തുക;
	അവസ്ഥ MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((അക്ഷരം *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		നിർത്തുക;
    }
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "gettagstack()" function
 */
    സ്ഥിതം ശൂന്യം
f_gettagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;			// default is current window

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	എങ്കിൽ (wp == ശൂന്യ)
	    തിരികെ;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    സ്ഥിതം ശൂന്യം
f_gettext(typval_T *argvars, typval_T *rettv)
{
#എങ്കിൽ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    അക്ഷരം *prev = ശൂന്യ;
#endif

    എങ്കിൽ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_STRING;

    എങ്കിൽ (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != ശൂന്യ &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#എങ്കിൽ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((സ്ഥിരം അക്ഷരം *)argvars[1].vval.v_string, (അക്ഷരം *)p_enc);
#endif

#എങ്കിൽ defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((സ്ഥിരം അക്ഷരം *)argvars[1].vval.v_string, (സ്ഥിരം അക്ഷരം *)argvars[0].vval.v_string));
#അല്ലെങ്കിൽ
	textdomain((സ്ഥിരം അക്ഷരം *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#endif

#എങ്കിൽ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	എങ്കിൽ (prev != ശൂന്യ)
	    bind_textdomain_codeset((സ്ഥിരം അക്ഷരം *)argvars[1].vval.v_string, prev);
#endif
    }
    അല്ലെങ്കിൽ
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
}

// for VIM_VERSION_ defines
#അന്തര്ഭാവ "version.h"

/*
 * "has()" function
 */
    ശൂന്യം
f_has(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*name;
    പൂർണ്ണസംഖ്യ		x = FALSE;
    പൂർണ്ണസംഖ്യ		n = FALSE;
    തരനിർവചനം ഘടന {
	അക്ഷരം *name;
	short present;
    } has_item_T;
    സ്ഥിതം has_item_T has_list[] =
    {
	{"amiga",
#ifdef AMIGA
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"android",
#ifdef __ANDROID__
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"arp",
#എങ്കിൽ defined(AMIGA) && defined(FEAT_ARP)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"haiku",
#ifdef __HAIKU__
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"bsd",
#എങ്കിൽ defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"hpux",
#ifdef hpux
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"hurd",
#ifdef __GNU__
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"linux",
#ifdef __linux__
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#ifdef MACOS_X
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"osx",		// Mac OS X
#ifdef MACOS_X
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"macunix",	// Mac OS X, with the darwin feature
#എങ്കിൽ defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"osxdarwin",	// synonym for macunix
#എങ്കിൽ defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"qnx",
#ifdef __QNX__
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"sun",
#ifdef SUN_SYSTEM
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"termux",
#ifdef __TERMUX__
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"unix",
#ifdef UNIX
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"vms",
#ifdef VMS
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"win32",
#ifdef MSWIN
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"win32unix",
#ifdef WIN32UNIX
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"win64",
#ifdef _WIN64
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"ebcdic", 0 },
	{"fname_case",
#ifndef CASE_INSENSITIVE_FILENAME
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"acl",
#ifdef HAVE_ACL
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"arabic",
#ifdef FEAT_ARABIC
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"autocmd", 1},
	{"autochdir",
#ifdef FEAT_AUTOCHDIR
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"autoservername",
#ifdef FEAT_AUTOSERVERNAME
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"socketserver",
#ifdef FEAT_SOCKETSERVER
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"balloon_eval",
#ifdef FEAT_BEVAL_GUI
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"balloon_multiline",
#ifdef FEAT_BEVAL_GUI
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"balloon_eval_term",
#ifdef FEAT_BEVAL_TERM
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#എങ്കിൽ defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"byte_offset",
#ifdef FEAT_BYTEOFF
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"channel",
#ifdef FEAT_JOB_CHANNEL
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"cindent", 1},
	{"clientserver",
#ifdef FEAT_CLIENTSERVER
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"clipboard",
#ifdef FEAT_CLIPBOARD
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"clipboard_provider",
#ifdef FEAT_CLIPBOARD_PROVIDER
		1
#അല്ലെങ്കിൽ
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
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"cryptv",
#ifdef FEAT_CRYPT
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"crypt-blowfish",
#ifdef FEAT_CRYPT
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"crypt-blowfish2",
#ifdef FEAT_CRYPT
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"cscope",
#ifdef FEAT_CSCOPE
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"cursorbind", 1},
	{"cursorshape",
#ifdef CURSOR_SHAPE
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"debug",
#ifdef DEBUG
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"dialog_con",
#ifdef FEAT_CON_DIALOG
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"dialog_con_gui",
#എങ്കിൽ defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"dialog_gui",
#ifdef FEAT_GUI_DIALOG
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"diff",
#ifdef FEAT_DIFF
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"digraphs",
#ifdef FEAT_DIGRAPHS
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"directx",
#ifdef FEAT_DIRECTX
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"dnd",
#ifdef FEAT_DND
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"drop_file",
#ifdef HAVE_DROP_FILE
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"emacs_tags",
#ifdef FEAT_EMACS_TAGS
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#ifdef FEAT_തിരയൽ_EXTRA
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"file_in_path", 1},
	{"filterpipe",
#എങ്കിൽ defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"find_in_path",
#ifdef FEAT_FIND_ID
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"float", 1},
	{"folding",
#ifdef FEAT_FOLDING
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"footer", 0},
	{"fork",
#എങ്കിൽ !defined(USE_SYSTEM) && defined(UNIX)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"gettext",
#ifdef FEAT_GETTEXT
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"gui",
#ifdef FEAT_GUI
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#ifdef FEAT_GUI_GTK
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"gui_gtk2",
#എങ്കിൽ defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"gui_gtk3",
#എങ്കിൽ defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"gui_gtk4",
#എങ്കിൽ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"gui_gnome",
#ifdef FEAT_GUI_GNOME
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"gui_haiku",
#ifdef FEAT_GUI_HAIKU
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"gui_mac", 0},
	{"gui_motif",
#ifdef FEAT_GUI_MOTIF
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"gui_photon",
#ifdef FEAT_GUI_PHOTON
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"gui_win32",
#ifdef FEAT_GUI_MSWIN
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"iconv",
#എങ്കിൽ defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"image",
#ifdef FEAT_IMAGE
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"image_popup",
#ifdef FEAT_IMAGE_POPUP
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"insert_expand", 1},
	{"ipv6",
#ifdef FEAT_IPV6
		1
#അല്ലെങ്കിൽ
		0
#endif
	},
	{"job",
#ifdef FEAT_JOB_CHANNEL
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"jumplist", 1},
	{"keymap",
#ifdef FEAT_KEYMAP
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#ifdef FEAT_LANGMAP
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"libcall",
#ifdef FEAT_LIBCALL
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"linebreak",
#ifdef FEAT_LINEBREAK
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#എങ്കിൽ defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"menu",
#ifdef FEAT_MENU
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"mksession",
#ifdef FEAT_SESSION
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#ifdef FEAT_MOUSESHAPE
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"mouse_dec",
#എങ്കിൽ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"mouse_gpm",
#എങ്കിൽ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"mouse_jsbterm",
#എങ്കിൽ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"mouse_netterm",
#എങ്കിൽ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"mouse_pterm",
#എങ്കിൽ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"mouse_sgr",
#എങ്കിൽ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"mouse_sysmouse",
#എങ്കിൽ (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"mouse_urxvt",
#എങ്കിൽ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"mouse_xterm",
#എങ്കിൽ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#ifdef FEAT_MBYTE_IME
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"multi_lang",
#ifdef FEAT_MULTI_LANG
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"mzscheme",
#എങ്കിൽ defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"nanotime",
#ifdef ST_MTIM_NSEC
		1
#അല്ലെങ്കിൽ
		0
#endif
	},
	{"num64", 1},
	{"ole",
#ifdef FEAT_OLE
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"packages",
#ifdef FEAT_EVAL
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"path_extra", 1},
	{"perl",
#എങ്കിൽ defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"persistent_undo",
#ifdef FEAT_PERSISTENT_UNDO
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"python_compiled",
#എങ്കിൽ defined(FEAT_PYTHON)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"python_dynamic",
#എങ്കിൽ defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"python",
#എങ്കിൽ defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"pythonx",
#എങ്കിൽ (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"python3_compiled",
#എങ്കിൽ defined(FEAT_PYTHON3)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"python3_dynamic",
#എങ്കിൽ defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"python3_stable",
#എങ്കിൽ defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"python3",
#എങ്കിൽ defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"popupwin",
#ifdef FEAT_PROP_POPUP
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"postscript",
#ifdef FEAT_POSTSCRIPT
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"pango",
#ifdef FEAT_PRINT_PANGO
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"printer",
#ifdef FEAT_PRINTER
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"profile",
#ifdef FEAT_PROFILE
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"prof_nsec",
#ifdef PROF_NSEC
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"reltime",
#ifdef FEAT_RELTIME
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"quickfix",
#ifdef FEAT_QUICKFIX
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"rightleft",
#ifdef FEAT_RIGHTLEFT
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"ruby",
#എങ്കിൽ defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#ifdef FEAT_SIGNS
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"smartindent", 1},
	{"startuptime",
#ifdef STARTUPTIME
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"statusline",
#ifdef FEAT_STL_OPT
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"statusline_click",
#ifdef FEAT_STL_OPT
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"netbeans_intg",
#ifdef FEAT_NETBEANS_INTG
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"sodium",
#എങ്കിൽ defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"sound",
#ifdef FEAT_SOUND
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"spell",
#ifdef FEAT_SPELL
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"syntax",
#ifdef FEAT_SYN_HL
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"system",
#എങ്കിൽ defined(USE_SYSTEM) || !defined(UNIX)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"tabpanel",
#എങ്കിൽ defined(FEAT_TABPANEL)
		1,
#അല്ലെങ്കിൽ
		0,
#endif
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#എങ്കിൽ defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"termguicolors",
#ifdef FEAT_TERMGUICOLORS
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"terminal",
#എങ്കിൽ defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"terminfo",
#ifdef TERMINFO
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"termresponse",
#ifdef FEAT_TERMRESPONSE
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"textobjects", 1},
	{"textprop",
#ifdef FEAT_PROP_POPUP
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"tgetent",
#ifdef HAVE_TGETENT
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"timers",
#ifdef FEAT_TIMERS
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"title", 1},
	{"toolbar",
#ifdef FEAT_TOOLBAR
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#ifdef FEAT_VARTABS
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"vertsplit", 1},
	{"viminfo",
#ifdef FEAT_VIMINFO
		1
#അല്ലെങ്കിൽ
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
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"wayland",
#ifdef FEAT_WAYLAND
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"wayland_clipboard",
#ifdef FEAT_WAYLAND_CLIPBOARD
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#ifdef FEAT_WAK
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"writebackup",
#ifdef FEAT_WRITEBACKUP
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"xattr",
#ifdef FEAT_XATTR
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"xim",
#ifdef FEAT_XIM
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"xfontset",
#ifdef FEAT_XFONTSET
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"xpm",
#എങ്കിൽ defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"xpm_w32",	// for backward compatibility
#ifdef FEAT_XPM_W32
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"xsmp",
#ifdef USE_XSMP
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"xsmp_interact",
#ifdef USE_XSMP_INTERACT
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"xterm_clipboard",
#ifdef FEAT_XCLIPBOARD
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"xterm_save",
#ifdef FEAT_XTERM_SAVE
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{"X11",
#എങ്കിൽ defined(UNIX) && defined(FEAT_X11)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#എങ്കിൽ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#അല്ലെങ്കിൽ
		0
#endif
		},
	{ശൂന്യ, 0}
    };

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	തിരികെ;

    name = tv_get_string(&argvars[0]);

    // Fast-path: check features not in has_list[] first to avoid the full
    // linear scan for very common queries like has('patch-...').
    എങ്കിൽ (STRNICMP(name, "patch", 5) == 0)
    {
	x = TRUE;
	എങ്കിൽ (name[5] == '-'
		&& STRLEN(name) >= 11
		&& (name[6] >= '1' && name[6] <= '9'))
	{
	    അക്ഷരം	*end;
	    പൂർണ്ണസംഖ്യ	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (പൂർണ്ണസംഖ്യ)strtoul((അക്ഷരം *)name + 6, &end, 10);
	    എങ്കിൽ (*end == '.' && vim_isdigit(end[1])
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
	അല്ലെങ്കിൽ എങ്കിൽ (SAFE_isdigit(name[5]))
	    n = has_patch(atoi((അക്ഷരം *)name + 5));
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_നിവേശം_isatty();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#ifdef FEAT_GUI
	n = (gui.in_use || gui.starting);
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#എങ്കിൽ defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.in_use;	// gui_mch_browse() works when GUI is running
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#ifdef FEAT_SYN_HL
	n = syntax_present(curwin);
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#ifdef FEAT_VTP
	n = is_term_win32() && has_vtp_working();
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_NETBEANS_INTG
	n = netbeans_active();
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_MOUSE_GPM
	n = gpm_enabled();
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#എങ്കിൽ defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_star.available;
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# എങ്കിൽ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	എങ്കിൽ (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	അല്ലെങ്കിൽ
	    n = FALSE;
# അല്ലെങ്കിൽ
	n = FALSE;
# endif
#endif
    }

    // Look up in has_list[] only if not already handled above.
    എങ്കിൽ (x == FALSE)
    {
	വേണ്ടി (i = 0; has_list[i].name != ശൂന്യ; ++i)
	    എങ്കിൽ (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		നിർത്തുക;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	എങ്കിൽ (x == TRUE && n == FALSE)
	{
	    എങ്കിൽ (0)
	    {
		// intentionally empty
	    }
#ifdef VIMDLL
	    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "filterpipe") == 0)
		n = gui.in_use || gui.starting;
#endif
#എങ്കിൽ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#endif
#ifdef DYNAMIC_LUA
	    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#endif
#ifdef DYNAMIC_MZSCHEME
	    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#endif
#ifdef DYNAMIC_PERL
	    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON
	    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON3
	    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#endif
#എങ്കിൽ defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "pythonx") == 0)
	    {
# എങ്കിൽ defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		എങ്കിൽ (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		അല്ലെങ്കിൽ എങ്കിൽ (p_pyx == 3)
		    n = python3_enabled(FALSE);
		അല്ലെങ്കിൽ എങ്കിൽ (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#endif
#ifdef DYNAMIC_RUBY
	    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#endif
#ifdef DYNAMIC_TCL
	    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#endif
#ifdef DYNAMIC_SODIUM
	    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#endif
#എങ്കിൽ defined(FEAT_TERMINAL) && defined(MSWIN)
	    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#endif
#ifdef DYNAMIC_GPM
	    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_available();
#endif
	}
    }

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_സംഖ്യ = x;
    അല്ലെങ്കിൽ
	// return whether feature is enabled
	rettv->vval.v_സംഖ്യ = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    പൂർണ്ണസംഖ്യ
dynamic_feature(char_u *feature)
{
    തിരികെ (feature == ശൂന്യ
#എങ്കിൽ defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.in_use)
#endif
#ifdef VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#endif
#എങ്കിൽ defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.in_use)
#endif
#എങ്കിൽ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
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
#എങ്കിൽ defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
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
#എങ്കിൽ defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#endif
	    );
}

/*
 * "haslocaldir()" function
 */
    സ്ഥിതം ശൂന്യം
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = ശൂന്യ;
    win_T	*wp = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL)))
	തിരികെ;

    wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    എങ്കിൽ (wp != ശൂന്യ && wp->w_localdir != ശൂന്യ)
	rettv->vval.v_സംഖ്യ = 1;
    അല്ലെങ്കിൽ എങ്കിൽ (tp != ശൂന്യ && tp->tp_localdir != ശൂന്യ)
	rettv->vval.v_സംഖ്യ = 2;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = 0;
}

/*
 * "highlightID(name)" function
 */
    സ്ഥിതം ശൂന്യം
f_hlID(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    സ്ഥിതം ശൂന്യം
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    സ്ഥിതം ശൂന്യം
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
# എങ്കിൽ defined(HAVE_NO_LONG_LONG)
തരനിർവചനം unsigned പൂർണ്ണസംഖ്യ uintptr_t;
# അല്ലെങ്കിൽ
തരനിർവചനം unsigned long long uintptr_t;
# endif
#endif // VMS

    സ്ഥിതം ശൂന്യം
f_id(typval_T *argvars, typval_T *rettv)
{
    അക്ഷരം    numbuf[NUMBUFLEN];
    അക്ഷരം    *p = numbuf;

    തിരഞ്ഞെടുപ്പ് (argvars[0].v_type)
    {
	അവസ്ഥ VAR_LIST:
	അവസ്ഥ VAR_TUPLE:
	അവസ്ഥ VAR_DICT:
	അവസ്ഥ VAR_OBJECT:
	അവസ്ഥ VAR_JOB:
	അവസ്ഥ VAR_CHANNEL:
	അവസ്ഥ VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    എങ്കിൽ (argvars[0].vval.v_object != ശൂന്യ)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(ശൂന്യം *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		വേണ്ടി (; p < numbuf + വലുപ്പം(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    സ്ഥിതം ശൂന്യം
index_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    പൂർണ്ണസംഖ്യ		start = 0;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    പൂർണ്ണസംഖ്യ		ic = FALSE;

    b = argvars[0].vval.v_blob;
    എങ്കിൽ (b == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	start = tv_get_സംഖ്യ_chk(&argvars[2], &error);
	എങ്കിൽ (error)
	    തിരികെ;
    }

    എങ്കിൽ (start < 0)
    {
	start = blob_len(b) + start;
	എങ്കിൽ (start < 0)
	    start = 0;
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ idx = start; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_സംഖ്യ;
	tv.vval.v_സംഖ്യ = blob_get(b, idx);
	എങ്കിൽ (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_സംഖ്യ = idx;
	    തിരികെ;
	}
    }
}

/*
 * index() function for a tuple
 */
    സ്ഥിതം ശൂന്യം
index_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    പൂർണ്ണസംഖ്യ		ic = FALSE;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    എങ്കിൽ (tuple == ശൂന്യ)
	തിരികെ;

    പൂർണ്ണസംഖ്യ	start_idx = 0;
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	start_idx = tv_get_സംഖ്യ_chk(&argvars[2], &error);
	എങ്കിൽ (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[3], &error);
	എങ്കിൽ (error)
	    തിരികെ;
    }

    rettv->vval.v_സംഖ്യ = index_tuple(tuple, &argvars[1], start_idx, ic);
}

/*
 * index() function for a list
 */
    സ്ഥിതം ശൂന്യം
index_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*item;
    long	idx = 0;
    പൂർണ്ണസംഖ്യ		ic = FALSE;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    l = argvars[0].vval.v_list;
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ;

    CHECK_LIST_MATERIALIZE(l);
    item = l->lv_first;
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, (long)tv_get_സംഖ്യ_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	എങ്കിൽ (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[3], &error);
	എങ്കിൽ (error)
	    item = ശൂന്യ;
    }

    വേണ്ടി ( ; item != ശൂന്യ; item = item->li_next, ++idx)
	എങ്കിൽ (tv_equal(&item->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_സംഖ്യ = idx;
	    നിർത്തുക;
	}
}

/*
 * "index()" function
 */
    സ്ഥിതം ശൂന്യം
f_index(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_സംഖ്യ_arg(argvars, 1) == FAIL)
		|| check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_BLOB)
	index_func_blob(argvars, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_TUPLE)
	index_func_tuple(argvars, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_LIST)
	index_func_list(argvars, rettv);
    അല്ലെങ്കിൽ
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    പൂർണ്ണസംഖ്യ
indexof_eval_വ്യഞ്ജകം(typval_T *വ്യഞ്ജകം)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	found;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_താക്കോൽ);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    എങ്കിൽ (eval_വ്യഞ്ജകം_typval(വ്യഞ്ജകം, FALSE, argv, 2, ശൂന്യ, &newtv) == FAIL)
	തിരികെ FALSE;

    found = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    തിരികെ error ? FALSE : found;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
indexof_blob(blob_T *b, long startidx, typval_T *വ്യഞ്ജകം)
{
    long	idx = 0;

    എങ്കിൽ (b == ശൂന്യ)
	തിരികെ -1;

    എങ്കിൽ (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	എങ്കിൽ (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_താക്കോൽ, VAR_സംഖ്യ);
    set_vim_var_type(VV_VAL, VAR_സംഖ്യ);

    പൂർണ്ണസംഖ്യ		called_emsg_start = called_emsg;
    വേണ്ടി (idx = startidx; idx < blob_len(b); ++idx)
    {
	set_vim_var_nr(VV_താക്കോൽ, idx);
	set_vim_var_nr(VV_VAL, blob_get(b, idx));

	എങ്കിൽ (indexof_eval_വ്യഞ്ജകം(വ്യഞ്ജകം))
	    തിരികെ idx;

	എങ്കിൽ (called_emsg != called_emsg_start)
	    തിരികെ -1;
    }

    തിരികെ -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
indexof_list(list_T *l, long startidx, typval_T *വ്യഞ്ജകം)
{
    listitem_T	*item;
    long	idx = 0;
    പൂർണ്ണസംഖ്യ		found;

    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ -1;

    CHECK_LIST_MATERIALIZE(l);

    എങ്കിൽ (startidx == 0)
	item = l->lv_first;
    അല്ലെങ്കിൽ
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, startidx);
	എങ്കിൽ (item != ശൂന്യ)
	    idx = l->lv_u.mat.lv_idx;
    }

    set_vim_var_type(VV_താക്കോൽ, VAR_സംഖ്യ);

    പൂർണ്ണസംഖ്യ		called_emsg_start = called_emsg;
    വേണ്ടി ( ; item != ശൂന്യ; item = item->li_next, ++idx)
    {
	set_vim_var_nr(VV_താക്കോൽ, idx);
	copy_tv(&item->li_tv, get_vim_var_tv(VV_VAL));

	found = indexof_eval_വ്യഞ്ജകം(വ്യഞ്ജകം);
	clear_tv(get_vim_var_tv(VV_VAL));

	എങ്കിൽ (found)
	    തിരികെ idx;

	എങ്കിൽ (called_emsg != called_emsg_start)
	    തിരികെ -1;
    }

    തിരികെ -1;
}

/*
 * "indexof()" function
 */
    സ്ഥിതം ശൂന്യം
f_indexof(typval_T *argvars, typval_T *rettv)
{
    long	startidx = 0;
    typval_T	save_val;
    typval_T	save_താക്കോൽ;
    പൂർണ്ണസംഖ്യ		save_did_emsg;

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	തിരികെ;

    എങ്കിൽ ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == ശൂന്യ
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == ശൂന്യ))
	തിരികെ;

    എങ്കിൽ (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_സംഖ്യ_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_താക്കോൽ, &save_താക്കോൽ);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    എങ്കിൽ (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_സംഖ്യ = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_സംഖ്യ = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_താക്കോൽ, &save_താക്കോൽ);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ inputsecret_flag = 0;

/*
 * "input()" function
 *     Also handles inputsecret() when inputsecret is set.
 */
    സ്ഥിതം ശൂന്യം
f_നിവേശം(typval_T *argvars, typval_T *rettv)
{
    get_user_നിവേശം(argvars, rettv, FALSE, inputsecret_flag);
}

/*
 * "inputdialog()" function
 */
    സ്ഥിതം ശൂന്യം
f_inputdialog(typval_T *argvars, typval_T *rettv)
{
#എങ്കിൽ defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    എങ്കിൽ (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == ശൂന്യ)
    {
	char_u	*message;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	എങ്കിൽ (in_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    തിരികെ;

	message = tv_get_string_chk(&argvars[0]);
	എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != ശൂന്യ)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	അല്ലെങ്കിൽ
	    IObuff[0] = NUL;
	എങ്കിൽ (message != ശൂന്യ && defstr != ശൂന്യ
		&& do_dialog(VIM_QUESTION, ശൂന്യ, message,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (message != ശൂന്യ && defstr != ശൂന്യ
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    അല്ലെങ്കിൽ
		rettv->vval.v_string = ശൂന്യ;
	}
	rettv->v_type = VAR_STRING;
    }
    അല്ലെങ്കിൽ
#endif
	get_user_നിവേശം(argvars, rettv, TRUE, inputsecret_flag);
}

/*
 * "inputlist()" function
 */
    സ്ഥിതം ശൂന്യം
f_inputlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ		selected;
    പൂർണ്ണസംഖ്യ		mouse_used;

#ifdef NO_CONSOLE_നിവേശം
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    എങ്കിൽ (no_console_നിവേശം() && !is_not_a_term())
	തിരികെ;
#endif
    എങ്കിൽ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ശൂന്യ)
    {
	semsg(_(e_argument_of_str_must_be_list), "inputlist()");
	തിരികെ;
    }

    സന്ദേശം_start();
    സന്ദേശം_row = Rows - 1;	// for when 'cmdheight' > 1
    വരികൾ_ഇടത് = Rows;	// avoid more prompt
    സന്ദേശം_scroll = TRUE;
    സന്ദേശം_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	സന്ദേശം_puts((അക്ഷരം *)tv_get_string(&li->li_tv));
	സന്ദേശം_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_സംഖ്യ(&mouse_used);
    എങ്കിൽ (mouse_used)
	selected -= വരികൾ_ഇടത്;

    rettv->vval.v_സംഖ്യ = selected;
}

സ്ഥിതം garray_T	    ga_userinput = {0, 0, വലുപ്പം(tasave_T), 4, ശൂന്യ};

/*
 * "inputrestore()" function
 */
    സ്ഥിതം ശൂന്യം
f_inputrestore(typval_T *argvars UNUSED, typval_T *rettv)
{
    എങ്കിൽ (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_data)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p_verbose > 1)
    {
	verb_സന്ദേശം(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_സംഖ്യ = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    സ്ഥിതം ശൂന്യം
f_inputsave(typval_T *argvars UNUSED, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    എങ്കിൽ (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_data)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    സ്ഥിതം ശൂന്യം
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    ++cmdline_star;
    ++inputsecret_flag;
    f_നിവേശം(argvars, rettv);
    --cmdline_star;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    സ്ഥിതം ശൂന്യം
f_interrupt(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    സ്ഥിതം ശൂന്യം
f_invert(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = ~tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    സ്ഥിതം ശൂന്യം
free_lval_വേര്(lval_വേര്_T *വേര്)
{
    എങ്കിൽ (വേര്->lr_tv != ശൂന്യ)
	free_tv(വേര്->lr_tv);
    class_unref(വേര്->lr_cl_exec);
    വേര്->lr_tv = ശൂന്യ;
    വേര്->lr_cl_exec = ശൂന്യ;
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

    സ്ഥിതം പൂർണ്ണസംഖ്യ
fix_variable_reference_lval_വേര്(lval_വേര്_T *വേര്, char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    പൂർണ്ണസംഖ്യ found_member = FALSE;
    എങ്കിൽ (വേര്->lr_tv->v_type == VAR_OBJECT)
    {
	എങ്കിൽ (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    found_member = TRUE;
    }
    എങ്കിൽ (!found_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*end;
	വേണ്ടി (end = name; ASCII_ISALNUM(*end) || *end == '_'; ++end)
	    ;
	പൂർണ്ണസംഖ്യ idx = class_member_idx(വേര്->lr_cl_exec, name, end - name);
	എങ്കിൽ (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(വേര്->lr_tv);
	    copy_tv(&വേര്->lr_cl_exec->class_members_tv[idx], വേര്->lr_tv);
	    found_member = TRUE;
	}
    }
    എങ്കിൽ (!found_member)
    {
	free_tv(വേര്->lr_tv);
	വേര്->lr_tv = ശൂന്യ;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    തിരികെ OK;
}

/*
 * "islocked()" function
 */
    സ്ഥിതം ശൂന്യം
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*end;
    dictitem_T	*di;

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    char_u *name = tv_get_string(&argvars[0]);
#ifdef LOG_LOCKVAR
    ch_log(ശൂന്യ, "LKVAR: f_islocked(): name: %s", name);
#endif

    lval_വേര്_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_വേര്_T *വേര് = ശൂന്യ;

    // Set up lval_root if executing in a method.
    എങ്കിൽ (fill_exec_lval_വേര്(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	എങ്കിൽ (fix_variable_reference_lval_വേര്(&aroot, name) == OK)
	    വേര് = &aroot;
    }

    lval_വേര്_T	*lval_വേര്_save = lval_വേര്;
    lval_വേര് = വേര്;
    end = get_lval(name, ശൂന്യ, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_START);
    lval_വേര് = lval_വേര്_save;

    എങ്കിൽ (end != ശൂന്യ && lv.ll_name != ശൂന്യ)
    {
	എങ്കിൽ (*end != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_end
		   ? e_invalid_argument_str : e_trailing_characters_str), end);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (lv.ll_tv == ശൂന്യ)
	    {
		di = find_var(lv.ll_name, ശൂന്യ, TRUE);
		എങ്കിൽ (di != ശൂന്യ)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_സംഖ്യ = ((di->di_flags & DI_FLAGS_LOCK)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (lv.ll_is_വേര്)
	    {
		rettv->vval.v_സംഖ്യ = tv_islocked(lv.ll_tv);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (lv.ll_object != ശൂന്യ)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_സംഖ്യ = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(ശൂന്യ, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#endif
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (lv.ll_class != ശൂന്യ)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_സംഖ്യ = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(ശൂന്യ, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#endif
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    അല്ലെങ്കിൽ എങ്കിൽ (lv.ll_newkey != ശൂന്യ)
		semsg(_(e_താക്കോൽ_not_present_in_dictionary_str), lv.ll_newkey);
	    അല്ലെങ്കിൽ എങ്കിൽ (lv.ll_list != ശൂന്യ)
		// List item.
		rettv->vval.v_സംഖ്യ = tv_islocked(&lv.ll_li->li_tv);
	    അല്ലെങ്കിൽ എങ്കിൽ (lv.ll_tuple != ശൂന്യ)
		// Tuple item.
		rettv->vval.v_സംഖ്യ = tv_islocked(lv.ll_tv);
	    അല്ലെങ്കിൽ
		// Dictionary item.
		rettv->vval.v_സംഖ്യ = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    എങ്കിൽ (വേര് != ശൂന്യ)
	free_lval_വേര്(വേര്);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    സ്ഥിതം ശൂന്യം
f_ഇനങ്ങൾ(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    അല്ലെങ്കിൽ
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), 1);
}

/*
 * "keytrans()" function
 */
    സ്ഥിതം ശൂന്യം
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == ശൂന്യ)
	തിരികെ;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    സ്ഥിതം ശൂന്യം
f_last_buffer_nr(typval_T *argvars UNUSED, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	എങ്കിൽ (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_സംഖ്യ = n;
}

/*
 * "len()" function
 */
    ശൂന്യം
f_len(typval_T *argvars, typval_T *rettv)
{
    തിരഞ്ഞെടുപ്പ് (argvars[0].v_type)
    {
	അവസ്ഥ VAR_STRING:
	അവസ്ഥ VAR_സംഖ്യ:
	    rettv->vval.v_സംഖ്യ = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    നിർത്തുക;
	അവസ്ഥ VAR_BLOB:
	    rettv->vval.v_സംഖ്യ = blob_len(argvars[0].vval.v_blob);
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    rettv->vval.v_സംഖ്യ = list_len(argvars[0].vval.v_list);
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    rettv->vval.v_സംഖ്യ = tuple_len(argvars[0].vval.v_tuple);
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    rettv->vval.v_സംഖ്യ = dict_len(argvars[0].vval.v_dict);
	    നിർത്തുക;
	അവസ്ഥ VAR_OBJECT:
	    rettv->vval.v_സംഖ്യ = object_len(argvars[0].vval.v_object);
	    നിർത്തുക;
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	അവസ്ഥ VAR_FLOAT:
	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL:
	അവസ്ഥ VAR_JOB:
	അവസ്ഥ VAR_CHANNEL:
	അവസ്ഥ VAR_INSTR:
	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    നിർത്തുക;
    }
}

    സ്ഥിതം ശൂന്യം
libcall_common(typval_T *argvars UNUSED, typval_T *rettv, പൂർണ്ണസംഖ്യ type)
{
#ifdef FEAT_LIBCALL
    char_u		*string_in;
    char_u		**string_result;
    പൂർണ്ണസംഖ്യ			nr_result;
#endif

    rettv->v_type = type;
    എങ്കിൽ (type != VAR_സംഖ്യ)
	rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_സംഖ്യ_arg(argvars, 2) == FAIL))
	തിരികെ;

#ifdef FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    എങ്കിൽ (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_in = ശൂന്യ;
	എങ്കിൽ (argvars[2].v_type == VAR_STRING)
	    string_in = argvars[2].vval.v_string;
	എങ്കിൽ (type == VAR_സംഖ്യ)
	{
	    string_result = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	{
	    rettv->vval.v_string = ശൂന്യ;
	    string_result = &rettv->vval.v_string;
	}
	എങ്കിൽ (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_in,
			     argvars[2].vval.v_സംഖ്യ,
			     string_result,
			     &nr_result) == OK
		&& type == VAR_സംഖ്യ)
	    rettv->vval.v_സംഖ്യ = nr_result;
    }
#endif
}

/*
 * "libcall()" function
 */
    സ്ഥിതം ശൂന്യം
f_libcall(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_STRING);
}

/*
 * "libcallnr()" function
 */
    സ്ഥിതം ശൂന്യം
f_libcallnr(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_സംഖ്യ);
}

/*
 * "line(string, [winid])" function
 */
    സ്ഥിതം ശൂന്യം
f_വരി(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum = 0;
    pos_T	*fp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		fnum;
    പൂർണ്ണസംഖ്യ		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	എങ്കിൽ (wp != ശൂന്യ && tp != ശൂന്യ)
	{
	    എങ്കിൽ (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		എങ്കിൽ (*p_spk != 'c'
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
    അല്ലെങ്കിൽ
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    എങ്കിൽ (fp != ശൂന്യ)
	lnum = fp->lnum;
    rettv->vval.v_സംഖ്യ = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    സ്ഥിതം ശൂന്യം
f_line2byte(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_സംഖ്യ = -1;
#അല്ലെങ്കിൽ
    linenr_T	lnum;

    എങ്കിൽ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	തിരികെ;

    lnum = tv_get_lnum(argvars);
    എങ്കിൽ (lnum < 1 || lnum > curbuf->b_ml.ml_വരി_എണ്ണം + 1)
	rettv->vval.v_സംഖ്യ = -1;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = ml_find_വരി_or_offset(curbuf, lnum, ശൂന്യ);
    എങ്കിൽ (rettv->vval.v_സംഖ്യ >= 0)
	++rettv->vval.v_സംഖ്യ;
#endif
}

#ifdef FEAT_LUA
/*
 * "luaeval()" function
 */
    സ്ഥിതം ശൂന്യം
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#endif

തരനിർവചനം എണ്ണൽ
{
    MATCH_END,	    // matchend()
    MATCH_MATCH,    // match()
    MATCH_STR,	    // matchstr()
    MATCH_LIST,	    // matchlist()
    MATCH_POS	    // matchstrpos()
} matchtype_T;

    സ്ഥിതം ശൂന്യം
find_some_match(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = ശൂന്യ;
    long	len = 0;
    char_u	*വ്യഞ്ജകം = ശൂന്യ;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    long	start = 0;
    long	nth = 1;
    colnr_T	startcol = 0;
    പൂർണ്ണസംഖ്യ		match = 0;
    list_T	*l = ശൂന്യ;
    listitem_T	*li = ശൂന്യ;
    long	idx = 0;
    പൂർണ്ണസംഖ്യ		prev_lock = 0;
    char_u	*tofree = ശൂന്യ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    rettv->vval.v_സംഖ്യ = -1;
    എങ്കിൽ (type == MATCH_LIST || type == MATCH_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	    പോകുക theend;
	എങ്കിൽ (type == MATCH_POS
		&& (list_append_string(rettv->vval.v_list,
					    (char_u *)"", 0) == FAIL
		    || list_append_സംഖ്യ(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_സംഖ്യ(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_സംഖ്യ(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL))
	{
		list_free(rettv->vval.v_list);
		rettv->vval.v_list = ശൂന്യ;
		പോകുക theend;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (type == MATCH_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ശൂന്യ;
    }

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 3) == FAIL)))
	പോകുക theend;

    എങ്കിൽ (argvars[0].v_type == VAR_LIST)
    {
	എങ്കിൽ ((l = argvars[0].vval.v_list) == ശൂന്യ)
	    പോകുക theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    അല്ലെങ്കിൽ
    {
	വ്യഞ്ജകം = str = tv_get_string(&argvars[0]);
	len = (long)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    എങ്കിൽ (pat == ശൂന്യ)
	പോകുക theend;

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	പൂർണ്ണസംഖ്യ	    error = FALSE;

	start = (long)tv_get_സംഖ്യ_chk(&argvars[2], &error);
	എങ്കിൽ (error)
	    പോകുക theend;
	എങ്കിൽ (l != ശൂന്യ)
	{
	    li = list_find(l, start);
	    എങ്കിൽ (li == ശൂന്യ)
		പോകുക theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (start < 0)
		start = 0;
	    എങ്കിൽ (start > len)
		പോകുക theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    എങ്കിൽ (argvars[3].v_type != VAR_UNKNOWN)
		startcol = start;
	    അല്ലെങ്കിൽ
	    {
		str += start;
		len -= start;
	    }
	}

	എങ്കിൽ (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (long)tv_get_സംഖ്യ_chk(&argvars[3], &error);
	എങ്കിൽ (error)
	    പോകുക theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    എങ്കിൽ (regmatch.regprog != ശൂന്യ)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	എങ്കിൽ (l != ശൂന്യ)
	{
	    prev_lock = l->lv_lock;
	    എങ്കിൽ (l->lv_lock == 0)
		l->lv_lock = VAR_LOCKED;
	}

	വേണ്ടി (;;)
	{
	    എങ്കിൽ (l != ശൂന്യ)
	    {
		എങ്കിൽ (li == ശൂന്യ)
		{
		    match = FALSE;
		    നിർത്തുക;
		}
		vim_free(tofree);
		വ്യഞ്ജകം = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		എങ്കിൽ (str == ശൂന്യ)
		    നിർത്തുക;
	    }

	    match = vim_regexec_nl(&regmatch, str, startcol);

	    എങ്കിൽ (match && --nth <= 0)
		നിർത്തുക;
	    എങ്കിൽ (l == ശൂന്യ && !match)
		നിർത്തുക;

	    // Advance to just after the match.
	    എങ്കിൽ (l != ശൂന്യ)
	    {
		li = li->li_next;
		++idx;
	    }
	    അല്ലെങ്കിൽ
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		എങ്കിൽ (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    match = FALSE;
		    നിർത്തുക;
		}
	    }
	}

	എങ്കിൽ (match)
	{
	    എങ്കിൽ (type == MATCH_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_first;
		listitem_T *li2 = li1->li_next;
		listitem_T *li3 = li2->li_next;
		listitem_T *li4 = li3->li_next;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_സംഖ്യ =
				      (varnumber_T)(regmatch.startp[0] - വ്യഞ്ജകം);
		li4->li_tv.vval.v_സംഖ്യ =
					(varnumber_T)(regmatch.endp[0] - വ്യഞ്ജകം);
		എങ്കിൽ (l != ശൂന്യ)
		    li2->li_tv.vval.v_സംഖ്യ = (varnumber_T)idx;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (type == MATCH_LIST)
	    {
		പൂർണ്ണസംഖ്യ i;

		// return list with matched string and submatches
		വേണ്ടി (i = 0; i < NSUBEXP; ++i)
		{
		    എങ്കിൽ (regmatch.endp[i] == ശൂന്യ)
		    {
			എങ്കിൽ (list_append_string(rettv->vval.v_list,
							    ശൂന്യ, 0) == FAIL)
			    നിർത്തുക;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(പൂർണ്ണസംഖ്യ)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			നിർത്തുക;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (type == MATCH_STR)
	    {
		// return matched string
		എങ്കിൽ (l != ശൂന്യ)
		    copy_tv(&li->li_tv, rettv);
		അല്ലെങ്കിൽ
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (l != ശൂന്യ)
		rettv->vval.v_സംഖ്യ = idx;
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (type != MATCH_END)
		    rettv->vval.v_സംഖ്യ =
				      (varnumber_T)(regmatch.startp[0] - str);
		അല്ലെങ്കിൽ
		    rettv->vval.v_സംഖ്യ =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_സംഖ്യ += (varnumber_T)(str - വ്യഞ്ജകം);
	    }
	}
	എങ്കിൽ (l != ശൂന്യ)
	    l->lv_lock = prev_lock;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    എങ്കിൽ (type == MATCH_POS && l == ശൂന്യ && rettv->vval.v_list != ശൂന്യ)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_matches_in_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    പൂർണ്ണസംഖ്യ		idx,
    പൂർണ്ണസംഖ്യ		submatches,
    പൂർണ്ണസംഖ്യ		matchbuf)
{
    long	len = (long)STRLEN(str);
    പൂർണ്ണസംഖ്യ		match = 0;
    colnr_T	startidx = 0;

    വേണ്ടി (;;)
    {
	match = vim_regexec_nl(rmp, str, startidx);
	എങ്കിൽ (!match)
	    നിർത്തുക;

	dict_T *d = dict_alloc();
	എങ്കിൽ (d == ശൂന്യ)
	    തിരികെ FAIL;
	എങ്കിൽ (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (dict_add_സംഖ്യ(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (dict_add_സംഖ്യ(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (dict_add_string_len(d, "text", rmp->startp[0],
		    (പൂർണ്ണസംഖ്യ)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (submatches)
	{
	    list_T *sml = list_alloc();
	    എങ്കിൽ (sml == ശൂന്യ)
		തിരികെ FAIL;

	    എങ്കിൽ (dict_add_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		തിരികെ FAIL;
	    }

	    // return a list with the submatches
	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < NSUBEXP; ++i)
	    {
		എങ്കിൽ (rmp->endp[i] == ശൂന്യ)
		{
		    എങ്കിൽ (list_append_string(sml, (char_u *)"", 0) == FAIL)
			തിരികെ FAIL;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (list_append_string(sml, rmp->startp[i],
			    (പൂർണ്ണസംഖ്യ)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    തിരികെ FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	എങ്കിൽ (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    നിർത്തുക;
    }

    തിരികെ OK;
}

/*
 * "matchbufline()" function
 */
    സ്ഥിതം ശൂന്യം
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ശൂന്യ;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_സംഖ്യ = -1;
    എങ്കിൽ (rettv_list_alloc(rettv) != OK)
	തിരികെ;
    retlist = rettv->vval.v_list;

    എങ്കിൽ (check_for_buffer_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	തിരികെ;

    പൂർണ്ണസംഖ്യ prev_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    എങ്കിൽ (buf == ശൂന്യ)
    {
	എങ്കിൽ (did_emsg == prev_did_emsg)
	    semsg(_(e_invalid_buffer_name_str), tv_get_string(&argvars[0]));
	തിരികെ;
    }
    എങ്കിൽ (buf->b_ml.ml_mfp == ശൂന്യ)
    {
	emsg(_(e_buffer_is_not_loaded));
	തിരികെ;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    എങ്കിൽ (did_emsg > did_emsg_before)
	തിരികെ;
    എങ്കിൽ (slnum < 1)
    {
	semsg(_(e_invalid_മൂല്യം_for_argument_str), "lnum");
	തിരികെ;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    എങ്കിൽ (did_emsg > did_emsg_before)
	തിരികെ;
    എങ്കിൽ (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_മൂല്യം_for_argument_str), "end_lnum");
	തിരികെ;
    }

    എങ്കിൽ (elnum > buf->b_ml.ml_വരി_എണ്ണം)
	elnum = buf->b_ml.ml_വരി_എണ്ണം;

    പൂർണ്ണസംഖ്യ		submatches = FALSE;
    എങ്കിൽ (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	എങ്കിൽ (d != ശൂന്യ)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    എങ്കിൽ (di != ശൂന്യ)
	    {
		എങ്കിൽ (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "submatches");
		    തിരികെ;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    എങ്കിൽ (regmatch.regprog == ശൂന്യ)
	പോകുക theend;
    regmatch.rm_ic = p_ic;

    ആകുമ്പോൾ (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	എങ്കിൽ (get_matches_in_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    പോകുക cleanup;
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
    സ്ഥിതം ശൂന്യം
f_match(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_MATCH);
}

/*
 * "matchend()" function
 */
    സ്ഥിതം ശൂന്യം
f_matchend(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_END);
}

/*
 * "matchlist()" function
 */
    സ്ഥിതം ശൂന്യം
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_LIST);
}

/*
 * "matchstr()" function
 */
    സ്ഥിതം ശൂന്യം
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_STR);
}

/*
 * "matchstrlist()" function
 */
    സ്ഥിതം ശൂന്യം
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ശൂന്യ;
    char_u	*save_cpo;
    list_T	*l = ശൂന്യ;
    listitem_T	*li = ശൂന്യ;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_സംഖ്യ = -1;
    എങ്കിൽ (rettv_list_alloc(rettv) != OK)
	തിരികെ;
    retlist = rettv->vval.v_list;

    എങ്കിൽ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	തിരികെ;

    എങ്കിൽ ((l = argvars[0].vval.v_list) == ശൂന്യ)
	തിരികെ;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    എങ്കിൽ (pat == ശൂന്യ)
	തിരികെ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    എങ്കിൽ (regmatch.regprog == ശൂന്യ)
	പോകുക theend;
    regmatch.rm_ic = p_ic;

    പൂർണ്ണസംഖ്യ		submatches = FALSE;
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	എങ്കിൽ (d != ശൂന്യ)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    എങ്കിൽ (di != ശൂന്യ)
	    {
		എങ്കിൽ (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_മൂല്യം_for_argument_str), "submatches");
		    പോകുക cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    പൂർണ്ണസംഖ്യ idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	എങ്കിൽ (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != ശൂന്യ)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    എങ്കിൽ (get_matches_in_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		പോകുക cleanup;
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
    സ്ഥിതം ശൂന്യം
f_matchstrpos(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_POS);
}

    സ്ഥിതം ശൂന്യം
max_min(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ domax)
{
    varnumber_T	n = 0;

    എങ്കിൽ (in_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = 0;

    എങ്കിൽ (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = ശൂന്യ;

	l = argvars[0].vval.v_list;
	എങ്കിൽ (l != ശൂന്യ && l->lv_len > 0)
	{
	    എങ്കിൽ (l->lv_first == &range_list_item)
	    {
		എങ്കിൽ ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_start;
		അല്ലെങ്കിൽ
		    n = l->lv_u.nonmat.lv_start + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_സംഖ്യ = n;
	    }
	    അല്ലെങ്കിൽ
	    {
		FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
		{
		    എങ്കിൽ (tv == ശൂന്യ)
			tv = &li->li_tv;
		    അല്ലെങ്കിൽ
		    {
			പൂർണ്ണസംഖ്യ res;
			എങ്കിൽ (typval_compare2(&li->li_tv, tv,
				domax ? വ്യഞ്ജകം_GREATER : വ്യഞ്ജകം_SMALLER, FALSE, &res) == FAIL)
			    തിരികെ;
			എങ്കിൽ (res == OK)
			    tv = &li->li_tv;
		    }
		}

		എങ്കിൽ (tv != ശൂന്യ)
		    copy_tv(tv, rettv);
	    }
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	എങ്കിൽ (tv != ശൂന്യ)
	    copy_tv(tv, rettv);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	പൂർണ്ണസംഖ്യ		todo;
	typval_T	*tv = ശൂന്യ;

	d = argvars[0].vval.v_dict;
	എങ്കിൽ (d != ശൂന്യ)
	{
	    todo = (പൂർണ്ണസംഖ്യ)d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&d->dv_hashtab, hi, todo)
	    {
		എങ്കിൽ (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    എങ്കിൽ (tv == ശൂന്യ)
			tv = &HI2DI(hi)->di_tv;
		    അല്ലെങ്കിൽ
		    {
			പൂർണ്ണസംഖ്യ res;
			എങ്കിൽ (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? വ്യഞ്ജകം_GREATER : വ്യഞ്ജകം_SMALLER, FALSE, &res) == FAIL)
			    തിരികെ;
			എങ്കിൽ (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	എങ്കിൽ (tv != ശൂന്യ)
	    copy_tv(tv, rettv);
    }
    അല്ലെങ്കിൽ
	semsg(_(e_argument_of_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
}

/*
 * "max()" function
 */
    സ്ഥിതം ശൂന്യം
f_max(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, TRUE);
}

/*
 * "min()" function
 */
    സ്ഥിതം ശൂന്യം
f_min(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, FALSE);
}

#എങ്കിൽ defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    സ്ഥിതം ശൂന്യം
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    എങ്കിൽ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	തിരികെ;

    വേണ്ടി (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	എങ്കിൽ (lnum < 0 || lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	{
	    lnum = 0;
	    നിർത്തുക;
	}
	എങ്കിൽ (*skipwhite(ml_get(lnum)) != NUL)
	    നിർത്തുക;
    }
    rettv->vval.v_സംഖ്യ = lnum;
}


/*
 * "ngettext()" function
 */
    സ്ഥിതം ശൂന്യം
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#എങ്കിൽ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    അക്ഷരം *prev = ശൂന്യ;
#endif

    എങ്കിൽ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_സംഖ്യ_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_STRING;

    എങ്കിൽ (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != ശൂന്യ &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#എങ്കിൽ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((സ്ഥിരം അക്ഷരം *)argvars[3].vval.v_string, (അക്ഷരം *)p_enc);
#endif

#എങ്കിൽ defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((സ്ഥിരം അക്ഷരം *)argvars[3].vval.v_string, (സ്ഥിരം അക്ഷരം *)argvars[0].vval.v_string, (സ്ഥിരം അക്ഷരം *)argvars[1].vval.v_string, (പൂർണ്ണസംഖ്യ)argvars[2].vval.v_സംഖ്യ));
#അല്ലെങ്കിൽ
	textdomain((സ്ഥിരം അക്ഷരം *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((സ്ഥിരം അക്ഷരം *)argvars[0].vval.v_string, (സ്ഥിരം അക്ഷരം *)argvars[1].vval.v_string, argvars[2].vval.v_സംഖ്യ));
	textdomain(VIMPACKAGE);
#endif

#എങ്കിൽ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	എങ്കിൽ (prev != ശൂന്യ)
	    bind_textdomain_codeset((സ്ഥിരം അക്ഷരം *)argvars[3].vval.v_string, prev);
#endif
    }
    അല്ലെങ്കിൽ
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((സ്ഥിരം അക്ഷരം *)argvars[0].vval.v_string, (സ്ഥിരം അക്ഷരം *)argvars[1].vval.v_string, argvars[2].vval.v_സംഖ്യ));
}


/*
 * "nr2char()" function
 */
    സ്ഥിതം ശൂന്യം
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (has_mbyte)
    {
	പൂർണ്ണസംഖ്യ	utf8 = 0;

	എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[1], ശൂന്യ);
	എങ്കിൽ (utf8)
	    buf[utf_char2bytes((പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]), buf)] = NUL;
	അല്ലെങ്കിൽ
	    buf[(*mb_char2bytes)((പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]), buf)] = NUL;
    }
    അല്ലെങ്കിൽ
    {
	buf[0] = (char_u)tv_get_സംഖ്യ(&argvars[0]);
	buf[1] = NUL;
    }
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "or(expr, expr)" function
 */
    സ്ഥിതം ശൂന്യം
f_or(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    rettv->vval.v_സംഖ്യ = tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ)
					| tv_get_സംഖ്യ_chk(&argvars[1], ശൂന്യ);
}

#ifdef FEAT_PERL
/*
 * "perleval()" function
 */
    സ്ഥിതം ശൂന്യം
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#endif

/*
 * "prevnonblank()" function
 */
    സ്ഥിതം ശൂന്യം
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    എങ്കിൽ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	തിരികെ;

    lnum = tv_get_lnum(argvars);
    എങ്കിൽ (lnum < 1 || lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	lnum = 0;
    അല്ലെങ്കിൽ
	ആകുമ്പോൾ (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
	    --lnum;
    rettv->vval.v_സംഖ്യ = lnum;
}

// This dummy va_list is here because:
// - passing a NULL pointer doesn't work when va_list isn't a pointer
// - locally in the function results in a "used before set" warning
// - using va_start() to initialize it gives "function with fixed args" error
സ്ഥിതം va_list	ap;

/*
 * "printf()" function
 */
    സ്ഥിതം ശൂന്യം
f_printf(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		len;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		saved_did_emsg = did_emsg;
    അക്ഷരം	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script() && check_for_string_or_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (അക്ഷരം *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(ശൂന്യ, 0, fmt, ap, argvars + 1);
    എങ്കിൽ (!did_emsg)
    {
	s = alloc(len + 1);
	എങ്കിൽ (s != ശൂന്യ)
	{
	    rettv->vval.v_string = s;
	    (ശൂന്യം)vim_vsnprintf_typval((അക്ഷരം *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    സ്ഥിതം ശൂന്യം
f_pum_getpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;
    pum_set_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    സ്ഥിതം ശൂന്യം
f_pumvisible(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    എങ്കിൽ (pum_visible())
	rettv->vval.v_സംഖ്യ = 1;
}

#ifdef FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    സ്ഥിതം ശൂന്യം
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (p_pyx == 0)
	p_pyx = 3;

    എങ്കിൽ (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	തിരികെ;
    }
    അല്ലെങ്കിൽ
    {
	locals = ശൂന്യ;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#endif

#ifdef FEAT_PYTHON
/*
 * "pyeval()" function
 */
    സ്ഥിതം ശൂന്യം
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	തിരികെ;

    എങ്കിൽ (p_pyx == 0)
	p_pyx = 2;

    എങ്കിൽ (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	തിരികെ;
    }
    അല്ലെങ്കിൽ
    {
	locals = ശൂന്യ;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#endif

#എങ്കിൽ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    സ്ഥിതം ശൂന്യം
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

# എങ്കിൽ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    എങ്കിൽ (p_pyx == 2)
	f_pyeval(argvars, rettv);
    അല്ലെങ്കിൽ
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#endif

സ്ഥിതം UINT32_T srand_seed_for_testing = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ	srand_seed_for_testing_is_used = FALSE;

    സ്ഥിതം ശൂന്യം
f_test_srand_seed(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_UNKNOWN)
	srand_seed_for_testing_is_used = FALSE;
    അല്ലെങ്കിൽ
    {
	srand_seed_for_testing = (UINT32_T)tv_get_സംഖ്യ(&argvars[0]);
	srand_seed_for_testing_is_used = TRUE;
    }
}

    സ്ഥിതം ശൂന്യം
init_srand(UINT32_T *x)
{
    ഘടന {
	സംഘം {
	    UINT32_T സംഖ്യ;
	    char_u   bytes[വലുപ്പം(UINT32_T)];
	} contents;
    } buf;

    എങ്കിൽ (srand_seed_for_testing_is_used)
    {
	*x = srand_seed_for_testing;
	തിരികെ;
    }

    എങ്കിൽ (mch_get_random(buf.contents.bytes, വലുപ്പം(buf.contents.bytes)) == OK)
    {
	*x = buf.contents.സംഖ്യ;
	തിരികെ;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#എങ്കിൽ defined(FEAT_SODIUM)
    എങ്കിൽ (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    അല്ലെങ്കിൽ
#endif
    {
#എങ്കിൽ defined(FEAT_RELTIME)
	proftime_T res;
	profile_start(&res);
# എങ്കിൽ defined(MSWIN)
	*x = (UINT32_T)res.താഴെയുള്ള_Part;
# അല്ലെങ്കിൽ
	*x = (UINT32_T)res.tv_fsec;
# endif
#അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = ശൂന്യ;
    സ്ഥിതം UINT32_T	gx, gy, gz, gw;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, result;

    എങ്കിൽ (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// When no argument is given use the global seed list.
	എങ്കിൽ (initialized == FALSE)
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
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	എങ്കിൽ (l == ശൂന്യ || list_len(l) != 4)
	    പോകുക theend;

	lx = list_find(l, 0L);
	ly = list_find(l, 1L);
	lz = list_find(l, 2L);
	lw = list_find(l, 3L);
	എങ്കിൽ (lx == ശൂന്യ || ly == ശൂന്യ || lz == ശൂന്യ || lw == ശൂന്യ)
	    പോകുക theend;
	എങ്കിൽ (lx->li_tv.v_type != VAR_സംഖ്യ
		|| ly->li_tv.v_type != VAR_സംഖ്യ
		|| lz->li_tv.v_type != VAR_സംഖ്യ
		|| lw->li_tv.v_type != VAR_സംഖ്യ)
	    പോകുക theend;
	x = (UINT32_T)lx->li_tv.vval.v_സംഖ്യ;
	y = (UINT32_T)ly->li_tv.vval.v_സംഖ്യ;
	z = (UINT32_T)lz->li_tv.vval.v_സംഖ്യ;
	w = (UINT32_T)lw->li_tv.vval.v_സംഖ്യ;

	SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w);

	lx->li_tv.vval.v_സംഖ്യ = (varnumber_T)x;
	ly->li_tv.vval.v_സംഖ്യ = (varnumber_T)y;
	lz->li_tv.vval.v_സംഖ്യ = (varnumber_T)z;
	lw->li_tv.vval.v_സംഖ്യ = (varnumber_T)w;
    }
    അല്ലെങ്കിൽ
	പോകുക theend;

    rettv->v_type = VAR_സംഖ്യ;
    rettv->vval.v_സംഖ്യ = (varnumber_T)result;
    തിരികെ;

theend:
    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    rettv->v_type = VAR_സംഖ്യ;
    rettv->vval.v_സംഖ്യ = -1;
}

/*
 * "srand()" function
 */
    സ്ഥിതം ശൂന്യം
f_srand(typval_T *argvars, typval_T *rettv)
{
    UINT32_T x = 0, z;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	    error = FALSE;

	x = (UINT32_T)tv_get_സംഖ്യ_chk(&argvars[0], &error);
	എങ്കിൽ (error)
	    തിരികെ;
    }

    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#undef ROTL
#undef SPLITMIX32
#undef SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    സ്ഥിതം ശൂന്യം
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	start;
    varnumber_T	end;
    varnumber_T	stride = 1;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL)))
	തിരികെ;

    start = tv_get_സംഖ്യ_chk(&argvars[0], &error);
    എങ്കിൽ (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = start - 1;
	start = 0;
    }
    അല്ലെങ്കിൽ
    {
	end = tv_get_സംഖ്യ_chk(&argvars[1], &error);
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_സംഖ്യ_chk(&argvars[2], &error);
    }

    എങ്കിൽ (error)
	തിരികെ;		// type error; errmsg already given
    എങ്കിൽ (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	തിരികെ;
    }

    // The stride is stored in "lv_stride", which is an int.
    എങ്കിൽ (stride < INT_MIN || stride > INT_MAX)
    {
	അക്ഷരം	buf[NUMBUFLEN];

	vim_snprintf(buf, വലുപ്പം(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	തിരികെ;
    }

    uvarnumber_T	len;

    എങ്കിൽ (stride > 0 ? end < start : end > start)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	back = stride > 0
				  ? (uvarnumber_T)start - (uvarnumber_T)end
				  : (uvarnumber_T)end - (uvarnumber_T)start;

	എങ്കിൽ (back > 1)
	{
	    emsg(_(e_start_past_end));
	    തിരികെ;
	}
	len = 0;
    }
    അല്ലെങ്കിൽ
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	span = stride > 0
				  ? (uvarnumber_T)end - (uvarnumber_T)start
				  : (uvarnumber_T)start - (uvarnumber_T)end;
	uvarnumber_T	എണ്ണം = span / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	എങ്കിൽ (എണ്ണം >= (uvarnumber_T)INT_MAX)
	{
	    അക്ഷരം	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, വലുപ്പം(buf), "%llu",
			      എണ്ണം < UVARNUM_MAX ? എണ്ണം + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    തിരികെ;
	}
	len = എണ്ണം + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_first = &range_list_item;
    list->lv_u.nonmat.lv_start = start;
    list->lv_u.nonmat.lv_end = end;
    list->lv_u.nonmat.lv_stride = (പൂർണ്ണസംഖ്യ)stride;
    list->lv_len = (പൂർണ്ണസംഖ്യ)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    ശൂന്യം
range_list_materialize(list_T *list)
{
    varnumber_T start = list->lv_u.nonmat.lv_start;
    varnumber_T end = list->lv_u.nonmat.lv_end;
    പൂർണ്ണസംഖ്യ		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_first = ശൂന്യ;
    list->lv_u.mat.lv_last = ശൂന്യ;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_item = ശൂന്യ;
    വേണ്ടി (i = start; stride > 0 ? i <= end : i >= end; i += stride)
    {
	എങ്കിൽ (list_append_സംഖ്യ(list, i) == FAIL)
	    നിർത്തുക;
	എങ്കിൽ (list->lv_lock & VAR_ഇനങ്ങൾ_LOCKED)
	    list->lv_u.mat.lv_last->li_tv.v_lock = VAR_LOCKED;
    }
    list->lv_lock &= ~VAR_ഇനങ്ങൾ_LOCKED;
}

/*
 * "getreginfo()" function
 */
    സ്ഥിതം ശൂന്യം
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		regname;
    char_u	buf[NUMBUFLEN + 2];
    size_t	buflen;
    long	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    എങ്കിൽ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    regname = getreg_get_regname(argvars);
    എങ്കിൽ (regname == 0)
	തിരികെ;

    എങ്കിൽ (regname == '@')
	regname = '"';

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_വ്യഞ്ജകം_SRC | GREG_LIST);
    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (dict_add_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    തിരഞ്ഞെടുപ്പ് (get_reg_type(regname, &reglen))
    {
	അവസ്ഥ MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    നിർത്തുക;
	അവസ്ഥ MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    നിർത്തുക;
	അവസ്ഥ MBLOCK:
	    buflen = vim_snprintf_safelen((അക്ഷരം *)buf, വലുപ്പം(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    buf[0] = NUL;
	    buflen = 0;
	    നിർത്തുക;
    }
    (ശൂന്യം)dict_add_string_len(dict, (അക്ഷരം *)"regtype", buf, (പൂർണ്ണസംഖ്യ)buflen);

    buf[0] = get_register_name(get_unname_register());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    എങ്കിൽ (regname == '"')
	(ശൂന്യം)dict_add_string_len(dict, (അക്ഷരം *)"points_to", buf, (പൂർണ്ണസംഖ്യ)buflen);
    അല്ലെങ്കിൽ
    {
	dictitem_T	*item = dictitem_alloc((char_u *)"isunnamed");

	എങ്കിൽ (item != ശൂന്യ)
	{
	    item->di_tv.v_type = VAR_BOOL;
	    item->di_tv.vval.v_സംഖ്യ = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (ശൂന്യം)dict_add(dict, item);
	}
    }
}

    സ്ഥിതം ശൂന്യം
return_register(പൂർണ്ണസംഖ്യ regname, typval_T *rettv)
{
    char_u buf[2] = {NUL, NUL};

    buf[0] = (char_u)regname;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(buf, (buf[0] == NUL) ? 0 : 1);
}

/*
 * "reg_executing()" function
 */
    സ്ഥിതം ശൂന്യം
f_reg_executing(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    സ്ഥിതം ശൂന്യം
f_reg_recording(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    സ്ഥിതം ശൂന്യം
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_സംഖ്യ = -1;
    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    rettv->vval.v_സംഖ്യ = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    സ്ഥിതം ശൂന്യം
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ slen = list_len(l);

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL
	    || l == ശൂന്യ
	    || n <= 0
	    || slen == 0)
	തിരികെ;

    എങ്കിൽ (check_repeat_എണ്ണം(slen, n) == FAIL)
	തിരികെ;

    ആകുമ്പോൾ (n-- > 0)
	എങ്കിൽ (list_extend(rettv->vval.v_list, l, ശൂന്യ) == FAIL)
	    നിർത്തുക;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    സ്ഥിതം ശൂന്യം
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		slen;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    എങ്കിൽ (rettv_blob_alloc(rettv) == FAIL
	    || blob == ശൂന്യ
	    || n <= 0)
	തിരികെ;

    slen = blob->bv_ga.ga_len;
    എങ്കിൽ (slen <= 0)
	തിരികെ;
    എങ്കിൽ (check_repeat_എണ്ണം(slen, n) == FAIL)
	തിരികെ;
    len = slen * (പൂർണ്ണസംഖ്യ)n;

    എങ്കിൽ (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	തിരികെ;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    വേണ്ടി (i = 0; i < slen; ++i)
	എങ്കിൽ (blob_get(blob, i) != 0)
	    നിർത്തുക;

    എങ്കിൽ (i == slen)
	// No need to copy since all bytes are already zero
	തിരികെ;

    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)n; ++i)
	blob_set_range(rettv->vval.v_blob,
		(long)i * slen, ((long)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    സ്ഥിതം ശൂന്യം
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    size_t	slen;
    പൂർണ്ണസംഖ്യ		len;
    char_u	*r;
    പൂർണ്ണസംഖ്യ		done;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    slen = STRLEN(p);
    എങ്കിൽ (slen == 0 || n <= 0)
	തിരികെ;
    എങ്കിൽ (check_repeat_എണ്ണം((varnumber_T)slen, n) == FAIL)
	തിരികെ;
    len = (പൂർണ്ണസംഖ്യ)slen * (പൂർണ്ണസംഖ്യ)n;

    r = alloc((size_t)len + 1);
    എങ്കിൽ (r == ശൂന്യ)
	തിരികെ;

    mch_memmove(r, p, slen);
    done = (പൂർണ്ണസംഖ്യ)slen;
    ആകുമ്പോൾ (done < len)
    {
	പൂർണ്ണസംഖ്യ copy_len = done;

	എങ്കിൽ (copy_len > len - done)
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
    സ്ഥിതം ശൂന്യം
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    എങ്കിൽ (in_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    n = tv_get_സംഖ്യ(&argvars[1]);
    എങ്കിൽ (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    അല്ലെങ്കിൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_തിരയൽ_arg(typval_T *varp, പൂർണ്ണസംഖ്യ *flagsp)
{
    പൂർണ്ണസംഖ്യ		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		mask;

    എങ്കിൽ (varp->v_type == VAR_UNKNOWN)
	തിരികെ FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    എങ്കിൽ (flags == ശൂന്യ)
	തിരികെ 0;		// type error; errmsg already given
    ആകുമ്പോൾ (*flags != NUL)
    {
	തിരഞ്ഞെടുപ്പ് (*flags)
	{
	    അവസ്ഥ 'b': dir = BACKWARD; നിർത്തുക;
	    അവസ്ഥ 'w': p_ws = TRUE; നിർത്തുക;
	    അവസ്ഥ 'W': p_ws = FALSE; നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:  mask = 0;
		      എങ്കിൽ (flagsp != ശൂന്യ)
			  തിരഞ്ഞെടുപ്പ് (*flags)
			  {
			      അവസ്ഥ 'c': mask = SP_START; നിർത്തുക;
			      അവസ്ഥ 'e': mask = SP_END; നിർത്തുക;
			      അവസ്ഥ 'm': mask = SP_RETCOUNT; നിർത്തുക;
			      അവസ്ഥ 'n': mask = SP_NOMOVE; നിർത്തുക;
			      അവസ്ഥ 'p': mask = SP_SUBPAT; നിർത്തുക;
			      അവസ്ഥ 'r': mask = SP_REPEAT; നിർത്തുക;
			      അവസ്ഥ 's': mask = SP_SETPCMARK; നിർത്തുക;
			      അവസ്ഥ 'z': mask = SP_COLUMN; നിർത്തുക;
			  }
		      എങ്കിൽ (mask == 0)
		      {
			  semsg(_(e_invalid_argument_str), flags);
			  dir = 0;
		      }
		      അല്ലെങ്കിൽ
			  *flagsp |= mask;
	}
	എങ്കിൽ (dir == 0)
	    നിർത്തുക;
	++flags;
    }
    തിരികെ dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
തിരയൽ_cmn(typval_T *argvars, pos_T *match_pos, പൂർണ്ണസംഖ്യ *flagsp)
{
    പൂർണ്ണസംഖ്യ		flags;
    char_u	*pat;
    size_t	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    പൂർണ്ണസംഖ്യ		save_p_ws = p_ws;
    പൂർണ്ണസംഖ്യ		dir;
    പൂർണ്ണസംഖ്യ		retval = 0;	// default: FAIL
    long	lnum_stop = 0;
#ifdef FEAT_RELTIME
    long	time_limit = 0;
#endif
    പൂർണ്ണസംഖ്യ		options = തിരയൽ_KEEP;
    പൂർണ്ണസംഖ്യ		subpatnum;
    searchit_arg_T sia;
    പൂർണ്ണസംഖ്യ		use_skip = FALSE;
    pos_T	firstpos;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_സംഖ്യ_arg(argvars, 3) == FAIL)))))
	പോകുക theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_തിരയൽ_arg(&argvars[1], flagsp);	// may set p_ws
    എങ്കിൽ (dir == 0)
	പോകുക theend;
    flags = *flagsp;
    എങ്കിൽ (flags & SP_START)
	options |= തിരയൽ_START;
    എങ്കിൽ (flags & SP_END)
	options |= തിരയൽ_END;
    എങ്കിൽ (flags & SP_COLUMN)
	options |= തിരയൽ_COL;

    // Optional arguments: line number to stop searching, timeout and skip.
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (long)tv_get_സംഖ്യ_chk(&argvars[2], ശൂന്യ);
	എങ്കിൽ (lnum_stop < 0)
	    പോകുക theend;
	എങ്കിൽ (argvars[3].v_type != VAR_UNKNOWN)
	{
#ifdef FEAT_RELTIME
	    time_limit = (long)tv_get_സംഖ്യ_chk(&argvars[3], ശൂന്യ);
	    എങ്കിൽ (time_limit < 0)
		പോകുക theend;
#endif
	    use_skip = eval_വ്യഞ്ജകം_valid_arg(&argvars[4]);
	}
    }

    /*
     * This function does not accept SP_REPEAT and SP_RETCOUNT flags.
     * Check to make sure only those flags are set.
     * Also, Only the SP_NOMOVE or the SP_SETPCMARK flag can be set. Both
     * flags cannot be set. Check for that condition also.
     */
    എങ്കിൽ (((flags & (SP_REPEAT | SP_RETCOUNT)) != 0)
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	പോകുക theend;
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
    വേണ്ടി (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, ശൂന്യ, dir, pat, patlen, 1L,
						     options, RE_തിരയൽ, &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	എങ്കിൽ (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	എങ്കിൽ (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    നിർത്തുക;
	എങ്കിൽ (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    പൂർണ്ണസംഖ്യ	    do_skip;
	    പൂർണ്ണസംഖ്യ	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_വ്യഞ്ജകം_to_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    എങ്കിൽ (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		നിർത്തുക;
	    }
	    എങ്കിൽ (!do_skip)
		നിർത്തുക;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~തിരയൽ_START;
    }

    എങ്കിൽ (subpatnum != FAIL)
    {
	എങ്കിൽ (flags & SP_SUBPAT)
	    retval = subpatnum;
	അല്ലെങ്കിൽ
	    retval = pos.lnum;
	എങ്കിൽ (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	എങ്കിൽ (match_pos != ശൂന്യ)
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
    എങ്കിൽ (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    അല്ലെങ്കിൽ
	curwin->w_set_curswant = true;
theend:
    p_ws = save_p_ws;

    തിരികെ retval;
}

#ifdef FEAT_RUBY
/*
 * "rubyeval()" function
 */
    സ്ഥിതം ശൂന്യം
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#endif

/*
 * "screenattr()" function
 */
    സ്ഥിതം ശൂന്യം
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		row;
    പൂർണ്ണസംഖ്യ		col;
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    row = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ) - 1;
    col = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[1], ശൂന്യ) - 1;
    എങ്കിൽ (row < 0 || row >= screen_Rows
	    || col < 0 || col >= screen_Columns)
	c = -1;
    അല്ലെങ്കിൽ
	c = ScreenAttrs[വരി_Offset[row] + col];
    rettv->vval.v_സംഖ്യ = c;
}

/*
 * "screenchar()" function
 */
    സ്ഥിതം ശൂന്യം
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		row;
    പൂർണ്ണസംഖ്യ		col;
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    row = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ) - 1;
    col = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[1], ശൂന്യ) - 1;
    എങ്കിൽ (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	c = -1;
    അല്ലെങ്കിൽ
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(row, col, buf, ശൂന്യ);
	c = (*mb_ptr2char)(buf);
    }
    rettv->vval.v_സംഖ്യ = c;
}

/*
 * "screenchars()" function
 */
    സ്ഥിതം ശൂന്യം
f_screenchars(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		row;
    പൂർണ്ണസംഖ്യ		col;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    row = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ) - 1;
    col = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[1], ശൂന്യ) - 1;
    എങ്കിൽ (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	തിരികെ;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(row, col, buf, ശൂന്യ);
    പൂർണ്ണസംഖ്യ pcc[MAX_MCO];
    എങ്കിൽ (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    അല്ലെങ്കിൽ
	c = (*mb_ptr2char)(buf);
    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)c);

    എങ്കിൽ (enc_utf8)
	വേണ്ടി (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    സ്ഥിതം ശൂന്യം
f_screencol(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    സ്ഥിതം ശൂന്യം
f_screenrow(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    സ്ഥിതം ശൂന്യം
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		row;
    പൂർണ്ണസംഖ്യ		col;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = ശൂന്യ;
    rettv->v_type = VAR_STRING;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    row = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ) - 1;
    col = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[1], ശൂന്യ) - 1;
    എങ്കിൽ (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	തിരികെ;

    screen_getbytes(row, col, buf, ശൂന്യ);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    സ്ഥിതം ശൂന്യം
f_തിരയൽ(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		flags = 0;

    rettv->vval.v_സംഖ്യ = തിരയൽ_cmn(argvars, ശൂന്യ, &flags);
}

/*
 * "searchdecl()" function
 */
    സ്ഥിതം ശൂന്യം
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		locally = TRUE;
    പൂർണ്ണസംഖ്യ		thisblock = FALSE;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    char_u	*name;

    rettv->vval.v_സംഖ്യ = 1;	// default: FAIL

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	തിരികെ;

    name = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[1], &error);
	എങ്കിൽ (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[2], &error);
    }
    എങ്കിൽ (!error && name != ശൂന്യ)
	rettv->vval.v_സംഖ്യ = find_decl(name, (പൂർണ്ണസംഖ്യ)STRLEN(name),
				     locally, thisblock, തിരയൽ_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
searchpair_cmn(typval_T *argvars, pos_T *match_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    പൂർണ്ണസംഖ്യ		save_p_ws = p_ws;
    പൂർണ്ണസംഖ്യ		dir;
    പൂർണ്ണസംഖ്യ		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		retval = 0;		// default: FAIL
    long	lnum_stop = 0;
    long	time_limit = 0;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_സംഖ്യ_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_സംഖ്യ_arg(argvars, 6) == FAIL)))))
	പോകുക theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    എങ്കിൽ (spat == ശൂന്യ || mpat == ശൂന്യ || epat == ശൂന്യ)
	പോകുക theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_തിരയൽ_arg(&argvars[3], &flags); // may set p_ws
    എങ്കിൽ (dir == 0)
	പോകുക theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    എങ്കിൽ ((flags & (SP_END | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[3]));
	പോകുക theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    എങ്കിൽ (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    എങ്കിൽ (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	// Type is checked later.
	skip = &argvars[4];

	എങ്കിൽ (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (long)tv_get_സംഖ്യ_chk(&argvars[5], ശൂന്യ);
	    എങ്കിൽ (lnum_stop < 0)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[5]));
		പോകുക theend;
	    }
#ifdef FEAT_RELTIME
	    എങ്കിൽ (argvars[6].v_type != VAR_UNKNOWN)
	    {
		time_limit = (long)tv_get_സംഖ്യ_chk(&argvars[6], ശൂന്യ);
		എങ്കിൽ (time_limit < 0)
		{
		    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[6]));
		    പോകുക theend;
		}
	    }
#endif
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    match_pos, lnum_stop, time_limit);

theend:
    p_ws = save_p_ws;

    തിരികെ retval;
}

/*
 * "searchpair()" function
 */
    സ്ഥിതം ശൂന്യം
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = searchpair_cmn(argvars, ശൂന്യ);
}

/*
 * "searchpairpos()" function
 */
    സ്ഥിതം ശൂന്യം
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    പൂർണ്ണസംഖ്യ		lnum = 0;
    പൂർണ്ണസംഖ്യ		col = 0;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (searchpair_cmn(argvars, &match_pos) > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)col);
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
    പൂർണ്ണസംഖ്യ		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    പൂർണ്ണസംഖ്യ		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*match_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    long	time_limit UNUSED) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = ശൂന്യ, *pat3 = ശൂന്യ;
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
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		r;
    പൂർണ്ണസംഖ്യ		nest = 1;
    പൂർണ്ണസംഖ്യ		use_skip = FALSE;
    പൂർണ്ണസംഖ്യ		err;
    പൂർണ്ണസംഖ്യ		options = തിരയൽ_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    എങ്കിൽ (pat2 == ശൂന്യ)
	പോകുക theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    എങ്കിൽ (pat3 == ശൂന്യ)
	പോകുക theend;
    pat2len = vim_snprintf((അക്ഷരം *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    എങ്കിൽ (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    അല്ലെങ്കിൽ
	pat3len = vim_snprintf((അക്ഷരം *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    എങ്കിൽ (flags & SP_START)
	options |= തിരയൽ_START;

    എങ്കിൽ (skip != ശൂന്യ)
	use_skip = eval_വ്യഞ്ജകം_valid_arg(skip);

#ifdef FEAT_RELTIME
    എങ്കിൽ (time_limit > 0)
	init_regexp_timeout(time_limit);
#endif
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    വേണ്ടി (;;)
    {
	searchit_arg_T sia;

	CLEAR_FIELD(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, ശൂന്യ, dir, pat, patlen, 1L,
						     options, RE_തിരയൽ, &sia);
	എങ്കിൽ (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    നിർത്തുക;

	എങ്കിൽ (firstpos.lnum == 0)
	    firstpos = pos;
	എങ്കിൽ (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    എങ്കിൽ (dir == BACKWARD)
		decl(&pos);
	    അല്ലെങ്കിൽ
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~തിരയൽ_START;

	// If the skip pattern matches, ignore this match.
	എങ്കിൽ (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_വ്യഞ്ജകം_to_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    എങ്കിൽ (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		നിർത്തുക;
	    }
	    എങ്കിൽ (r)
		തുടരുക;
	}

	എങ്കിൽ ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	അല്ലെങ്കിൽ
	{
	    // Found end when searching forward or start when searching
	    // backward: end of (nested) pair; or found middle in outer pair.
	    എങ്കിൽ (--nest == 1)
		pat = pat3;	// outer level, search for middle
	}

	എങ്കിൽ (nest == 0)
	{
	    // Found the match: return matchcount or line number.
	    എങ്കിൽ (flags & SP_RETCOUNT)
		++retval;
	    അല്ലെങ്കിൽ
		retval = pos.lnum;
	    എങ്കിൽ (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    എങ്കിൽ (!(flags & SP_REPEAT))
		നിർത്തുക;
	    nest = 1;	    // search for next unmatched
	}
    }

    എങ്കിൽ (match_pos != ശൂന്യ)
    {
	// Store the match cursor position
	match_pos->lnum = curwin->w_cursor.lnum;
	match_pos->col = curwin->w_cursor.col + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    എങ്കിൽ ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#ifdef FEAT_RELTIME
    എങ്കിൽ (time_limit > 0)
	disable_regexp_timeout();
#endif
    vim_free(pat2);
    vim_free(pat3);
    എങ്കിൽ (p_cpo == empty_option)
	p_cpo = save_cpo;
    അല്ലെങ്കിൽ
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	എങ്കിൽ (*p_cpo == NUL)
	    set_option_മൂല്യം_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    തിരികെ retval;
}

/*
 * "searchpos()" function
 */
    സ്ഥിതം ശൂന്യം
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    പൂർണ്ണസംഖ്യ		lnum = 0;
    പൂർണ്ണസംഖ്യ		col = 0;
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		flags = 0;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    n = തിരയൽ_cmn(argvars, &match_pos, &flags);
    എങ്കിൽ (n > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)col);
    എങ്കിൽ (flags & SP_SUBPAT)
	list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    സ്ഥിതം ശൂന്യം
set_position(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ charpos)
{
    pos_T	pos;
    പൂർണ്ണസംഖ്യ		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	തിരികെ;

    name = tv_get_string_chk(argvars);
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	തിരികെ;

    എങ്കിൽ (pos.col != MAXCOL && --pos.col < 0)
	pos.col = 0;
    എങ്കിൽ ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	എങ്കിൽ (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_set_curswant = false;
	}
	check_cursor();
	rettv->vval.v_സംഖ്യ = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	എങ്കിൽ (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_സംഖ്യ = 0;
    }
    അല്ലെങ്കിൽ
	emsg(_(e_invalid_argument));
}
/*
 * "setcharpos()" function
 */
    സ്ഥിതം ശൂന്യം
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, TRUE);
}

    സ്ഥിതം ശൂന്യം
f_setcharsearch(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    എങ്കിൽ (check_for_dict_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ ((d = argvars[0].vval.v_dict) == ശൂന്യ)
	തിരികെ;

    csearch = dict_get_string(d, "char", FALSE);
    എങ്കിൽ (csearch != ശൂന്യ)
    {
	എങ്കിൽ (enc_utf8)
	{
	    പൂർണ്ണസംഖ്യ pcc[MAX_MCO];
	    പൂർണ്ണസംഖ്യ c = utfc_ptr2char(csearch, pcc);

	    set_last_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	അല്ലെങ്കിൽ
	    set_last_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_find(d, (char_u *)"forward", -1);
    എങ്കിൽ (di != ശൂന്യ)
	set_csearch_direction((പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_find(d, (char_u *)"until", -1);
    എങ്കിൽ (di != ശൂന്യ)
	set_csearch_until(!!tv_get_സംഖ്യ(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    സ്ഥിതം ശൂന്യം
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    സ്ഥിതം ശൂന്യം
f_setenv(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    name = tv_get_string_buf(&argvars[0], namebuf);
    എങ്കിൽ (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_സംഖ്യ == VVAL_NULL)
	vim_unsetenv_ext(name);
    അല്ലെങ്കിൽ
	vim_setenv_ext(name, tv_get_string_buf(&argvars[1], valbuf));
}

/*
 * "setfperm({fname}, {mode})" function
 */
    സ്ഥിതം ശൂന്യം
f_setfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	modebuf[NUMBUFLEN];
    char_u	*mode_str;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		mask;
    പൂർണ്ണസംഖ്യ		mode = 0;

    rettv->vval.v_സംഖ്യ = 0;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    fname = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ;
    mode_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    എങ്കിൽ (mode_str == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (STRLEN(mode_str) != 9)
    {
	semsg(_(e_invalid_argument_str), mode_str);
	തിരികെ;
    }

    mask = 1;
    വേണ്ടി (i = 8; i >= 0; --i)
    {
	എങ്കിൽ (mode_str[i] != '-')
	    mode |= mask;
	mask = mask << 1;
    }
    rettv->vval.v_സംഖ്യ = mch_setperm(fname, mode) == OK;
}

/*
 * "setpos()" function
 */
    സ്ഥിതം ശൂന്യം
f_setpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    പൂർണ്ണസംഖ്യ
get_yank_type(char_u **pp, char_u *yank_type, long *block_len)
{
    char_u *stropt = *pp;
    തിരഞ്ഞെടുപ്പ് (*stropt)
    {
	അവസ്ഥ 'v': അവസ്ഥ 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    നിർത്തുക;
	അവസ്ഥ 'V': അവസ്ഥ 'l':	// line-wise selection
	    *yank_type = MLINE;
	    നിർത്തുക;
	അവസ്ഥ 'b': അവസ്ഥ Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    എങ്കിൽ (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*block_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    തിരികെ FAIL;
    }
    *pp = stropt;
    തിരികെ OK;
}

/*
 * "setreg()" function
 */
    സ്ഥിതം ശൂന്യം
f_setreg(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    പൂർണ്ണസംഖ്യ		append;
    char_u	yank_type;
    long	block_len;
    typval_T	*regcontents;
    പൂർണ്ണസംഖ്യ		pointreg;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	തിരികെ;

    pointreg = 0;
    regcontents = ശൂന്യ;
    block_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_സംഖ്യ = 1;		// FAIL is default

    എങ്കിൽ (strregname == ശൂന്യ)
	തിരികെ;		// type error; errmsg already given
    എങ്കിൽ (in_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_register_name_must_be_one_char_str), strregname);
	തിരികെ;
    }
    regname = *strregname;
    എങ്കിൽ (regname == 0 || regname == '@')
	regname = '"';

    എങ്കിൽ (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	എങ്കിൽ (d == ശൂന്യ || d->dv_hashtab.ht_used == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {ശൂന്യ, ശൂന്യ};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    തിരികെ;
	}

	di = dict_find(d, (char_u *)"regcontents", -1);
	എങ്കിൽ (di != ശൂന്യ)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	എങ്കിൽ (stropt != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ ret = get_yank_type(&stropt, &yank_type, &block_len);

	    എങ്കിൽ (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_മൂല്യം_for_argument_str), "value");
		തിരികെ;
	    }
	}

	എങ്കിൽ (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    എങ്കിൽ (stropt != ശൂന്യ)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    അല്ലെങ്കിൽ
	regcontents = &argvars[1];

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	എങ്കിൽ (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    തിരികെ;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	എങ്കിൽ (stropt == ശൂന്യ)
	    തിരികെ;		// type error
	വേണ്ടി (; *stropt != NUL; ++stropt)
	    തിരഞ്ഞെടുപ്പ് (*stropt)
	    {
		അവസ്ഥ 'a': അവസ്ഥ 'A':	// append
		    append = TRUE;
		    നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    get_yank_type(&stropt, &yank_type, &block_len);
	    }
    }

    എങ്കിൽ (regcontents && regcontents->v_type == VAR_LIST)
    {
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	list_T		*ll = regcontents->vval.v_list;
	listitem_T	*li;
	പൂർണ്ണസംഖ്യ		len;

	// If the list is NULL handle like an empty list.
	len = ll == ശൂന്യ ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	എങ്കിൽ (lstval == ശൂന്യ)
	    തിരികെ;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	എങ്കിൽ (ll != ശൂന്യ)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_ഇനങ്ങൾ(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		എങ്കിൽ (strval == ശൂന്യ)
		    പോകുക free_lstval;
		എങ്കിൽ (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    എങ്കിൽ (strval == ശൂന്യ)
			പോകുക free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ശൂന്യ;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, block_len);
free_lstval:
	ആകുമ്പോൾ (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	എങ്കിൽ (strval == ശൂന്യ)
	    തിരികെ;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, block_len);
    }
    എങ്കിൽ (pointreg != 0)
	get_yank_register(pointreg, TRUE);

    rettv->vval.v_സംഖ്യ = 0;
}

/*
 * "settagstack()" function
 */
    സ്ഥിതം ശൂന്യം
f_settagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    dict_T	*d;
    പൂർണ്ണസംഖ്യ		action = 'r';

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	തിരികെ;

    // first argument: window number or id
    wp = find_win_by_nr_or_id(&argvars[0]);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;

    // second argument: dict with items to set in the tag stack
    എങ്കിൽ (check_for_dict_arg(argvars, 1) == FAIL)
	തിരികെ;
    d = argvars[1].vval.v_dict;
    എങ്കിൽ (d == ശൂന്യ)
	തിരികെ;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    എങ്കിൽ (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    അല്ലെങ്കിൽ എങ്കിൽ (check_for_string_arg(argvars, 2) == FAIL)
	തിരികെ;
    അല്ലെങ്കിൽ
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	എങ്കിൽ (actstr == ശൂന്യ)
	    തിരികെ;
	എങ്കിൽ ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	അല്ലെങ്കിൽ
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    തിരികെ;
	}
    }

    എങ്കിൽ (set_tagstack(wp, d, action) == OK)
	rettv->vval.v_സംഖ്യ = 0;
}

#ifdef FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    സ്ഥിതം ശൂന്യം
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ (in_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != ശൂന്യ ? (char_u *)blob->bv_ga.ga_data : (char_u *)"";
	len = blob != ശൂന്യ ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_bytes(p, len, ശൂന്യ, 0));
    }
    അല്ലെങ്കിൽ
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_bytes(p, (പൂർണ്ണസംഖ്യ)STRLEN(p), ശൂന്യ, 0));
    }
}
#endif // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    സ്ഥിതം ശൂന്യം
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ do_special;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	തിരികെ;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    സ്ഥിതം ശൂന്യം
f_shiftwidth(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = 0;

    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	long	col;

	col = (long)tv_get_സംഖ്യ_chk(argvars, ശൂന്യ);
	എങ്കിൽ (col < 0)
	    തിരികെ;	// type error; errmsg already given
#ifdef FEAT_VARTABS
	rettv->vval.v_സംഖ്യ = get_sw_മൂല്യം_col(curbuf, col, FALSE);
	തിരികെ;
#endif
    }

    rettv->vval.v_സംഖ്യ = get_sw_മൂല്യം(curbuf);
}

/*
 * "soundfold({word})" function
 */
    സ്ഥിതം ശൂന്യം
f_soundfold(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#ifdef FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#അല്ലെങ്കിൽ
    rettv->vval.v_string = vim_strsave(s);
#endif
}

/*
 * "spellbadword()" function
 */
    സ്ഥിതം ശൂന്യം
f_spellbadword(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*word = (char_u *)"";
    hlf_T	attr = HLF_എണ്ണം;
    പൂർണ്ണസംഖ്യ		len = 0;
#ifdef FEAT_SPELL
    പൂർണ്ണസംഖ്യ		wo_spell_save = curwin->w_p_spell;

    എങ്കിൽ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    എങ്കിൽ (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	തിരികെ;
    }
#endif

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	തിരികെ;
    }

#ifdef FEAT_SPELL
    എങ്കിൽ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_to(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	എങ്കിൽ (len != 0)
	{
	    word = ml_get_cursor();
	    curwin->w_set_curswant = true;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	പൂർണ്ണസംഖ്യ	capcol = -1;

	എങ്കിൽ (str != ശൂന്യ)
	{
	    // Check the argument for spelling.
	    ആകുമ്പോൾ (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		എങ്കിൽ (attr != HLF_എണ്ണം)
		{
		    word = str;
		    നിർത്തുക;
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
    തിരഞ്ഞെടുപ്പ് (attr)
    {
    അവസ്ഥ HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	നിർത്തുക;

    അവസ്ഥ HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	നിർത്തുക;

    അവസ്ഥ HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	നിർത്തുക;

    അവസ്ഥ HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	നിർത്തുക;
    }
}

/*
 * "spellsuggest()" function
 */
    സ്ഥിതം ശൂന്യം
f_spellsuggest(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SPELL
    char_u	*str;
    പൂർണ്ണസംഖ്യ		typeerr = FALSE;
    പൂർണ്ണസംഖ്യ		maxcount;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		i;
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ		need_capital = FALSE;
    പൂർണ്ണസംഖ്യ		wo_spell_save = curwin->w_p_spell;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	തിരികെ;

    എങ്കിൽ (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    എങ്കിൽ (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	തിരികെ;
    }
#endif

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	തിരികെ;
    }

#ifdef FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[1], &typeerr);
	എങ്കിൽ (maxcount <= 0)
	    തിരികെ;
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[2], &typeerr);
	    എങ്കിൽ (typeerr)
		തിരികെ;
	}
    }
    അല്ലെങ്കിൽ
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    വേണ്ടി (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_data)[i];

	li = listitem_alloc();
	എങ്കിൽ (li == ശൂന്യ)
	    vim_free(str);
	അല്ലെങ്കിൽ
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

    സ്ഥിതം ശൂന്യം
f_split(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*end;
    char_u	*pat = ശൂന്യ;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    പൂർണ്ണസംഖ്യ		match;
    colnr_T	col = 0;
    പൂർണ്ണസംഖ്യ		keepempty = FALSE;
    പൂർണ്ണസംഖ്യ		typeerr = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	തിരികെ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    str = tv_get_string(&argvars[0]);
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	എങ്കിൽ (pat == ശൂന്യ)
	    typeerr = TRUE;
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    എങ്കിൽ (pat == ശൂന്യ || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	പോകുക theend;
    എങ്കിൽ (typeerr)
	പോകുക theend;

    regmatch.regprog = eval_regcomp(pat);
    എങ്കിൽ (regmatch.regprog != ശൂന്യ)
    {
	regmatch.rm_ic = FALSE;
	ആകുമ്പോൾ (*str != NUL || keepempty)
	{
	    എങ്കിൽ (*str == NUL)
		match = FALSE;	// empty item at the end
	    അല്ലെങ്കിൽ
		match = vim_regexec_nl(&regmatch, str, col);
	    എങ്കിൽ (match)
		end = regmatch.startp[0];
	    അല്ലെങ്കിൽ
		end = str + STRLEN(str);
	    എങ്കിൽ (keepempty || end > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && match && end < regmatch.endp[0]))
	    {
		എങ്കിൽ (list_append_string(rettv->vval.v_list, str,
						    (പൂർണ്ണസംഖ്യ)(end - str)) == FAIL)
		    നിർത്തുക;
	    }
	    എങ്കിൽ (!match)
		നിർത്തുക;
	    // Advance to just after the match.
	    എങ്കിൽ (regmatch.endp[0] > str)
		col = 0;
	    അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
f_submatch(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		error = FALSE;
    പൂർണ്ണസംഖ്യ		no;
    പൂർണ്ണസംഖ്യ		retList = 0;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	തിരികെ;

    no = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[0], &error);
    എങ്കിൽ (error)
	തിരികെ;
    എങ്കിൽ (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_സംഖ്യ_nr), no);
	തിരികെ;
    }
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	retList = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[1], &error);
    എങ്കിൽ (error)
	തിരികെ;

    എങ്കിൽ (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    അല്ലെങ്കിൽ
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = reg_submatch_list(no);
    }
}

/*
 * "substitute()" function
 */
    സ്ഥിതം ശൂന്യം
f_substitute(typval_T *argvars, typval_T *rettv)
{
    char_u	patbuf[NUMBUFLEN];
    char_u	subbuf[NUMBUFLEN];
    char_u	flagsbuf[NUMBUFLEN];
    char_u	*str;
    char_u	*pat;
    char_u	*sub = ശൂന്യ;
    typval_T	*വ്യഞ്ജകം = ശൂന്യ;
    char_u	*flg;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	തിരികെ;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    എങ്കിൽ (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	വ്യഞ്ജകം = &argvars[2];
    അല്ലെങ്കിൽ
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    എങ്കിൽ (str == ശൂന്യ || pat == ശൂന്യ || (sub == ശൂന്യ && വ്യഞ്ജകം == ശൂന്യ)
								|| flg == ശൂന്യ)
	rettv->vval.v_string = ശൂന്യ;
    അല്ലെങ്കിൽ
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, വ്യഞ്ജകം, flg, ശൂന്യ);
}

/*
 * "swapfilelist()" function
 */
    സ്ഥിതം ശൂന്യം
f_swapfilelist(typval_T *argvars UNUSED, typval_T *rettv)
{
    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;
    recover_names(ശൂന്യ, FALSE, rettv->vval.v_list, 0, ശൂന്യ);
}

/*
 * "swapinfo(swap_filename)" function
 */
    സ്ഥിതം ശൂന്യം
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (rettv_dict_alloc(rettv) == OK)
	get_b0_dict(tv_get_string(argvars), rettv->vval.v_dict);
}

/*
 * "swapname(expr)" function
 */
    സ്ഥിതം ശൂന്യം
f_swapname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = tv_get_buf(&argvars[0], FALSE);
    എങ്കിൽ (buf == ശൂന്യ || buf->b_ml.ml_mfp == ശൂന്യ
					|| buf->b_ml.ml_mfp->mf_fname == ശൂന്യ)
	rettv->vval.v_string = ശൂന്യ;
    അല്ലെങ്കിൽ
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    സ്ഥിതം ശൂന്യം
f_synID(typval_T *argvars UNUSED, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		id = 0;
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    പൂർണ്ണസംഖ്യ		trans;
    പൂർണ്ണസംഖ്യ		transerr = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	തിരികെ;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (linenr_T)tv_get_സംഖ്യ(&argvars[1]) - 1;	// -1 on type error
    trans = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[2], &transerr);

    എങ്കിൽ (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_വരി_എണ്ണം
	    && col >= 0 && col < (long)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, col, trans, ശൂന്യ, FALSE);
#endif

    rettv->vval.v_സംഖ്യ = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    സ്ഥിതം ശൂന്യം
f_synIDattr(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*p = ശൂന്യ;
#ifdef FEAT_SYN_HL
    പൂർണ്ണസംഖ്യ		id;
    char_u	*what;
    char_u	*mode;
    char_u	modebuf[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		modec;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	തിരികെ;

    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	mode = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(mode[0]);
	എങ്കിൽ (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    അല്ലെങ്കിൽ
    {
# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	എങ്കിൽ (USE_24BIT)
	    modec = 'g';
	അല്ലെങ്കിൽ
# endif
	    എങ്കിൽ (t_colors > 1)
		modec = 'c';
	    അല്ലെങ്കിൽ
		modec = 't';
    }

    തിരഞ്ഞെടുപ്പ് (TOLOWER_ASC(what[0]))
    {
	അവസ്ഥ 'b':
		എങ്കിൽ (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		അല്ലെങ്കിൽ					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		നിർത്തുക;

	അവസ്ഥ 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		നിർത്തുക;

	അവസ്ഥ 'i':
		എങ്കിൽ (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		അല്ലെങ്കിൽ					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		നിർത്തുക;

	അവസ്ഥ 'n':
		എങ്കിൽ (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		അല്ലെങ്കിൽ					// name
		    p = get_highlight_name_ext(ശൂന്യ, id - 1, FALSE);
		നിർത്തുക;

	അവസ്ഥ 'r':					// reverse
		p = highlight_has_attr(id, HL_INVERSE, modec);
		നിർത്തുക;

	അവസ്ഥ 's':
		എങ്കിൽ (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		അല്ലെങ്കിൽ എങ്കിൽ (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		അല്ലെങ്കിൽ					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		നിർത്തുക;

	അവസ്ഥ 'u':
		എങ്കിൽ (STRLEN(what) >= 9)
		{
		    എങ്കിൽ (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    അല്ലെങ്കിൽ എങ്കിൽ (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    അല്ലെങ്കിൽ എങ്കിൽ (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    അല്ലെങ്കിൽ എങ്കിൽ (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    അല്ലെങ്കിൽ
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		അല്ലെങ്കിൽ
							// ul
		    p = highlight_color(id, what, modec);
		നിർത്തുക;
    }

    എങ്കിൽ (p != ശൂന്യ)
	p = vim_strsave(p);
#endif
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    സ്ഥിതം ശൂന്യം
f_synIDtrans(typval_T *argvars UNUSED, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		id;

#ifdef FEAT_SYN_HL
    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]);

    എങ്കിൽ (id > 0)
	id = syn_get_final_id(id);
    അല്ലെങ്കിൽ
#endif
	id = 0;

    rettv->vval.v_സംഖ്യ = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    സ്ഥിതം ശൂന്യം
f_synconcealed(typval_T *argvars UNUSED, typval_T *rettv)
{
#എങ്കിൽ defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	col;
    പൂർണ്ണസംഖ്യ		syntax_flags = 0;
    പൂർണ്ണസംഖ്യ		cchar;
    പൂർണ്ണസംഖ്യ		matchid = 0;
    char_u	str[NUMBUFLEN];
#endif

    rettv_list_set(rettv, ശൂന്യ);

    എങ്കിൽ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

#എങ്കിൽ defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_സംഖ്യ(&argvars[1]) - 1;	// -1 on type error

    CLEAR_FIELD(str);

    എങ്കിൽ (rettv_list_alloc(rettv) == OK)
    {
	എങ്കിൽ (lnum >= 1 && lnum <= curbuf->b_ml.ml_വരി_എണ്ണം
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (ശൂന്യം)syn_get_id(curwin, lnum, col, FALSE, ശൂന്യ, FALSE);
	    syntax_flags = get_syntax_info(&matchid);

	    // get the conceal character
	    എങ്കിൽ ((syntax_flags & HL_CONCEAL) && curwin->w_p_cole < 3)
	    {
		cchar = syn_get_sub_char();
		എങ്കിൽ (cchar == NUL && curwin->w_p_cole == 1)
		    cchar = (curwin->w_lcs_chars.conceal == NUL) ? ' '
					: curwin->w_lcs_chars.conceal;
		എങ്കിൽ (cchar != NUL)
		{
		    എങ്കിൽ (has_mbyte)
			(*mb_char2bytes)(cchar, str);
		    അല്ലെങ്കിൽ
			str[0] = cchar;
		}
	    }
	}

	list_append_സംഖ്യ(rettv->vval.v_list,
					    (syntax_flags & HL_CONCEAL) != 0);
	// -1 to auto-determine strlen
	list_append_string(rettv->vval.v_list, str, -1);
	list_append_സംഖ്യ(rettv->vval.v_list, matchid);
    }
#endif
}

/*
 * "synstack(lnum, col)" function
 */
    സ്ഥിതം ശൂന്യം
f_synstack(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		id;
#endif

    rettv_list_set(rettv, ശൂന്യ);

    എങ്കിൽ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

#ifdef FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_സംഖ്യ(&argvars[1]) - 1;	// -1 on type error

    എങ്കിൽ (lnum >= 1 && lnum <= curbuf->b_ml.ml_വരി_എണ്ണം
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(ശൂന്യം)syn_get_id(curwin, lnum, col, FALSE, ശൂന്യ, TRUE);
	വേണ്ടി (i = 0; ; ++i)
	{
	    id = syn_get_അട്ടി_item(i);
	    എങ്കിൽ (id < 0)
		നിർത്തുക;
	    എങ്കിൽ (list_append_സംഖ്യ(rettv->vval.v_list, id) == FAIL)
		നിർത്തുക;
	}
    }
#endif
}

/*
 * "tabpagebuflist()" function
 */
    സ്ഥിതം ശൂന്യം
f_tabpagebuflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp = ശൂന്യ;

    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    അല്ലെങ്കിൽ
    {
	tp = find_tabpage((പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]));
	എങ്കിൽ (tp != ശൂന്യ)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    എങ്കിൽ (wp != ശൂന്യ && rettv_list_alloc(rettv) == OK)
    {
	വേണ്ടി (; wp != ശൂന്യ; wp = wp->w_next)
	    എങ്കിൽ (list_append_സംഖ്യ(rettv->vval.v_list,
						wp->w_buffer->b_fnum) == FAIL)
		നിർത്തുക;
    }
}

/*
 * "tagfiles()" function
 */
    സ്ഥിതം ശൂന്യം
f_tagfiles(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    പൂർണ്ണസംഖ്യ		first;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;
    fname = alloc(MAXPATHL);
    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ;

    വേണ്ടി (first = TRUE; ; first = FALSE)
	എങ്കിൽ (get_tagfname(&tn, first, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    നിർത്തുക;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    സ്ഥിതം ശൂന്യം
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = ശൂന്യ;
    char_u  *tag_pattern;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    tag_pattern = tv_get_string(&argvars[0]);

    rettv->vval.v_സംഖ്യ = FALSE;
    എങ്കിൽ (*tag_pattern == NUL)
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    എങ്കിൽ (rettv_list_alloc(rettv) == OK)
	(ശൂന്യം)get_tags(rettv->vval.v_list, tag_pattern, fname);
}

/*
 * "type(expr)" function
 */
    സ്ഥിതം ശൂന്യം
f_type(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ n = -1;

    തിരഞ്ഞെടുപ്പ് (argvars[0].v_type)
    {
	അവസ്ഥ VAR_സംഖ്യ:  n = VAR_TYPE_സംഖ്യ; നിർത്തുക;
	അവസ്ഥ VAR_STRING:  n = VAR_TYPE_STRING; നിർത്തുക;
	അവസ്ഥ VAR_PARTIAL:
	അവസ്ഥ VAR_FUNC:    n = VAR_TYPE_FUNC; നിർത്തുക;
	അവസ്ഥ VAR_LIST:    n = VAR_TYPE_LIST; നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:   n = VAR_TYPE_TUPLE; നിർത്തുക;
	അവസ്ഥ VAR_DICT:    n = VAR_TYPE_DICT; നിർത്തുക;
	അവസ്ഥ VAR_FLOAT:   n = VAR_TYPE_FLOAT; നിർത്തുക;
	അവസ്ഥ VAR_BOOL:	  n = VAR_TYPE_BOOL; നിർത്തുക;
	അവസ്ഥ VAR_SPECIAL: n = VAR_TYPE_NONE; നിർത്തുക;
	അവസ്ഥ VAR_JOB:     n = VAR_TYPE_JOB; നിർത്തുക;
	അവസ്ഥ VAR_CHANNEL: n = VAR_TYPE_CHANNEL; നിർത്തുക;
	അവസ്ഥ VAR_BLOB:    n = VAR_TYPE_BLOB; നിർത്തുക;
	അവസ്ഥ VAR_INSTR:   n = VAR_TYPE_INSTR; നിർത്തുക;
	അവസ്ഥ VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; നിർത്തുക;
	അവസ്ഥ VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		എങ്കിൽ (cl != ശൂന്യ && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		അല്ലെങ്കിൽ
		    n = VAR_TYPE_CLASS;
		നിർത്തുക;
	    }
	അവസ്ഥ VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		എങ്കിൽ (obj == ശൂന്യ)
		    n = VAR_TYPE_OBJECT;
		അല്ലെങ്കിൽ
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    എങ്കിൽ (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    അല്ലെങ്കിൽ
			n = VAR_TYPE_OBJECT;
		}
		നിർത്തുക;
	    }
	അവസ്ഥ VAR_VOID:
	    emsg(_(e_cannot_use_void_മൂല്യം));
	    നിർത്തുക;
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	    internal_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    നിർത്തുക;
    }
    rettv->vval.v_സംഖ്യ = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    സ്ഥിതം ശൂന്യം
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_start = 0;
    colnr_T	vcol_end = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    പൂർണ്ണസംഖ്യ		winchanged = FALSE;
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL))))
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[2]), &tp);
	എങ്കിൽ (wp == ശൂന്യ || tp == ശൂന്യ)
	    പോകുക theend;

	എങ്കിൽ (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    പോകുക theend;

	check_cursor();
	winchanged = TRUE;
    }

    പൂർണ്ണസംഖ്യ fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    എങ്കിൽ (fp != ശൂന്യ && fp->lnum <= curbuf->b_ml.ml_വരി_എണ്ണം
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	എങ്കിൽ (fp->col < 0)
	    fp->col = 0;
	അല്ലെങ്കിൽ
	{
	    len = (പൂർണ്ണസംഖ്യ)ml_get_len(fp->lnum);
	    എങ്കിൽ (fp->col > len)
		fp->col = len;
	}
	getvvcol(curwin, fp, &vcol_start, ശൂന്യ, &vcol_end, 0);
	++vcol_start;
	++vcol_end;
    }

theend:
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	എങ്കിൽ (rettv_list_alloc(rettv) == OK)
	{
	    list_append_സംഖ്യ(rettv->vval.v_list, vcol_start);
	    list_append_സംഖ്യ(rettv->vval.v_list, vcol_end);
	}
	അല്ലെങ്കിൽ
	    rettv->vval.v_സംഖ്യ = 0;
    }
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = vcol_end;

    എങ്കിൽ (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    സ്ഥിതം ശൂന്യം
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    എങ്കിൽ (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_mode_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    എങ്കിൽ (non_zero_arg(&argvars[0]))
	curbuf->b_visual_mode_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    സ്ഥിതം ശൂന്യം
f_wildmenumode(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    എങ്കിൽ (wild_menu_showing || ((State & MODE_CMDLINE) && cmdline_pum_active()))
	rettv->vval.v_സംഖ്യ = 1;
}

/*
 * "windowsversion()" function
 */
    സ്ഥിതം ശൂന്യം
f_windowsversion(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windowsVersion);
}

/*
 * "wordcount()" function
 */
    സ്ഥിതം ശൂന്യം
f_wordcount(typval_T *argvars UNUSED, typval_T *rettv)
{
    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    സ്ഥിതം ശൂന്യം
f_xor(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    rettv->vval.v_സംഖ്യ = tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ)
					^ tv_get_സംഖ്യ_chk(&argvars[1], ശൂന്യ);
}

#endif // FEAT_EVAL
