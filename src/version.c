/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#அந்தர்பாவ "vim.h"

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

#அந்தர்பாவ "version.h"

எழுத்து		*Version = VIM_VERSION_SHORT;
நிலையான எழுத்து	*mediumVersion = VIM_VERSION_MEDIUM;

#எனில் defined(HAVE_DATE_TIME)
# எனில் defined(VMS) && defined(VAXC)
எழுத்து	longVersion[அளவு(VIM_VERSION_LONG_DATE) + அளவு(__DATE__)
						      + அளவு(__TIME__) + 3];

    வெறுமை
init_longVersion(வெறுமை)
{
    /*
     * Construct the long version string.  Necessary because
     * VAX C can't concatenate strings in the preprocessor.
     */
    strcpy(longVersion, VIM_VERSION_LONG_DATE);
#  ifdef BUILD_DATE
    strcat(longVersion, BUILD_DATE);
#  இல்லையெனில்
    strcat(longVersion, __DATE__);
    strcat(longVersion, " ");
    strcat(longVersion, __TIME__);
#  endif
    strcat(longVersion, ")");
}

# இல்லையெனில்
எழுத்து	*longVersion = வெற்று;

    வெறுமை
init_longVersion(வெறுமை)
{
    எனில் (longVersion != வெற்று)
	திரும்பு;

#  ifdef BUILD_DATE
    எழுத்து *date_time = BUILD_DATE;
#  இல்லையெனில்
    எழுத்து *date_time = __DATE__ " " __TIME__;
#  endif
    எழுத்து *செய்தி = _("%s (%s, compiled %s)");
    size_t len = strlen(செய்தி)
	+ STRLEN_LITERAL(VIM_VERSION_LONG_ONLY)
	+ STRLEN_LITERAL(VIM_VERSION_DATE_ONLY)
	+ strlen(date_time);

    longVersion = alloc(len);
    எனில் (longVersion == வெற்று)
	longVersion = VIM_VERSION_LONG;
    இல்லையெனில்
	vim_snprintf(longVersion, len, செய்தி,
		VIM_VERSION_LONG_ONLY, VIM_VERSION_DATE_ONLY, date_time);
}
# endif
#இல்லையெனில்
எழுத்து	*longVersion = VIM_VERSION_LONG;

    வெறுமை
init_longVersion(வெறுமை)
{
    // nothing to do
}
#endif

