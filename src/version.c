/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

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

#ضمن "version.h"

حرف		*نسخه = VIM_نسخه_SHORT;
ساكن حرف	*medium_نسخه = VIM_نسخه_MEDIUM;

#إذا defined(HAVE_DATE_وخت)
# إذا defined(VMS) && defined(VAXC)
حرف	long_نسخه[حجم(VIM_نسخه_LONG_DATE) + حجم(__DATE__)
						      + حجم(__TIME__) + 3];

    فراغ
init_long_نسخه(فراغ)
{
    /*
     * Construct the long version string.  Necessary because
     * VAX C can't concatenate strings in the preprocessor.
     */
    strcpy(long_نسخه, VIM_نسخه_LONG_DATE);
#  ifdef BUILD_DATE
    strcat(long_نسخه, BUILD_DATE);
#  وإلا
    strcat(long_نسخه, __DATE__);
    strcat(long_نسخه, " ");
    strcat(long_نسخه, __TIME__);
#  endif
    strcat(long_نسخه, ")");
}

# وإلا
حرف	*long_نسخه = عدم;

    فراغ
init_long_نسخه(فراغ)
{
    إذا (long_نسخه != عدم)
	أرجع;

#  ifdef BUILD_DATE
    حرف *date_وخت = BUILD_DATE;
#  وإلا
    حرف *date_وخت = __DATE__ " " __TIME__;
#  endif
    حرف *پیغام = _("%s (%s, compiled %s)");
    size_t len = مزي_اوږدوالی(پیغام)
	+ STRLEN_LITERAL(VIM_نسخه_LONG_ONLY)
	+ STRLEN_LITERAL(VIM_نسخه_DATE_ONLY)
	+ مزي_اوږدوالی(date_وخت);

    long_نسخه = alloc(len);
    إذا (long_نسخه == عدم)
	long_نسخه = VIM_نسخه_LONG;
    وإلا
	vim_snprintf(long_نسخه, len, پیغام,
		VIM_نسخه_LONG_ONLY, VIM_نسخه_DATE_ONLY, date_وخت);
}
# endif
#وإلا
حرف	*long_نسخه = VIM_نسخه_LONG;

    فراغ
init_long_نسخه(فراغ)
{
    // nothing to do
}
#د_که_پای

