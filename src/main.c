/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#تعریف_کرو EXTERN
#شامل "vim.h"

#اگر_تعریف_شدہ __CYGWIN__
# include <cygwin/version.h>
# include <sys/cygwin.h>	// for cygwin_conv_to_posix_path() and/or
				// cygwin_conv_path()
# include <limits.h>
#اگر_ختم

#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# include "iscygpty.h"
#اگر_ختم

// Values for edit_type.
#تعریف_کرو EDIT_NONE   0	    // no edit type yet
#تعریف_کرو EDIT_FILE   1	    // file name argument[s] given, use argument list
#تعریف_کرو EDIT_STDIN  2	    // read file from stdin
#تعریف_کرو EDIT_لیبل    3	    // tag name argument given, use tagname
#تعریف_کرو EDIT_QF	    4	    // start in quickfix mode

#اگر (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
ساکن صحیح file_owned(حرف *fname);
#اگر_ختم
ساکن خالی mainerr(صحیح, char_u *);
ساکن خالی early_arg_جائزہ(mparm_T *parmp);
#اگر_غیر_تعریف_شدہ NO_VIM_MAIN
ساکن خالی usage(خالی);
ساکن خالی parse_command_name(mparm_T *parmp);
ساکن خالی command_سطر_جائزہ(mparm_T *parmp);
ساکن خالی check_tty(mparm_T *parmp);
ساکن خالی read_stdin(خالی);
ساکن خالی create_windows(mparm_T *parmp);
ساکن خالی edit_buffers(mparm_T *parmp, char_u *cwd);
ساکن خالی exe_pre_commands(mparm_T *parmp);
ساکن خالی exe_commands(mparm_T *parmp);
ساکن خالی ماخذ_startup_scripts(mparm_T *parmp);
ساکن خالی main_آغاز_gui(خالی);
ساکن خالی check_تبادلہ_exists_action(خالی);
# ifdef FEAT_EVAL
ساکن خالی سیٹ_progpath(char_u *argv0);
# endif
#اگر_ختم


/*
 * Different types of error messages.
 */
ساکن حرف *(main_errors[]) =
{
    N_("Unknown option argument"),
#تعریف_کرو ME_UNKNOWN_OPTION	0
    N_("Too many edit arguments"),
#تعریف_کرو ME_TOO_MANY_دلائل	1
    N_("Argument missing after"),
#تعریف_کرو ME_ARG_MISSING		2
    N_("Garbage after option argument"),
#تعریف_کرو ME_GARBAGE		3
    N_("Too many \"+command\", \"-c command\" or \"--cmd command\" arguments"),
#تعریف_کرو ME_EXTRA_CMD		4
    N_("Invalid argument for"),
#تعریف_کرو ME_INVALID_ARG		5
};

#اگر_غیر_تعریف_شدہ PROTO		// don't want a prototype for main()

// Various parameters passed between main() and other functions.
ساکن mparm_T	params;

# ifdef _IOLBF
ساکن خالی *s_vbuf = تہی;		// buffer for setvbuf()
# endif

# ifndef NO_VIM_MAIN	// skip this for unittests

ساکن char_u *آغاز_dir = تہی;	// current working dir on startup

ساکن صحیح has_dash_c_arg = FALSE;

#  ifdef VIMDLL
__declspec(dllexport)
#  endif
    صحیح
#  ifdef MSWIN
VimMain
#  ورنہ
مین
#  endif
(صحیح argc, حرف **argv)
{
#  اگر defined(STARTUPTIME) || defined(CLEAN_RUNTIMEPATH)
    صحیح		i;
#  endif

    /*
     * Do any system-specific initialisations.  These can NOT use IObuff or
     * NameBuff.  Thus emsg2() cannot be called!
     */
    mch_early_init();

#  ifdef MSWIN
    /*
     * MinGW expands command line arguments, which confuses our code to
     * convert when 'encoding' changes.  Get the unexpanded arguments.
     */
    argc = get_cmd_دلائل_W(&argv);
#  endif

    // Many variables are in "params" so that we can pass them to invoked
    // functions without a lot of arguments.  "argc" and "argv" are also
    // copied, so that they can be changed.
    CLEAR_میدان(params);
    params.argc = argc;
    params.argv = argv;
    params.want_بھرا_screen = TRUE;
#  ifdef FEAT_EVAL
    params.use_debug_break_level = -1;
#  endif
    params.کھڑکی_گنتی = -1;

    autocmd_init();

#  ifdef FEAT_RUBY
    {
	صحیح ruby_ڈھیر_آغاز;
	vim_ruby_init((خالی *)&ruby_ڈھیر_آغاز);
    }
#  endif

#  ifdef FEAT_TCL
    vim_tcl_init(params.argv[0]);
#  endif

#  ifdef MEM_PROFILE
    atexit(vim_mem_profile_dump);
#  endif

    /*
     * Various initialisations #1 shared with tests.
     */
    common_init_1();

#  اگر defined(STARTUPTIME) || defined(FEAT_کام_CHANNEL)
    // Need to find "--startuptime" and "--log" before actually parsing
    // arguments.
    برائے (i = 1; i < argc - 1; ++i)
    {
#   ifdef STARTUPTIME
	اگر (STRICMP(argv[i], "--startuptime") == 0 && وقت_fd == تہی)
	{
	    وقت_fd = mch_fopen(argv[i + 1], "a");
	    وقت_پیغام("--- VIM STARTING ---");
	}
#   endif
#   ifdef FEAT_EVAL
	اگر (STRICMP(argv[i], "--log") == 0)
	    ch_logfile((char_u *)(argv[i + 1]), (char_u *)"ao");
#   endif
    }
#  endif

#  ifdef CLEAN_RUNTIMEPATH
    // Need to find "--clean" before actually parsing arguments.
    برائے (i = 1; i < argc; ++i)
	اگر (STRICMP(argv[i], "--clean") == 0)
	{
	    params.clean = TRUE;
	    توڑو;
	}
#  endif
#  ifdef MSWIN
    // Need to find "-register" and "-unregister" before loading any libraries.
    برائے (i = 1; i < argc; ++i)
	اگر ((STRICMP(argv[i] + 1, "register") == 0
				    || STRICMP(argv[i] + 1, "unregister") == 0)
		&& (argv[i][0] == '-' || argv[i][0] == '/'))
	{
	    ملا_رجسٹر_arg = TRUE;
	    توڑو;
	}
#  endif

    /*
     * Various initialisations #2 shared with tests.
     */
    common_init_2(&params);

#  ifdef VIMDLL
    // Check if the current executable file is for the GUI subsystem.
    gui.starting = mch_is_gui_executable();
#  elif defined(FEAT_GUI_MSWIN)
    gui.starting = true;
#  endif

#  ifdef FEAT_CLIENTSERVER
    /*
     * Do the client-server stuff, unless "--servername ''" was used.
     * This may exit Vim if the command was sent to the server.
     */
    exec_on_server(&params);
#  endif

    /*
     * Figure out the way to work from the command name argv[0].
     * "vimdiff" starts diff mode, "rvim" sets "restricted", etc.
     */
    parse_command_name(&params);

    /*
     * Process the command line arguments.  File names are put in the global
     * argument list "global_alist".
     */
    command_سطر_جائزہ(&params);
    وقت_پیغام("parsing arguments");

    /*
     * On some systems, when we compile with the GUI, we always use it.  On Mac
     * there is no terminal version, and on Windows we can't fork one off with
     * :gui.
     */
#  ifdef ALWAYS_USE_GUI
    gui.starting = true;
#  ورنہ
#   اگر defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    /*
     * Check if the GUI can be started.  Reset gui.starting if not.
     * Don't know about other systems, stay on the safe side and don't check.
     */
    اگر (gui.starting)
    {
	اگر (gui_init_check() == FAIL)
	{
	    gui.starting = false;

	    // When running "evim" or "gvim -y" we need the menus, exit if we
	    // don't have them.
	    اگر (params.evim_منوال)
		mch_exit(1);
	}
    }
#   endif
#  endif

    اگر (GARGCOUNT > 0)
    {
#  ifdef EXPAND_FILENAMES
	/*
	 * Expand wildcards in file names.
	 */
	اگر (!params.literal)
	{
	    آغاز_dir = alloc(MAXPATHL);
	    اگر (آغاز_dir != تہی)
		mch_dirname(آغاز_dir, MAXPATHL);
	    // Temporarily add '(' and ')' to 'isfname'.  These are valid
	    // filename characters but are excluded from 'isfname' to make
	    // "gf" work on a file name in parentheses (e.g.: see vim.h).
	    do_cmdline_cmd((char_u *)":set isf+=(,)");
	    alist_expand(تہی, 0);
	    do_cmdline_cmd((char_u *)":set isf&");
	    اگر (آغاز_dir != تہی)
		mch_chdir((حرف *)آغاز_dir);
	}
#  endif
	params.fname = alist_name(&GARGLIST[0]);
    }

#  ifdef MSWIN
    {
	// Remember the number of entries in the argument list.  If it changes
	// we don't react on setting 'encoding'.
	سیٹ_alist_گنتی();
    }
#  endif

#  ifdef MSWIN
    اگر (GARGCOUNT == 1 && params.بھرا_راستہ)
    {
	/*
	 * If there is one filename, fully qualified, we have very probably
	 * been invoked from explorer, so change to the file's directory.
	 * Hint: to avoid this when typing a command use a forward slash.
	 * If the cd fails, it doesn't matter.
	 */
	اگر (vim_chdirfile(params.fname, "drop") == OK)
	    آخری_chdir_reason = "drop";
	اگر (آغاز_dir != تہی)
	    mch_dirname(آغاز_dir, MAXPATHL);
    }
#  endif
    وقت_پیغام("expanding arguments");

#  ifdef FEAT_DIFF
    اگر (params.diff_منوال && params.کھڑکی_گنتی == -1)
	params.کھڑکی_گنتی = 0;	// open up to 3 windows
#  endif

    // Don't redraw until much later.
    ++RedrawingDisabled;

    /*
     * When listing swap file names, don't do cursor positioning et. al.
     */
    اگر (recoverymode && params.fname == تہی)
	params.want_بھرا_screen = FALSE;

    /*
     * When certain to start the GUI, don't check terminal capabilities.
     * For GTK we can't be sure, but when started from the desktop it doesn't
     * make sense to try using a terminal.
     */
#  اگر defined(ALWAYS_USE_GUI) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	|| defined(VIMDLL)
    اگر (gui.starting
#   ifdef FEAT_GUI_GTK
	    && !isatty(2)
#   endif
	    )
	params.want_بھرا_screen = FALSE;
#  endif

    /*
     * mch_init() sets up the terminal (window) for use.  This must be
     * done after resetting full_screen, otherwise it may move the cursor.
     * Note that we may use mch_exit() before mch_init()!
     */
    mch_init();
    وقت_پیغام("shell init");

#  ifdef USE_XSMP
    /*
     * For want of anywhere else to do it, try to connect to xsmp here.
     * Fitting it in after gui_mch_init, but before gui_init (via termcapinit).
     * Hijacking -X 'no X connection' to also disable XSMP connection as that
     * has a similar delay upon failure.
     * Only try if SESSION_MANAGER is set to something non-null.
     */
    اگر (!x_no_connect)
    {
	حرف *p = getenv("SESSION_MANAGER");

	اگر (p != تہی && *p != NUL)
	{
	    xsmp_init();
	    وقت_پیغام("xsmp init");
	}
    }
#  endif

    /*
     * Print a warning if stdout is not a terminal.
     */
    check_tty(&params);

#  ifdef _IOLBF
    اگر (silent_منوال)
    {
	// Ensure output works usefully without a tty: buffer lines instead of
	// fully buffered.
	s_vbuf = حافظہ_دو(BUFSIZ);
	اگر (s_vbuf != تہی)
	    setvbuf(stdout, s_vbuf, _IOLBF, BUFSIZ);
    }
#  endif

    // This message comes before term inits, but after setting "silent_mode"
    // when the input is not a tty. Omit the message with --not-a-term.
    اگر (GARGCOUNT > 1 && !silent_منوال && !is_not_a_رکن())
	لکھو(_("%d files to edit\n"), GARGCOUNT);

    اگر (params.want_بھرا_screen && !silent_منوال)
    {
	termcapinit(params.رکن);	// set terminal name and get terminal
				   // capabilities (will set full_screen)
	screen_آغاز();		// don't know where cursor is now
	وقت_پیغام("Termcap init");
    }

    /*
     * Set the default values for the options that use Rows and Columns.
     */
    ui_get_shellsize();		// inits Rows and Columns
    win_init_سائز();
#  ifdef FEAT_DIFF
    // Set the 'diff' option now, so that it can be checked for in a .vimrc
    // file.  There is no buffer yet though.
    اگر (params.diff_منوال)
	diff_win_options(firstwin, FALSE);
#  endif

    cmdline_صف = صفیں - p_ch;
    cmdline_ستون_off = 0;
    cmdline_چوڑائی = (صحیح)Columns;
    پیغام_صف = cmdline_صف;
    پیغام_ستون = 0;
    screenalloc(FALSE);		// allocate screen buffers
    سیٹ_init_2();
    وقت_پیغام("inits 2");

    پیغام_scroll = TRUE;
    no_wait_return = TRUE;

    init_mappings();		// set up initial mappings

    init_highlight(TRUE, FALSE); // set the default highlight groups
    وقت_پیغام("init highlight");

#  اگر defined(FEAT_TERMRESPONSE)
    init_رکن_props(TRUE);
#  endif

#  ifdef FEAT_EVAL
    // Set the break level after the terminal is initialized.
    debug_break_level = params.use_debug_break_level;
#  endif

    // Reset 'loadplugins' for "-u NONE" before "--cmd" arguments.
    // Allows for setting 'loadplugins' there.
    اگر (params.use_vimrc != تہی
	    && (STRCMP(params.use_vimrc, "NONE") == 0
		|| STRCMP(params.use_vimrc, "DEFAULTS") == 0))
	p_lpl = FALSE;

    // Execute --cmd arguments.
    exe_pre_commands(&params);

    // Source startup scripts.
    ماخذ_startup_scripts(&params);

#  ifdef FEAT_EVAL
    سیٹ_vim_var_nr(VV_VIM_DID_INIT, 1L);
#  endif

#  ifdef FEAT_MZSCHEME
    /*
     * Newer version of MzScheme (Racket) require earlier (trampolined)
     * initialisation via scheme_main_setup.
     * Implement this by initialising it as early as possible
     * and splitting off remaining Vim main into vim_main2().
     * Do source startup scripts, so that 'mzschemedll' can be set.
     */
    واپس mzscheme_main();
#  ورنہ
    واپس vim_main2();
#  endif
}
# endif // NO_VIM_MAIN
#اگر_ختم // PROTO

#اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(FEAT_CLIPBOARD)
/*
 * Restore the state after a fatal X error.
 */
    ساکن خالی
x_restore_state(خالی)
{
    State = منوال_معمول;
    VIsual_فعال = FALSE;
    got_int = TRUE;
    need_wait_return = FALSE;
    global_busy = FALSE;
    exmode_فعال = 0;
    skip_redraw = FALSE;
    RedrawingDisabled = 0;
    no_wait_return = 0;
    vgetc_busy = 0;
# ifdef FEAT_EVAL
    emsg_skip = 0;
# endif
    emsg_off = 0;
    setmouse();
    settmode(TMODE_RAW);
    starttermcap();
    scroll_آغاز();
    redraw_later_clear();
    انتخاب_clipmethod();
}
#اگر_ختم

/*
 * vim_main2() is needed for FEAT_MZSCHEME, but we define it always to keep
 * things simple.
 * It is also defined when NO_VIM_MAIN is defined, but then it's empty.
 */
    صحیح
