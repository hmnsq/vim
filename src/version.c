/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ଅନ୍ତର୍ଭାଵ "vim.h"

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

#ଅନ୍ତର୍ଭାଵ "version.h"

ଅକ୍ଷର		*Version = VIM_VERSION_SHORT;
ସ୍ଥିର ଅକ୍ଷର	*mediumVersion = VIM_VERSION_MEDIUM;

#ଯଦି defined(HAVE_DATE_TIME)
# ଯଦି defined(VMS) && defined(VAXC)
ଅକ୍ଷର	longVersion[ଆକାର(VIM_VERSION_LONG_DATE) + ଆକାର(__DATE__)
						      + ଆକାର(__TIME__) + 3];

    ଶୂନ୍ୟ
init_longVersion(ଶୂନ୍ୟ)
{
    /*
     * Construct the long version string.  Necessary because
     * VAX C can't concatenate strings in the preprocessor.
     */
    strcpy(longVersion, VIM_VERSION_LONG_DATE);
#  ifdef BUILD_DATE
    strcat(longVersion, BUILD_DATE);
#  ଅନ୍ୟଥା
    strcat(longVersion, __DATE__);
    strcat(longVersion, " ");
    strcat(longVersion, __TIME__);
#  endif
    strcat(longVersion, ")");
}

# ଅନ୍ୟଥା
ଅକ୍ଷର	*longVersion = ଖାଲି;

    ଶୂନ୍ୟ
init_longVersion(ଶୂନ୍ୟ)
{
    ଯଦି (longVersion != ଖାଲି)
	ଫେରନ୍ତୁ;

#  ifdef BUILD_DATE
    ଅକ୍ଷର *date_time = BUILD_DATE;
#  ଅନ୍ୟଥା
    ଅକ୍ଷର *date_time = __DATE__ " " __TIME__;
#  endif
    ଅକ୍ଷର *ସନ୍ଦେଶ = _("%s (%s, compiled %s)");
    size_t len = strlen(ସନ୍ଦେଶ)
	+ STRLEN_LITERAL(VIM_VERSION_LONG_ONLY)
	+ STRLEN_LITERAL(VIM_VERSION_DATE_ONLY)
	+ strlen(date_time);

    longVersion = alloc(len);
    ଯଦି (longVersion == ଖାଲି)
	longVersion = VIM_VERSION_LONG;
    ଅନ୍ୟଥା
	vim_snprintf(longVersion, len, ସନ୍ଦେଶ,
		VIM_VERSION_LONG_ONLY, VIM_VERSION_DATE_ONLY, date_time);
}
# endif
#ଅନ୍ୟଥା
ଅକ୍ଷର	*longVersion = VIM_VERSION_LONG;

    ଶୂନ୍ୟ
init_longVersion(ଶୂନ୍ୟ)
{
    // nothing to do
}
#endif

