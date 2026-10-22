/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

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

#ಅನ್ತರ್ಭಾವ "version.h"

ಅಕ್ಷರ		*ಆವೃತ್ತಿ = VIM_ಆವೃತ್ತಿ_SHORT;
ಸ್ಥಿರ ಅಕ್ಷರ	*medium_ಆವೃತ್ತಿ = VIM_ಆವೃತ್ತಿ_MEDIUM;

#ಆದರೆ defined(HAVE_DATE_ಸಮಯ)
# ಆದರೆ defined(VMS) && defined(VAXC)
ಅಕ್ಷರ	long_ಆವೃತ್ತಿ[ಗಾತ್ರ(VIM_ಆವೃತ್ತಿ_LONG_DATE) + ಗಾತ್ರ(__DATE__)
						      + ಗಾತ್ರ(__TIME__) + 3];

    ಶೂನ್ಯ
init_long_ಆವೃತ್ತಿ(ಶೂನ್ಯ)
{
    /*
     * Construct the long version string.  Necessary because
     * VAX C can't concatenate strings in the preprocessor.
     */
    strcpy(long_ಆವೃತ್ತಿ, VIM_ಆವೃತ್ತಿ_LONG_DATE);
#  ifdef BUILD_DATE
    strcat(long_ಆವೃತ್ತಿ, BUILD_DATE);
#  ಇಲ್ಲದಿದ್ದರೆ
    strcat(long_ಆವೃತ್ತಿ, __DATE__);
    strcat(long_ಆವೃತ್ತಿ, " ");
    strcat(long_ಆವೃತ್ತಿ, __TIME__);
#  endif
    strcat(long_ಆವೃತ್ತಿ, ")");
}

# ಇಲ್ಲದಿದ್ದರೆ
ಅಕ್ಷರ	*long_ಆವೃತ್ತಿ = ಖಾಲಿ;

    ಶೂನ್ಯ
init_long_ಆವೃತ್ತಿ(ಶೂನ್ಯ)
{
    ಆದರೆ (long_ಆವೃತ್ತಿ != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

#  ifdef BUILD_DATE
    ಅಕ್ಷರ *date_ಸಮಯ = BUILD_DATE;
#  ಇಲ್ಲದಿದ್ದರೆ
    ಅಕ್ಷರ *date_ಸಮಯ = __DATE__ " " __TIME__;
#  endif
    ಅಕ್ಷರ *ಸಂದೇಶ = _("%s (%s, compiled %s)");
    size_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಸಂದೇಶ)
	+ STRLEN_LITERAL(VIM_ಆವೃತ್ತಿ_LONG_ONLY)
	+ STRLEN_LITERAL(VIM_ಆವೃತ್ತಿ_DATE_ONLY)
	+ ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(date_ಸಮಯ);

    long_ಆವೃತ್ತಿ = alloc(len);
    ಆದರೆ (long_ಆವೃತ್ತಿ == ಖಾಲಿ)
	long_ಆವೃತ್ತಿ = VIM_ಆವೃತ್ತಿ_LONG;
    ಇಲ್ಲದಿದ್ದರೆ
	vim_snprintf(long_ಆವೃತ್ತಿ, len, ಸಂದೇಶ,
		VIM_ಆವೃತ್ತಿ_LONG_ONLY, VIM_ಆವೃತ್ತಿ_DATE_ONLY, date_ಸಮಯ);
}
# endif
#ಇಲ್ಲದಿದ್ದರೆ
ಅಕ್ಷರ	*long_ಆವೃತ್ತಿ = VIM_ಆವೃತ್ತಿ_LONG;

    ಶೂನ್ಯ