vim_main2(خالی)
{
#اگر_غیر_تعریف_شدہ NO_VIM_MAIN
# ifdef FEAT_EVAL
    /*
     * Read all the plugin files.
     * Only when compiled with +eval, since most plugins need it.
     */
    اگر (p_lpl)
    {
	char_u *rtp_copy = تہی;
	char_u *plugin_نمونہ = (char_u *)
#  اگر defined(VMS) || defined(AMIGA) // VMS and Amiga don't handle the "**".
		"plugin/*.vim"
#  ورنہ
		"plugin/**/*.vim"
#  endif
		;

	// First add all package directories to 'runtimepath', so that their
	// autoload directories can be found.  Only if not done already with a
	// :packloadall command.
	// Make a copy of 'runtimepath', so that source_runtime does not use
	// the pack directories.
	اگر (!did_ماخذ_packages)
	{
	    rtp_copy = vim_strsave(p_rtp);
	    جوڑو_pack_آغاز_dirs();
	}

	ماخذ_اندر_راستہ(rtp_copy == تہی ? p_rtp : rtp_copy, plugin_نمونہ,
		DIP_ALL | DIP_NOAFTER, تہی);
	وقت_پیغام("loading plugins");
	vim_free(rtp_copy);

	// Only source "start" packages if not done already with a :packloadall
	// command.
	اگر (!did_ماخذ_packages)
	    بھرو_آغاز_packages();
	وقت_پیغام("loading packages");

	ماخذ_runtime(plugin_نمونہ, DIP_ALL | DIP_AFTER);
	وقت_پیغام("loading after plugins");
    }
# endif

# ifdef FEAT_DIFF
    // Decide about window layout for diff mode after reading vimrc.
    اگر (params.diff_منوال && params.کھڑکی_layout == 0)
    {
	اگر (diffopt_horizontal())
	    params.کھڑکی_layout = WIN_HOR;	// use horizontal split
	ورنہ
	    params.کھڑکی_layout = WIN_VER;	// use vertical split
    }
# endif

    /*
     * Recovery mode without a file name: List swap files.
     * This uses the 'dir' option, therefore it must be after the
     * initializations.
     */
    اگر (recoverymode && params.fname == تہی)
    {
	recover_نام(تہی, TRUE, تہی, 0, تہی);
	mch_exit(0);
    }

    /*
     * Set a few option defaults after reading .vimrc files:
     * 'title' and 'icon', Unix: 'shellpipe' and 'shellredir'.
     */
    سیٹ_init_3();
    وقت_پیغام("inits 3");

    /*
     * "-n" argument: Disable swap file by setting 'updatecount' to 0.
     * Note that this overrides anything from a vimrc file.
     */
    اگر (params.no_تبادلہ_file)
	p_uc = 0;

# ifdef FEAT_GUI
    اگر (gui.starting)
    {
#  اگر defined(UNIX) || defined(VMS)
	// When something caused a message from a vimrc script, need to output
	// an extra newline before the shell prompt.
	اگر (did_emsg || پیغام_didout)
	    حرف_لکھو('\n');
#  endif

	gui_آغاز(تہی);		// will set full_screen to TRUE
	وقت_پیغام("starting GUI");

	// When running "evim" or "gvim -y" we need the menus, exit if we
	// don't have them.
	اگر (!gui.اندر_use && params.evim_منوال)
	    mch_exit(1);
	firstwin->w_پچھلا_اونچائی = firstwin->w_اونچائی; // may have changed
    }
# endif

# ifdef FEAT_VIMINFO
    /*
     * Read in registers, history etc, but not marks, from the viminfo file.
     * This is where v:oldfiles gets filled.
     */
    اگر (*p_viminfo != NUL)
    {
	read_viminfo(تہی, VIF_WANT_INFO | VIF_GET_OLDFILES);
	وقت_پیغام("reading viminfo");
    }
# endif
# ifdef FEAT_EVAL
    // It's better to make v:oldfiles an empty list than NULL.
    اگر (get_vim_var_list(VV_OLDFILES) == تہی)
	سیٹ_vim_var_list(VV_OLDFILES, list_alloc());
# endif

# ifdef FEAT_QUICKFIX
    /*
     * "-q errorfile": Load the error file now.
     * If the error file can't be read, exit before doing anything else.
     */
    اگر (params.edit_type == EDIT_QF)
    {
	char_u	*enc = تہی;

	enc = p_menc;
	اگر (params.use_ef != تہی)
	    سیٹ_string_option_براہ_راست((char_u *)"ef", -1,
					   params.use_ef, OPT_FREE, SID_CARG);
	vim_snprintf((حرف *)IObuff, IOSIZE, "cfile %s", p_ef);
	اگر (qf_init(تہی, p_ef, p_efm, TRUE, IObuff, enc) < 0)
	{
	    برآمد_char('\n');
	    mch_exit(3);
	}
	وقت_پیغام("reading errorfile");
    }
# endif

    /*
     * Start putting things on the screen.
     * Scroll screen down before drawing over it
     * Clear screen now, so file message will not be cleared.
     */
    starting = NO_BUFFERS;
    no_wait_return = FALSE;
    اگر (!exmode_فعال)
	پیغام_scroll = FALSE;

# ifdef FEAT_GUI
    /*
     * This seems to be required to make callbacks to be called now, instead
     * of after things have been put on the screen, which then may be deleted
     * when getting a resize callback.
     * For the Mac this handles putting files dropped on the Vim icon to
     * global_alist.
     */
    اگر (gui.اندر_use)
    {
	gui_wait_for_chars(50L, typebuf.tb_change_cnt);
	وقت_پیغام("GUI delay");
    }
# endif

# اگر defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)
    qnx_clip_init();
# endif

# اگر defined(MACOS_X) && defined(FEAT_CLIPBOARD)
    clip_init(TRUE);
# endif

# ifdef FEAT_XCLIPBOARD
    // Start using the X clipboard, unless the GUI was started.
#  ifdef FEAT_GUI
    اگر (!gui.اندر_use)
#  endif
    {
	تیاری_رکن_clip();
	وقت_پیغام("setup x11 clipboard");
    }
# endif

# ifdef FEAT_CLIENTSERVER
    // Prepare for being a Vim server.
    prepare_server(&params);
# endif

# ifdef FEAT_WAYLAND
#  ifdef FEAT_GUI
    اگر (!gui.اندر_use)
#  endif
    {
	اگر (wayland_init_connection(wayland_display_name) == OK)
	{
	    وقت_پیغام("connected to Wayland display");

#  ifdef FEAT_WAYLAND_CLIPBOARD
	    اگر (clip_init_wayland() == OK)
		وقت_پیغام("setup Wayland clipboard");
#  endif
	}
    }
# endif

# ifdef HAVE_CLIPMETHOD
    انتخاب_clipmethod();
# endif

    /*
     * If "-" argument given: Read file from stdin.
     * Do this before starting Raw mode, because it may change things that the
     * writing end of the pipe doesn't like, e.g., in case stdin and stderr
     * are the same terminal: "cat | vim -".
     * Using autocommands here may cause trouble...
     */
    اگر (params.edit_type == EDIT_STDIN && !recoverymode)
	read_stdin();

# اگر defined(UNIX) || defined(VMS)
    // When switching screens and something caused a message from a vimrc
    // script, need to output an extra newline on exit.
    اگر ((did_emsg || پیغام_didout) && *T_TI != NUL && params.edit_type != EDIT_STDIN)
	newline_on_exit = TRUE;
# endif

    /*
     * When done something that is not allowed or given an error message call
     * wait_return().  This must be done before starttermcap(), because it may
     * switch to another screen. It must be done after settmode(TMODE_RAW),
     * because we want to react on a single key stroke.
     * Call settmode and starttermcap here, so the T_KS and T_TI may be
     * defined by termcapinit and redefined in .exrc.
     */
    settmode(TMODE_RAW);
    وقت_پیغام("setting raw mode");

    اگر (need_wait_return || پیغام_didany)
    {
	wait_return(TRUE);
	وقت_پیغام("waiting for return");
    }

    starttermcap();	    // start termcap if not done by wait_return()
    وقت_پیغام("start termcap");

    setmouse();				// may start using the mouse
    اگر (scroll_region)
	scroll_region_reset();		// In case Rows changed
    scroll_آغاز();	// may scroll the screen to the right position

# اگر defined(UNIX) || defined(VMS) || defined(MACOS_X)
    رکن_ڈالو_title(SAVE_RESTORE_BOTH);
# endif

    /*
     * Don't clear the screen when starting in Ex mode, unless using the GUI.
     */
    اگر (exmode_فعال
# ifdef FEAT_GUI
			&& !gui.اندر_use
# endif
					)
	سیٹ_must_redraw(UPD_CLEAR);
    ورنہ
    {
	screenclear();			// clear screen
	وقت_پیغام("clearing screen");
    }

# ifdef FEAT_CRYPT
    اگر (params.ask_for_کلید)
    {
	crypt_check_current_طریقہ();
	(خالی)crypt_get_کلید(TRUE, TRUE);
	وقت_پیغام("getting crypt key");
    }
# endif

    no_wait_return = TRUE;

    /*
     * Create the requested number of windows and edit buffers in them.
     * Also does recovery if "recoverymode" set.
     */
    create_windows(&params);
    وقت_پیغام("opening buffers");

# ifdef FEAT_EVAL
    // clear v:swapcommand
    سیٹ_vim_var_string(VV_SWAPCOMMAND, تہی, -1);
# endif

    // Ex starts at last line of the file
    اگر (exmode_فعال)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;

    apply_autocmds(EVENT_BUFENTER, تہی, تہی, FALSE, curbuf);
    وقت_پیغام("BufEnter autocommands");
    setpcmark();

# ifdef FEAT_QUICKFIX
    /*
     * When started with "-q errorfile" jump to first error now.
     */
    اگر (params.edit_type == EDIT_QF)
    {
	qf_jump(تہی, 0, 0, FALSE);
	وقت_پیغام("jump to first error");
    }
# endif

    /*
     * If opened more than one window, start editing files in the other
     * windows.
     */
    edit_buffers(&params, آغاز_dir);
    vim_free(آغاز_dir);

# ifdef FEAT_DIFF
    اگر (params.diff_منوال)
    {
	win_T	*wp;

	// set options in each window for "vimdiff".
	FOR_ALL_WINDOWS(wp)
	    diff_win_options(wp, TRUE);
    }
# endif

    /*
     * Shorten any of the filenames, but only when absolute.
     */
    shorten_fnames(FALSE);

    /*
     * Need to jump to the tag before executing the '-c command'.
     * Makes "vim -c '/return' -t main" work.
     */
    اگر (params.tagname != تہی)
    {
	تبادلہ_exists_did_quit = FALSE;

	vim_snprintf((حرف *)IObuff, IOSIZE, "ta %s", params.tagname);
	do_cmdline_cmd(IObuff);
	وقت_پیغام("jumping to tag");

	// If the user doesn't want to edit the file then we quit here.
	اگر (تبادلہ_exists_did_quit)
	    getout(1);
    }

# اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    // Temporarily set x_jump_env to here in case there is an X11 IO error,
    // because x_jump_env is only actually set in main_loop(), before
    // exe_commands(). May not be the best solution since commands passed via
    // the command line can be very broad like sourcing a file, in which case
    // an X IO error results in the command being partially done. In theory we
    // could use SETJMP in RealWaitForChar(), but the stack frame for that may
    // possibly exit and then LONGJMP is called on it.
    صحیح jump_نتیجہ = SETJMP(x_jump_env);

    اگر (jump_نتیجہ == 0)
    {
# endif
	// Execute any "+", "-c" and "-S" arguments.
	اگر (params.n_commands > 0)
	    exe_commands(&params);
# اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    }
    ورنہ
	// Restore state and continue just like what main_loop() does.
	x_restore_state();
# endif

    // Must come before the may_req_ calls.
    starting = 0;

# اگر defined(FEAT_TERMRESPONSE)
    // Must be done before redrawing, puts a few characters on the screen.
    check_terminal_behavior();
# endif

    RedrawingDisabled = 0;
    redraw_all_later(UPD_NOT_VALID);
    no_wait_return = FALSE;

    // 'autochdir' has been postponed
    DO_AUTOCHDIR;

# ifdef FEAT_TERMRESPONSE
    // Requesting the termresponse is postponed until here, so that a "-c q"
    // argument doesn't make it appear in the shell Vim was started from.
    may_req_termresponse();

    may_req_bg_color();
# endif
# اگر defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_سائز();
# endif

    // start in insert mode
    اگر (p_im)
	need_آغاز_insertmode = TRUE;

# ifdef FEAT_EVAL
    سیٹ_vim_var_nr(VV_VIM_DID_ENTER, 1L);
# endif
    apply_autocmds(EVENT_VIMENTER, تہی, تہی, FALSE, curbuf);
    وقت_پیغام("VimEnter autocommands");

# اگر defined(FEAT_EVAL) && defined(FEAT_CLIPBOARD)
    // Adjust default register name for "unnamed" in 'clipboard'. Can only be
    // done after the clipboard is available and all initial commands that may
    // modify the 'clipboard' setting have run; i.e. just before entering the
    // main loop.
    reset_reg_var();
# endif

# اگر defined(FEAT_DIFF)
    // When a startup script or session file setup for diff'ing and
    // scrollbind, sync the scrollbind now.
    اگر (curwin->w_p_diff && curwin->w_p_scb)
    {
	update_topline();
	check_scrollbind((linenr_T)0, 0L);
	وقت_پیغام("diff scrollbinding");
    }
# endif

# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    اگر (!gui.اندر_use)
#  endif
	mch_سیٹ_winsize_now();	    // Allow winsize changes from now on
# endif

# اگر defined(FEAT_GUI)
    // When tab pages were created, may need to update the tab pages line and
    // scrollbars.  This is skipped while creating them.
    اگر (gui.اندر_use && پہلا_tabpage->tp_اگلا != تہی)
    {
	برآمد_flush();
	gui_init_which_components(تہی);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
# endif

    // If ":startinsert" command used, stuff a dummy command to be able to
    // call normal_cmd(), which will then start Insert mode.
    اگر (restart_edit != 0)
	stuffcharReadbuff(K_NOP);

# ifdef FEAT_NETBEANS_INTG
    اگر (netbeansArg != تہی && لڑی_موازنہ("-nb", netbeansArg, 3) == 0)
    {
#  ifdef FEAT_GUI
#   اگر !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
	اگر (gui.اندر_use)
	{
	    mch_errmsg(_("netbeans is not supported with this GUI\n"));
	    mch_exit(2);
	}
#   endif
#  endif
	// Tell the client that it can start sending commands.
	netbeans_open(netbeansArg + 3, TRUE);
    }
# endif

# ifdef FEAT_IMAGE
    init_image_state();
# endif

    // Redraw at least once, also when 'lazyredraw' is set, to make sure the
    // window title gets updated.
    do_redraw = TRUE;

    وقت_پیغام("before starting main loop");

# ifdef FEAT_کام_CHANNEL
    اگر (use_stdio_channel)
    {
	// Serve the stdio channel instead of reading Ex commands, exit when
	// it is closed.
	channel_stdio_loop();
	getout(0);
    }
# endif

    /*
     * Call the main command loop.  This never returns.
     */
    main_loop(FALSE, FALSE);

#اگر_ختم // NO_VIM_MAIN

    واپس 0;
}

/*
 * Initialisation #1 shared by main() and some tests.
 */
    خالی
common_init_1(خالی)
{
    estack_init();
    cmdline_init();

    (خالی)mb_init();	// init mb_bytelen_tab[] to ones
#اگر_تعریف_شدہ FEAT_EVAL
    eval_init();	// init global variables
#اگر_ختم

#اگر_تعریف_شدہ __QNXNTO__
    qnx_init();		// PhAttach() for clipboard, (and gui)
#اگر_ختم

    /*
     * Allocate space for the generic buffers (needed for set_init_1() and
     * emsg()).
     */
    اگر ((IObuff = alloc(IOSIZE)) == تہی
	    || (NameBuff = alloc(MAXPATHL)) == تہی)
	mch_exit(0);
    وقت_پیغام("Allocated generic buffers");
}


/*
 * Initialisation #2 shared by main() and some tests.
 */
    خالی
common_init_2(mparm_T *paramp)
{
#اگر_تعریف_شدہ NBDEBUG
    // Wait a moment for debugging NetBeans.  Must be after allocating
    // NameBuff.
    nbdebug_log_init("SPRO_GVIM_DEBUG", "SPRO_GVIM_DLEVEL");
    nbdebug_wait(WT_ENV | WT_WAIT | WT_STOP, "SPRO_GVIM_WAIT", 20);
    وقت_پیغام("NetBeans debug wait");
#اگر_ختم

#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    /*
     * Setup to use the current locale (for ctype() and many other things).
     * NOTE: Translated messages with encodings other than latin1 will not
     * work until set_init_1() has been called!
     */
    init_locale();
    وقت_پیغام("locale set");
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    gui.dofork = true;		    // default is to use fork()
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CLIENTSERVER_BACKENDS
    /*
     * Check the $VIM_CLIENTSERVER env before we handle the --clientserver arg
     */
    check_clientserver_طریقہ_env();
#اگر_ختم

    /*
     * Do a first scan of the arguments in "argv[]":
     *   -display or --display
     *   --server...
     *   --socketid
     *   --windowid
     */
    early_arg_جائزہ(paramp);

#اگر defined(FEAT_GUI)
    // Prepare for possibly starting GUI sometime
    gui_prepare(&paramp->argc, paramp->argv);
    وقت_پیغام("GUI prepared");
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CLIPBOARD
    clip_init(FALSE);		// Initialise clipboard stuff
    وقت_پیغام("clipboard setup");
#اگر_ختم

    /*
     * Check if we have an interactive window.
     * On the Amiga: If there is no window, we open one with a newcli command
     * (needed for :! to * work). mch_check_win() will also handle the -d or
     * -dev argument.
     */
    stdout_isatty = (mch_check_win(paramp->argc, paramp->argv) != FAIL);
    وقت_پیغام("window checked");

    /*
     * Allocate the first window and buffer.
     * Can't do anything without it, exit when it fails.
     */
    اگر (win_alloc_پہلا() == FAIL)
	mch_exit(0);

    init_yank();		// init yank buffers

    alist_init(&global_alist);	// Init the argument list to empty.
    global_alist.id = 0;

    /*
     * Set the default values for the options.
     * NOTE: Non-latin1 translated messages are working only after this,
     * because this is where "has_mbyte" will be set, which is used by
     * msg_outtrans_len_attr().
     * First find out the home directory, needed to expand "~" in options.
     */
    init_homedir();		// find real value of $HOME
    سیٹ_init_1(paramp->clean);
    وقت_پیغام("inits 1");

#اگر_تعریف_شدہ FEAT_EVAL
    // set v:lang and v:ctype
    سیٹ_lang_var();

    // set v:argv
    سیٹ_argv_var(paramp->argv, paramp->argc);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SIGNS
    init_signs();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_QUICKFIX
    // initialize quickfix list
    // don't send an error message when memory allocation fails
    // do it when the user tries to access the quickfix list
    qf_init_ڈھیر();
#اگر_ختم
}

/*
 * Return TRUE when the --not-a-term argument was found.
 */
    صحیح
is_not_a_رکن(خالی)
{
    واپس params.not_a_رکن;
}

/*
 * Return TRUE when the --not-a-term argument was found or the GUI is in use.
 */
    صحیح
is_not_a_رکن_or_gui(خالی)
{
    واپس params.not_a_رکن
#اگر_تعریف_شدہ FEAT_GUI
			    || gui.اندر_use
#اگر_ختم
	;
}

#اگر defined(EXITFREE)
    خالی
free_vbuf(خالی)
{
# ifdef _IOLBF
    اگر (s_vbuf != تہی)
    {
	setvbuf(stdout, تہی, _IONBF, 0);
	آزاد(s_vbuf);
	s_vbuf = تہی;
    }
# endif
}
#اگر_ختم

#اگر defined(FEAT_GUI)
/*
 * If a --gui-dialog-file argument was given return the file name.
 * Otherwise return NULL.
 */
    char_u *
get_gui_dialog_file(خالی)
{
    واپس params.gui_dialog_file;
}
#اگر_ختم

// When TRUE in a safe state when starting to wait for a character.
ساکن صحیح	was_محفوظ = FALSE;
ساکن oparg_T	*current_oap = تہی;

/*
 * Return TRUE if an operator was started but not finished yet.
 * Includes typing a count or a register name.
 */
    صحیح
op_pending(خالی)
{
    واپس !(current_oap != تہی
	    && !اختتام_op
	    && current_oap->پچھلا_opcount == 0
	    && current_oap->پچھلا_count0 == 0
	    && current_oap->op_type == OP_NOP
	    && current_oap->regname == NUL);
}

/*
 * Return whether currently it is safe, assuming it was safe before (high level
 * state didn't change).
 */
    ساکن صحیح
is_محفوظ_now(خالی)
{
    واپس stuff_خالی_خانے()
	&& typebuf.tb_len == 0
	&& scriptin[curscript] == تہی
#اگر_تعریف_شدہ FEAT_EVAL
	&& !debug_منوال
#اگر_ختم
	&& !global_busy;
}

/*
 * Trigger SafeState if currently in a safe state, that is "safe" is TRUE and
 * there is no typeahead.
 */
    خالی
may_trigger_safestate(صحیح محفوظ_2)
{
    صحیح is_محفوظ = محفوظ_2 && is_محفوظ_now();

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (was_محفوظ != is_محفوظ)
	// Only log when the state changes, otherwise it happens at nearly
	// every key stroke.
	ch_log(تہی, is_محفوظ ? "SafeState: Start triggering"
					       : "SafeState: Stop triggering");
#اگر_ختم
    اگر (is_محفوظ)
	apply_autocmds(EVENT_SAFESTATE, تہی, تہی, FALSE, curbuf);
    was_محفوظ = is_محفوظ;
}

/*
 * Something changed which causes the state possibly to be unsafe, e.g. a
 * character was typed.  It will remain unsafe until the next call to
 * may_trigger_safestate().
 */
    خالی
state_no_longer_محفوظ(حرف *reason غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (was_محفوظ)
	ch_log(تہی, "SafeState: reset: %s", reason);
#اگر_ختم
    was_محفوظ = FALSE;
}

#اگر defined(FEAT_EVAL) || defined(پیغام_قطار)
    صحیح
get_was_محفوظ_state(خالی)
{
    واپس was_محفوظ;
}
#اگر_ختم

#اگر defined(پیغام_قطار)
/*
 * Invoked when leaving code that invokes callbacks.  Then trigger
 * SafeStateAgain, if it was safe when starting to wait for a character.
 */
    خالی
may_trigger_safestateagain(خالی)
{
    اگر (!was_محفوظ)
    {
	// If the safe state was reset in state_no_longer_safe(), e.g. because
	// of calling feedkeys(), we check if it's now safe again (all keys
	// were consumed).
	was_محفوظ = is_محفوظ_now();
# ifdef FEAT_EVAL
	اگر (was_محفوظ)
	    ch_log(تہی, "SafeState: undo reset");
# endif
    }
    اگر (was_محفوظ)
    {
# ifdef FEAT_EVAL
	// Only do this message when another message was given, otherwise we
	// get lots of them.
	اگر ((did_repeated_پیغام & REPEATED_پیغام_SAFESTATE) == 0)
	{
	    صحیح did = did_repeated_پیغام;

	    ch_log(تہی,
		      "SafeState: back to waiting, triggering SafeStateAgain");
	    did_repeated_پیغام = did | REPEATED_پیغام_SAFESTATE;
	}
# endif
	apply_autocmds(EVENT_SAFESTATEAGAIN, تہی, تہی, FALSE, curbuf);
    }
# ifdef FEAT_EVAL
    ورنہ
	ch_log(تہی,
		  "SafeState: back to waiting, not triggering SafeStateAgain");
# endif
}
#اگر_ختم

/*
 * Return TRUE if there is any typeahead, pending operator or command.
 */
    صحیح
work_pending(خالی)
{
    واپس op_pending() || !is_محفوظ_now();
}

#اگر_تعریف_شدہ FEAT_CONCEAL
ساکن linenr_T	conceal_old_cursor_سطر = 0;
ساکن linenr_T	conceal_new_cursor_سطر = 0;
ساکن صحیح	conceal_update_سطریں = FALSE;
#اگر_ختم

/*
 * Trigger the events that are not triggered where they happen, but by
 * comparing the current state against the state stored when they were last
 * triggered. Also updates what depends on the cursor having moved, which is
 * not affected by 'eventignore'.
 * Called from the main loop and when 'eventignore(win)' changes, so that what
 * happened with an event ignored is not reported once it is not ignored
 * anymore.
 */
    خالی
may_trigger_deferred_events(خالی)
{
    ساکن bool	recursive = false;

    اگر (recursive)
	واپس;
    recursive = true;

#اگر_تعریف_شدہ FEAT_CONCEAL
    اگر (curwin->w_p_cole == 0)
	conceal_update_سطریں = FALSE;
#اگر_ختم

    // Trigger CursorMoved if the cursor moved.
    اگر (!اختتام_op && (has_cursormoved()
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		|| popup_visible
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
		|| curwin->w_p_cole > 0
#اگر_ختم
		) && !EQUAL_POS(آخری_cursormoved, curwin->w_cursor))
    {
	اگر (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, تہی, تہی, FALSE, curbuf);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (popup_visible)
	    popup_check_cursor_pos();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
	اگر (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_سطر = آخری_cursormoved.lnum;
	    conceal_new_cursor_سطر = curwin->w_cursor.lnum;
	    conceal_update_سطریں = TRUE;
	}
#اگر_ختم
	آخری_cursormoved = curwin->w_cursor;
    }

#اگر_تعریف_شدہ FEAT_CONCEAL
    اگر (conceal_update_سطریں
	    && (conceal_old_cursor_سطر != conceal_new_cursor_سطر
		|| conceal_cursor_سطر(curwin)
		|| need_cursor_سطر_redraw))
    {
	اگر (conceal_old_cursor_سطر != conceal_new_cursor_سطر
		&& conceal_old_cursor_سطر != 0
		&& conceal_old_cursor_سطر <= curbuf->b_ml.ml_سطر_گنتی)
	    redrawWinline(curwin, conceal_old_cursor_سطر);
	redrawWinline(curwin, conceal_new_cursor_سطر);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_سطر_redraw = FALSE;
    }
#اگر_ختم

    // Trigger TextChanged if b:changedtick differs.
    اگر (!اختتام_op && has_textchanged()
	    && curbuf->b_آخری_changedtick != CHANGEDTICK(curbuf))
    {
	apply_autocmds(EVENT_TEXTCHANGED, تہی, تہی, FALSE, curbuf);
	curbuf->b_آخری_changedtick = CHANGEDTICK(curbuf);
    }

    // Ensure curwin->w_topline and curwin->w_leftcol are up to date before
    // triggering a WinScrolled autocommand.
    update_topline();
    validate_cursor();

    اگر (!اختتام_op)
	may_trigger_win_scrolled_resized();

    recursive = false;
}


/*
 * Main loop: Execute Normal mode commands until exiting Vim.
 * Also used to handle commands in the command-line window, until the window
 * is closed.
 * Also used to handle ":visual" command after ":global": execute Normal mode
 * commands, return when entering Ex mode.  "noexmode" is TRUE then.
 */
    خالی
main_loop(
    صحیح		cmdwin,	    // TRUE when working in the command-line window
    صحیح		noexmode)   // TRUE when return on entering Ex mode
{
    oparg_T	oa;		// operator arguments
    oparg_T	*پچھلا_oap;	// operator arguments
    متلون صحیح پچھلا_got_int_2 = FALSE;	// "got_int" was TRUE

    پچھلا_oap = current_oap;
    current_oap = &oa;

#اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    // Setup to catch a terminating error from the X server.  Just ignore
    // it, restore the state and continue.  This might not always work
    // properly, but at least we hopefully don't exit unexpectedly when the X
    // server exits while Vim is running in a console.
    اگر (!cmdwin && !noexmode && SETJMP(x_jump_env))
	x_restore_state();
#اگر_ختم

    clear_oparg(&oa);
    جب (!cmdwin || cmdwin_نتیجہ == 0)
    {
	اگر (stuff_خالی_خانے())
	{
	    did_check_timestamps = FALSE;
	    اگر (need_check_timestamps)
		check_timestamps(FALSE);
	    اگر (need_wait_return)	// if wait_return() still needed ...
		wait_return(FALSE);	// ... call it now
	    اگر (need_آغاز_insertmode && goto_im() && !VIsual_فعال)
	    {
		need_آغاز_insertmode = FALSE;
		stuffReadbuff((char_u *)"i");	// start insert mode next
		// skip the fileinfo message now, because it would be shown
		// after insert mode finishes!
		need_fileinfo = FALSE;
	    }
	}

	// Reset "got_int" now that we got back to the main loop.  Except when
	// inside a ":g/pat/cmd" command, then the "got_int" needs to abort
	// the ":g" command.
	// For ":g/pat/vi" we reset "got_int" when used once.  When used
	// a second time we go back to Ex mode and abort the ":g" command.
	اگر (got_int)
	{
	    اگر (noexmode && global_busy && !exmode_فعال && پچھلا_got_int_2)
	    {
		// Typed two CTRL-C in a row: go back to ex mode as if "Q" was
		// used and keep "got_int" set, so that it aborts ":g".
		exmode_فعال = EXMODE_معمول;
		State = منوال_معمول;
	    }
	    ورنہ اگر (!global_busy || !exmode_فعال)
	    {
		اگر (!quit_مزید)
		    (خالی)vgetc();		// flush all buffers
		got_int = FALSE;
	    }
	    پچھلا_got_int_2 = TRUE;
	}
	ورنہ
	    پچھلا_got_int_2 = FALSE;

#اگر_تعریف_شدہ FEAT_EVAL
	// At the toplevel there is no exception handling.  Discard any that
	// may be hanging around (e.g. from "interrupt" at the debug prompt).
	اگر (did_throw && !ex_معمول_busy)
	    discard_current_exception();
#اگر_ختم

	اگر (!exmode_فعال)
	    پیغام_scroll = FALSE;
	quit_مزید = FALSE;

	// it's not safe unless may_trigger_safestate_main() is called
	was_محفوظ = FALSE;

	/*
	 * If skip redraw is set (for ":" in wait_return()), don't redraw now.
	 * If there is nothing in the stuff_buffer or do_redraw is TRUE,
	 * update cursor and redraw.
	 */
	اگر (skip_redraw || exmode_فعال)
	{
	    skip_redraw = FALSE;
	    setcursor();
	    cursor_on();
	}
	ورنہ اگر (do_redraw || stuff_خالی_خانے())
	{
#اگر_تعریف_شدہ FEAT_GUI
	    // If ui_breakcheck() was used a resize may have been postponed.
	    gui_may_resize_shell();
#اگر_ختم
#اگر_تعریف_شدہ HAVE_DROP_FILE
	    // If files were dropped while text was locked or the curbuf was
	    // locked, this would be a good time to handle the drop.
	    handle_any_postponed_drop();
#اگر_ختم
	    may_trigger_deferred_events();

	    // If nothing is pending and we are going to wait for the user to
	    // type a character, trigger SafeState.
	    may_trigger_safestate(!op_pending() && restart_edit == 0);

#اگر defined(FEAT_DIFF)
	    // Updating diffs from changed() does not always work properly,
	    // esp. updating folds.  Do an update just before redrawing if
	    // needed.
	    اگر (curtab->tp_diff_update || curtab->tp_diff_invalid)
	    {
		ex_diffupdate(تہی);
		curtab->tp_diff_update = FALSE;
	    }

	    // Scroll-binding for diff mode may have been postponed until
	    // here.  Avoids doing it for every change.
	    اگر (diff_need_scrollbind)
	    {
		check_scrollbind((linenr_T)0, 0L);
		diff_need_scrollbind = FALSE;
	    }
#اگر_ختم
#اگر defined(FEAT_FOLDING)
	    // Include a closed fold completely in the Visual area.
	    foldAdjustVisual();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_FOLDING
	    /*
	     * When 'foldclose' is set, apply 'foldlevel' to folds that don't
	     * contain the cursor.
	     * When 'foldopen' is "all", open the fold(s) under the cursor.
	     * This may mark the window for redrawing.
	     */
	    اگر (hasAnyFolding(curwin) && !char_avail())
	    {
		foldCheckClose();
		اگر (fdo_flags & FDO_ALL)
		    foldOpenCursor();
	    }
#اگر_ختم

	    // Before redrawing, make sure w_topline is correct, and w_leftcol
	    // if lines don't wrap, and w_skipcol if lines wrap.
	    update_topline();
	    validate_cursor();

	    اگر (VIsual_فعال)
		update_curbuf(UPD_INVERTED); // update inverted part
	    ورنہ اگر (must_redraw)
	    {
		mch_disable_flush();	// Stop issuing gui_mch_flush().
		update_screen(0);
		mch_enable_flush();
	    }
	    ورنہ اگر (redraw_cmdline || clear_cmdline || redraw_منوال)
		showmode();
	    redraw_statuslines();
	    اگر (need_maketitle)
		maketitle();
#اگر_تعریف_شدہ FEAT_VIMINFO
	    curbuf->b_آخری_مستعمل = vim_وقت();
#اگر_ختم
	    // display message after redraw
	    اگر (keep_پیغام != تہی)
	    {
		char_u *p = vim_strsave(keep_پیغام);

		اگر (p != تہی)
		{
		    // msg_start() will set keep_msg to NULL, make a copy
		    // first.  Don't reset keep_msg, msg_attr_keep() uses it to
		    // check for duplicates.  Never put this message in
		    // history.
		    پیغام_hist_off = TRUE;
		    پیغام_attr((حرف *)p, keep_پیغام_attr);
		    پیغام_hist_off = FALSE;
		    vim_free(p);
		}
	    }
	    اگر (need_fileinfo)		// show file info after redraw
	    {
		fileinfo(FALSE, TRUE, FALSE);
		need_fileinfo = FALSE;
	    }

	    emsg_on_display = FALSE;	// can delete error message now
	    did_emsg = FALSE;
	    پیغام_didany = FALSE;		// reset lines_left in msg_start()
	    may_clear_sb_متن();	// clear scroll-back text on next msg
	    showruler(FALSE);

	    setcursor();
	    cursor_on();

	    do_redraw = FALSE;

#اگر_تعریف_شدہ STARTUPTIME
	    // Now that we have drawn the first screen all the startup stuff
	    // has been done, close any file for startup messages.
	    اگر (وقت_fd != تہی)
	    {
		وقت_پیغام("first screen update");
		وقت_پیغام("--- VIM STARTED ---");
		fclose(وقت_fd);
		وقت_fd = تہی;
	    }
#اگر_ختم
	    // After the first screen update may start triggering WinScrolled
	    // autocmd events.  Store all the scroll positions and sizes now.
	    may_make_initial_scroll_سائز_snapshot();
	}
#اگر_تعریف_شدہ FEAT_GUI
	اگر (need_mouse_correct)
	    gui_mouse_correct();
#اگر_ختم

	// May request the keyboard protocol state now.
	may_send_t_RK();

	// Update w_curswant if w_set_curswant has been set.
	// Postponed until here to avoid computing w_virtcol too often.
	update_curswant();

#اگر_تعریف_شدہ FEAT_EVAL
	/*
	 * May perform garbage collection when waiting for a character, but
	 * only at the very toplevel.  Otherwise we may be using a List or
	 * Dict internally somewhere.
	 * "may_garbage_collect" is reset in vgetc() which is invoked through
	 * do_exmode() and normal_cmd().
	 */
	may_garbage_collect = (!cmdwin && !noexmode);
#اگر_ختم
	/*
	 * If we're invoked as ex, do a round of ex commands.
	 * Otherwise, get and execute a normal mode command.
	 */
	اگر (exmode_فعال)
	{
	    اگر (noexmode)   // End of ":global/path/visual" commands
		جاؤ theend;
	    do_exmode(exmode_فعال == EXMODE_VIM);
	}
	ورنہ
	{
#اگر_تعریف_شدہ FEAT_TERMINAL
	    اگر (رکن_use_loop()
		    && oa.op_type == OP_NOP && oa.regname == NUL
		    && !VIsual_فعال
		    && !skip_رکن_loop)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal mode.  With FAIL we first need to position the
		// cursor and the screen needs to be redrawn.
		اگر (terminal_loop(TRUE) == OK)
		    معمول_cmd(&oa, TRUE);
	    }
	    ورنہ
#اگر_ختم
	    {
#اگر_تعریف_شدہ FEAT_TERMINAL
		skip_رکن_loop = FALSE;
#اگر_ختم
		معمول_cmd(&oa, TRUE);
	    }
	}
    }

theend:
    current_oap = پچھلا_oap;
}


#اگر defined(USE_XSMP) || defined(FEAT_GUI)
/*
 * Exit, but leave behind swap files for modified buffers.
 */
    خالی
getout_preserve_modified(صحیح exitval)
{
# اگر defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
# endif

    ml_close_notmod();		    // close all not-modified buffers
    ml_sync_all(FALSE, FALSE);	    // preserve all swap files
    ml_close_all(FALSE);	    // close all memfiles, without deleting
    getout(exitval);		    // exit Vim properly
}
#اگر_ختم


/*
 * Exit properly.  This is the only way to exit Vim after startup has
 * succeeded.  We are certain to exit here, no way to abort it.
 */
    خالی
getout(صحیح exitval)
{
    exiting = TRUE;
#اگر defined(FEAT_EVAL)
    ch_log(تہی, "Exiting...");
#اگر_ختم

    // When running in Ex mode an error causes us to exit with a non-zero exit
    // code.  POSIX requires this, although it's not 100% clear from the
    // standard.
    اگر (exmode_فعال)
	exitval += ex_exitval;

#اگر_تعریف_شدہ FEAT_EVAL
    سیٹ_vim_var_type(VV_EXITING, VAR_نمبر);
    سیٹ_vim_var_nr(VV_EXITING, exitval);
#اگر_ختم

    // Position the cursor on the last screen line, below all the text
    اگر (!is_not_a_رکن_or_gui())
	windgoto((صحیح)صفیں - 1, 0);

#اگر_تعریف_شدہ FEAT_EVAL
    // Invoked all deferred functions in the function stack.
    invoke_all_defer();
#اگر_ختم

#اگر defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
    // Optionally print hashtable efficiency.
    hash_debug_results();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    پیغام_didany = FALSE;
#اگر_ختم

    اگر (v_dying <= 1)
    {
	tabpage_T	*tp;
	tabpage_T	*اگلا_tp;
	buf_T		*buf;
	win_T		*wp;
	صحیح		unblock = 0;

	// Trigger BufWinLeave for all windows, but only once per buffer.
	برائے (tp = پہلا_tabpage; tp != تہی; tp = اگلا_tp)
	{
	    اگلا_tp = tp->tp_اگلا;
	    FOR_ALL_WINDOWS_اندر_TAB(tp, wp)
	    {
		اگر (wp->w_بفر == تہی || !buf_valid(wp->w_بفر))
		    // Autocmd must have close the buffer already, skip.
		    جاری;
		buf = wp->w_بفر;
		اگر (CHANGEDTICK(buf) != -1)
		{
		    bufref_T bufref;

		    سیٹ_bufref(&bufref, buf);
		    apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname,
						    buf->b_fname, FALSE, buf);
		    اگر (bufref_valid(&bufref))
			CHANGEDTICK(buf) = -1;  // note we did it already

		    // start all over, autocommands may mess up the lists
		    اگلا_tp = پہلا_tabpage;
		    توڑو;
		}
	    }
	}

	// Trigger BufUnload for buffers that are loaded
	FOR_ALL_BUFFERS(buf)
	    اگر (buf->b_ml.ml_mfp != تہی)
	    {
		bufref_T bufref;

		سیٹ_bufref(&bufref, buf);
		apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf);
		اگر (!bufref_valid(&bufref))
		    // autocmd deleted the buffer
		    توڑو;
	    }

	// deathtrap() blocks autocommands, but we do want to trigger
	// VimLeavePre.
	اگر (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVEPRE, تہی, تہی, FALSE, curbuf);
	اگر (unblock)
	    بلاک_autocmds();
    }

