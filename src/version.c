/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#అన్తర్భావ "vim.h"

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

#అన్తర్భావ "version.h"

అక్షరం		*సంస్కరణ = VIM_సంస్కరణ_SHORT;
స్థిర అక్షరం	*medium_సంస్కరణ = VIM_సంస్కరణ_MEDIUM;

#ఒకవేళ defined(HAVE_DATE_సమయం)
# ఒకవేళ defined(VMS) && defined(VAXC)
అక్షరం	long_సంస్కరణ[పరిమాణం(VIM_సంస్కరణ_LONG_DATE) + పరిమాణం(__DATE__)
						      + పరిమాణం(__TIME__) + 3];

    శూన్య
init_long_సంస్కరణ(శూన్య)
{
    /*
     * Construct the long version string.  Necessary because
     * VAX C can't concatenate strings in the preprocessor.
     */
    strcpy(long_సంస్కరణ, VIM_సంస్కరణ_LONG_DATE);
#  ifdef BUILD_DATE
    strcat(long_సంస్కరణ, BUILD_DATE);
#  లేకపోతే
    strcat(long_సంస్కరణ, __DATE__);
    strcat(long_సంస్కరణ, " ");
    strcat(long_సంస్కరణ, __TIME__);
#  endif
    strcat(long_సంస్కరణ, ")");
}

# లేకపోతే
అక్షరం	*long_సంస్కరణ = ఖాళీ;

    శూన్య
init_long_సంస్కరణ(శూన్య)
{
    ఒకవేళ (long_సంస్కరణ != ఖాళీ)
	తిరిగి;

#  ifdef BUILD_DATE
    అక్షరం *date_సమయం = BUILD_DATE;
#  లేకపోతే
    అక్షరం *date_సమయం = __DATE__ " " __TIME__;
#  endif
    అక్షరం *సందేశం = _("%s (%s, compiled %s)");
    size_t len = అక్షరశ్రేణి_పొడవు(సందేశం)
	+ STRLEN_LITERAL(VIM_సంస్కరణ_LONG_ONLY)
	+ STRLEN_LITERAL(VIM_సంస్కరణ_DATE_ONLY)
	+ అక్షరశ్రేణి_పొడవు(date_సమయం);

    long_సంస్కరణ = alloc(len);
    ఒకవేళ (long_సంస్కరణ == ఖాళీ)
	long_సంస్కరణ = VIM_సంస్కరణ_LONG;
    లేకపోతే
	vim_snprintf(long_సంస్కరణ, len, సందేశం,
		VIM_సంస్కరణ_LONG_ONLY, VIM_సంస్కరణ_DATE_ONLY, date_సమయం);
}
# endif
#లేకపోతే
అక్షరం	*long_సంస్కరణ = VIM_సంస్కరణ_LONG;

    శూన్య
init_long_సంస్కరణ(శూన్య)
{
    // nothing to do
}
#అయితే_ముగింపు

