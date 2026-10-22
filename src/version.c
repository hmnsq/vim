/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#અન્તર્ભાવ "vim.h"

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

#અન્તર્ભાવ "version.h"

અક્ષર		*Version = VIM_VERSION_SHORT;
સ્થિર અક્ષર	*mediumVersion = VIM_VERSION_MEDIUM;

#જો defined(HAVE_DATE_TIME)
# જો defined(VMS) && defined(VAXC)
અક્ષર	longVersion[કદ(VIM_VERSION_LONG_DATE) + કદ(__DATE__)
						      + કદ(__TIME__) + 3];

    શૂન્ય
init_longVersion(શૂન્ય)
{
    /*
     * Construct the long version string.  Necessary because
     * VAX C can't concatenate strings in the preprocessor.
     */
    strcpy(longVersion, VIM_VERSION_LONG_DATE);
#  ifdef BUILD_DATE
    strcat(longVersion, BUILD_DATE);
#  નહીંતર
    strcat(longVersion, __DATE__);
    strcat(longVersion, " ");
    strcat(longVersion, __TIME__);
#  endif
    strcat(longVersion, ")");
}

# નહીંતર
અક્ષર	*longVersion = ખાલી;

    શૂન્ય
init_longVersion(શૂન્ય)
{
    જો (longVersion != ખાલી)
	પાછા;

#  ifdef BUILD_DATE
    અક્ષર *date_time = BUILD_DATE;
#  નહીંતર
    અક્ષર *date_time = __DATE__ " " __TIME__;
#  endif
    અક્ષર *સંદેશ = _("%s (%s, compiled %s)");
    size_t len = strlen(સંદેશ)
	+ STRLEN_LITERAL(VIM_VERSION_LONG_ONLY)
	+ STRLEN_LITERAL(VIM_VERSION_DATE_ONLY)
	+ strlen(date_time);

    longVersion = alloc(len);
    જો (longVersion == ખાલી)
	longVersion = VIM_VERSION_LONG;
    નહીંતર
	vim_snprintf(longVersion, len, સંદેશ,
		VIM_VERSION_LONG_ONLY, VIM_VERSION_DATE_ONLY, date_time);
}
# endif
#નહીંતર
અક્ષર	*longVersion = VIM_VERSION_LONG;

    શૂન્ય
init_longVersion(શૂન્ય)
{
    // nothing to do
}
#endif

