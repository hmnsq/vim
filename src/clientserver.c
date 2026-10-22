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

#अन्तर्भाव "vim.h"

#जर defined(FEAT_CLIENTSERVER)

स्थिर शून्य cmdsrv_main(पूर्णांक *argc, अक्षर **argv, char_u *serverName_arg, char_u **serverStr);
स्थिर char_u *serverMakeName(char_u *arg, अक्षर *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    शून्य
server_to_आदान_buf(char_u *str)
{
    char_u      *ptr = रिक्त;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, रिक्त);
    p_cpo = cpo_save;

    जर (ptr != रिक्त && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	जर (typebuf.tb_maplen < typebuf.tb_len)
	    del_typebuf(typebuf.tb_len - typebuf.tb_maplen, typebuf.tb_maplen);
	(शून्य)ins_typebuf(str, REMAP_NONE, typebuf.tb_len, TRUE, FALSE);

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
eval_client_अभिव्यक्ती_to_string(char_u *अभिव्यक्ती)
{
    char_u	*res;
    पूर्णांक		save_dbl = debug_break_level;
    पूर्णांक		save_ro = redir_off;
    funccal_entry_T funccal_entry;
    पूर्णांक		did_save_funccal = FALSE;

# जर defined(FEAT_EVAL)
    ch_log(रिक्त, "eval_client_expr_to_string(\"%s\")", अभिव्यक्ती);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    जर (!debug_mode)
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

    res = eval_to_string(अभिव्यक्ती, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    जर (emsg_silent < 0)
	emsg_silent = 0;
    जर (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    out_flush_cursor(FALSE, FALSE);

    परत res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    पूर्णांक
sendToLocalVim(char_u *cmd, पूर्णांक as_अभिव्यक्ती, char_u **result)
{
    जर (as_अभिव्यक्ती)
    {
	char_u *ret;

	ret = eval_client_अभिव्यक्ती_to_string(cmd);
	जर (result != रिक्त)
	{
	    जर (ret == रिक्त)
	    {
		अक्षर	*err = _(e_invalid_expression_received);
		माप_प्रकार	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*संदेश;

		संदेश = alloc(len);
		जर (संदेश != रिक्त)
		    vim_snprintf((अक्षर *)संदेश, len, "%s: \"%s\"", err, cmd);
		*result = संदेश;
	    }
	    अन्यथा
		*result = ret;
	}
	अन्यथा
	    vim_free(ret);
	परत ret == रिक्त ? -1 : 0;
    }
    server_to_आदान_buf(cmd);
    परत 0;
}

/*
 * If conversion is needed, convert "data" from "client_enc" to 'encoding' and
 * return an allocated string.  Otherwise return "data".
 * "*tofree" is set to the result when it needs to be freed later.
 */
    char_u *
serverConvert(
    char_u *client_enc UNUSED,
    char_u *data,
    char_u **tofree)
{
    char_u	*res = data;

    *tofree = रिक्त;
    जर (client_enc == रिक्त || p_enc == रिक्त)
	परत res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    जर (convert_setup(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, data, रिक्त);
	जर (res == रिक्त)
	    res = data;
	अन्यथा
	    *tofree = res;
    }
    convert_setup(&vimconv, रिक्त, रिक्त);
    परत res;
}
#endif

#जर defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

स्थिर char_u *build_drop_cmd(पूर्णांक filec, अक्षर **filev, पूर्णांक tabs, पूर्णांक sendReply);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    शून्य
exec_on_server(mparm_T *parmp)
{
    पूर्णांक made_name = FALSE;

    जर (parmp->serverName_arg != रिक्त && *parmp->serverName_arg == NUL)
	परत;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    जर (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    जर (!made_name && parmp->servername == रिक्त)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    जर (parmp->servername != रिक्त && clientserver_method == CLIENTSERVER_METHOD_MSWIN)
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
    शून्य
prepare_server(mparm_T *parmp)
{
# जर defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    जर (
#  जर defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != रिक्त
#   endif
#   जर defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   जर defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_method == CLIENTSERVER_METHOD_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != रिक्त && (
#  जर defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   जर defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_method != CLIENTSERVER_METHOD_SOCKET &&
#   endif
#   जर defined(FEAT_AUTOSERVERNAME)
		    1
#   अन्यथा
		    gui.in_use
#   endif
#   ifdef UNIX
		 && getuid() != मूळ_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != रिक्त))
    {

#  ifdef FEAT_SOCKETSERVER
	जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	    socketserver_start(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	जर (clientserver_method == CLIENTSERVER_METHOD_X11)
	{
	    (शून्य)serverRegisterName(X_DISPLAY, parmp->servername);
	    TIME_संदेश("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    अन्यथा
    {
#  ifdef FEAT_X11
	जर (clientserver_method == CLIENTSERVER_METHOD_X11)
	    serverDelayedStartName = parmp->servername;
	अन्यथा
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    जर (parmp->serverStr != रिक्त)
    {
	char_u *p;

	server_to_आदान_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    स्थिर शून्य
cmdsrv_main(
    पूर्णांक		*argc,
    अक्षर	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    पूर्णांक		i;
    char_u	*sname;
    पूर्णांक		ret = -1;
    पूर्णांक		didone = FALSE;
    पूर्णांक		exiterr = 0;
    अक्षर	**newArgV = argv + 1;
    पूर्णांक		newArgC = 1,
		Argc = *argc;
    पूर्णांक		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    पूर्णांक		silent = FALSE;
    पूर्णांक		tabs = FALSE;
# ifdef MSWIN
    HWND	srv;
# elif defined(FEAT_X11)
    Window	srv;

    setup_term_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = रिक्त;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    जर (sname == रिक्त)
	परत;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    साठी (i = 1; i < Argc; i++)
    {
	res = रिक्त;
	जर (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    साठी (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    थांबा;
	}

	जर (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	अन्यथा जर (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    अक्षर	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    पर्यंत (*p != NUL)
	    {
		जर (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		अन्यथा जर (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		अन्यथा जर (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		अन्यथा
		{
		    argtype = ARGTYPE_OTHER;
		    थांबा;
		}
	    }
	}
	अन्यथा
	    argtype = ARGTYPE_OTHER;

	जर (argtype != ARGTYPE_OTHER)
	{
	    जर (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    जर (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    अन्यथा
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		जर (*serverStr == रिक्त)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    थांबा;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		ret = socketserver_send(sname, *serverStr, रिक्त, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    जर (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		जर (xterm_dpy == रिक्त)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		अन्यथा
		    ret = serverSendToVim(xterm_dpy, sname, *serverStr,
			    रिक्त, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    जर (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		ret = serverSendToVim(sname, *serverStr, रिक्त, &srv, 0, 0,
			silent);
# endif
	    जर (ret < 0)
	    {
		जर (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		अन्यथा जर (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		थांबा;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    जर (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		SetForegroundWindow(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    जर (argtype == ARGTYPE_EDIT_WAIT)
	    {
		पूर्णांक	अंक_Files = *argc - i - 1;
		char_u  *done = alloc(अंक_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		पूर्णांक	मोजणी = 0;
		बाह्य HWND message_window;
# endif

		जर (अंक_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --अंक_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cbSize = आकार(ni);
		ni.hWnd = message_window;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		प्र_लिखो(ni.szTip, _("%d of %d edited"), मोजणी, अंक_Files);
		Shell_NotifyIcon(NIM_ADD, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(done, 0, अंक_Files);
		पर्यंत (स_प्रथम(done, 0, अंक_Files) != रिक्त)
		{
		    char_u  *p = रिक्त;
		    पूर्णांक	    j;
# ifdef MSWIN
		    जर (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
			p = serverGetReply(srv, रिक्त, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET
			    && (ch == रिक्त
				|| socketserver_read_reply(sname, &p, -1, true)
				== FAIL))
			    थांबा;
# endif
# ifdef FEAT_X11
		    जर (clientserver_method == CLIENTSERVER_METHOD_X11
			    && serverReadReply(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    थांबा;
# endif
		    जर (p == रिक्त)
			थांबा;
		    j = म_से_प((अक्षर *)p);
		    vim_free(p);
		    जर (j >= 0 && j < अंक_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++मोजणी;
			प्र_लिखो(ni.szTip, _("%d of %d edited"),
							     मोजणी, अंक_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			done[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		जर (ch != रिक्त)
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
	अन्यथा जर (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    पूर्णांक status = OK;

	    जर (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    जर (clientserver_method == CLIENTSERVER_METHOD_MSWIN
		    && serverSendToVim(sname, (char_u *)argv[i + 1],
						  &res, रिक्त, 1, 0, FALSE) < 0)
		status = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, रिक्त) < 0)
		status = FAIL;
# endif
# ifdef FEAT_X11
	    जर (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		जर (xterm_dpy == रिक्त)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		अन्यथा जर (serverSendToVim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    रिक्त, 1, 0, 1, FALSE) < 0)
		    status = FAIL;
	    }
# endif
	    जर (status == FAIL)
	    {
		जर (res != रिक्त && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((अक्षर *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	अन्यथा जर (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = रिक्त;
	    garray_T	ga;

# ifdef MSWIN
	    जर (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		list = serverGetVimNames();
# endif
# ifdef FEAT_SOCKETSERVER
	    जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    जर (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		    xterm_dpy != रिक्त)
		list = serverGetVimNames(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    जर (list != रिक्त)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_data;

	    जर (did_emsg)
		mch_errmsg("\n");
	}
	अन्यथा जर (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    सुरूठेवा;
	}
	अन्यथा
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    सुरूठेवा;
	}
	didone = TRUE;
	जर (res != रिक्त && *res != NUL)
	{
	    mch_संदेश((अक्षर *)res);
	    जर (res[STRLEN(res) - 1] != '\n')
		mch_संदेश("\n");
	}
	vim_free(res);
    }

    जर (didone)
    {
	display_errors();	// display any collected messages
	निकास(exiterr);	// Mission accomplished - get out
    }

    // Return back into main()
    *argc = newArgC;
    vim_free(sname);
}

/*
 * Build a ":drop" command to send to a Vim server.
 */
    स्थिर char_u *
build_drop_cmd(
    पूर्णांक		filec,
    अक्षर	**filev,
    पूर्णांक		tabs,		// Use ":tab drop" instead of ":drop".
    पूर्णांक		sendReply)
{
    garray_T	ga;
    पूर्णांक		i;
    char_u	*inicmd = रिक्त;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    स्थिरांक string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    जर (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    जर (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    जर (cwd == रिक्त)
	परत रिक्त;
    जर (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	परत रिक्त;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_IN_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# अन्यथा
	    PATH_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    जर (cdp.string == रिक्त)
	परत रिक्त;
    cdp.length = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.length);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].length);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    जर (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    साठी (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		PATH_ESC_CHARS
# अन्यथा
		(char_u *)" \t%#"
# endif
		);
	जर (p == रिक्त)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_data);
	    परत रिक्त;
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
# अन्यथा
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.length);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].length);

    जर (sendReply)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    जर (inicmd != रिक्त)
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
    परत ga.ga_data;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    स्थिर char_u *
serverMakeName(char_u *arg, अक्षर *cmd)
{
    char_u *p;

    जर (arg != रिक्त && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	{
	    जर (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    अन्यथा
		p = vim_strsave_up(arg);
	}
	अन्यथा
	    p = vim_strsave_up(arg);
# अन्यथा
	p = vim_strsave_up(arg);
# endif
    }
    अन्यथा
    {
	p = vim_strsave_up(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	जर (p != रिक्त && vim_strchr(p, '.') != रिक्त)
	    *vim_strchr(p, '.') = NUL;
    }
    परत p;
}
#endif // FEAT_CLIENTSERVER

#जर defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    स्थिर शून्य
make_connection(शून्य)
{
    जर (X_DISPLAY == रिक्त
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

    स्थिर पूर्णांक
check_connection(शून्य)
{
    जर (clientserver_method != CLIENTSERVER_METHOD_X11)
	परत OK;
    make_connection();
    जर (X_DISPLAY == रिक्त)
    {
	emsg(_(e_no_connection_to_x_server));
	परत FAIL;
    }
    परत OK;
}
#endif

#ifdef FEAT_CLIENTSERVER
    स्थिर शून्य
remote_common(typval_T *argvars, typval_T *rettv, पूर्णांक अभिव्यक्ती)
{
    char_u	*server_name;
    char_u	*किल्ल्या;
    char_u	*r = रिक्त;
    char_u	buf[NUMBUFLEN];
    पूर्णांक		timeout = 0;
# ifdef MSWIN
    HWND	w;
# अन्यथा
#  ifdef FEAT_X11
    Window	w;
#  endif
# endif
    पूर्णांक		ret = OK;

    जर (check_restricted() || check_secure())
	परत;

# ifdef FEAT_X11
    जर (check_connection() == FAIL)
	परत;
# endif
    जर (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_संख्या(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    जर (server_name == रिक्त)
	परत;		// type error; errmsg already given
    किल्ल्या = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    जर (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendToVim(server_name, किल्ल्या, &r, &w, अभिव्यक्ती,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET
	&& socketserver_send(server_name, किल्ल्या, &r, अभिव्यक्ती,
	    timeout * 1000, true, रिक्त) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    जर (clientserver_method == CLIENTSERVER_METHOD_X11)
	जर (serverSendToVim(X_DISPLAY, server_name, किल्ल्या, &r, &w, अभिव्यक्ती, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    जर (ret == FAIL)
    {
	जर (r != रिक्त)
	{
	    emsg((अक्षर *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	अन्यथा
	    semsg(_(e_unable_to_send_to_str), server_name);
	परत;
    }

    rettv->vval.v_string = r;

    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# जर defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = रिक्त;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	जर (idvar != रिक्त && *idvar != NUL)
	{
# ifdef MSWIN
	    जर (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	    {
		प्र_लिखो((अक्षर *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    जर (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		प्र_लिखो((अक्षर *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		str = server_name;
# endif
	    जर (str == रिक्त)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    set_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#endif

#जर defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    शून्य
f_remote_अभिव्यक्ती(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

# ifdef FEAT_CLIENTSERVER
    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 3) == FAIL)))
	परत;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    शून्य
f_remote_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	जर (server_name != रिक्त)
	    serverForeground(server_name);
    }
#  अन्यथा
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    शून्य
f_remote_peek(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = रिक्त;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_संख्या = -1;
    जर (check_restricted() || check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	परत;

    serverid = tv_get_string_chk(&argvars[0]);
    जर (serverid == रिक्त)
	परत;		// type error; errmsg already given
#  ifdef MSWIN
    जर (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	माला_पूछो((स्थिरांक अक्षर *)serverid, SCANF_HEX_LONG_U, &n);
	जर (n == 0)
	    rettv->vval.v_संख्या = -1;
	अन्यथा
	{
	    s = serverGetReply((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_संख्या = (s != रिक्त);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	rettv->vval.v_संख्या = socketserver_peek_reply(serverid, &s);
#  endif
#  ifdef FEAT_X11
    जर (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	जर (check_connection() == FAIL)
	    परत;

	rettv->vval.v_संख्या = serverPeekReply(X_DISPLAY,
		serverStrToWin(serverid), &s);
    }
#  endif

    जर (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_संख्या > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	जर (retvar != रिक्त)
	    set_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# अन्यथा
    rettv->vval.v_संख्या = -1;
# endif
}

    शून्य
f_remote_read(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*r = रिक्त;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL))
	परत;

    serverid = tv_get_string_chk(&argvars[0]);
    जर (serverid != रिक्त && !check_restricted() && !check_secure())
    {
	पूर्णांक timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	जर (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_संख्या(&argvars[1]);

#  ifdef MSWIN
	जर (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	{
	    माला_पूछो((अक्षर *)serverid, SCANF_HEX_LONG_U, &n);
	    जर (n != 0)
		r = serverGetReply((HWND)n, FALSE, TRUE, TRUE, timeout);
	    जर (r == रिक्त)
		emsg(_(e_unable_to_read_server_reply));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
		socketserver_read_reply(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_to_read_server_reply));
#  endif
#  ifdef FEAT_X11
	जर (clientserver_method == CLIENTSERVER_METHOD_X11 &&
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
    शून्य
f_remote_send(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

# ifdef FEAT_CLIENTSERVER
    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	परत;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    शून्य
f_remote_startserver(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    जर (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	परत;

    जर (serverName != रिक्त)
    {
	emsg(_(e_already_started_server));
	परत;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    जर (server == रिक्त)
	परत;
#  ifdef MSWIN
    जर (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	serverSetName(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	socketserver_start(server, true);
#  endif
#  ifdef FEAT_X11
    जर (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    check_connection() == OK)
	serverRegisterName(X_DISPLAY, server);
#  endif

# अन्यथा
    emsg(_(e_clientserver_feature_not_available));
# endif
}

    शून्य
f_server2client(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*reply;
    पूर्णांक		ret = OK;

    rettv->vval.v_संख्या = -1;
    जर (check_restricted() || check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	परत;

    server = tv_get_string_chk(&argvars[0]);
    reply = tv_get_string_buf_chk(&argvars[1], buf);
    जर (server == रिक्त || reply == रिक्त)
	परत;

#  ifdef FEAT_SOCKETSERVER
    जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
	    socketserver_send_reply(server, reply) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    जर (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    जर (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif

    जर (ret == FAIL)
    {
	emsg(_(e_unable_to_send_to_client));
	परत;
    }
    rettv->vval.v_संख्या = 0;
# अन्यथा
    rettv->vval.v_संख्या = -1;
# endif
}

    शून्य
f_serverlist(typval_T *argvars UNUSED, typval_T *rettv)
{
    list_T  *list = रिक्त;
    bool    use_list = false;

    जर (check_for_opt_dict_arg(argvars, 0) == FAIL)
	परत;

    जर (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    जर (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	list = serverGetVimNames();
#  endif
#  ifdef FEAT_SOCKETSERVER
    जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    जर (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	make_connection();
	जर (X_DISPLAY != रिक्त)
	    list = serverGetVimNames(X_DISPLAY);
    }
#  endif
# endif
    जर (use_list)
    {
	जर (list == रिक्त)
	{
	    (शून्य)rettv_list_alloc(rettv);
	    परत;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    अन्यथा
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	जर (list != रिक्त)
	{
	    list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
	    ga_append(&ga, NUL);
	    list_free(list);
	}

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = (char_u *)ga.ga_data;
    }
}
#endif

#ifdef FEAT_CLIENTSERVER_BACKENDS
/*
 * Check the $VIM_CLIENTSERVER environment variable and set the method
 */
    शून्य
check_clientserver_method_env(शून्य)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    जर (env == रिक्त)
	परत;

    जर (STRICMP(env, "socket") == 0)
	clientserver_method = CLIENTSERVER_METHOD_SOCKET;
# ifdef FEAT_X11
    अन्यथा जर (STRICMP(env, "x11") == 0)
	clientserver_method = CLIENTSERVER_METHOD_X11;
# endif
# ifdef MSWIN
    अन्यथा जर (STRICMP(env, "mswin") == 0)
	clientserver_method = CLIENTSERVER_METHOD_MSWIN;
# endif
}
#endif
