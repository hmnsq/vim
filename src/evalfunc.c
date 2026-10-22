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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

ಸ್ಥಿರ ಶೂನ್ಯ f_and(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_BEVAL
ಸ್ಥಿರ ಶೂನ್ಯ f_balloon_gettext(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_balloon_show(typval_T *argvars, typval_T *rettv);
# ಯದಿ defined(FEAT_BEVAL_TERM)
ಸ್ಥಿರ ಶೂನ್ಯ f_balloon_split(typval_T *argvars, typval_T *rettv);
# endif
#endif
ಸ್ಥಿರ ಶೂನ್ಯ f_base64_encode(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_base64_decode(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_bindtextdomain(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_byte2line(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_call(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_changenr(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_char2nr(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_charcol(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_col(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_confirm(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_copy(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_cursor(typval_T *argsvars, typval_T *rettv);
#ifdef MSWIN
ಸ್ಥಿರ ಶೂನ್ಯ f_debugbreak(typval_T *argvars, typval_T *rettv);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ f_deepcopy(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_did_filetype(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_echoraw(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_empty(typval_T *argvars, typval_T *rettv);
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
ಸ್ಥಿರ ಶೂನ್ಯ f_index(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_indexof(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ಒಳಹರಿವು(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_inputdialog(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_inputlist(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_inputrestore(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_inputsave(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_inputsecret(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_interrupt(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_invert(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_islocked(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ವಸ್ತುಗಳು(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_keytrans(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_last_buffer_nr(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_libcall(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_libcallnr(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ಸಾಲು(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_line2byte(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_LUA
ಸ್ಥಿರ ಶೂನ್ಯ f_luaeval(typval_T *argvars, typval_T *rettv);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ f_match(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_matchbufline(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_matchend(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_matchlist(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_matchstr(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_matchstrlist(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_matchstrpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_max(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_min(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_MZSCHEME
ಸ್ಥಿರ ಶೂನ್ಯ f_mzeval(typval_T *argvars, typval_T *rettv);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ f_nextnonblank(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_ngettext(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_nr2char(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_or(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PERL
ಸ್ಥಿರ ಶೂನ್ಯ f_perleval(typval_T *argvars, typval_T *rettv);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ f_prevnonblank(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_printf(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_pum_getpos(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_pumvisible(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PYTHON3
ಸ್ಥಿರ ಶೂನ್ಯ f_py3eval(typval_T *argvars, typval_T *rettv);
#endif
#ifdef FEAT_PYTHON
ಸ್ಥಿರ ಶೂನ್ಯ f_pyeval(typval_T *argvars, typval_T *rettv);
#endif
#ಯದಿ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
ಸ್ಥಿರ ಶೂನ್ಯ f_pyxeval(typval_T *argvars, typval_T *rettv);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ f_test_srand_seed(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_rand(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_range(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_reg_executing(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_reg_recording(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_rename(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_repeat(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_RUBY
ಸ್ಥಿರ ಶೂನ್ಯ f_rubyeval(typval_T *argvars, typval_T *rettv);
#endif
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
#ifdef FEAT_CRYPT
ಸ್ಥಿರ ಶೂನ್ಯ f_sha256(typval_T *argvars, typval_T *rettv);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ f_shellescape(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_shiftwidth(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_soundfold(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_spellbadword(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_spellsuggest(typval_T *argvars, typval_T *rettv);
ಸ್ಥಿರ ಶೂನ್ಯ f_split(typval_T *argvars, typval_T *rettv);
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ		arg_ಎಣಿಕೆ;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    ಪೂರ್ಣಾಙ್ಕ		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    ಪ್ರತ್ಯಾಗಮ need_type(actual, expected, 0,
	    context->arg_idx - context->arg_ಎಣಿಕೆ, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    ಯದಿ (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_ಎಣಿಕೆ, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    ಪೂರ್ಣಾಙ್ಕ
arg_type_modifiable(type_T *type, ಪೂರ್ಣಾಙ್ಕ arg_idx)
{
    ವರ್ಣ *tofree;

    ಯದಿ ((type->tt_flags & TTFLAG_CONST) == 0)
	ಪ್ರತ್ಯಾಗಮ OK;
    semsg(_(e_argument_nr_trying_to_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_any(type_T *type UNUSED,
	type_T *decl_type UNUSED,
	argcontext_T *context UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check "type" is a float or a number.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_float_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_ಸಂಖ್ಯೆ, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a number.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_ಸಂಖ್ಯೆ(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಪ್ರತ್ಯಾಗಮ check_arg_type(&t_ಸಂಖ್ಯೆ, type, context);
}

/*
 * Check "type" is an object.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_object(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_dict_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಪ್ರತ್ಯಾಗಮ check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಪ್ರತ್ಯಾಗಮ check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    ಪ್ರತ್ಯಾಗಮ check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_ಸಂಖ್ಯೆ(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಪ್ರತ್ಯಾಗಮ check_arg_type(&t_list_ಸಂಖ್ಯೆ, type, context);
}

/*
 * Check "type" is a list of strings.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಪ್ರತ್ಯಾಗಮ check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_tuple_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಪ್ರತ್ಯಾಗಮ check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಪ್ರತ್ಯಾಗಮ check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಪ್ರತ್ಯಾಗಮ check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_bool(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಪ್ರತ್ಯಾಗಮ check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_bool_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_ಸಂಖ್ಯೆ, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_or_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ಯದಿ (arg_list_or_blob(type, decl_type, context) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a string or a number
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_string_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_buffer(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_buffer_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_lnum(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    ಯದಿ (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	ಪ್ರತ್ಯಾಗಮ OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ಯದಿ (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ಯದಿ (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr),
						 context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = ರಿಕ್ತ;
    type_T *(args[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, ರಿಕ್ತ, ರಿಕ್ತ, args};

    ಯದಿ (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	ಯದಿ (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	ಅನ್ಯಥಾ
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    ಅನ್ಯಥಾ ಯದಿ (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    ಅನ್ಯಥಾ ಯದಿ (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_ಸಂಖ್ಯೆ;

    args[0] = ರಿಕ್ತ;
    args[1] = &t_unknown;
    ಯದಿ (type->tt_argcount != -1)
    {
	ಯದಿ (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_ಸಂಖ್ಯೆ_of_arguments));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		args[0] = &t_ಸಂಖ್ಯೆ;
	    ಅನ್ಯಥಾ ಯದಿ (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		args[0] = &t_string;
	    ಯದಿ (args[0] != ರಿಕ್ತ)
		args[1] = expected_member;
	}
    }

    ಯದಿ (!type_any_or_unknown(type->tt_member) || args[0] != ರಿಕ್ತ)
    {
	where_T where = WHERE_INIT;

	ಯದಿ (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == ರಿಕ್ತ
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	ಅನ್ಯಥಾ ಯದಿ (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	ಅನ್ಯಥಾ // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	ಯದಿ (args[0] == ರಿಕ್ತ)
	    args[0] = &t_unknown;
	ಯದಿ (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_index = 2;
	where.wt_kind = WT_ARGUMENT;
	ಪ್ರತ್ಯಾಗಮ check_type(&t_func_exp, type, TRUE, where);
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_filter_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (type->tt_type == VAR_FUNC)
	ಪ್ರತ್ಯಾಗಮ check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_map_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (type->tt_type == VAR_FUNC)
	ಪ್ರತ್ಯಾಗಮ check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_foreach_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (type->tt_type == VAR_FUNC)
	ಪ್ರತ್ಯಾಗಮ check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_sort_how(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (type->tt_type == VAR_FUNC)
    {
	type_T *(args[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_ಸಂಖ್ಯೆ, ರಿಕ್ತ, args};

	ಯದಿ (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    args[0] = context->arg_types[0].type_curr->tt_member;
	ಅನ್ಯಥಾ
	    args[0] = &t_unknown;
	ಯದಿ (!type_any_or_unknown(type->tt_member) || args[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    args[1] = args[0];
	    ಯದಿ (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_index = 2;
	    where.wt_kind = WT_ARGUMENT;
	    ಪ್ರತ್ಯಾಗಮ check_type(&t_func_exp, type, TRUE, where);
	}

	ಪ್ರತ್ಯಾಗಮ OK;
    }
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
varargs_class(type_T *type UNUSED,
	      type_T *decl_type UNUSED,
	      argcontext_T *context)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = context->arg_idx; i < context->arg_ಎಣಿಕೆ; ++i)
    {
	type2_T *types = &context->arg_types[i];
	ಯದಿ (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_reverse(type_T *type, type_T *decl_type, argcontext_T *context)
{
    ಯದಿ (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಪ್ರತ್ಯಾಗಮ check_arg_type(&t_job, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_chan_or_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_JOB
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_same_as_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_decl;

    ಪ್ರತ್ಯಾಗಮ check_arg_type(prev_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_same_struct_as_prev(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;

    ಯದಿ (prev_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	ಪ್ರತ್ಯಾಗಮ check_arg_type(prev_type, type, context);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_item_of_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    ಯದಿ (prev_type->tt_type == VAR_LIST)
	expected = prev_type->tt_member;
    ಅನ್ಯಥಾ ಯದಿ (prev_type->tt_type == VAR_BLOB)
	expected = &t_ಸಂಖ್ಯೆ;
    ಅನ್ಯಥಾ
	// probably VAR_ANY, can't check
	ಪ್ರತ್ಯಾಗಮ OK;

    ಪ್ರತ್ಯಾಗಮ check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 2].type_curr;

    ಯದಿ (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	ಪ್ರತ್ಯಾಗಮ arg_ಸಂಖ್ಯೆ(type, decl_type, context);
    ಯದಿ (first_type->tt_type == VAR_DICT)
	ಪ್ರತ್ಯಾಗಮ arg_string(type, decl_type, context);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_get1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_len1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 1].type_curr;

    ಯದಿ (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	ಪ್ರತ್ಯಾಗಮ arg_ಸಂಖ್ಯೆ(type, decl_type, context);
    ಯದಿ (first_type->tt_type == VAR_DICT)
	ಪ್ರತ್ಯಾಗಮ arg_string_or_nr(type, decl_type, context);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_repeat1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_slice1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ಯದಿ (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr),
							 context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
arg_cursor1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ಯದಿ (type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ಪ್ರತ್ಯಾಗಮ OK;

    arg_type_mismatch(&t_ಸಂಖ್ಯೆ, type, context->arg_idx + 1);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
ಸ್ಥಿರ argcheck_T arg1_blob[] = {arg_blob};
ಸ್ಥಿರ argcheck_T arg1_bool[] = {arg_bool};
ಸ್ಥಿರ argcheck_T arg1_buffer[] = {arg_buffer};
ಸ್ಥಿರ argcheck_T arg1_buffer_or_dict_any[] = {arg_buffer_or_dict_any};
ಸ್ಥಿರ argcheck_T arg1_chan_or_job[] = {arg_chan_or_job};
ಸ್ಥಿರ argcheck_T arg1_dict_any[] = {arg_dict_any};
ಸ್ಥಿರ argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
ಸ್ಥಿರ argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
ಸ್ಥಿರ argcheck_T arg1_job[] = {arg_job};
ಸ್ಥಿರ argcheck_T arg1_list_any[] = {arg_list_any};
ಸ್ಥಿರ argcheck_T arg1_tuple_any[] = {arg_tuple_any};
ಸ್ಥಿರ argcheck_T arg1_list_ಸಂಖ್ಯೆ[] = {arg_list_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg1_reverse[] = {arg_reverse};
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
ಸ್ಥಿರ argcheck_T arg2_buffer_any[] = {arg_buffer, arg_any};
ಸ್ಥಿರ argcheck_T arg2_buffer_bool[] = {arg_buffer, arg_bool};
ಸ್ಥಿರ argcheck_T arg2_buffer_list_any[] = {arg_buffer, arg_list_any};
ಸ್ಥಿರ argcheck_T arg2_buffer_lnum[] = {arg_buffer, arg_lnum};
ಸ್ಥಿರ argcheck_T arg2_buffer_ಸಂಖ್ಯೆ[] = {arg_buffer, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_buffer_string[] = {arg_buffer, arg_string};
ಸ್ಥಿರ argcheck_T arg2_chan_or_job_dict[] = {arg_chan_or_job, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_chan_or_job_string[] = {arg_chan_or_job, arg_string};
ಸ್ಥಿರ argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
ಸ್ಥಿರ argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
ಸ್ಥಿರ argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
ಸ್ಥಿರ argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
ಸ್ಥಿರ argcheck_T arg2_job_dict[] = {arg_job, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_job_string_or_ಸಂಖ್ಯೆ[] = {arg_job, arg_string_or_nr};
ಸ್ಥಿರ argcheck_T arg2_list_any_ಸಂಖ್ಯೆ[] = {arg_list_any, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_list_ಸಂಖ್ಯೆ[] = {arg_list_ಸಂಖ್ಯೆ, arg_list_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_list_ಸಂಖ್ಯೆ_bool[] = {arg_list_ಸಂಖ್ಯೆ, arg_bool};
ಸ್ಥಿರ argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_listblobmod_item[] = {arg_list_or_blob_mod, arg_item_of_prev};
ಸ್ಥಿರ argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
ಸ್ಥಿರ argcheck_T arg2_lnum_ಸಂಖ್ಯೆ[] = {arg_lnum, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ[] = {arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_any[] = {arg_ಸಂಖ್ಯೆ, arg_any};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_bool[] = {arg_ಸಂಖ್ಯೆ, arg_bool};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_dict_any[] = {arg_ಸಂಖ್ಯೆ, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_list[] = {arg_ಸಂಖ್ಯೆ, arg_list_any};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_string[] = {arg_ಸಂಖ್ಯೆ, arg_string};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_buffer[] = {arg_ಸಂಖ್ಯೆ, arg_buffer};
ಸ್ಥಿರ argcheck_T arg2_ಸಂಖ್ಯೆ_string_or_list[] = {arg_ಸಂಖ್ಯೆ, arg_string_or_list_any};
ಸ್ಥಿರ argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_string[] = {arg_string, arg_string};
ಸ್ಥಿರ argcheck_T arg2_string_any[] = {arg_string, arg_any};
ಸ್ಥಿರ argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
ಸ್ಥಿರ argcheck_T arg2_string_chan_or_job[] = {arg_string, arg_chan_or_job};
ಸ್ಥಿರ argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_string_list_ಸಂಖ್ಯೆ[] = {arg_string, arg_list_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_string_ಸಂಖ್ಯೆ[] = {arg_string, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_string_or_list_ಸಂಖ್ಯೆ[] = {arg_string_or_list_any, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_string_string_or_ಸಂಖ್ಯೆ[] = {arg_string, arg_string_or_nr};
ಸ್ಥಿರ argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
ಸ್ಥಿರ argcheck_T arg3_any_buffer_bool_or_dict[] = {
			      arg_any, arg_buffer, arg_bool_or_dict_any};
ಸ್ಥಿರ argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_buffer_lnum_lnum[] = {arg_buffer, arg_lnum, arg_lnum};
ಸ್ಥಿರ argcheck_T arg3_buffer_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ[] = {arg_buffer, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg3_buffer_string_any[] = {arg_buffer, arg_string, arg_any};
ಸ್ಥಿರ argcheck_T arg3_buffer_string_dict[] = {arg_buffer, arg_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_dict_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ[] = {arg_dict_any, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_lnum_ಸಂಖ್ಯೆ_bool[] = {arg_lnum, arg_ಸಂಖ್ಯೆ, arg_bool};
ಸ್ಥಿರ argcheck_T arg3_ಸಂಖ್ಯೆ[] = {arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg3_ಸಂಖ್ಯೆ_any_dict[] = {arg_ಸಂಖ್ಯೆ, arg_any, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ_dict[] = {arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg3_ಸಂಖ್ಯೆ_string_any[] = {arg_ಸಂಖ್ಯೆ, arg_string, arg_any};
ಸ್ಥಿರ argcheck_T arg3_ಸಂಖ್ಯೆ_string_buffer[] = {arg_ಸಂಖ್ಯೆ, arg_string, arg_buffer};
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
ಸ್ಥಿರ argcheck_T arg23_chanexpr[] = {arg_chan_or_job, arg_any, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg23_chanraw[] = {arg_chan_or_job, arg_string_or_blob, arg_dict_any};
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
ಸ್ಥಿರ argcheck_T arg24_index[] = {arg_list_or_tuple_or_blob, arg_item_of_prev, arg_ಸಂಖ್ಯೆ, arg_bool};
ಸ್ಥಿರ argcheck_T arg23_index[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg23_ಸೇರಿಸು[] = {arg_list_or_blob, arg_item_of_prev, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg1_len[] = {arg_len1};
ಸ್ಥಿರ argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
ಸ್ಥಿರ argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
ಸ್ಥಿರ argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
ಸ್ಥಿರ argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
ಸ್ಥಿರ argcheck_T arg2_instanceof[] = {arg_object, varargs_class, ರಿಕ್ತ };
ಸ್ಥಿರ argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
ಸ್ಥಿರ argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
ಸ್ಥಿರ argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg45_matchbufline[] = {arg_buffer, arg_string, arg_lnum, arg_lnum, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
ಸ್ಥಿರ argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
ಸ್ಥಿರ argcheck_T arg24_remote_ಅಭಿವ್ಯಕ್ತಿ[] = {arg_string, arg_string, arg_string, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg2_repeat[] = {arg_repeat1, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg15_ಹುಡುಕು[] = {arg_string, arg_string, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ, arg_string_or_func};
ಸ್ಥಿರ argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg3_setbufline[] = {arg_buffer, arg_lnum, arg_str_or_nr_or_list};
ಸ್ಥಿರ argcheck_T arg2_setline[] = {arg_lnum, arg_any};
ಸ್ಥಿರ argcheck_T arg24_setloclist[] = {arg_ಸಂಖ್ಯೆ, arg_list_any, arg_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg23_settagstack[] = {arg_ಸಂಖ್ಯೆ, arg_dict_any, arg_string};
ಸ್ಥಿರ argcheck_T arg02_sign_getplaced[] = {arg_buffer, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg45_sign_place[] = {arg_ಸಂಖ್ಯೆ, arg_string, arg_string, arg_buffer, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg23_slice[] = {arg_slice1, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};
ಸ್ಥಿರ argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_sort_how, arg_dict_any};
ಸ್ಥಿರ argcheck_T arg24_strpart[] = {arg_string, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ, arg_bool};
ಸ್ಥಿರ argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
ಸ್ಥಿರ argcheck_T arg23_win_execute[] = {arg_ಸಂಖ್ಯೆ, arg_string_or_list_string, arg_string};
ಸ್ಥಿರ argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
ಸ್ಥಿರ argcheck_T arg24_match_func[] = {arg_string_or_list_any, arg_string, arg_ಸಂಖ್ಯೆ, arg_ಸಂಖ್ಯೆ};

// Can be used by functions called through "f_retfunc" to create new types.
ಸ್ಥಿರ garray_T *current_type_gap = ರಿಕ್ತ;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    ಸ್ಥಿರ type_T *
ret_void(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_void;
}
    ಸ್ಥಿರ type_T *
ret_any(ಪೂರ್ಣಾಙ್ಕ	argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_any;
}
    ಸ್ಥಿರ type_T *
ret_bool(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_bool;
}
    ಸ್ಥಿರ type_T *
ret_ಸಂಖ್ಯೆ_bool(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_ಸಂಖ್ಯೆ_bool;
}
    ಸ್ಥಿರ type_T *
ret_ಸಂಖ್ಯೆ(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_ಸಂಖ್ಯೆ;
}
    ಸ್ಥಿರ type_T *
ret_float(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_float;
}
    ಸ್ಥಿರ type_T *
ret_string(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_string;
}
    ಸ್ಥಿರ type_T *
ret_list_any(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_list_any;
}
    ಸ್ಥಿರ type_T *
ret_list_ಸಂಖ್ಯೆ(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಪ್ರತ್ಯಾಗಮ &t_list_ಸಂಖ್ಯೆ;
}
    ಸ್ಥಿರ type_T *
ret_list_string(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಪ್ರತ್ಯಾಗಮ &t_list_string;
}
    ಸ್ಥಿರ type_T *
ret_list_dict_any(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಪ್ರತ್ಯಾಗಮ &t_list_dict_any;
}
    ಸ್ಥಿರ type_T *
ret_list_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಪ್ರತ್ಯಾಗಮ &t_list_list_any;
}
    ಸ್ಥಿರ type_T *
ret_list_list_any(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಪ್ರತ್ಯಾಗಮ &t_list_list_any;
}
    ಸ್ಥಿರ type_T *
ret_list_string_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಪ್ರತ್ಯಾಗಮ &t_list_list_string;
}
    ಸ್ಥಿರ type_T *
ret_list_regionpos(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ಪ್ರತ್ಯಾಗಮ &t_list_list_list_ಸಂಖ್ಯೆ;
}
    ಸ್ಥಿರ type_T *
ret_tuple_any(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_tuple_any;
}
    ಸ್ಥಿರ type_T *
ret_dict_any(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_dict_any;
}
    ಸ್ಥಿರ type_T *
ret_job_info(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    ಯದಿ (argcount == 0)
    {
	*decl_type = &t_list_any;
	ಪ್ರತ್ಯಾಗಮ &t_list_job;
    }
    ಪ್ರತ್ಯಾಗಮ &t_dict_any;
}
    ಸ್ಥಿರ type_T *
ret_dict_ಸಂಖ್ಯೆ(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_dict_ಸಂಖ್ಯೆ;
}
    ಸ್ಥಿರ type_T *
ret_dict_string(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_dict_string;
}
    ಸ್ಥಿರ type_T *
ret_blob(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_blob;
}
    ಸ್ಥಿರ type_T *
ret_func_any(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_func_any;
}
    ಸ್ಥಿರ type_T *
ret_func_unknown(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_func_unknown;
}
    ಸ್ಥಿರ type_T *
ret_channel(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_channel;
}
    ಸ್ಥಿರ type_T *
ret_job(ಪೂರ್ಣಾಙ್ಕ argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ &t_job;
}
    ಸ್ಥಿರ type_T *
ret_first_arg(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಯದಿ (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	ಪ್ರತ್ಯಾಗಮ argtypes[0].type_curr;
    }
    ಪ್ರತ್ಯಾಗಮ &t_void;
}
    ಸ್ಥಿರ type_T *
ret_slice(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಯದಿ (argcount > 0)
    {
	ಯದಿ (argtypes[0].type_decl != ರಿಕ್ತ)
	{
	    ವಿಕಲ್ಪ (argtypes[0].type_decl->tt_type)
	    {
		ಪಕ್ಷ VAR_STRING: *decl_type = &t_string; ಭಙ್ಗ;
		ಪಕ್ಷ VAR_BLOB: *decl_type = &t_blob; ಭಙ್ಗ;
		ಪಕ್ಷ VAR_LIST: *decl_type = &t_list_any; ಭಙ್ಗ;
		ಪಕ್ಷ VAR_TUPLE: *decl_type = &t_tuple_any; ಭಙ್ಗ;
		ಸ್ವಭಾವ: ಭಙ್ಗ;
	    }
	}
	ಪ್ರತ್ಯಾಗಮ argtypes[0].type_curr;
    }
    ಪ್ರತ್ಯಾಗಮ &t_void;
}
    ಸ್ಥಿರ type_T *
ret_copy(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಯದಿ (argcount > 0)
    {
	ಯದಿ (argtypes[0].type_decl != ರಿಕ್ತ)
	{
	    ಯದಿ (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    ಅನ್ಯಥಾ ಯದಿ (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    ಅನ್ಯಥಾ
		*decl_type = argtypes[0].type_decl;
	}
	ಯದಿ (argtypes[0].type_curr != ರಿಕ್ತ)
	{
	    ಯದಿ (argtypes[0].type_curr->tt_type == VAR_LIST)
		ಪ್ರತ್ಯಾಗಮ &t_list_any;
	    ಅನ್ಯಥಾ ಯದಿ (argtypes[0].type_curr->tt_type == VAR_DICT)
		ಪ್ರತ್ಯಾಗಮ &t_dict_any;
	}
	ಪ್ರತ್ಯಾಗಮ argtypes[0].type_curr;
    }
    ಪ್ರತ್ಯಾಗಮ &t_void;
}
    ಸ್ಥಿರ type_T *
ret_extend(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಯದಿ (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	// if the second argument has a different current type then the current
	// type is "any"
	ಯದಿ (argcount > 1 && !equal_type(argtypes[0].type_curr,
						     argtypes[1].type_curr, 0))
	{
	    ಯದಿ (argtypes[0].type_curr->tt_type == VAR_LIST)
		ಪ್ರತ್ಯಾಗಮ &t_list_any;
	    ಯದಿ (argtypes[0].type_curr->tt_type == VAR_DICT)
		ಪ್ರತ್ಯಾಗಮ &t_dict_any;
	    ಯದಿ (argtypes[0].type_curr->tt_type == VAR_BLOB)
		ಪ್ರತ್ಯಾಗಮ &t_blob;
	}
	ಪ್ರತ್ಯಾಗಮ argtypes[0].type_curr;
    }
    ಪ್ರತ್ಯಾಗಮ &t_void;
}
    ಸ್ಥಿರ type_T *
ret_repeat(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    ಯದಿ (argcount == 0)
	ಪ್ರತ್ಯಾಗಮ &t_any;
    ಯದಿ (argtypes[0].type_curr == &t_ಸಂಖ್ಯೆ)
	ಪ್ರತ್ಯಾಗಮ &t_string;
    ಪ್ರತ್ಯಾಗಮ argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    ಸ್ಥಿರ type_T *
ret_first_cont(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    ಯದಿ (argcount > 0)
    {
	ಯದಿ (argtypes[0].type_curr->tt_type == VAR_LIST)
	    ಪ್ರತ್ಯಾಗಮ &t_list_any;
	ಯದಿ (argtypes[0].type_curr->tt_type == VAR_DICT)
	    ಪ್ರತ್ಯಾಗಮ &t_dict_any;
	ಯದಿ (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    ಪ್ರತ್ಯಾಗಮ argtypes[0].type_curr;
    }
    ಪ್ರತ್ಯಾಗಮ &t_any;
}
// for get(): the type of the item when a missing item has the same type
    ಸ್ಥಿರ type_T *
ret_get(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಯದಿ (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*item = ರಿಕ್ತ;

	ಯದಿ (t->tt_type == VAR_BLOB)
	    item = &t_ಸಂಖ್ಯೆ;
	ಅನ್ಯಥಾ ಯದಿ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    item = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	ಯದಿ (item != ರಿಕ್ತ && (argcount == 3
			? equal_type(item, argtypes[2].type_curr, 0)
			: item->tt_type == VAR_ಸಂಖ್ಯೆ))
	{
	    ಯದಿ (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    ಪ್ರತ್ಯಾಗಮ item;
	}
    }
    ಪ್ರತ್ಯಾಗಮ &t_any;
}
// for getline()
    ಸ್ಥಿರ type_T *
ret_getline(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಯದಿ (argcount == 1)
	ಪ್ರತ್ಯಾಗಮ &t_string;
    *decl_type = &t_list_any;
    ಪ್ರತ್ಯಾಗಮ &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    ಸ್ಥಿರ type_T *
ret_string_or_any_1(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಯದಿ (argcount < 1)
	ಪ್ರತ್ಯಾಗಮ &t_string;
    ಪ್ರತ್ಯಾಗಮ &t_any;
}
    ಸ್ಥಿರ type_T *
ret_string_or_any_2(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಯದಿ (argcount < 2)
	ಪ್ರತ್ಯಾಗಮ &t_string;
    ಪ್ರತ್ಯಾಗಮ &t_any;
}
    ಸ್ಥಿರ type_T *
ret_string_or_any_3(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಯದಿ (argcount < 3)
	ಪ್ರತ್ಯಾಗಮ &t_string;
    ಪ್ರತ್ಯಾಗಮ &t_any;
}
    ಸ್ಥಿರ type_T *
ret_string_or_any_4(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ಯದಿ (argcount < 4)
	ಪ್ರತ್ಯಾಗಮ &t_string;
    ಪ್ರತ್ಯಾಗಮ &t_any;
}
// for abs()
    ಸ್ಥಿರ type_T *
ret_abs(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    ಯದಿ (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	ಪ್ರತ್ಯಾಗಮ &t_float;
    ಪ್ರತ್ಯಾಗಮ &t_ಸಂಖ್ಯೆ;
}
// for sign_define()
    ಸ್ಥಿರ type_T *
ret_sign_define(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    ಯದಿ (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	ಪ್ರತ್ಯಾಗಮ &t_list_ಸಂಖ್ಯೆ;
    ಪ್ರತ್ಯಾಗಮ &t_ಸಂಖ್ಯೆ;
}
// for values(): list of member of first argument
    ಸ್ಥಿರ type_T *
ret_list_member(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಯದಿ (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	ಯದಿ (current_type_gap != ರಿಕ್ತ
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	ಅನ್ಯಥಾ
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	ಯದಿ (current_type_gap != ರಿಕ್ತ
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    ಪ್ರತ್ಯಾಗಮ get_list_type(t->tt_member, current_type_gap);
    }
    ಪ್ರತ್ಯಾಗಮ &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    ಸ್ಥಿರ type_T *
ret_list_or_dict_0(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    ಯದಿ (argcount > 0)
	ಪ್ರತ್ಯಾಗಮ &t_dict_any;
    *decl_type = &t_list_any;
    ಪ್ರತ್ಯಾಗಮ &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    ಸ್ಥಿರ type_T *
ret_list_or_dict_1(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    ಯದಿ (argcount > 1)
	ಪ್ರತ್ಯಾಗಮ &t_dict_any;
    *decl_type = &t_list_any;
    ಪ್ರತ್ಯಾಗಮ &t_list_dict_any;
}

    ಸ್ಥಿರ type_T *
ret_argv(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // argv() returns list of strings
    ಯದಿ (argcount == 0)
    {
	*decl_type = &t_list_any;
	ಪ್ರತ್ಯಾಗಮ &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    ಪ್ರತ್ಯಾಗಮ &t_any;
}

    ಸ್ಥಿರ type_T *
ret_remove(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಯದಿ (argcount > 0)
    {
	ಯದಿ (argtypes[0].type_curr->tt_type == VAR_LIST
		|| argtypes[0].type_curr->tt_type == VAR_DICT)
	{
	    ಯದಿ (argcount == 3)
	    {
		*decl_type = argtypes[0].type_decl;
		ಪ್ರತ್ಯಾಗಮ argtypes[0].type_curr;
	    }
	    ಯದಿ (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    ಪ್ರತ್ಯಾಗಮ argtypes[0].type_curr->tt_member;
	}
	ಯದಿ (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    ಪ್ರತ್ಯಾಗಮ &t_ಸಂಖ್ಯೆ;
    }
    ಪ್ರತ್ಯಾಗಮ &t_any;
}

    ಸ್ಥಿರ type_T *
ret_max_min(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ಯದಿ (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	ಯದಿ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	ಅನ್ಯಥಾ
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	ಯದಿ (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	ಅನ್ಯಥಾ
	    t = &t_any;
	ಪ್ರತ್ಯಾಗಮ t;
    }
    ಪ್ರತ್ಯಾಗಮ &t_any;
}

    ಸ್ಥಿರ type_T *
ret_getreg(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    ಯದಿ (argcount == 3)
    {
	*decl_type = &t_list_any;
	ಪ್ರತ್ಯಾಗಮ &t_list_string;
    }
    ಪ್ರತ್ಯಾಗಮ &t_string;
}

    ಸ್ಥಿರ type_T *
ret_virtcol(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    ಯದಿ (argcount > 1)
    {
	*decl_type = &t_list_any;
	ಪ್ರತ್ಯಾಗಮ &t_list_ಸಂಖ್ಯೆ;
    }
    ಪ್ರತ್ಯಾಗಮ &t_ಸಂಖ್ಯೆ;
}

    ಸ್ಥಿರ type_T *
ret_maparg(ಪೂರ್ಣಾಙ್ಕ argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    // Assume that if the fourth argument is passed it's non-zero
    ಯದಿ (argcount == 4)
	ಪ್ರತ್ಯಾಗಮ &t_dict_any;
    ಪ್ರತ್ಯಾಗಮ &t_string;
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ವರ್ಣ	*f_name;	// function name
    ವರ್ಣ	f_min_argc;	// minimal number of arguments
    ವರ್ಣ	f_max_argc;	// maximal number of arguments
    ವರ್ಣ	f_argtype;	// for method: FEARG_ values; bits FE_
    argcheck_T	*f_argcheck;	// list of functions to check argument types;
				// use "arg_any" (not NULL) to accept an
				// argument of any type
    ವರ್ಣ	*f_argnames;	// names of the arguments as in the help,
				// separated by ", "; an argument without a
				// name is empty, NULL when there is none
    type_T	*(*f_retfunc)(ಪೂರ್ಣಾಙ್ಕ argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    ಶೂನ್ಯ	(*f_func)(typval_T *args, typval_T *rvar);
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

#ಯದಿ defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#ಅನ್ಯಥಾ
# define MATH_FUNC(name) ರಿಕ್ತ
#endif
#ifdef FEAT_TIMERS
# define TIMER_FUNC(name) name
#ಅನ್ಯಥಾ
# define TIMER_FUNC(name) ರಿಕ್ತ
#endif
#ifdef FEAT_JOB_CHANNEL
# define JOB_FUNC(name) name
#ಅನ್ಯಥಾ
# define JOB_FUNC(name) ರಿಕ್ತ
#endif
#ifdef FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#ಅನ್ಯಥಾ
# define PROP_FUNC(name) ರಿಕ್ತ
#endif
#ifdef FEAT_SIGNS
# define SIGN_FUNC(name) name
#ಅನ್ಯಥಾ
# define SIGN_FUNC(name) ರಿಕ್ತ
#endif
#ifdef FEAT_SOUND
# define SOUND_FUNC(name) name
#ಅನ್ಯಥಾ
# define SOUND_FUNC(name) ರಿಕ್ತ
#endif
#ifdef FEAT_TERMINAL
# define TERM_FUNC(name) name
#ಅನ್ಯಥಾ
# define TERM_FUNC(name) ರಿಕ್ತ
#endif
#ifdef FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#ಅನ್ಯಥಾ
# define TABPANEL_FUNC(name) ರಿಕ್ತ
#endif
#ifdef FEAT_IMAGE
# define IMAGE_FUNC(name) name
#ಅನ್ಯಥಾ
# define IMAGE_FUNC(name) ರಿಕ್ತ
#endif

ಸ್ಥಿರ ಧ್ರುವ funcentry_T global_functions[] =
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
    {"argidx",		0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"assert_equal",	2, 3, FEARG_2,	    ರಿಕ್ತ,
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
    {"assert_false",	1, 2, FEARG_1,	    ರಿಕ್ತ,
			"actual, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_match},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    ರಿಕ್ತ,
			"expected, actual, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_ಸಂಖ್ಯೆ_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    ರಿಕ್ತ,
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
			ret_bool,	    f_autocmd_add},
    {"autocmd_delete",	1, 1, FEARG_1,	    arg1_list_any,
			"acmds",
			ret_bool,	    f_autocmd_delete},
    {"autocmd_get",	0, 1, FEARG_1,	    arg1_dict_any,
			"opts",
			ret_list_dict_any,  f_autocmd_get},
    {"balloon_gettext",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,
#ifdef FEAT_BEVAL
	    f_balloon_gettext
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#ifdef FEAT_BEVAL
	    f_balloon_show
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#ಯದಿ defined(FEAT_BEVAL_TERM)
	    f_balloon_split
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
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
    {"bufexists",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_ಸಂಖ್ಯೆ_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_ಸಂಖ್ಯೆ_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_ಸಂಖ್ಯೆ,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_ಸಂಖ್ಯೆ_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_ಸಂಖ್ಯೆ_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, create",
			ret_ಸಂಖ್ಯೆ,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_ಸಂಖ್ಯೆ,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_buffer,
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
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_job,
			"handle",
			ret_ಸಂಖ್ಯೆ_bool,    JOB_FUNC(f_ch_canread)},
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
			ret_ಸಂಖ್ಯೆ,	    JOB_FUNC(f_ch_getbufnr)},
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
    {"changenr",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"cmdcomplete_info",0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_ಸಂಖ್ಯೆ,
			"expr, winid",
			ret_ಸಂಖ್ಯೆ,	    f_col},
    {"complete",	2, 2, FEARG_2,	    arg2_ಸಂಖ್ಯೆ_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_ಸಂಖ್ಯೆ,	    f_complete_add},
    {"complete_check",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_ಸಂಖ್ಯೆ_string,
			"msg, choices, default, type",
			ret_ಸಂಖ್ಯೆ,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    ರಿಕ್ತ,
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
#ifdef MSWIN
	    f_debugbreak
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_ಸಂಖ್ಯೆ_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_buffer_lnum_lnum,
			"buf, first, last",
			ret_ಸಂಖ್ಯೆ_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
			ret_bool,	f_digraph_set},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    ರಿಕ್ತ,
			"expr",
			ret_ಸಂಖ್ಯೆ_bool,    f_empty},
    {"environ",		0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    ರಿಕ್ತ,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
			ret_first_cont,	    f_extendnew},
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
			ret_first_arg,	    f_filter},
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
    {"foldtext",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_ಸಂಖ್ಯೆ,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_first_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"getbgcolor",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"getcellpixels",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_ಸಂಖ್ಯೆ,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"getimstatus",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"getmarklist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_ಸಂಖ್ಯೆ,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_dict_ಸಂಖ್ಯೆ,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"getstacktrace",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"getwinposx",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"hostname",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    ರಿಕ್ತ,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_ಸಂಖ್ಯೆ,	    IMAGE_FUNC(f_image_add)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ಸಂಖ್ಯೆ,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_index,
			"object, expr, start, ic",
			ret_ಸಂಖ್ಯೆ,	    f_index},
    {"indexof",		2, 3, FEARG_1,	    arg23_index,
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
    {"inputrestore",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_ಸೇರಿಸು,
			"object, item, idx",
			ret_first_arg,	    f_ಸೇರಿಸು},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_void,	    f_interrupt},
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
    {"job_stop",	1, 2, FEARG_1,	    arg2_job_string_or_ಸಂಖ್ಯೆ,
			"job, how",
			ret_ಸಂಖ್ಯೆ_bool,    JOB_FUNC(f_job_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    ರಿಕ್ತ,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    ರಿಕ್ತ,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_ಕೀಲಿಗಳು},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    ರಿಕ್ತ,	// obsolete
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ,	    f_last_buffer_nr},
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
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_buffer_bool_or_dict,
			"callback, buf, options",
			ret_ಸಂಖ್ಯೆ,	    f_listener_add},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_ಸಂಖ್ಯೆ_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
#ifdef FEAT_LUA
		f_luaeval
#ಅನ್ಯಥಾ
		ರಿಕ್ತ
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
			ret_ಸಂಖ್ಯೆ,	    f_match},
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
    {"matchend",	2, 4, FEARG_1,	    arg24_match_func,
			"expr, pat, start, count",
			ret_ಸಂಖ್ಯೆ,	    f_matchend},
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
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_ಸಂಖ್ಯೆ,
			"name, flags, prot",
			ret_ಸಂಖ್ಯೆ_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_mode},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_MZSCHEME
	    f_mzeval
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
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
#ifdef FEAT_PERL
	    f_perleval
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
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
    {"popup_findecho",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"popup_list",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_buffer,
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
    {"preinserted",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ಸಂಖ್ಯೆ,	    f_prevnonblank},
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
    {"prop_add",	3, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ_dict,
			"lnum, col, props",
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_prop_add)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_add_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_find)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ,
			"props, lnum, lnum_end",
			ret_ಸಂಖ್ಯೆ,	    PROP_FUNC(f_prop_remove)},
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
    {"pum_getpos",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_dict_ಸಂಖ್ಯೆ,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON3
	    f_py3eval
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON
	    f_pyeval
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ಯದಿ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
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
			ret_ಸಂಖ್ಯೆ,	    f_redraw_listener_add},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
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
    {"screencol",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ,
			"winid, lnum, col",
			ret_dict_ಸಂಖ್ಯೆ,    f_screenpos},
    {"screenrow",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"setbufvar",	3, 3, FEARG_3,	    arg3_buffer_string_any,
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
#ifdef FEAT_CRYPT
	    f_sha256
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
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
    {"sign_jump",	3, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ_string_buffer,
			"id, group, buf",
			ret_ಸಂಖ್ಯೆ,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_place,
			"id, group, name, buf, dict",
			ret_ಸಂಖ್ಯೆ,	    SIGN_FUNC(f_sign_place)},
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
    {"slice",		2, 3, FEARG_1,	    arg23_slice,
			"expr, start, end",
			ret_slice,	    f_slice},
    {"sort",		1, 3, FEARG_1,	    arg13_sortuniq,
			"list, how, dict",
			ret_first_arg,	    f_sort},
    {"sound_clear",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
			ret_list_string,    f_split},
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
#ifdef HAVE_STRFTIME
	    f_strftime
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_ಸಂಖ್ಯೆ,
			"str, index",
			ret_ಸಂಖ್ಯೆ,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_ಸಂಖ್ಯೆ,
			"haystack, needle, start",
			ret_ಸಂಖ್ಯೆ,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, ರಿಕ್ತ,
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
#ifdef HAVE_STRPTIME
	    f_strptime
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
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
    {"swapfilelist",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_buffer,
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
    {"tabpanel_getinfo", 0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"tempname",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_ಸಂಖ್ಯೆ,	    TERM_FUNC(f_term_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_ಸಂಖ್ಯೆ,	    TERM_FUNC(f_term_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_buffer_string_dict,
			"buf, filename, options",
			ret_void,	    TERM_FUNC(f_term_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_buffer,
			"buf",
			ret_ಸಂಖ್ಯೆ,	    TERM_FUNC(f_term_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_buffer,
			"buf",
			ret_list_string,
#ಯದಿ defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_getansicolors
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
#endif
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_string,
			"attr, what",
			ret_ಸಂಖ್ಯೆ,	    TERM_FUNC(f_term_getattr)},
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
			ret_ಸಂಖ್ಯೆ,	    TERM_FUNC(f_term_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_ಸಂಖ್ಯೆ,    TERM_FUNC(f_term_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, input",
			ret_string,	    TERM_FUNC(f_term_gettty)},
    {"term_list",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_list_ಸಂಖ್ಯೆ,    TERM_FUNC(f_term_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_buffer_lnum,
			"buf, row",
			ret_list_dict_any,  TERM_FUNC(f_term_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_buffer_string,
			"buf, keys",
			ret_void,	    TERM_FUNC(f_term_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_buffer_list_any,
			"buf, colors",
			ret_void,
#ಯದಿ defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_setansicolors
#ಅನ್ಯಥಾ
	    ರಿಕ್ತ
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
    {"term_setsize",	3, 3, FEARG_1,	    arg3_buffer_ಸಂಖ್ಯೆ_ಸಂಖ್ಯೆ,
			"buf, rows, cols",
			ret_void,	    TERM_FUNC(f_term_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_ಸಂಖ್ಯೆ,	    TERM_FUNC(f_term_start)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_buffer_ಸಂಖ್ಯೆ,
			"buf, time",
			ret_void,	    TERM_FUNC(f_term_wait)},
    {"terminalprops",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"test_null_blob",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_channel,	    JOB_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_job,	    JOB_FUNC(f_test_null_job)},
    {"test_null_list",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_set},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_ಸಂಖ್ಯೆ,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, ರಿಕ್ತ,
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
			ret_void,	    f_test_srand_seed},
    {"test_unknown",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_ಸಂಖ್ಯೆ_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_ಸಂಖ್ಯೆ_any_dict,
			"time, callback, options",
			ret_ಸಂಖ್ಯೆ,	    TIMER_FUNC(f_timer_start)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"type",		1, 1, FEARG_1|FE_X, ರಿಕ್ತ,
			"expr",
			ret_ಸಂಖ್ಯೆ,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, ರಿಕ್ತ,
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
    {"wildmenumode",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    {"wincol",		0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"nr",
			ret_ಸಂಖ್ಯೆ,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_ಸಂಖ್ಯೆ,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_ಸಂಖ್ಯೆ,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
			ret_dict_ಸಂಖ್ಯೆ,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_ಸಂಖ್ಯೆ,
			"nr",
			ret_ಸಂಖ್ಯೆ,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    ರಿಕ್ತ,
			ರಿಕ್ತ,
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
func_allows_type(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	intidx = -1;
    char_u	*name;

    ಯದಿ (idx == 0)
	intidx = -1;
    ಯದಿ (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	ಯದಿ (name != ರಿಕ್ತ)
	{
	    ಯದಿ (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_pattern, 2) == 0)
		ಪ್ರತ್ಯಾಗಮ cat_prefix_varname('g', name);
	    ಪ್ರತ್ಯಾಗಮ name;
	}
    }
    ಯದಿ (++intidx < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	ಯದಿ (global_functions[intidx].f_func == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	ಯದಿ (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	ಪ್ರತ್ಯಾಗಮ IObuff;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_ಅಭಿವ್ಯಕ್ತಿ_name(expand_T *xp, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	intidx = -1;
    char_u	*name;

    ಯದಿ (idx == 0)
	intidx = -1;
    ಯದಿ (intidx < 0)
    {
	name = get_function_name(xp, idx);
	ಯದಿ (name != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ name;
    }
    ಪ್ರತ್ಯಾಗಮ get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_internal_func_opt(char_u *name, ಪೂರ್ಣಾಙ್ಕ implemented)
{
    ಪೂರ್ಣಾಙ್ಕ		first = 0;
    ಪೂರ್ಣಾಙ್ಕ		last;
    ಪೂರ್ಣಾಙ್ಕ		cmp;
    ಪೂರ್ಣಾಙ್ಕ		x;

    last = (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(global_functions) - 1;

    // Find the function name in the table. Binary search.
    ಯಾವತ್ (first <= last)
    {
	x = first + ((unsigned)(last - first) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	ಯದಿ (cmp < 0)
	    last = x - 1;
	ಅನ್ಯಥಾ ಯದಿ (cmp > 0)
	    first = x + 1;
	ಅನ್ಯಥಾ ಯದಿ (implemented && global_functions[x].f_func == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ x;
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    ಪೂರ್ಣಾಙ್ಕ
find_internal_func(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ find_internal_func_opt(name, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
has_internal_func(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ find_internal_func_opt(name, TRUE) >= 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
has_internal_func_name(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ find_internal_func_opt(name, FALSE) >= 0;
}

    ವರ್ಣ *
internal_func_name(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
internal_func_check_arg_types(
	type2_T	*types,
	ಪೂರ್ಣಾಙ್ಕ	idx,
	ಪೂರ್ಣಾಙ್ಕ	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    ಯದಿ (!(func_allows_type(idx)))
    {
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < argcount; ++i)
	    ಯದಿ (check_type_is_ಮೌಲ್ಯ(types[i].type_curr) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಧ್ರುವ argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    ಯದಿ (argchecks == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    argcontext_T context;

    context.arg_ಎಣಿಕೆ = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < argcount && argchecks[i] != ರಿಕ್ತ; ++i)
    {
	context.arg_idx = i;
	ಯದಿ (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    ಶೂನ್ಯ
internal_func_get_argcount(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ *argcount, ಪೂರ್ಣಾಙ್ಕ *min_argcount)
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
	ಪೂರ್ಣಾಙ್ಕ	    idx,
	ಪೂರ್ಣಾಙ್ಕ	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = ರಿಕ್ತ;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    ಯದಿ (*decl_type == ರಿಕ್ತ)
	*decl_type = ret;
    current_type_gap = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    ಪೂರ್ಣಾಙ್ಕ
internal_func_is_map(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_internal_func(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ argcount)
{
    funcerror_T	    res;
    ವರ್ಣ	    *name;

    ಯದಿ (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    ಅನ್ಯಥಾ ಯದಿ (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ global_functions[idx].f_argtype & FEARG_MASK;

    name = internal_func_name(idx);
    ಯದಿ (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    ಅನ್ಯಥಾ
	semsg(_(e_not_enough_arguments_for_function_str), name);
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_args_for_type(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ argcount, typval_T *argvars)
{
    ಯದಿ (!func_allows_type(idx))
    {
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < argcount; ++i)
	    ಯದಿ (check_typval_is_ಮೌಲ್ಯ(&argvars[i]) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    funcerror_T
call_internal_func(
	char_u	    *name,
	ಪೂರ್ಣಾಙ್ಕ	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    i = find_internal_func(name);
    ಯದಿ (i < 0)
	ಪ್ರತ್ಯಾಗಮ FCERR_UNKNOWN;
    ಯದಿ (argcount < global_functions[i].f_min_argc)
	ಪ್ರತ್ಯಾಗಮ FCERR_TOOFEW;
    ಯದಿ (argcount > global_functions[i].f_max_argc)
	ಪ್ರತ್ಯಾಗಮ FCERR_TOOMANY;
    ಯದಿ (check_args_for_type(i, argcount, argvars) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    ಯದಿ (in_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    ಪ್ರತ್ಯಾಗಮ FCERR_NONE;
}

    ಶೂನ್ಯ
call_internal_func_by_idx(
	ಪೂರ್ಣಾಙ್ಕ	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    ಯದಿ (in_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_internal_method(
	char_u	    *name,
	ಪೂರ್ಣಾಙ್ಕ	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    ಪೂರ್ಣಾಙ್ಕ		fi;
    typval_T	argv[MAX_FUNC_ARGS + 1];

    fi = find_internal_func(name);
    ಯದಿ (fi < 0)
	ಪ್ರತ್ಯಾಗಮ FCERR_UNKNOWN;
    ಯದಿ ((global_functions[fi].f_argtype & FEARG_MASK) == 0)
	ಪ್ರತ್ಯಾಗಮ FCERR_NOTMETHOD;
    ಯದಿ (argcount + 1 < global_functions[fi].f_min_argc)
	ಪ್ರತ್ಯಾಗಮ FCERR_TOOFEW;
    ಯದಿ (argcount + 1 > global_functions[fi].f_max_argc)
	ಪ್ರತ್ಯಾಗಮ FCERR_TOOMANY;
    ಯದಿ (check_args_for_type(fi, argcount, argvars) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FCERR_OTHER;

    ಯದಿ ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_2)
    {
	ಯದಿ (argcount < 1)
	    ಪ್ರತ್ಯಾಗಮ FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ಅನ್ಯಥಾ ಯದಿ ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_3)
    {
	ಯದಿ (argcount < 2)
	    ಪ್ರತ್ಯಾಗಮ FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ಅನ್ಯಥಾ ಯದಿ ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_4)
    {
	ಯದಿ (argcount < 3)
	    ಪ್ರತ್ಯಾಗಮ FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ಅನ್ಯಥಾ
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    ಯದಿ (check_args_for_type(fi, argcount + 1, argv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    ಪ್ರತ್ಯಾಗಮ FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    ಪೂರ್ಣಾಙ್ಕ
non_zero_arg(typval_T *argvars)
{
    ಪ್ರತ್ಯಾಗಮ ((argvars[0].v_type == VAR_ಸಂಖ್ಯೆ
		&& argvars[0].vval.v_ಸಂಖ್ಯೆ != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_ಸಂಖ್ಯೆ == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != ರಿಕ್ತ
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_and(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ)
					& tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ರಿಕ್ತ);
}

/*
 * "balloon_show()" function
 */
#ifdef FEAT_BEVAL
    ಸ್ಥಿರ ಶೂನ್ಯ
f_balloon_gettext(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    ಯದಿ (balloonEval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (balloonEval->ಸಂದೇಶ == ರಿಕ್ತ)
	rettv->vval.v_string = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	rettv->vval.v_string = vim_strsave(balloonEval->ಸಂದೇಶ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
f_balloon_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (balloonEval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, ರಿಕ್ತ,
		l == ರಿಕ್ತ || l->lv_len == 0 ? ರಿಕ್ತ : l);
    }
    ಅನ್ಯಥಾ
    {
	char_u *mesg;

	ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;

	mesg = tv_get_string_chk(&argvars[0]);
	ಯದಿ (mesg != ರಿಕ್ತ)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? ರಿಕ್ತ : mesg, ರಿಕ್ತ);
    }
}

# ಯದಿ defined(FEAT_BEVAL_TERM)
    ಸ್ಥಿರ ಶೂನ್ಯ
f_balloon_split(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (rettv_list_alloc(rettv) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    char_u *ಸಂದೇಶ;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಸಂದೇಶ = tv_get_string_chk(&argvars[0]);
    ಯದಿ (ಸಂದೇಶ != ರಿಕ್ತ)
    {
	pumitem_T	*array;
	ಪೂರ್ಣಾಙ್ಕ		size = split_message(ಸಂದೇಶ, &array);

	// Skip the first and last item, they are always empty.
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < size - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_text, -1);
	ಯಾವತ್ (size > 0)
	    vim_free(array[--size].pum_text);
	vim_free(array);
    }
}
# endif
#endif

/*
 * "base64_decode(string)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (rettv_blob_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    ಯದಿ (str != ರಿಕ್ತ)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (check_for_blob_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    blob_T *blob = argvars->vval.v_blob;
    ಯದಿ (blob != ರಿಕ್ತ)
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
    ಯದಿ (buf == ರಿಕ್ತ)
	semsg(_(e_invalid_buffer_name_str), tv_get_string(arg));
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;

    ಯದಿ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (strcmp((ಧ್ರುವ ವರ್ಣ *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    ಅನ್ಯಥಾ
    {
	ಯದಿ (bindtextdomain((ಧ್ರುವ ವರ್ಣ *)argvars[0].vval.v_string, (ಧ್ರುವ ವರ್ಣ *)argvars[1].vval.v_string) == ರಿಕ್ತ)
	{
	    do_outofmem_ಸಂದೇಶ((long)0);
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;
	}
    }

    ಪ್ರತ್ಯಾಗಮ;
}

/*
 * "byte2line(byte)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_byte2line(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
#ಅನ್ಯಥಾ
    long	boff = 0;

    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    boff = tv_get_ಸಂಖ್ಯೆ(&argvars[0]) - 1;  // boff gets -1 on type error
    ಯದಿ (boff < 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = ml_find_ಸಾಲು_or_offset(curbuf,
							  (linenr_T)0, &boff);
#endif
}

/*
 * "call(func, arglist [, dict])" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = ರಿಕ್ತ;
    dict_T	*selfdict = ರಿಕ್ತ;
    char_u	*tofree = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_list_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (argvars[1].vval.v_list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    ಅನ್ಯಥಾ
	func = tv_get_string(&argvars[0]);
    ಯದಿ (func == ರಿಕ್ತ || *func == NUL)
	ಪ್ರತ್ಯಾಗಮ;		// type error, empty name or null function

    ಯದಿ (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, ರಿಕ್ತ, FALSE, TFN_INT|TFN_QUIET);
	ಯದಿ (tofree == ರಿಕ್ತ)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_args(tofree, STRLEN(tofree),
									&p);
	    ಯದಿ (s != ರಿಕ್ತ)
	    {
		vim_free(tofree);
		tofree = s;
	    }
	}
	func = tofree;
    }

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಯದಿ (check_for_dict_arg(argvars, 2) == FAIL)
	    ಗಚ್ಛ done;

	selfdict = argvars[2].vval.v_dict;
    }

    (ಶೂನ್ಯ)func_call(func, &argvars[1], partial, selfdict, rettv);

done:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_changenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (has_mbyte)
    {
	ಪೂರ್ಣಾಙ್ಕ	utf8 = 0;

	ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[1], ರಿಕ್ತ);

	ಯದಿ (utf8)
	    rettv->vval.v_ಸಂಖ್ಯೆ = utf_ptr2char(tv_get_string(&argvars[0]));
	ಅನ್ಯಥಾ
	    rettv->vval.v_ಸಂಖ್ಯೆ = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_col(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ charcol)
{
    colnr_T	col = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    ಪೂರ್ಣಾಙ್ಕ		winchanged = FALSE;

    ಯದಿ (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]), &tp);
	ಯದಿ (wp == ರಿಕ್ತ || tp == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    ಪ್ರತ್ಯಾಗಮ;

	check_cursor();
	winchanged = TRUE;
    }

    ಪೂರ್ಣಾಙ್ಕ fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    ಯದಿ (fp != ರಿಕ್ತ && fnum == curbuf->b_fnum)
    {
	ಯದಿ (fp->col == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    ಯದಿ (fp->lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		col = ml_get_len(fp->lnum) + 1;
	    ಅನ್ಯಥಾ
		col = MAXCOL;
	}
	ಅನ್ಯಥಾ
	{
	    col = fp->col + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    ಯದಿ (virtual_active() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		ಯದಿ (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    ಪೂರ್ಣಾಙ್ಕ		l;

		    ಯದಿ (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			col += l;
		}
	    }
	}
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = col;

    ಯದಿ (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, TRUE);
}

    win_T *
get_optional_window(typval_T *argvars, ಪೂರ್ಣಾಙ್ಕ idx)
{
    win_T   *win = curwin;

    ಯದಿ (argvars[idx].v_type == VAR_UNKNOWN)
	ಪ್ರತ್ಯಾಗಮ curwin;

    win = find_win_by_nr_or_id(&argvars[idx]);
    ಯದಿ (win == ರಿಕ್ತ)
    {
	emsg(_(e_invalid_window_ಸಂಖ್ಯೆ));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ win;
}

/*
 * "col(string)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_col(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_confirm(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*message;
    char_u	*buttons = ರಿಕ್ತ;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		def = 1;
    ಪೂರ್ಣಾಙ್ಕ		type = VIM_GENERIC;
    char_u	*typestr;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	ಪ್ರತ್ಯಾಗಮ;

    message = tv_get_string_chk(&argvars[0]);
    ಯದಿ (message == ರಿಕ್ತ)
	error = TRUE;
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	ಯದಿ (buttons == ರಿಕ್ತ)
	    error = TRUE;
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	    ಯದಿ (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		ಯದಿ (typestr == ರಿಕ್ತ)
		    error = TRUE;
		ಅನ್ಯಥಾ
		{
		    ವಿಕಲ್ಪ (TOUPPER_ASC(*typestr))
		    {
			ಪಕ್ಷ 'E': type = VIM_ERROR; ಭಙ್ಗ;
			ಪಕ್ಷ 'Q': type = VIM_QUESTION; ಭಙ್ಗ;
			ಪಕ್ಷ 'I': type = VIM_INFO; ಭಙ್ಗ;
			ಪಕ್ಷ 'W': type = VIM_WARNING; ಭಙ್ಗ;
			ಪಕ್ಷ 'G': type = VIM_GENERIC; ಭಙ್ಗ;
		    }
		}
	    }
	}
    }

    ಯದಿ (buttons == ರಿಕ್ತ || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    ಯದಿ (!error)
	rettv->vval.v_ಸಂಖ್ಯೆ = do_dialog(type, ರಿಕ್ತ, message, buttons,
							    def, ರಿಕ್ತ, FALSE);
#endif
}

/*
 * "copy()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_copy(typval_T *argvars, typval_T *rettv)
{
    item_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_cursorpos(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ charcol)
{
    long	lnum, col;
    long	coladd = 0;
    ಪೂರ್ಣಾಙ್ಕ		set_curswant = TRUE;

    ಯದಿ (in_vim9script()
	    && (check_for_string_or_ಸಂಖ್ಯೆ_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಯದಿ (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	ಯದಿ (list2fpos(argvars, &pos, ರಿಕ್ತ, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_argument));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	lnum = pos.lnum;
	col = pos.col;
	coladd = pos.coladd;
	ಯದಿ (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    set_curswant = FALSE;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ ((argvars[0].v_type == VAR_ಸಂಖ್ಯೆ
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_ಸಂಖ್ಯೆ
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	ಯದಿ (lnum < 0)
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
	ಅನ್ಯಥಾ ಯದಿ (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	col = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ರಿಕ್ತ);
	ಯದಿ (charcol)
	    col = buf_charidx_to_byteidx(curbuf, lnum, col) + 1;
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ
    {
	emsg(_(e_invalid_argument));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (lnum < 0 || col < 0 || coladd < 0)
	ಪ್ರತ್ಯಾಗಮ;		// type error; errmsg already given
    ಯದಿ (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    ಯದಿ (col != MAXCOL && --col < 0)
	col = 0;
    curwin->w_cursor.col = col;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    ಯದಿ (has_mbyte)
	mb_adjust_cursor();

    curwin->w_set_curswant = set_curswant;
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
    set_cursorpos(argvars, rettv, FALSE);
}

#ifdef MSWIN
/*
 * "debugbreak()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		pid;

    rettv->vval.v_ಸಂಖ್ಯೆ = FAIL;
    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    pid = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    ಯದಿ (pid == 0)
    {
	emsg(_(e_invalid_argument));
	ಪ್ರತ್ಯಾಗಮ;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    ಯದಿ (hProcess == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_ಸಂಖ್ಯೆ = OK;
}
#endif

/*
 * "deepcopy()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    ಯದಿ (check_for_opt_bool_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], ರಿಕ್ತ);

    item_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_did_filetype(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_echoraw(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *str;

    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    str = tv_get_string_chk(&argvars[0]);
    ಯದಿ (str != ರಿಕ್ತ && *str != NUL)
    {
	out_str(str);
	out_flush();
    }
}

/*
 * "empty({expr})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_empty(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		n = FALSE;

    ವಿಕಲ್ಪ (argvars[0].v_type)
    {
	ಪಕ್ಷ VAR_STRING:
	ಪಕ್ಷ VAR_FUNC:
	    n = argvars[0].vval.v_string == ರಿಕ್ತ
					  || *argvars[0].vval.v_string == NUL;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	    n = argvars[0].vval.v_ಸಂಖ್ಯೆ == 0;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:
	    n = argvars[0].vval.v_list == ರಿಕ್ತ
					|| argvars[0].vval.v_list->lv_len == 0;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    n = argvars[0].vval.v_dict == ರಿಕ್ತ
			|| argvars[0].vval.v_dict->dv_hashtab.ht_used == 0;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	    n = argvars[0].vval.v_ಸಂಖ್ಯೆ != VVAL_TRUE;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CLASS:
	    n = argvars[0].vval.v_class != ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_OBJECT:
	    n = object_empty(argvars[0].vval.v_object);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_BLOB:
	    n = argvars[0].vval.v_blob == ರಿಕ್ತ
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_job == ರಿಕ್ತ
			   || argvars[0].vval.v_job->jv_status != JOB_STARTED;
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_channel == ರಿಕ್ತ
			       || !channel_is_open(argvars[0].vval.v_channel);
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == ರಿಕ್ತ
		|| argvars[0].vval.v_typealias->ta_name == ರಿಕ್ತ
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	ಪಕ್ಷ VAR_INSTR:
	    internal_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    ಭಙ್ಗ;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

/*
 * "environ()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_environ(typval_T *argvars UNUSED, typval_T *rettv)
{
#ಯದಿ !defined(AMIGA)
    ಪೂರ್ಣಾಙ್ಕ			i = 0;
    char_u		*entry, *ಮೌಲ್ಯ;
# ಯದಿ defined (MSWIN)
#  ಯದಿ !defined(_UCRT)
    ಬಾಹ್ಯ wchar_t	**_wenviron;
#  endif
# ಅನ್ಯಥಾ
    ಬಾಹ್ಯ ವರ್ಣ		**environ;
# endif

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

# ifdef MSWIN
    ಯದಿ (*_wenviron == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
# ಅನ್ಯಥಾ
    ಯದಿ (*environ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
# endif

    ಪ್ರತಿ (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	ಯದಿ ((p = (short_u *)_wenviron[i]) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	entry = utf16_to_enc(p, ರಿಕ್ತ);
# ಅನ್ಯಥಾ
	ಯದಿ ((entry = (char_u *)environ[i]) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	entry = vim_strsave(entry);
# endif
	ಯದಿ (entry == ರಿಕ್ತ) // out of memory
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ ((ಮೌಲ್ಯ = vim_strchr(entry, '=')) == ರಿಕ್ತ)
	{
	    vim_free(entry);
	    ಅನುವರ್ತ;
	}
	*ಮೌಲ್ಯ++ = NUL;
	dict_add_string(rettv->vval.v_dict, (ವರ್ಣ *)entry, ಮೌಲ್ಯ);
	vim_free(entry);
    }
#endif
}

/*
 * "err_teapot()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_err_teapot(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ಯದಿ (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    ಯದಿ (*skipwhite(s) == NUL)
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಪೂರ್ಣಾಙ್ಕ err = FALSE;
	ಪೂರ್ಣಾಙ್ಕ do_503 = eval_ಅಭಿವ್ಯಕ್ತಿ_to_bool(&argvars[0], &err);
	ಯದಿ (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_available));
	    ಪ್ರತ್ಯಾಗಮ;
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

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

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

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    s = tv_get_string_chk(&argvars[0]);
    ಯದಿ (s != ರಿಕ್ತ)
	s = skipwhite(s);

    p = s;
    ಯದಿ (s == ರಿಕ್ತ || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	ಯದಿ (p != ರಿಕ್ತ && !aborting())
	    semsg(_(e_invalid_expression_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_eventhandler(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = vgetc_busy || ಒಳಹರಿವು_busy;
}

ಸ್ಥಿರ garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    ಶೂನ್ಯ
execute_redir_str(char_u *ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ_len)
{
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (ಮೌಲ್ಯ_len == -1)
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಮೌಲ್ಯ);	// Append the entire string
    ಅನ್ಯಥಾ
	len = ಮೌಲ್ಯ_len;		// Append only "value_len" characters
    ಯದಿ (ga_grow(&redir_execute_ga, len) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    mch_memmove((ವರ್ಣ *)redir_execute_ga.ga_data
	    + redir_execute_ga.ga_len, ಮೌಲ್ಯ, len);
    redir_execute_ga.ga_len += len;
}

#ಯದಿ defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    ಸ್ಥಿರ char_u *
get_str_ಸಾಲು(
    ಪೂರ್ಣಾಙ್ಕ	    c UNUSED,
    ಶೂನ್ಯ    *cookie,
    ಪೂರ್ಣಾಙ್ಕ	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    char_u	*start = *(char_u **)cookie;
    char_u	*ಸಾಲು;
    char_u	*p;

    p = start;
    ಯದಿ (p == ರಿಕ್ತ || *p == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    p = vim_strchr(p, '\n');
    ಯದಿ (p == ರಿಕ್ತ)
	ಸಾಲು = vim_strsave(start);
    ಅನ್ಯಥಾ
    {
	ಸಾಲು = vim_strnsave(start, p - start);
	p++;
    }

    *(char_u **)cookie = p;
    ಪ್ರತ್ಯಾಗಮ ಸಾಲು;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    ಶೂನ್ಯ
execute_cmds_from_string(char_u *str)
{
    do_cmdline(ರಿಕ್ತ, get_str_ಸಾಲು, (ಶೂನ್ಯ *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#endif

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_ಸಾಲು(
    ಪೂರ್ಣಾಙ್ಕ	    c UNUSED,
    ಶೂನ್ಯ    *cookie,
    ಪೂರ್ಣಾಙ್ಕ	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *item = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    ಯದಿ (item == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    s = tv_get_string_buf_chk(&item->li_tv, buf);
    *p = item->li_next;
    ಪ್ರತ್ಯಾಗಮ s == ರಿಕ್ತ ? ರಿಕ್ತ : vim_strsave(s);
}

/*
 * "execute()" function
 */
    ಶೂನ್ಯ
execute_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ arg_off)
{
    char_u	*cmd = ರಿಕ್ತ;
    list_T	*list = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		save_ಸಂದೇಶ_silent = ಸಂದೇಶ_silent;
    ಪೂರ್ಣಾಙ್ಕ		save_emsg_silent = emsg_silent;
    ಪೂರ್ಣಾಙ್ಕ		save_emsg_noredir = emsg_noredir;
    ಪೂರ್ಣಾಙ್ಕ		save_redir_execute = redir_execute;
    ಪೂರ್ಣಾಙ್ಕ		save_redir_off = redir_off;
    garray_T	save_ga;
    ಪೂರ್ಣಾಙ್ಕ		save_ಸಂದೇಶ_col = ಸಂದೇಶ_col;
    ಪೂರ್ಣಾಙ್ಕ		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    ಪೂರ್ಣಾಙ್ಕ		echo_output = FALSE;

    rettv->vval.v_string = ರಿಕ್ತ;
    rettv->v_type = VAR_STRING;

    ಯದಿ (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	ಯದಿ (list == ರಿಕ್ತ || list->lv_len == 0)
	    // empty list, no commands, empty output
	    ಪ್ರತ್ಯಾಗಮ;
	++list->lv_refcount;
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[arg_off].v_type == VAR_JOB
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	ಯದಿ (cmd == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      in_vim9script());

	ಯದಿ (s == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (*s == NUL)
	    echo_output = TRUE;
	ಯದಿ (STRNCMP(s, "silent", 6) == 0)
	    ++ಸಂದೇಶ_silent;
	ಯದಿ (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    ಅನ್ಯಥಾ
	++ಸಂದೇಶ_silent;

    ಯದಿ (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, ಪರಿಮಾಣ(ವರ್ಣ), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    ಯದಿ (!echo_output)
	ಸಂದೇಶ_col = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    ಯದಿ (cmd != ರಿಕ್ತ)
	do_cmdline_cmd(cmd);
    ಅನ್ಯಥಾ
    {
	listitem_T	*item;

	CHECK_LIST_MATERIALIZE(list);
	item = list->lv_first;
	do_cmdline(ರಿಕ್ತ, get_list_ಸಾಲು, (ಶೂನ್ಯ *)&item,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    ಯದಿ (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((ವರ್ಣ *)redir_execute_ga.ga_data)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_data;
    }
    ಅನ್ಯಥಾ
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = ರಿಕ್ತ;
    }
    ಸಂದೇಶ_silent = save_ಸಂದೇಶ_silent;
    emsg_silent = save_emsg_silent;
    emsg_noredir = save_emsg_noredir;

    redir_execute = save_redir_execute;
    ಯದಿ (redir_execute)
	redir_execute_ga = save_ga;
    redir_off = save_redir_off;

    // "silent reg" or "silent echo x" leaves msg_col somewhere in the line.
    ಯದಿ (echo_output)
	// When not working silently: put it in column zero.  A following
	// "echon" will overwrite the message, unavoidably.
	ಸಂದೇಶ_col = 0;
    ಅನ್ಯಥಾ
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	ಸಂದೇಶ_col = save_ಸಂದೇಶ_col;
}

/*
 * "execute()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_execute(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    ಶೂನ್ಯ
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		n = FALSE;

    ಯದಿ (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    p = tv_get_string(&argvars[0]);
    ಯದಿ (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	ಯದಿ (mch_getenv(p + 1) != ರಿಕ್ತ)
	    n = TRUE;
	ಅನ್ಯಥಾ
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    ಯದಿ (p != ರಿಕ್ತ && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, ರಿಕ್ತ, TRUE) == OK);
	ಯದಿ (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    ಅನ್ಯಥಾ ಯದಿ (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    ಅನ್ಯಥಾ ಯದಿ (*p == '?')			// internal function only
    {
	n = has_internal_func_name(p + 1);
    }
    ಅನ್ಯಥಾ ಯದಿ (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    ಅನ್ಯಥಾ ಯದಿ (*p == '#')
    {
	ಯದಿ (p[1] == '#')
	    n = autocmd_supported(p + 2);
	ಅನ್ಯಥಾ
	    n = au_exists(p + 1);
    }
    ಅನ್ಯಥಾ				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
f_exists_compiled(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    emsg(_(e_exists_compiled_can_only_be_used_in_def_function));
}

/*
 * "expand()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    size_t	len;
    ಪೂರ್ಣಾಙ್ಕ		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    char_u	*result;
#ifdef BACKSLASH_IN_FILENAME
    char_u	*p_csl_save = p_csl;
#endif

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

#ifdef BACKSLASH_IN_FILENAME
    // avoid using 'completeslash' here
    p_csl = empty_option;
#endif

    rettv->v_type = VAR_STRING;
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_set(rettv, ರಿಕ್ತ);

    s = tv_get_string(&argvars[0]);
    ಯದಿ (*s == '%' || *s == '#' || *s == '<')
    {
	ವರ್ಣ	*errormsg = ರಿಕ್ತ;

	ಯದಿ (p_verbose == 0)
	    ++emsg_off;
	result = eval_vars(s, s, &len, ರಿಕ್ತ, &errormsg, ರಿಕ್ತ, FALSE);
	ಯದಿ (p_verbose == 0)
	    --emsg_off;
	ಅನ್ಯಥಾ ಯದಿ (errormsg != ರಿಕ್ತ)
	    emsg(errormsg);
	ಯದಿ (rettv->v_type == VAR_LIST)
	{
	    ಯದಿ (rettv_list_alloc(rettv) == OK && result != ರಿಕ್ತ)
		list_append_string(rettv->vval.v_list, result, -1);
	    vim_free(result);
	}
	ಅನ್ಯಥಾ
	    rettv->vval.v_string = result;
    }
    ಅನ್ಯಥಾ
    {
	// When the optional second argument is non-zero, don't remove matches
	// for 'wildignore' and don't put matches for 'suffixes' at the end.
	ಯದಿ (argvars[1].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	ಯದಿ (!error)
	{
	    ExpandInit(&xpc);
	    xpc.xp_context = EXPAND_FILES;
	    ಯದಿ (p_wic)
		options += WILD_ICASE;
	    ಯದಿ (rettv->v_type == VAR_STRING)
		rettv->vval.v_string = ExpandOne(&xpc, s, ರಿಕ್ತ,
							   options, WILD_ALL);
	    ಅನ್ಯಥಾ ಯದಿ (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, ರಿಕ್ತ, options, WILD_ALL_KEEP);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	ಅನ್ಯಥಾ
	    rettv->vval.v_string = ರಿಕ್ತ;
    }
#ifdef BACKSLASH_IN_FILENAME
    p_csl = p_csl_save;
#endif
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
    ವರ್ಣ	*errormsg = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		emsgoff = TRUE;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_FIELD(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = ರಿಕ್ತ;
    eap.cmdidx = CMD_USER;

    ಯದಿ (emsgoff)
	++emsg_off;
    ಯದಿ (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	ಯದಿ (!emsgoff && errormsg != ರಿಕ್ತ && *errormsg != NUL)
	    emsg(errormsg);
    ಯದಿ (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_feedkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		remap = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		ಸೇರಿಸು = FALSE;
    char_u	*ಕೀಲಿಗಳು, *flags;
    char_u	nbuf[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		typed = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		execute = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		context = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		dangerous = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		lowlevel = FALSE;

    // This is not allowed in the sandbox.  If the commands would still be
    // executed in the sandbox it would be OK, but it probably happens later,
    // when "sandbox" is no longer set.
    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಕೀಲಿಗಳು = tv_get_string(&argvars[0]);

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	ಪ್ರತಿ ( ; *flags != NUL; ++flags)
	{
	    ವಿಕಲ್ಪ (*flags)
	    {
		ಪಕ್ಷ 'n': remap = FALSE; ಭಙ್ಗ;
		ಪಕ್ಷ 'm': remap = TRUE; ಭಙ್ಗ;
		ಪಕ್ಷ 't': typed = TRUE; ಭಙ್ಗ;
		ಪಕ್ಷ 'i': ಸೇರಿಸು = TRUE; ಭಙ್ಗ;
		ಪಕ್ಷ 'x': execute = TRUE; ಭಙ್ಗ;
		ಪಕ್ಷ 'c': context = TRUE; ಭಙ್ಗ;
		ಪಕ್ಷ '!': dangerous = TRUE; ಭಙ್ಗ;
		ಪಕ್ಷ 'L': lowlevel = TRUE; ಭಙ್ಗ;
	    }
	}
    }

    ಯದಿ (*ಕೀಲಿಗಳು != NUL || execute)
    {
	ಯದಿ (lowlevel
#ifdef FEAT_VTP
		&& (!is_term_win32()
		    || (ಕೀಲಿಗಳು[0] == 3 && ctrl_c_interrupts && typed))
#endif
	   )
	{
#ifdef USE_ಒಳಹರಿವು_BUF
	    ch_log(ರಿಕ್ತ, "feedkeys() lowlevel: %s", ಕೀಲಿಗಳು);

	    ಪೂರ್ಣಾಙ್ಕ len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಕೀಲಿಗಳು);
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		ಯದಿ (ಕೀಲಿಗಳು[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		add_to_ಒಳಹರಿವು_buf(ಕೀಲಿಗಳು + idx, 1);
	    }
#ಅನ್ಯಥಾ
	    emsg(_(e_lowlevel_ಒಳಹರಿವು_not_supported));
#endif
	}
	ಅನ್ಯಥಾ
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *ಕೀಲಿಗಳು_esc = vim_strsave_escape_csi(ಕೀಲಿಗಳು);
	    ಯದಿ (ಕೀಲಿಗಳು_esc == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;

	    ch_log(ರಿಕ್ತ, "feedkeys(%s): %s", typed ? "typed" : "", ಕೀಲಿಗಳು);

	    ins_typebuf(ಕೀಲಿಗಳು_esc, (remap ? REMAP_YES : REMAP_NONE),
				   ಸೇರಿಸು ? 0 : typebuf.tb_len, !typed, FALSE);
	    ಯದಿ (vgetc_busy
#ifdef FEAT_TIMERS
		    || timer_busy
#endif
		    || ಒಳಹರಿವು_busy)
		typebuf_was_filled = TRUE;

	    vim_free(ಕೀಲಿಗಳು_esc);
	}

	ಯದಿ (execute)
	{
	    ಪೂರ್ಣಾಙ್ಕ		save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    ಸಂದೇಶ_scroll = FALSE;

	    ch_log(ರಿಕ್ತ, "feedkeys() executing");

	    ಯದಿ (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_version = 0;
	    }

	    ಯದಿ (!dangerous)
	    {
		++ex_normal_busy;
		++in_feedkeys;
	    }
	    ++allow_osc_ಕೀಲಿ;
	    exec_normal(TRUE, lowlevel, TRUE);
	    --allow_osc_ಕೀಲಿ;
	    ಯದಿ (!dangerous)
	    {
		--ex_normal_busy;
		--in_feedkeys;
	    }

	    ಸಂದೇಶ_scroll |= save_ಸಂದೇಶ_scroll;

	    ಯದಿ (context)
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
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	gui_mch_set_foreground();
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif
#ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_set_foreground();
#endif
}

/*
 * "function()" function
 * "funcref()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
common_function(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ is_funcref)
{
    char_u	*s;
    char_u	*name;
    ಪೂರ್ಣಾಙ್ಕ		use_string = FALSE;
    partial_T   *arg_pt = ರಿಕ್ತ;
    char_u	*trans_name = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		is_global = FALSE;
    char_u	*start_bracket = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != ರಿಕ್ತ)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    ಅನ್ಯಥಾ
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    ಯದಿ (s == ರಿಕ್ತ)
    {
	semsg(_(e_invalid_argument_str), "NULL");
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == ರಿಕ್ತ) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, ರಿಕ್ತ);
	ಯದಿ (*name == '<')
	{
	    // generic function
	    start_bracket = name;
	    ಯದಿ (skip_generic_func_type_args(&name) == FAIL)
		ಗಚ್ಛ theend;
	}
	ಯದಿ (*name != NUL)
	    s = ರಿಕ್ತ;
    }

    ಯದಿ (s == ರಿಕ್ತ || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == ರಿಕ್ತ))
	semsg(_(e_invalid_argument_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    ಅನ್ಯಥಾ ಯದಿ (trans_name != ರಿಕ್ತ && (is_funcref
			 ? find_func(trans_name, is_global) == ರಿಕ್ತ
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	dict_idx = 0;
	ಪೂರ್ಣಾಙ್ಕ	arg_idx = 0;
	list_T	*list = ರಿಕ್ತ;

	ಯದಿ (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	ಅನ್ಯಥಾ ಯದಿ (trans_name != ರಿಕ್ತ && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	ಅನ್ಯಥಾ
	    name = vim_strsave(s);

	ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    ಅನ್ಯಥಾ
		// function(name, [args])
		arg_idx = 1;
	    ಯದಿ (dict_idx > 0)
	    {
		ಯದಿ (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    ಗಚ್ಛ theend;
		}
		ಯದಿ (argvars[dict_idx].vval.v_dict == ರಿಕ್ತ)
		    dict_idx = 0;
	    }
	    ಯದಿ (arg_idx > 0)
	    {
		ಯದಿ (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_second_argument_of_function_must_be_list_or_dict));
		    vim_free(name);
		    ಗಚ್ಛ theend;
		}
		list = argvars[arg_idx].vval.v_list;
		ಯದಿ (list == ರಿಕ್ತ || list->lv_len == 0)
		    arg_idx = 0;
		ಅನ್ಯಥಾ ಯದಿ (list->lv_len > MAX_FUNC_ARGS)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    ಗಚ್ಛ theend;
		}
	    }
	}
	ಯದಿ (dict_idx > 0 || arg_idx > 0 || arg_pt != ರಿಕ್ತ || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    ಯದಿ (pt == ರಿಕ್ತ)
		vim_free(name);
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (arg_idx > 0 || (arg_pt != ರಿಕ್ತ && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    ಪೂರ್ಣಾಙ್ಕ		i = 0;
		    ಪೂರ್ಣಾಙ್ಕ		arg_len = 0;
		    ಪೂರ್ಣಾಙ್ಕ		lv_len = 0;

		    ಯದಿ (arg_pt != ರಿಕ್ತ)
			arg_len = arg_pt->pt_argc;
		    ಯದಿ (list != ರಿಕ್ತ)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    ಯದಿ (pt->pt_argv == ರಿಕ್ತ)
		    {
			vim_free(pt);
			vim_free(name);
			ಗಚ್ಛ theend;
		    }
		    ಪ್ರತಿ (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    ಯದಿ (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
			    copy_tv(&li->li_tv, &pt->pt_argv[i++]);
		    }
		}

		// For "function(dict.func, [], dict)" and "func" is a partial
		// use "dict".  That is backwards compatible.
		ಯದಿ (dict_idx > 0)
		{
		    // The dict is bound explicitly, pt_auto is FALSE.
		    pt->pt_dict = argvars[dict_idx].vval.v_dict;
		    ++pt->pt_dict->dv_refcount;
		}
		ಅನ್ಯಥಾ ಯದಿ (arg_pt != ರಿಕ್ತ)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    ಯದಿ (pt->pt_dict != ರಿಕ್ತ)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    ಯದಿ (pt->pt_obj != ರಿಕ್ತ)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		ಯದಿ (arg_pt != ರಿಕ್ತ && arg_pt->pt_func != ರಿಕ್ತ)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		ಅನ್ಯಥಾ ಯದಿ (is_funcref)
		{
		    pt->pt_func = find_func(trans_name, is_global);
		    ಯದಿ (IS_GENERIC_FUNC(pt->pt_func) && start_bracket != ರಿಕ್ತ)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&start_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		ಅನ್ಯಥಾ
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		ಯದಿ (arg_pt != ರಿಕ್ತ)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	ಅನ್ಯಥಾ
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    ಯದಿ (start_bracket == ರಿಕ್ತ)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    ಅನ್ಯಥಾ
	    {
		// generic function
		size_t len = STRLEN(name) + STRLEN(start_bracket);
		rettv->vval.v_string = alloc(len + 1);
		ಯದಿ (rettv->vval.v_string != ರಿಕ್ತ)
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
f_garbagecollect(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
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
    typval_T	*tv = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		what_is_dict = FALSE;

    ಯದಿ (argvars[0].v_type == VAR_BLOB)
    {
	ಪೂರ್ಣಾಙ್ಕ error = FALSE;
	ಪೂರ್ಣಾಙ್ಕ idx = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);

	ಯದಿ (!error)
	{
	    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	    ಯದಿ (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    ಯದಿ (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_ಸಂಖ್ಯೆ = -1;
	    ಅನ್ಯಥಾ
	    {
		rettv->vval.v_ಸಂಖ್ಯೆ = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_LIST)
    {
	ಯದಿ ((l = argvars[0].vval.v_list) != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

	    li = list_find(l, (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error));
	    ಯದಿ (!error && li != ರಿಕ್ತ)
		tv = &li->li_tv;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_TUPLE)
    {
	ಯದಿ ((tuple = argvars[0].vval.v_tuple) != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
	    long	idx;

	    idx = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
	    ಯದಿ (!error)
		tv = tuple_find(tuple, idx);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_DICT)
    {
	ಯದಿ ((d = argvars[0].vval.v_dict) != ರಿಕ್ತ)
	{
	    di = dict_find(d, tv_get_string(&argvars[1]), -1);
	    ಯದಿ (di != ರಿಕ್ತ)
		tv = &di->di_tv;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	ಯದಿ (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	ಅನ್ಯಥಾ
	{
	    CLEAR_FIELD(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	ಯದಿ (pt != ರಿಕ್ತ)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    ಯದಿ (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		ಯದಿ (name == ರಿಕ್ತ)
		    rettv->vval.v_string = ರಿಕ್ತ;
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    ಯದಿ (*what == 'n' && pt->pt_name == ರಿಕ್ತ
							&& pt->pt_func != ರಿಕ್ತ)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		ಯದಿ (pt->pt_dict != ರಿಕ್ತ)
		    rettv_dict_set(rettv, pt->pt_dict);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		ಯದಿ (rettv_list_alloc(rettv) == OK)
		{
		    ಪೂರ್ಣಾಙ್ಕ i;

		    ಪ್ರತಿ (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(what, "arity") == 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ required = 0, optional = 0, varargs = FALSE;
		char_u *name = partial_name(pt);

		get_func_arity(name, &required, &optional, &varargs);

		rettv->v_type = VAR_DICT;
		ಯದಿ (rettv_dict_alloc(rettv) == OK)
		{
		    dict_T *dict = rettv->vval.v_dict;

		    // Take into account the arguments of the partial, if any.
		    // Note that it is possible to supply more arguments than the function
		    // accepts.
		    ಯದಿ (pt->pt_argc >= required + optional)
			required = optional = 0;
		    ಅನ್ಯಥಾ ಯದಿ (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    ಅನ್ಯಥಾ
			required -= pt->pt_argc;

		    dict_add_ಸಂಖ್ಯೆ(dict, "required", required);
		    dict_add_ಸಂಖ್ಯೆ(dict, "optional", optional);
		    dict_add_bool(dict, "varargs", varargs);
		}
	    }
	    ಅನ್ಯಥಾ
		semsg(_(e_invalid_argument_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    ಯದಿ (!what_is_dict)
		ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಅನ್ಯಥಾ
	semsg(_(e_argument_of_str_must_be_list_tuple_dictionary_or_blob), "get()");

    ಯದಿ (tv == ರಿಕ್ತ)
    {
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    ಅನ್ಯಥಾ
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
f_getbgcolor(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	ಯದಿ (gui.in_use)
	    try_hl = true;
# endif
# ifdef FEAT_TERMGUICOLORS
	ಯದಿ (p_tgc)
	    try_hl = true;
# endif
	ಯದಿ (try_hl)
	{
	    guicolor_T	fg, bg;
	    ಪೂರ್ಣಾಙ್ಕ		hl_id = syn_name2id((char_u *)"Normal");

	    ಯದಿ (hl_id > 0)
	    {
		(ಶೂನ್ಯ)syn_id2colors(hl_id, &fg, &bg);
		ಯದಿ (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    ಪ್ರತ್ಯಾಗಮ;
		}
	    }
	}
    }
#endif
#ifdef FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	ಯದಿ (term_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)r);
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)g);
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#endif
}

/*
 * "getcellpixels()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getcellpixels(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

#ಯದಿ defined(FEAT_GUI)
    ಯದಿ (gui.in_use)
    {
	// success pixel size and no gui.
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)gui.char_width);
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)gui.char_height);
    }
    ಅನ್ಯಥಾ
#endif
    {
#ಯದಿ defined(UNIX) || defined(MSWIN)
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)cell_width);
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)cell_height);
#ಅನ್ಯಥಾ
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)-1);
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)-1);
#endif
    }

}

/*
 * "getchangelist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		i;
    list_T	*l;
    dict_T	*d;
    ಪೂರ್ಣಾಙ್ಕ		changelistindex;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    ಅನ್ಯಥಾ
	buf = tv_get_buf_from_arg(&argvars[0]);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    l = list_alloc();
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	ಪ್ರತ್ಯಾಗಮ;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    ಯದಿ (buf == curwin->w_buffer)
    {
	changelistindex = curwin->w_changelistidx;
    }
    ಅನ್ಯಥಾ
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    ಯದಿ (wip->wi_win == curwin)
		ಭಙ್ಗ;
	changelistindex = wip != ರಿಕ್ತ ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)changelistindex);

    ಪ್ರತಿ (i = 0; i < buf->b_changelistlen; ++i)
    {
	ಯದಿ (buf->b_changelist[i].lnum == 0)
	    ಅನುವರ್ತ;
	ಯದಿ ((d = dict_alloc()) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	dict_add_ಸಂಖ್ಯೆ(d, "lnum", (long)buf->b_changelist[i].lnum);
	dict_add_ಸಂಖ್ಯೆ(d, "col", (long)buf->b_changelist[i].col);
	dict_add_ಸಂಖ್ಯೆ(d, "coladd", (long)buf->b_changelist[i].coladd);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    ಪೂರ್ಣಾಙ್ಕ		getcurpos,
    ಪೂರ್ಣಾಙ್ಕ		charcol)
{
    pos_T	*fp = ರಿಕ್ತ;
    pos_T	pos;
    win_T	*wp = curwin;
    list_T	*l;
    ಪೂರ್ಣಾಙ್ಕ		fnum = -1;

    ಯದಿ (rettv_list_alloc(rettv) == OK)
    {
	l = rettv->vval.v_list;
	ಯದಿ (getcurpos)
	{
	    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
	    {
		wp = find_win_by_nr_or_id(&argvars[0]);
		ಯದಿ (wp != ರಿಕ್ತ)
		    fp = &wp->w_cursor;
	    }
	    ಅನ್ಯಥಾ
		fp = &curwin->w_cursor;
	    ಯದಿ (fp != ರಿಕ್ತ && charcol)
	    {
		pos = *fp;
		pos.col =
		    buf_byteidx_to_charidx(wp->w_buffer, pos.lnum, pos.col);
		fp = &pos;
	    }
	}
	ಅನ್ಯಥಾ
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	ಯದಿ (fnum != -1)
	    list_append_ಸಂಖ್ಯೆ(l, (varnumber_T)fnum);
	ಅನ್ಯಥಾ
	    list_append_ಸಂಖ್ಯೆ(l, (varnumber_T)0);
	list_append_ಸಂಖ್ಯೆ(l, (fp != ರಿಕ್ತ) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_ಸಂಖ್ಯೆ(l, (fp != ರಿಕ್ತ)
		     ? (varnumber_T)(fp->col == MAXCOL ? MAXCOL : fp->col + 1)
							    : (varnumber_T)0);
	list_append_ಸಂಖ್ಯೆ(l, (fp != ರಿಕ್ತ) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	ಯದಿ (getcurpos)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    save_set_curswant = curwin->w_set_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    ಯದಿ (wp == curwin)
		update_curswant();
	    list_append_ಸಂಖ್ಯೆ(l, wp == ರಿಕ್ತ ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    ಯದಿ (wp == curwin && save_set_curswant)
	    {
		curwin->w_set_curswant = save_set_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;
}

/*
 * "getcharpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getcharpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getcharsearch(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಯದಿ (rettv_dict_alloc(rettv) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_string(dict, "char", last_csearch());
    dict_add_ಸಂಖ್ಯೆ(dict, "forward", last_csearch_forward());
    dict_add_ಸಂಖ್ಯೆ(dict, "until", last_csearch_until());
}

/*
 * "getenv()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getenv(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ	    mustfree = FALSE;
    char_u  *p;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    ಯದಿ (p == ರಿಕ್ತ)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getfontname(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	GuiFont font;
	char_u	*name = ರಿಕ್ತ;

	ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	ಅನ್ಯಥಾ
	{
	    name = tv_get_string(&argvars[0]);
	    ಯದಿ (STRCMP(name, "*") == 0)	    // don't use font dialog
		ಪ್ರತ್ಯಾಗಮ;
	    font = gui_mch_get_font(name, FALSE);
	    ಯದಿ (font == NOFONT)
		ಪ್ರತ್ಯಾಗಮ;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#endif
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
ಸ್ಥಿರ ಸಂರಚನಾ
{
    argcheck_T	func;
    ವರ್ಣ	*types;	    // separated by commas
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
    ಸ್ಥಿರ list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    ವರ್ಣ	*types = "any";

    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(argcheck_types); ++i)
	ಯದಿ (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    ಭಙ್ಗ;
	}
    ಪ್ರತಿ (ವರ್ಣ *p = types; ; )
    {
	ವರ್ಣ	*end = (ವರ್ಣ *)vim_strchr((char_u *)p, ',');
	ಪೂರ್ಣಾಙ್ಕ	len = end == ರಿಕ್ತ ? (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p) : (ಪೂರ್ಣಾಙ್ಕ)(end - p);

	ಯದಿ (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಯದಿ (end == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	p = end + 1;
    }
    ಪ್ರತ್ಯಾಗಮ l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
getinfo_argtypes(
	list_T			*l,
	ಧ್ರುವ funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    ಪೂರ್ಣಾಙ್ಕ		argcount = 0;

    ಯದಿ (l != ರಿಕ್ತ)
	FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    ಯದಿ (s == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ -1;
	    ಯದಿ (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_ARGS)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		ಪ್ರತ್ಯಾಗಮ -1;
	    }
	    type = parse_type(&p, type_gap, ರಿಕ್ತ, ರಿಕ್ತ, TRUE);
	    ಯದಿ (type == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ -1;
	    ಯದಿ (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		ಪ್ರತ್ಯಾಗಮ -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    ಪ್ರತಿ ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_ARGS; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    ಪ್ರತ್ಯಾಗಮ argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    ಸ್ಥಿರ ವರ್ಣ *
builtin_ret_type_name(
	ಪೂರ್ಣಾಙ್ಕ	    idx,
	ಪೂರ್ಣಾಙ್ಕ	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	ವರ್ಣ	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = internal_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    ಪ್ರತ್ಯಾಗಮ type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_kind_T;

ಸ್ಥಿರ ಸಂರಚನಾ
{
    ವರ್ಣ		*name;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_kind_T	kind;
    ಪೂರ್ಣಾಙ್ಕ			ki;
    ಪೂರ್ಣಾಙ್ಕ			idx;
    ಧ್ರುವ funcentry_T	*fe;
    dict_T		*d;
    list_T		*args;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_ARGS];
    list_T		*argtypes_list = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			argcount;
    ಪೂರ್ಣಾಙ್ಕ			vim9 = -1;	// -1: the context of the caller
    ವರ್ಣ		*ret_name;
    ವರ್ಣ		*tofree;

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (in_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (check_for_opt_dict_arg(argvars, 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    kindname = tv_get_string(&argvars[0]);
    ಪ್ರತಿ (ki = 0; ki < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(getinfo_kinds); ++ki)
	ಯದಿ (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    ಭಙ್ಗ;
    ಯದಿ (ki == (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(getinfo_kinds))
    {
	semsg(_(e_invalid_argument_str), kindname);
	ಪ್ರತ್ಯಾಗಮ;
    }
    kind = getinfo_kinds[ki].kind;
    name = tv_get_string_buf(&argvars[1], namebuf);

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != ರಿಕ್ತ)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	ಪೂರ್ಣಾಙ್ಕ	    todo = (ಪೂರ್ಣಾಙ್ಕ)opts->dv_hashtab.ht_used;

	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&opts->dv_hashtab, hi, todo)
	{
	    ಯದಿ (HASHITEM_EMPTY(hi))
		ಅನುವರ್ತ;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    ಯದಿ (STRCMP(di->di_ಕೀಲಿ, "argtypes") == 0 && kind == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(di->di_ಕೀಲಿ, "vim9") == 0 && kind == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    ಅನ್ಯಥಾ
	    {
		semsg(_(e_invalid_argument_str), di->di_ಕೀಲಿ);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}
    }
    ಯದಿ (kind == GI_VIMVAR)
    {
	ಯದಿ (STRNCMP(name, "v:", 2) == 0)
	    (ಶೂನ್ಯ)vim_var_info(name + 2, rettv->vval.v_dict);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (kind == GI_COMMAND)
    {
	(ಶೂನ್ಯ)ex_command_info(name, vim9, rettv->vval.v_dict);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (kind == GI_OPTION)
    {
	(ಶೂನ್ಯ)option_info(name, rettv->vval.v_dict);
	ಪ್ರತ್ಯಾಗಮ;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = find_internal_func_opt(name, FALSE);
    ಯದಿ (idx < 0)
    {
	(ಶೂನ್ಯ)user_func_info(name, rettv->vval.v_dict);
	ಪ್ರತ್ಯಾಗಮ;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, ಪರಿಮಾಣ(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    ಯದಿ (argcount < 0)
    {
	clear_type_list(&type_gap);
	ಪ್ರತ್ಯಾಗಮ;
    }
    d = rettv->vval.v_dict;
    dict_add_string(d, "name", (char_u *)fe->f_name);
    dict_add_string(d, "kind", (char_u *)"builtin");
    dict_add_bool(d, "available", fe->f_func != ರಿಕ್ತ);
    dict_add_ಸಂಖ್ಯೆ(d, "minargs", fe->f_min_argc);
    dict_add_ಸಂಖ್ಯೆ(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_add_ಸಂಖ್ಯೆ(d, "method", fe->f_argtype & FEARG_MASK);

    args = list_alloc();
    ಯದಿ (args == ರಿಕ್ತ || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	clear_type_list(&type_gap);
	ಪ್ರತ್ಯಾಗಮ;
    }
    argnames = (char_u *)fe->f_argnames;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ	    argnamelen = 0;

	ಯದಿ (fe->f_argcheck == ರಿಕ್ತ)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    ಯದಿ (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		ಭಙ್ಗ;
	    check = arg_any;
	}
	ಅನ್ಯಥಾ
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    ಯದಿ ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	ಯದಿ (argnames != ರಿಕ್ತ)
	{
	    char_u  *end = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = end == ರಿಕ್ತ ? (ಪೂರ್ಣಾಙ್ಕ)STRLEN(argnames)
						     : (ಪೂರ್ಣಾಙ್ಕ)(end - argnames);
	    argnames = end == ರಿಕ್ತ ? ರಿಕ್ತ : skipwhite(end + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	ಯದಿ (arg == ರಿಕ್ತ || types == ರಿಕ್ತ
		|| dict_add_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ ((argnamelen > 0
		    && dict_add_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    ಯದಿ (argtypes_list == ರಿಕ್ತ
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	ವರ್ಣ	*tofree_max;
	ವರ್ಣ	*max_name;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = argcount; i < fe->f_max_argc && i < MAX_FUNC_ARGS; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	ಯದಿ (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = ರಿಕ್ತ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		i;
    list_T	*l;
    dict_T	*d;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    wp = find_tabwin(&argvars[0], &argvars[1], ರಿಕ್ತ);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	ಪ್ರತ್ಯಾಗಮ;
    }

    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    ಪ್ರತಿ (i = 0; i < wp->w_jumplistlen; ++i)
    {
	ಯದಿ (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    ಅನುವರ್ತ;
	ಯದಿ ((d = dict_alloc()) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	dict_add_ಸಂಖ್ಯೆ(d, "lnum", (long)wp->w_jumplist[i].fmark.mark.lnum);
	dict_add_ಸಂಖ್ಯೆ(d, "col", (long)wp->w_jumplist[i].fmark.mark.col);
	dict_add_ಸಂಖ್ಯೆ(d, "coladd", (long)wp->w_jumplist[i].fmark.mark.coladd);
	dict_add_ಸಂಖ್ಯೆ(d, "bufnr", (long)wp->w_jumplist[i].fmark.fnum);
	ಯದಿ (wp->w_jumplist[i].fname != ರಿಕ್ತ)
	    dict_add_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getpid(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getpos(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
block_def2str(ಸಂರಚನಾ block_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    ಯದಿ (ret->string == ರಿಕ್ತ)
    {
	ret->length = 0;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->length = bd->startspaces;
    mch_memmove(ret->string + ret->length, bd->textstart, bd->textlen);
    ret->length += bd->textlen;
    vim_memset(ret->string + ret->length, ' ', bd->endspaces);
    ret->length += bd->endspaces;
    ret->string[ret->length] = NUL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
getregionpos(
    typval_T	*argvars,
    typval_T	*rettv,
    pos_T	*p1,
    pos_T	*p2,
    ಪೂರ್ಣಾಙ್ಕ		*inclusive,
    ಪೂರ್ಣಾಙ್ಕ		*region_type,
    oparg_T	*oap)
{
    ಪೂರ್ಣಾಙ್ಕ		fnum1 = -1, fnum2 = -1;
    char_u	*type;
    buf_T	*findbuf;
    char_u	default_type[] = "v";
    ಪೂರ್ಣಾಙ್ಕ		block_width = 0;
    ಪೂರ್ಣಾಙ್ಕ		is_select_exclusive;
    ಪೂರ್ಣಾಙ್ಕ		l;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (list2fpos(&argvars[0], p1, &fnum1, ರಿಕ್ತ, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, ರಿಕ್ತ, FALSE) != OK
	    || fnum1 != fnum2)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	ಯದಿ (type == ರಿಕ್ತ)
	    type = default_type;
    }
    ಅನ್ಯಥಾ
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    ಯದಿ (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    ಅನ್ಯಥಾ ಯದಿ (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    ಅನ್ಯಥಾ ಯದಿ (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	ಯದಿ (*p != NUL && ((block_width = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "type", type);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	*region_type = MBLOCK;
    }
    ಅನ್ಯಥಾ
    {
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "type", type);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    ಯದಿ (findbuf == ರಿಕ್ತ || findbuf->b_ml.ml_mfp == ರಿಕ್ತ)
    {
	emsg(_(e_buffer_is_not_loaded));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), p1->lnum);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (p1->col == MAXCOL)
	p1->col = ml_get_buf_len(findbuf, p1->lnum) + 1;
    ಅನ್ಯಥಾ ಯದಿ (p1->col < 1 || p1->col > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_column_ಸಂಖ್ಯೆ_nr), p1->col);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), p2->lnum);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (p2->col == MAXCOL)
	p2->col = ml_get_buf_len(findbuf, p2->lnum) + 1;
    ಅನ್ಯಥಾ ಯದಿ (p2->col < 1 || p2->col > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_column_ಸಂಖ್ಯೆ_nr), p2->col);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    curbuf = findbuf;
    curwin->w_buffer = curbuf;
    virtual_op = virtual_active();

    // NOTE: Adjustment is needed.
    p1->col--;
    p2->col--;

    ಯದಿ (!LT_POS(*p1, *p2))
    {
	// swap position
	pos_T p;

	p = *p1;
	*p1 = *p2;
	*p2 = p;
    }

    ಯದಿ (*region_type == MCHAR)
    {
	// Handle 'selection' == "exclusive".
	ಯದಿ (is_select_exclusive && !EQUAL_POS(*p1, *p2))
	    // When backing up to previous line, inclusive becomes false.
	    *inclusive = !unadjust_for_sel_inner(p2);
	// If p2 is on NUL (end of line), inclusive becomes false.
	ಯದಿ (*inclusive && !virtual_op && *ml_get_pos(p2) == NUL)
	    *inclusive = FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#ifdef FEAT_LINEBREAK
	ಪೂರ್ಣಾಙ್ಕ	lbr_saved = reset_lbr();
#endif

	getvvcol(curwin, p1, &sc1, ರಿಕ್ತ, &ec1, 0);
	getvvcol(curwin, p2, &sc2, ರಿಕ್ತ, &ec2, 0);

#ifdef FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#endif

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->start = *p1;
	oap->end = *p2;
	oap->start_vcol = MIN(sc1, sc2);
	ಯದಿ (block_width > 0)
	    oap->end_vcol = oap->start_vcol + block_width - 1;
	ಅನ್ಯಥಾ ಯದಿ (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->end_vcol = sc2 - 1;
	ಅನ್ಯಥಾ
	    oap->end_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    ಯದಿ (l > 1)
	p2->col += l - 1;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * "getregion()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    ಪೂರ್ಣಾಙ್ಕ			inclusive = TRUE;
    ಪೂರ್ಣಾಙ್ಕ			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    ಪೂರ್ಣಾಙ್ಕ			save_virtual;
    string_T		akt = {ರಿಕ್ತ, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    ಯದಿ (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	ಪೂರ್ಣಾಙ್ಕ ret = OK;

	ಯದಿ (region_type == MBLOCK)
	{
	    ಸಂರಚನಾ block_def	bd;

	    block_prep(&oa, &bd, lnum, FALSE);
	    block_def2str(&bd, &akt);
	}
	ಅನ್ಯಥಾ ಯದಿ (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.length = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.length);
	    ಯದಿ (akt.string == ರಿಕ್ತ)
		akt.length = 0;
	    ಅನ್ಯಥಾ
		akt.length = s.length;
	}
	ಅನ್ಯಥಾ
	{
	    ಸಂರಚನಾ block_def	bd;

	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);
	    block_def2str(&bd, &akt);
	}

	ಯದಿ (akt.string == ರಿಕ್ತ)
	{
	    clear_tv(rettv);
	    (ಶೂನ್ಯ)rettv_list_alloc(rettv);
	    ಭಙ್ಗ;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (ಪೂರ್ಣಾಙ್ಕ)akt.length);
	vim_free(akt.string);

	ಯದಿ (ret == FAIL)
	{
	    clear_tv(rettv);
	    (ಶೂನ್ಯ)rettv_list_alloc(rettv);
	    ಭಙ್ಗ;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_buffer = curbuf;
    virtual_op = save_virtual;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
add_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    ಯದಿ (l1 == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	ಪ್ರತ್ಯಾಗಮ;
    }

    l2 = list_alloc();
    ಯದಿ (l2 == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	ಪ್ರತ್ಯಾಗಮ;
    }

    l3 = list_alloc();
    ಯದಿ (l3 == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	ಪ್ರತ್ಯಾಗಮ;
    }

    list_append_ಸಂಖ್ಯೆ(l2, curbuf->b_fnum);
    list_append_ಸಂಖ್ಯೆ(l2, p1.lnum);
    list_append_ಸಂಖ್ಯೆ(l2, p1.col);
    list_append_ಸಂಖ್ಯೆ(l2, p1.coladd);

    list_append_ಸಂಖ್ಯೆ(l3, curbuf->b_fnum);
    list_append_ಸಂಖ್ಯೆ(l3, p2.lnum);
    list_append_ಸಂಖ್ಯೆ(l3, p2.col);
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
    ಪೂರ್ಣಾಙ್ಕ		inclusive,
    ಪೂರ್ಣಾಙ್ಕ		region_type,
    oparg_T	*oap,
    ಪೂರ್ಣಾಙ್ಕ		allow_eol,
    pos_T	*ret_p1,
    pos_T	*ret_p2)
{
    char_u	*ಸಾಲು = ml_get(lnum);
    colnr_T	ಸಾಲು_len = ml_get_len(lnum);

    ಯದಿ (region_type == MLINE)
    {
	ret_p1->col = 1;
	ret_p1->coladd = 0;
	ret_p2->col = MAXCOL;
	ret_p2->coladd = 0;
    }
    ಅನ್ಯಥಾ
    {
	ಸಂರಚನಾ block_def	bd;

	ಯದಿ (region_type == MBLOCK)
	    block_prep(oap, &bd, lnum, FALSE);
	ಅನ್ಯಥಾ
	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);

	ಯದಿ (bd.is_oneChar)  // selection entirely inside one char
	{
	    ಯದಿ (region_type == MBLOCK)
	    {
		ret_p1->col = mb_prevptr(ಸಾಲು, bd.textstart) - ಸಾಲು + 1;
		ret_p1->coladd = bd.start_char_vcols
					   - (bd.start_vcol - oap->start_vcol);
	    }
	    ಅನ್ಯಥಾ
	    {
		ret_p1->col = p1.col + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (region_type == MBLOCK && oap->start_vcol > bd.start_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->col = MAXCOL;
	    ret_p1->coladd = oap->start_vcol - bd.start_vcol;
	    bd.is_oneChar = TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (bd.startspaces > 0)
	{
	    ret_p1->col = mb_prevptr(ಸಾಲು, bd.textstart) - ಸಾಲು + 1;
	    ret_p1->coladd = bd.start_char_vcols - bd.startspaces;
	}
	ಅನ್ಯಥಾ
	{
	    ret_p1->col = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	ಯದಿ (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->col = ret_p1->col;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	ಅನ್ಯಥಾ ಯದಿ (bd.endspaces > 0)
	{
	    ret_p2->col = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	ಅನ್ಯಥಾ
	{
	    ret_p2->col = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    ಯದಿ (!allow_eol && ret_p1->col > ಸಾಲು_len)
    {
	ret_p1->col = 0;
	ret_p1->coladd = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (ret_p1->col > ಸಾಲು_len + 1)
	ret_p1->col = ಸಾಲು_len + 1;

    ಯದಿ (!allow_eol && ret_p2->col > ಸಾಲು_len)
    {
	ret_p2->col = ret_p1->col == 0 ? 0 : ಸಾಲು_len;
	ret_p2->coladd = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (ret_p2->col > ಸಾಲು_len + 1)
	ret_p2->col = ಸಾಲು_len + 1;

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
    ಪೂರ್ಣಾಙ್ಕ		inclusive = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		region_type = -1;
    ಪೂರ್ಣಾಙ್ಕ		allow_eol = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		bounds_only = FALSE;
    oparg_T	oa;
    ಪೂರ್ಣಾಙ್ಕ		lnum;

    buf_T	*save_curbuf;
    ಪೂರ್ಣಾಙ್ಕ		save_virtual;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    ಯದಿ (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    ಯದಿ (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	start_pos, end_pos;

	getregionpos_ಸಾಲು(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &start_pos, &end_pos);
	ಯದಿ (p2.lnum != p1.lnum)
	{
	    pos_T	unused;

	    getregionpos_ಸಾಲು(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &unused, &end_pos);
	}
	add_regionpos_range(rettv, start_pos, end_pos);
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_ಸಾಲು(lnum, p1, p2, inclusive, region_type, &oa,
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	ಯದಿ (strregname != ರಿಕ್ತ && in_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_register_name_must_be_one_char_str), strregname);
	    strregname = ರಿಕ್ತ;
	}
	ಯದಿ (strregname == ರಿಕ್ತ)	    // type error; errmsg already given
	    ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಅನ್ಯಥಾ
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    ಪ್ರತ್ಯಾಗಮ *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getreg(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		regname;
    ಪೂರ್ಣಾಙ್ಕ		arg2 = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		return_list = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	ಪ್ರತ್ಯಾಗಮ;

    regname = getreg_get_regname(argvars);
    ಯದಿ (regname == 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

	arg2 = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[1], &error);

	ಯದಿ (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[2], &error);
	ಯದಿ (error)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC : 0) | GREG_LIST);
	ಯದಿ (rettv->vval.v_list == ರಿಕ್ತ)
	    (ಶೂನ್ಯ)rettv_list_alloc(rettv);
	ಅನ್ಯಥಾ
	    ++rettv->vval.v_list->lv_refcount;
    }
    ಅನ್ಯಥಾ
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
    ಪೂರ್ಣಾಙ್ಕ		regname;
    char_u	buf[NUMBUFLEN + 2];
    long	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    regname = getreg_get_regname(argvars);
    ಯದಿ (regname == 0)
	ಪ್ರತ್ಯಾಗಮ;

    buf[0] = NUL;
    buf[1] = NUL;
    ವಿಕಲ್ಪ (get_reg_type(regname, &reglen))
    {
	ಪಕ್ಷ MLINE: buf[0] = 'V'; ಭಙ್ಗ;
	ಪಕ್ಷ MCHAR: buf[0] = 'v'; ಭಙ್ಗ;
	ಪಕ್ಷ MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((ವರ್ಣ *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		ಭಙ್ಗ;
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

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	ಯದಿ (wp == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_gettext(typval_T *argvars, typval_T *rettv)
{
#ಯದಿ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    ವರ್ಣ *prev = ರಿಕ್ತ;
#endif

    ಯದಿ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_STRING;

    ಯದಿ (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != ರಿಕ್ತ &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#ಯದಿ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ಧ್ರುವ ವರ್ಣ *)argvars[1].vval.v_string, (ವರ್ಣ *)p_enc);
#endif

#ಯದಿ defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((ಧ್ರುವ ವರ್ಣ *)argvars[1].vval.v_string, (ಧ್ರುವ ವರ್ಣ *)argvars[0].vval.v_string));
#ಅನ್ಯಥಾ
	textdomain((ಧ್ರುವ ವರ್ಣ *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#endif

#ಯದಿ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	ಯದಿ (prev != ರಿಕ್ತ)
	    bind_textdomain_codeset((ಧ್ರುವ ವರ್ಣ *)argvars[1].vval.v_string, prev);
#endif
    }
    ಅನ್ಯಥಾ
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
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*name;
    ಪೂರ್ಣಾಙ್ಕ		x = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		n = FALSE;
    ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
	ವರ್ಣ *name;
	short present;
    } has_item_T;
    ಸ್ಥಿರ has_item_T has_list[] =
    {
	{"amiga",
#ifdef AMIGA
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"android",
#ifdef __ANDROID__
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"arp",
#ಯದಿ defined(AMIGA) && defined(FEAT_ARP)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"haiku",
#ifdef __HAIKU__
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"bsd",
#ಯದಿ defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"hpux",
#ifdef hpux
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"hurd",
#ifdef __GNU__
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"linux",
#ifdef __linux__
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#ifdef MACOS_X
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"osx",		// Mac OS X
#ifdef MACOS_X
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"macunix",	// Mac OS X, with the darwin feature
#ಯದಿ defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"osxdarwin",	// synonym for macunix
#ಯದಿ defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"qnx",
#ifdef __QNX__
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"sun",
#ifdef SUN_SYSTEM
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"termux",
#ifdef __TERMUX__
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"unix",
#ifdef UNIX
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"vms",
#ifdef VMS
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"win32",
#ifdef MSWIN
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"win32unix",
#ifdef WIN32UNIX
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"win64",
#ifdef _WIN64
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"ebcdic", 0 },
	{"fname_case",
#ifndef CASE_INSENSITIVE_FILENAME
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"acl",
#ifdef HAVE_ACL
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"arabic",
#ifdef FEAT_ARABIC
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"autocmd", 1},
	{"autochdir",
#ifdef FEAT_AUTOCHDIR
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"autoservername",
#ifdef FEAT_AUTOSERVERNAME
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"socketserver",
#ifdef FEAT_SOCKETSERVER
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"balloon_eval",
#ifdef FEAT_BEVAL_GUI
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"balloon_multiline",
#ifdef FEAT_BEVAL_GUI
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"balloon_eval_term",
#ifdef FEAT_BEVAL_TERM
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#ಯದಿ defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"byte_offset",
#ifdef FEAT_BYTEOFF
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"channel",
#ifdef FEAT_JOB_CHANNEL
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"cindent", 1},
	{"clientserver",
#ifdef FEAT_CLIENTSERVER
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"clipboard",
#ifdef FEAT_CLIPBOARD
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"clipboard_provider",
#ifdef FEAT_CLIPBOARD_PROVIDER
		1
#ಅನ್ಯಥಾ
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
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"cryptv",
#ifdef FEAT_CRYPT
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"crypt-blowfish",
#ifdef FEAT_CRYPT
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"crypt-blowfish2",
#ifdef FEAT_CRYPT
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"cscope",
#ifdef FEAT_CSCOPE
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"cursorbind", 1},
	{"cursorshape",
#ifdef CURSOR_SHAPE
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"debug",
#ifdef DEBUG
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"dialog_con",
#ifdef FEAT_CON_DIALOG
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"dialog_con_gui",
#ಯದಿ defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"dialog_gui",
#ifdef FEAT_GUI_DIALOG
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"diff",
#ifdef FEAT_DIFF
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"digraphs",
#ifdef FEAT_DIGRAPHS
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"directx",
#ifdef FEAT_DIRECTX
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"dnd",
#ifdef FEAT_DND
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"drop_file",
#ifdef HAVE_DROP_FILE
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"emacs_tags",
#ifdef FEAT_EMACS_TAGS
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#ifdef FEAT_ಹುಡುಕು_EXTRA
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"file_in_path", 1},
	{"filterpipe",
#ಯದಿ defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"find_in_path",
#ifdef FEAT_FIND_ID
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"float", 1},
	{"folding",
#ifdef FEAT_FOLDING
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"footer", 0},
	{"fork",
#ಯದಿ !defined(USE_SYSTEM) && defined(UNIX)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"gettext",
#ifdef FEAT_GETTEXT
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"gui",
#ifdef FEAT_GUI
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#ifdef FEAT_GUI_GTK
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"gui_gtk2",
#ಯದಿ defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"gui_gtk3",
#ಯದಿ defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"gui_gtk4",
#ಯದಿ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"gui_gnome",
#ifdef FEAT_GUI_GNOME
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"gui_haiku",
#ifdef FEAT_GUI_HAIKU
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"gui_mac", 0},
	{"gui_motif",
#ifdef FEAT_GUI_MOTIF
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"gui_photon",
#ifdef FEAT_GUI_PHOTON
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"gui_win32",
#ifdef FEAT_GUI_MSWIN
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"iconv",
#ಯದಿ defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"image",
#ifdef FEAT_IMAGE
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"image_popup",
#ifdef FEAT_IMAGE_POPUP
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"insert_expand", 1},
	{"ipv6",
#ifdef FEAT_IPV6
		1
#ಅನ್ಯಥಾ
		0
#endif
	},
	{"job",
#ifdef FEAT_JOB_CHANNEL
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"jumplist", 1},
	{"keymap",
#ifdef FEAT_KEYMAP
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#ifdef FEAT_LANGMAP
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"libcall",
#ifdef FEAT_LIBCALL
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"linebreak",
#ifdef FEAT_LINEBREAK
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#ಯದಿ defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"menu",
#ifdef FEAT_MENU
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"mksession",
#ifdef FEAT_SESSION
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#ifdef FEAT_MOUSESHAPE
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"mouse_dec",
#ಯದಿ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"mouse_gpm",
#ಯದಿ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"mouse_jsbterm",
#ಯದಿ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"mouse_netterm",
#ಯದಿ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"mouse_pterm",
#ಯದಿ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"mouse_sgr",
#ಯದಿ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"mouse_sysmouse",
#ಯದಿ (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"mouse_urxvt",
#ಯದಿ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"mouse_xterm",
#ಯದಿ (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#ifdef FEAT_MBYTE_IME
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"multi_lang",
#ifdef FEAT_MULTI_LANG
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"mzscheme",
#ಯದಿ defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"nanotime",
#ifdef ST_MTIM_NSEC
		1
#ಅನ್ಯಥಾ
		0
#endif
	},
	{"num64", 1},
	{"ole",
#ifdef FEAT_OLE
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"packages",
#ifdef FEAT_EVAL
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"path_extra", 1},
	{"perl",
#ಯದಿ defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"persistent_undo",
#ifdef FEAT_PERSISTENT_UNDO
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"python_compiled",
#ಯದಿ defined(FEAT_PYTHON)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"python_dynamic",
#ಯದಿ defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"python",
#ಯದಿ defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"pythonx",
#ಯದಿ (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"python3_compiled",
#ಯದಿ defined(FEAT_PYTHON3)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"python3_dynamic",
#ಯದಿ defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"python3_stable",
#ಯದಿ defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"python3",
#ಯದಿ defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"popupwin",
#ifdef FEAT_PROP_POPUP
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"postscript",
#ifdef FEAT_POSTSCRIPT
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"pango",
#ifdef FEAT_PRINT_PANGO
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"printer",
#ifdef FEAT_PRINTER
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"profile",
#ifdef FEAT_PROFILE
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"prof_nsec",
#ifdef PROF_NSEC
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"reltime",
#ifdef FEAT_RELTIME
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"quickfix",
#ifdef FEAT_QUICKFIX
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"rightleft",
#ifdef FEAT_RIGHTLEFT
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"ruby",
#ಯದಿ defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#ifdef FEAT_SIGNS
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"smartindent", 1},
	{"startuptime",
#ifdef STARTUPTIME
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"statusline",
#ifdef FEAT_STL_OPT
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"statusline_click",
#ifdef FEAT_STL_OPT
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"netbeans_intg",
#ifdef FEAT_NETBEANS_INTG
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"sodium",
#ಯದಿ defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"sound",
#ifdef FEAT_SOUND
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"spell",
#ifdef FEAT_SPELL
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"syntax",
#ifdef FEAT_SYN_HL
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"system",
#ಯದಿ defined(USE_SYSTEM) || !defined(UNIX)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"tabpanel",
#ಯದಿ defined(FEAT_TABPANEL)
		1,
#ಅನ್ಯಥಾ
		0,
#endif
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#ಯದಿ defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"termguicolors",
#ifdef FEAT_TERMGUICOLORS
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"terminal",
#ಯದಿ defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"terminfo",
#ifdef TERMINFO
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"termresponse",
#ifdef FEAT_TERMRESPONSE
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"textobjects", 1},
	{"textprop",
#ifdef FEAT_PROP_POPUP
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"tgetent",
#ifdef HAVE_TGETENT
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"timers",
#ifdef FEAT_TIMERS
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"title", 1},
	{"toolbar",
#ifdef FEAT_TOOLBAR
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#ifdef FEAT_VARTABS
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"vertsplit", 1},
	{"viminfo",
#ifdef FEAT_VIMINFO
		1
#ಅನ್ಯಥಾ
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
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"wayland",
#ifdef FEAT_WAYLAND
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"wayland_clipboard",
#ifdef FEAT_WAYLAND_CLIPBOARD
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#ifdef FEAT_WAK
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"writebackup",
#ifdef FEAT_WRITEBACKUP
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"xattr",
#ifdef FEAT_XATTR
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"xim",
#ifdef FEAT_XIM
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"xfontset",
#ifdef FEAT_XFONTSET
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"xpm",
#ಯದಿ defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"xpm_w32",	// for backward compatibility
#ifdef FEAT_XPM_W32
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"xsmp",
#ifdef USE_XSMP
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"xsmp_interact",
#ifdef USE_XSMP_INTERACT
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"xterm_clipboard",
#ifdef FEAT_XCLIPBOARD
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"xterm_save",
#ifdef FEAT_XTERM_SAVE
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{"X11",
#ಯದಿ defined(UNIX) && defined(FEAT_X11)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#ಯದಿ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#ಅನ್ಯಥಾ
		0
#endif
		},
	{ರಿಕ್ತ, 0}
    };

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string(&argvars[0]);

    // Fast-path: check features not in has_list[] first to avoid the full
    // linear scan for very common queries like has('patch-...').
    ಯದಿ (STRNICMP(name, "patch", 5) == 0)
    {
	x = TRUE;
	ಯದಿ (name[5] == '-'
		&& STRLEN(name) >= 11
		&& (name[6] >= '1' && name[6] <= '9'))
	{
	    ವರ್ಣ	*end;
	    ಪೂರ್ಣಾಙ್ಕ	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (ಪೂರ್ಣಾಙ್ಕ)strtoul((ವರ್ಣ *)name + 6, &end, 10);
	    ಯದಿ (*end == '.' && vim_isdigit(end[1])
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
	ಅನ್ಯಥಾ ಯದಿ (SAFE_isdigit(name[5]))
	    n = has_patch(atoi((ವರ್ಣ *)name + 5));
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_ಒಳಹರಿವು_isatty();
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#ifdef FEAT_GUI
	n = (gui.in_use || gui.starting);
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#ಯದಿ defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.in_use;	// gui_mch_browse() works when GUI is running
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#ifdef FEAT_SYN_HL
	n = syntax_present(curwin);
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#ifdef FEAT_VTP
	n = is_term_win32() && has_vtp_working();
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_NETBEANS_INTG
	n = netbeans_active();
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_MOUSE_GPM
	n = gpm_enabled();
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#ಯದಿ defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_star.available;
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# ಯದಿ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	ಯದಿ (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	ಅನ್ಯಥಾ
	    n = FALSE;
# ಅನ್ಯಥಾ
	n = FALSE;
# endif
#endif
    }

    // Look up in has_list[] only if not already handled above.
    ಯದಿ (x == FALSE)
    {
	ಪ್ರತಿ (i = 0; has_list[i].name != ರಿಕ್ತ; ++i)
	    ಯದಿ (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		ಭಙ್ಗ;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	ಯದಿ (x == TRUE && n == FALSE)
	{
	    ಯದಿ (0)
	    {
		// intentionally empty
	    }
#ifdef VIMDLL
	    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "filterpipe") == 0)
		n = gui.in_use || gui.starting;
#endif
#ಯದಿ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#endif
#ifdef DYNAMIC_LUA
	    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#endif
#ifdef DYNAMIC_MZSCHEME
	    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#endif
#ifdef DYNAMIC_PERL
	    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON
	    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON3
	    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#endif
#ಯದಿ defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "pythonx") == 0)
	    {
# ಯದಿ defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		ಯದಿ (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		ಅನ್ಯಥಾ ಯದಿ (p_pyx == 3)
		    n = python3_enabled(FALSE);
		ಅನ್ಯಥಾ ಯದಿ (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#endif
#ifdef DYNAMIC_RUBY
	    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#endif
#ifdef DYNAMIC_TCL
	    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#endif
#ifdef DYNAMIC_SODIUM
	    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#endif
#ಯದಿ defined(FEAT_TERMINAL) && defined(MSWIN)
	    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#endif
#ifdef DYNAMIC_GPM
	    ಅನ್ಯಥಾ ಯದಿ (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_available();
#endif
	}
    }

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_ಸಂಖ್ಯೆ = x;
    ಅನ್ಯಥಾ
	// return whether feature is enabled
	rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    ಪೂರ್ಣಾಙ್ಕ
dynamic_feature(char_u *feature)
{
    ಪ್ರತ್ಯಾಗಮ (feature == ರಿಕ್ತ
#ಯದಿ defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.in_use)
#endif
#ifdef VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#endif
#ಯದಿ defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.in_use)
#endif
#ಯದಿ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
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
#ಯದಿ defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
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
#ಯದಿ defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#endif
	    );
}

/*
 * "haslocaldir()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = ರಿಕ್ತ;
    win_T	*wp = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    ಯದಿ (wp != ರಿಕ್ತ && wp->w_localdir != ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
    ಅನ್ಯಥಾ ಯದಿ (tp != ರಿಕ್ತ && tp->tp_localdir != ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = 2;
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
}

/*
 * "highlightID(name)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_hlID(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
# ಯದಿ defined(HAVE_NO_LONG_LONG)
ಪ್ರಕಾರನಿರ್ದೇಶ unsigned ಪೂರ್ಣಾಙ್ಕ uintptr_t;
# ಅನ್ಯಥಾ
ಪ್ರಕಾರನಿರ್ದೇಶ unsigned long long uintptr_t;
# endif
#endif // VMS

    ಸ್ಥಿರ ಶೂನ್ಯ
f_id(typval_T *argvars, typval_T *rettv)
{
    ವರ್ಣ    numbuf[NUMBUFLEN];
    ವರ್ಣ    *p = numbuf;

    ವಿಕಲ್ಪ (argvars[0].v_type)
    {
	ಪಕ್ಷ VAR_LIST:
	ಪಕ್ಷ VAR_TUPLE:
	ಪಕ್ಷ VAR_DICT:
	ಪಕ್ಷ VAR_OBJECT:
	ಪಕ್ಷ VAR_JOB:
	ಪಕ್ಷ VAR_CHANNEL:
	ಪಕ್ಷ VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    ಯದಿ (argvars[0].vval.v_object != ರಿಕ್ತ)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(ಶೂನ್ಯ *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		ಪ್ರತಿ (; p < numbuf + ಪರಿಮಾಣ(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
index_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    ಪೂರ್ಣಾಙ್ಕ		start = 0;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		ic = FALSE;

    b = argvars[0].vval.v_blob;
    ಯದಿ (b == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	start = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಯದಿ (error)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (start < 0)
    {
	start = blob_len(b) + start;
	ಯದಿ (start < 0)
	    start = 0;
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = start; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_ಸಂಖ್ಯೆ;
	tv.vval.v_ಸಂಖ್ಯೆ = blob_get(b, idx);
	ಯದಿ (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = idx;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
}

/*
 * index() function for a tuple
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
index_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    ಪೂರ್ಣಾಙ್ಕ		ic = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    ಯದಿ (tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ	start_idx = 0;
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	start_idx = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಯದಿ (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[3], &error);
	ಯದಿ (error)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = index_tuple(tuple, &argvars[1], start_idx, ic);
}

/*
 * index() function for a list
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
index_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*item;
    long	idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		ic = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    l = argvars[0].vval.v_list;
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    CHECK_LIST_MATERIALIZE(l);
    item = l->lv_first;
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	ಯದಿ (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[3], &error);
	ಯದಿ (error)
	    item = ರಿಕ್ತ;
    }

    ಪ್ರತಿ ( ; item != ರಿಕ್ತ; item = item->li_next, ++idx)
	ಯದಿ (tv_equal(&item->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = idx;
	    ಭಙ್ಗ;
	}
}

/*
 * "index()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_index(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_BLOB)
	index_func_blob(argvars, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_TUPLE)
	index_func_tuple(argvars, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_LIST)
	index_func_list(argvars, rettv);
    ಅನ್ಯಥಾ
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    ಪೂರ್ಣಾಙ್ಕ
indexof_eval_ಅಭಿವ್ಯಕ್ತಿ(typval_T *ಅಭಿವ್ಯಕ್ತಿ)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	found;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_ಕೀಲಿ);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    ಯದಿ (eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, FALSE, argv, 2, ರಿಕ್ತ, &newtv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    found = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    ಪ್ರತ್ಯಾಗಮ error ? FALSE : found;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
indexof_blob(blob_T *b, long startidx, typval_T *ಅಭಿವ್ಯಕ್ತಿ)
{
    long	idx = 0;

    ಯದಿ (b == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	ಯದಿ (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);
    set_vim_var_type(VV_VAL, VAR_ಸಂಖ್ಯೆ);

    ಪೂರ್ಣಾಙ್ಕ		called_emsg_start = called_emsg;
    ಪ್ರತಿ (idx = startidx; idx < blob_len(b); ++idx)
    {
	set_vim_var_nr(VV_ಕೀಲಿ, idx);
	set_vim_var_nr(VV_VAL, blob_get(b, idx));

	ಯದಿ (indexof_eval_ಅಭಿವ್ಯಕ್ತಿ(ಅಭಿವ್ಯಕ್ತಿ))
	    ಪ್ರತ್ಯಾಗಮ idx;

	ಯದಿ (called_emsg != called_emsg_start)
	    ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
indexof_list(list_T *l, long startidx, typval_T *ಅಭಿವ್ಯಕ್ತಿ)
{
    listitem_T	*item;
    long	idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		found;

    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    CHECK_LIST_MATERIALIZE(l);

    ಯದಿ (startidx == 0)
	item = l->lv_first;
    ಅನ್ಯಥಾ
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, startidx);
	ಯದಿ (item != ರಿಕ್ತ)
	    idx = l->lv_u.mat.lv_idx;
    }

    set_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);

    ಪೂರ್ಣಾಙ್ಕ		called_emsg_start = called_emsg;
    ಪ್ರತಿ ( ; item != ರಿಕ್ತ; item = item->li_next, ++idx)
    {
	set_vim_var_nr(VV_ಕೀಲಿ, idx);
	copy_tv(&item->li_tv, get_vim_var_tv(VV_VAL));

	found = indexof_eval_ಅಭಿವ್ಯಕ್ತಿ(ಅಭಿವ್ಯಕ್ತಿ);
	clear_tv(get_vim_var_tv(VV_VAL));

	ಯದಿ (found)
	    ಪ್ರತ್ಯಾಗಮ idx;

	ಯದಿ (called_emsg != called_emsg_start)
	    ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * "indexof()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_indexof(typval_T *argvars, typval_T *rettv)
{
    long	startidx = 0;
    typval_T	save_val;
    typval_T	save_ಕೀಲಿ;
    ಪೂರ್ಣಾಙ್ಕ		save_did_emsg;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == ರಿಕ್ತ
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_ಸಂಖ್ಯೆ_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_ಕೀಲಿ, &save_ಕೀಲಿ);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    ಯದಿ (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_ಸಂಖ್ಯೆ = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_ಸಂಖ್ಯೆ = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_ಕೀಲಿ, &save_ಕೀಲಿ);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ inputsecret_flag = 0;

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
#ಯದಿ defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    ಯದಿ (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == ರಿಕ್ತ)
    {
	char_u	*message;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	ಯದಿ (in_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    ಪ್ರತ್ಯಾಗಮ;

	message = tv_get_string_chk(&argvars[0]);
	ಯದಿ (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != ರಿಕ್ತ)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	ಅನ್ಯಥಾ
	    IObuff[0] = NUL;
	ಯದಿ (message != ರಿಕ್ತ && defstr != ರಿಕ್ತ
		&& do_dialog(VIM_QUESTION, ರಿಕ್ತ, message,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (message != ರಿಕ್ತ && defstr != ರಿಕ್ತ
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    ಅನ್ಯಥಾ
		rettv->vval.v_string = ರಿಕ್ತ;
	}
	rettv->v_type = VAR_STRING;
    }
    ಅನ್ಯಥಾ
#endif
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
    ಪೂರ್ಣಾಙ್ಕ		selected;
    ಪೂರ್ಣಾಙ್ಕ		mouse_used;

#ifdef NO_CONSOLE_ಒಳಹರಿವು
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    ಯದಿ (no_console_ಒಳಹರಿವು() && !is_not_a_term())
	ಪ್ರತ್ಯಾಗಮ;
#endif
    ಯದಿ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ರಿಕ್ತ)
    {
	semsg(_(e_argument_of_str_must_be_list), "inputlist()");
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಸಂದೇಶ_start();
    ಸಂದೇಶ_row = Rows - 1;	// for when 'cmdheight' > 1
    ಸಾಲುಗಳು_ಎಡ = Rows;	// avoid more prompt
    ಸಂದೇಶ_scroll = TRUE;
    ಸಂದೇಶ_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಸಂದೇಶ_puts((ವರ್ಣ *)tv_get_string(&li->li_tv));
	ಸಂದೇಶ_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_ಸಂಖ್ಯೆ(&mouse_used);
    ಯದಿ (mouse_used)
	selected -= ಸಾಲುಗಳು_ಎಡ;

    rettv->vval.v_ಸಂಖ್ಯೆ = selected;
}

ಸ್ಥಿರ garray_T	    ga_userinput = {0, 0, ಪರಿಮಾಣ(tasave_T), 4, ರಿಕ್ತ};

/*
 * "inputrestore()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_inputrestore(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಯದಿ (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_data)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    ಅನ್ಯಥಾ ಯದಿ (p_verbose > 1)
    {
	verb_ಸಂದೇಶ(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_ಸಂಖ್ಯೆ = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_inputsave(typval_T *argvars UNUSED, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    ಯದಿ (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_data)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ++cmdline_star;
    ++inputsecret_flag;
    f_ಒಳಹರಿವು(argvars, rettv);
    --cmdline_star;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_interrupt(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_invert(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = ~tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_lval_ಬೇರು(lval_ಬೇರು_T *ಬೇರು)
{
    ಯದಿ (ಬೇರು->lr_tv != ರಿಕ್ತ)
	free_tv(ಬೇರು->lr_tv);
    class_unref(ಬೇರು->lr_cl_exec);
    ಬೇರು->lr_tv = ರಿಕ್ತ;
    ಬೇರು->lr_cl_exec = ರಿಕ್ತ;
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

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fix_variable_reference_lval_ಬೇರು(lval_ಬೇರು_T *ಬೇರು, char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    ಪೂರ್ಣಾಙ್ಕ found_member = FALSE;
    ಯದಿ (ಬೇರು->lr_tv->v_type == VAR_OBJECT)
    {
	ಯದಿ (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    found_member = TRUE;
    }
    ಯದಿ (!found_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*end;
	ಪ್ರತಿ (end = name; ASCII_ISALNUM(*end) || *end == '_'; ++end)
	    ;
	ಪೂರ್ಣಾಙ್ಕ idx = class_member_idx(ಬೇರು->lr_cl_exec, name, end - name);
	ಯದಿ (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(ಬೇರು->lr_tv);
	    copy_tv(&ಬೇರು->lr_cl_exec->class_members_tv[idx], ಬೇರು->lr_tv);
	    found_member = TRUE;
	}
    }
    ಯದಿ (!found_member)
    {
	free_tv(ಬೇರು->lr_tv);
	ಬೇರು->lr_tv = ರಿಕ್ತ;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * "islocked()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*end;
    dictitem_T	*di;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    char_u *name = tv_get_string(&argvars[0]);
#ifdef LOG_LOCKVAR
    ch_log(ರಿಕ್ತ, "LKVAR: f_islocked(): name: %s", name);
#endif

    lval_ಬೇರು_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_ಬೇರು_T *ಬೇರು = ರಿಕ್ತ;

    // Set up lval_root if executing in a method.
    ಯದಿ (fill_exec_lval_ಬೇರು(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	ಯದಿ (fix_variable_reference_lval_ಬೇರು(&aroot, name) == OK)
	    ಬೇರು = &aroot;
    }

    lval_ಬೇರು_T	*lval_ಬೇರು_save = lval_ಬೇರು;
    lval_ಬೇರು = ಬೇರು;
    end = get_lval(name, ರಿಕ್ತ, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_START);
    lval_ಬೇರು = lval_ಬೇರು_save;

    ಯದಿ (end != ರಿಕ್ತ && lv.ll_name != ರಿಕ್ತ)
    {
	ಯದಿ (*end != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_end
		   ? e_invalid_argument_str : e_trailing_characters_str), end);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (lv.ll_tv == ರಿಕ್ತ)
	    {
		di = find_var(lv.ll_name, ರಿಕ್ತ, TRUE);
		ಯದಿ (di != ರಿಕ್ತ)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_ಸಂಖ್ಯೆ = ((di->di_flags & DI_FLAGS_LOCK)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (lv.ll_is_ಬೇರು)
	    {
		rettv->vval.v_ಸಂಖ್ಯೆ = tv_islocked(lv.ll_tv);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (lv.ll_object != ರಿಕ್ತ)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_ಸಂಖ್ಯೆ = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(ರಿಕ್ತ, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#endif
	    }
	    ಅನ್ಯಥಾ ಯದಿ (lv.ll_class != ರಿಕ್ತ)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_ಸಂಖ್ಯೆ = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(ರಿಕ್ತ, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#endif
	    }
	    ಅನ್ಯಥಾ ಯದಿ (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    ಅನ್ಯಥಾ ಯದಿ (lv.ll_newkey != ರಿಕ್ತ)
		semsg(_(e_ಕೀಲಿ_not_present_in_dictionary_str), lv.ll_newkey);
	    ಅನ್ಯಥಾ ಯದಿ (lv.ll_list != ರಿಕ್ತ)
		// List item.
		rettv->vval.v_ಸಂಖ್ಯೆ = tv_islocked(&lv.ll_li->li_tv);
	    ಅನ್ಯಥಾ ಯದಿ (lv.ll_tuple != ರಿಕ್ತ)
		// Tuple item.
		rettv->vval.v_ಸಂಖ್ಯೆ = tv_islocked(lv.ll_tv);
	    ಅನ್ಯಥಾ
		// Dictionary item.
		rettv->vval.v_ಸಂಖ್ಯೆ = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    ಯದಿ (ಬೇರು != ರಿಕ್ತ)
	free_lval_ಬೇರು(ಬೇರು);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ವಸ್ತುಗಳು(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    ಅನ್ಯಥಾ
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), 1);
}

/*
 * "keytrans()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_last_buffer_nr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ಯದಿ (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

/*
 * "len()" function
 */
    ಶೂನ್ಯ
f_len(typval_T *argvars, typval_T *rettv)
{
    ವಿಕಲ್ಪ (argvars[0].v_type)
    {
	ಪಕ್ಷ VAR_STRING:
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BLOB:
	    rettv->vval.v_ಸಂಖ್ಯೆ = blob_len(argvars[0].vval.v_blob);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:
	    rettv->vval.v_ಸಂಖ್ಯೆ = list_len(argvars[0].vval.v_list);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    rettv->vval.v_ಸಂಖ್ಯೆ = tuple_len(argvars[0].vval.v_tuple);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    rettv->vval.v_ಸಂಖ್ಯೆ = dict_len(argvars[0].vval.v_dict);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_OBJECT:
	    rettv->vval.v_ಸಂಖ್ಯೆ = object_len(argvars[0].vval.v_object);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	ಪಕ್ಷ VAR_FLOAT:
	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL:
	ಪಕ್ಷ VAR_JOB:
	ಪಕ್ಷ VAR_CHANNEL:
	ಪಕ್ಷ VAR_INSTR:
	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    ಭಙ್ಗ;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
libcall_common(typval_T *argvars UNUSED, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ type)
{
#ifdef FEAT_LIBCALL
    char_u		*string_in;
    char_u		**string_result;
    ಪೂರ್ಣಾಙ್ಕ			nr_result;
#endif

    rettv->v_type = type;
    ಯದಿ (type != VAR_ಸಂಖ್ಯೆ)
	rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    ಯದಿ (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_in = ರಿಕ್ತ;
	ಯದಿ (argvars[2].v_type == VAR_STRING)
	    string_in = argvars[2].vval.v_string;
	ಯದಿ (type == VAR_ಸಂಖ್ಯೆ)
	{
	    string_result = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    rettv->vval.v_string = ರಿಕ್ತ;
	    string_result = &rettv->vval.v_string;
	}
	ಯದಿ (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_in,
			     argvars[2].vval.v_ಸಂಖ್ಯೆ,
			     string_result,
			     &nr_result) == OK
		&& type == VAR_ಸಂಖ್ಯೆ)
	    rettv->vval.v_ಸಂಖ್ಯೆ = nr_result;
    }
#endif
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
    pos_T	*fp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		fnum;
    ಪೂರ್ಣಾಙ್ಕ		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	ಯದಿ (wp != ರಿಕ್ತ && tp != ರಿಕ್ತ)
	{
	    ಯದಿ (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		ಯದಿ (*p_spk != 'c'
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
    ಅನ್ಯಥಾ
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    ಯದಿ (fp != ರಿಕ್ತ)
	lnum = fp->lnum;
    rettv->vval.v_ಸಂಖ್ಯೆ = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_line2byte(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
#ಅನ್ಯಥಾ
    linenr_T	lnum;

    ಯದಿ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    lnum = tv_get_lnum(argvars);
    ಯದಿ (lnum < 1 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = ml_find_ಸಾಲು_or_offset(curbuf, lnum, ರಿಕ್ತ);
    ಯದಿ (rettv->vval.v_ಸಂಖ್ಯೆ >= 0)
	++rettv->vval.v_ಸಂಖ್ಯೆ;
#endif
}

#ifdef FEAT_LUA
/*
 * "luaeval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ
{
    MATCH_END,	    // matchend()
    MATCH_MATCH,    // match()
    MATCH_STR,	    // matchstr()
    MATCH_LIST,	    // matchlist()
    MATCH_POS	    // matchstrpos()
} matchtype_T;

    ಸ್ಥಿರ ಶೂನ್ಯ
find_some_match(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = ರಿಕ್ತ;
    long	len = 0;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ = ರಿಕ್ತ;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    long	start = 0;
    long	nth = 1;
    colnr_T	startcol = 0;
    ಪೂರ್ಣಾಙ್ಕ		match = 0;
    list_T	*l = ರಿಕ್ತ;
    listitem_T	*li = ರಿಕ್ತ;
    long	idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		prev_lock = 0;
    char_u	*tofree = ರಿಕ್ತ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಯದಿ (type == MATCH_LIST || type == MATCH_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	    ಗಚ್ಛ theend;
	ಯದಿ (type == MATCH_POS
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
		rettv->vval.v_list = ರಿಕ್ತ;
		ಗಚ್ಛ theend;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (type == MATCH_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ರಿಕ್ತ;
    }

    ಯದಿ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL)))
	ಗಚ್ಛ theend;

    ಯದಿ (argvars[0].v_type == VAR_LIST)
    {
	ಯದಿ ((l = argvars[0].vval.v_list) == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    ಅನ್ಯಥಾ
    {
	ಅಭಿವ್ಯಕ್ತಿ = str = tv_get_string(&argvars[0]);
	len = (long)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    ಯದಿ (pat == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಙ್ಕ	    error = FALSE;

	start = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಯದಿ (error)
	    ಗಚ್ಛ theend;
	ಯದಿ (l != ರಿಕ್ತ)
	{
	    li = list_find(l, start);
	    ಯದಿ (li == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (start < 0)
		start = 0;
	    ಯದಿ (start > len)
		ಗಚ್ಛ theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    ಯದಿ (argvars[3].v_type != VAR_UNKNOWN)
		startcol = start;
	    ಅನ್ಯಥಾ
	    {
		str += start;
		len -= start;
	    }
	}

	ಯದಿ (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[3], &error);
	ಯದಿ (error)
	    ಗಚ್ಛ theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    ಯದಿ (regmatch.regprog != ರಿಕ್ತ)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	ಯದಿ (l != ರಿಕ್ತ)
	{
	    prev_lock = l->lv_lock;
	    ಯದಿ (l->lv_lock == 0)
		l->lv_lock = VAR_LOCKED;
	}

	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (l != ರಿಕ್ತ)
	    {
		ಯದಿ (li == ರಿಕ್ತ)
		{
		    match = FALSE;
		    ಭಙ್ಗ;
		}
		vim_free(tofree);
		ಅಭಿವ್ಯಕ್ತಿ = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		ಯದಿ (str == ರಿಕ್ತ)
		    ಭಙ್ಗ;
	    }

	    match = vim_regexec_nl(&regmatch, str, startcol);

	    ಯದಿ (match && --nth <= 0)
		ಭಙ್ಗ;
	    ಯದಿ (l == ರಿಕ್ತ && !match)
		ಭಙ್ಗ;

	    // Advance to just after the match.
	    ಯದಿ (l != ರಿಕ್ತ)
	    {
		li = li->li_next;
		++idx;
	    }
	    ಅನ್ಯಥಾ
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		ಯದಿ (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    match = FALSE;
		    ಭಙ್ಗ;
		}
	    }
	}

	ಯದಿ (match)
	{
	    ಯದಿ (type == MATCH_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_first;
		listitem_T *li2 = li1->li_next;
		listitem_T *li3 = li2->li_next;
		listitem_T *li4 = li3->li_next;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_ಸಂಖ್ಯೆ =
				      (varnumber_T)(regmatch.startp[0] - ಅಭಿವ್ಯಕ್ತಿ);
		li4->li_tv.vval.v_ಸಂಖ್ಯೆ =
					(varnumber_T)(regmatch.endp[0] - ಅಭಿವ್ಯಕ್ತಿ);
		ಯದಿ (l != ರಿಕ್ತ)
		    li2->li_tv.vval.v_ಸಂಖ್ಯೆ = (varnumber_T)idx;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (type == MATCH_LIST)
	    {
		ಪೂರ್ಣಾಙ್ಕ i;

		// return list with matched string and submatches
		ಪ್ರತಿ (i = 0; i < NSUBEXP; ++i)
		{
		    ಯದಿ (regmatch.endp[i] == ರಿಕ್ತ)
		    {
			ಯದಿ (list_append_string(rettv->vval.v_list,
							    ರಿಕ್ತ, 0) == FAIL)
			    ಭಙ್ಗ;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(ಪೂರ್ಣಾಙ್ಕ)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			ಭಙ್ಗ;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (type == MATCH_STR)
	    {
		// return matched string
		ಯದಿ (l != ರಿಕ್ತ)
		    copy_tv(&li->li_tv, rettv);
		ಅನ್ಯಥಾ
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (l != ರಿಕ್ತ)
		rettv->vval.v_ಸಂಖ್ಯೆ = idx;
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (type != MATCH_END)
		    rettv->vval.v_ಸಂಖ್ಯೆ =
				      (varnumber_T)(regmatch.startp[0] - str);
		ಅನ್ಯಥಾ
		    rettv->vval.v_ಸಂಖ್ಯೆ =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_ಸಂಖ್ಯೆ += (varnumber_T)(str - ಅಭಿವ್ಯಕ್ತಿ);
	    }
	}
	ಯದಿ (l != ರಿಕ್ತ)
	    l->lv_lock = prev_lock;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    ಯದಿ (type == MATCH_POS && l == ರಿಕ್ತ && rettv->vval.v_list != ರಿಕ್ತ)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_matches_in_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    ಪೂರ್ಣಾಙ್ಕ		idx,
    ಪೂರ್ಣಾಙ್ಕ		submatches,
    ಪೂರ್ಣಾಙ್ಕ		matchbuf)
{
    long	len = (long)STRLEN(str);
    ಪೂರ್ಣಾಙ್ಕ		match = 0;
    colnr_T	startidx = 0;

    ಪ್ರತಿ (;;)
    {
	match = vim_regexec_nl(rmp, str, startidx);
	ಯದಿ (!match)
	    ಭಙ್ಗ;

	dict_T *d = dict_alloc();
	ಯದಿ (d == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (dict_add_ಸಂಖ್ಯೆ(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (dict_add_ಸಂಖ್ಯೆ(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (dict_add_string_len(d, "text", rmp->startp[0],
		    (ಪೂರ್ಣಾಙ್ಕ)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (submatches)
	{
	    list_T *sml = list_alloc();
	    ಯದಿ (sml == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    ಯದಿ (dict_add_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    // return a list with the submatches
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < NSUBEXP; ++i)
	    {
		ಯದಿ (rmp->endp[i] == ರಿಕ್ತ)
		{
		    ಯದಿ (list_append_string(sml, (char_u *)"", 0) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಅನ್ಯಥಾ ಯದಿ (list_append_string(sml, rmp->startp[i],
			    (ಪೂರ್ಣಾಙ್ಕ)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	ಯದಿ (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * "matchbufline()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ರಿಕ್ತ;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಯದಿ (rettv_list_alloc(rettv) != OK)
	ಪ್ರತ್ಯಾಗಮ;
    retlist = rettv->vval.v_list;

    ಯದಿ (check_for_buffer_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ prev_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    ಯದಿ (buf == ರಿಕ್ತ)
    {
	ಯದಿ (did_emsg == prev_did_emsg)
	    semsg(_(e_invalid_buffer_name_str), tv_get_string(&argvars[0]));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
    {
	emsg(_(e_buffer_is_not_loaded));
	ಪ್ರತ್ಯಾಗಮ;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    ಯದಿ (did_emsg > did_emsg_before)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (slnum < 1)
    {
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "lnum");
	ಪ್ರತ್ಯಾಗಮ;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    ಯದಿ (did_emsg > did_emsg_before)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "end_lnum");
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (elnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	elnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಪೂರ್ಣಾಙ್ಕ		submatches = FALSE;
    ಯದಿ (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	ಯದಿ (d != ರಿಕ್ತ)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    ಯದಿ (di != ರಿಕ್ತ)
	    {
		ಯದಿ (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "submatches");
		    ಪ್ರತ್ಯಾಗಮ;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
	ಗಚ್ಛ theend;
    regmatch.rm_ic = p_ic;

    ಯಾವತ್ (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	ಯದಿ (get_matches_in_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    ಗಚ್ಛ cleanup;
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
f_match(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_MATCH);
}

/*
 * "matchend()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_matchend(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_END);
}

/*
 * "matchlist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_LIST);
}

/*
 * "matchstr()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_STR);
}

/*
 * "matchstrlist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ರಿಕ್ತ;
    char_u	*save_cpo;
    list_T	*l = ರಿಕ್ತ;
    listitem_T	*li = ರಿಕ್ತ;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಯದಿ (rettv_list_alloc(rettv) != OK)
	ಪ್ರತ್ಯಾಗಮ;
    retlist = rettv->vval.v_list;

    ಯದಿ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((l = argvars[0].vval.v_list) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    ಯದಿ (pat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
	ಗಚ್ಛ theend;
    regmatch.rm_ic = p_ic;

    ಪೂರ್ಣಾಙ್ಕ		submatches = FALSE;
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	ಯದಿ (d != ರಿಕ್ತ)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    ಯದಿ (di != ರಿಕ್ತ)
	    {
		ಯದಿ (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "submatches");
		    ಗಚ್ಛ cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    ಪೂರ್ಣಾಙ್ಕ idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಯದಿ (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != ರಿಕ್ತ)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    ಯದಿ (get_matches_in_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		ಗಚ್ಛ cleanup;
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
    find_some_match(argvars, rettv, MATCH_POS);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
max_min(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ domax)
{
    varnumber_T	n = 0;

    ಯದಿ (in_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = 0;

    ಯದಿ (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = ರಿಕ್ತ;

	l = argvars[0].vval.v_list;
	ಯದಿ (l != ರಿಕ್ತ && l->lv_len > 0)
	{
	    ಯದಿ (l->lv_first == &range_list_item)
	    {
		ಯದಿ ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_start;
		ಅನ್ಯಥಾ
		    n = l->lv_u.nonmat.lv_start + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_ಸಂಖ್ಯೆ = n;
	    }
	    ಅನ್ಯಥಾ
	    {
		FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
		{
		    ಯದಿ (tv == ರಿಕ್ತ)
			tv = &li->li_tv;
		    ಅನ್ಯಥಾ
		    {
			ಪೂರ್ಣಾಙ್ಕ res;
			ಯದಿ (typval_compare2(&li->li_tv, tv,
				domax ? ಅಭಿವ್ಯಕ್ತಿ_GREATER : ಅಭಿವ್ಯಕ್ತಿ_SMALLER, FALSE, &res) == FAIL)
			    ಪ್ರತ್ಯಾಗಮ;
			ಯದಿ (res == OK)
			    tv = &li->li_tv;
		    }
		}

		ಯದಿ (tv != ರಿಕ್ತ)
		    copy_tv(tv, rettv);
	    }
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	ಯದಿ (tv != ರಿಕ್ತ)
	    copy_tv(tv, rettv);
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	ಪೂರ್ಣಾಙ್ಕ		todo;
	typval_T	*tv = ರಿಕ್ತ;

	d = argvars[0].vval.v_dict;
	ಯದಿ (d != ರಿಕ್ತ)
	{
	    todo = (ಪೂರ್ಣಾಙ್ಕ)d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d->dv_hashtab, hi, todo)
	    {
		ಯದಿ (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    ಯದಿ (tv == ರಿಕ್ತ)
			tv = &HI2DI(hi)->di_tv;
		    ಅನ್ಯಥಾ
		    {
			ಪೂರ್ಣಾಙ್ಕ res;
			ಯದಿ (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? ಅಭಿವ್ಯಕ್ತಿ_GREATER : ಅಭಿವ್ಯಕ್ತಿ_SMALLER, FALSE, &res) == FAIL)
			    ಪ್ರತ್ಯಾಗಮ;
			ಯದಿ (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	ಯದಿ (tv != ರಿಕ್ತ)
	    copy_tv(tv, rettv);
    }
    ಅನ್ಯಥಾ
	semsg(_(e_argument_of_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
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

#ಯದಿ defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ಯದಿ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	ಯದಿ (lnum < 0 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    lnum = 0;
	    ಭಙ್ಗ;
	}
	ಯದಿ (*skipwhite(ml_get(lnum)) != NUL)
	    ಭಙ್ಗ;
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = lnum;
}


/*
 * "ngettext()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#ಯದಿ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    ವರ್ಣ *prev = ರಿಕ್ತ;
#endif

    ಯದಿ (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_STRING;

    ಯದಿ (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != ರಿಕ್ತ &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#ಯದಿ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ಧ್ರುವ ವರ್ಣ *)argvars[3].vval.v_string, (ವರ್ಣ *)p_enc);
#endif

#ಯದಿ defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((ಧ್ರುವ ವರ್ಣ *)argvars[3].vval.v_string, (ಧ್ರುವ ವರ್ಣ *)argvars[0].vval.v_string, (ಧ್ರುವ ವರ್ಣ *)argvars[1].vval.v_string, (ಪೂರ್ಣಾಙ್ಕ)argvars[2].vval.v_ಸಂಖ್ಯೆ));
#ಅನ್ಯಥಾ
	textdomain((ಧ್ರುವ ವರ್ಣ *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ಧ್ರುವ ವರ್ಣ *)argvars[0].vval.v_string, (ಧ್ರುವ ವರ್ಣ *)argvars[1].vval.v_string, argvars[2].vval.v_ಸಂಖ್ಯೆ));
	textdomain(VIMPACKAGE);
#endif

#ಯದಿ defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	ಯದಿ (prev != ರಿಕ್ತ)
	    bind_textdomain_codeset((ಧ್ರುವ ವರ್ಣ *)argvars[3].vval.v_string, prev);
#endif
    }
    ಅನ್ಯಥಾ
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ಧ್ರುವ ವರ್ಣ *)argvars[0].vval.v_string, (ಧ್ರುವ ವರ್ಣ *)argvars[1].vval.v_string, argvars[2].vval.v_ಸಂಖ್ಯೆ));
}


/*
 * "nr2char()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (has_mbyte)
    {
	ಪೂರ್ಣಾಙ್ಕ	utf8 = 0;

	ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[1], ರಿಕ್ತ);
	ಯದಿ (utf8)
	    buf[utf_char2bytes((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]), buf)] = NUL;
	ಅನ್ಯಥಾ
	    buf[(*mb_char2bytes)((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]), buf)] = NUL;
    }
    ಅನ್ಯಥಾ
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
    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ)
					| tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ರಿಕ್ತ);
}

#ifdef FEAT_PERL
/*
 * "perleval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#endif

/*
 * "prevnonblank()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ಯದಿ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    lnum = tv_get_lnum(argvars);
    ಯದಿ (lnum < 1 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	lnum = 0;
    ಅನ್ಯಥಾ
	ಯಾವತ್ (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
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
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		saved_did_emsg = did_emsg;
    ವರ್ಣ	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script() && check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (ವರ್ಣ *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(ರಿಕ್ತ, 0, fmt, ap, argvars + 1);
    ಯದಿ (!did_emsg)
    {
	s = alloc(len + 1);
	ಯದಿ (s != ರಿಕ್ತ)
	{
	    rettv->vval.v_string = s;
	    (ಶೂನ್ಯ)vim_vsnprintf_typval((ವರ್ಣ *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_pum_getpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    pum_set_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_pumvisible(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ಯದಿ (pum_visible())
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
}

#ifdef FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (p_pyx == 0)
	p_pyx = 3;

    ಯದಿ (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ
    {
	locals = ರಿಕ್ತ;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#endif

#ifdef FEAT_PYTHON
/*
 * "pyeval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (p_pyx == 0)
	p_pyx = 2;

    ಯದಿ (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ
    {
	locals = ರಿಕ್ತ;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#endif

#ಯದಿ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

# ಯದಿ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ಯದಿ (p_pyx == 2)
	f_pyeval(argvars, rettv);
    ಅನ್ಯಥಾ
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#endif

ಸ್ಥಿರ UINT32_T srand_seed_for_testing = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	srand_seed_for_testing_is_used = FALSE;

    ಸ್ಥಿರ ಶೂನ್ಯ
f_test_srand_seed(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
	srand_seed_for_testing_is_used = FALSE;
    ಅನ್ಯಥಾ
    {
	srand_seed_for_testing = (UINT32_T)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
	srand_seed_for_testing_is_used = TRUE;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
init_srand(UINT32_T *x)
{
    ಸಂರಚನಾ {
	ಸಙ್ಘ {
	    UINT32_T ಸಂಖ್ಯೆ;
	    char_u   bytes[ಪರಿಮಾಣ(UINT32_T)];
	} contents;
    } buf;

    ಯದಿ (srand_seed_for_testing_is_used)
    {
	*x = srand_seed_for_testing;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (mch_get_random(buf.contents.bytes, ಪರಿಮಾಣ(buf.contents.bytes)) == OK)
    {
	*x = buf.contents.ಸಂಖ್ಯೆ;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#ಯದಿ defined(FEAT_SODIUM)
    ಯದಿ (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    ಅನ್ಯಥಾ
#endif
    {
#ಯದಿ defined(FEAT_RELTIME)
	proftime_T res;
	profile_start(&res);
# ಯದಿ defined(MSWIN)
	*x = (UINT32_T)res.ಕೆಳಗಿನ_Part;
# ಅನ್ಯಥಾ
	*x = (UINT32_T)res.tv_fsec;
# endif
#ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = ರಿಕ್ತ;
    ಸ್ಥಿರ UINT32_T	gx, gy, gz, gw;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, result;

    ಯದಿ (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// When no argument is given use the global seed list.
	ಯದಿ (initialized == FALSE)
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
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	ಯದಿ (l == ರಿಕ್ತ || list_len(l) != 4)
	    ಗಚ್ಛ theend;

	lx = list_find(l, 0L);
	ly = list_find(l, 1L);
	lz = list_find(l, 2L);
	lw = list_find(l, 3L);
	ಯದಿ (lx == ರಿಕ್ತ || ly == ರಿಕ್ತ || lz == ರಿಕ್ತ || lw == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	ಯದಿ (lx->li_tv.v_type != VAR_ಸಂಖ್ಯೆ
		|| ly->li_tv.v_type != VAR_ಸಂಖ್ಯೆ
		|| lz->li_tv.v_type != VAR_ಸಂಖ್ಯೆ
		|| lw->li_tv.v_type != VAR_ಸಂಖ್ಯೆ)
	    ಗಚ್ಛ theend;
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
    ಅನ್ಯಥಾ
	ಗಚ್ಛ theend;

    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)result;
    ಪ್ರತ್ಯಾಗಮ;

theend:
    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
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

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	    error = FALSE;

	x = (UINT32_T)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &error);
	ಯದಿ (error)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#undef ROTL
#undef SPLITMIX32
#undef SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	start;
    varnumber_T	end;
    varnumber_T	stride = 1;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    start = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &error);
    ಯದಿ (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = start - 1;
	start = 0;
    }
    ಅನ್ಯಥಾ
    {
	end = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
    }

    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ;		// type error; errmsg already given
    ಯದಿ (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	ಪ್ರತ್ಯಾಗಮ;
    }

    // The stride is stored in "lv_stride", which is an int.
    ಯದಿ (stride < INT_MIN || stride > INT_MAX)
    {
	ವರ್ಣ	buf[NUMBUFLEN];

	vim_snprintf(buf, ಪರಿಮಾಣ(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	ಪ್ರತ್ಯಾಗಮ;
    }

    uvarnumber_T	len;

    ಯದಿ (stride > 0 ? end < start : end > start)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	back = stride > 0
				  ? (uvarnumber_T)start - (uvarnumber_T)end
				  : (uvarnumber_T)end - (uvarnumber_T)start;

	ಯದಿ (back > 1)
	{
	    emsg(_(e_start_past_end));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	len = 0;
    }
    ಅನ್ಯಥಾ
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	span = stride > 0
				  ? (uvarnumber_T)end - (uvarnumber_T)start
				  : (uvarnumber_T)start - (uvarnumber_T)end;
	uvarnumber_T	ಎಣಿಕೆ = span / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	ಯದಿ (ಎಣಿಕೆ >= (uvarnumber_T)INT_MAX)
	{
	    ವರ್ಣ	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, ಪರಿಮಾಣ(buf), "%llu",
			      ಎಣಿಕೆ < UVARNUM_MAX ? ಎಣಿಕೆ + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	len = ಎಣಿಕೆ + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_first = &range_list_item;
    list->lv_u.nonmat.lv_start = start;
    list->lv_u.nonmat.lv_end = end;
    list->lv_u.nonmat.lv_stride = (ಪೂರ್ಣಾಙ್ಕ)stride;
    list->lv_len = (ಪೂರ್ಣಾಙ್ಕ)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    ಶೂನ್ಯ
range_list_materialize(list_T *list)
{
    varnumber_T start = list->lv_u.nonmat.lv_start;
    varnumber_T end = list->lv_u.nonmat.lv_end;
    ಪೂರ್ಣಾಙ್ಕ		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_first = ರಿಕ್ತ;
    list->lv_u.mat.lv_last = ರಿಕ್ತ;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_item = ರಿಕ್ತ;
    ಪ್ರತಿ (i = start; stride > 0 ? i <= end : i >= end; i += stride)
    {
	ಯದಿ (list_append_ಸಂಖ್ಯೆ(list, i) == FAIL)
	    ಭಙ್ಗ;
	ಯದಿ (list->lv_lock & VAR_ವಸ್ತುಗಳು_LOCKED)
	    list->lv_u.mat.lv_last->li_tv.v_lock = VAR_LOCKED;
    }
    list->lv_lock &= ~VAR_ವಸ್ತುಗಳು_LOCKED;
}

/*
 * "getreginfo()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		regname;
    char_u	buf[NUMBUFLEN + 2];
    size_t	buflen;
    long	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    ಯದಿ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    regname = getreg_get_regname(argvars);
    ಯದಿ (regname == 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (regname == '@')
	regname = '"';

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC | GREG_LIST);
    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (dict_add_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    ವಿಕಲ್ಪ (get_reg_type(regname, &reglen))
    {
	ಪಕ್ಷ MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ MBLOCK:
	    buflen = vim_snprintf_safelen((ವರ್ಣ *)buf, ಪರಿಮಾಣ(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    buf[0] = NUL;
	    buflen = 0;
	    ಭಙ್ಗ;
    }
    (ಶೂನ್ಯ)dict_add_string_len(dict, (ವರ್ಣ *)"regtype", buf, (ಪೂರ್ಣಾಙ್ಕ)buflen);

    buf[0] = get_register_name(get_unname_register());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    ಯದಿ (regname == '"')
	(ಶೂನ್ಯ)dict_add_string_len(dict, (ವರ್ಣ *)"points_to", buf, (ಪೂರ್ಣಾಙ್ಕ)buflen);
    ಅನ್ಯಥಾ
    {
	dictitem_T	*item = dictitem_alloc((char_u *)"isunnamed");

	ಯದಿ (item != ರಿಕ್ತ)
	{
	    item->di_tv.v_type = VAR_BOOL;
	    item->di_tv.vval.v_ಸಂಖ್ಯೆ = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (ಶೂನ್ಯ)dict_add(dict, item);
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
return_register(ಪೂರ್ಣಾಙ್ಕ regname, typval_T *rettv)
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
f_reg_executing(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_reg_recording(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ slen = list_len(l);

    ಯದಿ (rettv_list_alloc(rettv) == FAIL
	    || l == ರಿಕ್ತ
	    || n <= 0
	    || slen == 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_repeat_ಎಣಿಕೆ(slen, n) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯಾವತ್ (n-- > 0)
	ಯದಿ (list_extend(rettv->vval.v_list, l, ರಿಕ್ತ) == FAIL)
	    ಭಙ್ಗ;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		slen;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    ಯದಿ (rettv_blob_alloc(rettv) == FAIL
	    || blob == ರಿಕ್ತ
	    || n <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    slen = blob->bv_ga.ga_len;
    ಯದಿ (slen <= 0)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (check_repeat_ಎಣಿಕೆ(slen, n) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    len = slen * (ಪೂರ್ಣಾಙ್ಕ)n;

    ಯದಿ (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    ಪ್ರತಿ (i = 0; i < slen; ++i)
	ಯದಿ (blob_get(blob, i) != 0)
	    ಭಙ್ಗ;

    ಯದಿ (i == slen)
	// No need to copy since all bytes are already zero
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)n; ++i)
	blob_set_range(rettv->vval.v_blob,
		(long)i * slen, ((long)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    size_t	slen;
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*r;
    ಪೂರ್ಣಾಙ್ಕ		done;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    slen = STRLEN(p);
    ಯದಿ (slen == 0 || n <= 0)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (check_repeat_ಎಣಿಕೆ((varnumber_T)slen, n) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    len = (ಪೂರ್ಣಾಙ್ಕ)slen * (ಪೂರ್ಣಾಙ್ಕ)n;

    r = alloc((size_t)len + 1);
    ಯದಿ (r == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    mch_memmove(r, p, slen);
    done = (ಪೂರ್ಣಾಙ್ಕ)slen;
    ಯಾವತ್ (done < len)
    {
	ಪೂರ್ಣಾಙ್ಕ copy_len = done;

	ಯದಿ (copy_len > len - done)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    ಯದಿ (in_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    n = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    ಯದಿ (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_ಹುಡುಕು_arg(typval_T *varp, ಪೂರ್ಣಾಙ್ಕ *flagsp)
{
    ಪೂರ್ಣಾಙ್ಕ		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		mask;

    ಯದಿ (varp->v_type == VAR_UNKNOWN)
	ಪ್ರತ್ಯಾಗಮ FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    ಯದಿ (flags == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;		// type error; errmsg already given
    ಯಾವತ್ (*flags != NUL)
    {
	ವಿಕಲ್ಪ (*flags)
	{
	    ಪಕ್ಷ 'b': dir = BACKWARD; ಭಙ್ಗ;
	    ಪಕ್ಷ 'w': p_ws = TRUE; ಭಙ್ಗ;
	    ಪಕ್ಷ 'W': p_ws = FALSE; ಭಙ್ಗ;
	    ಸ್ವಭಾವ:  mask = 0;
		      ಯದಿ (flagsp != ರಿಕ್ತ)
			  ವಿಕಲ್ಪ (*flags)
			  {
			      ಪಕ್ಷ 'c': mask = SP_START; ಭಙ್ಗ;
			      ಪಕ್ಷ 'e': mask = SP_END; ಭಙ್ಗ;
			      ಪಕ್ಷ 'm': mask = SP_RETCOUNT; ಭಙ್ಗ;
			      ಪಕ್ಷ 'n': mask = SP_NOMOVE; ಭಙ್ಗ;
			      ಪಕ್ಷ 'p': mask = SP_SUBPAT; ಭಙ್ಗ;
			      ಪಕ್ಷ 'r': mask = SP_REPEAT; ಭಙ್ಗ;
			      ಪಕ್ಷ 's': mask = SP_SETPCMARK; ಭಙ್ಗ;
			      ಪಕ್ಷ 'z': mask = SP_COLUMN; ಭಙ್ಗ;
			  }
		      ಯದಿ (mask == 0)
		      {
			  semsg(_(e_invalid_argument_str), flags);
			  dir = 0;
		      }
		      ಅನ್ಯಥಾ
			  *flagsp |= mask;
	}
	ಯದಿ (dir == 0)
	    ಭಙ್ಗ;
	++flags;
    }
    ಪ್ರತ್ಯಾಗಮ dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ಹುಡುಕು_cmn(typval_T *argvars, pos_T *match_pos, ಪೂರ್ಣಾಙ್ಕ *flagsp)
{
    ಪೂರ್ಣಾಙ್ಕ		flags;
    char_u	*pat;
    size_t	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    ಪೂರ್ಣಾಙ್ಕ		save_p_ws = p_ws;
    ಪೂರ್ಣಾಙ್ಕ		dir;
    ಪೂರ್ಣಾಙ್ಕ		retval = 0;	// default: FAIL
    long	lnum_stop = 0;
#ifdef FEAT_RELTIME
    long	time_limit = 0;
#endif
    ಪೂರ್ಣಾಙ್ಕ		options = ಹುಡುಕು_KEEP;
    ಪೂರ್ಣಾಙ್ಕ		subpatnum;
    searchit_arg_T sia;
    ಪೂರ್ಣಾಙ್ಕ		use_skip = FALSE;
    pos_T	firstpos;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL)))))
	ಗಚ್ಛ theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_ಹುಡುಕು_arg(&argvars[1], flagsp);	// may set p_ws
    ಯದಿ (dir == 0)
	ಗಚ್ಛ theend;
    flags = *flagsp;
    ಯದಿ (flags & SP_START)
	options |= ಹುಡುಕು_START;
    ಯದಿ (flags & SP_END)
	options |= ಹುಡುಕು_END;
    ಯದಿ (flags & SP_COLUMN)
	options |= ಹುಡುಕು_COL;

    // Optional arguments: line number to stop searching, timeout and skip.
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], ರಿಕ್ತ);
	ಯದಿ (lnum_stop < 0)
	    ಗಚ್ಛ theend;
	ಯದಿ (argvars[3].v_type != VAR_UNKNOWN)
	{
#ifdef FEAT_RELTIME
	    time_limit = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[3], ರಿಕ್ತ);
	    ಯದಿ (time_limit < 0)
		ಗಚ್ಛ theend;
#endif
	    use_skip = eval_ಅಭಿವ್ಯಕ್ತಿ_valid_arg(&argvars[4]);
	}
    }

    /*
     * This function does not accept SP_REPEAT and SP_RETCOUNT flags.
     * Check to make sure only those flags are set.
     * Also, Only the SP_NOMOVE or the SP_SETPCMARK flag can be set. Both
     * flags cannot be set. Check for that condition also.
     */
    ಯದಿ (((flags & (SP_REPEAT | SP_RETCOUNT)) != 0)
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	ಗಚ್ಛ theend;
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
    ಪ್ರತಿ (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, ರಿಕ್ತ, dir, pat, patlen, 1L,
						     options, RE_ಹುಡುಕು, &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	ಯದಿ (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	ಯದಿ (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    ಭಙ್ಗ;
	ಯದಿ (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    ಪೂರ್ಣಾಙ್ಕ	    do_skip;
	    ಪೂರ್ಣಾಙ್ಕ	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_ಅಭಿವ್ಯಕ್ತಿ_to_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    ಯದಿ (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (!do_skip)
		ಭಙ್ಗ;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~ಹುಡುಕು_START;
    }

    ಯದಿ (subpatnum != FAIL)
    {
	ಯದಿ (flags & SP_SUBPAT)
	    retval = subpatnum;
	ಅನ್ಯಥಾ
	    retval = pos.lnum;
	ಯದಿ (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	ಯದಿ (match_pos != ರಿಕ್ತ)
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
    ಯದಿ (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    ಅನ್ಯಥಾ
	curwin->w_set_curswant = true;
theend:
    p_ws = save_p_ws;

    ಪ್ರತ್ಯಾಗಮ retval;
}

#ifdef FEAT_RUBY
/*
 * "rubyeval()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#endif

/*
 * "screenattr()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		row;
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    row = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ) - 1;
    col = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ರಿಕ್ತ) - 1;
    ಯದಿ (row < 0 || row >= screen_Rows
	    || col < 0 || col >= screen_Columns)
	c = -1;
    ಅನ್ಯಥಾ
	c = ScreenAttrs[ಸಾಲು_Offset[row] + col];
    rettv->vval.v_ಸಂಖ್ಯೆ = c;
}

/*
 * "screenchar()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		row;
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    row = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ) - 1;
    col = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ರಿಕ್ತ) - 1;
    ಯದಿ (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	c = -1;
    ಅನ್ಯಥಾ
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(row, col, buf, ರಿಕ್ತ);
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
    ಪೂರ್ಣಾಙ್ಕ		row;
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    row = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ) - 1;
    col = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ರಿಕ್ತ) - 1;
    ಯದಿ (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	ಪ್ರತ್ಯಾಗಮ;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(row, col, buf, ರಿಕ್ತ);
    ಪೂರ್ಣಾಙ್ಕ pcc[MAX_MCO];
    ಯದಿ (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    ಅನ್ಯಥಾ
	c = (*mb_ptr2char)(buf);
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)c);

    ಯದಿ (enc_utf8)
	ಪ್ರತಿ (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_screencol(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_screenrow(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		row;
    ಪೂರ್ಣಾಙ್ಕ		col;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = ರಿಕ್ತ;
    rettv->v_type = VAR_STRING;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    row = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ) - 1;
    col = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ರಿಕ್ತ) - 1;
    ಯದಿ (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	ಪ್ರತ್ಯಾಗಮ;

    screen_getbytes(row, col, buf, ರಿಕ್ತ);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_ಹುಡುಕು(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;

    rettv->vval.v_ಸಂಖ್ಯೆ = ಹುಡುಕು_cmn(argvars, ರಿಕ್ತ, &flags);
}

/*
 * "searchdecl()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		locally = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		thisblock = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    char_u	*name;

    rettv->vval.v_ಸಂಖ್ಯೆ = 1;	// default: FAIL

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string_chk(&argvars[0]);
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[1], &error);
	ಯದಿ (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[2], &error);
    }
    ಯದಿ (!error && name != ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = find_decl(name, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(name),
				     locally, thisblock, ಹುಡುಕು_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
searchpair_cmn(typval_T *argvars, pos_T *match_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    ಪೂರ್ಣಾಙ್ಕ		save_p_ws = p_ws;
    ಪೂರ್ಣಾಙ್ಕ		dir;
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		retval = 0;		// default: FAIL
    long	lnum_stop = 0;
    long	time_limit = 0;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 6) == FAIL)))))
	ಗಚ್ಛ theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    ಯದಿ (spat == ರಿಕ್ತ || mpat == ರಿಕ್ತ || epat == ರಿಕ್ತ)
	ಗಚ್ಛ theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_ಹುಡುಕು_arg(&argvars[3], &flags); // may set p_ws
    ಯದಿ (dir == 0)
	ಗಚ್ಛ theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    ಯದಿ ((flags & (SP_END | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[3]));
	ಗಚ್ಛ theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    ಯದಿ (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    ಯದಿ (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	// Type is checked later.
	skip = &argvars[4];

	ಯದಿ (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[5], ರಿಕ್ತ);
	    ಯದಿ (lnum_stop < 0)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[5]));
		ಗಚ್ಛ theend;
	    }
#ifdef FEAT_RELTIME
	    ಯದಿ (argvars[6].v_type != VAR_UNKNOWN)
	    {
		time_limit = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[6], ರಿಕ್ತ);
		ಯದಿ (time_limit < 0)
		{
		    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[6]));
		    ಗಚ್ಛ theend;
		}
	    }
#endif
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    match_pos, lnum_stop, time_limit);

theend:
    p_ws = save_p_ws;

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * "searchpair()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = searchpair_cmn(argvars, ರಿಕ್ತ);
}

/*
 * "searchpairpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    ಪೂರ್ಣಾಙ್ಕ		lnum = 0;
    ಪೂರ್ಣಾಙ್ಕ		col = 0;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (searchpair_cmn(argvars, &match_pos) > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)col);
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
    ಪೂರ್ಣಾಙ್ಕ		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    ಪೂರ್ಣಾಙ್ಕ		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*match_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    long	time_limit UNUSED) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = ರಿಕ್ತ, *pat3 = ರಿಕ್ತ;
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
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		r;
    ಪೂರ್ಣಾಙ್ಕ		nest = 1;
    ಪೂರ್ಣಾಙ್ಕ		use_skip = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		err;
    ಪೂರ್ಣಾಙ್ಕ		options = ಹುಡುಕು_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    ಯದಿ (pat2 == ರಿಕ್ತ)
	ಗಚ್ಛ theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    ಯದಿ (pat3 == ರಿಕ್ತ)
	ಗಚ್ಛ theend;
    pat2len = vim_snprintf((ವರ್ಣ *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    ಯದಿ (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    ಅನ್ಯಥಾ
	pat3len = vim_snprintf((ವರ್ಣ *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    ಯದಿ (flags & SP_START)
	options |= ಹುಡುಕು_START;

    ಯದಿ (skip != ರಿಕ್ತ)
	use_skip = eval_ಅಭಿವ್ಯಕ್ತಿ_valid_arg(skip);

#ifdef FEAT_RELTIME
    ಯದಿ (time_limit > 0)
	init_regexp_timeout(time_limit);
#endif
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    ಪ್ರತಿ (;;)
    {
	searchit_arg_T sia;

	CLEAR_FIELD(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, ರಿಕ್ತ, dir, pat, patlen, 1L,
						     options, RE_ಹುಡುಕು, &sia);
	ಯದಿ (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    ಭಙ್ಗ;

	ಯದಿ (firstpos.lnum == 0)
	    firstpos = pos;
	ಯದಿ (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    ಯದಿ (dir == BACKWARD)
		decl(&pos);
	    ಅನ್ಯಥಾ
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~ಹುಡುಕು_START;

	// If the skip pattern matches, ignore this match.
	ಯದಿ (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_ಅಭಿವ್ಯಕ್ತಿ_to_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    ಯದಿ (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (r)
		ಅನುವರ್ತ;
	}

	ಯದಿ ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	ಅನ್ಯಥಾ
	{
	    // Found end when searching forward or start when searching
	    // backward: end of (nested) pair; or found middle in outer pair.
	    ಯದಿ (--nest == 1)
		pat = pat3;	// outer level, search for middle
	}

	ಯದಿ (nest == 0)
	{
	    // Found the match: return matchcount or line number.
	    ಯದಿ (flags & SP_RETCOUNT)
		++retval;
	    ಅನ್ಯಥಾ
		retval = pos.lnum;
	    ಯದಿ (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    ಯದಿ (!(flags & SP_REPEAT))
		ಭಙ್ಗ;
	    nest = 1;	    // search for next unmatched
	}
    }

    ಯದಿ (match_pos != ರಿಕ್ತ)
    {
	// Store the match cursor position
	match_pos->lnum = curwin->w_cursor.lnum;
	match_pos->col = curwin->w_cursor.col + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    ಯದಿ ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#ifdef FEAT_RELTIME
    ಯದಿ (time_limit > 0)
	disable_regexp_timeout();
#endif
    vim_free(pat2);
    vim_free(pat3);
    ಯದಿ (p_cpo == empty_option)
	p_cpo = save_cpo;
    ಅನ್ಯಥಾ
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	ಯದಿ (*p_cpo == NUL)
	    set_option_ಮೌಲ್ಯ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * "searchpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    ಪೂರ್ಣಾಙ್ಕ		lnum = 0;
    ಪೂರ್ಣಾಙ್ಕ		col = 0;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    n = ಹುಡುಕು_cmn(argvars, &match_pos, &flags);
    ಯದಿ (n > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)col);
    ಯದಿ (flags & SP_SUBPAT)
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_position(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ charpos)
{
    pos_T	pos;
    ಪೂರ್ಣಾಙ್ಕ		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string_chk(argvars);
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (pos.col != MAXCOL && --pos.col < 0)
	pos.col = 0;
    ಯದಿ ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	ಯದಿ (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_set_curswant = false;
	}
	check_cursor();
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	ಯದಿ (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ಅನ್ಯಥಾ
	emsg(_(e_invalid_argument));
}
/*
 * "setcharpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
f_setcharsearch(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    ಯದಿ (check_for_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((d = argvars[0].vval.v_dict) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    csearch = dict_get_string(d, "char", FALSE);
    ಯದಿ (csearch != ರಿಕ್ತ)
    {
	ಯದಿ (enc_utf8)
	{
	    ಪೂರ್ಣಾಙ್ಕ pcc[MAX_MCO];
	    ಪೂರ್ಣಾಙ್ಕ c = utfc_ptr2char(csearch, pcc);

	    set_last_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	ಅನ್ಯಥಾ
	    set_last_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_find(d, (char_u *)"forward", -1);
    ಯದಿ (di != ರಿಕ್ತ)
	set_csearch_direction((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_find(d, (char_u *)"until", -1);
    ಯದಿ (di != ರಿಕ್ತ)
	set_csearch_until(!!tv_get_ಸಂಖ್ಯೆ(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_setenv(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string_buf(&argvars[0], namebuf);
    ಯದಿ (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_ಸಂಖ್ಯೆ == VVAL_NULL)
	vim_unsetenv_ext(name);
    ಅನ್ಯಥಾ
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
    char_u	*mode_str;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		mask;
    ಪೂರ್ಣಾಙ್ಕ		mode = 0;

    rettv->vval.v_ಸಂಖ್ಯೆ = 0;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    fname = tv_get_string_chk(&argvars[0]);
    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    mode_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    ಯದಿ (mode_str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (STRLEN(mode_str) != 9)
    {
	semsg(_(e_invalid_argument_str), mode_str);
	ಪ್ರತ್ಯಾಗಮ;
    }

    mask = 1;
    ಪ್ರತಿ (i = 8; i >= 0; --i)
    {
	ಯದಿ (mode_str[i] != '-')
	    mode |= mask;
	mask = mask << 1;
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = mch_setperm(fname, mode) == OK;
}

/*
 * "setpos()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_setpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    ಪೂರ್ಣಾಙ್ಕ
get_yank_type(char_u **pp, char_u *yank_type, long *block_len)
{
    char_u *stropt = *pp;
    ವಿಕಲ್ಪ (*stropt)
    {
	ಪಕ್ಷ 'v': ಪಕ್ಷ 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    ಭಙ್ಗ;
	ಪಕ್ಷ 'V': ಪಕ್ಷ 'l':	// line-wise selection
	    *yank_type = MLINE;
	    ಭಙ್ಗ;
	ಪಕ್ಷ 'b': ಪಕ್ಷ Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    ಯದಿ (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*block_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    *pp = stropt;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * "setreg()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_setreg(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    ಪೂರ್ಣಾಙ್ಕ		append;
    char_u	yank_type;
    long	block_len;
    typval_T	*regcontents;
    ಪೂರ್ಣಾಙ್ಕ		pointreg;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    pointreg = 0;
    regcontents = ರಿಕ್ತ;
    block_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_ಸಂಖ್ಯೆ = 1;		// FAIL is default

    ಯದಿ (strregname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;		// type error; errmsg already given
    ಯದಿ (in_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_register_name_must_be_one_char_str), strregname);
	ಪ್ರತ್ಯಾಗಮ;
    }
    regname = *strregname;
    ಯದಿ (regname == 0 || regname == '@')
	regname = '"';

    ಯದಿ (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	ಯದಿ (d == ರಿಕ್ತ || d->dv_hashtab.ht_used == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {ರಿಕ್ತ, ರಿಕ್ತ};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	di = dict_find(d, (char_u *)"regcontents", -1);
	ಯದಿ (di != ರಿಕ್ತ)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	ಯದಿ (stropt != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ ret = get_yank_type(&stropt, &yank_type, &block_len);

	    ಯದಿ (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "value");
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}

	ಯದಿ (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    ಯದಿ (stropt != ರಿಕ್ತ)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    ಅನ್ಯಥಾ
	regcontents = &argvars[1];

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಯದಿ (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    ಪ್ರತ್ಯಾಗಮ;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	ಯದಿ (stropt == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;		// type error
	ಪ್ರತಿ (; *stropt != NUL; ++stropt)
	    ವಿಕಲ್ಪ (*stropt)
	    {
		ಪಕ್ಷ 'a': ಪಕ್ಷ 'A':	// append
		    append = TRUE;
		    ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		    get_yank_type(&stropt, &yank_type, &block_len);
	    }
    }

    ಯದಿ (regcontents && regcontents->v_type == VAR_LIST)
    {
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	list_T		*ll = regcontents->vval.v_list;
	listitem_T	*li;
	ಪೂರ್ಣಾಙ್ಕ		len;

	// If the list is NULL handle like an empty list.
	len = ll == ರಿಕ್ತ ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	ಯದಿ (lstval == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	ಯದಿ (ll != ರಿಕ್ತ)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_ವಸ್ತುಗಳು(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		ಯದಿ (strval == ರಿಕ್ತ)
		    ಗಚ್ಛ free_lstval;
		ಯದಿ (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    ಯದಿ (strval == ರಿಕ್ತ)
			ಗಚ್ಛ free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ರಿಕ್ತ;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, block_len);
free_lstval:
	ಯಾವತ್ (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    ಅನ್ಯಥಾ ಯದಿ (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	ಯದಿ (strval == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, block_len);
    }
    ಯದಿ (pointreg != 0)
	get_yank_register(pointreg, TRUE);

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
    ಪೂರ್ಣಾಙ್ಕ		action = 'r';

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    // first argument: window number or id
    wp = find_win_by_nr_or_id(&argvars[0]);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // second argument: dict with items to set in the tag stack
    ಯದಿ (check_for_dict_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    d = argvars[1].vval.v_dict;
    ಯದಿ (d == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    ಯದಿ (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    ಅನ್ಯಥಾ ಯದಿ (check_for_string_arg(argvars, 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಅನ್ಯಥಾ
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	ಯದಿ (actstr == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	ಅನ್ಯಥಾ
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    ಯದಿ (set_tagstack(wp, d, action) == OK)
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
}

#ifdef FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (in_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != ರಿಕ್ತ ? (char_u *)blob->bv_ga.ga_data : (char_u *)"";
	len = blob != ರಿಕ್ತ ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_bytes(p, len, ರಿಕ್ತ, 0));
    }
    ಅನ್ಯಥಾ
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_bytes(p, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p), ರಿಕ್ತ, 0));
    }
}
#endif // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ do_special;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_shiftwidth(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = 0;

    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	long	col;

	col = (long)tv_get_ಸಂಖ್ಯೆ_chk(argvars, ರಿಕ್ತ);
	ಯದಿ (col < 0)
	    ಪ್ರತ್ಯಾಗಮ;	// type error; errmsg already given
#ifdef FEAT_VARTABS
	rettv->vval.v_ಸಂಖ್ಯೆ = get_sw_ಮೌಲ್ಯ_col(curbuf, col, FALSE);
	ಪ್ರತ್ಯಾಗಮ;
#endif
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

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#ifdef FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#ಅನ್ಯಥಾ
    rettv->vval.v_string = vim_strsave(s);
#endif
}

/*
 * "spellbadword()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_spellbadword(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*word = (char_u *)"";
    hlf_T	attr = HLF_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
#ifdef FEAT_SPELL
    ಪೂರ್ಣಾಙ್ಕ		wo_spell_save = curwin->w_p_spell;

    ಯದಿ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    ಯದಿ (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	ಪ್ರತ್ಯಾಗಮ;
    }

#ifdef FEAT_SPELL
    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_to(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	ಯದಿ (len != 0)
	{
	    word = ml_get_cursor();
	    curwin->w_set_curswant = true;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	ಪೂರ್ಣಾಙ್ಕ	capcol = -1;

	ಯದಿ (str != ರಿಕ್ತ)
	{
	    // Check the argument for spelling.
	    ಯಾವತ್ (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		ಯದಿ (attr != HLF_ಎಣಿಕೆ)
		{
		    word = str;
		    ಭಙ್ಗ;
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
    ವಿಕಲ್ಪ (attr)
    {
    ಪಕ್ಷ HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	ಭಙ್ಗ;

    ಪಕ್ಷ HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	ಭಙ್ಗ;

    ಪಕ್ಷ HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	ಭಙ್ಗ;

    ಪಕ್ಷ HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	ಭಙ್ಗ;

    ಸ್ವಭಾವ:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	ಭಙ್ಗ;
    }
}

/*
 * "spellsuggest()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_spellsuggest(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SPELL
    char_u	*str;
    ಪೂರ್ಣಾಙ್ಕ		typeerr = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		maxcount;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		i;
    listitem_T	*li;
    ಪೂರ್ಣಾಙ್ಕ		need_capital = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		wo_spell_save = curwin->w_p_spell;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    ಯದಿ (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	ಪ್ರತ್ಯಾಗಮ;
    }

#ifdef FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &typeerr);
	ಯದಿ (maxcount <= 0)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[2], &typeerr);
	    ಯದಿ (typeerr)
		ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಅನ್ಯಥಾ
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    ಪ್ರತಿ (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_data)[i];

	li = listitem_alloc();
	ಯದಿ (li == ರಿಕ್ತ)
	    vim_free(str);
	ಅನ್ಯಥಾ
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

    ಸ್ಥಿರ ಶೂನ್ಯ
f_split(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*end;
    char_u	*pat = ರಿಕ್ತ;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    ಪೂರ್ಣಾಙ್ಕ		match;
    colnr_T	col = 0;
    ಪೂರ್ಣಾಙ್ಕ		keepempty = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		typeerr = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    str = tv_get_string(&argvars[0]);
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	ಯದಿ (pat == ರಿಕ್ತ)
	    typeerr = TRUE;
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    ಯದಿ (pat == ರಿಕ್ತ || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಗಚ್ಛ theend;
    ಯದಿ (typeerr)
	ಗಚ್ಛ theend;

    regmatch.regprog = eval_regcomp(pat);
    ಯದಿ (regmatch.regprog != ರಿಕ್ತ)
    {
	regmatch.rm_ic = FALSE;
	ಯಾವತ್ (*str != NUL || keepempty)
	{
	    ಯದಿ (*str == NUL)
		match = FALSE;	// empty item at the end
	    ಅನ್ಯಥಾ
		match = vim_regexec_nl(&regmatch, str, col);
	    ಯದಿ (match)
		end = regmatch.startp[0];
	    ಅನ್ಯಥಾ
		end = str + STRLEN(str);
	    ಯದಿ (keepempty || end > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && match && end < regmatch.endp[0]))
	    {
		ಯದಿ (list_append_string(rettv->vval.v_list, str,
						    (ಪೂರ್ಣಾಙ್ಕ)(end - str)) == FAIL)
		    ಭಙ್ಗ;
	    }
	    ಯದಿ (!match)
		ಭಙ್ಗ;
	    // Advance to just after the match.
	    ಯದಿ (regmatch.endp[0] > str)
		col = 0;
	    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
f_submatch(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		no;
    ಪೂರ್ಣಾಙ್ಕ		retList = 0;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    no = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_ಸಂಖ್ಯೆ_nr), no);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	retList = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[1], &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    ಅನ್ಯಥಾ
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
    char_u	*sub = ರಿಕ್ತ;
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ = ರಿಕ್ತ;
    char_u	*flg;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    ಯದಿ (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	ಅಭಿವ್ಯಕ್ತಿ = &argvars[2];
    ಅನ್ಯಥಾ
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    ಯದಿ (str == ರಿಕ್ತ || pat == ರಿಕ್ತ || (sub == ರಿಕ್ತ && ಅಭಿವ್ಯಕ್ತಿ == ರಿಕ್ತ)
								|| flg == ರಿಕ್ತ)
	rettv->vval.v_string = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, ಅಭಿವ್ಯಕ್ತಿ, flg, ರಿಕ್ತ);
}

/*
 * "swapfilelist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_swapfilelist(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    recover_names(ರಿಕ್ತ, FALSE, rettv->vval.v_list, 0, ರಿಕ್ತ);
}

/*
 * "swapinfo(swap_filename)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (rettv_dict_alloc(rettv) == OK)
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

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ಯದಿ (buf == ರಿಕ್ತ || buf->b_ml.ml_mfp == ರಿಕ್ತ
					|| buf->b_ml.ml_mfp->mf_fname == ರಿಕ್ತ)
	rettv->vval.v_string = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_synID(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		id = 0;
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    ಪೂರ್ಣಾಙ್ಕ		trans;
    ಪೂರ್ಣಾಙ್ಕ		transerr = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (linenr_T)tv_get_ಸಂಖ್ಯೆ(&argvars[1]) - 1;	// -1 on type error
    trans = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[2], &transerr);

    ಯದಿ (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && col >= 0 && col < (long)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, col, trans, ರಿಕ್ತ, FALSE);
#endif

    rettv->vval.v_ಸಂಖ್ಯೆ = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_synIDattr(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*p = ರಿಕ್ತ;
#ifdef FEAT_SYN_HL
    ಪೂರ್ಣಾಙ್ಕ		id;
    char_u	*what;
    char_u	*mode;
    char_u	modebuf[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		modec;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	mode = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(mode[0]);
	ಯದಿ (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    ಅನ್ಯಥಾ
    {
# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	ಯದಿ (USE_24BIT)
	    modec = 'g';
	ಅನ್ಯಥಾ
# endif
	    ಯದಿ (t_colors > 1)
		modec = 'c';
	    ಅನ್ಯಥಾ
		modec = 't';
    }

    ವಿಕಲ್ಪ (TOLOWER_ASC(what[0]))
    {
	ಪಕ್ಷ 'b':
		ಯದಿ (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		ಅನ್ಯಥಾ					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		ಭಙ್ಗ;

	ಪಕ್ಷ 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		ಭಙ್ಗ;

	ಪಕ್ಷ 'i':
		ಯದಿ (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		ಅನ್ಯಥಾ					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		ಭಙ್ಗ;

	ಪಕ್ಷ 'n':
		ಯದಿ (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		ಅನ್ಯಥಾ					// name
		    p = get_highlight_name_ext(ರಿಕ್ತ, id - 1, FALSE);
		ಭಙ್ಗ;

	ಪಕ್ಷ 'r':					// reverse
		p = highlight_has_attr(id, HL_INVERSE, modec);
		ಭಙ್ಗ;

	ಪಕ್ಷ 's':
		ಯದಿ (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		ಅನ್ಯಥಾ ಯದಿ (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		ಅನ್ಯಥಾ					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		ಭಙ್ಗ;

	ಪಕ್ಷ 'u':
		ಯದಿ (STRLEN(what) >= 9)
		{
		    ಯದಿ (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    ಅನ್ಯಥಾ ಯದಿ (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    ಅನ್ಯಥಾ ಯದಿ (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    ಅನ್ಯಥಾ ಯದಿ (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    ಅನ್ಯಥಾ
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		ಅನ್ಯಥಾ
							// ul
		    p = highlight_color(id, what, modec);
		ಭಙ್ಗ;
    }

    ಯದಿ (p != ರಿಕ್ತ)
	p = vim_strsave(p);
#endif
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_synIDtrans(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		id;

#ifdef FEAT_SYN_HL
    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);

    ಯದಿ (id > 0)
	id = syn_get_final_id(id);
    ಅನ್ಯಥಾ
#endif
	id = 0;

    rettv->vval.v_ಸಂಖ್ಯೆ = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_synconcealed(typval_T *argvars UNUSED, typval_T *rettv)
{
#ಯದಿ defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	col;
    ಪೂರ್ಣಾಙ್ಕ		syntax_flags = 0;
    ಪೂರ್ಣಾಙ್ಕ		cchar;
    ಪೂರ್ಣಾಙ್ಕ		matchid = 0;
    char_u	str[NUMBUFLEN];
#endif

    rettv_list_set(rettv, ರಿಕ್ತ);

    ಯದಿ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

#ಯದಿ defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_ಸಂಖ್ಯೆ(&argvars[1]) - 1;	// -1 on type error

    CLEAR_FIELD(str);

    ಯದಿ (rettv_list_alloc(rettv) == OK)
    {
	ಯದಿ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (ಶೂನ್ಯ)syn_get_id(curwin, lnum, col, FALSE, ರಿಕ್ತ, FALSE);
	    syntax_flags = get_syntax_info(&matchid);

	    // get the conceal character
	    ಯದಿ ((syntax_flags & HL_CONCEAL) && curwin->w_p_cole < 3)
	    {
		cchar = syn_get_sub_char();
		ಯದಿ (cchar == NUL && curwin->w_p_cole == 1)
		    cchar = (curwin->w_lcs_chars.conceal == NUL) ? ' '
					: curwin->w_lcs_chars.conceal;
		ಯದಿ (cchar != NUL)
		{
		    ಯದಿ (has_mbyte)
			(*mb_char2bytes)(cchar, str);
		    ಅನ್ಯಥಾ
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
#endif
}

/*
 * "synstack(lnum, col)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_synstack(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		id;
#endif

    rettv_list_set(rettv, ರಿಕ್ತ);

    ಯದಿ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_ಸಂಖ್ಯೆ(&argvars[1]) - 1;	// -1 on type error

    ಯದಿ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(ಶೂನ್ಯ)syn_get_id(curwin, lnum, col, FALSE, ರಿಕ್ತ, TRUE);
	ಪ್ರತಿ (i = 0; ; ++i)
	{
	    id = syn_get_ರಾಶಿ_item(i);
	    ಯದಿ (id < 0)
		ಭಙ್ಗ;
	    ಯದಿ (list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, id) == FAIL)
		ಭಙ್ಗ;
	}
    }
#endif
}

/*
 * "tabpagebuflist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_tabpagebuflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp = ರಿಕ್ತ;

    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    ಅನ್ಯಥಾ
    {
	tp = find_tabpage((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
	ಯದಿ (tp != ರಿಕ್ತ)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    ಯದಿ (wp != ರಿಕ್ತ && rettv_list_alloc(rettv) == OK)
    {
	ಪ್ರತಿ (; wp != ರಿಕ್ತ; wp = wp->w_next)
	    ಯದಿ (list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list,
						wp->w_buffer->b_fnum) == FAIL)
		ಭಙ್ಗ;
    }
}

/*
 * "tagfiles()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_tagfiles(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    ಪೂರ್ಣಾಙ್ಕ		first;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    fname = alloc(MAXPATHL);
    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (first = TRUE; ; first = FALSE)
	ಯದಿ (get_tagfname(&tn, first, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    ಭಙ್ಗ;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = ರಿಕ್ತ;
    char_u  *tag_pattern;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    tag_pattern = tv_get_string(&argvars[0]);

    rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;
    ಯದಿ (*tag_pattern == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    ಯದಿ (rettv_list_alloc(rettv) == OK)
	(ಶೂನ್ಯ)get_tags(rettv->vval.v_list, tag_pattern, fname);
}

/*
 * "type(expr)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_type(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ n = -1;

    ವಿಕಲ್ಪ (argvars[0].v_type)
    {
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:  n = VAR_TYPE_ಸಂಖ್ಯೆ; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_STRING:  n = VAR_TYPE_STRING; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_PARTIAL:
	ಪಕ್ಷ VAR_FUNC:    n = VAR_TYPE_FUNC; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:    n = VAR_TYPE_LIST; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:   n = VAR_TYPE_TUPLE; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:    n = VAR_TYPE_DICT; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_FLOAT:   n = VAR_TYPE_FLOAT; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BOOL:	  n = VAR_TYPE_BOOL; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_SPECIAL: n = VAR_TYPE_NONE; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_JOB:     n = VAR_TYPE_JOB; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CHANNEL: n = VAR_TYPE_CHANNEL; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BLOB:    n = VAR_TYPE_BLOB; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_INSTR:   n = VAR_TYPE_INSTR; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		ಯದಿ (cl != ರಿಕ್ತ && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		ಅನ್ಯಥಾ
		    n = VAR_TYPE_CLASS;
		ಭಙ್ಗ;
	    }
	ಪಕ್ಷ VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		ಯದಿ (obj == ರಿಕ್ತ)
		    n = VAR_TYPE_OBJECT;
		ಅನ್ಯಥಾ
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    ಯದಿ (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    ಅನ್ಯಥಾ
			n = VAR_TYPE_OBJECT;
		}
		ಭಙ್ಗ;
	    }
	ಪಕ್ಷ VAR_VOID:
	    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	    internal_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    ಭಙ್ಗ;
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_start = 0;
    colnr_T	vcol_end = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    ಪೂರ್ಣಾಙ್ಕ		winchanged = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[2]), &tp);
	ಯದಿ (wp == ರಿಕ್ತ || tp == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;

	ಯದಿ (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    ಗಚ್ಛ theend;

	check_cursor();
	winchanged = TRUE;
    }

    ಪೂರ್ಣಾಙ್ಕ fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    ಯದಿ (fp != ರಿಕ್ತ && fp->lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	ಯದಿ (fp->col < 0)
	    fp->col = 0;
	ಅನ್ಯಥಾ
	{
	    len = (ಪೂರ್ಣಾಙ್ಕ)ml_get_len(fp->lnum);
	    ಯದಿ (fp->col > len)
		fp->col = len;
	}
	getvvcol(curwin, fp, &vcol_start, ರಿಕ್ತ, &vcol_end, 0);
	++vcol_start;
	++vcol_end;
    }

theend:
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	ಯದಿ (rettv_list_alloc(rettv) == OK)
	{
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, vcol_start);
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, vcol_end);
	}
	ಅನ್ಯಥಾ
	    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = vcol_end;

    ಯದಿ (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    ಯದಿ (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_mode_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    ಯದಿ (non_zero_arg(&argvars[0]))
	curbuf->b_visual_mode_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_wildmenumode(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ಯದಿ (wild_menu_showing || ((State & MODE_CMDLINE) && cmdline_pum_active()))
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
}

/*
 * "windowsversion()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_windowsversion(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windowsVersion);
}

/*
 * "wordcount()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_wordcount(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
f_xor(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ)
					^ tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ರಿಕ್ತ);
}

#endif // FEAT_EVAL
