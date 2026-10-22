/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

/*
 * Vim originated from Stevie version 3.6 (Fish disk 217) by GRWalter (Fred)
 * It has been changed beyond recognition since then.
 *
 * Differences between version 8.2 and 9.1 can be found with ":help version9".
 * Differences between version 7.4 and 8.x can be found with ":help version8".
 * Differences between version 6.4 and 7.x can be found with ":help version7".
 * Differences between version 5.8 and 6.x can be found with ":help version6".
 * Differences between version 4.x and 5.x can be found with ":help version5".
 * Differences between version 3.0 and 4.x can be found with ":help version4".
 * All the remarks about older versions have been removed, they are not very
 * interesting.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

ਅੱਖਰ		*ਸੰਸਕਰਣ = VIM_ਸੰਸਕਰਣ_SHORT;
ਸਥਿਰ ਅੱਖਰ	*medium_ਸੰਸਕਰਣ = VIM_ਸੰਸਕਰਣ_MEDIUM;

#ਜੇ defined(HAVE_DATE_ਸਮਾਂ)
# ਜੇ defined(VMS) && defined(VAXC)
ਅੱਖਰ	long_ਸੰਸਕਰਣ[ਆਕਾਰ(VIM_ਸੰਸਕਰਣ_LONG_DATE) + ਆਕਾਰ(__DATE__)
						      + ਆਕਾਰ(__TIME__) + 3];

    ਖਾਲੀ
init_long_ਸੰਸਕਰਣ(ਖਾਲੀ)
{
    /*
     * Construct the long version string.  Necessary because
     * VAX C can't concatenate strings in the preprocessor.
     */
    strcpy(long_ਸੰਸਕਰਣ, VIM_ਸੰਸਕਰਣ_LONG_DATE);
#  ifdef BUILD_DATE
    strcat(long_ਸੰਸਕਰਣ, BUILD_DATE);
#  ਨਹੀਂਤਾਂ
    strcat(long_ਸੰਸਕਰਣ, __DATE__);
    strcat(long_ਸੰਸਕਰਣ, " ");
    strcat(long_ਸੰਸਕਰਣ, __TIME__);
#  endif
    strcat(long_ਸੰਸਕਰਣ, ")");
}

# ਨਹੀਂਤਾਂ
ਅੱਖਰ	*long_ਸੰਸਕਰਣ = ਨਿਰਾ;

    ਖਾਲੀ
init_long_ਸੰਸਕਰਣ(ਖਾਲੀ)
{
    ਜੇ (long_ਸੰਸਕਰਣ != ਨਿਰਾ)
	ਵਾਪਸ;

#  ifdef BUILD_DATE
    ਅੱਖਰ *date_ਸਮਾਂ = BUILD_DATE;
#  ਨਹੀਂਤਾਂ
    ਅੱਖਰ *date_ਸਮਾਂ = __DATE__ " " __TIME__;
#  endif
    ਅੱਖਰ *ਸੁਨੇਹਾ = _("%s (%s, compiled %s)");
    size_t len = ਲੜੀ_ਲੰਬਾਈ(ਸੁਨੇਹਾ)
	+ STRLEN_LITERAL(VIM_ਸੰਸਕਰਣ_LONG_ONLY)
	+ STRLEN_LITERAL(VIM_ਸੰਸਕਰਣ_DATE_ONLY)
	+ ਲੜੀ_ਲੰਬਾਈ(date_ਸਮਾਂ);

    long_ਸੰਸਕਰਣ = alloc(len);
    ਜੇ (long_ਸੰਸਕਰਣ == ਨਿਰਾ)
	long_ਸੰਸਕਰਣ = VIM_ਸੰਸਕਰਣ_LONG;
    ਨਹੀਂਤਾਂ
	vim_snprintf(long_ਸੰਸਕਰਣ, len, ਸੁਨੇਹਾ,
		VIM_ਸੰਸਕਰਣ_LONG_ONLY, VIM_ਸੰਸਕਰਣ_DATE_ONLY, date_ਸਮਾਂ);
}
# endif
#ਨਹੀਂਤਾਂ
ਅੱਖਰ	*long_ਸੰਸਕਰਣ = VIM_ਸੰਸਕਰਣ_LONG;

    ਖਾਲੀ
