/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#define EXTERN
#अन्तर्भाव "vim.h"

#ifdef __CYGWIN__
# include <cygwin/version.h>
# include <sys/cygwin.h>	// for cygwin_conv_to_posix_path() and/or
				// cygwin_conv_path()
# include <limits.h>
#endif

#जर defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# include "iscygpty.h"
#endif

// Values for edit_type.
#define EDIT_NONE   0	    // no edit type yet
#define EDIT_FILE   1	    // file name argument[s] given, use argument list
#define EDIT_STDIN  2	    // read file from stdin
#define EDIT_TAG    3	    // tag name argument given, use tagname
#define EDIT_QF	    4	    // start in quickfix mode

#जर (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
स्थिर पूर्णांक file_owned(अक्षर *fname);
#endif
स्थिर शून्य mainerr(पूर्णांक, char_u *);
स्थिर शून्य early_arg_scan(mparm_T *parmp);
#ifndef NO_VIM_MAIN
स्थिर शून्य usage(शून्य);
स्थिर शून्य parse_command_name(mparm_T *parmp);
स्थिर शून्य command_ओळ_scan(mparm_T *parmp);
स्थिर शून्य check_tty(mparm_T *parmp);
स्थिर शून्य read_stdin(शून्य);
स्थिर शून्य create_windows(mparm_T *parmp);
स्थिर शून्य edit_buffers(mparm_T *parmp, char_u *cwd);
स्थिर शून्य exe_pre_commands(mparm_T *parmp);
स्थिर शून्य exe_commands(mparm_T *parmp);
स्थिर शून्य source_startup_scripts(mparm_T *parmp);
स्थिर शून्य main_start_gui(शून्य);
स्थिर शून्य check_swap_exists_action(शून्य);
# ifdef FEAT_EVAL
स्थिर शून्य set_progpath(char_u *argv0);
# endif
#endif


/*
 * Different types of error messages.
 */
स्थिर अक्षर *(main_errors[]) =
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
स्थिर mparm_T	params;

# ifdef _IOLBF
स्थिर शून्य *s_vbuf = रिक्त;		// buffer for setvbuf()
# endif

# ifndef NO_VIM_MAIN	// skip this for unittests

स्थिर char_u *start_dir = रिक्त;	// current working dir on startup

स्थिर पूर्णांक has_dash_c_arg = FALSE;

#  ifdef VIMDLL
__declspec(dllexport)
#  endif
    पूर्णांक
