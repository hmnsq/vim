/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#شامل "vim.h"

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

#شامل "version.h"

حرف		*نسخہ = VIM_نسخہ_SHORT;
ساکن حرف	*medium_نسخہ = VIM_نسخہ_MEDIUM;

#اگر defined(HAVE_DATE_وقت)
# اگر defined(VMS) && defined(VAXC)
حرف	long_نسخہ[حجم(VIM_نسخہ_LONG_DATE) + حجم(__DATE__)
						      + حجم(__TIME__) + 3];

    خالی
init_long_نسخہ(خالی)
{
    /*
     * Construct the long version string.  Necessary because
     * VAX C can't concatenate strings in the preprocessor.
     */
    strcpy(long_نسخہ, VIM_نسخہ_LONG_DATE);
#  ifdef BUILD_DATE
    strcat(long_نسخہ, BUILD_DATE);
#  ورنہ
    strcat(long_نسخہ, __DATE__);
    strcat(long_نسخہ, " ");
    strcat(long_نسخہ, __TIME__);
#  endif
    strcat(long_نسخہ, ")");
}

# ورنہ
حرف	*long_نسخہ = تہی;

    خالی
init_long_نسخہ(خالی)
{
    اگر (long_نسخہ != تہی)
	واپس;

#  ifdef BUILD_DATE
    حرف *date_وقت = BUILD_DATE;
#  ورنہ
    حرف *date_وقت = __DATE__ " " __TIME__;
#  endif
    حرف *پیغام = _("%s (%s, compiled %s)");
    size_t len = لڑی_لمبائی(پیغام)
	+ STRLEN_LITERAL(VIM_نسخہ_LONG_ONLY)
	+ STRLEN_LITERAL(VIM_نسخہ_DATE_ONLY)
	+ لڑی_لمبائی(date_وقت);

    long_نسخہ = alloc(len);
    اگر (long_نسخہ == تہی)
	long_نسخہ = VIM_نسخہ_LONG;
    ورنہ
	vim_snprintf(long_نسخہ, len, پیغام,
		VIM_نسخہ_LONG_ONLY, VIM_نسخہ_DATE_ONLY, date_وقت);
}
# endif
#ورنہ
حرف	*long_نسخہ = VIM_نسخہ_LONG;

    خالی
init_long_نسخہ(خالی)
{
    // nothing to do
}
#اگر_ختم

