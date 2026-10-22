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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_CLIENTSERVER)

நிலையான வெறுமை cmdsrv_main(முழு *argc, எழுத்து **argv, char_u *serverName_arg, char_u **serverStr);
நிலையான char_u *serverMakeName(char_u *arg, எழுத்து *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    வெறுமை
server_to_உள்ளீடு_buf(char_u *str)
{
    char_u      *ptr = வெற்று;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, வெற்று);
    p_cpo = cpo_save;

    எனில் (ptr != வெற்று && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	எனில் (typebuf.tb_maplen < typebuf.tb_len)
	    del_typebuf(typebuf.tb_len - typebuf.tb_maplen, typebuf.tb_maplen);
	(வெறுமை)ins_typebuf(str, REMAP_NONE, typebuf.tb_len, TRUE, FALSE);

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
eval_client_கோவை_to_string(char_u *கோவை)
{
    char_u	*res;
    முழு		save_dbl = debug_break_level;
    முழு		save_ro = redir_off;
    funccal_entry_T funccal_entry;
    முழு		did_save_funccal = FALSE;

# எனில் defined(FEAT_EVAL)
    ch_log(வெற்று, "eval_client_expr_to_string(\"%s\")", கோவை);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    எனில் (!debug_mode)
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

    res = eval_to_string(கோவை, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    எனில் (emsg_silent < 0)
	emsg_silent = 0;
    எனில் (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    out_flush_cursor(FALSE, FALSE);

    திரும்பு res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    முழு
sendToLocalVim(char_u *cmd, முழு as_கோவை, char_u **result)
{
    எனில் (as_கோவை)
    {
	char_u *ret;

	ret = eval_client_கோவை_to_string(cmd);
	எனில் (result != வெற்று)
	{
	    எனில் (ret == வெற்று)
	    {
		எழுத்து	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*செய்தி;

		செய்தி = alloc(len);
		எனில் (செய்தி != வெற்று)
		    vim_snprintf((எழுத்து *)செய்தி, len, "%s: \"%s\"", err, cmd);
		*result = செய்தி;
	    }
	    இல்லையெனில்
		*result = ret;
	}
	இல்லையெனில்
	    vim_free(ret);
	திரும்பு ret == வெற்று ? -1 : 0;
    }
    server_to_உள்ளீடு_buf(cmd);
    திரும்பு 0;
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

    *tofree = வெற்று;
    எனில் (client_enc == வெற்று || p_enc == வெற்று)
	திரும்பு res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    எனில் (convert_setup(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, data, வெற்று);
	எனில் (res == வெற்று)
	    res = data;
	இல்லையெனில்
	    *tofree = res;
    }
    convert_setup(&vimconv, வெற்று, வெற்று);
    திரும்பு res;
}
#endif

#எனில் defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

நிலையான char_u *build_drop_cmd(முழு filec, எழுத்து **filev, முழு tabs, முழு sendReply);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    வெறுமை
exec_on_server(mparm_T *parmp)
{
    முழு made_name = FALSE;

    எனில் (parmp->serverName_arg != வெற்று && *parmp->serverName_arg == NUL)
	திரும்பு;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    எனில் (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    எனில் (!made_name && parmp->servername == வெற்று)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    எனில் (parmp->servername != வெற்று && clientserver_method == CLIENTSERVER_METHOD_MSWIN)
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
    வெறுமை
prepare_server(mparm_T *parmp)
{
# எனில் defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    எனில் (
#  எனில் defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != வெற்று
#   endif
#   எனில் defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   எனில் defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_method == CLIENTSERVER_METHOD_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != வெற்று && (
#  எனில் defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   எனில் defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_method != CLIENTSERVER_METHOD_SOCKET &&
#   endif
#   எனில் defined(FEAT_AUTOSERVERNAME)
		    1
#   இல்லையெனில்
		    gui.in_use
#   endif
#   ifdef UNIX
		 && getuid() != வேர்_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != வெற்று))
    {

#  ifdef FEAT_SOCKETSERVER
	எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	    socketserver_start(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	எனில் (clientserver_method == CLIENTSERVER_METHOD_X11)
	{
	    (வெறுமை)serverRegisterName(X_DISPLAY, parmp->servername);
	    TIME_செய்தி("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    இல்லையெனில்
    {
#  ifdef FEAT_X11
	எனில் (clientserver_method == CLIENTSERVER_METHOD_X11)
	    serverDelayedStartName = parmp->servername;
	இல்லையெனில்
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    எனில் (parmp->serverStr != வெற்று)
    {
	char_u *p;

	server_to_உள்ளீடு_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    நிலையான வெறுமை
cmdsrv_main(
    முழு		*argc,
    எழுத்து	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    முழு		i;
    char_u	*sname;
    முழு		ret = -1;
    முழு		didone = FALSE;
    முழு		exiterr = 0;
    எழுத்து	**newArgV = argv + 1;
    முழு		newArgC = 1,
		Argc = *argc;
    முழு		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    முழு		silent = FALSE;
    முழு		tabs = FALSE;
# ifdef MSWIN
    HWND	srv;
# elif defined(FEAT_X11)
    Window	srv;

    setup_term_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = வெற்று;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    எனில் (sname == வெற்று)
	திரும்பு;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    க்கு (i = 1; i < Argc; i++)
    {
	res = வெற்று;
	எனில் (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    க்கு (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    நிறுத்து;
	}

	எனில் (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	இல்லையெனில் எனில் (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    எழுத்து	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    போது (*p != NUL)
	    {
		எனில் (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		இல்லையெனில் எனில் (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		இல்லையெனில் எனில் (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		இல்லையெனில்
		{
		    argtype = ARGTYPE_OTHER;
		    நிறுத்து;
		}
	    }
	}
	இல்லையெனில்
	    argtype = ARGTYPE_OTHER;

	எனில் (argtype != ARGTYPE_OTHER)
	{
	    எனில் (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    எனில் (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    இல்லையெனில்
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		எனில் (*serverStr == வெற்று)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    நிறுத்து;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		ret = socketserver_send(sname, *serverStr, வெற்று, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    எனில் (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		எனில் (xterm_dpy == வெற்று)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		இல்லையெனில்
		    ret = serverSendToVim(xterm_dpy, sname, *serverStr,
			    வெற்று, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    எனில் (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		ret = serverSendToVim(sname, *serverStr, வெற்று, &srv, 0, 0,
			silent);
# endif
	    எனில் (ret < 0)
	    {
		எனில் (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		இல்லையெனில் எனில் (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		நிறுத்து;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    எனில் (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		SetForegroundWindow(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    எனில் (argtype == ARGTYPE_EDIT_WAIT)
	    {
		முழு	இலக்கம்_Files = *argc - i - 1;
		char_u  *done = alloc(இலக்கம்_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		முழு	மொத்தம் = 0;
		வெளி HWND message_window;
# endif

		எனில் (இலக்கம்_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --இலக்கம்_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cbSize = அளவு(ni);
		ni.hWnd = message_window;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), மொத்தம், இலக்கம்_Files);
		Shell_NotifyIcon(NIM_ADD, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(done, 0, இலக்கம்_Files);
		போது (memchr(done, 0, இலக்கம்_Files) != வெற்று)
		{
		    char_u  *p = வெற்று;
		    முழு	    j;
# ifdef MSWIN
		    எனில் (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
			p = serverGetReply(srv, வெற்று, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET
			    && (ch == வெற்று
				|| socketserver_read_reply(sname, &p, -1, true)
				== FAIL))
			    நிறுத்து;
# endif
# ifdef FEAT_X11
		    எனில் (clientserver_method == CLIENTSERVER_METHOD_X11
			    && serverReadReply(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    நிறுத்து;
# endif
		    எனில் (p == வெற்று)
			நிறுத்து;
		    j = atoi((எழுத்து *)p);
		    vim_free(p);
		    எனில் (j >= 0 && j < இலக்கம்_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++மொத்தம்;
			sprintf(ni.szTip, _("%d of %d edited"),
							     மொத்தம், இலக்கம்_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			done[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		எனில் (ch != வெற்று)
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
	இல்லையெனில் எனில் (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    முழு status = OK;

	    எனில் (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    எனில் (clientserver_method == CLIENTSERVER_METHOD_MSWIN
		    && serverSendToVim(sname, (char_u *)argv[i + 1],
						  &res, வெற்று, 1, 0, FALSE) < 0)
		status = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, வெற்று) < 0)
		status = FAIL;
# endif
# ifdef FEAT_X11
	    எனில் (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		எனில் (xterm_dpy == வெற்று)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		இல்லையெனில் எனில் (serverSendToVim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    வெற்று, 1, 0, 1, FALSE) < 0)
		    status = FAIL;
	    }
# endif
	    எனில் (status == FAIL)
	    {
		எனில் (res != வெற்று && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((எழுத்து *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	இல்லையெனில் எனில் (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = வெற்று;
	    garray_T	ga;

# ifdef MSWIN
	    எனில் (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		list = serverGetVimNames();
# endif
# ifdef FEAT_SOCKETSERVER
	    எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    எனில் (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		    xterm_dpy != வெற்று)
		list = serverGetVimNames(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    எனில் (list != வெற்று)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_data;

	    எனில் (did_emsg)
		mch_errmsg("\n");
	}
	இல்லையெனில் எனில் (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    தொடர்;
	}
	இல்லையெனில்
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    தொடர்;
	}
	didone = TRUE;
	எனில் (res != வெற்று && *res != NUL)
	{
	    mch_செய்தி((எழுத்து *)res);
	    எனில் (res[STRLEN(res) - 1] != '\n')
		mch_செய்தி("\n");
	}
	vim_free(res);
    }

    எனில் (didone)
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
    நிலையான char_u *
build_drop_cmd(
    முழு		filec,
    எழுத்து	**filev,
    முழு		tabs,		// Use ":tab drop" instead of ":drop".
    முழு		sendReply)
{
    garray_T	ga;
    முழு		i;
    char_u	*inicmd = வெற்று;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    மாறிலி string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    எனில் (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    எனில் (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    எனில் (cwd == வெற்று)
	திரும்பு வெற்று;
    எனில் (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	திரும்பு வெற்று;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_IN_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# இல்லையெனில்
	    PATH_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    எனில் (cdp.string == வெற்று)
	திரும்பு வெற்று;
    cdp.length = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.length);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].length);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    எனில் (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    க்கு (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		PATH_ESC_CHARS
# இல்லையெனில்
		(char_u *)" \t%#"
# endif
		);
	எனில் (p == வெற்று)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_data);
	    திரும்பு வெற்று;
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
# இல்லையெனில்
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.length);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].length);

    எனில் (sendReply)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    எனில் (inicmd != வெற்று)
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
    திரும்பு ga.ga_data;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    நிலையான char_u *
serverMakeName(char_u *arg, எழுத்து *cmd)
{
    char_u *p;

    எனில் (arg != வெற்று && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	{
	    எனில் (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    இல்லையெனில்
		p = vim_strsave_up(arg);
	}
	இல்லையெனில்
	    p = vim_strsave_up(arg);
# இல்லையெனில்
	p = vim_strsave_up(arg);
# endif
    }
    இல்லையெனில்
    {
	p = vim_strsave_up(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	எனில் (p != வெற்று && vim_strchr(p, '.') != வெற்று)
	    *vim_strchr(p, '.') = NUL;
    }
    திரும்பு p;
}
#endif // FEAT_CLIENTSERVER

#எனில் defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    நிலையான வெறுமை
make_connection(வெறுமை)
{
    எனில் (X_DISPLAY == வெற்று
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

    நிலையான முழு
check_connection(வெறுமை)
{
    எனில் (clientserver_method != CLIENTSERVER_METHOD_X11)
	திரும்பு OK;
    make_connection();
    எனில் (X_DISPLAY == வெற்று)
    {
	emsg(_(e_no_connection_to_x_server));
	திரும்பு FAIL;
    }
    திரும்பு OK;
}
#endif

#ifdef FEAT_CLIENTSERVER
    நிலையான வெறுமை
remote_common(typval_T *argvars, typval_T *rettv, முழு கோவை)
{
    char_u	*server_name;
    char_u	*சாவிகள்;
    char_u	*r = வெற்று;
    char_u	buf[NUMBUFLEN];
    முழு		timeout = 0;
# ifdef MSWIN
    HWND	w;
# இல்லையெனில்
#  ifdef FEAT_X11
    Window	w;
#  endif
# endif
    முழு		ret = OK;

    எனில் (check_restricted() || check_secure())
	திரும்பு;

# ifdef FEAT_X11
    எனில் (check_connection() == FAIL)
	திரும்பு;
# endif
    எனில் (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_எண்(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    எனில் (server_name == வெற்று)
	திரும்பு;		// type error; errmsg already given
    சாவிகள் = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    எனில் (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendToVim(server_name, சாவிகள், &r, &w, கோவை,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET
	&& socketserver_send(server_name, சாவிகள், &r, கோவை,
	    timeout * 1000, true, வெற்று) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    எனில் (clientserver_method == CLIENTSERVER_METHOD_X11)
	எனில் (serverSendToVim(X_DISPLAY, server_name, சாவிகள், &r, &w, கோவை, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    எனில் (ret == FAIL)
    {
	எனில் (r != வெற்று)
	{
	    emsg((எழுத்து *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	இல்லையெனில்
	    semsg(_(e_unable_to_send_to_str), server_name);
	திரும்பு;
    }

    rettv->vval.v_string = r;

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# எனில் defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = வெற்று;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	எனில் (idvar != வெற்று && *idvar != NUL)
	{
# ifdef MSWIN
	    எனில் (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	    {
		sprintf((எழுத்து *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    எனில் (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		sprintf((எழுத்து *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		str = server_name;
# endif
	    எனில் (str == வெற்று)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    set_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#endif

#எனில் defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    வெறுமை
f_remote_கோவை(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

# ifdef FEAT_CLIENTSERVER
    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 3) == FAIL)))
	திரும்பு;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    வெறுமை
f_remote_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	எனில் (server_name != வெற்று)
	    serverForeground(server_name);
    }
#  இல்லையெனில்
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    வெறுமை
f_remote_peek(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = வெற்று;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_எண் = -1;
    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    serverid = tv_get_string_chk(&argvars[0]);
    எனில் (serverid == வெற்று)
	திரும்பு;		// type error; errmsg already given
#  ifdef MSWIN
    எனில் (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	sscanf((மாறிலி எழுத்து *)serverid, SCANF_HEX_LONG_U, &n);
	எனில் (n == 0)
	    rettv->vval.v_எண் = -1;
	இல்லையெனில்
	{
	    s = serverGetReply((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_எண் = (s != வெற்று);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	rettv->vval.v_எண் = socketserver_peek_reply(serverid, &s);
#  endif
#  ifdef FEAT_X11
    எனில் (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	எனில் (check_connection() == FAIL)
	    திரும்பு;

	rettv->vval.v_எண் = serverPeekReply(X_DISPLAY,
		serverStrToWin(serverid), &s);
    }
#  endif

    எனில் (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_எண் > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	எனில் (retvar != வெற்று)
	    set_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# இல்லையெனில்
    rettv->vval.v_எண் = -1;
# endif
}

    வெறுமை
f_remote_read(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*r = வெற்று;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    serverid = tv_get_string_chk(&argvars[0]);
    எனில் (serverid != வெற்று && !check_restricted() && !check_secure())
    {
	முழு timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	எனில் (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_எண்(&argvars[1]);

#  ifdef MSWIN
	எனில் (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	{
	    sscanf((எழுத்து *)serverid, SCANF_HEX_LONG_U, &n);
	    எனில் (n != 0)
		r = serverGetReply((HWND)n, FALSE, TRUE, TRUE, timeout);
	    எனில் (r == வெற்று)
		emsg(_(e_unable_to_read_server_reply));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
		socketserver_read_reply(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_to_read_server_reply));
#  endif
#  ifdef FEAT_X11
	எனில் (clientserver_method == CLIENTSERVER_METHOD_X11 &&
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
    வெறுமை
f_remote_send(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

# ifdef FEAT_CLIENTSERVER
    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	திரும்பு;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    வெறுமை
f_remote_startserver(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    எனில் (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (serverName != வெற்று)
    {
	emsg(_(e_already_started_server));
	திரும்பு;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    எனில் (server == வெற்று)
	திரும்பு;
#  ifdef MSWIN
    எனில் (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	serverSetName(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	socketserver_start(server, true);
#  endif
#  ifdef FEAT_X11
    எனில் (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    check_connection() == OK)
	serverRegisterName(X_DISPLAY, server);
#  endif

# இல்லையெனில்
    emsg(_(e_clientserver_feature_not_available));
# endif
}

    வெறுமை
f_server2client(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*reply;
    முழு		ret = OK;

    rettv->vval.v_எண் = -1;
    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    server = tv_get_string_chk(&argvars[0]);
    reply = tv_get_string_buf_chk(&argvars[1], buf);
    எனில் (server == வெற்று || reply == வெற்று)
	திரும்பு;

#  ifdef FEAT_SOCKETSERVER
    எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
	    socketserver_send_reply(server, reply) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    எனில் (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    எனில் (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif

    எனில் (ret == FAIL)
    {
	emsg(_(e_unable_to_send_to_client));
	திரும்பு;
    }
    rettv->vval.v_எண் = 0;
# இல்லையெனில்
    rettv->vval.v_எண் = -1;
# endif
}

    வெறுமை
f_serverlist(typval_T *argvars UNUSED, typval_T *rettv)
{
    list_T  *list = வெற்று;
    bool    use_list = false;

    எனில் (check_for_opt_dict_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    எனில் (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	list = serverGetVimNames();
#  endif
#  ifdef FEAT_SOCKETSERVER
    எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    எனில் (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	make_connection();
	எனில் (X_DISPLAY != வெற்று)
	    list = serverGetVimNames(X_DISPLAY);
    }
#  endif
# endif
    எனில் (use_list)
    {
	எனில் (list == வெற்று)
	{
	    (வெறுமை)rettv_list_alloc(rettv);
	    திரும்பு;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    இல்லையெனில்
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	எனில் (list != வெற்று)
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
    வெறுமை
check_clientserver_method_env(வெறுமை)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    எனில் (env == வெற்று)
	திரும்பு;

    எனில் (STRICMP(env, "socket") == 0)
	clientserver_method = CLIENTSERVER_METHOD_SOCKET;
# ifdef FEAT_X11
    இல்லையெனில் எனில் (STRICMP(env, "x11") == 0)
	clientserver_method = CLIENTSERVER_METHOD_X11;
# endif
# ifdef MSWIN
    இல்லையெனில் எனில் (STRICMP(env, "mswin") == 0)
	clientserver_method = CLIENTSERVER_METHOD_MSWIN;
# endif
}
#endif