ساكن حرف *(features[]) =
{
#که_تعریف_شوی HAVE_ACL
	"+acl",
#وإلا
	"-acl",
#د_که_پای
#که_تعریف_شوی FEAT_ARABIC
	"+arabic",
#وإلا
	"-arabic",
#د_که_پای
#که_تعریف_شوی AMIGA		// only for Amiga systems
# ifdef FEAT_ARP
	"+ARP",
# وإلا
	"-ARP",
# endif
#د_که_پای
#که_تعریف_شوی FEAT_AUTOCHDIR
	"+autochdir",
#وإلا
	"-autochdir",
#د_که_پای
	"+autocmd",
#که_تعریف_شوی FEAT_AUTOSERVERNAME
	"+autoservername",
#وإلا
	"-autoservername",
#د_که_پای
#که_تعریف_شوی FEAT_BEVAL_GUI
	"+balloon_eval",
#وإلا
	"-balloon_eval",
#د_که_پای
#که_تعریف_شوی FEAT_BEVAL_غړی
	"+balloon_eval_term",
#وإلا
	"-balloon_eval_term",
#د_که_پای
#که_تعریف_شوی FEAT_BROWSE
	"+browse",
#وإلا
	"-browse",
#د_که_پای
	"++builtin_terms",
#که_تعریف_شوی FEAT_BYTEOFF
	"+byte_offset",
#وإلا
	"-byte_offset",
#د_که_پای
#که_تعریف_شوی FEAT_دنده_CHANNEL
	"+channel",
#وإلا
	"-channel",
#د_که_پای
	"+cindent",
#که_تعریف_شوی FEAT_CLIENTSERVER
	"+clientserver",
#وإلا
	"-clientserver",
#د_که_پای
#که_تعریف_شوی FEAT_CLIPBOARD
	"+clipboard",
#وإلا
	"-clipboard",
#د_که_پای
#که_تعریف_شوی FEAT_CLIPBOARD_PROVIDER
	"+clipboard_provider",
#وإلا
	"-clipboard_provider",
#د_که_پای
	"+cmdline_compl",
	"+cmdline_hist",
	"+cmdline_info",
	"+comments",
#که_تعریف_شوی FEAT_CONCEAL
	"+conceal",
#وإلا
	"-conceal",
#د_که_پای
#که_تعریف_شوی FEAT_CRYPT
	"+cryptv",
#وإلا
	"-cryptv",
#د_که_پای
#که_تعریف_شوی FEAT_CSCOPE
	"+cscope",
#وإلا
	"-cscope",
#د_که_پای
	"+cursorbind",
#که_تعریف_شوی CURSOR_SHAPE
	"+cursorshape",
#وإلا
	"-cursorshape",
#د_که_پای
#إذا defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
	"+dialog_con_gui",
#وإلا
# إذا defined(FEAT_CON_DIALOG)
	"+dialog_con",
# وإلا
#  إذا defined(FEAT_GUI_DIALOG)
	"+dialog_gui",
#  وإلا
	"-dialog",
#  endif
# endif
#د_که_پای
#که_تعریف_شوی FEAT_DIFF
	"+diff",
#وإلا
	"-diff",
#د_که_پای
#که_تعریف_شوی FEAT_DIGRAPHS
	"+digraphs",
#وإلا
	"-digraphs",
#د_که_پای
#که_تعریف_شوی FEAT_GUI_MSWIN
# ifdef FEAT_DIRECTX
	"+directx",
# وإلا
	"-directx",
# endif
#د_که_پای
#که_تعریف_شوی FEAT_DND
	"+dnd",
#وإلا
	"-dnd",
#د_که_پای
	"-ebcdic",
#که_تعریف_شوی FEAT_EMACS_نښانونه
	"+emacs_tags",
#وإلا
	"-emacs_tags",
#د_که_پای
#که_تعریف_شوی FEAT_EVAL
	"+eval",
#وإلا
	"-eval",
#د_که_پای
	"+ex_extra",
#که_تعریف_شوی FEAT_پلټنه_EXTRA
	"+extra_search",
#وإلا
	"-extra_search",
#د_که_پای
	"-farsi",
	"+file_in_path",
#که_تعریف_شوی FEAT_پیدا_کړه_ID
	"+find_in_path",
#وإلا
	"-find_in_path",
#د_که_پای
	"+float",
#که_تعریف_شوی FEAT_FOLDING
	"+folding",
#وإلا
	"-folding",
#د_که_پای
	"-footer",
	// only interesting on Unix systems
#إذا !defined(USE_SYSTEM) && defined(UNIX)
	"+fork()",
#د_که_پای
#که_تعریف_شوی FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
	"+gettext/dyn",
# وإلا
	"+gettext",
# endif
#وإلا
	"-gettext",
#د_که_پای
	"-hangul_input",
#إذا (defined(HAVE_ICONV_H) && defined(USE_ICONV)) || defined(DYNAMIC_ICONV)
# ifdef DYNAMIC_ICONV
	"+iconv/dyn",
# وإلا
	"+iconv",
# endif
#وإلا
	"-iconv",
#د_که_پای
#که_تعریف_شوی FEAT_IMAGE
	"+image",
#وإلا
	"-image",
#د_که_پای
#که_تعریف_شوی FEAT_IMAGE_POPUP
	"+image_popup",
#وإلا
	"-image_popup",
#د_که_پای
	"+insert_expand",
#که_تعریف_شوی FEAT_IPV6
	"+ipv6",
#وإلا
	"-ipv6",
#د_که_پای
#که_تعریف_شوی FEAT_دنده_CHANNEL
	"+job",
#وإلا
	"-job",
#د_که_پای
	"+jumplist",
#که_تعریف_شوی FEAT_KEYMAP
	"+keymap",
#وإلا
	"-keymap",
#د_که_پای
#که_تعریف_شوی FEAT_EVAL
	"+lambda",
#وإلا
	"-lambda",
#د_که_پای
#که_تعریف_شوی FEAT_LANGMAP
	"+langmap",
#وإلا
	"-langmap",
#د_که_پای
#که_تعریف_شوی FEAT_LIBCALL
	"+libcall",
#وإلا
	"-libcall",
#د_که_پای
#که_تعریف_شوی FEAT_LINEBREAK
	"+linebreak",
#وإلا
	"-linebreak",
#د_که_پای
	"+lispindent",
	"+listcmds",
	"+localmap",
#که_تعریف_شوی FEAT_LUA
# ifdef DYNAMIC_LUA
	"+lua/dyn",
# وإلا
	"+lua",
# endif
#وإلا
	"-lua",
#د_که_پای
#که_تعریف_شوی FEAT_MENU
	"+menu",
#وإلا
	"-menu",
#د_که_پای
#که_تعریف_شوی FEAT_SESSION
	"+mksession",
#وإلا
	"-mksession",
#د_که_پای
	"+modify_fname",
	"+mouse",
#إذا defined(UNIX) || defined(VMS)
# ifdef FEAT_MOUSE_DEC
	"+mouse_dec",
# وإلا
	"-mouse_dec",
# endif
# ifdef FEAT_MOUSE_GPM
#  ifdef DYNAMIC_GPM
	"+mouse_gpm/dyn",
#  وإلا
	"+mouse_gpm",
#  endif
# وإلا
	"-mouse_gpm",
# endif
# ifdef FEAT_MOUSE_JSB
	"+mouse_jsbterm",
# وإلا
	"-mouse_jsbterm",
# endif
# ifdef FEAT_MOUSE_NET
	"+mouse_netterm",
# وإلا
	"-mouse_netterm",
# endif
#د_که_پای

#که_تعریف_شوی __QNX__
# ifdef FEAT_MOUSE_PTERM
	"+mouse_pterm",
# وإلا
	"-mouse_pterm",
# endif
#د_که_پای

#إذا defined(UNIX) || defined(VMS)
	"+mouse_sgr",
# ifdef FEAT_SYSMOUSE
	"+mouse_sysmouse",
# وإلا
	"-mouse_sysmouse",
# endif
# ifdef FEAT_MOUSE_URXVT
	"+mouse_urxvt",
# وإلا
	"-mouse_urxvt",
# endif
	"+mouse_xterm",
#د_که_پای
#که_تعریف_شوی FEAT_MOUSESHAPE
	"+mouseshape",
#وإلا
	"-mouseshape",
#د_که_پای

#که_تعریف_شوی FEAT_MBYTE_IME
# ifdef DYNAMIC_IME
	"+multi_byte_ime/dyn",
# وإلا
	"+multi_byte_ime",
# endif
#وإلا
	"+multi_byte",
#د_که_پای
#که_تعریف_شوی FEAT_MULTI_LANG
	"+multi_lang",
#وإلا
	"-multi_lang",
#د_که_پای
#که_تعریف_شوی FEAT_MZSCHEME
# ifdef DYNAMIC_MZSCHEME
	"+mzscheme/dyn",
# وإلا
	"+mzscheme",
# endif
#وإلا
	"-mzscheme",
#د_که_پای
#که_تعریف_شوی FEAT_NETBEANS_INTG
	"+netbeans_intg",
#وإلا
	"-netbeans_intg",
#د_که_پای
	"+num64",
#که_تعریف_شوی FEAT_GUI_MSWIN
# ifdef FEAT_OLE
	"+ole",
# وإلا
	"-ole",
# endif
#د_که_پای
#که_تعریف_شوی FEAT_EVAL
	"+packages",
#وإلا
	"-packages",
#د_که_پای
#که_تعریف_شوی FEAT_PRINT_PANGO
	"+pango",
#وإلا
	"-pango",
#د_که_پای
	"+path_extra",
#که_تعریف_شوی FEAT_PERL
# ifdef DYNAMIC_PERL
	"+perl/dyn",
# وإلا
	"+perl",
# endif
#وإلا
	"-perl",
#د_که_پای
#که_تعریف_شوی FEAT_PERSISTENT_UNDO
	"+persistent_undo",
#وإلا
	"-persistent_undo",
#د_که_پای
#که_تعریف_شوی FEAT_PROP_POPUP
	"+popupwin",
#وإلا
	"-popupwin",
#د_که_پای
#که_تعریف_شوی FEAT_PRINTER
# ifdef FEAT_POSTSCRIPT
	"+postscript",
# وإلا
	"-postscript",
# endif
	"+printer",
#وإلا
	"-printer",
#د_که_پای
#که_تعریف_شوی FEAT_PROFILE
	"+profile",
#وإلا
	"-profile",
#د_که_پای
#که_تعریف_شوی FEAT_PYTHON
# ifdef DYNAMIC_PYTHON
	"+python/dyn",
# وإلا
	"+python",
# endif
#وإلا
	"-python",
#د_که_پای
#که_تعریف_شوی FEAT_PYTHON3
# ifdef DYNAMIC_PYTHON3
#  ifdef DYNAMIC_PYTHON3_STABLE_ABI
	"+python3/dyn-stable",
#  وإلا
	"+python3/dyn",
#  endif
# وإلا
	"+python3",
# endif
#وإلا
	"-python3",
#د_که_پای
#که_تعریف_شوی FEAT_QUICKFIX
	"+quickfix",
#وإلا
	"-quickfix",
#د_که_پای
#که_تعریف_شوی FEAT_RELTIME
	"+reltime",
#وإلا
	"-reltime",
#د_که_پای
#که_تعریف_شوی FEAT_RIGHTLEFT
	"+rightleft",
#وإلا
	"-rightleft",
#د_که_پای
#که_تعریف_شوی FEAT_RUBY
# ifdef DYNAMIC_RUBY
	"+ruby/dyn",
# وإلا
	"+ruby",
# endif
#وإلا
	"-ruby",
#د_که_پای
	"+scrollbind",
#که_تعریف_شوی FEAT_SIGNS
	"+signs",
#وإلا
	"-signs",
#د_که_پای
	"+smartindent",
#که_تعریف_شوی FEAT_SOCKETSERVER
	"+socketserver",
#وإلا
	"-socketserver",
#د_که_پای
#که_تعریف_شوی FEAT_SODIUM
# ifdef DYNAMIC_SODIUM
	"+sodium/dyn",
# وإلا
	"+sodium",
# endif
#وإلا
	"-sodium",
#د_که_پای
#که_تعریف_شوی FEAT_SOUND
	"+sound",
#وإلا
	"-sound",
#د_که_پای
#که_تعریف_شوی FEAT_SPELL
	"+spell",
#وإلا
	"-spell",
#د_که_پای
#که_تعریف_شوی STARTUPTIME
	"+startuptime",
#وإلا
	"-startuptime",
#د_که_پای
#که_تعریف_شوی FEAT_STL_OPT
	"+statusline",
#وإلا
	"-statusline",
#د_که_پای
	"-sun_workshop",
#که_تعریف_شوی FEAT_SYN_HL
	"+syntax",
#وإلا
	"-syntax",
#د_که_پای
	    // only interesting on Unix systems
#إذا defined(USE_SYSTEM) && defined(UNIX)
	"+system()",
#د_که_پای
#إذا defined(FEAT_TABPANEL)
	"+tabpanel",
#وإلا
	"-tabpanel",
#د_که_پای
	"-tag_any_white",
	"+tag_binary",
	"-tag_old_static",
#که_تعریف_شوی FEAT_TCL
# ifdef DYNAMIC_TCL
	"+tcl/dyn",
# وإلا
	"+tcl",
# endif
#وإلا
	"-tcl",
#د_که_پای
#که_تعریف_شوی FEAT_TERMGUICOLORS
	"+termguicolors",
#وإلا
	"-termguicolors",
#د_که_پای
#که_تعریف_شوی FEAT_TERMINAL
	"+terminal",
#وإلا
	"-terminal",
#د_که_پای
#إذا defined(UNIX)
// only Unix can have terminfo instead of termcap
# ifdef TERMINFO
	"+terminfo",
# وإلا
	"-terminfo",
# endif
#د_که_پای
#که_تعریف_شوی FEAT_TERMRESPONSE
	"+termresponse",
#وإلا
	"-termresponse",
#د_که_پای
	"+textobjects",
#که_تعریف_شوی FEAT_PROP_POPUP
	"+textprop",
#وإلا
	"-textprop",
#د_که_پای
#إذا !defined(UNIX)
// unix always includes termcap support
# ifdef HAVE_TGETENT
	"+tgetent",
# وإلا
	"-tgetent",
# endif
#د_که_پای
#که_تعریف_شوی FEAT_TIMERS
	"+timers",
#وإلا
	"-timers",
#د_که_پای
	"+title",
#که_تعریف_شوی FEAT_TOOLBAR
	"+toolbar",
#وإلا
	"-toolbar",
#د_که_پای
	"+user_commands",
#که_تعریف_شوی FEAT_VARTABS
	"+vartabs",
#وإلا
	"-vartabs",
#د_که_پای
	"+vertsplit",
	"+vim9script",
#که_تعریف_شوی FEAT_VIMINFO
	"+viminfo",
#وإلا
	"-viminfo",
#د_که_پای
	"+virtualedit",
	"+visual",
	"+visualextra",
	"+vreplace",
#که_تعریف_شوی MSWIN
# ifdef FEAT_VTP
	"+vtp",
# وإلا
	"-vtp",
# endif
#د_که_پای
#که_تعریف_شوی FEAT_WAYLAND
	"+wayland",
#وإلا
	"-wayland",
#د_که_پای
#که_تعریف_شوی FEAT_WAYLAND_CLIPBOARD
	"+wayland_clipboard",
#وإلا
	"-wayland_clipboard",
#د_که_پای
	"+wildignore",
	"+wildmenu",
	"+windows",
#که_تعریف_شوی FEAT_WRITEBACKUP
	"+writebackup",
#وإلا
	"-writebackup",
#د_که_پای
#إذا defined(UNIX) || defined(VMS)
# ifdef FEAT_X11
	"+X11",
# وإلا
	"-X11",
# endif
#د_که_پای
#که_تعریف_شوی FEAT_XATTR
	"+xattr",
#وإلا
	"-xattr",
#د_که_پای
#که_تعریف_شوی FEAT_XFONTSET
	"+xfontset",
#وإلا
	"-xfontset",
#د_که_پای
#که_تعریف_شوی FEAT_XIM
	"+xim",
#وإلا
	"-xim",
#د_که_پای
#إذا defined(MSWIN)
# ifdef FEAT_XPM_W32
	"+xpm_w32",
# وإلا
	"-xpm_w32",
# endif
#وإلاإذا defined(HAVE_XPM)
	"+xpm",
#وإلا
	"-xpm",
#د_که_پای
#إذا defined(UNIX) || defined(VMS)
# إذا defined(USE_XSMP_INTERACT)
	"+xsmp_interact",
# elif defined(USE_XSMP)
	"+xsmp",
# وإلا
	"-xsmp",
# endif
# ifdef FEAT_XCLIPBOARD
	"+xterm_clipboard",
# وإلا
	"-xterm_clipboard",
# endif
#د_که_پای
#که_تعریف_شوی FEAT_XTERM_SAVE
	"+xterm_save",
#وإلا
	"-xterm_save",
#د_که_پای
	عدم
};