#اگر_تعریف_شدہ FEAT_VIMINFO
    اگر (
# ifdef EXITFREE
	    entered_free_all_mem == FALSE &&
# endif
	    *p_viminfo != NUL)
	// Write out the registers, history, marks etc, to the viminfo file
	write_viminfo(تہی, FALSE);
#اگر_ختم

    اگر (v_dying <= 1)
    {
	صحیح	unblock = 0;

	// deathtrap() blocks autocommands, but we do want to trigger VimLeave.
	اگر (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVE, تہی, تہی, FALSE, curbuf);
	اگر (unblock)
	    بلاک_autocmds();
    }

#اگر_تعریف_شدہ FEAT_PROFILE
    profile_dump();
#اگر_ختم

    اگر (did_emsg
#اگر_تعریف_شدہ FEAT_GUI
	    || (gui.اندر_use && پیغام_didany && p_verbose > 0)
#اگر_ختم
	    )
    {
	// give the user a chance to read the (error) message
	no_wait_return = FALSE;
	wait_return(FALSE);
    }

    // Position the cursor again, the autocommands may have moved it
    اگر (!is_not_a_رکن_or_gui())
	windgoto((صحیح)صفیں - 1, 0);

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    کام_stop_on_exit();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LUA
    lua_انتہا();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MZSCHEME
    mzscheme_انتہا();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TCL
    tcl_انتہا();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RUBY
    ruby_انتہا();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PYTHON
    python_انتہا();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PYTHON3
    python3_انتہا();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PERL
    perl_انتہا();