ସ୍ଥିର ଅକ୍ଷର *(features[]) =
{
#ifdef HAVE_ACL
	"+acl",
#ଅନ୍ୟଥା
	"-acl",
#endif
#ifdef FEAT_ARABIC
	"+arabic",
#ଅନ୍ୟଥା
	"-arabic",
#endif
#ifdef AMIGA		// only for Amiga systems
# ifdef FEAT_ARP
	"+ARP",
# ଅନ୍ୟଥା
	"-ARP",
# endif
#endif
#ifdef FEAT_AUTOCHDIR
	"+autochdir",
#ଅନ୍ୟଥା
	"-autochdir",
#endif
	"+autocmd",
#ifdef FEAT_AUTOSERVERNAME
	"+autoservername",
#ଅନ୍ୟଥା
	"-autoservername",
#endif
#ifdef FEAT_BEVAL_GUI
	"+balloon_eval",
#ଅନ୍ୟଥା
	"-balloon_eval",
#endif
#ifdef FEAT_BEVAL_TERM
	"+balloon_eval_term",
#ଅନ୍ୟଥା
	"-balloon_eval_term",
#endif
#ifdef FEAT_BROWSE
	"+browse",
#ଅନ୍ୟଥା
	"-browse",
#endif
	"++builtin_terms",
#ifdef FEAT_BYTEOFF
	"+byte_offset",
#ଅନ୍ୟଥା
	"-byte_offset",
#endif
#ifdef FEAT_JOB_CHANNEL
	"+channel",
#ଅନ୍ୟଥା
	"-channel",
#endif
	"+cindent",
#ifdef FEAT_CLIENTSERVER
	"+clientserver",
#ଅନ୍ୟଥା
	"-clientserver",
#endif
#ifdef FEAT_CLIPBOARD
	"+clipboard",
#ଅନ୍ୟଥା
	"-clipboard",
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	"+clipboard_provider",
#ଅନ୍ୟଥା
	"-clipboard_provider",
#endif
	"+cmdline_compl",
	"+cmdline_hist",
	"+cmdline_info",
	"+comments",
#ifdef FEAT_CONCEAL
	"+conceal",
#ଅନ୍ୟଥା
	"-conceal",
#endif
#ifdef FEAT_CRYPT
	"+cryptv",
#ଅନ୍ୟଥା
	"-cryptv",
#endif
#ifdef FEAT_CSCOPE
	"+cscope",
#ଅନ୍ୟଥା
	"-cscope",
#endif
	"+cursorbind",
#ifdef CURSOR_SHAPE
	"+cursorshape",
#ଅନ୍ୟଥା
	"-cursorshape",
#endif
#ଯଦି defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
	"+dialog_con_gui",
#ଅନ୍ୟଥା
# ଯଦି defined(FEAT_CON_DIALOG)
	"+dialog_con",
# ଅନ୍ୟଥା
#  ଯଦି defined(FEAT_GUI_DIALOG)
	"+dialog_gui",
#  ଅନ୍ୟଥା
	"-dialog",
#  endif
# endif
#endif
#ifdef FEAT_DIFF
	"+diff",
#ଅନ୍ୟଥା
	"-diff",
#endif
#ifdef FEAT_DIGRAPHS
	"+digraphs",
#ଅନ୍ୟଥା
	"-digraphs",
#endif
#ifdef FEAT_GUI_MSWIN
# ifdef FEAT_DIRECTX
	"+directx",
# ଅନ୍ୟଥା
	"-directx",
# endif
#endif
#ifdef FEAT_DND
	"+dnd",
#ଅନ୍ୟଥା
	"-dnd",
#endif
	"-ebcdic",
#ifdef FEAT_EMACS_TAGS
	"+emacs_tags",
#ଅନ୍ୟଥା
	"-emacs_tags",
#endif
#ifdef FEAT_EVAL
	"+eval",
#ଅନ୍ୟଥା
	"-eval",
#endif
	"+ex_extra",
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	"+extra_search",
#ଅନ୍ୟଥା
	"-extra_search",
#endif
	"-farsi",
	"+file_in_path",
#ifdef FEAT_FIND_ID
	"+find_in_path",
#ଅନ୍ୟଥା
	"-find_in_path",
#endif
	"+float",
#ifdef FEAT_FOLDING
	"+folding",
#ଅନ୍ୟଥା
	"-folding",
#endif
	"-footer",
	// only interesting on Unix systems
#ଯଦି !defined(USE_SYSTEM) && defined(UNIX)
	"+fork()",
#endif
#ifdef FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
	"+gettext/dyn",
# ଅନ୍ୟଥା
	"+gettext",
# endif
#ଅନ୍ୟଥା
	"-gettext",
#endif
	"-hangul_input",
#ଯଦି (defined(HAVE_ICONV_H) && defined(USE_ICONV)) || defined(DYNAMIC_ICONV)
# ifdef DYNAMIC_ICONV
	"+iconv/dyn",
# ଅନ୍ୟଥା
	"+iconv",
# endif
#ଅନ୍ୟଥା
	"-iconv",
#endif
#ifdef FEAT_IMAGE
	"+image",
#ଅନ୍ୟଥା
	"-image",
#endif
#ifdef FEAT_IMAGE_POPUP
	"+image_popup",
#ଅନ୍ୟଥା
	"-image_popup",
#endif
	"+insert_expand",
#ifdef FEAT_IPV6
	"+ipv6",
#ଅନ୍ୟଥା
	"-ipv6",
#endif
#ifdef FEAT_JOB_CHANNEL
	"+job",
#ଅନ୍ୟଥା
	"-job",
#endif
	"+jumplist",
#ifdef FEAT_KEYMAP
	"+keymap",
#ଅନ୍ୟଥା
	"-keymap",
#endif
#ifdef FEAT_EVAL
	"+lambda",
#ଅନ୍ୟଥା
	"-lambda",
#endif
#ifdef FEAT_LANGMAP
	"+langmap",
#ଅନ୍ୟଥା
	"-langmap",
#endif
#ifdef FEAT_LIBCALL
	"+libcall",
#ଅନ୍ୟଥା
	"-libcall",
#endif
#ifdef FEAT_LINEBREAK
	"+linebreak",
#ଅନ୍ୟଥା
	"-linebreak",
#endif
	"+lispindent",
	"+listcmds",
	"+localmap",
#ifdef FEAT_LUA
# ifdef DYNAMIC_LUA
	"+lua/dyn",
# ଅନ୍ୟଥା
	"+lua",
# endif
#ଅନ୍ୟଥା
	"-lua",
#endif
#ifdef FEAT_MENU
	"+menu",
#ଅନ୍ୟଥା
	"-menu",
#endif
#ifdef FEAT_SESSION
	"+mksession",
#ଅନ୍ୟଥା
	"-mksession",
#endif
	"+modify_fname",
	"+mouse",
#ଯଦି defined(UNIX) || defined(VMS)
# ifdef FEAT_MOUSE_DEC
	"+mouse_dec",
# ଅନ୍ୟଥା
	"-mouse_dec",
# endif
# ifdef FEAT_MOUSE_GPM
#  ifdef DYNAMIC_GPM
	"+mouse_gpm/dyn",
#  ଅନ୍ୟଥା
	"+mouse_gpm",
#  endif
# ଅନ୍ୟଥା
	"-mouse_gpm",
# endif
# ifdef FEAT_MOUSE_JSB
	"+mouse_jsbterm",
# ଅନ୍ୟଥା
	"-mouse_jsbterm",
# endif
# ifdef FEAT_MOUSE_NET
	"+mouse_netterm",
# ଅନ୍ୟଥା
	"-mouse_netterm",
# endif
#endif

#ifdef __QNX__
# ifdef FEAT_MOUSE_PTERM
	"+mouse_pterm",
# ଅନ୍ୟଥା
	"-mouse_pterm",
# endif
#endif

#ଯଦି defined(UNIX) || defined(VMS)
	"+mouse_sgr",
# ifdef FEAT_SYSMOUSE
	"+mouse_sysmouse",
# ଅନ୍ୟଥା
	"-mouse_sysmouse",
# endif
# ifdef FEAT_MOUSE_URXVT
	"+mouse_urxvt",
# ଅନ୍ୟଥା
	"-mouse_urxvt",
# endif
	"+mouse_xterm",
#endif
#ifdef FEAT_MOUSESHAPE
	"+mouseshape",
#ଅନ୍ୟଥା
	"-mouseshape",
#endif

#ifdef FEAT_MBYTE_IME
# ifdef DYNAMIC_IME
	"+multi_byte_ime/dyn",
# ଅନ୍ୟଥା
	"+multi_byte_ime",
# endif
#ଅନ୍ୟଥା
	"+multi_byte",
#endif
#ifdef FEAT_MULTI_LANG
	"+multi_lang",
#ଅନ୍ୟଥା
	"-multi_lang",
#endif
#ifdef FEAT_MZSCHEME
# ifdef DYNAMIC_MZSCHEME
	"+mzscheme/dyn",
# ଅନ୍ୟଥା
	"+mzscheme",
# endif
#ଅନ୍ୟଥା
	"-mzscheme",
#endif
#ifdef FEAT_NETBEANS_INTG
	"+netbeans_intg",
#ଅନ୍ୟଥା
	"-netbeans_intg",
#endif
	"+num64",
#ifdef FEAT_GUI_MSWIN
# ifdef FEAT_OLE
	"+ole",
# ଅନ୍ୟଥା
	"-ole",
# endif
#endif
#ifdef FEAT_EVAL
	"+packages",
#ଅନ୍ୟଥା
	"-packages",
#endif
#ifdef FEAT_PRINT_PANGO
	"+pango",
#ଅନ୍ୟଥା
	"-pango",
#endif
	"+path_extra",
#ifdef FEAT_PERL
# ifdef DYNAMIC_PERL
	"+perl/dyn",
# ଅନ୍ୟଥା
	"+perl",
# endif
#ଅନ୍ୟଥା
	"-perl",
#endif
#ifdef FEAT_PERSISTENT_UNDO
	"+persistent_undo",
#ଅନ୍ୟଥା
	"-persistent_undo",
#endif
#ifdef FEAT_PROP_POPUP
	"+popupwin",
#ଅନ୍ୟଥା
	"-popupwin",
#endif
#ifdef FEAT_PRINTER
# ifdef FEAT_POSTSCRIPT
	"+postscript",
# ଅନ୍ୟଥା
	"-postscript",
# endif
	"+printer",
#ଅନ୍ୟଥା
	"-printer",
#endif
#ifdef FEAT_PROFILE
	"+profile",
#ଅନ୍ୟଥା
	"-profile",
#endif
#ifdef FEAT_PYTHON
# ifdef DYNAMIC_PYTHON
	"+python/dyn",
# ଅନ୍ୟଥା
	"+python",
# endif
#ଅନ୍ୟଥା
	"-python",
#endif
#ifdef FEAT_PYTHON3
# ifdef DYNAMIC_PYTHON3
#  ifdef DYNAMIC_PYTHON3_STABLE_ABI
	"+python3/dyn-stable",
#  ଅନ୍ୟଥା
	"+python3/dyn",
#  endif
# ଅନ୍ୟଥା
	"+python3",
# endif
#ଅନ୍ୟଥା
	"-python3",
#endif
#ifdef FEAT_QUICKFIX
	"+quickfix",
#ଅନ୍ୟଥା
	"-quickfix",
#endif
#ifdef FEAT_RELTIME
	"+reltime",
#ଅନ୍ୟଥା
	"-reltime",
#endif
#ifdef FEAT_RIGHTLEFT
	"+rightleft",
#ଅନ୍ୟଥା
	"-rightleft",
#endif
#ifdef FEAT_RUBY
# ifdef DYNAMIC_RUBY
	"+ruby/dyn",
# ଅନ୍ୟଥା
	"+ruby",
# endif
#ଅନ୍ୟଥା
	"-ruby",
#endif
	"+scrollbind",
#ifdef FEAT_SIGNS
	"+signs",
#ଅନ୍ୟଥା
	"-signs",
#endif
	"+smartindent",
#ifdef FEAT_SOCKETSERVER
	"+socketserver",
#ଅନ୍ୟଥା
	"-socketserver",
#endif
#ifdef FEAT_SODIUM
# ifdef DYNAMIC_SODIUM
	"+sodium/dyn",
# ଅନ୍ୟଥା
	"+sodium",
# endif
#ଅନ୍ୟଥା
	"-sodium",
#endif
#ifdef FEAT_SOUND
	"+sound",
#ଅନ୍ୟଥା
	"-sound",
#endif
#ifdef FEAT_SPELL
	"+spell",
#ଅନ୍ୟଥା
	"-spell",
#endif
#ifdef STARTUPTIME
	"+startuptime",
#ଅନ୍ୟଥା
	"-startuptime",
#endif
#ifdef FEAT_STL_OPT
	"+statusline",
#ଅନ୍ୟଥା
	"-statusline",
#endif
	"-sun_workshop",
#ifdef FEAT_SYN_HL
	"+syntax",
#ଅନ୍ୟଥା
	"-syntax",
#endif
	    // only interesting on Unix systems
#ଯଦି defined(USE_SYSTEM) && defined(UNIX)
	"+system()",
#endif
#ଯଦି defined(FEAT_TABPANEL)
	"+tabpanel",
#ଅନ୍ୟଥା
	"-tabpanel",
#endif
	"-tag_any_white",
	"+tag_binary",
	"-tag_old_static",
#ifdef FEAT_TCL
# ifdef DYNAMIC_TCL
	"+tcl/dyn",
# ଅନ୍ୟଥା
	"+tcl",
# endif
#ଅନ୍ୟଥା
	"-tcl",
#endif
#ifdef FEAT_TERMGUICOLORS
	"+termguicolors",
#ଅନ୍ୟଥା
	"-termguicolors",
#endif
#ifdef FEAT_TERMINAL
	"+terminal",
#ଅନ୍ୟଥା
	"-terminal",
#endif
#ଯଦି defined(UNIX)
// only Unix can have terminfo instead of termcap
# ifdef TERMINFO
	"+terminfo",
# ଅନ୍ୟଥା
	"-terminfo",
# endif
#endif
#ifdef FEAT_TERMRESPONSE
	"+termresponse",
#ଅନ୍ୟଥା
	"-termresponse",
#endif
	"+textobjects",
#ifdef FEAT_PROP_POPUP
	"+textprop",
#ଅନ୍ୟଥା
	"-textprop",
#endif
#ଯଦି !defined(UNIX)
// unix always includes termcap support
# ifdef HAVE_TGETENT
	"+tgetent",
# ଅନ୍ୟଥା
	"-tgetent",
# endif
#endif
#ifdef FEAT_TIMERS
	"+timers",
#ଅନ୍ୟଥା
	"-timers",
#endif
	"+title",
#ifdef FEAT_TOOLBAR
	"+toolbar",
#ଅନ୍ୟଥା
	"-toolbar",
#endif
	"+user_commands",
#ifdef FEAT_VARTABS
	"+vartabs",
#ଅନ୍ୟଥା
	"-vartabs",
#endif
	"+vertsplit",
	"+vim9script",
#ifdef FEAT_VIMINFO
	"+viminfo",
#ଅନ୍ୟଥା
	"-viminfo",
#endif
	"+virtualedit",
	"+visual",
	"+visualextra",
	"+vreplace",
#ifdef MSWIN
# ifdef FEAT_VTP
	"+vtp",
# ଅନ୍ୟଥା
	"-vtp",
# endif
#endif
#ifdef FEAT_WAYLAND
	"+wayland",
#ଅନ୍ୟଥା
	"-wayland",
#endif
#ifdef FEAT_WAYLAND_CLIPBOARD
	"+wayland_clipboard",
#ଅନ୍ୟଥା
	"-wayland_clipboard",
#endif
	"+wildignore",
	"+wildmenu",
	"+windows",
#ifdef FEAT_WRITEBACKUP
	"+writebackup",
#ଅନ୍ୟଥା
	"-writebackup",
#endif
#ଯଦି defined(UNIX) || defined(VMS)
# ifdef FEAT_X11
	"+X11",
# ଅନ୍ୟଥା
	"-X11",
# endif
#endif
#ifdef FEAT_XATTR
	"+xattr",
#ଅନ୍ୟଥା
	"-xattr",
#endif
#ifdef FEAT_XFONTSET
	"+xfontset",
#ଅନ୍ୟଥା
	"-xfontset",
#endif
#ifdef FEAT_XIM
	"+xim",
#ଅନ୍ୟଥା
	"-xim",
#endif
#ଯଦି defined(MSWIN)
# ifdef FEAT_XPM_W32
	"+xpm_w32",
# ଅନ୍ୟଥା
	"-xpm_w32",
# endif
#ନଚେତଯଦି defined(HAVE_XPM)
	"+xpm",
#ଅନ୍ୟଥା
	"-xpm",
#endif
#ଯଦି defined(UNIX) || defined(VMS)
# ଯଦି defined(USE_XSMP_INTERACT)
	"+xsmp_interact",
# elif defined(USE_XSMP)
	"+xsmp",
# ଅନ୍ୟଥା
	"-xsmp",
# endif
# ifdef FEAT_XCLIPBOARD
	"+xterm_clipboard",
# ଅନ୍ୟଥା
	"-xterm_clipboard",
# endif
#endif
#ifdef FEAT_XTERM_SAVE
	"+xterm_save",
#ଅନ୍ୟଥା
	"-xterm_save",
#endif
	ଖାଲି
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା included_patches[] =
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
ସ୍ଥିର ଅକ୍ଷର *(extra_patches[]) =
{   /* Add your patch description below this line */
/**/
    ଖାଲି
};

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highest_patch(ଶୂନ୍ୟ)
{
    // this relies on the highest patch number to be the first entry
    ଫେରନ୍ତୁ included_patches[0];
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Return TRUE if patch "n" has been included.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_patch(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		h, m, l;

    // Perform a binary search.
    l = 0;
    h = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(included_patches) - 1;
    ପାଇଁ (;;)
    {
	m = (l + h) / 2;
	ଯଦି (included_patches[m] == n)
	    ଫେରନ୍ତୁ TRUE;
	ଯଦି (l == h)
	    ଭାଙ୍ଗ;
	ଯଦି (included_patches[m] < n)
	    h = m;
	ଅନ୍ୟଥା
	    l = m + 1;
    }
    ଫେରନ୍ତୁ FALSE;
}
#endif

    ଶୂନ୍ୟ
ex_version(exarg_T *eap)
{
    /*
     * Ignore a ":version 9.99" command.
     */
    ଯଦି (*eap->arg == NUL)
    {
	ସନ୍ଦେଶ_putchar('\n');
	list_version();
    }
}

/*
 * Output a string for the version message.  If it's going to wrap, output a
 * newline, unless the message is too long to fit on the screen anyway.
 * When "wrap" is TRUE wrap the string in [].
 */
    ସ୍ଥିର ଶୂନ୍ୟ
version_ସନ୍ଦେଶ_wrap(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା wrap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = vim_strsize(s) + (wrap ? 2 : 0);

    ଯଦି (!got_int && len < cmdline_width && ସନ୍ଦେଶ_col + len >= cmdline_width
								&& *s != '\n')
	ସନ୍ଦେଶ_putchar('\n');
    ଯଦି (!got_int)
    {
	ଯଦି (wrap)
	    ସନ୍ଦେଶ_puts("[");
	ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)s);
	ଯଦି (wrap)
	    ସନ୍ଦେଶ_puts("]");
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
version_ସନ୍ଦେଶ(ଅକ୍ଷର *s)
{
    version_ସନ୍ଦେଶ_wrap((char_u *)s, FALSE);
}

/*
 * List all features aligned in columns, dictionary style.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_features(ଶୂନ୍ୟ)
{
    list_in_columns((char_u **)features, -1, -1);
}

/*
 * List string items nicely aligned in columns.
 * When "size" is < 0 then the last entry is marked with NULL.
 * The entry with index "current" is inclosed in [].
 */
    ଶୂନ୍ୟ
list_in_columns(char_u **ବସ୍ତୁଗୁଡ଼ିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା current)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ncol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cur_row = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		item_ଗଣନା = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width = 0;
#ifdef FEAT_SYN_HL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_highlight = (ବସ୍ତୁଗୁଡ଼ିକ == (char_u **)features);
#endif

    // Find the length of the longest item, use that + 1 as the column
    // width.
    ପାଇଁ (i = 0; size < 0 ? ବସ୍ତୁଗୁଡ଼ିକ[i] != ଖାଲି : i < size; ++i)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = vim_strsize(ବସ୍ତୁଗୁଡ଼ିକ[i]) + (i == current ? 2 : 0);

	ଯଦି (l > width)
	    width = l;
	++item_ଗଣନା;
    }
    width += 1;

    ଯଦି (cmdline_width < width)
    {
	// Not enough screen columns - show one per line
	ପାଇଁ (i = 0; i < item_ଗଣନା; ++i)
	{
	    version_ସନ୍ଦେଶ_wrap(ବସ୍ତୁଗୁଡ଼ିକ[i], i == current);
	    ଯଦି (ସନ୍ଦେଶ_col > 0 && i < item_ଗଣନା - 1)
		ସନ୍ଦେଶ_putchar('\n');
	}
	ଫେରନ୍ତୁ;
    }

    // The rightmost column doesn't need a separator.
    // Sacrifice it to fit in one more column if possible.
    ncol = (cmdline_width + 1) / width;
    nrow = item_ଗଣନା / ncol + ((item_ଗଣନା % ncol) ? 1 : 0);

    // "i" counts columns then rows.  "idx" counts rows then columns.
    ପାଇଁ (i = 0; !got_int && i < nrow * ncol; ++i)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = (i / ncol) + (i % ncol) * nrow;

	ଯଦି (idx < item_ଗଣନା)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_col = (i + 1) % ncol == 0;

	    ଯଦି (idx == current)
		ସନ୍ଦେଶ_putchar('[');
#ifdef FEAT_SYN_HL
	    ଯଦି (use_highlight && ବସ୍ତୁଗୁଡ଼ିକ[idx][0] == '-')
		ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)ବସ୍ତୁଗୁଡ଼ିକ[idx], HL_ATTR(HLF_W));
	    ଅନ୍ୟଥା
