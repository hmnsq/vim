/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include "vim.h"

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

#include "version.h"

var_na		*Version = VIM_VERSION_SHORT;
static var_na	*mediumVersion = VIM_VERSION_MEDIUM;

#အကယ်၍ defined(HAVE_DATE_TIME)
# အကယ်၍ defined(VMS) && defined(VAXC)
var_na	longVersion[parimaa_na(VIM_VERSION_LONG_DATE) + parimaa_na(__DATE__)
						      + parimaa_na(__TIME__) + 3];

    void
init_longVersion(void)
{
    /*
     * Construct the long version string.  Necessary because
     * VAX C can't concatenate strings in the preprocessor.
     */
    strcpy(longVersion, VIM_VERSION_LONG_DATE);
#  ifdef BUILD_DATE
    strcat(longVersion, BUILD_DATE);
#  မဟုတ်ပါက
    strcat(longVersion, __DATE__);
    strcat(longVersion, " ");
    strcat(longVersion, __TIME__);
#  endif
    strcat(longVersion, ")");
}

# မဟုတ်ပါက
var_na	*longVersion = NULL;

    void
init_longVersion(void)
{
    အကယ်၍ (longVersion != NULL)
	ပြန်ပေး;

#  ifdef BUILD_DATE
    var_na *date_time = BUILD_DATE;
#  မဟုတ်ပါက
    var_na *date_time = __DATE__ " " __TIME__;
#  endif
    var_na *msg = _("%s (%s, compiled %s)");
    size_t len = strlen(msg)
	+ STRLEN_LITERAL(VIM_VERSION_LONG_ONLY)
	+ STRLEN_LITERAL(VIM_VERSION_DATE_ONLY)
	+ strlen(date_time);

    longVersion = alloc(len);
    အကယ်၍ (longVersion == NULL)
	longVersion = VIM_VERSION_LONG;
    မဟုတ်ပါက
	vim_snprintf(longVersion, len, msg,
		VIM_VERSION_LONG_ONLY, VIM_VERSION_DATE_ONLY, date_time);
}
# endif
#မဟုတ်ပါက
var_na	*longVersion = VIM_VERSION_LONG;

    void
init_longVersion(void)
{
    // nothing to do
}
#endif

