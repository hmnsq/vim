/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#अन्तर्भाव "vim.h"

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

#अन्तर्भाव "version.h"

अक्षर		*Version = VIM_VERSION_SHORT;
स्थिर अक्षर	*mediumVersion = VIM_VERSION_MEDIUM;

#जर defined(HAVE_DATE_TIME)
# जर defined(VMS) && defined(VAXC)
अक्षर	longVersion[आकार(VIM_VERSION_LONG_DATE) + आकार(__DATE__)
						      + आकार(__TIME__) + 3];

    शून्य
init_longVersion(शून्य)
{
    /*
     * Construct the long version string.  Necessary because
     * VAX C can't concatenate strings in the preprocessor.
     */
    म_नकल(longVersion, VIM_VERSION_LONG_DATE);
#  ifdef BUILD_DATE
    म_जोड़ो(longVersion, BUILD_DATE);
#  अन्यथा
    म_जोड़ो(longVersion, __DATE__);
    म_जोड़ो(longVersion, " ");
    म_जोड़ो(longVersion, __TIME__);
#  endif
    म_जोड़ो(longVersion, ")");
}

# अन्यथा
अक्षर	*longVersion = रिक्त;

    शून्य
init_longVersion(शून्य)
{
    जर (longVersion != रिक्त)
	परत;

#  ifdef BUILD_DATE
    अक्षर *date_time = BUILD_DATE;
#  अन्यथा
    अक्षर *date_time = __DATE__ " " __TIME__;
#  endif
    अक्षर *संदेश = _("%s (%s, compiled %s)");
    माप_प्रकार len = म_माप(संदेश)
	+ STRLEN_LITERAL(VIM_VERSION_LONG_ONLY)
	+ STRLEN_LITERAL(VIM_VERSION_DATE_ONLY)
	+ म_माप(date_time);

    longVersion = alloc(len);
    जर (longVersion == रिक्त)
	longVersion = VIM_VERSION_LONG;
    अन्यथा
	vim_snprintf(longVersion, len, संदेश,
		VIM_VERSION_LONG_ONLY, VIM_VERSION_DATE_ONLY, date_time);
}
# endif
#अन्यथा
अक्षर	*longVersion = VIM_VERSION_LONG;

    शून्य
init_longVersion(शून्य)
{
    // nothing to do
}
#endif