స్థిర అక్షరం *(features[]) =
{
#నిర్వచితమైతే HAVE_ACL
	"+acl",
#లేకపోతే
	"-acl",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_ARABIC
	"+arabic",
#లేకపోతే
	"-arabic",
#అయితే_ముగింపు
#నిర్వచితమైతే AMIGA		// only for Amiga systems
# ifdef FEAT_ARP
	"+ARP",
# లేకపోతే
	"-ARP",
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_AUTOCHDIR
	"+autochdir",
#లేకపోతే
	"-autochdir",
#అయితే_ముగింపు
	"+autocmd",
#నిర్వచితమైతే FEAT_AUTOSERVERNAME
	"+autoservername",
#లేకపోతే
	"-autoservername",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_BEVAL_GUI
	"+balloon_eval",
#లేకపోతే
	"-balloon_eval",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_BEVAL_పదం
	"+balloon_eval_term",
#లేకపోతే
	"-balloon_eval_term",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_BROWSE
	"+browse",
#లేకపోతే
	"-browse",
#అయితే_ముగింపు
	"++builtin_terms",
#నిర్వచితమైతే FEAT_BYTEOFF
	"+byte_offset",
#లేకపోతే
	"-byte_offset",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
	"+channel",
#లేకపోతే
	"-channel",
#అయితే_ముగింపు
	"+cindent",
#నిర్వచితమైతే FEAT_CLIENTSERVER
	"+clientserver",
#లేకపోతే
	"-clientserver",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD
	"+clipboard",
#లేకపోతే
	"-clipboard",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
	"+clipboard_provider",
#లేకపోతే
	"-clipboard_provider",
#అయితే_ముగింపు
	"+cmdline_compl",
	"+cmdline_hist",
	"+cmdline_info",
	"+comments",
#నిర్వచితమైతే FEAT_CONCEAL
	"+conceal",
#లేకపోతే
	"-conceal",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
	"+cryptv",
#లేకపోతే
	"-cryptv",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CSCOPE
	"+cscope",
#లేకపోతే
	"-cscope",
#అయితే_ముగింపు
	"+cursorbind",
#నిర్వచితమైతే CURSOR_SHAPE
	"+cursorshape",
#లేకపోతే
	"-cursorshape",
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
	"+dialog_con_gui",
#లేకపోతే
# ఒకవేళ defined(FEAT_CON_DIALOG)
	"+dialog_con",
# లేకపోతే
#  ఒకవేళ defined(FEAT_GUI_DIALOG)
	"+dialog_gui",
#  లేకపోతే
	"-dialog",
#  endif
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
	"+diff",
#లేకపోతే
	"-diff",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIGRAPHS
	"+digraphs",
#లేకపోతే
	"-digraphs",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_MSWIN
# ifdef FEAT_DIRECTX
	"+directx",
# లేకపోతే
	"-directx",
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DND
	"+dnd",
#లేకపోతే
	"-dnd",
#అయితే_ముగింపు
	"-ebcdic",
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
	"+emacs_tags",
#లేకపోతే
	"-emacs_tags",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
	"+eval",
#లేకపోతే
	"-eval",
#అయితే_ముగింపు
	"+ex_extra",
#నిర్వచితమైతే FEAT_శోధన_EXTRA
	"+extra_search",
#లేకపోతే
	"-extra_search",
#అయితే_ముగింపు
	"-farsi",
	"+file_in_path",
#నిర్వచితమైతే FEAT_కనుగొను_ID
	"+find_in_path",
#లేకపోతే
	"-find_in_path",
#అయితే_ముగింపు
	"+float",
#నిర్వచితమైతే FEAT_FOLDING
	"+folding",
#లేకపోతే
	"-folding",
#అయితే_ముగింపు
	"-footer",
	// only interesting on Unix systems
#ఒకవేళ !defined(USE_SYSTEM) && defined(UNIX)
	"+fork()",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
	"+gettext/dyn",
# లేకపోతే
	"+gettext",
# endif
#లేకపోతే
	"-gettext",
#అయితే_ముగింపు
	"-hangul_input",
#ఒకవేళ (defined(HAVE_ICONV_H) && defined(USE_ICONV)) || defined(DYNAMIC_ICONV)
# ifdef DYNAMIC_ICONV
	"+iconv/dyn",
# లేకపోతే
	"+iconv",
# endif
#లేకపోతే
	"-iconv",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_IMAGE
	"+image",
#లేకపోతే
	"-image",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_IMAGE_POPUP
	"+image_popup",
#లేకపోతే
	"-image_popup",
#అయితే_ముగింపు
	"+insert_expand",
#నిర్వచితమైతే FEAT_IPV6
	"+ipv6",
#లేకపోతే
	"-ipv6",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
	"+job",
#లేకపోతే
	"-job",
#అయితే_ముగింపు
	"+jumplist",
#నిర్వచితమైతే FEAT_KEYMAP
	"+keymap",
#లేకపోతే
	"-keymap",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
	"+lambda",
#లేకపోతే
	"-lambda",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LANGMAP
	"+langmap",
#లేకపోతే
	"-langmap",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LIBCALL
	"+libcall",
#లేకపోతే
	"-libcall",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
	"+linebreak",
#లేకపోతే
	"-linebreak",
#అయితే_ముగింపు
	"+lispindent",
	"+listcmds",
	"+localmap",
#నిర్వచితమైతే FEAT_LUA
# ifdef DYNAMIC_LUA
	"+lua/dyn",
# లేకపోతే
	"+lua",
# endif
#లేకపోతే
	"-lua",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MENU
	"+menu",
#లేకపోతే
	"-menu",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SESSION
	"+mksession",
#లేకపోతే
	"-mksession",
#అయితే_ముగింపు
	"+modify_fname",
	"+mouse",
#ఒకవేళ defined(UNIX) || defined(VMS)
# ifdef FEAT_MOUSE_DEC
	"+mouse_dec",
# లేకపోతే
	"-mouse_dec",
# endif
# ifdef FEAT_MOUSE_GPM
#  ifdef DYNAMIC_GPM
	"+mouse_gpm/dyn",
#  లేకపోతే
	"+mouse_gpm",
#  endif
# లేకపోతే
	"-mouse_gpm",
# endif
# ifdef FEAT_MOUSE_JSB
	"+mouse_jsbterm",
# లేకపోతే
	"-mouse_jsbterm",
# endif
# ifdef FEAT_MOUSE_NET
	"+mouse_netterm",
# లేకపోతే
	"-mouse_netterm",
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే __QNX__
# ifdef FEAT_MOUSE_PTERM
	"+mouse_pterm",
# లేకపోతే
	"-mouse_pterm",
# endif
#అయితే_ముగింపు

#ఒకవేళ defined(UNIX) || defined(VMS)
	"+mouse_sgr",
# ifdef FEAT_SYSMOUSE
	"+mouse_sysmouse",
# లేకపోతే
	"-mouse_sysmouse",
# endif
# ifdef FEAT_MOUSE_URXVT
	"+mouse_urxvt",
# లేకపోతే
	"-mouse_urxvt",
# endif
	"+mouse_xterm",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSESHAPE
	"+mouseshape",
#లేకపోతే
	"-mouseshape",
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MBYTE_IME
# ifdef DYNAMIC_IME
	"+multi_byte_ime/dyn",
# లేకపోతే
	"+multi_byte_ime",
# endif
#లేకపోతే
	"+multi_byte",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MULTI_LANG
	"+multi_lang",
#లేకపోతే
	"-multi_lang",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MZSCHEME
# ifdef DYNAMIC_MZSCHEME
	"+mzscheme/dyn",
# లేకపోతే
	"+mzscheme",
# endif
#లేకపోతే
	"-mzscheme",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	"+netbeans_intg",
#లేకపోతే
	"-netbeans_intg",
#అయితే_ముగింపు
	"+num64",
#నిర్వచితమైతే FEAT_GUI_MSWIN
# ifdef FEAT_OLE
	"+ole",
# లేకపోతే
	"-ole",
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
	"+packages",
#లేకపోతే
	"-packages",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PRINT_PANGO
	"+pango",
#లేకపోతే
	"-pango",
#అయితే_ముగింపు
	"+path_extra",
#నిర్వచితమైతే FEAT_PERL
# ifdef DYNAMIC_PERL
	"+perl/dyn",
# లేకపోతే
	"+perl",
# endif
#లేకపోతే
	"-perl",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
	"+persistent_undo",
#లేకపోతే
	"-persistent_undo",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
	"+popupwin",
#లేకపోతే
	"-popupwin",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PRINTER
# ifdef FEAT_POSTSCRIPT
	"+postscript",
# లేకపోతే
	"-postscript",
# endif
	"+printer",
#లేకపోతే
	"-printer",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROFILE
	"+profile",
#లేకపోతే
	"-profile",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PYTHON
# ifdef DYNAMIC_PYTHON
	"+python/dyn",
# లేకపోతే
	"+python",
# endif
#లేకపోతే
	"-python",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PYTHON3
# ifdef DYNAMIC_PYTHON3
#  ifdef DYNAMIC_PYTHON3_STABLE_ABI
	"+python3/dyn-stable",
#  లేకపోతే
	"+python3/dyn",
#  endif
# లేకపోతే
	"+python3",
# endif
#లేకపోతే
	"-python3",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
	"+quickfix",
#లేకపోతే
	"-quickfix",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RELTIME
	"+reltime",
#లేకపోతే
	"-reltime",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RIGHTLEFT
	"+rightleft",
#లేకపోతే
	"-rightleft",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RUBY
# ifdef DYNAMIC_RUBY
	"+ruby/dyn",
# లేకపోతే
	"+ruby",
# endif
#లేకపోతే
	"-ruby",
#అయితే_ముగింపు
	"+scrollbind",
#నిర్వచితమైతే FEAT_SIGNS
	"+signs",
#లేకపోతే
	"-signs",
#అయితే_ముగింపు
	"+smartindent",
#నిర్వచితమైతే FEAT_SOCKETSERVER
	"+socketserver",
#లేకపోతే
	"-socketserver",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SODIUM
# ifdef DYNAMIC_SODIUM
	"+sodium/dyn",
# లేకపోతే
	"+sodium",
# endif
#లేకపోతే
	"-sodium",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SOUND
	"+sound",
#లేకపోతే
	"-sound",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
	"+spell",
#లేకపోతే
	"-spell",
#అయితే_ముగింపు
#నిర్వచితమైతే STARTUPTIME
	"+startuptime",
#లేకపోతే
	"-startuptime",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_STL_OPT
	"+statusline",
#లేకపోతే
	"-statusline",
#అయితే_ముగింపు
	"-sun_workshop",
#నిర్వచితమైతే FEAT_SYN_HL
	"+syntax",
#లేకపోతే
	"-syntax",
#అయితే_ముగింపు
	    // only interesting on Unix systems
#ఒకవేళ defined(USE_SYSTEM) && defined(UNIX)
	"+system()",
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_TABPANEL)
	"+tabpanel",