static var_na *(features[]) =
{
#ifdef HAVE_ACL
	"+acl",
#မဟုတ်ပါက
	"-acl",
#endif
#ifdef FEAT_ARABIC
	"+arabic",
#မဟုတ်ပါက
	"-arabic",
#endif
#ifdef AMIGA		// only for Amiga systems
# ifdef FEAT_ARP
	"+ARP",
# မဟုတ်ပါက
	"-ARP",
# endif
#endif
#ifdef FEAT_AUTOCHDIR
	"+autochdir",
#မဟုတ်ပါက
	"-autochdir",
#endif
	"+autocmd",
#ifdef FEAT_AUTOSERVERNAME
	"+autoservername",
#မဟုတ်ပါက
	"-autoservername",
#endif
#ifdef FEAT_BEVAL_GUI
	"+balloon_eval",
#မဟုတ်ပါက
	"-balloon_eval",
#endif
#ifdef FEAT_BEVAL_TERM
	"+balloon_eval_term",
#မဟုတ်ပါက
	"-balloon_eval_term",
#endif
#ifdef FEAT_BROWSE
	"+browse",
#မဟုတ်ပါက
	"-browse",
#endif
	"++builtin_terms",
#ifdef FEAT_BYTEOFF
	"+byte_offset",
#မဟုတ်ပါက
	"-byte_offset",
#endif
#ifdef FEAT_JOB_CHANNEL
	"+channel",
#မဟုတ်ပါက
	"-channel",
#endif
	"+cindent",
#ifdef FEAT_CLIENTSERVER
	"+clientserver",
#မဟုတ်ပါက
	"-clientserver",
#endif
#ifdef FEAT_CLIPBOARD
	"+clipboard",
#မဟုတ်ပါက
	"-clipboard",
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	"+clipboard_provider",
#မဟုတ်ပါက
	"-clipboard_provider",
#endif
	"+cmdline_compl",
	"+cmdline_hist",
	"+cmdline_info",
	"+comments",
#ifdef FEAT_CONCEAL
	"+conceal",
#မဟုတ်ပါက
	"-conceal",
#endif
#ifdef FEAT_CRYPT
	"+cryptv",
#မဟုတ်ပါက
	"-cryptv",
#endif
#ifdef FEAT_CSCOPE
	"+cscope",
#မဟုတ်ပါက
	"-cscope",
#endif
	"+cursorbind",
#ifdef CURSOR_SHAPE
	"+cursorshape",
#မဟုတ်ပါက
	"-cursorshape",
#endif
#အကယ်၍ defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
	"+dialog_con_gui",
#မဟုတ်ပါက
# အကယ်၍ defined(FEAT_CON_DIALOG)
	"+dialog_con",
# မဟုတ်ပါက
#  အကယ်၍ defined(FEAT_GUI_DIALOG)
	"+dialog_gui",
#  မဟုတ်ပါက
	"-dialog",
#  endif
# endif
#endif
#ifdef FEAT_DIFF
	"+diff",
#မဟုတ်ပါက
	"-diff",
#endif
#ifdef FEAT_DIGRAPHS
	"+digraphs",
#မဟုတ်ပါက
	"-digraphs",
#endif
#ifdef FEAT_GUI_MSWIN
# ifdef FEAT_DIRECTX
	"+directx",
# မဟုတ်ပါက
	"-directx",
# endif
#endif
#ifdef FEAT_DND
	"+dnd",
#မဟုတ်ပါက
	"-dnd",
#endif
	"-ebcdic",
#ifdef FEAT_EMACS_TAGS
	"+emacs_tags",
#မဟုတ်ပါက
	"-emacs_tags",
#endif
#ifdef FEAT_EVAL
	"+eval",
#မဟုတ်ပါက
	"-eval",
#endif
	"+ex_extra",
#ifdef FEAT_SEARCH_EXTRA
	"+extra_search",
#မဟုတ်ပါက
	"-extra_search",
#endif
	"-farsi",
	"+file_in_path",
#ifdef FEAT_FIND_ID
	"+find_in_path",
#မဟုတ်ပါက
	"-find_in_path",
#endif
	"+float",
#ifdef FEAT_FOLDING
	"+folding",
#မဟုတ်ပါက
	"-folding",
#endif
	"-footer",
	// only interesting on Unix systems
#အကယ်၍ !defined(USE_SYSTEM) && defined(UNIX)
	"+fork()",
#endif
#ifdef FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
	"+gettext/dyn",
# မဟုတ်ပါက
	"+gettext",
# endif
#မဟုတ်ပါက
	"-gettext",
#endif
	"-hangul_input",
#အကယ်၍ (defined(HAVE_ICONV_H) && defined(USE_ICONV)) || defined(DYNAMIC_ICONV)
# ifdef DYNAMIC_ICONV
	"+iconv/dyn",
# မဟုတ်ပါက
	"+iconv",
# endif
#မဟုတ်ပါက
	"-iconv",
#endif
#ifdef FEAT_IMAGE
	"+image",
#မဟုတ်ပါက
	"-image",
#endif
#ifdef FEAT_IMAGE_POPUP
	"+image_popup",
#မဟုတ်ပါက
	"-image_popup",
#endif
	"+insert_expand",
#ifdef FEAT_IPV6
	"+ipv6",
#မဟုတ်ပါက
	"-ipv6",
#endif
#ifdef FEAT_JOB_CHANNEL
	"+job",
#မဟုတ်ပါက
	"-job",
#endif
	"+jumplist",
#ifdef FEAT_KEYMAP
	"+keymap",
#မဟုတ်ပါက
	"-keymap",
#endif
#ifdef FEAT_EVAL
	"+lambda",
#မဟုတ်ပါက
	"-lambda",
#endif
#ifdef FEAT_LANGMAP
	"+langmap",
#မဟုတ်ပါက
	"-langmap",
#endif
#ifdef FEAT_LIBCALL
	"+libcall",
#မဟုတ်ပါက
	"-libcall",
#endif
#ifdef FEAT_LINEBREAK
	"+linebreak",
#မဟုတ်ပါက
	"-linebreak",
#endif
	"+lispindent",
	"+listcmds",
	"+localmap",
#ifdef FEAT_LUA
# ifdef DYNAMIC_LUA
	"+lua/dyn",
# မဟုတ်ပါက
	"+lua",
# endif
#မဟုတ်ပါက
	"-lua",
#endif
#ifdef FEAT_MENU
	"+menu",
#မဟုတ်ပါက
	"-menu",
#endif
#ifdef FEAT_SESSION
	"+mksession",
#မဟုတ်ပါက
	"-mksession",
#endif
	"+modify_fname",
	"+mouse",
#အကယ်၍ defined(UNIX) || defined(VMS)
# ifdef FEAT_MOUSE_DEC
	"+mouse_dec",
# မဟုတ်ပါက
	"-mouse_dec",
# endif
# ifdef FEAT_MOUSE_GPM
#  ifdef DYNAMIC_GPM
	"+mouse_gpm/dyn",
#  မဟုတ်ပါက
	"+mouse_gpm",
#  endif
# မဟုတ်ပါက
	"-mouse_gpm",
# endif
# ifdef FEAT_MOUSE_JSB
	"+mouse_jsbterm",
# မဟုတ်ပါက
	"-mouse_jsbterm",
# endif
# ifdef FEAT_MOUSE_NET
	"+mouse_netterm",
# မဟုတ်ပါက
	"-mouse_netterm",
# endif
#endif

#ifdef __QNX__
# ifdef FEAT_MOUSE_PTERM
	"+mouse_pterm",
# မဟုတ်ပါက
	"-mouse_pterm",
# endif
#endif

#အကယ်၍ defined(UNIX) || defined(VMS)
	"+mouse_sgr",
# ifdef FEAT_SYSMOUSE
	"+mouse_sysmouse",
# မဟုတ်ပါက
	"-mouse_sysmouse",
# endif
# ifdef FEAT_MOUSE_URXVT
	"+mouse_urxvt",
# မဟုတ်ပါက
	"-mouse_urxvt",
# endif
	"+mouse_xterm",
#endif
#ifdef FEAT_MOUSESHAPE
	"+mouseshape",
#မဟုတ်ပါက
	"-mouseshape",
#endif

#ifdef FEAT_MBYTE_IME
# ifdef DYNAMIC_IME
	"+multi_byte_ime/dyn",
# မဟုတ်ပါက
	"+multi_byte_ime",
# endif
#မဟုတ်ပါက
	"+multi_byte",
#endif
#ifdef FEAT_MULTI_LANG
	"+multi_lang",
#မဟုတ်ပါက
	"-multi_lang",
#endif
#ifdef FEAT_MZSCHEME
# ifdef DYNAMIC_MZSCHEME
	"+mzscheme/dyn",
# မဟုတ်ပါက
	"+mzscheme",
# endif
#မဟုတ်ပါက
	"-mzscheme",
#endif
#ifdef FEAT_NETBEANS_INTG
	"+netbeans_intg",
#မဟုတ်ပါက
	"-netbeans_intg",
#endif
	"+num64",
#ifdef FEAT_GUI_MSWIN
# ifdef FEAT_OLE
	"+ole",
# မဟုတ်ပါက
	"-ole",
# endif
#endif
#ifdef FEAT_EVAL
	"+packages",
#မဟုတ်ပါက
	"-packages",
#endif
#ifdef FEAT_PRINT_PANGO
	"+pango",
#မဟုတ်ပါက
	"-pango",
#endif
	"+path_extra",
#ifdef FEAT_PERL
# ifdef DYNAMIC_PERL
	"+perl/dyn",
# မဟုတ်ပါက
	"+perl",
# endif
#မဟုတ်ပါက
	"-perl",
#endif
#ifdef FEAT_PERSISTENT_UNDO
	"+persistent_undo",
#မဟုတ်ပါက
	"-persistent_undo",
#endif
#ifdef FEAT_PROP_POPUP
	"+popupwin",
#မဟုတ်ပါက
	"-popupwin",
#endif
#ifdef FEAT_PRINTER
# ifdef FEAT_POSTSCRIPT
	"+postscript",
# မဟုတ်ပါက
	"-postscript",
# endif
	"+printer",
#မဟုတ်ပါက
	"-printer",
#endif
#ifdef FEAT_PROFILE
	"+profile",
#မဟုတ်ပါက
	"-profile",
#endif
#ifdef FEAT_PYTHON
# ifdef DYNAMIC_PYTHON
	"+python/dyn",
# မဟုတ်ပါက
	"+python",
# endif
#မဟုတ်ပါက
	"-python",
#endif
#ifdef FEAT_PYTHON3
# ifdef DYNAMIC_PYTHON3
#  ifdef DYNAMIC_PYTHON3_STABLE_ABI
	"+python3/dyn-stable",
#  မဟုတ်ပါက
	"+python3/dyn",
#  endif
# မဟုတ်ပါက
	"+python3",
# endif
#မဟုတ်ပါက
	"-python3",
#endif
#ifdef FEAT_QUICKFIX
	"+quickfix",
#မဟုတ်ပါက
	"-quickfix",
#endif
#ifdef FEAT_RELTIME
	"+reltime",
#မဟုတ်ပါက
	"-reltime",
#endif
#ifdef FEAT_RIGHTLEFT
	"+rightleft",
#မဟုတ်ပါက
	"-rightleft",
#endif
#ifdef FEAT_RUBY
# ifdef DYNAMIC_RUBY
	"+ruby/dyn",
# မဟုတ်ပါက
	"+ruby",
# endif
#မဟုတ်ပါက
	"-ruby",
#endif
	"+scrollbind",
#ifdef FEAT_SIGNS
	"+signs",
#မဟုတ်ပါက
	"-signs",
#endif
	"+smartindent",
#ifdef FEAT_SOCKETSERVER
	"+socketserver",
#မဟုတ်ပါက
	"-socketserver",
#endif
#ifdef FEAT_SODIUM
# ifdef DYNAMIC_SODIUM
	"+sodium/dyn",
# မဟုတ်ပါက
	"+sodium",
# endif
#မဟုတ်ပါက
	"-sodium",
#endif
#ifdef FEAT_SOUND
	"+sound",
#မဟုတ်ပါက
	"-sound",
#endif
#ifdef FEAT_SPELL
	"+spell",
#မဟုတ်ပါက
	"-spell",
#endif
#ifdef STARTUPTIME
	"+startuptime",
#မဟုတ်ပါက
	"-startuptime",
#endif
#ifdef FEAT_STL_OPT
	"+statusline",
#မဟုတ်ပါက
	"-statusline",
#endif
	"-sun_workshop",
#ifdef FEAT_SYN_HL
	"+syntax",
#မဟုတ်ပါက
	"-syntax",
#endif
	    // only interesting on Unix systems
#အကယ်၍ defined(USE_SYSTEM) && defined(UNIX)
	"+system()",
#endif
#အကယ်၍ defined(FEAT_TABPANEL)
	"+tabpanel",
#မဟုတ်ပါက
	"-tabpanel",
#endif
	"-tag_any_white",
	"+tag_binary",
	"-tag_old_static",
#ifdef FEAT_TCL
# ifdef DYNAMIC_TCL
	"+tcl/dyn",
# မဟုတ်ပါက
	"+tcl",
# endif
#မဟုတ်ပါက
	"-tcl",
#endif
#ifdef FEAT_TERMGUICOLORS
	"+termguicolors",
#မဟုတ်ပါက
	"-termguicolors",
#endif
#ifdef FEAT_TERMINAL
	"+terminal",
#မဟုတ်ပါက
	"-terminal",
#endif
#အကယ်၍ defined(UNIX)
// only Unix can have terminfo instead of termcap
# ifdef TERMINFO
	"+terminfo",
# မဟုတ်ပါက
	"-terminfo",
# endif
#endif
#ifdef FEAT_TERMRESPONSE
	"+termresponse",
#မဟုတ်ပါက
	"-termresponse",
#endif
	"+textobjects",
#ifdef FEAT_PROP_POPUP
	"+textprop",
#မဟုတ်ပါက
	"-textprop",
#endif
#အကယ်၍ !defined(UNIX)
// unix always includes termcap support
# ifdef HAVE_TGETENT
	"+tgetent",
# မဟုတ်ပါက
	"-tgetent",
# endif
#endif
#ifdef FEAT_TIMERS
	"+timers",
#မဟုတ်ပါက
	"-timers",
#endif
	"+title",
#ifdef FEAT_TOOLBAR
	"+toolbar",
#မဟုတ်ပါက
	"-toolbar",
#endif
	"+user_commands",
#ifdef FEAT_VARTABS
	"+vartabs",
#မဟုတ်ပါက
	"-vartabs",
#endif
	"+vertsplit",
	"+vim9script",
#ifdef FEAT_VIMINFO
	"+viminfo",
#မဟုတ်ပါက
	"-viminfo",
#endif
	"+virtualedit",
	"+visual",
	"+visualextra",
	"+vreplace",
#ifdef MSWIN
# ifdef FEAT_VTP
	"+vtp",
# မဟုတ်ပါက
	"-vtp",
# endif
#endif
#ifdef FEAT_WAYLAND
	"+wayland",
#မဟုတ်ပါက
	"-wayland",
#endif
#ifdef FEAT_WAYLAND_CLIPBOARD
	"+wayland_clipboard",
#မဟုတ်ပါက
	"-wayland_clipboard",
#endif
	"+wildignore",
	"+wildmenu",
	"+windows",
#ifdef FEAT_WRITEBACKUP
	"+writebackup",
#မဟုတ်ပါက
	"-writebackup",
#endif
#အကယ်၍ defined(UNIX) || defined(VMS)
# ifdef FEAT_X11
	"+X11",
# မဟုတ်ပါက
	"-X11",
# endif
#endif
#ifdef FEAT_XATTR
	"+xattr",
#မဟုတ်ပါက
	"-xattr",
#endif
#ifdef FEAT_XFONTSET
	"+xfontset",
#မဟုတ်ပါက
	"-xfontset",
#endif
#ifdef FEAT_XIM
	"+xim",
#မဟုတ်ပါက
	"-xim",
#endif
#အကယ်၍ defined(MSWIN)
# ifdef FEAT_XPM_W32
	"+xpm_w32",
# မဟုတ်ပါက
	"-xpm_w32",
# endif
#elif defined(HAVE_XPM)
	"+xpm",
#မဟုတ်ပါက
	"-xpm",
#endif
#အကယ်၍ defined(UNIX) || defined(VMS)
# အကယ်၍ defined(USE_XSMP_INTERACT)
	"+xsmp_interact",
# elif defined(USE_XSMP)
	"+xsmp",
# မဟုတ်ပါက
	"-xsmp",
# endif
# ifdef FEAT_XCLIPBOARD
	"+xterm_clipboard",
# မဟုတ်ပါက
	"-xterm_clipboard",
# endif
#endif
#ifdef FEAT_XTERM_SAVE
	"+xterm_save",
#မဟုတ်ပါက
	"-xterm_save",
#endif
	NULL
};