સ્થિર અક્ષર *(features[]) =
{
#ifdef HAVE_ACL
	"+acl",
#નહીંતર
	"-acl",
#endif
#ifdef FEAT_ARABIC
	"+arabic",
#નહીંતર
	"-arabic",
#endif
#ifdef AMIGA		// only for Amiga systems
# ifdef FEAT_ARP
	"+ARP",
# નહીંતર
	"-ARP",
# endif
#endif
#ifdef FEAT_AUTOCHDIR
	"+autochdir",
#નહીંતર
	"-autochdir",
#endif
	"+autocmd",
#ifdef FEAT_AUTOSERVERNAME
	"+autoservername",
#નહીંતર
	"-autoservername",
#endif
#ifdef FEAT_BEVAL_GUI
	"+balloon_eval",
#નહીંતર
	"-balloon_eval",
#endif
#ifdef FEAT_BEVAL_TERM
	"+balloon_eval_term",
#નહીંતર
	"-balloon_eval_term",
#endif
#ifdef FEAT_BROWSE
	"+browse",
#નહીંતર
	"-browse",
#endif
	"++builtin_terms",
#ifdef FEAT_BYTEOFF
	"+byte_offset",
#નહીંતર
	"-byte_offset",
#endif
#ifdef FEAT_JOB_CHANNEL
	"+channel",
#નહીંતર
	"-channel",
#endif
	"+cindent",
#ifdef FEAT_CLIENTSERVER
	"+clientserver",
#નહીંતર
	"-clientserver",
#endif
#ifdef FEAT_CLIPBOARD
	"+clipboard",
#નહીંતર
	"-clipboard",
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	"+clipboard_provider",
#નહીંતર
	"-clipboard_provider",
#endif
	"+cmdline_compl",
	"+cmdline_hist",
	"+cmdline_info",
	"+comments",
#ifdef FEAT_CONCEAL
	"+conceal",
#નહીંતર
	"-conceal",
#endif
#ifdef FEAT_CRYPT
	"+cryptv",
#નહીંતર
	"-cryptv",
#endif
#ifdef FEAT_CSCOPE
	"+cscope",
#નહીંતર
	"-cscope",
#endif
	"+cursorbind",
#ifdef CURSOR_SHAPE
	"+cursorshape",
#નહીંતર
	"-cursorshape",
#endif
#જો defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
	"+dialog_con_gui",
#નહીંતર
# જો defined(FEAT_CON_DIALOG)
	"+dialog_con",
# નહીંતર
#  જો defined(FEAT_GUI_DIALOG)
	"+dialog_gui",
#  નહીંતર
	"-dialog",
#  endif
# endif
#endif
#ifdef FEAT_DIFF
	"+diff",
#નહીંતર
	"-diff",
#endif
#ifdef FEAT_DIGRAPHS
	"+digraphs",
#નહીંતર
	"-digraphs",
#endif
#ifdef FEAT_GUI_MSWIN
# ifdef FEAT_DIRECTX
	"+directx",
# નહીંતર
	"-directx",
# endif
#endif
#ifdef FEAT_DND
	"+dnd",
#નહીંતર
	"-dnd",
#endif
	"-ebcdic",
#ifdef FEAT_EMACS_TAGS
	"+emacs_tags",
#નહીંતર
	"-emacs_tags",
#endif
#ifdef FEAT_EVAL
	"+eval",
#નહીંતર
	"-eval",
#endif
	"+ex_extra",
#ifdef FEAT_શોધ_EXTRA
	"+extra_search",
#નહીંતર
	"-extra_search",
#endif
	"-farsi",
	"+file_in_path",
#ifdef FEAT_FIND_ID
	"+find_in_path",
#નહીંતર
	"-find_in_path",
#endif
	"+float",
#ifdef FEAT_FOLDING
	"+folding",
#નહીંતર
	"-folding",
#endif
	"-footer",
	// only interesting on Unix systems
#જો !defined(USE_SYSTEM) && defined(UNIX)
	"+fork()",
#endif
#ifdef FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
	"+gettext/dyn",
# નહીંતર
	"+gettext",
# endif
#નહીંતર
	"-gettext",
#endif
	"-hangul_input",
#જો (defined(HAVE_ICONV_H) && defined(USE_ICONV)) || defined(DYNAMIC_ICONV)
# ifdef DYNAMIC_ICONV
	"+iconv/dyn",
# નહીંતર
	"+iconv",
# endif
#નહીંતર
	"-iconv",
#endif
#ifdef FEAT_IMAGE
	"+image",
#નહીંતર
	"-image",
#endif
#ifdef FEAT_IMAGE_POPUP
	"+image_popup",
#નહીંતર
	"-image_popup",
#endif
	"+insert_expand",
#ifdef FEAT_IPV6
	"+ipv6",
#નહીંતર
	"-ipv6",
#endif
#ifdef FEAT_JOB_CHANNEL
	"+job",
#નહીંતર
	"-job",
#endif
	"+jumplist",
#ifdef FEAT_KEYMAP
	"+keymap",
#નહીંતર
	"-keymap",
#endif
#ifdef FEAT_EVAL
	"+lambda",
#નહીંતર
	"-lambda",
#endif
#ifdef FEAT_LANGMAP
	"+langmap",
#નહીંતર
	"-langmap",
#endif
#ifdef FEAT_LIBCALL
	"+libcall",
#નહીંતર
	"-libcall",
#endif
#ifdef FEAT_LINEBREAK
	"+linebreak",
#નહીંતર
	"-linebreak",
#endif
	"+lispindent",
	"+listcmds",
	"+localmap",
#ifdef FEAT_LUA
# ifdef DYNAMIC_LUA
	"+lua/dyn",
# નહીંતર
	"+lua",
# endif
#નહીંતર
	"-lua",
#endif
#ifdef FEAT_MENU
	"+menu",
#નહીંતર
	"-menu",
#endif
#ifdef FEAT_SESSION
	"+mksession",
#નહીંતર
	"-mksession",
#endif
	"+modify_fname",
	"+mouse",
#જો defined(UNIX) || defined(VMS)
# ifdef FEAT_MOUSE_DEC
	"+mouse_dec",
# નહીંતર
	"-mouse_dec",
# endif
# ifdef FEAT_MOUSE_GPM
#  ifdef DYNAMIC_GPM
	"+mouse_gpm/dyn",
#  નહીંતર
	"+mouse_gpm",
#  endif
# નહીંતર
	"-mouse_gpm",
# endif
# ifdef FEAT_MOUSE_JSB
	"+mouse_jsbterm",
# નહીંતર
	"-mouse_jsbterm",
# endif
# ifdef FEAT_MOUSE_NET
	"+mouse_netterm",
# નહીંતર
	"-mouse_netterm",
# endif
#endif

#ifdef __QNX__
# ifdef FEAT_MOUSE_PTERM
	"+mouse_pterm",
# નહીંતર
	"-mouse_pterm",
# endif
#endif

#જો defined(UNIX) || defined(VMS)
	"+mouse_sgr",
# ifdef FEAT_SYSMOUSE
	"+mouse_sysmouse",
# નહીંતર
	"-mouse_sysmouse",
# endif
# ifdef FEAT_MOUSE_URXVT
	"+mouse_urxvt",
# નહીંતર
	"-mouse_urxvt",
# endif
	"+mouse_xterm",
#endif
#ifdef FEAT_MOUSESHAPE
	"+mouseshape",
#નહીંતર
	"-mouseshape",
#endif

#ifdef FEAT_MBYTE_IME
# ifdef DYNAMIC_IME
	"+multi_byte_ime/dyn",
# નહીંતર
	"+multi_byte_ime",
# endif
#નહીંતર
	"+multi_byte",
#endif
#ifdef FEAT_MULTI_LANG
	"+multi_lang",
#નહીંતર
	"-multi_lang",
#endif
#ifdef FEAT_MZSCHEME
# ifdef DYNAMIC_MZSCHEME
	"+mzscheme/dyn",
# નહીંતર
	"+mzscheme",
# endif
#નહીંતર
	"-mzscheme",
#endif
#ifdef FEAT_NETBEANS_INTG
	"+netbeans_intg",
#નહીંતર
	"-netbeans_intg",
#endif
	"+num64",
#ifdef FEAT_GUI_MSWIN
# ifdef FEAT_OLE
	"+ole",
# નહીંતર
	"-ole",
# endif
#endif
#ifdef FEAT_EVAL
	"+packages",
#નહીંતર
	"-packages",
#endif
#ifdef FEAT_PRINT_PANGO
	"+pango",
#નહીંતર
	"-pango",
#endif
	"+path_extra",
#ifdef FEAT_PERL
# ifdef DYNAMIC_PERL
	"+perl/dyn",
# નહીંતર
	"+perl",
# endif
#નહીંતર
	"-perl",
#endif
#ifdef FEAT_PERSISTENT_UNDO
	"+persistent_undo",
#નહીંતર
	"-persistent_undo",
#endif
#ifdef FEAT_PROP_POPUP
	"+popupwin",
#નહીંતર
	"-popupwin",
#endif
#ifdef FEAT_PRINTER
# ifdef FEAT_POSTSCRIPT
	"+postscript",
# નહીંતર
	"-postscript",
# endif
	"+printer",
#નહીંતર
	"-printer",
#endif
#ifdef FEAT_PROFILE
	"+profile",
#નહીંતર
	"-profile",
#endif
#ifdef FEAT_PYTHON
# ifdef DYNAMIC_PYTHON
	"+python/dyn",
# નહીંતર
	"+python",
# endif
#નહીંતર
	"-python",
#endif
#ifdef FEAT_PYTHON3
# ifdef DYNAMIC_PYTHON3
#  ifdef DYNAMIC_PYTHON3_STABLE_ABI
	"+python3/dyn-stable",
#  નહીંતર
	"+python3/dyn",
#  endif
# નહીંતર
	"+python3",
# endif
#નહીંતર
	"-python3",
#endif
#ifdef FEAT_QUICKFIX
	"+quickfix",
#નહીંતર
	"-quickfix",
#endif
#ifdef FEAT_RELTIME
	"+reltime",
#નહીંતર
	"-reltime",
#endif
#ifdef FEAT_RIGHTLEFT
	"+rightleft",
#નહીંતર
	"-rightleft",
#endif
#ifdef FEAT_RUBY
# ifdef DYNAMIC_RUBY
	"+ruby/dyn",
# નહીંતર
	"+ruby",
# endif
#નહીંતર
	"-ruby",
#endif
	"+scrollbind",
#ifdef FEAT_SIGNS
	"+signs",
#નહીંતર
	"-signs",
#endif
	"+smartindent",
#ifdef FEAT_SOCKETSERVER
	"+socketserver",
#નહીંતર
	"-socketserver",
#endif
#ifdef FEAT_SODIUM
# ifdef DYNAMIC_SODIUM
	"+sodium/dyn",
# નહીંતર
	"+sodium",
# endif
#નહીંતર
	"-sodium",
#endif
#ifdef FEAT_SOUND
	"+sound",
#નહીંતર
	"-sound",
#endif
#ifdef FEAT_SPELL
	"+spell",
#નહીંતર
	"-spell",
#endif
#ifdef STARTUPTIME
	"+startuptime",
#નહીંતર
	"-startuptime",
#endif
#ifdef FEAT_STL_OPT
	"+statusline",
#નહીંતર
	"-statusline",
#endif
	"-sun_workshop",
#ifdef FEAT_SYN_HL
	"+syntax",
#નહીંતર
	"-syntax",
#endif
	    // only interesting on Unix systems
#જો defined(USE_SYSTEM) && defined(UNIX)
	"+system()",
#endif
#જો defined(FEAT_TABPANEL)
	"+tabpanel",
#નહીંતર
	"-tabpanel",
#endif
	"-tag_any_white",
	"+tag_binary",
	"-tag_old_static",
#ifdef FEAT_TCL
# ifdef DYNAMIC_TCL
	"+tcl/dyn",
# નહીંતર
	"+tcl",
# endif
#નહીંતર
	"-tcl",
#endif
#ifdef FEAT_TERMGUICOLORS
	"+termguicolors",
#નહીંતર
	"-termguicolors",
#endif
#ifdef FEAT_TERMINAL
	"+terminal",
#નહીંતર
	"-terminal",
#endif
#જો defined(UNIX)
// only Unix can have terminfo instead of termcap
# ifdef TERMINFO
	"+terminfo",
# નહીંતર
	"-terminfo",
# endif
#endif
#ifdef FEAT_TERMRESPONSE
	"+termresponse",
#નહીંતર
	"-termresponse",
#endif
	"+textobjects",
#ifdef FEAT_PROP_POPUP
	"+textprop",
#નહીંતર
	"-textprop",
#endif
#જો !defined(UNIX)
// unix always includes termcap support
# ifdef HAVE_TGETENT
	"+tgetent",
# નહીંતર
	"-tgetent",
# endif
#endif
#ifdef FEAT_TIMERS
	"+timers",
#નહીંતર
	"-timers",
#endif
	"+title",
#ifdef FEAT_TOOLBAR
	"+toolbar",
#નહીંતર
	"-toolbar",
#endif
	"+user_commands",
#ifdef FEAT_VARTABS
	"+vartabs",
#નહીંતર
	"-vartabs",
#endif
	"+vertsplit",
	"+vim9script",
#ifdef FEAT_VIMINFO
	"+viminfo",
#નહીંતર
	"-viminfo",
#endif
	"+virtualedit",
	"+visual",
	"+visualextra",
	"+vreplace",
#ifdef MSWIN
# ifdef FEAT_VTP
	"+vtp",
# નહીંતર
	"-vtp",
# endif
#endif
#ifdef FEAT_WAYLAND
	"+wayland",
#નહીંતર
	"-wayland",
#endif
#ifdef FEAT_WAYLAND_CLIPBOARD
	"+wayland_clipboard",
#નહીંતર
	"-wayland_clipboard",
#endif
	"+wildignore",
	"+wildmenu",
	"+windows",
#ifdef FEAT_WRITEBACKUP
	"+writebackup",
#નહીંતર
	"-writebackup",
#endif
#જો defined(UNIX) || defined(VMS)
# ifdef FEAT_X11
	"+X11",
# નહીંતર
	"-X11",
# endif
#endif
#ifdef FEAT_XATTR
	"+xattr",
#નહીંતર
	"-xattr",
#endif
#ifdef FEAT_XFONTSET
	"+xfontset",
#નહીંતર
	"-xfontset",
#endif
#ifdef FEAT_XIM
	"+xim",
#નહીંતર
	"-xim",
#endif
#જો defined(MSWIN)
# ifdef FEAT_XPM_W32
	"+xpm_w32",
# નહીંતર
	"-xpm_w32",
# endif
#અથવાજો defined(HAVE_XPM)
	"+xpm",
#નહીંતર
	"-xpm",
#endif
#જો defined(UNIX) || defined(VMS)
# જો defined(USE_XSMP_INTERACT)
	"+xsmp_interact",
# elif defined(USE_XSMP)
	"+xsmp",
# નહીંતર
	"-xsmp",
# endif
# ifdef FEAT_XCLIPBOARD
	"+xterm_clipboard",
# નહીંતર
	"-xterm_clipboard",
# endif
#endif
#ifdef FEAT_XTERM_SAVE
	"+xterm_save",
#નહીંતર
	"-xterm_save",
#endif
	ખાલી
};