#  ifdef MSWIN
VimMain
#  अन्यथा
main
#  endif
(पूर्णांक argc, अक्षर **argv)
{
#  जर defined(STARTUPTIME) || defined(CLEAN_RUNTIMEPATH)
    पूर्णांक		i;
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
    params.window_मोजणी = -1;

    autocmd_init();

#  ifdef FEAT_RUBY
    {
	पूर्णांक ruby_चळत_start;
	vim_ruby_init((शून्य *)&ruby_चळत_start);
    }
#  endif

#  ifdef FEAT_TCL
    vim_tcl_init(params.argv[0]);
#  endif

#  ifdef MEM_PROFILE
    निकास_पर(vim_mem_profile_dump);
#  endif

    /*
     * Various initialisations #1 shared with tests.
     */
    common_init_1();

#  जर defined(STARTUPTIME) || defined(FEAT_JOB_CHANNEL)
    // Need to find "--startuptime" and "--log" before actually parsing
    // arguments.
    साठी (i = 1; i < argc - 1; ++i)
    {
#   ifdef STARTUPTIME
	जर (STRICMP(argv[i], "--startuptime") == 0 && time_fd == रिक्त)
	{
	    time_fd = mch_fopen(argv[i + 1], "a");
	    TIME_संदेश("--- VIM STARTING ---");
	}
#   endif
#   ifdef FEAT_EVAL
	जर (STRICMP(argv[i], "--log") == 0)
	    ch_logfile((char_u *)(argv[i + 1]), (char_u *)"ao");
#   endif
    }
#  endif

#  ifdef CLEAN_RUNTIMEPATH
    // Need to find "--clean" before actually parsing arguments.
    साठी (i = 1; i < argc; ++i)
	जर (STRICMP(argv[i], "--clean") == 0)
	{
	    params.clean = TRUE;
	    थांबा;
	}
#  endif
#  ifdef MSWIN
    // Need to find "-register" and "-unregister" before loading any libraries.
    साठी (i = 1; i < argc; ++i)
	जर ((STRICMP(argv[i] + 1, "register") == 0
				    || STRICMP(argv[i] + 1, "unregister") == 0)
		&& (argv[i][0] == '-' || argv[i][0] == '/'))
	{
	    found_register_arg = TRUE;
	    थांबा;
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
    command_ओळ_scan(&params);
    TIME_संदेश("parsing arguments");

    /*
     * On some systems, when we compile with the GUI, we always use it.  On Mac
     * there is no terminal version, and on Windows we can't fork one off with
     * :gui.
     */
#  ifdef ALWAYS_USE_GUI
    gui.starting = true;
#  अन्यथा
#   जर defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    /*
     * Check if the GUI can be started.  Reset gui.starting if not.
     * Don't know about other systems, stay on the safe side and don't check.
     */
    जर (gui.starting)
    {
	जर (gui_init_check() == FAIL)
	{
	    gui.starting = false;

	    // When running "evim" or "gvim -y" we need the menus, exit if we
	    // don't have them.
	    जर (params.evim_mode)
		mch_exit(1);
	}
    }
#   endif
#  endif

    जर (GARGCOUNT > 0)
    {
#  ifdef EXPAND_FILENAMES
	/*
	 * Expand wildcards in file names.
	 */
	जर (!params.literal)
	{
	    start_dir = alloc(MAXPATHL);
	    जर (start_dir != रिक्त)
		mch_dirname(start_dir, MAXPATHL);
	    // Temporarily add '(' and ')' to 'isfname'.  These are valid
	    // filename characters but are excluded from 'isfname' to make
	    // "gf" work on a file name in parentheses (e.g.: see vim.h).
	    do_cmdline_cmd((char_u *)":set isf+=(,)");
	    alist_expand(रिक्त, 0);
	    do_cmdline_cmd((char_u *)":set isf&");
	    जर (start_dir != रिक्त)
		mch_chdir((अक्षर *)start_dir);
	}
#  endif
	params.fname = alist_name(&GARGLIST[0]);
    }

#  ifdef MSWIN
    {
	// Remember the number of entries in the argument list.  If it changes
	// we don't react on setting 'encoding'.
	set_alist_मोजणी();
    }
#  endif

#  ifdef MSWIN
    जर (GARGCOUNT == 1 && params.full_path)
    {
	/*
	 * If there is one filename, fully qualified, we have very probably
	 * been invoked from explorer, so change to the file's directory.
	 * Hint: to avoid this when typing a command use a forward slash.
	 * If the cd fails, it doesn't matter.
	 */
	जर (vim_chdirfile(params.fname, "drop") == OK)
	    last_chdir_reason = "drop";
	जर (start_dir != रिक्त)
	    mch_dirname(start_dir, MAXPATHL);
    }
#  endif
    TIME_संदेश("expanding arguments");

#  ifdef FEAT_DIFF
    जर (params.diff_mode && params.window_मोजणी == -1)
	params.window_मोजणी = 0;	// open up to 3 windows
#  endif

    // Don't redraw until much later.
    ++RedrawingDisabled;

    /*
     * When listing swap file names, don't do cursor positioning et. al.
     */
    जर (recoverymode && params.fname == रिक्त)
	params.want_full_screen = FALSE;

    /*
     * When certain to start the GUI, don't check terminal capabilities.
     * For GTK we can't be sure, but when started from the desktop it doesn't
     * make sense to try using a terminal.
     */
#  जर defined(ALWAYS_USE_GUI) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	|| defined(VIMDLL)
    जर (gui.starting
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
    TIME_संदेश("shell init");

#  ifdef USE_XSMP
    /*
     * For want of anywhere else to do it, try to connect to xsmp here.
     * Fitting it in after gui_mch_init, but before gui_init (via termcapinit).
     * Hijacking -X 'no X connection' to also disable XSMP connection as that
     * has a similar delay upon failure.
     * Only try if SESSION_MANAGER is set to something non-null.
     */
    जर (!x_no_connect)
    {
	अक्षर *p = दो_पर्या("SESSION_MANAGER");

	जर (p != रिक्त && *p != NUL)
	{
	    xsmp_init();
	    TIME_संदेश("xsmp init");
	}
    }
#  endif

    /*
     * Print a warning if stdout is not a terminal.
     */
    check_tty(&params);

#  ifdef _IOLBF
    जर (silent_mode)
    {
	// Ensure output works usefully without a tty: buffer lines instead of
	// fully buffered.
	s_vbuf = दो_स्मृति(बफ_मान);
	जर (s_vbuf != रिक्त)
	    रखो_भबफ(मानक_निकास, s_vbuf, _IOLBF, बफ_मान);
    }
#  endif

    // This message comes before term inits, but after setting "silent_mode"
    // when the input is not a tty. Omit the message with --not-a-term.
    जर (GARGCOUNT > 1 && !silent_mode && !is_not_a_term())
	म_लिखो(_("%d files to edit\n"), GARGCOUNT);

    जर (params.want_full_screen && !silent_mode)
    {
	termcapinit(params.term);	// set terminal name and get terminal
				   // capabilities (will set full_screen)
	screen_start();		// don't know where cursor is now
	TIME_संदेश("Termcap init");
    }

    /*
     * Set the default values for the options that use Rows and Columns.
     */
    ui_get_shellsize();		// inits Rows and Columns
    win_init_size();
#  ifdef FEAT_DIFF
    // Set the 'diff' option now, so that it can be checked for in a .vimrc
    // file.  There is no buffer yet though.
    जर (params.diff_mode)
	diff_win_options(firstwin, FALSE);
#  endif

    cmdline_row = Rows - p_ch;
    cmdline_col_off = 0;
    cmdline_width = (पूर्णांक)Columns;
    संदेश_row = cmdline_row;
    संदेश_col = 0;
    screenalloc(FALSE);		// allocate screen buffers
    set_init_2();
    TIME_संदेश("inits 2");

    संदेश_scroll = TRUE;
    no_wait_return = TRUE;

    init_mappings();		// set up initial mappings

    init_highlight(TRUE, FALSE); // set the default highlight groups
    TIME_संदेश("init highlight");

#  जर defined(FEAT_TERMRESPONSE)
    init_term_props(TRUE);
#  endif

#  ifdef FEAT_EVAL
    // Set the break level after the terminal is initialized.
    debug_break_level = params.use_debug_break_level;
#  endif

    // Reset 'loadplugins' for "-u NONE" before "--cmd" arguments.
    // Allows for setting 'loadplugins' there.
    जर (params.use_vimrc != रिक्त
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
    परत mzscheme_main();
#  अन्यथा
    परत vim_main2();
#  endif
}
# endif // NO_VIM_MAIN
#endif // PROTO

#जर defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(FEAT_CLIPBOARD)
/*
 * Restore the state after a fatal X error.
 */
    स्थिर शून्य
x_restore_state(शून्य)
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
    पूर्णांक
vim_main2(शून्य)
{
#ifndef NO_VIM_MAIN
# ifdef FEAT_EVAL
    /*
     * Read all the plugin files.
     * Only when compiled with +eval, since most plugins need it.
     */
    जर (p_lpl)
    {
	char_u *rtp_copy = रिक्त;
	char_u *plugin_pattern = (char_u *)
#  जर defined(VMS) || defined(AMIGA) // VMS and Amiga don't handle the "**".
		"plugin/*.vim"
#  अन्यथा
		"plugin/**/*.vim"
#  endif
		;

	// First add all package directories to 'runtimepath', so that their
	// autoload directories can be found.  Only if not done already with a
	// :packloadall command.
	// Make a copy of 'runtimepath', so that source_runtime does not use
	// the pack directories.
	जर (!did_source_packages)
	{
	    rtp_copy = vim_strsave(p_rtp);
	    add_pack_start_dirs();
	}

	source_in_path(rtp_copy == रिक्त ? p_rtp : rtp_copy, plugin_pattern,
		DIP_ALL | DIP_NOAFTER, रिक्त);
	TIME_संदेश("loading plugins");
	vim_free(rtp_copy);

	// Only source "start" packages if not done already with a :packloadall
	// command.
	जर (!did_source_packages)
	    load_start_packages();
	TIME_संदेश("loading packages");

	source_runtime(plugin_pattern, DIP_ALL | DIP_AFTER);
	TIME_संदेश("loading after plugins");
    }
# endif

# ifdef FEAT_DIFF
    // Decide about window layout for diff mode after reading vimrc.
    जर (params.diff_mode && params.window_layout == 0)
    {
	जर (diffopt_horizontal())
	    params.window_layout = WIN_HOR;	// use horizontal split
	अन्यथा
	    params.window_layout = WIN_VER;	// use vertical split
    }
# endif

    /*
     * Recovery mode without a file name: List swap files.
     * This uses the 'dir' option, therefore it must be after the
     * initializations.
     */
    जर (recoverymode && params.fname == रिक्त)
    {
	recover_names(रिक्त, TRUE, रिक्त, 0, रिक्त);
	mch_exit(0);
    }

    /*
     * Set a few option defaults after reading .vimrc files:
     * 'title' and 'icon', Unix: 'shellpipe' and 'shellredir'.
     */
    set_init_3();
    TIME_संदेश("inits 3");

    /*
     * "-n" argument: Disable swap file by setting 'updatecount' to 0.
     * Note that this overrides anything from a vimrc file.
     */
    जर (params.no_swap_file)
	p_uc = 0;

# ifdef FEAT_GUI
    जर (gui.starting)
    {
#  जर defined(UNIX) || defined(VMS)
	// When something caused a message from a vimrc script, need to output
	// an extra newline before the shell prompt.
	जर (did_emsg || संदेश_didout)
	    अक्षर_दो('\n');
#  endif

	gui_start(रिक्त);		// will set full_screen to TRUE
	TIME_संदेश("starting GUI");

	// When running "evim" or "gvim -y" we need the menus, exit if we
	// don't have them.
	जर (!gui.in_use && params.evim_mode)
	    mch_exit(1);
	firstwin->w_prev_height = firstwin->w_height; // may have changed
    }
# endif

# ifdef FEAT_VIMINFO
    /*
     * Read in registers, history etc, but not marks, from the viminfo file.
     * This is where v:oldfiles gets filled.
     */
    जर (*p_viminfo != NUL)
    {
	read_viminfo(रिक्त, VIF_WANT_INFO | VIF_GET_OLDFILES);
	TIME_संदेश("reading viminfo");
    }
# endif
# ifdef FEAT_EVAL
    // It's better to make v:oldfiles an empty list than NULL.
    जर (get_vim_var_list(VV_OLDFILES) == रिक्त)
	set_vim_var_list(VV_OLDFILES, list_alloc());
# endif

# ifdef FEAT_QUICKFIX
    /*
     * "-q errorfile": Load the error file now.
     * If the error file can't be read, exit before doing anything else.
     */
    जर (params.edit_type == EDIT_QF)
    {
	char_u	*enc = रिक्त;

	enc = p_menc;
	जर (params.use_ef != रिक्त)
	    set_string_option_direct((char_u *)"ef", -1,
					   params.use_ef, OPT_FREE, SID_CARG);
	vim_snprintf((अक्षर *)IObuff, IOSIZE, "cfile %s", p_ef);
	जर (qf_init(रिक्त, p_ef, p_efm, TRUE, IObuff, enc) < 0)
	{
	    out_char('\n');
	    mch_exit(3);
	}
	TIME_संदेश("reading errorfile");
    }
# endif

    /*
     * Start putting things on the screen.
     * Scroll screen down before drawing over it
     * Clear screen now, so file message will not be cleared.
     */
    starting = NO_BUFFERS;
    no_wait_return = FALSE;
    जर (!exmode_active)
	संदेश_scroll = FALSE;

# ifdef FEAT_GUI
    /*
     * This seems to be required to make callbacks to be called now, instead
     * of after things have been put on the screen, which then may be deleted
     * when getting a resize callback.
     * For the Mac this handles putting files dropped on the Vim icon to
     * global_alist.
     */
    जर (gui.in_use)
    {
	gui_wait_for_chars(50L, typebuf.tb_change_cnt);
	TIME_संदेश("GUI delay");
    }
# endif

# जर defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)
    qnx_clip_init();
# endif

# जर defined(MACOS_X) && defined(FEAT_CLIPBOARD)
    clip_init(TRUE);
# endif

# ifdef FEAT_XCLIPBOARD
    // Start using the X clipboard, unless the GUI was started.
#  ifdef FEAT_GUI
    जर (!gui.in_use)
#  endif
    {
	setup_term_clip();
	TIME_संदेश("setup x11 clipboard");
    }
# endif

# ifdef FEAT_CLIENTSERVER
    // Prepare for being a Vim server.
    prepare_server(&params);
# endif

# ifdef FEAT_WAYLAND
#  ifdef FEAT_GUI
    जर (!gui.in_use)
#  endif
    {
	जर (wayland_init_connection(wayland_display_name) == OK)
	{
	    TIME_संदेश("connected to Wayland display");

#  ifdef FEAT_WAYLAND_CLIPBOARD
	    जर (clip_init_wayland() == OK)
		TIME_संदेश("setup Wayland clipboard");
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
    जर (params.edit_type == EDIT_STDIN && !recoverymode)
	read_stdin();

# जर defined(UNIX) || defined(VMS)
    // When switching screens and something caused a message from a vimrc
    // script, need to output an extra newline on exit.
    जर ((did_emsg || संदेश_didout) && *T_TI != NUL && params.edit_type != EDIT_STDIN)
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
    TIME_संदेश("setting raw mode");

    जर (need_wait_return || संदेश_didany)
    {
	wait_return(TRUE);
	TIME_संदेश("waiting for return");
    }

    starttermcap();	    // start termcap if not done by wait_return()
    TIME_संदेश("start termcap");

    setmouse();				// may start using the mouse
    जर (scroll_region)
	scroll_region_reset();		// In case Rows changed
    scroll_start();	// may scroll the screen to the right position

# जर defined(UNIX) || defined(VMS) || defined(MACOS_X)
    term_ढकला_title(SAVE_RESTORE_BOTH);
# endif

    /*
     * Don't clear the screen when starting in Ex mode, unless using the GUI.
     */
    जर (exmode_active
# ifdef FEAT_GUI
			&& !gui.in_use
# endif
					)
	set_must_redraw(UPD_CLEAR);
    अन्यथा
    {
	screenclear();			// clear screen
	TIME_संदेश("clearing screen");
    }

# ifdef FEAT_CRYPT
    जर (params.ask_for_किल्ली)
    {
	crypt_check_current_method();
	(शून्य)crypt_get_किल्ली(TRUE, TRUE);
	TIME_संदेश("getting crypt key");
    }
# endif

    no_wait_return = TRUE;

    /*
     * Create the requested number of windows and edit buffers in them.
     * Also does recovery if "recoverymode" set.
     */
    create_windows(&params);
    TIME_संदेश("opening buffers");

# ifdef FEAT_EVAL
    // clear v:swapcommand
    set_vim_var_string(VV_SWAPCOMMAND, रिक्त, -1);
# endif

    // Ex starts at last line of the file
    जर (exmode_active)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ओळ_मोजणी;

    apply_autocmds(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf);
    TIME_संदेश("BufEnter autocommands");
    setpcmark();

# ifdef FEAT_QUICKFIX
    /*
     * When started with "-q errorfile" jump to first error now.
     */
    जर (params.edit_type == EDIT_QF)
    {
	qf_jump(रिक्त, 0, 0, FALSE);
	TIME_संदेश("jump to first error");
    }
# endif

    /*
     * If opened more than one window, start editing files in the other
     * windows.
     */
    edit_buffers(&params, start_dir);
    vim_free(start_dir);

# ifdef FEAT_DIFF
    जर (params.diff_mode)
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
    जर (params.tagname != रिक्त)
    {
	swap_exists_did_quit = FALSE;

	vim_snprintf((अक्षर *)IObuff, IOSIZE, "ta %s", params.tagname);
	do_cmdline_cmd(IObuff);
	TIME_संदेश("jumping to tag");

	// If the user doesn't want to edit the file then we quit here.
	जर (swap_exists_did_quit)
	    getout(1);
    }

# जर defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    // Temporarily set x_jump_env to here in case there is an X11 IO error,
    // because x_jump_env is only actually set in main_loop(), before
    // exe_commands(). May not be the best solution since commands passed via
    // the command line can be very broad like sourcing a file, in which case
    // an X IO error results in the command being partially done. In theory we
    // could use SETJMP in RealWaitForChar(), but the stack frame for that may
    // possibly exit and then LONGJMP is called on it.
    पूर्णांक jump_result = SETJMP(x_jump_env);

    जर (jump_result == 0)
    {
# endif
	// Execute any "+", "-c" and "-S" arguments.
	जर (params.n_commands > 0)
	    exe_commands(&params);
# जर defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    }
    अन्यथा
	// Restore state and continue just like what main_loop() does.
	x_restore_state();
# endif

    // Must come before the may_req_ calls.
    starting = 0;

# जर defined(FEAT_TERMRESPONSE)
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
# जर defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_size();
# endif

    // start in insert mode
    जर (p_im)
	need_start_insertmode = TRUE;

# ifdef FEAT_EVAL
    set_vim_var_nr(VV_VIM_DID_ENTER, 1L);
# endif
    apply_autocmds(EVENT_VIMENTER, रिक्त, रिक्त, FALSE, curbuf);
    TIME_संदेश("VimEnter autocommands");

# जर defined(FEAT_EVAL) && defined(FEAT_CLIPBOARD)
    // Adjust default register name for "unnamed" in 'clipboard'. Can only be
    // done after the clipboard is available and all initial commands that may
    // modify the 'clipboard' setting have run; i.e. just before entering the
    // main loop.
    reset_reg_var();
# endif

# जर defined(FEAT_DIFF)
    // When a startup script or session file setup for diff'ing and
    // scrollbind, sync the scrollbind now.
    जर (curwin->w_p_diff && curwin->w_p_scb)
    {
	update_topline();
	check_scrollbind((linenr_T)0, 0L);
	TIME_संदेश("diff scrollbinding");
    }
# endif

# जर defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    जर (!gui.in_use)
#  endif
	mch_set_winsize_now();	    // Allow winsize changes from now on
# endif

# जर defined(FEAT_GUI)
    // When tab pages were created, may need to update the tab pages line and
    // scrollbars.  This is skipped while creating them.
    जर (gui.in_use && first_tabpage->tp_next != रिक्त)
    {
	out_flush();
	gui_init_which_components(रिक्त);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
# endif

    // If ":startinsert" command used, stuff a dummy command to be able to
    // call normal_cmd(), which will then start Insert mode.
    जर (restart_edit != 0)
	stuffcharReadbuff(K_NOP);

# ifdef FEAT_NETBEANS_INTG
    जर (netbeansArg != रिक्त && म_भेदन("-nb", netbeansArg, 3) == 0)
    {
#  ifdef FEAT_GUI
#   जर !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
	जर (gui.in_use)
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

    TIME_संदेश("before starting main loop");

# ifdef FEAT_JOB_CHANNEL
    जर (use_stdio_channel)
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

    परत 0;
}

/*
 * Initialisation #1 shared by main() and some tests.
 */
    शून्य
common_init_1(शून्य)
{
    estack_init();
    cmdline_init();

    (शून्य)mb_init();	// init mb_bytelen_tab[] to ones
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
    जर ((IObuff = alloc(IOSIZE)) == रिक्त
	    || (NameBuff = alloc(MAXPATHL)) == रिक्त)
	mch_exit(0);
    TIME_संदेश("Allocated generic buffers");
}


/*
 * Initialisation #2 shared by main() and some tests.
 */
    शून्य
common_init_2(mparm_T *paramp)
{
#ifdef NBDEBUG
    // Wait a moment for debugging NetBeans.  Must be after allocating
    // NameBuff.
    nbdebug_log_init("SPRO_GVIM_DEBUG", "SPRO_GVIM_DLEVEL");
    nbdebug_wait(WT_ENV | WT_WAIT | WT_STOP, "SPRO_GVIM_WAIT", 20);
    TIME_संदेश("NetBeans debug wait");
#endif

#जर defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    /*
     * Setup to use the current locale (for ctype() and many other things).
     * NOTE: Translated messages with encodings other than latin1 will not
     * work until set_init_1() has been called!
     */
    init_locale();
    TIME_संदेश("locale set");
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

#जर defined(FEAT_GUI)
    // Prepare for possibly starting GUI sometime
    gui_prepare(&paramp->argc, paramp->argv);
    TIME_संदेश("GUI prepared");
#endif

#ifdef FEAT_CLIPBOARD
    clip_init(FALSE);		// Initialise clipboard stuff
    TIME_संदेश("clipboard setup");
#endif

    /*
     * Check if we have an interactive window.
     * On the Amiga: If there is no window, we open one with a newcli command
     * (needed for :! to * work). mch_check_win() will also handle the -d or
     * -dev argument.
     */
    stdout_isatty = (mch_check_win(paramp->argc, paramp->argv) != FAIL);
    TIME_संदेश("window checked");

    /*
     * Allocate the first window and buffer.
     * Can't do anything without it, exit when it fails.
     */
    जर (win_alloc_first() == FAIL)
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
    TIME_संदेश("inits 1");

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
    qf_init_चळत();
#endif
}

/*
 * Return TRUE when the --not-a-term argument was found.
 */
    पूर्णांक
is_not_a_term(शून्य)
{
    परत params.not_a_term;
}

/*
 * Return TRUE when the --not-a-term argument was found or the GUI is in use.
 */
    पूर्णांक
is_not_a_term_or_gui(शून्य)
{
    परत params.not_a_term
#ifdef FEAT_GUI
			    || gui.in_use
#endif
	;
}

#जर defined(EXITFREE)
    शून्य
free_vbuf(शून्य)
{
# ifdef _IOLBF
    जर (s_vbuf != रिक्त)
    {
	रखो_भबफ(मानक_निकास, रिक्त, _IONBF, 0);
	मुक्त(s_vbuf);
	s_vbuf = रिक्त;
    }
# endif
}
#endif

#जर defined(FEAT_GUI)
/*
 * If a --gui-dialog-file argument was given return the file name.
 * Otherwise return NULL.
 */
    char_u *
get_gui_dialog_file(शून्य)
{
    परत params.gui_dialog_file;
}
#endif

// When TRUE in a safe state when starting to wait for a character.
स्थिर पूर्णांक	was_safe = FALSE;
स्थिर oparg_T	*current_oap = रिक्त;

/*
 * Return TRUE if an operator was started but not finished yet.
 * Includes typing a count or a register name.
 */
    पूर्णांक
op_pending(शून्य)
{
    परत !(current_oap != रिक्त
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
    स्थिर पूर्णांक
is_safe_now(शून्य)
{
    परत stuff_empty()
	&& typebuf.tb_len == 0
	&& scriptin[curscript] == रिक्त
#ifdef FEAT_EVAL
	&& !debug_mode
#endif
	&& !global_busy;
}

/*
 * Trigger SafeState if currently in a safe state, that is "safe" is TRUE and
 * there is no typeahead.
 */
    शून्य
may_trigger_safestate(पूर्णांक safe)
{
    पूर्णांक is_safe = safe && is_safe_now();

#ifdef FEAT_EVAL
    जर (was_safe != is_safe)
	// Only log when the state changes, otherwise it happens at nearly
	// every key stroke.
	ch_log(रिक्त, is_safe ? "SafeState: Start triggering"
					       : "SafeState: Stop triggering");
#endif
    जर (is_safe)
	apply_autocmds(EVENT_SAFESTATE, रिक्त, रिक्त, FALSE, curbuf);
    was_safe = is_safe;
}

/*
 * Something changed which causes the state possibly to be unsafe, e.g. a
 * character was typed.  It will remain unsafe until the next call to
 * may_trigger_safestate().
 */
    शून्य
state_no_longer_safe(अक्षर *reason UNUSED)
{
#ifdef FEAT_EVAL
    जर (was_safe)
	ch_log(रिक्त, "SafeState: reset: %s", reason);
#endif
    was_safe = FALSE;
}

#जर defined(FEAT_EVAL) || defined(MESSAGE_QUEUE)
    पूर्णांक
get_was_safe_state(शून्य)
{
    परत was_safe;
}
#endif

#जर defined(MESSAGE_QUEUE)
/*
 * Invoked when leaving code that invokes callbacks.  Then trigger
 * SafeStateAgain, if it was safe when starting to wait for a character.
 */
    शून्य
may_trigger_safestateagain(शून्य)
{
    जर (!was_safe)
    {
	// If the safe state was reset in state_no_longer_safe(), e.g. because
	// of calling feedkeys(), we check if it's now safe again (all keys
	// were consumed).
	was_safe = is_safe_now();
# ifdef FEAT_EVAL
	जर (was_safe)
	    ch_log(रिक्त, "SafeState: undo reset");
# endif
    }
    जर (was_safe)
    {
# ifdef FEAT_EVAL
	// Only do this message when another message was given, otherwise we
	// get lots of them.
	जर ((did_repeated_संदेश & REPEATED_संदेश_SAFESTATE) == 0)
	{
	    पूर्णांक did = did_repeated_संदेश;

	    ch_log(रिक्त,
		      "SafeState: back to waiting, triggering SafeStateAgain");
	    did_repeated_संदेश = did | REPEATED_संदेश_SAFESTATE;
	}
# endif
	apply_autocmds(EVENT_SAFESTATEAGAIN, रिक्त, रिक्त, FALSE, curbuf);
    }
# ifdef FEAT_EVAL
    अन्यथा
	ch_log(रिक्त,
		  "SafeState: back to waiting, not triggering SafeStateAgain");
# endif
}
#endif

/*
 * Return TRUE if there is any typeahead, pending operator or command.
 */
    पूर्णांक
work_pending(शून्य)
{
    परत op_pending() || !is_safe_now();
}

#ifdef FEAT_CONCEAL
स्थिर linenr_T	conceal_old_cursor_ओळ = 0;
स्थिर linenr_T	conceal_new_cursor_ओळ = 0;
स्थिर पूर्णांक	conceal_update_ओळी = FALSE;
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
    शून्य
may_trigger_deferred_events(शून्य)
{
    स्थिर bool	recursive = false;

    जर (recursive)
	परत;
    recursive = true;

#ifdef FEAT_CONCEAL
    जर (curwin->w_p_cole == 0)
	conceal_update_ओळी = FALSE;
#endif

    // Trigger CursorMoved if the cursor moved.
    जर (!finish_op && (has_cursormoved()
#ifdef FEAT_PROP_POPUP
		|| popup_visible
#endif
#ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
#endif
		) && !EQUAL_POS(last_cursormoved, curwin->w_cursor))
    {
	जर (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, रिक्त, रिक्त, FALSE, curbuf);
#ifdef FEAT_PROP_POPUP
	जर (popup_visible)
	    popup_check_cursor_pos();
#endif
#ifdef FEAT_CONCEAL
	जर (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_ओळ = last_cursormoved.lnum;
	    conceal_new_cursor_ओळ = curwin->w_cursor.lnum;
	    conceal_update_ओळी = TRUE;
	}
#endif
	last_cursormoved = curwin->w_cursor;
    }

#ifdef FEAT_CONCEAL
    जर (conceal_update_ओळी
	    && (conceal_old_cursor_ओळ != conceal_new_cursor_ओळ
		|| conceal_cursor_ओळ(curwin)
		|| need_cursor_ओळ_redraw))
    {
	जर (conceal_old_cursor_ओळ != conceal_new_cursor_ओळ
		&& conceal_old_cursor_ओळ != 0
		&& conceal_old_cursor_ओळ <= curbuf->b_ml.ml_ओळ_मोजणी)
	    redrawWinline(curwin, conceal_old_cursor_ओळ);
	redrawWinline(curwin, conceal_new_cursor_ओळ);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_ओळ_redraw = FALSE;
    }
#endif

    // Trigger TextChanged if b:changedtick differs.
    जर (!finish_op && has_textchanged()
	    && curbuf->b_last_changedtick != CHANGEDTICK(curbuf))
    {
	apply_autocmds(EVENT_TEXTCHANGED, रिक्त, रिक्त, FALSE, curbuf);
	curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    }

    // Ensure curwin->w_topline and curwin->w_leftcol are up to date before
    // triggering a WinScrolled autocommand.
    update_topline();
    validate_cursor();

    जर (!finish_op)
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
    शून्य
main_loop(
    पूर्णांक		cmdwin,	    // TRUE when working in the command-line window
    पूर्णांक		noexmode)   // TRUE when return on entering Ex mode
{
    oparg_T	oa;		// operator arguments
    oparg_T	*prev_oap;	// operator arguments
    अस्थिर पूर्णांक previous_got_int = FALSE;	// "got_int" was TRUE

    prev_oap = current_oap;
    current_oap = &oa;

#जर defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    // Setup to catch a terminating error from the X server.  Just ignore
    // it, restore the state and continue.  This might not always work
    // properly, but at least we hopefully don't exit unexpectedly when the X
    // server exits while Vim is running in a console.
    जर (!cmdwin && !noexmode && SETJMP(x_jump_env))
	x_restore_state();
#endif

    clear_oparg(&oa);
    पर्यंत (!cmdwin || cmdwin_result == 0)
    {
	जर (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    जर (need_check_timestamps)
		check_timestamps(FALSE);
	    जर (need_wait_return)	// if wait_return() still needed ...
		wait_return(FALSE);	// ... call it now
	    जर (need_start_insertmode && goto_im() && !VIsual_active)
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
	जर (got_int)
	{
	    जर (noexmode && global_busy && !exmode_active && previous_got_int)
	    {
		// Typed two CTRL-C in a row: go back to ex mode as if "Q" was
		// used and keep "got_int" set, so that it aborts ":g".
		exmode_active = EXMODE_NORMAL;
		State = MODE_NORMAL;
	    }
	    अन्यथा जर (!global_busy || !exmode_active)
	    {
		जर (!quit_more)
		    (शून्य)vgetc();		// flush all buffers
		got_int = FALSE;
	    }
	    previous_got_int = TRUE;
	}
	अन्यथा
	    previous_got_int = FALSE;

#ifdef FEAT_EVAL
	// At the toplevel there is no exception handling.  Discard any that
	// may be hanging around (e.g. from "interrupt" at the debug prompt).
	जर (did_throw && !ex_normal_busy)
	    discard_current_exception();
#endif

	जर (!exmode_active)
	    संदेश_scroll = FALSE;
	quit_more = FALSE;

	// it's not safe unless may_trigger_safestate_main() is called
	was_safe = FALSE;

	/*
	 * If skip redraw is set (for ":" in wait_return()), don't redraw now.
	 * If there is nothing in the stuff_buffer or do_redraw is TRUE,
	 * update cursor and redraw.
	 */
	जर (skip_redraw || exmode_active)
	{
	    skip_redraw = FALSE;
	    setcursor();
	    cursor_on();
	}
	अन्यथा जर (do_redraw || stuff_empty())
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

#जर defined(FEAT_DIFF)
	    // Updating diffs from changed() does not always work properly,
	    // esp. updating folds.  Do an update just before redrawing if
	    // needed.
	    जर (curtab->tp_diff_update || curtab->tp_diff_invalid)
	    {
		ex_diffupdate(रिक्त);
		curtab->tp_diff_update = FALSE;
	    }

	    // Scroll-binding for diff mode may have been postponed until
	    // here.  Avoids doing it for every change.
	    जर (diff_need_scrollbind)
	    {
		check_scrollbind((linenr_T)0, 0L);
		diff_need_scrollbind = FALSE;
	    }
#endif
#जर defined(FEAT_FOLDING)
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
	    जर (hasAnyFolding(curwin) && !char_avail())
	    {
		foldCheckClose();
		जर (fdo_flags & FDO_ALL)
		    foldOpenCursor();
	    }
#endif

	    // Before redrawing, make sure w_topline is correct, and w_leftcol
	    // if lines don't wrap, and w_skipcol if lines wrap.
	    update_topline();
	    validate_cursor();

	    जर (VIsual_active)
		update_curbuf(UPD_INVERTED); // update inverted part
	    अन्यथा जर (must_redraw)
	    {
		mch_disable_flush();	// Stop issuing gui_mch_flush().
		update_screen(0);
		mch_enable_flush();
	    }
	    अन्यथा जर (redraw_cmdline || clear_cmdline || redraw_mode)
		showmode();
	    redraw_statuslines();
	    जर (need_maketitle)
		maketitle();
#ifdef FEAT_VIMINFO
	    curbuf->b_last_used = vim_time();
#endif
	    // display message after redraw
	    जर (keep_संदेश != रिक्त)
	    {
		char_u *p = vim_strsave(keep_संदेश);

		जर (p != रिक्त)
		{
		    // msg_start() will set keep_msg to NULL, make a copy
		    // first.  Don't reset keep_msg, msg_attr_keep() uses it to
		    // check for duplicates.  Never put this message in
		    // history.
		    संदेश_hist_off = TRUE;
		    संदेश_attr((अक्षर *)p, keep_संदेश_attr);
		    संदेश_hist_off = FALSE;
		    vim_free(p);
		}
	    }
	    जर (need_fileinfo)		// show file info after redraw
	    {
		fileinfo(FALSE, TRUE, FALSE);
		need_fileinfo = FALSE;
	    }

	    emsg_on_display = FALSE;	// can delete error message now
	    did_emsg = FALSE;
	    संदेश_didany = FALSE;		// reset lines_left in msg_start()
	    may_clear_sb_text();	// clear scroll-back text on next msg
	    showruler(FALSE);

	    setcursor();
	    cursor_on();

	    do_redraw = FALSE;

#ifdef STARTUPTIME
	    // Now that we have drawn the first screen all the startup stuff
	    // has been done, close any file for startup messages.
	    जर (time_fd != रिक्त)
	    {
		TIME_संदेश("first screen update");
		TIME_संदेश("--- VIM STARTED ---");
		ख_बंद(time_fd);
		time_fd = रिक्त;
	    }
#endif
	    // After the first screen update may start triggering WinScrolled
	    // autocmd events.  Store all the scroll positions and sizes now.
	    may_make_initial_scroll_size_snapshot();
	}
#ifdef FEAT_GUI
	जर (need_mouse_correct)
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
	जर (exmode_active)
	{
	    जर (noexmode)   // End of ":global/path/visual" commands
		जा theend;
	    do_exmode(exmode_active == EXMODE_VIM);
	}
	अन्यथा
	{
#ifdef FEAT_TERMINAL
	    जर (term_use_loop()
		    && oa.op_type == OP_NOP && oa.regname == NUL
		    && !VIsual_active
		    && !skip_term_loop)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal mode.  With FAIL we first need to position the
		// cursor and the screen needs to be redrawn.
		जर (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    अन्यथा
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


#जर defined(USE_XSMP) || defined(FEAT_GUI)
/*
 * Exit, but leave behind swap files for modified buffers.
 */
    शून्य
getout_preserve_modified(पूर्णांक exitval)
{
# जर defined(SIGHUP) && defined(संक_छोड़ो)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, संक_छोड़ो);
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
    शून्य
getout(पूर्णांक exitval)
{
    exiting = TRUE;
#जर defined(FEAT_EVAL)
    ch_log(रिक्त, "Exiting...");
#endif

    // When running in Ex mode an error causes us to exit with a non-zero exit
    // code.  POSIX requires this, although it's not 100% clear from the
    // standard.
    जर (exmode_active)
	exitval += ex_exitval;

#ifdef FEAT_EVAL
    set_vim_var_type(VV_EXITING, VAR_संख्या);
    set_vim_var_nr(VV_EXITING, exitval);
#endif

    // Position the cursor on the last screen line, below all the text
    जर (!is_not_a_term_or_gui())
	windgoto((पूर्णांक)Rows - 1, 0);

#ifdef FEAT_EVAL
    // Invoked all deferred functions in the function stack.
    invoke_all_defer();
#endif

#जर defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
    // Optionally print hashtable efficiency.
    hash_debug_results();
#endif

#ifdef FEAT_GUI
    संदेश_didany = FALSE;
#endif

    जर (v_dying <= 1)
    {
	tabpage_T	*tp;
	tabpage_T	*next_tp;
	buf_T		*buf;
	win_T		*wp;
	पूर्णांक		unblock = 0;

	// Trigger BufWinLeave for all windows, but only once per buffer.
	साठी (tp = first_tabpage; tp != रिक्त; tp = next_tp)
	{
	    next_tp = tp->tp_next;
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		जर (wp->w_buffer == रिक्त || !buf_valid(wp->w_buffer))
		    // Autocmd must have close the buffer already, skip.
		    सुरूठेवा;
		buf = wp->w_buffer;
		जर (CHANGEDTICK(buf) != -1)
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf);
		    apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname,
						    buf->b_fname, FALSE, buf);
		    जर (bufref_valid(&bufref))
			CHANGEDTICK(buf) = -1;  // note we did it already

		    // start all over, autocommands may mess up the lists
		    next_tp = first_tabpage;
		    थांबा;
		}
	    }
	}

	// Trigger BufUnload for buffers that are loaded
	FOR_ALL_BUFFERS(buf)
	    जर (buf->b_ml.ml_mfp != रिक्त)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf);
		जर (!bufref_valid(&bufref))
		    // autocmd deleted the buffer
		    थांबा;
	    }

	// deathtrap() blocks autocommands, but we do want to trigger
	// VimLeavePre.
	जर (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVEPRE, रिक्त, रिक्त, FALSE, curbuf);
	जर (unblock)
	    block_autocmds();
    }

#ifdef FEAT_VIMINFO
    जर (
# ifdef EXITFREE
	    entered_free_all_mem == FALSE &&
# endif
	    *p_viminfo != NUL)
	// Write out the registers, history, marks etc, to the viminfo file
	write_viminfo(रिक्त, FALSE);
#endif

    जर (v_dying <= 1)
    {
	पूर्णांक	unblock = 0;

	// deathtrap() blocks autocommands, but we do want to trigger VimLeave.
	जर (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	जर (unblock)
	    block_autocmds();
    }

#ifdef FEAT_PROFILE
    profile_dump();
#endif

    जर (did_emsg
#ifdef FEAT_GUI
	    || (gui.in_use && संदेश_didany && p_verbose > 0)
#endif
	    )
    {
	// give the user a chance to read the (error) message
	no_wait_return = FALSE;
	wait_return(FALSE);
    }

    // Position the cursor again, the autocommands may have moved it
    जर (!is_not_a_term_or_gui())
	windgoto((पूर्णांक)Rows - 1, 0);

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
#जर defined(USE_ICONV) && defined(DYNAMIC_ICONV)
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
    जर (garbage_collect_at_exit)
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
    स्थिर शून्य
early_arg_scan(mparm_T *parmp UNUSED)
{
#जर defined(FEAT_XCLIPBOARD) || defined(FEAT_CLIENTSERVER) \
	|| !defined(FEAT_NETBEANS_INTG)
    पूर्णांक		argc = parmp->argc;
    अक्षर	**argv = parmp->argv;
    पूर्णांक		i;

    साठी (i = 1; i < argc; i++)
    {
	जर (STRCMP(argv[i], "--") == 0)
	    थांबा;
# ifdef FEAT_XCLIPBOARD
	अन्यथा जर (STRICMP(argv[i], "-display") == 0
#  जर defined(FEAT_GUI_GTK)
		|| STRICMP(argv[i], "--display") == 0
#  endif
		)
	{
	    जर (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    xterm_display = argv[++i];
	}
# endif
# ifdef FEAT_CLIENTSERVER
	अन्यथा जर (STRICMP(argv[i], "--servername") == 0)
	{
	    जर (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    parmp->serverName_arg = (char_u *)argv[++i];
	}
	अन्यथा जर (STRICMP(argv[i], "--serverlist") == 0)
	    parmp->serverArg = TRUE;
	अन्यथा जर (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    parmp->serverArg = TRUE;
#  ifdef FEAT_GUI
	    जर (म_माला(argv[i], "-wait") != 0)
		// don't fork() when starting the GUI to edit files ourself
		gui.dofork = false;
#  endif
	}
#  ifdef FEAT_CLIENTSERVER_BACKENDS
	अन्यथा जर (STRICMP(argv[i], "--clientserver") == 0)
	{
	    char_u *arg;
	    जर (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    arg = (char_u *)argv[++i];

	    जर (STRICMP(arg, "socket") == 0)
		clientserver_method = CLIENTSERVER_METHOD_SOCKET;
#   ifdef FEAT_X11
	    अन्यथा जर (STRICMP(arg, "x11") == 0)
		clientserver_method = CLIENTSERVER_METHOD_X11;
#   endif
#   ifdef MSWIN
	    अन्यथा जर (STRICMP(arg, "mswin") == 0)
		clientserver_method = CLIENTSERVER_METHOD_MSWIN;
#   endif
	    अन्यथा
		mainerr(ME_UNKNOWN_OPTION, arg);
	}
#  endif
# endif

# जर defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
#  ifdef FEAT_GUI_MSWIN
	अन्यथा जर (STRICMP(argv[i], "--windowid") == 0)
#  अन्यथा
	अन्यथा जर (STRICMP(argv[i], "--socketid") == 0)
#  endif
	{
	    long_u	id;
	    पूर्णांक		मोजणी;

	    जर (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    जर (STRNICMP(argv[i+1], "0x", 2) == 0)
		मोजणी = माला_पूछो(&(argv[i + 1][2]), SCANF_HEX_LONG_U, &id);
	    अन्यथा
		मोजणी = माला_पूछो(argv[i + 1], SCANF_DECIMAL_LONG_U, &id);
	    जर (मोजणी != 1)
		mainerr(ME_INVALID_ARG, (char_u *)argv[i]);
	    अन्यथा
#  ifdef FEAT_GUI_MSWIN
		win_socket_id = id;
#  elif !defined(USE_GTK4)
		gtk_socket_id = id;
#  endif
	    i++;
	}
# endif
# जर defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
	अन्यथा जर (STRICMP(argv[i], "--echo-wid") == 0)
	    echo_wid_arg = TRUE;
# endif
# ifndef FEAT_NETBEANS_INTG
	अन्यथा जर (म_भेदन(argv[i], "-nb", (माप_प्रकार)3) == 0)
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
    स्थिर पूर्णांक
get_संख्या_arg(
    char_u	*p,	    // pointer to argument
    पूर्णांक		*idx,	    // index in argument, is incremented
    पूर्णांक		def)	    // default value
{
    जर (vim_isdigit(p[*idx]))
    {
	def = म_से_प((अक्षर *)&(p[*idx]));
	पर्यंत (vim_isdigit(p[*idx]))
	    *idx = *idx + 1;
    }
    परत def;
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
    स्थिर शून्य
parse_command_name(mparm_T *parmp)
{
    char_u	*initstr;

    initstr = gettail((char_u *)parmp->argv[0]);

# ifdef FEAT_EVAL
    set_vim_var_string(VV_PROGNAME, initstr, -1);
    set_progpath((char_u *)parmp->argv[0]);
# endif

    जर (TOLOWER_ASC(initstr[0]) == 'r')
    {
	restricted = TRUE;
	++initstr;
    }

    // Use evim mode for "evim" and "egvim", not for "editor".
    जर (TOLOWER_ASC(initstr[0]) == 'e'
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
    जर (TOLOWER_ASC(initstr[0]) == 'g'
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
    अन्यथा
	gui.dospawn = true;	// Not "gvim". Need to spawn gvim.exe.
# endif


    जर (STRNICMP(initstr, "view", 4) == 0)
    {
	readonlymode = TRUE;
	curbuf->b_p_ro = TRUE;
	p_uc = 10000;			// don't update very often
	initstr += 4;
    }
    अन्यथा जर (STRNICMP(initstr, "vim", 3) == 0)
	initstr += 3;

    // Catch "[r][g]vimdiff" and "[r][g]viewdiff".
    जर (STRICMP(initstr, "diff") == 0)
    {
# ifdef FEAT_DIFF
	parmp->diff_mode = TRUE;
# अन्यथा
	mch_errmsg(_("This Vim was not compiled with the diff feature."));
	mch_errmsg("\n");
	mch_exit(2);
# endif
    }

    // Checking for "ex" here may catch some weird names, such as "vimex" or
    // "viewex", we assume the user knows that.
    जर (STRNICMP(initstr, "ex", 2) == 0)
    {
	जर (STRNICMP(initstr + 2, "im", 2) == 0)
	    exmode_active = EXMODE_VIM;
	अन्यथा
	    exmode_active = EXMODE_NORMAL;
	change_compatible(TRUE);	// set 'compatible'
    }
}

/*
 * Scan the command line arguments.
 */
    स्थिर शून्य
command_ओळ_scan(mparm_T *parmp)
{
    पूर्णांक		argc = parmp->argc;
    अक्षर	**argv = parmp->argv;
    पूर्णांक		argv_idx;		// index in argv[n][]
    पूर्णांक		had_minmin = FALSE;	// found "--" argument
    पूर्णांक		want_argument;		// option argument with argument
    पूर्णांक		c;
    char_u	*p = रिक्त;
    दीर्घ	n;

    --argc;
    ++argv;
    argv_idx = 1;	    // active option letter is argv[0][argv_idx]
    पर्यंत (argc > 0)
    {
	/*
	 * "+" or "+{number}" or "+/{pat}" or "+{command}" argument.
	 */
	जर (argv[0][0] == '+' && !had_minmin)
	{
	    जर (parmp->n_commands >= MAX_ARG_CMDS)
		mainerr(ME_EXTRA_CMD, रिक्त);
	    argv_idx = -1;	    // skip to next argument
	    जर (argv[0][1] == NUL)
		parmp->commands[parmp->n_commands++] = (char_u *)"$";
	    अन्यथा
		parmp->commands[parmp->n_commands++] = (char_u *)&(argv[0][1]);
	}

	/*
	 * Optional argument.
	 */
	अन्यथा जर (argv[0][0] == '-' && !had_minmin)
	{
	    want_argument = FALSE;
	    c = argv[0][argv_idx++];
# जर defined( VMS)
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
	    जर (c == '/')
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
	    निवड (c)
	    {
	    स्थिती NUL:		// "vim -"  read from stdin
				// "ex -" silent mode
		जर (exmode_active)
		    silent_mode = TRUE;
		अन्यथा
		{
		    जर (parmp->edit_type != EDIT_NONE)
			mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
		    parmp->edit_type = EDIT_STDIN;
		    read_cmd_fd = 2;	// read from stderr instead of stdin
		}
		argv_idx = -1;		// skip to next argument
		थांबा;

	    स्थिती '-':		// "--" don't take any more option arguments
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
		जर (STRICMP(argv[0] + argv_idx, "help") == 0)
		    usage();
		अन्यथा जर (STRICMP(argv[0] + argv_idx, "version") == 0)
		{
		    cmdline_width = Columns = 80;   // need to init Columns
		    info_message = TRUE; // use mch_msg(), not mch_errmsg()
# जर defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
		    gui.starting = false; // not starting GUI, will exit
# endif
		    list_version();
		    संदेश_putchar('\n');
		    संदेश_didout = FALSE;
		    mch_exit(0);
		}
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "clean", 5) == 0)
		{
		    parmp->use_vimrc = (char_u *)"DEFAULTS";
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)"NONE";
# endif
		    parmp->clean = TRUE;
		    set_option_मूल्य_give_err((char_u *)"vif",
						      0L, (char_u *)"NONE", 0);
		}
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "literal", 7) == 0)
		{
# ifdef EXPAND_FILENAMES
		    parmp->literal = TRUE;
# endif
		}
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "nofork", 6) == 0)
		{
# ifdef FEAT_GUI
		    gui.dofork = false;	// don't fork() when starting GUI
# endif
		}
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "noplugin", 8) == 0)
		    p_lpl = FALSE;
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "not-a-term", 10) == 0)
		    parmp->not_a_term = TRUE;
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "gui-dialog-file", 15)
									 == 0)
		{
		    want_argument = TRUE;
		    argv_idx += 15;
		}
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "ttyfail", 7) == 0)
		    parmp->tty_fail = TRUE;
# ifdef FEAT_JOB_CHANNEL
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "stdio-channel", 13) == 0)
		{
		    // Like "-es": no terminal, but the channel is served
		    // instead of reading Ex commands.
		    use_stdio_channel = TRUE;
		    exmode_active = EXMODE_NORMAL;
		    silent_mode = TRUE;
#  जर defined(FEAT_GUI) && !defined(VIMDLL)
		    gui.starting = false;	// don't start GUI
#  endif
		}
# endif
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "cmd", 3) == 0)
		{
		    want_argument = TRUE;
		    argv_idx += 3;
		}
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "startuptime", 11) == 0)
		{
		    want_argument = TRUE;
		    argv_idx += 11;
		}
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "log", 3) == 0)
		{
		    want_argument = TRUE;
		    argv_idx += 3;
		}
# ifdef FEAT_CLIENTSERVER
		अन्यथा जर (STRICMP(argv[0] + argv_idx, "serverlist") == 0)
		    ; // already processed -- no arg
		अन्यथा जर (STRICMP(argv[0] + argv_idx, "servername") == 0
		       || STRICMP(argv[0] + argv_idx, "serversend") == 0
		       // Don't put this under FEAT_CLIENTSERVER_BACKENDS, just
		       // let it be ignored. Makes tests less complicated
		       || STRICMP(argv[0] + argv_idx, "clientserver") == 0
		       )
		{
		    // already processed -- snatch the following arg
		    जर (argc > 1)
		    {
			--argc;
			++argv;
		    }
		}
# endif
# जर defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
#  ifdef FEAT_GUI_GTK
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "socketid", 8) == 0)
#  अन्यथा
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "windowid", 8) == 0)
#  endif
		{
		    // already processed -- snatch the following arg
		    जर (argc > 1)
		    {
			--argc;
			++argv;
		    }
		}