static ကိန်းပြည့် included_patches[] =
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
static var_na *(extra_patches[]) =
{   /* Add your patch description below this line */
/**/
    NULL
};

    ကိန်းပြည့်
highest_patch(void)
{
    // this relies on the highest patch number to be the first entry
    ပြန်ပေး included_patches[0];
}

#အကယ်၍ defined(FEAT_EVAL)
/*
 * Return TRUE if patch "n" has been included.
 */
    ကိန်းပြည့်
has_patch(ကိန်းပြည့် n)
{
    ကိန်းပြည့်		h, m, l;

    // Perform a binary search.
    l = 0;
    h = (ကိန်းပြည့်)ARRAY_LENGTH(included_patches) - 1;
    အတွက် (;;)
    {
	m = (l + h) / 2;
	အကယ်၍ (included_patches[m] == n)
	    ပြန်ပေး TRUE;
	အကယ်၍ (l == h)
	    bha_nga;
	အကယ်၍ (included_patches[m] < n)
	    h = m;
	မဟုတ်ပါက
	    l = m + 1;
    }
    ပြန်ပေး FALSE;
}
#endif

    void
ex_version(exarg_T *eap)
{
    /*
     * Ignore a ":version 9.99" command.
     */
    အကယ်၍ (*eap->arg == NUL)
    {
	msg_putchar('\n');
	list_version();
    }
}

