/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXTERN
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __CYGWIN__
# include <cygwin/version.h>
# include <sys/cygwin.h>	// for cygwin_conv_to_posix_path() and/or
				// cygwin_conv_path()
# include <limits.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# include "iscygpty.h"
#ਜੇ_ਅੰਤ

// Values for edit_type.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EDIT_NONE   0	    // no edit type yet
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EDIT_FILE   1	    // file name argument[s] given, use argument list
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EDIT_STDIN  2	    // read file from stdin
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EDIT_ਟੈਗ    3	    // tag name argument given, use tagname
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EDIT_QF	    4	    // start in quickfix mode

#ਜੇ (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
ਸਥਿਰ ਪੂਰਨ file_owned(ਅੱਖਰ *fname);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ mainerr(ਪੂਰਨ, char_u *);
ਸਥਿਰ ਖਾਲੀ early_arg_ਜਾਂਚ(mparm_T *parmp);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NO_VIM_MAIN
ਸਥਿਰ ਖਾਲੀ usage(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ parse_command_name(mparm_T *parmp);
ਸਥਿਰ ਖਾਲੀ command_ਸਤਰ_ਜਾਂਚ(mparm_T *parmp);
ਸਥਿਰ ਖਾਲੀ check_tty(mparm_T *parmp);
ਸਥਿਰ ਖਾਲੀ read_stdin(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ create_windows(mparm_T *parmp);
ਸਥਿਰ ਖਾਲੀ edit_buffers(mparm_T *parmp, char_u *cwd);
ਸਥਿਰ ਖਾਲੀ exe_pre_commands(mparm_T *parmp);
ਸਥਿਰ ਖਾਲੀ exe_commands(mparm_T *parmp);
ਸਥਿਰ ਖਾਲੀ ਸਰੋਤ_startup_scripts(mparm_T *parmp);
ਸਥਿਰ ਖਾਲੀ main_ਸ਼ੁਰੂ_gui(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ check_ਅਦਲਾ_ਬਦਲੀ_exists_action(ਖਾਲੀ);
# ifdef FEAT_EVAL
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_progpath(char_u *argv0);
# endif
#ਜੇ_ਅੰਤ


/*
 * Different types of error messages.
 */
ਸਥਿਰ ਅੱਖਰ *(main_errors[]) =
{
    N_("Unknown option argument"),
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ME_UNKNOWN_OPTION	0
    N_("Too many edit arguments"),
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ME_TOO_MANY_ਦਲੀਲਾਂ	1
    N_("Argument missing after"),
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ME_ARG_MISSING		2
    N_("Garbage after option argument"),
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ME_GARBAGE		3
    N_("Too many \"+command\", \"-c command\" or \"--cmd command\" arguments"),
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ME_EXTRA_CMD		4
    N_("Invalid argument for"),
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ME_INVALID_ARG		5
};

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO		// don't want a prototype for main()

// Various parameters passed between main() and other functions.
ਸਥਿਰ mparm_T	params;

# ifdef _IOLBF
ਸਥਿਰ ਖਾਲੀ *s_vbuf = ਨਿਰਾ;		// buffer for setvbuf()
# endif

# ifndef NO_VIM_MAIN	// skip this for unittests

ਸਥਿਰ char_u *ਸ਼ੁਰੂ_dir = ਨਿਰਾ;	// current working dir on startup

ਸਥਿਰ ਪੂਰਨ has_dash_c_arg = FALSE;

#  ifdef VIMDLL
__declspec(dllexport)
#  endif
    ਪੂਰਨ
#  ifdef MSWIN
VimMain
#  ਨਹੀਂਤਾਂ
ਮੁੱਖ
#  endif
(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
#  ਜੇ defined(STARTUPTIME) || defined(CLEAN_RUNTIMEPATH)
    ਪੂਰਨ		i;
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
    argc = get_cmd_ਦਲੀਲਾਂ_W(&argv);
#  endif

    // Many variables are in "params" so that we can pass them to invoked
    // functions without a lot of arguments.  "argc" and "argv" are also
    // copied, so that they can be changed.
    CLEAR_ਖੇਤਰ(params);
    params.argc = argc;
    params.argv = argv;
    params.want_ਭਰਿਆ_screen = TRUE;
#  ifdef FEAT_EVAL
    params.use_debug_break_level = -1;
#  endif
    params.ਖਿੜਕੀ_ਗਿਣਤੀ = -1;

    autocmd_init();

#  ifdef FEAT_RUBY
    {
	ਪੂਰਨ ruby_ਢੇਰ_ਸ਼ੁਰੂ;
	vim_ruby_init((ਖਾਲੀ *)&ruby_ਢੇਰ_ਸ਼ੁਰੂ);
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

#  ਜੇ defined(STARTUPTIME) || defined(FEAT_ਕੰਮ_CHANNEL)
    // Need to find "--startuptime" and "--log" before actually parsing
    // arguments.
    ਲਈ (i = 1; i < argc - 1; ++i)
    {
#   ifdef STARTUPTIME
	ਜੇ (STRICMP(argv[i], "--startuptime") == 0 && ਸਮਾਂ_fd == ਨਿਰਾ)
	{
	    ਸਮਾਂ_fd = mch_fopen(argv[i + 1], "a");
	    ਸਮਾਂ_ਸੁਨੇਹਾ("--- VIM STARTING ---");
	}
#   endif
#   ifdef FEAT_EVAL
	ਜੇ (STRICMP(argv[i], "--log") == 0)
	    ch_logfile((char_u *)(argv[i + 1]), (char_u *)"ao");
#   endif
    }
#  endif

#  ifdef CLEAN_RUNTIMEPATH
    // Need to find "--clean" before actually parsing arguments.
    ਲਈ (i = 1; i < argc; ++i)
	ਜੇ (STRICMP(argv[i], "--clean") == 0)
	{
	    params.clean = TRUE;
	    ਰੋਕੋ;
	}
#  endif
#  ifdef MSWIN
    // Need to find "-register" and "-unregister" before loading any libraries.
    ਲਈ (i = 1; i < argc; ++i)
	ਜੇ ((STRICMP(argv[i] + 1, "register") == 0
				    || STRICMP(argv[i] + 1, "unregister") == 0)
		&& (argv[i][0] == '-' || argv[i][0] == '/'))
	{
	    ਮਿਲਿਆ_ਰਜਿਸਟਰ_arg = TRUE;
	    ਰੋਕੋ;
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
    command_ਸਤਰ_ਜਾਂਚ(&params);
    ਸਮਾਂ_ਸੁਨੇਹਾ("parsing arguments");

    /*
     * On some systems, when we compile with the GUI, we always use it.  On Mac
     * there is no terminal version, and on Windows we can't fork one off with
     * :gui.
     */
#  ifdef ALWAYS_USE_GUI
    gui.starting = true;
#  ਨਹੀਂਤਾਂ
#   ਜੇ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    /*
     * Check if the GUI can be started.  Reset gui.starting if not.
     * Don't know about other systems, stay on the safe side and don't check.
     */
    ਜੇ (gui.starting)
    {
	ਜੇ (gui_init_check() == FAIL)
	{
	    gui.starting = false;

	    // When running "evim" or "gvim -y" we need the menus, exit if we
	    // don't have them.
	    ਜੇ (params.evim_ਬਹੁਲਕ)
		mch_exit(1);
	}
    }
#   endif
#  endif

    ਜੇ (GARGCOUNT > 0)
    {
#  ifdef EXPAND_FILENAMES
	/*
	 * Expand wildcards in file names.
	 */
	ਜੇ (!params.literal)
	{
	    ਸ਼ੁਰੂ_dir = alloc(MAXPATHL);
	    ਜੇ (ਸ਼ੁਰੂ_dir != ਨਿਰਾ)
		mch_dirname(ਸ਼ੁਰੂ_dir, MAXPATHL);
	    // Temporarily add '(' and ')' to 'isfname'.  These are valid
	    // filename characters but are excluded from 'isfname' to make
	    // "gf" work on a file name in parentheses (e.g.: see vim.h).
	    do_cmdline_cmd((char_u *)":set isf+=(,)");
	    alist_expand(ਨਿਰਾ, 0);
	    do_cmdline_cmd((char_u *)":set isf&");
	    ਜੇ (ਸ਼ੁਰੂ_dir != ਨਿਰਾ)
		mch_chdir((ਅੱਖਰ *)ਸ਼ੁਰੂ_dir);
	}
#  endif
	params.fname = alist_name(&GARGLIST[0]);
    }

#  ifdef MSWIN
    {
	// Remember the number of entries in the argument list.  If it changes
	// we don't react on setting 'encoding'.
	ਸੈੱਟ_alist_ਗਿਣਤੀ();
    }
#  endif

#  ifdef MSWIN
    ਜੇ (GARGCOUNT == 1 && params.ਭਰਿਆ_ਰਸਤਾ)
    {
	/*
	 * If there is one filename, fully qualified, we have very probably
	 * been invoked from explorer, so change to the file's directory.
	 * Hint: to avoid this when typing a command use a forward slash.
	 * If the cd fails, it doesn't matter.
	 */
	ਜੇ (vim_chdirfile(params.fname, "drop") == OK)
	    ਆਖਰੀ_chdir_reason = "drop";
	ਜੇ (ਸ਼ੁਰੂ_dir != ਨਿਰਾ)
	    mch_dirname(ਸ਼ੁਰੂ_dir, MAXPATHL);
    }
#  endif
    ਸਮਾਂ_ਸੁਨੇਹਾ("expanding arguments");

#  ifdef FEAT_DIFF
    ਜੇ (params.diff_ਬਹੁਲਕ && params.ਖਿੜਕੀ_ਗਿਣਤੀ == -1)
	params.ਖਿੜਕੀ_ਗਿਣਤੀ = 0;	// open up to 3 windows
#  endif

    // Don't redraw until much later.
    ++RedrawingDisabled;

    /*
     * When listing swap file names, don't do cursor positioning et. al.
     */
    ਜੇ (recoverymode && params.fname == ਨਿਰਾ)
	params.want_ਭਰਿਆ_screen = FALSE;

    /*
     * When certain to start the GUI, don't check terminal capabilities.
     * For GTK we can't be sure, but when started from the desktop it doesn't
     * make sense to try using a terminal.
     */
#  ਜੇ defined(ALWAYS_USE_GUI) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	|| defined(VIMDLL)
    ਜੇ (gui.starting
#   ifdef FEAT_GUI_GTK
	    && !isatty(2)
#   endif
	    )
	params.want_ਭਰਿਆ_screen = FALSE;
#  endif

    /*
     * mch_init() sets up the terminal (window) for use.  This must be
     * done after resetting full_screen, otherwise it may move the cursor.
     * Note that we may use mch_exit() before mch_init()!
     */
    mch_init();
    ਸਮਾਂ_ਸੁਨੇਹਾ("shell init");

#  ifdef USE_XSMP
    /*
     * For want of anywhere else to do it, try to connect to xsmp here.
     * Fitting it in after gui_mch_init, but before gui_init (via termcapinit).
     * Hijacking -X 'no X connection' to also disable XSMP connection as that
     * has a similar delay upon failure.
     * Only try if SESSION_MANAGER is set to something non-null.
     */
    ਜੇ (!x_no_connect)
    {
	ਅੱਖਰ *p = getenv("SESSION_MANAGER");

	ਜੇ (p != ਨਿਰਾ && *p != NUL)
	{
	    xsmp_init();
	    ਸਮਾਂ_ਸੁਨੇਹਾ("xsmp init");
	}
    }
#  endif

    /*
     * Print a warning if stdout is not a terminal.
     */
    check_tty(&params);

#  ifdef _IOLBF
    ਜੇ (silent_ਬਹੁਲਕ)
    {
	// Ensure output works usefully without a tty: buffer lines instead of
	// fully buffered.
	s_vbuf = ਸਮ੍ਰਿਤੀ_ਦਿਓ(BUFSIZ);
	ਜੇ (s_vbuf != ਨਿਰਾ)
	    setvbuf(stdout, s_vbuf, _IOLBF, BUFSIZ);
    }
#  endif

    // This message comes before term inits, but after setting "silent_mode"
    // when the input is not a tty. Omit the message with --not-a-term.
    ਜੇ (GARGCOUNT > 1 && !silent_ਬਹੁਲਕ && !is_not_a_ਪਦ())
	ਲਿਖੋ(_("%d files to edit\n"), GARGCOUNT);

    ਜੇ (params.want_ਭਰਿਆ_screen && !silent_ਬਹੁਲਕ)
    {
	termcapinit(params.ਪਦ);	// set terminal name and get terminal
				   // capabilities (will set full_screen)
	screen_ਸ਼ੁਰੂ();		// don't know where cursor is now
	ਸਮਾਂ_ਸੁਨੇਹਾ("Termcap init");
    }

    /*
     * Set the default values for the options that use Rows and Columns.
     */
    ui_get_shellsize();		// inits Rows and Columns
    win_init_ਮਾਪ();
#  ifdef FEAT_DIFF
    // Set the 'diff' option now, so that it can be checked for in a .vimrc
    // file.  There is no buffer yet though.
    ਜੇ (params.diff_ਬਹੁਲਕ)
	diff_win_options(firstwin, FALSE);
#  endif

    cmdline_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - p_ch;
    cmdline_ਖਾਨਾ_off = 0;
    cmdline_ਚੌੜਾਈ = (ਪੂਰਨ)Columns;
    ਸੁਨੇਹਾ_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ;
    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
    screenalloc(FALSE);		// allocate screen buffers
    ਸੈੱਟ_init_2();
    ਸਮਾਂ_ਸੁਨੇਹਾ("inits 2");

    ਸੁਨੇਹਾ_scroll = TRUE;
    no_wait_return = TRUE;

    init_mappings();		// set up initial mappings

    init_highlight(TRUE, FALSE); // set the default highlight groups
    ਸਮਾਂ_ਸੁਨੇਹਾ("init highlight");

#  ਜੇ defined(FEAT_TERMRESPONSE)
    init_ਪਦ_props(TRUE);
#  endif

#  ifdef FEAT_EVAL
    // Set the break level after the terminal is initialized.
    debug_break_level = params.use_debug_break_level;
#  endif

    // Reset 'loadplugins' for "-u NONE" before "--cmd" arguments.
    // Allows for setting 'loadplugins' there.
    ਜੇ (params.use_vimrc != ਨਿਰਾ
	    && (STRCMP(params.use_vimrc, "NONE") == 0
		|| STRCMP(params.use_vimrc, "DEFAULTS") == 0))
	p_lpl = FALSE;

    // Execute --cmd arguments.
    exe_pre_commands(&params);

    // Source startup scripts.
    ਸਰੋਤ_startup_scripts(&params);

#  ifdef FEAT_EVAL
    ਸੈੱਟ_vim_var_nr(VV_VIM_DID_INIT, 1L);
#  endif

#  ifdef FEAT_MZSCHEME
    /*
     * Newer version of MzScheme (Racket) require earlier (trampolined)
     * initialisation via scheme_main_setup.
     * Implement this by initialising it as early as possible
     * and splitting off remaining Vim main into vim_main2().
     * Do source startup scripts, so that 'mzschemedll' can be set.
     */
    ਵਾਪਸ mzscheme_main();
#  ਨਹੀਂਤਾਂ
    ਵਾਪਸ vim_main2();
#  endif
}
# endif // NO_VIM_MAIN
#ਜੇ_ਅੰਤ // PROTO

#ਜੇ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(FEAT_CLIPBOARD)
/*
 * Restore the state after a fatal X error.
 */
    ਸਥਿਰ ਖਾਲੀ
x_restore_state(ਖਾਲੀ)
{
    State = ਬਹੁਲਕ_ਸਾਧਾਰਨ;
    VIsual_ਸਰਗਰਮ = FALSE;
    got_int = TRUE;
    need_wait_return = FALSE;
    global_busy = FALSE;
    exmode_ਸਰਗਰਮ = 0;
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
    scroll_ਸ਼ੁਰੂ();
    redraw_later_clear();
    ਚੁਣੋ_clipmethod();
}
#ਜੇ_ਅੰਤ

/*
 * vim_main2() is needed for FEAT_MZSCHEME, but we define it always to keep
 * things simple.
 * It is also defined when NO_VIM_MAIN is defined, but then it's empty.
 */
    ਪੂਰਨ
vim_main2(ਖਾਲੀ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NO_VIM_MAIN
# ifdef FEAT_EVAL
    /*
     * Read all the plugin files.
     * Only when compiled with +eval, since most plugins need it.
     */
    ਜੇ (p_lpl)
    {
	char_u *rtp_copy = ਨਿਰਾ;
	char_u *plugin_ਨਮੂਨਾ = (char_u *)
#  ਜੇ defined(VMS) || defined(AMIGA) // VMS and Amiga don't handle the "**".
		"plugin/*.vim"
#  ਨਹੀਂਤਾਂ
		"plugin/**/*.vim"
#  endif
		;

	// First add all package directories to 'runtimepath', so that their
	// autoload directories can be found.  Only if not done already with a
	// :packloadall command.
	// Make a copy of 'runtimepath', so that source_runtime does not use
	// the pack directories.
	ਜੇ (!did_ਸਰੋਤ_packages)
	{
	    rtp_copy = vim_strsave(p_rtp);
	    ਜੋੜੋ_pack_ਸ਼ੁਰੂ_dirs();
	}

	ਸਰੋਤ_ਵਿੱਚ_ਰਸਤਾ(rtp_copy == ਨਿਰਾ ? p_rtp : rtp_copy, plugin_ਨਮੂਨਾ,
		DIP_ALL | DIP_NOAFTER, ਨਿਰਾ);
	ਸਮਾਂ_ਸੁਨੇਹਾ("loading plugins");
	vim_free(rtp_copy);

	// Only source "start" packages if not done already with a :packloadall
	// command.
	ਜੇ (!did_ਸਰੋਤ_packages)
	    ਲੱਦੋ_ਸ਼ੁਰੂ_packages();
	ਸਮਾਂ_ਸੁਨੇਹਾ("loading packages");

	ਸਰੋਤ_runtime(plugin_ਨਮੂਨਾ, DIP_ALL | DIP_AFTER);
	ਸਮਾਂ_ਸੁਨੇਹਾ("loading after plugins");
    }
# endif

# ifdef FEAT_DIFF
    // Decide about window layout for diff mode after reading vimrc.
    ਜੇ (params.diff_ਬਹੁਲਕ && params.ਖਿੜਕੀ_layout == 0)
    {
	ਜੇ (diffopt_horizontal())
	    params.ਖਿੜਕੀ_layout = WIN_HOR;	// use horizontal split
	ਨਹੀਂਤਾਂ
	    params.ਖਿੜਕੀ_layout = WIN_VER;	// use vertical split
    }
# endif

    /*
     * Recovery mode without a file name: List swap files.
     * This uses the 'dir' option, therefore it must be after the
     * initializations.
     */
    ਜੇ (recoverymode && params.fname == ਨਿਰਾ)
    {
	recover_ਨਾਂ(ਨਿਰਾ, TRUE, ਨਿਰਾ, 0, ਨਿਰਾ);
	mch_exit(0);
    }

    /*
     * Set a few option defaults after reading .vimrc files:
     * 'title' and 'icon', Unix: 'shellpipe' and 'shellredir'.
     */
    ਸੈੱਟ_init_3();
    ਸਮਾਂ_ਸੁਨੇਹਾ("inits 3");

    /*
     * "-n" argument: Disable swap file by setting 'updatecount' to 0.
     * Note that this overrides anything from a vimrc file.
     */
    ਜੇ (params.no_ਅਦਲਾ_ਬਦਲੀ_file)
	p_uc = 0;

# ifdef FEAT_GUI
    ਜੇ (gui.starting)
    {
#  ਜੇ defined(UNIX) || defined(VMS)
	// When something caused a message from a vimrc script, need to output
	// an extra newline before the shell prompt.
	ਜੇ (did_emsg || ਸੁਨੇਹਾ_didout)
	    ਅੱਖਰ_ਲਿਖੋ('\n');
#  endif

	gui_ਸ਼ੁਰੂ(ਨਿਰਾ);		// will set full_screen to TRUE
	ਸਮਾਂ_ਸੁਨੇਹਾ("starting GUI");

	// When running "evim" or "gvim -y" we need the menus, exit if we
	// don't have them.
	ਜੇ (!gui.ਵਿੱਚ_use && params.evim_ਬਹੁਲਕ)
	    mch_exit(1);
	firstwin->w_prev_ਉਚਾਈ = firstwin->w_ਉਚਾਈ; // may have changed
    }
# endif

# ifdef FEAT_VIMINFO
    /*
     * Read in registers, history etc, but not marks, from the viminfo file.
     * This is where v:oldfiles gets filled.
     */
    ਜੇ (*p_viminfo != NUL)
    {
	read_viminfo(ਨਿਰਾ, VIF_WANT_INFO | VIF_GET_OLDFILES);
	ਸਮਾਂ_ਸੁਨੇਹਾ("reading viminfo");
    }
# endif
# ifdef FEAT_EVAL
    // It's better to make v:oldfiles an empty list than NULL.
    ਜੇ (get_vim_var_list(VV_OLDFILES) == ਨਿਰਾ)
	ਸੈੱਟ_vim_var_list(VV_OLDFILES, list_alloc());
# endif

# ifdef FEAT_QUICKFIX
    /*
     * "-q errorfile": Load the error file now.
     * If the error file can't be read, exit before doing anything else.
     */
    ਜੇ (params.edit_type == EDIT_QF)
    {
	char_u	*enc = ਨਿਰਾ;

	enc = p_menc;
	ਜੇ (params.use_ef != ਨਿਰਾ)
	    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"ef", -1,
					   params.use_ef, OPT_FREE, SID_CARG);
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "cfile %s", p_ef);
	ਜੇ (qf_init(ਨਿਰਾ, p_ef, p_efm, TRUE, IObuff, enc) < 0)
	{
	    ਬਾਹਰਲਾ_char('\n');
	    mch_exit(3);
	}
	ਸਮਾਂ_ਸੁਨੇਹਾ("reading errorfile");
    }
# endif

    /*
     * Start putting things on the screen.
     * Scroll screen down before drawing over it
     * Clear screen now, so file message will not be cleared.
     */
    starting = NO_BUFFERS;
    no_wait_return = FALSE;
    ਜੇ (!exmode_ਸਰਗਰਮ)
	ਸੁਨੇਹਾ_scroll = FALSE;

# ifdef FEAT_GUI
    /*
     * This seems to be required to make callbacks to be called now, instead
     * of after things have been put on the screen, which then may be deleted
     * when getting a resize callback.
     * For the Mac this handles putting files dropped on the Vim icon to
     * global_alist.
     */
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	gui_wait_for_chars(50L, typebuf.tb_change_cnt);
	ਸਮਾਂ_ਸੁਨੇਹਾ("GUI delay");
    }
# endif

# ਜੇ defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)
    qnx_clip_init();
# endif

# ਜੇ defined(MACOS_X) && defined(FEAT_CLIPBOARD)
    clip_init(TRUE);
# endif

# ifdef FEAT_XCLIPBOARD
    // Start using the X clipboard, unless the GUI was started.
#  ifdef FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use)
#  endif
    {
	ਤਿਆਰੀ_ਪਦ_clip();
	ਸਮਾਂ_ਸੁਨੇਹਾ("setup x11 clipboard");
    }
# endif

# ifdef FEAT_CLIENTSERVER
    // Prepare for being a Vim server.
    prepare_server(&params);
# endif

# ifdef FEAT_WAYLAND
#  ifdef FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use)
#  endif
    {
	ਜੇ (wayland_init_connection(wayland_display_name) == OK)
	{
	    ਸਮਾਂ_ਸੁਨੇਹਾ("connected to Wayland display");

#  ifdef FEAT_WAYLAND_CLIPBOARD
	    ਜੇ (clip_init_wayland() == OK)
		ਸਮਾਂ_ਸੁਨੇਹਾ("setup Wayland clipboard");
#  endif
	}
    }
# endif

# ifdef HAVE_CLIPMETHOD
    ਚੁਣੋ_clipmethod();
# endif

    /*
     * If "-" argument given: Read file from stdin.
     * Do this before starting Raw mode, because it may change things that the
     * writing end of the pipe doesn't like, e.g., in case stdin and stderr
     * are the same terminal: "cat | vim -".
     * Using autocommands here may cause trouble...
     */
    ਜੇ (params.edit_type == EDIT_STDIN && !recoverymode)
	read_stdin();

# ਜੇ defined(UNIX) || defined(VMS)
    // When switching screens and something caused a message from a vimrc
    // script, need to output an extra newline on exit.
    ਜੇ ((did_emsg || ਸੁਨੇਹਾ_didout) && *T_TI != NUL && params.edit_type != EDIT_STDIN)
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
    ਸਮਾਂ_ਸੁਨੇਹਾ("setting raw mode");

    ਜੇ (need_wait_return || ਸੁਨੇਹਾ_didany)
    {
	wait_return(TRUE);
	ਸਮਾਂ_ਸੁਨੇਹਾ("waiting for return");
    }

    starttermcap();	    // start termcap if not done by wait_return()
    ਸਮਾਂ_ਸੁਨੇਹਾ("start termcap");

    setmouse();				// may start using the mouse
    ਜੇ (scroll_region)
	scroll_region_reset();		// In case Rows changed
    scroll_ਸ਼ੁਰੂ();	// may scroll the screen to the right position

# ਜੇ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    ਪਦ_ਧੱਕੋ_title(SAVE_RESTORE_BOTH);
# endif

    /*
     * Don't clear the screen when starting in Ex mode, unless using the GUI.
     */
    ਜੇ (exmode_ਸਰਗਰਮ
# ifdef FEAT_GUI
			&& !gui.ਵਿੱਚ_use
# endif
					)
	ਸੈੱਟ_must_redraw(UPD_CLEAR);
    ਨਹੀਂਤਾਂ
    {
	screenclear();			// clear screen
	ਸਮਾਂ_ਸੁਨੇਹਾ("clearing screen");
    }

# ifdef FEAT_CRYPT
    ਜੇ (params.ask_for_ਕੁੰਜੀ)
    {
	crypt_check_current_ਵਿਧੀ();
	(ਖਾਲੀ)crypt_get_ਕੁੰਜੀ(TRUE, TRUE);
	ਸਮਾਂ_ਸੁਨੇਹਾ("getting crypt key");
    }
# endif

    no_wait_return = TRUE;

    /*
     * Create the requested number of windows and edit buffers in them.
     * Also does recovery if "recoverymode" set.
     */
    create_windows(&params);
    ਸਮਾਂ_ਸੁਨੇਹਾ("opening buffers");

# ifdef FEAT_EVAL
    // clear v:swapcommand
    ਸੈੱਟ_vim_var_string(VV_SWAPCOMMAND, ਨਿਰਾ, -1);
# endif

    // Ex starts at last line of the file
    ਜੇ (exmode_ਸਰਗਰਮ)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    apply_autocmds(EVENT_BUFENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    ਸਮਾਂ_ਸੁਨੇਹਾ("BufEnter autocommands");
    setpcmark();

# ifdef FEAT_QUICKFIX
    /*
     * When started with "-q errorfile" jump to first error now.
     */
    ਜੇ (params.edit_type == EDIT_QF)
    {
	qf_jump(ਨਿਰਾ, 0, 0, FALSE);
	ਸਮਾਂ_ਸੁਨੇਹਾ("jump to first error");
    }
# endif

    /*
     * If opened more than one window, start editing files in the other
     * windows.
     */
    edit_buffers(&params, ਸ਼ੁਰੂ_dir);
    vim_free(ਸ਼ੁਰੂ_dir);

# ifdef FEAT_DIFF
    ਜੇ (params.diff_ਬਹੁਲਕ)
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
    ਜੇ (params.tagname != ਨਿਰਾ)
    {
	ਅਦਲਾ_ਬਦਲੀ_exists_did_quit = FALSE;

	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "ta %s", params.tagname);
	do_cmdline_cmd(IObuff);
	ਸਮਾਂ_ਸੁਨੇਹਾ("jumping to tag");

	// If the user doesn't want to edit the file then we quit here.
	ਜੇ (ਅਦਲਾ_ਬਦਲੀ_exists_did_quit)
	    getout(1);
    }

# ਜੇ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    // Temporarily set x_jump_env to here in case there is an X11 IO error,
    // because x_jump_env is only actually set in main_loop(), before
    // exe_commands(). May not be the best solution since commands passed via
    // the command line can be very broad like sourcing a file, in which case
    // an X IO error results in the command being partially done. In theory we
    // could use SETJMP in RealWaitForChar(), but the stack frame for that may
    // possibly exit and then LONGJMP is called on it.
    ਪੂਰਨ jump_ਨਤੀਜਾ = SETJMP(x_jump_env);

    ਜੇ (jump_ਨਤੀਜਾ == 0)
    {
# endif
	// Execute any "+", "-c" and "-S" arguments.
	ਜੇ (params.n_commands > 0)
	    exe_commands(&params);
# ਜੇ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    }
    ਨਹੀਂਤਾਂ
	// Restore state and continue just like what main_loop() does.
	x_restore_state();
# endif

    // Must come before the may_req_ calls.
    starting = 0;

# ਜੇ defined(FEAT_TERMRESPONSE)
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
# ਜੇ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_ਮਾਪ();
# endif

    // start in insert mode
    ਜੇ (p_im)
	need_ਸ਼ੁਰੂ_insertmode = TRUE;

# ifdef FEAT_EVAL
    ਸੈੱਟ_vim_var_nr(VV_VIM_DID_ENTER, 1L);
# endif
    apply_autocmds(EVENT_VIMENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    ਸਮਾਂ_ਸੁਨੇਹਾ("VimEnter autocommands");

# ਜੇ defined(FEAT_EVAL) && defined(FEAT_CLIPBOARD)
    // Adjust default register name for "unnamed" in 'clipboard'. Can only be
    // done after the clipboard is available and all initial commands that may
    // modify the 'clipboard' setting have run; i.e. just before entering the
    // main loop.
    reset_reg_var();
# endif

# ਜੇ defined(FEAT_DIFF)
    // When a startup script or session file setup for diff'ing and
    // scrollbind, sync the scrollbind now.
    ਜੇ (curwin->w_p_diff && curwin->w_p_scb)
    {
	update_topline();
	check_scrollbind((linenr_T)0, 0L);
	ਸਮਾਂ_ਸੁਨੇਹਾ("diff scrollbinding");
    }
# endif

# ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ਜੇ (!gui.ਵਿੱਚ_use)
#  endif
	mch_ਸੈੱਟ_winsize_now();	    // Allow winsize changes from now on
# endif

# ਜੇ defined(FEAT_GUI)
    // When tab pages were created, may need to update the tab pages line and
    // scrollbars.  This is skipped while creating them.
    ਜੇ (gui.ਵਿੱਚ_use && ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ != ਨਿਰਾ)
    {
	ਬਾਹਰਲਾ_flush();
	gui_init_which_components(ਨਿਰਾ);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
# endif

    // If ":startinsert" command used, stuff a dummy command to be able to
    // call normal_cmd(), which will then start Insert mode.
    ਜੇ (restart_edit != 0)
	stuffcharReadbuff(K_NOP);

# ifdef FEAT_NETBEANS_INTG
    ਜੇ (netbeansArg != ਨਿਰਾ && ਲੜੀ_ਤੁਲਨਾ("-nb", netbeansArg, 3) == 0)
    {
#  ifdef FEAT_GUI
#   ਜੇ !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
	ਜੇ (gui.ਵਿੱਚ_use)
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

    ਸਮਾਂ_ਸੁਨੇਹਾ("before starting main loop");

# ifdef FEAT_ਕੰਮ_CHANNEL
    ਜੇ (use_stdio_channel)
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

#ਜੇ_ਅੰਤ // NO_VIM_MAIN

    ਵਾਪਸ 0;
}

/*
 * Initialisation #1 shared by main() and some tests.
 */
    ਖਾਲੀ
common_init_1(ਖਾਲੀ)
{
    estack_init();
    cmdline_init();

    (ਖਾਲੀ)mb_init();	// init mb_bytelen_tab[] to ones
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    eval_init();	// init global variables
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __QNXNTO__
    qnx_init();		// PhAttach() for clipboard, (and gui)
#ਜੇ_ਅੰਤ

    /*
     * Allocate space for the generic buffers (needed for set_init_1() and
     * emsg()).
     */
    ਜੇ ((IObuff = alloc(IOSIZE)) == ਨਿਰਾ
	    || (NameBuff = alloc(MAXPATHL)) == ਨਿਰਾ)
	mch_exit(0);
    ਸਮਾਂ_ਸੁਨੇਹਾ("Allocated generic buffers");
}


/*
 * Initialisation #2 shared by main() and some tests.
 */
    ਖਾਲੀ
common_init_2(mparm_T *paramp)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NBDEBUG
    // Wait a moment for debugging NetBeans.  Must be after allocating
    // NameBuff.
    nbdebug_log_init("SPRO_GVIM_DEBUG", "SPRO_GVIM_DLEVEL");
    nbdebug_wait(WT_ENV | WT_WAIT | WT_STOP, "SPRO_GVIM_WAIT", 20);
    ਸਮਾਂ_ਸੁਨੇਹਾ("NetBeans debug wait");
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    /*
     * Setup to use the current locale (for ctype() and many other things).
     * NOTE: Translated messages with encodings other than latin1 will not
     * work until set_init_1() has been called!
     */
    init_locale();
    ਸਮਾਂ_ਸੁਨੇਹਾ("locale set");
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    gui.dofork = true;		    // default is to use fork()
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER_BACKENDS
    /*
     * Check the $VIM_CLIENTSERVER env before we handle the --clientserver arg
     */
    check_clientserver_ਵਿਧੀ_env();
#ਜੇ_ਅੰਤ

    /*
     * Do a first scan of the arguments in "argv[]":
     *   -display or --display
     *   --server...
     *   --socketid
     *   --windowid
     */
    early_arg_ਜਾਂਚ(paramp);

#ਜੇ defined(FEAT_GUI)
    // Prepare for possibly starting GUI sometime
    gui_prepare(&paramp->argc, paramp->argv);
    ਸਮਾਂ_ਸੁਨੇਹਾ("GUI prepared");
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    clip_init(FALSE);		// Initialise clipboard stuff
    ਸਮਾਂ_ਸੁਨੇਹਾ("clipboard setup");
#ਜੇ_ਅੰਤ

    /*
     * Check if we have an interactive window.
     * On the Amiga: If there is no window, we open one with a newcli command
     * (needed for :! to * work). mch_check_win() will also handle the -d or
     * -dev argument.
     */
    stdout_isatty = (mch_check_win(paramp->argc, paramp->argv) != FAIL);
    ਸਮਾਂ_ਸੁਨੇਹਾ("window checked");

    /*
     * Allocate the first window and buffer.
     * Can't do anything without it, exit when it fails.
     */
    ਜੇ (win_alloc_ਪਹਿਲਾ() == FAIL)
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
    ਸੈੱਟ_init_1(paramp->clean);
    ਸਮਾਂ_ਸੁਨੇਹਾ("inits 1");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // set v:lang and v:ctype
    ਸੈੱਟ_lang_var();

    // set v:argv
    ਸੈੱਟ_argv_var(paramp->argv, paramp->argc);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
    init_signs();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    // initialize quickfix list
    // don't send an error message when memory allocation fails
    // do it when the user tries to access the quickfix list
    qf_init_ਢੇਰ();
#ਜੇ_ਅੰਤ
}

/*
 * Return TRUE when the --not-a-term argument was found.
 */
    ਪੂਰਨ
is_not_a_ਪਦ(ਖਾਲੀ)
{
    ਵਾਪਸ params.not_a_ਪਦ;
}

/*
 * Return TRUE when the --not-a-term argument was found or the GUI is in use.
 */
    ਪੂਰਨ
is_not_a_ਪਦ_or_gui(ਖਾਲੀ)
{
    ਵਾਪਸ params.not_a_ਪਦ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
			    || gui.ਵਿੱਚ_use
#ਜੇ_ਅੰਤ
	;
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_vbuf(ਖਾਲੀ)
{
# ifdef _IOLBF
    ਜੇ (s_vbuf != ਨਿਰਾ)
    {
	setvbuf(stdout, ਨਿਰਾ, _IONBF, 0);
	ਮੁਕਤ(s_vbuf);
	s_vbuf = ਨਿਰਾ;
    }
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI)
/*
 * If a --gui-dialog-file argument was given return the file name.
 * Otherwise return NULL.
 */
    char_u *
get_gui_dialog_file(ਖਾਲੀ)
{
    ਵਾਪਸ params.gui_dialog_file;
}
#ਜੇ_ਅੰਤ

// When TRUE in a safe state when starting to wait for a character.
ਸਥਿਰ ਪੂਰਨ	was_ਸੁਰੱਖਿਅਤ = FALSE;
ਸਥਿਰ oparg_T	*current_oap = ਨਿਰਾ;

/*
 * Return TRUE if an operator was started but not finished yet.
 * Includes typing a count or a register name.
 */
    ਪੂਰਨ
op_pending(ਖਾਲੀ)
{
    ਵਾਪਸ !(current_oap != ਨਿਰਾ
	    && !ਅੰਤ_op
	    && current_oap->prev_opcount == 0
	    && current_oap->prev_count0 == 0
	    && current_oap->op_type == OP_NOP
	    && current_oap->regname == NUL);
}

/*
 * Return whether currently it is safe, assuming it was safe before (high level
 * state didn't change).
 */
    ਸਥਿਰ ਪੂਰਨ
is_ਸੁਰੱਖਿਅਤ_now(ਖਾਲੀ)
{
    ਵਾਪਸ stuff_ਖਾਲੀ_ਥਾਵਾਂ()
	&& typebuf.tb_len == 0
	&& scriptin[curscript] == ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	&& !debug_ਬਹੁਲਕ
#ਜੇ_ਅੰਤ
	&& !global_busy;
}

/*
 * Trigger SafeState if currently in a safe state, that is "safe" is TRUE and
 * there is no typeahead.
 */
    ਖਾਲੀ
may_trigger_safestate(ਪੂਰਨ ਸੁਰੱਖਿਅਤ_2)
{
    ਪੂਰਨ is_ਸੁਰੱਖਿਅਤ = ਸੁਰੱਖਿਅਤ_2 && is_ਸੁਰੱਖਿਅਤ_now();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (was_ਸੁਰੱਖਿਅਤ != is_ਸੁਰੱਖਿਅਤ)
	// Only log when the state changes, otherwise it happens at nearly
	// every key stroke.
	ch_log(ਨਿਰਾ, is_ਸੁਰੱਖਿਅਤ ? "SafeState: Start triggering"
					       : "SafeState: Stop triggering");
#ਜੇ_ਅੰਤ
    ਜੇ (is_ਸੁਰੱਖਿਅਤ)
	apply_autocmds(EVENT_SAFESTATE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    was_ਸੁਰੱਖਿਅਤ = is_ਸੁਰੱਖਿਅਤ;
}

/*
 * Something changed which causes the state possibly to be unsafe, e.g. a
 * character was typed.  It will remain unsafe until the next call to
 * may_trigger_safestate().
 */
    ਖਾਲੀ
state_no_longer_ਸੁਰੱਖਿਅਤ(ਅੱਖਰ *reason ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (was_ਸੁਰੱਖਿਅਤ)
	ch_log(ਨਿਰਾ, "SafeState: reset: %s", reason);
#ਜੇ_ਅੰਤ
    was_ਸੁਰੱਖਿਅਤ = FALSE;
}

#ਜੇ defined(FEAT_EVAL) || defined(ਸੁਨੇਹਾ_ਕਤਾਰ)
    ਪੂਰਨ
get_was_ਸੁਰੱਖਿਅਤ_state(ਖਾਲੀ)
{
    ਵਾਪਸ was_ਸੁਰੱਖਿਅਤ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(ਸੁਨੇਹਾ_ਕਤਾਰ)
/*
 * Invoked when leaving code that invokes callbacks.  Then trigger
 * SafeStateAgain, if it was safe when starting to wait for a character.
 */
    ਖਾਲੀ
may_trigger_safestateagain(ਖਾਲੀ)
{
    ਜੇ (!was_ਸੁਰੱਖਿਅਤ)
    {
	// If the safe state was reset in state_no_longer_safe(), e.g. because
	// of calling feedkeys(), we check if it's now safe again (all keys
	// were consumed).
	was_ਸੁਰੱਖਿਅਤ = is_ਸੁਰੱਖਿਅਤ_now();
# ifdef FEAT_EVAL
	ਜੇ (was_ਸੁਰੱਖਿਅਤ)
	    ch_log(ਨਿਰਾ, "SafeState: undo reset");
# endif
    }
    ਜੇ (was_ਸੁਰੱਖਿਅਤ)
    {
# ifdef FEAT_EVAL
	// Only do this message when another message was given, otherwise we
	// get lots of them.
	ਜੇ ((did_repeated_ਸੁਨੇਹਾ & REPEATED_ਸੁਨੇਹਾ_SAFESTATE) == 0)
	{
	    ਪੂਰਨ did = did_repeated_ਸੁਨੇਹਾ;

	    ch_log(ਨਿਰਾ,
		      "SafeState: back to waiting, triggering SafeStateAgain");
	    did_repeated_ਸੁਨੇਹਾ = did | REPEATED_ਸੁਨੇਹਾ_SAFESTATE;
	}
# endif
	apply_autocmds(EVENT_SAFESTATEAGAIN, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    }
# ifdef FEAT_EVAL
    ਨਹੀਂਤਾਂ
	ch_log(ਨਿਰਾ,
		  "SafeState: back to waiting, not triggering SafeStateAgain");
# endif
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if there is any typeahead, pending operator or command.
 */
    ਪੂਰਨ
work_pending(ਖਾਲੀ)
{
    ਵਾਪਸ op_pending() || !is_ਸੁਰੱਖਿਅਤ_now();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
ਸਥਿਰ linenr_T	conceal_old_cursor_ਸਤਰ = 0;
ਸਥਿਰ linenr_T	conceal_new_cursor_ਸਤਰ = 0;
ਸਥਿਰ ਪੂਰਨ	conceal_update_ਸਤਰਾਂ = FALSE;
#ਜੇ_ਅੰਤ

/*
 * Trigger the events that are not triggered where they happen, but by
 * comparing the current state against the state stored when they were last
 * triggered. Also updates what depends on the cursor having moved, which is
 * not affected by 'eventignore'.
 * Called from the main loop and when 'eventignore(win)' changes, so that what
 * happened with an event ignored is not reported once it is not ignored
 * anymore.
 */
    ਖਾਲੀ
may_trigger_deferred_events(ਖਾਲੀ)
{
    ਸਥਿਰ bool	recursive = false;

    ਜੇ (recursive)
	ਵਾਪਸ;
    recursive = true;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਜੇ (curwin->w_p_cole == 0)
	conceal_update_ਸਤਰਾਂ = FALSE;
#ਜੇ_ਅੰਤ

    // Trigger CursorMoved if the cursor moved.
    ਜੇ (!ਅੰਤ_op && (has_cursormoved()
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		|| popup_visible
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
		|| curwin->w_p_cole > 0
#ਜੇ_ਅੰਤ
		) && !EQUAL_POS(ਆਖਰੀ_cursormoved, curwin->w_cursor))
    {
	ਜੇ (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (popup_visible)
	    popup_check_cursor_pos();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	ਜੇ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_ਸਤਰ = ਆਖਰੀ_cursormoved.lnum;
	    conceal_new_cursor_ਸਤਰ = curwin->w_cursor.lnum;
	    conceal_update_ਸਤਰਾਂ = TRUE;
	}
#ਜੇ_ਅੰਤ
	ਆਖਰੀ_cursormoved = curwin->w_cursor;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਜੇ (conceal_update_ਸਤਰਾਂ
	    && (conceal_old_cursor_ਸਤਰ != conceal_new_cursor_ਸਤਰ
		|| conceal_cursor_ਸਤਰ(curwin)
		|| need_cursor_ਸਤਰ_redraw))
    {
	ਜੇ (conceal_old_cursor_ਸਤਰ != conceal_new_cursor_ਸਤਰ
		&& conceal_old_cursor_ਸਤਰ != 0
		&& conceal_old_cursor_ਸਤਰ <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    redrawWinline(curwin, conceal_old_cursor_ਸਤਰ);
	redrawWinline(curwin, conceal_new_cursor_ਸਤਰ);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_ਸਤਰ_redraw = FALSE;
    }
#ਜੇ_ਅੰਤ

    // Trigger TextChanged if b:changedtick differs.
    ਜੇ (!ਅੰਤ_op && has_textchanged()
	    && curbuf->b_ਆਖਰੀ_changedtick != CHANGEDTICK(curbuf))
    {
	apply_autocmds(EVENT_TEXTCHANGED, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	curbuf->b_ਆਖਰੀ_changedtick = CHANGEDTICK(curbuf);
    }

    // Ensure curwin->w_topline and curwin->w_leftcol are up to date before
    // triggering a WinScrolled autocommand.
    update_topline();
    validate_cursor();

    ਜੇ (!ਅੰਤ_op)
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
    ਖਾਲੀ
main_loop(
    ਪੂਰਨ		cmdwin,	    // TRUE when working in the command-line window
    ਪੂਰਨ		noexmode)   // TRUE when return on entering Ex mode
{
    oparg_T	oa;		// operator arguments
    oparg_T	*prev_oap;	// operator arguments
    ਅਸਥਿਰ ਪੂਰਨ ਪਿਛਲਾ_got_int = FALSE;	// "got_int" was TRUE

    prev_oap = current_oap;
    current_oap = &oa;

#ਜੇ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    // Setup to catch a terminating error from the X server.  Just ignore
    // it, restore the state and continue.  This might not always work
    // properly, but at least we hopefully don't exit unexpectedly when the X
    // server exits while Vim is running in a console.
    ਜੇ (!cmdwin && !noexmode && SETJMP(x_jump_env))
	x_restore_state();
#ਜੇ_ਅੰਤ

    clear_oparg(&oa);
    ਜਦੋਂ (!cmdwin || cmdwin_ਨਤੀਜਾ == 0)
    {
	ਜੇ (stuff_ਖਾਲੀ_ਥਾਵਾਂ())
	{
	    did_check_timestamps = FALSE;
	    ਜੇ (need_check_timestamps)
		check_timestamps(FALSE);
	    ਜੇ (need_wait_return)	// if wait_return() still needed ...
		wait_return(FALSE);	// ... call it now
	    ਜੇ (need_ਸ਼ੁਰੂ_insertmode && goto_im() && !VIsual_ਸਰਗਰਮ)
	    {
		need_ਸ਼ੁਰੂ_insertmode = FALSE;
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
	ਜੇ (got_int)
	{
	    ਜੇ (noexmode && global_busy && !exmode_ਸਰਗਰਮ && ਪਿਛਲਾ_got_int)
	    {
		// Typed two CTRL-C in a row: go back to ex mode as if "Q" was
		// used and keep "got_int" set, so that it aborts ":g".
		exmode_ਸਰਗਰਮ = EXMODE_ਸਾਧਾਰਨ;
		State = ਬਹੁਲਕ_ਸਾਧਾਰਨ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (!global_busy || !exmode_ਸਰਗਰਮ)
	    {
		ਜੇ (!quit_ਹੋਰ)
		    (ਖਾਲੀ)vgetc();		// flush all buffers
		got_int = FALSE;
	    }
	    ਪਿਛਲਾ_got_int = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    ਪਿਛਲਾ_got_int = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// At the toplevel there is no exception handling.  Discard any that
	// may be hanging around (e.g. from "interrupt" at the debug prompt).
	ਜੇ (did_throw && !ex_ਸਾਧਾਰਨ_busy)
	    discard_current_exception();
#ਜੇ_ਅੰਤ

	ਜੇ (!exmode_ਸਰਗਰਮ)
	    ਸੁਨੇਹਾ_scroll = FALSE;
	quit_ਹੋਰ = FALSE;

	// it's not safe unless may_trigger_safestate_main() is called
	was_ਸੁਰੱਖਿਅਤ = FALSE;

	/*
	 * If skip redraw is set (for ":" in wait_return()), don't redraw now.
	 * If there is nothing in the stuff_buffer or do_redraw is TRUE,
	 * update cursor and redraw.
	 */
	ਜੇ (skip_redraw || exmode_ਸਰਗਰਮ)
	{
	    skip_redraw = FALSE;
	    setcursor();
	    cursor_on();
	}
	ਨਹੀਂਤਾਂ ਜੇ (do_redraw || stuff_ਖਾਲੀ_ਥਾਵਾਂ())
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    // If ui_breakcheck() was used a resize may have been postponed.
	    gui_may_resize_shell();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_DROP_FILE
	    // If files were dropped while text was locked or the curbuf was
	    // locked, this would be a good time to handle the drop.
	    handle_any_postponed_drop();
#ਜੇ_ਅੰਤ
	    may_trigger_deferred_events();

	    // If nothing is pending and we are going to wait for the user to
	    // type a character, trigger SafeState.
	    may_trigger_safestate(!op_pending() && restart_edit == 0);

#ਜੇ defined(FEAT_DIFF)
	    // Updating diffs from changed() does not always work properly,
	    // esp. updating folds.  Do an update just before redrawing if
	    // needed.
	    ਜੇ (curtab->tp_diff_update || curtab->tp_diff_invalid)
	    {
		ex_diffupdate(ਨਿਰਾ);
		curtab->tp_diff_update = FALSE;
	    }

	    // Scroll-binding for diff mode may have been postponed until
	    // here.  Avoids doing it for every change.
	    ਜੇ (diff_need_scrollbind)
	    {
		check_scrollbind((linenr_T)0, 0L);
		diff_need_scrollbind = FALSE;
	    }
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_FOLDING)
	    // Include a closed fold completely in the Visual area.
	    foldAdjustVisual();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    /*
	     * When 'foldclose' is set, apply 'foldlevel' to folds that don't
	     * contain the cursor.
	     * When 'foldopen' is "all", open the fold(s) under the cursor.
	     * This may mark the window for redrawing.
	     */
	    ਜੇ (hasAnyFolding(curwin) && !char_avail())
	    {
		foldCheckClose();
		ਜੇ (fdo_flags & FDO_ALL)
		    foldOpenCursor();
	    }
#ਜੇ_ਅੰਤ

	    // Before redrawing, make sure w_topline is correct, and w_leftcol
	    // if lines don't wrap, and w_skipcol if lines wrap.
	    update_topline();
	    validate_cursor();

	    ਜੇ (VIsual_ਸਰਗਰਮ)
		update_curbuf(UPD_INVERTED); // update inverted part
	    ਨਹੀਂਤਾਂ ਜੇ (must_redraw)
	    {
		mch_disable_flush();	// Stop issuing gui_mch_flush().
		update_screen(0);
		mch_enable_flush();
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (redraw_cmdline || clear_cmdline || redraw_ਬਹੁਲਕ)
		showmode();
	    redraw_statuslines();
	    ਜੇ (need_maketitle)
		maketitle();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
	    curbuf->b_ਆਖਰੀ_ਵਰਤਿਆ = vim_ਸਮਾਂ();
#ਜੇ_ਅੰਤ
	    // display message after redraw
	    ਜੇ (keep_ਸੁਨੇਹਾ != ਨਿਰਾ)
	    {
		char_u *p = vim_strsave(keep_ਸੁਨੇਹਾ);

		ਜੇ (p != ਨਿਰਾ)
		{
		    // msg_start() will set keep_msg to NULL, make a copy
		    // first.  Don't reset keep_msg, msg_attr_keep() uses it to
		    // check for duplicates.  Never put this message in
		    // history.
		    ਸੁਨੇਹਾ_hist_off = TRUE;
		    ਸੁਨੇਹਾ_attr((ਅੱਖਰ *)p, keep_ਸੁਨੇਹਾ_attr);
		    ਸੁਨੇਹਾ_hist_off = FALSE;
		    vim_free(p);
		}
	    }
	    ਜੇ (need_fileinfo)		// show file info after redraw
	    {
		fileinfo(FALSE, TRUE, FALSE);
		need_fileinfo = FALSE;
	    }

	    emsg_on_display = FALSE;	// can delete error message now
	    did_emsg = FALSE;
	    ਸੁਨੇਹਾ_didany = FALSE;		// reset lines_left in msg_start()
	    may_clear_sb_ਪਾਠ();	// clear scroll-back text on next msg
	    showruler(FALSE);

	    setcursor();
	    cursor_on();

	    do_redraw = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ STARTUPTIME
	    // Now that we have drawn the first screen all the startup stuff
	    // has been done, close any file for startup messages.
	    ਜੇ (ਸਮਾਂ_fd != ਨਿਰਾ)
	    {
		ਸਮਾਂ_ਸੁਨੇਹਾ("first screen update");
		ਸਮਾਂ_ਸੁਨੇਹਾ("--- VIM STARTED ---");
		fclose(ਸਮਾਂ_fd);
		ਸਮਾਂ_fd = ਨਿਰਾ;
	    }
#ਜੇ_ਅੰਤ
	    // After the first screen update may start triggering WinScrolled
	    // autocmd events.  Store all the scroll positions and sizes now.
	    may_make_initial_scroll_ਮਾਪ_snapshot();
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (need_mouse_correct)
	    gui_mouse_correct();
#ਜੇ_ਅੰਤ

	// May request the keyboard protocol state now.
	may_send_t_RK();

	// Update w_curswant if w_set_curswant has been set.
	// Postponed until here to avoid computing w_virtcol too often.
	update_curswant();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	/*
	 * May perform garbage collection when waiting for a character, but
	 * only at the very toplevel.  Otherwise we may be using a List or
	 * Dict internally somewhere.
	 * "may_garbage_collect" is reset in vgetc() which is invoked through
	 * do_exmode() and normal_cmd().
	 */
	may_garbage_collect = (!cmdwin && !noexmode);
#ਜੇ_ਅੰਤ
	/*
	 * If we're invoked as ex, do a round of ex commands.
	 * Otherwise, get and execute a normal mode command.
	 */
	ਜੇ (exmode_ਸਰਗਰਮ)
	{
	    ਜੇ (noexmode)   // End of ":global/path/visual" commands
		ਜਾਓ theend;
	    do_exmode(exmode_ਸਰਗਰਮ == EXMODE_VIM);
	}
	ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    ਜੇ (ਪਦ_use_loop()
		    && oa.op_type == OP_NOP && oa.regname == NUL
		    && !VIsual_ਸਰਗਰਮ
		    && !skip_ਪਦ_loop)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal mode.  With FAIL we first need to position the
		// cursor and the screen needs to be redrawn.
		ਜੇ (terminal_loop(TRUE) == OK)
		    ਸਾਧਾਰਨ_cmd(&oa, TRUE);
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
		skip_ਪਦ_loop = FALSE;
#ਜੇ_ਅੰਤ
		ਸਾਧਾਰਨ_cmd(&oa, TRUE);
	    }
	}
    }

theend:
    current_oap = prev_oap;
}


#ਜੇ defined(USE_XSMP) || defined(FEAT_GUI)
/*
 * Exit, but leave behind swap files for modified buffers.
 */
    ਖਾਲੀ
getout_preserve_modified(ਪੂਰਨ exitval)
{
# ਜੇ defined(SIGHUP) && defined(SIG_IGN)
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
#ਜੇ_ਅੰਤ


/*
 * Exit properly.  This is the only way to exit Vim after startup has
 * succeeded.  We are certain to exit here, no way to abort it.
 */
    ਖਾਲੀ
getout(ਪੂਰਨ exitval)
{
    exiting = TRUE;
#ਜੇ defined(FEAT_EVAL)
    ch_log(ਨਿਰਾ, "Exiting...");
#ਜੇ_ਅੰਤ

    // When running in Ex mode an error causes us to exit with a non-zero exit
    // code.  POSIX requires this, although it's not 100% clear from the
    // standard.
    ਜੇ (exmode_ਸਰਗਰਮ)
	exitval += ex_exitval;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸੈੱਟ_vim_var_type(VV_EXITING, VAR_ਸੰਖਿਆ);
    ਸੈੱਟ_vim_var_nr(VV_EXITING, exitval);
#ਜੇ_ਅੰਤ

    // Position the cursor on the last screen line, below all the text
    ਜੇ (!is_not_a_ਪਦ_or_gui())
	windgoto((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // Invoked all deferred functions in the function stack.
    invoke_all_defer();
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
    // Optionally print hashtable efficiency.
    hash_debug_results();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਸੁਨੇਹਾ_didany = FALSE;
#ਜੇ_ਅੰਤ

    ਜੇ (v_dying <= 1)
    {
	tabpage_T	*tp;
	tabpage_T	*ਅਗਲਾ_tp;
	buf_T		*buf;
	win_T		*wp;
	ਪੂਰਨ		unblock = 0;

	// Trigger BufWinLeave for all windows, but only once per buffer.
	ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp != ਨਿਰਾ; tp = ਅਗਲਾ_tp)
	{
	    ਅਗਲਾ_tp = tp->tp_ਅਗਲਾ;
	    FOR_ALL_WINDOWS_ਵਿੱਚ_TAB(tp, wp)
	    {
		ਜੇ (wp->w_ਬਫਰ == ਨਿਰਾ || !buf_valid(wp->w_ਬਫਰ))
		    // Autocmd must have close the buffer already, skip.
		    ਜਾਰੀ;
		buf = wp->w_ਬਫਰ;
		ਜੇ (CHANGEDTICK(buf) != -1)
		{
		    bufref_T bufref;

		    ਸੈੱਟ_bufref(&bufref, buf);
		    apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname,
						    buf->b_fname, FALSE, buf);
		    ਜੇ (bufref_valid(&bufref))
			CHANGEDTICK(buf) = -1;  // note we did it already

		    // start all over, autocommands may mess up the lists
		    ਅਗਲਾ_tp = ਪਹਿਲਾ_tabpage;
		    ਰੋਕੋ;
		}
	    }
	}

	// Trigger BufUnload for buffers that are loaded
	FOR_ALL_BUFFERS(buf)
	    ਜੇ (buf->b_ml.ml_mfp != ਨਿਰਾ)
	    {
		bufref_T bufref;

		ਸੈੱਟ_bufref(&bufref, buf);
		apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf);
		ਜੇ (!bufref_valid(&bufref))
		    // autocmd deleted the buffer
		    ਰੋਕੋ;
	    }

	// deathtrap() blocks autocommands, but we do want to trigger
	// VimLeavePre.
	ਜੇ (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVEPRE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	ਜੇ (unblock)
	    ਖੰਡ_autocmds();
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    ਜੇ (
# ifdef EXITFREE
	    entered_free_all_mem == FALSE &&
# endif
	    *p_viminfo != NUL)
	// Write out the registers, history, marks etc, to the viminfo file
	write_viminfo(ਨਿਰਾ, FALSE);
#ਜੇ_ਅੰਤ

    ਜੇ (v_dying <= 1)
    {
	ਪੂਰਨ	unblock = 0;

	// deathtrap() blocks autocommands, but we do want to trigger VimLeave.
	ਜੇ (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	ਜੇ (unblock)
	    ਖੰਡ_autocmds();
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    profile_dump();
#ਜੇ_ਅੰਤ

    ਜੇ (did_emsg
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    || (gui.ਵਿੱਚ_use && ਸੁਨੇਹਾ_didany && p_verbose > 0)
#ਜੇ_ਅੰਤ
	    )
    {
	// give the user a chance to read the (error) message
	no_wait_return = FALSE;
	wait_return(FALSE);
    }

    // Position the cursor again, the autocommands may have moved it
    ਜੇ (!is_not_a_ਪਦ_or_gui())
	windgoto((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਕੰਮ_stop_on_exit();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LUA
    lua_ਸਿਰਾ();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
    mzscheme_ਸਿਰਾ();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TCL
    tcl_ਸਿਰਾ();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RUBY
    ruby_ਸਿਰਾ();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
    python_ਸਿਰਾ();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
    python3_ਸਿਰਾ();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERL
    perl_ਸਿਰਾ();
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
    iconv_ਸਿਰਾ();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    netbeans_ਸਿਰਾ();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOCKETSERVER
    socketserver_stop();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
    cs_ਸਿਰਾ();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
    uninit_image_state();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (garbage_collect_at_exit)
	garbage_collect(FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    free_cmd_ਦਲੀਲਾਂ_W();
#ਜੇ_ਅੰਤ

    ਪਦ_disable_dec();

    mch_exit(exitval);
}

/*
 * Get the name of the display, before gui_prepare() removes it from
 * argv[].  Used for the xterm-clipboard display.
 *
 * Also find the --server, --clientserver... arguments and --socketid and
 * --windowid
 */
    ਸਥਿਰ ਖਾਲੀ
early_arg_ਜਾਂਚ(mparm_T *parmp ਅਣਵਰਤਿਆ)
{
#ਜੇ defined(FEAT_XCLIPBOARD) || defined(FEAT_CLIENTSERVER) \
	|| !defined(FEAT_NETBEANS_INTG)
    ਪੂਰਨ		argc = parmp->argc;
    ਅੱਖਰ	**argv = parmp->argv;
    ਪੂਰਨ		i;

    ਲਈ (i = 1; i < argc; i++)
    {
	ਜੇ (STRCMP(argv[i], "--") == 0)
	    ਰੋਕੋ;
# ifdef FEAT_XCLIPBOARD
	ਨਹੀਂਤਾਂ ਜੇ (STRICMP(argv[i], "-display") == 0
#  ਜੇ defined(FEAT_GUI_GTK)
		|| STRICMP(argv[i], "--display") == 0
#  endif
		)
	{
	    ਜੇ (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    xterm_display = argv[++i];
	}
# endif
# ifdef FEAT_CLIENTSERVER
	ਨਹੀਂਤਾਂ ਜੇ (STRICMP(argv[i], "--servername") == 0)
	{
	    ਜੇ (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    parmp->serverName_arg = (char_u *)argv[++i];
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRICMP(argv[i], "--serverlist") == 0)
	    parmp->serverArg = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    parmp->serverArg = TRUE;
#  ifdef FEAT_GUI
	    ਜੇ (ਲੜੀ_ਖੋਜ(argv[i], "-wait") != 0)
		// don't fork() when starting the GUI to edit files ourself
		gui.dofork = false;
#  endif
	}
#  ifdef FEAT_CLIENTSERVER_BACKENDS
	ਨਹੀਂਤਾਂ ਜੇ (STRICMP(argv[i], "--clientserver") == 0)
	{
	    char_u *arg;
	    ਜੇ (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    arg = (char_u *)argv[++i];

	    ਜੇ (STRICMP(arg, "socket") == 0)
		clientserver_ਵਿਧੀ = CLIENTSERVER_ਵਿਧੀ_SOCKET;
#   ifdef FEAT_X11
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(arg, "x11") == 0)
		clientserver_ਵਿਧੀ = CLIENTSERVER_ਵਿਧੀ_X11;
#   endif
#   ifdef MSWIN
	    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(arg, "mswin") == 0)
		clientserver_ਵਿਧੀ = CLIENTSERVER_ਵਿਧੀ_MSWIN;
#   endif
	    ਨਹੀਂਤਾਂ
		mainerr(ME_UNKNOWN_OPTION, arg);
	}
#  endif
# endif

# ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
#  ifdef FEAT_GUI_MSWIN
	ਨਹੀਂਤਾਂ ਜੇ (STRICMP(argv[i], "--windowid") == 0)
#  ਨਹੀਂਤਾਂ
	ਨਹੀਂਤਾਂ ਜੇ (STRICMP(argv[i], "--socketid") == 0)
#  endif
	{
	    long_u	id;
	    ਪੂਰਨ		ਗਿਣਤੀ_2;

	    ਜੇ (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    ਜੇ (STRNICMP(argv[i+1], "0x", 2) == 0)
		ਗਿਣਤੀ_2 = ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(&(argv[i + 1][2]), SCANF_HEX_LONG_U, &id);
	    ਨਹੀਂਤਾਂ
		ਗਿਣਤੀ_2 = ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(argv[i + 1], SCANF_DECIMAL_LONG_U, &id);
	    ਜੇ (ਗਿਣਤੀ_2 != 1)
		mainerr(ME_INVALID_ARG, (char_u *)argv[i]);
	    ਨਹੀਂਤਾਂ
#  ifdef FEAT_GUI_MSWIN
		win_socket_id = id;
#  elif !defined(USE_GTK4)
		gtk_socket_id = id;
#  endif
	    i++;
	}
# endif
# ਜੇ defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
	ਨਹੀਂਤਾਂ ਜੇ (STRICMP(argv[i], "--echo-wid") == 0)
	    echo_wid_arg = TRUE;
# endif
# ifndef FEAT_NETBEANS_INTG
	ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(argv[i], "-nb", (size_t)3) == 0)
	{
	    mch_errmsg(_("'-nb' cannot be used: not enabled at compile time\n"));
	    mch_exit(2);
	}
# endif

    }
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NO_VIM_MAIN
/*
 * Get a (optional) count for a Vim argument.
 */
    ਸਥਿਰ ਪੂਰਨ
get_ਸੰਖਿਆ_arg(
    char_u	*p,	    // pointer to argument
    ਪੂਰਨ		*idx,	    // index in argument, is incremented
    ਪੂਰਨ		def)	    // default value
{
    ਜੇ (vim_isdigit(p[*idx]))
    {
	def = atoi((ਅੱਖਰ *)&(p[*idx]));
	ਜਦੋਂ (vim_isdigit(p[*idx]))
	    *idx = *idx + 1;
    }
    ਵਾਪਸ def;
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
    ਸਥਿਰ ਖਾਲੀ
parse_command_name(mparm_T *parmp)
{
    char_u	*initstr;

    initstr = gettail((char_u *)parmp->argv[0]);

# ifdef FEAT_EVAL
    ਸੈੱਟ_vim_var_string(VV_PROGNAME, initstr, -1);
    ਸੈੱਟ_progpath((char_u *)parmp->argv[0]);
# endif

    ਜੇ (TOLOWER_ASC(initstr[0]) == 'r')
    {
	restricted = TRUE;
	++initstr;
    }

    // Use evim mode for "evim" and "egvim", not for "editor".
    ਜੇ (TOLOWER_ASC(initstr[0]) == 'e'
	    && (TOLOWER_ASC(initstr[1]) == 'v'
		|| TOLOWER_ASC(initstr[1]) == 'g'))
    {
# ifdef FEAT_GUI
	gui.starting = true;
# endif
	parmp->evim_ਬਹੁਲਕ = TRUE;
	++initstr;
    }

    // "gvim" starts the GUI.  Also accept "Gvim" for MS-Windows.
    ਜੇ (TOLOWER_ASC(initstr[0]) == 'g'
# ifdef VIMDLL
	    || mch_is_gui_executable()
# endif
       )
    {
	main_ਸ਼ੁਰੂ_gui();
# ifdef FEAT_GUI
	++initstr;
# endif
# ifdef GUI_MAY_SPAWN
	gui.dospawn = false;	// No need to spawn a new process.
# endif
    }
# ifdef GUI_MAY_SPAWN
    ਨਹੀਂਤਾਂ
	gui.dospawn = true;	// Not "gvim". Need to spawn gvim.exe.
# endif


    ਜੇ (STRNICMP(initstr, "view", 4) == 0)
    {
	readonlymode = TRUE;
	curbuf->b_p_ro = TRUE;
	p_uc = 10000;			// don't update very often
	initstr += 4;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(initstr, "vim", 3) == 0)
	initstr += 3;

    // Catch "[r][g]vimdiff" and "[r][g]viewdiff".
    ਜੇ (STRICMP(initstr, "diff") == 0)
    {
# ifdef FEAT_DIFF
	parmp->diff_ਬਹੁਲਕ = TRUE;
# ਨਹੀਂਤਾਂ
	mch_errmsg(_("This Vim was not compiled with the diff feature."));
	mch_errmsg("\n");
	mch_exit(2);
# endif
    }

    // Checking for "ex" here may catch some weird names, such as "vimex" or
    // "viewex", we assume the user knows that.
    ਜੇ (STRNICMP(initstr, "ex", 2) == 0)
    {
	ਜੇ (STRNICMP(initstr + 2, "im", 2) == 0)
	    exmode_ਸਰਗਰਮ = EXMODE_VIM;
	ਨਹੀਂਤਾਂ
	    exmode_ਸਰਗਰਮ = EXMODE_ਸਾਧਾਰਨ;
	change_compatible(TRUE);	// set 'compatible'
    }
}

/*
 * Scan the command line arguments.
 */
    ਸਥਿਰ ਖਾਲੀ
command_ਸਤਰ_ਜਾਂਚ(mparm_T *parmp)
{
    ਪੂਰਨ		argc = parmp->argc;
    ਅੱਖਰ	**argv = parmp->argv;
    ਪੂਰਨ		argv_idx;		// index in argv[n][]
    ਪੂਰਨ		had_minmin = FALSE;	// found "--" argument
    ਪੂਰਨ		want_ਦਲੀਲ;		// option argument with argument
    ਪੂਰਨ		c;
    char_u	*p = ਨਿਰਾ;
    ਲੰਮਾ	n;

    --argc;
    ++argv;
    argv_idx = 1;	    // active option letter is argv[0][argv_idx]
    ਜਦੋਂ (argc > 0)
    {
	/*
	 * "+" or "+{number}" or "+/{pat}" or "+{command}" argument.
	 */
	ਜੇ (argv[0][0] == '+' && !had_minmin)
	{
	    ਜੇ (parmp->n_commands >= MAX_ARG_CMDS)
		mainerr(ME_EXTRA_CMD, ਨਿਰਾ);
	    argv_idx = -1;	    // skip to next argument
	    ਜੇ (argv[0][1] == NUL)
		parmp->commands[parmp->n_commands++] = (char_u *)"$";
	    ਨਹੀਂਤਾਂ
		parmp->commands[parmp->n_commands++] = (char_u *)&(argv[0][1]);
	}

	/*
	 * Optional argument.
	 */
	ਨਹੀਂਤਾਂ ਜੇ (argv[0][0] == '-' && !had_minmin)
	{
	    want_ਦਲੀਲ = FALSE;
	    c = argv[0][argv_idx++];
# ਜੇ defined( VMS)
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
	    ਜੇ (c == '/')
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
	    ਚੋਣ (c)
	    {
	    ਹਾਲਤ NUL:		// "vim -"  read from stdin
				// "ex -" silent mode
		ਜੇ (exmode_ਸਰਗਰਮ)
		    silent_ਬਹੁਲਕ = TRUE;
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (parmp->edit_type != EDIT_NONE)
			mainerr(ME_TOO_MANY_ਦਲੀਲਾਂ, (char_u *)argv[0]);
		    parmp->edit_type = EDIT_STDIN;
		    read_cmd_fd = 2;	// read from stderr instead of stdin
		}
		argv_idx = -1;		// skip to next argument
		ਰੋਕੋ;

	    ਹਾਲਤ '-':		// "--" don't take any more option arguments
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
		ਜੇ (STRICMP(argv[0] + argv_idx, "help") == 0)
		    usage();
		ਨਹੀਂਤਾਂ ਜੇ (STRICMP(argv[0] + argv_idx, "version") == 0)
		{
		    cmdline_ਚੌੜਾਈ = Columns = 80;   // need to init Columns
		    info_ਸੁਨੇਹਾ = TRUE; // use mch_msg(), not mch_errmsg()
# ਜੇ defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
		    gui.starting = false; // not starting GUI, will exit
# endif
		    list_ਸੰਸਕਰਣ();
		    ਸੁਨੇਹਾ_putchar('\n');
		    ਸੁਨੇਹਾ_didout = FALSE;
		    mch_exit(0);
		}
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "clean", 5) == 0)
		{
		    parmp->use_vimrc = (char_u *)"DEFAULTS";
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)"NONE";
# endif
		    parmp->clean = TRUE;
		    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"vif",
						      0L, (char_u *)"NONE", 0);
		}
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "literal", 7) == 0)
		{
# ifdef EXPAND_FILENAMES
		    parmp->literal = TRUE;
# endif
		}
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "nofork", 6) == 0)
		{
# ifdef FEAT_GUI
		    gui.dofork = false;	// don't fork() when starting GUI
# endif
		}
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "noplugin", 8) == 0)
		    p_lpl = FALSE;
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "not-a-term", 10) == 0)
		    parmp->not_a_ਪਦ = TRUE;
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "gui-dialog-file", 15)
									 == 0)
		{
		    want_ਦਲੀਲ = TRUE;
		    argv_idx += 15;
		}
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "ttyfail", 7) == 0)
		    parmp->tty_fail = TRUE;
# ifdef FEAT_ਕੰਮ_CHANNEL
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "stdio-channel", 13) == 0)
		{
		    // Like "-es": no terminal, but the channel is served
		    // instead of reading Ex commands.
		    use_stdio_channel = TRUE;
		    exmode_ਸਰਗਰਮ = EXMODE_ਸਾਧਾਰਨ;
		    silent_ਬਹੁਲਕ = TRUE;
#  ਜੇ defined(FEAT_GUI) && !defined(VIMDLL)
		    gui.starting = false;	// don't start GUI
#  endif
		}
# endif
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "cmd", 3) == 0)
		{
		    want_ਦਲੀਲ = TRUE;
		    argv_idx += 3;
		}
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "startuptime", 11) == 0)
		{
		    want_ਦਲੀਲ = TRUE;
		    argv_idx += 11;
		}
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "log", 3) == 0)
		{
		    want_ਦਲੀਲ = TRUE;
		    argv_idx += 3;
		}
# ifdef FEAT_CLIENTSERVER
		ਨਹੀਂਤਾਂ ਜੇ (STRICMP(argv[0] + argv_idx, "serverlist") == 0)
		    ; // already processed -- no arg
		ਨਹੀਂਤਾਂ ਜੇ (STRICMP(argv[0] + argv_idx, "servername") == 0
		       || STRICMP(argv[0] + argv_idx, "serversend") == 0
		       // Don't put this under FEAT_CLIENTSERVER_BACKENDS, just
		       // let it be ignored. Makes tests less complicated
		       || STRICMP(argv[0] + argv_idx, "clientserver") == 0
		       )
		{
		    // already processed -- snatch the following arg
		    ਜੇ (argc > 1)
		    {
			--argc;
			++argv;
		    }
		}
# endif
# ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
#  ifdef FEAT_GUI_GTK
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "socketid", 8) == 0)
#  ਨਹੀਂਤਾਂ
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "windowid", 8) == 0)
#  endif
		{
		    // already processed -- snatch the following arg
		    ਜੇ (argc > 1)
		    {
			--argc;
			++argv;
		    }
		}
# endif
# ifdef FEAT_GUI_GTK
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[0] + argv_idx, "echo-wid", 8) == 0)
		{
		    // already processed, skip
		}