ساکن حرف *(features[]) =
{
#اگر_تعریف_شدہ HAVE_ACL
	"+acl",
#ورنہ
	"-acl",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_ARABIC
	"+arabic",
#ورنہ
	"-arabic",
#اگر_ختم
#اگر_تعریف_شدہ AMIGA		// only for Amiga systems
# ifdef FEAT_ARP
	"+ARP",
# ورنہ
	"-ARP",
# endif
#اگر_ختم
#اگر_تعریف_شدہ FEAT_AUTOCHDIR
	"+autochdir",
#ورنہ
	"-autochdir",
#اگر_ختم
	"+autocmd",
#اگر_تعریف_شدہ FEAT_AUTOSERVERNAME
	"+autoservername",
#ورنہ
	"-autoservername",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_BEVAL_GUI
	"+balloon_eval",
#ورنہ
	"-balloon_eval",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_BEVAL_رکن
	"+balloon_eval_term",
#ورنہ
	"-balloon_eval_term",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_BROWSE
	"+browse",
#ورنہ
	"-browse",
#اگر_ختم
	"++builtin_terms",
#اگر_تعریف_شدہ FEAT_BYTEOFF
	"+byte_offset",
#ورنہ
	"-byte_offset",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	"+channel",
#ورنہ
	"-channel",
#اگر_ختم
	"+cindent",
#اگر_تعریف_شدہ FEAT_CLIENTSERVER
	"+clientserver",
#ورنہ
	"-clientserver",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD
	"+clipboard",
#ورنہ
	"-clipboard",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
	"+clipboard_provider",
#ورنہ
	"-clipboard_provider",
#اگر_ختم
	"+cmdline_compl",
	"+cmdline_hist",
	"+cmdline_info",
	"+comments",
#اگر_تعریف_شدہ FEAT_CONCEAL
	"+conceal",
#ورنہ
	"-conceal",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CRYPT
	"+cryptv",
#ورنہ
	"-cryptv",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CSCOPE
	"+cscope",
#ورنہ
	"-cscope",
#اگر_ختم
	"+cursorbind",
#اگر_تعریف_شدہ CURSOR_SHAPE
	"+cursorshape",
#ورنہ
	"-cursorshape",
#اگر_ختم
#اگر defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
	"+dialog_con_gui",
#ورنہ
# اگر defined(FEAT_CON_DIALOG)
	"+dialog_con",
# ورنہ
#  اگر defined(FEAT_GUI_DIALOG)
	"+dialog_gui",
#  ورنہ
	"-dialog",
#  endif
# endif
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
	"+diff",
#ورنہ
	"-diff",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIGRAPHS
	"+digraphs",
#ورنہ
	"-digraphs",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
# ifdef FEAT_DIRECTX
	"+directx",
# ورنہ
	"-directx",
# endif
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DND
	"+dnd",
#ورنہ
	"-dnd",
#اگر_ختم
	"-ebcdic",
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
	"+emacs_tags",
#ورنہ
	"-emacs_tags",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
	"+eval",
#ورنہ
	"-eval",
#اگر_ختم
	"+ex_extra",
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	"+extra_search",
#ورنہ
	"-extra_search",
#اگر_ختم
	"-farsi",
	"+file_in_path",
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
	"+find_in_path",
#ورنہ
	"-find_in_path",
#اگر_ختم
	"+float",
#اگر_تعریف_شدہ FEAT_FOLDING
	"+folding",
#ورنہ
	"-folding",
#اگر_ختم
	"-footer",
	// only interesting on Unix systems
#اگر !defined(USE_SYSTEM) && defined(UNIX)
	"+fork()",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
	"+gettext/dyn",
# ورنہ
	"+gettext",
# endif
#ورنہ
	"-gettext",
#اگر_ختم
	"-hangul_input",
#اگر (defined(HAVE_ICONV_H) && defined(USE_ICONV)) || defined(DYNAMIC_ICONV)
# ifdef DYNAMIC_ICONV
	"+iconv/dyn",
# ورنہ
	"+iconv",
# endif
#ورنہ
	"-iconv",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_IMAGE
	"+image",
#ورنہ
	"-image",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
	"+image_popup",
#ورنہ
	"-image_popup",
#اگر_ختم
	"+insert_expand",
#اگر_تعریف_شدہ FEAT_IPV6
	"+ipv6",
#ورنہ
	"-ipv6",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	"+job",
#ورنہ
	"-job",
#اگر_ختم
	"+jumplist",
#اگر_تعریف_شدہ FEAT_KEYMAP
	"+keymap",
#ورنہ
	"-keymap",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
	"+lambda",
#ورنہ
	"-lambda",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LANGMAP
	"+langmap",
#ورنہ
	"-langmap",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LIBCALL
	"+libcall",
#ورنہ
	"-libcall",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
	"+linebreak",
#ورنہ
	"-linebreak",
#اگر_ختم
	"+lispindent",
	"+listcmds",
	"+localmap",
#اگر_تعریف_شدہ FEAT_LUA
# ifdef DYNAMIC_LUA
	"+lua/dyn",
# ورنہ
	"+lua",
# endif
#ورنہ
	"-lua",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MENU
	"+menu",
#ورنہ
	"-menu",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SESSION
	"+mksession",
#ورنہ
	"-mksession",
#اگر_ختم
	"+modify_fname",
	"+mouse",
#اگر defined(UNIX) || defined(VMS)
# ifdef FEAT_MOUSE_DEC
	"+mouse_dec",
# ورنہ
	"-mouse_dec",
# endif
# ifdef FEAT_MOUSE_GPM
#  ifdef DYNAMIC_GPM
	"+mouse_gpm/dyn",
#  ورنہ
	"+mouse_gpm",
#  endif
# ورنہ
	"-mouse_gpm",
# endif
# ifdef FEAT_MOUSE_JSB
	"+mouse_jsbterm",
# ورنہ
	"-mouse_jsbterm",
# endif
# ifdef FEAT_MOUSE_NET
	"+mouse_netterm",
# ورنہ
	"-mouse_netterm",
# endif
#اگر_ختم

#اگر_تعریف_شدہ __QNX__
# ifdef FEAT_MOUSE_PTERM
	"+mouse_pterm",
# ورنہ
	"-mouse_pterm",
# endif
#اگر_ختم

#اگر defined(UNIX) || defined(VMS)
	"+mouse_sgr",
# ifdef FEAT_SYSMOUSE
	"+mouse_sysmouse",
# ورنہ
	"-mouse_sysmouse",
# endif
# ifdef FEAT_MOUSE_URXVT
	"+mouse_urxvt",
# ورنہ
	"-mouse_urxvt",
# endif
	"+mouse_xterm",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
	"+mouseshape",
#ورنہ
	"-mouseshape",
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MBYTE_IME
# ifdef DYNAMIC_IME
	"+multi_byte_ime/dyn",
# ورنہ
	"+multi_byte_ime",
# endif
#ورنہ
	"+multi_byte",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MULTI_LANG
	"+multi_lang",
#ورنہ
	"-multi_lang",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MZSCHEME
# ifdef DYNAMIC_MZSCHEME
	"+mzscheme/dyn",
# ورنہ
	"+mzscheme",
# endif
#ورنہ
	"-mzscheme",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	"+netbeans_intg",
#ورنہ
	"-netbeans_intg",
#اگر_ختم
	"+num64",
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
# ifdef FEAT_OLE
	"+ole",
# ورنہ
	"-ole",
# endif
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
	"+packages",
#ورنہ
	"-packages",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PRINT_PANGO
	"+pango",
#ورنہ
	"-pango",
#اگر_ختم
	"+path_extra",
#اگر_تعریف_شدہ FEAT_PERL
# ifdef DYNAMIC_PERL
	"+perl/dyn",
# ورنہ
	"+perl",
# endif
#ورنہ
	"-perl",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
	"+persistent_undo",
#ورنہ
	"-persistent_undo",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	"+popupwin",
#ورنہ
	"-popupwin",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PRINTER
# ifdef FEAT_POSTSCRIPT
	"+postscript",
# ورنہ
	"-postscript",
# endif
	"+printer",
#ورنہ
	"-printer",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROFILE
	"+profile",
#ورنہ
	"-profile",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PYTHON
# ifdef DYNAMIC_PYTHON
	"+python/dyn",
# ورنہ
	"+python",
# endif
#ورنہ
	"-python",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PYTHON3
# ifdef DYNAMIC_PYTHON3
#  ifdef DYNAMIC_PYTHON3_STABLE_ABI
	"+python3/dyn-stable",
#  ورنہ
	"+python3/dyn",
#  endif
# ورنہ
	"+python3",
# endif
#ورنہ
	"-python3",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_QUICKFIX
	"+quickfix",
#ورنہ
	"-quickfix",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RELTIME
	"+reltime",
#ورنہ
	"-reltime",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	"+rightleft",
#ورنہ
	"-rightleft",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RUBY
# ifdef DYNAMIC_RUBY
	"+ruby/dyn",
# ورنہ
	"+ruby",
# endif
#ورنہ
	"-ruby",
#اگر_ختم
	"+scrollbind",
#اگر_تعریف_شدہ FEAT_SIGNS
	"+signs",
#ورنہ
	"-signs",
#اگر_ختم
	"+smartindent",
#اگر_تعریف_شدہ FEAT_SOCKETSERVER
	"+socketserver",
#ورنہ
	"-socketserver",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SODIUM
# ifdef DYNAMIC_SODIUM
	"+sodium/dyn",
# ورنہ
	"+sodium",
# endif
#ورنہ
	"-sodium",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SOUND
	"+sound",
#ورنہ
	"-sound",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SPELL
	"+spell",
#ورنہ
	"-spell",
#اگر_ختم
#اگر_تعریف_شدہ STARTUPTIME
	"+startuptime",
#ورنہ
	"-startuptime",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_STL_OPT
	"+statusline",
#ورنہ
	"-statusline",
#اگر_ختم
	"-sun_workshop",
#اگر_تعریف_شدہ FEAT_SYN_HL
	"+syntax",
#ورنہ
	"-syntax",
#اگر_ختم
	    // only interesting on Unix systems
#اگر defined(USE_SYSTEM) && defined(UNIX)
	"+system()",
#اگر_ختم
#اگر defined(FEAT_TABPANEL)
	"+tabpanel",
#ورنہ
	"-tabpanel",
#اگر_ختم
	"-tag_any_white",
	"+tag_binary",
	"-tag_old_static",
#اگر_تعریف_شدہ FEAT_TCL
# ifdef DYNAMIC_TCL
	"+tcl/dyn",
# ورنہ
	"+tcl",
# endif
#ورنہ
	"-tcl",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	"+termguicolors",
#ورنہ
	"-termguicolors",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
	"+terminal",
#ورنہ
	"-terminal",
#اگر_ختم
#اگر defined(UNIX)
// only Unix can have terminfo instead of termcap
# ifdef TERMINFO
	"+terminfo",
# ورنہ
	"-terminfo",
# endif
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
	"+termresponse",
#ورنہ
	"-termresponse",
#اگر_ختم
	"+textobjects",
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	"+textprop",
#ورنہ
	"-textprop",
#اگر_ختم
#اگر !defined(UNIX)
// unix always includes termcap support
# ifdef HAVE_TGETENT
	"+tgetent",
# ورنہ
	"-tgetent",
# endif
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TIMERS
	"+timers",
#ورنہ
	"-timers",
#اگر_ختم
	"+title",
#اگر_تعریف_شدہ FEAT_TOOLBAR
	"+toolbar",
#ورنہ
	"-toolbar",
#اگر_ختم
	"+user_commands",
#اگر_تعریف_شدہ FEAT_VARTABS
	"+vartabs",
#ورنہ
	"-vartabs",
#اگر_ختم
	"+vertsplit",
	"+vim9script",
#اگر_تعریف_شدہ FEAT_VIMINFO
	"+viminfo",
#ورنہ
	"-viminfo",
#اگر_ختم
	"+virtualedit",
	"+visual",
	"+visualextra",
	"+vreplace",
#اگر_تعریف_شدہ MSWIN
# ifdef FEAT_VTP
	"+vtp",
# ورنہ
	"-vtp",
# endif
#اگر_ختم
#اگر_تعریف_شدہ FEAT_WAYLAND
	"+wayland",
#ورنہ
	"-wayland",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD
	"+wayland_clipboard",
#ورنہ
	"-wayland_clipboard",
#اگر_ختم
	"+wildignore",
	"+wildmenu",
	"+windows",
#اگر_تعریف_شدہ FEAT_WRITEBACKUP
	"+writebackup",
#ورنہ
	"-writebackup",
#اگر_ختم
#اگر defined(UNIX) || defined(VMS)
# ifdef FEAT_X11
	"+X11",
# ورنہ
	"-X11",
# endif
#اگر_ختم
#اگر_تعریف_شدہ FEAT_XATTR
	"+xattr",
#ورنہ
	"-xattr",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_XFONTSET
	"+xfontset",
#ورنہ
	"-xfontset",
#اگر_ختم
#اگر_تعریف_شدہ FEAT_XIM
	"+xim",
#ورنہ
	"-xim",
#اگر_ختم
#اگر defined(MSWIN)
# ifdef FEAT_XPM_W32
	"+xpm_w32",
# ورنہ
	"-xpm_w32",
# endif
#وگرنہ_اگر defined(HAVE_XPM)
	"+xpm",
#ورنہ
	"-xpm",
#اگر_ختم
#اگر defined(UNIX) || defined(VMS)
# اگر defined(USE_XSMP_INTERACT)
	"+xsmp_interact",
# elif defined(USE_XSMP)
	"+xsmp",
# ورنہ
	"-xsmp",
# endif
# ifdef FEAT_XCLIPBOARD
	"+xterm_clipboard",
# ورنہ
	"-xterm_clipboard",
# endif
#اگر_ختم
#اگر_تعریف_شدہ FEAT_XTERM_SAVE
	"+xterm_save",
#ورنہ
	"-xterm_save",
#اگر_ختم
	تہی
};