स्थिर अक्षर *(features[]) =
{
#ifdef HAVE_ACL
	"+acl",
#अन्यथा
	"-acl",
#endif
#ifdef FEAT_ARABIC
	"+arabic",
#अन्यथा
	"-arabic",
#endif
#ifdef AMIGA		// only for Amiga systems
# ifdef FEAT_ARP
	"+ARP",
# अन्यथा
	"-ARP",
# endif
#endif
#ifdef FEAT_AUTOCHDIR
	"+autochdir",
#अन्यथा
	"-autochdir",
#endif
	"+autocmd",
#ifdef FEAT_AUTOSERVERNAME
	"+autoservername",
#अन्यथा
	"-autoservername",
#endif
#ifdef FEAT_BEVAL_GUI
	"+balloon_eval",
#अन्यथा
	"-balloon_eval",
#endif
#ifdef FEAT_BEVAL_TERM
	"+balloon_eval_term",
#अन्यथा
	"-balloon_eval_term",
#endif
#ifdef FEAT_BROWSE
	"+browse",
#अन्यथा
	"-browse",
#endif
	"++builtin_terms",
#ifdef FEAT_BYTEOFF
	"+byte_offset",
#अन्यथा
	"-byte_offset",
#endif
#ifdef FEAT_JOB_CHANNEL
	"+channel",
#अन्यथा
	"-channel",
#endif
	"+cindent",
#ifdef FEAT_CLIENTSERVER
	"+clientserver",
#अन्यथा
	"-clientserver",
#endif
#ifdef FEAT_CLIPBOARD
	"+clipboard",
#अन्यथा
	"-clipboard",
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	"+clipboard_provider",
#अन्यथा
	"-clipboard_provider",
#endif
	"+cmdline_compl",
	"+cmdline_hist",
	"+cmdline_info",
	"+comments",
#ifdef FEAT_CONCEAL
	"+conceal",
#अन्यथा
	"-conceal",
#endif
#ifdef FEAT_CRYPT
	"+cryptv",
#अन्यथा
	"-cryptv",
#endif
#ifdef FEAT_CSCOPE
	"+cscope",
#अन्यथा
	"-cscope",
#endif
	"+cursorbind",
#ifdef CURSOR_SHAPE
	"+cursorshape",
#अन्यथा
	"-cursorshape",
#endif
#जर defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
	"+dialog_con_gui",
#अन्यथा
# जर defined(FEAT_CON_DIALOG)
	"+dialog_con",
# अन्यथा
#  जर defined(FEAT_GUI_DIALOG)
	"+dialog_gui",
#  अन्यथा
	"-dialog",
#  endif
# endif
#endif
#ifdef FEAT_DIFF
	"+diff",
#अन्यथा
	"-diff",
#endif
#ifdef FEAT_DIGRAPHS
	"+digraphs",
#अन्यथा
	"-digraphs",
#endif
#ifdef FEAT_GUI_MSWIN
# ifdef FEAT_DIRECTX
	"+directx",
# अन्यथा
	"-directx",
# endif
#endif
#ifdef FEAT_DND
	"+dnd",
#अन्यथा
	"-dnd",
#endif
	"-ebcdic",
#ifdef FEAT_EMACS_TAGS
	"+emacs_tags",
#अन्यथा
	"-emacs_tags",
#endif
#ifdef FEAT_EVAL
	"+eval",
#अन्यथा
	"-eval",
#endif
	"+ex_extra",
#ifdef FEAT_शोध_EXTRA
	"+extra_search",
#अन्यथा
	"-extra_search",
#endif
	"-farsi",
	"+file_in_path",
#ifdef FEAT_FIND_ID
	"+find_in_path",
#अन्यथा
	"-find_in_path",
#endif
	"+float",
#ifdef FEAT_FOLDING
	"+folding",
#अन्यथा
	"-folding",
#endif
	"-footer",
	// only interesting on Unix systems
#जर !defined(USE_SYSTEM) && defined(UNIX)
	"+fork()",
#endif
#ifdef FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
	"+gettext/dyn",
# अन्यथा
	"+gettext",
# endif
#अन्यथा
	"-gettext",
#endif
	"-hangul_input",
#जर (defined(HAVE_ICONV_H) && defined(USE_ICONV)) || defined(DYNAMIC_ICONV)
# ifdef DYNAMIC_ICONV
	"+iconv/dyn",
# अन्यथा
	"+iconv",
# endif
#अन्यथा
	"-iconv",
#endif
#ifdef FEAT_IMAGE
	"+image",
#अन्यथा
	"-image",
#endif
#ifdef FEAT_IMAGE_POPUP
	"+image_popup",
#अन्यथा
	"-image_popup",
#endif
	"+insert_expand",
#ifdef FEAT_IPV6
	"+ipv6",
#अन्यथा
	"-ipv6",
#endif
#ifdef FEAT_JOB_CHANNEL
	"+job",
#अन्यथा
	"-job",
#endif
	"+jumplist",
#ifdef FEAT_KEYMAP
	"+keymap",
#अन्यथा
	"-keymap",
#endif
#ifdef FEAT_EVAL
	"+lambda",
#अन्यथा
	"-lambda",
#endif
#ifdef FEAT_LANGMAP
	"+langmap",
#अन्यथा
	"-langmap",
#endif
#ifdef FEAT_LIBCALL
	"+libcall",
#अन्यथा
	"-libcall",
#endif
#ifdef FEAT_LINEBREAK
	"+linebreak",
#अन्यथा
	"-linebreak",
#endif
	"+lispindent",
	"+listcmds",
	"+localmap",
#ifdef FEAT_LUA
# ifdef DYNAMIC_LUA
	"+lua/dyn",
# अन्यथा
	"+lua",
# endif
#अन्यथा
	"-lua",
#endif
#ifdef FEAT_MENU
	"+menu",
#अन्यथा
	"-menu",
#endif
#ifdef FEAT_SESSION
	"+mksession",
#अन्यथा
	"-mksession",
#endif
	"+modify_fname",
	"+mouse",
#जर defined(UNIX) || defined(VMS)
# ifdef FEAT_MOUSE_DEC
	"+mouse_dec",
# अन्यथा
	"-mouse_dec",
# endif
# ifdef FEAT_MOUSE_GPM
#  ifdef DYNAMIC_GPM
	"+mouse_gpm/dyn",
#  अन्यथा
	"+mouse_gpm",
#  endif
# अन्यथा
	"-mouse_gpm",
# endif
# ifdef FEAT_MOUSE_JSB
	"+mouse_jsbterm",
# अन्यथा
	"-mouse_jsbterm",
# endif
# ifdef FEAT_MOUSE_NET
	"+mouse_netterm",
# अन्यथा
	"-mouse_netterm",
# endif
#endif

#ifdef __QNX__
# ifdef FEAT_MOUSE_PTERM
	"+mouse_pterm",
# अन्यथा
	"-mouse_pterm",
# endif
#endif

#जर defined(UNIX) || defined(VMS)
	"+mouse_sgr",
# ifdef FEAT_SYSMOUSE
	"+mouse_sysmouse",
# अन्यथा
	"-mouse_sysmouse",
# endif
# ifdef FEAT_MOUSE_URXVT
	"+mouse_urxvt",
# अन्यथा
	"-mouse_urxvt",
# endif
	"+mouse_xterm",
#endif
#ifdef FEAT_MOUSESHAPE
	"+mouseshape",
#अन्यथा
	"-mouseshape",
#endif

#ifdef FEAT_MBYTE_IME
# ifdef DYNAMIC_IME
	"+multi_byte_ime/dyn",
# अन्यथा
	"+multi_byte_ime",
# endif
#अन्यथा
	"+multi_byte",
#endif
#ifdef FEAT_MULTI_LANG
	"+multi_lang",
#अन्यथा
	"-multi_lang",
#endif
#ifdef FEAT_MZSCHEME
# ifdef DYNAMIC_MZSCHEME
	"+mzscheme/dyn",
# अन्यथा
	"+mzscheme",
# endif
#अन्यथा
	"-mzscheme",
#endif
#ifdef FEAT_NETBEANS_INTG
	"+netbeans_intg",
#अन्यथा
	"-netbeans_intg",
#endif
	"+num64",
#ifdef FEAT_GUI_MSWIN
# ifdef FEAT_OLE
	"+ole",
# अन्यथा
	"-ole",
# endif
#endif
#ifdef FEAT_EVAL
	"+packages",
#अन्यथा
	"-packages",
#endif
#ifdef FEAT_PRINT_PANGO
	"+pango",
#अन्यथा
	"-pango",
#endif
	"+path_extra",
#ifdef FEAT_PERL
# ifdef DYNAMIC_PERL
	"+perl/dyn",
# अन्यथा
	"+perl",
# endif
#अन्यथा
	"-perl",
#endif
#ifdef FEAT_PERSISTENT_UNDO
	"+persistent_undo",
#अन्यथा
	"-persistent_undo",
#endif
#ifdef FEAT_PROP_POPUP
	"+popupwin",
#अन्यथा
	"-popupwin",
#endif
#ifdef FEAT_PRINTER
# ifdef FEAT_POSTSCRIPT
	"+postscript",
# अन्यथा
	"-postscript",
# endif
	"+printer",
#अन्यथा
	"-printer",
#endif
#ifdef FEAT_PROFILE
	"+profile",
#अन्यथा
	"-profile",
#endif
#ifdef FEAT_PYTHON
# ifdef DYNAMIC_PYTHON
	"+python/dyn",
# अन्यथा
	"+python",
# endif
#अन्यथा
	"-python",
#endif
#ifdef FEAT_PYTHON3
# ifdef DYNAMIC_PYTHON3
#  ifdef DYNAMIC_PYTHON3_STABLE_ABI
	"+python3/dyn-stable",
#  अन्यथा
	"+python3/dyn",
#  endif
# अन्यथा
	"+python3",
# endif
#अन्यथा
	"-python3",
#endif
#ifdef FEAT_QUICKFIX
	"+quickfix",
#अन्यथा
	"-quickfix",
#endif
#ifdef FEAT_RELTIME
	"+reltime",
#अन्यथा
	"-reltime",
#endif
#ifdef FEAT_RIGHTLEFT
	"+rightleft",
#अन्यथा
	"-rightleft",
#endif
#ifdef FEAT_RUBY
# ifdef DYNAMIC_RUBY
	"+ruby/dyn",
# अन्यथा
	"+ruby",
# endif
#अन्यथा
	"-ruby",
#endif
	"+scrollbind",
#ifdef FEAT_SIGNS
	"+signs",
#अन्यथा
	"-signs",
#endif
	"+smartindent",
#ifdef FEAT_SOCKETSERVER
	"+socketserver",
#अन्यथा
	"-socketserver",
#endif
#ifdef FEAT_SODIUM
# ifdef DYNAMIC_SODIUM
	"+sodium/dyn",
# अन्यथा
	"+sodium",
# endif
#अन्यथा
	"-sodium",
#endif
#ifdef FEAT_SOUND
	"+sound",
#अन्यथा
	"-sound",
#endif
#ifdef FEAT_SPELL
	"+spell",
#अन्यथा
	"-spell",
#endif
#ifdef STARTUPTIME
	"+startuptime",
#अन्यथा
	"-startuptime",
#endif
#ifdef FEAT_STL_OPT
	"+statusline",
#अन्यथा
	"-statusline",
#endif
	"-sun_workshop",
#ifdef FEAT_SYN_HL
	"+syntax",
#अन्यथा
	"-syntax",
#endif
	    // only interesting on Unix systems
#जर defined(USE_SYSTEM) && defined(UNIX)
	"+system()",
#endif
#जर defined(FEAT_TABPANEL)
	"+tabpanel",
#अन्यथा
	"-tabpanel",
#endif
	"-tag_any_white",
	"+tag_binary",
	"-tag_old_static",
#ifdef FEAT_TCL
# ifdef DYNAMIC_TCL
	"+tcl/dyn",
# अन्यथा
	"+tcl",
# endif
#अन्यथा
	"-tcl",
#endif
#ifdef FEAT_TERMGUICOLORS
	"+termguicolors",
#अन्यथा
	"-termguicolors",
#endif
#ifdef FEAT_TERMINAL
	"+terminal",
#अन्यथा
	"-terminal",
#endif
#जर defined(UNIX)
// only Unix can have terminfo instead of termcap
# ifdef TERMINFO
	"+terminfo",
# अन्यथा
	"-terminfo",
# endif
#endif
#ifdef FEAT_TERMRESPONSE
	"+termresponse",
#अन्यथा
	"-termresponse",
#endif
	"+textobjects",
#ifdef FEAT_PROP_POPUP
	"+textprop",
#अन्यथा
	"-textprop",
#endif
#जर !defined(UNIX)
// unix always includes termcap support
# ifdef HAVE_TGETENT
	"+tgetent",
# अन्यथा
	"-tgetent",
# endif
#endif
#ifdef FEAT_TIMERS
	"+timers",
#अन्यथा
	"-timers",
#endif
	"+title",
#ifdef FEAT_TOOLBAR
	"+toolbar",
#अन्यथा
	"-toolbar",
#endif
	"+user_commands",
#ifdef FEAT_VARTABS
	"+vartabs",
#अन्यथा
	"-vartabs",
#endif
	"+vertsplit",
	"+vim9script",
#ifdef FEAT_VIMINFO
	"+viminfo",
#अन्यथा
	"-viminfo",
#endif
	"+virtualedit",
	"+visual",
	"+visualextra",
	"+vreplace",
#ifdef MSWIN
# ifdef FEAT_VTP
	"+vtp",
# अन्यथा
	"-vtp",
# endif
#endif
#ifdef FEAT_WAYLAND
	"+wayland",
#अन्यथा
	"-wayland",
#endif
#ifdef FEAT_WAYLAND_CLIPBOARD
	"+wayland_clipboard",
#अन्यथा
	"-wayland_clipboard",
#endif
	"+wildignore",
	"+wildmenu",
	"+windows",
#ifdef FEAT_WRITEBACKUP
	"+writebackup",
#अन्यथा
	"-writebackup",
#endif
#जर defined(UNIX) || defined(VMS)
# ifdef FEAT_X11
	"+X11",
# अन्यथा
	"-X11",
# endif
#endif
#ifdef FEAT_XATTR
	"+xattr",
#अन्यथा
	"-xattr",
#endif
#ifdef FEAT_XFONTSET
	"+xfontset",
#अन्यथा
	"-xfontset",
#endif
#ifdef FEAT_XIM
	"+xim",
#अन्यथा
	"-xim",
#endif
#जर defined(MSWIN)
# ifdef FEAT_XPM_W32
	"+xpm_w32",
# अन्यथा
	"-xpm_w32",
# endif
#किंवाजर defined(HAVE_XPM)
	"+xpm",
#अन्यथा
	"-xpm",
#endif
#जर defined(UNIX) || defined(VMS)
# जर defined(USE_XSMP_INTERACT)
	"+xsmp_interact",
# elif defined(USE_XSMP)
	"+xsmp",
# अन्यथा
	"-xsmp",
# endif
# ifdef FEAT_XCLIPBOARD
	"+xterm_clipboard",
# अन्यथा
	"-xterm_clipboard",
# endif
#endif
#ifdef FEAT_XTERM_SAVE
	"+xterm_save",
#अन्यथा
	"-xterm_save",
#endif
	रिक्त
};

