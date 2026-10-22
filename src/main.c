/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#define EXTERN
#شامل "vim.h"

#ifdef __CYGWIN__
# include <cygwin/version.h>
# include <sys/cygwin.h>	// for cygwin_conv_to_posix_path() and/or
				// cygwin_conv_path()
# include <limits.h>
#endif

#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# include "iscygpty.h"
#endif

// Values for edit_type.
#define EDIT_NONE   0	    // no edit type yet
#define EDIT_FILE   1	    // file name argument[s] given, use argument list
#define EDIT_STDIN  2	    // read file from stdin
#define EDIT_TAG    3	    // tag name argument given, use tagname
#define EDIT_QF	    4	    // start in quickfix mode

#اگر (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
ایستا صحیح file_owned(نویسه *fname);
#endif
ایستا تهی mainerr(صحیح, char_u *);
ایستا تهی early_arg_scan(mparm_T *parmp);
#ifndef NO_VIM_MAIN
ایستا تهی usage(تهی);
ایستا تهی parse_command_name(mparm_T *parmp);
ایستا تهی command_سطر_scan(mparm_T *parmp);
ایستا تهی check_tty(mparm_T *parmp);
ایستا تهی read_stdin(تهی);
ایستا تهی create_windows(mparm_T *parmp);
ایستا تهی edit_buffers(mparm_T *parmp, char_u *cwd);
ایستا تهی exe_pre_commands(mparm_T *parmp);
ایستا تهی exe_commands(mparm_T *parmp);
ایستا تهی source_startup_scripts(mparm_T *parmp);
ایستا تهی main_start_gui(تهی);
ایستا تهی check_swap_exists_action(تهی);
# ifdef FEAT_EVAL
ایستا تهی set_progpath(char_u *argv0);
# endif
#endif


/*
 * Different types of error messages.
 */
ایستا نویسه *(main_errors[]) =
{
    N_("Unknown option argument"),
#define ME_UNKNOWN_OPTION	0
    N_("Too many edit arguments"),
#define ME_TOO_MANY_ARGS	1
    N_("Argument missing after"),
#define ME_ARG_MISSING		2
    N_("Garbage after option argument"),
#define ME_GARBAGE		3
    N_("Too many \"+command\", \"-c command\" or \"--cmd command\" arguments"),
#define ME_EXTRA_CMD		4
    N_("Invalid argument for"),
#define ME_INVALID_ARG		5
};

#ifndef PROTO		// don't want a prototype for main()

// Various parameters passed between main() and other functions.
ایستا mparm_T	params;

# ifdef _IOLBF
ایستا تهی *s_vbuf = پوچ;		// buffer for setvbuf()
# endif

# ifndef NO_VIM_MAIN	// skip this for unittests

ایستا char_u *start_dir = پوچ;	// current working dir on startup

ایستا صحیح has_dash_c_arg = FALSE;

#  ifdef VIMDLL
__declspec(dllexport)
#  endif
    صحیح
#  ifdef MSWIN
VimMain
#  وگرنه
اصلی
#  endif
(صحیح argc, نویسه **argv)
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
    argc = get_cmd_argsW(&argv);
#  endif

    // Many variables are in "params" so that we can pass them to invoked
    // functions without a lot of arguments.  "argc" and "argv" are also
    // copied, so that they can be changed.
    CLEAR_FIELD(params);
    params.argc = argc;
    params.argv = argv;
    params.want_full_screen = TRUE;
#  ifdef FEAT_EVAL
    params.use_debug_break_level = -1;
#  endif
    params.window_شمار = -1;

    autocmd_init();

#  ifdef FEAT_RUBY
    {
	صحیح ruby_پشته_start;
	vim_ruby_init((تهی *)&ruby_پشته_start);
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

#  اگر defined(STARTUPTIME) || defined(FEAT_JOB_CHANNEL)
    // Need to find "--startuptime" and "--log" before actually parsing
    // arguments.
    برای (i = 1; i < argc - 1; ++i)
    {
#   ifdef STARTUPTIME
	اگر (STRICMP(argv[i], "--startuptime") == 0 && time_fd == پوچ)
	{
	    time_fd = mch_fopen(argv[i + 1], "a");
	    TIME_پیام("--- VIM STARTING ---");
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
    برای (i = 1; i < argc; ++i)
	اگر (STRICMP(argv[i], "--clean") == 0)
	{
	    params.clean = TRUE;
	    بشکن;
	}
#  endif
#  ifdef MSWIN
    // Need to find "-register" and "-unregister" before loading any libraries.
    برای (i = 1; i < argc; ++i)
	اگر ((STRICMP(argv[i] + 1, "register") == 0
				    || STRICMP(argv[i] + 1, "unregister") == 0)
		&& (argv[i][0] == '-' || argv[i][0] == '/'))
	{
	    found_register_arg = TRUE;
	    بشکن;
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
    command_سطر_scan(&params);
    TIME_پیام("parsing arguments");

    /*
     * On some systems, when we compile with the GUI, we always use it.  On Mac
     * there is no terminal version, and on Windows we can't fork one off with
     * :gui.
     */
#  ifdef ALWAYS_USE_GUI
    gui.starting = true;
#  وگرنه
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
	    اگر (params.evim_mode)
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
	    start_dir = alloc(MAXPATHL);
	    اگر (start_dir != پوچ)
		mch_dirname(start_dir, MAXPATHL);
	    // Temporarily add '(' and ')' to 'isfname'.  These are valid
	    // filename characters but are excluded from 'isfname' to make
	    // "gf" work on a file name in parentheses (e.g.: see vim.h).
	    do_cmdline_cmd((char_u *)":set isf+=(,)");
	    alist_expand(پوچ, 0);
	    do_cmdline_cmd((char_u *)":set isf&");
	    اگر (start_dir != پوچ)
		mch_chdir((نویسه *)start_dir);
	}
#  endif
	params.fname = alist_name(&GARGLIST[0]);
    }

#  ifdef MSWIN
    {
	// Remember the number of entries in the argument list.  If it changes
	// we don't react on setting 'encoding'.
	set_alist_شمار();
    }
#  endif

#  ifdef MSWIN
    اگر (GARGCOUNT == 1 && params.full_path)
    {
	/*
	 * If there is one filename, fully qualified, we have very probably
	 * been invoked from explorer, so change to the file's directory.
	 * Hint: to avoid this when typing a command use a forward slash.
	 * If the cd fails, it doesn't matter.
	 */
	اگر (vim_chdirfile(params.fname, "drop") == OK)
	    last_chdir_reason = "drop";
	اگر (start_dir != پوچ)
	    mch_dirname(start_dir, MAXPATHL);
    }
#  endif
    TIME_پیام("expanding arguments");

#  ifdef FEAT_DIFF
    اگر (params.diff_mode && params.window_شمار == -1)
	params.window_شمار = 0;	// open up to 3 windows
#  endif

    // Don't redraw until much later.
    ++RedrawingDisabled;

    /*
     * When listing swap file names, don't do cursor positioning et. al.
     */
    اگر (recoverymode && params.fname == پوچ)
	params.want_full_screen = FALSE;

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
	params.want_full_screen = FALSE;
#  endif

    /*
     * mch_init() sets up the terminal (window) for use.  This must be
     * done after resetting full_screen, otherwise it may move the cursor.
     * Note that we may use mch_exit() before mch_init()!
     */
    mch_init();
    TIME_پیام("shell init");

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
	نویسه *p = getenv("SESSION_MANAGER");

	اگر (p != پوچ && *p != NUL)
	{
	    xsmp_init();
	    TIME_پیام("xsmp init");
	}
    }
#  endif

    /*
     * Print a warning if stdout is not a terminal.
     */
    check_tty(&params);

#  ifdef _IOLBF
    اگر (silent_mode)
    {
	// Ensure output works usefully without a tty: buffer lines instead of
	// fully buffered.
	s_vbuf = malloc(BUFSIZ);
	اگر (s_vbuf != پوچ)
	    setvbuf(stdout, s_vbuf, _IOLBF, BUFSIZ);
    }
#  endif

    // This message comes before term inits, but after setting "silent_mode"
    // when the input is not a tty. Omit the message with --not-a-term.
    اگر (GARGCOUNT > 1 && !silent_mode && !is_not_a_term())
	printf(_("%d files to edit\n"), GARGCOUNT);

    اگر (params.want_full_screen && !silent_mode)
    {
	termcapinit(params.term);	// set terminal name and get terminal
				   // capabilities (will set full_screen)
	screen_start();		// don't know where cursor is now
	TIME_پیام("Termcap init");
    }

    /*
     * Set the default values for the options that use Rows and Columns.
     */
    ui_get_shellsize();		// inits Rows and Columns
    win_init_size();
#  ifdef FEAT_DIFF
    // Set the 'diff' option now, so that it can be checked for in a .vimrc
    // file.  There is no buffer yet though.
    اگر (params.diff_mode)
	diff_win_options(firstwin, FALSE);
#  endif

    cmdline_row = Rows - p_ch;
    cmdline_col_off = 0;
    cmdline_width = (صحیح)Columns;
    پیام_row = cmdline_row;
    پیام_col = 0;
    screenalloc(FALSE);		// allocate screen buffers
    set_init_2();
    TIME_پیام("inits 2");

    پیام_scroll = TRUE;
    no_wait_return = TRUE;

    init_mappings();		// set up initial mappings

    init_highlight(TRUE, FALSE); // set the default highlight groups
    TIME_پیام("init highlight");

#  اگر defined(FEAT_TERMRESPONSE)
    init_term_props(TRUE);
#  endif

#  ifdef FEAT_EVAL
    // Set the break level after the terminal is initialized.
    debug_break_level = params.use_debug_break_level;
#  endif

    // Reset 'loadplugins' for "-u NONE" before "--cmd" arguments.
    // Allows for setting 'loadplugins' there.
    اگر (params.use_vimrc != پوچ
	    && (STRCMP(params.use_vimrc, "NONE") == 0
		|| STRCMP(params.use_vimrc, "DEFAULTS") == 0))
	p_lpl = FALSE;

    // Execute --cmd arguments.
    exe_pre_commands(&params);

    // Source startup scripts.
    source_startup_scripts(&params);

#  ifdef FEAT_EVAL
    set_vim_var_nr(VV_VIM_DID_INIT, 1L);
#  endif

#  ifdef FEAT_MZSCHEME
    /*
     * Newer version of MzScheme (Racket) require earlier (trampolined)
     * initialisation via scheme_main_setup.
     * Implement this by initialising it as early as possible
     * and splitting off remaining Vim main into vim_main2().
     * Do source startup scripts, so that 'mzschemedll' can be set.
     */
    بازگشت mzscheme_main();
#  وگرنه
    بازگشت vim_main2();
#  endif
}
# endif // NO_VIM_MAIN
#endif // PROTO

#اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(FEAT_CLIPBOARD)
/*
 * Restore the state after a fatal X error.
 */
    ایستا تهی
x_restore_state(تهی)
{
    State = MODE_NORMAL;
    VIsual_active = FALSE;
    got_int = TRUE;
    need_wait_return = FALSE;
    global_busy = FALSE;
    exmode_active = 0;
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
    scroll_start();
    redraw_later_clear();
    choose_clipmethod();
}
#endif

/*
 * vim_main2() is needed for FEAT_MZSCHEME, but we define it always to keep
 * things simple.
 * It is also defined when NO_VIM_MAIN is defined, but then it's empty.
 */
    صحیح
