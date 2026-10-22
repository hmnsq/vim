/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#నిర్వచించు EXTERN
#అన్తర్భావ "vim.h"

#నిర్వచితమైతే __CYGWIN__
# include <cygwin/version.h>
# include <sys/cygwin.h>	// for cygwin_conv_to_posix_path() and/or
				// cygwin_conv_path()
# include <limits.h>
#అయితే_ముగింపు

#ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# include "iscygpty.h"
#అయితే_ముగింపు

// Values for edit_type.
#నిర్వచించు EDIT_NONE   0	    // no edit type yet
#నిర్వచించు EDIT_FILE   1	    // file name argument[s] given, use argument list
#నిర్వచించు EDIT_STDIN  2	    // read file from stdin
#నిర్వచించు EDIT_ట్యాగ్    3	    // tag name argument given, use tagname
#నిర్వచించు EDIT_QF	    4	    // start in quickfix mode

#ఒకవేళ (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
స్థిర పూర్ణాంకం file_owned(అక్షరం *fname);
#అయితే_ముగింపు
స్థిర శూన్య mainerr(పూర్ణాంకం, char_u *);
స్థిర శూన్య early_arg_పరిశీలన(mparm_T *parmp);
#నిర్వచితం_కాకపోతే NO_VIM_MAIN
స్థిర శూన్య usage(శూన్య);
స్థిర శూన్య parse_command_name(mparm_T *parmp);
స్థిర శూన్య command_పంక్తి_పరిశీలన(mparm_T *parmp);
స్థిర శూన్య check_tty(mparm_T *parmp);
స్థిర శూన్య read_stdin(శూన్య);
స్థిర శూన్య create_windows(mparm_T *parmp);
స్థిర శూన్య edit_buffers(mparm_T *parmp, char_u *cwd);
స్థిర శూన్య exe_pre_commands(mparm_T *parmp);
స్థిర శూన్య exe_commands(mparm_T *parmp);
స్థిర శూన్య ఆధారం_startup_scripts(mparm_T *parmp);
స్థిర శూన్య main_ప్రారంభం_gui(శూన్య);
స్థిర శూన్య check_మార్పిడి_exists_action(శూన్య);
# ifdef FEAT_EVAL
స్థిర శూన్య సమితి_progpath(char_u *argv0);
# endif
#అయితే_ముగింపు


/*
 * Different types of error messages.
 */
స్థిర అక్షరం *(main_errors[]) =
{
    N_("Unknown option argument"),
#నిర్వచించు ME_UNKNOWN_OPTION	0
    N_("Too many edit arguments"),
#నిర్వచించు ME_TOO_MANY_వాదనలు	1
    N_("Argument missing after"),
#నిర్వచించు ME_ARG_MISSING		2
    N_("Garbage after option argument"),
#నిర్వచించు ME_GARBAGE		3
    N_("Too many \"+command\", \"-c command\" or \"--cmd command\" arguments"),
#నిర్వచించు ME_EXTRA_CMD		4
    N_("Invalid argument for"),
#నిర్వచించు ME_INVALID_ARG		5
};

#నిర్వచితం_కాకపోతే PROTO		// don't want a prototype for main()

// Various parameters passed between main() and other functions.
స్థిర mparm_T	params;

# ifdef _IOLBF
స్థిర శూన్య *s_vbuf = ఖాళీ;		// buffer for setvbuf()
# endif

# ifndef NO_VIM_MAIN	// skip this for unittests

స్థిర char_u *ప్రారంభం_dir = ఖాళీ;	// current working dir on startup

స్థిర పూర్ణాంకం has_dash_c_arg = FALSE;

#  ifdef VIMDLL
__declspec(dllexport)
#  endif
    పూర్ణాంకం