स्थिर पूर्णांक included_patches[] =
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
स्थिर अक्षर *(extra_patches[]) =
{   /* Add your patch description below this line */
/**/
    रिक्त
};

    पूर्णांक
highest_patch(शून्य)
{
    // this relies on the highest patch number to be the first entry
    परत included_patches[0];
}

#जर defined(FEAT_EVAL)
/*
 * Return TRUE if patch "n" has been included.
 */
    पूर्णांक
has_patch(पूर्णांक n)
{
    पूर्णांक		h, m, l;

    // Perform a binary search.
    l = 0;
    h = (पूर्णांक)ARRAY_LENGTH(included_patches) - 1;
    साठी (;;)
    {
	m = (l + h) / 2;
	जर (included_patches[m] == n)
	    परत TRUE;
	जर (l == h)
	    थांबा;
	जर (included_patches[m] < n)
	    h = m;
	अन्यथा
	    l = m + 1;
    }
    परत FALSE;
}
#endif

    शून्य
ex_version(exarg_T *eap)
{
    /*
     * Ignore a ":version 9.99" command.
     */
    जर (*eap->arg == NUL)
    {
	संदेश_putchar('\n');
	list_version();
    }
}

/*
 * Output a string for the version message.  If it's going to wrap, output a
 * newline, unless the message is too long to fit on the screen anyway.
 * When "wrap" is TRUE wrap the string in [].
 */
    स्थिर शून्य