vim_main2(تهی)
{
#ifndef NO_VIM_MAIN
# ifdef FEAT_EVAL
    /*
     * Read all the plugin files.
     * Only when compiled with +eval, since most plugins need it.
     */
    اگر (p_lpl)
    {
	char_u *rtp_copy = پوچ;
	char_u *plugin_pattern = (char_u *)
#  اگر defined(VMS) || defined(AMIGA) // VMS and Amiga don't handle the "**".
		"plugin/*.vim"
#  وگرنه
		"plugin/**/*.vim"
#  endif
		;

	// First add all package directories to 'runtimepath', so that their
	// autoload directories can be found.  Only if not done already with a
	// :packloadall command.
	// Make a copy of 'runtimepath', so that source_runtime does not use
	// the pack directories.
	اگر (!did_source_packages)
	{
	    rtp_copy = vim_strsave(p_rtp);
	    add_pack_start_dirs();
	}

	source_in_path(rtp_copy == پوچ ? p_rtp : rtp_copy, plugin_pattern,
		DIP_ALL | DIP_NOAFTER, پوچ);
	TIME_پیام("loading plugins");
	vim_free(rtp_copy);

	// Only source "start" packages if not done already with a :packloadall
	// command.
	اگر (!did_source_packages)
	    load_start_packages();
	TIME_پیام("loading packages");

	source_runtime(plugin_pattern, DIP_ALL | DIP_AFTER);
	TIME_پیام("loading after plugins");
    }
# endif

# ifdef FEAT_DIFF
    // Decide about window layout for diff mode after reading vimrc.
    اگر (params.diff_mode && params.window_layout == 0)
    {
	اگر (diffopt_horizontal())
	    params.window_layout = WIN_HOR;	// use horizontal split
	وگرنه
	    params.window_layout = WIN_VER;	// use vertical split
    }
# endif

    /*
     * Recovery mode without a file name: List swap files.
     * This uses the 'dir' option, therefore it must be after the
     * initializations.
     */
    اگر (recoverymode && params.fname == پوچ)
    {
	recover_names(پوچ, TRUE, پوچ, 0, پوچ);
	mch_exit(0);
    }

    /*
     * Set a few option defaults after reading .vimrc files:
     * 'title' and 'icon', Unix: 'shellpipe' and 'shellredir'.
     */
    set_init_3();
    TIME_پیام("inits 3");

    /*
     * "-n" argument: Disable swap file by setting 'updatecount' to 0.
     * Note that this overrides anything from a vimrc file.
     */
    اگر (params.no_swap_file)
	p_uc = 0;

# ifdef FEAT_GUI
    اگر (gui.starting)
    {
#  اگر defined(UNIX) || defined(VMS)
	// When something caused a message from a vimrc script, need to output
	// an extra newline before the shell prompt.
	اگر (did_emsg || پیام_didout)
	    putchar('\n');
#  endif

	gui_start(پوچ);		// will set full_screen to TRUE
	TIME_پیام("starting GUI");

	// When running "evim" or "gvim -y" we need the menus, exit if we
	// don't have them.
	اگر (!gui.in_use && params.evim_mode)
	    mch_exit(1);
	firstwin->w_prev_height = firstwin->w_height; // may have changed
    }
# endif

# ifdef FEAT_VIMINFO
    /*
     * Read in registers, history etc, but not marks, from the viminfo file.
     * This is where v:oldfiles gets filled.
     */
    اگر (*p_viminfo != NUL)
    {
	read_viminfo(پوچ, VIF_WANT_INFO | VIF_GET_OLDFILES);
	TIME_پیام("reading viminfo");
    }
# endif
# ifdef FEAT_EVAL
    // It's better to make v:oldfiles an empty list than NULL.
    اگر (get_vim_var_list(VV_OLDFILES) == پوچ)
	set_vim_var_list(VV_OLDFILES, list_alloc());
# endif

# ifdef FEAT_QUICKFIX
    /*
     * "-q errorfile": Load the error file now.
     * If the error file can't be read, exit before doing anything else.
     */
    اگر (params.edit_type == EDIT_QF)
    {
	char_u	*enc = پوچ;

	enc = p_menc;
	اگر (params.use_ef != پوچ)
	    set_string_option_direct((char_u *)"ef", -1,
					   params.use_ef, OPT_FREE, SID_CARG);
	vim_snprintf((نویسه *)IObuff, IOSIZE, "cfile %s", p_ef);
	اگر (qf_init(پوچ, p_ef, p_efm, TRUE, IObuff, enc) < 0)
	{
	    out_char('\n');
	    mch_exit(3);
	}
	TIME_پیام("reading errorfile");
    }
# endif

    /*
     * Start putting things on the screen.
     * Scroll screen down before drawing over it
     * Clear screen now, so file message will not be cleared.
     */
    starting = NO_BUFFERS;
    no_wait_return = FALSE;
    اگر (!exmode_active)
	پیام_scroll = FALSE;

# ifdef FEAT_GUI
    /*
     * This seems to be required to make callbacks to be called now, instead
     * of after things have been put on the screen, which then may be deleted
     * when getting a resize callback.
     * For the Mac this handles putting files dropped on the Vim icon to
     * global_alist.
     */
    اگر (gui.in_use)
    {
	gui_wait_for_chars(50L, typebuf.tb_change_cnt);
	TIME_پیام("GUI delay");
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
    اگر (!gui.in_use)
#  endif
    {
	setup_term_clip();
	TIME_پیام("setup x11 clipboard");
    }
# endif

# ifdef FEAT_CLIENTSERVER
    // Prepare for being a Vim server.
    prepare_server(&params);
# endif

# ifdef FEAT_WAYLAND
#  ifdef FEAT_GUI
    اگر (!gui.in_use)
#  endif
    {
	اگر (wayland_init_connection(wayland_display_name) == OK)
	{
	    TIME_پیام("connected to Wayland display");

#  ifdef FEAT_WAYLAND_CLIPBOARD
	    اگر (clip_init_wayland() == OK)
		TIME_پیام("setup Wayland clipboard");
#  endif
	}
    }
# endif

# ifdef HAVE_CLIPMETHOD
    choose_clipmethod();
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
    اگر ((did_emsg || پیام_didout) && *T_TI != NUL && params.edit_type != EDIT_STDIN)
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
    TIME_پیام("setting raw mode");

    اگر (need_wait_return || پیام_didany)
    {
	wait_return(TRUE);
	TIME_پیام("waiting for return");
    }

    starttermcap();	    // start termcap if not done by wait_return()
    TIME_پیام("start termcap");

    setmouse();				// may start using the mouse
    اگر (scroll_region)
	scroll_region_reset();		// In case Rows changed
    scroll_start();	// may scroll the screen to the right position

# اگر defined(UNIX) || defined(VMS) || defined(MACOS_X)
    term_افزودن_title(SAVE_RESTORE_BOTH);
# endif

    /*
     * Don't clear the screen when starting in Ex mode, unless using the GUI.
     */
    اگر (exmode_active
# ifdef FEAT_GUI
			&& !gui.in_use
# endif
					)
	set_must_redraw(UPD_CLEAR);
    وگرنه
    {
	screenclear();			// clear screen
	TIME_پیام("clearing screen");
    }

# ifdef FEAT_CRYPT
    اگر (params.ask_for_کلید)
    {
	crypt_check_current_method();
	(تهی)crypt_get_کلید(TRUE, TRUE);
	TIME_پیام("getting crypt key");
    }
# endif

    no_wait_return = TRUE;

    /*
     * Create the requested number of windows and edit buffers in them.
     * Also does recovery if "recoverymode" set.
     */
    create_windows(&params);
    TIME_پیام("opening buffers");

# ifdef FEAT_EVAL
    // clear v:swapcommand
    set_vim_var_string(VV_SWAPCOMMAND, پوچ, -1);
# endif

    // Ex starts at last line of the file
    اگر (exmode_active)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_شمار;

    apply_autocmds(EVENT_BUFENTER, پوچ, پوچ, FALSE, curbuf);
    TIME_پیام("BufEnter autocommands");
    setpcmark();

# ifdef FEAT_QUICKFIX
    /*
     * When started with "-q errorfile" jump to first error now.
     */
    اگر (params.edit_type == EDIT_QF)
    {
	qf_jump(پوچ, 0, 0, FALSE);
	TIME_پیام("jump to first error");
    }
# endif

    /*
     * If opened more than one window, start editing files in the other
     * windows.
     */
    edit_buffers(&params, start_dir);
    vim_free(start_dir);

# ifdef FEAT_DIFF
    اگر (params.diff_mode)
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
    اگر (params.tagname != پوچ)
    {
	swap_exists_did_quit = FALSE;

	vim_snprintf((نویسه *)IObuff, IOSIZE, "ta %s", params.tagname);
	do_cmdline_cmd(IObuff);
	TIME_پیام("jumping to tag");

	// If the user doesn't want to edit the file then we quit here.
	اگر (swap_exists_did_quit)
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
    صحیح jump_result = SETJMP(x_jump_env);

    اگر (jump_result == 0)
    {
# endif
	// Execute any "+", "-c" and "-S" arguments.
	اگر (params.n_commands > 0)
	    exe_commands(&params);
# اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    }
    وگرنه
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
    update_cell_size();
# endif

    // start in insert mode
    اگر (p_im)
	need_start_insertmode = TRUE;

# ifdef FEAT_EVAL
    set_vim_var_nr(VV_VIM_DID_ENTER, 1L);
# endif
    apply_autocmds(EVENT_VIMENTER, پوچ, پوچ, FALSE, curbuf);
    TIME_پیام("VimEnter autocommands");

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
	TIME_پیام("diff scrollbinding");
    }
# endif

# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    اگر (!gui.in_use)
#  endif
	mch_set_winsize_now();	    // Allow winsize changes from now on
# endif

# اگر defined(FEAT_GUI)
    // When tab pages were created, may need to update the tab pages line and
    // scrollbars.  This is skipped while creating them.
    اگر (gui.in_use && first_tabpage->tp_next != پوچ)
    {
	out_flush();
	gui_init_which_components(پوچ);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
# endif

    // If ":startinsert" command used, stuff a dummy command to be able to
    // call normal_cmd(), which will then start Insert mode.
    اگر (restart_edit != 0)
	stuffcharReadbuff(K_NOP);

# ifdef FEAT_NETBEANS_INTG
    اگر (netbeansArg != پوچ && strncmp("-nb", netbeansArg, 3) == 0)
    {
#  ifdef FEAT_GUI
#   اگر !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
	اگر (gui.in_use)
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

    TIME_پیام("before starting main loop");

# ifdef FEAT_JOB_CHANNEL
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

#endif // NO_VIM_MAIN

    بازگشت 0;
}

/*
 * Initialisation #1 shared by main() and some tests.
 */
    تهی
common_init_1(تهی)
{
    estack_init();
    cmdline_init();

    (تهی)mb_init();	// init mb_bytelen_tab[] to ones
#ifdef FEAT_EVAL
    eval_init();	// init global variables
#endif

#ifdef __QNXNTO__
    qnx_init();		// PhAttach() for clipboard, (and gui)
#endif

    /*
     * Allocate space for the generic buffers (needed for set_init_1() and
     * emsg()).
     */
    اگر ((IObuff = alloc(IOSIZE)) == پوچ
	    || (NameBuff = alloc(MAXPATHL)) == پوچ)
	mch_exit(0);
    TIME_پیام("Allocated generic buffers");
}


/*
 * Initialisation #2 shared by main() and some tests.
 */
    تهی
common_init_2(mparm_T *paramp)
{
#ifdef NBDEBUG
    // Wait a moment for debugging NetBeans.  Must be after allocating
    // NameBuff.
    nbdebug_log_init("SPRO_GVIM_DEBUG", "SPRO_GVIM_DLEVEL");
    nbdebug_wait(WT_ENV | WT_WAIT | WT_STOP, "SPRO_GVIM_WAIT", 20);
    TIME_پیام("NetBeans debug wait");
#endif

#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    /*
     * Setup to use the current locale (for ctype() and many other things).
     * NOTE: Translated messages with encodings other than latin1 will not
     * work until set_init_1() has been called!
     */
    init_locale();
    TIME_پیام("locale set");
#endif

#ifdef FEAT_GUI
    gui.dofork = true;		    // default is to use fork()
#endif

#ifdef FEAT_CLIENTSERVER_BACKENDS
    /*
     * Check the $VIM_CLIENTSERVER env before we handle the --clientserver arg
     */
    check_clientserver_method_env();
#endif

    /*
     * Do a first scan of the arguments in "argv[]":
     *   -display or --display
     *   --server...
     *   --socketid
     *   --windowid
     */
    early_arg_scan(paramp);

#اگر defined(FEAT_GUI)
    // Prepare for possibly starting GUI sometime
    gui_prepare(&paramp->argc, paramp->argv);
    TIME_پیام("GUI prepared");
#endif

#ifdef FEAT_CLIPBOARD
    clip_init(FALSE);		// Initialise clipboard stuff
    TIME_پیام("clipboard setup");
#endif

    /*
     * Check if we have an interactive window.
     * On the Amiga: If there is no window, we open one with a newcli command
     * (needed for :! to * work). mch_check_win() will also handle the -d or
     * -dev argument.
     */
    stdout_isatty = (mch_check_win(paramp->argc, paramp->argv) != FAIL);
    TIME_پیام("window checked");

    /*
     * Allocate the first window and buffer.
     * Can't do anything without it, exit when it fails.
     */
    اگر (win_alloc_first() == FAIL)
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
    set_init_1(paramp->clean);
    TIME_پیام("inits 1");

#ifdef FEAT_EVAL
    // set v:lang and v:ctype
    set_lang_var();

    // set v:argv
    set_argv_var(paramp->argv, paramp->argc);
#endif

#ifdef FEAT_SIGNS
    init_signs();
#endif

#ifdef FEAT_QUICKFIX
    // initialize quickfix list
    // don't send an error message when memory allocation fails
    // do it when the user tries to access the quickfix list
    qf_init_پشته();
#endif
}

/*
 * Return TRUE when the --not-a-term argument was found.
 */
    صحیح
is_not_a_term(تهی)
{
    بازگشت params.not_a_term;
}

/*
 * Return TRUE when the --not-a-term argument was found or the GUI is in use.
 */
    صحیح
is_not_a_term_or_gui(تهی)
{
    بازگشت params.not_a_term
#ifdef FEAT_GUI
			    || gui.in_use
#endif
	;
}

#اگر defined(EXITFREE)
    تهی
free_vbuf(تهی)
{
# ifdef _IOLBF
    اگر (s_vbuf != پوچ)
    {
	setvbuf(stdout, پوچ, _IONBF, 0);
	آزاد(s_vbuf);
	s_vbuf = پوچ;
    }
# endif
}
#endif

#اگر defined(FEAT_GUI)
/*
 * If a --gui-dialog-file argument was given return the file name.
 * Otherwise return NULL.
 */
    char_u *
get_gui_dialog_file(تهی)
{
    بازگشت params.gui_dialog_file;
}
#endif

// When TRUE in a safe state when starting to wait for a character.
ایستا صحیح	was_safe = FALSE;
ایستا oparg_T	*current_oap = پوچ;

/*
 * Return TRUE if an operator was started but not finished yet.
 * Includes typing a count or a register name.
 */
    صحیح
op_pending(تهی)
{
    بازگشت !(current_oap != پوچ
	    && !finish_op
	    && current_oap->prev_opcount == 0
	    && current_oap->prev_count0 == 0
	    && current_oap->op_type == OP_NOP
	    && current_oap->regname == NUL);
}

/*
 * Return whether currently it is safe, assuming it was safe before (high level
 * state didn't change).
 */
    ایستا صحیح
is_safe_now(تهی)
{
    بازگشت stuff_empty()
	&& typebuf.tb_len == 0
	&& scriptin[curscript] == پوچ
#ifdef FEAT_EVAL
	&& !debug_mode
#endif
	&& !global_busy;
}

/*
 * Trigger SafeState if currently in a safe state, that is "safe" is TRUE and
 * there is no typeahead.
 */
    تهی
may_trigger_safestate(صحیح safe)
{
    صحیح is_safe = safe && is_safe_now();

#ifdef FEAT_EVAL
    اگر (was_safe != is_safe)
	// Only log when the state changes, otherwise it happens at nearly
	// every key stroke.
	ch_log(پوچ, is_safe ? "SafeState: Start triggering"
					       : "SafeState: Stop triggering");
#endif
    اگر (is_safe)
	apply_autocmds(EVENT_SAFESTATE, پوچ, پوچ, FALSE, curbuf);
    was_safe = is_safe;
}

/*
 * Something changed which causes the state possibly to be unsafe, e.g. a
 * character was typed.  It will remain unsafe until the next call to
 * may_trigger_safestate().
 */
    تهی
state_no_longer_safe(نویسه *reason UNUSED)
{
#ifdef FEAT_EVAL
    اگر (was_safe)
	ch_log(پوچ, "SafeState: reset: %s", reason);
#endif
    was_safe = FALSE;
}

#اگر defined(FEAT_EVAL) || defined(MESSAGE_QUEUE)
    صحیح
get_was_safe_state(تهی)
{
    بازگشت was_safe;
}
#endif

#اگر defined(MESSAGE_QUEUE)
/*
 * Invoked when leaving code that invokes callbacks.  Then trigger
 * SafeStateAgain, if it was safe when starting to wait for a character.
 */
    تهی
may_trigger_safestateagain(تهی)
{
    اگر (!was_safe)
    {
	// If the safe state was reset in state_no_longer_safe(), e.g. because
	// of calling feedkeys(), we check if it's now safe again (all keys
	// were consumed).
	was_safe = is_safe_now();
# ifdef FEAT_EVAL
	اگر (was_safe)
	    ch_log(پوچ, "SafeState: undo reset");
# endif
    }
    اگر (was_safe)
    {
# ifdef FEAT_EVAL
	// Only do this message when another message was given, otherwise we
	// get lots of them.
	اگر ((did_repeated_پیام & REPEATED_پیام_SAFESTATE) == 0)
	{
	    صحیح did = did_repeated_پیام;

	    ch_log(پوچ,
		      "SafeState: back to waiting, triggering SafeStateAgain");
	    did_repeated_پیام = did | REPEATED_پیام_SAFESTATE;
	}
# endif
	apply_autocmds(EVENT_SAFESTATEAGAIN, پوچ, پوچ, FALSE, curbuf);
    }
# ifdef FEAT_EVAL
    وگرنه
	ch_log(پوچ,
		  "SafeState: back to waiting, not triggering SafeStateAgain");
# endif
}
#endif

/*
 * Return TRUE if there is any typeahead, pending operator or command.
 */
    صحیح
work_pending(تهی)
{
    بازگشت op_pending() || !is_safe_now();
}

#ifdef FEAT_CONCEAL
ایستا linenr_T	conceal_old_cursor_سطر = 0;
ایستا linenr_T	conceal_new_cursor_سطر = 0;
ایستا صحیح	conceal_update_سطرها = FALSE;
#endif

/*
 * Trigger the events that are not triggered where they happen, but by
 * comparing the current state against the state stored when they were last
 * triggered. Also updates what depends on the cursor having moved, which is
 * not affected by 'eventignore'.
 * Called from the main loop and when 'eventignore(win)' changes, so that what
 * happened with an event ignored is not reported once it is not ignored
 * anymore.
 */
    تهی
may_trigger_deferred_events(تهی)
{
    ایستا bool	recursive = false;

    اگر (recursive)
	بازگشت;
    recursive = true;

#ifdef FEAT_CONCEAL
    اگر (curwin->w_p_cole == 0)
	conceal_update_سطرها = FALSE;
#endif

    // Trigger CursorMoved if the cursor moved.
    اگر (!finish_op && (has_cursormoved()
#ifdef FEAT_PROP_POPUP
		|| popup_visible
#endif
#ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
#endif
		) && !EQUAL_POS(last_cursormoved, curwin->w_cursor))
    {
	اگر (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, پوچ, پوچ, FALSE, curbuf);
#ifdef FEAT_PROP_POPUP
	اگر (popup_visible)
	    popup_check_cursor_pos();
#endif
#ifdef FEAT_CONCEAL
	اگر (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_سطر = last_cursormoved.lnum;
	    conceal_new_cursor_سطر = curwin->w_cursor.lnum;
	    conceal_update_سطرها = TRUE;
	}
#endif
	last_cursormoved = curwin->w_cursor;
    }

#ifdef FEAT_CONCEAL
    اگر (conceal_update_سطرها
	    && (conceal_old_cursor_سطر != conceal_new_cursor_سطر
		|| conceal_cursor_سطر(curwin)
		|| need_cursor_سطر_redraw))
    {
	اگر (conceal_old_cursor_سطر != conceal_new_cursor_سطر
		&& conceal_old_cursor_سطر != 0
		&& conceal_old_cursor_سطر <= curbuf->b_ml.ml_سطر_شمار)
	    redrawWinline(curwin, conceal_old_cursor_سطر);
	redrawWinline(curwin, conceal_new_cursor_سطر);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_سطر_redraw = FALSE;
    }
#endif

    // Trigger TextChanged if b:changedtick differs.
    اگر (!finish_op && has_textchanged()
	    && curbuf->b_last_changedtick != CHANGEDTICK(curbuf))
    {
	apply_autocmds(EVENT_TEXTCHANGED, پوچ, پوچ, FALSE, curbuf);
	curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    }

    // Ensure curwin->w_topline and curwin->w_leftcol are up to date before
    // triggering a WinScrolled autocommand.
    update_topline();
    validate_cursor();

    اگر (!finish_op)
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
    تهی
main_loop(
    صحیح		cmdwin,	    // TRUE when working in the command-line window
    صحیح		noexmode)   // TRUE when return on entering Ex mode
{
    oparg_T	oa;		// operator arguments
    oparg_T	*prev_oap;	// operator arguments
    فرار صحیح previous_got_int = FALSE;	// "got_int" was TRUE

    prev_oap = current_oap;
    current_oap = &oa;

#اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    // Setup to catch a terminating error from the X server.  Just ignore
    // it, restore the state and continue.  This might not always work
    // properly, but at least we hopefully don't exit unexpectedly when the X
    // server exits while Vim is running in a console.
    اگر (!cmdwin && !noexmode && SETJMP(x_jump_env))
	x_restore_state();
#endif

    clear_oparg(&oa);
    وقتی (!cmdwin || cmdwin_result == 0)
    {
	اگر (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    اگر (need_check_timestamps)
		check_timestamps(FALSE);
	    اگر (need_wait_return)	// if wait_return() still needed ...
		wait_return(FALSE);	// ... call it now
	    اگر (need_start_insertmode && goto_im() && !VIsual_active)
	    {
		need_start_insertmode = FALSE;
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
	    اگر (noexmode && global_busy && !exmode_active && previous_got_int)
	    {
		// Typed two CTRL-C in a row: go back to ex mode as if "Q" was
		// used and keep "got_int" set, so that it aborts ":g".
		exmode_active = EXMODE_NORMAL;
		State = MODE_NORMAL;
	    }
	    وگرنه اگر (!global_busy || !exmode_active)
	    {
		اگر (!quit_more)
		    (تهی)vgetc();		// flush all buffers
		got_int = FALSE;
	    }
	    previous_got_int = TRUE;
	}
	وگرنه
	    previous_got_int = FALSE;

#ifdef FEAT_EVAL
	// At the toplevel there is no exception handling.  Discard any that
	// may be hanging around (e.g. from "interrupt" at the debug prompt).
	اگر (did_throw && !ex_normal_busy)
	    discard_current_exception();
#endif

	اگر (!exmode_active)
	    پیام_scroll = FALSE;
	quit_more = FALSE;

	// it's not safe unless may_trigger_safestate_main() is called
	was_safe = FALSE;

	/*
	 * If skip redraw is set (for ":" in wait_return()), don't redraw now.
	 * If there is nothing in the stuff_buffer or do_redraw is TRUE,
	 * update cursor and redraw.
	 */
	اگر (skip_redraw || exmode_active)
	{
	    skip_redraw = FALSE;
	    setcursor();
	    cursor_on();
	}
	وگرنه اگر (do_redraw || stuff_empty())
	{
#ifdef FEAT_GUI
	    // If ui_breakcheck() was used a resize may have been postponed.
	    gui_may_resize_shell();
#endif
#ifdef HAVE_DROP_FILE
	    // If files were dropped while text was locked or the curbuf was
	    // locked, this would be a good time to handle the drop.
	    handle_any_postponed_drop();
#endif
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
		ex_diffupdate(پوچ);
		curtab->tp_diff_update = FALSE;
	    }

	    // Scroll-binding for diff mode may have been postponed until
	    // here.  Avoids doing it for every change.
	    اگر (diff_need_scrollbind)
	    {
		check_scrollbind((linenr_T)0, 0L);
		diff_need_scrollbind = FALSE;
	    }
#endif
#اگر defined(FEAT_FOLDING)
	    // Include a closed fold completely in the Visual area.
	    foldAdjustVisual();
#endif
#ifdef FEAT_FOLDING
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
#endif

	    // Before redrawing, make sure w_topline is correct, and w_leftcol
	    // if lines don't wrap, and w_skipcol if lines wrap.
	    update_topline();
	    validate_cursor();

	    اگر (VIsual_active)
		update_curbuf(UPD_INVERTED); // update inverted part
	    وگرنه اگر (must_redraw)
	    {
		mch_disable_flush();	// Stop issuing gui_mch_flush().
		update_screen(0);
		mch_enable_flush();
	    }
	    وگرنه اگر (redraw_cmdline || clear_cmdline || redraw_mode)
		showmode();
	    redraw_statuslines();
	    اگر (need_maketitle)
		maketitle();
#ifdef FEAT_VIMINFO
	    curbuf->b_last_used = vim_time();
#endif
	    // display message after redraw
	    اگر (keep_پیام != پوچ)
	    {
		char_u *p = vim_strsave(keep_پیام);

		اگر (p != پوچ)
		{
		    // msg_start() will set keep_msg to NULL, make a copy
		    // first.  Don't reset keep_msg, msg_attr_keep() uses it to
		    // check for duplicates.  Never put this message in
		    // history.
		    پیام_hist_off = TRUE;
		    پیام_attr((نویسه *)p, keep_پیام_attr);
		    پیام_hist_off = FALSE;
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
	    پیام_didany = FALSE;		// reset lines_left in msg_start()
	    may_clear_sb_text();	// clear scroll-back text on next msg
	    showruler(FALSE);

	    setcursor();
	    cursor_on();

	    do_redraw = FALSE;

#ifdef STARTUPTIME
	    // Now that we have drawn the first screen all the startup stuff
	    // has been done, close any file for startup messages.
	    اگر (time_fd != پوچ)
	    {
		TIME_پیام("first screen update");
		TIME_پیام("--- VIM STARTED ---");
		fclose(time_fd);
		time_fd = پوچ;
	    }
#endif
	    // After the first screen update may start triggering WinScrolled
	    // autocmd events.  Store all the scroll positions and sizes now.
	    may_make_initial_scroll_size_snapshot();
	}
#ifdef FEAT_GUI
	اگر (need_mouse_correct)
	    gui_mouse_correct();
#endif

	// May request the keyboard protocol state now.
	may_send_t_RK();

	// Update w_curswant if w_set_curswant has been set.
	// Postponed until here to avoid computing w_virtcol too often.
	update_curswant();

#ifdef FEAT_EVAL
	/*
	 * May perform garbage collection when waiting for a character, but
	 * only at the very toplevel.  Otherwise we may be using a List or
	 * Dict internally somewhere.
	 * "may_garbage_collect" is reset in vgetc() which is invoked through
	 * do_exmode() and normal_cmd().
	 */
	may_garbage_collect = (!cmdwin && !noexmode);
#endif
	/*
	 * If we're invoked as ex, do a round of ex commands.
	 * Otherwise, get and execute a normal mode command.
	 */
	اگر (exmode_active)
	{
	    اگر (noexmode)   // End of ":global/path/visual" commands
		برو theend;
	    do_exmode(exmode_active == EXMODE_VIM);
	}
	وگرنه
	{
#ifdef FEAT_TERMINAL
	    اگر (term_use_loop()
		    && oa.op_type == OP_NOP && oa.regname == NUL
		    && !VIsual_active
		    && !skip_term_loop)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal mode.  With FAIL we first need to position the
		// cursor and the screen needs to be redrawn.
		اگر (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    وگرنه
#endif
	    {
#ifdef FEAT_TERMINAL
		skip_term_loop = FALSE;
#endif
		normal_cmd(&oa, TRUE);
	    }
	}
    }

theend:
    current_oap = prev_oap;
}


#اگر defined(USE_XSMP) || defined(FEAT_GUI)
/*
 * Exit, but leave behind swap files for modified buffers.
 */
    تهی
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
#endif


/*
 * Exit properly.  This is the only way to exit Vim after startup has
 * succeeded.  We are certain to exit here, no way to abort it.
 */
    تهی
getout(صحیح exitval)
{
    exiting = TRUE;
#اگر defined(FEAT_EVAL)
    ch_log(پوچ, "Exiting...");
#endif

    // When running in Ex mode an error causes us to exit with a non-zero exit
    // code.  POSIX requires this, although it's not 100% clear from the
    // standard.
    اگر (exmode_active)
	exitval += ex_exitval;

#ifdef FEAT_EVAL
    set_vim_var_type(VV_EXITING, VAR_عدد);
    set_vim_var_nr(VV_EXITING, exitval);
#endif

    // Position the cursor on the last screen line, below all the text
    اگر (!is_not_a_term_or_gui())
	windgoto((صحیح)Rows - 1, 0);

#ifdef FEAT_EVAL
    // Invoked all deferred functions in the function stack.
    invoke_all_defer();
#endif

#اگر defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
    // Optionally print hashtable efficiency.
    hash_debug_results();
#endif

#ifdef FEAT_GUI
    پیام_didany = FALSE;
#endif

    اگر (v_dying <= 1)
    {
	tabpage_T	*tp;
	tabpage_T	*next_tp;
	buf_T		*buf;
	win_T		*wp;
	صحیح		unblock = 0;

	// Trigger BufWinLeave for all windows, but only once per buffer.
	برای (tp = first_tabpage; tp != پوچ; tp = next_tp)
	{
	    next_tp = tp->tp_next;
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		اگر (wp->w_buffer == پوچ || !buf_valid(wp->w_buffer))
		    // Autocmd must have close the buffer already, skip.
		    ادامه;
		buf = wp->w_buffer;
		اگر (CHANGEDTICK(buf) != -1)
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf);
		    apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname,
						    buf->b_fname, FALSE, buf);
		    اگر (bufref_valid(&bufref))
			CHANGEDTICK(buf) = -1;  // note we did it already

		    // start all over, autocommands may mess up the lists
		    next_tp = first_tabpage;
		    بشکن;
		}
	    }
	}

	// Trigger BufUnload for buffers that are loaded
	FOR_ALL_BUFFERS(buf)
	    اگر (buf->b_ml.ml_mfp != پوچ)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf);
		اگر (!bufref_valid(&bufref))
		    // autocmd deleted the buffer
		    بشکن;
	    }

	// deathtrap() blocks autocommands, but we do want to trigger
	// VimLeavePre.
	اگر (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVEPRE, پوچ, پوچ, FALSE, curbuf);
	اگر (unblock)
	    block_autocmds();
    }

#ifdef FEAT_VIMINFO
    اگر (
# ifdef EXITFREE
	    entered_free_all_mem == FALSE &&
# endif
	    *p_viminfo != NUL)
	// Write out the registers, history, marks etc, to the viminfo file
	write_viminfo(پوچ, FALSE);
#endif

    اگر (v_dying <= 1)
    {
	صحیح	unblock = 0;

	// deathtrap() blocks autocommands, but we do want to trigger VimLeave.
	اگر (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVE, پوچ, پوچ, FALSE, curbuf);
	اگر (unblock)
	    block_autocmds();
    }

#ifdef FEAT_PROFILE
    profile_dump();
#endif

    اگر (did_emsg
#ifdef FEAT_GUI
	    || (gui.in_use && پیام_didany && p_verbose > 0)
#endif
	    )
    {
	// give the user a chance to read the (error) message
	no_wait_return = FALSE;
	wait_return(FALSE);
    }

    // Position the cursor again, the autocommands may have moved it
    اگر (!is_not_a_term_or_gui())
	windgoto((صحیح)Rows - 1, 0);

#ifdef FEAT_JOB_CHANNEL
    job_stop_on_exit();
#endif
#ifdef FEAT_LUA
    lua_end();
#endif
#ifdef FEAT_MZSCHEME
    mzscheme_end();
#endif
#ifdef FEAT_TCL
    tcl_end();
#endif
#ifdef FEAT_RUBY
    ruby_end();
#endif
#ifdef FEAT_PYTHON
    python_end();
#endif
#ifdef FEAT_PYTHON3
    python3_end();
#endif
#ifdef FEAT_PERL
    perl_end();
#endif
#اگر defined(USE_ICONV) && defined(DYNAMIC_ICONV)
    iconv_end();
#endif
#ifdef FEAT_NETBEANS_INTG
    netbeans_end();
#endif
#ifdef FEAT_SOCKETSERVER
    socketserver_stop();
#endif
#ifdef FEAT_CSCOPE
    cs_end();
#endif
#ifdef FEAT_IMAGE
    uninit_image_state();
#endif
#ifdef FEAT_EVAL
    اگر (garbage_collect_at_exit)
	garbage_collect(FALSE);
#endif
#ifdef MSWIN
    free_cmd_argsW();
#endif

    term_disable_dec();

    mch_exit(exitval);
}

/*
 * Get the name of the display, before gui_prepare() removes it from
 * argv[].  Used for the xterm-clipboard display.
 *
 * Also find the --server, --clientserver... arguments and --socketid and
 * --windowid
 */
    ایستا تهی
early_arg_scan(mparm_T *parmp UNUSED)
{
#اگر defined(FEAT_XCLIPBOARD) || defined(FEAT_CLIENTSERVER) \
	|| !defined(FEAT_NETBEANS_INTG)
    صحیح		argc = parmp->argc;
    نویسه	**argv = parmp->argv;
    صحیح		i;

    برای (i = 1; i < argc; i++)
    {
	اگر (STRCMP(argv[i], "--") == 0)
	    بشکن;
# ifdef FEAT_XCLIPBOARD
	وگرنه اگر (STRICMP(argv[i], "-display") == 0
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
	وگرنه اگر (STRICMP(argv[i], "--servername") == 0)
	{
	    اگر (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    parmp->serverName_arg = (char_u *)argv[++i];
	}
	وگرنه اگر (STRICMP(argv[i], "--serverlist") == 0)
	    parmp->serverArg = TRUE;
	وگرنه اگر (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    parmp->serverArg = TRUE;
#  ifdef FEAT_GUI
	    اگر (strstr(argv[i], "-wait") != 0)
		// don't fork() when starting the GUI to edit files ourself
		gui.dofork = false;
#  endif
	}
#  ifdef FEAT_CLIENTSERVER_BACKENDS
	وگرنه اگر (STRICMP(argv[i], "--clientserver") == 0)
	{
	    char_u *arg;
	    اگر (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    arg = (char_u *)argv[++i];

	    اگر (STRICMP(arg, "socket") == 0)
		clientserver_method = CLIENTSERVER_METHOD_SOCKET;
#   ifdef FEAT_X11
	    وگرنه اگر (STRICMP(arg, "x11") == 0)
		clientserver_method = CLIENTSERVER_METHOD_X11;
#   endif
#   ifdef MSWIN
	    وگرنه اگر (STRICMP(arg, "mswin") == 0)
		clientserver_method = CLIENTSERVER_METHOD_MSWIN;
#   endif
	    وگرنه
		mainerr(ME_UNKNOWN_OPTION, arg);
	}
#  endif
# endif

# اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
#  ifdef FEAT_GUI_MSWIN
	وگرنه اگر (STRICMP(argv[i], "--windowid") == 0)
#  وگرنه
	وگرنه اگر (STRICMP(argv[i], "--socketid") == 0)
#  endif
	{
	    long_u	id;
	    صحیح		شمار;

	    اگر (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    اگر (STRNICMP(argv[i+1], "0x", 2) == 0)
		شمار = sscanf(&(argv[i + 1][2]), SCANF_HEX_LONG_U, &id);
	    وگرنه
		شمار = sscanf(argv[i + 1], SCANF_DECIMAL_LONG_U, &id);
	    اگر (شمار != 1)
		mainerr(ME_INVALID_ARG, (char_u *)argv[i]);
	    وگرنه
#  ifdef FEAT_GUI_MSWIN
		win_socket_id = id;
#  elif !defined(USE_GTK4)
		gtk_socket_id = id;
#  endif
	    i++;
	}
# endif
# اگر defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
	وگرنه اگر (STRICMP(argv[i], "--echo-wid") == 0)
	    echo_wid_arg = TRUE;
# endif
# ifndef FEAT_NETBEANS_INTG
	وگرنه اگر (strncmp(argv[i], "-nb", (size_t)3) == 0)
	{
	    mch_errmsg(_("'-nb' cannot be used: not enabled at compile time\n"));
	    mch_exit(2);
	}
# endif

    }
#endif
}

#ifndef NO_VIM_MAIN
/*
 * Get a (optional) count for a Vim argument.
 */
    ایستا صحیح
get_عدد_arg(
    char_u	*p,	    // pointer to argument
    صحیح		*idx,	    // index in argument, is incremented
    صحیح		def)	    // default value
{
    اگر (vim_isdigit(p[*idx]))
    {
	def = atoi((نویسه *)&(p[*idx]));
	وقتی (vim_isdigit(p[*idx]))
	    *idx = *idx + 1;
    }
    بازگشت def;
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
    ایستا تهی
parse_command_name(mparm_T *parmp)
{
    char_u	*initstr;

    initstr = gettail((char_u *)parmp->argv[0]);

# ifdef FEAT_EVAL
    set_vim_var_string(VV_PROGNAME, initstr, -1);
    set_progpath((char_u *)parmp->argv[0]);
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
	parmp->evim_mode = TRUE;
	++initstr;
    }

    // "gvim" starts the GUI.  Also accept "Gvim" for MS-Windows.
    اگر (TOLOWER_ASC(initstr[0]) == 'g'
# ifdef VIMDLL
	    || mch_is_gui_executable()
# endif
       )
    {
	main_start_gui();
# ifdef FEAT_GUI
	++initstr;
# endif
# ifdef GUI_MAY_SPAWN
	gui.dospawn = false;	// No need to spawn a new process.
# endif
    }
# ifdef GUI_MAY_SPAWN
    وگرنه
	gui.dospawn = true;	// Not "gvim". Need to spawn gvim.exe.
# endif


    اگر (STRNICMP(initstr, "view", 4) == 0)
    {
	readonlymode = TRUE;
	curbuf->b_p_ro = TRUE;
	p_uc = 10000;			// don't update very often
	initstr += 4;
    }
    وگرنه اگر (STRNICMP(initstr, "vim", 3) == 0)
	initstr += 3;

    // Catch "[r][g]vimdiff" and "[r][g]viewdiff".
    اگر (STRICMP(initstr, "diff") == 0)
    {
# ifdef FEAT_DIFF
	parmp->diff_mode = TRUE;
# وگرنه
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
	    exmode_active = EXMODE_VIM;
	وگرنه
	    exmode_active = EXMODE_NORMAL;
	change_compatible(TRUE);	// set 'compatible'
    }
}

/*
 * Scan the command line arguments.
 */
    ایستا تهی
command_سطر_scan(mparm_T *parmp)
{
    صحیح		argc = parmp->argc;
    نویسه	**argv = parmp->argv;
    صحیح		argv_idx;		// index in argv[n][]
    صحیح		had_minmin = FALSE;	// found "--" argument
    صحیح		want_argument;		// option argument with argument
    صحیح		c;
    char_u	*p = پوچ;
    long	n;

    --argc;
    ++argv;
    argv_idx = 1;	    // active option letter is argv[0][argv_idx]
    وقتی (argc > 0)
    {
	/*
	 * "+" or "+{number}" or "+/{pat}" or "+{command}" argument.
	 */
	اگر (argv[0][0] == '+' && !had_minmin)
	{
	    اگر (parmp->n_commands >= MAX_ARG_CMDS)
		mainerr(ME_EXTRA_CMD, پوچ);
	    argv_idx = -1;	    // skip to next argument
	    اگر (argv[0][1] == NUL)
		parmp->commands[parmp->n_commands++] = (char_u *)"$";
	    وگرنه
		parmp->commands[parmp->n_commands++] = (char_u *)&(argv[0][1]);
	}

	/*
	 * Optional argument.
	 */
	وگرنه اگر (argv[0][0] == '-' && !had_minmin)
	{
	    want_argument = FALSE;
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
		اگر (exmode_active)
		    silent_mode = TRUE;
		وگرنه
		{
		    اگر (parmp->edit_type != EDIT_NONE)
			mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
		    parmp->edit_type = EDIT_STDIN;
		    read_cmd_fd = 2;	// read from stderr instead of stdin
		}
		argv_idx = -1;		// skip to next argument
		بشکن;

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
		وگرنه اگر (STRICMP(argv[0] + argv_idx, "version") == 0)
		{
		    cmdline_width = Columns = 80;   // need to init Columns
		    info_message = TRUE; // use mch_msg(), not mch_errmsg()
# اگر defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
		    gui.starting = false; // not starting GUI, will exit
# endif
		    list_version();
		    پیام_putchar('\n');
		    پیام_didout = FALSE;
		    mch_exit(0);
		}
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "clean", 5) == 0)
		{
		    parmp->use_vimrc = (char_u *)"DEFAULTS";
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)"NONE";
# endif
		    parmp->clean = TRUE;
		    set_option_مقدار_give_err((char_u *)"vif",
						      0L, (char_u *)"NONE", 0);
		}
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "literal", 7) == 0)
		{
# ifdef EXPAND_FILENAMES
		    parmp->literal = TRUE;
# endif
		}
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "nofork", 6) == 0)
		{
# ifdef FEAT_GUI
		    gui.dofork = false;	// don't fork() when starting GUI
# endif
		}
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "noplugin", 8) == 0)
		    p_lpl = FALSE;
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "not-a-term", 10) == 0)
		    parmp->not_a_term = TRUE;
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "gui-dialog-file", 15)
									 == 0)
		{
		    want_argument = TRUE;
		    argv_idx += 15;
		}
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "ttyfail", 7) == 0)
		    parmp->tty_fail = TRUE;