init_long_ਸੰਸਕਰਣ(ਖਾਲੀ)
{
    // nothing to do
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅੱਖਰ *(features[]) =
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
	"+acl",
#ਨਹੀਂਤਾਂ
	"-acl",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
	"+arabic",
#ਨਹੀਂਤਾਂ
	"-arabic",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA		// only for Amiga systems
# ifdef FEAT_ARP
	"+ARP",
# ਨਹੀਂਤਾਂ
	"-ARP",
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_AUTOCHDIR
	"+autochdir",
#ਨਹੀਂਤਾਂ
	"-autochdir",
#ਜੇ_ਅੰਤ
	"+autocmd",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_AUTOSERVERNAME
	"+autoservername",
#ਨਹੀਂਤਾਂ
	"-autoservername",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
	"+balloon_eval",
#ਨਹੀਂਤਾਂ
	"-balloon_eval",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_ਪਦ
	"+balloon_eval_term",
#ਨਹੀਂਤਾਂ
	"-balloon_eval_term",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
	"+browse",
#ਨਹੀਂਤਾਂ
	"-browse",
#ਜੇ_ਅੰਤ
	"++builtin_terms",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BYTEOFF
	"+byte_offset",
#ਨਹੀਂਤਾਂ
	"-byte_offset",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	"+channel",
#ਨਹੀਂਤਾਂ
	"-channel",
#ਜੇ_ਅੰਤ
	"+cindent",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
	"+clientserver",
#ਨਹੀਂਤਾਂ
	"-clientserver",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	"+clipboard",
#ਨਹੀਂਤਾਂ
	"-clipboard",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
	"+clipboard_provider",
#ਨਹੀਂਤਾਂ
	"-clipboard_provider",
#ਜੇ_ਅੰਤ
	"+cmdline_compl",
	"+cmdline_hist",
	"+cmdline_info",
	"+comments",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	"+conceal",
#ਨਹੀਂਤਾਂ
	"-conceal",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	"+cryptv",
#ਨਹੀਂਤਾਂ
	"-cryptv",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
	"+cscope",
#ਨਹੀਂਤਾਂ
	"-cscope",
#ਜੇ_ਅੰਤ
	"+cursorbind",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
	"+cursorshape",
#ਨਹੀਂਤਾਂ
	"-cursorshape",
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
	"+dialog_con_gui",
#ਨਹੀਂਤਾਂ
# ਜੇ defined(FEAT_CON_DIALOG)
	"+dialog_con",
# ਨਹੀਂਤਾਂ
#  ਜੇ defined(FEAT_GUI_DIALOG)
	"+dialog_gui",
#  ਨਹੀਂਤਾਂ
	"-dialog",
#  endif
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	"+diff",
#ਨਹੀਂਤਾਂ
	"-diff",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
	"+digraphs",
#ਨਹੀਂਤਾਂ
	"-digraphs",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
# ifdef FEAT_DIRECTX
	"+directx",
# ਨਹੀਂਤਾਂ
	"-directx",
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
	"+dnd",
#ਨਹੀਂਤਾਂ
	"-dnd",
#ਜੇ_ਅੰਤ
	"-ebcdic",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	"+emacs_tags",
#ਨਹੀਂਤਾਂ
	"-emacs_tags",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	"+eval",
#ਨਹੀਂਤਾਂ
	"-eval",
#ਜੇ_ਅੰਤ
	"+ex_extra",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	"+extra_search",
#ਨਹੀਂਤਾਂ
	"-extra_search",
#ਜੇ_ਅੰਤ
	"-farsi",
	"+file_in_path",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
	"+find_in_path",
#ਨਹੀਂਤਾਂ
	"-find_in_path",
#ਜੇ_ਅੰਤ
	"+float",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	"+folding",
#ਨਹੀਂਤਾਂ
	"-folding",
#ਜੇ_ਅੰਤ
	"-footer",
	// only interesting on Unix systems
#ਜੇ !defined(USE_SYSTEM) && defined(UNIX)
	"+fork()",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
	"+gettext/dyn",
# ਨਹੀਂਤਾਂ
	"+gettext",
# endif
#ਨਹੀਂਤਾਂ
	"-gettext",
#ਜੇ_ਅੰਤ
	"-hangul_input",
#ਜੇ (defined(HAVE_ICONV_H) && defined(USE_ICONV)) || defined(DYNAMIC_ICONV)
# ifdef DYNAMIC_ICONV
	"+iconv/dyn",
# ਨਹੀਂਤਾਂ
	"+iconv",
# endif
#ਨਹੀਂਤਾਂ
	"-iconv",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
	"+image",
#ਨਹੀਂਤਾਂ
	"-image",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
	"+image_popup",
#ਨਹੀਂਤਾਂ
	"-image_popup",
#ਜੇ_ਅੰਤ
	"+insert_expand",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IPV6
	"+ipv6",
#ਨਹੀਂਤਾਂ
	"-ipv6",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	"+job",
#ਨਹੀਂਤਾਂ
	"-job",
#ਜੇ_ਅੰਤ
	"+jumplist",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
	"+keymap",
#ਨਹੀਂਤਾਂ
	"-keymap",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	"+lambda",
#ਨਹੀਂਤਾਂ
	"-lambda",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LANGMAP
	"+langmap",
#ਨਹੀਂਤਾਂ
	"-langmap",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LIBCALL
	"+libcall",
#ਨਹੀਂਤਾਂ
	"-libcall",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	"+linebreak",
#ਨਹੀਂਤਾਂ
	"-linebreak",
#ਜੇ_ਅੰਤ
	"+lispindent",
	"+listcmds",
	"+localmap",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LUA
# ifdef DYNAMIC_LUA
	"+lua/dyn",
# ਨਹੀਂਤਾਂ
	"+lua",
# endif
#ਨਹੀਂਤਾਂ
	"-lua",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	"+menu",
#ਨਹੀਂਤਾਂ
	"-menu",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
	"+mksession",
#ਨਹੀਂਤਾਂ
	"-mksession",
#ਜੇ_ਅੰਤ
	"+modify_fname",
	"+mouse",
#ਜੇ defined(UNIX) || defined(VMS)
# ifdef FEAT_MOUSE_DEC
	"+mouse_dec",
# ਨਹੀਂਤਾਂ
	"-mouse_dec",
# endif
# ifdef FEAT_MOUSE_GPM
#  ifdef DYNAMIC_GPM
	"+mouse_gpm/dyn",
#  ਨਹੀਂਤਾਂ
	"+mouse_gpm",
#  endif
# ਨਹੀਂਤਾਂ
	"-mouse_gpm",
# endif
# ifdef FEAT_MOUSE_JSB
	"+mouse_jsbterm",
# ਨਹੀਂਤਾਂ
	"-mouse_jsbterm",
# endif
# ifdef FEAT_MOUSE_NET
	"+mouse_netterm",
# ਨਹੀਂਤਾਂ
	"-mouse_netterm",
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __QNX__
# ifdef FEAT_MOUSE_PTERM
	"+mouse_pterm",
# ਨਹੀਂਤਾਂ
	"-mouse_pterm",
# endif
#ਜੇ_ਅੰਤ

#ਜੇ defined(UNIX) || defined(VMS)
	"+mouse_sgr",
# ifdef FEAT_SYSMOUSE
	"+mouse_sysmouse",
# ਨਹੀਂਤਾਂ
	"-mouse_sysmouse",
# endif
# ifdef FEAT_MOUSE_URXVT
	"+mouse_urxvt",
# ਨਹੀਂਤਾਂ
	"-mouse_urxvt",
# endif
	"+mouse_xterm",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
	"+mouseshape",
#ਨਹੀਂਤਾਂ
	"-mouseshape",
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
# ifdef DYNAMIC_IME
	"+multi_byte_ime/dyn",
# ਨਹੀਂਤਾਂ
	"+multi_byte_ime",
# endif
#ਨਹੀਂਤਾਂ
	"+multi_byte",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	"+multi_lang",
#ਨਹੀਂਤਾਂ
	"-multi_lang",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
# ifdef DYNAMIC_MZSCHEME
	"+mzscheme/dyn",
# ਨਹੀਂਤਾਂ
	"+mzscheme",
# endif
#ਨਹੀਂਤਾਂ
	"-mzscheme",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	"+netbeans_intg",
#ਨਹੀਂਤਾਂ
	"-netbeans_intg",
#ਜੇ_ਅੰਤ
	"+num64",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
# ifdef FEAT_OLE
	"+ole",
# ਨਹੀਂਤਾਂ
	"-ole",
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	"+packages",
#ਨਹੀਂਤਾਂ
	"-packages",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINT_PANGO
	"+pango",
#ਨਹੀਂਤਾਂ
	"-pango",
#ਜੇ_ਅੰਤ
	"+path_extra",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERL
# ifdef DYNAMIC_PERL
	"+perl/dyn",
# ਨਹੀਂਤਾਂ
	"+perl",
# endif
#ਨਹੀਂਤਾਂ
	"-perl",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
	"+persistent_undo",
#ਨਹੀਂਤਾਂ
	"-persistent_undo",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	"+popupwin",
#ਨਹੀਂਤਾਂ
	"-popupwin",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINTER
# ifdef FEAT_POSTSCRIPT
	"+postscript",
# ਨਹੀਂਤਾਂ
	"-postscript",
# endif
	"+printer",
#ਨਹੀਂਤਾਂ
	"-printer",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	"+profile",
#ਨਹੀਂਤਾਂ
	"-profile",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
# ifdef DYNAMIC_PYTHON
	"+python/dyn",
# ਨਹੀਂਤਾਂ
	"+python",
# endif
#ਨਹੀਂਤਾਂ
	"-python",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
# ifdef DYNAMIC_PYTHON3
#  ifdef DYNAMIC_PYTHON3_STABLE_ABI
	"+python3/dyn-stable",
#  ਨਹੀਂਤਾਂ
	"+python3/dyn",
#  endif
# ਨਹੀਂਤਾਂ
	"+python3",
# endif
#ਨਹੀਂਤਾਂ
	"-python3",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	"+quickfix",
#ਨਹੀਂਤਾਂ
	"-quickfix",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
	"+reltime",
#ਨਹੀਂਤਾਂ
	"-reltime",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	"+rightleft",
#ਨਹੀਂਤਾਂ
	"-rightleft",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RUBY
# ifdef DYNAMIC_RUBY
	"+ruby/dyn",
# ਨਹੀਂਤਾਂ
	"+ruby",
# endif
#ਨਹੀਂਤਾਂ
	"-ruby",
#ਜੇ_ਅੰਤ
	"+scrollbind",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	"+signs",
#ਨਹੀਂਤਾਂ
	"-signs",
#ਜੇ_ਅੰਤ
	"+smartindent",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOCKETSERVER
	"+socketserver",
#ਨਹੀਂਤਾਂ
	"-socketserver",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SODIUM
# ifdef DYNAMIC_SODIUM
	"+sodium/dyn",
# ਨਹੀਂਤਾਂ
	"+sodium",
# endif
#ਨਹੀਂਤਾਂ
	"-sodium",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOUND
	"+sound",
#ਨਹੀਂਤਾਂ
	"-sound",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	"+spell",
#ਨਹੀਂਤਾਂ
	"-spell",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ STARTUPTIME
	"+startuptime",
#ਨਹੀਂਤਾਂ
	"-startuptime",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
	"+statusline",
#ਨਹੀਂਤਾਂ
	"-statusline",
#ਜੇ_ਅੰਤ
	"-sun_workshop",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	"+syntax",
#ਨਹੀਂਤਾਂ
	"-syntax",
#ਜੇ_ਅੰਤ
	    // only interesting on Unix systems
#ਜੇ defined(USE_SYSTEM) && defined(UNIX)
	"+system()",
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_TABPANEL)
	"+tabpanel",
#ਨਹੀਂਤਾਂ
	"-tabpanel",
#ਜੇ_ਅੰਤ
	"-tag_any_white",
	"+tag_binary",
	"-tag_old_static",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TCL
# ifdef DYNAMIC_TCL
	"+tcl/dyn",
# ਨਹੀਂਤਾਂ
	"+tcl",
# endif
#ਨਹੀਂਤਾਂ
	"-tcl",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	"+termguicolors",
#ਨਹੀਂਤਾਂ
	"-termguicolors",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	"+terminal",
#ਨਹੀਂਤਾਂ
	"-terminal",
#ਜੇ_ਅੰਤ
#ਜੇ defined(UNIX)
// only Unix can have terminfo instead of termcap
# ifdef TERMINFO
	"+terminfo",
# ਨਹੀਂਤਾਂ
	"-terminfo",
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
	"+termresponse",
#ਨਹੀਂਤਾਂ
	"-termresponse",
#ਜੇ_ਅੰਤ
	"+textobjects",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	"+textprop",
#ਨਹੀਂਤਾਂ
	"-textprop",
#ਜੇ_ਅੰਤ
#ਜੇ !defined(UNIX)
// unix always includes termcap support
# ifdef HAVE_TGETENT
	"+tgetent",
# ਨਹੀਂਤਾਂ
	"-tgetent",
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
	"+timers",
#ਨਹੀਂਤਾਂ
	"-timers",
#ਜੇ_ਅੰਤ
	"+title",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
	"+toolbar",
#ਨਹੀਂਤਾਂ
	"-toolbar",
#ਜੇ_ਅੰਤ
	"+user_commands",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	"+vartabs",
#ਨਹੀਂਤਾਂ
	"-vartabs",
#ਜੇ_ਅੰਤ
	"+vertsplit",
	"+vim9script",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
	"+viminfo",
#ਨਹੀਂਤਾਂ
	"-viminfo",
#ਜੇ_ਅੰਤ
	"+virtualedit",
	"+visual",
	"+visualextra",
	"+vreplace",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