ساکن صحیح included_patches[] =
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
ساکن حرف *(extra_patches[]) =
{   /* Add your patch description below this line */
/**/
    تہی
};

    صحیح
highest_patch(خالی)
{
    // this relies on the highest patch number to be the first entry
    واپس included_patches[0];
}

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE if patch "n" has been included.
 */
    صحیح
has_patch(صحیح n)
{
    صحیح		h, m, l;

    // Perform a binary search.
    l = 0;
    h = (صحیح)ARRAY_لمبائی(included_patches) - 1;
    برائے (;;)
    {
	m = (l + h) / 2;
	اگر (included_patches[m] == n)
	    واپس TRUE;
	اگر (l == h)
	    توڑو;
	اگر (included_patches[m] < n)
	    h = m;
	ورنہ
	    l = m + 1;
    }
    واپس FALSE;
}
#اگر_ختم

    خالی
ex_نسخہ(exarg_T *eap)
{
    /*
     * Ignore a ":version 9.99" command.
     */
    اگر (*eap->arg == NUL)
    {
	پیغام_putchar('\n');
	list_نسخہ();
    }
}

/*
 * Output a string for the version message.  If it's going to wrap, output a
 * newline, unless the message is too long to fit on the screen anyway.
 * When "wrap" is TRUE wrap the string in [].
 */
    ساکن خالی