# ifdef FEAT_JOB_CHANNEL
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "stdio-channel", 13) == 0)
		{
		    // Like "-es": no terminal, but the channel is served
		    // instead of reading Ex commands.
		    use_stdio_channel = TRUE;
		    exmode_active = EXMODE_NORMAL;
		    silent_mode = TRUE;
#  اگر defined(FEAT_GUI) && !defined(VIMDLL)
		    gui.starting = false;	// don't start GUI
#  endif
		}
# endif
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "cmd", 3) == 0)
		{
		    want_argument = TRUE;
		    argv_idx += 3;
		}
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "startuptime", 11) == 0)
		{
		    want_argument = TRUE;
		    argv_idx += 11;
		}
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "log", 3) == 0)
		{
		    want_argument = TRUE;
		    argv_idx += 3;
		}
# ifdef FEAT_CLIENTSERVER
		وگرنه اگر (STRICMP(argv[0] + argv_idx, "serverlist") == 0)
		    ; // already processed -- no arg
		وگرنه اگر (STRICMP(argv[0] + argv_idx, "servername") == 0
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
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "socketid", 8) == 0)
#  وگرنه
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "windowid", 8) == 0)
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
		وگرنه اگر (STRNICMP(argv[0] + argv_idx, "echo-wid", 8) == 0)
		{
		    // already processed, skip
		}
