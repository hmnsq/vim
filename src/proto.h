/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * proto.h: include the (automatically generated) function prototypes
 */

/*
 * Don't include these while generating prototypes.  Prevents problems when
 * files are missing.
 */
#जइ !defined(PROTO) && !defined(NOPROTO)

/*
 * Machine-dependent routines.
 */
// avoid errors in function prototypes
# जइ !defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
#  define Display पूर्णाङ्क
#  define Widget पूर्णाङ्क
# endif
# ifndef FEAT_GUI_GTK
#  define GdkEvent पूर्णाङ्क
#  define Gdk_Event_कुञ्चिका पूर्णाङ्क
# endif
# ifndef FEAT_X11
#  define XImage पूर्णाङ्क
# endif

# ifdef AMIGA
#  include "os_amiga.pro"
# endif
# जइ defined(UNIX) || defined(VMS)
#  include "os_unix.pro"
# endif
# ifdef MSWIN
#  include "os_win32.pro"
#  include "os_mswin.pro"
#  include "winclip.pro"
#  जइ (defined(__GNUC__) && !defined(__MINGW32__))
बाह्य पूर्णाङ्क _stricoll(वर्ण *a, वर्ण *b);
#  endif
# endif
# ifdef VMS
#  include "os_vms.pro"
# endif
# ifdef __QNX__
#  include "os_qnx.pro"
# endif

# ifdef FEAT_CRYPT
#  include "blowfish.pro"
#  include "crypt.pro"
#  include "crypt_zip.pro"
# endif
# include "alloc.pro"
# include "arglist.pro"
# include "autocmd.pro"
# include "buffer.pro"
# include "bufwrite.pro"
# include "change.pro"
# include "charset.pro"
# include "cindent.pro"
# include "clientserver.pro"
# include "clipboard.pro"
# include "cmdexpand.pro"
# include "cmdhist.pro"
# include "if_cscope.pro"
# include "debugger.pro"
# include "dict.pro"
# include "diff.pro"
# include "linematch.pro"
# include "digraph.pro"
# include "drawline.pro"
# include "drawscreen.pro"
# include "edit.pro"
# include "eval.pro"
# include "evalbuffer.pro"
# include "evalfunc.pro"
# include "evalvars.pro"
# include "evalwindow.pro"
# include "ex_cmds.pro"
# include "ex_cmds2.pro"
# include "ex_docmd.pro"
# include "ex_eval.pro"
# include "ex_getln.pro"
# include "fileio.pro"
# include "filepath.pro"
# include "findfile.pro"
# include "float.pro"
# include "fold.pro"
# include "getchar.pro"
# include "gc.pro"
# include "gui_xim.pro"
# include "hardcopy.pro"
# ifdef FEAT_PRINT_PANGO
#  include "hardcopy_pango.pro"
# endif
# ifdef FEAT_POSTSCRIPT
#  include "hardcopy_postscript.pro"
# endif
# include "hashtab.pro"
# include "help.pro"
# include "highlight.pro"
# include "indent.pro"
# include "insexpand.pro"
# include "json.pro"
# include "list.pro"
# include "locale.pro"
# include "logfile.pro"
# include "blob.pro"
# include "main.pro"
# include "map.pro"
# include "mark.pro"
# include "match.pro"
# include "memfile.pro"
# include "memline.pro"
# ifdef FEAT_MENU
#  include "menu.pro"
# endif
# ifdef FEAT_ARABIC
#  include "arabic.pro"
# endif
# ifdef FEAT_VIMINFO
#  include "viminfo.pro"
# endif
# ifdef FEAT_TABPANEL
#  include "tabpanel.pro"
# endif

// These prototypes cannot be produced automatically.
पूर्णाङ्क smsg(ध्रुव वर्ण *, ...) ATTRIBUTE_COLD ATTRIBUTE_FORMAT_PRINTF(1, 2);

पूर्णाङ्क smsg_attr(पूर्णाङ्क, ध्रुव वर्ण *, ...) ATTRIBUTE_FORMAT_PRINTF(2, 3);

पूर्णाङ्क smsg_attr_keep(पूर्णाङ्क, ध्रुव वर्ण *, ...) ATTRIBUTE_FORMAT_PRINTF(2, 3);

