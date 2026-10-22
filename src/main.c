/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು EXTERN
#ಅನ್ತರ್ಭಾವ "vim.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __CYGWIN__
# include <cygwin/version.h>
# include <sys/cygwin.h>	// for cygwin_conv_to_posix_path() and/or
				// cygwin_conv_path()
# include <limits.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# include "iscygpty.h"
#ಆದರೆ_ಅಂತ್ಯ

// Values for edit_type.
#ವ್ಯಾಖ್ಯಾನಿಸು EDIT_NONE   0	    // no edit type yet
#ವ್ಯಾಖ್ಯಾನಿಸು EDIT_FILE   1	    // file name argument[s] given, use argument list
#ವ್ಯಾಖ್ಯಾನಿಸು EDIT_STDIN  2	    // read file from stdin
#ವ್ಯಾಖ್ಯಾನಿಸು EDIT_ಟ್ಯಾಗ್    3	    // tag name argument given, use tagname
#ವ್ಯಾಖ್ಯಾನಿಸು EDIT_QF	    4	    // start in quickfix mode

#ಆದರೆ (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ file_owned(ಅಕ್ಷರ *fname);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ mainerr(ಪೂರ್ಣಾಂಕ, char_u *);
ಸ್ಥಿರ ಶೂನ್ಯ early_arg_ಪರಿಶೀಲನೆ(mparm_T *parmp);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NO_VIM_MAIN
ಸ್ಥಿರ ಶೂನ್ಯ usage(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ parse_command_name(mparm_T *parmp);
ಸ್ಥಿರ ಶೂನ್ಯ command_ಸಾಲು_ಪರಿಶೀಲನೆ(mparm_T *parmp);
ಸ್ಥಿರ ಶೂನ್ಯ check_tty(mparm_T *parmp);
ಸ್ಥಿರ ಶೂನ್ಯ read_stdin(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ create_windows(mparm_T *parmp);
ಸ್ಥಿರ ಶೂನ್ಯ edit_buffers(mparm_T *parmp, char_u *cwd);
ಸ್ಥಿರ ಶೂನ್ಯ exe_pre_commands(mparm_T *parmp);
ಸ್ಥಿರ ಶೂನ್ಯ exe_commands(mparm_T *parmp);
ಸ್ಥಿರ ಶೂನ್ಯ ಮೂಲ_startup_scripts(mparm_T *parmp);
ಸ್ಥಿರ ಶೂನ್ಯ main_ಆರಂಭ_gui(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ check_ಅದಲುಬದಲು_exists_action(ಶೂನ್ಯ);
# ifdef FEAT_EVAL
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_progpath(char_u *argv0);
# endif
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Different types of error messages.
 */
ಸ್ಥಿರ ಅಕ್ಷರ *(main_errors[]) =
{
    N_("Unknown option argument"),
#ವ್ಯಾಖ್ಯಾನಿಸು ME_UNKNOWN_OPTION	0
    N_("Too many edit arguments"),
#ವ್ಯಾಖ್ಯಾನಿಸು ME_TOO_MANY_ವಾದಗಳು	1
    N_("Argument missing after"),
#ವ್ಯಾಖ್ಯಾನಿಸು ME_ARG_MISSING		2
    N_("Garbage after option argument"),
#ವ್ಯಾಖ್ಯಾನಿಸು ME_GARBAGE		3
    N_("Too many \"+command\", \"-c command\" or \"--cmd command\" arguments"),
#ವ್ಯಾಖ್ಯಾನಿಸು ME_EXTRA_CMD		4
    N_("Invalid argument for"),
#ವ್ಯಾಖ್ಯಾನಿಸು ME_INVALID_ARG		5
};

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO		// don't want a prototype for main()

// Various parameters passed between main() and other functions.
ಸ್ಥಿರ mparm_T	params;

# ifdef _IOLBF
ಸ್ಥಿರ ಶೂನ್ಯ *s_vbuf = ಖಾಲಿ;		// buffer for setvbuf()
# endif

# ifndef NO_VIM_MAIN	// skip this for unittests

ಸ್ಥಿರ char_u *ಆರಂಭ_dir = ಖಾಲಿ;	// current working dir on startup

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ has_dash_c_arg = FALSE;

#  ifdef VIMDLL
__declspec(dllexport)
#  endif
    ಪೂರ್ಣಾಂಕ
#  ifdef MSWIN
VimMain
#  ಇಲ್ಲದಿದ್ದರೆ
ಮುಖ್ಯ
#  endif
(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
#  ಆದರೆ defined(STARTUPTIME) || defined(CLEAN_RUNTIMEPATH)
    ಪೂರ್ಣಾಂಕ		i;
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
    argc = get_cmd_ವಾದಗಳು_W(&argv);
#  endif

    // Many variables are in "params" so that we can pass them to invoked
    // functions without a lot of arguments.  "argc" and "argv" are also
    // copied, so that they can be changed.
    CLEAR_ಕ್ಷೇತ್ರ(params);
    params.argc = argc;
    params.argv = argv;
    params.want_ತುಂಬಿದ_screen = TRUE;
#  ifdef FEAT_EVAL
    params.use_debug_break_level = -1;
#  endif
    params.ಕಿಟಕಿ_ಎಣಿಕೆ = -1;

    autocmd_init();

#  ifdef FEAT_RUBY
    {
	ಪೂರ್ಣಾಂಕ ruby_ರಾಶಿ_ಆರಂಭ;
	vim_ruby_init((ಶೂನ್ಯ *)&ruby_ರಾಶಿ_ಆರಂಭ);
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

#  ಆದರೆ defined(STARTUPTIME) || defined(FEAT_ಕೆಲಸ_CHANNEL)
    // Need to find "--startuptime" and "--log" before actually parsing
    // arguments.
    ಗಾಗಿ (i = 1; i < argc - 1; ++i)
    {
#   ifdef STARTUPTIME
	ಆದರೆ (STRICMP(argv[i], "--startuptime") == 0 && ಸಮಯ_fd == ಖಾಲಿ)
	{
	    ಸಮಯ_fd = mch_fopen(argv[i + 1], "a");
	    ಸಮಯ_ಸಂದೇಶ("--- VIM STARTING ---");
	}
#   endif
#   ifdef FEAT_EVAL
	ಆದರೆ (STRICMP(argv[i], "--log") == 0)
	    ch_logfile((char_u *)(argv[i + 1]), (char_u *)"ao");
#   endif
    }
#  endif

#  ifdef CLEAN_RUNTIMEPATH
    // Need to find "--clean" before actually parsing arguments.
    ಗಾಗಿ (i = 1; i < argc; ++i)
	ಆದರೆ (STRICMP(argv[i], "--clean") == 0)
	{
	    params.clean = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
#  endif
#  ifdef MSWIN
    // Need to find "-register" and "-unregister" before loading any libraries.
    ಗಾಗಿ (i = 1; i < argc; ++i)
	ಆದರೆ ((STRICMP(argv[i] + 1, "register") == 0
				    || STRICMP(argv[i] + 1, "unregister") == 0)
		&& (argv[i][0] == '-' || argv[i][0] == '/'))
	{
	    ಸಿಕ್ಕಿತು_ರಿಜಿಸ್ಟರ್_arg = TRUE;
	    ನಿಲ್ಲಿಸು;
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
    command_ಸಾಲು_ಪರಿಶೀಲನೆ(&params);
    ಸಮಯ_ಸಂದೇಶ("parsing arguments");

    /*
     * On some systems, when we compile with the GUI, we always use it.  On Mac
     * there is no terminal version, and on Windows we can't fork one off with
     * :gui.
     */
#  ifdef ALWAYS_USE_GUI
    gui.starting = true;
#  ಇಲ್ಲದಿದ್ದರೆ
#   ಆದರೆ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    /*
     * Check if the GUI can be started.  Reset gui.starting if not.
     * Don't know about other systems, stay on the safe side and don't check.
     */
    ಆದರೆ (gui.starting)
    {
	ಆದರೆ (gui_init_check() == FAIL)
	{
	    gui.starting = false;

	    // When running "evim" or "gvim -y" we need the menus, exit if we
	    // don't have them.
	    ಆದರೆ (params.evim_ಬಹುಲಕ)
		mch_exit(1);
	}
    }
#   endif
#  endif

    ಆದರೆ (GARGCOUNT > 0)
    {
#  ifdef EXPAND_FILENAMES
	/*
	 * Expand wildcards in file names.
	 */
	ಆದರೆ (!params.literal)
	{
	    ಆರಂಭ_dir = alloc(MAXPATHL);
	    ಆದರೆ (ಆರಂಭ_dir != ಖಾಲಿ)
		mch_dirname(ಆರಂಭ_dir, MAXPATHL);
	    // Temporarily add '(' and ')' to 'isfname'.  These are valid
	    // filename characters but are excluded from 'isfname' to make
	    // "gf" work on a file name in parentheses (e.g.: see vim.h).
	    do_cmdline_cmd((char_u *)":set isf+=(,)");
	    alist_expand(ಖಾಲಿ, 0);
	    do_cmdline_cmd((char_u *)":set isf&");
	    ಆದರೆ (ಆರಂಭ_dir != ಖಾಲಿ)
		mch_chdir((ಅಕ್ಷರ *)ಆರಂಭ_dir);
	}
#  endif
	params.fname = alist_name(&GARGLIST[0]);
    }

#  ifdef MSWIN
    {
	// Remember the number of entries in the argument list.  If it changes
	// we don't react on setting 'encoding'.
	ಗಣ_alist_ಎಣಿಕೆ();
    }
#  endif

#  ifdef MSWIN
    ಆದರೆ (GARGCOUNT == 1 && params.ತುಂಬಿದ_ಮಾರ್ಗ)
    {
	/*
	 * If there is one filename, fully qualified, we have very probably
	 * been invoked from explorer, so change to the file's directory.
	 * Hint: to avoid this when typing a command use a forward slash.
	 * If the cd fails, it doesn't matter.
	 */
	ಆದರೆ (vim_chdirfile(params.fname, "drop") == OK)
	    ಕೊನೆಯ_chdir_reason = "drop";
	ಆದರೆ (ಆರಂಭ_dir != ಖಾಲಿ)
	    mch_dirname(ಆರಂಭ_dir, MAXPATHL);
    }
#  endif
    ಸಮಯ_ಸಂದೇಶ("expanding arguments");

#  ifdef FEAT_DIFF
    ಆದರೆ (params.diff_ಬಹುಲಕ && params.ಕಿಟಕಿ_ಎಣಿಕೆ == -1)
	params.ಕಿಟಕಿ_ಎಣಿಕೆ = 0;	// open up to 3 windows
#  endif

    // Don't redraw until much later.
    ++RedrawingDisabled;

    /*
     * When listing swap file names, don't do cursor positioning et. al.
     */
    ಆದರೆ (recoverymode && params.fname == ಖಾಲಿ)
	params.want_ತುಂಬಿದ_screen = FALSE;

    /*
     * When certain to start the GUI, don't check terminal capabilities.
     * For GTK we can't be sure, but when started from the desktop it doesn't
     * make sense to try using a terminal.
     */
#  ಆದರೆ defined(ALWAYS_USE_GUI) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	|| defined(VIMDLL)
    ಆದರೆ (gui.starting
#   ifdef FEAT_GUI_GTK
	    && !isatty(2)
#   endif
	    )
	params.want_ತುಂಬಿದ_screen = FALSE;
#  endif

    /*
     * mch_init() sets up the terminal (window) for use.  This must be
     * done after resetting full_screen, otherwise it may move the cursor.
     * Note that we may use mch_exit() before mch_init()!
     */
    mch_init();
    ಸಮಯ_ಸಂದೇಶ("shell init");

#  ifdef USE_XSMP
    /*
     * For want of anywhere else to do it, try to connect to xsmp here.
     * Fitting it in after gui_mch_init, but before gui_init (via termcapinit).
     * Hijacking -X 'no X connection' to also disable XSMP connection as that
     * has a similar delay upon failure.
     * Only try if SESSION_MANAGER is set to something non-null.
     */
    ಆದರೆ (!x_no_connect)
    {
	ಅಕ್ಷರ *p = getenv("SESSION_MANAGER");

	ಆದರೆ (p != ಖಾಲಿ && *p != NUL)
	{
	    xsmp_init();
	    ಸಮಯ_ಸಂದೇಶ("xsmp init");
	}
    }
#  endif

    /*
     * Print a warning if stdout is not a terminal.
     */
    check_tty(&params);

#  ifdef _IOLBF
    ಆದರೆ (silent_ಬಹುಲಕ)
    {
	// Ensure output works usefully without a tty: buffer lines instead of
	// fully buffered.
	s_vbuf = ಸ್ಮರಣೆ_ಕೊಡು(BUFSIZ);
	ಆದರೆ (s_vbuf != ಖಾಲಿ)
	    setvbuf(stdout, s_vbuf, _IOLBF, BUFSIZ);
    }
#  endif

    // This message comes before term inits, but after setting "silent_mode"
    // when the input is not a tty. Omit the message with --not-a-term.
    ಆದರೆ (GARGCOUNT > 1 && !silent_ಬಹುಲಕ && !is_not_a_ಪದ())
	ಬರೆ(_("%d files to edit\n"), GARGCOUNT);

    ಆದರೆ (params.want_ತುಂಬಿದ_screen && !silent_ಬಹುಲಕ)
    {
	termcapinit(params.ಪದ);	// set terminal name and get terminal
				   // capabilities (will set full_screen)
	screen_ಆರಂಭ();		// don't know where cursor is now
	ಸಮಯ_ಸಂದೇಶ("Termcap init");
    }

    /*
     * Set the default values for the options that use Rows and Columns.
     */
    ui_get_shellsize();		// inits Rows and Columns
    win_init_ಗಾತ್ರ();
#  ifdef FEAT_DIFF
    // Set the 'diff' option now, so that it can be checked for in a .vimrc
    // file.  There is no buffer yet though.
    ಆದರೆ (params.diff_ಬಹುಲಕ)
	diff_win_options(firstwin, FALSE);
#  endif

    cmdline_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - p_ch;
    cmdline_ಲಂಬಸಾಲು_off = 0;
    cmdline_ಅಗಲ = (ಪೂರ್ಣಾಂಕ)Columns;
    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು;
    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
    screenalloc(FALSE);		// allocate screen buffers
    ಗಣ_init_2();
    ಸಮಯ_ಸಂದೇಶ("inits 2");

    ಸಂದೇಶ_scroll = TRUE;
    no_wait_return = TRUE;

    init_mappings();		// set up initial mappings

    init_highlight(TRUE, FALSE); // set the default highlight groups
    ಸಮಯ_ಸಂದೇಶ("init highlight");

#  ಆದರೆ defined(FEAT_TERMRESPONSE)
    init_ಪದ_props(TRUE);
#  endif

#  ifdef FEAT_EVAL
    // Set the break level after the terminal is initialized.
    debug_break_level = params.use_debug_break_level;
#  endif

    // Reset 'loadplugins' for "-u NONE" before "--cmd" arguments.
    // Allows for setting 'loadplugins' there.
    ಆದರೆ (params.use_vimrc != ಖಾಲಿ
	    && (STRCMP(params.use_vimrc, "NONE") == 0
		|| STRCMP(params.use_vimrc, "DEFAULTS") == 0))
	p_lpl = FALSE;

    // Execute --cmd arguments.
    exe_pre_commands(&params);

    // Source startup scripts.
    ಮೂಲ_startup_scripts(&params);

#  ifdef FEAT_EVAL
    ಗಣ_vim_var_nr(VV_VIM_DID_INIT, 1L);
#  endif

#  ifdef FEAT_MZSCHEME
    /*
     * Newer version of MzScheme (Racket) require earlier (trampolined)
     * initialisation via scheme_main_setup.
     * Implement this by initialising it as early as possible
     * and splitting off remaining Vim main into vim_main2().
     * Do source startup scripts, so that 'mzschemedll' can be set.
     */
    ಹಿಂತಿರುಗಿ mzscheme_main();
#  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ vim_main2();
#  endif
}
# endif // NO_VIM_MAIN
#ಆದರೆ_ಅಂತ್ಯ // PROTO

#ಆದರೆ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(FEAT_CLIPBOARD)
/*
 * Restore the state after a fatal X error.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
x_restore_state(ಶೂನ್ಯ)
{
    State = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;
    VIsual_ಸಕ್ರಿಯ = FALSE;
    got_int = TRUE;
    need_wait_return = FALSE;
    global_busy = FALSE;
    exmode_ಸಕ್ರಿಯ = 0;
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
    scroll_ಆರಂಭ();
    redraw_later_clear();
    ಆರಿಸು_clipmethod();
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * vim_main2() is needed for FEAT_MZSCHEME, but we define it always to keep
 * things simple.
 * It is also defined when NO_VIM_MAIN is defined, but then it's empty.
 */
    ಪೂರ್ಣಾಂಕ
vim_main2(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NO_VIM_MAIN
# ifdef FEAT_EVAL
    /*
     * Read all the plugin files.
     * Only when compiled with +eval, since most plugins need it.
     */
    ಆದರೆ (p_lpl)
    {
	char_u *rtp_copy = ಖಾಲಿ;
	char_u *plugin_ನಮೂನೆ = (char_u *)
#  ಆದರೆ defined(VMS) || defined(AMIGA) // VMS and Amiga don't handle the "**".
		"plugin/*.vim"
#  ಇಲ್ಲದಿದ್ದರೆ
		"plugin/**/*.vim"
#  endif
		;

	// First add all package directories to 'runtimepath', so that their
	// autoload directories can be found.  Only if not done already with a
	// :packloadall command.
	// Make a copy of 'runtimepath', so that source_runtime does not use
	// the pack directories.
	ಆದರೆ (!did_ಮೂಲ_packages)
	{
	    rtp_copy = vim_strsave(p_rtp);
	    ಕೂಡಿಸು_pack_ಆರಂಭ_dirs();
	}

	ಮೂಲ_ಒಳ_ಮಾರ್ಗ(rtp_copy == ಖಾಲಿ ? p_rtp : rtp_copy, plugin_ನಮೂನೆ,
		DIP_ALL | DIP_NOAFTER, ಖಾಲಿ);
	ಸಮಯ_ಸಂದೇಶ("loading plugins");
	vim_free(rtp_copy);

	// Only source "start" packages if not done already with a :packloadall
	// command.
	ಆದರೆ (!did_ಮೂಲ_packages)
	    ತುಂಬು_ಆರಂಭ_packages();
	ಸಮಯ_ಸಂದೇಶ("loading packages");

	ಮೂಲ_runtime(plugin_ನಮೂನೆ, DIP_ALL | DIP_AFTER);
	ಸಮಯ_ಸಂದೇಶ("loading after plugins");
    }
# endif

# ifdef FEAT_DIFF
    // Decide about window layout for diff mode after reading vimrc.
    ಆದರೆ (params.diff_ಬಹುಲಕ && params.ಕಿಟಕಿ_layout == 0)
    {
	ಆದರೆ (diffopt_horizontal())
	    params.ಕಿಟಕಿ_layout = WIN_HOR;	// use horizontal split
	ಇಲ್ಲದಿದ್ದರೆ
	    params.ಕಿಟಕಿ_layout = WIN_VER;	// use vertical split
    }
# endif

    /*
     * Recovery mode without a file name: List swap files.
     * This uses the 'dir' option, therefore it must be after the
     * initializations.
     */
    ಆದರೆ (recoverymode && params.fname == ಖಾಲಿ)
    {
	recover_ಹೆಸರುಗಳು(ಖಾಲಿ, TRUE, ಖಾಲಿ, 0, ಖಾಲಿ);
	mch_exit(0);
    }

    /*
     * Set a few option defaults after reading .vimrc files:
     * 'title' and 'icon', Unix: 'shellpipe' and 'shellredir'.
     */
    ಗಣ_init_3();
    ಸಮಯ_ಸಂದೇಶ("inits 3");

    /*
     * "-n" argument: Disable swap file by setting 'updatecount' to 0.
     * Note that this overrides anything from a vimrc file.
     */
    ಆದರೆ (params.no_ಅದಲುಬದಲು_file)
	p_uc = 0;

# ifdef FEAT_GUI
    ಆದರೆ (gui.starting)
    {
#  ಆದರೆ defined(UNIX) || defined(VMS)
	// When something caused a message from a vimrc script, need to output
	// an extra newline before the shell prompt.
	ಆದರೆ (did_emsg || ಸಂದೇಶ_didout)
	    ಅಕ್ಷರ_ಬರೆ('\n');
#  endif

	gui_ಆರಂಭ(ಖಾಲಿ);		// will set full_screen to TRUE
	ಸಮಯ_ಸಂದೇಶ("starting GUI");

	// When running "evim" or "gvim -y" we need the menus, exit if we
	// don't have them.
	ಆದರೆ (!gui.ಒಳ_use && params.evim_ಬಹುಲಕ)
	    mch_exit(1);
	firstwin->w_prev_ಎತ್ತರ = firstwin->w_ಎತ್ತರ; // may have changed
    }
# endif

# ifdef FEAT_VIMINFO
    /*
     * Read in registers, history etc, but not marks, from the viminfo file.
     * This is where v:oldfiles gets filled.
     */
    ಆದರೆ (*p_viminfo != NUL)
    {
	read_viminfo(ಖಾಲಿ, VIF_WANT_INFO | VIF_GET_OLDFILES);
	ಸಮಯ_ಸಂದೇಶ("reading viminfo");
    }
# endif
# ifdef FEAT_EVAL
    // It's better to make v:oldfiles an empty list than NULL.
    ಆದರೆ (get_vim_var_list(VV_OLDFILES) == ಖಾಲಿ)
	ಗಣ_vim_var_list(VV_OLDFILES, list_alloc());
# endif

# ifdef FEAT_QUICKFIX
    /*
     * "-q errorfile": Load the error file now.
     * If the error file can't be read, exit before doing anything else.
     */
    ಆದರೆ (params.edit_type == EDIT_QF)
    {
	char_u	*enc = ಖಾಲಿ;

	enc = p_menc;
	ಆದರೆ (params.use_ef != ಖಾಲಿ)
	    ಗಣ_string_option_ನೇರ((char_u *)"ef", -1,
					   params.use_ef, OPT_FREE, SID_CARG);
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "cfile %s", p_ef);
	ಆದರೆ (qf_init(ಖಾಲಿ, p_ef, p_efm, TRUE, IObuff, enc) < 0)
	{
	    ಹೊರಹೋಗು_char('\n');
	    mch_exit(3);
	}
	ಸಮಯ_ಸಂದೇಶ("reading errorfile");
    }
# endif

    /*
     * Start putting things on the screen.
     * Scroll screen down before drawing over it
     * Clear screen now, so file message will not be cleared.
     */
    starting = NO_BUFFERS;
    no_wait_return = FALSE;
    ಆದರೆ (!exmode_ಸಕ್ರಿಯ)
	ಸಂದೇಶ_scroll = FALSE;

# ifdef FEAT_GUI
    /*
     * This seems to be required to make callbacks to be called now, instead
     * of after things have been put on the screen, which then may be deleted
     * when getting a resize callback.
     * For the Mac this handles putting files dropped on the Vim icon to
     * global_alist.
     */
    ಆದರೆ (gui.ಒಳ_use)
    {
	gui_wait_for_chars(50L, typebuf.tb_change_cnt);
	ಸಮಯ_ಸಂದೇಶ("GUI delay");
    }
# endif

# ಆದರೆ defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)
    qnx_clip_init();
# endif

# ಆದರೆ defined(MACOS_X) && defined(FEAT_CLIPBOARD)
    clip_init(TRUE);
# endif

# ifdef FEAT_XCLIPBOARD
    // Start using the X clipboard, unless the GUI was started.
#  ifdef FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use)
#  endif
    {
	ಸಿದ್ಧತೆ_ಪದ_clip();
	ಸಮಯ_ಸಂದೇಶ("setup x11 clipboard");
    }
# endif

# ifdef FEAT_CLIENTSERVER
    // Prepare for being a Vim server.
    prepare_server(&params);
# endif

# ifdef FEAT_WAYLAND
#  ifdef FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use)
#  endif
    {
	ಆದರೆ (wayland_init_connection(wayland_display_name) == OK)
	{
	    ಸಮಯ_ಸಂದೇಶ("connected to Wayland display");

#  ifdef FEAT_WAYLAND_CLIPBOARD
	    ಆದರೆ (clip_init_wayland() == OK)
		ಸಮಯ_ಸಂದೇಶ("setup Wayland clipboard");
#  endif
	}
    }
# endif

# ifdef HAVE_CLIPMETHOD
    ಆರಿಸು_clipmethod();
# endif

    /*
     * If "-" argument given: Read file from stdin.
     * Do this before starting Raw mode, because it may change things that the
     * writing end of the pipe doesn't like, e.g., in case stdin and stderr
     * are the same terminal: "cat | vim -".
     * Using autocommands here may cause trouble...
     */
    ಆದರೆ (params.edit_type == EDIT_STDIN && !recoverymode)
	read_stdin();

# ಆದರೆ defined(UNIX) || defined(VMS)
    // When switching screens and something caused a message from a vimrc
    // script, need to output an extra newline on exit.
    ಆದರೆ ((did_emsg || ಸಂದೇಶ_didout) && *T_TI != NUL && params.edit_type != EDIT_STDIN)
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
    ಸಮಯ_ಸಂದೇಶ("setting raw mode");

    ಆದರೆ (need_wait_return || ಸಂದೇಶ_didany)
    {
	wait_return(TRUE);
	ಸಮಯ_ಸಂದೇಶ("waiting for return");
    }

    starttermcap();	    // start termcap if not done by wait_return()
    ಸಮಯ_ಸಂದೇಶ("start termcap");

    setmouse();				// may start using the mouse
    ಆದರೆ (scroll_region)
	scroll_region_reset();		// In case Rows changed
    scroll_ಆರಂಭ();	// may scroll the screen to the right position

# ಆದರೆ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    ಪದ_ತಳ್ಳು_title(SAVE_RESTORE_BOTH);
# endif

    /*
     * Don't clear the screen when starting in Ex mode, unless using the GUI.
     */
    ಆದರೆ (exmode_ಸಕ್ರಿಯ
# ifdef FEAT_GUI
			&& !gui.ಒಳ_use
# endif
					)
	ಗಣ_must_redraw(UPD_CLEAR);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	screenclear();			// clear screen
	ಸಮಯ_ಸಂದೇಶ("clearing screen");
    }

# ifdef FEAT_CRYPT
    ಆದರೆ (params.ask_for_ಕೀಲಿ)
    {
	crypt_check_current_ವಿಧಾನ();
	(ಶೂನ್ಯ)crypt_get_ಕೀಲಿ(TRUE, TRUE);
	ಸಮಯ_ಸಂದೇಶ("getting crypt key");
    }
# endif

    no_wait_return = TRUE;

    /*
     * Create the requested number of windows and edit buffers in them.
     * Also does recovery if "recoverymode" set.
     */
    create_windows(&params);
    ಸಮಯ_ಸಂದೇಶ("opening buffers");

# ifdef FEAT_EVAL
    // clear v:swapcommand
    ಗಣ_vim_var_string(VV_SWAPCOMMAND, ಖಾಲಿ, -1);
# endif

    // Ex starts at last line of the file
    ಆದರೆ (exmode_ಸಕ್ರಿಯ)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    apply_autocmds(EVENT_BUFENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    ಸಮಯ_ಸಂದೇಶ("BufEnter autocommands");
    setpcmark();

# ifdef FEAT_QUICKFIX
    /*
     * When started with "-q errorfile" jump to first error now.
     */
    ಆದರೆ (params.edit_type == EDIT_QF)
    {
	qf_jump(ಖಾಲಿ, 0, 0, FALSE);
	ಸಮಯ_ಸಂದೇಶ("jump to first error");
    }
# endif

    /*
     * If opened more than one window, start editing files in the other
     * windows.
     */
    edit_buffers(&params, ಆರಂಭ_dir);
    vim_free(ಆರಂಭ_dir);

# ifdef FEAT_DIFF
    ಆದರೆ (params.diff_ಬಹುಲಕ)
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
    ಆದರೆ (params.tagname != ಖಾಲಿ)
    {
	ಅದಲುಬದಲು_exists_did_quit = FALSE;

	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "ta %s", params.tagname);
	do_cmdline_cmd(IObuff);
	ಸಮಯ_ಸಂದೇಶ("jumping to tag");

	// If the user doesn't want to edit the file then we quit here.
	ಆದರೆ (ಅದಲುಬದಲು_exists_did_quit)
	    getout(1);
    }

# ಆದರೆ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    // Temporarily set x_jump_env to here in case there is an X11 IO error,
    // because x_jump_env is only actually set in main_loop(), before
    // exe_commands(). May not be the best solution since commands passed via
    // the command line can be very broad like sourcing a file, in which case
    // an X IO error results in the command being partially done. In theory we
    // could use SETJMP in RealWaitForChar(), but the stack frame for that may
    // possibly exit and then LONGJMP is called on it.
    ಪೂರ್ಣಾಂಕ jump_ಫಲಿತಾಂಶ = SETJMP(x_jump_env);

    ಆದರೆ (jump_ಫಲಿತಾಂಶ == 0)
    {
# endif
	// Execute any "+", "-c" and "-S" arguments.
	ಆದರೆ (params.n_commands > 0)
	    exe_commands(&params);
# ಆದರೆ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// Restore state and continue just like what main_loop() does.
	x_restore_state();
# endif

    // Must come before the may_req_ calls.
    starting = 0;

# ಆದರೆ defined(FEAT_TERMRESPONSE)
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
# ಆದರೆ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_ಗಾತ್ರ();
# endif

    // start in insert mode
    ಆದರೆ (p_im)
	need_ಆರಂಭ_insertmode = TRUE;

# ifdef FEAT_EVAL
    ಗಣ_vim_var_nr(VV_VIM_DID_ENTER, 1L);
# endif
    apply_autocmds(EVENT_VIMENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    ಸಮಯ_ಸಂದೇಶ("VimEnter autocommands");

# ಆದರೆ defined(FEAT_EVAL) && defined(FEAT_CLIPBOARD)
    // Adjust default register name for "unnamed" in 'clipboard'. Can only be
    // done after the clipboard is available and all initial commands that may
    // modify the 'clipboard' setting have run; i.e. just before entering the
    // main loop.
    reset_reg_var();
# endif

# ಆದರೆ defined(FEAT_DIFF)
    // When a startup script or session file setup for diff'ing and
    // scrollbind, sync the scrollbind now.
    ಆದರೆ (curwin->w_p_diff && curwin->w_p_scb)
    {
	update_topline();
	check_scrollbind((linenr_T)0, 0L);
	ಸಮಯ_ಸಂದೇಶ("diff scrollbinding");
    }
# endif

# ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ಆದರೆ (!gui.ಒಳ_use)
#  endif
	mch_ಗಣ_winsize_now();	    // Allow winsize changes from now on
# endif

# ಆದರೆ defined(FEAT_GUI)
    // When tab pages were created, may need to update the tab pages line and
    // scrollbars.  This is skipped while creating them.
    ಆದರೆ (gui.ಒಳ_use && ಮೊದಲ_tabpage->tp_ಮುಂದಿನ != ಖಾಲಿ)
    {
	ಹೊರಹೋಗು_flush();
	gui_init_which_components(ಖಾಲಿ);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
# endif

    // If ":startinsert" command used, stuff a dummy command to be able to
    // call normal_cmd(), which will then start Insert mode.
    ಆದರೆ (restart_edit != 0)
	stuffcharReadbuff(K_NOP);

# ifdef FEAT_NETBEANS_INTG
    ಆದರೆ (netbeansArg != ಖಾಲಿ && ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ("-nb", netbeansArg, 3) == 0)
    {
#  ifdef FEAT_GUI
#   ಆದರೆ !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
	ಆದರೆ (gui.ಒಳ_use)
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

    ಸಮಯ_ಸಂದೇಶ("before starting main loop");

# ifdef FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (use_stdio_channel)
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

#ಆದರೆ_ಅಂತ್ಯ // NO_VIM_MAIN

    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Initialisation #1 shared by main() and some tests.
 */
    ಶೂನ್ಯ
common_init_1(ಶೂನ್ಯ)
{
    estack_init();
    cmdline_init();

    (ಶೂನ್ಯ)mb_init();	// init mb_bytelen_tab[] to ones
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    eval_init();	// init global variables
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __QNXNTO__
    qnx_init();		// PhAttach() for clipboard, (and gui)
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Allocate space for the generic buffers (needed for set_init_1() and
     * emsg()).
     */
    ಆದರೆ ((IObuff = alloc(IOSIZE)) == ಖಾಲಿ
	    || (NameBuff = alloc(MAXPATHL)) == ಖಾಲಿ)
	mch_exit(0);
    ಸಮಯ_ಸಂದೇಶ("Allocated generic buffers");
}


/*
 * Initialisation #2 shared by main() and some tests.
 */
    ಶೂನ್ಯ
common_init_2(mparm_T *paramp)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NBDEBUG
    // Wait a moment for debugging NetBeans.  Must be after allocating
    // NameBuff.
    nbdebug_log_init("SPRO_GVIM_DEBUG", "SPRO_GVIM_DLEVEL");
    nbdebug_wait(WT_ENV | WT_WAIT | WT_STOP, "SPRO_GVIM_WAIT", 20);
    ಸಮಯ_ಸಂದೇಶ("NetBeans debug wait");
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    /*
     * Setup to use the current locale (for ctype() and many other things).
     * NOTE: Translated messages with encodings other than latin1 will not
     * work until set_init_1() has been called!
     */
    init_locale();
    ಸಮಯ_ಸಂದೇಶ("locale set");
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    gui.dofork = true;		    // default is to use fork()
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER_BACKENDS
    /*
     * Check the $VIM_CLIENTSERVER env before we handle the --clientserver arg
     */
    check_clientserver_ವಿಧಾನ_env();
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Do a first scan of the arguments in "argv[]":
     *   -display or --display
     *   --server...
     *   --socketid
     *   --windowid
     */
    early_arg_ಪರಿಶೀಲನೆ(paramp);

#ಆದರೆ defined(FEAT_GUI)
    // Prepare for possibly starting GUI sometime
    gui_prepare(&paramp->argc, paramp->argv);
    ಸಮಯ_ಸಂದೇಶ("GUI prepared");
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    clip_init(FALSE);		// Initialise clipboard stuff
    ಸಮಯ_ಸಂದೇಶ("clipboard setup");
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Check if we have an interactive window.
     * On the Amiga: If there is no window, we open one with a newcli command
     * (needed for :! to * work). mch_check_win() will also handle the -d or
     * -dev argument.
     */
    stdout_isatty = (mch_check_win(paramp->argc, paramp->argv) != FAIL);
    ಸಮಯ_ಸಂದೇಶ("window checked");

    /*
     * Allocate the first window and buffer.
     * Can't do anything without it, exit when it fails.
     */
    ಆದರೆ (win_alloc_ಮೊದಲ() == FAIL)
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
    ಗಣ_init_1(paramp->clean);
    ಸಮಯ_ಸಂದೇಶ("inits 1");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // set v:lang and v:ctype
    ಗಣ_lang_var();

    // set v:argv
    ಗಣ_argv_var(paramp->argv, paramp->argc);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
    init_signs();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    // initialize quickfix list
    // don't send an error message when memory allocation fails
    // do it when the user tries to access the quickfix list
    qf_init_ರಾಶಿ();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return TRUE when the --not-a-term argument was found.
 */
    ಪೂರ್ಣಾಂಕ
is_not_a_ಪದ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ params.not_a_ಪದ;
}

/*
 * Return TRUE when the --not-a-term argument was found or the GUI is in use.
 */
    ಪೂರ್ಣಾಂಕ
is_not_a_ಪದ_or_gui(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ params.not_a_ಪದ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
			    || gui.ಒಳ_use
#ಆದರೆ_ಅಂತ್ಯ
	;
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_vbuf(ಶೂನ್ಯ)
{
# ifdef _IOLBF
    ಆದರೆ (s_vbuf != ಖಾಲಿ)
    {
	setvbuf(stdout, ಖಾಲಿ, _IONBF, 0);
	ಬಿಡುಗಡೆ(s_vbuf);
	s_vbuf = ಖಾಲಿ;
    }
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI)
/*
 * If a --gui-dialog-file argument was given return the file name.
 * Otherwise return NULL.
 */
    char_u *
get_gui_dialog_file(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ params.gui_dialog_file;
}
#ಆದರೆ_ಅಂತ್ಯ

// When TRUE in a safe state when starting to wait for a character.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	was_ಸುರಕ್ಷಿತ = FALSE;
ಸ್ಥಿರ oparg_T	*current_oap = ಖಾಲಿ;

/*
 * Return TRUE if an operator was started but not finished yet.
 * Includes typing a count or a register name.
 */
    ಪೂರ್ಣಾಂಕ
op_pending(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ !(current_oap != ಖಾಲಿ
	    && !ಮುಕ್ತಾಯ_op
	    && current_oap->prev_opcount == 0
	    && current_oap->prev_count0 == 0
	    && current_oap->op_type == OP_NOP
	    && current_oap->regname == NUL);
}

/*
 * Return whether currently it is safe, assuming it was safe before (high level
 * state didn't change).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_ಸುರಕ್ಷಿತ_now(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ stuff_ಖಾಲಿಗಳು()
	&& typebuf.tb_len == 0
	&& scriptin[curscript] == ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	&& !debug_ಬಹುಲಕ
#ಆದರೆ_ಅಂತ್ಯ
	&& !global_busy;
}

/*
 * Trigger SafeState if currently in a safe state, that is "safe" is TRUE and
 * there is no typeahead.
 */
    ಶೂನ್ಯ
may_trigger_safestate(ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ)
{
    ಪೂರ್ಣಾಂಕ is_ಸುರಕ್ಷಿತ = ಸುರಕ್ಷಿತ && is_ಸುರಕ್ಷಿತ_now();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (was_ಸುರಕ್ಷಿತ != is_ಸುರಕ್ಷಿತ)
	// Only log when the state changes, otherwise it happens at nearly
	// every key stroke.
	ch_log(ಖಾಲಿ, is_ಸುರಕ್ಷಿತ ? "SafeState: Start triggering"
					       : "SafeState: Stop triggering");
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (is_ಸುರಕ್ಷಿತ)
	apply_autocmds(EVENT_SAFESTATE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    was_ಸುರಕ್ಷಿತ = is_ಸುರಕ್ಷಿತ;
}

/*
 * Something changed which causes the state possibly to be unsafe, e.g. a
 * character was typed.  It will remain unsafe until the next call to
 * may_trigger_safestate().
 */
    ಶೂನ್ಯ
state_no_longer_ಸುರಕ್ಷಿತ(ಅಕ್ಷರ *reason ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (was_ಸುರಕ್ಷಿತ)
	ch_log(ಖಾಲಿ, "SafeState: reset: %s", reason);
#ಆದರೆ_ಅಂತ್ಯ
    was_ಸುರಕ್ಷಿತ = FALSE;
}

#ಆದರೆ defined(FEAT_EVAL) || defined(ಸಂದೇಶ_ಸರತಿ)
    ಪೂರ್ಣಾಂಕ
get_was_ಸುರಕ್ಷಿತ_state(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ was_ಸುರಕ್ಷಿತ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(ಸಂದೇಶ_ಸರತಿ)
/*
 * Invoked when leaving code that invokes callbacks.  Then trigger
 * SafeStateAgain, if it was safe when starting to wait for a character.
 */
    ಶೂನ್ಯ
may_trigger_safestateagain(ಶೂನ್ಯ)
{
    ಆದರೆ (!was_ಸುರಕ್ಷಿತ)
    {
	// If the safe state was reset in state_no_longer_safe(), e.g. because
	// of calling feedkeys(), we check if it's now safe again (all keys
	// were consumed).
	was_ಸುರಕ್ಷಿತ = is_ಸುರಕ್ಷಿತ_now();
# ifdef FEAT_EVAL
	ಆದರೆ (was_ಸುರಕ್ಷಿತ)
	    ch_log(ಖಾಲಿ, "SafeState: undo reset");
# endif
    }
    ಆದರೆ (was_ಸುರಕ್ಷಿತ)
    {
# ifdef FEAT_EVAL
	// Only do this message when another message was given, otherwise we
	// get lots of them.
	ಆದರೆ ((did_repeated_ಸಂದೇಶ & REPEATED_ಸಂದೇಶ_SAFESTATE) == 0)
	{
	    ಪೂರ್ಣಾಂಕ did = did_repeated_ಸಂದೇಶ;

	    ch_log(ಖಾಲಿ,
		      "SafeState: back to waiting, triggering SafeStateAgain");
	    did_repeated_ಸಂದೇಶ = did | REPEATED_ಸಂದೇಶ_SAFESTATE;
	}
# endif
	apply_autocmds(EVENT_SAFESTATEAGAIN, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    }
# ifdef FEAT_EVAL
    ಇಲ್ಲದಿದ್ದರೆ
	ch_log(ಖಾಲಿ,
		  "SafeState: back to waiting, not triggering SafeStateAgain");
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if there is any typeahead, pending operator or command.
 */
    ಪೂರ್ಣಾಂಕ
work_pending(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ op_pending() || !is_ಸುರಕ್ಷಿತ_now();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
ಸ್ಥಿರ linenr_T	conceal_old_cursor_ಸಾಲು = 0;
ಸ್ಥಿರ linenr_T	conceal_new_cursor_ಸಾಲು = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	conceal_update_ಸಾಲುಗಳು = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Trigger the events that are not triggered where they happen, but by
 * comparing the current state against the state stored when they were last
 * triggered. Also updates what depends on the cursor having moved, which is
 * not affected by 'eventignore'.
 * Called from the main loop and when 'eventignore(win)' changes, so that what
 * happened with an event ignored is not reported once it is not ignored
 * anymore.
 */
    ಶೂನ್ಯ
may_trigger_deferred_events(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ bool	recursive = false;

    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ;
    recursive = true;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಆದರೆ (curwin->w_p_cole == 0)
	conceal_update_ಸಾಲುಗಳು = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // Trigger CursorMoved if the cursor moved.
    ಆದರೆ (!ಮುಕ್ತಾಯ_op && (has_cursormoved()
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		|| popup_visible
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
		|| curwin->w_p_cole > 0
#ಆದರೆ_ಅಂತ್ಯ
		) && !EQUAL_POS(ಕೊನೆಯ_cursormoved, curwin->w_cursor))
    {
	ಆದರೆ (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (popup_visible)
	    popup_check_cursor_pos();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	ಆದರೆ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_ಸಾಲು = ಕೊನೆಯ_cursormoved.lnum;
	    conceal_new_cursor_ಸಾಲು = curwin->w_cursor.lnum;
	    conceal_update_ಸಾಲುಗಳು = TRUE;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಕೊನೆಯ_cursormoved = curwin->w_cursor;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಆದರೆ (conceal_update_ಸಾಲುಗಳು
	    && (conceal_old_cursor_ಸಾಲು != conceal_new_cursor_ಸಾಲು
		|| conceal_cursor_ಸಾಲು(curwin)
		|| need_cursor_ಸಾಲು_redraw))
    {
	ಆದರೆ (conceal_old_cursor_ಸಾಲು != conceal_new_cursor_ಸಾಲು
		&& conceal_old_cursor_ಸಾಲು != 0
		&& conceal_old_cursor_ಸಾಲು <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    redrawWinline(curwin, conceal_old_cursor_ಸಾಲು);
	redrawWinline(curwin, conceal_new_cursor_ಸಾಲು);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_ಸಾಲು_redraw = FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Trigger TextChanged if b:changedtick differs.
    ಆದರೆ (!ಮುಕ್ತಾಯ_op && has_textchanged()
	    && curbuf->b_ಕೊನೆಯ_changedtick != CHANGEDTICK(curbuf))
    {
	apply_autocmds(EVENT_TEXTCHANGED, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	curbuf->b_ಕೊನೆಯ_changedtick = CHANGEDTICK(curbuf);
    }

    // Ensure curwin->w_topline and curwin->w_leftcol are up to date before
    // triggering a WinScrolled autocommand.
    update_topline();
    validate_cursor();

    ಆದರೆ (!ಮುಕ್ತಾಯ_op)
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
    ಶೂನ್ಯ
main_loop(
    ಪೂರ್ಣಾಂಕ		cmdwin,	    // TRUE when working in the command-line window
    ಪೂರ್ಣಾಂಕ		noexmode)   // TRUE when return on entering Ex mode
{
    oparg_T	oa;		// operator arguments
    oparg_T	*prev_oap;	// operator arguments
    ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಹಿಂದಿನ_got_int = FALSE;	// "got_int" was TRUE

    prev_oap = current_oap;
    current_oap = &oa;

#ಆದರೆ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    // Setup to catch a terminating error from the X server.  Just ignore
    // it, restore the state and continue.  This might not always work
    // properly, but at least we hopefully don't exit unexpectedly when the X
    // server exits while Vim is running in a console.
    ಆದರೆ (!cmdwin && !noexmode && SETJMP(x_jump_env))
	x_restore_state();
#ಆದರೆ_ಅಂತ್ಯ

    clear_oparg(&oa);
    ಆಗ (!cmdwin || cmdwin_ಫಲಿತಾಂಶ == 0)
    {
	ಆದರೆ (stuff_ಖಾಲಿಗಳು())
	{
	    did_check_timestamps = FALSE;
	    ಆದರೆ (need_check_timestamps)
		check_timestamps(FALSE);
	    ಆದರೆ (need_wait_return)	// if wait_return() still needed ...
		wait_return(FALSE);	// ... call it now
	    ಆದರೆ (need_ಆರಂಭ_insertmode && goto_im() && !VIsual_ಸಕ್ರಿಯ)
	    {
		need_ಆರಂಭ_insertmode = FALSE;
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
	ಆದರೆ (got_int)
	{
	    ಆದರೆ (noexmode && global_busy && !exmode_ಸಕ್ರಿಯ && ಹಿಂದಿನ_got_int)
	    {
		// Typed two CTRL-C in a row: go back to ex mode as if "Q" was
		// used and keep "got_int" set, so that it aborts ":g".
		exmode_ಸಕ್ರಿಯ = EXMODE_ಸಾಮಾನ್ಯ;
		State = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!global_busy || !exmode_ಸಕ್ರಿಯ)
	    {
		ಆದರೆ (!quit_ಇನ್ನಷ್ಟು)
		    (ಶೂನ್ಯ)vgetc();		// flush all buffers
		got_int = FALSE;
	    }
	    ಹಿಂದಿನ_got_int = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂದಿನ_got_int = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// At the toplevel there is no exception handling.  Discard any that
	// may be hanging around (e.g. from "interrupt" at the debug prompt).
	ಆದರೆ (did_throw && !ex_ಸಾಮಾನ್ಯ_busy)
	    discard_current_exception();
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (!exmode_ಸಕ್ರಿಯ)
	    ಸಂದೇಶ_scroll = FALSE;
	quit_ಇನ್ನಷ್ಟು = FALSE;

	// it's not safe unless may_trigger_safestate_main() is called
	was_ಸುರಕ್ಷಿತ = FALSE;

	/*
	 * If skip redraw is set (for ":" in wait_return()), don't redraw now.
	 * If there is nothing in the stuff_buffer or do_redraw is TRUE,
	 * update cursor and redraw.
	 */
	ಆದರೆ (skip_redraw || exmode_ಸಕ್ರಿಯ)
	{
	    skip_redraw = FALSE;
	    setcursor();
	    cursor_on();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_redraw || stuff_ಖಾಲಿಗಳು())
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    // If ui_breakcheck() was used a resize may have been postponed.
	    gui_may_resize_shell();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_DROP_FILE
	    // If files were dropped while text was locked or the curbuf was
	    // locked, this would be a good time to handle the drop.
	    handle_any_postponed_drop();
#ಆದರೆ_ಅಂತ್ಯ
	    may_trigger_deferred_events();

	    // If nothing is pending and we are going to wait for the user to
	    // type a character, trigger SafeState.
	    may_trigger_safestate(!op_pending() && restart_edit == 0);

#ಆದರೆ defined(FEAT_DIFF)
	    // Updating diffs from changed() does not always work properly,
	    // esp. updating folds.  Do an update just before redrawing if
	    // needed.
	    ಆದರೆ (curtab->tp_diff_update || curtab->tp_diff_invalid)
	    {
		ex_diffupdate(ಖಾಲಿ);
		curtab->tp_diff_update = FALSE;
	    }

	    // Scroll-binding for diff mode may have been postponed until
	    // here.  Avoids doing it for every change.
	    ಆದರೆ (diff_need_scrollbind)
	    {
		check_scrollbind((linenr_T)0, 0L);
		diff_need_scrollbind = FALSE;
	    }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_FOLDING)
	    // Include a closed fold completely in the Visual area.
	    foldAdjustVisual();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    /*
	     * When 'foldclose' is set, apply 'foldlevel' to folds that don't
	     * contain the cursor.
	     * When 'foldopen' is "all", open the fold(s) under the cursor.
	     * This may mark the window for redrawing.
	     */
	    ಆದರೆ (hasAnyFolding(curwin) && !char_avail())
	    {
		foldCheckClose();
		ಆದರೆ (fdo_flags & FDO_ALL)
		    foldOpenCursor();
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    // Before redrawing, make sure w_topline is correct, and w_leftcol
	    // if lines don't wrap, and w_skipcol if lines wrap.
	    update_topline();
	    validate_cursor();

	    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
		update_curbuf(UPD_INVERTED); // update inverted part
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (must_redraw)
	    {
		mch_disable_flush();	// Stop issuing gui_mch_flush().
		update_screen(0);
		mch_enable_flush();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (redraw_cmdline || clear_cmdline || redraw_ಬಹುಲಕ)
		showmode();
	    redraw_statuslines();
	    ಆದರೆ (need_maketitle)
		maketitle();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
	    curbuf->b_ಕೊನೆಯ_ಬಳಸಿದ = vim_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ
	    // display message after redraw
	    ಆದರೆ (keep_ಸಂದೇಶ != ಖಾಲಿ)
	    {
		char_u *p = vim_strsave(keep_ಸಂದೇಶ);

		ಆದರೆ (p != ಖಾಲಿ)
		{
		    // msg_start() will set keep_msg to NULL, make a copy
		    // first.  Don't reset keep_msg, msg_attr_keep() uses it to
		    // check for duplicates.  Never put this message in
		    // history.
		    ಸಂದೇಶ_hist_off = TRUE;
		    ಸಂದೇಶ_attr((ಅಕ್ಷರ *)p, keep_ಸಂದೇಶ_attr);
		    ಸಂದೇಶ_hist_off = FALSE;
		    vim_free(p);
		}
	    }
	    ಆದರೆ (need_fileinfo)		// show file info after redraw
	    {
		fileinfo(FALSE, TRUE, FALSE);
		need_fileinfo = FALSE;
	    }

	    emsg_on_display = FALSE;	// can delete error message now
	    did_emsg = FALSE;
	    ಸಂದೇಶ_didany = FALSE;		// reset lines_left in msg_start()
	    may_clear_sb_ಪಠ್ಯ();	// clear scroll-back text on next msg
	    showruler(FALSE);

	    setcursor();
	    cursor_on();

	    do_redraw = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ STARTUPTIME
	    // Now that we have drawn the first screen all the startup stuff
	    // has been done, close any file for startup messages.
	    ಆದರೆ (ಸಮಯ_fd != ಖಾಲಿ)
	    {
		ಸಮಯ_ಸಂದೇಶ("first screen update");
		ಸಮಯ_ಸಂದೇಶ("--- VIM STARTED ---");
		fclose(ಸಮಯ_fd);
		ಸಮಯ_fd = ಖಾಲಿ;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    // After the first screen update may start triggering WinScrolled
	    // autocmd events.  Store all the scroll positions and sizes now.
	    may_make_initial_scroll_ಗಾತ್ರ_snapshot();
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (need_mouse_correct)
	    gui_mouse_correct();
#ಆದರೆ_ಅಂತ್ಯ

	// May request the keyboard protocol state now.
	may_send_t_RK();

	// Update w_curswant if w_set_curswant has been set.
	// Postponed until here to avoid computing w_virtcol too often.
	update_curswant();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	/*
	 * May perform garbage collection when waiting for a character, but
	 * only at the very toplevel.  Otherwise we may be using a List or
	 * Dict internally somewhere.
	 * "may_garbage_collect" is reset in vgetc() which is invoked through
	 * do_exmode() and normal_cmd().
	 */
	may_garbage_collect = (!cmdwin && !noexmode);
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * If we're invoked as ex, do a round of ex commands.
	 * Otherwise, get and execute a normal mode command.
	 */
	ಆದರೆ (exmode_ಸಕ್ರಿಯ)
	{
	    ಆದರೆ (noexmode)   // End of ":global/path/visual" commands
		ಹೋಗು theend;
	    do_exmode(exmode_ಸಕ್ರಿಯ == EXMODE_VIM);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    ಆದರೆ (ಪದ_use_loop()
		    && oa.op_type == OP_NOP && oa.regname == NUL
		    && !VIsual_ಸಕ್ರಿಯ
		    && !skip_ಪದ_loop)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal mode.  With FAIL we first need to position the
		// cursor and the screen needs to be redrawn.
		ಆದರೆ (terminal_loop(TRUE) == OK)
		    ಸಾಮಾನ್ಯ_cmd(&oa, TRUE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
		skip_ಪದ_loop = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
		ಸಾಮಾನ್ಯ_cmd(&oa, TRUE);
	    }
	}
    }

theend:
    current_oap = prev_oap;
}


#ಆದರೆ defined(USE_XSMP) || defined(FEAT_GUI)
/*
 * Exit, but leave behind swap files for modified buffers.
 */
    ಶೂನ್ಯ
getout_preserve_modified(ಪೂರ್ಣಾಂಕ exitval)
{
# ಆದರೆ defined(SIGHUP) && defined(SIG_IGN)
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
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Exit properly.  This is the only way to exit Vim after startup has
 * succeeded.  We are certain to exit here, no way to abort it.
 */
    ಶೂನ್ಯ
getout(ಪೂರ್ಣಾಂಕ exitval)
{
    exiting = TRUE;
#ಆದರೆ defined(FEAT_EVAL)
    ch_log(ಖಾಲಿ, "Exiting...");
#ಆದರೆ_ಅಂತ್ಯ

    // When running in Ex mode an error causes us to exit with a non-zero exit
    // code.  POSIX requires this, although it's not 100% clear from the
    // standard.
    ಆದರೆ (exmode_ಸಕ್ರಿಯ)
	exitval += ex_exitval;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಗಣ_vim_var_type(VV_EXITING, VAR_ಸಂಖ್ಯೆ);
    ಗಣ_vim_var_nr(VV_EXITING, exitval);
#ಆದರೆ_ಅಂತ್ಯ

    // Position the cursor on the last screen line, below all the text
    ಆದರೆ (!is_not_a_ಪದ_or_gui())
	windgoto((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // Invoked all deferred functions in the function stack.
    invoke_all_defer();
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
    // Optionally print hashtable efficiency.
    hash_debug_results();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಸಂದೇಶ_didany = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (v_dying <= 1)
    {
	tabpage_T	*tp;
	tabpage_T	*ಮುಂದಿನ_tp;
	buf_T		*buf;
	win_T		*wp;
	ಪೂರ್ಣಾಂಕ		unblock = 0;

	// Trigger BufWinLeave for all windows, but only once per buffer.
	ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp != ಖಾಲಿ; tp = ಮುಂದಿನ_tp)
	{
	    ಮುಂದಿನ_tp = tp->tp_ಮುಂದಿನ;
	    FOR_ALL_WINDOWS_ಒಳ_TAB(tp, wp)
	    {
		ಆದರೆ (wp->w_ಬಫರ್ == ಖಾಲಿ || !buf_valid(wp->w_ಬಫರ್))
		    // Autocmd must have close the buffer already, skip.
		    ಮುಂದುವರಿ;
		buf = wp->w_ಬಫರ್;
		ಆದರೆ (CHANGEDTICK(buf) != -1)
		{
		    bufref_T bufref;

		    ಗಣ_bufref(&bufref, buf);
		    apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname,
						    buf->b_fname, FALSE, buf);
		    ಆದರೆ (bufref_valid(&bufref))
			CHANGEDTICK(buf) = -1;  // note we did it already

		    // start all over, autocommands may mess up the lists
		    ಮುಂದಿನ_tp = ಮೊದಲ_tabpage;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}

	// Trigger BufUnload for buffers that are loaded
	FOR_ALL_BUFFERS(buf)
	    ಆದರೆ (buf->b_ml.ml_mfp != ಖಾಲಿ)
	    {
		bufref_T bufref;

		ಗಣ_bufref(&bufref, buf);
		apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf);
		ಆದರೆ (!bufref_valid(&bufref))
		    // autocmd deleted the buffer
		    ನಿಲ್ಲಿಸು;
	    }

	// deathtrap() blocks autocommands, but we do want to trigger
	// VimLeavePre.
	ಆದರೆ (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVEPRE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	ಆದರೆ (unblock)
	    ಖಂಡ_autocmds();
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    ಆದರೆ (
# ifdef EXITFREE
	    entered_free_all_mem == FALSE &&
# endif
	    *p_viminfo != NUL)
	// Write out the registers, history, marks etc, to the viminfo file
	write_viminfo(ಖಾಲಿ, FALSE);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (v_dying <= 1)
    {
	ಪೂರ್ಣಾಂಕ	unblock = 0;

	// deathtrap() blocks autocommands, but we do want to trigger VimLeave.
	ಆದರೆ (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	ಆದರೆ (unblock)
	    ಖಂಡ_autocmds();
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    profile_dump();
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (did_emsg
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    || (gui.ಒಳ_use && ಸಂದೇಶ_didany && p_verbose > 0)
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	// give the user a chance to read the (error) message
	no_wait_return = FALSE;
	wait_return(FALSE);
    }

    // Position the cursor again, the autocommands may have moved it
    ಆದರೆ (!is_not_a_ಪದ_or_gui())
	windgoto((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಕೆಲಸ_stop_on_exit();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LUA
    lua_ಕೊನೆ();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
    mzscheme_ಕೊನೆ();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TCL
    tcl_ಕೊನೆ();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RUBY
    ruby_ಕೊನೆ();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
    python_ಕೊನೆ();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
    python3_ಕೊನೆ();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERL
    perl_ಕೊನೆ();
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
    iconv_ಕೊನೆ();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    netbeans_ಕೊನೆ();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER
    socketserver_stop();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
    cs_ಕೊನೆ();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
    uninit_image_state();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (garbage_collect_at_exit)
	garbage_collect(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    free_cmd_ವಾದಗಳು_W();
#ಆದರೆ_ಅಂತ್ಯ

    ಪದ_disable_dec();

    mch_exit(exitval);
}

/*
 * Get the name of the display, before gui_prepare() removes it from
 * argv[].  Used for the xterm-clipboard display.
 *
 * Also find the --server, --clientserver... arguments and --socketid and
 * --windowid
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
early_arg_ಪರಿಶೀಲನೆ(mparm_T *parmp ಬಳಸದ)
{
#ಆದರೆ defined(FEAT_XCLIPBOARD) || defined(FEAT_CLIENTSERVER) \
	|| !defined(FEAT_NETBEANS_INTG)
    ಪೂರ್ಣಾಂಕ		argc = parmp->argc;
    ಅಕ್ಷರ	**argv = parmp->argv;
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 1; i < argc; i++)
    {
	ಆದರೆ (STRCMP(argv[i], "--") == 0)
	    ನಿಲ್ಲಿಸು;
# ifdef FEAT_XCLIPBOARD
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(argv[i], "-display") == 0
#  ಆದರೆ defined(FEAT_GUI_GTK)
		|| STRICMP(argv[i], "--display") == 0
#  endif
		)
	{
	    ಆದರೆ (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    xterm_display = argv[++i];
	}
# endif
# ifdef FEAT_CLIENTSERVER
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(argv[i], "--servername") == 0)
	{
	    ಆದರೆ (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    parmp->serverName_arg = (char_u *)argv[++i];
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(argv[i], "--serverlist") == 0)
	    parmp->serverArg = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    parmp->serverArg = TRUE;
#  ifdef FEAT_GUI
	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(argv[i], "-wait") != 0)
		// don't fork() when starting the GUI to edit files ourself
		gui.dofork = false;
#  endif
	}
#  ifdef FEAT_CLIENTSERVER_BACKENDS
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(argv[i], "--clientserver") == 0)
	{
	    char_u *arg;
	    ಆದರೆ (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    arg = (char_u *)argv[++i];

	    ಆದರೆ (STRICMP(arg, "socket") == 0)
		clientserver_ವಿಧಾನ = CLIENTSERVER_ವಿಧಾನ_SOCKET;
#   ifdef FEAT_X11
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(arg, "x11") == 0)
		clientserver_ವಿಧಾನ = CLIENTSERVER_ವಿಧಾನ_X11;
#   endif
#   ifdef MSWIN
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(arg, "mswin") == 0)
		clientserver_ವಿಧಾನ = CLIENTSERVER_ವಿಧಾನ_MSWIN;
#   endif
	    ಇಲ್ಲದಿದ್ದರೆ
		mainerr(ME_UNKNOWN_OPTION, arg);
	}
#  endif
# endif

# ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
#  ifdef FEAT_GUI_MSWIN
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(argv[i], "--windowid") == 0)
#  ಇಲ್ಲದಿದ್ದರೆ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(argv[i], "--socketid") == 0)
#  endif
	{
	    long_u	id;
	    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;

	    ಆದರೆ (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    ಆದರೆ (STRNICMP(argv[i+1], "0x", 2) == 0)
		ಎಣಿಕೆ_2 = ಅಕ್ಷರಸರಣಿ_ಓದು(&(argv[i + 1][2]), SCANF_HEX_LONG_U, &id);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಎಣಿಕೆ_2 = ಅಕ್ಷರಸರಣಿ_ಓದು(argv[i + 1], SCANF_DECIMAL_LONG_U, &id);
	    ಆದರೆ (ಎಣಿಕೆ_2 != 1)
		mainerr(ME_INVALID_ARG, (char_u *)argv[i]);
	    ಇಲ್ಲದಿದ್ದರೆ
#  ifdef FEAT_GUI_MSWIN
		win_socket_id = id;
#  elif !defined(USE_GTK4)
		gtk_socket_id = id;
#  endif
	    i++;
	}
# endif
# ಆದರೆ defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(argv[i], "--echo-wid") == 0)
	    echo_wid_arg = TRUE;
# endif
# ifndef FEAT_NETBEANS_INTG
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(argv[i], "-nb", (size_t)3) == 0)
	{
	    mch_errmsg(_("'-nb' cannot be used: not enabled at compile time\n"));
	    mch_exit(2);
	}
# endif

    }
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NO_VIM_MAIN
/*
 * Get a (optional) count for a Vim argument.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_ಸಂಖ್ಯೆ_arg(
    char_u	*p,	    // pointer to argument
    ಪೂರ್ಣಾಂಕ		*idx,	    // index in argument, is incremented
    ಪೂರ್ಣಾಂಕ		def)	    // default value
{
    ಆದರೆ (vim_isdigit(p[*idx]))
    {
	def = atoi((ಅಕ್ಷರ *)&(p[*idx]));
	ಆಗ (vim_isdigit(p[*idx]))
	    *idx = *idx + 1;
    }
    ಹಿಂತಿರುಗಿ def;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
parse_command_name(mparm_T *parmp)
{
    char_u	*initstr;

    initstr = gettail((char_u *)parmp->argv[0]);

# ifdef FEAT_EVAL
    ಗಣ_vim_var_string(VV_PROGNAME, initstr, -1);
    ಗಣ_progpath((char_u *)parmp->argv[0]);
# endif

    ಆದರೆ (TOLOWER_ASC(initstr[0]) == 'r')
    {
	restricted = TRUE;
	++initstr;
    }

    // Use evim mode for "evim" and "egvim", not for "editor".
    ಆದರೆ (TOLOWER_ASC(initstr[0]) == 'e'
	    && (TOLOWER_ASC(initstr[1]) == 'v'
		|| TOLOWER_ASC(initstr[1]) == 'g'))
    {
# ifdef FEAT_GUI
	gui.starting = true;
# endif
	parmp->evim_ಬಹುಲಕ = TRUE;
	++initstr;
    }

    // "gvim" starts the GUI.  Also accept "Gvim" for MS-Windows.
    ಆದರೆ (TOLOWER_ASC(initstr[0]) == 'g'
# ifdef VIMDLL
	    || mch_is_gui_executable()
# endif
       )
    {
	main_ಆರಂಭ_gui();
# ifdef FEAT_GUI
	++initstr;
# endif
# ifdef GUI_MAY_SPAWN
	gui.dospawn = false;	// No need to spawn a new process.
# endif
    }
# ifdef GUI_MAY_SPAWN
    ಇಲ್ಲದಿದ್ದರೆ
	gui.dospawn = true;	// Not "gvim". Need to spawn gvim.exe.
# endif


    ಆದರೆ (STRNICMP(initstr, "view", 4) == 0)
    {
	readonlymode = TRUE;
	curbuf->b_p_ro = TRUE;
	p_uc = 10000;			// don't update very often
	initstr += 4;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(initstr, "vim", 3) == 0)
	initstr += 3;

    // Catch "[r][g]vimdiff" and "[r][g]viewdiff".
    ಆದರೆ (STRICMP(initstr, "diff") == 0)
    {
# ifdef FEAT_DIFF
	parmp->diff_ಬಹುಲಕ = TRUE;
# ಇಲ್ಲದಿದ್ದರೆ
	mch_errmsg(_("This Vim was not compiled with the diff feature."));
	mch_errmsg("\n");
	mch_exit(2);
# endif
    }

    // Checking for "ex" here may catch some weird names, such as "vimex" or
    // "viewex", we assume the user knows that.
    ಆದರೆ (STRNICMP(initstr, "ex", 2) == 0)
    {
	ಆದರೆ (STRNICMP(initstr + 2, "im", 2) == 0)
	    exmode_ಸಕ್ರಿಯ = EXMODE_VIM;
	ಇಲ್ಲದಿದ್ದರೆ
	    exmode_ಸಕ್ರಿಯ = EXMODE_ಸಾಮಾನ್ಯ;
	change_compatible(TRUE);	// set 'compatible'
    }
}

/*
 * Scan the command line arguments.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
command_ಸಾಲು_ಪರಿಶೀಲನೆ(mparm_T *parmp)
{
    ಪೂರ್ಣಾಂಕ		argc = parmp->argc;
    ಅಕ್ಷರ	**argv = parmp->argv;
    ಪೂರ್ಣಾಂಕ		argv_idx;		// index in argv[n][]
    ಪೂರ್ಣಾಂಕ		had_minmin = FALSE;	// found "--" argument
    ಪೂರ್ಣಾಂಕ		want_ವಾದ;		// option argument with argument
    ಪೂರ್ಣಾಂಕ		c;
    char_u	*p = ಖಾಲಿ;
    ದೀರ್ಘ	n;

    --argc;
    ++argv;
    argv_idx = 1;	    // active option letter is argv[0][argv_idx]
    ಆಗ (argc > 0)
    {
	/*
	 * "+" or "+{number}" or "+/{pat}" or "+{command}" argument.
	 */
	ಆದರೆ (argv[0][0] == '+' && !had_minmin)
	{
	    ಆದರೆ (parmp->n_commands >= MAX_ARG_CMDS)
		mainerr(ME_EXTRA_CMD, ಖಾಲಿ);
	    argv_idx = -1;	    // skip to next argument
	    ಆದರೆ (argv[0][1] == NUL)
		parmp->commands[parmp->n_commands++] = (char_u *)"$";
	    ಇಲ್ಲದಿದ್ದರೆ
		parmp->commands[parmp->n_commands++] = (char_u *)&(argv[0][1]);
	}

	/*
	 * Optional argument.
	 */
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argv[0][0] == '-' && !had_minmin)
	{
	    want_ವಾದ = FALSE;
	    c = argv[0][argv_idx++];
# ಆದರೆ defined( VMS)
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
	    ಆದರೆ (c == '/')
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
	    ಆಯ್ಕೆ (c)
	    {
	    ಸ್ಥಿತಿ NUL:		// "vim -"  read from stdin
				// "ex -" silent mode
		ಆದರೆ (exmode_ಸಕ್ರಿಯ)
		    silent_ಬಹುಲಕ = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (parmp->edit_type != EDIT_NONE)
			mainerr(ME_TOO_MANY_ವಾದಗಳು, (char_u *)argv[0]);
		    parmp->edit_type = EDIT_STDIN;
		    read_cmd_fd = 2;	// read from stderr instead of stdin
		}
		argv_idx = -1;		// skip to next argument
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ '-':		// "--" don't take any more option arguments
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
		ಆದರೆ (STRICMP(argv[0] + argv_idx, "help") == 0)
		    usage();
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(argv[0] + argv_idx, "version") == 0)
		{
		    cmdline_ಅಗಲ = Columns = 80;   // need to init Columns
		    info_ಸಂದೇಶ = TRUE; // use mch_msg(), not mch_errmsg()
# ಆದರೆ defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
		    gui.starting = false; // not starting GUI, will exit
# endif
		    list_ಆವೃತ್ತಿ();
		    ಸಂದೇಶ_putchar('\n');
		    ಸಂದೇಶ_didout = FALSE;
		    mch_exit(0);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "clean", 5) == 0)
		{
		    parmp->use_vimrc = (char_u *)"DEFAULTS";
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)"NONE";
# endif
		    parmp->clean = TRUE;
		    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"vif",
						      0L, (char_u *)"NONE", 0);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "literal", 7) == 0)
		{
# ifdef EXPAND_FILENAMES
		    parmp->literal = TRUE;
# endif
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "nofork", 6) == 0)
		{
# ifdef FEAT_GUI
		    gui.dofork = false;	// don't fork() when starting GUI
# endif
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "noplugin", 8) == 0)
		    p_lpl = FALSE;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "not-a-term", 10) == 0)
		    parmp->not_a_ಪದ = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "gui-dialog-file", 15)
									 == 0)
		{
		    want_ವಾದ = TRUE;
		    argv_idx += 15;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "ttyfail", 7) == 0)
		    parmp->tty_fail = TRUE;
# ifdef FEAT_ಕೆಲಸ_CHANNEL
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "stdio-channel", 13) == 0)
		{
		    // Like "-es": no terminal, but the channel is served
		    // instead of reading Ex commands.
		    use_stdio_channel = TRUE;
		    exmode_ಸಕ್ರಿಯ = EXMODE_ಸಾಮಾನ್ಯ;
		    silent_ಬಹುಲಕ = TRUE;
#  ಆದರೆ defined(FEAT_GUI) && !defined(VIMDLL)
		    gui.starting = false;	// don't start GUI
#  endif
		}
# endif
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "cmd", 3) == 0)
		{
		    want_ವಾದ = TRUE;
		    argv_idx += 3;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "startuptime", 11) == 0)
		{
		    want_ವಾದ = TRUE;
		    argv_idx += 11;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "log", 3) == 0)
		{
		    want_ವಾದ = TRUE;
		    argv_idx += 3;
		}
# ifdef FEAT_CLIENTSERVER
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(argv[0] + argv_idx, "serverlist") == 0)
		    ; // already processed -- no arg
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(argv[0] + argv_idx, "servername") == 0
		       || STRICMP(argv[0] + argv_idx, "serversend") == 0
		       // Don't put this under FEAT_CLIENTSERVER_BACKENDS, just
		       // let it be ignored. Makes tests less complicated
		       || STRICMP(argv[0] + argv_idx, "clientserver") == 0
		       )
		{
		    // already processed -- snatch the following arg
		    ಆದರೆ (argc > 1)
		    {
			--argc;
			++argv;
		    }
		}
# endif
# ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
#  ifdef FEAT_GUI_GTK
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "socketid", 8) == 0)
#  ಇಲ್ಲದಿದ್ದರೆ
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "windowid", 8) == 0)
#  endif
		{
		    // already processed -- snatch the following arg
		    ಆದರೆ (argc > 1)
		    {
			--argc;
			++argv;
		    }
		}
# endif
# ifdef FEAT_GUI_GTK
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[0] + argv_idx, "echo-wid", 8) == 0)
		{
		    // already processed, skip
		}
# endif
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (argv[0][argv_idx])
			mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
		    had_minmin = TRUE;
		}
		ಆದರೆ (!want_ವಾದ)
		    argv_idx = -1;	// skip to next argument
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'A':		// "-A" start in Arabic mode
# ifdef FEAT_ARABIC
		ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"arabic", 1L, ಖಾಲಿ, 0);
# ಇಲ್ಲದಿದ್ದರೆ
		mch_errmsg(_(e_arabic_cannot_be_ಬಳಸಿದ_not_enabled_at_compile_ಸಮಯ));
		mch_exit(2);
# endif
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'b':		// "-b" binary mode
		// Needs to be effective before expanding file names, because
		// for Win32 this makes us edit a shortcut file itself,
		// instead of the file it links to.
		ಗಣ_options_bin(curbuf->b_p_bin, 1, 0);
		curbuf->b_p_bin = 1;	    // binary file I/O
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'C':		// "-C"  Compatible
		change_compatible(TRUE);
		has_dash_c_arg = TRUE;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'e':		// "-e" Ex mode
		exmode_ಸಕ್ರಿಯ = EXMODE_ಸಾಮಾನ್ಯ;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'E':		// "-E" Improved Ex mode
		exmode_ಸಕ್ರಿಯ = EXMODE_VIM;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'f':		// "-f"  GUI: run in foreground.  Amiga: open
				// window directly, not with newcli
# ifdef FEAT_GUI
		gui.dofork = false;	// don't fork() when starting GUI
# endif
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'g':		// "-g" start GUI
		main_ಆರಂಭ_gui();
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'F':		// "-F" was for Farsi mode
		mch_errmsg(_(e_farsi_support_has_been_removed));
		mch_exit(2);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ '?':		// "-?" give help message (for MS-Windows)
	    ಸ್ಥಿತಿ 'h':		// "-h" give help message
# ifdef FEAT_GUI_GNOME
		// Tell usage() to exit for "gvim".
		gui.starting = false;
# endif
		usage();
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'H':		// "-H" start in Hebrew mode: rl + hkmap set
# ifdef FEAT_RIGHTLEFT
		p_hkmap = TRUE;
		ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"rl", 1L, ಖಾಲಿ, 0);
# ಇಲ್ಲದಿದ್ದರೆ
		mch_errmsg(_(e_hebrew_cannot_be_ಬಳಸಿದ_not_enabled_at_compile_ಸಮಯ));
		mch_exit(2);
# endif
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'l':		// "-l" lisp mode, 'lisp' and 'showmatch' on
		ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"lisp", 1L, ಖಾಲಿ, 0);
		p_sm = TRUE;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'M':		// "-M"  no changes or writing of files
		reset_modifiable();
		// FALLTHROUGH

	    ಸ್ಥಿತಿ 'm':		// "-m"  no writing of files
		p_write = FALSE;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'y':		// "-y"  easy mode
# ifdef FEAT_GUI
		gui.starting = true;	// start GUI a bit later
# endif
		parmp->evim_ಬಹುಲಕ = TRUE;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'N':		// "-N"  Nocompatible
		change_compatible(FALSE);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'n':		// "-n" no swap file
# ifdef FEAT_NETBEANS_INTG
		// checking for "-nb", netbeans parameters
		ಆದರೆ (argv[0][argv_idx] == 'b')
		{
		    netbeansArg = argv[0];
		    argv_idx = -1;	    // skip to next argument
		}
		ಇಲ್ಲದಿದ್ದರೆ
# endif
		parmp->no_ಅದಲುಬದಲು_file = TRUE;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'p':		// "-p[N]" open N tab pages
# ifdef ಗುರಿ_API_MAC_OSX
		// For some reason on MacOS X, an argument like:
		// -psn_0_10223617 is passed in when invoke from Finder
		// or with the 'open' command
		ಆದರೆ (argv[0][argv_idx] == 's')
		{
		    argv_idx = -1; // bypass full -psn
		    main_ಆರಂಭ_gui();
		    ನಿಲ್ಲಿಸು;
		}
# endif
		// default is 0: open window for each file
		parmp->ಕಿಟಕಿ_ಎಣಿಕೆ = get_ಸಂಖ್ಯೆ_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->ಕಿಟಕಿ_layout = WIN_TABS;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'o':		// "-o[N]" open N horizontal split windows
		// default is 0: open window for each file
		parmp->ಕಿಟಕಿ_ಎಣಿಕೆ = get_ಸಂಖ್ಯೆ_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->ಕಿಟಕಿ_layout = WIN_HOR;
		ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'O':	// "-O[N]" open N vertical split windows
		// default is 0: open window for each file
		parmp->ಕಿಟಕಿ_ಎಣಿಕೆ = get_ಸಂಖ್ಯೆ_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->ಕಿಟಕಿ_layout = WIN_VER;
		ನಿಲ್ಲಿಸು;

# ifdef FEAT_QUICKFIX
	    ಸ್ಥಿತಿ 'q':		// "-q" QuickFix mode
		ಆದರೆ (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_ವಾದಗಳು, (char_u *)argv[0]);
		parmp->edit_type = EDIT_QF;
		ಆದರೆ (argv[0][argv_idx])		// "-q{errorfile}"
		{
		    parmp->use_ef = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc > 1)		// "-q {errorfile}"
		    want_ವಾದ = TRUE;
		ನಿಲ್ಲಿಸು;
# endif

	    ಸ್ಥಿತಿ 'R':		// "-R" readonly mode
		readonlymode = TRUE;
		curbuf->b_p_ro = TRUE;
		p_uc = 10000;			// don't update very often
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'r':		// "-r" recovery mode
	    ಸ್ಥಿತಿ 'L':		// "-L" recovery mode
		recoverymode = 1;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 's':
		ಆದರೆ (exmode_ಸಕ್ರಿಯ)	// "-s" silent (batch) mode
		    silent_ಬಹುಲಕ = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ		// "-s {scriptin}" read from script file
		    want_ವಾದ = TRUE;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 't':		// "-t {tag}" or "-t{tag}" jump to tag
		ಆದರೆ (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_ವಾದಗಳು, (char_u *)argv[0]);
		parmp->edit_type = EDIT_ಟ್ಯಾಗ್;
		ಆದರೆ (argv[0][argv_idx])		// "-t{tag}"
		{
		    parmp->tagname = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		ಇಲ್ಲದಿದ್ದರೆ				// "-t {tag}"
		    want_ವಾದ = TRUE;
		ನಿಲ್ಲಿಸು;

# ifdef FEAT_EVAL
	    ಸ್ಥಿತಿ 'D':		// "-D"		Debugging
		parmp->use_debug_break_level = 9999;
		ನಿಲ್ಲಿಸು;
# endif
# ifdef FEAT_DIFF
	    ಸ್ಥಿತಿ 'd':		// "-d"		'diff'
#  ifdef AMIGA
		// check for "-dev {device}"
		ಆದರೆ (argv[0][argv_idx] == 'e' && argv[0][argv_idx + 1] == 'v')
		    want_ವಾದ = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
#  endif
		    parmp->diff_ಬಹುಲಕ = TRUE;
		ನಿಲ್ಲಿಸು;
# endif
	    ಸ್ಥಿತಿ 'V':		// "-V{N}"	Verbose level
		// default is 10: a little bit verbose
		p_verbose = get_ಸಂಖ್ಯೆ_arg((char_u *)argv[0], &argv_idx, 10);
		ಆದರೆ (argv[0][argv_idx] != NUL)
		{
		    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"verbosefile",
					  0L, (char_u *)argv[0] + argv_idx, 0);
		    argv_idx = (ಪೂರ್ಣಾಂಕ)STRLEN(argv[0]);
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'v':		// "-v"  Vi-mode (as if called "vi")
		exmode_ಸಕ್ರಿಯ = 0;
# ಆದರೆ defined(FEAT_GUI) && !defined(VIMDLL)
		gui.starting = false;	// don't start GUI
# endif
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'w':		// "-w{number}"	set window height
				// "-w {scriptout}"	write to script
		ಆದರೆ (vim_isdigit(((char_u *)argv[0])[argv_idx]))
		{
		    n = get_ಸಂಖ್ಯೆ_arg((char_u *)argv[0], &argv_idx, 10);
		    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"window", n, ಖಾಲಿ, 0);
		    ನಿಲ್ಲಿಸು;
		}
		want_ವಾದ = TRUE;
		ನಿಲ್ಲಿಸು;

# ifdef FEAT_CRYPT
	    ಸ್ಥಿತಿ 'x':		// "-x"  encrypted reading/writing of files
		parmp->ask_for_ಕೀಲಿ = TRUE;
		ನಿಲ್ಲಿಸು;
# endif

	    ಸ್ಥಿತಿ 'X':		// "-X"  don't connect to X server
# ಆದರೆ (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
		x_no_connect = TRUE;
# endif
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'Y':		// "-Y" don't connect to Wayland compositor
# ಆದರೆ defined(FEAT_WAYLAND)
		wayland_no_connect = TRUE;
# endif
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'Z':		// "-Z"  restricted mode
		restricted = TRUE;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'c':		// "-c{command}" or "-c {command}" execute
				// command
		ಆದರೆ (argv[0][argv_idx] != NUL)
		{
		    ಆದರೆ (parmp->n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, ಖಾಲಿ);
		    parmp->commands[parmp->n_commands++] = (char_u *)argv[0]
								   + argv_idx;
		    argv_idx = -1;
		    ನಿಲ್ಲಿಸು;
		}
		// FALLTHROUGH
	    ಸ್ಥಿತಿ 'S':		// "-S {file}" execute Vim script
	    ಸ್ಥಿತಿ 'i':		// "-i {viminfo}" use for viminfo
# ifndef FEAT_DIFF
	    ಸ್ಥಿತಿ 'd':		// "-d {device}" device (for Amiga)
# endif
	    ಸ್ಥಿತಿ 'T':		// "-T {terminal}" terminal name
	    ಸ್ಥಿತಿ 'u':		// "-u {vimrc}" vim inits file
	    ಸ್ಥಿತಿ 'U':		// "-U {gvimrc}" gvim inits file
	    ಸ್ಥಿತಿ 'W':		// "-W {scriptout}" overwrite
# ifdef FEAT_GUI_MSWIN
	    ಸ್ಥಿತಿ 'P':		// "-P {parent title}" MDI parent
# endif
		want_ವಾದ = TRUE;
		ನಿಲ್ಲಿಸು;

	    ಪೂರ್ವನಿಯೋಜಿತ:
		mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
	    }

	    /*
	     * Handle option arguments with argument.
	     */
	    ಆದರೆ (want_ವಾದ)
	    {
		/*
		 * Check for garbage immediately after the option letter.
		 */
		ಆದರೆ (argv[0][argv_idx] != NUL)
		    mainerr(ME_GARBAGE, (char_u *)argv[0]);

		--argc;
		ಆದರೆ (argc < 1 && c != 'S')  // -S has an optional argument
		    mainerr_arg_missing((char_u *)argv[0]);
		++argv;
		argv_idx = -1;

		ಆಯ್ಕೆ (c)
		{
		ಸ್ಥಿತಿ 'c':	// "-c {command}" execute command
		ಸ್ಥಿತಿ 'S':	// "-S {file}" execute Vim script
		    ಆದರೆ (parmp->n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, ಖಾಲಿ);
		    ಆದರೆ (c == 'S')
		    {
			ಅಕ್ಷರ	*a;

			ಆದರೆ (argc < 1)
			    // "-S" without argument: use default session file
			    // name.
			    a = SESSION_FILE;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argv[0][0] == '-')
			{
			    // "-S" followed by another option: use default
			    // session file name.
			    a = SESSION_FILE;
			    ++argc;
			    --argv;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    a = argv[0];
			p = alloc(STRLEN(a) + 4);
			ಆದರೆ (p == ಖಾಲಿ)
			    mch_exit(2);
			sprintf((ಅಕ್ಷರ *)p, "so %s", a);
			parmp->cmds_tofree[parmp->n_commands] = TRUE;
			parmp->commands[parmp->n_commands++] = p;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			parmp->commands[parmp->n_commands++] =
							    (char_u *)argv[0];
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ '-':
		    ಆದರೆ (argv[-1][2] == 'c')
		    {
			// "--cmd {command}" execute command
			ಆದರೆ (parmp->n_pre_commands >= MAX_ARG_CMDS)
			    mainerr(ME_EXTRA_CMD, ಖಾಲಿ);
			parmp->pre_commands[parmp->n_pre_commands++] =
							    (char_u *)argv[0];
		    }
		    // --gui-dialog-file fname
		    ಆದರೆ (argv[-1][2] == 'g')
		    {
			// without GUI ignore the argument
# ifdef FEAT_GUI
			parmp->gui_dialog_file = (char_u *)argv[0];
# endif
		    }

		    // "--startuptime <file>" already handled
		    // "--log <file>" already handled
		    ನಿಲ್ಲಿಸು;

	    //	case 'd':   -d {device} is handled in mch_check_win() for the
	    //		    Amiga

# ifdef FEAT_QUICKFIX
		ಸ್ಥಿತಿ 'q':	// "-q {errorfile}" QuickFix mode
		    parmp->use_ef = (char_u *)argv[0];
		    ನಿಲ್ಲಿಸು;
# endif

		ಸ್ಥಿತಿ 'i':	// "-i {viminfo}" use for viminfo
		    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"vif",
						     0L, (char_u *)argv[0], 0);
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 's':	// "-s {scriptin}" read from script file
		    ಆದರೆ (scriptin[0] != ಖಾಲಿ)
		    {
scripterror:
			mch_errmsg(_("Attempt to open script file again: \""));
			mch_errmsg(argv[-1]);
			mch_errmsg(" ");
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    ಆದರೆ ((scriptin[0] = mch_fopen(argv[0], READBIN)) == ಖಾಲಿ)
		    {
			mch_errmsg(_("Cannot open for reading: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    ಆದರೆ (save_typebuf() == FAIL)
			mch_exit(2);	// out of memory
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 't':	// "-t {tag}"
		    parmp->tagname = (char_u *)argv[0];
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'T':	// "-T {terminal}" terminal name
		    /*
		     * The -T term argument is always available and when
		     * HAVE_TERMLIB is supported it overrides the environment
		     * variable TERM.
		     */
# ifdef FEAT_GUI
		    ಆದರೆ (ಪದ_is_gui((char_u *)argv[0]))
			gui.starting = true;	// start GUI a bit later
		    ಇಲ್ಲದಿದ್ದರೆ
# endif
			parmp->ಪದ = (char_u *)argv[0];
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'u':	// "-u {vimrc}" vim inits file
		    parmp->use_vimrc = (char_u *)argv[0];
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'U':	// "-U {gvimrc}" gvim inits file
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)argv[0];
# endif
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'w':	// "-w {nr}" 'window' value
				// "-w {scriptout}" append to script file
		    ಆದರೆ (vim_isdigit(*((char_u *)argv[0])))
		    {
			argv_idx = 0;
			n = get_ಸಂಖ್ಯೆ_arg((char_u *)argv[0], &argv_idx, 10);
			ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"window",
								   n, ಖಾಲಿ, 0);
			argv_idx = -1;
			ನಿಲ್ಲಿಸು;
		    }
		    // FALLTHROUGH
		ಸ್ಥಿತಿ 'W':	// "-W {scriptout}" overwrite script file
		    ಆದರೆ (scriptout != ಖಾಲಿ)
			ಹೋಗು scripterror;
		    ಆದರೆ ((scriptout = mch_fopen(argv[0],
				    c == 'w' ? APPENDBIN : WRITEBIN)) == ಖಾಲಿ)
		    {
			mch_errmsg(_("Cannot open for script output: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    ನಿಲ್ಲಿಸು;

# ifdef FEAT_GUI_MSWIN
		ಸ್ಥಿತಿ 'P':		// "-P {parent title}" MDI parent
		    gui_mch_ಗಣ_ಪೋಷಕ(argv[0]);
		    ನಿಲ್ಲಿಸು;
# endif
		}
	    }
	}

	/*
	 * File name argument.
	 */
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    argv_idx = -1;	    // skip to next argument

	    // Check for only one type of editing.
	    ಆದರೆ (parmp->edit_type != EDIT_NONE && parmp->edit_type != EDIT_FILE)
		mainerr(ME_TOO_MANY_ವಾದಗಳು, (char_u *)argv[0]);
	    parmp->edit_type = EDIT_FILE;

# ifdef MSWIN
	    // Remember if the argument was a full path before changing
	    // slashes to backslashes.
	    ಆದರೆ (argv[0][0] != NUL && argv[0][1] == ':' && argv[0][2] == '\\')
		parmp->ತುಂಬಿದ_ಮಾರ್ಗ = TRUE;
# endif

	    // Add the file to the global argument list.
	    ಆದರೆ (ga_grow(&global_alist.al_ga, 1) == FAIL
		    || (p = vim_strsave((char_u *)argv[0])) == ಖಾಲಿ)
		mch_exit(2);
# ifdef FEAT_DIFF
	    ಆದರೆ (parmp->diff_ಬಹುಲಕ && mch_isdir(p) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *ಬಾಲ;
		string_T    ret;

		ಬಾಲ = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(p, STRLEN(p), ಬಾಲ, STRLEN(ಬಾಲ), TRUE, &ret);
		ಆದರೆ (ret.string != ಖಾಲಿ)
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
	    ಆದರೆ (vim_strpbrk(p, "\\:") != ಖಾಲಿ && !ಮಾರ್ಗ_with_url(p))
	    {
		ಅಕ್ಷರ posix_ಮಾರ್ಗ[MAXPATHL];

#  ಆದರೆ CYGWIN_ಆವೃತ್ತಿ_DLL_MAJOR >= 1007
		cygwin_conv_ಮಾರ್ಗ(CCP_WIN_A_ಗಮ್ಯ_POSIX, p, posix_ಮಾರ್ಗ, MAXPATHL);
#  ಇಲ್ಲದಿದ್ದರೆ
		cygwin_conv_ಗಮ್ಯ_posix_ಮಾರ್ಗ(p, posix_ಮಾರ್ಗ);
#  endif
		vim_free(p);
		p = vim_strsave((char_u *)posix_ಮಾರ್ಗ);
		ಆದರೆ (p == ಖಾಲಿ)
		    mch_exit(2);
	    }
# endif

# ifdef USE_FNAME_CASE
	    // Make the case of the file name match the actual file.
	    fname_case(p, 0);
# endif

	    alist_ಕೂಡಿಸು(&global_alist, p,
# ifdef EXPAND_FILENAMES
		    parmp->literal ? 2 : 0	// add buffer nr after exp.
# ಇಲ್ಲದಿದ್ದರೆ
		    2		// add buffer number now and use curbuf
# endif
		    );

# ifdef MSWIN
	    {
		// Remember this argument has been added to the argument list.
		// Needed when 'encoding' is changed.
		ಬಳಸಿದ_file_arg(argv[0], parmp->literal, parmp->ತುಂಬಿದ_ಮಾರ್ಗ,
#  ifdef FEAT_DIFF
							    parmp->diff_ಬಹುಲಕ
#  ಇಲ್ಲದಿದ್ದರೆ
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
	ಆದರೆ (argv_idx <= 0 || argv[0][argv_idx] == NUL)
	{
	    --argc;
	    ++argv;
	    argv_idx = 1;
	}
    }

# ifdef FEAT_EVAL
    // If there is a "+123" or "-c" command, set v:swapcommand to the first
    // one.
    ಆದರೆ (parmp->n_commands > 0)
    {
	size_t  plen;

	plen = STRLEN(parmp->commands[0]) + 2;
	p = alloc(plen + 1);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    sprintf((ಅಕ್ಷರ *)p, ":%s\r", parmp->commands[0]);
	    ಗಣ_vim_var_string(VV_SWAPCOMMAND, p, (ಪೂರ್ಣಾಂಕ)plen);
	    vim_free(p);
	}
    }
# endif
}

/*
 * Print a warning if stdout is not a terminal.
 * When starting in Ex mode and commands come from a file, set silent_mode.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_tty(mparm_T *parmp)
{
    ಪೂರ್ಣಾಂಕ		ಒಳಹರಿವು_isatty;		// is active input a terminal?

    ಒಳಹರಿವು_isatty = mch_ಒಳಹರಿವು_isatty();
    ಆದರೆ (exmode_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (!ಒಳಹರಿವು_isatty)
	    silent_ಬಹುಲಕ = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (parmp->want_ತುಂಬಿದ_screen && (!stdout_isatty || !ಒಳಹರಿವು_isatty)
# ifdef FEAT_GUI
	    // don't want the delay when started from the desktop
	    && !gui.starting
# endif
	    && !parmp->not_a_ಪದ)
    {
# ifdef NBDEBUG
	/*
	 * This shouldn't be necessary. But if I run netbeans with the log
	 * output coming to the console and XOpenDisplay fails, I get vim
	 * trying to start with input/output to my console tty.  This fills my
	 * input buffer so fast I can't even kill the process in under 2
	 * minutes (and it beeps continuously the whole time :-)
	 */
	ಆದರೆ (netbeans_ಸಕ್ರಿಯ() && (!stdout_isatty || !ಒಳಹರಿವು_isatty))
	{
	    mch_errmsg(_("Vim: Error: Failure to start gvim from NetBeans\n"));
	    ನಿರ್ಗಮಿಸು(1);
	}
# endif
# ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	ಆದರೆ (
#  ifdef VIMDLL
	    !gui.starting &&
#  endif
	    is_cygpty_ಬಳಸಿದ())
	{
#  ಆದರೆ defined(HAVE_BIND_TEXTDOMAIN_CODESET) \
	&& defined(FEAT_GETTEXT)
	    ಅಕ್ಷರ    *s, *tofree = ಖಾಲಿ;

	    // Set the encoding of the error message based on $LC_ALL or
	    // other environment variables instead of 'encoding'.
	    // Note that the message is shown on a Cygwin terminal (e.g.
	    // mintty) which encoding is based on $LC_ALL or etc., not the
	    // current codepage used by normal Win32 console programs.
	    tofree = s = (ಅಕ್ಷರ *)enc_locale_env(ಖಾಲಿ);
	    ಆದರೆ (s == ಖಾಲಿ)
		s = "utf-8";	// Use "utf-8" by default.
	    (ಶೂನ್ಯ)bind_textdomain_codeset(VIMPACKAGE, s);
	    vim_free(tofree);
#  endif
	    mch_errmsg(_("Vim: Error: This version of Vim does not run in a Cygwin terminal\n"));
	    ನಿರ್ಗಮಿಸು(1);
	}
# endif
	ಆದರೆ (!stdout_isatty)
	    mch_errmsg(_("Vim: Warning: Output is not to a terminal\n"));
	ಆದರೆ (!ಒಳಹರಿವು_isatty)
	    mch_errmsg(_("Vim: Warning: Input is not from a terminal\n"));
	ಹೊರಹೋಗು_flush();
	ಆದರೆ (parmp->tty_fail && (!stdout_isatty || !ಒಳಹರಿವು_isatty))
	    ನಿರ್ಗಮಿಸು(1);
	ಆದರೆ (scriptin[0] == ಖಾಲಿ)
	    ui_delay(2005L, TRUE);
	ಸಮಯ_ಸಂದೇಶ("Warning delay");
    }
}

/*
 * Read text from stdin.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
read_stdin(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    i;

    // When getting the ATTENTION prompt here, use a dialog
    ಅದಲುಬದಲು_exists_action = SEA_DIALOG;

    no_wait_return = TRUE;
    i = ಸಂದೇಶ_didany;
    ಗಣ_buflisted(TRUE);

    // Create memfile and read from stdin.
    (ಶೂನ್ಯ)open_ಬಫರ್(TRUE, ಖಾಲಿ, 0);

    no_wait_return = FALSE;
    ಸಂದೇಶ_didany = i;
    ಸಮಯ_ಸಂದೇಶ("reading stdin");

    check_ಅದಲುಬದಲು_exists_action();

# ಆದರೆ !(defined(AMIGA) || defined(MACOS_X))
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
    ಸ್ಥಿರ ಶೂನ್ಯ
create_windows(mparm_T *parmp ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		dorewind;
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ = 0;

    /*
     * Create the number of windows that was requested.
     */
    ಆದರೆ (parmp->ಕಿಟಕಿ_ಎಣಿಕೆ == -1)	// was not set
	parmp->ಕಿಟಕಿ_ಎಣಿಕೆ = 1;
    ಆದರೆ (parmp->ಕಿಟಕಿ_ಎಣಿಕೆ == 0)
	parmp->ಕಿಟಕಿ_ಎಣಿಕೆ = GARGCOUNT;
    ಆದರೆ (parmp->ಕಿಟಕಿ_ಎಣಿಕೆ > 1)
    {
	// Don't change the windows if there was a command in .vimrc that
	// already split some windows
	ಆದರೆ (parmp->ಕಿಟಕಿ_layout == 0)
	    parmp->ಕಿಟಕಿ_layout = WIN_HOR;
	ಆದರೆ (parmp->ಕಿಟಕಿ_layout == WIN_TABS)
	{
	    parmp->ಕಿಟಕಿ_ಎಣಿಕೆ = make_tabpages(parmp->ಕಿಟಕಿ_ಎಣಿಕೆ);
	    ಸಮಯ_ಸಂದೇಶ("making tab pages");
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (firstwin->w_ಮುಂದಿನ == ಖಾಲಿ)
	{
	    parmp->ಕಿಟಕಿ_ಎಣಿಕೆ = make_windows(parmp->ಕಿಟಕಿ_ಎಣಿಕೆ,
					     parmp->ಕಿಟಕಿ_layout == WIN_VER);
	    ಸಮಯ_ಸಂದೇಶ("making windows");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    parmp->ಕಿಟಕಿ_ಎಣಿಕೆ = win_ಎಣಿಕೆ();
    }
    ಇಲ್ಲದಿದ್ದರೆ
	parmp->ಕಿಟಕಿ_ಎಣಿಕೆ = 1;

    ಆದರೆ (recoverymode)			// do recover
    {
	ಸಂದೇಶ_scroll = TRUE;		// scroll message up
	ml_recover(TRUE);
	ಆದರೆ (curbuf->b_ml.ml_mfp == ಖಾಲಿ) // failed
	    getout(1);
	do_modelines(0);		// do modelines
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
	ಆಗ (ಮುಗಿದ++ < 1000)
	{
	    ಆದರೆ (dorewind)
	    {
		ಆದರೆ (parmp->ಕಿಟಕಿ_layout == WIN_TABS)
		    goto_tabpage(1);
		ಇಲ್ಲದಿದ್ದರೆ
		    curwin = firstwin;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (parmp->ಕಿಟಕಿ_layout == WIN_TABS)
	    {
		ಆದರೆ (curtab->tp_ಮುಂದಿನ == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		goto_tabpage(0);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (curwin->w_ಮುಂದಿನ == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		curwin = curwin->w_ಮುಂದಿನ;
	    }
	    dorewind = FALSE;
	    curbuf = curwin->w_ಬಫರ್;
	    ಆದರೆ (curbuf->b_ml.ml_mfp == ಖಾಲಿ)
	    {
# ifdef FEAT_FOLDING
		// Set 'foldlevel' to 'foldlevelstart' if it's not negative.
		ಆದರೆ (p_fdls >= 0)
		    curwin->w_p_fdl = p_fdls;
# endif
		// When getting the ATTENTION prompt here, use a dialog
		ಅದಲುಬದಲು_exists_action = SEA_DIALOG;

		ಗಣ_buflisted(TRUE);

		// create memfile, read file
		(ಶೂನ್ಯ)open_ಬಫರ್(FALSE, ಖಾಲಿ, 0);

		ಆದರೆ (ಅದಲುಬದಲು_exists_action == SEA_QUIT)
		{
		    ಆದರೆ (got_int || only_one_ಕಿಟಕಿ())
		    {
			// abort selected or quit and only one window
			did_emsg = FALSE;   // avoid hit-enter prompt
			getout(1);
		    }
		    // We can't close the window, it would disturb what
		    // happens next.  Clear the file name and set the arg
		    // index to -1 to delete it later.
		    setfname(curbuf, ಖಾಲಿ, ಖಾಲಿ, FALSE);
		    curwin->w_arg_idx = -1;
		    ಅದಲುಬದಲು_exists_action = SEA_NONE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    handle_ಅದಲುಬದಲು_exists(ಖಾಲಿ);
		dorewind = TRUE;		// start again
	    }
	    ui_breakcheck();
	    ಆದರೆ (got_int)
	    {
		(ಶೂನ್ಯ)vgetc();	// only break the file loading, not the rest
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (parmp->ಕಿಟಕಿ_layout == WIN_TABS)
	    goto_tabpage(1);
	ಇಲ್ಲದಿದ್ದರೆ
	    curwin = firstwin;
	curbuf = curwin->w_ಬಫರ್;
	--autocmd_no_enter;
	--autocmd_no_leave;
    }
}

/*
 * If opened more than one window, start editing files in the other
 * windows.  make_windows() has already opened the windows.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
edit_buffers(
    mparm_T	*parmp,
    char_u	*cwd)			// current working dir
{
    ಪೂರ್ಣಾಂಕ		arg_idx;		// index in argument list
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		advance = TRUE;
    win_T	*win;
    char_u	*p_shm_save = ಖಾಲಿ;

    /*
     * Don't execute Win/Buf Enter/Leave autocommands here
     */
    ++autocmd_no_enter;
    ++autocmd_no_leave;

    // When w_arg_idx is -1 remove the window (see create_windows()).
    ಆದರೆ (curwin->w_arg_idx == -1)
    {
	win_close(curwin, TRUE);
	advance = FALSE;
    }

    arg_idx = 1;
    ಗಾಗಿ (i = 1; i < parmp->ಕಿಟಕಿ_ಎಣಿಕೆ; ++i)
    {
	ಆದರೆ (cwd != ಖಾಲಿ)
	    mch_chdir((ಅಕ್ಷರ *)cwd);
	// When w_arg_idx is -1 remove the window (see create_windows()).
	ಆದರೆ (curwin->w_arg_idx == -1)
	{
	    ++arg_idx;
	    win_close(curwin, TRUE);
	    advance = FALSE;
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ (advance)
	{
	    ಆದರೆ (parmp->ಕಿಟಕಿ_layout == WIN_TABS)
	    {
		ಆದರೆ (curtab->tp_ಮುಂದಿನ == ಖಾಲಿ)	// just checking
		    ನಿಲ್ಲಿಸು;
		goto_tabpage(0);
		// Temporarily reset 'shm' option to not print fileinfo when
		// loading the other buffers. This would overwrite the already
		// existing fileinfo for the first tab.
		ಆದರೆ (i == 1)
		{
		    ಅಕ್ಷರ buf[100];

		    p_shm_save = vim_strsave(p_shm);
		    vim_snprintf(buf, 100, "F%s", p_shm);
		    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"shm",
							 0L, (char_u *)buf, 0);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (curwin->w_ಮುಂದಿನ == ಖಾಲಿ)	// just checking
		    ನಿಲ್ಲಿಸು;
		win_enter(curwin->w_ಮುಂದಿನ, FALSE);
	    }
	}
	advance = TRUE;

	// Only open the file if there is no file in this window yet (that can
	// happen when .vimrc contains ":sall").
	ಆದರೆ (curbuf == firstwin->w_ಬಫರ್ || curbuf->b_ffname == ಖಾಲಿ)
	{
	    curwin->w_arg_idx = arg_idx;
	    // Edit file from arg list, if there is one.  When "Quit" selected
	    // at the ATTENTION prompt close the window.
	    ಅದಲುಬದಲು_exists_did_quit = FALSE;
	    (ಶೂನ್ಯ)do_ecmd(0, arg_idx < GARGCOUNT
			  ? alist_name(&GARGLIST[arg_idx]) : ಖಾಲಿ,
			  ಖಾಲಿ, ಖಾಲಿ, ECMD_LASTL, ECMD_HIDE, curwin);
	    ಆದರೆ (ಅದಲುಬದಲು_exists_did_quit)
	    {
		// abort or quit selected
		ಆದರೆ (got_int || only_one_ಕಿಟಕಿ())
		{
		    // abort selected and only one window
		    did_emsg = FALSE;   // avoid hit-enter prompt
		    getout(1);
		}
		win_close(curwin, TRUE);
		advance = FALSE;
	    }
	    ಆದರೆ (arg_idx == GARGCOUNT - 1)
		arg_had_ಕೊನೆಯ = TRUE;
	    ++arg_idx;
	}
	ui_breakcheck();
	ಆದರೆ (got_int)
	{
	    (ಶೂನ್ಯ)vgetc();	// only break the file loading, not the rest
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (p_shm_save != ಖಾಲಿ)
    {
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"shm", 0L, p_shm_save, 0);
	vim_free(p_shm_save);
    }

    ಆದರೆ (parmp->ಕಿಟಕಿ_layout == WIN_TABS)
	goto_tabpage(1);
    --autocmd_no_enter;

    // make the first window the current window
    win = firstwin;
# ಆದರೆ defined(FEAT_QUICKFIX)
    // Avoid making a preview window the current window.
    ಆಗ (win->w_p_pvw)
    {
	win = win->w_ಮುಂದಿನ;
	ಆದರೆ (win == ಖಾಲಿ)
	{
	    win = firstwin;
	    ನಿಲ್ಲಿಸು;
	}
    }
# endif
    win_enter(win, FALSE);

    --autocmd_no_leave;
    ಸಮಯ_ಸಂದೇಶ("editing files in windows");
    ಆದರೆ (parmp->ಕಿಟಕಿ_ಎಣಿಕೆ > 1 && parmp->ಕಿಟಕಿ_layout != WIN_TABS)
	win_equal(curwin, FALSE, 'b');	// adjust heights
}

/*
 * Execute the commands from --cmd arguments "cmds[cnt]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
exe_pre_commands(mparm_T *parmp)
{
    char_u	**cmds = parmp->pre_commands;
    ಪೂರ್ಣಾಂಕ		cnt = parmp->n_pre_commands;
    ಪೂರ್ಣಾಂಕ		i;
    ESTACK_CHECK_DECLARATION;

    ಆದರೆ (cnt <= 0)
	ಹಿಂತಿರುಗಿ;

    curwin->w_cursor.lnum = 0; // just in case..
    estack_ತಳ್ಳು(ETYPE_ವಾದಗಳು, (char_u *)_("pre-vimrc command line"), 0);
    ESTACK_CHECK_ಸಿದ್ಧತೆ;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CMDARG;
# endif
    ಗಾಗಿ (i = 0; i < cnt; ++i)
	do_cmdline_cmd(cmds[i]);
    ESTACK_CHECK_NOW;
    estack_ತೆಗೆ();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    ಸಮಯ_ಸಂದೇಶ("--cmd commands");
}

/*
 * Execute "+", "-c" and "-S" arguments.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
exe_commands(mparm_T *parmp)
{
    ಪೂರ್ಣಾಂಕ		i;
    ESTACK_CHECK_DECLARATION;

    /*
     * We start commands on line 0, make "vim +/pat file" match a
     * pattern on line 1.  But don't move the cursor when an autocommand
     * with g`" was used.
     */
    ಸಂದೇಶ_scroll = TRUE;
    ಆದರೆ (parmp->tagname == ಖಾಲಿ && curwin->w_cursor.lnum <= 1)
	curwin->w_cursor.lnum = 0;
    estack_ತಳ್ಳು(ETYPE_ವಾದಗಳು, (char_u *)"command line", 0);
    ESTACK_CHECK_ಸಿದ್ಧತೆ;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CARG;
    current_sctx.sc_seq = 0;
# endif
    ಗಾಗಿ (i = 0; i < parmp->n_commands; ++i)
    {
	do_cmdline_cmd(parmp->commands[i]);
	ಆದರೆ (parmp->cmds_tofree[i])
	    vim_free(parmp->commands[i]);
    }
    ESTACK_CHECK_NOW;
    estack_ತೆಗೆ();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    ಆದರೆ (curwin->w_cursor.lnum == 0)
	curwin->w_cursor.lnum = 1;

    ಆದರೆ (!exmode_ಸಕ್ರಿಯ)
	ಸಂದೇಶ_scroll = FALSE;

# ifdef FEAT_QUICKFIX
    // When started with "-q errorfile" jump to first error again.
    ಆದರೆ (parmp->edit_type == EDIT_QF)
	qf_jump(ಖಾಲಿ, 0, 0, FALSE);
# endif
    ಸಮಯ_ಸಂದೇಶ("executing command arguments");
}

/*
 * Source startup scripts.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಮೂಲ_startup_scripts(mparm_T *parmp)
{
    ಪೂರ್ಣಾಂಕ		i;

    /*
     * For "evim" source evim.vim first of all, so that the user can overrule
     * any things he doesn't like.
     */
    ಆದರೆ (parmp->evim_ಬಹುಲಕ)
    {
	(ಶೂನ್ಯ)do_ಮೂಲ((char_u *)EVIM_FILE, FALSE, DOSO_NONE, ಖಾಲಿ);
	ಸಮಯ_ಸಂದೇಶ("source evim file");
    }

    /*
     * If -u argument given, use only the initializations from that file and
     * nothing else.
     */
    ಆದರೆ (parmp->use_vimrc != ಖಾಲಿ)
    {
	ಆದರೆ (STRCMP(parmp->use_vimrc, "DEFAULTS") == 0)
	{
	    ಆದರೆ (do_ಮೂಲ((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE, ಖಾಲಿ)
									 != OK)
		emsg(_(e_failed_ಗಮ್ಯ_ಮೂಲ_defaults));
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(parmp->use_vimrc, "NONE") == 0
				     || STRCMP(parmp->use_vimrc, "NORC") == 0)
	{
# ifdef FEAT_GUI
	    ಆದರೆ (use_gvimrc == ಖಾಲಿ)	    // don't load gvimrc either
		use_gvimrc = parmp->use_vimrc;
# endif
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (do_ಮೂಲ(parmp->use_vimrc, FALSE, DOSO_NONE, ಖಾಲಿ) != OK)
		semsg(_(e_cannot_read_from_str_2), parmp->use_vimrc);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!silent_ಬಹುಲಕ)
    {
# ifdef AMIGA
	ರಚನೆ Process	*proc = (ರಚನೆ Process *)ಕಂಡುಹಿಡಿ_Task(0L);
	APTR		save_winptr = proc->pr_ಕಿಟಕಿ_Ptr;

	// Avoid a requester here for a volume that doesn't exist.
	proc->pr_ಕಿಟಕಿ_Ptr = (APTR)-1L;
# endif

	/*
	 * Get system wide defaults, if the file name is defined.
	 */
# ifdef SYS_VIMRC_FILE
	(ಶೂನ್ಯ)do_ಮೂಲ((char_u *)SYS_VIMRC_FILE, FALSE, DOSO_NONE, ಖಾಲಿ);
# endif
# ifdef MACOS_X
	(ಶೂನ್ಯ)do_ಮೂಲ((char_u *)"$VIMRUNTIME/macmap.vim", FALSE,
							      DOSO_NONE, ಖಾಲಿ);
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
	ಆದರೆ (process_env((char_u *)"VIMINIT", TRUE) != OK)
	{
	    ಆದರೆ (do_ಮೂಲ((char_u *)USR_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, ಖಾಲಿ) == FAIL
# ifdef USR_VIMRC_FILE2
		&& do_ಮೂಲ((char_u *)USR_VIMRC_FILE2, TRUE,
						      DOSO_VIMRC, ಖಾಲಿ) == FAIL
# endif
# ifdef XDG_VIMRC_FILE
		&& do_ಮೂಲ((char_u *)XDG_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, ಖಾಲಿ) == FAIL
# endif
# ifdef USR_VIMRC_FILE3
		&& do_ಮೂಲ((char_u *)USR_VIMRC_FILE3, TRUE,
						      DOSO_VIMRC, ಖಾಲಿ) == FAIL
# endif
# ifdef USR_VIMRC_FILE4
		&& do_ಮೂಲ((char_u *)USR_VIMRC_FILE4, TRUE,
						      DOSO_VIMRC, ಖಾಲಿ) == FAIL
# endif
		&& process_env((char_u *)"EXINIT", FALSE) == FAIL
		&& do_ಮೂಲ((char_u *)USR_EXRC_FILE, FALSE,
						       DOSO_NONE, ಖಾಲಿ) == FAIL
# ifdef USR_EXRC_FILE2
		&& do_ಮೂಲ((char_u *)USR_EXRC_FILE2, FALSE,
						       DOSO_NONE, ಖಾಲಿ) == FAIL
# endif
		&& !has_dash_c_arg)
	    {
		// When no .vimrc file was found: source defaults.vim.
		ಆದರೆ (do_ಮೂಲ((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE,
								 ಖಾಲಿ) == FAIL)
		    emsg(_(e_failed_ಗಮ್ಯ_ಮೂಲ_defaults));
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
	ಆದರೆ (p_exrc)
	{
# ಆದರೆ defined(UNIX) || defined(VMS)
	    // If ".vimrc" file is not owned by user, set 'secure' mode.
	    ಆದರೆ (!file_owned(VIMRC_FILE))
# endif
		secure = p_secure;

	    i = FAIL;
	    ಆದರೆ (fullpathcmp((char_u *)USR_VIMRC_FILE,
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
		i = do_ಮೂಲ((char_u *)VIMRC_FILE, TRUE, DOSO_VIMRC, ಖಾಲಿ);

	    ಆದರೆ (i == FAIL)
	    {
# ಆದರೆ defined(UNIX) || defined(VMS)
		// if ".exrc" is not owned by user set 'secure' mode
		ಆದರೆ (!file_owned(EXRC_FILE))
		    secure = p_secure;
		ಇಲ್ಲದಿದ್ದರೆ
		    secure = 0;
# endif
		ಆದರೆ (	   fullpathcmp((char_u *)USR_EXRC_FILE,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# ifdef USR_EXRC_FILE2
			&& fullpathcmp((char_u *)USR_EXRC_FILE2,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# endif
				)
		    (ಶೂನ್ಯ)do_ಮೂಲ((char_u *)EXRC_FILE, FALSE,
							      DOSO_NONE, ಖಾಲಿ);
	    }
	}
	ಆದರೆ (secure == 2)
	    need_wait_return = TRUE;
	secure = 0;
# ifdef AMIGA
	proc->pr_ಕಿಟಕಿ_Ptr = save_winptr;
# endif
    }
    ಸಮಯ_ಸಂದೇಶ("sourcing vimrc file(s)");
}

/*
 * Setup to start using the GUI.  Exit with an error when not available.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
main_ಆರಂಭ_gui(ಶೂನ್ಯ)
{
# ifdef FEAT_GUI
    gui.starting = true;	// start GUI a bit later
# ಇಲ್ಲದಿದ್ದರೆ
    mch_errmsg(_(e_gui_cannot_be_ಬಳಸಿದ_not_enabled_at_compile_ಸಮಯ));
    mch_errmsg("\n");
    mch_exit(2);
# endif
}

#ಆದರೆ_ಅಂತ್ಯ  // NO_VIM_MAIN

/*
 * Get an environment variable and execute it as Ex commands.
 * Returns FAIL if the environment variable was not executed, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
process_env(
    char_u	*env,
    ಪೂರ್ಣಾಂಕ		is_viminit) // when TRUE, called for VIMINIT
{
    char_u	*initstr;
    sctx_T	save_current_sctx;
    ESTACK_CHECK_DECLARATION;

    ಆದರೆ ((initstr = mch_getenv(env)) == ಖಾಲಿ || *initstr == NUL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (is_viminit)
	vimrc_ಸಿಕ್ಕಿತು(ಖಾಲಿ, ಖಾಲಿ);
    estack_ತಳ್ಳು(ETYPE_ENV, env, 0);
    ESTACK_CHECK_ಸಿದ್ಧತೆ;
    save_current_sctx = current_sctx;
    current_sctx.sc_ಆವೃತ್ತಿ = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    current_sctx.sc_sid = SID_ENV;
    current_sctx.sc_seq = 0;
    current_sctx.sc_lnum = 0;
#ಆದರೆ_ಅಂತ್ಯ

    do_cmdline_cmd(initstr);

    ESTACK_CHECK_NOW;
    estack_ತೆಗೆ();
    current_sctx = save_current_sctx;
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
/*
 * Return TRUE if we are certain the user owns the file "fname".
 * Used for ".vimrc" and ".exrc".
 * Use both stat() and lstat() for extra security.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
file_owned(ಅಕ್ಷರ *fname)
{
    stat_T	s;
# ifdef UNIX
    uid_t	uid = getuid();
# ಇಲ್ಲದಿದ್ದರೆ	 // VMS
    uid_t	uid = ((getgid() << 16) | getuid());
# endif

    ಹಿಂತಿರುಗಿ !(mch_stat(fname, &s) != 0 || s.st_uid != uid
# ifdef HAVE_LSTAT
	    || mch_lstat(fname, &s) != 0 || s.st_uid != uid
# endif
	    );
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Give an error message main_errors["n"] and exit.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mainerr(
    ಪೂರ್ಣಾಂಕ		n,	// one of the ME_ defines
    char_u	*str)	// extra argument or NULL
{
#ಆದರೆ defined(UNIX) || defined(VMS)
    reset_ಸಂಕೇತಗಳು();		// kill us with CTRL-C here, if you like
#ಆದರೆ_ಅಂತ್ಯ

    // If this is a Windows GUI executable, show an error dialog box.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VIMDLL
    gui.ಒಳ_use = mch_is_gui_executable();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    gui.starting = false;   // Needed to show as error.
#ಆದರೆ_ಅಂತ್ಯ

    init_long_ಆವೃತ್ತಿ();
    mch_errmsg(long_ಆವೃತ್ತಿ);
    mch_errmsg("\n");
    mch_errmsg(_(main_errors[n]));
    ಆದರೆ (str != ಖಾಲಿ)
    {
	mch_errmsg(": \"");
	mch_errmsg((ಅಕ್ಷರ *)str);
	mch_errmsg("\"");
    }
    mch_errmsg(_("\nMore info with: \"vim -h\"\n"));

    mch_exit(1);
}

    ಶೂನ್ಯ
mainerr_arg_missing(char_u *str)
{
    mainerr(ME_ARG_MISSING, str);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NO_VIM_MAIN
/*
 * print a message with three spaces prepended and '\n' appended.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
main_ಸಂದೇಶ(ಅಕ್ಷರ *s)
{
    mch_ಸಂದೇಶ("   ");
    mch_ಸಂದೇಶ(s);
    mch_ಸಂದೇಶ("\n");
}

/*
 * Print messages for "vim -h" or "vim --help" and exit.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
usage(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಸ್ಥಿರ ಅಕ್ಷರ	*(use[]) =
    {
	N_("[file ..]       edit specified file(s)"),
	N_("-               read text from stdin"),
	N_("-t tag          edit file where tag is defined"),
# ifdef FEAT_QUICKFIX
	N_("-q [errorfile]  edit file with first error")
# endif
    };

# ಆದರೆ defined(UNIX) || defined(VMS)
    reset_ಸಂಕೇತಗಳು();		// kill us with CTRL-C here, if you like
# endif

    init_long_ಆವೃತ್ತಿ();
    mch_ಸಂದೇಶ(long_ಆವೃತ್ತಿ);
    mch_ಸಂದೇಶ(_("\n\nUsage:"));
    ಗಾಗಿ (i = 0; ; ++i)
    {
	mch_ಸಂದೇಶ(_(" vim [arguments] "));
	mch_ಸಂದೇಶ(_(use[i]));
	ಆದರೆ (i == ARRAY_ಉದ್ದಳತೆ(use) - 1)
	    ನಿಲ್ಲಿಸು;
	mch_ಸಂದೇಶ(_("\n   or:"));
    }
# ಆದರೆ defined( VMS)
    mch_ಸಂದೇಶ(_("\nWhere command is down-cased, prepend / (like: -/R) to treat flag as upper-case."));
    mch_ಸಂದೇಶ(_("\nOr, where supported, SET PROC/PARSE=EXT, or else quote upper-case material."));
# endif /* defined( VMS) */

    mch_ಸಂದೇಶ(_("\n\nArguments:\n"));
    main_ಸಂದೇಶ(_("--\t\t\tOnly file names after this"));
# ifdef EXPAND_FILENAMES
    main_ಸಂದೇಶ(_("--literal\t\tDon't expand wildcards"));
# endif
# ifdef FEAT_OLE
    main_ಸಂದೇಶ(_("-register\t\tRegister this gvim for OLE"));
    main_ಸಂದೇಶ(_("-unregister\t\tUnregister gvim for OLE"));
# endif
# ifdef FEAT_GUI
    main_ಸಂದೇಶ(_("-g\t\t\tRun using GUI (like \"gvim\")"));
    main_ಸಂದೇಶ(_("-f  or  --nofork\tForeground: Don't fork when starting GUI"));
# endif
    main_ಸಂದೇಶ(_("-v\t\t\tVi mode (like \"vi\")"));
    main_ಸಂದೇಶ(_("-e\t\t\tEx mode (like \"ex\")"));
    main_ಸಂದೇಶ(_("-E\t\t\tImproved Ex mode"));
    main_ಸಂದೇಶ(_("-s\t\t\tSilent (batch) mode (only for \"ex\")"));
# ifdef FEAT_DIFF
    main_ಸಂದೇಶ(_("-d\t\t\tDiff mode (like \"vimdiff\")"));
# endif
    main_ಸಂದೇಶ(_("-y\t\t\tEasy mode (like \"evim\", modeless)"));
    main_ಸಂದೇಶ(_("-R\t\t\tReadonly mode (like \"view\")"));
    main_ಸಂದೇಶ(_("-Z\t\t\tRestricted mode (like \"rvim\")"));
    main_ಸಂದೇಶ(_("-m\t\t\tModifications (writing files) not allowed"));
    main_ಸಂದೇಶ(_("-M\t\t\tModifications in text not allowed"));
    main_ಸಂದೇಶ(_("-b\t\t\tBinary mode"));
    main_ಸಂದೇಶ(_("-l\t\t\tLisp mode"));
    main_ಸಂದೇಶ(_("-C\t\t\tCompatible with Vi: 'compatible'"));
    main_ಸಂದೇಶ(_("-N\t\t\tNot fully Vi compatible: 'nocompatible'"));
    main_ಸಂದೇಶ(_("-V[N][fname]\t\tBe verbose [level N] [log messages to fname]"));
# ifdef FEAT_EVAL
    main_ಸಂದೇಶ(_("-D\t\t\tDebugging mode"));
# endif
    main_ಸಂದೇಶ(_("-n\t\t\tNo swap file, use memory only"));
    main_ಸಂದೇಶ(_("-r\t\t\tList swap files and exit"));
    main_ಸಂದೇಶ(_("-r (with file name)\tRecover crashed session"));
    main_ಸಂದೇಶ(_("-L\t\t\tSame as -r"));
# ifdef AMIGA
    main_ಸಂದೇಶ(_("-f\t\t\tDon't use newcli to open window"));
    main_ಸಂದೇಶ(_("-dev <device>\t\tUse <device> for I/O"));
# endif
# ifdef FEAT_ARABIC
    main_ಸಂದೇಶ(_("-A\t\t\tStart in Arabic mode"));
# endif
# ifdef FEAT_RIGHTLEFT
    main_ಸಂದೇಶ(_("-H\t\t\tStart in Hebrew mode"));
# endif
    main_ಸಂದೇಶ(_("-T <terminal>\tSet terminal type to <terminal>"));
    main_ಸಂದೇಶ(_("--not-a-term\t\tSkip warning for input/output not being a terminal"));
# ifdef FEAT_GUI
    main_ಸಂದೇಶ(_("--gui-dialog-file {fname}  For testing: write dialog text"));
# endif
    main_ಸಂದೇಶ(_("--ttyfail\t\tExit if input or output is not a terminal"));
# ifdef FEAT_ಕೆಲಸ_CHANNEL
    main_ಸಂದೇಶ(_("--stdio-channel\tUse stdin and stdout as a channel"));
# endif
    main_ಸಂದೇಶ(_("-u <vimrc>\t\tUse <vimrc> instead of any .vimrc"));
# ifdef FEAT_GUI
    main_ಸಂದೇಶ(_("-U <gvimrc>\t\tUse <gvimrc> instead of any .gvimrc"));
# endif
    main_ಸಂದೇಶ(_("--noplugin\t\tDon't load plugin scripts"));
    main_ಸಂದೇಶ(_("-p[N]\t\tOpen N tab pages (default: one for each file)"));
    main_ಸಂದೇಶ(_("-o[N]\t\tOpen N windows (default: one for each file)"));
    main_ಸಂದೇಶ(_("-O[N]\t\tLike -o but split vertically"));
    main_ಸಂದೇಶ(_("+\t\t\tStart at end of file"));
    main_ಸಂದೇಶ(_("+<lnum>\t\tStart at line <lnum>"));
    main_ಸಂದೇಶ(_("--cmd <command>\tExecute <command> before loading any vimrc file"));
    main_ಸಂದೇಶ(_("-c <command>\t\tExecute <command> after loading the first file"));
    main_ಸಂದೇಶ(_("-S <session>\t\tSource file <session> after loading the first file"));
    main_ಸಂದೇಶ(_("-s <scriptin>\tRead Normal mode commands from file <scriptin>"));
    main_ಸಂದೇಶ(_("-w <scriptout>\tAppend all typed commands to file <scriptout>"));
    main_ಸಂದೇಶ(_("-W <scriptout>\tWrite all typed commands to file <scriptout>"));
# ifdef FEAT_CRYPT
    main_ಸಂದೇಶ(_("-x\t\t\tEdit encrypted files"));
# endif
# ಆದರೆ (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
#  ಆದರೆ defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
    main_ಸಂದೇಶ(_("-display <display>\tConnect Vim to this particular X-server"));
#  endif
    main_ಸಂದೇಶ(_("-X\t\t\tDo not connect to X server"));
# endif
# ಆದರೆ defined(FEAT_WAYLAND)
    main_ಸಂದೇಶ(_("-Y\t\t\tDo not connect to Wayland compositor"));
# endif
# ifdef FEAT_CLIENTSERVER
#  ifdef FEAT_CLIENTSERVER_BACKENDS
    main_ಸಂದೇಶ(_("--clientserver <socket|x11|mswin> Backend for clientserver communication"));
#  endif
    main_ಸಂದೇಶ(_("--remote <files>\tEdit <files> in a Vim server if possible"));
    main_ಸಂದೇಶ(_("--remote-silent <files>  Same, don't complain if there is no server"));
    main_ಸಂದೇಶ(_("--remote-wait <files>  As --remote but wait for files to have been edited"));
    main_ಸಂದೇಶ(_("--remote-wait-silent <files>  Same, don't complain if there is no server"));
    main_ಸಂದೇಶ(_("--remote-tab[-wait][-silent] <files>  As --remote but use tab page per file"));
    main_ಸಂದೇಶ(_("--remote-send <keys>\tSend <keys> to a Vim server and exit"));
    main_ಸಂದೇಶ(_("--remote-expr <expr>\tEvaluate <expr> in a Vim server and print result"));
    main_ಸಂದೇಶ(_("--serverlist\t\tList available Vim server names and exit"));
    main_ಸಂದೇಶ(_("--servername <name>\tSend to/become the Vim server <name>"));
# endif
# ifdef STARTUPTIME
    main_ಸಂದೇಶ(_("--startuptime <file>\tWrite startup timing messages to <file>"));
# endif
# ifdef FEAT_ಕೆಲಸ_CHANNEL
    main_ಸಂದೇಶ(_("--log <file>\t\tStart logging to <file> early"));
# endif
# ifdef FEAT_VIMINFO
    main_ಸಂದೇಶ(_("-i <viminfo>\t\tUse <viminfo> instead of .viminfo"));
# endif
    main_ಸಂದೇಶ(_("--clean\t\t'nocompatible', Vim defaults, no plugins, no viminfo"));
    main_ಸಂದೇಶ(_("-h  or  --help\tPrint Help (this message) and exit"));
    main_ಸಂದೇಶ(_("--version\t\tPrint version information and exit"));

# ifdef FEAT_GUI_X11
#  ifdef FEAT_GUI_MOTIF
    mch_ಸಂದೇಶ(_("\nArguments recognised by gvim (Motif version):\n"));
#  endif
    main_ಸಂದೇಶ(_("-display <display>\tRun Vim on <display>"));
    main_ಸಂದೇಶ(_("-iconic\t\tStart Vim iconified"));
    main_ಸಂದೇಶ(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_ಸಂದೇಶ(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_ಸಂದೇಶ(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_ಸಂದೇಶ(_("-boldfont <font>\tUse <font> for bold text"));
    main_ಸಂದೇಶ(_("-italicfont <font>\tUse <font> for italic text"));
    main_ಸಂದೇಶ(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
    main_ಸಂದೇಶ(_("-borderwidth <width>\tUse a border width of <width> (also: -bw)"));
    main_ಸಂದೇಶ(_("-scrollbarwidth <width>  Use a scrollbar width of <width> (also: -sw)"));
    main_ಸಂದೇಶ(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_ಸಂದೇಶ(_("+reverse\t\tDon't use reverse video (also: +rv)"));
    main_ಸಂದೇಶ(_("-xrm <resource>\tSet the specified resource"));
# endif // FEAT_GUI_X11
# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
    mch_ಸಂದೇಶ(_("\nArguments recognised by gvim (GTK4 version):\n"));
#  ಇಲ್ಲದಿದ್ದರೆ
    mch_ಸಂದೇಶ(_("\nArguments recognised by gvim (GTK+ version):\n"));
#  endif
    main_ಸಂದೇಶ(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_ಸಂದೇಶ(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_ಸಂದೇಶ(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_ಸಂದೇಶ(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
#  ifdef USE_GTK4
    main_ಸಂದೇಶ("--prg-name <name>\tSet GTK program name");
#  ಇಲ್ಲದಿದ್ದರೆ
    main_ಸಂದೇಶ(_("-iconic\t\tStart Vim iconified"));
    main_ಸಂದೇಶ(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_ಸಂದೇಶ(_("-display <display>\tRun Vim on <display> (also: --display)"));
    main_ಸಂದೇಶ(_("--role <role>\tSet a unique role to identify the main window"));
    main_ಸಂದೇಶ(_("--socketid <xid>\tOpen Vim inside another GTK widget"));
    main_ಸಂದೇಶ(_("--echo-wid\t\tMake gvim echo the Window ID on stdout"));
#  endif
# endif
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
    ಆದರೆ (gui.starting)
#  endif
    {
	main_ಸಂದೇಶ(_("-P <parent title>\tOpen Vim inside parent application"));
	main_ಸಂದೇಶ(_("--windowid <HWND>\tOpen Vim inside another win32 widget"));
    }
# endif

# ifdef FEAT_GUI_GNOME
    // Gnome gives extra messages for --help if we continue, but not for -h.
    ಆದರೆ (gui.starting)
    {
	mch_ಸಂದೇಶ("\n");
	gui.dofork = false;
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	mch_exit(0);
}

/*
 * Check the result of the ATTENTION dialog:
 * When "Quit" selected, exit Vim.
 * When "Recover" selected, recover the file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_ಅದಲುಬದಲು_exists_action(ಶೂನ್ಯ)
{
    ಆದರೆ (ಅದಲುಬದಲು_exists_action == SEA_QUIT)
	getout(1);
    handle_ಅದಲುಬದಲು_exists(ಖಾಲಿ);
}

#ಆದರೆ_ಅಂತ್ಯ // NO_VIM_MAIN

#ಆದರೆ !defined(NO_VIM_MAIN) && defined(FEAT_EVAL)
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_progpath(char_u *argv0)
{
    char_u *val = argv0;

# ifdef MSWIN
    // A relative path containing a "/" will become invalid when using ":cd",
    // turn it into a full path.
    // On MS-Windows "vim" should be expanded to "vim.exe", thus always do
    // this.
    char_u *ಮಾರ್ಗ = ಖಾಲಿ;

    ಆದರೆ (mch_can_exe(argv0, &ಮಾರ್ಗ, FALSE) && ಮಾರ್ಗ != ಖಾಲಿ)
	val = ಮಾರ್ಗ;
# ಇಲ್ಲದಿದ್ದರೆ
    char_u	buf[MAXPATHL + 1];
#  ifdef PROC_EXE_LINK
    ಅಕ್ಷರ	linkbuf[MAXPATHL + 1];
    ssize_t	len;

    len = readlink(PROC_EXE_LINK, linkbuf, MAXPATHL);
    ಆದರೆ (len > 0)
    {
	linkbuf[len] = NUL;
	val = (char_u *)linkbuf;
    }
#  endif

    ಆದರೆ (!mch_is_ತುಂಬಿದ_Name(val))
    {
	ಆದರೆ (gettail(val) != val
			   && vim_ತುಂಬಿದ_Name(val, buf, MAXPATHL, TRUE) != FAIL)
	    val = buf;
    }
# endif

    ಗಣ_vim_var_string(VV_PROGPATH, val, -1);

# ifdef MSWIN
    vim_free(ಮಾರ್ಗ);
# endif
}

#ಆದರೆ_ಅಂತ್ಯ // NO_VIM_MAIN