# ifdef FEAT_VTP
	"+vtp",
# ਨਹੀਂਤਾਂ
	"-vtp",
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND
	"+wayland",
#ਨਹੀਂਤਾਂ
	"-wayland",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD
	"+wayland_clipboard",
#ਨਹੀਂਤਾਂ
	"-wayland_clipboard",
#ਜੇ_ਅੰਤ
	"+wildignore",
	"+wildmenu",
	"+windows",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WRITEBACKUP
	"+writebackup",
#ਨਹੀਂਤਾਂ
	"-writebackup",
#ਜੇ_ਅੰਤ
#ਜੇ defined(UNIX) || defined(VMS)
# ifdef FEAT_X11
	"+X11",
# ਨਹੀਂਤਾਂ
	"-X11",
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XATTR
	"+xattr",
#ਨਹੀਂਤਾਂ
	"-xattr",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
	"+xfontset",
#ਨਹੀਂਤਾਂ
	"-xfontset",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
	"+xim",
#ਨਹੀਂਤਾਂ
	"-xim",
#ਜੇ_ਅੰਤ
#ਜੇ defined(MSWIN)
# ifdef FEAT_XPM_W32
	"+xpm_w32",
# ਨਹੀਂਤਾਂ
	"-xpm_w32",
# endif
#ਜਾਂਜੇ defined(HAVE_XPM)
	"+xpm",
#ਨਹੀਂਤਾਂ
	"-xpm",
#ਜੇ_ਅੰਤ
#ਜੇ defined(UNIX) || defined(VMS)
# ਜੇ defined(USE_XSMP_INTERACT)
	"+xsmp_interact",
# elif defined(USE_XSMP)
	"+xsmp",
# ਨਹੀਂਤਾਂ
	"-xsmp",
# endif
# ifdef FEAT_XCLIPBOARD
	"+xterm_clipboard",
# ਨਹੀਂਤਾਂ
	"-xterm_clipboard",
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XTERM_SAVE
	"+xterm_save",
#ਨਹੀਂਤਾਂ
	"-xterm_save",
#ਜੇ_ਅੰਤ
	ਨਿਰਾ
};