#లేకపోతే
	"-tabpanel",
#అయితే_ముగింపు
	"-tag_any_white",
	"+tag_binary",
	"-tag_old_static",
#నిర్వచితమైతే FEAT_TCL
# ifdef DYNAMIC_TCL
	"+tcl/dyn",
# లేకపోతే
	"+tcl",
# endif
#లేకపోతే
	"-tcl",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMGUICOLORS
	"+termguicolors",
#లేకపోతే
	"-termguicolors",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
	"+terminal",
#లేకపోతే
	"-terminal",
#అయితే_ముగింపు
#ఒకవేళ defined(UNIX)
// only Unix can have terminfo instead of termcap
# ifdef TERMINFO
	"+terminfo",
# లేకపోతే
	"-terminfo",
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMRESPONSE
	"+termresponse",
#లేకపోతే
	"-termresponse",
#అయితే_ముగింపు
	"+textobjects",
#నిర్వచితమైతే FEAT_PROP_POPUP
	"+textprop",
#లేకపోతే
	"-textprop",
#అయితే_ముగింపు
#ఒకవేళ !defined(UNIX)
// unix always includes termcap support
# ifdef HAVE_TGETENT
	"+tgetent",
# లేకపోతే
	"-tgetent",
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TIMERS
	"+timers",
#లేకపోతే
	"-timers",
#అయితే_ముగింపు
	"+title",
#నిర్వచితమైతే FEAT_TOOLBAR
	"+toolbar",
#లేకపోతే
	"-toolbar",
#అయితే_ముగింపు
	"+user_commands",
#నిర్వచితమైతే FEAT_VARTABS
	"+vartabs",
#లేకపోతే
	"-vartabs",
#అయితే_ముగింపు
	"+vertsplit",
	"+vim9script",
#నిర్వచితమైతే FEAT_VIMINFO
	"+viminfo",
#లేకపోతే
	"-viminfo",
#అయితే_ముగింపు
	"+virtualedit",
	"+visual",
	"+visualextra",
	"+vreplace",
#నిర్వచితమైతే MSWIN
# ifdef FEAT_VTP
	"+vtp",
# లేకపోతే
	"-vtp",
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_WAYLAND
	"+wayland",
#లేకపోతే
	"-wayland",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD
	"+wayland_clipboard",
#లేకపోతే
	"-wayland_clipboard",
#అయితే_ముగింపు
	"+wildignore",
	"+wildmenu",
	"+windows",
#నిర్వచితమైతే FEAT_WRITEBACKUP
	"+writebackup",
#లేకపోతే
	"-writebackup",
#అయితే_ముగింపు
#ఒకవేళ defined(UNIX) || defined(VMS)
# ifdef FEAT_X11
	"+X11",
# లేకపోతే
	"-X11",
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_XATTR
	"+xattr",
#లేకపోతే
	"-xattr",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_XFONTSET
	"+xfontset",
#లేకపోతే
	"-xfontset",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_XIM
	"+xim",
#లేకపోతే
	"-xim",