/*
 * Output a string for the version message.  If it's going to wrap, output a
 * newline, unless the message is too long to fit on the screen anyway.
 * When "wrap" is TRUE wrap the string in [].
 */
    static void
version_msg_wrap(char_u *s, ကိန်းပြည့် wrap)
{
    ကိန်းပြည့်		len = vim_strsize(s) + (wrap ? 2 : 0);

    အကယ်၍ (!got_int && len < cmdline_width && msg_col + len >= cmdline_width
								&& *s != '\n')
	msg_putchar('\n');
    အကယ်၍ (!got_int)
    {
	အကယ်၍ (wrap)
	    msg_puts("[");
	msg_puts((var_na *)s);
	အကယ်၍ (wrap)
	    msg_puts("]");
    }
}

    static void
version_msg(var_na *s)
{
    version_msg_wrap((char_u *)s, FALSE);
}

/*
 * List all features aligned in columns, dictionary style.
 */
    static void
list_features(void)
{
    list_in_columns((char_u **)features, -1, -1);
}

/*
 * List string items nicely aligned in columns.
 * When "size" is < 0 then the last entry is marked with NULL.
 * The entry with index "current" is inclosed in [].
 */
    void
list_in_columns(char_u **items, ကိန်းပြည့် size, ကိန်းပြည့် current)
{
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		ncol;
    ကိန်းပြည့်		nrow;
    ကိန်းပြည့်		cur_row = 1;
    ကိန်းပြည့်		item_count = 0;
    ကိန်းပြည့်		width = 0;
#ifdef FEAT_SYN_HL
    ကိန်းပြည့်		use_highlight = (items == (char_u **)features);
#endif

    // Find the length of the longest item, use that + 1 as the column
    // width.
    အတွက် (i = 0; size < 0 ? items[i] != NULL : i < size; ++i)
    {
	ကိန်းပြည့် l = vim_strsize(items[i]) + (i == current ? 2 : 0);

	အကယ်၍ (l > width)
	    width = l;
	++item_count;
    }
    width += 1;

    အကယ်၍ (cmdline_width < width)
    {
	// Not enough screen columns - show one per line
	အတွက် (i = 0; i < item_count; ++i)
	{
	    version_msg_wrap(items[i], i == current);
	    အကယ်၍ (msg_col > 0 && i < item_count - 1)
		msg_putchar('\n');
	}
	ပြန်ပေး;
    }

    // The rightmost column doesn't need a separator.
    // Sacrifice it to fit in one more column if possible.
    ncol = (cmdline_width + 1) / width;
    nrow = item_count / ncol + ((item_count % ncol) ? 1 : 0);

    // "i" counts columns then rows.  "idx" counts rows then columns.
    အတွက် (i = 0; !got_int && i < nrow * ncol; ++i)
    {
	ကိန်းပြည့် idx = (i / ncol) + (i % ncol) * nrow;

	အကယ်၍ (idx < item_count)
	{
	    ကိန်းပြည့် last_col = (i + 1) % ncol == 0;

	    အကယ်၍ (idx == current)
		msg_putchar('[');
#ifdef FEAT_SYN_HL
	    အကယ်၍ (use_highlight && items[idx][0] == '-')
		msg_puts_attr((var_na *)items[idx], HL_ATTR(HLF_W));
	    မဟုတ်ပါက
#endif
		msg_puts((var_na *)items[idx]);
	    အကယ်၍ (idx == current)
		msg_putchar(']');
	    အကယ်၍ (last_col)
	    {
		အကယ်၍ (msg_col > 0 && cur_row < nrow)
		    msg_putchar('\n');
		++cur_row;
	    }
	    မဟုတ်ပါက
	    {
		နေစဉ် (msg_col % width)
		    msg_putchar(' ');
	    }
	}
	မဟုတ်ပါက
	{
	    // this row is out of items, thus at the end of the row
	    အကယ်၍ (msg_col > 0)
	    {
		အကယ်၍ (cur_row < nrow)
		    msg_putchar('\n');
		++cur_row;
	    }
	}
    }
}

    void
