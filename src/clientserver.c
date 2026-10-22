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

#ضمن "vim.h"

#אם defined(FEAT_CLIENTSERVER)

ساكن فراغ cmdsrv_main(صحيح *argc, حرف **argv, char_u *serverName_arg, char_u **serverStr);
ساكن char_u *serverMakeName(char_u *arg, حرف *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    فراغ
server_to_input_buf(char_u *str)
{
    char_u      *ptr = عدم;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, عدم);
    p_cpo = cpo_save;

    אם (ptr != عدم && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	אם (typebuf.tb_maplen < typebuf.tb_len)
	    del_typebuf(typebuf.tb_len - typebuf.tb_maplen, typebuf.tb_maplen);
	(فراغ)ins_typebuf(str, REMAP_NONE, typebuf.tb_len, TRUE, FALSE);

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
eval_client_expr_to_string(char_u *expr)
{
    char_u	*res;
    صحيح		save_dbl = debug_break_level;
    صحيح		save_ro = redir_off;
    funccal_entry_T funccal_entry;
    صحيح		did_save_funccal = FALSE;

# אם defined(FEAT_EVAL)
    ch_log(عدم, "eval_client_expr_to_string(\"%s\")", expr);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    אם (!debug_mode)
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

    res = eval_to_string(expr, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    אם (emsg_silent < 0)
	emsg_silent = 0;
    אם (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    out_flush_cursor(FALSE, FALSE);

    החזר res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    صحيح
sendToLocalVim(char_u *cmd, صحيح asExpr, char_u **result)
{
    אם (asExpr)
    {
	char_u *ret;

	ret = eval_client_expr_to_string(cmd);
	אם (result != عدم)
	{
	    אם (ret == عدم)
	    {
		حرف	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*msg;

		msg = alloc(len);
		אם (msg != عدم)
		    vim_snprintf((حرف *)msg, len, "%s: \"%s\"", err, cmd);
		*result = msg;
	    }
	    אחרת
		*result = ret;
	}
	אחרת
	    vim_free(ret);
	החזר ret == عدم ? -1 : 0;
    }
    server_to_input_buf(cmd);
    החזר 0;
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

    *tofree = عدم;
    אם (client_enc == عدم || p_enc == عدم)
	החזר res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    אם (convert_setup(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, data, عدم);
	אם (res == عدم)
	    res = data;
	אחרת
	    *tofree = res;
    }
    convert_setup(&vimconv, عدم, عدم);
    החזר res;
}
#endif

#אם defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

ساكن char_u *build_drop_cmd(صحيح filec, حرف **filev, صحيح tabs, صحيح sendReply);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    فراغ
exec_on_server(mparm_T *parmp)
{
    صحيح made_name = FALSE;

    אם (parmp->serverName_arg != عدم && *parmp->serverName_arg == NUL)
	החזר;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    אם (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    אם (!made_name && parmp->servername == عدم)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    אם (parmp->servername != عدم && clientserver_method == CLIENTSERVER_METHOD_MSWIN)
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
    فراغ
prepare_server(mparm_T *parmp)
{
# אם defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    אם (
#  אם defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != عدم
#   endif
#   אם defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   אם defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_method == CLIENTSERVER_METHOD_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != عدم && (
#  אם defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   אם defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_method != CLIENTSERVER_METHOD_SOCKET &&
#   endif
#   אם defined(FEAT_AUTOSERVERNAME)
		    1
#   אחרת
		    gui.in_use
#   endif
#   ifdef UNIX
		 && getuid() != ROOT_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != عدم))
    {

#  ifdef FEAT_SOCKETSERVER
	אם (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	    socketserver_start(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	אם (clientserver_method == CLIENTSERVER_METHOD_X11)
	{
	    (فراغ)serverRegisterName(X_DISPLAY, parmp->servername);
	    TIME_MSG("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    אחרת
    {
#  ifdef FEAT_X11
	אם (clientserver_method == CLIENTSERVER_METHOD_X11)
	    serverDelayedStartName = parmp->servername;
	אחרת
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    אם (parmp->serverStr != عدم)
    {
	char_u *p;

	server_to_input_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    ساكن فراغ
cmdsrv_main(
    صحيح		*argc,
    حرف	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    صحيح		i;
    char_u	*sname;
    صحيح		ret = -1;
    صحيح		didone = FALSE;
    صحيح		exiterr = 0;
    حرف	**newArgV = argv + 1;
    صحيح		newArgC = 1,
		Argc = *argc;
    صحيح		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    صحيح		silent = FALSE;
    صحيح		tabs = FALSE;
# ifdef MSWIN
    HWND	srv;
# elif defined(FEAT_X11)
    Window	srv;

    setup_term_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = عدم;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    אם (sname == عدم)
	החזר;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    עבור (i = 1; i < Argc; i++)
    {
	res = عدم;
	אם (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    עבור (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    اكسر;
	}

	אם (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	אחרת אם (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    حرف	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    כלעוד (*p != NUL)
	    {
		אם (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		אחרת אם (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		אחרת אם (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		אחרת
		{
		    argtype = ARGTYPE_OTHER;
		    اكسر;
		}
	    }
	}
	אחרת
	    argtype = ARGTYPE_OTHER;

	אם (argtype != ARGTYPE_OTHER)
	{
	    אם (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    אם (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    אחרת
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		אם (*serverStr == عدم)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    اكسر;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    אם (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		ret = socketserver_send(sname, *serverStr, عدم, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    אם (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		אם (xterm_dpy == عدم)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		אחרת
		    ret = serverSendToVim(xterm_dpy, sname, *serverStr,
			    عدم, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    אם (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		ret = serverSendToVim(sname, *serverStr, عدم, &srv, 0, 0,
			silent);
# endif
	    אם (ret < 0)
	    {
		אם (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		אחרת אם (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		اكسر;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    אם (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		SetForegroundWindow(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    אם (argtype == ARGTYPE_EDIT_WAIT)
	    {
		صحيح	numFiles = *argc - i - 1;
		char_u  *done = alloc(numFiles);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		صحيح	count = 0;
		خارجي HWND message_window;
# endif

		אם (numFiles > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --numFiles;

# ifdef FEAT_GUI_MSWIN
		ni.cbSize = حجم(ni);
		ni.hWnd = message_window;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), count, numFiles);
		Shell_NotifyIcon(NIM_ADD, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(done, 0, numFiles);
		כלעוד (memchr(done, 0, numFiles) != عدم)
		{
		    char_u  *p = عدم;
		    صحيح	    j;
# ifdef MSWIN
		    אם (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
			p = serverGetReply(srv, عدم, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    אם (clientserver_method == CLIENTSERVER_METHOD_SOCKET
			    && (ch == عدم
				|| socketserver_read_reply(sname, &p, -1, true)
				== FAIL))
			    اكسر;
# endif
# ifdef FEAT_X11
		    אם (clientserver_method == CLIENTSERVER_METHOD_X11
			    && serverReadReply(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    اكسر;
# endif
		    אם (p == عدم)
			اكسر;
		    j = atoi((حرف *)p);
		    vim_free(p);
		    אם (j >= 0 && j < numFiles)
		    {
# ifdef FEAT_GUI_MSWIN
			++count;
			sprintf(ni.szTip, _("%d of %d edited"),
							     count, numFiles);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			done[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		אם (ch != عدم)
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
	אחרת אם (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    صحيح status = OK;

	    אם (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    אם (clientserver_method == CLIENTSERVER_METHOD_MSWIN
		    && serverSendToVim(sname, (char_u *)argv[i + 1],
						  &res, عدم, 1, 0, FALSE) < 0)
		status = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    אם (clientserver_method == CLIENTSERVER_METHOD_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, عدم) < 0)
		status = FAIL;
# endif
# ifdef FEAT_X11
	    אם (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		אם (xterm_dpy == عدم)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		אחרת אם (serverSendToVim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    عدم, 1, 0, 1, FALSE) < 0)
		    status = FAIL;
	    }
# endif
	    אם (status == FAIL)
	    {
		אם (res != عدم && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((حرف *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	אחרת אם (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = عدم;
	    garray_T	ga;

# ifdef MSWIN
	    אם (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		list = serverGetVimNames();
# endif
# ifdef FEAT_SOCKETSERVER
	    אם (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    אם (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		    xterm_dpy != عدم)
		list = serverGetVimNames(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    אם (list != عدم)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_data;

	    אם (did_emsg)
		mch_errmsg("\n");
	}
	אחרת אם (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    استمر;
	}
	אחרת
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    استمر;
	}
	didone = TRUE;
	אם (res != عدم && *res != NUL)
	{
	    mch_msg((حرف *)res);
	    אם (res[STRLEN(res) - 1] != '\n')
		mch_msg("\n");
	}
	vim_free(res);
    }

    אם (didone)
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
    ساكن char_u *
build_drop_cmd(
    صحيح		filec,
    حرف	**filev,
    صحيح		tabs,		// Use ":tab drop" instead of ":drop".
    صحيح		sendReply)
{
    garray_T	ga;
    صحيح		i;
    char_u	*inicmd = عدم;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    ثابت string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    אם (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    אם (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    אם (cwd == عدم)
	החזר عدم;
    אם (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	החזר عدم;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_IN_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# אחרת
	    PATH_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    אם (cdp.string == عدم)
	החזר عدم;
    cdp.length = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.length);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].length);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    אם (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    עבור (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		PATH_ESC_CHARS
# אחרת
		(char_u *)" \t%#"
# endif
		);
	אם (p == عدم)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_data);
	    החזר عدم;
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
# אחרת
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.length);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].length);

    אם (sendReply)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    אם (inicmd != عدم)
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
    החזר ga.ga_data;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    ساكن char_u *
serverMakeName(char_u *arg, حرف *cmd)
{
    char_u *p;

    אם (arg != عدم && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	אם (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	{
	    אם (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    אחרת
		p = vim_strsave_up(arg);
	}
	אחרת
	    p = vim_strsave_up(arg);
# אחרת
	p = vim_strsave_up(arg);
# endif
    }
    אחרת
    {
	p = vim_strsave_up(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	אם (p != عدم && vim_strchr(p, '.') != عدم)
	    *vim_strchr(p, '.') = NUL;
    }
    החזר p;
}
#endif // FEAT_CLIENTSERVER

#אם defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    ساكن فراغ
make_connection(فراغ)
{
    אם (X_DISPLAY == عدم
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

    ساكن صحيح
check_connection(فراغ)
{
    אם (clientserver_method != CLIENTSERVER_METHOD_X11)
	החזר OK;
    make_connection();
    אם (X_DISPLAY == عدم)
    {
	emsg(_(e_no_connection_to_x_server));
	החזר FAIL;
    }
    החזר OK;
}
#endif

#ifdef FEAT_CLIENTSERVER
    ساكن فراغ
remote_common(typval_T *argvars, typval_T *rettv, صحيح expr)
{
    char_u	*server_name;
    char_u	*keys;
    char_u	*r = عدم;
    char_u	buf[NUMBUFLEN];
    صحيح		timeout = 0;
# ifdef MSWIN
    HWND	w;
# אחרת
#  ifdef FEAT_X11
    Window	w;
#  endif
# endif
    صحيح		ret = OK;

    אם (check_restricted() || check_secure())
	החזר;

# ifdef FEAT_X11
    אם (check_connection() == FAIL)
	החזר;
# endif
    אם (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_number(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    אם (server_name == عدم)
	החזר;		// type error; errmsg already given
    keys = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    אם (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendToVim(server_name, keys, &r, &w, expr,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    אם (clientserver_method == CLIENTSERVER_METHOD_SOCKET
	&& socketserver_send(server_name, keys, &r, expr,
	    timeout * 1000, true, عدم) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    אם (clientserver_method == CLIENTSERVER_METHOD_X11)
	אם (serverSendToVim(X_DISPLAY, server_name, keys, &r, &w, expr, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    אם (ret == FAIL)
    {
	אם (r != عدم)
	{
	    emsg((حرف *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	אחרת
	    semsg(_(e_unable_to_send_to_str), server_name);
	החזר;
    }

    rettv->vval.v_string = r;

    אם (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# אם defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = عدم;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	אם (idvar != عدم && *idvar != NUL)
	{
# ifdef MSWIN
	    אם (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	    {
		sprintf((حرف *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    אם (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		sprintf((حرف *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    אם (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		str = server_name;
# endif
	    אם (str == عدم)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    set_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#endif

#אם defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    فراغ
f_remote_expr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

# ifdef FEAT_CLIENTSERVER
    אם (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 3) == FAIL)))
	החזר;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    فراغ
f_remote_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    אם (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	החזר;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	אם (server_name != عدم)
	    serverForeground(server_name);
    }
#  אחרת
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    فراغ
f_remote_peek(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = عدم;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_number = -1;
    אם (check_restricted() || check_secure())
	החזר;

    אם (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	החזר;

    serverid = tv_get_string_chk(&argvars[0]);
    אם (serverid == عدم)
	החזר;		// type error; errmsg already given
#  ifdef MSWIN
    אם (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	sscanf((ثابت حرف *)serverid, SCANF_HEX_LONG_U, &n);
	אם (n == 0)
	    rettv->vval.v_number = -1;
	אחרת
	{
	    s = serverGetReply((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_number = (s != عدم);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    אם (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	rettv->vval.v_number = socketserver_peek_reply(serverid, &s);
#  endif
#  ifdef FEAT_X11
    אם (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	אם (check_connection() == FAIL)
	    החזר;

	rettv->vval.v_number = serverPeekReply(X_DISPLAY,
		serverStrToWin(serverid), &s);
    }
#  endif

    אם (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_number > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	אם (retvar != عدم)
	    set_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# אחרת
    rettv->vval.v_number = -1;
# endif
}

    فراغ
f_remote_read(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*r = عدم;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    אם (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	החזר;

    serverid = tv_get_string_chk(&argvars[0]);
    אם (serverid != عدم && !check_restricted() && !check_secure())
    {
	صحيح timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	אם (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_number(&argvars[1]);

#  ifdef MSWIN
	אם (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	{
	    sscanf((حرف *)serverid, SCANF_HEX_LONG_U, &n);
	    אם (n != 0)
		r = serverGetReply((HWND)n, FALSE, TRUE, TRUE, timeout);
	    אם (r == عدم)
		emsg(_(e_unable_to_read_server_reply));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	אם (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
		socketserver_read_reply(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_to_read_server_reply));
#  endif
#  ifdef FEAT_X11
	אם (clientserver_method == CLIENTSERVER_METHOD_X11 &&
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
    فراغ
f_remote_send(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

# ifdef FEAT_CLIENTSERVER
    אם (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	החזר;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    فراغ
f_remote_startserver(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    אם (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	החזר;

    אם (serverName != عدم)
    {
	emsg(_(e_already_started_server));
	החזר;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    אם (server == عدم)
	החזר;
#  ifdef MSWIN
    אם (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	serverSetName(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    אם (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	socketserver_start(server, true);
#  endif
#  ifdef FEAT_X11
    אם (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    check_connection() == OK)
	serverRegisterName(X_DISPLAY, server);
#  endif

# אחרת
    emsg(_(e_clientserver_feature_not_available));
# endif
}

    فراغ
f_server2client(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*reply;
    صحيح		ret = OK;

    rettv->vval.v_number = -1;
    אם (check_restricted() || check_secure())
	החזר;

    אם (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	החזר;

    server = tv_get_string_chk(&argvars[0]);
    reply = tv_get_string_buf_chk(&argvars[1], buf);
    אם (server == عدم || reply == عدم)
	החזר;

#  ifdef FEAT_SOCKETSERVER
    אם (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
	    socketserver_send_reply(server, reply) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    אם (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    אם (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif

    אם (ret == FAIL)
    {
	emsg(_(e_unable_to_send_to_client));
	החזר;
    }
    rettv->vval.v_number = 0;
# אחרת
    rettv->vval.v_number = -1;
# endif
}

    فراغ
f_serverlist(typval_T *argvars UNUSED, typval_T *rettv)
{
    list_T  *list = عدم;
    bool    use_list = false;

    אם (check_for_opt_dict_arg(argvars, 0) == FAIL)
	החזר;

    אם (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    אם (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	list = serverGetVimNames();
#  endif
#  ifdef FEAT_SOCKETSERVER
    אם (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    אם (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	make_connection();
	אם (X_DISPLAY != عدم)
	    list = serverGetVimNames(X_DISPLAY);
    }
#  endif
# endif
    אם (use_list)
    {
	אם (list == عدم)
	{
	    (فراغ)rettv_list_alloc(rettv);
	    החזר;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    אחרת
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	אם (list != عدم)
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
    فراغ
check_clientserver_method_env(فراغ)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    אם (env == عدم)
	החזר;

    אם (STRICMP(env, "socket") == 0)
	clientserver_method = CLIENTSERVER_METHOD_SOCKET;
# ifdef FEAT_X11
    אחרת אם (STRICMP(env, "x11") == 0)
	clientserver_method = CLIENTSERVER_METHOD_X11;
# endif
# ifdef MSWIN
    אחרת אם (STRICMP(env, "mswin") == 0)
	clientserver_method = CLIENTSERVER_METHOD_MSWIN;
# endif
}
#endif