init_long_ಆವೃತ್ತಿ(ಶೂನ್ಯ)
{
    // nothing to do
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಅಕ್ಷರ *(features[]) =
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
	"+acl",
#ಇಲ್ಲದಿದ್ದರೆ
	"-acl",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
	"+arabic",
#ಇಲ್ಲದಿದ್ದರೆ
	"-arabic",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA		// only for Amiga systems
# ifdef FEAT_ARP
	"+ARP",
# ಇಲ್ಲದಿದ್ದರೆ
	"-ARP",
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_AUTOCHDIR
	"+autochdir",
#ಇಲ್ಲದಿದ್ದರೆ
	"-autochdir",
#ಆದರೆ_ಅಂತ್ಯ
	"+autocmd",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_AUTOSERVERNAME
	"+autoservername",
#ಇಲ್ಲದಿದ್ದರೆ
	"-autoservername",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
	"+balloon_eval",
#ಇಲ್ಲದಿದ್ದರೆ
	"-balloon_eval",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_ಪದ
	"+balloon_eval_term",
#ಇಲ್ಲದಿದ್ದರೆ
	"-balloon_eval_term",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
	"+browse",
#ಇಲ್ಲದಿದ್ದರೆ
	"-browse",
#ಆದರೆ_ಅಂತ್ಯ
	"++builtin_terms",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BYTEOFF
	"+byte_offset",
#ಇಲ್ಲದಿದ್ದರೆ
	"-byte_offset",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	"+channel",
#ಇಲ್ಲದಿದ್ದರೆ
	"-channel",
#ಆದರೆ_ಅಂತ್ಯ
	"+cindent",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
	"+clientserver",
#ಇಲ್ಲದಿದ್ದರೆ
	"-clientserver",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	"+clipboard",
#ಇಲ್ಲದಿದ್ದರೆ
	"-clipboard",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
	"+clipboard_provider",
#ಇಲ್ಲದಿದ್ದರೆ
	"-clipboard_provider",
#ಆದರೆ_ಅಂತ್ಯ
	"+cmdline_compl",
	"+cmdline_hist",
	"+cmdline_info",
	"+comments",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	"+conceal",
#ಇಲ್ಲದಿದ್ದರೆ
	"-conceal",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	"+cryptv",
#ಇಲ್ಲದಿದ್ದರೆ
	"-cryptv",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
	"+cscope",
#ಇಲ್ಲದಿದ್ದರೆ
	"-cscope",
#ಆದರೆ_ಅಂತ್ಯ
	"+cursorbind",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
	"+cursorshape",
#ಇಲ್ಲದಿದ್ದರೆ
	"-cursorshape",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
	"+dialog_con_gui",
#ಇಲ್ಲದಿದ್ದರೆ
# ಆದರೆ defined(FEAT_CON_DIALOG)
	"+dialog_con",
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ defined(FEAT_GUI_DIALOG)
	"+dialog_gui",
#  ಇಲ್ಲದಿದ್ದರೆ
	"-dialog",
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	"+diff",
#ಇಲ್ಲದಿದ್ದರೆ
	"-diff",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
	"+digraphs",
#ಇಲ್ಲದಿದ್ದರೆ
	"-digraphs",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
# ifdef FEAT_DIRECTX
	"+directx",
# ಇಲ್ಲದಿದ್ದರೆ
	"-directx",
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
	"+dnd",
#ಇಲ್ಲದಿದ್ದರೆ
	"-dnd",
#ಆದರೆ_ಅಂತ್ಯ
	"-ebcdic",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	"+emacs_tags",
#ಇಲ್ಲದಿದ್ದರೆ
	"-emacs_tags",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	"+eval",
#ಇಲ್ಲದಿದ್ದರೆ
	"-eval",
#ಆದರೆ_ಅಂತ್ಯ
	"+ex_extra",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	"+extra_search",
#ಇಲ್ಲದಿದ್ದರೆ
	"-extra_search",
#ಆದರೆ_ಅಂತ್ಯ
	"-farsi",
	"+file_in_path",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
	"+find_in_path",
#ಇಲ್ಲದಿದ್ದರೆ
	"-find_in_path",
#ಆದರೆ_ಅಂತ್ಯ
	"+float",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	"+folding",
#ಇಲ್ಲದಿದ್ದರೆ
	"-folding",
#ಆದರೆ_ಅಂತ್ಯ
	"-footer",
	// only interesting on Unix systems
#ಆದರೆ !defined(USE_SYSTEM) && defined(UNIX)
	"+fork()",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
	"+gettext/dyn",
# ಇಲ್ಲದಿದ್ದರೆ
	"+gettext",
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	"-gettext",
#ಆದರೆ_ಅಂತ್ಯ
	"-hangul_input",
#ಆದರೆ (defined(HAVE_ICONV_H) && defined(USE_ICONV)) || defined(DYNAMIC_ICONV)
# ifdef DYNAMIC_ICONV
	"+iconv/dyn",
# ಇಲ್ಲದಿದ್ದರೆ
	"+iconv",
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	"-iconv",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
	"+image",
#ಇಲ್ಲದಿದ್ದರೆ
	"-image",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
	"+image_popup",
#ಇಲ್ಲದಿದ್ದರೆ
	"-image_popup",
#ಆದರೆ_ಅಂತ್ಯ
	"+insert_expand",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IPV6
	"+ipv6",
#ಇಲ್ಲದಿದ್ದರೆ
	"-ipv6",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	"+job",
#ಇಲ್ಲದಿದ್ದರೆ
	"-job",
#ಆದರೆ_ಅಂತ್ಯ
	"+jumplist",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
	"+keymap",
#ಇಲ್ಲದಿದ್ದರೆ
	"-keymap",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	"+lambda",
#ಇಲ್ಲದಿದ್ದರೆ
	"-lambda",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
	"+langmap",
#ಇಲ್ಲದಿದ್ದರೆ
	"-langmap",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LIBCALL
	"+libcall",
#ಇಲ್ಲದಿದ್ದರೆ
	"-libcall",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	"+linebreak",
#ಇಲ್ಲದಿದ್ದರೆ
	"-linebreak",
#ಆದರೆ_ಅಂತ್ಯ
	"+lispindent",
	"+listcmds",
	"+localmap",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LUA
# ifdef DYNAMIC_LUA
	"+lua/dyn",
# ಇಲ್ಲದಿದ್ದರೆ
	"+lua",
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	"-lua",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	"+menu",
#ಇಲ್ಲದಿದ್ದರೆ
	"-menu",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
	"+mksession",
#ಇಲ್ಲದಿದ್ದರೆ
	"-mksession",
#ಆದರೆ_ಅಂತ್ಯ
	"+modify_fname",
	"+mouse",
#ಆದರೆ defined(UNIX) || defined(VMS)
# ifdef FEAT_MOUSE_DEC
	"+mouse_dec",
# ಇಲ್ಲದಿದ್ದರೆ
	"-mouse_dec",
# endif
# ifdef FEAT_MOUSE_GPM
#  ifdef DYNAMIC_GPM
	"+mouse_gpm/dyn",
#  ಇಲ್ಲದಿದ್ದರೆ
	"+mouse_gpm",
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
	"-mouse_gpm",
# endif
# ifdef FEAT_MOUSE_JSB
	"+mouse_jsbterm",
# ಇಲ್ಲದಿದ್ದರೆ
	"-mouse_jsbterm",
# endif
# ifdef FEAT_MOUSE_NET
	"+mouse_netterm",
# ಇಲ್ಲದಿದ್ದರೆ
	"-mouse_netterm",
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __QNX__
# ifdef FEAT_MOUSE_PTERM
	"+mouse_pterm",
# ಇಲ್ಲದಿದ್ದರೆ
	"-mouse_pterm",
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(UNIX) || defined(VMS)
	"+mouse_sgr",
# ifdef FEAT_SYSMOUSE
	"+mouse_sysmouse",
# ಇಲ್ಲದಿದ್ದರೆ
	"-mouse_sysmouse",
# endif
# ifdef FEAT_MOUSE_URXVT
	"+mouse_urxvt",
# ಇಲ್ಲದಿದ್ದರೆ
	"-mouse_urxvt",
# endif
	"+mouse_xterm",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
	"+mouseshape",
#ಇಲ್ಲದಿದ್ದರೆ
	"-mouseshape",
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
# ifdef DYNAMIC_IME
	"+multi_byte_ime/dyn",
# ಇಲ್ಲದಿದ್ದರೆ
	"+multi_byte_ime",
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	"+multi_byte",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	"+multi_lang",
#ಇಲ್ಲದಿದ್ದರೆ
	"-multi_lang",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
# ifdef DYNAMIC_MZSCHEME
	"+mzscheme/dyn",
# ಇಲ್ಲದಿದ್ದರೆ
	"+mzscheme",
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	"-mzscheme",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	"+netbeans_intg",
#ಇಲ್ಲದಿದ್ದರೆ
	"-netbeans_intg",
#ಆದರೆ_ಅಂತ್ಯ
	"+num64",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
# ifdef FEAT_OLE
	"+ole",
# ಇಲ್ಲದಿದ್ದರೆ
	"-ole",
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	"+packages",
#ಇಲ್ಲದಿದ್ದರೆ
	"-packages",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINT_PANGO
	"+pango",
#ಇಲ್ಲದಿದ್ದರೆ
	"-pango",
#ಆದರೆ_ಅಂತ್ಯ
	"+path_extra",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERL
# ifdef DYNAMIC_PERL
	"+perl/dyn",
# ಇಲ್ಲದಿದ್ದರೆ
	"+perl",
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	"-perl",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
	"+persistent_undo",
#ಇಲ್ಲದಿದ್ದರೆ
	"-persistent_undo",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	"+popupwin",
#ಇಲ್ಲದಿದ್ದರೆ
	"-popupwin",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINTER
# ifdef FEAT_POSTSCRIPT
	"+postscript",
# ಇಲ್ಲದಿದ್ದರೆ
	"-postscript",
# endif
	"+printer",
#ಇಲ್ಲದಿದ್ದರೆ
	"-printer",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	"+profile",
#ಇಲ್ಲದಿದ್ದರೆ
	"-profile",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
# ifdef DYNAMIC_PYTHON
	"+python/dyn",
# ಇಲ್ಲದಿದ್ದರೆ
	"+python",
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	"-python",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
# ifdef DYNAMIC_PYTHON3
#  ifdef DYNAMIC_PYTHON3_STABLE_ABI
	"+python3/dyn-stable",
#  ಇಲ್ಲದಿದ್ದರೆ
	"+python3/dyn",
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
	"+python3",
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	"-python3",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	"+quickfix",
#ಇಲ್ಲದಿದ್ದರೆ
	"-quickfix",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
	"+reltime",
#ಇಲ್ಲದಿದ್ದರೆ
	"-reltime",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	"+rightleft",
#ಇಲ್ಲದಿದ್ದರೆ
	"-rightleft",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RUBY
# ifdef DYNAMIC_RUBY
	"+ruby/dyn",
# ಇಲ್ಲದಿದ್ದರೆ
	"+ruby",
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	"-ruby",
#ಆದರೆ_ಅಂತ್ಯ
	"+scrollbind",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	"+signs",
#ಇಲ್ಲದಿದ್ದರೆ
	"-signs",
#ಆದರೆ_ಅಂತ್ಯ
	"+smartindent",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER
	"+socketserver",
#ಇಲ್ಲದಿದ್ದರೆ
	"-socketserver",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SODIUM
# ifdef DYNAMIC_SODIUM
	"+sodium/dyn",
# ಇಲ್ಲದಿದ್ದರೆ
	"+sodium",
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	"-sodium",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOUND
	"+sound",
#ಇಲ್ಲದಿದ್ದರೆ
	"-sound",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	"+spell",
#ಇಲ್ಲದಿದ್ದರೆ
	"-spell",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ STARTUPTIME
	"+startuptime",
#ಇಲ್ಲದಿದ್ದರೆ
	"-startuptime",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
	"+statusline",
#ಇಲ್ಲದಿದ್ದರೆ
	"-statusline",
#ಆದರೆ_ಅಂತ್ಯ
	"-sun_workshop",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	"+syntax",
#ಇಲ್ಲದಿದ್ದರೆ
	"-syntax",
#ಆದರೆ_ಅಂತ್ಯ
	    // only interesting on Unix systems
#ಆದರೆ defined(USE_SYSTEM) && defined(UNIX)
	"+system()",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_TABPANEL)
	"+tabpanel",