#అయితే_ముగింపు
#ఒకవేళ defined(MSWIN)
# ifdef FEAT_XPM_W32
	"+xpm_w32",
# లేకపోతే
	"-xpm_w32",
# endif
#లేదావేళ defined(HAVE_XPM)
	"+xpm",
#లేకపోతే
	"-xpm",
#అయితే_ముగింపు
#ఒకవేళ defined(UNIX) || defined(VMS)
# ఒకవేళ defined(USE_XSMP_INTERACT)
	"+xsmp_interact",
# elif defined(USE_XSMP)
	"+xsmp",
# లేకపోతే
	"-xsmp",
# endif
# ifdef FEAT_XCLIPBOARD
	"+xterm_clipboard",
# లేకపోతే
	"-xterm_clipboard",
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_XTERM_SAVE
	"+xterm_save",
#లేకపోతే
	"-xterm_save",
#అయితే_ముగింపు
	ఖాళీ
};

స్థిర పూర్ణాంకం included_patches[] =
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
స్థిర అక్షరం *(extra_patches[]) =
{   /* Add your patch description below this line */
/**/
    ఖాళీ
};

    పూర్ణాంకం
highest_patch(శూన్య)
{
    // this relies on the highest patch number to be the first entry
    తిరిగి included_patches[0];
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return TRUE if patch "n" has been included.
 */
    పూర్ణాంకం
has_patch(పూర్ణాంకం n)
{
    పూర్ణాంకం		h, m, l;

    // Perform a binary search.
    l = 0;
    h = (పూర్ణాంకం)ARRAY_నిడివి(included_patches) - 1;
    కొరకు (;;)
    {
	m = (l + h) / 2;
	ఒకవేళ (included_patches[m] == n)
	    తిరిగి TRUE;
	ఒకవేళ (l == h)
	    ఆపు;
	ఒకవేళ (included_patches[m] < n)
	    h = m;
	లేకపోతే
	    l = m + 1;
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు

    శూన్య
ex_సంస్కరణ(exarg_T *eap)
{
    /*
     * Ignore a ":version 9.99" command.
     */
    ఒకవేళ (*eap->arg == NUL)
    {
	సందేశం_putchar('\n');
	list_సంస్కరణ();
    }
}

/*
 * Output a string for the version message.  If it's going to wrap, output a
 * newline, unless the message is too long to fit on the screen anyway.
 * When "wrap" is TRUE wrap the string in [].
 */
    స్థిర శూన్య
సంస్కరణ_సందేశం_wrap(char_u *s, పూర్ణాంకం wrap)
{
    పూర్ణాంకం		len = vim_strsize(s) + (wrap ? 2 : 0);

    ఒకవేళ (!got_int && len < cmdline_వెడల్పు && సందేశం_నిలువు + len >= cmdline_వెడల్పు
								&& *s != '\n')
	సందేశం_putchar('\n');
    ఒకవేళ (!got_int)
    {
	ఒకవేళ (wrap)
	    సందేశం_puts("[");
	సందేశం_puts((అక్షరం *)s);
	ఒకవేళ (wrap)
	    సందేశం_puts("]");
    }
}

    స్థిర శూన్య
సంస్కరణ_సందేశం(అక్షరం *s)
{
    సంస్కరణ_సందేశం_wrap((char_u *)s, FALSE);
}

/*
 * List all features aligned in columns, dictionary style.
 */
    స్థిర శూన్య
list_features(శూన్య)
{
    list_లోపల_columns((char_u **)features, -1, -1);
}

/*
 * List string items nicely aligned in columns.
 * When "size" is < 0 then the last entry is marked with NULL.
 * The entry with index "current" is inclosed in [].
 */
    శూన్య
list_లోపల_columns(char_u **వస్తువులు, పూర్ణాంకం కొలత, పూర్ణాంకం current)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		ncol;
    పూర్ణాంకం		nrow;
    పూర్ణాంకం		cur_అడ్డువరుస = 1;
    పూర్ణాంకం		అంశం_లెక్క = 0;
    పూర్ణాంకం		వెడల్పు = 0;
#నిర్వచితమైతే FEAT_SYN_HL
    పూర్ణాంకం		use_highlight = (వస్తువులు == (char_u **)features);
#అయితే_ముగింపు

    // Find the length of the longest item, use that + 1 as the column
    // width.
    కొరకు (i = 0; కొలత < 0 ? వస్తువులు[i] != ఖాళీ : i < కొలత; ++i)
    {
	పూర్ణాంకం l = vim_strsize(వస్తువులు[i]) + (i == current ? 2 : 0);

	ఒకవేళ (l > వెడల్పు)
	    వెడల్పు = l;
	++అంశం_లెక్క;
    }
    వెడల్పు += 1;

    ఒకవేళ (cmdline_వెడల్పు < వెడల్పు)
    {
	// Not enough screen columns - show one per line
	కొరకు (i = 0; i < అంశం_లెక్క; ++i)
	{
	    సంస్కరణ_సందేశం_wrap(వస్తువులు[i], i == current);
	    ఒకవేళ (సందేశం_నిలువు > 0 && i < అంశం_లెక్క - 1)
		సందేశం_putchar('\n');
	}
	తిరిగి;
    }

    // The rightmost column doesn't need a separator.
    // Sacrifice it to fit in one more column if possible.
    ncol = (cmdline_వెడల్పు + 1) / వెడల్పు;
    nrow = అంశం_లెక్క / ncol + ((అంశం_లెక్క % ncol) ? 1 : 0);

    // "i" counts columns then rows.  "idx" counts rows then columns.
    కొరకు (i = 0; !got_int && i < nrow * ncol; ++i)
    {
	పూర్ణాంకం idx = (i / ncol) + (i % ncol) * nrow;

	ఒకవేళ (idx < అంశం_లెక్క)
	{
	    పూర్ణాంకం చివరి_నిలువు = (i + 1) % ncol == 0;

	    ఒకవేళ (idx == current)
		సందేశం_putchar('[');
#నిర్వచితమైతే FEAT_SYN_HL
	    ఒకవేళ (use_highlight && వస్తువులు[idx][0] == '-')
		సందేశం_puts_attr((అక్షరం *)వస్తువులు[idx], HL_ATTR(HLF_W));
	    లేకపోతే
#అయితే_ముగింపు
		సందేశం_puts((అక్షరం *)వస్తువులు[idx]);
	    ఒకవేళ (idx == current)
		సందేశం_putchar(']');
	    ఒకవేళ (చివరి_నిలువు)
	    {
		ఒకవేళ (సందేశం_నిలువు > 0 && cur_అడ్డువరుస < nrow)
		    సందేశం_putchar('\n');
		++cur_అడ్డువరుస;
	    }
	    లేకపోతే
	    {
		అయితే (సందేశం_నిలువు % వెడల్పు)
		    సందేశం_putchar(' ');
	    }
	}
	లేకపోతే
	{
	    // this row is out of items, thus at the end of the row
	    ఒకవేళ (సందేశం_నిలువు > 0)
	    {
		ఒకవేళ (cur_అడ్డువరుస < nrow)
		    సందేశం_putchar('\n');
		++cur_అడ్డువరుస;
	    }
	}
    }
}

    శూన్య
list_సంస్కరణ(శూన్య)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		మొదటి;
    అక్షరం	*s = "";

    /*
     * When adding features here, don't forget to update the list of
     * internal variables in eval.c!
     */
    init_long_సంస్కరణ();
    సందేశం(long_సంస్కరణ);
#నిర్వచితమైతే MSWIN
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
#   ifdef _WIN64
#    ఒకవేళ defined(_M_ARM64) || defined(_M_ARM64EC)
     సందేశం_puts(_("\nMS-Windows ARM64 GUI/console version"));
#    లేకపోతే
     సందేశం_puts(_("\nMS-Windows 64-bit GUI/console version"));
#    endif
#   లేకపోతే
    సందేశం_puts(_("\nMS-Windows 32-bit GUI/console version"));
#   endif
#  లేకపోతే
#   ifdef _WIN64
#    ఒకవేళ defined(_M_ARM64) || defined(_M_ARM64EC)
     సందేశం_puts(_("\nMS-Windows ARM64 GUI version"));
#    లేకపోతే
     సందేశం_puts(_("\nMS-Windows 64-bit GUI version"));
#    endif
#   లేకపోతే
    సందేశం_puts(_("\nMS-Windows 32-bit GUI version"));
#   endif
#  endif
#  ifdef FEAT_OLE
    సందేశం_puts(_(" with OLE support"));
#  endif
# లేకపోతే
#  ifdef _WIN64
#   ఒకవేళ defined(_M_ARM64) || defined(_M_ARM64EC)
    సందేశం_puts(_("\nMS-Windows ARM64 console version"));
#   లేకపోతే
    సందేశం_puts(_("\nMS-Windows 64-bit console version"));
#   endif
#  లేకపోతే
    సందేశం_puts(_("\nMS-Windows 32-bit console version"));
#  endif
# endif
#అయితే_ముగింపు
#ఒకవేళ defined(MACOS_X)
# ఒకవేళ defined(MACOS_X_DARWIN)
    సందేశం_puts(_("\nmacOS version"));
# లేకపోతే
    సందేశం_puts(_("\nmacOS version w/o darwin feat."));
# endif
# ఒకవేళ defined(__arm64__)
    సందేశం_puts(" - arm64");
# elif defined(__x86_64__)
    సందేశం_puts(" - x86_64");
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే VMS
    సందేశం_puts(_("\nOpenVMS (build) arch, version"));
# ifdef HAVE_PATHDEF
    ఒకవేళ (*compiled_arch != NUL)
    {
	సందేశం_puts(": ");
	సందేశం_puts((అక్షరం *)compiled_arch);
	ఒకవేళ (*compiled_vers != NUL)
	{
	    సందేశం_puts(", ");
	    సందేశం_puts((అక్షరం *)compiled_vers);
	}
    }

# endif

#అయితే_ముగింపు

    // Print the list of patch numbers if there is at least one.
    // Print a range when patches are consecutive: "1-10, 12, 15-40, 42-45"
    ఒకవేళ (included_patches[0] != 0)
    {
	సందేశం_puts(_("\nIncluded patches: "));
	మొదటి = -1;
	i = (పూర్ణాంకం)ARRAY_నిడివి(included_patches) - 1;
	అయితే (--i >= 0)
	{
	    ఒకవేళ (మొదటి < 0)
		మొదటి = included_patches[i];
	    ఒకవేళ (i == 0 || included_patches[i - 1] != included_patches[i] + 1)
	    {
		సందేశం_puts(s);
		s = ", ";
		సందేశం_outnum((దీర్ఘ)మొదటి);
		ఒకవేళ (మొదటి != included_patches[i])
		{
		    సందేశం_puts("-");
		    సందేశం_outnum((దీర్ఘ)included_patches[i]);
		}
		మొదటి = -1;
	    }
	}
    }

    // Print the list of extra patch descriptions if there is at least one.
    ఒకవేళ (extra_patches[0] != ఖాళీ)
    {
	సందేశం_puts(_("\nExtra patches: "));
	s = "";
	కొరకు (i = 0; extra_patches[i] != ఖాళీ; ++i)
	{
	    సందేశం_puts(s);
	    s = ", ";
	    సందేశం_puts(extra_patches[i]);
	}
    }

#నిర్వచితమైతే MODIFIED_BY
    సందేశం_puts("\n");
    సందేశం_puts(_("Modified by "));
    సందేశం_puts(MODIFIED_BY);
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_PATHDEF
    ఒకవేళ (*compiled_user != NUL || *compiled_sys != NUL)
    {
	సందేశం_puts(_("\nCompiled "));
	ఒకవేళ (*compiled_user != NUL)
	{
	    సందేశం_puts(_("by "));
	    సందేశం_puts((అక్షరం *)compiled_user);
	}
	ఒకవేళ (*compiled_sys != NUL)
	{
	    సందేశం_puts("@");
	    సందేశం_puts((అక్షరం *)compiled_sys);
	}
    }
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_HUGE)
    సందేశం_puts(_("\nHuge version "));
#లేదావేళ defined(FEAT_సాధారణ)
    సందేశం_puts(_("\nNormal version "));
#లేకపోతే
    సందేశం_puts(_("\nTiny version "));
#అయితే_ముగింపు
#ఒకవేళ !defined(FEAT_GUI)
    సందేశం_puts(_("without GUI."));
#లేదావేళ defined(FEAT_GUI_GTK)
# ఒకవేళ defined(USE_GTK4)
    సందేశం_puts(_("with GTK4 GUI."));
# elif defined(USE_GTK3)
    సందేశం_puts(_("with GTK3 GUI."));
# elif defined(FEAT_GUI_GNOME)
    సందేశం_puts(_("with GTK2-GNOME GUI."));
# లేకపోతే
    సందేశం_puts(_("with GTK2 GUI."));
# endif
#లేదావేళ defined(FEAT_GUI_MOTIF)
    సందేశం_puts(_("with X11-Motif GUI."));
#లేదావేళ defined(FEAT_GUI_HAIKU)
    సందేశం_puts(_("with Haiku GUI."));
#లేదావేళ defined(FEAT_GUI_PHOTON)
    సందేశం_puts(_("with Photon GUI."));
#లేదావేళ defined(MSWIN)
    సందేశం_puts(_("with GUI."));
#అయితే_ముగింపు
    సంస్కరణ_సందేశం(_("  Features included (+) or not (-):\n"));

    list_features();
    ఒకవేళ (సందేశం_నిలువు > 0)
	సందేశం_putchar('\n');

#నిర్వచితమైతే SYS_VIMRC_FILE
    సంస్కరణ_సందేశం(_("   system vimrc file: \""));
    సంస్కరణ_సందేశం(SYS_VIMRC_FILE);
    సంస్కరణ_సందేశం("\"\n");
#అయితే_ముగింపు
#నిర్వచితమైతే USR_VIMRC_FILE
    సంస్కరణ_సందేశం(_("     user vimrc file: \""));
    సంస్కరణ_సందేశం(USR_VIMRC_FILE);
    సంస్కరణ_సందేశం("\"\n");
#అయితే_ముగింపు
#నిర్వచితమైతే USR_VIMRC_FILE2
    సంస్కరణ_సందేశం(_(" 2nd user vimrc file: \""));
    సంస్కరణ_సందేశం(USR_VIMRC_FILE2);
    సంస్కరణ_సందేశం("\"\n");
#అయితే_ముగింపు
#ఒకవేళ defined(USR_VIMRC_FILE3) && defined(XDG_VIMRC_FILE)
    సంస్కరణ_సందేశం(_(" 3rd user vimrc file: \""));
    సంస్కరణ_సందేశం(USR_VIMRC_FILE3);
    సంస్కరణ_సందేశం("\"\n");
    సంస్కరణ_సందేశం(_(" 4th user vimrc file: \""));
    సంస్కరణ_సందేశం((అక్షరం *)(XDG_VIMRC_FILE));
    సంస్కరణ_సందేశం("\"\n");
#లేదావేళ defined(USR_VIMRC_FILE3)
    సంస్కరణ_సందేశం(_(" 3rd user vimrc file: \""));
    సంస్కరణ_సందేశం(USR_VIMRC_FILE3);
    సంస్కరణ_సందేశం("\"\n");
#లేదావేళ defined(XDG_VIMRC_FILE)
    సంస్కరణ_సందేశం(_(" 3rd user vimrc file: \""));
    సంస్కరణ_సందేశం((అక్షరం *)(XDG_VIMRC_FILE));
    సంస్కరణ_సందేశం("\"\n");
#అయితే_ముగింపు
#నిర్వచితమైతే USR_EXRC_FILE
    సంస్కరణ_సందేశం(_("      user exrc file: \""));
    సంస్కరణ_సందేశం(USR_EXRC_FILE);
    సంస్కరణ_సందేశం("\"\n");
#అయితే_ముగింపు
#నిర్వచితమైతే USR_EXRC_FILE2
    సంస్కరణ_సందేశం(_("  2nd user exrc file: \""));
    సంస్కరణ_సందేశం(USR_EXRC_FILE2);
    సంస్కరణ_సందేశం("\"\n");
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
# ifdef SYS_GVIMRC_FILE
    సంస్కరణ_సందేశం(_("  system gvimrc file: \""));
    సంస్కరణ_సందేశం(SYS_GVIMRC_FILE);
    సంస్కరణ_సందేశం("\"\n");
# endif
    సంస్కరణ_సందేశం(_("    user gvimrc file: \""));
    సంస్కరణ_సందేశం(USR_GVIMRC_FILE);
    సంస్కరణ_సందేశం("\"\n");
# ifdef USR_GVIMRC_FILE2
    సంస్కరణ_సందేశం(_("2nd user gvimrc file: \""));
    సంస్కరణ_సందేశం(USR_GVIMRC_FILE2);
    సంస్కరణ_సందేశం("\"\n");
# endif
# ifdef USR_GVIMRC_FILE3
    సంస్కరణ_సందేశం(_("3rd user gvimrc file: \""));
    సంస్కరణ_సందేశం(USR_GVIMRC_FILE3);
    సంస్కరణ_సందేశం("\"\n");
# endif
#అయితే_ముగింపు
    సంస్కరణ_సందేశం(_("       defaults file: \""));
    సంస్కరణ_సందేశం(VIM_DEFAULTS_FILE);
    సంస్కరణ_సందేశం("\"\n");
#నిర్వచితమైతే FEAT_GUI
# ifdef SYS_MENU_FILE
    సంస్కరణ_సందేశం(_("    system menu file: \""));
    సంస్కరణ_సందేశం(SYS_MENU_FILE);
    సంస్కరణ_సందేశం("\"\n");
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_PATHDEF
    ఒకవేళ (*default_vim_dir != NUL)
    {
	సంస్కరణ_సందేశం(_("  fall-back for $VIM: \""));
	సంస్కరణ_సందేశం((అక్షరం *)default_vim_dir);
	సంస్కరణ_సందేశం("\"\n");
    }
    ఒకవేళ (*default_vimruntime_dir != NUL)
    {
	సంస్కరణ_సందేశం(_(" f-b for $VIMRUNTIME: \""));
	సంస్కరణ_సందేశం((అక్షరం *)default_vimruntime_dir);
	సంస్కరణ_సందేశం("\"\n");
    }
    సంస్కరణ_సందేశం(_("Compilation: "));
    సంస్కరణ_సందేశం((అక్షరం *)all_cflags);
    సంస్కరణ_సందేశం("\n");
# ifdef VMS
    ఒకవేళ (*compiler_సంస్కరణ != NUL)
    {
	సంస్కరణ_సందేశం(_("Compiler: "));
	సంస్కరణ_సందేశం((అక్షరం *)compiler_సంస్కరణ);
	సంస్కరణ_సందేశం("\n");
    }
# endif
    సంస్కరణ_సందేశం(_("Linking: "));
    సంస్కరణ_సందేశం((అక్షరం *)all_lflags);
#అయితే_ముగింపు
#నిర్వచితమైతే DEBUG
    సంస్కరణ_సందేశం("\n");
    సంస్కరణ_సందేశం(_("  DEBUG BUILD"));
#అయితే_ముగింపు
}