# endif
		وگرنه
		{
		    اگر (argv[0][argv_idx])
			mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
		    had_minmin = TRUE;
		}
		اگر (!want_argument)
		    argv_idx = -1;	// skip to next argument
		بشکن;

	    حالت 'A':		// "-A" start in Arabic mode
# ifdef FEAT_ARABIC
		set_option_مقدار_give_err((char_u *)"arabic", 1L, پوچ, 0);
# وگرنه
		mch_errmsg(_(e_arabic_cannot_be_used_not_enabled_at_compile_time));
		mch_exit(2);
# endif
		بشکن;

	    حالت 'b':		// "-b" binary mode
		// Needs to be effective before expanding file names, because
		// for Win32 this makes us edit a shortcut file itself,
		// instead of the file it links to.
		set_options_bin(curbuf->b_p_bin, 1, 0);
		curbuf->b_p_bin = 1;	    // binary file I/O
		بشکن;

	    حالت 'C':		// "-C"  Compatible
		change_compatible(TRUE);
		has_dash_c_arg = TRUE;
		بشکن;

	    حالت 'e':		// "-e" Ex mode
		exmode_active = EXMODE_NORMAL;
		بشکن;

	    حالت 'E':		// "-E" Improved Ex mode
		exmode_active = EXMODE_VIM;
		بشکن;

	    حالت 'f':		// "-f"  GUI: run in foreground.  Amiga: open
				// window directly, not with newcli