நிலையான எழுத்து *(features[]) =
{
#ifdef HAVE_ACL
	"+acl",
#இல்லையெனில்
	"-acl",
#endif
#ifdef FEAT_ARABIC
	"+arabic",
#இல்லையெனில்
	"-arabic",
#endif
#ifdef AMIGA		// only for Amiga systems
# ifdef FEAT_ARP
	"+ARP",
# இல்லையெனில்
	"-ARP",
# endif
#endif
#ifdef FEAT_AUTOCHDIR
	"+autochdir",
#இல்லையெனில்
	"-autochdir",
#endif
	"+autocmd",
#ifdef FEAT_AUTOSERVERNAME
	"+autoservername",
#இல்லையெனில்
	"-autoservername",
#endif
#ifdef FEAT_BEVAL_GUI
	"+balloon_eval",
#இல்லையெனில்
	"-balloon_eval",
#endif
#ifdef FEAT_BEVAL_TERM
	"+balloon_eval_term",
#இல்லையெனில்
	"-balloon_eval_term",
#endif
#ifdef FEAT_BROWSE
	"+browse",
#இல்லையெனில்
	"-browse",
#endif
	"++builtin_terms",
#ifdef FEAT_BYTEOFF
	"+byte_offset",
#இல்லையெனில்
	"-byte_offset",
#endif
#ifdef FEAT_JOB_CHANNEL
	"+channel",
#இல்லையெனில்
	"-channel",
#endif
	"+cindent",
#ifdef FEAT_CLIENTSERVER
	"+clientserver",
#இல்லையெனில்
	"-clientserver",
#endif
#ifdef FEAT_CLIPBOARD
	"+clipboard",
#இல்லையெனில்
	"-clipboard",
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	"+clipboard_provider",
#இல்லையெனில்
	"-clipboard_provider",
#endif
	"+cmdline_compl",
	"+cmdline_hist",
	"+cmdline_info",
	"+comments",
#ifdef FEAT_CONCEAL
	"+conceal",
#இல்லையெனில்
	"-conceal",
#endif
#ifdef FEAT_CRYPT
	"+cryptv",
#இல்லையெனில்
	"-cryptv",
#endif
#ifdef FEAT_CSCOPE
	"+cscope",
#இல்லையெனில்
	"-cscope",
#endif
	"+cursorbind",
#ifdef CURSOR_SHAPE
	"+cursorshape",
#இல்லையெனில்
	"-cursorshape",
#endif
#எனில் defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
	"+dialog_con_gui",
#இல்லையெனில்
# எனில் defined(FEAT_CON_DIALOG)
	"+dialog_con",
# இல்லையெனில்
#  எனில் defined(FEAT_GUI_DIALOG)
	"+dialog_gui",
#  இல்லையெனில்
	"-dialog",
#  endif
# endif
#endif
#ifdef FEAT_DIFF
	"+diff",
#இல்லையெனில்
	"-diff",
#endif
#ifdef FEAT_DIGRAPHS
	"+digraphs",
#இல்லையெனில்
	"-digraphs",
#endif
#ifdef FEAT_GUI_MSWIN
# ifdef FEAT_DIRECTX
	"+directx",
# இல்லையெனில்
	"-directx",
# endif
#endif
#ifdef FEAT_DND
	"+dnd",
#இல்லையெனில்
	"-dnd",
#endif
	"-ebcdic",
#ifdef FEAT_EMACS_TAGS
	"+emacs_tags",
#இல்லையெனில்
	"-emacs_tags",
#endif
#ifdef FEAT_EVAL
	"+eval",
#இல்லையெனில்
	"-eval",
#endif
	"+ex_extra",
#ifdef FEAT_தேடல்_EXTRA
	"+extra_search",
#இல்லையெனில்
	"-extra_search",
#endif
	"-farsi",
	"+file_in_path",
#ifdef FEAT_FIND_ID
	"+find_in_path",
#இல்லையெனில்
	"-find_in_path",
#endif
	"+float",
#ifdef FEAT_FOLDING
	"+folding",
#இல்லையெனில்
	"-folding",
#endif
	"-footer",
	// only interesting on Unix systems
#எனில் !defined(USE_SYSTEM) && defined(UNIX)
	"+fork()",
#endif
#ifdef FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
	"+gettext/dyn",
# இல்லையெனில்
	"+gettext",
# endif
#இல்லையெனில்
	"-gettext",
#endif
	"-hangul_input",
#எனில் (defined(HAVE_ICONV_H) && defined(USE_ICONV)) || defined(DYNAMIC_ICONV)
# ifdef DYNAMIC_ICONV
	"+iconv/dyn",
# இல்லையெனில்
	"+iconv",
# endif
#இல்லையெனில்
	"-iconv",
#endif
#ifdef FEAT_IMAGE
	"+image",
#இல்லையெனில்
	"-image",
#endif
#ifdef FEAT_IMAGE_POPUP
	"+image_popup",
#இல்லையெனில்
	"-image_popup",
#endif
	"+insert_expand",
#ifdef FEAT_IPV6
	"+ipv6",
#இல்லையெனில்
	"-ipv6",
#endif
#ifdef FEAT_JOB_CHANNEL
	"+job",
#இல்லையெனில்
	"-job",
#endif
	"+jumplist",
#ifdef FEAT_KEYMAP
	"+keymap",
#இல்லையெனில்
	"-keymap",
#endif
#ifdef FEAT_EVAL
	"+lambda",
#இல்லையெனில்
	"-lambda",
#endif
#ifdef FEAT_LANGMAP
	"+langmap",
#இல்லையெனில்
	"-langmap",
#endif
#ifdef FEAT_LIBCALL
	"+libcall",
#இல்லையெனில்
	"-libcall",
#endif
#ifdef FEAT_LINEBREAK
	"+linebreak",
#இல்லையெனில்
	"-linebreak",
#endif
	"+lispindent",
	"+listcmds",
	"+localmap",
#ifdef FEAT_LUA
# ifdef DYNAMIC_LUA
	"+lua/dyn",
# இல்லையெனில்
	"+lua",
# endif
#இல்லையெனில்
	"-lua",
#endif
#ifdef FEAT_MENU
	"+menu",
#இல்லையெனில்
	"-menu",
#endif
#ifdef FEAT_SESSION
	"+mksession",
#இல்லையெனில்
	"-mksession",
#endif
	"+modify_fname",
	"+mouse",
#எனில் defined(UNIX) || defined(VMS)
# ifdef FEAT_MOUSE_DEC
	"+mouse_dec",
# இல்லையெனில்
	"-mouse_dec",
# endif
# ifdef FEAT_MOUSE_GPM
#  ifdef DYNAMIC_GPM
	"+mouse_gpm/dyn",
#  இல்லையெனில்
	"+mouse_gpm",
#  endif
# இல்லையெனில்
	"-mouse_gpm",
# endif
# ifdef FEAT_MOUSE_JSB
	"+mouse_jsbterm",
# இல்லையெனில்
	"-mouse_jsbterm",
# endif
# ifdef FEAT_MOUSE_NET
	"+mouse_netterm",
# இல்லையெனில்
	"-mouse_netterm",
# endif
#endif

#ifdef __QNX__
# ifdef FEAT_MOUSE_PTERM
	"+mouse_pterm",
# இல்லையெனில்
	"-mouse_pterm",
# endif
#endif

#எனில் defined(UNIX) || defined(VMS)
	"+mouse_sgr",
# ifdef FEAT_SYSMOUSE
	"+mouse_sysmouse",
# இல்லையெனில்
	"-mouse_sysmouse",
# endif
# ifdef FEAT_MOUSE_URXVT
	"+mouse_urxvt",
# இல்லையெனில்
	"-mouse_urxvt",
# endif
	"+mouse_xterm",
#endif
#ifdef FEAT_MOUSESHAPE
	"+mouseshape",
#இல்லையெனில்
	"-mouseshape",
#endif

#ifdef FEAT_MBYTE_IME
# ifdef DYNAMIC_IME
	"+multi_byte_ime/dyn",
# இல்லையெனில்
	"+multi_byte_ime",
# endif
#இல்லையெனில்
	"+multi_byte",
#endif
#ifdef FEAT_MULTI_LANG
	"+multi_lang",
#இல்லையெனில்
	"-multi_lang",
#endif
#ifdef FEAT_MZSCHEME
# ifdef DYNAMIC_MZSCHEME
	"+mzscheme/dyn",
# இல்லையெனில்
	"+mzscheme",
# endif
#இல்லையெனில்
	"-mzscheme",
#endif
#ifdef FEAT_NETBEANS_INTG
	"+netbeans_intg",
#இல்லையெனில்
	"-netbeans_intg",
#endif
	"+num64",
#ifdef FEAT_GUI_MSWIN
# ifdef FEAT_OLE
	"+ole",
# இல்லையெனில்
	"-ole",
# endif
#endif
#ifdef FEAT_EVAL
	"+packages",
#இல்லையெனில்
	"-packages",
#endif
#ifdef FEAT_PRINT_PANGO
	"+pango",
#இல்லையெனில்
	"-pango",
#endif
	"+path_extra",
#ifdef FEAT_PERL
# ifdef DYNAMIC_PERL
	"+perl/dyn",
# இல்லையெனில்
	"+perl",
# endif
#இல்லையெனில்
	"-perl",
#endif
#ifdef FEAT_PERSISTENT_UNDO
	"+persistent_undo",
#இல்லையெனில்
	"-persistent_undo",
#endif
#ifdef FEAT_PROP_POPUP
	"+popupwin",
#இல்லையெனில்
	"-popupwin",
#endif
#ifdef FEAT_PRINTER
# ifdef FEAT_POSTSCRIPT
	"+postscript",
# இல்லையெனில்
	"-postscript",
# endif
	"+printer",
#இல்லையெனில்
	"-printer",
#endif
#ifdef FEAT_PROFILE
	"+profile",
#இல்லையெனில்
	"-profile",
#endif
#ifdef FEAT_PYTHON
# ifdef DYNAMIC_PYTHON
	"+python/dyn",
# இல்லையெனில்
	"+python",
# endif
#இல்லையெனில்
	"-python",
#endif
#ifdef FEAT_PYTHON3
# ifdef DYNAMIC_PYTHON3
#  ifdef DYNAMIC_PYTHON3_STABLE_ABI
	"+python3/dyn-stable",
#  இல்லையெனில்
	"+python3/dyn",
#  endif
# இல்லையெனில்
	"+python3",
# endif
#இல்லையெனில்
	"-python3",
#endif
#ifdef FEAT_QUICKFIX
	"+quickfix",
#இல்லையெனில்
	"-quickfix",
#endif
#ifdef FEAT_RELTIME
	"+reltime",
#இல்லையெனில்
	"-reltime",
#endif
#ifdef FEAT_RIGHTLEFT
	"+rightleft",
#இல்லையெனில்
	"-rightleft",
#endif
#ifdef FEAT_RUBY
# ifdef DYNAMIC_RUBY
	"+ruby/dyn",
# இல்லையெனில்
	"+ruby",
# endif
#இல்லையெனில்
	"-ruby",
#endif
	"+scrollbind",
#ifdef FEAT_SIGNS
	"+signs",
#இல்லையெனில்
	"-signs",
#endif
	"+smartindent",
#ifdef FEAT_SOCKETSERVER
	"+socketserver",
#இல்லையெனில்
	"-socketserver",
#endif
#ifdef FEAT_SODIUM
# ifdef DYNAMIC_SODIUM
	"+sodium/dyn",
# இல்லையெனில்
	"+sodium",
# endif
#இல்லையெனில்
	"-sodium",
#endif
#ifdef FEAT_SOUND
	"+sound",
#இல்லையெனில்
	"-sound",
#endif
#ifdef FEAT_SPELL
	"+spell",
#இல்லையெனில்
	"-spell",
#endif
#ifdef STARTUPTIME
	"+startuptime",
#இல்லையெனில்
	"-startuptime",
#endif
#ifdef FEAT_STL_OPT
	"+statusline",
#இல்லையெனில்
	"-statusline",
#endif
	"-sun_workshop",
#ifdef FEAT_SYN_HL
	"+syntax",
#இல்லையெனில்
	"-syntax",
#endif
	    // only interesting on Unix systems
#எனில் defined(USE_SYSTEM) && defined(UNIX)
	"+system()",
#endif
#எனில் defined(FEAT_TABPANEL)
	"+tabpanel",
#இல்லையெனில்
	"-tabpanel",
#endif
	"-tag_any_white",
	"+tag_binary",
	"-tag_old_static",
#ifdef FEAT_TCL
# ifdef DYNAMIC_TCL
	"+tcl/dyn",
# இல்லையெனில்
	"+tcl",
# endif
#இல்லையெனில்
	"-tcl",
#endif
#ifdef FEAT_TERMGUICOLORS
	"+termguicolors",
#இல்லையெனில்
	"-termguicolors",
#endif
#ifdef FEAT_TERMINAL
	"+terminal",
#இல்லையெனில்
	"-terminal",
#endif
#எனில் defined(UNIX)
// only Unix can have terminfo instead of termcap
# ifdef TERMINFO
	"+terminfo",
# இல்லையெனில்
	"-terminfo",
# endif
#endif
#ifdef FEAT_TERMRESPONSE
	"+termresponse",
#இல்லையெனில்
	"-termresponse",
#endif
	"+textobjects",
#ifdef FEAT_PROP_POPUP
	"+textprop",
#இல்லையெனில்
	"-textprop",
#endif
#எனில் !defined(UNIX)
// unix always includes termcap support
# ifdef HAVE_TGETENT
	"+tgetent",
# இல்லையெனில்
	"-tgetent",
# endif
#endif
#ifdef FEAT_TIMERS
	"+timers",
#இல்லையெனில்
	"-timers",
#endif
	"+title",
#ifdef FEAT_TOOLBAR
	"+toolbar",
#இல்லையெனில்
	"-toolbar",
#endif
	"+user_commands",
#ifdef FEAT_VARTABS
	"+vartabs",
#இல்லையெனில்
	"-vartabs",
#endif
	"+vertsplit",
	"+vim9script",
#ifdef FEAT_VIMINFO
	"+viminfo",
#இல்லையெனில்
	"-viminfo",
#endif
	"+virtualedit",
	"+visual",
	"+visualextra",
	"+vreplace",
#ifdef MSWIN
# ifdef FEAT_VTP
	"+vtp",
# இல்லையெனில்
	"-vtp",
# endif
#endif
#ifdef FEAT_WAYLAND
	"+wayland",
#இல்லையெனில்
	"-wayland",
#endif
#ifdef FEAT_WAYLAND_CLIPBOARD
	"+wayland_clipboard",
#இல்லையெனில்
	"-wayland_clipboard",
#endif
	"+wildignore",
	"+wildmenu",
	"+windows",
#ifdef FEAT_WRITEBACKUP
	"+writebackup",
#இல்லையெனில்
	"-writebackup",
#endif
#எனில் defined(UNIX) || defined(VMS)
# ifdef FEAT_X11
	"+X11",
# இல்லையெனில்
	"-X11",
# endif
#endif
#ifdef FEAT_XATTR
	"+xattr",
#இல்லையெனில்
	"-xattr",
#endif
#ifdef FEAT_XFONTSET
	"+xfontset",
#இல்லையெனில்
	"-xfontset",
#endif
#ifdef FEAT_XIM
	"+xim",
#இல்லையெனில்
	"-xim",
#endif
#எனில் defined(MSWIN)
# ifdef FEAT_XPM_W32
	"+xpm_w32",
# இல்லையெனில்
	"-xpm_w32",
# endif
#அல்லதெனில் defined(HAVE_XPM)
	"+xpm",
#இல்லையெனில்
	"-xpm",
#endif
#எனில் defined(UNIX) || defined(VMS)
# எனில் defined(USE_XSMP_INTERACT)
	"+xsmp_interact",
# elif defined(USE_XSMP)
	"+xsmp",
# இல்லையெனில்
	"-xsmp",
# endif
# ifdef FEAT_XCLIPBOARD
	"+xterm_clipboard",
# இல்லையெனில்
	"-xterm_clipboard",
# endif
#endif
#ifdef FEAT_XTERM_SAVE
	"+xterm_save",
#இல்லையெனில்
	"-xterm_save",
#endif
	வெற்று
};

