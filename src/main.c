/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#define EXTERN
#அந்தர்பாவ "vim.h"

#ifdef __CYGWIN__
# include <cygwin/version.h>
# include <sys/cygwin.h>	// for cygwin_conv_to_posix_path() and/or
				// cygwin_conv_path()
# include <limits.h>
#endif

#எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# include "iscygpty.h"
#endif

// Values for edit_type.
#define EDIT_NONE   0	    // no edit type yet
#define EDIT_FILE   1	    // file name argument[s] given, use argument list
#define EDIT_STDIN  2	    // read file from stdin
#define EDIT_TAG    3	    // tag name argument given, use tagname
#define EDIT_QF	    4	    // start in quickfix mode

#எனில் (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
நிலையான முழு file_owned(எழுத்து *fname);
#endif
நிலையான வெறுமை mainerr(முழு, char_u *);
நிலையான வெறுமை early_arg_scan(mparm_T *parmp);
#ifndef NO_VIM_MAIN
நிலையான வெறுமை usage(வெறுமை);
நிலையான வெறுமை parse_command_name(mparm_T *parmp);
நிலையான வெறுமை command_வரி_scan(mparm_T *parmp);
நிலையான வெறுமை check_tty(mparm_T *parmp);
நிலையான வெறுமை read_stdin(வெறுமை);
நிலையான வெறுமை create_windows(mparm_T *parmp);
நிலையான வெறுமை edit_buffers(mparm_T *parmp, char_u *cwd);
நிலையான வெறுமை exe_pre_commands(mparm_T *parmp);
நிலையான வெறுமை exe_commands(mparm_T *parmp);
நிலையான வெறுமை source_startup_scripts(mparm_T *parmp);
நிலையான வெறுமை main_start_gui(வெறுமை);
நிலையான வெறுமை check_swap_exists_action(வெறுமை);
# ifdef FEAT_EVAL
நிலையான வெறுமை set_progpath(char_u *argv0);
# endif
#endif


/*
 * Different types of error messages.
 */
நிலையான எழுத்து *(main_errors[]) =
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
நிலையான mparm_T	params;

# ifdef _IOLBF
நிலையான வெறுமை *s_vbuf = வெற்று;		// buffer for setvbuf()
# endif

# ifndef NO_VIM_MAIN	// skip this for unittests

நிலையான char_u *start_dir = வெற்று;	// current working dir on startup

நிலையான முழு has_dash_c_arg = FALSE;

#  ifdef VIMDLL
__declspec(dllexport)
#  endif
    முழு