స్థిర శూన్య do_intro_పంక్తి(పూర్ణాంకం అడ్డువరుస, char_u *mesg, పూర్ణాంకం కూడు_సంస్కరణ, పూర్ణాంకం attr);
స్థిర శూన్య intro_సందేశం(పూర్ణాంకం colon);

/*
 * Show the intro message when not editing a file.
 */
    శూన్య
maybe_intro_సందేశం(శూన్య)
{
    ఒకవేళ (BUFEMPTY()
	    && curbuf->b_fname == ఖాళీ
	    && firstwin->w_తదుపరి == ఖాళీ
	    && vim_strchr(p_shm, SHM_INTRO) == ఖాళీ)
	intro_సందేశం(FALSE);
}

/*
 * Give an introductory message about Vim.
 * Only used when starting Vim on an empty file, without a file name.
 * Or with the ":intro" command (for Sven :-).
 */
    స్థిర శూన్య
intro_సందేశం(
    పూర్ణాంకం		colon)		// TRUE for ":intro"
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		అడ్డువరుస;
    పూర్ణాంకం		blanklines;
    పూర్ణాంకం		sponsor;
    అక్షరం	*p;
    స్థిర అక్షరం	*(పంక్తులు[]) =
    {
	N_("VIM - Vi IMproved"),
	"",
	N_("version "),
	N_("by Bram Moolenaar et al."),
#నిర్వచితమైతే MODIFIED_BY
	" ",
#అయితే_ముగింపు
	N_("Vim is open source and freely distributable"),
	"",
	N_("Help poor children in Uganda!"),
	N_("type  :help Kuwasha<Enter>    for information "),
	"",
	N_("type  :q<Enter>               to exit         "),
	N_("type  :help<Enter>  or  <F1>  for on-line help"),
	N_("type  :help version9<Enter>   for version info"),
	ఖాళీ,
	"",
	N_("Running in Vi compatible mode"),
	N_("type  :set nocp<Enter>        for Vim defaults"),
	N_("type  :help cp-default<Enter> for info on this"),
    };