நிலையான முழு included_patches[] =
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
நிலையான எழுத்து *(extra_patches[]) =
{   /* Add your patch description below this line */
/**/
    வெற்று
};

    முழு
highest_patch(வெறுமை)
{
    // this relies on the highest patch number to be the first entry
    திரும்பு included_patches[0];
}

#எனில் defined(FEAT_EVAL)
/*
 * Return TRUE if patch "n" has been included.
 */
    முழு
has_patch(முழு n)
{
    முழு		h, m, l;

    // Perform a binary search.
    l = 0;
    h = (முழு)ARRAY_LENGTH(included_patches) - 1;
    க்கு (;;)
    {
	m = (l + h) / 2;
	எனில் (included_patches[m] == n)
	    திரும்பு TRUE;
	எனில் (l == h)
	    நிறுத்து;
	எனில் (included_patches[m] < n)
	    h = m;
	இல்லையெனில்
	    l = m + 1;
    }
    திரும்பு FALSE;
}
#endif

    வெறுமை
ex_version(exarg_T *eap)
{
    /*
     * Ignore a ":version 9.99" command.
     */
    எனில் (*eap->arg == NUL)
    {
	செய்தி_putchar('\n');
	list_version();
    }
}

/*
 * Output a string for the version message.  If it's going to wrap, output a
 * newline, unless the message is too long to fit on the screen anyway.
 * When "wrap" is TRUE wrap the string in [].
 */
    நிலையான வெறுமை