version_संदेश_wrap(char_u *s, पूर्णांक wrap)
{
    पूर्णांक		len = vim_strsize(s) + (wrap ? 2 : 0);

    जर (!got_int && len < cmdline_width && संदेश_col + len >= cmdline_width
								&& *s != '\n')
	संदेश_putchar('\n');
    जर (!got_int)
    {
	जर (wrap)
	    संदेश_puts("[");
	संदेश_puts((अक्षर *)s);
	जर (wrap)
	    संदेश_puts("]");
    }
}

    स्थिर शून्य
version_संदेश(अक्षर *s)
{
    version_संदेश_wrap((char_u *)s, FALSE);
}

/*
 * List all features aligned in columns, dictionary style.
 */
    स्थिर शून्य
list_features(शून्य)
{
    list_in_columns((char_u **)features, -1, -1);
}

/*
 * List string items nicely aligned in columns.
 * When "size" is < 0 then the last entry is marked with NULL.
 * The entry with index "current" is inclosed in [].
 */
    शून्य
list_in_columns(char_u **वस्तुसंच, पूर्णांक size, पूर्णांक current)
{
    पूर्णांक		i;
    पूर्णांक		ncol;
    पूर्णांक		nrow;
    पूर्णांक		cur_row = 1;
    पूर्णांक		item_मोजणी = 0;
    पूर्णांक		width = 0;
#ifdef FEAT_SYN_HL
    पूर्णांक		use_highlight = (वस्तुसंच == (char_u **)features);
#endif

    // Find the length of the longest item, use that + 1 as the column
    // width.
    साठी (i = 0; size < 0 ? वस्तुसंच[i] != रिक्त : i < size; ++i)
    {
	पूर्णांक l = vim_strsize(वस्तुसंच[i]) + (i == current ? 2 : 0);

	जर (l > width)
	    width = l;
	++item_मोजणी;
    }
    width += 1;

    जर (cmdline_width < width)
    {
	// Not enough screen columns - show one per line
	साठी (i = 0; i < item_मोजणी; ++i)
	{
	    version_संदेश_wrap(वस्तुसंच[i], i == current);
	    जर (संदेश_col > 0 && i < item_मोजणी - 1)
		संदेश_putchar('\n');
	}
	परत;
    }

    // The rightmost column doesn't need a separator.
    // Sacrifice it to fit in one more column if possible.
    ncol = (cmdline_width + 1) / width;
    nrow = item_मोजणी / ncol + ((item_मोजणी % ncol) ? 1 : 0);

    // "i" counts columns then rows.  "idx" counts rows then columns.
    साठी (i = 0; !got_int && i < nrow * ncol; ++i)
    {
	पूर्णांक idx = (i / ncol) + (i % ncol) * nrow;

	जर (idx < item_मोजणी)
	{
	    पूर्णांक last_col = (i + 1) % ncol == 0;

	    जर (idx == current)
		संदेश_putchar('[');
#ifdef FEAT_SYN_HL
	    जर (use_highlight && वस्तुसंच[idx][0] == '-')
		संदेश_puts_attr((अक्षर *)वस्तुसंच[idx], HL_ATTR(HLF_W));
	    अन्यथा
#endif
		संदेश_puts((अक्षर *)वस्तुसंच[idx]);
	    जर (idx == current)
		संदेश_putchar(']');
	    जर (last_col)
	    {
		जर (संदेश_col > 0 && cur_row < nrow)
		    संदेश_putchar('\n');
		++cur_row;
	    }
	    अन्यथा
	    {
		पर्यंत (संदेश_col % width)
		    संदेश_putchar(' ');
	    }
	}
	अन्यथा
	{
	    // this row is out of items, thus at the end of the row
	    जर (संदेश_col > 0)
	    {
		जर (cur_row < nrow)
		    संदेश_putchar('\n');
		++cur_row;
	    }
	}
    }
}

    शून्य