# endif
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (argv[0][argv_idx])
			mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
		    had_minmin = TRUE;
		}
		ਜੇ (!want_ਦਲੀਲ)
		    argv_idx = -1;	// skip to next argument
		ਰੋਕੋ;

	    ਹਾਲਤ 'A':		// "-A" start in Arabic mode
# ifdef FEAT_ARABIC
		ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"arabic", 1L, ਨਿਰਾ, 0);
# ਨਹੀਂਤਾਂ
		mch_errmsg(_(e_arabic_cannot_be_ਵਰਤਿਆ_not_enabled_at_compile_ਸਮਾਂ));
		mch_exit(2);
# endif
		ਰੋਕੋ;

	    ਹਾਲਤ 'b':		// "-b" binary mode
		// Needs to be effective before expanding file names, because
		// for Win32 this makes us edit a shortcut file itself,
		// instead of the file it links to.
		ਸੈੱਟ_options_bin(curbuf->b_p_bin, 1, 0);
		curbuf->b_p_bin = 1;	    // binary file I/O
		ਰੋਕੋ;

	    ਹਾਲਤ 'C':		// "-C"  Compatible
		change_compatible(TRUE);
		has_dash_c_arg = TRUE;
		ਰੋਕੋ;

	    ਹਾਲਤ 'e':		// "-e" Ex mode
		exmode_ਸਰਗਰਮ = EXMODE_ਸਾਧਾਰਨ;
		ਰੋਕੋ;

	    ਹਾਲਤ 'E':		// "-E" Improved Ex mode
		exmode_ਸਰਗਰਮ = EXMODE_VIM;
		ਰੋਕੋ;

	    ਹਾਲਤ 'f':		// "-f"  GUI: run in foreground.  Amiga: open
				// window directly, not with newcli