#  ifdef MSWIN
VimMain
#  இல்லையெனில்
முதன்மை
#  endif
(முழு argc, எழுத்து **argv)
{
#  எனில் defined(STARTUPTIME) || defined(CLEAN_RUNTIMEPATH)
    முழு		i;
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
    params.window_மொத்தம் = -1;

    autocmd_init();

#  ifdef FEAT_RUBY
    {
	முழு ruby_அடுக்கு_start;
	vim_ruby_init((வெறுமை *)&ruby_அடுக்கு_start);
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

#  எனில் defined(STARTUPTIME) || defined(FEAT_JOB_CHANNEL)
    // Need to find "--startuptime" and "--log" before actually parsing
    // arguments.
    க்கு (i = 1; i < argc - 1; ++i)
    {
#   ifdef STARTUPTIME
	எனில் (STRICMP(argv[i], "--startuptime") == 0 && time_fd == வெற்று)
	{
	    time_fd = mch_fopen(argv[i + 1], "a");
	    TIME_செய்தி("--- VIM STARTING ---");
	}
#   endif
#   ifdef FEAT_EVAL
	எனில் (STRICMP(argv[i], "--log") == 0)
	    ch_logfile((char_u *)(argv[i + 1]), (char_u *)"ao");
#   endif
    }
#  endif

#  ifdef CLEAN_RUNTIMEPATH
    // Need to find "--clean" before actually parsing arguments.
    க்கு (i = 1; i < argc; ++i)
	எனில் (STRICMP(argv[i], "--clean") == 0)
	{
	    params.clean = TRUE;
	    நிறுத்து;
	}
#  endif
#  ifdef MSWIN
    // Need to find "-register" and "-unregister" before loading any libraries.
    க்கு (i = 1; i < argc; ++i)
	எனில் ((STRICMP(argv[i] + 1, "register") == 0
				    || STRICMP(argv[i] + 1, "unregister") == 0)
		&& (argv[i][0] == '-' || argv[i][0] == '/'))
	{
	    found_register_arg = TRUE;
	    நிறுத்து;
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
    command_வரி_scan(&params);
    TIME_செய்தி("parsing arguments");

    /*
     * On some systems, when we compile with the GUI, we always use it.  On Mac
     * there is no terminal version, and on Windows we can't fork one off with
     * :gui.
     */
#  ifdef ALWAYS_USE_GUI
    gui.starting = true;
#  இல்லையெனில்
#   எனில் defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    /*
     * Check if the GUI can be started.  Reset gui.starting if not.
     * Don't know about other systems, stay on the safe side and don't check.
     */
    எனில் (gui.starting)
    {
	எனில் (gui_init_check() == FAIL)
	{
	    gui.starting = false;

	    // When running "evim" or "gvim -y" we need the menus, exit if we
	    // don't have them.
	    எனில் (params.evim_mode)
		mch_exit(1);
	}
    }
#   endif
#  endif

    எனில் (GARGCOUNT > 0)
    {
#  ifdef EXPAND_FILENAMES
	/*
	 * Expand wildcards in file names.
	 */
	எனில் (!params.literal)
	{
	    start_dir = alloc(MAXPATHL);
	    எனில் (start_dir != வெற்று)
		mch_dirname(start_dir, MAXPATHL);
	    // Temporarily add '(' and ')' to 'isfname'.  These are valid
	    // filename characters but are excluded from 'isfname' to make
	    // "gf" work on a file name in parentheses (e.g.: see vim.h).
	    do_cmdline_cmd((char_u *)":set isf+=(,)");
	    alist_expand(வெற்று, 0);
	    do_cmdline_cmd((char_u *)":set isf&");
	    எனில் (start_dir != வெற்று)
		mch_chdir((எழுத்து *)start_dir);
	}
#  endif
	params.fname = alist_name(&GARGLIST[0]);
    }

#  ifdef MSWIN
    {
	// Remember the number of entries in the argument list.  If it changes
	// we don't react on setting 'encoding'.
	set_alist_மொத்தம்();
    }
#  endif

#  ifdef MSWIN
    எனில் (GARGCOUNT == 1 && params.full_path)
    {
	/*
	 * If there is one filename, fully qualified, we have very probably
	 * been invoked from explorer, so change to the file's directory.
	 * Hint: to avoid this when typing a command use a forward slash.
	 * If the cd fails, it doesn't matter.
	 */
	எனில் (vim_chdirfile(params.fname, "drop") == OK)
	    last_chdir_reason = "drop";
	எனில் (start_dir != வெற்று)
	    mch_dirname(start_dir, MAXPATHL);
    }
#  endif
    TIME_செய்தி("expanding arguments");

#  ifdef FEAT_DIFF
    எனில் (params.diff_mode && params.window_மொத்தம் == -1)
	params.window_மொத்தம் = 0;	// open up to 3 windows
#  endif

    // Don't redraw until much later.
    ++RedrawingDisabled;

    /*
     * When listing swap file names, don't do cursor positioning et. al.
     */
    எனில் (recoverymode && params.fname == வெற்று)
	params.want_full_screen = FALSE;

    /*
     * When certain to start the GUI, don't check terminal capabilities.
     * For GTK we can't be sure, but when started from the desktop it doesn't
     * make sense to try using a terminal.
     */
#  எனில் defined(ALWAYS_USE_GUI) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	|| defined(VIMDLL)
    எனில் (gui.starting
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
    TIME_செய்தி("shell init");

#  ifdef USE_XSMP
    /*
     * For want of anywhere else to do it, try to connect to xsmp here.
     * Fitting it in after gui_mch_init, but before gui_init (via termcapinit).
     * Hijacking -X 'no X connection' to also disable XSMP connection as that
     * has a similar delay upon failure.
     * Only try if SESSION_MANAGER is set to something non-null.
     */
    எனில் (!x_no_connect)
    {
	எழுத்து *p = getenv("SESSION_MANAGER");

	எனில் (p != வெற்று && *p != NUL)
	{
	    xsmp_init();
	    TIME_செய்தி("xsmp init");
	}
    }
#  endif

    /*
     * Print a warning if stdout is not a terminal.
     */
    check_tty(&params);

#  ifdef _IOLBF
    எனில் (silent_mode)
    {
	// Ensure output works usefully without a tty: buffer lines instead of
	// fully buffered.
	s_vbuf = malloc(BUFSIZ);
	எனில் (s_vbuf != வெற்று)
	    setvbuf(stdout, s_vbuf, _IOLBF, BUFSIZ);
    }
#  endif

    // This message comes before term inits, but after setting "silent_mode"
    // when the input is not a tty. Omit the message with --not-a-term.
    எனில் (GARGCOUNT > 1 && !silent_mode && !is_not_a_term())
	printf(_("%d files to edit\n"), GARGCOUNT);

    எனில் (params.want_full_screen && !silent_mode)
    {
	termcapinit(params.term);	// set terminal name and get terminal
				   // capabilities (will set full_screen)
	screen_start();		// don't know where cursor is now
	TIME_செய்தி("Termcap init");
    }

    /*
     * Set the default values for the options that use Rows and Columns.
     */
    ui_get_shellsize();		// inits Rows and Columns
    win_init_size();
#  ifdef FEAT_DIFF
    // Set the 'diff' option now, so that it can be checked for in a .vimrc
    // file.  There is no buffer yet though.
    எனில் (params.diff_mode)
	diff_win_options(firstwin, FALSE);
#  endif

    cmdline_row = Rows - p_ch;
    cmdline_col_off = 0;
    cmdline_width = (முழு)Columns;
    செய்தி_row = cmdline_row;
    செய்தி_col = 0;
    screenalloc(FALSE);		// allocate screen buffers
    set_init_2();
    TIME_செய்தி("inits 2");

    செய்தி_scroll = TRUE;
    no_wait_return = TRUE;

    init_mappings();		// set up initial mappings

    init_highlight(TRUE, FALSE); // set the default highlight groups
    TIME_செய்தி("init highlight");

#  எனில் defined(FEAT_TERMRESPONSE)
    init_term_props(TRUE);
#  endif

#  ifdef FEAT_EVAL
    // Set the break level after the terminal is initialized.
    debug_break_level = params.use_debug_break_level;
#  endif

    // Reset 'loadplugins' for "-u NONE" before "--cmd" arguments.
    // Allows for setting 'loadplugins' there.
    எனில் (params.use_vimrc != வெற்று
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
    திரும்பு mzscheme_main();
#  இல்லையெனில்
    திரும்பு vim_main2();
#  endif
}
# endif // NO_VIM_MAIN
#endif // PROTO

#எனில் defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(FEAT_CLIPBOARD)
/*
 * Restore the state after a fatal X error.
 */
    நிலையான வெறுமை
x_restore_state(வெறுமை)
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
    முழு
vim_main2(வெறுமை)
{
#ifndef NO_VIM_MAIN
# ifdef FEAT_EVAL
    /*
     * Read all the plugin files.
     * Only when compiled with +eval, since most plugins need it.
     */
    எனில் (p_lpl)
    {
	char_u *rtp_copy = வெற்று;
	char_u *plugin_pattern = (char_u *)
#  எனில் defined(VMS) || defined(AMIGA) // VMS and Amiga don't handle the "**".
		"plugin/*.vim"
#  இல்லையெனில்
		"plugin/**/*.vim"
#  endif
		;

	// First add all package directories to 'runtimepath', so that their
	// autoload directories can be found.  Only if not done already with a
	// :packloadall command.
	// Make a copy of 'runtimepath', so that source_runtime does not use
	// the pack directories.
	எனில் (!did_source_packages)
	{
	    rtp_copy = vim_strsave(p_rtp);
	    add_pack_start_dirs();
	}

	source_in_path(rtp_copy == வெற்று ? p_rtp : rtp_copy, plugin_pattern,
		DIP_ALL | DIP_NOAFTER, வெற்று);
	TIME_செய்தி("loading plugins");
	vim_free(rtp_copy);

	// Only source "start" packages if not done already with a :packloadall
	// command.
	எனில் (!did_source_packages)
	    load_start_packages();
	TIME_செய்தி("loading packages");

	source_runtime(plugin_pattern, DIP_ALL | DIP_AFTER);
	TIME_செய்தி("loading after plugins");
    }
# endif

# ifdef FEAT_DIFF
    // Decide about window layout for diff mode after reading vimrc.
    எனில் (params.diff_mode && params.window_layout == 0)
    {
	எனில் (diffopt_horizontal())
	    params.window_layout = WIN_HOR;	// use horizontal split
	இல்லையெனில்
	    params.window_layout = WIN_VER;	// use vertical split
    }
# endif

    /*
     * Recovery mode without a file name: List swap files.
     * This uses the 'dir' option, therefore it must be after the
     * initializations.
     */
    எனில் (recoverymode && params.fname == வெற்று)
    {
	recover_names(வெற்று, TRUE, வெற்று, 0, வெற்று);
	mch_exit(0);
    }

    /*
     * Set a few option defaults after reading .vimrc files:
     * 'title' and 'icon', Unix: 'shellpipe' and 'shellredir'.
     */
    set_init_3();
    TIME_செய்தி("inits 3");

    /*
     * "-n" argument: Disable swap file by setting 'updatecount' to 0.
     * Note that this overrides anything from a vimrc file.
     */
    எனில் (params.no_swap_file)
	p_uc = 0;

# ifdef FEAT_GUI
    எனில் (gui.starting)
    {
#  எனில் defined(UNIX) || defined(VMS)
	// When something caused a message from a vimrc script, need to output
	// an extra newline before the shell prompt.
	எனில் (did_emsg || செய்தி_didout)
	    putchar('\n');
#  endif

	gui_start(வெற்று);		// will set full_screen to TRUE
	TIME_செய்தி("starting GUI");

	// When running "evim" or "gvim -y" we need the menus, exit if we
	// don't have them.
	எனில் (!gui.in_use && params.evim_mode)
	    mch_exit(1);
	firstwin->w_prev_height = firstwin->w_height; // may have changed
    }
# endif

# ifdef FEAT_VIMINFO
    /*
     * Read in registers, history etc, but not marks, from the viminfo file.
     * This is where v:oldfiles gets filled.
     */
    எனில் (*p_viminfo != NUL)
    {
	read_viminfo(வெற்று, VIF_WANT_INFO | VIF_GET_OLDFILES);
	TIME_செய்தி("reading viminfo");
    }
# endif
# ifdef FEAT_EVAL
    // It's better to make v:oldfiles an empty list than NULL.
    எனில் (get_vim_var_list(VV_OLDFILES) == வெற்று)
	set_vim_var_list(VV_OLDFILES, list_alloc());
# endif

# ifdef FEAT_QUICKFIX
    /*
     * "-q errorfile": Load the error file now.
     * If the error file can't be read, exit before doing anything else.
     */
    எனில் (params.edit_type == EDIT_QF)
    {
	char_u	*enc = வெற்று;

	enc = p_menc;
	எனில் (params.use_ef != வெற்று)
	    set_string_option_direct((char_u *)"ef", -1,
					   params.use_ef, OPT_FREE, SID_CARG);
	vim_snprintf((எழுத்து *)IObuff, IOSIZE, "cfile %s", p_ef);
	எனில் (qf_init(வெற்று, p_ef, p_efm, TRUE, IObuff, enc) < 0)
	{
	    out_char('\n');
	    mch_exit(3);
	}
	TIME_செய்தி("reading errorfile");
    }
# endif

    /*
     * Start putting things on the screen.
     * Scroll screen down before drawing over it
     * Clear screen now, so file message will not be cleared.
     */
    starting = NO_BUFFERS;
    no_wait_return = FALSE;
    எனில் (!exmode_active)
	செய்தி_scroll = FALSE;

# ifdef FEAT_GUI
    /*
     * This seems to be required to make callbacks to be called now, instead
     * of after things have been put on the screen, which then may be deleted
     * when getting a resize callback.
     * For the Mac this handles putting files dropped on the Vim icon to
     * global_alist.
     */
    எனில் (gui.in_use)
    {
	gui_wait_for_chars(50L, typebuf.tb_change_cnt);
	TIME_செய்தி("GUI delay");
    }
# endif

# எனில் defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)
    qnx_clip_init();
# endif

# எனில் defined(MACOS_X) && defined(FEAT_CLIPBOARD)
    clip_init(TRUE);
# endif

# ifdef FEAT_XCLIPBOARD
    // Start using the X clipboard, unless the GUI was started.
#  ifdef FEAT_GUI
    எனில் (!gui.in_use)
#  endif
    {
	setup_term_clip();
	TIME_செய்தி("setup x11 clipboard");
    }
# endif

# ifdef FEAT_CLIENTSERVER
    // Prepare for being a Vim server.
    prepare_server(&params);
# endif

# ifdef FEAT_WAYLAND
#  ifdef FEAT_GUI
    எனில் (!gui.in_use)
#  endif
    {
	எனில் (wayland_init_connection(wayland_display_name) == OK)
	{
	    TIME_செய்தி("connected to Wayland display");

#  ifdef FEAT_WAYLAND_CLIPBOARD
	    எனில் (clip_init_wayland() == OK)
		TIME_செய்தி("setup Wayland clipboard");
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
    எனில் (params.edit_type == EDIT_STDIN && !recoverymode)
	read_stdin();

# எனில் defined(UNIX) || defined(VMS)
    // When switching screens and something caused a message from a vimrc
    // script, need to output an extra newline on exit.
    எனில் ((did_emsg || செய்தி_didout) && *T_TI != NUL && params.edit_type != EDIT_STDIN)
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
    TIME_செய்தி("setting raw mode");

    எனில் (need_wait_return || செய்தி_didany)
    {
	wait_return(TRUE);
	TIME_செய்தி("waiting for return");
    }

    starttermcap();	    // start termcap if not done by wait_return()
    TIME_செய்தி("start termcap");

    setmouse();				// may start using the mouse
    எனில் (scroll_region)
	scroll_region_reset();		// In case Rows changed
    scroll_start();	// may scroll the screen to the right position

# எனில் defined(UNIX) || defined(VMS) || defined(MACOS_X)
    term_தள்ளு_title(SAVE_RESTORE_BOTH);
# endif

    /*
     * Don't clear the screen when starting in Ex mode, unless using the GUI.
     */
    எனில் (exmode_active
# ifdef FEAT_GUI
			&& !gui.in_use
# endif
					)
	set_must_redraw(UPD_CLEAR);
    இல்லையெனில்
    {
	screenclear();			// clear screen
	TIME_செய்தி("clearing screen");
    }

# ifdef FEAT_CRYPT
    எனில் (params.ask_for_சாவி)
    {
	crypt_check_current_method();
	(வெறுமை)crypt_get_சாவி(TRUE, TRUE);
	TIME_செய்தி("getting crypt key");
    }
# endif

    no_wait_return = TRUE;

    /*
     * Create the requested number of windows and edit buffers in them.
     * Also does recovery if "recoverymode" set.
     */
    create_windows(&params);
    TIME_செய்தி("opening buffers");

# ifdef FEAT_EVAL
    // clear v:swapcommand
    set_vim_var_string(VV_SWAPCOMMAND, வெற்று, -1);
# endif

    // Ex starts at last line of the file
    எனில் (exmode_active)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;

    apply_autocmds(EVENT_BUFENTER, வெற்று, வெற்று, FALSE, curbuf);
    TIME_செய்தி("BufEnter autocommands");
    setpcmark();

# ifdef FEAT_QUICKFIX
    /*
     * When started with "-q errorfile" jump to first error now.
     */
    எனில் (params.edit_type == EDIT_QF)
    {
	qf_jump(வெற்று, 0, 0, FALSE);
	TIME_செய்தி("jump to first error");
    }
# endif

    /*
     * If opened more than one window, start editing files in the other
     * windows.
     */
    edit_buffers(&params, start_dir);
    vim_free(start_dir);

# ifdef FEAT_DIFF
    எனில் (params.diff_mode)
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
    எனில் (params.tagname != வெற்று)
    {
	swap_exists_did_quit = FALSE;

	vim_snprintf((எழுத்து *)IObuff, IOSIZE, "ta %s", params.tagname);
	do_cmdline_cmd(IObuff);
	TIME_செய்தி("jumping to tag");

	// If the user doesn't want to edit the file then we quit here.
	எனில் (swap_exists_did_quit)
	    getout(1);
    }

# எனில் defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    // Temporarily set x_jump_env to here in case there is an X11 IO error,
    // because x_jump_env is only actually set in main_loop(), before
    // exe_commands(). May not be the best solution since commands passed via
    // the command line can be very broad like sourcing a file, in which case
    // an X IO error results in the command being partially done. In theory we
    // could use SETJMP in RealWaitForChar(), but the stack frame for that may
    // possibly exit and then LONGJMP is called on it.
    முழு jump_result = SETJMP(x_jump_env);

    எனில் (jump_result == 0)
    {
# endif
	// Execute any "+", "-c" and "-S" arguments.
	எனில் (params.n_commands > 0)
	    exe_commands(&params);
# எனில் defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    }
    இல்லையெனில்
	// Restore state and continue just like what main_loop() does.
	x_restore_state();
# endif

    // Must come before the may_req_ calls.
    starting = 0;

# எனில் defined(FEAT_TERMRESPONSE)
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
# எனில் defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_size();
# endif

    // start in insert mode
    எனில் (p_im)
	need_start_insertmode = TRUE;

# ifdef FEAT_EVAL
    set_vim_var_nr(VV_VIM_DID_ENTER, 1L);
# endif
    apply_autocmds(EVENT_VIMENTER, வெற்று, வெற்று, FALSE, curbuf);
    TIME_செய்தி("VimEnter autocommands");

# எனில் defined(FEAT_EVAL) && defined(FEAT_CLIPBOARD)
    // Adjust default register name for "unnamed" in 'clipboard'. Can only be
    // done after the clipboard is available and all initial commands that may
    // modify the 'clipboard' setting have run; i.e. just before entering the
    // main loop.
    reset_reg_var();
# endif

# எனில் defined(FEAT_DIFF)
    // When a startup script or session file setup for diff'ing and
    // scrollbind, sync the scrollbind now.
    எனில் (curwin->w_p_diff && curwin->w_p_scb)
    {
	update_topline();
	check_scrollbind((linenr_T)0, 0L);
	TIME_செய்தி("diff scrollbinding");
    }
# endif

# எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    எனில் (!gui.in_use)
#  endif
	mch_set_winsize_now();	    // Allow winsize changes from now on
# endif

# எனில் defined(FEAT_GUI)
    // When tab pages were created, may need to update the tab pages line and
    // scrollbars.  This is skipped while creating them.
    எனில் (gui.in_use && first_tabpage->tp_next != வெற்று)
    {
	out_flush();
	gui_init_which_components(வெற்று);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
# endif

    // If ":startinsert" command used, stuff a dummy command to be able to
    // call normal_cmd(), which will then start Insert mode.
    எனில் (restart_edit != 0)
	stuffcharReadbuff(K_NOP);

# ifdef FEAT_NETBEANS_INTG
    எனில் (netbeansArg != வெற்று && strncmp("-nb", netbeansArg, 3) == 0)
    {
#  ifdef FEAT_GUI
#   எனில் !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
	எனில் (gui.in_use)
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

    TIME_செய்தி("before starting main loop");

# ifdef FEAT_JOB_CHANNEL
    எனில் (use_stdio_channel)
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

    திரும்பு 0;
}

/*
 * Initialisation #1 shared by main() and some tests.
 */
    வெறுமை
common_init_1(வெறுமை)
{
    estack_init();
    cmdline_init();

    (வெறுமை)mb_init();	// init mb_bytelen_tab[] to ones
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
    எனில் ((IObuff = alloc(IOSIZE)) == வெற்று
	    || (NameBuff = alloc(MAXPATHL)) == வெற்று)
	mch_exit(0);
    TIME_செய்தி("Allocated generic buffers");
}


/*
 * Initialisation #2 shared by main() and some tests.
 */
    வெறுமை
common_init_2(mparm_T *paramp)
{
#ifdef NBDEBUG
    // Wait a moment for debugging NetBeans.  Must be after allocating
    // NameBuff.
    nbdebug_log_init("SPRO_GVIM_DEBUG", "SPRO_GVIM_DLEVEL");
    nbdebug_wait(WT_ENV | WT_WAIT | WT_STOP, "SPRO_GVIM_WAIT", 20);
    TIME_செய்தி("NetBeans debug wait");
#endif

#எனில் defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    /*
     * Setup to use the current locale (for ctype() and many other things).
     * NOTE: Translated messages with encodings other than latin1 will not
     * work until set_init_1() has been called!
     */
    init_locale();
    TIME_செய்தி("locale set");
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

#எனில் defined(FEAT_GUI)
    // Prepare for possibly starting GUI sometime
    gui_prepare(&paramp->argc, paramp->argv);
    TIME_செய்தி("GUI prepared");
#endif

#ifdef FEAT_CLIPBOARD
    clip_init(FALSE);		// Initialise clipboard stuff
    TIME_செய்தி("clipboard setup");
#endif

    /*
     * Check if we have an interactive window.
     * On the Amiga: If there is no window, we open one with a newcli command
     * (needed for :! to * work). mch_check_win() will also handle the -d or
     * -dev argument.
     */
    stdout_isatty = (mch_check_win(paramp->argc, paramp->argv) != FAIL);
    TIME_செய்தி("window checked");

    /*
     * Allocate the first window and buffer.
     * Can't do anything without it, exit when it fails.
     */
    எனில் (win_alloc_first() == FAIL)
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
    TIME_செய்தி("inits 1");

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
    qf_init_அடுக்கு();
#endif
}

/*
 * Return TRUE when the --not-a-term argument was found.
 */
    முழு
is_not_a_term(வெறுமை)
{
    திரும்பு params.not_a_term;
}

/*
 * Return TRUE when the --not-a-term argument was found or the GUI is in use.
 */
    முழு
is_not_a_term_or_gui(வெறுமை)
{
    திரும்பு params.not_a_term
#ifdef FEAT_GUI
			    || gui.in_use
#endif
	;
}

#எனில் defined(EXITFREE)
    வெறுமை
free_vbuf(வெறுமை)
{
# ifdef _IOLBF
    எனில் (s_vbuf != வெற்று)
    {
	setvbuf(stdout, வெற்று, _IONBF, 0);
	விடுவி(s_vbuf);
	s_vbuf = வெற்று;
    }
# endif
}
#endif

#எனில் defined(FEAT_GUI)
/*
 * If a --gui-dialog-file argument was given return the file name.
 * Otherwise return NULL.
 */
    char_u *
get_gui_dialog_file(வெறுமை)
{
    திரும்பு params.gui_dialog_file;
}
#endif

// When TRUE in a safe state when starting to wait for a character.
நிலையான முழு	was_safe = FALSE;
நிலையான oparg_T	*current_oap = வெற்று;

/*
 * Return TRUE if an operator was started but not finished yet.
 * Includes typing a count or a register name.
 */
    முழு
op_pending(வெறுமை)
{
    திரும்பு !(current_oap != வெற்று
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
    நிலையான முழு
is_safe_now(வெறுமை)
{
    திரும்பு stuff_empty()
	&& typebuf.tb_len == 0
	&& scriptin[curscript] == வெற்று
#ifdef FEAT_EVAL
	&& !debug_mode
#endif
	&& !global_busy;
}

/*
 * Trigger SafeState if currently in a safe state, that is "safe" is TRUE and
 * there is no typeahead.
 */
    வெறுமை
may_trigger_safestate(முழு safe)
{
    முழு is_safe = safe && is_safe_now();

#ifdef FEAT_EVAL
    எனில் (was_safe != is_safe)
	// Only log when the state changes, otherwise it happens at nearly
	// every key stroke.
	ch_log(வெற்று, is_safe ? "SafeState: Start triggering"
					       : "SafeState: Stop triggering");
#endif
    எனில் (is_safe)
	apply_autocmds(EVENT_SAFESTATE, வெற்று, வெற்று, FALSE, curbuf);
    was_safe = is_safe;
}

/*
 * Something changed which causes the state possibly to be unsafe, e.g. a
 * character was typed.  It will remain unsafe until the next call to
 * may_trigger_safestate().
 */
    வெறுமை
state_no_longer_safe(எழுத்து *reason UNUSED)
{
#ifdef FEAT_EVAL
    எனில் (was_safe)
	ch_log(வெற்று, "SafeState: reset: %s", reason);
#endif
    was_safe = FALSE;
}

#எனில் defined(FEAT_EVAL) || defined(MESSAGE_QUEUE)
    முழு
get_was_safe_state(வெறுமை)
{
    திரும்பு was_safe;
}
#endif

#எனில் defined(MESSAGE_QUEUE)
/*
 * Invoked when leaving code that invokes callbacks.  Then trigger
 * SafeStateAgain, if it was safe when starting to wait for a character.
 */
    வெறுமை
may_trigger_safestateagain(வெறுமை)
{
    எனில் (!was_safe)
    {
	// If the safe state was reset in state_no_longer_safe(), e.g. because
	// of calling feedkeys(), we check if it's now safe again (all keys
	// were consumed).
	was_safe = is_safe_now();
# ifdef FEAT_EVAL
	எனில் (was_safe)
	    ch_log(வெற்று, "SafeState: undo reset");
# endif
    }
    எனில் (was_safe)
    {
# ifdef FEAT_EVAL
	// Only do this message when another message was given, otherwise we
	// get lots of them.
	எனில் ((did_repeated_செய்தி & REPEATED_செய்தி_SAFESTATE) == 0)
	{
	    முழு did = did_repeated_செய்தி;

	    ch_log(வெற்று,
		      "SafeState: back to waiting, triggering SafeStateAgain");
	    did_repeated_செய்தி = did | REPEATED_செய்தி_SAFESTATE;
	}
# endif
	apply_autocmds(EVENT_SAFESTATEAGAIN, வெற்று, வெற்று, FALSE, curbuf);
    }
# ifdef FEAT_EVAL
    இல்லையெனில்
	ch_log(வெற்று,
		  "SafeState: back to waiting, not triggering SafeStateAgain");
# endif
}
#endif

/*
 * Return TRUE if there is any typeahead, pending operator or command.
 */
    முழு
work_pending(வெறுமை)
{
    திரும்பு op_pending() || !is_safe_now();
}

#ifdef FEAT_CONCEAL
நிலையான linenr_T	conceal_old_cursor_வரி = 0;
நிலையான linenr_T	conceal_new_cursor_வரி = 0;
நிலையான முழு	conceal_update_வரிகள் = FALSE;
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
    வெறுமை
may_trigger_deferred_events(வெறுமை)
{
    நிலையான bool	recursive = false;

    எனில் (recursive)
	திரும்பு;
    recursive = true;

#ifdef FEAT_CONCEAL
    எனில் (curwin->w_p_cole == 0)
	conceal_update_வரிகள் = FALSE;
#endif

    // Trigger CursorMoved if the cursor moved.
    எனில் (!finish_op && (has_cursormoved()
#ifdef FEAT_PROP_POPUP
		|| popup_visible
#endif
#ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
#endif
		) && !EQUAL_POS(last_cursormoved, curwin->w_cursor))
    {
	எனில் (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, வெற்று, வெற்று, FALSE, curbuf);
#ifdef FEAT_PROP_POPUP
	எனில் (popup_visible)
	    popup_check_cursor_pos();
#endif
#ifdef FEAT_CONCEAL
	எனில் (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_வரி = last_cursormoved.lnum;
	    conceal_new_cursor_வரி = curwin->w_cursor.lnum;
	    conceal_update_வரிகள் = TRUE;
	}
#endif
	last_cursormoved = curwin->w_cursor;
    }

#ifdef FEAT_CONCEAL
    எனில் (conceal_update_வரிகள்
	    && (conceal_old_cursor_வரி != conceal_new_cursor_வரி
		|| conceal_cursor_வரி(curwin)
		|| need_cursor_வரி_redraw))
    {
	எனில் (conceal_old_cursor_வரி != conceal_new_cursor_வரி
		&& conceal_old_cursor_வரி != 0
		&& conceal_old_cursor_வரி <= curbuf->b_ml.ml_வரி_மொத்தம்)
	    redrawWinline(curwin, conceal_old_cursor_வரி);
	redrawWinline(curwin, conceal_new_cursor_வரி);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_வரி_redraw = FALSE;
    }
#endif

    // Trigger TextChanged if b:changedtick differs.
    எனில் (!finish_op && has_textchanged()
	    && curbuf->b_last_changedtick != CHANGEDTICK(curbuf))
    {
	apply_autocmds(EVENT_TEXTCHANGED, வெற்று, வெற்று, FALSE, curbuf);
	curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    }

    // Ensure curwin->w_topline and curwin->w_leftcol are up to date before
    // triggering a WinScrolled autocommand.
    update_topline();
    validate_cursor();

    எனில் (!finish_op)
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
    வெறுமை
main_loop(
    முழு		cmdwin,	    // TRUE when working in the command-line window
    முழு		noexmode)   // TRUE when return on entering Ex mode
{
    oparg_T	oa;		// operator arguments
    oparg_T	*prev_oap;	// operator arguments
    நிலையற்ற முழு previous_got_int = FALSE;	// "got_int" was TRUE

    prev_oap = current_oap;
    current_oap = &oa;

#எனில் defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    // Setup to catch a terminating error from the X server.  Just ignore
    // it, restore the state and continue.  This might not always work
    // properly, but at least we hopefully don't exit unexpectedly when the X
    // server exits while Vim is running in a console.
    எனில் (!cmdwin && !noexmode && SETJMP(x_jump_env))
	x_restore_state();
#endif

    clear_oparg(&oa);
    போது (!cmdwin || cmdwin_result == 0)
    {
	எனில் (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    எனில் (need_check_timestamps)
		check_timestamps(FALSE);
	    எனில் (need_wait_return)	// if wait_return() still needed ...
		wait_return(FALSE);	// ... call it now
	    எனில் (need_start_insertmode && goto_im() && !VIsual_active)
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
	எனில் (got_int)
	{
	    எனில் (noexmode && global_busy && !exmode_active && previous_got_int)
	    {
		// Typed two CTRL-C in a row: go back to ex mode as if "Q" was
		// used and keep "got_int" set, so that it aborts ":g".
		exmode_active = EXMODE_NORMAL;
		State = MODE_NORMAL;
	    }
	    இல்லையெனில் எனில் (!global_busy || !exmode_active)
	    {
		எனில் (!quit_more)
		    (வெறுமை)vgetc();		// flush all buffers
		got_int = FALSE;
	    }
	    previous_got_int = TRUE;
	}
	இல்லையெனில்
	    previous_got_int = FALSE;

#ifdef FEAT_EVAL
	// At the toplevel there is no exception handling.  Discard any that
	// may be hanging around (e.g. from "interrupt" at the debug prompt).
	எனில் (did_throw && !ex_normal_busy)
	    discard_current_exception();
#endif

	எனில் (!exmode_active)
	    செய்தி_scroll = FALSE;
	quit_more = FALSE;

	// it's not safe unless may_trigger_safestate_main() is called
	was_safe = FALSE;

	/*
	 * If skip redraw is set (for ":" in wait_return()), don't redraw now.
	 * If there is nothing in the stuff_buffer or do_redraw is TRUE,
	 * update cursor and redraw.
	 */
	எனில் (skip_redraw || exmode_active)
	{
	    skip_redraw = FALSE;
	    setcursor();
	    cursor_on();
	}
	இல்லையெனில் எனில் (do_redraw || stuff_empty())
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

#எனில் defined(FEAT_DIFF)
	    // Updating diffs from changed() does not always work properly,
	    // esp. updating folds.  Do an update just before redrawing if
	    // needed.
	    எனில் (curtab->tp_diff_update || curtab->tp_diff_invalid)
	    {
		ex_diffupdate(வெற்று);
		curtab->tp_diff_update = FALSE;
	    }

	    // Scroll-binding for diff mode may have been postponed until
	    // here.  Avoids doing it for every change.
	    எனில் (diff_need_scrollbind)
	    {
		check_scrollbind((linenr_T)0, 0L);
		diff_need_scrollbind = FALSE;
	    }
#endif
#எனில் defined(FEAT_FOLDING)
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
	    எனில் (hasAnyFolding(curwin) && !char_avail())
	    {
		foldCheckClose();
		எனில் (fdo_flags & FDO_ALL)
		    foldOpenCursor();
	    }
#endif

	    // Before redrawing, make sure w_topline is correct, and w_leftcol
	    // if lines don't wrap, and w_skipcol if lines wrap.
	    update_topline();
	    validate_cursor();

	    எனில் (VIsual_active)
		update_curbuf(UPD_INVERTED); // update inverted part
	    இல்லையெனில் எனில் (must_redraw)
	    {
		mch_disable_flush();	// Stop issuing gui_mch_flush().
		update_screen(0);
		mch_enable_flush();
	    }
	    இல்லையெனில் எனில் (redraw_cmdline || clear_cmdline || redraw_mode)
		showmode();
	    redraw_statuslines();
	    எனில் (need_maketitle)
		maketitle();
#ifdef FEAT_VIMINFO
	    curbuf->b_last_used = vim_time();
#endif
	    // display message after redraw
	    எனில் (keep_செய்தி != வெற்று)
	    {
		char_u *p = vim_strsave(keep_செய்தி);

		எனில் (p != வெற்று)
		{
		    // msg_start() will set keep_msg to NULL, make a copy
		    // first.  Don't reset keep_msg, msg_attr_keep() uses it to
		    // check for duplicates.  Never put this message in
		    // history.
		    செய்தி_hist_off = TRUE;
		    செய்தி_attr((எழுத்து *)p, keep_செய்தி_attr);
		    செய்தி_hist_off = FALSE;
		    vim_free(p);
		}
	    }
	    எனில் (need_fileinfo)		// show file info after redraw
	    {
		fileinfo(FALSE, TRUE, FALSE);
		need_fileinfo = FALSE;
	    }

	    emsg_on_display = FALSE;	// can delete error message now
	    did_emsg = FALSE;
	    செய்தி_didany = FALSE;		// reset lines_left in msg_start()
	    may_clear_sb_text();	// clear scroll-back text on next msg
	    showruler(FALSE);

	    setcursor();
	    cursor_on();

	    do_redraw = FALSE;

#ifdef STARTUPTIME
	    // Now that we have drawn the first screen all the startup stuff
	    // has been done, close any file for startup messages.
	    எனில் (time_fd != வெற்று)
	    {
		TIME_செய்தி("first screen update");
		TIME_செய்தி("--- VIM STARTED ---");
		fclose(time_fd);
		time_fd = வெற்று;
	    }
#endif
	    // After the first screen update may start triggering WinScrolled
	    // autocmd events.  Store all the scroll positions and sizes now.
	    may_make_initial_scroll_size_snapshot();
	}
#ifdef FEAT_GUI
	எனில் (need_mouse_correct)
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
	எனில் (exmode_active)
	{
	    எனில் (noexmode)   // End of ":global/path/visual" commands
		செல் theend;
	    do_exmode(exmode_active == EXMODE_VIM);
	}
	இல்லையெனில்
	{
#ifdef FEAT_TERMINAL
	    எனில் (term_use_loop()
		    && oa.op_type == OP_NOP && oa.regname == NUL
		    && !VIsual_active
		    && !skip_term_loop)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal mode.  With FAIL we first need to position the
		// cursor and the screen needs to be redrawn.
		எனில் (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    இல்லையெனில்
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


#எனில் defined(USE_XSMP) || defined(FEAT_GUI)
/*
 * Exit, but leave behind swap files for modified buffers.
 */
    வெறுமை
getout_preserve_modified(முழு exitval)
{
# எனில் defined(SIGHUP) && defined(SIG_IGN)
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
    வெறுமை
getout(முழு exitval)
{
    exiting = TRUE;
#எனில் defined(FEAT_EVAL)
    ch_log(வெற்று, "Exiting...");
#endif

    // When running in Ex mode an error causes us to exit with a non-zero exit
    // code.  POSIX requires this, although it's not 100% clear from the
    // standard.
    எனில் (exmode_active)
	exitval += ex_exitval;

#ifdef FEAT_EVAL
    set_vim_var_type(VV_EXITING, VAR_எண்);
    set_vim_var_nr(VV_EXITING, exitval);
#endif

    // Position the cursor on the last screen line, below all the text
    எனில் (!is_not_a_term_or_gui())
	windgoto((முழு)Rows - 1, 0);

#ifdef FEAT_EVAL
    // Invoked all deferred functions in the function stack.
    invoke_all_defer();
#endif

#எனில் defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
    // Optionally print hashtable efficiency.
    hash_debug_results();
#endif

#ifdef FEAT_GUI
    செய்தி_didany = FALSE;
#endif

    எனில் (v_dying <= 1)
    {
	tabpage_T	*tp;
	tabpage_T	*next_tp;
	buf_T		*buf;
	win_T		*wp;
	முழு		unblock = 0;

	// Trigger BufWinLeave for all windows, but only once per buffer.
	க்கு (tp = first_tabpage; tp != வெற்று; tp = next_tp)
	{
	    next_tp = tp->tp_next;
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		எனில் (wp->w_buffer == வெற்று || !buf_valid(wp->w_buffer))
		    // Autocmd must have close the buffer already, skip.
		    தொடர்;
		buf = wp->w_buffer;
		எனில் (CHANGEDTICK(buf) != -1)
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf);
		    apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname,
						    buf->b_fname, FALSE, buf);
		    எனில் (bufref_valid(&bufref))
			CHANGEDTICK(buf) = -1;  // note we did it already

		    // start all over, autocommands may mess up the lists
		    next_tp = first_tabpage;
		    நிறுத்து;
		}
	    }
	}

	// Trigger BufUnload for buffers that are loaded
	FOR_ALL_BUFFERS(buf)
	    எனில் (buf->b_ml.ml_mfp != வெற்று)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf);
		எனில் (!bufref_valid(&bufref))
		    // autocmd deleted the buffer
		    நிறுத்து;
	    }

	// deathtrap() blocks autocommands, but we do want to trigger
	// VimLeavePre.
	எனில் (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVEPRE, வெற்று, வெற்று, FALSE, curbuf);
	எனில் (unblock)
	    block_autocmds();
    }

#ifdef FEAT_VIMINFO
    எனில் (
# ifdef EXITFREE
	    entered_free_all_mem == FALSE &&
# endif
	    *p_viminfo != NUL)
	// Write out the registers, history, marks etc, to the viminfo file
	write_viminfo(வெற்று, FALSE);
#endif

    எனில் (v_dying <= 1)
    {
	முழு	unblock = 0;

	// deathtrap() blocks autocommands, but we do want to trigger VimLeave.
	எனில் (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVE, வெற்று, வெற்று, FALSE, curbuf);
	எனில் (unblock)
	    block_autocmds();
    }

#ifdef FEAT_PROFILE
    profile_dump();
#endif

    எனில் (did_emsg
#ifdef FEAT_GUI
	    || (gui.in_use && செய்தி_didany && p_verbose > 0)
#endif
	    )
    {
	// give the user a chance to read the (error) message
	no_wait_return = FALSE;
	wait_return(FALSE);
    }

    // Position the cursor again, the autocommands may have moved it
    எனில் (!is_not_a_term_or_gui())
	windgoto((முழு)Rows - 1, 0);

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
#எனில் defined(USE_ICONV) && defined(DYNAMIC_ICONV)
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
    எனில் (garbage_collect_at_exit)
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
    நிலையான வெறுமை
early_arg_scan(mparm_T *parmp UNUSED)
{
#எனில் defined(FEAT_XCLIPBOARD) || defined(FEAT_CLIENTSERVER) \
	|| !defined(FEAT_NETBEANS_INTG)
    முழு		argc = parmp->argc;
    எழுத்து	**argv = parmp->argv;
    முழு		i;

    க்கு (i = 1; i < argc; i++)
    {
	எனில் (STRCMP(argv[i], "--") == 0)
	    நிறுத்து;
# ifdef FEAT_XCLIPBOARD
	இல்லையெனில் எனில் (STRICMP(argv[i], "-display") == 0
#  எனில் defined(FEAT_GUI_GTK)
		|| STRICMP(argv[i], "--display") == 0
#  endif
		)
	{
	    எனில் (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    xterm_display = argv[++i];
	}
# endif
# ifdef FEAT_CLIENTSERVER
	இல்லையெனில் எனில் (STRICMP(argv[i], "--servername") == 0)
	{
	    எனில் (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    parmp->serverName_arg = (char_u *)argv[++i];
	}
	இல்லையெனில் எனில் (STRICMP(argv[i], "--serverlist") == 0)
	    parmp->serverArg = TRUE;
	இல்லையெனில் எனில் (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    parmp->serverArg = TRUE;
#  ifdef FEAT_GUI
	    எனில் (strstr(argv[i], "-wait") != 0)
		// don't fork() when starting the GUI to edit files ourself
		gui.dofork = false;
#  endif
	}
#  ifdef FEAT_CLIENTSERVER_BACKENDS
	இல்லையெனில் எனில் (STRICMP(argv[i], "--clientserver") == 0)
	{
	    char_u *arg;
	    எனில் (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    arg = (char_u *)argv[++i];

	    எனில் (STRICMP(arg, "socket") == 0)
		clientserver_method = CLIENTSERVER_METHOD_SOCKET;
#   ifdef FEAT_X11
	    இல்லையெனில் எனில் (STRICMP(arg, "x11") == 0)
		clientserver_method = CLIENTSERVER_METHOD_X11;
#   endif
#   ifdef MSWIN
	    இல்லையெனில் எனில் (STRICMP(arg, "mswin") == 0)
		clientserver_method = CLIENTSERVER_METHOD_MSWIN;
#   endif
	    இல்லையெனில்
		mainerr(ME_UNKNOWN_OPTION, arg);
	}
#  endif
# endif

# எனில் defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
#  ifdef FEAT_GUI_MSWIN
	இல்லையெனில் எனில் (STRICMP(argv[i], "--windowid") == 0)
#  இல்லையெனில்
	இல்லையெனில் எனில் (STRICMP(argv[i], "--socketid") == 0)
#  endif
	{
	    long_u	id;
	    முழு		மொத்தம்;

	    எனில் (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    எனில் (STRNICMP(argv[i+1], "0x", 2) == 0)
		மொத்தம் = sscanf(&(argv[i + 1][2]), SCANF_HEX_LONG_U, &id);
	    இல்லையெனில்
		மொத்தம் = sscanf(argv[i + 1], SCANF_DECIMAL_LONG_U, &id);
	    எனில் (மொத்தம் != 1)
		mainerr(ME_INVALID_ARG, (char_u *)argv[i]);
	    இல்லையெனில்
#  ifdef FEAT_GUI_MSWIN
		win_socket_id = id;
#  elif !defined(USE_GTK4)
		gtk_socket_id = id;
#  endif
	    i++;
	}
# endif
# எனில் defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
	இல்லையெனில் எனில் (STRICMP(argv[i], "--echo-wid") == 0)
	    echo_wid_arg = TRUE;
# endif
# ifndef FEAT_NETBEANS_INTG
	இல்லையெனில் எனில் (strncmp(argv[i], "-nb", (size_t)3) == 0)
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
    நிலையான முழு
get_எண்_arg(
    char_u	*p,	    // pointer to argument
    முழு		*idx,	    // index in argument, is incremented
    முழு		def)	    // default value
{
    எனில் (vim_isdigit(p[*idx]))
    {
	def = atoi((எழுத்து *)&(p[*idx]));
	போது (vim_isdigit(p[*idx]))
	    *idx = *idx + 1;
    }
    திரும்பு def;
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
    நிலையான வெறுமை
parse_command_name(mparm_T *parmp)
{
    char_u	*initstr;

    initstr = gettail((char_u *)parmp->argv[0]);

# ifdef FEAT_EVAL
    set_vim_var_string(VV_PROGNAME, initstr, -1);
    set_progpath((char_u *)parmp->argv[0]);
# endif

    எனில் (TOLOWER_ASC(initstr[0]) == 'r')
    {
	restricted = TRUE;
	++initstr;
    }

    // Use evim mode for "evim" and "egvim", not for "editor".
    எனில் (TOLOWER_ASC(initstr[0]) == 'e'
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
    எனில் (TOLOWER_ASC(initstr[0]) == 'g'
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
    இல்லையெனில்
	gui.dospawn = true;	// Not "gvim". Need to spawn gvim.exe.
# endif


    எனில் (STRNICMP(initstr, "view", 4) == 0)
    {
	readonlymode = TRUE;
	curbuf->b_p_ro = TRUE;
	p_uc = 10000;			// don't update very often
	initstr += 4;
    }
    இல்லையெனில் எனில் (STRNICMP(initstr, "vim", 3) == 0)
	initstr += 3;

    // Catch "[r][g]vimdiff" and "[r][g]viewdiff".
    எனில் (STRICMP(initstr, "diff") == 0)
    {
# ifdef FEAT_DIFF
	parmp->diff_mode = TRUE;
# இல்லையெனில்
	mch_errmsg(_("This Vim was not compiled with the diff feature."));
	mch_errmsg("\n");
	mch_exit(2);
# endif
    }

    // Checking for "ex" here may catch some weird names, such as "vimex" or
    // "viewex", we assume the user knows that.
    எனில் (STRNICMP(initstr, "ex", 2) == 0)
    {
	எனில் (STRNICMP(initstr + 2, "im", 2) == 0)
	    exmode_active = EXMODE_VIM;
	இல்லையெனில்
	    exmode_active = EXMODE_NORMAL;
	change_compatible(TRUE);	// set 'compatible'
    }
}

/*
 * Scan the command line arguments.
 */
    நிலையான வெறுமை
command_வரி_scan(mparm_T *parmp)
{
    முழு		argc = parmp->argc;
    எழுத்து	**argv = parmp->argv;
    முழு		argv_idx;		// index in argv[n][]
    முழு		had_minmin = FALSE;	// found "--" argument
    முழு		want_argument;		// option argument with argument
    முழு		c;
    char_u	*p = வெற்று;
    long	n;

    --argc;
    ++argv;
    argv_idx = 1;	    // active option letter is argv[0][argv_idx]
    போது (argc > 0)
    {
	/*
	 * "+" or "+{number}" or "+/{pat}" or "+{command}" argument.
	 */
	எனில் (argv[0][0] == '+' && !had_minmin)
	{
	    எனில் (parmp->n_commands >= MAX_ARG_CMDS)
		mainerr(ME_EXTRA_CMD, வெற்று);
	    argv_idx = -1;	    // skip to next argument
	    எனில் (argv[0][1] == NUL)
		parmp->commands[parmp->n_commands++] = (char_u *)"$";
	    இல்லையெனில்
		parmp->commands[parmp->n_commands++] = (char_u *)&(argv[0][1]);
	}

	/*
	 * Optional argument.
	 */
	இல்லையெனில் எனில் (argv[0][0] == '-' && !had_minmin)
	{
	    want_argument = FALSE;
	    c = argv[0][argv_idx++];
# எனில் defined( VMS)
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
	    எனில் (c == '/')
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
	    தேர்வு (c)
	    {
	    நிலை NUL:		// "vim -"  read from stdin
				// "ex -" silent mode
		எனில் (exmode_active)
		    silent_mode = TRUE;
		இல்லையெனில்
		{
		    எனில் (parmp->edit_type != EDIT_NONE)
			mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
		    parmp->edit_type = EDIT_STDIN;
		    read_cmd_fd = 2;	// read from stderr instead of stdin
		}
		argv_idx = -1;		// skip to next argument
		நிறுத்து;

	    நிலை '-':		// "--" don't take any more option arguments
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
		எனில் (STRICMP(argv[0] + argv_idx, "help") == 0)
		    usage();
		இல்லையெனில் எனில் (STRICMP(argv[0] + argv_idx, "version") == 0)
		{
		    cmdline_width = Columns = 80;   // need to init Columns
		    info_message = TRUE; // use mch_msg(), not mch_errmsg()
# எனில் defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
		    gui.starting = false; // not starting GUI, will exit
# endif
		    list_version();
		    செய்தி_putchar('\n');
		    செய்தி_didout = FALSE;
		    mch_exit(0);
		}
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "clean", 5) == 0)
		{
		    parmp->use_vimrc = (char_u *)"DEFAULTS";
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)"NONE";
# endif
		    parmp->clean = TRUE;
		    set_option_மதிப்பு_give_err((char_u *)"vif",
						      0L, (char_u *)"NONE", 0);
		}
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "literal", 7) == 0)
		{
# ifdef EXPAND_FILENAMES
		    parmp->literal = TRUE;
# endif
		}
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "nofork", 6) == 0)
		{
# ifdef FEAT_GUI
		    gui.dofork = false;	// don't fork() when starting GUI
# endif
		}
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "noplugin", 8) == 0)
		    p_lpl = FALSE;
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "not-a-term", 10) == 0)
		    parmp->not_a_term = TRUE;
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "gui-dialog-file", 15)
									 == 0)
		{
		    want_argument = TRUE;
		    argv_idx += 15;
		}
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "ttyfail", 7) == 0)
		    parmp->tty_fail = TRUE;
# ifdef FEAT_JOB_CHANNEL
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "stdio-channel", 13) == 0)
		{
		    // Like "-es": no terminal, but the channel is served
		    // instead of reading Ex commands.
		    use_stdio_channel = TRUE;
		    exmode_active = EXMODE_NORMAL;
		    silent_mode = TRUE;
#  எனில் defined(FEAT_GUI) && !defined(VIMDLL)
		    gui.starting = false;	// don't start GUI
#  endif
		}
# endif
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "cmd", 3) == 0)
		{
		    want_argument = TRUE;
		    argv_idx += 3;
		}
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "startuptime", 11) == 0)
		{
		    want_argument = TRUE;
		    argv_idx += 11;
		}
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "log", 3) == 0)
		{
		    want_argument = TRUE;
		    argv_idx += 3;
		}
# ifdef FEAT_CLIENTSERVER
		இல்லையெனில் எனில் (STRICMP(argv[0] + argv_idx, "serverlist") == 0)
		    ; // already processed -- no arg
		இல்லையெனில் எனில் (STRICMP(argv[0] + argv_idx, "servername") == 0
		       || STRICMP(argv[0] + argv_idx, "serversend") == 0
		       // Don't put this under FEAT_CLIENTSERVER_BACKENDS, just
		       // let it be ignored. Makes tests less complicated
		       || STRICMP(argv[0] + argv_idx, "clientserver") == 0
		       )
		{
		    // already processed -- snatch the following arg
		    எனில் (argc > 1)
		    {
			--argc;
			++argv;
		    }
		}
# endif
# எனில் defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
#  ifdef FEAT_GUI_GTK
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "socketid", 8) == 0)
#  இல்லையெனில்
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "windowid", 8) == 0)
#  endif
		{
		    // already processed -- snatch the following arg
		    எனில் (argc > 1)
		    {
			--argc;
			++argv;
		    }
		}
