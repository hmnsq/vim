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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_CLIENTSERVER)

ಸ್ಥಿರ ಶೂನ್ಯ cmdsrv_main(ಪೂರ್ಣಾಙ್ಕ *argc, ವರ್ಣ **argv, char_u *serverName_arg, char_u **serverStr);
ಸ್ಥಿರ char_u *serverMakeName(char_u *arg, ವರ್ಣ *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    ಶೂನ್ಯ
server_to_ಒಳಹರಿವು_buf(char_u *str)
{
    char_u      *ptr = ರಿಕ್ತ;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, ರಿಕ್ತ);
    p_cpo = cpo_save;

    ಯದಿ (ptr != ರಿಕ್ತ && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	ಯದಿ (typebuf.tb_maplen < typebuf.tb_len)
	    del_typebuf(typebuf.tb_len - typebuf.tb_maplen, typebuf.tb_maplen);
	(ಶೂನ್ಯ)ins_typebuf(str, REMAP_NONE, typebuf.tb_len, TRUE, FALSE);

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
eval_client_ಅಭಿವ್ಯಕ್ತಿ_to_string(char_u *ಅಭಿವ್ಯಕ್ತಿ)
{
    char_u	*res;
    ಪೂರ್ಣಾಙ್ಕ		save_dbl = debug_break_level;
    ಪೂರ್ಣಾಙ್ಕ		save_ro = redir_off;
    funccal_entry_T funccal_entry;
    ಪೂರ್ಣಾಙ್ಕ		did_save_funccal = FALSE;

# ಯದಿ defined(FEAT_EVAL)
    ch_log(ರಿಕ್ತ, "eval_client_expr_to_string(\"%s\")", ಅಭಿವ್ಯಕ್ತಿ);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    ಯದಿ (!debug_mode)
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

    res = eval_to_string(ಅಭಿವ್ಯಕ್ತಿ, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    ಯದಿ (emsg_silent < 0)
	emsg_silent = 0;
    ಯದಿ (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    out_flush_cursor(FALSE, FALSE);

    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    ಪೂರ್ಣಾಙ್ಕ
sendToLocalVim(char_u *cmd, ಪೂರ್ಣಾಙ್ಕ as_ಅಭಿವ್ಯಕ್ತಿ, char_u **result)
{
    ಯದಿ (as_ಅಭಿವ್ಯಕ್ತಿ)
    {
	char_u *ret;

	ret = eval_client_ಅಭಿವ್ಯಕ್ತಿ_to_string(cmd);
	ಯದಿ (result != ರಿಕ್ತ)
	{
	    ಯದಿ (ret == ರಿಕ್ತ)
	    {
		ವರ್ಣ	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*ಸಂದೇಶ;

		ಸಂದೇಶ = alloc(len);
		ಯದಿ (ಸಂದೇಶ != ರಿಕ್ತ)
		    vim_snprintf((ವರ್ಣ *)ಸಂದೇಶ, len, "%s: \"%s\"", err, cmd);
		*result = ಸಂದೇಶ;
	    }
	    ಅನ್ಯಥಾ
		*result = ret;
	}
	ಅನ್ಯಥಾ
	    vim_free(ret);
	ಪ್ರತ್ಯಾಗಮ ret == ರಿಕ್ತ ? -1 : 0;
    }
    server_to_ಒಳಹರಿವು_buf(cmd);
    ಪ್ರತ್ಯಾಗಮ 0;
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

    *tofree = ರಿಕ್ತ;
    ಯದಿ (client_enc == ರಿಕ್ತ || p_enc == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    ಯದಿ (convert_setup(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, data, ರಿಕ್ತ);
	ಯದಿ (res == ರಿಕ್ತ)
	    res = data;
	ಅನ್ಯಥಾ
	    *tofree = res;
    }
    convert_setup(&vimconv, ರಿಕ್ತ, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ res;
}
#endif

#ಯದಿ defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

ಸ್ಥಿರ char_u *build_drop_cmd(ಪೂರ್ಣಾಙ್ಕ filec, ವರ್ಣ **filev, ಪೂರ್ಣಾಙ್ಕ tabs, ಪೂರ್ಣಾಙ್ಕ sendReply);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    ಶೂನ್ಯ
exec_on_server(mparm_T *parmp)
{
    ಪೂರ್ಣಾಙ್ಕ made_name = FALSE;

    ಯದಿ (parmp->serverName_arg != ರಿಕ್ತ && *parmp->serverName_arg == NUL)
	ಪ್ರತ್ಯಾಗಮ;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    ಯದಿ (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    ಯದಿ (!made_name && parmp->servername == ರಿಕ್ತ)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    ಯದಿ (parmp->servername != ರಿಕ್ತ && clientserver_method == CLIENTSERVER_METHOD_MSWIN)
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
    ಶೂನ್ಯ
prepare_server(mparm_T *parmp)
{
# ಯದಿ defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    ಯದಿ (
#  ಯದಿ defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != ರಿಕ್ತ
#   endif
#   ಯದಿ defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   ಯದಿ defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_method == CLIENTSERVER_METHOD_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != ರಿಕ್ತ && (
#  ಯದಿ defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   ಯದಿ defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_method != CLIENTSERVER_METHOD_SOCKET &&
#   endif
#   ಯದಿ defined(FEAT_AUTOSERVERNAME)
		    1
#   ಅನ್ಯಥಾ
		    gui.in_use
#   endif
#   ifdef UNIX
		 && getuid() != ಬೇರು_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != ರಿಕ್ತ))
    {

#  ifdef FEAT_SOCKETSERVER
	ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	    socketserver_start(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11)
	{
	    (ಶೂನ್ಯ)serverRegisterName(X_DISPLAY, parmp->servername);
	    TIME_ಸಂದೇಶ("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    ಅನ್ಯಥಾ
    {
#  ifdef FEAT_X11
	ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    serverDelayedStartName = parmp->servername;
	ಅನ್ಯಥಾ
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    ಯದಿ (parmp->serverStr != ರಿಕ್ತ)
    {
	char_u *p;

	server_to_ಒಳಹರಿವು_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cmdsrv_main(
    ಪೂರ್ಣಾಙ್ಕ		*argc,
    ವರ್ಣ	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*sname;
    ಪೂರ್ಣಾಙ್ಕ		ret = -1;
    ಪೂರ್ಣಾಙ್ಕ		didone = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		exiterr = 0;
    ವರ್ಣ	**newArgV = argv + 1;
    ಪೂರ್ಣಾಙ್ಕ		newArgC = 1,
		Argc = *argc;
    ಪೂರ್ಣಾಙ್ಕ		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    ಪೂರ್ಣಾಙ್ಕ		silent = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		tabs = FALSE;
# ifdef MSWIN
    HWND	srv;
# elif defined(FEAT_X11)
    Window	srv;

    setup_term_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = ರಿಕ್ತ;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    ಯದಿ (sname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    ಪ್ರತಿ (i = 1; i < Argc; i++)
    {
	res = ರಿಕ್ತ;
	ಯದಿ (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    ಪ್ರತಿ (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    ಭಙ್ಗ;
	}

	ಯದಿ (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	ಅನ್ಯಥಾ ಯದಿ (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    ವರ್ಣ	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    ಯಾವತ್ (*p != NUL)
	    {
		ಯದಿ (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		ಅನ್ಯಥಾ
		{
		    argtype = ARGTYPE_OTHER;
		    ಭಙ್ಗ;
		}
	    }
	}
	ಅನ್ಯಥಾ
	    argtype = ARGTYPE_OTHER;

	ಯದಿ (argtype != ARGTYPE_OTHER)
	{
	    ಯದಿ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    ಯದಿ (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    ಅನ್ಯಥಾ
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		ಯದಿ (*serverStr == ರಿಕ್ತ)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    ಭಙ್ಗ;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		ret = socketserver_send(sname, *serverStr, ರಿಕ್ತ, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		ಯದಿ (xterm_dpy == ರಿಕ್ತ)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		ಅನ್ಯಥಾ
		    ret = serverSendToVim(xterm_dpy, sname, *serverStr,
			    ರಿಕ್ತ, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		ret = serverSendToVim(sname, *serverStr, ರಿಕ್ತ, &srv, 0, 0,
			silent);
# endif
	    ಯದಿ (ret < 0)
	    {
		ಯದಿ (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		ಅನ್ಯಥಾ ಯದಿ (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		ಭಙ್ಗ;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    ಯದಿ (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		SetForegroundWindow(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    ಯದಿ (argtype == ARGTYPE_EDIT_WAIT)
	    {
		ಪೂರ್ಣಾಙ್ಕ	ಅಂಕೆ_Files = *argc - i - 1;
		char_u  *done = alloc(ಅಂಕೆ_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		ಪೂರ್ಣಾಙ್ಕ	ಎಣಿಕೆ = 0;
		ಬಾಹ್ಯ HWND message_window;
# endif

		ಯದಿ (ಅಂಕೆ_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --ಅಂಕೆ_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cbSize = ಪರಿಮಾಣ(ni);
		ni.hWnd = message_window;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), ಎಣಿಕೆ, ಅಂಕೆ_Files);
		Shell_NotifyIcon(NIM_ADD, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(done, 0, ಅಂಕೆ_Files);
		ಯಾವತ್ (memchr(done, 0, ಅಂಕೆ_Files) != ರಿಕ್ತ)
		{
		    char_u  *p = ರಿಕ್ತ;
		    ಪೂರ್ಣಾಙ್ಕ	    j;
# ifdef MSWIN
		    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
			p = serverGetReply(srv, ರಿಕ್ತ, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET
			    && (ch == ರಿಕ್ತ
				|| socketserver_read_reply(sname, &p, -1, true)
				== FAIL))
			    ಭಙ್ಗ;
# endif
# ifdef FEAT_X11
		    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11
			    && serverReadReply(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    ಭಙ್ಗ;
# endif
		    ಯದಿ (p == ರಿಕ್ತ)
			ಭಙ್ಗ;
		    j = atoi((ವರ್ಣ *)p);
		    vim_free(p);
		    ಯದಿ (j >= 0 && j < ಅಂಕೆ_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++ಎಣಿಕೆ;
			sprintf(ni.szTip, _("%d of %d edited"),
							     ಎಣಿಕೆ, ಅಂಕೆ_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			done[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		ಯದಿ (ch != ರಿಕ್ತ)
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
	ಅನ್ಯಥಾ ಯದಿ (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ status = OK;

	    ಯದಿ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_MSWIN
		    && serverSendToVim(sname, (char_u *)argv[i + 1],
						  &res, ರಿಕ್ತ, 1, 0, FALSE) < 0)
		status = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, ರಿಕ್ತ) < 0)
		status = FAIL;
# endif
# ifdef FEAT_X11
	    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		ಯದಿ (xterm_dpy == ರಿಕ್ತ)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		ಅನ್ಯಥಾ ಯದಿ (serverSendToVim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    ರಿಕ್ತ, 1, 0, 1, FALSE) < 0)
		    status = FAIL;
	    }
# endif
	    ಯದಿ (status == FAIL)
	    {
		ಯದಿ (res != ರಿಕ್ತ && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((ವರ್ಣ *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = ರಿಕ್ತ;
	    garray_T	ga;

# ifdef MSWIN
	    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		list = serverGetVimNames();
# endif
# ifdef FEAT_SOCKETSERVER
	    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		    xterm_dpy != ರಿಕ್ತ)
		list = serverGetVimNames(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    ಯದಿ (list != ರಿಕ್ತ)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_data;

	    ಯದಿ (did_emsg)
		mch_errmsg("\n");
	}
	ಅನ್ಯಥಾ ಯದಿ (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    ಅನುವರ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    ಅನುವರ್ತ;
	}
	didone = TRUE;
	ಯದಿ (res != ರಿಕ್ತ && *res != NUL)
	{
	    mch_ಸಂದೇಶ((ವರ್ಣ *)res);
	    ಯದಿ (res[STRLEN(res) - 1] != '\n')
		mch_ಸಂದೇಶ("\n");
	}
	vim_free(res);
    }

    ಯದಿ (didone)
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
    ಸ್ಥಿರ char_u *
build_drop_cmd(
    ಪೂರ್ಣಾಙ್ಕ		filec,
    ವರ್ಣ	**filev,
    ಪೂರ್ಣಾಙ್ಕ		tabs,		// Use ":tab drop" instead of ":drop".
    ಪೂರ್ಣಾಙ್ಕ		sendReply)
{
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*inicmd = ರಿಕ್ತ;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    ಧ್ರುವ string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    ಯದಿ (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    ಯದಿ (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    ಯದಿ (cwd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_IN_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# ಅನ್ಯಥಾ
	    PATH_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    ಯದಿ (cdp.string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    cdp.length = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.length);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].length);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    ಯದಿ (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    ಪ್ರತಿ (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		PATH_ESC_CHARS
# ಅನ್ಯಥಾ
		(char_u *)" \t%#"
# endif
		);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_data);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
# ಅನ್ಯಥಾ
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.length);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].length);

    ಯದಿ (sendReply)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    ಯದಿ (inicmd != ರಿಕ್ತ)
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
    ಪ್ರತ್ಯಾಗಮ ga.ga_data;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    ಸ್ಥಿರ char_u *
serverMakeName(char_u *arg, ವರ್ಣ *cmd)
{
    char_u *p;

    ಯದಿ (arg != ರಿಕ್ತ && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	{
	    ಯದಿ (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    ಅನ್ಯಥಾ
		p = vim_strsave_up(arg);
	}
	ಅನ್ಯಥಾ
	    p = vim_strsave_up(arg);
# ಅನ್ಯಥಾ
	p = vim_strsave_up(arg);
# endif
    }
    ಅನ್ಯಥಾ
    {
	p = vim_strsave_up(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	ಯದಿ (p != ರಿಕ್ತ && vim_strchr(p, '.') != ರಿಕ್ತ)
	    *vim_strchr(p, '.') = NUL;
    }
    ಪ್ರತ್ಯಾಗಮ p;
}
#endif // FEAT_CLIENTSERVER

#ಯದಿ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    ಸ್ಥಿರ ಶೂನ್ಯ
make_connection(ಶೂನ್ಯ)
{
    ಯದಿ (X_DISPLAY == ರಿಕ್ತ
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

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_connection(ಶೂನ್ಯ)
{
    ಯದಿ (clientserver_method != CLIENTSERVER_METHOD_X11)
	ಪ್ರತ್ಯಾಗಮ OK;
    make_connection();
    ಯದಿ (X_DISPLAY == ರಿಕ್ತ)
    {
	emsg(_(e_no_connection_to_x_server));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

#ifdef FEAT_CLIENTSERVER
    ಸ್ಥಿರ ಶೂನ್ಯ
remote_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ ಅಭಿವ್ಯಕ್ತಿ)
{
    char_u	*server_name;
    char_u	*ಕೀಲಿಗಳು;
    char_u	*r = ರಿಕ್ತ;
    char_u	buf[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		timeout = 0;
# ifdef MSWIN
    HWND	w;
# ಅನ್ಯಥಾ
#  ifdef FEAT_X11
    Window	w;
#  endif
# endif
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

# ifdef FEAT_X11
    ಯದಿ (check_connection() == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
# endif
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_ಸಂಖ್ಯೆ(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    ಯದಿ (server_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;		// type error; errmsg already given
    ಕೀಲಿಗಳು = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendToVim(server_name, ಕೀಲಿಗಳು, &r, &w, ಅಭಿವ್ಯಕ್ತಿ,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET
	&& socketserver_send(server_name, ಕೀಲಿಗಳು, &r, ಅಭಿವ್ಯಕ್ತಿ,
	    timeout * 1000, true, ರಿಕ್ತ) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11)
	ಯದಿ (serverSendToVim(X_DISPLAY, server_name, ಕೀಲಿಗಳು, &r, &w, ಅಭಿವ್ಯಕ್ತಿ, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    ಯದಿ (ret == FAIL)
    {
	ಯದಿ (r != ರಿಕ್ತ)
	{
	    emsg((ವರ್ಣ *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	ಅನ್ಯಥಾ
	    semsg(_(e_unable_to_send_to_str), server_name);
	ಪ್ರತ್ಯಾಗಮ;
    }

    rettv->vval.v_string = r;

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# ಯದಿ defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = ರಿಕ್ತ;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	ಯದಿ (idvar != ರಿಕ್ತ && *idvar != NUL)
	{
# ifdef MSWIN
	    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	    {
		sprintf((ವರ್ಣ *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		sprintf((ವರ್ಣ *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		str = server_name;
# endif
	    ಯದಿ (str == ರಿಕ್ತ)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    set_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    ಶೂನ್ಯ
f_remote_ಅಭಿವ್ಯಕ್ತಿ(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

# ifdef FEAT_CLIENTSERVER
    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    ಶೂನ್ಯ
f_remote_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	ಯದಿ (server_name != ರಿಕ್ತ)
	    serverForeground(server_name);
    }
#  ಅನ್ಯಥಾ
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    ಶೂನ್ಯ
f_remote_peek(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = ರಿಕ್ತ;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    serverid = tv_get_string_chk(&argvars[0]);
    ಯದಿ (serverid == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;		// type error; errmsg already given
#  ifdef MSWIN
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	sscanf((ಧ್ರುವ ವರ್ಣ *)serverid, SCANF_HEX_LONG_U, &n);
	ಯದಿ (n == 0)
	    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
	ಅನ್ಯಥಾ
	{
	    s = serverGetReply((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_ಸಂಖ್ಯೆ = (s != ರಿಕ್ತ);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	rettv->vval.v_ಸಂಖ್ಯೆ = socketserver_peek_reply(serverid, &s);
#  endif
#  ifdef FEAT_X11
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	ಯದಿ (check_connection() == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;

	rettv->vval.v_ಸಂಖ್ಯೆ = serverPeekReply(X_DISPLAY,
		serverStrToWin(serverid), &s);
    }
#  endif

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_ಸಂಖ್ಯೆ > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	ಯದಿ (retvar != ರಿಕ್ತ)
	    set_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# ಅನ್ಯಥಾ
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
# endif
}

    ಶೂನ್ಯ
f_remote_read(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*r = ರಿಕ್ತ;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    serverid = tv_get_string_chk(&argvars[0]);
    ಯದಿ (serverid != ರಿಕ್ತ && !check_restricted() && !check_secure())
    {
	ಪೂರ್ಣಾಙ್ಕ timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);

#  ifdef MSWIN
	ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	{
	    sscanf((ವರ್ಣ *)serverid, SCANF_HEX_LONG_U, &n);
	    ಯದಿ (n != 0)
		r = serverGetReply((HWND)n, FALSE, TRUE, TRUE, timeout);
	    ಯದಿ (r == ರಿಕ್ತ)
		emsg(_(e_unable_to_read_server_reply));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
		socketserver_read_reply(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_to_read_server_reply));
#  endif
#  ifdef FEAT_X11
	ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
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
    ಶೂನ್ಯ
f_remote_send(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

# ifdef FEAT_CLIENTSERVER
    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    ಶೂನ್ಯ
f_remote_startserver(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    ಯದಿ (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (serverName != ರಿಕ್ತ)
    {
	emsg(_(e_already_started_server));
	ಪ್ರತ್ಯಾಗಮ;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    ಯದಿ (server == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
#  ifdef MSWIN
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	serverSetName(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	socketserver_start(server, true);
#  endif
#  ifdef FEAT_X11
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    check_connection() == OK)
	serverRegisterName(X_DISPLAY, server);
#  endif

# ಅನ್ಯಥಾ
    emsg(_(e_clientserver_feature_not_available));
# endif
}

    ಶೂನ್ಯ
f_server2client(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*reply;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    server = tv_get_string_chk(&argvars[0]);
    reply = tv_get_string_buf_chk(&argvars[1], buf);
    ಯದಿ (server == ರಿಕ್ತ || reply == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

#  ifdef FEAT_SOCKETSERVER
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
	    socketserver_send_reply(server, reply) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif

    ಯದಿ (ret == FAIL)
    {
	emsg(_(e_unable_to_send_to_client));
	ಪ್ರತ್ಯಾಗಮ;
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
# ಅನ್ಯಥಾ
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
# endif
}

    ಶೂನ್ಯ
f_serverlist(typval_T *argvars UNUSED, typval_T *rettv)
{
    list_T  *list = ರಿಕ್ತ;
    bool    use_list = false;

    ಯದಿ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	list = serverGetVimNames();
#  endif
#  ifdef FEAT_SOCKETSERVER
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	make_connection();
	ಯದಿ (X_DISPLAY != ರಿಕ್ತ)
	    list = serverGetVimNames(X_DISPLAY);
    }
#  endif
# endif
    ಯದಿ (use_list)
    {
	ಯದಿ (list == ರಿಕ್ತ)
	{
	    (ಶೂನ್ಯ)rettv_list_alloc(rettv);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    ಅನ್ಯಥಾ
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	ಯದಿ (list != ರಿಕ್ತ)
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
    ಶೂನ್ಯ
check_clientserver_method_env(ಶೂನ್ಯ)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    ಯದಿ (env == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (STRICMP(env, "socket") == 0)
	clientserver_method = CLIENTSERVER_METHOD_SOCKET;
# ifdef FEAT_X11
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(env, "x11") == 0)
	clientserver_method = CLIENTSERVER_METHOD_X11;
# endif
# ifdef MSWIN
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(env, "mswin") == 0)
	clientserver_method = CLIENTSERVER_METHOD_MSWIN;
# endif
}
#endif
