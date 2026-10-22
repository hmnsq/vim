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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

ସ୍ଥିର ଶୂନ୍ୟ f_and(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_BEVAL
ସ୍ଥିର ଶୂନ୍ୟ f_balloon_gettext(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_balloon_show(typval_T *argvars, typval_T *rettv);
# ଯଦି defined(FEAT_BEVAL_TERM)
ସ୍ଥିର ଶୂନ୍ୟ f_balloon_split(typval_T *argvars, typval_T *rettv);
# endif
#endif
ସ୍ଥିର ଶୂନ୍ୟ f_base64_encode(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_base64_decode(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_bindtextdomain(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_byte2line(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_call(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_changenr(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_char2nr(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_charcol(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_col(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_confirm(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_copy(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_cursor(typval_T *argsvars, typval_T *rettv);
#ifdef MSWIN
ସ୍ଥିର ଶୂନ୍ୟ f_debugbreak(typval_T *argvars, typval_T *rettv);
#endif
ସ୍ଥିର ଶୂନ୍ୟ f_deepcopy(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_did_filetype(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_echoraw(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_empty(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_environ(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_err_teapot(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_escape(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_eval(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_eventhandler(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_execute(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_exists_compiled(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_expand(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_expandcmd(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_feedkeys(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_fnameescape(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_foreground(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_funcref(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_function(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_garbagecollect(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_get(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getbgcolor(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getcellpixels(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getchangelist(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getcharpos(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getcharsearch(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getcurpos(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getcursorcharpos(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getenv(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getfontname(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getinfo(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getjumplist(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getpid(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getpos(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getreg(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getreginfo(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getregion(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getregionpos(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_getregtype(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_gettagstack(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_gettext(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_haslocaldir(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_hlID(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_hlexists(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_hostname(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_id(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_index(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_indexof(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_ନିବେଶ(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_inputdialog(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_inputlist(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_inputrestore(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_inputsave(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_inputsecret(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_interrupt(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_invert(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_islocked(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_ବସ୍ତୁଗୁଡ଼ିକ(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_keytrans(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_last_buffer_nr(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_libcall(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_libcallnr(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_ଧାଡ଼ି(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_line2byte(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_LUA
ସ୍ଥିର ଶୂନ୍ୟ f_luaeval(typval_T *argvars, typval_T *rettv);
#endif
ସ୍ଥିର ଶୂନ୍ୟ f_match(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_matchbufline(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_matchend(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_matchlist(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_matchstr(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_matchstrlist(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_matchstrpos(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_max(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_min(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_MZSCHEME
ସ୍ଥିର ଶୂନ୍ୟ f_mzeval(typval_T *argvars, typval_T *rettv);
#endif
ସ୍ଥିର ଶୂନ୍ୟ f_nextnonblank(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_ngettext(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_nr2char(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_or(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PERL
ସ୍ଥିର ଶୂନ୍ୟ f_perleval(typval_T *argvars, typval_T *rettv);
#endif
ସ୍ଥିର ଶୂନ୍ୟ f_prevnonblank(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_printf(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_pum_getpos(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_pumvisible(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_PYTHON3
ସ୍ଥିର ଶୂନ୍ୟ f_py3eval(typval_T *argvars, typval_T *rettv);
#endif
#ifdef FEAT_PYTHON
ସ୍ଥିର ଶୂନ୍ୟ f_pyeval(typval_T *argvars, typval_T *rettv);
#endif
#ଯଦି defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
ସ୍ଥିର ଶୂନ୍ୟ f_pyxeval(typval_T *argvars, typval_T *rettv);
#endif
ସ୍ଥିର ଶୂନ୍ୟ f_test_srand_seed(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_rand(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_range(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_reg_executing(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_reg_recording(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_rename(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_repeat(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_RUBY
ସ୍ଥିର ଶୂନ୍ୟ f_rubyeval(typval_T *argvars, typval_T *rettv);
#endif
ସ୍ଥିର ଶୂନ୍ୟ f_screenattr(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_screenchar(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_screenchars(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_screencol(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_screenrow(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_screenstring(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_ସନ୍ଧାନ(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_searchdecl(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_searchpair(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_searchpairpos(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_searchpos(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_setcharpos(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_setcharsearch(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_setcursorcharpos(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_setenv(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_setfperm(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_setpos(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_setreg(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_settagstack(typval_T *argvars, typval_T *rettv);
#ifdef FEAT_CRYPT
ସ୍ଥିର ଶୂନ୍ୟ f_sha256(typval_T *argvars, typval_T *rettv);
#endif
ସ୍ଥିର ଶୂନ୍ୟ f_shellescape(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_shiftwidth(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_soundfold(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_spellbadword(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_spellsuggest(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_split(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_srand(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_submatch(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_substitute(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_swapfilelist(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_swapinfo(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_swapname(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_synID(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_synIDattr(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_synIDtrans(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_synstack(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_synconcealed(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_tabpagebuflist(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_taglist(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_tagfiles(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_type(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_virtcol(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_visualmode(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_wildmenumode(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_windowsversion(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_wordcount(typval_T *argvars, typval_T *rettv);
ସ୍ଥିର ଶୂନ୍ୟ f_xor(typval_T *argvars, typval_T *rettv);


/*
 * Functions that check the argument type of a builtin function.
 * Each function returns FAIL and gives an error message if the type is wrong.
 */

// Context passed to an arg_ function.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		arg_ଗଣନା;	// actual argument count
    type2_T	*arg_types;	// list of argument types
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		arg_idx;	// current argument index (first arg is zero)
    cctx_T	*arg_cctx;
} argcontext_T;

// A function to check one argument type.  The first argument is the type to
// check.  If needed, other argument types can be obtained with the context.
// E.g. if "arg_idx" is 1, then (type - 1) is the first argument type.
// NOTE:    Use "arg_any", not NULL, in funcentry_T.f_argcheck array
//	    to accept an argument of any type.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*argcheck_T)(type_T *, type_T *, argcontext_T *);

/*
 * Call need_type() to check an argument type.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_arg_type(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    ଫେରନ୍ତୁ need_type(actual, expected, 0,
	    context->arg_idx - context->arg_ଗଣନା, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE);
}

/*
 * Call need_type() to check an argument type and that it is modifiable
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_arg_type_mod(
	type_T		*expected,
	type_T		*actual,
	argcontext_T	*context)
{
    ଯଦି (need_type(actual, expected, 0,
	    context->arg_idx - context->arg_ଗଣନା, context->arg_idx + 1,
	    context->arg_cctx, FALSE, FALSE) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ arg_type_modifiable(actual, context->arg_idx + 1);
}

/*
 * Give an error if "type" is a constant.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_type_modifiable(type_T *type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_idx)
{
    ଅକ୍ଷର *tofree;

    ଯଦି ((type->tt_flags & TTFLAG_CONST) == 0)
	ଫେରନ୍ତୁ OK;
    semsg(_(e_argument_nr_trying_to_modify_const_str),
	    arg_idx, type_name(type, &tofree));
    vim_free(tofree);
    ଫେରନ୍ତୁ FAIL;
}

/*
 *  Return OK for any type unconditionally.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_any(type_T *type UNUSED,
	type_T *decl_type UNUSED,
	argcontext_T *context UNUSED)
{
    ଫେରନ୍ତୁ OK;
}

/*
 * Check "type" is a float or a number.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_float_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_FLOAT
	    || type->tt_type == VAR_ସଂଖ୍ୟା
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_ସଂଖ୍ୟା, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a number.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_ସଂଖ୍ୟା(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଫେରନ୍ତୁ check_arg_type(&t_ସଂଖ୍ୟା, type, context);
}

/*
 * Check "type" is an object.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_object(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_object_any, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a dict of 'any'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_dict_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଫେରନ୍ତୁ check_arg_type(&t_dict_any, type, context);
}

/*
 * Check "type" is a list of 'any'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଫେରନ୍ତୁ check_arg_type(&t_list_any, type, context);
}

/*
 * Check "type" is a list of 'any' and modifiable
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_any_mod(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    ଫେରନ୍ତୁ check_arg_type_mod(&t_list_any, type, context);
}

/*
 * Check "type" is a list of numbers.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_ସଂଖ୍ୟା(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଫେରନ୍ତୁ check_arg_type(&t_list_ସଂଖ୍ୟା, type, context);
}

/*
 * Check "type" is a list of strings.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଫେରନ୍ତୁ check_arg_type(&t_list_string, type, context);
}

/*
 * Check "type" is a tuple of 'any'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_tuple_any(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଫେରନ୍ତୁ check_arg_type(&t_tuple_any, type, context);
}

/*
 * Check "type" is a string.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_string(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଫେରନ୍ତୁ check_arg_type(&t_string, type, context);
}

/*
 * Check "type" is a blob
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଫେରନ୍ତୁ check_arg_type(&t_blob, type, context);
}

/*
 * Check "type" is a bool or number 0 or 1.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_bool(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଫେରନ୍ତୁ check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a bool or a number.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_bool_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_ସଂଖ୍ୟା
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_ସଂଖ୍ୟା, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a bool or a dict of 'any'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_bool_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_DICT || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ check_arg_type(&t_bool, type, context);
}

/*
 * Check "type" is a list of 'any' or a blob.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_or_blob(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a modifiable list of 'any' or a blob.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ଯଦି (arg_list_or_blob(type, decl_type, context) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a tuple.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_or_tuple(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}


/*
 * Check "type" is a list of 'any', a tuple or a blob.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_or_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a string or a number
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_string_or_nr(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ସଂଖ୍ୟା
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a buffer (string or a number)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_buffer(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ସଂଖ୍ୟା
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a buffer or a dict of any
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_buffer_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ସଂଖ୍ୟା
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a line (string or a number)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_lnum(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ସଂଖ୍ୟା
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a string or a list of strings.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_string_or_list_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    ଯଦି (type->tt_type != VAR_LIST)
    {
	arg_type_mismatch(&t_string, type, context->arg_idx + 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (type->tt_member->tt_type == VAR_ANY
		    || type->tt_member->tt_type == VAR_STRING)
	ଫେରନ୍ତୁ OK;

    arg_type_mismatch(&t_list_string, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a string or a list of 'any'
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_string_or_list_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a string or a dict of 'any'
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_string_or_dict_any(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a string or a blob
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_string_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a list of 'any', a tuple of 'any' or dict of 'any'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_or_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_or_dict_or_blob(
	type_T	     *type,
	type_T	     *decl_type UNUSED,
	argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a list of 'any', a dict of 'any' or a blob.
 * Also check if "type" is modifiable.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_or_dict_or_blob_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ଯଦି (arg_list_or_dict_or_blob(type, decl_type, context) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_or_dict_or_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a list of 'any' or a dict of 'any' or a blob or a string.
 * Also check the value is modifiable.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_or_dict_or_blob_or_string_mod(
	type_T	     *type,
	type_T	     *decl_type,
	argcontext_T *context)
{
    ଯଦି (arg_list_or_dict_or_blob_or_string(type, decl_type, context) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a list of 'any', a tuple of 'any', a dict of 'any', a blob
 * or a string.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_list_tuple_dict_blob_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;

    semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr),
						 context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check second argument of map(), filter(), foreach().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_map_filter_arg2(
    type_T		*type,
    argcontext_T	*context,
    filtermap_T		filtermap)
{
    type_T *expected_member = ଖାଲି;
    type_T *(args[2]);
    type_T t_func_exp = {VAR_FUNC, 2, 0, 0, ଖାଲି, ଖାଲି, args};

    ଯଦି (context->arg_types[0].type_curr->tt_type == VAR_LIST
	    || context->arg_types[0].type_curr->tt_type == VAR_DICT)
    {
	// Use the declared type if possible, so that an error is given if
	// a declared list changes type, but not if a constant list changes
	// type.
	ଯଦି (context->arg_types[0].type_decl->tt_type == VAR_LIST
		|| context->arg_types[0].type_decl->tt_type == VAR_DICT)
	    expected_member = context->arg_types[0].type_decl->tt_member;
	ଅନ୍ୟଥା
	    expected_member = context->arg_types[0].type_curr->tt_member;
    }
    ଅନ୍ୟଥା ଯଦି (context->arg_types[0].type_curr->tt_type == VAR_STRING)
	expected_member = &t_string;
    ଅନ୍ୟଥା ଯଦି (context->arg_types[0].type_curr->tt_type == VAR_BLOB)
	expected_member = &t_ସଂଖ୍ୟା;

    args[0] = ଖାଲି;
    args[1] = &t_unknown;
    ଯଦି (type->tt_argcount != -1)
    {
	ଯଦି (!(type->tt_argcount == 2 || (type->tt_argcount == 1
				    && (type->tt_flags & TTFLAG_VARARGS))))
	{
	    emsg(_(e_invalid_ସଂଖ୍ୟା_of_arguments));
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (type->tt_flags & TTFLAG_VARARGS)
	    // check the argument types at runtime
	    t_func_exp.tt_argcount = -1;
	ଅନ୍ୟଥା
	{
	    ଯଦି (context->arg_types[0].type_curr->tt_type == VAR_STRING
		    || context->arg_types[0].type_curr->tt_type == VAR_BLOB
		    || context->arg_types[0].type_curr->tt_type == VAR_LIST)
		args[0] = &t_ସଂଖ୍ୟା;
	    ଅନ୍ୟଥା ଯଦି (context->arg_types[0].type_decl->tt_type == VAR_DICT)
		args[0] = &t_string;
	    ଯଦି (args[0] != ଖାଲି)
		args[1] = expected_member;
	}
    }

    ଯଦି (!type_any_or_unknown(type->tt_member) || args[0] != ଖାଲି)
    {
	where_T where = WHERE_INIT;

	ଯଦି (filtermap == FILTERMAP_MAP)
	    t_func_exp.tt_member = expected_member == ଖାଲି
					|| type_any_or_unknown(type->tt_member)
				? &t_any : expected_member;
	ଅନ୍ୟଥା ଯଦି (filtermap == FILTERMAP_FILTER)
	    t_func_exp.tt_member = &t_bool;
	ଅନ୍ୟଥା // filtermap == FILTERMAP_FOREACH
	    t_func_exp.tt_member = &t_unknown;
	ଯଦି (args[0] == ଖାଲି)
	    args[0] = &t_unknown;
	ଯଦି (type->tt_argcount == -1)
	    t_func_exp.tt_argcount = -1;

	where.wt_index = 2;
	where.wt_kind = WT_ARGUMENT;
	ଫେରନ୍ତୁ check_type(&t_func_exp, type, TRUE, where);
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check second argument of filter(): func must return a bool.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_filter_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;

    ଯଦି (type->tt_type == VAR_FUNC)
	ଫେରନ୍ତୁ check_map_filter_arg2(type, context, FILTERMAP_FILTER);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check second argument of map(), the function.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_map_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;

    ଯଦି (type->tt_type == VAR_FUNC)
	ଫେରନ୍ତୁ check_map_filter_arg2(type, context, FILTERMAP_MAP);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check second argument of foreach(), the function.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_foreach_func(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;

    ଯଦି (type->tt_type == VAR_FUNC)
	ଫେରନ୍ତୁ check_map_filter_arg2(type, context, FILTERMAP_FOREACH);
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check second argument of sort() and uniq(), the "how" argument.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_sort_how(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;

    ଯଦି (type->tt_type == VAR_FUNC)
    {
	type_T *(args[2]);
	type_T t_func_exp = {VAR_FUNC, 2, 0, 0, &t_ସଂଖ୍ୟା, ଖାଲି, args};

	ଯଦି (context->arg_types[0].type_curr->tt_type == VAR_LIST)
	    args[0] = context->arg_types[0].type_curr->tt_member;
	ଅନ୍ୟଥା
	    args[0] = &t_unknown;
	ଯଦି (!type_any_or_unknown(type->tt_member) || args[0] != &t_unknown)
	{
	    where_T where = WHERE_INIT;

	    args[1] = args[0];
	    ଯଦି (type->tt_argcount == -1)
		t_func_exp.tt_argcount = -1;
	    where.wt_index = 2;
	    where.wt_kind = WT_ARGUMENT;
	    ଫେରନ୍ତୁ check_type(&t_func_exp, type, TRUE, where);
	}

	ଫେରନ୍ତୁ OK;
    }
    semsg(_(e_string_or_function_required_for_argument_nr), 2);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check an expression argument, can be a string, funcref or partial.
 * Also accept a bool, a constant resulting from compiling a string argument.
 * Also accept a number, one and zero are accepted.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_string_or_func(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_PARTIAL
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_BOOL
	    || type->tt_type == VAR_ସଂଖ୍ୟା
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_func_any, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check varargs' "type" are class.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
varargs_class(type_T *type UNUSED,
	      type_T *decl_type UNUSED,
	      argcontext_T *context)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = context->arg_idx; i < context->arg_ଗଣନା; ++i)
    {
	type2_T *types = &context->arg_types[i];
	ଯଦି (types->type_curr->tt_type != VAR_CLASS)
	{
	    semsg(_(e_class_or_typealias_required_for_argument_nr), i + 1);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check "type" is a list of 'any', a tuple, a blob or a string.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_string_list_tuple_or_blob(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a tuple or a modifiable list of 'any' or a blob or a string.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_reverse(type_T *type, type_T *decl_type, argcontext_T *context)
{
    ଯଦି (arg_string_list_tuple_or_blob(type, decl_type, context) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ arg_type_modifiable(type, context->arg_idx + 1);
}

/*
 * Check "type" is a job.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଫେରନ୍ତୁ check_arg_type(&t_job, type, context);
}

/*
 * Check "type" is a channel or a job.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_chan_or_job(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_CHANNEL
	    || type->tt_type == VAR_JOB
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_channel, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" can be used as the type_decl of the previous argument.
 * Must not be used for the first argcheck_T entry.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_same_as_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_decl;

    ଫେରନ୍ତୁ check_arg_type(prev_type, type, context);
}

/*
 * Check "type" is the same basic type as the previous argument, checks list or
 * dict vs other type, but not member type.
 * Must not be used for the first argcheck_T entry.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_same_struct_as_prev(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;

    ଯଦି (prev_type->tt_type != context->arg_types[context->arg_idx].type_curr->tt_type)
	ଫେରନ୍ତୁ check_arg_type(prev_type, type, context);
    ଫେରନ୍ତୁ OK;
}

/*
 * Check "type" is an item of the list or blob of the previous arg.
 * Must not be used for the first argcheck_T entry.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_item_of_prev(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    type_T *prev_type = context->arg_types[context->arg_idx - 1].type_curr;
    type_T *expected;

    ଯଦି (prev_type->tt_type == VAR_LIST)
	expected = prev_type->tt_member;
    ଅନ୍ୟଥା ଯଦି (prev_type->tt_type == VAR_BLOB)
	expected = &t_ସଂଖ୍ୟା;
    ଅନ୍ୟଥା
	// probably VAR_ANY, can't check
	ଫେରନ୍ତୁ OK;

    ଫେରନ୍ତୁ check_arg_type(expected, type, context);
}

/*
 * Check "type" is a string or a number or a list
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_str_or_nr_or_list(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ସଂଖ୍ୟା
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" is a dict of 'any' or a string
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_dict_any_or_string(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_DICT
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;
    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" which is the third argument of extend() (number or string or
 * any)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_extend3(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 2].type_curr;

    ଯଦି (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	ଫେରନ୍ତୁ arg_ସଂଖ୍ୟା(type, decl_type, context);
    ଯଦି (first_type->tt_type == VAR_DICT)
	ଫେରନ୍ତୁ arg_string(type, decl_type, context);
    ଫେରନ୍ତୁ OK;
}

/*
 * Check "type" which is the first argument of get() (a blob, a list, a tuple,
 * a dict or a funcref)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_get1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_FUNC
	    || type->tt_type == VAR_PARTIAL
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" which is the first argument of len() (a string, a number, a
 * blob, a list, a tuple, a dict or an object)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_len1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ସଂଖ୍ୟା
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type->tt_type == VAR_OBJECT
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" which is the second argument of remove() (number or string or
 * any)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_remove2(type_T *type, type_T *decl_type, argcontext_T *context)
{
    type_T *first_type = context->arg_types[context->arg_idx - 1].type_curr;

    ଯଦି (first_type->tt_type == VAR_LIST || first_type->tt_type == VAR_BLOB)
	ଫେରନ୍ତୁ arg_ସଂଖ୍ୟା(type, decl_type, context);
    ଯଦି (first_type->tt_type == VAR_DICT)
	ଫେରନ୍ତୁ arg_string_or_nr(type, decl_type, context);
    ଫେରନ୍ତୁ OK;
}

/*
 * Check "type" which is the first argument of repeat() (a string, a number, a
 * blob, a list, a tuple or any)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_repeat1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_ସଂଖ୍ୟା
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;

    arg_type_mismatch(&t_string, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" which is the first argument of slice() (a list, a tuple, a
 * blob, a string or any)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_slice1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_BLOB
	    || type->tt_type == VAR_STRING
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;

    arg_type_mismatch(&t_list_any, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" which is the first argument of count() (a string, a list, a
 * tuple, a dict or any)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_string_list_tuple_or_dict(
    type_T		*type,
    type_T		*decl_type UNUSED,
    argcontext_T	*context)
{
    ଯଦି (type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type->tt_type == VAR_TUPLE
	    || type->tt_type == VAR_DICT
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;

    semsg(_(e_string_list_tuple_or_dict_required_for_argument_nr),
							 context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check "type" which is the first argument of cursor() (number or string or
 * list or any)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_cursor1(type_T *type, type_T *decl_type UNUSED, argcontext_T *context)
{
    ଯଦି (type->tt_type == VAR_ସଂଖ୍ୟା
	    || type->tt_type == VAR_STRING
	    || type->tt_type == VAR_LIST
	    || type_any_or_unknown(type))
	ଫେରନ୍ତୁ OK;

    arg_type_mismatch(&t_ସଂଖ୍ୟା, type, context->arg_idx + 1);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Lists of functions that check the argument types of a builtin function.
 */
ସ୍ଥିର argcheck_T arg1_blob[] = {arg_blob};
ସ୍ଥିର argcheck_T arg1_bool[] = {arg_bool};
ସ୍ଥିର argcheck_T arg1_buffer[] = {arg_buffer};
ସ୍ଥିର argcheck_T arg1_buffer_or_dict_any[] = {arg_buffer_or_dict_any};
ସ୍ଥିର argcheck_T arg1_chan_or_job[] = {arg_chan_or_job};
ସ୍ଥିର argcheck_T arg1_dict_any[] = {arg_dict_any};
ସ୍ଥିର argcheck_T arg1_dict_or_string[] = {arg_dict_any_or_string};
ସ୍ଥିର argcheck_T arg1_float_or_nr[] = {arg_float_or_nr};
ସ୍ଥିର argcheck_T arg1_job[] = {arg_job};
ସ୍ଥିର argcheck_T arg1_list_any[] = {arg_list_any};
ସ୍ଥିର argcheck_T arg1_tuple_any[] = {arg_tuple_any};
ସ୍ଥିର argcheck_T arg1_list_ସଂଖ୍ୟା[] = {arg_list_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg1_reverse[] = {arg_reverse};
ସ୍ଥିର argcheck_T arg1_list_or_tuple_or_dict[] = {arg_list_or_tuple_or_dict};
ସ୍ଥିର argcheck_T arg1_list_string[] = {arg_list_string};
ସ୍ଥିର argcheck_T arg1_list_tuple_dict_blob_or_string[] = {arg_list_tuple_dict_blob_or_string};
ସ୍ଥିର argcheck_T arg1_lnum[] = {arg_lnum};
ସ୍ଥିର argcheck_T arg1_ସଂଖ୍ୟା[] = {arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg1_string[] = {arg_string};
ସ୍ଥିର argcheck_T arg1_string_or_list_any[] = {arg_string_or_list_any};
ସ୍ଥିର argcheck_T arg1_string_or_list_string[] = {arg_string_or_list_string};
ସ୍ଥିର argcheck_T arg1_string_or_nr[] = {arg_string_or_nr};
ସ୍ଥିର argcheck_T arg1_string_or_blob[] = {arg_string_or_blob};
ସ୍ଥିର argcheck_T arg2_buffer_any[] = {arg_buffer, arg_any};
ସ୍ଥିର argcheck_T arg2_buffer_bool[] = {arg_buffer, arg_bool};
ସ୍ଥିର argcheck_T arg2_buffer_list_any[] = {arg_buffer, arg_list_any};
ସ୍ଥିର argcheck_T arg2_buffer_lnum[] = {arg_buffer, arg_lnum};
ସ୍ଥିର argcheck_T arg2_buffer_ସଂଖ୍ୟା[] = {arg_buffer, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg2_buffer_string[] = {arg_buffer, arg_string};
ସ୍ଥିର argcheck_T arg2_chan_or_job_dict[] = {arg_chan_or_job, arg_dict_any};
ସ୍ଥିର argcheck_T arg2_chan_or_job_string[] = {arg_chan_or_job, arg_string};
ସ୍ଥିର argcheck_T arg2_dict_any_list_any[] = {arg_dict_any, arg_list_any};
ସ୍ଥିର argcheck_T arg2_dict_any_string_or_nr[] = {arg_dict_any, arg_string_or_nr};
ସ୍ଥିର argcheck_T arg2_dict_string[] = {arg_dict_any, arg_string};
ସ୍ଥିର argcheck_T arg2_float_or_nr[] = {arg_float_or_nr, arg_float_or_nr};
ସ୍ଥିର argcheck_T arg2_job_dict[] = {arg_job, arg_dict_any};
ସ୍ଥିର argcheck_T arg2_job_string_or_ସଂଖ୍ୟା[] = {arg_job, arg_string_or_nr};
ସ୍ଥିର argcheck_T arg2_list_any_ସଂଖ୍ୟା[] = {arg_list_any, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg2_list_ସଂଖ୍ୟା[] = {arg_list_ସଂଖ୍ୟା, arg_list_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg2_list_ସଂଖ୍ୟା_bool[] = {arg_list_ସଂଖ୍ୟା, arg_bool};
ସ୍ଥିର argcheck_T arg2_list_string_dict[] = {arg_list_string, arg_dict_any};
ସ୍ଥିର argcheck_T arg2_listblobmod_item[] = {arg_list_or_blob_mod, arg_item_of_prev};
ସ୍ଥିର argcheck_T arg2_lnum[] = {arg_lnum, arg_lnum};
ସ୍ଥିର argcheck_T arg2_lnum_ସଂଖ୍ୟା[] = {arg_lnum, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg2_ସଂଖ୍ୟା[] = {arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg2_ସଂଖ୍ୟା_any[] = {arg_ସଂଖ୍ୟା, arg_any};
ସ୍ଥିର argcheck_T arg2_ସଂଖ୍ୟା_bool[] = {arg_ସଂଖ୍ୟା, arg_bool};
ସ୍ଥିର argcheck_T arg2_ସଂଖ୍ୟା_dict_any[] = {arg_ସଂଖ୍ୟା, arg_dict_any};
ସ୍ଥିର argcheck_T arg2_ସଂଖ୍ୟା_list[] = {arg_ସଂଖ୍ୟା, arg_list_any};
ସ୍ଥିର argcheck_T arg2_ସଂଖ୍ୟା_string[] = {arg_ସଂଖ୍ୟା, arg_string};
ସ୍ଥିର argcheck_T arg2_ସଂଖ୍ୟା_buffer[] = {arg_ସଂଖ୍ୟା, arg_buffer};
ସ୍ଥିର argcheck_T arg2_ସଂଖ୍ୟା_string_or_list[] = {arg_ସଂଖ୍ୟା, arg_string_or_list_any};
ସ୍ଥିର argcheck_T arg2_str_or_nr_or_list_dict[] = {arg_str_or_nr_or_list, arg_dict_any};
ସ୍ଥିର argcheck_T arg2_string[] = {arg_string, arg_string};
ସ୍ଥିର argcheck_T arg2_string_any[] = {arg_string, arg_any};
ସ୍ଥିର argcheck_T arg2_string_bool[] = {arg_string, arg_bool};
ସ୍ଥିର argcheck_T arg2_string_chan_or_job[] = {arg_string, arg_chan_or_job};
ସ୍ଥିର argcheck_T arg2_string_dict[] = {arg_string, arg_dict_any};
ସ୍ଥିର argcheck_T arg2_string_list_ସଂଖ୍ୟା[] = {arg_string, arg_list_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg2_string_ସଂଖ୍ୟା[] = {arg_string, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg2_string_or_list_dict[] = {arg_string_or_list_any, arg_dict_any};
ସ୍ଥିର argcheck_T arg2_string_or_list_ସଂଖ୍ୟା[] = {arg_string_or_list_any, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg2_string_string_or_ସଂଖ୍ୟା[] = {arg_string, arg_string_or_nr};
ସ୍ଥିର argcheck_T arg2_blob_dict[] = {arg_blob, arg_dict_any};
ସ୍ଥିର argcheck_T arg2_list_or_tuple_string[] = {arg_list_or_tuple, arg_string};
ସ୍ଥିର argcheck_T arg3_any_buffer_bool_or_dict[] = {
			      arg_any, arg_buffer, arg_bool_or_dict_any};
ସ୍ଥିର argcheck_T arg3_any_list_dict[] = {arg_any, arg_list_any, arg_dict_any};
ସ୍ଥିର argcheck_T arg3_buffer_lnum_lnum[] = {arg_buffer, arg_lnum, arg_lnum};
ସ୍ଥିର argcheck_T arg3_buffer_ସଂଖ୍ୟା_ସଂଖ୍ୟା[] = {arg_buffer, arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg3_buffer_string_any[] = {arg_buffer, arg_string, arg_any};
ସ୍ଥିର argcheck_T arg3_buffer_string_dict[] = {arg_buffer, arg_string, arg_dict_any};
ସ୍ଥିର argcheck_T arg3_dict_ସଂଖ୍ୟା_ସଂଖ୍ୟା[] = {arg_dict_any, arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg3_diff[] = {arg_list_string, arg_list_string, arg_dict_any};
ସ୍ଥିର argcheck_T arg3_list_string_dict[] = {arg_list_any, arg_string, arg_dict_any};
ସ୍ଥିର argcheck_T arg3_list_list_dict[] = {arg_list_any, arg_list_any, arg_dict_any};
ସ୍ଥିର argcheck_T arg3_lnum_ସଂଖ୍ୟା_bool[] = {arg_lnum, arg_ସଂଖ୍ୟା, arg_bool};
ସ୍ଥିର argcheck_T arg3_ସଂଖ୍ୟା[] = {arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg3_ସଂଖ୍ୟା_any_dict[] = {arg_ସଂଖ୍ୟା, arg_any, arg_dict_any};
ସ୍ଥିର argcheck_T arg3_ସଂଖ୍ୟା_ସଂଖ୍ୟା_dict[] = {arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା, arg_dict_any};
ସ୍ଥିର argcheck_T arg3_ସଂଖ୍ୟା_string_any[] = {arg_ସଂଖ୍ୟା, arg_string, arg_any};
ସ୍ଥିର argcheck_T arg3_ସଂଖ୍ୟା_string_buffer[] = {arg_ସଂଖ୍ୟା, arg_string, arg_buffer};
ସ୍ଥିର argcheck_T arg3_ସଂଖ୍ୟା_string_string[] = {arg_ସଂଖ୍ୟା, arg_string, arg_string};
ସ୍ଥିର argcheck_T arg3_string[] = {arg_string, arg_string, arg_string};
ସ୍ଥିର argcheck_T arg3_string_any_dict[] = {arg_string, arg_any, arg_dict_any};
ସ୍ଥିର argcheck_T arg3_string_any_string[] = {arg_string, arg_any, arg_string};
ସ୍ଥିର argcheck_T arg3_string_bool_bool[] = {arg_string, arg_bool, arg_bool};
ସ୍ଥିର argcheck_T arg3_string_ସଂଖ୍ୟା_bool[] = {arg_string, arg_ସଂଖ୍ୟା, arg_bool};
ସ୍ଥିର argcheck_T arg3_string_ସଂଖ୍ୟା_ସଂଖ୍ୟା[] = {arg_string, arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg3_string_or_dict_bool_dict[] = {arg_string_or_dict_any, arg_bool, arg_dict_any};
ସ୍ଥିର argcheck_T arg3_string_or_list_bool_ସଂଖ୍ୟା[] = {arg_string_or_list_any, arg_bool, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg3_string_string_bool[] = {arg_string, arg_string, arg_bool};
ସ୍ଥିର argcheck_T arg3_string_string_dict[] = {arg_string, arg_string, arg_dict_any};
ସ୍ଥିର argcheck_T arg3_string_string_ସଂଖ୍ୟା[] = {arg_string, arg_string, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg4_ସଂଖ୍ୟା_ସଂଖ୍ୟା_string_any[] = {arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା, arg_string, arg_any};
ସ୍ଥିର argcheck_T arg4_string_string_any_string[] = {arg_string, arg_string, arg_any, arg_string};
ସ୍ଥିର argcheck_T arg4_string_string_ସଂଖ୍ୟା_string[] = {arg_string, arg_string, arg_ସଂଖ୍ୟା, arg_string};
ସ୍ଥିର argcheck_T arg4_string_ସଂଖ୍ୟା_bool_bool[] = {arg_string, arg_ସଂଖ୍ୟା, arg_bool, arg_bool};
/* Function specific argument types (not covered by the above) */
ସ୍ଥିର argcheck_T arg15_assert_fails[] = {arg_string_or_nr, arg_string_or_list_any, arg_any, arg_ସଂଖ୍ୟା, arg_string};
ସ୍ଥିର argcheck_T arg34_assert_inrange[] = {arg_float_or_nr, arg_float_or_nr, arg_float_or_nr, arg_string};
ସ୍ଥିର argcheck_T arg4_browse[] = {arg_bool, arg_string, arg_string, arg_string};
ସ୍ଥିର argcheck_T arg23_chanexpr[] = {arg_chan_or_job, arg_any, arg_dict_any};
ସ୍ଥିର argcheck_T arg23_chanraw[] = {arg_chan_or_job, arg_string_or_blob, arg_dict_any};
ସ୍ଥିର argcheck_T arg24_ଗଣନା[] = {arg_string_list_tuple_or_dict, arg_any, arg_bool, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg13_cursor[] = {arg_cursor1, arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg12_deepcopy[] = {arg_any, arg_bool};
ସ୍ଥିର argcheck_T arg12_execute[] = {arg_string_or_list_string, arg_string};
ସ୍ଥିର argcheck_T arg12_getchar[] = {arg_bool_or_nr, arg_dict_any};
ସ୍ଥିର argcheck_T arg23_extend[] = {arg_list_or_dict_or_blob_mod, arg_same_as_prev, arg_extend3};
ସ୍ଥିର argcheck_T arg23_extendnew[] = {arg_list_or_dict_or_blob, arg_same_struct_as_prev, arg_extend3};
ସ୍ଥିର argcheck_T arg23_get[] = {arg_get1, arg_string_or_nr, arg_any};
ସ୍ଥିର argcheck_T arg14_glob[] = {arg_string, arg_bool, arg_bool, arg_bool};
ସ୍ଥିର argcheck_T arg25_globpath[] = {arg_string, arg_string, arg_bool, arg_bool, arg_bool};
ସ୍ଥିର argcheck_T arg24_index[] = {arg_list_or_tuple_or_blob, arg_item_of_prev, arg_ସଂଖ୍ୟା, arg_bool};
ସ୍ଥିର argcheck_T arg23_index[] = {arg_list_or_tuple_or_blob, arg_filter_func, arg_dict_any};
ସ୍ଥିର argcheck_T arg23_ଭର୍ତ୍ତି[] = {arg_list_or_blob, arg_item_of_prev, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg1_len[] = {arg_len1};
ସ୍ଥିର argcheck_T arg3_libcall[] = {arg_string, arg_string, arg_string_or_nr};
ସ୍ଥିର argcheck_T arg14_maparg[] = {arg_string, arg_string, arg_bool, arg_bool};
ସ୍ଥିର argcheck_T arg2_filter[] = {arg_list_or_dict_or_blob_or_string_mod, arg_filter_func};
ସ୍ଥିର argcheck_T arg2_foreach[] = {arg_list_tuple_dict_blob_or_string, arg_foreach_func};
ସ୍ଥିର argcheck_T arg2_instanceof[] = {arg_object, varargs_class, ଖାଲି };
ସ୍ଥିର argcheck_T arg2_map[] = {arg_list_or_dict_or_blob_or_string_mod, arg_map_func};
ସ୍ଥିର argcheck_T arg2_mapnew[] = {arg_list_or_dict_or_blob_or_string, arg_any};
ସ୍ଥିର argcheck_T arg25_matchadd[] = {arg_string, arg_string, arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା, arg_dict_any};
ସ୍ଥିର argcheck_T arg25_matchaddpos[] = {arg_string, arg_list_any, arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା, arg_dict_any};
ସ୍ଥିର argcheck_T arg23_matchstrlist[] = {arg_list_string, arg_string, arg_dict_any};
ସ୍ଥିର argcheck_T arg45_matchbufline[] = {arg_buffer, arg_string, arg_lnum, arg_lnum, arg_dict_any};
ସ୍ଥିର argcheck_T arg119_printf[] = {arg_string_or_nr, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any, arg_any};
ସ୍ଥିର argcheck_T arg23_reduce[] = {arg_string_list_tuple_or_blob, arg_any, arg_any};
ସ୍ଥିର argcheck_T arg24_remote_ପଦସମଷ୍ଟି[] = {arg_string, arg_string, arg_string, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg23_remove[] = {arg_list_or_dict_or_blob_mod, arg_remove2, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg2_repeat[] = {arg_repeat1, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg15_ସନ୍ଧାନ[] = {arg_string, arg_string, arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା, arg_string_or_func};
ସ୍ଥିର argcheck_T arg37_searchpair[] = {arg_string, arg_string, arg_string, arg_string, arg_string_or_func, arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg3_setbufline[] = {arg_buffer, arg_lnum, arg_str_or_nr_or_list};
ସ୍ଥିର argcheck_T arg2_setline[] = {arg_lnum, arg_any};
ସ୍ଥିର argcheck_T arg24_setloclist[] = {arg_ସଂଖ୍ୟା, arg_list_any, arg_string, arg_dict_any};
ସ୍ଥିର argcheck_T arg13_setqflist[] = {arg_list_any, arg_string, arg_dict_any};
ସ୍ଥିର argcheck_T arg23_settagstack[] = {arg_ସଂଖ୍ୟା, arg_dict_any, arg_string};
ସ୍ଥିର argcheck_T arg02_sign_getplaced[] = {arg_buffer, arg_dict_any};
ସ୍ଥିର argcheck_T arg45_sign_place[] = {arg_ସଂଖ୍ୟା, arg_string, arg_string, arg_buffer, arg_dict_any};
ସ୍ଥିର argcheck_T arg23_slice[] = {arg_slice1, arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା};
ସ୍ଥିର argcheck_T arg13_sortuniq[] = {arg_list_any_mod, arg_sort_how, arg_dict_any};
ସ୍ଥିର argcheck_T arg24_strpart[] = {arg_string, arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା, arg_bool};
ସ୍ଥିର argcheck_T arg12_system[] = {arg_string_or_list_any, arg_str_or_nr_or_list};
ସ୍ଥିର argcheck_T arg23_win_execute[] = {arg_ସଂଖ୍ୟା, arg_string_or_list_string, arg_string};
ସ୍ଥିର argcheck_T arg23_writefile[] = {arg_list_or_blob, arg_string, arg_string};
ସ୍ଥିର argcheck_T arg24_match_func[] = {arg_string_or_list_any, arg_string, arg_ସଂଖ୍ୟା, arg_ସଂଖ୍ୟା};

// Can be used by functions called through "f_retfunc" to create new types.
ସ୍ଥିର garray_T *current_type_gap = ଖାଲି;

/*
 * Functions that return the return type of a builtin function.
 * Note that "argtypes" is NULL if "argcount" is zero.
 */
    ସ୍ଥିର type_T *
ret_void(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_void;
}
    ସ୍ଥିର type_T *
ret_any(ପୂର୍ଣ୍ଣସଂଖ୍ୟା	argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_any;
}
    ସ୍ଥିର type_T *
ret_bool(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_bool;
}
    ସ୍ଥିର type_T *
ret_ସଂଖ୍ୟା_bool(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_ସଂଖ୍ୟା_bool;
}
    ସ୍ଥିର type_T *
ret_ସଂଖ୍ୟା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_ସଂଖ୍ୟା;
}
    ସ୍ଥିର type_T *
ret_float(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_float;
}
    ସ୍ଥିର type_T *
ret_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_string;
}
    ସ୍ଥିର type_T *
ret_list_any(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_list_any;
}
    ସ୍ଥିର type_T *
ret_list_ସଂଖ୍ୟା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ଫେରନ୍ତୁ &t_list_ସଂଖ୍ୟା;
}
    ସ୍ଥିର type_T *
ret_list_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ଫେରନ୍ତୁ &t_list_string;
}
    ସ୍ଥିର type_T *
ret_list_dict_any(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ଫେରନ୍ତୁ &t_list_dict_any;
}
    ସ୍ଥିର type_T *
ret_list_ବସ୍ତୁଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ଫେରନ୍ତୁ &t_list_list_any;
}
    ସ୍ଥିର type_T *
ret_list_list_any(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ଫେରନ୍ତୁ &t_list_list_any;
}
    ସ୍ଥିର type_T *
ret_list_string_ବସ୍ତୁଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ଫେରନ୍ତୁ &t_list_list_string;
}
    ସ୍ଥିର type_T *
ret_list_regionpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    *decl_type = &t_list_any;
    ଫେରନ୍ତୁ &t_list_list_list_ସଂଖ୍ୟା;
}
    ସ୍ଥିର type_T *
ret_tuple_any(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_tuple_any;
}
    ସ୍ଥିର type_T *
ret_dict_any(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_dict_any;
}
    ସ୍ଥିର type_T *
ret_job_info(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    ଯଦି (argcount == 0)
    {
	*decl_type = &t_list_any;
	ଫେରନ୍ତୁ &t_list_job;
    }
    ଫେରନ୍ତୁ &t_dict_any;
}
    ସ୍ଥିର type_T *
ret_dict_ସଂଖ୍ୟା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_dict_ସଂଖ୍ୟା;
}
    ସ୍ଥିର type_T *
ret_dict_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_dict_string;
}
    ସ୍ଥିର type_T *
ret_blob(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_blob;
}
    ସ୍ଥିର type_T *
ret_func_any(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_func_any;
}
    ସ୍ଥିର type_T *
ret_func_unknown(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_func_unknown;
}
    ସ୍ଥିର type_T *
ret_channel(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_channel;
}
    ସ୍ଥିର type_T *
ret_job(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଫେରନ୍ତୁ &t_job;
}
    ସ୍ଥିର type_T *
ret_first_arg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ଯଦି (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	ଫେରନ୍ତୁ argtypes[0].type_curr;
    }
    ଫେରନ୍ତୁ &t_void;
}
    ସ୍ଥିର type_T *
ret_slice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ଯଦି (argcount > 0)
    {
	ଯଦି (argtypes[0].type_decl != ଖାଲି)
	{
	    ବାଛ (argtypes[0].type_decl->tt_type)
	    {
		ସ୍ଥିତି VAR_STRING: *decl_type = &t_string; ଭାଙ୍ଗ;
		ସ୍ଥିତି VAR_BLOB: *decl_type = &t_blob; ଭାଙ୍ଗ;
		ସ୍ଥିତି VAR_LIST: *decl_type = &t_list_any; ଭାଙ୍ଗ;
		ସ୍ଥିତି VAR_TUPLE: *decl_type = &t_tuple_any; ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଭାଙ୍ଗ;
	    }
	}
	ଫେରନ୍ତୁ argtypes[0].type_curr;
    }
    ଫେରନ୍ତୁ &t_void;
}
    ସ୍ଥିର type_T *
ret_copy(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ଯଦି (argcount > 0)
    {
	ଯଦି (argtypes[0].type_decl != ଖାଲି)
	{
	    ଯଦି (argtypes[0].type_decl->tt_type == VAR_LIST)
		*decl_type = &t_list_any;
	    ଅନ୍ୟଥା ଯଦି (argtypes[0].type_decl->tt_type == VAR_DICT)
		*decl_type = &t_dict_any;
	    ଅନ୍ୟଥା
		*decl_type = argtypes[0].type_decl;
	}
	ଯଦି (argtypes[0].type_curr != ଖାଲି)
	{
	    ଯଦି (argtypes[0].type_curr->tt_type == VAR_LIST)
		ଫେରନ୍ତୁ &t_list_any;
	    ଅନ୍ୟଥା ଯଦି (argtypes[0].type_curr->tt_type == VAR_DICT)
		ଫେରନ୍ତୁ &t_dict_any;
	}
	ଫେରନ୍ତୁ argtypes[0].type_curr;
    }
    ଫେରନ୍ତୁ &t_void;
}
    ସ୍ଥିର type_T *
ret_extend(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ଯଦି (argcount > 0)
    {
	*decl_type = argtypes[0].type_decl;
	// if the second argument has a different current type then the current
	// type is "any"
	ଯଦି (argcount > 1 && !equal_type(argtypes[0].type_curr,
						     argtypes[1].type_curr, 0))
	{
	    ଯଦି (argtypes[0].type_curr->tt_type == VAR_LIST)
		ଫେରନ୍ତୁ &t_list_any;
	    ଯଦି (argtypes[0].type_curr->tt_type == VAR_DICT)
		ଫେରନ୍ତୁ &t_dict_any;
	    ଯଦି (argtypes[0].type_curr->tt_type == VAR_BLOB)
		ଫେରନ୍ତୁ &t_blob;
	}
	ଫେରନ୍ତୁ argtypes[0].type_curr;
    }
    ଫେରନ୍ତୁ &t_void;
}
    ସ୍ଥିର type_T *
ret_repeat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    ଯଦି (argcount == 0)
	ଫେରନ୍ତୁ &t_any;
    ଯଦି (argtypes[0].type_curr == &t_ସଂଖ୍ୟା)
	ଫେରନ୍ତୁ &t_string;
    ଫେରନ୍ତୁ argtypes[0].type_curr;
}
// for map(): returns first argument but item type may differ
    ସ୍ଥିର type_T *
ret_first_cont(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    ଯଦି (argcount > 0)
    {
	ଯଦି (argtypes[0].type_curr->tt_type == VAR_LIST)
	    ଫେରନ୍ତୁ &t_list_any;
	ଯଦି (argtypes[0].type_curr->tt_type == VAR_DICT)
	    ଫେରନ୍ତୁ &t_dict_any;
	ଯଦି (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    ଫେରନ୍ତୁ argtypes[0].type_curr;
    }
    ଫେରନ୍ତୁ &t_any;
}
// for get(): the type of the item when a missing item has the same type
    ସ୍ଥିର type_T *
ret_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ଯଦି (argcount > 0)
    {
	type_T	*t = argtypes[0].type_curr;
	type_T	*item = ଖାଲି;

	ଯଦି (t->tt_type == VAR_BLOB)
	    item = &t_ସଂଖ୍ୟା;
	ଅନ୍ୟଥା ଯଦି (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    item = t->tt_member;
	// A missing item gives {default} when present, otherwise zero.
	ଯଦି (item != ଖାଲି && (argcount == 3
			? equal_type(item, argtypes[2].type_curr, 0)
			: item->tt_type == VAR_ସଂଖ୍ୟା))
	{
	    ଯଦି (t->tt_type != VAR_BLOB
			    && argtypes[0].type_decl->tt_type == t->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    ଫେରନ୍ତୁ item;
	}
    }
    ଫେରନ୍ତୁ &t_any;
}
// for getline()
    ସ୍ଥିର type_T *
ret_getline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଯଦି (argcount == 1)
	ଫେରନ୍ତୁ &t_string;
    *decl_type = &t_list_any;
    ଫେରନ୍ତୁ &t_list_string;
}
// A string with fewer than N arguments, otherwise it depends on the value of
// the last one: a string or a list of strings.
    ସ୍ଥିର type_T *
ret_string_or_any_1(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଯଦି (argcount < 1)
	ଫେରନ୍ତୁ &t_string;
    ଫେରନ୍ତୁ &t_any;
}
    ସ୍ଥିର type_T *
ret_string_or_any_2(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଯଦି (argcount < 2)
	ଫେରନ୍ତୁ &t_string;
    ଫେରନ୍ତୁ &t_any;
}
    ସ୍ଥିର type_T *
ret_string_or_any_3(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଯଦି (argcount < 3)
	ଫେରନ୍ତୁ &t_string;
    ଫେରନ୍ତୁ &t_any;
}
    ସ୍ଥିର type_T *
ret_string_or_any_4(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    ଯଦି (argcount < 4)
	ଫେରନ୍ତୁ &t_string;
    ଫେରନ୍ତୁ &t_any;
}
// for abs()
    ସ୍ଥିର type_T *
ret_abs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    ଯଦି (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_FLOAT)
	ଫେରନ୍ତୁ &t_float;
    ଫେରନ୍ତୁ &t_ସଂଖ୍ୟା;
}
// for sign_define()
    ସ୍ଥିର type_T *
ret_sign_define(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes,
	type_T	**decl_type UNUSED)
{
    ଯଦି (argcount > 0 && argtypes[0].type_curr->tt_type == VAR_LIST)
	ଫେରନ୍ତୁ &t_list_ସଂଖ୍ୟା;
    ଫେରନ୍ତୁ &t_ସଂଖ୍ୟା;
}
// for values(): list of member of first argument
    ସ୍ଥିର type_T *
ret_list_member(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ଯଦି (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	ଯଦି (current_type_gap != ଖାଲି
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    t = get_list_type(t->tt_member, current_type_gap);
	ଅନ୍ୟଥା
	    t = &t_list_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	ଯଦି (current_type_gap != ଖାଲି
		&& (t->tt_type == VAR_DICT || t->tt_type == VAR_LIST))
	    ଫେରନ୍ତୁ get_list_type(t->tt_member, current_type_gap);
    }
    ଫେରନ୍ତୁ &t_list_any;
}

/*
 * Used for getqflist(): returns list if there is no argument, dict if there is
 * one.
 */
    ସ୍ଥିର type_T *
ret_list_or_dict_0(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    ଯଦି (argcount > 0)
	ଫେରନ୍ତୁ &t_dict_any;
    *decl_type = &t_list_any;
    ଫେରନ୍ତୁ &t_list_dict_any;
}

/*
 * Used for getloclist(): returns list if there is one argument, dict if there
 * are two.
 */
    ସ୍ଥିର type_T *
ret_list_or_dict_1(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    ଯଦି (argcount > 1)
	ଫେରନ୍ତୁ &t_dict_any;
    *decl_type = &t_list_any;
    ଫେରନ୍ତୁ &t_list_dict_any;
}

    ସ୍ଥିର type_T *
ret_argv(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // argv() returns list of strings
    ଯଦି (argcount == 0)
    {
	*decl_type = &t_list_any;
	ଫେରନ୍ତୁ &t_list_string;
    }

    // argv(0) returns a string, but argv(-1] returns a list
    ଫେରନ୍ତୁ &t_any;
}

    ସ୍ଥିର type_T *
ret_remove(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ଯଦି (argcount > 0)
    {
	ଯଦି (argtypes[0].type_curr->tt_type == VAR_LIST
		|| argtypes[0].type_curr->tt_type == VAR_DICT)
	{
	    ଯଦି (argcount == 3)
	    {
		*decl_type = argtypes[0].type_decl;
		ଫେରନ୍ତୁ argtypes[0].type_curr;
	    }
	    ଯଦି (argtypes[0].type_curr->tt_type
					     == argtypes[0].type_decl->tt_type)
		*decl_type = argtypes[0].type_decl->tt_member;
	    ଫେରନ୍ତୁ argtypes[0].type_curr->tt_member;
	}
	ଯଦି (argtypes[0].type_curr->tt_type == VAR_BLOB)
	    ଫେରନ୍ତୁ &t_ସଂଖ୍ୟା;
    }
    ଫେରନ୍ତୁ &t_any;
}

    ସ୍ଥିର type_T *
ret_max_min(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes,
	type_T	**decl_type)
{
    ଯଦି (argcount > 0)
    {
	type_T *t = argtypes[0].type_decl;
	ଯଦି (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	ଅନ୍ୟଥା
	    t = &t_any;
	*decl_type = t;

	t = argtypes[0].type_curr;
	ଯଦି (t->tt_type == VAR_LIST || t->tt_type == VAR_DICT)
	    t = t->tt_member;
	ଅନ୍ୟଥା
	    t = &t_any;
	ଫେରନ୍ତୁ t;
    }
    ଫେରନ୍ତୁ &t_any;
}

    ସ୍ଥିର type_T *
ret_getreg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the third argument is passed it's non-zero
    ଯଦି (argcount == 3)
    {
	*decl_type = &t_list_any;
	ଫେରନ୍ତୁ &t_list_string;
    }
    ଫେରନ୍ତୁ &t_string;
}

    ସ୍ଥିର type_T *
ret_virtcol(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type)
{
    // Assume that if the second argument is passed it's non-zero
    ଯଦି (argcount > 1)
    {
	*decl_type = &t_list_any;
	ଫେରନ୍ତୁ &t_list_ସଂଖ୍ୟା;
    }
    ଫେରନ୍ତୁ &t_ସଂଖ୍ୟା;
}

    ସ୍ଥିର type_T *
ret_maparg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount,
	type2_T *argtypes UNUSED,
	type_T	**decl_type UNUSED)
{
    // Assume that if the fourth argument is passed it's non-zero
    ଯଦି (argcount == 4)
	ଫେରନ୍ତୁ &t_dict_any;
    ଫେରନ୍ତୁ &t_string;
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ଅକ୍ଷର	*f_name;	// function name
    ଅକ୍ଷର	f_min_argc;	// minimal number of arguments
    ଅକ୍ଷର	f_max_argc;	// maximal number of arguments
    ଅକ୍ଷର	f_argtype;	// for method: FEARG_ values; bits FE_
    argcheck_T	*f_argcheck;	// list of functions to check argument types;
				// use "arg_any" (not NULL) to accept an
				// argument of any type
    ଅକ୍ଷର	*f_argnames;	// names of the arguments as in the help,
				// separated by ", "; an argument without a
				// name is empty, NULL when there is none
    type_T	*(*f_retfunc)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, type2_T *argtypes,
							   type_T **decl_type);
				// return type function
    ଶୂନ୍ୟ	(*f_func)(typval_T *args, typval_T *rvar);
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

#ଯଦି defined(HAVE_MATH_H)
# define MATH_FUNC(name) name
#ଅନ୍ୟଥା
# define MATH_FUNC(name) ଖାଲି
#endif
#ifdef FEAT_TIMERS
# define TIMER_FUNC(name) name
#ଅନ୍ୟଥା
# define TIMER_FUNC(name) ଖାଲି
#endif
#ifdef FEAT_JOB_CHANNEL
# define JOB_FUNC(name) name
#ଅନ୍ୟଥା
# define JOB_FUNC(name) ଖାଲି
#endif
#ifdef FEAT_PROP_POPUP
# define PROP_FUNC(name) name
#ଅନ୍ୟଥା
# define PROP_FUNC(name) ଖାଲି
#endif
#ifdef FEAT_SIGNS
# define SIGN_FUNC(name) name
#ଅନ୍ୟଥା
# define SIGN_FUNC(name) ଖାଲି
#endif
#ifdef FEAT_SOUND
# define SOUND_FUNC(name) name
#ଅନ୍ୟଥା
# define SOUND_FUNC(name) ଖାଲି
#endif
#ifdef FEAT_TERMINAL
# define TERM_FUNC(name) name
#ଅନ୍ୟଥା
# define TERM_FUNC(name) ଖାଲି
#endif
#ifdef FEAT_TABPANEL
# define TABPANEL_FUNC(name) name
#ଅନ୍ୟଥା
# define TABPANEL_FUNC(name) ଖାଲି
#endif
#ifdef FEAT_IMAGE
# define IMAGE_FUNC(name) name
#ଅନ୍ୟଥା
# define IMAGE_FUNC(name) ଖାଲି
#endif

ସ୍ଥିର ଅଚଳ funcentry_T global_functions[] =
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
    {"and",		2, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା,
			"expr1, expr2",
			ret_ସଂଖ୍ୟା,	    f_and},
    {"append",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_ସଂଖ୍ୟା_bool,    f_append},
    {"appendbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_ସଂଖ୍ୟା_bool,    f_appendbufline},
    {"argc",		0, 1, 0,	    arg1_ସଂଖ୍ୟା,
			"winid",
			ret_ସଂଖ୍ୟା,	    f_argc},
    {"argidx",		0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_argidx},
    {"arglistid",	0, 2, 0,	    arg2_ସଂଖ୍ୟା,
			"winnr, tabnr",
			ret_ସଂଖ୍ୟା,	    f_arglistid},
    {"argv",		0, 2, 0,	    arg2_ସଂଖ୍ୟା,
			"nr, winid",
			ret_argv,	    f_argv},
    {"asin",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_asin},
    {"assert_beeps",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_ସଂଖ୍ୟା_bool,    f_assert_beeps},
    {"assert_equal",	2, 3, FEARG_2,	    ଖାଲି,
			"expected, actual, msg",
			ret_ସଂଖ୍ୟା_bool,    f_assert_equal},
    {"assert_equalfile", 2, 3, FEARG_1,	    arg3_string,
			"fname_one, fname_two, msg",
			ret_ସଂଖ୍ୟା_bool,    f_assert_equalfile},
    {"assert_exception", 1, 2, 0,	    arg2_string,
			"error, msg",
			ret_ସଂଖ୍ୟା_bool,    f_assert_exception},
    {"assert_fails",	1, 5, FEARG_1,	    arg15_assert_fails,
			"cmd, error, msg, lnum, context",
			ret_ସଂଖ୍ୟା_bool,    f_assert_fails},
    {"assert_false",	1, 2, FEARG_1,	    ଖାଲି,
			"actual, msg",
			ret_ସଂଖ୍ୟା_bool,    f_assert_false},
    {"assert_inrange",	3, 4, FEARG_3,	    arg34_assert_inrange,
			"lower, upper, actual, msg",
			ret_ସଂଖ୍ୟା_bool,    f_assert_inrange},
    {"assert_match",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_ସଂଖ୍ୟା_bool,    f_assert_match},
    {"assert_nobeep",	1, 1, FEARG_1,	    arg1_string,
			"cmd",
			ret_ସଂଖ୍ୟା_bool,    f_assert_nobeep},
    {"assert_notequal",	2, 3, FEARG_2,	    ଖାଲି,
			"expected, actual, msg",
			ret_ସଂଖ୍ୟା_bool,    f_assert_notequal},
    {"assert_notmatch",	2, 3, FEARG_2,	    arg3_string,
			"pattern, actual, msg",
			ret_ସଂଖ୍ୟା_bool,    f_assert_notmatch},
    {"assert_report",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_ସଂଖ୍ୟା_bool,    f_assert_report},
    {"assert_true",	1, 2, FEARG_1,	    ଖାଲି,
			"actual, msg",
			ret_ସଂଖ୍ୟା_bool,    f_assert_true},
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
    {"balloon_gettext",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,
#ifdef FEAT_BEVAL
	    f_balloon_gettext
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
			},
    {"balloon_show",	1, 1, FEARG_1,	    arg1_string_or_list_any,
			"expr",
			ret_void,
#ifdef FEAT_BEVAL
	    f_balloon_show
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
			},
    {"balloon_split",	1, 1, FEARG_1,	    arg1_string,
			"msg",
			ret_list_string,
#ଯଦି defined(FEAT_BEVAL_TERM)
	    f_balloon_split
#ଅନ୍ୟଥା
	    ଖାଲି
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
			ret_list_ସଂଖ୍ୟା,    f_blob2list},
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
			ret_ସଂଖ୍ୟା,	    f_bufadd},
    {"bufexists",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_ସଂଖ୍ୟା_bool,    f_bufexists},
    {"buffer_exists",	1, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_ସଂଖ୍ୟା_bool,    f_bufexists},
    {"buffer_name",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_string,	    f_bufname},
    {"buffer_number",	0, 1, FEARG_1,	    arg1_buffer,	// obsolete
			"buf",
			ret_ସଂଖ୍ୟା,	    f_bufnr},
    {"buflisted",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_ସଂଖ୍ୟା_bool,    f_buflisted},
    {"bufload",		1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_bufload},
    {"bufloaded",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_ସଂଖ୍ୟା_bool,    f_bufloaded},
    {"bufname",		0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_bufname},
    {"bufnr",		0, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, create",
			ret_ସଂଖ୍ୟା,	    f_bufnr},
    {"bufwinid",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_ସଂଖ୍ୟା,	    f_bufwinid},
    {"bufwinnr",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_ସଂଖ୍ୟା,	    f_bufwinnr},
    {"byte2line",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"byte",
			ret_ସଂଖ୍ୟା,	    f_byte2line},
    {"byteidx",		2, 3, FEARG_1,	    arg3_string_ସଂଖ୍ୟା_bool,
			"expr, nr, utf16",
			ret_ସଂଖ୍ୟା,	    f_byteidx},
    {"byteidxcomp",	2, 3, FEARG_1,	    arg3_string_ସଂଖ୍ୟା_bool,
			"expr, nr, utf16",
			ret_ସଂଖ୍ୟା,	    f_byteidxcomp},
    {"call",		2, 3, FEARG_1,	    arg3_any_list_dict,
			"func, arglist, dict",
			ret_any,	    f_call},
    {"ceil",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_ceil},
    {"ch_canread",	1, 1, FEARG_1,	    arg1_chan_or_job,
			"handle",
			ret_ସଂଖ୍ୟା_bool,    JOB_FUNC(f_ch_canread)},
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
			ret_ସଂଖ୍ୟା,	    JOB_FUNC(f_ch_getbufnr)},
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
    {"changenr",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_changenr},
    {"char2nr",		1, 2, FEARG_1,	    arg2_string_bool,
			"string, utf8",
			ret_ସଂଖ୍ୟା,	    f_char2nr},
    {"charclass",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_ସଂଖ୍ୟା,	    f_charclass},
    {"charcol",		1, 2, FEARG_1,	    arg2_string_or_list_ସଂଖ୍ୟା,
			"expr, winid",
			ret_ସଂଖ୍ୟା,	    f_charcol},
    {"charidx",		2, 4, FEARG_1,	    arg4_string_ସଂଖ୍ୟା_bool_bool,
			"string, idx, countcc, utf16",
			ret_ସଂଖ୍ୟା,	    f_charidx},
    {"chdir",		1, 2, FEARG_1,	    arg2_string,
			"dir, scope",
			ret_string,	    f_chdir},
    {"cindent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ସଂଖ୍ୟା,	    f_cindent},
    {"clearmatches",	0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"win",
			ret_void,	    f_clearmatches},
    {"cmdcomplete_info",0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_dict_any,	    f_cmdcomplete_info},
    {"col",		1, 2, FEARG_1,	    arg2_string_or_list_ସଂଖ୍ୟା,
			"expr, winid",
			ret_ସଂଖ୍ୟା,	    f_col},
    {"complete",	2, 2, FEARG_2,	    arg2_ସଂଖ୍ୟା_list,
			"startcol, matches",
			ret_void,	    f_complete},
    {"complete_add",	1, 1, FEARG_1,	    arg1_dict_or_string,
			"expr",
			ret_ସଂଖ୍ୟା,	    f_complete_add},
    {"complete_check",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା_bool,    f_complete_check},
    {"complete_info",	0, 1, FEARG_1,	    arg1_list_string,
			"what",
			ret_dict_any,	    f_complete_info},
    {"confirm",		1, 4, FEARG_1,	    arg4_string_string_ସଂଖ୍ୟା_string,
			"msg, choices, default, type",
			ret_ସଂଖ୍ୟା,	    f_confirm},
    {"copy",		1, 1, FEARG_1,	    ଖାଲି,
			"expr",
			ret_copy,	    f_copy},
    {"cos",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cos},
    {"cosh",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_cosh},
    {"count",		2, 4, FEARG_1,	    arg24_ଗଣନା,
			"comp, expr, ic, start",
			ret_ସଂଖ୍ୟା,	    f_ଗଣନା},
    {"cscope_connection",0,3, 0,	    arg3_ସଂଖ୍ୟା_string_string,
			"num, dbpath, prepend",
			ret_ସଂଖ୍ୟା,	    f_cscope_connection},
    {"cursor",		1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_ସଂଖ୍ୟା,	    f_cursor},
    {"debugbreak",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"pid",
			ret_ସଂଖ୍ୟା,
#ifdef MSWIN
	    f_debugbreak
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
			},
    {"deepcopy",	1, 2, FEARG_1,	    arg12_deepcopy,
			"expr, noref",
			ret_copy,	    f_deepcopy},
    {"delete",		1, 2, FEARG_1,	    arg2_string,
			"fname, flags",
			ret_ସଂଖ୍ୟା_bool,    f_delete},
    {"deletebufline",	2, 3, FEARG_1,	    arg3_buffer_lnum_lnum,
			"buf, first, last",
			ret_ସଂଖ୍ୟା_bool,    f_deletebufline},
    {"did_filetype",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା_bool,    f_did_filetype},
    {"diff",		2, 3, FEARG_1,	    arg3_diff,
			"fromlist, tolist, options",
			ret_string_or_any_3,  f_diff},
    {"diff_filler",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ସଂଖ୍ୟା,	    f_diff_filler},
    {"diff_hlID",	2, 2, FEARG_1,	    arg2_lnum_ସଂଖ୍ୟା,
			"lnum, col",
			ret_ସଂଖ୍ୟା,	    f_diff_hlID},
    {"digraph_get",	1, 1, FEARG_1,	    arg1_string,
			"chars",
			ret_string,	    f_digraph_get},
    {"digraph_getlist",	0, 1, FEARG_1,	    arg1_bool,
			"listall",
			ret_list_string_ବସ୍ତୁଗୁଡ଼ିକ, f_digraph_getlist},
    {"digraph_set",	2, 2, FEARG_1,	    arg2_string,
			"chars, digraph",
			ret_bool,	f_digraph_set},
    {"digraph_setlist",	1, 1, FEARG_1,	    arg1_list_string,
			"digraphlist",
			ret_bool,	    f_digraph_setlist},
    {"echoraw",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_echoraw},
    {"empty",		1, 1, FEARG_1,	    ଖାଲି,
			"expr",
			ret_ସଂଖ୍ୟା_bool,    f_empty},
    {"environ",		0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_dict_string,    f_environ},
    {"err_teapot",	0, 1, 0,	    ଖାଲି,
			"expr",
			ret_void,	    f_err_teapot},
    {"escape",		2, 2, FEARG_1,	    arg2_string,
			"string, chars",
			ret_string,	    f_escape},
    {"eval",		1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_eval},
    {"eventhandler",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା_bool,    f_eventhandler},
    {"executable",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_ସଂଖ୍ୟା,	    f_executable},
    {"execute",		1, 2, FEARG_1,	    arg12_execute,
			"command, silent",
			ret_string,	    f_execute},
    {"exepath",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_string,	    f_exepath},
    {"exists",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_ସଂଖ୍ୟା_bool,    f_exists},
    {"exists_compiled",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_ସଂଖ୍ୟା_bool,    f_exists_compiled},
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
			ret_ସଂଖ୍ୟା_bool,    f_filereadable},
    {"filecopy",	2, 2, FEARG_1,	    arg2_string,
			"from, to",
			ret_ସଂଖ୍ୟା_bool,    f_filecopy},
    {"filereadable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_ସଂଖ୍ୟା_bool,    f_filereadable},
    {"filewritable",	1, 1, FEARG_1,	    arg1_string,
			"file",
			ret_ସଂଖ୍ୟା,	    f_filewritable},
    {"filter",		2, 2, FEARG_1,	    arg2_filter,
			"expr1, expr2",
			ret_first_arg,	    f_filter},
    {"finddir",		1, 3, FEARG_1,	    arg3_string_string_ସଂଖ୍ୟା,
			"name, path, count",
			ret_string_or_any_3,	    f_finddir},
    {"findfile",	1, 3, FEARG_1,	    arg3_string_string_ସଂଖ୍ୟା,
			"name, path, count",
			ret_string_or_any_3,	    f_findfile},
    {"flatten",		1, 2, FEARG_1,	    arg2_list_any_ସଂଖ୍ୟା,
			"list, maxdepth",
			ret_list_any,	    f_flatten},
    {"flattennew",	1, 2, FEARG_1,	    arg2_list_any_ସଂଖ୍ୟା,
			"list, maxdepth",
			ret_list_any,	    f_flattennew},
    {"float2nr",	1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_ସଂଖ୍ୟା,	    f_float2nr},
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
    {"foldclosed",	1, 2, FEARG_1,	    arg2_lnum_ସଂଖ୍ୟା,
			"lnum, winid",
			ret_ସଂଖ୍ୟା,	    f_foldclosed},
    {"foldclosedend",	1, 2, FEARG_1,	    arg2_lnum_ସଂଖ୍ୟା,
			"lnum, winid",
			ret_ସଂଖ୍ୟା,	    f_foldclosedend},
    {"foldlevel",	1, 2, FEARG_1,	    arg2_lnum_ସଂଖ୍ୟା,
			"lnum, winid",
			ret_ସଂଖ୍ୟା,	    f_foldlevel},
    {"foldtext",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_foldtext},
    {"foldtextresult",	1, 2, FEARG_1,	    arg2_lnum_ସଂଖ୍ୟା,
			"lnum, winid",
			ret_string,	    f_foldtextresult},
    {"foreach",		2, 2, FEARG_1,	    arg2_foreach,
			"expr1, expr2",
			ret_first_arg,	    f_foreach},
    {"foreground",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
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
    {"getbgcolor",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
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
    {"getcellpixels",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_list_any,	    f_getcellpixels},
    {"getcellwidths",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_list_any,	    f_getcellwidths},
    {"getchangelist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_any,	    f_getchangelist},
    {"getchar",		0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_any,	    f_getchar},
    {"getcharmod",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_getcharmod},
    {"getcharpos",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_ସଂଖ୍ୟା,    f_getcharpos},
    {"getcharsearch",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_dict_any,	    f_getcharsearch},
    {"getcharstr",	0, 2, 0,	    arg12_getchar,
			"expr, opts",
			ret_string,	    f_getcharstr},
    {"getcmdcomplpat",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_getcmdcomplpat},
    {"getcmdcompltype",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_getcmdcompltype},
    {"getcmdline",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_getcmdline},
    {"getcmdpos",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_getcmdpos},
    {"getcmdprompt",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_getcmdprompt},
    {"getcmdscreenpos",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_getcmdscreenpos},
    {"getcmdtype",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_getcmdtype},
    {"getcmdwintype",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_getcmdwintype},
    {"getcompletion",	2, 3, FEARG_1,	    arg3_string_string_bool,
			"pat, type, filtered",
			ret_list_string,    f_getcompletion},
    {"getcompletiontype", 1, 1, FEARG_1,    arg1_string,
			"pat",
			ret_string,	    f_getcompletiontype},
    {"getcurpos",	0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"winid",
			ret_list_ସଂଖ୍ୟା,    f_getcurpos},
    {"getcursorcharpos", 0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"winid",
			ret_list_ସଂଖ୍ୟା,    f_getcursorcharpos},
    {"getcwd",		0, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା,
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
			ret_ସଂଖ୍ୟା,	    f_getfsize},
    {"getftime",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_ସଂଖ୍ୟା,	    f_getftime},
    {"getftype",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_string,	    f_getftype},
    {"getimstatus",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା_bool,    f_getimstatus},
    {"getinfo",		2, 3, FEARG_2,	    arg3_string_string_dict,
			"kind, name, opts",
			ret_dict_any,	    f_getinfo},
    {"getjumplist",	0, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା,
			"winnr, tabnr",
			ret_list_any,	    f_getjumplist},
    {"getline",		1, 2, FEARG_1,	    arg2_lnum,
			"lnum, end",
			ret_getline,	    f_getline},
    {"getloclist",	1, 2, 0,	    arg2_ସଂଖ୍ୟା_dict_any,
			"nr, what",
			ret_list_or_dict_1, f_getloclist},
    {"getmarklist",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_dict_any,  f_getmarklist},
    {"getmatches",	0, 1, 0,	    arg1_ସଂଖ୍ୟା,
			"win",
			ret_list_dict_any,  f_getmatches},
    {"getmousepos",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_dict_ସଂଖ୍ୟା,    f_getmousepos},
    {"getmouseshape",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_getmouseshape},
    {"getpid",		0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_getpid},
    {"getpos",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_list_ସଂଖ୍ୟା,    f_getpos},
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
    {"getstacktrace",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_list_dict_any,  f_getstacktrace},
    {"gettabinfo",	0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"tabnr",
			ret_list_dict_any,  f_gettabinfo},
    {"gettabvar",	2, 3, FEARG_1,	    arg3_ସଂଖ୍ୟା_string_any,
			"tabnr, varname, def",
			ret_any,	    f_gettabvar},
    {"gettabwinvar",	3, 4, FEARG_1,	    arg4_ସଂଖ୍ୟା_ସଂଖ୍ୟା_string_any,
			"tabnr, winnr, varname, def",
			ret_any,	    f_gettabwinvar},
    {"gettagstack",	0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"winnr",
			ret_dict_any,	    f_gettagstack},
    {"gettext",		1, 2, FEARG_1,	    arg2_string,
			"text, package",
			ret_string,	    f_gettext},
    {"getwininfo",	0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"winid",
			ret_list_dict_any,  f_getwininfo},
    {"getwinpos",	0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"timeout",
			ret_list_ସଂଖ୍ୟା,    f_getwinpos},
    {"getwinposx",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_getwinposx},
    {"getwinposy",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_getwinposy},
    {"getwinvar",	2, 3, FEARG_1,	    arg3_ସଂଖ୍ୟା_string_any,
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
			ret_ସଂଖ୍ୟା_bool,    f_has},
    {"has_key",		2, 2, FEARG_1,	    arg2_dict_any_string_or_nr,
			"dict, key",
			ret_ସଂଖ୍ୟା_bool,    f_has_ଚାବି},
    {"haslocaldir",	0, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା,
			"winnr, tabnr",
			ret_ସଂଖ୍ୟା,	    f_haslocaldir},
    {"hasmapto",	1, 3, FEARG_1,	    arg3_string_string_bool,
			"what, mode, abbr",
			ret_ସଂଖ୍ୟା_bool,    f_hasmapto},
    {"highlightID",	1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_ସଂଖ୍ୟା,	    f_hlID},
    {"highlight_exists",1, 1, FEARG_1,	    arg1_string,	// obsolete
			"name",
			ret_ସଂଖ୍ୟା_bool,    f_hlexists},
    {"histadd",		2, 2, FEARG_2,	    arg2_string,
			"history, item",
			ret_ସଂଖ୍ୟା_bool,    f_histadd},
    {"histdel",		1, 2, FEARG_1,	    arg2_string_string_or_ସଂଖ୍ୟା,
			"history, item",
			ret_ସଂଖ୍ୟା_bool,    f_histdel},
    {"histget",		1, 2, FEARG_1,	    arg2_string_ସଂଖ୍ୟା,
			"history, index",
			ret_string,	    f_histget},
    {"histnr",		1, 1, FEARG_1,	    arg1_string,
			"history",
			ret_ସଂଖ୍ୟା,	    f_histnr},
    {"hlID",		1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_ସଂଖ୍ୟା,	    f_hlID},
    {"hlexists",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_ସଂଖ୍ୟା_bool,    f_hlexists},
    {"hlget",		0, 2, FEARG_1,	    arg2_string_bool,
			"name, resolve",
			ret_list_dict_any,  f_hlget},
    {"hlset",		1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_ସଂଖ୍ୟା_bool,    f_hlset},
    {"hostname",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_hostname},
    {"iconv",		3, 3, FEARG_1,	    arg3_string,
			"string, from, to",
			ret_string,	    f_iconv},
    {"id",		1, 1, FEARG_1,	    ଖାଲି,
			"item",
			ret_string,	    f_id},
    {"image_add",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_ସଂଖ୍ୟା,	    IMAGE_FUNC(f_image_add)},
    {"image_discard",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"id",
			ret_void,	    IMAGE_FUNC(f_image_discard)},
    {"image_info",	0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"id",
			ret_list_dict_any,  IMAGE_FUNC(f_image_info)},
    {"indent",		1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ସଂଖ୍ୟା,	    f_indent},
    {"index",		2, 4, FEARG_1,	    arg24_index,
			"object, expr, start, ic",
			ret_ସଂଖ୍ୟା,	    f_index},
    {"indexof",		2, 3, FEARG_1,	    arg23_index,
			"object, expr, opts",
			ret_ସଂଖ୍ୟା,	    f_indexof},
    {"input",		1, 3, FEARG_1,	    arg3_string,
			"prompt, text, completion",
			ret_string,	    f_ନିବେଶ},
    {"inputdialog",	1, 3, FEARG_1,	    arg3_string,
			"prompt, text, cancelreturn",
			ret_string,	    f_inputdialog},
    {"inputlist",	1, 1, FEARG_1,	    arg1_list_string,
			"textlist",
			ret_ସଂଖ୍ୟା,	    f_inputlist},
    {"inputrestore",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା_bool,    f_inputrestore},
    {"inputsave",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା_bool,    f_inputsave},
    {"inputsecret",	1, 2, FEARG_1,	    arg2_string,
			"prompt, text",
			ret_string,	    f_inputsecret},
    {"insert",		2, 3, FEARG_1,	    arg23_ଭର୍ତ୍ତି,
			"object, item, idx",
			ret_first_arg,	    f_ଭର୍ତ୍ତି},
    {"instanceof",	2, VARGS, FEARG_1|FE_X,	arg2_instanceof,
			"object, class",
			ret_bool,	    f_instanceof},
    {"interrupt",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_void,	    f_interrupt},
    {"invert",		1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"expr",
			ret_ସଂଖ୍ୟା,	    f_invert},
    {"isabsolutepath",	1, 1, FEARG_1,	    arg1_string,
			"path",
			ret_ସଂଖ୍ୟା_bool,    f_isabsolutepath},
    {"isdirectory",	1, 1, FEARG_1,	    arg1_string,
			"directory",
			ret_ସଂଖ୍ୟା_bool,    f_isdirectory},
    {"isinf",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_ସଂଖ୍ୟା,	    MATH_FUNC(f_isinf)},
    {"islocked",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_ସଂଖ୍ୟା_bool,    f_islocked},
    {"isnan",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_ସଂଖ୍ୟା_bool,    MATH_FUNC(f_isnan)},
    {"items",		1, 1, FEARG_1,	    arg1_list_tuple_dict_blob_or_string,
			"expr",
			ret_list_ବସ୍ତୁଗୁଡ଼ିକ,	    f_ବସ୍ତୁଗୁଡ଼ିକ},
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
    {"job_stop",	1, 2, FEARG_1,	    arg2_job_string_or_ସଂଖ୍ୟା,
			"job, how",
			ret_ସଂଖ୍ୟା_bool,    JOB_FUNC(f_job_stop)},
    {"join",		1, 2, FEARG_1,	    arg2_list_or_tuple_string,
			"expr, sep",
			ret_string,	    f_join},
    {"js_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_js_decode},
    {"js_encode",	1, 1, FEARG_1,	    ଖାଲି,
			"expr",
			ret_string,	    f_js_encode},
    {"json_decode",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_any,	    f_json_decode},
    {"json_encode",	1, 1, FEARG_1,	    ଖାଲି,
			"expr",
			ret_string,	    f_json_encode},
    {"keys",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_string,    f_ଚାବିଗୁଡ଼ିକ},
    {"keytrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_keytrans},
    {"last_buffer_nr",	0, 0, 0,	    ଖାଲି,	// obsolete
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_last_buffer_nr},
    {"len",		1, 1, FEARG_1,	    arg1_len,
			"expr",
			ret_ସଂଖ୍ୟା,	    f_len},
    {"libcall",		3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_string,	    f_libcall},
    {"libcallnr",	3, 3, FEARG_3,	    arg3_libcall,
			"libname, funcname, argument",
			ret_ସଂଖ୍ୟା,	    f_libcallnr},
    {"line",		1, 2, FEARG_1,	    arg2_string_ସଂଖ୍ୟା,
			"expr, winid",
			ret_ସଂଖ୍ୟା,	    f_ଧାଡ଼ି},
    {"line2byte",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ସଂଖ୍ୟା,	    f_line2byte},
    {"lispindent",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ସଂଖ୍ୟା,	    f_lispindent},
    {"list2blob",	1, 1, FEARG_1,	    arg1_list_ସଂଖ୍ୟା,
			"list",
			ret_blob,	    f_list2blob},
    {"list2str",	1, 2, FEARG_1,	    arg2_list_ସଂଖ୍ୟା_bool,
			"list, utf8",
			ret_string,	    f_list2str},
    {"list2tuple",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_tuple_any,	    f_list2tuple},
    {"listener_add",	1, 3, FEARG_2,	    arg3_any_buffer_bool_or_dict,
			"callback, buf, options",
			ret_ସଂଖ୍ୟା,	    f_listener_add},
    {"listener_flush",	0, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_void,	    f_listener_flush},
    {"listener_remove",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"id",
			ret_ସଂଖ୍ୟା_bool,    f_listener_remove},
    {"localtime",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_localtime},
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
#ଅନ୍ୟଥା
		ଖାଲି
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
			ret_ସଂଖ୍ୟା,	    f_match},
    {"matchadd",	2, 5, FEARG_1,	    arg25_matchadd,
			"group, pattern, priority, id, dict",
			ret_ସଂଖ୍ୟା,	    f_matchadd},
    {"matchaddpos",	2, 5, FEARG_1,	    arg25_matchaddpos,
			"group, pos, priority, id, dict",
			ret_ସଂଖ୍ୟା,	    f_matchaddpos},
    {"matcharg",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"nr",
			ret_list_string,    f_matcharg},
    {"matchbufline",	4, 5, FEARG_1,	    arg45_matchbufline,
			"buf, pat, lnum, end, dict",
			ret_list_any,	    f_matchbufline},
    {"matchdelete",	1, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା,
			"id, win",
			ret_ସଂଖ୍ୟା_bool,    f_matchdelete},
    {"matchend",	2, 4, FEARG_1,	    arg24_match_func,
			"expr, pat, start, count",
			ret_ସଂଖ୍ୟା,	    f_matchend},
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
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
			},
    {"min",		1, 1, FEARG_1,	    arg1_list_or_tuple_or_dict,
			"expr",
			ret_max_min,	    f_min},
    {"mkdir",		1, 3, FEARG_1,	    arg3_string_string_ସଂଖ୍ୟା,
			"name, flags, prot",
			ret_ସଂଖ୍ୟା_bool,    f_mkdir},
    {"mode",		0, 1, FEARG_1,	    arg1_bool,
			"expr",
			ret_string,	    f_mode},
    {"mzeval",		1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_MZSCHEME
	    f_mzeval
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
			},
    {"nextnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ସଂଖ୍ୟା,	    f_nextnonblank},
    {"ngettext",	3, 4, FEARG_3,	    arg4_string_string_ସଂଖ୍ୟା_string,
			"single, plural, number, domain",
			ret_string,	    f_ngettext},
    {"nr2char",		1, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା_bool,
			"expr, utf8",
			ret_string,	    f_nr2char},
    {"or",		2, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା,
			"expr1, expr2",
			ret_ସଂଖ୍ୟା,	    f_or},
    {"pathshorten",	1, 2, FEARG_1,	    arg2_string_ସଂଖ୍ୟା,
			"path, len",
			ret_string,	    f_pathshorten},
    {"perleval",	1, 1, FEARG_1,	    arg1_string,
			"expr",
			ret_any,
#ifdef FEAT_PERL
	    f_perleval
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
			},
    {"popup_atcursor",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ସଂଖ୍ୟା,	    PROP_FUNC(f_popup_atcursor)},
    {"popup_beval",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ସଂଖ୍ୟା,	    PROP_FUNC(f_popup_beval)},
    {"popup_clear",	0, 1, 0,	    arg1_bool,
			"force",
			ret_void,	    PROP_FUNC(f_popup_clear)},
    {"popup_close",	1, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା_any,
			"id, result",
			ret_void,	    PROP_FUNC(f_popup_close)},
    {"popup_create",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ସଂଖ୍ୟା,	    PROP_FUNC(f_popup_create)},
    {"popup_dialog",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ସଂଖ୍ୟା,	    PROP_FUNC(f_popup_dialog)},
    {"popup_filter_menu", 2, 2, 0,	    arg2_ସଂଖ୍ୟା_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_menu)},
    {"popup_filter_yesno", 2, 2, 0,	    arg2_ସଂଖ୍ୟା_string,
			"id, key",
			ret_bool,	    PROP_FUNC(f_popup_filter_yesno)},
    {"popup_findecho",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    PROP_FUNC(f_popup_findecho)},
    {"popup_findinfo",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    PROP_FUNC(f_popup_findinfo)},
    {"popup_findpreview", 0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    PROP_FUNC(f_popup_findpreview)},
    {"popup_getoptions", 1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getoptions)},
    {"popup_getpos",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"id",
			ret_dict_any,	    PROP_FUNC(f_popup_getpos)},
    {"popup_hide",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"id",
			ret_void,	    PROP_FUNC(f_popup_hide)},
    {"popup_list",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_list_ସଂଖ୍ୟା,    PROP_FUNC(f_popup_list)},
    {"popup_locate",	2, 2, 0,	    arg2_ସଂଖ୍ୟା,
			"row, col",
			ret_ସଂଖ୍ୟା,	    PROP_FUNC(f_popup_locate)},
    {"popup_menu",	2, 2, FEARG_1,	    arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ସଂଖ୍ୟା,	    PROP_FUNC(f_popup_menu)},
    {"popup_move",	2, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_move)},
    {"popup_notification", 2, 2, FEARG_1,   arg2_str_or_nr_or_list_dict,
			"what, options",
			ret_ସଂଖ୍ୟା,	    PROP_FUNC(f_popup_notification)},
    {"popup_setbuf",	2, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା_buffer,
			"id, buf",
			ret_ସଂଖ୍ୟା_bool,    PROP_FUNC(f_popup_setbuf)},
    {"popup_setoptions", 2, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା_dict_any,
			"id, options",
			ret_void,	    PROP_FUNC(f_popup_setoptions)},
    {"popup_settext",	2, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା_string_or_list,
			"id, text",
			ret_void,	    PROP_FUNC(f_popup_settext)},
    {"popup_show",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"id",
			ret_ସଂଖ୍ୟା,	    PROP_FUNC(f_popup_show)},
    {"pow",		2, 2, FEARG_1,	    arg2_float_or_nr,
			"x, y",
			ret_float,	    f_pow},
    {"preinserted",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା_bool,    f_preinserted},
    {"prevnonblank",	1, 1, FEARG_1,	    arg1_lnum,
			"lnum",
			ret_ସଂଖ୍ୟା,	    f_prevnonblank},
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
    {"prop_add",	3, 3, FEARG_1,	    arg3_ସଂଖ୍ୟା_ସଂଖ୍ୟା_dict,
			"lnum, col, props",
			ret_ସଂଖ୍ୟା,	    PROP_FUNC(f_prop_add)},
    {"prop_add_list",	2, 2, FEARG_1,	    arg2_dict_any_list_any,
			"props",
			ret_void,	    PROP_FUNC(f_prop_add_list)},
    {"prop_clear",	1, 3, FEARG_1,	    arg3_ସଂଖ୍ୟା_ସଂଖ୍ୟା_dict,
			"lnum, lnum_end, props",
			ret_void,	    PROP_FUNC(f_prop_clear)},
    {"prop_find",	1, 2, FEARG_1,	    arg2_dict_string,
			"props, direction",
			ret_dict_any,	    PROP_FUNC(f_prop_find)},
    {"prop_list",	1, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା_dict_any,
			"lnum, props",
			ret_list_dict_any,  PROP_FUNC(f_prop_list)},
    {"prop_remove",	1, 3, FEARG_1,	    arg3_dict_ସଂଖ୍ୟା_ସଂଖ୍ୟା,
			"props, lnum, lnum_end",
			ret_ସଂଖ୍ୟା,	    PROP_FUNC(f_prop_remove)},
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
    {"pum_getpos",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_dict_ସଂଖ୍ୟା,    f_pum_getpos},
    {"pumvisible",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା_bool,    f_pumvisible},
    {"py3eval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON3
	    f_py3eval
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
	    },
    {"pyeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ifdef FEAT_PYTHON
	    f_pyeval
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
			},
    {"pyxeval",		1, 2, FEARG_1,	    arg2_string_dict,
			"expr, locals",
			ret_any,
#ଯଦି defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
	    f_pyxeval
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
			},
    {"rand",		0, 1, FEARG_1,	    arg1_list_ସଂଖ୍ୟା,
			"expr",
			ret_ସଂଖ୍ୟା,	    f_rand},
    {"range",		1, 3, FEARG_1,	    arg3_ସଂଖ୍ୟା,
			"expr, max, stride",
			ret_list_ସଂଖ୍ୟା,    f_range},
    {"readblob",	1, 3, FEARG_1,	    arg3_string_ସଂଖ୍ୟା_ସଂଖ୍ୟା,
			"fname, offset, size",
			ret_blob,	    f_readblob},
    {"readdir",		1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_string,    f_readdir},
    {"readdirex",	1, 3, FEARG_1,	    arg3_string_any_dict,
			"directory, expr, dict",
			ret_list_dict_any,  f_readdirex},
    {"readfile",	1, 3, FEARG_1,	    arg3_string_string_ସଂଖ୍ୟା,
			"fname, type, max",
			ret_list_string,    f_readfile},
    {"redraw_listener_add", 1, 1, FEARG_1,  arg1_dict_any,
			"opts",
			ret_ସଂଖ୍ୟା,	    f_redraw_listener_add},
    {"redraw_listener_remove", 1, 1, FEARG_1, arg1_ସଂଖ୍ୟା,
			"id",
			ret_void,	    f_redraw_listener_remove},
    {"reduce",		2, 3, FEARG_1,	    arg23_reduce,
			"object, func, initial",
			ret_any,	    f_reduce},
    {"reg_executing",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_reg_executing},
    {"reg_recording",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_reg_recording},
    {"reltime",		0, 2, FEARG_1,	    arg2_list_ସଂଖ୍ୟା,
			"start, end",
			ret_list_ସଂଖ୍ୟା,    f_reltime},
    {"reltimefloat",	1, 1, FEARG_1,	    arg1_list_ସଂଖ୍ୟା,
			"time",
			ret_float,	    f_reltimefloat},
    {"reltimestr",	1, 1, FEARG_1,	    arg1_list_ସଂଖ୍ୟା,
			"time",
			ret_string,	    f_reltimestr},
    {"remote_expr",	2, 4, FEARG_1,	    arg24_remote_ପଦସମଷ୍ଟି,
			"server, string, idvar, timeout",
			ret_string,	    f_remote_ପଦସମଷ୍ଟି},
    {"remote_foreground", 1, 1, FEARG_1,    arg1_string,
			"server",
			ret_void,	    f_remote_foreground},
    {"remote_peek",	1, 2, FEARG_1,	    arg2_string,
			"serverid, retvar",
			ret_ସଂଖ୍ୟା,	    f_remote_peek},
    {"remote_read",	1, 2, FEARG_1,	    arg2_string_ସଂଖ୍ୟା,
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
			ret_ସଂଖ୍ୟା_bool,    f_rename},
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
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
			},
    {"screenattr",	2, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା,
			"row, col",
			ret_ସଂଖ୍ୟା,	    f_screenattr},
    {"screenchar",	2, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା,
			"row, col",
			ret_ସଂଖ୍ୟା,	    f_screenchar},
    {"screenchars",	2, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା,
			"row, col",
			ret_list_ସଂଖ୍ୟା,    f_screenchars},
    {"screencol",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_screencol},
    {"screenpos",	3, 3, FEARG_1,	    arg3_ସଂଖ୍ୟା,
			"winid, lnum, col",
			ret_dict_ସଂଖ୍ୟା,    f_screenpos},
    {"screenrow",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_screenrow},
    {"screenstring",	2, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା,
			"row, col",
			ret_string,	    f_screenstring},
    {"search",		1, 5, FEARG_1,	    arg15_ସନ୍ଧାନ,
			"pattern, flags, stopline, timeout, skip",
			ret_ସଂଖ୍ୟା,	    f_ସନ୍ଧାନ},
    {"searchcount",	0, 1, FEARG_1,	    arg1_dict_any,
			"options",
			ret_dict_ସଂଖ୍ୟା,    f_searchcount},
    {"searchdecl",	1, 3, FEARG_1,	    arg3_string_bool_bool,
			"name, global, thisblock",
			ret_ସଂଖ୍ୟା_bool,    f_searchdecl},
    {"searchpair",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_ସଂଖ୍ୟା,	    f_searchpair},
    {"searchpairpos",	3, 7, 0,	    arg37_searchpair,
			"start, middle, end, flags, skip, stopline, timeout",
			ret_list_ସଂଖ୍ୟା,    f_searchpairpos},
    {"searchpos",	1, 5, FEARG_1,	    arg15_ସନ୍ଧାନ,
			"pattern, flags, stopline, timeout, skip",
			ret_list_ସଂଖ୍ୟା,    f_searchpos},
    {"server2client",	2, 2, FEARG_1,	    arg2_string,
			"clientid, string",
			ret_ସଂଖ୍ୟା_bool,    f_server2client},
    {"serverlist",	0, 1, 0,	    arg1_dict_any,
			"dict",
			ret_string_or_any_1,	    f_serverlist},
    {"setbufline",	3, 3, FEARG_3,	    arg3_setbufline,
			"buf, lnum, text",
			ret_ସଂଖ୍ୟା_bool,    f_setbufline},
    {"setbufvar",	3, 3, FEARG_3,	    arg3_buffer_string_any,
			"buf, varname, val",
			ret_void,	    f_setbufvar},
    {"setcellwidths",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_void,	    f_setcellwidths},
    {"setcharpos",	2, 2, FEARG_2,	    arg2_string_list_ସଂଖ୍ୟା,
			"expr, list",
			ret_ସଂଖ୍ୟା_bool,    f_setcharpos},
    {"setcharsearch",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_setcharsearch},
    {"setcmdline",	1, 2, FEARG_1,	    arg2_string_ସଂଖ୍ୟା,
			"str, pos",
			ret_ସଂଖ୍ୟା_bool,    f_setcmdline},
    {"setcmdpos",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"pos",
			ret_ସଂଖ୍ୟା_bool,    f_setcmdpos},
    {"setcursorcharpos", 1, 3, FEARG_1,	    arg13_cursor,
			"lnum, col, off",
			ret_ସଂଖ୍ୟା_bool,    f_setcursorcharpos},
    {"setenv",		2, 2, FEARG_2,	    arg2_string_any,
			"name, val",
			ret_void,	    f_setenv},
    {"setfperm",	2, 2, FEARG_1,	    arg2_string,
			"fname, mode",
			ret_ସଂଖ୍ୟା_bool,    f_setfperm},
    {"setline",		2, 2, FEARG_2,	    arg2_setline,
			"lnum, text",
			ret_ସଂଖ୍ୟା_bool,    f_setline},
    {"setloclist",	2, 4, FEARG_2,	    arg24_setloclist,
			"nr, list, action, what",
			ret_ସଂଖ୍ୟା_bool,    f_setloclist},
    {"setmatches",	1, 2, FEARG_1,	    arg2_list_any_ସଂଖ୍ୟା,
			"list, win",
			ret_ସଂଖ୍ୟା_bool,    f_setmatches},
    {"setpos",		2, 2, FEARG_2,	    arg2_string_list_ସଂଖ୍ୟା,
			"expr, list",
			ret_ସଂଖ୍ୟା_bool,    f_setpos},
    {"setqflist",	1, 3, FEARG_1,	    arg13_setqflist,
			"list, action, what",
			ret_ସଂଖ୍ୟା_bool,    f_setqflist},
    {"setreg",		2, 3, FEARG_2,	    arg3_string_any_string,
			"regname, value, options",
			ret_ସଂଖ୍ୟା_bool,    f_setreg},
    {"settabvar",	3, 3, FEARG_3,	    arg3_ସଂଖ୍ୟା_string_any,
			"tabnr, varname, val",
			ret_void,	    f_settabvar},
    {"settabwinvar",	4, 4, FEARG_4,	    arg4_ସଂଖ୍ୟା_ସଂଖ୍ୟା_string_any,
			"tabnr, winnr, varname, val",
			ret_void,	    f_settabwinvar},
    {"settagstack",	2, 3, FEARG_2,	    arg23_settagstack,
			"nr, dict, action",
			ret_ସଂଖ୍ୟା_bool,    f_settagstack},
    {"setwinvar",	3, 3, FEARG_3,	    arg3_ସଂଖ୍ୟା_string_any,
			"winnr, varname, val",
			ret_void,	    f_setwinvar},
    {"sha256",		1, 1, FEARG_1,	    arg1_string_or_blob,
			"expr",
			ret_string,
#ifdef FEAT_CRYPT
	    f_sha256
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
			},
    {"shellescape",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, special",
			ret_string,	    f_shellescape},
    {"shiftwidth",	0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"col",
			ret_ସଂଖ୍ୟା,	    f_shiftwidth},
    {"sign_define",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"name, dict",
			ret_sign_define,	    SIGN_FUNC(f_sign_define)},
    {"sign_getdefined",	0, 1, FEARG_1,	    arg1_string,
			"name",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getdefined)},
    {"sign_getplaced",	0, 2, FEARG_1,	    arg02_sign_getplaced,
			"buf, dict",
			ret_list_dict_any,  SIGN_FUNC(f_sign_getplaced)},
    {"sign_jump",	3, 3, FEARG_1,	    arg3_ସଂଖ୍ୟା_string_buffer,
			"id, group, buf",
			ret_ସଂଖ୍ୟା,	    SIGN_FUNC(f_sign_jump)},
    {"sign_place",	4, 5, FEARG_1,	    arg45_sign_place,
			"id, group, name, buf, dict",
			ret_ସଂଖ୍ୟା,	    SIGN_FUNC(f_sign_place)},
    {"sign_placelist",	1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_ସଂଖ୍ୟା,    SIGN_FUNC(f_sign_placelist)},
    {"sign_undefine",	0, 1, FEARG_1,	    arg1_string_or_list_string,
			"name",
			ret_ସଂଖ୍ୟା_bool,    SIGN_FUNC(f_sign_undefine)},
    {"sign_unplace",	1, 2, FEARG_1,	    arg2_string_dict,
			"group, dict",
			ret_ସଂଖ୍ୟା_bool,    SIGN_FUNC(f_sign_unplace)},
    {"sign_unplacelist", 1, 1, FEARG_1,	    arg1_list_any,
			"list",
			ret_list_ସଂଖ୍ୟା,    SIGN_FUNC(f_sign_unplacelist)},
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
    {"sound_clear",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_void,	    SOUND_FUNC(f_sound_clear)},
    {"sound_playevent",	1, 2, FEARG_1,	    arg2_string_any,
			"name, callback",
			ret_ସଂଖ୍ୟା,	    SOUND_FUNC(f_sound_playevent)},
    {"sound_playfile",	1, 2, FEARG_1,	    arg2_string_any,
			"path, callback",
			ret_ସଂଖ୍ୟା,	    SOUND_FUNC(f_sound_playfile)},
    {"sound_stop",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"id",
			ret_void,	    SOUND_FUNC(f_sound_stop)},
    {"soundfold",	1, 1, FEARG_1,	    arg1_string,
			"word",
			ret_string,	    f_soundfold},
    {"spellbadword",	0, 1, FEARG_1,	    arg1_string,
			"sentence",
			ret_list_string,    f_spellbadword},
    {"spellsuggest",	1, 3, FEARG_1,	    arg3_string_ସଂଖ୍ୟା_bool,
			"word, max, capital",
			ret_list_string,    f_spellsuggest},
    {"split",		1, 3, FEARG_1,	    arg3_string_string_bool,
			"string, pattern, keepempty",
			ret_list_string,    f_split},
    {"sqrt",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_sqrt},
    {"srand",		0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"expr",
			ret_list_ସଂଖ୍ୟା,    f_srand},
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
			ret_list_ସଂଖ୍ୟା,    f_str2list},
    {"str2nr",		1, 3, FEARG_1,	    arg3_string_ସଂଖ୍ୟା_bool,
			"string, base, quoted",
			ret_ସଂଖ୍ୟା,	    f_str2nr},
    {"strcharlen",	1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_ସଂଖ୍ୟା,	    f_strcharlen},
    {"strcharpart",	2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, skipcc",
			ret_string,	    f_strcharpart},
    {"strchars",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, skipcc",
			ret_ସଂଖ୍ୟା,	    f_strchars},
    {"strdisplaywidth",	1, 2, FEARG_1,	    arg2_string_ସଂଖ୍ୟା,
			"string, col",
			ret_ସଂଖ୍ୟା,	    f_strdisplaywidth},
    {"strftime",	1, 2, FEARG_1,	    arg2_string_ସଂଖ୍ୟା,
			"format, time",
			ret_string,
#ifdef HAVE_STRFTIME
	    f_strftime
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
			},
    {"strgetchar",	2, 2, FEARG_1,	    arg2_string_ସଂଖ୍ୟା,
			"str, index",
			ret_ସଂଖ୍ୟା,	    f_strgetchar},
    {"stridx",		2, 3, FEARG_1,	    arg3_string_string_ସଂଖ୍ୟା,
			"haystack, needle, start",
			ret_ସଂଖ୍ୟା,	    f_stridx},
    {"string",		1, 1, FEARG_1|FE_X, ଖାଲି,
			"expr",
			ret_string,	    f_string},
    {"strlen",		1, 1, FEARG_1,	    arg1_string_or_nr,
			"string",
			ret_ସଂଖ୍ୟା,	    f_strlen},
    {"strpart",		2, 4, FEARG_1,	    arg24_strpart,
			"src, start, len, chars",
			ret_string,	    f_strpart},
    {"strptime",	2, 2, FEARG_1,	    arg2_string,
			"format, timestring",
			ret_ସଂଖ୍ୟା,
#ifdef HAVE_STRPTIME
	    f_strptime
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
			},
    {"strridx",		2, 3, FEARG_1,	    arg3_string_string_ସଂଖ୍ୟା,
			"haystack, needle, start",
			ret_ସଂଖ୍ୟା,	    f_strridx},
    {"strtrans",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_string,	    f_strtrans},
    {"strutf16len",	1, 2, FEARG_1,	    arg2_string_bool,
			"string, countcc",
			ret_ସଂଖ୍ୟା,	    f_strutf16len},
    {"strwidth",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_ସଂଖ୍ୟା,	    f_strwidth},
    {"submatch",	1, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା_bool,
			"nr, list",
			ret_string_or_any_2,	    f_submatch},
    {"substitute",	4, 4, FEARG_1,	    arg4_string_string_any_string,
			"string, pat, sub, flags",
			ret_string,	    f_substitute},
    {"swapfilelist",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_list_string,    f_swapfilelist},
    {"swapinfo",	1, 1, FEARG_1,	    arg1_string,
			"fname",
			ret_dict_any,	    f_swapinfo},
    {"swapname",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    f_swapname},
    {"synID",		3, 3, 0,	    arg3_lnum_ସଂଖ୍ୟା_bool,
			"lnum, col, trans",
			ret_ସଂଖ୍ୟା,	    f_synID},
    {"synIDattr",	2, 3, FEARG_1,	    arg3_ସଂଖ୍ୟା_string_string,
			"synID, what, mode",
			ret_string,	    f_synIDattr},
    {"synIDtrans",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"synID",
			ret_ସଂଖ୍ୟା,	    f_synIDtrans},
    {"synconcealed",	2, 2, 0,	    arg2_lnum_ସଂଖ୍ୟା,
			"lnum, col",
			ret_list_any,	    f_synconcealed},
    {"synstack",	2, 2, 0,	    arg2_lnum_ସଂଖ୍ୟା,
			"lnum, col",
			ret_list_ସଂଖ୍ୟା,    f_synstack},
    {"system",		1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_string,	    f_system},
    {"systemlist",	1, 2, FEARG_1,	    arg12_system,
			"expr, input",
			ret_list_string,    f_systemlist},
    {"tabpagebuflist",	0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"arg",
			ret_list_ସଂଖ୍ୟା,    f_tabpagebuflist},
    {"tabpagenr",	0, 1, 0,	    arg1_string,
			"arg",
			ret_ସଂଖ୍ୟା,	    f_tabpagenr},
    {"tabpagewinnr",	1, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା_string,
			"tabarg, arg",
			ret_ସଂଖ୍ୟା,	    f_tabpagewinnr},
    {"tabpanel_getinfo", 0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_dict_any,	    TABPANEL_FUNC(f_tabpanel_getinfo)},
    {"tabpanel_scroll",	1, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା_dict_any,
			"n, opts",
			ret_bool,	    TABPANEL_FUNC(f_tabpanel_scroll)},
    {"tagfiles",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
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
    {"tempname",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_tempname},
    {"term_dumpdiff",	2, 3, FEARG_1,	    arg3_string_string_dict,
			"filename1, filename2, options",
			ret_ସଂଖ୍ୟା,	    TERM_FUNC(f_term_dumpdiff)},
    {"term_dumpload",	1, 2, FEARG_1,	    arg2_string_dict,
			"filename, options",
			ret_ସଂଖ୍ୟା,	    TERM_FUNC(f_term_dumpload)},
    {"term_dumpwrite",	2, 3, FEARG_2,	    arg3_buffer_string_dict,
			"buf, filename, options",
			ret_void,	    TERM_FUNC(f_term_dumpwrite)},
    {"term_getaltscreen", 1, 1, FEARG_1,    arg1_buffer,
			"buf",
			ret_ସଂଖ୍ୟା,	    TERM_FUNC(f_term_getaltscreen)},
    {"term_getansicolors", 1, 1, FEARG_1,   arg1_buffer,
			"buf",
			ret_list_string,
#ଯଦି defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_getansicolors
#ଅନ୍ୟଥା
	    ଖାଲି
#endif
			},
    {"term_getattr",	2, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା_string,
			"attr, what",
			ret_ସଂଖ୍ୟା,	    TERM_FUNC(f_term_getattr)},
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
			ret_ସଂଖ୍ୟା,	    TERM_FUNC(f_term_getscrolled)},
    {"term_getsize",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_list_ସଂଖ୍ୟା,    TERM_FUNC(f_term_getsize)},
    {"term_getstatus",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_getstatus)},
    {"term_gettitle",	1, 1, FEARG_1,	    arg1_buffer,
			"buf",
			ret_string,	    TERM_FUNC(f_term_gettitle)},
    {"term_gettty",	1, 2, FEARG_1,	    arg2_buffer_bool,
			"buf, input",
			ret_string,	    TERM_FUNC(f_term_gettty)},
    {"term_list",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_list_ସଂଖ୍ୟା,    TERM_FUNC(f_term_list)},
    {"term_scrape",	2, 2, FEARG_1,	    arg2_buffer_lnum,
			"buf, row",
			ret_list_dict_any,  TERM_FUNC(f_term_scrape)},
    {"term_sendkeys",	2, 2, FEARG_1,	    arg2_buffer_string,
			"buf, keys",
			ret_void,	    TERM_FUNC(f_term_sendkeys)},
    {"term_setansicolors", 2, 2, FEARG_1,   arg2_buffer_list_any,
			"buf, colors",
			ret_void,
#ଯଦି defined(FEAT_TERMINAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	    f_term_setansicolors
#ଅନ୍ୟଥା
	    ଖାଲି
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
    {"term_setsize",	3, 3, FEARG_1,	    arg3_buffer_ସଂଖ୍ୟା_ସଂଖ୍ୟା,
			"buf, rows, cols",
			ret_void,	    TERM_FUNC(f_term_setsize)},
    {"term_start",	1, 2, FEARG_1,	    arg2_string_or_list_dict,
			"cmd, options",
			ret_ସଂଖ୍ୟା,	    TERM_FUNC(f_term_start)},
    {"term_wait",	1, 2, FEARG_1,	    arg2_buffer_ସଂଖ୍ୟା,
			"buf, time",
			ret_void,	    TERM_FUNC(f_term_wait)},
    {"terminalprops",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_dict_string,    f_terminalprops},
    {"test_alloc_fail",	3, 3, FEARG_1,	    arg3_ସଂଖ୍ୟା,
			"id, countdown, repeat",
			ret_void,	    f_test_alloc_fail},
    {"test_autochdir",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_void,	    f_test_autochdir},
    {"test_feedinput",	1, 1, FEARG_1,	    arg1_string,
			"string",
			ret_void,	    f_test_feedinput},
    {"test_garbagecollect_now",	0, 0, 0,    ଖାଲି,
			ଖାଲି,
			ret_void,	    f_test_garbagecollect_now},
    {"test_garbagecollect_soon", 0, 0, 0,   ଖାଲି,
			ଖାଲି,
			ret_void,	    f_test_garbagecollect_soon},
    {"test_getvalue",	1, 1, FEARG_1,	    arg1_string,
			"name",
			ret_ସଂଖ୍ୟା,	    f_test_getvalue},
    {"test_gui_event",	2, 2, FEARG_1,	    arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_gui_event},
    {"test_ignore_error", 1, 1, FEARG_1,    arg1_string,
			"expr",
			ret_void,	    f_test_ignore_error},
    {"test_mswin_event", 2, 2, FEARG_1,     arg2_string_dict,
			"event, args",
			ret_bool,	    f_test_mswin_event},
    {"test_null_blob",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_blob,	    f_test_null_blob},
    {"test_null_channel", 0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_channel,	    JOB_FUNC(f_test_null_channel)},
    {"test_null_dict",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_dict_any,	    f_test_null_dict},
    {"test_null_function", 0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_func_any,	    f_test_null_function},
    {"test_null_job",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_job,	    JOB_FUNC(f_test_null_job)},
    {"test_null_list",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_list_any,	    f_test_null_list},
    {"test_null_partial", 0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_func_any,	    f_test_null_partial},
    {"test_null_string", 0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_test_null_string},
    {"test_null_tuple",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_tuple_any,	    f_test_null_tuple},
    {"test_option_not_set", 1, 1, FEARG_1,  arg1_string,
			"name",
			ret_void,	    f_test_option_not_set},
    {"test_override",	2, 2, FEARG_2,	    arg2_string_ସଂଖ୍ୟା,
			"name, val",
			ret_void,	    f_test_override},
    {"test_refcount",	1, 1, FEARG_1|FE_X, ଖାଲି,
			"expr",
			ret_ସଂଖ୍ୟା,	    f_test_refcount},
    {"test_setmouse",	2, 2, 0,	    arg2_ସଂଖ୍ୟା,
			"row, col",
			ret_void,	    f_test_setmouse},
    {"test_settime",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"expr",
			ret_void,	    f_test_settime},
    {"test_srand_seed",	0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"seed",
			ret_void,	    f_test_srand_seed},
    {"test_unknown",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_any,	    f_test_unknown},
    {"test_void",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_void,	    f_test_void},
    {"timer_info",	0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"id",
			ret_list_dict_any,  TIMER_FUNC(f_timer_info)},
    {"timer_pause",	2, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା_bool,
			"timer, paused",
			ret_void,	    TIMER_FUNC(f_timer_pause)},
    {"timer_start",	2, 3, FEARG_1,	    arg3_ସଂଖ୍ୟା_any_dict,
			"time, callback, options",
			ret_ସଂଖ୍ୟା,	    TIMER_FUNC(f_timer_start)},
    {"timer_stop",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"timer",
			ret_void,	    TIMER_FUNC(f_timer_stop)},
    {"timer_stopall",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
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
    {"trim",		1, 3, FEARG_1,	    arg3_string_string_ସଂଖ୍ୟା,
			"text, mask, dir",
			ret_string,	    f_trim},
    {"trunc",		1, 1, FEARG_1,	    arg1_float_or_nr,
			"expr",
			ret_float,	    f_trunc},
    {"tuple2list",	1, 1, FEARG_1,	    arg1_tuple_any,
			"tuple",
			ret_list_any,	    f_tuple2list},
    {"type",		1, 1, FEARG_1|FE_X, ଖାଲି,
			"expr",
			ret_ସଂଖ୍ୟା,	    f_type},
    {"typename",	1, 1, FEARG_1|FE_X, ଖାଲି,
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
    {"utf16idx",	2, 4, FEARG_1,	    arg4_string_ସଂଖ୍ୟା_bool_bool,
			"string, idx, countcc, charidx",
			ret_ସଂଖ୍ୟା,	    f_utf16idx},
    {"values",		1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_list_member,    f_ମୂଲ୍ୟଗୁଡ଼ିକ},
    {"virtcol",		1, 3, FEARG_1,	    arg3_string_or_list_bool_ସଂଖ୍ୟା,
			"expr, list, winid",
			ret_virtcol,	    f_virtcol},
    {"virtcol2col",	3, 3, FEARG_1,	    arg3_ସଂଖ୍ୟା,
			"winid, lnum, col",
			ret_ସଂଖ୍ୟା,	    f_virtcol2col},
    {"visualmode",	0, 1, 0,	    arg1_bool,
			"expr",
			ret_string,	    f_visualmode},
    {"wildmenumode",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_wildmenumode},
    {"wildtrigger",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_void,	    f_wildtrigger},
    {"win_execute",	2, 3, FEARG_2,	    arg23_win_execute,
			"id, command, silent",
			ret_string,	    f_win_execute},
    {"win_findbuf",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"bufnr",
			ret_list_ସଂଖ୍ୟା,    f_win_findbuf},
    {"win_getid",	0, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା,
			"win, tab",
			ret_ସଂଖ୍ୟା,	    f_win_getid},
    {"win_gettype",	0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"nr",
			ret_string,	    f_win_gettype},
    {"win_gotoid",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"expr",
			ret_ସଂଖ୍ୟା_bool,    f_win_gotoid},
    {"win_id2tabwin",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"expr",
			ret_list_ସଂଖ୍ୟା,    f_win_id2tabwin},
    {"win_id2win",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"expr",
			ret_ସଂଖ୍ୟା,	    f_win_id2win},
    {"win_move_separator", 2, 2, FEARG_1,   arg2_ସଂଖ୍ୟା,
			"nr, offset",
			ret_ସଂଖ୍ୟା_bool,    f_win_move_separator},
    {"win_move_statusline", 2, 2, FEARG_1,  arg2_ସଂଖ୍ୟା,
			"nr, offset",
			ret_ସଂଖ୍ୟା_bool,    f_win_move_statusline},
    {"win_screenpos",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"nr",
			ret_list_ସଂଖ୍ୟା,    f_win_screenpos},
    {"win_splitmove",   2, 3, FEARG_1,	    arg3_ସଂଖ୍ୟା_ସଂଖ୍ୟା_dict,
			"nr, target, options",
			ret_ସଂଖ୍ୟା_bool,    f_win_splitmove},
    {"winbufnr",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"nr",
			ret_ସଂଖ୍ୟା,	    f_winbufnr},
    {"wincol",		0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_wincol},
    {"windowsversion",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_windowsversion},
    {"winheight",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"nr",
			ret_ସଂଖ୍ୟା,	    f_winheight},
    {"winlayout",	0, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"tabnr",
			ret_list_any,	    f_winlayout},
    {"winline",		0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_ସଂଖ୍ୟା,	    f_winline},
    {"winnr",		0, 1, FEARG_1,	    arg1_string,
			"arg",
			ret_ସଂଖ୍ୟା,	    f_winnr},
    {"winrestcmd",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_string,	    f_winrestcmd},
    {"winrestview",	1, 1, FEARG_1,	    arg1_dict_any,
			"dict",
			ret_void,	    f_winrestview},
    {"winsaveview",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_dict_ସଂଖ୍ୟା,    f_winsaveview},
    {"winwidth",	1, 1, FEARG_1,	    arg1_ସଂଖ୍ୟା,
			"nr",
			ret_ସଂଖ୍ୟା,	    f_winwidth},
    {"wordcount",	0, 0, 0,	    ଖାଲି,
			ଖାଲି,
			ret_dict_ସଂଖ୍ୟା,    f_wordcount},
    {"writefile",	2, 3, FEARG_1,	    arg23_writefile,
			"object, fname, flags",
			ret_ସଂଖ୍ୟା_bool,    f_writefile},
    {"xor",		2, 2, FEARG_1,	    arg2_ସଂଖ୍ୟା,
			"expr1, expr2",
			ret_ସଂଖ୍ୟା,	    f_xor},
};

/*
 * Return true if specified function allows a type as an argument.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
func_allows_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ (global_functions[idx].f_argtype & FE_X) != 0;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal
 * or user defined function names.
 */
    char_u *
get_function_name(expand_T *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	intidx = -1;
    char_u	*name;

    ଯଦି (idx == 0)
	intidx = -1;
    ଯଦି (intidx < 0)
    {
	name = get_user_func_name(xp, idx);
	ଯଦି (name != ଖାଲି)
	{
	    ଯଦି (*name != NUL && *name != '<'
				      && STRNCMP("g:", xp->xp_pattern, 2) == 0)
		ଫେରନ୍ତୁ cat_prefix_varname('g', name);
	    ଫେରନ୍ତୁ name;
	}
    }
    ଯଦି (++intidx < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(global_functions))
    {
	// Skip if the function doesn't have an implementation (feature not
	// implemented).
	ଯଦି (global_functions[intidx].f_func == ଖାଲି)
	    ଫେରନ୍ତୁ (char_u *)"";
	STRCPY(IObuff, global_functions[intidx].f_name);
	STRCAT(IObuff, "(");
	ଯଦି (global_functions[intidx].f_max_argc == 0)
	    STRCAT(IObuff, ")");
	ଫେରନ୍ତୁ IObuff;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Function given to ExpandGeneric() to obtain the list of internal or
 * user defined variable or function names.
 */
    char_u *
get_ପଦସମଷ୍ଟି_name(expand_T *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	intidx = -1;
    char_u	*name;

    ଯଦି (idx == 0)
	intidx = -1;
    ଯଦି (intidx < 0)
    {
	name = get_function_name(xp, idx);
	ଯଦି (name != ଖାଲି)
	    ଫେରନ୍ତୁ name;
    }
    ଫେରନ୍ତୁ get_user_var_name(xp, ++intidx);
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or "implemented" is TRUE and the function
 * is not implemented.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_internal_func_opt(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା implemented)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x;

    last = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(global_functions) - 1;

    // Find the function name in the table. Binary search.
    ଯେତେବେଳେ (first <= last)
    {
	x = first + ((unsigned)(last - first) >> 1);
	cmp = STRCMP(name, global_functions[x].f_name);
	ଯଦି (cmp < 0)
	    last = x - 1;
	ଅନ୍ୟଥା ଯଦି (cmp > 0)
	    first = x + 1;
	ଅନ୍ୟଥା ଯଦି (implemented && global_functions[x].f_func == ଖାଲି)
	    ଭାଙ୍ଗ;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ x;
    }
    ଫେରନ୍ତୁ -1;
}

/*
 * Find internal function "name" in table "global_functions".
 * Return index, or -1 if not found or the function is not implemented.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_internal_func(char_u *name)
{
    ଫେରନ୍ତୁ find_internal_func_opt(name, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_internal_func(char_u *name)
{
    ଫେରନ୍ତୁ find_internal_func_opt(name, TRUE) >= 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_internal_func_name(char_u *name)
{
    ଫେରନ୍ତୁ find_internal_func_opt(name, FALSE) >= 0;
}

    ଅକ୍ଷର *
internal_func_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ global_functions[idx].f_name;
}

/*
 * Check the argument types for builtin function "idx".
 * Uses the list of types on the type stack: "types".
 * Return FAIL and gives an error message when a type is wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
internal_func_check_arg_types(
	type2_T	*types,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	argcount,
	cctx_T	*cctx)
{
    // Some internal functions accept types like Class as arguments. For other
    // functions, check the arguments are not types.
    ଯଦି (!(func_allows_type(idx)))
    {
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < argcount; ++i)
	    ଯଦି (check_type_is_ମୂଲ୍ୟ(types[i].type_curr) == FAIL)
		ଫେରନ୍ତୁ FAIL;
    }

    ଅଚଳ argcheck_T	*argchecks = global_functions[idx].f_argcheck;

    ଯଦି (argchecks == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    argcontext_T context;

    context.arg_ଗଣନା = argcount;
    context.arg_types = types;
    context.arg_cctx = cctx;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < argcount && argchecks[i] != ଖାଲି; ++i)
    {
	context.arg_idx = i;
	ଯଦି (argchecks[i](types[i].type_curr, types[i].type_decl,
							    &context) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Get the argument count for function "idx".
 * "argcount" is the total argument count, "min_argcount" the non-optional
 * argument count.
 */
    ଶୂନ୍ୟ
internal_func_get_argcount(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *argcount, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *min_argcount)
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount,
	type2_T	    *argtypes,
	type_T	    **decl_type,
	garray_T    *type_gap)
{
    type_T *ret;

    current_type_gap = type_gap;
    *decl_type = ଖାଲି;
    ret = global_functions[idx].f_retfunc(argcount, argtypes, decl_type);
    ଯଦି (*decl_type == ଖାଲି)
	*decl_type = ret;
    current_type_gap = ଖାଲି;
    ଫେରନ୍ତୁ ret;
}

/*
 * Return TRUE if "idx" is for the map() function.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
internal_func_is_map(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ global_functions[idx].f_func == f_map;
}

/*
 * Check the argument count to use for internal function "idx".
 * Returns -1 for failure, 0 if no method base accepted, 1 if method base is
 * first argument, 2 if method base is second argument, etc.  9 if method base
 * is last argument.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_internal_func(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount)
{
    funcerror_T	    res;
    ଅକ୍ଷର	    *name;

    ଯଦି (argcount < global_functions[idx].f_min_argc)
	res = FCERR_TOOFEW;
    ଅନ୍ୟଥା ଯଦି (argcount > global_functions[idx].f_max_argc)
	res = FCERR_TOOMANY;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ global_functions[idx].f_argtype & FEARG_MASK;

    name = internal_func_name(idx);
    ଯଦି (res == FCERR_TOOMANY)
	semsg(_(e_too_many_arguments_for_function_str), name);
    ଅନ୍ୟଥା
	semsg(_(e_not_enough_arguments_for_function_str), name);
    ଫେରନ୍ତୁ -1;
}

/*
 * Some internal functions accept types like Class as arguments. For other
 * functions, check the arguments are not types.
 *
 * Return OK/FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_args_for_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, typval_T *argvars)
{
    ଯଦି (!func_allows_type(idx))
    {
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < argcount; ++i)
	    ଯଦି (check_typval_is_ମୂଲ୍ୟ(&argvars[i]) == FAIL)
		ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

    funcerror_T
call_internal_func(
	char_u	    *name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount,
	typval_T    *argvars,
	typval_T    *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    i = find_internal_func(name);
    ଯଦି (i < 0)
	ଫେରନ୍ତୁ FCERR_UNKNOWN;
    ଯଦି (argcount < global_functions[i].f_min_argc)
	ଫେରନ୍ତୁ FCERR_TOOFEW;
    ଯଦି (argcount > global_functions[i].f_max_argc)
	ଫେରନ୍ତୁ FCERR_TOOMANY;
    ଯଦି (check_args_for_type(i, argcount, argvars) == FAIL)
	ଫେରନ୍ତୁ FCERR_OTHER;
    argvars[argcount].v_type = VAR_UNKNOWN;
    global_functions[i].f_func(argvars, rettv);
    ଯଦି (in_vim9script() && global_functions[i].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
    ଫେରନ୍ତୁ FCERR_NONE;
}

    ଶୂନ୍ୟ
call_internal_func_by_idx(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx,
	typval_T    *argvars,
	typval_T    *rettv)
{
    global_functions[idx].f_func(argvars, rettv);
    ଯଦି (in_vim9script() && global_functions[idx].f_retfunc == ret_void)
	rettv->v_type = VAR_VOID;
}

/*
 * Invoke a method for base->method().
 */
    funcerror_T
call_internal_method(
	char_u	    *name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	typval_T    *basetv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fi;
    typval_T	argv[MAX_FUNC_ARGS + 1];

    fi = find_internal_func(name);
    ଯଦି (fi < 0)
	ଫେରନ୍ତୁ FCERR_UNKNOWN;
    ଯଦି ((global_functions[fi].f_argtype & FEARG_MASK) == 0)
	ଫେରନ୍ତୁ FCERR_NOTMETHOD;
    ଯଦି (argcount + 1 < global_functions[fi].f_min_argc)
	ଫେରନ୍ତୁ FCERR_TOOFEW;
    ଯଦି (argcount + 1 > global_functions[fi].f_max_argc)
	ଫେରନ୍ତୁ FCERR_TOOMANY;
    ଯଦି (check_args_for_type(fi, argcount, argvars) == FAIL)
	ଫେରନ୍ତୁ FCERR_OTHER;

    ଯଦି ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_2)
    {
	ଯଦି (argcount < 1)
	    ଫେରନ୍ତୁ FCERR_TOOFEW;

	// base value goes second
	argv[0] = argvars[0];
	argv[1] = *basetv;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ଅନ୍ୟଥା ଯଦି ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_3)
    {
	ଯଦି (argcount < 2)
	    ଫେରନ୍ତୁ FCERR_TOOFEW;

	// base value goes third
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = *basetv;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 2; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ଅନ୍ୟଥା ଯଦି ((global_functions[fi].f_argtype & FEARG_MASK) == FEARG_4)
    {
	ଯଦି (argcount < 3)
	    ଫେରନ୍ତୁ FCERR_TOOFEW;

	// base value goes fourth
	argv[0] = argvars[0];
	argv[1] = argvars[1];
	argv[2] = argvars[2];
	argv[3] = *basetv;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 3; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    ଅନ୍ୟଥା
    {
	// FEARG_1: base value goes first
	argv[0] = *basetv;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < argcount; ++i)
	    argv[i + 1] = argvars[i];
    }
    argv[argcount + 1].v_type = VAR_UNKNOWN;

    ଯଦି (check_args_for_type(fi, argcount + 1, argv) == FAIL)
	ଫେରନ୍ତୁ FCERR_OTHER;

    global_functions[fi].f_func(argv, rettv);
    ଫେରନ୍ତୁ FCERR_NONE;
}

/*
 * Return TRUE for a non-zero Number and a non-empty String.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
non_zero_arg(typval_T *argvars)
{
    ଫେରନ୍ତୁ ((argvars[0].v_type == VAR_ସଂଖ୍ୟା
		&& argvars[0].vval.v_ସଂଖ୍ୟା != 0)
	    || (argvars[0].v_type == VAR_BOOL
		&& argvars[0].vval.v_ସଂଖ୍ୟା == VVAL_TRUE)
	    || (argvars[0].v_type == VAR_STRING
		&& argvars[0].vval.v_string != ଖାଲି
		&& *argvars[0].vval.v_string != NUL));
}

/*
 * "and(expr, expr)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_and(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି)
					& tv_get_ସଂଖ୍ୟା_chk(&argvars[1], ଖାଲି);
}

/*
 * "balloon_show()" function
 */
#ifdef FEAT_BEVAL
    ସ୍ଥିର ଶୂନ୍ୟ
f_balloon_gettext(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    ଯଦି (balloonEval == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (balloonEval->ସନ୍ଦେଶ == ଖାଲି)
	rettv->vval.v_string = ଖାଲି;
    ଅନ୍ୟଥା
	rettv->vval.v_string = vim_strsave(balloonEval->ସନ୍ଦେଶ);
}

    ସ୍ଥିର ଶୂନ୍ୟ
f_balloon_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (balloonEval == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && check_for_string_or_list_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_LIST
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
       )
    {
	list_T *l = argvars[0].vval.v_list;

	// empty list removes the balloon
	post_balloon(balloonEval, ଖାଲି,
		l == ଖାଲି || l->lv_len == 0 ? ଖାଲି : l);
    }
    ଅନ୍ୟଥା
    {
	char_u *mesg;

	ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	    ଫେରନ୍ତୁ;

	mesg = tv_get_string_chk(&argvars[0]);
	ଯଦି (mesg != ଖାଲି)
	    // empty string removes the balloon
	    post_balloon(balloonEval, *mesg == NUL ? ଖାଲି : mesg, ଖାଲି);
    }
}

# ଯଦି defined(FEAT_BEVAL_TERM)
    ସ୍ଥିର ଶୂନ୍ୟ
f_balloon_split(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (rettv_list_alloc(rettv) != OK)
	ଫେରନ୍ତୁ;

    char_u *ସନ୍ଦେଶ;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;
    ସନ୍ଦେଶ = tv_get_string_chk(&argvars[0]);
    ଯଦି (ସନ୍ଦେଶ != ଖାଲି)
    {
	pumitem_T	*array;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		size = split_message(ସନ୍ଦେଶ, &array);

	// Skip the first and last item, they are always empty.
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < size - 1; ++i)
	    list_append_string(rettv->vval.v_list, array[i].pum_text, -1);
	ଯେତେବେଳେ (size > 0)
	    vim_free(array[--size].pum_text);
	vim_free(array);
    }
}
# endif
#endif

/*
 * "base64_decode(string)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_base64_decode(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (rettv_blob_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    ଯଦି (str != ଖାଲି)
	base64_decode(str, STRLEN(str), &rettv->vval.v_blob->bv_ga);
}

/*
 * "base64_encode(blob)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_base64_encode(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (check_for_blob_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    blob_T *blob = argvars->vval.v_blob;
    ଯଦି (blob != ଖାଲି)
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
    ଯଦି (buf == ଖାଲି)
	semsg(_(e_invalid_buffer_name_str), tv_get_string(arg));
    ଫେରନ୍ତୁ buf;
}

/*
 * "bindtextdomain(package, path)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_bindtextdomain(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ସଂଖ୍ୟା = VVAL_TRUE;

    ଯଦି (check_for_nonempty_string_arg(argvars, 0) == FAIL
	    || check_for_nonempty_string_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (strcmp((ଅଚଳ ଅକ୍ଷର *)argvars[0].vval.v_string, VIMPACKAGE) == 0)
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    ଅନ୍ୟଥା
    {
	ଯଦି (bindtextdomain((ଅଚଳ ଅକ୍ଷର *)argvars[0].vval.v_string, (ଅଚଳ ଅକ୍ଷର *)argvars[1].vval.v_string) == ଖାଲି)
	{
	    do_outofmem_ସନ୍ଦେଶ((long)0);
	    rettv->vval.v_ସଂଖ୍ୟା = VVAL_FALSE;
	}
    }

    ଫେରନ୍ତୁ;
}

/*
 * "byte2line(byte)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_byte2line(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_ସଂଖ୍ୟା = -1;
#ଅନ୍ୟଥା
    long	boff = 0;

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    boff = tv_get_ସଂଖ୍ୟା(&argvars[0]) - 1;  // boff gets -1 on type error
    ଯଦି (boff < 0)
	rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = ml_find_ଧାଡ଼ି_or_offset(curbuf,
							  (linenr_T)0, &boff);
#endif
}

/*
 * "call(func, arglist [, dict])" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_call(typval_T *argvars, typval_T *rettv)
{
    char_u	*func;
    partial_T   *partial = ଖାଲି;
    dict_T	*selfdict = ଖାଲି;
    char_u	*tofree = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_list_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (argvars[1].vval.v_list == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_FUNC)
	func = argvars[0].vval.v_string;
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_PARTIAL)
    {
	partial = argvars[0].vval.v_partial;
	func = partial_name(partial);
    }
    ଅନ୍ୟଥା
	func = tv_get_string(&argvars[0]);
    ଯଦି (func == ଖାଲି || *func == NUL)
	ଫେରନ୍ତୁ;		// type error, empty name or null function

    ଯଦି (argvars[0].v_type == VAR_STRING)
    {
	char_u	*p = func;
	tofree = trans_function_name(&p, ଖାଲି, FALSE, TFN_INT|TFN_QUIET);
	ଯଦି (tofree == ଖାଲି)
	{
	    emsg_funcname(e_unknown_function_str, func);
	    ଫେରନ୍ତୁ;
	}
	ଯଦି (*p == '<')
	{
	    // generic function
	    char_u *s = append_generic_func_type_args(tofree, STRLEN(tofree),
									&p);
	    ଯଦି (s != ଖାଲି)
	    {
		vim_free(tofree);
		tofree = s;
	    }
	}
	func = tofree;
    }

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	ଯଦି (check_for_dict_arg(argvars, 2) == FAIL)
	    ଯାଅ done;

	selfdict = argvars[2].vval.v_dict;
    }

    (ଶୂନ୍ୟ)func_call(func, &argvars[1], partial, selfdict, rettv);

done:
    vim_free(tofree);
}

/*
 * "changenr()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_changenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = curbuf->b_u_seq_cur;
}

/*
 * "char2nr(string)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_char2nr(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (has_mbyte)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	utf8 = 0;

	ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[1], ଖାଲି);

	ଯଦି (utf8)
	    rettv->vval.v_ସଂଖ୍ୟା = utf_ptr2char(tv_get_string(&argvars[0]));
	ଅନ୍ୟଥା
	    rettv->vval.v_ସଂଖ୍ୟା = (*mb_ptr2char)(tv_get_string(&argvars[0]));
    }
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = tv_get_string(&argvars[0])[0];
}

/*
 * Get the current cursor column and store it in 'rettv'. If 'charcol' is TRUE,
 * returns the character index of the column. Otherwise, returns the byte index
 * of the column.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_col(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା charcol)
{
    colnr_T	col = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		winchanged = FALSE;

    ଯଦି (check_for_string_or_list_arg(argvars, 0) == FAIL
	    || check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the second argument
	wp = win_id2wp_tp((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[1]), &tp);
	ଯଦି (wp == ଖାଲି || tp == ଖାଲି)
	    ଫେରନ୍ତୁ;

	ଯଦି (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    ଫେରନ୍ତୁ;

	check_cursor();
	winchanged = TRUE;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, charcol);
    ଯଦି (fp != ଖାଲି && fnum == curbuf->b_fnum)
    {
	ଯଦି (fp->col == MAXCOL)
	{
	    // '> can be MAXCOL, get the length of the line then
	    ଯଦି (fp->lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		col = ml_get_len(fp->lnum) + 1;
	    ଅନ୍ୟଥା
		col = MAXCOL;
	}
	ଅନ୍ୟଥା
	{
	    col = fp->col + 1;
	    // col(".") when the cursor is on the NUL at the end of the line
	    // because of "coladd" can be seen as an extra column.
	    ଯଦି (virtual_active() && fp == &curwin->w_cursor)
	    {
		char_u	*p = ml_get_cursor();

		ଯଦି (curwin->w_cursor.coladd >= (colnr_T)chartabsize(p,
				 curwin->w_virtcol - curwin->w_cursor.coladd))
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

		    ଯଦି (*p != NUL && p[(l = (*mb_ptr2len)(p))] == NUL)
			col += l;
		}
	    }
	}
    }
    rettv->vval.v_ସଂଖ୍ୟା = col;

    ଯଦି (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "charcol()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_charcol(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, TRUE);
}

    win_T *
get_optional_window(typval_T *argvars, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    win_T   *win = curwin;

    ଯଦି (argvars[idx].v_type == VAR_UNKNOWN)
	ଫେରନ୍ତୁ curwin;

    win = find_win_by_nr_or_id(&argvars[idx]);
    ଯଦି (win == ଖାଲି)
    {
	emsg(_(e_invalid_window_ସଂଖ୍ୟା));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ win;
}

/*
 * "col(string)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_col(typval_T *argvars, typval_T *rettv)
{
    get_col(argvars, rettv, FALSE);
}

/*
 * "confirm(message, buttons[, default [, type]])" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_confirm(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    char_u	*message;
    char_u	*buttons = ଖାଲି;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		def = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type = VIM_GENERIC;
    char_u	*typestr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& (check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL
			    || (argvars[2].v_type != VAR_UNKNOWN
				&& check_for_opt_string_arg(argvars, 3) == FAIL))))))
	ଫେରନ୍ତୁ;

    message = tv_get_string_chk(&argvars[0]);
    ଯଦି (message == ଖାଲି)
	error = TRUE;
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	buttons = tv_get_string_buf_chk(&argvars[1], buf);
	ଯଦି (buttons == ଖାଲି)
	    error = TRUE;
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	{
	    def = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
	    ଯଦି (argvars[3].v_type != VAR_UNKNOWN)
	    {
		typestr = tv_get_string_buf_chk(&argvars[3], buf2);
		ଯଦି (typestr == ଖାଲି)
		    error = TRUE;
		ଅନ୍ୟଥା
		{
		    ବାଛ (TOUPPER_ASC(*typestr))
		    {
			ସ୍ଥିତି 'E': type = VIM_ERROR; ଭାଙ୍ଗ;
			ସ୍ଥିତି 'Q': type = VIM_QUESTION; ଭାଙ୍ଗ;
			ସ୍ଥିତି 'I': type = VIM_INFO; ଭାଙ୍ଗ;
			ସ୍ଥିତି 'W': type = VIM_WARNING; ଭାଙ୍ଗ;
			ସ୍ଥିତି 'G': type = VIM_GENERIC; ଭାଙ୍ଗ;
		    }
		}
	    }
	}
    }

    ଯଦି (buttons == ଖାଲି || *buttons == NUL)
	buttons = (char_u *)_("&Ok");

    ଯଦି (!error)
	rettv->vval.v_ସଂଖ୍ୟା = do_dialog(type, ଖାଲି, message, buttons,
							    def, ଖାଲି, FALSE);
#endif
}

/*
 * "copy()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_copy(typval_T *argvars, typval_T *rettv)
{
    item_copy(&argvars[0], rettv, FALSE, TRUE, 0);
}

/*
 * Set the cursor position.
 * If "charcol" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_cursorpos(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା charcol)
{
    long	lnum, col;
    long	coladd = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		set_curswant = TRUE;

    ଯଦି (in_vim9script()
	    && (check_for_string_or_ସଂଖ୍ୟା_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଯଦି (argvars[0].v_type == VAR_LIST)
    {
	pos_T	    pos;
	colnr_T	    curswant = -1;

	ଯଦି (list2fpos(argvars, &pos, ଖାଲି, &curswant, charcol) == FAIL)
	{
	    emsg(_(e_invalid_argument));
	    ଫେରନ୍ତୁ;
	}
	lnum = pos.lnum;
	col = pos.col;
	coladd = pos.coladd;
	ଯଦି (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    set_curswant = FALSE;
	}
    }
    ଅନ୍ୟଥା ଯଦି ((argvars[0].v_type == VAR_ସଂଖ୍ୟା
					    || argvars[0].v_type == VAR_STRING)
	    && (argvars[1].v_type == VAR_ସଂଖ୍ୟା
					   || argvars[1].v_type == VAR_STRING))
    {
	lnum = tv_get_lnum(argvars);
	ଯଦି (lnum < 0)
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
	ଅନ୍ୟଥା ଯଦି (lnum == 0)
	    lnum = curwin->w_cursor.lnum;
	col = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], ଖାଲି);
	ଯଦି (charcol)
	    col = buf_charidx_to_byteidx(curbuf, lnum, col) + 1;
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	    coladd = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], ଖାଲି);
    }
    ଅନ୍ୟଥା
    {
	emsg(_(e_invalid_argument));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (lnum < 0 || col < 0 || coladd < 0)
	ଫେରନ୍ତୁ;		// type error; errmsg already given
    ଯଦି (lnum > 0)
	curwin->w_cursor.lnum = lnum;
    ଯଦି (col != MAXCOL && --col < 0)
	col = 0;
    curwin->w_cursor.col = col;
    curwin->w_cursor.coladd = coladd;

    // Make sure the cursor is in a valid position.
    check_cursor();
    // Correct cursor for multi-byte character.
    ଯଦି (has_mbyte)
	mb_adjust_cursor();

    curwin->w_set_curswant = set_curswant;
    rettv->vval.v_ସଂଖ୍ୟା = 0;
}

/*
 * "cursor(lnum, col)" function, or
 * "cursor(list)"
 *
 * Moves the cursor to the specified line and column.
 * Returns 0 when the position could be set, -1 otherwise.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_cursor(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, FALSE);
}

#ifdef MSWIN
/*
 * "debugbreak()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_debugbreak(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pid;

    rettv->vval.v_ସଂଖ୍ୟା = FAIL;
    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    pid = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]);
    ଯଦି (pid == 0)
    {
	emsg(_(e_invalid_argument));
	ଫେରନ୍ତୁ;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    ଯଦି (hProcess == ଖାଲି)
	ଫେରନ୍ତୁ;

    DebugBreakProcess(hProcess);
    CloseHandle(hProcess);
    rettv->vval.v_ସଂଖ୍ୟା = OK;
}
#endif

/*
 * "deepcopy()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_deepcopy(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	noref = 0;

    ଯଦି (check_for_opt_bool_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	noref = tv_get_bool_chk(&argvars[1], ଖାଲି);

    item_copy(&argvars[0], rettv, TRUE, TRUE, noref == 0 ? get_copyID() : 0);
}

/*
 * "did_filetype()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_did_filetype(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->vval.v_ସଂଖ୍ୟା = curbuf->b_did_filetype;
}

/*
 * "echoraw({expr})" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_echoraw(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *str;

    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    str = tv_get_string_chk(&argvars[0]);
    ଯଦି (str != ଖାଲି && *str != NUL)
    {
	out_str(str);
	out_flush();
    }
}

/*
 * "empty({expr})" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_empty(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = FALSE;

    ବାଛ (argvars[0].v_type)
    {
	ସ୍ଥିତି VAR_STRING:
	ସ୍ଥିତି VAR_FUNC:
	    n = argvars[0].vval.v_string == ଖାଲି
					  || *argvars[0].vval.v_string == NUL;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_PARTIAL:
	    n = argvars[0].vval.v_partial == ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	    n = argvars[0].vval.v_ସଂଖ୍ୟା == 0;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FLOAT:
	    n = argvars[0].vval.v_float == 0.0;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    n = argvars[0].vval.v_list == ଖାଲି
					|| argvars[0].vval.v_list->lv_len == 0;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    n = tuple_len(argvars[0].vval.v_tuple) == 0;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    n = argvars[0].vval.v_dict == ଖାଲି
			|| argvars[0].vval.v_dict->dv_hashtab.ht_used == 0;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	    n = argvars[0].vval.v_ସଂଖ୍ୟା != VVAL_TRUE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CLASS:
	    n = argvars[0].vval.v_class != ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_OBJECT:
	    n = object_empty(argvars[0].vval.v_object);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_BLOB:
	    n = argvars[0].vval.v_blob == ଖାଲି
		|| argvars[0].vval.v_blob->bv_ga.ga_len == 0;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_job == ଖାଲି
			   || argvars[0].vval.v_job->jv_status != JOB_STARTED;
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    n = argvars[0].vval.v_channel == ଖାଲି
			       || !channel_is_open(argvars[0].vval.v_channel);
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_TYPEALIAS:
	    n = argvars[0].vval.v_typealias == ଖାଲି
		|| argvars[0].vval.v_typealias->ta_name == ଖାଲି
		|| *argvars[0].vval.v_typealias->ta_name == NUL;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	ସ୍ଥିତି VAR_INSTR:
	    internal_error_no_abort("f_empty(UNKNOWN)");
	    n = TRUE;
	    ଭାଙ୍ଗ;
    }

    rettv->vval.v_ସଂଖ୍ୟା = n;
}

/*
 * "environ()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_environ(typval_T *argvars UNUSED, typval_T *rettv)
{
#ଯଦି !defined(AMIGA)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i = 0;
    char_u		*entry, *ମୂଲ୍ୟ;
# ଯଦି defined (MSWIN)
#  ଯଦି !defined(_UCRT)
    ବାହ୍ୟ wchar_t	**_wenviron;
#  endif
# ଅନ୍ୟଥା
    ବାହ୍ୟ ଅକ୍ଷର		**environ;
# endif

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

# ifdef MSWIN
    ଯଦି (*_wenviron == ଖାଲି)
	ଫେରନ୍ତୁ;
# ଅନ୍ୟଥା
    ଯଦି (*environ == ଖାଲି)
	ଫେରନ୍ତୁ;
# endif

    ପାଇଁ (i = 0; ; ++i)
    {
# ifdef MSWIN
	short_u		*p;

	ଯଦି ((p = (short_u *)_wenviron[i]) == ଖାଲି)
	    ଫେରନ୍ତୁ;
	entry = utf16_to_enc(p, ଖାଲି);
# ଅନ୍ୟଥା
	ଯଦି ((entry = (char_u *)environ[i]) == ଖାଲି)
	    ଫେରନ୍ତୁ;
	entry = vim_strsave(entry);
# endif
	ଯଦି (entry == ଖାଲି) // out of memory
	    ଫେରନ୍ତୁ;
	ଯଦି ((ମୂଲ୍ୟ = vim_strchr(entry, '=')) == ଖାଲି)
	{
	    vim_free(entry);
	    ଜାରି;
	}
	*ମୂଲ୍ୟ++ = NUL;
	dict_add_string(rettv->vval.v_dict, (ଅକ୍ଷର *)entry, ମୂଲ୍ୟ);
	vim_free(entry);
    }
#endif
}

/*
 * "err_teapot()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_err_teapot(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	ଯଦି (argvars[0].v_type == VAR_STRING)
	{
	    char_u *s = tv_get_string_strict(&argvars[0]);
	    ଯଦି (*skipwhite(s) == NUL)
		ଫେରନ୍ତୁ;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_503 = eval_ପଦସମଷ୍ଟି_to_bool(&argvars[0], &err);
	ଯଦି (!err && do_503)
	{
	    emsg(_(e_coffee_currently_not_available));
	    ଫେରନ୍ତୁ;
	}
    }

    emsg(_(e_im_a_teapot));
}

/*
 * "escape({string}, {chars})" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_escape(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    rettv->vval.v_string = vim_strsave_escaped(tv_get_string(&argvars[0]),
					 tv_get_string_buf(&argvars[1], buf));
    rettv->v_type = VAR_STRING;
}

/*
 * "eval()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*s, *p;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    s = tv_get_string_chk(&argvars[0]);
    ଯଦି (s != ଖାଲି)
	s = skipwhite(s);

    p = s;
    ଯଦି (s == ଖାଲି || eval1(&s, rettv, &EVALARG_EVALUATE) == FAIL)
    {
	ଯଦି (p != ଖାଲି && !aborting())
	    semsg(_(e_invalid_expression_str), p);
	need_clr_eos = FALSE;
	rettv->v_type = VAR_ସଂଖ୍ୟା;
	rettv->vval.v_ସଂଖ୍ୟା = 0;
    }
    ଅନ୍ୟଥା ଯଦି (*s != NUL)
	semsg(_(e_trailing_characters_str), s);
}

/*
 * "eventhandler()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_eventhandler(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = vgetc_busy || ନିବେଶ_busy;
}

ସ୍ଥିର garray_T	redir_execute_ga;

/*
 * Append "value[value_len]" to the execute() output.
 */
    ଶୂନ୍ୟ
execute_redir_str(char_u *ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ_len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି (ମୂଲ୍ୟ_len == -1)
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ମୂଲ୍ୟ);	// Append the entire string
    ଅନ୍ୟଥା
	len = ମୂଲ୍ୟ_len;		// Append only "value_len" characters
    ଯଦି (ga_grow(&redir_execute_ga, len) == FAIL)
	ଫେରନ୍ତୁ;

    mch_memmove((ଅକ୍ଷର *)redir_execute_ga.ga_data
	    + redir_execute_ga.ga_len, ମୂଲ୍ୟ, len);
    redir_execute_ga.ga_len += len;
}

#ଯଦି defined(FEAT_LUA)
/*
 * Get next line from a string containing NL separated lines.
 * Called by do_cmdline() to get the next line.
 * Returns an allocated string, or NULL when at the end of the string.
 */
    ସ୍ଥିର char_u *
get_str_ଧାଡ଼ି(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c UNUSED,
    ଶୂନ୍ୟ    *cookie,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    char_u	*start = *(char_u **)cookie;
    char_u	*ଧାଡ଼ି;
    char_u	*p;

    p = start;
    ଯଦି (p == ଖାଲି || *p == NUL)
	ଫେରନ୍ତୁ ଖାଲି;
    p = vim_strchr(p, '\n');
    ଯଦି (p == ଖାଲି)
	ଧାଡ଼ି = vim_strsave(start);
    ଅନ୍ୟଥା
    {
	ଧାଡ଼ି = vim_strnsave(start, p - start);
	p++;
    }

    *(char_u **)cookie = p;
    ଫେରନ୍ତୁ ଧାଡ଼ି;
}

/*
 * Execute a series of Ex commands in 'str'
 */
    ଶୂନ୍ୟ
execute_cmds_from_string(char_u *str)
{
    do_cmdline(ଖାଲି, get_str_ଧାଡ଼ି, (ଶୂନ୍ୟ *)&str,
	    DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
}
#endif

/*
 * Get next line from a list.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_list_ଧାଡ଼ି(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c UNUSED,
    ଶୂନ୍ୟ    *cookie,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    listitem_T **p = (listitem_T **)cookie;
    listitem_T *item = *p;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    ଯଦି (item == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    s = tv_get_string_buf_chk(&item->li_tv, buf);
    *p = item->li_next;
    ଫେରନ୍ତୁ s == ଖାଲି ? ଖାଲି : vim_strsave(s);
}

/*
 * "execute()" function
 */
    ଶୂନ୍ୟ
execute_common(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_off)
{
    char_u	*cmd = ଖାଲି;
    list_T	*list = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ସନ୍ଦେଶ_silent = ସନ୍ଦେଶ_silent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_emsg_silent = emsg_silent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_emsg_noredir = emsg_noredir;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_redir_execute = redir_execute;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_redir_off = redir_off;
    garray_T	save_ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ସନ୍ଦେଶ_col = ସନ୍ଦେଶ_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		echo_output = FALSE;

    rettv->vval.v_string = ଖାଲି;
    rettv->v_type = VAR_STRING;

    ଯଦି (argvars[arg_off].v_type == VAR_LIST)
    {
	list = argvars[arg_off].vval.v_list;
	ଯଦି (list == ଖାଲି || list->lv_len == 0)
	    // empty list, no commands, empty output
	    ଫେରନ୍ତୁ;
	++list->lv_refcount;
    }
    ଅନ୍ୟଥା ଯଦି (argvars[arg_off].v_type == VAR_JOB
	    || argvars[arg_off].v_type == VAR_CHANNEL)
    {
	semsg(_(e_using_invalid_ମୂଲ୍ୟ_as_string_str),
				       vartype_name(argvars[arg_off].v_type));
	ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା
    {
	cmd = tv_get_string_chk(&argvars[arg_off]);
	ଯଦି (cmd == ଖାଲି)
	    ଫେରନ୍ତୁ;
    }

    ଯଦି (argvars[arg_off + 1].v_type != VAR_UNKNOWN)
    {
	char_u	buf[NUMBUFLEN];
	char_u  *s = tv_get_string_buf_chk_strict(&argvars[arg_off + 1], buf,
							      in_vim9script());

	ଯଦି (s == ଖାଲି)
	    ଫେରନ୍ତୁ;
	ଯଦି (*s == NUL)
	    echo_output = TRUE;
	ଯଦି (STRNCMP(s, "silent", 6) == 0)
	    ++ସନ୍ଦେଶ_silent;
	ଯଦି (STRCMP(s, "silent!") == 0)
	{
	    emsg_silent = TRUE;
	    emsg_noredir = TRUE;
	}
    }
    ଅନ୍ୟଥା
	++ସନ୍ଦେଶ_silent;

    ଯଦି (redir_execute)
	save_ga = redir_execute_ga;
    ga_init2(&redir_execute_ga, ଆକାର(ଅକ୍ଷର), 500);
    redir_execute = TRUE;
    redir_off = FALSE;
    ଯଦି (!echo_output)
	ସନ୍ଦେଶ_col = 0;  // prevent leading spaces

    // For "legacy call execute('cmd')" and "vim9cmd execute('cmd')" apply the
    // command modifiers to "cmd".
    sticky_cmdmod_flags = cmdmod.cmod_flags & (CMOD_LEGACY | CMOD_VIM9CMD);
    ଯଦି (cmd != ଖାଲି)
	do_cmdline_cmd(cmd);
    ଅନ୍ୟଥା
    {
	listitem_T	*item;

	CHECK_LIST_MATERIALIZE(list);
	item = list->lv_first;
	do_cmdline(ଖାଲି, get_list_ଧାଡ଼ି, (ଶୂନ୍ୟ *)&item,
		      DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT|DOCMD_KEYTYPED);
	--list->lv_refcount;
    }
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    // Need to append a NUL to the result.
    ଯଦି (ga_grow(&redir_execute_ga, 1) == OK)
    {
	((ଅକ୍ଷର *)redir_execute_ga.ga_data)[redir_execute_ga.ga_len] = NUL;
	rettv->vval.v_string = redir_execute_ga.ga_data;
    }
    ଅନ୍ୟଥା
    {
	ga_clear(&redir_execute_ga);
	rettv->vval.v_string = ଖାଲି;
    }
    ସନ୍ଦେଶ_silent = save_ସନ୍ଦେଶ_silent;
    emsg_silent = save_emsg_silent;
    emsg_noredir = save_emsg_noredir;

    redir_execute = save_redir_execute;
    ଯଦି (redir_execute)
	redir_execute_ga = save_ga;
    redir_off = save_redir_off;

    // "silent reg" or "silent echo x" leaves msg_col somewhere in the line.
    ଯଦି (echo_output)
	// When not working silently: put it in column zero.  A following
	// "echon" will overwrite the message, unavoidably.
	ସନ୍ଦେଶ_col = 0;
    ଅନ୍ୟଥା
	// When working silently: Put it back where it was, since nothing
	// should have been written.
	ସନ୍ଦେଶ_col = save_ସନ୍ଦେଶ_col;
}

/*
 * "execute()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_execute(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    execute_common(argvars, rettv, 0);
}

/*
 * "exists()" function
 */
    ଶୂନ୍ୟ
f_exists(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = FALSE;

    ଯଦି (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    p = tv_get_string(&argvars[0]);
    ଯଦି (*p == '$')			// environment variable
    {
	// first try "normal" environment variables (fast)
	ଯଦି (mch_getenv(p + 1) != ଖାଲି)
	    n = TRUE;
	ଅନ୍ୟଥା
	{
	    // try expanding things like $VIM and ${HOME}
	    p = expand_env_save(p);
	    ଯଦି (p != ଖାଲି && *p != '$')
		n = TRUE;
	    vim_free(p);
	}
    }
    ଅନ୍ୟଥା ଯଦି (*p == '&' || *p == '+')			// option
    {
	n = (eval_option(&p, ଖାଲି, TRUE) == OK);
	ଯଦି (*skipwhite(p) != NUL)
	    n = FALSE;			// trailing garbage
    }
    ଅନ୍ୟଥା ଯଦି (*p == '*')			// internal or user defined function
    {
	n = function_exists(p + 1, FALSE);
    }
    ଅନ୍ୟଥା ଯଦି (*p == '?')			// internal function only
    {
	n = has_internal_func_name(p + 1);
    }
    ଅନ୍ୟଥା ଯଦି (*p == ':')
    {
	n = cmd_exists(p + 1);
    }
    ଅନ୍ୟଥା ଯଦି (*p == '#')
    {
	ଯଦି (p[1] == '#')
	    n = autocmd_supported(p + 2);
	ଅନ୍ୟଥା
	    n = au_exists(p + 1);
    }
    ଅନ୍ୟଥା				// internal variable
    {
	n = var_exists(p);
    }

    rettv->vval.v_ସଂଖ୍ୟା = n;
}

    ସ୍ଥିର ଶୂନ୍ୟ
f_exists_compiled(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    emsg(_(e_exists_compiled_can_only_be_used_in_def_function));
}

/*
 * "expand()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_expand(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    size_t	len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options = WILD_SILENT|WILD_USE_NL|WILD_LIST_NOTFOUND;
    expand_T	xpc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    char_u	*result;
#ifdef BACKSLASH_IN_FILENAME
    char_u	*p_csl_save = p_csl;
#endif

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

#ifdef BACKSLASH_IN_FILENAME
    // avoid using 'completeslash' here
    p_csl = empty_option;
#endif

    rettv->v_type = VAR_STRING;
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN
	    && argvars[2].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[2], &error)
	    && !error)
	rettv_list_set(rettv, ଖାଲି);

    s = tv_get_string(&argvars[0]);
    ଯଦି (*s == '%' || *s == '#' || *s == '<')
    {
	ଅକ୍ଷର	*errormsg = ଖାଲି;

	ଯଦି (p_verbose == 0)
	    ++emsg_off;
	result = eval_vars(s, s, &len, ଖାଲି, &errormsg, ଖାଲି, FALSE);
	ଯଦି (p_verbose == 0)
	    --emsg_off;
	ଅନ୍ୟଥା ଯଦି (errormsg != ଖାଲି)
	    emsg(errormsg);
	ଯଦି (rettv->v_type == VAR_LIST)
	{
	    ଯଦି (rettv_list_alloc(rettv) == OK && result != ଖାଲି)
		list_append_string(rettv->vval.v_list, result, -1);
	    vim_free(result);
	}
	ଅନ୍ୟଥା
	    rettv->vval.v_string = result;
    }
    ଅନ୍ୟଥା
    {
	// When the optional second argument is non-zero, don't remove matches
	// for 'wildignore' and don't put matches for 'suffixes' at the end.
	ଯଦି (argvars[1].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	ଯଦି (!error)
	{
	    ExpandInit(&xpc);
	    xpc.xp_context = EXPAND_FILES;
	    ଯଦି (p_wic)
		options += WILD_ICASE;
	    ଯଦି (rettv->v_type == VAR_STRING)
		rettv->vval.v_string = ExpandOne(&xpc, s, ଖାଲି,
							   options, WILD_ALL);
	    ଅନ୍ୟଥା ଯଦି (rettv_list_alloc(rettv) == OK)
	    {
		ExpandOne(&xpc, s, ଖାଲି, options, WILD_ALL_KEEP);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < xpc.xp_numfiles; i++)
		    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
		ExpandCleanup(&xpc);
	    }
	}
	ଅନ୍ୟଥା
	    rettv->vval.v_string = ଖାଲି;
    }
#ifdef BACKSLASH_IN_FILENAME
    p_csl = p_csl_save;
#endif
}

/*
 * "expandcmd()" function
 * Expand all the special characters in a command string.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_expandcmd(typval_T *argvars, typval_T *rettv)
{
    exarg_T	eap;
    char_u	*cmdstr;
    ଅକ୍ଷର	*errormsg = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		emsgoff = TRUE;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type == VAR_DICT
		&& dict_get_bool(argvars[1].vval.v_dict, "errmsg", VVAL_FALSE))
	emsgoff = FALSE;

    rettv->v_type = VAR_STRING;
    cmdstr = vim_strsave(tv_get_string(&argvars[0]));

    CLEAR_FIELD(eap);
    eap.cmd = cmdstr;
    eap.arg = cmdstr;
    eap.argt |= EX_NOSPC;
    eap.usefilter = FALSE;
    eap.nextcmd = ଖାଲି;
    eap.cmdidx = CMD_USER;

    ଯଦି (emsgoff)
	++emsg_off;
    ଯଦି (expand_filename(&eap, &cmdstr, &errormsg) == FAIL)
	ଯଦି (!emsgoff && errormsg != ଖାଲି && *errormsg != NUL)
	    emsg(errormsg);
    ଯଦି (emsgoff)
	--emsg_off;

    rettv->vval.v_string = cmdstr;
}

/*
 * "feedkeys()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_feedkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		remap = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଭର୍ତ୍ତି = FALSE;
    char_u	*ଚାବିଗୁଡ଼ିକ, *flags;
    char_u	nbuf[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		typed = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		execute = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		context = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dangerous = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lowlevel = FALSE;

    // This is not allowed in the sandbox.  If the commands would still be
    // executed in the sandbox it would be OK, but it probably happens later,
    // when "sandbox" is no longer set.
    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଚାବିଗୁଡ଼ିକ = tv_get_string(&argvars[0]);

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	flags = tv_get_string_buf(&argvars[1], nbuf);
	ପାଇଁ ( ; *flags != NUL; ++flags)
	{
	    ବାଛ (*flags)
	    {
		ସ୍ଥିତି 'n': remap = FALSE; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'm': remap = TRUE; ଭାଙ୍ଗ;
		ସ୍ଥିତି 't': typed = TRUE; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'i': ଭର୍ତ୍ତି = TRUE; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'x': execute = TRUE; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'c': context = TRUE; ଭାଙ୍ଗ;
		ସ୍ଥିତି '!': dangerous = TRUE; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'L': lowlevel = TRUE; ଭାଙ୍ଗ;
	    }
	}
    }

    ଯଦି (*ଚାବିଗୁଡ଼ିକ != NUL || execute)
    {
	ଯଦି (lowlevel
#ifdef FEAT_VTP
		&& (!is_term_win32()
		    || (ଚାବିଗୁଡ଼ିକ[0] == 3 && ctrl_c_interrupts && typed))
#endif
	   )
	{
#ifdef USE_ନିବେଶ_BUF
	    ch_log(ଖାଲି, "feedkeys() lowlevel: %s", ଚାବିଗୁଡ଼ିକ);

	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ଚାବିଗୁଡ଼ିକ);
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < len; ++idx)
	    {
		// if a CTRL-C was typed, set got_int, similar to what
		// happens in fill_input_buf()
		ଯଦି (ଚାବିଗୁଡ଼ିକ[idx] == 3 && ctrl_c_interrupts && typed)
		    got_int = TRUE;
		add_to_ନିବେଶ_buf(ଚାବିଗୁଡ଼ିକ + idx, 1);
	    }
#ଅନ୍ୟଥା
	    emsg(_(e_lowlevel_ନିବେଶ_not_supported));
#endif
	}
	ଅନ୍ୟଥା
	{
	    // Need to escape K_SPECIAL and CSI before putting the string in
	    // the typeahead buffer.
	    char_u *ଚାବିଗୁଡ଼ିକ_esc = vim_strsave_escape_csi(ଚାବିଗୁଡ଼ିକ);
	    ଯଦି (ଚାବିଗୁଡ଼ିକ_esc == ଖାଲି)
		ଫେରନ୍ତୁ;

	    ch_log(ଖାଲି, "feedkeys(%s): %s", typed ? "typed" : "", ଚାବିଗୁଡ଼ିକ);

	    ins_typebuf(ଚାବିଗୁଡ଼ିକ_esc, (remap ? REMAP_YES : REMAP_NONE),
				   ଭର୍ତ୍ତି ? 0 : typebuf.tb_len, !typed, FALSE);
	    ଯଦି (vgetc_busy
#ifdef FEAT_TIMERS
		    || timer_busy
#endif
		    || ନିବେଶ_busy)
		typebuf_was_filled = TRUE;

	    vim_free(ଚାବିଗୁଡ଼ିକ_esc);
	}

	ଯଦି (execute)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ସନ୍ଦେଶ_scroll = ସନ୍ଦେଶ_scroll;
	    sctx_T	save_sctx;

	    // Avoid a 1 second delay when the keys start Insert mode.
	    ସନ୍ଦେଶ_scroll = FALSE;

	    ch_log(ଖାଲି, "feedkeys() executing");

	    ଯଦି (context)
	    {
		save_sctx = current_sctx;
		current_sctx.sc_sid = 0;
		current_sctx.sc_version = 0;
	    }

	    ଯଦି (!dangerous)
	    {
		++ex_normal_busy;
		++in_feedkeys;
	    }
	    ++allow_osc_ଚାବି;
	    exec_normal(TRUE, lowlevel, TRUE);
	    --allow_osc_ଚାବି;
	    ଯଦି (!dangerous)
	    {
		--ex_normal_busy;
		--in_feedkeys;
	    }

	    ସନ୍ଦେଶ_scroll |= save_ସନ୍ଦେଶ_scroll;

	    ଯଦି (context)
		current_sctx = save_sctx;
	}
    }
}

/*
 * "fnameescape({string})" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_fnameescape(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_string = vim_strsave_fnameescape(
					 tv_get_string(&argvars[0]), VSE_NONE);
    rettv->v_type = VAR_STRING;
}

/*
 * "foreground()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
    {
	gui_mch_set_foreground();
	ଫେରନ୍ତୁ;
    }
#endif
#ଯଦି defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    win32_set_foreground();
#endif
}

/*
 * "function()" function
 * "funcref()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
common_function(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_funcref)
{
    char_u	*s;
    char_u	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_string = FALSE;
    partial_T   *arg_pt = ଖାଲି;
    char_u	*trans_name = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_global = FALSE;
    char_u	*start_bracket = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_string_or_func_arg(argvars, 0) == FAIL
		|| check_for_opt_list_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_FUNC)
    {
	// function(MyFunc, [arg], dict)
	s = argvars[0].vval.v_string;
    }
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_PARTIAL
					 && argvars[0].vval.v_partial != ଖାଲି)
    {
	// function(dict.MyFunc, [arg])
	arg_pt = argvars[0].vval.v_partial;
	s = partial_name(arg_pt);
    }
    ଅନ୍ୟଥା
    {
	// function('MyFunc', [arg], dict)
	s = tv_get_string(&argvars[0]);
	use_string = TRUE;
    }
    ଯଦି (s == ଖାଲି)
    {
	semsg(_(e_invalid_argument_str), "NULL");
	ଫେରନ୍ତୁ;
    }

    ଯଦି ((use_string && vim_strchr(s, AUTOLOAD_CHAR) == ଖାଲି) || is_funcref)
    {
	name = s;
	trans_name = save_function_name(&name, &is_global, FALSE,
		   TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF, ଖାଲି);
	ଯଦି (*name == '<')
	{
	    // generic function
	    start_bracket = name;
	    ଯଦି (skip_generic_func_type_args(&name) == FAIL)
		ଯାଅ theend;
	}
	ଯଦି (*name != NUL)
	    s = ଖାଲି;
    }

    ଯଦି (s == ଖାଲି || *s == NUL || (use_string && VIM_ISDIGIT(*s))
					 || (is_funcref && trans_name == ଖାଲି))
	semsg(_(e_invalid_argument_str),
				  use_string ? tv_get_string(&argvars[0]) : s);
    // Don't check an autoload name for existence here.
    ଅନ୍ୟଥା ଯଦି (trans_name != ଖାଲି && (is_funcref
			 ? find_func(trans_name, is_global) == ଖାଲି
			 : !translated_function_exists(trans_name, is_global)))
	semsg(_(e_unknown_function_str_2), s);
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	dict_idx = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	arg_idx = 0;
	list_T	*list = ଖାଲି;

	ଯଦି (STRNCMP(s, "s:", 2) == 0 || STRNCMP(s, "<SID>", 5) == 0)
	    // Expand s: and <SID> into <SNR>nr_, so that the function can
	    // also be called from another script. Using trans_function_name()
	    // would also work, but some plugins depend on the name being
	    // printable text.
	    name = get_scriptlocal_funcname(s);
	ଅନ୍ୟଥା ଯଦି (trans_name != ଖାଲି && *trans_name == K_SPECIAL)
	    name = alloc_printable_func_name(trans_name);
	ଅନ୍ୟଥା
	    name = vim_strsave(s);

	ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	{
	    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	    {
		// function(name, [args], dict)
		arg_idx = 1;
		dict_idx = 2;
	    }
	    ଅନ୍ୟଥା ଯଦି (argvars[1].v_type == VAR_DICT)
		// function(name, dict)
		dict_idx = 1;
	    ଅନ୍ୟଥା
		// function(name, [args])
		arg_idx = 1;
	    ଯଦି (dict_idx > 0)
	    {
		ଯଦି (check_for_dict_arg(argvars, dict_idx) == FAIL)
		{
		    vim_free(name);
		    ଯାଅ theend;
		}
		ଯଦି (argvars[dict_idx].vval.v_dict == ଖାଲି)
		    dict_idx = 0;
	    }
	    ଯଦି (arg_idx > 0)
	    {
		ଯଦି (argvars[arg_idx].v_type != VAR_LIST)
		{
		    emsg(_(e_second_argument_of_function_must_be_list_or_dict));
		    vim_free(name);
		    ଯାଅ theend;
		}
		list = argvars[arg_idx].vval.v_list;
		ଯଦି (list == ଖାଲି || list->lv_len == 0)
		    arg_idx = 0;
		ଅନ୍ୟଥା ଯଦି (list->lv_len > MAX_FUNC_ARGS)
		{
		    emsg_funcname(e_too_many_arguments_for_function_str, s);
		    vim_free(name);
		    ଯାଅ theend;
		}
	    }
	}
	ଯଦି (dict_idx > 0 || arg_idx > 0 || arg_pt != ଖାଲି || is_funcref)
	{
	    partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	    // result is a VAR_PARTIAL
	    ଯଦି (pt == ଖାଲି)
		vim_free(name);
	    ଅନ୍ୟଥା
	    {
		ଯଦି (arg_idx > 0 || (arg_pt != ଖାଲି && arg_pt->pt_argc > 0))
		{
		    listitem_T	*li;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		arg_len = 0;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lv_len = 0;

		    ଯଦି (arg_pt != ଖାଲି)
			arg_len = arg_pt->pt_argc;
		    ଯଦି (list != ଖାଲି)
			lv_len = list->lv_len;
		    pt->pt_argc = arg_len + lv_len;
		    pt->pt_argv = ALLOC_MULT(typval_T, pt->pt_argc);
		    ଯଦି (pt->pt_argv == ଖାଲି)
		    {
			vim_free(pt);
			vim_free(name);
			ଯାଅ theend;
		    }
		    ପାଇଁ (i = 0; i < arg_len; i++)
			copy_tv(&arg_pt->pt_argv[i], &pt->pt_argv[i]);
		    ଯଦି (lv_len > 0)
		    {
			CHECK_LIST_MATERIALIZE(list);
			FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(list, li)
			    copy_tv(&li->li_tv, &pt->pt_argv[i++]);
		    }
		}

		// For "function(dict.func, [], dict)" and "func" is a partial
		// use "dict".  That is backwards compatible.
		ଯଦି (dict_idx > 0)
		{
		    // The dict is bound explicitly, pt_auto is FALSE.
		    pt->pt_dict = argvars[dict_idx].vval.v_dict;
		    ++pt->pt_dict->dv_refcount;
		}
		ଅନ୍ୟଥା ଯଦି (arg_pt != ଖାଲି)
		{
		    // If the dict was bound automatically the result is also
		    // bound automatically.
		    pt->pt_dict = arg_pt->pt_dict;
		    pt->pt_auto = arg_pt->pt_auto;
		    ଯଦି (pt->pt_dict != ଖାଲି)
			++pt->pt_dict->dv_refcount;
		    pt->pt_obj = arg_pt->pt_obj;
		    ଯଦି (pt->pt_obj != ଖାଲି)
			++pt->pt_obj->obj_refcount;
		}

		pt->pt_refcount = 1;
		ଯଦି (arg_pt != ଖାଲି && arg_pt->pt_func != ଖାଲି)
		{
		    pt->pt_func = arg_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		ଅନ୍ୟଥା ଯଦି (is_funcref)
		{
		    pt->pt_func = find_func(trans_name, is_global);
		    ଯଦି (IS_GENERIC_FUNC(pt->pt_func) && start_bracket != ଖାଲି)
			pt->pt_func = eval_generic_func(pt->pt_func, s,
							&start_bracket);
		    func_ptr_ref(pt->pt_func);
		    vim_free(name);
		}
		ଅନ୍ୟଥା
		{
		    pt->pt_name = name;
		    func_ref(name);
		}

		ଯଦି (arg_pt != ଖାଲି)
		{
		    pt->pt_outer_partial = arg_pt;
		    ++arg_pt->pt_refcount;
		}
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
	ଅନ୍ୟଥା
	{
	    // result is a VAR_FUNC
	    rettv->v_type = VAR_FUNC;
	    ଯଦି (start_bracket == ଖାଲି)
	    {
		rettv->vval.v_string = name;
		func_ref(name);
	    }
	    ଅନ୍ୟଥା
	    {
		// generic function
		size_t len = STRLEN(name) + STRLEN(start_bracket);
		rettv->vval.v_string = alloc(len + 1);
		ଯଦି (rettv->vval.v_string != ଖାଲି)
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
    ସ୍ଥିର ଶୂନ୍ୟ
f_funcref(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, TRUE);
}

/*
 * "function()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_function(typval_T *argvars, typval_T *rettv)
{
    common_function(argvars, rettv, FALSE);
}

/*
 * "garbagecollect()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_garbagecollect(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    // This is postponed until we are back at the toplevel, because we may be
    // using Lists and Dicts internally.  E.g.: ":echo [garbagecollect()]".
    want_garbage_collect = TRUE;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[0]) == 1)
	garbage_collect_at_exit = TRUE;
}

/*
 * "get()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_get(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    list_T	*l;
    tuple_T	*tuple;
    dictitem_T	*di;
    dict_T	*d;
    typval_T	*tv = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		what_is_dict = FALSE;

    ଯଦି (argvars[0].v_type == VAR_BLOB)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);

	ଯଦି (!error)
	{
	    rettv->v_type = VAR_ସଂଖ୍ୟା;
	    ଯଦି (idx < 0)
		idx = blob_len(argvars[0].vval.v_blob) + idx;
	    ଯଦି (idx < 0 || idx >= blob_len(argvars[0].vval.v_blob))
		rettv->vval.v_ସଂଖ୍ୟା = -1;
	    ଅନ୍ୟଥା
	    {
		rettv->vval.v_ସଂଖ୍ୟା = blob_get(argvars[0].vval.v_blob, idx);
		tv = rettv;
	    }
	}
    }
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_LIST)
    {
	ଯଦି ((l = argvars[0].vval.v_list) != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

	    li = list_find(l, (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error));
	    ଯଦି (!error && li != ଖାଲି)
		tv = &li->li_tv;
	}
    }
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_TUPLE)
    {
	ଯଦି ((tuple = argvars[0].vval.v_tuple) != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
	    long	idx;

	    idx = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);
	    ଯଦି (!error)
		tv = tuple_find(tuple, idx);
	}
    }
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_DICT)
    {
	ଯଦି ((d = argvars[0].vval.v_dict) != ଖାଲି)
	{
	    di = dict_find(d, tv_get_string(&argvars[1]), -1);
	    ଯଦି (di != ଖାଲି)
		tv = &di->di_tv;
	}
    }
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_PARTIAL || argvars[0].v_type == VAR_FUNC)
    {
	partial_T	*pt;
	partial_T	fref_pt;

	ଯଦି (argvars[0].v_type == VAR_PARTIAL)
	    pt = argvars[0].vval.v_partial;
	ଅନ୍ୟଥା
	{
	    CLEAR_FIELD(fref_pt);
	    fref_pt.pt_name = argvars[0].vval.v_string;
	    pt = &fref_pt;
	}

	ଯଦି (pt != ଖାଲି)
	{
	    char_u *what = tv_get_string(&argvars[1]);

	    ଯଦି (STRCMP(what, "func") == 0 || STRCMP(what, "name") == 0)
	    {
		char_u *name = partial_name(pt);

		rettv->v_type = (*what == 'f' ? VAR_FUNC : VAR_STRING);
		ଯଦି (name == ଖାଲି)
		    rettv->vval.v_string = ଖାଲି;
		ଅନ୍ୟଥା
		{
		    ଯଦି (rettv->v_type == VAR_FUNC)
			func_ref(name);
		    ଯଦି (*what == 'n' && pt->pt_name == ଖାଲି
							&& pt->pt_func != ଖାଲି)
			// use <SNR> instead of the byte code
			name = printable_func_name(pt->pt_func);
		    rettv->vval.v_string = vim_strsave(name);
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(what, "dict") == 0)
	    {
		what_is_dict = TRUE;
		ଯଦି (pt->pt_dict != ଖାଲି)
		    rettv_dict_set(rettv, pt->pt_dict);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(what, "args") == 0)
	    {
		rettv->v_type = VAR_LIST;
		ଯଦି (rettv_list_alloc(rettv) == OK)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

		    ପାଇଁ (i = 0; i < pt->pt_argc; ++i)
			list_append_tv(rettv->vval.v_list, &pt->pt_argv[i]);
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(what, "arity") == 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା required = 0, optional = 0, varargs = FALSE;
		char_u *name = partial_name(pt);

		get_func_arity(name, &required, &optional, &varargs);

		rettv->v_type = VAR_DICT;
		ଯଦି (rettv_dict_alloc(rettv) == OK)
		{
		    dict_T *dict = rettv->vval.v_dict;

		    // Take into account the arguments of the partial, if any.
		    // Note that it is possible to supply more arguments than the function
		    // accepts.
		    ଯଦି (pt->pt_argc >= required + optional)
			required = optional = 0;
		    ଅନ୍ୟଥା ଯଦି (pt->pt_argc > required)
		    {
			optional -= pt->pt_argc - required;
			required = 0;
		    }
		    ଅନ୍ୟଥା
			required -= pt->pt_argc;

		    dict_add_ସଂଖ୍ୟା(dict, "required", required);
		    dict_add_ସଂଖ୍ୟା(dict, "optional", optional);
		    dict_add_bool(dict, "varargs", varargs);
		}
	    }
	    ଅନ୍ୟଥା
		semsg(_(e_invalid_argument_str), what);

	    // When {what} == "dict" and pt->pt_dict == NULL, evaluate the
	    // third argument
	    ଯଦି (!what_is_dict)
		ଫେରନ୍ତୁ;
	}
    }
    ଅନ୍ୟଥା
	semsg(_(e_argument_of_str_must_be_list_tuple_dictionary_or_blob), "get()");

    ଯଦି (tv == ଖାଲି)
    {
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	    copy_tv(&argvars[2], rettv);
    }
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
f_getbgcolor(typval_T *argvars UNUSED, typval_T *rettv)
{
    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    {
	bool try_hl = false;

# ifdef FEAT_GUI
	ଯଦି (gui.in_use)
	    try_hl = true;
# endif
# ifdef FEAT_TERMGUICOLORS
	ଯଦି (p_tgc)
	    try_hl = true;
# endif
	ଯଦି (try_hl)
	{
	    guicolor_T	fg, bg;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hl_id = syn_name2id((char_u *)"Normal");

	    ଯଦି (hl_id > 0)
	    {
		(ଶୂନ୍ୟ)syn_id2colors(hl_id, &fg, &bg);
		ଯଦି (bg != INVALCOLOR)
		{
		    long_u  rgb = GUI_MCH_GET_RGB(bg);

		    list_append_ସଂଖ୍ୟା(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 16) & 0xff));
		    list_append_ସଂଖ୍ୟା(rettv->vval.v_list,
					    (varnumber_T)((rgb >> 8) & 0xff));
		    list_append_ସଂଖ୍ୟା(rettv->vval.v_list,
					    (varnumber_T)(rgb & 0xff));
		    ଫେରନ୍ତୁ;
		}
	    }
	}
    }
#endif
#ifdef FEAT_TERMRESPONSE
    {
	char_u	r = 0, g = 0, b = 0;

	ଯଦି (term_get_bg_color(&r, &g, &b) == OK)
	{
	    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)r);
	    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)g);
	    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)b);
	}
    }
#endif
}

/*
 * "getcellpixels()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getcellpixels(typval_T *argvars UNUSED, typval_T *rettv)
{
    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

#ଯଦି defined(FEAT_GUI)
    ଯଦି (gui.in_use)
    {
	// success pixel size and no gui.
	list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)gui.char_width);
	list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)gui.char_height);
    }
    ଅନ୍ୟଥା
#endif
    {
#ଯଦି defined(UNIX) || defined(MSWIN)
	list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)cell_width);
	list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)cell_height);
#ଅନ୍ୟଥା
	// Non-Unix CUIs are not supported, so set this to -1x-1.
	list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)-1);
	list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)-1);
#endif
    }

}

/*
 * "getchangelist()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getchangelist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    list_T	*l;
    dict_T	*d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		changelistindex;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    ଅନ୍ୟଥା
	buf = tv_get_buf_from_arg(&argvars[0]);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    l = list_alloc();
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	ଫେରନ୍ତୁ;
    }

    /*
     * The current window change list index tracks only the position for the
     * current buffer. For other buffers use the stored index for the current
     * window, or, if that's not available, the change list length.
     */
    ଯଦି (buf == curwin->w_buffer)
    {
	changelistindex = curwin->w_changelistidx;
    }
    ଅନ୍ୟଥା
    {
	wininfo_T	*wip;

	FOR_ALL_BUF_WININFO(buf, wip)
	    ଯଦି (wip->wi_win == curwin)
		ଭାଙ୍ଗ;
	changelistindex = wip != ଖାଲି ? wip->wi_changelistidx
							: buf->b_changelistlen;
    }
    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)changelistindex);

    ପାଇଁ (i = 0; i < buf->b_changelistlen; ++i)
    {
	ଯଦି (buf->b_changelist[i].lnum == 0)
	    ଜାରି;
	ଯଦି ((d = dict_alloc()) == ଖାଲି)
	    ଫେରନ୍ତୁ;
	ଯଦି (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ଫେରନ୍ତୁ;
	}
	dict_add_ସଂଖ୍ୟା(d, "lnum", (long)buf->b_changelist[i].lnum);
	dict_add_ସଂଖ୍ୟା(d, "col", (long)buf->b_changelist[i].col);
	dict_add_ସଂଖ୍ୟା(d, "coladd", (long)buf->b_changelist[i].coladd);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
getpos_both(
    typval_T	*argvars,
    typval_T	*rettv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		getcurpos,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		charcol)
{
    pos_T	*fp = ଖାଲି;
    pos_T	pos;
    win_T	*wp = curwin;
    list_T	*l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnum = -1;

    ଯଦି (rettv_list_alloc(rettv) == OK)
    {
	l = rettv->vval.v_list;
	ଯଦି (getcurpos)
	{
	    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
	    {
		wp = find_win_by_nr_or_id(&argvars[0]);
		ଯଦି (wp != ଖାଲି)
		    fp = &wp->w_cursor;
	    }
	    ଅନ୍ୟଥା
		fp = &curwin->w_cursor;
	    ଯଦି (fp != ଖାଲି && charcol)
	    {
		pos = *fp;
		pos.col =
		    buf_byteidx_to_charidx(wp->w_buffer, pos.lnum, pos.col);
		fp = &pos;
	    }
	}
	ଅନ୍ୟଥା
	    fp = var2fpos(&argvars[0], TRUE, &fnum, charcol);
	ଯଦି (fnum != -1)
	    list_append_ସଂଖ୍ୟା(l, (varnumber_T)fnum);
	ଅନ୍ୟଥା
	    list_append_ସଂଖ୍ୟା(l, (varnumber_T)0);
	list_append_ସଂଖ୍ୟା(l, (fp != ଖାଲି) ? (varnumber_T)fp->lnum
							    : (varnumber_T)0);
	list_append_ସଂଖ୍ୟା(l, (fp != ଖାଲି)
		     ? (varnumber_T)(fp->col == MAXCOL ? MAXCOL : fp->col + 1)
							    : (varnumber_T)0);
	list_append_ସଂଖ୍ୟା(l, (fp != ଖାଲି) ? (varnumber_T)fp->coladd :
							      (varnumber_T)0);
	ଯଦି (getcurpos)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_set_curswant = curwin->w_set_curswant;
	    colnr_T save_curswant = curwin->w_curswant;
	    colnr_T save_virtcol = curwin->w_virtcol;

	    ଯଦି (wp == curwin)
		update_curswant();
	    list_append_ସଂଖ୍ୟା(l, wp == ଖାଲି ? 0 : wp->w_curswant == MAXCOL
		    ?  (varnumber_T)MAXCOL : (varnumber_T)wp->w_curswant + 1);

	    // Do not change "curswant", as it is unexpected that a get
	    // function has a side effect.
	    ଯଦି (wp == curwin && save_set_curswant)
	    {
		curwin->w_set_curswant = save_set_curswant;
		curwin->w_curswant = save_curswant;
		curwin->w_virtcol = save_virtcol;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = FALSE;
}

/*
 * "getcharpos()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getcharpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    getpos_both(argvars, rettv, FALSE, TRUE);
}

/*
 * "getcharsearch()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getcharsearch(typval_T *argvars UNUSED, typval_T *rettv)
{
    ଯଦି (rettv_dict_alloc(rettv) != OK)
	ଫେରନ୍ତୁ;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_string(dict, "char", last_csearch());
    dict_add_ସଂଖ୍ୟା(dict, "forward", last_csearch_forward());
    dict_add_ସଂଖ୍ୟା(dict, "until", last_csearch_until());
}

/*
 * "getenv()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getenv(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    mustfree = FALSE;
    char_u  *p;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    p = vim_getenv(tv_get_string(&argvars[0]), &mustfree);
    ଯଦି (p == ଖାଲି)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_ସଂଖ୍ୟା = VVAL_NULL;
	ଫେରନ୍ତୁ;
    }
    ଯଦି (!mustfree)
	p = vim_strsave(p);
    rettv->vval.v_string = p;
    rettv->v_type = VAR_STRING;
}

/*
 * "getfontname()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getfontname(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
    {
	GuiFont font;
	char_u	*name = ଖାଲି;

	ଯଦି (argvars[0].v_type == VAR_UNKNOWN)
	{
	    // Get the "Normal" font.  Either the name saved by
	    // hl_set_font_name() or from the font ID.
	    font = gui.norm_font;
	    name = hl_get_font_name();
	}
	ଅନ୍ୟଥା
	{
	    name = tv_get_string(&argvars[0]);
	    ଯଦି (STRCMP(name, "*") == 0)	    // don't use font dialog
		ଫେରନ୍ତୁ;
	    font = gui_mch_get_font(name, FALSE);
	    ଯଦି (font == NOFONT)
		ଫେରନ୍ତୁ;	    // Invalid font name, return empty string.
	}
	rettv->vval.v_string = gui_mch_get_fontname(font, name);
	ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
	    gui_mch_free_font(font);
    }
#endif
}

/*
 * The types each argument check function accepts, for getinfo().  A check
 * that depends on another argument accepts "any".
 */
ସ୍ଥିର ସଂରଚନା
{
    argcheck_T	func;
    ଅକ୍ଷର	*types;	    // separated by commas
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
    {arg_list_ସଂଖ୍ୟା,		"list<number>"},
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
    {arg_ସଂଖ୍ୟା,		"number"},
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
    ସ୍ଥିର list_T *
argcheck_type_list(argcheck_T check)
{
    list_T	*l = list_alloc();
    ଅକ୍ଷର	*types = "any";

    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(argcheck_types); ++i)
	ଯଦି (argcheck_types[i].func == check)
	{
	    types = argcheck_types[i].types;
	    ଭାଙ୍ଗ;
	}
    ପାଇଁ (ଅକ୍ଷର *p = types; ; )
    {
	ଅକ୍ଷର	*end = (ଅକ୍ଷର *)vim_strchr((char_u *)p, ',');
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len = end == ଖାଲି ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p) : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - p);

	ଯଦି (list_append_string(l, (char_u *)p, len) == FAIL)
	{
	    list_free(l);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	ଯଦି (end == ଖାଲି)
	    ଭାଙ୍ଗ;
	p = end + 1;
    }
    ଫେରନ୍ତୁ l;
}

/*
 * Fill "argtypes" with the types in list "l" for builtin function "fe", the
 * "argtypes" item of the {opts} argument of getinfo().  Arguments that are
 * required but not in "l" get type "any".
 * Returns the number of arguments, -1 for an error.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
getinfo_argtypes(
	list_T			*l,
	ଅଚଳ funcentry_T	*fe,
	type2_T			*argtypes,
	garray_T		*type_gap)
{
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = 0;

    ଯଦି (l != ଖାଲି)
	FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
	{
	    char_u  *s = tv_get_string_chk(&li->li_tv);
	    char_u  *p = s;
	    type_T  *type;

	    ଯଦି (s == ଖାଲି)
		ଫେରନ୍ତୁ -1;
	    ଯଦି (argcount >= fe->f_max_argc || argcount >= MAX_FUNC_ARGS)
	    {
		semsg(_(e_too_many_arguments_for_function_str), fe->f_name);
		ଫେରନ୍ତୁ -1;
	    }
	    type = parse_type(&p, type_gap, ଖାଲି, ଖାଲି, TRUE);
	    ଯଦି (type == ଖାଲି)
		ଫେରନ୍ତୁ -1;
	    ଯଦି (*skipwhite(p) != NUL)
	    {
		semsg(_(e_type_not_recognized_str), s);
		ଫେରନ୍ତୁ -1;
	    }
	    argtypes[argcount].type_curr = type;
	    argtypes[argcount].type_decl = type;
	    ++argcount;
	}
    ପାଇଁ ( ; argcount < fe->f_min_argc && argcount < MAX_FUNC_ARGS; ++argcount)
    {
	argtypes[argcount].type_curr = &t_any;
	argtypes[argcount].type_decl = &t_any;
    }
    ଫେରନ୍ତୁ argcount;
}

/*
 * The name of the type builtin function "idx" returns when called with
 * "argcount" arguments of the types in "argtypes".  "tofree" is set to what
 * to free.
 */
    ସ୍ଥିର ଅକ୍ଷର *
builtin_ret_type_name(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount,
	type2_T	    *argtypes,
	garray_T    *type_gap,
	ଅକ୍ଷର	    **tofree)
{
    type_T	*decl_type;
    type_T	*ret = internal_func_ret_type(idx, argcount, argtypes,
							&decl_type, type_gap);

    ଫେରନ୍ତୁ type_name(ret, tofree);
}

/*
 * What getinfo() can look up, the {kind} argument.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    GI_COMMAND,
    GI_FUNCTION,
    GI_OPTION,
    GI_VIMVAR
} getinfo_kind_T;

ସ୍ଥିର ସଂରଚନା
{
    ଅକ୍ଷର		*name;
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
    ସ୍ଥିର ଶୂନ୍ୟ
f_getinfo(typval_T *argvars, typval_T *rettv)
{
    char_u		*kindname;
    char_u		*name;
    char_u		namebuf[NUMBUFLEN];
    getinfo_kind_T	kind;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ki;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			idx;
    ଅଚଳ funcentry_T	*fe;
    dict_T		*d;
    list_T		*args;
    char_u		*argnames;
    garray_T		type_gap;
    type2_T		argtypes[MAX_FUNC_ARGS];
    list_T		*argtypes_list = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			argcount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			vim9 = -1;	// -1: the context of the caller
    ଅକ୍ଷର		*ret_name;
    ଅକ୍ଷର		*tofree;

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (in_vim9script() && (check_for_string_arg(argvars, 0) == FAIL
				|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;
    ଯଦି (check_for_opt_dict_arg(argvars, 2) == FAIL)
	ଫେରନ୍ତୁ;

    kindname = tv_get_string(&argvars[0]);
    ପାଇଁ (ki = 0; ki < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(getinfo_kinds); ++ki)
	ଯଦି (STRCMP(kindname, getinfo_kinds[ki].name) == 0)
	    ଭାଙ୍ଗ;
    ଯଦି (ki == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(getinfo_kinds))
    {
	semsg(_(e_invalid_argument_str), kindname);
	ଫେରନ୍ତୁ;
    }
    kind = getinfo_kinds[ki].kind;
    name = tv_get_string_buf(&argvars[1], namebuf);

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN && argvars[2].vval.v_dict != ଖାଲି)
    {
	// Each item of {opts} is for one kind.
	dict_T	    *opts = argvars[2].vval.v_dict;
	hashitem_T  *hi;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)opts->dv_hashtab.ht_used;

	FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&opts->dv_hashtab, hi, todo)
	{
	    ଯଦି (HASHITEM_EMPTY(hi))
		ଜାରି;
	    --todo;
	    dictitem_T *di = HI2DI(hi);

	    ଯଦି (STRCMP(di->di_ଚାବି, "argtypes") == 0 && kind == GI_FUNCTION
		    && di->di_tv.v_type == VAR_LIST)
		argtypes_list = di->di_tv.vval.v_list;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(di->di_ଚାବି, "vim9") == 0 && kind == GI_COMMAND)
		vim9 = tv_get_bool(&di->di_tv);
	    ଅନ୍ୟଥା
	    {
		semsg(_(e_invalid_argument_str), di->di_ଚାବି);
		ଫେରନ୍ତୁ;
	    }
	}
    }
    ଯଦି (kind == GI_VIMVAR)
    {
	ଯଦି (STRNCMP(name, "v:", 2) == 0)
	    (ଶୂନ୍ୟ)vim_var_info(name + 2, rettv->vval.v_dict);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (kind == GI_COMMAND)
    {
	(ଶୂନ୍ୟ)ex_command_info(name, vim9, rettv->vval.v_dict);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (kind == GI_OPTION)
    {
	(ଶୂନ୍ୟ)option_info(name, rettv->vval.v_dict);
	ଫେରନ୍ତୁ;
    }
    // A builtin function, also one that is not implemented in this Vim, or a
    // user defined function when there is no builtin one with that name.
    idx = find_internal_func_opt(name, FALSE);
    ଯଦି (idx < 0)
    {
	(ଶୂନ୍ୟ)user_func_info(name, rettv->vval.v_dict);
	ଫେରନ୍ତୁ;
    }

    fe = &global_functions[idx];
    ga_init2(&type_gap, ଆକାର(type_T *), 10);
    argcount = getinfo_argtypes(argtypes_list, fe, argtypes, &type_gap);
    ଯଦି (argcount < 0)
    {
	clear_type_list(&type_gap);
	ଫେରନ୍ତୁ;
    }
    d = rettv->vval.v_dict;
    dict_add_string(d, "name", (char_u *)fe->f_name);
    dict_add_string(d, "kind", (char_u *)"builtin");
    dict_add_bool(d, "available", fe->f_func != ଖାଲି);
    dict_add_ସଂଖ୍ୟା(d, "minargs", fe->f_min_argc);
    dict_add_ସଂଖ୍ୟା(d, "maxargs", fe->f_max_argc == VARGS
						    ? -1 : fe->f_max_argc);
    dict_add_ସଂଖ୍ୟା(d, "method", fe->f_argtype & FEARG_MASK);

    args = list_alloc();
    ଯଦି (args == ଖାଲି || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	clear_type_list(&type_gap);
	ଫେରନ୍ତୁ;
    }
    argnames = (char_u *)fe->f_argnames;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; ; ++i)
    {
	argcheck_T  check;
	dict_T	    *arg;
	list_T	    *types;
	char_u	    *argname = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argnamelen = 0;

	ଯଦି (fe->f_argcheck == ଖାଲି)
	{
	    // Every argument is "any"; one item stands for all of them when
	    // there is no maximum.
	    ଯଦି (i >= (fe->f_max_argc == VARGS ? 1 : fe->f_max_argc))
		ଭାଙ୍ଗ;
	    check = arg_any;
	}
	ଅନ୍ୟଥା
	{
	    // One check per argument up to the maximum; without a maximum the
	    // checks end with NULL.
	    ଯଦି ((fe->f_max_argc != VARGS && i >= fe->f_max_argc)
						|| fe->f_argcheck[i] == ଖାଲି)
		ଭାଙ୍ଗ;
	    check = fe->f_argcheck[i];
	}
	// The name of the argument, when the help gives one.
	ଯଦି (argnames != ଖାଲି)
	{
	    char_u  *end = vim_strchr(argnames, ',');

	    argname = argnames;
	    argnamelen = end == ଖାଲି ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(argnames)
						     : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - argnames);
	    argnames = end == ଖାଲି ? ଖାଲି : skipwhite(end + 1);
	}
	arg = dict_alloc();
	types = argcheck_type_list(check);
	ଯଦି (arg == ଖାଲି || types == ଖାଲି
		|| dict_add_list(arg, "types", types) == FAIL)
	{
	    list_unref(types);
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    ଫେରନ୍ତୁ;
	}
	ଯଦି ((argnamelen > 0
		    && dict_add_string_len(arg, "name", argname, argnamelen)
								       == FAIL)
		|| list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    clear_type_list(&type_gap);
	    ଫେରନ୍ତୁ;
	}
    }

    // The type returned for arguments of the given types.  Without them "any"
    // when it differs with the number of arguments.
    ret_name = builtin_ret_type_name(idx, argcount, argtypes, &type_gap,
								    &tofree);
    ଯଦି (argtypes_list == ଖାଲି
	    && fe->f_max_argc != VARGS && fe->f_max_argc != fe->f_min_argc)
    {
	ଅକ୍ଷର	*tofree_max;
	ଅକ୍ଷର	*max_name;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = argcount; i < fe->f_max_argc && i < MAX_FUNC_ARGS; ++i)
	{
	    argtypes[i].type_curr = &t_any;
	    argtypes[i].type_decl = &t_any;
	}
	max_name = builtin_ret_type_name(idx, fe->f_max_argc, argtypes,
						    &type_gap, &tofree_max);

	ଯଦି (STRCMP(ret_name, max_name) != 0)
	{
	    vim_free(tofree);
	    tofree = ଖାଲି;
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
    ସ୍ଥିର ଶୂନ୍ୟ
f_getjumplist(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    list_T	*l;
    dict_T	*d;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL)))
	ଫେରନ୍ତୁ;

    wp = find_tabwin(&argvars[0], &argvars[1], ଖାଲି);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;

    cleanup_jumplist(wp, TRUE);

    l = list_alloc();
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (list_append_list(rettv->vval.v_list, l) == FAIL)
    {
	list_free(l);
	ଫେରନ୍ତୁ;
    }

    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)wp->w_jumplistidx);

    ପାଇଁ (i = 0; i < wp->w_jumplistlen; ++i)
    {
	ଯଦି (wp->w_jumplist[i].fmark.mark.lnum == 0)
	    ଜାରି;
	ଯଦି ((d = dict_alloc()) == ଖାଲି)
	    ଫେରନ୍ତୁ;
	ଯଦି (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ଫେରନ୍ତୁ;
	}
	dict_add_ସଂଖ୍ୟା(d, "lnum", (long)wp->w_jumplist[i].fmark.mark.lnum);
	dict_add_ସଂଖ୍ୟା(d, "col", (long)wp->w_jumplist[i].fmark.mark.col);
	dict_add_ସଂଖ୍ୟା(d, "coladd", (long)wp->w_jumplist[i].fmark.mark.coladd);
	dict_add_ସଂଖ୍ୟା(d, "bufnr", (long)wp->w_jumplist[i].fmark.fnum);
	ଯଦି (wp->w_jumplist[i].fname != ଖାଲି)
	    dict_add_string(d, "filename", wp->w_jumplist[i].fname);
    }
}

/*
 * "getpid()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getpid(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = mch_get_pid();
}

/*
 * "getcurpos()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getcurpos(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    getpos_both(argvars, rettv, TRUE, FALSE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
f_getcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    getpos_both(argvars, rettv, TRUE, TRUE);
}

/*
 * "getpos(string)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getpos(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    getpos_both(argvars, rettv, FALSE, FALSE);
}

/*
 * Convert from block_def to string
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
block_def2str(ସଂରଚନା block_def *bd, string_T *ret)
{
    ret->string = alloc(bd->startspaces + bd->endspaces + bd->textlen + 1);
    ଯଦି (ret->string == ଖାଲି)
    {
	ret->length = 0;
	ଫେରନ୍ତୁ FAIL;
    }

    vim_memset(ret->string, ' ', bd->startspaces);
    ret->length = bd->startspaces;
    mch_memmove(ret->string + ret->length, bd->textstart, bd->textlen);
    ret->length += bd->textlen;
    vim_memset(ret->string + ret->length, ' ', bd->endspaces);
    ret->length += bd->endspaces;
    ret->string[ret->length] = NUL;

    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
getregionpos(
    typval_T	*argvars,
    typval_T	*rettv,
    pos_T	*p1,
    pos_T	*p2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*inclusive,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*region_type,
    oparg_T	*oap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnum1 = -1, fnum2 = -1;
    char_u	*type;
    buf_T	*findbuf;
    char_u	default_type[] = "v";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		block_width = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_select_exclusive;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (list2fpos(&argvars[0], p1, &fnum1, ଖାଲି, FALSE) != OK
	    || list2fpos(&argvars[1], p2, &fnum2, ଖାଲି, FALSE) != OK
	    || fnum1 != fnum2)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (argvars[2].v_type == VAR_DICT)
    {
	is_select_exclusive = dict_get_bool(
		argvars[2].vval.v_dict, "exclusive", *p_sel == 'e');
	type = dict_get_string(
		argvars[2].vval.v_dict, "type", FALSE);
	ଯଦି (type == ଖାଲି)
	    type = default_type;
    }
    ଅନ୍ୟଥା
    {
	is_select_exclusive = *p_sel == 'e';
	type = default_type;
    }

    ଯଦି (type[0] == 'v' && type[1] == NUL)
	*region_type = MCHAR;
    ଅନ୍ୟଥା ଯଦି (type[0] == 'V' && type[1] == NUL)
	*region_type = MLINE;
    ଅନ୍ୟଥା ଯଦି (type[0] == Ctrl_V)
    {
	char_u *p = type + 1;

	ଯଦି (*p != NUL && ((block_width = getdigits(&p)) <= 0 || *p != NUL))
	{
	    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "type", type);
	    ଫେରନ୍ତୁ FAIL;
	}
	*region_type = MBLOCK;
    }
    ଅନ୍ୟଥା
    {
	semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "type", type);
	ଫେରନ୍ତୁ FAIL;
    }

    findbuf = fnum1 != 0 ? buflist_findnr(fnum1) : curbuf;
    ଯଦି (findbuf == ଖାଲି || findbuf->b_ml.ml_mfp == ଖାଲି)
    {
	emsg(_(e_buffer_is_not_loaded));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (p1->lnum < 1 || p1->lnum > findbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	semsg(_(e_invalid_ଧାଡ଼ି_ସଂଖ୍ୟା_nr), p1->lnum);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (p1->col == MAXCOL)
	p1->col = ml_get_buf_len(findbuf, p1->lnum) + 1;
    ଅନ୍ୟଥା ଯଦି (p1->col < 1 || p1->col > ml_get_buf_len(findbuf, p1->lnum) + 1)
    {
	semsg(_(e_invalid_column_ସଂଖ୍ୟା_nr), p1->col);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (p2->lnum < 1 || p2->lnum > findbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	semsg(_(e_invalid_ଧାଡ଼ି_ସଂଖ୍ୟା_nr), p2->lnum);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (p2->col == MAXCOL)
	p2->col = ml_get_buf_len(findbuf, p2->lnum) + 1;
    ଅନ୍ୟଥା ଯଦି (p2->col < 1 || p2->col > ml_get_buf_len(findbuf, p2->lnum) + 1)
    {
	semsg(_(e_invalid_column_ସଂଖ୍ୟା_nr), p2->col);
	ଫେରନ୍ତୁ FAIL;
    }

    curbuf = findbuf;
    curwin->w_buffer = curbuf;
    virtual_op = virtual_active();

    // NOTE: Adjustment is needed.
    p1->col--;
    p2->col--;

    ଯଦି (!LT_POS(*p1, *p2))
    {
	// swap position
	pos_T p;

	p = *p1;
	*p1 = *p2;
	*p2 = p;
    }

    ଯଦି (*region_type == MCHAR)
    {
	// Handle 'selection' == "exclusive".
	ଯଦି (is_select_exclusive && !EQUAL_POS(*p1, *p2))
	    // When backing up to previous line, inclusive becomes false.
	    *inclusive = !unadjust_for_sel_inner(p2);
	// If p2 is on NUL (end of line), inclusive becomes false.
	ଯଦି (*inclusive && !virtual_op && *ml_get_pos(p2) == NUL)
	    *inclusive = FALSE;
    }
    ଅନ୍ୟଥା ଯଦି (*region_type == MBLOCK)
    {
	colnr_T sc1, ec1, sc2, ec2;

#ifdef FEAT_LINEBREAK
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lbr_saved = reset_lbr();
#endif

	getvvcol(curwin, p1, &sc1, ଖାଲି, &ec1, 0);
	getvvcol(curwin, p2, &sc2, ଖାଲି, &ec2, 0);

#ifdef FEAT_LINEBREAK
	restore_lbr(lbr_saved);
#endif

	oap->motion_type = MBLOCK;
	oap->inclusive = TRUE;
	oap->op_type = OP_NOP;
	oap->start = *p1;
	oap->end = *p2;
	oap->start_vcol = MIN(sc1, sc2);
	ଯଦି (block_width > 0)
	    oap->end_vcol = oap->start_vcol + block_width - 1;
	ଅନ୍ୟଥା ଯଦି (is_select_exclusive && ec1 < sc2 && 0 < sc2 && ec2 > ec1)
	    oap->end_vcol = sc2 - 1;
	ଅନ୍ୟଥା
	    oap->end_vcol = MAX(ec1, ec2);
    }

    // Include the trailing byte of a multi-byte char.
    l = mb_ptr2len((char_u *)ml_get_pos(p2));
    ଯଦି (l > 1)
	p2->col += l - 1;

    ଫେରନ୍ତୁ OK;
}

/*
 * "getregion()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getregion(typval_T *argvars, typval_T *rettv)
{
    pos_T		p1, p2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			inclusive = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			region_type = -1;
    oparg_T		oa;

    buf_T		*save_curbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			save_virtual;
    string_T		akt = {ଖାଲି, 0};
    linenr_T		lnum;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    ଯଦି (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	ଫେରନ୍ତୁ;

    ପାଇଁ (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = OK;

	ଯଦି (region_type == MBLOCK)
	{
	    ସଂରଚନା block_def	bd;

	    block_prep(&oa, &bd, lnum, FALSE);
	    block_def2str(&bd, &akt);
	}
	ଅନ୍ୟଥା ଯଦି (region_type == MLINE
	    || (p1.lnum < lnum && lnum < p2.lnum))
	{
	    string_T	s;

	    s.string = ml_get(lnum);
	    s.length = ml_get_len(lnum);
	    akt.string = vim_strnsave(s.string, s.length);
	    ଯଦି (akt.string == ଖାଲି)
		akt.length = 0;
	    ଅନ୍ୟଥା
		akt.length = s.length;
	}
	ଅନ୍ୟଥା
	{
	    ସଂରଚନା block_def	bd;

	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);
	    block_def2str(&bd, &akt);
	}

	ଯଦି (akt.string == ଖାଲି)
	{
	    clear_tv(rettv);
	    (ଶୂନ୍ୟ)rettv_list_alloc(rettv);
	    ଭାଙ୍ଗ;
	}

	ret = list_append_string(rettv->vval.v_list, akt.string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)akt.length);
	vim_free(akt.string);

	ଯଦି (ret == FAIL)
	{
	    clear_tv(rettv);
	    (ଶୂନ୍ୟ)rettv_list_alloc(rettv);
	    ଭାଙ୍ଗ;
	}
    }

    // getregionpos() may change curbuf and virtual_op
    curbuf = save_curbuf;
    curwin->w_buffer = curbuf;
    virtual_op = save_virtual;
}

    ସ୍ଥିର ଶୂନ୍ୟ
add_regionpos_range(typval_T *rettv, pos_T p1, pos_T p2)
{
    list_T	*l1, *l2, *l3;

    l1 = list_alloc();
    ଯଦି (l1 == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (list_append_list(rettv->vval.v_list, l1) == FAIL)
    {
	list_free(l1);
	ଫେରନ୍ତୁ;
    }

    l2 = list_alloc();
    ଯଦି (l2 == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (list_append_list(l1, l2) == FAIL)
    {
	list_free(l2);
	ଫେରନ୍ତୁ;
    }

    l3 = list_alloc();
    ଯଦି (l3 == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (list_append_list(l1, l3) == FAIL)
    {
	list_free(l3);
	ଫେରନ୍ତୁ;
    }

    list_append_ସଂଖ୍ୟା(l2, curbuf->b_fnum);
    list_append_ସଂଖ୍ୟା(l2, p1.lnum);
    list_append_ସଂଖ୍ୟା(l2, p1.col);
    list_append_ସଂଖ୍ୟା(l2, p1.coladd);

    list_append_ସଂଖ୍ୟା(l3, curbuf->b_fnum);
    list_append_ସଂଖ୍ୟା(l3, p2.lnum);
    list_append_ସଂଖ୍ୟା(l3, p2.col);
    list_append_ସଂଖ୍ୟା(l3, p2.coladd);
}

/*
 * Compute the positions of the region segment on line "lnum".
 * "ret_p1" is set to the start position of the segment and "ret_p2" to its
 * end position.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
getregionpos_ଧାଡ଼ି(
    linenr_T	lnum,
    pos_T	p1,
    pos_T	p2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		inclusive,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		region_type,
    oparg_T	*oap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		allow_eol,
    pos_T	*ret_p1,
    pos_T	*ret_p2)
{
    char_u	*ଧାଡ଼ି = ml_get(lnum);
    colnr_T	ଧାଡ଼ି_len = ml_get_len(lnum);

    ଯଦି (region_type == MLINE)
    {
	ret_p1->col = 1;
	ret_p1->coladd = 0;
	ret_p2->col = MAXCOL;
	ret_p2->coladd = 0;
    }
    ଅନ୍ୟଥା
    {
	ସଂରଚନା block_def	bd;

	ଯଦି (region_type == MBLOCK)
	    block_prep(oap, &bd, lnum, FALSE);
	ଅନ୍ୟଥା
	    charwise_block_prep(p1, p2, &bd, lnum, inclusive);

	ଯଦି (bd.is_oneChar)  // selection entirely inside one char
	{
	    ଯଦି (region_type == MBLOCK)
	    {
		ret_p1->col = mb_prevptr(ଧାଡ଼ି, bd.textstart) - ଧାଡ଼ି + 1;
		ret_p1->coladd = bd.start_char_vcols
					   - (bd.start_vcol - oap->start_vcol);
	    }
	    ଅନ୍ୟଥା
	    {
		ret_p1->col = p1.col + 1;
		ret_p1->coladd = p1.coladd;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (region_type == MBLOCK && oap->start_vcol > bd.start_vcol)
	{
	    // blockwise selection entirely beyond end of line
	    ret_p1->col = MAXCOL;
	    ret_p1->coladd = oap->start_vcol - bd.start_vcol;
	    bd.is_oneChar = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (bd.startspaces > 0)
	{
	    ret_p1->col = mb_prevptr(ଧାଡ଼ି, bd.textstart) - ଧାଡ଼ି + 1;
	    ret_p1->coladd = bd.start_char_vcols - bd.startspaces;
	}
	ଅନ୍ୟଥା
	{
	    ret_p1->col = bd.textcol + 1;
	    ret_p1->coladd = 0;
	}

	ଯଦି (bd.is_oneChar)  // selection entirely inside one char
	{
	    ret_p2->col = ret_p1->col;
	    ret_p2->coladd = ret_p1->coladd + bd.startspaces + bd.endspaces;
	}
	ଅନ୍ୟଥା ଯଦି (bd.endspaces > 0)
	{
	    ret_p2->col = bd.textcol + bd.textlen + 1;
	    ret_p2->coladd = bd.endspaces;
	}
	ଅନ୍ୟଥା
	{
	    ret_p2->col = bd.textcol + bd.textlen;
	    ret_p2->coladd = 0;
	}
    }

    ଯଦି (!allow_eol && ret_p1->col > ଧାଡ଼ି_len)
    {
	ret_p1->col = 0;
	ret_p1->coladd = 0;
    }
    ଅନ୍ୟଥା ଯଦି (ret_p1->col > ଧାଡ଼ି_len + 1)
	ret_p1->col = ଧାଡ଼ି_len + 1;

    ଯଦି (!allow_eol && ret_p2->col > ଧାଡ଼ି_len)
    {
	ret_p2->col = ret_p1->col == 0 ? 0 : ଧାଡ଼ି_len;
	ret_p2->coladd = 0;
    }
    ଅନ୍ୟଥା ଯଦି (ret_p2->col > ଧାଡ଼ି_len + 1)
	ret_p2->col = ଧାଡ଼ି_len + 1;

    ret_p1->lnum = lnum;
    ret_p2->lnum = lnum;
}

/*
 * "getregionpos()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getregionpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	p1, p2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		inclusive = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		region_type = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		allow_eol = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bounds_only = FALSE;
    oparg_T	oa;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum;

    buf_T	*save_curbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_virtual;

    save_curbuf = curbuf;
    save_virtual = virtual_op;

    ଯଦି (getregionpos(argvars, rettv,
		&p1, &p2, &inclusive, &region_type, &oa) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[2].v_type == VAR_DICT)
    {
	allow_eol = dict_get_bool(argvars[2].vval.v_dict, "eol", FALSE);
	bounds_only = dict_get_bool(argvars[2].vval.v_dict, "bounds", FALSE);
    }

    ଯଦି (bounds_only)
    {
	// Only the outer bounds of the region are wanted, so the lines in
	// between do not have to be visited.
	pos_T	start_pos, end_pos;

	getregionpos_ଧାଡ଼ି(p1.lnum, p1, p2, inclusive, region_type, &oa,
					   allow_eol, &start_pos, &end_pos);
	ଯଦି (p2.lnum != p1.lnum)
	{
	    pos_T	unused;

	    getregionpos_ଧାଡ଼ି(p2.lnum, p1, p2, inclusive, region_type, &oa,
					      allow_eol, &unused, &end_pos);
	}
	add_regionpos_range(rettv, start_pos, end_pos);
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (lnum = p1.lnum; lnum <= p2.lnum; lnum++)
	{
	    pos_T	ret_p1, ret_p2;

	    getregionpos_ଧାଡ଼ି(lnum, p1, p2, inclusive, region_type, &oa,
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
getreg_get_regname(typval_T *argvars)
{
    char_u  *strregname;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	strregname = tv_get_string_chk(&argvars[0]);
	ଯଦି (strregname != ଖାଲି && in_vim9script() && STRLEN(strregname) > 1)
	{
	    semsg(_(e_register_name_must_be_one_char_str), strregname);
	    strregname = ଖାଲି;
	}
	ଯଦି (strregname == ଖାଲି)	    // type error; errmsg already given
	    ଫେରନ୍ତୁ 0;
    }
    ଅନ୍ୟଥା
	// Default to v:register
	strregname = get_vim_var_str(VV_REG);

    ଫେରନ୍ତୁ *strregname == 0 ? '"' : *strregname;
}

/*
 * "getreg()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getreg(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		arg2 = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		return_list = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_opt_string_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 2) == FAIL)))))
	ଫେରନ୍ତୁ;

    regname = getreg_get_regname(argvars);
    ଯଦି (regname == 0)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN && argvars[1].v_type != VAR_UNKNOWN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

	arg2 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[1], &error);

	ଯଦି (!error && argvars[2].v_type != VAR_UNKNOWN)
	    return_list = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[2], &error);
	ଯଦି (error)
	    ଫେରନ୍ତୁ;
    }

    ଯଦି (return_list)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = (list_T *)get_reg_contents(regname,
				      (arg2 ? GREG_ପଦସମଷ୍ଟି_SRC : 0) | GREG_LIST);
	ଯଦି (rettv->vval.v_list == ଖାଲି)
	    (ଶୂନ୍ୟ)rettv_list_alloc(rettv);
	ଅନ୍ୟଥା
	    ++rettv->vval.v_list->lv_refcount;
    }
    ଅନ୍ୟଥା
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = get_reg_contents(regname,
						    arg2 ? GREG_ପଦସମଷ୍ଟି_SRC : 0);
    }
}

/*
 * "getregtype()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getregtype(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regname;
    char_u	buf[NUMBUFLEN + 2];
    long	reglen = 0;

    // on error return an empty string
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    regname = getreg_get_regname(argvars);
    ଯଦି (regname == 0)
	ଫେରନ୍ତୁ;

    buf[0] = NUL;
    buf[1] = NUL;
    ବାଛ (get_reg_type(regname, &reglen))
    {
	ସ୍ଥିତି MLINE: buf[0] = 'V'; ଭାଙ୍ଗ;
	ସ୍ଥିତି MCHAR: buf[0] = 'v'; ଭାଙ୍ଗ;
	ସ୍ଥିତି MBLOCK:
		buf[0] = Ctrl_V;
		vim_snprintf((ଅକ୍ଷର *)buf + 1, NUMBUFLEN + 1, "%ld", reglen + 1);
		ଭାଙ୍ଗ;
    }
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "gettagstack()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_gettagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;			// default is current window

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	ଯଦି (wp == ଖାଲି)
	    ଫେରନ୍ତୁ;
    }

    get_tagstack(wp, rettv->vval.v_dict);
}

/*
 * "gettext()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_gettext(typval_T *argvars, typval_T *rettv)
{
#ଯଦି defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    ଅକ୍ଷର *prev = ଖାଲି;
#endif

    ଯଦି (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_opt_string_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_STRING;

    ଯଦି (argvars[1].v_type == VAR_STRING &&
	    argvars[1].vval.v_string != ଖାଲି &&
	    *(argvars[1].vval.v_string) != NUL)
    {
#ଯଦି defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ଅଚଳ ଅକ୍ଷର *)argvars[1].vval.v_string, (ଅକ୍ଷର *)p_enc);
#endif

#ଯଦି defined(HAVE_DGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dgettext((ଅଚଳ ଅକ୍ଷର *)argvars[1].vval.v_string, (ଅଚଳ ଅକ୍ଷର *)argvars[0].vval.v_string));
#ଅନ୍ୟଥା
	textdomain((ଅଚଳ ଅକ୍ଷର *)argvars[1].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
	textdomain(VIMPACKAGE);
#endif

#ଯଦି defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	ଯଦି (prev != ଖାଲି)
	    bind_textdomain_codeset((ଅଚଳ ଅକ୍ଷର *)argvars[1].vval.v_string, prev);
#endif
    }
    ଅନ୍ୟଥା
	rettv->vval.v_string = vim_strsave((char_u *)_(argvars[0].vval.v_string));
}

// for VIM_VERSION_ defines
#ଅନ୍ତର୍ଭାଵ "version.h"

/*
 * "has()" function
 */
    ଶୂନ୍ୟ
f_has(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = FALSE;
    ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
	ଅକ୍ଷର *name;
	short present;
    } has_item_T;
    ସ୍ଥିର has_item_T has_list[] =
    {
	{"amiga",
#ifdef AMIGA
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"android",
#ifdef __ANDROID__
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"arp",
#ଯଦି defined(AMIGA) && defined(FEAT_ARP)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"haiku",
#ifdef __HAIKU__
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"bsd",
#ଯଦି defined(BSD) && !defined(MACOS_X) && !defined(__GNU__)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"hpux",
#ifdef hpux
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"hurd",
#ifdef __GNU__
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"linux",
#ifdef __linux__
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"mac",		// Mac OS X (and, once, Mac OS Classic)
#ifdef MACOS_X
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"osx",		// Mac OS X
#ifdef MACOS_X
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"macunix",	// Mac OS X, with the darwin feature
#ଯଦି defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"osxdarwin",	// synonym for macunix
#ଯଦି defined(MACOS_X) && defined(MACOS_X_DARWIN)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"qnx",
#ifdef __QNX__
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"sun",
#ifdef SUN_SYSTEM
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"termux",
#ifdef __TERMUX__
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"unix",
#ifdef UNIX
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"vms",
#ifdef VMS
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"win32",
#ifdef MSWIN
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"win32unix",
#ifdef WIN32UNIX
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"win64",
#ifdef _WIN64
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"ebcdic", 0 },
	{"fname_case",
#ifndef CASE_INSENSITIVE_FILENAME
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"acl",
#ifdef HAVE_ACL
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"arabic",
#ifdef FEAT_ARABIC
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"autocmd", 1},
	{"autochdir",
#ifdef FEAT_AUTOCHDIR
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"autoservername",
#ifdef FEAT_AUTOSERVERNAME
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"socketserver",
#ifdef FEAT_SOCKETSERVER
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"balloon_eval",
#ifdef FEAT_BEVAL_GUI
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"balloon_multiline",
#ifdef FEAT_BEVAL_GUI
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"balloon_eval_term",
#ifdef FEAT_BEVAL_TERM
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"builtin_terms", 1},
	{"all_builtin_terms", 1},
	{"browsefilter",
#ଯଦି defined(FEAT_BROWSE) && (defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"byte_offset",
#ifdef FEAT_BYTEOFF
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"channel",
#ifdef FEAT_JOB_CHANNEL
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"cindent", 1},
	{"clientserver",
#ifdef FEAT_CLIENTSERVER
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"clipboard",
#ifdef FEAT_CLIPBOARD
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"clipboard_provider",
#ifdef FEAT_CLIPBOARD_PROVIDER
		1
#ଅନ୍ୟଥା
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
#ଅନ୍ୟଥା
		0
#endif
		},
	{"cryptv",
#ifdef FEAT_CRYPT
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"crypt-blowfish",
#ifdef FEAT_CRYPT
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"crypt-blowfish2",
#ifdef FEAT_CRYPT
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"cscope",
#ifdef FEAT_CSCOPE
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"cursorbind", 1},
	{"cursorshape",
#ifdef CURSOR_SHAPE
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"debug",
#ifdef DEBUG
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"dialog_con",
#ifdef FEAT_CON_DIALOG
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"dialog_con_gui",
#ଯଦି defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"dialog_gui",
#ifdef FEAT_GUI_DIALOG
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"diff",
#ifdef FEAT_DIFF
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"digraphs",
#ifdef FEAT_DIGRAPHS
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"directx",
#ifdef FEAT_DIRECTX
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"dnd",
#ifdef FEAT_DND
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"drop_file",
#ifdef HAVE_DROP_FILE
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"emacs_tags",
#ifdef FEAT_EMACS_TAGS
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"eval", 1},		// always present, of course!
	{"ex_extra", 1},	// graduated feature
	{"extra_search",
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"file_in_path", 1},
	{"filterpipe",
#ଯଦି defined(FEAT_FILTERPIPE) && !defined(VIMDLL)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"find_in_path",
#ifdef FEAT_FIND_ID
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"float", 1},
	{"folding",
#ifdef FEAT_FOLDING
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"footer", 0},
	{"fork",
#ଯଦି !defined(USE_SYSTEM) && defined(UNIX)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"gettext",
#ifdef FEAT_GETTEXT
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"gui",
#ifdef FEAT_GUI
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"gui_neXtaw", 0 },
	{"gui_athena", 0 },
	{"gui_gtk",
#ifdef FEAT_GUI_GTK
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"gui_gtk2",
#ଯଦି defined(FEAT_GUI_GTK) && !defined(USE_GTK3) && !defined(USE_GTK4)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"gui_gtk3",
#ଯଦି defined(FEAT_GUI_GTK) && defined(USE_GTK3)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"gui_gtk4",
#ଯଦି defined(FEAT_GUI_GTK) && defined(USE_GTK4)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"gui_gnome",
#ifdef FEAT_GUI_GNOME
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"gui_haiku",
#ifdef FEAT_GUI_HAIKU
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"gui_mac", 0},
	{"gui_motif",
#ifdef FEAT_GUI_MOTIF
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"gui_photon",
#ifdef FEAT_GUI_PHOTON
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"gui_win32",
#ifdef FEAT_GUI_MSWIN
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"iconv",
#ଯଦି defined(HAVE_ICONV_H) && defined(USE_ICONV)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"image",
#ifdef FEAT_IMAGE
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"image_popup",
#ifdef FEAT_IMAGE_POPUP
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"insert_expand", 1},
	{"ipv6",
#ifdef FEAT_IPV6
		1
#ଅନ୍ୟଥା
		0
#endif
	},
	{"job",
#ifdef FEAT_JOB_CHANNEL
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"jumplist", 1},
	{"keymap",
#ifdef FEAT_KEYMAP
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"lambda", 1}, // always with FEAT_EVAL, since 7.4.2120 with closure
	{"langmap",
#ifdef FEAT_LANGMAP
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"libcall",
#ifdef FEAT_LIBCALL
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"linebreak",
#ifdef FEAT_LINEBREAK
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"lispindent", 1},
	{"listcmds", 1},
	{"localmap", 1},
	{"lua",
#ଯଦି defined(FEAT_LUA) && !defined(DYNAMIC_LUA)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"menu",
#ifdef FEAT_MENU
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"mksession",
#ifdef FEAT_SESSION
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"modify_fname", 1},
	{"mouse", 1},
	{"mouseshape",
#ifdef FEAT_MOUSESHAPE
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"mouse_dec",
#ଯଦି (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_DEC)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"mouse_gpm",
#ଯଦି (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_GPM) && !defined(DYNAMIC_GPM)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"mouse_jsbterm",
#ଯଦି (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_JSB)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"mouse_netterm",
#ଯଦି (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_NET)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"mouse_pterm",
#ଯଦି (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_PTERM)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"mouse_sgr",
#ଯଦି (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"mouse_sysmouse",
#ଯଦି (defined(UNIX) || defined(VMS)) && defined(FEAT_SYSMOUSE)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"mouse_urxvt",
#ଯଦି (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_URXVT)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"mouse_xterm",
#ଯଦି (defined(UNIX) || defined(VMS)) && defined(FEAT_MOUSE_XTERM)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"multi_byte", 1},
	{"multi_byte_ime",
#ifdef FEAT_MBYTE_IME
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"multi_lang",
#ifdef FEAT_MULTI_LANG
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"mzscheme",
#ଯଦି defined(FEAT_MZSCHEME) && !defined(DYNAMIC_MZSCHEME)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"nanotime",
#ifdef ST_MTIM_NSEC
		1
#ଅନ୍ୟଥା
		0
#endif
	},
	{"num64", 1},
	{"ole",
#ifdef FEAT_OLE
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"packages",
#ifdef FEAT_EVAL
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"path_extra", 1},
	{"perl",
#ଯଦି defined(FEAT_PERL) && !defined(DYNAMIC_PERL)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"persistent_undo",
#ifdef FEAT_PERSISTENT_UNDO
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"python_compiled",
#ଯଦି defined(FEAT_PYTHON)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"python_dynamic",
#ଯଦି defined(FEAT_PYTHON) && defined(DYNAMIC_PYTHON)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"python",
#ଯଦି defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"pythonx",
#ଯଦି (defined(FEAT_PYTHON) && !defined(DYNAMIC_PYTHON)) \
	|| (defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3))
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"python3_compiled",
#ଯଦି defined(FEAT_PYTHON3)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"python3_dynamic",
#ଯଦି defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"python3_stable",
#ଯଦି defined(FEAT_PYTHON3) && defined(DYNAMIC_PYTHON3_STABLE_ABI)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"python3",
#ଯଦି defined(FEAT_PYTHON3) && !defined(DYNAMIC_PYTHON3)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"popupwin",
#ifdef FEAT_PROP_POPUP
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"postscript",
#ifdef FEAT_POSTSCRIPT
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"pango",
#ifdef FEAT_PRINT_PANGO
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"printer",
#ifdef FEAT_PRINTER
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"profile",
#ifdef FEAT_PROFILE
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"prof_nsec",
#ifdef PROF_NSEC
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"reltime",
#ifdef FEAT_RELTIME
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"quickfix",
#ifdef FEAT_QUICKFIX
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"rightleft",
#ifdef FEAT_RIGHTLEFT
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"ruby",
#ଯଦି defined(FEAT_RUBY) && !defined(DYNAMIC_RUBY)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"scrollbind", 1},
	{"showcmd", 1},
	{"cmdline_info", 1},
	{"signs",
#ifdef FEAT_SIGNS
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"smartindent", 1},
	{"startuptime",
#ifdef STARTUPTIME
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"statusline",
#ifdef FEAT_STL_OPT
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"statusline_click",
#ifdef FEAT_STL_OPT
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"netbeans_intg",
#ifdef FEAT_NETBEANS_INTG
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"sodium",
#ଯଦି defined(FEAT_SODIUM) && !defined(DYNAMIC_SODIUM)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"sound",
#ifdef FEAT_SOUND
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"spell",
#ifdef FEAT_SPELL
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"syntax",
#ifdef FEAT_SYN_HL
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"system",
#ଯଦି defined(USE_SYSTEM) || !defined(UNIX)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"tabpanel",
#ଯଦି defined(FEAT_TABPANEL)
		1,
#ଅନ୍ୟଥା
		0,
#endif
	},
	{"tag_binary", 1},	// graduated feature
	{"tcl",
#ଯଦି defined(FEAT_TCL) && !defined(DYNAMIC_TCL)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"termguicolors",
#ifdef FEAT_TERMGUICOLORS
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"terminal",
#ଯଦି defined(FEAT_TERMINAL) && !defined(MSWIN)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"terminfo",
#ifdef TERMINFO
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"termresponse",
#ifdef FEAT_TERMRESPONSE
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"textobjects", 1},
	{"textprop",
#ifdef FEAT_PROP_POPUP
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"tgetent",
#ifdef HAVE_TGETENT
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"timers",
#ifdef FEAT_TIMERS
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"title", 1},
	{"toolbar",
#ifdef FEAT_TOOLBAR
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"user-commands", 1},    // was accidentally included in 5.4
	{"user_commands", 1},
	{"vartabs",
#ifdef FEAT_VARTABS
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"vertsplit", 1},
	{"viminfo",
#ifdef FEAT_VIMINFO
		1
#ଅନ୍ୟଥା
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
#ଅନ୍ୟଥା
		0
#endif
		},
	{"wayland",
#ifdef FEAT_WAYLAND
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"wayland_clipboard",
#ifdef FEAT_WAYLAND_CLIPBOARD
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"wildignore", 1},
	{"wildmenu", 1},
	{"windows", 1},
	{"winaltkeys",
#ifdef FEAT_WAK
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"writebackup",
#ifdef FEAT_WRITEBACKUP
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"xattr",
#ifdef FEAT_XATTR
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"xim",
#ifdef FEAT_XIM
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"xfontset",
#ifdef FEAT_XFONTSET
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"xpm",
#ଯଦି defined(FEAT_XPM_W32) || defined(HAVE_XPM)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"xpm_w32",	// for backward compatibility
#ifdef FEAT_XPM_W32
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"xsmp",
#ifdef USE_XSMP
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"xsmp_interact",
#ifdef USE_XSMP_INTERACT
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"xterm_clipboard",
#ifdef FEAT_XCLIPBOARD
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"xterm_save",
#ifdef FEAT_XTERM_SAVE
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{"X11",
#ଯଦି defined(UNIX) && defined(FEAT_X11)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{":tearoff",
// same #ifdef as used for ex_tearoff().
#ଯଦି defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		1
#ଅନ୍ୟଥା
		0
#endif
		},
	{ଖାଲି, 0}
    };

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    name = tv_get_string(&argvars[0]);

    // Fast-path: check features not in has_list[] first to avoid the full
    // linear scan for very common queries like has('patch-...').
    ଯଦି (STRNICMP(name, "patch", 5) == 0)
    {
	x = TRUE;
	ଯଦି (name[5] == '-'
		&& STRLEN(name) >= 11
		&& (name[6] >= '1' && name[6] <= '9'))
	{
	    ଅକ୍ଷର	*end;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	major, minor;

	    // This works for patch-8.1.2, patch-9.0.3, patch-10.0.4, etc.
	    // Not for patch-9.10.5.
	    major = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strtoul((ଅକ୍ଷର *)name + 6, &end, 10);
	    ଯଦି (*end == '.' && vim_isdigit(end[1])
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
	ଅନ୍ୟଥା ଯଦି (SAFE_isdigit(name[5]))
	    n = has_patch(atoi((ଅକ୍ଷର *)name + 5));
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "vim_starting") == 0)
    {
	x = TRUE;
	n = (starting != 0);
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "ttyin") == 0)
    {
	x = TRUE;
	n = mch_ନିବେଶ_isatty();
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "ttyout") == 0)
    {
	x = TRUE;
	n = stdout_isatty;
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "multi_byte_encoding") == 0)
    {
	x = TRUE;
	n = has_mbyte;
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "gui_running") == 0)
    {
	x = TRUE;
#ifdef FEAT_GUI
	n = (gui.in_use || gui.starting);
#endif
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "browse") == 0)
    {
	x = TRUE;
#ଯଦି defined(FEAT_GUI) && defined(FEAT_BROWSE)
	n = gui.in_use;	// gui_mch_browse() works when GUI is running
#endif
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "syntax_items") == 0)
    {
	x = TRUE;
#ifdef FEAT_SYN_HL
	n = syntax_present(curwin);
#endif
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "vcon") == 0)
    {
	x = TRUE;
#ifdef FEAT_VTP
	n = is_term_win32() && has_vtp_working();
#endif
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "netbeans_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_NETBEANS_INTG
	n = netbeans_active();
#endif
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "mouse_gpm_enabled") == 0)
    {
	x = TRUE;
#ifdef FEAT_MOUSE_GPM
	n = gpm_enabled();
#endif
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "conpty") == 0)
    {
	x = TRUE;
#ଯଦି defined(FEAT_TERMINAL) && defined(MSWIN)
	n = use_conpty();
#endif
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "clipboard_working") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	n = clipmethod == CLIPMETHOD_PROVIDER ? TRUE : clip_star.available;
#endif
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "unnamedplus") == 0)
    {
	x = TRUE;
#ifdef FEAT_CLIPBOARD
	// The + register is available when clipmethod is set to a provider,
	// but becomes unavailable if on a platform that doesn't support it
	// and clipmethod is "none".
	// (Windows, MacOS).
# ଯଦି defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
	n = TRUE;
# elif defined(FEAT_EVAL)
	ଯଦି (clipmethod == CLIPMETHOD_PROVIDER)
	    n = TRUE;
	ଅନ୍ୟଥା
	    n = FALSE;
# ଅନ୍ୟଥା
	n = FALSE;
# endif
#endif
    }

    // Look up in has_list[] only if not already handled above.
    ଯଦି (x == FALSE)
    {
	ପାଇଁ (i = 0; has_list[i].name != ଖାଲି; ++i)
	    ଯଦି (STRICMP(name, has_list[i].name) == 0)
	    {
		x = TRUE;
		n = has_list[i].present;
		ଭାଙ୍ଗ;
	    }

	// features also in has_list[] but sometimes enabled at runtime
	ଯଦି (x == TRUE && n == FALSE)
	{
	    ଯଦି (0)
	    {
		// intentionally empty
	    }
#ifdef VIMDLL
	    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "filterpipe") == 0)
		n = gui.in_use || gui.starting;
#endif
#ଯଦି defined(USE_ICONV) && defined(DYNAMIC_ICONV)
	    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "iconv") == 0)
		n = iconv_enabled(FALSE);
#endif
#ifdef DYNAMIC_LUA
	    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "lua") == 0)
		n = lua_enabled(FALSE);
#endif
#ifdef DYNAMIC_MZSCHEME
	    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "mzscheme") == 0)
		n = mzscheme_enabled(FALSE);
#endif
#ifdef DYNAMIC_PERL
	    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "perl") == 0)
		n = perl_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON
	    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "python") == 0)
		n = python_enabled(FALSE);
#endif
#ifdef DYNAMIC_PYTHON3
	    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "python3") == 0)
		n = python3_enabled(FALSE);
#endif
#ଯଦି defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
	    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "pythonx") == 0)
	    {
# ଯଦି defined(DYNAMIC_PYTHON) && defined(DYNAMIC_PYTHON3)
		ଯଦି (p_pyx == 0)
		    n = python3_enabled(FALSE) || python_enabled(FALSE);
		ଅନ୍ୟଥା ଯଦି (p_pyx == 3)
		    n = python3_enabled(FALSE);
		ଅନ୍ୟଥା ଯଦି (p_pyx == 2)
		    n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON)
		n = python_enabled(FALSE);
# elif defined(DYNAMIC_PYTHON3)
		n = python3_enabled(FALSE);
# endif
	    }
#endif
#ifdef DYNAMIC_RUBY
	    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "ruby") == 0)
		n = ruby_enabled(FALSE);
#endif
#ifdef DYNAMIC_TCL
	    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "tcl") == 0)
		n = tcl_enabled(FALSE);
#endif
#ifdef DYNAMIC_SODIUM
	    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "sodium") == 0)
		n = sodium_enabled(FALSE);
#endif
#ଯଦି defined(FEAT_TERMINAL) && defined(MSWIN)
	    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "terminal") == 0)
		n = terminal_enabled();
#endif
#ifdef DYNAMIC_GPM
	    ଅନ୍ୟଥା ଯଦି (STRICMP(name, "mouse_gpm") == 0)
		n = gpm_available();
#endif
	}
    }

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
	// return whether feature could ever be enabled
	rettv->vval.v_ସଂଖ୍ୟା = x;
    ଅନ୍ୟଥା
	// return whether feature is enabled
	rettv->vval.v_ସଂଖ୍ୟା = n;
}

/*
 * Return TRUE if "feature" can change later.
 * Also when checking for the feature has side effects, such as loading a DLL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dynamic_feature(char_u *feature)
{
    ଫେରନ୍ତୁ (feature == ଖାଲି
#ଯଦି defined(FEAT_GUI) && defined(FEAT_BROWSE)
	    || (STRICMP(feature, "browse") == 0 && !gui.in_use)
#endif
#ifdef VIMDLL
	    || STRICMP(feature, "filterpipe") == 0
#endif
#ଯଦି defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
	    // this can only change on Unix where the ":gui" command could be
	    // used.
	    || (STRICMP(feature, "gui_running") == 0 && !gui.in_use)
#endif
#ଯଦି defined(USE_ICONV) && defined(DYNAMIC_ICONV)
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
#ଯଦି defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
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
#ଯଦି defined(FEAT_TERMINAL) && defined(MSWIN)
	    || STRICMP(feature, "conpty") == 0
#endif
	    );
}

/*
 * "haslocaldir()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_haslocaldir(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp = ଖାଲି;
    win_T	*wp = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL)))
	ଫେରନ୍ତୁ;

    wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    // Check for window-local and tab-local directories
    ଯଦି (wp != ଖାଲି && wp->w_localdir != ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = 1;
    ଅନ୍ୟଥା ଯଦି (tp != ଖାଲି && tp->tp_localdir != ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = 2;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = 0;
}

/*
 * "highlightID(name)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_hlID(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = syn_name2id(tv_get_string(&argvars[0]));
}

/*
 * "highlight_exists()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_hlexists(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = highlight_exists(tv_get_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
# ଯଦି defined(HAVE_NO_LONG_LONG)
ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uintptr_t;
# ଅନ୍ୟଥା
ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned long long uintptr_t;
# endif
#endif // VMS

    ସ୍ଥିର ଶୂନ୍ୟ
f_id(typval_T *argvars, typval_T *rettv)
{
    ଅକ୍ଷର    numbuf[NUMBUFLEN];
    ଅକ୍ଷର    *p = numbuf;

    ବାଛ (argvars[0].v_type)
    {
	ସ୍ଥିତି VAR_LIST:
	ସ୍ଥିତି VAR_TUPLE:
	ସ୍ଥିତି VAR_DICT:
	ସ୍ଥିତି VAR_OBJECT:
	ସ୍ଥିତି VAR_JOB:
	ସ୍ଥିତି VAR_CHANNEL:
	ସ୍ଥିତି VAR_BLOB:
	    // Assume pointer value in typval_T vval union at common location.
	    ଯଦି (argvars[0].vval.v_object != ଖାଲି)
	    {
		// "v" gets the address as an integer
		uintptr_t v = (uintptr_t)(ଶୂନ୍ୟ *)argvars[0].vval.v_object;
		// Build a hex string from the item's address; it is in
		// reverse order. Ignore trailing zeros.
		ପାଇଁ (; p < numbuf + ଆକାର(uintptr_t) * 2 && v != 0;
								++p, v >>= 4)
		    *p = "0123456789abcdef"[v & 0xf];
	    }
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
    *p = NUL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)numbuf);
}

/*
 * index() function for a blob
 */
    ସ୍ଥିର ଶୂନ୍ୟ
index_func_blob(typval_T *argvars, typval_T *rettv)
{
    typval_T	tv;
    blob_T	*b;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic = FALSE;

    b = argvars[0].vval.v_blob;
    ଯଦି (b == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	start = tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
	ଯଦି (error)
	    ଫେରନ୍ତୁ;
    }

    ଯଦି (start < 0)
    {
	start = blob_len(b) + start;
	ଯଦି (start < 0)
	    start = 0;
    }

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = start; idx < blob_len(b); ++idx)
    {
	tv.v_type = VAR_ସଂଖ୍ୟା;
	tv.vval.v_ସଂଖ୍ୟା = blob_get(b, idx);
	ଯଦି (tv_equal(&tv, &argvars[1], ic))
	{
	    rettv->vval.v_ସଂଖ୍ୟା = idx;
	    ଫେରନ୍ତୁ;
	}
    }
}

/*
 * index() function for a tuple
 */
    ସ୍ଥିର ଶୂନ୍ୟ
index_func_tuple(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ଯଦି (tuple == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	start_idx = 0;
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	start_idx = tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
	ଯଦି (!error && argvars[3].v_type != VAR_UNKNOWN)
	    ic = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[3], &error);
	ଯଦି (error)
	    ଫେରନ୍ତୁ;
    }

    rettv->vval.v_ସଂଖ୍ୟା = index_tuple(tuple, &argvars[1], start_idx, ic);
}

/*
 * index() function for a list
 */
    ସ୍ଥିର ଶୂନ୍ୟ
index_func_list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*item;
    long	idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    l = argvars[0].vval.v_list;
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ;

    CHECK_LIST_MATERIALIZE(l);
    item = l->lv_first;
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error));
	idx = l->lv_u.mat.lv_idx;
	ଯଦି (argvars[3].v_type != VAR_UNKNOWN)
	    ic = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[3], &error);
	ଯଦି (error)
	    item = ଖାଲି;
    }

    ପାଇଁ ( ; item != ଖାଲି; item = item->li_next, ++idx)
	ଯଦି (tv_equal(&item->li_tv, &argvars[1], ic))
	{
	    rettv->vval.v_ସଂଖ୍ୟା = idx;
	    ଭାଙ୍ଗ;
	}
}

/*
 * "index()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_index(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script()
	    && (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL)
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_BLOB)
	index_func_blob(argvars, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_TUPLE)
	index_func_tuple(argvars, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_LIST)
	index_func_list(argvars, rettv);
    ଅନ୍ୟଥା
	emsg(_(e_list_or_blob_required));
}

/*
 * Evaluate 'expr' with the v:key and v:val arguments and return the result.
 * The expression is expected to return a boolean value.  The caller should set
 * the VV_KEY and VV_VAL vim variables before calling this function.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
indexof_eval_ପଦସମଷ୍ଟି(typval_T *ପଦସମଷ୍ଟି)
{
    typval_T	argv[3];
    typval_T	newtv;
    varnumber_T	found;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    argv[0] = *get_vim_var_tv(VV_ଚାବି);
    argv[1] = *get_vim_var_tv(VV_VAL);
    newtv.v_type = VAR_UNKNOWN;

    ଯଦି (eval_ପଦସମଷ୍ଟି_typval(ପଦସମଷ୍ଟି, FALSE, argv, 2, ଖାଲି, &newtv) == FAIL)
	ଫେରନ୍ତୁ FALSE;

    found = tv_get_bool_chk(&newtv, &error);
    clear_tv(&newtv);

    ଫେରନ୍ତୁ error ? FALSE : found;
}

/*
 * Evaluate 'expr' for each byte in the Blob 'b' starting with the byte at
 * 'startidx' and return the index of the byte where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the bytes.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
indexof_blob(blob_T *b, long startidx, typval_T *ପଦସମଷ୍ଟି)
{
    long	idx = 0;

    ଯଦି (b == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    ଯଦି (startidx < 0)
    {
	// negative index: index from the last byte
	startidx = blob_len(b) + startidx;
	ଯଦି (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_ଚାବି, VAR_ସଂଖ୍ୟା);
    set_vim_var_type(VV_VAL, VAR_ସଂଖ୍ୟା);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_start = called_emsg;
    ପାଇଁ (idx = startidx; idx < blob_len(b); ++idx)
    {
	set_vim_var_nr(VV_ଚାବି, idx);
	set_vim_var_nr(VV_VAL, blob_get(b, idx));

	ଯଦି (indexof_eval_ପଦସମଷ୍ଟି(ପଦସମଷ୍ଟି))
	    ଫେରନ୍ତୁ idx;

	ଯଦି (called_emsg != called_emsg_start)
	    ଫେରନ୍ତୁ -1;
    }

    ଫେରନ୍ତୁ -1;
}

/*
 * Evaluate 'expr' for each item in the List 'l' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
indexof_list(list_T *l, long startidx, typval_T *ପଦସମଷ୍ଟି)
{
    listitem_T	*item;
    long	idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found;

    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    CHECK_LIST_MATERIALIZE(l);

    ଯଦି (startidx == 0)
	item = l->lv_first;
    ଅନ୍ୟଥା
    {
	// Start at specified item.  Use the cached index that list_find()
	// sets, so that a negative number also works.
	item = list_find(l, startidx);
	ଯଦି (item != ଖାଲି)
	    idx = l->lv_u.mat.lv_idx;
    }

    set_vim_var_type(VV_ଚାବି, VAR_ସଂଖ୍ୟା);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_start = called_emsg;
    ପାଇଁ ( ; item != ଖାଲି; item = item->li_next, ++idx)
    {
	set_vim_var_nr(VV_ଚାବି, idx);
	copy_tv(&item->li_tv, get_vim_var_tv(VV_VAL));

	found = indexof_eval_ପଦସମଷ୍ଟି(ପଦସମଷ୍ଟି);
	clear_tv(get_vim_var_tv(VV_VAL));

	ଯଦି (found)
	    ଫେରନ୍ତୁ idx;

	ଯଦି (called_emsg != called_emsg_start)
	    ଫେରନ୍ତୁ -1;
    }

    ଫେରନ୍ତୁ -1;
}

/*
 * "indexof()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_indexof(typval_T *argvars, typval_T *rettv)
{
    long	startidx = 0;
    typval_T	save_val;
    typval_T	save_ଚାବି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_did_emsg;

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (check_for_list_or_tuple_or_blob_arg(argvars, 0) == FAIL
	    || check_for_string_or_func_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି ((argvars[1].v_type == VAR_STRING &&
		(argvars[1].vval.v_string == ଖାଲି
		 || *argvars[1].vval.v_string == NUL))
	    || (argvars[1].v_type == VAR_FUNC
		&& argvars[1].vval.v_partial == ଖାଲି))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[2].v_type == VAR_DICT)
	startidx = dict_get_ସଂଖ୍ୟା_def(argvars[2].vval.v_dict, "startidx", 0);

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_ଚାବି, &save_ଚାବି);

    // We reset "did_emsg" to be able to detect whether an error occurred
    // during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    ଯଦି (argvars[0].v_type == VAR_BLOB)
	rettv->vval.v_ସଂଖ୍ୟା = indexof_blob(argvars[0].vval.v_blob, startidx,
								&argvars[1]);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_TUPLE)
	rettv->vval.v_ସଂଖ୍ୟା = indexof_tuple(argvars[0].vval.v_tuple, startidx,
								&argvars[1]);
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = indexof_list(argvars[0].vval.v_list, startidx,
								&argvars[1]);

    restore_vimvar(VV_ଚାବି, &save_ଚାବି);
    restore_vimvar(VV_VAL, &save_val);
    did_emsg |= save_did_emsg;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା inputsecret_flag = 0;

/*
 * "input()" function
 *     Also handles inputsecret() when inputsecret is set.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_ନିବେଶ(typval_T *argvars, typval_T *rettv)
{
    get_user_ନିବେଶ(argvars, rettv, FALSE, inputsecret_flag);
}

/*
 * "inputdialog()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_inputdialog(typval_T *argvars, typval_T *rettv)
{
#ଯଦି defined(FEAT_GUI_TEXTDIALOG)
    // Use a GUI dialog if the GUI is running and 'c' is not in 'guioptions'
    ଯଦି (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == ଖାଲି)
    {
	char_u	*message;
	char_u	buf[NUMBUFLEN];
	char_u	*defstr = (char_u *)"";

	ଯଦି (in_vim9script()
		&& (check_for_string_arg(argvars, 0) == FAIL
		    || check_for_opt_string_arg(argvars, 1) == FAIL
		    || (argvars[1].v_type != VAR_UNKNOWN
			&& check_for_opt_string_arg(argvars, 2) == FAIL)))
	    ଫେରନ୍ତୁ;

	message = tv_get_string_chk(&argvars[0]);
	ଯଦି (argvars[1].v_type != VAR_UNKNOWN
		&& (defstr = tv_get_string_buf_chk(&argvars[1], buf)) != ଖାଲି)
	    vim_strncpy(IObuff, defstr, IOSIZE - 1);
	ଅନ୍ୟଥା
	    IObuff[0] = NUL;
	ଯଦି (message != ଖାଲି && defstr != ଖାଲି
		&& do_dialog(VIM_QUESTION, ଖାଲି, message,
			  (char_u *)_("&OK\n&Cancel"), 1, IObuff, FALSE) == 1)
	    rettv->vval.v_string = vim_strsave(IObuff);
	ଅନ୍ୟଥା
	{
	    ଯଦି (message != ଖାଲି && defstr != ଖାଲି
					&& argvars[1].v_type != VAR_UNKNOWN
					&& argvars[2].v_type != VAR_UNKNOWN)
		rettv->vval.v_string = vim_strsave(
				      tv_get_string_buf(&argvars[2], buf));
	    ଅନ୍ୟଥା
		rettv->vval.v_string = ଖାଲି;
	}
	rettv->v_type = VAR_STRING;
    }
    ଅନ୍ୟଥା
#endif
	get_user_ନିବେଶ(argvars, rettv, TRUE, inputsecret_flag);
}

/*
 * "inputlist()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_inputlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		selected;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mouse_used;

#ifdef NO_CONSOLE_ନିବେଶ
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    ଯଦି (no_console_ନିବେଶ() && !is_not_a_term())
	ଫେରନ୍ତୁ;
#endif
    ଯଦି (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == ଖାଲି)
    {
	semsg(_(e_argument_of_str_must_be_list), "inputlist()");
	ଫେରନ୍ତୁ;
    }

    ସନ୍ଦେଶ_start();
    ସନ୍ଦେଶ_row = Rows - 1;	// for when 'cmdheight' > 1
    ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = Rows;	// avoid more prompt
    ସନ୍ଦେଶ_scroll = TRUE;
    ସନ୍ଦେଶ_clr_eos();

    l = argvars[0].vval.v_list;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)tv_get_string(&li->li_tv));
	ସନ୍ଦେଶ_putchar('\n');
    }

    // Ask for choice.
    selected = prompt_for_ସଂଖ୍ୟା(&mouse_used);
    ଯଦି (mouse_used)
	selected -= ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ;

    rettv->vval.v_ସଂଖ୍ୟା = selected;
}

ସ୍ଥିର garray_T	    ga_userinput = {0, 0, ଆକାର(tasave_T), 4, ଖାଲି};

/*
 * "inputrestore()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_inputrestore(typval_T *argvars UNUSED, typval_T *rettv)
{
    ଯଦି (ga_userinput.ga_len > 0)
    {
	--ga_userinput.ga_len;
	restore_typeahead((tasave_T *)(ga_userinput.ga_data)
						  + ga_userinput.ga_len, TRUE);
	// default return is zero == OK
    }
    ଅନ୍ୟଥା ଯଦି (p_verbose > 1)
    {
	verb_ସନ୍ଦେଶ(_("called inputrestore() more often than inputsave()"));
	rettv->vval.v_ସଂଖ୍ୟା = 1; // Failed
    }
}

/*
 * "inputsave()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_inputsave(typval_T *argvars UNUSED, typval_T *rettv)
{
    // Add an entry to the stack of typeahead storage.
    ଯଦି (ga_grow(&ga_userinput, 1) == OK)
    {
	save_typeahead((tasave_T *)(ga_userinput.ga_data)
						       + ga_userinput.ga_len);
	++ga_userinput.ga_len;
	// default return is zero == OK
    }
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = 1; // Failed
}

/*
 * "inputsecret()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_inputsecret(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ++cmdline_star;
    ++inputsecret_flag;
    f_ନିବେଶ(argvars, rettv);
    --cmdline_star;
    --inputsecret_flag;
}

/*
 * "interrupt()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_interrupt(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    got_int = TRUE;
}

/*
 * "invert(expr)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_invert(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = ~tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି);
}

/*
 * Free resources in lval_root allocated by fill_exec_lval_root().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_lval_ମୂଳ(lval_ମୂଳ_T *ମୂଳ)
{
    ଯଦି (ମୂଳ->lr_tv != ଖାଲି)
	free_tv(ମୂଳ->lr_tv);
    class_unref(ମୂଳ->lr_cl_exec);
    ମୂଳ->lr_tv = ଖାଲି;
    ମୂଳ->lr_cl_exec = ଖାଲି;
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

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fix_variable_reference_lval_ମୂଳ(lval_ମୂଳ_T *ମୂଳ, char_u *name)
{

    // Check if lr_tv is the name of an object/class reference: name start with
    // "this" or name is class variable. Clear lr_tv if neither.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା found_member = FALSE;
    ଯଦି (ମୂଳ->lr_tv->v_type == VAR_OBJECT)
    {
	ଯଦି (STRNCMP("this.", name, 5) == 0 ||STRCMP("this", name) == 0)
	    found_member = TRUE;
    }
    ଯଦି (!found_member)	// not object member, try class member
    {
	// Explicitly check if the name is a class member.
	// If it's not then do nothing.
	char_u	*end;
	ପାଇଁ (end = name; ASCII_ISALNUM(*end) || *end == '_'; ++end)
	    ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = class_member_idx(ମୂଳ->lr_cl_exec, name, end - name);
	ଯଦି (idx >= 0)
	{
	    // A class variable, replace lr_tv with it
	    clear_tv(ମୂଳ->lr_tv);
	    copy_tv(&ମୂଳ->lr_cl_exec->class_members_tv[idx], ମୂଳ->lr_tv);
	    found_member = TRUE;
	}
    }
    ଯଦି (!found_member)
    {
	free_tv(ମୂଳ->lr_tv);
	ମୂଳ->lr_tv = ଖାଲି;	    // Not a member variable
    }
    // If FAIL, then must free_lval_root(root);
    ଫେରନ୍ତୁ OK;
}

/*
 * "islocked()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_islocked(typval_T *argvars, typval_T *rettv)
{
    lval_T	lv;
    char_u	*end;
    dictitem_T	*di;

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    char_u *name = tv_get_string(&argvars[0]);
#ifdef LOG_LOCKVAR
    ch_log(ଖାଲି, "LKVAR: f_islocked(): name: %s", name);
#endif

    lval_ମୂଳ_T	aroot;	// fully initialized in fill_exec_lval_root
    lval_ମୂଳ_T *ମୂଳ = ଖାଲି;

    // Set up lval_root if executing in a method.
    ଯଦି (fill_exec_lval_ମୂଳ(&aroot) == OK)
    {
	// Almost always produces a valid lval_root since lr_cl_exec is used
	// for access verification, lr_tv may be set to NULL.
	ଯଦି (fix_variable_reference_lval_ମୂଳ(&aroot, name) == OK)
	    ମୂଳ = &aroot;
    }

    lval_ମୂଳ_T	*lval_ମୂଳ_save = lval_ମୂଳ;
    lval_ମୂଳ = ମୂଳ;
    end = get_lval(name, ଖାଲି, &lv, FALSE, FALSE,
			     GLV_NO_AUTOLOAD | GLV_READ_ONLY | GLV_NO_DECL,
			     FNE_CHECK_START);
    lval_ମୂଳ = lval_ମୂଳ_save;

    ଯଦି (end != ଖାଲି && lv.ll_name != ଖାଲି)
    {
	ଯଦି (*end != NUL)
	{
	    semsg(_(lv.ll_name == lv.ll_name_end
		   ? e_invalid_argument_str : e_trailing_characters_str), end);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (lv.ll_tv == ଖାଲି)
	    {
		di = find_var(lv.ll_name, ଖାଲି, TRUE);
		ଯଦି (di != ଖାଲି)
		{
		    // Consider a variable locked when:
		    // 1. the variable itself is locked
		    // 2. the value of the variable is locked.
		    // 3. the List or Dict value is locked.
		    rettv->vval.v_ସଂଖ୍ୟା = ((di->di_flags & DI_FLAGS_LOCK)
						   || tv_islocked(&di->di_tv));
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (lv.ll_is_ମୂଳ)
	    {
		rettv->vval.v_ସଂଖ୍ୟା = tv_islocked(lv.ll_tv);
	    }
	    ଅନ୍ୟଥା ଯଦି (lv.ll_object != ଖାଲି)
	    {
		typval_T *tv = ((typval_T *)(lv.ll_object + 1)) + lv.ll_oi;
		rettv->vval.v_ସଂଖ୍ୟା = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(ଖାଲି, "LKVAR: f_islocked(): name %s (obj)", lv.ll_name);
#endif
	    }
	    ଅନ୍ୟଥା ଯଦି (lv.ll_class != ଖାଲି)
	    {
		typval_T *tv = &lv.ll_class->class_members_tv[lv.ll_oi];
		rettv->vval.v_ସଂଖ୍ୟା = tv_islocked(tv);
#ifdef LOG_LOCKVAR
		ch_log(ଖାଲି, "LKVAR: f_islocked(): name %s (cl)", lv.ll_name);
#endif
	    }
	    ଅନ୍ୟଥା ଯଦି (lv.ll_range)
		emsg(_(e_range_not_allowed));
	    ଅନ୍ୟଥା ଯଦି (lv.ll_newkey != ଖାଲି)
		semsg(_(e_ଚାବି_not_present_in_dictionary_str), lv.ll_newkey);
	    ଅନ୍ୟଥା ଯଦି (lv.ll_list != ଖାଲି)
		// List item.
		rettv->vval.v_ସଂଖ୍ୟା = tv_islocked(&lv.ll_li->li_tv);
	    ଅନ୍ୟଥା ଯଦି (lv.ll_tuple != ଖାଲି)
		// Tuple item.
		rettv->vval.v_ସଂଖ୍ୟା = tv_islocked(lv.ll_tv);
	    ଅନ୍ୟଥା
		// Dictionary item.
		rettv->vval.v_ସଂଖ୍ୟା = tv_islocked(&lv.ll_di->di_tv);
	}
    }

    ଯଦି (ମୂଳ != ଖାଲି)
	free_lval_ମୂଳ(ମୂଳ);
    clear_lval(&lv);
}

/*
 * "items()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_ବସ୍ତୁଗୁଡ଼ିକ(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (argvars[0].v_type == VAR_STRING)
	string2items(argvars, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_LIST)
	list2items(argvars, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_TUPLE)
	tuple2items(argvars, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_BLOB)
	blob2items(argvars, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_DICT)
	dict2items(argvars, rettv);
    ଅନ୍ୟଥା
	semsg(_(e_list_tuple_dict_blob_or_string_required_for_argument_nr), 1);
}

/*
 * "keytrans()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_keytrans(typval_T *argvars, typval_T *rettv)
{
    char_u *escaped;

    rettv->v_type = VAR_STRING;
    ଯଦି (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == ଖାଲି)
	ଫେରନ୍ତୁ;
    // Need to escape K_SPECIAL and CSI for mb_unescape().
    escaped = vim_strsave_escape_csi(argvars[0].vval.v_string);
    rettv->vval.v_string = str2special_save(escaped, TRUE, TRUE);
    vim_free(escaped);
}

/*
 * "last_buffer_nr()" function.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_last_buffer_nr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = 0;
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ଯଦି (n < buf->b_fnum)
	    n = buf->b_fnum;

    rettv->vval.v_ସଂଖ୍ୟା = n;
}

/*
 * "len()" function
 */
    ଶୂନ୍ୟ
f_len(typval_T *argvars, typval_T *rettv)
{
    ବାଛ (argvars[0].v_type)
    {
	ସ୍ଥିତି VAR_STRING:
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	    rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)STRLEN(
					       tv_get_string(&argvars[0]));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BLOB:
	    rettv->vval.v_ସଂଖ୍ୟା = blob_len(argvars[0].vval.v_blob);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    rettv->vval.v_ସଂଖ୍ୟା = list_len(argvars[0].vval.v_list);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    rettv->vval.v_ସଂଖ୍ୟା = tuple_len(argvars[0].vval.v_tuple);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    rettv->vval.v_ସଂଖ୍ୟା = dict_len(argvars[0].vval.v_dict);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_OBJECT:
	    rettv->vval.v_ସଂଖ୍ୟା = object_len(argvars[0].vval.v_object);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	ସ୍ଥିତି VAR_FLOAT:
	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL:
	ସ୍ଥିତି VAR_JOB:
	ସ୍ଥିତି VAR_CHANNEL:
	ସ୍ଥିତି VAR_INSTR:
	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_TYPEALIAS:
	    emsg(_(e_invalid_type_for_len));
	    ଭାଙ୍ଗ;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
libcall_common(typval_T *argvars UNUSED, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
#ifdef FEAT_LIBCALL
    char_u		*string_in;
    char_u		**string_result;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			nr_result;
#endif

    rettv->v_type = type;
    ଯଦି (type != VAR_ସଂଖ୍ୟା)
	rettv->vval.v_string = ଖାଲି;

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_or_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

#ifdef FEAT_LIBCALL
    // The first two args must be strings, otherwise it's meaningless
    ଯଦି (argvars[0].v_type == VAR_STRING && argvars[1].v_type == VAR_STRING)
    {
	string_in = ଖାଲି;
	ଯଦି (argvars[2].v_type == VAR_STRING)
	    string_in = argvars[2].vval.v_string;
	ଯଦି (type == VAR_ସଂଖ୍ୟା)
	{
	    string_result = ଖାଲି;
	}
	ଅନ୍ୟଥା
	{
	    rettv->vval.v_string = ଖାଲି;
	    string_result = &rettv->vval.v_string;
	}
	ଯଦି (mch_libcall(argvars[0].vval.v_string,
			     argvars[1].vval.v_string,
			     string_in,
			     argvars[2].vval.v_ସଂଖ୍ୟା,
			     string_result,
			     &nr_result) == OK
		&& type == VAR_ସଂଖ୍ୟା)
	    rettv->vval.v_ସଂଖ୍ୟା = nr_result;
    }
#endif
}

/*
 * "libcall()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_libcall(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_STRING);
}

/*
 * "libcallnr()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_libcallnr(typval_T *argvars, typval_T *rettv)
{
    libcall_common(argvars, rettv, VAR_ସଂଖ୍ୟା);
}

/*
 * "line(string, [winid])" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_ଧାଡ଼ି(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum = 0;
    pos_T	*fp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	// use window specified in the second argument
	id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[1]);
	wp = win_id2wp_tp(id, &tp);
	ଯଦି (wp != ଖାଲି && tp != ଖାଲି)
	{
	    ଯଦି (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
	    {
		// With 'splitkeep' != cursor and in diff mode, prevent that the
		// window scrolls and keep the topline.
		ଯଦି (*p_spk != 'c'
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
    ଅନ୍ୟଥା
	// use current window
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);

    ଯଦି (fp != ଖାଲି)
	lnum = fp->lnum;
    rettv->vval.v_ସଂଖ୍ୟା = lnum;
}

/*
 * "line2byte(lnum)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_line2byte(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifndef FEAT_BYTEOFF
    rettv->vval.v_ସଂଖ୍ୟା = -1;
#ଅନ୍ୟଥା
    linenr_T	lnum;

    ଯଦି (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    lnum = tv_get_lnum(argvars);
    ଯଦି (lnum < 1 || lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1)
	rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = ml_find_ଧାଡ଼ି_or_offset(curbuf, lnum, ଖାଲି);
    ଯଦି (rettv->vval.v_ସଂଖ୍ୟା >= 0)
	++rettv->vval.v_ସଂଖ୍ୟା;
#endif
}

#ifdef FEAT_LUA
/*
 * "luaeval()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_luaeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_luaeval(str, argvars + 1, rettv);
}
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା
{
    MATCH_END,	    // matchend()
    MATCH_MATCH,    // match()
    MATCH_STR,	    // matchstr()
    MATCH_LIST,	    // matchlist()
    MATCH_POS	    // matchstrpos()
} matchtype_T;

    ସ୍ଥିର ଶୂନ୍ୟ
find_some_match(typval_T *argvars, typval_T *rettv, matchtype_T type)
{
    char_u	*str = ଖାଲି;
    long	len = 0;
    char_u	*ପଦସମଷ୍ଟି = ଖାଲି;
    char_u	*pat;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	strbuf[NUMBUFLEN];
    char_u	*save_cpo;
    long	start = 0;
    long	nth = 1;
    colnr_T	startcol = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match = 0;
    list_T	*l = ଖାଲି;
    listitem_T	*li = ଖାଲି;
    long	idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_lock = 0;
    char_u	*tofree = ଖାଲି;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଯଦି (type == MATCH_LIST || type == MATCH_POS)
    {
	// type MATCH_LIST: return empty list when there are no matches.
	// type MATCH_POS: return ["", -1, -1, -1]
	ଯଦି (rettv_list_alloc(rettv) == FAIL)
	    ଯାଅ theend;
	ଯଦି (type == MATCH_POS
		&& (list_append_string(rettv->vval.v_list,
					    (char_u *)"", 0) == FAIL
		    || list_append_ସଂଖ୍ୟା(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_ସଂଖ୍ୟା(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL
		    || list_append_ସଂଖ୍ୟା(rettv->vval.v_list,
					    (varnumber_T)-1) == FAIL))
	{
		list_free(rettv->vval.v_list);
		rettv->vval.v_list = ଖାଲି;
		ଯାଅ theend;
	}
    }
    ଅନ୍ୟଥା ଯଦି (type == MATCH_STR)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ଖାଲି;
    }

    ଯଦି (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 3) == FAIL)))
	ଯାଅ theend;

    ଯଦି (argvars[0].v_type == VAR_LIST)
    {
	ଯଦି ((l = argvars[0].vval.v_list) == ଖାଲି)
	    ଯାଅ theend;
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    ଅନ୍ୟଥା
    {
	ପଦସମଷ୍ଟି = str = tv_get_string(&argvars[0]);
	len = (long)STRLEN(str);
    }

    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    ଯଦି (pat == ଖାଲି)
	ଯାଅ theend;

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    error = FALSE;

	start = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
	ଯଦି (error)
	    ଯାଅ theend;
	ଯଦି (l != ଖାଲି)
	{
	    li = list_find(l, start);
	    ଯଦି (li == ଖାଲି)
		ଯାଅ theend;
	    idx = l->lv_u.mat.lv_idx;	// use the cached index
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (start < 0)
		start = 0;
	    ଯଦି (start > len)
		ଯାଅ theend;
	    // When "count" argument is there ignore matches before "start",
	    // otherwise skip part of the string.  Differs when pattern is "^"
	    // or "\<".
	    ଯଦି (argvars[3].v_type != VAR_UNKNOWN)
		startcol = start;
	    ଅନ୍ୟଥା
	    {
		str += start;
		len -= start;
	    }
	}

	ଯଦି (argvars[3].v_type != VAR_UNKNOWN)
	    nth = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[3], &error);
	ଯଦି (error)
	    ଯାଅ theend;
    }

    regmatch.regprog = eval_regcomp(pat);
    ଯଦି (regmatch.regprog != ଖାଲି)
    {
	regmatch.rm_ic = p_ic;

	// Lock the list, the string() method of an object item could remove
	// the item the loop is standing on.
	ଯଦି (l != ଖାଲି)
	{
	    prev_lock = l->lv_lock;
	    ଯଦି (l->lv_lock == 0)
		l->lv_lock = VAR_LOCKED;
	}

	ପାଇଁ (;;)
	{
	    ଯଦି (l != ଖାଲି)
	    {
		ଯଦି (li == ଖାଲି)
		{
		    match = FALSE;
		    ଭାଙ୍ଗ;
		}
		vim_free(tofree);
		ପଦସମଷ୍ଟି = str = echo_string(&li->li_tv, &tofree, strbuf, 0);
		ଯଦି (str == ଖାଲି)
		    ଭାଙ୍ଗ;
	    }

	    match = vim_regexec_nl(&regmatch, str, startcol);

	    ଯଦି (match && --nth <= 0)
		ଭାଙ୍ଗ;
	    ଯଦି (l == ଖାଲି && !match)
		ଭାଙ୍ଗ;

	    // Advance to just after the match.
	    ଯଦି (l != ଖାଲି)
	    {
		li = li->li_next;
		++idx;
	    }
	    ଅନ୍ୟଥା
	    {
		startcol = (colnr_T)(regmatch.startp[0]
				    + (*mb_ptr2len)(regmatch.startp[0]) - str);
		ଯଦି (startcol > (colnr_T)len
				      || str + startcol <= regmatch.startp[0])
		{
		    match = FALSE;
		    ଭାଙ୍ଗ;
		}
	    }
	}

	ଯଦି (match)
	{
	    ଯଦି (type == MATCH_POS)
	    {
		listitem_T *li1 = rettv->vval.v_list->lv_first;
		listitem_T *li2 = li1->li_next;
		listitem_T *li3 = li2->li_next;
		listitem_T *li4 = li3->li_next;

		vim_free(li1->li_tv.vval.v_string);
		li1->li_tv.vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
		li3->li_tv.vval.v_ସଂଖ୍ୟା =
				      (varnumber_T)(regmatch.startp[0] - ପଦସମଷ୍ଟି);
		li4->li_tv.vval.v_ସଂଖ୍ୟା =
					(varnumber_T)(regmatch.endp[0] - ପଦସମଷ୍ଟି);
		ଯଦି (l != ଖାଲି)
		    li2->li_tv.vval.v_ସଂଖ୍ୟା = (varnumber_T)idx;
	    }
	    ଅନ୍ୟଥା ଯଦି (type == MATCH_LIST)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

		// return list with matched string and submatches
		ପାଇଁ (i = 0; i < NSUBEXP; ++i)
		{
		    ଯଦି (regmatch.endp[i] == ଖାଲି)
		    {
			ଯଦି (list_append_string(rettv->vval.v_list,
							    ଖାଲି, 0) == FAIL)
			    ଭାଙ୍ଗ;
		    }
		    ଅନ୍ୟଥା ଯଦି (list_append_string(rettv->vval.v_list,
				regmatch.startp[i],
				(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(regmatch.endp[i] - regmatch.startp[i]))
			    == FAIL)
			ଭାଙ୍ଗ;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (type == MATCH_STR)
	    {
		// return matched string
		ଯଦି (l != ଖାଲି)
		    copy_tv(&li->li_tv, rettv);
		ଅନ୍ୟଥା
		    rettv->vval.v_string = vim_strnsave(regmatch.startp[0],
					regmatch.endp[0] - regmatch.startp[0]);
	    }
	    ଅନ୍ୟଥା ଯଦି (l != ଖାଲି)
		rettv->vval.v_ସଂଖ୍ୟା = idx;
	    ଅନ୍ୟଥା
	    {
		ଯଦି (type != MATCH_END)
		    rettv->vval.v_ସଂଖ୍ୟା =
				      (varnumber_T)(regmatch.startp[0] - str);
		ଅନ୍ୟଥା
		    rettv->vval.v_ସଂଖ୍ୟା =
					(varnumber_T)(regmatch.endp[0] - str);
		rettv->vval.v_ସଂଖ୍ୟା += (varnumber_T)(str - ପଦସମଷ୍ଟି);
	    }
	}
	ଯଦି (l != ଖାଲି)
	    l->lv_lock = prev_lock;
	eval_regfree(pat, regmatch.regprog);
    }

theend:
    ଯଦି (type == MATCH_POS && l == ଖାଲି && rettv->vval.v_list != ଖାଲି)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_matches_in_str(
    char_u	*str,
    regmatch_T	*rmp,
    list_T	*mlist,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		submatches,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		matchbuf)
{
    long	len = (long)STRLEN(str);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match = 0;
    colnr_T	startidx = 0;

    ପାଇଁ (;;)
    {
	match = vim_regexec_nl(rmp, str, startidx);
	ଯଦି (!match)
	    ଭାଙ୍ଗ;

	dict_T *d = dict_alloc();
	ଯଦି (d == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (list_append_dict(mlist, d) == FAIL)
	{
	    dict_unref(d);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (dict_add_ସଂଖ୍ୟା(d, matchbuf ? "lnum" : "idx", idx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (dict_add_ସଂଖ୍ୟା(d, "byteidx",
		    (colnr_T)(rmp->startp[0] - str)) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (dict_add_string_len(d, "text", rmp->startp[0],
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rmp->endp[0] - rmp->startp[0])) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (submatches)
	{
	    list_T *sml = list_alloc();
	    ଯଦି (sml == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;

	    ଯଦି (dict_add_list(d, "submatches", sml) == FAIL)
	    {
		list_unref(sml);
		ଫେରନ୍ତୁ FAIL;
	    }

	    // return a list with the submatches
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < NSUBEXP; ++i)
	    {
		ଯଦି (rmp->endp[i] == ଖାଲି)
		{
		    ଯଦି (list_append_string(sml, (char_u *)"", 0) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		}
		ଅନ୍ୟଥା ଯଦି (list_append_string(sml, rmp->startp[i],
			    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rmp->endp[i] - rmp->startp[i])) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	}
	startidx = (colnr_T)(rmp->endp[0] - str);
	ଯଦି (startidx >= (colnr_T)len || str + startidx <= rmp->startp[0])
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * "matchbufline()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_matchbufline(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ଖାଲି;
    char_u	*save_cpo;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଯଦି (rettv_list_alloc(rettv) != OK)
	ଫେରନ୍ତୁ;
    retlist = rettv->vval.v_list;

    ଯଦି (check_for_buffer_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_lnum_arg(argvars, 2) == FAIL
	    || check_for_lnum_arg(argvars, 3) == FAIL
	    || check_for_opt_dict_arg(argvars, 4) == FAIL)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_did_emsg = did_emsg;
    buf_T *buf = tv_get_buf(&argvars[0], FALSE);
    ଯଦି (buf == ଖାଲି)
    {
	ଯଦି (did_emsg == prev_did_emsg)
	    semsg(_(e_invalid_buffer_name_str), tv_get_string(&argvars[0]));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (buf->b_ml.ml_mfp == ଖାଲି)
    {
	emsg(_(e_buffer_is_not_loaded));
	ଫେରନ୍ତୁ;
    }

    char_u *pat = tv_get_string_buf(&argvars[1], patbuf);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;
    linenr_T slnum = tv_get_lnum_buf(&argvars[2], buf);
    ଯଦି (did_emsg > did_emsg_before)
	ଫେରନ୍ତୁ;
    ଯଦି (slnum < 1)
    {
	semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "lnum");
	ଫେରନ୍ତୁ;
    }

    linenr_T elnum = tv_get_lnum_buf(&argvars[3], buf);
    ଯଦି (did_emsg > did_emsg_before)
	ଫେରନ୍ତୁ;
    ଯଦି (elnum < 1 || elnum < slnum)
    {
	semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "end_lnum");
	ଫେରନ୍ତୁ;
    }

    ଯଦି (elnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	elnum = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		submatches = FALSE;
    ଯଦି (argvars[4].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[4].vval.v_dict;
	ଯଦି (d != ଖାଲି)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    ଯଦି (di != ଖାଲି)
	    {
		ଯଦି (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "submatches");
		    ଫେରନ୍ତୁ;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    ଯଦି (regmatch.regprog == ଖାଲି)
	ଯାଅ theend;
    regmatch.rm_ic = p_ic;

    ଯେତେବେଳେ (slnum <= elnum)
    {
	char_u *str = ml_get_buf(buf, slnum, FALSE);
	ଯଦି (get_matches_in_str(str, &regmatch, retlist, slnum, submatches,
								TRUE) == FAIL)
	    ଯାଅ cleanup;
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
    ସ୍ଥିର ଶୂନ୍ୟ
f_match(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_MATCH);
}

/*
 * "matchend()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_matchend(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_END);
}

/*
 * "matchlist()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_matchlist(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_LIST);
}

/*
 * "matchstr()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_matchstr(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_STR);
}

/*
 * "matchstrlist()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_matchstrlist(typval_T *argvars, typval_T *rettv)
{
    list_T	*retlist = ଖାଲି;
    char_u	*save_cpo;
    list_T	*l = ଖାଲି;
    listitem_T	*li = ଖାଲି;
    char_u	patbuf[NUMBUFLEN];
    regmatch_T	regmatch;

    rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଯଦି (rettv_list_alloc(rettv) != OK)
	ଫେରନ୍ତୁ;
    retlist = rettv->vval.v_list;

    ଯଦି (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_string_arg(argvars, 1) == FAIL
	    || check_for_opt_dict_arg(argvars, 2) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି ((l = argvars[0].vval.v_list) == ଖାଲି)
	ଫେରନ୍ତୁ;

    char_u *pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    ଯଦି (pat == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    regmatch.regprog = eval_regcomp(pat);
    ଯଦି (regmatch.regprog == ଖାଲି)
	ଯାଅ theend;
    regmatch.rm_ic = p_ic;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		submatches = FALSE;
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[2].vval.v_dict;
	ଯଦି (d != ଖାଲି)
	{
	    dictitem_T *di = dict_find(d, (char_u *)"submatches", -1);
	    ଯଦି (di != ଖାଲି)
	    {
		ଯଦି (di->di_tv.v_type != VAR_BOOL)
		{
		    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "submatches");
		    ଯାଅ cleanup;
		}
		submatches = tv_get_bool(&di->di_tv);
	    }
	}
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0;
    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	ଯଦି (li->li_tv.v_type == VAR_STRING && li->li_tv.vval.v_string != ଖାଲି)
	{
	    char_u *str = li->li_tv.vval.v_string;
	    ଯଦି (get_matches_in_str(str, &regmatch, retlist, idx, submatches,
								FALSE) == FAIL)
		ଯାଅ cleanup;
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
    ସ୍ଥିର ଶୂନ୍ୟ
f_matchstrpos(typval_T *argvars, typval_T *rettv)
{
    find_some_match(argvars, rettv, MATCH_POS);
}

    ସ୍ଥିର ଶୂନ୍ୟ
max_min(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା domax)
{
    varnumber_T	n = 0;

    ଯଦି (in_vim9script() &&
	    check_for_list_or_tuple_or_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = 0;

    ଯଦି (argvars[0].v_type == VAR_LIST)
    {
	list_T		*l;
	listitem_T	*li;
	typval_T	*tv = ଖାଲି;

	l = argvars[0].vval.v_list;
	ଯଦି (l != ଖାଲି && l->lv_len > 0)
	{
	    ଯଦି (l->lv_first == &range_list_item)
	    {
		ଯଦି ((l->lv_u.nonmat.lv_stride > 0) ^ domax)
		    n = l->lv_u.nonmat.lv_start;
		ଅନ୍ୟଥା
		    n = l->lv_u.nonmat.lv_start + ((varnumber_T)l->lv_len - 1)
						    * l->lv_u.nonmat.lv_stride;
		rettv->vval.v_ସଂଖ୍ୟା = n;
	    }
	    ଅନ୍ୟଥା
	    {
		FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
		{
		    ଯଦି (tv == ଖାଲି)
			tv = &li->li_tv;
		    ଅନ୍ୟଥା
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା res;
			ଯଦି (typval_compare2(&li->li_tv, tv,
				domax ? ପଦସମଷ୍ଟି_GREATER : ପଦସମଷ୍ଟି_SMALLER, FALSE, &res) == FAIL)
			    ଫେରନ୍ତୁ;
			ଯଦି (res == OK)
			    tv = &li->li_tv;
		    }
		}

		ଯଦି (tv != ଖାଲି)
		    copy_tv(tv, rettv);
	    }
	}
    }
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_TUPLE)
    {
	typval_T *tv;

	tv = tuple_max_min(argvars[0].vval.v_tuple, domax);
	ଯଦି (tv != ଖାଲି)
	    copy_tv(tv, rettv);
    }
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_DICT)
    {
	dict_T		*d;
	hashitem_T	*hi;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
	typval_T	*tv = ଖାଲି;

	d = argvars[0].vval.v_dict;
	ଯଦି (d != ଖାଲି)
	{
	    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)d->dv_hashtab.ht_used;
	    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&d->dv_hashtab, hi, todo)
	    {
		ଯଦି (!HASHITEM_EMPTY(hi))
		{
		    --todo;
		    ଯଦି (tv == ଖାଲି)
			tv = &HI2DI(hi)->di_tv;
		    ଅନ୍ୟଥା
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା res;
			ଯଦି (typval_compare2(&HI2DI(hi)->di_tv, tv,
				domax ? ପଦସମଷ୍ଟି_GREATER : ପଦସମଷ୍ଟି_SMALLER, FALSE, &res) == FAIL)
			    ଫେରନ୍ତୁ;
			ଯଦି (res == OK)
			    tv = &HI2DI(hi)->di_tv;
		    }
		}
	    }
	}

	ଯଦି (tv != ଖାଲି)
	    copy_tv(tv, rettv);
    }
    ଅନ୍ୟଥା
	semsg(_(e_argument_of_str_must_be_list_or_dictionary), domax ? "max()" : "min()");
}

/*
 * "max()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_max(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, TRUE);
}

/*
 * "min()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_min(typval_T *argvars, typval_T *rettv)
{
    max_min(argvars, rettv, FALSE);
}

#ଯଦି defined(FEAT_MZSCHEME)
/*
 * "mzeval()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_mzeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_mzeval(str, rettv);
}

    ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
f_nextnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ଯଦି (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ପାଇଁ (lnum = tv_get_lnum(argvars); ; ++lnum)
    {
	ଯଦି (lnum < 0 || lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    lnum = 0;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (*skipwhite(ml_get(lnum)) != NUL)
	    ଭାଙ୍ଗ;
    }
    rettv->vval.v_ସଂଖ୍ୟା = lnum;
}


/*
 * "ngettext()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_ngettext(typval_T *argvars, typval_T *rettv)
{
#ଯଦି defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    ଅକ୍ଷର *prev = ଖାଲି;
#endif

    ଯଦି (check_for_nonempty_string_arg(argvars, 0) == FAIL
	|| check_for_nonempty_string_arg(argvars, 1) == FAIL
	|| check_for_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL
	|| check_for_opt_string_arg(argvars, 3) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_STRING;

    ଯଦି (argvars[3].v_type == VAR_STRING &&
	    argvars[3].vval.v_string != ଖାଲି &&
	    *(argvars[3].vval.v_string) != NUL)
    {
#ଯଦି defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	prev = bind_textdomain_codeset((ଅଚଳ ଅକ୍ଷର *)argvars[3].vval.v_string, (ଅକ୍ଷର *)p_enc);
#endif

#ଯଦି defined(HAVE_DNGETTEXT)
	rettv->vval.v_string = vim_strsave((char_u *)dngettext((ଅଚଳ ଅକ୍ଷର *)argvars[3].vval.v_string, (ଅଚଳ ଅକ୍ଷର *)argvars[0].vval.v_string, (ଅଚଳ ଅକ୍ଷର *)argvars[1].vval.v_string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)argvars[2].vval.v_ସଂଖ୍ୟା));
#ଅନ୍ୟଥା
	textdomain((ଅଚଳ ଅକ୍ଷର *)argvars[3].vval.v_string);
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ଅଚଳ ଅକ୍ଷର *)argvars[0].vval.v_string, (ଅଚଳ ଅକ୍ଷର *)argvars[1].vval.v_string, argvars[2].vval.v_ସଂଖ୍ୟା));
	textdomain(VIMPACKAGE);
#endif

#ଯଦି defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	ଯଦି (prev != ଖାଲି)
	    bind_textdomain_codeset((ଅଚଳ ଅକ୍ଷର *)argvars[3].vval.v_string, prev);
#endif
    }
    ଅନ୍ୟଥା
	rettv->vval.v_string = vim_strsave((char_u *)NGETTEXT((ଅଚଳ ଅକ୍ଷର *)argvars[0].vval.v_string, (ଅଚଳ ଅକ୍ଷର *)argvars[1].vval.v_string, argvars[2].vval.v_ସଂଖ୍ୟା));
}


/*
 * "nr2char()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_nr2char(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (has_mbyte)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	utf8 = 0;

	ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	    utf8 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[1], ଖାଲି);
	ଯଦି (utf8)
	    buf[utf_char2bytes((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]), buf)] = NUL;
	ଅନ୍ୟଥା
	    buf[(*mb_char2bytes)((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]), buf)] = NUL;
    }
    ଅନ୍ୟଥା
    {
	buf[0] = (char_u)tv_get_ସଂଖ୍ୟା(&argvars[0]);
	buf[1] = NUL;
    }
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "or(expr, expr)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_or(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି)
					| tv_get_ସଂଖ୍ୟା_chk(&argvars[1], ଖାଲି);
}

#ifdef FEAT_PERL
/*
 * "perleval()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_perleval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_perleval(str, rettv);
}
#endif

/*
 * "prevnonblank()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_prevnonblank(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ଯଦି (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    lnum = tv_get_lnum(argvars);
    ଯଦି (lnum < 1 || lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	lnum = 0;
    ଅନ୍ୟଥା
	ଯେତେବେଳେ (lnum >= 1 && *skipwhite(ml_get(lnum)) == NUL)
	    --lnum;
    rettv->vval.v_ସଂଖ୍ୟା = lnum;
}

// This dummy va_list is here because:
// - passing a NULL pointer doesn't work when va_list isn't a pointer
// - locally in the function results in a "used before set" warning
// - using va_start() to initialize it gives "function with fixed args" error
ସ୍ଥିର va_list	ap;

/*
 * "printf()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_printf(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_did_emsg = did_emsg;
    ଅକ୍ଷର	*fmt;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script() && check_for_string_or_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    // Get the required length, allocate the buffer and do it for real.
    did_emsg = FALSE;
    fmt = (ଅକ୍ଷର *)tv_get_string_buf(&argvars[0], buf);
    len = vim_vsnprintf_typval(ଖାଲି, 0, fmt, ap, argvars + 1);
    ଯଦି (!did_emsg)
    {
	s = alloc(len + 1);
	ଯଦି (s != ଖାଲି)
	{
	    rettv->vval.v_string = s;
	    (ଶୂନ୍ୟ)vim_vsnprintf_typval((ଅକ୍ଷର *)s, len + 1, fmt,
							      ap, argvars + 1);
	}
    }
    did_emsg |= saved_did_emsg;
}

/*
 * "pum_getpos()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_pum_getpos(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    pum_set_event_info(rettv->vval.v_dict);
}

/*
 * "pumvisible()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_pumvisible(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ଯଦି (pum_visible())
	rettv->vval.v_ସଂଖ୍ୟା = 1;
}

#ifdef FEAT_PYTHON3
/*
 * "py3eval()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_py3eval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (p_pyx == 0)
	p_pyx = 3;

    ଯଦି (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    ଅନ୍ୟଥା ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg(_(e_dictionary_required));
	ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା
    {
	locals = ଖାଲି;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_py3eval(str, locals, rettv);
}
#endif

#ifdef FEAT_PYTHON
/*
 * "pyeval()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_pyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];
    dict_T	*locals;

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && (
	    check_for_string_arg(argvars, 0) == FAIL ||
	    check_for_opt_dict_arg(argvars, 1) == FAIL ) )
	ଫେରନ୍ତୁ;

    ଯଦି (p_pyx == 0)
	p_pyx = 2;

    ଯଦି (argvars[1].v_type == VAR_DICT)
    {
	locals = argvars[1].vval.v_dict;
    }
    ଅନ୍ୟଥା ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	emsg( "Invalid argument: must be dict" );
	ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା
    {
	locals = ଖାଲି;
    }

    str = tv_get_string_buf(&argvars[0], buf);
    do_pyeval(str, locals, rettv);
}
#endif

#ଯଦି defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * "pyxeval()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_pyxeval(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

# ଯଦି defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ଯଦି (p_pyx == 2)
	f_pyeval(argvars, rettv);
    ଅନ୍ୟଥା
	f_py3eval(argvars, rettv);
# elif defined(FEAT_PYTHON)
    f_pyeval(argvars, rettv);
# elif defined(FEAT_PYTHON3)
    f_py3eval(argvars, rettv);
# endif
}
#endif

ସ୍ଥିର UINT32_T srand_seed_for_testing = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	srand_seed_for_testing_is_used = FALSE;

    ସ୍ଥିର ଶୂନ୍ୟ
f_test_srand_seed(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_UNKNOWN)
	srand_seed_for_testing_is_used = FALSE;
    ଅନ୍ୟଥା
    {
	srand_seed_for_testing = (UINT32_T)tv_get_ସଂଖ୍ୟା(&argvars[0]);
	srand_seed_for_testing_is_used = TRUE;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
init_srand(UINT32_T *x)
{
    ସଂରଚନା {
	ସଂଘ {
	    UINT32_T ସଂଖ୍ୟା;
	    char_u   bytes[ଆକାର(UINT32_T)];
	} contents;
    } buf;

    ଯଦି (srand_seed_for_testing_is_used)
    {
	*x = srand_seed_for_testing;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (mch_get_random(buf.contents.bytes, ଆକାର(buf.contents.bytes)) == OK)
    {
	*x = buf.contents.ସଂଖ୍ୟା;
	ଫେରନ୍ତୁ;
    }

    // The system's random number generator doesn't work, fall back to:
    // - randombytes_random()
    // - reltime() or time()
    // - XOR with process ID
#ଯଦି defined(FEAT_SODIUM)
    ଯଦି (crypt_sodium_init() >= 0)
	*x = crypt_sodium_randombytes_random();
    ଅନ୍ୟଥା
#endif
    {
#ଯଦି defined(FEAT_RELTIME)
	proftime_T res;
	profile_start(&res);
# ଯଦି defined(MSWIN)
	*x = (UINT32_T)res.ତଳ_Part;
# ଅନ୍ୟଥା
	*x = (UINT32_T)res.tv_fsec;
# endif
#ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
f_rand(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = ଖାଲି;
    ସ୍ଥିର UINT32_T	gx, gy, gz, gw;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	initialized = FALSE;
    listitem_T	*lx, *ly, *lz, *lw;
    UINT32_T	x = 0, y, z, w, t, result;

    ଯଦି (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_UNKNOWN)
    {
	// When no argument is given use the global seed list.
	ଯଦି (initialized == FALSE)
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
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_LIST)
    {
	l = argvars[0].vval.v_list;
	ଯଦି (l == ଖାଲି || list_len(l) != 4)
	    ଯାଅ theend;

	lx = list_find(l, 0L);
	ly = list_find(l, 1L);
	lz = list_find(l, 2L);
	lw = list_find(l, 3L);
	ଯଦି (lx == ଖାଲି || ly == ଖାଲି || lz == ଖାଲି || lw == ଖାଲି)
	    ଯାଅ theend;
	ଯଦି (lx->li_tv.v_type != VAR_ସଂଖ୍ୟା
		|| ly->li_tv.v_type != VAR_ସଂଖ୍ୟା
		|| lz->li_tv.v_type != VAR_ସଂଖ୍ୟା
		|| lw->li_tv.v_type != VAR_ସଂଖ୍ୟା)
	    ଯାଅ theend;
	x = (UINT32_T)lx->li_tv.vval.v_ସଂଖ୍ୟା;
	y = (UINT32_T)ly->li_tv.vval.v_ସଂଖ୍ୟା;
	z = (UINT32_T)lz->li_tv.vval.v_ସଂଖ୍ୟା;
	w = (UINT32_T)lw->li_tv.vval.v_ସଂଖ୍ୟା;

	SHUFFLE_XOSHIRO128STARSTAR(x, y, z, w);

	lx->li_tv.vval.v_ସଂଖ୍ୟା = (varnumber_T)x;
	ly->li_tv.vval.v_ସଂଖ୍ୟା = (varnumber_T)y;
	lz->li_tv.vval.v_ସଂଖ୍ୟା = (varnumber_T)z;
	lw->li_tv.vval.v_ସଂଖ୍ୟା = (varnumber_T)w;
    }
    ଅନ୍ୟଥା
	ଯାଅ theend;

    rettv->v_type = VAR_ସଂଖ୍ୟା;
    rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)result;
    ଫେରନ୍ତୁ;

theend:
    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[0]));
    rettv->v_type = VAR_ସଂଖ୍ୟା;
    rettv->vval.v_ସଂଖ୍ୟା = -1;
}

/*
 * "srand()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_srand(typval_T *argvars, typval_T *rettv)
{
    UINT32_T x = 0, z;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_UNKNOWN)
    {
	init_srand(&x);
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    error = FALSE;

	x = (UINT32_T)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], &error);
	ଯଦି (error)
	    ଫେରନ୍ତୁ;
    }

    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)SPLITMIX32(x, z));
}

#undef ROTL
#undef SPLITMIX32
#undef SHUFFLE_XOSHIRO128STARSTAR

/*
 * "range()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_range(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	start;
    varnumber_T	end;
    varnumber_T	stride = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    start = tv_get_ସଂଖ୍ୟା_chk(&argvars[0], &error);
    ଯଦି (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = start - 1;
	start = 0;
    }
    ଅନ୍ୟଥା
    {
	end = tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	    stride = tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
    }

    ଯଦି (error)
	ଫେରନ୍ତୁ;		// type error; errmsg already given
    ଯଦି (stride == 0)
    {
	emsg(_(e_stride_is_zero));
	ଫେରନ୍ତୁ;
    }

    // The stride is stored in "lv_stride", which is an int.
    ଯଦି (stride < INT_MIN || stride > INT_MAX)
    {
	ଅକ୍ଷର	buf[NUMBUFLEN];

	vim_snprintf(buf, ଆକାର(buf), "%lld", stride);
	semsg(_(e_val_too_large), buf);
	ଫେରନ୍ତୁ;
    }

    uvarnumber_T	len;

    ଯଦି (stride > 0 ? end < start : end > start)
    {
	// One step before the start gives an empty list, further away is
	// an error.  Subtract unsigned to avoid an overflow.
	uvarnumber_T	back = stride > 0
				  ? (uvarnumber_T)start - (uvarnumber_T)end
				  : (uvarnumber_T)end - (uvarnumber_T)start;

	ଯଦି (back > 1)
	{
	    emsg(_(e_start_past_end));
	    ଫେରନ୍ତୁ;
	}
	len = 0;
    }
    ଅନ୍ୟଥା
    {
	varnumber_T	astride = stride > 0 ? stride : -stride;
	uvarnumber_T	span = stride > 0
				  ? (uvarnumber_T)end - (uvarnumber_T)start
				  : (uvarnumber_T)start - (uvarnumber_T)end;
	uvarnumber_T	ଗଣନା_2 = span / (uvarnumber_T)astride;

	// The number of items is "count" + 1 and must fit in "lv_len".
	ଯଦି (ଗଣନା_2 >= (uvarnumber_T)INT_MAX)
	{
	    ଅକ୍ଷର	buf[NUMBUFLEN];

	    // "count + 1" can wrap around.
	    vim_snprintf(buf, ଆକାର(buf), "%llu",
			      ଗଣନା_2 < UVARNUM_MAX ? ଗଣନା_2 + 1 : UVARNUM_MAX);
	    semsg(_(e_val_too_large), buf);
	    ଫେରନ୍ତୁ;
	}
	len = ଗଣନା_2 + 1;
    }

    list_T *list = rettv->vval.v_list;

    // Create a non-materialized list.  This is much more efficient and
    // works with ":for".  If used otherwise CHECK_LIST_MATERIALIZE() must
    // be called.
    list->lv_first = &range_list_item;
    list->lv_u.nonmat.lv_start = start;
    list->lv_u.nonmat.lv_end = end;
    list->lv_u.nonmat.lv_stride = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)stride;
    list->lv_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len;
}

/*
 * Materialize "list".
 * Do not call directly, use CHECK_LIST_MATERIALIZE()
 */
    ଶୂନ୍ୟ
range_list_materialize(list_T *list)
{
    varnumber_T start = list->lv_u.nonmat.lv_start;
    varnumber_T end = list->lv_u.nonmat.lv_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		stride = list->lv_u.nonmat.lv_stride;
    varnumber_T i;

    list->lv_first = ଖାଲି;
    list->lv_u.mat.lv_last = ଖାଲି;
    list->lv_len = 0;
    list->lv_u.mat.lv_idx_item = ଖାଲି;
    ପାଇଁ (i = start; stride > 0 ? i <= end : i >= end; i += stride)
    {
	ଯଦି (list_append_ସଂଖ୍ୟା(list, i) == FAIL)
	    ଭାଙ୍ଗ;
	ଯଦି (list->lv_lock & VAR_ବସ୍ତୁଗୁଡ଼ିକ_LOCKED)
	    list->lv_u.mat.lv_last->li_tv.v_lock = VAR_LOCKED;
    }
    list->lv_lock &= ~VAR_ବସ୍ତୁଗୁଡ଼ିକ_LOCKED;
}

/*
 * "getreginfo()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_getreginfo(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regname;
    char_u	buf[NUMBUFLEN + 2];
    size_t	buflen;
    long	reglen = 0;
    dict_T	*dict;
    list_T	*list;

    ଯଦି (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    regname = getreg_get_regname(argvars);
    ଯଦି (regname == 0)
	ଫେରନ୍ତୁ;

    ଯଦି (regname == '@')
	regname = '"';

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    dict = rettv->vval.v_dict;

    list = (list_T *)get_reg_contents(regname, GREG_ପଦସମଷ୍ଟି_SRC | GREG_LIST);
    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (dict_add_list(dict, "regcontents", list) == FAIL)
	list_unref(list);

    ବାଛ (get_reg_type(regname, &reglen))
    {
	ସ୍ଥିତି MLINE:
	    buf[0] = 'V';
	    buf[1] = NUL;
	    buflen = 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି MCHAR:
	    buf[0] = 'v';
	    buf[1] = NUL;
	    buflen = 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି MBLOCK:
	    buflen = vim_snprintf_safelen((ଅକ୍ଷର *)buf, ଆକାର(buf),
		"%c%ld", Ctrl_V, reglen + 1);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    buf[0] = NUL;
	    buflen = 0;
	    ଭାଙ୍ଗ;
    }
    (ଶୂନ୍ୟ)dict_add_string_len(dict, (ଅକ୍ଷର *)"regtype", buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buflen);

    buf[0] = get_register_name(get_unname_register());
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    ଯଦି (regname == '"')
	(ଶୂନ୍ୟ)dict_add_string_len(dict, (ଅକ୍ଷର *)"points_to", buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buflen);
    ଅନ୍ୟଥା
    {
	dictitem_T	*item = dictitem_alloc((char_u *)"isunnamed");

	ଯଦି (item != ଖାଲି)
	{
	    item->di_tv.v_type = VAR_BOOL;
	    item->di_tv.vval.v_ସଂଖ୍ୟା = regname == buf[0]
						      ? VVAL_TRUE : VVAL_FALSE;
	    (ଶୂନ୍ୟ)dict_add(dict, item);
	}
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
return_register(ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname, typval_T *rettv)
{
    char_u buf[2] = {NUL, NUL};

    buf[0] = (char_u)regname;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(buf, (buf[0] == NUL) ? 0 : 1);
}

/*
 * "reg_executing()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_reg_executing(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_executing, rettv);
}

/*
 * "reg_recording()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_reg_recording(typval_T *argvars UNUSED, typval_T *rettv)
{
    return_register(reg_recording, rettv);
}

/*
 * "rename({from}, {to})" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_rename(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = vim_rename(tv_get_string(&argvars[0]),
				      tv_get_string_buf(&argvars[1], buf));
}

/*
 * Repeat the list "l" "n" times and set "rettv" to the new list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
repeat_list(list_T *l, varnumber_T n, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା slen = list_len(l);

    ଯଦି (rettv_list_alloc(rettv) == FAIL
	    || l == ଖାଲି
	    || n <= 0
	    || slen == 0)
	ଫେରନ୍ତୁ;

    ଯଦି (check_repeat_ଗଣନା(slen, n) == FAIL)
	ଫେରନ୍ତୁ;

    ଯେତେବେଳେ (n-- > 0)
	ଯଦି (list_extend(rettv->vval.v_list, l, ଖାଲି) == FAIL)
	    ଭାଙ୍ଗ;
}

/*
 * Repeat the blob "b" "n" times and set "rettv" to the new blob.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
repeat_blob(typval_T *blob_tv, varnumber_T n, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		slen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    blob_T	*blob = blob_tv->vval.v_blob;

    ଯଦି (rettv_blob_alloc(rettv) == FAIL
	    || blob == ଖାଲି
	    || n <= 0)
	ଫେରନ୍ତୁ;

    slen = blob->bv_ga.ga_len;
    ଯଦି (slen <= 0)
	ଫେରନ୍ତୁ;
    ଯଦି (check_repeat_ଗଣନା(slen, n) == FAIL)
	ଫେରନ୍ତୁ;
    len = slen * (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n;

    ଯଦି (ga_grow(&rettv->vval.v_blob->bv_ga, len) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_blob->bv_ga.ga_len = len;

    ପାଇଁ (i = 0; i < slen; ++i)
	ଯଦି (blob_get(blob, i) != 0)
	    ଭାଙ୍ଗ;

    ଯଦି (i == slen)
	// No need to copy since all bytes are already zero
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ++i)
	blob_set_range(rettv->vval.v_blob,
		(long)i * slen, ((long)i + 1) * slen - 1, blob_tv);
}

/*
 * Repeat the string "str" "n" times and set "rettv" to the new string.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
repeat_string(typval_T *str_tv, varnumber_T n, typval_T *rettv)
{
    char_u	*p;
    size_t	slen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*r;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		done;

    p = tv_get_string(str_tv);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    slen = STRLEN(p);
    ଯଦି (slen == 0 || n <= 0)
	ଫେରନ୍ତୁ;
    ଯଦି (check_repeat_ଗଣନା((varnumber_T)slen, n) == FAIL)
	ଫେରନ୍ତୁ;
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)slen * (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n;

    r = alloc((size_t)len + 1);
    ଯଦି (r == ଖାଲି)
	ଫେରନ୍ତୁ;

    mch_memmove(r, p, slen);
    done = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)slen;
    ଯେତେବେଳେ (done < len)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା copy_len = done;

	ଯଦି (copy_len > len - done)
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
    ସ୍ଥିର ଶୂନ୍ୟ
f_repeat(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;

    ଯଦି (in_vim9script()
	    && (check_for_repeat_func_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    n = tv_get_ସଂଖ୍ୟା(&argvars[1]);
    ଯଦି (argvars[0].v_type == VAR_LIST)
	repeat_list(argvars[0].vval.v_list, n, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_TUPLE)
	tuple_repeat(argvars[0].vval.v_tuple, n, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_BLOB)
	repeat_blob(&argvars[0], n, rettv);
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_ସନ୍ଧାନ_arg(typval_T *varp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flagsp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir = FORWARD;
    char_u	*flags;
    char_u	nbuf[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mask;

    ଯଦି (varp->v_type == VAR_UNKNOWN)
	ଫେରନ୍ତୁ FORWARD;

    flags = tv_get_string_buf_chk(varp, nbuf);
    ଯଦି (flags == ଖାଲି)
	ଫେରନ୍ତୁ 0;		// type error; errmsg already given
    ଯେତେବେଳେ (*flags != NUL)
    {
	ବାଛ (*flags)
	{
	    ସ୍ଥିତି 'b': dir = BACKWARD; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'w': p_ws = TRUE; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'W': p_ws = FALSE; ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  mask = 0;
		      ଯଦି (flagsp != ଖାଲି)
			  ବାଛ (*flags)
			  {
			      ସ୍ଥିତି 'c': mask = SP_START; ଭାଙ୍ଗ;
			      ସ୍ଥିତି 'e': mask = SP_END; ଭାଙ୍ଗ;
			      ସ୍ଥିତି 'm': mask = SP_RETCOUNT; ଭାଙ୍ଗ;
			      ସ୍ଥିତି 'n': mask = SP_NOMOVE; ଭାଙ୍ଗ;
			      ସ୍ଥିତି 'p': mask = SP_SUBPAT; ଭାଙ୍ଗ;
			      ସ୍ଥିତି 'r': mask = SP_REPEAT; ଭାଙ୍ଗ;
			      ସ୍ଥିତି 's': mask = SP_SETPCMARK; ଭାଙ୍ଗ;
			      ସ୍ଥିତି 'z': mask = SP_COLUMN; ଭାଙ୍ଗ;
			  }
		      ଯଦି (mask == 0)
		      {
			  semsg(_(e_invalid_argument_str), flags);
			  dir = 0;
		      }
		      ଅନ୍ୟଥା
			  *flagsp |= mask;
	}
	ଯଦି (dir == 0)
	    ଭାଙ୍ଗ;
	++flags;
    }
    ଫେରନ୍ତୁ dir;
}

/*
 * Shared by search() and searchpos() functions.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଧାନ_cmn(typval_T *argvars, pos_T *match_pos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flagsp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;
    char_u	*pat;
    size_t	patlen;
    pos_T	pos;
    pos_T	save_cursor;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_p_ws = p_ws;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = 0;	// default: FAIL
    long	lnum_stop = 0;
#ifdef FEAT_RELTIME
    long	time_limit = 0;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options = ସନ୍ଧାନ_KEEP;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		subpatnum;
    searchit_arg_T sia;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_skip = FALSE;
    pos_T	firstpos;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 3) == FAIL)))))
	ଯାଅ theend;

    pat = tv_get_string(&argvars[0]);
    dir = get_ସନ୍ଧାନ_arg(&argvars[1], flagsp);	// may set p_ws
    ଯଦି (dir == 0)
	ଯାଅ theend;
    flags = *flagsp;
    ଯଦି (flags & SP_START)
	options |= ସନ୍ଧାନ_START;
    ଯଦି (flags & SP_END)
	options |= ସନ୍ଧାନ_END;
    ଯଦି (flags & SP_COLUMN)
	options |= ସନ୍ଧାନ_COL;

    // Optional arguments: line number to stop searching, timeout and skip.
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	lnum_stop = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], ଖାଲି);
	ଯଦି (lnum_stop < 0)
	    ଯାଅ theend;
	ଯଦି (argvars[3].v_type != VAR_UNKNOWN)
	{
#ifdef FEAT_RELTIME
	    time_limit = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[3], ଖାଲି);
	    ଯଦି (time_limit < 0)
		ଯାଅ theend;
#endif
	    use_skip = eval_ପଦସମଷ୍ଟି_valid_arg(&argvars[4]);
	}
    }

    /*
     * This function does not accept SP_REPEAT and SP_RETCOUNT flags.
     * Check to make sure only those flags are set.
     * Also, Only the SP_NOMOVE or the SP_SETPCMARK flag can be set. Both
     * flags cannot be set. Check for that condition also.
     */
    ଯଦି (((flags & (SP_REPEAT | SP_RETCOUNT)) != 0)
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	ଯାଅ theend;
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
    ପାଇଁ (;;)
    {
	subpatnum = searchit(curwin, curbuf, &pos, ଖାଲି, dir, pat, patlen, 1L,
						     options, RE_ସନ୍ଧାନ, &sia);
	// finding the first match again means there is no match where {skip}
	// evaluates to zero.
	ଯଦି (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos))
	    subpatnum = FAIL;

	ଯଦି (subpatnum == FAIL || !use_skip)
	    // didn't find it or no skip argument
	    ଭାଙ୍ଗ;
	ଯଦି (firstpos.lnum == 0)
	    firstpos = pos;

	// If the skip expression matches, ignore this match.
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    do_skip;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    err;
	    pos_T   save_pos = curwin->w_cursor;

	    curwin->w_cursor = pos;
	    err = FALSE;
	    do_skip = eval_ପଦସମଷ୍ଟି_to_bool(&argvars[4], &err);
	    curwin->w_cursor = save_pos;
	    ଯଦି (err)
	    {
		// Evaluating {skip} caused an error, break here.
		subpatnum = FAIL;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (!do_skip)
		ଭାଙ୍ଗ;
	}

	// clear the start flag to avoid getting stuck here
	options &= ~ସନ୍ଧାନ_START;
    }

    ଯଦି (subpatnum != FAIL)
    {
	ଯଦି (flags & SP_SUBPAT)
	    retval = subpatnum;
	ଅନ୍ୟଥା
	    retval = pos.lnum;
	ଯଦି (flags & SP_SETPCMARK)
	    setpcmark();
	curwin->w_cursor = pos;
	ଯଦି (match_pos != ଖାଲି)
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
    ଯଦି (flags & SP_NOMOVE)
	curwin->w_cursor = save_cursor;
    ଅନ୍ୟଥା
	curwin->w_set_curswant = true;
theend:
    p_ws = save_p_ws;

    ଫେରନ୍ତୁ retval;
}

#ifdef FEAT_RUBY
/*
 * "rubyeval()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_rubyeval(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    str = tv_get_string_buf(&argvars[0], buf);
    do_rubyeval(str, rettv);
}
#endif

/*
 * "screenattr()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_screenattr(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    row = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି) - 1;
    col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], ଖାଲି) - 1;
    ଯଦି (row < 0 || row >= screen_Rows
	    || col < 0 || col >= screen_Columns)
	c = -1;
    ଅନ୍ୟଥା
	c = ScreenAttrs[ଧାଡ଼ି_Offset[row] + col];
    rettv->vval.v_ସଂଖ୍ୟା = c;
}

/*
 * "screenchar()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_screenchar(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    row = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି) - 1;
    col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], ଖାଲି) - 1;
    ଯଦି (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	c = -1;
    ଅନ୍ୟଥା
    {
	char_u buf[MB_MAXBYTES + 1];
	screen_getbytes(row, col, buf, ଖାଲି);
	c = (*mb_ptr2char)(buf);
    }
    rettv->vval.v_ସଂଖ୍ୟା = c;
}

/*
 * "screenchars()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_screenchars(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    row = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି) - 1;
    col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], ଖାଲି) - 1;
    ଯଦି (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	ଫେରନ୍ତୁ;

    char_u buf[MB_MAXBYTES + 1];
    screen_getbytes(row, col, buf, ଖାଲି);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pcc[MAX_MCO];
    ଯଦି (enc_utf8)
	c = utfc_ptr2char(buf, pcc);
    ଅନ୍ୟଥା
	c = (*mb_ptr2char)(buf);
    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)c);

    ଯଦି (enc_utf8)
	ପାଇଁ (i = 0; i < Screen_mco && pcc[i] != 0; ++i)
	    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)pcc[i]);
}

/*
 * "screencol()" function
 *
 * First column is 1 to be consistent with virtcol().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_screencol(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = screen_screencol() + 1;
}

/*
 * "screenrow()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_screenrow(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = screen_screenrow() + 1;
}

/*
 * "screenstring()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_screenstring(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    char_u	buf[MB_MAXBYTES + 1];

    rettv->vval.v_string = ଖାଲି;
    rettv->v_type = VAR_STRING;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    row = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି) - 1;
    col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], ଖାଲି) - 1;
    ଯଦି (row < 0 || row >= screen_Rows || col < 0 || col >= screen_Columns)
	ଫେରନ୍ତୁ;

    screen_getbytes(row, col, buf, ଖାଲି);
    rettv->vval.v_string = vim_strsave(buf);
}

/*
 * "search()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_ସନ୍ଧାନ(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;

    rettv->vval.v_ସଂଖ୍ୟା = ସନ୍ଧାନ_cmn(argvars, ଖାଲି, &flags);
}

/*
 * "searchdecl()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_searchdecl(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		locally = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		thisblock = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    char_u	*name;

    rettv->vval.v_ସଂଖ୍ୟା = 1;	// default: FAIL

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    name = tv_get_string_chk(&argvars[0]);
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	locally = !(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[1], &error);
	ଯଦି (!error && argvars[2].v_type != VAR_UNKNOWN)
	    thisblock = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[2], &error);
    }
    ଯଦି (!error && name != ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = find_decl(name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(name),
				     locally, thisblock, ସନ୍ଧାନ_KEEP) == FAIL;
}

/*
 * Used by searchpair() and searchpairpos()
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
searchpair_cmn(typval_T *argvars, pos_T *match_pos)
{
    char_u	*spat, *mpat, *epat;
    typval_T	*skip;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_p_ws = p_ws;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;
    char_u	nbuf1[NUMBUFLEN];
    char_u	nbuf2[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = 0;		// default: FAIL
    long	lnum_stop = 0;
    long	time_limit = 0;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_opt_string_arg(argvars, 3) == FAIL
		|| (argvars[3].v_type != VAR_UNKNOWN
		    && argvars[4].v_type != VAR_UNKNOWN
		    && (check_for_opt_ସଂଖ୍ୟା_arg(argvars, 5) == FAIL
			|| (argvars[5].v_type != VAR_UNKNOWN
			    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 6) == FAIL)))))
	ଯାଅ theend;

    // Get the three pattern arguments: start, middle, end. Will result in an
    // error if not a valid argument.
    spat = tv_get_string_chk(&argvars[0]);
    mpat = tv_get_string_buf_chk(&argvars[1], nbuf1);
    epat = tv_get_string_buf_chk(&argvars[2], nbuf2);
    ଯଦି (spat == ଖାଲି || mpat == ଖାଲି || epat == ଖାଲି)
	ଯାଅ theend;	    // type error

    // Handle the optional fourth argument: flags
    dir = get_ସନ୍ଧାନ_arg(&argvars[3], &flags); // may set p_ws
    ଯଦି (dir == 0)
	ଯାଅ theend;

    // Don't accept SP_END or SP_SUBPAT.
    // Only one of the SP_NOMOVE or SP_SETPCMARK flags can be set.
    ଯଦି ((flags & (SP_END | SP_SUBPAT)) != 0
	    || ((flags & SP_NOMOVE) && (flags & SP_SETPCMARK)))
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[3]));
	ଯାଅ theend;
    }

    // Using 'r' implies 'W', otherwise it doesn't work.
    ଯଦି (flags & SP_REPEAT)
	p_ws = FALSE;

    // Optional fifth argument: skip expression
    ଯଦି (argvars[3].v_type == VAR_UNKNOWN
	    || argvars[4].v_type == VAR_UNKNOWN)
	skip = ଖାଲି;
    ଅନ୍ୟଥା
    {
	// Type is checked later.
	skip = &argvars[4];

	ଯଦି (argvars[5].v_type != VAR_UNKNOWN)
	{
	    lnum_stop = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[5], ଖାଲି);
	    ଯଦି (lnum_stop < 0)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[5]));
		ଯାଅ theend;
	    }
#ifdef FEAT_RELTIME
	    ଯଦି (argvars[6].v_type != VAR_UNKNOWN)
	    {
		time_limit = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[6], ଖାଲି);
		ଯଦି (time_limit < 0)
		{
		    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[6]));
		    ଯାଅ theend;
		}
	    }
#endif
	}
    }

    retval = do_searchpair(spat, mpat, epat, dir, skip, flags,
					    match_pos, lnum_stop, time_limit);

theend:
    p_ws = save_p_ws;

    ଫେରନ୍ତୁ retval;
}

/*
 * "searchpair()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_searchpair(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = searchpair_cmn(argvars, ଖାଲି);
}

/*
 * "searchpairpos()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_searchpairpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = 0;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (searchpair_cmn(argvars, &match_pos) > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)col);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,	    // BACKWARD or FORWARD
    typval_T	*skip,	    // skip expression
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,	    // SP_SETPCMARK and other SP_ values
    pos_T	*match_pos,
    linenr_T	lnum_stop,  // stop at this line if not zero
    long	time_limit UNUSED) // stop after this many msec
{
    char_u	*save_cpo;
    char_u	*pat, *pat2 = ଖାଲି, *pat3 = ଖାଲି;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nest = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_skip = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options = ସନ୍ଧାନ_KEEP;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    // Make two search patterns: start/end (pat2, for in nested pairs) and
    // start/middle/end (pat3, for the top pair).
    spatlen = STRLEN(spat);
    epatlen = STRLEN(epat);
    pat2size = spatlen + epatlen + 17;
    pat2 = alloc(pat2size);
    ଯଦି (pat2 == ଖାଲି)
	ଯାଅ theend;
    pat3size = spatlen + STRLEN(mpat) + epatlen + 25;
    pat3 = alloc(pat3size);
    ଯଦି (pat3 == ଖାଲି)
	ଯାଅ theend;
    pat2len = vim_snprintf((ଅକ୍ଷର *)pat2, pat2size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)", spat, epat);
    ଯଦି (*mpat == NUL)
    {
	STRCPY(pat3, pat2);
	pat3len = pat2len;
    }
    ଅନ୍ୟଥା
	pat3len = vim_snprintf((ଅକ୍ଷର *)pat3, pat3size, "\\m\\(%s\\m\\)\\|\\(%s\\m\\)\\|\\(%s\\m\\)",
							    spat, epat, mpat);
    ଯଦି (flags & SP_START)
	options |= ସନ୍ଧାନ_START;

    ଯଦି (skip != ଖାଲି)
	use_skip = eval_ପଦସମଷ୍ଟି_valid_arg(skip);

#ifdef FEAT_RELTIME
    ଯଦି (time_limit > 0)
	init_regexp_timeout(time_limit);
#endif
    save_cursor = curwin->w_cursor;
    pos = curwin->w_cursor;
    CLEAR_POS(&firstpos);
    CLEAR_POS(&foundpos);
    pat = pat3;
    patlen = pat3len;
    ପାଇଁ (;;)
    {
	searchit_arg_T sia;

	CLEAR_FIELD(sia);
	sia.sa_stop_lnum = lnum_stop;
	n = searchit(curwin, curbuf, &pos, ଖାଲି, dir, pat, patlen, 1L,
						     options, RE_ସନ୍ଧାନ, &sia);
	ଯଦି (n == FAIL || (firstpos.lnum != 0 && EQUAL_POS(pos, firstpos)))
	    // didn't find it or found the first match again: FAIL
	    ଭାଙ୍ଗ;

	ଯଦି (firstpos.lnum == 0)
	    firstpos = pos;
	ଯଦି (EQUAL_POS(pos, foundpos))
	{
	    // Found the same position again.  Can happen with a pattern that
	    // has "\zs" at the end and searching backwards.  Advance one
	    // character and try again.
	    ଯଦି (dir == BACKWARD)
		decl(&pos);
	    ଅନ୍ୟଥା
		incl(&pos);
	}
	foundpos = pos;

	// clear the start flag to avoid getting stuck here
	options &= ~ସନ୍ଧାନ_START;

	// If the skip pattern matches, ignore this match.
	ଯଦି (use_skip)
	{
	    save_pos = curwin->w_cursor;
	    curwin->w_cursor = pos;
	    err = FALSE;
	    r = eval_ପଦସମଷ୍ଟି_to_bool(skip, &err);
	    curwin->w_cursor = save_pos;
	    ଯଦି (err)
	    {
		// Evaluating {skip} caused an error, break here.
		curwin->w_cursor = save_cursor;
		retval = -1;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (r)
		ଜାରି;
	}

	ଯଦି ((dir == BACKWARD && n == 3) || (dir == FORWARD && n == 2))
	{
	    // Found end when searching backwards or start when searching
	    // forward: nested pair.
	    ++nest;
	    pat = pat2;		// nested, don't search for middle
	}
	ଅନ୍ୟଥା
	{
	    // Found end when searching forward or start when searching
	    // backward: end of (nested) pair; or found middle in outer pair.
	    ଯଦି (--nest == 1)
		pat = pat3;	// outer level, search for middle
	}

	ଯଦି (nest == 0)
	{
	    // Found the match: return matchcount or line number.
	    ଯଦି (flags & SP_RETCOUNT)
		++retval;
	    ଅନ୍ୟଥା
		retval = pos.lnum;
	    ଯଦି (flags & SP_SETPCMARK)
		setpcmark();
	    curwin->w_cursor = pos;
	    ଯଦି (!(flags & SP_REPEAT))
		ଭାଙ୍ଗ;
	    nest = 1;	    // search for next unmatched
	}
    }

    ଯଦି (match_pos != ଖାଲି)
    {
	// Store the match cursor position
	match_pos->lnum = curwin->w_cursor.lnum;
	match_pos->col = curwin->w_cursor.col + 1;
    }

    // If 'n' flag is used or search failed: restore cursor position.
    ଯଦି ((flags & SP_NOMOVE) || retval == 0)
	curwin->w_cursor = save_cursor;

theend:
#ifdef FEAT_RELTIME
    ଯଦି (time_limit > 0)
	disable_regexp_timeout();
#endif
    vim_free(pat2);
    vim_free(pat3);
    ଯଦି (p_cpo == empty_option)
	p_cpo = save_cpo;
    ଅନ୍ୟଥା
    {
	// Darn, evaluating the {skip} expression changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	ଯଦି (*p_cpo == NUL)
	    set_option_ମୂଲ୍ୟ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * "searchpos()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_searchpos(typval_T *argvars, typval_T *rettv)
{
    pos_T	match_pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    n = ସନ୍ଧାନ_cmn(argvars, &match_pos, &flags);
    ଯଦି (n > 0)
    {
	lnum = match_pos.lnum;
	col = match_pos.col;
    }

    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)lnum);
    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)col);
    ଯଦି (flags & SP_SUBPAT)
	list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)n);
}

/*
 * Set the cursor or mark position.
 * If "charpos" is TRUE, then use the column number as a character offset.
 * Otherwise use the column number as a byte offset.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_position(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା charpos)
{
    pos_T	pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnum;
    char_u	*name;
    colnr_T	curswant = -1;

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    name = tv_get_string_chk(argvars);
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (list2fpos(&argvars[1], &pos, &fnum, &curswant, charpos) != OK)
	ଫେରନ୍ତୁ;

    ଯଦି (pos.col != MAXCOL && --pos.col < 0)
	pos.col = 0;
    ଯଦି ((name[0] == '.' && name[1] == NUL))
    {
	// set cursor; "fnum" is ignored
	curwin->w_cursor = pos;
	ଯଦି (curswant >= 0)
	{
	    curwin->w_curswant = curswant - 1;
	    curwin->w_set_curswant = false;
	}
	check_cursor();
	rettv->vval.v_ସଂଖ୍ୟା = 0;
    }
    ଅନ୍ୟଥା ଯଦି (name[0] == '\'' && name[1] != NUL && name[2] == NUL)
    {
	// set mark
	ଯଦି (setmark_pos(name[1], &pos, fnum) == OK)
	    rettv->vval.v_ସଂଖ୍ୟା = 0;
    }
    ଅନ୍ୟଥା
	emsg(_(e_invalid_argument));
}
/*
 * "setcharpos()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_setcharpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, TRUE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
f_setcharsearch(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*d;
    dictitem_T	*di;
    char_u	*csearch;

    ଯଦି (check_for_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି ((d = argvars[0].vval.v_dict) == ଖାଲି)
	ଫେରନ୍ତୁ;

    csearch = dict_get_string(d, "char", FALSE);
    ଯଦି (csearch != ଖାଲି)
    {
	ଯଦି (enc_utf8)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pcc[MAX_MCO];
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = utfc_ptr2char(csearch, pcc);

	    set_last_csearch(c, csearch, utfc_ptr2len(csearch));
	}
	ଅନ୍ୟଥା
	    set_last_csearch(PTR2CHAR(csearch),
		    csearch, mb_ptr2len(csearch));
    }

    di = dict_find(d, (char_u *)"forward", -1);
    ଯଦି (di != ଖାଲି)
	set_csearch_direction((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&di->di_tv)
		? FORWARD : BACKWARD);

    di = dict_find(d, (char_u *)"until", -1);
    ଯଦି (di != ଖାଲି)
	set_csearch_until(!!tv_get_ସଂଖ୍ୟା(&di->di_tv));
}

/*
 * "setcursorcharpos" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_setcursorcharpos(typval_T *argvars, typval_T *rettv)
{
    set_cursorpos(argvars, rettv, TRUE);
}

/*
 * "setenv()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_setenv(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u   namebuf[NUMBUFLEN];
    char_u   valbuf[NUMBUFLEN];
    char_u  *name;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    // setting an environment variable may be dangerous, e.g. you could
    // setenv GCONV_PATH=/tmp and then have iconv() unexpectedly call
    // a shell command using some shared library:
    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    name = tv_get_string_buf(&argvars[0], namebuf);
    ଯଦି (argvars[1].v_type == VAR_SPECIAL
				      && argvars[1].vval.v_ସଂଖ୍ୟା == VVAL_NULL)
	vim_unsetenv_ext(name);
    ଅନ୍ୟଥା
	vim_setenv_ext(name, tv_get_string_buf(&argvars[1], valbuf));
}

/*
 * "setfperm({fname}, {mode})" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_setfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	modebuf[NUMBUFLEN];
    char_u	*mode_str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mask;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode = 0;

    rettv->vval.v_ସଂଖ୍ୟା = 0;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    fname = tv_get_string_chk(&argvars[0]);
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ;
    mode_str = tv_get_string_buf_chk(&argvars[1], modebuf);
    ଯଦି (mode_str == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (STRLEN(mode_str) != 9)
    {
	semsg(_(e_invalid_argument_str), mode_str);
	ଫେରନ୍ତୁ;
    }

    mask = 1;
    ପାଇଁ (i = 8; i >= 0; --i)
    {
	ଯଦି (mode_str[i] != '-')
	    mode |= mask;
	mask = mask << 1;
    }
    rettv->vval.v_ସଂଖ୍ୟା = mch_setperm(fname, mode) == OK;
}

/*
 * "setpos()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_setpos(typval_T *argvars, typval_T *rettv)
{
    set_position(argvars, rettv, FALSE);
}

/*
 * Translate a register type string to the yank type and block length
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_yank_type(char_u **pp, char_u *yank_type, long *block_len)
{
    char_u *stropt = *pp;
    ବାଛ (*stropt)
    {
	ସ୍ଥିତି 'v': ସ୍ଥିତି 'c':	// character-wise selection
	    *yank_type = MCHAR;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'V': ସ୍ଥିତି 'l':	// line-wise selection
	    *yank_type = MLINE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'b': ସ୍ଥିତି Ctrl_V:	// block-wise selection
	    *yank_type = MBLOCK;
	    ଯଦି (VIM_ISDIGIT(stropt[1]))
	    {
		++stropt;
		*block_len = getdigits(&stropt) - 1;
		--stropt;
	    }
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ FAIL;
    }
    *pp = stropt;
    ଫେରନ୍ତୁ OK;
}

/*
 * "setreg()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_setreg(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regname;
    char_u	*strregname;
    char_u	*stropt;
    char_u	*strval;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		append;
    char_u	yank_type;
    long	block_len;
    typval_T	*regcontents;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pointreg;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    pointreg = 0;
    regcontents = ଖାଲି;
    block_len = -1;
    yank_type = MAUTO;
    append = FALSE;

    strregname = tv_get_string_chk(argvars);
    rettv->vval.v_ସଂଖ୍ୟା = 1;		// FAIL is default

    ଯଦି (strregname == ଖାଲି)
	ଫେରନ୍ତୁ;		// type error; errmsg already given
    ଯଦି (in_vim9script() && STRLEN(strregname) > 1)
    {
	semsg(_(e_register_name_must_be_one_char_str), strregname);
	ଫେରନ୍ତୁ;
    }
    regname = *strregname;
    ଯଦି (regname == 0 || regname == '@')
	regname = '"';

    ଯଦି (argvars[1].v_type == VAR_DICT)
    {
	dict_T	    *d = argvars[1].vval.v_dict;
	dictitem_T  *di;

	ଯଦି (d == ଖାଲି || d->dv_hashtab.ht_used == 0)
	{
	    // Empty dict, clear the register (like setreg(0, []))
	    char_u *lstval[2] = {ଖାଲି, ଖାଲି};
	    write_reg_contents_lst(regname, lstval, 0, FALSE, MAUTO, -1);
	    ଫେରନ୍ତୁ;
	}

	di = dict_find(d, (char_u *)"regcontents", -1);
	ଯଦି (di != ଖାଲି)
	    regcontents = &di->di_tv;

	stropt = dict_get_string(d, "regtype", FALSE);
	ଯଦି (stropt != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = get_yank_type(&stropt, &yank_type, &block_len);

	    ଯଦି (ret == FAIL || *++stropt != NUL)
	    {
		semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "value");
		ଫେରନ୍ତୁ;
	    }
	}

	ଯଦି (regname == '"')
	{
	    stropt = dict_get_string(d, "points_to", FALSE);
	    ଯଦି (stropt != ଖାଲି)
	    {
		pointreg = *stropt;
		regname = pointreg;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (dict_get_bool(d, "isunnamed", -1) > 0)
	    pointreg = regname;
    }
    ଅନ୍ୟଥା
	regcontents = &argvars[1];

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	ଯଦି (yank_type != MAUTO)
	{
	    semsg(_(e_too_many_arguments_for_function_str), "setreg");
	    ଫେରନ୍ତୁ;
	}

	stropt = tv_get_string_chk(&argvars[2]);
	ଯଦି (stropt == ଖାଲି)
	    ଫେରନ୍ତୁ;		// type error
	ପାଇଁ (; *stropt != NUL; ++stropt)
	    ବାଛ (*stropt)
	    {
		ସ୍ଥିତି 'a': ସ୍ଥିତି 'A':	// append
		    append = TRUE;
		    ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    get_yank_type(&stropt, &yank_type, &block_len);
	    }
    }

    ଯଦି (regcontents && regcontents->v_type == VAR_LIST)
    {
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	list_T		*ll = regcontents->vval.v_list;
	listitem_T	*li;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

	// If the list is NULL handle like an empty list.
	len = ll == ଖାଲି ? 0 : ll->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	ଯଦି (lstval == ଖାଲି)
	    ଫେରନ୍ତୁ;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	ଯଦି (ll != ଖାଲି)
	{
	    CHECK_LIST_MATERIALIZE(ll);
	    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(ll, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		ଯଦି (strval == ଖାଲି)
		    ଯାଅ free_lstval;
		ଯଦି (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    ଯଦି (strval == ଖାଲି)
			ଯାଅ free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ଖାଲି;

	write_reg_contents_lst(regname, lstval, -1,
						append, yank_type, block_len);
free_lstval:
	ଯେତେବେଳେ (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }
    ଅନ୍ୟଥା ଯଦି (regcontents)
    {
	strval = tv_get_string_chk(regcontents);
	ଯଦି (strval == ଖାଲି)
	    ଫେରନ୍ତୁ;
	write_reg_contents_ex(regname, strval, -1,
						append, yank_type, block_len);
    }
    ଯଦି (pointreg != 0)
	get_yank_register(pointreg, TRUE);

    rettv->vval.v_ସଂଖ୍ୟା = 0;
}

/*
 * "settagstack()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_settagstack(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    dict_T	*d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		action = 'r';

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    // first argument: window number or id
    wp = find_win_by_nr_or_id(&argvars[0]);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;

    // second argument: dict with items to set in the tag stack
    ଯଦି (check_for_dict_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;
    d = argvars[1].vval.v_dict;
    ଯଦି (d == ଖାଲି)
	ଫେରନ୍ତୁ;

    // third argument: action - 'a' for append and 'r' for replace.
    // default is to replace the stack.
    ଯଦି (argvars[2].v_type == VAR_UNKNOWN)
	action = 'r';
    ଅନ୍ୟଥା ଯଦି (check_for_string_arg(argvars, 2) == FAIL)
	ଫେରନ୍ତୁ;
    ଅନ୍ୟଥା
    {
	char_u	*actstr;
	actstr = tv_get_string_chk(&argvars[2]);
	ଯଦି (actstr == ଖାଲି)
	    ଫେରନ୍ତୁ;
	ଯଦି ((*actstr == 'r' || *actstr == 'a' || *actstr == 't')
		&& actstr[1] == NUL)
	    action = *actstr;
	ଅନ୍ୟଥା
	{
	    semsg(_(e_invalid_action_str_2), actstr);
	    ଫେରନ୍ତୁ;
	}
    }

    ଯଦି (set_tagstack(wp, d, action) == OK)
	rettv->vval.v_ସଂଖ୍ୟା = 0;
}

#ifdef FEAT_CRYPT
/*
 * "sha256({expr})" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_sha256(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି (in_vim9script() && check_for_string_or_blob_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (argvars[0].v_type == VAR_BLOB)
    {
	blob_T *blob = argvars[0].vval.v_blob;
	p = blob != ଖାଲି ? (char_u *)blob->bv_ga.ga_data : (char_u *)"";
	len = blob != ଖାଲି ? blob->bv_ga.ga_len : 0;
	rettv->vval.v_string = vim_strsave(sha256_bytes(p, len, ଖାଲି, 0));
    }
    ଅନ୍ୟଥା
    {
	p = tv_get_string(&argvars[0]);
	rettv->vval.v_string = vim_strsave(
				    sha256_bytes(p, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p), ଖାଲି, 0));
    }
}
#endif // FEAT_CRYPT

/*
 * "shellescape({string})" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_shellescape(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_special;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    do_special = non_zero_arg(&argvars[1]);
    rettv->vval.v_string = vim_strsave_shellescape(
			   tv_get_string(&argvars[0]), do_special, do_special);
    rettv->v_type = VAR_STRING;
}

/*
 * shiftwidth() function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_shiftwidth(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = 0;

    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	long	col;

	col = (long)tv_get_ସଂଖ୍ୟା_chk(argvars, ଖାଲି);
	ଯଦି (col < 0)
	    ଫେରନ୍ତୁ;	// type error; errmsg already given
#ifdef FEAT_VARTABS
	rettv->vval.v_ସଂଖ୍ୟା = get_sw_ମୂଲ୍ୟ_col(curbuf, col, FALSE);
	ଫେରନ୍ତୁ;
#endif
    }

    rettv->vval.v_ସଂଖ୍ୟା = get_sw_ମୂଲ୍ୟ(curbuf);
}

/*
 * "soundfold({word})" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_soundfold(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_STRING;
    s = tv_get_string(&argvars[0]);
#ifdef FEAT_SPELL
    rettv->vval.v_string = eval_soundfold(s);
#ଅନ୍ୟଥା
    rettv->vval.v_string = vim_strsave(s);
#endif
}

/*
 * "spellbadword()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_spellbadword(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*word = (char_u *)"";
    hlf_T	attr = HLF_ଗଣନା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
#ifdef FEAT_SPELL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wo_spell_save = curwin->w_p_spell;

    ଯଦି (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    ଯଦି (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	ଫେରନ୍ତୁ;
    }
#endif

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	ଫେରନ୍ତୁ;
    }

#ifdef FEAT_SPELL
    ଯଦି (argvars[0].v_type == VAR_UNKNOWN)
    {
	// Find the start and length of the badly spelled word.
	len = spell_move_to(curwin, FORWARD, SMT_ALL, TRUE, &attr);
	ଯଦି (len != 0)
	{
	    word = ml_get_cursor();
	    curwin->w_set_curswant = true;
	}
    }
    ଅନ୍ୟଥା ଯଦି (*curbuf->b_s.b_p_spl != NUL)
    {
	char_u	*str = tv_get_string_chk(&argvars[0]);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	capcol = -1;

	ଯଦି (str != ଖାଲି)
	{
	    // Check the argument for spelling.
	    ଯେତେବେଳେ (*str != NUL)
	    {
		len = spell_check(curwin, str, &attr, &capcol, FALSE);
		ଯଦି (attr != HLF_ଗଣନା)
		{
		    word = str;
		    ଭାଙ୍ଗ;
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
    ବାଛ (attr)
    {
    ସ୍ଥିତି HLF_SPB:
	list_append_string(rettv->vval.v_list, (char_u *)"bad", STRLEN_LITERAL("bad"));
	ଭାଙ୍ଗ;

    ସ୍ଥିତି HLF_SPR:
	list_append_string(rettv->vval.v_list, (char_u *)"rare", STRLEN_LITERAL("rare"));
	ଭାଙ୍ଗ;

    ସ୍ଥିତି HLF_SPL:
	list_append_string(rettv->vval.v_list, (char_u *)"local", STRLEN_LITERAL("local"));
	ଭାଙ୍ଗ;

    ସ୍ଥିତି HLF_SPC:
	list_append_string(rettv->vval.v_list, (char_u *)"caps", STRLEN_LITERAL("caps"));
	ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);
	ଭାଙ୍ଗ;
    }
}

/*
 * "spellsuggest()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_spellsuggest(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SPELL
    char_u	*str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		typeerr = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxcount;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_capital = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wo_spell_save = curwin->w_p_spell;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    ଯଦି (!curwin->w_p_spell)
    {
	parse_spelllang(curwin);
	curwin->w_p_spell = TRUE;
    }

    ଯଦି (*curwin->w_s->b_p_spl == NUL)
    {
	emsg(_(e_spell_checking_is_not_possible));
	curwin->w_p_spell = wo_spell_save;
	ଫେରନ୍ତୁ;
    }
#endif

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
    {
#ifdef FEAT_SPELL
	curwin->w_p_spell = wo_spell_save;
#endif
	ଫେରନ୍ତୁ;
    }

#ifdef FEAT_SPELL
    str = tv_get_string(&argvars[0]);
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	maxcount = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &typeerr);
	ଯଦି (maxcount <= 0)
	    ଫେରନ୍ତୁ;
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	{
	    need_capital = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[2], &typeerr);
	    ଯଦି (typeerr)
		ଫେରନ୍ତୁ;
	}
    }
    ଅନ୍ୟଥା
	maxcount = 25;

    spell_suggest_list(&ga, str, maxcount, need_capital, FALSE);

    ପାଇଁ (i = 0; i < ga.ga_len; ++i)
    {
	str = ((char_u **)ga.ga_data)[i];

	li = listitem_alloc();
	ଯଦି (li == ଖାଲି)
	    vim_free(str);
	ଅନ୍ୟଥା
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

    ସ୍ଥିର ଶୂନ୍ୟ
f_split(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    char_u	*end;
    char_u	*pat = ଖାଲି;
    regmatch_T	regmatch;
    char_u	patbuf[NUMBUFLEN];
    char_u	*save_cpo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match;
    colnr_T	col = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keepempty = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		typeerr = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    p_cpo = empty_option;

    str = tv_get_string(&argvars[0]);
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	pat = tv_get_string_buf_chk(&argvars[1], patbuf);
	ଯଦି (pat == ଖାଲି)
	    typeerr = TRUE;
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	    keepempty = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[2], &typeerr);
    }
    ଯଦି (pat == ଖାଲି || *pat == NUL)
	pat = (char_u *)"[\\x01- ]\\+";

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଯାଅ theend;
    ଯଦି (typeerr)
	ଯାଅ theend;

    regmatch.regprog = eval_regcomp(pat);
    ଯଦି (regmatch.regprog != ଖାଲି)
    {
	regmatch.rm_ic = FALSE;
	ଯେତେବେଳେ (*str != NUL || keepempty)
	{
	    ଯଦି (*str == NUL)
		match = FALSE;	// empty item at the end
	    ଅନ୍ୟଥା
		match = vim_regexec_nl(&regmatch, str, col);
	    ଯଦି (match)
		end = regmatch.startp[0];
	    ଅନ୍ୟଥା
		end = str + STRLEN(str);
	    ଯଦି (keepempty || end > str || (rettv->vval.v_list->lv_len > 0
			   && *str != NUL && match && end < regmatch.endp[0]))
	    {
		ଯଦି (list_append_string(rettv->vval.v_list, str,
						    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - str)) == FAIL)
		    ଭାଙ୍ଗ;
	    }
	    ଯଦି (!match)
		ଭାଙ୍ଗ;
	    // Advance to just after the match.
	    ଯଦି (regmatch.endp[0] > str)
		col = 0;
	    ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
f_submatch(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		no;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retList = 0;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    no = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ;
    ଯଦି (no < 0 || no >= NSUBEXP)
    {
	semsg(_(e_invalid_submatch_ସଂଖ୍ୟା_nr), no);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	retList = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[1], &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ;

    ଯଦି (retList == 0)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = reg_submatch(no);
    }
    ଅନ୍ୟଥା
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = reg_submatch_list(no);
    }
}

/*
 * "substitute()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_substitute(typval_T *argvars, typval_T *rettv)
{
    char_u	patbuf[NUMBUFLEN];
    char_u	subbuf[NUMBUFLEN];
    char_u	flagsbuf[NUMBUFLEN];
    char_u	*str;
    char_u	*pat;
    char_u	*sub = ଖାଲି;
    typval_T	*ପଦସମଷ୍ଟି = ଖାଲି;
    char_u	*flg;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	ଫେରନ୍ତୁ;

    str = tv_get_string_chk(&argvars[0]);
    pat = tv_get_string_buf_chk(&argvars[1], patbuf);
    flg = tv_get_string_buf_chk(&argvars[3], flagsbuf);

    ଯଦି (argvars[2].v_type == VAR_FUNC
	    || argvars[2].v_type == VAR_PARTIAL
	    || argvars[2].v_type == VAR_INSTR
	    || argvars[2].v_type == VAR_CLASS
	    || argvars[2].v_type == VAR_OBJECT)
	ପଦସମଷ୍ଟି = &argvars[2];
    ଅନ୍ୟଥା
	sub = tv_get_string_buf_chk(&argvars[2], subbuf);

    rettv->v_type = VAR_STRING;
    ଯଦି (str == ଖାଲି || pat == ଖାଲି || (sub == ଖାଲି && ପଦସମଷ୍ଟି == ଖାଲି)
								|| flg == ଖାଲି)
	rettv->vval.v_string = ଖାଲି;
    ଅନ୍ୟଥା
	rettv->vval.v_string = do_string_sub(str, STRLEN(str), pat, sub, ପଦସମଷ୍ଟି, flg, ଖାଲି);
}

/*
 * "swapfilelist()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_swapfilelist(typval_T *argvars UNUSED, typval_T *rettv)
{
    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    recover_names(ଖାଲି, FALSE, rettv->vval.v_list, 0, ଖାଲି);
}

/*
 * "swapinfo(swap_filename)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_swapinfo(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (rettv_dict_alloc(rettv) == OK)
	get_b0_dict(tv_get_string(argvars), rettv->vval.v_dict);
}

/*
 * "swapname(expr)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_swapname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ଯଦି (buf == ଖାଲି || buf->b_ml.ml_mfp == ଖାଲି
					|| buf->b_ml.ml_mfp->mf_fname == ଖାଲି)
	rettv->vval.v_string = ଖାଲି;
    ଅନ୍ୟଥା
	rettv->vval.v_string = vim_strsave(buf->b_ml.ml_mfp->mf_fname);
}

/*
 * "synID(lnum, col, trans)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_synID(typval_T *argvars UNUSED, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id = 0;
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trans;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		transerr = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| check_for_bool_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (linenr_T)tv_get_ସଂଖ୍ୟା(&argvars[1]) - 1;	// -1 on type error
    trans = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[2], &transerr);

    ଯଦି (!transerr && lnum >= 1 && lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
	    && col >= 0 && col < (long)ml_get_len(lnum))
	id = syn_get_id(curwin, lnum, col, trans, ଖାଲି, FALSE);
#endif

    rettv->vval.v_ସଂଖ୍ୟା = id;
}

/*
 * "synIDattr(id, what [, mode])" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_synIDattr(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*p = ଖାଲି;
#ifdef FEAT_SYN_HL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    char_u	*what;
    char_u	*mode;
    char_u	modebuf[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modec;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]);
    what = tv_get_string(&argvars[1]);
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	mode = tv_get_string_buf(&argvars[2], modebuf);
	modec = TOLOWER_ASC(mode[0]);
	ଯଦି (modec != 't' && modec != 'c' && modec != 'g')
	    modec = 0;	// replace invalid with current
    }
    ଅନ୍ୟଥା
    {
# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	ଯଦି (USE_24BIT)
	    modec = 'g';
	ଅନ୍ୟଥା
# endif
	    ଯଦି (t_colors > 1)
		modec = 'c';
	    ଅନ୍ୟଥା
		modec = 't';
    }

    ବାଛ (TOLOWER_ASC(what[0]))
    {
	ସ୍ଥିତି 'b':
		ଯଦି (TOLOWER_ASC(what[1]) == 'g')	// bg[#]
		    p = highlight_color(id, what, modec);
		ଅନ୍ୟଥା					// bold
		    p = highlight_has_attr(id, HL_BOLD, modec);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି 'f':					// fg[#] or font
		p = highlight_color(id, what, modec);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି 'i':
		ଯଦି (TOLOWER_ASC(what[1]) == 'n')	// inverse
		    p = highlight_has_attr(id, HL_INVERSE, modec);
		ଅନ୍ୟଥା					// italic
		    p = highlight_has_attr(id, HL_ITALIC, modec);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି 'n':
		ଯଦି (TOLOWER_ASC(what[1]) == 'o')	// nocombine
		    p = highlight_has_attr(id, HL_NOCOMBINE, modec);
		ଅନ୍ୟଥା					// name
		    p = get_highlight_name_ext(ଖାଲି, id - 1, FALSE);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି 'r':					// reverse
		p = highlight_has_attr(id, HL_INVERSE, modec);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି 's':
		ଯଦି (TOLOWER_ASC(what[1]) == 'p')	// sp[#]
		    p = highlight_color(id, what, modec);
							// strikeout
		ଅନ୍ୟଥା ଯଦି (TOLOWER_ASC(what[1]) == 't' &&
			TOLOWER_ASC(what[2]) == 'r')
		    p = highlight_has_attr(id, HL_STRIKETHROUGH, modec);
		ଅନ୍ୟଥା					// standout
		    p = highlight_has_attr(id, HL_STANDOUT, modec);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି 'u':
		ଯଦି (STRLEN(what) >= 9)
		{
		    ଯଦି (TOLOWER_ASC(what[5]) == 'l')
							// underline
			p = highlight_has_attr(id, HL_UNDERLINE, modec);
		    ଅନ୍ୟଥା ଯଦି (TOLOWER_ASC(what[5]) != 'd')
							// undercurl
			p = highlight_has_attr(id, HL_UNDERCURL, modec);
		    ଅନ୍ୟଥା ଯଦି (TOLOWER_ASC(what[6]) != 'o')
							// underdashed
			p = highlight_has_attr(id, HL_UNDERDASHED, modec);
		    ଅନ୍ୟଥା ଯଦି (TOLOWER_ASC(what[7]) == 'u')
							// underdouble
			p = highlight_has_attr(id, HL_UNDERDOUBLE, modec);
		    ଅନ୍ୟଥା
							// underdotted
			p = highlight_has_attr(id, HL_UNDERDOTTED, modec);
		}
		ଅନ୍ୟଥା
							// ul
		    p = highlight_color(id, what, modec);
		ଭାଙ୍ଗ;
    }

    ଯଦି (p != ଖାଲି)
	p = vim_strsave(p);
#endif
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_synIDtrans(typval_T *argvars UNUSED, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;

#ifdef FEAT_SYN_HL
    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]);

    ଯଦି (id > 0)
	id = syn_get_final_id(id);
    ଅନ୍ୟଥା
#endif
	id = 0;

    rettv->vval.v_ସଂଖ୍ୟା = id;
}

/*
 * "synconcealed(lnum, col)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_synconcealed(typval_T *argvars UNUSED, typval_T *rettv)
{
#ଯଦି defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    linenr_T	lnum;
    colnr_T	col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syntax_flags = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cchar;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		matchid = 0;
    char_u	str[NUMBUFLEN];
#endif

    rettv_list_set(rettv, ଖାଲି);

    ଯଦି (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

#ଯଦି defined(FEAT_SYN_HL) && defined(FEAT_CONCEAL)
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_ସଂଖ୍ୟା(&argvars[1]) - 1;	// -1 on type error

    CLEAR_FIELD(str);

    ଯଦି (rettv_list_alloc(rettv) == OK)
    {
	ଯଦି (lnum >= 1 && lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && curwin->w_p_cole > 0)
	{
	    (ଶୂନ୍ୟ)syn_get_id(curwin, lnum, col, FALSE, ଖାଲି, FALSE);
	    syntax_flags = get_syntax_info(&matchid);

	    // get the conceal character
	    ଯଦି ((syntax_flags & HL_CONCEAL) && curwin->w_p_cole < 3)
	    {
		cchar = syn_get_sub_char();
		ଯଦି (cchar == NUL && curwin->w_p_cole == 1)
		    cchar = (curwin->w_lcs_chars.conceal == NUL) ? ' '
					: curwin->w_lcs_chars.conceal;
		ଯଦି (cchar != NUL)
		{
		    ଯଦି (has_mbyte)
			(*mb_char2bytes)(cchar, str);
		    ଅନ୍ୟଥା
			str[0] = cchar;
		}
	    }
	}

	list_append_ସଂଖ୍ୟା(rettv->vval.v_list,
					    (syntax_flags & HL_CONCEAL) != 0);
	// -1 to auto-determine strlen
	list_append_string(rettv->vval.v_list, str, -1);
	list_append_ସଂଖ୍ୟା(rettv->vval.v_list, matchid);
    }
#endif
}

/*
 * "synstack(lnum, col)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_synstack(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_SYN_HL
    linenr_T	lnum;
    colnr_T	col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
#endif

    rettv_list_set(rettv, ଖାଲି);

    ଯଦି (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

#ifdef FEAT_SYN_HL
    lnum = tv_get_lnum(argvars);		// -1 on type error
    col = (colnr_T)tv_get_ସଂଖ୍ୟା(&argvars[1]) - 1;	// -1 on type error

    ଯଦି (lnum >= 1 && lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
	    && col >= 0 && col <= (long)ml_get_len(lnum)
	    && rettv_list_alloc(rettv) == OK)
    {
	(ଶୂନ୍ୟ)syn_get_id(curwin, lnum, col, FALSE, ଖାଲି, TRUE);
	ପାଇଁ (i = 0; ; ++i)
	{
	    id = syn_get_ଗଦା_item(i);
	    ଯଦି (id < 0)
		ଭାଙ୍ଗ;
	    ଯଦି (list_append_ସଂଖ୍ୟା(rettv->vval.v_list, id) == FAIL)
		ଭାଙ୍ଗ;
	}
    }
#endif
}

/*
 * "tabpagebuflist()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_tabpagebuflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp = ଖାଲି;

    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_UNKNOWN)
	wp = firstwin;
    ଅନ୍ୟଥା
    {
	tp = find_tabpage((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]));
	ଯଦି (tp != ଖାଲି)
	    wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
    }
    ଯଦି (wp != ଖାଲି && rettv_list_alloc(rettv) == OK)
    {
	ପାଇଁ (; wp != ଖାଲି; wp = wp->w_next)
	    ଯଦି (list_append_ସଂଖ୍ୟା(rettv->vval.v_list,
						wp->w_buffer->b_fnum) == FAIL)
		ଭାଙ୍ଗ;
    }
}

/*
 * "tagfiles()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_tagfiles(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*fname;
    tagname_T	tn;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    fname = alloc(MAXPATHL);
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (first = TRUE; ; first = FALSE)
	ଯଦି (get_tagfname(&tn, first, fname) == FAIL
		|| list_append_string(rettv->vval.v_list, fname, -1) == FAIL)
	    ଭାଙ୍ଗ;
    tagname_free(&tn);
    vim_free(fname);
}

/*
 * "taglist()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_taglist(typval_T *argvars, typval_T *rettv)
{
    char_u  *fname = ଖାଲି;
    char_u  *tag_pattern;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    tag_pattern = tv_get_string(&argvars[0]);

    rettv->vval.v_ସଂଖ୍ୟା = FALSE;
    ଯଦି (*tag_pattern == NUL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	fname = tv_get_string(&argvars[1]);
    ଯଦି (rettv_list_alloc(rettv) == OK)
	(ଶୂନ୍ୟ)get_tags(rettv->vval.v_list, tag_pattern, fname);
}

/*
 * "type(expr)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_type(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = -1;

    ବାଛ (argvars[0].v_type)
    {
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:  n = VAR_TYPE_ସଂଖ୍ୟା; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_STRING:  n = VAR_TYPE_STRING; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_PARTIAL:
	ସ୍ଥିତି VAR_FUNC:    n = VAR_TYPE_FUNC; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:    n = VAR_TYPE_LIST; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:   n = VAR_TYPE_TUPLE; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:    n = VAR_TYPE_DICT; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FLOAT:   n = VAR_TYPE_FLOAT; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BOOL:	  n = VAR_TYPE_BOOL; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_SPECIAL: n = VAR_TYPE_NONE; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_JOB:     n = VAR_TYPE_JOB; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CHANNEL: n = VAR_TYPE_CHANNEL; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BLOB:    n = VAR_TYPE_BLOB; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_INSTR:   n = VAR_TYPE_INSTR; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TYPEALIAS: n = VAR_TYPE_TYPEALIAS; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CLASS:
	    {
		class_T *cl = argvars[0].vval.v_class;
		ଯଦି (cl != ଖାଲି && IS_ENUM(cl))
		    n = VAR_TYPE_ENUM;
		ଅନ୍ୟଥା
		    n = VAR_TYPE_CLASS;
		ଭାଙ୍ଗ;
	    }
	ସ୍ଥିତି VAR_OBJECT:
	    {
		object_T *obj = argvars[0].vval.v_object;

		ଯଦି (obj == ଖାଲି)
		    n = VAR_TYPE_OBJECT;
		ଅନ୍ୟଥା
		{
		    class_T *cl = argvars[0].vval.v_object->obj_class;
		    ଯଦି (IS_ENUM(cl))
			n = VAR_TYPE_ENUMVALUE;
		    ଅନ୍ୟଥା
			n = VAR_TYPE_OBJECT;
		}
		ଭାଙ୍ଗ;
	    }
	ସ୍ଥିତି VAR_VOID:
	    emsg(_(e_cannot_use_void_ମୂଲ୍ୟ));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	    internal_error_no_abort("f_type(UNKNOWN)");
	    n = -1;
	    ଭାଙ୍ଗ;
    }
    rettv->vval.v_ସଂଖ୍ୟା = n;
}

/*
 * "virtcol({expr}, [, {list} [, {winid}]])" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_virtcol(typval_T *argvars, typval_T *rettv)
{
    colnr_T	vcol_start = 0;
    colnr_T	vcol_end = 0;
    pos_T	*fp;
    switchwin_T	switchwin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		winchanged = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL))))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN)
    {
	tabpage_T	*tp;
	win_T		*wp;

	// use the window specified in the third argument
	wp = win_id2wp_tp((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[2]), &tp);
	ଯଦି (wp == ଖାଲି || tp == ଖାଲି)
	    ଯାଅ theend;

	ଯଦି (switch_win_noblock(&switchwin, wp, tp, TRUE) != OK)
	    ଯାଅ theend;

	check_cursor();
	winchanged = TRUE;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fnum = curbuf->b_fnum;
    fp = var2fpos(&argvars[0], FALSE, &fnum, FALSE);
    ଯଦି (fp != ଖାଲି && fp->lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
	    && fnum == curbuf->b_fnum)
    {
	// Limit the column to a valid value, getvvcol() doesn't check.
	ଯଦି (fp->col < 0)
	    fp->col = 0;
	ଅନ୍ୟଥା
	{
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ml_get_len(fp->lnum);
	    ଯଦି (fp->col > len)
		fp->col = len;
	}
	getvvcol(curwin, fp, &vcol_start, ଖାଲି, &vcol_end, 0);
	++vcol_start;
	++vcol_end;
    }

theend:
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]))
    {
	ଯଦି (rettv_list_alloc(rettv) == OK)
	{
	    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, vcol_start);
	    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, vcol_end);
	}
	ଅନ୍ୟଥା
	    rettv->vval.v_ସଂଖ୍ୟା = 0;
    }
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = vcol_end;

    ଯଦି (winchanged)
	restore_win_noblock(&switchwin, TRUE);
}

/*
 * "visualmode()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_visualmode(typval_T *argvars, typval_T *rettv)
{
    char_u	str[2];

    ଯଦି (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_STRING;
    str[0] = curbuf->b_visual_mode_eval;
    str[1] = NUL;
    rettv->vval.v_string = vim_strsave(str);

    // A non-zero number or non-empty string argument: reset mode.
    ଯଦି (non_zero_arg(&argvars[0]))
	curbuf->b_visual_mode_eval = NUL;
}

/*
 * "wildmenumode()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_wildmenumode(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ଯଦି (wild_menu_showing || ((State & MODE_CMDLINE) && cmdline_pum_active()))
	rettv->vval.v_ସଂଖ୍ୟା = 1;
}

/*
 * "windowsversion()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_windowsversion(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strsave((char_u *)windowsVersion);
}

/*
 * "wordcount()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_wordcount(typval_T *argvars UNUSED, typval_T *rettv)
{
    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    cursor_pos_info(rettv->vval.v_dict);
}

/*
 * "xor(expr, expr)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
f_xor(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି)
					^ tv_get_ସଂଖ୍ୟା_chk(&argvars[1], ଖାଲି);
}

#endif // FEAT_EVAL