# endif
# ifdef FEAT_GUI_GTK
		இல்லையெனில் எனில் (STRNICMP(argv[0] + argv_idx, "echo-wid", 8) == 0)
		{
		    // already processed, skip
		}
# endif
		இல்லையெனில்
		{
		    எனில் (argv[0][argv_idx])
			mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
		    had_minmin = TRUE;
		}
		எனில் (!want_argument)
		    argv_idx = -1;	// skip to next argument
		நிறுத்து;

	    நிலை 'A':		// "-A" start in Arabic mode
# ifdef FEAT_ARABIC
		set_option_மதிப்பு_give_err((char_u *)"arabic", 1L, வெற்று, 0);
# இல்லையெனில்
		mch_errmsg(_(e_arabic_cannot_be_used_not_enabled_at_compile_time));
		mch_exit(2);
# endif
		நிறுத்து;

	    நிலை 'b':		// "-b" binary mode
		// Needs to be effective before expanding file names, because
		// for Win32 this makes us edit a shortcut file itself,
		// instead of the file it links to.
		set_options_bin(curbuf->b_p_bin, 1, 0);
		curbuf->b_p_bin = 1;	    // binary file I/O
		நிறுத்து;

	    நிலை 'C':		// "-C"  Compatible
		change_compatible(TRUE);
		has_dash_c_arg = TRUE;
		நிறுத்து;

	    நிலை 'e':		// "-e" Ex mode
		exmode_active = EXMODE_NORMAL;
		நிறுத்து;

	    நிலை 'E':		// "-E" Improved Ex mode
		exmode_active = EXMODE_VIM;
		நிறுத்து;

	    நிலை 'f':		// "-f"  GUI: run in foreground.  Amiga: open
				// window directly, not with newcli