#  ifdef MSWIN
VimMain
#  లేకపోతే
ప్రధాన
#  endif
(పూర్ణాంకం argc, అక్షరం **argv)
{
#  ఒకవేళ defined(STARTUPTIME) || defined(CLEAN_RUNTIMEPATH)
    పూర్ణాంకం		i;
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
    argc = get_cmd_వాదనలు_W(&argv);
#  endif

    // Many variables are in "params" so that we can pass them to invoked
    // functions without a lot of arguments.  "argc" and "argv" are also
    // copied, so that they can be changed.
    CLEAR_క్షేత్రం(params);
    params.argc = argc;
    params.argv = argv;
    params.want_నిండిన_screen = TRUE;
#  ifdef FEAT_EVAL
    params.use_debug_break_level = -1;
#  endif
    params.కిటికీ_లెక్క = -1;

    autocmd_init();

#  ifdef FEAT_RUBY
    {
	పూర్ణాంకం ruby_దొంతర_ప్రారంభం;
	vim_ruby_init((శూన్య *)&ruby_దొంతర_ప్రారంభం);
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

#  ఒకవేళ defined(STARTUPTIME) || defined(FEAT_పని_CHANNEL)
    // Need to find "--startuptime" and "--log" before actually parsing
    // arguments.
    కొరకు (i = 1; i < argc - 1; ++i)
    {
#   ifdef STARTUPTIME
	ఒకవేళ (STRICMP(argv[i], "--startuptime") == 0 && సమయం_fd == ఖాళీ)
	{
	    సమయం_fd = mch_fopen(argv[i + 1], "a");
	    సమయం_సందేశం("--- VIM STARTING ---");
	}
#   endif
#   ifdef FEAT_EVAL
	ఒకవేళ (STRICMP(argv[i], "--log") == 0)
	    ch_logfile((char_u *)(argv[i + 1]), (char_u *)"ao");
#   endif
    }
#  endif

#  ifdef CLEAN_RUNTIMEPATH
    // Need to find "--clean" before actually parsing arguments.
    కొరకు (i = 1; i < argc; ++i)
	ఒకవేళ (STRICMP(argv[i], "--clean") == 0)
	{
	    params.clean = TRUE;
	    ఆపు;
	}
#  endif
#  ifdef MSWIN
    // Need to find "-register" and "-unregister" before loading any libraries.
    కొరకు (i = 1; i < argc; ++i)
	ఒకవేళ ((STRICMP(argv[i] + 1, "register") == 0
				    || STRICMP(argv[i] + 1, "unregister") == 0)
		&& (argv[i][0] == '-' || argv[i][0] == '/'))
	{
	    దొరికింది_రిజిస్టరు_arg = TRUE;
	    ఆపు;
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
    command_పంక్తి_పరిశీలన(&params);
    సమయం_సందేశం("parsing arguments");

    /*
     * On some systems, when we compile with the GUI, we always use it.  On Mac
     * there is no terminal version, and on Windows we can't fork one off with
     * :gui.
     */
#  ifdef ALWAYS_USE_GUI
    gui.starting = true;
#  లేకపోతే
#   ఒకవేళ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    /*
     * Check if the GUI can be started.  Reset gui.starting if not.
     * Don't know about other systems, stay on the safe side and don't check.
     */
    ఒకవేళ (gui.starting)
    {
	ఒకవేళ (gui_init_check() == FAIL)
	{
	    gui.starting = false;

	    // When running "evim" or "gvim -y" we need the menus, exit if we
	    // don't have them.
	    ఒకవేళ (params.evim_బహుళకం)
		mch_exit(1);
	}
    }
#   endif
#  endif

    ఒకవేళ (GARGCOUNT > 0)
    {
#  ifdef EXPAND_FILENAMES
	/*
	 * Expand wildcards in file names.
	 */
	ఒకవేళ (!params.literal)
	{
	    ప్రారంభం_dir = alloc(MAXPATHL);
	    ఒకవేళ (ప్రారంభం_dir != ఖాళీ)
		mch_dirname(ప్రారంభం_dir, MAXPATHL);
	    // Temporarily add '(' and ')' to 'isfname'.  These are valid
	    // filename characters but are excluded from 'isfname' to make
	    // "gf" work on a file name in parentheses (e.g.: see vim.h).
	    do_cmdline_cmd((char_u *)":set isf+=(,)");
	    alist_expand(ఖాళీ, 0);
	    do_cmdline_cmd((char_u *)":set isf&");
	    ఒకవేళ (ప్రారంభం_dir != ఖాళీ)
		mch_chdir((అక్షరం *)ప్రారంభం_dir);
	}
#  endif
	params.fname = alist_name(&GARGLIST[0]);
    }

#  ifdef MSWIN
    {
	// Remember the number of entries in the argument list.  If it changes
	// we don't react on setting 'encoding'.
	సమితి_alist_లెక్క();
    }
#  endif

#  ifdef MSWIN
    ఒకవేళ (GARGCOUNT == 1 && params.నిండిన_మార్గం)
    {
	/*
	 * If there is one filename, fully qualified, we have very probably
	 * been invoked from explorer, so change to the file's directory.
	 * Hint: to avoid this when typing a command use a forward slash.
	 * If the cd fails, it doesn't matter.
	 */
	ఒకవేళ (vim_chdirfile(params.fname, "drop") == OK)
	    చివరి_chdir_reason = "drop";
	ఒకవేళ (ప్రారంభం_dir != ఖాళీ)
	    mch_dirname(ప్రారంభం_dir, MAXPATHL);
    }
#  endif
    సమయం_సందేశం("expanding arguments");

#  ifdef FEAT_DIFF
    ఒకవేళ (params.diff_బహుళకం && params.కిటికీ_లెక్క == -1)
	params.కిటికీ_లెక్క = 0;	// open up to 3 windows
#  endif

    // Don't redraw until much later.
    ++RedrawingDisabled;

    /*
     * When listing swap file names, don't do cursor positioning et. al.
     */
    ఒకవేళ (recoverymode && params.fname == ఖాళీ)
	params.want_నిండిన_screen = FALSE;

    /*
     * When certain to start the GUI, don't check terminal capabilities.
     * For GTK we can't be sure, but when started from the desktop it doesn't
     * make sense to try using a terminal.
     */
#  ఒకవేళ defined(ALWAYS_USE_GUI) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	|| defined(VIMDLL)
    ఒకవేళ (gui.starting
#   ifdef FEAT_GUI_GTK
	    && !isatty(2)
#   endif
	    )
	params.want_నిండిన_screen = FALSE;
#  endif

    /*
     * mch_init() sets up the terminal (window) for use.  This must be
     * done after resetting full_screen, otherwise it may move the cursor.
     * Note that we may use mch_exit() before mch_init()!
     */
    mch_init();
    సమయం_సందేశం("shell init");

#  ifdef USE_XSMP
    /*
     * For want of anywhere else to do it, try to connect to xsmp here.
     * Fitting it in after gui_mch_init, but before gui_init (via termcapinit).
     * Hijacking -X 'no X connection' to also disable XSMP connection as that
     * has a similar delay upon failure.
     * Only try if SESSION_MANAGER is set to something non-null.
     */
    ఒకవేళ (!x_no_connect)
    {
	అక్షరం *p = getenv("SESSION_MANAGER");

	ఒకవేళ (p != ఖాళీ && *p != NUL)
	{
	    xsmp_init();
	    సమయం_సందేశం("xsmp init");
	}
    }
#  endif

    /*
     * Print a warning if stdout is not a terminal.
     */
    check_tty(&params);

#  ifdef _IOLBF
    ఒకవేళ (silent_బహుళకం)
    {
	// Ensure output works usefully without a tty: buffer lines instead of
	// fully buffered.
	s_vbuf = జ్ఞాపకం_ఇవ్వు(BUFSIZ);
	ఒకవేళ (s_vbuf != ఖాళీ)
	    setvbuf(stdout, s_vbuf, _IOLBF, BUFSIZ);
    }
#  endif

    // This message comes before term inits, but after setting "silent_mode"
    // when the input is not a tty. Omit the message with --not-a-term.
    ఒకవేళ (GARGCOUNT > 1 && !silent_బహుళకం && !is_not_a_పదం())
	రాయి(_("%d files to edit\n"), GARGCOUNT);

    ఒకవేళ (params.want_నిండిన_screen && !silent_బహుళకం)
    {
	termcapinit(params.పదం);	// set terminal name and get terminal
				   // capabilities (will set full_screen)
	screen_ప్రారంభం();		// don't know where cursor is now
	సమయం_సందేశం("Termcap init");
    }

    /*
     * Set the default values for the options that use Rows and Columns.
     */
    ui_get_shellsize();		// inits Rows and Columns
    win_init_కొలత();
#  ifdef FEAT_DIFF
    // Set the 'diff' option now, so that it can be checked for in a .vimrc
    // file.  There is no buffer yet though.
    ఒకవేళ (params.diff_బహుళకం)
	diff_win_options(firstwin, FALSE);
#  endif

    cmdline_అడ్డువరుస = అడ్డువరుసలు - p_ch;
    cmdline_నిలువు_off = 0;
    cmdline_వెడల్పు = (పూర్ణాంకం)Columns;
    సందేశం_అడ్డువరుస = cmdline_అడ్డువరుస;
    సందేశం_నిలువు = 0;
    screenalloc(FALSE);		// allocate screen buffers
    సమితి_init_2();
    సమయం_సందేశం("inits 2");

    సందేశం_scroll = TRUE;
    no_wait_return = TRUE;

    init_mappings();		// set up initial mappings

    init_highlight(TRUE, FALSE); // set the default highlight groups
    సమయం_సందేశం("init highlight");

#  ఒకవేళ defined(FEAT_TERMRESPONSE)
    init_పదం_props(TRUE);
#  endif

#  ifdef FEAT_EVAL
    // Set the break level after the terminal is initialized.
    debug_break_level = params.use_debug_break_level;
#  endif

    // Reset 'loadplugins' for "-u NONE" before "--cmd" arguments.
    // Allows for setting 'loadplugins' there.
    ఒకవేళ (params.use_vimrc != ఖాళీ
	    && (STRCMP(params.use_vimrc, "NONE") == 0
		|| STRCMP(params.use_vimrc, "DEFAULTS") == 0))
	p_lpl = FALSE;

    // Execute --cmd arguments.
    exe_pre_commands(&params);

    // Source startup scripts.
    ఆధారం_startup_scripts(&params);

#  ifdef FEAT_EVAL
    సమితి_vim_var_nr(VV_VIM_DID_INIT, 1L);
#  endif

#  ifdef FEAT_MZSCHEME
    /*
     * Newer version of MzScheme (Racket) require earlier (trampolined)
     * initialisation via scheme_main_setup.
     * Implement this by initialising it as early as possible
     * and splitting off remaining Vim main into vim_main2().
     * Do source startup scripts, so that 'mzschemedll' can be set.
     */
    తిరిగి mzscheme_main();
#  లేకపోతే
    తిరిగి vim_main2();
#  endif
}
# endif // NO_VIM_MAIN
#అయితే_ముగింపు // PROTO

#ఒకవేళ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(FEAT_CLIPBOARD)
/*
 * Restore the state after a fatal X error.
 */
    స్థిర శూన్య
x_restore_state(శూన్య)
{
    State = బహుళకం_సాధారణ;
    VIsual_చురుకైన = FALSE;
    got_int = TRUE;
    need_wait_return = FALSE;
    global_busy = FALSE;
    exmode_చురుకైన = 0;
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
    scroll_ప్రారంభం();
    redraw_later_clear();
    ఎంచుకో_clipmethod();
}
#అయితే_ముగింపు

/*
 * vim_main2() is needed for FEAT_MZSCHEME, but we define it always to keep
 * things simple.
 * It is also defined when NO_VIM_MAIN is defined, but then it's empty.
 */
    పూర్ణాంకం
vim_main2(శూన్య)
{
#నిర్వచితం_కాకపోతే NO_VIM_MAIN
# ifdef FEAT_EVAL
    /*
     * Read all the plugin files.
     * Only when compiled with +eval, since most plugins need it.
     */
    ఒకవేళ (p_lpl)
    {
	char_u *rtp_copy = ఖాళీ;
	char_u *plugin_నమూనా = (char_u *)
#  ఒకవేళ defined(VMS) || defined(AMIGA) // VMS and Amiga don't handle the "**".
		"plugin/*.vim"
#  లేకపోతే
		"plugin/**/*.vim"
#  endif
		;

	// First add all package directories to 'runtimepath', so that their
	// autoload directories can be found.  Only if not done already with a
	// :packloadall command.
	// Make a copy of 'runtimepath', so that source_runtime does not use
	// the pack directories.
	ఒకవేళ (!did_ఆధారం_packages)
	{
	    rtp_copy = vim_strsave(p_rtp);
	    కూడు_pack_ప్రారంభం_dirs();
	}

	ఆధారం_లోపల_మార్గం(rtp_copy == ఖాళీ ? p_rtp : rtp_copy, plugin_నమూనా,
		DIP_ALL | DIP_NOAFTER, ఖాళీ);
	సమయం_సందేశం("loading plugins");
	vim_free(rtp_copy);

	// Only source "start" packages if not done already with a :packloadall
	// command.
	ఒకవేళ (!did_ఆధారం_packages)
	    ఎక్కించు_ప్రారంభం_packages();
	సమయం_సందేశం("loading packages");

	ఆధారం_runtime(plugin_నమూనా, DIP_ALL | DIP_AFTER);
	సమయం_సందేశం("loading after plugins");
    }
# endif

# ifdef FEAT_DIFF
    // Decide about window layout for diff mode after reading vimrc.
    ఒకవేళ (params.diff_బహుళకం && params.కిటికీ_layout == 0)
    {
	ఒకవేళ (diffopt_horizontal())
	    params.కిటికీ_layout = WIN_HOR;	// use horizontal split
	లేకపోతే
	    params.కిటికీ_layout = WIN_VER;	// use vertical split
    }
# endif

    /*
     * Recovery mode without a file name: List swap files.
     * This uses the 'dir' option, therefore it must be after the
     * initializations.
     */
    ఒకవేళ (recoverymode && params.fname == ఖాళీ)
    {
	recover_పేర్లు(ఖాళీ, TRUE, ఖాళీ, 0, ఖాళీ);
	mch_exit(0);
    }

    /*
     * Set a few option defaults after reading .vimrc files:
     * 'title' and 'icon', Unix: 'shellpipe' and 'shellredir'.
     */
    సమితి_init_3();
    సమయం_సందేశం("inits 3");

    /*
     * "-n" argument: Disable swap file by setting 'updatecount' to 0.
     * Note that this overrides anything from a vimrc file.
     */
    ఒకవేళ (params.no_మార్పిడి_file)
	p_uc = 0;

# ifdef FEAT_GUI
    ఒకవేళ (gui.starting)
    {
#  ఒకవేళ defined(UNIX) || defined(VMS)
	// When something caused a message from a vimrc script, need to output
	// an extra newline before the shell prompt.
	ఒకవేళ (did_emsg || సందేశం_didout)
	    అక్షరం_రాయి('\n');
#  endif

	gui_ప్రారంభం(ఖాళీ);		// will set full_screen to TRUE
	సమయం_సందేశం("starting GUI");

	// When running "evim" or "gvim -y" we need the menus, exit if we
	// don't have them.
	ఒకవేళ (!gui.లోపల_use && params.evim_బహుళకం)
	    mch_exit(1);
	firstwin->w_prev_ఎత్తు = firstwin->w_ఎత్తు; // may have changed
    }
# endif

# ifdef FEAT_VIMINFO
    /*
     * Read in registers, history etc, but not marks, from the viminfo file.
     * This is where v:oldfiles gets filled.
     */
    ఒకవేళ (*p_viminfo != NUL)
    {
	read_viminfo(ఖాళీ, VIF_WANT_INFO | VIF_GET_OLDFILES);
	సమయం_సందేశం("reading viminfo");
    }
# endif
# ifdef FEAT_EVAL
    // It's better to make v:oldfiles an empty list than NULL.
    ఒకవేళ (get_vim_var_list(VV_OLDFILES) == ఖాళీ)
	సమితి_vim_var_list(VV_OLDFILES, list_alloc());
# endif

# ifdef FEAT_QUICKFIX
    /*
     * "-q errorfile": Load the error file now.
     * If the error file can't be read, exit before doing anything else.
     */
    ఒకవేళ (params.edit_type == EDIT_QF)
    {
	char_u	*enc = ఖాళీ;

	enc = p_menc;
	ఒకవేళ (params.use_ef != ఖాళీ)
	    సమితి_string_option_ప్రత్యక్ష((char_u *)"ef", -1,
					   params.use_ef, OPT_FREE, SID_CARG);
	vim_snprintf((అక్షరం *)IObuff, IOSIZE, "cfile %s", p_ef);
	ఒకవేళ (qf_init(ఖాళీ, p_ef, p_efm, TRUE, IObuff, enc) < 0)
	{
	    బహిర్గతం_char('\n');
	    mch_exit(3);
	}
	సమయం_సందేశం("reading errorfile");
    }
# endif

    /*
     * Start putting things on the screen.
     * Scroll screen down before drawing over it
     * Clear screen now, so file message will not be cleared.
     */
    starting = NO_BUFFERS;
    no_wait_return = FALSE;
    ఒకవేళ (!exmode_చురుకైన)
	సందేశం_scroll = FALSE;

# ifdef FEAT_GUI
    /*
     * This seems to be required to make callbacks to be called now, instead
     * of after things have been put on the screen, which then may be deleted
     * when getting a resize callback.
     * For the Mac this handles putting files dropped on the Vim icon to
     * global_alist.
     */
    ఒకవేళ (gui.లోపల_use)
    {
	gui_wait_for_chars(50L, typebuf.tb_change_cnt);
	సమయం_సందేశం("GUI delay");
    }
# endif

# ఒకవేళ defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)
    qnx_clip_init();
# endif

# ఒకవేళ defined(MACOS_X) && defined(FEAT_CLIPBOARD)
    clip_init(TRUE);
# endif

# ifdef FEAT_XCLIPBOARD
    // Start using the X clipboard, unless the GUI was started.
#  ifdef FEAT_GUI
    ఒకవేళ (!gui.లోపల_use)
#  endif
    {
	అమరిక_పదం_clip();
	సమయం_సందేశం("setup x11 clipboard");
    }
# endif

# ifdef FEAT_CLIENTSERVER
    // Prepare for being a Vim server.
    prepare_server(&params);
# endif

# ifdef FEAT_WAYLAND
#  ifdef FEAT_GUI
    ఒకవేళ (!gui.లోపల_use)
#  endif
    {
	ఒకవేళ (wayland_init_connection(wayland_display_name) == OK)
	{
	    సమయం_సందేశం("connected to Wayland display");

#  ifdef FEAT_WAYLAND_CLIPBOARD
	    ఒకవేళ (clip_init_wayland() == OK)
		సమయం_సందేశం("setup Wayland clipboard");
#  endif
	}
    }
# endif

# ifdef HAVE_CLIPMETHOD
    ఎంచుకో_clipmethod();
# endif

    /*
     * If "-" argument given: Read file from stdin.
     * Do this before starting Raw mode, because it may change things that the
     * writing end of the pipe doesn't like, e.g., in case stdin and stderr
     * are the same terminal: "cat | vim -".
     * Using autocommands here may cause trouble...
     */
    ఒకవేళ (params.edit_type == EDIT_STDIN && !recoverymode)
	read_stdin();

# ఒకవేళ defined(UNIX) || defined(VMS)
    // When switching screens and something caused a message from a vimrc
    // script, need to output an extra newline on exit.
    ఒకవేళ ((did_emsg || సందేశం_didout) && *T_TI != NUL && params.edit_type != EDIT_STDIN)
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
    సమయం_సందేశం("setting raw mode");

    ఒకవేళ (need_wait_return || సందేశం_didany)
    {
	wait_return(TRUE);
	సమయం_సందేశం("waiting for return");
    }

    starttermcap();	    // start termcap if not done by wait_return()
    సమయం_సందేశం("start termcap");

    setmouse();				// may start using the mouse
    ఒకవేళ (scroll_region)
	scroll_region_reset();		// In case Rows changed
    scroll_ప్రారంభం();	// may scroll the screen to the right position

# ఒకవేళ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    పదం_నెట్టు_title(SAVE_RESTORE_BOTH);
# endif

    /*
     * Don't clear the screen when starting in Ex mode, unless using the GUI.
     */
    ఒకవేళ (exmode_చురుకైన
# ifdef FEAT_GUI
			&& !gui.లోపల_use
# endif
					)
	సమితి_must_redraw(UPD_CLEAR);
    లేకపోతే
    {
	screenclear();			// clear screen
	సమయం_సందేశం("clearing screen");
    }

# ifdef FEAT_CRYPT
    ఒకవేళ (params.ask_for_కీలకం)
    {
	crypt_check_current_పద్ధతి();
	(శూన్య)crypt_get_కీలకం(TRUE, TRUE);
	సమయం_సందేశం("getting crypt key");
    }
# endif

    no_wait_return = TRUE;

    /*
     * Create the requested number of windows and edit buffers in them.
     * Also does recovery if "recoverymode" set.
     */
    create_windows(&params);
    సమయం_సందేశం("opening buffers");

# ifdef FEAT_EVAL
    // clear v:swapcommand
    సమితి_vim_var_string(VV_SWAPCOMMAND, ఖాళీ, -1);
# endif

    // Ex starts at last line of the file
    ఒకవేళ (exmode_చురుకైన)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;

    apply_autocmds(EVENT_BUFENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
    సమయం_సందేశం("BufEnter autocommands");
    setpcmark();

# ifdef FEAT_QUICKFIX
    /*
     * When started with "-q errorfile" jump to first error now.
     */
    ఒకవేళ (params.edit_type == EDIT_QF)
    {
	qf_jump(ఖాళీ, 0, 0, FALSE);
	సమయం_సందేశం("jump to first error");
    }
# endif

    /*
     * If opened more than one window, start editing files in the other
     * windows.
     */
    edit_buffers(&params, ప్రారంభం_dir);
    vim_free(ప్రారంభం_dir);

# ifdef FEAT_DIFF
    ఒకవేళ (params.diff_బహుళకం)
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
    ఒకవేళ (params.tagname != ఖాళీ)
    {
	మార్పిడి_exists_did_quit = FALSE;

	vim_snprintf((అక్షరం *)IObuff, IOSIZE, "ta %s", params.tagname);
	do_cmdline_cmd(IObuff);
	సమయం_సందేశం("jumping to tag");

	// If the user doesn't want to edit the file then we quit here.
	ఒకవేళ (మార్పిడి_exists_did_quit)
	    getout(1);
    }

# ఒకవేళ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    // Temporarily set x_jump_env to here in case there is an X11 IO error,
    // because x_jump_env is only actually set in main_loop(), before
    // exe_commands(). May not be the best solution since commands passed via
    // the command line can be very broad like sourcing a file, in which case
    // an X IO error results in the command being partially done. In theory we
    // could use SETJMP in RealWaitForChar(), but the stack frame for that may
    // possibly exit and then LONGJMP is called on it.
    పూర్ణాంకం jump_ఫలితం = SETJMP(x_jump_env);

    ఒకవేళ (jump_ఫలితం == 0)
    {
# endif
	// Execute any "+", "-c" and "-S" arguments.
	ఒకవేళ (params.n_commands > 0)
	    exe_commands(&params);
# ఒకవేళ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    }
    లేకపోతే
	// Restore state and continue just like what main_loop() does.
	x_restore_state();
# endif

    // Must come before the may_req_ calls.
    starting = 0;

# ఒకవేళ defined(FEAT_TERMRESPONSE)
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
# ఒకవేళ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_కొలత();
# endif

    // start in insert mode
    ఒకవేళ (p_im)
	need_ప్రారంభం_insertmode = TRUE;

# ifdef FEAT_EVAL
    సమితి_vim_var_nr(VV_VIM_DID_ENTER, 1L);
# endif
    apply_autocmds(EVENT_VIMENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
    సమయం_సందేశం("VimEnter autocommands");

# ఒకవేళ defined(FEAT_EVAL) && defined(FEAT_CLIPBOARD)
    // Adjust default register name for "unnamed" in 'clipboard'. Can only be
    // done after the clipboard is available and all initial commands that may
    // modify the 'clipboard' setting have run; i.e. just before entering the
    // main loop.
    reset_reg_var();
# endif

# ఒకవేళ defined(FEAT_DIFF)
    // When a startup script or session file setup for diff'ing and
    // scrollbind, sync the scrollbind now.
    ఒకవేళ (curwin->w_p_diff && curwin->w_p_scb)
    {
	update_topline();
	check_scrollbind((linenr_T)0, 0L);
	సమయం_సందేశం("diff scrollbinding");
    }
# endif

# ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ఒకవేళ (!gui.లోపల_use)
#  endif
	mch_సమితి_winsize_now();	    // Allow winsize changes from now on
# endif

# ఒకవేళ defined(FEAT_GUI)
    // When tab pages were created, may need to update the tab pages line and
    // scrollbars.  This is skipped while creating them.
    ఒకవేళ (gui.లోపల_use && మొదటి_tabpage->tp_తదుపరి != ఖాళీ)
    {
	బహిర్గతం_flush();
	gui_init_which_components(ఖాళీ);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
# endif

    // If ":startinsert" command used, stuff a dummy command to be able to
    // call normal_cmd(), which will then start Insert mode.
    ఒకవేళ (restart_edit != 0)
	stuffcharReadbuff(K_NOP);

# ifdef FEAT_NETBEANS_INTG
    ఒకవేళ (netbeansArg != ఖాళీ && అక్షరశ్రేణి_పోలిక("-nb", netbeansArg, 3) == 0)
    {
#  ifdef FEAT_GUI
#   ఒకవేళ !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
	ఒకవేళ (gui.లోపల_use)
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

    సమయం_సందేశం("before starting main loop");

# ifdef FEAT_పని_CHANNEL
    ఒకవేళ (use_stdio_channel)
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

#అయితే_ముగింపు // NO_VIM_MAIN

    తిరిగి 0;
}

/*
 * Initialisation #1 shared by main() and some tests.
 */
    శూన్య
common_init_1(శూన్య)
{
    estack_init();
    cmdline_init();

    (శూన్య)mb_init();	// init mb_bytelen_tab[] to ones
#నిర్వచితమైతే FEAT_EVAL
    eval_init();	// init global variables
#అయితే_ముగింపు

#నిర్వచితమైతే __QNXNTO__
    qnx_init();		// PhAttach() for clipboard, (and gui)
#అయితే_ముగింపు

    /*
     * Allocate space for the generic buffers (needed for set_init_1() and
     * emsg()).
     */
    ఒకవేళ ((IObuff = alloc(IOSIZE)) == ఖాళీ
	    || (NameBuff = alloc(MAXPATHL)) == ఖాళీ)
	mch_exit(0);
    సమయం_సందేశం("Allocated generic buffers");
}


/*
 * Initialisation #2 shared by main() and some tests.
 */
    శూన్య
common_init_2(mparm_T *paramp)
{
#నిర్వచితమైతే NBDEBUG
    // Wait a moment for debugging NetBeans.  Must be after allocating
    // NameBuff.
    nbdebug_log_init("SPRO_GVIM_DEBUG", "SPRO_GVIM_DLEVEL");
    nbdebug_wait(WT_ENV | WT_WAIT | WT_STOP, "SPRO_GVIM_WAIT", 20);
    సమయం_సందేశం("NetBeans debug wait");
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    /*
     * Setup to use the current locale (for ctype() and many other things).
     * NOTE: Translated messages with encodings other than latin1 will not
     * work until set_init_1() has been called!
     */
    init_locale();
    సమయం_సందేశం("locale set");
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    gui.dofork = true;		    // default is to use fork()
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CLIENTSERVER_BACKENDS
    /*
     * Check the $VIM_CLIENTSERVER env before we handle the --clientserver arg
     */
    check_clientserver_పద్ధతి_env();
#అయితే_ముగింపు

    /*
     * Do a first scan of the arguments in "argv[]":
     *   -display or --display
     *   --server...
     *   --socketid
     *   --windowid
     */
    early_arg_పరిశీలన(paramp);

#ఒకవేళ defined(FEAT_GUI)
    // Prepare for possibly starting GUI sometime
    gui_prepare(&paramp->argc, paramp->argv);
    సమయం_సందేశం("GUI prepared");
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CLIPBOARD
    clip_init(FALSE);		// Initialise clipboard stuff
    సమయం_సందేశం("clipboard setup");
#అయితే_ముగింపు

    /*
     * Check if we have an interactive window.
     * On the Amiga: If there is no window, we open one with a newcli command
     * (needed for :! to * work). mch_check_win() will also handle the -d or
     * -dev argument.
     */
    stdout_isatty = (mch_check_win(paramp->argc, paramp->argv) != FAIL);
    సమయం_సందేశం("window checked");

    /*
     * Allocate the first window and buffer.
     * Can't do anything without it, exit when it fails.
     */
    ఒకవేళ (win_alloc_మొదటి() == FAIL)
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
    సమితి_init_1(paramp->clean);
    సమయం_సందేశం("inits 1");

#నిర్వచితమైతే FEAT_EVAL
    // set v:lang and v:ctype
    సమితి_lang_var();

    // set v:argv
    సమితి_argv_var(paramp->argv, paramp->argc);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SIGNS
    init_signs();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_QUICKFIX
    // initialize quickfix list
    // don't send an error message when memory allocation fails
    // do it when the user tries to access the quickfix list
    qf_init_దొంతర();
#అయితే_ముగింపు
}

/*
 * Return TRUE when the --not-a-term argument was found.
 */
    పూర్ణాంకం
is_not_a_పదం(శూన్య)
{
    తిరిగి params.not_a_పదం;
}

/*
 * Return TRUE when the --not-a-term argument was found or the GUI is in use.
 */
    పూర్ణాంకం
is_not_a_పదం_or_gui(శూన్య)
{
    తిరిగి params.not_a_పదం
#నిర్వచితమైతే FEAT_GUI
			    || gui.లోపల_use
#అయితే_ముగింపు
	;
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_vbuf(శూన్య)
{
# ifdef _IOLBF
    ఒకవేళ (s_vbuf != ఖాళీ)
    {
	setvbuf(stdout, ఖాళీ, _IONBF, 0);
	విడుదల(s_vbuf);
	s_vbuf = ఖాళీ;
    }
# endif
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI)
/*
 * If a --gui-dialog-file argument was given return the file name.
 * Otherwise return NULL.
 */
    char_u *
get_gui_dialog_file(శూన్య)
{
    తిరిగి params.gui_dialog_file;
}
#అయితే_ముగింపు

// When TRUE in a safe state when starting to wait for a character.
స్థిర పూర్ణాంకం	was_సురక్షిత = FALSE;
స్థిర oparg_T	*current_oap = ఖాళీ;

/*
 * Return TRUE if an operator was started but not finished yet.
 * Includes typing a count or a register name.
 */
    పూర్ణాంకం
op_pending(శూన్య)
{
    తిరిగి !(current_oap != ఖాళీ
	    && !ముగింపు_op
	    && current_oap->prev_opcount == 0
	    && current_oap->prev_count0 == 0
	    && current_oap->op_type == OP_NOP
	    && current_oap->regname == NUL);
}

/*
 * Return whether currently it is safe, assuming it was safe before (high level
 * state didn't change).
 */
    స్థిర పూర్ణాంకం
is_సురక్షిత_now(శూన్య)
{
    తిరిగి stuff_ఖాళీలు()
	&& typebuf.tb_len == 0
	&& scriptin[curscript] == ఖాళీ
#నిర్వచితమైతే FEAT_EVAL
	&& !debug_బహుళకం
#అయితే_ముగింపు
	&& !global_busy;
}

/*
 * Trigger SafeState if currently in a safe state, that is "safe" is TRUE and
 * there is no typeahead.
 */
    శూన్య
may_trigger_safestate(పూర్ణాంకం సురక్షిత)
{
    పూర్ణాంకం is_సురక్షిత = సురక్షిత && is_సురక్షిత_now();

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (was_సురక్షిత != is_సురక్షిత)
	// Only log when the state changes, otherwise it happens at nearly
	// every key stroke.
	ch_log(ఖాళీ, is_సురక్షిత ? "SafeState: Start triggering"
					       : "SafeState: Stop triggering");
#అయితే_ముగింపు
    ఒకవేళ (is_సురక్షిత)
	apply_autocmds(EVENT_SAFESTATE, ఖాళీ, ఖాళీ, FALSE, curbuf);
    was_సురక్షిత = is_సురక్షిత;
}

/*
 * Something changed which causes the state possibly to be unsafe, e.g. a
 * character was typed.  It will remain unsafe until the next call to
 * may_trigger_safestate().
 */
    శూన్య
state_no_longer_సురక్షిత(అక్షరం *reason వాడని)
{
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (was_సురక్షిత)
	ch_log(ఖాళీ, "SafeState: reset: %s", reason);
#అయితే_ముగింపు
    was_సురక్షిత = FALSE;
}

#ఒకవేళ defined(FEAT_EVAL) || defined(సందేశం_వరుస)
    పూర్ణాంకం
get_was_సురక్షిత_state(శూన్య)
{
    తిరిగి was_సురక్షిత;
}
#అయితే_ముగింపు

#ఒకవేళ defined(సందేశం_వరుస)
/*
 * Invoked when leaving code that invokes callbacks.  Then trigger
 * SafeStateAgain, if it was safe when starting to wait for a character.
 */
    శూన్య
may_trigger_safestateagain(శూన్య)
{
    ఒకవేళ (!was_సురక్షిత)
    {
	// If the safe state was reset in state_no_longer_safe(), e.g. because
	// of calling feedkeys(), we check if it's now safe again (all keys
	// were consumed).
	was_సురక్షిత = is_సురక్షిత_now();
# ifdef FEAT_EVAL
	ఒకవేళ (was_సురక్షిత)
	    ch_log(ఖాళీ, "SafeState: undo reset");
# endif
    }
    ఒకవేళ (was_సురక్షిత)
    {
# ifdef FEAT_EVAL
	// Only do this message when another message was given, otherwise we
	// get lots of them.
	ఒకవేళ ((did_repeated_సందేశం & REPEATED_సందేశం_SAFESTATE) == 0)
	{
	    పూర్ణాంకం did = did_repeated_సందేశం;

	    ch_log(ఖాళీ,
		      "SafeState: back to waiting, triggering SafeStateAgain");
	    did_repeated_సందేశం = did | REPEATED_సందేశం_SAFESTATE;
	}
# endif
	apply_autocmds(EVENT_SAFESTATEAGAIN, ఖాళీ, ఖాళీ, FALSE, curbuf);
    }
# ifdef FEAT_EVAL
    లేకపోతే
	ch_log(ఖాళీ,
		  "SafeState: back to waiting, not triggering SafeStateAgain");
# endif
}
#అయితే_ముగింపు

/*
 * Return TRUE if there is any typeahead, pending operator or command.
 */
    పూర్ణాంకం
work_pending(శూన్య)
{
    తిరిగి op_pending() || !is_సురక్షిత_now();
}

#నిర్వచితమైతే FEAT_CONCEAL
స్థిర linenr_T	conceal_old_cursor_పంక్తి = 0;
స్థిర linenr_T	conceal_new_cursor_పంక్తి = 0;
స్థిర పూర్ణాంకం	conceal_update_పంక్తులు = FALSE;
#అయితే_ముగింపు

/*
 * Trigger the events that are not triggered where they happen, but by
 * comparing the current state against the state stored when they were last
 * triggered. Also updates what depends on the cursor having moved, which is
 * not affected by 'eventignore'.
 * Called from the main loop and when 'eventignore(win)' changes, so that what
 * happened with an event ignored is not reported once it is not ignored
 * anymore.
 */
    శూన్య
may_trigger_deferred_events(శూన్య)
{
    స్థిర bool	recursive = false;

    ఒకవేళ (recursive)
	తిరిగి;
    recursive = true;

#నిర్వచితమైతే FEAT_CONCEAL
    ఒకవేళ (curwin->w_p_cole == 0)
	conceal_update_పంక్తులు = FALSE;
#అయితే_ముగింపు

    // Trigger CursorMoved if the cursor moved.
    ఒకవేళ (!ముగింపు_op && (has_cursormoved()
#నిర్వచితమైతే FEAT_PROP_POPUP
		|| popup_visible
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
		|| curwin->w_p_cole > 0
#అయితే_ముగింపు
		) && !EQUAL_POS(చివరి_cursormoved, curwin->w_cursor))
    {
	ఒకవేళ (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, ఖాళీ, ఖాళీ, FALSE, curbuf);
#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ (popup_visible)
	    popup_check_cursor_pos();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
	ఒకవేళ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_పంక్తి = చివరి_cursormoved.lnum;
	    conceal_new_cursor_పంక్తి = curwin->w_cursor.lnum;
	    conceal_update_పంక్తులు = TRUE;
	}
#అయితే_ముగింపు
	చివరి_cursormoved = curwin->w_cursor;
    }

#నిర్వచితమైతే FEAT_CONCEAL
    ఒకవేళ (conceal_update_పంక్తులు
	    && (conceal_old_cursor_పంక్తి != conceal_new_cursor_పంక్తి
		|| conceal_cursor_పంక్తి(curwin)
		|| need_cursor_పంక్తి_redraw))
    {
	ఒకవేళ (conceal_old_cursor_పంక్తి != conceal_new_cursor_పంక్తి
		&& conceal_old_cursor_పంక్తి != 0
		&& conceal_old_cursor_పంక్తి <= curbuf->b_ml.ml_పంక్తి_లెక్క)
	    redrawWinline(curwin, conceal_old_cursor_పంక్తి);
	redrawWinline(curwin, conceal_new_cursor_పంక్తి);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_పంక్తి_redraw = FALSE;
    }
#అయితే_ముగింపు

    // Trigger TextChanged if b:changedtick differs.
    ఒకవేళ (!ముగింపు_op && has_textchanged()
	    && curbuf->b_చివరి_changedtick != CHANGEDTICK(curbuf))
    {
	apply_autocmds(EVENT_TEXTCHANGED, ఖాళీ, ఖాళీ, FALSE, curbuf);
	curbuf->b_చివరి_changedtick = CHANGEDTICK(curbuf);
    }

    // Ensure curwin->w_topline and curwin->w_leftcol are up to date before
    // triggering a WinScrolled autocommand.
    update_topline();
    validate_cursor();

    ఒకవేళ (!ముగింపు_op)
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
    శూన్య
main_loop(
    పూర్ణాంకం		cmdwin,	    // TRUE when working in the command-line window
    పూర్ణాంకం		noexmode)   // TRUE when return on entering Ex mode
{
    oparg_T	oa;		// operator arguments
    oparg_T	*prev_oap;	// operator arguments
    అస్థిర పూర్ణాంకం మునుపటి_got_int = FALSE;	// "got_int" was TRUE

    prev_oap = current_oap;
    current_oap = &oa;

#ఒకవేళ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    // Setup to catch a terminating error from the X server.  Just ignore
    // it, restore the state and continue.  This might not always work
    // properly, but at least we hopefully don't exit unexpectedly when the X
    // server exits while Vim is running in a console.
    ఒకవేళ (!cmdwin && !noexmode && SETJMP(x_jump_env))
	x_restore_state();
#అయితే_ముగింపు

    clear_oparg(&oa);
    అయితే (!cmdwin || cmdwin_ఫలితం == 0)
    {
	ఒకవేళ (stuff_ఖాళీలు())
	{
	    did_check_timestamps = FALSE;
	    ఒకవేళ (need_check_timestamps)
		check_timestamps(FALSE);
	    ఒకవేళ (need_wait_return)	// if wait_return() still needed ...
		wait_return(FALSE);	// ... call it now
	    ఒకవేళ (need_ప్రారంభం_insertmode && goto_im() && !VIsual_చురుకైన)
	    {
		need_ప్రారంభం_insertmode = FALSE;
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
	ఒకవేళ (got_int)
	{
	    ఒకవేళ (noexmode && global_busy && !exmode_చురుకైన && మునుపటి_got_int)
	    {
		// Typed two CTRL-C in a row: go back to ex mode as if "Q" was
		// used and keep "got_int" set, so that it aborts ":g".
		exmode_చురుకైన = EXMODE_సాధారణ;
		State = బహుళకం_సాధారణ;
	    }
	    లేకపోతే ఒకవేళ (!global_busy || !exmode_చురుకైన)
	    {
		ఒకవేళ (!quit_మరిన్ని)
		    (శూన్య)vgetc();		// flush all buffers
		got_int = FALSE;
	    }
	    మునుపటి_got_int = TRUE;
	}
	లేకపోతే
	    మునుపటి_got_int = FALSE;

#నిర్వచితమైతే FEAT_EVAL
	// At the toplevel there is no exception handling.  Discard any that
	// may be hanging around (e.g. from "interrupt" at the debug prompt).
	ఒకవేళ (did_throw && !ex_సాధారణ_busy)
	    discard_current_exception();
#అయితే_ముగింపు

	ఒకవేళ (!exmode_చురుకైన)
	    సందేశం_scroll = FALSE;
	quit_మరిన్ని = FALSE;

	// it's not safe unless may_trigger_safestate_main() is called
	was_సురక్షిత = FALSE;

	/*
	 * If skip redraw is set (for ":" in wait_return()), don't redraw now.
	 * If there is nothing in the stuff_buffer or do_redraw is TRUE,
	 * update cursor and redraw.
	 */
	ఒకవేళ (skip_redraw || exmode_చురుకైన)
	{
	    skip_redraw = FALSE;
	    setcursor();
	    cursor_on();
	}
	లేకపోతే ఒకవేళ (do_redraw || stuff_ఖాళీలు())
	{
#నిర్వచితమైతే FEAT_GUI
	    // If ui_breakcheck() was used a resize may have been postponed.
	    gui_may_resize_shell();
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_DROP_FILE
	    // If files were dropped while text was locked or the curbuf was
	    // locked, this would be a good time to handle the drop.
	    handle_any_postponed_drop();
#అయితే_ముగింపు
	    may_trigger_deferred_events();

	    // If nothing is pending and we are going to wait for the user to
	    // type a character, trigger SafeState.
	    may_trigger_safestate(!op_pending() && restart_edit == 0);

#ఒకవేళ defined(FEAT_DIFF)
	    // Updating diffs from changed() does not always work properly,
	    // esp. updating folds.  Do an update just before redrawing if
	    // needed.
	    ఒకవేళ (curtab->tp_diff_update || curtab->tp_diff_invalid)
	    {
		ex_diffupdate(ఖాళీ);
		curtab->tp_diff_update = FALSE;
	    }

	    // Scroll-binding for diff mode may have been postponed until
	    // here.  Avoids doing it for every change.
	    ఒకవేళ (diff_need_scrollbind)
	    {
		check_scrollbind((linenr_T)0, 0L);
		diff_need_scrollbind = FALSE;
	    }
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_FOLDING)
	    // Include a closed fold completely in the Visual area.
	    foldAdjustVisual();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
	    /*
	     * When 'foldclose' is set, apply 'foldlevel' to folds that don't
	     * contain the cursor.
	     * When 'foldopen' is "all", open the fold(s) under the cursor.
	     * This may mark the window for redrawing.
	     */
	    ఒకవేళ (hasAnyFolding(curwin) && !char_avail())
	    {
		foldCheckClose();
		ఒకవేళ (fdo_flags & FDO_ALL)
		    foldOpenCursor();
	    }
#అయితే_ముగింపు

	    // Before redrawing, make sure w_topline is correct, and w_leftcol
	    // if lines don't wrap, and w_skipcol if lines wrap.
	    update_topline();
	    validate_cursor();

	    ఒకవేళ (VIsual_చురుకైన)
		update_curbuf(UPD_INVERTED); // update inverted part
	    లేకపోతే ఒకవేళ (must_redraw)
	    {
		mch_disable_flush();	// Stop issuing gui_mch_flush().
		update_screen(0);
		mch_enable_flush();
	    }
	    లేకపోతే ఒకవేళ (redraw_cmdline || clear_cmdline || redraw_బహుళకం)
		showmode();
	    redraw_statuslines();
	    ఒకవేళ (need_maketitle)
		maketitle();
#నిర్వచితమైతే FEAT_VIMINFO
	    curbuf->b_చివరి_వాడిన = vim_సమయం();
#అయితే_ముగింపు
	    // display message after redraw
	    ఒకవేళ (keep_సందేశం != ఖాళీ)
	    {
		char_u *p = vim_strsave(keep_సందేశం);

		ఒకవేళ (p != ఖాళీ)
		{
		    // msg_start() will set keep_msg to NULL, make a copy
		    // first.  Don't reset keep_msg, msg_attr_keep() uses it to
		    // check for duplicates.  Never put this message in
		    // history.
		    సందేశం_hist_off = TRUE;
		    సందేశం_attr((అక్షరం *)p, keep_సందేశం_attr);
		    సందేశం_hist_off = FALSE;
		    vim_free(p);
		}
	    }
	    ఒకవేళ (need_fileinfo)		// show file info after redraw
	    {
		fileinfo(FALSE, TRUE, FALSE);
		need_fileinfo = FALSE;
	    }

	    emsg_on_display = FALSE;	// can delete error message now
	    did_emsg = FALSE;
	    సందేశం_didany = FALSE;		// reset lines_left in msg_start()
	    may_clear_sb_పాఠ్యం();	// clear scroll-back text on next msg
	    showruler(FALSE);

	    setcursor();
	    cursor_on();

	    do_redraw = FALSE;

#నిర్వచితమైతే STARTUPTIME
	    // Now that we have drawn the first screen all the startup stuff
	    // has been done, close any file for startup messages.
	    ఒకవేళ (సమయం_fd != ఖాళీ)
	    {
		సమయం_సందేశం("first screen update");
		సమయం_సందేశం("--- VIM STARTED ---");
		fclose(సమయం_fd);
		సమయం_fd = ఖాళీ;
	    }
#అయితే_ముగింపు
	    // After the first screen update may start triggering WinScrolled
	    // autocmd events.  Store all the scroll positions and sizes now.
	    may_make_initial_scroll_కొలత_snapshot();
	}
#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (need_mouse_correct)
	    gui_mouse_correct();
#అయితే_ముగింపు

	// May request the keyboard protocol state now.
	may_send_t_RK();

	// Update w_curswant if w_set_curswant has been set.
	// Postponed until here to avoid computing w_virtcol too often.
	update_curswant();

#నిర్వచితమైతే FEAT_EVAL
	/*
	 * May perform garbage collection when waiting for a character, but
	 * only at the very toplevel.  Otherwise we may be using a List or
	 * Dict internally somewhere.
	 * "may_garbage_collect" is reset in vgetc() which is invoked through
	 * do_exmode() and normal_cmd().
	 */
	may_garbage_collect = (!cmdwin && !noexmode);
#అయితే_ముగింపు
	/*
	 * If we're invoked as ex, do a round of ex commands.
	 * Otherwise, get and execute a normal mode command.
	 */
	ఒకవేళ (exmode_చురుకైన)
	{
	    ఒకవేళ (noexmode)   // End of ":global/path/visual" commands
		వెళ్ళు theend;
	    do_exmode(exmode_చురుకైన == EXMODE_VIM);
	}
	లేకపోతే
	{
#నిర్వచితమైతే FEAT_TERMINAL
	    ఒకవేళ (పదం_use_loop()
		    && oa.op_type == OP_NOP && oa.regname == NUL
		    && !VIsual_చురుకైన
		    && !skip_పదం_loop)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal mode.  With FAIL we first need to position the
		// cursor and the screen needs to be redrawn.
		ఒకవేళ (terminal_loop(TRUE) == OK)
		    సాధారణ_cmd(&oa, TRUE);
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
#నిర్వచితమైతే FEAT_TERMINAL
		skip_పదం_loop = FALSE;
#అయితే_ముగింపు
		సాధారణ_cmd(&oa, TRUE);
	    }
	}
    }

theend:
    current_oap = prev_oap;
}


#ఒకవేళ defined(USE_XSMP) || defined(FEAT_GUI)
/*
 * Exit, but leave behind swap files for modified buffers.
 */
    శూన్య
getout_preserve_modified(పూర్ణాంకం exitval)
{
# ఒకవేళ defined(SIGHUP) && defined(SIG_IGN)
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
#అయితే_ముగింపు


/*
 * Exit properly.  This is the only way to exit Vim after startup has
 * succeeded.  We are certain to exit here, no way to abort it.
 */
    శూన్య
getout(పూర్ణాంకం exitval)
{
    exiting = TRUE;
#ఒకవేళ defined(FEAT_EVAL)
    ch_log(ఖాళీ, "Exiting...");
#అయితే_ముగింపు

    // When running in Ex mode an error causes us to exit with a non-zero exit
    // code.  POSIX requires this, although it's not 100% clear from the
    // standard.
    ఒకవేళ (exmode_చురుకైన)
	exitval += ex_exitval;

#నిర్వచితమైతే FEAT_EVAL
    సమితి_vim_var_type(VV_EXITING, VAR_సంఖ్య);
    సమితి_vim_var_nr(VV_EXITING, exitval);
#అయితే_ముగింపు

    // Position the cursor on the last screen line, below all the text
    ఒకవేళ (!is_not_a_పదం_or_gui())
	windgoto((పూర్ణాంకం)అడ్డువరుసలు - 1, 0);

#నిర్వచితమైతే FEAT_EVAL
    // Invoked all deferred functions in the function stack.
    invoke_all_defer();
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
    // Optionally print hashtable efficiency.
    hash_debug_results();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    సందేశం_didany = FALSE;
#అయితే_ముగింపు

    ఒకవేళ (v_dying <= 1)
    {
	tabpage_T	*tp;
	tabpage_T	*తదుపరి_tp;
	buf_T		*buf;
	win_T		*wp;
	పూర్ణాంకం		unblock = 0;

	// Trigger BufWinLeave for all windows, but only once per buffer.
	కొరకు (tp = మొదటి_tabpage; tp != ఖాళీ; tp = తదుపరి_tp)
	{
	    తదుపరి_tp = tp->tp_తదుపరి;
	    FOR_ALL_WINDOWS_లోపల_TAB(tp, wp)
	    {
		ఒకవేళ (wp->w_బఫరు == ఖాళీ || !buf_valid(wp->w_బఫరు))
		    // Autocmd must have close the buffer already, skip.
		    కొనసాగు;
		buf = wp->w_బఫరు;
		ఒకవేళ (CHANGEDTICK(buf) != -1)
		{
		    bufref_T bufref;

		    సమితి_bufref(&bufref, buf);
		    apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname,
						    buf->b_fname, FALSE, buf);
		    ఒకవేళ (bufref_valid(&bufref))
			CHANGEDTICK(buf) = -1;  // note we did it already

		    // start all over, autocommands may mess up the lists
		    తదుపరి_tp = మొదటి_tabpage;
		    ఆపు;
		}
	    }
	}

	// Trigger BufUnload for buffers that are loaded
	FOR_ALL_BUFFERS(buf)
	    ఒకవేళ (buf->b_ml.ml_mfp != ఖాళీ)
	    {
		bufref_T bufref;

		సమితి_bufref(&bufref, buf);
		apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf);
		ఒకవేళ (!bufref_valid(&bufref))
		    // autocmd deleted the buffer
		    ఆపు;
	    }

	// deathtrap() blocks autocommands, but we do want to trigger
	// VimLeavePre.
	ఒకవేళ (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVEPRE, ఖాళీ, ఖాళీ, FALSE, curbuf);
	ఒకవేళ (unblock)
	    ఖండం_autocmds();
    }

#నిర్వచితమైతే FEAT_VIMINFO
    ఒకవేళ (
# ifdef EXITFREE
	    entered_free_all_mem == FALSE &&
# endif
	    *p_viminfo != NUL)
	// Write out the registers, history, marks etc, to the viminfo file
	write_viminfo(ఖాళీ, FALSE);
#అయితే_ముగింపు

    ఒకవేళ (v_dying <= 1)
    {
	పూర్ణాంకం	unblock = 0;

	// deathtrap() blocks autocommands, but we do want to trigger VimLeave.
	ఒకవేళ (is_autocmd_blocked())
	{
	    unblock_autocmds();
	    ++unblock;
	}
	apply_autocmds(EVENT_VIMLEAVE, ఖాళీ, ఖాళీ, FALSE, curbuf);
	ఒకవేళ (unblock)
	    ఖండం_autocmds();
    }

#నిర్వచితమైతే FEAT_PROFILE
    profile_dump();
#అయితే_ముగింపు

    ఒకవేళ (did_emsg
#నిర్వచితమైతే FEAT_GUI
	    || (gui.లోపల_use && సందేశం_didany && p_verbose > 0)
#అయితే_ముగింపు
	    )
    {
	// give the user a chance to read the (error) message
	no_wait_return = FALSE;
	wait_return(FALSE);
    }

    // Position the cursor again, the autocommands may have moved it
    ఒకవేళ (!is_not_a_పదం_or_gui())
	windgoto((పూర్ణాంకం)అడ్డువరుసలు - 1, 0);

#నిర్వచితమైతే FEAT_పని_CHANNEL
    పని_stop_on_exit();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LUA
    lua_చివర();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MZSCHEME
    mzscheme_చివర();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TCL
    tcl_చివర();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RUBY
    ruby_చివర();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PYTHON
    python_చివర();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PYTHON3
    python3_చివర();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PERL
    perl_చివర();
#అయితే_ముగింపు
#ఒకవేళ defined(USE_ICONV) && defined(DYNAMIC_ICONV)
    iconv_చివర();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
    netbeans_చివర();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SOCKETSERVER
    socketserver_stop();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CSCOPE
    cs_చివర();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_IMAGE
    uninit_image_state();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (garbage_collect_at_exit)
	garbage_collect(FALSE);
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
    free_cmd_వాదనలు_W();
#అయితే_ముగింపు

    పదం_disable_dec();

    mch_exit(exitval);
}

/*
 * Get the name of the display, before gui_prepare() removes it from
 * argv[].  Used for the xterm-clipboard display.
 *
 * Also find the --server, --clientserver... arguments and --socketid and
 * --windowid
 */
    స్థిర శూన్య
early_arg_పరిశీలన(mparm_T *parmp వాడని)
{
#ఒకవేళ defined(FEAT_XCLIPBOARD) || defined(FEAT_CLIENTSERVER) \
	|| !defined(FEAT_NETBEANS_INTG)
    పూర్ణాంకం		argc = parmp->argc;
    అక్షరం	**argv = parmp->argv;
    పూర్ణాంకం		i;

    కొరకు (i = 1; i < argc; i++)
    {
	ఒకవేళ (STRCMP(argv[i], "--") == 0)
	    ఆపు;
# ifdef FEAT_XCLIPBOARD
	లేకపోతే ఒకవేళ (STRICMP(argv[i], "-display") == 0
#  ఒకవేళ defined(FEAT_GUI_GTK)
		|| STRICMP(argv[i], "--display") == 0
#  endif
		)
	{
	    ఒకవేళ (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    xterm_display = argv[++i];
	}
# endif
# ifdef FEAT_CLIENTSERVER
	లేకపోతే ఒకవేళ (STRICMP(argv[i], "--servername") == 0)
	{
	    ఒకవేళ (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    parmp->serverName_arg = (char_u *)argv[++i];
	}
	లేకపోతే ఒకవేళ (STRICMP(argv[i], "--serverlist") == 0)
	    parmp->serverArg = TRUE;
	లేకపోతే ఒకవేళ (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    parmp->serverArg = TRUE;
#  ifdef FEAT_GUI
	    ఒకవేళ (అక్షరశ్రేణి_శోధన(argv[i], "-wait") != 0)
		// don't fork() when starting the GUI to edit files ourself
		gui.dofork = false;
#  endif
	}
#  ifdef FEAT_CLIENTSERVER_BACKENDS
	లేకపోతే ఒకవేళ (STRICMP(argv[i], "--clientserver") == 0)
	{
	    char_u *arg;
	    ఒకవేళ (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    arg = (char_u *)argv[++i];

	    ఒకవేళ (STRICMP(arg, "socket") == 0)
		clientserver_పద్ధతి = CLIENTSERVER_పద్ధతి_SOCKET;
#   ifdef FEAT_X11
	    లేకపోతే ఒకవేళ (STRICMP(arg, "x11") == 0)
		clientserver_పద్ధతి = CLIENTSERVER_పద్ధతి_X11;
#   endif
#   ifdef MSWIN
	    లేకపోతే ఒకవేళ (STRICMP(arg, "mswin") == 0)
		clientserver_పద్ధతి = CLIENTSERVER_పద్ధతి_MSWIN;
#   endif
	    లేకపోతే
		mainerr(ME_UNKNOWN_OPTION, arg);
	}
#  endif
# endif

# ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
#  ifdef FEAT_GUI_MSWIN
	లేకపోతే ఒకవేళ (STRICMP(argv[i], "--windowid") == 0)
#  లేకపోతే
	లేకపోతే ఒకవేళ (STRICMP(argv[i], "--socketid") == 0)
#  endif
	{
	    long_u	id;
	    పూర్ణాంకం		లెక్క;

	    ఒకవేళ (i == argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    ఒకవేళ (STRNICMP(argv[i+1], "0x", 2) == 0)
		లెక్క = అక్షరశ్రేణి_చదువు(&(argv[i + 1][2]), SCANF_HEX_LONG_U, &id);
	    లేకపోతే
		లెక్క = అక్షరశ్రేణి_చదువు(argv[i + 1], SCANF_DECIMAL_LONG_U, &id);
	    ఒకవేళ (లెక్క != 1)
		mainerr(ME_INVALID_ARG, (char_u *)argv[i]);
	    లేకపోతే
#  ifdef FEAT_GUI_MSWIN
		win_socket_id = id;
#  elif !defined(USE_GTK4)
		gtk_socket_id = id;
#  endif
	    i++;
	}
# endif
# ఒకవేళ defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
	లేకపోతే ఒకవేళ (STRICMP(argv[i], "--echo-wid") == 0)
	    echo_wid_arg = TRUE;
# endif
# ifndef FEAT_NETBEANS_INTG
	లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(argv[i], "-nb", (size_t)3) == 0)
	{
	    mch_errmsg(_("'-nb' cannot be used: not enabled at compile time\n"));
	    mch_exit(2);
	}
# endif

    }
#అయితే_ముగింపు
}

#నిర్వచితం_కాకపోతే NO_VIM_MAIN
/*
 * Get a (optional) count for a Vim argument.
 */
    స్థిర పూర్ణాంకం
get_సంఖ్య_arg(
    char_u	*p,	    // pointer to argument
    పూర్ణాంకం		*idx,	    // index in argument, is incremented
    పూర్ణాంకం		def)	    // default value
{
    ఒకవేళ (vim_isdigit(p[*idx]))
    {
	def = atoi((అక్షరం *)&(p[*idx]));
	అయితే (vim_isdigit(p[*idx]))
	    *idx = *idx + 1;
    }
    తిరిగి def;
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
    స్థిర శూన్య
parse_command_name(mparm_T *parmp)
{
    char_u	*initstr;

    initstr = gettail((char_u *)parmp->argv[0]);

# ifdef FEAT_EVAL
    సమితి_vim_var_string(VV_PROGNAME, initstr, -1);
    సమితి_progpath((char_u *)parmp->argv[0]);
# endif

    ఒకవేళ (TOLOWER_ASC(initstr[0]) == 'r')
    {
	restricted = TRUE;
	++initstr;
    }

    // Use evim mode for "evim" and "egvim", not for "editor".
    ఒకవేళ (TOLOWER_ASC(initstr[0]) == 'e'
	    && (TOLOWER_ASC(initstr[1]) == 'v'
		|| TOLOWER_ASC(initstr[1]) == 'g'))
    {
# ifdef FEAT_GUI
	gui.starting = true;
# endif
	parmp->evim_బహుళకం = TRUE;
	++initstr;
    }

    // "gvim" starts the GUI.  Also accept "Gvim" for MS-Windows.
    ఒకవేళ (TOLOWER_ASC(initstr[0]) == 'g'
# ifdef VIMDLL
	    || mch_is_gui_executable()
# endif
       )
    {
	main_ప్రారంభం_gui();
# ifdef FEAT_GUI
	++initstr;
# endif
# ifdef GUI_MAY_SPAWN
	gui.dospawn = false;	// No need to spawn a new process.
# endif
    }
# ifdef GUI_MAY_SPAWN
    లేకపోతే
	gui.dospawn = true;	// Not "gvim". Need to spawn gvim.exe.
# endif


    ఒకవేళ (STRNICMP(initstr, "view", 4) == 0)
    {
	readonlymode = TRUE;
	curbuf->b_p_ro = TRUE;
	p_uc = 10000;			// don't update very often
	initstr += 4;
    }
    లేకపోతే ఒకవేళ (STRNICMP(initstr, "vim", 3) == 0)
	initstr += 3;

    // Catch "[r][g]vimdiff" and "[r][g]viewdiff".
    ఒకవేళ (STRICMP(initstr, "diff") == 0)
    {
# ifdef FEAT_DIFF
	parmp->diff_బహుళకం = TRUE;
# లేకపోతే
	mch_errmsg(_("This Vim was not compiled with the diff feature."));
	mch_errmsg("\n");
	mch_exit(2);
# endif
    }

    // Checking for "ex" here may catch some weird names, such as "vimex" or
    // "viewex", we assume the user knows that.
    ఒకవేళ (STRNICMP(initstr, "ex", 2) == 0)
    {
	ఒకవేళ (STRNICMP(initstr + 2, "im", 2) == 0)
	    exmode_చురుకైన = EXMODE_VIM;
	లేకపోతే
	    exmode_చురుకైన = EXMODE_సాధారణ;
	change_compatible(TRUE);	// set 'compatible'
    }
}

/*
 * Scan the command line arguments.
 */
    స్థిర శూన్య
command_పంక్తి_పరిశీలన(mparm_T *parmp)
{
    పూర్ణాంకం		argc = parmp->argc;
    అక్షరం	**argv = parmp->argv;
    పూర్ణాంకం		argv_idx;		// index in argv[n][]
    పూర్ణాంకం		had_minmin = FALSE;	// found "--" argument
    పూర్ణాంకం		want_వాదం;		// option argument with argument
    పూర్ణాంకం		c;
    char_u	*p = ఖాళీ;
    దీర్ఘ	n;

    --argc;
    ++argv;
    argv_idx = 1;	    // active option letter is argv[0][argv_idx]
    అయితే (argc > 0)
    {
	/*
	 * "+" or "+{number}" or "+/{pat}" or "+{command}" argument.
	 */
	ఒకవేళ (argv[0][0] == '+' && !had_minmin)
	{
	    ఒకవేళ (parmp->n_commands >= MAX_ARG_CMDS)
		mainerr(ME_EXTRA_CMD, ఖాళీ);
	    argv_idx = -1;	    // skip to next argument
	    ఒకవేళ (argv[0][1] == NUL)
		parmp->commands[parmp->n_commands++] = (char_u *)"$";
	    లేకపోతే
		parmp->commands[parmp->n_commands++] = (char_u *)&(argv[0][1]);
	}

	/*
	 * Optional argument.
	 */
	లేకపోతే ఒకవేళ (argv[0][0] == '-' && !had_minmin)
	{
	    want_వాదం = FALSE;
	    c = argv[0][argv_idx++];
# ఒకవేళ defined( VMS)
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
	    ఒకవేళ (c == '/')
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
	    ఎంపిక (c)
	    {
	    స్థితి NUL:		// "vim -"  read from stdin
				// "ex -" silent mode
		ఒకవేళ (exmode_చురుకైన)
		    silent_బహుళకం = TRUE;
		లేకపోతే
		{
		    ఒకవేళ (parmp->edit_type != EDIT_NONE)
			mainerr(ME_TOO_MANY_వాదనలు, (char_u *)argv[0]);
		    parmp->edit_type = EDIT_STDIN;
		    read_cmd_fd = 2;	// read from stderr instead of stdin
		}
		argv_idx = -1;		// skip to next argument
		ఆపు;

	    స్థితి '-':		// "--" don't take any more option arguments
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
		ఒకవేళ (STRICMP(argv[0] + argv_idx, "help") == 0)
		    usage();
		లేకపోతే ఒకవేళ (STRICMP(argv[0] + argv_idx, "version") == 0)
		{
		    cmdline_వెడల్పు = Columns = 80;   // need to init Columns
		    info_సందేశం = TRUE; // use mch_msg(), not mch_errmsg()
# ఒకవేళ defined(FEAT_GUI) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
		    gui.starting = false; // not starting GUI, will exit
# endif
		    list_సంస్కరణ();
		    సందేశం_putchar('\n');
		    సందేశం_didout = FALSE;
		    mch_exit(0);
		}
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "clean", 5) == 0)
		{
		    parmp->use_vimrc = (char_u *)"DEFAULTS";
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)"NONE";
# endif
		    parmp->clean = TRUE;
		    సమితి_option_విలువ_give_err((char_u *)"vif",
						      0L, (char_u *)"NONE", 0);
		}
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "literal", 7) == 0)
		{
# ifdef EXPAND_FILENAMES
		    parmp->literal = TRUE;
# endif
		}
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "nofork", 6) == 0)
		{
# ifdef FEAT_GUI
		    gui.dofork = false;	// don't fork() when starting GUI
# endif
		}
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "noplugin", 8) == 0)
		    p_lpl = FALSE;
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "not-a-term", 10) == 0)
		    parmp->not_a_పదం = TRUE;
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "gui-dialog-file", 15)
									 == 0)
		{
		    want_వాదం = TRUE;
		    argv_idx += 15;
		}
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "ttyfail", 7) == 0)
		    parmp->tty_fail = TRUE;