// These prototypes cannot be produced automatically.
पूर्णाङ्क semsg(ध्रुव वर्ण *, ...) ATTRIBUTE_COLD ATTRIBUTE_FORMAT_PRINTF(1, 2);

// These prototypes cannot be produced automatically.
शून्य siemsg(ध्रुव वर्ण *, ...) ATTRIBUTE_COLD ATTRIBUTE_FORMAT_PRINTF(1, 2);

पूर्णाङ्क vim_snprintf_add(वर्ण *, size_t, ध्रुव वर्ण *, ...) ATTRIBUTE_FORMAT_PRINTF(3, 4);

पूर्णाङ्क vim_snprintf(वर्ण *, size_t, ध्रुव वर्ण *, ...) ATTRIBUTE_FORMAT_PRINTF(3, 4);
size_t vim_snprintf_safelen(वर्ण *, size_t, ध्रुव वर्ण *, ...) ATTRIBUTE_FORMAT_PRINTF(3, 4);

पूर्णाङ्क vim_vsnprintf(वर्ण *str, size_t str_m, ध्रुव वर्ण *fmt, va_list ap)
	ATTRIBUTE_FORMAT_PRINTF(3, 0);
पूर्णाङ्क vim_vsnprintf_typval(वर्ण *str, size_t str_m, ध्रुव वर्ण *fmt, va_list ap, typval_T *tvs)
	ATTRIBUTE_FORMAT_PRINTF(3, 0);

# include "message.pro"
# include "misc1.pro"
# include "misc2.pro"
# ifndef HAVE_STRPBRK	    // not generated automatically from misc2.c
char_u *vim_strpbrk(char_u *s, char_u *charset);
# endif
# ifndef HAVE_QSORT
// Use our own qsort(), don't define the prototype when not used.
शून्य qsort(शून्य *base, size_t elm_गणनम्, size_t elm_size, पूर्णाङ्क (*cmp)(ध्रुव शून्य *, ध्रुव शून्य *));
# endif
# include "mouse.pro"
# include "move.pro"
# include "mbyte.pro"
# ifdef VIMDLL
// Function name differs when VIMDLL is defined
पूर्णाङ्क mbyte_im_get_status(शून्य);
शून्य mbyte_im_set_active(पूर्णाङ्क active_arg);
# endif
# include "normal.pro"
# include "ops.pro"
# include "option.pro"
# include "optionstr.pro"
# include "popupmenu.pro"
# जइ defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
#  include "profiler.pro"
# endif
# include "quickfix.pro"
# ifdef FEAT_WAYLAND
#  include "wayland.pro"
# endif
# include "regexp.pro"
# include "register.pro"
# include "scriptfile.pro"
# include "screen.pro"
# include "session.pro"
# जइ defined(FEAT_CRYPT) || defined(FEAT_PERSISTENT_UNDO)
#  include "sha256.pro"
# endif
# include "fuzzy.pro"
# include "search.pro"
# ifdef FEAT_SIGNS
#  include "sign.pro"
# endif
# include "sound.pro"
# include "spell.pro"
# include "spellfile.pro"
# include "spellsuggest.pro"
# include "strings.pro"
# include "syntax.pro"
# include "tag.pro"
# include "term.pro"
# ifdef FEAT_TERMINAL
#  include "terminal.pro"
# endif
# जइ defined(HAVE_TGETENT) && (defined(AMIGA) || defined(VMS))
#  include "termlib.pro"
# endif
# ifdef FEAT_PROP_POPUP
#  include "popupwin.pro"
#  include "textprop.pro"
# endif
# ifdef FEAT_IMAGE
#  include "image.pro"
#  ifdef FEAT_IMAGE_KITTY
#   include "image_kitty.pro"
#  endif
#  ifdef FEAT_IMAGE_SIXEL
#   include "image_sixel.pro"
#  endif
# endif
# include "testing.pro"
# include "textobject.pro"
# include "textformat.pro"
# include "time.pro"
# include "tuple.pro"
# include "typval.pro"
# include "ui.pro"
# include "undo.pro"
# include "usercmd.pro"
# include "userfunc.pro"
# include "version.pro"
# include "vim9script.pro"
# ifdef FEAT_EVAL
// include vim9.h here, the types defined there are used by function arguments.
#  include "vim9.h"
#  include "vim9class.pro"
#  include "vim9cmds.pro"
#  include "vim9compile.pro"
#  include "vim9execute.pro"
#  include "vim9expr.pro"
#  include "vim9generics.pro"
#  include "vim9instr.pro"
#  include "vim9type.pro"
# endif
# include "window.pro"

