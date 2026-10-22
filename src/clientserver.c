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

#जइ defined(FEAT_CLIENTSERVER)

स्थिर शून्य cmdsrv_main(पूर्णाङ्क *argc, वर्ण **argv, char_u *serverName_arg, char_u **serverStr);
स्थिर char_u *serverMakeName(char_u *arg, वर्ण *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    शून्य
server_to_निवेशः_buf(char_u *str)
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

    जइ (ptr != रिक्त && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	जइ (typebuf.tb_maplen < typebuf.tb_len)
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
eval_client_व्यञ्जकम्_to_string(char_u *व्यञ्जकम्)
{
    char_u	*res;
    पूर्णाङ्क		save_dbl = debug_break_level;
    पूर्णाङ्क		save_ro = redir_off;
    funccal_entry_T funccal_entry;
    पूर्णाङ्क		did_save_funccal = FALSE;

# जइ defined(FEAT_EVAL)
    ch_log(रिक्त, "eval_client_expr_to_string(\"%s\")", व्यञ्जकम्);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    जइ (!debug_mode)
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

    res = eval_to_string(व्यञ्जकम्, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    जइ (emsg_silent < 0)
	emsg_silent = 0;
    जइ (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    out_flush_cursor(FALSE, FALSE);

    प्रत्यागम res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    पूर्णाङ्क
sendToLocalVim(char_u *cmd, पूर्णाङ्क as_व्यञ्जकम्, char_u **result)
{
    जइ (as_व्यञ्जकम्)
    {
	char_u *ret;

	ret = eval_client_व्यञ्जकम्_to_string(cmd);
	जइ (result != रिक्त)
	{
	    जइ (ret == रिक्त)
	    {
		वर्ण	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*सन्देशः;

		सन्देशः = alloc(len);
		जइ (सन्देशः != रिक्त)
		    vim_snprintf((वर्ण *)सन्देशः, len, "%s: \"%s\"", err, cmd);
		*result = सन्देशः;
	    }
	    अण्णहा
		*result = ret;
	}
	अण्णहा
	    vim_free(ret);
	प्रत्यागम ret == रिक्त ? -1 : 0;
    }
    server_to_निवेशः_buf(cmd);
    प्रत्यागम 0;
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
    जइ (client_enc == रिक्त || p_enc == रिक्त)
	प्रत्यागम res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    जइ (convert_setup(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, data, रिक्त);
	जइ (res == रिक्त)
	    res = data;
	अण्णहा
	    *tofree = res;
    }
    convert_setup(&vimconv, रिक्त, रिक्त);
    प्रत्यागम res;
}
#endif

#जइ defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

स्थिर char_u *build_drop_cmd(पूर्णाङ्क filec, वर्ण **filev, पूर्णाङ्क tabs, पूर्णाङ्क sendReply);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    शून्य
exec_on_server(mparm_T *parmp)
{
    पूर्णाङ्क made_name = FALSE;

    जइ (parmp->serverName_arg != रिक्त && *parmp->serverName_arg == NUL)
	प्रत्यागम;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    जइ (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    जइ (!made_name && parmp->servername == रिक्त)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    जइ (parmp->servername != रिक्त && clientserver_method == CLIENTSERVER_METHOD_MSWIN)
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
# जइ defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    जइ (
#  जइ defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != रिक्त
#   endif
#   जइ defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   जइ defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_method == CLIENTSERVER_METHOD_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != रिक्त && (
#  जइ defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   जइ defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_method != CLIENTSERVER_METHOD_SOCKET &&
#   endif
#   जइ defined(FEAT_AUTOSERVERNAME)
		    1
#   अण्णहा
		    gui.in_use
#   endif
#   ifdef UNIX
		 && getuid() != मूलम्_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != रिक्त))
    {

#  ifdef FEAT_SOCKETSERVER
	जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	    socketserver_start(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	जइ (clientserver_method == CLIENTSERVER_METHOD_X11)
	{
	    (शून्य)serverRegisterName(X_DISPLAY, parmp->servername);
	    TIME_सन्देशः("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    अण्णहा
    {
#  ifdef FEAT_X11
	जइ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    serverDelayedStartName = parmp->servername;
	अण्णहा
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    जइ (parmp->serverStr != रिक्त)
    {
	char_u *p;

	server_to_निवेशः_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    स्थिर शून्य
cmdsrv_main(
    पूर्णाङ्क		*argc,
    वर्ण	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    पूर्णाङ्क		i;
    char_u	*sname;
    पूर्णाङ्क		ret = -1;
    पूर्णाङ्क		didone = FALSE;
    पूर्णाङ्क		exiterr = 0;
    वर्ण	**newArgV = argv + 1;
    पूर्णाङ्क		newArgC = 1,
		Argc = *argc;
    पूर्णाङ्क		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    पूर्णाङ्क		silent = FALSE;
    पूर्णाङ्क		tabs = FALSE;
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
    जइ (sname == रिक्त)
	प्रत्यागम;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    प्रति (i = 1; i < Argc; i++)
    {
	res = रिक्त;
	जइ (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    प्रति (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    भङ्ग;
	}

	जइ (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	अण्णहा जइ (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    वर्ण	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    जाव (*p != NUL)
	    {
		जइ (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		अण्णहा जइ (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		अण्णहा जइ (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		अण्णहा
		{
		    argtype = ARGTYPE_OTHER;
		    भङ्ग;
		}
	    }
	}
	अण्णहा
	    argtype = ARGTYPE_OTHER;

	जइ (argtype != ARGTYPE_OTHER)
	{
	    जइ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    जइ (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    अण्णहा
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		जइ (*serverStr == रिक्त)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    भङ्ग;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		ret = socketserver_send(sname, *serverStr, रिक्त, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    जइ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		जइ (xterm_dpy == रिक्त)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		अण्णहा
		    ret = serverSendToVim(xterm_dpy, sname, *serverStr,
			    रिक्त, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    जइ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		ret = serverSendToVim(sname, *serverStr, रिक्त, &srv, 0, 0,
			silent);
# endif
	    जइ (ret < 0)
	    {
		जइ (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		अण्णहा जइ (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		भङ्ग;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    जइ (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		SetForegroundWindow(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    जइ (argtype == ARGTYPE_EDIT_WAIT)
	    {
		पूर्णाङ्क	अङ्कः_Files = *argc - i - 1;
		char_u  *done = alloc(अङ्कः_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		पूर्णाङ्क	गणनम् = 0;
		बाह्य HWND message_window;
# endif

		जइ (अङ्कः_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --अङ्कः_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cbSize = परिमाण(ni);
		ni.hWnd = message_window;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), गणनम्, अङ्कः_Files);
		Shell_NotifyIcon(NIM_ADD, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(done, 0, अङ्कः_Files);
		जाव (memchr(done, 0, अङ्कः_Files) != रिक्त)
		{
		    char_u  *p = रिक्त;
		    पूर्णाङ्क	    j;
# ifdef MSWIN
		    जइ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
			p = serverGetReply(srv, रिक्त, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET
			    && (ch == रिक्त
				|| socketserver_read_reply(sname, &p, -1, true)
				== FAIL))
			    भङ्ग;
# endif
# ifdef FEAT_X11
		    जइ (clientserver_method == CLIENTSERVER_METHOD_X11
			    && serverReadReply(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    भङ्ग;
# endif
		    जइ (p == रिक्त)
			भङ्ग;
		    j = atoi((वर्ण *)p);
		    vim_free(p);
		    जइ (j >= 0 && j < अङ्कः_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++गणनम्;
			sprintf(ni.szTip, _("%d of %d edited"),
							     गणनम्, अङ्कः_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			done[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		जइ (ch != रिक्त)
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
	अण्णहा जइ (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    पूर्णाङ्क status = OK;

	    जइ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    जइ (clientserver_method == CLIENTSERVER_METHOD_MSWIN
		    && serverSendToVim(sname, (char_u *)argv[i + 1],
						  &res, रिक्त, 1, 0, FALSE) < 0)
		status = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, रिक्त) < 0)
		status = FAIL;
# endif
# ifdef FEAT_X11
	    जइ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		जइ (xterm_dpy == रिक्त)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		अण्णहा जइ (serverSendToVim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    रिक्त, 1, 0, 1, FALSE) < 0)
		    status = FAIL;
	    }
# endif
	    जइ (status == FAIL)
	    {
		जइ (res != रिक्त && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((वर्ण *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	अण्णहा जइ (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = रिक्त;
	    garray_T	ga;

# ifdef MSWIN
	    जइ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		list = serverGetVimNames();
# endif
# ifdef FEAT_SOCKETSERVER
	    जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    जइ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		    xterm_dpy != रिक्त)
		list = serverGetVimNames(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    जइ (list != रिक्त)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_data;

	    जइ (did_emsg)
		mch_errmsg("\n");
	}
	अण्णहा जइ (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    अनुवर्त;
	}
	अण्णहा
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    अनुवर्त;
	}
	didone = TRUE;
	जइ (res != रिक्त && *res != NUL)
	{
	    mch_सन्देशः((वर्ण *)res);
	    जइ (res[STRLEN(res) - 1] != '\n')
		mch_सन्देशः("\n");
	}
	vim_free(res);
    }

    जइ (didone)
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
    स्थिर char_u *
build_drop_cmd(
    पूर्णाङ्क		filec,
    वर्ण	**filev,
    पूर्णाङ्क		tabs,		// Use ":tab drop" instead of ":drop".
    पूर्णाङ्क		sendReply)
{
    garray_T	ga;
    पूर्णाङ्क		i;
    char_u	*inicmd = रिक्त;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    ध्रुव string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    जइ (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    जइ (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    जइ (cwd == रिक्त)
	प्रत्यागम रिक्त;
    जइ (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	प्रत्यागम रिक्त;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_IN_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# अण्णहा
	    PATH_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    जइ (cdp.string == रिक्त)
	प्रत्यागम रिक्त;
    cdp.length = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.length);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].length);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    जइ (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    प्रति (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		PATH_ESC_CHARS
# अण्णहा
		(char_u *)" \t%#"
# endif
		);
	जइ (p == रिक्त)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_data);
	    प्रत्यागम रिक्त;
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
# अण्णहा
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.length);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].length);

    जइ (sendReply)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    जइ (inicmd != रिक्त)
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
    प्रत्यागम ga.ga_data;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    स्थिर char_u *
serverMakeName(char_u *arg, वर्ण *cmd)
{
    char_u *p;

    जइ (arg != रिक्त && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	{
	    जइ (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    अण्णहा
		p = vim_strsave_up(arg);
	}
	अण्णहा
	    p = vim_strsave_up(arg);
# अण्णहा
	p = vim_strsave_up(arg);
# endif
    }
    अण्णहा
    {
	p = vim_strsave_up(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	जइ (p != रिक्त && vim_strchr(p, '.') != रिक्त)
	    *vim_strchr(p, '.') = NUL;
    }
    प्रत्यागम p;
}
#endif // FEAT_CLIENTSERVER

#जइ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    स्थिर शून्य
make_connection(शून्य)
{
    जइ (X_DISPLAY == रिक्त
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

    स्थिर पूर्णाङ्क
check_connection(शून्य)
{
    जइ (clientserver_method != CLIENTSERVER_METHOD_X11)
	प्रत्यागम OK;
    make_connection();
    जइ (X_DISPLAY == रिक्त)
    {
	emsg(_(e_no_connection_to_x_server));
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}
#endif

#ifdef FEAT_CLIENTSERVER
    स्थिर शून्य
remote_common(typval_T *argvars, typval_T *rettv, पूर्णाङ्क व्यञ्जकम्)
{
    char_u	*server_name;
    char_u	*कुञ्चिकाः;
    char_u	*r = रिक्त;
    char_u	buf[NUMBUFLEN];
    पूर्णाङ्क		timeout = 0;
# ifdef MSWIN
    HWND	w;
# अण्णहा
#  ifdef FEAT_X11
    Window	w;
#  endif
# endif
    पूर्णाङ्क		ret = OK;

    जइ (check_restricted() || check_secure())
	प्रत्यागम;

# ifdef FEAT_X11
    जइ (check_connection() == FAIL)
	प्रत्यागम;
# endif
    जइ (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_सङ्ख्या(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    जइ (server_name == रिक्त)
	प्रत्यागम;		// type error; errmsg already given
    कुञ्चिकाः = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    जइ (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendToVim(server_name, कुञ्चिकाः, &r, &w, व्यञ्जकम्,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET
	&& socketserver_send(server_name, कुञ्चिकाः, &r, व्यञ्जकम्,
	    timeout * 1000, true, रिक्त) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    जइ (clientserver_method == CLIENTSERVER_METHOD_X11)
	जइ (serverSendToVim(X_DISPLAY, server_name, कुञ्चिकाः, &r, &w, व्यञ्जकम्, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    जइ (ret == FAIL)
    {
	जइ (r != रिक्त)
	{
	    emsg((वर्ण *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	अण्णहा
	    semsg(_(e_unable_to_send_to_str), server_name);
	प्रत्यागम;
    }

    rettv->vval.v_string = r;

    जइ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# जइ defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = रिक्त;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	जइ (idvar != रिक्त && *idvar != NUL)
	{
# ifdef MSWIN
	    जइ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	    {
		sprintf((वर्ण *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    जइ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		sprintf((वर्ण *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		str = server_name;
# endif
	    जइ (str == रिक्त)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    set_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#endif

#जइ defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    शून्य
f_remote_व्यञ्जकम्(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

# ifdef FEAT_CLIENTSERVER
    जइ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_सङ्ख्या_arg(argvars, 3) == FAIL)))
	प्रत्यागम;

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
    जइ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	जइ (server_name != रिक्त)
	    serverForeground(server_name);
    }
#  अण्णहा
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

    rettv->vval.v_सङ्ख्या = -1;
    जइ (check_restricted() || check_secure())
	प्रत्यागम;

    जइ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    serverid = tv_get_string_chk(&argvars[0]);
    जइ (serverid == रिक्त)
	प्रत्यागम;		// type error; errmsg already given
#  ifdef MSWIN
    जइ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	sscanf((ध्रुव वर्ण *)serverid, SCANF_HEX_LONG_U, &n);
	जइ (n == 0)
	    rettv->vval.v_सङ्ख्या = -1;
	अण्णहा
	{
	    s = serverGetReply((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_सङ्ख्या = (s != रिक्त);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	rettv->vval.v_सङ्ख्या = socketserver_peek_reply(serverid, &s);
#  endif
#  ifdef FEAT_X11
    जइ (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	जइ (check_connection() == FAIL)
	    प्रत्यागम;

	rettv->vval.v_सङ्ख्या = serverPeekReply(X_DISPLAY,
		serverStrToWin(serverid), &s);
    }
#  endif

    जइ (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_सङ्ख्या > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	जइ (retvar != रिक्त)
	    set_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# अण्णहा
    rettv->vval.v_सङ्ख्या = -1;
# endif
}

    शून्य
f_remote_read(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*r = रिक्त;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    जइ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    serverid = tv_get_string_chk(&argvars[0]);
    जइ (serverid != रिक्त && !check_restricted() && !check_secure())
    {
	पूर्णाङ्क timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	जइ (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_सङ्ख्या(&argvars[1]);

#  ifdef MSWIN
	जइ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	{
	    sscanf((वर्ण *)serverid, SCANF_HEX_LONG_U, &n);
	    जइ (n != 0)
		r = serverGetReply((HWND)n, FALSE, TRUE, TRUE, timeout);
	    जइ (r == रिक्त)
		emsg(_(e_unable_to_read_server_reply));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
		socketserver_read_reply(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_to_read_server_reply));
#  endif
#  ifdef FEAT_X11
	जइ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
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
    जइ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	प्रत्यागम;

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
    जइ (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    जइ (serverName != रिक्त)
    {
	emsg(_(e_already_started_server));
	प्रत्यागम;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    जइ (server == रिक्त)
	प्रत्यागम;
#  ifdef MSWIN
    जइ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	serverSetName(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	socketserver_start(server, true);
#  endif
#  ifdef FEAT_X11
    जइ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    check_connection() == OK)
	serverRegisterName(X_DISPLAY, server);
#  endif

# अण्णहा
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
    पूर्णाङ्क		ret = OK;

    rettv->vval.v_सङ्ख्या = -1;
    जइ (check_restricted() || check_secure())
	प्रत्यागम;

    जइ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    server = tv_get_string_chk(&argvars[0]);
    reply = tv_get_string_buf_chk(&argvars[1], buf);
    जइ (server == रिक्त || reply == रिक्त)
	प्रत्यागम;

#  ifdef FEAT_SOCKETSERVER
    जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
	    socketserver_send_reply(server, reply) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    जइ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    जइ (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif

    जइ (ret == FAIL)
    {
	emsg(_(e_unable_to_send_to_client));
	प्रत्यागम;
    }
    rettv->vval.v_सङ्ख्या = 0;
# अण्णहा
    rettv->vval.v_सङ्ख्या = -1;
# endif
}

    शून्य
f_serverlist(typval_T *argvars UNUSED, typval_T *rettv)
{
    list_T  *list = रिक्त;
    bool    use_list = false;

    जइ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    जइ (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    जइ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	list = serverGetVimNames();
#  endif
#  ifdef FEAT_SOCKETSERVER
    जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    जइ (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	make_connection();
	जइ (X_DISPLAY != रिक्त)
	    list = serverGetVimNames(X_DISPLAY);
    }
#  endif
# endif
    जइ (use_list)
    {
	जइ (list == रिक्त)
	{
	    (शून्य)rettv_list_alloc(rettv);
	    प्रत्यागम;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    अण्णहा
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	जइ (list != रिक्त)
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

    जइ (env == रिक्त)
	प्रत्यागम;

    जइ (STRICMP(env, "socket") == 0)
	clientserver_method = CLIENTSERVER_METHOD_SOCKET;
# ifdef FEAT_X11
    अण्णहा जइ (STRICMP(env, "x11") == 0)
	clientserver_method = CLIENTSERVER_METHOD_X11;
# endif
# ifdef MSWIN
    अण्णहा जइ (STRICMP(env, "mswin") == 0)
	clientserver_method = CLIENTSERVER_METHOD_MSWIN;
# endif
}
#endif