#اگر_ختم
#اگر defined(USE_ICONV) && defined(DYNAMIC_ICONV)
    iconv_انتہا();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    netbeans_انتہا();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SOCKETSERVER
    socketserver_stop();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CSCOPE
    cs_انتہا();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_IMAGE
    uninit_image_state();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (garbage_collect_at_exit)
	garbage_collect(FALSE);
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
    free_cmd_دلائل_W();
#اگر_ختم

    رکن_disable_dec();

    mch_exit(exitval);
}

/*
 * Get the name of the display, before gui_prepare() removes it from
 * argv[].  Used for the xterm-clipboard display.
 *
 * Also find the --server, --clientserver... arguments and --socketid and
 * --windowid
 */
    ساکن خالی
early_arg_جائزہ(mparm_T *parmp غیر_مستعمل)
{
#اگر defined(FEAT_XCLIPBOARD) || defined(FEAT_CLIENTSERVER) \
	|| !defined(FEAT_NETBEANS_INTG)
    صحیح		argc = parmp->argc;
    حرف	**argv = parmp->argv;
    صحیح		i;

    برائے (i = 1; i < argc; i++)
    {
	اگر (STRCMP(argv[i], "--") == 0)
	    توڑو;
# ifdef FEAT_XCLIPBOARD
	ورنہ اگر (STRICMP(argv[i], "-display") == 0
#  اگر defined(FEAT_GUI_GTK)
		|| STRICMP(argv[i], "--display") == 0
#  endif
		)
	{
	    اگر (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    xterm_display = argv[++i];
	}
# endif
# ifdef FEAT_CLIENTSERVER
	ورنہ اگر (STRICMP(argv[i], "--servername") == 0)
	{
	    اگر (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    parmp->serverName_arg = (char_u *)argv[++i];
	}
	ورنہ اگر (STRICMP(argv[i], "--serverlist") == 0)
	    parmp->serverArg = TRUE;
	ورنہ اگر (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    parmp->serverArg = TRUE;
#  ifdef FEAT_GUI
	    اگر (لڑی_تلاش(argv[i], "-wait") != 0)
		// don't fork() when starting the GUI to edit files ourself
		gui.dofork = false;
#  endif
	}
#  ifdef FEAT_CLIENTSERVER_BACKENDS
	ورنہ اگر (STRICMP(argv[i], "--clientserver") == 0)
	{
	    char_u *arg;
	    اگر (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    arg = (char_u *)argv[++i];

	    اگر (STRICMP(arg, "socket") == 0)
		clientserver_طریقہ = CLIENTSERVER_طریقہ_SOCKET;
#   ifdef FEAT_X11
	    ورنہ اگر (STRICMP(arg, "x11") == 0)
		clientserver_طریقہ = CLIENTSERVER_طریقہ_X11;
#   endif
#   ifdef MSWIN
	    ورنہ اگر (STRICMP(arg, "mswin") == 0)
		clientserver_طریقہ = CLIENTSERVER_طریقہ_MSWIN;
#   endif
	    ورنہ
		mainerr(ME_UNKNOWN_OPTION, arg);
	}
#  endif
# endif

# اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
#  ifdef FEAT_GUI_MSWIN
	ورنہ اگر (STRICMP(argv[i], "--windowid") == 0)
#  ورنہ
	ورنہ اگر (STRICMP(argv[i], "--socketid") == 0)
#  endif
	{
	    long_u	id;
	    صحیح		گنتی;

	    اگر (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    اگر (STRNICMP(argv[i+1], "0x", 2) == 0)
		گنتی = لڑی_سے_پڑھو(&(argv[i + 1][2]), SCANF_HEX_LONG_U, &id);
	    ورنہ
		گنتی = لڑی_سے_پڑھو(argv[i + 1], SCANF_DECIMAL_LONG_U, &id);
	    اگر (گنتی != 1)
		mainerr(ME_INVALID_ARG, (char_u *)argv[i]);
	    ورنہ
#  ifdef FEAT_GUI_MSWIN
		win_socket_id = id;
#  elif !defined(USE_GTK4)
		gtk_socket_id = id;
#  endif
	    i++;
	}
# endif
# اگر defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
	ورنہ اگر (STRICMP(argv[i], "--echo-wid") == 0)
	    echo_wid_arg = TRUE;
# endif
# ifndef FEAT_NETBEANS_INTG
	ورنہ اگر (لڑی_موازنہ(argv[i], "-nb", (size_t)3) == 0)
	{
	    mch_errmsg(_("'-nb' cannot be used: not enabled at compile time\n"));
	    mch_exit(2);
	}
# endif

    }
#اگر_ختم
}

#اگر_غیر_تعریف_شدہ NO_VIM_MAIN
/*
 * Get a (optional) count for a Vim argument.
 */
    ساکن صحیح
get_نمبر_arg(
    char_u	*p,	    // pointer to argument
    صحیح		*idx,	    // index in argument, is incremented
    صحیح		def)	    // default value
{
    اگر (vim_isdigit(p[*idx]))
    {
	def = atoi((حرف *)&(p[*idx]));
	جب (vim_isdigit(p[*idx]))
	    *idx = *idx + 1;
    }
    واپس def;
}

/*
 * Check for: [r][e][g][vi|vim|view][diff][ex[im]]  (sort of)
 * If the executable name starts with "r" we disable shell commands.
 * If the next character is "e" we run in Easy mode.
 * If the next character is "g" we run the GUI version.
 * If the next characters are "view" we start in readonly mode.
 * If the next characters are "diff" or "vimdiff" we start in diff mode.
 * If the next characters are "ex" we start in Ex mode.  If it's followed
 * by "im" use improved Ex mode.
 */
    ساکن خالی
parse_command_name(mparm_T *parmp)
{
    char_u	*initstr;

    initstr = gettail((char_u *)parmp->argv[0]);

# ifdef FEAT_EVAL
    سیٹ_vim_var_string(VV_PROGNAME, initstr, -1);
    سیٹ_progpath((char_u *)parmp->argv[0]);
# endif

    اگر (TOLOWER_ASC(initstr[0]) == 'r')
    {
	restricted = TRUE;
	++initstr;
    }

    // Use evim mode for "evim" and "egvim", not for "editor".
    اگر (TOLOWER_ASC(initstr[0]) == 'e'
	    && (TOLOWER_ASC(initstr[1]) == 'v'
		|| TOLOWER_ASC(initstr[1]) == 'g'))
    {
# ifdef FEAT_GUI
	gui.starting = true;
# endif
	parmp->evim_منوال = TRUE;
	++initstr;
    }

    // "gvim" starts the GUI.  Also accept "Gvim" for MS-Windows.
    اگر (TOLOWER_ASC(initstr[0]) == 'g'
# ifdef VIMDLL
	    || mch_is_gui_executable()
# endif
       )
    {
	main_آغاز_gui();
# ifdef FEAT_GUI
	++initstr;
# endif
# ifdef GUI_MAY_SPAWN
	gui.dospawn = false;	// No need to spawn a new process.
# endif
    }
# ifdef GUI_MAY_SPAWN
    ورنہ
	gui.dospawn = true;	// Not "gvim". Need to spawn gvim.exe.
# endif


    اگر (STRNICMP(initstr, "view", 4) == 0)
    {
	readonlymode = TRUE;
	curbuf->b_p_ro = TRUE;
	p_uc = 10000;			// don't update very often
	initstr += 4;
    }
    ورنہ اگر (STRNICMP(initstr, "vim", 3) == 0)
	initstr += 3;

    // Catch "[r][g]vimdiff" and "[r][g]viewdiff".
    اگر (STRICMP(initstr, "diff") == 0)
    {
# ifdef FEAT_DIFF
	parmp->diff_منوال = TRUE;
# ورنہ
	mch_errmsg(_("This Vim was not compiled with the diff feature."));
	mch_errmsg("\n");
	mch_exit(2);
# endif
    }

    // Checking for "ex" here may catch some weird names, such as "vimex" or
    // "viewex", we assume the user knows that.
    اگر (STRNICMP(initstr, "ex", 2) == 0)
    {
	اگر (STRNICMP(initstr + 2, "im", 2) == 0)
	    exmode_فعال = EXMODE_VIM;
	ورنہ
	    exmode_فعال = EXMODE_معمول;
	change_compatible(TRUE);	// set 'compatible'
    }
}

/*
 * Scan the command line arguments.
 */
    ساکن خالی
command_سطر_جائزہ(mparm_T *parmp)
{
    صحیح		argc = parmp->argc;
    حرف	**argv = parmp->argv;
    صحیح		argv_idx;		// index in argv[n][]
    صحیح		had_minmin = FALSE;	// found "--" argument
    صحیح		want_دلیل;		// option argument with argument
    صحیح		c;
    char_u	*p = تہی;
    طویل	n;

    --argc;
    ++argv;
    argv_idx = 1;	    // active option letter is argv[0][argv_idx]
    جب (argc > 0)
    {
	/*
	 * "+" or "+{number}" or "+/{pat}" or "+{command}" argument.
	 */
	اگر (argv[0][0] == '+' && !had_minmin)
	{
	    اگر (parmp->n_commands >= MAX_ARG_CMDS)
		mainerr(ME_EXTRA_CMD, تہی);
	    argv_idx = -1;	    // skip to next argument
	    اگر (argv[0][1] == NUL)
		parmp->commands[parmp->n_commands++] = (char_u *)"$";
	    ورنہ
		parmp->commands[parmp->n_commands++] = (char_u *)&(argv[0][1]);
	}

	/*
	 * Optional argument.
	 */
	ورنہ اگر (argv[0][0] == '-' && !had_minmin)
	{
	    want_دلیل = FALSE;
	    c = argv[0][argv_idx++];
# اگر defined( VMS)
	    /* 2025-05-13  SMS
	     * On sufficiently recent non-VAX systems, case preservation
	     * of the command line is possible/routine.  And quotation
	     * always works, and is the expected method in such cases.
	     * However, leaving this slash-prefix scheme available is
	     * nearly harmless.  But note that it doesn't help with the
	     * case of other command-line arguments, such as file names.
	     * For details, see os_vms.c:vms_init().
	     * On VAX and old non-VAX systems, or with SET PROC/PARSE=TRAD,
	     * DCL upcases the command line, and the C RTL downcases it.
	     * I would not say "only uses upper case command lines".
	     */
	    /*
	     * VMS only uses upper case command lines.  Interpret "-X" as "-x"
	     * and "-/X" as "-X".
	     */
	    اگر (c == '/')
	    {
		c = argv[0][argv_idx++];
		c = TOUPPER_ASC(c);
	    }
	    /* Note that although DCL might upcase things, the C RTL
	     * will only downcase them, so there should be no need for
	     * the following (additional?) downcasing (which spoils the
	     * preserve-case results):
	     */
# endif /* defined( VMS) */
	    انتخاب (c)
	    {
	    حالت NUL:		// "vim -"  read from stdin
				// "ex -" silent mode
		اگر (exmode_فعال)
		    silent_منوال = TRUE;
		ورنہ
		{
		    اگر (parmp->edit_type != EDIT_NONE)
			mainerr(ME_TOO_MANY_دلائل, (char_u *)argv[0]);
		    parmp->edit_type = EDIT_STDIN;
		    read_cmd_fd = 2;	// read from stderr instead of stdin
		}
		argv_idx = -1;		// skip to next argument
		توڑو;

	    حالت '-':		// "--" don't take any more option arguments
				// "--help" give help message
				// "--version" give version message
				// "--clean" clean context
				// "--literal" take files literally
				// "--startuptime fname" write timing info
				// "--log fname" start logging early
				// "--nofork" don't fork
				// "--not-a-term" don't warn for not a term
				// "--gui-dialog-file fname" write dialog text
				// "--ttyfail" exit if not a term
				// "--stdio-channel" use stdin/stdout as channel
				// "--noplugin[s]" skip plugins
				// "--cmd <cmd>" execute cmd before vimrc
		اگر (STRICMP(argv[0] + argv_idx, "help") == 0)
		    usage();
		ورنہ اگر (STRICMP(argv[0] + argv_idx, "version") == 0)
		{
		    cmdline_چوڑائی = Columns = 80;   // need to init Columns
		    info_پیغام = TRUE; // use mch_msg(), not mch_errmsg()
# اگر defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
		    gui.starting = false; // not starting GUI, will exit
# endif
		    list_نسخہ();
		    پیغام_putchar('\n');
		    پیغام_didout = FALSE;
		    mch_exit(0);
		}
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "clean", 5) == 0)
		{
		    parmp->use_vimrc = (char_u *)"DEFAULTS";
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)"NONE";
# endif
		    parmp->clean = TRUE;
		    سیٹ_option_قدر_give_err((char_u *)"vif",
						      0L, (char_u *)"NONE", 0);
		}
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "literal", 7) == 0)
		{
# ifdef EXPAND_FILENAMES
		    parmp->literal = TRUE;
# endif
		}
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "nofork", 6) == 0)
		{
# ifdef FEAT_GUI
		    gui.dofork = false;	// don't fork() when starting GUI
# endif
		}
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "noplugin", 8) == 0)
		    p_lpl = FALSE;
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "not-a-term", 10) == 0)
		    parmp->not_a_رکن = TRUE;
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "gui-dialog-file", 15)
									 == 0)
		{
		    want_دلیل = TRUE;
		    argv_idx += 15;
		}
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "ttyfail", 7) == 0)
		    parmp->tty_fail = TRUE;