list_version(void)
{
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		first;
    var_na	*s = "";

    /*
     * When adding features here, don't forget to update the list of
     * internal variables in eval.c!
     */
    init_longVersion();
    msg(longVersion);
#ifdef MSWIN
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
#   ifdef _WIN64
#    အကယ်၍ defined(_M_ARM64) || defined(_M_ARM64EC)
     msg_puts(_("\nMS-Windows ARM64 GUI/console version"));
#    မဟုတ်ပါက
     msg_puts(_("\nMS-Windows 64-bit GUI/console version"));
#    endif
#   မဟုတ်ပါက
    msg_puts(_("\nMS-Windows 32-bit GUI/console version"));
#   endif
#  မဟုတ်ပါက
#   ifdef _WIN64
#    အကယ်၍ defined(_M_ARM64) || defined(_M_ARM64EC)
     msg_puts(_("\nMS-Windows ARM64 GUI version"));
#    မဟုတ်ပါက
     msg_puts(_("\nMS-Windows 64-bit GUI version"));
#    endif
#   မဟုတ်ပါက
    msg_puts(_("\nMS-Windows 32-bit GUI version"));
#   endif
#  endif
#  ifdef FEAT_OLE
    msg_puts(_(" with OLE support"));
#  endif
# မဟုတ်ပါက
#  ifdef _WIN64
#   အကယ်၍ defined(_M_ARM64) || defined(_M_ARM64EC)
    msg_puts(_("\nMS-Windows ARM64 console version"));
#   မဟုတ်ပါက
    msg_puts(_("\nMS-Windows 64-bit console version"));
#   endif
#  မဟုတ်ပါက
    msg_puts(_("\nMS-Windows 32-bit console version"));
#  endif
# endif
#endif
#အကယ်၍ defined(MACOS_X)
# အကယ်၍ defined(MACOS_X_DARWIN)
    msg_puts(_("\nmacOS version"));
# မဟုတ်ပါက
    msg_puts(_("\nmacOS version w/o darwin feat."));
# endif
# အကယ်၍ defined(__arm64__)
    msg_puts(" - arm64");
# elif defined(__x86_64__)
    msg_puts(" - x86_64");
# endif
#endif

#ifdef VMS
    msg_puts(_("\nOpenVMS (build) arch, version"));
# ifdef HAVE_PATHDEF
    အကယ်၍ (*compiled_arch != NUL)
    {
	msg_puts(": ");
	msg_puts((var_na *)compiled_arch);
	အကယ်၍ (*compiled_vers != NUL)
	{
	    msg_puts(", ");
	    msg_puts((var_na *)compiled_vers);
	}
    }

# endif

#endif

    // Print the list of patch numbers if there is at least one.
    // Print a range when patches are consecutive: "1-10, 12, 15-40, 42-45"
    အကယ်၍ (included_patches[0] != 0)
    {
	msg_puts(_("\nIncluded patches: "));
	first = -1;
	i = (ကိန်းပြည့်)ARRAY_LENGTH(included_patches) - 1;
	နေစဉ် (--i >= 0)
	{
	    အကယ်၍ (first < 0)
		first = included_patches[i];
	    အကယ်၍ (i == 0 || included_patches[i - 1] != included_patches[i] + 1)
	    {
		msg_puts(s);
		s = ", ";
		msg_outnum((long)first);
		အကယ်၍ (first != included_patches[i])
		{
		    msg_puts("-");
		    msg_outnum((long)included_patches[i]);
		}
		first = -1;
	    }
	}
    }

    // Print the list of extra patch descriptions if there is at least one.
    အကယ်၍ (extra_patches[0] != NULL)
    {
	msg_puts(_("\nExtra patches: "));
	s = "";
	အတွက် (i = 0; extra_patches[i] != NULL; ++i)
	{
	    msg_puts(s);
	    s = ", ";
	    msg_puts(extra_patches[i]);
	}
    }

#ifdef MODIFIED_BY
    msg_puts("\n");
    msg_puts(_("Modified by "));
    msg_puts(MODIFIED_BY);
#endif

#ifdef HAVE_PATHDEF
    အကယ်၍ (*compiled_user != NUL || *compiled_sys != NUL)
    {
	msg_puts(_("\nCompiled "));
	အကယ်၍ (*compiled_user != NUL)
	{
	    msg_puts(_("by "));
	    msg_puts((var_na *)compiled_user);
	}
	အကယ်၍ (*compiled_sys != NUL)
	{
	    msg_puts("@");
	    msg_puts((var_na *)compiled_sys);
	}
    }
#endif

#အကယ်၍ defined(FEAT_HUGE)
    msg_puts(_("\nHuge version "));
#elif defined(FEAT_NORMAL)
    msg_puts(_("\nNormal version "));
#မဟုတ်ပါက
    msg_puts(_("\nTiny version "));
#endif
#အကယ်၍ !defined(FEAT_GUI)
    msg_puts(_("without GUI."));
#elif defined(FEAT_GUI_GTK)
# အကယ်၍ defined(USE_GTK4)
    msg_puts(_("with GTK4 GUI."));
# elif defined(USE_GTK3)
    msg_puts(_("with GTK3 GUI."));
# elif defined(FEAT_GUI_GNOME)
    msg_puts(_("with GTK2-GNOME GUI."));
# မဟုတ်ပါက
    msg_puts(_("with GTK2 GUI."));
# endif
#elif defined(FEAT_GUI_MOTIF)
    msg_puts(_("with X11-Motif GUI."));
#elif defined(FEAT_GUI_HAIKU)
    msg_puts(_("with Haiku GUI."));
#elif defined(FEAT_GUI_PHOTON)
    msg_puts(_("with Photon GUI."));
#elif defined(MSWIN)
    msg_puts(_("with GUI."));
#endif
    version_msg(_("  Features included (+) or not (-):\n"));

    list_features();
    အကယ်၍ (msg_col > 0)
	msg_putchar('\n');

#ifdef SYS_VIMRC_FILE
    version_msg(_("   system vimrc file: \""));
    version_msg(SYS_VIMRC_FILE);
    version_msg("\"\n");
#endif
#ifdef USR_VIMRC_FILE
    version_msg(_("     user vimrc file: \""));
    version_msg(USR_VIMRC_FILE);
    version_msg("\"\n");
#endif
#ifdef USR_VIMRC_FILE2
    version_msg(_(" 2nd user vimrc file: \""));
    version_msg(USR_VIMRC_FILE2);
    version_msg("\"\n");
#endif
#အကယ်၍ defined(USR_VIMRC_FILE3) && defined(XDG_VIMRC_FILE)
    version_msg(_(" 3rd user vimrc file: \""));
    version_msg(USR_VIMRC_FILE3);
    version_msg("\"\n");
    version_msg(_(" 4th user vimrc file: \""));
    version_msg((var_na *)(XDG_VIMRC_FILE));
    version_msg("\"\n");
#elif defined(USR_VIMRC_FILE3)
    version_msg(_(" 3rd user vimrc file: \""));
    version_msg(USR_VIMRC_FILE3);
    version_msg("\"\n");
#elif defined(XDG_VIMRC_FILE)
    version_msg(_(" 3rd user vimrc file: \""));
    version_msg((var_na *)(XDG_VIMRC_FILE));
    version_msg("\"\n");
#endif
#ifdef USR_EXRC_FILE
    version_msg(_("      user exrc file: \""));
    version_msg(USR_EXRC_FILE);
    version_msg("\"\n");
#endif
#ifdef USR_EXRC_FILE2
    version_msg(_("  2nd user exrc file: \""));
    version_msg(USR_EXRC_FILE2);
    version_msg("\"\n");
#endif
#ifdef FEAT_GUI
# ifdef SYS_GVIMRC_FILE
    version_msg(_("  system gvimrc file: \""));
    version_msg(SYS_GVIMRC_FILE);
    version_msg("\"\n");
# endif
    version_msg(_("    user gvimrc file: \""));
    version_msg(USR_GVIMRC_FILE);
    version_msg("\"\n");
# ifdef USR_GVIMRC_FILE2
    version_msg(_("2nd user gvimrc file: \""));
    version_msg(USR_GVIMRC_FILE2);
    version_msg("\"\n");
# endif
# ifdef USR_GVIMRC_FILE3
    version_msg(_("3rd user gvimrc file: \""));
    version_msg(USR_GVIMRC_FILE3);
    version_msg("\"\n");
# endif
#endif
    version_msg(_("       defaults file: \""));
    version_msg(VIM_DEFAULTS_FILE);
    version_msg("\"\n");
#ifdef FEAT_GUI
# ifdef SYS_MENU_FILE
    version_msg(_("    system menu file: \""));
    version_msg(SYS_MENU_FILE);
    version_msg("\"\n");
# endif
#endif
#ifdef HAVE_PATHDEF
    အကယ်၍ (*default_vim_dir != NUL)
    {
	version_msg(_("  fall-back for $VIM: \""));
	version_msg((var_na *)default_vim_dir);
	version_msg("\"\n");
    }
    အကယ်၍ (*default_vimruntime_dir != NUL)
    {
	version_msg(_(" f-b for $VIMRUNTIME: \""));
	version_msg((var_na *)default_vimruntime_dir);
	version_msg("\"\n");
    }
    version_msg(_("Compilation: "));
    version_msg((var_na *)all_cflags);
    version_msg("\n");
# ifdef VMS
    အကယ်၍ (*compiler_version != NUL)
    {
	version_msg(_("Compiler: "));
	version_msg((var_na *)compiler_version);
	version_msg("\n");
    }
# endif
    version_msg(_("Linking: "));
    version_msg((var_na *)all_lflags);
#endif
#ifdef DEBUG
    version_msg("\n");
    version_msg(_("  DEBUG BUILD"));
#endif
}