ਸਥਿਰ ਪੂਰਨ included_patches[] =
{   /* Add new patch number below this line */
/**/
    1225,
/**/
    1224,
/**/
    1223,
/**/
    1222,
/**/
    1221,
/**/
    1220,
/**/
    1219,
/**/
    1218,
/**/
    1217,
/**/
    1216,
/**/
    1215,
/**/
    1214,
/**/
    1213,
/**/
    1212,
/**/
    1211,
/**/
    1210,
/**/
    1209,
/**/
    1208,
/**/
    1207,
/**/
    1206,
/**/
    1205,
/**/
    1204,
/**/
    1203,
/**/
    1202,
/**/
    1201,
/**/
    1200,
/**/
    1199,
/**/
    1198,
/**/
    1197,
/**/
    1196,
/**/
    1195,
/**/
    1194,
/**/
    1193,
/**/
    1192,
/**/
    1191,
/**/
    1190,
/**/
    1189,
/**/
    1188,
/**/
    1187,
/**/
    1186,
/**/
    1185,
/**/
    1184,
/**/
    1183,
/**/
    1182,
/**/
    1181,
/**/
    1180,
/**/
    1179,
/**/
    1178,
/**/
    1177,
/**/
    1176,
/**/
    1175,
/**/
    1174,
/**/
    1173,
/**/
    1172,
/**/
    1171,
/**/
    1170,
/**/
    1169,
/**/
    1168,
/**/
    1167,
/**/
    1166,
/**/
    1165,
/**/
    1164,
/**/
    1163,
/**/
    1162,
/**/
    1161,
/**/
    1160,
/**/
    1159,
/**/
    1158,
/**/
    1157,
/**/
    1156,
/**/
    1155,
/**/
    1154,
/**/
    1153,
/**/
    1152,
/**/
    1151,
/**/
    1150,
/**/
    1149,
/**/
    1148,
/**/
    1147,
/**/
    1146,
/**/
    1145,
/**/
    1144,
/**/
    1143,
/**/
    1142,
/**/
    1141,
/**/
    1140,
/**/
    1139,
/**/
    1138,
/**/
    1137,
/**/
    1136,
/**/
    1135,
/**/
    1134,
/**/
    1133,
/**/
    1132,
/**/
    1131,
/**/
    1130,
/**/
    1129,
/**/
    1128,
/**/
    1127,
/**/
    1126,
/**/
    1125,
/**/
    1124,
/**/
    1123,
/**/
    1122,
/**/
    1121,
/**/
    1120,
/**/
    1119,
/**/
    1118,
/**/
    1117,
/**/
    1116,
/**/
    1115,
/**/
    1114,
/**/
    1113,
/**/
    1112,
/**/
    1111,
/**/
    1110,
/**/
    1109,
/**/
    1108,
/**/
    1107,
/**/
    1106,
/**/
    1105,
/**/
    1104,
/**/
    1103,
/**/
    1102,
/**/
    1101,
/**/
    1100,
/**/
    1099,
/**/
    1098,
/**/
    1097,
/**/
    1096,
/**/
    1095,
/**/
    1094,
/**/
    1093,
/**/
    1092,
/**/
    1091,
/**/
    1090,
/**/
    1089,
/**/
    1088,
/**/
    1087,
/**/
    1086,
/**/
    1085,
/**/
    1084,
/**/
    1083,
/**/
    1082,
/**/
    1081,
/**/
    1080,
/**/
    1079,
/**/
    1078,
/**/
    1077,
/**/
    1076,
/**/
    1075,
/**/
    1074,
/**/
    1073,
/**/
    1072,
/**/
    1071,
/**/
    1070,
/**/
    1069,
/**/
    1068,
/**/
    1067,
/**/
    1066,
/**/
    1065,
/**/
    1064,
/**/
    1063,
/**/
    1062,
/**/
    1061,
/**/
    1060,
/**/
    1059,
/**/
    1058,
/**/
    1057,
/**/
    1056,
/**/
    1055,
/**/
    1054,
/**/
    1053,
/**/
    1052,
/**/
    1051,
/**/
    1050,
/**/
    1049,
/**/
    1048,
/**/
    1047,
/**/
    1046,
/**/
    1045,
/**/
    1044,
/**/
    1043,
/**/
    1042,
/**/
    1041,
/**/
    1040,
/**/
    1039,
/**/
    1038,
/**/
    1037,
/**/
    1036,
/**/
    1035,
/**/
    1034,
/**/
    1033,
/**/
    1032,
/**/
    1031,
/**/
    1030,
/**/
    1029,
/**/
    1028,
/**/
    1027,
/**/
    1026,
/**/
    1025,
/**/
    1024,
/**/
    1023,
/**/
    1022,
/**/
    1021,
/**/
    1020,
/**/
    1019,
/**/
    1018,
/**/
    1017,
/**/
    1016,
/**/
    1015,
/**/
    1014,
/**/
    1013,
/**/
    1012,
/**/
    1011,
/**/
    1010,
/**/
    1009,
/**/
    1008,
/**/
    1007,
/**/
    1006,
/**/
    1005,
/**/
    1004,
/**/
    1003,
/**/
    1002,
/**/
    1001,
/**/
    1000,
/**/
    999,
/**/
    998,
/**/
    997,
/**/
    996,
/**/
    995,
/**/
    994,
/**/
    993,
/**/
    992,
/**/
    991,
/**/
    990,
/**/
    989,
/**/
    988,
/**/
    987,
/**/
    986,
/**/
    985,
/**/
    984,
/**/
    983,
/**/
    982,
/**/
    981,
/**/
    980,
/**/
    979,
/**/
    978,
/**/
    977,
/**/
    976,
/**/
    975,
/**/
    974,
/**/
    973,
/**/
    972,
/**/
    971,
/**/
    970,
/**/
    969,
/**/
    968,
/**/
    967,
/**/
    966,
/**/
    965,
/**/
    964,
/**/
    963,
/**/
    962,
/**/
    961,
/**/
    960,
/**/
    959,
/**/
    958,
/**/
    957,
/**/
    956,
/**/
    955,
/**/
    954,
/**/
    953,
/**/
    952,
/**/
    951,
/**/
    950,
/**/
    949,
/**/
    948,
/**/
    947,
/**/
    946,
/**/
    945,
/**/
    944,
/**/
    943,
/**/
    942,
/**/
    941,
/**/
    940,
/**/
    939,
/**/
    938,
/**/
    937,
/**/
    936,
/**/
    935,
/**/
    934,
/**/
    933,
/**/
    932,
/**/
    931,
/**/
    930,
/**/
    929,
/**/
    928,
/**/
    927,
/**/
    926,
/**/
    925,
/**/
    924,
/**/
    923,
/**/
    922,
/**/
    921,
/**/
    920,
/**/
    919,
/**/
    918,
/**/
    917,
/**/
    916,
/**/
    915,
/**/
    914,
/**/
    913,
/**/
    912,
/**/
    911,
/**/
    910,
/**/
    909,
/**/
    908,
/**/
    907,
/**/
    906,
/**/
    905,
/**/
    904,
/**/
    903,
/**/
    902,
/**/
    901,
/**/
    900,
/**/
    899,
/**/
    898,
/**/
    897,
/**/
    896,
/**/
    895,
/**/
    894,
/**/
    893,
/**/
    892,
/**/
    891,
/**/
    890,
/**/
    889,
/**/
    888,
/**/
    887,
/**/
    886,
/**/
    885,
/**/
    884,
/**/
    883,
/**/
    882,
/**/
    881,
/**/
    880,
/**/
    879,
/**/
    878,
/**/
    877,
/**/
    876,
/**/
    875,
/**/
    874,
/**/
    873,
/**/
    872,
/**/
    871,
/**/
    870,
/**/
    869,
/**/
    868,
/**/
    867,
/**/
    866,
/**/
    865,
/**/
    864,
/**/
    863,
/**/
    862,
/**/
    861,
/**/
    860,
/**/
    859,
/**/
    858,
/**/
    857,
/**/
    856,
/**/
    855,
/**/
    854,
/**/
    853,
/**/
    852,
/**/
    851,
/**/
    850,
/**/
    849,
/**/
    848,
/**/
    847,
/**/
    846,
/**/
    845,
/**/
    844,
/**/
    843,
/**/
    842,
/**/
    841,
/**/
    840,
/**/
    839,
/**/
    838,
/**/
    837,
/**/
    836,
/**/
    835,
/**/
    834,
/**/
    833,
/**/
    832,
/**/
    831,
/**/
    830,
/**/
    829,
/**/
    828,
/**/
    827,
/**/
    826,
/**/
    825,
/**/
    824,
/**/
    823,
/**/
    822,
/**/
    821,
/**/
    820,
/**/
    819,
/**/
    818,
/**/
    817,
/**/
    816,
/**/
    815,
/**/
    814,
/**/
    813,
/**/
    812,
/**/
    811,
/**/
    810,
/**/
    809,
/**/
    808,
/**/
    807,
/**/
    806,
/**/
    805,
/**/
    804,
/**/
    803,
/**/
    802,
/**/
    801,
/**/
    800,
/**/
    799,
/**/
    798,
/**/
    797,
/**/
    796,
/**/
    795,
/**/
    794,
/**/
    793,
/**/
    792,
/**/
    791,
/**/
    790,
/**/
    789,
/**/
    788,
/**/
    787,
/**/
    786,
/**/
    785,
/**/
    784,
/**/
    783,
/**/
    782,
/**/
    781,
/**/
    780,
/**/
    779,
/**/
    778,
/**/
    777,
/**/
    776,
/**/
    775,
/**/
    774,
/**/
    773,
/**/
    772,
/**/
    771,
/**/
    770,
/**/
    769,
/**/
    768,
/**/
    767,
/**/
    766,
/**/
    765,
/**/
    764,
/**/
    763,
/**/
    762,
/**/
    761,
/**/
    760,
/**/
    759,
/**/
    758,
/**/
    757,
/**/
    756,
/**/
    755,
/**/
    754,
/**/
    753,
/**/
    752,
/**/
    751,
/**/
    750,
/**/
    749,
/**/
    748,
/**/
    747,
/**/
    746,
/**/
    745,
/**/
    744,
/**/
    743,
/**/
    742,
/**/
    741,
/**/
    740,
/**/
    739,
/**/
    738,
/**/
    737,
/**/
    736,
/**/
    735,
/**/
    734,
/**/
    733,
/**/
    732,
/**/
    731,
/**/
    730,
/**/
    729,
/**/
    728,
/**/
    727,
/**/
    726,
/**/
    725,
/**/
    724,
/**/
    723,
/**/
    722,
/**/
    721,
/**/
    720,
/**/
    719,
/**/
    718,
/**/
    717,
/**/
    716,
/**/
    715,
/**/
    714,
/**/
    713,
/**/
    712,
/**/
    711,
/**/
    710,
/**/
    709,
/**/
    708,
/**/
    707,
/**/
    706,
/**/
    705,
/**/
    704,
/**/
    703,
/**/
    702,
/**/
    701,
/**/
    700,
/**/
    699,
/**/
    698,
/**/
    697,
/**/
    696,
/**/
    695,
/**/
    694,
/**/
    693,
/**/
    692,
/**/
    691,
/**/
    690,
/**/
    689,
/**/
    688,
/**/
    687,
/**/
    686,
/**/
    685,
/**/
    684,
/**/
    683,
/**/
    682,
/**/
    681,
/**/
    680,
/**/
    679,
/**/
    678,
/**/
    677,
/**/
    676,
/**/
    675,
/**/
    674,
/**/
    673,
/**/
    672,
/**/
    671,
/**/
    670,
/**/
    669,
/**/
    668,
/**/
    667,
/**/
    666,
/**/
    665,
/**/
    664,
/**/
    663,
/**/
    662,
/**/
    661,
/**/
    660,
/**/
    659,
/**/
    658,
/**/
    657,
/**/
    656,
/**/
    655,
/**/
    654,
/**/
    653,
/**/
    652,
/**/
    651,
/**/
    650,
/**/
    649,
/**/
    648,
/**/
    647,
/**/
    646,
/**/
    645,
/**/
    644,
/**/
    643,
/**/
    642,
/**/
    641,
/**/
    640,
/**/
    639,
/**/
    638,
/**/
    637,
/**/
    636,
/**/
    635,
/**/
    634,
/**/
    633,
/**/
    632,
/**/
    631,
/**/
    630,
/**/
    629,
/**/
    628,
/**/
    627,
/**/
    626,
/**/
    625,
/**/
    624,
/**/
    623,
/**/
    622,
/**/
    621,
/**/
    620,
/**/
    619,
/**/
    618,
/**/
    617,
/**/
    616,
/**/
    615,
/**/
    614,
/**/
    613,
/**/
    612,
/**/
    611,
/**/
    610,
/**/
    609,
/**/
    608,
/**/
    607,
/**/
    606,
/**/
    605,
/**/
    604,
/**/
    603,
/**/
    602,
/**/
    601,
/**/
    600,
/**/
    599,
/**/
    598,
/**/
    597,
/**/
    596,
/**/
    595,
/**/
    594,
/**/
    593,
/**/
    592,
/**/
    591,
/**/
    590,
/**/
    589,
/**/
    588,
/**/
    587,
/**/
    586,
/**/
    585,
/**/
    584,
/**/
    583,
/**/
    582,
/**/
    581,
/**/
    580,
/**/
    579,
/**/
    578,
/**/
    577,
/**/
    576,
/**/
    575,
/**/
    574,
/**/
    573,
/**/
    572,
/**/
    571,
/**/
    570,
/**/
    569,
/**/
    568,
/**/
    567,
/**/
    566,
/**/
    565,
/**/
    564,
/**/
    563,
/**/
    562,
/**/
    561,
/**/
    560,
/**/
    559,
/**/
    558,
/**/
    557,
/**/
    556,
/**/
    555,
/**/
    554,
/**/
    553,
/**/
    552,
/**/
    551,
/**/
    550,
/**/
    549,
/**/
    548,
/**/
    547,
/**/
    546,
/**/
    545,
/**/
    544,
/**/
    543,
/**/
    542,
/**/
    541,
/**/
    540,
/**/
    539,
/**/
    538,
/**/
    537,
/**/
    536,
/**/
    535,
/**/
    534,
/**/
    533,
/**/
    532,
/**/
    531,
/**/
    530,
/**/
    529,
/**/
    528,
/**/
    527,
/**/
    526,
/**/
    525,
/**/
    524,
/**/
    523,
/**/
    522,
/**/
    521,
/**/
    520,
/**/
    519,
/**/
    518,
/**/
    517,
/**/
    516,
/**/
    515,
/**/
    514,
/**/
    513,
/**/
    512,
/**/
    511,
/**/
    510,
/**/
    509,
/**/
    508,
/**/
    507,
/**/
    506,
/**/
    505,
/**/
    504,
/**/
    503,
/**/
    502,
/**/
    501,
/**/
    500,
/**/
    499,
/**/
    498,
/**/
    497,
/**/
    496,
/**/
    495,
/**/
    494,
/**/
    493,
/**/
    492,
/**/
    491,
/**/
    490,
/**/
    489,
/**/
    488,
/**/
    487,
/**/
    486,
/**/
    485,
/**/
    484,
/**/
    483,
/**/
    482,
/**/
    481,
/**/
    480,
/**/
    479,
/**/
    478,
/**/
    477,
/**/
    476,
/**/
    475,
/**/
    474,
/**/
    473,
/**/
    472,
/**/
    471,
/**/
    470,
/**/
    469,
/**/
    468,
/**/
    467,
/**/
    466,
/**/
    465,
/**/
    464,
/**/
    463,
/**/
    462,
/**/
    461,
/**/
    460,
/**/
    459,
/**/
    458,
/**/
    457,
/**/
    456,
/**/
    455,
/**/
    454,
/**/
    453,
/**/
    452,
/**/
    451,
/**/
    450,
/**/
    449,
/**/
    448,
/**/
    447,
/**/
    446,
/**/
    445,
/**/
    444,
/**/
    443,
/**/
    442,
/**/
    441,
/**/
    440,
/**/
    439,
/**/
    438,
/**/
    437,
/**/
    436,
/**/
    435,
/**/
    434,
/**/
    433,
/**/
    432,
/**/
    431,
/**/
    430,
/**/
    429,
/**/
    428,
/**/
    427,
/**/
    426,
/**/
    425,
/**/
    424,
/**/
    423,
/**/
    422,
/**/
    421,
/**/
    420,
/**/
    419,
/**/
    418,
/**/
    417,
/**/
    416,
/**/
    415,
/**/
    414,
/**/
    413,
/**/
    412,
/**/
    411,
/**/
    410,
/**/
    409,
/**/
    408,
/**/
    407,
/**/
    406,
/**/
    405,
/**/
    404,
/**/
    403,
/**/
    402,
/**/
    401,
/**/
    400,
/**/
    399,
/**/
    398,
/**/
    397,
/**/
    396,
/**/
    395,
/**/
    394,
/**/
    393,
/**/
    392,
/**/
    391,
/**/
    390,
/**/
    389,
/**/
    388,
/**/
    387,
/**/
    386,
/**/
    385,
/**/
    384,
/**/
    383,
/**/
    382,
/**/
    381,
/**/
    380,
/**/
    379,
/**/
    378,
/**/
    377,
/**/
    376,
/**/
    375,
/**/
    374,
/**/
    373,
/**/
    372,
/**/
    371,
/**/
    370,
/**/
    369,
/**/
    368,
/**/
    367,
/**/
    366,
/**/
    365,
/**/
    364,
/**/
    363,
/**/
    362,
/**/
    361,
/**/
    360,
/**/
    359,
/**/
    358,
/**/
    357,
/**/
    356,
/**/
    355,
/**/
    354,
/**/
    353,
/**/
    352,
/**/
    351,
/**/
    350,
/**/
    349,
/**/
    348,
/**/
    347,
/**/
    346,
/**/
    345,
/**/
    344,
/**/
    343,
/**/
    342,
/**/
    341,
/**/
    340,
/**/
    339,
/**/
    338,
/**/
    337,
/**/
    336,
/**/
    335,
/**/
    334,
/**/
    333,
/**/
    332,
/**/
    331,
/**/
    330,
/**/
    329,
/**/
    328,
/**/
    327,
/**/
    326,
/**/
    325,
/**/
    324,
/**/
    323,
/**/
    322,
/**/
    321,
/**/
    320,
/**/
    319,
/**/
    318,
/**/
    317,
/**/
    316,
/**/
    315,
/**/
    314,
/**/
    313,
/**/
    312,
/**/
    311,
/**/
    310,
/**/
    309,
/**/
    308,
/**/
    307,
/**/
    306,
/**/
    305,
/**/
    304,
/**/
    303,
/**/
    302,
/**/
    301,
/**/
    300,
/**/
    299,
/**/
    298,
/**/
    297,
/**/
    296,
/**/
    295,
/**/
    294,
/**/
    293,
/**/
    292,
/**/
    291,
/**/
    290,
/**/
    289,
/**/
    288,
/**/
    287,
/**/
    286,
/**/
    285,
/**/
    284,
/**/
    283,
/**/
    282,
/**/
    281,
/**/
    280,
/**/
    279,
/**/
    278,
/**/
    277,
/**/
    276,
/**/
    275,
/**/
    274,
/**/
    273,
/**/
    272,
/**/
    271,
/**/
    270,
/**/
    269,
/**/
    268,
/**/
    267,
/**/
    266,
/**/
    265,
/**/
    264,
/**/
    263,
/**/
    262,
/**/
    261,
/**/
    260,
/**/
    259,
/**/
    258,
/**/
    257,
/**/
    256,
/**/
    255,
/**/
    254,
/**/
    253,
/**/
    252,
/**/
    251,
/**/
    250,
/**/
    249,
/**/
    248,
/**/
    247,
/**/
    246,
/**/
    245,
/**/
    244,
/**/
    243,
/**/
    242,
/**/
    241,
/**/
    240,
/**/
    239,
/**/
    238,
/**/
    237,
/**/
    236,
/**/
    235,
/**/
    234,
/**/
    233,
/**/
    232,
/**/
    231,
/**/
    230,
/**/
    229,
/**/
    228,
/**/
    227,
/**/
    226,
/**/
    225,
/**/
    224,
/**/
    223,
/**/
    222,
/**/
    221,
/**/
    220,
/**/
    219,
/**/
    218,
/**/
    217,
/**/
    216,
/**/
    215,
/**/
    214,
/**/
    213,
/**/
    212,
/**/
    211,
/**/
    210,
/**/
    209,
/**/
    208,
/**/
    207,
/**/
    206,
/**/
    205,
/**/
    204,
/**/
    203,
/**/
    202,
/**/
    201,
/**/
    200,
/**/
    199,
/**/
    198,
/**/
    197,
/**/
    196,
/**/
    195,
/**/
    194,
/**/
    193,
/**/
    192,
/**/
    191,
/**/
    190,
/**/
    189,
/**/
    188,
/**/
    187,
/**/
    186,
/**/
    185,
/**/
    184,
/**/
    183,
/**/
    182,
/**/
    181,
/**/
    180,
/**/
    179,
/**/
    178,
/**/
    177,
/**/
    176,
/**/
    175,
/**/
    174,
/**/
    173,
/**/
    172,
/**/
    171,
/**/
    170,
/**/
    169,
/**/
    168,
/**/
    167,
/**/
    166,
/**/
    165,
/**/
    164,
/**/
    163,
/**/
    162,
/**/
    161,
/**/
    160,
/**/
    159,
/**/
    158,
/**/
    157,
/**/
    156,
/**/
    155,
/**/
    154,
/**/
    153,
/**/
    152,
/**/
    151,
/**/
    150,
/**/
    149,
/**/
    148,
/**/
    147,
/**/
    146,
/**/
    145,
/**/
    144,
/**/
    143,
/**/
    142,
/**/
    141,
/**/
    140,
/**/
    139,
/**/
    138,
/**/
    137,
/**/
    136,
/**/
    135,
/**/
    134,
/**/
    133,
/**/
    132,
/**/
    131,
/**/
    130,
/**/
    129,
/**/
    128,
/**/
    127,
/**/
    126,
/**/
    125,
/**/
    124,
/**/
    123,
/**/
    122,
/**/
    121,
/**/
    120,
/**/
    119,
/**/
    118,
/**/
    117,
/**/
    116,
/**/
    115,
/**/
    114,
/**/
    113,
/**/
    112,
/**/
    111,
/**/
    110,
/**/
    109,
/**/
    108,
/**/
    107,
/**/
    106,
/**/
    105,
/**/
    104,
/**/
    103,
/**/
    102,
/**/
    101,
/**/
    100,
/**/
    99,
/**/
    98,
/**/
    97,
/**/
    96,
/**/
    95,
/**/
    94,
/**/
    93,
/**/
    92,
/**/
    91,
/**/
    90,
/**/
    89,
/**/
    88,
/**/
    87,
/**/
    86,
/**/
    85,
/**/
    84,
/**/
    83,
/**/
    82,
/**/
    81,
/**/
    80,
/**/
    79,
/**/
    78,
/**/
    77,
/**/
    76,
/**/
    75,
/**/
    74,
/**/
    73,
/**/
    72,
/**/
    71,
/**/
    70,
/**/
    69,
/**/
    68,
/**/
    67,
/**/
    66,
/**/
    65,
/**/
    64,
/**/
    63,
/**/
    62,
/**/
    61,
/**/
    60,
/**/
    59,
/**/
    58,
/**/
    57,
/**/
    56,
/**/
    55,
/**/
    54,
/**/
    53,
/**/
    52,
/**/
    51,
/**/
    50,
/**/
    49,
/**/
    48,
/**/
    47,
/**/
    46,
/**/
    45,
/**/
    44,
/**/
    43,
/**/
    42,
/**/
    41,
/**/
    40,
/**/
    39,
/**/
    38,
/**/
    37,
/**/
    36,
/**/
    35,
/**/
    34,
/**/
    33,
/**/
    32,
/**/
    31,
/**/
    30,
/**/
    29,
/**/
    28,
/**/
    27,
/**/
    26,
/**/
    25,
/**/
    24,
/**/
    23,
/**/
    22,
/**/
    21,
/**/
    20,
/**/
    19,
/**/
    18,
/**/
    17,
/**/
    16,
/**/
    15,
/**/
    14,
/**/
    13,
/**/
    12,
/**/
    11,
/**/
    10,
/**/
    9,
/**/
    8,
/**/
    7,
/**/
    6,
/**/
    5,
/**/
    4,
/**/
    3,
/**/
    2,
/**/
    1,
/**/
    0
};