# ifdef FEAT_పని_CHANNEL
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "stdio-channel", 13) == 0)
		{
		    // Like "-es": no terminal, but the channel is served
		    // instead of reading Ex commands.
		    use_stdio_channel = TRUE;
		    exmode_చురుకైన = EXMODE_సాధారణ;
		    silent_బహుళకం = TRUE;
#  ఒకవేళ defined(FEAT_GUI) && !defined(VIMDLL)
		    gui.starting = false;	// don't start GUI
#  endif
		}
# endif
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "cmd", 3) == 0)
		{
		    want_వాదం = TRUE;
		    argv_idx += 3;
		}
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "startuptime", 11) == 0)
		{
		    want_వాదం = TRUE;
		    argv_idx += 11;
		}
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "log", 3) == 0)
		{
		    want_వాదం = TRUE;
		    argv_idx += 3;
		}
# ifdef FEAT_CLIENTSERVER
		లేకపోతే ఒకవేళ (STRICMP(argv[0] + argv_idx, "serverlist") == 0)
		    ; // already processed -- no arg
		లేకపోతే ఒకవేళ (STRICMP(argv[0] + argv_idx, "servername") == 0
		       || STRICMP(argv[0] + argv_idx, "serversend") == 0
		       // Don't put this under FEAT_CLIENTSERVER_BACKENDS, just
		       // let it be ignored. Makes tests less complicated
		       || STRICMP(argv[0] + argv_idx, "clientserver") == 0
		       )
		{
		    // already processed -- snatch the following arg
		    ఒకవేళ (argc > 1)
		    {
			--argc;
			++argv;
		    }
		}