#endif
		ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)ବସ୍ତୁଗୁଡ଼ିକ[idx]);
	    ଯଦି (idx == current)
		ସନ୍ଦେଶ_putchar(']');
	    ଯଦି (last_col)
	    {
		ଯଦି (ସନ୍ଦେଶ_col > 0 && cur_row < nrow)
		    ସନ୍ଦେଶ_putchar('\n');
		++cur_row;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯେତେବେଳେ (ସନ୍ଦେଶ_col % width)
		    ସନ୍ଦେଶ_putchar(' ');
	    }
	}
	ଅନ୍ୟଥା
	{
	    // this row is out of items, thus at the end of the row
	    ଯଦି (ସନ୍ଦେଶ_col > 0)
	    {
		ଯଦି (cur_row < nrow)
		    ସନ୍ଦେଶ_putchar('\n');
		++cur_row;
	    }
	}
    }
}

    ଶୂନ୍ୟ
list_version(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first;
    ଅକ୍ଷର	*s = "";

    /*
     * When adding features here, don't forget to update the list of
     * internal variables in eval.c!
     */
    init_longVersion();
    ସନ୍ଦେଶ(longVersion);
#ifdef MSWIN
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
#   ifdef _WIN64
#    ଯଦି defined(_M_ARM64) || defined(_M_ARM64EC)
     ସନ୍ଦେଶ_puts(_("\nMS-Windows ARM64 GUI/console version"));
#    ଅନ୍ୟଥା
     ସନ୍ଦେଶ_puts(_("\nMS-Windows 64-bit GUI/console version"));
#    endif
#   ଅନ୍ୟଥା
    ସନ୍ଦେଶ_puts(_("\nMS-Windows 32-bit GUI/console version"));
#   endif
#  ଅନ୍ୟଥା
#   ifdef _WIN64
#    ଯଦି defined(_M_ARM64) || defined(_M_ARM64EC)
     ସନ୍ଦେଶ_puts(_("\nMS-Windows ARM64 GUI version"));
#    ଅନ୍ୟଥା
     ସନ୍ଦେଶ_puts(_("\nMS-Windows 64-bit GUI version"));
#    endif
#   ଅନ୍ୟଥା
    ସନ୍ଦେଶ_puts(_("\nMS-Windows 32-bit GUI version"));
#   endif
#  endif
#  ifdef FEAT_OLE
    ସନ୍ଦେଶ_puts(_(" with OLE support"));
#  endif
# ଅନ୍ୟଥା
#  ifdef _WIN64
#   ଯଦି defined(_M_ARM64) || defined(_M_ARM64EC)
    ସନ୍ଦେଶ_puts(_("\nMS-Windows ARM64 console version"));
#   ଅନ୍ୟଥା
    ସନ୍ଦେଶ_puts(_("\nMS-Windows 64-bit console version"));
#   endif
#  ଅନ୍ୟଥା
    ସନ୍ଦେଶ_puts(_("\nMS-Windows 32-bit console version"));
#  endif
# endif
#endif
#ଯଦି defined(MACOS_X)
# ଯଦି defined(MACOS_X_DARWIN)
    ସନ୍ଦେଶ_puts(_("\nmacOS version"));
# ଅନ୍ୟଥା
    ସନ୍ଦେଶ_puts(_("\nmacOS version w/o darwin feat."));
# endif
# ଯଦି defined(__arm64__)
    ସନ୍ଦେଶ_puts(" - arm64");
# elif defined(__x86_64__)
    ସନ୍ଦେଶ_puts(" - x86_64");
# endif
#endif

#ifdef VMS
    ସନ୍ଦେଶ_puts(_("\nOpenVMS (build) arch, version"));
# ifdef HAVE_PATHDEF
    ଯଦି (*compiled_arch != NUL)
    {
	ସନ୍ଦେଶ_puts(": ");
	ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)compiled_arch);
	ଯଦି (*compiled_vers != NUL)
	{
	    ସନ୍ଦେଶ_puts(", ");
	    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)compiled_vers);
	}
    }