# ifdef FEAT_GUI
		gui.dofork = false;	// don't fork() when starting GUI
# endif
		ਰੋਕੋ;

	    ਹਾਲਤ 'g':		// "-g" start GUI
		main_ਸ਼ੁਰੂ_gui();
		ਰੋਕੋ;

	    ਹਾਲਤ 'F':		// "-F" was for Farsi mode
		mch_errmsg(_(e_farsi_support_has_been_removed));
		mch_exit(2);
		ਰੋਕੋ;

	    ਹਾਲਤ '?':		// "-?" give help message (for MS-Windows)
	    ਹਾਲਤ 'h':		// "-h" give help message
# ifdef FEAT_GUI_GNOME
		// Tell usage() to exit for "gvim".
		gui.starting = false;
# endif
		usage();
		ਰੋਕੋ;

	    ਹਾਲਤ 'H':		// "-H" start in Hebrew mode: rl + hkmap set
# ifdef FEAT_RIGHTLEFT
		p_hkmap = TRUE;
		ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"rl", 1L, ਨਿਰਾ, 0);
# ਨਹੀਂਤਾਂ
		mch_errmsg(_(e_hebrew_cannot_be_ਵਰਤਿਆ_not_enabled_at_compile_ਸਮਾਂ));
		mch_exit(2);