#ಇಲ್ಲದಿದ್ದರೆ
	"-tabpanel",
#ಆದರೆ_ಅಂತ್ಯ
	"-tag_any_white",
	"+tag_binary",
	"-tag_old_static",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TCL
# ifdef DYNAMIC_TCL
	"+tcl/dyn",
# ಇಲ್ಲದಿದ್ದರೆ
	"+tcl",
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	"-tcl",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	"+termguicolors",
#ಇಲ್ಲದಿದ್ದರೆ
	"-termguicolors",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	"+terminal",
#ಇಲ್ಲದಿದ್ದರೆ
	"-terminal",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(UNIX)
// only Unix can have terminfo instead of termcap
# ifdef TERMINFO
	"+terminfo",
# ಇಲ್ಲದಿದ್ದರೆ
	"-terminfo",
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
	"+termresponse",
#ಇಲ್ಲದಿದ್ದರೆ
	"-termresponse",
#ಆದರೆ_ಅಂತ್ಯ
	"+textobjects",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	"+textprop",
#ಇಲ್ಲದಿದ್ದರೆ
	"-textprop",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(UNIX)
// unix always includes termcap support
# ifdef HAVE_TGETENT
	"+tgetent",
# ಇಲ್ಲದಿದ್ದರೆ
	"-tgetent",
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
	"+timers",
#ಇಲ್ಲದಿದ್ದರೆ
	"-timers",
#ಆದರೆ_ಅಂತ್ಯ
	"+title",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
	"+toolbar",