સ્થિર પૂર્ણાંક included_patches[] =
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
સ્થિર અક્ષર *(extra_patches[]) =
{   /* Add your patch description below this line */
/**/
    ખાલી
};

    પૂર્ણાંક
highest_patch(શૂન્ય)
{
    // this relies on the highest patch number to be the first entry
    પાછા included_patches[0];
}

#જો defined(FEAT_EVAL)
/*
 * Return TRUE if patch "n" has been included.
 */
    પૂર્ણાંક
has_patch(પૂર્ણાંક n)
{
    પૂર્ણાંક		h, m, l;

    // Perform a binary search.
    l = 0;
    h = (પૂર્ણાંક)ARRAY_LENGTH(included_patches) - 1;
    માટે (;;)
    {
	m = (l + h) / 2;
	જો (included_patches[m] == n)
	    પાછા TRUE;
	જો (l == h)
	    તોડો;
	જો (included_patches[m] < n)
	    h = m;
	નહીંતર
	    l = m + 1;
    }
    પાછા FALSE;
}
#endif

    શૂન્ય
ex_version(exarg_T *eap)
{
    /*
     * Ignore a ":version 9.99" command.
     */
    જો (*eap->arg == NUL)
    {
	સંદેશ_putchar('\n');
	list_version();
    }
}