# endif
		ਰੋਕੋ;

	    ਹਾਲਤ 'l':		// "-l" lisp mode, 'lisp' and 'showmatch' on
		ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"lisp", 1L, ਨਿਰਾ, 0);
		p_sm = TRUE;
		ਰੋਕੋ;

	    ਹਾਲਤ 'M':		// "-M"  no changes or writing of files
		reset_modifiable();
		// FALLTHROUGH

	    ਹਾਲਤ 'm':		// "-m"  no writing of files
		p_write = FALSE;
		ਰੋਕੋ;

	    ਹਾਲਤ 'y':		// "-y"  easy mode
# ifdef FEAT_GUI
		gui.starting = true;	// start GUI a bit later
# endif
		parmp->evim_ਬਹੁਲਕ = TRUE;
		ਰੋਕੋ;

	    ਹਾਲਤ 'N':		// "-N"  Nocompatible
		change_compatible(FALSE);
		ਰੋਕੋ;

	    ਹਾਲਤ 'n':		// "-n" no swap file
# ifdef FEAT_NETBEANS_INTG
		// checking for "-nb", netbeans parameters
		ਜੇ (argv[0][argv_idx] == 'b')
		{
		    netbeansArg = argv[0];
		    argv_idx = -1;	    // skip to next argument
		}
		ਨਹੀਂਤਾਂ