static void do_intro_line(ကိန်းပြည့် row, char_u *mesg, ကိန်းပြည့် add_version, ကိန်းပြည့် attr);
static void intro_message(ကိန်းပြည့် colon);

/*
 * Show the intro message when not editing a file.
 */
    void
maybe_intro_message(void)
{
    အကယ်၍ (BUFEMPTY()
	    && curbuf->b_fname == NULL
	    && firstwin->w_next == NULL
	    && vim_strchr(p_shm, SHM_INTRO) == NULL)
	intro_message(FALSE);
}

/*
 * Give an introductory message about Vim.
 * Only used when starting Vim on an empty file, without a file name.
 * Or with the ":intro" command (for Sven :-).
 */
    static void
intro_message(
    ကိန်းပြည့်		colon)		// TRUE for ":intro"
{
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		row;
    ကိန်းပြည့်		blanklines;
    ကိန်းပြည့်		sponsor;
    var_na	*p;
    static var_na	*(lines[]) =
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
	NULL,
	"",
	N_("Running in Vi compatible mode"),
	N_("type  :set nocp<Enter>        for Vim defaults"),
	N_("type  :help cp-default<Enter> for info on this"),
    };
#ifdef FEAT_GUI
    static var_na	*(gui_lines[]) =
    {
	NULL,
	NULL,
	NULL,
	NULL,
# ifdef MODIFIED_BY
	NULL,
# endif
	NULL,
	NULL,
	NULL,
	N_("menu  Help->Orphans           for information    "),
	NULL,
	N_("Running modeless, typed text is inserted"),
	N_("menu  Edit->Global Settings->Toggle Insert Mode  "),
	N_("                              for two modes      "),
	NULL,
	NULL,
	NULL,
	N_("menu  Edit->Global Settings->Toggle Vi Compatible"),
	N_("                              for Vim defaults   "),
    };