# endif

#endif

    // Print the list of patch numbers if there is at least one.
    // Print a range when patches are consecutive: "1-10, 12, 15-40, 42-45"
    ଯଦି (included_patches[0] != 0)
    {
	ସନ୍ଦେଶ_puts(_("\nIncluded patches: "));
	first = -1;
	i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(included_patches) - 1;
	ଯେତେବେଳେ (--i >= 0)
	{
	    ଯଦି (first < 0)
		first = included_patches[i];
	    ଯଦି (i == 0 || included_patches[i - 1] != included_patches[i] + 1)
	    {
		ସନ୍ଦେଶ_puts(s);
		s = ", ";
		ସନ୍ଦେଶ_outnum((long)first);
		ଯଦି (first != included_patches[i])
		{
		    ସନ୍ଦେଶ_puts("-");
		    ସନ୍ଦେଶ_outnum((long)included_patches[i]);
		}
		first = -1;
	    }
	}
    }

    // Print the list of extra patch descriptions if there is at least one.
    ଯଦି (extra_patches[0] != ଖାଲି)
    {
	ସନ୍ଦେଶ_puts(_("\nExtra patches: "));
	s = "";
	ପାଇଁ (i = 0; extra_patches[i] != ଖାଲି; ++i)
	{
	    ସନ୍ଦେଶ_puts(s);
	    s = ", ";
	    ସନ୍ଦେଶ_puts(extra_patches[i]);
	}
    }