# endif
		parmp->no_ਅਦਲਾ_ਬਦਲੀ_file = TRUE;
		ਰੋਕੋ;

	    ਹਾਲਤ 'p':		// "-p[N]" open N tab pages
# ifdef ਨਿਸ਼ਾਨਾ_API_MAC_OSX
		// For some reason on MacOS X, an argument like:
		// -psn_0_10223617 is passed in when invoke from Finder
		// or with the 'open' command
		ਜੇ (argv[0][argv_idx] == 's')
		{
		    argv_idx = -1; // bypass full -psn
		    main_ਸ਼ੁਰੂ_gui();
		    ਰੋਕੋ;
		}
# endif
		// default is 0: open window for each file
		parmp->ਖਿੜਕੀ_ਗਿਣਤੀ = get_ਸੰਖਿਆ_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->ਖਿੜਕੀ_layout = WIN_TABS;
		ਰੋਕੋ;

	    ਹਾਲਤ 'o':		// "-o[N]" open N horizontal split windows
		// default is 0: open window for each file
		parmp->ਖਿੜਕੀ_ਗਿਣਤੀ = get_ਸੰਖਿਆ_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->ਖਿੜਕੀ_layout = WIN_HOR;
		ਰੋਕੋ;

		ਹਾਲਤ 'O':	// "-O[N]" open N vertical split windows
		// default is 0: open window for each file
		parmp->ਖਿੜਕੀ_ਗਿਣਤੀ = get_ਸੰਖਿਆ_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->ਖਿੜਕੀ_layout = WIN_VER;
		ਰੋਕੋ;

# ifdef FEAT_QUICKFIX
	    ਹਾਲਤ 'q':		// "-q" QuickFix mode
		ਜੇ (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_ਦਲੀਲਾਂ, (char_u *)argv[0]);
		parmp->edit_type = EDIT_QF;
		ਜੇ (argv[0][argv_idx])		// "-q{errorfile}"
		{
		    parmp->use_ef = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		ਨਹੀਂਤਾਂ ਜੇ (argc > 1)		// "-q {errorfile}"
		    want_ਦਲੀਲ = TRUE;
		ਰੋਕੋ;
# endif

	    ਹਾਲਤ 'R':		// "-R" readonly mode
		readonlymode = TRUE;
		curbuf->b_p_ro = TRUE;
		p_uc = 10000;			// don't update very often
		ਰੋਕੋ;

	    ਹਾਲਤ 'r':		// "-r" recovery mode
	    ਹਾਲਤ 'L':		// "-L" recovery mode
		recoverymode = 1;
		ਰੋਕੋ;

	    ਹਾਲਤ 's':
		ਜੇ (exmode_ਸਰਗਰਮ)	// "-s" silent (batch) mode
		    silent_ਬਹੁਲਕ = TRUE;
		ਨਹੀਂਤਾਂ		// "-s {scriptin}" read from script file
		    want_ਦਲੀਲ = TRUE;
		ਰੋਕੋ;

	    ਹਾਲਤ 't':		// "-t {tag}" or "-t{tag}" jump to tag
		ਜੇ (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_ਦਲੀਲਾਂ, (char_u *)argv[0]);
		parmp->edit_type = EDIT_ਟੈਗ;
		ਜੇ (argv[0][argv_idx])		// "-t{tag}"
		{
		    parmp->tagname = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		ਨਹੀਂਤਾਂ				// "-t {tag}"
		    want_ਦਲੀਲ = TRUE;
		ਰੋਕੋ;

# ifdef FEAT_EVAL
	    ਹਾਲਤ 'D':		// "-D"		Debugging
		parmp->use_debug_break_level = 9999;
		ਰੋਕੋ;
# endif
# ifdef FEAT_DIFF
	    ਹਾਲਤ 'd':		// "-d"		'diff'
#  ifdef AMIGA
		// check for "-dev {device}"
		ਜੇ (argv[0][argv_idx] == 'e' && argv[0][argv_idx + 1] == 'v')
		    want_ਦਲੀਲ = TRUE;
		ਨਹੀਂਤਾਂ
#  endif
		    parmp->diff_ਬਹੁਲਕ = TRUE;
		ਰੋਕੋ;
# endif
	    ਹਾਲਤ 'V':		// "-V{N}"	Verbose level
		// default is 10: a little bit verbose
		p_verbose = get_ਸੰਖਿਆ_arg((char_u *)argv[0], &argv_idx, 10);
		ਜੇ (argv[0][argv_idx] != NUL)
		{
		    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"verbosefile",
					  0L, (char_u *)argv[0] + argv_idx, 0);
		    argv_idx = (ਪੂਰਨ)STRLEN(argv[0]);
		}
		ਰੋਕੋ;

	    ਹਾਲਤ 'v':		// "-v"  Vi-mode (as if called "vi")
		exmode_ਸਰਗਰਮ = 0;
# ਜੇ defined(FEAT_GUI) && !defined(VIMDLL)
		gui.starting = false;	// don't start GUI
# endif
		ਰੋਕੋ;

	    ਹਾਲਤ 'w':		// "-w{number}"	set window height
				// "-w {scriptout}"	write to script
		ਜੇ (vim_isdigit(((char_u *)argv[0])[argv_idx]))
		{
		    n = get_ਸੰਖਿਆ_arg((char_u *)argv[0], &argv_idx, 10);
		    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"window", n, ਨਿਰਾ, 0);
		    ਰੋਕੋ;
		}
		want_ਦਲੀਲ = TRUE;
		ਰੋਕੋ;

# ifdef FEAT_CRYPT
	    ਹਾਲਤ 'x':		// "-x"  encrypted reading/writing of files
		parmp->ask_for_ਕੁੰਜੀ = TRUE;
		ਰੋਕੋ;
# endif

	    ਹਾਲਤ 'X':		// "-X"  don't connect to X server
# ਜੇ (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
		x_no_connect = TRUE;
# endif
		ਰੋਕੋ;
	    ਹਾਲਤ 'Y':		// "-Y" don't connect to Wayland compositor
# ਜੇ defined(FEAT_WAYLAND)
		wayland_no_connect = TRUE;
# endif
		ਰੋਕੋ;

	    ਹਾਲਤ 'Z':		// "-Z"  restricted mode
		restricted = TRUE;
		ਰੋਕੋ;

	    ਹਾਲਤ 'c':		// "-c{command}" or "-c {command}" execute
				// command
		ਜੇ (argv[0][argv_idx] != NUL)
		{
		    ਜੇ (parmp->n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, ਨਿਰਾ);
		    parmp->commands[parmp->n_commands++] = (char_u *)argv[0]
								   + argv_idx;
		    argv_idx = -1;
		    ਰੋਕੋ;
		}
		// FALLTHROUGH
	    ਹਾਲਤ 'S':		// "-S {file}" execute Vim script
	    ਹਾਲਤ 'i':		// "-i {viminfo}" use for viminfo
# ifndef FEAT_DIFF
	    ਹਾਲਤ 'd':		// "-d {device}" device (for Amiga)
# endif
	    ਹਾਲਤ 'T':		// "-T {terminal}" terminal name
	    ਹਾਲਤ 'u':		// "-u {vimrc}" vim inits file
	    ਹਾਲਤ 'U':		// "-U {gvimrc}" gvim inits file
	    ਹਾਲਤ 'W':		// "-W {scriptout}" overwrite
# ifdef FEAT_GUI_MSWIN
	    ਹਾਲਤ 'P':		// "-P {parent title}" MDI parent