# ifdef FEAT_GUI
		gui.dofork = false;	// don't fork() when starting GUI
# endif
		நிறுத்து;

	    நிலை 'g':		// "-g" start GUI
		main_start_gui();
		நிறுத்து;

	    நிலை 'F':		// "-F" was for Farsi mode
		mch_errmsg(_(e_farsi_support_has_been_removed));
		mch_exit(2);
		நிறுத்து;

	    நிலை '?':		// "-?" give help message (for MS-Windows)
	    நிலை 'h':		// "-h" give help message
# ifdef FEAT_GUI_GNOME
		// Tell usage() to exit for "gvim".
		gui.starting = false;
# endif
		usage();
		நிறுத்து;

	    நிலை 'H':		// "-H" start in Hebrew mode: rl + hkmap set
# ifdef FEAT_RIGHTLEFT
		p_hkmap = TRUE;
		set_option_மதிப்பு_give_err((char_u *)"rl", 1L, வெற்று, 0);
# இல்லையெனில்
		mch_errmsg(_(e_hebrew_cannot_be_used_not_enabled_at_compile_time));
		mch_exit(2);
# endif
		நிறுத்து;

	    நிலை 'l':		// "-l" lisp mode, 'lisp' and 'showmatch' on
		set_option_மதிப்பு_give_err((char_u *)"lisp", 1L, வெற்று, 0);
		p_sm = TRUE;
		நிறுத்து;

	    நிலை 'M':		// "-M"  no changes or writing of files
		reset_modifiable();
		// FALLTHROUGH

	    நிலை 'm':		// "-m"  no writing of files
		p_write = FALSE;
		நிறுத்து;

	    நிலை 'y':		// "-y"  easy mode