#endif

    // blanklines = screen height - # message lines
    blanklines = (ကိန်းပြည့်)Rows - (ARRAY_LENGTH(lines) - 1);
    အကယ်၍ (!p_cp)
	blanklines += 4;  // add 4 for not showing "Vi compatible" message

    // Don't overwrite a statusline.  Depends on 'cmdheight'.
    အကယ်၍ (p_ls > 1)
	blanklines -= Rows - topframe->fr_height;
    အကယ်၍ (blanklines < 0)
	blanklines = 0;

    // Show the sponsor and Uganda message two out of four times
    sponsor = (ကိန်းပြည့်)time(NULL);
    sponsor = ((sponsor & 2) == 0) - ((sponsor & 4) == 0);

    // start displaying the message lines after half of the blank lines
    row = blanklines / 2;
    အကယ်၍ ((row >= 2 && topframe->fr_width >= 50) || colon)
    {
	အတွက် (i = 0; i < (ကိန်းပြည့်)ARRAY_LENGTH(lines); ++i)
	{
	    p = lines[i];
#ifdef FEAT_GUI
	    အကယ်၍ (p_im && gui.in_use && gui_lines[i] != NULL)
		p = gui_lines[i];
#endif
	    အကယ်၍ (p == NULL)
	    {
		အကယ်၍ (!p_cp)
		    bha_nga;
		continue;
	    }
	    အကယ်၍ (sponsor != 0)
	    {
		အကယ်၍ (strstr(p, "children") != NULL)
		    p = N_("Sponsor Vim development!");
		မဟုတ်ပါက အကယ်၍ (strstr(p, "Kuwasha") != NULL)
		    p = N_("type  :help sponsor<Enter>    for information ");
		မဟုတ်ပါက အကယ်၍ (strstr(p, "Orphans") != NULL)
		    p = N_("menu  Help->Sponsor           for information    ");
	    }
	    အကယ်၍ (*p != NUL)
		do_intro_line(row, (char_u *)_(p), i == 2, 0);
	    ++row;
	}
    }

    // Make the wait-return message appear just below the text.
    အကယ်၍ (colon)
	msg_row = row;
}

    static void