#నిర్వచితమైతే FEAT_GUI
    స్థిర అక్షరం	*(gui_పంక్తులు[]) =
    {
	ఖాళీ,
	ఖాళీ,
	ఖాళీ,
	ఖాళీ,
# ifdef MODIFIED_BY
	ఖాళీ,
# endif
	ఖాళీ,
	ఖాళీ,
	ఖాళీ,
	N_("menu  Help->Orphans           for information    "),
	ఖాళీ,
	N_("Running modeless, typed text is inserted"),
	N_("menu  Edit->Global Settings->Toggle Insert Mode  "),
	N_("                              for two modes      "),
	ఖాళీ,
	ఖాళీ,
	ఖాళీ,
	N_("menu  Edit->Global Settings->Toggle Vi Compatible"),
	N_("                              for Vim defaults   "),
    };
#అయితే_ముగింపు

    // blanklines = screen height - # message lines
    blanklines = (పూర్ణాంకం)అడ్డువరుసలు - (ARRAY_నిడివి(పంక్తులు) - 1);
    ఒకవేళ (!p_cp)
	blanklines += 4;  // add 4 for not showing "Vi compatible" message

    // Don't overwrite a statusline.  Depends on 'cmdheight'.
    ఒకవేళ (p_ls > 1)
	blanklines -= అడ్డువరుసలు - topframe->fr_ఎత్తు;
    ఒకవేళ (blanklines < 0)
	blanklines = 0;

    // Show the sponsor and Uganda message two out of four times
    sponsor = (పూర్ణాంకం)సమయం(ఖాళీ);
    sponsor = ((sponsor & 2) == 0) - ((sponsor & 4) == 0);

    // start displaying the message lines after half of the blank lines
    అడ్డువరుస = blanklines / 2;
    ఒకవేళ ((అడ్డువరుస >= 2 && topframe->fr_వెడల్పు >= 50) || colon)
    {
	కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(పంక్తులు); ++i)
	{
	    p = పంక్తులు[i];
#నిర్వచితమైతే FEAT_GUI
	    ఒకవేళ (p_im && gui.లోపల_use && gui_పంక్తులు[i] != ఖాళీ)
		p = gui_పంక్తులు[i];
#అయితే_ముగింపు
	    ఒకవేళ (p == ఖాళీ)
	    {
		ఒకవేళ (!p_cp)
		    ఆపు;
		కొనసాగు;
	    }
	    ఒకవేళ (sponsor != 0)
	    {
		ఒకవేళ (అక్షరశ్రేణి_శోధన(p, "children") != ఖాళీ)
		    p = N_("Sponsor Vim development!");
		లేకపోతే ఒకవేళ (అక్షరశ్రేణి_శోధన(p, "Kuwasha") != ఖాళీ)
		    p = N_("type  :help sponsor<Enter>    for information ");
		లేకపోతే ఒకవేళ (అక్షరశ్రేణి_శోధన(p, "Orphans") != ఖాళీ)
		    p = N_("menu  Help->Sponsor           for information    ");
	    }
	    ఒకవేళ (*p != NUL)
		do_intro_పంక్తి(అడ్డువరుస, (char_u *)_(p), i == 2, 0);
	    ++అడ్డువరుస;
	}
    }

    // Make the wait-return message appear just below the text.
    ఒకవేళ (colon)
	సందేశం_అడ్డువరుస = అడ్డువరుస;
}

    స్థిర శూన్య