list_version(शून्य)
{
    पूर्णांक		i;
    पूर्णांक		first;
    अक्षर	*s = "";

    /*
     * When adding features here, don't forget to update the list of
     * internal variables in eval.c!
     */
    init_longVersion();
    संदेश(longVersion);
#ifdef MSWIN
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
#   ifdef _WIN64
#    जर defined(_M_ARM64) || defined(_M_ARM64EC)
     संदेश_puts(_("\nMS-Windows ARM64 GUI/console version"));
#    अन्यथा
     संदेश_puts(_("\nMS-Windows 64-bit GUI/console version"));
#    endif
#   अन्यथा
    संदेश_puts(_("\nMS-Windows 32-bit GUI/console version"));
#   endif
#  अन्यथा
#   ifdef _WIN64
#    जर defined(_M_ARM64) || defined(_M_ARM64EC)
     संदेश_puts(_("\nMS-Windows ARM64 GUI version"));
#    अन्यथा
     संदेश_puts(_("\nMS-Windows 64-bit GUI version"));
#    endif
#   अन्यथा
    संदेश_puts(_("\nMS-Windows 32-bit GUI version"));
#   endif
#  endif
#  ifdef FEAT_OLE
    संदेश_puts(_(" with OLE support"));
#  endif
# अन्यथा
#  ifdef _WIN64
#   जर defined(_M_ARM64) || defined(_M_ARM64EC)
    संदेश_puts(_("\nMS-Windows ARM64 console version"));
#   अन्यथा
    संदेश_puts(_("\nMS-Windows 64-bit console version"));
#   endif
#  अन्यथा
    संदेश_puts(_("\nMS-Windows 32-bit console version"));
#  endif
# endif
#endif
#जर defined(MACOS_X)
# जर defined(MACOS_X_DARWIN)
    संदेश_puts(_("\nmacOS version"));
# अन्यथा
    संदेश_puts(_("\nmacOS version w/o darwin feat."));
# endif
# जर defined(__arm64__)
    संदेश_puts(" - arm64");
# elif defined(__x86_64__)
    संदेश_puts(" - x86_64");
# endif
#endif

#ifdef VMS
    संदेश_puts(_("\nOpenVMS (build) arch, version"));
# ifdef HAVE_PATHDEF
    जर (*compiled_arch != NUL)
    {
	संदेश_puts(": ");
	संदेश_puts((अक्षर *)compiled_arch);
	जर (*compiled_vers != NUL)
	{
	    संदेश_puts(", ");
	    संदेश_puts((अक्षर *)compiled_vers);
	}
    }

# endif

#endif

    // Print the list of patch numbers if there is at least one.
    // Print a range when patches are consecutive: "1-10, 12, 15-40, 42-45"
    जर (included_patches[0] != 0)
    {
	संदेश_puts(_("\nIncluded patches: "));
	first = -1;
	i = (पूर्णांक)ARRAY_LENGTH(included_patches) - 1;
	पर्यंत (--i >= 0)
	{
	    जर (first < 0)
		first = included_patches[i];
	    जर (i == 0 || included_patches[i - 1] != included_patches[i] + 1)
	    {
		संदेश_puts(s);
		s = ", ";
		संदेश_outnum((दीर्घ)first);
		जर (first != included_patches[i])
		{
		    संदेश_puts("-");
		    संदेश_outnum((दीर्घ)included_patches[i]);
		}
		first = -1;
	    }
	}
    }

    // Print the list of extra patch descriptions if there is at least one.
    जर (extra_patches[0] != रिक्त)
    {
	संदेश_puts(_("\nExtra patches: "));
	s = "";
	साठी (i = 0; extra_patches[i] != रिक्त; ++i)
	{
	    संदेश_puts(s);
	    s = ", ";
	    संदेश_puts(extra_patches[i]);
	}
    }

#ifdef MODIFIED_BY
    संदेश_puts("\n");
    संदेश_puts(_("Modified by "));
    संदेश_puts(MODIFIED_BY);
#endif

#ifdef HAVE_PATHDEF
    जर (*compiled_user != NUL || *compiled_sys != NUL)
    {
	संदेश_puts(_("\nCompiled "));
	जर (*compiled_user != NUL)
	{
	    संदेश_puts(_("by "));
	    संदेश_puts((अक्षर *)compiled_user);
	}
	जर (*compiled_sys != NUL)
	{
	    संदेश_puts("@");
	    संदेश_puts((अक्षर *)compiled_sys);
	}
    }
#endif

#जर defined(FEAT_HUGE)
    संदेश_puts(_("\nHuge version "));
#किंवाजर defined(FEAT_NORMAL)
    संदेश_puts(_("\nNormal version "));
#अन्यथा
    संदेश_puts(_("\nTiny version "));
#endif
#जर !defined(FEAT_GUI)
    संदेश_puts(_("without GUI."));
#किंवाजर defined(FEAT_GUI_GTK)
# जर defined(USE_GTK4)
    संदेश_puts(_("with GTK4 GUI."));
# elif defined(USE_GTK3)
    संदेश_puts(_("with GTK3 GUI."));
# elif defined(FEAT_GUI_GNOME)
    संदेश_puts(_("with GTK2-GNOME GUI."));
# अन्यथा
    संदेश_puts(_("with GTK2 GUI."));
# endif
#किंवाजर defined(FEAT_GUI_MOTIF)
    संदेश_puts(_("with X11-Motif GUI."));
#किंवाजर defined(FEAT_GUI_HAIKU)
    संदेश_puts(_("with Haiku GUI."));
#किंवाजर defined(FEAT_GUI_PHOTON)
    संदेश_puts(_("with Photon GUI."));
#किंवाजर defined(MSWIN)
    संदेश_puts(_("with GUI."));
#endif
    version_संदेश(_("  Features included (+) or not (-):\n"));

    list_features();
    जर (संदेश_col > 0)
	संदेश_putchar('\n');

#ifdef SYS_VIMRC_FILE
    version_संदेश(_("   system vimrc file: \""));
    version_संदेश(SYS_VIMRC_FILE);
    version_संदेश("\"\n");
#endif
#ifdef USR_VIMRC_FILE
    version_संदेश(_("     user vimrc file: \""));
    version_संदेश(USR_VIMRC_FILE);
    version_संदेश("\"\n");
#endif
#ifdef USR_VIMRC_FILE2
    version_संदेश(_(" 2nd user vimrc file: \""));
    version_संदेश(USR_VIMRC_FILE2);
    version_संदेश("\"\n");
#endif
#जर defined(USR_VIMRC_FILE3) && defined(XDG_VIMRC_FILE)
    version_संदेश(_(" 3rd user vimrc file: \""));
    version_संदेश(USR_VIMRC_FILE3);
    version_संदेश("\"\n");
    version_संदेश(_(" 4th user vimrc file: \""));
    version_संदेश((अक्षर *)(XDG_VIMRC_FILE));
    version_संदेश("\"\n");
#किंवाजर defined(USR_VIMRC_FILE3)
    version_संदेश(_(" 3rd user vimrc file: \""));
    version_संदेश(USR_VIMRC_FILE3);
    version_संदेश("\"\n");
#किंवाजर defined(XDG_VIMRC_FILE)
    version_संदेश(_(" 3rd user vimrc file: \""));
    version_संदेश((अक्षर *)(XDG_VIMRC_FILE));
    version_संदेश("\"\n");
#endif
#ifdef USR_EXRC_FILE
    version_संदेश(_("      user exrc file: \""));
    version_संदेश(USR_EXRC_FILE);
    version_संदेश("\"\n");
#endif
#ifdef USR_EXRC_FILE2
    version_संदेश(_("  2nd user exrc file: \""));
    version_संदेश(USR_EXRC_FILE2);
    version_संदेश("\"\n");
#endif
#ifdef FEAT_GUI
# ifdef SYS_GVIMRC_FILE
    version_संदेश(_("  system gvimrc file: \""));
    version_संदेश(SYS_GVIMRC_FILE);
    version_संदेश("\"\n");
# endif
    version_संदेश(_("    user gvimrc file: \""));
    version_संदेश(USR_GVIMRC_FILE);
    version_संदेश("\"\n");
# ifdef USR_GVIMRC_FILE2
    version_संदेश(_("2nd user gvimrc file: \""));
    version_संदेश(USR_GVIMRC_FILE2);
    version_संदेश("\"\n");
# endif
# ifdef USR_GVIMRC_FILE3
    version_संदेश(_("3rd user gvimrc file: \""));
    version_संदेश(USR_GVIMRC_FILE3);
    version_संदेश("\"\n");
# endif
#endif
    version_संदेश(_("       defaults file: \""));
    version_संदेश(VIM_DEFAULTS_FILE);
    version_संदेश("\"\n");
#ifdef FEAT_GUI
# ifdef SYS_MENU_FILE
    version_संदेश(_("    system menu file: \""));
    version_संदेश(SYS_MENU_FILE);
    version_संदेश("\"\n");
# endif
#endif
#ifdef HAVE_PATHDEF
    जर (*default_vim_dir != NUL)
    {
	version_संदेश(_("  fall-back for $VIM: \""));
	version_संदेश((अक्षर *)default_vim_dir);
	version_संदेश("\"\n");
    }
    जर (*default_vimruntime_dir != NUL)
    {
	version_संदेश(_(" f-b for $VIMRUNTIME: \""));
	version_संदेश((अक्षर *)default_vimruntime_dir);
	version_संदेश("\"\n");
    }
    version_संदेश(_("Compilation: "));
    version_संदेश((अक्षर *)all_cflags);
    version_संदेश("\n");
# ifdef VMS
    जर (*compiler_version != NUL)
    {
	version_संदेश(_("Compiler: "));
	version_संदेश((अक्षर *)compiler_version);
	version_संदेश("\n");
    }
# endif
    version_संदेश(_("Linking: "));
    version_संदेश((अक्षर *)all_lflags);
#endif
#ifdef DEBUG
    version_संदेश("\n");
    version_संदेश(_("  DEBUG BUILD"));
#endif
}