# endif
		want_ਦਲੀਲ = TRUE;
		ਰੋਕੋ;

	    ਮੂਲ:
		mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
	    }

	    /*
	     * Handle option arguments with argument.
	     */
	    ਜੇ (want_ਦਲੀਲ)
	    {
		/*
		 * Check for garbage immediately after the option letter.
		 */
		ਜੇ (argv[0][argv_idx] != NUL)
		    mainerr(ME_GARBAGE, (char_u *)argv[0]);

		--argc;
		ਜੇ (argc < 1 && c != 'S')  // -S has an optional argument
		    mainerr_arg_missing((char_u *)argv[0]);
		++argv;
		argv_idx = -1;

		ਚੋਣ (c)
		{
		ਹਾਲਤ 'c':	// "-c {command}" execute command
		ਹਾਲਤ 'S':	// "-S {file}" execute Vim script
		    ਜੇ (parmp->n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, ਨਿਰਾ);
		    ਜੇ (c == 'S')
		    {
			ਅੱਖਰ	*a;

			ਜੇ (argc < 1)
			    // "-S" without argument: use default session file
			    // name.
			    a = SESSION_FILE;
			ਨਹੀਂਤਾਂ ਜੇ (argv[0][0] == '-')
			{
			    // "-S" followed by another option: use default
			    // session file name.
			    a = SESSION_FILE;
			    ++argc;
			    --argv;
			}
			ਨਹੀਂਤਾਂ
			    a = argv[0];
			p = alloc(STRLEN(a) + 4);
			ਜੇ (p == ਨਿਰਾ)
			    mch_exit(2);
			sprintf((ਅੱਖਰ *)p, "so %s", a);
			parmp->cmds_tofree[parmp->n_commands] = TRUE;
			parmp->commands[parmp->n_commands++] = p;
		    }
		    ਨਹੀਂਤਾਂ
			parmp->commands[parmp->n_commands++] =
							    (char_u *)argv[0];
		    ਰੋਕੋ;

		ਹਾਲਤ '-':
		    ਜੇ (argv[-1][2] == 'c')
		    {
			// "--cmd {command}" execute command
			ਜੇ (parmp->n_pre_commands >= MAX_ARG_CMDS)
			    mainerr(ME_EXTRA_CMD, ਨਿਰਾ);
			parmp->pre_commands[parmp->n_pre_commands++] =
							    (char_u *)argv[0];
		    }
		    // --gui-dialog-file fname
		    ਜੇ (argv[-1][2] == 'g')
		    {
			// without GUI ignore the argument
# ifdef FEAT_GUI
			parmp->gui_dialog_file = (char_u *)argv[0];
# endif
		    }

		    // "--startuptime <file>" already handled
		    // "--log <file>" already handled
		    ਰੋਕੋ;

	    //	case 'd':   -d {device} is handled in mch_check_win() for the
	    //		    Amiga

# ifdef FEAT_QUICKFIX
		ਹਾਲਤ 'q':	// "-q {errorfile}" QuickFix mode
		    parmp->use_ef = (char_u *)argv[0];
		    ਰੋਕੋ;
# endif

		ਹਾਲਤ 'i':	// "-i {viminfo}" use for viminfo
		    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"vif",
						     0L, (char_u *)argv[0], 0);
		    ਰੋਕੋ;

		ਹਾਲਤ 's':	// "-s {scriptin}" read from script file
		    ਜੇ (scriptin[0] != ਨਿਰਾ)
		    {
scripterror:
			mch_errmsg(_("Attempt to open script file again: \""));
			mch_errmsg(argv[-1]);
			mch_errmsg(" ");
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    ਜੇ ((scriptin[0] = mch_fopen(argv[0], READBIN)) == ਨਿਰਾ)
		    {
			mch_errmsg(_("Cannot open for reading: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    ਜੇ (save_typebuf() == FAIL)
			mch_exit(2);	// out of memory
		    ਰੋਕੋ;

		ਹਾਲਤ 't':	// "-t {tag}"
		    parmp->tagname = (char_u *)argv[0];
		    ਰੋਕੋ;

		ਹਾਲਤ 'T':	// "-T {terminal}" terminal name
		    /*
		     * The -T term argument is always available and when
		     * HAVE_TERMLIB is supported it overrides the environment
		     * variable TERM.
		     */
# ifdef FEAT_GUI
		    ਜੇ (ਪਦ_is_gui((char_u *)argv[0]))
			gui.starting = true;	// start GUI a bit later
		    ਨਹੀਂਤਾਂ
# endif
			parmp->ਪਦ = (char_u *)argv[0];
		    ਰੋਕੋ;

		ਹਾਲਤ 'u':	// "-u {vimrc}" vim inits file
		    parmp->use_vimrc = (char_u *)argv[0];
		    ਰੋਕੋ;

		ਹਾਲਤ 'U':	// "-U {gvimrc}" gvim inits file
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)argv[0];
# endif
		    ਰੋਕੋ;

		ਹਾਲਤ 'w':	// "-w {nr}" 'window' value
				// "-w {scriptout}" append to script file
		    ਜੇ (vim_isdigit(*((char_u *)argv[0])))
		    {
			argv_idx = 0;
			n = get_ਸੰਖਿਆ_arg((char_u *)argv[0], &argv_idx, 10);
			ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"window",
								   n, ਨਿਰਾ, 0);
			argv_idx = -1;
			ਰੋਕੋ;
		    }
		    // FALLTHROUGH
		ਹਾਲਤ 'W':	// "-W {scriptout}" overwrite script file
		    ਜੇ (scriptout != ਨਿਰਾ)
			ਜਾਓ scripterror;
		    ਜੇ ((scriptout = mch_fopen(argv[0],
				    c == 'w' ? APPENDBIN : WRITEBIN)) == ਨਿਰਾ)
		    {
			mch_errmsg(_("Cannot open for script output: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    ਰੋਕੋ;

# ifdef FEAT_GUI_MSWIN
		ਹਾਲਤ 'P':		// "-P {parent title}" MDI parent
		    gui_mch_ਸੈੱਟ_ਮਾਪਾ(argv[0]);
		    ਰੋਕੋ;
# endif
		}
	    }
	}

	/*
	 * File name argument.
	 */
	ਨਹੀਂਤਾਂ
	{
	    argv_idx = -1;	    // skip to next argument

	    // Check for only one type of editing.
	    ਜੇ (parmp->edit_type != EDIT_NONE && parmp->edit_type != EDIT_FILE)
		mainerr(ME_TOO_MANY_ਦਲੀਲਾਂ, (char_u *)argv[0]);
	    parmp->edit_type = EDIT_FILE;

# ifdef MSWIN
	    // Remember if the argument was a full path before changing
	    // slashes to backslashes.
	    ਜੇ (argv[0][0] != NUL && argv[0][1] == ':' && argv[0][2] == '\\')
		parmp->ਭਰਿਆ_ਰਸਤਾ = TRUE;
# endif

	    // Add the file to the global argument list.
	    ਜੇ (ga_grow(&global_alist.al_ga, 1) == FAIL
		    || (p = vim_strsave((char_u *)argv[0])) == ਨਿਰਾ)
		mch_exit(2);
# ifdef FEAT_DIFF
	    ਜੇ (parmp->diff_ਬਹੁਲਕ && mch_isdir(p) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *ਪੂਛ;
		string_T    ret;

		ਪੂਛ = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(p, STRLEN(p), ਪੂਛ, STRLEN(ਪੂਛ), TRUE, &ret);
		ਜੇ (ret.string != ਨਿਰਾ)
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
	    ਜੇ (vim_strpbrk(p, "\\:") != ਨਿਰਾ && !ਰਸਤਾ_with_url(p))
	    {
		ਅੱਖਰ posix_ਰਸਤਾ[MAXPATHL];

#  ਜੇ CYGWIN_ਸੰਸਕਰਣ_DLL_MAJOR >= 1007
		cygwin_conv_ਰਸਤਾ(CCP_WIN_A_ਮੰਜ਼ਿਲ_POSIX, p, posix_ਰਸਤਾ, MAXPATHL);
#  ਨਹੀਂਤਾਂ
		cygwin_conv_ਮੰਜ਼ਿਲ_posix_ਰਸਤਾ(p, posix_ਰਸਤਾ);
#  endif
		vim_free(p);
		p = vim_strsave((char_u *)posix_ਰਸਤਾ);
		ਜੇ (p == ਨਿਰਾ)
		    mch_exit(2);
	    }
# endif

# ifdef USE_FNAME_CASE
	    // Make the case of the file name match the actual file.
	    fname_case(p, 0);
# endif

	    alist_ਜੋੜੋ(&global_alist, p,
# ifdef EXPAND_FILENAMES
		    parmp->literal ? 2 : 0	// add buffer nr after exp.
# ਨਹੀਂਤਾਂ
		    2		// add buffer number now and use curbuf
# endif
		    );

# ifdef MSWIN
	    {
		// Remember this argument has been added to the argument list.
		// Needed when 'encoding' is changed.
		ਵਰਤਿਆ_file_arg(argv[0], parmp->literal, parmp->ਭਰਿਆ_ਰਸਤਾ,
#  ifdef FEAT_DIFF
							    parmp->diff_ਬਹੁਲਕ
#  ਨਹੀਂਤਾਂ
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
	ਜੇ (argv_idx <= 0 || argv[0][argv_idx] == NUL)
	{
	    --argc;
	    ++argv;
	    argv_idx = 1;
	}
    }

# ifdef FEAT_EVAL
    // If there is a "+123" or "-c" command, set v:swapcommand to the first
    // one.
    ਜੇ (parmp->n_commands > 0)
    {
	size_t  plen;

	plen = STRLEN(parmp->commands[0]) + 2;
	p = alloc(plen + 1);
	ਜੇ (p != ਨਿਰਾ)
	{
	    sprintf((ਅੱਖਰ *)p, ":%s\r", parmp->commands[0]);
	    ਸੈੱਟ_vim_var_string(VV_SWAPCOMMAND, p, (ਪੂਰਨ)plen);
	    vim_free(p);
	}
    }
# endif
}

/*
 * Print a warning if stdout is not a terminal.
 * When starting in Ex mode and commands come from a file, set silent_mode.
 */
    ਸਥਿਰ ਖਾਲੀ
check_tty(mparm_T *parmp)
{
    ਪੂਰਨ		ਨਿਵੇਸ਼_isatty;		// is active input a terminal?

    ਨਿਵੇਸ਼_isatty = mch_ਨਿਵੇਸ਼_isatty();
    ਜੇ (exmode_ਸਰਗਰਮ)
    {
	ਜੇ (!ਨਿਵੇਸ਼_isatty)
	    silent_ਬਹੁਲਕ = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (parmp->want_ਭਰਿਆ_screen && (!stdout_isatty || !ਨਿਵੇਸ਼_isatty)
# ifdef FEAT_GUI
	    // don't want the delay when started from the desktop
	    && !gui.starting
# endif
	    && !parmp->not_a_ਪਦ)
    {
# ifdef NBDEBUG
	/*
	 * This shouldn't be necessary. But if I run netbeans with the log
	 * output coming to the console and XOpenDisplay fails, I get vim
	 * trying to start with input/output to my console tty.  This fills my
	 * input buffer so fast I can't even kill the process in under 2
	 * minutes (and it beeps continuously the whole time :-)
	 */
	ਜੇ (netbeans_ਸਰਗਰਮ() && (!stdout_isatty || !ਨਿਵੇਸ਼_isatty))
	{
	    mch_errmsg(_("Vim: Error: Failure to start gvim from NetBeans\n"));
	    ਬਾਹਰ_ਨਿਕਲੋ(1);
	}
# endif
# ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	ਜੇ (
#  ifdef VIMDLL
	    !gui.starting &&
#  endif
	    is_cygpty_ਵਰਤਿਆ())
	{
#  ਜੇ defined(HAVE_BIND_TEXTDOMAIN_CODESET) \
	&& defined(FEAT_GETTEXT)
	    ਅੱਖਰ    *s, *tofree = ਨਿਰਾ;

	    // Set the encoding of the error message based on $LC_ALL or
	    // other environment variables instead of 'encoding'.
	    // Note that the message is shown on a Cygwin terminal (e.g.
	    // mintty) which encoding is based on $LC_ALL or etc., not the
	    // current codepage used by normal Win32 console programs.
	    tofree = s = (ਅੱਖਰ *)enc_locale_env(ਨਿਰਾ);
	    ਜੇ (s == ਨਿਰਾ)
		s = "utf-8";	// Use "utf-8" by default.
	    (ਖਾਲੀ)bind_textdomain_codeset(VIMPACKAGE, s);
	    vim_free(tofree);
#  endif
	    mch_errmsg(_("Vim: Error: This version of Vim does not run in a Cygwin terminal\n"));
	    ਬਾਹਰ_ਨਿਕਲੋ(1);
	}
# endif
	ਜੇ (!stdout_isatty)
	    mch_errmsg(_("Vim: Warning: Output is not to a terminal\n"));
	ਜੇ (!ਨਿਵੇਸ਼_isatty)
	    mch_errmsg(_("Vim: Warning: Input is not from a terminal\n"));
	ਬਾਹਰਲਾ_flush();
	ਜੇ (parmp->tty_fail && (!stdout_isatty || !ਨਿਵੇਸ਼_isatty))
	    ਬਾਹਰ_ਨਿਕਲੋ(1);
	ਜੇ (scriptin[0] == ਨਿਰਾ)
	    ui_delay(2005L, TRUE);
	ਸਮਾਂ_ਸੁਨੇਹਾ("Warning delay");
    }
}

/*
 * Read text from stdin.
 */
    ਸਥਿਰ ਖਾਲੀ
read_stdin(ਖਾਲੀ)
{
    ਪੂਰਨ	    i;

    // When getting the ATTENTION prompt here, use a dialog
    ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_DIALOG;

    no_wait_return = TRUE;
    i = ਸੁਨੇਹਾ_didany;
    ਸੈੱਟ_buflisted(TRUE);

    // Create memfile and read from stdin.
    (ਖਾਲੀ)open_ਬਫਰ(TRUE, ਨਿਰਾ, 0);

    no_wait_return = FALSE;
    ਸੁਨੇਹਾ_didany = i;
    ਸਮਾਂ_ਸੁਨੇਹਾ("reading stdin");

    check_ਅਦਲਾ_ਬਦਲੀ_exists_action();

# ਜੇ !(defined(AMIGA) || defined(MACOS_X))
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
    ਸਥਿਰ ਖਾਲੀ
create_windows(mparm_T *parmp ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		dorewind;
    ਪੂਰਨ		ਪੂਰਾ = 0;

    /*
     * Create the number of windows that was requested.
     */
    ਜੇ (parmp->ਖਿੜਕੀ_ਗਿਣਤੀ == -1)	// was not set
	parmp->ਖਿੜਕੀ_ਗਿਣਤੀ = 1;
    ਜੇ (parmp->ਖਿੜਕੀ_ਗਿਣਤੀ == 0)
	parmp->ਖਿੜਕੀ_ਗਿਣਤੀ = GARGCOUNT;
    ਜੇ (parmp->ਖਿੜਕੀ_ਗਿਣਤੀ > 1)
    {
	// Don't change the windows if there was a command in .vimrc that
	// already split some windows
	ਜੇ (parmp->ਖਿੜਕੀ_layout == 0)
	    parmp->ਖਿੜਕੀ_layout = WIN_HOR;
	ਜੇ (parmp->ਖਿੜਕੀ_layout == WIN_TABS)
	{
	    parmp->ਖਿੜਕੀ_ਗਿਣਤੀ = make_tabpages(parmp->ਖਿੜਕੀ_ਗਿਣਤੀ);
	    ਸਮਾਂ_ਸੁਨੇਹਾ("making tab pages");
	}
	ਨਹੀਂਤਾਂ ਜੇ (firstwin->w_ਅਗਲਾ == ਨਿਰਾ)
	{
	    parmp->ਖਿੜਕੀ_ਗਿਣਤੀ = make_windows(parmp->ਖਿੜਕੀ_ਗਿਣਤੀ,
					     parmp->ਖਿੜਕੀ_layout == WIN_VER);
	    ਸਮਾਂ_ਸੁਨੇਹਾ("making windows");
	}
	ਨਹੀਂਤਾਂ
	    parmp->ਖਿੜਕੀ_ਗਿਣਤੀ = win_ਗਿਣਤੀ();
    }
    ਨਹੀਂਤਾਂ
	parmp->ਖਿੜਕੀ_ਗਿਣਤੀ = 1;

    ਜੇ (recoverymode)			// do recover
    {
	ਸੁਨੇਹਾ_scroll = TRUE;		// scroll message up
	ml_recover(TRUE);
	ਜੇ (curbuf->b_ml.ml_mfp == ਨਿਰਾ) // failed
	    getout(1);
	do_modelines(0);		// do modelines
    }
    ਨਹੀਂਤਾਂ
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
	ਜਦੋਂ (ਪੂਰਾ++ < 1000)
	{
	    ਜੇ (dorewind)
	    {
		ਜੇ (parmp->ਖਿੜਕੀ_layout == WIN_TABS)
		    goto_tabpage(1);
		ਨਹੀਂਤਾਂ
		    curwin = firstwin;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (parmp->ਖਿੜਕੀ_layout == WIN_TABS)
	    {
		ਜੇ (curtab->tp_ਅਗਲਾ == ਨਿਰਾ)
		    ਰੋਕੋ;
		goto_tabpage(0);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (curwin->w_ਅਗਲਾ == ਨਿਰਾ)
		    ਰੋਕੋ;
		curwin = curwin->w_ਅਗਲਾ;
	    }
	    dorewind = FALSE;
	    curbuf = curwin->w_ਬਫਰ;
	    ਜੇ (curbuf->b_ml.ml_mfp == ਨਿਰਾ)
	    {
# ifdef FEAT_FOLDING
		// Set 'foldlevel' to 'foldlevelstart' if it's not negative.
		ਜੇ (p_fdls >= 0)
		    curwin->w_p_fdl = p_fdls;
# endif
		// When getting the ATTENTION prompt here, use a dialog
		ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_DIALOG;

		ਸੈੱਟ_buflisted(TRUE);

		// create memfile, read file
		(ਖਾਲੀ)open_ਬਫਰ(FALSE, ਨਿਰਾ, 0);

		ਜੇ (ਅਦਲਾ_ਬਦਲੀ_exists_action == SEA_QUIT)
		{
		    ਜੇ (got_int || only_one_ਖਿੜਕੀ())
		    {
			// abort selected or quit and only one window
			did_emsg = FALSE;   // avoid hit-enter prompt
			getout(1);
		    }
		    // We can't close the window, it would disturb what
		    // happens next.  Clear the file name and set the arg
		    // index to -1 to delete it later.
		    setfname(curbuf, ਨਿਰਾ, ਨਿਰਾ, FALSE);
		    curwin->w_arg_idx = -1;
		    ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_NONE;
		}
		ਨਹੀਂਤਾਂ
		    handle_ਅਦਲਾ_ਬਦਲੀ_exists(ਨਿਰਾ);
		dorewind = TRUE;		// start again
	    }
	    ui_breakcheck();
	    ਜੇ (got_int)
	    {
		(ਖਾਲੀ)vgetc();	// only break the file loading, not the rest
		ਰੋਕੋ;
	    }
	}
	ਜੇ (parmp->ਖਿੜਕੀ_layout == WIN_TABS)
	    goto_tabpage(1);
	ਨਹੀਂਤਾਂ
	    curwin = firstwin;
	curbuf = curwin->w_ਬਫਰ;
	--autocmd_no_enter;
	--autocmd_no_leave;
    }
}

/*
 * If opened more than one window, start editing files in the other
 * windows.  make_windows() has already opened the windows.
 */
    ਸਥਿਰ ਖਾਲੀ
edit_buffers(
    mparm_T	*parmp,
    char_u	*cwd)			// current working dir
{
    ਪੂਰਨ		arg_idx;		// index in argument list
    ਪੂਰਨ		i;
    ਪੂਰਨ		advance = TRUE;
    win_T	*win;
    char_u	*p_shm_save = ਨਿਰਾ;

    /*
     * Don't execute Win/Buf Enter/Leave autocommands here
     */
    ++autocmd_no_enter;
    ++autocmd_no_leave;

    // When w_arg_idx is -1 remove the window (see create_windows()).
    ਜੇ (curwin->w_arg_idx == -1)
    {
	win_close(curwin, TRUE);
	advance = FALSE;
    }

    arg_idx = 1;
    ਲਈ (i = 1; i < parmp->ਖਿੜਕੀ_ਗਿਣਤੀ; ++i)
    {
	ਜੇ (cwd != ਨਿਰਾ)
	    mch_chdir((ਅੱਖਰ *)cwd);
	// When w_arg_idx is -1 remove the window (see create_windows()).
	ਜੇ (curwin->w_arg_idx == -1)
	{
	    ++arg_idx;
	    win_close(curwin, TRUE);
	    advance = FALSE;
	    ਜਾਰੀ;
	}

	ਜੇ (advance)
	{
	    ਜੇ (parmp->ਖਿੜਕੀ_layout == WIN_TABS)
	    {
		ਜੇ (curtab->tp_ਅਗਲਾ == ਨਿਰਾ)	// just checking
		    ਰੋਕੋ;
		goto_tabpage(0);
		// Temporarily reset 'shm' option to not print fileinfo when
		// loading the other buffers. This would overwrite the already
		// existing fileinfo for the first tab.
		ਜੇ (i == 1)
		{
		    ਅੱਖਰ buf[100];

		    p_shm_save = vim_strsave(p_shm);
		    vim_snprintf(buf, 100, "F%s", p_shm);
		    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"shm",
							 0L, (char_u *)buf, 0);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (curwin->w_ਅਗਲਾ == ਨਿਰਾ)	// just checking
		    ਰੋਕੋ;
		win_enter(curwin->w_ਅਗਲਾ, FALSE);
	    }
	}
	advance = TRUE;

	// Only open the file if there is no file in this window yet (that can
	// happen when .vimrc contains ":sall").
	ਜੇ (curbuf == firstwin->w_ਬਫਰ || curbuf->b_ffname == ਨਿਰਾ)
	{
	    curwin->w_arg_idx = arg_idx;
	    // Edit file from arg list, if there is one.  When "Quit" selected
	    // at the ATTENTION prompt close the window.
	    ਅਦਲਾ_ਬਦਲੀ_exists_did_quit = FALSE;
	    (ਖਾਲੀ)do_ecmd(0, arg_idx < GARGCOUNT
			  ? alist_name(&GARGLIST[arg_idx]) : ਨਿਰਾ,
			  ਨਿਰਾ, ਨਿਰਾ, ECMD_LASTL, ECMD_HIDE, curwin);
	    ਜੇ (ਅਦਲਾ_ਬਦਲੀ_exists_did_quit)
	    {
		// abort or quit selected
		ਜੇ (got_int || only_one_ਖਿੜਕੀ())
		{
		    // abort selected and only one window
		    did_emsg = FALSE;   // avoid hit-enter prompt
		    getout(1);
		}
		win_close(curwin, TRUE);
		advance = FALSE;
	    }
	    ਜੇ (arg_idx == GARGCOUNT - 1)
		arg_had_ਆਖਰੀ = TRUE;
	    ++arg_idx;
	}
	ui_breakcheck();
	ਜੇ (got_int)
	{
	    (ਖਾਲੀ)vgetc();	// only break the file loading, not the rest
	    ਰੋਕੋ;
	}
    }

    ਜੇ (p_shm_save != ਨਿਰਾ)
    {
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"shm", 0L, p_shm_save, 0);
	vim_free(p_shm_save);
    }

    ਜੇ (parmp->ਖਿੜਕੀ_layout == WIN_TABS)
	goto_tabpage(1);
    --autocmd_no_enter;

    // make the first window the current window
    win = firstwin;
# ਜੇ defined(FEAT_QUICKFIX)
    // Avoid making a preview window the current window.
    ਜਦੋਂ (win->w_p_pvw)
    {
	win = win->w_ਅਗਲਾ;
	ਜੇ (win == ਨਿਰਾ)
	{
	    win = firstwin;
	    ਰੋਕੋ;
	}
    }
# endif
    win_enter(win, FALSE);

    --autocmd_no_leave;
    ਸਮਾਂ_ਸੁਨੇਹਾ("editing files in windows");
    ਜੇ (parmp->ਖਿੜਕੀ_ਗਿਣਤੀ > 1 && parmp->ਖਿੜਕੀ_layout != WIN_TABS)
	win_equal(curwin, FALSE, 'b');	// adjust heights
}

/*
 * Execute the commands from --cmd arguments "cmds[cnt]".
 */
    ਸਥਿਰ ਖਾਲੀ
exe_pre_commands(mparm_T *parmp)
{
    char_u	**cmds = parmp->pre_commands;
    ਪੂਰਨ		cnt = parmp->n_pre_commands;
    ਪੂਰਨ		i;
    ESTACK_CHECK_DECLARATION;

    ਜੇ (cnt <= 0)
	ਵਾਪਸ;

    curwin->w_cursor.lnum = 0; // just in case..
    estack_ਧੱਕੋ(ETYPE_ਦਲੀਲਾਂ, (char_u *)_("pre-vimrc command line"), 0);
    ESTACK_CHECK_ਤਿਆਰੀ;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CMDARG;
# endif
    ਲਈ (i = 0; i < cnt; ++i)
	do_cmdline_cmd(cmds[i]);
    ESTACK_CHECK_NOW;
    estack_ਕੱਢੋ();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    ਸਮਾਂ_ਸੁਨੇਹਾ("--cmd commands");
}

/*
 * Execute "+", "-c" and "-S" arguments.
 */
    ਸਥਿਰ ਖਾਲੀ
exe_commands(mparm_T *parmp)
{
    ਪੂਰਨ		i;
    ESTACK_CHECK_DECLARATION;

    /*
     * We start commands on line 0, make "vim +/pat file" match a
     * pattern on line 1.  But don't move the cursor when an autocommand
     * with g`" was used.
     */
    ਸੁਨੇਹਾ_scroll = TRUE;
    ਜੇ (parmp->tagname == ਨਿਰਾ && curwin->w_cursor.lnum <= 1)
	curwin->w_cursor.lnum = 0;
    estack_ਧੱਕੋ(ETYPE_ਦਲੀਲਾਂ, (char_u *)"command line", 0);
    ESTACK_CHECK_ਤਿਆਰੀ;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CARG;
    current_sctx.sc_seq = 0;
# endif
    ਲਈ (i = 0; i < parmp->n_commands; ++i)
    {
	do_cmdline_cmd(parmp->commands[i]);
	ਜੇ (parmp->cmds_tofree[i])
	    vim_free(parmp->commands[i]);
    }
    ESTACK_CHECK_NOW;
    estack_ਕੱਢੋ();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    ਜੇ (curwin->w_cursor.lnum == 0)
	curwin->w_cursor.lnum = 1;

    ਜੇ (!exmode_ਸਰਗਰਮ)
	ਸੁਨੇਹਾ_scroll = FALSE;

# ifdef FEAT_QUICKFIX
    // When started with "-q errorfile" jump to first error again.
    ਜੇ (parmp->edit_type == EDIT_QF)
	qf_jump(ਨਿਰਾ, 0, 0, FALSE);
# endif
    ਸਮਾਂ_ਸੁਨੇਹਾ("executing command arguments");
}

/*
 * Source startup scripts.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸਰੋਤ_startup_scripts(mparm_T *parmp)
{
    ਪੂਰਨ		i;

    /*
     * For "evim" source evim.vim first of all, so that the user can overrule
     * any things he doesn't like.
     */
    ਜੇ (parmp->evim_ਬਹੁਲਕ)
    {
	(ਖਾਲੀ)do_ਸਰੋਤ((char_u *)EVIM_FILE, FALSE, DOSO_NONE, ਨਿਰਾ);
	ਸਮਾਂ_ਸੁਨੇਹਾ("source evim file");
    }

    /*
     * If -u argument given, use only the initializations from that file and
     * nothing else.
     */
    ਜੇ (parmp->use_vimrc != ਨਿਰਾ)
    {
	ਜੇ (STRCMP(parmp->use_vimrc, "DEFAULTS") == 0)
	{
	    ਜੇ (do_ਸਰੋਤ((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE, ਨਿਰਾ)
									 != OK)
		emsg(_(e_failed_ਮੰਜ਼ਿਲ_ਸਰੋਤ_defaults));
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(parmp->use_vimrc, "NONE") == 0
				     || STRCMP(parmp->use_vimrc, "NORC") == 0)
	{
# ifdef FEAT_GUI
	    ਜੇ (use_gvimrc == ਨਿਰਾ)	    // don't load gvimrc either
		use_gvimrc = parmp->use_vimrc;
# endif
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (do_ਸਰੋਤ(parmp->use_vimrc, FALSE, DOSO_NONE, ਨਿਰਾ) != OK)
		semsg(_(e_cannot_read_from_str_2), parmp->use_vimrc);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (!silent_ਬਹੁਲਕ)
    {
# ifdef AMIGA
	ਬਣਤਰ Process	*proc = (ਬਣਤਰ Process *)ਲੱਭੋ_Task(0L);
	APTR		save_winptr = proc->pr_ਖਿੜਕੀ_Ptr;

	// Avoid a requester here for a volume that doesn't exist.
	proc->pr_ਖਿੜਕੀ_Ptr = (APTR)-1L;
# endif

	/*
	 * Get system wide defaults, if the file name is defined.
	 */
# ifdef SYS_VIMRC_FILE
	(ਖਾਲੀ)do_ਸਰੋਤ((char_u *)SYS_VIMRC_FILE, FALSE, DOSO_NONE, ਨਿਰਾ);
# endif
# ifdef MACOS_X
	(ਖਾਲੀ)do_ਸਰੋਤ((char_u *)"$VIMRUNTIME/macmap.vim", FALSE,
							      DOSO_NONE, ਨਿਰਾ);
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
	ਜੇ (process_env((char_u *)"VIMINIT", TRUE) != OK)
	{
	    ਜੇ (do_ਸਰੋਤ((char_u *)USR_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, ਨਿਰਾ) == FAIL
# ifdef USR_VIMRC_FILE2
		&& do_ਸਰੋਤ((char_u *)USR_VIMRC_FILE2, TRUE,
						      DOSO_VIMRC, ਨਿਰਾ) == FAIL
# endif
# ifdef XDG_VIMRC_FILE
		&& do_ਸਰੋਤ((char_u *)XDG_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, ਨਿਰਾ) == FAIL
# endif
# ifdef USR_VIMRC_FILE3
		&& do_ਸਰੋਤ((char_u *)USR_VIMRC_FILE3, TRUE,
						      DOSO_VIMRC, ਨਿਰਾ) == FAIL
# endif
# ifdef USR_VIMRC_FILE4
		&& do_ਸਰੋਤ((char_u *)USR_VIMRC_FILE4, TRUE,
						      DOSO_VIMRC, ਨਿਰਾ) == FAIL
# endif
		&& process_env((char_u *)"EXINIT", FALSE) == FAIL
		&& do_ਸਰੋਤ((char_u *)USR_EXRC_FILE, FALSE,
						       DOSO_NONE, ਨਿਰਾ) == FAIL
# ifdef USR_EXRC_FILE2
		&& do_ਸਰੋਤ((char_u *)USR_EXRC_FILE2, FALSE,
						       DOSO_NONE, ਨਿਰਾ) == FAIL
# endif
		&& !has_dash_c_arg)
	    {
		// When no .vimrc file was found: source defaults.vim.
		ਜੇ (do_ਸਰੋਤ((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE,
								 ਨਿਰਾ) == FAIL)
		    emsg(_(e_failed_ਮੰਜ਼ਿਲ_ਸਰੋਤ_defaults));
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
	ਜੇ (p_exrc)
	{
# ਜੇ defined(UNIX) || defined(VMS)
	    // If ".vimrc" file is not owned by user, set 'secure' mode.
	    ਜੇ (!file_owned(VIMRC_FILE))
# endif
		secure = p_secure;

	    i = FAIL;
	    ਜੇ (fullpathcmp((char_u *)USR_VIMRC_FILE,
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
		i = do_ਸਰੋਤ((char_u *)VIMRC_FILE, TRUE, DOSO_VIMRC, ਨਿਰਾ);

	    ਜੇ (i == FAIL)
	    {
# ਜੇ defined(UNIX) || defined(VMS)
		// if ".exrc" is not owned by user set 'secure' mode
		ਜੇ (!file_owned(EXRC_FILE))
		    secure = p_secure;
		ਨਹੀਂਤਾਂ
		    secure = 0;
# endif
		ਜੇ (	   fullpathcmp((char_u *)USR_EXRC_FILE,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# ifdef USR_EXRC_FILE2
			&& fullpathcmp((char_u *)USR_EXRC_FILE2,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# endif
				)
		    (ਖਾਲੀ)do_ਸਰੋਤ((char_u *)EXRC_FILE, FALSE,
							      DOSO_NONE, ਨਿਰਾ);
	    }
	}
	ਜੇ (secure == 2)
	    need_wait_return = TRUE;
	secure = 0;
# ifdef AMIGA
	proc->pr_ਖਿੜਕੀ_Ptr = save_winptr;
# endif
    }
    ਸਮਾਂ_ਸੁਨੇਹਾ("sourcing vimrc file(s)");
}

/*
 * Setup to start using the GUI.  Exit with an error when not available.
 */
    ਸਥਿਰ ਖਾਲੀ
main_ਸ਼ੁਰੂ_gui(ਖਾਲੀ)
{
# ifdef FEAT_GUI
    gui.starting = true;	// start GUI a bit later
# ਨਹੀਂਤਾਂ
    mch_errmsg(_(e_gui_cannot_be_ਵਰਤਿਆ_not_enabled_at_compile_ਸਮਾਂ));
    mch_errmsg("\n");
    mch_exit(2);
# endif
}

#ਜੇ_ਅੰਤ  // NO_VIM_MAIN

/*
 * Get an environment variable and execute it as Ex commands.
 * Returns FAIL if the environment variable was not executed, OK otherwise.
 */
    ਪੂਰਨ
process_env(
    char_u	*env,
    ਪੂਰਨ		is_viminit) // when TRUE, called for VIMINIT
{
    char_u	*initstr;
    sctx_T	save_current_sctx;
    ESTACK_CHECK_DECLARATION;

    ਜੇ ((initstr = mch_getenv(env)) == ਨਿਰਾ || *initstr == NUL)
	ਵਾਪਸ FAIL;

    ਜੇ (is_viminit)
	vimrc_ਮਿਲਿਆ(ਨਿਰਾ, ਨਿਰਾ);
    estack_ਧੱਕੋ(ETYPE_ENV, env, 0);
    ESTACK_CHECK_ਤਿਆਰੀ;
    save_current_sctx = current_sctx;
    current_sctx.sc_ਸੰਸਕਰਣ = 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    current_sctx.sc_sid = SID_ENV;
    current_sctx.sc_seq = 0;
    current_sctx.sc_lnum = 0;
#ਜੇ_ਅੰਤ

    do_cmdline_cmd(initstr);

    ESTACK_CHECK_NOW;
    estack_ਕੱਢੋ();
    current_sctx = save_current_sctx;
    ਵਾਪਸ OK;
}

#ਜੇ (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
/*
 * Return TRUE if we are certain the user owns the file "fname".
 * Used for ".vimrc" and ".exrc".
 * Use both stat() and lstat() for extra security.
 */
    ਸਥਿਰ ਪੂਰਨ
file_owned(ਅੱਖਰ *fname)
{
    stat_T	s;
# ifdef UNIX
    uid_t	uid = getuid();
# ਨਹੀਂਤਾਂ	 // VMS
    uid_t	uid = ((getgid() << 16) | getuid());
# endif

    ਵਾਪਸ !(mch_stat(fname, &s) != 0 || s.st_uid != uid
# ifdef HAVE_LSTAT
	    || mch_lstat(fname, &s) != 0 || s.st_uid != uid
# endif
	    );
}
#ਜੇ_ਅੰਤ

/*
 * Give an error message main_errors["n"] and exit.
 */
    ਸਥਿਰ ਖਾਲੀ
mainerr(
    ਪੂਰਨ		n,	// one of the ME_ defines
    char_u	*str)	// extra argument or NULL
{
#ਜੇ defined(UNIX) || defined(VMS)
    reset_ਸੰਕੇਤ();		// kill us with CTRL-C here, if you like
#ਜੇ_ਅੰਤ

    // If this is a Windows GUI executable, show an error dialog box.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VIMDLL
    gui.ਵਿੱਚ_use = mch_is_gui_executable();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    gui.starting = false;   // Needed to show as error.
#ਜੇ_ਅੰਤ

    init_long_ਸੰਸਕਰਣ();
    mch_errmsg(long_ਸੰਸਕਰਣ);
    mch_errmsg("\n");
    mch_errmsg(_(main_errors[n]));
    ਜੇ (str != ਨਿਰਾ)
    {
	mch_errmsg(": \"");
	mch_errmsg((ਅੱਖਰ *)str);
	mch_errmsg("\"");
    }
    mch_errmsg(_("\nMore info with: \"vim -h\"\n"));

    mch_exit(1);
}

    ਖਾਲੀ
mainerr_arg_missing(char_u *str)
{
    mainerr(ME_ARG_MISSING, str);
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NO_VIM_MAIN
/*
 * print a message with three spaces prepended and '\n' appended.
 */
    ਸਥਿਰ ਖਾਲੀ
main_ਸੁਨੇਹਾ(ਅੱਖਰ *s)
{
    mch_ਸੁਨੇਹਾ("   ");
    mch_ਸੁਨੇਹਾ(s);
    mch_ਸੁਨੇਹਾ("\n");
}

/*
 * Print messages for "vim -h" or "vim --help" and exit.
 */
    ਸਥਿਰ ਖਾਲੀ
usage(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    ਸਥਿਰ ਅੱਖਰ	*(use[]) =
    {
	N_("[file ..]       edit specified file(s)"),
	N_("-               read text from stdin"),
	N_("-t tag          edit file where tag is defined"),
# ifdef FEAT_QUICKFIX
	N_("-q [errorfile]  edit file with first error")
# endif
    };

# ਜੇ defined(UNIX) || defined(VMS)
    reset_ਸੰਕੇਤ();		// kill us with CTRL-C here, if you like
# endif

    init_long_ਸੰਸਕਰਣ();
    mch_ਸੁਨੇਹਾ(long_ਸੰਸਕਰਣ);
    mch_ਸੁਨੇਹਾ(_("\n\nUsage:"));
    ਲਈ (i = 0; ; ++i)
    {
	mch_ਸੁਨੇਹਾ(_(" vim [arguments] "));
	mch_ਸੁਨੇਹਾ(_(use[i]));
	ਜੇ (i == ARRAY_ਲੰਮਾਈ(use) - 1)
	    ਰੋਕੋ;
	mch_ਸੁਨੇਹਾ(_("\n   or:"));
    }
# ਜੇ defined( VMS)
    mch_ਸੁਨੇਹਾ(_("\nWhere command is down-cased, prepend / (like: -/R) to treat flag as upper-case."));
    mch_ਸੁਨੇਹਾ(_("\nOr, where supported, SET PROC/PARSE=EXT, or else quote upper-case material."));
# endif /* defined( VMS) */

    mch_ਸੁਨੇਹਾ(_("\n\nArguments:\n"));
    main_ਸੁਨੇਹਾ(_("--\t\t\tOnly file names after this"));
# ifdef EXPAND_FILENAMES
    main_ਸੁਨੇਹਾ(_("--literal\t\tDon't expand wildcards"));
# endif
# ifdef FEAT_OLE
    main_ਸੁਨੇਹਾ(_("-register\t\tRegister this gvim for OLE"));
    main_ਸੁਨੇਹਾ(_("-unregister\t\tUnregister gvim for OLE"));
# endif
# ifdef FEAT_GUI
    main_ਸੁਨੇਹਾ(_("-g\t\t\tRun using GUI (like \"gvim\")"));
    main_ਸੁਨੇਹਾ(_("-f  or  --nofork\tForeground: Don't fork when starting GUI"));
# endif
    main_ਸੁਨੇਹਾ(_("-v\t\t\tVi mode (like \"vi\")"));
    main_ਸੁਨੇਹਾ(_("-e\t\t\tEx mode (like \"ex\")"));
    main_ਸੁਨੇਹਾ(_("-E\t\t\tImproved Ex mode"));
    main_ਸੁਨੇਹਾ(_("-s\t\t\tSilent (batch) mode (only for \"ex\")"));
# ifdef FEAT_DIFF
    main_ਸੁਨੇਹਾ(_("-d\t\t\tDiff mode (like \"vimdiff\")"));
# endif
    main_ਸੁਨੇਹਾ(_("-y\t\t\tEasy mode (like \"evim\", modeless)"));
    main_ਸੁਨੇਹਾ(_("-R\t\t\tReadonly mode (like \"view\")"));
    main_ਸੁਨੇਹਾ(_("-Z\t\t\tRestricted mode (like \"rvim\")"));
    main_ਸੁਨੇਹਾ(_("-m\t\t\tModifications (writing files) not allowed"));
    main_ਸੁਨੇਹਾ(_("-M\t\t\tModifications in text not allowed"));
    main_ਸੁਨੇਹਾ(_("-b\t\t\tBinary mode"));
    main_ਸੁਨੇਹਾ(_("-l\t\t\tLisp mode"));
    main_ਸੁਨੇਹਾ(_("-C\t\t\tCompatible with Vi: 'compatible'"));
    main_ਸੁਨੇਹਾ(_("-N\t\t\tNot fully Vi compatible: 'nocompatible'"));
    main_ਸੁਨੇਹਾ(_("-V[N][fname]\t\tBe verbose [level N] [log messages to fname]"));
# ifdef FEAT_EVAL
    main_ਸੁਨੇਹਾ(_("-D\t\t\tDebugging mode"));
# endif
    main_ਸੁਨੇਹਾ(_("-n\t\t\tNo swap file, use memory only"));
    main_ਸੁਨੇਹਾ(_("-r\t\t\tList swap files and exit"));
    main_ਸੁਨੇਹਾ(_("-r (with file name)\tRecover crashed session"));
    main_ਸੁਨੇਹਾ(_("-L\t\t\tSame as -r"));
# ifdef AMIGA
    main_ਸੁਨੇਹਾ(_("-f\t\t\tDon't use newcli to open window"));
    main_ਸੁਨੇਹਾ(_("-dev <device>\t\tUse <device> for I/O"));
# endif
# ifdef FEAT_ARABIC
    main_ਸੁਨੇਹਾ(_("-A\t\t\tStart in Arabic mode"));
# endif
# ifdef FEAT_RIGHTLEFT
    main_ਸੁਨੇਹਾ(_("-H\t\t\tStart in Hebrew mode"));
# endif
    main_ਸੁਨੇਹਾ(_("-T <terminal>\tSet terminal type to <terminal>"));
    main_ਸੁਨੇਹਾ(_("--not-a-term\t\tSkip warning for input/output not being a terminal"));
# ifdef FEAT_GUI
    main_ਸੁਨੇਹਾ(_("--gui-dialog-file {fname}  For testing: write dialog text"));
# endif
    main_ਸੁਨੇਹਾ(_("--ttyfail\t\tExit if input or output is not a terminal"));
# ifdef FEAT_ਕੰਮ_CHANNEL
    main_ਸੁਨੇਹਾ(_("--stdio-channel\tUse stdin and stdout as a channel"));
# endif
    main_ਸੁਨੇਹਾ(_("-u <vimrc>\t\tUse <vimrc> instead of any .vimrc"));
# ifdef FEAT_GUI
    main_ਸੁਨੇਹਾ(_("-U <gvimrc>\t\tUse <gvimrc> instead of any .gvimrc"));
# endif
    main_ਸੁਨੇਹਾ(_("--noplugin\t\tDon't load plugin scripts"));
    main_ਸੁਨੇਹਾ(_("-p[N]\t\tOpen N tab pages (default: one for each file)"));
    main_ਸੁਨੇਹਾ(_("-o[N]\t\tOpen N windows (default: one for each file)"));
    main_ਸੁਨੇਹਾ(_("-O[N]\t\tLike -o but split vertically"));
    main_ਸੁਨੇਹਾ(_("+\t\t\tStart at end of file"));
    main_ਸੁਨੇਹਾ(_("+<lnum>\t\tStart at line <lnum>"));
    main_ਸੁਨੇਹਾ(_("--cmd <command>\tExecute <command> before loading any vimrc file"));
    main_ਸੁਨੇਹਾ(_("-c <command>\t\tExecute <command> after loading the first file"));
    main_ਸੁਨੇਹਾ(_("-S <session>\t\tSource file <session> after loading the first file"));
    main_ਸੁਨੇਹਾ(_("-s <scriptin>\tRead Normal mode commands from file <scriptin>"));
    main_ਸੁਨੇਹਾ(_("-w <scriptout>\tAppend all typed commands to file <scriptout>"));
    main_ਸੁਨੇਹਾ(_("-W <scriptout>\tWrite all typed commands to file <scriptout>"));
# ifdef FEAT_CRYPT
    main_ਸੁਨੇਹਾ(_("-x\t\t\tEdit encrypted files"));
# endif
# ਜੇ (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
#  ਜੇ defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
    main_ਸੁਨੇਹਾ(_("-display <display>\tConnect Vim to this particular X-server"));
#  endif
    main_ਸੁਨੇਹਾ(_("-X\t\t\tDo not connect to X server"));
# endif
# ਜੇ defined(FEAT_WAYLAND)
    main_ਸੁਨੇਹਾ(_("-Y\t\t\tDo not connect to Wayland compositor"));
# endif
# ifdef FEAT_CLIENTSERVER
#  ifdef FEAT_CLIENTSERVER_BACKENDS
    main_ਸੁਨੇਹਾ(_("--clientserver <socket|x11|mswin> Backend for clientserver communication"));
#  endif
    main_ਸੁਨੇਹਾ(_("--remote <files>\tEdit <files> in a Vim server if possible"));
    main_ਸੁਨੇਹਾ(_("--remote-silent <files>  Same, don't complain if there is no server"));
    main_ਸੁਨੇਹਾ(_("--remote-wait <files>  As --remote but wait for files to have been edited"));
    main_ਸੁਨੇਹਾ(_("--remote-wait-silent <files>  Same, don't complain if there is no server"));
    main_ਸੁਨੇਹਾ(_("--remote-tab[-wait][-silent] <files>  As --remote but use tab page per file"));
    main_ਸੁਨੇਹਾ(_("--remote-send <keys>\tSend <keys> to a Vim server and exit"));
    main_ਸੁਨੇਹਾ(_("--remote-expr <expr>\tEvaluate <expr> in a Vim server and print result"));
    main_ਸੁਨੇਹਾ(_("--serverlist\t\tList available Vim server names and exit"));
    main_ਸੁਨੇਹਾ(_("--servername <name>\tSend to/become the Vim server <name>"));
# endif
# ifdef STARTUPTIME
    main_ਸੁਨੇਹਾ(_("--startuptime <file>\tWrite startup timing messages to <file>"));
# endif
# ifdef FEAT_ਕੰਮ_CHANNEL
    main_ਸੁਨੇਹਾ(_("--log <file>\t\tStart logging to <file> early"));
# endif
# ifdef FEAT_VIMINFO
    main_ਸੁਨੇਹਾ(_("-i <viminfo>\t\tUse <viminfo> instead of .viminfo"));
# endif
    main_ਸੁਨੇਹਾ(_("--clean\t\t'nocompatible', Vim defaults, no plugins, no viminfo"));
    main_ਸੁਨੇਹਾ(_("-h  or  --help\tPrint Help (this message) and exit"));
    main_ਸੁਨੇਹਾ(_("--version\t\tPrint version information and exit"));

# ifdef FEAT_GUI_X11
#  ifdef FEAT_GUI_MOTIF
    mch_ਸੁਨੇਹਾ(_("\nArguments recognised by gvim (Motif version):\n"));
#  endif
    main_ਸੁਨੇਹਾ(_("-display <display>\tRun Vim on <display>"));
    main_ਸੁਨੇਹਾ(_("-iconic\t\tStart Vim iconified"));
    main_ਸੁਨੇਹਾ(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_ਸੁਨੇਹਾ(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_ਸੁਨੇਹਾ(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_ਸੁਨੇਹਾ(_("-boldfont <font>\tUse <font> for bold text"));
    main_ਸੁਨੇਹਾ(_("-italicfont <font>\tUse <font> for italic text"));
    main_ਸੁਨੇਹਾ(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
    main_ਸੁਨੇਹਾ(_("-borderwidth <width>\tUse a border width of <width> (also: -bw)"));
    main_ਸੁਨੇਹਾ(_("-scrollbarwidth <width>  Use a scrollbar width of <width> (also: -sw)"));
    main_ਸੁਨੇਹਾ(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_ਸੁਨੇਹਾ(_("+reverse\t\tDon't use reverse video (also: +rv)"));
    main_ਸੁਨੇਹਾ(_("-xrm <resource>\tSet the specified resource"));
# endif // FEAT_GUI_X11
# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
    mch_ਸੁਨੇਹਾ(_("\nArguments recognised by gvim (GTK4 version):\n"));
#  ਨਹੀਂਤਾਂ
    mch_ਸੁਨੇਹਾ(_("\nArguments recognised by gvim (GTK+ version):\n"));
#  endif
    main_ਸੁਨੇਹਾ(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_ਸੁਨੇਹਾ(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_ਸੁਨੇਹਾ(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_ਸੁਨੇਹਾ(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
#  ifdef USE_GTK4
    main_ਸੁਨੇਹਾ("--prg-name <name>\tSet GTK program name");
#  ਨਹੀਂਤਾਂ
    main_ਸੁਨੇਹਾ(_("-iconic\t\tStart Vim iconified"));
    main_ਸੁਨੇਹਾ(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_ਸੁਨੇਹਾ(_("-display <display>\tRun Vim on <display> (also: --display)"));
    main_ਸੁਨੇਹਾ(_("--role <role>\tSet a unique role to identify the main window"));
    main_ਸੁਨੇਹਾ(_("--socketid <xid>\tOpen Vim inside another GTK widget"));
    main_ਸੁਨੇਹਾ(_("--echo-wid\t\tMake gvim echo the Window ID on stdout"));
#  endif
# endif
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
    ਜੇ (gui.starting)
#  endif
    {
	main_ਸੁਨੇਹਾ(_("-P <parent title>\tOpen Vim inside parent application"));
	main_ਸੁਨੇਹਾ(_("--windowid <HWND>\tOpen Vim inside another win32 widget"));
    }
# endif

# ifdef FEAT_GUI_GNOME
    // Gnome gives extra messages for --help if we continue, but not for -h.
    ਜੇ (gui.starting)
    {
	mch_ਸੁਨੇਹਾ("\n");
	gui.dofork = false;
    }
    ਨਹੀਂਤਾਂ
# endif
	mch_exit(0);
}

/*
 * Check the result of the ATTENTION dialog:
 * When "Quit" selected, exit Vim.
 * When "Recover" selected, recover the file.
 */
    ਸਥਿਰ ਖਾਲੀ
check_ਅਦਲਾ_ਬਦਲੀ_exists_action(ਖਾਲੀ)
{
    ਜੇ (ਅਦਲਾ_ਬਦਲੀ_exists_action == SEA_QUIT)
	getout(1);
    handle_ਅਦਲਾ_ਬਦਲੀ_exists(ਨਿਰਾ);
}

#ਜੇ_ਅੰਤ // NO_VIM_MAIN

#ਜੇ !defined(NO_VIM_MAIN) && defined(FEAT_EVAL)
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_progpath(char_u *argv0)
{
    char_u *val = argv0;

# ifdef MSWIN
    // A relative path containing a "/" will become invalid when using ":cd",
    // turn it into a full path.
    // On MS-Windows "vim" should be expanded to "vim.exe", thus always do
    // this.
    char_u *ਰਸਤਾ = ਨਿਰਾ;

    ਜੇ (mch_can_exe(argv0, &ਰਸਤਾ, FALSE) && ਰਸਤਾ != ਨਿਰਾ)
	val = ਰਸਤਾ;
# ਨਹੀਂਤਾਂ
    char_u	buf[MAXPATHL + 1];
#  ifdef PROC_EXE_LINK
    ਅੱਖਰ	linkbuf[MAXPATHL + 1];
    ssize_t	len;

    len = readlink(PROC_EXE_LINK, linkbuf, MAXPATHL);
    ਜੇ (len > 0)
    {
	linkbuf[len] = NUL;
	val = (char_u *)linkbuf;
    }
#  endif

    ਜੇ (!mch_is_ਭਰਿਆ_Name(val))
    {
	ਜੇ (gettail(val) != val
			   && vim_ਭਰਿਆ_Name(val, buf, MAXPATHL, TRUE) != FAIL)
	    val = buf;
    }
# endif

    ਸੈੱਟ_vim_var_string(VV_PROGPATH, val, -1);

# ifdef MSWIN
    vim_free(ਰਸਤਾ);
# endif
}

#ਜੇ_ਅੰਤ // NO_VIM_MAIN