نسخہ_پیغام_wrap(char_u *s, صحیح wrap)
{
    صحیح		len = vim_strsize(s) + (wrap ? 2 : 0);

    اگر (!got_int && len < cmdline_چوڑائی && پیغام_ستون + len >= cmdline_چوڑائی
								&& *s != '\n')
	پیغام_putchar('\n');
    اگر (!got_int)
    {
	اگر (wrap)
	    پیغام_puts("[");
	پیغام_puts((حرف *)s);
	اگر (wrap)
	    پیغام_puts("]");
    }
}

    ساکن خالی
نسخہ_پیغام(حرف *s)
{
    نسخہ_پیغام_wrap((char_u *)s, FALSE);
}

/*
 * List all features aligned in columns, dictionary style.
 */
    ساکن خالی
list_features(خالی)
{
    list_اندر_columns((char_u **)features, -1, -1);
}

/*
 * List string items nicely aligned in columns.
 * When "size" is < 0 then the last entry is marked with NULL.
 * The entry with index "current" is inclosed in [].
 */
    خالی
list_اندر_columns(char_u **اشیاء, صحیح سائز, صحیح current)
{
    صحیح		i;
    صحیح		ncol;
    صحیح		nrow;
    صحیح		cur_صف = 1;
    صحیح		شے_گنتی = 0;
    صحیح		چوڑائی = 0;
#اگر_تعریف_شدہ FEAT_SYN_HL
    صحیح		use_highlight = (اشیاء == (char_u **)features);
#اگر_ختم

    // Find the length of the longest item, use that + 1 as the column
    // width.
    برائے (i = 0; سائز < 0 ? اشیاء[i] != تہی : i < سائز; ++i)
    {
	صحیح l = vim_strsize(اشیاء[i]) + (i == current ? 2 : 0);

	اگر (l > چوڑائی)
	    چوڑائی = l;
	++شے_گنتی;
    }
    چوڑائی += 1;

    اگر (cmdline_چوڑائی < چوڑائی)
    {
	// Not enough screen columns - show one per line
	برائے (i = 0; i < شے_گنتی; ++i)
	{
	    نسخہ_پیغام_wrap(اشیاء[i], i == current);
	    اگر (پیغام_ستون > 0 && i < شے_گنتی - 1)
		پیغام_putchar('\n');
	}
	واپس;
    }

    // The rightmost column doesn't need a separator.
    // Sacrifice it to fit in one more column if possible.
    ncol = (cmdline_چوڑائی + 1) / چوڑائی;
    nrow = شے_گنتی / ncol + ((شے_گنتی % ncol) ? 1 : 0);

    // "i" counts columns then rows.  "idx" counts rows then columns.
    برائے (i = 0; !got_int && i < nrow * ncol; ++i)
    {
	صحیح idx = (i / ncol) + (i % ncol) * nrow;

	اگر (idx < شے_گنتی)
	{
	    صحیح آخری_ستون = (i + 1) % ncol == 0;

	    اگر (idx == current)
		پیغام_putchar('[');
#اگر_تعریف_شدہ FEAT_SYN_HL
	    اگر (use_highlight && اشیاء[idx][0] == '-')
		پیغام_puts_attr((حرف *)اشیاء[idx], HL_ATTR(HLF_W));
	    ورنہ
#اگر_ختم
		پیغام_puts((حرف *)اشیاء[idx]);
	    اگر (idx == current)
		پیغام_putchar(']');
	    اگر (آخری_ستون)
	    {
		اگر (پیغام_ستون > 0 && cur_صف < nrow)
		    پیغام_putchar('\n');
		++cur_صف;
	    }
	    ورنہ
	    {
		جب (پیغام_ستون % چوڑائی)
		    پیغام_putchar(' ');
	    }
	}
	ورنہ
	{
	    // this row is out of items, thus at the end of the row
	    اگر (پیغام_ستون > 0)
	    {
		اگر (cur_صف < nrow)
		    پیغام_putchar('\n');
		++cur_صف;
	    }
	}
    }
}

    خالی