ساكن صحيح included_patches[] =
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
ساكن حرف *(extra_patches[]) =
{   /* Add your patch description below this line */
/**/
    عدم
};

    صحيح
highest_patch(فراغ)
{
    // this relies on the highest patch number to be the first entry
    أرجع included_patches[0];
}

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE if patch "n" has been included.
 */
    صحيح
has_patch(صحيح n)
{
    صحيح		h, m, l;

    // Perform a binary search.
    l = 0;
    h = (صحيح)ARRAY_اوږدتیا(included_patches) - 1;
    لكل (;;)
    {
	m = (l + h) / 2;
	إذا (included_patches[m] == n)
	    أرجع TRUE;
	إذا (l == h)
	    اكسر;
	إذا (included_patches[m] < n)
	    h = m;
	وإلا
	    l = m + 1;
    }
    أرجع FALSE;
}
#د_که_پای

    فراغ
ex_نسخه(exarg_T *eap)
{
    /*
     * Ignore a ":version 9.99" command.
     */
    إذا (*eap->arg == NUL)
    {
	پیغام_putchar('\n');
	list_نسخه();
    }
}

/*
 * Output a string for the version message.  If it's going to wrap, output a
 * newline, unless the message is too long to fit on the screen anyway.
 * When "wrap" is TRUE wrap the string in [].
 */
    ساكن فراغ