/*
 * Output a string for the version message.  If it's going to wrap, output a
 * newline, unless the message is too long to fit on the screen anyway.
 * When "wrap" is TRUE wrap the string in [].
 */
    સ્થિર શૂન્ય
version_સંદેશ_wrap(char_u *s, પૂર્ણાંક wrap)
{
    પૂર્ણાંક		len = vim_strsize(s) + (wrap ? 2 : 0);

    જો (!got_int && len < cmdline_width && સંદેશ_col + len >= cmdline_width
								&& *s != '\n')
	સંદેશ_putchar('\n');
    જો (!got_int)
    {
	જો (wrap)
	    સંદેશ_puts("[");
	સંદેશ_puts((અક્ષર *)s);
	જો (wrap)
	    સંદેશ_puts("]");
    }
}

    સ્થિર શૂન્ય
version_સંદેશ(અક્ષર *s)
{
    version_સંદેશ_wrap((char_u *)s, FALSE);
}

/*
 * List all features aligned in columns, dictionary style.
 */
    સ્થિર શૂન્ય
list_features(શૂન્ય)
{
    list_in_columns((char_u **)features, -1, -1);
}

/*
 * List string items nicely aligned in columns.
 * When "size" is < 0 then the last entry is marked with NULL.
 * The entry with index "current" is inclosed in [].
 */
    શૂન્ય