do_intro_line(
    ကိန်းပြည့်		row,
    char_u	*mesg,
    ကိန်းပြည့်		add_version,
    ကိန်းပြည့်		attr)
{
    char_u	vers[20];
    ကိန်းပြည့်		col;
    char_u	*p;
    ကိန်းပြည့်		l;
    ကိန်းပြည့်		clen;
#ifdef MODIFIED_BY
# define MODBY_LEN 150
    char_u	modby[MODBY_LEN];

    အကယ်၍ (*mesg == ' ')
    {
	vim_strncpy(modby, (char_u *)_("Modified by "), MODBY_LEN - 1);
	l = (ကိန်းပြည့်)STRLEN(modby);
	vim_strncpy(modby + l, (char_u *)MODIFIED_BY, MODBY_LEN - l - 1);
	mesg = modby;
    }
#endif

    // Center the message horizontally.
    col = vim_strsize(mesg);
    အကယ်၍ (add_version)
    {
	STRCPY(vers, mediumVersion);
	အကယ်၍ (highest_patch())
	{
	    // Check for 9.9x or 9.9xx, alpha/beta version
	    အကယ်၍ (SAFE_isalpha((ကိန်းပြည့်)vers[3]))
	    {
		ကိန်းပြည့် len = (SAFE_isalpha((ကိန်းပြည့်)vers[4])) ? 5 : 4;
		sprintf((var_na *)vers + len, ".%d%s", highest_patch(),
							 mediumVersion + len);
	    }
	    မဟုတ်ပါက
		sprintf((var_na *)vers + 3, ".%d", highest_patch());
	}
	col += (ကိန်းပြည့်)STRLEN(vers);
    }
    col = (topframe->fr_width - col) / 2;
    အကယ်၍ (col < 0)
	col = 0;

    // Split up in parts to highlight <> items differently.
    အတွက် (p = mesg; *p != NUL; p += l)
    {
	clen = 0;
	အတွက် (l = 0; p[l] != NUL
			 && (l == 0 || (p[l] != '<' && p[l - 1] != '>')); ++l)
	{
	    အကယ်၍ (has_mbyte)
	    {
		clen += ptr2cells(p + l);
		l += (*mb_ptr2len)(p + l) - 1;
	    }
	    မဟုတ်ပါက
		clen += byte2cells(p[l]);
	}
	screen_puts_len(p, l, row, col + firstwin->w_wincol,
		*p == '<' ? HL_ATTR(HLF_8) : attr);
	col += clen;
    }

    // Add the version number to the version line.
    အကယ်၍ (add_version)
	screen_puts(vers, row, col + firstwin->w_wincol, 0);
}

/*
 * ":intro": clear screen, display intro screen and wait for return.
 */
    void
ex_intro(exarg_T *eap UNUSED)
{
    screenclear();
#အကယ်၍ defined(FEAT_TABPANEL)
    draw_tabpanel();
#endif
    intro_message(TRUE);
    wait_return(TRUE);
}