نسخه_پیغام_wrap(char_u *s, صحيح wrap)
{
    صحيح		len = vim_strsize(s) + (wrap ? 2 : 0);

    إذا (!got_int && len < cmdline_پلنوالی && پیغام_ستون + len >= cmdline_پلنوالی
								&& *s != '\n')
	پیغام_putchar('\n');
    إذا (!got_int)
    {
	إذا (wrap)
	    پیغام_puts("[");
	پیغام_puts((حرف *)s);
	إذا (wrap)
	    پیغام_puts("]");
    }
}

    ساكن فراغ
نسخه_پیغام(حرف *s)
{
    نسخه_پیغام_wrap((char_u *)s, FALSE);
}

/*
 * List all features aligned in columns, dictionary style.
 */
    ساكن فراغ
list_features(فراغ)
{
    list_دننه_columns((char_u **)features, -1, -1);
}

/*
 * List string items nicely aligned in columns.
 * When "size" is < 0 then the last entry is marked with NULL.
 * The entry with index "current" is inclosed in [].
 */
    فراغ
list_دننه_columns(char_u **توکي, صحيح اندازه, صحيح current)
{
    صحيح		i;
    صحيح		ncol;
    صحيح		nrow;
    صحيح		cur_لیکه = 1;
    صحيح		توکی_شمېر = 0;
    صحيح		پلنوالی = 0;
#که_تعریف_شوی FEAT_SYN_HL
    صحيح		use_highlight = (توکي == (char_u **)features);
#د_که_پای

    // Find the length of the longest item, use that + 1 as the column
    // width.
    لكل (i = 0; اندازه < 0 ? توکي[i] != عدم : i < اندازه; ++i)
    {
	صحيح l = vim_strsize(توکي[i]) + (i == current ? 2 : 0);

	إذا (l > پلنوالی)
	    پلنوالی = l;
	++توکی_شمېر;
    }
    پلنوالی += 1;

    إذا (cmdline_پلنوالی < پلنوالی)
    {
	// Not enough screen columns - show one per line
	لكل (i = 0; i < توکی_شمېر; ++i)
	{
	    نسخه_پیغام_wrap(توکي[i], i == current);
	    إذا (پیغام_ستون > 0 && i < توکی_شمېر - 1)
		پیغام_putchar('\n');
	}
	أرجع;
    }

    // The rightmost column doesn't need a separator.
    // Sacrifice it to fit in one more column if possible.
    ncol = (cmdline_پلنوالی + 1) / پلنوالی;
    nrow = توکی_شمېر / ncol + ((توکی_شمېر % ncol) ? 1 : 0);

    // "i" counts columns then rows.  "idx" counts rows then columns.
    لكل (i = 0; !got_int && i < nrow * ncol; ++i)
    {
	صحيح idx = (i / ncol) + (i % ncol) * nrow;

	إذا (idx < توکی_شمېر)
	{
	    صحيح وروستی_ستون = (i + 1) % ncol == 0;

	    إذا (idx == current)
		پیغام_putchar('[');
#که_تعریف_شوی FEAT_SYN_HL
	    إذا (use_highlight && توکي[idx][0] == '-')
		پیغام_puts_attr((حرف *)توکي[idx], HL_ATTR(HLF_W));
	    وإلا
#د_که_پای
		پیغام_puts((حرف *)توکي[idx]);
	    إذا (idx == current)
		پیغام_putchar(']');
	    إذا (وروستی_ستون)
	    {
		إذا (پیغام_ستون > 0 && cur_لیکه < nrow)
		    پیغام_putchar('\n');
		++cur_لیکه;
	    }
	    وإلا
	    {
		طالما (پیغام_ستون % پلنوالی)
		    پیغام_putchar(' ');
	    }
	}
	وإلا
	{
	    // this row is out of items, thus at the end of the row
	    إذا (پیغام_ستون > 0)
	    {
		إذا (cur_لیکه < nrow)
		    پیغام_putchar('\n');
		++cur_لیکه;
	    }
	}
    }
}

    فراغ