# ifdef FEAT_کام_CHANNEL
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "stdio-channel", 13) == 0)
		{
		    // Like "-es": no terminal, but the channel is served
		    // instead of reading Ex commands.
		    use_stdio_channel = TRUE;
		    exmode_فعال = EXMODE_معمول;
		    silent_منوال = TRUE;
#  اگر defined(FEAT_GUI) && !defined(VIMDLL)
		    gui.starting = false;	// don't start GUI
#  endif
		}
# endif
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "cmd", 3) == 0)
		{
		    want_دلیل = TRUE;
		    argv_idx += 3;
		}
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "startuptime", 11) == 0)
		{
		    want_دلیل = TRUE;
		    argv_idx += 11;
		}
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "log", 3) == 0)
		{
		    want_دلیل = TRUE;
		    argv_idx += 3;
		}
# ifdef FEAT_CLIENTSERVER
		ورنہ اگر (STRICMP(argv[0] + argv_idx, "serverlist") == 0)
		    ; // already processed -- no arg
		ورنہ اگر (STRICMP(argv[0] + argv_idx, "servername") == 0
		       || STRICMP(argv[0] + argv_idx, "serversend") == 0
		       // Don't put this under FEAT_CLIENTSERVER_BACKENDS, just
		       // let it be ignored. Makes tests less complicated
		       || STRICMP(argv[0] + argv_idx, "clientserver") == 0
		       )
		{
		    // already processed -- snatch the following arg
		    اگر (argc > 1)
		    {
			--argc;
			++argv;
		    }
		}
# endif
# اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
#  ifdef FEAT_GUI_GTK
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "socketid", 8) == 0)
#  ورنہ
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "windowid", 8) == 0)
#  endif
		{
		    // already processed -- snatch the following arg
		    اگر (argc > 1)
		    {
			--argc;
			++argv;
		    }
		}
# endif
# ifdef FEAT_GUI_GTK
		ورنہ اگر (STRNICMP(argv[0] + argv_idx, "echo-wid", 8) == 0)
		{
		    // already processed, skip
		}
# endif
		ورنہ
		{
		    اگر (argv[0][argv_idx])
			mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
		    had_minmin = TRUE;
		}
		اگر (!want_دلیل)
		    argv_idx = -1;	// skip to next argument
		توڑو;

	    حالت 'A':		// "-A" start in Arabic mode
# ifdef FEAT_ARABIC
		سیٹ_option_قدر_give_err((char_u *)"arabic", 1L, تہی, 0);
# ورنہ
		mch_errmsg(_(e_arabic_cannot_be_مستعمل_not_enabled_at_compile_وقت));
		mch_exit(2);
# endif
		توڑو;

	    حالت 'b':		// "-b" binary mode
		// Needs to be effective before expanding file names, because
		// for Win32 this makes us edit a shortcut file itself,
		// instead of the file it links to.
		سیٹ_options_bin(curbuf->b_p_bin, 1, 0);
		curbuf->b_p_bin = 1;	    // binary file I/O
		توڑو;

	    حالت 'C':		// "-C"  Compatible
		change_compatible(TRUE);
		has_dash_c_arg = TRUE;
		توڑو;

	    حالت 'e':		// "-e" Ex mode
		exmode_فعال = EXMODE_معمول;
		توڑو;

	    حالت 'E':		// "-E" Improved Ex mode
		exmode_فعال = EXMODE_VIM;
		توڑو;

	    حالت 'f':		// "-f"  GUI: run in foreground.  Amiga: open
				// window directly, not with newcli
# ifdef FEAT_GUI
		gui.dofork = false;	// don't fork() when starting GUI
# endif
		توڑو;

	    حالت 'g':		// "-g" start GUI
		main_آغاز_gui();
		توڑو;

	    حالت 'F':		// "-F" was for Farsi mode
		mch_errmsg(_(e_farsi_support_has_been_removed));
		mch_exit(2);
		توڑو;

	    حالت '?':		// "-?" give help message (for MS-Windows)
	    حالت 'h':		// "-h" give help message
# ifdef FEAT_GUI_GNOME
		// Tell usage() to exit for "gvim".
		gui.starting = false;
# endif
		usage();
		توڑو;

	    حالت 'H':		// "-H" start in Hebrew mode: rl + hkmap set
# ifdef FEAT_RIGHTLEFT
		p_hkmap = TRUE;
		سیٹ_option_قدر_give_err((char_u *)"rl", 1L, تہی, 0);
# ورنہ
		mch_errmsg(_(e_hebrew_cannot_be_مستعمل_not_enabled_at_compile_وقت));
		mch_exit(2);
# endif
		توڑو;

	    حالت 'l':		// "-l" lisp mode, 'lisp' and 'showmatch' on
		سیٹ_option_قدر_give_err((char_u *)"lisp", 1L, تہی, 0);
		p_sm = TRUE;
		توڑو;

	    حالت 'M':		// "-M"  no changes or writing of files
		reset_modifiable();
		// FALLTHROUGH

	    حالت 'm':		// "-m"  no writing of files
		p_write = FALSE;
		توڑو;

	    حالت 'y':		// "-y"  easy mode
# ifdef FEAT_GUI
		gui.starting = true;	// start GUI a bit later
# endif
		parmp->evim_منوال = TRUE;
		توڑو;

	    حالت 'N':		// "-N"  Nocompatible
		change_compatible(FALSE);
		توڑو;

	    حالت 'n':		// "-n" no swap file
# ifdef FEAT_NETBEANS_INTG
		// checking for "-nb", netbeans parameters
		اگر (argv[0][argv_idx] == 'b')
		{
		    netbeansArg = argv[0];
		    argv_idx = -1;	    // skip to next argument
		}
		ورنہ
# endif
		parmp->no_تبادلہ_file = TRUE;
		توڑو;

	    حالت 'p':		// "-p[N]" open N tab pages
# ifdef ہدف_API_MAC_OSX
		// For some reason on MacOS X, an argument like:
		// -psn_0_10223617 is passed in when invoke from Finder
		// or with the 'open' command
		اگر (argv[0][argv_idx] == 's')
		{
		    argv_idx = -1; // bypass full -psn
		    main_آغاز_gui();
		    توڑو;
		}
# endif
		// default is 0: open window for each file
		parmp->کھڑکی_گنتی = get_نمبر_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->کھڑکی_layout = WIN_TABS;
		توڑو;

	    حالت 'o':		// "-o[N]" open N horizontal split windows
		// default is 0: open window for each file
		parmp->کھڑکی_گنتی = get_نمبر_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->کھڑکی_layout = WIN_HOR;
		توڑو;

		حالت 'O':	// "-O[N]" open N vertical split windows
		// default is 0: open window for each file
		parmp->کھڑکی_گنتی = get_نمبر_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->کھڑکی_layout = WIN_VER;
		توڑو;