/*
 * Place to put a short description when adding a feature with a patch.
 * Keep it short, e.g.,: "relative numbers", "persistent undo".
 * Also add a comment marker to separate the lines.
 * See the official Vim patches for the diff format: It must use a context of
 * one line only.  Create it by hand or use "diff -C2" and edit the patch.
 */
ਸਥਿਰ ਅੱਖਰ *(extra_patches[]) =
{   /* Add your patch description below this line */
/**/
    ਨਿਰਾ
};

    ਪੂਰਨ
highest_patch(ਖਾਲੀ)
{
    // this relies on the highest patch number to be the first entry
    ਵਾਪਸ included_patches[0];
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return TRUE if patch "n" has been included.
 */
    ਪੂਰਨ
has_patch(ਪੂਰਨ n)
{
    ਪੂਰਨ		h, m, l;

    // Perform a binary search.
    l = 0;
    h = (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(included_patches) - 1;
    ਲਈ (;;)
    {
	m = (l + h) / 2;
	ਜੇ (included_patches[m] == n)
	    ਵਾਪਸ TRUE;
	ਜੇ (l == h)
	    ਰੋਕੋ;
	ਜੇ (included_patches[m] < n)
	    h = m;
	ਨਹੀਂਤਾਂ
	    l = m + 1;
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
ex_ਸੰਸਕਰਣ(exarg_T *eap)
{
    /*
     * Ignore a ":version 9.99" command.
     */
    ਜੇ (*eap->arg == NUL)
    {
	ਸੁਨੇਹਾ_putchar('\n');
	list_ਸੰਸਕਰਣ();
    }
}

/*
 * Output a string for the version message.  If it's going to wrap, output a
 * newline, unless the message is too long to fit on the screen anyway.
 * When "wrap" is TRUE wrap the string in [].
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੰਸਕਰਣ_ਸੁਨੇਹਾ_wrap(char_u *s, ਪੂਰਨ wrap)
{
    ਪੂਰਨ		len = vim_strsize(s) + (wrap ? 2 : 0);

    ਜੇ (!got_int && len < cmdline_ਚੌੜਾਈ && ਸੁਨੇਹਾ_ਖਾਨਾ + len >= cmdline_ਚੌੜਾਈ
								&& *s != '\n')
	ਸੁਨੇਹਾ_putchar('\n');
    ਜੇ (!got_int)
    {
	ਜੇ (wrap)
	    ਸੁਨੇਹਾ_puts("[");
	ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)s);
	ਜੇ (wrap)
	    ਸੁਨੇਹਾ_puts("]");
    }
}

    ਸਥਿਰ ਖਾਲੀ
ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(ਅੱਖਰ *s)
{
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ_wrap((char_u *)s, FALSE);
}

/*
 * List all features aligned in columns, dictionary style.
 */
    ਸਥਿਰ ਖਾਲੀ
list_features(ਖਾਲੀ)
{
    list_ਵਿੱਚ_columns((char_u **)features, -1, -1);
}

/*
 * List string items nicely aligned in columns.
 * When "size" is < 0 then the last entry is marked with NULL.
 * The entry with index "current" is inclosed in [].
 */
    ਖਾਲੀ
list_ਵਿੱਚ_columns(char_u **ਚੀਜ਼ਾਂ, ਪੂਰਨ ਮਾਪ, ਪੂਰਨ current)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		ncol;
    ਪੂਰਨ		nrow;
    ਪੂਰਨ		cur_ਪੰਕਤੀ = 1;
    ਪੂਰਨ		ਵਸਤੂ_ਗਿਣਤੀ = 0;
    ਪੂਰਨ		ਚੌੜਾਈ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਪੂਰਨ		use_highlight = (ਚੀਜ਼ਾਂ == (char_u **)features);
#ਜੇ_ਅੰਤ

    // Find the length of the longest item, use that + 1 as the column
    // width.
    ਲਈ (i = 0; ਮਾਪ < 0 ? ਚੀਜ਼ਾਂ[i] != ਨਿਰਾ : i < ਮਾਪ; ++i)
    {
	ਪੂਰਨ l = vim_strsize(ਚੀਜ਼ਾਂ[i]) + (i == current ? 2 : 0);

	ਜੇ (l > ਚੌੜਾਈ)
	    ਚੌੜਾਈ = l;
	++ਵਸਤੂ_ਗਿਣਤੀ;
    }
    ਚੌੜਾਈ += 1;

    ਜੇ (cmdline_ਚੌੜਾਈ < ਚੌੜਾਈ)
    {
	// Not enough screen columns - show one per line
	ਲਈ (i = 0; i < ਵਸਤੂ_ਗਿਣਤੀ; ++i)
	{
	    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ_wrap(ਚੀਜ਼ਾਂ[i], i == current);
	    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ > 0 && i < ਵਸਤੂ_ਗਿਣਤੀ - 1)
		ਸੁਨੇਹਾ_putchar('\n');
	}
	ਵਾਪਸ;
    }

    // The rightmost column doesn't need a separator.
    // Sacrifice it to fit in one more column if possible.
    ncol = (cmdline_ਚੌੜਾਈ + 1) / ਚੌੜਾਈ;
    nrow = ਵਸਤੂ_ਗਿਣਤੀ / ncol + ((ਵਸਤੂ_ਗਿਣਤੀ % ncol) ? 1 : 0);

    // "i" counts columns then rows.  "idx" counts rows then columns.
    ਲਈ (i = 0; !got_int && i < nrow * ncol; ++i)
    {
	ਪੂਰਨ idx = (i / ncol) + (i % ncol) * nrow;

	ਜੇ (idx < ਵਸਤੂ_ਗਿਣਤੀ)
	{
	    ਪੂਰਨ ਆਖਰੀ_ਖਾਨਾ = (i + 1) % ncol == 0;

	    ਜੇ (idx == current)
		ਸੁਨੇਹਾ_putchar('[');
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਜੇ (use_highlight && ਚੀਜ਼ਾਂ[idx][0] == '-')
		ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)ਚੀਜ਼ਾਂ[idx], HL_ATTR(HLF_W));
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)ਚੀਜ਼ਾਂ[idx]);
	    ਜੇ (idx == current)
		ਸੁਨੇਹਾ_putchar(']');
	    ਜੇ (ਆਖਰੀ_ਖਾਨਾ)
	    {
		ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ > 0 && cur_ਪੰਕਤੀ < nrow)
		    ਸੁਨੇਹਾ_putchar('\n');
		++cur_ਪੰਕਤੀ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜਦੋਂ (ਸੁਨੇਹਾ_ਖਾਨਾ % ਚੌੜਾਈ)
		    ਸੁਨੇਹਾ_putchar(' ');
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // this row is out of items, thus at the end of the row
	    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ > 0)
	    {
		ਜੇ (cur_ਪੰਕਤੀ < nrow)
		    ਸੁਨੇਹਾ_putchar('\n');
		++cur_ਪੰਕਤੀ;
	    }
	}
    }
}

    ਖਾਲੀ
