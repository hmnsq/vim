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

ವರ್ಣ		*Version = VIM_VERSION_SHORT;
ಸ್ಥಿರ ವರ್ಣ	*mediumVersion = VIM_VERSION_MEDIUM;

#ಯದಿ defined(HAVE_DATE_TIME)
# ಯದಿ defined(VMS) && defined(VAXC)
ವರ್ಣ	longVersion[ಪರಿಮಾಣ(VIM_VERSION_LONG_DATE) + ಪರಿಮಾಣ(__DATE__)
						      + ಪರಿಮಾಣ(__TIME__) + 3];

    ಶೂನ್ಯ
init_longVersion(ಶೂನ್ಯ)
{
    /*
     * Construct the long version string.  Necessary because
     * VAX C can't concatenate strings in the preprocessor.
     */
    strcpy(longVersion, VIM_VERSION_LONG_DATE);
#  ifdef BUILD_DATE
    strcat(longVersion, BUILD_DATE);
#  ಅನ್ಯಥಾ
    strcat(longVersion, __DATE__);
    strcat(longVersion, " ");
    strcat(longVersion, __TIME__);
#  endif
    strcat(longVersion, ")");
}

# ಅನ್ಯಥಾ
ವರ್ಣ	*longVersion = ರಿಕ್ತ;

    ಶೂನ್ಯ