# ifdef FEAT_GUI
		gui.starting = true;	// start GUI a bit later
# endif
		parmp->evim_mode = TRUE;
		நிறுத்து;

	    நிலை 'N':		// "-N"  Nocompatible
		change_compatible(FALSE);
		நிறுத்து;

	    நிலை 'n':		// "-n" no swap file
# ifdef FEAT_NETBEANS_INTG
		// checking for "-nb", netbeans parameters
		எனில் (argv[0][argv_idx] == 'b')
		{
		    netbeansArg = argv[0];
		    argv_idx = -1;	    // skip to next argument
		}
		இல்லையெனில்
# endif
		parmp->no_swap_file = TRUE;
		நிறுத்து;

	    நிலை 'p':		// "-p[N]" open N tab pages
# ifdef TARGET_API_MAC_OSX
		// For some reason on MacOS X, an argument like:
		// -psn_0_10223617 is passed in when invoke from Finder
		// or with the 'open' command
		எனில் (argv[0][argv_idx] == 's')
		{
		    argv_idx = -1; // bypass full -psn
		    main_start_gui();
		    நிறுத்து;
		}
# endif
		// default is 0: open window for each file
		parmp->window_மொத்தம் = get_எண்_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->window_layout = WIN_TABS;
		நிறுத்து;

	    நிலை 'o':		// "-o[N]" open N horizontal split windows
		// default is 0: open window for each file
		parmp->window_மொத்தம் = get_எண்_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->window_layout = WIN_HOR;
		நிறுத்து;

		நிலை 'O':	// "-O[N]" open N vertical split windows
		// default is 0: open window for each file
		parmp->window_மொத்தம் = get_எண்_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->window_layout = WIN_VER;
		நிறுத்து;