list_ਸੰਸਕਰਣ(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਪਹਿਲਾ;
    ਅੱਖਰ	*s = "";

    /*
     * When adding features here, don't forget to update the list of
     * internal variables in eval.c!
     */
    init_long_ਸੰਸਕਰਣ();
    ਸੁਨੇਹਾ(long_ਸੰਸਕਰਣ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
#   ifdef _WIN64
#    ਜੇ defined(_M_ARM64) || defined(_M_ARM64EC)
     ਸੁਨੇਹਾ_puts(_("\nMS-Windows ARM64 GUI/console version"));
#    ਨਹੀਂਤਾਂ
     ਸੁਨੇਹਾ_puts(_("\nMS-Windows 64-bit GUI/console version"));
#    endif
#   ਨਹੀਂਤਾਂ
    ਸੁਨੇਹਾ_puts(_("\nMS-Windows 32-bit GUI/console version"));
#   endif
#  ਨਹੀਂਤਾਂ
#   ifdef _WIN64
#    ਜੇ defined(_M_ARM64) || defined(_M_ARM64EC)
     ਸੁਨੇਹਾ_puts(_("\nMS-Windows ARM64 GUI version"));
#    ਨਹੀਂਤਾਂ
     ਸੁਨੇਹਾ_puts(_("\nMS-Windows 64-bit GUI version"));
#    endif
#   ਨਹੀਂਤਾਂ
    ਸੁਨੇਹਾ_puts(_("\nMS-Windows 32-bit GUI version"));
#   endif
#  endif
#  ifdef FEAT_OLE
    ਸੁਨੇਹਾ_puts(_(" with OLE support"));
#  endif
# ਨਹੀਂਤਾਂ
#  ifdef _WIN64
#   ਜੇ defined(_M_ARM64) || defined(_M_ARM64EC)
    ਸੁਨੇਹਾ_puts(_("\nMS-Windows ARM64 console version"));
#   ਨਹੀਂਤਾਂ
    ਸੁਨੇਹਾ_puts(_("\nMS-Windows 64-bit console version"));
#   endif
#  ਨਹੀਂਤਾਂ
    ਸੁਨੇਹਾ_puts(_("\nMS-Windows 32-bit console version"));
#  endif
# endif
#ਜੇ_ਅੰਤ
#ਜੇ defined(MACOS_X)
# ਜੇ defined(MACOS_X_DARWIN)
    ਸੁਨੇਹਾ_puts(_("\nmacOS version"));
# ਨਹੀਂਤਾਂ
    ਸੁਨੇਹਾ_puts(_("\nmacOS version w/o darwin feat."));
# endif
# ਜੇ defined(__arm64__)
    ਸੁਨੇਹਾ_puts(" - arm64");
# elif defined(__x86_64__)
    ਸੁਨੇਹਾ_puts(" - x86_64");
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    ਸੁਨੇਹਾ_puts(_("\nOpenVMS (build) arch, version"));
# ifdef HAVE_PATHDEF
    ਜੇ (*compiled_arch != NUL)
    {
	ਸੁਨੇਹਾ_puts(": ");
	ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)compiled_arch);
	ਜੇ (*compiled_vers != NUL)
	{
	    ਸੁਨੇਹਾ_puts(", ");
	    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)compiled_vers);
	}
    }