स्थिर शून्य do_intro_ओळ(पूर्णांक row, char_u *mesg, पूर्णांक add_version, पूर्णांक attr);
स्थिर शून्य intro_message(पूर्णांक colon);

/*
 * Show the intro message when not editing a file.
 */
    शून्य
maybe_intro_message(शून्य)
{
    जर (BUFEMPTY()
	    && curbuf->b_fname == रिक्त
	    && firstwin->w_next == रिक्त
	    && vim_strchr(p_shm, SHM_INTRO) == रिक्त)
	intro_message(FALSE);
}

/*
 * Give an introductory message about Vim.
 * Only used when starting Vim on an empty file, without a file name.
 * Or with the ":intro" command (for Sven :-).
 */
    स्थिर शून्य
intro_message(
    पूर्णांक		colon)		// TRUE for ":intro"
{
    पूर्णांक		i;
    पूर्णांक		row;
    पूर्णांक		blanklines;
    पूर्णांक		sponsor;
    अक्षर	*p;
    स्थिर अक्षर	*(ओळी[]) =
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
	रिक्त,
	"",
	N_("Running in Vi compatible mode"),
	N_("type  :set nocp<Enter>        for Vim defaults"),
	N_("type  :help cp-default<Enter> for info on this"),
    };
#ifdef FEAT_GUI
    स्थिर अक्षर	*(gui_ओळी[]) =
    {
	रिक्त,
	रिक्त,
	रिक्त,
	रिक्त,
# ifdef MODIFIED_BY
	रिक्त,
# endif
	रिक्त,
	रिक्त,
	रिक्त,
	N_("menu  Help->Orphans           for information    "),
	रिक्त,
	N_("Running modeless, typed text is inserted"),
	N_("menu  Edit->Global Settings->Toggle Insert Mode  "),
	N_("                              for two modes      "),
	रिक्त,
	रिक्त,
	रिक्त,
	N_("menu  Edit->Global Settings->Toggle Vi Compatible"),
	N_("                              for Vim defaults   "),
    };