list_نسخہ(خالی)
{
    صحیح		i;
    صحیح		پہلا;
    حرف	*s = "";

    /*
     * When adding features here, don't forget to update the list of
     * internal variables in eval.c!
     */
    init_long_نسخہ();
    پیغام(long_نسخہ);
#اگر_تعریف_شدہ MSWIN
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
#   ifdef _WIN64
#    اگر defined(_M_ARM64) || defined(_M_ARM64EC)
     پیغام_puts(_("\nMS-Windows ARM64 GUI/console version"));
#    ورنہ
     پیغام_puts(_("\nMS-Windows 64-bit GUI/console version"));
#    endif
#   ورنہ
    پیغام_puts(_("\nMS-Windows 32-bit GUI/console version"));
#   endif
#  ورنہ
#   ifdef _WIN64
#    اگر defined(_M_ARM64) || defined(_M_ARM64EC)
     پیغام_puts(_("\nMS-Windows ARM64 GUI version"));
#    ورنہ
     پیغام_puts(_("\nMS-Windows 64-bit GUI version"));
#    endif
#   ورنہ
    پیغام_puts(_("\nMS-Windows 32-bit GUI version"));
#   endif
#  endif
#  ifdef FEAT_OLE
    پیغام_puts(_(" with OLE support"));
#  endif
# ورنہ
#  ifdef _WIN64
#   اگر defined(_M_ARM64) || defined(_M_ARM64EC)
    پیغام_puts(_("\nMS-Windows ARM64 console version"));
#   ورنہ
    پیغام_puts(_("\nMS-Windows 64-bit console version"));
#   endif
#  ورنہ
    پیغام_puts(_("\nMS-Windows 32-bit console version"));
#  endif
# endif
#اگر_ختم
#اگر defined(MACOS_X)
# اگر defined(MACOS_X_DARWIN)
    پیغام_puts(_("\nmacOS version"));
# ورنہ
    پیغام_puts(_("\nmacOS version w/o darwin feat."));
# endif
# اگر defined(__arm64__)
    پیغام_puts(" - arm64");
# elif defined(__x86_64__)
    پیغام_puts(" - x86_64");
# endif
#اگر_ختم

#اگر_تعریف_شدہ VMS
    پیغام_puts(_("\nOpenVMS (build) arch, version"));
# ifdef HAVE_PATHDEF
    اگر (*compiled_arch != NUL)
    {
	پیغام_puts(": ");
	پیغام_puts((حرف *)compiled_arch);
	اگر (*compiled_vers != NUL)
	{
	    پیغام_puts(", ");
	    پیغام_puts((حرف *)compiled_vers);
	}
    }

# endif

#اگر_ختم

    // Print the list of patch numbers if there is at least one.
    // Print a range when patches are consecutive: "1-10, 12, 15-40, 42-45"
    اگر (included_patches[0] != 0)
    {
	پیغام_puts(_("\nIncluded patches: "));
	پہلا = -1;
	i = (صحیح)ARRAY_لمبائی(included_patches) - 1;
	جب (--i >= 0)
	{
	    اگر (پہلا < 0)
		پہلا = included_patches[i];
	    اگر (i == 0 || included_patches[i - 1] != included_patches[i] + 1)
	    {
		پیغام_puts(s);
		s = ", ";
		پیغام_outnum((طویل)پہلا);
		اگر (پہلا != included_patches[i])
		{
		    پیغام_puts("-");
		    پیغام_outnum((طویل)included_patches[i]);
		}
		پہلا = -1;
	    }
	}
    }

    // Print the list of extra patch descriptions if there is at least one.
    اگر (extra_patches[0] != تہی)
    {
	پیغام_puts(_("\nExtra patches: "));
	s = "";
	برائے (i = 0; extra_patches[i] != تہی; ++i)
	{
	    پیغام_puts(s);
	    s = ", ";
	    پیغام_puts(extra_patches[i]);
	}
    }

#اگر_تعریف_شدہ MODIFIED_BY
    پیغام_puts("\n");
    پیغام_puts(_("Modified by "));
    پیغام_puts(MODIFIED_BY);
#اگر_ختم

#اگر_تعریف_شدہ HAVE_PATHDEF
    اگر (*compiled_user != NUL || *compiled_sys != NUL)
    {
	پیغام_puts(_("\nCompiled "));
	اگر (*compiled_user != NUL)
	{
	    پیغام_puts(_("by "));
	    پیغام_puts((حرف *)compiled_user);
	}
	اگر (*compiled_sys != NUL)
	{
	    پیغام_puts("@");
	    پیغام_puts((حرف *)compiled_sys);
	}
    }
#اگر_ختم

#اگر defined(FEAT_HUGE)
    پیغام_puts(_("\nHuge version "));
#وگرنہ_اگر defined(FEAT_معمول)
    پیغام_puts(_("\nNormal version "));
#ورنہ
    پیغام_puts(_("\nTiny version "));
#اگر_ختم
#اگر !defined(FEAT_GUI)
    پیغام_puts(_("without GUI."));
#وگرنہ_اگر defined(FEAT_GUI_GTK)
# اگر defined(USE_GTK4)
    پیغام_puts(_("with GTK4 GUI."));
# elif defined(USE_GTK3)
    پیغام_puts(_("with GTK3 GUI."));
# elif defined(FEAT_GUI_GNOME)
    پیغام_puts(_("with GTK2-GNOME GUI."));
# ورنہ
    پیغام_puts(_("with GTK2 GUI."));
# endif
#وگرنہ_اگر defined(FEAT_GUI_MOTIF)
    پیغام_puts(_("with X11-Motif GUI."));
#وگرنہ_اگر defined(FEAT_GUI_HAIKU)
    پیغام_puts(_("with Haiku GUI."));
#وگرنہ_اگر defined(FEAT_GUI_PHOTON)
    پیغام_puts(_("with Photon GUI."));
#وگرنہ_اگر defined(MSWIN)
    پیغام_puts(_("with GUI."));
#اگر_ختم
    نسخہ_پیغام(_("  Features included (+) or not (-):\n"));

    list_features();
    اگر (پیغام_ستون > 0)
	پیغام_putchar('\n');

#اگر_تعریف_شدہ SYS_VIMRC_FILE
    نسخہ_پیغام(_("   system vimrc file: \""));
    نسخہ_پیغام(SYS_VIMRC_FILE);
    نسخہ_پیغام("\"\n");
#اگر_ختم
#اگر_تعریف_شدہ USR_VIMRC_FILE
    نسخہ_پیغام(_("     user vimrc file: \""));
    نسخہ_پیغام(USR_VIMRC_FILE);
    نسخہ_پیغام("\"\n");
#اگر_ختم
#اگر_تعریف_شدہ USR_VIMRC_FILE2
    نسخہ_پیغام(_(" 2nd user vimrc file: \""));
    نسخہ_پیغام(USR_VIMRC_FILE2);
    نسخہ_پیغام("\"\n");
#اگر_ختم
#اگر defined(USR_VIMRC_FILE3) && defined(XDG_VIMRC_FILE)
    نسخہ_پیغام(_(" 3rd user vimrc file: \""));
    نسخہ_پیغام(USR_VIMRC_FILE3);
    نسخہ_پیغام("\"\n");
    نسخہ_پیغام(_(" 4th user vimrc file: \""));
    نسخہ_پیغام((حرف *)(XDG_VIMRC_FILE));
    نسخہ_پیغام("\"\n");
#وگرنہ_اگر defined(USR_VIMRC_FILE3)
    نسخہ_پیغام(_(" 3rd user vimrc file: \""));
    نسخہ_پیغام(USR_VIMRC_FILE3);
    نسخہ_پیغام("\"\n");
#وگرنہ_اگر defined(XDG_VIMRC_FILE)
    نسخہ_پیغام(_(" 3rd user vimrc file: \""));
    نسخہ_پیغام((حرف *)(XDG_VIMRC_FILE));
    نسخہ_پیغام("\"\n");
#اگر_ختم
#اگر_تعریف_شدہ USR_EXRC_FILE
    نسخہ_پیغام(_("      user exrc file: \""));
    نسخہ_پیغام(USR_EXRC_FILE);
    نسخہ_پیغام("\"\n");
#اگر_ختم
#اگر_تعریف_شدہ USR_EXRC_FILE2
    نسخہ_پیغام(_("  2nd user exrc file: \""));
    نسخہ_پیغام(USR_EXRC_FILE2);
    نسخہ_پیغام("\"\n");
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
# ifdef SYS_GVIMRC_FILE
    نسخہ_پیغام(_("  system gvimrc file: \""));
    نسخہ_پیغام(SYS_GVIMRC_FILE);
    نسخہ_پیغام("\"\n");
# endif
    نسخہ_پیغام(_("    user gvimrc file: \""));
    نسخہ_پیغام(USR_GVIMRC_FILE);
    نسخہ_پیغام("\"\n");
# ifdef USR_GVIMRC_FILE2
    نسخہ_پیغام(_("2nd user gvimrc file: \""));
    نسخہ_پیغام(USR_GVIMRC_FILE2);
    نسخہ_پیغام("\"\n");
# endif
# ifdef USR_GVIMRC_FILE3
    نسخہ_پیغام(_("3rd user gvimrc file: \""));
    نسخہ_پیغام(USR_GVIMRC_FILE3);
    نسخہ_پیغام("\"\n");
# endif
#اگر_ختم
    نسخہ_پیغام(_("       defaults file: \""));
    نسخہ_پیغام(VIM_DEFAULTS_FILE);
    نسخہ_پیغام("\"\n");
#اگر_تعریف_شدہ FEAT_GUI
# ifdef SYS_MENU_FILE
    نسخہ_پیغام(_("    system menu file: \""));
    نسخہ_پیغام(SYS_MENU_FILE);
    نسخہ_پیغام("\"\n");
# endif
#اگر_ختم
#اگر_تعریف_شدہ HAVE_PATHDEF
    اگر (*default_vim_dir != NUL)
    {
	نسخہ_پیغام(_("  fall-back for $VIM: \""));
	نسخہ_پیغام((حرف *)default_vim_dir);
	نسخہ_پیغام("\"\n");
    }
    اگر (*default_vimruntime_dir != NUL)
    {
	نسخہ_پیغام(_(" f-b for $VIMRUNTIME: \""));
	نسخہ_پیغام((حرف *)default_vimruntime_dir);
	نسخہ_پیغام("\"\n");
    }
    نسخہ_پیغام(_("Compilation: "));
    نسخہ_پیغام((حرف *)all_cflags);
    نسخہ_پیغام("\n");
# ifdef VMS
    اگر (*compiler_نسخہ != NUL)
    {
	نسخہ_پیغام(_("Compiler: "));
	نسخہ_پیغام((حرف *)compiler_نسخہ);
	نسخہ_پیغام("\n");
    }
# endif
    نسخہ_پیغام(_("Linking: "));
    نسخہ_پیغام((حرف *)all_lflags);
#اگر_ختم
#اگر_تعریف_شدہ DEBUG
    نسخہ_پیغام("\n");
    نسخہ_پیغام(_("  DEBUG BUILD"));
#اگر_ختم
}