version_செய்தி_wrap(char_u *s, முழு wrap)
{
    முழு		len = vim_strsize(s) + (wrap ? 2 : 0);

    எனில் (!got_int && len < cmdline_width && செய்தி_col + len >= cmdline_width
								&& *s != '\n')
	செய்தி_putchar('\n');
    எனில் (!got_int)
    {
	எனில் (wrap)
	    செய்தி_puts("[");
	செய்தி_puts((எழுத்து *)s);
	எனில் (wrap)
	    செய்தி_puts("]");
    }
}

    நிலையான வெறுமை
version_செய்தி(எழுத்து *s)
{
    version_செய்தி_wrap((char_u *)s, FALSE);
}

/*
 * List all features aligned in columns, dictionary style.
 */
    நிலையான வெறுமை
list_features(வெறுமை)
{
    list_in_columns((char_u **)features, -1, -1);
}

/*
 * List string items nicely aligned in columns.
 * When "size" is < 0 then the last entry is marked with NULL.
 * The entry with index "current" is inclosed in [].
 */
    வெறுமை
list_in_columns(char_u **பொருட்கள், முழு size, முழு current)
{
    முழு		i;
    முழு		ncol;
    முழு		nrow;
    முழு		cur_row = 1;
    முழு		item_மொத்தம் = 0;
    முழு		width = 0;
#ifdef FEAT_SYN_HL
    முழு		use_highlight = (பொருட்கள் == (char_u **)features);
#endif

    // Find the length of the longest item, use that + 1 as the column
    // width.
    க்கு (i = 0; size < 0 ? பொருட்கள்[i] != வெற்று : i < size; ++i)
    {
	முழு l = vim_strsize(பொருட்கள்[i]) + (i == current ? 2 : 0);

	எனில் (l > width)
	    width = l;
	++item_மொத்தம்;
    }
    width += 1;

    எனில் (cmdline_width < width)
    {
	// Not enough screen columns - show one per line
	க்கு (i = 0; i < item_மொத்தம்; ++i)
	{
	    version_செய்தி_wrap(பொருட்கள்[i], i == current);
	    எனில் (செய்தி_col > 0 && i < item_மொத்தம் - 1)
		செய்தி_putchar('\n');
	}
	திரும்பு;
    }

    // The rightmost column doesn't need a separator.
    // Sacrifice it to fit in one more column if possible.
    ncol = (cmdline_width + 1) / width;
    nrow = item_மொத்தம் / ncol + ((item_மொத்தம் % ncol) ? 1 : 0);

    // "i" counts columns then rows.  "idx" counts rows then columns.
    க்கு (i = 0; !got_int && i < nrow * ncol; ++i)
    {
	முழு idx = (i / ncol) + (i % ncol) * nrow;

	எனில் (idx < item_மொத்தம்)
	{
	    முழு last_col = (i + 1) % ncol == 0;

	    எனில் (idx == current)
		செய்தி_putchar('[');
#ifdef FEAT_SYN_HL
	    எனில் (use_highlight && பொருட்கள்[idx][0] == '-')
		செய்தி_puts_attr((எழுத்து *)பொருட்கள்[idx], HL_ATTR(HLF_W));
	    இல்லையெனில்
#endif
		செய்தி_puts((எழுத்து *)பொருட்கள்[idx]);
	    எனில் (idx == current)
		செய்தி_putchar(']');
	    எனில் (last_col)
	    {
		எனில் (செய்தி_col > 0 && cur_row < nrow)
		    செய்தி_putchar('\n');
		++cur_row;
	    }
	    இல்லையெனில்
	    {
		போது (செய்தி_col % width)
		    செய்தி_putchar(' ');
	    }
	}
	இல்லையெனில்
	{
	    // this row is out of items, thus at the end of the row
	    எனில் (செய்தி_col > 0)
	    {
		எனில் (cur_row < nrow)
		    செய்தி_putchar('\n');
		++cur_row;
	    }
	}
    }
}

    வெறுமை