# ifdef FEAT_GUI
		gui.dofork = false;	// don't fork() when starting GUI
# endif
		بشکن;

	    حالت 'g':		// "-g" start GUI
		main_start_gui();
		بشکن;

	    حالت 'F':		// "-F" was for Farsi mode
		mch_errmsg(_(e_farsi_support_has_been_removed));
		mch_exit(2);
		بشکن;

	    حالت '?':		// "-?" give help message (for MS-Windows)
	    حالت 'h':		// "-h" give help message
# ifdef FEAT_GUI_GNOME
		// Tell usage() to exit for "gvim".
		gui.starting = false;
# endif
		usage();
		بشکن;

	    حالت 'H':		// "-H" start in Hebrew mode: rl + hkmap set
# ifdef FEAT_RIGHTLEFT
		p_hkmap = TRUE;
		set_option_مقدار_give_err((char_u *)"rl", 1L, پوچ, 0);
# وگرنه
		mch_errmsg(_(e_hebrew_cannot_be_used_not_enabled_at_compile_time));
		mch_exit(2);
# endif
		بشکن;

	    حالت 'l':		// "-l" lisp mode, 'lisp' and 'showmatch' on
		set_option_مقدار_give_err((char_u *)"lisp", 1L, پوچ, 0);
		p_sm = TRUE;
		بشکن;

	    حالت 'M':		// "-M"  no changes or writing of files
		reset_modifiable();
		// FALLTHROUGH

	    حالت 'm':		// "-m"  no writing of files
		p_write = FALSE;
		بشکن;

	    حالت 'y':		// "-y"  easy mode
# ifdef FEAT_GUI
		gui.starting = true;	// start GUI a bit later
# endif
		parmp->evim_mode = TRUE;
		بشکن;

	    حالت 'N':		// "-N"  Nocompatible
		change_compatible(FALSE);
		بشکن;

	    حالت 'n':		// "-n" no swap file
# ifdef FEAT_NETBEANS_INTG
		// checking for "-nb", netbeans parameters
		اگر (argv[0][argv_idx] == 'b')
		{
		    netbeansArg = argv[0];
		    argv_idx = -1;	    // skip to next argument
		}
		وگرنه
# endif
		parmp->no_swap_file = TRUE;
		بشکن;

	    حالت 'p':		// "-p[N]" open N tab pages
# ifdef TARGET_API_MAC_OSX
		// For some reason on MacOS X, an argument like:
		// -psn_0_10223617 is passed in when invoke from Finder
		// or with the 'open' command
		اگر (argv[0][argv_idx] == 's')
		{
		    argv_idx = -1; // bypass full -psn
		    main_start_gui();
		    بشکن;
		}
# endif
		// default is 0: open window for each file
		parmp->window_شمار = get_عدد_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->window_layout = WIN_TABS;
		بشکن;

	    حالت 'o':		// "-o[N]" open N horizontal split windows
		// default is 0: open window for each file
		parmp->window_شمار = get_عدد_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->window_layout = WIN_HOR;
		بشکن;

		حالت 'O':	// "-O[N]" open N vertical split windows
		// default is 0: open window for each file
		parmp->window_شمار = get_عدد_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->window_layout = WIN_VER;
		بشکن;