list_نسخه(فراغ)
{
    صحيح		i;
    صحيح		لومړی;
    حرف	*s = "";

    /*
     * When adding features here, don't forget to update the list of
     * internal variables in eval.c!
     */
    init_long_نسخه();
    پیغام(long_نسخه);
#که_تعریف_شوی MSWIN
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
#   ifdef _WIN64
#    إذا defined(_M_ARM64) || defined(_M_ARM64EC)
     پیغام_puts(_("\nMS-Windows ARM64 GUI/console version"));
#    وإلا
     پیغام_puts(_("\nMS-Windows 64-bit GUI/console version"));
#    endif
#   وإلا
    پیغام_puts(_("\nMS-Windows 32-bit GUI/console version"));
#   endif
#  وإلا
#   ifdef _WIN64
#    إذا defined(_M_ARM64) || defined(_M_ARM64EC)
     پیغام_puts(_("\nMS-Windows ARM64 GUI version"));
#    وإلا
     پیغام_puts(_("\nMS-Windows 64-bit GUI version"));
#    endif
#   وإلا
    پیغام_puts(_("\nMS-Windows 32-bit GUI version"));
#   endif
#  endif
#  ifdef FEAT_OLE
    پیغام_puts(_(" with OLE support"));
#  endif
# وإلا
#  ifdef _WIN64
#   إذا defined(_M_ARM64) || defined(_M_ARM64EC)
    پیغام_puts(_("\nMS-Windows ARM64 console version"));
#   وإلا
    پیغام_puts(_("\nMS-Windows 64-bit console version"));
#   endif
#  وإلا
    پیغام_puts(_("\nMS-Windows 32-bit console version"));
#  endif
# endif
#د_که_پای
#إذا defined(MACOS_X)
# إذا defined(MACOS_X_DARWIN)
    پیغام_puts(_("\nmacOS version"));
# وإلا
    پیغام_puts(_("\nmacOS version w/o darwin feat."));
# endif
# إذا defined(__arm64__)
    پیغام_puts(" - arm64");
# elif defined(__x86_64__)
    پیغام_puts(" - x86_64");
# endif
#د_که_پای

#که_تعریف_شوی VMS
    پیغام_puts(_("\nOpenVMS (build) arch, version"));
# ifdef HAVE_PATHDEF
    إذا (*compiled_arch != NUL)
    {
	پیغام_puts(": ");
	پیغام_puts((حرف *)compiled_arch);
	إذا (*compiled_vers != NUL)
	{
	    پیغام_puts(", ");
	    پیغام_puts((حرف *)compiled_vers);
	}
    }

# endif

#د_که_پای

    // Print the list of patch numbers if there is at least one.
    // Print a range when patches are consecutive: "1-10, 12, 15-40, 42-45"
    إذا (included_patches[0] != 0)
    {
	پیغام_puts(_("\nIncluded patches: "));
	لومړی = -1;
	i = (صحيح)ARRAY_اوږدتیا(included_patches) - 1;
	طالما (--i >= 0)
	{
	    إذا (لومړی < 0)
		لومړی = included_patches[i];
	    إذا (i == 0 || included_patches[i - 1] != included_patches[i] + 1)
	    {
		پیغام_puts(s);
		s = ", ";
		پیغام_outnum((اوږد)لومړی);
		إذا (لومړی != included_patches[i])
		{
		    پیغام_puts("-");
		    پیغام_outnum((اوږد)included_patches[i]);
		}
		لومړی = -1;
	    }
	}
    }

    // Print the list of extra patch descriptions if there is at least one.
    إذا (extra_patches[0] != عدم)
    {
	پیغام_puts(_("\nExtra patches: "));
	s = "";
	لكل (i = 0; extra_patches[i] != عدم; ++i)
	{
	    پیغام_puts(s);
	    s = ", ";
	    پیغام_puts(extra_patches[i]);
	}
    }

#که_تعریف_شوی MODIFIED_BY
    پیغام_puts("\n");
    پیغام_puts(_("Modified by "));
    پیغام_puts(MODIFIED_BY);
#د_که_پای

#که_تعریف_شوی HAVE_PATHDEF
    إذا (*compiled_user != NUL || *compiled_sys != NUL)
    {
	پیغام_puts(_("\nCompiled "));
	إذا (*compiled_user != NUL)
	{
	    پیغام_puts(_("by "));
	    پیغام_puts((حرف *)compiled_user);
	}
	إذا (*compiled_sys != NUL)
	{
	    پیغام_puts("@");
	    پیغام_puts((حرف *)compiled_sys);
	}
    }
#د_که_پای

#إذا defined(FEAT_HUGE)
    پیغام_puts(_("\nHuge version "));
#وإلاإذا defined(FEAT_نورمال)
    پیغام_puts(_("\nNormal version "));
#وإلا
    پیغام_puts(_("\nTiny version "));
#د_که_پای
#إذا !defined(FEAT_GUI)
    پیغام_puts(_("without GUI."));
#وإلاإذا defined(FEAT_GUI_GTK)
# إذا defined(USE_GTK4)
    پیغام_puts(_("with GTK4 GUI."));
# elif defined(USE_GTK3)
    پیغام_puts(_("with GTK3 GUI."));
# elif defined(FEAT_GUI_GNOME)
    پیغام_puts(_("with GTK2-GNOME GUI."));
# وإلا
    پیغام_puts(_("with GTK2 GUI."));
# endif
#وإلاإذا defined(FEAT_GUI_MOTIF)
    پیغام_puts(_("with X11-Motif GUI."));
#وإلاإذا defined(FEAT_GUI_HAIKU)
    پیغام_puts(_("with Haiku GUI."));
#وإلاإذا defined(FEAT_GUI_PHOTON)
    پیغام_puts(_("with Photon GUI."));
#وإلاإذا defined(MSWIN)
    پیغام_puts(_("with GUI."));
#د_که_پای
    نسخه_پیغام(_("  Features included (+) or not (-):\n"));

    list_features();
    إذا (پیغام_ستون > 0)
	پیغام_putchar('\n');

#که_تعریف_شوی SYS_VIMRC_FILE
    نسخه_پیغام(_("   system vimrc file: \""));
    نسخه_پیغام(SYS_VIMRC_FILE);
    نسخه_پیغام("\"\n");
#د_که_پای
#که_تعریف_شوی USR_VIMRC_FILE
    نسخه_پیغام(_("     user vimrc file: \""));
    نسخه_پیغام(USR_VIMRC_FILE);
    نسخه_پیغام("\"\n");
#د_که_پای
#که_تعریف_شوی USR_VIMRC_FILE2
    نسخه_پیغام(_(" 2nd user vimrc file: \""));
    نسخه_پیغام(USR_VIMRC_FILE2);
    نسخه_پیغام("\"\n");
#د_که_پای
#إذا defined(USR_VIMRC_FILE3) && defined(XDG_VIMRC_FILE)
    نسخه_پیغام(_(" 3rd user vimrc file: \""));
    نسخه_پیغام(USR_VIMRC_FILE3);
    نسخه_پیغام("\"\n");
    نسخه_پیغام(_(" 4th user vimrc file: \""));
    نسخه_پیغام((حرف *)(XDG_VIMRC_FILE));
    نسخه_پیغام("\"\n");
#وإلاإذا defined(USR_VIMRC_FILE3)
    نسخه_پیغام(_(" 3rd user vimrc file: \""));
    نسخه_پیغام(USR_VIMRC_FILE3);
    نسخه_پیغام("\"\n");
#وإلاإذا defined(XDG_VIMRC_FILE)
    نسخه_پیغام(_(" 3rd user vimrc file: \""));
    نسخه_پیغام((حرف *)(XDG_VIMRC_FILE));
    نسخه_پیغام("\"\n");
#د_که_پای
#که_تعریف_شوی USR_EXRC_FILE
    نسخه_پیغام(_("      user exrc file: \""));
    نسخه_پیغام(USR_EXRC_FILE);
    نسخه_پیغام("\"\n");
#د_که_پای
#که_تعریف_شوی USR_EXRC_FILE2
    نسخه_پیغام(_("  2nd user exrc file: \""));
    نسخه_پیغام(USR_EXRC_FILE2);
    نسخه_پیغام("\"\n");
#د_که_پای
#که_تعریف_شوی FEAT_GUI
# ifdef SYS_GVIMRC_FILE
    نسخه_پیغام(_("  system gvimrc file: \""));
    نسخه_پیغام(SYS_GVIMRC_FILE);
    نسخه_پیغام("\"\n");
# endif
    نسخه_پیغام(_("    user gvimrc file: \""));
    نسخه_پیغام(USR_GVIMRC_FILE);
    نسخه_پیغام("\"\n");
# ifdef USR_GVIMRC_FILE2
    نسخه_پیغام(_("2nd user gvimrc file: \""));
    نسخه_پیغام(USR_GVIMRC_FILE2);
    نسخه_پیغام("\"\n");
# endif
# ifdef USR_GVIMRC_FILE3
    نسخه_پیغام(_("3rd user gvimrc file: \""));
    نسخه_پیغام(USR_GVIMRC_FILE3);
    نسخه_پیغام("\"\n");
# endif
#د_که_پای
    نسخه_پیغام(_("       defaults file: \""));
    نسخه_پیغام(VIM_DEFAULTS_FILE);
    نسخه_پیغام("\"\n");
#که_تعریف_شوی FEAT_GUI
# ifdef SYS_MENU_FILE
    نسخه_پیغام(_("    system menu file: \""));
    نسخه_پیغام(SYS_MENU_FILE);
    نسخه_پیغام("\"\n");
# endif
#د_که_پای
#که_تعریف_شوی HAVE_PATHDEF
    إذا (*default_vim_dir != NUL)
    {
	نسخه_پیغام(_("  fall-back for $VIM: \""));
	نسخه_پیغام((حرف *)default_vim_dir);
	نسخه_پیغام("\"\n");
    }
    إذا (*default_vimruntime_dir != NUL)
    {
	نسخه_پیغام(_(" f-b for $VIMRUNTIME: \""));
	نسخه_پیغام((حرف *)default_vimruntime_dir);
	نسخه_پیغام("\"\n");
    }
    نسخه_پیغام(_("Compilation: "));
    نسخه_پیغام((حرف *)all_cflags);
    نسخه_پیغام("\n");
# ifdef VMS
    إذا (*compiler_نسخه != NUL)
    {
	نسخه_پیغام(_("Compiler: "));
	نسخه_پیغام((حرف *)compiler_نسخه);
	نسخه_پیغام("\n");
    }
# endif
    نسخه_پیغام(_("Linking: "));
    نسخه_پیغام((حرف *)all_lflags);
#د_که_پای
#که_تعریف_شوی DEBUG
    نسخه_پیغام("\n");
    نسخه_پیغام(_("  DEBUG BUILD"));
#د_که_پای
}