#ifdef MODIFIED_BY
    ସନ୍ଦେଶ_puts("\n");
    ସନ୍ଦେଶ_puts(_("Modified by "));
    ସନ୍ଦେଶ_puts(MODIFIED_BY);
#endif

#ifdef HAVE_PATHDEF
    ଯଦି (*compiled_user != NUL || *compiled_sys != NUL)
    {
	ସନ୍ଦେଶ_puts(_("\nCompiled "));
	ଯଦି (*compiled_user != NUL)
	{
	    ସନ୍ଦେଶ_puts(_("by "));
	    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)compiled_user);
	}
	ଯଦି (*compiled_sys != NUL)
	{
	    ସନ୍ଦେଶ_puts("@");
	    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)compiled_sys);
	}
    }
#endif

#ଯଦି defined(FEAT_HUGE)
    ସନ୍ଦେଶ_puts(_("\nHuge version "));
#ନଚେତଯଦି defined(FEAT_NORMAL)
    ସନ୍ଦେଶ_puts(_("\nNormal version "));
#ଅନ୍ୟଥା
    ସନ୍ଦେଶ_puts(_("\nTiny version "));
#endif
#ଯଦି !defined(FEAT_GUI)
    ସନ୍ଦେଶ_puts(_("without GUI."));
#ନଚେତଯଦି defined(FEAT_GUI_GTK)
# ଯଦି defined(USE_GTK4)
    ସନ୍ଦେଶ_puts(_("with GTK4 GUI."));