ساکن خالی do_intro_سطر(صحیح صف, char_u *mesg, صحیح جوڑو_نسخہ, صحیح attr);
ساکن خالی intro_پیغام(صحیح colon);

/*
 * Show the intro message when not editing a file.
 */
    خالی
maybe_intro_پیغام(خالی)
{
    اگر (BUFEMPTY()
	    && curbuf->b_fname == تہی
	    && firstwin->w_اگلا == تہی
	    && vim_strchr(p_shm, SHM_INTRO) == تہی)
	intro_پیغام(FALSE);
}

/*
 * Give an introductory message about Vim.
 * Only used when starting Vim on an empty file, without a file name.
 * Or with the ":intro" command (for Sven :-).
 */
    ساکن خالی
intro_پیغام(
    صحیح		colon)		// TRUE for ":intro"
{
    صحیح		i;
    صحیح		صف;
    صحیح		blanklines;
    صحیح		sponsor;
    حرف	*p;
    ساکن حرف	*(سطریں[]) =
    {
	N_("VIM - Vi IMproved"),
	"",
	N_("version "),
	N_("by Bram Moolenaar et al."),
#اگر_تعریف_شدہ MODIFIED_BY
	" ",
#اگر_ختم
	N_("Vim is open source and freely distributable"),
	"",
	N_("Help poor children in Uganda!"),
	N_("type  :help Kuwasha<Enter>    for information "),
	"",
	N_("type  :q<Enter>               to exit         "),
	N_("type  :help<Enter>  or  <F1>  for on-line help"),
	N_("type  :help version9<Enter>   for version info"),
	تہی,
	"",
	N_("Running in Vi compatible mode"),
	N_("type  :set nocp<Enter>        for Vim defaults"),
	N_("type  :help cp-default<Enter> for info on this"),
    };
#اگر_تعریف_شدہ FEAT_GUI
    ساکن حرف	*(gui_سطریں[]) =
    {
	تہی,
	تہی,
	تہی,
	تہی,
# ifdef MODIFIED_BY
	تہی,
# endif
	تہی,
	تہی,
	تہی,
	N_("menu  Help->Orphans           for information    "),
	تہی,
	N_("Running modeless, typed text is inserted"),
	N_("menu  Edit->Global Settings->Toggle Insert Mode  "),
	N_("                              for two modes      "),
	تہی,
	تہی,
	تہی,
	N_("menu  Edit->Global Settings->Toggle Vi Compatible"),
	N_("                              for Vim defaults   "),
    };
#اگر_ختم

    // blanklines = screen height - # message lines
    blanklines = (صحیح)صفیں - (ARRAY_لمبائی(سطریں) - 1);
    اگر (!p_cp)
	blanklines += 4;  // add 4 for not showing "Vi compatible" message

    // Don't overwrite a statusline.  Depends on 'cmdheight'.
    اگر (p_ls > 1)
	blanklines -= صفیں - topframe->fr_اونچائی;
    اگر (blanklines < 0)
	blanklines = 0;

    // Show the sponsor and Uganda message two out of four times
    sponsor = (صحیح)وقت(تہی);
    sponsor = ((sponsor & 2) == 0) - ((sponsor & 4) == 0);

    // start displaying the message lines after half of the blank lines
    صف = blanklines / 2;
    اگر ((صف >= 2 && topframe->fr_چوڑائی >= 50) || colon)
    {
	برائے (i = 0; i < (صحیح)ARRAY_لمبائی(سطریں); ++i)
	{
	    p = سطریں[i];
#اگر_تعریف_شدہ FEAT_GUI
	    اگر (p_im && gui.اندر_use && gui_سطریں[i] != تہی)
		p = gui_سطریں[i];
#اگر_ختم
	    اگر (p == تہی)
	    {
		اگر (!p_cp)
		    توڑو;
		جاری;
	    }
	    اگر (sponsor != 0)
	    {
		اگر (لڑی_تلاش(p, "children") != تہی)
		    p = N_("Sponsor Vim development!");
		ورنہ اگر (لڑی_تلاش(p, "Kuwasha") != تہی)
		    p = N_("type  :help sponsor<Enter>    for information ");
		ورنہ اگر (لڑی_تلاش(p, "Orphans") != تہی)
		    p = N_("menu  Help->Sponsor           for information    ");
	    }
	    اگر (*p != NUL)
		do_intro_سطر(صف, (char_u *)_(p), i == 2, 0);
	    ++صف;
	}
    }

    // Make the wait-return message appear just below the text.
    اگر (colon)
	پیغام_صف = صف;
}

    ساکن خالی