list_in_columns(char_u **વસ્તુઓ, પૂર્ણાંક size, પૂર્ણાંક current)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		ncol;
    પૂર્ણાંક		nrow;
    પૂર્ણાંક		cur_row = 1;
    પૂર્ણાંક		item_ગણતરી = 0;
    પૂર્ણાંક		width = 0;
#ifdef FEAT_SYN_HL
    પૂર્ણાંક		use_highlight = (વસ્તુઓ == (char_u **)features);
#endif

    // Find the length of the longest item, use that + 1 as the column
    // width.
    માટે (i = 0; size < 0 ? વસ્તુઓ[i] != ખાલી : i < size; ++i)
    {
	પૂર્ણાંક l = vim_strsize(વસ્તુઓ[i]) + (i == current ? 2 : 0);

	જો (l > width)
	    width = l;
	++item_ગણતરી;
    }
    width += 1;

    જો (cmdline_width < width)
    {
	// Not enough screen columns - show one per line
	માટે (i = 0; i < item_ગણતરી; ++i)
	{
	    version_સંદેશ_wrap(વસ્તુઓ[i], i == current);
	    જો (સંદેશ_col > 0 && i < item_ગણતરી - 1)
		સંદેશ_putchar('\n');
	}
	પાછા;
    }

    // The rightmost column doesn't need a separator.
    // Sacrifice it to fit in one more column if possible.
    ncol = (cmdline_width + 1) / width;
    nrow = item_ગણતરી / ncol + ((item_ગણતરી % ncol) ? 1 : 0);

    // "i" counts columns then rows.  "idx" counts rows then columns.
    માટે (i = 0; !got_int && i < nrow * ncol; ++i)
    {
	પૂર્ણાંક idx = (i / ncol) + (i % ncol) * nrow;

	જો (idx < item_ગણતરી)
	{
	    પૂર્ણાંક last_col = (i + 1) % ncol == 0;

	    જો (idx == current)
		સંદેશ_putchar('[');
#ifdef FEAT_SYN_HL
	    જો (use_highlight && વસ્તુઓ[idx][0] == '-')
		સંદેશ_puts_attr((અક્ષર *)વસ્તુઓ[idx], HL_ATTR(HLF_W));
	    નહીંતર
#endif
		સંદેશ_puts((અક્ષર *)વસ્તુઓ[idx]);
	    જો (idx == current)
		સંદેશ_putchar(']');
	    જો (last_col)
	    {
		જો (સંદેશ_col > 0 && cur_row < nrow)
		    સંદેશ_putchar('\n');
		++cur_row;
	    }
	    નહીંતર
	    {
		જ્યારે (સંદેશ_col % width)
		    સંદેશ_putchar(' ');
	    }
	}
	નહીંતર
	{
	    // this row is out of items, thus at the end of the row
	    જો (સંદેશ_col > 0)
	    {
		જો (cur_row < nrow)
		    સંદેશ_putchar('\n');
		++cur_row;
	    }
	}
    }
}

    શૂન્ય
list_version(શૂન્ય)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		first;
    અક્ષર	*s = "";

    /*
     * When adding features here, don't forget to update the list of
     * internal variables in eval.c!
     */
    init_longVersion();
    સંદેશ(longVersion);
#ifdef MSWIN
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
#   ifdef _WIN64
#    જો defined(_M_ARM64) || defined(_M_ARM64EC)
     સંદેશ_puts(_("\nMS-Windows ARM64 GUI/console version"));
#    નહીંતર
     સંદેશ_puts(_("\nMS-Windows 64-bit GUI/console version"));
#    endif
#   નહીંતર
    સંદેશ_puts(_("\nMS-Windows 32-bit GUI/console version"));
#   endif
#  નહીંતર
#   ifdef _WIN64
#    જો defined(_M_ARM64) || defined(_M_ARM64EC)
     સંદેશ_puts(_("\nMS-Windows ARM64 GUI version"));
#    નહીંતર
     સંદેશ_puts(_("\nMS-Windows 64-bit GUI version"));
#    endif
#   નહીંતર
    સંદેશ_puts(_("\nMS-Windows 32-bit GUI version"));
#   endif
#  endif
#  ifdef FEAT_OLE
    સંદેશ_puts(_(" with OLE support"));
#  endif
# નહીંતર
#  ifdef _WIN64
#   જો defined(_M_ARM64) || defined(_M_ARM64EC)
    સંદેશ_puts(_("\nMS-Windows ARM64 console version"));
#   નહીંતર
    સંદેશ_puts(_("\nMS-Windows 64-bit console version"));
#   endif
#  નહીંતર
    સંદેશ_puts(_("\nMS-Windows 32-bit console version"));
#  endif
# endif
#endif
#જો defined(MACOS_X)
# જો defined(MACOS_X_DARWIN)
    સંદેશ_puts(_("\nmacOS version"));
# નહીંતર
    સંદેશ_puts(_("\nmacOS version w/o darwin feat."));
# endif
# જો defined(__arm64__)
    સંદેશ_puts(" - arm64");
# elif defined(__x86_64__)
    સંદેશ_puts(" - x86_64");