# ifdef FEAT_QUICKFIX
	    حالت 'q':		// "-q" QuickFix mode
		اگر (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_دلائل, (char_u *)argv[0]);
		parmp->edit_type = EDIT_QF;
		اگر (argv[0][argv_idx])		// "-q{errorfile}"
		{
		    parmp->use_ef = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		ورنہ اگر (argc > 1)		// "-q {errorfile}"
		    want_دلیل = TRUE;
		توڑو;
# endif

	    حالت 'R':		// "-R" readonly mode
		readonlymode = TRUE;
		curbuf->b_p_ro = TRUE;
		p_uc = 10000;			// don't update very often
		توڑو;

	    حالت 'r':		// "-r" recovery mode
	    حالت 'L':		// "-L" recovery mode
		recoverymode = 1;
		توڑو;

	    حالت 's':
		اگر (exmode_فعال)	// "-s" silent (batch) mode
		    silent_منوال = TRUE;
		ورنہ		// "-s {scriptin}" read from script file
		    want_دلیل = TRUE;
		توڑو;

	    حالت 't':		// "-t {tag}" or "-t{tag}" jump to tag
		اگر (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_دلائل, (char_u *)argv[0]);
		parmp->edit_type = EDIT_لیبل;
		اگر (argv[0][argv_idx])		// "-t{tag}"
		{
		    parmp->tagname = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		ورنہ				// "-t {tag}"
		    want_دلیل = TRUE;
		توڑو;

# ifdef FEAT_EVAL
	    حالت 'D':		// "-D"		Debugging
		parmp->use_debug_break_level = 9999;
		توڑو;
# endif
# ifdef FEAT_DIFF
	    حالت 'd':		// "-d"		'diff'
#  ifdef AMIGA
		// check for "-dev {device}"
		اگر (argv[0][argv_idx] == 'e' && argv[0][argv_idx + 1] == 'v')
		    want_دلیل = TRUE;
		ورنہ
#  endif
		    parmp->diff_منوال = TRUE;
		توڑو;
# endif
	    حالت 'V':		// "-V{N}"	Verbose level
		// default is 10: a little bit verbose
		p_verbose = get_نمبر_arg((char_u *)argv[0], &argv_idx, 10);
		اگر (argv[0][argv_idx] != NUL)
		{
		    سیٹ_option_قدر_give_err((char_u *)"verbosefile",
					  0L, (char_u *)argv[0] + argv_idx, 0);
		    argv_idx = (صحیح)STRLEN(argv[0]);
		}
		توڑو;

	    حالت 'v':		// "-v"  Vi-mode (as if called "vi")
		exmode_فعال = 0;
# اگر defined(FEAT_GUI) && !defined(VIMDLL)
		gui.starting = false;	// don't start GUI
# endif
		توڑو;

	    حالت 'w':		// "-w{number}"	set window height
				// "-w {scriptout}"	write to script
		اگر (vim_isdigit(((char_u *)argv[0])[argv_idx]))
		{
		    n = get_نمبر_arg((char_u *)argv[0], &argv_idx, 10);
		    سیٹ_option_قدر_give_err((char_u *)"window", n, تہی, 0);
		    توڑو;
		}
		want_دلیل = TRUE;
		توڑو;

# ifdef FEAT_CRYPT
	    حالت 'x':		// "-x"  encrypted reading/writing of files
		parmp->ask_for_کلید = TRUE;
		توڑو;
# endif

	    حالت 'X':		// "-X"  don't connect to X server
# اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
		x_no_connect = TRUE;
# endif
		توڑو;
	    حالت 'Y':		// "-Y" don't connect to Wayland compositor
# اگر defined(FEAT_WAYLAND)
		wayland_no_connect = TRUE;
# endif
		توڑو;

	    حالت 'Z':		// "-Z"  restricted mode
		restricted = TRUE;
		توڑو;

	    حالت 'c':		// "-c{command}" or "-c {command}" execute
				// command
		اگر (argv[0][argv_idx] != NUL)
		{
		    اگر (parmp->n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, تہی);
		    parmp->commands[parmp->n_commands++] = (char_u *)argv[0]
								   + argv_idx;
		    argv_idx = -1;
		    توڑو;
		}
		// FALLTHROUGH
	    حالت 'S':		// "-S {file}" execute Vim script
	    حالت 'i':		// "-i {viminfo}" use for viminfo
# ifndef FEAT_DIFF
	    حالت 'd':		// "-d {device}" device (for Amiga)
# endif
	    حالت 'T':		// "-T {terminal}" terminal name
	    حالت 'u':		// "-u {vimrc}" vim inits file
	    حالت 'U':		// "-U {gvimrc}" gvim inits file
	    حالت 'W':		// "-W {scriptout}" overwrite
# ifdef FEAT_GUI_MSWIN
	    حالت 'P':		// "-P {parent title}" MDI parent
# endif
		want_دلیل = TRUE;
		توڑو;

	    طےشدہ:
		mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
	    }

	    /*
	     * Handle option arguments with argument.
	     */
	    اگر (want_دلیل)
	    {
		/*
		 * Check for garbage immediately after the option letter.
		 */
		اگر (argv[0][argv_idx] != NUL)
		    mainerr(ME_GARBAGE, (char_u *)argv[0]);

		--argc;
		اگر (argc < 1 && c != 'S')  // -S has an optional argument
		    mainerr_arg_missing((char_u *)argv[0]);
		++argv;
		argv_idx = -1;

		انتخاب (c)
		{
		حالت 'c':	// "-c {command}" execute command
		حالت 'S':	// "-S {file}" execute Vim script
		    اگر (parmp->n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, تہی);
		    اگر (c == 'S')
		    {
			حرف	*a;

			اگر (argc < 1)
			    // "-S" without argument: use default session file
			    // name.
			    a = SESSION_FILE;
			ورنہ اگر (argv[0][0] == '-')
			{
			    // "-S" followed by another option: use default
			    // session file name.
			    a = SESSION_FILE;
			    ++argc;
			    --argv;
			}
			ورنہ
			    a = argv[0];
			p = alloc(STRLEN(a) + 4);
			اگر (p == تہی)
			    mch_exit(2);
			sprintf((حرف *)p, "so %s", a);
			parmp->cmds_tofree[parmp->n_commands] = TRUE;
			parmp->commands[parmp->n_commands++] = p;
		    }
		    ورنہ
			parmp->commands[parmp->n_commands++] =
							    (char_u *)argv[0];
		    توڑو;

		حالت '-':
		    اگر (argv[-1][2] == 'c')
		    {
			// "--cmd {command}" execute command
			اگر (parmp->n_pre_commands >= MAX_ARG_CMDS)
			    mainerr(ME_EXTRA_CMD, تہی);
			parmp->pre_commands[parmp->n_pre_commands++] =
							    (char_u *)argv[0];
		    }
		    // --gui-dialog-file fname
		    اگر (argv[-1][2] == 'g')
		    {
			// without GUI ignore the argument
# ifdef FEAT_GUI
			parmp->gui_dialog_file = (char_u *)argv[0];
# endif
		    }

		    // "--startuptime <file>" already handled
		    // "--log <file>" already handled
		    توڑو;

	    //	case 'd':   -d {device} is handled in mch_check_win() for the
	    //		    Amiga

# ifdef FEAT_QUICKFIX
		حالت 'q':	// "-q {errorfile}" QuickFix mode
		    parmp->use_ef = (char_u *)argv[0];
		    توڑو;
# endif

		حالت 'i':	// "-i {viminfo}" use for viminfo
		    سیٹ_option_قدر_give_err((char_u *)"vif",
						     0L, (char_u *)argv[0], 0);
		    توڑو;

		حالت 's':	// "-s {scriptin}" read from script file
		    اگر (scriptin[0] != تہی)
		    {
scripterror:
			mch_errmsg(_("Attempt to open script file again: \""));
			mch_errmsg(argv[-1]);
			mch_errmsg(" ");
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    اگر ((scriptin[0] = mch_fopen(argv[0], READBIN)) == تہی)
		    {
			mch_errmsg(_("Cannot open for reading: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    اگر (save_typebuf() == FAIL)
			mch_exit(2);	// out of memory
		    توڑو;

		حالت 't':	// "-t {tag}"
		    parmp->tagname = (char_u *)argv[0];
		    توڑو;

		حالت 'T':	// "-T {terminal}" terminal name
		    /*
		     * The -T term argument is always available and when
		     * HAVE_TERMLIB is supported it overrides the environment
		     * variable TERM.
		     */
# ifdef FEAT_GUI
		    اگر (رکن_is_gui((char_u *)argv[0]))
			gui.starting = true;	// start GUI a bit later
		    ورنہ
# endif
			parmp->رکن = (char_u *)argv[0];
		    توڑو;

		حالت 'u':	// "-u {vimrc}" vim inits file
		    parmp->use_vimrc = (char_u *)argv[0];
		    توڑو;

		حالت 'U':	// "-U {gvimrc}" gvim inits file
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)argv[0];
# endif
		    توڑو;

		حالت 'w':	// "-w {nr}" 'window' value
				// "-w {scriptout}" append to script file
		    اگر (vim_isdigit(*((char_u *)argv[0])))
		    {
			argv_idx = 0;
			n = get_نمبر_arg((char_u *)argv[0], &argv_idx, 10);
			سیٹ_option_قدر_give_err((char_u *)"window",
								   n, تہی, 0);
			argv_idx = -1;
			توڑو;
		    }
		    // FALLTHROUGH
		حالت 'W':	// "-W {scriptout}" overwrite script file
		    اگر (scriptout != تہی)
			جاؤ scripterror;
		    اگر ((scriptout = mch_fopen(argv[0],
				    c == 'w' ? APPENDBIN : WRITEBIN)) == تہی)
		    {
			mch_errmsg(_("Cannot open for script output: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    توڑو;

# ifdef FEAT_GUI_MSWIN
		حالت 'P':		// "-P {parent title}" MDI parent
		    gui_mch_سیٹ_والد(argv[0]);
		    توڑو;
# endif
		}
	    }
	}

	/*
	 * File name argument.
	 */
	ورنہ
	{
	    argv_idx = -1;	    // skip to next argument

	    // Check for only one type of editing.
	    اگر (parmp->edit_type != EDIT_NONE && parmp->edit_type != EDIT_FILE)
		mainerr(ME_TOO_MANY_دلائل, (char_u *)argv[0]);
	    parmp->edit_type = EDIT_FILE;

# ifdef MSWIN
	    // Remember if the argument was a full path before changing
	    // slashes to backslashes.
	    اگر (argv[0][0] != NUL && argv[0][1] == ':' && argv[0][2] == '\\')
		parmp->بھرا_راستہ = TRUE;
# endif

	    // Add the file to the global argument list.
	    اگر (ga_grow(&global_alist.al_ga, 1) == FAIL
		    || (p = vim_strsave((char_u *)argv[0])) == تہی)
		mch_exit(2);
# ifdef FEAT_DIFF
	    اگر (parmp->diff_منوال && mch_isdir(p) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *دم;
		string_T    ret;

		دم = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(p, STRLEN(p), دم, STRLEN(دم), TRUE, &ret);
		اگر (ret.string != تہی)
		{
		    vim_free(p);
		    p = ret.string;
		}
	    }
# endif
# ifdef __CYGWIN__
	    /*
	     * If vim is invoked by non-Cygwin tools, convert away any
	     * DOS paths, so things like .swp files are created correctly.
	     * Look for evidence of non-Cygwin paths before we bother.
	     * This is only for when using the Unix files.
	     */
	    اگر (vim_strpbrk(p, "\\:") != تہی && !راستہ_with_url(p))
	    {
		حرف posix_راستہ[MAXPATHL];

#  اگر CYGWIN_نسخہ_DLL_MAJOR >= 1007
		cygwin_conv_راستہ(CCP_WIN_A_مقصد_POSIX, p, posix_راستہ, MAXPATHL);
#  ورنہ
		cygwin_conv_مقصد_posix_راستہ(p, posix_راستہ);
#  endif
		vim_free(p);
		p = vim_strsave((char_u *)posix_راستہ);
		اگر (p == تہی)
		    mch_exit(2);
	    }
# endif

# ifdef USE_FNAME_CASE
	    // Make the case of the file name match the actual file.
	    fname_case(p, 0);
# endif

	    alist_جوڑو(&global_alist, p,
# ifdef EXPAND_FILENAMES
		    parmp->literal ? 2 : 0	// add buffer nr after exp.
# ورنہ
		    2		// add buffer number now and use curbuf
# endif
		    );

# ifdef MSWIN
	    {
		// Remember this argument has been added to the argument list.
		// Needed when 'encoding' is changed.
		مستعمل_file_arg(argv[0], parmp->literal, parmp->بھرا_راستہ,
#  ifdef FEAT_DIFF
							    parmp->diff_منوال
#  ورنہ
							    FALSE
#  endif
							    );
	    }
# endif
	}

	/*
	 * If there are no more letters after the current "-", go to next
	 * argument.  argv_idx is set to -1 when the current argument is to be
	 * skipped.
	 */
	اگر (argv_idx <= 0 || argv[0][argv_idx] == NUL)
	{
	    --argc;
	    ++argv;
	    argv_idx = 1;
	}
    }

# ifdef FEAT_EVAL
    // If there is a "+123" or "-c" command, set v:swapcommand to the first
    // one.
    اگر (parmp->n_commands > 0)
    {
	size_t  plen;

	plen = STRLEN(parmp->commands[0]) + 2;
	p = alloc(plen + 1);
	اگر (p != تہی)
	{
	    sprintf((حرف *)p, ":%s\r", parmp->commands[0]);
	    سیٹ_vim_var_string(VV_SWAPCOMMAND, p, (صحیح)plen);
	    vim_free(p);
	}
    }
# endif
}

/*
 * Print a warning if stdout is not a terminal.
 * When starting in Ex mode and commands come from a file, set silent_mode.
 */
    ساکن خالی
check_tty(mparm_T *parmp)
{
    صحیح		داخلہ_isatty;		// is active input a terminal?

    داخلہ_isatty = mch_داخلہ_isatty();
    اگر (exmode_فعال)
    {
	اگر (!داخلہ_isatty)
	    silent_منوال = TRUE;
    }
    ورنہ اگر (parmp->want_بھرا_screen && (!stdout_isatty || !داخلہ_isatty)
# ifdef FEAT_GUI
	    // don't want the delay when started from the desktop
	    && !gui.starting
# endif
	    && !parmp->not_a_رکن)
    {
# ifdef NBDEBUG
	/*
	 * This shouldn't be necessary. But if I run netbeans with the log
	 * output coming to the console and XOpenDisplay fails, I get vim
	 * trying to start with input/output to my console tty.  This fills my
	 * input buffer so fast I can't even kill the process in under 2
	 * minutes (and it beeps continuously the whole time :-)
	 */
	اگر (netbeans_فعال() && (!stdout_isatty || !داخلہ_isatty))
	{
	    mch_errmsg(_("Vim: Error: Failure to start gvim from NetBeans\n"));
	    باہر_نکلو(1);
	}
# endif
# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	اگر (
#  ifdef VIMDLL
	    !gui.starting &&
#  endif
	    is_cygpty_مستعمل())
	{
#  اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET) \
	&& defined(FEAT_GETTEXT)
	    حرف    *s, *tofree = تہی;

	    // Set the encoding of the error message based on $LC_ALL or
	    // other environment variables instead of 'encoding'.
	    // Note that the message is shown on a Cygwin terminal (e.g.
	    // mintty) which encoding is based on $LC_ALL or etc., not the
	    // current codepage used by normal Win32 console programs.
	    tofree = s = (حرف *)enc_locale_env(تہی);
	    اگر (s == تہی)
		s = "utf-8";	// Use "utf-8" by default.
	    (خالی)bind_textdomain_codeset(VIMPACKAGE, s);
	    vim_free(tofree);
#  endif
	    mch_errmsg(_("Vim: Error: This version of Vim does not run in a Cygwin terminal\n"));
	    باہر_نکلو(1);
	}
# endif
	اگر (!stdout_isatty)
	    mch_errmsg(_("Vim: Warning: Output is not to a terminal\n"));
	اگر (!داخلہ_isatty)
	    mch_errmsg(_("Vim: Warning: Input is not from a terminal\n"));
	برآمد_flush();
	اگر (parmp->tty_fail && (!stdout_isatty || !داخلہ_isatty))
	    باہر_نکلو(1);
	اگر (scriptin[0] == تہی)
	    ui_delay(2005L, TRUE);
	وقت_پیغام("Warning delay");
    }
}

/*
 * Read text from stdin.
 */
    ساکن خالی
read_stdin(خالی)
{
    صحیح	    i;

    // When getting the ATTENTION prompt here, use a dialog
    تبادلہ_exists_action = SEA_DIALOG;

    no_wait_return = TRUE;
    i = پیغام_didany;
    سیٹ_buflisted(TRUE);

    // Create memfile and read from stdin.
    (خالی)open_بفر(TRUE, تہی, 0);

    no_wait_return = FALSE;
    پیغام_didany = i;
    وقت_پیغام("reading stdin");

    check_تبادلہ_exists_action();

# اگر !(defined(AMIGA) || defined(MACOS_X))
    // Dup stdin from stderr to read commands from, so that shell commands
    // work.
    // TODO: why is this needed, even though readfile() has done this?
    close(0);
    vim_ignored = dup(2);
# endif
}

/*
 * Create the requested number of windows and edit buffers in them.
 * Also does recovery if "recoverymode" set.
 */
    ساکن خالی
create_windows(mparm_T *parmp غیر_مستعمل)
{
    صحیح		dorewind;
    صحیح		مکمل = 0;

    /*
     * Create the number of windows that was requested.
     */
    اگر (parmp->کھڑکی_گنتی == -1)	// was not set
	parmp->کھڑکی_گنتی = 1;
    اگر (parmp->کھڑکی_گنتی == 0)
	parmp->کھڑکی_گنتی = GARGCOUNT;
    اگر (parmp->کھڑکی_گنتی > 1)
    {
	// Don't change the windows if there was a command in .vimrc that
	// already split some windows
	اگر (parmp->کھڑکی_layout == 0)
	    parmp->کھڑکی_layout = WIN_HOR;
	اگر (parmp->کھڑکی_layout == WIN_TABS)
	{
	    parmp->کھڑکی_گنتی = make_tabpages(parmp->کھڑکی_گنتی);
	    وقت_پیغام("making tab pages");
	}
	ورنہ اگر (firstwin->w_اگلا == تہی)
	{
	    parmp->کھڑکی_گنتی = make_windows(parmp->کھڑکی_گنتی,
					     parmp->کھڑکی_layout == WIN_VER);
	    وقت_پیغام("making windows");
	}
	ورنہ
	    parmp->کھڑکی_گنتی = win_گنتی();
    }
    ورنہ
	parmp->کھڑکی_گنتی = 1;

    اگر (recoverymode)			// do recover
    {
	پیغام_scroll = TRUE;		// scroll message up
	ml_recover(TRUE);
	اگر (curbuf->b_ml.ml_mfp == تہی) // failed
	    getout(1);
	do_modelines(0);		// do modelines
    }
    ورنہ
    {
	/*
	 * Open a buffer for windows that don't have one yet.
	 * Commands in the .vimrc might have loaded a file or split the window.
	 * Watch out for autocommands that delete a window.
	 */
	/*
	 * Don't execute Win/Buf Enter/Leave autocommands here
	 */
	++autocmd_no_enter;
	++autocmd_no_leave;
	dorewind = TRUE;
	جب (مکمل++ < 1000)
	{
	    اگر (dorewind)
	    {
		اگر (parmp->کھڑکی_layout == WIN_TABS)
		    goto_tabpage(1);
		ورنہ
		    curwin = firstwin;
	    }
	    ورنہ اگر (parmp->کھڑکی_layout == WIN_TABS)
	    {
		اگر (curtab->tp_اگلا == تہی)
		    توڑو;
		goto_tabpage(0);
	    }
	    ورنہ
	    {
		اگر (curwin->w_اگلا == تہی)
		    توڑو;
		curwin = curwin->w_اگلا;
	    }
	    dorewind = FALSE;
	    curbuf = curwin->w_بفر;
	    اگر (curbuf->b_ml.ml_mfp == تہی)
	    {
# ifdef FEAT_FOLDING
		// Set 'foldlevel' to 'foldlevelstart' if it's not negative.
		اگر (p_fdls >= 0)
		    curwin->w_p_fdl = p_fdls;
# endif
		// When getting the ATTENTION prompt here, use a dialog
		تبادلہ_exists_action = SEA_DIALOG;

		سیٹ_buflisted(TRUE);

		// create memfile, read file
		(خالی)open_بفر(FALSE, تہی, 0);

		اگر (تبادلہ_exists_action == SEA_QUIT)
		{
		    اگر (got_int || only_one_کھڑکی())
		    {
			// abort selected or quit and only one window
			did_emsg = FALSE;   // avoid hit-enter prompt
			getout(1);
		    }
		    // We can't close the window, it would disturb what
		    // happens next.  Clear the file name and set the arg
		    // index to -1 to delete it later.
		    setfname(curbuf, تہی, تہی, FALSE);
		    curwin->w_arg_idx = -1;
		    تبادلہ_exists_action = SEA_NONE;
		}
		ورنہ
		    handle_تبادلہ_exists(تہی);
		dorewind = TRUE;		// start again
	    }
	    ui_breakcheck();
	    اگر (got_int)
	    {
		(خالی)vgetc();	// only break the file loading, not the rest
		توڑو;
	    }
	}
	اگر (parmp->کھڑکی_layout == WIN_TABS)
	    goto_tabpage(1);
	ورنہ
	    curwin = firstwin;
	curbuf = curwin->w_بفر;
	--autocmd_no_enter;
	--autocmd_no_leave;
    }
}

/*
 * If opened more than one window, start editing files in the other
 * windows.  make_windows() has already opened the windows.
 */
    ساکن خالی
edit_buffers(
    mparm_T	*parmp,
    char_u	*cwd)			// current working dir
{
    صحیح		arg_idx;		// index in argument list
    صحیح		i;
    صحیح		advance = TRUE;
    win_T	*win;
    char_u	*p_shm_save = تہی;

    /*
     * Don't execute Win/Buf Enter/Leave autocommands here
     */
    ++autocmd_no_enter;
    ++autocmd_no_leave;

    // When w_arg_idx is -1 remove the window (see create_windows()).
    اگر (curwin->w_arg_idx == -1)
    {
	win_close(curwin, TRUE);
	advance = FALSE;
    }

    arg_idx = 1;
    برائے (i = 1; i < parmp->کھڑکی_گنتی; ++i)
    {
	اگر (cwd != تہی)
	    mch_chdir((حرف *)cwd);
	// When w_arg_idx is -1 remove the window (see create_windows()).
	اگر (curwin->w_arg_idx == -1)
	{
	    ++arg_idx;
	    win_close(curwin, TRUE);
	    advance = FALSE;
	    جاری;
	}

	اگر (advance)
	{
	    اگر (parmp->کھڑکی_layout == WIN_TABS)
	    {
		اگر (curtab->tp_اگلا == تہی)	// just checking
		    توڑو;
		goto_tabpage(0);
		// Temporarily reset 'shm' option to not print fileinfo when
		// loading the other buffers. This would overwrite the already
		// existing fileinfo for the first tab.
		اگر (i == 1)
		{
		    حرف buf[100];

		    p_shm_save = vim_strsave(p_shm);
		    vim_snprintf(buf, 100, "F%s", p_shm);
		    سیٹ_option_قدر_give_err((char_u *)"shm",
							 0L, (char_u *)buf, 0);
		}
	    }
	    ورنہ
	    {
		اگر (curwin->w_اگلا == تہی)	// just checking
		    توڑو;
		win_enter(curwin->w_اگلا, FALSE);
	    }
	}
	advance = TRUE;

	// Only open the file if there is no file in this window yet (that can
	// happen when .vimrc contains ":sall").
	اگر (curbuf == firstwin->w_بفر || curbuf->b_ffname == تہی)
	{
	    curwin->w_arg_idx = arg_idx;
	    // Edit file from arg list, if there is one.  When "Quit" selected
	    // at the ATTENTION prompt close the window.
	    تبادلہ_exists_did_quit = FALSE;
	    (خالی)do_ecmd(0, arg_idx < GARGCOUNT
			  ? alist_name(&GARGLIST[arg_idx]) : تہی,
			  تہی, تہی, ECMD_LASTL, ECMD_HIDE, curwin);
	    اگر (تبادلہ_exists_did_quit)
	    {
		// abort or quit selected
		اگر (got_int || only_one_کھڑکی())
		{
		    // abort selected and only one window
		    did_emsg = FALSE;   // avoid hit-enter prompt
		    getout(1);
		}
		win_close(curwin, TRUE);
		advance = FALSE;
	    }
	    اگر (arg_idx == GARGCOUNT - 1)
		arg_had_آخری = TRUE;
	    ++arg_idx;
	}
	ui_breakcheck();
	اگر (got_int)
	{
	    (خالی)vgetc();	// only break the file loading, not the rest
	    توڑو;
	}
    }

    اگر (p_shm_save != تہی)
    {
	سیٹ_option_قدر_give_err((char_u *)"shm", 0L, p_shm_save, 0);
	vim_free(p_shm_save);
    }

    اگر (parmp->کھڑکی_layout == WIN_TABS)
	goto_tabpage(1);
    --autocmd_no_enter;

    // make the first window the current window
    win = firstwin;
# اگر defined(FEAT_QUICKFIX)
    // Avoid making a preview window the current window.
    جب (win->w_p_pvw)
    {
	win = win->w_اگلا;
	اگر (win == تہی)
	{
	    win = firstwin;
	    توڑو;
	}
    }
# endif
    win_enter(win, FALSE);

    --autocmd_no_leave;
    وقت_پیغام("editing files in windows");
    اگر (parmp->کھڑکی_گنتی > 1 && parmp->کھڑکی_layout != WIN_TABS)
	win_equal(curwin, FALSE, 'b');	// adjust heights
}

/*
 * Execute the commands from --cmd arguments "cmds[cnt]".
 */
    ساکن خالی
exe_pre_commands(mparm_T *parmp)
{
    char_u	**cmds = parmp->pre_commands;
    صحیح		cnt = parmp->n_pre_commands;
    صحیح		i;
    ESTACK_CHECK_DECLARATION;

    اگر (cnt <= 0)
	واپس;

    curwin->w_cursor.lnum = 0; // just in case..
    estack_ڈالو(ETYPE_دلائل, (char_u *)_("pre-vimrc command line"), 0);
    ESTACK_CHECK_تیاری;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CMDARG;
# endif
    برائے (i = 0; i < cnt; ++i)
	do_cmdline_cmd(cmds[i]);
    ESTACK_CHECK_NOW;
    estack_نکالو();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    وقت_پیغام("--cmd commands");
}

/*
 * Execute "+", "-c" and "-S" arguments.
 */
    ساکن خالی
exe_commands(mparm_T *parmp)
{
    صحیح		i;
    ESTACK_CHECK_DECLARATION;

    /*
     * We start commands on line 0, make "vim +/pat file" match a
     * pattern on line 1.  But don't move the cursor when an autocommand
     * with g`" was used.
     */
    پیغام_scroll = TRUE;
    اگر (parmp->tagname == تہی && curwin->w_cursor.lnum <= 1)
	curwin->w_cursor.lnum = 0;
    estack_ڈالو(ETYPE_دلائل, (char_u *)"command line", 0);
    ESTACK_CHECK_تیاری;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CARG;
    current_sctx.sc_seq = 0;
# endif
    برائے (i = 0; i < parmp->n_commands; ++i)
    {
	do_cmdline_cmd(parmp->commands[i]);
	اگر (parmp->cmds_tofree[i])
	    vim_free(parmp->commands[i]);
    }
    ESTACK_CHECK_NOW;
    estack_نکالو();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    اگر (curwin->w_cursor.lnum == 0)
	curwin->w_cursor.lnum = 1;

    اگر (!exmode_فعال)
	پیغام_scroll = FALSE;

# ifdef FEAT_QUICKFIX
    // When started with "-q errorfile" jump to first error again.
    اگر (parmp->edit_type == EDIT_QF)
	qf_jump(تہی, 0, 0, FALSE);
# endif
    وقت_پیغام("executing command arguments");
}

/*
 * Source startup scripts.
 */
    ساکن خالی
ماخذ_startup_scripts(mparm_T *parmp)
{
    صحیح		i;

    /*
     * For "evim" source evim.vim first of all, so that the user can overrule
     * any things he doesn't like.
     */
    اگر (parmp->evim_منوال)
    {
	(خالی)do_ماخذ((char_u *)EVIM_FILE, FALSE, DOSO_NONE, تہی);
	وقت_پیغام("source evim file");
    }

    /*
     * If -u argument given, use only the initializations from that file and
     * nothing else.
     */
    اگر (parmp->use_vimrc != تہی)
    {
	اگر (STRCMP(parmp->use_vimrc, "DEFAULTS") == 0)
	{
	    اگر (do_ماخذ((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE, تہی)
									 != OK)
		emsg(_(e_failed_مقصد_ماخذ_defaults));
	}
	ورنہ اگر (STRCMP(parmp->use_vimrc, "NONE") == 0
				     || STRCMP(parmp->use_vimrc, "NORC") == 0)
	{
# ifdef FEAT_GUI
	    اگر (use_gvimrc == تہی)	    // don't load gvimrc either
		use_gvimrc = parmp->use_vimrc;
# endif
	}
	ورنہ
	{
	    اگر (do_ماخذ(parmp->use_vimrc, FALSE, DOSO_NONE, تہی) != OK)
		semsg(_(e_cannot_read_from_str_2), parmp->use_vimrc);
	}
    }
    ورنہ اگر (!silent_منوال)
    {
# ifdef AMIGA
	ساخت Process	*proc = (ساخت Process *)ڈھونڈو_Task(0L);
	APTR		save_winptr = proc->pr_کھڑکی_Ptr;

	// Avoid a requester here for a volume that doesn't exist.
	proc->pr_کھڑکی_Ptr = (APTR)-1L;
# endif

	/*
	 * Get system wide defaults, if the file name is defined.
	 */
# ifdef SYS_VIMRC_FILE
	(خالی)do_ماخذ((char_u *)SYS_VIMRC_FILE, FALSE, DOSO_NONE, تہی);
# endif
# ifdef MACOS_X
	(خالی)do_ماخذ((char_u *)"$VIMRUNTIME/macmap.vim", FALSE,
							      DOSO_NONE, تہی);
# endif

	/*
	 * Try to read initialization commands from the following places:
	 * - environment variable VIMINIT
	 * - user vimrc file (s:.vimrc for Amiga, ~/.vimrc otherwise)
	 * - second user vimrc file ($VIM/.vimrc for Dos)
	 * - environment variable EXINIT
	 * - user exrc file (s:.exrc for Amiga, ~/.exrc otherwise)
	 * - second user exrc file ($VIM/.exrc for Dos)
	 * The first that exists is used, the rest is ignored.
	 */
	اگر (process_env((char_u *)"VIMINIT", TRUE) != OK)
	{
	    اگر (do_ماخذ((char_u *)USR_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, تہی) == FAIL
# ifdef USR_VIMRC_FILE2
		&& do_ماخذ((char_u *)USR_VIMRC_FILE2, TRUE,
						      DOSO_VIMRC, تہی) == FAIL
# endif
# ifdef XDG_VIMRC_FILE
		&& do_ماخذ((char_u *)XDG_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, تہی) == FAIL
# endif
# ifdef USR_VIMRC_FILE3
		&& do_ماخذ((char_u *)USR_VIMRC_FILE3, TRUE,
						      DOSO_VIMRC, تہی) == FAIL
# endif
# ifdef USR_VIMRC_FILE4
		&& do_ماخذ((char_u *)USR_VIMRC_FILE4, TRUE,
						      DOSO_VIMRC, تہی) == FAIL
# endif
		&& process_env((char_u *)"EXINIT", FALSE) == FAIL
		&& do_ماخذ((char_u *)USR_EXRC_FILE, FALSE,
						       DOSO_NONE, تہی) == FAIL
# ifdef USR_EXRC_FILE2
		&& do_ماخذ((char_u *)USR_EXRC_FILE2, FALSE,
						       DOSO_NONE, تہی) == FAIL
# endif
		&& !has_dash_c_arg)
	    {
		// When no .vimrc file was found: source defaults.vim.
		اگر (do_ماخذ((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE,
								 تہی) == FAIL)
		    emsg(_(e_failed_مقصد_ماخذ_defaults));
	    }
	}

	/*
	 * Read initialization commands from ".vimrc" or ".exrc" in current
	 * directory.  This is only done if the 'exrc' option is set.
	 * Because of security reasons we disallow shell and write commands
	 * now, except for Unix if the file is owned by the user or 'secure'
	 * option has been reset in environment of global ".exrc" or ".vimrc".
	 * Only do this if VIMRC_FILE is not the same as USR_VIMRC_FILE or
	 * SYS_VIMRC_FILE.
	 */
	اگر (p_exrc)
	{
# اگر defined(UNIX) || defined(VMS)
	    // If ".vimrc" file is not owned by user, set 'secure' mode.
	    اگر (!file_owned(VIMRC_FILE))
# endif
		secure = p_secure;

	    i = FAIL;
	    اگر (fullpathcmp((char_u *)USR_VIMRC_FILE,
				(char_u *)VIMRC_FILE, FALSE, TRUE) != FPC_SAME
# ifdef USR_VIMRC_FILE2
		    && fullpathcmp((char_u *)USR_VIMRC_FILE2,
				(char_u *)VIMRC_FILE, FALSE, TRUE) != FPC_SAME
# endif
# ifdef USR_VIMRC_FILE3
		    && fullpathcmp((char_u *)USR_VIMRC_FILE3,
				(char_u *)VIMRC_FILE, FALSE, TRUE) != FPC_SAME
# endif
# ifdef SYS_VIMRC_FILE
		    && fullpathcmp((char_u *)SYS_VIMRC_FILE,
				(char_u *)VIMRC_FILE, FALSE, TRUE) != FPC_SAME
# endif
				)
		i = do_ماخذ((char_u *)VIMRC_FILE, TRUE, DOSO_VIMRC, تہی);

	    اگر (i == FAIL)
	    {
# اگر defined(UNIX) || defined(VMS)
		// if ".exrc" is not owned by user set 'secure' mode
		اگر (!file_owned(EXRC_FILE))
		    secure = p_secure;
		ورنہ
		    secure = 0;
# endif
		اگر (	   fullpathcmp((char_u *)USR_EXRC_FILE,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# ifdef USR_EXRC_FILE2
			&& fullpathcmp((char_u *)USR_EXRC_FILE2,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# endif
				)
		    (خالی)do_ماخذ((char_u *)EXRC_FILE, FALSE,
							      DOSO_NONE, تہی);
	    }
	}
	اگر (secure == 2)
	    need_wait_return = TRUE;
	secure = 0;
# ifdef AMIGA
	proc->pr_کھڑکی_Ptr = save_winptr;
# endif
    }
    وقت_پیغام("sourcing vimrc file(s)");
}

/*
 * Setup to start using the GUI.  Exit with an error when not available.
 */
    ساکن خالی
main_آغاز_gui(خالی)
{
# ifdef FEAT_GUI
    gui.starting = true;	// start GUI a bit later
# ورنہ
    mch_errmsg(_(e_gui_cannot_be_مستعمل_not_enabled_at_compile_وقت));
    mch_errmsg("\n");
    mch_exit(2);
# endif
}

#اگر_ختم  // NO_VIM_MAIN

/*
 * Get an environment variable and execute it as Ex commands.
 * Returns FAIL if the environment variable was not executed, OK otherwise.
 */
    صحیح
process_env(
    char_u	*env,
    صحیح		is_viminit) // when TRUE, called for VIMINIT
{
    char_u	*initstr;
    sctx_T	save_current_sctx;
    ESTACK_CHECK_DECLARATION;

    اگر ((initstr = mch_getenv(env)) == تہی || *initstr == NUL)
	واپس FAIL;

    اگر (is_viminit)
	vimrc_ملا(تہی, تہی);
    estack_ڈالو(ETYPE_ENV, env, 0);
    ESTACK_CHECK_تیاری;
    save_current_sctx = current_sctx;
    current_sctx.sc_نسخہ = 1;
#اگر_تعریف_شدہ FEAT_EVAL
    current_sctx.sc_sid = SID_ENV;
    current_sctx.sc_seq = 0;
    current_sctx.sc_lnum = 0;
#اگر_ختم

    do_cmdline_cmd(initstr);

    ESTACK_CHECK_NOW;
    estack_نکالو();
    current_sctx = save_current_sctx;
    واپس OK;
}

#اگر (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
/*
 * Return TRUE if we are certain the user owns the file "fname".
 * Used for ".vimrc" and ".exrc".
 * Use both stat() and lstat() for extra security.
 */
    ساکن صحیح
file_owned(حرف *fname)
{
    stat_T	s;
# ifdef UNIX
    uid_t	uid = getuid();
# ورنہ	 // VMS
    uid_t	uid = ((getgid() << 16) | getuid());
# endif

    واپس !(mch_stat(fname, &s) != 0 || s.st_uid != uid
# ifdef HAVE_LSTAT
	    || mch_lstat(fname, &s) != 0 || s.st_uid != uid
# endif
	    );
}
#اگر_ختم

/*
 * Give an error message main_errors["n"] and exit.
 */
    ساکن خالی
mainerr(
    صحیح		n,	// one of the ME_ defines
    char_u	*str)	// extra argument or NULL
{
#اگر defined(UNIX) || defined(VMS)
    reset_اشارات();		// kill us with CTRL-C here, if you like
#اگر_ختم

    // If this is a Windows GUI executable, show an error dialog box.
#اگر_تعریف_شدہ VIMDLL
    gui.اندر_use = mch_is_gui_executable();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
    gui.starting = false;   // Needed to show as error.
#اگر_ختم

    init_long_نسخہ();
    mch_errmsg(long_نسخہ);
    mch_errmsg("\n");
    mch_errmsg(_(main_errors[n]));
    اگر (str != تہی)
    {
	mch_errmsg(": \"");
	mch_errmsg((حرف *)str);
	mch_errmsg("\"");
    }
    mch_errmsg(_("\nMore info with: \"vim -h\"\n"));

    mch_exit(1);
}

    خالی
mainerr_arg_missing(char_u *str)
{
    mainerr(ME_ARG_MISSING, str);
}

#اگر_غیر_تعریف_شدہ NO_VIM_MAIN
/*
 * print a message with three spaces prepended and '\n' appended.
 */
    ساکن خالی
main_پیغام(حرف *s)
{
    mch_پیغام("   ");
    mch_پیغام(s);
    mch_پیغام("\n");
}

/*
 * Print messages for "vim -h" or "vim --help" and exit.
 */
    ساکن خالی
usage(خالی)
{
    صحیح		i;
    ساکن حرف	*(use[]) =
    {
	N_("[file ..]       edit specified file(s)"),
	N_("-               read text from stdin"),
	N_("-t tag          edit file where tag is defined"),
# ifdef FEAT_QUICKFIX
	N_("-q [errorfile]  edit file with first error")
# endif
    };

# اگر defined(UNIX) || defined(VMS)
    reset_اشارات();		// kill us with CTRL-C here, if you like
# endif

    init_long_نسخہ();
    mch_پیغام(long_نسخہ);
    mch_پیغام(_("\n\nUsage:"));
    برائے (i = 0; ; ++i)
    {
	mch_پیغام(_(" vim [arguments] "));
	mch_پیغام(_(use[i]));
	اگر (i == ARRAY_لمبائی(use) - 1)
	    توڑو;
	mch_پیغام(_("\n   or:"));
    }
# اگر defined( VMS)
    mch_پیغام(_("\nWhere command is down-cased, prepend / (like: -/R) to treat flag as upper-case."));
    mch_پیغام(_("\nOr, where supported, SET PROC/PARSE=EXT, or else quote upper-case material."));
# endif /* defined( VMS) */

    mch_پیغام(_("\n\nArguments:\n"));
    main_پیغام(_("--\t\t\tOnly file names after this"));
# ifdef EXPAND_FILENAMES
    main_پیغام(_("--literal\t\tDon't expand wildcards"));
# endif
# ifdef FEAT_OLE
    main_پیغام(_("-register\t\tRegister this gvim for OLE"));
    main_پیغام(_("-unregister\t\tUnregister gvim for OLE"));
# endif
# ifdef FEAT_GUI
    main_پیغام(_("-g\t\t\tRun using GUI (like \"gvim\")"));
    main_پیغام(_("-f  or  --nofork\tForeground: Don't fork when starting GUI"));
# endif
    main_پیغام(_("-v\t\t\tVi mode (like \"vi\")"));
    main_پیغام(_("-e\t\t\tEx mode (like \"ex\")"));
    main_پیغام(_("-E\t\t\tImproved Ex mode"));
    main_پیغام(_("-s\t\t\tSilent (batch) mode (only for \"ex\")"));
# ifdef FEAT_DIFF
    main_پیغام(_("-d\t\t\tDiff mode (like \"vimdiff\")"));
# endif
    main_پیغام(_("-y\t\t\tEasy mode (like \"evim\", modeless)"));
    main_پیغام(_("-R\t\t\tReadonly mode (like \"view\")"));
    main_پیغام(_("-Z\t\t\tRestricted mode (like \"rvim\")"));
    main_پیغام(_("-m\t\t\tModifications (writing files) not allowed"));
    main_پیغام(_("-M\t\t\tModifications in text not allowed"));
    main_پیغام(_("-b\t\t\tBinary mode"));
    main_پیغام(_("-l\t\t\tLisp mode"));
    main_پیغام(_("-C\t\t\tCompatible with Vi: 'compatible'"));
    main_پیغام(_("-N\t\t\tNot fully Vi compatible: 'nocompatible'"));
    main_پیغام(_("-V[N][fname]\t\tBe verbose [level N] [log messages to fname]"));
# ifdef FEAT_EVAL
    main_پیغام(_("-D\t\t\tDebugging mode"));
# endif
    main_پیغام(_("-n\t\t\tNo swap file, use memory only"));
    main_پیغام(_("-r\t\t\tList swap files and exit"));
    main_پیغام(_("-r (with file name)\tRecover crashed session"));
    main_پیغام(_("-L\t\t\tSame as -r"));
# ifdef AMIGA
    main_پیغام(_("-f\t\t\tDon't use newcli to open window"));
    main_پیغام(_("-dev <device>\t\tUse <device> for I/O"));
# endif
# ifdef FEAT_ARABIC
    main_پیغام(_("-A\t\t\tStart in Arabic mode"));
# endif
# ifdef FEAT_RIGHTLEFT
    main_پیغام(_("-H\t\t\tStart in Hebrew mode"));
# endif
    main_پیغام(_("-T <terminal>\tSet terminal type to <terminal>"));
    main_پیغام(_("--not-a-term\t\tSkip warning for input/output not being a terminal"));
# ifdef FEAT_GUI
    main_پیغام(_("--gui-dialog-file {fname}  For testing: write dialog text"));
# endif
    main_پیغام(_("--ttyfail\t\tExit if input or output is not a terminal"));
# ifdef FEAT_کام_CHANNEL
    main_پیغام(_("--stdio-channel\tUse stdin and stdout as a channel"));
# endif
    main_پیغام(_("-u <vimrc>\t\tUse <vimrc> instead of any .vimrc"));
# ifdef FEAT_GUI
    main_پیغام(_("-U <gvimrc>\t\tUse <gvimrc> instead of any .gvimrc"));
# endif
    main_پیغام(_("--noplugin\t\tDon't load plugin scripts"));
    main_پیغام(_("-p[N]\t\tOpen N tab pages (default: one for each file)"));
    main_پیغام(_("-o[N]\t\tOpen N windows (default: one for each file)"));
    main_پیغام(_("-O[N]\t\tLike -o but split vertically"));
    main_پیغام(_("+\t\t\tStart at end of file"));
    main_پیغام(_("+<lnum>\t\tStart at line <lnum>"));
    main_پیغام(_("--cmd <command>\tExecute <command> before loading any vimrc file"));
    main_پیغام(_("-c <command>\t\tExecute <command> after loading the first file"));
    main_پیغام(_("-S <session>\t\tSource file <session> after loading the first file"));
    main_پیغام(_("-s <scriptin>\tRead Normal mode commands from file <scriptin>"));
    main_پیغام(_("-w <scriptout>\tAppend all typed commands to file <scriptout>"));
    main_پیغام(_("-W <scriptout>\tWrite all typed commands to file <scriptout>"));
# ifdef FEAT_CRYPT
    main_پیغام(_("-x\t\t\tEdit encrypted files"));
# endif
# اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
#  اگر defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
    main_پیغام(_("-display <display>\tConnect Vim to this particular X-server"));
#  endif
    main_پیغام(_("-X\t\t\tDo not connect to X server"));
# endif
# اگر defined(FEAT_WAYLAND)
    main_پیغام(_("-Y\t\t\tDo not connect to Wayland compositor"));
# endif
# ifdef FEAT_CLIENTSERVER
#  ifdef FEAT_CLIENTSERVER_BACKENDS
    main_پیغام(_("--clientserver <socket|x11|mswin> Backend for clientserver communication"));
#  endif
    main_پیغام(_("--remote <files>\tEdit <files> in a Vim server if possible"));
    main_پیغام(_("--remote-silent <files>  Same, don't complain if there is no server"));
    main_پیغام(_("--remote-wait <files>  As --remote but wait for files to have been edited"));
    main_پیغام(_("--remote-wait-silent <files>  Same, don't complain if there is no server"));
    main_پیغام(_("--remote-tab[-wait][-silent] <files>  As --remote but use tab page per file"));
    main_پیغام(_("--remote-send <keys>\tSend <keys> to a Vim server and exit"));
    main_پیغام(_("--remote-expr <expr>\tEvaluate <expr> in a Vim server and print result"));
    main_پیغام(_("--serverlist\t\tList available Vim server names and exit"));
    main_پیغام(_("--servername <name>\tSend to/become the Vim server <name>"));
# endif
# ifdef STARTUPTIME
    main_پیغام(_("--startuptime <file>\tWrite startup timing messages to <file>"));
# endif
# ifdef FEAT_کام_CHANNEL
    main_پیغام(_("--log <file>\t\tStart logging to <file> early"));
# endif
# ifdef FEAT_VIMINFO
    main_پیغام(_("-i <viminfo>\t\tUse <viminfo> instead of .viminfo"));
# endif
    main_پیغام(_("--clean\t\t'nocompatible', Vim defaults, no plugins, no viminfo"));
    main_پیغام(_("-h  or  --help\tPrint Help (this message) and exit"));
    main_پیغام(_("--version\t\tPrint version information and exit"));

# ifdef FEAT_GUI_X11
#  ifdef FEAT_GUI_MOTIF
    mch_پیغام(_("\nArguments recognised by gvim (Motif version):\n"));
#  endif
    main_پیغام(_("-display <display>\tRun Vim on <display>"));
    main_پیغام(_("-iconic\t\tStart Vim iconified"));
    main_پیغام(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_پیغام(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_پیغام(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_پیغام(_("-boldfont <font>\tUse <font> for bold text"));
    main_پیغام(_("-italicfont <font>\tUse <font> for italic text"));
    main_پیغام(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
    main_پیغام(_("-borderwidth <width>\tUse a border width of <width> (also: -bw)"));
    main_پیغام(_("-scrollbarwidth <width>  Use a scrollbar width of <width> (also: -sw)"));
    main_پیغام(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_پیغام(_("+reverse\t\tDon't use reverse video (also: +rv)"));
    main_پیغام(_("-xrm <resource>\tSet the specified resource"));
# endif // FEAT_GUI_X11
# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
    mch_پیغام(_("\nArguments recognised by gvim (GTK4 version):\n"));
#  ورنہ
    mch_پیغام(_("\nArguments recognised by gvim (GTK+ version):\n"));
#  endif
    main_پیغام(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_پیغام(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_پیغام(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_پیغام(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
#  ifdef USE_GTK4
    main_پیغام("--prg-name <name>\tSet GTK program name");
#  ورنہ
    main_پیغام(_("-iconic\t\tStart Vim iconified"));
    main_پیغام(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_پیغام(_("-display <display>\tRun Vim on <display> (also: --display)"));
    main_پیغام(_("--role <role>\tSet a unique role to identify the main window"));
    main_پیغام(_("--socketid <xid>\tOpen Vim inside another GTK widget"));
    main_پیغام(_("--echo-wid\t\tMake gvim echo the Window ID on stdout"));
#  endif
# endif
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
    اگر (gui.starting)
#  endif
    {
	main_پیغام(_("-P <parent title>\tOpen Vim inside parent application"));
	main_پیغام(_("--windowid <HWND>\tOpen Vim inside another win32 widget"));
    }
# endif

# ifdef FEAT_GUI_GNOME
    // Gnome gives extra messages for --help if we continue, but not for -h.
    اگر (gui.starting)
    {
	mch_پیغام("\n");
	gui.dofork = false;
    }
    ورنہ
# endif
	mch_exit(0);
}

/*
 * Check the result of the ATTENTION dialog:
 * When "Quit" selected, exit Vim.
 * When "Recover" selected, recover the file.
 */
    ساکن خالی
check_تبادلہ_exists_action(خالی)
{
    اگر (تبادلہ_exists_action == SEA_QUIT)
	getout(1);
    handle_تبادلہ_exists(تہی);
}

#اگر_ختم // NO_VIM_MAIN

#اگر !defined(NO_VIM_MAIN) && defined(FEAT_EVAL)
    ساکن خالی
سیٹ_progpath(char_u *argv0)
{
    char_u *val = argv0;

# ifdef MSWIN
    // A relative path containing a "/" will become invalid when using ":cd",
    // turn it into a full path.
    // On MS-Windows "vim" should be expanded to "vim.exe", thus always do
    // this.
    char_u *راستہ = تہی;

    اگر (mch_can_exe(argv0, &راستہ, FALSE) && راستہ != تہی)
	val = راستہ;
# ورنہ
    char_u	buf[MAXPATHL + 1];
#  ifdef PROC_EXE_LINK
    حرف	linkbuf[MAXPATHL + 1];
    ssize_t	len;

    len = readlink(PROC_EXE_LINK, linkbuf, MAXPATHL);
    اگر (len > 0)
    {
	linkbuf[len] = NUL;
	val = (char_u *)linkbuf;
    }
#  endif

    اگر (!mch_is_بھرا_Name(val))
    {
	اگر (gettail(val) != val
			   && vim_بھرا_Name(val, buf, MAXPATHL, TRUE) != FAIL)
	    val = buf;
    }
# endif

    سیٹ_vim_var_string(VV_PROGPATH, val, -1);

# ifdef MSWIN
    vim_free(راستہ);
# endif
}

#اگر_ختم // NO_VIM_MAIN
