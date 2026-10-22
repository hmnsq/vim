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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_CLIENTSERVER)

സ്ഥിതം ശൂന്യം cmdsrv_main(പൂർണ്ണസംഖ്യ *argc, അക്ഷരം **argv, char_u *serverName_arg, char_u **serverStr);
സ്ഥിതം char_u *serverMakeName(char_u *arg, അക്ഷരം *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    ശൂന്യം
server_to_നിവേശം_buf(char_u *str)
{
    char_u      *ptr = ശൂന്യ;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, ശൂന്യ);
    p_cpo = cpo_save;

    എങ്കിൽ (ptr != ശൂന്യ && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	എങ്കിൽ (typebuf.tb_maplen < typebuf.tb_len)
	    del_typebuf(typebuf.tb_len - typebuf.tb_maplen, typebuf.tb_maplen);
	(ശൂന്യം)ins_typebuf(str, REMAP_NONE, typebuf.tb_len, TRUE, FALSE);

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
eval_client_വ്യഞ്ജകം_to_string(char_u *വ്യഞ്ജകം)
{
    char_u	*res;
    പൂർണ്ണസംഖ്യ		save_dbl = debug_break_level;
    പൂർണ്ണസംഖ്യ		save_ro = redir_off;
    funccal_entry_T funccal_entry;
    പൂർണ്ണസംഖ്യ		did_save_funccal = FALSE;

# എങ്കിൽ defined(FEAT_EVAL)
    ch_log(ശൂന്യ, "eval_client_expr_to_string(\"%s\")", വ്യഞ്ജകം);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    എങ്കിൽ (!debug_mode)
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

    res = eval_to_string(വ്യഞ്ജകം, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    എങ്കിൽ (emsg_silent < 0)
	emsg_silent = 0;
    എങ്കിൽ (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    out_flush_cursor(FALSE, FALSE);

    തിരികെ res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    പൂർണ്ണസംഖ്യ
sendToLocalVim(char_u *cmd, പൂർണ്ണസംഖ്യ as_വ്യഞ്ജകം, char_u **result)
{
    എങ്കിൽ (as_വ്യഞ്ജകം)
    {
	char_u *ret;

	ret = eval_client_വ്യഞ്ജകം_to_string(cmd);
	എങ്കിൽ (result != ശൂന്യ)
	{
	    എങ്കിൽ (ret == ശൂന്യ)
	    {
		അക്ഷരം	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*സന്ദേശം;

		സന്ദേശം = alloc(len);
		എങ്കിൽ (സന്ദേശം != ശൂന്യ)
		    vim_snprintf((അക്ഷരം *)സന്ദേശം, len, "%s: \"%s\"", err, cmd);
		*result = സന്ദേശം;
	    }
	    അല്ലെങ്കിൽ
		*result = ret;
	}
	അല്ലെങ്കിൽ
	    vim_free(ret);
	തിരികെ ret == ശൂന്യ ? -1 : 0;
    }
    server_to_നിവേശം_buf(cmd);
    തിരികെ 0;
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

    *tofree = ശൂന്യ;
    എങ്കിൽ (client_enc == ശൂന്യ || p_enc == ശൂന്യ)
	തിരികെ res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    എങ്കിൽ (convert_setup(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, data, ശൂന്യ);
	എങ്കിൽ (res == ശൂന്യ)
	    res = data;
	അല്ലെങ്കിൽ
	    *tofree = res;
    }
    convert_setup(&vimconv, ശൂന്യ, ശൂന്യ);
    തിരികെ res;
}
#endif

#എങ്കിൽ defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

സ്ഥിതം char_u *build_drop_cmd(പൂർണ്ണസംഖ്യ filec, അക്ഷരം **filev, പൂർണ്ണസംഖ്യ tabs, പൂർണ്ണസംഖ്യ sendReply);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    ശൂന്യം
exec_on_server(mparm_T *parmp)
{
    പൂർണ്ണസംഖ്യ made_name = FALSE;

    എങ്കിൽ (parmp->serverName_arg != ശൂന്യ && *parmp->serverName_arg == NUL)
	തിരികെ;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    എങ്കിൽ (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    എങ്കിൽ (!made_name && parmp->servername == ശൂന്യ)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    എങ്കിൽ (parmp->servername != ശൂന്യ && clientserver_method == CLIENTSERVER_METHOD_MSWIN)
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
    ശൂന്യം
prepare_server(mparm_T *parmp)
{
# എങ്കിൽ defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    എങ്കിൽ (
#  എങ്കിൽ defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != ശൂന്യ
#   endif
#   എങ്കിൽ defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   എങ്കിൽ defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_method == CLIENTSERVER_METHOD_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != ശൂന്യ && (
#  എങ്കിൽ defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   എങ്കിൽ defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_method != CLIENTSERVER_METHOD_SOCKET &&
#   endif
#   എങ്കിൽ defined(FEAT_AUTOSERVERNAME)
		    1
#   അല്ലെങ്കിൽ
		    gui.in_use
#   endif
#   ifdef UNIX
		 && getuid() != വേര്_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != ശൂന്യ))
    {

#  ifdef FEAT_SOCKETSERVER
	എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	    socketserver_start(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11)
	{
	    (ശൂന്യം)serverRegisterName(X_DISPLAY, parmp->servername);
	    TIME_സന്ദേശം("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    അല്ലെങ്കിൽ
    {
#  ifdef FEAT_X11
	എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    serverDelayedStartName = parmp->servername;
	അല്ലെങ്കിൽ
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    എങ്കിൽ (parmp->serverStr != ശൂന്യ)
    {
	char_u *p;

	server_to_നിവേശം_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    സ്ഥിതം ശൂന്യം
cmdsrv_main(
    പൂർണ്ണസംഖ്യ		*argc,
    അക്ഷരം	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*sname;
    പൂർണ്ണസംഖ്യ		ret = -1;
    പൂർണ്ണസംഖ്യ		didone = FALSE;
    പൂർണ്ണസംഖ്യ		exiterr = 0;
    അക്ഷരം	**newArgV = argv + 1;
    പൂർണ്ണസംഖ്യ		newArgC = 1,
		Argc = *argc;
    പൂർണ്ണസംഖ്യ		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    പൂർണ്ണസംഖ്യ		silent = FALSE;
    പൂർണ്ണസംഖ്യ		tabs = FALSE;
# ifdef MSWIN
    HWND	srv;
# elif defined(FEAT_X11)
    Window	srv;

    setup_term_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = ശൂന്യ;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    എങ്കിൽ (sname == ശൂന്യ)
	തിരികെ;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    വേണ്ടി (i = 1; i < Argc; i++)
    {
	res = ശൂന്യ;
	എങ്കിൽ (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    വേണ്ടി (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    നിർത്തുക;
	}

	എങ്കിൽ (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    അക്ഷരം	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    ആകുമ്പോൾ (*p != NUL)
	    {
		എങ്കിൽ (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		അല്ലെങ്കിൽ
		{
		    argtype = ARGTYPE_OTHER;
		    നിർത്തുക;
		}
	    }
	}
	അല്ലെങ്കിൽ
	    argtype = ARGTYPE_OTHER;

	എങ്കിൽ (argtype != ARGTYPE_OTHER)
	{
	    എങ്കിൽ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    എങ്കിൽ (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    അല്ലെങ്കിൽ
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		എങ്കിൽ (*serverStr == ശൂന്യ)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    നിർത്തുക;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		ret = socketserver_send(sname, *serverStr, ശൂന്യ, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		എങ്കിൽ (xterm_dpy == ശൂന്യ)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		അല്ലെങ്കിൽ
		    ret = serverSendToVim(xterm_dpy, sname, *serverStr,
			    ശൂന്യ, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		ret = serverSendToVim(sname, *serverStr, ശൂന്യ, &srv, 0, 0,
			silent);
# endif
	    എങ്കിൽ (ret < 0)
	    {
		എങ്കിൽ (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		നിർത്തുക;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    എങ്കിൽ (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		SetForegroundWindow(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    എങ്കിൽ (argtype == ARGTYPE_EDIT_WAIT)
	    {
		പൂർണ്ണസംഖ്യ	അക്കം_Files = *argc - i - 1;
		char_u  *done = alloc(അക്കം_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		പൂർണ്ണസംഖ്യ	എണ്ണം = 0;
		ബാഹ്യ HWND message_window;
# endif

		എങ്കിൽ (അക്കം_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --അക്കം_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cbSize = വലുപ്പം(ni);
		ni.hWnd = message_window;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), എണ്ണം, അക്കം_Files);
		Shell_NotifyIcon(NIM_ADD, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(done, 0, അക്കം_Files);
		ആകുമ്പോൾ (memchr(done, 0, അക്കം_Files) != ശൂന്യ)
		{
		    char_u  *p = ശൂന്യ;
		    പൂർണ്ണസംഖ്യ	    j;
# ifdef MSWIN
		    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
			p = serverGetReply(srv, ശൂന്യ, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET
			    && (ch == ശൂന്യ
				|| socketserver_read_reply(sname, &p, -1, true)
				== FAIL))
			    നിർത്തുക;
# endif
# ifdef FEAT_X11
		    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11
			    && serverReadReply(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    നിർത്തുക;
# endif
		    എങ്കിൽ (p == ശൂന്യ)
			നിർത്തുക;
		    j = atoi((അക്ഷരം *)p);
		    vim_free(p);
		    എങ്കിൽ (j >= 0 && j < അക്കം_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++എണ്ണം;
			sprintf(ni.szTip, _("%d of %d edited"),
							     എണ്ണം, അക്കം_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			done[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		എങ്കിൽ (ch != ശൂന്യ)
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
	അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    പൂർണ്ണസംഖ്യ status = OK;

	    എങ്കിൽ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_MSWIN
		    && serverSendToVim(sname, (char_u *)argv[i + 1],
						  &res, ശൂന്യ, 1, 0, FALSE) < 0)
		status = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, ശൂന്യ) < 0)
		status = FAIL;
# endif
# ifdef FEAT_X11
	    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		എങ്കിൽ (xterm_dpy == ശൂന്യ)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		അല്ലെങ്കിൽ എങ്കിൽ (serverSendToVim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    ശൂന്യ, 1, 0, 1, FALSE) < 0)
		    status = FAIL;
	    }
# endif
	    എങ്കിൽ (status == FAIL)
	    {
		എങ്കിൽ (res != ശൂന്യ && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((അക്ഷരം *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = ശൂന്യ;
	    garray_T	ga;

# ifdef MSWIN
	    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		list = serverGetVimNames();
# endif
# ifdef FEAT_SOCKETSERVER
	    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		    xterm_dpy != ശൂന്യ)
		list = serverGetVimNames(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    എങ്കിൽ (list != ശൂന്യ)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_data;

	    എങ്കിൽ (did_emsg)
		mch_errmsg("\n");
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    തുടരുക;
	}
	അല്ലെങ്കിൽ
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    തുടരുക;
	}
	didone = TRUE;
	എങ്കിൽ (res != ശൂന്യ && *res != NUL)
	{
	    mch_സന്ദേശം((അക്ഷരം *)res);
	    എങ്കിൽ (res[STRLEN(res) - 1] != '\n')
		mch_സന്ദേശം("\n");
	}
	vim_free(res);
    }

    എങ്കിൽ (didone)
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
    സ്ഥിതം char_u *
build_drop_cmd(
    പൂർണ്ണസംഖ്യ		filec,
    അക്ഷരം	**filev,
    പൂർണ്ണസംഖ്യ		tabs,		// Use ":tab drop" instead of ":drop".
    പൂർണ്ണസംഖ്യ		sendReply)
{
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*inicmd = ശൂന്യ;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    സ്ഥിരം string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    എങ്കിൽ (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    എങ്കിൽ (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    എങ്കിൽ (cwd == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	തിരികെ ശൂന്യ;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_IN_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# അല്ലെങ്കിൽ
	    PATH_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    എങ്കിൽ (cdp.string == ശൂന്യ)
	തിരികെ ശൂന്യ;
    cdp.length = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.length);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].length);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    എങ്കിൽ (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    വേണ്ടി (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		PATH_ESC_CHARS
# അല്ലെങ്കിൽ
		(char_u *)" \t%#"
# endif
		);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_data);
	    തിരികെ ശൂന്യ;
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
# അല്ലെങ്കിൽ
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.length);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].length);

    എങ്കിൽ (sendReply)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    എങ്കിൽ (inicmd != ശൂന്യ)
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
    തിരികെ ga.ga_data;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    സ്ഥിതം char_u *
serverMakeName(char_u *arg, അക്ഷരം *cmd)
{
    char_u *p;

    എങ്കിൽ (arg != ശൂന്യ && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	{
	    എങ്കിൽ (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    അല്ലെങ്കിൽ
		p = vim_strsave_up(arg);
	}
	അല്ലെങ്കിൽ
	    p = vim_strsave_up(arg);
# അല്ലെങ്കിൽ
	p = vim_strsave_up(arg);
# endif
    }
    അല്ലെങ്കിൽ
    {
	p = vim_strsave_up(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	എങ്കിൽ (p != ശൂന്യ && vim_strchr(p, '.') != ശൂന്യ)
	    *vim_strchr(p, '.') = NUL;
    }
    തിരികെ p;
}
#endif // FEAT_CLIENTSERVER

#എങ്കിൽ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    സ്ഥിതം ശൂന്യം
make_connection(ശൂന്യം)
{
    എങ്കിൽ (X_DISPLAY == ശൂന്യ
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

    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_connection(ശൂന്യം)
{
    എങ്കിൽ (clientserver_method != CLIENTSERVER_METHOD_X11)
	തിരികെ OK;
    make_connection();
    എങ്കിൽ (X_DISPLAY == ശൂന്യ)
    {
	emsg(_(e_no_connection_to_x_server));
	തിരികെ FAIL;
    }
    തിരികെ OK;
}
#endif

#ifdef FEAT_CLIENTSERVER
    സ്ഥിതം ശൂന്യം
remote_common(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ വ്യഞ്ജകം)
{
    char_u	*server_name;
    char_u	*താക്കോലുകൾ;
    char_u	*r = ശൂന്യ;
    char_u	buf[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		timeout = 0;
# ifdef MSWIN
    HWND	w;
# അല്ലെങ്കിൽ
#  ifdef FEAT_X11
    Window	w;
#  endif
# endif
    പൂർണ്ണസംഖ്യ		ret = OK;

    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

# ifdef FEAT_X11
    എങ്കിൽ (check_connection() == FAIL)
	തിരികെ;
# endif
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_സംഖ്യ(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (server_name == ശൂന്യ)
	തിരികെ;		// type error; errmsg already given
    താക്കോലുകൾ = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendToVim(server_name, താക്കോലുകൾ, &r, &w, വ്യഞ്ജകം,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET
	&& socketserver_send(server_name, താക്കോലുകൾ, &r, വ്യഞ്ജകം,
	    timeout * 1000, true, ശൂന്യ) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11)
	എങ്കിൽ (serverSendToVim(X_DISPLAY, server_name, താക്കോലുകൾ, &r, &w, വ്യഞ്ജകം, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    എങ്കിൽ (ret == FAIL)
    {
	എങ്കിൽ (r != ശൂന്യ)
	{
	    emsg((അക്ഷരം *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	അല്ലെങ്കിൽ
	    semsg(_(e_unable_to_send_to_str), server_name);
	തിരികെ;
    }

    rettv->vval.v_string = r;

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# എങ്കിൽ defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = ശൂന്യ;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	എങ്കിൽ (idvar != ശൂന്യ && *idvar != NUL)
	{
# ifdef MSWIN
	    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	    {
		sprintf((അക്ഷരം *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		sprintf((അക്ഷരം *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		str = server_name;
# endif
	    എങ്കിൽ (str == ശൂന്യ)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    set_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#endif

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    ശൂന്യം
f_remote_വ്യഞ്ജകം(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

# ifdef FEAT_CLIENTSERVER
    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 3) == FAIL)))
	തിരികെ;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    ശൂന്യം
f_remote_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	എങ്കിൽ (server_name != ശൂന്യ)
	    serverForeground(server_name);
    }
#  അല്ലെങ്കിൽ
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    ശൂന്യം
f_remote_peek(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = ശൂന്യ;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_സംഖ്യ = -1;
    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    serverid = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (serverid == ശൂന്യ)
	തിരികെ;		// type error; errmsg already given
#  ifdef MSWIN
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	sscanf((സ്ഥിരം അക്ഷരം *)serverid, SCANF_HEX_LONG_U, &n);
	എങ്കിൽ (n == 0)
	    rettv->vval.v_സംഖ്യ = -1;
	അല്ലെങ്കിൽ
	{
	    s = serverGetReply((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_സംഖ്യ = (s != ശൂന്യ);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	rettv->vval.v_സംഖ്യ = socketserver_peek_reply(serverid, &s);
#  endif
#  ifdef FEAT_X11
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	എങ്കിൽ (check_connection() == FAIL)
	    തിരികെ;

	rettv->vval.v_സംഖ്യ = serverPeekReply(X_DISPLAY,
		serverStrToWin(serverid), &s);
    }
#  endif

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_സംഖ്യ > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	എങ്കിൽ (retvar != ശൂന്യ)
	    set_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# അല്ലെങ്കിൽ
    rettv->vval.v_സംഖ്യ = -1;
# endif
}

    ശൂന്യം
f_remote_read(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*r = ശൂന്യ;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    serverid = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (serverid != ശൂന്യ && !check_restricted() && !check_secure())
    {
	പൂർണ്ണസംഖ്യ timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_സംഖ്യ(&argvars[1]);

#  ifdef MSWIN
	എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	{
	    sscanf((അക്ഷരം *)serverid, SCANF_HEX_LONG_U, &n);
	    എങ്കിൽ (n != 0)
		r = serverGetReply((HWND)n, FALSE, TRUE, TRUE, timeout);
	    എങ്കിൽ (r == ശൂന്യ)
		emsg(_(e_unable_to_read_server_reply));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
		socketserver_read_reply(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_to_read_server_reply));
#  endif
#  ifdef FEAT_X11
	എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
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
    ശൂന്യം
f_remote_send(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

# ifdef FEAT_CLIENTSERVER
    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	തിരികെ;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    ശൂന്യം
f_remote_startserver(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    എങ്കിൽ (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (serverName != ശൂന്യ)
    {
	emsg(_(e_already_started_server));
	തിരികെ;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (server == ശൂന്യ)
	തിരികെ;
#  ifdef MSWIN
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	serverSetName(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	socketserver_start(server, true);
#  endif
#  ifdef FEAT_X11
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    check_connection() == OK)
	serverRegisterName(X_DISPLAY, server);
#  endif

# അല്ലെങ്കിൽ
    emsg(_(e_clientserver_feature_not_available));
# endif
}

    ശൂന്യം
f_server2client(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*reply;
    പൂർണ്ണസംഖ്യ		ret = OK;

    rettv->vval.v_സംഖ്യ = -1;
    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    server = tv_get_string_chk(&argvars[0]);
    reply = tv_get_string_buf_chk(&argvars[1], buf);
    എങ്കിൽ (server == ശൂന്യ || reply == ശൂന്യ)
	തിരികെ;

#  ifdef FEAT_SOCKETSERVER
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
	    socketserver_send_reply(server, reply) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif

    എങ്കിൽ (ret == FAIL)
    {
	emsg(_(e_unable_to_send_to_client));
	തിരികെ;
    }
    rettv->vval.v_സംഖ്യ = 0;
# അല്ലെങ്കിൽ
    rettv->vval.v_സംഖ്യ = -1;
# endif
}

    ശൂന്യം
f_serverlist(typval_T *argvars UNUSED, typval_T *rettv)
{
    list_T  *list = ശൂന്യ;
    bool    use_list = false;

    എങ്കിൽ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	list = serverGetVimNames();
#  endif
#  ifdef FEAT_SOCKETSERVER
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	make_connection();
	എങ്കിൽ (X_DISPLAY != ശൂന്യ)
	    list = serverGetVimNames(X_DISPLAY);
    }
#  endif
# endif
    എങ്കിൽ (use_list)
    {
	എങ്കിൽ (list == ശൂന്യ)
	{
	    (ശൂന്യം)rettv_list_alloc(rettv);
	    തിരികെ;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    അല്ലെങ്കിൽ
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	എങ്കിൽ (list != ശൂന്യ)
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
    ശൂന്യം
check_clientserver_method_env(ശൂന്യം)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    എങ്കിൽ (env == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (STRICMP(env, "socket") == 0)
	clientserver_method = CLIENTSERVER_METHOD_SOCKET;
# ifdef FEAT_X11
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(env, "x11") == 0)
	clientserver_method = CLIENTSERVER_METHOD_X11;
# endif
# ifdef MSWIN
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(env, "mswin") == 0)
	clientserver_method = CLIENTSERVER_METHOD_MSWIN;
# endif
}
#endif