# endif
#endif

#ifdef VMS
    સંદેશ_puts(_("\nOpenVMS (build) arch, version"));
# ifdef HAVE_PATHDEF
    જો (*compiled_arch != NUL)
    {
	સંદેશ_puts(": ");
	સંદેશ_puts((અક્ષર *)compiled_arch);
	જો (*compiled_vers != NUL)
	{
	    સંદેશ_puts(", ");
	    સંદેશ_puts((અક્ષર *)compiled_vers);
	}
    }

# endif

#endif

    // Print the list of patch numbers if there is at least one.
    // Print a range when patches are consecutive: "1-10, 12, 15-40, 42-45"
    જો (included_patches[0] != 0)
    {
	સંદેશ_puts(_("\nIncluded patches: "));
	first = -1;
	i = (પૂર્ણાંક)ARRAY_LENGTH(included_patches) - 1;
	જ્યારે (--i >= 0)
	{
	    જો (first < 0)
		first = included_patches[i];
	    જો (i == 0 || included_patches[i - 1] != included_patches[i] + 1)
	    {
		સંદેશ_puts(s);
		s = ", ";
		સંદેશ_outnum((long)first);
		જો (first != included_patches[i])
		{
		    સંદેશ_puts("-");
		    સંદેશ_outnum((long)included_patches[i]);
		}
		first = -1;
	    }
	}
    }

    // Print the list of extra patch descriptions if there is at least one.
    જો (extra_patches[0] != ખાલી)
    {
	સંદેશ_puts(_("\nExtra patches: "));
	s = "";
	માટે (i = 0; extra_patches[i] != ખાલી; ++i)
	{
	    સંદેશ_puts(s);
	    s = ", ";
	    સંદેશ_puts(extra_patches[i]);
	}
    }

#ifdef MODIFIED_BY
    સંદેશ_puts("\n");
    સંદેશ_puts(_("Modified by "));
    સંદેશ_puts(MODIFIED_BY);
#endif

#ifdef HAVE_PATHDEF
    જો (*compiled_user != NUL || *compiled_sys != NUL)
    {
	સંદેશ_puts(_("\nCompiled "));
	જો (*compiled_user != NUL)
	{
	    સંદેશ_puts(_("by "));
	    સંદેશ_puts((અક્ષર *)compiled_user);
	}
	જો (*compiled_sys != NUL)
	{
	    સંદેશ_puts("@");
	    સંદેશ_puts((અક્ષર *)compiled_sys);
	}
    }
#endif

#જો defined(FEAT_HUGE)
    સંદેશ_puts(_("\nHuge version "));
#અથવાજો defined(FEAT_NORMAL)
    સંદેશ_puts(_("\nNormal version "));
#નહીંતર
    સંદેશ_puts(_("\nTiny version "));
#endif
#જો !defined(FEAT_GUI)
    સંદેશ_puts(_("without GUI."));
#અથવાજો defined(FEAT_GUI_GTK)
# જો defined(USE_GTK4)
    સંદેશ_puts(_("with GTK4 GUI."));
# elif defined(USE_GTK3)
    સંદેશ_puts(_("with GTK3 GUI."));
# elif defined(FEAT_GUI_GNOME)
    સંદેશ_puts(_("with GTK2-GNOME GUI."));
# નહીંતર
    સંદેશ_puts(_("with GTK2 GUI."));
# endif
#અથવાજો defined(FEAT_GUI_MOTIF)
    સંદેશ_puts(_("with X11-Motif GUI."));
#અથવાજો defined(FEAT_GUI_HAIKU)
    સંદેશ_puts(_("with Haiku GUI."));
#અથવાજો defined(FEAT_GUI_PHOTON)
    સંદેશ_puts(_("with Photon GUI."));
#અથવાજો defined(MSWIN)
    સંદેશ_puts(_("with GUI."));
#endif
    version_સંદેશ(_("  Features included (+) or not (-):\n"));

    list_features();
    જો (સંદેશ_col > 0)
	સંદેશ_putchar('\n');

#ifdef SYS_VIMRC_FILE
    version_સંદેશ(_("   system vimrc file: \""));
    version_સંદેશ(SYS_VIMRC_FILE);
    version_સંદેશ("\"\n");
#endif
#ifdef USR_VIMRC_FILE
    version_સંદેશ(_("     user vimrc file: \""));
    version_સંદેશ(USR_VIMRC_FILE);
    version_સંદેશ("\"\n");
#endif
#ifdef USR_VIMRC_FILE2
    version_સંદેશ(_(" 2nd user vimrc file: \""));
    version_સંદેશ(USR_VIMRC_FILE2);
    version_સંદેશ("\"\n");
#endif
#જો defined(USR_VIMRC_FILE3) && defined(XDG_VIMRC_FILE)
    version_સંદેશ(_(" 3rd user vimrc file: \""));
    version_સંદેશ(USR_VIMRC_FILE3);
    version_સંદેશ("\"\n");
    version_સંદેશ(_(" 4th user vimrc file: \""));
    version_સંદેશ((અક્ષર *)(XDG_VIMRC_FILE));
    version_સંદેશ("\"\n");