# endif
# ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
#  ifdef FEAT_GUI_GTK
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "socketid", 8) == 0)
#  లేకపోతే
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "windowid", 8) == 0)
#  endif
		{
		    // already processed -- snatch the following arg
		    ఒకవేళ (argc > 1)
		    {
			--argc;
			++argv;
		    }
		}
# endif
# ifdef FEAT_GUI_GTK
		లేకపోతే ఒకవేళ (STRNICMP(argv[0] + argv_idx, "echo-wid", 8) == 0)
		{
		    // already processed, skip
		}
# endif
		లేకపోతే
		{
		    ఒకవేళ (argv[0][argv_idx])
			mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
		    had_minmin = TRUE;
		}
		ఒకవేళ (!want_వాదం)
		    argv_idx = -1;	// skip to next argument
		ఆపు;

	    స్థితి 'A':		// "-A" start in Arabic mode
# ifdef FEAT_ARABIC
		సమితి_option_విలువ_give_err((char_u *)"arabic", 1L, ఖాళీ, 0);
# లేకపోతే
		mch_errmsg(_(e_arabic_cannot_be_వాడిన_not_enabled_at_compile_సమయం));
		mch_exit(2);
# endif
		ఆపు;

	    స్థితి 'b':		// "-b" binary mode
		// Needs to be effective before expanding file names, because
		// for Win32 this makes us edit a shortcut file itself,
		// instead of the file it links to.
		సమితి_options_bin(curbuf->b_p_bin, 1, 0);
		curbuf->b_p_bin = 1;	    // binary file I/O
		ఆపు;

	    స్థితి 'C':		// "-C"  Compatible
		change_compatible(TRUE);
		has_dash_c_arg = TRUE;
		ఆపు;

	    స్థితి 'e':		// "-e" Ex mode
		exmode_చురుకైన = EXMODE_సాధారణ;
		ఆపు;

	    స్థితి 'E':		// "-E" Improved Ex mode
		exmode_చురుకైన = EXMODE_VIM;
		ఆపు;

	    స్థితి 'f':		// "-f"  GUI: run in foreground.  Amiga: open
				// window directly, not with newcli
