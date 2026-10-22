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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_CLIENTSERVER)

સ્થિર શૂન્ય cmdsrv_main(પૂર્ણાંક *argc, અક્ષર **argv, char_u *serverName_arg, char_u **serverStr);
સ્થિર char_u *serverMakeName(char_u *arg, અક્ષર *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    શૂન્ય
server_to_આગત_buf(char_u *str)
{
    char_u      *ptr = ખાલી;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, ખાલી);
    p_cpo = cpo_save;

    જો (ptr != ખાલી && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	જો (typebuf.tb_maplen < typebuf.tb_len)
	    del_typebuf(typebuf.tb_len - typebuf.tb_maplen, typebuf.tb_maplen);
	(શૂન્ય)ins_typebuf(str, REMAP_NONE, typebuf.tb_len, TRUE, FALSE);

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
eval_client_પદાવલિ_to_string(char_u *પદાવલિ)
{
    char_u	*res;
    પૂર્ણાંક		save_dbl = debug_break_level;
    પૂર્ણાંક		save_ro = redir_off;
    funccal_entry_T funccal_entry;
    પૂર્ણાંક		did_save_funccal = FALSE;

# જો defined(FEAT_EVAL)
    ch_log(ખાલી, "eval_client_expr_to_string(\"%s\")", પદાવલિ);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    જો (!debug_mode)
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

    res = eval_to_string(પદાવલિ, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    જો (emsg_silent < 0)
	emsg_silent = 0;
    જો (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    out_flush_cursor(FALSE, FALSE);

    પાછા res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    પૂર્ણાંક
sendToLocalVim(char_u *cmd, પૂર્ણાંક as_પદાવલિ, char_u **result)
{
    જો (as_પદાવલિ)
    {
	char_u *ret;

	ret = eval_client_પદાવલિ_to_string(cmd);
	જો (result != ખાલી)
	{
	    જો (ret == ખાલી)
	    {
		અક્ષર	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*સંદેશ;

		સંદેશ = alloc(len);
		જો (સંદેશ != ખાલી)
		    vim_snprintf((અક્ષર *)સંદેશ, len, "%s: \"%s\"", err, cmd);
		*result = સંદેશ;
	    }
	    નહીંતર
		*result = ret;
	}
	નહીંતર
	    vim_free(ret);
	પાછા ret == ખાલી ? -1 : 0;
    }
    server_to_આગત_buf(cmd);
    પાછા 0;
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

    *tofree = ખાલી;
    જો (client_enc == ખાલી || p_enc == ખાલી)
	પાછા res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    જો (convert_setup(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, data, ખાલી);
	જો (res == ખાલી)
	    res = data;
	નહીંતર
	    *tofree = res;
    }
    convert_setup(&vimconv, ખાલી, ખાલી);
    પાછા res;
}
#endif

#જો defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

સ્થિર char_u *build_drop_cmd(પૂર્ણાંક filec, અક્ષર **filev, પૂર્ણાંક tabs, પૂર્ણાંક sendReply);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    શૂન્ય
exec_on_server(mparm_T *parmp)
{
    પૂર્ણાંક made_name = FALSE;

    જો (parmp->serverName_arg != ખાલી && *parmp->serverName_arg == NUL)
	પાછા;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    જો (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    જો (!made_name && parmp->servername == ખાલી)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    જો (parmp->servername != ખાલી && clientserver_method == CLIENTSERVER_METHOD_MSWIN)
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
    શૂન્ય
prepare_server(mparm_T *parmp)
{
# જો defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    જો (
#  જો defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != ખાલી
#   endif
#   જો defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   જો defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_method == CLIENTSERVER_METHOD_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != ખાલી && (
#  જો defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   જો defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_method != CLIENTSERVER_METHOD_SOCKET &&
#   endif
#   જો defined(FEAT_AUTOSERVERNAME)
		    1
#   નહીંતર
		    gui.in_use
#   endif
#   ifdef UNIX
		 && getuid() != મૂળ_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != ખાલી))
    {

#  ifdef FEAT_SOCKETSERVER
	જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	    socketserver_start(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	જો (clientserver_method == CLIENTSERVER_METHOD_X11)
	{
	    (શૂન્ય)serverRegisterName(X_DISPLAY, parmp->servername);
	    TIME_સંદેશ("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    નહીંતર
    {
#  ifdef FEAT_X11
	જો (clientserver_method == CLIENTSERVER_METHOD_X11)
	    serverDelayedStartName = parmp->servername;
	નહીંતર
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    જો (parmp->serverStr != ખાલી)
    {
	char_u *p;

	server_to_આગત_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    સ્થિર શૂન્ય
cmdsrv_main(
    પૂર્ણાંક		*argc,
    અક્ષર	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    પૂર્ણાંક		i;
    char_u	*sname;
    પૂર્ણાંક		ret = -1;
    પૂર્ણાંક		didone = FALSE;
    પૂર્ણાંક		exiterr = 0;
    અક્ષર	**newArgV = argv + 1;
    પૂર્ણાંક		newArgC = 1,
		Argc = *argc;
    પૂર્ણાંક		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    પૂર્ણાંક		silent = FALSE;
    પૂર્ણાંક		tabs = FALSE;
# ifdef MSWIN
    HWND	srv;
# elif defined(FEAT_X11)
    Window	srv;

    setup_term_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = ખાલી;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    જો (sname == ખાલી)
	પાછા;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    માટે (i = 1; i < Argc; i++)
    {
	res = ખાલી;
	જો (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    માટે (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    તોડો;
	}

	જો (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	નહીંતર જો (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    અક્ષર	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    જ્યારે (*p != NUL)
	    {
		જો (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		નહીંતર જો (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		નહીંતર જો (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		નહીંતર
		{
		    argtype = ARGTYPE_OTHER;
		    તોડો;
		}
	    }
	}
	નહીંતર
	    argtype = ARGTYPE_OTHER;

	જો (argtype != ARGTYPE_OTHER)
	{
	    જો (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    જો (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    નહીંતર
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		જો (*serverStr == ખાલી)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    તોડો;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		ret = socketserver_send(sname, *serverStr, ખાલી, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    જો (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		જો (xterm_dpy == ખાલી)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		નહીંતર
		    ret = serverSendToVim(xterm_dpy, sname, *serverStr,
			    ખાલી, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    જો (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		ret = serverSendToVim(sname, *serverStr, ખાલી, &srv, 0, 0,
			silent);
# endif
	    જો (ret < 0)
	    {
		જો (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		નહીંતર જો (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		તોડો;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    જો (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		SetForegroundWindow(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    જો (argtype == ARGTYPE_EDIT_WAIT)
	    {
		પૂર્ણાંક	અંક_Files = *argc - i - 1;
		char_u  *done = alloc(અંક_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		પૂર્ણાંક	ગણતરી = 0;
		બાહ્ય HWND message_window;
# endif

		જો (અંક_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --અંક_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cbSize = કદ(ni);
		ni.hWnd = message_window;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), ગણતરી, અંક_Files);
		Shell_NotifyIcon(NIM_ADD, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(done, 0, અંક_Files);
		જ્યારે (memchr(done, 0, અંક_Files) != ખાલી)
		{
		    char_u  *p = ખાલી;
		    પૂર્ણાંક	    j;
# ifdef MSWIN
		    જો (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
			p = serverGetReply(srv, ખાલી, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET
			    && (ch == ખાલી
				|| socketserver_read_reply(sname, &p, -1, true)
				== FAIL))
			    તોડો;
# endif
# ifdef FEAT_X11
		    જો (clientserver_method == CLIENTSERVER_METHOD_X11
			    && serverReadReply(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    તોડો;
# endif
		    જો (p == ખાલી)
			તોડો;
		    j = atoi((અક્ષર *)p);
		    vim_free(p);
		    જો (j >= 0 && j < અંક_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++ગણતરી;
			sprintf(ni.szTip, _("%d of %d edited"),
							     ગણતરી, અંક_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			done[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		જો (ch != ખાલી)
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
	નહીંતર જો (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    પૂર્ણાંક status = OK;

	    જો (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    જો (clientserver_method == CLIENTSERVER_METHOD_MSWIN
		    && serverSendToVim(sname, (char_u *)argv[i + 1],
						  &res, ખાલી, 1, 0, FALSE) < 0)
		status = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, ખાલી) < 0)
		status = FAIL;
# endif
# ifdef FEAT_X11
	    જો (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		જો (xterm_dpy == ખાલી)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		નહીંતર જો (serverSendToVim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    ખાલી, 1, 0, 1, FALSE) < 0)
		    status = FAIL;
	    }
# endif
	    જો (status == FAIL)
	    {
		જો (res != ખાલી && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((અક્ષર *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	નહીંતર જો (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = ખાલી;
	    garray_T	ga;

# ifdef MSWIN
	    જો (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		list = serverGetVimNames();
# endif
# ifdef FEAT_SOCKETSERVER
	    જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    જો (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		    xterm_dpy != ખાલી)
		list = serverGetVimNames(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    જો (list != ખાલી)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_data;

	    જો (did_emsg)
		mch_errmsg("\n");
	}
	નહીંતર જો (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    ચાલુ;
	}
	નહીંતર
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    ચાલુ;
	}
	didone = TRUE;
	જો (res != ખાલી && *res != NUL)
	{
	    mch_સંદેશ((અક્ષર *)res);
	    જો (res[STRLEN(res) - 1] != '\n')
		mch_સંદેશ("\n");
	}
	vim_free(res);
    }

    જો (didone)
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
    સ્થિર char_u *
build_drop_cmd(
    પૂર્ણાંક		filec,
    અક્ષર	**filev,
    પૂર્ણાંક		tabs,		// Use ":tab drop" instead of ":drop".
    પૂર્ણાંક		sendReply)
{
    garray_T	ga;
    પૂર્ણાંક		i;
    char_u	*inicmd = ખાલી;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    અચલ string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    જો (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    જો (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    જો (cwd == ખાલી)
	પાછા ખાલી;
    જો (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	પાછા ખાલી;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_IN_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# નહીંતર
	    PATH_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    જો (cdp.string == ખાલી)
	પાછા ખાલી;
    cdp.length = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.length);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].length);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    જો (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    માટે (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		PATH_ESC_CHARS
# નહીંતર
		(char_u *)" \t%#"
# endif
		);
	જો (p == ખાલી)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_data);
	    પાછા ખાલી;
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
# નહીંતર
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.length);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].length);

    જો (sendReply)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    જો (inicmd != ખાલી)
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
    પાછા ga.ga_data;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    સ્થિર char_u *
serverMakeName(char_u *arg, અક્ષર *cmd)
{
    char_u *p;

    જો (arg != ખાલી && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	{
	    જો (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    નહીંતર
		p = vim_strsave_up(arg);
	}
	નહીંતર
	    p = vim_strsave_up(arg);
# નહીંતર
	p = vim_strsave_up(arg);
# endif
    }
    નહીંતર
    {
	p = vim_strsave_up(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	જો (p != ખાલી && vim_strchr(p, '.') != ખાલી)
	    *vim_strchr(p, '.') = NUL;
    }
    પાછા p;
}
#endif // FEAT_CLIENTSERVER

#જો defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    સ્થિર શૂન્ય
make_connection(શૂન્ય)
{
    જો (X_DISPLAY == ખાલી
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

    સ્થિર પૂર્ણાંક
check_connection(શૂન્ય)
{
    જો (clientserver_method != CLIENTSERVER_METHOD_X11)
	પાછા OK;
    make_connection();
    જો (X_DISPLAY == ખાલી)
    {
	emsg(_(e_no_connection_to_x_server));
	પાછા FAIL;
    }
    પાછા OK;
}
#endif

#ifdef FEAT_CLIENTSERVER
    સ્થિર શૂન્ય
remote_common(typval_T *argvars, typval_T *rettv, પૂર્ણાંક પદાવલિ)
{
    char_u	*server_name;
    char_u	*ચાવીઓ;
    char_u	*r = ખાલી;
    char_u	buf[NUMBUFLEN];
    પૂર્ણાંક		timeout = 0;
# ifdef MSWIN
    HWND	w;
# નહીંતર
#  ifdef FEAT_X11
    Window	w;
#  endif
# endif
    પૂર્ણાંક		ret = OK;

    જો (check_restricted() || check_secure())
	પાછા;

# ifdef FEAT_X11
    જો (check_connection() == FAIL)
	પાછા;
# endif
    જો (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_સંખ્યા(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    જો (server_name == ખાલી)
	પાછા;		// type error; errmsg already given
    ચાવીઓ = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    જો (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendToVim(server_name, ચાવીઓ, &r, &w, પદાવલિ,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET
	&& socketserver_send(server_name, ચાવીઓ, &r, પદાવલિ,
	    timeout * 1000, true, ખાલી) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    જો (clientserver_method == CLIENTSERVER_METHOD_X11)
	જો (serverSendToVim(X_DISPLAY, server_name, ચાવીઓ, &r, &w, પદાવલિ, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    જો (ret == FAIL)
    {
	જો (r != ખાલી)
	{
	    emsg((અક્ષર *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	નહીંતર
	    semsg(_(e_unable_to_send_to_str), server_name);
	પાછા;
    }

    rettv->vval.v_string = r;

    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# જો defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = ખાલી;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	જો (idvar != ખાલી && *idvar != NUL)
	{
# ifdef MSWIN
	    જો (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	    {
		sprintf((અક્ષર *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    જો (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		sprintf((અક્ષર *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		str = server_name;
# endif
	    જો (str == ખાલી)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    set_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#endif

#જો defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    શૂન્ય
f_remote_પદાવલિ(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

# ifdef FEAT_CLIENTSERVER
    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 3) == FAIL)))
	પાછા;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    શૂન્ય
f_remote_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	જો (server_name != ખાલી)
	    serverForeground(server_name);
    }
#  નહીંતર
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    શૂન્ય
f_remote_peek(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = ખાલી;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_સંખ્યા = -1;
    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	પાછા;

    serverid = tv_get_string_chk(&argvars[0]);
    જો (serverid == ખાલી)
	પાછા;		// type error; errmsg already given
#  ifdef MSWIN
    જો (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	sscanf((અચલ અક્ષર *)serverid, SCANF_HEX_LONG_U, &n);
	જો (n == 0)
	    rettv->vval.v_સંખ્યા = -1;
	નહીંતર
	{
	    s = serverGetReply((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_સંખ્યા = (s != ખાલી);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	rettv->vval.v_સંખ્યા = socketserver_peek_reply(serverid, &s);
#  endif
#  ifdef FEAT_X11
    જો (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	જો (check_connection() == FAIL)
	    પાછા;

	rettv->vval.v_સંખ્યા = serverPeekReply(X_DISPLAY,
		serverStrToWin(serverid), &s);
    }
#  endif

    જો (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_સંખ્યા > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	જો (retvar != ખાલી)
	    set_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# નહીંતર
    rettv->vval.v_સંખ્યા = -1;
# endif
}

    શૂન્ય
f_remote_read(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*r = ખાલી;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    serverid = tv_get_string_chk(&argvars[0]);
    જો (serverid != ખાલી && !check_restricted() && !check_secure())
    {
	પૂર્ણાંક timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	જો (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_સંખ્યા(&argvars[1]);

#  ifdef MSWIN
	જો (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	{
	    sscanf((અક્ષર *)serverid, SCANF_HEX_LONG_U, &n);
	    જો (n != 0)
		r = serverGetReply((HWND)n, FALSE, TRUE, TRUE, timeout);
	    જો (r == ખાલી)
		emsg(_(e_unable_to_read_server_reply));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
		socketserver_read_reply(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_to_read_server_reply));
#  endif
#  ifdef FEAT_X11
	જો (clientserver_method == CLIENTSERVER_METHOD_X11 &&
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
    શૂન્ય
f_remote_send(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

# ifdef FEAT_CLIENTSERVER
    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	પાછા;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    શૂન્ય
f_remote_startserver(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    જો (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	પાછા;

    જો (serverName != ખાલી)
    {
	emsg(_(e_already_started_server));
	પાછા;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    જો (server == ખાલી)
	પાછા;
#  ifdef MSWIN
    જો (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	serverSetName(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	socketserver_start(server, true);
#  endif
#  ifdef FEAT_X11
    જો (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    check_connection() == OK)
	serverRegisterName(X_DISPLAY, server);
#  endif

# નહીંતર
    emsg(_(e_clientserver_feature_not_available));
# endif
}

    શૂન્ય
f_server2client(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*reply;
    પૂર્ણાંક		ret = OK;

    rettv->vval.v_સંખ્યા = -1;
    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    server = tv_get_string_chk(&argvars[0]);
    reply = tv_get_string_buf_chk(&argvars[1], buf);
    જો (server == ખાલી || reply == ખાલી)
	પાછા;

#  ifdef FEAT_SOCKETSERVER
    જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
	    socketserver_send_reply(server, reply) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    જો (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    જો (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif

    જો (ret == FAIL)
    {
	emsg(_(e_unable_to_send_to_client));
	પાછા;
    }
    rettv->vval.v_સંખ્યા = 0;
# નહીંતર
    rettv->vval.v_સંખ્યા = -1;
# endif
}

    શૂન્ય
f_serverlist(typval_T *argvars UNUSED, typval_T *rettv)
{
    list_T  *list = ખાલી;
    bool    use_list = false;

    જો (check_for_opt_dict_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    જો (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	list = serverGetVimNames();
#  endif
#  ifdef FEAT_SOCKETSERVER
    જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    જો (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	make_connection();
	જો (X_DISPLAY != ખાલી)
	    list = serverGetVimNames(X_DISPLAY);
    }
#  endif
# endif
    જો (use_list)
    {
	જો (list == ખાલી)
	{
	    (શૂન્ય)rettv_list_alloc(rettv);
	    પાછા;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    નહીંતર
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	જો (list != ખાલી)
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
    શૂન્ય
check_clientserver_method_env(શૂન્ય)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    જો (env == ખાલી)
	પાછા;

    જો (STRICMP(env, "socket") == 0)
	clientserver_method = CLIENTSERVER_METHOD_SOCKET;
# ifdef FEAT_X11
    નહીંતર જો (STRICMP(env, "x11") == 0)
	clientserver_method = CLIENTSERVER_METHOD_X11;
# endif
# ifdef MSWIN
    નહીંતર જો (STRICMP(env, "mswin") == 0)
	clientserver_method = CLIENTSERVER_METHOD_MSWIN;
# endif
}
#endif