# elif defined(USE_GTK3)
    ସନ୍ଦେଶ_puts(_("with GTK3 GUI."));
# elif defined(FEAT_GUI_GNOME)
    ସନ୍ଦେଶ_puts(_("with GTK2-GNOME GUI."));
# ଅନ୍ୟଥା
    ସନ୍ଦେଶ_puts(_("with GTK2 GUI."));
# endif
#ନଚେତଯଦି defined(FEAT_GUI_MOTIF)
    ସନ୍ଦେଶ_puts(_("with X11-Motif GUI."));
#ନଚେତଯଦି defined(FEAT_GUI_HAIKU)
    ସନ୍ଦେଶ_puts(_("with Haiku GUI."));
#ନଚେତଯଦି defined(FEAT_GUI_PHOTON)
    ସନ୍ଦେଶ_puts(_("with Photon GUI."));
#ନଚେତଯଦି defined(MSWIN)
    ସନ୍ଦେଶ_puts(_("with GUI."));
#endif
    version_ସନ୍ଦେଶ(_("  Features included (+) or not (-):\n"));

    list_features();
    ଯଦି (ସନ୍ଦେଶ_col > 0)
	ସନ୍ଦେଶ_putchar('\n');

#ifdef SYS_VIMRC_FILE
    version_ସନ୍ଦେଶ(_("   system vimrc file: \""));
    version_ସନ୍ଦେଶ(SYS_VIMRC_FILE);
    version_ସନ୍ଦେଶ("\"\n");
#endif
#ifdef USR_VIMRC_FILE
    version_ସନ୍ଦେଶ(_("     user vimrc file: \""));
    version_ସନ୍ଦେଶ(USR_VIMRC_FILE);
    version_ସନ୍ଦେଶ("\"\n");
#endif
#ifdef USR_VIMRC_FILE2
    version_ସନ୍ଦେଶ(_(" 2nd user vimrc file: \""));
    version_ସନ୍ଦେଶ(USR_VIMRC_FILE2);
    version_ସନ୍ଦେଶ("\"\n");
#endif
#ଯଦି defined(USR_VIMRC_FILE3) && defined(XDG_VIMRC_FILE)
    version_ସନ୍ଦେଶ(_(" 3rd user vimrc file: \""));
    version_ସନ୍ଦେଶ(USR_VIMRC_FILE3);
    version_ସନ୍ଦେଶ("\"\n");
    version_ସନ୍ଦେଶ(_(" 4th user vimrc file: \""));
    version_ସନ୍ଦେଶ((ଅକ୍ଷର *)(XDG_VIMRC_FILE));
    version_ସନ୍ଦେଶ("\"\n");