# ifdef FEAT_LUA
#  include "if_lua.pro"
# endif

# ifdef FEAT_MZSCHEME
#  include "if_mzsch.pro"
# endif

# ifdef FEAT_PYTHON
#  include "if_python.pro"
# endif

# ifdef FEAT_PYTHON3
#  include "if_python3.pro"
# endif

# ifdef FEAT_TCL
#  include "if_tcl.pro"
# endif

# ifdef FEAT_RUBY
#  include "if_ruby.pro"
# endif

// Ugly solution for "BalloonEval" not being defined while it's used in some
// .pro files.
# ifndef FEAT_BEVAL
#  define BalloonEval पूर्णाङ्क
# endif
# जइ defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP)
#  include "beval.pro"
# endif

# ifdef FEAT_NETBEANS_INTG
#  include "netbeans.pro"
# endif
# ifdef FEAT_JOB_CHANNEL
#  include "job.pro"
#  include "channel.pro"
# endif
# ifdef FEAT_SOCKETSERVER
#  include "socketserver.pro"
# endif

# ifdef FEAT_EVAL
// Not generated automatically so that we can add an extra attribute.
शून्य ch_log(channel_T *ch, ध्रुव वर्ण *fmt, ...) ATTRIBUTE_FORMAT_PRINTF(2, 3);
शून्य ch_error(channel_T *ch, ध्रुव वर्ण *fmt, ...) ATTRIBUTE_FORMAT_PRINTF(2, 3);
# endif

# जइ defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)
#  जइ defined(UNIX) || defined(MACOS_X) || defined(VMS)
#   include "pty.pro"
#  endif
# endif

# ifdef FEAT_GUI
#  include "gui.pro"
#  जइ !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(VMS)
बाह्य पूर्णाङ्क putenv(ध्रुव वर्ण *string);			// in misc2.c
#   ifdef USE_VIMPTY_GETENV
बाह्य char_u *vimpty_getenv(ध्रुव char_u *string);	// in misc2.c
#   endif
#  endif
#  ifdef FEAT_GUI_MSWIN
#   include "gui_w32.pro"
#  endif
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
#    include "gui_gtk4.pro"
#   अण्णहा
#    include "gui_gtk.pro"
#    include "gui_gtk_x11.pro"
#   endif
#  endif
#  ifdef FEAT_GUI_MOTIF
#   include "gui_motif.pro"
#   include "gui_xmdlg.pro"
#  endif
#  ifdef FEAT_GUI_HAIKU
#   include "gui_haiku.pro"
#  endif
#  ifdef FEAT_GUI_X11
#   include "gui_x11.pro"
#  endif
#  ifdef FEAT_GUI_PHOTON
#   include "gui_photon.pro"
#  endif
# endif	// FEAT_GUI

# ifdef FEAT_OLE
#  include "if_ole.pro"
# endif
# जइ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
#  include "if_xcmdsrv.pro"
# endif

/*
 * The perl include files pollute the namespace, therefore proto.h must be
 * included before the perl include files.  But then CV is not defined, which
 * is used in if_perl.pro.  To get around this, the perl prototype files are
 * not included here for the perl files.  Use a dummy define for CV for the
 * other files.
 */
# जइ defined(FEAT_PERL) && !defined(IN_PERL_FILE)
#  define CV शून्य
#  include "if_perl.pro"
# endif

# ifdef MACOS_CONVERT
#  include "os_mac_conv.pro"
# endif
# ifdef MACOS_X
#  include "os_macosx.pro"
# endif
# जइ defined(MACOS_X_DARWIN) && defined(FEAT_CLIPBOARD) && !defined(FEAT_GUI)
// functions in os_macosx.m
शून्य clip_mch_lose_selection(Clipboard_T *cbd);
पूर्णाङ्क clip_mch_own_selection(Clipboard_T *cbd);
शून्य clip_mch_request_selection(Clipboard_T *cbd);
शून्य clip_mch_set_selection(Clipboard_T *cbd);
# endif
#endif // !PROTO && !NOPROTO