# endif
# ifdef FEAT_GUI_GTK
		अन्यथा जर (STRNICMP(argv[0] + argv_idx, "echo-wid", 8) == 0)
		{
		    // already processed, skip
		}
# endif
		अन्यथा
		{
		    जर (argv[0][argv_idx])
			mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
		    had_minmin = TRUE;
		}
		जर (!want_argument)
		    argv_idx = -1;	// skip to next argument
		थांबा;

	    स्थिती 'A':		// "-A" start in Arabic mode
# ifdef FEAT_ARABIC
		set_option_मूल्य_give_err((char_u *)"arabic", 1L, रिक्त, 0);
# अन्यथा
		mch_errmsg(_(e_arabic_cannot_be_used_not_enabled_at_compile_time));
		mch_exit(2);
# endif
		थांबा;

	    स्थिती 'b':		// "-b" binary mode
		// Needs to be effective before expanding file names, because
		// for Win32 this makes us edit a shortcut file itself,
		// instead of the file it links to.
		set_options_bin(curbuf->b_p_bin, 1, 0);
		curbuf->b_p_bin = 1;	    // binary file I/O
		थांबा;

	    स्थिती 'C':		// "-C"  Compatible
		change_compatible(TRUE);
		has_dash_c_arg = TRUE;
		थांबा;

	    स्थिती 'e':		// "-e" Ex mode
		exmode_active = EXMODE_NORMAL;
		थांबा;

	    स्थिती 'E':		// "-E" Improved Ex mode
		exmode_active = EXMODE_VIM;
		थांबा;

	    स्थिती 'f':		// "-f"  GUI: run in foreground.  Amiga: open
				// window directly, not with newcli