#ಇಲ್ಲದಿದ್ದರೆ
	"-toolbar",
#ಆದರೆ_ಅಂತ್ಯ
	"+user_commands",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	"+vartabs",
#ಇಲ್ಲದಿದ್ದರೆ
	"-vartabs",
#ಆದರೆ_ಅಂತ್ಯ
	"+vertsplit",
	"+vim9script",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
	"+viminfo",
#ಇಲ್ಲದಿದ್ದರೆ
	"-viminfo",
#ಆದರೆ_ಅಂತ್ಯ
	"+virtualedit",
	"+visual",
	"+visualextra",
	"+vreplace",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
# ifdef FEAT_VTP
	"+vtp",
# ಇಲ್ಲದಿದ್ದರೆ
	"-vtp",
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND
	"+wayland",
#ಇಲ್ಲದಿದ್ದರೆ
	"-wayland",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND_CLIPBOARD
	"+wayland_clipboard",
#ಇಲ್ಲದಿದ್ದರೆ
	"-wayland_clipboard",
#ಆದರೆ_ಅಂತ್ಯ
	"+wildignore",
	"+wildmenu",
	"+windows",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WRITEBACKUP
	"+writebackup",
#ಇಲ್ಲದಿದ್ದರೆ
	"-writebackup",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(UNIX) || defined(VMS)
# ifdef FEAT_X11
	"+X11",
# ಇಲ್ಲದಿದ್ದರೆ
	"-X11",
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XATTR
	"+xattr",
#ಇಲ್ಲದಿದ್ದರೆ
	"-xattr",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
	"+xfontset",
#ಇಲ್ಲದಿದ್ದರೆ
	"-xfontset",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
	"+xim",
#ಇಲ್ಲದಿದ್ದರೆ
	"-xim",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(MSWIN)
# ifdef FEAT_XPM_W32
	"+xpm_w32",
# ಇಲ್ಲದಿದ್ದರೆ
	"-xpm_w32",
# endif
#ಅಥವಾಆದರೆ defined(HAVE_XPM)
	"+xpm",
#ಇಲ್ಲದಿದ್ದರೆ
	"-xpm",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(UNIX) || defined(VMS)
# ಆದರೆ defined(USE_XSMP_INTERACT)
	"+xsmp_interact",
# elif defined(USE_XSMP)
	"+xsmp",
# ಇಲ್ಲದಿದ್ದರೆ
	"-xsmp",
# endif
# ifdef FEAT_XCLIPBOARD
	"+xterm_clipboard",
# ಇಲ್ಲದಿದ್ದರೆ
	"-xterm_clipboard",
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XTERM_SAVE
	"+xterm_save",
#ಇಲ್ಲದಿದ್ದರೆ
	"-xterm_save",
#ಆದರೆ_ಅಂತ್ಯ
	ಖಾಲಿ
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ included_patches[] =
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
ಸ್ಥಿರ ಅಕ್ಷರ *(extra_patches[]) =
{   /* Add your patch description below this line */
/**/
    ಖಾಲಿ
};

    ಪೂರ್ಣಾಂಕ