#endif

    // blanklines = screen height - # message lines
    blanklines = (पूर्णांक)Rows - (ARRAY_LENGTH(ओळी) - 1);
    जर (!p_cp)
	blanklines += 4;  // add 4 for not showing "Vi compatible" message

    // Don't overwrite a statusline.  Depends on 'cmdheight'.
    जर (p_ls > 1)
	blanklines -= Rows - topframe->fr_height;
    जर (blanklines < 0)
	blanklines = 0;

    // Show the sponsor and Uganda message two out of four times
    sponsor = (पूर्णांक)समय(रिक्त);
    sponsor = ((sponsor & 2) == 0) - ((sponsor & 4) == 0);

    // start displaying the message lines after half of the blank lines
    row = blanklines / 2;
    जर ((row >= 2 && topframe->fr_width >= 50) || colon)
    {
	साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(ओळी); ++i)
	{
	    p = ओळी[i];
#ifdef FEAT_GUI
	    जर (p_im && gui.in_use && gui_ओळी[i] != रिक्त)
		p = gui_ओळी[i];
#endif
	    जर (p == रिक्त)
	    {
		जर (!p_cp)
		    थांबा;
		सुरूठेवा;
	    }
	    जर (sponsor != 0)
	    {
		जर (म_माला(p, "children") != रिक्त)
		    p = N_("Sponsor Vim development!");
		अन्यथा जर (म_माला(p, "Kuwasha") != रिक्त)
		    p = N_("type  :help sponsor<Enter>    for information ");
		अन्यथा जर (म_माला(p, "Orphans") != रिक्त)
		    p = N_("menu  Help->Sponsor           for information    ");
	    }
	    जर (*p != NUL)
		do_intro_ओळ(row, (char_u *)_(p), i == 2, 0);
	    ++row;
	}
    }

    // Make the wait-return message appear just below the text.
    जर (colon)
	संदेश_row = row;
}

    स्थिर शून्य
