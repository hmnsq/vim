/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * clientserver.c: functions for Client Server functionality
 */

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_CLIENTSERVER)

স্থির শূন্য cmdsrv_main(পূর্ণসংখ্যা *argc, অক্ষর **argv, char_u *serverName_arg, char_u **serverStr);
স্থির char_u *serverMakeName(char_u *arg, অক্ষর *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    শূন্য
server_to_প্রবেশ_buf(char_u *str)
{
    char_u      *ptr = ফাঁকা;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, ফাঁকা);
    p_cpo = cpo_save;

    যদি (ptr != ফাঁকা && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	যদি (typebuf.tb_maplen < typebuf.tb_len)
	    del_typebuf(typebuf.tb_len - typebuf.tb_maplen, typebuf.tb_maplen);
	(শূন্য)ins_typebuf(str, REMAP_NONE, typebuf.tb_len, TRUE, FALSE);

	// Let input_available() know we inserted text in the typeahead
	// buffer.
	typebuf_was_filled = TRUE;
    }
    vim_free(ptr);
}

/*
 * Evaluate an expression that the client sent to a string.
 */
    char_u *
eval_client_রাশিমালা_to_string(char_u *রাশিমালা)
{
    char_u	*res;
    পূর্ণসংখ্যা		save_dbl = debug_break_level;
    পূর্ণসংখ্যা		save_ro = redir_off;
    funccal_entry_T funccal_entry;
    পূর্ণসংখ্যা		did_save_funccal = FALSE;

# যদি defined(FEAT_EVAL)
    ch_log(ফাঁকা, "eval_client_expr_to_string(\"%s\")", রাশিমালা);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    যদি (!debug_mode)
    {
	save_funccal(&funccal_entry);
	did_save_funccal = TRUE;
    }

     // Disable debugging, otherwise Vim hangs, waiting for "cont" to be
     // typed.
    debug_break_level = -1;
    redir_off = 0;
    // Do not display error message, otherwise Vim hangs, waiting for "cont"
    // to be typed.  Do generate errors so that try/catch works.
    ++emsg_silent;

    res = eval_to_string(রাশিমালা, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    যদি (emsg_silent < 0)
	emsg_silent = 0;
    যদি (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    out_flush_cursor(FALSE, FALSE);

    ফেরত res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    পূর্ণসংখ্যা
sendToLocalVim(char_u *cmd, পূর্ণসংখ্যা as_রাশিমালা, char_u **ফলাফল)
{
    যদি (as_রাশিমালা)
    {
	char_u *ret;

	ret = eval_client_রাশিমালা_to_string(cmd);
	যদি (ফলাফল != ফাঁকা)
	{
	    যদি (ret == ফাঁকা)
	    {
		অক্ষর	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*বার্তা;

		বার্তা = alloc(len);
		যদি (বার্তা != ফাঁকা)
		    vim_snprintf((অক্ষর *)বার্তা, len, "%s: \"%s\"", err, cmd);
		*ফলাফল = বার্তা;
	    }
	    অন্যথা
		*ফলাফল = ret;
	}
	অন্যথা
	    vim_free(ret);
	ফেরত ret == ফাঁকা ? -1 : 0;
    }
    server_to_প্রবেশ_buf(cmd);
    ফেরত 0;
}

/*
 * If conversion is needed, convert "data" from "client_enc" to 'encoding' and
 * return an allocated string.  Otherwise return "data".
 * "*tofree" is set to the result when it needs to be freed later.
 */
    char_u *
serverConvert(
    char_u *client_enc UNUSED,
    char_u *তথ্য,
    char_u **tofree)
{
    char_u	*res = তথ্য;

    *tofree = ফাঁকা;
    যদি (client_enc == ফাঁকা || p_enc == ফাঁকা)
	ফেরত res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    যদি (convert_setup(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, তথ্য, ফাঁকা);
	যদি (res == ফাঁকা)
	    res = তথ্য;
	অন্যথা
	    *tofree = res;
    }
    convert_setup(&vimconv, ফাঁকা, ফাঁকা);
    ফেরত res;
}
#endif

#যদি defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

স্থির char_u *build_drop_cmd(পূর্ণসংখ্যা filec, অক্ষর **filev, পূর্ণসংখ্যা tabs, পূর্ণসংখ্যা sendReply);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    শূন্য
exec_on_server(mparm_T *parmp)
{
    পূর্ণসংখ্যা made_name = FALSE;

    যদি (parmp->serverName_arg != ফাঁকা && *parmp->serverName_arg == NUL)
	ফেরত;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    যদি (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    যদি (!made_name && parmp->servername == ফাঁকা)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    যদি (parmp->servername != ফাঁকা && clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	serverSetName(parmp->servername);
#  ifndef FEAT_SOCKETSERVER
	vim_free(parmp->servername);
#  endif
    }
# endif
}
/*
 * Prepare for running as a Vim server.
 */
    শূন্য
prepare_server(mparm_T *parmp)
{
# যদি defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
    /*
     * Register for remote command execution with :serversend and --remote
     * unless there was a -X or a --servername '' on the command line.
     * Only register nongui-vim's with an explicit --servername argument,
     * or when compiling with autoservername.
     * When running as root --servername is also required.
     *
     * If we are Windows and socketserver is being used, then don't use
     * autoservername, since traditional naming isn't supported.
     */

    যদি (
#  যদি defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != ফাঁকা
#   endif
#   যদি defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   যদি defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_method == CLIENTSERVER_METHOD_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != ফাঁকা && (
#  যদি defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   যদি defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_method != CLIENTSERVER_METHOD_SOCKET &&
#   endif
#   যদি defined(FEAT_AUTOSERVERNAME)
		    1
#   অন্যথা
		    gui.in_use
#   endif
#   ifdef UNIX
		 && getuid() != মূল_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != ফাঁকা))
    {

#  ifdef FEAT_SOCKETSERVER
	যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	    socketserver_start(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	যদি (clientserver_method == CLIENTSERVER_METHOD_X11)
	{
	    (শূন্য)serverRegisterName(X_DISPLAY, parmp->servername);
	    TIME_বার্তা("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    অন্যথা
    {
#  ifdef FEAT_X11
	যদি (clientserver_method == CLIENTSERVER_METHOD_X11)
	    serverDelayedStartName = parmp->servername;
	অন্যথা
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    যদি (parmp->serverStr != ফাঁকা)
    {
	char_u *p;

	server_to_প্রবেশ_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    স্থির শূন্য
cmdsrv_main(
    পূর্ণসংখ্যা		*argc,
    অক্ষর	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    পূর্ণসংখ্যা		i;
    char_u	*sname;
    পূর্ণসংখ্যা		ret = -1;
    পূর্ণসংখ্যা		didone = FALSE;
    পূর্ণসংখ্যা		exiterr = 0;
    অক্ষর	**newArgV = argv + 1;
    পূর্ণসংখ্যা		newArgC = 1,
		Argc = *argc;
    পূর্ণসংখ্যা		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    পূর্ণসংখ্যা		silent = FALSE;
    পূর্ণসংখ্যা		tabs = FALSE;
# ifdef MSWIN
    HWND	srv;
# elif defined(FEAT_X11)
    Window	srv;

    setup_term_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = ফাঁকা;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    যদি (sname == ফাঁকা)
	ফেরত;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    জন্য (i = 1; i < Argc; i++)
    {
	res = ফাঁকা;
	যদি (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    জন্য (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    বিরতি;
	}

	যদি (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	অন্যথা যদি (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    অক্ষর	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    যখন (*p != NUL)
	    {
		যদি (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		অন্যথা যদি (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		অন্যথা যদি (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		অন্যথা
		{
		    argtype = ARGTYPE_OTHER;
		    বিরতি;
		}
	    }
	}
	অন্যথা
	    argtype = ARGTYPE_OTHER;

	যদি (argtype != ARGTYPE_OTHER)
	{
	    যদি (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    যদি (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    অন্যথা
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		যদি (*serverStr == ফাঁকা)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    বিরতি;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		ret = socketserver_send(sname, *serverStr, ফাঁকা, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    যদি (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		যদি (xterm_dpy == ফাঁকা)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		অন্যথা
		    ret = serverSendToVim(xterm_dpy, sname, *serverStr,
			    ফাঁকা, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    যদি (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		ret = serverSendToVim(sname, *serverStr, ফাঁকা, &srv, 0, 0,
			silent);
# endif
	    যদি (ret < 0)
	    {
		যদি (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		অন্যথা যদি (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		বিরতি;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    যদি (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		SetForegroundWindow(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    যদি (argtype == ARGTYPE_EDIT_WAIT)
	    {
		পূর্ণসংখ্যা	অঙ্ক_Files = *argc - i - 1;
		char_u  *done = alloc(অঙ্ক_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		পূর্ণসংখ্যা	গুনতি = 0;
		বাহ্যিক HWND message_window;
# endif

		যদি (অঙ্ক_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --অঙ্ক_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cb_আকার = আকার(ni);
		ni.hWnd = message_window;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), গুনতি, অঙ্ক_Files);
		Shell_NotifyIcon(NIM_ADD, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(done, 0, অঙ্ক_Files);
		যখন (memchr(done, 0, অঙ্ক_Files) != ফাঁকা)
		{
		    char_u  *p = ফাঁকা;
		    পূর্ণসংখ্যা	    j;
# ifdef MSWIN
		    যদি (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
			p = serverGetReply(srv, ফাঁকা, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET
			    && (ch == ফাঁকা
				|| socketserver_read_reply(sname, &p, -1, true)
				== FAIL))
			    বিরতি;
# endif
# ifdef FEAT_X11
		    যদি (clientserver_method == CLIENTSERVER_METHOD_X11
			    && serverReadReply(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    বিরতি;
# endif
		    যদি (p == ফাঁকা)
			বিরতি;
		    j = atoi((অক্ষর *)p);
		    vim_free(p);
		    যদি (j >= 0 && j < অঙ্ক_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++গুনতি;
			sprintf(ni.szTip, _("%d of %d edited"),
							     গুনতি, অঙ্ক_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			done[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		যদি (ch != ফাঁকা)
		{
		    channel_close(ch, false);
		    channel_clear(ch);
		}
# endif
# ifdef FEAT_GUI_MSWIN
		Shell_NotifyIcon(NIM_DELETE, &ni);
# endif
		vim_free(done);
	    }
	}
	অন্যথা যদি (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    পূর্ণসংখ্যা status = OK;

	    যদি (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    যদি (clientserver_method == CLIENTSERVER_METHOD_MSWIN
		    && serverSendToVim(sname, (char_u *)argv[i + 1],
						  &res, ফাঁকা, 1, 0, FALSE) < 0)
		status = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, ফাঁকা) < 0)
		status = FAIL;
# endif
# ifdef FEAT_X11
	    যদি (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		যদি (xterm_dpy == ফাঁকা)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		অন্যথা যদি (serverSendToVim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    ফাঁকা, 1, 0, 1, FALSE) < 0)
		    status = FAIL;
	    }
# endif
	    যদি (status == FAIL)
	    {
		যদি (res != ফাঁকা && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((অক্ষর *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	অন্যথা যদি (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = ফাঁকা;
	    garray_T	ga;

# ifdef MSWIN
	    যদি (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		list = serverGetVimNames();
# endif
# ifdef FEAT_SOCKETSERVER
	    যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    যদি (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		    xterm_dpy != ফাঁকা)
		list = serverGetVimNames(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    যদি (list != ফাঁকা)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_তথ্য;

	    যদি (did_emsg)
		mch_errmsg("\n");
	}
	অন্যথা যদি (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    চালু;
	}
	অন্যথা
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    চালু;
	}
	didone = TRUE;
	যদি (res != ফাঁকা && *res != NUL)
	{
	    mch_বার্তা((অক্ষর *)res);
	    যদি (res[STRLEN(res) - 1] != '\n')
		mch_বার্তা("\n");
	}
	vim_free(res);
    }

    যদি (didone)
    {
	display_errors();	// display any collected messages
	exit(exiterr);	// Mission accomplished - get out
    }

    // Return back into main()
    *argc = newArgC;
    vim_free(sname);
}

/*
 * Build a ":drop" command to send to a Vim server.
 */
    স্থির char_u *
build_drop_cmd(
    পূর্ণসংখ্যা		filec,
    অক্ষর	**filev,
    পূর্ণসংখ্যা		tabs,		// Use ":tab drop" instead of ":drop".
    পূর্ণসংখ্যা		sendReply)
{
    garray_T	ga;
    পূর্ণসংখ্যা		i;
    char_u	*inicmd = ফাঁকা;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    ধ্রুবক string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    যদি (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    যদি (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    যদি (cwd == ফাঁকা)
	ফেরত ফাঁকা;
    যদি (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	ফেরত ফাঁকা;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_IN_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# অন্যথা
	    PATH_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    যদি (cdp.string == ফাঁকা)
	ফেরত ফাঁকা;
    cdp.দৈর্ঘ্য = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.দৈর্ঘ্য);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].দৈর্ঘ্য);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    যদি (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    জন্য (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		PATH_ESC_CHARS
# অন্যথা
		(char_u *)" \t%#"
# endif
		);
	যদি (p == ফাঁকা)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_তথ্য);
	    ফেরত ফাঁকা;
	}
	GA_CONCAT_LITERAL(&ga, " ");
	ga_concat(&ga, p);
	vim_free(p);
    }
    GA_CONCAT_LITERAL(&ga, "|if exists('*inputrestore')|call inputrestore()|endif<CR>");

    // The :drop commands goes to Insert mode when 'insertmode' is set, use
    // CTRL-\ CTRL-N again.
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>");

    // Switch back to the correct current directory (prior to temporary path
    // switch) unless 'autochdir' is set, in which case it will already be
    // correct after the :drop command. With line breaks and spaces:
    //  if !exists('+acd') || !&acd
    //    if haslocaldir()
    //	    cd -
    //      lcd -
    //    elseif getcwd() ==# 'current path'
    //      cd -
    //    endif
    //  endif
    GA_CONCAT_LITERAL(&ga, ":if !exists('+acd')||!&acd|if haslocaldir()|");
# ifdef MSWIN
    // in case :set shellslash is set, need to normalize the directory separators
    // '/' is not valid in a filename so replacing '/' by '\\' should be safe
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd()->tr('/','\\') ==# '");
# অন্যথা
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.দৈর্ঘ্য);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].দৈর্ঘ্য);

    যদি (sendReply)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    যদি (inicmd != ফাঁকা)
    {
	// Can't use <CR> after "inicmd", because a "startinsert" would cause
	// the following commands to be inserted as text.  Use a "|",
	// hopefully "inicmd" does allow this...
	ga_concat(&ga, inicmd);
	GA_CONCAT_LITERAL(&ga, "|");
    }
    // Bring the window to the foreground, goto Insert mode when 'im' set and
    // clear command line.
    GA_CONCAT_LITERAL(&ga, "cal foreground()|if &im|star|en|redr|f<CR>");
    ga_append(&ga, NUL);
    ফেরত ga.ga_তথ্য;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    স্থির char_u *
serverMakeName(char_u *arg, অক্ষর *cmd)
{
    char_u *p;

    যদি (arg != ফাঁকা && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	{
	    যদি (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    অন্যথা
		p = vim_strsave_up(arg);
	}
	অন্যথা
	    p = vim_strsave_up(arg);
# অন্যথা
	p = vim_strsave_up(arg);
# endif
    }
    অন্যথা
    {
	p = vim_strsave_up(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	যদি (p != ফাঁকা && vim_strchr(p, '.') != ফাঁকা)
	    *vim_strchr(p, '.') = NUL;
    }
    ফেরত p;
}
#endif // FEAT_CLIENTSERVER

#যদি defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    স্থির শূন্য
make_connection(শূন্য)
{
    যদি (X_DISPLAY == ফাঁকা
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
    {
	x_force_connect = TRUE;
	setup_term_clip();
	x_force_connect = FALSE;
    }
}

    স্থির পূর্ণসংখ্যা
check_connection(শূন্য)
{
    যদি (clientserver_method != CLIENTSERVER_METHOD_X11)
	ফেরত OK;
    make_connection();
    যদি (X_DISPLAY == ফাঁকা)
    {
	emsg(_(e_no_connection_to_x_server));
	ফেরত FAIL;
    }
    ফেরত OK;
}
#endif

#ifdef FEAT_CLIENTSERVER
    স্থির শূন্য
remote_common(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা রাশিমালা)
{
    char_u	*server_name;
    char_u	*চাবিগুলি;
    char_u	*r = ফাঁকা;
    char_u	buf[NUMBUFLEN];
    পূর্ণসংখ্যা		timeout = 0;
# ifdef MSWIN
    HWND	w;
# অন্যথা
#  ifdef FEAT_X11
    Window	w;
#  endif
# endif
    পূর্ণসংখ্যা		ret = OK;

    যদি (check_restricted() || check_secure())
	ফেরত;

# ifdef FEAT_X11
    যদি (check_connection() == FAIL)
	ফেরত;
# endif
    যদি (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_সংখ্যা(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    যদি (server_name == ফাঁকা)
	ফেরত;		// type error; errmsg already given
    চাবিগুলি = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    যদি (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendToVim(server_name, চাবিগুলি, &r, &w, রাশিমালা,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET
	&& socketserver_send(server_name, চাবিগুলি, &r, রাশিমালা,
	    timeout * 1000, true, ফাঁকা) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    যদি (clientserver_method == CLIENTSERVER_METHOD_X11)
	যদি (serverSendToVim(X_DISPLAY, server_name, চাবিগুলি, &r, &w, রাশিমালা, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    যদি (ret == FAIL)
    {
	যদি (r != ফাঁকা)
	{
	    emsg((অক্ষর *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	অন্যথা
	    semsg(_(e_unable_to_send_to_str), server_name);
	ফেরত;
    }

    rettv->vval.v_string = r;

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# যদি defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = ফাঁকা;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	যদি (idvar != ফাঁকা && *idvar != NUL)
	{
# ifdef MSWIN
	    যদি (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	    {
		sprintf((অক্ষর *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    যদি (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		sprintf((অক্ষর *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		str = server_name;
# endif
	    যদি (str == ফাঁকা)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    set_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    শূন্য
f_remote_রাশিমালা(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

# ifdef FEAT_CLIENTSERVER
    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 3) == FAIL)))
	ফেরত;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    শূন্য
f_remote_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	যদি (server_name != ফাঁকা)
	    serverForeground(server_name);
    }
#  অন্যথা
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    শূন্য
f_remote_peek(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = ফাঁকা;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_সংখ্যা = -1;
    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ফেরত;

    serverid = tv_get_string_chk(&argvars[0]);
    যদি (serverid == ফাঁকা)
	ফেরত;		// type error; errmsg already given
#  ifdef MSWIN
    যদি (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	sscanf((ধ্রুবক অক্ষর *)serverid, SCANF_HEX_LONG_U, &n);
	যদি (n == 0)
	    rettv->vval.v_সংখ্যা = -1;
	অন্যথা
	{
	    s = serverGetReply((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_সংখ্যা = (s != ফাঁকা);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	rettv->vval.v_সংখ্যা = socketserver_peek_reply(serverid, &s);
#  endif
#  ifdef FEAT_X11
    যদি (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	যদি (check_connection() == FAIL)
	    ফেরত;

	rettv->vval.v_সংখ্যা = serverPeekReply(X_DISPLAY,
		serverStrToWin(serverid), &s);
    }
#  endif

    যদি (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_সংখ্যা > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	যদি (retvar != ফাঁকা)
	    set_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# অন্যথা
    rettv->vval.v_সংখ্যা = -1;
# endif
}

    শূন্য
f_remote_read(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*r = ফাঁকা;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    serverid = tv_get_string_chk(&argvars[0]);
    যদি (serverid != ফাঁকা && !check_restricted() && !check_secure())
    {
	পূর্ণসংখ্যা timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	যদি (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_সংখ্যা(&argvars[1]);

#  ifdef MSWIN
	যদি (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	{
	    sscanf((অক্ষর *)serverid, SCANF_HEX_LONG_U, &n);
	    যদি (n != 0)
		r = serverGetReply((HWND)n, FALSE, TRUE, TRUE, timeout);
	    যদি (r == ফাঁকা)
		emsg(_(e_unable_to_read_server_reply));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
		socketserver_read_reply(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_to_read_server_reply));
#  endif
#  ifdef FEAT_X11
	যদি (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		(check_connection() == FAIL
		|| serverReadReply(X_DISPLAY, serverStrToWin(serverid),
						       &r, FALSE, timeout) < 0))
	    emsg(_(e_unable_to_read_server_reply));
#  endif
    }
# endif
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = r;
}

/*
 * "remote_send()" function
 */
    শূন্য
f_remote_send(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

# ifdef FEAT_CLIENTSERVER
    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ফেরত;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    শূন্য
f_remote_startserver(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    যদি (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (serverName != ফাঁকা)
    {
	emsg(_(e_already_started_server));
	ফেরত;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    যদি (server == ফাঁকা)
	ফেরত;
#  ifdef MSWIN
    যদি (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	serverSetName(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	socketserver_start(server, true);
#  endif
#  ifdef FEAT_X11
    যদি (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    check_connection() == OK)
	serverRegisterName(X_DISPLAY, server);
#  endif

# অন্যথা
    emsg(_(e_clientserver_feature_not_available));
# endif
}

    শূন্য
f_server2client(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*reply;
    পূর্ণসংখ্যা		ret = OK;

    rettv->vval.v_সংখ্যা = -1;
    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    server = tv_get_string_chk(&argvars[0]);
    reply = tv_get_string_buf_chk(&argvars[1], buf);
    যদি (server == ফাঁকা || reply == ফাঁকা)
	ফেরত;

#  ifdef FEAT_SOCKETSERVER
    যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
	    socketserver_send_reply(server, reply) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    যদি (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    যদি (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif

    যদি (ret == FAIL)
    {
	emsg(_(e_unable_to_send_to_client));
	ফেরত;
    }
    rettv->vval.v_সংখ্যা = 0;
# অন্যথা
    rettv->vval.v_সংখ্যা = -1;
# endif
}

    শূন্য
f_serverlist(typval_T *argvars UNUSED, typval_T *rettv)
{
    list_T  *list = ফাঁকা;
    bool    use_list = false;

    যদি (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    যদি (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	list = serverGetVimNames();
#  endif
#  ifdef FEAT_SOCKETSERVER
    যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    যদি (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	make_connection();
	যদি (X_DISPLAY != ফাঁকা)
	    list = serverGetVimNames(X_DISPLAY);
    }
#  endif
# endif
    যদি (use_list)
    {
	যদি (list == ফাঁকা)
	{
	    (শূন্য)rettv_list_alloc(rettv);
	    ফেরত;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    অন্যথা
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	যদি (list != ফাঁকা)
	{
	    list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
	    ga_append(&ga, NUL);
	    list_free(list);
	}

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = (char_u *)ga.ga_তথ্য;
    }
}
#endif

#ifdef FEAT_CLIENTSERVER_BACKENDS
/*
 * Check the $VIM_CLIENTSERVER environment variable and set the method
 */
    শূন্য
check_clientserver_method_env(শূন্য)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    যদি (env == ফাঁকা)
	ফেরত;

    যদি (STRICMP(env, "socket") == 0)
	clientserver_method = CLIENTSERVER_METHOD_SOCKET;
# ifdef FEAT_X11
    অন্যথা যদি (STRICMP(env, "x11") == 0)
	clientserver_method = CLIENTSERVER_METHOD_X11;
# endif
# ifdef MSWIN
    অন্যথা যদি (STRICMP(env, "mswin") == 0)
	clientserver_method = CLIENTSERVER_METHOD_MSWIN;
# endif
}
#endif