# ifdef FEAT_QUICKFIX
	    حالت 'q':		// "-q" QuickFix mode
		اگر (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
		parmp->edit_type = EDIT_QF;
		اگر (argv[0][argv_idx])		// "-q{errorfile}"
		{
		    parmp->use_ef = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		وگرنه اگر (argc > 1)		// "-q {errorfile}"
		    want_argument = TRUE;
		بشکن;
# endif

	    حالت 'R':		// "-R" readonly mode
		readonlymode = TRUE;
		curbuf->b_p_ro = TRUE;
		p_uc = 10000;			// don't update very often
		بشکن;

	    حالت 'r':		// "-r" recovery mode
	    حالت 'L':		// "-L" recovery mode
		recoverymode = 1;
		بشکن;

	    حالت 's':
		اگر (exmode_active)	// "-s" silent (batch) mode
		    silent_mode = TRUE;
		وگرنه		// "-s {scriptin}" read from script file
		    want_argument = TRUE;
		بشکن;

	    حالت 't':		// "-t {tag}" or "-t{tag}" jump to tag
		اگر (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
		parmp->edit_type = EDIT_TAG;
		اگر (argv[0][argv_idx])		// "-t{tag}"
		{
		    parmp->tagname = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		وگرنه				// "-t {tag}"
		    want_argument = TRUE;
		بشکن;

# ifdef FEAT_EVAL
	    حالت 'D':		// "-D"		Debugging
		parmp->use_debug_break_level = 9999;
		بشکن;
# endif
# ifdef FEAT_DIFF
	    حالت 'd':		// "-d"		'diff'
#  ifdef AMIGA
		// check for "-dev {device}"
		اگر (argv[0][argv_idx] == 'e' && argv[0][argv_idx + 1] == 'v')
		    want_argument = TRUE;
		وگرنه
#  endif
		    parmp->diff_mode = TRUE;
		بشکن;
# endif
	    حالت 'V':		// "-V{N}"	Verbose level
		// default is 10: a little bit verbose
		p_verbose = get_عدد_arg((char_u *)argv[0], &argv_idx, 10);
		اگر (argv[0][argv_idx] != NUL)
		{
		    set_option_مقدار_give_err((char_u *)"verbosefile",
					  0L, (char_u *)argv[0] + argv_idx, 0);
		    argv_idx = (صحیح)STRLEN(argv[0]);
		}
		بشکن;

	    حالت 'v':		// "-v"  Vi-mode (as if called "vi")
		exmode_active = 0;
# اگر defined(FEAT_GUI) && !defined(VIMDLL)
		gui.starting = false;	// don't start GUI
# endif
		بشکن;

	    حالت 'w':		// "-w{number}"	set window height
				// "-w {scriptout}"	write to script
		اگر (vim_isdigit(((char_u *)argv[0])[argv_idx]))
		{
		    n = get_عدد_arg((char_u *)argv[0], &argv_idx, 10);
		    set_option_مقدار_give_err((char_u *)"window", n, پوچ, 0);
		    بشکن;
		}
		want_argument = TRUE;
		بشکن;

# ifdef FEAT_CRYPT
	    حالت 'x':		// "-x"  encrypted reading/writing of files
		parmp->ask_for_کلید = TRUE;
		بشکن;
# endif

	    حالت 'X':		// "-X"  don't connect to X server
# اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
		x_no_connect = TRUE;
# endif
		بشکن;
	    حالت 'Y':		// "-Y" don't connect to Wayland compositor
# اگر defined(FEAT_WAYLAND)
		wayland_no_connect = TRUE;
# endif
		بشکن;

	    حالت 'Z':		// "-Z"  restricted mode
		restricted = TRUE;
		بشکن;

	    حالت 'c':		// "-c{command}" or "-c {command}" execute
				// command
		اگر (argv[0][argv_idx] != NUL)
		{
		    اگر (parmp->n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, پوچ);
		    parmp->commands[parmp->n_commands++] = (char_u *)argv[0]
								   + argv_idx;
		    argv_idx = -1;
		    بشکن;
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
		want_argument = TRUE;
		بشکن;

	    پیشفرض:
		mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
	    }

	    /*
	     * Handle option arguments with argument.
	     */
	    اگر (want_argument)
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
			mainerr(ME_EXTRA_CMD, پوچ);
		    اگر (c == 'S')
		    {
			نویسه	*a;

			اگر (argc < 1)
			    // "-S" without argument: use default session file
			    // name.
			    a = SESSION_FILE;
			وگرنه اگر (argv[0][0] == '-')
			{
			    // "-S" followed by another option: use default
			    // session file name.
			    a = SESSION_FILE;
			    ++argc;
			    --argv;
			}
			وگرنه
			    a = argv[0];
			p = alloc(STRLEN(a) + 4);
			اگر (p == پوچ)
			    mch_exit(2);
			sprintf((نویسه *)p, "so %s", a);
			parmp->cmds_tofree[parmp->n_commands] = TRUE;
			parmp->commands[parmp->n_commands++] = p;
		    }
		    وگرنه
			parmp->commands[parmp->n_commands++] =
							    (char_u *)argv[0];
		    بشکن;

		حالت '-':
		    اگر (argv[-1][2] == 'c')
		    {
			// "--cmd {command}" execute command
			اگر (parmp->n_pre_commands >= MAX_ARG_CMDS)
			    mainerr(ME_EXTRA_CMD, پوچ);
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
		    بشکن;

	    //	case 'd':   -d {device} is handled in mch_check_win() for the
	    //		    Amiga

# ifdef FEAT_QUICKFIX
		حالت 'q':	// "-q {errorfile}" QuickFix mode
		    parmp->use_ef = (char_u *)argv[0];
		    بشکن;
# endif

		حالت 'i':	// "-i {viminfo}" use for viminfo
		    set_option_مقدار_give_err((char_u *)"vif",
						     0L, (char_u *)argv[0], 0);
		    بشکن;

		حالت 's':	// "-s {scriptin}" read from script file
		    اگر (scriptin[0] != پوچ)
		    {
scripterror:
			mch_errmsg(_("Attempt to open script file again: \""));
			mch_errmsg(argv[-1]);
			mch_errmsg(" ");
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    اگر ((scriptin[0] = mch_fopen(argv[0], READBIN)) == پوچ)
		    {
			mch_errmsg(_("Cannot open for reading: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    اگر (save_typebuf() == FAIL)
			mch_exit(2);	// out of memory
		    بشکن;

		حالت 't':	// "-t {tag}"
		    parmp->tagname = (char_u *)argv[0];
		    بشکن;

		حالت 'T':	// "-T {terminal}" terminal name
		    /*
		     * The -T term argument is always available and when
		     * HAVE_TERMLIB is supported it overrides the environment
		     * variable TERM.
		     */
# ifdef FEAT_GUI
		    اگر (term_is_gui((char_u *)argv[0]))
			gui.starting = true;	// start GUI a bit later
		    وگرنه
# endif
			parmp->term = (char_u *)argv[0];
		    بشکن;

		حالت 'u':	// "-u {vimrc}" vim inits file
		    parmp->use_vimrc = (char_u *)argv[0];
		    بشکن;

		حالت 'U':	// "-U {gvimrc}" gvim inits file
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)argv[0];
# endif
		    بشکن;

		حالت 'w':	// "-w {nr}" 'window' value
				// "-w {scriptout}" append to script file
		    اگر (vim_isdigit(*((char_u *)argv[0])))
		    {
			argv_idx = 0;
			n = get_عدد_arg((char_u *)argv[0], &argv_idx, 10);
			set_option_مقدار_give_err((char_u *)"window",
								   n, پوچ, 0);
			argv_idx = -1;
			بشکن;
		    }
		    // FALLTHROUGH
		حالت 'W':	// "-W {scriptout}" overwrite script file
		    اگر (scriptout != پوچ)
			برو scripterror;
		    اگر ((scriptout = mch_fopen(argv[0],
				    c == 'w' ? APPENDBIN : WRITEBIN)) == پوچ)
		    {
			mch_errmsg(_("Cannot open for script output: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    بشکن;

# ifdef FEAT_GUI_MSWIN
		حالت 'P':		// "-P {parent title}" MDI parent
		    gui_mch_set_parent(argv[0]);
		    بشکن;
# endif
		}
	    }
	}

	/*
	 * File name argument.
	 */
	وگرنه
	{
	    argv_idx = -1;	    // skip to next argument

	    // Check for only one type of editing.
	    اگر (parmp->edit_type != EDIT_NONE && parmp->edit_type != EDIT_FILE)
		mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
	    parmp->edit_type = EDIT_FILE;

# ifdef MSWIN
	    // Remember if the argument was a full path before changing
	    // slashes to backslashes.
	    اگر (argv[0][0] != NUL && argv[0][1] == ':' && argv[0][2] == '\\')
		parmp->full_path = TRUE;
# endif

	    // Add the file to the global argument list.
	    اگر (ga_grow(&global_alist.al_ga, 1) == FAIL
		    || (p = vim_strsave((char_u *)argv[0])) == پوچ)
		mch_exit(2);
# ifdef FEAT_DIFF
	    اگر (parmp->diff_mode && mch_isdir(p) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *tail;
		string_T    ret;

		tail = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(p, STRLEN(p), tail, STRLEN(tail), TRUE, &ret);
		اگر (ret.string != پوچ)
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
	    اگر (vim_strpbrk(p, "\\:") != پوچ && !path_with_url(p))
	    {
		نویسه posix_path[MAXPATHL];

#  اگر CYGWIN_VERSION_DLL_MAJOR >= 1007
		cygwin_conv_path(CCP_WIN_A_TO_POSIX, p, posix_path, MAXPATHL);
#  وگرنه
		cygwin_conv_to_posix_path(p, posix_path);
#  endif
		vim_free(p);
		p = vim_strsave((char_u *)posix_path);
		اگر (p == پوچ)
		    mch_exit(2);
	    }
# endif

# ifdef USE_FNAME_CASE
	    // Make the case of the file name match the actual file.
	    fname_case(p, 0);
# endif

	    alist_add(&global_alist, p,
# ifdef EXPAND_FILENAMES
		    parmp->literal ? 2 : 0	// add buffer nr after exp.
# وگرنه
		    2		// add buffer number now and use curbuf
# endif
		    );

# ifdef MSWIN
	    {
		// Remember this argument has been added to the argument list.
		// Needed when 'encoding' is changed.
		used_file_arg(argv[0], parmp->literal, parmp->full_path,
#  ifdef FEAT_DIFF
							    parmp->diff_mode
#  وگرنه
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
	اگر (p != پوچ)
	{
	    sprintf((نویسه *)p, ":%s\r", parmp->commands[0]);
	    set_vim_var_string(VV_SWAPCOMMAND, p, (صحیح)plen);
	    vim_free(p);
	}
    }
# endif
}

/*
 * Print a warning if stdout is not a terminal.
 * When starting in Ex mode and commands come from a file, set silent_mode.
 */
    ایستا تهی
check_tty(mparm_T *parmp)
{
    صحیح		درون_داد_isatty;		// is active input a terminal?

    درون_داد_isatty = mch_درون_داد_isatty();
    اگر (exmode_active)
    {
	اگر (!درون_داد_isatty)
	    silent_mode = TRUE;
    }
    وگرنه اگر (parmp->want_full_screen && (!stdout_isatty || !درون_داد_isatty)
# ifdef FEAT_GUI
	    // don't want the delay when started from the desktop
	    && !gui.starting
# endif
	    && !parmp->not_a_term)
    {
# ifdef NBDEBUG
	/*
	 * This shouldn't be necessary. But if I run netbeans with the log
	 * output coming to the console and XOpenDisplay fails, I get vim
	 * trying to start with input/output to my console tty.  This fills my
	 * input buffer so fast I can't even kill the process in under 2
	 * minutes (and it beeps continuously the whole time :-)
	 */
	اگر (netbeans_active() && (!stdout_isatty || !درون_داد_isatty))
	{
	    mch_errmsg(_("Vim: Error: Failure to start gvim from NetBeans\n"));
	    exit(1);
	}
# endif
# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	اگر (
#  ifdef VIMDLL
	    !gui.starting &&
#  endif
	    is_cygpty_used())
	{
#  اگر defined(HAVE_BIND_TEXTDOMAIN_CODESET) \
	&& defined(FEAT_GETTEXT)
	    نویسه    *s, *tofree = پوچ;

	    // Set the encoding of the error message based on $LC_ALL or
	    // other environment variables instead of 'encoding'.
	    // Note that the message is shown on a Cygwin terminal (e.g.
	    // mintty) which encoding is based on $LC_ALL or etc., not the
	    // current codepage used by normal Win32 console programs.
	    tofree = s = (نویسه *)enc_locale_env(پوچ);
	    اگر (s == پوچ)
		s = "utf-8";	// Use "utf-8" by default.
	    (تهی)bind_textdomain_codeset(VIMPACKAGE, s);
	    vim_free(tofree);
#  endif
	    mch_errmsg(_("Vim: Error: This version of Vim does not run in a Cygwin terminal\n"));
	    exit(1);
	}
# endif
	اگر (!stdout_isatty)
	    mch_errmsg(_("Vim: Warning: Output is not to a terminal\n"));
	اگر (!درون_داد_isatty)
	    mch_errmsg(_("Vim: Warning: Input is not from a terminal\n"));
	out_flush();
	اگر (parmp->tty_fail && (!stdout_isatty || !درون_داد_isatty))
	    exit(1);
	اگر (scriptin[0] == پوچ)
	    ui_delay(2005L, TRUE);
	TIME_پیام("Warning delay");
    }
}

/*
 * Read text from stdin.
 */
    ایستا تهی
read_stdin(تهی)
{
    صحیح	    i;

    // When getting the ATTENTION prompt here, use a dialog
    swap_exists_action = SEA_DIALOG;

    no_wait_return = TRUE;
    i = پیام_didany;
    set_buflisted(TRUE);

    // Create memfile and read from stdin.
    (تهی)open_buffer(TRUE, پوچ, 0);

    no_wait_return = FALSE;
    پیام_didany = i;
    TIME_پیام("reading stdin");

    check_swap_exists_action();

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
    ایستا تهی
create_windows(mparm_T *parmp UNUSED)
{
    صحیح		dorewind;
    صحیح		done = 0;

    /*
     * Create the number of windows that was requested.
     */
    اگر (parmp->window_شمار == -1)	// was not set
	parmp->window_شمار = 1;
    اگر (parmp->window_شمار == 0)
	parmp->window_شمار = GARGCOUNT;
    اگر (parmp->window_شمار > 1)
    {
	// Don't change the windows if there was a command in .vimrc that
	// already split some windows
	اگر (parmp->window_layout == 0)
	    parmp->window_layout = WIN_HOR;
	اگر (parmp->window_layout == WIN_TABS)
	{
	    parmp->window_شمار = make_tabpages(parmp->window_شمار);
	    TIME_پیام("making tab pages");
	}
	وگرنه اگر (firstwin->w_next == پوچ)
	{
	    parmp->window_شمار = make_windows(parmp->window_شمار,
					     parmp->window_layout == WIN_VER);
	    TIME_پیام("making windows");
	}
	وگرنه
	    parmp->window_شمار = win_شمار();
    }
    وگرنه
	parmp->window_شمار = 1;

    اگر (recoverymode)			// do recover
    {
	پیام_scroll = TRUE;		// scroll message up
	ml_recover(TRUE);
	اگر (curbuf->b_ml.ml_mfp == پوچ) // failed
	    getout(1);
	do_modelines(0);		// do modelines
    }
    وگرنه
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
	وقتی (done++ < 1000)
	{
	    اگر (dorewind)
	    {
		اگر (parmp->window_layout == WIN_TABS)
		    goto_tabpage(1);
		وگرنه
		    curwin = firstwin;
	    }
	    وگرنه اگر (parmp->window_layout == WIN_TABS)
	    {
		اگر (curtab->tp_next == پوچ)
		    بشکن;
		goto_tabpage(0);
	    }
	    وگرنه
	    {
		اگر (curwin->w_next == پوچ)
		    بشکن;
		curwin = curwin->w_next;
	    }
	    dorewind = FALSE;
	    curbuf = curwin->w_buffer;
	    اگر (curbuf->b_ml.ml_mfp == پوچ)
	    {
# ifdef FEAT_FOLDING
		// Set 'foldlevel' to 'foldlevelstart' if it's not negative.
		اگر (p_fdls >= 0)
		    curwin->w_p_fdl = p_fdls;
# endif
		// When getting the ATTENTION prompt here, use a dialog
		swap_exists_action = SEA_DIALOG;

		set_buflisted(TRUE);

		// create memfile, read file
		(تهی)open_buffer(FALSE, پوچ, 0);

		اگر (swap_exists_action == SEA_QUIT)
		{
		    اگر (got_int || only_one_window())
		    {
			// abort selected or quit and only one window
			did_emsg = FALSE;   // avoid hit-enter prompt
			getout(1);
		    }
		    // We can't close the window, it would disturb what
		    // happens next.  Clear the file name and set the arg
		    // index to -1 to delete it later.
		    setfname(curbuf, پوچ, پوچ, FALSE);
		    curwin->w_arg_idx = -1;
		    swap_exists_action = SEA_NONE;
		}
		وگرنه
		    handle_swap_exists(پوچ);
		dorewind = TRUE;		// start again
	    }
	    ui_breakcheck();
	    اگر (got_int)
	    {
		(تهی)vgetc();	// only break the file loading, not the rest
		بشکن;
	    }
	}
	اگر (parmp->window_layout == WIN_TABS)
	    goto_tabpage(1);
	وگرنه
	    curwin = firstwin;
	curbuf = curwin->w_buffer;
	--autocmd_no_enter;
	--autocmd_no_leave;
    }
}

/*
 * If opened more than one window, start editing files in the other
 * windows.  make_windows() has already opened the windows.
 */
    ایستا تهی
edit_buffers(
    mparm_T	*parmp,
    char_u	*cwd)			// current working dir
{
    صحیح		arg_idx;		// index in argument list
    صحیح		i;
    صحیح		advance = TRUE;
    win_T	*win;
    char_u	*p_shm_save = پوچ;

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
    برای (i = 1; i < parmp->window_شمار; ++i)
    {
	اگر (cwd != پوچ)
	    mch_chdir((نویسه *)cwd);
	// When w_arg_idx is -1 remove the window (see create_windows()).
	اگر (curwin->w_arg_idx == -1)
	{
	    ++arg_idx;
	    win_close(curwin, TRUE);
	    advance = FALSE;
	    ادامه;
	}

	اگر (advance)
	{
	    اگر (parmp->window_layout == WIN_TABS)
	    {
		اگر (curtab->tp_next == پوچ)	// just checking
		    بشکن;
		goto_tabpage(0);
		// Temporarily reset 'shm' option to not print fileinfo when
		// loading the other buffers. This would overwrite the already
		// existing fileinfo for the first tab.
		اگر (i == 1)
		{
		    نویسه buf[100];

		    p_shm_save = vim_strsave(p_shm);
		    vim_snprintf(buf, 100, "F%s", p_shm);
		    set_option_مقدار_give_err((char_u *)"shm",
							 0L, (char_u *)buf, 0);
		}
	    }
	    وگرنه
	    {
		اگر (curwin->w_next == پوچ)	// just checking
		    بشکن;
		win_enter(curwin->w_next, FALSE);
	    }
	}
	advance = TRUE;

	// Only open the file if there is no file in this window yet (that can
	// happen when .vimrc contains ":sall").
	اگر (curbuf == firstwin->w_buffer || curbuf->b_ffname == پوچ)
	{
	    curwin->w_arg_idx = arg_idx;
	    // Edit file from arg list, if there is one.  When "Quit" selected
	    // at the ATTENTION prompt close the window.
	    swap_exists_did_quit = FALSE;
	    (تهی)do_ecmd(0, arg_idx < GARGCOUNT
			  ? alist_name(&GARGLIST[arg_idx]) : پوچ,
			  پوچ, پوچ, ECMD_LASTL, ECMD_HIDE, curwin);
	    اگر (swap_exists_did_quit)
	    {
		// abort or quit selected
		اگر (got_int || only_one_window())
		{
		    // abort selected and only one window
		    did_emsg = FALSE;   // avoid hit-enter prompt
		    getout(1);
		}
		win_close(curwin, TRUE);
		advance = FALSE;
	    }
	    اگر (arg_idx == GARGCOUNT - 1)
		arg_had_last = TRUE;
	    ++arg_idx;
	}
	ui_breakcheck();
	اگر (got_int)
	{
	    (تهی)vgetc();	// only break the file loading, not the rest
	    بشکن;
	}
    }

    اگر (p_shm_save != پوچ)
    {
	set_option_مقدار_give_err((char_u *)"shm", 0L, p_shm_save, 0);
	vim_free(p_shm_save);
    }

    اگر (parmp->window_layout == WIN_TABS)
	goto_tabpage(1);
    --autocmd_no_enter;

    // make the first window the current window
    win = firstwin;
# اگر defined(FEAT_QUICKFIX)
    // Avoid making a preview window the current window.
    وقتی (win->w_p_pvw)
    {
	win = win->w_next;
	اگر (win == پوچ)
	{
	    win = firstwin;
	    بشکن;
	}
    }
# endif
    win_enter(win, FALSE);

    --autocmd_no_leave;
    TIME_پیام("editing files in windows");
    اگر (parmp->window_شمار > 1 && parmp->window_layout != WIN_TABS)
	win_equal(curwin, FALSE, 'b');	// adjust heights
}

/*
 * Execute the commands from --cmd arguments "cmds[cnt]".
 */
    ایستا تهی
exe_pre_commands(mparm_T *parmp)
{
    char_u	**cmds = parmp->pre_commands;
    صحیح		cnt = parmp->n_pre_commands;
    صحیح		i;
    ESTACK_CHECK_DECLARATION;

    اگر (cnt <= 0)
	بازگشت;

    curwin->w_cursor.lnum = 0; // just in case..
    estack_افزودن(ETYPE_ARGS, (char_u *)_("pre-vimrc command line"), 0);
    ESTACK_CHECK_SETUP;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CMDARG;
# endif
    برای (i = 0; i < cnt; ++i)
	do_cmdline_cmd(cmds[i]);
    ESTACK_CHECK_NOW;
    estack_برداشت();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    TIME_پیام("--cmd commands");
}

/*
 * Execute "+", "-c" and "-S" arguments.
 */
    ایستا تهی
exe_commands(mparm_T *parmp)
{
    صحیح		i;
    ESTACK_CHECK_DECLARATION;

    /*
     * We start commands on line 0, make "vim +/pat file" match a
     * pattern on line 1.  But don't move the cursor when an autocommand
     * with g`" was used.
     */
    پیام_scroll = TRUE;
    اگر (parmp->tagname == پوچ && curwin->w_cursor.lnum <= 1)
	curwin->w_cursor.lnum = 0;
    estack_افزودن(ETYPE_ARGS, (char_u *)"command line", 0);
    ESTACK_CHECK_SETUP;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CARG;
    current_sctx.sc_seq = 0;
# endif
    برای (i = 0; i < parmp->n_commands; ++i)
    {
	do_cmdline_cmd(parmp->commands[i]);
	اگر (parmp->cmds_tofree[i])
	    vim_free(parmp->commands[i]);
    }
    ESTACK_CHECK_NOW;
    estack_برداشت();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    اگر (curwin->w_cursor.lnum == 0)
	curwin->w_cursor.lnum = 1;

    اگر (!exmode_active)
	پیام_scroll = FALSE;

# ifdef FEAT_QUICKFIX
    // When started with "-q errorfile" jump to first error again.
    اگر (parmp->edit_type == EDIT_QF)
	qf_jump(پوچ, 0, 0, FALSE);
# endif
    TIME_پیام("executing command arguments");
}

/*
 * Source startup scripts.
 */
    ایستا تهی
source_startup_scripts(mparm_T *parmp)
{
    صحیح		i;

    /*
     * For "evim" source evim.vim first of all, so that the user can overrule
     * any things he doesn't like.
     */
    اگر (parmp->evim_mode)
    {
	(تهی)do_source((char_u *)EVIM_FILE, FALSE, DOSO_NONE, پوچ);
	TIME_پیام("source evim file");
    }

    /*
     * If -u argument given, use only the initializations from that file and
     * nothing else.
     */
    اگر (parmp->use_vimrc != پوچ)
    {
	اگر (STRCMP(parmp->use_vimrc, "DEFAULTS") == 0)
	{
	    اگر (do_source((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE, پوچ)
									 != OK)
		emsg(_(e_failed_to_source_defaults));
	}
	وگرنه اگر (STRCMP(parmp->use_vimrc, "NONE") == 0
				     || STRCMP(parmp->use_vimrc, "NORC") == 0)
	{
# ifdef FEAT_GUI
	    اگر (use_gvimrc == پوچ)	    // don't load gvimrc either
		use_gvimrc = parmp->use_vimrc;
# endif
	}
	وگرنه
	{
	    اگر (do_source(parmp->use_vimrc, FALSE, DOSO_NONE, پوچ) != OK)
		semsg(_(e_cannot_read_from_str_2), parmp->use_vimrc);
	}
    }
    وگرنه اگر (!silent_mode)
    {
# ifdef AMIGA
	ساختار Process	*proc = (ساختار Process *)FindTask(0L);
	APTR		save_winptr = proc->pr_WindowPtr;

	// Avoid a requester here for a volume that doesn't exist.
	proc->pr_WindowPtr = (APTR)-1L;
# endif

	/*
	 * Get system wide defaults, if the file name is defined.
	 */
# ifdef SYS_VIMRC_FILE
	(تهی)do_source((char_u *)SYS_VIMRC_FILE, FALSE, DOSO_NONE, پوچ);
# endif
# ifdef MACOS_X
	(تهی)do_source((char_u *)"$VIMRUNTIME/macmap.vim", FALSE,
							      DOSO_NONE, پوچ);
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
	    اگر (do_source((char_u *)USR_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, پوچ) == FAIL
# ifdef USR_VIMRC_FILE2
		&& do_source((char_u *)USR_VIMRC_FILE2, TRUE,
						      DOSO_VIMRC, پوچ) == FAIL
# endif
# ifdef XDG_VIMRC_FILE
		&& do_source((char_u *)XDG_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, پوچ) == FAIL
# endif
# ifdef USR_VIMRC_FILE3
		&& do_source((char_u *)USR_VIMRC_FILE3, TRUE,
						      DOSO_VIMRC, پوچ) == FAIL
# endif
# ifdef USR_VIMRC_FILE4
		&& do_source((char_u *)USR_VIMRC_FILE4, TRUE,
						      DOSO_VIMRC, پوچ) == FAIL
# endif
		&& process_env((char_u *)"EXINIT", FALSE) == FAIL
		&& do_source((char_u *)USR_EXRC_FILE, FALSE,
						       DOSO_NONE, پوچ) == FAIL
# ifdef USR_EXRC_FILE2
		&& do_source((char_u *)USR_EXRC_FILE2, FALSE,
						       DOSO_NONE, پوچ) == FAIL
# endif
		&& !has_dash_c_arg)
	    {
		// When no .vimrc file was found: source defaults.vim.
		اگر (do_source((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE,
								 پوچ) == FAIL)
		    emsg(_(e_failed_to_source_defaults));
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
		i = do_source((char_u *)VIMRC_FILE, TRUE, DOSO_VIMRC, پوچ);

	    اگر (i == FAIL)
	    {
# اگر defined(UNIX) || defined(VMS)
		// if ".exrc" is not owned by user set 'secure' mode
		اگر (!file_owned(EXRC_FILE))
		    secure = p_secure;
		وگرنه
		    secure = 0;
# endif
		اگر (	   fullpathcmp((char_u *)USR_EXRC_FILE,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# ifdef USR_EXRC_FILE2
			&& fullpathcmp((char_u *)USR_EXRC_FILE2,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# endif
				)
		    (تهی)do_source((char_u *)EXRC_FILE, FALSE,
							      DOSO_NONE, پوچ);
	    }
	}
	اگر (secure == 2)
	    need_wait_return = TRUE;
	secure = 0;
# ifdef AMIGA
	proc->pr_WindowPtr = save_winptr;
# endif
    }
    TIME_پیام("sourcing vimrc file(s)");
}

/*
 * Setup to start using the GUI.  Exit with an error when not available.
 */
    ایستا تهی
main_start_gui(تهی)
{
# ifdef FEAT_GUI
    gui.starting = true;	// start GUI a bit later
# وگرنه
    mch_errmsg(_(e_gui_cannot_be_used_not_enabled_at_compile_time));
    mch_errmsg("\n");
    mch_exit(2);
# endif
}

#endif  // NO_VIM_MAIN

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

    اگر ((initstr = mch_getenv(env)) == پوچ || *initstr == NUL)
	بازگشت FAIL;

    اگر (is_viminit)
	vimrc_found(پوچ, پوچ);
    estack_افزودن(ETYPE_ENV, env, 0);
    ESTACK_CHECK_SETUP;
    save_current_sctx = current_sctx;
    current_sctx.sc_version = 1;
#ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_ENV;
    current_sctx.sc_seq = 0;
    current_sctx.sc_lnum = 0;
#endif

    do_cmdline_cmd(initstr);

    ESTACK_CHECK_NOW;
    estack_برداشت();
    current_sctx = save_current_sctx;
    بازگشت OK;
}

#اگر (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
/*
 * Return TRUE if we are certain the user owns the file "fname".
 * Used for ".vimrc" and ".exrc".
 * Use both stat() and lstat() for extra security.
 */
    ایستا صحیح
file_owned(نویسه *fname)
{
    stat_T	s;
# ifdef UNIX
    uid_t	uid = getuid();
# وگرنه	 // VMS
    uid_t	uid = ((getgid() << 16) | getuid());
# endif

    بازگشت !(mch_stat(fname, &s) != 0 || s.st_uid != uid
# ifdef HAVE_LSTAT
	    || mch_lstat(fname, &s) != 0 || s.st_uid != uid
# endif
	    );
}
#endif

/*
 * Give an error message main_errors["n"] and exit.
 */
    ایستا تهی
mainerr(
    صحیح		n,	// one of the ME_ defines
    char_u	*str)	// extra argument or NULL
{
#اگر defined(UNIX) || defined(VMS)
    reset_signals();		// kill us with CTRL-C here, if you like
#endif

    // If this is a Windows GUI executable, show an error dialog box.
#ifdef VIMDLL
    gui.in_use = mch_is_gui_executable();
#endif
#ifdef FEAT_GUI_MSWIN
    gui.starting = false;   // Needed to show as error.
#endif

    init_longVersion();
    mch_errmsg(longVersion);
    mch_errmsg("\n");
    mch_errmsg(_(main_errors[n]));
    اگر (str != پوچ)
    {
	mch_errmsg(": \"");
	mch_errmsg((نویسه *)str);
	mch_errmsg("\"");
    }
    mch_errmsg(_("\nMore info with: \"vim -h\"\n"));

    mch_exit(1);
}

    تهی
mainerr_arg_missing(char_u *str)
{
    mainerr(ME_ARG_MISSING, str);
}

#ifndef NO_VIM_MAIN
/*
 * print a message with three spaces prepended and '\n' appended.
 */
    ایستا تهی
main_پیام(نویسه *s)
{
    mch_پیام("   ");
    mch_پیام(s);
    mch_پیام("\n");
}

/*
 * Print messages for "vim -h" or "vim --help" and exit.
 */
    ایستا تهی
usage(تهی)
{
    صحیح		i;
    ایستا نویسه	*(use[]) =
    {
	N_("[file ..]       edit specified file(s)"),
	N_("-               read text from stdin"),
	N_("-t tag          edit file where tag is defined"),
# ifdef FEAT_QUICKFIX
	N_("-q [errorfile]  edit file with first error")
# endif
    };

# اگر defined(UNIX) || defined(VMS)
    reset_signals();		// kill us with CTRL-C here, if you like
# endif

    init_longVersion();
    mch_پیام(longVersion);
    mch_پیام(_("\n\nUsage:"));
    برای (i = 0; ; ++i)
    {
	mch_پیام(_(" vim [arguments] "));
	mch_پیام(_(use[i]));
	اگر (i == ARRAY_LENGTH(use) - 1)
	    بشکن;
	mch_پیام(_("\n   or:"));
    }
# اگر defined( VMS)
    mch_پیام(_("\nWhere command is down-cased, prepend / (like: -/R) to treat flag as upper-case."));
    mch_پیام(_("\nOr, where supported, SET PROC/PARSE=EXT, or else quote upper-case material."));
# endif /* defined( VMS) */

    mch_پیام(_("\n\nArguments:\n"));
    main_پیام(_("--\t\t\tOnly file names after this"));
# ifdef EXPAND_FILENAMES
    main_پیام(_("--literal\t\tDon't expand wildcards"));
# endif
# ifdef FEAT_OLE
    main_پیام(_("-register\t\tRegister this gvim for OLE"));
    main_پیام(_("-unregister\t\tUnregister gvim for OLE"));
# endif
# ifdef FEAT_GUI
    main_پیام(_("-g\t\t\tRun using GUI (like \"gvim\")"));
    main_پیام(_("-f  or  --nofork\tForeground: Don't fork when starting GUI"));
# endif
    main_پیام(_("-v\t\t\tVi mode (like \"vi\")"));
    main_پیام(_("-e\t\t\tEx mode (like \"ex\")"));
    main_پیام(_("-E\t\t\tImproved Ex mode"));
    main_پیام(_("-s\t\t\tSilent (batch) mode (only for \"ex\")"));
# ifdef FEAT_DIFF
    main_پیام(_("-d\t\t\tDiff mode (like \"vimdiff\")"));
# endif
    main_پیام(_("-y\t\t\tEasy mode (like \"evim\", modeless)"));
    main_پیام(_("-R\t\t\tReadonly mode (like \"view\")"));
    main_پیام(_("-Z\t\t\tRestricted mode (like \"rvim\")"));
    main_پیام(_("-m\t\t\tModifications (writing files) not allowed"));
    main_پیام(_("-M\t\t\tModifications in text not allowed"));
    main_پیام(_("-b\t\t\tBinary mode"));
    main_پیام(_("-l\t\t\tLisp mode"));
    main_پیام(_("-C\t\t\tCompatible with Vi: 'compatible'"));
    main_پیام(_("-N\t\t\tNot fully Vi compatible: 'nocompatible'"));
    main_پیام(_("-V[N][fname]\t\tBe verbose [level N] [log messages to fname]"));
# ifdef FEAT_EVAL
    main_پیام(_("-D\t\t\tDebugging mode"));
# endif
    main_پیام(_("-n\t\t\tNo swap file, use memory only"));
    main_پیام(_("-r\t\t\tList swap files and exit"));
    main_پیام(_("-r (with file name)\tRecover crashed session"));
    main_پیام(_("-L\t\t\tSame as -r"));
# ifdef AMIGA
    main_پیام(_("-f\t\t\tDon't use newcli to open window"));
    main_پیام(_("-dev <device>\t\tUse <device> for I/O"));
# endif
# ifdef FEAT_ARABIC
    main_پیام(_("-A\t\t\tStart in Arabic mode"));
# endif
# ifdef FEAT_RIGHTLEFT
    main_پیام(_("-H\t\t\tStart in Hebrew mode"));
# endif
    main_پیام(_("-T <terminal>\tSet terminal type to <terminal>"));
    main_پیام(_("--not-a-term\t\tSkip warning for input/output not being a terminal"));
# ifdef FEAT_GUI
    main_پیام(_("--gui-dialog-file {fname}  For testing: write dialog text"));
# endif
    main_پیام(_("--ttyfail\t\tExit if input or output is not a terminal"));
# ifdef FEAT_JOB_CHANNEL
    main_پیام(_("--stdio-channel\tUse stdin and stdout as a channel"));
# endif
    main_پیام(_("-u <vimrc>\t\tUse <vimrc> instead of any .vimrc"));
# ifdef FEAT_GUI
    main_پیام(_("-U <gvimrc>\t\tUse <gvimrc> instead of any .gvimrc"));
# endif
    main_پیام(_("--noplugin\t\tDon't load plugin scripts"));
    main_پیام(_("-p[N]\t\tOpen N tab pages (default: one for each file)"));
    main_پیام(_("-o[N]\t\tOpen N windows (default: one for each file)"));
    main_پیام(_("-O[N]\t\tLike -o but split vertically"));
    main_پیام(_("+\t\t\tStart at end of file"));
    main_پیام(_("+<lnum>\t\tStart at line <lnum>"));
    main_پیام(_("--cmd <command>\tExecute <command> before loading any vimrc file"));
    main_پیام(_("-c <command>\t\tExecute <command> after loading the first file"));
    main_پیام(_("-S <session>\t\tSource file <session> after loading the first file"));
    main_پیام(_("-s <scriptin>\tRead Normal mode commands from file <scriptin>"));
    main_پیام(_("-w <scriptout>\tAppend all typed commands to file <scriptout>"));
    main_پیام(_("-W <scriptout>\tWrite all typed commands to file <scriptout>"));
# ifdef FEAT_CRYPT
    main_پیام(_("-x\t\t\tEdit encrypted files"));
# endif
# اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
#  اگر defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
    main_پیام(_("-display <display>\tConnect Vim to this particular X-server"));
#  endif
    main_پیام(_("-X\t\t\tDo not connect to X server"));
# endif
# اگر defined(FEAT_WAYLAND)
    main_پیام(_("-Y\t\t\tDo not connect to Wayland compositor"));
# endif
# ifdef FEAT_CLIENTSERVER
#  ifdef FEAT_CLIENTSERVER_BACKENDS
    main_پیام(_("--clientserver <socket|x11|mswin> Backend for clientserver communication"));
#  endif
    main_پیام(_("--remote <files>\tEdit <files> in a Vim server if possible"));
    main_پیام(_("--remote-silent <files>  Same, don't complain if there is no server"));
    main_پیام(_("--remote-wait <files>  As --remote but wait for files to have been edited"));
    main_پیام(_("--remote-wait-silent <files>  Same, don't complain if there is no server"));
    main_پیام(_("--remote-tab[-wait][-silent] <files>  As --remote but use tab page per file"));
    main_پیام(_("--remote-send <keys>\tSend <keys> to a Vim server and exit"));
    main_پیام(_("--remote-expr <expr>\tEvaluate <expr> in a Vim server and print result"));
    main_پیام(_("--serverlist\t\tList available Vim server names and exit"));
    main_پیام(_("--servername <name>\tSend to/become the Vim server <name>"));
# endif
# ifdef STARTUPTIME
    main_پیام(_("--startuptime <file>\tWrite startup timing messages to <file>"));
# endif
# ifdef FEAT_JOB_CHANNEL
    main_پیام(_("--log <file>\t\tStart logging to <file> early"));
# endif
# ifdef FEAT_VIMINFO
    main_پیام(_("-i <viminfo>\t\tUse <viminfo> instead of .viminfo"));
# endif
    main_پیام(_("--clean\t\t'nocompatible', Vim defaults, no plugins, no viminfo"));
    main_پیام(_("-h  or  --help\tPrint Help (this message) and exit"));
    main_پیام(_("--version\t\tPrint version information and exit"));

# ifdef FEAT_GUI_X11
#  ifdef FEAT_GUI_MOTIF
    mch_پیام(_("\nArguments recognised by gvim (Motif version):\n"));
#  endif
    main_پیام(_("-display <display>\tRun Vim on <display>"));
    main_پیام(_("-iconic\t\tStart Vim iconified"));
    main_پیام(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_پیام(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_پیام(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_پیام(_("-boldfont <font>\tUse <font> for bold text"));
    main_پیام(_("-italicfont <font>\tUse <font> for italic text"));
    main_پیام(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
    main_پیام(_("-borderwidth <width>\tUse a border width of <width> (also: -bw)"));
    main_پیام(_("-scrollbarwidth <width>  Use a scrollbar width of <width> (also: -sw)"));
    main_پیام(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_پیام(_("+reverse\t\tDon't use reverse video (also: +rv)"));
    main_پیام(_("-xrm <resource>\tSet the specified resource"));
# endif // FEAT_GUI_X11
# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
    mch_پیام(_("\nArguments recognised by gvim (GTK4 version):\n"));
#  وگرنه
    mch_پیام(_("\nArguments recognised by gvim (GTK+ version):\n"));
#  endif
    main_پیام(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_پیام(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_پیام(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_پیام(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
#  ifdef USE_GTK4
    main_پیام("--prg-name <name>\tSet GTK program name");
#  وگرنه
    main_پیام(_("-iconic\t\tStart Vim iconified"));
    main_پیام(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_پیام(_("-display <display>\tRun Vim on <display> (also: --display)"));
    main_پیام(_("--role <role>\tSet a unique role to identify the main window"));
    main_پیام(_("--socketid <xid>\tOpen Vim inside another GTK widget"));
    main_پیام(_("--echo-wid\t\tMake gvim echo the Window ID on stdout"));
#  endif
# endif
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
    اگر (gui.starting)
#  endif
    {
	main_پیام(_("-P <parent title>\tOpen Vim inside parent application"));
	main_پیام(_("--windowid <HWND>\tOpen Vim inside another win32 widget"));
    }
# endif

# ifdef FEAT_GUI_GNOME
    // Gnome gives extra messages for --help if we continue, but not for -h.
    اگر (gui.starting)
    {
	mch_پیام("\n");
	gui.dofork = false;
    }
    وگرنه
# endif
	mch_exit(0);
}

/*
 * Check the result of the ATTENTION dialog:
 * When "Quit" selected, exit Vim.
 * When "Recover" selected, recover the file.
 */
    ایستا تهی
check_swap_exists_action(تهی)
{
    اگر (swap_exists_action == SEA_QUIT)
	getout(1);
    handle_swap_exists(پوچ);
}

#endif // NO_VIM_MAIN

#اگر !defined(NO_VIM_MAIN) && defined(FEAT_EVAL)
    ایستا تهی
set_progpath(char_u *argv0)
{
    char_u *val = argv0;

# ifdef MSWIN
    // A relative path containing a "/" will become invalid when using ":cd",
    // turn it into a full path.
    // On MS-Windows "vim" should be expanded to "vim.exe", thus always do
    // this.
    char_u *path = پوچ;

    اگر (mch_can_exe(argv0, &path, FALSE) && path != پوچ)
	val = path;
# وگرنه
    char_u	buf[MAXPATHL + 1];
#  ifdef PROC_EXE_LINK
    نویسه	linkbuf[MAXPATHL + 1];
    ssize_t	len;

    len = readlink(PROC_EXE_LINK, linkbuf, MAXPATHL);
    اگر (len > 0)
    {
	linkbuf[len] = NUL;
	val = (char_u *)linkbuf;
    }
#  endif

    اگر (!mch_isFullName(val))
    {
	اگر (gettail(val) != val
			   && vim_FullName(val, buf, MAXPATHL, TRUE) != FAIL)
	    val = buf;
    }
# endif

    set_vim_var_string(VV_PROGPATH, val, -1);

# ifdef MSWIN
    vim_free(path);
# endif
}

#endif // NO_VIM_MAIN