list_version(வெறுமை)
{
    முழு		i;
    முழு		first;
    எழுத்து	*s = "";

    /*
     * When adding features here, don't forget to update the list of
     * internal variables in eval.c!
     */
    init_longVersion();
    செய்தி(longVersion);
#ifdef MSWIN
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
#   ifdef _WIN64
#    எனில் defined(_M_ARM64) || defined(_M_ARM64EC)
     செய்தி_puts(_("\nMS-Windows ARM64 GUI/console version"));
#    இல்லையெனில்
     செய்தி_puts(_("\nMS-Windows 64-bit GUI/console version"));
#    endif
#   இல்லையெனில்
    செய்தி_puts(_("\nMS-Windows 32-bit GUI/console version"));
#   endif
#  இல்லையெனில்
#   ifdef _WIN64
#    எனில் defined(_M_ARM64) || defined(_M_ARM64EC)
     செய்தி_puts(_("\nMS-Windows ARM64 GUI version"));
#    இல்லையெனில்
     செய்தி_puts(_("\nMS-Windows 64-bit GUI version"));
#    endif
#   இல்லையெனில்
    செய்தி_puts(_("\nMS-Windows 32-bit GUI version"));
#   endif
#  endif
#  ifdef FEAT_OLE
    செய்தி_puts(_(" with OLE support"));
#  endif
# இல்லையெனில்
#  ifdef _WIN64
#   எனில் defined(_M_ARM64) || defined(_M_ARM64EC)
    செய்தி_puts(_("\nMS-Windows ARM64 console version"));
#   இல்லையெனில்
    செய்தி_puts(_("\nMS-Windows 64-bit console version"));
#   endif
#  இல்லையெனில்
    செய்தி_puts(_("\nMS-Windows 32-bit console version"));
#  endif
# endif
#endif
#எனில் defined(MACOS_X)
# எனில் defined(MACOS_X_DARWIN)
    செய்தி_puts(_("\nmacOS version"));
# இல்லையெனில்
    செய்தி_puts(_("\nmacOS version w/o darwin feat."));
# endif
# எனில் defined(__arm64__)
    செய்தி_puts(" - arm64");
# elif defined(__x86_64__)
    செய்தி_puts(" - x86_64");
# endif
#endif

#ifdef VMS
    செய்தி_puts(_("\nOpenVMS (build) arch, version"));
# ifdef HAVE_PATHDEF
    எனில் (*compiled_arch != NUL)
    {
	செய்தி_puts(": ");
	செய்தி_puts((எழுத்து *)compiled_arch);
	எனில் (*compiled_vers != NUL)
	{
	    செய்தி_puts(", ");
	    செய்தி_puts((எழுத்து *)compiled_vers);
	}
    }

# endif

#endif

    // Print the list of patch numbers if there is at least one.
    // Print a range when patches are consecutive: "1-10, 12, 15-40, 42-45"
    எனில் (included_patches[0] != 0)
    {
	செய்தி_puts(_("\nIncluded patches: "));
	first = -1;
	i = (முழு)ARRAY_LENGTH(included_patches) - 1;
	போது (--i >= 0)
	{
	    எனில் (first < 0)
		first = included_patches[i];
	    எனில் (i == 0 || included_patches[i - 1] != included_patches[i] + 1)
	    {
		செய்தி_puts(s);
		s = ", ";
		செய்தி_outnum((long)first);
		எனில் (first != included_patches[i])
		{
		    செய்தி_puts("-");
		    செய்தி_outnum((long)included_patches[i]);
		}
		first = -1;
	    }
	}
    }

    // Print the list of extra patch descriptions if there is at least one.
    எனில் (extra_patches[0] != வெற்று)
    {
	செய்தி_puts(_("\nExtra patches: "));
	s = "";
	க்கு (i = 0; extra_patches[i] != வெற்று; ++i)
	{
	    செய்தி_puts(s);
	    s = ", ";
	    செய்தி_puts(extra_patches[i]);
	}
    }

#ifdef MODIFIED_BY
    செய்தி_puts("\n");
    செய்தி_puts(_("Modified by "));
    செய்தி_puts(MODIFIED_BY);
#endif

#ifdef HAVE_PATHDEF
    எனில் (*compiled_user != NUL || *compiled_sys != NUL)
    {
	செய்தி_puts(_("\nCompiled "));
	எனில் (*compiled_user != NUL)
	{
	    செய்தி_puts(_("by "));
	    செய்தி_puts((எழுத்து *)compiled_user);
	}
	எனில் (*compiled_sys != NUL)
	{
	    செய்தி_puts("@");
	    செய்தி_puts((எழுத்து *)compiled_sys);
	}
    }
#endif

#எனில் defined(FEAT_HUGE)
    செய்தி_puts(_("\nHuge version "));
#அல்லதெனில் defined(FEAT_NORMAL)
    செய்தி_puts(_("\nNormal version "));
#இல்லையெனில்
    செய்தி_puts(_("\nTiny version "));
#endif
#எனில் !defined(FEAT_GUI)
    செய்தி_puts(_("without GUI."));
#அல்லதெனில் defined(FEAT_GUI_GTK)
# எனில் defined(USE_GTK4)
    செய்தி_puts(_("with GTK4 GUI."));
# elif defined(USE_GTK3)
    செய்தி_puts(_("with GTK3 GUI."));
# elif defined(FEAT_GUI_GNOME)
    செய்தி_puts(_("with GTK2-GNOME GUI."));
# இல்லையெனில்
    செய்தி_puts(_("with GTK2 GUI."));
# endif
#அல்லதெனில் defined(FEAT_GUI_MOTIF)
    செய்தி_puts(_("with X11-Motif GUI."));
#அல்லதெனில் defined(FEAT_GUI_HAIKU)
    செய்தி_puts(_("with Haiku GUI."));
#அல்லதெனில் defined(FEAT_GUI_PHOTON)
    செய்தி_puts(_("with Photon GUI."));
#அல்லதெனில் defined(MSWIN)
    செய்தி_puts(_("with GUI."));
#endif
    version_செய்தி(_("  Features included (+) or not (-):\n"));

    list_features();
    எனில் (செய்தி_col > 0)
	செய்தி_putchar('\n');

#ifdef SYS_VIMRC_FILE
    version_செய்தி(_("   system vimrc file: \""));
    version_செய்தி(SYS_VIMRC_FILE);
    version_செய்தி("\"\n");
#endif
#ifdef USR_VIMRC_FILE
    version_செய்தி(_("     user vimrc file: \""));
    version_செய்தி(USR_VIMRC_FILE);
    version_செய்தி("\"\n");
#endif
#ifdef USR_VIMRC_FILE2
    version_செய்தி(_(" 2nd user vimrc file: \""));
    version_செய்தி(USR_VIMRC_FILE2);
    version_செய்தி("\"\n");
#endif
#எனில் defined(USR_VIMRC_FILE3) && defined(XDG_VIMRC_FILE)
    version_செய்தி(_(" 3rd user vimrc file: \""));
    version_செய்தி(USR_VIMRC_FILE3);
    version_செய்தி("\"\n");
    version_செய்தி(_(" 4th user vimrc file: \""));
    version_செய்தி((எழுத்து *)(XDG_VIMRC_FILE));
    version_செய்தி("\"\n");
#அல்லதெனில் defined(USR_VIMRC_FILE3)
    version_செய்தி(_(" 3rd user vimrc file: \""));
    version_செய்தி(USR_VIMRC_FILE3);
    version_செய்தி("\"\n");
#அல்லதெனில் defined(XDG_VIMRC_FILE)
    version_செய்தி(_(" 3rd user vimrc file: \""));
    version_செய்தி((எழுத்து *)(XDG_VIMRC_FILE));
    version_செய்தி("\"\n");
#endif
#ifdef USR_EXRC_FILE
    version_செய்தி(_("      user exrc file: \""));
    version_செய்தி(USR_EXRC_FILE);
    version_செய்தி("\"\n");
#endif
#ifdef USR_EXRC_FILE2
    version_செய்தி(_("  2nd user exrc file: \""));
    version_செய்தி(USR_EXRC_FILE2);
    version_செய்தி("\"\n");
#endif
#ifdef FEAT_GUI
# ifdef SYS_GVIMRC_FILE
    version_செய்தி(_("  system gvimrc file: \""));
    version_செய்தி(SYS_GVIMRC_FILE);
    version_செய்தி("\"\n");
# endif
    version_செய்தி(_("    user gvimrc file: \""));
    version_செய்தி(USR_GVIMRC_FILE);
    version_செய்தி("\"\n");
# ifdef USR_GVIMRC_FILE2
    version_செய்தி(_("2nd user gvimrc file: \""));
    version_செய்தி(USR_GVIMRC_FILE2);
    version_செய்தி("\"\n");
# endif
# ifdef USR_GVIMRC_FILE3
    version_செய்தி(_("3rd user gvimrc file: \""));
    version_செய்தி(USR_GVIMRC_FILE3);
    version_செய்தி("\"\n");
# endif
#endif
    version_செய்தி(_("       defaults file: \""));
    version_செய்தி(VIM_DEFAULTS_FILE);
    version_செய்தி("\"\n");
#ifdef FEAT_GUI
# ifdef SYS_MENU_FILE
    version_செய்தி(_("    system menu file: \""));
    version_செய்தி(SYS_MENU_FILE);
    version_செய்தி("\"\n");
# endif
#endif
#ifdef HAVE_PATHDEF
    எனில் (*default_vim_dir != NUL)
    {
	version_செய்தி(_("  fall-back for $VIM: \""));
	version_செய்தி((எழுத்து *)default_vim_dir);
	version_செய்தி("\"\n");
    }
    எனில் (*default_vimruntime_dir != NUL)
    {
	version_செய்தி(_(" f-b for $VIMRUNTIME: \""));
	version_செய்தி((எழுத்து *)default_vimruntime_dir);
	version_செய்தி("\"\n");
    }
    version_செய்தி(_("Compilation: "));
    version_செய்தி((எழுத்து *)all_cflags);
    version_செய்தி("\n");
# ifdef VMS
    எனில் (*compiler_version != NUL)
    {
	version_செய்தி(_("Compiler: "));
	version_செய்தி((எழுத்து *)compiler_version);
	version_செய்தி("\n");
    }
# endif
    version_செய்தி(_("Linking: "));
    version_செய்தி((எழுத்து *)all_lflags);
#endif
#ifdef DEBUG
    version_செய்தி("\n");
    version_செய்தி(_("  DEBUG BUILD"));
#endif
}