ساكن فراغ do_intro_کرښه(صحيح لیکه, char_u *mesg, صحيح جمع_کړه_نسخه, صحيح attr);
ساكن فراغ intro_پیغام(صحيح colon);

/*
 * Show the intro message when not editing a file.
 */
    فراغ
maybe_intro_پیغام(فراغ)
{
    إذا (BUFEMPTY()
	    && curbuf->b_fname == عدم
	    && firstwin->w_بل == عدم
	    && vim_strchr(p_shm, SHM_INTRO) == عدم)
	intro_پیغام(FALSE);
}

/*
 * Give an introductory message about Vim.
 * Only used when starting Vim on an empty file, without a file name.
 * Or with the ":intro" command (for Sven :-).
 */
    ساكن فراغ
intro_پیغام(
    صحيح		colon)		// TRUE for ":intro"
{
    صحيح		i;
    صحيح		لیکه;
    صحيح		blanklines;
    صحيح		sponsor;
    حرف	*p;
    ساكن حرف	*(کرښې[]) =
    {
	N_("VIM - Vi IMproved"),
	"",
	N_("version "),
	N_("by Bram Moolenaar et al."),
#که_تعریف_شوی MODIFIED_BY
	" ",
#د_که_پای
	N_("Vim is open source and freely distributable"),
	"",
	N_("Help poor children in Uganda!"),
	N_("type  :help Kuwasha<Enter>    for information "),
	"",
	N_("type  :q<Enter>               to exit         "),
	N_("type  :help<Enter>  or  <F1>  for on-line help"),
	N_("type  :help version9<Enter>   for version info"),
	عدم,
	"",
	N_("Running in Vi compatible mode"),
	N_("type  :set nocp<Enter>        for Vim defaults"),
	N_("type  :help cp-default<Enter> for info on this"),
    };
#که_تعریف_شوی FEAT_GUI
    ساكن حرف	*(gui_کرښې[]) =
    {
	عدم,
	عدم,
	عدم,
	عدم,
# ifdef MODIFIED_BY
	عدم,
# endif
	عدم,
	عدم,
	عدم,
	N_("menu  Help->Orphans           for information    "),
	عدم,
	N_("Running modeless, typed text is inserted"),
	N_("menu  Edit->Global Settings->Toggle Insert Mode  "),
	N_("                              for two modes      "),
	عدم,
	عدم,
	عدم,
	N_("menu  Edit->Global Settings->Toggle Vi Compatible"),
	N_("                              for Vim defaults   "),
    };
#د_که_پای

    // blanklines = screen height - # message lines
    blanklines = (صحيح)لیکې - (ARRAY_اوږدتیا(کرښې) - 1);
    إذا (!p_cp)
	blanklines += 4;  // add 4 for not showing "Vi compatible" message

    // Don't overwrite a statusline.  Depends on 'cmdheight'.
    إذا (p_ls > 1)
	blanklines -= لیکې - topframe->fr_لوړوالی;
    إذا (blanklines < 0)
	blanklines = 0;

    // Show the sponsor and Uganda message two out of four times
    sponsor = (صحيح)وخت(عدم);
    sponsor = ((sponsor & 2) == 0) - ((sponsor & 4) == 0);

    // start displaying the message lines after half of the blank lines
    لیکه = blanklines / 2;
    إذا ((لیکه >= 2 && topframe->fr_پلنوالی >= 50) || colon)
    {
	لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(کرښې); ++i)
	{
	    p = کرښې[i];
#که_تعریف_شوی FEAT_GUI
	    إذا (p_im && gui.دننه_use && gui_کرښې[i] != عدم)
		p = gui_کرښې[i];
#د_که_پای
	    إذا (p == عدم)
	    {
		إذا (!p_cp)
		    اكسر;
		استمر;
	    }
	    إذا (sponsor != 0)
	    {
		إذا (مزي_پلټنه(p, "children") != عدم)
		    p = N_("Sponsor Vim development!");
		وإلا إذا (مزي_پلټنه(p, "Kuwasha") != عدم)
		    p = N_("type  :help sponsor<Enter>    for information ");
		وإلا إذا (مزي_پلټنه(p, "Orphans") != عدم)
		    p = N_("menu  Help->Sponsor           for information    ");
	    }
	    إذا (*p != NUL)
		do_intro_کرښه(لیکه, (char_u *)_(p), i == 2, 0);
	    ++لیکه;
	}
    }

    // Make the wait-return message appear just below the text.
    إذا (colon)
	پیغام_لیکه = لیکه;
}

    ساكن فراغ