do_intro_పంక్తి(
    పూర్ణాంకం		అడ్డువరుస,
    char_u	*mesg,
    పూర్ణాంకం		కూడు_సంస్కరణ,
    పూర్ణాంకం		attr)
{
    char_u	vers[20];
    పూర్ణాంకం		నిలువు;
    char_u	*p;
    పూర్ణాంకం		l;
    పూర్ణాంకం		clen;
#నిర్వచితమైతే MODIFIED_BY
# define MODBY_LEN 150
    char_u	modby[MODBY_LEN];

    ఒకవేళ (*mesg == ' ')
    {
	vim_strncpy(modby, (char_u *)_("Modified by "), MODBY_LEN - 1);
	l = (పూర్ణాంకం)STRLEN(modby);
	vim_strncpy(modby + l, (char_u *)MODIFIED_BY, MODBY_LEN - l - 1);
	mesg = modby;
    }
#అయితే_ముగింపు

    // Center the message horizontally.
    నిలువు = vim_strsize(mesg);
    ఒకవేళ (కూడు_సంస్కరణ)
    {
	STRCPY(vers, medium_సంస్కరణ);
	ఒకవేళ (highest_patch())
	{
	    // Check for 9.9x or 9.9xx, alpha/beta version
	    ఒకవేళ (సురక్షిత_isalpha((పూర్ణాంకం)vers[3]))
	    {
		పూర్ణాంకం len = (సురక్షిత_isalpha((పూర్ణాంకం)vers[4])) ? 5 : 4;
		sprintf((అక్షరం *)vers + len, ".%d%s", highest_patch(),
							 medium_సంస్కరణ + len);
	    }
	    లేకపోతే
		sprintf((అక్షరం *)vers + 3, ".%d", highest_patch());
	}
	నిలువు += (పూర్ణాంకం)STRLEN(vers);
    }
    నిలువు = (topframe->fr_వెడల్పు - నిలువు) / 2;
    ఒకవేళ (నిలువు < 0)
	నిలువు = 0;

    // Split up in parts to highlight <> items differently.
    కొరకు (p = mesg; *p != NUL; p += l)
    {
	clen = 0;
	కొరకు (l = 0; p[l] != NUL
			 && (l == 0 || (p[l] != '<' && p[l - 1] != '>')); ++l)
	{
	    ఒకవేళ (has_mbyte)
	    {
		clen += ptr2cells(p + l);
		l += (*mb_ptr2len)(p + l) - 1;
	    }
	    లేకపోతే
		clen += byte2cells(p[l]);
	}
	screen_puts_len(p, l, అడ్డువరుస, నిలువు + firstwin->w_wincol,
		*p == '<' ? HL_ATTR(HLF_8) : attr);
	నిలువు += clen;
    }

    // Add the version number to the version line.
    ఒకవేళ (కూడు_సంస్కరణ)
	screen_puts(vers, అడ్డువరుస, నిలువు + firstwin->w_wincol, 0);
}

/*
 * ":intro": clear screen, display intro screen and wait for return.
 */
    శూన్య
ex_intro(exarg_T *eap వాడని)
{
    screenclear();
#ఒకవేళ defined(FEAT_TABPANEL)
    draw_tabpanel();
#అయితే_ముగింపు
    intro_సందేశం(TRUE);
    wait_return(TRUE);
}
