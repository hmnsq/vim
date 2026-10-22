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

#यदि defined(FEAT_CLIENTSERVER)

स्थिर शून्य cmdsrv_main(पूर्णाङ्क *argc, वर्ण **argv, char_u *serverName_arg, char_u **serverStr);
स्थिर char_u *serverMakeName(char_u *arg, वर्ण *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    शून्य
server_to_आगत_buf(char_u *str)
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

    यदि (ptr != रिक्त && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	यदि (typebuf.tb_maplen < typebuf.tb_len)
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
eval_client_अभिव्यक्ति_to_string(char_u *अभिव्यक्ति)
{
    char_u	*res;
    पूर्णाङ्क		save_dbl = debug_break_level;
    पूर्णाङ्क		save_ro = redir_off;
    funccal_entry_T funccal_entry;
    पूर्णाङ्क		did_save_funccal = FALSE;

# यदि defined(FEAT_EVAL)
    ch_log(रिक्त, "eval_client_expr_to_string(\"%s\")", अभिव्यक्ति);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    यदि (!debug_mode)
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

    res = eval_to_string(अभिव्यक्ति, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    यदि (emsg_silent < 0)
	emsg_silent = 0;
    यदि (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    out_flush_cursor(FALSE, FALSE);

    फर्काऊ res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    पूर्णाङ्क
sendToLocalVim(char_u *cmd, पूर्णाङ्क as_अभिव्यक्ति, char_u **result)
{
    यदि (as_अभिव्यक्ति)
    {
	char_u *ret;

	ret = eval_client_अभिव्यक्ति_to_string(cmd);
	यदि (result != रिक्त)
	{
	    यदि (ret == रिक्त)
	    {
		वर्ण	*err = _(e_invalid_expression_received);
		माप_प्रकार	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*सन्देश;

		सन्देश = alloc(len);
		यदि (सन्देश != रिक्त)
		    vim_snprintf((वर्ण *)सन्देश, len, "%s: \"%s\"", err, cmd);
		*result = सन्देश;
	    }
	    अन्यथा
		*result = ret;
	}
	अन्यथा
	    vim_free(ret);
	फर्काऊ ret == रिक्त ? -1 : 0;
    }
    server_to_आगत_buf(cmd);
    फर्काऊ 0;
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
    यदि (client_enc == रिक्त || p_enc == रिक्त)
	फर्काऊ res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    यदि (convert_setup(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, data, रिक्त);
	यदि (res == रिक्त)
	    res = data;
	अन्यथा
	    *tofree = res;
    }
    convert_setup(&vimconv, रिक्त, रिक्त);
    फर्काऊ res;
}
#endif

#यदि defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

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

    यदि (parmp->serverName_arg != रिक्त && *parmp->serverName_arg == NUL)
	फर्काऊ;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    यदि (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    यदि (!made_name && parmp->servername == रिक्त)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    यदि (parmp->servername != रिक्त && clientserver_method == CLIENTSERVER_METHOD_MSWIN)
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
# यदि defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    यदि (
#  यदि defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != रिक्त
#   endif
#   यदि defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   यदि defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_method == CLIENTSERVER_METHOD_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != रिक्त && (
#  यदि defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   यदि defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_method != CLIENTSERVER_METHOD_SOCKET &&
#   endif
#   यदि defined(FEAT_AUTOSERVERNAME)
		    1
#   अन्यथा
		    gui.in_use
#   endif
#   ifdef UNIX
		 && getuid() != जरा_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != रिक्त))
    {

#  ifdef FEAT_SOCKETSERVER
	यदि (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	    socketserver_start(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	यदि (clientserver_method == CLIENTSERVER_METHOD_X11)
	{
	    (शून्य)serverRegisterName(X_DISPLAY, parmp->servername);
	    TIME_सन्देश("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    अन्यथा
    {
#  ifdef FEAT_X11
	यदि (clientserver_method == CLIENTSERVER_METHOD_X11)
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
    यदि (parmp->serverStr != रिक्त)
    {
	char_u *p;

	server_to_आगत_buf(serverConvert(parmp->serverStrEnc,
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
    यदि (sname == रिक्त)
	फर्काऊ;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    लागि (i = 1; i < Argc; i++)
    {
	res = रिक्त;
	यदि (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    लागि (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    रोक;
	}

	यदि (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	अन्यथा यदि (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    वर्ण	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    जबसम्म (*p != NUL)
	    {
		यदि (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		अन्यथा यदि (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		अन्यथा यदि (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		अन्यथा
		{
		    argtype = ARGTYPE_OTHER;
		    रोक;
		}
	    }
	}
	अन्यथा
	    argtype = ARGTYPE_OTHER;

	यदि (argtype != ARGTYPE_OTHER)
	{
	    यदि (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    यदि (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    अन्यथा
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		यदि (*serverStr == रिक्त)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    रोक;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    यदि (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		ret = socketserver_send(sname, *serverStr, रिक्त, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    यदि (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		यदि (xterm_dpy == रिक्त)
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
	    यदि (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		ret = serverSendToVim(sname, *serverStr, रिक्त, &srv, 0, 0,
			silent);
# endif
	    यदि (ret < 0)
	    {
		यदि (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		अन्यथा यदि (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		रोक;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    यदि (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		SetForegroundWindow(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    यदि (argtype == ARGTYPE_EDIT_WAIT)
	    {
		पूर्णाङ्क	अङ्क_Files = *argc - i - 1;
		char_u  *done = alloc(अङ्क_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		पूर्णाङ्क	गन्ती = 0;
		बाह्य HWND message_window;
# endif

		यदि (अङ्क_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --अङ्क_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cbSize = परिमाण(ni);
		ni.hWnd = message_window;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		प्र_लिखो(ni.szTip, _("%d of %d edited"), गन्ती, अङ्क_Files);
		Shell_NotifyIcon(NIM_ADD, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(done, 0, अङ्क_Files);
		जबसम्म (स_प्रथम(done, 0, अङ्क_Files) != रिक्त)
		{
		    char_u  *p = रिक्त;
		    पूर्णाङ्क	    j;
# ifdef MSWIN
		    यदि (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
			p = serverGetReply(srv, रिक्त, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    यदि (clientserver_method == CLIENTSERVER_METHOD_SOCKET
			    && (ch == रिक्त
				|| socketserver_read_reply(sname, &p, -1, true)
				== FAIL))
			    रोक;
# endif
# ifdef FEAT_X11
		    यदि (clientserver_method == CLIENTSERVER_METHOD_X11
			    && serverReadReply(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    रोक;
# endif
		    यदि (p == रिक्त)
			रोक;
		    j = म_से_प((वर्ण *)p);
		    vim_free(p);
		    यदि (j >= 0 && j < अङ्क_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++गन्ती;
			प्र_लिखो(ni.szTip, _("%d of %d edited"),
							     गन्ती, अङ्क_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			done[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		यदि (ch != रिक्त)
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
	अन्यथा यदि (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    पूर्णाङ्क status = OK;

	    यदि (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    यदि (clientserver_method == CLIENTSERVER_METHOD_MSWIN
		    && serverSendToVim(sname, (char_u *)argv[i + 1],
						  &res, रिक्त, 1, 0, FALSE) < 0)
		status = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    यदि (clientserver_method == CLIENTSERVER_METHOD_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, रिक्त) < 0)
		status = FAIL;
# endif
# ifdef FEAT_X11
	    यदि (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		यदि (xterm_dpy == रिक्त)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		अन्यथा यदि (serverSendToVim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    रिक्त, 1, 0, 1, FALSE) < 0)
		    status = FAIL;
	    }
# endif
	    यदि (status == FAIL)
	    {
		यदि (res != रिक्त && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((वर्ण *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	अन्यथा यदि (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = रिक्त;
	    garray_T	ga;

# ifdef MSWIN
	    यदि (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		list = serverGetVimNames();
# endif
# ifdef FEAT_SOCKETSERVER
	    यदि (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    यदि (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		    xterm_dpy != रिक्त)
		list = serverGetVimNames(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    यदि (list != रिक्त)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_data;

	    यदि (did_emsg)
		mch_errmsg("\n");
	}
	अन्यथा यदि (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    जारी;
	}
	अन्यथा
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    जारी;
	}
	didone = TRUE;
	यदि (res != रिक्त && *res != NUL)
	{
	    mch_सन्देश((वर्ण *)res);
	    यदि (res[STRLEN(res) - 1] != '\n')
		mch_सन्देश("\n");
	}
	vim_free(res);
    }

    यदि (didone)
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
    अचर string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    यदि (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    यदि (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    यदि (cwd == रिक्त)
	फर्काऊ रिक्त;
    यदि (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	फर्काऊ रिक्त;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_IN_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# अन्यथा
	    PATH_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    यदि (cdp.string == रिक्त)
	फर्काऊ रिक्त;
    cdp.length = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.length);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].length);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    यदि (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    लागि (i = 0; i < filec; i++)
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
	यदि (p == रिक्त)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_data);
	    फर्काऊ रिक्त;
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

    यदि (sendReply)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    यदि (inicmd != रिक्त)
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
    फर्काऊ ga.ga_data;
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

    यदि (arg != रिक्त && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	यदि (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	{
	    यदि (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
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
	यदि (p != रिक्त && vim_strchr(p, '.') != रिक्त)
	    *vim_strchr(p, '.') = NUL;
    }
    फर्काऊ p;
}
#endif // FEAT_CLIENTSERVER

#यदि defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    स्थिर शून्य
make_connection(शून्य)
{
    यदि (X_DISPLAY == रिक्त
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
    यदि (clientserver_method != CLIENTSERVER_METHOD_X11)
	फर्काऊ OK;
    make_connection();
    यदि (X_DISPLAY == रिक्त)
    {
	emsg(_(e_no_connection_to_x_server));
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}
#endif

#ifdef FEAT_CLIENTSERVER
    स्थिर शून्य
remote_common(typval_T *argvars, typval_T *rettv, पूर्णाङ्क अभिव्यक्ति)
{
    char_u	*server_name;
    char_u	*साँचोहरू;
    char_u	*r = रिक्त;
    char_u	buf[NUMBUFLEN];
    पूर्णाङ्क		timeout = 0;
# ifdef MSWIN
    HWND	w;
# अन्यथा
#  ifdef FEAT_X11
    Window	w;
#  endif
# endif
    पूर्णाङ्क		ret = OK;

    यदि (check_restricted() || check_secure())
	फर्काऊ;

# ifdef FEAT_X11
    यदि (check_connection() == FAIL)
	फर्काऊ;
# endif
    यदि (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_सङ्ख्या(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    यदि (server_name == रिक्त)
	फर्काऊ;		// type error; errmsg already given
    साँचोहरू = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    यदि (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendToVim(server_name, साँचोहरू, &r, &w, अभिव्यक्ति,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    यदि (clientserver_method == CLIENTSERVER_METHOD_SOCKET
	&& socketserver_send(server_name, साँचोहरू, &r, अभिव्यक्ति,
	    timeout * 1000, true, रिक्त) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    यदि (clientserver_method == CLIENTSERVER_METHOD_X11)
	यदि (serverSendToVim(X_DISPLAY, server_name, साँचोहरू, &r, &w, अभिव्यक्ति, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    यदि (ret == FAIL)
    {
	यदि (r != रिक्त)
	{
	    emsg((वर्ण *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	अन्यथा
	    semsg(_(e_unable_to_send_to_str), server_name);
	फर्काऊ;
    }

    rettv->vval.v_string = r;

    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# यदि defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = रिक्त;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	यदि (idvar != रिक्त && *idvar != NUL)
	{
# ifdef MSWIN
	    यदि (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	    {
		प्र_लिखो((वर्ण *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    यदि (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		प्र_लिखो((वर्ण *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    यदि (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		str = server_name;
# endif
	    यदि (str == रिक्त)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    set_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#endif

#यदि defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    शून्य
f_remote_अभिव्यक्ति(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

# ifdef FEAT_CLIENTSERVER
    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_सङ्ख्या_arg(argvars, 3) == FAIL)))
	फर्काऊ;

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
    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	फर्काऊ;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	यदि (server_name != रिक्त)
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

    rettv->vval.v_सङ्ख्या = -1;
    यदि (check_restricted() || check_secure())
	फर्काऊ;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	फर्काऊ;

    serverid = tv_get_string_chk(&argvars[0]);
    यदि (serverid == रिक्त)
	फर्काऊ;		// type error; errmsg already given
#  ifdef MSWIN
    यदि (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	माला_पूछो((अचर वर्ण *)serverid, SCANF_HEX_LONG_U, &n);
	यदि (n == 0)
	    rettv->vval.v_सङ्ख्या = -1;
	अन्यथा
	{
	    s = serverGetReply((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_सङ्ख्या = (s != रिक्त);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    यदि (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	rettv->vval.v_सङ्ख्या = socketserver_peek_reply(serverid, &s);
#  endif
#  ifdef FEAT_X11
    यदि (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	यदि (check_connection() == FAIL)
	    फर्काऊ;

	rettv->vval.v_सङ्ख्या = serverPeekReply(X_DISPLAY,
		serverStrToWin(serverid), &s);
    }
#  endif

    यदि (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_सङ्ख्या > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	यदि (retvar != रिक्त)
	    set_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# अन्यथा
    rettv->vval.v_सङ्ख्या = -1;
# endif
}

    शून्य
f_remote_read(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*r = रिक्त;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL))
	फर्काऊ;

    serverid = tv_get_string_chk(&argvars[0]);
    यदि (serverid != रिक्त && !check_restricted() && !check_secure())
    {
	पूर्णाङ्क timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	यदि (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_सङ्ख्या(&argvars[1]);

#  ifdef MSWIN
	यदि (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	{
	    माला_पूछो((वर्ण *)serverid, SCANF_HEX_LONG_U, &n);
	    यदि (n != 0)
		r = serverGetReply((HWND)n, FALSE, TRUE, TRUE, timeout);
	    यदि (r == रिक्त)
		emsg(_(e_unable_to_read_server_reply));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	यदि (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
		socketserver_read_reply(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_to_read_server_reply));
#  endif
#  ifdef FEAT_X11
	यदि (clientserver_method == CLIENTSERVER_METHOD_X11 &&
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
    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	फर्काऊ;

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
    यदि (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	फर्काऊ;

    यदि (serverName != रिक्त)
    {
	emsg(_(e_already_started_server));
	फर्काऊ;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    यदि (server == रिक्त)
	फर्काऊ;
#  ifdef MSWIN
    यदि (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	serverSetName(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    यदि (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	socketserver_start(server, true);
#  endif
#  ifdef FEAT_X11
    यदि (clientserver_method == CLIENTSERVER_METHOD_X11 &&
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
    पूर्णाङ्क		ret = OK;

    rettv->vval.v_सङ्ख्या = -1;
    यदि (check_restricted() || check_secure())
	फर्काऊ;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	फर्काऊ;

    server = tv_get_string_chk(&argvars[0]);
    reply = tv_get_string_buf_chk(&argvars[1], buf);
    यदि (server == रिक्त || reply == रिक्त)
	फर्काऊ;

#  ifdef FEAT_SOCKETSERVER
    यदि (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
	    socketserver_send_reply(server, reply) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    यदि (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    यदि (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif

    यदि (ret == FAIL)
    {
	emsg(_(e_unable_to_send_to_client));
	फर्काऊ;
    }
    rettv->vval.v_सङ्ख्या = 0;
# अन्यथा
    rettv->vval.v_सङ्ख्या = -1;
# endif
}

    शून्य
f_serverlist(typval_T *argvars UNUSED, typval_T *rettv)
{
    list_T  *list = रिक्त;
    bool    use_list = false;

    यदि (check_for_opt_dict_arg(argvars, 0) == FAIL)
	फर्काऊ;

    यदि (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    यदि (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	list = serverGetVimNames();
#  endif
#  ifdef FEAT_SOCKETSERVER
    यदि (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    यदि (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	make_connection();
	यदि (X_DISPLAY != रिक्त)
	    list = serverGetVimNames(X_DISPLAY);
    }
#  endif
# endif
    यदि (use_list)
    {
	यदि (list == रिक्त)
	{
	    (शून्य)rettv_list_alloc(rettv);
	    फर्काऊ;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    अन्यथा
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	यदि (list != रिक्त)
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

    यदि (env == रिक्त)
	फर्काऊ;

    यदि (STRICMP(env, "socket") == 0)
	clientserver_method = CLIENTSERVER_METHOD_SOCKET;
# ifdef FEAT_X11
    अन्यथा यदि (STRICMP(env, "x11") == 0)
	clientserver_method = CLIENTSERVER_METHOD_X11;
# endif
# ifdef MSWIN
    अन्यथा यदि (STRICMP(env, "mswin") == 0)
	clientserver_method = CLIENTSERVER_METHOD_MSWIN;
# endif
}
#endif