do_intro_کرښه(
    صحيح		لیکه,
    char_u	*mesg,
    صحيح		جمع_کړه_نسخه,
    صحيح		attr)
{
    char_u	vers[20];
    صحيح		ستون;
    char_u	*p;
    صحيح		l;
    صحيح		clen;
#که_تعریف_شوی MODIFIED_BY
# define MODBY_LEN 150
    char_u	modby[MODBY_LEN];

    إذا (*mesg == ' ')
    {
	vim_strncpy(modby, (char_u *)_("Modified by "), MODBY_LEN - 1);
	l = (صحيح)STRLEN(modby);
	vim_strncpy(modby + l, (char_u *)MODIFIED_BY, MODBY_LEN - l - 1);
	mesg = modby;
    }
#د_که_پای

    // Center the message horizontally.
    ستون = vim_strsize(mesg);
    إذا (جمع_کړه_نسخه)
    {
	STRCPY(vers, medium_نسخه);
	إذا (highest_patch())
	{
	    // Check for 9.9x or 9.9xx, alpha/beta version
	    إذا (خوندي_isalpha((صحيح)vers[3]))
	    {
		صحيح len = (خوندي_isalpha((صحيح)vers[4])) ? 5 : 4;
		sprintf((حرف *)vers + len, ".%d%s", highest_patch(),
							 medium_نسخه + len);
	    }
	    وإلا
		sprintf((حرف *)vers + 3, ".%d", highest_patch());
	}
	ستون += (صحيح)STRLEN(vers);
    }
    ستون = (topframe->fr_پلنوالی - ستون) / 2;
    إذا (ستون < 0)
	ستون = 0;

    // Split up in parts to highlight <> items differently.
    لكل (p = mesg; *p != NUL; p += l)
    {
	clen = 0;
	لكل (l = 0; p[l] != NUL
			 && (l == 0 || (p[l] != '<' && p[l - 1] != '>')); ++l)
	{
	    إذا (has_mbyte)
	    {
		clen += ptr2cells(p + l);
		l += (*mb_ptr2len)(p + l) - 1;
	    }
	    وإلا
		clen += byte2cells(p[l]);
	}
	screen_puts_len(p, l, لیکه, ستون + firstwin->w_wincol,
		*p == '<' ? HL_ATTR(HLF_8) : attr);
	ستون += clen;
    }

    // Add the version number to the version line.
    إذا (جمع_کړه_نسخه)
	screen_puts(vers, لیکه, ستون + firstwin->w_wincol, 0);
}

/*
 * ":intro": clear screen, display intro screen and wait for return.
 */
    فراغ
ex_intro(exarg_T *eap ناکارول_شوی)
{
    screenclear();
#إذا defined(FEAT_TABPANEL)
    draw_tabpanel();
#د_که_پای
    intro_پیغام(TRUE);
    wait_return(TRUE);
}