#અથવાજો defined(USR_VIMRC_FILE3)
    version_સંદેશ(_(" 3rd user vimrc file: \""));
    version_સંદેશ(USR_VIMRC_FILE3);
    version_સંદેશ("\"\n");
#અથવાજો defined(XDG_VIMRC_FILE)
    version_સંદેશ(_(" 3rd user vimrc file: \""));
    version_સંદેશ((અક્ષર *)(XDG_VIMRC_FILE));
    version_સંદેશ("\"\n");
#endif
#ifdef USR_EXRC_FILE
    version_સંદેશ(_("      user exrc file: \""));
    version_સંદેશ(USR_EXRC_FILE);
    version_સંદેશ("\"\n");
#endif
#ifdef USR_EXRC_FILE2
    version_સંદેશ(_("  2nd user exrc file: \""));
    version_સંદેશ(USR_EXRC_FILE2);
    version_સંદેશ("\"\n");
#endif
#ifdef FEAT_GUI
# ifdef SYS_GVIMRC_FILE
    version_સંદેશ(_("  system gvimrc file: \""));
    version_સંદેશ(SYS_GVIMRC_FILE);
    version_સંદેશ("\"\n");
# endif
    version_સંદેશ(_("    user gvimrc file: \""));
    version_સંદેશ(USR_GVIMRC_FILE);
    version_સંદેશ("\"\n");
# ifdef USR_GVIMRC_FILE2
    version_સંદેશ(_("2nd user gvimrc file: \""));
    version_સંદેશ(USR_GVIMRC_FILE2);
    version_સંદેશ("\"\n");
# endif
# ifdef USR_GVIMRC_FILE3
    version_સંદેશ(_("3rd user gvimrc file: \""));
    version_સંદેશ(USR_GVIMRC_FILE3);
    version_સંદેશ("\"\n");
# endif
#endif
    version_સંદેશ(_("       defaults file: \""));
    version_સંદેશ(VIM_DEFAULTS_FILE);
    version_સંદેશ("\"\n");
#ifdef FEAT_GUI
# ifdef SYS_MENU_FILE
    version_સંદેશ(_("    system menu file: \""));
    version_સંદેશ(SYS_MENU_FILE);
    version_સંદેશ("\"\n");
# endif
#endif
#ifdef HAVE_PATHDEF
    જો (*default_vim_dir != NUL)
    {
	version_સંદેશ(_("  fall-back for $VIM: \""));
	version_સંદેશ((અક્ષર *)default_vim_dir);
	version_સંદેશ("\"\n");
    }
    જો (*default_vimruntime_dir != NUL)
    {
	version_સંદેશ(_(" f-b for $VIMRUNTIME: \""));
	version_સંદેશ((અક્ષર *)default_vimruntime_dir);
	version_સંદેશ("\"\n");
    }
    version_સંદેશ(_("Compilation: "));
    version_સંદેશ((અક્ષર *)all_cflags);
    version_સંદેશ("\n");
# ifdef VMS
    જો (*compiler_version != NUL)
    {
	version_સંદેશ(_("Compiler: "));
	version_સંદેશ((અક્ષર *)compiler_version);
	version_સંદેશ("\n");
    }
# endif
    version_સંદેશ(_("Linking: "));
    version_સંદેશ((અક્ષર *)all_lflags);
#endif
#ifdef DEBUG
    version_સંદેશ("\n");
    version_સંદેશ(_("  DEBUG BUILD"));
#endif
}

સ્થિર શૂન્ય do_intro_લીટી(પૂર્ણાંક row, char_u *mesg, પૂર્ણાંક add_version, પૂર્ણાંક attr);
સ્થિર શૂન્ય intro_message(પૂર્ણાંક colon);

/*
 * Show the intro message when not editing a file.
 */
    શૂન્ય
maybe_intro_message(શૂન્ય)
{
    જો (BUFEMPTY()
	    && curbuf->b_fname == ખાલી
	    && firstwin->w_next == ખાલી
	    && vim_strchr(p_shm, SHM_INTRO) == ખાલી)
	intro_message(FALSE);
}

/*
 * Give an introductory message about Vim.
 * Only used when starting Vim on an empty file, without a file name.
 * Or with the ":intro" command (for Sven :-).
 */
    સ્થિર શૂન્ય
intro_message(
    પૂર્ણાંક		colon)		// TRUE for ":intro"
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		row;
    પૂર્ણાંક		blanklines;
    પૂર્ણાંક		sponsor;
    અક્ષર	*p;
    સ્થિર અક્ષર	*(લીટીઓ[]) =
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
	ખાલી,
	"",
	N_("Running in Vi compatible mode"),
	N_("type  :set nocp<Enter>        for Vim defaults"),
	N_("type  :help cp-default<Enter> for info on this"),
    };
#ifdef FEAT_GUI
    સ્થિર અક્ષર	*(gui_લીટીઓ[]) =
    {
	ખાલી,
	ખાલી,
	ખાલી,
	ખાલી,
# ifdef MODIFIED_BY
	ખાલી,
# endif
	ખાલી,
	ખાલી,
	ખાલી,
	N_("menu  Help->Orphans           for information    "),
	ખાલી,
	N_("Running modeless, typed text is inserted"),
	N_("menu  Edit->Global Settings->Toggle Insert Mode  "),
	N_("                              for two modes      "),
	ખાલી,
	ખાલી,
	ખાલી,
	N_("menu  Edit->Global Settings->Toggle Vi Compatible"),
	N_("                              for Vim defaults   "),
    };