# ifdef FEAT_GUI
		gui.dofork = false;	// don't fork() when starting GUI
# endif
		ఆపు;

	    స్థితి 'g':		// "-g" start GUI
		main_ప్రారంభం_gui();
		ఆపు;

	    స్థితి 'F':		// "-F" was for Farsi mode
		mch_errmsg(_(e_farsi_support_has_been_removed));
		mch_exit(2);
		ఆపు;

	    స్థితి '?':		// "-?" give help message (for MS-Windows)
	    స్థితి 'h':		// "-h" give help message
# ifdef FEAT_GUI_GNOME
		// Tell usage() to exit for "gvim".
		gui.starting = false;
# endif
		usage();
		ఆపు;

	    స్థితి 'H':		// "-H" start in Hebrew mode: rl + hkmap set
# ifdef FEAT_RIGHTLEFT
		p_hkmap = TRUE;
		సమితి_option_విలువ_give_err((char_u *)"rl", 1L, ఖాళీ, 0);
# లేకపోతే
		mch_errmsg(_(e_hebrew_cannot_be_వాడిన_not_enabled_at_compile_సమయం));
		mch_exit(2);
# endif
		ఆపు;

	    స్థితి 'l':		// "-l" lisp mode, 'lisp' and 'showmatch' on
		సమితి_option_విలువ_give_err((char_u *)"lisp", 1L, ఖాళీ, 0);
		p_sm = TRUE;
		ఆపు;

	    స్థితి 'M':		// "-M"  no changes or writing of files
		reset_modifiable();
		// FALLTHROUGH

	    స్థితి 'm':		// "-m"  no writing of files
		p_write = FALSE;
		ఆపు;

	    స్థితి 'y':		// "-y"  easy mode
# ifdef FEAT_GUI
		gui.starting = true;	// start GUI a bit later
# endif
		parmp->evim_బహుళకం = TRUE;
		ఆపు;

	    స్థితి 'N':		// "-N"  Nocompatible
		change_compatible(FALSE);
		ఆపు;

	    స్థితి 'n':		// "-n" no swap file
# ifdef FEAT_NETBEANS_INTG
		// checking for "-nb", netbeans parameters
		ఒకవేళ (argv[0][argv_idx] == 'b')
		{
		    netbeansArg = argv[0];
		    argv_idx = -1;	    // skip to next argument
		}
		లేకపోతే
# endif
		parmp->no_మార్పిడి_file = TRUE;
		ఆపు;

	    స్థితి 'p':		// "-p[N]" open N tab pages
# ifdef లక్ష్యం_API_MAC_OSX
		// For some reason on MacOS X, an argument like:
		// -psn_0_10223617 is passed in when invoke from Finder
		// or with the 'open' command
		ఒకవేళ (argv[0][argv_idx] == 's')
		{
		    argv_idx = -1; // bypass full -psn
		    main_ప్రారంభం_gui();
		    ఆపు;
		}
# endif
		// default is 0: open window for each file
		parmp->కిటికీ_లెక్క = get_సంఖ్య_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->కిటికీ_layout = WIN_TABS;
		ఆపు;

	    స్థితి 'o':		// "-o[N]" open N horizontal split windows
		// default is 0: open window for each file
		parmp->కిటికీ_లెక్క = get_సంఖ్య_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->కిటికీ_layout = WIN_HOR;
		ఆపు;

		స్థితి 'O':	// "-O[N]" open N vertical split windows
		// default is 0: open window for each file
		parmp->కిటికీ_లెక్క = get_సంఖ్య_arg((char_u *)argv[0],
								&argv_idx, 0);
		parmp->కిటికీ_layout = WIN_VER;
		ఆపు;