highest_patch(ಶೂನ್ಯ)
{
    // this relies on the highest patch number to be the first entry
    ಹಿಂತಿರುಗಿ included_patches[0];
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return TRUE if patch "n" has been included.
 */
    ಪೂರ್ಣಾಂಕ
has_patch(ಪೂರ್ಣಾಂಕ n)
{
    ಪೂರ್ಣಾಂಕ		h, m, l;

    // Perform a binary search.
    l = 0;
    h = (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(included_patches) - 1;
    ಗಾಗಿ (;;)
    {
	m = (l + h) / 2;
	ಆದರೆ (included_patches[m] == n)
	    ಹಿಂತಿರುಗಿ TRUE;
	ಆದರೆ (l == h)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (included_patches[m] < n)
	    h = m;
	ಇಲ್ಲದಿದ್ದರೆ
	    l = m + 1;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
ex_ಆವೃತ್ತಿ(exarg_T *eap)
{
    /*
     * Ignore a ":version 9.99" command.
     */
    ಆದರೆ (*eap->arg == NUL)
    {
	ಸಂದೇಶ_putchar('\n');
	list_ಆವೃತ್ತಿ();
    }
}

/*
 * Output a string for the version message.  If it's going to wrap, output a
 * newline, unless the message is too long to fit on the screen anyway.
 * When "wrap" is TRUE wrap the string in [].
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಆವೃತ್ತಿ_ಸಂದೇಶ_wrap(char_u *s, ಪೂರ್ಣಾಂಕ wrap)
{
    ಪೂರ್ಣಾಂಕ		len = vim_strsize(s) + (wrap ? 2 : 0);

    ಆದರೆ (!got_int && len < cmdline_ಅಗಲ && ಸಂದೇಶ_ಲಂಬಸಾಲು + len >= cmdline_ಅಗಲ
								&& *s != '\n')
	ಸಂದೇಶ_putchar('\n');
    ಆದರೆ (!got_int)
    {
	ಆದರೆ (wrap)
	    ಸಂದೇಶ_puts("[");
	ಸಂದೇಶ_puts((ಅಕ್ಷರ *)s);
	ಆದರೆ (wrap)
	    ಸಂದೇಶ_puts("]");
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಆವೃತ್ತಿ_ಸಂದೇಶ(ಅಕ್ಷರ *s)
{
    ಆವೃತ್ತಿ_ಸಂದೇಶ_wrap((char_u *)s, FALSE);
}

/*
 * List all features aligned in columns, dictionary style.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_features(ಶೂನ್ಯ)
{
    list_ಒಳ_columns((char_u **)features, -1, -1);
}

/*
 * List string items nicely aligned in columns.
 * When "size" is < 0 then the last entry is marked with NULL.
 * The entry with index "current" is inclosed in [].
 */
    ಶೂನ್ಯ
list_ಒಳ_columns(char_u **ವಸ್ತುಗಳು, ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2, ಪೂರ್ಣಾಂಕ current)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ncol;
    ಪೂರ್ಣಾಂಕ		nrow;
    ಪೂರ್ಣಾಂಕ		cur_ಅಡ್ಡಸಾಲು = 1;
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಪೂರ್ಣಾಂಕ		use_highlight = (ವಸ್ತುಗಳು == (char_u **)features);
#ಆದರೆ_ಅಂತ್ಯ

    // Find the length of the longest item, use that + 1 as the column
    // width.
    ಗಾಗಿ (i = 0; ಗಾತ್ರ_2 < 0 ? ವಸ್ತುಗಳು[i] != ಖಾಲಿ : i < ಗಾತ್ರ_2; ++i)
    {
	ಪೂರ್ಣಾಂಕ l = vim_strsize(ವಸ್ತುಗಳು[i]) + (i == current ? 2 : 0);

	ಆದರೆ (l > ಅಗಲ)
	    ಅಗಲ = l;
	++ಅಂಶ_ಎಣಿಕೆ;
    }
    ಅಗಲ += 1;

    ಆದರೆ (cmdline_ಅಗಲ < ಅಗಲ)
    {
	// Not enough screen columns - show one per line
	ಗಾಗಿ (i = 0; i < ಅಂಶ_ಎಣಿಕೆ; ++i)
	{
	    ಆವೃತ್ತಿ_ಸಂದೇಶ_wrap(ವಸ್ತುಗಳು[i], i == current);
	    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು > 0 && i < ಅಂಶ_ಎಣಿಕೆ - 1)
		ಸಂದೇಶ_putchar('\n');
	}
	ಹಿಂತಿರುಗಿ;
    }

    // The rightmost column doesn't need a separator.
    // Sacrifice it to fit in one more column if possible.
    ncol = (cmdline_ಅಗಲ + 1) / ಅಗಲ;
    nrow = ಅಂಶ_ಎಣಿಕೆ / ncol + ((ಅಂಶ_ಎಣಿಕೆ % ncol) ? 1 : 0);

    // "i" counts columns then rows.  "idx" counts rows then columns.
    ಗಾಗಿ (i = 0; !got_int && i < nrow * ncol; ++i)
    {
	ಪೂರ್ಣಾಂಕ idx = (i / ncol) + (i % ncol) * nrow;

	ಆದರೆ (idx < ಅಂಶ_ಎಣಿಕೆ)
	{
	    ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_ಲಂಬಸಾಲು = (i + 1) % ncol == 0;

	    ಆದರೆ (idx == current)
		ಸಂದೇಶ_putchar('[');
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಆದರೆ (use_highlight && ವಸ್ತುಗಳು[idx][0] == '-')
		ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)ವಸ್ತುಗಳು[idx], HL_ATTR(HLF_W));
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಸಂದೇಶ_puts((ಅಕ್ಷರ *)ವಸ್ತುಗಳು[idx]);
	    ಆದರೆ (idx == current)
		ಸಂದೇಶ_putchar(']');
	    ಆದರೆ (ಕೊನೆಯ_ಲಂಬಸಾಲು)
	    {
		ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು > 0 && cur_ಅಡ್ಡಸಾಲು < nrow)
		    ಸಂದೇಶ_putchar('\n');
		++cur_ಅಡ್ಡಸಾಲು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆಗ (ಸಂದೇಶ_ಲಂಬಸಾಲು % ಅಗಲ)
		    ಸಂದೇಶ_putchar(' ');
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // this row is out of items, thus at the end of the row
	    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು > 0)
	    {
		ಆದರೆ (cur_ಅಡ್ಡಸಾಲು < nrow)
		    ಸಂದೇಶ_putchar('\n');
		++cur_ಅಡ್ಡಸಾಲು;
	    }
	}
    }
}

    ಶೂನ್ಯ
list_ಆವೃತ್ತಿ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ;
    ಅಕ್ಷರ	*s = "";

    /*
     * When adding features here, don't forget to update the list of
     * internal variables in eval.c!
     */
    init_long_ಆವೃತ್ತಿ();
    ಸಂದೇಶ(long_ಆವೃತ್ತಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
#   ifdef _WIN64
#    ಆದರೆ defined(_M_ARM64) || defined(_M_ARM64EC)
     ಸಂದೇಶ_puts(_("\nMS-Windows ARM64 GUI/console version"));
#    ಇಲ್ಲದಿದ್ದರೆ
     ಸಂದೇಶ_puts(_("\nMS-Windows 64-bit GUI/console version"));
#    endif
#   ಇಲ್ಲದಿದ್ದರೆ
    ಸಂದೇಶ_puts(_("\nMS-Windows 32-bit GUI/console version"));
#   endif
#  ಇಲ್ಲದಿದ್ದರೆ
#   ifdef _WIN64
#    ಆದರೆ defined(_M_ARM64) || defined(_M_ARM64EC)
     ಸಂದೇಶ_puts(_("\nMS-Windows ARM64 GUI version"));
#    ಇಲ್ಲದಿದ್ದರೆ
     ಸಂದೇಶ_puts(_("\nMS-Windows 64-bit GUI version"));
#    endif
#   ಇಲ್ಲದಿದ್ದರೆ
    ಸಂದೇಶ_puts(_("\nMS-Windows 32-bit GUI version"));
#   endif
#  endif
#  ifdef FEAT_OLE
    ಸಂದೇಶ_puts(_(" with OLE support"));
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef _WIN64
#   ಆದರೆ defined(_M_ARM64) || defined(_M_ARM64EC)
    ಸಂದೇಶ_puts(_("\nMS-Windows ARM64 console version"));
#   ಇಲ್ಲದಿದ್ದರೆ
    ಸಂದೇಶ_puts(_("\nMS-Windows 64-bit console version"));
#   endif
#  ಇಲ್ಲದಿದ್ದರೆ
    ಸಂದೇಶ_puts(_("\nMS-Windows 32-bit console version"));
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(MACOS_X)
# ಆದರೆ defined(MACOS_X_DARWIN)
    ಸಂದೇಶ_puts(_("\nmacOS version"));
# ಇಲ್ಲದಿದ್ದರೆ
    ಸಂದೇಶ_puts(_("\nmacOS version w/o darwin feat."));
# endif
# ಆದರೆ defined(__arm64__)
    ಸಂದೇಶ_puts(" - arm64");
# elif defined(__x86_64__)
    ಸಂದೇಶ_puts(" - x86_64");
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    ಸಂದೇಶ_puts(_("\nOpenVMS (build) arch, version"));
# ifdef HAVE_PATHDEF
    ಆದರೆ (*compiled_arch != NUL)
    {
	ಸಂದೇಶ_puts(": ");
	ಸಂದೇಶ_puts((ಅಕ್ಷರ *)compiled_arch);
	ಆದರೆ (*compiled_vers != NUL)
	{
	    ಸಂದೇಶ_puts(", ");
	    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)compiled_vers);
	}
    }

