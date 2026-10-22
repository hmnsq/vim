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

#شامل "vim.h"

#اگر defined(FEAT_CLIENTSERVER)

ایستا تهی cmdsrv_main(صحیح *argc, نویسه **argv, char_u *serverName_arg, char_u **serverStr);
ایستا char_u *serverMakeName(char_u *arg, نویسه *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    تهی
server_to_درون_داد_buf(char_u *str)
{
    char_u      *ptr = پوچ;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, پوچ);
    p_cpo = cpo_save;

    اگر (ptr != پوچ && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	اگر (typebuf.tb_maplen < typebuf.tb_len)
	    del_typebuf(typebuf.tb_len - typebuf.tb_maplen, typebuf.tb_maplen);
	(تهی)ins_typebuf(str, REMAP_NONE, typebuf.tb_len, TRUE, FALSE);

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
eval_client_عبارت_to_string(char_u *عبارت)
{
    char_u	*res;
    صحیح		save_dbl = debug_break_level;
    صحیح		save_ro = redir_off;
    funccal_entry_T funccal_entry;
    صحیح		did_save_funccal = FALSE;

# اگر defined(FEAT_EVAL)
    ch_log(پوچ, "eval_client_expr_to_string(\"%s\")", عبارت);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    اگر (!debug_mode)
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

    res = eval_to_string(عبارت, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    اگر (emsg_silent < 0)
	emsg_silent = 0;
    اگر (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    out_flush_cursor(FALSE, FALSE);

    بازگشت res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    صحیح
sendToLocalVim(char_u *cmd, صحیح as_عبارت, char_u **result)
{
    اگر (as_عبارت)
    {
	char_u *ret;

	ret = eval_client_عبارت_to_string(cmd);
	اگر (result != پوچ)
	{
	    اگر (ret == پوچ)
	    {
		نویسه	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*پیام;

		پیام = alloc(len);
		اگر (پیام != پوچ)
		    vim_snprintf((نویسه *)پیام, len, "%s: \"%s\"", err, cmd);
		*result = پیام;
	    }
	    وگرنه
		*result = ret;
	}
	وگرنه
	    vim_free(ret);
	بازگشت ret == پوچ ? -1 : 0;
    }
    server_to_درون_داد_buf(cmd);
    بازگشت 0;
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

    *tofree = پوچ;
    اگر (client_enc == پوچ || p_enc == پوچ)
	بازگشت res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    اگر (convert_setup(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, data, پوچ);
	اگر (res == پوچ)
	    res = data;
	وگرنه
	    *tofree = res;
    }
    convert_setup(&vimconv, پوچ, پوچ);
    بازگشت res;
}
#endif

#اگر defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

ایستا char_u *build_drop_cmd(صحیح filec, نویسه **filev, صحیح tabs, صحیح sendReply);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    تهی
exec_on_server(mparm_T *parmp)
{
    صحیح made_name = FALSE;

    اگر (parmp->serverName_arg != پوچ && *parmp->serverName_arg == NUL)
	بازگشت;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    اگر (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    اگر (!made_name && parmp->servername == پوچ)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    اگر (parmp->servername != پوچ && clientserver_method == CLIENTSERVER_METHOD_MSWIN)
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
    تهی
prepare_server(mparm_T *parmp)
{
# اگر defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    اگر (
#  اگر defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != پوچ
#   endif
#   اگر defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   اگر defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_method == CLIENTSERVER_METHOD_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != پوچ && (
#  اگر defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   اگر defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_method != CLIENTSERVER_METHOD_SOCKET &&
#   endif
#   اگر defined(FEAT_AUTOSERVERNAME)
		    1
#   وگرنه
		    gui.in_use
#   endif
#   ifdef UNIX
		 && getuid() != ریشه_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != پوچ))
    {

#  ifdef FEAT_SOCKETSERVER
	اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	    socketserver_start(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	اگر (clientserver_method == CLIENTSERVER_METHOD_X11)
	{
	    (تهی)serverRegisterName(X_DISPLAY, parmp->servername);
	    TIME_پیام("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    وگرنه
    {
#  ifdef FEAT_X11
	اگر (clientserver_method == CLIENTSERVER_METHOD_X11)
	    serverDelayedStartName = parmp->servername;
	وگرنه
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    اگر (parmp->serverStr != پوچ)
    {
	char_u *p;

	server_to_درون_داد_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    ایستا تهی
cmdsrv_main(
    صحیح		*argc,
    نویسه	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    صحیح		i;
    char_u	*sname;
    صحیح		ret = -1;
    صحیح		didone = FALSE;
    صحیح		exiterr = 0;
    نویسه	**newArgV = argv + 1;
    صحیح		newArgC = 1,
		Argc = *argc;
    صحیح		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    صحیح		silent = FALSE;
    صحیح		tabs = FALSE;
# ifdef MSWIN
    HWND	srv;
# elif defined(FEAT_X11)
    Window	srv;

    setup_term_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = پوچ;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    اگر (sname == پوچ)
	بازگشت;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    برای (i = 1; i < Argc; i++)
    {
	res = پوچ;
	اگر (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    برای (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    بشکن;
	}

	اگر (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	وگرنه اگر (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    نویسه	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    وقتی (*p != NUL)
	    {
		اگر (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		وگرنه اگر (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		وگرنه اگر (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		وگرنه
		{
		    argtype = ARGTYPE_OTHER;
		    بشکن;
		}
	    }
	}
	وگرنه
	    argtype = ARGTYPE_OTHER;

	اگر (argtype != ARGTYPE_OTHER)
	{
	    اگر (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    اگر (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    وگرنه
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		اگر (*serverStr == پوچ)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    بشکن;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		ret = socketserver_send(sname, *serverStr, پوچ, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    اگر (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		اگر (xterm_dpy == پوچ)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		وگرنه
		    ret = serverSendToVim(xterm_dpy, sname, *serverStr,
			    پوچ, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    اگر (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		ret = serverSendToVim(sname, *serverStr, پوچ, &srv, 0, 0,
			silent);
# endif
	    اگر (ret < 0)
	    {
		اگر (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		وگرنه اگر (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		بشکن;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    اگر (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		SetForegroundWindow(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    اگر (argtype == ARGTYPE_EDIT_WAIT)
	    {
		صحیح	رقم_Files = *argc - i - 1;
		char_u  *done = alloc(رقم_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		صحیح	شمار = 0;
		بیرونی HWND message_window;
# endif

		اگر (رقم_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --رقم_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cbSize = اندازه(ni);
		ni.hWnd = message_window;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), شمار, رقم_Files);
		Shell_NotifyIcon(NIM_ADD, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(done, 0, رقم_Files);
		وقتی (memchr(done, 0, رقم_Files) != پوچ)
		{
		    char_u  *p = پوچ;
		    صحیح	    j;
# ifdef MSWIN
		    اگر (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
			p = serverGetReply(srv, پوچ, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET
			    && (ch == پوچ
				|| socketserver_read_reply(sname, &p, -1, true)
				== FAIL))
			    بشکن;
# endif
# ifdef FEAT_X11
		    اگر (clientserver_method == CLIENTSERVER_METHOD_X11
			    && serverReadReply(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    بشکن;
# endif
		    اگر (p == پوچ)
			بشکن;
		    j = atoi((نویسه *)p);
		    vim_free(p);
		    اگر (j >= 0 && j < رقم_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++شمار;
			sprintf(ni.szTip, _("%d of %d edited"),
							     شمار, رقم_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			done[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		اگر (ch != پوچ)
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
	وگرنه اگر (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    صحیح status = OK;

	    اگر (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    اگر (clientserver_method == CLIENTSERVER_METHOD_MSWIN
		    && serverSendToVim(sname, (char_u *)argv[i + 1],
						  &res, پوچ, 1, 0, FALSE) < 0)
		status = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, پوچ) < 0)
		status = FAIL;
# endif
# ifdef FEAT_X11
	    اگر (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		اگر (xterm_dpy == پوچ)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		وگرنه اگر (serverSendToVim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    پوچ, 1, 0, 1, FALSE) < 0)
		    status = FAIL;
	    }
# endif
	    اگر (status == FAIL)
	    {
		اگر (res != پوچ && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((نویسه *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	وگرنه اگر (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = پوچ;
	    garray_T	ga;

# ifdef MSWIN
	    اگر (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		list = serverGetVimNames();
# endif
# ifdef FEAT_SOCKETSERVER
	    اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    اگر (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		    xterm_dpy != پوچ)
		list = serverGetVimNames(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    اگر (list != پوچ)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_data;

	    اگر (did_emsg)
		mch_errmsg("\n");
	}
	وگرنه اگر (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    ادامه;
	}
	وگرنه
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    ادامه;
	}
	didone = TRUE;
	اگر (res != پوچ && *res != NUL)
	{
	    mch_پیام((نویسه *)res);
	    اگر (res[STRLEN(res) - 1] != '\n')
		mch_پیام("\n");
	}
	vim_free(res);
    }

    اگر (didone)
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
    ایستا char_u *
build_drop_cmd(
    صحیح		filec,
    نویسه	**filev,
    صحیح		tabs,		// Use ":tab drop" instead of ":drop".
    صحیح		sendReply)
{
    garray_T	ga;
    صحیح		i;
    char_u	*inicmd = پوچ;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    ثابت string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    اگر (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    اگر (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    اگر (cwd == پوچ)
	بازگشت پوچ;
    اگر (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	بازگشت پوچ;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_IN_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# وگرنه
	    PATH_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    اگر (cdp.string == پوچ)
	بازگشت پوچ;
    cdp.length = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.length);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].length);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    اگر (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    برای (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		PATH_ESC_CHARS
# وگرنه
		(char_u *)" \t%#"
# endif
		);
	اگر (p == پوچ)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_data);
	    بازگشت پوچ;
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
# وگرنه
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.length);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].length);

    اگر (sendReply)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    اگر (inicmd != پوچ)
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
    بازگشت ga.ga_data;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    ایستا char_u *
serverMakeName(char_u *arg, نویسه *cmd)
{
    char_u *p;

    اگر (arg != پوچ && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	{
	    اگر (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    وگرنه
		p = vim_strsave_up(arg);
	}
	وگرنه
	    p = vim_strsave_up(arg);
# وگرنه
	p = vim_strsave_up(arg);
# endif
    }
    وگرنه
    {
	p = vim_strsave_up(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	اگر (p != پوچ && vim_strchr(p, '.') != پوچ)
	    *vim_strchr(p, '.') = NUL;
    }
    بازگشت p;
}
#endif // FEAT_CLIENTSERVER

#اگر defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    ایستا تهی
make_connection(تهی)
{
    اگر (X_DISPLAY == پوچ
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

    ایستا صحیح
check_connection(تهی)
{
    اگر (clientserver_method != CLIENTSERVER_METHOD_X11)
	بازگشت OK;
    make_connection();
    اگر (X_DISPLAY == پوچ)
    {
	emsg(_(e_no_connection_to_x_server));
	بازگشت FAIL;
    }
    بازگشت OK;
}
#endif

#ifdef FEAT_CLIENTSERVER
    ایستا تهی
remote_common(typval_T *argvars, typval_T *rettv, صحیح عبارت)
{
    char_u	*server_name;
    char_u	*کلیدها;
    char_u	*r = پوچ;
    char_u	buf[NUMBUFLEN];
    صحیح		timeout = 0;
# ifdef MSWIN
    HWND	w;
# وگرنه
#  ifdef FEAT_X11
    Window	w;
#  endif
# endif
    صحیح		ret = OK;

    اگر (check_restricted() || check_secure())
	بازگشت;

# ifdef FEAT_X11
    اگر (check_connection() == FAIL)
	بازگشت;
# endif
    اگر (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_عدد(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    اگر (server_name == پوچ)
	بازگشت;		// type error; errmsg already given
    کلیدها = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    اگر (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendToVim(server_name, کلیدها, &r, &w, عبارت,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET
	&& socketserver_send(server_name, کلیدها, &r, عبارت,
	    timeout * 1000, true, پوچ) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    اگر (clientserver_method == CLIENTSERVER_METHOD_X11)
	اگر (serverSendToVim(X_DISPLAY, server_name, کلیدها, &r, &w, عبارت, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    اگر (ret == FAIL)
    {
	اگر (r != پوچ)
	{
	    emsg((نویسه *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	وگرنه
	    semsg(_(e_unable_to_send_to_str), server_name);
	بازگشت;
    }

    rettv->vval.v_string = r;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# اگر defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = پوچ;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	اگر (idvar != پوچ && *idvar != NUL)
	{
# ifdef MSWIN
	    اگر (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	    {
		sprintf((نویسه *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    اگر (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		sprintf((نویسه *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		str = server_name;
# endif
	    اگر (str == پوچ)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    set_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#endif

#اگر defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    تهی
f_remote_عبارت(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

# ifdef FEAT_CLIENTSERVER
    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 3) == FAIL)))
	بازگشت;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    تهی
f_remote_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	اگر (server_name != پوچ)
	    serverForeground(server_name);
    }
#  وگرنه
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    تهی
f_remote_peek(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = پوچ;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_عدد = -1;
    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	بازگشت;

    serverid = tv_get_string_chk(&argvars[0]);
    اگر (serverid == پوچ)
	بازگشت;		// type error; errmsg already given
#  ifdef MSWIN
    اگر (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	sscanf((ثابت نویسه *)serverid, SCANF_HEX_LONG_U, &n);
	اگر (n == 0)
	    rettv->vval.v_عدد = -1;
	وگرنه
	{
	    s = serverGetReply((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_عدد = (s != پوچ);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	rettv->vval.v_عدد = socketserver_peek_reply(serverid, &s);
#  endif
#  ifdef FEAT_X11
    اگر (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	اگر (check_connection() == FAIL)
	    بازگشت;

	rettv->vval.v_عدد = serverPeekReply(X_DISPLAY,
		serverStrToWin(serverid), &s);
    }
#  endif

    اگر (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_عدد > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	اگر (retvar != پوچ)
	    set_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# وگرنه
    rettv->vval.v_عدد = -1;
# endif
}

    تهی
f_remote_read(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*r = پوچ;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    serverid = tv_get_string_chk(&argvars[0]);
    اگر (serverid != پوچ && !check_restricted() && !check_secure())
    {
	صحیح timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	اگر (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_عدد(&argvars[1]);

#  ifdef MSWIN
	اگر (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	{
	    sscanf((نویسه *)serverid, SCANF_HEX_LONG_U, &n);
	    اگر (n != 0)
		r = serverGetReply((HWND)n, FALSE, TRUE, TRUE, timeout);
	    اگر (r == پوچ)
		emsg(_(e_unable_to_read_server_reply));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
		socketserver_read_reply(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_to_read_server_reply));
#  endif
#  ifdef FEAT_X11
	اگر (clientserver_method == CLIENTSERVER_METHOD_X11 &&
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
    تهی
f_remote_send(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

# ifdef FEAT_CLIENTSERVER
    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	بازگشت;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    تهی
f_remote_startserver(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    اگر (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (serverName != پوچ)
    {
	emsg(_(e_already_started_server));
	بازگشت;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    اگر (server == پوچ)
	بازگشت;
#  ifdef MSWIN
    اگر (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	serverSetName(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	socketserver_start(server, true);
#  endif
#  ifdef FEAT_X11
    اگر (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    check_connection() == OK)
	serverRegisterName(X_DISPLAY, server);
#  endif

# وگرنه
    emsg(_(e_clientserver_feature_not_available));
# endif
}

    تهی
f_server2client(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*reply;
    صحیح		ret = OK;

    rettv->vval.v_عدد = -1;
    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    server = tv_get_string_chk(&argvars[0]);
    reply = tv_get_string_buf_chk(&argvars[1], buf);
    اگر (server == پوچ || reply == پوچ)
	بازگشت;

#  ifdef FEAT_SOCKETSERVER
    اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
	    socketserver_send_reply(server, reply) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    اگر (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    اگر (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif

    اگر (ret == FAIL)
    {
	emsg(_(e_unable_to_send_to_client));
	بازگشت;
    }
    rettv->vval.v_عدد = 0;
# وگرنه
    rettv->vval.v_عدد = -1;
# endif
}

    تهی
f_serverlist(typval_T *argvars UNUSED, typval_T *rettv)
{
    list_T  *list = پوچ;
    bool    use_list = false;

    اگر (check_for_opt_dict_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    اگر (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	list = serverGetVimNames();
#  endif
#  ifdef FEAT_SOCKETSERVER
    اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    اگر (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	make_connection();
	اگر (X_DISPLAY != پوچ)
	    list = serverGetVimNames(X_DISPLAY);
    }
#  endif
# endif
    اگر (use_list)
    {
	اگر (list == پوچ)
	{
	    (تهی)rettv_list_alloc(rettv);
	    بازگشت;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    وگرنه
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	اگر (list != پوچ)
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
    تهی
check_clientserver_method_env(تهی)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    اگر (env == پوچ)
	بازگشت;

    اگر (STRICMP(env, "socket") == 0)
	clientserver_method = CLIENTSERVER_METHOD_SOCKET;
# ifdef FEAT_X11
    وگرنه اگر (STRICMP(env, "x11") == 0)
	clientserver_method = CLIENTSERVER_METHOD_X11;
# endif
# ifdef MSWIN
    وگرنه اگر (STRICMP(env, "mswin") == 0)
	clientserver_method = CLIENTSERVER_METHOD_MSWIN;
# endif
}
#endif