#ନଚେତଯଦି defined(USR_VIMRC_FILE3)
    version_ସନ୍ଦେଶ(_(" 3rd user vimrc file: \""));
    version_ସନ୍ଦେଶ(USR_VIMRC_FILE3);
    version_ସନ୍ଦେଶ("\"\n");
#ନଚେତଯଦି defined(XDG_VIMRC_FILE)
    version_ସନ୍ଦେଶ(_(" 3rd user vimrc file: \""));
    version_ସନ୍ଦେଶ((ଅକ୍ଷର *)(XDG_VIMRC_FILE));
    version_ସନ୍ଦେଶ("\"\n");
#endif
#ifdef USR_EXRC_FILE
    version_ସନ୍ଦେଶ(_("      user exrc file: \""));
    version_ସନ୍ଦେଶ(USR_EXRC_FILE);
    version_ସନ୍ଦେଶ("\"\n");
#endif
#ifdef USR_EXRC_FILE2
    version_ସନ୍ଦେଶ(_("  2nd user exrc file: \""));
    version_ସନ୍ଦେଶ(USR_EXRC_FILE2);
    version_ସନ୍ଦେଶ("\"\n");
#endif
#ifdef FEAT_GUI
# ifdef SYS_GVIMRC_FILE
    version_ସନ୍ଦେଶ(_("  system gvimrc file: \""));
    version_ସନ୍ଦେଶ(SYS_GVIMRC_FILE);
    version_ସନ୍ଦେଶ("\"\n");
# endif
    version_ସନ୍ଦେଶ(_("    user gvimrc file: \""));
    version_ସନ୍ଦେଶ(USR_GVIMRC_FILE);
    version_ସନ୍ଦେଶ("\"\n");
# ifdef USR_GVIMRC_FILE2
    version_ସନ୍ଦେଶ(_("2nd user gvimrc file: \""));
    version_ସନ୍ଦେଶ(USR_GVIMRC_FILE2);
    version_ସନ୍ଦେଶ("\"\n");
# endif
# ifdef USR_GVIMRC_FILE3
    version_ସନ୍ଦେଶ(_("3rd user gvimrc file: \""));
    version_ସନ୍ଦେଶ(USR_GVIMRC_FILE3);
    version_ସନ୍ଦେଶ("\"\n");
# endif
#endif
    version_ସନ୍ଦେଶ(_("       defaults file: \""));
    version_ସନ୍ଦେଶ(VIM_DEFAULTS_FILE);
    version_ସନ୍ଦେଶ("\"\n");
#ifdef FEAT_GUI
# ifdef SYS_MENU_FILE
    version_ସନ୍ଦେଶ(_("    system menu file: \""));
    version_ସନ୍ଦେଶ(SYS_MENU_FILE);
    version_ସନ୍ଦେଶ("\"\n");
# endif
#endif
#ifdef HAVE_PATHDEF
    ଯଦି (*default_vim_dir != NUL)
    {
	version_ସନ୍ଦେଶ(_("  fall-back for $VIM: \""));
	version_ସନ୍ଦେଶ((ଅକ୍ଷର *)default_vim_dir);
	version_ସନ୍ଦେଶ("\"\n");
    }
    ଯଦି (*default_vimruntime_dir != NUL)
    {
	version_ସନ୍ଦେଶ(_(" f-b for $VIMRUNTIME: \""));
	version_ସନ୍ଦେଶ((ଅକ୍ଷର *)default_vimruntime_dir);
	version_ସନ୍ଦେଶ("\"\n");
    }
    version_ସନ୍ଦେଶ(_("Compilation: "));
    version_ସନ୍ଦେଶ((ଅକ୍ଷର *)all_cflags);
    version_ସନ୍ଦେଶ("\n");
# ifdef VMS
    ଯଦି (*compiler_version != NUL)
    {
	version_ସନ୍ଦେଶ(_("Compiler: "));
	version_ସନ୍ଦେଶ((ଅକ୍ଷର *)compiler_version);
	version_ସନ୍ଦେଶ("\n");
    }
# endif
    version_ସନ୍ଦେଶ(_("Linking: "));
    version_ସନ୍ଦେଶ((ଅକ୍ଷର *)all_lflags);
#endif
#ifdef DEBUG
    version_ସନ୍ଦେଶ("\n");
    version_ସନ୍ଦେଶ(_("  DEBUG BUILD"));
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ do_intro_ଧାଡ଼ି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, char_u *mesg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_version, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr);
ସ୍ଥିର ଶୂନ୍ୟ intro_message(ପୂର୍ଣ୍ଣସଂଖ୍ୟା colon);

/*
 * Show the intro message when not editing a file.
 */
    ଶୂନ୍ୟ
maybe_intro_message(ଶୂନ୍ୟ)
{
    ଯଦି (BUFEMPTY()
	    && curbuf->b_fname == ଖାଲି
	    && firstwin->w_next == ଖାଲି
	    && vim_strchr(p_shm, SHM_INTRO) == ଖାଲି)
	intro_message(FALSE);
}

