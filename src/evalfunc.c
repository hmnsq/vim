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
#ವ್ಯಾಖ್ಯಾನಿಸು USING_FLOAT_STUFF

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
# include <float.h>
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ f_and(typval_T *argvars, typval_T *rettv);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL
ಸ್ಥಿರ ಶೂನ್ಯ f_balloon_gettext(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_balloon_show(typval_T *argvars, typval_T *rettv);
# ಆದರೆ defined(FEAT_BEVAL_ಪದ)
ಸ್ಥಿರ ಶೂನ್ಯ f_balloon_ವಿಭಜಿಸು(typval_T *argvars, typval_T *rettv);
# endif
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ f_base64_encode(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_base64_decode(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_bindtextdomain(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_byte2line(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_call(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_changenr(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_char2nr(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_charcol(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ಲಂಬಸಾಲು(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_confirm(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_copy(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_cursor(typval_T *argsvars, typval_T *rettv);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
ಸ್ಥಿರ ಶೂನ್ಯ f_debugbreak(typval_T *argvars, typval_T *rettv);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ f_deepcopy(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_did_filetype(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_echoraw(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ಖಾಲಿಗಳು(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_environ(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_err_teapot(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_escape(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_eval(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_eventhandler(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_execute(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_exists_compiled(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_expand(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_expandcmd(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_feedkeys(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_fnameescape(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_foreground(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_funcref(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_function(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_garbagecollect(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_get(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getbgcolor(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getcellpixels(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getchangelist(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getcharpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getcharsearch(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getcurpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getcursorcharpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getenv(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getfontname(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getinfo(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getjumplist(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getpid(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getreg(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getreginfo(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getregion(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getregionpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_getregtype(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_gettagstack(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_gettext(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_haslocaldir(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_hlID(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_hlexists(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_hostname(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_id(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ಸೂಚ್ಯಂಕ(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_indexof(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ಒಳಹರಿವು(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_inputdialog(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_inputlist(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_inputrestore(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_inputsave(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_inputsecret(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ಅಡಚಣೆ(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_invert(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_islocked(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ವಸ್ತುಗಳು(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_keytrans(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ಕೊನೆಯ_ಬಫರ್_nr(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_libcall(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_libcallnr(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ಸಾಲು(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_line2byte(typval_T *argvars, typval_T *rettv);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LUA
ಸ್ಥಿರ ಶೂನ್ಯ f_luaeval(typval_T *argvars, typval_T *rettv);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ f_ಹೊಂದಿಕೆ(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_matchbufline(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_matchend(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_matchlist(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_matchstr(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_matchstrlist(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_matchstrpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_max(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_min(typval_T *argvars, typval_T *rettv);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
ಸ್ಥಿರ ಶೂನ್ಯ f_mzeval(typval_T *argvars, typval_T *rettv);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ f_nextnonblank(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ngettext(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_nr2char(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_or(typval_T *argvars, typval_T *rettv);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERL
ಸ್ಥಿರ ಶೂನ್ಯ f_perleval(typval_T *argvars, typval_T *rettv);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ f_prevnonblank(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_printf(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_pum_getpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_pumvisible(typval_T *argvars, typval_T *rettv);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
ಸ್ಥಿರ ಶೂನ್ಯ f_py3eval(typval_T *argvars, typval_T *rettv);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
ಸ್ಥಿರ ಶೂನ್ಯ f_pyeval(typval_T *argvars, typval_T *rettv);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
ಸ್ಥಿರ ಶೂನ್ಯ f_pyxeval(typval_T *argvars, typval_T *rettv);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ f_test_srand_ಬೀಜ(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_rand(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_range(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_reg_executing(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_reg_recording(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_rename(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_repeat(typval_T *argvars, typval_T *rettv);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RUBY
ಸ್ಥಿರ ಶೂನ್ಯ f_rubyeval(typval_T *argvars, typval_T *rettv);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ f_screenattr(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_screenchar(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_screenchars(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_screencol(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_screenrow(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_screenstring(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ಹುಡುಕು(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_searchdecl(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_searchpair(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_searchpairpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_searchpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_setcharpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_setcharsearch(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_setcursorcharpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_setenv(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_setfperm(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_setpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_setreg(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_settagstack(typval_T *argvars, typval_T *rettv);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
ಸ್ಥಿರ ಶೂನ್ಯ f_sha256(typval_T *argvars, typval_T *rettv);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ f_shellescape(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_shiftwidth(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_soundfold(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_spellbadword(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_spellsuggest(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ವಿಭಜಿಸು(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_srand(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_submatch(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_substitute(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_swapfilelist(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_swapinfo(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_swapname(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_synID(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_synIDattr(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_synIDtrans(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_synstack(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_synconcealed(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_tabpagebuflist(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_taglist(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_tagfiles(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_type(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_virtcol(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_visualmode(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_wildmenumode(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_windowsversion(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_wordcount(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_xor(typval_T *argvars, typval_T *rettv);


/*
 * Functions that check the argument type of a builtin function.
 * Each function returns FAIL and gives an error message if the type is wrong.
 */

// Context passed to an arg_ function.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		arg_ಎಣಿಕೆ;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    ಪೂರ್ಣಾಂಕ		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    ಹಿಂತಿರುಗಿ need_type(actual, expected, 0,
	    context->arg_idx - context->arg_ಎಣಿಕೆ, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    ಆದರೆ (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_ಎಣಿಕೆ, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    ಪೂರ್ಣಾಂಕ
arg_type_modifiable(type_T *type, ಪೂರ್ಣಾಂಕ arg_idx)
{
    ಅಕ್ಷರ *tofree;

    ಆದರೆ ((type->tt_flags & TTFLAG_CONST) == 0)
	ಹಿಂತಿರುಗಿ OK;
    semsg(_(e_ವಾದ_nr_trying_ಗಮ್ಯ_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_any(type_T *type ಬಳಸದ,
	type_T *decl_type ಬಳಸದ,
	argcontext_T *context ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check "type" is a float or a number.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_float_or_nr(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_ಸಂಖ್ಯೆ, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a number.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_ಸಂಖ್ಯೆ(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಹಿಂತಿರುಗಿ check_arg_type(&t_ಸಂಖ್ಯೆ, type, context);
}

/*
 * Check "type" is an object.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_object(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_dict_any(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಹಿಂತಿರುಗಿ check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_any(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಹಿಂತಿರುಗಿ check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type ಬಳಸದ,
	argcontext_T *context)
{
    ಹಿಂತಿರುಗಿ check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_ಸಂಖ್ಯೆ(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಹಿಂತಿರುಗಿ check_arg_type(&t_list_ಸಂಖ್ಯೆ, type, context);
}

/*
 * Check "type" is a list of strings.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_string(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಹಿಂತಿರುಗಿ check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_tuple_any(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಹಿಂತಿರುಗಿ check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_string(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಹಿಂತಿರುಗಿ check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_blob(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಹಿಂತಿರುಗಿ check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_bool(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಹಿಂತಿರುಗಿ check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_bool_or_nr(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_ಸಂಖ್ಯೆ, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_or_blob(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ಆದರೆ (arg_list_or_blob(type, decl_type, context) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a string or a number
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_string_or_nr(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_ಬಫರ್(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_ಬಫರ್_or_dict_any(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_lnum(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    ಆದರೆ (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	ಹಿಂತಿರುಗಿ OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type ಬಳಸದ,
	argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ಆದರೆ (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ಆದರೆ (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_ವಾದ_nr),
						 context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = ಖಾಲಿ;
    type_T *(ವಾದಗಳು[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, ಖಾಲಿ, ಖಾಲಿ, ವಾದಗಳು};

    ಆದರೆ (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	ಆದರೆ (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	ಇಲ್ಲದಿದ್ದರೆ
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_ಸಂಖ್ಯೆ;

    ವಾದಗಳು[0] = ಖಾಲಿ;
    ವಾದಗಳು[1] = &t_unknown;
    ಆದರೆ (type->tt_argcount != -1)
    {
	ಆದರೆ (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_ಸಂಖ್ಯೆ_ದ_arguments));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		ವಾದಗಳು[0] = &t_ಸಂಖ್ಯೆ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		ವಾದಗಳು[0] = &t_string;
	    ಆದರೆ (ವಾದಗಳು[0] != ಖಾಲಿ)
		ವಾದಗಳು[1] = expected_member;
	}
    }

    ಆದರೆ (!type_any_or_unknown(type->tt_member) || ವಾದಗಳು[0] != ಖಾಲಿ)
    {
	where_T where = WHERE_INIT;

	ಆದರೆ (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == ಖಾಲಿ
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	ಇಲ್ಲದಿದ್ದರೆ // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	ಆದರೆ (ವಾದಗಳು[0] == ಖಾಲಿ)
	    ವಾದಗಳು[0] = &t_unknown;
	ಆದರೆ (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_ಸೂಚ್ಯಂಕ = 2;
	where.wt_ಬಗೆ = WT_ವಾದ;
	ಹಿಂತಿರುಗಿ check_type(&t_func_exp, type, TRUE, where);
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_filter_func(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (type->tt_type == VAR_FUNC)
	ಹಿಂತಿರುಗಿ check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_ವಾದ_nr), 2);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_map_func(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (type->tt_type == VAR_FUNC)
	ಹಿಂತಿರುಗಿ check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_ವಾದ_nr), 2);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_foreach_func(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (type->tt_type == VAR_FUNC)
	ಹಿಂತಿರುಗಿ check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_ವಾದ_nr), 2);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_ವಿಂಗಡಿಸು_how(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (type->tt_type == VAR_FUNC)
    {
	type_T *(ವಾದಗಳು[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_ಸಂಖ್ಯೆ, ಖಾಲಿ, ವಾದಗಳು};

	ಆದರೆ (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    ವಾದಗಳು[0] = context->arg_types[0].type_curr->tt_member;
	ಇಲ್ಲದಿದ್ದರೆ
	    ವಾದಗಳು[0] = &t_unknown;
	ಆದರೆ (!type_any_or_unknown(type->tt_member) || ವಾದಗಳು[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    ವಾದಗಳು[1] = ವಾದಗಳು[0];
	    ಆದರೆ (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_ಸೂಚ್ಯಂಕ = 2;
	    where.wt_ಬಗೆ = WT_ವಾದ;
	    ಹಿಂತಿರುಗಿ check_type(&t_func_exp, type, TRUE, where);
	}

	ಹಿಂತಿರುಗಿ OK;
    }
    semsg(_(e_string_or_function_required_for_ವಾದ_nr), 2);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
varargs_class(type_T *type ಬಳಸದ,
	      type_T *decl_type ಬಳಸದ,
	      argcontext_T *context)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = context->arg_idx; i < context->arg_ಎಣಿಕೆ; ++i)
    {
	type2_T *types = &context->arg_types[i];
	ಆದರೆ (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_ವಾದ_nr), i + 1);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_ಹಿಮ್ಮುಖ(type_T *type, type_T *decl_type, argcontext_T *context)
{
    ಆದರೆ (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_ಕೆಲಸ(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಹಿಂತಿರುಗಿ check_arg_type(&t_ಕೆಲಸ, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_chan_or_ಕೆಲಸ(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_ಕೆಲಸ
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_same_as_prev(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_decl;

    ಹಿಂತಿರುಗಿ check_arg_type(prev_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_same_struct_as_prev(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;

    ಆದರೆ (prev_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	ಹಿಂತಿರುಗಿ check_arg_type(prev_type, type, context);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_ಅಂಶ_ದ_prev(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    ಆದರೆ (prev_type->tt_type == VAR_LIST)
	expected = prev_type->tt_member;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prev_type->tt_type == VAR_BLOB)
	expected = &t_ಸಂಖ್ಯೆ;
    ಇಲ್ಲದಿದ್ದರೆ
	// probably VAR_ANY, can't check
	ಹಿಂತಿರುಗಿ OK;

    ಹಿಂತಿರುಗಿ check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *ಮೊದಲ_type = context->arg_types[context->arg_idx - 2].type_curr;

    ಆದರೆ (ಮೊದಲ_type->tt_type == VAR_LIST || ಮೊದಲ_type->tt_type == VAR_BLOB)
	ಹಿಂತಿರುಗಿ arg_ಸಂಖ್ಯೆ(type, decl_type, context);
    ಆದರೆ (ಮೊದಲ_type->tt_type == VAR_DICT)
	ಹಿಂತಿರುಗಿ arg_string(type, decl_type, context);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_get1(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_len1(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *ಮೊದಲ_type = context->arg_types[context->arg_idx - 1].type_curr;

    ಆದರೆ (ಮೊದಲ_type->tt_type == VAR_LIST || ಮೊದಲ_type->tt_type == VAR_BLOB)
	ಹಿಂತಿರುಗಿ arg_ಸಂಖ್ಯೆ(type, decl_type, context);
    ಆದರೆ (ಮೊದಲ_type->tt_type == VAR_DICT)
	ಹಿಂತಿರುಗಿ arg_string_or_nr(type, decl_type, context);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_repeat1(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_slice1(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type ಬಳಸದ,
    argcontext_T	*context)
{
    ಆದರೆ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_ವಾದ_nr),
							 context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
arg_cursor1(type_T *type, type_T *decl_type ಬಳಸದ, argcontext_T *context)
{
    ಆದರೆ (type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ಹಿಂತಿರುಗಿ OK;

    arg_type_mismatch(&t_ಸಂಖ್ಯೆ, type, context->arg_idx + 1);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
ಸ್ಥಿರ argcheck_T arg1_blob[] = {arg_blob};
ಸ್ಥಿರ argcheck_T arg1_bool[] = {arg_bool};
ಸ್ಥಿರ argcheck_T arg1_ಬಫರ್[] = {arg_ಬಫರ್};
ಸ್ಥಿರ argcheck_T arg1_ಬಫರ್_or_dict_any[] = {arg_ಬಫರ್_or_dict_any};
ಸ್ಥಿರ argcheck_T arg1_chan_or_ಕೆಲಸ[] = {arg_chan_or_ಕೆಲಸ};
ಸ್ಥಿರ argcheck_T arg1_dict_any[] = {arg_dict_any};
ಸ್ಥಿರ argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
ಸ್ಥಿರ argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
ಸ್ಥಿರ argcheck_T arg1_ಕೆಲಸ[] = {arg_ಕೆಲಸ};
ಸ್ಥಿರ argcheck_T arg1_list_any[] = {arg_list_any};
ಸ್ಥಿರ argcheck_T arg1_tuple_any[] = {arg_tuple_any};
ಸ್ಥಿರ argcheck_T arg1_list_ಸಂಖ್ಯೆ[] = {arg_list_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg1_ಹಿಮ್ಮುಖ[] = {arg_ಹಿಮ್ಮುಖ};
ಸ್ಥಿರ argcheck_T arg1_list_or_tuple_or_dict[] = {arg_list_or_tuple_or_dict};
ಸ್ಥಿರ argcheck_T arg1_list_string[] = {arg_list_string};
ಸ್ಥಿರ argcheck_T arg1_list_tuple_dict_blob_or_string[] = {arg_list_tuple_dict_blob_or_string};
ಸ್ಥಿರ argcheck_T arg1_lnum[] = {arg_lnum};
ಸ್ಥಿರ argcheck_T arg1_ಸಂಖ್ಯೆ[] = {arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg1_string[] = {arg_string};
ಸ್ಥಿರ argcheck_T arg1_string_or_list_any[] = {arg_string_or_list_any};
ಸ್ಥಿರ argcheck_T arg1_string_or_list_string[] = {arg_string_or_list_string};
ಸ್ಥಿರ argcheck_T arg1_string_or_nr[] = {arg_string_or_nr};
ಸ್ಥಿರ argcheck_T arg1_string_or_blob[] = {arg_string_or_blob};
ಸ್ಥಿರ argcheck_T arg2_ಬಫರ್_any[] = {arg_ಬಫರ್, arg_any};
ಸ್ಥಿರ argcheck_T arg2_ಬಫರ್_bool[] = {arg_ಬಫರ್, arg_bool};
ಸ್ಥಿರ argcheck_T arg2_ಬಫರ್_list_any[] = {arg_ಬಫರ್, arg_list_any};
ಸ್ಥಿರ argcheck_T arg2_ಬಫರ್_lnum[] = {arg_ಬಫರ್, arg_lnum};
ಸ್ಥಿರ argcheck_T arg2_ಬಫರ್_ಸಂಖ್ಯೆ[] = {arg_ಬಫರ್, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_ಬಫರ್_string[] = {arg_ಬಫರ್, arg_string};
ಸ್ಥಿರ argcheck_T arg2_chan_or_ಕೆಲಸ_dict[] = {arg_chan_or_ಕೆಲಸ, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_chan_or_ಕೆಲಸ_string[] = {arg_chan_or_ಕೆಲಸ, arg_string};
ಸ್ಥಿರ argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
ಸ್ಥಿರ argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
ಸ್ಥಿರ argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
ಸ್ಥಿರ argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
ಸ್ಥಿರ argcheck_T arg2_ಕೆಲಸ_dict[] = {arg_ಕೆಲಸ, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_ಕೆಲಸ_string_or_ಸಂಖ್ಯೆ[] = {arg_ಕೆಲಸ, arg_string_or_nr};
ಸ್ಥಿರ argcheck_T arg2_list_any_ಸಂಖ್ಯೆ[] = {arg_list_any, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_list_ಸಂಖ್ಯೆ[] = {arg_list_ಸಂಖ್ಯೆ, arg_list_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_list_ಸಂಖ್ಯೆ_bool[] = {arg_list_ಸಂಖ್ಯೆ, arg_bool};
ಸ್ಥಿರ argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_listblobmod_ಅಂಶ[] = {arg_list_or_blob_mod, arg_ಅಂಶ_ದ_prev};
ಸ್ಥಿರ argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
ಸ್ಥಿರ argcheck_T arg2_lnum_ಸಂಖ್ಯೆ[] = {arg_lnum, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ[] = {arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_any[] = {arg_ಸಂಖ್ಯೆ, arg_any};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_bool[] = {arg_ಸಂಖ್ಯೆ, arg_bool};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_dict_any[] = {arg_ಸಂಖ್ಯೆ, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_list[] = {arg_ಸಂಖ್ಯೆ, arg_list_any};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_string[] = {arg_ಸಂಖ್ಯೆ, arg_string};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_ಬಫರ್[] = {arg_ಸಂಖ್ಯೆ, arg_ಬಫರ್};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_string_or_list[] = {arg_ಸಂಖ್ಯೆ, arg_string_or_list_any};
ಸ್ಥಿರ argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_string[] = {arg_string, arg_string};
ಸ್ಥಿರ argcheck_T arg2_string_any[] = {arg_string, arg_any};
ಸ್ಥಿರ argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
ಸ್ಥಿರ argcheck_T arg2_string_chan_or_ಕೆಲಸ[] = {arg_string, arg_chan_or_ಕೆಲಸ};
ಸ್ಥಿರ argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_string_list_ಸಂಖ್ಯೆ[] = {arg_string, arg_list_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_string_ಸಂಖ್ಯೆ[] = {arg_string, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_string_or_list_ಸಂಖ್ಯೆ[] = {arg_string_or_list_any, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_string_string_or_ಸಂಖ್ಯೆ[] = {arg_string, arg_string_or_nr};
ಸ್ಥಿರ argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
ಸ್ಥಿರ argcheck_T arg3_any_ಬಫರ್_bool_or_dict[] = {
			      arg_any, arg_ಬಫರ್, arg_bool_or_dict_any};
ಸ್ಥಿರ argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_ಬಫರ್_lnum_lnum[] = {arg_ಬಫರ್, arg_lnum, arg_lnum};
ಸ್ಥಿರ argcheck_T arg3_ಬಫರ್_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ[] = {arg_ಬಫರ್, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg3_ಬಫರ್_string_any[] = {arg_ಬಫರ್, arg_string, arg_any};
ಸ್ಥಿರ argcheck_T arg3_ಬಫರ್_string_dict[] = {arg_ಬಫರ್, arg_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_dict_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ[] = {arg_dict_any, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_lnum_ಸಂಖ್ಯೆ_bool[] = {arg_lnum, arg_ಸಂಖ್ಯೆ, arg_bool};
ಸ್ಥಿರ argcheck_T arg3_ಸಂಖ್ಯೆ[] = {arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg3_ಸಂಖ್ಯೆ_any_dict[] = {arg_ಸಂಖ್ಯೆ, arg_any, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ_dict[] = {arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_ಸಂಖ್ಯೆ_string_any[] = {arg_ಸಂಖ್ಯೆ, arg_string, arg_any};
ಸ್ಥಿರ argcheck_T arg3_ಸಂಖ್ಯೆ_string_ಬಫರ್[] = {arg_ಸಂಖ್ಯೆ, arg_string, arg_ಬಫರ್};
ಸ್ಥಿರ argcheck_T arg3_ಸಂಖ್ಯೆ_string_string[] = {arg_ಸಂಖ್ಯೆ, arg_string, arg_string};
ಸ್ಥಿರ argcheck_T arg3_string[] = {arg_string, arg_string, arg_string};
ಸ್ಥಿರ argcheck_T arg3_string_any_dict[] = {arg_string, arg_any, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_string_any_string[] = {arg_string, arg_any, arg_string};
ಸ್ಥಿರ argcheck_T arg3_string_bool_bool[] = {arg_string, arg_bool, arg_bool};
ಸ್ಥಿರ argcheck_T arg3_string_ಸಂಖ್ಯೆ_bool[] = {arg_string, arg_ಸಂಖ್ಯೆ, arg_bool};
ಸ್ಥಿರ argcheck_T arg3_string_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ[] = {arg_string, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg3_string_or_dict_bool_dict[] = {arg_string_or_dict_any, arg_bool, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_string_or_list_bool_ಸಂಖ್ಯೆ[] = {arg_string_or_list_any, arg_bool, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg3_string_string_bool[] = {arg_string, arg_string, arg_bool};
ಸ್ಥಿರ argcheck_T arg3_string_string_dict[] = {arg_string, arg_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_string_string_ಸಂಖ್ಯೆ[] = {arg_string, arg_string, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg4_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ_string_any[] = {arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ, arg_string, arg_any};
ಸ್ಥಿರ argcheck_T arg4_string_string_any_string[] = {arg_string, arg_string, arg_any, arg_string};
ಸ್ಥಿರ argcheck_T arg4_string_string_ಸಂಖ್ಯೆ_string[] = {arg_string, arg_string, arg_ಸಂಖ್ಯೆ, arg_string};
ಸ್ಥಿರ argcheck_T arg4_string_ಸಂಖ್ಯೆ_bool_bool[] = {arg_string, arg_ಸಂಖ್ಯೆ, arg_bool, arg_bool};
/* Function specific argument types (not covered by the above) */
ಸ್ಥಿರ argcheck_T arg15_assert_fails[] = {arg_string_or_nr, arg_string_or_list_any, arg_any, arg_ಸಂಖ್ಯೆ, arg_string};
ಸ್ಥಿರ argcheck_T arg34_assert_inrange[] = {arg_float_or_nr, arg_float_or_nr, arg_float_or_nr, arg_string};
ಸ್ಥಿರ argcheck_T arg4_browse[] = {arg_bool, arg_string, arg_string, arg_string};
ಸ್ಥಿರ argcheck_T arg23_chanexpr[] = {arg_chan_or_ಕೆಲಸ, arg_any, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg23_chanraw[] = {arg_chan_or_ಕೆಲಸ, arg_string_or_blob, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg24_ಎಣಿಕೆ[] = {arg_string_list_tuple_or_dict, arg_any, arg_bool, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg13_cursor[] = {arg_cursor1, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg12_deepcopy[] = {arg_any, arg_bool};
ಸ್ಥಿರ argcheck_T arg12_execute[] = {arg_string_or_list_string, arg_string};
ಸ್ಥಿರ argcheck_T arg12_getchar[] = {arg_bool_or_nr, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg23_extend[] = {arg_list_or_dict_or_blob_mod, arg_same_as_prev, arg_extend3};
ಸ್ಥಿರ argcheck_T arg23_extendnew[] = {arg_list_or_dict_or_blob, arg_same_struct_as_prev, arg_extend3};
ಸ್ಥಿರ argcheck_T arg23_get[] = {arg_get1, arg_string_or_nr, arg_any};
ಸ್ಥಿರ argcheck_T arg14_glob[] = {arg_string, arg_bool, arg_bool, arg_bool};
ಸ್ಥಿರ argcheck_T arg25_globpath[] = {arg_string, arg_string, arg_bool, arg_bool, arg_bool};
ಸ್ಥಿರ argcheck_T arg24_ಸೂಚ್ಯಂಕ[] = {arg_list_or_tuple_or_blob, arg_ಅಂಶ_ದ_prev, arg_ಸಂಖ್ಯೆ, arg_bool};
ಸ್ಥಿರ argcheck_T arg23_ಸೂಚ್ಯಂಕ[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg23_ಸೇರಿಸು[] = {arg_list_or_blob, arg_ಅಂಶ_ದ_prev, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg1_len[] = {arg_len1};
ಸ್ಥಿರ argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
ಸ್ಥಿರ argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
ಸ್ಥಿರ argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
ಸ್ಥಿರ argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
ಸ್ಥಿರ argcheck_T arg2_instanceof[] = {arg_object, varargs_class, ಖಾಲಿ };
ಸ್ಥಿರ argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
ಸ್ಥಿರ argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
ಸ್ಥಿರ argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg45_matchbufline[] = {arg_ಬಫರ್, arg_string, arg_lnum, arg_lnum, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
ಸ್ಥಿರ argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
ಸ್ಥಿರ argcheck_T arg24_remote_ಅಭಿವ್ಯಕ್ತಿ[] = {arg_string, arg_string, arg_string, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_repeat[] = {arg_repeat1, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg15_ಹುಡುಕು[] = {arg_string, arg_string, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ, arg_string_or_func};
ಸ್ಥಿರ argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg3_setbufline[] = {arg_ಬಫರ್, arg_lnum, arg_str_or_nr_or_list};
ಸ್ಥಿರ argcheck_T arg2_setline[] = {arg_lnum, arg_any};
ಸ್ಥಿರ argcheck_T arg24_setloclist[] = {arg_ಸಂಖ್ಯೆ, arg_list_any, arg_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg23_settagstack[] = {arg_ಸಂಖ್ಯೆ, arg_dict_any, arg_string};
ಸ್ಥಿರ argcheck_T arg02_sign_getplaced[] = {arg_ಬಫರ್, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg45_sign_ಇರಿಸು[] = {arg_ಸಂಖ್ಯೆ, arg_string, arg_string, arg_ಬಫರ್, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg23_ತುಂಡು[] = {arg_slice1, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_ವಿಂಗಡಿಸು_how, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg24_strpart[] = {arg_string, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ, arg_bool};
ಸ್ಥಿರ argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
ಸ್ಥಿರ argcheck_T arg23_win_execute[] = {arg_ಸಂಖ್ಯೆ, arg_string_or_list_string, arg_string};
ಸ್ಥಿರ argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
ಸ್ಥಿರ argcheck_T arg24_ಹೊಂದಿಕೆ_func[] = {arg_string_or_list_any, arg_string, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};

// Can be used by functions called through "f_retfunc" to create new types.
ಸ್ಥಿರ garray_T *current_type_gap = ಖಾಲಿ;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    ಸ್ಥಿರ type_T *
ret_void(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_void;
}
    ಸ್ಥಿರ type_T *
ret_any(ಪೂರ್ಣಾಂಕ	argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_any;
}
    ಸ್ಥಿರ type_T *
ret_bool(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_bool;
}
    ಸ್ಥಿರ type_T *
ret_ಸಂಖ್ಯೆ_bool(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_ಸಂಖ್ಯೆ_bool;
}
    ಸ್ಥಿರ type_T *
ret_ಸಂಖ್ಯೆ(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_ಸಂಖ್ಯೆ;
}
    ಸ್ಥಿರ type_T *
ret_float(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_float;
}
    ಸ್ಥಿರ type_T *
ret_string(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_string;
}
    ಸ್ಥಿರ type_T *
ret_list_any(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_list_any;
}
    ಸ್ಥಿರ type_T *
ret_list_ಸಂಖ್ಯೆ(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಹಿಂತಿರುಗಿ &t_list_ಸಂಖ್ಯೆ;
}
    ಸ್ಥಿರ type_T *
ret_list_string(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಹಿಂತಿರುಗಿ &t_list_string;
}
    ಸ್ಥಿರ type_T *
ret_list_dict_any(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಹಿಂತಿರುಗಿ &t_list_dict_any;
}
    ಸ್ಥಿರ type_T *
ret_list_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಹಿಂತಿರುಗಿ &t_list_list_any;
}
    ಸ್ಥಿರ type_T *
ret_list_list_any(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಹಿಂತಿರುಗಿ &t_list_list_any;
}
    ಸ್ಥಿರ type_T *
ret_list_string_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಹಿಂತಿರುಗಿ &t_list_list_string;
}
    ಸ್ಥಿರ type_T *
ret_list_regionpos(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಹಿಂತಿರುಗಿ &t_list_list_list_ಸಂಖ್ಯೆ;
}
    ಸ್ಥಿರ type_T *
ret_tuple_any(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_tuple_any;
}
    ಸ್ಥಿರ type_T *
ret_dict_any(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_dict_any;
}
    ಸ್ಥಿರ type_T *
ret_ಕೆಲಸ_info(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type)
{
    ಆದರೆ (argcount == 0)
    {
	*decl_type = &t_list_any;
	ಹಿಂತಿರುಗಿ &t_list_ಕೆಲಸ;
    }
    ಹಿಂತಿರುಗಿ &t_dict_any;
}
    ಸ್ಥಿರ type_T *
ret_dict_ಸಂಖ್ಯೆ(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_dict_ಸಂಖ್ಯೆ;
}
    ಸ್ಥಿರ type_T *
ret_dict_string(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_dict_string;
}
    ಸ್ಥಿರ type_T *
ret_blob(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_blob;
}
    ಸ್ಥಿರ type_T *
ret_func_any(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_func_any;
}
    ಸ್ಥಿರ type_T *
ret_func_unknown(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_func_unknown;
}
    ಸ್ಥಿರ type_T *
ret_channel(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_channel;
}
    ಸ್ಥಿರ type_T *
ret_ಕೆಲಸ(ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ &t_ಕೆಲಸ;
}
    ಸ್ಥಿರ type_T *
ret_ಮೊದಲ_arg(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಆದರೆ (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	ಹಿಂತಿರುಗಿ argtypes[0].type_curr;
    }
    ಹಿಂತಿರುಗಿ &t_void;
}
    ಸ್ಥಿರ type_T *
ret_ತುಂಡು(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಆದರೆ (argcount > 0)
    {
	ಆದರೆ (argtypes[0].type_decl != ಖಾಲಿ)
	{
	    ಆಯ್ಕೆ (argtypes[0].type_decl->tt_type)
	    {
		ಸ್ಥಿತಿ VAR_STRING: *decl_type = &t_string; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ VAR_BLOB: *decl_type = &t_blob; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ VAR_LIST: *decl_type = &t_list_any; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ VAR_TUPLE: *decl_type = &t_tuple_any; ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ: ನಿಲ್ಲಿಸು;
	    }
	}
	ಹಿಂತಿರುಗಿ argtypes[0].type_curr;
    }
    ಹಿಂತಿರುಗಿ &t_void;
}
    ಸ್ಥಿರ type_T *
ret_copy(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಆದರೆ (argcount > 0)
    {
	ಆದರೆ (argtypes[0].type_decl != ಖಾಲಿ)
	{
	    ಆದರೆ (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    ಇಲ್ಲದಿದ್ದರೆ
		*decl_type = argtypes[0].type_decl;
	}
	ಆದರೆ (argtypes[0].type_curr != ಖಾಲಿ)
	{
	    ಆದರೆ (argtypes[0].type_curr->tt_type == VAR_LIST)
		ಹಿಂತಿರುಗಿ &t_list_any;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argtypes[0].type_curr->tt_type == VAR_DICT)
		ಹಿಂತಿರುಗಿ &t_dict_any;
	}
	ಹಿಂತಿರುಗಿ argtypes[0].type_curr;
    }
    ಹಿಂತಿರುಗಿ &t_void;
}
    ಸ್ಥಿರ type_T *
ret_extend(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಆದರೆ (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	// if the second argument has a different current type then the current
	// type is "any"
	ಆದರೆ (argcount > 1 && !equal_type(argtypes[0].type_curr,
						     argtypes[1].type_curr, 0))
	{
	    ಆದರೆ (argtypes[0].type_curr->tt_type == VAR_LIST)
		ಹಿಂತಿರುಗಿ &t_list_any;
	    ಆದರೆ (argtypes[0].type_curr->tt_type == VAR_DICT)
		ಹಿಂತಿರುಗಿ &t_dict_any;
	    ಆದರೆ (argtypes[0].type_curr->tt_type == VAR_BLOB)
		ಹಿಂತಿರುಗಿ &t_blob;
	}
	ಹಿಂತಿರುಗಿ argtypes[0].type_curr;
    }
    ಹಿಂತಿರುಗಿ &t_void;
}
    ಸ್ಥಿರ type_T *
ret_repeat(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type ಬಳಸದ)
{
    ಆದರೆ (argcount == 0)
	ಹಿಂತಿರುಗಿ &t_any;
    ಆದರೆ (argtypes[0].type_curr == &t_ಸಂಖ್ಯೆ)
	ಹಿಂತಿರುಗಿ &t_string;
    ಹಿಂತಿರುಗಿ argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    ಸ್ಥಿರ type_T *
ret_ಮೊದಲ_cont(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type ಬಳಸದ)
{
    ಆದರೆ (argcount > 0)
    {
	ಆದರೆ (argtypes[0].type_curr->tt_type == VAR_LIST)
	    ಹಿಂತಿರುಗಿ &t_list_any;
	ಆದರೆ (argtypes[0].type_curr->tt_type == VAR_DICT)
	    ಹಿಂತಿರುಗಿ &t_dict_any;
	ಆದರೆ (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    ಹಿಂತಿರುಗಿ argtypes[0].type_curr;
    }
    ಹಿಂತಿರುಗಿ &t_any;
}
// for get(): the type of the item when a missing item has the same type
    ಸ್ಥಿರ type_T *
ret_get(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಆದರೆ (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*ಅಂಶ = ಖಾಲಿ;

	ಆದರೆ (t->tt_type == VAR_BLOB)
	    ಅಂಶ = &t_ಸಂಖ್ಯೆ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    ಅಂಶ = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	ಆದರೆ (ಅಂಶ != ಖಾಲಿ && (argcount == 3
			? equal_type(ಅಂಶ, argtypes[2].type_curr, 0)
			: ಅಂಶ->tt_type == VAR_ಸಂಖ್ಯೆ))
	{
	    ಆದರೆ (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    ಹಿಂತಿರುಗಿ ಅಂಶ;
	}
    }
    ಹಿಂತಿರುಗಿ &t_any;
}
// for getline()
    ಸ್ಥಿರ type_T *
ret_getline(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಆದರೆ (argcount == 1)
	ಹಿಂತಿರುಗಿ &t_string;
    *decl_type = &t_list_any;
    ಹಿಂತಿರುಗಿ &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    ಸ್ಥಿರ type_T *
ret_string_or_any_1(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಆದರೆ (argcount < 1)
	ಹಿಂತಿರುಗಿ &t_string;
    ಹಿಂತಿರುಗಿ &t_any;
}
    ಸ್ಥಿರ type_T *
ret_string_or_any_2(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಆದರೆ (argcount < 2)
	ಹಿಂತಿರುಗಿ &t_string;
    ಹಿಂತಿರುಗಿ &t_any;
}
    ಸ್ಥಿರ type_T *
ret_string_or_any_3(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಆದರೆ (argcount < 3)
	ಹಿಂತಿರುಗಿ &t_string;
    ಹಿಂತಿರುಗಿ &t_any;
}
    ಸ್ಥಿರ type_T *
ret_string_or_any_4(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    ಆದರೆ (argcount < 4)
	ಹಿಂತಿರುಗಿ &t_string;
    ಹಿಂತಿರುಗಿ &t_any;
}
// for abs()
    ಸ್ಥಿರ type_T *
ret_abs(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type ಬಳಸದ)
{
    ಆದರೆ (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	ಹಿಂತಿರುಗಿ &t_float;
    ಹಿಂತಿರುಗಿ &t_ಸಂಖ್ಯೆ;
}
// for sign_define()
    ಸ್ಥಿರ type_T *
ret_sign_define(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type ಬಳಸದ)
{
    ಆದರೆ (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	ಹಿಂತಿರುಗಿ &t_list_ಸಂಖ್ಯೆ;
    ಹಿಂತಿರುಗಿ &t_ಸಂಖ್ಯೆ;
}
// for values(): list of member of first argument
    ಸ್ಥಿರ type_T *
ret_list_member(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಆದರೆ (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	ಆದರೆ (current_type_gap != ಖಾಲಿ
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	ಇಲ್ಲದಿದ್ದರೆ
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	ಆದರೆ (current_type_gap != ಖಾಲಿ
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    ಹಿಂತಿರುಗಿ get_list_type(t->tt_member, current_type_gap);
    }
    ಹಿಂತಿರುಗಿ &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    ಸ್ಥಿರ type_T *
ret_list_or_dict_0(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type)
{
    ಆದರೆ (argcount > 0)
	ಹಿಂತಿರುಗಿ &t_dict_any;
    *decl_type = &t_list_any;
    ಹಿಂತಿರುಗಿ &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    ಸ್ಥಿರ type_T *
ret_list_or_dict_1(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type)
{
    ಆದರೆ (argcount > 1)
	ಹಿಂತಿರುಗಿ &t_dict_any;
    *decl_type = &t_list_any;
    ಹಿಂತಿರುಗಿ &t_list_dict_any;
}

    ಸ್ಥಿರ type_T *
ret_argv(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type)
{
    // argv() returns list of strings
    ಆದರೆ (argcount == 0)
    {
	*decl_type = &t_list_any;
	ಹಿಂತಿರುಗಿ &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    ಹಿಂತಿರುಗಿ &t_any;
}

    ಸ್ಥಿರ type_T *
ret_remove(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಆದರೆ (argcount > 0)
    {
	ಆದರೆ (argtypes[0].type_curr->tt_type == VAR_LIST
		|| argtypes[0].type_curr->tt_type == VAR_DICT)
	{
	    ಆದರೆ (argcount == 3)
	    {
		*decl_type = argtypes[0].type_decl;
		ಹಿಂತಿರುಗಿ argtypes[0].type_curr;
	    }
	    ಆದರೆ (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    ಹಿಂತಿರುಗಿ argtypes[0].type_curr->tt_member;
	}
	ಆದರೆ (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    ಹಿಂತಿರುಗಿ &t_ಸಂಖ್ಯೆ;
    }
    ಹಿಂತಿರುಗಿ &t_any;
}

    ಸ್ಥಿರ type_T *
ret_max_min(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಆದರೆ (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	ಆದರೆ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	ಇಲ್ಲದಿದ್ದರೆ
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	ಆದರೆ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	ಇಲ್ಲದಿದ್ದರೆ
	    t = &t_any;
	ಹಿಂತಿರುಗಿ t;
    }
    ಹಿಂತಿರುಗಿ &t_any;
}

    ಸ್ಥಿರ type_T *
ret_getreg(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    ಆದರೆ (argcount == 3)
    {
	*decl_type = &t_list_any;
	ಹಿಂತಿರುಗಿ &t_list_string;
    }
    ಹಿಂತಿರುಗಿ &t_string;
}

    ಸ್ಥಿರ type_T *
ret_virtcol(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    ಆದರೆ (argcount > 1)
    {
	*decl_type = &t_list_any;
	ಹಿಂತಿರುಗಿ &t_list_ಸಂಖ್ಯೆ;
    }
    ಹಿಂತಿರುಗಿ &t_ಸಂಖ್ಯೆ;
}

    ಸ್ಥಿರ type_T *
ret_maparg(ಪೂರ್ಣಾಂಕ argcount,
	type2_T *argtypes ಬಳಸದ,
	type_T	**decl_type ಬಳಸದ)
{
    // Assume that if the fourth argument is passed it's non-zero
    ಆದರೆ (argcount == 4)
	ಹಿಂತಿರುಗಿ &t_dict_any;
    ಹಿಂತಿರುಗಿ &t_string;
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಅಕ್ಷರ	*f_name;	// function name
    ಅಕ್ಷರ	f_min_argc;	// minimal number of arguments
    ಅಕ್ಷರ	f_max_argc;	// maximal number of arguments
    ಅಕ್ಷರ	f_argtype;	// for method: FEARG_ values; bits FE_
    argcheck_T	*f_argcheck;	// list of functions to check argument types;
				// use "arg_any" (not NULL) to accept an
				// argument of any type
    ಅಕ್ಷರ	*f_argnames;	// names of the arguments as in the help,
				// separated by ", "; an argument without a
				// name is empty, NULL when there is none
    type_T	*(*f_retfunc)(ಪೂರ್ಣಾಂಕ argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    ಶೂನ್ಯ	(*f_func)(typval_T *ವಾದಗಳು, typval_T *rvar);
				// implementation of function
} funcentry_T;

// Set f_max_argc to VARGS for varargs.
#ವ್ಯಾಖ್ಯಾನಿಸು VARGS    CHAR_MAX

// values for f_argtype; zero means it cannot be used as a method
#ವ್ಯಾಖ್ಯಾನಿಸು FEARG_1	    0x01    // base is the first argument
#ವ್ಯಾಖ್ಯಾನಿಸು FEARG_2     0x02    // base is the second argument
#ವ್ಯಾಖ್ಯಾನಿಸು FEARG_3     0x03    // base is the third argument
#ವ್ಯಾಖ್ಯಾನಿಸು FEARG_4     0x04    // base is the fourth argument
#ವ್ಯಾಖ್ಯಾನಿಸು FEARG_ಮುಖವಾಡ  0x0F    // bits in f_argtype used as argument index
#ವ್ಯಾಖ್ಯಾನಿಸು FE_X	    0x10    // builtin accepts a non-value (class, typealias)

#ಆದರೆ defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#ಇಲ್ಲದಿದ್ದರೆ
# define MATH_FUNC(name) ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
# define TIMER_FUNC(name) name
#ಇಲ್ಲದಿದ್ದರೆ
# define TIMER_FUNC(name) ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
# define ಕೆಲಸ_FUNC(name) name
#ಇಲ್ಲದಿದ್ದರೆ
# define ಕೆಲಸ_FUNC(name) ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#ಇಲ್ಲದಿದ್ದರೆ
# define PROP_FUNC(name) ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
# define SIGN_FUNC(name) name
#ಇಲ್ಲದಿದ್ದರೆ
# define SIGN_FUNC(name) ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOUND
# define SOUND_FUNC(name) name
#ಇಲ್ಲದಿದ್ದರೆ
# define SOUND_FUNC(name) ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
# define ಪದ_FUNC(name) name
#ಇಲ್ಲದಿದ್ದರೆ
# define ಪದ_FUNC(name) ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#ಇಲ್ಲದಿದ್ದರೆ
# define TABPANEL_FUNC(name) ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
# define IMAGE_FUNC(name) name
#ಇಲ್ಲದಿದ್ದರೆ
# define IMAGE_FUNC(name) ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ funcentry_T global_functions[] =
{
    {"abs",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_abs,	    f_abs},
    {"acos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_acos},
    {"add",		2, 2, FEARG_1,	    arg2_listblobmod_ಅಂಶ,
			"object, expr",
			ret_ಮೊದಲ_arg,	    f_ಕೂಡಿಸು},
    {"and",		2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ,
			"expr1, expr2",
			ret_ಸಂಖ್ಯೆ,	    f_and},
    {"append",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_ಸಂಖ್ಯೆ_bool,    f_append},
    {"appendbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_ಸಂಖ್ಯೆ_bool,    f_appendbufline},
    {"argc",		0, 1, 0,	    arg1_ಸಂಖ್ಯೆ,
			"winid",
			ret_ಸಂಖ್ಯೆ,	    f_argc},
    {"argidx",		0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_argidx},
    {"arglistid",	0, 2, 0,	    arg2_ಸಂಖ್ಯೆ,
			"winnr, tabnr",
			ret_ಸಂಖ್ಯೆ,	    f_arglistid},
    {"argv",		0, 2, 0,	    arg2_ಸಂಖ್ಯೆ,
			"nr, winid",
			ret_argv,	    f_argv},
    {"asin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_asin},
    {"assert_beeps",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_beeps},
    {"assert_equal",	2, 3, FEARG_2,	    ಖಾಲಿ,
			"expected, actual, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_equal},
    {"assert_equalfile", 2, 3, FEARG_1,	    arg3_string,
			"fname_one, fname_two, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_equalfile},
    {"assert_exception", 1, 2, 0,	    arg2_string,
			"error, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_exception},
    {"assert_fails",	1, 5, FEARG_1,	    arg15_assert_fails,
			"cmd, error, msg, lnum, context",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_fails},
    {"assert_false",	1, 2, FEARG_1,	    ಖಾಲಿ,
			"actual, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_ಹೊಂದಿಕೆ},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    ಖಾಲಿ,
			"expected, actual, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    ಖಾಲಿ,
			"actual, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_true},
    {"atan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_atan},
    {"atan2",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"expr1, expr2",
			ret_float,	    f_atan2},
    {"autocmd_add",	1, 1, FEARG_1,	    arg1_list_any,
			"acmds",
			ret_bool,	    f_autocmd_ಕೂಡಿಸು},
    {"autocmd_delete",	1, 1, FEARG_1,	    arg1_list_any,
			"acmds",
			ret_bool,	    f_autocmd_delete},
    {"autocmd_get",	0, 1, FEARG_1,	    arg1_dict_any,
			"opts",
			ret_list_dict_any,  f_autocmd_get},
    {"balloon_gettext",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL
	    f_balloon_gettext
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL
	    f_balloon_show
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#ಆದರೆ defined(FEAT_BEVAL_ಪದ)
	    f_balloon_ವಿಭಜಿಸು
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
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
			ret_list_ಸಂಖ್ಯೆ,    f_blob2list},
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
			ret_ಸಂಖ್ಯೆ,	    f_bufadd},
    {"bufexists",	1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_ಸಂಖ್ಯೆ_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_ಬಫರ್,	// obsolete
			"buf",
			ret_ಸಂಖ್ಯೆ_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_ಬಫರ್,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_ಬಫರ್,	// obsolete
			"buf",
			ret_ಸಂಖ್ಯೆ,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_ಸಂಖ್ಯೆ_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_ಸಂಖ್ಯೆ_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_ಬಫರ್_bool,
			"buf, create",
			ret_ಸಂಖ್ಯೆ,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_ಸಂಖ್ಯೆ,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_ಸಂಖ್ಯೆ,	    f_bufwinnr},
    {"byte2line",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"byte",
			ret_ಸಂಖ್ಯೆ,	    f_byte2line},
    {"byteidx",		2, 3, FEARG_1,	    arg3_string_ಸಂಖ್ಯೆ_bool,
			"expr, nr, utf16",
			ret_ಸಂಖ್ಯೆ,	    f_byteidx},
    {"byteidxcomp",	2, 3, FEARG_1,	    arg3_string_ಸಂಖ್ಯೆ_bool,
			"expr, nr, utf16",
			ret_ಸಂಖ್ಯೆ,	    f_byteidxcomp},
    {"call",		2, 3, FEARG_1,	    arg3_any_list_dict,
			"func, arglist, dict",
			ret_any,	    f_call},
    {"ceil",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ceil},
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_ಕೆಲಸ,
			"handle",
			ret_ಸಂಖ್ಯೆ_bool,    ಕೆಲಸ_FUNC(f_ch_canread)},
    {"ch_close",	1, 1, FEARG_1,	    arg1_chan_or_ಕೆಲಸ,
			"handle",
			ret_void,	    ಕೆಲಸ_FUNC(f_ch_close)},
    {"ch_close_in",	1, 1, FEARG_1,	    arg1_chan_or_ಕೆಲಸ,
			"handle",
			ret_void,	    ಕೆಲಸ_FUNC(f_ch_close_ಒಳ)},
    {"ch_evalexpr",	2, 3, FEARG_1,	    arg23_chanexpr,
			"handle, expr, options",
			ret_any,	    ಕೆಲಸ_FUNC(f_ch_evalexpr)},
    {"ch_evalraw",	2, 3, FEARG_1,	    arg23_chanraw,
			"handle, string, options",
			ret_any,	    ಕೆಲಸ_FUNC(f_ch_evalraw)},
    {"ch_getbufnr",	2, 2, FEARG_1,	    arg2_chan_or_ಕೆಲಸ_string,
			"handle, what",
			ret_ಸಂಖ್ಯೆ,	    ಕೆಲಸ_FUNC(f_ch_getbufnr)},
    {"ch_getjob",	1, 1, FEARG_1,	    arg1_chan_or_ಕೆಲಸ,
			"channel",
			ret_ಕೆಲಸ,	    ಕೆಲಸ_FUNC(f_ch_getjob)},
    {"ch_info",		1, 1, FEARG_1,	    arg1_chan_or_ಕೆಲಸ,
			"handle",
			ret_dict_any,	    ಕೆಲಸ_FUNC(f_ch_info)},
    {"ch_listen",	1, 2, FEARG_1,	    arg2_string_dict,
			"address, options",
			ret_channel,	    ಕೆಲಸ_FUNC(f_ch_listen)},
    {"ch_log",		1, 2, FEARG_1,	    arg2_string_chan_or_ಕೆಲಸ,
			"msg, handle",
			ret_void,	    f_ch_log},
    {"ch_logfile",	1, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_void,	    f_ch_logfile},
    {"ch_open",		1, 2, FEARG_1,	    arg2_string_dict,
			"address, options",
			ret_channel,	    ಕೆಲಸ_FUNC(f_ch_open)},
    {"ch_read",		1, 2, FEARG_1,	    arg2_chan_or_ಕೆಲಸ_dict,
			"handle, options",
			ret_string,	    ಕೆಲಸ_FUNC(f_ch_read)},
    {"ch_readblob",	1, 2, FEARG_1,	    arg2_chan_or_ಕೆಲಸ_dict,
			"handle, options",
			ret_blob,	    ಕೆಲಸ_FUNC(f_ch_readblob)},
    {"ch_readraw",	1, 2, FEARG_1,	    arg2_chan_or_ಕೆಲಸ_dict,
			"handle, options",
			ret_string,	    ಕೆಲಸ_FUNC(f_ch_readraw)},
    {"ch_sendexpr",	2, 3, FEARG_1,	    arg23_chanexpr,
			"handle, expr, options",
			ret_any,	    ಕೆಲಸ_FUNC(f_ch_sendexpr)},
    {"ch_sendraw",	2, 3, FEARG_1,	    arg23_chanraw,
			"handle, expr, options",
			ret_void,	    ಕೆಲಸ_FUNC(f_ch_sendraw)},
    {"ch_setoptions",	2, 2, FEARG_1,	    arg2_chan_or_ಕೆಲಸ_dict,
			"handle, options",
			ret_void,	    ಕೆಲಸ_FUNC(f_ch_setoptions)},
    {"ch_status",	1, 2, FEARG_1,	    arg2_chan_or_ಕೆಲಸ_dict,
			"handle, options",
			ret_string,	    ಕೆಲಸ_FUNC(f_ch_ಸ್ಥಿತಿ)},
    {"changenr",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_changenr},
    {"char2nr",		1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_ಸಂಖ್ಯೆ,	    f_char2nr},
    {"charclass",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_ಸಂಖ್ಯೆ,	    f_charclass},
    {"charcol",		1, 2, FEARG_1,	    arg2_string_or_list_ಸಂಖ್ಯೆ,
			"expr, winid",
			ret_ಸಂಖ್ಯೆ,	    f_charcol},
    {"charidx",		2, 4, FEARG_1,	    arg4_string_ಸಂಖ್ಯೆ_bool_bool,
			"string, idx, countcc, utf16",
			ret_ಸಂಖ್ಯೆ,	    f_charidx},
    {"chdir",		1, 2, FEARG_1,	    arg2_string,
			"dir, scope",
			ret_string,	    f_chdir},
    {"cindent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ಸಂಖ್ಯೆ,	    f_cindent},
    {"clearmatches",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"win",
			ret_void,	    f_clearmatches},
    {"cmdcomplete_info",0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_ಸಂಖ್ಯೆ,
			"expr, winid",
			ret_ಸಂಖ್ಯೆ,	    f_ಲಂಬಸಾಲು},
    {"complete",	2, 2, FEARG_2,	    arg2_ಸಂಖ್ಯೆ_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_ಸಂಖ್ಯೆ,	    f_complete_ಕೂಡಿಸು},
    {"complete_check",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_ಸಂಖ್ಯೆ_string,
			"msg, choices, default, type",
			ret_ಸಂಖ್ಯೆ,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    ಖಾಲಿ,
			"expr",
			ret_copy,	    f_copy},
    {"cos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cos},
    {"cosh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cosh},
    {"count",		2, 4, FEARG_1,	    arg24_ಎಣಿಕೆ,
			"comp, expr, ic, start",
			ret_ಸಂಖ್ಯೆ,	    f_ಎಣಿಕೆ},
    {"cscope_connection",0,3, 0,	    arg3_ಸಂಖ್ಯೆ_string_string,
			"num, dbpath, prepend",
			ret_ಸಂಖ್ಯೆ,	    f_cscope_connection},
    {"cursor",		1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_ಸಂಖ್ಯೆ,	    f_cursor},
    {"debugbreak",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"pid",
			ret_ಸಂಖ್ಯೆ,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    f_debugbreak
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_ಸಂಖ್ಯೆ_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_ಬಫರ್_lnum_lnum,
			"buf, first, last",
			ret_ಸಂಖ್ಯೆ_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ_bool,    f_did_filetype},
    {"diff",		2, 3, FEARG_1,	    arg3_diff,
			"fromlist, tolist, options",
			ret_string_or_any_3,  f_diff},
    {"diff_filler",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ಸಂಖ್ಯೆ,	    f_diff_filler},
    {"diff_hlID",	2, 2, FEARG_1,	    arg2_lnum_ಸಂಖ್ಯೆ,
			"lnum, col",
			ret_ಸಂಖ್ಯೆ,	    f_diff_hlID},
    {"digraph_get",	1, 1, FEARG_1,	    arg1_string,
			"chars",
			ret_string,	    f_digraph_get},
    {"digraph_getlist",	0, 1, FEARG_1,	    arg1_bool,
			"listall",
			ret_list_string_ವಸ್ತುಗಳು, f_digraph_getlist},
    {"digraph_set",	2, 2, FEARG_1,	    arg2_string,
			"chars, digraph",
			ret_bool,	f_digraph_ಗಣ},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    ಖಾಲಿ,
			"expr",
			ret_ಸಂಖ್ಯೆ_bool,    f_ಖಾಲಿಗಳು},
    {"environ",		0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    ಖಾಲಿ,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ_bool,    f_eventhandler},
    {"executable",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_ಸಂಖ್ಯೆ,	    f_executable},
    {"execute",		1, 2, FEARG_1,	    arg12_execute,
			"command, silent",
			ret_string,	    f_execute},
    {"exepath",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_exepath},
    {"exists",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_ಸಂಖ್ಯೆ_bool,    f_exists},
    {"exists_compiled",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_ಸಂಖ್ಯೆ_bool,    f_exists_compiled},
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
			ret_ಮೊದಲ_cont,	    f_extendnew},
    {"feedkeys",	1, 2, FEARG_1,	    arg2_string,
			"string, mode",
			ret_void,	    f_feedkeys},
    {"file_readable",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"file",
			ret_ಸಂಖ್ಯೆ_bool,    f_filereadable},
    {"filecopy",	2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_ಸಂಖ್ಯೆ_bool,    f_filecopy},
    {"filereadable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_ಸಂಖ್ಯೆ_bool,    f_filereadable},
    {"filewritable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_ಸಂಖ್ಯೆ,	    f_filewritable},
    {"filter",		2, 2, FEARG_1,	    arg2_filter,
			"expr1, expr2",
			ret_ಮೊದಲ_arg,	    f_filter},
    {"finddir",		1, 3, FEARG_1,	    arg3_string_string_ಸಂಖ್ಯೆ,
			"name, path, count",
			ret_string_or_any_3,	    f_finddir},
    {"findfile",	1, 3, FEARG_1,	    arg3_string_string_ಸಂಖ್ಯೆ,
			"name, path, count",
			ret_string_or_any_3,	    f_findfile},
    {"flatten",		1, 2, FEARG_1,	    arg2_list_any_ಸಂಖ್ಯೆ,
			"list, maxdepth",
			ret_list_any,	    f_flatten},
    {"flattennew",	1, 2, FEARG_1,	    arg2_list_any_ಸಂಖ್ಯೆ,
			"list, maxdepth",
			ret_list_any,	    f_flattennew},
    {"float2nr",	1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_ಸಂಖ್ಯೆ,	    f_float2nr},
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
    {"foldclosed",	1, 2, FEARG_1,	    arg2_lnum_ಸಂಖ್ಯೆ,
			"lnum, winid",
			ret_ಸಂಖ್ಯೆ,	    f_foldclosed},
    {"foldclosedend",	1, 2, FEARG_1,	    arg2_lnum_ಸಂಖ್ಯೆ,
			"lnum, winid",
			ret_ಸಂಖ್ಯೆ,	    f_foldclosedend},
    {"foldlevel",	1, 2, FEARG_1,	    arg2_lnum_ಸಂಖ್ಯೆ,
			"lnum, winid",
			ret_ಸಂಖ್ಯೆ,	    f_foldlevel},
    {"foldtext",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_ಸಂಖ್ಯೆ,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_ಮೊದಲ_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
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
    {"getbgcolor",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_list_any,	    f_getbgcolor},
    {"getbufinfo",	0, 1, FEARG_1,	    arg1_ಬಫರ್_or_dict_any,
			"buf",
			ret_list_dict_any,  f_getbufinfo},
    {"getbufline",	2, 3, FEARG_1,	    arg3_ಬಫರ್_lnum_lnum,
			"buf, lnum, end",
			ret_list_string,    f_getbufline},
    {"getbufoneline",	2, 2, FEARG_1,	    arg2_ಬಫರ್_lnum,
			"buf, lnum",
			ret_string,	    f_getbufoneline},
    {"getbufvar",	2, 3, FEARG_1,	    arg3_ಬಫರ್_string_any,
			"buf, varname, def",
			ret_any,	    f_getbufvar},
    {"getcellpixels",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_ಸಂಖ್ಯೆ,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_getcmdwintype},
    {"getcompletion",	2, 3, FEARG_1,	    arg3_string_string_bool,
			"pat, type, filtered",
			ret_list_string,    f_getcompletion},
    {"getcompletiontype", 1, 1, FEARG_1,    arg1_string,
			"pat",
			ret_string,	    f_getcompletiontype},
    {"getcurpos",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"winid",
			ret_list_ಸಂಖ್ಯೆ,    f_getcurpos},
    {"getcursorcharpos", 0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"winid",
			ret_list_ಸಂಖ್ಯೆ,    f_getcursorcharpos},
    {"getcwd",		0, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ,
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
			ret_ಸಂಖ್ಯೆ,	    f_getfsize},
    {"getftime",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_ಸಂಖ್ಯೆ,	    f_getftime},
    {"getftype",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getftype},
    {"getimstatus",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ_bool,    f_getimstatus},
    {"getinfo",		2, 3, FEARG_2,	    arg3_string_string_dict,
			"kind, name, opts",
			ret_dict_any,	    f_getinfo},
    {"getjumplist",	0, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ,
			"winnr, tabnr",
			ret_list_any,	    f_getjumplist},
    {"getline",		1, 2, FEARG_1,	    arg2_lnum,
			"lnum, end",
			ret_getline,	    f_getline},
    {"getloclist",	1, 2, 0,	    arg2_ಸಂಖ್ಯೆ_dict_any,
			"nr, what",
			ret_list_or_dict_1, f_getloclist},
    {"getmarklist",	0, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_ಸಂಖ್ಯೆ,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_dict_ಸಂಖ್ಯೆ,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_getpid},
    {"getpos",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_ಸಂಖ್ಯೆ,    f_getpos},
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
    {"getstacktrace",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_list_dict_any,  f_getstacktrace},
    {"gettabinfo",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"tabnr",
			ret_list_dict_any,  f_gettabinfo},
    {"gettabvar",	2, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ_string_any,
			"tabnr, varname, def",
			ret_any,	    f_gettabvar},
    {"gettabwinvar",	3, 4, FEARG_1,	    arg4_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ_string_any,
			"tabnr, winnr, varname, def",
			ret_any,	    f_gettabwinvar},
    {"gettagstack",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"winnr",
			ret_dict_any,	    f_gettagstack},
    {"gettext",		1, 2, FEARG_1,	    arg2_string,
			"text, package",
			ret_string,	    f_gettext},
    {"getwininfo",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"winid",
			ret_list_dict_any,  f_getwininfo},
    {"getwinpos",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"timeout",
			ret_list_ಸಂಖ್ಯೆ,    f_getwinpos},
    {"getwinposx",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_getwinposy},
    {"getwinvar",	2, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ_string_any,
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
			ret_ಸಂಖ್ಯೆ_bool,    f_has},
    {"has_key",		2, 2, FEARG_1,	    arg2_dict_any_string_or_nr,
			"dict, key",
			ret_ಸಂಖ್ಯೆ_bool,    f_has_ಕೀಲಿ},
    {"haslocaldir",	0, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ,
			"winnr, tabnr",
			ret_ಸಂಖ್ಯೆ,	    f_haslocaldir},
    {"hasmapto",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"what, mode, abbr",
			ret_ಸಂಖ್ಯೆ_bool,    f_hasmapto},
    {"highlightID",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_ಸಂಖ್ಯೆ,	    f_hlID},
    {"highlight_exists",1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_ಸಂಖ್ಯೆ_bool,    f_hlexists},
    {"histadd",		2, 2, FEARG_2,	    arg2_string,
			"history, item",
			ret_ಸಂಖ್ಯೆ_bool,    f_histadd},
    {"histdel",		1, 2, FEARG_1,	    arg2_string_string_or_ಸಂಖ್ಯೆ,
			"history, item",
			ret_ಸಂಖ್ಯೆ_bool,    f_histdel},
    {"histget",		1, 2, FEARG_1,	    arg2_string_ಸಂಖ್ಯೆ,
			"history, index",
			ret_string,	    f_histget},
    {"histnr",		1, 1, FEARG_1,	    arg1_string,
			"history",
			ret_ಸಂಖ್ಯೆ,	    f_histnr},
    {"hlID",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_ಸಂಖ್ಯೆ,	    f_hlID},
    {"hlexists",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_ಸಂಖ್ಯೆ_bool,    f_hlexists},
    {"hlget",		0, 2, FEARG_1,	    arg2_string_bool,
			"name, resolve",
			ret_list_dict_any,  f_hlget},
    {"hlset",		1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_ಸಂಖ್ಯೆ_bool,    f_hlset},
    {"hostname",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    ಖಾಲಿ,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_ಸಂಖ್ಯೆ,	    IMAGE_FUNC(f_image_ಕೂಡಿಸು)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ಸಂಖ್ಯೆ,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_ಸೂಚ್ಯಂಕ,
			"object, expr, start, ic",
			ret_ಸಂಖ್ಯೆ,	    f_ಸೂಚ್ಯಂಕ},
    {"indexof",		2, 3, FEARG_1,	    arg23_ಸೂಚ್ಯಂಕ,
			"object, expr, opts",
			ret_ಸಂಖ್ಯೆ,	    f_indexof},
    {"input",		1, 3, FEARG_1,	    arg3_string,
			"prompt, text, completion",
			ret_string,	    f_ಒಳಹರಿವು},
    {"inputdialog",	1, 3, FEARG_1,	    arg3_string,
			"prompt, text, cancelreturn",
			ret_string,	    f_inputdialog},
    {"inputlist",	1, 1, FEARG_1,	    arg1_list_string,
			"textlist",
			ret_ಸಂಖ್ಯೆ,	    f_inputlist},
    {"inputrestore",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_ಸೇರಿಸು,
			"object, item, idx",
			ret_ಮೊದಲ_arg,	    f_ಸೇರಿಸು},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_void,	    f_ಅಡಚಣೆ},
    {"invert",		1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"expr",
			ret_ಸಂಖ್ಯೆ,	    f_invert},
    {"isabsolutepath",	1, 1, FEARG_1,	    arg1_string,
			"path",
			ret_ಸಂಖ್ಯೆ_bool,    f_isabsolutepath},
    {"isdirectory",	1, 1, FEARG_1,	    arg1_string,
			"directory",
			ret_ಸಂಖ್ಯೆ_bool,    f_isdirectory},
    {"isinf",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_ಸಂಖ್ಯೆ,	    MATH_FUNC(f_isinf)},
    {"islocked",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_ಸಂಖ್ಯೆ_bool,    f_islocked},
    {"isnan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_ಸಂಖ್ಯೆ_bool,    MATH_FUNC(f_isnan)},
    {"items",		1, 1, FEARG_1,	    arg1_list_tuple_dict_blob_or_string,
			"expr",
			ret_list_ವಸ್ತುಗಳು,	    f_ವಸ್ತುಗಳು},
    {"job_getchannel",	1, 1, FEARG_1,	    arg1_ಕೆಲಸ,
			"job",
			ret_channel,	    ಕೆಲಸ_FUNC(f_ಕೆಲಸ_getchannel)},
    {"job_info",	0, 1, FEARG_1,	    arg1_ಕೆಲಸ,
			"job",
			ret_ಕೆಲಸ_info,	    ಕೆಲಸ_FUNC(f_ಕೆಲಸ_info)},
    {"job_setoptions",	2, 2, FEARG_1,	    arg2_ಕೆಲಸ_dict,
			"job, options",
			ret_void,	    ಕೆಲಸ_FUNC(f_ಕೆಲಸ_setoptions)},
    {"job_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"command, options",
			ret_ಕೆಲಸ,	    ಕೆಲಸ_FUNC(f_ಕೆಲಸ_ಆರಂಭ)},
    {"job_status",	1, 1, FEARG_1,	    arg1_ಕೆಲಸ,
			"job",
			ret_string,	    ಕೆಲಸ_FUNC(f_ಕೆಲಸ_ಸ್ಥಿತಿ)},
    {"job_stop",	1, 2, FEARG_1,	    arg2_ಕೆಲಸ_string_or_ಸಂಖ್ಯೆ,
			"job, how",
			ret_ಸಂಖ್ಯೆ_bool,    ಕೆಲಸ_FUNC(f_ಕೆಲಸ_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    ಖಾಲಿ,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    ಖಾಲಿ,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_ಕೀಲಿಗಳು},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    ಖಾಲಿ,	// obsolete
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_ಕೊನೆಯ_ಬಫರ್_nr},
    {"len",		1, 1, FEARG_1,	    arg1_len,
			"expr",
			ret_ಸಂಖ್ಯೆ,	    f_len},
    {"libcall",		3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_string,	    f_libcall},
    {"libcallnr",	3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_ಸಂಖ್ಯೆ,	    f_libcallnr},
    {"line",		1, 2, FEARG_1,	    arg2_string_ಸಂಖ್ಯೆ,
			"expr, winid",
			ret_ಸಂಖ್ಯೆ,	    f_ಸಾಲು},
    {"line2byte",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ಸಂಖ್ಯೆ,	    f_line2byte},
    {"lispindent",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ಸಂಖ್ಯೆ,	    f_lispindent},
    {"list2blob",	1, 1, FEARG_1,	    arg1_list_ಸಂಖ್ಯೆ,
			"list",
			ret_blob,	    f_list2blob},
    {"list2str",	1, 2, FEARG_1,	    arg2_list_ಸಂಖ್ಯೆ_bool,
			"list, utf8",
			ret_string,	    f_list2str},
    {"list2tuple",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_tuple_any,	    f_list2tuple},
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_ಬಫರ್_bool_or_dict,
			"callback, buf, options",
			ret_ಸಂಖ್ಯೆ,	    f_listener_ಕೂಡಿಸು},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_ಸಂಖ್ಯೆ_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_localtime},
    {"log",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_log},
    {"log10",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_log10},
    {"luaeval",		1, 2, FEARG_1,	    arg2_string_any,
			"expr1, expr2",
			ret_any,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LUA
		f_luaeval
#ಇಲ್ಲದಿದ್ದರೆ
		ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"map",		2, 2, FEARG_1,	    arg2_map,
			"expr1, expr2",
			ret_ಮೊದಲ_cont,	    f_map},
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
			ret_ಮೊದಲ_cont,	    f_mapnew},
    {"mapset",		1, 3, FEARG_1,	    arg3_string_or_dict_bool_dict,
			"mode, abbr, dict",
			ret_void,	    f_mapset},
    {"match",		2, 4, FEARG_1,	    arg24_ಹೊಂದಿಕೆ_func,
			"expr, pat, start, count",
			ret_ಸಂಖ್ಯೆ,	    f_ಹೊಂದಿಕೆ},
    {"matchadd",	2, 5, FEARG_1,	    arg25_matchadd,
			"group, pattern, priority, id, dict",
			ret_ಸಂಖ್ಯೆ,	    f_matchadd},
    {"matchaddpos",	2, 5, FEARG_1,	    arg25_matchaddpos,
			"group, pos, priority, id, dict",
			ret_ಸಂಖ್ಯೆ,	    f_matchaddpos},
    {"matcharg",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"nr",
			ret_list_string,    f_matcharg},
    {"matchbufline",	4, 5, FEARG_1,	    arg45_matchbufline,
			"buf, pat, lnum, end, dict",
			ret_list_any,	    f_matchbufline},
    {"matchdelete",	1, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ,
			"id, win",
			ret_ಸಂಖ್ಯೆ_bool,    f_matchdelete},
    {"matchend",	2, 4, FEARG_1,	    arg24_ಹೊಂದಿಕೆ_func,
			"expr, pat, start, count",
			ret_ಸಂಖ್ಯೆ,	    f_matchend},
    {"matchfuzzy",	2, 3, FEARG_1,	    arg3_list_string_dict,
			"list, str, dict",
			ret_list_any,	    f_matchfuzzy},
    {"matchfuzzypos",	2, 3, FEARG_1,	    arg3_list_string_dict,
			"list, str, dict",
			ret_list_list_any,  f_matchfuzzypos},
    {"matchlist",	2, 4, FEARG_1,	    arg24_ಹೊಂದಿಕೆ_func,
			"expr, pat, start, count",
			ret_list_string,    f_matchlist},
    {"matchstr",	2, 4, FEARG_1,	    arg24_ಹೊಂದಿಕೆ_func,
			"expr, pat, start, count",
			ret_string,	    f_matchstr},
    {"matchstrlist",	2, 3, FEARG_1,	    arg23_matchstrlist,
			"list, pat, dict",
			ret_list_any,	    f_matchstrlist},
    {"matchstrpos",	2, 4, FEARG_1,	    arg24_ಹೊಂದಿಕೆ_func,
			"expr, pat, start, count",
			ret_list_any,	    f_matchstrpos},
    {"max",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_max},
    {"menu_info",	1, 2, FEARG_1,	    arg2_string,
			"name, mode",
			ret_dict_any,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	    f_menu_info
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_ಸಂಖ್ಯೆ,
			"name, flags, prot",
			ret_ಸಂಖ್ಯೆ_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_ಬಹುಲಕ},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
	    f_mzeval
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"nextnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ಸಂಖ್ಯೆ,	    f_nextnonblank},
    {"ngettext",	3, 4, FEARG_3,	    arg4_string_string_ಸಂಖ್ಯೆ_string,
			"single, plural, number, domain",
			ret_string,	    f_ngettext},
    {"nr2char",		1, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_bool,
			"expr, utf8",
			ret_string,	    f_nr2char},
    {"or",		2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ,
			"expr1, expr2",
			ret_ಸಂಖ್ಯೆ,	    f_or},
    {"pathshorten",	1, 2, FEARG_1,	    arg2_string_ಸಂಖ್ಯೆ,
			"path, len",
			ret_string,	    f_pathshorten},
    {"perleval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERL
	    f_perleval
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"popup_atcursor",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_popup_atcursor)},
    {"popup_beval",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_popup_beval)},
    {"popup_clear",	0, 1, 0,	    arg1_bool,
			"force",
			ret_void,	    PROP_FUNC(f_popup_clear)},
    {"popup_close",	1, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_any,
			"id, result",
			ret_void,	    PROP_FUNC(f_popup_close)},
    {"popup_create",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_popup_create)},
    {"popup_dialog",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_popup_dialog)},
    {"popup_filter_menu", 2, 2, 0,	    arg2_ಸಂಖ್ಯೆ_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_menu)},
    {"popup_filter_yesno", 2, 2, 0,	    arg2_ಸಂಖ್ಯೆ_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_yesno)},
    {"popup_findecho",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_popup_findpreview)},
    {"popup_getoptions", 1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getoptions)},
    {"popup_getpos",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getpos)},
    {"popup_hide",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_void,	    PROP_FUNC(f_popup_hide)},
    {"popup_list",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_list_ಸಂಖ್ಯೆ,    PROP_FUNC(f_popup_list)},
    {"popup_locate",	2, 2, 0,	    arg2_ಸಂಖ್ಯೆ,
			"row, col",
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_popup_locate)},
    {"popup_menu",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_popup_menu)},
    {"popup_move",	2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_move)},
    {"popup_notification", 2, 2, FEARG_1,   arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_popup_notification)},
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_ಬಫರ್,
			"id, buf",
			ret_ಸಂಖ್ಯೆ_bool,    PROP_FUNC(f_popup_setbuf)},
    {"popup_setoptions", 2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_setoptions)},
    {"popup_settext",	2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_string_or_list,
			"id, text",
			ret_void,	    PROP_FUNC(f_popup_settext)},
    {"popup_show",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_popup_show)},
    {"pow",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"x, y",
			ret_float,	    f_pow},
    {"preinserted",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ಸಂಖ್ಯೆ,	    f_prevnonblank},
    {"printf",		1, 19, FEARG_2,	    arg119_printf,
			"fmt, expr1",
			ret_string,	    f_printf},
    {"prompt_getprompt", 1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_string,	    ಕೆಲಸ_FUNC(f_prompt_getprompt)},
    {"prompt_setcallback", 2, 2, FEARG_1,   arg2_ಬಫರ್_any,
			"buf, expr",
			ret_void,	    ಕೆಲಸ_FUNC(f_prompt_setcallback)},
    {"prompt_setinterrupt", 2, 2, FEARG_1,  arg2_ಬಫರ್_any,
			"buf, expr",
			ret_void,	    ಕೆಲಸ_FUNC(f_prompt_setinterrupt)},
    {"prompt_setprompt", 2, 2, FEARG_1,	    arg2_ಬಫರ್_string,
			"buf, text",
			ret_void,	    ಕೆಲಸ_FUNC(f_prompt_setprompt)},
    {"prop_add",	3, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ_dict,
			"lnum, col, props",
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_prop_ಕೂಡಿಸು)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_ಕೂಡಿಸು_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_ಕಂಡುಹಿಡಿ)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ,
			"props, lnum, lnum_end",
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_prop_remove)},
    {"prop_type_add",	2, 2, FEARG_1,	    arg2_string_dict,
			"name, props",
			ret_void,	    PROP_FUNC(f_prop_type_ಕೂಡಿಸು)},
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
    {"pum_getpos",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_dict_ಸಂಖ್ಯೆ,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
	    f_py3eval
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
	    f_pyeval
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"rand",		0, 1, FEARG_1,	    arg1_list_ಸಂಖ್ಯೆ,
			"expr",
			ret_ಸಂಖ್ಯೆ,	    f_rand},
    {"range",		1, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ,
			"expr, max, stride",
			ret_list_ಸಂಖ್ಯೆ,    f_range},
    {"readblob",	1, 3, FEARG_1,	    arg3_string_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ,
			"fname, offset, size",
			ret_blob,	    f_readblob},
    {"readdir",		1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_string,    f_readdir},
    {"readdirex",	1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_dict_any,  f_readdirex},
    {"readfile",	1, 3, FEARG_1,	    arg3_string_string_ಸಂಖ್ಯೆ,
			"fname, type, max",
			ret_list_string,    f_readfile},
    {"redraw_listener_add", 1, 1, FEARG_1,  arg1_dict_any,
			"opts",
			ret_ಸಂಖ್ಯೆ,	    f_redraw_listener_ಕೂಡಿಸು},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_reg_recording},
    {"reltime",		0, 2, FEARG_1,	    arg2_list_ಸಂಖ್ಯೆ,
			"start, end",
			ret_list_ಸಂಖ್ಯೆ,    f_reltime},
    {"reltimefloat",	1, 1, FEARG_1,	    arg1_list_ಸಂಖ್ಯೆ,
			"time",
			ret_float,	    f_reltimefloat},
    {"reltimestr",	1, 1, FEARG_1,	    arg1_list_ಸಂಖ್ಯೆ,
			"time",
			ret_string,	    f_reltimestr},
    {"remote_expr",	2, 4, FEARG_1,	    arg24_remote_ಅಭಿವ್ಯಕ್ತಿ,
			"server, string, idvar, timeout",
			ret_string,	    f_remote_ಅಭಿವ್ಯಕ್ತಿ},
    {"remote_foreground", 1, 1, FEARG_1,    arg1_string,
			"server",
			ret_void,	    f_remote_foreground},
    {"remote_peek",	1, 2, FEARG_1,	    arg2_string,
			"serverid, retvar",
			ret_ಸಂಖ್ಯೆ,	    f_remote_peek},
    {"remote_read",	1, 2, FEARG_1,	    arg2_string_ಸಂಖ್ಯೆ,
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
			ret_ಸಂಖ್ಯೆ_bool,    f_rename},
    {"repeat",		2, 2, FEARG_1,	    arg2_repeat,
			"expr, count",
			ret_repeat,	    f_repeat},
    {"resolve",		1, 1, FEARG_1,	    arg1_string,
			"filename",
			ret_string,	    f_resolve},
    {"reverse",		1, 1, FEARG_1,	    arg1_ಹಿಮ್ಮುಖ,
			"object",
			ret_ಮೊದಲ_arg,	    f_ಹಿಮ್ಮುಖ},
    {"round",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ಸುತ್ತು},
    {"rubyeval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RUBY
	    f_rubyeval
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"screenattr",	2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ,
			"row, col",
			ret_ಸಂಖ್ಯೆ,	    f_screenattr},
    {"screenchar",	2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ,
			"row, col",
			ret_ಸಂಖ್ಯೆ,	    f_screenchar},
    {"screenchars",	2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ,
			"row, col",
			ret_list_ಸಂಖ್ಯೆ,    f_screenchars},
    {"screencol",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ,
			"winid, lnum, col",
			ret_dict_ಸಂಖ್ಯೆ,    f_screenpos},
    {"screenrow",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_screenrow},
    {"screenstring",	2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ,
			"row, col",
			ret_string,	    f_screenstring},
    {"search",		1, 5, FEARG_1,	    arg15_ಹುಡುಕು,
			"pattern, flags, stopline, timeout, skip",
			ret_ಸಂಖ್ಯೆ,	    f_ಹುಡುಕು},
    {"searchcount",	0, 1, FEARG_1,	    arg1_dict_any,
			"options",
			ret_dict_ಸಂಖ್ಯೆ,    f_searchcount},
    {"searchdecl",	1, 3, FEARG_1,	    arg3_string_bool_bool,
			"name, global, thisblock",
			ret_ಸಂಖ್ಯೆ_bool,    f_searchdecl},
    {"searchpair",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_ಸಂಖ್ಯೆ,	    f_searchpair},
    {"searchpairpos",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_list_ಸಂಖ್ಯೆ,    f_searchpairpos},
    {"searchpos",	1, 5, FEARG_1,	    arg15_ಹುಡುಕು,
			"pattern, flags, stopline, timeout, skip",
			ret_list_ಸಂಖ್ಯೆ,    f_searchpos},
    {"server2client",	2, 2, FEARG_1,	    arg2_string,
			"clientid, string",
			ret_ಸಂಖ್ಯೆ_bool,    f_server2client},
    {"serverlist",	0, 1, 0,	    arg1_dict_any,
			"dict",
			ret_string_or_any_1,	    f_serverlist},
    {"setbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_ಸಂಖ್ಯೆ_bool,    f_setbufline},
    {"setbufvar",	3, 3, FEARG_3,	    arg3_ಬಫರ್_string_any,
			"buf, varname, val",
			ret_void,	    f_setbufvar},
    {"setcellwidths",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_void,	    f_setcellwidths},
    {"setcharpos",	2, 2, FEARG_2,	    arg2_string_list_ಸಂಖ್ಯೆ,
			"expr, list",
			ret_ಸಂಖ್ಯೆ_bool,    f_setcharpos},
    {"setcharsearch",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_setcharsearch},
    {"setcmdline",	1, 2, FEARG_1,	    arg2_string_ಸಂಖ್ಯೆ,
			"str, pos",
			ret_ಸಂಖ್ಯೆ_bool,    f_setcmdline},
    {"setcmdpos",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"pos",
			ret_ಸಂಖ್ಯೆ_bool,    f_setcmdpos},
    {"setcursorcharpos", 1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_ಸಂಖ್ಯೆ_bool,    f_setcursorcharpos},
    {"setenv",		2, 2, FEARG_2,	    arg2_string_any,
			"name, val",
			ret_void,	    f_setenv},
    {"setfperm",	2, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_ಸಂಖ್ಯೆ_bool,    f_setfperm},
    {"setline",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_ಸಂಖ್ಯೆ_bool,    f_setline},
    {"setloclist",	2, 4, FEARG_2,	    arg24_setloclist,
			"nr, list, action, what",
			ret_ಸಂಖ್ಯೆ_bool,    f_setloclist},
    {"setmatches",	1, 2, FEARG_1,	    arg2_list_any_ಸಂಖ್ಯೆ,
			"list, win",
			ret_ಸಂಖ್ಯೆ_bool,    f_setmatches},
    {"setpos",		2, 2, FEARG_2,	    arg2_string_list_ಸಂಖ್ಯೆ,
			"expr, list",
			ret_ಸಂಖ್ಯೆ_bool,    f_setpos},
    {"setqflist",	1, 3, FEARG_1,	    arg13_setqflist,
			"list, action, what",
			ret_ಸಂಖ್ಯೆ_bool,    f_setqflist},
    {"setreg",		2, 3, FEARG_2,	    arg3_string_any_string,
			"regname, value, options",
			ret_ಸಂಖ್ಯೆ_bool,    f_setreg},
    {"settabvar",	3, 3, FEARG_3,	    arg3_ಸಂಖ್ಯೆ_string_any,
			"tabnr, varname, val",
			ret_void,	    f_settabvar},
    {"settabwinvar",	4, 4, FEARG_4,	    arg4_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ_string_any,
			"tabnr, winnr, varname, val",
			ret_void,	    f_settabwinvar},
    {"settagstack",	2, 3, FEARG_2,	    arg23_settagstack,
			"nr, dict, action",
			ret_ಸಂಖ್ಯೆ_bool,    f_settagstack},
    {"setwinvar",	3, 3, FEARG_3,	    arg3_ಸಂಖ್ಯೆ_string_any,
			"winnr, varname, val",
			ret_void,	    f_setwinvar},
    {"sha256",		1, 1, FEARG_1,	    arg1_string_or_blob,
			"expr",
			ret_string,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	    f_sha256
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"shellescape",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, special",
			ret_string,	    f_shellescape},
    {"shiftwidth",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"col",
			ret_ಸಂಖ್ಯೆ,	    f_shiftwidth},
    {"sign_define",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"name, dict",
			ret_sign_define,	    SIGN_FUNC(f_sign_define)},
    {"sign_getdefined",	0, 1, FEARG_1,	    arg1_string,
			"name",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getdefined)},
    {"sign_getplaced",	0, 2, FEARG_1,	    arg02_sign_getplaced,
			"buf, dict",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getplaced)},
    {"sign_jump",	3, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ_string_ಬಫರ್,
			"id, group, buf",
			ret_ಸಂಖ್ಯೆ,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_ಇರಿಸು,
			"id, group, name, buf, dict",
			ret_ಸಂಖ್ಯೆ,	    SIGN_FUNC(f_sign_ಇರಿಸು)},
    {"sign_placelist",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_ಸಂಖ್ಯೆ,    SIGN_FUNC(f_sign_placelist)},
    {"sign_undefine",	0, 1, FEARG_1,	    arg1_string_or_list_string,
			"name",
			ret_ಸಂಖ್ಯೆ_bool,    SIGN_FUNC(f_sign_undefine)},
    {"sign_unplace",	1, 2, FEARG_1,	    arg2_string_dict,
			"group, dict",
			ret_ಸಂಖ್ಯೆ_bool,    SIGN_FUNC(f_sign_unplace)},
    {"sign_unplacelist", 1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_ಸಂಖ್ಯೆ,    SIGN_FUNC(f_sign_unplacelist)},
    {"simplify",	1, 1, FEARG_1,	    arg1_string,
			"filename",
			ret_string,	    f_simplify},
    {"sin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sin},
    {"sinh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sinh},
    {"slice",		2, 3, FEARG_1,	    arg23_ತುಂಡು,
			"expr, start, end",
			ret_ತುಂಡು,	    f_ತುಂಡು},
    {"sort",		1, 3, FEARG_1,	    arg13_sortuniq,
			"list, how, dict",
			ret_ಮೊದಲ_arg,	    f_ವಿಂಗಡಿಸು},
    {"sound_clear",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_void,	    SOUND_FUNC(f_sound_clear)},
    {"sound_playevent",	1, 2, FEARG_1,	    arg2_string_any,
			"name, callback",
			ret_ಸಂಖ್ಯೆ,	    SOUND_FUNC(f_sound_playevent)},
    {"sound_playfile",	1, 2, FEARG_1,	    arg2_string_any,
			"path, callback",
			ret_ಸಂಖ್ಯೆ,	    SOUND_FUNC(f_sound_playfile)},
    {"sound_stop",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_void,	    SOUND_FUNC(f_sound_stop)},
    {"soundfold",	1, 1, FEARG_1,	    arg1_string,
			"word",
			ret_string,	    f_soundfold},
    {"spellbadword",	0, 1, FEARG_1,	    arg1_string,
			"sentence",
			ret_list_string,    f_spellbadword},
    {"spellsuggest",	1, 3, FEARG_1,	    arg3_string_ಸಂಖ್ಯೆ_bool,
			"word, max, capital",
			ret_list_string,    f_spellsuggest},
    {"split",		1, 3, FEARG_1,	    arg3_string_string_bool,
			"string, pattern, keepempty",
			ret_list_string,    f_ವಿಭಜಿಸು},
    {"sqrt",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sqrt},
    {"srand",		0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"expr",
			ret_list_ಸಂಖ್ಯೆ,    f_srand},
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
			ret_list_ಸಂಖ್ಯೆ,    f_str2list},
    {"str2nr",		1, 3, FEARG_1,	    arg3_string_ಸಂಖ್ಯೆ_bool,
			"string, base, quoted",
			ret_ಸಂಖ್ಯೆ,	    f_str2nr},
    {"strcharlen",	1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_ಸಂಖ್ಯೆ,	    f_strcharlen},
    {"strcharpart",	2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, skipcc",
			ret_string,	    f_strcharpart},
    {"strchars",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, skipcc",
			ret_ಸಂಖ್ಯೆ,	    f_strchars},
    {"strdisplaywidth",	1, 2, FEARG_1,	    arg2_string_ಸಂಖ್ಯೆ,
			"string, col",
			ret_ಸಂಖ್ಯೆ,	    f_strdisplaywidth},
    {"strftime",	1, 2, FEARG_1,	    arg2_string_ಸಂಖ್ಯೆ,
			"format, time",
			ret_string,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRFTIME
	    f_strftime
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_ಸಂಖ್ಯೆ,
			"str, index",
			ret_ಸಂಖ್ಯೆ,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_ಸಂಖ್ಯೆ,
			"haystack, needle, start",
			ret_ಸಂಖ್ಯೆ,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, ಖಾಲಿ,
			"expr",
			ret_string,	    f_string},
    {"strlen",		1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_ಸಂಖ್ಯೆ,	    f_strlen},
    {"strpart",		2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, chars",
			ret_string,	    f_strpart},
    {"strptime",	2, 2, FEARG_1,	    arg2_string,
			"format, timestring",
			ret_ಸಂಖ್ಯೆ,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRPTIME
	    f_strptime
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"strridx",		2, 3, FEARG_1,	    arg3_string_string_ಸಂಖ್ಯೆ,
			"haystack, needle, start",
			ret_ಸಂಖ್ಯೆ,	    f_strridx},
    {"strtrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_strtrans},
    {"strutf16len",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, countcc",
			ret_ಸಂಖ್ಯೆ,	    f_strutf16len},
    {"strwidth",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_ಸಂಖ್ಯೆ,	    f_strwidth},
    {"submatch",	1, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_bool,
			"nr, list",
			ret_string_or_any_2,	    f_submatch},
    {"substitute",	4, 4, FEARG_1,	    arg4_string_string_any_string,
			"string, pat, sub, flags",
			ret_string,	    f_substitute},
    {"swapfilelist",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_string,	    f_swapname},
    {"synID",		3, 3, 0,	    arg3_lnum_ಸಂಖ್ಯೆ_bool,
			"lnum, col, trans",
			ret_ಸಂಖ್ಯೆ,	    f_synID},
    {"synIDattr",	2, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ_string_string,
			"synID, what, mode",
			ret_string,	    f_synIDattr},
    {"synIDtrans",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"synID",
			ret_ಸಂಖ್ಯೆ,	    f_synIDtrans},
    {"synconcealed",	2, 2, 0,	    arg2_lnum_ಸಂಖ್ಯೆ,
			"lnum, col",
			ret_list_any,	    f_synconcealed},
    {"synstack",	2, 2, 0,	    arg2_lnum_ಸಂಖ್ಯೆ,
			"lnum, col",
			ret_list_ಸಂಖ್ಯೆ,    f_synstack},
    {"system",		1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_string,	    f_system},
    {"systemlist",	1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_list_string,    f_systemlist},
    {"tabpagebuflist",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"arg",
			ret_list_ಸಂಖ್ಯೆ,    f_tabpagebuflist},
    {"tabpagenr",	0, 1, 0,	    arg1_string,
			"arg",
			ret_ಸಂಖ್ಯೆ,	    f_tabpagenr},
    {"tabpagewinnr",	1, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_string,
			"tabarg, arg",
			ret_ಸಂಖ್ಯೆ,	    f_tabpagewinnr},
    {"tabpanel_getinfo", 0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
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
    {"tempname",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_ಸಂಖ್ಯೆ,	    ಪದ_FUNC(f_ಪದ_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_ಸಂಖ್ಯೆ,	    ಪದ_FUNC(f_ಪದ_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_ಬಫರ್_string_dict,
			"buf, filename, options",
			ret_void,	    ಪದ_FUNC(f_ಪದ_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_ಬಫರ್,
			"buf",
			ret_ಸಂಖ್ಯೆ,	    ಪದ_FUNC(f_ಪದ_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_ಬಫರ್,
			"buf",
			ret_list_string,
#ಆದರೆ defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_ಪದ_getansicolors
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_string,
			"attr, what",
			ret_ಸಂಖ್ಯೆ,	    ಪದ_FUNC(f_ಪದ_getattr)},
    {"term_getcursor",	1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_list_any,	    ಪದ_FUNC(f_ಪದ_getcursor)},
    {"term_getjob",	1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_ಕೆಲಸ,	    ಪದ_FUNC(f_ಪದ_getjob)},
    {"term_getline",	2, 2, FEARG_1,	    arg2_ಬಫರ್_lnum,
			"buf, row",
			ret_string,	    ಪದ_FUNC(f_ಪದ_getline)},
    {"term_getscrolled", 1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_ಸಂಖ್ಯೆ,	    ಪದ_FUNC(f_ಪದ_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_list_ಸಂಖ್ಯೆ,    ಪದ_FUNC(f_ಪದ_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_string,	    ಪದ_FUNC(f_ಪದ_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_string,	    ಪದ_FUNC(f_ಪದ_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_ಬಫರ್_bool,
			"buf, input",
			ret_string,	    ಪದ_FUNC(f_ಪದ_gettty)},
    {"term_list",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_list_ಸಂಖ್ಯೆ,    ಪದ_FUNC(f_ಪದ_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_ಬಫರ್_lnum,
			"buf, row",
			ret_list_dict_any,  ಪದ_FUNC(f_ಪದ_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_ಬಫರ್_string,
			"buf, keys",
			ret_void,	    ಪದ_FUNC(f_ಪದ_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_ಬಫರ್_list_any,
			"buf, colors",
			ret_void,
#ಆದರೆ defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_ಪದ_setansicolors
#ಇಲ್ಲದಿದ್ದರೆ
	    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			},
    {"term_setapi",	2, 2, FEARG_1,	    arg2_ಬಫರ್_string,
			"buf, expr",
			ret_void,	    ಪದ_FUNC(f_ಪದ_setapi)},
    {"term_setkill",	2, 2, FEARG_1,	    arg2_ಬಫರ್_string,
			"buf, how",
			ret_void,	    ಪದ_FUNC(f_ಪದ_setkill)},
    {"term_setrestore",	2, 2, FEARG_1,	    arg2_ಬಫರ್_string,
			"buf, command",
			ret_void,	    ಪದ_FUNC(f_ಪದ_setrestore)},
    {"term_setsize",	3, 3, FEARG_1,	    arg3_ಬಫರ್_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ,
			"buf, rows, cols",
			ret_void,	    ಪದ_FUNC(f_ಪದ_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_ಸಂಖ್ಯೆ,	    ಪದ_FUNC(f_ಪದ_ಆರಂಭ)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_ಬಫರ್_ಸಂಖ್ಯೆ,
			"buf, time",
			ret_void,	    ಪದ_FUNC(f_ಪದ_wait)},
    {"terminalprops",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   ಖಾಲಿ,
			ಖಾಲಿ,
			ret_void,	    f_test_garbagecollect_soon},
    {"test_getvalue",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_ಸಂಖ್ಯೆ,	    f_test_getvalue},
    {"test_gui_event",	2, 2, FEARG_1,	    arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_gui_event},
    {"test_ignore_error", 1, 1, FEARG_1,    arg1_string,
			"expr",
			ret_void,	    f_test_ignore_error},
    {"test_mswin_event", 2, 2, FEARG_1,     arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_mswin_event},
    {"test_null_blob",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_channel,	    ಕೆಲಸ_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಕೆಲಸ,	    ಕೆಲಸ_FUNC(f_test_null_ಕೆಲಸ)},
    {"test_null_list",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_ಗಣ},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_ಸಂಖ್ಯೆ,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, ಖಾಲಿ,
			"expr",
			ret_ಸಂಖ್ಯೆ,	    f_test_refcount},
    {"test_setmouse",	2, 2, 0,	    arg2_ಸಂಖ್ಯೆ,
			"row, col",
			ret_void,	    f_test_setmouse},
    {"test_settime",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"expr",
			ret_void,	    f_test_settime},
    {"test_srand_seed",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"seed",
			ret_void,	    f_test_srand_ಬೀಜ},
    {"test_unknown",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ_any_dict,
			"time, callback, options",
			ret_ಸಂಖ್ಯೆ,	    TIMER_FUNC(f_timer_ಆರಂಭ)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
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
    {"trim",		1, 3, FEARG_1,	    arg3_string_string_ಸಂಖ್ಯೆ,
			"text, mask, dir",
			ret_string,	    f_trim},
    {"trunc",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_trunc},
    {"tuple2list",	1, 1, FEARG_1,	    arg1_tuple_any,
			"tuple",
			ret_list_any,	    f_tuple2list},
    {"type",		1, 1, FEARG_1|FE_X, ಖಾಲಿ,
			"expr",
			ret_ಸಂಖ್ಯೆ,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, ಖಾಲಿ,
			"expr",
			ret_string,	    f_typename},
    {"undofile",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_string,	    f_undofile},
    {"undotree",	0, 1, FEARG_1,	    arg1_ಬಫರ್,
			"buf",
			ret_dict_any,	    f_undotree},
    {"uniq",		1, 3, FEARG_1,	    arg13_sortuniq,
			"list, func, dict",
			ret_ಮೊದಲ_arg,	    f_uniq},
    {"uri_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_uridecode},
    {"uri_encode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_uriencode},
    {"utf16idx",	2, 4, FEARG_1,	    arg4_string_ಸಂಖ್ಯೆ_bool_bool,
			"string, idx, countcc, charidx",
			ret_ಸಂಖ್ಯೆ,	    f_utf16idx},
    {"values",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_member,    f_ಮೌಲ್ಯಗಳು},
    {"virtcol",		1, 3, FEARG_1,	    arg3_string_or_list_bool_ಸಂಖ್ಯೆ,
			"expr, list, winid",
			ret_virtcol,	    f_virtcol},
    {"virtcol2col",	3, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ,
			"winid, lnum, col",
			ret_ಸಂಖ್ಯೆ,	    f_virtcol2col},
    {"visualmode",	0, 1, 0,	    arg1_bool,
			"expr",
			ret_string,	    f_visualmode},
    {"wildmenumode",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_void,	    f_wildtrigger},
    {"win_execute",	2, 3, FEARG_2,	    arg23_win_execute,
			"id, command, silent",
			ret_string,	    f_win_execute},
    {"win_findbuf",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"bufnr",
			ret_list_ಸಂಖ್ಯೆ,    f_win_findbuf},
    {"win_getid",	0, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ,
			"win, tab",
			ret_ಸಂಖ್ಯೆ,	    f_win_getid},
    {"win_gettype",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"nr",
			ret_string,	    f_win_gettype},
    {"win_gotoid",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"expr",
			ret_ಸಂಖ್ಯೆ_bool,    f_win_gotoid},
    {"win_id2tabwin",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"expr",
			ret_list_ಸಂಖ್ಯೆ,    f_win_id2tabwin},
    {"win_id2win",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"expr",
			ret_ಸಂಖ್ಯೆ,	    f_win_id2win},
    {"win_move_separator", 2, 2, FEARG_1,   arg2_ಸಂಖ್ಯೆ,
			"nr, offset",
			ret_ಸಂಖ್ಯೆ_bool,    f_win_move_separator},
    {"win_move_statusline", 2, 2, FEARG_1,  arg2_ಸಂಖ್ಯೆ,
			"nr, offset",
			ret_ಸಂಖ್ಯೆ_bool,    f_win_move_statusline},
    {"win_screenpos",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"nr",
			ret_list_ಸಂಖ್ಯೆ,    f_win_screenpos},
    {"win_splitmove",   2, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ_dict,
			"nr, target, options",
			ret_ಸಂಖ್ಯೆ_bool,    f_win_splitmove},
    {"winbufnr",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"nr",
			ret_ಸಂಖ್ಯೆ,	    f_winbufnr},
    {"wincol",		0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"nr",
			ret_ಸಂಖ್ಯೆ,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_ಸಂಖ್ಯೆ,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_ಸಂಖ್ಯೆ,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_dict_ಸಂಖ್ಯೆ,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"nr",
			ret_ಸಂಖ್ಯೆ,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    ಖಾಲಿ,
			ಖಾಲಿ,
			ret_dict_ಸಂಖ್ಯೆ,    f_wordcount},
    {"writefile",	2, 3, FEARG_1,	    arg23_writefile,
			"object, fname, flags",
			ret_ಸಂಖ್ಯೆ_bool,    f_writefile},
    {"xor",		2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ,
			"expr1, expr2",
			ret_ಸಂಖ್ಯೆ,	    f_xor},
};

/*
 * Return true if specified function allows a type as an argument.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
func_allows_type(ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, ಪೂರ್ಣಾಂಕ idx)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	intidx = -1;
    char_u	*name;

    ಆದರೆ (idx == 0)
	intidx = -1;
    ಆದರೆ (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	ಆದರೆ (name != ಖಾಲಿ)
	{
	    ಆದರೆ (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_ನಮೂನೆ, 2) == 0)
		ಹಿಂತಿರುಗಿ cat_ಪೂರ್ವಪ್ರತ್ಯಯ_varname('g', name);
	    ಹಿಂತಿರುಗಿ name;
	}
    }
    ಆದರೆ (++intidx < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	ಆದರೆ (global_functions[intidx].f_func == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	ಆದರೆ (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	ಹಿಂತಿರುಗಿ IObuff;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_ಅಭಿವ್ಯಕ್ತಿ_name(expand_T *xp, ಪೂರ್ಣಾಂಕ idx)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	intidx = -1;
    char_u	*name;

    ಆದರೆ (idx == 0)
	intidx = -1;
    ಆದರೆ (intidx < 0)
    {
	name = get_function_name(xp, idx);
	ಆದರೆ (name != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ name;
    }
    ಹಿಂತಿರುಗಿ get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func_opt(char_u *name, ಪೂರ್ಣಾಂಕ implemented)
{
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ = 0;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ;
    ಪೂರ್ಣಾಂಕ		cmp;
    ಪೂರ್ಣಾಂಕ		x;

    ಕೊನೆಯ = (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(global_functions) - 1;

    // Find the function name in the table. Binary search.
    ಆಗ (ಮೊದಲ <= ಕೊನೆಯ)
    {
	x = ಮೊದಲ + ((ಚಿಹ್ನೆರಹಿತ)(ಕೊನೆಯ - ಮೊದಲ) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	ಆದರೆ (cmp < 0)
	    ಕೊನೆಯ = x - 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmp > 0)
	    ಮೊದಲ = x + 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (implemented && global_functions[x].f_func == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ x;
    }
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func(char_u *name)
{
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func_opt(name, TRUE);
}

    ಪೂರ್ಣಾಂಕ
has_ಆಂತರಿಕ_func(char_u *name)
{
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func_opt(name, TRUE) >= 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
has_ಆಂತರಿಕ_func_name(char_u *name)
{
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func_opt(name, FALSE) >= 0;
}

    ಅಕ್ಷರ *
ಆಂತರಿಕ_func_name(ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    ಪೂರ್ಣಾಂಕ
ಆಂತರಿಕ_func_check_arg_types(
	type2_T	*types,
	ಪೂರ್ಣಾಂಕ	idx,
	ಪೂರ್ಣಾಂಕ	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    ಆದರೆ (!(func_allows_type(idx)))
    {
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < argcount; ++i)
	    ಆದರೆ (check_type_is_ಮೌಲ್ಯ(types[i].type_curr) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
    }

    ಸ್ಥಿರಾಂಕ argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    ಆದರೆ (argchecks == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    argcontext_T context;

    context.arg_ಎಣಿಕೆ = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < argcount && argchecks[i] != ಖಾಲಿ; ++i)
    {
	context.arg_idx = i;
	ಆದರೆ (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    ಶೂನ್ಯ
ಆಂತರಿಕ_func_get_argcount(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ *argcount, ಪೂರ್ಣಾಂಕ *min_argcount)
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
ಆಂತರಿಕ_func_ret_type(
	ಪೂರ್ಣಾಂಕ	    idx,
	ಪೂರ್ಣಾಂಕ	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = ಖಾಲಿ;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    ಆದರೆ (*decl_type == ಖಾಲಿ)
	*decl_type = ret;
    current_type_gap = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    ಪೂರ್ಣಾಂಕ
ಆಂತರಿಕ_func_is_map(ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    ಪೂರ್ಣಾಂಕ
check_ಆಂತರಿಕ_func(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ argcount)
{
    funcerror_T	    res;
    ಅಕ್ಷರ	    *name;

    ಆದರೆ (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ global_functions[idx].f_argtype & FEARG_ಮುಖವಾಡ;

    name = ಆಂತರಿಕ_func_name(idx);
    ಆದರೆ (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_not_enough_arguments_for_function_str), name);
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_ವಾದಗಳು_for_type(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ argcount, typval_T *argvars)
{
    ಆದರೆ (!func_allows_type(idx))
    {
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < argcount; ++i)
	    ಆದರೆ (check_typval_is_ಮೌಲ್ಯ(&argvars[i]) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

    funcerror_T
call_ಆಂತರಿಕ_func(
	char_u	    *name,
	ಪೂರ್ಣಾಂಕ	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    ಪೂರ್ಣಾಂಕ i;

    i = ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func(name);
    ಆದರೆ (i < 0)
	ಹಿಂತಿರುಗಿ FCERR_UNKNOWN;
    ಆದರೆ (argcount < global_functions[i].f_min_argc)
	ಹಿಂತಿರುಗಿ FCERR_TOOFEW;
    ಆದರೆ (argcount > global_functions[i].f_max_argc)
	ಹಿಂತಿರುಗಿ FCERR_TOOMANY;
    ಆದರೆ (check_ವಾದಗಳು_for_type(i, argcount, argvars) == FAIL)
	ಹಿಂತಿರುಗಿ FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    ಆದರೆ (ಒಳ_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    ಹಿಂತಿರುಗಿ FCERR_NONE;
}

    ಶೂನ್ಯ
call_ಆಂತರಿಕ_func_by_idx(
	ಪೂರ್ಣಾಂಕ	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    ಆದರೆ (ಒಳ_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_ಆಂತರಿಕ_ವಿಧಾನ(
	char_u	    *name,
	ಪೂರ್ಣಾಂಕ	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    ಪೂರ್ಣಾಂಕ		fi;
    typval_T	argv[MAX_FUNC_ವಾದಗಳು + 1];

    fi = ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func(name);
    ಆದರೆ (fi < 0)
	ಹಿಂತಿರುಗಿ FCERR_UNKNOWN;
    ಆದರೆ ((global_functions[fi].f_argtype & FEARG_ಮುಖವಾಡ) == 0)
	ಹಿಂತಿರುಗಿ FCERR_NOTMETHOD;
    ಆದರೆ (argcount + 1 < global_functions[fi].f_min_argc)
	ಹಿಂತಿರುಗಿ FCERR_TOOFEW;
    ಆದರೆ (argcount + 1 > global_functions[fi].f_max_argc)
	ಹಿಂತಿರುಗಿ FCERR_TOOMANY;
    ಆದರೆ (check_ವಾದಗಳು_for_type(fi, argcount, argvars) == FAIL)
	ಹಿಂತಿರುಗಿ FCERR_OTHER;

    ಆದರೆ ((global_functions[fi].f_argtype & FEARG_ಮುಖವಾಡ) == FEARG_2)
    {
	ಆದರೆ (argcount < 1)
	    ಹಿಂತಿರುಗಿ FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((global_functions[fi].f_argtype & FEARG_ಮುಖವಾಡ) == FEARG_3)
    {
	ಆದರೆ (argcount < 2)
	    ಹಿಂತಿರುಗಿ FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((global_functions[fi].f_argtype & FEARG_ಮುಖವಾಡ) == FEARG_4)
    {
	ಆದರೆ (argcount < 3)
	    ಹಿಂತಿರುಗಿ FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    ಆದರೆ (check_ವಾದಗಳು_for_type(fi, argcount + 1, argv) == FAIL)
	ಹಿಂತಿರುಗಿ FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    ಹಿಂತಿರುಗಿ FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    ಪೂರ್ಣಾಂಕ
non_zero_arg(typval_T *argvars)
{
    ಹಿಂತಿರುಗಿ ((argvars[0].v_type == VAR_ಸಂಖ್ಯೆ
		&& argvars[0].vval.v_ಸಂಖ್ಯೆ != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_ಸಂಖ್ಯೆ == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != ಖಾಲಿ
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_and(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ)
					& tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ಖಾಲಿ);
}

/*
 * "balloon_show()" function
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL
    ಸ್ಥಿರ ಶೂನ್ಯ
f_balloon_gettext(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    ಆದರೆ (balloonEval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (balloonEval->ಸಂದೇಶ == ಖಾಲಿ)
	rettv->vval.v_string = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_string = vim_strsave(balloonEval->ಸಂದೇಶ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
f_balloon_show(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (balloonEval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.ಒಳ_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, ಖಾಲಿ,
		l == ಖಾಲಿ || l->lv_len == 0 ? ಖಾಲಿ : l);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u *mesg;

	ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    ಹಿಂತಿರುಗಿ;

	mesg = tv_get_string_chk(&argvars[0]);
	ಆದರೆ (mesg != ಖಾಲಿ)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? ಖಾಲಿ : mesg, ಖಾಲಿ);
    }
}

# ಆದರೆ defined(FEAT_BEVAL_ಪದ)
    ಸ್ಥಿರ ಶೂನ್ಯ
f_balloon_ವಿಭಜಿಸು(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (rettv_list_alloc(rettv) != OK)
	ಹಿಂತಿರುಗಿ;

    char_u *ಸಂದೇಶ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಸಂದೇಶ = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (ಸಂದೇಶ != ಖಾಲಿ)
    {
	pumitem_T	*array;
	ಪೂರ್ಣಾಂಕ		ಗಾತ್ರ_2 = ವಿಭಜಿಸು_ಸಂದೇಶ(ಸಂದೇಶ, &array);

	// Skip the first and last item, they are always empty.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < ಗಾತ್ರ_2 - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_ಪಠ್ಯ, -1);
	ಆಗ (ಗಾತ್ರ_2 > 0)
	    vim_free(array[--ಗಾತ್ರ_2].pum_ಪಠ್ಯ);
	vim_free(array);
    }
}
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * "base64_decode(string)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (rettv_blob_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (str != ಖಾಲಿ)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (check_for_blob_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    blob_T *blob = argvars->vval.v_blob;
    ಆದರೆ (blob != ಖಾಲಿ)
	rettv->vval.v_string =
	    base64_encode(blob->bv_ga.ga_ದತ್ತಾಂಶ, blob->bv_ga.ga_len);
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
    ಆದರೆ (buf == ಖಾಲಿ)
	semsg(_(e_invalid_ಬಫರ್_name_str), tv_get_string(arg));
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;

    ಆದರೆ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (strcmp((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_ವಾದ_str), tv_get_string(&argvars[0]));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (bindtextdomain((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[0].vval.v_string, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[1].vval.v_string) == ಖಾಲಿ)
	{
	    do_outofmem_ಸಂದೇಶ((ದೀರ್ಘ)0);
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;
	}
    }

    ಹಿಂತಿರುಗಿ;
}

/*
 * "byte2line(byte)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_byte2line(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_BYTEOFF
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
#ಇಲ್ಲದಿದ್ದರೆ
    ದೀರ್ಘ	boff = 0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    boff = tv_get_ಸಂಖ್ಯೆ(&argvars[0]) - 1;  // boff gets -1 on type error
    ಆದರೆ (boff < 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು_or_ಸ್ಥಾನಾಂತರ(curbuf,
							  (linenr_T)0, &boff);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "call(func, arglist [, dict])" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = ಖಾಲಿ;
    dict_T	*selfdict = ಖಾಲಿ;
    char_u	*tofree = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_list_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (argvars[1].vval.v_list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	func = tv_get_string(&argvars[0]);
    ಆದರೆ (func == ಖಾಲಿ || *func == NUL)
	ಹಿಂತಿರುಗಿ;		// type error, empty name or null function

    ಆದರೆ (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, ಖಾಲಿ, FALSE, TFN_INT|TFN_QUIET);
	ಆದರೆ (tofree == ಖಾಲಿ)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_ವಾದಗಳು(tofree, STRLEN(tofree),
									&p);
	    ಆದರೆ (s != ಖಾಲಿ)
	    {
		vim_free(tofree);
		tofree = s;
	    }
	}
	func = tofree;
    }

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಆದರೆ (check_for_dict_arg(argvars, 2) == FAIL)
	    ಹೋಗು ಮುಗಿದ;

	selfdict = argvars[2].vval.v_dict;
    }

    (ಶೂನ್ಯ)func_call(func, &argvars[1], partial, selfdict, rettv);

ಮುಗಿದ:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_changenr(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (has_mbyte)
    {
	ಪೂರ್ಣಾಂಕ	utf8 = 0;

	ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[1], ಖಾಲಿ);

	ಆದರೆ (utf8)
	    rettv->vval.v_ಸಂಖ್ಯೆ = utf_ptr2char(tv_get_string(&argvars[0]));
	ಇಲ್ಲದಿದ್ದರೆ
	    rettv->vval.v_ಸಂಖ್ಯೆ = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ಲಂಬಸಾಲು(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ charcol)
{
    colnr_T	ಲಂಬಸಾಲು = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    ಪೂರ್ಣಾಂಕ		winchanged = FALSE;

    ಆದರೆ (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]), &tp);
	ಆದರೆ (wp == ಖಾಲಿ || tp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	ಆದರೆ (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    ಹಿಂತಿರುಗಿ;

	check_cursor();
	winchanged = TRUE;
    }

    ಪೂರ್ಣಾಂಕ fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    ಆದರೆ (fp != ಖಾಲಿ && fnum == curbuf->b_fnum)
    {
	ಆದರೆ (fp->ಲಂಬಸಾಲು == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    ಆದರೆ (fp->lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಲಂಬಸಾಲು = ml_get_len(fp->lnum) + 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಲಂಬಸಾಲು = MAXCOL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಲಂಬಸಾಲು = fp->ಲಂಬಸಾಲು + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    ಆದರೆ (virtual_ಸಕ್ರಿಯ() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		ಆದರೆ (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    ಪೂರ್ಣಾಂಕ		l;

		    ಆದರೆ (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			ಲಂಬಸಾಲು += l;
		}
	    }
	}
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು;

    ಆದರೆ (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_ಲಂಬಸಾಲು(argvars, rettv, TRUE);
}

    win_T *
get_optional_ಕಿಟಕಿ(typval_T *argvars, ಪೂರ್ಣಾಂಕ idx)
{
    win_T   *win = curwin;

    ಆದರೆ (argvars[idx].v_type == VAR_UNKNOWN)
	ಹಿಂತಿರುಗಿ curwin;

    win = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[idx]);
    ಆದರೆ (win == ಖಾಲಿ)
    {
	emsg(_(e_invalid_ಕಿಟಕಿ_ಸಂಖ್ಯೆ));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ win;
}

/*
 * "col(string)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ಲಂಬಸಾಲು(typval_T *argvars, typval_T *rettv)
{
    get_ಲಂಬಸಾಲು(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_confirm(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*ಸಂದೇಶ_2;
    char_u	*buttons = ಖಾಲಿ;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		def = 1;
    ಪೂರ್ಣಾಂಕ		type = VIM_GENERIC;
    char_u	*typestr;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	ಹಿಂತಿರುಗಿ;

    ಸಂದೇಶ_2 = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (ಸಂದೇಶ_2 == ಖಾಲಿ)
	error = TRUE;
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	ಆದರೆ (buttons == ಖಾಲಿ)
	    error = TRUE;
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	    ಆದರೆ (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		ಆದರೆ (typestr == ಖಾಲಿ)
		    error = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆಯ್ಕೆ (TOUPPER_ASC(*typestr))
		    {
			ಸ್ಥಿತಿ 'E': type = VIM_ERROR; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 'Q': type = VIM_QUESTION; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 'I': type = VIM_INFO; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 'W': type = VIM_WARNING; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 'G': type = VIM_GENERIC; ನಿಲ್ಲಿಸು;
		    }
		}
	    }
	}
    }

    ಆದರೆ (buttons == ಖಾಲಿ || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    ಆದರೆ (!error)
	rettv->vval.v_ಸಂಖ್ಯೆ = do_dialog(type, ಖಾಲಿ, ಸಂದೇಶ_2, buttons,
							    def, ಖಾಲಿ, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "copy()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_copy(typval_T *argvars, typval_T *rettv)
{
    ಅಂಶ_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_cursorpos(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ charcol)
{
    ದೀರ್ಘ	lnum, ಲಂಬಸಾಲು;
    ದೀರ್ಘ	coladd = 0;
    ಪೂರ್ಣಾಂಕ		ಗಣ_curswant = TRUE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_or_ಸಂಖ್ಯೆ_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಆದರೆ (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	ಆದರೆ (list2fpos(argvars, &pos, ಖಾಲಿ, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_ವಾದ));
	    ಹಿಂತಿರುಗಿ;
	}
	lnum = pos.lnum;
	ಲಂಬಸಾಲು = pos.ಲಂಬಸಾಲು;
	coladd = pos.coladd;
	ಆದರೆ (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    ಗಣ_curswant = FALSE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((argvars[0].v_type == VAR_ಸಂಖ್ಯೆ
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_ಸಂಖ್ಯೆ
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	ಆದರೆ (lnum < 0)
	    semsg(_(e_invalid_ವಾದ_str), tv_get_string(&argvars[0]));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	ಲಂಬಸಾಲು = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ಖಾಲಿ);
	ಆದರೆ (charcol)
	    ಲಂಬಸಾಲು = buf_charidx_ಗಮ್ಯ_byteidx(curbuf, lnum, ಲಂಬಸಾಲು) + 1;
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	emsg(_(e_invalid_ವಾದ));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (lnum < 0 || ಲಂಬಸಾಲು < 0 || coladd < 0)
	ಹಿಂತಿರುಗಿ;		// type error; errmsg already given
    ಆದರೆ (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    ಆದರೆ (ಲಂಬಸಾಲು != MAXCOL && --ಲಂಬಸಾಲು < 0)
	ಲಂಬಸಾಲು = 0;
    curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    ಆದರೆ (has_mbyte)
	mb_adjust_cursor();

    curwin->w_ಗಣ_curswant = ಗಣ_curswant;
    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
}

/*
 * "cursor(lnum, col)" function, or
 * "cursor(list)"
 *
 * Moves the cursor to the specified line and column.
 * Returns 0 when the position could be set, -1 otherwise.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_cursor(typval_T *argvars, typval_T *rettv)
{
    ಗಣ_cursorpos(argvars, rettv, FALSE);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
/*
 * "debugbreak()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		pid;

    rettv->vval.v_ಸಂಖ್ಯೆ = FAIL;
    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    pid = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    ಆದರೆ (pid == 0)
    {
	emsg(_(e_invalid_ವಾದ));
	ಹಿಂತಿರುಗಿ;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    ಆದರೆ (hProcess == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_ಸಂಖ್ಯೆ = OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * "deepcopy()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    ಆದರೆ (check_for_opt_bool_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], ಖಾಲಿ);

    ಅಂಶ_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_did_filetype(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_echoraw(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    char_u *str;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    str = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (str != ಖಾಲಿ && *str != NUL)
    {
	ಹೊರಹೋಗು_str(str);
	ಹೊರಹೋಗು_flush();
    }
}

/*
 * "empty({expr})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ಖಾಲಿಗಳು(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		n = FALSE;

    ಆಯ್ಕೆ (argvars[0].v_type)
    {
	ಸ್ಥಿತಿ VAR_STRING:
	ಸ್ಥಿತಿ VAR_FUNC:
	    n = argvars[0].vval.v_string == ಖಾಲಿ
					  || *argvars[0].vval.v_string == NUL;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	    n = argvars[0].vval.v_ಸಂಖ್ಯೆ == 0;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    n = argvars[0].vval.v_list == ಖಾಲಿ
					|| argvars[0].vval.v_list->lv_len == 0;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    n = argvars[0].vval.v_dict == ಖಾಲಿ
			|| argvars[0].vval.v_dict->dv_hashtab.ht_ಬಳಸಿದ == 0;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	    n = argvars[0].vval.v_ಸಂಖ್ಯೆ != VVAL_TRUE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CLASS:
	    n = argvars[0].vval.v_class != ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_OBJECT:
	    n = object_ಖಾಲಿಗಳು(argvars[0].vval.v_object);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_BLOB:
	    n = argvars[0].vval.v_blob == ಖಾಲಿ
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    n = argvars[0].vval.v_ಕೆಲಸ == ಖಾಲಿ
			   || argvars[0].vval.v_ಕೆಲಸ->jv_ಸ್ಥಿತಿ != ಕೆಲಸ_STARTED;
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_CHANNEL:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    n = argvars[0].vval.v_channel == ಖಾಲಿ
			       || !channel_is_open(argvars[0].vval.v_channel);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == ಖಾಲಿ
		|| argvars[0].vval.v_typealias->ta_name == ಖಾಲಿ
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	ಸ್ಥಿತಿ VAR_INSTR:
	    ಆಂತರಿಕ_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    ನಿಲ್ಲಿಸು;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

/*
 * "environ()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_environ(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
#ಆದರೆ !defined(AMIGA)
    ಪೂರ್ಣಾಂಕ			i = 0;
    char_u		*ನಮೂದು, *ಮೌಲ್ಯ;
# ಆದರೆ defined (MSWIN)
#  ಆದರೆ !defined(_UCRT)
    ಬಾಹ್ಯ wchar_t	**_wenviron;
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
    ಬಾಹ್ಯ ಅಕ್ಷರ		**environ;
# endif

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

# ifdef MSWIN
    ಆದರೆ (*_wenviron == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
# ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (*environ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
# endif

    ಗಾಗಿ (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	ಆದರೆ ((p = (short_u *)_wenviron[i]) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ನಮೂದು = utf16_ಗಮ್ಯ_enc(p, ಖಾಲಿ);
# ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ ((ನಮೂದು = (char_u *)environ[i]) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ನಮೂದು = vim_strsave(ನಮೂದು);
# endif
	ಆದರೆ (ನಮೂದು == ಖಾಲಿ) // out of memory
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ ((ಮೌಲ್ಯ = vim_strchr(ನಮೂದು, '=')) == ಖಾಲಿ)
	{
	    vim_free(ನಮೂದು);
	    ಮುಂದುವರಿ;
	}
	*ಮೌಲ್ಯ++ = NUL;
	dict_ಕೂಡಿಸು_string(rettv->vval.v_dict, (ಅಕ್ಷರ *)ನಮೂದು, ಮೌಲ್ಯ);
	vim_free(ನಮೂದು);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "err_teapot()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_err_teapot(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ಆದರೆ (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    ಆದರೆ (*skipwhite(s) == NUL)
		ಹಿಂತಿರುಗಿ;
	}

	ಪೂರ್ಣಾಂಕ err = FALSE;
	ಪೂರ್ಣಾಂಕ do_503 = eval_ಅಭಿವ್ಯಕ್ತಿ_ಗಮ್ಯ_bool(&argvars[0], &err);
	ಆದರೆ (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_ಲಭ್ಯ));
	    ಹಿಂತಿರುಗಿ;
	}
    }

    emsg(_(e_im_a_teapot));
}

/*
 * "escape({string}, {chars})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_escape(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_string = vim_strsave_escaped(tv_get_string(&argvars[0]),
					 tv_get_string_buf(&argvars[1], buf));
    rettv->v_type = VAR_STRING;
}

/*
 * "eval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*s, *p;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    s = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (s != ಖಾಲಿ)
	s = skipwhite(s);

    p = s;
    ಆದರೆ (s == ಖಾಲಿ || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	ಆದರೆ (p != ಖಾಲಿ && !aborting())
	    semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_eventhandler(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = vgetc_busy || ಒಳಹರಿವು_busy;
}

ಸ್ಥಿರ garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    ಶೂನ್ಯ
execute_redir_str(char_u *ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ_len)
{
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (ಮೌಲ್ಯ_len == -1)
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(ಮೌಲ್ಯ);	// Append the entire string
    ಇಲ್ಲದಿದ್ದರೆ
	len = ಮೌಲ್ಯ_len;		// Append only "value_len" characters
    ಆದರೆ (ga_grow(&redir_execute_ga, len) == FAIL)
	ಹಿಂತಿರುಗಿ;

    mch_memmove((ಅಕ್ಷರ *)redir_execute_ga.ga_ದತ್ತಾಂಶ
	    + redir_execute_ga.ga_len, ಮೌಲ್ಯ, len);
    redir_execute_ga.ga_len += len;
}

#ಆದರೆ defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    ಸ್ಥಿರ char_u *
get_str_ಸಾಲು(
    ಪೂರ್ಣಾಂಕ	    c ಬಳಸದ,
    ಶೂನ್ಯ    *cookie,
    ಪೂರ್ಣಾಂಕ	    indent ಬಳಸದ,
    getline_opt_T options ಬಳಸದ)
{
    char_u	*ಆರಂಭ = *(char_u **)cookie;
    char_u	*ಸಾಲು;
    char_u	*p;

    p = ಆರಂಭ;
    ಆದರೆ (p == ಖಾಲಿ || *p == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    p = vim_strchr(p, '\n');
    ಆದರೆ (p == ಖಾಲಿ)
	ಸಾಲು = vim_strsave(ಆರಂಭ);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸಾಲು = vim_strnsave(ಆರಂಭ, p - ಆರಂಭ);
	p++;
    }

    *(char_u **)cookie = p;
    ಹಿಂತಿರುಗಿ ಸಾಲು;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    ಶೂನ್ಯ
execute_cmds_from_string(char_u *str)
{
    do_cmdline(ಖಾಲಿ, get_str_ಸಾಲು, (ಶೂನ್ಯ *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_ಸಾಲು(
    ಪೂರ್ಣಾಂಕ	    c ಬಳಸದ,
    ಶೂನ್ಯ    *cookie,
    ಪೂರ್ಣಾಂಕ	    indent ಬಳಸದ,
    getline_opt_T options ಬಳಸದ)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *ಅಂಶ = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    s = tv_get_string_buf_chk(&ಅಂಶ->li_tv, buf);
    *p = ಅಂಶ->li_ಮುಂದಿನ;
    ಹಿಂತಿರುಗಿ s == ಖಾಲಿ ? ಖಾಲಿ : vim_strsave(s);
}

/*
 * "execute()" function
 */
    ಶೂನ್ಯ
execute_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ arg_off)
{
    char_u	*cmd = ಖಾಲಿ;
    list_T	*list = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		save_ಸಂದೇಶ_silent = ಸಂದೇಶ_silent;
    ಪೂರ್ಣಾಂಕ		save_emsg_silent = emsg_silent;
    ಪೂರ್ಣಾಂಕ		save_emsg_noredir = emsg_noredir;
    ಪೂರ್ಣಾಂಕ		save_redir_execute = redir_execute;
    ಪೂರ್ಣಾಂಕ		save_redir_off = redir_off;
    garray_T	save_ga;
    ಪೂರ್ಣಾಂಕ		save_ಸಂದೇಶ_ಲಂಬಸಾಲು = ಸಂದೇಶ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    ಪೂರ್ಣಾಂಕ		echo_output = FALSE;

    rettv->vval.v_string = ಖಾಲಿ;
    rettv->v_type = VAR_STRING;

    ಆದರೆ (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	ಆದರೆ (list == ಖಾಲಿ || list->lv_len == 0)
	    // empty list, no commands, empty output
	    ಹಿಂತಿರುಗಿ;
	++list->lv_refcount;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[arg_off].v_type == VAR_ಕೆಲಸ
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	ಆದರೆ (cmd == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      ಒಳ_vim9script());

	ಆದರೆ (s == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (*s == NUL)
	    echo_output = TRUE;
	ಆದರೆ (STRNCMP(s, "silent", 6) == 0)
	    ++ಸಂದೇಶ_silent;
	ಆದರೆ (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	++ಸಂದೇಶ_silent;

    ಆದರೆ (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, ಗಾತ್ರ(ಅಕ್ಷರ), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    ಆದರೆ (!echo_output)
	ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    ಆದರೆ (cmd != ಖಾಲಿ)
	do_cmdline_cmd(cmd);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	listitem_T	*ಅಂಶ;

	CHECK_LIST_MATERIALIZE(list);
	ಅಂಶ = list->lv_ಮೊದಲ;
	do_cmdline(ಖಾಲಿ, get_list_ಸಾಲು, (ಶೂನ್ಯ *)&ಅಂಶ,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    ಆದರೆ (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((ಅಕ್ಷರ *)redir_execute_ga.ga_ದತ್ತಾಂಶ)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_ದತ್ತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = ಖಾಲಿ;
    }
    ಸಂದೇಶ_silent = save_ಸಂದೇಶ_silent;
    emsg_silent = save_emsg_silent;
    emsg_noredir = save_emsg_noredir;

    redir_execute = save_redir_execute;
    ಆದರೆ (redir_execute)
	redir_execute_ga = save_ga;
    redir_off = save_redir_off;

    // "silent reg" or "silent echo x" leaves msg_col somewhere in the line.
    ಆದರೆ (echo_output)
	// When not working silently: put it in column zero.  A following
	// "echon" will overwrite the message, unavoidably.
	ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	ಸಂದೇಶ_ಲಂಬಸಾಲು = save_ಸಂದೇಶ_ಲಂಬಸಾಲು;
}

/*
 * "execute()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_execute(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    ಶೂನ್ಯ
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		n = FALSE;

    ಆದರೆ (ಒಳ_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    p = tv_get_string(&argvars[0]);
    ಆದರೆ (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	ಆದರೆ (mch_getenv(p + 1) != ಖಾಲಿ)
	    n = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    ಆದರೆ (p != ಖಾಲಿ && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, ಖಾಲಿ, TRUE) == OK);
	ಆದರೆ (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '?')			// internal function only
    {
	n = has_ಆಂತರಿಕ_func_name(p + 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '#')
    {
	ಆದರೆ (p[1] == '#')
	    n = autocmd_supported(p + 2);
	ಇಲ್ಲದಿದ್ದರೆ
	    n = au_exists(p + 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
f_exists_compiled(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    emsg(_(e_exists_compiled_can_only_be_ಬಳಸಿದ_ಒಳ_def_function));
}

/*
 * "expand()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    size_t	len;
    ಪೂರ್ಣಾಂಕ		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    char_u	*ಫಲಿತಾಂಶ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    char_u	*p_csl_save = p_csl;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    // avoid using 'completeslash' here
    p_csl = ಖಾಲಿಗಳು_option;
#ಆದರೆ_ಅಂತ್ಯ

    rettv->v_type = VAR_STRING;
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_ಗಣ(rettv, ಖಾಲಿ);

    s = tv_get_string(&argvars[0]);
    ಆದರೆ (*s == '%' || *s == '#' || *s == '<')
    {
	ಅಕ್ಷರ	*errormsg = ಖಾಲಿ;

	ಆದರೆ (p_verbose == 0)
	    ++emsg_off;
	ಫಲಿತಾಂಶ = eval_vars(s, s, &len, ಖಾಲಿ, &errormsg, ಖಾಲಿ, FALSE);
	ಆದರೆ (p_verbose == 0)
	    --emsg_off;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (errormsg != ಖಾಲಿ)
	    emsg(errormsg);
	ಆದರೆ (rettv->v_type == VAR_LIST)
	{
	    ಆದರೆ (rettv_list_alloc(rettv) == OK && ಫಲಿತಾಂಶ != ಖಾಲಿ)
		list_append_string(rettv->vval.v_list, ಫಲಿತಾಂಶ, -1);
	    vim_free(ಫಲಿತಾಂಶ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    rettv->vval.v_string = ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// When the optional second argument is non-zero, don't remove matches
	// for 'wildignore' and don't put matches for 'suffixes' at the end.
	ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	ಆದರೆ (!error)
	{
	    ExpandInit(&xpc);
	    xpc.xp_context = EXPAND_FILES;
	    ಆದರೆ (p_wic)
		options += WILD_ICASE;
	    ಆದರೆ (rettv->v_type == VAR_STRING)
		rettv->vval.v_string = ExpandOne(&xpc, s, ಖಾಲಿ,
							   options, WILD_ALL);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, ಖಾಲಿ, options, WILD_ALL_KEEP);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    rettv->vval.v_string = ಖಾಲಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    p_csl = p_csl_save;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "expandcmd()" function
 * Expand all the special characters in a command string.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_expandcmd(typval_T *argvars, typval_T *rettv)
{
    exarg_T	eap;
    char_u	*cmdstr;
    ಅಕ್ಷರ	*errormsg = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		emsgoff = TRUE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_ಕ್ಷೇತ್ರ(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = ಖಾಲಿ;
    eap.cmdidx = CMD_USER;

    ಆದರೆ (emsgoff)
	++emsg_off;
    ಆದರೆ (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	ಆದರೆ (!emsgoff && errormsg != ಖಾಲಿ && *errormsg != NUL)
	    emsg(errormsg);
    ಆದರೆ (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_feedkeys(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		remap = TRUE;
    ಪೂರ್ಣಾಂಕ		ಸೇರಿಸು = FALSE;
    char_u	*ಕೀಲಿಗಳು, *flags;
    char_u	nbuf[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		typed = FALSE;
    ಪೂರ್ಣಾಂಕ		execute = FALSE;
    ಪೂರ್ಣಾಂಕ		context = FALSE;
    ಪೂರ್ಣಾಂಕ		dangerous = FALSE;
    ಪೂರ್ಣಾಂಕ		lowlevel = FALSE;

    // This is not allowed in the sandbox.  If the commands would still be
    // executed in the sandbox it would be OK, but it probably happens later,
    // when "sandbox" is no longer set.
    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಕೀಲಿಗಳು = tv_get_string(&argvars[0]);

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	ಗಾಗಿ ( ; *flags != NUL; ++flags)
	{
	    ಆಯ್ಕೆ (*flags)
	    {
		ಸ್ಥಿತಿ 'n': remap = FALSE; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'm': remap = TRUE; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 't': typed = TRUE; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'i': ಸೇರಿಸು = TRUE; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'x': execute = TRUE; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'c': context = TRUE; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '!': dangerous = TRUE; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'L': lowlevel = TRUE; ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ಆದರೆ (*ಕೀಲಿಗಳು != NUL || execute)
    {
	ಆದರೆ (lowlevel
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VTP
		&& (!is_ಪದ_win32()
		    || (ಕೀಲಿಗಳು[0] == 3 && ctrl_c_interrupts && typed))
#ಆದರೆ_ಅಂತ್ಯ
	   )
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಒಳಹರಿವು_BUF
	    ch_log(ಖಾಲಿ, "feedkeys() lowlevel: %s", ಕೀಲಿಗಳು);

	    ಪೂರ್ಣಾಂಕ len = (ಪೂರ್ಣಾಂಕ)STRLEN(ಕೀಲಿಗಳು);
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		ಆದರೆ (ಕೀಲಿಗಳು[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(ಕೀಲಿಗಳು + idx, 1);
	    }
#ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_lowlevel_ಒಳಹರಿವು_not_supported));
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *ಕೀಲಿಗಳು_esc = vim_strsave_escape_csi(ಕೀಲಿಗಳು);
	    ಆದರೆ (ಕೀಲಿಗಳು_esc == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;

	    ch_log(ಖಾಲಿ, "feedkeys(%s): %s", typed ? "typed" : "", ಕೀಲಿಗಳು);

	    ins_typebuf(ಕೀಲಿಗಳು_esc, (remap ? REMAP_YES : REMAP_NONE),
				   ಸೇರಿಸು ? 0 : typebuf.tb_len, !typed, FALSE);
	    ಆದರೆ (vgetc_busy
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
		    || timer_busy
#ಆದರೆ_ಅಂತ್ಯ
		    || ಒಳಹರಿವು_busy)
		typebuf_was_filled = TRUE;

	    vim_free(ಕೀಲಿಗಳು_esc);
	}

	ಆದರೆ (execute)
	{
	    ಪೂರ್ಣಾಂಕ		save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    ಸಂದೇಶ_scroll = FALSE;

	    ch_log(ಖಾಲಿ, "feedkeys() executing");

	    ಆದರೆ (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_ಆವೃತ್ತಿ = 0;
	    }

	    ಆದರೆ (!dangerous)
	    {
		++ex_ಸಾಮಾನ್ಯ_busy;
		++ಒಳ_feedkeys;
	    }
	    ++allow_osc_ಕೀಲಿ;
	    exec_ಸಾಮಾನ್ಯ(TRUE, lowlevel, TRUE);
	    --allow_osc_ಕೀಲಿ;
	    ಆದರೆ (!dangerous)
	    {
		--ex_ಸಾಮಾನ್ಯ_busy;
		--ಒಳ_feedkeys;
	    }

	    ಸಂದೇಶ_scroll |= save_ಸಂದೇಶ_scroll;

	    ಆದರೆ (context)
		current_sctx = save_sctx;
	}
    }
}

/*
 * "fnameescape({string})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_fnameescape(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_foreground(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	gui_mch_ಗಣ_foreground();
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_ಗಣ_foreground();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "function()" function
 * "funcref()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
common_function(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ is_funcref)
{
    char_u	*s;
    char_u	*name;
    ಪೂರ್ಣಾಂಕ		use_string = FALSE;
    partial_T   *arg_pt = ಖಾಲಿ;
    char_u	*trans_name = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		is_global = FALSE;
    char_u	*ಆರಂಭ_bracket = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != ಖಾಲಿ)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    ಆದರೆ (s == ಖಾಲಿ)
    {
	semsg(_(e_invalid_ವಾದ_str), "NULL");
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == ಖಾಲಿ) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, ಖಾಲಿ);
	ಆದರೆ (*name == '<')
	{
	    // generic function
	    ಆರಂಭ_bracket = name;
	    ಆದರೆ (skip_generic_func_type_ವಾದಗಳು(&name) == FAIL)
		ಹೋಗು theend;
	}
	ಆದರೆ (*name != NUL)
	    s = ಖಾಲಿ;
    }

    ಆದರೆ (s == ಖಾಲಿ || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == ಖಾಲಿ))
	semsg(_(e_invalid_ವಾದ_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (trans_name != ಖಾಲಿ && (is_funcref
			 ? ಕಂಡುಹಿಡಿ_func(trans_name, is_global) == ಖಾಲಿ
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	dict_idx = 0;
	ಪೂರ್ಣಾಂಕ	arg_idx = 0;
	list_T	*list = ಖಾಲಿ;

	ಆದರೆ (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (trans_name != ಖಾಲಿ && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	ಇಲ್ಲದಿದ್ದರೆ
	    name = vim_strsave(s);

	ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		// function(name, [args])
		arg_idx = 1;
	    ಆದರೆ (dict_idx > 0)
	    {
		ಆದರೆ (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    ಹೋಗು theend;
		}
		ಆದರೆ (argvars[dict_idx].vval.v_dict == ಖಾಲಿ)
		    dict_idx = 0;
	    }
	    ಆದರೆ (arg_idx > 0)
	    {
		ಆದರೆ (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_ಎರಡನೇ_ವಾದ_ದ_function_must_be_list_or_dict));
		    vim_free(name);
		    ಹೋಗು theend;
		}
		list = argvars[arg_idx].vval.v_list;
		ಆದರೆ (list == ಖಾಲಿ || list->lv_len == 0)
		    arg_idx = 0;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (list->lv_len > MAX_FUNC_ವಾದಗಳು)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    ಹೋಗು theend;
		}
	    }
	}
	ಆದರೆ (dict_idx > 0 || arg_idx > 0 || arg_pt != ಖಾಲಿ || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    ಆದರೆ (pt == ಖಾಲಿ)
		vim_free(name);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (arg_idx > 0 || (arg_pt != ಖಾಲಿ && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    ಪೂರ್ಣಾಂಕ		i = 0;
		    ಪೂರ್ಣಾಂಕ		arg_len = 0;
		    ಪೂರ್ಣಾಂಕ		lv_len = 0;

		    ಆದರೆ (arg_pt != ಖಾಲಿ)
			arg_len = arg_pt->pt_argc;
		    ಆದರೆ (list != ಖಾಲಿ)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    ಆದರೆ (pt->pt_argv == ಖಾಲಿ)
		    {
			vim_free(pt);
			vim_free(name);
			ಹೋಗು theend;
		    }
		    ಗಾಗಿ (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    ಆದರೆ (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
			    copy_tv(&li->li_tv, &pt->pt_argv[i++]);
		    }
		}

		// For "function(dict.func, [], dict)" and "func" is a partial
		// use "dict".  That is backwards compatible.
		ಆದರೆ (dict_idx > 0)
		{
		    // The dict is bound explicitly, pt_auto is FALSE.
		    pt->pt_dict = argvars[dict_idx].vval.v_dict;
		    ++pt->pt_dict->dv_refcount;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg_pt != ಖಾಲಿ)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    ಆದರೆ (pt->pt_dict != ಖಾಲಿ)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    ಆದರೆ (pt->pt_obj != ಖಾಲಿ)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		ಆದರೆ (arg_pt != ಖಾಲಿ && arg_pt->pt_func != ಖಾಲಿ)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_funcref)
		{
		    pt->pt_func = ಕಂಡುಹಿಡಿ_func(trans_name, is_global);
		    ಆದರೆ (IS_GENERIC_FUNC(pt->pt_func) && ಆರಂಭ_bracket != ಖಾಲಿ)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&ಆರಂಭ_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		ಆದರೆ (arg_pt != ಖಾಲಿ)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    ಆದರೆ (ಆರಂಭ_bracket == ಖಾಲಿ)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// generic function
		size_t len = STRLEN(name) + STRLEN(ಆರಂಭ_bracket);
		rettv->vval.v_string = alloc(len + 1);
		ಆದರೆ (rettv->vval.v_string != ಖಾಲಿ)
		{
		    STRCPY(rettv->vval.v_string, name);
		    STRCAT(rettv->vval.v_string, ಆರಂಭ_bracket);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
f_funcref(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, TRUE);
}

/*
 * "function()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_function(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, FALSE);
}

/*
 * "garbagecollect()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_garbagecollect(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
	garbage_collect_at_exit = TRUE;
}

/*
 * "get()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_get(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    list_T	*l;
    tuple_T	*tuple;
    dictitem_T	*di;
    dict_T	*d;
    typval_T	*tv = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		what_is_dict = FALSE;

    ಆದರೆ (argvars[0].v_type == VAR_BLOB)
    {
	ಪೂರ್ಣಾಂಕ error = FALSE;
	ಪೂರ್ಣಾಂಕ idx = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);

	ಆದರೆ (!error)
	{
	    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	    ಆದರೆ (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    ಆದರೆ (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_ಸಂಖ್ಯೆ = -1;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		rettv->vval.v_ಸಂಖ್ಯೆ = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_LIST)
    {
	ಆದರೆ ((l = argvars[0].vval.v_list) != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ		error = FALSE;

	    li = list_ಕಂಡುಹಿಡಿ(l, (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error));
	    ಆದರೆ (!error && li != ಖಾಲಿ)
		tv = &li->li_tv;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_TUPLE)
    {
	ಆದರೆ ((tuple = argvars[0].vval.v_tuple) != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ		error = FALSE;
	    ದೀರ್ಘ	idx;

	    idx = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
	    ಆದರೆ (!error)
		tv = tuple_ಕಂಡುಹಿಡಿ(tuple, idx);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_DICT)
    {
	ಆದರೆ ((d = argvars[0].vval.v_dict) != ಖಾಲಿ)
	{
	    di = dict_ಕಂಡುಹಿಡಿ(d, tv_get_string(&argvars[1]), -1);
	    ಆದರೆ (di != ಖಾಲಿ)
		tv = &di->di_tv;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	ಆದರೆ (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    CLEAR_ಕ್ಷೇತ್ರ(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	ಆದರೆ (pt != ಖಾಲಿ)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    ಆದರೆ (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		ಆದರೆ (name == ಖಾಲಿ)
		    rettv->vval.v_string = ಖಾಲಿ;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    ಆದರೆ (*what == 'n' && pt->pt_name == ಖಾಲಿ
							&& pt->pt_func != ಖಾಲಿ)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		ಆದರೆ (pt->pt_dict != ಖಾಲಿ)
		    rettv_dict_ಗಣ(rettv, pt->pt_dict);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		ಆದರೆ (rettv_list_alloc(rettv) == OK)
		{
		    ಪೂರ್ಣಾಂಕ i;

		    ಗಾಗಿ (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(what, "arity") == 0)
	    {
		ಪೂರ್ಣಾಂಕ required = 0, optional = 0, varargs = FALSE;
		char_u *name = partial_name(pt);

		get_func_arity(name, &required, &optional, &varargs);

		rettv->v_type = VAR_DICT;
		ಆದರೆ (rettv_dict_alloc(rettv) == OK)
		{
		    dict_T *dict = rettv->vval.v_dict;

		    // Take into account the arguments of the partial, if any.
		    // Note that it is possible to supply more arguments than the function
		    // accepts.
		    ಆದರೆ (pt->pt_argc >= required + optional)
			required = optional = 0;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			required -= pt->pt_argc;

		    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "required", required);
		    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "optional", optional);
		    dict_ಕೂಡಿಸು_bool(dict, "varargs", varargs);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_invalid_ವಾದ_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    ಆದರೆ (!what_is_dict)
		ಹಿಂತಿರುಗಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_ವಾದ_ದ_str_must_be_list_tuple_dictionary_or_blob), "get()");

    ಆದರೆ (tv == ಖಾಲಿ)
    {
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getbgcolor(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	ಆದರೆ (gui.ಒಳ_use)
	    try_hl = true;
# endif
# ifdef FEAT_TERMGUICOLORS
	ಆದರೆ (p_tgc)
	    try_hl = true;
# endif
	ಆದರೆ (try_hl)
	{
	    guicolor_T	fg, bg;
	    ಪೂರ್ಣಾಂಕ		hl_id = syn_name2id((char_u *)"Normal");

	    ಆದರೆ (hl_id > 0)
	    {
		(ಶೂನ್ಯ)syn_id2colors(hl_id, &fg, &bg);
		ಆದರೆ (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    ಹಿಂತಿರುಗಿ;
		}
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	ಆದರೆ (ಪದ_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)r);
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)g);
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "getcellpixels()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getcellpixels(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

#ಆದರೆ defined(FEAT_GUI)
    ಆದರೆ (gui.ಒಳ_use)
    {
	// success pixel size and no gui.
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)gui.char_ಅಗಲ);
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)gui.char_ಎತ್ತರ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
#ಆದರೆ defined(UNIX) || defined(MSWIN)
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)cell_ಅಗಲ);
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)cell_ಎತ್ತರ);
#ಇಲ್ಲದಿದ್ದರೆ
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)-1);
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)-1);
#ಆದರೆ_ಅಂತ್ಯ
    }

}

/*
 * "getchangelist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		i;
    list_T	*l;
    dict_T	*d;
    ಪೂರ್ಣಾಂಕ		changelistindex;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    ಇಲ್ಲದಿದ್ದರೆ
	buf = tv_get_buf_from_arg(&argvars[0]);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    l = list_alloc();
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	ಹಿಂತಿರುಗಿ;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    ಆದರೆ (buf == curwin->w_ಬಫರ್)
    {
	changelistindex = curwin->w_changelistidx;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    ಆದರೆ (wip->wi_win == curwin)
		ನಿಲ್ಲಿಸು;
	changelistindex = wip != ಖಾಲಿ ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)changelistindex);

    ಗಾಗಿ (i = 0; i < buf->b_changelistlen; ++i)
    {
	ಆದರೆ (buf->b_changelist[i].lnum == 0)
	    ಮುಂದುವರಿ;
	ಆದರೆ ((d = dict_alloc()) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ಹಿಂತಿರುಗಿ;
	}
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "lnum", (ದೀರ್ಘ)buf->b_changelist[i].lnum);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "col", (ದೀರ್ಘ)buf->b_changelist[i].ಲಂಬಸಾಲು);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "coladd", (ದೀರ್ಘ)buf->b_changelist[i].coladd);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    ಪೂರ್ಣಾಂಕ		getcurpos,
    ಪೂರ್ಣಾಂಕ		charcol)
{
    pos_T	*fp = ಖಾಲಿ;
    pos_T	pos;
    win_T	*wp = curwin;
    list_T	*l;
    ಪೂರ್ಣಾಂಕ		fnum = -1;

    ಆದರೆ (rettv_list_alloc(rettv) == OK)
    {
	l = rettv->vval.v_list;
	ಆದರೆ (getcurpos)
	{
	    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
	    {
		wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
		ಆದರೆ (wp != ಖಾಲಿ)
		    fp = &wp->w_cursor;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		fp = &curwin->w_cursor;
	    ಆದರೆ (fp != ಖಾಲಿ && charcol)
	    {
		pos = *fp;
		pos.ಲಂಬಸಾಲು =
		    buf_byteidx_ಗಮ್ಯ_charidx(wp->w_ಬಫರ್, pos.lnum, pos.ಲಂಬಸಾಲು);
		fp = &pos;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	ಆದರೆ (fnum != -1)
	    list_append_ಸಂಖ್ಯೆ(l, (varnumber_T)fnum);
	ಇಲ್ಲದಿದ್ದರೆ
	    list_append_ಸಂಖ್ಯೆ(l, (varnumber_T)0);
	list_append_ಸಂಖ್ಯೆ(l, (fp != ಖಾಲಿ) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_ಸಂಖ್ಯೆ(l, (fp != ಖಾಲಿ)
		     ? (varnumber_T)(fp->ಲಂಬಸಾಲು == MAXCOL ? MAXCOL : fp->ಲಂಬಸಾಲು + 1)
							    : (varnumber_T)0);
	list_append_ಸಂಖ್ಯೆ(l, (fp != ಖಾಲಿ) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	ಆದರೆ (getcurpos)
	{
	    ಪೂರ್ಣಾಂಕ	    save_ಗಣ_curswant = curwin->w_ಗಣ_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    ಆದರೆ (wp == curwin)
		update_curswant();
	    list_append_ಸಂಖ್ಯೆ(l, wp == ಖಾಲಿ ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    ಆದರೆ (wp == curwin && save_ಗಣ_curswant)
	    {
		curwin->w_ಗಣ_curswant = save_ಗಣ_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;
}

/*
 * "getcharpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getcharpos(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getcharsearch(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಆದರೆ (rettv_dict_alloc(rettv) != OK)
	ಹಿಂತಿರುಗಿ;

    dict_T *dict = rettv->vval.v_dict;

    dict_ಕೂಡಿಸು_string(dict, "char", ಕೊನೆಯ_csearch());
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "forward", ಕೊನೆಯ_csearch_forward());
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "until", ಕೊನೆಯ_csearch_until());
}

/*
 * "getenv()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getenv(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ	    mustfree = FALSE;
    char_u  *p;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    ಆದರೆ (p == ಖಾಲಿ)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getfontname(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	GuiFont font;
	char_u	*name = ಖಾಲಿ;

	ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    name = tv_get_string(&argvars[0]);
	    ಆದರೆ (STRCMP(name, "*") == 0)	    // don't use font dialog
		ಹಿಂತಿರುಗಿ;
	    font = gui_mch_get_font(name, FALSE);
	    ಆದರೆ (font == NOFONT)
		ಹಿಂತಿರುಗಿ;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
ಸ್ಥಿರ ರಚನೆ
{
    argcheck_T	func;
    ಅಕ್ಷರ	*types;	    // separated by commas
} argcheck_types[] =
{
    {arg_any,			"any"},
    {arg_blob,			"blob"},
    {arg_bool,			"bool,number"},
    {arg_bool_or_dict_any,	"bool,number,dict<any>"},
    {arg_bool_or_nr,		"bool,number"},
    {arg_ಬಫರ್,		"string,number"},
    {arg_ಬಫರ್_or_dict_any,	"string,number,dict<any>"},
    {arg_chan_or_ಕೆಲಸ,		"channel,job"},
    {arg_cursor1,		"number,string,list<any>"},
    {arg_dict_any,		"dict<any>"},
    {arg_dict_any_or_string,	"dict<any>,string"},
    {arg_extend3,		"number,string"},
    {arg_filter_func,		"string,func"},
    {arg_float_or_nr,		"float,number"},
    {arg_foreach_func,		"string,func"},
    {arg_get1,			"blob,list<any>,tuple<any>,dict<any>,func"},
    {arg_ಅಂಶ_ದ_prev,		"any"},
    {arg_ಕೆಲಸ,			"job"},
    {arg_len1,
	    "string,number,blob,list<any>,tuple<any>,dict<any>,object<any>"},
    {arg_list_any,		"list<any>"},
    {arg_list_any_mod,		"list<any>"},
    {arg_list_ಸಂಖ್ಯೆ,		"list<number>"},
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
    {arg_ಸಂಖ್ಯೆ,		"number"},
    {arg_object,		"object<any>"},
    {arg_remove2,		"number,string"},
    {arg_repeat1,		"string,number,blob,list<any>,tuple<any>"},
    {arg_ಹಿಮ್ಮುಖ,		"list<any>,tuple<any>,blob,string"},
    {arg_same_as_prev,		"any"},
    {arg_same_struct_as_prev,	"any"},
    {arg_slice1,		"list<any>,tuple<any>,blob,string"},
    {arg_ವಿಂಗಡಿಸು_how,		"string,func"},
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
    ಸ್ಥಿರ list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    ಅಕ್ಷರ	*types = "any";

    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(argcheck_types); ++i)
	ಆದರೆ (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    ನಿಲ್ಲಿಸು;
	}
    ಗಾಗಿ (ಅಕ್ಷರ *p = types; ; )
    {
	ಅಕ್ಷರ	*ಕೊನೆ = (ಅಕ್ಷರ *)vim_strchr((char_u *)p, ',');
	ಪೂರ್ಣಾಂಕ	len = ಕೊನೆ == ಖಾಲಿ ? (ಪೂರ್ಣಾಂಕ)STRLEN(p) : (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - p);

	ಆದರೆ (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಆದರೆ (ಕೊನೆ == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	p = ಕೊನೆ + 1;
    }
    ಹಿಂತಿರುಗಿ l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
getinfo_argtypes(
	list_T			*l,
	ಸ್ಥಿರಾಂಕ funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		argcount = 0;

    ಆದರೆ (l != ಖಾಲಿ)
	FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    ಆದರೆ (s == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ -1;
	    ಆದರೆ (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_ವಾದಗಳು)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		ಹಿಂತಿರುಗಿ -1;
	    }
	    type = parse_type(&p, type_gap, ಖಾಲಿ, ಖಾಲಿ, TRUE);
	    ಆದರೆ (type == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ -1;
	    ಆದರೆ (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		ಹಿಂತಿರುಗಿ -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    ಗಾಗಿ ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_ವಾದಗಳು; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    ಹಿಂತಿರುಗಿ argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
builtin_ret_type_name(
	ಪೂರ್ಣಾಂಕ	    idx,
	ಪೂರ್ಣಾಂಕ	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	ಅಕ್ಷರ	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = ಆಂತರಿಕ_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    ಹಿಂತಿರುಗಿ type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_ಬಗೆ_T;

ಸ್ಥಿರ ರಚನೆ
{
    ಅಕ್ಷರ		*name;
    getinfo_ಬಗೆ_T	ಬಗೆ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_ಬಗೆ_T	ಬಗೆ;
    ಪೂರ್ಣಾಂಕ			ki;
    ಪೂರ್ಣಾಂಕ			idx;
    ಸ್ಥಿರಾಂಕ funcentry_T	*fe;
    dict_T		*d;
    list_T		*ವಾದಗಳು;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_ವಾದಗಳು];
    list_T		*argtypes_list = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			argcount;
    ಪೂರ್ಣಾಂಕ			vim9 = -1;	// -1: the context of the caller
    ಅಕ್ಷರ		*ret_name;
    ಅಕ್ಷರ		*tofree;

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (ಒಳ_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (check_for_opt_dict_arg(argvars, 2) == FAIL)
	ಹಿಂತಿರುಗಿ;

    kindname = tv_get_string(&argvars[0]);
    ಗಾಗಿ (ki = 0; ki < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(getinfo_kinds); ++ki)
	ಆದರೆ (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (ki == (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(getinfo_kinds))
    {
	semsg(_(e_invalid_ವಾದ_str), kindname);
	ಹಿಂತಿರುಗಿ;
    }
    ಬಗೆ = getinfo_kinds[ki].ಬಗೆ;
    name = tv_get_string_buf(&argvars[1], namebuf);

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != ಖಾಲಿ)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	ಪೂರ್ಣಾಂಕ	    todo = (ಪೂರ್ಣಾಂಕ)opts->dv_hashtab.ht_ಬಳಸಿದ;

	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&opts->dv_hashtab, hi, todo)
	{
	    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
		ಮುಂದುವರಿ;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    ಆದರೆ (STRCMP(di->di_ಕೀಲಿ, "argtypes") == 0 && ಬಗೆ == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(di->di_ಕೀಲಿ, "vim9") == 0 && ಬಗೆ == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		semsg(_(e_invalid_ವಾದ_str), di->di_ಕೀಲಿ);
		ಹಿಂತಿರುಗಿ;
	    }
	}
    }
    ಆದರೆ (ಬಗೆ == GI_VIMVAR)
    {
	ಆದರೆ (STRNCMP(name, "v:", 2) == 0)
	    (ಶೂನ್ಯ)vim_var_info(name + 2, rettv->vval.v_dict);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಬಗೆ == GI_COMMAND)
    {
	(ಶೂನ್ಯ)ex_command_info(name, vim9, rettv->vval.v_dict);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಬಗೆ == GI_OPTION)
    {
	(ಶೂನ್ಯ)option_info(name, rettv->vval.v_dict);
	ಹಿಂತಿರುಗಿ;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func_opt(name, FALSE);
    ಆದರೆ (idx < 0)
    {
	(ಶೂನ್ಯ)user_func_info(name, rettv->vval.v_dict);
	ಹಿಂತಿರುಗಿ;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, ಗಾತ್ರ(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    ಆದರೆ (argcount < 0)
    {
	clear_type_list(&type_gap);
	ಹಿಂತಿರುಗಿ;
    }
    d = rettv->vval.v_dict;
    dict_ಕೂಡಿಸು_string(d, "name", (char_u *)fe->f_name);
    dict_ಕೂಡಿಸು_string(d, "kind", (char_u *)"builtin");
    dict_ಕೂಡಿಸು_bool(d, "available", fe->f_func != ಖಾಲಿ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "minargs", fe->f_min_argc);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "method", fe->f_argtype & FEARG_ಮುಖವಾಡ);

    ವಾದಗಳು = list_alloc();
    ಆದರೆ (ವಾದಗಳು == ಖಾಲಿ || dict_ಕೂಡಿಸು_list(d, "args", ವಾದಗಳು) == FAIL)
    {
	list_unref(ವಾದಗಳು);
	clear_type_list(&type_gap);
	ಹಿಂತಿರುಗಿ;
    }
    argnames = (char_u *)fe->f_argnames;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ	    argnamelen = 0;

	ಆದರೆ (fe->f_argcheck == ಖಾಲಿ)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    ಆದರೆ (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		ನಿಲ್ಲಿಸು;
	    check = arg_any;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    ಆದರೆ ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	ಆದರೆ (argnames != ಖಾಲಿ)
	{
	    char_u  *ಕೊನೆ = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = ಕೊನೆ == ಖಾಲಿ ? (ಪೂರ್ಣಾಂಕ)STRLEN(argnames)
						     : (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - argnames);
	    argnames = ಕೊನೆ == ಖಾಲಿ ? ಖಾಲಿ : skipwhite(ಕೊನೆ + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	ಆದರೆ (arg == ಖಾಲಿ || types == ಖಾಲಿ
		|| dict_ಕೂಡಿಸು_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ ((argnamelen > 0
		    && dict_ಕೂಡಿಸು_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(ವಾದಗಳು, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    ಆದರೆ (argtypes_list == ಖಾಲಿ
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	ಅಕ್ಷರ	*tofree_max;
	ಅಕ್ಷರ	*max_name;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = argcount; i < fe->f_max_argc && i < MAX_FUNC_ವಾದಗಳು; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	ಆದರೆ (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = ಖಾಲಿ;
	    ret_name = "any";
	}
	vim_free(tofree_max);
    }
    dict_ಕೂಡಿಸು_string(d, "returns", (char_u *)ret_name);
    vim_free(tofree);
    clear_type_list(&type_gap);
}

/*
 * "getjumplist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		i;
    list_T	*l;
    dict_T	*d;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    wp = ಕಂಡುಹಿಡಿ_tabwin(&argvars[0], &argvars[1], ಖಾಲಿ);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	ಹಿಂತಿರುಗಿ;
    }

    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    ಗಾಗಿ (i = 0; i < wp->w_jumplistlen; ++i)
    {
	ಆದರೆ (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    ಮುಂದುವರಿ;
	ಆದರೆ ((d = dict_alloc()) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ಹಿಂತಿರುಗಿ;
	}
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "lnum", (ದೀರ್ಘ)wp->w_jumplist[i].fmark.mark.lnum);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "col", (ದೀರ್ಘ)wp->w_jumplist[i].fmark.mark.ಲಂಬಸಾಲು);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "coladd", (ದೀರ್ಘ)wp->w_jumplist[i].fmark.mark.coladd);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "bufnr", (ದೀರ್ಘ)wp->w_jumplist[i].fmark.fnum);
	ಆದರೆ (wp->w_jumplist[i].fname != ಖಾಲಿ)
	    dict_ಕೂಡಿಸು_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getpid(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getpos(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಖಂಡ_def2str(ರಚನೆ ಖಂಡ_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    ಆದರೆ (ret->string == ಖಾಲಿ)
    {
	ret->ಉದ್ದಳತೆ = 0;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->ಉದ್ದಳತೆ = bd->startspaces;
    mch_memmove(ret->string + ret->ಉದ್ದಳತೆ, bd->textstart, bd->textlen);
    ret->ಉದ್ದಳತೆ += bd->textlen;
    vim_memset(ret->string + ret->ಉದ್ದಳತೆ, ' ', bd->endspaces);
    ret->ಉದ್ದಳತೆ += bd->endspaces;
    ret->string[ret->ಉದ್ದಳತೆ] = NUL;

    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
getregionpos(
    typval_T	*argvars,
    typval_T	*rettv,
    pos_T	*p1,
    pos_T	*p2,
    ಪೂರ್ಣಾಂಕ		*inclusive,
    ಪೂರ್ಣಾಂಕ		*region_type,
    oparg_T	*oap)
{
    ಪೂರ್ಣಾಂಕ		fnum1 = -1, fnum2 = -1;
    char_u	*type;
    buf_T	*findbuf;
    char_u	default_type[] = "v";
    ಪೂರ್ಣಾಂಕ		ಖಂಡ_ಅಗಲ = 0;
    ಪೂರ್ಣಾಂಕ		is_select_exclusive;
    ಪೂರ್ಣಾಂಕ		l;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (list2fpos(&argvars[0], p1, &fnum1, ಖಾಲಿ, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, ಖಾಲಿ, FALSE) != OK
	    || fnum1 != fnum2)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	ಆದರೆ (type == ಖಾಲಿ)
	    type = default_type;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    ಆದರೆ (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	ಆದರೆ (*p != NUL && ((ಖಂಡ_ಅಗಲ = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "type", type);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	*region_type = MBLOCK;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "type", type);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    ಆದರೆ (findbuf == ಖಾಲಿ || findbuf->b_ml.ml_mfp == ಖಾಲಿ)
    {
	emsg(_(e_ಬಫರ್_is_not_loaded));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), p1->lnum);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (p1->ಲಂಬಸಾಲು == MAXCOL)
	p1->ಲಂಬಸಾಲು = ml_get_buf_len(findbuf, p1->lnum) + 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p1->ಲಂಬಸಾಲು < 1 || p1->ಲಂಬಸಾಲು > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_ಸ್ತಂಭ_ಸಂಖ್ಯೆ_nr), p1->ಲಂಬಸಾಲು);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), p2->lnum);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (p2->ಲಂಬಸಾಲು == MAXCOL)
	p2->ಲಂಬಸಾಲು = ml_get_buf_len(findbuf, p2->lnum) + 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p2->ಲಂಬಸಾಲು < 1 || p2->ಲಂಬಸಾಲು > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_ಸ್ತಂಭ_ಸಂಖ್ಯೆ_nr), p2->ಲಂಬಸಾಲು);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    curbuf = findbuf;
    curwin->w_ಬಫರ್ = curbuf;
    virtual_op = virtual_ಸಕ್ರಿಯ();

    // NOTE: Adjustment is needed.
    p1->ಲಂಬಸಾಲು--;
    p2->ಲಂಬಸಾಲು--;

    ಆದರೆ (!LT_POS(*p1, *p2))
    {
	// swap position
	pos_T p;

	p = *p1;
	*p1 = *p2;
	*p2 = p;
    }

    ಆದರೆ (*region_type == MCHAR)
    {
	// Handle 'selection' == "exclusive".
	ಆದರೆ (is_select_exclusive && !EQUAL_POS(*p1, *p2))
	    // When backing up to previous line, inclusive becomes false.
	    *inclusive = !unadjust_for_sel_inner(p2);
	// If p2 is on NUL (end of line), inclusive becomes false.
	ಆದರೆ (*inclusive && !virtual_op && *ml_get_pos(p2) == NUL)
	    *inclusive = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	ಪೂರ್ಣಾಂಕ	lbr_saved = reset_lbr();
#ಆದರೆ_ಅಂತ್ಯ

	getvvcol(curwin, p1, &sc1, ಖಾಲಿ, &ec1, 0);
	getvvcol(curwin, p2, &sc2, ಖಾಲಿ, &ec2, 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#ಆದರೆ_ಅಂತ್ಯ

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->ಆರಂಭ = *p1;
	oap->ಕೊನೆ = *p2;
	oap->ಆರಂಭ_vcol = MIN(sc1, sc2);
	ಆದರೆ (ಖಂಡ_ಅಗಲ > 0)
	    oap->ಕೊನೆ_vcol = oap->ಆರಂಭ_vcol + ಖಂಡ_ಅಗಲ - 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->ಕೊನೆ_vcol = sc2 - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    oap->ಕೊನೆ_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    ಆದರೆ (l > 1)
	p2->ಲಂಬಸಾಲು += l - 1;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * "getregion()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    ಪೂರ್ಣಾಂಕ			inclusive = TRUE;
    ಪೂರ್ಣಾಂಕ			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    ಪೂರ್ಣಾಂಕ			save_virtual;
    string_T		akt = {ಖಾಲಿ, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    ಆದರೆ (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	ಪೂರ್ಣಾಂಕ ret = OK;

	ಆದರೆ (region_type == MBLOCK)
	{
	    ರಚನೆ ಖಂಡ_def	bd;

	    ಖಂಡ_prep(&oa, &bd, lnum, FALSE);
	    ಖಂಡ_def2str(&bd, &akt);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.ಉದ್ದಳತೆ = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.ಉದ್ದಳತೆ);
	    ಆದರೆ (akt.string == ಖಾಲಿ)
		akt.ಉದ್ದಳತೆ = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		akt.ಉದ್ದಳತೆ = s.ಉದ್ದಳತೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ರಚನೆ ಖಂಡ_def	bd;

	    charwise_ಖಂಡ_prep(p1, p2, &bd, lnum, inclusive);
	    ಖಂಡ_def2str(&bd, &akt);
	}

	ಆದರೆ (akt.string == ಖಾಲಿ)
	{
	    clear_tv(rettv);
	    (ಶೂನ್ಯ)rettv_list_alloc(rettv);
	    ನಿಲ್ಲಿಸು;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (ಪೂರ್ಣಾಂಕ)akt.ಉದ್ದಳತೆ);
	vim_free(akt.string);

	ಆದರೆ (ret == FAIL)
	{
	    clear_tv(rettv);
	    (ಶೂನ್ಯ)rettv_list_alloc(rettv);
	    ನಿಲ್ಲಿಸು;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_ಬಫರ್ = curbuf;
    virtual_op = save_virtual;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    ಆದರೆ (l1 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	ಹಿಂತಿರುಗಿ;
    }

    l2 = list_alloc();
    ಆದರೆ (l2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	ಹಿಂತಿರುಗಿ;
    }

    l3 = list_alloc();
    ಆದರೆ (l3 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	ಹಿಂತಿರುಗಿ;
    }

    list_append_ಸಂಖ್ಯೆ(l2, curbuf->b_fnum);
    list_append_ಸಂಖ್ಯೆ(l2, p1.lnum);
    list_append_ಸಂಖ್ಯೆ(l2, p1.ಲಂಬಸಾಲು);
    list_append_ಸಂಖ್ಯೆ(l2, p1.coladd);

    list_append_ಸಂಖ್ಯೆ(l3, curbuf->b_fnum);
    list_append_ಸಂಖ್ಯೆ(l3, p2.lnum);
    list_append_ಸಂಖ್ಯೆ(l3, p2.ಲಂಬಸಾಲು);
    list_append_ಸಂಖ್ಯೆ(l3, p2.coladd);
}

/*
 * Compute the positions of the region segment on line "lnum".
 * "ret_p1" is set to the start position of the segment and "ret_p2" to its
 * end position.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
getregionpos_ಸಾಲು(
    linenr_T	lnum,
    pos_T	p1,
    pos_T	p2,
    ಪೂರ್ಣಾಂಕ		inclusive,
    ಪೂರ್ಣಾಂಕ		region_type,
    oparg_T	*oap,
    ಪೂರ್ಣಾಂಕ		allow_eol,
    pos_T	*ret_p1,
    pos_T	*ret_p2)
{
    char_u	*ಸಾಲು = ml_get(lnum);
    colnr_T	ಸಾಲು_len = ml_get_len(lnum);

    ಆದರೆ (region_type == MLINE)
    {
	ret_p1->ಲಂಬಸಾಲು = 1;
	ret_p1->coladd = 0;
	ret_p2->ಲಂಬಸಾಲು = MAXCOL;
	ret_p2->coladd = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ರಚನೆ ಖಂಡ_def	bd;

	ಆದರೆ (region_type == MBLOCK)
	    ಖಂಡ_prep(oap, &bd, lnum, FALSE);
	ಇಲ್ಲದಿದ್ದರೆ
	    charwise_ಖಂಡ_prep(p1, p2, &bd, lnum, inclusive);

	ಆದರೆ (bd.is_oneChar)  // selection entirely inside one char
	{
	    ಆದರೆ (region_type == MBLOCK)
	    {
		ret_p1->ಲಂಬಸಾಲು = mb_prevptr(ಸಾಲು, bd.textstart) - ಸಾಲು + 1;
		ret_p1->coladd = bd.ಆರಂಭ_char_vcols
					   - (bd.ಆರಂಭ_vcol - oap->ಆರಂಭ_vcol);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ret_p1->ಲಂಬಸಾಲು = p1.ಲಂಬಸಾಲು + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (region_type == MBLOCK && oap->ಆರಂಭ_vcol > bd.ಆರಂಭ_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->ಲಂಬಸಾಲು = MAXCOL;
	    ret_p1->coladd = oap->ಆರಂಭ_vcol - bd.ಆರಂಭ_vcol;
	    bd.is_oneChar = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bd.startspaces > 0)
	{
	    ret_p1->ಲಂಬಸಾಲು = mb_prevptr(ಸಾಲು, bd.textstart) - ಸಾಲು + 1;
	    ret_p1->coladd = bd.ಆರಂಭ_char_vcols - bd.startspaces;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ret_p1->ಲಂಬಸಾಲು = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	ಆದರೆ (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->ಲಂಬಸಾಲು = ret_p1->ಲಂಬಸಾಲು;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bd.endspaces > 0)
	{
	    ret_p2->ಲಂಬಸಾಲು = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ret_p2->ಲಂಬಸಾಲು = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    ಆದರೆ (!allow_eol && ret_p1->ಲಂಬಸಾಲು > ಸಾಲು_len)
    {
	ret_p1->ಲಂಬಸಾಲು = 0;
	ret_p1->coladd = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret_p1->ಲಂಬಸಾಲು > ಸಾಲು_len + 1)
	ret_p1->ಲಂಬಸಾಲು = ಸಾಲು_len + 1;

    ಆದರೆ (!allow_eol && ret_p2->ಲಂಬಸಾಲು > ಸಾಲು_len)
    {
	ret_p2->ಲಂಬಸಾಲು = ret_p1->ಲಂಬಸಾಲು == 0 ? 0 : ಸಾಲು_len;
	ret_p2->coladd = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret_p2->ಲಂಬಸಾಲು > ಸಾಲು_len + 1)
	ret_p2->ಲಂಬಸಾಲು = ಸಾಲು_len + 1;

    ret_p1->lnum = lnum;
    ret_p2->lnum = lnum;
}

/*
 * "getregionpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getregionpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	p1, p2;
    ಪೂರ್ಣಾಂಕ		inclusive = TRUE;
    ಪೂರ್ಣಾಂಕ		region_type = -1;
    ಪೂರ್ಣಾಂಕ		allow_eol = FALSE;
    ಪೂರ್ಣಾಂಕ		bounds_only = FALSE;
    oparg_T	oa;
    ಪೂರ್ಣಾಂಕ		lnum;

    buf_T	*save_curbuf;
    ಪೂರ್ಣಾಂಕ		save_virtual;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    ಆದರೆ (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    ಆದರೆ (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	ಆರಂಭ_pos, ಕೊನೆ_pos;

	getregionpos_ಸಾಲು(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &ಆರಂಭ_pos, &ಕೊನೆ_pos);
	ಆದರೆ (p2.lnum != p1.lnum)
	{
	    pos_T	ಬಳಸದ_2;

	    getregionpos_ಸಾಲು(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &ಬಳಸದ_2, &ಕೊನೆ_pos);
	}
	ಕೂಡಿಸು_regionpos_range(rettv, ಆರಂಭ_pos, ಕೊನೆ_pos);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_ಸಾಲು(lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &ret_p1, &ret_p2);
	    ಕೂಡಿಸು_regionpos_range(rettv, ret_p1, ret_p2);
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_ಬಫರ್ = curbuf;
    virtual_op = save_virtual;
}

/*
 * Common between getreg(), getreginfo() and getregtype(): get the register
 * name from the first argument.
 * Returns zero on error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	ಆದರೆ (strregname != ಖಾಲಿ && ಒಳ_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_ರಿಜಿಸ್ಟರ್_name_must_be_one_char_str), strregname);
	    strregname = ಖಾಲಿ;
	}
	ಆದರೆ (strregname == ಖಾಲಿ)	    // type error; errmsg already given
	    ಹಿಂತಿರುಗಿ 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    ಹಿಂತಿರುಗಿ *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getreg(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		regname;
    ಪೂರ್ಣಾಂಕ		arg2 = FALSE;
    ಪೂರ್ಣಾಂಕ		return_list = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	ಹಿಂತಿರುಗಿ;

    regname = getreg_get_regname(argvars);
    ಆದರೆ (regname == 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಂಕ		error = FALSE;

	arg2 = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[1], &error);

	ಆದರೆ (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[2], &error);
	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC : 0) | GREG_LIST);
	ಆದರೆ (rettv->vval.v_list == ಖಾಲಿ)
	    (ಶೂನ್ಯ)rettv_list_alloc(rettv);
	ಇಲ್ಲದಿದ್ದರೆ
	    ++rettv->vval.v_list->lv_refcount;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = get_reg_contents(regname,
						    arg2 ? GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC : 0);
    }
}

/*
 * "getregtype()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getregtype(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		regname;
    char_u	buf[NUMBUFLEN + 2];
    ದೀರ್ಘ	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    regname = getreg_get_regname(argvars);
    ಆದರೆ (regname == 0)
	ಹಿಂತಿರುಗಿ;

    buf[0] = NUL;
    buf[1] = NUL;
    ಆಯ್ಕೆ (get_reg_type(regname, &reglen))
    {
	ಸ್ಥಿತಿ MLINE: buf[0] = 'V'; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MCHAR: buf[0] = 'v'; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((ಅಕ್ಷರ *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		ನಿಲ್ಲಿಸು;
    }
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "gettagstack()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_gettagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;			// default is current window

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
	ಆದರೆ (wp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_gettext(typval_T *argvars, typval_T *rettv)
{
#ಆದರೆ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    ಅಕ್ಷರ *prev = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_STRING;

    ಆದರೆ (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != ಖಾಲಿ &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#ಆದರೆ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[1].vval.v_string, (ಅಕ್ಷರ *)p_enc);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[1].vval.v_string, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[0].vval.v_string));
#ಇಲ್ಲದಿದ್ದರೆ
	textdomain((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	ಆದರೆ (prev != ಖಾಲಿ)
	    bind_textdomain_codeset((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[1].vval.v_string, prev);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
}

// for VIM_VERSION_ defines
#ಅನ್ತರ್ಭಾವ "version.h"

/*
 * "has()" function
 */
    ಶೂನ್ಯ
f_has(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*name;
    ಪೂರ್ಣಾಂಕ		x = FALSE;
    ಪೂರ್ಣಾಂಕ		n = FALSE;
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
	ಅಕ್ಷರ *name;
	ಗಿಡ್ಡ present;
    } has_ಅಂಶ_T;
    ಸ್ಥಿರ has_ಅಂಶ_T has_list[] =
    {
	{"amiga",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"android",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __ANDROID__
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"arp",
#ಆದರೆ defined(AMIGA) && defined(FEAT_ARP)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"haiku",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __HAIKU__
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"bsd",
#ಆದರೆ defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"hpux",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ hpux
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"hurd",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GNU__
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"linux",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __linux__
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_X
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"osx",		// Mac OS X
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_X
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"macunix",	// Mac OS X, with the darwin feature
#ಆದರೆ defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"osxdarwin",	// synonym for macunix
#ಆದರೆ defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"qnx",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __QNX__
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"sun",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SUN_SYSTEM
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"termux",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __TERMUX__
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"unix",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"vms",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"win32",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"win32unix",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32UNIX
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"win64",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN64
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"ebcdic", 0 },
	{"fname_case",
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CASE_INSENSITIVE_FILENAME
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"acl",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"arabic",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"autocmd", 1},
	{"autochdir",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_AUTOCHDIR
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"autoservername",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_AUTOSERVERNAME
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"socketserver",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"balloon_eval",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"balloon_multiline",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"balloon_eval_term",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_ಪದ
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#ಆದರೆ defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"byte_offset",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BYTEOFF
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"channel",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"cindent", 1},
	{"clientserver",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"clipboard",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"clipboard_provider",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"cmdline_compl", 1},
	{"cmdline_hist", 1},
	{"cmdwin", 1},
	{"comments", 1},
	{"conceal",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"cryptv",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"crypt-blowfish",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"crypt-blowfish2",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"cscope",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"cursorbind", 1},
	{"cursorshape",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"debug",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"dialog_con",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CON_DIALOG
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"dialog_con_gui",
#ಆದರೆ defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"dialog_gui",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_DIALOG
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"diff",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"digraphs",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"directx",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIRECTX
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"dnd",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"drop_file",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_DROP_FILE
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"emacs_tags",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"file_in_path", 1},
	{"filterpipe",
#ಆದರೆ defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"find_in_path",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"float", 1},
	{"folding",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"footer", 0},
	{"fork",
#ಆದರೆ !defined(USE_SYSTEM) && defined(UNIX)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"gettext",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GETTEXT
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"gui",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"gui_gtk2",
#ಆದರೆ defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"gui_gtk3",
#ಆದರೆ defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"gui_gtk4",
#ಆದರೆ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"gui_gnome",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GNOME
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"gui_haiku",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_HAIKU
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"gui_mac", 0},
	{"gui_motif",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MOTIF
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"gui_photon",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_PHOTON
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"gui_win32",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"iconv",
#ಆದರೆ defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"image",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"image_popup",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"insert_expand", 1},
	{"ipv6",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IPV6
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
	},
	{"job",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"jumplist", 1},
	{"keymap",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"libcall",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LIBCALL
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"linebreak",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#ಆದರೆ defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"menu",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"mksession",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"mouse_dec",
#ಆದರೆ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"mouse_gpm",
#ಆದರೆ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"mouse_jsbterm",
#ಆದರೆ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"mouse_netterm",
#ಆದರೆ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"mouse_pterm",
#ಆದರೆ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"mouse_sgr",
#ಆದರೆ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"mouse_sysmouse",
#ಆದರೆ (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"mouse_urxvt",
#ಆದರೆ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"mouse_xterm",
#ಆದರೆ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"multi_lang",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"mzscheme",
#ಆದರೆ defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"nanotime",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ST_MTIM_NSEC
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
	},
	{"num64", 1},
	{"ole",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_OLE
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"packages",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"path_extra", 1},
	{"perl",
#ಆದರೆ defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"persistent_undo",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"python_compiled",
#ಆದರೆ defined(FEAT_PYTHON)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"python_dynamic",
#ಆದರೆ defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"python",
#ಆದರೆ defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"pythonx",
#ಆದರೆ (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"python3_compiled",
#ಆದರೆ defined(FEAT_PYTHON3)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"python3_dynamic",
#ಆದರೆ defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"python3_stable",
#ಆದರೆ defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"python3",
#ಆದರೆ defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"popupwin",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"postscript",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_POSTSCRIPT
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"pango",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINT_PANGO
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"printer",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINTER
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"profile",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"prof_nsec",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PROF_NSEC
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"reltime",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"quickfix",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"rightleft",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"ruby",
#ಆದರೆ defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"smartindent", 1},
	{"startuptime",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ STARTUPTIME
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"statusline",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"statusline_click",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"netbeans_intg",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"sodium",
#ಆದರೆ defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"sound",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOUND
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"spell",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"syntax",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"system",
#ಆದರೆ defined(USE_SYSTEM) || !defined(UNIX)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"tabpanel",
#ಆದರೆ defined(FEAT_TABPANEL)
		1,
#ಇಲ್ಲದಿದ್ದರೆ
		0,
#ಆದರೆ_ಅಂತ್ಯ
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#ಆದರೆ defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"termguicolors",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"terminal",
#ಆದರೆ defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"terminfo",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"termresponse",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"textobjects", 1},
	{"textprop",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"tgetent",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"timers",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"title", 1},
	{"toolbar",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"vertsplit", 1},
	{"viminfo",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VTP
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"wayland",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"wayland_clipboard",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND_CLIPBOARD
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAK
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"writebackup",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WRITEBACKUP
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"xattr",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XATTR
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"xim",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"xfontset",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"xpm",
#ಆದರೆ defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"xpm_w32",	// for backward compatibility
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XPM_W32
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"xsmp",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_XSMP
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"xsmp_interact",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_XSMP_INTERACT
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"xterm_clipboard",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XCLIPBOARD
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"xterm_save",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XTERM_SAVE
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{"X11",
#ಆದರೆ defined(UNIX) && defined(FEAT_X11)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#ಆದರೆ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#ಇಲ್ಲದಿದ್ದರೆ
		0
#ಆದರೆ_ಅಂತ್ಯ
		},
	{ಖಾಲಿ, 0}
    };

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string(&argvars[0]);

    // Fast-path: check features not in has_list[] first to avoid the full
    // linear scan for very common queries like has('patch-...').
    ಆದರೆ (STRNICMP(name, "patch", 5) == 0)
    {
	x = TRUE;
	ಆದರೆ (name[5] == '-'
		&& STRLEN(name) >= 11
		&& (name[6] >= '1' && name[6] <= '9'))
	{
	    ಅಕ್ಷರ	*ಕೊನೆ;
	    ಪೂರ್ಣಾಂಕ	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (ಪೂರ್ಣಾಂಕ)strtoul((ಅಕ್ಷರ *)name + 6, &ಕೊನೆ, 10);
	    ಆದರೆ (*ಕೊನೆ == '.' && vim_isdigit(ಕೊನೆ[1])
		    && ಕೊನೆ[2] == '.' && vim_isdigit(ಕೊನೆ[3]))
	    {
		minor = atoi(ಕೊನೆ + 1);

		// Expect "patch-9.9.01234".
		n = (major < VIM_ಆವೃತ್ತಿ_MAJOR
		     || (major == VIM_ಆವೃತ್ತಿ_MAJOR
			 && (minor < VIM_ಆವೃತ್ತಿ_MINOR
			     || (minor == VIM_ಆವೃತ್ತಿ_MINOR
				 && has_patch(atoi(ಕೊನೆ + 3))))));
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸುರಕ್ಷಿತ_isdigit(name[5]))
	    n = has_patch(atoi((ಅಕ್ಷರ *)name + 5));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_ಒಳಹರಿವು_isatty();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	n = (gui.ಒಳ_use || gui.starting);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#ಆದರೆ defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.ಒಳ_use;	// gui_mch_browse() works when GUI is running
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	n = syntax_present(curwin);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VTP
	n = is_ಪದ_win32() && has_vtp_working();
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	n = netbeans_ಸಕ್ರಿಯ();
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_GPM
	n = gpm_enabled();
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#ಆದರೆ defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# ಆದರೆ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	ಆದರೆ (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    n = FALSE;
# ಇಲ್ಲದಿದ್ದರೆ
	n = FALSE;
# endif
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Look up in has_list[] only if not already handled above.
    ಆದರೆ (x == FALSE)
    {
	ಗಾಗಿ (i = 0; has_list[i].name != ಖಾಲಿ; ++i)
	    ಆದರೆ (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		ನಿಲ್ಲಿಸು;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	ಆದರೆ (x == TRUE && n == FALSE)
	{
	    ಆದರೆ (0)
	    {
		// intentionally empty
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VIMDLL
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "filterpipe") == 0)
		n = gui.ಒಳ_use || gui.starting;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_LUA
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_MZSCHEME
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_PERL
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_PYTHON
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_PYTHON3
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "pythonx") == 0)
	    {
# ಆದರೆ defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		ಆದರೆ (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_pyx == 3)
		    n = python3_enabled(FALSE);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_RUBY
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_TCL
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_SODIUM
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_TERMINAL) && defined(MSWIN)
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_GPM
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_ಲಭ್ಯ();
#ಆದರೆ_ಅಂತ್ಯ
	}
    }

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_ಸಂಖ್ಯೆ = x;
    ಇಲ್ಲದಿದ್ದರೆ
	// return whether feature is enabled
	rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    ಪೂರ್ಣಾಂಕ
dynamic_feature(char_u *feature)
{
    ಹಿಂತಿರುಗಿ (feature == ಖಾಲಿ
#ಆದರೆ defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.ಒಳ_use)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.ಒಳ_use)
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    || STRICMP(feature, "iconv") == 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_LUA
	    || STRICMP(feature, "lua") == 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_GPM
	    || (STRICMP(feature, "mouse_gpm_enabled") == 0 && !gpm_enabled())
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_MZSCHEME
	    || STRICMP(feature, "mzscheme") == 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	    || STRICMP(feature, "netbeans_enabled") == 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_PERL
	    || STRICMP(feature, "perl") == 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_PYTHON
	    || STRICMP(feature, "python") == 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_PYTHON3
	    || STRICMP(feature, "python3") == 0
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    || STRICMP(feature, "pythonx") == 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_RUBY
	    || STRICMP(feature, "ruby") == 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    || STRICMP(feature, "syntax_items") == 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_TCL
	    || STRICMP(feature, "tcl") == 0
#ಆದರೆ_ಅಂತ್ಯ
	    // once "starting" is zero it will stay that way
	    || (STRICMP(feature, "vim_starting") == 0 && starting != 0)
	    || STRICMP(feature, "multi_byte_encoding") == 0
#ಆದರೆ defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#ಆದರೆ_ಅಂತ್ಯ
	    );
}

/*
 * "haslocaldir()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = ಖಾಲಿ;
    win_T	*wp = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    wp = ಕಂಡುಹಿಡಿ_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    ಆದರೆ (wp != ಖಾಲಿ && wp->w_localdir != ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tp != ಖಾಲಿ && tp->tp_localdir != ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = 2;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
}

/*
 * "highlightID(name)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_hlID(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_hostname(typval_T *argvars ಬಳಸದ, typval_T *rettv)
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS // VMS does not have defined uintptr_t
# ಆದರೆ defined(HAVE_NO_LONG_LONG)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uintptr_t;
# ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ uintptr_t;
# endif
#ಆದರೆ_ಅಂತ್ಯ // VMS

    ಸ್ಥಿರ ಶೂನ್ಯ
f_id(typval_T *argvars, typval_T *rettv)
{
    ಅಕ್ಷರ    numbuf[NUMBUFLEN];
    ಅಕ್ಷರ    *p = numbuf;

    ಆಯ್ಕೆ (argvars[0].v_type)
    {
	ಸ್ಥಿತಿ VAR_LIST:
	ಸ್ಥಿತಿ VAR_TUPLE:
	ಸ್ಥಿತಿ VAR_DICT:
	ಸ್ಥಿತಿ VAR_OBJECT:
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	ಸ್ಥಿತಿ VAR_CHANNEL:
	ಸ್ಥಿತಿ VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    ಆದರೆ (argvars[0].vval.v_object != ಖಾಲಿ)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(ಶೂನ್ಯ *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		ಗಾಗಿ (; p < numbuf + ಗಾತ್ರ(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸೂಚ್ಯಂಕ_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ = 0;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    ಪೂರ್ಣಾಂಕ		ic = FALSE;

    b = argvars[0].vval.v_blob;
    ಆದರೆ (b == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಆರಂಭ = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಆರಂಭ < 0)
    {
	ಆರಂಭ = blob_len(b) + ಆರಂಭ;
	ಆದರೆ (ಆರಂಭ < 0)
	    ಆರಂಭ = 0;
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = ಆರಂಭ; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_ಸಂಖ್ಯೆ;
	tv.vval.v_ಸಂಖ್ಯೆ = blob_get(b, idx);
	ಆದರೆ (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = idx;
	    ಹಿಂತಿರುಗಿ;
	}
    }
}

/*
 * index() function for a tuple
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸೂಚ್ಯಂಕ_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    ಪೂರ್ಣಾಂಕ		ic = FALSE;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಆದರೆ (tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ	ಆರಂಭ_idx = 0;
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಆರಂಭ_idx = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಆದರೆ (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[3], &error);
	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = ಸೂಚ್ಯಂಕ_tuple(tuple, &argvars[1], ಆರಂಭ_idx, ic);
}

/*
 * index() function for a list
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸೂಚ್ಯಂಕ_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*ಅಂಶ;
    ದೀರ್ಘ	idx = 0;
    ಪೂರ್ಣಾಂಕ		ic = FALSE;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    l = argvars[0].vval.v_list;
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    CHECK_LIST_MATERIALIZE(l);
    ಅಂಶ = l->lv_ಮೊದಲ;
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	ಅಂಶ = list_ಕಂಡುಹಿಡಿ(l, (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	ಆದರೆ (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[3], &error);
	ಆದರೆ (error)
	    ಅಂಶ = ಖಾಲಿ;
    }

    ಗಾಗಿ ( ; ಅಂಶ != ಖಾಲಿ; ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ, ++idx)
	ಆದರೆ (tv_equal(&ಅಂಶ->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = idx;
	    ನಿಲ್ಲಿಸು;
	}
}

/*
 * "index()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ಸೂಚ್ಯಂಕ(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_BLOB)
	ಸೂಚ್ಯಂಕ_func_blob(argvars, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_TUPLE)
	ಸೂಚ್ಯಂಕ_func_tuple(argvars, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_LIST)
	ಸೂಚ್ಯಂಕ_func_list(argvars, rettv);
    ಇಲ್ಲದಿದ್ದರೆ
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    ಪೂರ್ಣಾಂಕ
indexof_eval_ಅಭಿವ್ಯಕ್ತಿ(typval_T *ಅಭಿವ್ಯಕ್ತಿ)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	ಸಿಕ್ಕಿತು;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_ಕೀಲಿ);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    ಆದರೆ (eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, FALSE, argv, 2, ಖಾಲಿ, &newtv) == FAIL)
	ಹಿಂತಿರುಗಿ FALSE;

    ಸಿಕ್ಕಿತು = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    ಹಿಂತಿರುಗಿ error ? FALSE : ಸಿಕ್ಕಿತು;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
indexof_blob(blob_T *b, ದೀರ್ಘ startidx, typval_T *ಅಭಿವ್ಯಕ್ತಿ)
{
    ದೀರ್ಘ	idx = 0;

    ಆದರೆ (b == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	ಆದರೆ (startidx < 0)
	    startidx = 0;
    }

    ಗಣ_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);
    ಗಣ_vim_var_type(VV_VAL, VAR_ಸಂಖ್ಯೆ);

    ಪೂರ್ಣಾಂಕ		called_emsg_ಆರಂಭ = called_emsg;
    ಗಾಗಿ (idx = startidx; idx < blob_len(b); ++idx)
    {
	ಗಣ_vim_var_nr(VV_ಕೀಲಿ, idx);
	ಗಣ_vim_var_nr(VV_VAL, blob_get(b, idx));

	ಆದರೆ (indexof_eval_ಅಭಿವ್ಯಕ್ತಿ(ಅಭಿವ್ಯಕ್ತಿ))
	    ಹಿಂತಿರುಗಿ idx;

	ಆದರೆ (called_emsg != called_emsg_ಆರಂಭ)
	    ಹಿಂತಿರುಗಿ -1;
    }

    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
indexof_list(list_T *l, ದೀರ್ಘ startidx, typval_T *ಅಭಿವ್ಯಕ್ತಿ)
{
    listitem_T	*ಅಂಶ;
    ದೀರ್ಘ	idx = 0;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು;

    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    CHECK_LIST_MATERIALIZE(l);

    ಆದರೆ (startidx == 0)
	ಅಂಶ = l->lv_ಮೊದಲ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	ಅಂಶ = list_ಕಂಡುಹಿಡಿ(l, startidx);
	ಆದರೆ (ಅಂಶ != ಖಾಲಿ)
	    idx = l->lv_u.mat.lv_idx;
    }

    ಗಣ_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);

    ಪೂರ್ಣಾಂಕ		called_emsg_ಆರಂಭ = called_emsg;
    ಗಾಗಿ ( ; ಅಂಶ != ಖಾಲಿ; ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ, ++idx)
    {
	ಗಣ_vim_var_nr(VV_ಕೀಲಿ, idx);
	copy_tv(&ಅಂಶ->li_tv, get_vim_var_tv(VV_VAL));

	ಸಿಕ್ಕಿತು = indexof_eval_ಅಭಿವ್ಯಕ್ತಿ(ಅಭಿವ್ಯಕ್ತಿ);
	clear_tv(get_vim_var_tv(VV_VAL));

	ಆದರೆ (ಸಿಕ್ಕಿತು)
	    ಹಿಂತಿರುಗಿ idx;

	ಆದರೆ (called_emsg != called_emsg_ಆರಂಭ)
	    ಹಿಂತಿರುಗಿ -1;
    }

    ಹಿಂತಿರುಗಿ -1;
}

/*
 * "indexof()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_indexof(typval_T *argvars, typval_T *rettv)
{
    ದೀರ್ಘ	startidx = 0;
    typval_T	save_val;
    typval_T	save_ಕೀಲಿ;
    ಪೂರ್ಣಾಂಕ		save_did_emsg;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == ಖಾಲಿ
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_ಸಂಖ್ಯೆ_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_ಕೀಲಿ, &save_ಕೀಲಿ);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    ಆದರೆ (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_ಸಂಖ್ಯೆ = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_ಸಂಖ್ಯೆ = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_ಕೀಲಿ, &save_ಕೀಲಿ);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ inputsecret_flag = 0;

/*
 * "input()" function
 *     Also handles inputsecret() when inputsecret is set.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ಒಳಹರಿವು(typval_T *argvars, typval_T *rettv)
{
    get_user_ಒಳಹರಿವು(argvars, rettv, FALSE, inputsecret_flag);
}

/*
 * "inputdialog()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_inputdialog(typval_T *argvars, typval_T *rettv)
{
#ಆದರೆ defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    ಆದರೆ (gui.ಒಳ_use && vim_strchr(p_go, GO_CONDIALOG) == ಖಾಲಿ)
    {
	char_u	*ಸಂದೇಶ_2;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	ಆದರೆ (ಒಳ_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    ಹಿಂತಿರುಗಿ;

	ಸಂದೇಶ_2 = tv_get_string_chk(&argvars[0]);
	ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != ಖಾಲಿ)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	ಇಲ್ಲದಿದ್ದರೆ
	    IObuff[0] = NUL;
	ಆದರೆ (ಸಂದೇಶ_2 != ಖಾಲಿ && defstr != ಖಾಲಿ
		&& do_dialog(VIM_QUESTION, ಖಾಲಿ, ಸಂದೇಶ_2,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (ಸಂದೇಶ_2 != ಖಾಲಿ && defstr != ಖಾಲಿ
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    ಇಲ್ಲದಿದ್ದರೆ
		rettv->vval.v_string = ಖಾಲಿ;
	}
	rettv->v_type = VAR_STRING;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	get_user_ಒಳಹರಿವು(argvars, rettv, TRUE, inputsecret_flag);
}

/*
 * "inputlist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_inputlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		selected;
    ಪೂರ್ಣಾಂಕ		mouse_ಬಳಸಿದ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NO_CONSOLE_ಒಳಹರಿವು
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    ಆದರೆ (no_console_ಒಳಹರಿವು() && !is_not_a_ಪದ())
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಒಳ_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ಖಾಲಿ)
    {
	semsg(_(e_ವಾದ_ದ_str_must_be_list), "inputlist()");
	ಹಿಂತಿರುಗಿ;
    }

    ಸಂದೇಶ_ಆರಂಭ();
    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;	// for when 'cmdheight' > 1
    ಸಾಲುಗಳು_ಎಡ = ಅಡ್ಡಸಾಲುಗಳು;	// avoid more prompt
    ಸಂದೇಶ_scroll = TRUE;
    ಸಂದೇಶ_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಸಂದೇಶ_puts((ಅಕ್ಷರ *)tv_get_string(&li->li_tv));
	ಸಂದೇಶ_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_ಸಂಖ್ಯೆ(&mouse_ಬಳಸಿದ);
    ಆದರೆ (mouse_ಬಳಸಿದ)
	selected -= ಸಾಲುಗಳು_ಎಡ;

    rettv->vval.v_ಸಂಖ್ಯೆ = selected;
}

ಸ್ಥಿರ garray_T	    ga_userinput = {0, 0, ಗಾತ್ರ(tasave_T), 4, ಖಾಲಿ};

/*
 * "inputrestore()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_inputrestore(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಆದರೆ (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_ದತ್ತಾಂಶ)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_verbose > 1)
    {
	verb_ಸಂದೇಶ(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_ಸಂಖ್ಯೆ = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_inputsave(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    ಆದರೆ (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_ದತ್ತಾಂಶ)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ++cmdline_ನಕ್ಷತ್ರ;
    ++inputsecret_flag;
    f_ಒಳಹರಿವು(argvars, rettv);
    --cmdline_ನಕ್ಷತ್ರ;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ಅಡಚಣೆ(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_invert(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = ~tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_lval_ಬೇರು(lval_ಬೇರು_T *ಬೇರು)
{
    ಆದರೆ (ಬೇರು->lr_tv != ಖಾಲಿ)
	free_tv(ಬೇರು->lr_tv);
    class_unref(ಬೇರು->lr_cl_exec);
    ಬೇರು->lr_tv = ಖಾಲಿ;
    ಬೇರು->lr_cl_exec = ಖಾಲಿ;
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

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fix_variable_reference_lval_ಬೇರು(lval_ಬೇರು_T *ಬೇರು, char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು_member = FALSE;
    ಆದರೆ (ಬೇರು->lr_tv->v_type == VAR_OBJECT)
    {
	ಆದರೆ (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    ಸಿಕ್ಕಿತು_member = TRUE;
    }
    ಆದರೆ (!ಸಿಕ್ಕಿತು_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*ಕೊನೆ;
	ಗಾಗಿ (ಕೊನೆ = name; ASCII_ISALNUM(*ಕೊನೆ) || *ಕೊನೆ == '_'; ++ಕೊನೆ)
	    ;
	ಪೂರ್ಣಾಂಕ idx = class_member_idx(ಬೇರು->lr_cl_exec, name, ಕೊನೆ - name);
	ಆದರೆ (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(ಬೇರು->lr_tv);
	    copy_tv(&ಬೇರು->lr_cl_exec->class_members_tv[idx], ಬೇರು->lr_tv);
	    ಸಿಕ್ಕಿತು_member = TRUE;
	}
    }
    ಆದರೆ (!ಸಿಕ್ಕಿತು_member)
    {
	free_tv(ಬೇರು->lr_tv);
	ಬೇರು->lr_tv = ಖಾಲಿ;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * "islocked()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*ಕೊನೆ;
    dictitem_T	*di;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    char_u *name = tv_get_string(&argvars[0]);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
    ch_log(ಖಾಲಿ, "LKVAR: f_islocked(): name: %s", name);
#ಆದರೆ_ಅಂತ್ಯ

    lval_ಬೇರು_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_ಬೇರು_T *ಬೇರು = ಖಾಲಿ;

    // Set up lval_root if executing in a method.
    ಆದರೆ (fill_exec_lval_ಬೇರು(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	ಆದರೆ (fix_variable_reference_lval_ಬೇರು(&aroot, name) == OK)
	    ಬೇರು = &aroot;
    }

    lval_ಬೇರು_T	*lval_ಬೇರು_save = lval_ಬೇರು;
    lval_ಬೇರು = ಬೇರು;
    ಕೊನೆ = get_lval(name, ಖಾಲಿ, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_ಆರಂಭ);
    lval_ಬೇರು = lval_ಬೇರು_save;

    ಆದರೆ (ಕೊನೆ != ಖಾಲಿ && lv.ll_name != ಖಾಲಿ)
    {
	ಆದರೆ (*ಕೊನೆ != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_ಕೊನೆ
		   ? e_invalid_ವಾದ_str : e_trailing_characters_str), ಕೊನೆ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (lv.ll_tv == ಖಾಲಿ)
	    {
		di = ಕಂಡುಹಿಡಿ_var(lv.ll_name, ಖಾಲಿ, TRUE);
		ಆದರೆ (di != ಖಾಲಿ)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_ಸಂಖ್ಯೆ = ((di->di_flags & DI_FLAGS_ಬೀಗ)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lv.ll_is_ಬೇರು)
	    {
		rettv->vval.v_ಸಂಖ್ಯೆ = tv_islocked(lv.ll_tv);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lv.ll_object != ಖಾಲಿ)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_ಸಂಖ್ಯೆ = tv_islocked(tv);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
		ch_log(ಖಾಲಿ, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lv.ll_class != ಖಾಲಿ)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_ಸಂಖ್ಯೆ = tv_islocked(tv);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
		ch_log(ಖಾಲಿ, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lv.ll_newkey != ಖಾಲಿ)
		semsg(_(e_ಕೀಲಿ_not_present_ಒಳ_dictionary_str), lv.ll_newkey);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lv.ll_list != ಖಾಲಿ)
		// List item.
		rettv->vval.v_ಸಂಖ್ಯೆ = tv_islocked(&lv.ll_li->li_tv);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lv.ll_tuple != ಖಾಲಿ)
		// Tuple item.
		rettv->vval.v_ಸಂಖ್ಯೆ = tv_islocked(lv.ll_tv);
	    ಇಲ್ಲದಿದ್ದರೆ
		// Dictionary item.
		rettv->vval.v_ಸಂಖ್ಯೆ = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    ಆದರೆ (ಬೇರು != ಖಾಲಿ)
	free_lval_ಬೇರು(ಬೇರು);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ವಸ್ತುಗಳು(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_ವಾದ_nr), 1);
}

/*
 * "keytrans()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ಕೊನೆಯ_ಬಫರ್_nr(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ಆದರೆ (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

/*
 * "len()" function
 */
    ಶೂನ್ಯ
f_len(typval_T *argvars, typval_T *rettv)
{
    ಆಯ್ಕೆ (argvars[0].v_type)
    {
	ಸ್ಥಿತಿ VAR_STRING:
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BLOB:
	    rettv->vval.v_ಸಂಖ್ಯೆ = blob_len(argvars[0].vval.v_blob);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    rettv->vval.v_ಸಂಖ್ಯೆ = list_len(argvars[0].vval.v_list);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    rettv->vval.v_ಸಂಖ್ಯೆ = tuple_len(argvars[0].vval.v_tuple);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    rettv->vval.v_ಸಂಖ್ಯೆ = dict_len(argvars[0].vval.v_dict);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_OBJECT:
	    rettv->vval.v_ಸಂಖ್ಯೆ = object_len(argvars[0].vval.v_object);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	ಸ್ಥಿತಿ VAR_FLOAT:
	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	ಸ್ಥಿತಿ VAR_CHANNEL:
	ಸ್ಥಿತಿ VAR_INSTR:
	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    ನಿಲ್ಲಿಸು;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
libcall_common(typval_T *argvars ಬಳಸದ, typval_T *rettv, ಪೂರ್ಣಾಂಕ type)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LIBCALL
    char_u		*string_ಒಳ;
    char_u		**string_ಫಲಿತಾಂಶ;
    ಪೂರ್ಣಾಂಕ			nr_ಫಲಿತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ

    rettv->v_type = type;
    ಆದರೆ (type != VAR_ಸಂಖ್ಯೆ)
	rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    ಆದರೆ (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_ಒಳ = ಖಾಲಿ;
	ಆದರೆ (argvars[2].v_type == VAR_STRING)
	    string_ಒಳ = argvars[2].vval.v_string;
	ಆದರೆ (type == VAR_ಸಂಖ್ಯೆ)
	{
	    string_ಫಲಿತಾಂಶ = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    rettv->vval.v_string = ಖಾಲಿ;
	    string_ಫಲಿತಾಂಶ = &rettv->vval.v_string;
	}
	ಆದರೆ (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_ಒಳ,
			     argvars[2].vval.v_ಸಂಖ್ಯೆ,
			     string_ಫಲಿತಾಂಶ,
			     &nr_ಫಲಿತಾಂಶ) == OK
		&& type == VAR_ಸಂಖ್ಯೆ)
	    rettv->vval.v_ಸಂಖ್ಯೆ = nr_ಫಲಿತಾಂಶ;
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "libcall()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_libcall(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_STRING);
}

/*
 * "libcallnr()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_libcallnr(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_ಸಂಖ್ಯೆ);
}

/*
 * "line(string, [winid])" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ಸಾಲು(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum = 0;
    pos_T	*fp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		fnum;
    ಪೂರ್ಣಾಂಕ		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	ಆದರೆ (wp != ಖಾಲಿ && tp != ಖಾಲಿ)
	{
	    ಆದರೆ (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		ಆದರೆ (*p_spk != 'c'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		|| (curwin->w_p_diff && switchwin.sw_curwin->w_p_diff)
#ಆದರೆ_ಅಂತ್ಯ
		)
		    skip_update_topline = TRUE;
		check_cursor();
		fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	    }
	    skip_update_topline = FALSE;
	    restore_win_noblock(&switchwin, TRUE);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    ಆದರೆ (fp != ಖಾಲಿ)
	lnum = fp->lnum;
    rettv->vval.v_ಸಂಖ್ಯೆ = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_line2byte(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_BYTEOFF
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
#ಇಲ್ಲದಿದ್ದರೆ
    linenr_T	lnum;

    ಆದರೆ (ಒಳ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    lnum = tv_get_lnum(argvars);
    ಆದರೆ (lnum < 1 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು_or_ಸ್ಥಾನಾಂತರ(curbuf, lnum, ಖಾಲಿ);
    ಆದರೆ (rettv->vval.v_ಸಂಖ್ಯೆ >= 0)
	++rettv->vval.v_ಸಂಖ್ಯೆ;
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LUA
/*
 * "luaeval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    ಹೊಂದಿಕೆ_ಕೊನೆ_2,	    // matchend()
    ಹೊಂದಿಕೆ_ಹೊಂದಿಕೆ,    // match()
    ಹೊಂದಿಕೆ_STR,	    // matchstr()
    ಹೊಂದಿಕೆ_LIST,	    // matchlist()
    ಹೊಂದಿಕೆ_POS	    // matchstrpos()
} matchtype_T;

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_some_ಹೊಂದಿಕೆ(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = ಖಾಲಿ;
    ದೀರ್ಘ	len = 0;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ = ಖಾಲಿ;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    ದೀರ್ಘ	ಆರಂಭ = 0;
    ದೀರ್ಘ	nth = 1;
    colnr_T	startcol = 0;
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ = 0;
    list_T	*l = ಖಾಲಿ;
    listitem_T	*li = ಖಾಲಿ;
    ದೀರ್ಘ	idx = 0;
    ಪೂರ್ಣಾಂಕ		prev_ಬೀಗ = 0;
    char_u	*tofree = ಖಾಲಿ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ಖಾಲಿಗಳು_option;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಆದರೆ (type == ಹೊಂದಿಕೆ_LIST || type == ಹೊಂದಿಕೆ_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	    ಹೋಗು theend;
	ಆದರೆ (type == ಹೊಂದಿಕೆ_POS
		&& (list_append_string(rettv->vval.v_list,
					    (char_u *)"", 0) == FAIL
		    || list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL))
	{
		list_free(rettv->vval.v_list);
		rettv->vval.v_list = ಖಾಲಿ;
		ಹೋಗು theend;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == ಹೊಂದಿಕೆ_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ಖಾಲಿ;
    }

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL)))
	ಹೋಗು theend;

    ಆದರೆ (argvars[0].v_type == VAR_LIST)
    {
	ಆದರೆ ((l = argvars[0].vval.v_list) == ಖಾಲಿ)
	    ಹೋಗು theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_ಮೊದಲ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಅಭಿವ್ಯಕ್ತಿ = str = tv_get_string(&argvars[0]);
	len = (ದೀರ್ಘ)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    ಆದರೆ (pat == ಖಾಲಿ)
	ಹೋಗು theend;

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಂಕ	    error = FALSE;

	ಆರಂಭ = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಆದರೆ (error)
	    ಹೋಗು theend;
	ಆದರೆ (l != ಖಾಲಿ)
	{
	    li = list_ಕಂಡುಹಿಡಿ(l, ಆರಂಭ);
	    ಆದರೆ (li == ಖಾಲಿ)
		ಹೋಗು theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (ಆರಂಭ < 0)
		ಆರಂಭ = 0;
	    ಆದರೆ (ಆರಂಭ > len)
		ಹೋಗು theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    ಆದರೆ (argvars[3].v_type != VAR_UNKNOWN)
		startcol = ಆರಂಭ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		str += ಆರಂಭ;
		len -= ಆರಂಭ;
	    }
	}

	ಆದರೆ (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[3], &error);
	ಆದರೆ (error)
	    ಹೋಗು theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	ಆದರೆ (l != ಖಾಲಿ)
	{
	    prev_ಬೀಗ = l->lv_ಬೀಗ;
	    ಆದರೆ (l->lv_ಬೀಗ == 0)
		l->lv_ಬೀಗ = VAR_LOCKED;
	}

	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (l != ಖಾಲಿ)
	    {
		ಆದರೆ (li == ಖಾಲಿ)
		{
		    ಹೊಂದಿಕೆ = FALSE;
		    ನಿಲ್ಲಿಸು;
		}
		vim_free(tofree);
		ಅಭಿವ್ಯಕ್ತಿ = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		ಆದರೆ (str == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
	    }

	    ಹೊಂದಿಕೆ = vim_regexec_nl(&regmatch, str, startcol);

	    ಆದರೆ (ಹೊಂದಿಕೆ && --nth <= 0)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (l == ಖಾಲಿ && !ಹೊಂದಿಕೆ)
		ನಿಲ್ಲಿಸು;

	    // Advance to just after the match.
	    ಆದರೆ (l != ಖಾಲಿ)
	    {
		li = li->li_ಮುಂದಿನ;
		++idx;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		ಆದರೆ (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    ಹೊಂದಿಕೆ = FALSE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}

	ಆದರೆ (ಹೊಂದಿಕೆ)
	{
	    ಆದರೆ (type == ಹೊಂದಿಕೆ_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_ಮೊದಲ;
		listitem_T *li2 = li1->li_ಮುಂದಿನ;
		listitem_T *li3 = li2->li_ಮುಂದಿನ;
		listitem_T *li4 = li3->li_ಮುಂದಿನ;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_ಸಂಖ್ಯೆ =
				      (varnumber_T)(regmatch.startp[0] - ಅಭಿವ್ಯಕ್ತಿ);
		li4->li_tv.vval.v_ಸಂಖ್ಯೆ =
					(varnumber_T)(regmatch.endp[0] - ಅಭಿವ್ಯಕ್ತಿ);
		ಆದರೆ (l != ಖಾಲಿ)
		    li2->li_tv.vval.v_ಸಂಖ್ಯೆ = (varnumber_T)idx;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == ಹೊಂದಿಕೆ_LIST)
	    {
		ಪೂರ್ಣಾಂಕ i;

		// return list with matched string and submatches
		ಗಾಗಿ (i = 0; i < NSUBEXP; ++i)
		{
		    ಆದರೆ (regmatch.endp[i] == ಖಾಲಿ)
		    {
			ಆದರೆ (list_append_string(rettv->vval.v_list,
							    ಖಾಲಿ, 0) == FAIL)
			    ನಿಲ್ಲಿಸು;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(ಪೂರ್ಣಾಂಕ)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			ನಿಲ್ಲಿಸು;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == ಹೊಂದಿಕೆ_STR)
	    {
		// return matched string
		ಆದರೆ (l != ಖಾಲಿ)
		    copy_tv(&li->li_tv, rettv);
		ಇಲ್ಲದಿದ್ದರೆ
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (l != ಖಾಲಿ)
		rettv->vval.v_ಸಂಖ್ಯೆ = idx;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (type != ಹೊಂದಿಕೆ_ಕೊನೆ_2)
		    rettv->vval.v_ಸಂಖ್ಯೆ =
				      (varnumber_T)(regmatch.startp[0] - str);
		ಇಲ್ಲದಿದ್ದರೆ
		    rettv->vval.v_ಸಂಖ್ಯೆ =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_ಸಂಖ್ಯೆ += (varnumber_T)(str - ಅಭಿವ್ಯಕ್ತಿ);
	    }
	}
	ಆದರೆ (l != ಖಾಲಿ)
	    l->lv_ಬೀಗ = prev_ಬೀಗ;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    ಆದರೆ (type == ಹೊಂದಿಕೆ_POS && l == ಖಾಲಿ && rettv->vval.v_list != ಖಾಲಿ)
	// matchstrpos() without a list: drop the second item.
	listitem_remove(rettv->vval.v_list,
				       rettv->vval.v_list->lv_ಮೊದಲ->li_ಮುಂದಿನ);
    vim_free(tofree);
    p_cpo = save_cpo;
}

/*
 * Return all the matches in string "str" for pattern "rmp".
 * The matches are returned in the List "mlist".
 * If "submatches" is TRUE, then submatch information is also returned.
 * "matchbuf" is TRUE when called for matchbufline().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_matches_ಒಳ_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    ಪೂರ್ಣಾಂಕ		idx,
    ಪೂರ್ಣಾಂಕ		submatches,
    ಪೂರ್ಣಾಂಕ		matchbuf)
{
    ದೀರ್ಘ	len = (ದೀರ್ಘ)STRLEN(str);
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ = 0;
    colnr_T	startidx = 0;

    ಗಾಗಿ (;;)
    {
	ಹೊಂದಿಕೆ = vim_regexec_nl(rmp, str, startidx);
	ಆದರೆ (!ಹೊಂದಿಕೆ)
	    ನಿಲ್ಲಿಸು;

	dict_T *d = dict_alloc();
	ಆದರೆ (d == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (dict_ಕೂಡಿಸು_string_len(d, "text", rmp->startp[0],
		    (ಪೂರ್ಣಾಂಕ)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (submatches)
	{
	    list_T *sml = list_alloc();
	    ಆದರೆ (sml == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;

	    ಆದರೆ (dict_ಕೂಡಿಸು_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    // return a list with the submatches
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < NSUBEXP; ++i)
	    {
		ಆದರೆ (rmp->endp[i] == ಖಾಲಿ)
		{
		    ಆದರೆ (list_append_string(sml, (char_u *)"", 0) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (list_append_string(sml, rmp->startp[i],
			    (ಪೂರ್ಣಾಂಕ)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	ಆದರೆ (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * "matchbufline()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ಖಾಲಿ;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಆದರೆ (rettv_list_alloc(rettv) != OK)
	ಹಿಂತಿರುಗಿ;
    retlist = rettv->vval.v_list;

    ಆದರೆ (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ prev_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    ಆದರೆ (buf == ಖಾಲಿ)
    {
	ಆದರೆ (did_emsg == prev_did_emsg)
	    semsg(_(e_invalid_ಬಫರ್_name_str), tv_get_string(&argvars[0]));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)
    {
	emsg(_(e_ಬಫರ್_is_not_loaded));
	ಹಿಂತಿರುಗಿ;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    ಆದರೆ (did_emsg > did_emsg_before)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (slnum < 1)
    {
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "lnum");
	ಹಿಂತಿರುಗಿ;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    ಆದರೆ (did_emsg > did_emsg_before)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "end_lnum");
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (elnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	elnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಪೂರ್ಣಾಂಕ		submatches = FALSE;
    ಆದರೆ (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	ಆದರೆ (d != ಖಾಲಿ)
	{
	    dictitem_T *di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"submatches", -1);
	    ಆದರೆ (di != ಖಾಲಿ)
	    {
		ಆದರೆ (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "submatches");
		    ಹಿಂತಿರುಗಿ;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ಖಾಲಿಗಳು_option;

    regmatch.regprog = eval_regcomp(pat);
    ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
	ಹೋಗು theend;
    regmatch.rm_ic = p_ic;

    ಆಗ (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	ಆದರೆ (get_matches_ಒಳ_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    ಹೋಗು cleanup;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ಹೊಂದಿಕೆ(typval_T *argvars, typval_T *rettv)
{
    ಕಂಡುಹಿಡಿ_some_ಹೊಂದಿಕೆ(argvars, rettv, ಹೊಂದಿಕೆ_ಹೊಂದಿಕೆ);
}

/*
 * "matchend()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_matchend(typval_T *argvars, typval_T *rettv)
{
    ಕಂಡುಹಿಡಿ_some_ಹೊಂದಿಕೆ(argvars, rettv, ಹೊಂದಿಕೆ_ಕೊನೆ_2);
}

/*
 * "matchlist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    ಕಂಡುಹಿಡಿ_some_ಹೊಂದಿಕೆ(argvars, rettv, ಹೊಂದಿಕೆ_LIST);
}

/*
 * "matchstr()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    ಕಂಡುಹಿಡಿ_some_ಹೊಂದಿಕೆ(argvars, rettv, ಹೊಂದಿಕೆ_STR);
}

/*
 * "matchstrlist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ಖಾಲಿ;
    char_u	*save_cpo;
    list_T	*l = ಖಾಲಿ;
    listitem_T	*li = ಖಾಲಿ;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಆದರೆ (rettv_list_alloc(rettv) != OK)
	ಹಿಂತಿರುಗಿ;
    retlist = rettv->vval.v_list;

    ಆದರೆ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((l = argvars[0].vval.v_list) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    ಆದರೆ (pat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ಖಾಲಿಗಳು_option;

    regmatch.regprog = eval_regcomp(pat);
    ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
	ಹೋಗು theend;
    regmatch.rm_ic = p_ic;

    ಪೂರ್ಣಾಂಕ		submatches = FALSE;
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	ಆದರೆ (d != ಖಾಲಿ)
	{
	    dictitem_T *di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"submatches", -1);
	    ಆದರೆ (di != ಖಾಲಿ)
	    {
		ಆದರೆ (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "submatches");
		    ಹೋಗು cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    ಪೂರ್ಣಾಂಕ idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಆದರೆ (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != ಖಾಲಿ)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    ಆದರೆ (get_matches_ಒಳ_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		ಹೋಗು cleanup;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
f_matchstrpos(typval_T *argvars, typval_T *rettv)
{
    ಕಂಡುಹಿಡಿ_some_ಹೊಂದಿಕೆ(argvars, rettv, ಹೊಂದಿಕೆ_POS);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
max_min(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ domax)
{
    varnumber_T	n = 0;

    ಆದರೆ (ಒಳ_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = 0;

    ಆದರೆ (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = ಖಾಲಿ;

	l = argvars[0].vval.v_list;
	ಆದರೆ (l != ಖಾಲಿ && l->lv_len > 0)
	{
	    ಆದರೆ (l->lv_ಮೊದಲ == &range_list_ಅಂಶ)
	    {
		ಆದರೆ ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_ಆರಂಭ;
		ಇಲ್ಲದಿದ್ದರೆ
		    n = l->lv_u.nonmat.lv_ಆರಂಭ + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_ಸಂಖ್ಯೆ = n;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
		{
		    ಆದರೆ (tv == ಖಾಲಿ)
			tv = &li->li_tv;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಪೂರ್ಣಾಂಕ res;
			ಆದರೆ (typval_compare2(&li->li_tv, tv,
				domax ? ಅಭಿವ್ಯಕ್ತಿ_GREATER : ಅಭಿವ್ಯಕ್ತಿ_SMALLER, FALSE, &res) == FAIL)
			    ಹಿಂತಿರುಗಿ;
			ಆದರೆ (res == OK)
			    tv = &li->li_tv;
		    }
		}

		ಆದರೆ (tv != ಖಾಲಿ)
		    copy_tv(tv, rettv);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	ಆದರೆ (tv != ಖಾಲಿ)
	    copy_tv(tv, rettv);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	ಪೂರ್ಣಾಂಕ		todo;
	typval_T	*tv = ಖಾಲಿ;

	d = argvars[0].vval.v_dict;
	ಆದರೆ (d != ಖಾಲಿ)
	{
	    todo = (ಪೂರ್ಣಾಂಕ)d->dv_hashtab.ht_ಬಳಸಿದ;
	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d->dv_hashtab, hi, todo)
	    {
		ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
		{
		    --todo;
		    ಆದರೆ (tv == ಖಾಲಿ)
			tv = &HI2DI(hi)->di_tv;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಪೂರ್ಣಾಂಕ res;
			ಆದರೆ (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? ಅಭಿವ್ಯಕ್ತಿ_GREATER : ಅಭಿವ್ಯಕ್ತಿ_SMALLER, FALSE, &res) == FAIL)
			    ಹಿಂತಿರುಗಿ;
			ಆದರೆ (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	ಆದರೆ (tv != ಖಾಲಿ)
	    copy_tv(tv, rettv);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_ವಾದ_ದ_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
}

/*
 * "max()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_max(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, TRUE);
}

/*
 * "min()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_min(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, FALSE);
}

#ಆದರೆ defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    ಶೂನ್ಯ
mzscheme_call_vim(char_u *name, typval_T *ವಾದಗಳು, typval_T *rettv)
{
    typval_T argvars[3];

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = name;
    copy_tv(ವಾದಗಳು, &argvars[1]);
    argvars[2].v_type = VAR_UNKNOWN;
    f_call(argvars, rettv);
    clear_tv(&argvars[1]);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * "nextnonblank()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ಆದರೆ (ಒಳ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	ಆದರೆ (lnum < 0 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    lnum = 0;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (*skipwhite(ml_get(lnum)) != NUL)
	    ನಿಲ್ಲಿಸು;
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = lnum;
}


/*
 * "ngettext()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#ಆದರೆ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    ಅಕ್ಷರ *prev = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_STRING;

    ಆದರೆ (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != ಖಾಲಿ &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#ಆದರೆ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[3].vval.v_string, (ಅಕ್ಷರ *)p_enc);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[3].vval.v_string, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[0].vval.v_string, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[1].vval.v_string, (ಪೂರ್ಣಾಂಕ)argvars[2].vval.v_ಸಂಖ್ಯೆ));
#ಇಲ್ಲದಿದ್ದರೆ
	textdomain((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[0].vval.v_string, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[1].vval.v_string, argvars[2].vval.v_ಸಂಖ್ಯೆ));
	textdomain(VIMPACKAGE);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	ಆದರೆ (prev != ಖಾಲಿ)
	    bind_textdomain_codeset((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[3].vval.v_string, prev);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[0].vval.v_string, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)argvars[1].vval.v_string, argvars[2].vval.v_ಸಂಖ್ಯೆ));
}


/*
 * "nr2char()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (has_mbyte)
    {
	ಪೂರ್ಣಾಂಕ	utf8 = 0;

	ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[1], ಖಾಲಿ);
	ಆದರೆ (utf8)
	    buf[utf_char2bytes((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]), buf)] = NUL;
	ಇಲ್ಲದಿದ್ದರೆ
	    buf[(*mb_char2bytes)((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]), buf)] = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	buf[0] = (char_u)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
	buf[1] = NUL;
    }
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "or(expr, expr)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_or(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ)
					| tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ಖಾಲಿ);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERL
/*
 * "perleval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * "prevnonblank()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ಆದರೆ (ಒಳ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    lnum = tv_get_lnum(argvars);
    ಆದರೆ (lnum < 1 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	lnum = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	ಆಗ (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
	    --lnum;
    rettv->vval.v_ಸಂಖ್ಯೆ = lnum;
}

// This dummy va_list is here because:
// - passing a NULL pointer doesn't work when va_list isn't a pointer
// - locally in the function results in a "used before set" warning
// - using va_start() to initialize it gives "function with fixed args" error
ಸ್ಥಿರ va_list	ap;

/*
 * "printf()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_printf(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		saved_did_emsg = did_emsg;
    ಅಕ್ಷರ	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (ಅಕ್ಷರ *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(ಖಾಲಿ, 0, fmt, ap, argvars + 1);
    ಆದರೆ (!did_emsg)
    {
	s = alloc(len + 1);
	ಆದರೆ (s != ಖಾಲಿ)
	{
	    rettv->vval.v_string = s;
	    (ಶೂನ್ಯ)vim_vsnprintf_typval((ಅಕ್ಷರ *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_pum_getpos(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    pum_ಗಣ_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_pumvisible(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (pum_visible())
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (p_pyx == 0)
	p_pyx = 3;

    ಆದರೆ (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	locals = ಖಾಲಿ;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
/*
 * "pyeval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (p_pyx == 0)
	p_pyx = 2;

    ಆದರೆ (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	locals = ಖಾಲಿ;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

# ಆದರೆ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ಆದರೆ (p_pyx == 2)
	f_pyeval(argvars, rettv);
    ಇಲ್ಲದಿದ್ದರೆ
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ UINT32_T srand_ಬೀಜ_for_testing = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	srand_ಬೀಜ_for_testing_is_ಬಳಸಿದ = FALSE;

    ಸ್ಥಿರ ಶೂನ್ಯ
f_test_srand_ಬೀಜ(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
	srand_ಬೀಜ_for_testing_is_ಬಳಸಿದ = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	srand_ಬೀಜ_for_testing = (UINT32_T)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
	srand_ಬೀಜ_for_testing_is_ಬಳಸಿದ = TRUE;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
init_srand(UINT32_T *x)
{
    ರಚನೆ {
	ಒಕ್ಕೂಟ {
	    UINT32_T ಸಂಖ್ಯೆ;
	    char_u   ಬೈಟುಗಳು[ಗಾತ್ರ(UINT32_T)];
	} contents;
    } buf;

    ಆದರೆ (srand_ಬೀಜ_for_testing_is_ಬಳಸಿದ)
    {
	*x = srand_ಬೀಜ_for_testing;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (mch_get_random(buf.contents.ಬೈಟುಗಳು, ಗಾತ್ರ(buf.contents.ಬೈಟುಗಳು)) == OK)
    {
	*x = buf.contents.ಸಂಖ್ಯೆ;
	ಹಿಂತಿರುಗಿ;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#ಆದರೆ defined(FEAT_SODIUM)
    ಆದರೆ (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
#ಆದರೆ defined(FEAT_RELTIME)
	proftime_T res;
	profile_ಆರಂಭ(&res);
# ಆದರೆ defined(MSWIN)
	*x = (UINT32_T)res.ಕೆಳಗಿನ_Part;
# ಇಲ್ಲದಿದ್ದರೆ
	*x = (UINT32_T)res.tv_fsec;
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	*x = vim_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ
	*x ^= mch_get_pid();
    }
}

#ವ್ಯಾಖ್ಯಾನಿಸು ROTL(x, k) (((x) << (k)) | ((x) >> (32 - (k))))
#ವ್ಯಾಖ್ಯಾನಿಸು SPLITMIX32(x, z) ( \
    (z) = ((x) += 0x9e3779b9), \
    (z) = ((z) ^ ((z) >> 16)) * 0x85ebca6b, \
    (z) = ((z) ^ ((z) >> 13)) * 0xc2b2ae35, \
    (z) ^ ((z) >> 16) \
    )
#ವ್ಯಾಖ್ಯಾನಿಸು SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w) \
    ಫಲಿತಾಂಶ = ROTL((y) * 5, 7) * 9; \
    t = (y) << 9; \
    (z) ^= (x); \
    (w) ^= (y); \
    (y) ^= (z), (x) ^= (w); \
    (z) ^= t; \
    (w) = ROTL(w, 11);

/*
 * "rand()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = ಖಾಲಿ;
    ಸ್ಥಿರ UINT32_T	gx, gy, gz, gw;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, ಫಲಿತಾಂಶ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// When no argument is given use the global seed list.
	ಆದರೆ (initialized == FALSE)
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
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	ಆದರೆ (l == ಖಾಲಿ || list_len(l) != 4)
	    ಹೋಗು theend;

	lx = list_ಕಂಡುಹಿಡಿ(l, 0L);
	ly = list_ಕಂಡುಹಿಡಿ(l, 1L);
	lz = list_ಕಂಡುಹಿಡಿ(l, 2L);
	lw = list_ಕಂಡುಹಿಡಿ(l, 3L);
	ಆದರೆ (lx == ಖಾಲಿ || ly == ಖಾಲಿ || lz == ಖಾಲಿ || lw == ಖಾಲಿ)
	    ಹೋಗು theend;
	ಆದರೆ (lx->li_tv.v_type != VAR_ಸಂಖ್ಯೆ
		|| ly->li_tv.v_type != VAR_ಸಂಖ್ಯೆ
		|| lz->li_tv.v_type != VAR_ಸಂಖ್ಯೆ
		|| lw->li_tv.v_type != VAR_ಸಂಖ್ಯೆ)
	    ಹೋಗು theend;
	x = (UINT32_T)lx->li_tv.vval.v_ಸಂಖ್ಯೆ;
	y = (UINT32_T)ly->li_tv.vval.v_ಸಂಖ್ಯೆ;
	z = (UINT32_T)lz->li_tv.vval.v_ಸಂಖ್ಯೆ;
	w = (UINT32_T)lw->li_tv.vval.v_ಸಂಖ್ಯೆ;

	SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w);

	lx->li_tv.vval.v_ಸಂಖ್ಯೆ = (varnumber_T)x;
	ly->li_tv.vval.v_ಸಂಖ್ಯೆ = (varnumber_T)y;
	lz->li_tv.vval.v_ಸಂಖ್ಯೆ = (varnumber_T)z;
	lw->li_tv.vval.v_ಸಂಖ್ಯೆ = (varnumber_T)w;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹೋಗು theend;

    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)ಫಲಿತಾಂಶ;
    ಹಿಂತಿರುಗಿ;

theend:
    semsg(_(e_invalid_ವಾದ_str), tv_get_string(&argvars[0]));
    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
}

/*
 * "srand()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_srand(typval_T *argvars, typval_T *rettv)
{
    UINT32_T x = 0, z;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	    error = FALSE;

	x = (UINT32_T)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &error);
	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ;
    }

    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ROTL
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು SPLITMIX32
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	ಆರಂಭ;
    varnumber_T	ಕೊನೆ;
    varnumber_T	stride = 1;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆರಂಭ = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &error);
    ಆದರೆ (argvars[1].v_type == VAR_UNKNOWN)
    {
	ಕೊನೆ = ಆರಂಭ - 1;
	ಆರಂಭ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಕೊನೆ = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
    }

    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ;		// type error; errmsg already given
    ಆದರೆ (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	ಹಿಂತಿರುಗಿ;
    }

    // The stride is stored in "lv_stride", which is an int.
    ಆದರೆ (stride < INT_MIN || stride > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
    {
	ಅಕ್ಷರ	buf[NUMBUFLEN];

	vim_snprintf(buf, ಗಾತ್ರ(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	ಹಿಂತಿರುಗಿ;
    }

    uvarnumber_T	len;

    ಆದರೆ (stride > 0 ? ಕೊನೆ < ಆರಂಭ : ಕೊನೆ > ಆರಂಭ)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	ಹಿಂಭಾಗ = stride > 0
				  ? (uvarnumber_T)ಆರಂಭ - (uvarnumber_T)ಕೊನೆ
				  : (uvarnumber_T)ಕೊನೆ - (uvarnumber_T)ಆರಂಭ;

	ಆದರೆ (ಹಿಂಭಾಗ > 1)
	{
	    emsg(_(e_ಆರಂಭ_past_ಕೊನೆ));
	    ಹಿಂತಿರುಗಿ;
	}
	len = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	ಹರವು = stride > 0
				  ? (uvarnumber_T)ಕೊನೆ - (uvarnumber_T)ಆರಂಭ
				  : (uvarnumber_T)ಆರಂಭ - (uvarnumber_T)ಕೊನೆ;
	uvarnumber_T	ಎಣಿಕೆ_2 = ಹರವು / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	ಆದರೆ (ಎಣಿಕೆ_2 >= (uvarnumber_T)ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
	{
	    ಅಕ್ಷರ	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, ಗಾತ್ರ(buf), "%llu",
			      ಎಣಿಕೆ_2 < UVARNUM_MAX ? ಎಣಿಕೆ_2 + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    ಹಿಂತಿರುಗಿ;
	}
	len = ಎಣಿಕೆ_2 + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_ಮೊದಲ = &range_list_ಅಂಶ;
    list->lv_u.nonmat.lv_ಆರಂಭ = ಆರಂಭ;
    list->lv_u.nonmat.lv_ಕೊನೆ = ಕೊನೆ;
    list->lv_u.nonmat.lv_stride = (ಪೂರ್ಣಾಂಕ)stride;
    list->lv_len = (ಪೂರ್ಣಾಂಕ)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    ಶೂನ್ಯ
range_list_materialize(list_T *list)
{
    varnumber_T ಆರಂಭ = list->lv_u.nonmat.lv_ಆರಂಭ;
    varnumber_T ಕೊನೆ = list->lv_u.nonmat.lv_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_ಮೊದಲ = ಖಾಲಿ;
    list->lv_u.mat.lv_ಕೊನೆಯ = ಖಾಲಿ;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_ಅಂಶ = ಖಾಲಿ;
    ಗಾಗಿ (i = ಆರಂಭ; stride > 0 ? i <= ಕೊನೆ : i >= ಕೊನೆ; i += stride)
    {
	ಆದರೆ (list_append_ಸಂಖ್ಯೆ(list, i) == FAIL)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (list->lv_ಬೀಗ & VAR_ವಸ್ತುಗಳು_LOCKED)
	    list->lv_u.mat.lv_ಕೊನೆಯ->li_tv.v_ಬೀಗ = VAR_LOCKED;
    }
    list->lv_ಬೀಗ &= ~VAR_ವಸ್ತುಗಳು_LOCKED;
}

/*
 * "getreginfo()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		regname;
    char_u	buf[NUMBUFLEN + 2];
    size_t	buflen;
    ದೀರ್ಘ	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    regname = getreg_get_regname(argvars);
    ಆದರೆ (regname == 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (regname == '@')
	regname = '"';

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC | GREG_LIST);
    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (dict_ಕೂಡಿಸು_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    ಆಯ್ಕೆ (get_reg_type(regname, &reglen))
    {
	ಸ್ಥಿತಿ MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MBLOCK:
	    buflen = vim_snprintf_safelen((ಅಕ್ಷರ *)buf, ಗಾತ್ರ(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    buf[0] = NUL;
	    buflen = 0;
	    ನಿಲ್ಲಿಸು;
    }
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string_len(dict, (ಅಕ್ಷರ *)"regtype", buf, (ಪೂರ್ಣಾಂಕ)buflen);

    buf[0] = get_ರಿಜಿಸ್ಟರ್_name(get_unname_ರಿಜಿಸ್ಟರ್());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    ಆದರೆ (regname == '"')
	(ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string_len(dict, (ಅಕ್ಷರ *)"points_to", buf, (ಪೂರ್ಣಾಂಕ)buflen);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	dictitem_T	*ಅಂಶ = dictitem_alloc((char_u *)"isunnamed");

	ಆದರೆ (ಅಂಶ != ಖಾಲಿ)
	{
	    ಅಂಶ->di_tv.v_type = VAR_BOOL;
	    ಅಂಶ->di_tv.vval.v_ಸಂಖ್ಯೆ = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು(dict, ಅಂಶ);
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
return_ರಿಜಿಸ್ಟರ್(ಪೂರ್ಣಾಂಕ regname, typval_T *rettv)
{
    char_u buf[2] = {NUL, NUL};

    buf[0] = (char_u)regname;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(buf, (buf[0] == NUL) ? 0 : 1);
}

/*
 * "reg_executing()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_reg_executing(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    return_ರಿಜಿಸ್ಟರ್(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_reg_recording(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    return_ರಿಜಿಸ್ಟರ್(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ slen = list_len(l);

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL
	    || l == ಖಾಲಿ
	    || n <= 0
	    || slen == 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_repeat_ಎಣಿಕೆ(slen, n) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆಗ (n-- > 0)
	ಆದರೆ (list_extend(rettv->vval.v_list, l, ಖಾಲಿ) == FAIL)
	    ನಿಲ್ಲಿಸು;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		slen;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    ಆದರೆ (rettv_blob_alloc(rettv) == FAIL
	    || blob == ಖಾಲಿ
	    || n <= 0)
	ಹಿಂತಿರುಗಿ;

    slen = blob->bv_ga.ga_len;
    ಆದರೆ (slen <= 0)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (check_repeat_ಎಣಿಕೆ(slen, n) == FAIL)
	ಹಿಂತಿರುಗಿ;
    len = slen * (ಪೂರ್ಣಾಂಕ)n;

    ಆದರೆ (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    ಗಾಗಿ (i = 0; i < slen; ++i)
	ಆದರೆ (blob_get(blob, i) != 0)
	    ನಿಲ್ಲಿಸು;

    ಆದರೆ (i == slen)
	// No need to copy since all bytes are already zero
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)n; ++i)
	blob_ಗಣ_range(rettv->vval.v_blob,
		(ದೀರ್ಘ)i * slen, ((ದೀರ್ಘ)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    size_t	slen;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*r;
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    slen = STRLEN(p);
    ಆದರೆ (slen == 0 || n <= 0)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (check_repeat_ಎಣಿಕೆ((varnumber_T)slen, n) == FAIL)
	ಹಿಂತಿರುಗಿ;
    len = (ಪೂರ್ಣಾಂಕ)slen * (ಪೂರ್ಣಾಂಕ)n;

    r = alloc((size_t)len + 1);
    ಆದರೆ (r == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    mch_memmove(r, p, slen);
    ಮುಗಿದ = (ಪೂರ್ಣಾಂಕ)slen;
    ಆಗ (ಮುಗಿದ < len)
    {
	ಪೂರ್ಣಾಂಕ copy_len = ಮುಗಿದ;

	ಆದರೆ (copy_len > len - ಮುಗಿದ)
	    copy_len = len - ಮುಗಿದ;
	mch_memmove(r + ಮುಗಿದ, r, (size_t)copy_len);
	ಮುಗಿದ += copy_len;
    }
    r[len] = NUL;

    rettv->vval.v_string = r;
}

/*
 * "repeat()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    n = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    ಆದರೆ (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    ಇಲ್ಲದಿದ್ದರೆ
	repeat_string(&argvars[0], n, rettv);
}

#ವ್ಯಾಖ್ಯಾನಿಸು SP_NOMOVE	0x01	    // don't move cursor
#ವ್ಯಾಖ್ಯಾನಿಸು SP_REPEAT	0x02	    // repeat to find outer pair
#ವ್ಯಾಖ್ಯಾನಿಸು SP_RETCOUNT	0x04	    // return matchcount
#ವ್ಯಾಖ್ಯಾನಿಸು SP_SETPCMARK	0x08	    // set previous context mark
#ವ್ಯಾಖ್ಯಾನಿಸು SP_ಆರಂಭ	0x10	    // accept match at start position
#ವ್ಯಾಖ್ಯಾನಿಸು SP_SUBPAT	0x20	    // return nr of matching sub-pattern
#ವ್ಯಾಖ್ಯಾನಿಸು SP_ಕೊನೆ		0x40	    // leave cursor at end of match
#ವ್ಯಾಖ್ಯಾನಿಸು SP_ಸ್ತಂಭ	0x80	    // start at cursor column

/*
 * Get flags for a search function.
 * Possibly sets "p_ws".
 * Returns BACKWARD, FORWARD or zero (for an error).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_ಹುಡುಕು_arg(typval_T *varp, ಪೂರ್ಣಾಂಕ *flagsp)
{
    ಪೂರ್ಣಾಂಕ		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		ಮುಖವಾಡ;

    ಆದರೆ (varp->v_type == VAR_UNKNOWN)
	ಹಿಂತಿರುಗಿ FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    ಆದರೆ (flags == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;		// type error; errmsg already given
    ಆಗ (*flags != NUL)
    {
	ಆಯ್ಕೆ (*flags)
	{
	    ಸ್ಥಿತಿ 'b': dir = BACKWARD; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'w': p_ws = TRUE; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'W': p_ws = FALSE; ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:  ಮುಖವಾಡ = 0;
		      ಆದರೆ (flagsp != ಖಾಲಿ)
			  ಆಯ್ಕೆ (*flags)
			  {
			      ಸ್ಥಿತಿ 'c': ಮುಖವಾಡ = SP_ಆರಂಭ; ನಿಲ್ಲಿಸು;
			      ಸ್ಥಿತಿ 'e': ಮುಖವಾಡ = SP_ಕೊನೆ; ನಿಲ್ಲಿಸು;
			      ಸ್ಥಿತಿ 'm': ಮುಖವಾಡ = SP_RETCOUNT; ನಿಲ್ಲಿಸು;
			      ಸ್ಥಿತಿ 'n': ಮುಖವಾಡ = SP_NOMOVE; ನಿಲ್ಲಿಸು;
			      ಸ್ಥಿತಿ 'p': ಮುಖವಾಡ = SP_SUBPAT; ನಿಲ್ಲಿಸು;
			      ಸ್ಥಿತಿ 'r': ಮುಖವಾಡ = SP_REPEAT; ನಿಲ್ಲಿಸು;
			      ಸ್ಥಿತಿ 's': ಮುಖವಾಡ = SP_SETPCMARK; ನಿಲ್ಲಿಸು;
			      ಸ್ಥಿತಿ 'z': ಮುಖವಾಡ = SP_ಸ್ತಂಭ; ನಿಲ್ಲಿಸು;
			  }
		      ಆದರೆ (ಮುಖವಾಡ == 0)
		      {
			  semsg(_(e_invalid_ವಾದ_str), flags);
			  dir = 0;
		      }
		      ಇಲ್ಲದಿದ್ದರೆ
			  *flagsp |= ಮುಖವಾಡ;
	}
	ಆದರೆ (dir == 0)
	    ನಿಲ್ಲಿಸು;
	++flags;
    }
    ಹಿಂತಿರುಗಿ dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹುಡುಕು_cmn(typval_T *argvars, pos_T *ಹೊಂದಿಕೆ_pos, ಪೂರ್ಣಾಂಕ *flagsp)
{
    ಪೂರ್ಣಾಂಕ		flags;
    char_u	*pat;
    size_t	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    ಪೂರ್ಣಾಂಕ		save_p_ws = p_ws;
    ಪೂರ್ಣಾಂಕ		dir;
    ಪೂರ್ಣಾಂಕ		retval = 0;	// default: FAIL
    ದೀರ್ಘ	lnum_stop = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    ದೀರ್ಘ	ಸಮಯ_ಎಲ್ಲೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		options = ಹುಡುಕು_KEEP;
    ಪೂರ್ಣಾಂಕ		subpatnum;
    searchit_arg_T sia;
    ಪೂರ್ಣಾಂಕ		use_skip = FALSE;
    pos_T	firstpos;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL)))))
	ಹೋಗು theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_ಹುಡುಕು_arg(&argvars[1], flagsp);	// may set p_ws
    ಆದರೆ (dir == 0)
	ಹೋಗು theend;
    flags = *flagsp;
    ಆದರೆ (flags & SP_ಆರಂಭ)
	options |= ಹುಡುಕು_ಆರಂಭ;
    ಆದರೆ (flags & SP_ಕೊನೆ)
	options |= ಹುಡುಕು_ಕೊನೆ;
    ಆದರೆ (flags & SP_ಸ್ತಂಭ)
	options |= ಹುಡುಕು_ಲಂಬಸಾಲು;

    // Optional arguments: line number to stop searching, timeout and skip.
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], ಖಾಲಿ);
	ಆದರೆ (lnum_stop < 0)
	    ಹೋಗು theend;
	ಆದರೆ (argvars[3].v_type != VAR_UNKNOWN)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
	    ಸಮಯ_ಎಲ್ಲೆ = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[3], ಖಾಲಿ);
	    ಆದರೆ (ಸಮಯ_ಎಲ್ಲೆ < 0)
		ಹೋಗು theend;
#ಆದರೆ_ಅಂತ್ಯ
	    use_skip = eval_ಅಭಿವ್ಯಕ್ತಿ_valid_arg(&argvars[4]);
	}
    }

    /*
     * This function does not accept SP_REPEAT and SP_RETCOUNT flags.
     * Check to make sure only those flags are set.
     * Also, Only the SP_NOMOVE or the SP_SETPCMARK flag can be set. Both
     * flags cannot be set. Check for that condition also.
     */
    ಆದರೆ (((flags & (SP_REPEAT | SP_RETCOUNT)) != 0)
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_ವಾದ_str), tv_get_string(&argvars[1]));
	ಹೋಗು theend;
    }

    pos = save_cursor = curwin->w_cursor;
    CLEAR_ಕ್ಷೇತ್ರ(firstpos);
    CLEAR_ಕ್ಷೇತ್ರ(sia);
    sia.sa_stop_lnum = (linenr_T)lnum_stop;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    sia.sa_tm = ಸಮಯ_ಎಲ್ಲೆ;
#ಆದರೆ_ಅಂತ್ಯ

    patlen = STRLEN(pat);

    // Repeat until {skip} returns FALSE.
    ಗಾಗಿ (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, ಖಾಲಿ, dir, pat, patlen, 1L,
						     options, RE_ಹುಡುಕು, &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	ಆದರೆ (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	ಆದರೆ (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    ಪೂರ್ಣಾಂಕ	    do_skip;
	    ಪೂರ್ಣಾಂಕ	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_ಅಭಿವ್ಯಕ್ತಿ_ಗಮ್ಯ_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    ಆದರೆ (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (!do_skip)
		ನಿಲ್ಲಿಸು;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~ಹುಡುಕು_ಆರಂಭ;
    }

    ಆದರೆ (subpatnum != FAIL)
    {
	ಆದರೆ (flags & SP_SUBPAT)
	    retval = subpatnum;
	ಇಲ್ಲದಿದ್ದರೆ
	    retval = pos.lnum;
	ಆದರೆ (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	ಆದರೆ (ಹೊಂದಿಕೆ_pos != ಖಾಲಿ)
	{
	    // Store the match cursor position
	    ಹೊಂದಿಕೆ_pos->lnum = pos.lnum;
	    ಹೊಂದಿಕೆ_pos->ಲಂಬಸಾಲು = pos.ಲಂಬಸಾಲು + 1;
	}
	// "/$" will put the cursor after the end of the line, may need to
	// correct that here
	check_cursor();
    }

    // If 'n' flag is used: restore cursor position.
    ಆದರೆ (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    ಇಲ್ಲದಿದ್ದರೆ
	curwin->w_ಗಣ_curswant = true;
theend:
    p_ws = save_p_ws;

    ಹಿಂತಿರುಗಿ retval;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RUBY
/*
 * "rubyeval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * "screenattr()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಅಡ್ಡಸಾಲು = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ) - 1;
    ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ಖಾಲಿ) - 1;
    ಆದರೆ (ಅಡ್ಡಸಾಲು < 0 || ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು
	    || ಲಂಬಸಾಲು < 0 || ಲಂಬಸಾಲು >= screen_Columns)
	c = -1;
    ಇಲ್ಲದಿದ್ದರೆ
	c = ScreenAttrs[ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + ಲಂಬಸಾಲು];
    rettv->vval.v_ಸಂಖ್ಯೆ = c;
}

/*
 * "screenchar()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಅಡ್ಡಸಾಲು = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ) - 1;
    ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ಖಾಲಿ) - 1;
    ಆದರೆ (ಅಡ್ಡಸಾಲು < 0 || ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು || ಲಂಬಸಾಲು < 0 || ಲಂಬಸಾಲು >= screen_Columns)
	c = -1;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, buf, ಖಾಲಿ);
	c = (*mb_ptr2char)(buf);
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = c;
}

/*
 * "screenchars()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_screenchars(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಅಡ್ಡಸಾಲು = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ) - 1;
    ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ಖಾಲಿ) - 1;
    ಆದರೆ (ಅಡ್ಡಸಾಲು < 0 || ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು || ಲಂಬಸಾಲು < 0 || ಲಂಬಸಾಲು >= screen_Columns)
	ಹಿಂತಿರುಗಿ;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, buf, ಖಾಲಿ);
    ಪೂರ್ಣಾಂಕ pcc[MAX_MCO];
    ಆದರೆ (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    ಇಲ್ಲದಿದ್ದರೆ
	c = (*mb_ptr2char)(buf);
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)c);

    ಆದರೆ (enc_utf8)
	ಗಾಗಿ (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_screencol(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_screenrow(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = ಖಾಲಿ;
    rettv->v_type = VAR_STRING;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಅಡ್ಡಸಾಲು = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ) - 1;
    ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ಖಾಲಿ) - 1;
    ಆದರೆ (ಅಡ್ಡಸಾಲು < 0 || ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು || ಲಂಬಸಾಲು < 0 || ಲಂಬಸಾಲು >= screen_Columns)
	ಹಿಂತಿರುಗಿ;

    screen_getbytes(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, buf, ಖಾಲಿ);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ಹುಡುಕು(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		flags = 0;

    rettv->vval.v_ಸಂಖ್ಯೆ = ಹುಡುಕು_cmn(argvars, ಖಾಲಿ, &flags);
}

/*
 * "searchdecl()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		locally = TRUE;
    ಪೂರ್ಣಾಂಕ		thisblock = FALSE;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    char_u	*name;

    rettv->vval.v_ಸಂಖ್ಯೆ = 1;	// default: FAIL

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[1], &error);
	ಆದರೆ (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[2], &error);
    }
    ಆದರೆ (!error && name != ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = ಕಂಡುಹಿಡಿ_decl(name, (ಪೂರ್ಣಾಂಕ)STRLEN(name),
				     locally, thisblock, ಹುಡುಕು_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
searchpair_cmn(typval_T *argvars, pos_T *ಹೊಂದಿಕೆ_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    ಪೂರ್ಣಾಂಕ		save_p_ws = p_ws;
    ಪೂರ್ಣಾಂಕ		dir;
    ಪೂರ್ಣಾಂಕ		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		retval = 0;		// default: FAIL
    ದೀರ್ಘ	lnum_stop = 0;
    ದೀರ್ಘ	ಸಮಯ_ಎಲ್ಲೆ = 0;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 6) == FAIL)))))
	ಹೋಗು theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    ಆದರೆ (spat == ಖಾಲಿ || mpat == ಖಾಲಿ || epat == ಖಾಲಿ)
	ಹೋಗು theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_ಹುಡುಕು_arg(&argvars[3], &flags); // may set p_ws
    ಆದರೆ (dir == 0)
	ಹೋಗು theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    ಆದರೆ ((flags & (SP_ಕೊನೆ | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_ವಾದ_str), tv_get_string(&argvars[3]));
	ಹೋಗು theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    ಆದರೆ (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    ಆದರೆ (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Type is checked later.
	skip = &argvars[4];

	ಆದರೆ (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[5], ಖಾಲಿ);
	    ಆದರೆ (lnum_stop < 0)
	    {
		semsg(_(e_invalid_ವಾದ_str), tv_get_string(&argvars[5]));
		ಹೋಗು theend;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
	    ಆದರೆ (argvars[6].v_type != VAR_UNKNOWN)
	    {
		ಸಮಯ_ಎಲ್ಲೆ = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[6], ಖಾಲಿ);
		ಆದರೆ (ಸಮಯ_ಎಲ್ಲೆ < 0)
		{
		    semsg(_(e_invalid_ವಾದ_str), tv_get_string(&argvars[6]));
		    ಹೋಗು theend;
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    ಹೊಂದಿಕೆ_pos, lnum_stop, ಸಮಯ_ಎಲ್ಲೆ);

theend:
    p_ws = save_p_ws;

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * "searchpair()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = searchpair_cmn(argvars, ಖಾಲಿ);
}

/*
 * "searchpairpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	ಹೊಂದಿಕೆ_pos;
    ಪೂರ್ಣಾಂಕ		lnum = 0;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = 0;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (searchpair_cmn(argvars, &ಹೊಂದಿಕೆ_pos) > 0)
    {
	lnum = ಹೊಂದಿಕೆ_pos.lnum;
	ಲಂಬಸಾಲು = ಹೊಂದಿಕೆ_pos.ಲಂಬಸಾಲು;
    }

    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)ಲಂಬಸಾಲು);
}

/*
 * Search for a start/middle/end thing.
 * Used by searchpair(), see its documentation for the details.
 * Returns 0 or -1 for no match,
 */
    ದೀರ್ಘ
do_searchpair(
    char_u	*spat,	    // start pattern
    char_u	*mpat,	    // middle pattern
    char_u	*epat,	    // end pattern
    ಪೂರ್ಣಾಂಕ		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    ಪೂರ್ಣಾಂಕ		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*ಹೊಂದಿಕೆ_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    ದೀರ್ಘ	ಸಮಯ_ಎಲ್ಲೆ ಬಳಸದ) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = ಖಾಲಿ, *pat3 = ಖಾಲಿ;
    size_t	patlen;
    size_t	spatlen;
    size_t	epatlen;
    size_t	pat2size;
    size_t	pat2len;
    size_t	pat3size;
    size_t	pat3len;
    ದೀರ್ಘ	retval = 0;
    pos_T	pos;
    pos_T	firstpos;
    pos_T	foundpos;
    pos_T	save_cursor;
    pos_T	save_pos;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		r;
    ಪೂರ್ಣಾಂಕ		nest = 1;
    ಪೂರ್ಣಾಂಕ		use_skip = FALSE;
    ಪೂರ್ಣಾಂಕ		err;
    ಪೂರ್ಣಾಂಕ		options = ಹುಡುಕು_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ಖಾಲಿಗಳು_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    ಆದರೆ (pat2 == ಖಾಲಿ)
	ಹೋಗು theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    ಆದರೆ (pat3 == ಖಾಲಿ)
	ಹೋಗು theend;
    pat2len = vim_snprintf((ಅಕ್ಷರ *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    ಆದರೆ (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	pat3len = vim_snprintf((ಅಕ್ಷರ *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    ಆದರೆ (flags & SP_ಆರಂಭ)
	options |= ಹುಡುಕು_ಆರಂಭ;

    ಆದರೆ (skip != ಖಾಲಿ)
	use_skip = eval_ಅಭಿವ್ಯಕ್ತಿ_valid_arg(skip);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    ಆದರೆ (ಸಮಯ_ಎಲ್ಲೆ > 0)
	init_regexp_timeout(ಸಮಯ_ಎಲ್ಲೆ);
#ಆದರೆ_ಅಂತ್ಯ
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    ಗಾಗಿ (;;)
    {
	searchit_arg_T sia;

	CLEAR_ಕ್ಷೇತ್ರ(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, ಖಾಲಿ, dir, pat, patlen, 1L,
						     options, RE_ಹುಡುಕು, &sia);
	ಆದರೆ (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (firstpos.lnum == 0)
	    firstpos = pos;
	ಆದರೆ (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    ಆದರೆ (dir == BACKWARD)
		decl(&pos);
	    ಇಲ್ಲದಿದ್ದರೆ
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~ಹುಡುಕು_ಆರಂಭ;

	// If the skip pattern matches, ignore this match.
	ಆದರೆ (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_ಅಭಿವ್ಯಕ್ತಿ_ಗಮ್ಯ_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    ಆದರೆ (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (r)
		ಮುಂದುವರಿ;
	}

	ಆದರೆ ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Found end when searching forward or start when searching
	    // backward: end of (nested) pair; or found middle in outer pair.
	    ಆದರೆ (--nest == 1)
		pat = pat3;	// outer level, search for middle
	}

	ಆದರೆ (nest == 0)
	{
	    // Found the match: return matchcount or line number.
	    ಆದರೆ (flags & SP_RETCOUNT)
		++retval;
	    ಇಲ್ಲದಿದ್ದರೆ
		retval = pos.lnum;
	    ಆದರೆ (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    ಆದರೆ (!(flags & SP_REPEAT))
		ನಿಲ್ಲಿಸು;
	    nest = 1;	    // search for next unmatched
	}
    }

    ಆದರೆ (ಹೊಂದಿಕೆ_pos != ಖಾಲಿ)
    {
	// Store the match cursor position
	ಹೊಂದಿಕೆ_pos->lnum = curwin->w_cursor.lnum;
	ಹೊಂದಿಕೆ_pos->ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    ಆದರೆ ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    ಆದರೆ (ಸಮಯ_ಎಲ್ಲೆ > 0)
	disable_regexp_timeout();
#ಆದರೆ_ಅಂತ್ಯ
    vim_free(pat2);
    vim_free(pat3);
    ಆದರೆ (p_cpo == ಖಾಲಿಗಳು_option)
	p_cpo = save_cpo;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	ಆದರೆ (*p_cpo == NUL)
	    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * "searchpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	ಹೊಂದಿಕೆ_pos;
    ಪೂರ್ಣಾಂಕ		lnum = 0;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		flags = 0;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    n = ಹುಡುಕು_cmn(argvars, &ಹೊಂದಿಕೆ_pos, &flags);
    ಆದರೆ (n > 0)
    {
	lnum = ಹೊಂದಿಕೆ_pos.lnum;
	ಲಂಬಸಾಲು = ಹೊಂದಿಕೆ_pos.ಲಂಬಸಾಲು;
    }

    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)ಲಂಬಸಾಲು);
    ಆದರೆ (flags & SP_SUBPAT)
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_position(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ charpos)
{
    pos_T	pos;
    ಪೂರ್ಣಾಂಕ		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string_chk(argvars);
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (pos.ಲಂಬಸಾಲು != MAXCOL && --pos.ಲಂಬಸಾಲು < 0)
	pos.ಲಂಬಸಾಲು = 0;
    ಆದರೆ ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	ಆದರೆ (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_ಗಣ_curswant = false;
	}
	check_cursor();
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	ಆದರೆ (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	emsg(_(e_invalid_ವಾದ));
}
/*
 * "setcharpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    ಗಣ_position(argvars, rettv, TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
f_setcharsearch(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    ಆದರೆ (check_for_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((d = argvars[0].vval.v_dict) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    csearch = dict_get_string(d, "char", FALSE);
    ಆದರೆ (csearch != ಖಾಲಿ)
    {
	ಆದರೆ (enc_utf8)
	{
	    ಪೂರ್ಣಾಂಕ pcc[MAX_MCO];
	    ಪೂರ್ಣಾಂಕ c = utfc_ptr2char(csearch, pcc);

	    ಗಣ_ಕೊನೆಯ_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗಣ_ಕೊನೆಯ_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"forward", -1);
    ಆದರೆ (di != ಖಾಲಿ)
	ಗಣ_csearch_direction((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"until", -1);
    ಆದರೆ (di != ಖಾಲಿ)
	ಗಣ_csearch_until(!!tv_get_ಸಂಖ್ಯೆ(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    ಗಣ_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_setenv(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string_buf(&argvars[0], namebuf);
    ಆದರೆ (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_ಸಂಖ್ಯೆ == VVAL_NULL)
	vim_unsetenv_ext(name);
    ಇಲ್ಲದಿದ್ದರೆ
	vim_setenv_ext(name, tv_get_string_buf(&argvars[1], valbuf));
}

/*
 * "setfperm({fname}, {mode})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_setfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	modebuf[NUMBUFLEN];
    char_u	*ಬಹುಲಕ_str;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಮುಖವಾಡ;
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ = 0;

    rettv->vval.v_ಸಂಖ್ಯೆ = 0;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    fname = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಬಹುಲಕ_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    ಆದರೆ (ಬಹುಲಕ_str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (STRLEN(ಬಹುಲಕ_str) != 9)
    {
	semsg(_(e_invalid_ವಾದ_str), ಬಹುಲಕ_str);
	ಹಿಂತಿರುಗಿ;
    }

    ಮುಖವಾಡ = 1;
    ಗಾಗಿ (i = 8; i >= 0; --i)
    {
	ಆದರೆ (ಬಹುಲಕ_str[i] != '-')
	    ಬಹುಲಕ |= ಮುಖವಾಡ;
	ಮುಖವಾಡ = ಮುಖವಾಡ << 1;
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = mch_setperm(fname, ಬಹುಲಕ) == OK;
}

/*
 * "setpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_setpos(typval_T *argvars, typval_T *rettv)
{
    ಗಣ_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    ಪೂರ್ಣಾಂಕ
get_yank_type(char_u **pp, char_u *yank_type, ದೀರ್ಘ *ಖಂಡ_len)
{
    char_u *stropt = *pp;
    ಆಯ್ಕೆ (*stropt)
    {
	ಸ್ಥಿತಿ 'v': ಸ್ಥಿತಿ 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'V': ಸ್ಥಿತಿ 'l':	// line-wise selection
	    *yank_type = MLINE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'b': ಸ್ಥಿತಿ Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    ಆದರೆ (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*ಖಂಡ_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    *pp = stropt;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * "setreg()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_setreg(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    ಪೂರ್ಣಾಂಕ		append;
    char_u	yank_type;
    ದೀರ್ಘ	ಖಂಡ_len;
    typval_T	*regcontents;
    ಪೂರ್ಣಾಂಕ		pointreg;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    pointreg = 0;
    regcontents = ಖಾಲಿ;
    ಖಂಡ_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_ಸಂಖ್ಯೆ = 1;		// FAIL is default

    ಆದರೆ (strregname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;		// type error; errmsg already given
    ಆದರೆ (ಒಳ_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_ರಿಜಿಸ್ಟರ್_name_must_be_one_char_str), strregname);
	ಹಿಂತಿರುಗಿ;
    }
    regname = *strregname;
    ಆದರೆ (regname == 0 || regname == '@')
	regname = '"';

    ಆದರೆ (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	ಆದರೆ (d == ಖಾಲಿ || d->dv_hashtab.ht_ಬಳಸಿದ == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {ಖಾಲಿ, ಖಾಲಿ};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    ಹಿಂತಿರುಗಿ;
	}

	di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"regcontents", -1);
	ಆದರೆ (di != ಖಾಲಿ)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	ಆದರೆ (stropt != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ ret = get_yank_type(&stropt, &yank_type, &ಖಂಡ_len);

	    ಆದರೆ (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "value");
		ಹಿಂತಿರುಗಿ;
	    }
	}

	ಆದರೆ (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    ಆದರೆ (stropt != ಖಾಲಿ)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	regcontents = &argvars[1];

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಆದರೆ (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    ಹಿಂತಿರುಗಿ;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	ಆದರೆ (stropt == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;		// type error
	ಗಾಗಿ (; *stropt != NUL; ++stropt)
	    ಆಯ್ಕೆ (*stropt)
	    {
		ಸ್ಥಿತಿ 'a': ಸ್ಥಿತಿ 'A':	// append
		    append = TRUE;
		    ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    get_yank_type(&stropt, &yank_type, &ಖಂಡ_len);
	    }
    }

    ಆದರೆ (regcontents && regcontents->v_type == VAR_LIST)
    {
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	list_T		*ll = regcontents->vval.v_list;
	listitem_T	*li;
	ಪೂರ್ಣಾಂಕ		len;

	// If the list is NULL handle like an empty list.
	len = ll == ಖಾಲಿ ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	ಆದರೆ (lstval == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	ಆದರೆ (ll != ಖಾಲಿ)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_ವಸ್ತುಗಳು(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		ಆದರೆ (strval == ಖಾಲಿ)
		    ಹೋಗು free_lstval;
		ಆದರೆ (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    ಆದರೆ (strval == ಖಾಲಿ)
			ಹೋಗು free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ಖಾಲಿ;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, ಖಂಡ_len);
free_lstval:
	ಆಗ (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	ಆದರೆ (strval == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, ಖಂಡ_len);
    }
    ಆದರೆ (pointreg != 0)
	get_yank_ರಿಜಿಸ್ಟರ್(pointreg, TRUE);

    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
}

/*
 * "settagstack()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_settagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    dict_T	*d;
    ಪೂರ್ಣಾಂಕ		action = 'r';

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    // first argument: window number or id
    wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // second argument: dict with items to set in the tag stack
    ಆದರೆ (check_for_dict_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;
    d = argvars[1].vval.v_dict;
    ಆದರೆ (d == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    ಆದರೆ (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_for_string_arg(argvars, 2) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	ಆದರೆ (actstr == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    ಆದರೆ (ಗಣ_tagstack(wp, d, action) == OK)
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != ಖಾಲಿ ? (char_u *)blob->bv_ga.ga_ದತ್ತಾಂಶ : (char_u *)"";
	len = blob != ಖಾಲಿ ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_ಬೈಟುಗಳು(p, len, ಖಾಲಿ, 0));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_ಬೈಟುಗಳು(p, (ಪೂರ್ಣಾಂಕ)STRLEN(p), ಖಾಲಿ, 0));
    }
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ do_special;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_shiftwidth(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = 0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ದೀರ್ಘ	ಲಂಬಸಾಲು;

	ಲಂಬಸಾಲು = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(argvars, ಖಾಲಿ);
	ಆದರೆ (ಲಂಬಸಾಲು < 0)
	    ಹಿಂತಿರುಗಿ;	// type error; errmsg already given
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	rettv->vval.v_ಸಂಖ್ಯೆ = get_sw_ಮೌಲ್ಯ_ಲಂಬಸಾಲು(curbuf, ಲಂಬಸಾಲು, FALSE);
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = get_sw_ಮೌಲ್ಯ(curbuf);
}

/*
 * "soundfold({word})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_soundfold(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#ಇಲ್ಲದಿದ್ದರೆ
    rettv->vval.v_string = vim_strsave(s);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "spellbadword()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_spellbadword(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    char_u	*ಶಬ್ದ = (char_u *)"";
    hlf_T	attr = HLF_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		len = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    ಪೂರ್ಣಾಂಕ		wo_spell_save = curwin->w_p_spell;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    ಆದರೆ (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_ಗಮ್ಯ(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	ಆದರೆ (len != 0)
	{
	    ಶಬ್ದ = ml_get_cursor();
	    curwin->w_ಗಣ_curswant = true;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	ಪೂರ್ಣಾಂಕ	capcol = -1;

	ಆದರೆ (str != ಖಾಲಿ)
	{
	    // Check the argument for spelling.
	    ಆಗ (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		ಆದರೆ (attr != HLF_ಎಣಿಕೆ)
		{
		    ಶಬ್ದ = str;
		    ನಿಲ್ಲಿಸು;
		}
		str += len;
		capcol -= len;
		len = 0;
	    }
	}
    }
    curwin->w_p_spell = wo_spell_save;
#ಆದರೆ_ಅಂತ್ಯ

    list_append_string(rettv->vval.v_list, ಶಬ್ದ, len);
    ಆಯ್ಕೆ (attr)
    {
    ಸ್ಥಿತಿ HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	ನಿಲ್ಲಿಸು;
    }
}

/*
 * "spellsuggest()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_spellsuggest(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    char_u	*str;
    ಪೂರ್ಣಾಂಕ		typeerr = FALSE;
    ಪೂರ್ಣಾಂಕ		maxcount;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		i;
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		need_capital = FALSE;
    ಪೂರ್ಣಾಂಕ		wo_spell_save = curwin->w_p_spell;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    ಆದರೆ (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &typeerr);
	ಆದರೆ (maxcount <= 0)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[2], &typeerr);
	    ಆದರೆ (typeerr)
		ಹಿಂತಿರುಗಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    ಗಾಗಿ (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_ದತ್ತಾಂಶ)[i];

	li = listitem_alloc();
	ಆದರೆ (li == ಖಾಲಿ)
	    vim_free(str);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_ಬೀಗ = 0;
	    li->li_tv.vval.v_string = str;
	    list_append(rettv->vval.v_list, li);
	}
    }
    ga_clear(&ga);
    curwin->w_p_spell = wo_spell_save;
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಶೂನ್ಯ
f_ವಿಭಜಿಸು(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*ಕೊನೆ;
    char_u	*pat = ಖಾಲಿ;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ;
    colnr_T	ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		keepempty = FALSE;
    ಪೂರ್ಣಾಂಕ		typeerr = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = ಖಾಲಿಗಳು_option;

    str = tv_get_string(&argvars[0]);
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	ಆದರೆ (pat == ಖಾಲಿ)
	    typeerr = TRUE;
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    ಆದರೆ (pat == ಖಾಲಿ || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹೋಗು theend;
    ಆದರೆ (typeerr)
	ಹೋಗು theend;

    regmatch.regprog = eval_regcomp(pat);
    ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
    {
	regmatch.rm_ic = FALSE;
	ಆಗ (*str != NUL || keepempty)
	{
	    ಆದರೆ (*str == NUL)
		ಹೊಂದಿಕೆ = FALSE;	// empty item at the end
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹೊಂದಿಕೆ = vim_regexec_nl(&regmatch, str, ಲಂಬಸಾಲು);
	    ಆದರೆ (ಹೊಂದಿಕೆ)
		ಕೊನೆ = regmatch.startp[0];
	    ಇಲ್ಲದಿದ್ದರೆ
		ಕೊನೆ = str + STRLEN(str);
	    ಆದರೆ (keepempty || ಕೊನೆ > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && ಹೊಂದಿಕೆ && ಕೊನೆ < regmatch.endp[0]))
	    {
		ಆದರೆ (list_append_string(rettv->vval.v_list, str,
						    (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - str)) == FAIL)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (!ಹೊಂದಿಕೆ)
		ನಿಲ್ಲಿಸು;
	    // Advance to just after the match.
	    ಆದರೆ (regmatch.endp[0] > str)
		ಲಂಬಸಾಲು = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		// Don't get stuck at the same match.
		ಲಂಬಸಾಲು = (*mb_ptr2len)(regmatch.endp[0]);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
f_submatch(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    ಪೂರ್ಣಾಂಕ		no;
    ಪೂರ್ಣಾಂಕ		retList = 0;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    no = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_ಸಂಖ್ಯೆ_nr), no);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	retList = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[1], &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = reg_submatch_list(no);
    }
}

/*
 * "substitute()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_substitute(typval_T *argvars, typval_T *rettv)
{
    char_u	patbuf[NUMBUFLEN];
    char_u	subbuf[NUMBUFLEN];
    char_u	flagsbuf[NUMBUFLEN];
    char_u	*str;
    char_u	*pat;
    char_u	*sub = ಖಾಲಿ;
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ = ಖಾಲಿ;
    char_u	*flg;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	ಹಿಂತಿರುಗಿ;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    ಆದರೆ (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	ಅಭಿವ್ಯಕ್ತಿ = &argvars[2];
    ಇಲ್ಲದಿದ್ದರೆ
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    ಆದರೆ (str == ಖಾಲಿ || pat == ಖಾಲಿ || (sub == ಖಾಲಿ && ಅಭಿವ್ಯಕ್ತಿ == ಖಾಲಿ)
								|| flg == ಖಾಲಿ)
	rettv->vval.v_string = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, ಅಭಿವ್ಯಕ್ತಿ, flg, ಖಾಲಿ);
}

/*
 * "swapfilelist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_swapfilelist(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    recover_ಹೆಸರುಗಳು(ಖಾಲಿ, FALSE, rettv->vval.v_list, 0, ಖಾಲಿ);
}

/*
 * "swapinfo(swap_filename)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (rettv_dict_alloc(rettv) == OK)
	get_b0_dict(tv_get_string(argvars), rettv->vval.v_dict);
}

/*
 * "swapname(expr)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_swapname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ಆದರೆ (buf == ಖಾಲಿ || buf->b_ml.ml_mfp == ಖಾಲಿ
					|| buf->b_ml.ml_mfp->mf_fname == ಖಾಲಿ)
	rettv->vval.v_string = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_synID(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		id = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		trans;
    ಪೂರ್ಣಾಂಕ		transerr = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    ಲಂಬಸಾಲು = (linenr_T)tv_get_ಸಂಖ್ಯೆ(&argvars[1]) - 1;	// -1 on type error
    trans = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[2], &transerr);

    ಆದರೆ (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && ಲಂಬಸಾಲು >= 0 && ಲಂಬಸಾಲು < (ದೀರ್ಘ)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, ಲಂಬಸಾಲು, trans, ಖಾಲಿ, FALSE);
#ಆದರೆ_ಅಂತ್ಯ

    rettv->vval.v_ಸಂಖ್ಯೆ = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_synIDattr(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    char_u	*p = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಪೂರ್ಣಾಂಕ		id;
    char_u	*what;
    char_u	*ಬಹುಲಕ;
    char_u	modebuf[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		modec;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಬಹುಲಕ = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(ಬಹುಲಕ[0]);
	ಆದರೆ (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	ಆದರೆ (USE_24_ಬಿಟ್)
	    modec = 'g';
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    ಆದರೆ (t_colors > 1)
		modec = 'c';
	    ಇಲ್ಲದಿದ್ದರೆ
		modec = 't';
    }

    ಆಯ್ಕೆ (TOLOWER_ASC(what[0]))
    {
	ಸ್ಥಿತಿ 'b':
		ಆದರೆ (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		ಇಲ್ಲದಿದ್ದರೆ					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ 'i':
		ಆದರೆ (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		ಇಲ್ಲದಿದ್ದರೆ					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ 'n':
		ಆದರೆ (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		ಇಲ್ಲದಿದ್ದರೆ					// name
		    p = get_highlight_name_ext(ಖಾಲಿ, id - 1, FALSE);
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ 'r':					//ಹಿಮ್ಮುಖ
		p = highlight_has_attr(id, HL_INVERSE, modec);
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ 's':
		ಆದರೆ (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		ಇಲ್ಲದಿದ್ದರೆ					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ 'u':
		ಆದರೆ (STRLEN(what) >= 9)
		{
		    ಆದರೆ (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    ಇಲ್ಲದಿದ್ದರೆ
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		ಇಲ್ಲದಿದ್ದರೆ
							// ul
		    p = highlight_color(id, what, modec);
		ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (p != ಖಾಲಿ)
	p = vim_strsave(p);
#ಆದರೆ_ಅಂತ್ಯ
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_synIDtrans(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		id;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);

    ಆದರೆ (id > 0)
	id = syn_get_final_id(id);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	id = 0;

    rettv->vval.v_ಸಂಖ್ಯೆ = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_synconcealed(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
#ಆದರೆ defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		syntax_flags = 0;
    ಪೂರ್ಣಾಂಕ		cchar;
    ಪೂರ್ಣಾಂಕ		matchid = 0;
    char_u	str[NUMBUFLEN];
#ಆದರೆ_ಅಂತ್ಯ

    rettv_list_ಗಣ(rettv, ಖಾಲಿ);

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

#ಆದರೆ defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    ಲಂಬಸಾಲು = (colnr_T)tv_get_ಸಂಖ್ಯೆ(&argvars[1]) - 1;	// -1 on type error

    CLEAR_ಕ್ಷೇತ್ರ(str);

    ಆದರೆ (rettv_list_alloc(rettv) == OK)
    {
	ಆದರೆ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && ಲಂಬಸಾಲು >= 0 && ಲಂಬಸಾಲು <= (ದೀರ್ಘ)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (ಶೂನ್ಯ)syn_get_id(curwin, lnum, ಲಂಬಸಾಲು, FALSE, ಖಾಲಿ, FALSE);
	    syntax_flags = get_syntax_info(&matchid);

	    // get the conceal character
	    ಆದರೆ ((syntax_flags & HL_CONCEAL) && curwin->w_p_cole < 3)
	    {
		cchar = syn_get_sub_char();
		ಆದರೆ (cchar == NUL && curwin->w_p_cole == 1)
		    cchar = (curwin->w_lcs_chars.conceal == NUL) ? ' '
					: curwin->w_lcs_chars.conceal;
		ಆದರೆ (cchar != NUL)
		{
		    ಆದರೆ (has_mbyte)
			(*mb_char2bytes)(cchar, str);
		    ಇಲ್ಲದಿದ್ದರೆ
			str[0] = cchar;
		}
	    }
	}

	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list,
					    (syntax_flags & HL_CONCEAL) != 0);
	// -1 to auto-determine strlen
	list_append_string(rettv->vval.v_list, str, -1);
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, matchid);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "synstack(lnum, col)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_synstack(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		id;
#ಆದರೆ_ಅಂತ್ಯ

    rettv_list_ಗಣ(rettv, ಖಾಲಿ);

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    ಲಂಬಸಾಲು = (colnr_T)tv_get_ಸಂಖ್ಯೆ(&argvars[1]) - 1;	// -1 on type error

    ಆದರೆ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && ಲಂಬಸಾಲು >= 0 && ಲಂಬಸಾಲು <= (ದೀರ್ಘ)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(ಶೂನ್ಯ)syn_get_id(curwin, lnum, ಲಂಬಸಾಲು, FALSE, ಖಾಲಿ, TRUE);
	ಗಾಗಿ (i = 0; ; ++i)
	{
	    id = syn_get_ರಾಶಿ_ಅಂಶ(i);
	    ಆದರೆ (id < 0)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, id) == FAIL)
		ನಿಲ್ಲಿಸು;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "tabpagebuflist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_tabpagebuflist(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    tabpage_T	*tp;
    win_T	*wp = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	tp = ಕಂಡುಹಿಡಿ_tabpage((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
	ಆದರೆ (tp != ಖಾಲಿ)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    ಆದರೆ (wp != ಖಾಲಿ && rettv_list_alloc(rettv) == OK)
    {
	ಗಾಗಿ (; wp != ಖಾಲಿ; wp = wp->w_ಮುಂದಿನ)
	    ಆದರೆ (list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list,
						wp->w_ಬಫರ್->b_fnum) == FAIL)
		ನಿಲ್ಲಿಸು;
    }
}

/*
 * "tagfiles()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_tagfiles(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    fname = alloc(MAXPATHL);
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ಮೊದಲ = TRUE; ; ಮೊದಲ = FALSE)
	ಆದರೆ (get_tagfname(&tn, ಮೊದಲ, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    ನಿಲ್ಲಿಸು;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = ಖಾಲಿ;
    char_u  *ಟ್ಯಾಗ್_ನಮೂನೆ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಟ್ಯಾಗ್_ನಮೂನೆ = tv_get_string(&argvars[0]);

    rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;
    ಆದರೆ (*ಟ್ಯಾಗ್_ನಮೂನೆ == NUL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    ಆದರೆ (rettv_list_alloc(rettv) == OK)
	(ಶೂನ್ಯ)get_ಟ್ಯಾಗುಗಳು(rettv->vval.v_list, ಟ್ಯಾಗ್_ನಮೂನೆ, fname);
}

/*
 * "type(expr)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_type(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ n = -1;

    ಆಯ್ಕೆ (argvars[0].v_type)
    {
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:  n = VAR_TYPE_ಸಂಖ್ಯೆ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_STRING:  n = VAR_TYPE_STRING; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_PARTIAL:
	ಸ್ಥಿತಿ VAR_FUNC:    n = VAR_TYPE_FUNC; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:    n = VAR_TYPE_LIST; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:   n = VAR_TYPE_TUPLE; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:    n = VAR_TYPE_DICT; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FLOAT:   n = VAR_TYPE_FLOAT; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BOOL:	  n = VAR_TYPE_BOOL; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_SPECIAL: n = VAR_TYPE_NONE; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:     n = VAR_TYPE_ಕೆಲಸ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CHANNEL: n = VAR_TYPE_CHANNEL; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BLOB:    n = VAR_TYPE_BLOB; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_INSTR:   n = VAR_TYPE_INSTR; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		ಆದರೆ (cl != ಖಾಲಿ && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		ಇಲ್ಲದಿದ್ದರೆ
		    n = VAR_TYPE_CLASS;
		ನಿಲ್ಲಿಸು;
	    }
	ಸ್ಥಿತಿ VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		ಆದರೆ (obj == ಖಾಲಿ)
		    n = VAR_TYPE_OBJECT;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    ಆದರೆ (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    ಇಲ್ಲದಿದ್ದರೆ
			n = VAR_TYPE_OBJECT;
		}
		ನಿಲ್ಲಿಸು;
	    }
	ಸ್ಥಿತಿ VAR_VOID:
	    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	    ಆಂತರಿಕ_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    ನಿಲ್ಲಿಸು;
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_ಆರಂಭ = 0;
    colnr_T	vcol_ಕೊನೆ = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    ಪೂರ್ಣಾಂಕ		winchanged = FALSE;
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[2]), &tp);
	ಆದರೆ (wp == ಖಾಲಿ || tp == ಖಾಲಿ)
	    ಹೋಗು theend;

	ಆದರೆ (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    ಹೋಗು theend;

	check_cursor();
	winchanged = TRUE;
    }

    ಪೂರ್ಣಾಂಕ fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    ಆದರೆ (fp != ಖಾಲಿ && fp->lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	ಆದರೆ (fp->ಲಂಬಸಾಲು < 0)
	    fp->ಲಂಬಸಾಲು = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    len = (ಪೂರ್ಣಾಂಕ)ml_get_len(fp->lnum);
	    ಆದರೆ (fp->ಲಂಬಸಾಲು > len)
		fp->ಲಂಬಸಾಲು = len;
	}
	getvvcol(curwin, fp, &vcol_ಆರಂಭ, ಖಾಲಿ, &vcol_ಕೊನೆ, 0);
	++vcol_ಆರಂಭ;
	++vcol_ಕೊನೆ;
    }

theend:
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	ಆದರೆ (rettv_list_alloc(rettv) == OK)
	{
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, vcol_ಆರಂಭ);
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, vcol_ಕೊನೆ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = vcol_ಕೊನೆ;

    ಆದರೆ (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_ಬಹುಲಕ_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    ಆದರೆ (non_zero_arg(&argvars[0]))
	curbuf->b_visual_ಬಹುಲಕ_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_wildmenumode(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (wild_menu_showing || ((State & ಬಹುಲಕ_CMDLINE) && cmdline_pum_ಸಕ್ರಿಯ()))
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
}

/*
 * "windowsversion()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_windowsversion(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windows_ಆವೃತ್ತಿ);
}

/*
 * "wordcount()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_wordcount(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_xor(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ)
					^ tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ಖಾಲಿ);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