do_intro_سطر(
    صحیح		صف,
    char_u	*mesg,
    صحیح		جوڑو_نسخہ,
    صحیح		attr)
{
    char_u	vers[20];
    صحیح		ستون;
    char_u	*p;
    صحیح		l;
    صحیح		clen;
#اگر_تعریف_شدہ MODIFIED_BY
# define MODBY_LEN 150
    char_u	modby[MODBY_LEN];

    اگر (*mesg == ' ')
    {
	vim_strncpy(modby, (char_u *)_("Modified by "), MODBY_LEN - 1);
	l = (صحیح)STRLEN(modby);
	vim_strncpy(modby + l, (char_u *)MODIFIED_BY, MODBY_LEN - l - 1);
	mesg = modby;
    }
#اگر_ختم

    // Center the message horizontally.
    ستون = vim_strsize(mesg);
    اگر (جوڑو_نسخہ)
    {
	STRCPY(vers, medium_نسخہ);
	اگر (highest_patch())
	{
	    // Check for 9.9x or 9.9xx, alpha/beta version
	    اگر (محفوظ_isalpha((صحیح)vers[3]))
	    {
		صحیح len = (محفوظ_isalpha((صحیح)vers[4])) ? 5 : 4;
		sprintf((حرف *)vers + len, ".%d%s", highest_patch(),
							 medium_نسخہ + len);
	    }
	    ورنہ
		sprintf((حرف *)vers + 3, ".%d", highest_patch());
	}
	ستون += (صحیح)STRLEN(vers);
    }
    ستون = (topframe->fr_چوڑائی - ستون) / 2;
    اگر (ستون < 0)
	ستون = 0;

    // Split up in parts to highlight <> items differently.
    برائے (p = mesg; *p != NUL; p += l)
    {
	clen = 0;
	برائے (l = 0; p[l] != NUL
			 && (l == 0 || (p[l] != '<' && p[l - 1] != '>')); ++l)
	{
	    اگر (has_mbyte)
	    {
		clen += ptr2cells(p + l);
		l += (*mb_ptr2len)(p + l) - 1;
	    }
	    ورنہ
		clen += byte2cells(p[l]);
	}
	screen_puts_len(p, l, صف, ستون + firstwin->w_wincol,
		*p == '<' ? HL_ATTR(HLF_8) : attr);
	ستون += clen;
    }

    // Add the version number to the version line.
    اگر (جوڑو_نسخہ)
	screen_puts(vers, صف, ستون + firstwin->w_wincol, 0);
}

/*
 * ":intro": clear screen, display intro screen and wait for return.
 */
    خالی
ex_intro(exarg_T *eap غیر_مستعمل)
{
    screenclear();
#اگر defined(FEAT_TABPANEL)
    draw_tabpanel();
#اگر_ختم
    intro_پیغام(TRUE);
    wait_return(TRUE);
}