init_longVersion(ಶೂನ್ಯ)
{
    ಯದಿ (longVersion != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

#  ifdef BUILD_DATE
    ವರ್ಣ *date_time = BUILD_DATE;
#  ಅನ್ಯಥಾ
    ವರ್ಣ *date_time = __DATE__ " " __TIME__;
#  endif
    ವರ್ಣ *ಸಂದೇಶ = _("%s (%s, compiled %s)");
    size_t len = strlen(ಸಂದೇಶ)
	+ STRLEN_LITERAL(VIM_VERSION_LONG_ONLY)
	+ STRLEN_LITERAL(VIM_VERSION_DATE_ONLY)
	+ strlen(date_time);

    longVersion = alloc(len);
    ಯದಿ (longVersion == ರಿಕ್ತ)
	longVersion = VIM_VERSION_LONG;
    ಅನ್ಯಥಾ
	vim_snprintf(longVersion, len, ಸಂದೇಶ,
		VIM_VERSION_LONG_ONLY, VIM_VERSION_DATE_ONLY, date_time);
}
# endif
#ಅನ್ಯಥಾ
ವರ್ಣ	*longVersion = VIM_VERSION_LONG;

    ಶೂನ್ಯ
init_longVersion(ಶೂನ್ಯ)
{
    // nothing to do
}
#endif

ಸ್ಥಿರ ವರ್ಣ *(features[]) =
{
#ifdef HAVE_ACL
	"+acl",
#ಅನ್ಯಥಾ
	"-acl",
#endif
#ifdef FEAT_ARABIC
	"+arabic",
#ಅನ್ಯಥಾ
	"-arabic",
#endif
#ifdef AMIGA		// only for Amiga systems
# ifdef FEAT_ARP
	"+ARP",
# ಅನ್ಯಥಾ
	"-ARP",
# endif
#endif
#ifdef FEAT_AUTOCHDIR
	"+autochdir",
#ಅನ್ಯಥಾ
	"-autochdir",
#endif
	"+autocmd",
#ifdef FEAT_AUTOSERVERNAME
	"+autoservername",
#ಅನ್ಯಥಾ
	"-autoservername",
#endif
#ifdef FEAT_BEVAL_GUI
	"+balloon_eval",
#ಅನ್ಯಥಾ
	"-balloon_eval",
#endif
#ifdef FEAT_BEVAL_TERM
	"+balloon_eval_term",
#ಅನ್ಯಥಾ
	"-balloon_eval_term",
#endif
#ifdef FEAT_BROWSE
	"+browse",
#ಅನ್ಯಥಾ
	"-browse",
#endif
	"++builtin_terms",
#ifdef FEAT_BYTEOFF
	"+byte_offset",
#ಅನ್ಯಥಾ
	"-byte_offset",
#endif
#ifdef FEAT_JOB_CHANNEL
	"+channel",
#ಅನ್ಯಥಾ
	"-channel",
#endif
	"+cindent",
#ifdef FEAT_CLIENTSERVER
	"+clientserver",
#ಅನ್ಯಥಾ
	"-clientserver",
#endif
#ifdef FEAT_CLIPBOARD
	"+clipboard",
#ಅನ್ಯಥಾ
	"-clipboard",
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	"+clipboard_provider",
#ಅನ್ಯಥಾ
	"-clipboard_provider",
#endif
	"+cmdline_compl",
	"+cmdline_hist",
	"+cmdline_info",
	"+comments",
#ifdef FEAT_CONCEAL
	"+conceal",
#ಅನ್ಯಥಾ
	"-conceal",
#endif
#ifdef FEAT_CRYPT
	"+cryptv",
#ಅನ್ಯಥಾ
	"-cryptv",
#endif
#ifdef FEAT_CSCOPE
	"+cscope",
#ಅನ್ಯಥಾ
	"-cscope",
#endif
	"+cursorbind",
#ifdef CURSOR_SHAPE
	"+cursorshape",
#ಅನ್ಯಥಾ
	"-cursorshape",
#endif
#ಯದಿ defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
	"+dialog_con_gui",
#ಅನ್ಯಥಾ
# ಯದಿ defined(FEAT_CON_DIALOG)
	"+dialog_con",
# ಅನ್ಯಥಾ
#  ಯದಿ defined(FEAT_GUI_DIALOG)
	"+dialog_gui",
#  ಅನ್ಯಥಾ
	"-dialog",
#  endif
# endif
#endif
#ifdef FEAT_DIFF
	"+diff",
#ಅನ್ಯಥಾ
	"-diff",
#endif
#ifdef FEAT_DIGRAPHS
	"+digraphs",
#ಅನ್ಯಥಾ
	"-digraphs",
#endif
#ifdef FEAT_GUI_MSWIN
# ifdef FEAT_DIRECTX
	"+directx",
# ಅನ್ಯಥಾ
	"-directx",
# endif
#endif
#ifdef FEAT_DND
	"+dnd",
#ಅನ್ಯಥಾ
	"-dnd",
#endif
	"-ebcdic",
#ifdef FEAT_EMACS_TAGS
	"+emacs_tags",
#ಅನ್ಯಥಾ
	"-emacs_tags",
#endif
#ifdef FEAT_EVAL
	"+eval",
#ಅನ್ಯಥಾ
	"-eval",
#endif
	"+ex_extra",
#ifdef FEAT_ಹುಡುಕು_EXTRA
	"+extra_search",
#ಅನ್ಯಥಾ
	"-extra_search",
#endif
	"-farsi",
	"+file_in_path",
#ifdef FEAT_FIND_ID
	"+find_in_path",
#ಅನ್ಯಥಾ
	"-find_in_path",
#endif
	"+float",
#ifdef FEAT_FOLDING
	"+folding",
#ಅನ್ಯಥಾ
	"-folding",
#endif
	"-footer",
	// only interesting on Unix systems
#ಯದಿ !defined(USE_SYSTEM) && defined(UNIX)
	"+fork()",
#endif
#ifdef FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
	"+gettext/dyn",
# ಅನ್ಯಥಾ
	"+gettext",
# endif
#ಅನ್ಯಥಾ
	"-gettext",
#endif
	"-hangul_input",
#ಯದಿ (defined(HAVE_ICONV_H) && defined(USE_ICONV)) || defined(DYNAMIC_ICONV)
# ifdef DYNAMIC_ICONV
	"+iconv/dyn",
# ಅನ್ಯಥಾ
	"+iconv",
# endif
#ಅನ್ಯಥಾ
	"-iconv",
#endif
#ifdef FEAT_IMAGE
	"+image",
#ಅನ್ಯಥಾ
	"-image",
#endif
#ifdef FEAT_IMAGE_POPUP
	"+image_popup",
#ಅನ್ಯಥಾ
	"-image_popup",
#endif
	"+insert_expand",
#ifdef FEAT_IPV6
	"+ipv6",
#ಅನ್ಯಥಾ
	"-ipv6",
#endif
#ifdef FEAT_JOB_CHANNEL
	"+job",
#ಅನ್ಯಥಾ
	"-job",
#endif
	"+jumplist",
#ifdef FEAT_KEYMAP
	"+keymap",
#ಅನ್ಯಥಾ
	"-keymap",
#endif
#ifdef FEAT_EVAL
	"+lambda",
#ಅನ್ಯಥಾ
	"-lambda",
#endif
#ifdef FEAT_LANGMAP
	"+langmap",
#ಅನ್ಯಥಾ
	"-langmap",
#endif
#ifdef FEAT_LIBCALL
	"+libcall",
#ಅನ್ಯಥಾ
	"-libcall",
#endif
#ifdef FEAT_LINEBREAK
	"+linebreak",
#ಅನ್ಯಥಾ
	"-linebreak",
#endif
	"+lispindent",
	"+listcmds",
	"+localmap",
#ifdef FEAT_LUA
# ifdef DYNAMIC_LUA
	"+lua/dyn",
# ಅನ್ಯಥಾ
	"+lua",
# endif
#ಅನ್ಯಥಾ
	"-lua",
#endif
#ifdef FEAT_MENU
	"+menu",
#ಅನ್ಯಥಾ
	"-menu",
#endif
#ifdef FEAT_SESSION
	"+mksession",
#ಅನ್ಯಥಾ
	"-mksession",
#endif
	"+modify_fname",
	"+mouse",
#ಯದಿ defined(UNIX) || defined(VMS)
# ifdef FEAT_MOUSE_DEC
	"+mouse_dec",
# ಅನ್ಯಥಾ
	"-mouse_dec",
# endif
# ifdef FEAT_MOUSE_GPM
#  ifdef DYNAMIC_GPM
	"+mouse_gpm/dyn",
#  ಅನ್ಯಥಾ
	"+mouse_gpm",
#  endif
# ಅನ್ಯಥಾ
	"-mouse_gpm",
# endif
# ifdef FEAT_MOUSE_JSB
	"+mouse_jsbterm",
# ಅನ್ಯಥಾ
	"-mouse_jsbterm",
# endif
# ifdef FEAT_MOUSE_NET
	"+mouse_netterm",
# ಅನ್ಯಥಾ
	"-mouse_netterm",
# endif
#endif

#ifdef __QNX__
# ifdef FEAT_MOUSE_PTERM
	"+mouse_pterm",
# ಅನ್ಯಥಾ
	"-mouse_pterm",
# endif
#endif

#ಯದಿ defined(UNIX) || defined(VMS)
	"+mouse_sgr",
# ifdef FEAT_SYSMOUSE
	"+mouse_sysmouse",
# ಅನ್ಯಥಾ
	"-mouse_sysmouse",
# endif
# ifdef FEAT_MOUSE_URXVT
	"+mouse_urxvt",
# ಅನ್ಯಥಾ
	"-mouse_urxvt",
# endif
	"+mouse_xterm",
#endif
#ifdef FEAT_MOUSESHAPE
	"+mouseshape",
#ಅನ್ಯಥಾ
	"-mouseshape",
#endif

#ifdef FEAT_MBYTE_IME
# ifdef DYNAMIC_IME
	"+multi_byte_ime/dyn",
# ಅನ್ಯಥಾ
	"+multi_byte_ime",
# endif
#ಅನ್ಯಥಾ
	"+multi_byte",
#endif
#ifdef FEAT_MULTI_LANG
	"+multi_lang",
#ಅನ್ಯಥಾ
	"-multi_lang",
#endif
#ifdef FEAT_MZSCHEME
# ifdef DYNAMIC_MZSCHEME
	"+mzscheme/dyn",
# ಅನ್ಯಥಾ
	"+mzscheme",
# endif
#ಅನ್ಯಥಾ
	"-mzscheme",
#endif
#ifdef FEAT_NETBEANS_INTG
	"+netbeans_intg",
#ಅನ್ಯಥಾ
	"-netbeans_intg",
#endif
	"+num64",
#ifdef FEAT_GUI_MSWIN
# ifdef FEAT_OLE
	"+ole",
# ಅನ್ಯಥಾ
	"-ole",
# endif
#endif
#ifdef FEAT_EVAL
	"+packages",
#ಅನ್ಯಥಾ
	"-packages",
#endif
#ifdef FEAT_PRINT_PANGO
	"+pango",
#ಅನ್ಯಥಾ
	"-pango",
#endif
	"+path_extra",
#ifdef FEAT_PERL
# ifdef DYNAMIC_PERL
	"+perl/dyn",
# ಅನ್ಯಥಾ
	"+perl",
# endif
#ಅನ್ಯಥಾ
	"-perl",
#endif
#ifdef FEAT_PERSISTENT_UNDO
	"+persistent_undo",
#ಅನ್ಯಥಾ
	"-persistent_undo",
#endif
#ifdef FEAT_PROP_POPUP
	"+popupwin",
#ಅನ್ಯಥಾ
	"-popupwin",
#endif
#ifdef FEAT_PRINTER
# ifdef FEAT_POSTSCRIPT
	"+postscript",
# ಅನ್ಯಥಾ
	"-postscript",
# endif
	"+printer",
#ಅನ್ಯಥಾ
	"-printer",
#endif
#ifdef FEAT_PROFILE
	"+profile",
#ಅನ್ಯಥಾ
	"-profile",
#endif
#ifdef FEAT_PYTHON
# ifdef DYNAMIC_PYTHON
	"+python/dyn",
# ಅನ್ಯಥಾ
	"+python",
# endif
#ಅನ್ಯಥಾ
	"-python",
#endif
#ifdef FEAT_PYTHON3
# ifdef DYNAMIC_PYTHON3
#  ifdef DYNAMIC_PYTHON3_STABLE_ABI
	"+python3/dyn-stable",
#  ಅನ್ಯಥಾ
	"+python3/dyn",
#  endif
# ಅನ್ಯಥಾ
	"+python3",
# endif
#ಅನ್ಯಥಾ
	"-python3",
#endif
#ifdef FEAT_QUICKFIX
	"+quickfix",
#ಅನ್ಯಥಾ
	"-quickfix",
#endif
#ifdef FEAT_RELTIME
	"+reltime",
#ಅನ್ಯಥಾ
	"-reltime",
#endif
#ifdef FEAT_RIGHTLEFT
	"+rightleft",
#ಅನ್ಯಥಾ
	"-rightleft",
#endif
#ifdef FEAT_RUBY
# ifdef DYNAMIC_RUBY
	"+ruby/dyn",
# ಅನ್ಯಥಾ
	"+ruby",
# endif
#ಅನ್ಯಥಾ
	"-ruby",
#endif
	"+scrollbind",
#ifdef FEAT_SIGNS
	"+signs",
#ಅನ್ಯಥಾ
	"-signs",
#endif
	"+smartindent",
#ifdef FEAT_SOCKETSERVER
	"+socketserver",
#ಅನ್ಯಥಾ
	"-socketserver",
#endif
#ifdef FEAT_SODIUM
# ifdef DYNAMIC_SODIUM
	"+sodium/dyn",
# ಅನ್ಯಥಾ
	"+sodium",
# endif
#ಅನ್ಯಥಾ
	"-sodium",
#endif
#ifdef FEAT_SOUND
	"+sound",
#ಅನ್ಯಥಾ
	"-sound",
#endif
#ifdef FEAT_SPELL
	"+spell",
#ಅನ್ಯಥಾ
	"-spell",
#endif
#ifdef STARTUPTIME
	"+startuptime",
#ಅನ್ಯಥಾ
	"-startuptime",
#endif
#ifdef FEAT_STL_OPT
	"+statusline",
#ಅನ್ಯಥಾ
	"-statusline",
#endif
	"-sun_workshop",
#ifdef FEAT_SYN_HL
	"+syntax",
#ಅನ್ಯಥಾ
	"-syntax",
#endif
	    // only interesting on Unix systems
#ಯದಿ defined(USE_SYSTEM) && defined(UNIX)
	"+system()",
#endif
#ಯದಿ defined(FEAT_TABPANEL)
	"+tabpanel",
#ಅನ್ಯಥಾ
	"-tabpanel",
#endif
	"-tag_any_white",
	"+tag_binary",
	"-tag_old_static",
#ifdef FEAT_TCL
# ifdef DYNAMIC_TCL
	"+tcl/dyn",
# ಅನ್ಯಥಾ
	"+tcl",
# endif
#ಅನ್ಯಥಾ
	"-tcl",
#endif
#ifdef FEAT_TERMGUICOLORS
	"+termguicolors",
#ಅನ್ಯಥಾ
	"-termguicolors",
#endif
#ifdef FEAT_TERMINAL
	"+terminal",
#ಅನ್ಯಥಾ
	"-terminal",
#endif
#ಯದಿ defined(UNIX)
// only Unix can have terminfo instead of termcap
# ifdef TERMINFO
	"+terminfo",
# ಅನ್ಯಥಾ
	"-terminfo",
# endif
#endif
#ifdef FEAT_TERMRESPONSE
	"+termresponse",
#ಅನ್ಯಥಾ
	"-termresponse",
#endif
	"+textobjects",
#ifdef FEAT_PROP_POPUP
	"+textprop",
#ಅನ್ಯಥಾ
	"-textprop",
#endif
#ಯದಿ !defined(UNIX)
// unix always includes termcap support
# ifdef HAVE_TGETENT
	"+tgetent",
# ಅನ್ಯಥಾ
	"-tgetent",
# endif
#endif
#ifdef FEAT_TIMERS
	"+timers",
#ಅನ್ಯಥಾ
	"-timers",
#endif
	"+title",
#ifdef FEAT_TOOLBAR
	"+toolbar",
#ಅನ್ಯಥಾ
	"-toolbar",
#endif
	"+user_commands",
#ifdef FEAT_VARTABS
	"+vartabs",
#ಅನ್ಯಥಾ
	"-vartabs",
#endif
	"+vertsplit",
	"+vim9script",
#ifdef FEAT_VIMINFO
	"+viminfo",
#ಅನ್ಯಥಾ
	"-viminfo",
#endif
	"+virtualedit",
	"+visual",
	"+visualextra",
	"+vreplace",
#ifdef MSWIN
# ifdef FEAT_VTP
	"+vtp",
# ಅನ್ಯಥಾ
	"-vtp",
# endif
#endif
#ifdef FEAT_WAYLAND
	"+wayland",
#ಅನ್ಯಥಾ
	"-wayland",
#endif
#ifdef FEAT_WAYLAND_CLIPBOARD
	"+wayland_clipboard",
#ಅನ್ಯಥಾ
	"-wayland_clipboard",
#endif
	"+wildignore",
	"+wildmenu",
	"+windows",
#ifdef FEAT_WRITEBACKUP
	"+writebackup",
#ಅನ್ಯಥಾ
	"-writebackup",
#endif
#ಯದಿ defined(UNIX) || defined(VMS)
# ifdef FEAT_X11
	"+X11",
# ಅನ್ಯಥಾ
	"-X11",
# endif
#endif
#ifdef FEAT_XATTR
	"+xattr",
#ಅನ್ಯಥಾ
	"-xattr",
#endif
#ifdef FEAT_XFONTSET
	"+xfontset",
#ಅನ್ಯಥಾ
	"-xfontset",
#endif
#ifdef FEAT_XIM
	"+xim",
#ಅನ್ಯಥಾ
	"-xim",
#endif
#ಯದಿ defined(MSWIN)
# ifdef FEAT_XPM_W32
	"+xpm_w32",
# ಅನ್ಯಥಾ
	"-xpm_w32",
# endif
#ನೋಚೇದ್ಯದಿ defined(HAVE_XPM)
	"+xpm",
#ಅನ್ಯಥಾ
	"-xpm",
#endif
#ಯದಿ defined(UNIX) || defined(VMS)
# ಯದಿ defined(USE_XSMP_INTERACT)
	"+xsmp_interact",
# elif defined(USE_XSMP)
	"+xsmp",
# ಅನ್ಯಥಾ
	"-xsmp",
# endif
# ifdef FEAT_XCLIPBOARD
	"+xterm_clipboard",
# ಅನ್ಯಥಾ
	"-xterm_clipboard",
# endif
#endif
#ifdef FEAT_XTERM_SAVE
	"+xterm_save",
#ಅನ್ಯಥಾ
	"-xterm_save",
#endif
	ರಿಕ್ತ
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ included_patches[] =
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
ಸ್ಥಿರ ವರ್ಣ *(extra_patches[]) =
{   /* Add your patch description below this line */
/**/
    ರಿಕ್ತ
};

    ಪೂರ್ಣಾಙ್ಕ
highest_patch(ಶೂನ್ಯ)
{
    // this relies on the highest patch number to be the first entry
    ಪ್ರತ್ಯಾಗಮ included_patches[0];
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return TRUE if patch "n" has been included.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_patch(ಪೂರ್ಣಾಙ್ಕ n)
{
    ಪೂರ್ಣಾಙ್ಕ		h, m, l;

    // Perform a binary search.
    l = 0;
    h = (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(included_patches) - 1;
    ಪ್ರತಿ (;;)
    {
	m = (l + h) / 2;
	ಯದಿ (included_patches[m] == n)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	ಯದಿ (l == h)
	    ಭಙ್ಗ;
	ಯದಿ (included_patches[m] < n)
	    h = m;
	ಅನ್ಯಥಾ
	    l = m + 1;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

    ಶೂನ್ಯ
ex_version(exarg_T *eap)
{
    /*
     * Ignore a ":version 9.99" command.
     */
    ಯದಿ (*eap->arg == NUL)
    {
	ಸಂದೇಶ_putchar('\n');
	list_version();
    }
}

/*
 * Output a string for the version message.  If it's going to wrap, output a
 * newline, unless the message is too long to fit on the screen anyway.
 * When "wrap" is TRUE wrap the string in [].
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
version_ಸಂದೇಶ_wrap(char_u *s, ಪೂರ್ಣಾಙ್ಕ wrap)
{
    ಪೂರ್ಣಾಙ್ಕ		len = vim_strsize(s) + (wrap ? 2 : 0);

    ಯದಿ (!got_int && len < cmdline_width && ಸಂದೇಶ_col + len >= cmdline_width
								&& *s != '\n')
	ಸಂದೇಶ_putchar('\n');
    ಯದಿ (!got_int)
    {
	ಯದಿ (wrap)
	    ಸಂದೇಶ_puts("[");
	ಸಂದೇಶ_puts((ವರ್ಣ *)s);
	ಯದಿ (wrap)
	    ಸಂದೇಶ_puts("]");
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
version_ಸಂದೇಶ(ವರ್ಣ *s)
{
    version_ಸಂದೇಶ_wrap((char_u *)s, FALSE);
}

/*
 * List all features aligned in columns, dictionary style.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_features(ಶೂನ್ಯ)
{
    list_in_columns((char_u **)features, -1, -1);
}

/*
 * List string items nicely aligned in columns.
 * When "size" is < 0 then the last entry is marked with NULL.
 * The entry with index "current" is inclosed in [].
 */
    ಶೂನ್ಯ
list_in_columns(char_u **ವಸ್ತುಗಳು, ಪೂರ್ಣಾಙ್ಕ size, ಪೂರ್ಣಾಙ್ಕ current)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		ncol;
    ಪೂರ್ಣಾಙ್ಕ		nrow;
    ಪೂರ್ಣಾಙ್ಕ		cur_row = 1;
    ಪೂರ್ಣಾಙ್ಕ		item_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		width = 0;
#ifdef FEAT_SYN_HL
    ಪೂರ್ಣಾಙ್ಕ		use_highlight = (ವಸ್ತುಗಳು == (char_u **)features);
#endif

    // Find the length of the longest item, use that + 1 as the column
    // width.
    ಪ್ರತಿ (i = 0; size < 0 ? ವಸ್ತುಗಳು[i] != ರಿಕ್ತ : i < size; ++i)
    {
	ಪೂರ್ಣಾಙ್ಕ l = vim_strsize(ವಸ್ತುಗಳು[i]) + (i == current ? 2 : 0);

	ಯದಿ (l > width)
	    width = l;
	++item_ಎಣಿಕೆ;
    }
    width += 1;

    ಯದಿ (cmdline_width < width)
    {
	// Not enough screen columns - show one per line
	ಪ್ರತಿ (i = 0; i < item_ಎಣಿಕೆ; ++i)
	{
	    version_ಸಂದೇಶ_wrap(ವಸ್ತುಗಳು[i], i == current);
	    ಯದಿ (ಸಂದೇಶ_col > 0 && i < item_ಎಣಿಕೆ - 1)
		ಸಂದೇಶ_putchar('\n');
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    // The rightmost column doesn't need a separator.
    // Sacrifice it to fit in one more column if possible.
    ncol = (cmdline_width + 1) / width;
    nrow = item_ಎಣಿಕೆ / ncol + ((item_ಎಣಿಕೆ % ncol) ? 1 : 0);

    // "i" counts columns then rows.  "idx" counts rows then columns.
    ಪ್ರತಿ (i = 0; !got_int && i < nrow * ncol; ++i)
    {
	ಪೂರ್ಣಾಙ್ಕ idx = (i / ncol) + (i % ncol) * nrow;

	ಯದಿ (idx < item_ಎಣಿಕೆ)
	{
	    ಪೂರ್ಣಾಙ್ಕ last_col = (i + 1) % ncol == 0;

	    ಯದಿ (idx == current)
		ಸಂದೇಶ_putchar('[');
#ifdef FEAT_SYN_HL
	    ಯದಿ (use_highlight && ವಸ್ತುಗಳು[idx][0] == '-')
		ಸಂದೇಶ_puts_attr((ವರ್ಣ *)ವಸ್ತುಗಳು[idx], HL_ATTR(HLF_W));
	    ಅನ್ಯಥಾ
#endif
		ಸಂದೇಶ_puts((ವರ್ಣ *)ವಸ್ತುಗಳು[idx]);
	    ಯದಿ (idx == current)
		ಸಂದೇಶ_putchar(']');
	    ಯದಿ (last_col)
	    {
		ಯದಿ (ಸಂದೇಶ_col > 0 && cur_row < nrow)
		    ಸಂದೇಶ_putchar('\n');
		++cur_row;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯಾವತ್ (ಸಂದೇಶ_col % width)
		    ಸಂದೇಶ_putchar(' ');
	    }
	}
	ಅನ್ಯಥಾ
	{
	    // this row is out of items, thus at the end of the row
	    ಯದಿ (ಸಂದೇಶ_col > 0)
	    {
		ಯದಿ (cur_row < nrow)
		    ಸಂದೇಶ_putchar('\n');
		++cur_row;
	    }
	}
    }
}

    ಶೂನ್ಯ
list_version(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		first;
    ವರ್ಣ	*s = "";

    /*
     * When adding features here, don't forget to update the list of
     * internal variables in eval.c!
     */
    init_longVersion();
    ಸಂದೇಶ(longVersion);
#ifdef MSWIN
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
#   ifdef _WIN64
#    ಯದಿ defined(_M_ARM64) || defined(_M_ARM64EC)
     ಸಂದೇಶ_puts(_("\nMS-Windows ARM64 GUI/console version"));
#    ಅನ್ಯಥಾ
     ಸಂದೇಶ_puts(_("\nMS-Windows 64-bit GUI/console version"));
#    endif
#   ಅನ್ಯಥಾ
    ಸಂದೇಶ_puts(_("\nMS-Windows 32-bit GUI/console version"));
#   endif
#  ಅನ್ಯಥಾ
#   ifdef _WIN64
#    ಯದಿ defined(_M_ARM64) || defined(_M_ARM64EC)
     ಸಂದೇಶ_puts(_("\nMS-Windows ARM64 GUI version"));
#    ಅನ್ಯಥಾ
     ಸಂದೇಶ_puts(_("\nMS-Windows 64-bit GUI version"));
#    endif
#   ಅನ್ಯಥಾ
    ಸಂದೇಶ_puts(_("\nMS-Windows 32-bit GUI version"));
#   endif
#  endif
#  ifdef FEAT_OLE
    ಸಂದೇಶ_puts(_(" with OLE support"));
#  endif
# ಅನ್ಯಥಾ
#  ifdef _WIN64
#   ಯದಿ defined(_M_ARM64) || defined(_M_ARM64EC)
    ಸಂದೇಶ_puts(_("\nMS-Windows ARM64 console version"));
#   ಅನ್ಯಥಾ
    ಸಂದೇಶ_puts(_("\nMS-Windows 64-bit console version"));
#   endif
#  ಅನ್ಯಥಾ
    ಸಂದೇಶ_puts(_("\nMS-Windows 32-bit console version"));
#  endif
# endif
#endif
#ಯದಿ defined(MACOS_X)
# ಯದಿ defined(MACOS_X_DARWIN)
    ಸಂದೇಶ_puts(_("\nmacOS version"));
# ಅನ್ಯಥಾ
    ಸಂದೇಶ_puts(_("\nmacOS version w/o darwin feat."));
# endif
# ಯದಿ defined(__arm64__)
    ಸಂದೇಶ_puts(" - arm64");
# elif defined(__x86_64__)
    ಸಂದೇಶ_puts(" - x86_64");
# endif
#endif

#ifdef VMS
    ಸಂದೇಶ_puts(_("\nOpenVMS (build) arch, version"));
# ifdef HAVE_PATHDEF
    ಯದಿ (*compiled_arch != NUL)
    {
	ಸಂದೇಶ_puts(": ");
	ಸಂದೇಶ_puts((ವರ್ಣ *)compiled_arch);
	ಯದಿ (*compiled_vers != NUL)
	{
	    ಸಂದೇಶ_puts(", ");
	    ಸಂದೇಶ_puts((ವರ್ಣ *)compiled_vers);
	}
    }

# endif

#endif

    // Print the list of patch numbers if there is at least one.
    // Print a range when patches are consecutive: "1-10, 12, 15-40, 42-45"
    ಯದಿ (included_patches[0] != 0)
    {
	ಸಂದೇಶ_puts(_("\nIncluded patches: "));
	first = -1;
	i = (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(included_patches) - 1;
	ಯಾವತ್ (--i >= 0)
	{
	    ಯದಿ (first < 0)
		first = included_patches[i];
	    ಯದಿ (i == 0 || included_patches[i - 1] != included_patches[i] + 1)
	    {
		ಸಂದೇಶ_puts(s);
		s = ", ";
		ಸಂದೇಶ_outnum((long)first);
		ಯದಿ (first != included_patches[i])
		{
		    ಸಂದೇಶ_puts("-");
		    ಸಂದೇಶ_outnum((long)included_patches[i]);
		}
		first = -1;
	    }
	}
    }

    // Print the list of extra patch descriptions if there is at least one.
    ಯದಿ (extra_patches[0] != ರಿಕ್ತ)
    {
	ಸಂದೇಶ_puts(_("\nExtra patches: "));
	s = "";
	ಪ್ರತಿ (i = 0; extra_patches[i] != ರಿಕ್ತ; ++i)
	{
	    ಸಂದೇಶ_puts(s);
	    s = ", ";
	    ಸಂದೇಶ_puts(extra_patches[i]);
	}
    }

#ifdef MODIFIED_BY
    ಸಂದೇಶ_puts("\n");
    ಸಂದೇಶ_puts(_("Modified by "));
    ಸಂದೇಶ_puts(MODIFIED_BY);
#endif

#ifdef HAVE_PATHDEF
    ಯದಿ (*compiled_user != NUL || *compiled_sys != NUL)
    {
	ಸಂದೇಶ_puts(_("\nCompiled "));
	ಯದಿ (*compiled_user != NUL)
	{
	    ಸಂದೇಶ_puts(_("by "));
	    ಸಂದೇಶ_puts((ವರ್ಣ *)compiled_user);
	}
	ಯದಿ (*compiled_sys != NUL)
	{
	    ಸಂದೇಶ_puts("@");
	    ಸಂದೇಶ_puts((ವರ್ಣ *)compiled_sys);
	}
    }
#endif

#ಯದಿ defined(FEAT_HUGE)
    ಸಂದೇಶ_puts(_("\nHuge version "));
#ನೋಚೇದ್ಯದಿ defined(FEAT_NORMAL)
    ಸಂದೇಶ_puts(_("\nNormal version "));
#ಅನ್ಯಥಾ
    ಸಂದೇಶ_puts(_("\nTiny version "));
#endif
#ಯದಿ !defined(FEAT_GUI)
    ಸಂದೇಶ_puts(_("without GUI."));
#ನೋಚೇದ್ಯದಿ defined(FEAT_GUI_GTK)
# ಯದಿ defined(USE_GTK4)
    ಸಂದೇಶ_puts(_("with GTK4 GUI."));
# elif defined(USE_GTK3)
    ಸಂದೇಶ_puts(_("with GTK3 GUI."));
# elif defined(FEAT_GUI_GNOME)
    ಸಂದೇಶ_puts(_("with GTK2-GNOME GUI."));
# ಅನ್ಯಥಾ
    ಸಂದೇಶ_puts(_("with GTK2 GUI."));
# endif
#ನೋಚೇದ್ಯದಿ defined(FEAT_GUI_MOTIF)
    ಸಂದೇಶ_puts(_("with X11-Motif GUI."));
#ನೋಚೇದ್ಯದಿ defined(FEAT_GUI_HAIKU)
    ಸಂದೇಶ_puts(_("with Haiku GUI."));
#ನೋಚೇದ್ಯದಿ defined(FEAT_GUI_PHOTON)
    ಸಂದೇಶ_puts(_("with Photon GUI."));
#ನೋಚೇದ್ಯದಿ defined(MSWIN)
    ಸಂದೇಶ_puts(_("with GUI."));
#endif
    version_ಸಂದೇಶ(_("  Features included (+) or not (-):\n"));

    list_features();
    ಯದಿ (ಸಂದೇಶ_col > 0)
	ಸಂದೇಶ_putchar('\n');

#ifdef SYS_VIMRC_FILE
    version_ಸಂದೇಶ(_("   system vimrc file: \""));
    version_ಸಂದೇಶ(SYS_VIMRC_FILE);
    version_ಸಂದೇಶ("\"\n");
#endif
#ifdef USR_VIMRC_FILE
    version_ಸಂದೇಶ(_("     user vimrc file: \""));
    version_ಸಂದೇಶ(USR_VIMRC_FILE);
    version_ಸಂದೇಶ("\"\n");
#endif
#ifdef USR_VIMRC_FILE2
    version_ಸಂದೇಶ(_(" 2nd user vimrc file: \""));
    version_ಸಂದೇಶ(USR_VIMRC_FILE2);
    version_ಸಂದೇಶ("\"\n");
#endif
#ಯದಿ defined(USR_VIMRC_FILE3) && defined(XDG_VIMRC_FILE)
    version_ಸಂದೇಶ(_(" 3rd user vimrc file: \""));
    version_ಸಂದೇಶ(USR_VIMRC_FILE3);
    version_ಸಂದೇಶ("\"\n");
    version_ಸಂದೇಶ(_(" 4th user vimrc file: \""));
    version_ಸಂದೇಶ((ವರ್ಣ *)(XDG_VIMRC_FILE));
    version_ಸಂದೇಶ("\"\n");
#ನೋಚೇದ್ಯದಿ defined(USR_VIMRC_FILE3)
    version_ಸಂದೇಶ(_(" 3rd user vimrc file: \""));
    version_ಸಂದೇಶ(USR_VIMRC_FILE3);
    version_ಸಂದೇಶ("\"\n");
#ನೋಚೇದ್ಯದಿ defined(XDG_VIMRC_FILE)
    version_ಸಂದೇಶ(_(" 3rd user vimrc file: \""));
    version_ಸಂದೇಶ((ವರ್ಣ *)(XDG_VIMRC_FILE));
    version_ಸಂದೇಶ("\"\n");
#endif
#ifdef USR_EXRC_FILE
    version_ಸಂದೇಶ(_("      user exrc file: \""));
    version_ಸಂದೇಶ(USR_EXRC_FILE);
    version_ಸಂದೇಶ("\"\n");
#endif
#ifdef USR_EXRC_FILE2
    version_ಸಂದೇಶ(_("  2nd user exrc file: \""));
    version_ಸಂದೇಶ(USR_EXRC_FILE2);
    version_ಸಂದೇಶ("\"\n");
#endif
#ifdef FEAT_GUI
# ifdef SYS_GVIMRC_FILE
    version_ಸಂದೇಶ(_("  system gvimrc file: \""));
    version_ಸಂದೇಶ(SYS_GVIMRC_FILE);
    version_ಸಂದೇಶ("\"\n");
# endif
    version_ಸಂದೇಶ(_("    user gvimrc file: \""));
    version_ಸಂದೇಶ(USR_GVIMRC_FILE);
    version_ಸಂದೇಶ("\"\n");
# ifdef USR_GVIMRC_FILE2
    version_ಸಂದೇಶ(_("2nd user gvimrc file: \""));
    version_ಸಂದೇಶ(USR_GVIMRC_FILE2);
    version_ಸಂದೇಶ("\"\n");
# endif
# ifdef USR_GVIMRC_FILE3
    version_ಸಂದೇಶ(_("3rd user gvimrc file: \""));
    version_ಸಂದೇಶ(USR_GVIMRC_FILE3);
    version_ಸಂದೇಶ("\"\n");
# endif
#endif
    version_ಸಂದೇಶ(_("       defaults file: \""));
    version_ಸಂದೇಶ(VIM_DEFAULTS_FILE);
    version_ಸಂದೇಶ("\"\n");
#ifdef FEAT_GUI
# ifdef SYS_MENU_FILE
    version_ಸಂದೇಶ(_("    system menu file: \""));
    version_ಸಂದೇಶ(SYS_MENU_FILE);
    version_ಸಂದೇಶ("\"\n");
# endif
#endif
#ifdef HAVE_PATHDEF
    ಯದಿ (*default_vim_dir != NUL)
    {
	version_ಸಂದೇಶ(_("  fall-back for $VIM: \""));
	version_ಸಂದೇಶ((ವರ್ಣ *)default_vim_dir);
	version_ಸಂದೇಶ("\"\n");
    }
    ಯದಿ (*default_vimruntime_dir != NUL)
    {
	version_ಸಂದೇಶ(_(" f-b for $VIMRUNTIME: \""));
	version_ಸಂದೇಶ((ವರ್ಣ *)default_vimruntime_dir);
	version_ಸಂದೇಶ("\"\n");
    }
    version_ಸಂದೇಶ(_("Compilation: "));
    version_ಸಂದೇಶ((ವರ್ಣ *)all_cflags);
    version_ಸಂದೇಶ("\n");
# ifdef VMS
    ಯದಿ (*compiler_version != NUL)
    {
	version_ಸಂದೇಶ(_("Compiler: "));
	version_ಸಂದೇಶ((ವರ್ಣ *)compiler_version);
	version_ಸಂದೇಶ("\n");
    }
# endif
    version_ಸಂದೇಶ(_("Linking: "));
    version_ಸಂದೇಶ((ವರ್ಣ *)all_lflags);
#endif
#ifdef DEBUG
    version_ಸಂದೇಶ("\n");
    version_ಸಂದೇಶ(_("  DEBUG BUILD"));
#endif
}

ಸ್ಥಿರ ಶೂನ್ಯ do_intro_ಸಾಲು(ಪೂರ್ಣಾಙ್ಕ row, char_u *mesg, ಪೂರ್ಣಾಙ್ಕ add_version, ಪೂರ್ಣಾಙ್ಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ intro_message(ಪೂರ್ಣಾಙ್ಕ colon);

/*
 * Show the intro message when not editing a file.
 */
    ಶೂನ್ಯ
maybe_intro_message(ಶೂನ್ಯ)
{
    ಯದಿ (BUFEMPTY()
	    && curbuf->b_fname == ರಿಕ್ತ
	    && firstwin->w_next == ರಿಕ್ತ
	    && vim_strchr(p_shm, SHM_INTRO) == ರಿಕ್ತ)
	intro_message(FALSE);
}

/*
 * Give an introductory message about Vim.
 * Only used when starting Vim on an empty file, without a file name.
 * Or with the ":intro" command (for Sven :-).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
intro_message(
    ಪೂರ್ಣಾಙ್ಕ		colon)		// TRUE for ":intro"
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		row;
    ಪೂರ್ಣಾಙ್ಕ		blanklines;
    ಪೂರ್ಣಾಙ್ಕ		sponsor;
    ವರ್ಣ	*p;
    ಸ್ಥಿರ ವರ್ಣ	*(ಸಾಲುಗಳು[]) =
    {
	N_("VIM - Vi IMproved"),
	"",
	N_("version "),
	N_("by Bram Moolenaar et al."),
#ifdef MODIFIED_BY
	" ",
#endif
	N_("Vim is open source and freely distributable"),
	"",
	N_("Help poor children in Uganda!"),
	N_("type  :help Kuwasha<Enter>    for information "),
	"",
	N_("type  :q<Enter>               to exit         "),
	N_("type  :help<Enter>  or  <F1>  for on-line help"),
	N_("type  :help version9<Enter>   for version info"),
	ರಿಕ್ತ,
	"",
	N_("Running in Vi compatible mode"),
	N_("type  :set nocp<Enter>        for Vim defaults"),
	N_("type  :help cp-default<Enter> for info on this"),
    };
#ifdef FEAT_GUI
    ಸ್ಥಿರ ವರ್ಣ	*(gui_ಸಾಲುಗಳು[]) =
    {
	ರಿಕ್ತ,
	ರಿಕ್ತ,
	ರಿಕ್ತ,
	ರಿಕ್ತ,
# ifdef MODIFIED_BY
	ರಿಕ್ತ,
# endif
	ರಿಕ್ತ,
	ರಿಕ್ತ,
	ರಿಕ್ತ,
	N_("menu  Help->Orphans           for information    "),
	ರಿಕ್ತ,
	N_("Running modeless, typed text is inserted"),
	N_("menu  Edit->Global Settings->Toggle Insert Mode  "),
	N_("                              for two modes      "),
	ರಿಕ್ತ,
	ರಿಕ್ತ,
	ರಿಕ್ತ,
	N_("menu  Edit->Global Settings->Toggle Vi Compatible"),
	N_("                              for Vim defaults   "),
    };
#endif

    // blanklines = screen height - # message lines
    blanklines = (ಪೂರ್ಣಾಙ್ಕ)Rows - (ARRAY_LENGTH(ಸಾಲುಗಳು) - 1);
    ಯದಿ (!p_cp)
	blanklines += 4;  // add 4 for not showing "Vi compatible" message

    // Don't overwrite a statusline.  Depends on 'cmdheight'.
    ಯದಿ (p_ls > 1)
	blanklines -= Rows - topframe->fr_height;
    ಯದಿ (blanklines < 0)
	blanklines = 0;

    // Show the sponsor and Uganda message two out of four times
    sponsor = (ಪೂರ್ಣಾಙ್ಕ)time(ರಿಕ್ತ);
    sponsor = ((sponsor & 2) == 0) - ((sponsor & 4) == 0);

    // start displaying the message lines after half of the blank lines
    row = blanklines / 2;
    ಯದಿ ((row >= 2 && topframe->fr_width >= 50) || colon)
    {
	ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(ಸಾಲುಗಳು); ++i)
	{
	    p = ಸಾಲುಗಳು[i];
#ifdef FEAT_GUI
	    ಯದಿ (p_im && gui.in_use && gui_ಸಾಲುಗಳು[i] != ರಿಕ್ತ)
		p = gui_ಸಾಲುಗಳು[i];
#endif
	    ಯದಿ (p == ರಿಕ್ತ)
	    {
		ಯದಿ (!p_cp)
		    ಭಙ್ಗ;
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (sponsor != 0)
	    {
		ಯದಿ (strstr(p, "children") != ರಿಕ್ತ)
		    p = N_("Sponsor Vim development!");
		ಅನ್ಯಥಾ ಯದಿ (strstr(p, "Kuwasha") != ರಿಕ್ತ)
		    p = N_("type  :help sponsor<Enter>    for information ");
		ಅನ್ಯಥಾ ಯದಿ (strstr(p, "Orphans") != ರಿಕ್ತ)
		    p = N_("menu  Help->Sponsor           for information    ");
	    }
	    ಯದಿ (*p != NUL)
		do_intro_ಸಾಲು(row, (char_u *)_(p), i == 2, 0);
	    ++row;
	}
    }

    // Make the wait-return message appear just below the text.
    ಯದಿ (colon)
	ಸಂದೇಶ_row = row;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_intro_ಸಾಲು(
    ಪೂರ್ಣಾಙ್ಕ		row,
    char_u	*mesg,
    ಪೂರ್ಣಾಙ್ಕ		add_version,
    ಪೂರ್ಣಾಙ್ಕ		attr)
{
    char_u	vers[20];
    ಪೂರ್ಣಾಙ್ಕ		col;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		l;
    ಪೂರ್ಣಾಙ್ಕ		clen;
#ifdef MODIFIED_BY
# define MODBY_LEN 150
    char_u	modby[MODBY_LEN];

    ಯದಿ (*mesg == ' ')
    {
	vim_strncpy(modby, (char_u *)_("Modified by "), MODBY_LEN - 1);
	l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(modby);
	vim_strncpy(modby + l, (char_u *)MODIFIED_BY, MODBY_LEN - l - 1);
	mesg = modby;
    }
#endif

    // Center the message horizontally.
    col = vim_strsize(mesg);
    ಯದಿ (add_version)
    {
	STRCPY(vers, mediumVersion);
	ಯದಿ (highest_patch())
	{
	    // Check for 9.9x or 9.9xx, alpha/beta version
	    ಯದಿ (SAFE_isalpha((ಪೂರ್ಣಾಙ್ಕ)vers[3]))
	    {
		ಪೂರ್ಣಾಙ್ಕ len = (SAFE_isalpha((ಪೂರ್ಣಾಙ್ಕ)vers[4])) ? 5 : 4;
		sprintf((ವರ್ಣ *)vers + len, ".%d%s", highest_patch(),
							 mediumVersion + len);
	    }
	    ಅನ್ಯಥಾ
		sprintf((ವರ್ಣ *)vers + 3, ".%d", highest_patch());
	}
	col += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(vers);
    }
    col = (topframe->fr_width - col) / 2;
    ಯದಿ (col < 0)
	col = 0;

    // Split up in parts to highlight <> items differently.
    ಪ್ರತಿ (p = mesg; *p != NUL; p += l)
    {
	clen = 0;
	ಪ್ರತಿ (l = 0; p[l] != NUL
			 && (l == 0 || (p[l] != '<' && p[l - 1] != '>')); ++l)
	{
	    ಯದಿ (has_mbyte)
	    {
		clen += ptr2cells(p + l);
		l += (*mb_ptr2len)(p + l) - 1;
	    }
	    ಅನ್ಯಥಾ
		clen += byte2cells(p[l]);
	}
	screen_puts_len(p, l, row, col + firstwin->w_wincol,
		*p == '<' ? HL_ATTR(HLF_8) : attr);
	col += clen;
    }

    // Add the version number to the version line.
    ಯದಿ (add_version)
	screen_puts(vers, row, col + firstwin->w_wincol, 0);
}

/*
 * ":intro": clear screen, display intro screen and wait for return.
 */
    ಶೂನ್ಯ
ex_intro(exarg_T *eap UNUSED)
{
    screenclear();
#ಯದಿ defined(FEAT_TABPANEL)
    draw_tabpanel();
#endif
    intro_message(TRUE);
    wait_return(TRUE);
}
