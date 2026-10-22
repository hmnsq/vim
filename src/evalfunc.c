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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_FLOAT_STUFF

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
# include <float.h>
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ f_and(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL
ਸਥਿਰ ਖਾਲੀ f_balloon_gettext(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_balloon_show(typval_T *argvars, typval_T *rettv);
# ਜੇ defined(FEAT_BEVAL_ਪਦ)
ਸਥਿਰ ਖਾਲੀ f_balloon_ਵੰਡੋ(typval_T *argvars, typval_T *rettv);
# endif
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ f_base64_encode(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_base64_decode(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_bindtextdomain(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_byte2line(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_call(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_changenr(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_char2nr(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_charcol(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_ਖਾਨਾ(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_confirm(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_copy(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_cursor(typval_T *argsvars, typval_T *rettv);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
ਸਥਿਰ ਖਾਲੀ f_debugbreak(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ f_deepcopy(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_did_filetype(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_echoraw(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_ਖਾਲੀ_ਥਾਵਾਂ(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_environ(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_err_teapot(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_escape(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_eval(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_eventhandler(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_execute(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_exists_compiled(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_expand(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_expandcmd(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_feedkeys(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_fnameescape(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_foreground(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_funcref(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_function(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_garbagecollect(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_get(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getbgcolor(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getcellpixels(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getchangelist(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getcharpos(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getcharsearch(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getcurpos(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getcursorcharpos(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getenv(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getfontname(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getinfo(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getjumplist(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getpid(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getpos(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getreg(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getreginfo(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getregion(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getregionpos(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_getregtype(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_gettagstack(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_gettext(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_haslocaldir(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_hlID(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_hlexists(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_hostname(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_id(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_ਸੂਚਕਾਂਕ(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_indexof(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_ਨਿਵੇਸ਼(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_inputdialog(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_inputlist(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_inputrestore(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_inputsave(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_inputsecret(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_ਰੁਕਾਵਟ(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_invert(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_islocked(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_ਚੀਜ਼ਾਂ(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_keytrans(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_ਆਖਰੀ_ਬਫਰ_nr(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_libcall(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_libcallnr(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_ਸਤਰ(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_line2byte(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LUA
ਸਥਿਰ ਖਾਲੀ f_luaeval(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ f_ਮੇਲ(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_matchbufline(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_matchend(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_matchlist(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_matchstr(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_matchstrlist(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_matchstrpos(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_max(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_min(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
ਸਥਿਰ ਖਾਲੀ f_mzeval(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ f_nextnonblank(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_ngettext(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_nr2char(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_or(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERL
ਸਥਿਰ ਖਾਲੀ f_perleval(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ f_prevnonblank(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_printf(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_pum_getpos(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_pumvisible(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
ਸਥਿਰ ਖਾਲੀ f_py3eval(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
ਸਥਿਰ ਖਾਲੀ f_pyeval(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
ਸਥਿਰ ਖਾਲੀ f_pyxeval(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ f_test_srand_ਬੀਜ(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_rand(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_range(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_reg_executing(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_reg_recording(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_rename(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_repeat(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RUBY
ਸਥਿਰ ਖਾਲੀ f_rubyeval(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ f_screenattr(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_screenchar(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_screenchars(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_screencol(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_screenrow(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_screenstring(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_ਖੋਜ(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_searchdecl(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_searchpair(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_searchpairpos(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_searchpos(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_setcharpos(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_setcharsearch(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_setcursorcharpos(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_setenv(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_setfperm(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_setpos(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_setreg(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_settagstack(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
ਸਥਿਰ ਖਾਲੀ f_sha256(typval_T *argvars, typval_T *rettv);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ f_shellescape(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_shiftwidth(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_soundfold(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_spellbadword(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_spellsuggest(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_ਵੰਡੋ(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_srand(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_submatch(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_substitute(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_swapfilelist(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_swapinfo(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_swapname(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_synID(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_synIDattr(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_synIDtrans(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_synstack(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_synconcealed(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_tabpagebuflist(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_taglist(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_tagfiles(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_type(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_virtcol(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_visualmode(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_wildmenumode(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_windowsversion(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_wordcount(typval_T *argvars, typval_T *rettv);
ਸਥਿਰ ਖਾਲੀ f_xor(typval_T *argvars, typval_T *rettv);


/*
 * Functions that check the argument type of a builtin function.
 * Each function returns FAIL and gives an error message if the type is wrong.
 */

// Context passed to an arg_ function.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		arg_ਗਿਣਤੀ;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    ਪੂਰਨ		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    ਸਥਿਰ ਪੂਰਨ
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    ਵਾਪਸ need_type(actual, expected, 0,
	    context->arg_idx - context->arg_ਗਿਣਤੀ, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    ਸਥਿਰ ਪੂਰਨ
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    ਜੇ (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_ਗਿਣਤੀ, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    ਪੂਰਨ
arg_type_modifiable(type_T *type, ਪੂਰਨ arg_idx)
{
    ਅੱਖਰ *tofree;

    ਜੇ ((type->tt_flags & TTFLAG_CONST) == 0)
	ਵਾਪਸ OK;
    semsg(_(e_ਦਲੀਲ_nr_trying_ਮੰਜ਼ਿਲ_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    ਵਾਪਸ FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_any(type_T *type ਅਣਵਰਤਿਆ,
	type_T *decl_type ਅਣਵਰਤਿਆ,
	argcontext_T *context ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ OK;
}

/*
 * Check "type" is a float or a number.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_float_or_nr(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_ਸੰਖਿਆ
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_ਸੰਖਿਆ, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a number.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_ਸੰਖਿਆ(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਵਾਪਸ check_arg_type(&t_ਸੰਖਿਆ, type, context);
}

/*
 * Check "type" is an object.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_object(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_dict_any(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਵਾਪਸ check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_any(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਵਾਪਸ check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type ਅਣਵਰਤਿਆ,
	argcontext_T *context)
{
    ਵਾਪਸ check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_ਸੰਖਿਆ(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਵਾਪਸ check_arg_type(&t_list_ਸੰਖਿਆ, type, context);
}

/*
 * Check "type" is a list of strings.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_string(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਵਾਪਸ check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_tuple_any(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਵਾਪਸ check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_string(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਵਾਪਸ check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    ਸਥਿਰ ਪੂਰਨ
arg_blob(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਵਾਪਸ check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_bool(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਵਾਪਸ check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_bool_or_nr(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_ਸੰਖਿਆ
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_ਸੰਖਿਆ, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    ਵਾਪਸ check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_or_blob(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ਜੇ (arg_list_or_blob(type, decl_type, context) == FAIL)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a string or a number
 */
    ਸਥਿਰ ਪੂਰਨ
arg_string_or_nr(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ਸੰਖਿਆ
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    ਸਥਿਰ ਪੂਰਨ
arg_ਬਫਰ(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ਸੰਖਿਆ
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    ਸਥਿਰ ਪੂਰਨ
arg_ਬਫਰ_or_dict_any(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ਸੰਖਿਆ
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    ਸਥਿਰ ਪੂਰਨ
arg_lnum(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ਸੰਖਿਆ
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    ਜੇ (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	ਵਾਪਸ FAIL;
    }
    ਜੇ (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	ਵਾਪਸ OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    ਸਥਿਰ ਪੂਰਨ
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    ਸਥਿਰ ਪੂਰਨ
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    ਸਥਿਰ ਪੂਰਨ
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type ਅਣਵਰਤਿਆ,
	argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ਜੇ (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ਜੇ (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_ਦਲੀਲ_nr),
						 context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    ਸਥਿਰ ਪੂਰਨ
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = ਨਿਰਾ;
    type_T *(ਦਲੀਲਾਂ[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, ਨਿਰਾ, ਨਿਰਾ, ਦਲੀਲਾਂ};

    ਜੇ (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	ਜੇ (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	ਨਹੀਂਤਾਂ
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    ਨਹੀਂਤਾਂ ਜੇ (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    ਨਹੀਂਤਾਂ ਜੇ (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_ਸੰਖਿਆ;

    ਦਲੀਲਾਂ[0] = ਨਿਰਾ;
    ਦਲੀਲਾਂ[1] = &t_unknown;
    ਜੇ (type->tt_argcount != -1)
    {
	ਜੇ (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_ਸੰਖਿਆ_ਦਾ_arguments));
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		ਦਲੀਲਾਂ[0] = &t_ਸੰਖਿਆ;
	    ਨਹੀਂਤਾਂ ਜੇ (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		ਦਲੀਲਾਂ[0] = &t_string;
	    ਜੇ (ਦਲੀਲਾਂ[0] != ਨਿਰਾ)
		ਦਲੀਲਾਂ[1] = expected_member;
	}
    }

    ਜੇ (!type_any_or_unknown(type->tt_member) || ਦਲੀਲਾਂ[0] != ਨਿਰਾ)
    {
	where_T where = WHERE_INIT;

	ਜੇ (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == ਨਿਰਾ
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	ਨਹੀਂਤਾਂ ਜੇ (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	ਨਹੀਂਤਾਂ // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	ਜੇ (ਦਲੀਲਾਂ[0] == ਨਿਰਾ)
	    ਦਲੀਲਾਂ[0] = &t_unknown;
	ਜੇ (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_ਸੂਚਕਾਂਕ = 2;
	where.wt_ਕਿਸਮ = WT_ਦਲੀਲ;
	ਵਾਪਸ check_type(&t_func_exp, type, TRUE, where);
    }
    ਵਾਪਸ OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_filter_func(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;

    ਜੇ (type->tt_type == VAR_FUNC)
	ਵਾਪਸ check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_ਦਲੀਲ_nr), 2);
    ਵਾਪਸ FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_map_func(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;

    ਜੇ (type->tt_type == VAR_FUNC)
	ਵਾਪਸ check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_ਦਲੀਲ_nr), 2);
    ਵਾਪਸ FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_foreach_func(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;

    ਜੇ (type->tt_type == VAR_FUNC)
	ਵਾਪਸ check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_ਦਲੀਲ_nr), 2);
    ਵਾਪਸ FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_ਛਾਂਟੋ_how(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;

    ਜੇ (type->tt_type == VAR_FUNC)
    {
	type_T *(ਦਲੀਲਾਂ[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_ਸੰਖਿਆ, ਨਿਰਾ, ਦਲੀਲਾਂ};

	ਜੇ (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    ਦਲੀਲਾਂ[0] = context->arg_types[0].type_curr->tt_member;
	ਨਹੀਂਤਾਂ
	    ਦਲੀਲਾਂ[0] = &t_unknown;
	ਜੇ (!type_any_or_unknown(type->tt_member) || ਦਲੀਲਾਂ[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    ਦਲੀਲਾਂ[1] = ਦਲੀਲਾਂ[0];
	    ਜੇ (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_ਸੂਚਕਾਂਕ = 2;
	    where.wt_ਕਿਸਮ = WT_ਦਲੀਲ;
	    ਵਾਪਸ check_type(&t_func_exp, type, TRUE, where);
	}

	ਵਾਪਸ OK;
    }
    semsg(_(e_string_or_function_required_for_ਦਲੀਲ_nr), 2);
    ਵਾਪਸ FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_ਸੰਖਿਆ
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    ਸਥਿਰ ਪੂਰਨ
varargs_class(type_T *type ਅਣਵਰਤਿਆ,
	      type_T *decl_type ਅਣਵਰਤਿਆ,
	      argcontext_T *context)
{
    ਲਈ (ਪੂਰਨ i = context->arg_idx; i < context->arg_ਗਿਣਤੀ; ++i)
    {
	type2_T *types = &context->arg_types[i];
	ਜੇ (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_ਦਲੀਲ_nr), i + 1);
	    ਵਾਪਸ FAIL;
	}
    }
    ਵਾਪਸ OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_ਉਲਟ(type_T *type, type_T *decl_type, argcontext_T *context)
{
    ਜੇ (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_ਕੰਮ(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਵਾਪਸ check_arg_type(&t_ਕੰਮ, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_chan_or_ਕੰਮ(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_ਕੰਮ
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_same_as_prev(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_decl;

    ਵਾਪਸ check_arg_type(prev_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_same_struct_as_prev(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;

    ਜੇ (prev_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	ਵਾਪਸ check_arg_type(prev_type, type, context);
    ਵਾਪਸ OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    ਸਥਿਰ ਪੂਰਨ
arg_ਵਸਤੂ_ਦਾ_prev(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    ਜੇ (prev_type->tt_type == VAR_LIST)
	expected = prev_type->tt_member;
    ਨਹੀਂਤਾਂ ਜੇ (prev_type->tt_type == VAR_BLOB)
	expected = &t_ਸੰਖਿਆ;
    ਨਹੀਂਤਾਂ
	// probably VAR_ANY, can't check
	ਵਾਪਸ OK;

    ਵਾਪਸ check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    ਸਥਿਰ ਪੂਰਨ
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ਸੰਖਿਆ
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    ਸਥਿਰ ਪੂਰਨ
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    ਸਥਿਰ ਪੂਰਨ
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *ਪਹਿਲਾ_type = context->arg_types[context->arg_idx - 2].type_curr;

    ਜੇ (ਪਹਿਲਾ_type->tt_type == VAR_LIST || ਪਹਿਲਾ_type->tt_type == VAR_BLOB)
	ਵਾਪਸ arg_ਸੰਖਿਆ(type, decl_type, context);
    ਜੇ (ਪਹਿਲਾ_type->tt_type == VAR_DICT)
	ਵਾਪਸ arg_string(type, decl_type, context);
    ਵਾਪਸ OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    ਸਥਿਰ ਪੂਰਨ
arg_get1(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    ਸਥਿਰ ਪੂਰਨ
arg_len1(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ਸੰਖਿਆ
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    ਸਥਿਰ ਪੂਰਨ
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *ਪਹਿਲਾ_type = context->arg_types[context->arg_idx - 1].type_curr;

    ਜੇ (ਪਹਿਲਾ_type->tt_type == VAR_LIST || ਪਹਿਲਾ_type->tt_type == VAR_BLOB)
	ਵਾਪਸ arg_ਸੰਖਿਆ(type, decl_type, context);
    ਜੇ (ਪਹਿਲਾ_type->tt_type == VAR_DICT)
	ਵਾਪਸ arg_string_or_nr(type, decl_type, context);
    ਵਾਪਸ OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    ਸਥਿਰ ਪੂਰਨ
arg_repeat1(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ਸੰਖਿਆ
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    ਸਥਿਰ ਪੂਰਨ
arg_slice1(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    ਸਥਿਰ ਪੂਰਨ
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type ਅਣਵਰਤਿਆ,
    argcontext_T	*context)
{
    ਜੇ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_ਦਲੀਲ_nr),
							 context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    ਸਥਿਰ ਪੂਰਨ
arg_cursor1(type_T *type, type_T *decl_type ਅਣਵਰਤਿਆ, argcontext_T *context)
{
    ਜੇ (type->tt_type == VAR_ਸੰਖਿਆ
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ਵਾਪਸ OK;

    arg_type_mismatch(&t_ਸੰਖਿਆ, type, context->arg_idx + 1);
    ਵਾਪਸ FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
ਸਥਿਰ argcheck_T arg1_blob[] = {arg_blob};
ਸਥਿਰ argcheck_T arg1_bool[] = {arg_bool};
ਸਥਿਰ argcheck_T arg1_ਬਫਰ[] = {arg_ਬਫਰ};
ਸਥਿਰ argcheck_T arg1_ਬਫਰ_or_dict_any[] = {arg_ਬਫਰ_or_dict_any};
ਸਥਿਰ argcheck_T arg1_chan_or_ਕੰਮ[] = {arg_chan_or_ਕੰਮ};
ਸਥਿਰ argcheck_T arg1_dict_any[] = {arg_dict_any};
ਸਥਿਰ argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
ਸਥਿਰ argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
ਸਥਿਰ argcheck_T arg1_ਕੰਮ[] = {arg_ਕੰਮ};
ਸਥਿਰ argcheck_T arg1_list_any[] = {arg_list_any};
ਸਥਿਰ argcheck_T arg1_tuple_any[] = {arg_tuple_any};
ਸਥਿਰ argcheck_T arg1_list_ਸੰਖਿਆ[] = {arg_list_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg1_ਉਲਟ[] = {arg_ਉਲਟ};
ਸਥਿਰ argcheck_T arg1_list_or_tuple_or_dict[] = {arg_list_or_tuple_or_dict};
ਸਥਿਰ argcheck_T arg1_list_string[] = {arg_list_string};
ਸਥਿਰ argcheck_T arg1_list_tuple_dict_blob_or_string[] = {arg_list_tuple_dict_blob_or_string};
ਸਥਿਰ argcheck_T arg1_lnum[] = {arg_lnum};
ਸਥਿਰ argcheck_T arg1_ਸੰਖਿਆ[] = {arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg1_string[] = {arg_string};
ਸਥਿਰ argcheck_T arg1_string_or_list_any[] = {arg_string_or_list_any};
ਸਥਿਰ argcheck_T arg1_string_or_list_string[] = {arg_string_or_list_string};
ਸਥਿਰ argcheck_T arg1_string_or_nr[] = {arg_string_or_nr};
ਸਥਿਰ argcheck_T arg1_string_or_blob[] = {arg_string_or_blob};
ਸਥਿਰ argcheck_T arg2_ਬਫਰ_any[] = {arg_ਬਫਰ, arg_any};
ਸਥਿਰ argcheck_T arg2_ਬਫਰ_bool[] = {arg_ਬਫਰ, arg_bool};
ਸਥਿਰ argcheck_T arg2_ਬਫਰ_list_any[] = {arg_ਬਫਰ, arg_list_any};
ਸਥਿਰ argcheck_T arg2_ਬਫਰ_lnum[] = {arg_ਬਫਰ, arg_lnum};
ਸਥਿਰ argcheck_T arg2_ਬਫਰ_ਸੰਖਿਆ[] = {arg_ਬਫਰ, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg2_ਬਫਰ_string[] = {arg_ਬਫਰ, arg_string};
ਸਥਿਰ argcheck_T arg2_chan_or_ਕੰਮ_dict[] = {arg_chan_or_ਕੰਮ, arg_dict_any};
ਸਥਿਰ argcheck_T arg2_chan_or_ਕੰਮ_string[] = {arg_chan_or_ਕੰਮ, arg_string};
ਸਥਿਰ argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
ਸਥਿਰ argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
ਸਥਿਰ argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
ਸਥਿਰ argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
ਸਥਿਰ argcheck_T arg2_ਕੰਮ_dict[] = {arg_ਕੰਮ, arg_dict_any};
ਸਥਿਰ argcheck_T arg2_ਕੰਮ_string_or_ਸੰਖਿਆ[] = {arg_ਕੰਮ, arg_string_or_nr};
ਸਥਿਰ argcheck_T arg2_list_any_ਸੰਖਿਆ[] = {arg_list_any, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg2_list_ਸੰਖਿਆ[] = {arg_list_ਸੰਖਿਆ, arg_list_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg2_list_ਸੰਖਿਆ_bool[] = {arg_list_ਸੰਖਿਆ, arg_bool};
ਸਥਿਰ argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
ਸਥਿਰ argcheck_T arg2_listblobmod_ਵਸਤੂ[] = {arg_list_or_blob_mod, arg_ਵਸਤੂ_ਦਾ_prev};
ਸਥਿਰ argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
ਸਥਿਰ argcheck_T arg2_lnum_ਸੰਖਿਆ[] = {arg_lnum, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg2_ਸੰਖਿਆ[] = {arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg2_ਸੰਖਿਆ_any[] = {arg_ਸੰਖਿਆ, arg_any};
ਸਥਿਰ argcheck_T arg2_ਸੰਖਿਆ_bool[] = {arg_ਸੰਖਿਆ, arg_bool};
ਸਥਿਰ argcheck_T arg2_ਸੰਖਿਆ_dict_any[] = {arg_ਸੰਖਿਆ, arg_dict_any};
ਸਥਿਰ argcheck_T arg2_ਸੰਖਿਆ_list[] = {arg_ਸੰਖਿਆ, arg_list_any};
ਸਥਿਰ argcheck_T arg2_ਸੰਖਿਆ_string[] = {arg_ਸੰਖਿਆ, arg_string};
ਸਥਿਰ argcheck_T arg2_ਸੰਖਿਆ_ਬਫਰ[] = {arg_ਸੰਖਿਆ, arg_ਬਫਰ};
ਸਥਿਰ argcheck_T arg2_ਸੰਖਿਆ_string_or_list[] = {arg_ਸੰਖਿਆ, arg_string_or_list_any};
ਸਥਿਰ argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
ਸਥਿਰ argcheck_T arg2_string[] = {arg_string, arg_string};
ਸਥਿਰ argcheck_T arg2_string_any[] = {arg_string, arg_any};
ਸਥਿਰ argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
ਸਥਿਰ argcheck_T arg2_string_chan_or_ਕੰਮ[] = {arg_string, arg_chan_or_ਕੰਮ};
ਸਥਿਰ argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
ਸਥਿਰ argcheck_T arg2_string_list_ਸੰਖਿਆ[] = {arg_string, arg_list_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg2_string_ਸੰਖਿਆ[] = {arg_string, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
ਸਥਿਰ argcheck_T arg2_string_or_list_ਸੰਖਿਆ[] = {arg_string_or_list_any, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg2_string_string_or_ਸੰਖਿਆ[] = {arg_string, arg_string_or_nr};
ਸਥਿਰ argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
ਸਥਿਰ argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
ਸਥਿਰ argcheck_T arg3_any_ਬਫਰ_bool_or_dict[] = {
			      arg_any, arg_ਬਫਰ, arg_bool_or_dict_any};
ਸਥਿਰ argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
ਸਥਿਰ argcheck_T arg3_ਬਫਰ_lnum_lnum[] = {arg_ਬਫਰ, arg_lnum, arg_lnum};
ਸਥਿਰ argcheck_T arg3_ਬਫਰ_ਸੰਖਿਆ_ਸੰਖਿਆ[] = {arg_ਬਫਰ, arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg3_ਬਫਰ_string_any[] = {arg_ਬਫਰ, arg_string, arg_any};
ਸਥਿਰ argcheck_T arg3_ਬਫਰ_string_dict[] = {arg_ਬਫਰ, arg_string, arg_dict_any};
ਸਥਿਰ argcheck_T arg3_dict_ਸੰਖਿਆ_ਸੰਖਿਆ[] = {arg_dict_any, arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
ਸਥਿਰ argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
ਸਥਿਰ argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
ਸਥਿਰ argcheck_T arg3_lnum_ਸੰਖਿਆ_bool[] = {arg_lnum, arg_ਸੰਖਿਆ, arg_bool};
ਸਥਿਰ argcheck_T arg3_ਸੰਖਿਆ[] = {arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg3_ਸੰਖਿਆ_any_dict[] = {arg_ਸੰਖਿਆ, arg_any, arg_dict_any};
ਸਥਿਰ argcheck_T arg3_ਸੰਖਿਆ_ਸੰਖਿਆ_dict[] = {arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ, arg_dict_any};
ਸਥਿਰ argcheck_T arg3_ਸੰਖਿਆ_string_any[] = {arg_ਸੰਖਿਆ, arg_string, arg_any};
ਸਥਿਰ argcheck_T arg3_ਸੰਖਿਆ_string_ਬਫਰ[] = {arg_ਸੰਖਿਆ, arg_string, arg_ਬਫਰ};
ਸਥਿਰ argcheck_T arg3_ਸੰਖਿਆ_string_string[] = {arg_ਸੰਖਿਆ, arg_string, arg_string};
ਸਥਿਰ argcheck_T arg3_string[] = {arg_string, arg_string, arg_string};
ਸਥਿਰ argcheck_T arg3_string_any_dict[] = {arg_string, arg_any, arg_dict_any};
ਸਥਿਰ argcheck_T arg3_string_any_string[] = {arg_string, arg_any, arg_string};
ਸਥਿਰ argcheck_T arg3_string_bool_bool[] = {arg_string, arg_bool, arg_bool};
ਸਥਿਰ argcheck_T arg3_string_ਸੰਖਿਆ_bool[] = {arg_string, arg_ਸੰਖਿਆ, arg_bool};
ਸਥਿਰ argcheck_T arg3_string_ਸੰਖਿਆ_ਸੰਖਿਆ[] = {arg_string, arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg3_string_or_dict_bool_dict[] = {arg_string_or_dict_any, arg_bool, arg_dict_any};
ਸਥਿਰ argcheck_T arg3_string_or_list_bool_ਸੰਖਿਆ[] = {arg_string_or_list_any, arg_bool, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg3_string_string_bool[] = {arg_string, arg_string, arg_bool};
ਸਥਿਰ argcheck_T arg3_string_string_dict[] = {arg_string, arg_string, arg_dict_any};
ਸਥਿਰ argcheck_T arg3_string_string_ਸੰਖਿਆ[] = {arg_string, arg_string, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg4_ਸੰਖਿਆ_ਸੰਖਿਆ_string_any[] = {arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ, arg_string, arg_any};
ਸਥਿਰ argcheck_T arg4_string_string_any_string[] = {arg_string, arg_string, arg_any, arg_string};
ਸਥਿਰ argcheck_T arg4_string_string_ਸੰਖਿਆ_string[] = {arg_string, arg_string, arg_ਸੰਖਿਆ, arg_string};
ਸਥਿਰ argcheck_T arg4_string_ਸੰਖਿਆ_bool_bool[] = {arg_string, arg_ਸੰਖਿਆ, arg_bool, arg_bool};
/* Function specific argument types (not covered by the above) */
ਸਥਿਰ argcheck_T arg15_assert_fails[] = {arg_string_or_nr, arg_string_or_list_any, arg_any, arg_ਸੰਖਿਆ, arg_string};
ਸਥਿਰ argcheck_T arg34_assert_inrange[] = {arg_float_or_nr, arg_float_or_nr, arg_float_or_nr, arg_string};
ਸਥਿਰ argcheck_T arg4_browse[] = {arg_bool, arg_string, arg_string, arg_string};
ਸਥਿਰ argcheck_T arg23_chanexpr[] = {arg_chan_or_ਕੰਮ, arg_any, arg_dict_any};
ਸਥਿਰ argcheck_T arg23_chanraw[] = {arg_chan_or_ਕੰਮ, arg_string_or_blob, arg_dict_any};
ਸਥਿਰ argcheck_T arg24_ਗਿਣਤੀ[] = {arg_string_list_tuple_or_dict, arg_any, arg_bool, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg13_cursor[] = {arg_cursor1, arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg12_deepcopy[] = {arg_any, arg_bool};
ਸਥਿਰ argcheck_T arg12_execute[] = {arg_string_or_list_string, arg_string};
ਸਥਿਰ argcheck_T arg12_getchar[] = {arg_bool_or_nr, arg_dict_any};
ਸਥਿਰ argcheck_T arg23_extend[] = {arg_list_or_dict_or_blob_mod, arg_same_as_prev, arg_extend3};
ਸਥਿਰ argcheck_T arg23_extendnew[] = {arg_list_or_dict_or_blob, arg_same_struct_as_prev, arg_extend3};
ਸਥਿਰ argcheck_T arg23_get[] = {arg_get1, arg_string_or_nr, arg_any};
ਸਥਿਰ argcheck_T arg14_glob[] = {arg_string, arg_bool, arg_bool, arg_bool};
ਸਥਿਰ argcheck_T arg25_globpath[] = {arg_string, arg_string, arg_bool, arg_bool, arg_bool};
ਸਥਿਰ argcheck_T arg24_ਸੂਚਕਾਂਕ[] = {arg_list_or_tuple_or_blob, arg_ਵਸਤੂ_ਦਾ_prev, arg_ਸੰਖਿਆ, arg_bool};
ਸਥਿਰ argcheck_T arg23_ਸੂਚਕਾਂਕ[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
ਸਥਿਰ argcheck_T arg23_ਪਾਓ[] = {arg_list_or_blob, arg_ਵਸਤੂ_ਦਾ_prev, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg1_len[] = {arg_len1};
ਸਥਿਰ argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
ਸਥਿਰ argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
ਸਥਿਰ argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
ਸਥਿਰ argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
ਸਥਿਰ argcheck_T arg2_instanceof[] = {arg_object, varargs_class, ਨਿਰਾ };
ਸਥਿਰ argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
ਸਥਿਰ argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
ਸਥਿਰ argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ, arg_dict_any};
ਸਥਿਰ argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ, arg_dict_any};
ਸਥਿਰ argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
ਸਥਿਰ argcheck_T arg45_matchbufline[] = {arg_ਬਫਰ, arg_string, arg_lnum, arg_lnum, arg_dict_any};
ਸਥਿਰ argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
ਸਥਿਰ argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
ਸਥਿਰ argcheck_T arg24_remote_ਵਿਅੰਜਕ[] = {arg_string, arg_string, arg_string, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg2_repeat[] = {arg_repeat1, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg15_ਖੋਜ[] = {arg_string, arg_string, arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ, arg_string_or_func};
ਸਥਿਰ argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg3_setbufline[] = {arg_ਬਫਰ, arg_lnum, arg_str_or_nr_or_list};
ਸਥਿਰ argcheck_T arg2_setline[] = {arg_lnum, arg_any};
ਸਥਿਰ argcheck_T arg24_setloclist[] = {arg_ਸੰਖਿਆ, arg_list_any, arg_string, arg_dict_any};
ਸਥਿਰ argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
ਸਥਿਰ argcheck_T arg23_settagstack[] = {arg_ਸੰਖਿਆ, arg_dict_any, arg_string};
ਸਥਿਰ argcheck_T arg02_sign_getplaced[] = {arg_ਬਫਰ, arg_dict_any};
ਸਥਿਰ argcheck_T arg45_sign_ਰੱਖੋ[] = {arg_ਸੰਖਿਆ, arg_string, arg_string, arg_ਬਫਰ, arg_dict_any};
ਸਥਿਰ argcheck_T arg23_ਟੁਕੜਾ[] = {arg_slice1, arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ};
ਸਥਿਰ argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_ਛਾਂਟੋ_how, arg_dict_any};
ਸਥਿਰ argcheck_T arg24_strpart[] = {arg_string, arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ, arg_bool};
ਸਥਿਰ argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
ਸਥਿਰ argcheck_T arg23_win_execute[] = {arg_ਸੰਖਿਆ, arg_string_or_list_string, arg_string};
ਸਥਿਰ argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
ਸਥਿਰ argcheck_T arg24_ਮੇਲ_func[] = {arg_string_or_list_any, arg_string, arg_ਸੰਖਿਆ, arg_ਸੰਖਿਆ};

// Can be used by functions called through "f_retfunc" to create new types.
ਸਥਿਰ garray_T *current_type_gap = ਨਿਰਾ;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    ਸਥਿਰ type_T *
ret_void(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_void;
}
    ਸਥਿਰ type_T *
ret_any(ਪੂਰਨ	argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_any;
}
    ਸਥਿਰ type_T *
ret_bool(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_bool;
}
    ਸਥਿਰ type_T *
ret_ਸੰਖਿਆ_bool(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_ਸੰਖਿਆ_bool;
}
    ਸਥਿਰ type_T *
ret_ਸੰਖਿਆ(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_ਸੰਖਿਆ;
}
    ਸਥਿਰ type_T *
ret_float(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_float;
}
    ਸਥਿਰ type_T *
ret_string(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_string;
}
    ਸਥਿਰ type_T *
ret_list_any(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_list_any;
}
    ਸਥਿਰ type_T *
ret_list_ਸੰਖਿਆ(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ਵਾਪਸ &t_list_ਸੰਖਿਆ;
}
    ਸਥਿਰ type_T *
ret_list_string(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ਵਾਪਸ &t_list_string;
}
    ਸਥਿਰ type_T *
ret_list_dict_any(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ਵਾਪਸ &t_list_dict_any;
}
    ਸਥਿਰ type_T *
ret_list_ਚੀਜ਼ਾਂ(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ਵਾਪਸ &t_list_list_any;
}
    ਸਥਿਰ type_T *
ret_list_list_any(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ਵਾਪਸ &t_list_list_any;
}
    ਸਥਿਰ type_T *
ret_list_string_ਚੀਜ਼ਾਂ(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ਵਾਪਸ &t_list_list_string;
}
    ਸਥਿਰ type_T *
ret_list_regionpos(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ਵਾਪਸ &t_list_list_list_ਸੰਖਿਆ;
}
    ਸਥਿਰ type_T *
ret_tuple_any(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_tuple_any;
}
    ਸਥਿਰ type_T *
ret_dict_any(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_dict_any;
}
    ਸਥਿਰ type_T *
ret_ਕੰਮ_info(ਪੂਰਨ argcount,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type)
{
    ਜੇ (argcount == 0)
    {
	*decl_type = &t_list_any;
	ਵਾਪਸ &t_list_ਕੰਮ;
    }
    ਵਾਪਸ &t_dict_any;
}
    ਸਥਿਰ type_T *
ret_dict_ਸੰਖਿਆ(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_dict_ਸੰਖਿਆ;
}
    ਸਥਿਰ type_T *
ret_dict_string(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_dict_string;
}
    ਸਥਿਰ type_T *
ret_blob(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_blob;
}
    ਸਥਿਰ type_T *
ret_func_any(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_func_any;
}
    ਸਥਿਰ type_T *
ret_func_unknown(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_func_unknown;
}
    ਸਥਿਰ type_T *
ret_channel(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_channel;
}
    ਸਥਿਰ type_T *
ret_ਕੰਮ(ਪੂਰਨ argcount ਅਣਵਰਤਿਆ,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ &t_ਕੰਮ;
}
    ਸਥਿਰ type_T *
ret_ਪਹਿਲਾ_arg(ਪੂਰਨ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ਜੇ (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	ਵਾਪਸ argtypes[0].type_curr;
    }
    ਵਾਪਸ &t_void;
}
    ਸਥਿਰ type_T *
ret_ਟੁਕੜਾ(ਪੂਰਨ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ਜੇ (argcount > 0)
    {
	ਜੇ (argtypes[0].type_decl != ਨਿਰਾ)
	{
	    ਚੋਣ (argtypes[0].type_decl->tt_type)
	    {
		ਹਾਲਤ VAR_STRING: *decl_type = &t_string; ਰੋਕੋ;
		ਹਾਲਤ VAR_BLOB: *decl_type = &t_blob; ਰੋਕੋ;
		ਹਾਲਤ VAR_LIST: *decl_type = &t_list_any; ਰੋਕੋ;
		ਹਾਲਤ VAR_TUPLE: *decl_type = &t_tuple_any; ਰੋਕੋ;
		ਮੂਲ: ਰੋਕੋ;
	    }
	}
	ਵਾਪਸ argtypes[0].type_curr;
    }
    ਵਾਪਸ &t_void;
}
    ਸਥਿਰ type_T *
ret_copy(ਪੂਰਨ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ਜੇ (argcount > 0)
    {
	ਜੇ (argtypes[0].type_decl != ਨਿਰਾ)
	{
	    ਜੇ (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    ਨਹੀਂਤਾਂ ਜੇ (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    ਨਹੀਂਤਾਂ
		*decl_type = argtypes[0].type_decl;
	}
	ਜੇ (argtypes[0].type_curr != ਨਿਰਾ)
	{
	    ਜੇ (argtypes[0].type_curr->tt_type == VAR_LIST)
		ਵਾਪਸ &t_list_any;
	    ਨਹੀਂਤਾਂ ਜੇ (argtypes[0].type_curr->tt_type == VAR_DICT)
		ਵਾਪਸ &t_dict_any;
	}
	ਵਾਪਸ argtypes[0].type_curr;
    }
    ਵਾਪਸ &t_void;
}
    ਸਥਿਰ type_T *
ret_extend(ਪੂਰਨ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ਜੇ (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	// if the second argument has a different current type then the current
	// type is "any"
	ਜੇ (argcount > 1 && !equal_type(argtypes[0].type_curr,
						     argtypes[1].type_curr, 0))
	{
	    ਜੇ (argtypes[0].type_curr->tt_type == VAR_LIST)
		ਵਾਪਸ &t_list_any;
	    ਜੇ (argtypes[0].type_curr->tt_type == VAR_DICT)
		ਵਾਪਸ &t_dict_any;
	    ਜੇ (argtypes[0].type_curr->tt_type == VAR_BLOB)
		ਵਾਪਸ &t_blob;
	}
	ਵਾਪਸ argtypes[0].type_curr;
    }
    ਵਾਪਸ &t_void;
}
    ਸਥਿਰ type_T *
ret_repeat(ਪੂਰਨ argcount,
	type2_T *argtypes,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਜੇ (argcount == 0)
	ਵਾਪਸ &t_any;
    ਜੇ (argtypes[0].type_curr == &t_ਸੰਖਿਆ)
	ਵਾਪਸ &t_string;
    ਵਾਪਸ argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    ਸਥਿਰ type_T *
ret_ਪਹਿਲਾ_cont(ਪੂਰਨ argcount,
	type2_T *argtypes,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਜੇ (argcount > 0)
    {
	ਜੇ (argtypes[0].type_curr->tt_type == VAR_LIST)
	    ਵਾਪਸ &t_list_any;
	ਜੇ (argtypes[0].type_curr->tt_type == VAR_DICT)
	    ਵਾਪਸ &t_dict_any;
	ਜੇ (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    ਵਾਪਸ argtypes[0].type_curr;
    }
    ਵਾਪਸ &t_any;
}
// for get(): the type of the item when a missing item has the same type
    ਸਥਿਰ type_T *
ret_get(ਪੂਰਨ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ਜੇ (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*ਵਸਤੂ = ਨਿਰਾ;

	ਜੇ (t->tt_type == VAR_BLOB)
	    ਵਸਤੂ = &t_ਸੰਖਿਆ;
	ਨਹੀਂਤਾਂ ਜੇ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    ਵਸਤੂ = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	ਜੇ (ਵਸਤੂ != ਨਿਰਾ && (argcount == 3
			? equal_type(ਵਸਤੂ, argtypes[2].type_curr, 0)
			: ਵਸਤੂ->tt_type == VAR_ਸੰਖਿਆ))
	{
	    ਜੇ (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    ਵਾਪਸ ਵਸਤੂ;
	}
    }
    ਵਾਪਸ &t_any;
}
// for getline()
    ਸਥਿਰ type_T *
ret_getline(ਪੂਰਨ argcount,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਜੇ (argcount == 1)
	ਵਾਪਸ &t_string;
    *decl_type = &t_list_any;
    ਵਾਪਸ &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    ਸਥਿਰ type_T *
ret_string_or_any_1(ਪੂਰਨ argcount,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਜੇ (argcount < 1)
	ਵਾਪਸ &t_string;
    ਵਾਪਸ &t_any;
}
    ਸਥਿਰ type_T *
ret_string_or_any_2(ਪੂਰਨ argcount,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਜੇ (argcount < 2)
	ਵਾਪਸ &t_string;
    ਵਾਪਸ &t_any;
}
    ਸਥਿਰ type_T *
ret_string_or_any_3(ਪੂਰਨ argcount,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਜੇ (argcount < 3)
	ਵਾਪਸ &t_string;
    ਵਾਪਸ &t_any;
}
    ਸਥਿਰ type_T *
ret_string_or_any_4(ਪੂਰਨ argcount,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਜੇ (argcount < 4)
	ਵਾਪਸ &t_string;
    ਵਾਪਸ &t_any;
}
// for abs()
    ਸਥਿਰ type_T *
ret_abs(ਪੂਰਨ argcount,
	type2_T *argtypes,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਜੇ (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	ਵਾਪਸ &t_float;
    ਵਾਪਸ &t_ਸੰਖਿਆ;
}
// for sign_define()
    ਸਥਿਰ type_T *
ret_sign_define(ਪੂਰਨ argcount,
	type2_T *argtypes,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    ਜੇ (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	ਵਾਪਸ &t_list_ਸੰਖਿਆ;
    ਵਾਪਸ &t_ਸੰਖਿਆ;
}
// for values(): list of member of first argument
    ਸਥਿਰ type_T *
ret_list_member(ਪੂਰਨ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ਜੇ (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	ਜੇ (current_type_gap != ਨਿਰਾ
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	ਨਹੀਂਤਾਂ
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	ਜੇ (current_type_gap != ਨਿਰਾ
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    ਵਾਪਸ get_list_type(t->tt_member, current_type_gap);
    }
    ਵਾਪਸ &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    ਸਥਿਰ type_T *
ret_list_or_dict_0(ਪੂਰਨ argcount,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type)
{
    ਜੇ (argcount > 0)
	ਵਾਪਸ &t_dict_any;
    *decl_type = &t_list_any;
    ਵਾਪਸ &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    ਸਥਿਰ type_T *
ret_list_or_dict_1(ਪੂਰਨ argcount,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type)
{
    ਜੇ (argcount > 1)
	ਵਾਪਸ &t_dict_any;
    *decl_type = &t_list_any;
    ਵਾਪਸ &t_list_dict_any;
}

    ਸਥਿਰ type_T *
ret_argv(ਪੂਰਨ argcount,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type)
{
    // argv() returns list of strings
    ਜੇ (argcount == 0)
    {
	*decl_type = &t_list_any;
	ਵਾਪਸ &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    ਵਾਪਸ &t_any;
}

    ਸਥਿਰ type_T *
ret_remove(ਪੂਰਨ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ਜੇ (argcount > 0)
    {
	ਜੇ (argtypes[0].type_curr->tt_type == VAR_LIST
		|| argtypes[0].type_curr->tt_type == VAR_DICT)
	{
	    ਜੇ (argcount == 3)
	    {
		*decl_type = argtypes[0].type_decl;
		ਵਾਪਸ argtypes[0].type_curr;
	    }
	    ਜੇ (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    ਵਾਪਸ argtypes[0].type_curr->tt_member;
	}
	ਜੇ (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    ਵਾਪਸ &t_ਸੰਖਿਆ;
    }
    ਵਾਪਸ &t_any;
}

    ਸਥਿਰ type_T *
ret_max_min(ਪੂਰਨ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ਜੇ (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	ਜੇ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	ਨਹੀਂਤਾਂ
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	ਜੇ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	ਨਹੀਂਤਾਂ
	    t = &t_any;
	ਵਾਪਸ t;
    }
    ਵਾਪਸ &t_any;
}

    ਸਥਿਰ type_T *
ret_getreg(ਪੂਰਨ argcount,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    ਜੇ (argcount == 3)
    {
	*decl_type = &t_list_any;
	ਵਾਪਸ &t_list_string;
    }
    ਵਾਪਸ &t_string;
}

    ਸਥਿਰ type_T *
ret_virtcol(ਪੂਰਨ argcount,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    ਜੇ (argcount > 1)
    {
	*decl_type = &t_list_any;
	ਵਾਪਸ &t_list_ਸੰਖਿਆ;
    }
    ਵਾਪਸ &t_ਸੰਖਿਆ;
}

    ਸਥਿਰ type_T *
ret_maparg(ਪੂਰਨ argcount,
	type2_T *argtypes ਅਣਵਰਤਿਆ,
	type_T	**decl_type ਅਣਵਰਤਿਆ)
{
    // Assume that if the fourth argument is passed it's non-zero
    ਜੇ (argcount == 4)
	ਵਾਪਸ &t_dict_any;
    ਵਾਪਸ &t_string;
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਅੱਖਰ	*f_name;	// function name
    ਅੱਖਰ	f_min_argc;	// minimal number of arguments
    ਅੱਖਰ	f_max_argc;	// maximal number of arguments
    ਅੱਖਰ	f_argtype;	// for method: FEARG_ values; bits FE_
    argcheck_T	*f_argcheck;	// list of functions to check argument types;
				// use "arg_any" (not NULL) to accept an
				// argument of any type
    ਅੱਖਰ	*f_argnames;	// names of the arguments as in the help,
				// separated by ", "; an argument without a
				// name is empty, NULL when there is none
    type_T	*(*f_retfunc)(ਪੂਰਨ argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    ਖਾਲੀ	(*f_func)(typval_T *ਦਲੀਲਾਂ, typval_T *rvar);
				// implementation of function
} funcentry_T;

// Set f_max_argc to VARGS for varargs.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VARGS    CHAR_MAX

// values for f_argtype; zero means it cannot be used as a method
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEARG_1	    0x01    // base is the first argument
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEARG_2     0x02    // base is the second argument
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEARG_3     0x03    // base is the third argument
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEARG_4     0x04    // base is the fourth argument
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEARG_ਨਕਾਬ  0x0F    // bits in f_argtype used as argument index
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FE_X	    0x10    // builtin accepts a non-value (class, typealias)

#ਜੇ defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#ਨਹੀਂਤਾਂ
# define MATH_FUNC(name) ਨਿਰਾ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
# define TIMER_FUNC(name) name
#ਨਹੀਂਤਾਂ
# define TIMER_FUNC(name) ਨਿਰਾ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
# define ਕੰਮ_FUNC(name) name
#ਨਹੀਂਤਾਂ
# define ਕੰਮ_FUNC(name) ਨਿਰਾ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#ਨਹੀਂਤਾਂ
# define PROP_FUNC(name) ਨਿਰਾ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
# define SIGN_FUNC(name) name
#ਨਹੀਂਤਾਂ
# define SIGN_FUNC(name) ਨਿਰਾ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOUND
# define SOUND_FUNC(name) name
#ਨਹੀਂਤਾਂ
# define SOUND_FUNC(name) ਨਿਰਾ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
# define ਪਦ_FUNC(name) name
#ਨਹੀਂਤਾਂ
# define ਪਦ_FUNC(name) ਨਿਰਾ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#ਨਹੀਂਤਾਂ
# define TABPANEL_FUNC(name) ਨਿਰਾ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
# define IMAGE_FUNC(name) name
#ਨਹੀਂਤਾਂ
# define IMAGE_FUNC(name) ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅਚਲ funcentry_T global_functions[] =
{
    {"abs",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_abs,	    f_abs},
    {"acos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_acos},
    {"add",		2, 2, FEARG_1,	    arg2_listblobmod_ਵਸਤੂ,
			"object, expr",
			ret_ਪਹਿਲਾ_arg,	    f_ਜੋੜੋ},
    {"and",		2, 2, FEARG_1,	    arg2_ਸੰਖਿਆ,
			"expr1, expr2",
			ret_ਸੰਖਿਆ,	    f_and},
    {"append",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_ਸੰਖਿਆ_bool,    f_append},
    {"appendbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_ਸੰਖਿਆ_bool,    f_appendbufline},
    {"argc",		0, 1, 0,	    arg1_ਸੰਖਿਆ,
			"winid",
			ret_ਸੰਖਿਆ,	    f_argc},
    {"argidx",		0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_argidx},
    {"arglistid",	0, 2, 0,	    arg2_ਸੰਖਿਆ,
			"winnr, tabnr",
			ret_ਸੰਖਿਆ,	    f_arglistid},
    {"argv",		0, 2, 0,	    arg2_ਸੰਖਿਆ,
			"nr, winid",
			ret_argv,	    f_argv},
    {"asin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_asin},
    {"assert_beeps",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_ਸੰਖਿਆ_bool,    f_assert_beeps},
    {"assert_equal",	2, 3, FEARG_2,	    ਨਿਰਾ,
			"expected, actual, msg",
			ret_ਸੰਖਿਆ_bool,    f_assert_equal},
    {"assert_equalfile", 2, 3, FEARG_1,	    arg3_string,
			"fname_one, fname_two, msg",
			ret_ਸੰਖਿਆ_bool,    f_assert_equalfile},
    {"assert_exception", 1, 2, 0,	    arg2_string,
			"error, msg",
			ret_ਸੰਖਿਆ_bool,    f_assert_exception},
    {"assert_fails",	1, 5, FEARG_1,	    arg15_assert_fails,
			"cmd, error, msg, lnum, context",
			ret_ਸੰਖਿਆ_bool,    f_assert_fails},
    {"assert_false",	1, 2, FEARG_1,	    ਨਿਰਾ,
			"actual, msg",
			ret_ਸੰਖਿਆ_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_ਸੰਖਿਆ_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_ਸੰਖਿਆ_bool,    f_assert_ਮੇਲ},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_ਸੰਖਿਆ_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    ਨਿਰਾ,
			"expected, actual, msg",
			ret_ਸੰਖਿਆ_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_ਸੰਖਿਆ_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_ਸੰਖਿਆ_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    ਨਿਰਾ,
			"actual, msg",
			ret_ਸੰਖਿਆ_bool,    f_assert_true},
    {"atan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_atan},
    {"atan2",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"expr1, expr2",
			ret_float,	    f_atan2},
    {"autocmd_add",	1, 1, FEARG_1,	    arg1_list_any,
			"acmds",
			ret_bool,	    f_autocmd_ਜੋੜੋ},
    {"autocmd_delete",	1, 1, FEARG_1,	    arg1_list_any,
			"acmds",
			ret_bool,	    f_autocmd_delete},
    {"autocmd_get",	0, 1, FEARG_1,	    arg1_dict_any,
			"opts",
			ret_list_dict_any,  f_autocmd_get},
    {"balloon_gettext",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL
	    f_balloon_gettext
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL
	    f_balloon_show
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#ਜੇ defined(FEAT_BEVAL_ਪਦ)
	    f_balloon_ਵੰਡੋ
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
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
			ret_list_ਸੰਖਿਆ,    f_blob2list},
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
			ret_ਸੰਖਿਆ,	    f_bufadd},
    {"bufexists",	1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_ਸੰਖਿਆ_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_ਬਫਰ,	// obsolete
			"buf",
			ret_ਸੰਖਿਆ_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_ਬਫਰ,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_ਬਫਰ,	// obsolete
			"buf",
			ret_ਸੰਖਿਆ,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_ਸੰਖਿਆ_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_ਸੰਖਿਆ_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_ਬਫਰ_bool,
			"buf, create",
			ret_ਸੰਖਿਆ,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_ਸੰਖਿਆ,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_ਸੰਖਿਆ,	    f_bufwinnr},
    {"byte2line",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"byte",
			ret_ਸੰਖਿਆ,	    f_byte2line},
    {"byteidx",		2, 3, FEARG_1,	    arg3_string_ਸੰਖਿਆ_bool,
			"expr, nr, utf16",
			ret_ਸੰਖਿਆ,	    f_byteidx},
    {"byteidxcomp",	2, 3, FEARG_1,	    arg3_string_ਸੰਖਿਆ_bool,
			"expr, nr, utf16",
			ret_ਸੰਖਿਆ,	    f_byteidxcomp},
    {"call",		2, 3, FEARG_1,	    arg3_any_list_dict,
			"func, arglist, dict",
			ret_any,	    f_call},
    {"ceil",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ceil},
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_ਕੰਮ,
			"handle",
			ret_ਸੰਖਿਆ_bool,    ਕੰਮ_FUNC(f_ch_canread)},
    {"ch_close",	1, 1, FEARG_1,	    arg1_chan_or_ਕੰਮ,
			"handle",
			ret_void,	    ਕੰਮ_FUNC(f_ch_close)},
    {"ch_close_in",	1, 1, FEARG_1,	    arg1_chan_or_ਕੰਮ,
			"handle",
			ret_void,	    ਕੰਮ_FUNC(f_ch_close_ਵਿੱਚ)},
    {"ch_evalexpr",	2, 3, FEARG_1,	    arg23_chanexpr,
			"handle, expr, options",
			ret_any,	    ਕੰਮ_FUNC(f_ch_evalexpr)},
    {"ch_evalraw",	2, 3, FEARG_1,	    arg23_chanraw,
			"handle, string, options",
			ret_any,	    ਕੰਮ_FUNC(f_ch_evalraw)},
    {"ch_getbufnr",	2, 2, FEARG_1,	    arg2_chan_or_ਕੰਮ_string,
			"handle, what",
			ret_ਸੰਖਿਆ,	    ਕੰਮ_FUNC(f_ch_getbufnr)},
    {"ch_getjob",	1, 1, FEARG_1,	    arg1_chan_or_ਕੰਮ,
			"channel",
			ret_ਕੰਮ,	    ਕੰਮ_FUNC(f_ch_getjob)},
    {"ch_info",		1, 1, FEARG_1,	    arg1_chan_or_ਕੰਮ,
			"handle",
			ret_dict_any,	    ਕੰਮ_FUNC(f_ch_info)},
    {"ch_listen",	1, 2, FEARG_1,	    arg2_string_dict,
			"address, options",
			ret_channel,	    ਕੰਮ_FUNC(f_ch_listen)},
    {"ch_log",		1, 2, FEARG_1,	    arg2_string_chan_or_ਕੰਮ,
			"msg, handle",
			ret_void,	    f_ch_log},
    {"ch_logfile",	1, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_void,	    f_ch_logfile},
    {"ch_open",		1, 2, FEARG_1,	    arg2_string_dict,
			"address, options",
			ret_channel,	    ਕੰਮ_FUNC(f_ch_open)},
    {"ch_read",		1, 2, FEARG_1,	    arg2_chan_or_ਕੰਮ_dict,
			"handle, options",
			ret_string,	    ਕੰਮ_FUNC(f_ch_read)},
    {"ch_readblob",	1, 2, FEARG_1,	    arg2_chan_or_ਕੰਮ_dict,
			"handle, options",
			ret_blob,	    ਕੰਮ_FUNC(f_ch_readblob)},
    {"ch_readraw",	1, 2, FEARG_1,	    arg2_chan_or_ਕੰਮ_dict,
			"handle, options",
			ret_string,	    ਕੰਮ_FUNC(f_ch_readraw)},
    {"ch_sendexpr",	2, 3, FEARG_1,	    arg23_chanexpr,
			"handle, expr, options",
			ret_any,	    ਕੰਮ_FUNC(f_ch_sendexpr)},
    {"ch_sendraw",	2, 3, FEARG_1,	    arg23_chanraw,
			"handle, expr, options",
			ret_void,	    ਕੰਮ_FUNC(f_ch_sendraw)},
    {"ch_setoptions",	2, 2, FEARG_1,	    arg2_chan_or_ਕੰਮ_dict,
			"handle, options",
			ret_void,	    ਕੰਮ_FUNC(f_ch_setoptions)},
    {"ch_status",	1, 2, FEARG_1,	    arg2_chan_or_ਕੰਮ_dict,
			"handle, options",
			ret_string,	    ਕੰਮ_FUNC(f_ch_ਸਥਿਤੀ)},
    {"changenr",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_changenr},
    {"char2nr",		1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_ਸੰਖਿਆ,	    f_char2nr},
    {"charclass",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_ਸੰਖਿਆ,	    f_charclass},
    {"charcol",		1, 2, FEARG_1,	    arg2_string_or_list_ਸੰਖਿਆ,
			"expr, winid",
			ret_ਸੰਖਿਆ,	    f_charcol},
    {"charidx",		2, 4, FEARG_1,	    arg4_string_ਸੰਖਿਆ_bool_bool,
			"string, idx, countcc, utf16",
			ret_ਸੰਖਿਆ,	    f_charidx},
    {"chdir",		1, 2, FEARG_1,	    arg2_string,
			"dir, scope",
			ret_string,	    f_chdir},
    {"cindent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ਸੰਖਿਆ,	    f_cindent},
    {"clearmatches",	0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"win",
			ret_void,	    f_clearmatches},
    {"cmdcomplete_info",0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_ਸੰਖਿਆ,
			"expr, winid",
			ret_ਸੰਖਿਆ,	    f_ਖਾਨਾ},
    {"complete",	2, 2, FEARG_2,	    arg2_ਸੰਖਿਆ_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_ਸੰਖਿਆ,	    f_complete_ਜੋੜੋ},
    {"complete_check",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_ਸੰਖਿਆ_string,
			"msg, choices, default, type",
			ret_ਸੰਖਿਆ,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    ਨਿਰਾ,
			"expr",
			ret_copy,	    f_copy},
    {"cos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cos},
    {"cosh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cosh},
    {"count",		2, 4, FEARG_1,	    arg24_ਗਿਣਤੀ,
			"comp, expr, ic, start",
			ret_ਸੰਖਿਆ,	    f_ਗਿਣਤੀ},
    {"cscope_connection",0,3, 0,	    arg3_ਸੰਖਿਆ_string_string,
			"num, dbpath, prepend",
			ret_ਸੰਖਿਆ,	    f_cscope_connection},
    {"cursor",		1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_ਸੰਖਿਆ,	    f_cursor},
    {"debugbreak",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"pid",
			ret_ਸੰਖਿਆ,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    f_debugbreak
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_ਸੰਖਿਆ_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_ਬਫਰ_lnum_lnum,
			"buf, first, last",
			ret_ਸੰਖਿਆ_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ_bool,    f_did_filetype},
    {"diff",		2, 3, FEARG_1,	    arg3_diff,
			"fromlist, tolist, options",
			ret_string_or_any_3,  f_diff},
    {"diff_filler",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ਸੰਖਿਆ,	    f_diff_filler},
    {"diff_hlID",	2, 2, FEARG_1,	    arg2_lnum_ਸੰਖਿਆ,
			"lnum, col",
			ret_ਸੰਖਿਆ,	    f_diff_hlID},
    {"digraph_get",	1, 1, FEARG_1,	    arg1_string,
			"chars",
			ret_string,	    f_digraph_get},
    {"digraph_getlist",	0, 1, FEARG_1,	    arg1_bool,
			"listall",
			ret_list_string_ਚੀਜ਼ਾਂ, f_digraph_getlist},
    {"digraph_set",	2, 2, FEARG_1,	    arg2_string,
			"chars, digraph",
			ret_bool,	f_digraph_ਸੈੱਟ},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    ਨਿਰਾ,
			"expr",
			ret_ਸੰਖਿਆ_bool,    f_ਖਾਲੀ_ਥਾਵਾਂ},
    {"environ",		0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    ਨਿਰਾ,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ_bool,    f_eventhandler},
    {"executable",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_ਸੰਖਿਆ,	    f_executable},
    {"execute",		1, 2, FEARG_1,	    arg12_execute,
			"command, silent",
			ret_string,	    f_execute},
    {"exepath",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_exepath},
    {"exists",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_ਸੰਖਿਆ_bool,    f_exists},
    {"exists_compiled",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_ਸੰਖਿਆ_bool,    f_exists_compiled},
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
			ret_ਪਹਿਲਾ_cont,	    f_extendnew},
    {"feedkeys",	1, 2, FEARG_1,	    arg2_string,
			"string, mode",
			ret_void,	    f_feedkeys},
    {"file_readable",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"file",
			ret_ਸੰਖਿਆ_bool,    f_filereadable},
    {"filecopy",	2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_ਸੰਖਿਆ_bool,    f_filecopy},
    {"filereadable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_ਸੰਖਿਆ_bool,    f_filereadable},
    {"filewritable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_ਸੰਖਿਆ,	    f_filewritable},
    {"filter",		2, 2, FEARG_1,	    arg2_filter,
			"expr1, expr2",
			ret_ਪਹਿਲਾ_arg,	    f_filter},
    {"finddir",		1, 3, FEARG_1,	    arg3_string_string_ਸੰਖਿਆ,
			"name, path, count",
			ret_string_or_any_3,	    f_finddir},
    {"findfile",	1, 3, FEARG_1,	    arg3_string_string_ਸੰਖਿਆ,
			"name, path, count",
			ret_string_or_any_3,	    f_findfile},
    {"flatten",		1, 2, FEARG_1,	    arg2_list_any_ਸੰਖਿਆ,
			"list, maxdepth",
			ret_list_any,	    f_flatten},
    {"flattennew",	1, 2, FEARG_1,	    arg2_list_any_ਸੰਖਿਆ,
			"list, maxdepth",
			ret_list_any,	    f_flattennew},
    {"float2nr",	1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_ਸੰਖਿਆ,	    f_float2nr},
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
    {"foldclosed",	1, 2, FEARG_1,	    arg2_lnum_ਸੰਖਿਆ,
			"lnum, winid",
			ret_ਸੰਖਿਆ,	    f_foldclosed},
    {"foldclosedend",	1, 2, FEARG_1,	    arg2_lnum_ਸੰਖਿਆ,
			"lnum, winid",
			ret_ਸੰਖਿਆ,	    f_foldclosedend},
    {"foldlevel",	1, 2, FEARG_1,	    arg2_lnum_ਸੰਖਿਆ,
			"lnum, winid",
			ret_ਸੰਖਿਆ,	    f_foldlevel},
    {"foldtext",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_ਸੰਖਿਆ,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_ਪਹਿਲਾ_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
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
    {"getbgcolor",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_list_any,	    f_getbgcolor},
    {"getbufinfo",	0, 1, FEARG_1,	    arg1_ਬਫਰ_or_dict_any,
			"buf",
			ret_list_dict_any,  f_getbufinfo},
    {"getbufline",	2, 3, FEARG_1,	    arg3_ਬਫਰ_lnum_lnum,
			"buf, lnum, end",
			ret_list_string,    f_getbufline},
    {"getbufoneline",	2, 2, FEARG_1,	    arg2_ਬਫਰ_lnum,
			"buf, lnum",
			ret_string,	    f_getbufoneline},
    {"getbufvar",	2, 3, FEARG_1,	    arg3_ਬਫਰ_string_any,
			"buf, varname, def",
			ret_any,	    f_getbufvar},
    {"getcellpixels",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_ਸੰਖਿਆ,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_getcmdwintype},
    {"getcompletion",	2, 3, FEARG_1,	    arg3_string_string_bool,
			"pat, type, filtered",
			ret_list_string,    f_getcompletion},
    {"getcompletiontype", 1, 1, FEARG_1,    arg1_string,
			"pat",
			ret_string,	    f_getcompletiontype},
    {"getcurpos",	0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"winid",
			ret_list_ਸੰਖਿਆ,    f_getcurpos},
    {"getcursorcharpos", 0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"winid",
			ret_list_ਸੰਖਿਆ,    f_getcursorcharpos},
    {"getcwd",		0, 2, FEARG_1,	    arg2_ਸੰਖਿਆ,
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
			ret_ਸੰਖਿਆ,	    f_getfsize},
    {"getftime",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_ਸੰਖਿਆ,	    f_getftime},
    {"getftype",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getftype},
    {"getimstatus",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ_bool,    f_getimstatus},
    {"getinfo",		2, 3, FEARG_2,	    arg3_string_string_dict,
			"kind, name, opts",
			ret_dict_any,	    f_getinfo},
    {"getjumplist",	0, 2, FEARG_1,	    arg2_ਸੰਖਿਆ,
			"winnr, tabnr",
			ret_list_any,	    f_getjumplist},
    {"getline",		1, 2, FEARG_1,	    arg2_lnum,
			"lnum, end",
			ret_getline,	    f_getline},
    {"getloclist",	1, 2, 0,	    arg2_ਸੰਖਿਆ_dict_any,
			"nr, what",
			ret_list_or_dict_1, f_getloclist},
    {"getmarklist",	0, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_ਸੰਖਿਆ,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_dict_ਸੰਖਿਆ,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_getpid},
    {"getpos",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_ਸੰਖਿਆ,    f_getpos},
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
    {"getstacktrace",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_list_dict_any,  f_getstacktrace},
    {"gettabinfo",	0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"tabnr",
			ret_list_dict_any,  f_gettabinfo},
    {"gettabvar",	2, 3, FEARG_1,	    arg3_ਸੰਖਿਆ_string_any,
			"tabnr, varname, def",
			ret_any,	    f_gettabvar},
    {"gettabwinvar",	3, 4, FEARG_1,	    arg4_ਸੰਖਿਆ_ਸੰਖਿਆ_string_any,
			"tabnr, winnr, varname, def",
			ret_any,	    f_gettabwinvar},
    {"gettagstack",	0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"winnr",
			ret_dict_any,	    f_gettagstack},
    {"gettext",		1, 2, FEARG_1,	    arg2_string,
			"text, package",
			ret_string,	    f_gettext},
    {"getwininfo",	0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"winid",
			ret_list_dict_any,  f_getwininfo},
    {"getwinpos",	0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"timeout",
			ret_list_ਸੰਖਿਆ,    f_getwinpos},
    {"getwinposx",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_getwinposy},
    {"getwinvar",	2, 3, FEARG_1,	    arg3_ਸੰਖਿਆ_string_any,
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
			ret_ਸੰਖਿਆ_bool,    f_has},
    {"has_key",		2, 2, FEARG_1,	    arg2_dict_any_string_or_nr,
			"dict, key",
			ret_ਸੰਖਿਆ_bool,    f_has_ਕੁੰਜੀ},
    {"haslocaldir",	0, 2, FEARG_1,	    arg2_ਸੰਖਿਆ,
			"winnr, tabnr",
			ret_ਸੰਖਿਆ,	    f_haslocaldir},
    {"hasmapto",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"what, mode, abbr",
			ret_ਸੰਖਿਆ_bool,    f_hasmapto},
    {"highlightID",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_ਸੰਖਿਆ,	    f_hlID},
    {"highlight_exists",1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_ਸੰਖਿਆ_bool,    f_hlexists},
    {"histadd",		2, 2, FEARG_2,	    arg2_string,
			"history, item",
			ret_ਸੰਖਿਆ_bool,    f_histadd},
    {"histdel",		1, 2, FEARG_1,	    arg2_string_string_or_ਸੰਖਿਆ,
			"history, item",
			ret_ਸੰਖਿਆ_bool,    f_histdel},
    {"histget",		1, 2, FEARG_1,	    arg2_string_ਸੰਖਿਆ,
			"history, index",
			ret_string,	    f_histget},
    {"histnr",		1, 1, FEARG_1,	    arg1_string,
			"history",
			ret_ਸੰਖਿਆ,	    f_histnr},
    {"hlID",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_ਸੰਖਿਆ,	    f_hlID},
    {"hlexists",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_ਸੰਖਿਆ_bool,    f_hlexists},
    {"hlget",		0, 2, FEARG_1,	    arg2_string_bool,
			"name, resolve",
			ret_list_dict_any,  f_hlget},
    {"hlset",		1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_ਸੰਖਿਆ_bool,    f_hlset},
    {"hostname",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    ਨਿਰਾ,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_ਸੰਖਿਆ,	    IMAGE_FUNC(f_image_ਜੋੜੋ)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ਸੰਖਿਆ,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_ਸੂਚਕਾਂਕ,
			"object, expr, start, ic",
			ret_ਸੰਖਿਆ,	    f_ਸੂਚਕਾਂਕ},
    {"indexof",		2, 3, FEARG_1,	    arg23_ਸੂਚਕਾਂਕ,
			"object, expr, opts",
			ret_ਸੰਖਿਆ,	    f_indexof},
    {"input",		1, 3, FEARG_1,	    arg3_string,
			"prompt, text, completion",
			ret_string,	    f_ਨਿਵੇਸ਼},
    {"inputdialog",	1, 3, FEARG_1,	    arg3_string,
			"prompt, text, cancelreturn",
			ret_string,	    f_inputdialog},
    {"inputlist",	1, 1, FEARG_1,	    arg1_list_string,
			"textlist",
			ret_ਸੰਖਿਆ,	    f_inputlist},
    {"inputrestore",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_ਪਾਓ,
			"object, item, idx",
			ret_ਪਹਿਲਾ_arg,	    f_ਪਾਓ},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_void,	    f_ਰੁਕਾਵਟ},
    {"invert",		1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"expr",
			ret_ਸੰਖਿਆ,	    f_invert},
    {"isabsolutepath",	1, 1, FEARG_1,	    arg1_string,
			"path",
			ret_ਸੰਖਿਆ_bool,    f_isabsolutepath},
    {"isdirectory",	1, 1, FEARG_1,	    arg1_string,
			"directory",
			ret_ਸੰਖਿਆ_bool,    f_isdirectory},
    {"isinf",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_ਸੰਖਿਆ,	    MATH_FUNC(f_isinf)},
    {"islocked",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_ਸੰਖਿਆ_bool,    f_islocked},
    {"isnan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_ਸੰਖਿਆ_bool,    MATH_FUNC(f_isnan)},
    {"items",		1, 1, FEARG_1,	    arg1_list_tuple_dict_blob_or_string,
			"expr",
			ret_list_ਚੀਜ਼ਾਂ,	    f_ਚੀਜ਼ਾਂ},
    {"job_getchannel",	1, 1, FEARG_1,	    arg1_ਕੰਮ,
			"job",
			ret_channel,	    ਕੰਮ_FUNC(f_ਕੰਮ_getchannel)},
    {"job_info",	0, 1, FEARG_1,	    arg1_ਕੰਮ,
			"job",
			ret_ਕੰਮ_info,	    ਕੰਮ_FUNC(f_ਕੰਮ_info)},
    {"job_setoptions",	2, 2, FEARG_1,	    arg2_ਕੰਮ_dict,
			"job, options",
			ret_void,	    ਕੰਮ_FUNC(f_ਕੰਮ_setoptions)},
    {"job_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"command, options",
			ret_ਕੰਮ,	    ਕੰਮ_FUNC(f_ਕੰਮ_ਸ਼ੁਰੂ)},
    {"job_status",	1, 1, FEARG_1,	    arg1_ਕੰਮ,
			"job",
			ret_string,	    ਕੰਮ_FUNC(f_ਕੰਮ_ਸਥਿਤੀ)},
    {"job_stop",	1, 2, FEARG_1,	    arg2_ਕੰਮ_string_or_ਸੰਖਿਆ,
			"job, how",
			ret_ਸੰਖਿਆ_bool,    ਕੰਮ_FUNC(f_ਕੰਮ_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    ਨਿਰਾ,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    ਨਿਰਾ,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_ਕੁੰਜੀਆਂ},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    ਨਿਰਾ,	// obsolete
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_ਆਖਰੀ_ਬਫਰ_nr},
    {"len",		1, 1, FEARG_1,	    arg1_len,
			"expr",
			ret_ਸੰਖਿਆ,	    f_len},
    {"libcall",		3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_string,	    f_libcall},
    {"libcallnr",	3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_ਸੰਖਿਆ,	    f_libcallnr},
    {"line",		1, 2, FEARG_1,	    arg2_string_ਸੰਖਿਆ,
			"expr, winid",
			ret_ਸੰਖਿਆ,	    f_ਸਤਰ},
    {"line2byte",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ਸੰਖਿਆ,	    f_line2byte},
    {"lispindent",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ਸੰਖਿਆ,	    f_lispindent},
    {"list2blob",	1, 1, FEARG_1,	    arg1_list_ਸੰਖਿਆ,
			"list",
			ret_blob,	    f_list2blob},
    {"list2str",	1, 2, FEARG_1,	    arg2_list_ਸੰਖਿਆ_bool,
			"list, utf8",
			ret_string,	    f_list2str},
    {"list2tuple",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_tuple_any,	    f_list2tuple},
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_ਬਫਰ_bool_or_dict,
			"callback, buf, options",
			ret_ਸੰਖਿਆ,	    f_listener_ਜੋੜੋ},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"id",
			ret_ਸੰਖਿਆ_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_localtime},
    {"log",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_log},
    {"log10",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_log10},
    {"luaeval",		1, 2, FEARG_1,	    arg2_string_any,
			"expr1, expr2",
			ret_any,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LUA
		f_luaeval
#ਨਹੀਂਤਾਂ
		ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"map",		2, 2, FEARG_1,	    arg2_map,
			"expr1, expr2",
			ret_ਪਹਿਲਾ_cont,	    f_map},
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
			ret_ਪਹਿਲਾ_cont,	    f_mapnew},
    {"mapset",		1, 3, FEARG_1,	    arg3_string_or_dict_bool_dict,
			"mode, abbr, dict",
			ret_void,	    f_mapset},
    {"match",		2, 4, FEARG_1,	    arg24_ਮੇਲ_func,
			"expr, pat, start, count",
			ret_ਸੰਖਿਆ,	    f_ਮੇਲ},
    {"matchadd",	2, 5, FEARG_1,	    arg25_matchadd,
			"group, pattern, priority, id, dict",
			ret_ਸੰਖਿਆ,	    f_matchadd},
    {"matchaddpos",	2, 5, FEARG_1,	    arg25_matchaddpos,
			"group, pos, priority, id, dict",
			ret_ਸੰਖਿਆ,	    f_matchaddpos},
    {"matcharg",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"nr",
			ret_list_string,    f_matcharg},
    {"matchbufline",	4, 5, FEARG_1,	    arg45_matchbufline,
			"buf, pat, lnum, end, dict",
			ret_list_any,	    f_matchbufline},
    {"matchdelete",	1, 2, FEARG_1,	    arg2_ਸੰਖਿਆ,
			"id, win",
			ret_ਸੰਖਿਆ_bool,    f_matchdelete},
    {"matchend",	2, 4, FEARG_1,	    arg24_ਮੇਲ_func,
			"expr, pat, start, count",
			ret_ਸੰਖਿਆ,	    f_matchend},
    {"matchfuzzy",	2, 3, FEARG_1,	    arg3_list_string_dict,
			"list, str, dict",
			ret_list_any,	    f_matchfuzzy},
    {"matchfuzzypos",	2, 3, FEARG_1,	    arg3_list_string_dict,
			"list, str, dict",
			ret_list_list_any,  f_matchfuzzypos},
    {"matchlist",	2, 4, FEARG_1,	    arg24_ਮੇਲ_func,
			"expr, pat, start, count",
			ret_list_string,    f_matchlist},
    {"matchstr",	2, 4, FEARG_1,	    arg24_ਮੇਲ_func,
			"expr, pat, start, count",
			ret_string,	    f_matchstr},
    {"matchstrlist",	2, 3, FEARG_1,	    arg23_matchstrlist,
			"list, pat, dict",
			ret_list_any,	    f_matchstrlist},
    {"matchstrpos",	2, 4, FEARG_1,	    arg24_ਮੇਲ_func,
			"expr, pat, start, count",
			ret_list_any,	    f_matchstrpos},
    {"max",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_max},
    {"menu_info",	1, 2, FEARG_1,	    arg2_string,
			"name, mode",
			ret_dict_any,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	    f_menu_info
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_ਸੰਖਿਆ,
			"name, flags, prot",
			ret_ਸੰਖਿਆ_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_ਬਹੁਲਕ},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
	    f_mzeval
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"nextnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ਸੰਖਿਆ,	    f_nextnonblank},
    {"ngettext",	3, 4, FEARG_3,	    arg4_string_string_ਸੰਖਿਆ_string,
			"single, plural, number, domain",
			ret_string,	    f_ngettext},
    {"nr2char",		1, 2, FEARG_1,	    arg2_ਸੰਖਿਆ_bool,
			"expr, utf8",
			ret_string,	    f_nr2char},
    {"or",		2, 2, FEARG_1,	    arg2_ਸੰਖਿਆ,
			"expr1, expr2",
			ret_ਸੰਖਿਆ,	    f_or},
    {"pathshorten",	1, 2, FEARG_1,	    arg2_string_ਸੰਖਿਆ,
			"path, len",
			ret_string,	    f_pathshorten},
    {"perleval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERL
	    f_perleval
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"popup_atcursor",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ਸੰਖਿਆ,	    PROP_FUNC(f_popup_atcursor)},
    {"popup_beval",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ਸੰਖਿਆ,	    PROP_FUNC(f_popup_beval)},
    {"popup_clear",	0, 1, 0,	    arg1_bool,
			"force",
			ret_void,	    PROP_FUNC(f_popup_clear)},
    {"popup_close",	1, 2, FEARG_1,	    arg2_ਸੰਖਿਆ_any,
			"id, result",
			ret_void,	    PROP_FUNC(f_popup_close)},
    {"popup_create",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ਸੰਖਿਆ,	    PROP_FUNC(f_popup_create)},
    {"popup_dialog",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ਸੰਖਿਆ,	    PROP_FUNC(f_popup_dialog)},
    {"popup_filter_menu", 2, 2, 0,	    arg2_ਸੰਖਿਆ_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_menu)},
    {"popup_filter_yesno", 2, 2, 0,	    arg2_ਸੰਖਿਆ_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_yesno)},
    {"popup_findecho",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    PROP_FUNC(f_popup_findpreview)},
    {"popup_getoptions", 1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getoptions)},
    {"popup_getpos",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getpos)},
    {"popup_hide",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"id",
			ret_void,	    PROP_FUNC(f_popup_hide)},
    {"popup_list",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_list_ਸੰਖਿਆ,    PROP_FUNC(f_popup_list)},
    {"popup_locate",	2, 2, 0,	    arg2_ਸੰਖਿਆ,
			"row, col",
			ret_ਸੰਖਿਆ,	    PROP_FUNC(f_popup_locate)},
    {"popup_menu",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ਸੰਖਿਆ,	    PROP_FUNC(f_popup_menu)},
    {"popup_move",	2, 2, FEARG_1,	    arg2_ਸੰਖਿਆ_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_move)},
    {"popup_notification", 2, 2, FEARG_1,   arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ਸੰਖਿਆ,	    PROP_FUNC(f_popup_notification)},
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_ਸੰਖਿਆ_ਬਫਰ,
			"id, buf",
			ret_ਸੰਖਿਆ_bool,    PROP_FUNC(f_popup_setbuf)},
    {"popup_setoptions", 2, 2, FEARG_1,	    arg2_ਸੰਖਿਆ_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_setoptions)},
    {"popup_settext",	2, 2, FEARG_1,	    arg2_ਸੰਖਿਆ_string_or_list,
			"id, text",
			ret_void,	    PROP_FUNC(f_popup_settext)},
    {"popup_show",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"id",
			ret_ਸੰਖਿਆ,	    PROP_FUNC(f_popup_show)},
    {"pow",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"x, y",
			ret_float,	    f_pow},
    {"preinserted",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ਸੰਖਿਆ,	    f_prevnonblank},
    {"printf",		1, 19, FEARG_2,	    arg119_printf,
			"fmt, expr1",
			ret_string,	    f_printf},
    {"prompt_getprompt", 1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_string,	    ਕੰਮ_FUNC(f_prompt_getprompt)},
    {"prompt_setcallback", 2, 2, FEARG_1,   arg2_ਬਫਰ_any,
			"buf, expr",
			ret_void,	    ਕੰਮ_FUNC(f_prompt_setcallback)},
    {"prompt_setinterrupt", 2, 2, FEARG_1,  arg2_ਬਫਰ_any,
			"buf, expr",
			ret_void,	    ਕੰਮ_FUNC(f_prompt_setinterrupt)},
    {"prompt_setprompt", 2, 2, FEARG_1,	    arg2_ਬਫਰ_string,
			"buf, text",
			ret_void,	    ਕੰਮ_FUNC(f_prompt_setprompt)},
    {"prop_add",	3, 3, FEARG_1,	    arg3_ਸੰਖਿਆ_ਸੰਖਿਆ_dict,
			"lnum, col, props",
			ret_ਸੰਖਿਆ,	    PROP_FUNC(f_prop_ਜੋੜੋ)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_ਜੋੜੋ_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_ਸੰਖਿਆ_ਸੰਖਿਆ_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_ਲੱਭੋ)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_ਸੰਖਿਆ_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_ਸੰਖਿਆ_ਸੰਖਿਆ,
			"props, lnum, lnum_end",
			ret_ਸੰਖਿਆ,	    PROP_FUNC(f_prop_remove)},
    {"prop_type_add",	2, 2, FEARG_1,	    arg2_string_dict,
			"name, props",
			ret_void,	    PROP_FUNC(f_prop_type_ਜੋੜੋ)},
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
    {"pum_getpos",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_dict_ਸੰਖਿਆ,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
	    f_py3eval
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
	    f_pyeval
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ਜੇ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"rand",		0, 1, FEARG_1,	    arg1_list_ਸੰਖਿਆ,
			"expr",
			ret_ਸੰਖਿਆ,	    f_rand},
    {"range",		1, 3, FEARG_1,	    arg3_ਸੰਖਿਆ,
			"expr, max, stride",
			ret_list_ਸੰਖਿਆ,    f_range},
    {"readblob",	1, 3, FEARG_1,	    arg3_string_ਸੰਖਿਆ_ਸੰਖਿਆ,
			"fname, offset, size",
			ret_blob,	    f_readblob},
    {"readdir",		1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_string,    f_readdir},
    {"readdirex",	1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_dict_any,  f_readdirex},
    {"readfile",	1, 3, FEARG_1,	    arg3_string_string_ਸੰਖਿਆ,
			"fname, type, max",
			ret_list_string,    f_readfile},
    {"redraw_listener_add", 1, 1, FEARG_1,  arg1_dict_any,
			"opts",
			ret_ਸੰਖਿਆ,	    f_redraw_listener_ਜੋੜੋ},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_ਸੰਖਿਆ,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_reg_recording},
    {"reltime",		0, 2, FEARG_1,	    arg2_list_ਸੰਖਿਆ,
			"start, end",
			ret_list_ਸੰਖਿਆ,    f_reltime},
    {"reltimefloat",	1, 1, FEARG_1,	    arg1_list_ਸੰਖਿਆ,
			"time",
			ret_float,	    f_reltimefloat},
    {"reltimestr",	1, 1, FEARG_1,	    arg1_list_ਸੰਖਿਆ,
			"time",
			ret_string,	    f_reltimestr},
    {"remote_expr",	2, 4, FEARG_1,	    arg24_remote_ਵਿਅੰਜਕ,
			"server, string, idvar, timeout",
			ret_string,	    f_remote_ਵਿਅੰਜਕ},
    {"remote_foreground", 1, 1, FEARG_1,    arg1_string,
			"server",
			ret_void,	    f_remote_foreground},
    {"remote_peek",	1, 2, FEARG_1,	    arg2_string,
			"serverid, retvar",
			ret_ਸੰਖਿਆ,	    f_remote_peek},
    {"remote_read",	1, 2, FEARG_1,	    arg2_string_ਸੰਖਿਆ,
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
			ret_ਸੰਖਿਆ_bool,    f_rename},
    {"repeat",		2, 2, FEARG_1,	    arg2_repeat,
			"expr, count",
			ret_repeat,	    f_repeat},
    {"resolve",		1, 1, FEARG_1,	    arg1_string,
			"filename",
			ret_string,	    f_resolve},
    {"reverse",		1, 1, FEARG_1,	    arg1_ਉਲਟ,
			"object",
			ret_ਪਹਿਲਾ_arg,	    f_ਉਲਟ},
    {"round",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ਗੇੜਾ},
    {"rubyeval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RUBY
	    f_rubyeval
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"screenattr",	2, 2, FEARG_1,	    arg2_ਸੰਖਿਆ,
			"row, col",
			ret_ਸੰਖਿਆ,	    f_screenattr},
    {"screenchar",	2, 2, FEARG_1,	    arg2_ਸੰਖਿਆ,
			"row, col",
			ret_ਸੰਖਿਆ,	    f_screenchar},
    {"screenchars",	2, 2, FEARG_1,	    arg2_ਸੰਖਿਆ,
			"row, col",
			ret_list_ਸੰਖਿਆ,    f_screenchars},
    {"screencol",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_ਸੰਖਿਆ,
			"winid, lnum, col",
			ret_dict_ਸੰਖਿਆ,    f_screenpos},
    {"screenrow",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_screenrow},
    {"screenstring",	2, 2, FEARG_1,	    arg2_ਸੰਖਿਆ,
			"row, col",
			ret_string,	    f_screenstring},
    {"search",		1, 5, FEARG_1,	    arg15_ਖੋਜ,
			"pattern, flags, stopline, timeout, skip",
			ret_ਸੰਖਿਆ,	    f_ਖੋਜ},
    {"searchcount",	0, 1, FEARG_1,	    arg1_dict_any,
			"options",
			ret_dict_ਸੰਖਿਆ,    f_searchcount},
    {"searchdecl",	1, 3, FEARG_1,	    arg3_string_bool_bool,
			"name, global, thisblock",
			ret_ਸੰਖਿਆ_bool,    f_searchdecl},
    {"searchpair",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_ਸੰਖਿਆ,	    f_searchpair},
    {"searchpairpos",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_list_ਸੰਖਿਆ,    f_searchpairpos},
    {"searchpos",	1, 5, FEARG_1,	    arg15_ਖੋਜ,
			"pattern, flags, stopline, timeout, skip",
			ret_list_ਸੰਖਿਆ,    f_searchpos},
    {"server2client",	2, 2, FEARG_1,	    arg2_string,
			"clientid, string",
			ret_ਸੰਖਿਆ_bool,    f_server2client},
    {"serverlist",	0, 1, 0,	    arg1_dict_any,
			"dict",
			ret_string_or_any_1,	    f_serverlist},
    {"setbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_ਸੰਖਿਆ_bool,    f_setbufline},
    {"setbufvar",	3, 3, FEARG_3,	    arg3_ਬਫਰ_string_any,
			"buf, varname, val",
			ret_void,	    f_setbufvar},
    {"setcellwidths",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_void,	    f_setcellwidths},
    {"setcharpos",	2, 2, FEARG_2,	    arg2_string_list_ਸੰਖਿਆ,
			"expr, list",
			ret_ਸੰਖਿਆ_bool,    f_setcharpos},
    {"setcharsearch",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_setcharsearch},
    {"setcmdline",	1, 2, FEARG_1,	    arg2_string_ਸੰਖਿਆ,
			"str, pos",
			ret_ਸੰਖਿਆ_bool,    f_setcmdline},
    {"setcmdpos",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"pos",
			ret_ਸੰਖਿਆ_bool,    f_setcmdpos},
    {"setcursorcharpos", 1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_ਸੰਖਿਆ_bool,    f_setcursorcharpos},
    {"setenv",		2, 2, FEARG_2,	    arg2_string_any,
			"name, val",
			ret_void,	    f_setenv},
    {"setfperm",	2, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_ਸੰਖਿਆ_bool,    f_setfperm},
    {"setline",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_ਸੰਖਿਆ_bool,    f_setline},
    {"setloclist",	2, 4, FEARG_2,	    arg24_setloclist,
			"nr, list, action, what",
			ret_ਸੰਖਿਆ_bool,    f_setloclist},
    {"setmatches",	1, 2, FEARG_1,	    arg2_list_any_ਸੰਖਿਆ,
			"list, win",
			ret_ਸੰਖਿਆ_bool,    f_setmatches},
    {"setpos",		2, 2, FEARG_2,	    arg2_string_list_ਸੰਖਿਆ,
			"expr, list",
			ret_ਸੰਖਿਆ_bool,    f_setpos},
    {"setqflist",	1, 3, FEARG_1,	    arg13_setqflist,
			"list, action, what",
			ret_ਸੰਖਿਆ_bool,    f_setqflist},
    {"setreg",		2, 3, FEARG_2,	    arg3_string_any_string,
			"regname, value, options",
			ret_ਸੰਖਿਆ_bool,    f_setreg},
    {"settabvar",	3, 3, FEARG_3,	    arg3_ਸੰਖਿਆ_string_any,
			"tabnr, varname, val",
			ret_void,	    f_settabvar},
    {"settabwinvar",	4, 4, FEARG_4,	    arg4_ਸੰਖਿਆ_ਸੰਖਿਆ_string_any,
			"tabnr, winnr, varname, val",
			ret_void,	    f_settabwinvar},
    {"settagstack",	2, 3, FEARG_2,	    arg23_settagstack,
			"nr, dict, action",
			ret_ਸੰਖਿਆ_bool,    f_settagstack},
    {"setwinvar",	3, 3, FEARG_3,	    arg3_ਸੰਖਿਆ_string_any,
			"winnr, varname, val",
			ret_void,	    f_setwinvar},
    {"sha256",		1, 1, FEARG_1,	    arg1_string_or_blob,
			"expr",
			ret_string,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	    f_sha256
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"shellescape",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, special",
			ret_string,	    f_shellescape},
    {"shiftwidth",	0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"col",
			ret_ਸੰਖਿਆ,	    f_shiftwidth},
    {"sign_define",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"name, dict",
			ret_sign_define,	    SIGN_FUNC(f_sign_define)},
    {"sign_getdefined",	0, 1, FEARG_1,	    arg1_string,
			"name",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getdefined)},
    {"sign_getplaced",	0, 2, FEARG_1,	    arg02_sign_getplaced,
			"buf, dict",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getplaced)},
    {"sign_jump",	3, 3, FEARG_1,	    arg3_ਸੰਖਿਆ_string_ਬਫਰ,
			"id, group, buf",
			ret_ਸੰਖਿਆ,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_ਰੱਖੋ,
			"id, group, name, buf, dict",
			ret_ਸੰਖਿਆ,	    SIGN_FUNC(f_sign_ਰੱਖੋ)},
    {"sign_placelist",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_ਸੰਖਿਆ,    SIGN_FUNC(f_sign_placelist)},
    {"sign_undefine",	0, 1, FEARG_1,	    arg1_string_or_list_string,
			"name",
			ret_ਸੰਖਿਆ_bool,    SIGN_FUNC(f_sign_undefine)},
    {"sign_unplace",	1, 2, FEARG_1,	    arg2_string_dict,
			"group, dict",
			ret_ਸੰਖਿਆ_bool,    SIGN_FUNC(f_sign_unplace)},
    {"sign_unplacelist", 1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_ਸੰਖਿਆ,    SIGN_FUNC(f_sign_unplacelist)},
    {"simplify",	1, 1, FEARG_1,	    arg1_string,
			"filename",
			ret_string,	    f_simplify},
    {"sin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sin},
    {"sinh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sinh},
    {"slice",		2, 3, FEARG_1,	    arg23_ਟੁਕੜਾ,
			"expr, start, end",
			ret_ਟੁਕੜਾ,	    f_ਟੁਕੜਾ},
    {"sort",		1, 3, FEARG_1,	    arg13_sortuniq,
			"list, how, dict",
			ret_ਪਹਿਲਾ_arg,	    f_ਛਾਂਟੋ},
    {"sound_clear",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_void,	    SOUND_FUNC(f_sound_clear)},
    {"sound_playevent",	1, 2, FEARG_1,	    arg2_string_any,
			"name, callback",
			ret_ਸੰਖਿਆ,	    SOUND_FUNC(f_sound_playevent)},
    {"sound_playfile",	1, 2, FEARG_1,	    arg2_string_any,
			"path, callback",
			ret_ਸੰਖਿਆ,	    SOUND_FUNC(f_sound_playfile)},
    {"sound_stop",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"id",
			ret_void,	    SOUND_FUNC(f_sound_stop)},
    {"soundfold",	1, 1, FEARG_1,	    arg1_string,
			"word",
			ret_string,	    f_soundfold},
    {"spellbadword",	0, 1, FEARG_1,	    arg1_string,
			"sentence",
			ret_list_string,    f_spellbadword},
    {"spellsuggest",	1, 3, FEARG_1,	    arg3_string_ਸੰਖਿਆ_bool,
			"word, max, capital",
			ret_list_string,    f_spellsuggest},
    {"split",		1, 3, FEARG_1,	    arg3_string_string_bool,
			"string, pattern, keepempty",
			ret_list_string,    f_ਵੰਡੋ},
    {"sqrt",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sqrt},
    {"srand",		0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"expr",
			ret_list_ਸੰਖਿਆ,    f_srand},
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
			ret_list_ਸੰਖਿਆ,    f_str2list},
    {"str2nr",		1, 3, FEARG_1,	    arg3_string_ਸੰਖਿਆ_bool,
			"string, base, quoted",
			ret_ਸੰਖਿਆ,	    f_str2nr},
    {"strcharlen",	1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_ਸੰਖਿਆ,	    f_strcharlen},
    {"strcharpart",	2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, skipcc",
			ret_string,	    f_strcharpart},
    {"strchars",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, skipcc",
			ret_ਸੰਖਿਆ,	    f_strchars},
    {"strdisplaywidth",	1, 2, FEARG_1,	    arg2_string_ਸੰਖਿਆ,
			"string, col",
			ret_ਸੰਖਿਆ,	    f_strdisplaywidth},
    {"strftime",	1, 2, FEARG_1,	    arg2_string_ਸੰਖਿਆ,
			"format, time",
			ret_string,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRFTIME
	    f_strftime
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_ਸੰਖਿਆ,
			"str, index",
			ret_ਸੰਖਿਆ,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_ਸੰਖਿਆ,
			"haystack, needle, start",
			ret_ਸੰਖਿਆ,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, ਨਿਰਾ,
			"expr",
			ret_string,	    f_string},
    {"strlen",		1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_ਸੰਖਿਆ,	    f_strlen},
    {"strpart",		2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, chars",
			ret_string,	    f_strpart},
    {"strptime",	2, 2, FEARG_1,	    arg2_string,
			"format, timestring",
			ret_ਸੰਖਿਆ,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRPTIME
	    f_strptime
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"strridx",		2, 3, FEARG_1,	    arg3_string_string_ਸੰਖਿਆ,
			"haystack, needle, start",
			ret_ਸੰਖਿਆ,	    f_strridx},
    {"strtrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_strtrans},
    {"strutf16len",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, countcc",
			ret_ਸੰਖਿਆ,	    f_strutf16len},
    {"strwidth",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_ਸੰਖਿਆ,	    f_strwidth},
    {"submatch",	1, 2, FEARG_1,	    arg2_ਸੰਖਿਆ_bool,
			"nr, list",
			ret_string_or_any_2,	    f_submatch},
    {"substitute",	4, 4, FEARG_1,	    arg4_string_string_any_string,
			"string, pat, sub, flags",
			ret_string,	    f_substitute},
    {"swapfilelist",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_string,	    f_swapname},
    {"synID",		3, 3, 0,	    arg3_lnum_ਸੰਖਿਆ_bool,
			"lnum, col, trans",
			ret_ਸੰਖਿਆ,	    f_synID},
    {"synIDattr",	2, 3, FEARG_1,	    arg3_ਸੰਖਿਆ_string_string,
			"synID, what, mode",
			ret_string,	    f_synIDattr},
    {"synIDtrans",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"synID",
			ret_ਸੰਖਿਆ,	    f_synIDtrans},
    {"synconcealed",	2, 2, 0,	    arg2_lnum_ਸੰਖਿਆ,
			"lnum, col",
			ret_list_any,	    f_synconcealed},
    {"synstack",	2, 2, 0,	    arg2_lnum_ਸੰਖਿਆ,
			"lnum, col",
			ret_list_ਸੰਖਿਆ,    f_synstack},
    {"system",		1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_string,	    f_system},
    {"systemlist",	1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_list_string,    f_systemlist},
    {"tabpagebuflist",	0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"arg",
			ret_list_ਸੰਖਿਆ,    f_tabpagebuflist},
    {"tabpagenr",	0, 1, 0,	    arg1_string,
			"arg",
			ret_ਸੰਖਿਆ,	    f_tabpagenr},
    {"tabpagewinnr",	1, 2, FEARG_1,	    arg2_ਸੰਖਿਆ_string,
			"tabarg, arg",
			ret_ਸੰਖਿਆ,	    f_tabpagewinnr},
    {"tabpanel_getinfo", 0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_ਸੰਖਿਆ_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
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
    {"tempname",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_ਸੰਖਿਆ,	    ਪਦ_FUNC(f_ਪਦ_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_ਸੰਖਿਆ,	    ਪਦ_FUNC(f_ਪਦ_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_ਬਫਰ_string_dict,
			"buf, filename, options",
			ret_void,	    ਪਦ_FUNC(f_ਪਦ_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_ਬਫਰ,
			"buf",
			ret_ਸੰਖਿਆ,	    ਪਦ_FUNC(f_ਪਦ_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_ਬਫਰ,
			"buf",
			ret_list_string,
#ਜੇ defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_ਪਦ_getansicolors
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_ਸੰਖਿਆ_string,
			"attr, what",
			ret_ਸੰਖਿਆ,	    ਪਦ_FUNC(f_ਪਦ_getattr)},
    {"term_getcursor",	1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_list_any,	    ਪਦ_FUNC(f_ਪਦ_getcursor)},
    {"term_getjob",	1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_ਕੰਮ,	    ਪਦ_FUNC(f_ਪਦ_getjob)},
    {"term_getline",	2, 2, FEARG_1,	    arg2_ਬਫਰ_lnum,
			"buf, row",
			ret_string,	    ਪਦ_FUNC(f_ਪਦ_getline)},
    {"term_getscrolled", 1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_ਸੰਖਿਆ,	    ਪਦ_FUNC(f_ਪਦ_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_list_ਸੰਖਿਆ,    ਪਦ_FUNC(f_ਪਦ_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_string,	    ਪਦ_FUNC(f_ਪਦ_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_string,	    ਪਦ_FUNC(f_ਪਦ_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_ਬਫਰ_bool,
			"buf, input",
			ret_string,	    ਪਦ_FUNC(f_ਪਦ_gettty)},
    {"term_list",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_list_ਸੰਖਿਆ,    ਪਦ_FUNC(f_ਪਦ_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_ਬਫਰ_lnum,
			"buf, row",
			ret_list_dict_any,  ਪਦ_FUNC(f_ਪਦ_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_ਬਫਰ_string,
			"buf, keys",
			ret_void,	    ਪਦ_FUNC(f_ਪਦ_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_ਬਫਰ_list_any,
			"buf, colors",
			ret_void,
#ਜੇ defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_ਪਦ_setansicolors
#ਨਹੀਂਤਾਂ
	    ਨਿਰਾ
#ਜੇ_ਅੰਤ
			},
    {"term_setapi",	2, 2, FEARG_1,	    arg2_ਬਫਰ_string,
			"buf, expr",
			ret_void,	    ਪਦ_FUNC(f_ਪਦ_setapi)},
    {"term_setkill",	2, 2, FEARG_1,	    arg2_ਬਫਰ_string,
			"buf, how",
			ret_void,	    ਪਦ_FUNC(f_ਪਦ_setkill)},
    {"term_setrestore",	2, 2, FEARG_1,	    arg2_ਬਫਰ_string,
			"buf, command",
			ret_void,	    ਪਦ_FUNC(f_ਪਦ_setrestore)},
    {"term_setsize",	3, 3, FEARG_1,	    arg3_ਬਫਰ_ਸੰਖਿਆ_ਸੰਖਿਆ,
			"buf, rows, cols",
			ret_void,	    ਪਦ_FUNC(f_ਪਦ_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_ਸੰਖਿਆ,	    ਪਦ_FUNC(f_ਪਦ_ਸ਼ੁਰੂ)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_ਬਫਰ_ਸੰਖਿਆ,
			"buf, time",
			ret_void,	    ਪਦ_FUNC(f_ਪਦ_wait)},
    {"terminalprops",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_ਸੰਖਿਆ,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   ਨਿਰਾ,
			ਨਿਰਾ,
			ret_void,	    f_test_garbagecollect_soon},
    {"test_getvalue",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_ਸੰਖਿਆ,	    f_test_getvalue},
    {"test_gui_event",	2, 2, FEARG_1,	    arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_gui_event},
    {"test_ignore_error", 1, 1, FEARG_1,    arg1_string,
			"expr",
			ret_void,	    f_test_ignore_error},
    {"test_mswin_event", 2, 2, FEARG_1,     arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_mswin_event},
    {"test_null_blob",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_channel,	    ਕੰਮ_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਕੰਮ,	    ਕੰਮ_FUNC(f_test_null_ਕੰਮ)},
    {"test_null_list",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_ਸੈੱਟ},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_ਸੰਖਿਆ,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, ਨਿਰਾ,
			"expr",
			ret_ਸੰਖਿਆ,	    f_test_refcount},
    {"test_setmouse",	2, 2, 0,	    arg2_ਸੰਖਿਆ,
			"row, col",
			ret_void,	    f_test_setmouse},
    {"test_settime",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"expr",
			ret_void,	    f_test_settime},
    {"test_srand_seed",	0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"seed",
			ret_void,	    f_test_srand_ਬੀਜ},
    {"test_unknown",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_ਸੰਖਿਆ_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_ਸੰਖਿਆ_any_dict,
			"time, callback, options",
			ret_ਸੰਖਿਆ,	    TIMER_FUNC(f_timer_ਸ਼ੁਰੂ)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
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
    {"trim",		1, 3, FEARG_1,	    arg3_string_string_ਸੰਖਿਆ,
			"text, mask, dir",
			ret_string,	    f_trim},
    {"trunc",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_trunc},
    {"tuple2list",	1, 1, FEARG_1,	    arg1_tuple_any,
			"tuple",
			ret_list_any,	    f_tuple2list},
    {"type",		1, 1, FEARG_1|FE_X, ਨਿਰਾ,
			"expr",
			ret_ਸੰਖਿਆ,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, ਨਿਰਾ,
			"expr",
			ret_string,	    f_typename},
    {"undofile",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_string,	    f_undofile},
    {"undotree",	0, 1, FEARG_1,	    arg1_ਬਫਰ,
			"buf",
			ret_dict_any,	    f_undotree},
    {"uniq",		1, 3, FEARG_1,	    arg13_sortuniq,
			"list, func, dict",
			ret_ਪਹਿਲਾ_arg,	    f_uniq},
    {"uri_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_uridecode},
    {"uri_encode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_uriencode},
    {"utf16idx",	2, 4, FEARG_1,	    arg4_string_ਸੰਖਿਆ_bool_bool,
			"string, idx, countcc, charidx",
			ret_ਸੰਖਿਆ,	    f_utf16idx},
    {"values",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_member,    f_ਮੁੱਲਾਂ},
    {"virtcol",		1, 3, FEARG_1,	    arg3_string_or_list_bool_ਸੰਖਿਆ,
			"expr, list, winid",
			ret_virtcol,	    f_virtcol},
    {"virtcol2col",	3, 3, FEARG_1,	    arg3_ਸੰਖਿਆ,
			"winid, lnum, col",
			ret_ਸੰਖਿਆ,	    f_virtcol2col},
    {"visualmode",	0, 1, 0,	    arg1_bool,
			"expr",
			ret_string,	    f_visualmode},
    {"wildmenumode",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_void,	    f_wildtrigger},
    {"win_execute",	2, 3, FEARG_2,	    arg23_win_execute,
			"id, command, silent",
			ret_string,	    f_win_execute},
    {"win_findbuf",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"bufnr",
			ret_list_ਸੰਖਿਆ,    f_win_findbuf},
    {"win_getid",	0, 2, FEARG_1,	    arg2_ਸੰਖਿਆ,
			"win, tab",
			ret_ਸੰਖਿਆ,	    f_win_getid},
    {"win_gettype",	0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"nr",
			ret_string,	    f_win_gettype},
    {"win_gotoid",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"expr",
			ret_ਸੰਖਿਆ_bool,    f_win_gotoid},
    {"win_id2tabwin",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"expr",
			ret_list_ਸੰਖਿਆ,    f_win_id2tabwin},
    {"win_id2win",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"expr",
			ret_ਸੰਖਿਆ,	    f_win_id2win},
    {"win_move_separator", 2, 2, FEARG_1,   arg2_ਸੰਖਿਆ,
			"nr, offset",
			ret_ਸੰਖਿਆ_bool,    f_win_move_separator},
    {"win_move_statusline", 2, 2, FEARG_1,  arg2_ਸੰਖਿਆ,
			"nr, offset",
			ret_ਸੰਖਿਆ_bool,    f_win_move_statusline},
    {"win_screenpos",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"nr",
			ret_list_ਸੰਖਿਆ,    f_win_screenpos},
    {"win_splitmove",   2, 3, FEARG_1,	    arg3_ਸੰਖਿਆ_ਸੰਖਿਆ_dict,
			"nr, target, options",
			ret_ਸੰਖਿਆ_bool,    f_win_splitmove},
    {"winbufnr",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"nr",
			ret_ਸੰਖਿਆ,	    f_winbufnr},
    {"wincol",		0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"nr",
			ret_ਸੰਖਿਆ,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_ਸੰਖਿਆ,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_ਸੰਖਿਆ,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_dict_ਸੰਖਿਆ,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_ਸੰਖਿਆ,
			"nr",
			ret_ਸੰਖਿਆ,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    ਨਿਰਾ,
			ਨਿਰਾ,
			ret_dict_ਸੰਖਿਆ,    f_wordcount},
    {"writefile",	2, 3, FEARG_1,	    arg23_writefile,
			"object, fname, flags",
			ret_ਸੰਖਿਆ_bool,    f_writefile},
    {"xor",		2, 2, FEARG_1,	    arg2_ਸੰਖਿਆ,
			"expr1, expr2",
			ret_ਸੰਖਿਆ,	    f_xor},
};

/*
 * Return true if specified function allows a type as an argument.
 */
    ਸਥਿਰ ਪੂਰਨ
func_allows_type(ਪੂਰਨ idx)
{
    ਵਾਪਸ (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, ਪੂਰਨ idx)
{
    ਸਥਿਰ ਪੂਰਨ	intidx = -1;
    char_u	*name;

    ਜੇ (idx == 0)
	intidx = -1;
    ਜੇ (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	ਜੇ (name != ਨਿਰਾ)
	{
	    ਜੇ (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_ਨਮੂਨਾ, 2) == 0)
		ਵਾਪਸ cat_ਅਗੇਤਰ_varname('g', name);
	    ਵਾਪਸ name;
	}
    }
    ਜੇ (++intidx < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	ਜੇ (global_functions[intidx].f_func == ਨਿਰਾ)
	    ਵਾਪਸ (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	ਜੇ (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	ਵਾਪਸ IObuff;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_ਵਿਅੰਜਕ_name(expand_T *xp, ਪੂਰਨ idx)
{
    ਸਥਿਰ ਪੂਰਨ	intidx = -1;
    char_u	*name;

    ਜੇ (idx == 0)
	intidx = -1;
    ਜੇ (intidx < 0)
    {
	name = get_function_name(xp, idx);
	ਜੇ (name != ਨਿਰਾ)
	    ਵਾਪਸ name;
    }
    ਵਾਪਸ get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_ਅੰਦਰੂਨੀ_func_opt(char_u *name, ਪੂਰਨ implemented)
{
    ਪੂਰਨ		ਪਹਿਲਾ = 0;
    ਪੂਰਨ		ਆਖਰੀ;
    ਪੂਰਨ		cmp;
    ਪੂਰਨ		x;

    ਆਖਰੀ = (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(global_functions) - 1;

    // Find the function name in the table. Binary search.
    ਜਦੋਂ (ਪਹਿਲਾ <= ਆਖਰੀ)
    {
	x = ਪਹਿਲਾ + ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਆਖਰੀ - ਪਹਿਲਾ) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	ਜੇ (cmp < 0)
	    ਆਖਰੀ = x - 1;
	ਨਹੀਂਤਾਂ ਜੇ (cmp > 0)
	    ਪਹਿਲਾ = x + 1;
	ਨਹੀਂਤਾਂ ਜੇ (implemented && global_functions[x].f_func == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ x;
    }
    ਵਾਪਸ -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    ਪੂਰਨ
ਲੱਭੋ_ਅੰਦਰੂਨੀ_func(char_u *name)
{
    ਵਾਪਸ ਲੱਭੋ_ਅੰਦਰੂਨੀ_func_opt(name, TRUE);
}

    ਪੂਰਨ
has_ਅੰਦਰੂਨੀ_func(char_u *name)
{
    ਵਾਪਸ ਲੱਭੋ_ਅੰਦਰੂਨੀ_func_opt(name, TRUE) >= 0;
}

    ਸਥਿਰ ਪੂਰਨ
has_ਅੰਦਰੂਨੀ_func_name(char_u *name)
{
    ਵਾਪਸ ਲੱਭੋ_ਅੰਦਰੂਨੀ_func_opt(name, FALSE) >= 0;
}

    ਅੱਖਰ *
ਅੰਦਰੂਨੀ_func_name(ਪੂਰਨ idx)
{
    ਵਾਪਸ global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    ਪੂਰਨ
ਅੰਦਰੂਨੀ_func_check_arg_types(
	type2_T	*types,
	ਪੂਰਨ	idx,
	ਪੂਰਨ	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    ਜੇ (!(func_allows_type(idx)))
    {
	ਲਈ (ਪੂਰਨ i = 0; i < argcount; ++i)
	    ਜੇ (check_type_is_ਮੁੱਲ(types[i].type_curr) == FAIL)
		ਵਾਪਸ FAIL;
    }

    ਅਚਲ argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    ਜੇ (argchecks == ਨਿਰਾ)
	ਵਾਪਸ OK;

    argcontext_T context;

    context.arg_ਗਿਣਤੀ = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    ਲਈ (ਪੂਰਨ i = 0; i < argcount && argchecks[i] != ਨਿਰਾ; ++i)
    {
	context.arg_idx = i;
	ਜੇ (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    ਖਾਲੀ
ਅੰਦਰੂਨੀ_func_get_argcount(ਪੂਰਨ idx, ਪੂਰਨ *argcount, ਪੂਰਨ *min_argcount)
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
ਅੰਦਰੂਨੀ_func_ret_type(
	ਪੂਰਨ	    idx,
	ਪੂਰਨ	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = ਨਿਰਾ;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    ਜੇ (*decl_type == ਨਿਰਾ)
	*decl_type = ret;
    current_type_gap = ਨਿਰਾ;
    ਵਾਪਸ ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    ਪੂਰਨ
ਅੰਦਰੂਨੀ_func_is_map(ਪੂਰਨ idx)
{
    ਵਾਪਸ global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    ਪੂਰਨ
check_ਅੰਦਰੂਨੀ_func(ਪੂਰਨ idx, ਪੂਰਨ argcount)
{
    funcerror_T	    res;
    ਅੱਖਰ	    *name;

    ਜੇ (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    ਨਹੀਂਤਾਂ ਜੇ (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ global_functions[idx].f_argtype & FEARG_ਨਕਾਬ;

    name = ਅੰਦਰੂਨੀ_func_name(idx);
    ਜੇ (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    ਨਹੀਂਤਾਂ
	semsg(_(e_not_enough_arguments_for_function_str), name);
    ਵਾਪਸ -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
check_ਦਲੀਲਾਂ_for_type(ਪੂਰਨ idx, ਪੂਰਨ argcount, typval_T *argvars)
{
    ਜੇ (!func_allows_type(idx))
    {
	ਲਈ (ਪੂਰਨ i = 0; i < argcount; ++i)
	    ਜੇ (check_typval_is_ਮੁੱਲ(&argvars[i]) == FAIL)
		ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

    funcerror_T
call_ਅੰਦਰੂਨੀ_func(
	char_u	    *name,
	ਪੂਰਨ	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    ਪੂਰਨ i;

    i = ਲੱਭੋ_ਅੰਦਰੂਨੀ_func(name);
    ਜੇ (i < 0)
	ਵਾਪਸ FCERR_UNKNOWN;
    ਜੇ (argcount < global_functions[i].f_min_argc)
	ਵਾਪਸ FCERR_TOOFEW;
    ਜੇ (argcount > global_functions[i].f_max_argc)
	ਵਾਪਸ FCERR_TOOMANY;
    ਜੇ (check_ਦਲੀਲਾਂ_for_type(i, argcount, argvars) == FAIL)
	ਵਾਪਸ FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    ਜੇ (ਵਿੱਚ_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    ਵਾਪਸ FCERR_NONE;
}

    ਖਾਲੀ
call_ਅੰਦਰੂਨੀ_func_by_idx(
	ਪੂਰਨ	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    ਜੇ (ਵਿੱਚ_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_ਅੰਦਰੂਨੀ_ਵਿਧੀ(
	char_u	    *name,
	ਪੂਰਨ	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    ਪੂਰਨ		fi;
    typval_T	argv[MAX_FUNC_ਦਲੀਲਾਂ + 1];

    fi = ਲੱਭੋ_ਅੰਦਰੂਨੀ_func(name);
    ਜੇ (fi < 0)
	ਵਾਪਸ FCERR_UNKNOWN;
    ਜੇ ((global_functions[fi].f_argtype & FEARG_ਨਕਾਬ) == 0)
	ਵਾਪਸ FCERR_NOTMETHOD;
    ਜੇ (argcount + 1 < global_functions[fi].f_min_argc)
	ਵਾਪਸ FCERR_TOOFEW;
    ਜੇ (argcount + 1 > global_functions[fi].f_max_argc)
	ਵਾਪਸ FCERR_TOOMANY;
    ਜੇ (check_ਦਲੀਲਾਂ_for_type(fi, argcount, argvars) == FAIL)
	ਵਾਪਸ FCERR_OTHER;

    ਜੇ ((global_functions[fi].f_argtype & FEARG_ਨਕਾਬ) == FEARG_2)
    {
	ਜੇ (argcount < 1)
	    ਵਾਪਸ FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	ਲਈ (ਪੂਰਨ i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ਨਹੀਂਤਾਂ ਜੇ ((global_functions[fi].f_argtype & FEARG_ਨਕਾਬ) == FEARG_3)
    {
	ਜੇ (argcount < 2)
	    ਵਾਪਸ FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	ਲਈ (ਪੂਰਨ i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ਨਹੀਂਤਾਂ ਜੇ ((global_functions[fi].f_argtype & FEARG_ਨਕਾਬ) == FEARG_4)
    {
	ਜੇ (argcount < 3)
	    ਵਾਪਸ FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	ਲਈ (ਪੂਰਨ i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ਨਹੀਂਤਾਂ
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	ਲਈ (ਪੂਰਨ i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    ਜੇ (check_ਦਲੀਲਾਂ_for_type(fi, argcount + 1, argv) == FAIL)
	ਵਾਪਸ FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    ਵਾਪਸ FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    ਪੂਰਨ
non_zero_arg(typval_T *argvars)
{
    ਵਾਪਸ ((argvars[0].v_type == VAR_ਸੰਖਿਆ
		&& argvars[0].vval.v_ਸੰਖਿਆ != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_ਸੰਖਿਆ == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != ਨਿਰਾ
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_and(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ)
					& tv_get_ਸੰਖਿਆ_chk(&argvars[1], ਨਿਰਾ);
}

/*
 * "balloon_show()" function
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL
    ਸਥਿਰ ਖਾਲੀ
f_balloon_gettext(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    ਜੇ (balloonEval == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (balloonEval->ਸੁਨੇਹਾ == ਨਿਰਾ)
	rettv->vval.v_string = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_string = vim_strsave(balloonEval->ਸੁਨੇਹਾ);
}

    ਸਥਿਰ ਖਾਲੀ
f_balloon_show(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (balloonEval == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.ਵਿੱਚ_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, ਨਿਰਾ,
		l == ਨਿਰਾ || l->lv_len == 0 ? ਨਿਰਾ : l);
    }
    ਨਹੀਂਤਾਂ
    {
	char_u *mesg;

	ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    ਵਾਪਸ;

	mesg = tv_get_string_chk(&argvars[0]);
	ਜੇ (mesg != ਨਿਰਾ)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? ਨਿਰਾ : mesg, ਨਿਰਾ);
    }
}

# ਜੇ defined(FEAT_BEVAL_ਪਦ)
    ਸਥਿਰ ਖਾਲੀ
f_balloon_ਵੰਡੋ(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (rettv_list_alloc(rettv) != OK)
	ਵਾਪਸ;

    char_u *ਸੁਨੇਹਾ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;
    ਸੁਨੇਹਾ = tv_get_string_chk(&argvars[0]);
    ਜੇ (ਸੁਨੇਹਾ != ਨਿਰਾ)
    {
	pumitem_T	*array;
	ਪੂਰਨ		ਮਾਪ = ਵੰਡੋ_ਸੁਨੇਹਾ(ਸੁਨੇਹਾ, &array);

	// Skip the first and last item, they are always empty.
	ਲਈ (ਪੂਰਨ i = 1; i < ਮਾਪ - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_ਪਾਠ, -1);
	ਜਦੋਂ (ਮਾਪ > 0)
	    vim_free(array[--ਮਾਪ].pum_ਪਾਠ);
	vim_free(array);
    }
}
# endif
#ਜੇ_ਅੰਤ

/*
 * "base64_decode(string)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (rettv_blob_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    ਜੇ (str != ਨਿਰਾ)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (check_for_blob_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    blob_T *blob = argvars->vval.v_blob;
    ਜੇ (blob != ਨਿਰਾ)
	rettv->vval.v_string =
	    base64_encode(blob->bv_ga.ga_ਅੰਕੜੇ, blob->bv_ga.ga_len);
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
    ਜੇ (buf == ਨਿਰਾ)
	semsg(_(e_invalid_ਬਫਰ_name_str), tv_get_string(arg));
    ਵਾਪਸ buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ਸੰਖਿਆ = VVAL_TRUE;

    ਜੇ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;

    ਜੇ (strcmp((ਅਚਲ ਅੱਖਰ *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(&argvars[0]));
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (bindtextdomain((ਅਚਲ ਅੱਖਰ *)argvars[0].vval.v_string, (ਅਚਲ ਅੱਖਰ *)argvars[1].vval.v_string) == ਨਿਰਾ)
	{
	    do_outofmem_ਸੁਨੇਹਾ((ਲੰਮਾ)0);
	    rettv->vval.v_ਸੰਖਿਆ = VVAL_FALSE;
	}
    }

    ਵਾਪਸ;
}

/*
 * "byte2line(byte)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_byte2line(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_BYTEOFF
    rettv->vval.v_ਸੰਖਿਆ = -1;
#ਨਹੀਂਤਾਂ
    ਲੰਮਾ	boff = 0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    boff = tv_get_ਸੰਖਿਆ(&argvars[0]) - 1;  // boff gets -1 on type error
    ਜੇ (boff < 0)
	rettv->vval.v_ਸੰਖਿਆ = -1;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = ml_ਲੱਭੋ_ਸਤਰ_or_ਵਿੱਥ(curbuf,
							  (linenr_T)0, &boff);
#ਜੇ_ਅੰਤ
}

/*
 * "call(func, arglist [, dict])" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = ਨਿਰਾ;
    dict_T	*selfdict = ਨਿਰਾ;
    char_u	*tofree = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    ਜੇ (check_for_list_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;
    ਜੇ (argvars[1].vval.v_list == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    ਨਹੀਂਤਾਂ
	func = tv_get_string(&argvars[0]);
    ਜੇ (func == ਨਿਰਾ || *func == NUL)
	ਵਾਪਸ;		// type error, empty name or null function

    ਜੇ (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, ਨਿਰਾ, FALSE, TFN_INT|TFN_QUIET);
	ਜੇ (tofree == ਨਿਰਾ)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    ਵਾਪਸ;
	}
	ਜੇ (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_ਦਲੀਲਾਂ(tofree, STRLEN(tofree),
									&p);
	    ਜੇ (s != ਨਿਰਾ)
	    {
		vim_free(tofree);
		tofree = s;
	    }
	}
	func = tofree;
    }

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ਜੇ (check_for_dict_arg(argvars, 2) == FAIL)
	    ਜਾਓ ਪੂਰਾ;

	selfdict = argvars[2].vval.v_dict;
    }

    (ਖਾਲੀ)func_call(func, &argvars[1], partial, selfdict, rettv);

ਪੂਰਾ:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_changenr(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (has_mbyte)
    {
	ਪੂਰਨ	utf8 = 0;

	ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (ਪੂਰਨ)tv_get_bool_chk(&argvars[1], ਨਿਰਾ);

	ਜੇ (utf8)
	    rettv->vval.v_ਸੰਖਿਆ = utf_ptr2char(tv_get_string(&argvars[0]));
	ਨਹੀਂਤਾਂ
	    rettv->vval.v_ਸੰਖਿਆ = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    ਸਥਿਰ ਖਾਲੀ
get_ਖਾਨਾ(typval_T *argvars, typval_T *rettv, ਪੂਰਨ charcol)
{
    colnr_T	ਖਾਨਾ = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    ਪੂਰਨ		winchanged = FALSE;

    ਜੇ (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[1]), &tp);
	ਜੇ (wp == ਨਿਰਾ || tp == ਨਿਰਾ)
	    ਵਾਪਸ;

	ਜੇ (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    ਵਾਪਸ;

	check_cursor();
	winchanged = TRUE;
    }

    ਪੂਰਨ fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    ਜੇ (fp != ਨਿਰਾ && fnum == curbuf->b_fnum)
    {
	ਜੇ (fp->ਖਾਨਾ == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    ਜੇ (fp->lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ਖਾਨਾ = ml_get_len(fp->lnum) + 1;
	    ਨਹੀਂਤਾਂ
		ਖਾਨਾ = MAXCOL;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਖਾਨਾ = fp->ਖਾਨਾ + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    ਜੇ (virtual_ਸਰਗਰਮ() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		ਜੇ (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    ਪੂਰਨ		l;

		    ਜੇ (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			ਖਾਨਾ += l;
		}
	    }
	}
    }
    rettv->vval.v_ਸੰਖਿਆ = ਖਾਨਾ;

    ਜੇ (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_ਖਾਨਾ(argvars, rettv, TRUE);
}

    win_T *
get_optional_ਖਿੜਕੀ(typval_T *argvars, ਪੂਰਨ idx)
{
    win_T   *win = curwin;

    ਜੇ (argvars[idx].v_type == VAR_UNKNOWN)
	ਵਾਪਸ curwin;

    win = ਲੱਭੋ_win_by_nr_or_id(&argvars[idx]);
    ਜੇ (win == ਨਿਰਾ)
    {
	emsg(_(e_invalid_ਖਿੜਕੀ_ਸੰਖਿਆ));
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ win;
}

/*
 * "col(string)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_ਖਾਨਾ(typval_T *argvars, typval_T *rettv)
{
    get_ਖਾਨਾ(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_confirm(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*ਸੁਨੇਹਾ_2;
    char_u	*buttons = ਨਿਰਾ;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    ਪੂਰਨ		def = 1;
    ਪੂਰਨ		type = VIM_GENERIC;
    char_u	*typestr;
    ਪੂਰਨ		error = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	ਵਾਪਸ;

    ਸੁਨੇਹਾ_2 = tv_get_string_chk(&argvars[0]);
    ਜੇ (ਸੁਨੇਹਾ_2 == ਨਿਰਾ)
	error = TRUE;
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	ਜੇ (buttons == ਨਿਰਾ)
	    error = TRUE;
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
	    ਜੇ (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		ਜੇ (typestr == ਨਿਰਾ)
		    error = TRUE;
		ਨਹੀਂਤਾਂ
		{
		    ਚੋਣ (TOUPPER_ASC(*typestr))
		    {
			ਹਾਲਤ 'E': type = VIM_ERROR; ਰੋਕੋ;
			ਹਾਲਤ 'Q': type = VIM_QUESTION; ਰੋਕੋ;
			ਹਾਲਤ 'I': type = VIM_INFO; ਰੋਕੋ;
			ਹਾਲਤ 'W': type = VIM_WARNING; ਰੋਕੋ;
			ਹਾਲਤ 'G': type = VIM_GENERIC; ਰੋਕੋ;
		    }
		}
	    }
	}
    }

    ਜੇ (buttons == ਨਿਰਾ || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    ਜੇ (!error)
	rettv->vval.v_ਸੰਖਿਆ = do_dialog(type, ਨਿਰਾ, ਸੁਨੇਹਾ_2, buttons,
							    def, ਨਿਰਾ, FALSE);
#ਜੇ_ਅੰਤ
}

/*
 * "copy()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_copy(typval_T *argvars, typval_T *rettv)
{
    ਵਸਤੂ_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_cursorpos(typval_T *argvars, typval_T *rettv, ਪੂਰਨ charcol)
{
    ਲੰਮਾ	lnum, ਖਾਨਾ;
    ਲੰਮਾ	coladd = 0;
    ਪੂਰਨ		ਸੈੱਟ_curswant = TRUE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_or_ਸੰਖਿਆ_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = -1;
    ਜੇ (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	ਜੇ (list2fpos(argvars, &pos, ਨਿਰਾ, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_ਦਲੀਲ));
	    ਵਾਪਸ;
	}
	lnum = pos.lnum;
	ਖਾਨਾ = pos.ਖਾਨਾ;
	coladd = pos.coladd;
	ਜੇ (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    ਸੈੱਟ_curswant = FALSE;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ ((argvars[0].v_type == VAR_ਸੰਖਿਆ
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_ਸੰਖਿਆ
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	ਜੇ (lnum < 0)
	    semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(&argvars[0]));
	ਨਹੀਂਤਾਂ ਜੇ (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	ਖਾਨਾ = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[1], ਨਿਰਾ);
	ਜੇ (charcol)
	    ਖਾਨਾ = buf_charidx_ਮੰਜ਼ਿਲ_byteidx(curbuf, lnum, ਖਾਨਾ) + 1;
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਵਾਪਸ;
    }
    ਜੇ (lnum < 0 || ਖਾਨਾ < 0 || coladd < 0)
	ਵਾਪਸ;		// type error; errmsg already given
    ਜੇ (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    ਜੇ (ਖਾਨਾ != MAXCOL && --ਖਾਨਾ < 0)
	ਖਾਨਾ = 0;
    curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    ਜੇ (has_mbyte)
	mb_adjust_cursor();

    curwin->w_ਸੈੱਟ_curswant = ਸੈੱਟ_curswant;
    rettv->vval.v_ਸੰਖਿਆ = 0;
}

/*
 * "cursor(lnum, col)" function, or
 * "cursor(list)"
 *
 * Moves the cursor to the specified line and column.
 * Returns 0 when the position could be set, -1 otherwise.
 */
    ਸਥਿਰ ਖਾਲੀ
f_cursor(typval_T *argvars, typval_T *rettv)
{
    ਸੈੱਟ_cursorpos(argvars, rettv, FALSE);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
/*
 * "debugbreak()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		pid;

    rettv->vval.v_ਸੰਖਿਆ = FAIL;
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    pid = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]);
    ਜੇ (pid == 0)
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਵਾਪਸ;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    ਜੇ (hProcess == ਨਿਰਾ)
	ਵਾਪਸ;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_ਸੰਖਿਆ = OK;
}
#ਜੇ_ਅੰਤ

/*
 * "deepcopy()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    ਜੇ (check_for_opt_bool_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], ਨਿਰਾ);

    ਵਸਤੂ_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_did_filetype(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    rettv->vval.v_ਸੰਖਿਆ = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_echoraw(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    char_u *str;

    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    str = tv_get_string_chk(&argvars[0]);
    ਜੇ (str != ਨਿਰਾ && *str != NUL)
    {
	ਬਾਹਰਲਾ_str(str);
	ਬਾਹਰਲਾ_flush();
    }
}

/*
 * "empty({expr})" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_ਖਾਲੀ_ਥਾਵਾਂ(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		n = FALSE;

    ਚੋਣ (argvars[0].v_type)
    {
	ਹਾਲਤ VAR_STRING:
	ਹਾਲਤ VAR_FUNC:
	    n = argvars[0].vval.v_string == ਨਿਰਾ
					  || *argvars[0].vval.v_string == NUL;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	    n = argvars[0].vval.v_ਸੰਖਿਆ == 0;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    n = argvars[0].vval.v_list == ਨਿਰਾ
					|| argvars[0].vval.v_list->lv_len == 0;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    n = argvars[0].vval.v_dict == ਨਿਰਾ
			|| argvars[0].vval.v_dict->dv_hashtab.ht_ਵਰਤਿਆ == 0;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	    n = argvars[0].vval.v_ਸੰਖਿਆ != VVAL_TRUE;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_CLASS:
	    n = argvars[0].vval.v_class != ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_OBJECT:
	    n = object_ਖਾਲੀ_ਥਾਵਾਂ(argvars[0].vval.v_object);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_BLOB:
	    n = argvars[0].vval.v_blob == ਨਿਰਾ
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_ਕੰਮ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    n = argvars[0].vval.v_ਕੰਮ == ਨਿਰਾ
			   || argvars[0].vval.v_ਕੰਮ->jv_ਸਥਿਤੀ != ਕੰਮ_STARTED;
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_CHANNEL:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    n = argvars[0].vval.v_channel == ਨਿਰਾ
			       || !channel_is_open(argvars[0].vval.v_channel);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == ਨਿਰਾ
		|| argvars[0].vval.v_typealias->ta_name == ਨਿਰਾ
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	ਹਾਲਤ VAR_INSTR:
	    ਅੰਦਰੂਨੀ_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    ਰੋਕੋ;
    }

    rettv->vval.v_ਸੰਖਿਆ = n;
}

/*
 * "environ()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_environ(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
#ਜੇ !defined(AMIGA)
    ਪੂਰਨ			i = 0;
    char_u		*ਇੰਦਰਾਜ, *ਮੁੱਲ;
# ਜੇ defined (MSWIN)
#  ਜੇ !defined(_UCRT)
    ਬਾਹਰੀ wchar_t	**_wenviron;
#  endif
# ਨਹੀਂਤਾਂ
    ਬਾਹਰੀ ਅੱਖਰ		**environ;
# endif

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;

# ifdef MSWIN
    ਜੇ (*_wenviron == ਨਿਰਾ)
	ਵਾਪਸ;
# ਨਹੀਂਤਾਂ
    ਜੇ (*environ == ਨਿਰਾ)
	ਵਾਪਸ;
# endif

    ਲਈ (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	ਜੇ ((p = (short_u *)_wenviron[i]) == ਨਿਰਾ)
	    ਵਾਪਸ;
	ਇੰਦਰਾਜ = utf16_ਮੰਜ਼ਿਲ_enc(p, ਨਿਰਾ);
# ਨਹੀਂਤਾਂ
	ਜੇ ((ਇੰਦਰਾਜ = (char_u *)environ[i]) == ਨਿਰਾ)
	    ਵਾਪਸ;
	ਇੰਦਰਾਜ = vim_strsave(ਇੰਦਰਾਜ);
# endif
	ਜੇ (ਇੰਦਰਾਜ == ਨਿਰਾ) // out of memory
	    ਵਾਪਸ;
	ਜੇ ((ਮੁੱਲ = vim_strchr(ਇੰਦਰਾਜ, '=')) == ਨਿਰਾ)
	{
	    vim_free(ਇੰਦਰਾਜ);
	    ਜਾਰੀ;
	}
	*ਮੁੱਲ++ = NUL;
	dict_ਜੋੜੋ_string(rettv->vval.v_dict, (ਅੱਖਰ *)ਇੰਦਰਾਜ, ਮੁੱਲ);
	vim_free(ਇੰਦਰਾਜ);
    }
#ਜੇ_ਅੰਤ
}

/*
 * "err_teapot()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_err_teapot(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ਜੇ (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    ਜੇ (*skipwhite(s) == NUL)
		ਵਾਪਸ;
	}

	ਪੂਰਨ err = FALSE;
	ਪੂਰਨ do_503 = eval_ਵਿਅੰਜਕ_ਮੰਜ਼ਿਲ_bool(&argvars[0], &err);
	ਜੇ (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_ਉਪਲਬਧ));
	    ਵਾਪਸ;
	}
    }

    emsg(_(e_im_a_teapot));
}

/*
 * "escape({string}, {chars})" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_escape(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    rettv->vval.v_string = vim_strsave_escaped(tv_get_string(&argvars[0]),
					 tv_get_string_buf(&argvars[1], buf));
    rettv->v_type = VAR_STRING;
}

/*
 * "eval()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*s, *p;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    s = tv_get_string_chk(&argvars[0]);
    ਜੇ (s != ਨਿਰਾ)
	s = skipwhite(s);

    p = s;
    ਜੇ (s == ਨਿਰਾ || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	ਜੇ (p != ਨਿਰਾ && !aborting())
	    semsg(_(e_invalid_ਵਿਅੰਜਕ_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_ਸੰਖਿਆ;
	rettv->vval.v_ਸੰਖਿਆ = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_eventhandler(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = vgetc_busy || ਨਿਵੇਸ਼_busy;
}

ਸਥਿਰ garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    ਖਾਲੀ
execute_redir_str(char_u *ਮੁੱਲ, ਪੂਰਨ ਮੁੱਲ_len)
{
    ਪੂਰਨ		len;

    ਜੇ (ਮੁੱਲ_len == -1)
	len = (ਪੂਰਨ)STRLEN(ਮੁੱਲ);	// Append the entire string
    ਨਹੀਂਤਾਂ
	len = ਮੁੱਲ_len;		// Append only "value_len" characters
    ਜੇ (ga_grow(&redir_execute_ga, len) == FAIL)
	ਵਾਪਸ;

    mch_memmove((ਅੱਖਰ *)redir_execute_ga.ga_ਅੰਕੜੇ
	    + redir_execute_ga.ga_len, ਮੁੱਲ, len);
    redir_execute_ga.ga_len += len;
}

#ਜੇ defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    ਸਥਿਰ char_u *
get_str_ਸਤਰ(
    ਪੂਰਨ	    c ਅਣਵਰਤਿਆ,
    ਖਾਲੀ    *cookie,
    ਪੂਰਨ	    indent ਅਣਵਰਤਿਆ,
    getline_opt_T options ਅਣਵਰਤਿਆ)
{
    char_u	*ਸ਼ੁਰੂ = *(char_u **)cookie;
    char_u	*ਸਤਰ;
    char_u	*p;

    p = ਸ਼ੁਰੂ;
    ਜੇ (p == ਨਿਰਾ || *p == NUL)
	ਵਾਪਸ ਨਿਰਾ;
    p = vim_strchr(p, '\n');
    ਜੇ (p == ਨਿਰਾ)
	ਸਤਰ = vim_strsave(ਸ਼ੁਰੂ);
    ਨਹੀਂਤਾਂ
    {
	ਸਤਰ = vim_strnsave(ਸ਼ੁਰੂ, p - ਸ਼ੁਰੂ);
	p++;
    }

    *(char_u **)cookie = p;
    ਵਾਪਸ ਸਤਰ;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    ਖਾਲੀ
execute_cmds_from_string(char_u *str)
{
    do_cmdline(ਨਿਰਾ, get_str_ਸਤਰ, (ਖਾਲੀ *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#ਜੇ_ਅੰਤ

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_ਸਤਰ(
    ਪੂਰਨ	    c ਅਣਵਰਤਿਆ,
    ਖਾਲੀ    *cookie,
    ਪੂਰਨ	    indent ਅਣਵਰਤਿਆ,
    getline_opt_T options ਅਣਵਰਤਿਆ)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *ਵਸਤੂ = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    s = tv_get_string_buf_chk(&ਵਸਤੂ->li_tv, buf);
    *p = ਵਸਤੂ->li_ਅਗਲਾ;
    ਵਾਪਸ s == ਨਿਰਾ ? ਨਿਰਾ : vim_strsave(s);
}

/*
 * "execute()" function
 */
    ਖਾਲੀ
execute_common(typval_T *argvars, typval_T *rettv, ਪੂਰਨ arg_off)
{
    char_u	*cmd = ਨਿਰਾ;
    list_T	*list = ਨਿਰਾ;
    ਪੂਰਨ		save_ਸੁਨੇਹਾ_silent = ਸੁਨੇਹਾ_silent;
    ਪੂਰਨ		save_emsg_silent = emsg_silent;
    ਪੂਰਨ		save_emsg_noredir = emsg_noredir;
    ਪੂਰਨ		save_redir_execute = redir_execute;
    ਪੂਰਨ		save_redir_off = redir_off;
    garray_T	save_ga;
    ਪੂਰਨ		save_ਸੁਨੇਹਾ_ਖਾਨਾ = ਸੁਨੇਹਾ_ਖਾਨਾ;
    ਪੂਰਨ		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    ਪੂਰਨ		echo_output = FALSE;

    rettv->vval.v_string = ਨਿਰਾ;
    rettv->v_type = VAR_STRING;

    ਜੇ (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	ਜੇ (list == ਨਿਰਾ || list->lv_len == 0)
	    // empty list, no commands, empty output
	    ਵਾਪਸ;
	++list->lv_refcount;
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[arg_off].v_type == VAR_ਕੰਮ
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_ਮੁੱਲ_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	ਜੇ (cmd == ਨਿਰਾ)
	    ਵਾਪਸ;
    }

    ਜੇ (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      ਵਿੱਚ_vim9script());

	ਜੇ (s == ਨਿਰਾ)
	    ਵਾਪਸ;
	ਜੇ (*s == NUL)
	    echo_output = TRUE;
	ਜੇ (STRNCMP(s, "silent", 6) == 0)
	    ++ਸੁਨੇਹਾ_silent;
	ਜੇ (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    ਨਹੀਂਤਾਂ
	++ਸੁਨੇਹਾ_silent;

    ਜੇ (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, ਆਕਾਰ(ਅੱਖਰ), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    ਜੇ (!echo_output)
	ਸੁਨੇਹਾ_ਖਾਨਾ = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    ਜੇ (cmd != ਨਿਰਾ)
	do_cmdline_cmd(cmd);
    ਨਹੀਂਤਾਂ
    {
	listitem_T	*ਵਸਤੂ;

	CHECK_LIST_MATERIALIZE(list);
	ਵਸਤੂ = list->lv_ਪਹਿਲਾ;
	do_cmdline(ਨਿਰਾ, get_list_ਸਤਰ, (ਖਾਲੀ *)&ਵਸਤੂ,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    ਜੇ (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((ਅੱਖਰ *)redir_execute_ga.ga_ਅੰਕੜੇ)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_ਅੰਕੜੇ;
    }
    ਨਹੀਂਤਾਂ
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = ਨਿਰਾ;
    }
    ਸੁਨੇਹਾ_silent = save_ਸੁਨੇਹਾ_silent;
    emsg_silent = save_emsg_silent;
    emsg_noredir = save_emsg_noredir;

    redir_execute = save_redir_execute;
    ਜੇ (redir_execute)
	redir_execute_ga = save_ga;
    redir_off = save_redir_off;

    // "silent reg" or "silent echo x" leaves msg_col somewhere in the line.
    ਜੇ (echo_output)
	// When not working silently: put it in column zero.  A following
	// "echon" will overwrite the message, unavoidably.
	ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
    ਨਹੀਂਤਾਂ
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	ਸੁਨੇਹਾ_ਖਾਨਾ = save_ਸੁਨੇਹਾ_ਖਾਨਾ;
}

/*
 * "execute()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_execute(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    ਖਾਲੀ
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ਪੂਰਨ		n = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    p = tv_get_string(&argvars[0]);
    ਜੇ (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	ਜੇ (mch_getenv(p + 1) != ਨਿਰਾ)
	    n = TRUE;
	ਨਹੀਂਤਾਂ
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    ਜੇ (p != ਨਿਰਾ && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, ਨਿਰਾ, TRUE) == OK);
	ਜੇ (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    ਨਹੀਂਤਾਂ ਜੇ (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    ਨਹੀਂਤਾਂ ਜੇ (*p == '?')			// internal function only
    {
	n = has_ਅੰਦਰੂਨੀ_func_name(p + 1);
    }
    ਨਹੀਂਤਾਂ ਜੇ (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    ਨਹੀਂਤਾਂ ਜੇ (*p == '#')
    {
	ਜੇ (p[1] == '#')
	    n = autocmd_supported(p + 2);
	ਨਹੀਂਤਾਂ
	    n = au_exists(p + 1);
    }
    ਨਹੀਂਤਾਂ				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_ਸੰਖਿਆ = n;
}

    ਸਥਿਰ ਖਾਲੀ
f_exists_compiled(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    emsg(_(e_exists_compiled_can_only_be_ਵਰਤਿਆ_ਵਿੱਚ_def_function));
}

/*
 * "expand()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    size_t	len;
    ਪੂਰਨ		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    ਪੂਰਨ		error = FALSE;
    char_u	*ਨਤੀਜਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    char_u	*p_csl_save = p_csl;
#ਜੇ_ਅੰਤ

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    // avoid using 'completeslash' here
    p_csl = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਅੰਤ

    rettv->v_type = VAR_STRING;
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_ਸੈੱਟ(rettv, ਨਿਰਾ);

    s = tv_get_string(&argvars[0]);
    ਜੇ (*s == '%' || *s == '#' || *s == '<')
    {
	ਅੱਖਰ	*errormsg = ਨਿਰਾ;

	ਜੇ (p_verbose == 0)
	    ++emsg_off;
	ਨਤੀਜਾ = eval_vars(s, s, &len, ਨਿਰਾ, &errormsg, ਨਿਰਾ, FALSE);
	ਜੇ (p_verbose == 0)
	    --emsg_off;
	ਨਹੀਂਤਾਂ ਜੇ (errormsg != ਨਿਰਾ)
	    emsg(errormsg);
	ਜੇ (rettv->v_type == VAR_LIST)
	{
	    ਜੇ (rettv_list_alloc(rettv) == OK && ਨਤੀਜਾ != ਨਿਰਾ)
		list_append_string(rettv->vval.v_list, ਨਤੀਜਾ, -1);
	    vim_free(ਨਤੀਜਾ);
	}
	ਨਹੀਂਤਾਂ
	    rettv->vval.v_string = ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	// When the optional second argument is non-zero, don't remove matches
	// for 'wildignore' and don't put matches for 'suffixes' at the end.
	ਜੇ (argvars[1].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	ਜੇ (!error)
	{
	    ExpandInit(&xpc);
	    xpc.xp_context = EXPAND_FILES;
	    ਜੇ (p_wic)
		options += WILD_ICASE;
	    ਜੇ (rettv->v_type == VAR_STRING)
		rettv->vval.v_string = ExpandOne(&xpc, s, ਨਿਰਾ,
							   options, WILD_ALL);
	    ਨਹੀਂਤਾਂ ਜੇ (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, ਨਿਰਾ, options, WILD_ALL_KEEP);
		ਲਈ (ਪੂਰਨ i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	ਨਹੀਂਤਾਂ
	    rettv->vval.v_string = ਨਿਰਾ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    p_csl = p_csl_save;
#ਜੇ_ਅੰਤ
}

/*
 * "expandcmd()" function
 * Expand all the special characters in a command string.
 */
    ਸਥਿਰ ਖਾਲੀ
f_expandcmd(typval_T *argvars, typval_T *rettv)
{
    exarg_T	eap;
    char_u	*cmdstr;
    ਅੱਖਰ	*errormsg = ਨਿਰਾ;
    ਪੂਰਨ		emsgoff = TRUE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_ਖੇਤਰ(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = ਨਿਰਾ;
    eap.cmdidx = CMD_USER;

    ਜੇ (emsgoff)
	++emsg_off;
    ਜੇ (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	ਜੇ (!emsgoff && errormsg != ਨਿਰਾ && *errormsg != NUL)
	    emsg(errormsg);
    ਜੇ (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_feedkeys(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		remap = TRUE;
    ਪੂਰਨ		ਪਾਓ = FALSE;
    char_u	*ਕੁੰਜੀਆਂ, *flags;
    char_u	nbuf[NUMBUFLEN];
    ਪੂਰਨ		typed = FALSE;
    ਪੂਰਨ		execute = FALSE;
    ਪੂਰਨ		context = FALSE;
    ਪੂਰਨ		dangerous = FALSE;
    ਪੂਰਨ		lowlevel = FALSE;

    // This is not allowed in the sandbox.  If the commands would still be
    // executed in the sandbox it would be OK, but it probably happens later,
    // when "sandbox" is no longer set.
    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਕੁੰਜੀਆਂ = tv_get_string(&argvars[0]);

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	ਲਈ ( ; *flags != NUL; ++flags)
	{
	    ਚੋਣ (*flags)
	    {
		ਹਾਲਤ 'n': remap = FALSE; ਰੋਕੋ;
		ਹਾਲਤ 'm': remap = TRUE; ਰੋਕੋ;
		ਹਾਲਤ 't': typed = TRUE; ਰੋਕੋ;
		ਹਾਲਤ 'i': ਪਾਓ = TRUE; ਰੋਕੋ;
		ਹਾਲਤ 'x': execute = TRUE; ਰੋਕੋ;
		ਹਾਲਤ 'c': context = TRUE; ਰੋਕੋ;
		ਹਾਲਤ '!': dangerous = TRUE; ਰੋਕੋ;
		ਹਾਲਤ 'L': lowlevel = TRUE; ਰੋਕੋ;
	    }
	}
    }

    ਜੇ (*ਕੁੰਜੀਆਂ != NUL || execute)
    {
	ਜੇ (lowlevel
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VTP
		&& (!is_ਪਦ_win32()
		    || (ਕੁੰਜੀਆਂ[0] == 3 && ctrl_c_interrupts && typed))
#ਜੇ_ਅੰਤ
	   )
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਨਿਵੇਸ਼_BUF
	    ch_log(ਨਿਰਾ, "feedkeys() lowlevel: %s", ਕੁੰਜੀਆਂ);

	    ਪੂਰਨ len = (ਪੂਰਨ)STRLEN(ਕੁੰਜੀਆਂ);
	    ਲਈ (ਪੂਰਨ idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		ਜੇ (ਕੁੰਜੀਆਂ[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(ਕੁੰਜੀਆਂ + idx, 1);
	    }
#ਨਹੀਂਤਾਂ
	    emsg(_(e_lowlevel_ਨਿਵੇਸ਼_not_supported));
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *ਕੁੰਜੀਆਂ_esc = vim_strsave_escape_csi(ਕੁੰਜੀਆਂ);
	    ਜੇ (ਕੁੰਜੀਆਂ_esc == ਨਿਰਾ)
		ਵਾਪਸ;

	    ch_log(ਨਿਰਾ, "feedkeys(%s): %s", typed ? "typed" : "", ਕੁੰਜੀਆਂ);

	    ins_typebuf(ਕੁੰਜੀਆਂ_esc, (remap ? REMAP_YES : REMAP_NONE),
				   ਪਾਓ ? 0 : typebuf.tb_len, !typed, FALSE);
	    ਜੇ (vgetc_busy
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
		    || timer_busy
#ਜੇ_ਅੰਤ
		    || ਨਿਵੇਸ਼_busy)
		typebuf_was_filled = TRUE;

	    vim_free(ਕੁੰਜੀਆਂ_esc);
	}

	ਜੇ (execute)
	{
	    ਪੂਰਨ		save_ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    ਸੁਨੇਹਾ_scroll = FALSE;

	    ch_log(ਨਿਰਾ, "feedkeys() executing");

	    ਜੇ (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_ਸੰਸਕਰਣ = 0;
	    }

	    ਜੇ (!dangerous)
	    {
		++ex_ਸਾਧਾਰਨ_busy;
		++ਵਿੱਚ_feedkeys;
	    }
	    ++allow_osc_ਕੁੰਜੀ;
	    exec_ਸਾਧਾਰਨ(TRUE, lowlevel, TRUE);
	    --allow_osc_ਕੁੰਜੀ;
	    ਜੇ (!dangerous)
	    {
		--ex_ਸਾਧਾਰਨ_busy;
		--ਵਿੱਚ_feedkeys;
	    }

	    ਸੁਨੇਹਾ_scroll |= save_ਸੁਨੇਹਾ_scroll;

	    ਜੇ (context)
		current_sctx = save_sctx;
	}
    }
}

/*
 * "fnameescape({string})" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_fnameescape(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_foreground(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	gui_mch_ਸੈੱਟ_foreground();
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
#ਜੇ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_ਸੈੱਟ_foreground();
#ਜੇ_ਅੰਤ
}

/*
 * "function()" function
 * "funcref()" function
 */
    ਸਥਿਰ ਖਾਲੀ
common_function(typval_T *argvars, typval_T *rettv, ਪੂਰਨ is_funcref)
{
    char_u	*s;
    char_u	*name;
    ਪੂਰਨ		use_string = FALSE;
    partial_T   *arg_pt = ਨਿਰਾ;
    char_u	*trans_name = ਨਿਰਾ;
    ਪੂਰਨ		is_global = FALSE;
    char_u	*ਸ਼ੁਰੂ_bracket = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != ਨਿਰਾ)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    ਨਹੀਂਤਾਂ
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    ਜੇ (s == ਨਿਰਾ)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), "NULL");
	ਵਾਪਸ;
    }

    ਜੇ ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == ਨਿਰਾ) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, ਨਿਰਾ);
	ਜੇ (*name == '<')
	{
	    // generic function
	    ਸ਼ੁਰੂ_bracket = name;
	    ਜੇ (skip_generic_func_type_ਦਲੀਲਾਂ(&name) == FAIL)
		ਜਾਓ theend;
	}
	ਜੇ (*name != NUL)
	    s = ਨਿਰਾ;
    }

    ਜੇ (s == ਨਿਰਾ || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == ਨਿਰਾ))
	semsg(_(e_invalid_ਦਲੀਲ_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    ਨਹੀਂਤਾਂ ਜੇ (trans_name != ਨਿਰਾ && (is_funcref
			 ? ਲੱਭੋ_func(trans_name, is_global) == ਨਿਰਾ
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	dict_idx = 0;
	ਪੂਰਨ	arg_idx = 0;
	list_T	*list = ਨਿਰਾ;

	ਜੇ (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	ਨਹੀਂਤਾਂ ਜੇ (trans_name != ਨਿਰਾ && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	ਨਹੀਂਤਾਂ
	    name = vim_strsave(s);

	ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    ਨਹੀਂਤਾਂ
		// function(name, [args])
		arg_idx = 1;
	    ਜੇ (dict_idx > 0)
	    {
		ਜੇ (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    ਜਾਓ theend;
		}
		ਜੇ (argvars[dict_idx].vval.v_dict == ਨਿਰਾ)
		    dict_idx = 0;
	    }
	    ਜੇ (arg_idx > 0)
	    {
		ਜੇ (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_ਦੂਜਾ_ਦਲੀਲ_ਦਾ_function_must_be_list_or_dict));
		    vim_free(name);
		    ਜਾਓ theend;
		}
		list = argvars[arg_idx].vval.v_list;
		ਜੇ (list == ਨਿਰਾ || list->lv_len == 0)
		    arg_idx = 0;
		ਨਹੀਂਤਾਂ ਜੇ (list->lv_len > MAX_FUNC_ਦਲੀਲਾਂ)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    ਜਾਓ theend;
		}
	    }
	}
	ਜੇ (dict_idx > 0 || arg_idx > 0 || arg_pt != ਨਿਰਾ || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    ਜੇ (pt == ਨਿਰਾ)
		vim_free(name);
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (arg_idx > 0 || (arg_pt != ਨਿਰਾ && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    ਪੂਰਨ		i = 0;
		    ਪੂਰਨ		arg_len = 0;
		    ਪੂਰਨ		lv_len = 0;

		    ਜੇ (arg_pt != ਨਿਰਾ)
			arg_len = arg_pt->pt_argc;
		    ਜੇ (list != ਨਿਰਾ)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    ਜੇ (pt->pt_argv == ਨਿਰਾ)
		    {
			vim_free(pt);
			vim_free(name);
			ਜਾਓ theend;
		    }
		    ਲਈ (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    ਜੇ (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_ਚੀਜ਼ਾਂ(list, li)
			    copy_tv(&li->li_tv, &pt->pt_argv[i++]);
		    }
		}

		// For "function(dict.func, [], dict)" and "func" is a partial
		// use "dict".  That is backwards compatible.
		ਜੇ (dict_idx > 0)
		{
		    // The dict is bound explicitly, pt_auto is FALSE.
		    pt->pt_dict = argvars[dict_idx].vval.v_dict;
		    ++pt->pt_dict->dv_refcount;
		}
		ਨਹੀਂਤਾਂ ਜੇ (arg_pt != ਨਿਰਾ)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    ਜੇ (pt->pt_dict != ਨਿਰਾ)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    ਜੇ (pt->pt_obj != ਨਿਰਾ)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		ਜੇ (arg_pt != ਨਿਰਾ && arg_pt->pt_func != ਨਿਰਾ)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		ਨਹੀਂਤਾਂ ਜੇ (is_funcref)
		{
		    pt->pt_func = ਲੱਭੋ_func(trans_name, is_global);
		    ਜੇ (IS_GENERIC_FUNC(pt->pt_func) && ਸ਼ੁਰੂ_bracket != ਨਿਰਾ)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&ਸ਼ੁਰੂ_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		ਨਹੀਂਤਾਂ
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		ਜੇ (arg_pt != ਨਿਰਾ)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	ਨਹੀਂਤਾਂ
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    ਜੇ (ਸ਼ੁਰੂ_bracket == ਨਿਰਾ)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// generic function
		size_t len = STRLEN(name) + STRLEN(ਸ਼ੁਰੂ_bracket);
		rettv->vval.v_string = alloc(len + 1);
		ਜੇ (rettv->vval.v_string != ਨਿਰਾ)
		{
		    STRCPY(rettv->vval.v_string, name);
		    STRCAT(rettv->vval.v_string, ਸ਼ੁਰੂ_bracket);
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
    ਸਥਿਰ ਖਾਲੀ
f_funcref(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, TRUE);
}

/*
 * "function()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_function(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, FALSE);
}

/*
 * "garbagecollect()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_garbagecollect(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
	garbage_collect_at_exit = TRUE;
}

/*
 * "get()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_get(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    list_T	*l;
    tuple_T	*tuple;
    dictitem_T	*di;
    dict_T	*d;
    typval_T	*tv = ਨਿਰਾ;
    ਪੂਰਨ		what_is_dict = FALSE;

    ਜੇ (argvars[0].v_type == VAR_BLOB)
    {
	ਪੂਰਨ error = FALSE;
	ਪੂਰਨ idx = tv_get_ਸੰਖਿਆ_chk(&argvars[1], &error);

	ਜੇ (!error)
	{
	    rettv->v_type = VAR_ਸੰਖਿਆ;
	    ਜੇ (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    ਜੇ (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_ਸੰਖਿਆ = -1;
	    ਨਹੀਂਤਾਂ
	    {
		rettv->vval.v_ਸੰਖਿਆ = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_LIST)
    {
	ਜੇ ((l = argvars[0].vval.v_list) != ਨਿਰਾ)
	{
	    ਪੂਰਨ		error = FALSE;

	    li = list_ਲੱਭੋ(l, (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[1], &error));
	    ਜੇ (!error && li != ਨਿਰਾ)
		tv = &li->li_tv;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_TUPLE)
    {
	ਜੇ ((tuple = argvars[0].vval.v_tuple) != ਨਿਰਾ)
	{
	    ਪੂਰਨ		error = FALSE;
	    ਲੰਮਾ	idx;

	    idx = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[1], &error);
	    ਜੇ (!error)
		tv = tuple_ਲੱਭੋ(tuple, idx);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_DICT)
    {
	ਜੇ ((d = argvars[0].vval.v_dict) != ਨਿਰਾ)
	{
	    di = dict_ਲੱਭੋ(d, tv_get_string(&argvars[1]), -1);
	    ਜੇ (di != ਨਿਰਾ)
		tv = &di->di_tv;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	ਜੇ (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	ਨਹੀਂਤਾਂ
	{
	    CLEAR_ਖੇਤਰ(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	ਜੇ (pt != ਨਿਰਾ)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    ਜੇ (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		ਜੇ (name == ਨਿਰਾ)
		    rettv->vval.v_string = ਨਿਰਾ;
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    ਜੇ (*what == 'n' && pt->pt_name == ਨਿਰਾ
							&& pt->pt_func != ਨਿਰਾ)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		ਜੇ (pt->pt_dict != ਨਿਰਾ)
		    rettv_dict_ਸੈੱਟ(rettv, pt->pt_dict);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		ਜੇ (rettv_list_alloc(rettv) == OK)
		{
		    ਪੂਰਨ i;

		    ਲਈ (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(what, "arity") == 0)
	    {
		ਪੂਰਨ required = 0, optional = 0, varargs = FALSE;
		char_u *name = partial_name(pt);

		get_func_arity(name, &required, &optional, &varargs);

		rettv->v_type = VAR_DICT;
		ਜੇ (rettv_dict_alloc(rettv) == OK)
		{
		    dict_T *dict = rettv->vval.v_dict;

		    // Take into account the arguments of the partial, if any.
		    // Note that it is possible to supply more arguments than the function
		    // accepts.
		    ਜੇ (pt->pt_argc >= required + optional)
			required = optional = 0;
		    ਨਹੀਂਤਾਂ ਜੇ (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    ਨਹੀਂਤਾਂ
			required -= pt->pt_argc;

		    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "required", required);
		    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "optional", optional);
		    dict_ਜੋੜੋ_bool(dict, "varargs", varargs);
		}
	    }
	    ਨਹੀਂਤਾਂ
		semsg(_(e_invalid_ਦਲੀਲ_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    ਜੇ (!what_is_dict)
		ਵਾਪਸ;
	}
    }
    ਨਹੀਂਤਾਂ
	semsg(_(e_ਦਲੀਲ_ਦਾ_str_must_be_list_tuple_dictionary_or_blob), "get()");

    ਜੇ (tv == ਨਿਰਾ)
    {
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    ਨਹੀਂਤਾਂ
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
    ਸਥਿਰ ਖਾਲੀ
f_getbgcolor(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	ਜੇ (gui.ਵਿੱਚ_use)
	    try_hl = true;
# endif
# ifdef FEAT_TERMGUICOLORS
	ਜੇ (p_tgc)
	    try_hl = true;
# endif
	ਜੇ (try_hl)
	{
	    guicolor_T	fg, bg;
	    ਪੂਰਨ		hl_id = syn_name2id((char_u *)"Normal");

	    ਜੇ (hl_id > 0)
	    {
		(ਖਾਲੀ)syn_id2colors(hl_id, &fg, &bg);
		ਜੇ (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_ਸੰਖਿਆ(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_ਸੰਖਿਆ(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_ਸੰਖਿਆ(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    ਵਾਪਸ;
		}
	    }
	}
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	ਜੇ (ਪਦ_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)r);
	    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)g);
	    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#ਜੇ_ਅੰਤ
}

/*
 * "getcellpixels()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getcellpixels(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

#ਜੇ defined(FEAT_GUI)
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	// success pixel size and no gui.
	list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)gui.char_ਚੌੜਾਈ);
	list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)gui.char_ਉਚਾਈ);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
#ਜੇ defined(UNIX) || defined(MSWIN)
	list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)cell_ਚੌੜਾਈ);
	list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)cell_ਉਚਾਈ);
#ਨਹੀਂਤਾਂ
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)-1);
	list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)-1);
#ਜੇ_ਅੰਤ
    }

}

/*
 * "getchangelist()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ਪੂਰਨ		i;
    list_T	*l;
    dict_T	*d;
    ਪੂਰਨ		changelistindex;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    ਨਹੀਂਤਾਂ
	buf = tv_get_buf_from_arg(&argvars[0]);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    l = list_alloc();
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	ਵਾਪਸ;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    ਜੇ (buf == curwin->w_ਬਫਰ)
    {
	changelistindex = curwin->w_changelistidx;
    }
    ਨਹੀਂਤਾਂ
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    ਜੇ (wip->wi_win == curwin)
		ਰੋਕੋ;
	changelistindex = wip != ਨਿਰਾ ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)changelistindex);

    ਲਈ (i = 0; i < buf->b_changelistlen; ++i)
    {
	ਜੇ (buf->b_changelist[i].lnum == 0)
	    ਜਾਰੀ;
	ਜੇ ((d = dict_alloc()) == ਨਿਰਾ)
	    ਵਾਪਸ;
	ਜੇ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ਵਾਪਸ;
	}
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "lnum", (ਲੰਮਾ)buf->b_changelist[i].lnum);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "col", (ਲੰਮਾ)buf->b_changelist[i].ਖਾਨਾ);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "coladd", (ਲੰਮਾ)buf->b_changelist[i].coladd);
    }
}

    ਸਥਿਰ ਖਾਲੀ
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    ਪੂਰਨ		getcurpos,
    ਪੂਰਨ		charcol)
{
    pos_T	*fp = ਨਿਰਾ;
    pos_T	pos;
    win_T	*wp = curwin;
    list_T	*l;
    ਪੂਰਨ		fnum = -1;

    ਜੇ (rettv_list_alloc(rettv) == OK)
    {
	l = rettv->vval.v_list;
	ਜੇ (getcurpos)
	{
	    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
	    {
		wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
		ਜੇ (wp != ਨਿਰਾ)
		    fp = &wp->w_cursor;
	    }
	    ਨਹੀਂਤਾਂ
		fp = &curwin->w_cursor;
	    ਜੇ (fp != ਨਿਰਾ && charcol)
	    {
		pos = *fp;
		pos.ਖਾਨਾ =
		    buf_byteidx_ਮੰਜ਼ਿਲ_charidx(wp->w_ਬਫਰ, pos.lnum, pos.ਖਾਨਾ);
		fp = &pos;
	    }
	}
	ਨਹੀਂਤਾਂ
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	ਜੇ (fnum != -1)
	    list_append_ਸੰਖਿਆ(l, (varnumber_T)fnum);
	ਨਹੀਂਤਾਂ
	    list_append_ਸੰਖਿਆ(l, (varnumber_T)0);
	list_append_ਸੰਖਿਆ(l, (fp != ਨਿਰਾ) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_ਸੰਖਿਆ(l, (fp != ਨਿਰਾ)
		     ? (varnumber_T)(fp->ਖਾਨਾ == MAXCOL ? MAXCOL : fp->ਖਾਨਾ + 1)
							    : (varnumber_T)0);
	list_append_ਸੰਖਿਆ(l, (fp != ਨਿਰਾ) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	ਜੇ (getcurpos)
	{
	    ਪੂਰਨ	    save_ਸੈੱਟ_curswant = curwin->w_ਸੈੱਟ_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    ਜੇ (wp == curwin)
		update_curswant();
	    list_append_ਸੰਖਿਆ(l, wp == ਨਿਰਾ ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    ਜੇ (wp == curwin && save_ਸੈੱਟ_curswant)
	    {
		curwin->w_ਸੈੱਟ_curswant = save_ਸੈੱਟ_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = FALSE;
}

/*
 * "getcharpos()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getcharpos(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getcharsearch(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਜੇ (rettv_dict_alloc(rettv) != OK)
	ਵਾਪਸ;

    dict_T *dict = rettv->vval.v_dict;

    dict_ਜੋੜੋ_string(dict, "char", ਆਖਰੀ_csearch());
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "forward", ਆਖਰੀ_csearch_forward());
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "until", ਆਖਰੀ_csearch_until());
}

/*
 * "getenv()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getenv(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ	    mustfree = FALSE;
    char_u  *p;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    ਜੇ (p == ਨਿਰਾ)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_ਸੰਖਿਆ = VVAL_NULL;
	ਵਾਪਸ;
    }
    ਜੇ (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getfontname(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	GuiFont font;
	char_u	*name = ਨਿਰਾ;

	ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	ਨਹੀਂਤਾਂ
	{
	    name = tv_get_string(&argvars[0]);
	    ਜੇ (STRCMP(name, "*") == 0)	    // don't use font dialog
		ਵਾਪਸ;
	    font = gui_mch_get_font(name, FALSE);
	    ਜੇ (font == NOFONT)
		ਵਾਪਸ;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#ਜੇ_ਅੰਤ
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
ਸਥਿਰ ਬਣਤਰ
{
    argcheck_T	func;
    ਅੱਖਰ	*types;	    // separated by commas
} argcheck_types[] =
{
    {arg_any,			"any"},
    {arg_blob,			"blob"},
    {arg_bool,			"bool,number"},
    {arg_bool_or_dict_any,	"bool,number,dict<any>"},
    {arg_bool_or_nr,		"bool,number"},
    {arg_ਬਫਰ,		"string,number"},
    {arg_ਬਫਰ_or_dict_any,	"string,number,dict<any>"},
    {arg_chan_or_ਕੰਮ,		"channel,job"},
    {arg_cursor1,		"number,string,list<any>"},
    {arg_dict_any,		"dict<any>"},
    {arg_dict_any_or_string,	"dict<any>,string"},
    {arg_extend3,		"number,string"},
    {arg_filter_func,		"string,func"},
    {arg_float_or_nr,		"float,number"},
    {arg_foreach_func,		"string,func"},
    {arg_get1,			"blob,list<any>,tuple<any>,dict<any>,func"},
    {arg_ਵਸਤੂ_ਦਾ_prev,		"any"},
    {arg_ਕੰਮ,			"job"},
    {arg_len1,
	    "string,number,blob,list<any>,tuple<any>,dict<any>,object<any>"},
    {arg_list_any,		"list<any>"},
    {arg_list_any_mod,		"list<any>"},
    {arg_list_ਸੰਖਿਆ,		"list<number>"},
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
    {arg_ਸੰਖਿਆ,		"number"},
    {arg_object,		"object<any>"},
    {arg_remove2,		"number,string"},
    {arg_repeat1,		"string,number,blob,list<any>,tuple<any>"},
    {arg_ਉਲਟ,		"list<any>,tuple<any>,blob,string"},
    {arg_same_as_prev,		"any"},
    {arg_same_struct_as_prev,	"any"},
    {arg_slice1,		"list<any>,tuple<any>,blob,string"},
    {arg_ਛਾਂਟੋ_how,		"string,func"},
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
    ਸਥਿਰ list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    ਅੱਖਰ	*types = "any";

    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਲਈ (ਪੂਰਨ i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(argcheck_types); ++i)
	ਜੇ (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    ਰੋਕੋ;
	}
    ਲਈ (ਅੱਖਰ *p = types; ; )
    {
	ਅੱਖਰ	*ਸਿਰਾ = (ਅੱਖਰ *)vim_strchr((char_u *)p, ',');
	ਪੂਰਨ	len = ਸਿਰਾ == ਨਿਰਾ ? (ਪੂਰਨ)STRLEN(p) : (ਪੂਰਨ)(ਸਿਰਾ - p);

	ਜੇ (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	ਜੇ (ਸਿਰਾ == ਨਿਰਾ)
	    ਰੋਕੋ;
	p = ਸਿਰਾ + 1;
    }
    ਵਾਪਸ l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    ਸਥਿਰ ਪੂਰਨ
getinfo_argtypes(
	list_T			*l,
	ਅਚਲ funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    ਪੂਰਨ		argcount = 0;

    ਜੇ (l != ਨਿਰਾ)
	FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    ਜੇ (s == ਨਿਰਾ)
		ਵਾਪਸ -1;
	    ਜੇ (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_ਦਲੀਲਾਂ)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		ਵਾਪਸ -1;
	    }
	    type = parse_type(&p, type_gap, ਨਿਰਾ, ਨਿਰਾ, TRUE);
	    ਜੇ (type == ਨਿਰਾ)
		ਵਾਪਸ -1;
	    ਜੇ (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		ਵਾਪਸ -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    ਲਈ ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_ਦਲੀਲਾਂ; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    ਵਾਪਸ argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    ਸਥਿਰ ਅੱਖਰ *
builtin_ret_type_name(
	ਪੂਰਨ	    idx,
	ਪੂਰਨ	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	ਅੱਖਰ	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = ਅੰਦਰੂਨੀ_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    ਵਾਪਸ type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_ਕਿਸਮ_T;

ਸਥਿਰ ਬਣਤਰ
{
    ਅੱਖਰ		*name;
    getinfo_ਕਿਸਮ_T	ਕਿਸਮ;
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
    ਸਥਿਰ ਖਾਲੀ
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_ਕਿਸਮ_T	ਕਿਸਮ;
    ਪੂਰਨ			ki;
    ਪੂਰਨ			idx;
    ਅਚਲ funcentry_T	*fe;
    dict_T		*d;
    list_T		*ਦਲੀਲਾਂ;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_ਦਲੀਲਾਂ];
    list_T		*argtypes_list = ਨਿਰਾ;
    ਪੂਰਨ			argcount;
    ਪੂਰਨ			vim9 = -1;	// -1: the context of the caller
    ਅੱਖਰ		*ret_name;
    ਅੱਖਰ		*tofree;

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    ਜੇ (ਵਿੱਚ_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;
    ਜੇ (check_for_opt_dict_arg(argvars, 2) == FAIL)
	ਵਾਪਸ;

    kindname = tv_get_string(&argvars[0]);
    ਲਈ (ki = 0; ki < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(getinfo_kinds); ++ki)
	ਜੇ (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    ਰੋਕੋ;
    ਜੇ (ki == (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(getinfo_kinds))
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), kindname);
	ਵਾਪਸ;
    }
    ਕਿਸਮ = getinfo_kinds[ki].ਕਿਸਮ;
    name = tv_get_string_buf(&argvars[1], namebuf);

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != ਨਿਰਾ)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	ਪੂਰਨ	    todo = (ਪੂਰਨ)opts->dv_hashtab.ht_ਵਰਤਿਆ;

	FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&opts->dv_hashtab, hi, todo)
	{
	    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		ਜਾਰੀ;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    ਜੇ (STRCMP(di->di_ਕੁੰਜੀ, "argtypes") == 0 && ਕਿਸਮ == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(di->di_ਕੁੰਜੀ, "vim9") == 0 && ਕਿਸਮ == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    ਨਹੀਂਤਾਂ
	    {
		semsg(_(e_invalid_ਦਲੀਲ_str), di->di_ਕੁੰਜੀ);
		ਵਾਪਸ;
	    }
	}
    }
    ਜੇ (ਕਿਸਮ == GI_VIMVAR)
    {
	ਜੇ (STRNCMP(name, "v:", 2) == 0)
	    (ਖਾਲੀ)vim_var_info(name + 2, rettv->vval.v_dict);
	ਵਾਪਸ;
    }
    ਜੇ (ਕਿਸਮ == GI_COMMAND)
    {
	(ਖਾਲੀ)ex_command_info(name, vim9, rettv->vval.v_dict);
	ਵਾਪਸ;
    }
    ਜੇ (ਕਿਸਮ == GI_OPTION)
    {
	(ਖਾਲੀ)option_info(name, rettv->vval.v_dict);
	ਵਾਪਸ;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = ਲੱਭੋ_ਅੰਦਰੂਨੀ_func_opt(name, FALSE);
    ਜੇ (idx < 0)
    {
	(ਖਾਲੀ)user_func_info(name, rettv->vval.v_dict);
	ਵਾਪਸ;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, ਆਕਾਰ(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    ਜੇ (argcount < 0)
    {
	clear_type_list(&type_gap);
	ਵਾਪਸ;
    }
    d = rettv->vval.v_dict;
    dict_ਜੋੜੋ_string(d, "name", (char_u *)fe->f_name);
    dict_ਜੋੜੋ_string(d, "kind", (char_u *)"builtin");
    dict_ਜੋੜੋ_bool(d, "available", fe->f_func != ਨਿਰਾ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "minargs", fe->f_min_argc);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "method", fe->f_argtype & FEARG_ਨਕਾਬ);

    ਦਲੀਲਾਂ = list_alloc();
    ਜੇ (ਦਲੀਲਾਂ == ਨਿਰਾ || dict_ਜੋੜੋ_list(d, "args", ਦਲੀਲਾਂ) == FAIL)
    {
	list_unref(ਦਲੀਲਾਂ);
	clear_type_list(&type_gap);
	ਵਾਪਸ;
    }
    argnames = (char_u *)fe->f_argnames;
    ਲਈ (ਪੂਰਨ i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = ਨਿਰਾ;
	ਪੂਰਨ	    argnamelen = 0;

	ਜੇ (fe->f_argcheck == ਨਿਰਾ)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    ਜੇ (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		ਰੋਕੋ;
	    check = arg_any;
	}
	ਨਹੀਂਤਾਂ
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    ਜੇ ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == ਨਿਰਾ)
		ਰੋਕੋ;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	ਜੇ (argnames != ਨਿਰਾ)
	{
	    char_u  *ਸਿਰਾ = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = ਸਿਰਾ == ਨਿਰਾ ? (ਪੂਰਨ)STRLEN(argnames)
						     : (ਪੂਰਨ)(ਸਿਰਾ - argnames);
	    argnames = ਸਿਰਾ == ਨਿਰਾ ? ਨਿਰਾ : skipwhite(ਸਿਰਾ + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	ਜੇ (arg == ਨਿਰਾ || types == ਨਿਰਾ
		|| dict_ਜੋੜੋ_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    ਵਾਪਸ;
	}
	ਜੇ ((argnamelen > 0
		    && dict_ਜੋੜੋ_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(ਦਲੀਲਾਂ, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    ਵਾਪਸ;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    ਜੇ (argtypes_list == ਨਿਰਾ
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	ਅੱਖਰ	*tofree_max;
	ਅੱਖਰ	*max_name;

	ਲਈ (ਪੂਰਨ i = argcount; i < fe->f_max_argc && i < MAX_FUNC_ਦਲੀਲਾਂ; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	ਜੇ (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = ਨਿਰਾ;
	    ret_name = "any";
	}
	vim_free(tofree_max);
    }
    dict_ਜੋੜੋ_string(d, "returns", (char_u *)ret_name);
    vim_free(tofree);
    clear_type_list(&type_gap);
}

/*
 * "getjumplist()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ਪੂਰਨ		i;
    list_T	*l;
    dict_T	*d;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL)))
	ਵਾਪਸ;

    wp = ਲੱਭੋ_tabwin(&argvars[0], &argvars[1], ਨਿਰਾ);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	ਵਾਪਸ;
    }

    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    ਲਈ (i = 0; i < wp->w_jumplistlen; ++i)
    {
	ਜੇ (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    ਜਾਰੀ;
	ਜੇ ((d = dict_alloc()) == ਨਿਰਾ)
	    ਵਾਪਸ;
	ਜੇ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ਵਾਪਸ;
	}
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "lnum", (ਲੰਮਾ)wp->w_jumplist[i].fmark.mark.lnum);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "col", (ਲੰਮਾ)wp->w_jumplist[i].fmark.mark.ਖਾਨਾ);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "coladd", (ਲੰਮਾ)wp->w_jumplist[i].fmark.mark.coladd);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "bufnr", (ਲੰਮਾ)wp->w_jumplist[i].fmark.fnum);
	ਜੇ (wp->w_jumplist[i].fname != ਨਿਰਾ)
	    dict_ਜੋੜੋ_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getpid(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    ਸਥਿਰ ਖਾਲੀ
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getpos(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    ਸਥਿਰ ਪੂਰਨ
ਖੰਡ_def2str(ਬਣਤਰ ਖੰਡ_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    ਜੇ (ret->string == ਨਿਰਾ)
    {
	ret->ਲੰਮਾਈ = 0;
	ਵਾਪਸ FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->ਲੰਮਾਈ = bd->startspaces;
    mch_memmove(ret->string + ret->ਲੰਮਾਈ, bd->textstart, bd->textlen);
    ret->ਲੰਮਾਈ += bd->textlen;
    vim_memset(ret->string + ret->ਲੰਮਾਈ, ' ', bd->endspaces);
    ret->ਲੰਮਾਈ += bd->endspaces;
    ret->string[ret->ਲੰਮਾਈ] = NUL;

    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
getregionpos(
    typval_T	*argvars,
    typval_T	*rettv,
    pos_T	*p1,
    pos_T	*p2,
    ਪੂਰਨ		*inclusive,
    ਪੂਰਨ		*region_type,
    oparg_T	*oap)
{
    ਪੂਰਨ		fnum1 = -1, fnum2 = -1;
    char_u	*type;
    buf_T	*findbuf;
    char_u	default_type[] = "v";
    ਪੂਰਨ		ਖੰਡ_ਚੌੜਾਈ = 0;
    ਪੂਰਨ		is_select_exclusive;
    ਪੂਰਨ		l;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (list2fpos(&argvars[0], p1, &fnum1, ਨਿਰਾ, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, ਨਿਰਾ, FALSE) != OK
	    || fnum1 != fnum2)
	ਵਾਪਸ FAIL;

    ਜੇ (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	ਜੇ (type == ਨਿਰਾ)
	    type = default_type;
    }
    ਨਹੀਂਤਾਂ
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    ਜੇ (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    ਨਹੀਂਤਾਂ ਜੇ (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    ਨਹੀਂਤਾਂ ਜੇ (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	ਜੇ (*p != NUL && ((ਖੰਡ_ਚੌੜਾਈ = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "type", type);
	    ਵਾਪਸ FAIL;
	}
	*region_type = MBLOCK;
    }
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "type", type);
	ਵਾਪਸ FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    ਜੇ (findbuf == ਨਿਰਾ || findbuf->b_ml.ml_mfp == ਨਿਰਾ)
    {
	emsg(_(e_ਬਫਰ_is_not_loaded));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	semsg(_(e_invalid_ਸਤਰ_ਸੰਖਿਆ_nr), p1->lnum);
	ਵਾਪਸ FAIL;
    }
    ਜੇ (p1->ਖਾਨਾ == MAXCOL)
	p1->ਖਾਨਾ = ml_get_buf_len(findbuf, p1->lnum) + 1;
    ਨਹੀਂਤਾਂ ਜੇ (p1->ਖਾਨਾ < 1 || p1->ਖਾਨਾ > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_ਕਾਲਮ_ਸੰਖਿਆ_nr), p1->ਖਾਨਾ);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	semsg(_(e_invalid_ਸਤਰ_ਸੰਖਿਆ_nr), p2->lnum);
	ਵਾਪਸ FAIL;
    }
    ਜੇ (p2->ਖਾਨਾ == MAXCOL)
	p2->ਖਾਨਾ = ml_get_buf_len(findbuf, p2->lnum) + 1;
    ਨਹੀਂਤਾਂ ਜੇ (p2->ਖਾਨਾ < 1 || p2->ਖਾਨਾ > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_ਕਾਲਮ_ਸੰਖਿਆ_nr), p2->ਖਾਨਾ);
	ਵਾਪਸ FAIL;
    }

    curbuf = findbuf;
    curwin->w_ਬਫਰ = curbuf;
    virtual_op = virtual_ਸਰਗਰਮ();

    // NOTE: Adjustment is needed.
    p1->ਖਾਨਾ--;
    p2->ਖਾਨਾ--;

    ਜੇ (!LT_POS(*p1, *p2))
    {
	// swap position
	pos_T p;

	p = *p1;
	*p1 = *p2;
	*p2 = p;
    }

    ਜੇ (*region_type == MCHAR)
    {
	// Handle 'selection' == "exclusive".
	ਜੇ (is_select_exclusive && !EQUAL_POS(*p1, *p2))
	    // When backing up to previous line, inclusive becomes false.
	    *inclusive = !unadjust_for_sel_inner(p2);
	// If p2 is on NUL (end of line), inclusive becomes false.
	ਜੇ (*inclusive && !virtual_op && *ml_get_pos(p2) == NUL)
	    *inclusive = FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	ਪੂਰਨ	lbr_saved = reset_lbr();
#ਜੇ_ਅੰਤ

	getvvcol(curwin, p1, &sc1, ਨਿਰਾ, &ec1, 0);
	getvvcol(curwin, p2, &sc2, ਨਿਰਾ, &ec2, 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#ਜੇ_ਅੰਤ

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->ਸ਼ੁਰੂ = *p1;
	oap->ਸਿਰਾ = *p2;
	oap->ਸ਼ੁਰੂ_vcol = MIN(sc1, sc2);
	ਜੇ (ਖੰਡ_ਚੌੜਾਈ > 0)
	    oap->ਸਿਰਾ_vcol = oap->ਸ਼ੁਰੂ_vcol + ਖੰਡ_ਚੌੜਾਈ - 1;
	ਨਹੀਂਤਾਂ ਜੇ (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->ਸਿਰਾ_vcol = sc2 - 1;
	ਨਹੀਂਤਾਂ
	    oap->ਸਿਰਾ_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    ਜੇ (l > 1)
	p2->ਖਾਨਾ += l - 1;

    ਵਾਪਸ OK;
}

/*
 * "getregion()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    ਪੂਰਨ			inclusive = TRUE;
    ਪੂਰਨ			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    ਪੂਰਨ			save_virtual;
    string_T		akt = {ਨਿਰਾ, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    ਜੇ (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	ਵਾਪਸ;

    ਲਈ (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	ਪੂਰਨ ret = OK;

	ਜੇ (region_type == MBLOCK)
	{
	    ਬਣਤਰ ਖੰਡ_def	bd;

	    ਖੰਡ_prep(&oa, &bd, lnum, FALSE);
	    ਖੰਡ_def2str(&bd, &akt);
	}
	ਨਹੀਂਤਾਂ ਜੇ (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.ਲੰਮਾਈ = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.ਲੰਮਾਈ);
	    ਜੇ (akt.string == ਨਿਰਾ)
		akt.ਲੰਮਾਈ = 0;
	    ਨਹੀਂਤਾਂ
		akt.ਲੰਮਾਈ = s.ਲੰਮਾਈ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਬਣਤਰ ਖੰਡ_def	bd;

	    charwise_ਖੰਡ_prep(p1, p2, &bd, lnum, inclusive);
	    ਖੰਡ_def2str(&bd, &akt);
	}

	ਜੇ (akt.string == ਨਿਰਾ)
	{
	    clear_tv(rettv);
	    (ਖਾਲੀ)rettv_list_alloc(rettv);
	    ਰੋਕੋ;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (ਪੂਰਨ)akt.ਲੰਮਾਈ);
	vim_free(akt.string);

	ਜੇ (ret == FAIL)
	{
	    clear_tv(rettv);
	    (ਖਾਲੀ)rettv_list_alloc(rettv);
	    ਰੋਕੋ;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_ਬਫਰ = curbuf;
    virtual_op = save_virtual;
}

    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    ਜੇ (l1 == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	ਵਾਪਸ;
    }

    l2 = list_alloc();
    ਜੇ (l2 == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	ਵਾਪਸ;
    }

    l3 = list_alloc();
    ਜੇ (l3 == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	ਵਾਪਸ;
    }

    list_append_ਸੰਖਿਆ(l2, curbuf->b_fnum);
    list_append_ਸੰਖਿਆ(l2, p1.lnum);
    list_append_ਸੰਖਿਆ(l2, p1.ਖਾਨਾ);
    list_append_ਸੰਖਿਆ(l2, p1.coladd);

    list_append_ਸੰਖਿਆ(l3, curbuf->b_fnum);
    list_append_ਸੰਖਿਆ(l3, p2.lnum);
    list_append_ਸੰਖਿਆ(l3, p2.ਖਾਨਾ);
    list_append_ਸੰਖਿਆ(l3, p2.coladd);
}

/*
 * Compute the positions of the region segment on line "lnum".
 * "ret_p1" is set to the start position of the segment and "ret_p2" to its
 * end position.
 */
    ਸਥਿਰ ਖਾਲੀ
getregionpos_ਸਤਰ(
    linenr_T	lnum,
    pos_T	p1,
    pos_T	p2,
    ਪੂਰਨ		inclusive,
    ਪੂਰਨ		region_type,
    oparg_T	*oap,
    ਪੂਰਨ		allow_eol,
    pos_T	*ret_p1,
    pos_T	*ret_p2)
{
    char_u	*ਸਤਰ = ml_get(lnum);
    colnr_T	ਸਤਰ_len = ml_get_len(lnum);

    ਜੇ (region_type == MLINE)
    {
	ret_p1->ਖਾਨਾ = 1;
	ret_p1->coladd = 0;
	ret_p2->ਖਾਨਾ = MAXCOL;
	ret_p2->coladd = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	ਬਣਤਰ ਖੰਡ_def	bd;

	ਜੇ (region_type == MBLOCK)
	    ਖੰਡ_prep(oap, &bd, lnum, FALSE);
	ਨਹੀਂਤਾਂ
	    charwise_ਖੰਡ_prep(p1, p2, &bd, lnum, inclusive);

	ਜੇ (bd.is_oneChar)  // selection entirely inside one char
	{
	    ਜੇ (region_type == MBLOCK)
	    {
		ret_p1->ਖਾਨਾ = mb_prevptr(ਸਤਰ, bd.textstart) - ਸਤਰ + 1;
		ret_p1->coladd = bd.ਸ਼ੁਰੂ_char_vcols
					   - (bd.ਸ਼ੁਰੂ_vcol - oap->ਸ਼ੁਰੂ_vcol);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ret_p1->ਖਾਨਾ = p1.ਖਾਨਾ + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (region_type == MBLOCK && oap->ਸ਼ੁਰੂ_vcol > bd.ਸ਼ੁਰੂ_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->ਖਾਨਾ = MAXCOL;
	    ret_p1->coladd = oap->ਸ਼ੁਰੂ_vcol - bd.ਸ਼ੁਰੂ_vcol;
	    bd.is_oneChar = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (bd.startspaces > 0)
	{
	    ret_p1->ਖਾਨਾ = mb_prevptr(ਸਤਰ, bd.textstart) - ਸਤਰ + 1;
	    ret_p1->coladd = bd.ਸ਼ੁਰੂ_char_vcols - bd.startspaces;
	}
	ਨਹੀਂਤਾਂ
	{
	    ret_p1->ਖਾਨਾ = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	ਜੇ (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->ਖਾਨਾ = ret_p1->ਖਾਨਾ;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	ਨਹੀਂਤਾਂ ਜੇ (bd.endspaces > 0)
	{
	    ret_p2->ਖਾਨਾ = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	ਨਹੀਂਤਾਂ
	{
	    ret_p2->ਖਾਨਾ = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    ਜੇ (!allow_eol && ret_p1->ਖਾਨਾ > ਸਤਰ_len)
    {
	ret_p1->ਖਾਨਾ = 0;
	ret_p1->coladd = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ret_p1->ਖਾਨਾ > ਸਤਰ_len + 1)
	ret_p1->ਖਾਨਾ = ਸਤਰ_len + 1;

    ਜੇ (!allow_eol && ret_p2->ਖਾਨਾ > ਸਤਰ_len)
    {
	ret_p2->ਖਾਨਾ = ret_p1->ਖਾਨਾ == 0 ? 0 : ਸਤਰ_len;
	ret_p2->coladd = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ret_p2->ਖਾਨਾ > ਸਤਰ_len + 1)
	ret_p2->ਖਾਨਾ = ਸਤਰ_len + 1;

    ret_p1->lnum = lnum;
    ret_p2->lnum = lnum;
}

/*
 * "getregionpos()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getregionpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	p1, p2;
    ਪੂਰਨ		inclusive = TRUE;
    ਪੂਰਨ		region_type = -1;
    ਪੂਰਨ		allow_eol = FALSE;
    ਪੂਰਨ		bounds_only = FALSE;
    oparg_T	oa;
    ਪੂਰਨ		lnum;

    buf_T	*save_curbuf;
    ਪੂਰਨ		save_virtual;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    ਜੇ (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    ਜੇ (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	ਸ਼ੁਰੂ_pos, ਸਿਰਾ_pos;

	getregionpos_ਸਤਰ(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &ਸ਼ੁਰੂ_pos, &ਸਿਰਾ_pos);
	ਜੇ (p2.lnum != p1.lnum)
	{
	    pos_T	ਅਣਵਰਤਿਆ_2;

	    getregionpos_ਸਤਰ(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &ਅਣਵਰਤਿਆ_2, &ਸਿਰਾ_pos);
	}
	ਜੋੜੋ_regionpos_range(rettv, ਸ਼ੁਰੂ_pos, ਸਿਰਾ_pos);
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_ਸਤਰ(lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &ret_p1, &ret_p2);
	    ਜੋੜੋ_regionpos_range(rettv, ret_p1, ret_p2);
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_ਬਫਰ = curbuf;
    virtual_op = save_virtual;
}

/*
 * Common between getreg(), getreginfo() and getregtype(): get the register
 * name from the first argument.
 * Returns zero on error.
 */
    ਸਥਿਰ ਪੂਰਨ
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	ਜੇ (strregname != ਨਿਰਾ && ਵਿੱਚ_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_ਰਜਿਸਟਰ_name_must_be_one_char_str), strregname);
	    strregname = ਨਿਰਾ;
	}
	ਜੇ (strregname == ਨਿਰਾ)	    // type error; errmsg already given
	    ਵਾਪਸ 0;
    }
    ਨਹੀਂਤਾਂ
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    ਵਾਪਸ *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getreg(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		regname;
    ਪੂਰਨ		arg2 = FALSE;
    ਪੂਰਨ		return_list = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	ਵਾਪਸ;

    regname = getreg_get_regname(argvars);
    ਜੇ (regname == 0)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	ਪੂਰਨ		error = FALSE;

	arg2 = (ਪੂਰਨ)tv_get_bool_chk(&argvars[1], &error);

	ਜੇ (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (ਪੂਰਨ)tv_get_bool_chk(&argvars[2], &error);
	ਜੇ (error)
	    ਵਾਪਸ;
    }

    ਜੇ (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_ਵਿਅੰਜਕ_SRC : 0) | GREG_LIST);
	ਜੇ (rettv->vval.v_list == ਨਿਰਾ)
	    (ਖਾਲੀ)rettv_list_alloc(rettv);
	ਨਹੀਂਤਾਂ
	    ++rettv->vval.v_list->lv_refcount;
    }
    ਨਹੀਂਤਾਂ
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = get_reg_contents(regname,
						    arg2 ? GREG_ਵਿਅੰਜਕ_SRC : 0);
    }
}

/*
 * "getregtype()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getregtype(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		regname;
    char_u	buf[NUMBUFLEN + 2];
    ਲੰਮਾ	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    regname = getreg_get_regname(argvars);
    ਜੇ (regname == 0)
	ਵਾਪਸ;

    buf[0] = NUL;
    buf[1] = NUL;
    ਚੋਣ (get_reg_type(regname, &reglen))
    {
	ਹਾਲਤ MLINE: buf[0] = 'V'; ਰੋਕੋ;
	ਹਾਲਤ MCHAR: buf[0] = 'v'; ਰੋਕੋ;
	ਹਾਲਤ MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((ਅੱਖਰ *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		ਰੋਕੋ;
    }
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "gettagstack()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_gettagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;			// default is current window

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
	ਜੇ (wp == ਨਿਰਾ)
	    ਵਾਪਸ;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_gettext(typval_T *argvars, typval_T *rettv)
{
#ਜੇ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    ਅੱਖਰ *prev = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    ਜੇ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_STRING;

    ਜੇ (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != ਨਿਰਾ &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#ਜੇ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ਅਚਲ ਅੱਖਰ *)argvars[1].vval.v_string, (ਅੱਖਰ *)p_enc);
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((ਅਚਲ ਅੱਖਰ *)argvars[1].vval.v_string, (ਅਚਲ ਅੱਖਰ *)argvars[0].vval.v_string));
#ਨਹੀਂਤਾਂ
	textdomain((ਅਚਲ ਅੱਖਰ *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	ਜੇ (prev != ਨਿਰਾ)
	    bind_textdomain_codeset((ਅਚਲ ਅੱਖਰ *)argvars[1].vval.v_string, prev);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
}

// for VIM_VERSION_ defines
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

/*
 * "has()" function
 */
    ਖਾਲੀ
f_has(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		i;
    char_u	*name;
    ਪੂਰਨ		x = FALSE;
    ਪੂਰਨ		n = FALSE;
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
	ਅੱਖਰ *name;
	ਛੋਟਾ present;
    } has_ਵਸਤੂ_T;
    ਸਥਿਰ has_ਵਸਤੂ_T has_list[] =
    {
	{"amiga",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"android",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __ANDROID__
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"arp",
#ਜੇ defined(AMIGA) && defined(FEAT_ARP)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"haiku",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __HAIKU__
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"bsd",
#ਜੇ defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"hpux",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ hpux
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"hurd",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GNU__
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"linux",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __linux__
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_X
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"osx",		// Mac OS X
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_X
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"macunix",	// Mac OS X, with the darwin feature
#ਜੇ defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"osxdarwin",	// synonym for macunix
#ਜੇ defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"qnx",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __QNX__
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"sun",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SUN_SYSTEM
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"termux",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __TERMUX__
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"unix",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"vms",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"win32",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"win32unix",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32UNIX
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"win64",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN64
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"ebcdic", 0 },
	{"fname_case",
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CASE_INSENSITIVE_FILENAME
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"acl",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"arabic",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"autocmd", 1},
	{"autochdir",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_AUTOCHDIR
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"autoservername",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_AUTOSERVERNAME
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"socketserver",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOCKETSERVER
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"balloon_eval",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"balloon_multiline",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"balloon_eval_term",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_ਪਦ
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#ਜੇ defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"byte_offset",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BYTEOFF
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"channel",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"cindent", 1},
	{"clientserver",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"clipboard",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"clipboard_provider",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"cmdline_compl", 1},
	{"cmdline_hist", 1},
	{"cmdwin", 1},
	{"comments", 1},
	{"conceal",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"cryptv",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"crypt-blowfish",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"crypt-blowfish2",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"cscope",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"cursorbind", 1},
	{"cursorshape",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"debug",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"dialog_con",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CON_DIALOG
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"dialog_con_gui",
#ਜੇ defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"dialog_gui",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_DIALOG
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"diff",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"digraphs",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"directx",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIRECTX
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"dnd",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"drop_file",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_DROP_FILE
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"emacs_tags",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"file_in_path", 1},
	{"filterpipe",
#ਜੇ defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"find_in_path",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"float", 1},
	{"folding",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"footer", 0},
	{"fork",
#ਜੇ !defined(USE_SYSTEM) && defined(UNIX)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"gettext",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GETTEXT
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"gui",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"gui_gtk2",
#ਜੇ defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"gui_gtk3",
#ਜੇ defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"gui_gtk4",
#ਜੇ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"gui_gnome",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GNOME
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"gui_haiku",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_HAIKU
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"gui_mac", 0},
	{"gui_motif",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MOTIF
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"gui_photon",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_PHOTON
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"gui_win32",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"iconv",
#ਜੇ defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"image",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"image_popup",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"insert_expand", 1},
	{"ipv6",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IPV6
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
	},
	{"job",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"jumplist", 1},
	{"keymap",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LANGMAP
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"libcall",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LIBCALL
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"linebreak",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#ਜੇ defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"menu",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"mksession",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"mouse_dec",
#ਜੇ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"mouse_gpm",
#ਜੇ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"mouse_jsbterm",
#ਜੇ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"mouse_netterm",
#ਜੇ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"mouse_pterm",
#ਜੇ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"mouse_sgr",
#ਜੇ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"mouse_sysmouse",
#ਜੇ (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"mouse_urxvt",
#ਜੇ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"mouse_xterm",
#ਜੇ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"multi_lang",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"mzscheme",
#ਜੇ defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"nanotime",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ST_MTIM_NSEC
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
	},
	{"num64", 1},
	{"ole",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_OLE
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"packages",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"path_extra", 1},
	{"perl",
#ਜੇ defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"persistent_undo",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"python_compiled",
#ਜੇ defined(FEAT_PYTHON)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"python_dynamic",
#ਜੇ defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"python",
#ਜੇ defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"pythonx",
#ਜੇ (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"python3_compiled",
#ਜੇ defined(FEAT_PYTHON3)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"python3_dynamic",
#ਜੇ defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"python3_stable",
#ਜੇ defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"python3",
#ਜੇ defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"popupwin",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"postscript",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_POSTSCRIPT
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"pango",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINT_PANGO
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"printer",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINTER
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"profile",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"prof_nsec",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PROF_NSEC
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"reltime",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"quickfix",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"rightleft",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"ruby",
#ਜੇ defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"smartindent", 1},
	{"startuptime",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ STARTUPTIME
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"statusline",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"statusline_click",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"netbeans_intg",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"sodium",
#ਜੇ defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"sound",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOUND
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"spell",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"syntax",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"system",
#ਜੇ defined(USE_SYSTEM) || !defined(UNIX)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"tabpanel",
#ਜੇ defined(FEAT_TABPANEL)
		1,
#ਨਹੀਂਤਾਂ
		0,
#ਜੇ_ਅੰਤ
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#ਜੇ defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"termguicolors",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"terminal",
#ਜੇ defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"terminfo",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"termresponse",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"textobjects", 1},
	{"textprop",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"tgetent",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"timers",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"title", 1},
	{"toolbar",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"vertsplit", 1},
	{"viminfo",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VTP
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"wayland",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"wayland_clipboard",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAK
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"writebackup",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WRITEBACKUP
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"xattr",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XATTR
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"xim",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"xfontset",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"xpm",
#ਜੇ defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"xpm_w32",	// for backward compatibility
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XPM_W32
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"xsmp",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_XSMP
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"xsmp_interact",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_XSMP_INTERACT
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"xterm_clipboard",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XCLIPBOARD
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"xterm_save",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XTERM_SAVE
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{"X11",
#ਜੇ defined(UNIX) && defined(FEAT_X11)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#ਜੇ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#ਨਹੀਂਤਾਂ
		0
#ਜੇ_ਅੰਤ
		},
	{ਨਿਰਾ, 0}
    };

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    name = tv_get_string(&argvars[0]);

    // Fast-path: check features not in has_list[] first to avoid the full
    // linear scan for very common queries like has('patch-...').
    ਜੇ (STRNICMP(name, "patch", 5) == 0)
    {
	x = TRUE;
	ਜੇ (name[5] == '-'
		&& STRLEN(name) >= 11
		&& (name[6] >= '1' && name[6] <= '9'))
	{
	    ਅੱਖਰ	*ਸਿਰਾ;
	    ਪੂਰਨ	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (ਪੂਰਨ)strtoul((ਅੱਖਰ *)name + 6, &ਸਿਰਾ, 10);
	    ਜੇ (*ਸਿਰਾ == '.' && vim_isdigit(ਸਿਰਾ[1])
		    && ਸਿਰਾ[2] == '.' && vim_isdigit(ਸਿਰਾ[3]))
	    {
		minor = atoi(ਸਿਰਾ + 1);

		// Expect "patch-9.9.01234".
		n = (major < VIM_ਸੰਸਕਰਣ_MAJOR
		     || (major == VIM_ਸੰਸਕਰਣ_MAJOR
			 && (minor < VIM_ਸੰਸਕਰਣ_MINOR
			     || (minor == VIM_ਸੰਸਕਰਣ_MINOR
				 && has_patch(atoi(ਸਿਰਾ + 3))))));
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਸੁਰੱਖਿਅਤ_isdigit(name[5]))
	    n = has_patch(atoi((ਅੱਖਰ *)name + 5));
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_ਨਿਵੇਸ਼_isatty();
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	n = (gui.ਵਿੱਚ_use || gui.starting);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#ਜੇ defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.ਵਿੱਚ_use;	// gui_mch_browse() works when GUI is running
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	n = syntax_present(curwin);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VTP
	n = is_ਪਦ_win32() && has_vtp_working();
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	n = netbeans_ਸਰਗਰਮ();
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_GPM
	n = gpm_enabled();
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#ਜੇ defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_ਤਾਰਾ.ਉਪਲਬਧ;
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# ਜੇ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	ਜੇ (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	ਨਹੀਂਤਾਂ
	    n = FALSE;
# ਨਹੀਂਤਾਂ
	n = FALSE;
# endif
#ਜੇ_ਅੰਤ
    }

    // Look up in has_list[] only if not already handled above.
    ਜੇ (x == FALSE)
    {
	ਲਈ (i = 0; has_list[i].name != ਨਿਰਾ; ++i)
	    ਜੇ (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		ਰੋਕੋ;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	ਜੇ (x == TRUE && n == FALSE)
	{
	    ਜੇ (0)
	    {
		// intentionally empty
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VIMDLL
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "filterpipe") == 0)
		n = gui.ਵਿੱਚ_use || gui.starting;
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_LUA
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_MZSCHEME
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_PERL
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_PYTHON
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_PYTHON3
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "pythonx") == 0)
	    {
# ਜੇ defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		ਜੇ (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		ਨਹੀਂਤਾਂ ਜੇ (p_pyx == 3)
		    n = python3_enabled(FALSE);
		ਨਹੀਂਤਾਂ ਜੇ (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_RUBY
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_TCL
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_SODIUM
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_TERMINAL) && defined(MSWIN)
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_GPM
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_ਉਪਲਬਧ();
#ਜੇ_ਅੰਤ
	}
    }

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_ਸੰਖਿਆ = x;
    ਨਹੀਂਤਾਂ
	// return whether feature is enabled
	rettv->vval.v_ਸੰਖਿਆ = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    ਪੂਰਨ
dynamic_feature(char_u *feature)
{
    ਵਾਪਸ (feature == ਨਿਰਾ
#ਜੇ defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.ਵਿੱਚ_use)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.ਵਿੱਚ_use)
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    || STRICMP(feature, "iconv") == 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_LUA
	    || STRICMP(feature, "lua") == 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_GPM
	    || (STRICMP(feature, "mouse_gpm_enabled") == 0 && !gpm_enabled())
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_MZSCHEME
	    || STRICMP(feature, "mzscheme") == 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	    || STRICMP(feature, "netbeans_enabled") == 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_PERL
	    || STRICMP(feature, "perl") == 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_PYTHON
	    || STRICMP(feature, "python") == 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_PYTHON3
	    || STRICMP(feature, "python3") == 0
#ਜੇ_ਅੰਤ
#ਜੇ defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    || STRICMP(feature, "pythonx") == 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_RUBY
	    || STRICMP(feature, "ruby") == 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    || STRICMP(feature, "syntax_items") == 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_TCL
	    || STRICMP(feature, "tcl") == 0
#ਜੇ_ਅੰਤ
	    // once "starting" is zero it will stay that way
	    || (STRICMP(feature, "vim_starting") == 0 && starting != 0)
	    || STRICMP(feature, "multi_byte_encoding") == 0
#ਜੇ defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#ਜੇ_ਅੰਤ
	    );
}

/*
 * "haslocaldir()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = ਨਿਰਾ;
    win_T	*wp = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL)))
	ਵਾਪਸ;

    wp = ਲੱਭੋ_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    ਜੇ (wp != ਨਿਰਾ && wp->w_localdir != ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = 1;
    ਨਹੀਂਤਾਂ ਜੇ (tp != ਨਿਰਾ && tp->tp_localdir != ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = 2;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = 0;
}

/*
 * "highlightID(name)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_hlID(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_hostname(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS // VMS does not have defined uintptr_t
# ਜੇ defined(HAVE_NO_LONG_LONG)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ uintptr_t;
# ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ uintptr_t;
# endif
#ਜੇ_ਅੰਤ // VMS

    ਸਥਿਰ ਖਾਲੀ
f_id(typval_T *argvars, typval_T *rettv)
{
    ਅੱਖਰ    numbuf[NUMBUFLEN];
    ਅੱਖਰ    *p = numbuf;

    ਚੋਣ (argvars[0].v_type)
    {
	ਹਾਲਤ VAR_LIST:
	ਹਾਲਤ VAR_TUPLE:
	ਹਾਲਤ VAR_DICT:
	ਹਾਲਤ VAR_OBJECT:
	ਹਾਲਤ VAR_ਕੰਮ:
	ਹਾਲਤ VAR_CHANNEL:
	ਹਾਲਤ VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    ਜੇ (argvars[0].vval.v_object != ਨਿਰਾ)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(ਖਾਲੀ *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		ਲਈ (; p < numbuf + ਆਕਾਰ(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	ਮੂਲ:
	    ਰੋਕੋ;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੂਚਕਾਂਕ_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    ਪੂਰਨ		ਸ਼ੁਰੂ = 0;
    ਪੂਰਨ		error = FALSE;
    ਪੂਰਨ		ic = FALSE;

    b = argvars[0].vval.v_blob;
    ਜੇ (b == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ਸ਼ੁਰੂ = tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
	ਜੇ (error)
	    ਵਾਪਸ;
    }

    ਜੇ (ਸ਼ੁਰੂ < 0)
    {
	ਸ਼ੁਰੂ = blob_len(b) + ਸ਼ੁਰੂ;
	ਜੇ (ਸ਼ੁਰੂ < 0)
	    ਸ਼ੁਰੂ = 0;
    }

    ਲਈ (ਪੂਰਨ idx = ਸ਼ੁਰੂ; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_ਸੰਖਿਆ;
	tv.vval.v_ਸੰਖਿਆ = blob_get(b, idx);
	ਜੇ (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_ਸੰਖਿਆ = idx;
	    ਵਾਪਸ;
	}
    }
}

/*
 * index() function for a tuple
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੂਚਕਾਂਕ_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    ਪੂਰਨ		ic = FALSE;
    ਪੂਰਨ		error = FALSE;

    ਜੇ (tuple == ਨਿਰਾ)
	ਵਾਪਸ;

    ਪੂਰਨ	ਸ਼ੁਰੂ_idx = 0;
    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ਸ਼ੁਰੂ_idx = tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
	ਜੇ (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (ਪੂਰਨ)tv_get_bool_chk(&argvars[3], &error);
	ਜੇ (error)
	    ਵਾਪਸ;
    }

    rettv->vval.v_ਸੰਖਿਆ = ਸੂਚਕਾਂਕ_tuple(tuple, &argvars[1], ਸ਼ੁਰੂ_idx, ic);
}

/*
 * index() function for a list
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੂਚਕਾਂਕ_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*ਵਸਤੂ;
    ਲੰਮਾ	idx = 0;
    ਪੂਰਨ		ic = FALSE;
    ਪੂਰਨ		error = FALSE;

    l = argvars[0].vval.v_list;
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ;

    CHECK_LIST_MATERIALIZE(l);
    ਵਸਤੂ = l->lv_ਪਹਿਲਾ;
    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	ਵਸਤੂ = list_ਲੱਭੋ(l, (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	ਜੇ (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (ਪੂਰਨ)tv_get_bool_chk(&argvars[3], &error);
	ਜੇ (error)
	    ਵਸਤੂ = ਨਿਰਾ;
    }

    ਲਈ ( ; ਵਸਤੂ != ਨਿਰਾ; ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ, ++idx)
	ਜੇ (tv_equal(&ਵਸਤੂ->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_ਸੰਖਿਆ = idx;
	    ਰੋਕੋ;
	}
}

/*
 * "index()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_ਸੂਚਕਾਂਕ(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL)
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_BLOB)
	ਸੂਚਕਾਂਕ_func_blob(argvars, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_TUPLE)
	ਸੂਚਕਾਂਕ_func_tuple(argvars, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_LIST)
	ਸੂਚਕਾਂਕ_func_list(argvars, rettv);
    ਨਹੀਂਤਾਂ
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    ਪੂਰਨ
indexof_eval_ਵਿਅੰਜਕ(typval_T *ਵਿਅੰਜਕ)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	ਮਿਲਿਆ;
    ਪੂਰਨ		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_ਕੁੰਜੀ);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    ਜੇ (eval_ਵਿਅੰਜਕ_typval(ਵਿਅੰਜਕ, FALSE, argv, 2, ਨਿਰਾ, &newtv) == FAIL)
	ਵਾਪਸ FALSE;

    ਮਿਲਿਆ = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    ਵਾਪਸ error ? FALSE : ਮਿਲਿਆ;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    ਸਥਿਰ ਪੂਰਨ
indexof_blob(blob_T *b, ਲੰਮਾ startidx, typval_T *ਵਿਅੰਜਕ)
{
    ਲੰਮਾ	idx = 0;

    ਜੇ (b == ਨਿਰਾ)
	ਵਾਪਸ -1;

    ਜੇ (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	ਜੇ (startidx < 0)
	    startidx = 0;
    }

    ਸੈੱਟ_vim_var_type(VV_ਕੁੰਜੀ, VAR_ਸੰਖਿਆ);
    ਸੈੱਟ_vim_var_type(VV_VAL, VAR_ਸੰਖਿਆ);

    ਪੂਰਨ		called_emsg_ਸ਼ੁਰੂ = called_emsg;
    ਲਈ (idx = startidx; idx < blob_len(b); ++idx)
    {
	ਸੈੱਟ_vim_var_nr(VV_ਕੁੰਜੀ, idx);
	ਸੈੱਟ_vim_var_nr(VV_VAL, blob_get(b, idx));

	ਜੇ (indexof_eval_ਵਿਅੰਜਕ(ਵਿਅੰਜਕ))
	    ਵਾਪਸ idx;

	ਜੇ (called_emsg != called_emsg_ਸ਼ੁਰੂ)
	    ਵਾਪਸ -1;
    }

    ਵਾਪਸ -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ਸਥਿਰ ਪੂਰਨ
indexof_list(list_T *l, ਲੰਮਾ startidx, typval_T *ਵਿਅੰਜਕ)
{
    listitem_T	*ਵਸਤੂ;
    ਲੰਮਾ	idx = 0;
    ਪੂਰਨ		ਮਿਲਿਆ;

    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ -1;

    CHECK_LIST_MATERIALIZE(l);

    ਜੇ (startidx == 0)
	ਵਸਤੂ = l->lv_ਪਹਿਲਾ;
    ਨਹੀਂਤਾਂ
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	ਵਸਤੂ = list_ਲੱਭੋ(l, startidx);
	ਜੇ (ਵਸਤੂ != ਨਿਰਾ)
	    idx = l->lv_u.mat.lv_idx;
    }

    ਸੈੱਟ_vim_var_type(VV_ਕੁੰਜੀ, VAR_ਸੰਖਿਆ);

    ਪੂਰਨ		called_emsg_ਸ਼ੁਰੂ = called_emsg;
    ਲਈ ( ; ਵਸਤੂ != ਨਿਰਾ; ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ, ++idx)
    {
	ਸੈੱਟ_vim_var_nr(VV_ਕੁੰਜੀ, idx);
	copy_tv(&ਵਸਤੂ->li_tv, get_vim_var_tv(VV_VAL));

	ਮਿਲਿਆ = indexof_eval_ਵਿਅੰਜਕ(ਵਿਅੰਜਕ);
	clear_tv(get_vim_var_tv(VV_VAL));

	ਜੇ (ਮਿਲਿਆ)
	    ਵਾਪਸ idx;

	ਜੇ (called_emsg != called_emsg_ਸ਼ੁਰੂ)
	    ਵਾਪਸ -1;
    }

    ਵਾਪਸ -1;
}

/*
 * "indexof()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_indexof(typval_T *argvars, typval_T *rettv)
{
    ਲੰਮਾ	startidx = 0;
    typval_T	save_val;
    typval_T	save_ਕੁੰਜੀ;
    ਪੂਰਨ		save_did_emsg;

    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ਵਾਪਸ;

    ਜੇ ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == ਨਿਰਾ
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == ਨਿਰਾ))
	ਵਾਪਸ;

    ਜੇ (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_ਸੰਖਿਆ_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_ਕੁੰਜੀ, &save_ਕੁੰਜੀ);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    ਜੇ (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_ਸੰਖਿਆ = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_ਸੰਖਿਆ = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_ਕੁੰਜੀ, &save_ਕੁੰਜੀ);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

ਸਥਿਰ ਪੂਰਨ inputsecret_flag = 0;

/*
 * "input()" function
 *     Also handles inputsecret() when inputsecret is set.
 */
    ਸਥਿਰ ਖਾਲੀ
f_ਨਿਵੇਸ਼(typval_T *argvars, typval_T *rettv)
{
    get_user_ਨਿਵੇਸ਼(argvars, rettv, FALSE, inputsecret_flag);
}

/*
 * "inputdialog()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_inputdialog(typval_T *argvars, typval_T *rettv)
{
#ਜੇ defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    ਜੇ (gui.ਵਿੱਚ_use && vim_strchr(p_go, GO_CONDIALOG) == ਨਿਰਾ)
    {
	char_u	*ਸੁਨੇਹਾ_2;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	ਜੇ (ਵਿੱਚ_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    ਵਾਪਸ;

	ਸੁਨੇਹਾ_2 = tv_get_string_chk(&argvars[0]);
	ਜੇ (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != ਨਿਰਾ)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	ਨਹੀਂਤਾਂ
	    IObuff[0] = NUL;
	ਜੇ (ਸੁਨੇਹਾ_2 != ਨਿਰਾ && defstr != ਨਿਰਾ
		&& do_dialog(VIM_QUESTION, ਨਿਰਾ, ਸੁਨੇਹਾ_2,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (ਸੁਨੇਹਾ_2 != ਨਿਰਾ && defstr != ਨਿਰਾ
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    ਨਹੀਂਤਾਂ
		rettv->vval.v_string = ਨਿਰਾ;
	}
	rettv->v_type = VAR_STRING;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	get_user_ਨਿਵੇਸ਼(argvars, rettv, TRUE, inputsecret_flag);
}

/*
 * "inputlist()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_inputlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    ਪੂਰਨ		selected;
    ਪੂਰਨ		mouse_ਵਰਤਿਆ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NO_CONSOLE_ਨਿਵੇਸ਼
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    ਜੇ (no_console_ਨਿਵੇਸ਼() && !is_not_a_ਪਦ())
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ਨਿਰਾ)
    {
	semsg(_(e_ਦਲੀਲ_ਦਾ_str_must_be_list), "inputlist()");
	ਵਾਪਸ;
    }

    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;	// for when 'cmdheight' > 1
    ਸਤਰਾਂ_ਖੱਬਾ = ਪੰਕਤੀਆਂ;	// avoid more prompt
    ਸੁਨੇਹਾ_scroll = TRUE;
    ਸੁਨੇਹਾ_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)tv_get_string(&li->li_tv));
	ਸੁਨੇਹਾ_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_ਸੰਖਿਆ(&mouse_ਵਰਤਿਆ);
    ਜੇ (mouse_ਵਰਤਿਆ)
	selected -= ਸਤਰਾਂ_ਖੱਬਾ;

    rettv->vval.v_ਸੰਖਿਆ = selected;
}

ਸਥਿਰ garray_T	    ga_userinput = {0, 0, ਆਕਾਰ(tasave_T), 4, ਨਿਰਾ};

/*
 * "inputrestore()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_inputrestore(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਜੇ (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_ਅੰਕੜੇ)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    ਨਹੀਂਤਾਂ ਜੇ (p_verbose > 1)
    {
	verb_ਸੁਨੇਹਾ(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_ਸੰਖਿਆ = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_inputsave(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    ਜੇ (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_ਅੰਕੜੇ)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ++cmdline_ਤਾਰਾ;
    ++inputsecret_flag;
    f_ਨਿਵੇਸ਼(argvars, rettv);
    --cmdline_ਤਾਰਾ;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_ਰੁਕਾਵਟ(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_invert(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = ~tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    ਸਥਿਰ ਖਾਲੀ
free_lval_ਜੜ੍ਹ(lval_ਜੜ੍ਹ_T *ਜੜ੍ਹ)
{
    ਜੇ (ਜੜ੍ਹ->lr_tv != ਨਿਰਾ)
	free_tv(ਜੜ੍ਹ->lr_tv);
    class_unref(ਜੜ੍ਹ->lr_cl_exec);
    ਜੜ੍ਹ->lr_tv = ਨਿਰਾ;
    ਜੜ੍ਹ->lr_cl_exec = ਨਿਰਾ;
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

    ਸਥਿਰ ਪੂਰਨ
fix_variable_reference_lval_ਜੜ੍ਹ(lval_ਜੜ੍ਹ_T *ਜੜ੍ਹ, char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    ਪੂਰਨ ਮਿਲਿਆ_member = FALSE;
    ਜੇ (ਜੜ੍ਹ->lr_tv->v_type == VAR_OBJECT)
    {
	ਜੇ (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    ਮਿਲਿਆ_member = TRUE;
    }
    ਜੇ (!ਮਿਲਿਆ_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*ਸਿਰਾ;
	ਲਈ (ਸਿਰਾ = name; ASCII_ISALNUM(*ਸਿਰਾ) || *ਸਿਰਾ == '_'; ++ਸਿਰਾ)
	    ;
	ਪੂਰਨ idx = class_member_idx(ਜੜ੍ਹ->lr_cl_exec, name, ਸਿਰਾ - name);
	ਜੇ (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(ਜੜ੍ਹ->lr_tv);
	    copy_tv(&ਜੜ੍ਹ->lr_cl_exec->class_members_tv[idx], ਜੜ੍ਹ->lr_tv);
	    ਮਿਲਿਆ_member = TRUE;
	}
    }
    ਜੇ (!ਮਿਲਿਆ_member)
    {
	free_tv(ਜੜ੍ਹ->lr_tv);
	ਜੜ੍ਹ->lr_tv = ਨਿਰਾ;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    ਵਾਪਸ OK;
}

/*
 * "islocked()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*ਸਿਰਾ;
    dictitem_T	*di;

    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    char_u *name = tv_get_string(&argvars[0]);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
    ch_log(ਨਿਰਾ, "LKVAR: f_islocked(): name: %s", name);
#ਜੇ_ਅੰਤ

    lval_ਜੜ੍ਹ_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_ਜੜ੍ਹ_T *ਜੜ੍ਹ = ਨਿਰਾ;

    // Set up lval_root if executing in a method.
    ਜੇ (fill_exec_lval_ਜੜ੍ਹ(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	ਜੇ (fix_variable_reference_lval_ਜੜ੍ਹ(&aroot, name) == OK)
	    ਜੜ੍ਹ = &aroot;
    }

    lval_ਜੜ੍ਹ_T	*lval_ਜੜ੍ਹ_save = lval_ਜੜ੍ਹ;
    lval_ਜੜ੍ਹ = ਜੜ੍ਹ;
    ਸਿਰਾ = get_lval(name, ਨਿਰਾ, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_ਸ਼ੁਰੂ);
    lval_ਜੜ੍ਹ = lval_ਜੜ੍ਹ_save;

    ਜੇ (ਸਿਰਾ != ਨਿਰਾ && lv.ll_name != ਨਿਰਾ)
    {
	ਜੇ (*ਸਿਰਾ != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_ਸਿਰਾ
		   ? e_invalid_ਦਲੀਲ_str : e_trailing_characters_str), ਸਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (lv.ll_tv == ਨਿਰਾ)
	    {
		di = ਲੱਭੋ_var(lv.ll_name, ਨਿਰਾ, TRUE);
		ਜੇ (di != ਨਿਰਾ)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_ਸੰਖਿਆ = ((di->di_flags & DI_FLAGS_ਤਾਲਾ)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (lv.ll_is_ਜੜ੍ਹ)
	    {
		rettv->vval.v_ਸੰਖਿਆ = tv_islocked(lv.ll_tv);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (lv.ll_object != ਨਿਰਾ)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_ਸੰਖਿਆ = tv_islocked(tv);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
		ch_log(ਨਿਰਾ, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (lv.ll_class != ਨਿਰਾ)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_ਸੰਖਿਆ = tv_islocked(tv);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
		ch_log(ਨਿਰਾ, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    ਨਹੀਂਤਾਂ ਜੇ (lv.ll_newkey != ਨਿਰਾ)
		semsg(_(e_ਕੁੰਜੀ_not_present_ਵਿੱਚ_dictionary_str), lv.ll_newkey);
	    ਨਹੀਂਤਾਂ ਜੇ (lv.ll_list != ਨਿਰਾ)
		// List item.
		rettv->vval.v_ਸੰਖਿਆ = tv_islocked(&lv.ll_li->li_tv);
	    ਨਹੀਂਤਾਂ ਜੇ (lv.ll_tuple != ਨਿਰਾ)
		// Tuple item.
		rettv->vval.v_ਸੰਖਿਆ = tv_islocked(lv.ll_tv);
	    ਨਹੀਂਤਾਂ
		// Dictionary item.
		rettv->vval.v_ਸੰਖਿਆ = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    ਜੇ (ਜੜ੍ਹ != ਨਿਰਾ)
	free_lval_ਜੜ੍ਹ(ਜੜ੍ਹ);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_ਚੀਜ਼ਾਂ(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    ਨਹੀਂਤਾਂ
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_ਦਲੀਲ_nr), 1);
}

/*
 * "keytrans()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    ਜੇ (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == ਨਿਰਾ)
	ਵਾਪਸ;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    ਸਥਿਰ ਖਾਲੀ
f_ਆਖਰੀ_ਬਫਰ_nr(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਪੂਰਨ		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ਜੇ (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_ਸੰਖਿਆ = n;
}

/*
 * "len()" function
 */
    ਖਾਲੀ
f_len(typval_T *argvars, typval_T *rettv)
{
    ਚੋਣ (argvars[0].v_type)
    {
	ਹਾਲਤ VAR_STRING:
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	    rettv->vval.v_ਸੰਖਿਆ = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_BLOB:
	    rettv->vval.v_ਸੰਖਿਆ = blob_len(argvars[0].vval.v_blob);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    rettv->vval.v_ਸੰਖਿਆ = list_len(argvars[0].vval.v_list);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    rettv->vval.v_ਸੰਖਿਆ = tuple_len(argvars[0].vval.v_tuple);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    rettv->vval.v_ਸੰਖਿਆ = dict_len(argvars[0].vval.v_dict);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_OBJECT:
	    rettv->vval.v_ਸੰਖਿਆ = object_len(argvars[0].vval.v_object);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	ਹਾਲਤ VAR_FLOAT:
	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL:
	ਹਾਲਤ VAR_ਕੰਮ:
	ਹਾਲਤ VAR_CHANNEL:
	ਹਾਲਤ VAR_INSTR:
	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    ਰੋਕੋ;
    }
}

    ਸਥਿਰ ਖਾਲੀ
libcall_common(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv, ਪੂਰਨ type)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LIBCALL
    char_u		*string_ਵਿੱਚ;
    char_u		**string_ਨਤੀਜਾ;
    ਪੂਰਨ			nr_ਨਤੀਜਾ;
#ਜੇ_ਅੰਤ

    rettv->v_type = type;
    ਜੇ (type != VAR_ਸੰਖਿਆ)
	rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_ਸੰਖਿਆ_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    ਜੇ (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_ਵਿੱਚ = ਨਿਰਾ;
	ਜੇ (argvars[2].v_type == VAR_STRING)
	    string_ਵਿੱਚ = argvars[2].vval.v_string;
	ਜੇ (type == VAR_ਸੰਖਿਆ)
	{
	    string_ਨਤੀਜਾ = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    rettv->vval.v_string = ਨਿਰਾ;
	    string_ਨਤੀਜਾ = &rettv->vval.v_string;
	}
	ਜੇ (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_ਵਿੱਚ,
			     argvars[2].vval.v_ਸੰਖਿਆ,
			     string_ਨਤੀਜਾ,
			     &nr_ਨਤੀਜਾ) == OK
		&& type == VAR_ਸੰਖਿਆ)
	    rettv->vval.v_ਸੰਖਿਆ = nr_ਨਤੀਜਾ;
    }
#ਜੇ_ਅੰਤ
}

/*
 * "libcall()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_libcall(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_STRING);
}

/*
 * "libcallnr()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_libcallnr(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_ਸੰਖਿਆ);
}

/*
 * "line(string, [winid])" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_ਸਤਰ(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum = 0;
    pos_T	*fp = ਨਿਰਾ;
    ਪੂਰਨ		fnum;
    ਪੂਰਨ		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	ਜੇ (wp != ਨਿਰਾ && tp != ਨਿਰਾ)
	{
	    ਜੇ (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		ਜੇ (*p_spk != 'c'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		|| (curwin->w_p_diff && switchwin.sw_curwin->w_p_diff)
#ਜੇ_ਅੰਤ
		)
		    skip_update_topline = TRUE;
		check_cursor();
		fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	    }
	    skip_update_topline = FALSE;
	    restore_win_noblock(&switchwin, TRUE);
	}
    }
    ਨਹੀਂਤਾਂ
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    ਜੇ (fp != ਨਿਰਾ)
	lnum = fp->lnum;
    rettv->vval.v_ਸੰਖਿਆ = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_line2byte(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_BYTEOFF
    rettv->vval.v_ਸੰਖਿਆ = -1;
#ਨਹੀਂਤਾਂ
    linenr_T	lnum;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    lnum = tv_get_lnum(argvars);
    ਜੇ (lnum < 1 || lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1)
	rettv->vval.v_ਸੰਖਿਆ = -1;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = ml_ਲੱਭੋ_ਸਤਰ_or_ਵਿੱਥ(curbuf, lnum, ਨਿਰਾ);
    ਜੇ (rettv->vval.v_ਸੰਖਿਆ >= 0)
	++rettv->vval.v_ਸੰਖਿਆ;
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LUA
/*
 * "luaeval()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    ਮੇਲ_ਸਿਰਾ_2,	    // matchend()
    ਮੇਲ_ਮੇਲ,    // match()
    ਮੇਲ_STR,	    // matchstr()
    ਮੇਲ_LIST,	    // matchlist()
    ਮੇਲ_POS	    // matchstrpos()
} matchtype_T;

    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_some_ਮੇਲ(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = ਨਿਰਾ;
    ਲੰਮਾ	len = 0;
    char_u	*ਵਿਅੰਜਕ = ਨਿਰਾ;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    ਲੰਮਾ	ਸ਼ੁਰੂ = 0;
    ਲੰਮਾ	nth = 1;
    colnr_T	startcol = 0;
    ਪੂਰਨ		ਮੇਲ = 0;
    list_T	*l = ਨਿਰਾ;
    listitem_T	*li = ਨਿਰਾ;
    ਲੰਮਾ	idx = 0;
    ਪੂਰਨ		prev_ਤਾਲਾ = 0;
    char_u	*tofree = ਨਿਰਾ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;

    rettv->vval.v_ਸੰਖਿਆ = -1;
    ਜੇ (type == ਮੇਲ_LIST || type == ਮੇਲ_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	ਜੇ (rettv_list_alloc(rettv) == FAIL)
	    ਜਾਓ theend;
	ਜੇ (type == ਮੇਲ_POS
		&& (list_append_string(rettv->vval.v_list,
					    (char_u *)"", 0) == FAIL
		    || list_append_ਸੰਖਿਆ(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_ਸੰਖਿਆ(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_ਸੰਖਿਆ(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL))
	{
		list_free(rettv->vval.v_list);
		rettv->vval.v_list = ਨਿਰਾ;
		ਜਾਓ theend;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (type == ਮੇਲ_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ਨਿਰਾ;
    }

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 3) == FAIL)))
	ਜਾਓ theend;

    ਜੇ (argvars[0].v_type == VAR_LIST)
    {
	ਜੇ ((l = argvars[0].vval.v_list) == ਨਿਰਾ)
	    ਜਾਓ theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_ਪਹਿਲਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	ਵਿਅੰਜਕ = str = tv_get_string(&argvars[0]);
	len = (ਲੰਮਾ)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    ਜੇ (pat == ਨਿਰਾ)
	ਜਾਓ theend;

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ਪੂਰਨ	    error = FALSE;

	ਸ਼ੁਰੂ = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
	ਜੇ (error)
	    ਜਾਓ theend;
	ਜੇ (l != ਨਿਰਾ)
	{
	    li = list_ਲੱਭੋ(l, ਸ਼ੁਰੂ);
	    ਜੇ (li == ਨਿਰਾ)
		ਜਾਓ theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (ਸ਼ੁਰੂ < 0)
		ਸ਼ੁਰੂ = 0;
	    ਜੇ (ਸ਼ੁਰੂ > len)
		ਜਾਓ theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    ਜੇ (argvars[3].v_type != VAR_UNKNOWN)
		startcol = ਸ਼ੁਰੂ;
	    ਨਹੀਂਤਾਂ
	    {
		str += ਸ਼ੁਰੂ;
		len -= ਸ਼ੁਰੂ;
	    }
	}

	ਜੇ (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[3], &error);
	ਜੇ (error)
	    ਜਾਓ theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    ਜੇ (regmatch.regprog != ਨਿਰਾ)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	ਜੇ (l != ਨਿਰਾ)
	{
	    prev_ਤਾਲਾ = l->lv_ਤਾਲਾ;
	    ਜੇ (l->lv_ਤਾਲਾ == 0)
		l->lv_ਤਾਲਾ = VAR_LOCKED;
	}

	ਲਈ (;;)
	{
	    ਜੇ (l != ਨਿਰਾ)
	    {
		ਜੇ (li == ਨਿਰਾ)
		{
		    ਮੇਲ = FALSE;
		    ਰੋਕੋ;
		}
		vim_free(tofree);
		ਵਿਅੰਜਕ = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		ਜੇ (str == ਨਿਰਾ)
		    ਰੋਕੋ;
	    }

	    ਮੇਲ = vim_regexec_nl(&regmatch, str, startcol);

	    ਜੇ (ਮੇਲ && --nth <= 0)
		ਰੋਕੋ;
	    ਜੇ (l == ਨਿਰਾ && !ਮੇਲ)
		ਰੋਕੋ;

	    // Advance to just after the match.
	    ਜੇ (l != ਨਿਰਾ)
	    {
		li = li->li_ਅਗਲਾ;
		++idx;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		ਜੇ (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    ਮੇਲ = FALSE;
		    ਰੋਕੋ;
		}
	    }
	}

	ਜੇ (ਮੇਲ)
	{
	    ਜੇ (type == ਮੇਲ_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_ਪਹਿਲਾ;
		listitem_T *li2 = li1->li_ਅਗਲਾ;
		listitem_T *li3 = li2->li_ਅਗਲਾ;
		listitem_T *li4 = li3->li_ਅਗਲਾ;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_ਸੰਖਿਆ =
				      (varnumber_T)(regmatch.startp[0] - ਵਿਅੰਜਕ);
		li4->li_tv.vval.v_ਸੰਖਿਆ =
					(varnumber_T)(regmatch.endp[0] - ਵਿਅੰਜਕ);
		ਜੇ (l != ਨਿਰਾ)
		    li2->li_tv.vval.v_ਸੰਖਿਆ = (varnumber_T)idx;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (type == ਮੇਲ_LIST)
	    {
		ਪੂਰਨ i;

		// return list with matched string and submatches
		ਲਈ (i = 0; i < NSUBEXP; ++i)
		{
		    ਜੇ (regmatch.endp[i] == ਨਿਰਾ)
		    {
			ਜੇ (list_append_string(rettv->vval.v_list,
							    ਨਿਰਾ, 0) == FAIL)
			    ਰੋਕੋ;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(ਪੂਰਨ)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			ਰੋਕੋ;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (type == ਮੇਲ_STR)
	    {
		// return matched string
		ਜੇ (l != ਨਿਰਾ)
		    copy_tv(&li->li_tv, rettv);
		ਨਹੀਂਤਾਂ
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (l != ਨਿਰਾ)
		rettv->vval.v_ਸੰਖਿਆ = idx;
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (type != ਮੇਲ_ਸਿਰਾ_2)
		    rettv->vval.v_ਸੰਖਿਆ =
				      (varnumber_T)(regmatch.startp[0] - str);
		ਨਹੀਂਤਾਂ
		    rettv->vval.v_ਸੰਖਿਆ =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_ਸੰਖਿਆ += (varnumber_T)(str - ਵਿਅੰਜਕ);
	    }
	}
	ਜੇ (l != ਨਿਰਾ)
	    l->lv_ਤਾਲਾ = prev_ਤਾਲਾ;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    ਜੇ (type == ਮੇਲ_POS && l == ਨਿਰਾ && rettv->vval.v_list != ਨਿਰਾ)
	// matchstrpos() without a list: drop the second item.
	listitem_remove(rettv->vval.v_list,
				       rettv->vval.v_list->lv_ਪਹਿਲਾ->li_ਅਗਲਾ);
    vim_free(tofree);
    p_cpo = save_cpo;
}

/*
 * Return all the matches in string "str" for pattern "rmp".
 * The matches are returned in the List "mlist".
 * If "submatches" is TRUE, then submatch information is also returned.
 * "matchbuf" is TRUE when called for matchbufline().
 */
    ਸਥਿਰ ਪੂਰਨ
get_matches_ਵਿੱਚ_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    ਪੂਰਨ		idx,
    ਪੂਰਨ		submatches,
    ਪੂਰਨ		matchbuf)
{
    ਲੰਮਾ	len = (ਲੰਮਾ)STRLEN(str);
    ਪੂਰਨ		ਮੇਲ = 0;
    colnr_T	startidx = 0;

    ਲਈ (;;)
    {
	ਮੇਲ = vim_regexec_nl(rmp, str, startidx);
	ਜੇ (!ਮੇਲ)
	    ਰੋਕੋ;

	dict_T *d = dict_alloc();
	ਜੇ (d == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	ਜੇ (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (dict_ਜੋੜੋ_ਸੰਖਿਆ(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (dict_ਜੋੜੋ_string_len(d, "text", rmp->startp[0],
		    (ਪੂਰਨ)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (submatches)
	{
	    list_T *sml = list_alloc();
	    ਜੇ (sml == ਨਿਰਾ)
		ਵਾਪਸ FAIL;

	    ਜੇ (dict_ਜੋੜੋ_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		ਵਾਪਸ FAIL;
	    }

	    // return a list with the submatches
	    ਲਈ (ਪੂਰਨ i = 1; i < NSUBEXP; ++i)
	    {
		ਜੇ (rmp->endp[i] == ਨਿਰਾ)
		{
		    ਜੇ (list_append_string(sml, (char_u *)"", 0) == FAIL)
			ਵਾਪਸ FAIL;
		}
		ਨਹੀਂਤਾਂ ਜੇ (list_append_string(sml, rmp->startp[i],
			    (ਪੂਰਨ)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	ਜੇ (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    ਰੋਕੋ;
    }

    ਵਾਪਸ OK;
}

/*
 * "matchbufline()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ਨਿਰਾ;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_ਸੰਖਿਆ = -1;
    ਜੇ (rettv_list_alloc(rettv) != OK)
	ਵਾਪਸ;
    retlist = rettv->vval.v_list;

    ਜੇ (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	ਵਾਪਸ;

    ਪੂਰਨ prev_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    ਜੇ (buf == ਨਿਰਾ)
    {
	ਜੇ (did_emsg == prev_did_emsg)
	    semsg(_(e_invalid_ਬਫਰ_name_str), tv_get_string(&argvars[0]));
	ਵਾਪਸ;
    }
    ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)
    {
	emsg(_(e_ਬਫਰ_is_not_loaded));
	ਵਾਪਸ;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    ਪੂਰਨ		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    ਜੇ (did_emsg > did_emsg_before)
	ਵਾਪਸ;
    ਜੇ (slnum < 1)
    {
	semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "lnum");
	ਵਾਪਸ;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    ਜੇ (did_emsg > did_emsg_before)
	ਵਾਪਸ;
    ਜੇ (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "end_lnum");
	ਵਾਪਸ;
    }

    ਜੇ (elnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	elnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਪੂਰਨ		submatches = FALSE;
    ਜੇ (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	ਜੇ (d != ਨਿਰਾ)
	{
	    dictitem_T *di = dict_ਲੱਭੋ(d, (char_u *)"submatches", -1);
	    ਜੇ (di != ਨਿਰਾ)
	    {
		ਜੇ (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "submatches");
		    ਵਾਪਸ;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;

    regmatch.regprog = eval_regcomp(pat);
    ਜੇ (regmatch.regprog == ਨਿਰਾ)
	ਜਾਓ theend;
    regmatch.rm_ic = p_ic;

    ਜਦੋਂ (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	ਜੇ (get_matches_ਵਿੱਚ_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    ਜਾਓ cleanup;
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
    ਸਥਿਰ ਖਾਲੀ
f_ਮੇਲ(typval_T *argvars, typval_T *rettv)
{
    ਲੱਭੋ_some_ਮੇਲ(argvars, rettv, ਮੇਲ_ਮੇਲ);
}

/*
 * "matchend()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_matchend(typval_T *argvars, typval_T *rettv)
{
    ਲੱਭੋ_some_ਮੇਲ(argvars, rettv, ਮੇਲ_ਸਿਰਾ_2);
}

/*
 * "matchlist()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    ਲੱਭੋ_some_ਮੇਲ(argvars, rettv, ਮੇਲ_LIST);
}

/*
 * "matchstr()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    ਲੱਭੋ_some_ਮੇਲ(argvars, rettv, ਮੇਲ_STR);
}

/*
 * "matchstrlist()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ਨਿਰਾ;
    char_u	*save_cpo;
    list_T	*l = ਨਿਰਾ;
    listitem_T	*li = ਨਿਰਾ;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_ਸੰਖਿਆ = -1;
    ਜੇ (rettv_list_alloc(rettv) != OK)
	ਵਾਪਸ;
    retlist = rettv->vval.v_list;

    ਜੇ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ਵਾਪਸ;

    ਜੇ ((l = argvars[0].vval.v_list) == ਨਿਰਾ)
	ਵਾਪਸ;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    ਜੇ (pat == ਨਿਰਾ)
	ਵਾਪਸ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;

    regmatch.regprog = eval_regcomp(pat);
    ਜੇ (regmatch.regprog == ਨਿਰਾ)
	ਜਾਓ theend;
    regmatch.rm_ic = p_ic;

    ਪੂਰਨ		submatches = FALSE;
    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	ਜੇ (d != ਨਿਰਾ)
	{
	    dictitem_T *di = dict_ਲੱਭੋ(d, (char_u *)"submatches", -1);
	    ਜੇ (di != ਨਿਰਾ)
	    {
		ਜੇ (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "submatches");
		    ਜਾਓ cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    ਪੂਰਨ idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	ਜੇ (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != ਨਿਰਾ)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    ਜੇ (get_matches_ਵਿੱਚ_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		ਜਾਓ cleanup;
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
    ਸਥਿਰ ਖਾਲੀ
f_matchstrpos(typval_T *argvars, typval_T *rettv)
{
    ਲੱਭੋ_some_ਮੇਲ(argvars, rettv, ਮੇਲ_POS);
}

    ਸਥਿਰ ਖਾਲੀ
max_min(typval_T *argvars, typval_T *rettv, ਪੂਰਨ domax)
{
    varnumber_T	n = 0;

    ਜੇ (ਵਿੱਚ_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = 0;

    ਜੇ (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = ਨਿਰਾ;

	l = argvars[0].vval.v_list;
	ਜੇ (l != ਨਿਰਾ && l->lv_len > 0)
	{
	    ਜੇ (l->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ)
	    {
		ਜੇ ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_ਸ਼ੁਰੂ;
		ਨਹੀਂਤਾਂ
		    n = l->lv_u.nonmat.lv_ਸ਼ੁਰੂ + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_ਸੰਖਿਆ = n;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
		{
		    ਜੇ (tv == ਨਿਰਾ)
			tv = &li->li_tv;
		    ਨਹੀਂਤਾਂ
		    {
			ਪੂਰਨ res;
			ਜੇ (typval_compare2(&li->li_tv, tv,
				domax ? ਵਿਅੰਜਕ_GREATER : ਵਿਅੰਜਕ_SMALLER, FALSE, &res) == FAIL)
			    ਵਾਪਸ;
			ਜੇ (res == OK)
			    tv = &li->li_tv;
		    }
		}

		ਜੇ (tv != ਨਿਰਾ)
		    copy_tv(tv, rettv);
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	ਜੇ (tv != ਨਿਰਾ)
	    copy_tv(tv, rettv);
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	ਪੂਰਨ		todo;
	typval_T	*tv = ਨਿਰਾ;

	d = argvars[0].vval.v_dict;
	ਜੇ (d != ਨਿਰਾ)
	{
	    todo = (ਪੂਰਨ)d->dv_hashtab.ht_ਵਰਤਿਆ;
	    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&d->dv_hashtab, hi, todo)
	    {
		ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		{
		    --todo;
		    ਜੇ (tv == ਨਿਰਾ)
			tv = &HI2DI(hi)->di_tv;
		    ਨਹੀਂਤਾਂ
		    {
			ਪੂਰਨ res;
			ਜੇ (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? ਵਿਅੰਜਕ_GREATER : ਵਿਅੰਜਕ_SMALLER, FALSE, &res) == FAIL)
			    ਵਾਪਸ;
			ਜੇ (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	ਜੇ (tv != ਨਿਰਾ)
	    copy_tv(tv, rettv);
    }
    ਨਹੀਂਤਾਂ
	semsg(_(e_ਦਲੀਲ_ਦਾ_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
}

/*
 * "max()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_max(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, TRUE);
}

/*
 * "min()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_min(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, FALSE);
}

#ਜੇ defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    ਖਾਲੀ
mzscheme_call_vim(char_u *name, typval_T *ਦਲੀਲਾਂ, typval_T *rettv)
{
    typval_T argvars[3];

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = name;
    copy_tv(ਦਲੀਲਾਂ, &argvars[1]);
    argvars[2].v_type = VAR_UNKNOWN;
    f_call(argvars, rettv);
    clear_tv(&argvars[1]);
}
#ਜੇ_ਅੰਤ

/*
 * "nextnonblank()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਲਈ (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	ਜੇ (lnum < 0 || lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    lnum = 0;
	    ਰੋਕੋ;
	}
	ਜੇ (*skipwhite(ml_get(lnum)) != NUL)
	    ਰੋਕੋ;
    }
    rettv->vval.v_ਸੰਖਿਆ = lnum;
}


/*
 * "ngettext()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#ਜੇ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    ਅੱਖਰ *prev = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    ਜੇ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_ਸੰਖਿਆ_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_STRING;

    ਜੇ (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != ਨਿਰਾ &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#ਜੇ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ਅਚਲ ਅੱਖਰ *)argvars[3].vval.v_string, (ਅੱਖਰ *)p_enc);
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((ਅਚਲ ਅੱਖਰ *)argvars[3].vval.v_string, (ਅਚਲ ਅੱਖਰ *)argvars[0].vval.v_string, (ਅਚਲ ਅੱਖਰ *)argvars[1].vval.v_string, (ਪੂਰਨ)argvars[2].vval.v_ਸੰਖਿਆ));
#ਨਹੀਂਤਾਂ
	textdomain((ਅਚਲ ਅੱਖਰ *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ਅਚਲ ਅੱਖਰ *)argvars[0].vval.v_string, (ਅਚਲ ਅੱਖਰ *)argvars[1].vval.v_string, argvars[2].vval.v_ਸੰਖਿਆ));
	textdomain(VIMPACKAGE);
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	ਜੇ (prev != ਨਿਰਾ)
	    bind_textdomain_codeset((ਅਚਲ ਅੱਖਰ *)argvars[3].vval.v_string, prev);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ਅਚਲ ਅੱਖਰ *)argvars[0].vval.v_string, (ਅਚਲ ਅੱਖਰ *)argvars[1].vval.v_string, argvars[2].vval.v_ਸੰਖਿਆ));
}


/*
 * "nr2char()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (has_mbyte)
    {
	ਪੂਰਨ	utf8 = 0;

	ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (ਪੂਰਨ)tv_get_bool_chk(&argvars[1], ਨਿਰਾ);
	ਜੇ (utf8)
	    buf[utf_char2bytes((ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]), buf)] = NUL;
	ਨਹੀਂਤਾਂ
	    buf[(*mb_char2bytes)((ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]), buf)] = NUL;
    }
    ਨਹੀਂਤਾਂ
    {
	buf[0] = (char_u)tv_get_ਸੰਖਿਆ(&argvars[0]);
	buf[1] = NUL;
    }
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "or(expr, expr)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_or(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ)
					| tv_get_ਸੰਖਿਆ_chk(&argvars[1], ਨਿਰਾ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERL
/*
 * "perleval()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#ਜੇ_ਅੰਤ

/*
 * "prevnonblank()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    lnum = tv_get_lnum(argvars);
    ਜੇ (lnum < 1 || lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	lnum = 0;
    ਨਹੀਂਤਾਂ
	ਜਦੋਂ (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
	    --lnum;
    rettv->vval.v_ਸੰਖਿਆ = lnum;
}

// This dummy va_list is here because:
// - passing a NULL pointer doesn't work when va_list isn't a pointer
// - locally in the function results in a "used before set" warning
// - using va_start() to initialize it gives "function with fixed args" error
ਸਥਿਰ va_list	ap;

/*
 * "printf()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_printf(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    ਪੂਰਨ		len;
    char_u	*s;
    ਪੂਰਨ		saved_did_emsg = did_emsg;
    ਅੱਖਰ	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_or_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (ਅੱਖਰ *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(ਨਿਰਾ, 0, fmt, ap, argvars + 1);
    ਜੇ (!did_emsg)
    {
	s = alloc(len + 1);
	ਜੇ (s != ਨਿਰਾ)
	{
	    rettv->vval.v_string = s;
	    (ਖਾਲੀ)vim_vsnprintf_typval((ਅੱਖਰ *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_pum_getpos(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    pum_ਸੈੱਟ_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_pumvisible(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (pum_visible())
	rettv->vval.v_ਸੰਖਿਆ = 1;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (p_pyx == 0)
	p_pyx = 3;

    ਜੇ (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ
    {
	locals = ਨਿਰਾ;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
/*
 * "pyeval()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	ਵਾਪਸ;

    ਜੇ (p_pyx == 0)
	p_pyx = 2;

    ਜੇ (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ
    {
	locals = ਨਿਰਾ;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

# ਜੇ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ਜੇ (p_pyx == 2)
	f_pyeval(argvars, rettv);
    ਨਹੀਂਤਾਂ
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ UINT32_T srand_ਬੀਜ_for_testing = 0;
ਸਥਿਰ ਪੂਰਨ	srand_ਬੀਜ_for_testing_is_ਵਰਤਿਆ = FALSE;

    ਸਥਿਰ ਖਾਲੀ
f_test_srand_ਬੀਜ(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
	srand_ਬੀਜ_for_testing_is_ਵਰਤਿਆ = FALSE;
    ਨਹੀਂਤਾਂ
    {
	srand_ਬੀਜ_for_testing = (UINT32_T)tv_get_ਸੰਖਿਆ(&argvars[0]);
	srand_ਬੀਜ_for_testing_is_ਵਰਤਿਆ = TRUE;
    }
}

    ਸਥਿਰ ਖਾਲੀ
init_srand(UINT32_T *x)
{
    ਬਣਤਰ {
	ਸੰਘ {
	    UINT32_T ਸੰਖਿਆ;
	    char_u   ਬਾਈਟਾਂ[ਆਕਾਰ(UINT32_T)];
	} contents;
    } buf;

    ਜੇ (srand_ਬੀਜ_for_testing_is_ਵਰਤਿਆ)
    {
	*x = srand_ਬੀਜ_for_testing;
	ਵਾਪਸ;
    }

    ਜੇ (mch_get_random(buf.contents.ਬਾਈਟਾਂ, ਆਕਾਰ(buf.contents.ਬਾਈਟਾਂ)) == OK)
    {
	*x = buf.contents.ਸੰਖਿਆ;
	ਵਾਪਸ;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#ਜੇ defined(FEAT_SODIUM)
    ਜੇ (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
#ਜੇ defined(FEAT_RELTIME)
	proftime_T res;
	profile_ਸ਼ੁਰੂ(&res);
# ਜੇ defined(MSWIN)
	*x = (UINT32_T)res.ਨੀਵਾਂ_Part;
# ਨਹੀਂਤਾਂ
	*x = (UINT32_T)res.tv_fsec;
# endif
#ਨਹੀਂਤਾਂ
	*x = vim_ਸਮਾਂ();
#ਜੇ_ਅੰਤ
	*x ^= mch_get_pid();
    }
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ROTL(x, k) (((x) << (k)) | ((x) >> (32 - (k))))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPLITMIX32(x, z) ( \
    (z) = ((x) += 0x9e3779b9), \
    (z) = ((z) ^ ((z) >> 16)) * 0x85ebca6b, \
    (z) = ((z) ^ ((z) >> 13)) * 0xc2b2ae35, \
    (z) ^ ((z) >> 16) \
    )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w) \
    ਨਤੀਜਾ = ROTL((y) * 5, 7) * 9; \
    t = (y) << 9; \
    (z) ^= (x); \
    (w) ^= (y); \
    (y) ^= (z), (x) ^= (w); \
    (z) ^= t; \
    (w) = ROTL(w, 11);

/*
 * "rand()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = ਨਿਰਾ;
    ਸਥਿਰ UINT32_T	gx, gy, gz, gw;
    ਸਥਿਰ ਪੂਰਨ	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, ਨਤੀਜਾ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// When no argument is given use the global seed list.
	ਜੇ (initialized == FALSE)
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
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	ਜੇ (l == ਨਿਰਾ || list_len(l) != 4)
	    ਜਾਓ theend;

	lx = list_ਲੱਭੋ(l, 0L);
	ly = list_ਲੱਭੋ(l, 1L);
	lz = list_ਲੱਭੋ(l, 2L);
	lw = list_ਲੱਭੋ(l, 3L);
	ਜੇ (lx == ਨਿਰਾ || ly == ਨਿਰਾ || lz == ਨਿਰਾ || lw == ਨਿਰਾ)
	    ਜਾਓ theend;
	ਜੇ (lx->li_tv.v_type != VAR_ਸੰਖਿਆ
		|| ly->li_tv.v_type != VAR_ਸੰਖਿਆ
		|| lz->li_tv.v_type != VAR_ਸੰਖਿਆ
		|| lw->li_tv.v_type != VAR_ਸੰਖਿਆ)
	    ਜਾਓ theend;
	x = (UINT32_T)lx->li_tv.vval.v_ਸੰਖਿਆ;
	y = (UINT32_T)ly->li_tv.vval.v_ਸੰਖਿਆ;
	z = (UINT32_T)lz->li_tv.vval.v_ਸੰਖਿਆ;
	w = (UINT32_T)lw->li_tv.vval.v_ਸੰਖਿਆ;

	SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w);

	lx->li_tv.vval.v_ਸੰਖਿਆ = (varnumber_T)x;
	ly->li_tv.vval.v_ਸੰਖਿਆ = (varnumber_T)y;
	lz->li_tv.vval.v_ਸੰਖਿਆ = (varnumber_T)z;
	lw->li_tv.vval.v_ਸੰਖਿਆ = (varnumber_T)w;
    }
    ਨਹੀਂਤਾਂ
	ਜਾਓ theend;

    rettv->v_type = VAR_ਸੰਖਿਆ;
    rettv->vval.v_ਸੰਖਿਆ = (varnumber_T)ਨਤੀਜਾ;
    ਵਾਪਸ;

theend:
    semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(&argvars[0]));
    rettv->v_type = VAR_ਸੰਖਿਆ;
    rettv->vval.v_ਸੰਖਿਆ = -1;
}

/*
 * "srand()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_srand(typval_T *argvars, typval_T *rettv)
{
    UINT32_T x = 0, z;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	    error = FALSE;

	x = (UINT32_T)tv_get_ਸੰਖਿਆ_chk(&argvars[0], &error);
	ਜੇ (error)
	    ਵਾਪਸ;
    }

    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ROTL
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ SPLITMIX32
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	ਸ਼ੁਰੂ;
    varnumber_T	ਸਿਰਾ;
    varnumber_T	stride = 1;
    ਪੂਰਨ		error = FALSE;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    ਸ਼ੁਰੂ = tv_get_ਸੰਖਿਆ_chk(&argvars[0], &error);
    ਜੇ (argvars[1].v_type == VAR_UNKNOWN)
    {
	ਸਿਰਾ = ਸ਼ੁਰੂ - 1;
	ਸ਼ੁਰੂ = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	ਸਿਰਾ = tv_get_ਸੰਖਿਆ_chk(&argvars[1], &error);
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
    }

    ਜੇ (error)
	ਵਾਪਸ;		// type error; errmsg already given
    ਜੇ (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	ਵਾਪਸ;
    }

    // The stride is stored in "lv_stride", which is an int.
    ਜੇ (stride < INT_MIN || stride > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
    {
	ਅੱਖਰ	buf[NUMBUFLEN];

	vim_snprintf(buf, ਆਕਾਰ(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	ਵਾਪਸ;
    }

    uvarnumber_T	len;

    ਜੇ (stride > 0 ? ਸਿਰਾ < ਸ਼ੁਰੂ : ਸਿਰਾ > ਸ਼ੁਰੂ)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	ਪਿੱਠ = stride > 0
				  ? (uvarnumber_T)ਸ਼ੁਰੂ - (uvarnumber_T)ਸਿਰਾ
				  : (uvarnumber_T)ਸਿਰਾ - (uvarnumber_T)ਸ਼ੁਰੂ;

	ਜੇ (ਪਿੱਠ > 1)
	{
	    emsg(_(e_ਸ਼ੁਰੂ_past_ਸਿਰਾ));
	    ਵਾਪਸ;
	}
	len = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	ਵਿਸਤਾਰ = stride > 0
				  ? (uvarnumber_T)ਸਿਰਾ - (uvarnumber_T)ਸ਼ੁਰੂ
				  : (uvarnumber_T)ਸ਼ੁਰੂ - (uvarnumber_T)ਸਿਰਾ;
	uvarnumber_T	ਗਿਣਤੀ_2 = ਵਿਸਤਾਰ / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	ਜੇ (ਗਿਣਤੀ_2 >= (uvarnumber_T)ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
	{
	    ਅੱਖਰ	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, ਆਕਾਰ(buf), "%llu",
			      ਗਿਣਤੀ_2 < UVARNUM_MAX ? ਗਿਣਤੀ_2 + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    ਵਾਪਸ;
	}
	len = ਗਿਣਤੀ_2 + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_ਪਹਿਲਾ = &range_list_ਵਸਤੂ;
    list->lv_u.nonmat.lv_ਸ਼ੁਰੂ = ਸ਼ੁਰੂ;
    list->lv_u.nonmat.lv_ਸਿਰਾ = ਸਿਰਾ;
    list->lv_u.nonmat.lv_stride = (ਪੂਰਨ)stride;
    list->lv_len = (ਪੂਰਨ)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    ਖਾਲੀ
range_list_materialize(list_T *list)
{
    varnumber_T ਸ਼ੁਰੂ = list->lv_u.nonmat.lv_ਸ਼ੁਰੂ;
    varnumber_T ਸਿਰਾ = list->lv_u.nonmat.lv_ਸਿਰਾ;
    ਪੂਰਨ		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_ਪਹਿਲਾ = ਨਿਰਾ;
    list->lv_u.mat.lv_ਆਖਰੀ = ਨਿਰਾ;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_ਵਸਤੂ = ਨਿਰਾ;
    ਲਈ (i = ਸ਼ੁਰੂ; stride > 0 ? i <= ਸਿਰਾ : i >= ਸਿਰਾ; i += stride)
    {
	ਜੇ (list_append_ਸੰਖਿਆ(list, i) == FAIL)
	    ਰੋਕੋ;
	ਜੇ (list->lv_ਤਾਲਾ & VAR_ਚੀਜ਼ਾਂ_LOCKED)
	    list->lv_u.mat.lv_ਆਖਰੀ->li_tv.v_ਤਾਲਾ = VAR_LOCKED;
    }
    list->lv_ਤਾਲਾ &= ~VAR_ਚੀਜ਼ਾਂ_LOCKED;
}

/*
 * "getreginfo()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		regname;
    char_u	buf[NUMBUFLEN + 2];
    size_t	buflen;
    ਲੰਮਾ	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    regname = getreg_get_regname(argvars);
    ਜੇ (regname == 0)
	ਵਾਪਸ;

    ਜੇ (regname == '@')
	regname = '"';

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_ਵਿਅੰਜਕ_SRC | GREG_LIST);
    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (dict_ਜੋੜੋ_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    ਚੋਣ (get_reg_type(regname, &reglen))
    {
	ਹਾਲਤ MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    ਰੋਕੋ;
	ਹਾਲਤ MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    ਰੋਕੋ;
	ਹਾਲਤ MBLOCK:
	    buflen = vim_snprintf_safelen((ਅੱਖਰ *)buf, ਆਕਾਰ(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    ਰੋਕੋ;
	ਮੂਲ:
	    buf[0] = NUL;
	    buflen = 0;
	    ਰੋਕੋ;
    }
    (ਖਾਲੀ)dict_ਜੋੜੋ_string_len(dict, (ਅੱਖਰ *)"regtype", buf, (ਪੂਰਨ)buflen);

    buf[0] = get_ਰਜਿਸਟਰ_name(get_unname_ਰਜਿਸਟਰ());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    ਜੇ (regname == '"')
	(ਖਾਲੀ)dict_ਜੋੜੋ_string_len(dict, (ਅੱਖਰ *)"points_to", buf, (ਪੂਰਨ)buflen);
    ਨਹੀਂਤਾਂ
    {
	dictitem_T	*ਵਸਤੂ = dictitem_alloc((char_u *)"isunnamed");

	ਜੇ (ਵਸਤੂ != ਨਿਰਾ)
	{
	    ਵਸਤੂ->di_tv.v_type = VAR_BOOL;
	    ਵਸਤੂ->di_tv.vval.v_ਸੰਖਿਆ = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (ਖਾਲੀ)dict_ਜੋੜੋ(dict, ਵਸਤੂ);
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
return_ਰਜਿਸਟਰ(ਪੂਰਨ regname, typval_T *rettv)
{
    char_u buf[2] = {NUL, NUL};

    buf[0] = (char_u)regname;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(buf, (buf[0] == NUL) ? 0 : 1);
}

/*
 * "reg_executing()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_reg_executing(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    return_ਰਜਿਸਟਰ(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_reg_recording(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    return_ਰਜਿਸਟਰ(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_ਸੰਖਿਆ = -1;
    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    ਸਥਿਰ ਖਾਲੀ
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    ਪੂਰਨ slen = list_len(l);

    ਜੇ (rettv_list_alloc(rettv) == FAIL
	    || l == ਨਿਰਾ
	    || n <= 0
	    || slen == 0)
	ਵਾਪਸ;

    ਜੇ (check_repeat_ਗਿਣਤੀ(slen, n) == FAIL)
	ਵਾਪਸ;

    ਜਦੋਂ (n-- > 0)
	ਜੇ (list_extend(rettv->vval.v_list, l, ਨਿਰਾ) == FAIL)
	    ਰੋਕੋ;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    ਸਥਿਰ ਖਾਲੀ
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    ਪੂਰਨ		slen;
    ਪੂਰਨ		len;
    ਪੂਰਨ		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    ਜੇ (rettv_blob_alloc(rettv) == FAIL
	    || blob == ਨਿਰਾ
	    || n <= 0)
	ਵਾਪਸ;

    slen = blob->bv_ga.ga_len;
    ਜੇ (slen <= 0)
	ਵਾਪਸ;
    ਜੇ (check_repeat_ਗਿਣਤੀ(slen, n) == FAIL)
	ਵਾਪਸ;
    len = slen * (ਪੂਰਨ)n;

    ਜੇ (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    ਲਈ (i = 0; i < slen; ++i)
	ਜੇ (blob_get(blob, i) != 0)
	    ਰੋਕੋ;

    ਜੇ (i == slen)
	// No need to copy since all bytes are already zero
	ਵਾਪਸ;

    ਲਈ (i = 0; i < (ਪੂਰਨ)n; ++i)
	blob_ਸੈੱਟ_range(rettv->vval.v_blob,
		(ਲੰਮਾ)i * slen, ((ਲੰਮਾ)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    ਸਥਿਰ ਖਾਲੀ
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    size_t	slen;
    ਪੂਰਨ		len;
    char_u	*r;
    ਪੂਰਨ		ਪੂਰਾ;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    slen = STRLEN(p);
    ਜੇ (slen == 0 || n <= 0)
	ਵਾਪਸ;
    ਜੇ (check_repeat_ਗਿਣਤੀ((varnumber_T)slen, n) == FAIL)
	ਵਾਪਸ;
    len = (ਪੂਰਨ)slen * (ਪੂਰਨ)n;

    r = alloc((size_t)len + 1);
    ਜੇ (r == ਨਿਰਾ)
	ਵਾਪਸ;

    mch_memmove(r, p, slen);
    ਪੂਰਾ = (ਪੂਰਨ)slen;
    ਜਦੋਂ (ਪੂਰਾ < len)
    {
	ਪੂਰਨ copy_len = ਪੂਰਾ;

	ਜੇ (copy_len > len - ਪੂਰਾ)
	    copy_len = len - ਪੂਰਾ;
	mch_memmove(r + ਪੂਰਾ, r, (size_t)copy_len);
	ਪੂਰਾ += copy_len;
    }
    r[len] = NUL;

    rettv->vval.v_string = r;
}

/*
 * "repeat()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    n = tv_get_ਸੰਖਿਆ(&argvars[1]);
    ਜੇ (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    ਨਹੀਂਤਾਂ
	repeat_string(&argvars[0], n, rettv);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_NOMOVE	0x01	    // don't move cursor
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_REPEAT	0x02	    // repeat to find outer pair
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_RETCOUNT	0x04	    // return matchcount
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_SETPCMARK	0x08	    // set previous context mark
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_ਸ਼ੁਰੂ	0x10	    // accept match at start position
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_SUBPAT	0x20	    // return nr of matching sub-pattern
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_ਸਿਰਾ		0x40	    // leave cursor at end of match
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_ਕਾਲਮ	0x80	    // start at cursor column

/*
 * Get flags for a search function.
 * Possibly sets "p_ws".
 * Returns BACKWARD, FORWARD or zero (for an error).
 */
    ਸਥਿਰ ਪੂਰਨ
get_ਖੋਜ_arg(typval_T *varp, ਪੂਰਨ *flagsp)
{
    ਪੂਰਨ		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    ਪੂਰਨ		ਨਕਾਬ;

    ਜੇ (varp->v_type == VAR_UNKNOWN)
	ਵਾਪਸ FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    ਜੇ (flags == ਨਿਰਾ)
	ਵਾਪਸ 0;		// type error; errmsg already given
    ਜਦੋਂ (*flags != NUL)
    {
	ਚੋਣ (*flags)
	{
	    ਹਾਲਤ 'b': dir = BACKWARD; ਰੋਕੋ;
	    ਹਾਲਤ 'w': p_ws = TRUE; ਰੋਕੋ;
	    ਹਾਲਤ 'W': p_ws = FALSE; ਰੋਕੋ;
	    ਮੂਲ:  ਨਕਾਬ = 0;
		      ਜੇ (flagsp != ਨਿਰਾ)
			  ਚੋਣ (*flags)
			  {
			      ਹਾਲਤ 'c': ਨਕਾਬ = SP_ਸ਼ੁਰੂ; ਰੋਕੋ;
			      ਹਾਲਤ 'e': ਨਕਾਬ = SP_ਸਿਰਾ; ਰੋਕੋ;
			      ਹਾਲਤ 'm': ਨਕਾਬ = SP_RETCOUNT; ਰੋਕੋ;
			      ਹਾਲਤ 'n': ਨਕਾਬ = SP_NOMOVE; ਰੋਕੋ;
			      ਹਾਲਤ 'p': ਨਕਾਬ = SP_SUBPAT; ਰੋਕੋ;
			      ਹਾਲਤ 'r': ਨਕਾਬ = SP_REPEAT; ਰੋਕੋ;
			      ਹਾਲਤ 's': ਨਕਾਬ = SP_SETPCMARK; ਰੋਕੋ;
			      ਹਾਲਤ 'z': ਨਕਾਬ = SP_ਕਾਲਮ; ਰੋਕੋ;
			  }
		      ਜੇ (ਨਕਾਬ == 0)
		      {
			  semsg(_(e_invalid_ਦਲੀਲ_str), flags);
			  dir = 0;
		      }
		      ਨਹੀਂਤਾਂ
			  *flagsp |= ਨਕਾਬ;
	}
	ਜੇ (dir == 0)
	    ਰੋਕੋ;
	++flags;
    }
    ਵਾਪਸ dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    ਸਥਿਰ ਪੂਰਨ
ਖੋਜ_cmn(typval_T *argvars, pos_T *ਮੇਲ_pos, ਪੂਰਨ *flagsp)
{
    ਪੂਰਨ		flags;
    char_u	*pat;
    size_t	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    ਪੂਰਨ		save_p_ws = p_ws;
    ਪੂਰਨ		dir;
    ਪੂਰਨ		retval = 0;	// default: FAIL
    ਲੰਮਾ	lnum_stop = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    ਲੰਮਾ	ਸਮਾਂ_ਹੱਦਬੰਦੀ = 0;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		options = ਖੋਜ_KEEP;
    ਪੂਰਨ		subpatnum;
    searchit_arg_T sia;
    ਪੂਰਨ		use_skip = FALSE;
    pos_T	firstpos;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 3) == FAIL)))))
	ਜਾਓ theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_ਖੋਜ_arg(&argvars[1], flagsp);	// may set p_ws
    ਜੇ (dir == 0)
	ਜਾਓ theend;
    flags = *flagsp;
    ਜੇ (flags & SP_ਸ਼ੁਰੂ)
	options |= ਖੋਜ_ਸ਼ੁਰੂ;
    ਜੇ (flags & SP_ਸਿਰਾ)
	options |= ਖੋਜ_ਸਿਰਾ;
    ਜੇ (flags & SP_ਕਾਲਮ)
	options |= ਖੋਜ_ਖਾਨਾ;

    // Optional arguments: line number to stop searching, timeout and skip.
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], ਨਿਰਾ);
	ਜੇ (lnum_stop < 0)
	    ਜਾਓ theend;
	ਜੇ (argvars[3].v_type != VAR_UNKNOWN)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
	    ਸਮਾਂ_ਹੱਦਬੰਦੀ = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[3], ਨਿਰਾ);
	    ਜੇ (ਸਮਾਂ_ਹੱਦਬੰਦੀ < 0)
		ਜਾਓ theend;
#ਜੇ_ਅੰਤ
	    use_skip = eval_ਵਿਅੰਜਕ_valid_arg(&argvars[4]);
	}
    }

    /*
     * This function does not accept SP_REPEAT and SP_RETCOUNT flags.
     * Check to make sure only those flags are set.
     * Also, Only the SP_NOMOVE or the SP_SETPCMARK flag can be set. Both
     * flags cannot be set. Check for that condition also.
     */
    ਜੇ (((flags & (SP_REPEAT | SP_RETCOUNT)) != 0)
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(&argvars[1]));
	ਜਾਓ theend;
    }

    pos = save_cursor = curwin->w_cursor;
    CLEAR_ਖੇਤਰ(firstpos);
    CLEAR_ਖੇਤਰ(sia);
    sia.sa_stop_lnum = (linenr_T)lnum_stop;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    sia.sa_tm = ਸਮਾਂ_ਹੱਦਬੰਦੀ;
#ਜੇ_ਅੰਤ

    patlen = STRLEN(pat);

    // Repeat until {skip} returns FALSE.
    ਲਈ (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, ਨਿਰਾ, dir, pat, patlen, 1L,
						     options, RE_ਖੋਜ, &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	ਜੇ (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	ਜੇ (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    ਰੋਕੋ;
	ਜੇ (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    ਪੂਰਨ	    do_skip;
	    ਪੂਰਨ	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_ਵਿਅੰਜਕ_ਮੰਜ਼ਿਲ_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    ਜੇ (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		ਰੋਕੋ;
	    }
	    ਜੇ (!do_skip)
		ਰੋਕੋ;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~ਖੋਜ_ਸ਼ੁਰੂ;
    }

    ਜੇ (subpatnum != FAIL)
    {
	ਜੇ (flags & SP_SUBPAT)
	    retval = subpatnum;
	ਨਹੀਂਤਾਂ
	    retval = pos.lnum;
	ਜੇ (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	ਜੇ (ਮੇਲ_pos != ਨਿਰਾ)
	{
	    // Store the match cursor position
	    ਮੇਲ_pos->lnum = pos.lnum;
	    ਮੇਲ_pos->ਖਾਨਾ = pos.ਖਾਨਾ + 1;
	}
	// "/$" will put the cursor after the end of the line, may need to
	// correct that here
	check_cursor();
    }

    // If 'n' flag is used: restore cursor position.
    ਜੇ (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    ਨਹੀਂਤਾਂ
	curwin->w_ਸੈੱਟ_curswant = true;
theend:
    p_ws = save_p_ws;

    ਵਾਪਸ retval;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RUBY
/*
 * "rubyeval()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#ਜੇ_ਅੰਤ

/*
 * "screenattr()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		c;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਪੰਕਤੀ = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ) - 1;
    ਖਾਨਾ = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[1], ਨਿਰਾ) - 1;
    ਜੇ (ਪੰਕਤੀ < 0 || ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ
	    || ਖਾਨਾ < 0 || ਖਾਨਾ >= screen_Columns)
	c = -1;
    ਨਹੀਂਤਾਂ
	c = ScreenAttrs[ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + ਖਾਨਾ];
    rettv->vval.v_ਸੰਖਿਆ = c;
}

/*
 * "screenchar()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		c;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਪੰਕਤੀ = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ) - 1;
    ਖਾਨਾ = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[1], ਨਿਰਾ) - 1;
    ਜੇ (ਪੰਕਤੀ < 0 || ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ || ਖਾਨਾ < 0 || ਖਾਨਾ >= screen_Columns)
	c = -1;
    ਨਹੀਂਤਾਂ
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(ਪੰਕਤੀ, ਖਾਨਾ, buf, ਨਿਰਾ);
	c = (*mb_ptr2char)(buf);
    }
    rettv->vval.v_ਸੰਖਿਆ = c;
}

/*
 * "screenchars()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_screenchars(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		c;
    ਪੂਰਨ		i;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਪੰਕਤੀ = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ) - 1;
    ਖਾਨਾ = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[1], ਨਿਰਾ) - 1;
    ਜੇ (ਪੰਕਤੀ < 0 || ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ || ਖਾਨਾ < 0 || ਖਾਨਾ >= screen_Columns)
	ਵਾਪਸ;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(ਪੰਕਤੀ, ਖਾਨਾ, buf, ਨਿਰਾ);
    ਪੂਰਨ pcc[MAX_MCO];
    ਜੇ (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    ਨਹੀਂਤਾਂ
	c = (*mb_ptr2char)(buf);
    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)c);

    ਜੇ (enc_utf8)
	ਲਈ (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    ਸਥਿਰ ਖਾਲੀ
f_screencol(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_screenrow(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = ਨਿਰਾ;
    rettv->v_type = VAR_STRING;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਪੰਕਤੀ = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ) - 1;
    ਖਾਨਾ = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[1], ਨਿਰਾ) - 1;
    ਜੇ (ਪੰਕਤੀ < 0 || ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ || ਖਾਨਾ < 0 || ਖਾਨਾ >= screen_Columns)
	ਵਾਪਸ;

    screen_getbytes(ਪੰਕਤੀ, ਖਾਨਾ, buf, ਨਿਰਾ);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_ਖੋਜ(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		flags = 0;

    rettv->vval.v_ਸੰਖਿਆ = ਖੋਜ_cmn(argvars, ਨਿਰਾ, &flags);
}

/*
 * "searchdecl()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		locally = TRUE;
    ਪੂਰਨ		thisblock = FALSE;
    ਪੂਰਨ		error = FALSE;
    char_u	*name;

    rettv->vval.v_ਸੰਖਿਆ = 1;	// default: FAIL

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    name = tv_get_string_chk(&argvars[0]);
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(ਪੂਰਨ)tv_get_bool_chk(&argvars[1], &error);
	ਜੇ (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (ਪੂਰਨ)tv_get_bool_chk(&argvars[2], &error);
    }
    ਜੇ (!error && name != ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = ਲੱਭੋ_decl(name, (ਪੂਰਨ)STRLEN(name),
				     locally, thisblock, ਖੋਜ_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    ਸਥਿਰ ਪੂਰਨ
searchpair_cmn(typval_T *argvars, pos_T *ਮੇਲ_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    ਪੂਰਨ		save_p_ws = p_ws;
    ਪੂਰਨ		dir;
    ਪੂਰਨ		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    ਪੂਰਨ		retval = 0;		// default: FAIL
    ਲੰਮਾ	lnum_stop = 0;
    ਲੰਮਾ	ਸਮਾਂ_ਹੱਦਬੰਦੀ = 0;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_ਸੰਖਿਆ_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 6) == FAIL)))))
	ਜਾਓ theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    ਜੇ (spat == ਨਿਰਾ || mpat == ਨਿਰਾ || epat == ਨਿਰਾ)
	ਜਾਓ theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_ਖੋਜ_arg(&argvars[3], &flags); // may set p_ws
    ਜੇ (dir == 0)
	ਜਾਓ theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    ਜੇ ((flags & (SP_ਸਿਰਾ | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(&argvars[3]));
	ਜਾਓ theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    ਜੇ (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    ਜੇ (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	// Type is checked later.
	skip = &argvars[4];

	ਜੇ (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[5], ਨਿਰਾ);
	    ਜੇ (lnum_stop < 0)
	    {
		semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(&argvars[5]));
		ਜਾਓ theend;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
	    ਜੇ (argvars[6].v_type != VAR_UNKNOWN)
	    {
		ਸਮਾਂ_ਹੱਦਬੰਦੀ = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[6], ਨਿਰਾ);
		ਜੇ (ਸਮਾਂ_ਹੱਦਬੰਦੀ < 0)
		{
		    semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(&argvars[6]));
		    ਜਾਓ theend;
		}
	    }
#ਜੇ_ਅੰਤ
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    ਮੇਲ_pos, lnum_stop, ਸਮਾਂ_ਹੱਦਬੰਦੀ);

theend:
    p_ws = save_p_ws;

    ਵਾਪਸ retval;
}

/*
 * "searchpair()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = searchpair_cmn(argvars, ਨਿਰਾ);
}

/*
 * "searchpairpos()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	ਮੇਲ_pos;
    ਪੂਰਨ		lnum = 0;
    ਪੂਰਨ		ਖਾਨਾ = 0;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (searchpair_cmn(argvars, &ਮੇਲ_pos) > 0)
    {
	lnum = ਮੇਲ_pos.lnum;
	ਖਾਨਾ = ਮੇਲ_pos.ਖਾਨਾ;
    }

    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)ਖਾਨਾ);
}

/*
 * Search for a start/middle/end thing.
 * Used by searchpair(), see its documentation for the details.
 * Returns 0 or -1 for no match,
 */
    ਲੰਮਾ
do_searchpair(
    char_u	*spat,	    // start pattern
    char_u	*mpat,	    // middle pattern
    char_u	*epat,	    // end pattern
    ਪੂਰਨ		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    ਪੂਰਨ		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*ਮੇਲ_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    ਲੰਮਾ	ਸਮਾਂ_ਹੱਦਬੰਦੀ ਅਣਵਰਤਿਆ) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = ਨਿਰਾ, *pat3 = ਨਿਰਾ;
    size_t	patlen;
    size_t	spatlen;
    size_t	epatlen;
    size_t	pat2size;
    size_t	pat2len;
    size_t	pat3size;
    size_t	pat3len;
    ਲੰਮਾ	retval = 0;
    pos_T	pos;
    pos_T	firstpos;
    pos_T	foundpos;
    pos_T	save_cursor;
    pos_T	save_pos;
    ਪੂਰਨ		n;
    ਪੂਰਨ		r;
    ਪੂਰਨ		nest = 1;
    ਪੂਰਨ		use_skip = FALSE;
    ਪੂਰਨ		err;
    ਪੂਰਨ		options = ਖੋਜ_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    ਜੇ (pat2 == ਨਿਰਾ)
	ਜਾਓ theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    ਜੇ (pat3 == ਨਿਰਾ)
	ਜਾਓ theend;
    pat2len = vim_snprintf((ਅੱਖਰ *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    ਜੇ (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    ਨਹੀਂਤਾਂ
	pat3len = vim_snprintf((ਅੱਖਰ *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    ਜੇ (flags & SP_ਸ਼ੁਰੂ)
	options |= ਖੋਜ_ਸ਼ੁਰੂ;

    ਜੇ (skip != ਨਿਰਾ)
	use_skip = eval_ਵਿਅੰਜਕ_valid_arg(skip);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    ਜੇ (ਸਮਾਂ_ਹੱਦਬੰਦੀ > 0)
	init_regexp_timeout(ਸਮਾਂ_ਹੱਦਬੰਦੀ);
#ਜੇ_ਅੰਤ
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    ਲਈ (;;)
    {
	searchit_arg_T sia;

	CLEAR_ਖੇਤਰ(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, ਨਿਰਾ, dir, pat, patlen, 1L,
						     options, RE_ਖੋਜ, &sia);
	ਜੇ (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    ਰੋਕੋ;

	ਜੇ (firstpos.lnum == 0)
	    firstpos = pos;
	ਜੇ (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    ਜੇ (dir == BACKWARD)
		decl(&pos);
	    ਨਹੀਂਤਾਂ
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~ਖੋਜ_ਸ਼ੁਰੂ;

	// If the skip pattern matches, ignore this match.
	ਜੇ (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_ਵਿਅੰਜਕ_ਮੰਜ਼ਿਲ_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    ਜੇ (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		ਰੋਕੋ;
	    }
	    ਜੇ (r)
		ਜਾਰੀ;
	}

	ਜੇ ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	ਨਹੀਂਤਾਂ
	{
	    // Found end when searching forward or start when searching
	    // backward: end of (nested) pair; or found middle in outer pair.
	    ਜੇ (--nest == 1)
		pat = pat3;	// outer level, search for middle
	}

	ਜੇ (nest == 0)
	{
	    // Found the match: return matchcount or line number.
	    ਜੇ (flags & SP_RETCOUNT)
		++retval;
	    ਨਹੀਂਤਾਂ
		retval = pos.lnum;
	    ਜੇ (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    ਜੇ (!(flags & SP_REPEAT))
		ਰੋਕੋ;
	    nest = 1;	    // search for next unmatched
	}
    }

    ਜੇ (ਮੇਲ_pos != ਨਿਰਾ)
    {
	// Store the match cursor position
	ਮੇਲ_pos->lnum = curwin->w_cursor.lnum;
	ਮੇਲ_pos->ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    ਜੇ ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    ਜੇ (ਸਮਾਂ_ਹੱਦਬੰਦੀ > 0)
	disable_regexp_timeout();
#ਜੇ_ਅੰਤ
    vim_free(pat2);
    vim_free(pat3);
    ਜੇ (p_cpo == ਖਾਲੀ_ਥਾਵਾਂ_option)
	p_cpo = save_cpo;
    ਨਹੀਂਤਾਂ
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	ਜੇ (*p_cpo == NUL)
	    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    ਵਾਪਸ retval;
}

/*
 * "searchpos()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	ਮੇਲ_pos;
    ਪੂਰਨ		lnum = 0;
    ਪੂਰਨ		ਖਾਨਾ = 0;
    ਪੂਰਨ		n;
    ਪੂਰਨ		flags = 0;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    n = ਖੋਜ_cmn(argvars, &ਮੇਲ_pos, &flags);
    ਜੇ (n > 0)
    {
	lnum = ਮੇਲ_pos.lnum;
	ਖਾਨਾ = ਮੇਲ_pos.ਖਾਨਾ;
    }

    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)ਖਾਨਾ);
    ਜੇ (flags & SP_SUBPAT)
	list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_position(typval_T *argvars, typval_T *rettv, ਪੂਰਨ charpos)
{
    pos_T	pos;
    ਪੂਰਨ		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    name = tv_get_string_chk(argvars);
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	ਵਾਪਸ;

    ਜੇ (pos.ਖਾਨਾ != MAXCOL && --pos.ਖਾਨਾ < 0)
	pos.ਖਾਨਾ = 0;
    ਜੇ ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	ਜੇ (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_ਸੈੱਟ_curswant = false;
	}
	check_cursor();
	rettv->vval.v_ਸੰਖਿਆ = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	ਜੇ (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_ਸੰਖਿਆ = 0;
    }
    ਨਹੀਂਤਾਂ
	emsg(_(e_invalid_ਦਲੀਲ));
}
/*
 * "setcharpos()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    ਸੈੱਟ_position(argvars, rettv, TRUE);
}

    ਸਥਿਰ ਖਾਲੀ
f_setcharsearch(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    ਜੇ (check_for_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ ((d = argvars[0].vval.v_dict) == ਨਿਰਾ)
	ਵਾਪਸ;

    csearch = dict_get_string(d, "char", FALSE);
    ਜੇ (csearch != ਨਿਰਾ)
    {
	ਜੇ (enc_utf8)
	{
	    ਪੂਰਨ pcc[MAX_MCO];
	    ਪੂਰਨ c = utfc_ptr2char(csearch, pcc);

	    ਸੈੱਟ_ਆਖਰੀ_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	ਨਹੀਂਤਾਂ
	    ਸੈੱਟ_ਆਖਰੀ_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_ਲੱਭੋ(d, (char_u *)"forward", -1);
    ਜੇ (di != ਨਿਰਾ)
	ਸੈੱਟ_csearch_direction((ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_ਲੱਭੋ(d, (char_u *)"until", -1);
    ਜੇ (di != ਨਿਰਾ)
	ਸੈੱਟ_csearch_until(!!tv_get_ਸੰਖਿਆ(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    ਸੈੱਟ_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_setenv(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    name = tv_get_string_buf(&argvars[0], namebuf);
    ਜੇ (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_ਸੰਖਿਆ == VVAL_NULL)
	vim_unsetenv_ext(name);
    ਨਹੀਂਤਾਂ
	vim_setenv_ext(name, tv_get_string_buf(&argvars[1], valbuf));
}

/*
 * "setfperm({fname}, {mode})" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_setfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	modebuf[NUMBUFLEN];
    char_u	*ਬਹੁਲਕ_str;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਨਕਾਬ;
    ਪੂਰਨ		ਬਹੁਲਕ = 0;

    rettv->vval.v_ਸੰਖਿਆ = 0;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    fname = tv_get_string_chk(&argvars[0]);
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ;
    ਬਹੁਲਕ_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    ਜੇ (ਬਹੁਲਕ_str == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (STRLEN(ਬਹੁਲਕ_str) != 9)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), ਬਹੁਲਕ_str);
	ਵਾਪਸ;
    }

    ਨਕਾਬ = 1;
    ਲਈ (i = 8; i >= 0; --i)
    {
	ਜੇ (ਬਹੁਲਕ_str[i] != '-')
	    ਬਹੁਲਕ |= ਨਕਾਬ;
	ਨਕਾਬ = ਨਕਾਬ << 1;
    }
    rettv->vval.v_ਸੰਖਿਆ = mch_setperm(fname, ਬਹੁਲਕ) == OK;
}

/*
 * "setpos()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_setpos(typval_T *argvars, typval_T *rettv)
{
    ਸੈੱਟ_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    ਪੂਰਨ
get_yank_type(char_u **pp, char_u *yank_type, ਲੰਮਾ *ਖੰਡ_len)
{
    char_u *stropt = *pp;
    ਚੋਣ (*stropt)
    {
	ਹਾਲਤ 'v': ਹਾਲਤ 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    ਰੋਕੋ;
	ਹਾਲਤ 'V': ਹਾਲਤ 'l':	// line-wise selection
	    *yank_type = MLINE;
	    ਰੋਕੋ;
	ਹਾਲਤ 'b': ਹਾਲਤ Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    ਜੇ (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*ਖੰਡ_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਵਾਪਸ FAIL;
    }
    *pp = stropt;
    ਵਾਪਸ OK;
}

/*
 * "setreg()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_setreg(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    ਪੂਰਨ		append;
    char_u	yank_type;
    ਲੰਮਾ	ਖੰਡ_len;
    typval_T	*regcontents;
    ਪੂਰਨ		pointreg;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    pointreg = 0;
    regcontents = ਨਿਰਾ;
    ਖੰਡ_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_ਸੰਖਿਆ = 1;		// FAIL is default

    ਜੇ (strregname == ਨਿਰਾ)
	ਵਾਪਸ;		// type error; errmsg already given
    ਜੇ (ਵਿੱਚ_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_ਰਜਿਸਟਰ_name_must_be_one_char_str), strregname);
	ਵਾਪਸ;
    }
    regname = *strregname;
    ਜੇ (regname == 0 || regname == '@')
	regname = '"';

    ਜੇ (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	ਜੇ (d == ਨਿਰਾ || d->dv_hashtab.ht_ਵਰਤਿਆ == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {ਨਿਰਾ, ਨਿਰਾ};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    ਵਾਪਸ;
	}

	di = dict_ਲੱਭੋ(d, (char_u *)"regcontents", -1);
	ਜੇ (di != ਨਿਰਾ)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	ਜੇ (stropt != ਨਿਰਾ)
	{
	    ਪੂਰਨ ret = get_yank_type(&stropt, &yank_type, &ਖੰਡ_len);

	    ਜੇ (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "value");
		ਵਾਪਸ;
	    }
	}

	ਜੇ (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    ਜੇ (stropt != ਨਿਰਾ)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    ਨਹੀਂਤਾਂ
	regcontents = &argvars[1];

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ਜੇ (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    ਵਾਪਸ;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	ਜੇ (stropt == ਨਿਰਾ)
	    ਵਾਪਸ;		// type error
	ਲਈ (; *stropt != NUL; ++stropt)
	    ਚੋਣ (*stropt)
	    {
		ਹਾਲਤ 'a': ਹਾਲਤ 'A':	// append
		    append = TRUE;
		    ਰੋਕੋ;
		ਮੂਲ:
		    get_yank_type(&stropt, &yank_type, &ਖੰਡ_len);
	    }
    }

    ਜੇ (regcontents && regcontents->v_type == VAR_LIST)
    {
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	list_T		*ll = regcontents->vval.v_list;
	listitem_T	*li;
	ਪੂਰਨ		len;

	// If the list is NULL handle like an empty list.
	len = ll == ਨਿਰਾ ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	ਜੇ (lstval == ਨਿਰਾ)
	    ਵਾਪਸ;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	ਜੇ (ll != ਨਿਰਾ)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		ਜੇ (strval == ਨਿਰਾ)
		    ਜਾਓ free_lstval;
		ਜੇ (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    ਜੇ (strval == ਨਿਰਾ)
			ਜਾਓ free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ਨਿਰਾ;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, ਖੰਡ_len);
free_lstval:
	ਜਦੋਂ (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    ਨਹੀਂਤਾਂ ਜੇ (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	ਜੇ (strval == ਨਿਰਾ)
	    ਵਾਪਸ;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, ਖੰਡ_len);
    }
    ਜੇ (pointreg != 0)
	get_yank_ਰਜਿਸਟਰ(pointreg, TRUE);

    rettv->vval.v_ਸੰਖਿਆ = 0;
}

/*
 * "settagstack()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_settagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    dict_T	*d;
    ਪੂਰਨ		action = 'r';

    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    // first argument: window number or id
    wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;

    // second argument: dict with items to set in the tag stack
    ਜੇ (check_for_dict_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;
    d = argvars[1].vval.v_dict;
    ਜੇ (d == ਨਿਰਾ)
	ਵਾਪਸ;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    ਜੇ (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    ਨਹੀਂਤਾਂ ਜੇ (check_for_string_arg(argvars, 2) == FAIL)
	ਵਾਪਸ;
    ਨਹੀਂਤਾਂ
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	ਜੇ (actstr == ਨਿਰਾ)
	    ਵਾਪਸ;
	ਜੇ ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	ਨਹੀਂਤਾਂ
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    ਵਾਪਸ;
	}
    }

    ਜੇ (ਸੈੱਟ_tagstack(wp, d, action) == OK)
	rettv->vval.v_ਸੰਖਿਆ = 0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ਪੂਰਨ		len;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != ਨਿਰਾ ? (char_u *)blob->bv_ga.ga_ਅੰਕੜੇ : (char_u *)"";
	len = blob != ਨਿਰਾ ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_ਬਾਈਟਾਂ(p, len, ਨਿਰਾ, 0));
    }
    ਨਹੀਂਤਾਂ
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_ਬਾਈਟਾਂ(p, (ਪੂਰਨ)STRLEN(p), ਨਿਰਾ, 0));
    }
}
#ਜੇ_ਅੰਤ // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ do_special;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    ਸਥਿਰ ਖਾਲੀ
f_shiftwidth(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = 0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ਲੰਮਾ	ਖਾਨਾ;

	ਖਾਨਾ = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(argvars, ਨਿਰਾ);
	ਜੇ (ਖਾਨਾ < 0)
	    ਵਾਪਸ;	// type error; errmsg already given
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	rettv->vval.v_ਸੰਖਿਆ = get_sw_ਮੁੱਲ_ਖਾਨਾ(curbuf, ਖਾਨਾ, FALSE);
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    }

    rettv->vval.v_ਸੰਖਿਆ = get_sw_ਮੁੱਲ(curbuf);
}

/*
 * "soundfold({word})" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_soundfold(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#ਨਹੀਂਤਾਂ
    rettv->vval.v_string = vim_strsave(s);
#ਜੇ_ਅੰਤ
}

/*
 * "spellbadword()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_spellbadword(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    char_u	*ਸ਼ਬਦ = (char_u *)"";
    hlf_T	attr = HLF_ਗਿਣਤੀ;
    ਪੂਰਨ		len = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    ਪੂਰਨ		wo_spell_save = curwin->w_p_spell;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    ਜੇ (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#ਜੇ_ਅੰਤ
	ਵਾਪਸ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_ਮੰਜ਼ਿਲ(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	ਜੇ (len != 0)
	{
	    ਸ਼ਬਦ = ml_get_cursor();
	    curwin->w_ਸੈੱਟ_curswant = true;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	ਪੂਰਨ	capcol = -1;

	ਜੇ (str != ਨਿਰਾ)
	{
	    // Check the argument for spelling.
	    ਜਦੋਂ (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		ਜੇ (attr != HLF_ਗਿਣਤੀ)
		{
		    ਸ਼ਬਦ = str;
		    ਰੋਕੋ;
		}
		str += len;
		capcol -= len;
		len = 0;
	    }
	}
    }
    curwin->w_p_spell = wo_spell_save;
#ਜੇ_ਅੰਤ

    list_append_string(rettv->vval.v_list, ਸ਼ਬਦ, len);
    ਚੋਣ (attr)
    {
    ਹਾਲਤ HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	ਰੋਕੋ;

    ਹਾਲਤ HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	ਰੋਕੋ;

    ਹਾਲਤ HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	ਰੋਕੋ;

    ਹਾਲਤ HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	ਰੋਕੋ;

    ਮੂਲ:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	ਰੋਕੋ;
    }
}

/*
 * "spellsuggest()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_spellsuggest(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    char_u	*str;
    ਪੂਰਨ		typeerr = FALSE;
    ਪੂਰਨ		maxcount;
    garray_T	ga;
    ਪੂਰਨ		i;
    listitem_T	*li;
    ਪੂਰਨ		need_capital = FALSE;
    ਪੂਰਨ		wo_spell_save = curwin->w_p_spell;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    ਜੇ (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    ਜੇ (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#ਜੇ_ਅੰਤ
	ਵਾਪਸ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[1], &typeerr);
	ਜੇ (maxcount <= 0)
	    ਵਾਪਸ;
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (ਪੂਰਨ)tv_get_bool_chk(&argvars[2], &typeerr);
	    ਜੇ (typeerr)
		ਵਾਪਸ;
	}
    }
    ਨਹੀਂਤਾਂ
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    ਲਈ (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_ਅੰਕੜੇ)[i];

	li = listitem_alloc();
	ਜੇ (li == ਨਿਰਾ)
	    vim_free(str);
	ਨਹੀਂਤਾਂ
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_ਤਾਲਾ = 0;
	    li->li_tv.vval.v_string = str;
	    list_append(rettv->vval.v_list, li);
	}
    }
    ga_clear(&ga);
    curwin->w_p_spell = wo_spell_save;
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਖਾਲੀ
f_ਵੰਡੋ(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*ਸਿਰਾ;
    char_u	*pat = ਨਿਰਾ;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    ਪੂਰਨ		ਮੇਲ;
    colnr_T	ਖਾਨਾ = 0;
    ਪੂਰਨ		keepempty = FALSE;
    ਪੂਰਨ		typeerr = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;

    str = tv_get_string(&argvars[0]);
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	ਜੇ (pat == ਨਿਰਾ)
	    typeerr = TRUE;
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (ਪੂਰਨ)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    ਜੇ (pat == ਨਿਰਾ || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਜਾਓ theend;
    ਜੇ (typeerr)
	ਜਾਓ theend;

    regmatch.regprog = eval_regcomp(pat);
    ਜੇ (regmatch.regprog != ਨਿਰਾ)
    {
	regmatch.rm_ic = FALSE;
	ਜਦੋਂ (*str != NUL || keepempty)
	{
	    ਜੇ (*str == NUL)
		ਮੇਲ = FALSE;	// empty item at the end
	    ਨਹੀਂਤਾਂ
		ਮੇਲ = vim_regexec_nl(&regmatch, str, ਖਾਨਾ);
	    ਜੇ (ਮੇਲ)
		ਸਿਰਾ = regmatch.startp[0];
	    ਨਹੀਂਤਾਂ
		ਸਿਰਾ = str + STRLEN(str);
	    ਜੇ (keepempty || ਸਿਰਾ > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && ਮੇਲ && ਸਿਰਾ < regmatch.endp[0]))
	    {
		ਜੇ (list_append_string(rettv->vval.v_list, str,
						    (ਪੂਰਨ)(ਸਿਰਾ - str)) == FAIL)
		    ਰੋਕੋ;
	    }
	    ਜੇ (!ਮੇਲ)
		ਰੋਕੋ;
	    // Advance to just after the match.
	    ਜੇ (regmatch.endp[0] > str)
		ਖਾਨਾ = 0;
	    ਨਹੀਂਤਾਂ
		// Don't get stuck at the same match.
		ਖਾਨਾ = (*mb_ptr2len)(regmatch.endp[0]);
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
    ਸਥਿਰ ਖਾਲੀ
f_submatch(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		error = FALSE;
    ਪੂਰਨ		no;
    ਪੂਰਨ		retList = 0;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    no = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[0], &error);
    ਜੇ (error)
	ਵਾਪਸ;
    ਜੇ (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_ਸੰਖਿਆ_nr), no);
	ਵਾਪਸ;
    }
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	retList = (ਪੂਰਨ)tv_get_bool_chk(&argvars[1], &error);
    ਜੇ (error)
	ਵਾਪਸ;

    ਜੇ (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    ਨਹੀਂਤਾਂ
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = reg_submatch_list(no);
    }
}

/*
 * "substitute()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_substitute(typval_T *argvars, typval_T *rettv)
{
    char_u	patbuf[NUMBUFLEN];
    char_u	subbuf[NUMBUFLEN];
    char_u	flagsbuf[NUMBUFLEN];
    char_u	*str;
    char_u	*pat;
    char_u	*sub = ਨਿਰਾ;
    typval_T	*ਵਿਅੰਜਕ = ਨਿਰਾ;
    char_u	*flg;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	ਵਾਪਸ;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    ਜੇ (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	ਵਿਅੰਜਕ = &argvars[2];
    ਨਹੀਂਤਾਂ
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    ਜੇ (str == ਨਿਰਾ || pat == ਨਿਰਾ || (sub == ਨਿਰਾ && ਵਿਅੰਜਕ == ਨਿਰਾ)
								|| flg == ਨਿਰਾ)
	rettv->vval.v_string = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, ਵਿਅੰਜਕ, flg, ਨਿਰਾ);
}

/*
 * "swapfilelist()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_swapfilelist(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    recover_ਨਾਂ(ਨਿਰਾ, FALSE, rettv->vval.v_list, 0, ਨਿਰਾ);
}

/*
 * "swapinfo(swap_filename)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (rettv_dict_alloc(rettv) == OK)
	get_b0_dict(tv_get_string(argvars), rettv->vval.v_dict);
}

/*
 * "swapname(expr)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_swapname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ਜੇ (buf == ਨਿਰਾ || buf->b_ml.ml_mfp == ਨਿਰਾ
					|| buf->b_ml.ml_mfp->mf_fname == ਨਿਰਾ)
	rettv->vval.v_string = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_synID(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਪੂਰਨ		id = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	ਖਾਨਾ;
    ਪੂਰਨ		trans;
    ਪੂਰਨ		transerr = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    ਖਾਨਾ = (linenr_T)tv_get_ਸੰਖਿਆ(&argvars[1]) - 1;	// -1 on type error
    trans = (ਪੂਰਨ)tv_get_bool_chk(&argvars[2], &transerr);

    ਜੇ (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
	    && ਖਾਨਾ >= 0 && ਖਾਨਾ < (ਲੰਮਾ)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, ਖਾਨਾ, trans, ਨਿਰਾ, FALSE);
#ਜੇ_ਅੰਤ

    rettv->vval.v_ਸੰਖਿਆ = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_synIDattr(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    char_u	*p = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਪੂਰਨ		id;
    char_u	*what;
    char_u	*ਬਹੁਲਕ;
    char_u	modebuf[NUMBUFLEN];
    ਪੂਰਨ		modec;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ਬਹੁਲਕ = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(ਬਹੁਲਕ[0]);
	ਜੇ (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    ਨਹੀਂਤਾਂ
    {
# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	ਜੇ (USE_24_ਦੋਆਧਾਰੀ_ਅੰਕ)
	    modec = 'g';
	ਨਹੀਂਤਾਂ
# endif
	    ਜੇ (t_colors > 1)
		modec = 'c';
	    ਨਹੀਂਤਾਂ
		modec = 't';
    }

    ਚੋਣ (TOLOWER_ASC(what[0]))
    {
	ਹਾਲਤ 'b':
		ਜੇ (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		ਨਹੀਂਤਾਂ					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		ਰੋਕੋ;

	ਹਾਲਤ 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		ਰੋਕੋ;

	ਹਾਲਤ 'i':
		ਜੇ (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		ਨਹੀਂਤਾਂ					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		ਰੋਕੋ;

	ਹਾਲਤ 'n':
		ਜੇ (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		ਨਹੀਂਤਾਂ					// name
		    p = get_highlight_name_ext(ਨਿਰਾ, id - 1, FALSE);
		ਰੋਕੋ;

	ਹਾਲਤ 'r':					//ਉਲਟ
		p = highlight_has_attr(id, HL_INVERSE, modec);
		ਰੋਕੋ;

	ਹਾਲਤ 's':
		ਜੇ (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		ਨਹੀਂਤਾਂ ਜੇ (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		ਨਹੀਂਤਾਂ					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		ਰੋਕੋ;

	ਹਾਲਤ 'u':
		ਜੇ (STRLEN(what) >= 9)
		{
		    ਜੇ (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    ਨਹੀਂਤਾਂ ਜੇ (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    ਨਹੀਂਤਾਂ ਜੇ (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    ਨਹੀਂਤਾਂ ਜੇ (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    ਨਹੀਂਤਾਂ
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		ਨਹੀਂਤਾਂ
							// ul
		    p = highlight_color(id, what, modec);
		ਰੋਕੋ;
    }

    ਜੇ (p != ਨਿਰਾ)
	p = vim_strsave(p);
#ਜੇ_ਅੰਤ
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_synIDtrans(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਪੂਰਨ		id;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]);

    ਜੇ (id > 0)
	id = syn_get_final_id(id);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	id = 0;

    rettv->vval.v_ਸੰਖਿਆ = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_synconcealed(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
#ਜੇ defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	ਖਾਨਾ;
    ਪੂਰਨ		syntax_flags = 0;
    ਪੂਰਨ		cchar;
    ਪੂਰਨ		matchid = 0;
    char_u	str[NUMBUFLEN];
#ਜੇ_ਅੰਤ

    rettv_list_ਸੈੱਟ(rettv, ਨਿਰਾ);

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

#ਜੇ defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    ਖਾਨਾ = (colnr_T)tv_get_ਸੰਖਿਆ(&argvars[1]) - 1;	// -1 on type error

    CLEAR_ਖੇਤਰ(str);

    ਜੇ (rettv_list_alloc(rettv) == OK)
    {
	ਜੇ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
	    && ਖਾਨਾ >= 0 && ਖਾਨਾ <= (ਲੰਮਾ)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (ਖਾਲੀ)syn_get_id(curwin, lnum, ਖਾਨਾ, FALSE, ਨਿਰਾ, FALSE);
	    syntax_flags = get_syntax_info(&matchid);

	    // get the conceal character
	    ਜੇ ((syntax_flags & HL_CONCEAL) && curwin->w_p_cole < 3)
	    {
		cchar = syn_get_sub_char();
		ਜੇ (cchar == NUL && curwin->w_p_cole == 1)
		    cchar = (curwin->w_lcs_chars.conceal == NUL) ? ' '
					: curwin->w_lcs_chars.conceal;
		ਜੇ (cchar != NUL)
		{
		    ਜੇ (has_mbyte)
			(*mb_char2bytes)(cchar, str);
		    ਨਹੀਂਤਾਂ
			str[0] = cchar;
		}
	    }
	}

	list_append_ਸੰਖਿਆ(rettv->vval.v_list,
					    (syntax_flags & HL_CONCEAL) != 0);
	// -1 to auto-determine strlen
	list_append_string(rettv->vval.v_list, str, -1);
	list_append_ਸੰਖਿਆ(rettv->vval.v_list, matchid);
    }
#ਜੇ_ਅੰਤ
}

/*
 * "synstack(lnum, col)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_synstack(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	ਖਾਨਾ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		id;
#ਜੇ_ਅੰਤ

    rettv_list_ਸੈੱਟ(rettv, ਨਿਰਾ);

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    ਖਾਨਾ = (colnr_T)tv_get_ਸੰਖਿਆ(&argvars[1]) - 1;	// -1 on type error

    ਜੇ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
	    && ਖਾਨਾ >= 0 && ਖਾਨਾ <= (ਲੰਮਾ)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(ਖਾਲੀ)syn_get_id(curwin, lnum, ਖਾਨਾ, FALSE, ਨਿਰਾ, TRUE);
	ਲਈ (i = 0; ; ++i)
	{
	    id = syn_get_ਢੇਰ_ਵਸਤੂ(i);
	    ਜੇ (id < 0)
		ਰੋਕੋ;
	    ਜੇ (list_append_ਸੰਖਿਆ(rettv->vval.v_list, id) == FAIL)
		ਰੋਕੋ;
	}
    }
#ਜੇ_ਅੰਤ
}

/*
 * "tabpagebuflist()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_tabpagebuflist(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    tabpage_T	*tp;
    win_T	*wp = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    ਨਹੀਂਤਾਂ
    {
	tp = ਲੱਭੋ_tabpage((ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]));
	ਜੇ (tp != ਨਿਰਾ)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    ਜੇ (wp != ਨਿਰਾ && rettv_list_alloc(rettv) == OK)
    {
	ਲਈ (; wp != ਨਿਰਾ; wp = wp->w_ਅਗਲਾ)
	    ਜੇ (list_append_ਸੰਖਿਆ(rettv->vval.v_list,
						wp->w_ਬਫਰ->b_fnum) == FAIL)
		ਰੋਕੋ;
    }
}

/*
 * "tagfiles()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_tagfiles(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    ਪੂਰਨ		ਪਹਿਲਾ;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    fname = alloc(MAXPATHL);
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (ਪਹਿਲਾ = TRUE; ; ਪਹਿਲਾ = FALSE)
	ਜੇ (get_tagfname(&tn, ਪਹਿਲਾ, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    ਰੋਕੋ;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = ਨਿਰਾ;
    char_u  *ਟੈਗ_ਨਮੂਨਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਟੈਗ_ਨਮੂਨਾ = tv_get_string(&argvars[0]);

    rettv->vval.v_ਸੰਖਿਆ = FALSE;
    ਜੇ (*ਟੈਗ_ਨਮੂਨਾ == NUL)
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    ਜੇ (rettv_list_alloc(rettv) == OK)
	(ਖਾਲੀ)get_ਟੈਗ_ਸੂਚੀ(rettv->vval.v_list, ਟੈਗ_ਨਮੂਨਾ, fname);
}

/*
 * "type(expr)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_type(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ n = -1;

    ਚੋਣ (argvars[0].v_type)
    {
	ਹਾਲਤ VAR_ਸੰਖਿਆ:  n = VAR_TYPE_ਸੰਖਿਆ; ਰੋਕੋ;
	ਹਾਲਤ VAR_STRING:  n = VAR_TYPE_STRING; ਰੋਕੋ;
	ਹਾਲਤ VAR_PARTIAL:
	ਹਾਲਤ VAR_FUNC:    n = VAR_TYPE_FUNC; ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:    n = VAR_TYPE_LIST; ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:   n = VAR_TYPE_TUPLE; ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:    n = VAR_TYPE_DICT; ਰੋਕੋ;
	ਹਾਲਤ VAR_FLOAT:   n = VAR_TYPE_FLOAT; ਰੋਕੋ;
	ਹਾਲਤ VAR_BOOL:	  n = VAR_TYPE_BOOL; ਰੋਕੋ;
	ਹਾਲਤ VAR_SPECIAL: n = VAR_TYPE_NONE; ਰੋਕੋ;
	ਹਾਲਤ VAR_ਕੰਮ:     n = VAR_TYPE_ਕੰਮ; ਰੋਕੋ;
	ਹਾਲਤ VAR_CHANNEL: n = VAR_TYPE_CHANNEL; ਰੋਕੋ;
	ਹਾਲਤ VAR_BLOB:    n = VAR_TYPE_BLOB; ਰੋਕੋ;
	ਹਾਲਤ VAR_INSTR:   n = VAR_TYPE_INSTR; ਰੋਕੋ;
	ਹਾਲਤ VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; ਰੋਕੋ;
	ਹਾਲਤ VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		ਜੇ (cl != ਨਿਰਾ && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		ਨਹੀਂਤਾਂ
		    n = VAR_TYPE_CLASS;
		ਰੋਕੋ;
	    }
	ਹਾਲਤ VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		ਜੇ (obj == ਨਿਰਾ)
		    n = VAR_TYPE_OBJECT;
		ਨਹੀਂਤਾਂ
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    ਜੇ (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    ਨਹੀਂਤਾਂ
			n = VAR_TYPE_OBJECT;
		}
		ਰੋਕੋ;
	    }
	ਹਾਲਤ VAR_VOID:
	    emsg(_(e_cannot_use_void_ਮੁੱਲ));
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	    ਅੰਦਰੂਨੀ_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    ਰੋਕੋ;
    }
    rettv->vval.v_ਸੰਖਿਆ = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_ਸ਼ੁਰੂ = 0;
    colnr_T	vcol_ਸਿਰਾ = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    ਪੂਰਨ		winchanged = FALSE;
    ਪੂਰਨ		len;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL))))
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[2]), &tp);
	ਜੇ (wp == ਨਿਰਾ || tp == ਨਿਰਾ)
	    ਜਾਓ theend;

	ਜੇ (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    ਜਾਓ theend;

	check_cursor();
	winchanged = TRUE;
    }

    ਪੂਰਨ fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    ਜੇ (fp != ਨਿਰਾ && fp->lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	ਜੇ (fp->ਖਾਨਾ < 0)
	    fp->ਖਾਨਾ = 0;
	ਨਹੀਂਤਾਂ
	{
	    len = (ਪੂਰਨ)ml_get_len(fp->lnum);
	    ਜੇ (fp->ਖਾਨਾ > len)
		fp->ਖਾਨਾ = len;
	}
	getvvcol(curwin, fp, &vcol_ਸ਼ੁਰੂ, ਨਿਰਾ, &vcol_ਸਿਰਾ, 0);
	++vcol_ਸ਼ੁਰੂ;
	++vcol_ਸਿਰਾ;
    }

theend:
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	ਜੇ (rettv_list_alloc(rettv) == OK)
	{
	    list_append_ਸੰਖਿਆ(rettv->vval.v_list, vcol_ਸ਼ੁਰੂ);
	    list_append_ਸੰਖਿਆ(rettv->vval.v_list, vcol_ਸਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
	    rettv->vval.v_ਸੰਖਿਆ = 0;
    }
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = vcol_ਸਿਰਾ;

    ਜੇ (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_ਬਹੁਲਕ_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    ਜੇ (non_zero_arg(&argvars[0]))
	curbuf->b_visual_ਬਹੁਲਕ_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_wildmenumode(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (wild_menu_showing || ((State & ਬਹੁਲਕ_CMDLINE) && cmdline_pum_ਸਰਗਰਮ()))
	rettv->vval.v_ਸੰਖਿਆ = 1;
}

/*
 * "windowsversion()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_windowsversion(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windows_ਸੰਸਕਰਣ);
}

/*
 * "wordcount()" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_wordcount(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    ਸਥਿਰ ਖਾਲੀ
f_xor(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ)
					^ tv_get_ਸੰਖਿਆ_chk(&argvars[1], ਨਿਰਾ);
}

#ਜੇ_ਅੰਤ // FEAT_EVAL