நிலையான வெறுமை do_intro_வரி(முழு row, char_u *mesg, முழு add_version, முழு attr);
நிலையான வெறுமை intro_message(முழு colon);

/*
 * Show the intro message when not editing a file.
 */
    வெறுமை
maybe_intro_message(வெறுமை)
{
    எனில் (BUFEMPTY()
	    && curbuf->b_fname == வெற்று
	    && firstwin->w_next == வெற்று
	    && vim_strchr(p_shm, SHM_INTRO) == வெற்று)
	intro_message(FALSE);
}

/*
 * Give an introductory message about Vim.
 * Only used when starting Vim on an empty file, without a file name.
 * Or with the ":intro" command (for Sven :-).
 */
    நிலையான வெறுமை
intro_message(
    முழு		colon)		// TRUE for ":intro"
{
    முழு		i;
    முழு		row;
    முழு		blanklines;
    முழு		sponsor;
    எழுத்து	*p;
    நிலையான எழுத்து	*(வரிகள்[]) =
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
	வெற்று,
	"",
	N_("Running in Vi compatible mode"),
	N_("type  :set nocp<Enter>        for Vim defaults"),
	N_("type  :help cp-default<Enter> for info on this"),
    };
#ifdef FEAT_GUI
    நிலையான எழுத்து	*(gui_வரிகள்[]) =
    {
	வெற்று,
	வெற்று,
	வெற்று,
	வெற்று,
# ifdef MODIFIED_BY
	வெற்று,
# endif
	வெற்று,
	வெற்று,
	வெற்று,
	N_("menu  Help->Orphans           for information    "),
	வெற்று,
	N_("Running modeless, typed text is inserted"),
	N_("menu  Edit->Global Settings->Toggle Insert Mode  "),
	N_("                              for two modes      "),
	வெற்று,
	வெற்று,
	வெற்று,
	N_("menu  Edit->Global Settings->Toggle Vi Compatible"),
	N_("                              for Vim defaults   "),
    };