# ifdef FEAT_QUICKFIX
	    స్థితి 'q':		// "-q" QuickFix mode
		ఒకవేళ (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_వాదనలు, (char_u *)argv[0]);
		parmp->edit_type = EDIT_QF;
		ఒకవేళ (argv[0][argv_idx])		// "-q{errorfile}"
		{
		    parmp->use_ef = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		లేకపోతే ఒకవేళ (argc > 1)		// "-q {errorfile}"
		    want_వాదం = TRUE;
		ఆపు;
# endif

	    స్థితి 'R':		// "-R" readonly mode
		readonlymode = TRUE;
		curbuf->b_p_ro = TRUE;
		p_uc = 10000;			// don't update very often
		ఆపు;

	    స్థితి 'r':		// "-r" recovery mode
	    స్థితి 'L':		// "-L" recovery mode
		recoverymode = 1;
		ఆపు;

	    స్థితి 's':
		ఒకవేళ (exmode_చురుకైన)	// "-s" silent (batch) mode
		    silent_బహుళకం = TRUE;
		లేకపోతే		// "-s {scriptin}" read from script file
		    want_వాదం = TRUE;
		ఆపు;

	    స్థితి 't':		// "-t {tag}" or "-t{tag}" jump to tag
		ఒకవేళ (parmp->edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_వాదనలు, (char_u *)argv[0]);
		parmp->edit_type = EDIT_ట్యాగ్;
		ఒకవేళ (argv[0][argv_idx])		// "-t{tag}"
		{
		    parmp->tagname = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		లేకపోతే				// "-t {tag}"
		    want_వాదం = TRUE;
		ఆపు;

# ifdef FEAT_EVAL
	    స్థితి 'D':		// "-D"		Debugging
		parmp->use_debug_break_level = 9999;
		ఆపు;
# endif
# ifdef FEAT_DIFF
	    స్థితి 'd':		// "-d"		'diff'
#  ifdef AMIGA
		// check for "-dev {device}"
		ఒకవేళ (argv[0][argv_idx] == 'e' && argv[0][argv_idx + 1] == 'v')
		    want_వాదం = TRUE;
		లేకపోతే
#  endif
		    parmp->diff_బహుళకం = TRUE;
		ఆపు;
# endif
	    స్థితి 'V':		// "-V{N}"	Verbose level
		// default is 10: a little bit verbose
		p_verbose = get_సంఖ్య_arg((char_u *)argv[0], &argv_idx, 10);
		ఒకవేళ (argv[0][argv_idx] != NUL)
		{
		    సమితి_option_విలువ_give_err((char_u *)"verbosefile",
					  0L, (char_u *)argv[0] + argv_idx, 0);
		    argv_idx = (పూర్ణాంకం)STRLEN(argv[0]);
		}
		ఆపు;

	    స్థితి 'v':		// "-v"  Vi-mode (as if called "vi")
		exmode_చురుకైన = 0;
# ఒకవేళ defined(FEAT_GUI) && !defined(VIMDLL)
		gui.starting = false;	// don't start GUI
# endif
		ఆపు;

	    స్థితి 'w':		// "-w{number}"	set window height
				// "-w {scriptout}"	write to script
		ఒకవేళ (vim_isdigit(((char_u *)argv[0])[argv_idx]))
		{
		    n = get_సంఖ్య_arg((char_u *)argv[0], &argv_idx, 10);
		    సమితి_option_విలువ_give_err((char_u *)"window", n, ఖాళీ, 0);
		    ఆపు;
		}
		want_వాదం = TRUE;
		ఆపు;

# ifdef FEAT_CRYPT
	    స్థితి 'x':		// "-x"  encrypted reading/writing of files
		parmp->ask_for_కీలకం = TRUE;
		ఆపు;
# endif

	    స్థితి 'X':		// "-X"  don't connect to X server
# ఒకవేళ (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
		x_no_connect = TRUE;
# endif
		ఆపు;
	    స్థితి 'Y':		// "-Y" don't connect to Wayland compositor
# ఒకవేళ defined(FEAT_WAYLAND)
		wayland_no_connect = TRUE;
# endif
		ఆపు;

	    స్థితి 'Z':		// "-Z"  restricted mode
		restricted = TRUE;
		ఆపు;

	    స్థితి 'c':		// "-c{command}" or "-c {command}" execute
				// command
		ఒకవేళ (argv[0][argv_idx] != NUL)
		{
		    ఒకవేళ (parmp->n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, ఖాళీ);
		    parmp->commands[parmp->n_commands++] = (char_u *)argv[0]
								   + argv_idx;
		    argv_idx = -1;
		    ఆపు;
		}
		// FALLTHROUGH
	    స్థితి 'S':		// "-S {file}" execute Vim script
	    స్థితి 'i':		// "-i {viminfo}" use for viminfo
# ifndef FEAT_DIFF
	    స్థితి 'd':		// "-d {device}" device (for Amiga)
# endif
	    స్థితి 'T':		// "-T {terminal}" terminal name
	    స్థితి 'u':		// "-u {vimrc}" vim inits file
	    స్థితి 'U':		// "-U {gvimrc}" gvim inits file
	    స్థితి 'W':		// "-W {scriptout}" overwrite
# ifdef FEAT_GUI_MSWIN
	    స్థితి 'P':		// "-P {parent title}" MDI parent
# endif
		want_వాదం = TRUE;
		ఆపు;

	    అప్రమేయ:
		mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
	    }

	    /*
	     * Handle option arguments with argument.
	     */
	    ఒకవేళ (want_వాదం)
	    {
		/*
		 * Check for garbage immediately after the option letter.
		 */
		ఒకవేళ (argv[0][argv_idx] != NUL)
		    mainerr(ME_GARBAGE, (char_u *)argv[0]);

		--argc;
		ఒకవేళ (argc < 1 && c != 'S')  // -S has an optional argument
		    mainerr_arg_missing((char_u *)argv[0]);
		++argv;
		argv_idx = -1;

		ఎంపిక (c)
		{
		స్థితి 'c':	// "-c {command}" execute command
		స్థితి 'S':	// "-S {file}" execute Vim script
		    ఒకవేళ (parmp->n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, ఖాళీ);
		    ఒకవేళ (c == 'S')
		    {
			అక్షరం	*a;

			ఒకవేళ (argc < 1)
			    // "-S" without argument: use default session file
			    // name.
			    a = SESSION_FILE;
			లేకపోతే ఒకవేళ (argv[0][0] == '-')
			{
			    // "-S" followed by another option: use default
			    // session file name.
			    a = SESSION_FILE;
			    ++argc;
			    --argv;
			}
			లేకపోతే
			    a = argv[0];
			p = alloc(STRLEN(a) + 4);
			ఒకవేళ (p == ఖాళీ)
			    mch_exit(2);
			sprintf((అక్షరం *)p, "so %s", a);
			parmp->cmds_tofree[parmp->n_commands] = TRUE;
			parmp->commands[parmp->n_commands++] = p;
		    }
		    లేకపోతే
			parmp->commands[parmp->n_commands++] =
							    (char_u *)argv[0];
		    ఆపు;

		స్థితి '-':
		    ఒకవేళ (argv[-1][2] == 'c')
		    {
			// "--cmd {command}" execute command
			ఒకవేళ (parmp->n_pre_commands >= MAX_ARG_CMDS)
			    mainerr(ME_EXTRA_CMD, ఖాళీ);
			parmp->pre_commands[parmp->n_pre_commands++] =
							    (char_u *)argv[0];
		    }
		    // --gui-dialog-file fname
		    ఒకవేళ (argv[-1][2] == 'g')
		    {
			// without GUI ignore the argument
# ifdef FEAT_GUI
			parmp->gui_dialog_file = (char_u *)argv[0];
# endif
		    }

		    // "--startuptime <file>" already handled
		    // "--log <file>" already handled
		    ఆపు;

	    //	case 'd':   -d {device} is handled in mch_check_win() for the
	    //		    Amiga

# ifdef FEAT_QUICKFIX
		స్థితి 'q':	// "-q {errorfile}" QuickFix mode
		    parmp->use_ef = (char_u *)argv[0];
		    ఆపు;
# endif

		స్థితి 'i':	// "-i {viminfo}" use for viminfo
		    సమితి_option_విలువ_give_err((char_u *)"vif",
						     0L, (char_u *)argv[0], 0);
		    ఆపు;

		స్థితి 's':	// "-s {scriptin}" read from script file
		    ఒకవేళ (scriptin[0] != ఖాళీ)
		    {
scripterror:
			mch_errmsg(_("Attempt to open script file again: \""));
			mch_errmsg(argv[-1]);
			mch_errmsg(" ");
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    ఒకవేళ ((scriptin[0] = mch_fopen(argv[0], READBIN)) == ఖాళీ)
		    {
			mch_errmsg(_("Cannot open for reading: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    ఒకవేళ (save_typebuf() == FAIL)
			mch_exit(2);	// out of memory
		    ఆపు;

		స్థితి 't':	// "-t {tag}"
		    parmp->tagname = (char_u *)argv[0];
		    ఆపు;

		స్థితి 'T':	// "-T {terminal}" terminal name
		    /*
		     * The -T term argument is always available and when
		     * HAVE_TERMLIB is supported it overrides the environment
		     * variable TERM.
		     */
# ifdef FEAT_GUI
		    ఒకవేళ (పదం_is_gui((char_u *)argv[0]))
			gui.starting = true;	// start GUI a bit later
		    లేకపోతే
# endif
			parmp->పదం = (char_u *)argv[0];
		    ఆపు;

		స్థితి 'u':	// "-u {vimrc}" vim inits file
		    parmp->use_vimrc = (char_u *)argv[0];
		    ఆపు;

		స్థితి 'U':	// "-U {gvimrc}" gvim inits file
# ifdef FEAT_GUI
		    use_gvimrc = (char_u *)argv[0];
# endif
		    ఆపు;

		స్థితి 'w':	// "-w {nr}" 'window' value
				// "-w {scriptout}" append to script file
		    ఒకవేళ (vim_isdigit(*((char_u *)argv[0])))
		    {
			argv_idx = 0;
			n = get_సంఖ్య_arg((char_u *)argv[0], &argv_idx, 10);
			సమితి_option_విలువ_give_err((char_u *)"window",
								   n, ఖాళీ, 0);
			argv_idx = -1;
			ఆపు;
		    }
		    // FALLTHROUGH
		స్థితి 'W':	// "-W {scriptout}" overwrite script file
		    ఒకవేళ (scriptout != ఖాళీ)
			వెళ్ళు scripterror;
		    ఒకవేళ ((scriptout = mch_fopen(argv[0],
				    c == 'w' ? APPENDBIN : WRITEBIN)) == ఖాళీ)
		    {
			mch_errmsg(_("Cannot open for script output: \""));
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_exit(2);
		    }
		    ఆపు;

# ifdef FEAT_GUI_MSWIN
		స్థితి 'P':		// "-P {parent title}" MDI parent
		    gui_mch_సమితి_మాతృ(argv[0]);
		    ఆపు;
# endif
		}
	    }
	}

	/*
	 * File name argument.
	 */
	లేకపోతే
	{
	    argv_idx = -1;	    // skip to next argument

	    // Check for only one type of editing.
	    ఒకవేళ (parmp->edit_type != EDIT_NONE && parmp->edit_type != EDIT_FILE)
		mainerr(ME_TOO_MANY_వాదనలు, (char_u *)argv[0]);
	    parmp->edit_type = EDIT_FILE;

# ifdef MSWIN
	    // Remember if the argument was a full path before changing
	    // slashes to backslashes.
	    ఒకవేళ (argv[0][0] != NUL && argv[0][1] == ':' && argv[0][2] == '\\')
		parmp->నిండిన_మార్గం = TRUE;
# endif

	    // Add the file to the global argument list.
	    ఒకవేళ (ga_grow(&global_alist.al_ga, 1) == FAIL
		    || (p = vim_strsave((char_u *)argv[0])) == ఖాళీ)
		mch_exit(2);
# ifdef FEAT_DIFF
	    ఒకవేళ (parmp->diff_బహుళకం && mch_isdir(p) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *తోక;
		string_T    ret;

		తోక = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(p, STRLEN(p), తోక, STRLEN(తోక), TRUE, &ret);
		ఒకవేళ (ret.string != ఖాళీ)
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
	    ఒకవేళ (vim_strpbrk(p, "\\:") != ఖాళీ && !మార్గం_with_url(p))
	    {
		అక్షరం posix_మార్గం[MAXPATHL];

#  ఒకవేళ CYGWIN_సంస్కరణ_DLL_MAJOR >= 1007
		cygwin_conv_మార్గం(CCP_WIN_A_గమ్యం_POSIX, p, posix_మార్గం, MAXPATHL);
#  లేకపోతే
		cygwin_conv_గమ్యం_posix_మార్గం(p, posix_మార్గం);
#  endif
		vim_free(p);
		p = vim_strsave((char_u *)posix_మార్గం);
		ఒకవేళ (p == ఖాళీ)
		    mch_exit(2);
	    }
# endif

# ifdef USE_FNAME_CASE
	    // Make the case of the file name match the actual file.
	    fname_case(p, 0);
# endif

	    alist_కూడు(&global_alist, p,
# ifdef EXPAND_FILENAMES
		    parmp->literal ? 2 : 0	// add buffer nr after exp.
# లేకపోతే
		    2		// add buffer number now and use curbuf
# endif
		    );

# ifdef MSWIN
	    {
		// Remember this argument has been added to the argument list.
		// Needed when 'encoding' is changed.
		వాడిన_file_arg(argv[0], parmp->literal, parmp->నిండిన_మార్గం,
#  ifdef FEAT_DIFF
							    parmp->diff_బహుళకం
#  లేకపోతే
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
	ఒకవేళ (argv_idx <= 0 || argv[0][argv_idx] == NUL)
	{
	    --argc;
	    ++argv;
	    argv_idx = 1;
	}
    }

# ifdef FEAT_EVAL
    // If there is a "+123" or "-c" command, set v:swapcommand to the first
    // one.
    ఒకవేళ (parmp->n_commands > 0)
    {
	size_t  plen;

	plen = STRLEN(parmp->commands[0]) + 2;
	p = alloc(plen + 1);
	ఒకవేళ (p != ఖాళీ)
	{
	    sprintf((అక్షరం *)p, ":%s\r", parmp->commands[0]);
	    సమితి_vim_var_string(VV_SWAPCOMMAND, p, (పూర్ణాంకం)plen);
	    vim_free(p);
	}
    }
# endif
}

/*
 * Print a warning if stdout is not a terminal.
 * When starting in Ex mode and commands come from a file, set silent_mode.
 */
    స్థిర శూన్య
check_tty(mparm_T *parmp)
{
    పూర్ణాంకం		ఆదానం_isatty;		// is active input a terminal?

    ఆదానం_isatty = mch_ఆదానం_isatty();
    ఒకవేళ (exmode_చురుకైన)
    {
	ఒకవేళ (!ఆదానం_isatty)
	    silent_బహుళకం = TRUE;
    }
    లేకపోతే ఒకవేళ (parmp->want_నిండిన_screen && (!stdout_isatty || !ఆదానం_isatty)
# ifdef FEAT_GUI
	    // don't want the delay when started from the desktop
	    && !gui.starting
# endif
	    && !parmp->not_a_పదం)
    {
# ifdef NBDEBUG
	/*
	 * This shouldn't be necessary. But if I run netbeans with the log
	 * output coming to the console and XOpenDisplay fails, I get vim
	 * trying to start with input/output to my console tty.  This fills my
	 * input buffer so fast I can't even kill the process in under 2
	 * minutes (and it beeps continuously the whole time :-)
	 */
	ఒకవేళ (netbeans_చురుకైన() && (!stdout_isatty || !ఆదానం_isatty))
	{
	    mch_errmsg(_("Vim: Error: Failure to start gvim from NetBeans\n"));
	    నిష్క్రమించు(1);
	}
# endif
# ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	ఒకవేళ (
#  ifdef VIMDLL
	    !gui.starting &&
#  endif
	    is_cygpty_వాడిన())
	{
#  ఒకవేళ defined(HAVE_BIND_TEXTDOMAIN_CODESET) \
	&& defined(FEAT_GETTEXT)
	    అక్షరం    *s, *tofree = ఖాళీ;

	    // Set the encoding of the error message based on $LC_ALL or
	    // other environment variables instead of 'encoding'.
	    // Note that the message is shown on a Cygwin terminal (e.g.
	    // mintty) which encoding is based on $LC_ALL or etc., not the
	    // current codepage used by normal Win32 console programs.
	    tofree = s = (అక్షరం *)enc_locale_env(ఖాళీ);
	    ఒకవేళ (s == ఖాళీ)
		s = "utf-8";	// Use "utf-8" by default.
	    (శూన్య)bind_textdomain_codeset(VIMPACKAGE, s);
	    vim_free(tofree);
#  endif
	    mch_errmsg(_("Vim: Error: This version of Vim does not run in a Cygwin terminal\n"));
	    నిష్క్రమించు(1);
	}
# endif
	ఒకవేళ (!stdout_isatty)
	    mch_errmsg(_("Vim: Warning: Output is not to a terminal\n"));
	ఒకవేళ (!ఆదానం_isatty)
	    mch_errmsg(_("Vim: Warning: Input is not from a terminal\n"));
	బహిర్గతం_flush();
	ఒకవేళ (parmp->tty_fail && (!stdout_isatty || !ఆదానం_isatty))
	    నిష్క్రమించు(1);
	ఒకవేళ (scriptin[0] == ఖాళీ)
	    ui_delay(2005L, TRUE);
	సమయం_సందేశం("Warning delay");
    }
}

/*
 * Read text from stdin.
 */
    స్థిర శూన్య
read_stdin(శూన్య)
{
    పూర్ణాంకం	    i;

    // When getting the ATTENTION prompt here, use a dialog
    మార్పిడి_exists_action = SEA_DIALOG;

    no_wait_return = TRUE;
    i = సందేశం_didany;
    సమితి_buflisted(TRUE);

    // Create memfile and read from stdin.
    (శూన్య)open_బఫరు(TRUE, ఖాళీ, 0);

    no_wait_return = FALSE;
    సందేశం_didany = i;
    సమయం_సందేశం("reading stdin");

    check_మార్పిడి_exists_action();

# ఒకవేళ !(defined(AMIGA) || defined(MACOS_X))
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
    స్థిర శూన్య
create_windows(mparm_T *parmp వాడని)
{
    పూర్ణాంకం		dorewind;
    పూర్ణాంకం		పూర్తి = 0;

    /*
     * Create the number of windows that was requested.
     */
    ఒకవేళ (parmp->కిటికీ_లెక్క == -1)	// was not set
	parmp->కిటికీ_లెక్క = 1;
    ఒకవేళ (parmp->కిటికీ_లెక్క == 0)
	parmp->కిటికీ_లెక్క = GARGCOUNT;
    ఒకవేళ (parmp->కిటికీ_లెక్క > 1)
    {
	// Don't change the windows if there was a command in .vimrc that
	// already split some windows
	ఒకవేళ (parmp->కిటికీ_layout == 0)
	    parmp->కిటికీ_layout = WIN_HOR;
	ఒకవేళ (parmp->కిటికీ_layout == WIN_TABS)
	{
	    parmp->కిటికీ_లెక్క = make_tabpages(parmp->కిటికీ_లెక్క);
	    సమయం_సందేశం("making tab pages");
	}
	లేకపోతే ఒకవేళ (firstwin->w_తదుపరి == ఖాళీ)
	{
	    parmp->కిటికీ_లెక్క = make_windows(parmp->కిటికీ_లెక్క,
					     parmp->కిటికీ_layout == WIN_VER);
	    సమయం_సందేశం("making windows");
	}
	లేకపోతే
	    parmp->కిటికీ_లెక్క = win_లెక్క();
    }
    లేకపోతే
	parmp->కిటికీ_లెక్క = 1;

    ఒకవేళ (recoverymode)			// do recover
    {
	సందేశం_scroll = TRUE;		// scroll message up
	ml_recover(TRUE);
	ఒకవేళ (curbuf->b_ml.ml_mfp == ఖాళీ) // failed
	    getout(1);
	do_modelines(0);		// do modelines
    }
    లేకపోతే
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
	అయితే (పూర్తి++ < 1000)
	{
	    ఒకవేళ (dorewind)
	    {
		ఒకవేళ (parmp->కిటికీ_layout == WIN_TABS)
		    goto_tabpage(1);
		లేకపోతే
		    curwin = firstwin;
	    }
	    లేకపోతే ఒకవేళ (parmp->కిటికీ_layout == WIN_TABS)
	    {
		ఒకవేళ (curtab->tp_తదుపరి == ఖాళీ)
		    ఆపు;
		goto_tabpage(0);
	    }
	    లేకపోతే
	    {
		ఒకవేళ (curwin->w_తదుపరి == ఖాళీ)
		    ఆపు;
		curwin = curwin->w_తదుపరి;
	    }
	    dorewind = FALSE;
	    curbuf = curwin->w_బఫరు;
	    ఒకవేళ (curbuf->b_ml.ml_mfp == ఖాళీ)
	    {
# ifdef FEAT_FOLDING
		// Set 'foldlevel' to 'foldlevelstart' if it's not negative.
		ఒకవేళ (p_fdls >= 0)
		    curwin->w_p_fdl = p_fdls;
# endif
		// When getting the ATTENTION prompt here, use a dialog
		మార్పిడి_exists_action = SEA_DIALOG;

		సమితి_buflisted(TRUE);

		// create memfile, read file
		(శూన్య)open_బఫరు(FALSE, ఖాళీ, 0);

		ఒకవేళ (మార్పిడి_exists_action == SEA_QUIT)
		{
		    ఒకవేళ (got_int || only_one_కిటికీ())
		    {
			// abort selected or quit and only one window
			did_emsg = FALSE;   // avoid hit-enter prompt
			getout(1);
		    }
		    // We can't close the window, it would disturb what
		    // happens next.  Clear the file name and set the arg
		    // index to -1 to delete it later.
		    setfname(curbuf, ఖాళీ, ఖాళీ, FALSE);
		    curwin->w_arg_idx = -1;
		    మార్పిడి_exists_action = SEA_NONE;
		}
		లేకపోతే
		    handle_మార్పిడి_exists(ఖాళీ);
		dorewind = TRUE;		// start again
	    }
	    ui_breakcheck();
	    ఒకవేళ (got_int)
	    {
		(శూన్య)vgetc();	// only break the file loading, not the rest
		ఆపు;
	    }
	}
	ఒకవేళ (parmp->కిటికీ_layout == WIN_TABS)
	    goto_tabpage(1);
	లేకపోతే
	    curwin = firstwin;
	curbuf = curwin->w_బఫరు;
	--autocmd_no_enter;
	--autocmd_no_leave;
    }
}

/*
 * If opened more than one window, start editing files in the other
 * windows.  make_windows() has already opened the windows.
 */
    స్థిర శూన్య
edit_buffers(
    mparm_T	*parmp,
    char_u	*cwd)			// current working dir
{
    పూర్ణాంకం		arg_idx;		// index in argument list
    పూర్ణాంకం		i;
    పూర్ణాంకం		advance = TRUE;
    win_T	*win;
    char_u	*p_shm_save = ఖాళీ;

    /*
     * Don't execute Win/Buf Enter/Leave autocommands here
     */
    ++autocmd_no_enter;
    ++autocmd_no_leave;

    // When w_arg_idx is -1 remove the window (see create_windows()).
    ఒకవేళ (curwin->w_arg_idx == -1)
    {
	win_close(curwin, TRUE);
	advance = FALSE;
    }

    arg_idx = 1;
    కొరకు (i = 1; i < parmp->కిటికీ_లెక్క; ++i)
    {
	ఒకవేళ (cwd != ఖాళీ)
	    mch_chdir((అక్షరం *)cwd);
	// When w_arg_idx is -1 remove the window (see create_windows()).
	ఒకవేళ (curwin->w_arg_idx == -1)
	{
	    ++arg_idx;
	    win_close(curwin, TRUE);
	    advance = FALSE;
	    కొనసాగు;
	}

	ఒకవేళ (advance)
	{
	    ఒకవేళ (parmp->కిటికీ_layout == WIN_TABS)
	    {
		ఒకవేళ (curtab->tp_తదుపరి == ఖాళీ)	// just checking
		    ఆపు;
		goto_tabpage(0);
		// Temporarily reset 'shm' option to not print fileinfo when
		// loading the other buffers. This would overwrite the already
		// existing fileinfo for the first tab.
		ఒకవేళ (i == 1)
		{
		    అక్షరం buf[100];

		    p_shm_save = vim_strsave(p_shm);
		    vim_snprintf(buf, 100, "F%s", p_shm);
		    సమితి_option_విలువ_give_err((char_u *)"shm",
							 0L, (char_u *)buf, 0);
		}
	    }
	    లేకపోతే
	    {
		ఒకవేళ (curwin->w_తదుపరి == ఖాళీ)	// just checking
		    ఆపు;
		win_enter(curwin->w_తదుపరి, FALSE);
	    }
	}
	advance = TRUE;

	// Only open the file if there is no file in this window yet (that can
	// happen when .vimrc contains ":sall").
	ఒకవేళ (curbuf == firstwin->w_బఫరు || curbuf->b_ffname == ఖాళీ)
	{
	    curwin->w_arg_idx = arg_idx;
	    // Edit file from arg list, if there is one.  When "Quit" selected
	    // at the ATTENTION prompt close the window.
	    మార్పిడి_exists_did_quit = FALSE;
	    (శూన్య)do_ecmd(0, arg_idx < GARGCOUNT
			  ? alist_name(&GARGLIST[arg_idx]) : ఖాళీ,
			  ఖాళీ, ఖాళీ, ECMD_LASTL, ECMD_HIDE, curwin);
	    ఒకవేళ (మార్పిడి_exists_did_quit)
	    {
		// abort or quit selected
		ఒకవేళ (got_int || only_one_కిటికీ())
		{
		    // abort selected and only one window
		    did_emsg = FALSE;   // avoid hit-enter prompt
		    getout(1);
		}
		win_close(curwin, TRUE);
		advance = FALSE;
	    }
	    ఒకవేళ (arg_idx == GARGCOUNT - 1)
		arg_had_చివరి = TRUE;
	    ++arg_idx;
	}
	ui_breakcheck();
	ఒకవేళ (got_int)
	{
	    (శూన్య)vgetc();	// only break the file loading, not the rest
	    ఆపు;
	}
    }

    ఒకవేళ (p_shm_save != ఖాళీ)
    {
	సమితి_option_విలువ_give_err((char_u *)"shm", 0L, p_shm_save, 0);
	vim_free(p_shm_save);
    }

    ఒకవేళ (parmp->కిటికీ_layout == WIN_TABS)
	goto_tabpage(1);
    --autocmd_no_enter;

    // make the first window the current window
    win = firstwin;
# ఒకవేళ defined(FEAT_QUICKFIX)
    // Avoid making a preview window the current window.
    అయితే (win->w_p_pvw)
    {
	win = win->w_తదుపరి;
	ఒకవేళ (win == ఖాళీ)
	{
	    win = firstwin;
	    ఆపు;
	}
    }
# endif
    win_enter(win, FALSE);

    --autocmd_no_leave;
    సమయం_సందేశం("editing files in windows");
    ఒకవేళ (parmp->కిటికీ_లెక్క > 1 && parmp->కిటికీ_layout != WIN_TABS)
	win_equal(curwin, FALSE, 'b');	// adjust heights
}

/*
 * Execute the commands from --cmd arguments "cmds[cnt]".
 */
    స్థిర శూన్య
exe_pre_commands(mparm_T *parmp)
{
    char_u	**cmds = parmp->pre_commands;
    పూర్ణాంకం		cnt = parmp->n_pre_commands;
    పూర్ణాంకం		i;
    ESTACK_CHECK_DECLARATION;

    ఒకవేళ (cnt <= 0)
	తిరిగి;

    curwin->w_cursor.lnum = 0; // just in case..
    estack_నెట్టు(ETYPE_వాదనలు, (char_u *)_("pre-vimrc command line"), 0);
    ESTACK_CHECK_అమరిక;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CMDARG;
# endif
    కొరకు (i = 0; i < cnt; ++i)
	do_cmdline_cmd(cmds[i]);
    ESTACK_CHECK_NOW;
    estack_తీయి();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    సమయం_సందేశం("--cmd commands");
}

/*
 * Execute "+", "-c" and "-S" arguments.
 */
    స్థిర శూన్య
exe_commands(mparm_T *parmp)
{
    పూర్ణాంకం		i;
    ESTACK_CHECK_DECLARATION;

    /*
     * We start commands on line 0, make "vim +/pat file" match a
     * pattern on line 1.  But don't move the cursor when an autocommand
     * with g`" was used.
     */
    సందేశం_scroll = TRUE;
    ఒకవేళ (parmp->tagname == ఖాళీ && curwin->w_cursor.lnum <= 1)
	curwin->w_cursor.lnum = 0;
    estack_నెట్టు(ETYPE_వాదనలు, (char_u *)"command line", 0);
    ESTACK_CHECK_అమరిక;
# ifdef FEAT_EVAL
    current_sctx.sc_sid = SID_CARG;
    current_sctx.sc_seq = 0;
# endif
    కొరకు (i = 0; i < parmp->n_commands; ++i)
    {
	do_cmdline_cmd(parmp->commands[i]);
	ఒకవేళ (parmp->cmds_tofree[i])
	    vim_free(parmp->commands[i]);
    }
    ESTACK_CHECK_NOW;
    estack_తీయి();
# ifdef FEAT_EVAL
    current_sctx.sc_sid = 0;
# endif
    ఒకవేళ (curwin->w_cursor.lnum == 0)
	curwin->w_cursor.lnum = 1;

    ఒకవేళ (!exmode_చురుకైన)
	సందేశం_scroll = FALSE;

# ifdef FEAT_QUICKFIX
    // When started with "-q errorfile" jump to first error again.
    ఒకవేళ (parmp->edit_type == EDIT_QF)
	qf_jump(ఖాళీ, 0, 0, FALSE);
# endif
    సమయం_సందేశం("executing command arguments");
}

/*
 * Source startup scripts.
 */
    స్థిర శూన్య
ఆధారం_startup_scripts(mparm_T *parmp)
{
    పూర్ణాంకం		i;

    /*
     * For "evim" source evim.vim first of all, so that the user can overrule
     * any things he doesn't like.
     */
    ఒకవేళ (parmp->evim_బహుళకం)
    {
	(శూన్య)do_ఆధారం((char_u *)EVIM_FILE, FALSE, DOSO_NONE, ఖాళీ);
	సమయం_సందేశం("source evim file");
    }

    /*
     * If -u argument given, use only the initializations from that file and
     * nothing else.
     */
    ఒకవేళ (parmp->use_vimrc != ఖాళీ)
    {
	ఒకవేళ (STRCMP(parmp->use_vimrc, "DEFAULTS") == 0)
	{
	    ఒకవేళ (do_ఆధారం((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE, ఖాళీ)
									 != OK)
		emsg(_(e_failed_గమ్యం_ఆధారం_defaults));
	}
	లేకపోతే ఒకవేళ (STRCMP(parmp->use_vimrc, "NONE") == 0
				     || STRCMP(parmp->use_vimrc, "NORC") == 0)
	{
# ifdef FEAT_GUI
	    ఒకవేళ (use_gvimrc == ఖాళీ)	    // don't load gvimrc either
		use_gvimrc = parmp->use_vimrc;
# endif
	}
	లేకపోతే
	{
	    ఒకవేళ (do_ఆధారం(parmp->use_vimrc, FALSE, DOSO_NONE, ఖాళీ) != OK)
		semsg(_(e_cannot_read_from_str_2), parmp->use_vimrc);
	}
    }
    లేకపోతే ఒకవేళ (!silent_బహుళకం)
    {
# ifdef AMIGA
	నిర్మాణం Process	*proc = (నిర్మాణం Process *)కనుగొను_Task(0L);
	APTR		save_winptr = proc->pr_కిటికీ_Ptr;

	// Avoid a requester here for a volume that doesn't exist.
	proc->pr_కిటికీ_Ptr = (APTR)-1L;
# endif

	/*
	 * Get system wide defaults, if the file name is defined.
	 */
# ifdef SYS_VIMRC_FILE
	(శూన్య)do_ఆధారం((char_u *)SYS_VIMRC_FILE, FALSE, DOSO_NONE, ఖాళీ);
# endif
# ifdef MACOS_X
	(శూన్య)do_ఆధారం((char_u *)"$VIMRUNTIME/macmap.vim", FALSE,
							      DOSO_NONE, ఖాళీ);
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
	ఒకవేళ (process_env((char_u *)"VIMINIT", TRUE) != OK)
	{
	    ఒకవేళ (do_ఆధారం((char_u *)USR_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, ఖాళీ) == FAIL
# ifdef USR_VIMRC_FILE2
		&& do_ఆధారం((char_u *)USR_VIMRC_FILE2, TRUE,
						      DOSO_VIMRC, ఖాళీ) == FAIL
# endif
# ifdef XDG_VIMRC_FILE
		&& do_ఆధారం((char_u *)XDG_VIMRC_FILE, TRUE,
						      DOSO_VIMRC, ఖాళీ) == FAIL
# endif
# ifdef USR_VIMRC_FILE3
		&& do_ఆధారం((char_u *)USR_VIMRC_FILE3, TRUE,
						      DOSO_VIMRC, ఖాళీ) == FAIL
# endif
# ifdef USR_VIMRC_FILE4
		&& do_ఆధారం((char_u *)USR_VIMRC_FILE4, TRUE,
						      DOSO_VIMRC, ఖాళీ) == FAIL
# endif
		&& process_env((char_u *)"EXINIT", FALSE) == FAIL
		&& do_ఆధారం((char_u *)USR_EXRC_FILE, FALSE,
						       DOSO_NONE, ఖాళీ) == FAIL
# ifdef USR_EXRC_FILE2
		&& do_ఆధారం((char_u *)USR_EXRC_FILE2, FALSE,
						       DOSO_NONE, ఖాళీ) == FAIL
# endif
		&& !has_dash_c_arg)
	    {
		// When no .vimrc file was found: source defaults.vim.
		ఒకవేళ (do_ఆధారం((char_u *)VIM_DEFAULTS_FILE, FALSE, DOSO_NONE,
								 ఖాళీ) == FAIL)
		    emsg(_(e_failed_గమ్యం_ఆధారం_defaults));
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
	ఒకవేళ (p_exrc)
	{
# ఒకవేళ defined(UNIX) || defined(VMS)
	    // If ".vimrc" file is not owned by user, set 'secure' mode.
	    ఒకవేళ (!file_owned(VIMRC_FILE))
# endif
		secure = p_secure;

	    i = FAIL;
	    ఒకవేళ (fullpathcmp((char_u *)USR_VIMRC_FILE,
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
		i = do_ఆధారం((char_u *)VIMRC_FILE, TRUE, DOSO_VIMRC, ఖాళీ);

	    ఒకవేళ (i == FAIL)
	    {
# ఒకవేళ defined(UNIX) || defined(VMS)
		// if ".exrc" is not owned by user set 'secure' mode
		ఒకవేళ (!file_owned(EXRC_FILE))
		    secure = p_secure;
		లేకపోతే
		    secure = 0;
# endif
		ఒకవేళ (	   fullpathcmp((char_u *)USR_EXRC_FILE,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# ifdef USR_EXRC_FILE2
			&& fullpathcmp((char_u *)USR_EXRC_FILE2,
				(char_u *)EXRC_FILE, FALSE, TRUE) != FPC_SAME
# endif
				)
		    (శూన్య)do_ఆధారం((char_u *)EXRC_FILE, FALSE,
							      DOSO_NONE, ఖాళీ);
	    }
	}
	ఒకవేళ (secure == 2)
	    need_wait_return = TRUE;
	secure = 0;
# ifdef AMIGA
	proc->pr_కిటికీ_Ptr = save_winptr;
# endif
    }
    సమయం_సందేశం("sourcing vimrc file(s)");
}

/*
 * Setup to start using the GUI.  Exit with an error when not available.
 */
    స్థిర శూన్య
main_ప్రారంభం_gui(శూన్య)
{
# ifdef FEAT_GUI
    gui.starting = true;	// start GUI a bit later
# లేకపోతే
    mch_errmsg(_(e_gui_cannot_be_వాడిన_not_enabled_at_compile_సమయం));
    mch_errmsg("\n");
    mch_exit(2);
# endif
}

#అయితే_ముగింపు  // NO_VIM_MAIN

/*
 * Get an environment variable and execute it as Ex commands.
 * Returns FAIL if the environment variable was not executed, OK otherwise.
 */
    పూర్ణాంకం
process_env(
    char_u	*env,
    పూర్ణాంకం		is_viminit) // when TRUE, called for VIMINIT
{
    char_u	*initstr;
    sctx_T	save_current_sctx;
    ESTACK_CHECK_DECLARATION;

    ఒకవేళ ((initstr = mch_getenv(env)) == ఖాళీ || *initstr == NUL)
	తిరిగి FAIL;

    ఒకవేళ (is_viminit)
	vimrc_దొరికింది(ఖాళీ, ఖాళీ);
    estack_నెట్టు(ETYPE_ENV, env, 0);
    ESTACK_CHECK_అమరిక;
    save_current_sctx = current_sctx;
    current_sctx.sc_సంస్కరణ = 1;
#నిర్వచితమైతే FEAT_EVAL
    current_sctx.sc_sid = SID_ENV;
    current_sctx.sc_seq = 0;
    current_sctx.sc_lnum = 0;
#అయితే_ముగింపు

    do_cmdline_cmd(initstr);

    ESTACK_CHECK_NOW;
    estack_తీయి();
    current_sctx = save_current_sctx;
    తిరిగి OK;
}

#ఒకవేళ (defined(UNIX) || defined(VMS)) && !defined(NO_VIM_MAIN)
/*
 * Return TRUE if we are certain the user owns the file "fname".
 * Used for ".vimrc" and ".exrc".
 * Use both stat() and lstat() for extra security.
 */
    స్థిర పూర్ణాంకం
file_owned(అక్షరం *fname)
{
    stat_T	s;
# ifdef UNIX
    uid_t	uid = getuid();
# లేకపోతే	 // VMS
    uid_t	uid = ((getgid() << 16) | getuid());
# endif

    తిరిగి !(mch_stat(fname, &s) != 0 || s.st_uid != uid
# ifdef HAVE_LSTAT
	    || mch_lstat(fname, &s) != 0 || s.st_uid != uid
# endif
	    );
}
#అయితే_ముగింపు

/*
 * Give an error message main_errors["n"] and exit.
 */
    స్థిర శూన్య
mainerr(
    పూర్ణాంకం		n,	// one of the ME_ defines
    char_u	*str)	// extra argument or NULL
{
#ఒకవేళ defined(UNIX) || defined(VMS)
    reset_సంకేతాలు();		// kill us with CTRL-C here, if you like
#అయితే_ముగింపు

    // If this is a Windows GUI executable, show an error dialog box.
#నిర్వచితమైతే VIMDLL
    gui.లోపల_use = mch_is_gui_executable();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_MSWIN
    gui.starting = false;   // Needed to show as error.
#అయితే_ముగింపు

    init_long_సంస్కరణ();
    mch_errmsg(long_సంస్కరణ);
    mch_errmsg("\n");
    mch_errmsg(_(main_errors[n]));
    ఒకవేళ (str != ఖాళీ)
    {
	mch_errmsg(": \"");
	mch_errmsg((అక్షరం *)str);
	mch_errmsg("\"");
    }
    mch_errmsg(_("\nMore info with: \"vim -h\"\n"));

    mch_exit(1);
}

    శూన్య
mainerr_arg_missing(char_u *str)
{
    mainerr(ME_ARG_MISSING, str);
}

#నిర్వచితం_కాకపోతే NO_VIM_MAIN
/*
 * print a message with three spaces prepended and '\n' appended.
 */
    స్థిర శూన్య
main_సందేశం(అక్షరం *s)
{
    mch_సందేశం("   ");
    mch_సందేశం(s);
    mch_సందేశం("\n");
}

/*
 * Print messages for "vim -h" or "vim --help" and exit.
 */
    స్థిర శూన్య
usage(శూన్య)
{
    పూర్ణాంకం		i;
    స్థిర అక్షరం	*(use[]) =
    {
	N_("[file ..]       edit specified file(s)"),
	N_("-               read text from stdin"),
	N_("-t tag          edit file where tag is defined"),
# ifdef FEAT_QUICKFIX
	N_("-q [errorfile]  edit file with first error")
# endif
    };

# ఒకవేళ defined(UNIX) || defined(VMS)
    reset_సంకేతాలు();		// kill us with CTRL-C here, if you like
# endif

    init_long_సంస్కరణ();
    mch_సందేశం(long_సంస్కరణ);
    mch_సందేశం(_("\n\nUsage:"));
    కొరకు (i = 0; ; ++i)
    {
	mch_సందేశం(_(" vim [arguments] "));
	mch_సందేశం(_(use[i]));
	ఒకవేళ (i == ARRAY_నిడివి(use) - 1)
	    ఆపు;
	mch_సందేశం(_("\n   or:"));
    }
# ఒకవేళ defined( VMS)
    mch_సందేశం(_("\nWhere command is down-cased, prepend / (like: -/R) to treat flag as upper-case."));
    mch_సందేశం(_("\nOr, where supported, SET PROC/PARSE=EXT, or else quote upper-case material."));
# endif /* defined( VMS) */

    mch_సందేశం(_("\n\nArguments:\n"));
    main_సందేశం(_("--\t\t\tOnly file names after this"));
# ifdef EXPAND_FILENAMES
    main_సందేశం(_("--literal\t\tDon't expand wildcards"));
# endif
# ifdef FEAT_OLE
    main_సందేశం(_("-register\t\tRegister this gvim for OLE"));
    main_సందేశం(_("-unregister\t\tUnregister gvim for OLE"));
# endif
# ifdef FEAT_GUI
    main_సందేశం(_("-g\t\t\tRun using GUI (like \"gvim\")"));
    main_సందేశం(_("-f  or  --nofork\tForeground: Don't fork when starting GUI"));
# endif
    main_సందేశం(_("-v\t\t\tVi mode (like \"vi\")"));
    main_సందేశం(_("-e\t\t\tEx mode (like \"ex\")"));
    main_సందేశం(_("-E\t\t\tImproved Ex mode"));
    main_సందేశం(_("-s\t\t\tSilent (batch) mode (only for \"ex\")"));
# ifdef FEAT_DIFF
    main_సందేశం(_("-d\t\t\tDiff mode (like \"vimdiff\")"));
# endif
    main_సందేశం(_("-y\t\t\tEasy mode (like \"evim\", modeless)"));
    main_సందేశం(_("-R\t\t\tReadonly mode (like \"view\")"));
    main_సందేశం(_("-Z\t\t\tRestricted mode (like \"rvim\")"));
    main_సందేశం(_("-m\t\t\tModifications (writing files) not allowed"));
    main_సందేశం(_("-M\t\t\tModifications in text not allowed"));
    main_సందేశం(_("-b\t\t\tBinary mode"));
    main_సందేశం(_("-l\t\t\tLisp mode"));
    main_సందేశం(_("-C\t\t\tCompatible with Vi: 'compatible'"));
    main_సందేశం(_("-N\t\t\tNot fully Vi compatible: 'nocompatible'"));
    main_సందేశం(_("-V[N][fname]\t\tBe verbose [level N] [log messages to fname]"));
# ifdef FEAT_EVAL
    main_సందేశం(_("-D\t\t\tDebugging mode"));
# endif
    main_సందేశం(_("-n\t\t\tNo swap file, use memory only"));
    main_సందేశం(_("-r\t\t\tList swap files and exit"));
    main_సందేశం(_("-r (with file name)\tRecover crashed session"));
    main_సందేశం(_("-L\t\t\tSame as -r"));
# ifdef AMIGA
    main_సందేశం(_("-f\t\t\tDon't use newcli to open window"));
    main_సందేశం(_("-dev <device>\t\tUse <device> for I/O"));
# endif
# ifdef FEAT_ARABIC
    main_సందేశం(_("-A\t\t\tStart in Arabic mode"));
# endif
# ifdef FEAT_RIGHTLEFT
    main_సందేశం(_("-H\t\t\tStart in Hebrew mode"));
# endif
    main_సందేశం(_("-T <terminal>\tSet terminal type to <terminal>"));
    main_సందేశం(_("--not-a-term\t\tSkip warning for input/output not being a terminal"));
# ifdef FEAT_GUI
    main_సందేశం(_("--gui-dialog-file {fname}  For testing: write dialog text"));
# endif
    main_సందేశం(_("--ttyfail\t\tExit if input or output is not a terminal"));
# ifdef FEAT_పని_CHANNEL
    main_సందేశం(_("--stdio-channel\tUse stdin and stdout as a channel"));
# endif
    main_సందేశం(_("-u <vimrc>\t\tUse <vimrc> instead of any .vimrc"));
# ifdef FEAT_GUI
    main_సందేశం(_("-U <gvimrc>\t\tUse <gvimrc> instead of any .gvimrc"));
# endif
    main_సందేశం(_("--noplugin\t\tDon't load plugin scripts"));
    main_సందేశం(_("-p[N]\t\tOpen N tab pages (default: one for each file)"));
    main_సందేశం(_("-o[N]\t\tOpen N windows (default: one for each file)"));
    main_సందేశం(_("-O[N]\t\tLike -o but split vertically"));
    main_సందేశం(_("+\t\t\tStart at end of file"));
    main_సందేశం(_("+<lnum>\t\tStart at line <lnum>"));
    main_సందేశం(_("--cmd <command>\tExecute <command> before loading any vimrc file"));
    main_సందేశం(_("-c <command>\t\tExecute <command> after loading the first file"));
    main_సందేశం(_("-S <session>\t\tSource file <session> after loading the first file"));
    main_సందేశం(_("-s <scriptin>\tRead Normal mode commands from file <scriptin>"));
    main_సందేశం(_("-w <scriptout>\tAppend all typed commands to file <scriptout>"));
    main_సందేశం(_("-W <scriptout>\tWrite all typed commands to file <scriptout>"));
# ifdef FEAT_CRYPT
    main_సందేశం(_("-x\t\t\tEdit encrypted files"));
# endif
# ఒకవేళ (defined(UNIX) || defined(VMS)) && defined(FEAT_X11)
#  ఒకవేళ defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
    main_సందేశం(_("-display <display>\tConnect Vim to this particular X-server"));
#  endif
    main_సందేశం(_("-X\t\t\tDo not connect to X server"));
# endif
# ఒకవేళ defined(FEAT_WAYLAND)
    main_సందేశం(_("-Y\t\t\tDo not connect to Wayland compositor"));
# endif
# ifdef FEAT_CLIENTSERVER
#  ifdef FEAT_CLIENTSERVER_BACKENDS
    main_సందేశం(_("--clientserver <socket|x11|mswin> Backend for clientserver communication"));
#  endif
    main_సందేశం(_("--remote <files>\tEdit <files> in a Vim server if possible"));
    main_సందేశం(_("--remote-silent <files>  Same, don't complain if there is no server"));
    main_సందేశం(_("--remote-wait <files>  As --remote but wait for files to have been edited"));
    main_సందేశం(_("--remote-wait-silent <files>  Same, don't complain if there is no server"));
    main_సందేశం(_("--remote-tab[-wait][-silent] <files>  As --remote but use tab page per file"));
    main_సందేశం(_("--remote-send <keys>\tSend <keys> to a Vim server and exit"));
    main_సందేశం(_("--remote-expr <expr>\tEvaluate <expr> in a Vim server and print result"));
    main_సందేశం(_("--serverlist\t\tList available Vim server names and exit"));
    main_సందేశం(_("--servername <name>\tSend to/become the Vim server <name>"));
# endif
# ifdef STARTUPTIME
    main_సందేశం(_("--startuptime <file>\tWrite startup timing messages to <file>"));
# endif
# ifdef FEAT_పని_CHANNEL
    main_సందేశం(_("--log <file>\t\tStart logging to <file> early"));
# endif
# ifdef FEAT_VIMINFO
    main_సందేశం(_("-i <viminfo>\t\tUse <viminfo> instead of .viminfo"));
# endif
    main_సందేశం(_("--clean\t\t'nocompatible', Vim defaults, no plugins, no viminfo"));
    main_సందేశం(_("-h  or  --help\tPrint Help (this message) and exit"));
    main_సందేశం(_("--version\t\tPrint version information and exit"));

# ifdef FEAT_GUI_X11
#  ifdef FEAT_GUI_MOTIF
    mch_సందేశం(_("\nArguments recognised by gvim (Motif version):\n"));
#  endif
    main_సందేశం(_("-display <display>\tRun Vim on <display>"));
    main_సందేశం(_("-iconic\t\tStart Vim iconified"));
    main_సందేశం(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_సందేశం(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_సందేశం(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_సందేశం(_("-boldfont <font>\tUse <font> for bold text"));
    main_సందేశం(_("-italicfont <font>\tUse <font> for italic text"));
    main_సందేశం(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
    main_సందేశం(_("-borderwidth <width>\tUse a border width of <width> (also: -bw)"));
    main_సందేశం(_("-scrollbarwidth <width>  Use a scrollbar width of <width> (also: -sw)"));
    main_సందేశం(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_సందేశం(_("+reverse\t\tDon't use reverse video (also: +rv)"));
    main_సందేశం(_("-xrm <resource>\tSet the specified resource"));
# endif // FEAT_GUI_X11
# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
    mch_సందేశం(_("\nArguments recognised by gvim (GTK4 version):\n"));
#  లేకపోతే
    mch_సందేశం(_("\nArguments recognised by gvim (GTK+ version):\n"));
#  endif
    main_సందేశం(_("-background <color>\tUse <color> for the background (also: -bg)"));
    main_సందేశం(_("-foreground <color>\tUse <color> for normal text (also: -fg)"));
    main_సందేశం(_("-font <font>\t\tUse <font> for normal text (also: -fn)"));
    main_సందేశం(_("-geometry <geom>\tUse <geom> for initial geometry (also: -geom)"));
#  ifdef USE_GTK4
    main_సందేశం("--prg-name <name>\tSet GTK program name");
#  లేకపోతే
    main_సందేశం(_("-iconic\t\tStart Vim iconified"));
    main_సందేశం(_("-reverse\t\tUse reverse video (also: -rv)"));
    main_సందేశం(_("-display <display>\tRun Vim on <display> (also: --display)"));
    main_సందేశం(_("--role <role>\tSet a unique role to identify the main window"));
    main_సందేశం(_("--socketid <xid>\tOpen Vim inside another GTK widget"));
    main_సందేశం(_("--echo-wid\t\tMake gvim echo the Window ID on stdout"));
#  endif
# endif
# ifdef FEAT_GUI_MSWIN
#  ifdef VIMDLL
    ఒకవేళ (gui.starting)
#  endif
    {
	main_సందేశం(_("-P <parent title>\tOpen Vim inside parent application"));
	main_సందేశం(_("--windowid <HWND>\tOpen Vim inside another win32 widget"));
    }
# endif

# ifdef FEAT_GUI_GNOME
    // Gnome gives extra messages for --help if we continue, but not for -h.
    ఒకవేళ (gui.starting)
    {
	mch_సందేశం("\n");
	gui.dofork = false;
    }
    లేకపోతే
# endif
	mch_exit(0);
}

/*
 * Check the result of the ATTENTION dialog:
 * When "Quit" selected, exit Vim.
 * When "Recover" selected, recover the file.
 */
    స్థిర శూన్య
check_మార్పిడి_exists_action(శూన్య)
{
    ఒకవేళ (మార్పిడి_exists_action == SEA_QUIT)
	getout(1);
    handle_మార్పిడి_exists(ఖాళీ);
}

#అయితే_ముగింపు // NO_VIM_MAIN

#ఒకవేళ !defined(NO_VIM_MAIN) && defined(FEAT_EVAL)
    స్థిర శూన్య
సమితి_progpath(char_u *argv0)
{
    char_u *val = argv0;

# ifdef MSWIN
    // A relative path containing a "/" will become invalid when using ":cd",
    // turn it into a full path.
    // On MS-Windows "vim" should be expanded to "vim.exe", thus always do
    // this.
    char_u *మార్గం = ఖాళీ;

    ఒకవేళ (mch_can_exe(argv0, &మార్గం, FALSE) && మార్గం != ఖాళీ)
	val = మార్గం;
# లేకపోతే
    char_u	buf[MAXPATHL + 1];
#  ifdef PROC_EXE_LINK
    అక్షరం	linkbuf[MAXPATHL + 1];
    ssize_t	len;

    len = readlink(PROC_EXE_LINK, linkbuf, MAXPATHL);
    ఒకవేళ (len > 0)
    {
	linkbuf[len] = NUL;
	val = (char_u *)linkbuf;
    }
#  endif

    ఒకవేళ (!mch_is_నిండిన_Name(val))
    {
	ఒకవేళ (gettail(val) != val
			   && vim_నిండిన_Name(val, buf, MAXPATHL, TRUE) != FAIL)
	    val = buf;
    }
# endif

    సమితి_vim_var_string(VV_PROGPATH, val, -1);

# ifdef MSWIN
    vim_free(మార్గం);
# endif
}

#అయితే_ముగింపు // NO_VIM_MAIN