# ifdef FEAT_GUI
		gui.dofork = false;	// don't fork() when starting GUI
# endif
		थांबा;

	    स्थिती 'g':		// "-g" start GUI
		main_start_gui();
		थांबा;

	    स्थिती 'F':		// "-F" was for Farsi mode
		mch_errmsg(_(e_farsi_support_has_been_removed));
		mch_exit(2);
		थांबा;

	    स्थिती '?':		// "-?" give help message (for MS-Windows)
	    स्थिती 'h':		// "-h" give help message
# ifdef FEAT_GUI_GNOME
		// Tell usage() to exit for "gvim".
		gui.starting = false;
# endif
		usage();
		थांबा;

	    स्थिती 'H':		// "-H" start in Hebrew mode: rl + hkmap set
# ifdef FEAT_RIGHTLEFT
		p_hkmap = TRUE;
		set_option_मूल्य_give_err((char_u *)"rl", 1L, रिक्त, 0);
# अन्यथा
		mch_errmsg(_(e_hebrew_cannot_be_used_not_enabled_at_compile_time));
		mch_exit(2);
# endif
		थांबा;

	    स्थिती 'l':		// "-l" lisp mode, 'lisp' and 'showmatch' on
		set_option_मूल्य_give_err((char_u *)"lisp", 1L, रिक्त, 0);
		p_sm = TRUE;
		थांबा;

	    स्थिती 'M':		// "-M"  no changes or writing of files
		reset_modifiable();
		// FALLTHROUGH

	    स्थिती 'm':		// "-m"  no writing of files
		p_write = FALSE;
		थांबा;

	    स्थिती 'y':		// "-y"  easy mode