#endif

    // blanklines = screen height - # message lines
    blanklines = (પૂર્ણાંક)Rows - (ARRAY_LENGTH(લીટીઓ) - 1);
    જો (!p_cp)
	blanklines += 4;  // add 4 for not showing "Vi compatible" message

    // Don't overwrite a statusline.  Depends on 'cmdheight'.
    જો (p_ls > 1)
	blanklines -= Rows - topframe->fr_height;
    જો (blanklines < 0)
	blanklines = 0;

    // Show the sponsor and Uganda message two out of four times
    sponsor = (પૂર્ણાંક)time(ખાલી);
    sponsor = ((sponsor & 2) == 0) - ((sponsor & 4) == 0);

    // start displaying the message lines after half of the blank lines
    row = blanklines / 2;
    જો ((row >= 2 && topframe->fr_width >= 50) || colon)
    {
	માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(લીટીઓ); ++i)
	{
	    p = લીટીઓ[i];
#ifdef FEAT_GUI
	    જો (p_im && gui.in_use && gui_લીટીઓ[i] != ખાલી)
		p = gui_લીટીઓ[i];
#endif
	    જો (p == ખાલી)
	    {
		જો (!p_cp)
		    તોડો;
		ચાલુ;
	    }
	    જો (sponsor != 0)
	    {
		જો (strstr(p, "children") != ખાલી)
		    p = N_("Sponsor Vim development!");
		નહીંતર જો (strstr(p, "Kuwasha") != ખાલી)
		    p = N_("type  :help sponsor<Enter>    for information ");
		નહીંતર જો (strstr(p, "Orphans") != ખાલી)
		    p = N_("menu  Help->Sponsor           for information    ");
	    }
	    જો (*p != NUL)
		do_intro_લીટી(row, (char_u *)_(p), i == 2, 0);
	    ++row;
	}
    }

    // Make the wait-return message appear just below the text.
    જો (colon)
	સંદેશ_row = row;
}

    સ્થિર શૂન્ય
do_intro_લીટી(
    પૂર્ણાંક		row,
    char_u	*mesg,
    પૂર્ણાંક		add_version,
    પૂર્ણાંક		attr)
{
    char_u	vers[20];
    પૂર્ણાંક		col;
    char_u	*p;
    પૂર્ણાંક		l;
    પૂર્ણાંક		clen;
#ifdef MODIFIED_BY
# define MODBY_LEN 150
    char_u	modby[MODBY_LEN];

    જો (*mesg == ' ')
    {
	vim_strncpy(modby, (char_u *)_("Modified by "), MODBY_LEN - 1);
	l = (પૂર્ણાંક)STRLEN(modby);
	vim_strncpy(modby + l, (char_u *)MODIFIED_BY, MODBY_LEN - l - 1);
	mesg = modby;
    }
#endif

    // Center the message horizontally.
    col = vim_strsize(mesg);
    જો (add_version)
    {
	STRCPY(vers, mediumVersion);
	જો (highest_patch())
	{
	    // Check for 9.9x or 9.9xx, alpha/beta version
	    જો (SAFE_isalpha((પૂર્ણાંક)vers[3]))
	    {
		પૂર્ણાંક len = (SAFE_isalpha((પૂર્ણાંક)vers[4])) ? 5 : 4;
		sprintf((અક્ષર *)vers + len, ".%d%s", highest_patch(),
							 mediumVersion + len);
	    }
	    નહીંતર
		sprintf((અક્ષર *)vers + 3, ".%d", highest_patch());
	}
	col += (પૂર્ણાંક)STRLEN(vers);
    }
    col = (topframe->fr_width - col) / 2;
    જો (col < 0)
	col = 0;

    // Split up in parts to highlight <> items differently.
    માટે (p = mesg; *p != NUL; p += l)
    {
	clen = 0;
	માટે (l = 0; p[l] != NUL
			 && (l == 0 || (p[l] != '<' && p[l - 1] != '>')); ++l)
	{
	    જો (has_mbyte)
	    {
		clen += ptr2cells(p + l);
		l += (*mb_ptr2len)(p + l) - 1;
	    }
	    નહીંતર
		clen += byte2cells(p[l]);
	}
	screen_puts_len(p, l, row, col + firstwin->w_wincol,
		*p == '<' ? HL_ATTR(HLF_8) : attr);
	col += clen;
    }

    // Add the version number to the version line.
    જો (add_version)
	screen_puts(vers, row, col + firstwin->w_wincol, 0);
}

/*
 * ":intro": clear screen, display intro screen and wait for return.
 */
    શૂન્ય
ex_intro(exarg_T *eap UNUSED)
{
    screenclear();
#જો defined(FEAT_TABPANEL)
    draw_tabpanel();
#endif
    intro_message(TRUE);
    wait_return(TRUE);
}