# endif

#ಆದರೆ_ಅಂತ್ಯ

    // Print the list of patch numbers if there is at least one.
    // Print a range when patches are consecutive: "1-10, 12, 15-40, 42-45"
    ಆದರೆ (included_patches[0] != 0)
    {
	ಸಂದೇಶ_puts(_("\nIncluded patches: "));
	ಮೊದಲ = -1;
	i = (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(included_patches) - 1;
	ಆಗ (--i >= 0)
	{
	    ಆದರೆ (ಮೊದಲ < 0)
		ಮೊದಲ = included_patches[i];
	    ಆದರೆ (i == 0 || included_patches[i - 1] != included_patches[i] + 1)
	    {
		ಸಂದೇಶ_puts(s);
		s = ", ";
		ಸಂದೇಶ_outnum((ದೀರ್ಘ)ಮೊದಲ);
		ಆದರೆ (ಮೊದಲ != included_patches[i])
		{
		    ಸಂದೇಶ_puts("-");
		    ಸಂದೇಶ_outnum((ದೀರ್ಘ)included_patches[i]);
		}
		ಮೊದಲ = -1;
	    }
	}
    }

    // Print the list of extra patch descriptions if there is at least one.
    ಆದರೆ (extra_patches[0] != ಖಾಲಿ)
    {
	ಸಂದೇಶ_puts(_("\nExtra patches: "));
	s = "";
	ಗಾಗಿ (i = 0; extra_patches[i] != ಖಾಲಿ; ++i)
	{
	    ಸಂದೇಶ_puts(s);
	    s = ", ";
	    ಸಂದೇಶ_puts(extra_patches[i]);
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MODIFIED_BY
    ಸಂದೇಶ_puts("\n");
    ಸಂದೇಶ_puts(_("Modified by "));
    ಸಂದೇಶ_puts(MODIFIED_BY);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PATHDEF
    ಆದರೆ (*compiled_user != NUL || *compiled_sys != NUL)
    {
	ಸಂದೇಶ_puts(_("\nCompiled "));
	ಆದರೆ (*compiled_user != NUL)
	{
	    ಸಂದೇಶ_puts(_("by "));
	    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)compiled_user);
	}
	ಆದರೆ (*compiled_sys != NUL)
	{
	    ಸಂದೇಶ_puts("@");
	    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)compiled_sys);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_HUGE)
    ಸಂದೇಶ_puts(_("\nHuge version "));
#ಅಥವಾಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ)
    ಸಂದೇಶ_puts(_("\nNormal version "));
#ಇಲ್ಲದಿದ್ದರೆ
    ಸಂದೇಶ_puts(_("\nTiny version "));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_GUI)
    ಸಂದೇಶ_puts(_("without GUI."));
#ಅಥವಾಆದರೆ defined(FEAT_GUI_GTK)
# ಆದರೆ defined(USE_GTK4)
    ಸಂದೇಶ_puts(_("with GTK4 GUI."));
# elif defined(USE_GTK3)
    ಸಂದೇಶ_puts(_("with GTK3 GUI."));
# elif defined(FEAT_GUI_GNOME)
    ಸಂದೇಶ_puts(_("with GTK2-GNOME GUI."));
# ಇಲ್ಲದಿದ್ದರೆ
    ಸಂದೇಶ_puts(_("with GTK2 GUI."));
# endif
#ಅಥವಾಆದರೆ defined(FEAT_GUI_MOTIF)
    ಸಂದೇಶ_puts(_("with X11-Motif GUI."));
#ಅಥವಾಆದರೆ defined(FEAT_GUI_HAIKU)
    ಸಂದೇಶ_puts(_("with Haiku GUI."));
#ಅಥವಾಆದರೆ defined(FEAT_GUI_PHOTON)
    ಸಂದೇಶ_puts(_("with Photon GUI."));
#ಅಥವಾಆದರೆ defined(MSWIN)
    ಸಂದೇಶ_puts(_("with GUI."));