# ifdef FEAT_GUI
		gui.starting = true;	// start GUI a bit later
# endif
		parmp->evim_mode = TRUE;
		थांबा;

	    स्थिती 'N':		// "-N"  Nocompatible
		change_compatible(FALSE);
		थांबा;

	    स्थिती 'n':		// "-n" no swap file
# ifdef FEAT_NETBEANS_INTG
		// checking for "-nb", netbeans parameters
		जर (argv[0][argv_idx] == 'b')
		{
		    netbeansArg = argv[0];
		    argv_idx = -1;	    // skip to next argument
		}
		अन्यथा
# endif
		parmp->no_swap_file = TRUE;
		थांबा;

	    स्थिती 'p':		// "-p[N]" open N tab pages
# ifdef TARGET_API_MAC_OSX
		// For some reason on MacOS X, an argument like:
		// -psn_0_10223617 is passed in when invoke from Finder
		// or with the 'open' command
		जर (argv[0][argv_idx] == 's')
		{
		    argv_idx = -1; // bypass full -psn
		    main_start_gui();
		    थांबा;
		}
# endif
		// default is 0: open window for each file
		parmp->window_मोजणी = get_संख्या_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->window_layout = WIN_TABS;
		थांबा;

	    स्थिती 'o':		// "-o[N]" open N horizontal split windows
		// default is 0: open window for each file
		parmp->window_मोजणी = get_संख्या_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->window_layout = WIN_HOR;
		थांबा;

		स्थिती 'O':	// "-O[N]" open N vertical split windows
		// default is 0: open window for each file
		parmp->window_मोजणी = get_संख्या_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->window_layout = WIN_VER;
		थांबा;