#endif

    // blanklines = screen height - # message lines
    blanklines = (முழு)Rows - (ARRAY_LENGTH(வரிகள்) - 1);
    எனில் (!p_cp)
	blanklines += 4;  // add 4 for not showing "Vi compatible" message

    // Don't overwrite a statusline.  Depends on 'cmdheight'.
    எனில் (p_ls > 1)
	blanklines -= Rows - topframe->fr_height;
    எனில் (blanklines < 0)
	blanklines = 0;

    // Show the sponsor and Uganda message two out of four times
    sponsor = (முழு)time(வெற்று);
    sponsor = ((sponsor & 2) == 0) - ((sponsor & 4) == 0);

    // start displaying the message lines after half of the blank lines
    row = blanklines / 2;
    எனில் ((row >= 2 && topframe->fr_width >= 50) || colon)
    {
	க்கு (i = 0; i < (முழு)ARRAY_LENGTH(வரிகள்); ++i)
	{
	    p = வரிகள்[i];
#ifdef FEAT_GUI
	    எனில் (p_im && gui.in_use && gui_வரிகள்[i] != வெற்று)
		p = gui_வரிகள்[i];
#endif
	    எனில் (p == வெற்று)
	    {
		எனில் (!p_cp)
		    நிறுத்து;
		தொடர்;
	    }
	    எனில் (sponsor != 0)
	    {
		எனில் (strstr(p, "children") != வெற்று)
		    p = N_("Sponsor Vim development!");
		இல்லையெனில் எனில் (strstr(p, "Kuwasha") != வெற்று)
		    p = N_("type  :help sponsor<Enter>    for information ");
		இல்லையெனில் எனில் (strstr(p, "Orphans") != வெற்று)
		    p = N_("menu  Help->Sponsor           for information    ");
	    }
	    எனில் (*p != NUL)
		do_intro_வரி(row, (char_u *)_(p), i == 2, 0);
	    ++row;
	}
    }

    // Make the wait-return message appear just below the text.
    எனில் (colon)
	செய்தி_row = row;
}

    நிலையான வெறுமை