# endif

#ਜੇ_ਅੰਤ

    // Print the list of patch numbers if there is at least one.
    // Print a range when patches are consecutive: "1-10, 12, 15-40, 42-45"
    ਜੇ (included_patches[0] != 0)
    {
	ਸੁਨੇਹਾ_puts(_("\nIncluded patches: "));
	ਪਹਿਲਾ = -1;
	i = (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(included_patches) - 1;
	ਜਦੋਂ (--i >= 0)
	{
	    ਜੇ (ਪਹਿਲਾ < 0)
		ਪਹਿਲਾ = included_patches[i];
	    ਜੇ (i == 0 || included_patches[i - 1] != included_patches[i] + 1)
	    {
		ਸੁਨੇਹਾ_puts(s);
		s = ", ";
		ਸੁਨੇਹਾ_outnum((ਲੰਮਾ)ਪਹਿਲਾ);
		ਜੇ (ਪਹਿਲਾ != included_patches[i])
		{
		    ਸੁਨੇਹਾ_puts("-");
		    ਸੁਨੇਹਾ_outnum((ਲੰਮਾ)included_patches[i]);
		}
		ਪਹਿਲਾ = -1;
	    }
	}
    }

    // Print the list of extra patch descriptions if there is at least one.
    ਜੇ (extra_patches[0] != ਨਿਰਾ)
    {
	ਸੁਨੇਹਾ_puts(_("\nExtra patches: "));
	s = "";
	ਲਈ (i = 0; extra_patches[i] != ਨਿਰਾ; ++i)
	{
	    ਸੁਨੇਹਾ_puts(s);
	    s = ", ";
	    ਸੁਨੇਹਾ_puts(extra_patches[i]);
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MODIFIED_BY
    ਸੁਨੇਹਾ_puts("\n");
    ਸੁਨੇਹਾ_puts(_("Modified by "));
    ਸੁਨੇਹਾ_puts(MODIFIED_BY);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PATHDEF
    ਜੇ (*compiled_user != NUL || *compiled_sys != NUL)
    {
	ਸੁਨੇਹਾ_puts(_("\nCompiled "));
	ਜੇ (*compiled_user != NUL)
	{
	    ਸੁਨੇਹਾ_puts(_("by "));
	    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)compiled_user);
	}
	ਜੇ (*compiled_sys != NUL)
	{
	    ਸੁਨੇਹਾ_puts("@");
	    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)compiled_sys);
	}
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_HUGE)
    ਸੁਨੇਹਾ_puts(_("\nHuge version "));
#ਜਾਂਜੇ defined(FEAT_ਸਾਧਾਰਨ)
    ਸੁਨੇਹਾ_puts(_("\nNormal version "));
#ਨਹੀਂਤਾਂ
    ਸੁਨੇਹਾ_puts(_("\nTiny version "));
#ਜੇ_ਅੰਤ
#ਜੇ !defined(FEAT_GUI)
    ਸੁਨੇਹਾ_puts(_("without GUI."));
#ਜਾਂਜੇ defined(FEAT_GUI_GTK)
# ਜੇ defined(USE_GTK4)
    ਸੁਨੇਹਾ_puts(_("with GTK4 GUI."));
# elif defined(USE_GTK3)
    ਸੁਨੇਹਾ_puts(_("with GTK3 GUI."));
# elif defined(FEAT_GUI_GNOME)
    ਸੁਨੇਹਾ_puts(_("with GTK2-GNOME GUI."));
# ਨਹੀਂਤਾਂ
    ਸੁਨੇਹਾ_puts(_("with GTK2 GUI."));
# endif
#ਜਾਂਜੇ defined(FEAT_GUI_MOTIF)
    ਸੁਨੇਹਾ_puts(_("with X11-Motif GUI."));
#ਜਾਂਜੇ defined(FEAT_GUI_HAIKU)
    ਸੁਨੇਹਾ_puts(_("with Haiku GUI."));
#ਜਾਂਜੇ defined(FEAT_GUI_PHOTON)
    ਸੁਨੇਹਾ_puts(_("with Photon GUI."));
#ਜਾਂਜੇ defined(MSWIN)
    ਸੁਨੇਹਾ_puts(_("with GUI."));