do_intro_ओळ(
    पूर्णांक		row,
    char_u	*mesg,
    पूर्णांक		add_version,
    पूर्णांक		attr)
{
    char_u	vers[20];
    पूर्णांक		col;
    char_u	*p;
    पूर्णांक		l;
    पूर्णांक		clen;
#ifdef MODIFIED_BY
# define MODBY_LEN 150
    char_u	modby[MODBY_LEN];

    जर (*mesg == ' ')
    {
	vim_strncpy(modby, (char_u *)_("Modified by "), MODBY_LEN - 1);
	l = (पूर्णांक)STRLEN(modby);
	vim_strncpy(modby + l, (char_u *)MODIFIED_BY, MODBY_LEN - l - 1);
	mesg = modby;
    }
#endif

    // Center the message horizontally.
    col = vim_strsize(mesg);
    जर (add_version)
    {
	STRCPY(vers, mediumVersion);
	जर (highest_patch())
	{
	    // Check for 9.9x or 9.9xx, alpha/beta version
	    जर (SAFE_isalpha((पूर्णांक)vers[3]))
	    {
		पूर्णांक len = (SAFE_isalpha((पूर्णांक)vers[4])) ? 5 : 4;
		प्र_लिखो((अक्षर *)vers + len, ".%d%s", highest_patch(),
							 mediumVersion + len);
	    }
	    अन्यथा
		प्र_लिखो((अक्षर *)vers + 3, ".%d", highest_patch());
	}
	col += (पूर्णांक)STRLEN(vers);
    }
    col = (topframe->fr_width - col) / 2;
    जर (col < 0)
	col = 0;

    // Split up in parts to highlight <> items differently.
    साठी (p = mesg; *p != NUL; p += l)
    {
	clen = 0;
	साठी (l = 0; p[l] != NUL
			 && (l == 0 || (p[l] != '<' && p[l - 1] != '>')); ++l)
	{
	    जर (has_mbyte)
	    {
		clen += ptr2cells(p + l);
		l += (*mb_ptr2len)(p + l) - 1;
	    }
	    अन्यथा
		clen += byte2cells(p[l]);
	}
	screen_puts_len(p, l, row, col + firstwin->w_wincol,
		*p == '<' ? HL_ATTR(HLF_8) : attr);
	col += clen;
    }

    // Add the version number to the version line.
    जर (add_version)
	screen_puts(vers, row, col + firstwin->w_wincol, 0);
}

/*
 * ":intro": clear screen, display intro screen and wait for return.
 */
    शून्य
ex_intro(exarg_T *eap UNUSED)
{
    screenclear();
#जर defined(FEAT_TABPANEL)
    draw_tabpanel();
#endif
    intro_message(TRUE);
    wait_return(TRUE);
}