#ಆದರೆ_ಅಂತ್ಯ
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("  Features included (+) or not (-):\n"));

    list_features();
    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು > 0)
	ಸಂದೇಶ_putchar('\n');

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SYS_VIMRC_FILE
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("   system vimrc file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ(SYS_VIMRC_FILE);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USR_VIMRC_FILE
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("     user vimrc file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ(USR_VIMRC_FILE);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USR_VIMRC_FILE2
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_(" 2nd user vimrc file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ(USR_VIMRC_FILE2);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USR_VIMRC_FILE3) && defined(XDG_VIMRC_FILE)
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_(" 3rd user vimrc file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ(USR_VIMRC_FILE3);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_(" 4th user vimrc file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ((ಅಕ್ಷರ *)(XDG_VIMRC_FILE));
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
#ಅಥವಾಆದರೆ defined(USR_VIMRC_FILE3)
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_(" 3rd user vimrc file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ(USR_VIMRC_FILE3);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
#ಅಥವಾಆದರೆ defined(XDG_VIMRC_FILE)
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_(" 3rd user vimrc file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ((ಅಕ್ಷರ *)(XDG_VIMRC_FILE));
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USR_EXRC_FILE
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("      user exrc file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ(USR_EXRC_FILE);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USR_EXRC_FILE2
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("  2nd user exrc file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ(USR_EXRC_FILE2);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
# ifdef SYS_GVIMRC_FILE
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("  system gvimrc file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ(SYS_GVIMRC_FILE);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
# endif
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("    user gvimrc file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ(USR_GVIMRC_FILE);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
# ifdef USR_GVIMRC_FILE2
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("2nd user gvimrc file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ(USR_GVIMRC_FILE2);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
# endif
# ifdef USR_GVIMRC_FILE3
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("3rd user gvimrc file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ(USR_GVIMRC_FILE3);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
# endif
#ಆದರೆ_ಅಂತ್ಯ
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("       defaults file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ(VIM_DEFAULTS_FILE);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
# ifdef SYS_MENU_FILE
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("    system menu file: \""));
    ಆವೃತ್ತಿ_ಸಂದೇಶ(SYS_MENU_FILE);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PATHDEF
    ಆದರೆ (*default_vim_dir != NUL)
    {
	ಆವೃತ್ತಿ_ಸಂದೇಶ(_("  fall-back for $VIM: \""));
	ಆವೃತ್ತಿ_ಸಂದೇಶ((ಅಕ್ಷರ *)default_vim_dir);
	ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
    }
    ಆದರೆ (*default_vimruntime_dir != NUL)
    {
	ಆವೃತ್ತಿ_ಸಂದೇಶ(_(" f-b for $VIMRUNTIME: \""));
	ಆವೃತ್ತಿ_ಸಂದೇಶ((ಅಕ್ಷರ *)default_vimruntime_dir);
	ಆವೃತ್ತಿ_ಸಂದೇಶ("\"\n");
    }
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("Compilation: "));
    ಆವೃತ್ತಿ_ಸಂದೇಶ((ಅಕ್ಷರ *)all_cflags);
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\n");
# ifdef VMS
    ಆದರೆ (*compiler_ಆವೃತ್ತಿ != NUL)
    {
	ಆವೃತ್ತಿ_ಸಂದೇಶ(_("Compiler: "));
	ಆವೃತ್ತಿ_ಸಂದೇಶ((ಅಕ್ಷರ *)compiler_ಆವೃತ್ತಿ);
	ಆವೃತ್ತಿ_ಸಂದೇಶ("\n");
    }
# endif
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("Linking: "));
    ಆವೃತ್ತಿ_ಸಂದೇಶ((ಅಕ್ಷರ *)all_lflags);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    ಆವೃತ್ತಿ_ಸಂದೇಶ("\n");
    ಆವೃತ್ತಿ_ಸಂದೇಶ(_("  DEBUG BUILD"));
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ do_intro_ಸಾಲು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, char_u *mesg, ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_ಆವೃತ್ತಿ, ಪೂರ್ಣಾಂಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ intro_ಸಂದೇಶ(ಪೂರ್ಣಾಂಕ colon);

/*
 * Show the intro message when not editing a file.
 */
    ಶೂನ್ಯ
maybe_intro_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    ಆದರೆ (BUFEMPTY()
	    && curbuf->b_fname == ಖಾಲಿ
	    && firstwin->w_ಮುಂದಿನ == ಖಾಲಿ
	    && vim_strchr(p_shm, SHM_INTRO) == ಖಾಲಿ)
	intro_ಸಂದೇಶ(FALSE);
}

/*
 * Give an introductory message about Vim.
 * Only used when starting Vim on an empty file, without a file name.
 * Or with the ":intro" command (for Sven :-).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
intro_ಸಂದೇಶ(
    ಪೂರ್ಣಾಂಕ		colon)		// TRUE for ":intro"
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		blanklines;
    ಪೂರ್ಣಾಂಕ		sponsor;
    ಅಕ್ಷರ	*p;
    ಸ್ಥಿರ ಅಕ್ಷರ	*(ಸಾಲುಗಳು[]) =
    {
	N_("VIM - Vi IMproved"),
	"",
	N_("version "),
	N_("by Bram Moolenaar et al."),
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MODIFIED_BY
	" ",
#ಆದರೆ_ಅಂತ್ಯ
	N_("Vim is open source and freely distributable"),
	"",
	N_("Help poor children in Uganda!"),
	N_("type  :help Kuwasha<Enter>    for information "),
	"",
	N_("type  :q<Enter>               to exit         "),
	N_("type  :help<Enter>  or  <F1>  for on-line help"),
	N_("type  :help version9<Enter>   for version info"),
	ಖಾಲಿ,
	"",
	N_("Running in Vi compatible mode"),
	N_("type  :set nocp<Enter>        for Vim defaults"),
	N_("type  :help cp-default<Enter> for info on this"),
    };
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಸ್ಥಿರ ಅಕ್ಷರ	*(gui_ಸಾಲುಗಳು[]) =
    {
	ಖಾಲಿ,
	ಖಾಲಿ,
	ಖಾಲಿ,
	ಖಾಲಿ,
# ifdef MODIFIED_BY
	ಖಾಲಿ,
# endif
	ಖಾಲಿ,
	ಖಾಲಿ,
	ಖಾಲಿ,
	N_("menu  Help->Orphans           for information    "),
	ಖಾಲಿ,
	N_("Running modeless, typed text is inserted"),
	N_("menu  Edit->Global Settings->Toggle Insert Mode  "),
	N_("                              for two modes      "),
	ಖಾಲಿ,
	ಖಾಲಿ,
	ಖಾಲಿ,
	N_("menu  Edit->Global Settings->Toggle Vi Compatible"),
	N_("                              for Vim defaults   "),
    };
#ಆದರೆ_ಅಂತ್ಯ

    // blanklines = screen height - # message lines
    blanklines = (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - (ARRAY_ಉದ್ದಳತೆ(ಸಾಲುಗಳು) - 1);
    ಆದರೆ (!p_cp)
	blanklines += 4;  // add 4 for not showing "Vi compatible" message

    // Don't overwrite a statusline.  Depends on 'cmdheight'.
    ಆದರೆ (p_ls > 1)
	blanklines -= ಅಡ್ಡಸಾಲುಗಳು - topframe->fr_ಎತ್ತರ;
    ಆದರೆ (blanklines < 0)
	blanklines = 0;

    // Show the sponsor and Uganda message two out of four times
    sponsor = (ಪೂರ್ಣಾಂಕ)ಸಮಯ(ಖಾಲಿ);
    sponsor = ((sponsor & 2) == 0) - ((sponsor & 4) == 0);

    // start displaying the message lines after half of the blank lines
    ಅಡ್ಡಸಾಲು = blanklines / 2;
    ಆದರೆ ((ಅಡ್ಡಸಾಲು >= 2 && topframe->fr_ಅಗಲ >= 50) || colon)
    {
	ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(ಸಾಲುಗಳು); ++i)
	{
	    p = ಸಾಲುಗಳು[i];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    ಆದರೆ (p_im && gui.ಒಳ_use && gui_ಸಾಲುಗಳು[i] != ಖಾಲಿ)
		p = gui_ಸಾಲುಗಳು[i];
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (p == ಖಾಲಿ)
	    {
		ಆದರೆ (!p_cp)
		    ನಿಲ್ಲಿಸು;
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (sponsor != 0)
	    {
		ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(p, "children") != ಖಾಲಿ)
		    p = N_("Sponsor Vim development!");
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(p, "Kuwasha") != ಖಾಲಿ)
		    p = N_("type  :help sponsor<Enter>    for information ");
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(p, "Orphans") != ಖಾಲಿ)
		    p = N_("menu  Help->Sponsor           for information    ");
	    }
	    ಆದರೆ (*p != NUL)
		do_intro_ಸಾಲು(ಅಡ್ಡಸಾಲು, (char_u *)_(p), i == 2, 0);
	    ++ಅಡ್ಡಸಾಲು;
	}
    }

    // Make the wait-return message appear just below the text.
    ಆದರೆ (colon)
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_intro_ಸಾಲು(
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    char_u	*mesg,
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_ಆವೃತ್ತಿ,
    ಪೂರ್ಣಾಂಕ		attr)
{
    char_u	vers[20];
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		l;
    ಪೂರ್ಣಾಂಕ		clen;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MODIFIED_BY
# define MODBY_LEN 150
    char_u	modby[MODBY_LEN];

    ಆದರೆ (*mesg == ' ')
    {
	vim_strncpy(modby, (char_u *)_("Modified by "), MODBY_LEN - 1);
	l = (ಪೂರ್ಣಾಂಕ)STRLEN(modby);
	vim_strncpy(modby + l, (char_u *)MODIFIED_BY, MODBY_LEN - l - 1);
	mesg = modby;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Center the message horizontally.
    ಲಂಬಸಾಲು = vim_strsize(mesg);
    ಆದರೆ (ಕೂಡಿಸು_ಆವೃತ್ತಿ)
    {
	STRCPY(vers, medium_ಆವೃತ್ತಿ);
	ಆದರೆ (highest_patch())
	{
	    // Check for 9.9x or 9.9xx, alpha/beta version
	    ಆದರೆ (ಸುರಕ್ಷಿತ_isalpha((ಪೂರ್ಣಾಂಕ)vers[3]))
	    {
		ಪೂರ್ಣಾಂಕ len = (ಸುರಕ್ಷಿತ_isalpha((ಪೂರ್ಣಾಂಕ)vers[4])) ? 5 : 4;
		sprintf((ಅಕ್ಷರ *)vers + len, ".%d%s", highest_patch(),
							 medium_ಆವೃತ್ತಿ + len);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		sprintf((ಅಕ್ಷರ *)vers + 3, ".%d", highest_patch());
	}
	ಲಂಬಸಾಲು += (ಪೂರ್ಣಾಂಕ)STRLEN(vers);
    }
    ಲಂಬಸಾಲು = (topframe->fr_ಅಗಲ - ಲಂಬಸಾಲು) / 2;
    ಆದರೆ (ಲಂಬಸಾಲು < 0)
	ಲಂಬಸಾಲು = 0;

    // Split up in parts to highlight <> items differently.
    ಗಾಗಿ (p = mesg; *p != NUL; p += l)
    {
	clen = 0;
	ಗಾಗಿ (l = 0; p[l] != NUL
			 && (l == 0 || (p[l] != '<' && p[l - 1] != '>')); ++l)
	{
	    ಆದರೆ (has_mbyte)
	    {
		clen += ptr2cells(p + l);
		l += (*mb_ptr2len)(p + l) - 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		clen += byte2cells(p[l]);
	}
	screen_puts_len(p, l, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + firstwin->w_wincol,
		*p == '<' ? HL_ATTR(HLF_8) : attr);
	ಲಂಬಸಾಲು += clen;
    }

    // Add the version number to the version line.
    ಆದರೆ (ಕೂಡಿಸು_ಆವೃತ್ತಿ)
	screen_puts(vers, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + firstwin->w_wincol, 0);
}

/*
 * ":intro": clear screen, display intro screen and wait for return.
 */
    ಶೂನ್ಯ
ex_intro(exarg_T *eap ಬಳಸದ)
{
    screenclear();
#ಆದರೆ defined(FEAT_TABPANEL)
    draw_tabpanel();
#ಆದರೆ_ಅಂತ್ಯ
    intro_ಸಂದೇಶ(TRUE);
    wait_return(TRUE);
}