#ਜੇ_ਅੰਤ
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("  Features included (+) or not (-):\n"));

    list_features();
    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ > 0)
	ਸੁਨੇਹਾ_putchar('\n');

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SYS_VIMRC_FILE
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("   system vimrc file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(SYS_VIMRC_FILE);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USR_VIMRC_FILE
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("     user vimrc file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(USR_VIMRC_FILE);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USR_VIMRC_FILE2
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_(" 2nd user vimrc file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(USR_VIMRC_FILE2);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
#ਜੇ_ਅੰਤ
#ਜੇ defined(USR_VIMRC_FILE3) && defined(XDG_VIMRC_FILE)
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_(" 3rd user vimrc file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(USR_VIMRC_FILE3);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_(" 4th user vimrc file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ((ਅੱਖਰ *)(XDG_VIMRC_FILE));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
#ਜਾਂਜੇ defined(USR_VIMRC_FILE3)
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_(" 3rd user vimrc file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(USR_VIMRC_FILE3);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
#ਜਾਂਜੇ defined(XDG_VIMRC_FILE)
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_(" 3rd user vimrc file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ((ਅੱਖਰ *)(XDG_VIMRC_FILE));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USR_EXRC_FILE
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("      user exrc file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(USR_EXRC_FILE);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USR_EXRC_FILE2
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("  2nd user exrc file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(USR_EXRC_FILE2);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
# ifdef SYS_GVIMRC_FILE
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("  system gvimrc file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(SYS_GVIMRC_FILE);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
# endif
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("    user gvimrc file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(USR_GVIMRC_FILE);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
# ifdef USR_GVIMRC_FILE2
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("2nd user gvimrc file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(USR_GVIMRC_FILE2);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
# endif
# ifdef USR_GVIMRC_FILE3
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("3rd user gvimrc file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(USR_GVIMRC_FILE3);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
# endif
#ਜੇ_ਅੰਤ
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("       defaults file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(VIM_DEFAULTS_FILE);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
# ifdef SYS_MENU_FILE
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("    system menu file: \""));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(SYS_MENU_FILE);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PATHDEF
    ਜੇ (*default_vim_dir != NUL)
    {
	ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("  fall-back for $VIM: \""));
	ਸੰਸਕਰਣ_ਸੁਨੇਹਾ((ਅੱਖਰ *)default_vim_dir);
	ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
    }
    ਜੇ (*default_vimruntime_dir != NUL)
    {
	ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_(" f-b for $VIMRUNTIME: \""));
	ਸੰਸਕਰਣ_ਸੁਨੇਹਾ((ਅੱਖਰ *)default_vimruntime_dir);
	ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\"\n");
    }
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("Compilation: "));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ((ਅੱਖਰ *)all_cflags);
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\n");
# ifdef VMS
    ਜੇ (*compiler_ਸੰਸਕਰਣ != NUL)
    {
	ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("Compiler: "));
	ਸੰਸਕਰਣ_ਸੁਨੇਹਾ((ਅੱਖਰ *)compiler_ਸੰਸਕਰਣ);
	ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\n");
    }
# endif
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("Linking: "));
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ((ਅੱਖਰ *)all_lflags);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ("\n");
    ਸੰਸਕਰਣ_ਸੁਨੇਹਾ(_("  DEBUG BUILD"));
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ do_intro_ਸਤਰ(ਪੂਰਨ ਪੰਕਤੀ, char_u *mesg, ਪੂਰਨ ਜੋੜੋ_ਸੰਸਕਰਣ, ਪੂਰਨ attr);
ਸਥਿਰ ਖਾਲੀ intro_ਸੁਨੇਹਾ(ਪੂਰਨ colon);

/*
 * Show the intro message when not editing a file.
 */
    ਖਾਲੀ
maybe_intro_ਸੁਨੇਹਾ(ਖਾਲੀ)
{
    ਜੇ (BUFEMPTY()
	    && curbuf->b_fname == ਨਿਰਾ
	    && firstwin->w_ਅਗਲਾ == ਨਿਰਾ
	    && vim_strchr(p_shm, SHM_INTRO) == ਨਿਰਾ)
	intro_ਸੁਨੇਹਾ(FALSE);
}

/*
 * Give an introductory message about Vim.
 * Only used when starting Vim on an empty file, without a file name.
 * Or with the ":intro" command (for Sven :-).
 */
    ਸਥਿਰ ਖਾਲੀ
intro_ਸੁਨੇਹਾ(
    ਪੂਰਨ		colon)		// TRUE for ":intro"
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਪੰਕਤੀ;
    ਪੂਰਨ		blanklines;
    ਪੂਰਨ		sponsor;
    ਅੱਖਰ	*p;
    ਸਥਿਰ ਅੱਖਰ	*(ਸਤਰਾਂ[]) =
    {
	N_("VIM - Vi IMproved"),
	"",
	N_("version "),
	N_("by Bram Moolenaar et al."),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MODIFIED_BY
	" ",
#ਜੇ_ਅੰਤ
	N_("Vim is open source and freely distributable"),
	"",
	N_("Help poor children in Uganda!"),
	N_("type  :help Kuwasha<Enter>    for information "),
	"",
	N_("type  :q<Enter>               to exit         "),
	N_("type  :help<Enter>  or  <F1>  for on-line help"),
	N_("type  :help version9<Enter>   for version info"),
	ਨਿਰਾ,
	"",
	N_("Running in Vi compatible mode"),
	N_("type  :set nocp<Enter>        for Vim defaults"),
	N_("type  :help cp-default<Enter> for info on this"),
    };
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਸਥਿਰ ਅੱਖਰ	*(gui_ਸਤਰਾਂ[]) =
    {
	ਨਿਰਾ,
	ਨਿਰਾ,
	ਨਿਰਾ,
	ਨਿਰਾ,
# ifdef MODIFIED_BY
	ਨਿਰਾ,
# endif
	ਨਿਰਾ,
	ਨਿਰਾ,
	ਨਿਰਾ,
	N_("menu  Help->Orphans           for information    "),
	ਨਿਰਾ,
	N_("Running modeless, typed text is inserted"),
	N_("menu  Edit->Global Settings->Toggle Insert Mode  "),
	N_("                              for two modes      "),
	ਨਿਰਾ,
	ਨਿਰਾ,
	ਨਿਰਾ,
	N_("menu  Edit->Global Settings->Toggle Vi Compatible"),
	N_("                              for Vim defaults   "),
    };
#ਜੇ_ਅੰਤ

    // blanklines = screen height - # message lines
    blanklines = (ਪੂਰਨ)ਪੰਕਤੀਆਂ - (ARRAY_ਲੰਮਾਈ(ਸਤਰਾਂ) - 1);
    ਜੇ (!p_cp)
	blanklines += 4;  // add 4 for not showing "Vi compatible" message

    // Don't overwrite a statusline.  Depends on 'cmdheight'.
    ਜੇ (p_ls > 1)
	blanklines -= ਪੰਕਤੀਆਂ - topframe->fr_ਉਚਾਈ;
    ਜੇ (blanklines < 0)
	blanklines = 0;

    // Show the sponsor and Uganda message two out of four times
    sponsor = (ਪੂਰਨ)ਸਮਾਂ(ਨਿਰਾ);
    sponsor = ((sponsor & 2) == 0) - ((sponsor & 4) == 0);

    // start displaying the message lines after half of the blank lines
    ਪੰਕਤੀ = blanklines / 2;
    ਜੇ ((ਪੰਕਤੀ >= 2 && topframe->fr_ਚੌੜਾਈ >= 50) || colon)
    {
	ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(ਸਤਰਾਂ); ++i)
	{
	    p = ਸਤਰਾਂ[i];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    ਜੇ (p_im && gui.ਵਿੱਚ_use && gui_ਸਤਰਾਂ[i] != ਨਿਰਾ)
		p = gui_ਸਤਰਾਂ[i];
#ਜੇ_ਅੰਤ
	    ਜੇ (p == ਨਿਰਾ)
	    {
		ਜੇ (!p_cp)
		    ਰੋਕੋ;
		ਜਾਰੀ;
	    }
	    ਜੇ (sponsor != 0)
	    {
		ਜੇ (ਲੜੀ_ਖੋਜ(p, "children") != ਨਿਰਾ)
		    p = N_("Sponsor Vim development!");
		ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਖੋਜ(p, "Kuwasha") != ਨਿਰਾ)
		    p = N_("type  :help sponsor<Enter>    for information ");
		ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਖੋਜ(p, "Orphans") != ਨਿਰਾ)
		    p = N_("menu  Help->Sponsor           for information    ");
	    }
	    ਜੇ (*p != NUL)
		do_intro_ਸਤਰ(ਪੰਕਤੀ, (char_u *)_(p), i == 2, 0);
	    ++ਪੰਕਤੀ;
	}
    }

    // Make the wait-return message appear just below the text.
    ਜੇ (colon)
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀ;
}

    ਸਥਿਰ ਖਾਲੀ
do_intro_ਸਤਰ(
    ਪੂਰਨ		ਪੰਕਤੀ,
    char_u	*mesg,
    ਪੂਰਨ		ਜੋੜੋ_ਸੰਸਕਰਣ,
    ਪੂਰਨ		attr)
{
    char_u	vers[20];
    ਪੂਰਨ		ਖਾਨਾ;
    char_u	*p;
    ਪੂਰਨ		l;
    ਪੂਰਨ		clen;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MODIFIED_BY
# define MODBY_LEN 150
    char_u	modby[MODBY_LEN];

    ਜੇ (*mesg == ' ')
    {
	vim_strncpy(modby, (char_u *)_("Modified by "), MODBY_LEN - 1);
	l = (ਪੂਰਨ)STRLEN(modby);
	vim_strncpy(modby + l, (char_u *)MODIFIED_BY, MODBY_LEN - l - 1);
	mesg = modby;
    }
#ਜੇ_ਅੰਤ

    // Center the message horizontally.
    ਖਾਨਾ = vim_strsize(mesg);
    ਜੇ (ਜੋੜੋ_ਸੰਸਕਰਣ)
    {
	STRCPY(vers, medium_ਸੰਸਕਰਣ);
	ਜੇ (highest_patch())
	{
	    // Check for 9.9x or 9.9xx, alpha/beta version
	    ਜੇ (ਸੁਰੱਖਿਅਤ_isalpha((ਪੂਰਨ)vers[3]))
	    {
		ਪੂਰਨ len = (ਸੁਰੱਖਿਅਤ_isalpha((ਪੂਰਨ)vers[4])) ? 5 : 4;
		sprintf((ਅੱਖਰ *)vers + len, ".%d%s", highest_patch(),
							 medium_ਸੰਸਕਰਣ + len);
	    }
	    ਨਹੀਂਤਾਂ
		sprintf((ਅੱਖਰ *)vers + 3, ".%d", highest_patch());
	}
	ਖਾਨਾ += (ਪੂਰਨ)STRLEN(vers);
    }
    ਖਾਨਾ = (topframe->fr_ਚੌੜਾਈ - ਖਾਨਾ) / 2;
    ਜੇ (ਖਾਨਾ < 0)
	ਖਾਨਾ = 0;

    // Split up in parts to highlight <> items differently.
    ਲਈ (p = mesg; *p != NUL; p += l)
    {
	clen = 0;
	ਲਈ (l = 0; p[l] != NUL
			 && (l == 0 || (p[l] != '<' && p[l - 1] != '>')); ++l)
	{
	    ਜੇ (has_mbyte)
	    {
		clen += ptr2cells(p + l);
		l += (*mb_ptr2len)(p + l) - 1;
	    }
	    ਨਹੀਂਤਾਂ
		clen += byte2cells(p[l]);
	}
	screen_puts_len(p, l, ਪੰਕਤੀ, ਖਾਨਾ + firstwin->w_wincol,
		*p == '<' ? HL_ATTR(HLF_8) : attr);
	ਖਾਨਾ += clen;
    }

    // Add the version number to the version line.
    ਜੇ (ਜੋੜੋ_ਸੰਸਕਰਣ)
	screen_puts(vers, ਪੰਕਤੀ, ਖਾਨਾ + firstwin->w_wincol, 0);
}

/*
 * ":intro": clear screen, display intro screen and wait for return.
 */
    ਖਾਲੀ
ex_intro(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    screenclear();
#ਜੇ defined(FEAT_TABPANEL)
    draw_tabpanel();
#ਜੇ_ਅੰਤ
    intro_ਸੁਨੇਹਾ(TRUE);
    wait_return(TRUE);
}