/*
 * Give an introductory message about Vim.
 * Only used when starting Vim on an empty file, without a file name.
 * Or with the ":intro" command (for Sven :-).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
intro_message(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		colon)		// TRUE for ":intro"
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		blanklines;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sponsor;
    ଅକ୍ଷର	*p;
    ସ୍ଥିର ଅକ୍ଷର	*(ଧାଡ଼ିଗୁଡ଼ିକ[]) =
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
	ଖାଲି,
	"",
	N_("Running in Vi compatible mode"),
	N_("type  :set nocp<Enter>        for Vim defaults"),
	N_("type  :help cp-default<Enter> for info on this"),
    };
#ifdef FEAT_GUI
    ସ୍ଥିର ଅକ୍ଷର	*(gui_ଧାଡ଼ିଗୁଡ଼ିକ[]) =
    {
	ଖାଲି,
	ଖାଲି,
	ଖାଲି,
	ଖାଲି,
# ifdef MODIFIED_BY
	ଖାଲି,
# endif
	ଖାଲି,
	ଖାଲି,
	ଖାଲି,
	N_("menu  Help->Orphans           for information    "),
	ଖାଲି,
	N_("Running modeless, typed text is inserted"),
	N_("menu  Edit->Global Settings->Toggle Insert Mode  "),
	N_("                              for two modes      "),
	ଖାଲି,
	ଖାଲି,
	ଖାଲି,
	N_("menu  Edit->Global Settings->Toggle Vi Compatible"),
	N_("                              for Vim defaults   "),
    };
#endif

    // blanklines = screen height - # message lines
    blanklines = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - (ARRAY_LENGTH(ଧାଡ଼ିଗୁଡ଼ିକ) - 1);
    ଯଦି (!p_cp)
	blanklines += 4;  // add 4 for not showing "Vi compatible" message

    // Don't overwrite a statusline.  Depends on 'cmdheight'.
    ଯଦି (p_ls > 1)
	blanklines -= Rows - topframe->fr_height;
    ଯଦି (blanklines < 0)
	blanklines = 0;

    // Show the sponsor and Uganda message two out of four times
    sponsor = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)time(ଖାଲି);
    sponsor = ((sponsor & 2) == 0) - ((sponsor & 4) == 0);

    // start displaying the message lines after half of the blank lines
    row = blanklines / 2;
    ଯଦି ((row >= 2 && topframe->fr_width >= 50) || colon)
    {
	ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(ଧାଡ଼ିଗୁଡ଼ିକ); ++i)
	{
	    p = ଧାଡ଼ିଗୁଡ଼ିକ[i];
#ifdef FEAT_GUI
	    ଯଦି (p_im && gui.in_use && gui_ଧାଡ଼ିଗୁଡ଼ିକ[i] != ଖାଲି)
		p = gui_ଧାଡ଼ିଗୁଡ଼ିକ[i];
#endif
	    ଯଦି (p == ଖାଲି)
	    {
		ଯଦି (!p_cp)
		    ଭାଙ୍ଗ;
		ଜାରି;
	    }
	    ଯଦି (sponsor != 0)
	    {
		ଯଦି (strstr(p, "children") != ଖାଲି)
		    p = N_("Sponsor Vim development!");
		ଅନ୍ୟଥା ଯଦି (strstr(p, "Kuwasha") != ଖାଲି)
		    p = N_("type  :help sponsor<Enter>    for information ");
		ଅନ୍ୟଥା ଯଦି (strstr(p, "Orphans") != ଖାଲି)
		    p = N_("menu  Help->Sponsor           for information    ");
	    }
	    ଯଦି (*p != NUL)
		do_intro_ଧାଡ଼ି(row, (char_u *)_(p), i == 2, 0);
	    ++row;
	}
    }

    // Make the wait-return message appear just below the text.
    ଯଦି (colon)
	ସନ୍ଦେଶ_row = row;
}

    ସ୍ଥିର ଶୂନ୍ୟ
do_intro_ଧାଡ଼ି(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row,
    char_u	*mesg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_version,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr)
{
    char_u	vers[20];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		clen;
#ifdef MODIFIED_BY
# define MODBY_LEN 150
    char_u	modby[MODBY_LEN];

    ଯଦି (*mesg == ' ')
    {
	vim_strncpy(modby, (char_u *)_("Modified by "), MODBY_LEN - 1);
	l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(modby);
	vim_strncpy(modby + l, (char_u *)MODIFIED_BY, MODBY_LEN - l - 1);
	mesg = modby;
    }
#endif

    // Center the message horizontally.
    col = vim_strsize(mesg);
    ଯଦି (add_version)
    {
	STRCPY(vers, mediumVersion);
	ଯଦି (highest_patch())
	{
	    // Check for 9.9x or 9.9xx, alpha/beta version
	    ଯଦି (SAFE_isalpha((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vers[3]))
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = (SAFE_isalpha((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vers[4])) ? 5 : 4;
		sprintf((ଅକ୍ଷର *)vers + len, ".%d%s", highest_patch(),
							 mediumVersion + len);
	    }
	    ଅନ୍ୟଥା
		sprintf((ଅକ୍ଷର *)vers + 3, ".%d", highest_patch());
	}
	col += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(vers);
    }
    col = (topframe->fr_width - col) / 2;
    ଯଦି (col < 0)
	col = 0;

    // Split up in parts to highlight <> items differently.
    ପାଇଁ (p = mesg; *p != NUL; p += l)
    {
	clen = 0;
	ପାଇଁ (l = 0; p[l] != NUL
			 && (l == 0 || (p[l] != '<' && p[l - 1] != '>')); ++l)
	{
	    ଯଦି (has_mbyte)
	    {
		clen += ptr2cells(p + l);
		l += (*mb_ptr2len)(p + l) - 1;
	    }
	    ଅନ୍ୟଥା
		clen += byte2cells(p[l]);
	}
	screen_puts_len(p, l, row, col + firstwin->w_wincol,
		*p == '<' ? HL_ATTR(HLF_8) : attr);
	col += clen;
    }

    // Add the version number to the version line.
    ଯଦି (add_version)
	screen_puts(vers, row, col + firstwin->w_wincol, 0);
}

/*
 * ":intro": clear screen, display intro screen and wait for return.
 */
    ଶୂନ୍ୟ
ex_intro(exarg_T *eap UNUSED)
{
    screenclear();
#ଯଦି defined(FEAT_TABPANEL)
    draw_tabpanel();
#endif
    intro_message(TRUE);
    wait_return(TRUE);
}