do_intro_வரி(
    முழு		row,
    char_u	*mesg,
    முழு		add_version,
    முழு		attr)
{
    char_u	vers[20];
    முழு		col;
    char_u	*p;
    முழு		l;
    முழு		clen;
#ifdef MODIFIED_BY
# define MODBY_LEN 150
    char_u	modby[MODBY_LEN];

    எனில் (*mesg == ' ')
    {
	vim_strncpy(modby, (char_u *)_("Modified by "), MODBY_LEN - 1);
	l = (முழு)STRLEN(modby);
	vim_strncpy(modby + l, (char_u *)MODIFIED_BY, MODBY_LEN - l - 1);
	mesg = modby;
    }
#endif

    // Center the message horizontally.
    col = vim_strsize(mesg);
    எனில் (add_version)
    {
	STRCPY(vers, mediumVersion);
	எனில் (highest_patch())
	{
	    // Check for 9.9x or 9.9xx, alpha/beta version
	    எனில் (SAFE_isalpha((முழு)vers[3]))
	    {
		முழு len = (SAFE_isalpha((முழு)vers[4])) ? 5 : 4;
		sprintf((எழுத்து *)vers + len, ".%d%s", highest_patch(),
							 mediumVersion + len);
	    }
	    இல்லையெனில்
		sprintf((எழுத்து *)vers + 3, ".%d", highest_patch());
	}
	col += (முழு)STRLEN(vers);
    }
    col = (topframe->fr_width - col) / 2;
    எனில் (col < 0)
	col = 0;

    // Split up in parts to highlight <> items differently.
    க்கு (p = mesg; *p != NUL; p += l)
    {
	clen = 0;
	க்கு (l = 0; p[l] != NUL
			 && (l == 0 || (p[l] != '<' && p[l - 1] != '>')); ++l)
	{
	    எனில் (has_mbyte)
	    {
		clen += ptr2cells(p + l);
		l += (*mb_ptr2len)(p + l) - 1;
	    }
	    இல்லையெனில்
		clen += byte2cells(p[l]);
	}
	screen_puts_len(p, l, row, col + firstwin->w_wincol,
		*p == '<' ? HL_ATTR(HLF_8) : attr);
	col += clen;
    }

    // Add the version number to the version line.
    எனில் (add_version)
	screen_puts(vers, row, col + firstwin->w_wincol, 0);
}

/*
 * ":intro": clear screen, display intro screen and wait for return.
 */
    வெறுமை
ex_intro(exarg_T *eap UNUSED)
{
    screenclear();
#எனில் defined(FEAT_TABPANEL)
    draw_tabpanel();
#endif
    intro_message(TRUE);
    wait_return(TRUE);
}