# ifdef FEAT_QUICKFIX
	    நிலை 'q':		// "-q" QuickFix mode
		எனில் (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
		parmp->edit_type = EDIT_QF;
		எனில் (argv[0][argv_idx])		// "-q{errorfile}"
		{
		    parmp->use_ef = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		இல்லையெனில் எனில் (argc > 1)		// "-q {errorfile}"
		    want_argument = TRUE;
		நிறுத்து;
# endif

	    நிலை 'R':		// "-R" readonly mode
		readonlymode = TRUE;
		curbuf->b_p_ro = TRUE;
		p_uc = 10000;			// don't update very often
		நிறுத்து;

	    நிலை 'r':		// "-r" recovery mode
	    நிலை 'L':		// "-L" recovery mode
		recoverymode = 1;
		நிறுத்து;

	    நிலை 's':
		எனில் (exmode_active)	// "-s" silent (batch) mode
		    silent_mode = TRUE;
		இல்லையெனில்		// "-s {scriptin}" read from script file
		    want_argument = TRUE;
		நிறுத்து;

	    நிலை 't':		// "-t {tag}" or "-t{tag}" jump to tag
		எனில் (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
		parmp->edit_type = EDIT_TAG;
		எனில் (argv[0][argv_idx])		// "-t{tag}"
		{
		    parmp->tagname = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		இல்லையெனில்				// "-t {tag}"
		    want_argument = TRUE;
		நிறுத்து;

# ifdef FEAT_EVAL
	    நிலை 'D':		// "-D"		Debugging
		parmp->use_debug_break_level = 9999;
		நிறுத்து;
# endif
# ifdef FEAT_DIFF
	    நிலை 'd':		// "-d"		'diff'
#  ifdef AMIGA
		// check for "-dev {device}"
		எனில் (argv[0][argv_idx] == 'e' && argv[0][argv_idx + 1] == 'v')
		    want_argument = TRUE;
		இல்லையெனில்
#  endif
		    parmp->diff_mode = TRUE;
		நிறுத்து;
# endif
	    நிலை 'V':		// "-V{N}"	Verbose level
		// default is 10: a little bit verbose
		p_verbose = get_எண்_arg((char_u *)argv[0], &argv_idx, 10);
		எனில் (argv[0][argv_idx] != NUL)
		{
		    set_option_மதிப்பு_give_err((char_u *)"verbosefile",
					  0L, (char_u *)argv[0] + argv_idx, 0);
		    argv_idx = (முழு)STRLEN(argv[0]);
		}
		நிறுத்து;

	    நிலை 'v':		// "-v"  Vi-mode (as if called "vi")
		exmode_active = 0;
# எனில் defined(FEAT_GUI) && !defined(VIMDLL)
		gui.starting = false;	// don't start GUI
# endif
		நிறுத்து;

	    நிலை 'w':		// "-w{number}"	set window height
				// "-w {scriptout}"	write to script
		எனில் (vim_isdigit(((char_u *)argv[0])[argv_idx]))
		{
		    n = get_எண்_arg((char_u *)argv[0], &argv_idx, 10);
		    set_option_மதிப்பு_give_err((char_u *)"window", n, வெற்று, 0);
		    நிறுத்து;
		}
		want_argument = TRUE;
		நிறுத்து;

# ifdef FEAT_CRYPT
	    நிலை 'x':		// "-x"  encrypted reading/writing of files
		parmp->ask_for_சாவி = TRUE;
		நிறுத்து;
# endif

	    நிலை 'X':		// "-X"  don't connect to X server
# எனில் (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
		x_no_connect = TRUE;
# endif
		நிறுத்து;
	    நிலை 'Y':		// "-Y" don't connect to Wayland compositor
# எனில் defined(FEAT_WAYLAND)
		wayland_no_connect = TRUE;
# endif
		நிறுத்து;

	    நிலை 'Z':		// "-Z"  restricted mode
		restricted = TRUE;
		நிறுத்து;

	    நிலை 'c':		// "-c{command}" or "-c {command}" execute
				// command
		எனில் (argv[0][argv_idx] != NUL)
		{
		    எனில் (parmp->n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, வெற்று);
		    parmp->commands[parmp->n_commands++] = (char_u *)argv[0]
								   + argv_idx;
		    argv_idx = -1;
		    நிறுத்து;
		}
		// FALLTHROUGH
	    நிலை 'S':		// "-S {file}" execute Vim script
	    நிலை 'i':		// "-i {viminfo}" use for viminfo
# ifndef FEAT_DIFF
	    நிலை 'd':		// "-d {device}" device (for Amiga)
# endif
	    நிலை 'T':		// "-T {terminal}" terminal name
	    நிலை 'u':		// "-u {vimrc}" vim inits file
	    நிலை 'U':		// "-U {gvimrc}" gvim inits file
	    நிலை 'W':		// "-W {scriptout}" overwrite
# ifdef FEAT_GUI_MSWIN
	    நிலை 'P':		// "-P {parent title}" MDI parent
# endif
		want_argument = TRUE;
		நிறுத்து;

	    இயல்பு:
		mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
	    }

	    /*
	     * Handle option arguments with argument.
	     */
	    எனில் (want_argument)
	    {
		/*
		 * Check for garbage immediately after the option letter.
		 */
		எனில் (argv[0][argv_idx] != NUL)
		    mainerr(ME_GARBAGE, (char_u *)argv[0]);

		--argc;
		எனில் (argc < 1 && c != 'S')  // -S has an optional argument
		    mainerr_arg_missing((char_u *)argv[0]);
		++argv;
		argv_idx = -1;

		தேர்வு (c)
		{
		நிலை 'c':	// "-c {command}" execute command
		நிலை 'S':	// "-S {file}" execute Vim script
		    எனில் (parmp->n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, வெற்று);
		    எனில் (c == 'S')
		    {
			எழுத்து	*a;

			எனில் (argc < 1)
			    // "-S" without argument: use default session file
			    // name.
			    a = SESSION_FILE;
			இல்லையெனில் எனில் (argv[0][0] == '-')
			{
			    // "-S" followed by another option: use default
			    // session file name.
			    a = SESSION_FILE;
			    ++argc;
			    --argv;
			}
			இல்லையெனில்
			    a = argv[0];
			p = alloc(STRLEN(a) + 4);
			எனில் (p == வெற்று)
			    mch_exit(2);
			sprintf((எழுத்து *)p, "so %s", a);
			parmp->cmds_tofree[parmp->n_commands] = TRUE;
			parmp->commands[parmp->n_commands++] = p;
		    }
		    இல்லையெனில்
			parmp->commands[parmp->n_commands++] =
							    (char_u *)argv[0];
		    நிறுத்து;

		நிலை '-':
		    எனில் (argv[-1][2] == 'c')
		    {
			// "--cmd {command}" execute command
			எனில் (parmp->n_pre_commands >= MAX_ARG_CMDS)
			    mainerr(ME_EXTRA_CMD, வெற்று);
			parmp->pre_commands[parmp->n_pre_commands++] =
							    (char_u *)argv[0];
		    }
		    // --gui-dialog-file fname
		    எனில் (argv[-1][2] == 'g')
		    {
			// without GUI ignore the argument
# ifdef FEAT_GUI
			parmp->gui_dialog_file = (char_u *)argv[0];
# endif
		    }

		    // "--startuptime <file>" already handled
		    // "--log <file>" already handled
		    நிறுத்து;

	    //	case 'd':   -d {device} is handled in mch_check_win() for the
	    //		    Amiga

# ifdef FEAT_QUICKFIX
		நிலை 'q':	// "-q {errorfile}" QuickFix mode
		    parmp->use_ef = (char_u *)argv[0];
		    நிறுத்து;
# endif

		நிலை 'i':	// "-i {viminfo}" use for viminfo
		    set_option_மதிப்பு_give_err((char_u *)"vif",
						     0L, (char_u *)argv[0], 0);
		    நிறுத்து;

		நிலை 's':	// "-s {scriptin}" read from script file
		    எனில் (scriptin[0] != வெற்று)
		    {
scripterror:
			mch_errmsg(_("Attempt to open script file again: \""));
			mch_errmsg(argv[-1]);
			mch_errmsg(" ");
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    எனில் ((scriptin[0] = mch_fopen(argv[0], READBIN)) == வெற்று)
		    {
			mch_errmsg(_("Cannot open for reading: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    எனில் (save_typebuf() == FAIL)
			mch_exit(2);	// out of memory
		    நிறுத்து;

		நிலை 't':	// "-t {tag}"
		    parmp->tagname = (char_u *)argv[0];
		    நிறுத்து;

		நிலை 'T':	// "-T {terminal}" terminal name
		    /*
		     * The -T term argument is always available and when
		     * HAVE_TERMLIB is supported it overrides the environment
		     * variable TERM.
		     */
# ifdef FEAT_GUI
		    எனில் (term_is_gui((char_u *)argv[0]))
			gui.starting = true;	// start GUI a bit later
		    இல்லையெனில்
# endif
			parmp->term = (char_u *)argv[0];
		    நிறுத்து;

		நிலை 'u':	// "-u {vimrc}" vim inits file
		    parmp->use_vimrc = (char_u *)argv[0];
		    நிறுத்து;

		நிலை 'U':	// "-U {gvimrc}" gvim inits file
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)argv[0];
# endif
		    நிறுத்து;

		நிலை 'w':	// "-w {nr}" 'window' value
				// "-w {scriptout}" append to script file
		    எனில் (vim_isdigit(*((char_u *)argv[0])))
		    {
			argv_idx = 0;
			n = get_எண்_arg((char_u *)argv[0], &argv_idx, 10);
			set_option_மதிப்பு_give_err((char_u *)"window",
								   n, வெற்று, 0);
			argv_idx = -1;
			நிறுத்து;
		    }
		    // FALLTHROUGH
		நிலை 'W':	// "-W {scriptout}" overwrite script file
		    எனில் (scriptout != வெற்று)
			செல் scripterror;
		    எனில் ((scriptout = mch_fopen(argv[0],
				    c == 'w' ? APPENDBIN : WRITEBIN)) == வெற்று)
		    {
			mch_errmsg(_("Cannot open for script output: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    நிறுத்து;

# ifdef FEAT_GUI_MSWIN
		நிலை 'P':		// "-P {parent title}" MDI parent
		    gui_mch_set_parent(argv[0]);
		    நிறுத்து;
# endif
		}
	    }
	}

	/*
	 * File name argument.
	 */
	இல்லையெனில்
	{
	    argv_idx = -1;	    // skip to next argument

	    // Check for only one type of editing.
	    எனில் (parmp->edit_type != EDIT_NONE && parmp->edit_type != EDIT_FILE)
		mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
	    parmp->edit_type = EDIT_FILE;

# ifdef MSWIN
	    // Remember if the argument was a full path before changing
	    // slashes to backslashes.
	    எனில் (argv[0][0] != NUL && argv[0][1] == ':' && argv[0][2] == '\\')
		parmp->full_path = TRUE;
# endif

	    // Add the file to the global argument list.
	    எனில் (ga_grow(&global_alist.al_ga, 1) == FAIL
		    || (p = vim_strsave((char_u *)argv[0])) == வெற்று)
		mch_exit(2);
# ifdef FEAT_DIFF
	    எனில் (parmp->diff_mode && mch_isdir(p) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *tail;
		string_T    ret;

		tail = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(p, STRLEN(p), tail, STRLEN(tail), TRUE, &ret);
		எனில் (ret.string != வெற்று)
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
	    எனில் (vim_strpbrk(p, "\\:") != வெற்று && !path_with_url(p))
	    {
		எழுத்து posix_path[MAXPATHL];

#  எனில் CYGWIN_VERSION_DLL_MAJOR >= 1007
		cygwin_conv_path(CCP_WIN_A_TO_POSIX, p, posix_path, MAXPATHL);
#  இல்லையெனில்
		cygwin_conv_to_posix_path(p, posix_path);
#  endif
		vim_free(p);
		p = vim_strsave((char_u *)posix_path);
		எனில் (p == வெற்று)
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
# இல்லையெனில்
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
#  இல்லையெனில்
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
	எனில் (argv_idx <= 0 || argv[0][argv_idx] == NUL)
	{
	    --argc;
	    ++argv;
	    argv_idx = 1;
	}
    }

# ifdef FEAT_EVAL
    // If there is a "+123" or "-c" command, set v:swapcommand to the first
    // one.
    எனில் (parmp->n_commands > 0)
    {
	size_t  plen;

	plen = STRLEN(parmp->commands[0]) + 2;
	p = alloc(plen + 1);
	எனில் (p != வெற்று)
	{
	    sprintf((எழுத்து *)p, ":%s\r", parmp->commands[0]);
	    set_vim_var_string(VV_SWAPCOMMAND, p, (முழு)plen);
	    vim_free(p);
	}
    }
# endif
}

/*
 * Print a warning if stdout is not a terminal.
 * When starting in Ex mode and commands come from a file, set silent_mode.
 */
    நிலையான வெறுமை
check_tty(mparm_T *parmp)
{
    முழு		உள்ளீடு_isatty;		// is active input a terminal?

    உள்ளீடு_isatty = mch_உள்ளீடு_isatty();
    எனில் (exmode_active)
    {
	எனில் (!உள்ளீடு_isatty)
	    silent_mode = TRUE;
    }
    இல்லையெனில் எனில் (parmp->want_full_screen && (!stdout_isatty || !உள்ளீடு_isatty)
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
	எனில் (netbeans_active() && (!stdout_isatty || !உள்ளீடு_isatty))
	{
	    mch_errmsg(_("Vim: Error: Failure to start gvim from NetBeans\n"));
	    exit(1);
	}
# endif
# எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	எனில் (
#  ifdef VIMDLL
	    !gui.starting &&
#  endif
	    is_cygpty_used())
	{
#  எனில் defined(HAVE_BIND_TEXTDOMAIN_CODESET) \
	&& defined(FEAT_GETTEXT)
	    எழுத்து    *s, *tofree = வெற்று;

	    // Set the encoding of the error message based on $LC_ALL or
	    // other environment variables instead of 'encoding'.
	    // Note that the message is shown on a Cygwin terminal (e.g.
	    // mintty) which encoding is based on $LC_ALL or etc., not the
	    // current codepage used by normal Win32 console programs.
	    tofree = s = (எழுத்து *)enc_locale_env(வெற்று);
	    எனில் (s == வெற்று)
		s = "utf-8";	// Use "utf-8" by default.
	    (வெறுமை)bind_textdomain_codeset(VIMPACKAGE, s);
	    vim_free(tofree);
#  endif
	    mch_errmsg(_("Vim: Error: This version of Vim does not run in a Cygwin terminal\n"));
	    exit(1);
	}
# endif
	எனில் (!stdout_isatty)
	    mch_errmsg(_("Vim: Warning: Output is not to a terminal\n"));
	எனில் (!உள்ளீடு_isatty)
	    mch_errmsg(_("Vim: Warning: Input is not from a terminal\n"));
	out_flush();
	எனில் (parmp->tty_fail && (!stdout_isatty || !உள்ளீடு_isatty))
	    exit(1);
	எனில் (scriptin[0] == வெற்று)
	    ui_delay(2005L, TRUE);
	TIME_செய்தி("Warning delay");
    }
}

/*
 * Read text from stdin.
 */
    நிலையான வெறுமை
read_stdin(வெறுமை)
{
    முழு	    i;

    // When getting the ATTENTION prompt here, use a dialog
    swap_exists_action = SEA_DIALOG;

    no_wait_return = TRUE;
    i = செய்தி_didany;
    set_buflisted(TRUE);

    // Create memfile and read from stdin.
    (வெறுமை)open_buffer(TRUE, வெற்று, 0);

    no_wait_return = FALSE;
    செய்தி_didany = i;
    TIME_செய்தி("reading stdin");

    check_swap_exists_action();

# எனில் !(defined(AMIGA) || defined(MACOS_X))
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
    நிலையான வெறுமை
create_windows(mparm_T *parmp UNUSED)
{
    முழு		dorewind;
    முழு		done = 0;

    /*
     * Create the number of windows that was requested.
     */
    எனில் (parmp->window_மொத்தம் == -1)	// was not set
	parmp->window_மொத்தம் = 1;
    எனில் (parmp->window_மொத்தம் == 0)
	parmp->window_மொத்தம் = GARGCOUNT;
    எனில் (parmp->window_மொத்தம் > 1)
    {
	// Don't change the windows if there was a command in .vimrc that
	// already split some windows
	எனில் (parmp->window_layout == 0)
	    parmp->window_layout = WIN_HOR;
	எனில் (parmp->window_layout == WIN_TABS)
	{
	    parmp->window_மொத்தம் = make_tabpages(parmp->window_மொத்தம்);
	    TIME_செய்தி("making tab pages");
	}
	இல்லையெனில் எனில் (firstwin->w_next == வெற்று)
	{
	    parmp->window_மொத்தம் = make_windows(parmp->window_மொத்தம்,
					     parmp->window_layout == WIN_VER);
	    TIME_செய்தி("making windows");
	}
	இல்லையெனில்
	    parmp->window_மொத்தம் = win_மொத்தம்();
    }
    இல்லையெனில்
	parmp->window_மொத்தம் = 1;

    எனில் (recoverymode)			// do recover
    {
	செய்தி_scroll = TRUE;		// scroll message up
	ml_recover(TRUE);
	எனில் (curbuf->b_ml.ml_mfp == வெற்று) // failed
	    getout(1);
	do_modelines(0);		// do modelines
    }
    இல்லையெனில்
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
	போது (done++ < 1000)
	{
	    எனில் (dorewind)
	    {
		எனில் (parmp->window_layout == WIN_TABS)
		    goto_tabpage(1);
		இல்லையெனில்
		    curwin = firstwin;
	    }
	    இல்லையெனில் எனில் (parmp->window_layout == WIN_TABS)
	    {
		எனில் (curtab->tp_next == வெற்று)
		    நிறுத்து;
		goto_tabpage(0);
	    }
	    இல்லையெனில்
	    {
		எனில் (curwin->w_next == வெற்று)
		    நிறுத்து;
		curwin = curwin->w_next;
	    }
	    dorewind = FALSE;
	    curbuf = curwin->w_buffer;
	    எனில் (curbuf->b_ml.ml_mfp == வெற்று)
	    {
# ifdef FEAT_FOLDING
		// Set 'foldlevel' to 'foldlevelstart' if it's not negative.
		எனில் (p_fdls >= 0)
		    curwin->w_p_fdl = p_fdls;
# endif
		// When getting the ATTENTION prompt here, use a dialog
		swap_exists_action = SEA_DIALOG;

		set_buflisted(TRUE);

		// create memfile, read file
		(வெறுமை)open_buffer(FALSE, வெற்று, 0);

		எனில் (swap_exists_action == SEA_QUIT)
		{
		    எனில் (got_int || only_one_window())
		    {
			// abort selected or quit and only one window
			did_emsg = FALSE;   // avoid hit-enter prompt
			getout(1);
		    }
		    // We can't close the window, it would disturb what
		    // happens next.  Clear the file name and set the arg
		    // index to -1 to delete it later.
		    setfname(curbuf, வெற்று, வெற்று, FALSE);
		    curwin->w_arg_idx = -1;
		    swap_exists_action = SEA_NONE;
		}
		இல்லையெனில்
		    handle_swap_exists(வெற்று);
		dorewind = TRUE;		// start again
	    }
	    ui_breakcheck();
	    எனில் (got_int)
	    {
		(வெறுமை)vgetc();	// only break the file loading, not the rest
		நிறுத்து;
	    }
	}
	எனில் (parmp->window_layout == WIN_TABS)
	    goto_tabpage(1);
	இல்லையெனில்
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
    நிலையான வெறுமை
edit_buffers(
    mparm_T	*parmp,
    char_u	*cwd)			// current working dir
{
    முழு		arg_idx;		// index in argument list
    முழு		i;
    முழு		advance = TRUE;
    win_T	*win;
    char_u	*p_shm_save = வெற்று;

    /*
     * Don't execute Win/Buf Enter/Leave autocommands here
     */
    ++autocmd_no_enter;
    ++autocmd_no_leave;

    // When w_arg_idx is -1 remove the window (see create_windows()).
    எனில் (curwin->w_arg_idx == -1)
    {
	win_close(curwin, TRUE);
	advance = FALSE;
    }

    arg_idx = 1;
    க்கு (i = 1; i < parmp->window_மொத்தம்; ++i)
    {
	எனில் (cwd != வெற்று)
	    mch_chdir((எழுத்து *)cwd);
	// When w_arg_idx is -1 remove the window (see create_windows()).
	எனில் (curwin->w_arg_idx == -1)
	{
	    ++arg_idx;
	    win_close(curwin, TRUE);
	    advance = FALSE;
	    தொடர்;
	}

	எனில் (advance)
	{
	    எனில் (parmp->window_layout == WIN_TABS)
	    {
		எனில் (curtab->tp_next == வெற்று)	// just checking
		    நிறுத்து;
		goto_tabpage(0);
		// Temporarily reset 'shm' option to not print fileinfo when
		// loading the other buffers. This would overwrite the already
		// existing fileinfo for the first tab.
		எனில் (i == 1)
		{
		    எழுத்து buf[100];

		    p_shm_save = vim_strsave(p_shm);
		    vim_snprintf(buf, 100, "F%s", p_shm);
		    set_option_மதிப்பு_give_err((char_u *)"shm",
							 0L, (char_u *)buf, 0);
		}
	    }
	    இல்லையெனில்
	    {
		எனில் (curwin->w_next == வெற்று)	// just checking
		    நிறுத்து;
		win_enter(curwin->w_next, FALSE);
	    }
	}
	advance = TRUE;

	// Only open the file if there is no file in this window yet (that can
	// happen when .vimrc contains ":sall").
	எனில் (curbuf == firstwin->w_buffer || curbuf->b_ffname == வெற்று)
	{
	    curwin->w_arg_idx = arg_idx;
	    // Edit file from arg list, if there is one.  When "Quit" selected
	    // at the ATTENTION prompt close the window.
	    swap_exists_did_quit = FALSE;
	    (வெறுமை)do_ecmd(0, arg_idx < GARGCOUNT
			  ? alist_name(&GARGLIST[arg_idx]) : வெற்று,
			  வெற்று, வெற்று, ECMD_LASTL, ECMD_HIDE, curwin);
	    எனில் (swap_exists_did_quit)
	    {
		// abort or quit selected
		எனில் (got_int || only_one_window())
		{
		    // abort selected and only one window
		    did_emsg = FALSE;   // avoid hit-enter prompt
		    getout(1);
		}
		win_close(curwin, TRUE);
		advance = FALSE;
	    }
	    எனில் (arg_idx == GARGCOUNT - 1)
		arg_had_last = TRUE;
	    ++arg_idx;
	}
	ui_breakcheck();
	எனில் (got_int)
	{
	    (வெறுமை)vgetc();	// only break the file loading, not the rest
	    நிறுத்து;
	}
    }

    எனில் (p_shm_save != வெற்று)
    {
	set_option_மதிப்பு_give_err((char_u *)"shm", 0L, p_shm_save, 0);
	vim_free(p_shm_save);
    }

    எனில் (parmp->window_layout == WIN_TABS)
	goto_tabpage(1);
    --autocmd_no_enter;

    // make the first window the current window
    win = firstwin;
# எனில் defined(FEAT_QUICKFIX)
    // Avoid making a preview window the current window.
    போது (win->w_p_pvw)
    {
	win = win->w_next;
	எனில் (win == வெற்று)
	{
	    win = firstwin;
	    நிறுத்து;
	}
    }
# endif
    win_enter(win, FALSE);

    --autocmd_no_leave;
    TIME_செய்தி("editing files in windows");
    எனில் (parmp->window_மொத்தம் > 1 && parmp->window_layout != WIN_TABS)
	win_equal(curwin, FALSE, 'b');	// adjust heights
}

/*
 * Execute the commands from --cmd arguments "cmds[cnt]".
 */
    நிலையான வெறுமை
exe_pre_commands(mparm_T *parmp)
{
    char_u	**cmds = parmp->pre_commands;
    முழு		cnt = parmp->n_pre_commands;
    முழு		i;
    ESTACK_CHECK_DECLARATION;

    எனில் (cnt <= 0)
	திரும்பு;

    curwin->w_cursor.lnum = 0; // just in case..
    estack_தள்ளு(ETYPE_ARGS, (char_u *)_("pre-vimrc command line"), 0);
    ESTACK_CHECK_SETUP;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CMDARG;
# endif
    க்கு (i = 0; i < cnt; ++i)
	do_cmdline_cmd(cmds[i]);
    ESTACK_CHECK_NOW;
    estack_எடு();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    TIME_செய்தி("--cmd commands");
}

/*
 * Execute "+", "-c" and "-S" arguments.
 */
    நிலையான வெறுமை
exe_commands(mparm_T *parmp)
{
    முழு		i;
    ESTACK_CHECK_DECLARATION;

    /*
     * We start commands on line 0, make "vim +/pat file" match a
     * pattern on line 1.  But don't move the cursor when an autocommand
     * with g`" was used.
     */
    செய்தி_scroll = TRUE;
    எனில் (parmp->tagname == வெற்று && curwin->w_cursor.lnum <= 1)
	curwin->w_cursor.lnum = 0;
    estack_தள்ளு(ETYPE_ARGS, (char_u *)"command line", 0);
    ESTACK_CHECK_SETUP;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CARG;
    current_sctx.sc_seq = 0;
# endif
    க்கு (i = 0; i < parmp->n_commands; ++i)
    {
	do_cmdline_cmd(parmp->commands[i]);
	எனில் (parmp->cmds_tofree[i])
	    vim_free(parmp->commands[i]);
    }
    ESTACK_CHECK_NOW;
    estack_எடு();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    எனில் (curwin->w_cursor.lnum == 0)
	curwin->w_cursor.lnum = 1;

    எனில் (!exmode_active)
	செய்தி_scroll = FALSE;

# ifdef FEAT_QUICKFIX
    // When started with "-q errorfile" jump to first error again.
    எனில் (parmp->edit_type == EDIT_QF)
	qf_jump(வெற்று, 0, 0, FALSE);
# endif
    TIME_செய்தி("executing command arguments");
}

/*
 * Source startup scripts.
 */
    நிலையான வெறுமை
source_startup_scripts(mparm_T *parmp)
{
    முழு		i;

    /*
     * For "evim" source evim.vim first of all, so that the user can overrule
     * any things he doesn't like.
     */
    எனில் (parmp->evim_mode)
    {
	(வெறுமை)do_source((char_u *)EVIM_FILE, FALSE, DOSO_NONE, வெற்று);
	TIME_செய்தி("source evim file");
    }

    /*
     * If -u argument given, use only the initializations from that file and
     * nothing else.
     */
    எனில் (parmp->use_vimrc != வெற்று)
    {
	எனில் (STRCMP(parmp->use_vimrc, "DEFAULTS") == 0)
	{
	    எனில் (do_source((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE, வெற்று)
									 != OK)
		emsg(_(e_failed_to_source_defaults));
	}
	இல்லையெனில் எனில் (STRCMP(parmp->use_vimrc, "NONE") == 0
				     || STRCMP(parmp->use_vimrc, "NORC") == 0)
	{
# ifdef FEAT_GUI
	    எனில் (use_gvimrc == வெற்று)	    // don't load gvimrc either
		use_gvimrc = parmp->use_vimrc;
# endif
	}
	இல்லையெனில்
	{
	    எனில் (do_source(parmp->use_vimrc, FALSE, DOSO_NONE, வெற்று) != OK)
		semsg(_(e_cannot_read_from_str_2), parmp->use_vimrc);
	}
    }
    இல்லையெனில் எனில் (!silent_mode)
    {
# ifdef AMIGA
	கட்டமைப்பு Process	*proc = (கட்டமைப்பு Process *)FindTask(0L);
	APTR		save_winptr = proc->pr_WindowPtr;

	// Avoid a requester here for a volume that doesn't exist.
	proc->pr_WindowPtr = (APTR)-1L;
# endif

	/*
	 * Get system wide defaults, if the file name is defined.
	 */
# ifdef SYS_VIMRC_FILE
	(வெறுமை)do_source((char_u *)SYS_VIMRC_FILE, FALSE, DOSO_NONE, வெற்று);
# endif
# ifdef MACOS_X
	(வெறுமை)do_source((char_u *)"$VIMRUNTIME/macmap.vim", FALSE,
							      DOSO_NONE, வெற்று);
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
	எனில் (process_env((char_u *)"VIMINIT", TRUE) != OK)
	{
	    எனில் (do_source((char_u *)USR_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, வெற்று) == FAIL
# ifdef USR_VIMRC_FILE2
		&& do_source((char_u *)USR_VIMRC_FILE2, TRUE,
						      DOSO_VIMRC, வெற்று) == FAIL
# endif
# ifdef XDG_VIMRC_FILE
		&& do_source((char_u *)XDG_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, வெற்று) == FAIL
# endif
# ifdef USR_VIMRC_FILE3
		&& do_source((char_u *)USR_VIMRC_FILE3, TRUE,
						      DOSO_VIMRC, வெற்று) == FAIL
# endif
# ifdef USR_VIMRC_FILE4
		&& do_source((char_u *)USR_VIMRC_FILE4, TRUE,
						      DOSO_VIMRC, வெற்று) == FAIL
# endif
		&& process_env((char_u *)"EXINIT", FALSE) == FAIL
		&& do_source((char_u *)USR_EXRC_FILE, FALSE,
						       DOSO_NONE, வெற்று) == FAIL
# ifdef USR_EXRC_FILE2
		&& do_source((char_u *)USR_EXRC_FILE2, FALSE,
						       DOSO_NONE, வெற்று) == FAIL
# endif
		&& !has_dash_c_arg)
	    {
		// When no .vimrc file was found: source defaults.vim.
		எனில் (do_source((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE,
								 வெற்று) == FAIL)
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
	எனில் (p_exrc)
	{
# எனில் defined(UNIX) || defined(VMS)
	    // If ".vimrc" file is not owned by user, set 'secure' mode.
	    எனில் (!file_owned(VIMRC_FILE))
# endif
		secure = p_secure;

	    i = FAIL;
	    எனில் (fullpathcmp((char_u *)USR_VIMRC_FILE,
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
		i = do_source((char_u *)VIMRC_FILE, TRUE, DOSO_VIMRC, வெற்று);

	    எனில் (i == FAIL)
	    {
# எனில் defined(UNIX) || defined(VMS)
		// if ".exrc" is not owned by user set 'secure' mode
		எனில் (!file_owned(EXRC_FILE))
		    secure = p_secure;
		இல்லையெனில்
		    secure = 0;
# endif
		எனில் (	   fullpathcmp((char_u *)USR_EXRC_FILE,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# ifdef USR_EXRC_FILE2
			&& fullpathcmp((char_u *)USR_EXRC_FILE2,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# endif
				)
		    (வெறுமை)do_source((char_u *)EXRC_FILE, FALSE,
							      DOSO_NONE, வெற்று);
	    }
	}
	எனில் (secure == 2)
	    need_wait_return = TRUE;
	secure = 0;
# ifdef AMIGA
	proc->pr_WindowPtr = save_winptr;
# endif
    }
    TIME_செய்தி("sourcing vimrc file(s)");
}

/*
 * Setup to start using the GUI.  Exit with an error when not available.
 */
    நிலையான வெறுமை
main_start_gui(வெறுமை)
{
# ifdef FEAT_GUI
    gui.starting = true;	// start GUI a bit later
# இல்லையெனில்
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
    முழு
process_env(
    char_u	*env,
    முழு		is_viminit) // when TRUE, called for VIMINIT
{
    char_u	*initstr;
    sctx_T	save_current_sctx;
    ESTACK_CHECK_DECLARATION;

    எனில் ((initstr = mch_getenv(env)) == வெற்று || *initstr == NUL)
	திரும்பு FAIL;

    எனில் (is_viminit)
	vimrc_found(வெற்று, வெற்று);
    estack_தள்ளு(ETYPE_ENV, env, 0);
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
    estack_எடு();
    current_sctx = save_current_sctx;
    திரும்பு OK;
}

#எனில் (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
/*
 * Return TRUE if we are certain the user owns the file "fname".
 * Used for ".vimrc" and ".exrc".
 * Use both stat() and lstat() for extra security.
 */
    நிலையான முழு
file_owned(எழுத்து *fname)
{
    stat_T	s;
# ifdef UNIX
    uid_t	uid = getuid();
# இல்லையெனில்	 // VMS
    uid_t	uid = ((getgid() << 16) | getuid());
# endif

    திரும்பு !(mch_stat(fname, &s) != 0 || s.st_uid != uid
# ifdef HAVE_LSTAT
	    || mch_lstat(fname, &s) != 0 || s.st_uid != uid
# endif
	    );
}
#endif

/*
 * Give an error message main_errors["n"] and exit.
 */
    நிலையான வெறுமை
mainerr(
    முழு		n,	// one of the ME_ defines
    char_u	*str)	// extra argument or NULL
{
#எனில் defined(UNIX) || defined(VMS)
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
    எனில் (str != வெற்று)
    {
	mch_errmsg(": \"");
	mch_errmsg((எழுத்து *)str);
	mch_errmsg("\"");
    }
    mch_errmsg(_("\nMore info with: \"vim -h\"\n"));

    mch_exit(1);
}

    வெறுமை
mainerr_arg_missing(char_u *str)
{
    mainerr(ME_ARG_MISSING, str);
}

#ifndef NO_VIM_MAIN
/*
 * print a message with three spaces prepended and '\n' appended.
 */
    நிலையான வெறுமை
main_செய்தி(எழுத்து *s)
{
    mch_செய்தி("   ");
    mch_செய்தி(s);
    mch_செய்தி("\n");
}

/*
 * Print messages for "vim -h" or "vim --help" and exit.
 */
    நிலையான வெறுமை
usage(வெறுமை)
{
    முழு		i;
    நிலையான எழுத்து	*(use[]) =
    {
	N_("[file ..]       edit specified file(s)"),
	N_("-               read text from stdin"),
	N_("-t tag          edit file where tag is defined"),
# ifdef FEAT_QUICKFIX
	N_("-q [errorfile]  edit file with first error")
# endif
    };

# எனில் defined(UNIX) || defined(VMS)
    reset_signals();		// kill us with CTRL-C here, if you like
# endif

    init_longVersion();
    mch_செய்தி(longVersion);
    mch_செய்தி(_("\n\nUsage:"));
    க்கு (i = 0; ; ++i)
    {
	mch_செய்தி(_(" vim [arguments] "));
	mch_செய்தி(_(use[i]));
	எனில் (i == ARRAY_LENGTH(use) - 1)
	    நிறுத்து;
	mch_செய்தி(_("\n   or:"));
    }
# எனில் defined( VMS)
    mch_செய்தி(_("\nWhere command is down-cased, prepend / (like: -/R) to treat flag as upper-case."));
    mch_செய்தி(_("\nOr, where supported, SET PROC/PARSE=EXT, or else quote upper-case material."));
# endif /* defined( VMS) */

    mch_செய்தி(_("\n\nArguments:\n"));
    main_செய்தி(_("--\t\t\tOnly file names after this"));
# ifdef EXPAND_FILENAMES
    main_செய்தி(_("--literal\t\tDon't expand wildcards"));
# endif
# ifdef FEAT_OLE
    main_செய்தி(_("-register\t\tRegister this gvim for OLE"));
    main_செய்தி(_("-unregister\t\tUnregister gvim for OLE"));
# endif
# ifdef FEAT_GUI
    main_செய்தி(_("-g\t\t\tRun using GUI (like \"gvim\")"));
    main_செய்தி(_("-f  or  --nofork\tForeground: Don't fork when starting GUI"));
# endif
    main_செய்தி(_("-v\t\t\tVi mode (like \"vi\")"));
    main_செய்தி(_("-e\t\t\tEx mode (like \"ex\")"));
    main_செய்தி(_("-E\t\t\tImproved Ex mode"));
    main_செய்தி(_("-s\t\t\tSilent (batch) mode (only for \"ex\")"));
# ifdef FEAT_DIFF
    main_செய்தி(_("-d\t\t\tDiff mode (like \"vimdiff\")"));
# endif
    main_செய்தி(_("-y\t\t\tEasy mode (like \"evim\", modeless)"));
    main_செய்தி(_("-R\t\t\tReadonly mode (like \"view\")"));
    main_செய்தி(_("-Z\t\t\tRestricted mode (like \"rvim\")"));
    main_செய்தி(_("-m\t\t\tModifications (writing files) not allowed"));
    main_செய்தி(_("-M\t\t\tModifications in text not allowed"));
    main_செய்தி(_("-b\t\t\tBinary mode"));
    main_செய்தி(_("-l\t\t\tLisp mode"));
    main_செய்தி(_("-C\t\t\tCompatible with Vi: 'compatible'"));
    main_செய்தி(_("-N\t\t\tNot fully Vi compatible: 'nocompatible'"));
    main_செய்தி(_("-V[N][fname]\t\tBe verbose [level N] [log messages to fname]"));
# ifdef FEAT_EVAL
    main_செய்தி(_("-D\t\t\tDebugging mode"));
# endif
    main_செய்தி(_("-n\t\t\tNo swap file, use memory only"));
    main_செய்தி(_("-r\t\t\tList swap files and exit"));
    main_செய்தி(_("-r (with file name)\tRecover crashed session"));
    main_செய்தி(_("-L\t\t\tSame as -r"));
# ifdef AMIGA
    main_செய்தி(_("-f\t\t\tDon't use newcli to open window"));
    main_செய்தி(_("-dev <device>\t\tUse <device> for I/O"));
# endif
# ifdef FEAT_ARABIC
    main_செய்தி(_("-A\t\t\tStart in Arabic mode"));
# endif
# ifdef FEAT_RIGHTLEFT
    main_செய்தி(_("-H\t\t\tStart in Hebrew mode"));
# endif
    main_செய்தி(_("-T <terminal>\tSet terminal type to <terminal>"));
    main_செய்தி(_("--not-a-term\t\tSkip warning for input/output not being a terminal"));
# ifdef FEAT_GUI
    main_செய்தி(_("--gui-dialog-file {fname}  For testing: write dialog text"));
# endif
    main_செய்தி(_("--ttyfail\t\tExit if input or output is not a terminal"));
# ifdef FEAT_JOB_CHANNEL
    main_செய்தி(_("--stdio-channel\tUse stdin and stdout as a channel"));
# endif
    main_செய்தி(_("-u <vimrc>\t\tUse <vimrc> instead of any .vimrc"));
# ifdef FEAT_GUI
    main_செய்தி(_("-U <gvimrc>\t\tUse <gvimrc> instead of any .gvimrc"));
# endif
    main_செய்தி(_("--noplugin\t\tDon't load plugin scripts"));
    main_செய்தி(_("-p[N]\t\tOpen N tab pages (default: one for each file)"));
    main_செய்தி(_("-o[N]\t\tOpen N windows (default: one for each file)"));
    main_செய்தி(_("-O[N]\t\tLike -o but split vertically"));
    main_செய்தி(_("+\t\t\tStart at end of file"));
    main_செய்தி(_("+<lnum>\t\tStart at line <lnum>"));
    main_செய்தி(_("--cmd <command>\tExecute <command> before loading any vimrc file"));
    main_செய்தி(_("-c <command>\t\tExecute <command> after loading the first file"));
    main_செய்தி(_("-S <session>\t\tSource file <session> after loading the first file"));
    main_செய்தி(_("-s <scriptin>\tRead Normal mode commands from file <scriptin>"));
    main_செய்தி(_("-w <scriptout>\tAppend all typed commands to file <scriptout>"));
    main_செய்தி(_("-W <scriptout>\tWrite all typed commands to file <scriptout>"));
# ifdef FEAT_CRYPT
    main_செய்தி(_("-x\t\t\tEdit encrypted files"));
# endif
# எனில் (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
#  எனில் defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
    main_செய்தி(_("-display <display>\tConnect Vim to this particular X-server"));
#  endif
    main_செய்தி(_("-X\t\t\tDo not connect to X server"));
# endif
# எனில் defined(FEAT_WAYLAND)
    main_செய்தி(_("-Y\t\t\tDo not connect to Wayland compositor"));
# endif
# ifdef FEAT_CLIENTSERVER
#  ifdef FEAT_CLIENTSERVER_BACKENDS
    main_செய்தி(_("--clientserver <socket|x11|mswin> Backend for clientserver communication"));
#  endif
    main_செய்தி(_("--remote <files>\tEdit <files> in a Vim server if possible"));
    main_செய்தி(_("--remote-silent <files>  Same, don't complain if there is no server"));
    main_செய்தி(_("--remote-wait <files>  As --remote but wait for files to have been edited"));
    main_செய்தி(_("--remote-wait-silent <files>  Same, don't complain if there is no server"));
    main_செய்தி(_("--remote-tab[-wait][-silent] <files>  As --remote but use tab page per file"));
    main_செய்தி(_("--remote-send <keys>\tSend <keys> to a Vim server and exit"));
    main_செய்தி(_("--remote-expr <expr>\tEvaluate <expr> in a Vim server and print result"));
    main_செய்தி(_("--serverlist\t\tList available Vim server names and exit"));
    main_செய்தி(_("--servername <name>\tSend to/become the Vim server <name>"));
# endif
# ifdef STARTUPTIME
    main_செய்தி(_("--startuptime <file>\tWrite startup timing messages to <file>"));
# endif
# ifdef FEAT_JOB_CHANNEL
    main_செய்தி(_("--log <file>\t\tStart logging to <file> early"));
# endif
# ifdef FEAT_VIMINFO
    main_செய்தி(_("-i <viminfo>\t\tUse <viminfo> instead of .viminfo"));
# endif
    main_செய்தி(_("--clean\t\t'nocompatible', Vim defaults, no plugins, no viminfo"));
    main_செய்தி(_("-h  or  --help\tPrint Help (this message) and exit"));
    main_செய்தி(_("--version\t\tPrint version information and exit"));

# ifdef FEAT_GUI_X11
#  ifdef FEAT_GUI_MOTIF
    mch_செய்தி(_("\nArguments recognised by gvim (Motif version):\n"));
#  endif
    main_செய்தி(_("-display <display>\tRun Vim on <display>"));
    main_செய்தி(_("-iconic\t\tStart Vim iconified"));
    main_செய்தி(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_செய்தி(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_செய்தி(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_செய்தி(_("-boldfont <font>\tUse <font> for bold text"));
    main_செய்தி(_("-italicfont <font>\tUse <font> for italic text"));
    main_செய்தி(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
    main_செய்தி(_("-borderwidth <width>\tUse a border width of <width> (also: -bw)"));
    main_செய்தி(_("-scrollbarwidth <width>  Use a scrollbar width of <width> (also: -sw)"));
    main_செய்தி(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_செய்தி(_("+reverse\t\tDon't use reverse video (also: +rv)"));
    main_செய்தி(_("-xrm <resource>\tSet the specified resource"));
# endif // FEAT_GUI_X11
# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
    mch_செய்தி(_("\nArguments recognised by gvim (GTK4 version):\n"));
#  இல்லையெனில்
    mch_செய்தி(_("\nArguments recognised by gvim (GTK+ version):\n"));
#  endif
    main_செய்தி(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_செய்தி(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_செய்தி(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_செய்தி(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
#  ifdef USE_GTK4
    main_செய்தி("--prg-name <name>\tSet GTK program name");
#  இல்லையெனில்
    main_செய்தி(_("-iconic\t\tStart Vim iconified"));
    main_செய்தி(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_செய்தி(_("-display <display>\tRun Vim on <display> (also: --display)"));
    main_செய்தி(_("--role <role>\tSet a unique role to identify the main window"));
    main_செய்தி(_("--socketid <xid>\tOpen Vim inside another GTK widget"));
    main_செய்தி(_("--echo-wid\t\tMake gvim echo the Window ID on stdout"));
#  endif
# endif
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
    எனில் (gui.starting)
#  endif
    {
	main_செய்தி(_("-P <parent title>\tOpen Vim inside parent application"));
	main_செய்தி(_("--windowid <HWND>\tOpen Vim inside another win32 widget"));
    }
# endif

# ifdef FEAT_GUI_GNOME
    // Gnome gives extra messages for --help if we continue, but not for -h.
    எனில் (gui.starting)
    {
	mch_செய்தி("\n");
	gui.dofork = false;
    }
    இல்லையெனில்
# endif
	mch_exit(0);
}

/*
 * Check the result of the ATTENTION dialog:
 * When "Quit" selected, exit Vim.
 * When "Recover" selected, recover the file.
 */
    நிலையான வெறுமை
check_swap_exists_action(வெறுமை)
{
    எனில் (swap_exists_action == SEA_QUIT)
	getout(1);
    handle_swap_exists(வெற்று);
}

#endif // NO_VIM_MAIN

#எனில் !defined(NO_VIM_MAIN) && defined(FEAT_EVAL)
    நிலையான வெறுமை
set_progpath(char_u *argv0)
{
    char_u *val = argv0;

# ifdef MSWIN
    // A relative path containing a "/" will become invalid when using ":cd",
    // turn it into a full path.
    // On MS-Windows "vim" should be expanded to "vim.exe", thus always do
    // this.
    char_u *path = வெற்று;

    எனில் (mch_can_exe(argv0, &path, FALSE) && path != வெற்று)
	val = path;
# இல்லையெனில்
    char_u	buf[MAXPATHL + 1];
#  ifdef PROC_EXE_LINK
    எழுத்து	linkbuf[MAXPATHL + 1];
    ssize_t	len;

    len = readlink(PROC_EXE_LINK, linkbuf, MAXPATHL);
    எனில் (len > 0)
    {
	linkbuf[len] = NUL;
	val = (char_u *)linkbuf;
    }
#  endif

    எனில் (!mch_isFullName(val))
    {
	எனில் (gettail(val) != val
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