# ifdef FEAT_QUICKFIX
	    स्थिती 'q':		// "-q" QuickFix mode
		जर (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
		parmp->edit_type = EDIT_QF;
		जर (argv[0][argv_idx])		// "-q{errorfile}"
		{
		    parmp->use_ef = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		अन्यथा जर (argc > 1)		// "-q {errorfile}"
		    want_argument = TRUE;
		थांबा;
# endif

	    स्थिती 'R':		// "-R" readonly mode
		readonlymode = TRUE;
		curbuf->b_p_ro = TRUE;
		p_uc = 10000;			// don't update very often
		थांबा;

	    स्थिती 'r':		// "-r" recovery mode
	    स्थिती 'L':		// "-L" recovery mode
		recoverymode = 1;
		थांबा;

	    स्थिती 's':
		जर (exmode_active)	// "-s" silent (batch) mode
		    silent_mode = TRUE;
		अन्यथा		// "-s {scriptin}" read from script file
		    want_argument = TRUE;
		थांबा;

	    स्थिती 't':		// "-t {tag}" or "-t{tag}" jump to tag
		जर (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
		parmp->edit_type = EDIT_TAG;
		जर (argv[0][argv_idx])		// "-t{tag}"
		{
		    parmp->tagname = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		अन्यथा				// "-t {tag}"
		    want_argument = TRUE;
		थांबा;

# ifdef FEAT_EVAL
	    स्थिती 'D':		// "-D"		Debugging
		parmp->use_debug_break_level = 9999;
		थांबा;
# endif
# ifdef FEAT_DIFF
	    स्थिती 'd':		// "-d"		'diff'
#  ifdef AMIGA
		// check for "-dev {device}"
		जर (argv[0][argv_idx] == 'e' && argv[0][argv_idx + 1] == 'v')
		    want_argument = TRUE;
		अन्यथा
#  endif
		    parmp->diff_mode = TRUE;
		थांबा;
# endif
	    स्थिती 'V':		// "-V{N}"	Verbose level
		// default is 10: a little bit verbose
		p_verbose = get_संख्या_arg((char_u *)argv[0], &argv_idx, 10);
		जर (argv[0][argv_idx] != NUL)
		{
		    set_option_मूल्य_give_err((char_u *)"verbosefile",
					  0L, (char_u *)argv[0] + argv_idx, 0);
		    argv_idx = (पूर्णांक)STRLEN(argv[0]);
		}
		थांबा;

	    स्थिती 'v':		// "-v"  Vi-mode (as if called "vi")
		exmode_active = 0;
# जर defined(FEAT_GUI) && !defined(VIMDLL)
		gui.starting = false;	// don't start GUI
# endif
		थांबा;

	    स्थिती 'w':		// "-w{number}"	set window height
				// "-w {scriptout}"	write to script
		जर (vim_isdigit(((char_u *)argv[0])[argv_idx]))
		{
		    n = get_संख्या_arg((char_u *)argv[0], &argv_idx, 10);
		    set_option_मूल्य_give_err((char_u *)"window", n, रिक्त, 0);
		    थांबा;
		}
		want_argument = TRUE;
		थांबा;

# ifdef FEAT_CRYPT
	    स्थिती 'x':		// "-x"  encrypted reading/writing of files
		parmp->ask_for_किल्ली = TRUE;
		थांबा;
# endif

	    स्थिती 'X':		// "-X"  don't connect to X server
# जर (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
		x_no_connect = TRUE;
# endif
		थांबा;
	    स्थिती 'Y':		// "-Y" don't connect to Wayland compositor
# जर defined(FEAT_WAYLAND)
		wayland_no_connect = TRUE;
# endif
		थांबा;

	    स्थिती 'Z':		// "-Z"  restricted mode
		restricted = TRUE;
		थांबा;

	    स्थिती 'c':		// "-c{command}" or "-c {command}" execute
				// command
		जर (argv[0][argv_idx] != NUL)
		{
		    जर (parmp->n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, रिक्त);
		    parmp->commands[parmp->n_commands++] = (char_u *)argv[0]
								   + argv_idx;
		    argv_idx = -1;
		    थांबा;
		}
		// FALLTHROUGH
	    स्थिती 'S':		// "-S {file}" execute Vim script
	    स्थिती 'i':		// "-i {viminfo}" use for viminfo
# ifndef FEAT_DIFF
	    स्थिती 'd':		// "-d {device}" device (for Amiga)
# endif
	    स्थिती 'T':		// "-T {terminal}" terminal name
	    स्थिती 'u':		// "-u {vimrc}" vim inits file
	    स्थिती 'U':		// "-U {gvimrc}" gvim inits file
	    स्थिती 'W':		// "-W {scriptout}" overwrite
# ifdef FEAT_GUI_MSWIN
	    स्थिती 'P':		// "-P {parent title}" MDI parent
# endif
		want_argument = TRUE;
		थांबा;

	    पूर्वनिर्धारित:
		mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
	    }

	    /*
	     * Handle option arguments with argument.
	     */
	    जर (want_argument)
	    {
		/*
		 * Check for garbage immediately after the option letter.
		 */
		जर (argv[0][argv_idx] != NUL)
		    mainerr(ME_GARBAGE, (char_u *)argv[0]);

		--argc;
		जर (argc < 1 && c != 'S')  // -S has an optional argument
		    mainerr_arg_missing((char_u *)argv[0]);
		++argv;
		argv_idx = -1;

		निवड (c)
		{
		स्थिती 'c':	// "-c {command}" execute command
		स्थिती 'S':	// "-S {file}" execute Vim script
		    जर (parmp->n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, रिक्त);
		    जर (c == 'S')
		    {
			अक्षर	*a;

			जर (argc < 1)
			    // "-S" without argument: use default session file
			    // name.
			    a = SESSION_FILE;
			अन्यथा जर (argv[0][0] == '-')
			{
			    // "-S" followed by another option: use default
			    // session file name.
			    a = SESSION_FILE;
			    ++argc;
			    --argv;
			}
			अन्यथा
			    a = argv[0];
			p = alloc(STRLEN(a) + 4);
			जर (p == रिक्त)
			    mch_exit(2);
			प्र_लिखो((अक्षर *)p, "so %s", a);
			parmp->cmds_tofree[parmp->n_commands] = TRUE;
			parmp->commands[parmp->n_commands++] = p;
		    }
		    अन्यथा
			parmp->commands[parmp->n_commands++] =
							    (char_u *)argv[0];
		    थांबा;

		स्थिती '-':
		    जर (argv[-1][2] == 'c')
		    {
			// "--cmd {command}" execute command
			जर (parmp->n_pre_commands >= MAX_ARG_CMDS)
			    mainerr(ME_EXTRA_CMD, रिक्त);
			parmp->pre_commands[parmp->n_pre_commands++] =
							    (char_u *)argv[0];
		    }
		    // --gui-dialog-file fname
		    जर (argv[-1][2] == 'g')
		    {
			// without GUI ignore the argument
# ifdef FEAT_GUI
			parmp->gui_dialog_file = (char_u *)argv[0];
# endif
		    }

		    // "--startuptime <file>" already handled
		    // "--log <file>" already handled
		    थांबा;

	    //	case 'd':   -d {device} is handled in mch_check_win() for the
	    //		    Amiga

# ifdef FEAT_QUICKFIX
		स्थिती 'q':	// "-q {errorfile}" QuickFix mode
		    parmp->use_ef = (char_u *)argv[0];
		    थांबा;
# endif

		स्थिती 'i':	// "-i {viminfo}" use for viminfo
		    set_option_मूल्य_give_err((char_u *)"vif",
						     0L, (char_u *)argv[0], 0);
		    थांबा;

		स्थिती 's':	// "-s {scriptin}" read from script file
		    जर (scriptin[0] != रिक्त)
		    {
scripterror:
			mch_errmsg(_("Attempt to open script file again: \""));
			mch_errmsg(argv[-1]);
			mch_errmsg(" ");
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    जर ((scriptin[0] = mch_fopen(argv[0], READBIN)) == रिक्त)
		    {
			mch_errmsg(_("Cannot open for reading: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    जर (save_typebuf() == FAIL)
			mch_exit(2);	// out of memory
		    थांबा;

		स्थिती 't':	// "-t {tag}"
		    parmp->tagname = (char_u *)argv[0];
		    थांबा;

		स्थिती 'T':	// "-T {terminal}" terminal name
		    /*
		     * The -T term argument is always available and when
		     * HAVE_TERMLIB is supported it overrides the environment
		     * variable TERM.
		     */
# ifdef FEAT_GUI
		    जर (term_is_gui((char_u *)argv[0]))
			gui.starting = true;	// start GUI a bit later
		    अन्यथा
# endif
			parmp->term = (char_u *)argv[0];
		    थांबा;

		स्थिती 'u':	// "-u {vimrc}" vim inits file
		    parmp->use_vimrc = (char_u *)argv[0];
		    थांबा;

		स्थिती 'U':	// "-U {gvimrc}" gvim inits file
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)argv[0];
# endif
		    थांबा;

		स्थिती 'w':	// "-w {nr}" 'window' value
				// "-w {scriptout}" append to script file
		    जर (vim_isdigit(*((char_u *)argv[0])))
		    {
			argv_idx = 0;
			n = get_संख्या_arg((char_u *)argv[0], &argv_idx, 10);
			set_option_मूल्य_give_err((char_u *)"window",
								   n, रिक्त, 0);
			argv_idx = -1;
			थांबा;
		    }
		    // FALLTHROUGH
		स्थिती 'W':	// "-W {scriptout}" overwrite script file
		    जर (scriptout != रिक्त)
			जा scripterror;
		    जर ((scriptout = mch_fopen(argv[0],
				    c == 'w' ? APPENDBIN : WRITEBIN)) == रिक्त)
		    {
			mch_errmsg(_("Cannot open for script output: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    थांबा;

# ifdef FEAT_GUI_MSWIN
		स्थिती 'P':		// "-P {parent title}" MDI parent
		    gui_mch_set_parent(argv[0]);
		    थांबा;
# endif
		}
	    }
	}

	/*
	 * File name argument.
	 */
	अन्यथा
	{
	    argv_idx = -1;	    // skip to next argument

	    // Check for only one type of editing.
	    जर (parmp->edit_type != EDIT_NONE && parmp->edit_type != EDIT_FILE)
		mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
	    parmp->edit_type = EDIT_FILE;

# ifdef MSWIN
	    // Remember if the argument was a full path before changing
	    // slashes to backslashes.
	    जर (argv[0][0] != NUL && argv[0][1] == ':' && argv[0][2] == '\\')
		parmp->full_path = TRUE;
# endif

	    // Add the file to the global argument list.
	    जर (ga_grow(&global_alist.al_ga, 1) == FAIL
		    || (p = vim_strsave((char_u *)argv[0])) == रिक्त)
		mch_exit(2);
# ifdef FEAT_DIFF
	    जर (parmp->diff_mode && mch_isdir(p) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *tail;
		string_T    ret;

		tail = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(p, STRLEN(p), tail, STRLEN(tail), TRUE, &ret);
		जर (ret.string != रिक्त)
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
	    जर (vim_strpbrk(p, "\\:") != रिक्त && !path_with_url(p))
	    {
		अक्षर posix_path[MAXPATHL];

#  जर CYGWIN_VERSION_DLL_MAJOR >= 1007
		cygwin_conv_path(CCP_WIN_A_TO_POSIX, p, posix_path, MAXPATHL);
#  अन्यथा
		cygwin_conv_to_posix_path(p, posix_path);
#  endif
		vim_free(p);
		p = vim_strsave((char_u *)posix_path);
		जर (p == रिक्त)
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
# अन्यथा
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
#  अन्यथा
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
	जर (argv_idx <= 0 || argv[0][argv_idx] == NUL)
	{
	    --argc;
	    ++argv;
	    argv_idx = 1;
	}
    }

# ifdef FEAT_EVAL
    // If there is a "+123" or "-c" command, set v:swapcommand to the first
    // one.
    जर (parmp->n_commands > 0)
    {
	माप_प्रकार  plen;

	plen = STRLEN(parmp->commands[0]) + 2;
	p = alloc(plen + 1);
	जर (p != रिक्त)
	{
	    प्र_लिखो((अक्षर *)p, ":%s\r", parmp->commands[0]);
	    set_vim_var_string(VV_SWAPCOMMAND, p, (पूर्णांक)plen);
	    vim_free(p);
	}
    }
# endif
}

/*
 * Print a warning if stdout is not a terminal.
 * When starting in Ex mode and commands come from a file, set silent_mode.
 */
    स्थिर शून्य
check_tty(mparm_T *parmp)
{
    पूर्णांक		आदान_isatty;		// is active input a terminal?

    आदान_isatty = mch_आदान_isatty();
    जर (exmode_active)
    {
	जर (!आदान_isatty)
	    silent_mode = TRUE;
    }
    अन्यथा जर (parmp->want_full_screen && (!stdout_isatty || !आदान_isatty)
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
	जर (netbeans_active() && (!stdout_isatty || !आदान_isatty))
	{
	    mch_errmsg(_("Vim: Error: Failure to start gvim from NetBeans\n"));
	    निकास(1);
	}
# endif
# जर defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	जर (
#  ifdef VIMDLL
	    !gui.starting &&
#  endif
	    is_cygpty_used())
	{
#  जर defined(HAVE_BIND_TEXTDOMAIN_CODESET) \
	&& defined(FEAT_GETTEXT)
	    अक्षर    *s, *tofree = रिक्त;

	    // Set the encoding of the error message based on $LC_ALL or
	    // other environment variables instead of 'encoding'.
	    // Note that the message is shown on a Cygwin terminal (e.g.
	    // mintty) which encoding is based on $LC_ALL or etc., not the
	    // current codepage used by normal Win32 console programs.
	    tofree = s = (अक्षर *)enc_locale_env(रिक्त);
	    जर (s == रिक्त)
		s = "utf-8";	// Use "utf-8" by default.
	    (शून्य)bind_textdomain_codeset(VIMPACKAGE, s);
	    vim_free(tofree);
#  endif
	    mch_errmsg(_("Vim: Error: This version of Vim does not run in a Cygwin terminal\n"));
	    निकास(1);
	}
# endif
	जर (!stdout_isatty)
	    mch_errmsg(_("Vim: Warning: Output is not to a terminal\n"));
	जर (!आदान_isatty)
	    mch_errmsg(_("Vim: Warning: Input is not from a terminal\n"));
	out_flush();
	जर (parmp->tty_fail && (!stdout_isatty || !आदान_isatty))
	    निकास(1);
	जर (scriptin[0] == रिक्त)
	    ui_delay(2005L, TRUE);
	TIME_संदेश("Warning delay");
    }
}

/*
 * Read text from stdin.
 */
    स्थिर शून्य
read_stdin(शून्य)
{
    पूर्णांक	    i;

    // When getting the ATTENTION prompt here, use a dialog
    swap_exists_action = SEA_DIALOG;

    no_wait_return = TRUE;
    i = संदेश_didany;
    set_buflisted(TRUE);

    // Create memfile and read from stdin.
    (शून्य)open_buffer(TRUE, रिक्त, 0);

    no_wait_return = FALSE;
    संदेश_didany = i;
    TIME_संदेश("reading stdin");

    check_swap_exists_action();

# जर !(defined(AMIGA) || defined(MACOS_X))
    // Dup stdin from stderr to read commands from, so that shell commands
    // work.
    // TODO: why is this needed, even though readfile() has done this?
    बंद(0);
    vim_ignored = dup(2);
# endif
}

/*
 * Create the requested number of windows and edit buffers in them.
 * Also does recovery if "recoverymode" set.
 */
    स्थिर शून्य
create_windows(mparm_T *parmp UNUSED)
{
    पूर्णांक		dorewind;
    पूर्णांक		done = 0;

    /*
     * Create the number of windows that was requested.
     */
    जर (parmp->window_मोजणी == -1)	// was not set
	parmp->window_मोजणी = 1;
    जर (parmp->window_मोजणी == 0)
	parmp->window_मोजणी = GARGCOUNT;
    जर (parmp->window_मोजणी > 1)
    {
	// Don't change the windows if there was a command in .vimrc that
	// already split some windows
	जर (parmp->window_layout == 0)
	    parmp->window_layout = WIN_HOR;
	जर (parmp->window_layout == WIN_TABS)
	{
	    parmp->window_मोजणी = make_tabpages(parmp->window_मोजणी);
	    TIME_संदेश("making tab pages");
	}
	अन्यथा जर (firstwin->w_next == रिक्त)
	{
	    parmp->window_मोजणी = make_windows(parmp->window_मोजणी,
					     parmp->window_layout == WIN_VER);
	    TIME_संदेश("making windows");
	}
	अन्यथा
	    parmp->window_मोजणी = win_मोजणी();
    }
    अन्यथा
	parmp->window_मोजणी = 1;

    जर (recoverymode)			// do recover
    {
	संदेश_scroll = TRUE;		// scroll message up
	ml_recover(TRUE);
	जर (curbuf->b_ml.ml_mfp == रिक्त) // failed
	    getout(1);
	do_modelines(0);		// do modelines
    }
    अन्यथा
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
	पर्यंत (done++ < 1000)
	{
	    जर (dorewind)
	    {
		जर (parmp->window_layout == WIN_TABS)
		    goto_tabpage(1);
		अन्यथा
		    curwin = firstwin;
	    }
	    अन्यथा जर (parmp->window_layout == WIN_TABS)
	    {
		जर (curtab->tp_next == रिक्त)
		    थांबा;
		goto_tabpage(0);
	    }
	    अन्यथा
	    {
		जर (curwin->w_next == रिक्त)
		    थांबा;
		curwin = curwin->w_next;
	    }
	    dorewind = FALSE;
	    curbuf = curwin->w_buffer;
	    जर (curbuf->b_ml.ml_mfp == रिक्त)
	    {
# ifdef FEAT_FOLDING
		// Set 'foldlevel' to 'foldlevelstart' if it's not negative.
		जर (p_fdls >= 0)
		    curwin->w_p_fdl = p_fdls;
# endif
		// When getting the ATTENTION prompt here, use a dialog
		swap_exists_action = SEA_DIALOG;

		set_buflisted(TRUE);

		// create memfile, read file
		(शून्य)open_buffer(FALSE, रिक्त, 0);

		जर (swap_exists_action == SEA_QUIT)
		{
		    जर (got_int || only_one_window())
		    {
			// abort selected or quit and only one window
			did_emsg = FALSE;   // avoid hit-enter prompt
			getout(1);
		    }
		    // We can't close the window, it would disturb what
		    // happens next.  Clear the file name and set the arg
		    // index to -1 to delete it later.
		    setfname(curbuf, रिक्त, रिक्त, FALSE);
		    curwin->w_arg_idx = -1;
		    swap_exists_action = SEA_NONE;
		}
		अन्यथा
		    handle_swap_exists(रिक्त);
		dorewind = TRUE;		// start again
	    }
	    ui_breakcheck();
	    जर (got_int)
	    {
		(शून्य)vgetc();	// only break the file loading, not the rest
		थांबा;
	    }
	}
	जर (parmp->window_layout == WIN_TABS)
	    goto_tabpage(1);
	अन्यथा
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
    स्थिर शून्य
edit_buffers(
    mparm_T	*parmp,
    char_u	*cwd)			// current working dir
{
    पूर्णांक		arg_idx;		// index in argument list
    पूर्णांक		i;
    पूर्णांक		advance = TRUE;
    win_T	*win;
    char_u	*p_shm_save = रिक्त;

    /*
     * Don't execute Win/Buf Enter/Leave autocommands here
     */
    ++autocmd_no_enter;
    ++autocmd_no_leave;

    // When w_arg_idx is -1 remove the window (see create_windows()).
    जर (curwin->w_arg_idx == -1)
    {
	win_close(curwin, TRUE);
	advance = FALSE;
    }

    arg_idx = 1;
    साठी (i = 1; i < parmp->window_मोजणी; ++i)
    {
	जर (cwd != रिक्त)
	    mch_chdir((अक्षर *)cwd);
	// When w_arg_idx is -1 remove the window (see create_windows()).
	जर (curwin->w_arg_idx == -1)
	{
	    ++arg_idx;
	    win_close(curwin, TRUE);
	    advance = FALSE;
	    सुरूठेवा;
	}

	जर (advance)
	{
	    जर (parmp->window_layout == WIN_TABS)
	    {
		जर (curtab->tp_next == रिक्त)	// just checking
		    थांबा;
		goto_tabpage(0);
		// Temporarily reset 'shm' option to not print fileinfo when
		// loading the other buffers. This would overwrite the already
		// existing fileinfo for the first tab.
		जर (i == 1)
		{
		    अक्षर buf[100];

		    p_shm_save = vim_strsave(p_shm);
		    vim_snprintf(buf, 100, "F%s", p_shm);
		    set_option_मूल्य_give_err((char_u *)"shm",
							 0L, (char_u *)buf, 0);
		}
	    }
	    अन्यथा
	    {
		जर (curwin->w_next == रिक्त)	// just checking
		    थांबा;
		win_enter(curwin->w_next, FALSE);
	    }
	}
	advance = TRUE;

	// Only open the file if there is no file in this window yet (that can
	// happen when .vimrc contains ":sall").
	जर (curbuf == firstwin->w_buffer || curbuf->b_ffname == रिक्त)
	{
	    curwin->w_arg_idx = arg_idx;
	    // Edit file from arg list, if there is one.  When "Quit" selected
	    // at the ATTENTION prompt close the window.
	    swap_exists_did_quit = FALSE;
	    (शून्य)do_ecmd(0, arg_idx < GARGCOUNT
			  ? alist_name(&GARGLIST[arg_idx]) : रिक्त,
			  रिक्त, रिक्त, ECMD_LASTL, ECMD_HIDE, curwin);
	    जर (swap_exists_did_quit)
	    {
		// abort or quit selected
		जर (got_int || only_one_window())
		{
		    // abort selected and only one window
		    did_emsg = FALSE;   // avoid hit-enter prompt
		    getout(1);
		}
		win_close(curwin, TRUE);
		advance = FALSE;
	    }
	    जर (arg_idx == GARGCOUNT - 1)
		arg_had_last = TRUE;
	    ++arg_idx;
	}
	ui_breakcheck();
	जर (got_int)
	{
	    (शून्य)vgetc();	// only break the file loading, not the rest
	    थांबा;
	}
    }

    जर (p_shm_save != रिक्त)
    {
	set_option_मूल्य_give_err((char_u *)"shm", 0L, p_shm_save, 0);
	vim_free(p_shm_save);
    }

    जर (parmp->window_layout == WIN_TABS)
	goto_tabpage(1);
    --autocmd_no_enter;

    // make the first window the current window
    win = firstwin;
# जर defined(FEAT_QUICKFIX)
    // Avoid making a preview window the current window.
    पर्यंत (win->w_p_pvw)
    {
	win = win->w_next;
	जर (win == रिक्त)
	{
	    win = firstwin;
	    थांबा;
	}
    }
# endif
    win_enter(win, FALSE);

    --autocmd_no_leave;
    TIME_संदेश("editing files in windows");
    जर (parmp->window_मोजणी > 1 && parmp->window_layout != WIN_TABS)
	win_equal(curwin, FALSE, 'b');	// adjust heights
}

/*
 * Execute the commands from --cmd arguments "cmds[cnt]".
 */
    स्थिर शून्य
exe_pre_commands(mparm_T *parmp)
{
    char_u	**cmds = parmp->pre_commands;
    पूर्णांक		cnt = parmp->n_pre_commands;
    पूर्णांक		i;
    ESTACK_CHECK_DECLARATION;

    जर (cnt <= 0)
	परत;

    curwin->w_cursor.lnum = 0; // just in case..
    estack_ढकला(ETYPE_ARGS, (char_u *)_("pre-vimrc command line"), 0);
    ESTACK_CHECK_SETUP;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CMDARG;
# endif
    साठी (i = 0; i < cnt; ++i)
	do_cmdline_cmd(cmds[i]);
    ESTACK_CHECK_NOW;
    estack_काढा();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    TIME_संदेश("--cmd commands");
}

/*
 * Execute "+", "-c" and "-S" arguments.
 */
    स्थिर शून्य
exe_commands(mparm_T *parmp)
{
    पूर्णांक		i;
    ESTACK_CHECK_DECLARATION;

    /*
     * We start commands on line 0, make "vim +/pat file" match a
     * pattern on line 1.  But don't move the cursor when an autocommand
     * with g`" was used.
     */
    संदेश_scroll = TRUE;
    जर (parmp->tagname == रिक्त && curwin->w_cursor.lnum <= 1)
	curwin->w_cursor.lnum = 0;
    estack_ढकला(ETYPE_ARGS, (char_u *)"command line", 0);
    ESTACK_CHECK_SETUP;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CARG;
    current_sctx.sc_seq = 0;
# endif
    साठी (i = 0; i < parmp->n_commands; ++i)
    {
	do_cmdline_cmd(parmp->commands[i]);
	जर (parmp->cmds_tofree[i])
	    vim_free(parmp->commands[i]);
    }
    ESTACK_CHECK_NOW;
    estack_काढा();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    जर (curwin->w_cursor.lnum == 0)
	curwin->w_cursor.lnum = 1;

    जर (!exmode_active)
	संदेश_scroll = FALSE;

# ifdef FEAT_QUICKFIX
    // When started with "-q errorfile" jump to first error again.
    जर (parmp->edit_type == EDIT_QF)
	qf_jump(रिक्त, 0, 0, FALSE);
# endif
    TIME_संदेश("executing command arguments");
}

/*
 * Source startup scripts.
 */
    स्थिर शून्य
source_startup_scripts(mparm_T *parmp)
{
    पूर्णांक		i;

    /*
     * For "evim" source evim.vim first of all, so that the user can overrule
     * any things he doesn't like.
     */
    जर (parmp->evim_mode)
    {
	(शून्य)do_source((char_u *)EVIM_FILE, FALSE, DOSO_NONE, रिक्त);
	TIME_संदेश("source evim file");
    }

    /*
     * If -u argument given, use only the initializations from that file and
     * nothing else.
     */
    जर (parmp->use_vimrc != रिक्त)
    {
	जर (STRCMP(parmp->use_vimrc, "DEFAULTS") == 0)
	{
	    जर (do_source((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE, रिक्त)
									 != OK)
		emsg(_(e_failed_to_source_defaults));
	}
	अन्यथा जर (STRCMP(parmp->use_vimrc, "NONE") == 0
				     || STRCMP(parmp->use_vimrc, "NORC") == 0)
	{
# ifdef FEAT_GUI
	    जर (use_gvimrc == रिक्त)	    // don't load gvimrc either
		use_gvimrc = parmp->use_vimrc;
# endif
	}
	अन्यथा
	{
	    जर (do_source(parmp->use_vimrc, FALSE, DOSO_NONE, रिक्त) != OK)
		semsg(_(e_cannot_read_from_str_2), parmp->use_vimrc);
	}
    }
    अन्यथा जर (!silent_mode)
    {
# ifdef AMIGA
	संरचना Process	*proc = (संरचना Process *)FindTask(0L);
	APTR		save_winptr = proc->pr_WindowPtr;

	// Avoid a requester here for a volume that doesn't exist.
	proc->pr_WindowPtr = (APTR)-1L;
# endif

	/*
	 * Get system wide defaults, if the file name is defined.
	 */
# ifdef SYS_VIMRC_FILE
	(शून्य)do_source((char_u *)SYS_VIMRC_FILE, FALSE, DOSO_NONE, रिक्त);
# endif
# ifdef MACOS_X
	(शून्य)do_source((char_u *)"$VIMRUNTIME/macmap.vim", FALSE,
							      DOSO_NONE, रिक्त);
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
	जर (process_env((char_u *)"VIMINIT", TRUE) != OK)
	{
	    जर (do_source((char_u *)USR_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, रिक्त) == FAIL
# ifdef USR_VIMRC_FILE2
		&& do_source((char_u *)USR_VIMRC_FILE2, TRUE,
						      DOSO_VIMRC, रिक्त) == FAIL
# endif
# ifdef XDG_VIMRC_FILE
		&& do_source((char_u *)XDG_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, रिक्त) == FAIL
# endif
# ifdef USR_VIMRC_FILE3
		&& do_source((char_u *)USR_VIMRC_FILE3, TRUE,
						      DOSO_VIMRC, रिक्त) == FAIL
# endif
# ifdef USR_VIMRC_FILE4
		&& do_source((char_u *)USR_VIMRC_FILE4, TRUE,
						      DOSO_VIMRC, रिक्त) == FAIL
# endif
		&& process_env((char_u *)"EXINIT", FALSE) == FAIL
		&& do_source((char_u *)USR_EXRC_FILE, FALSE,
						       DOSO_NONE, रिक्त) == FAIL
# ifdef USR_EXRC_FILE2
		&& do_source((char_u *)USR_EXRC_FILE2, FALSE,
						       DOSO_NONE, रिक्त) == FAIL
# endif
		&& !has_dash_c_arg)
	    {
		// When no .vimrc file was found: source defaults.vim.
		जर (do_source((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE,
								 रिक्त) == FAIL)
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
	जर (p_exrc)
	{
# जर defined(UNIX) || defined(VMS)
	    // If ".vimrc" file is not owned by user, set 'secure' mode.
	    जर (!file_owned(VIMRC_FILE))
# endif
		secure = p_secure;

	    i = FAIL;
	    जर (fullpathcmp((char_u *)USR_VIMRC_FILE,
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
		i = do_source((char_u *)VIMRC_FILE, TRUE, DOSO_VIMRC, रिक्त);

	    जर (i == FAIL)
	    {
# जर defined(UNIX) || defined(VMS)
		// if ".exrc" is not owned by user set 'secure' mode
		जर (!file_owned(EXRC_FILE))
		    secure = p_secure;
		अन्यथा
		    secure = 0;
# endif
		जर (	   fullpathcmp((char_u *)USR_EXRC_FILE,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# ifdef USR_EXRC_FILE2
			&& fullpathcmp((char_u *)USR_EXRC_FILE2,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# endif
				)
		    (शून्य)do_source((char_u *)EXRC_FILE, FALSE,
							      DOSO_NONE, रिक्त);
	    }
	}
	जर (secure == 2)
	    need_wait_return = TRUE;
	secure = 0;
# ifdef AMIGA
	proc->pr_WindowPtr = save_winptr;
# endif
    }
    TIME_संदेश("sourcing vimrc file(s)");
}

/*
 * Setup to start using the GUI.  Exit with an error when not available.
 */
    स्थिर शून्य
main_start_gui(शून्य)
{
# ifdef FEAT_GUI
    gui.starting = true;	// start GUI a bit later
# अन्यथा
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
    पूर्णांक
process_env(
    char_u	*env,
    पूर्णांक		is_viminit) // when TRUE, called for VIMINIT
{
    char_u	*initstr;
    sctx_T	save_current_sctx;
    ESTACK_CHECK_DECLARATION;

    जर ((initstr = mch_getenv(env)) == रिक्त || *initstr == NUL)
	परत FAIL;

    जर (is_viminit)
	vimrc_found(रिक्त, रिक्त);
    estack_ढकला(ETYPE_ENV, env, 0);
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
    estack_काढा();
    current_sctx = save_current_sctx;
    परत OK;
}

#जर (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
/*
 * Return TRUE if we are certain the user owns the file "fname".
 * Used for ".vimrc" and ".exrc".
 * Use both stat() and lstat() for extra security.
 */
    स्थिर पूर्णांक
file_owned(अक्षर *fname)
{
    stat_T	s;
# ifdef UNIX
    uid_t	uid = getuid();
# अन्यथा	 // VMS
    uid_t	uid = ((getgid() << 16) | getuid());
# endif

    परत !(mch_stat(fname, &s) != 0 || s.st_uid != uid
# ifdef HAVE_LSTAT
	    || mch_lstat(fname, &s) != 0 || s.st_uid != uid
# endif
	    );
}
#endif

/*
 * Give an error message main_errors["n"] and exit.
 */
    स्थिर शून्य
mainerr(
    पूर्णांक		n,	// one of the ME_ defines
    char_u	*str)	// extra argument or NULL
{
#जर defined(UNIX) || defined(VMS)
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
    जर (str != रिक्त)
    {
	mch_errmsg(": \"");
	mch_errmsg((अक्षर *)str);
	mch_errmsg("\"");
    }
    mch_errmsg(_("\nMore info with: \"vim -h\"\n"));

    mch_exit(1);
}

    शून्य
mainerr_arg_missing(char_u *str)
{
    mainerr(ME_ARG_MISSING, str);
}

#ifndef NO_VIM_MAIN
/*
 * print a message with three spaces prepended and '\n' appended.
 */
    स्थिर शून्य
main_संदेश(अक्षर *s)
{
    mch_संदेश("   ");
    mch_संदेश(s);
    mch_संदेश("\n");
}

/*
 * Print messages for "vim -h" or "vim --help" and exit.
 */
    स्थिर शून्य
usage(शून्य)
{
    पूर्णांक		i;
    स्थिर अक्षर	*(use[]) =
    {
	N_("[file ..]       edit specified file(s)"),
	N_("-               read text from stdin"),
	N_("-t tag          edit file where tag is defined"),
# ifdef FEAT_QUICKFIX
	N_("-q [errorfile]  edit file with first error")
# endif
    };

# जर defined(UNIX) || defined(VMS)
    reset_signals();		// kill us with CTRL-C here, if you like
# endif

    init_longVersion();
    mch_संदेश(longVersion);
    mch_संदेश(_("\n\nUsage:"));
    साठी (i = 0; ; ++i)
    {
	mch_संदेश(_(" vim [arguments] "));
	mch_संदेश(_(use[i]));
	जर (i == ARRAY_LENGTH(use) - 1)
	    थांबा;
	mch_संदेश(_("\n   or:"));
    }
# जर defined( VMS)
    mch_संदेश(_("\nWhere command is down-cased, prepend / (like: -/R) to treat flag as upper-case."));
    mch_संदेश(_("\nOr, where supported, SET PROC/PARSE=EXT, or else quote upper-case material."));
# endif /* defined( VMS) */

    mch_संदेश(_("\n\nArguments:\n"));
    main_संदेश(_("--\t\t\tOnly file names after this"));
# ifdef EXPAND_FILENAMES
    main_संदेश(_("--literal\t\tDon't expand wildcards"));
# endif
# ifdef FEAT_OLE
    main_संदेश(_("-register\t\tRegister this gvim for OLE"));
    main_संदेश(_("-unregister\t\tUnregister gvim for OLE"));
# endif
# ifdef FEAT_GUI
    main_संदेश(_("-g\t\t\tRun using GUI (like \"gvim\")"));
    main_संदेश(_("-f  or  --nofork\tForeground: Don't fork when starting GUI"));
# endif
    main_संदेश(_("-v\t\t\tVi mode (like \"vi\")"));
    main_संदेश(_("-e\t\t\tEx mode (like \"ex\")"));
    main_संदेश(_("-E\t\t\tImproved Ex mode"));
    main_संदेश(_("-s\t\t\tSilent (batch) mode (only for \"ex\")"));
# ifdef FEAT_DIFF
    main_संदेश(_("-d\t\t\tDiff mode (like \"vimdiff\")"));
# endif
    main_संदेश(_("-y\t\t\tEasy mode (like \"evim\", modeless)"));
    main_संदेश(_("-R\t\t\tReadonly mode (like \"view\")"));
    main_संदेश(_("-Z\t\t\tRestricted mode (like \"rvim\")"));
    main_संदेश(_("-m\t\t\tModifications (writing files) not allowed"));
    main_संदेश(_("-M\t\t\tModifications in text not allowed"));
    main_संदेश(_("-b\t\t\tBinary mode"));
    main_संदेश(_("-l\t\t\tLisp mode"));
    main_संदेश(_("-C\t\t\tCompatible with Vi: 'compatible'"));
    main_संदेश(_("-N\t\t\tNot fully Vi compatible: 'nocompatible'"));
    main_संदेश(_("-V[N][fname]\t\tBe verbose [level N] [log messages to fname]"));
# ifdef FEAT_EVAL
    main_संदेश(_("-D\t\t\tDebugging mode"));
# endif
    main_संदेश(_("-n\t\t\tNo swap file, use memory only"));
    main_संदेश(_("-r\t\t\tList swap files and exit"));
    main_संदेश(_("-r (with file name)\tRecover crashed session"));
    main_संदेश(_("-L\t\t\tSame as -r"));
# ifdef AMIGA
    main_संदेश(_("-f\t\t\tDon't use newcli to open window"));
    main_संदेश(_("-dev <device>\t\tUse <device> for I/O"));
# endif
# ifdef FEAT_ARABIC
    main_संदेश(_("-A\t\t\tStart in Arabic mode"));
# endif
# ifdef FEAT_RIGHTLEFT
    main_संदेश(_("-H\t\t\tStart in Hebrew mode"));
# endif
    main_संदेश(_("-T <terminal>\tSet terminal type to <terminal>"));
    main_संदेश(_("--not-a-term\t\tSkip warning for input/output not being a terminal"));
# ifdef FEAT_GUI
    main_संदेश(_("--gui-dialog-file {fname}  For testing: write dialog text"));
# endif
    main_संदेश(_("--ttyfail\t\tExit if input or output is not a terminal"));
# ifdef FEAT_JOB_CHANNEL
    main_संदेश(_("--stdio-channel\tUse stdin and stdout as a channel"));
# endif
    main_संदेश(_("-u <vimrc>\t\tUse <vimrc> instead of any .vimrc"));
# ifdef FEAT_GUI
    main_संदेश(_("-U <gvimrc>\t\tUse <gvimrc> instead of any .gvimrc"));
# endif
    main_संदेश(_("--noplugin\t\tDon't load plugin scripts"));
    main_संदेश(_("-p[N]\t\tOpen N tab pages (default: one for each file)"));
    main_संदेश(_("-o[N]\t\tOpen N windows (default: one for each file)"));
    main_संदेश(_("-O[N]\t\tLike -o but split vertically"));
    main_संदेश(_("+\t\t\tStart at end of file"));
    main_संदेश(_("+<lnum>\t\tStart at line <lnum>"));
    main_संदेश(_("--cmd <command>\tExecute <command> before loading any vimrc file"));
    main_संदेश(_("-c <command>\t\tExecute <command> after loading the first file"));
    main_संदेश(_("-S <session>\t\tSource file <session> after loading the first file"));
    main_संदेश(_("-s <scriptin>\tRead Normal mode commands from file <scriptin>"));
    main_संदेश(_("-w <scriptout>\tAppend all typed commands to file <scriptout>"));
    main_संदेश(_("-W <scriptout>\tWrite all typed commands to file <scriptout>"));
# ifdef FEAT_CRYPT
    main_संदेश(_("-x\t\t\tEdit encrypted files"));
# endif
# जर (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
#  जर defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
    main_संदेश(_("-display <display>\tConnect Vim to this particular X-server"));
#  endif
    main_संदेश(_("-X\t\t\tDo not connect to X server"));
# endif
# जर defined(FEAT_WAYLAND)
    main_संदेश(_("-Y\t\t\tDo not connect to Wayland compositor"));
# endif
# ifdef FEAT_CLIENTSERVER
#  ifdef FEAT_CLIENTSERVER_BACKENDS
    main_संदेश(_("--clientserver <socket|x11|mswin> Backend for clientserver communication"));
#  endif
    main_संदेश(_("--remote <files>\tEdit <files> in a Vim server if possible"));
    main_संदेश(_("--remote-silent <files>  Same, don't complain if there is no server"));
    main_संदेश(_("--remote-wait <files>  As --remote but wait for files to have been edited"));
    main_संदेश(_("--remote-wait-silent <files>  Same, don't complain if there is no server"));
    main_संदेश(_("--remote-tab[-wait][-silent] <files>  As --remote but use tab page per file"));
    main_संदेश(_("--remote-send <keys>\tSend <keys> to a Vim server and exit"));
    main_संदेश(_("--remote-expr <expr>\tEvaluate <expr> in a Vim server and print result"));
    main_संदेश(_("--serverlist\t\tList available Vim server names and exit"));
    main_संदेश(_("--servername <name>\tSend to/become the Vim server <name>"));
# endif
# ifdef STARTUPTIME
    main_संदेश(_("--startuptime <file>\tWrite startup timing messages to <file>"));
# endif
# ifdef FEAT_JOB_CHANNEL
    main_संदेश(_("--log <file>\t\tStart logging to <file> early"));
# endif
# ifdef FEAT_VIMINFO
    main_संदेश(_("-i <viminfo>\t\tUse <viminfo> instead of .viminfo"));
# endif
    main_संदेश(_("--clean\t\t'nocompatible', Vim defaults, no plugins, no viminfo"));
    main_संदेश(_("-h  or  --help\tPrint Help (this message) and exit"));
    main_संदेश(_("--version\t\tPrint version information and exit"));

# ifdef FEAT_GUI_X11
#  ifdef FEAT_GUI_MOTIF
    mch_संदेश(_("\nArguments recognised by gvim (Motif version):\n"));
#  endif
    main_संदेश(_("-display <display>\tRun Vim on <display>"));
    main_संदेश(_("-iconic\t\tStart Vim iconified"));
    main_संदेश(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_संदेश(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_संदेश(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_संदेश(_("-boldfont <font>\tUse <font> for bold text"));
    main_संदेश(_("-italicfont <font>\tUse <font> for italic text"));
    main_संदेश(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
    main_संदेश(_("-borderwidth <width>\tUse a border width of <width> (also: -bw)"));
    main_संदेश(_("-scrollbarwidth <width>  Use a scrollbar width of <width> (also: -sw)"));
    main_संदेश(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_संदेश(_("+reverse\t\tDon't use reverse video (also: +rv)"));
    main_संदेश(_("-xrm <resource>\tSet the specified resource"));
# endif // FEAT_GUI_X11
# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
    mch_संदेश(_("\nArguments recognised by gvim (GTK4 version):\n"));
#  अन्यथा
    mch_संदेश(_("\nArguments recognised by gvim (GTK+ version):\n"));
#  endif
    main_संदेश(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_संदेश(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_संदेश(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_संदेश(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
#  ifdef USE_GTK4
    main_संदेश("--prg-name <name>\tSet GTK program name");
#  अन्यथा
    main_संदेश(_("-iconic\t\tStart Vim iconified"));
    main_संदेश(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_संदेश(_("-display <display>\tRun Vim on <display> (also: --display)"));
    main_संदेश(_("--role <role>\tSet a unique role to identify the main window"));
    main_संदेश(_("--socketid <xid>\tOpen Vim inside another GTK widget"));
    main_संदेश(_("--echo-wid\t\tMake gvim echo the Window ID on stdout"));
#  endif
# endif
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
    जर (gui.starting)
#  endif
    {
	main_संदेश(_("-P <parent title>\tOpen Vim inside parent application"));
	main_संदेश(_("--windowid <HWND>\tOpen Vim inside another win32 widget"));
    }
# endif

# ifdef FEAT_GUI_GNOME
    // Gnome gives extra messages for --help if we continue, but not for -h.
    जर (gui.starting)
    {
	mch_संदेश("\n");
	gui.dofork = false;
    }
    अन्यथा
# endif
	mch_exit(0);
}

/*
 * Check the result of the ATTENTION dialog:
 * When "Quit" selected, exit Vim.
 * When "Recover" selected, recover the file.
 */
    स्थिर शून्य
check_swap_exists_action(शून्य)
{
    जर (swap_exists_action == SEA_QUIT)
	getout(1);
    handle_swap_exists(रिक्त);
}

#endif // NO_VIM_MAIN

#जर !defined(NO_VIM_MAIN) && defined(FEAT_EVAL)
    स्थिर शून्य
set_progpath(char_u *argv0)
{
    char_u *val = argv0;

# ifdef MSWIN
    // A relative path containing a "/" will become invalid when using ":cd",
    // turn it into a full path.
    // On MS-Windows "vim" should be expanded to "vim.exe", thus always do
    // this.
    char_u *path = रिक्त;

    जर (mch_can_exe(argv0, &path, FALSE) && path != रिक्त)
	val = path;
# अन्यथा
    char_u	buf[MAXPATHL + 1];
#  ifdef PROC_EXE_LINK
    अक्षर	linkbuf[MAXPATHL + 1];
    ssize_t	len;

    len = readlink(PROC_EXE_LINK, linkbuf, MAXPATHL);
    जर (len > 0)
    {
	linkbuf[len] = NUL;
	val = (char_u *)linkbuf;
    }
#  endif

    जर (!mch_isFullName(val))
    {
	जर (gettail(val) != val
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
